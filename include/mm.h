#ifndef _mm_h
#define _mm_h

#include <common.h>
#include <kernel.h>

#define MEM_RANGE_SIZE_ADDRESS (GDT_ADDRESS+GDT_SIZE)
#define MEM_RANGE_LIST_BASE    (MEM_RANGE_SIZE_ADDRESS+4)

#define MEM_RANGE_SIZE_ADDRESS_TMP  0xf000
#define MEM_RANGE_LIST_BASE_TMP     (MEM_RANGE_SIZE_ADDRESS_TMP+4)


typedef struct addr_range_desc_struct 
{
	uint32 baseLow;
	uint32 baseHigh;
	uint32 lengthLow;
	uint32 lengthHigh;
	uint32 type;
} addr_range_desc_t;

// ranges 
typedef struct addr_range_list_struct
{
	int num_entries;
	addr_range_desc_t ranges[0];
} addr_range_list_t;


// page dir start after memory range struture ( remain maximum of 20*20 bytes )
//#define PAGE_DIR_BASE      ((MEM_RANGE_LIST_BASE + 20*20 + PAGE_SIZE-1) & 0xfffff000)
#define PAGE_DIR_BASE      (0x100000)

// we use 4k-size page
#define PAGE_SIZE          (1<<12)
#define PAGE_TABLE_BASE    (PAGE_DIR_BASE+PAGE_SIZE)

#define PG_P			0x0001
#define PG_RD_ONLY		0x0000
#define PG_RW	    	0x0002
#define PG_US_SUPER     0x0000
#define PG_US_USER      0x0004
#define PG_PWT			0x0008
#define PG_PCD			0x0010
#define PG_ACCESS		0x0020
#define PG_Dirty        0x0040
/* bit 0x0080, clear for 4k */
#define PG_PS_4K        0x0000
#define PG_PAT          0x0000
#define PG_Global       0x1000

extern void check_memory();
/**
 * should call check_memory before call this
 */
extern void init_mm();

/**
 * setup default virtual -> physical memory map
 */
extern void setup_default_page_table();

#endif
