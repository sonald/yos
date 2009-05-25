// contains some global definition
	.set SECTOR_SIZE, 0x200		/* 512 */
	.set KERNEL_ADDRESS, 0		/* kernel base */
	.set NUM_SECTORS, 32		/* kernel size in sectors */
	.set KERNEL_STACK_SIZE, 0x800 	/* default: 2K */

	.set IDT_ADDRESS, 0x90000	/* base address of IDT */	
	.set IDT_SIZE, (256<<3)		/* 256 entries */
	.set GDT_ADDRESS, (IDT_ADDRESS+IDT_SIZE) /* base address of GDT */
	.set GDT_ENTRIES, 9
	.set GDT_SIZE, (GDT_ENTRIES<<3)

	// descriptor attributes
	.set DPL_0, 	0x0000
	.set DPL_1, 	0x0020 
	.set DPL_2, 	0x0040
	.set DPL_3, 	0x0060

	// Present
	.set F_P, 	0x0080
	
	// S: Data/Code, Gate/Sys
	.set F_DC,	0x0010
	.set F_SYS, 	0x0000

	//TYPE  Gate/Code/Data
	// Accessed, Write/Read, Confirming, EXE
	.set F_CODE, 	0x0008	/* 0b00001000 */
	.set F_CODE_A, 	0x0009 	/* 0b00001001 */
	.set F_CODE_R, 	0x000a	/* 0b00001010 */
	.set F_CODE_RA,	0x000b	/* 0b00001011 */
	.set F_CODE_C, 	0x000c	/* 0b00001100 */
	.set F_CODE_CA,	0x000d	/* 0b00001101 */
	.set F_CODE_CR,	0x000e	/* 0b00001110 */
	.set F_CODE_CRA,0x000f	/* 0b00001111 */

	.set F_DATA, 	0x0000	/* 0b00000000 */
	.set F_DATA_A, 	0x0001	/* 0b00000001 */
	.set F_DATA_W, 	0x0002	/* 0b00000010 */
	.set F_DATA_WA,	0x0003  /* 0b00000011 */
	.set F_DATA_D, 	0x0004  /* 0b00000100 */
	.set F_DATA_DA,	0x0005  /* 0b00000101 */
	.set F_DATA_DW,	0x0006  /* 0b00000110 */
	.set F_DATA_DWA,0x0007	/* 0b00000111 */

	//Gate Type
	.set F_LDT,	0x0002
	.set F_TASK,	0x0005
	.set F_386TSS,	0x0009	/* available tss */
	.set F_386TSS_BUSY,	0x000b	/* busy tss */
	.set F_836C,	0x000c	/* call gate */
	.set F_386I,  	0x000e
	.set F_386TRAP,	0x000f


	//Granularity, D/B 32bit
	.set F_G, 	0x8000
	.set F_DB32,	0x4000

	// selector
	.set RPL_0, 	0
	.set RPL_1, 	1
	.set RPL_2, 	2
	.set RPL_3, 	3
	.set SEL_LOCAL,	0x04

	// convience combination flags
	.set F_DC32_4G, 	(F_DC + F_G + F_DB32 + F_P)
	.set F_USER32_DATA,     (F_G + F_P + F_DC + DPL_3 + F_DB32 + F_DATA_WA)
	.set F_USER32_CODE,     (F_G + F_P + F_DC + DPL_3 + F_DB32 + F_CODE_CRA)

	.set SEL_CODE,  	0x08 + RPL_0
	.set SEL_DATA,  	0x10 + RPL_0
	.set SEL_STACK, 	0x18 + RPL_0
	.set SEL_VIDEO, 	0x20 + RPL_0
	.set SEL_CUR_TSS,	0x28 + RPL_3
	.set SEL_CUR_LDT,	0x30 + RPL_3
	.set SEL_USER_CODE,     0x38 + RPL_3
	.set SEL_USER_DATA,     0x40 + RPL_3
	
	.macro DESC_ENTRY base, limit, attrs
	.word \limit & 0xffff
	.word \base & 0xffff
	.byte (\base >> 16) & 0x00ff
	.word (\attrs & 0xf0ff) | ((\limit >> 8) & 0x0f00)
	.byte (\base >> 24) & 0xff
	.endm
	
	// used to put str into display memory
	.macro display_str str, r = 0, c = 0
	xorw %ax, %ax
	movw %ax, %ds
	movw \str, %si

	movw $0xb800, %ax
	movw %ax, %es
	
	// calculate coordinate
	xorw %di, %di
	mov $(\r*80 + \c)<<1, %di
	movb $0x07, %al
	cld
1:	
	cmp $0, (%si)
	je 1f

	movsb
	stosb
	jmp 1b
1:	nop
	.endm

	
	