	.section .data
	.global _address_range_list
	.global _num_ards
_data:
pm_msg:
	.asciz "Entering Protecting Mode. \x0"

	.section .text
	.globl _kernel_start
	.code16
	.include "common.inc"
_code:
	jmp _kernel_start

kern_welcome:
	.asciz "welcome to yos ......\x0"

GDT_START:	
	DESC_ENTRY 0,	0, 		0
DESC_CODE:	
	DESC_ENTRY 0, 	0xffff, 	DPL_0 + F_DC32_4G + F_CODE_R
DESC_DATA:	
	DESC_ENTRY 0, 	0xffff,		DPL_0 + F_DC32_4G + F_DATA_W
DESC_STACK:	
	DESC_ENTRY 0, 	0xffff,		DPL_0 + F_DC32_4G + F_DATA_W
DESC_VIDOE:	
	DESC_ENTRY 0xb8000,  (0xc0000-0xb8000), DPL_3 + F_DC32_4G + F_DATA_W
DESC_CUR_TSS:	
	DESC_ENTRY 0,  	0xffff, 	DPL_0 + F_DC32_4G + F_DATA_W
DESC_LDT:	
	DESC_ENTRY 0, 	0xffff,		DPL_0 + F_DC32_4G + F_DATA_W
DESC_USER_CODE:
	DESC_ENTRY 0, 	0xffff, 	F_USER32_CODE
DESC_USER_DATA:	
	DESC_ENTRY 0, 	0xffff,		F_USER32_DATA
	
gdt_struct:
	.2byte GDT_SIZE - 1
	// use GDT_ADDRESS here instead of GDT_START because
	// we are gonna move GDT to GDT_ADDRESS for accessable
	// of c routines
	.4byte GDT_ADDRESS
	
_kernel_start:
.if 0
	// move adrs to MEM_RANGE_LIST_BASE
	push	%ds
	mov	$MEM_RANGE_LIST_BASE_TMP>>4, %ax
	mov	%ax, %ds
	mov	$MEM_RANGE_LIST_BASE_TMP & 0x0f, %si

	mov	$MEM_RANGE_LIST_BASE>>4, %ax
	mov	%eax, %es
	mov 	$MEM_RANGE_LIST_BASE & 0x0f, %di
	
	mov	$MEM_RANGE_SIZE_ADDRESS_TMP, %ax
	movl	(%eax), %ecx
	movl	$MEM_RANGE_SIZE_ADDRESS, %eax
	movl	%ecx, (%eax)
	imul 	$5, %cx, %cx
	cld
1:	
	cmp 	$0, %cx
	je 	1f
	rep	movsl
	pop	%ds
.endif

.if 0 /* test how many entries read */
	movl 	$0xb800, %eax
	movl 	%eax, %es
	movl 	$(0*80 + 2)<<1, %edi
	movl	$MEM_RANGE_SIZE_ADDRESS_TMP, %eax
	movl	(%eax), %ebx
	movl	%ebx, %eax
	addb	$'a', %al
	movb	%al, %es:(%edi)
	inc	%edi
	movb	$0x05, %es:(%edi)
1:	jmp 1b
.endif

	display_str $kern_welcome, 3, 0
	// prepare descriptors
/*	
	xorl	%eax, %eax
	movw 	$_code32_size, %ax
	movw 	%ax, DESC_CODE

	xorl	%eax, %eax
	movw 	$_code32, %ax
	movw 	%ax, (DESC_CODE+2)
	shr	$16, %eax
	movb	%al, (DESC_CODE+4)
	movb	%ah, (DESC_CODE+7)
*/

.if 0
	// prepare testing user code seg
	xorl	%eax, %eax
	movw 	$_user_code_size, %ax
	movw 	%ax, DESC_USER_CODE
	
	xorl	%eax, %eax
	movw 	$_user_code, %ax
	movw 	%ax, (DESC_USER_CODE+2)
	shr	$16, %eax
	movb	%al, (DESC_USER_CODE+4)
	movb	%ah, (DESC_USER_CODE+7)
.endif	
	// move GDT to some place easy to find by C routines
	xor	%ax, %ax
	mov	%ax, %ds
	mov 	$GDT_ADDRESS>>4, %eax
	mov	%ax, %es
	mov	$GDT_START, %si
	xor	%di, %di
	mov	$GDT_SIZE>>2, %cx
	rep 	movsl

	cli

enable_a20:       
        // The Undocumented PC
        inb	$0x64,   %al   
        testb  	$0x2,    %al
        jnz	enable_a20
        movb   	$0xbf, %al
        outb   	%al, $0x64

	// set GDT
	lgdt 	gdt_struct
	
	// go to pmode!
	movl 	%cr0, %eax
	or 	$0x01, %eax
	mov 	%eax, %cr0

	//sti cause 'triple fault of qemu
	//	sti  

	// we set all segment base 0x0, so memory model will follow linux convension
	ljmp	$SEL_CODE, $_code32
	
_code32:
	.code32
	.global init

	/*
	 * notes: before I have my own assembler, I should keep all thess descriptors
	 * compatible with linux ( cause i use gas/gcc ), so gas generated asm code
	 * and memory convension can be correctly used.
	 */
	movl	$SEL_DATA, %eax
	movl	%eax, %ds
	movl	%eax, %fs
	movl	%eax, %gs

	movl	%eax, %ss
	movl	$0x80000, %esp
	
	movl 	$SEL_VIDEO, %eax
	movl	%eax, %es
	movl	$pm_msg, %esi
	
	xorl 	%edi, %edi
	movl 	$(4*80 + 0)<<1, %edi
	movb 	$0x07, %al
	cld
1:	
	cmp 	$0, (%esi)
	je 	1f

	movsb
	stosb
	jmp 	1b
1:	nop

	// test jmp to ring 3: ss, esp, params(#0), cs, eip
.if 0
	pushl $SEL_USER_DATA
	pushl $0x80000
	pushl $SEL_USER_CODE
	pushl $0
	retf
.endif

	call init
1:	jmp 1b	

_user_code:
	xorl 	%edi, %edi
	movl 	$(1*80 + 0)<<1, %edi
	movb 	$0x05, %al
	movb	$'U', %es:(%edi)
	inc	%edi
	movb	%al, %es:(%edi)
	nop
1:	jmp 1b

	.set _user_code_size, . - _user_code
	