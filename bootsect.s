	.section .text
	.code16
	.global _init
	.include "common.inc"
_init:
	jmp _code

	.macro clear_screen
	movb $0, %al
	movb $0, %bh
	movb $0, %ch
	movb $0, %cl
	movb $23, %dh
	movb $79, %dl
	movb $0x06, %ah
	int $0x10
	.endm
msg:
	.asciz "loading kernel ......\x0"
msg2:
	.string "load success, now executing ......\x0"
num_read:  /* record number of sectors read */
	.byte 0x0

_code:
	mov %cs, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %gs
	mov %ax, %fs
	mov %ax, %ss
	mov $0x6000, %ax
	mov %ax, %sp

	clear_screen
	display_str $msg, 0, 0

	call load_kernel
	
	//move kernel to 0:0
	mov	$0x8000, %si
	mov 	$KERNEL_ADDRESS, %ax
	mov	%ax, %es
	xor	%di, %di
	
	xor	%cx, %cx
	mov	$NUM_SECTORS<<9, %cx
	cld
1:	
	cmp 	$0, %cx
	je 	1f

	movsb
	dec %cx
	
	jmp 	1b
1:	nop
	
	display_str $msg2, 1, 0

	jmp $0,$KERNEL_ADDRESS
1:	jmp 1b	

	
read_sector:
	push %bp
	movw %sp, %bp

retry:	
	movb $0, %ch 	/* cylinder */
	movb $0, %dh    /* # head */
	movb $0, %dl	/* # driver */
	movb $1, %al   	/* n_sectors */
	movb $0x02, %ah
	int $0x13

	jc retry
	
	movw %bp, %sp
	pop %bp
	ret
	
load_kernel:
	push %bp
	movw %sp, %bp

	// reset disk
	movb $0, %ah
	int $0x13
	
	movb $2, %cl

	movw $0x800, %ax
	movw %ax, %es
	movw $0, %bx
1:
	call read_sector
	inc %cl
	addw $512, %bx

	cmpb $NUM_SECTORS, %cl
	jl 1b

	movb %cl, num_read
	
	movw %bp, %sp
	pop %bp
	ret
	
	.org 0x01fe, 0x90
	.word 0xaa55
	