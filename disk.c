#include "disk.h"
#include <io.h>
#include <timer.h>

// a disk with capacity about 102MB
struct hd_capacity_struct tmp_disk = {
	.cylinders = 208,
	.heads = 16,
	.sectors = 63,
	.drive = 0
};
	
uint32 chs_to_lba(hd_chs_t chs, hd_capacity_t disk)
{
	return 0;
}

hd_chs_t lba_to_chs(uint32 lba, hd_capacity_t disk)
{
	struct hd_chs_struct chs = { 0, 0, 0 };
	chs.cylinder = cylinder_from_lba(lba, disk);
	chs.head = head_from_lba(lba, disk);
	chs.sect = sector_from_lba(lba, disk);
	return chs;
}

// bad implementation, without quite important error-checking,
// may not work in real-world hard drive
int disk_read(hd_capacity_t disk, uint32 logic_sector, unsigned char *buf)
{
	if ( buf == NULL )
		return -1;
	
	hd_chs_t chs = lba_to_chs(logic_sector, disk);
	
	byte status = inb( HD_PORT_STATUS );
	if ( !(status & HD_STATUS_DRIVE_READY) ) {
		early_kprint( PL_ERROR, "drive is not ready\n" );
		return -HD_STATUS_DRIVE_READY;
	}

	while ( (status & HD_STATUS_BUSY) ) {
		delay(1);
		early_kprint( PL_ERROR, "drive is busy\n" );
	}
	
	// select drive 0 / head
	outb( 0b00000101 | (chs.head & 0xf0), HD_PORT_DRV_HEAD );

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
			"pushl %%es   \n\t"
			"cld          \n\t"
			"rep insl     \n\t"
			"popl %%es    \n\t"
			::"d"(HD_PORT_DATA), "D"(buf), "c"(512>>2)
			);
	}

	byte remains = inb( HD_PORT_SECT_NUM );
	if ( remains > 0 ) {
		early_kprint( PL_DEBUG, "sector counter is %d\n", remains );
	}

	status = inb( HD_PORT_STATUS );
	while ( status & HD_STATUS_DRQ ) {
		early_kprint( PL_DEBUG, "wait for DRQ clear\n" );
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
		"OS/2 High Performance File System",
		"Windows NT (NTFS)",
		"Advanced Unix",
		"OS/2 Boot Manager"
	};
	if ( type >= sizeof(types)/sizeof(types[0]) )
		return "Unknown";
	else
		return types[type];
}

// parse a partition for buf of 16 bytes
static partition_t parse_partition( unsigned char *buf )
{
	partition_t part = {
		.used = 0
	};

	part.boot = buf[0] == 0 ? 0 : ( buf[0] == 0x80 ? 1: 0 );
	part.offset = *(uint32*)(buf + 0x08);
	part.sectors = *(uint32*)(buf + 0x0c);
	part.part_type = buf[0x04];
	
#ifdef _YOS_HD_DEBUG
	early_kprint( PL_DEBUG, "0x%x\t%s\t\t0x%x\t0x%x",
				  part.boot, partTypeStr(part.part_type),
				  part.offset, part.part_type );

#endif
	return part;
}

dpt_t read_dpt(hd_capacity_t disk)
{
	dpt_t dpt = {
		.valid = 0
	};
	
	unsigned char sect_buf[512];
	if ( disk_read(disk, 0, sect_buf) < 0 ) {
		early_kprint( PL_ERROR, "read mbr failed\n" );
		return dpt;
	}

	if ( sect_buf[0x1fe] != 0x55 || sect_buf[0x1ff] != 0xaa ) {
#ifdef _YOS_HD_DEBUG		
		early_kprint( PL_DEBUG, "partition table is invalid\n" );
#endif		
		return dpt;
	}
	
	int offsets[4] = {
		0x1be, 0x1ce, 0x1de, 0x1ee
	};

#ifdef _YOS_HD_DEBUG	
	early_kprint( PL_DEBUG, "#\tBoot\tType\t\tOffset\t# of sects\n" );
#endif
	
	int i = 0;
	for ( i = 0; i < 4; i++ ) {
#ifdef _YOS_HD_DEBUG	
		early_kprint( PL_DEBUG, "%d\t", i );
#endif
		dpt.partitions[i] = parse_partition(sect_buf + offsets[i]);
	}
	
	return dpt;
}

int disk_write(hd_capacity_t disk, uint32 logic_sector, unsigned char *buf)
{
	return 0;
}

