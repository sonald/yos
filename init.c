#include <io.h>
#include <kernel.h>
#include <isr.h>
#include <timer.h>
#include <keyboard.h>
#include <task.h>

//////////////////////////////  testing routines  //////////////////////////////

void test_io()
{
	char c = 'P';
	print_level_color_t clr = {
		.fg = GREEN,
		.bg = BLACK
	};

	scroll_up(4);
	set_cursor(0, 5);	

	early_video_putc(c, clr);
	early_video_putc('\n', clr);
	early_video_putc(c, clr);
	early_video_putc('\t', clr);
	early_video_putc(c, clr);
	
	set_cursor(VIDEO_COLUMNS-1, VIDEO_ROWS-1);
	early_video_putc('\r', clr);
	early_video_putc('M', clr);
}

void test_io2()
{
	early_kprint(PL_ERROR, "KPRINT: this is 1st testing\n");
	early_kprint(PL_ERROR, "KPRINT: this is %dnd testing\n", 2);
	set_cursor(0, 10);
	early_kprint(PL_ERROR, "12345678123456781234567812345678\n" );
	early_kprint(PL_ERROR, "KPRINT: this\tis\t%drd\ttesting\n", 3);
	char buf[] = "str_arg";
	early_kprint(PL_ERROR, "KPRINT: test str: %s\n", buf);
	set_cursor(0, 24);
	early_kprint(PL_INFO, "KPRINT: test scroll \n\n", buf);
}



void test_iolib()
{
	print_level_color_t clr = {
		.fg = GREEN,
		.bg = BLACK
	};
	
	set_cursor(0, 6);
	char buf[100] = "------";
	
	int i = 219, len = 0;
	char *s = early_itos(i, 10);
	early_video_putc(*s, clr);
	len = early_strlen(s);
	early_video_putc(len+'0', clr);

	early_strncpy(buf, s, len);
	early_video_putc(buf[0], clr);
}

////////////////////  KERNEL START  ////////////////////////////////////////////

/**
 * Global interrupt service table and GDT pointer for C routines
 * because CS/DS are both base 0x0, so this works
 */
u64* idt = (u64*)IDT_ADDRESS;
u64* gdt = (u64*)GDT_ADDRESS;

void install_isr_descripter(int vector, u64 offset)
{
	if ( vector >= 0 && vector < IDT_ENTRIES ) {
//		u64 idt_entry = ((ATTR_386I)<<32) | (((u64)SEL_CODE)<<16);
		u64 idt_entry = (((u64)ATTR_386I)<<40) | (((u64)SEL_CODE)<<16);		
		idt_entry |= (offset & 0xffff);
		idt_entry |= (offset << 32 ) & 0xffff000000000000ULL;
		idt[vector] = idt_entry;
	}
}

//Note that only exceptions 8 and 10-14 have an error code
// dummy is actually %ebp pushed from cpu_info caller, so is ret_ip
void cpu_info(PRINT_LEVEL lvl, uint32 dummy, uint32 ret_ip, uint32 ss, uint32 fs,
			  uint32 es, uint32 gs, uint32 ds, uint32 edi, uint32 esi, uint32 ebp,
			  uint32 esp, uint32 ebx, uint32 edx, uint32 ecx, uint32 eax, uint32 isr,
			  uint32 errno, uint32 eip, uint32 cs, uint32 eflags, uint32 old_esp,
			  uint32 old_ss )
{
	dummy = dummy;
	ret_ip = ret_ip;
	static char *exception_msgs[] = {
		"Division By Zero Exception",
		"Debug Exception",
		"Non Maskable Interrupt Exception",
		"Breakpoint Exception",
		"Into Detected Overflow Exception",
		"Out of Bounds Exception",
		"Invalid Opcode Exception",
		"No Coprocessor Exception",
		"Double Fault Exception",
		"Coprocessor Segment Overrun Exception",
		"Bad TSS Exception",
		"Segment Not Present Exception",
		"Stack Fault Exception",
		"General Protection Fault Exception",
		"Page Fault Exception",
		"Unknown Interrupt Exception",
		"Coprocessor Fault Exception",
		"Alignment Check Exception (486+)",
		"Machine Check Exception (Pentium/586+)",
		"Reserved"
	};

	char *err_info = isr<20 ? exception_msgs[isr]
		: ( isr<32 ? "Reserved" : "User Interrupt" );
	early_kprint( lvl, "%s(errcode: %x)\n", err_info, errno );
	early_kprint( lvl, "eax:%x ebx:%x ecx:%x edx:%x\nesi:%x edi:%x ebp:%x esp:%x\n",
				  eax, ebx, ecx, edx, esi, edi, ebp, esp );
	early_kprint( lvl, "CS:%x EIP:%x\n", cs, eip );
	early_kprint( lvl, "DS:%x ES:%x SS:%x GS:%x FS:%x\n", ds, es, ss, gs, fs );
}

#define TMP_COMMON_HANDLER 	{					\
		__asm__ __volatile__ (					\
			"pushl %%eax  \n\t"					\
			"call cpu_info \n\t"				\
			:: "a"(PL_ERROR)					\
			);									\
		halt();									\
	}

/* #define TMP_COMMON_HANDLER 	{					\ */
/* 		halt();									\ */
/* 	} */


/**
 * exception handlers called from corresponding ISR
 */
void do_divide_zero()
{
	TMP_COMMON_HANDLER
}

void do_debug_exception()
{
	TMP_COMMON_HANDLER
}

void do_nmi()
{
	TMP_COMMON_HANDLER
}

void do_breakpoint()
{
	TMP_COMMON_HANDLER
}

void do_overflow()
{
	TMP_COMMON_HANDLER
}

void do_out_of_bound()
{
	TMP_COMMON_HANDLER
}

void do_invalid_opcode()
{
	TMP_COMMON_HANDLER
}

void do_no_coprocessor()
{
	TMP_COMMON_HANDLER
}

void do_double_fault()
{
	TMP_COMMON_HANDLER
}

void do_coprocessor_overrun()
{
	TMP_COMMON_HANDLER
}

void do_bad_tss()
{
	TMP_COMMON_HANDLER
}

void do_segment_not_present()
{
	TMP_COMMON_HANDLER
}

void do_stack_exception()
{
	TMP_COMMON_HANDLER
}

void do_gp_fault()
{
	TMP_COMMON_HANDLER
}

void do_page_fault()
{
	TMP_COMMON_HANDLER
}

void do_unknown_interrupt()
{
	TMP_COMMON_HANDLER
}

void do_coprocessor_fault()
{
	TMP_COMMON_HANDLER
}

void do_alignment_check_exception()
{
	TMP_COMMON_HANDLER
}

void do_machine_check_exception()
{
	TMP_COMMON_HANDLER
}

void do_reserved()
{
	TMP_COMMON_HANDLER
}

static void setup_idt()
{
	struct _idt_struct {
		unsigned short length;
		unsigned long base;
	} __attribute__((packed)) idt_t = {
		.length = IDT_SIZE-1,
		.base = IDT_ADDRESS
	};

	int i = 0;
	for ( i = 0; i < PRESERVED_ISRS; i++ )
		install_isr_descripter( i, (uint32)(isr[i<<1]) );

	for (; i < IDT_ENTRIES; i++ )
		install_isr_descripter( i, (uint32)default_isr );
	
	__asm__ __volatile__ (
		"lidt %0 \n\t"
		::"m"(idt_t)
		);
}

/**
 * initial PICs, remap IRQs
 */
void init_pic() 
{
	// write icw2 ~ icw4 
	outb(0b00010001, 0x20);
	outb(0b00010001, 0xa0);

	// icw2: offset in idt
	// master: 0x20 ~ 0x27
	// slave : 0x28 ~ 0x2f
	outb(MASTER_PIC, 0x21);
	outb(SLAVE_PIC,  0xa1);

	// icw3: connect at pin2
	outb(0x04,  0x21);
	outb(0x02,  0xa1);

	// icw4: 8086 mode, Manual EOI, Slave PIC
	outb(0x01, 0x21);
	outb(0x01, 0xa1);

	// disable all interrupts
	outb(0xff, 0x21);
	outb(0xff, 0xa1);
}

void setup_gdt_entry(int pos, uint32 base, uint32 limit, uint32 attrs)
{
	
	u64 entry = (limit & 0xffffULL);
	u64 _attrs = (u64)attrs;
	u64 _base = (u64)base;
	u64 _limit = (u64)limit;
	early_kprint( PL_DEBUG, "pos %d, base %x, limit %x, attrs %x \n",
				  pos, base, limit, _attrs );
	
	entry |= ((_base << 16) & 0xff0000ffffff0000ULL);
	entry |= ((_attrs << 40) & 0x00ffff0000000000ULL);
	entry |= ((_limit & 0x000f0000ULL) << 32);
	gdt[pos] = entry;
	
/*	
	.word \limit & 0xffff
	.word \base & 0xffff
	.byte (\base >> 16) & 0x00ff
	.word (\attrs & 0xf0ff) | ((\limit >> 8) & 0x0f00)
	.byte (\base >> 24) & 0xff
*/	
}

void user_test()
{
	__asm__ __volatile__ (
		"mov $0x43, %%eax   \n\t"
		"mov %%eax, %%ds \n\t"
		"mov %%eax, %%fs \n\t"
		"mov %%eax, %%gs \n\t"
		"mov $0x20, %%eax \n\t"
		"mov %%eax, %%es \n\t"
		:::"eax"
		);
	
	early_kprint( PL_WARN, "Hello From User Space\n" );
	while(1);
}

/**
 * now we switch to use c, and this is the first routine
 * seting up all ISRs and other initialization
 */

void init()
{
	cli();
	
	setup_idt();
	init_pic();

	init_8254_timer();
	init_kbd();
	
	set_cursor(0, 0);
	setup_gdt_entry(SEL_USER_CODE>>3, 0UL, 0xffffUL, (F_USER32_CODE));
	setup_gdt_entry(SEL_USER_DATA>>3, 0UL, 0xffffUL, (F_USER32_DATA));

	struct tss_struct tss0 = {
		.link = 0,
		.esp0 = (uint32)0x80000,
		.ss0 = SEL_USER_DATA,
		.cr3 = 0,
		.eip = (uint32)user_test,
		.eflags = 0x3202,
		.es = SEL_VIDEO,
		.cs = SEL_USER_DATA,
		.ss = SEL_USER_DATA,
		.ds = SEL_USER_DATA,
		.fs = SEL_USER_DATA,
		.gs = SEL_USER_DATA,
		.ldt_selector = SEL_CUR_LDT,
		.t = 0,
		.io_map = 0
	};

	//for TSS & LDT
	setup_gdt_entry((SEL_CUR_TSS>>3), 0UL, 0xffffUL, (F_USER32_TSS));
	setup_gdt_entry((SEL_CUR_LDT>>3), 0UL, 0xffffUL, (F_USER32_LDT));
	
	__asm__ (
		"ltrw %%ax \n\t"
		"lldt %%bx \n\t"
		::"a"(SEL_CUR_TSS), "b"(SEL_CUR_LDT)
		);

	sti();
	
	__asm__ __volatile__ (
		"pushl %0        \n\t"
		"pushl $0x80000  \n\t"
		"pushl %1        \n\t"
		"pushl %2        \n\t"
		"retf            \n\t"
		"mov %0, %%eax   \n\t"
		"mov %%eax, %%ds \n\t"
		"mov %%eax, %%fs \n\t"
		"mov %%eax, %%gs \n\t"
		::"i"(SEL_USER_DATA), "i"(SEL_USER_CODE), "i"(user_test)
		);

	while(1);
	
#if 0
	int i = 0;
	int x = 0, y = 0;
	while(1) {
		get_cursor( &x, &y );
		set_cursor( 0, 10 );
		uint32 tick = jiffies;
		if ( (tick % 100) == 0 )
			early_kprint( PL_WARN, "%d", i );
		i++;
		
		set_cursor( x, y );
	}
	
	early_kprint( PL_INFO, "after while\n" );
#endif
	
}
