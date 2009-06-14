#include "disk.h"
#include <io.h>
#include <timer.h>

#ifdef _YOS_HD_DEBUG
#define hd_debug(...) early_kprint( PL_DEBUG, __VA_ARGS__ )
#else
#define hd_debug(...) 
#endif												
	
// bad implementation, without quite important error-checking,
// may not work in real-world hard drive
int disk_read(const hd_capacity_t *disk, uint32 logic_sector, unsigned char *buf)
{
	if ( buf == NULL )
		return -1;

	struct hd_chs_struct chs = { 0, 0, 0 };
	chs.cylinder = cylinder_from_lba(logic_sector, disk);
	chs.head = head_from_lba(logic_sector, disk);
	chs.sect = sector_from_lba(logic_sector, disk);
	hd_debug( "chs(c %d, h %d, s %d)\n", chs.cylinder, chs.head, chs.sect );
	
	byte status = inb( HD_PORT_STATUS );
	hd_debug( "drive status: %x\n", status );
	while ( !(status & HD_STATUS_DRIVE_READY) ) {
		hd_debug( "drive is not ready\n" );
		delay(1);
		status = inb( HD_PORT_STATUS );
		hd_debug( "drive status: %x\n", status );		
	}

	while ( (status & HD_STATUS_BUSY) ) {
		delay(1);
		hd_debug( "drive is busy\n" );
	}
	
	// select drive 0 / head
	outb( 0b10100000 | (chs.head & 0x0f), HD_PORT_DRV_HEAD );
	
	outb( 1, HD_PORT_SECT_COUNT );
	outb( chs.sect, HD_PORT_SECT_NUM );
	outb( chs.cylinder, HD_PORT_CYL_LOW );
	outb( chs.cylinder >> 8, HD_PORT_CYL_HIGH );

	outb( READ_WITH_RETRY, HD_PORT_COMMAND );

	// cause we choose drive above, which requires a little time
	inb( HD_PORT_STATUS );
	inb( HD_PORT_STATUS );
	inb( HD_PORT_STATUS );
	inb( HD_PORT_STATUS );
	status = inb( HD_PORT_STATUS );
	if ( status & HD_STATUS_ERROR ) {
		early_kprint( PL_ERROR, "read error\n" );
		return -HD_STATUS_ERROR;
	}

	if ( status & HD_STATUS_DRQ ) {
		__asm__ __volatile__ (
			"pushl %%es       \n\t"
			"movl %%ds, %%eax \n\t"
			"movl %%eax, %%es \n\t"
			"cld              \n\t"
			"rep insl         \n\t"
			"popl %%es        \n\t"
			::"d"(HD_PORT_DATA), "D"(buf), "c"(512>>2)
			: "eax"
			);
	} else {
		hd_debug( "send data failed\n" );
	}

	hd_debug( "read mbr flag: %x\n", *(unsigned*)&buf[0x1fe] );

	delay(1);
	byte remains = inb( HD_PORT_SECT_COUNT );
	if ( remains > 0 ) {
		hd_debug( "sector counter is %d\n", remains );
	}

	status = inb( HD_PORT_STATUS );
	while ( status & HD_STATUS_DRQ ) {
		hd_debug( "wait for DRQ clear\n" );
	}

	return 0;
}

static const char* partTypeStr(byte type)
{
	const char* types[] = {
		"Empty", 
		"DOS 12 bit FAT",
		"XENIX root file system",
		"XENIX /usr file system",
		"DOS 16 bit FAT",
		"Extended DOS partition",
		"DOS 16 bit FAT",
		"OS/2 High Performance",
		"Windows NT (NTFS)",
		"Advanced Unix",
		"OS/2 Boot Manager"
	};
	if ( type >= sizeof(types)/sizeof(types[0]) ) {
		if ( type == YOS_FS )
			return "YOS FS";
		else
			return "Unknown";
	} else
		return types[type];
}

// parse a partition for buf of 16 bytes
static void parse_partition( unsigned char *buf )
{
	partition_t part = {
		.used = 0
	};

	part.boot = buf[0] == 0 ? 0 : ( buf[0] == 0x80 ? 1: 0 );
	part.offset = *(uint32*)(buf + 0x08);
	part.sectors = *(uint32*)(buf + 0x0c);
	part.part_type = buf[0x04];

	int size = part.sectors * 512;
	int kilo = size / 1024;
	int mega = kilo / 1024;
	int giga = mega / 1024;
	const char *str_size = (giga? "G": ( mega? "M" : (kilo? "K" : "B")));
	size = (giga? giga: ( mega? mega : (kilo? kilo : size)));	
	hd_debug( "0x%x\t%s\t\t\t0x%x\t%d%s\n",
			  part.boot, partTypeStr(part.part_type),
			  part.offset, size, str_size );
}

void setup_dpt(const hd_capacity_t *disk)
{
	early_kprint( PL_DEBUG, "setting up default DPT\n" );
	
	unsigned char mbr[512] = "Y.O.S.";

	mbr[0x1be] = 0x80;
	mbr[0x1be + 0x04] = YOS_FS;
    *(unsigned long *)(mbr + 0x1be + 0x08) = 1;
    *(unsigned long *)(mbr + 0x1be + 0x0c) = 85*1024*2; /* 85MB */
	mbr[0x1ce + 0x04] = 0x07; //NTFS
    *(unsigned long *)(mbr + 0x1ce + 0x08) = 85*1024*2+1;
    *(unsigned long *)(mbr + 0x1ce + 0x0c) = 16*1024*2; /* 16MB */
	*(unsigned short *)(mbr + 0x1fe) = 0xaa55;
	disk_write( disk, 0, mbr );
}

int read_dpt(const hd_capacity_t *disk)
{
	dpt_t dpt = {
		.valid = 0
	};

	unsigned char sect_buf[512];
	if ( disk_read(disk, 0, sect_buf) < 0 ) {
		early_kprint( PL_ERROR, "read mbr failed\n" );
		return -1;
	}

	if ( sect_buf[0x1fe] != 0x55 || sect_buf[0x1ff] != 0xaa ) {
		hd_debug( "partition table is invalid\n" );
		return -1;
	}

	early_kprint( PL_DEBUG, "Partition Table:\n" );
	hd_debug( "#\tBoot\tType\t\t\tOffset\tSize\n" );
	
	int offsets[4] = {
		0x1be, 0x1ce, 0x1de, 0x1ee
	};

	int i = 0;
	for ( i = 0; i < 4; i++ ) {
		early_kprint( PL_DEBUG, "%d\t", i );
		parse_partition(sect_buf + offsets[i]);		
	}

	return 0;
}

int disk_write(const hd_capacity_t *disk, uint32 logic_sector, unsigned char *buf)
{
	//TODO: check if logic_sector is inside disk
	if ( buf == NULL )
		return -1;
	
	struct hd_chs_struct chs = { 0, 0, 0 };
	chs.cylinder = cylinder_from_lba(logic_sector, disk);
	chs.head = head_from_lba(logic_sector, disk);
	chs.sect = sector_from_lba(logic_sector, disk);
	hd_debug( "chs(c %d, h %d, s %d)\n", chs.cylinder, chs.head, chs.sect );

	byte status = inb( HD_PORT_STATUS );
	hd_debug( "drive status: %x\n", status );
	while ( !(status & HD_STATUS_DRIVE_READY) ) {
		hd_debug( "drive is not ready\n" );
		delay(1);
		status = inb( HD_PORT_STATUS );		
	}

	while ( (status & HD_STATUS_BUSY) ) {
		delay(1);
		early_kprint( PL_ERROR, "drive is busy\n" );
	}
	
	// select drive 0 / head
	outb( 0b10100000 | (chs.head & 0x0f), HD_PORT_DRV_HEAD );
	
	outb( 1, HD_PORT_SECT_COUNT );
	outb( chs.sect, HD_PORT_SECT_NUM );
	outb( chs.cylinder, HD_PORT_CYL_LOW );
	outb( chs.cylinder >> 8, HD_PORT_CYL_HIGH );

	outb( WRITE_WITH_RETRY, HD_PORT_COMMAND );

	// cause we choose drive above, which requires a little time
	inb( HD_PORT_STATUS );
	inb( HD_PORT_STATUS );
	inb( HD_PORT_STATUS );
	inb( HD_PORT_STATUS );
	status = inb( HD_PORT_STATUS );
	if ( status & HD_STATUS_ERROR ) {
		early_kprint( PL_ERROR, "read error\n" );
		return -HD_STATUS_ERROR;
	} 

	if ( status & HD_STATUS_DRQ ) {
		__asm__ __volatile__ (
			"cld				\n\t"
			"rep outsl			\n\t"
			::"d"(HD_PORT_DATA), "S"(buf), "c"(512>>2)
			);
	} else {
		hd_debug( "write data failed\n" );
	}
	
	byte remains = inb( HD_PORT_SECT_COUNT );
	if ( remains > 0 ) {
		hd_debug( "sector counter is %d\n", remains );
	}

	status = inb( HD_PORT_STATUS );
	hd_debug( "after write status: %x\n", status );
	
	if ( status & HD_STATUS_DRQ ) {
		hd_debug( "wait for DRQ clear\n" );
	}
	
	return 0;
}

