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
	.2byte 0x00
ards_error_str:
	.asciz "Read Memory Ranges Failed. \x0"

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
.if 1 
	// read memory info into _address_region_list
	xor	%bx, %bx
	xor	%esi, %esi /* used to calculate table size */
	push	%es
	movl	$MEM_RANGE_LIST_BASE_TMP>>4, %eax
	movl	%eax, %es
	movl 	$MEM_RANGE_LIST_BASE_TMP & 0x0f, %edi

ards_next:
	movl	$0x00e820, %eax
	movl	$0x534d4150, %edx
	mov	$20, %cx	
	int 	$0x15
	jc	ards_error
	
	inc	%esi
	add	%cx, %di
	cmp	$0, %ebx
	jne 	ards_next
	jmp 	ards_done
	
ards_error:
	display_str $ards_error_str, 0, 0
	xorl 	%esi, %esi
	hlt
	
ards_done:
	pop	%es
	movl 	$MEM_RANGE_SIZE_ADDRESS_TMP, %ebx
	movl	%esi, (%ebx)
.endif	

	display_str $msg, 1, 0
	call load_kernel

	//move kernel to 0:0
	xorl 	%esi, %esi
	mov	$0x8000, %si
	mov 	$KERNEL_ADDRESS, %ax
	mov	%ax, %es
	xorl	%edi, %edi
	
	xorl	%ecx, %ecx
	mov	$NUM_SECTORS<<7, %cx
	cld
1:	
	cmp 	$0, %ecx
	je 	1f
	rep	movsl

.if 0 /* this is buggy? */
	movl 	$MEM_RANGE_SIZE_ADDRESS_TMP, %ebx
	movl 	$0xb800, %eax
	movl 	%eax, %es
	xorl 	%edi, %edi
	movl 	$(0*80 + 2)<<1, %edi
	movb 	(%ebx), %al
	addb	$'0', %al
	movb	%al, %es:(%edi)
	inc	%edi
	movb	$0x05, %es:(%edi)
.endif
	
	display_str $msg2, 2, 0

	jmp $0,$KERNEL_ADDRESS
1:	jmp 1b	


	// in: ax LBA #sector, should convert to sector/cylinder/head
read_sector:
        pushw   %ax
        pushw   %cx
        pushw   %dx
        pushw   %bx

        movw    %si,    %ax       
        xorw    %dx,    %dx
        movw    $18,    %bx    /* 18 sectors per track for floppy disk */
	
        divw    %bx  		/* ax / dx -> ax (Quo) , dx (Rem) */
        incw    %dx
        movb    %dl,    %cl    /* cl=sector number */

	movb	%al,	%ch
	shr	$1,	%ch  	/* track(cylinder) */
	andb	$1, 	%al
	movb	%al,	%dh	/* head */
	
	xorb	%dl, 	%dl	/* driver */
	popw	%bx
	
retry:
        movb    $0x1,   %al    /* read 1 sector */
        movb    $0x2,   %ah
        int     $0x13
        jc      retry
	
        popw    %dx
        popw    %cx
        popw    %ax
        ret
	
load_kernel:
	push %bp
	movw %sp, %bp

	// reset disk
	movb $0, %ah
	int $0x13

	movw $0x800, %ax
	movw %ax, %es
	xorw %bx, %bx
	
	movw $NUM_SECTORS, %cx
	movw $1, %si     /* 0 is boot sector	*/
goon:	
	call read_sector
        addw    $512,    %bx
        incw    %si
        loop    goon

	movw %si, num_read
	
	movw %bp, %sp
	pop %bp
	ret
	
	.org 0x01fe, 0x90
	.word 0xaa55
	