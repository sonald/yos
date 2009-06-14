// ISRs
	.section .text
	.global divide_zero
	.global debug_exception
	.global nmi
	.global breakpoint
	.global overflow
	.global out_of_bound
	.global invalid_opcode
	.global no_coprocessor
	.global double_fault
	.global coprocessor_overrun
	
	.global bad_tss
	.global segment_not_present
	.global stack_exception
	.global gp_fault
	.global page_fault
	.global unknown_interrupt
	.global coprocessor_fault
	.global alignment_check_exception
	.global machine_check_exception

	// IRQs
	.global timer
	.global keyboard_handler
	.global hd_handler
	
	.global default_isr
	.global isr
	
	.code32
	.include "common.inc"
/**	
#  	 Description  	 			Error Code?
0 	Division By Zero Exception 		No
1 	Debug Exception 			No
2 	Non Maskable Interrupt Exception 	No
3 	Breakpoint Exception 			No
4 	Into Detected Overflow Exception 	No
5 	Out of Bounds Exception 		No
6 	Invalid Opcode Exception 		No
7 	No Coprocessor Exception 		No
8 	Double Fault Exception 			Yes
9 	Coprocessor Segment Overrun Exception 	No
10 	Bad TSS Exception 			Yes
11 	Segment Not Present Exception 		Yes
12 	Stack Fault Exception 			Yes
13 	General Protection Fault Exception 	Yes
14 	Page Fault Exception 			Yes
15 	Unknown Interrupt Exception 		No
16 	Coprocessor Fault Exception 		No
17 	Alignment Check Exception (486+) 	No
18 	Machine Check Exception (Pentium/586+) 	No
19 \
    |-	Reserved Exceptions 			No
31 /	
*/
	.macro define_isr except_name, isr_no, has_errno = 0
\except_name:	
	.ifeq \has_errno
	pushl $0
	.endif
	pushl $\isr_no
	jmp isr_common
	.endm

	define_isr divide_zero, 0, 0
	define_isr debug_exception, 1, 0
	define_isr nmi, 2, 0
	define_isr breakpoint, 3, 0
	define_isr overflow, 4, 0
	define_isr out_of_bound, 5, 0
	define_isr invalid_opcode, 6, 0
	define_isr no_coprocessor, 7, 0
	define_isr double_fault, 8, 1
	define_isr coprocessor_overrun, 9, 0
	define_isr bad_tss, 10, 1
	define_isr segment_not_present, 11, 1
	define_isr stack_exception, 12, 1
	define_isr gp_fault, 13, 1
	define_isr page_fault, 14, 1
	define_isr unknown_interrupt, 15, 0
	define_isr coprocessor_fault, 16, 0
	define_isr alignment_check_exception, 17, 0
	define_isr machine_check_exception, 18, 0

	define_isr reserved_19, 19, 0
	define_isr reserved_20, 20, 0
	define_isr reserved_21, 21, 0
	define_isr reserved_22, 22, 0	
	define_isr reserved_23, 23, 0
	define_isr reserved_24, 24, 0	
	define_isr reserved_25, 25, 0
	define_isr reserved_26, 26, 0	
	define_isr reserved_27, 27, 0
	define_isr reserved_28, 28, 0	
	define_isr reserved_29, 29, 0
	define_isr reserved_30, 30, 0	
	define_isr reserved_31, 31, 0
	
	define_isr timer, 32, 0
	define_isr keyboard_handler, 33, 0	
	define_isr _irq2,	34, 0		
	define_isr _irq3,	35, 0				
	define_isr _irq4,	36, 0				
	define_isr _irq5,	37, 0				
	define_isr _irq6,	38, 0				
	define_isr _irq7,	39, 0				
	define_isr _irq8,	40, 0				
	define_isr _irq9,	41, 0				
	define_isr _irq10,	42, 0				
	define_isr _irq11,	43, 0				
	define_isr _irq12,	44, 0					
	define_isr _irq13,	45, 0				
	define_isr hd_handler,	46, 0				
	define_isr _irq15,	47, 0				

isr:
	.4byte 	divide_zero,  			do_divide_zero
	.4byte 	debug_exception,  		do_debug_exception
	.4byte 	nmi,				do_nmi
	.4byte 	breakpoint, 			do_breakpoint
	.4byte 	overflow,			do_overflow
	.4byte 	out_of_bound,			do_out_of_bound
	.4byte 	invalid_opcode,			do_invalid_opcode
	.4byte 	no_coprocessor,			do_no_coprocessor
	.4byte 	double_fault,			do_double_fault
	.4byte 	coprocessor_overrun,		do_coprocessor_overrun
	.4byte 	bad_tss,			do_bad_tss
	.4byte 	segment_not_present,		do_segment_not_present
	.4byte 	stack_exception,		do_stack_exception
	.4byte 	gp_fault,			do_gp_fault
	.4byte 	page_fault,			do_page_fault
	.4byte 	unknown_interrupt,		do_unknown_interrupt
	.4byte 	coprocessor_fault,		do_coprocessor_fault
	.4byte 	alignment_check_exception,	do_alignment_check_exception
	.4byte 	machine_check_exception,	do_machine_check_exception

	.4byte reserved_19, 			do_reserved
	.4byte reserved_20, 			do_reserved
	.4byte reserved_21, 			do_reserved
	.4byte reserved_22, 			do_reserved
	.4byte reserved_23, 			do_reserved
	.4byte reserved_24, 			do_reserved
	.4byte reserved_25, 			do_reserved
	.4byte reserved_26, 			do_reserved
	.4byte reserved_27, 			do_reserved
	.4byte reserved_28, 			do_reserved
	.4byte reserved_29, 			do_reserved
	.4byte reserved_30, 			do_reserved
	.4byte reserved_31, 			do_reserved

	.4byte timer,				do_timer
	.4byte keyboard_handler,		do_keyboard
	.4byte _irq2,				do_reserved
	.4byte _irq3,				do_reserved
	.4byte _irq4,				do_reserved
	.4byte _irq5,				do_reserved
	.4byte _irq6,				do_reserved
	.4byte _irq7,				do_reserved
	.4byte _irq8,				do_reserved
	.4byte _irq9,				do_reserved
	.4byte _irq10,				do_reserved
	.4byte _irq11,				do_reserved
	.4byte _irq12,				do_reserved	
	.4byte _irq13,				do_reserved
	.4byte hd_handler,			do_reserved
	.4byte _irq15,				do_reserved

	
isr_common:
	/**
	stack order of isr:
76	ss(orig)
72	esp(orig)
68	eflags	
64	cs
60	eip
56	0/errno
52	isr_nr     <----=
48	eax             |
44	ecx             |
40	edx             |
36	ebx             |
32	esp(orig) =-----=
28	ebp
24	esi
20	edi
16	ds
12	gs
8	es
4	fs
0	ss
	 */
	pushal
	pushl 	%ds
	pushl	%gs
	pushl 	%es
	pushl 	%fs
	pushl 	%ss

	mov 	$SEL_DATA, %eax
	mov 	%eax, %ds
	mov 	%eax, %gs
	mov 	%eax, %fs
	mov 	$SEL_VIDEO, %eax
	mov 	%eax, %es

	movl 	$isr, %ebx
	movl 	52(%esp), %esi
	call 	*4(%ebx, %esi, 8)

	// why not pop %ss?
	addl 	$4, %esp
//	pop	%ss /* this inhibits all interrupts (include NMIs) */
	pop  	%fs
	pop  	%es
	pop  	%gs
	pop  	%ds
	popal

	// pop up #isr and error code
	addl	$8, %esp
	iret

default_isr:
	pushal
	push 	%ds
	push 	%gs
	push 	%es
	push 	%ss
	push 	%fs

	pop  	%fs
	pop  	%ss
	pop  	%es
	pop 	%gs
	pop  	%ds
	popal
	iret
	