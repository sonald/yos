#ifndef _isr_h
#define _isr_h

/**
 * contains all isr routine entries, callable by c
 */
extern void divide_zero();
extern void debug_exception();
extern void nmi();
extern void breakpoint();
extern void overflow();
extern void out_of_bound();
extern void invalid_opcode();
extern void no_coprocessor();
extern void double_fault();
extern void coprocessor_overrun();
extern void bad_tss();
extern void segment_not_present();
extern void stack_exception();
extern void gp_fault();
extern void page_fault();
extern void unknown_interrupt();
extern void coprocessor_fault();
extern void alignment_check_exception();
extern void machine_check_exception();

extern void default_isr();

#define PRESERVED_ISRS     32

extern void (*isr[PRESERVED_ISRS<<1])();

struct __regs 
{
	uint32 eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint32 ds, gs, es, fs, ss;
};

struct __exception_stack_frame
{
	uint32 ss, fs, es, gs, ds;
	uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32 isr, errno, eip, cs, eflags, old_esp, old_ss;
};
	
#endif
