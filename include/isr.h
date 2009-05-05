#ifndef _isr_h
#define _isr_h

/**
 * contains all isr routine entries, callable by c
 */
extern void divide_zero();
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
extern void reserved();

extern void default_isr();

#endif
