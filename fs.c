#include <fs.h>
#include <io.h>

#ifdef _YOS_FS_DEBUG
#define yfs_debug(fmt, ...) early_kprint( PL_DEBUG, "YFS: "fmt, ##__VA_ARGS__ )
#else
#define yfs_debug(...) 
#endif												

static yfs_inode_t yfs_root_inode = {
	.i_mode = FT_DIRECTORY,
	.i_size = BLK_SIZE_IN_SECT,
	.i_timestamp = 0,
	.i_block = {0, }
	
};

static void setb(void *map, uint32 bit)
{
	unsigned char *m = map;
	m[ bit/8 ] |= (1<<(bit%8));
}

static void clrb(void *map, uint32 bit)
{
	unsigned char *m = map;
	m[ bit/8 ] &= ~(1<<(bit%8));
}

/*
 * retrun: >0 if bit is set, =0 if unset
 */
static int testb(void *map, uint32 bit)
{
	unsigned char *m = map;
	return (m[bit/8] & (1<<(bit%8))) != 0;
}

int init_yfs(const disk_t *disk, const partition_t *part)
{
	unsigned char sect[SECT_SIZE];
	memset( sect, 0, sizeof(sect) );
	yfs_superblock_t *psb = (yfs_superblock_t*)&sect[0];
	psb->sb_magic = YFS_MAGIC;
	psb->sb_offset = part->offset;
	psb->sb_size = part->sectors;

	int num_blocks = psb->sb_size / BLK_SIZE_IN_SECT;

	psb->sb_bmap_blks = (num_blocks + BITS_PER_BLK - 1) / BITS_PER_BLK;
	psb->sb_imap_blks = YFS_INODE_BITMAP_BLKS;
	int num_inodes = YFS_INODE_BITMAP_BLKS * BITS_PER_BLK;
	psb->sb_inode_blks = (num_inodes + INODES_PER_BLK - 1) / INODES_PER_BLK;

	if ( disk_write(disk, ABS_SBLOCK_SECT(*psb), sect) < 0 ) {
		yfs_debug( "write superblock failed.\n" );
		return -1;
	}

	// initialize bmap
	// all blocks occupied by superblock, bmap, imap and inodes array
	// should be marked as used here

	int bmap_offset = ABS_BMAP_SECT(*psb);
	int bmap_sects = psb->sb_bmap_blks * BLK_SIZE_IN_SECT;
	int meta_data_blks = 1/*bootsect*/ + 1 /*sb*/ + psb->sb_bmap_blks
		+ psb->sb_imap_blks + psb->sb_inode_blks;
	
	int idx = bmap_offset;
	memset( sect, 0xff, sizeof sect );
	int i = 0;
	for ( i = (meta_data_blks/(SECT_SIZE<<3)); i > 0; i-- ) {
		disk_write( disk, idx, sect );
		idx++;
	}

	for ( i = (meta_data_blks%(SECT_SIZE<<3)); i < (SECT_SIZE<<3); i++ )
		clrb( sect, i );
	disk_write( disk, idx++, sect );
	
	memset( sect, 0, sizeof sect );	
	for ( ; idx < bmap_offset + bmap_sects; idx++ ) {
		disk_write(disk, idx, sect);
	}

	// initialize imap
	idx = ABS_IMAP_SECT(*psb);
	for ( ; idx < psb->sb_imap_blks * BLK_SIZE_IN_SECT; idx++ )
		disk_write( disk, idx, sect );

	// initialize inode array
	idx = ABS_INODE_SECT(*psb);
	for ( ; idx < psb->sb_inode_blks * BLK_SIZE_IN_SECT; idx++ )
		disk_write( disk, idx, sect );

	yfs_debug( "initialization of YFS finished.\n" );
	
	return 0;
}

static int check_yfs_for(const disk_t *disk, const partition_t *part)
{
	unsigned char sect[512];
	yfs_superblock_t sb;
	sb.sb_magic = YFS_MAGIC;
	sb.sb_offset = part->offset;
	
	if ( disk_read( disk, ABS_SBLOCK_SECT(sb), sect ) < 0 ) {
		yfs_debug( "read superblock failed.\n" );
		return -1;
	}

	// dump yfs information
	yfs_superblock_t *psb = (yfs_superblock_t*)&sect[0];
	if ( psb->sb_magic == YFS_MAGIC ) {
		early_kprint( PL_INFO, "found invalid YFS partition\n" );
		yfs_debug( "dump superblock:\n" );
		yfs_debug( "offset: 0x%x, size(sects): 0x%x ",
				   psb->sb_offset, psb->sb_size );
		yfs_debug( "bmap 0x%x, imap 0x%x, inodes 0x%x\n",
				   psb->sb_bmap_blks, psb->sb_imap_blks, psb->sb_inode_blks );
		
		return 0;
	} else {
		yfs_debug( "invalid yfs partition, initialize it\n" );
		return init_yfs( disk, part );
	}
		
	return -1;
}

int check_yfs(const disk_t *disk)
{
	if ( !disk || !disk->disk_dpt.valid )
		return -1;

	const dpt_t *dpt = &disk->disk_dpt;
	int i = 0;
	
	for ( i = 0; i < 4; i++ ) {
		if ( dpt->partitions[i].used
			 && dpt->partitions[i].part_type == YOS_FS ) {
			yfs_debug( "check partition %d\n", i );			
			check_yfs_for( disk, &dpt->partitions[i] );
		}
	}

	return 0;
}

uint32 yfs_alloc_block(const disk_t *disk, yfs_superblock_t *sb)
{
	if ( !disk || !sb )
		return 0;

	unsigned char sect[SECT_SIZE];
	int bmap_sects = sb->sb_bmap_blks * BLK_SIZE_IN_SECT;

	int idx = ABS_BMAP_SECT(*sb), n = 0, b = 0;
	for( ; idx < bmap_sects; idx++ ) {
		disk_read( disk, idx, sect );
		for ( b = 0; b < (SECT_SIZE<<3); b++ ) {
			if ( testb(sect, b) ) {
				n++;
				continue;
			} 

			if ( n >= (sb->sb_size / BLK_SIZE_IN_SECT) )
				return 0;
			
			setb(sect, b);
			disk_write( disk, idx, sect );
			return ABS_BOOTSECT_SECT(*sb) + n;
		}
	}

	return 0;
}

void yfs_free_block(const disk_t *disk, yfs_superblock_t *sb, uint32 sector)
{
	if ( !disk || !sb )
		return 0;

	unsigned char sect[SECT_SIZE];

	int block_num = (sector - sb->sb_offset) / BLK_SIZE_IN_SECT;
	int offset = (sector/BLK_SIZE_IN_SECT) / (SECT_SIZE<<3);
	int offset_by_bmap = ABS_BMAP_SECT(*sb) + offset;
	disk_read( disk, offset_by_bmap, sect );

	int offset_in_sect = sector % bits_per_sect;
	if ( testb(sect, offset_in_sect) ) {
		early_kprint( PL_ERROR, "block 0x%x is not allocated!\n" );
		return;
	}
	
	clrb(sect, offset_in_sect);
	disk_write( disk, offset_by_bmap, sect );
}

