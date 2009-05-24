#ifndef _kernel_h
#define _kernel_h

#include <common.h>

#define SECTOR_SIZE        0x200

/* kernel base */
#define KERNEL_ADDRESS     0

/* kernel size in sectors */
#define NUM_SECTORS        30

/* default: 2K */
#define KERNEL_STACK_SIZE  0x800

/* base address of IDT */	
#define IDT_ADDRESS        0x90000
#define IDT_ENTRIES        256
#define IDT_SIZE           (IDT_ENTRIES<<3)

#define GDT_ADDRESS        (IDT_ADDRESS+IDT_SIZE)
#define GDT_ENTRIES        9
#define GDT_SIZE           (GDT_ENTRIES<<3)

#define DPL_0       0x0000
#define DPL_1    	0x0020 
#define DPL_2       0x0040
#define DPL_3       0x0060

// Present
#define F_P 	0x0080
	
// S: Data/Code, Gate/Sys
#define F_DC	0x0010
#define F_SYS 	0x0000

//Granularity, D/B 32bit
#define F_G 	0x8000
#define F_DB32	0x4000

#define F_CODE_R 	0x000a
#define F_CODE_RA	0x000b
#define F_CODE_CRA  0x000f

#define F_DATA_W 	0x0002
#define F_DATA_WA	0x0003
#define F_DATA_D 	0x0004
#define F_DATA_DW	0x0006
#define F_DATA_DWA  0x0007

//Gate Type
#define F_LDT       0x0002
#define F_TASK	    0x0005
#define F_386TSS	0x0009
#define F_386TSS_BUSY	0x000b
#define F_836C   	0x000c
#define F_386I  	0x000e
#define F_386TRAP	0x000f

// convient defs, include Present, Sys, Type
#define ATTR_LDT    (F_P + F_SYS + F_LDT)
#define ATTR_TASK   (F_P + F_SYS + F_TASK)
#define ATTR_386I   (F_P + F_SYS + F_386I)
//#define ATTR_386I   0x8e00ULL

//convient defs
#define F_USER32_DATA     (F_G + F_P + F_DC + DPL_3 + F_DB32 + F_DATA_WA)
#define F_USER32_CODE     (F_G + F_P + F_DC + DPL_3 + F_DB32 + F_CODE_RA)
#define F_USER32_LDT      (F_G + F_P + DPL_0 + F_SYS + F_LDT)
#define F_USER32_TSS      (F_G + F_P + DPL_0 + F_SYS + F_386TSS)

#define RPL_0              0
#define RPL_1              1
#define RPL_2              2
#define RPL_3              3

#define SEL_TI             4

#define SEL_CODE           (0x08 + RPL_0)
#define SEL_DATA           (0x10 + RPL_0)
#define SEL_VIDEO          (0x20 + RPL_0)
#define SEL_CUR_TSS	       (0x28 + RPL_0)
#define SEL_CUR_LDT	       (0x30 + RPL_0)
//#define SEL_USER_CODE      (0x38 + RPL_3)
//#define SEL_USER_DATA      (0x40 + RPL_3)
#define SEL_USER_CODE      (0x00 + RPL_3 + SEL_TI)
#define SEL_USER_DATA      (0x08 + RPL_3 + SEL_TI)

typedef unsigned long long u64;

#define halt() __asm__ __volatile__ ( "hlt \n\t" )

#define cli()  __asm__ __volatile__ ( "cli \n\t" )
#define sti()  __asm__ __volatile__ ( "sti \n\t" )

// interrupt corresponding
#define HZ  100
extern uint32 jiffies;

extern void setup_gdt_entry(int pos, uint32 base, uint32 limit, uint32 attrs);

#endif
	 
