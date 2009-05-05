// ISRs
	.section .text
	.global divide_zero
	.global default_isr
	.code32

	.include "common.inc"
/**	
Exception #  	 Description  	 Error Code?
0 	Division By Zero Exception 	No
1 	Debug Exception 	No
2 	Non Maskable Interrupt Exception 	No
3 	Breakpoint Exception 	No
4 	Into Detected Overflow Exception 	No
5 	Out of Bounds Exception 	No
6 	Invalid Opcode Exception 	No
7 	No Coprocessor Exception 	No
8 	Double Fault Exception 	Yes
9 	Coprocessor Segment Overrun Exception 	No
10 	Bad TSS Exception 	Yes
11 	Segment Not Present Exception 	Yes
12 	Stack Fault Exception 	Yes
13 	General Protection Fault Exception 	Yes
14 	Page Fault Exception 	Yes
15 	Unknown Interrupt Exception 	No
16 	Coprocessor Fault Exception 	No
17 	Alignment Check Exception (486+) 	No
18 	Machine Check Exception (Pentium/586+) 	No
19 to 31 	Reserved Exceptions 	No
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

bad_tss:
	pushl $10
	jmp isr_common
	
/*	define_isr bad_tss, 10, 1
	define_isr segment_not_present, 11, 1
	define_isr stack_exception, 12, 1
/*	define_isr gp_fault, 13, 1
	define_isr page_fault, 14, 1
	define_isr unknown_interrupt, 15, 0
	define_isr coprocessor_fault, 16, 0
	define_isr alignment_check_exception, 17, 0
	define_isr machine_check_exception, 18, 0
	
	define_isr reserved_19, 19, 0
	define_isr reserved_20, 20, 0
*/	
isr:
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero

	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero

	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero

	.4byte 	divide_zero,  do_divide_zero
	.4byte 	divide_zero,  do_divide_zero		
	
	
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

	movl 	$isr, %ebp
	movl 	52(%esp), %esi
	call 	*4(%ebp, %esi, 8)

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

1:	jmp 1b
	
	pop  	%fs
	pop  	%ss
	pop  	%es
	pop 	%gs
	pop  	%ds
	popal
	iret
	