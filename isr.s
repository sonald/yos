// ISRs
	.section .text
	.global divide_zero
	.global default_isr
	.code32

	.include "common.inc"
	
divide_zero:
	pushal
	push %ds
	push %gs
	push %es
	push %ss
	push %fs
/*
	mov $SEL_DATA, %eax
	mov %eax, %ds
	mov %eax, %gs
	mov %eax, %fs
	mov %eax, %ss
	mov $SEL_VIDEO, %eax
	mov %eax, %es

	call do_divide_zero
*/	
	pop  %fs
	pop  %ss
	pop  %es
	pop  %gs
	pop  %ds
	popal
	iret
isr:
	.4byte 0
	
default_isr:
	pushal
	push %ds
	push %gs
	push %es
	push %ss
	push %fs

	call do_divide_zero
	
	pop  %fs
	pop  %ss
	pop  %es
	pop  %gs
	pop  %ds
	popal
	iret
	