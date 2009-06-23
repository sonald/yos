#ifndef _YOS_FS_H
#define _YOS_FS_H

#include <common.h>
#include <disk.h>

#define MAX_FILENAME_SIZE 12

enum INODE_TYPE {
	FT_FILE         = 0x1<<24,
	FT_DIRECTORY    = 0x2<<24,
	FT_LINK         = 0x3<<24,
	FT_SOCK         = 0x4<<24,
	FT_MAX          = 0xf<<24
};
	
// this struct compat in 2^^3, so a sector can have 512 / 8*4 = 16 inodes
typedef struct yfs_inode_struct {
	int i_mode;
	int i_size;         // real size in bytes, size in blk can be calculated 
	int i_timestamp;
	uint32 i_block[5];  // block[3] is once-indirect block
						// block[4] is twice-indirect block
						// the rest is inplace data blocks
} yfs_inode_t;

// 16 bytes now
typedef struct dir_entry_struct {
	char d_name[MAX_FILENAME_SIZE];
	int d_ino;
} dir_entry_t;

/**
 * disk map
 * bootsect | sb                     |  maps               |    data
 *          | magic,offset,size,...  |  bmap, imap, inodes |
 * superblock occupies one sector
 */
typedef struct yfs_superblock_struct 
{
	uint32 sb_magic;			/* "YOS1"->0x31534f59 */
	uint32 sb_offset;			/* start sector */
	uint32 sb_size;				/* size of sectors */
	uint16 sb_bmap_blks;        /* blks for block usage map */
	uint32 sb_imap_blks;        /* blks for inode usage map */ 
	uint32 sb_inode_blks;       /* blks used for inodes */
} yfs_superblock_t;

// yfs type id used in mbr
#define YOS_FS      0x2e
// yfs magic number in superblock
#define YFS_MAGIC   0x31534f59
/**
 * yfs use one block for inode maps, so we can have maximum of
 * BLK_SIZE_IN_SECT * SECT_SIZE * 8 inode in a partition, which is sufficient
 * for the time being
 */
#define YFS_INODE_BITMAP_BLKS 1

// 2k block ( 4 sectors )
#define BITS_PER_SECT     (SECT_SIZE<<3)
#define BLK_SIZE_IN_SECT  4
#define BLK_SIZE          (SECT_SIZE*BLK_SIZE_IN_SECT)

#define BITS_PER_BLK      (BLK_SIZE<<3)
#define INODES_PER_SECT   (SECT_SIZE/sizeof(struct yfs_inode_struct))
#define INODES_PER_BLK    (INODES_PER_SECT * BLK_SIZE_IN_SECT)

#define DIRS_PER_SECT     (SECT_SIZE/sizeof(dir_entry_t))
#define DIRS_PER_BLK      (DIRS_PER_BLK * BLK_SIZE_IN_SECT)

#define YFS_ROOT_INODE_NUM 0

// make it easier to calculate the offset of every section in a yfs file system
#define ABS_BOOTSECT_SECT(sb)    ((sb).sb_offset)
#define ABS_SBLOCK_SECT(sb)      (ABS_BOOTSECT_SECT(sb)+1)
#define ABS_BMAP_SECT(sb)        (ABS_SBLOCK_SECT(sb)+1)
#define ABS_IMAP_SECT(sb)        (ABS_BMAP_SECT(sb)+(sb).sb_bmap_blks)
#define ABS_INODE_SECT(sb)       (ABS_IMAP_SECT(sb)+(sb).sb_imap_blks)
#define ABS_DATA_SECT(sb)        (ABS_INODE_SECT(sb)+(sb).sb_inode_blks)

/**
 * init yfs in partition part_num in disk
 */
extern int init_yfs(const disk_t *disk, const partition_t *part);
/**
 * check if there is a valid yfs in disk if one partition marked as yfs
 */
extern int check_yfs(const disk_t *disk);

/**
 * alloc a free block, and return LBA start address of it
 * if alloc failed, 0 return, >0 means success
 */
extern uint32 yfs_alloc_block(const disk_t *disk, yfs_superblock_t *sb);

/**
 * @param sector is LBA start address of block
 */
extern void yfs_free_block(const disk_t *disk, yfs_superblock_t *sb,
						   uint32 sector);

extern void check_root(const disk_t *disk, yfs_superblock_t *sb);
extern void yfs_stat(const disk_t *disk, yfs_superblock_t *sb, int ino);

#endif
