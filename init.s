	.file	"init.c"
	.local	__last_request_type_size
	.comm	__last_request_type_size,4,4
	.text
.globl init
	.type	init, @function
init:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	subl	$16, %esp
	movb	$80, -10(%ebp)
	movb	$7, -9(%ebp)
	movzbl	-10(%ebp), %ebx
	movzbl	-9(%ebp), %ecx
#APP
# 14 "init.c" 1
	pusha 
	xorl %esi, %esi 
	movb %bl, %es: (%esi) 
	inc %esi 
	movb %cl, %es: (%esi) 
	popa 
	
# 0 "" 2
#NO_APP
.L2:
	jmp	.L2
	.size	init, .-init
	.ident	"GCC: (GNU) 4.3.2 20081105 (Red Hat 4.3.2-7)"
	.section	.note.GNU-stack,"",@progbits
