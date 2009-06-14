#ifndef _disk_h
#define _disk_h

#include <common.h>

typedef struct hd_capacity_struct 
{
	uint32 cylinders;
	uint32 heads;
	uint32 sectors;  // per track
	int drive;  // doom to 0 at this time
} hd_capacity_t;

typedef struct hd_chs_struct
{
	uint32 cylinder;
	uint32 head;
	uint32 sect;
} hd_chs_t;

// cylinder # start from 0
#define cylinder_from_lba(_lba, _cap)  ({				\
			(_lba) / ((_cap)->heads*(_cap)->sectors);	\
		})

// head # start from 0
#define head_from_lba(_lba, _cap) ({									\
			uint32 __rem = (_lba) % ((_cap)->heads*(_cap)->sectors);	\
			__rem / (_cap)->sectors;									\
		})

// sector number start from 1
#define sector_from_lba(_lba, _cap) ({									\
			uint32 __rem = (_lba) % ((_cap)->heads*(_cap)->sectors);	\
			__rem % (_cap)->heads + 1;									\
		})


// just copy from skelix
#define HD_PORT_DATA         0x1f0
#define HD_PORT_ERROR        0x1f1
#define HD_PORT_SECT_COUNT   0x1f2
#define HD_PORT_SECT_NUM     0x1f3
#define HD_PORT_CYL_LOW      0x1f4
#define HD_PORT_CYL_HIGH     0x1f5
#define HD_PORT_DRV_HEAD     0x1f6
#define HD_PORT_STATUS       0x1f7
#define HD_PORT_COMMAND      0x1f7

// commands
enum HD_CMD_ENUM {
	READ_WITH_RETRY = 0x20,
	WRITE_WITH_RETRY = 0x30
};

// status for HD_PORT_STATUS
#define HD_STATUS_ERROR               0x01
#define HD_STATUS_INDEX               0x02 
#define HD_STATUS_READ_CORRECT        0x04
#define HD_STATUS_DRQ                 0x08
#define HD_STATUS_SEEK_COMPLETE       0x10
#define HD_STATUS_WRITE_FAULT         0x20
#define HD_STATUS_DRIVE_READY         0x40 
#define HD_STATUS_BUSY                0x80

// return errno
extern int disk_read(const hd_capacity_t *disk, uint32 logic_sector, unsigned char *buf);
extern int disk_write(const hd_capacity_t *disk, uint32 logic_sector, unsigned char *buf);

typedef struct partition_struct 
{
	byte used;  // set if this is a valid partition
	byte boot;
	hd_chs_t chs;
	byte part_type;
	uint32 offset;  
	uint32 sectors;
} partition_t;
	
typedef struct dpt_struct
{
	byte valid;  
	partition_t partitions[4];
} dpt_t;
	
extern int read_dpt(const hd_capacity_t *disk);
extern void setup_dpt(const hd_capacity_t *disk);

#define YOS_FS 0x2e
#endif
