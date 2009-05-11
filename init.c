#include <io.h>
#include <kernel.h>
#include <isr.h>

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
		u64 idt_entry = ((F_386I)<<32) | (((u64)SEL_CODE)<<16);
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
	/* for ( i = 0; i < 1; i++ ) */
	/* 	install_isr_descripter( i, (uint32)divide_zero ); */

	for ( i = 0; i < PRESERVED_ISRS; i++ )
		install_isr_descripter( i, (u64)(isr[i<<1]) );

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

/**
 * now we switch to use c, and this is the first routine
 * seting up all ISRs and other initialization
 */

void init()
{
	setup_idt();
	init_pic();

	__asm__ __volatile__ ( " sti \n\t" );
	
//	while(1);
	
	int d = 10, i = 0;
//	d = d / i;

	early_kprint( PL_INFO, "after exception\n" );
	
}
