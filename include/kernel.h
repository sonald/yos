#ifndef _kernel_h
#define _kernel_h

#define SECTOR_SIZE        0x200

/* kernel base */
#define KERNEL_ADDRESS     0

/* kernel size in sectors */
#define NUM_SECTORS        0x10

/* default: 2K */
#define KERNEL_STACK_SIZE  0x800

/* base address of IDT */	
#define IDT_ADDRESS        0x90000
#define IDT_ENTRIES        256
#define IDT_SIZE           (IDT_ENTRIES<<3)

#define GDT_ADDRESS        (IDT_ADDRESS+IDT_SIZE)
#define GDT_ENTRIES        7
#define GDT_SIZE           (GDT_ENTRIES<<3)


#define DPL_0       0x0000
#define DPL_1    	0x2000 
#define DPL_2       0x4000
#define DPL_3       0x6000

// include Present, Sys, Type
#define F_LDT       0x8200ULL
#define F_TASK      0x8500ULL
#define F_386I      0x8e00ULL

#define RPL_0              0
#define RPL_1              1
#define RPL_2              2
#define RPL_3              3

#define SEL_CODE           0x08 + RPL_0
#define SEL_DATA           0x10 + RPL_0
#define SEL_VIDEO          0x20 + RPL_0

typedef unsigned long long u64;

#define PRESERVED_ISRS     32


#define halt() __asm__ __volatile__ ( "hlt \n\t" )

#define cli()  __asm__ __volatile__ ( "cli \n\t" )
#define sti()  __asm__ __volatile__ ( "sti \n\t" )
#endif
	 
