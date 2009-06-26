#include <mm.h>
#include <io.h>

void init_mm()
{
	early_kprint( PL_INFO, "MEM_RANGE_LIST_BASE: 0x%x, PAGE_DIR_BASE: 0x%x\n",
				  MEM_RANGE_LIST_BASE, PAGE_DIR_BASE );
	setup_default_page_table();
	__asm__ __volatile__ (
		"mov %%eax, %%cr3 \n\t"
		"mov %%cr0, %%eax \n\t"
// 		"1: jmp 1b \n\t"
		"or  $0x80000000, %%eax \n\t"
		"mov %%eax, %%cr0 \n\t"
		"nop \n\t"
		"nop \n\t"		
		::"a"(PAGE_DIR_BASE)
		);
}

void setup_default_page_table()
{
	uint32 *page_dir = (uint32*)PAGE_DIR_BASE;
	uint32 *page_table = (uint32*)PAGE_TABLE_BASE;
	int num_table = 1024;
	uint32 page_num = 0;
	int i = 0, j = 0;
	for ( i = 0; i < 1024; i++ ) {
		page_dir[i] = (uint32)page_table + i*PAGE_SIZE + (PG_P | PG_US_USER | PG_RW);
		for ( j = 0; j < num_table; j++ ) {
			(page_table + i*1024)[j] = PAGE_SIZE*page_num++ + (PG_P | PG_US_USER | PG_RW);
		}
	}
	early_kprint( PL_DEBUG, "setting up 0x%x pages\n", page_num );
}


//TODO: store these info into a kernel structure
void check_memory()
{
	static char* types[] = {
		"Undefined", "Usable", "Reserved", "ACPI Reclaim", "ACPI NVS"
	};

	addr_range_list_t *ards_list = (addr_range_list_t*)MEM_RANGE_SIZE_ADDRESS_TMP;
	memcpy((void*)MEM_RANGE_SIZE_ADDRESS, (void*)MEM_RANGE_SIZE_ADDRESS_TMP,
		   sizeof(int)+ards_list->num_entries*sizeof(addr_range_desc_t) );
	ards_list = (addr_range_list_t*)MEM_RANGE_SIZE_ADDRESS;
	
	early_kprint( PL_WARN, "Memory Ranges(0x%x, %d):\n",
				  (int)&ards_list->ranges, ards_list->num_entries );
	early_kprint( PL_WARN, "Base Address\tLength\tType\n" );

	int total_mem = 0;
	int i = 0;
	for ( ; i < ards_list->num_entries; i++ ) {
		addr_range_desc_t *entry = &ards_list->ranges[i];
		int idx = (entry->type > 4 ? 2: entry->type);
		early_kprint( PL_WARN, "%x\t\t%x\t%s\n",
					  entry->baseLow, entry->lengthLow, types[idx] );
		if ( (int)entry->baseLow +  (int)entry->lengthLow > total_mem )
			total_mem = (int)entry->baseLow +  (int)entry->lengthLow;
		++entry;
	}

	int kilo = total_mem / 1024;
	int mega = kilo / 1024;
	int giga = mega / 1024;
	const char *str_size = (giga? "G": ( mega? "M" : (kilo? "K" : "B")));
	int memsize = (giga? giga: ( mega? mega : (kilo? kilo : total_mem)));

	early_kprint( PL_DEBUG, "Total Memory Size: %d%s(0x%x)\n", memsize,
				  str_size, total_mem );
}

#if 0
void check_memory()
{
	static char* types[] = {
		"Undefined", "Usable", "Reserved", "ACPI Reclaim", "ACPI NVS"
	};

	int num_ards = *(int*)MEM_RANGE_SIZE_ADDRESS_TMP;
	addr_range_desc_t *entry = (addr_range_desc_t*)MEM_RANGE_LIST_BASE_TMP;
	early_kprint( PL_WARN, "Memory Ranges(0x%x, %d):\n", (int)entry, num_ards );
	early_kprint( PL_WARN, "Base Address\tLength\tType\n" );

	int total_mem = 0;
	int i = 0;
	for ( ; i < num_ards; i++ ) {
		int idx = (entry->type > 4 ? 2: entry->type);
		early_kprint( PL_WARN, "%x\t\t%x\t%s\n",
					  entry->baseLow, entry->lengthLow, types[idx] );
		if ( (int)entry->baseLow +  (int)entry->lengthLow > total_mem )
			total_mem = (int)entry->baseLow +  (int)entry->lengthLow;
		++entry;
	}

	int kilo = total_mem / 1024;
	int mega = kilo / 1024;
	int giga = mega / 1024;
	const char *str_size = (giga? "G": ( mega? "M" : (kilo? "K" : "B")));
	int memsize = (giga? giga: ( mega? mega : (kilo? kilo : total_mem)));

	early_kprint( PL_DEBUG, "Total Memory Size: %d%s(0x%x)\n", memsize,
				  str_size, total_mem );
}
#endif

