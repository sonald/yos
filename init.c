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

		/* u64 idt_entry = 0x00008e0000000000ULL | (((u64)SEL_CODE)<<16); */
		/* idt_entry |= (offset<<32) & 0xffff000000000000ULL; */
		/* idt_entry |= (offset) & 0xffff; */
		/* idt[vector] = idt_entry;		 */
	}
}


//Note that only exceptions 8 and 10-14 have an error code

void cpu_info() 
{
	uint32 eax, ebx, ecx, edx, edi, esi, ebp, esp;
	__asm__ __volatile__ (
		"nop \n\t"
		: "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		);
	
	early_kprint( PL_ERROR, "EAX = %x EBX = %x ECX = %x EDX = %x \n", eax, ebx, ecx, edx );
	//TODO: bug! next line cause triple fault
//	early_kprint( PL_ERROR, "ESI = %x EDI = %x \n", esi, edi );
}

/**
 * exception handlers called from corresponding ISR
 */
void do_divide_zero()
{
	set_cursor(0, 5);
	early_kprint( PL_ERROR, "divide by zero!\n" );
	cpu_info();
//	halt();
}

void do_nmi()
{
}

void do_breakpoint()
{
}

void do_overflow()
{
}

void do_out_of_bound()
{
}

void do_invalid_opcode()
{
}

void do_no_coprocessor()
{
}

void do_double_fault()
{
}

void do_coprocessor_overrun()
{
}

void do_bad_tss()
{
	halt();
}

void do_segment_not_present()
{
}

void do_stack_exception()
{
}

void do_gp_fault()
{
}

void do_page_fault()
{
}

void do_unknown_interrupt()
{
}

void do_coprocessor_fault()
{
}

void do_alignment_check_exception()
{
}

void do_machine_check_exception()
{
}

void do_reserved()
{
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
	for ( i = 0; i < 1; i++ )	
		install_isr_descripter( i, (uint32)divide_zero );
	
	/* for ( i = 0; i < PRESERVED_ISRS; i++ ) */
	/* 	install_isr_descriptor(i, isr[i]); */

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

	int d = 10, i = 0;
	d = d / i;
}
