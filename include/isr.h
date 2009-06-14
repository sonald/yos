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

extern void timer();
extern void keyboard_handler();
extern void hd_handler();

extern void default_isr();

// 32 exceptions and IRQ0 IRQ1
#define PRESERVED_ISRS     (32+2)

extern void (*isr[PRESERVED_ISRS<<1])();

#define MASTER_PIC  0x20
#define SLAVE_PIC   0x28

#define IRQ_TIMER            MASTER_PIC+0
#define IRQ_PS2_KBD          MASTER_PIC+1

#define IRQ_COMM2            MASTER_PIC+3
#define IRQ_COMM1            MASTER_PIC+4
#define IRQ_LPT2             MASTER_PIC+5
#define IRQ_FLOPPY           MASTER_PIC+6
#define IRQ_LPT1             MASTER_PIC+7

#define IRQ_RTC              SLAVE_PIC+0
#define PS2_MOUSE            SLAVE_PIC+4
#define IRQ_PPU_EXCEPTION    SLAVE_PIC+5

#endif
