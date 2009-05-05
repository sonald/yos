	.file	"io.c"
	.local	__last_request_type_size
	.comm	__last_request_type_size,4,4
	.section	.rodata
	.align 4
	.type	PAGE_SIZE, @object
	.size	PAGE_SIZE, 4
PAGE_SIZE:
	.long	4000
.globl screen_x
	.bss
	.align 4
	.type	screen_x, @object
	.size	screen_x, 4
screen_x:
	.zero	4
.globl screen_y
	.align 4
	.type	screen_y, @object
	.size	screen_y, 4
screen_y:
	.zero	4
	.text
.globl memcpy
	.type	memcpy, @function
memcpy:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	subl	$20, %esp
	movl	8(%ebp), %eax
	movl	12(%ebp), %edx
	subl	%edx, %eax
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jns	.L2
	negl	-12(%ebp)
.L2:
	movl	-12(%ebp), %eax
	cmpl	16(%ebp), %eax
	jae	.L3
	movl	8(%ebp), %eax
	movl	%eax, -28(%ebp)
	jmp	.L4
.L3:
	movl	12(%ebp), %esi
	movl	8(%ebp), %edi
	movl	16(%ebp), %ecx
#APP
# 24 "io.c" 1
	pushl %es       
	pushl %eax      
	cld              
	movl %ds, %eax 
	movl %eax, %es 
	1:               
	cmp $0, %ecx    
	je 1f            
	movsb            
	dec %ecx        
	jmp 1b           
	1:		          
	popl %eax       
	popl %es        
	
# 0 "" 2
#NO_APP
	movl	8(%ebp), %eax
	movl	%eax, -28(%ebp)
.L4:
	movl	-28(%ebp), %eax
	addl	$20, %esp
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	memcpy, .-memcpy
.globl memmove
	.type	memmove, @function
memmove:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	movl	12(%ebp), %esi
	movl	8(%ebp), %edi
	movl	16(%ebp), %ecx
#APP
# 49 "io.c" 1
	pushl %es       
	pushl %eax      
	cld              
	movl %ds, %eax 
	movl %eax, %es 
	1:               
	cmp $0, %ecx    
	je 1f            
	movsb            
	dec %ecx        
	jmp 1b           
	1:		          
	popl %eax       
	popl %es        
	
# 0 "" 2
#NO_APP
	movl	8(%ebp), %eax
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	memmove, .-memmove
.globl memset
	.type	memset, @function
memset:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$16, %esp
	cmpl	$0, 16(%ebp)
	sete	%al
	movzbl	%al, %eax
	testl	%eax, %eax
	jne	.L11
	movl	12(%ebp), %eax
	movb	%al, -5(%ebp)
	movl	16(%ebp), %eax
	leal	-1(%eax), %ecx
	movzbl	-5(%ebp), %eax
	movl	8(%ebp), %esi
#APP
# 77 "io.c" 1
	cld 
	1: cmp $0, %ecx 
	jl 1f 
	movb %al, (%esi, %ecx) 
	dec %ecx 
	jmp 1b 
	1: nop 
	
# 0 "" 2
#NO_APP
	jmp	.L10
.L11:
	nop
.L10:
	addl	$16, %esp
	popl	%esi
	popl	%ebp
	ret
	.size	memset, .-memset
	.section	.rodata
.LC0:
	.string	"0123456789ABCDEF"
	.text
.globl early_itos
	.type	early_itos, @function
early_itos:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	subl	$1100, %esp
	cmpl	$2, 12(%ebp)
	je	.L13
	cmpl	$16, 12(%ebp)
	je	.L13
	cmpl	$10, 12(%ebp)
	jne	.L14
.L13:
	movl	$1, -1068(%ebp)
	jmp	.L15
.L14:
	movl	$0, -1068(%ebp)
.L15:
	movl	-1068(%ebp), %eax
	movl	%eax, -24(%ebp)
	cmpl	$0, -24(%ebp)
	jne	.L16
	movl	$0, -1064(%ebp)
	jmp	.L17
.L16:
	leal	-1048(%ebp), %edx
	movl	%edx, -1072(%ebp)
	movl	$0, -1076(%ebp)
	movl	$1024, %eax
	cmpl	$4, %eax
	jb	.L18
	movl	$256, -1080(%ebp)
	movl	-1072(%ebp), %edi
	movl	-1080(%ebp), %ecx
	movl	-1076(%ebp), %eax
	rep stosl
.L18:
	movl	$1024, 8(%esp)
	movl	$0, 4(%esp)
	movl	$__early_buf, (%esp)
	call	memset
	movl	8(%ebp), %eax
	movl	%eax, -20(%ebp)
	movl	$0, -16(%ebp)
	leal	-1048(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	movb	$48, (%eax)
	jmp	.L19
.L20:
	movl	-20(%ebp), %edx
	movl	%edx, %eax
	sarl	$31, %edx
	idivl	12(%ebp)
	movl	%edx, -16(%ebp)
	movl	-20(%ebp), %edx
	movl	%edx, -1092(%ebp)
	movl	-1092(%ebp), %edx
	movl	%edx, %eax
	sarl	$31, %edx
	idivl	12(%ebp)
	movl	%eax, -20(%ebp)
	movl	-16(%ebp), %eax
	movzbl	.LC0(%eax), %edx
	movl	-12(%ebp), %eax
	movb	%dl, (%eax)
	addl	$1, -12(%ebp)
.L19:
	cmpl	$0, -20(%ebp)
	jne	.L20
	movl	-12(%ebp), %eax
	leal	-1048(%ebp), %edx
	subl	%edx, %eax
	cmpl	$1024, %eax
	jle	.L21
	movl	$0, -1064(%ebp)
	jmp	.L17
.L21:
	leal	-1048(%ebp), %eax
	cmpl	%eax, -12(%ebp)
	jne	.L22
	movl	$0, -1064(%ebp)
	jmp	.L17
.L22:
	movl	$__early_buf, -8(%ebp)
	jmp	.L23
.L24:
	subl	$1, -12(%ebp)
	movl	-12(%ebp), %eax
	movzbl	(%eax), %edx
	movl	-8(%ebp), %eax
	movb	%dl, (%eax)
	addl	$1, -8(%ebp)
.L23:
	leal	-1048(%ebp), %eax
	cmpl	%eax, -12(%ebp)
	jae	.L24
	movl	-8(%ebp), %eax
	movb	$0, (%eax)
	movl	$__early_buf, -1064(%ebp)
.L17:
	movl	-1064(%ebp), %eax
	addl	$1100, %esp
	popl	%edi
	popl	%ebp
	ret
	.size	early_itos, .-early_itos
.globl early_strncpy
	.type	early_strncpy, @function
early_strncpy:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$28, %esp
	movl	16(%ebp), %eax
	movw	%ax, -32(%ebp)
	movzwl	-32(%ebp), %ebx
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strlen
	cmpl	%eax, %ebx
	jle	.L27
	movl	12(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strlen
	movw	%ax, -34(%ebp)
	jmp	.L28
.L27:
	movzwl	-32(%ebp), %eax
	movw	%ax, -34(%ebp)
.L28:
	movzwl	-34(%ebp), %eax
	movw	%ax, -14(%ebp)
	movl	8(%ebp), %edi
	movl	12(%ebp), %esi
	movzwl	-14(%ebp), %ecx
#APP
# 133 "io.c" 1
	cld 
	 rep movsb 
	
# 0 "" 2
#NO_APP
	addl	$28, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	early_strncpy, .-early_strncpy
.globl early_strlen
	.type	early_strlen, @function
early_strlen:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L31
.L32:
	addl	$1, -4(%ebp)
.L31:
	movl	8(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	setne	%al
	addl	$1, 8(%ebp)
	testb	%al, %al
	jne	.L32
	movl	-4(%ebp), %eax
	leave
	ret
	.size	early_strlen, .-early_strlen
.globl __default_color_scheme
	.data
	.align 32
	.type	__default_color_scheme, @object
	.size	__default_color_scheme, 32
__default_color_scheme:
	.long	7
	.long	0
	.long	2
	.long	0
	.long	3
	.long	0
	.long	4
	.long	0
	.text
.globl early_kprint
	.type	early_kprint, @function
early_kprint:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$56, %esp
	movl	$0, -20(%ebp)
	leal	12(%ebp), %eax
	movl	%eax, -24(%ebp)
	movl	$4, __last_request_type_size
	movl	$1024, 8(%esp)
	movl	$0, 4(%esp)
	movl	$__early_print_buf, (%esp)
	call	memset
	movl	$__early_print_buf, -4(%ebp)
	jmp	.L35
.L45:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$37, %al
	jne	.L36
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 12(%ebp)
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al,%eax
	movl	%eax, -36(%ebp)
	cmpl	$100, -36(%ebp)
	je	.L40
	cmpl	$100, -36(%ebp)
	jg	.L43
	cmpl	$37, -36(%ebp)
	je	.L38
	cmpl	$99, -36(%ebp)
	je	.L39
	jmp	.L44
.L43:
	cmpl	$115, -36(%ebp)
	je	.L41
	cmpl	$120, -36(%ebp)
	je	.L42
	jmp	.L44
.L40:
	movl	__last_request_type_size, %eax
	addl	%eax, -24(%ebp)
	movl	$4, __last_request_type_size
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	$10, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	early_itos
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strlen
	movw	%ax, -10(%ebp)
	movzwl	-10(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strncpy
	movzwl	-10(%ebp), %eax
	addl	%eax, -4(%ebp)
	jmp	.L44
.L42:
	movl	__last_request_type_size, %eax
	addl	%eax, -24(%ebp)
	movl	$4, __last_request_type_size
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -16(%ebp)
	movl	$16, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	early_itos
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strlen
	movw	%ax, -10(%ebp)
	movzwl	-10(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strncpy
	movzwl	-10(%ebp), %eax
	addl	%eax, -4(%ebp)
	jmp	.L44
.L41:
	movl	__last_request_type_size, %eax
	addl	%eax, -24(%ebp)
	movl	$4, __last_request_type_size
	movl	-24(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strlen
	movw	%ax, -10(%ebp)
	movzwl	-10(%ebp), %eax
	movl	%eax, 8(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strncpy
	movzwl	-10(%ebp), %eax
	addl	%eax, -4(%ebp)
	jmp	.L44
.L39:
	movl	__last_request_type_size, %eax
	addl	%eax, -24(%ebp)
	movl	$4, __last_request_type_size
	movl	-24(%ebp), %eax
	movzbl	(%eax), %eax
	movb	%al, -25(%ebp)
	movl	$1, 8(%esp)
	leal	-25(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	early_strncpy
	addl	$1, -4(%ebp)
	nop
	jmp	.L44
.L38:
	movl	-4(%ebp), %eax
	movb	$37, (%eax)
	addl	$1, -4(%ebp)
	jmp	.L44
.L36:
	movl	12(%ebp), %eax
	movzbl	(%eax), %edx
	movl	-4(%ebp), %eax
	movb	%dl, (%eax)
	addl	$1, -4(%ebp)
.L44:
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 12(%ebp)
.L35:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L45
	movl	-4(%ebp), %eax
	movb	$0, (%eax)
	movl	$0, -24(%ebp)
	movl	$0, __last_request_type_size
	movl	-4(%ebp), %edx
	movl	$__early_print_buf, %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, -20(%ebp)
	movl	$0, -16(%ebp)
	jmp	.L46
.L47:
	movl	8(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	$__early_print_buf, %eax
	movzbl	(%eax), %eax
	movsbl	%al,%ecx
	movl	__default_color_scheme(,%edx,8), %eax
	movl	__default_color_scheme+4(,%edx,8), %edx
	movl	%eax, 4(%esp)
	movl	%edx, 8(%esp)
	movl	%ecx, (%esp)
	call	early_video_putc
	addl	$1, -16(%ebp)
.L46:
	movl	-16(%ebp), %eax
	cmpl	-20(%ebp), %eax
	jl	.L47
	movl	-20(%ebp), %eax
	leave
	ret
	.size	early_kprint, .-early_kprint
.globl scroll_up
	.type	scroll_up, @function
scroll_up:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	subl	$16, %esp
	cmpl	$24, 8(%ebp)
	jle	.L50
	movl	$25, 8(%ebp)
	jmp	.L51
.L50:
	cmpl	$0, 8(%ebp)
	jg	.L51
	movl	$0, 8(%ebp)
	jmp	.L53
.L51:
	movl	$160, -24(%ebp)
	movl	$0, -20(%ebp)
	movl	-24(%ebp), %eax
	imull	8(%ebp), %eax
	addl	-20(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	PAGE_SIZE, %edx
	movl	-24(%ebp), %eax
	imull	8(%ebp), %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, -12(%ebp)
	movl	-16(%ebp), %esi
	movl	-20(%ebp), %edi
	movl	-12(%ebp), %ecx
#APP
# 240 "io.c" 1
	pushl %ds       
	cld              
	movl %es, %eax 
	movl %eax, %ds 
	1:               
	cmp $0, %ecx    
	je 1f            
	movsb            
	dec %ecx        
	jmp 1b           
	1:		          
	popl %ds        
	
# 0 "" 2
#NO_APP
.L53:
	addl	$16, %esp
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
	.size	scroll_up, .-scroll_up
.globl need_scroll
	.type	need_scroll, @function
need_scroll:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	screen_x, %eax
	cmpl	$79, %eax
	jne	.L55
	movl	screen_y, %eax
	cmpl	$24, %eax
	jne	.L55
	movl	$1, -4(%ebp)
	jmp	.L56
.L55:
	movl	$0, -4(%ebp)
.L56:
	movl	-4(%ebp), %eax
	leave
	ret
	.size	need_scroll, .-need_scroll
.globl forward_cursor
	.type	forward_cursor, @function
forward_cursor:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	call	need_scroll
	testl	%eax, %eax
	je	.L59
	movl	$1, (%esp)
	call	scroll_up
	jmp	.L62
.L59:
	movl	screen_x, %eax
	cmpl	$80, %eax
	jne	.L61
	movl	$0, screen_x
	movl	screen_y, %eax
	addl	$1, %eax
	movl	%eax, screen_y
	jmp	.L62
.L61:
	movl	screen_x, %eax
	addl	$1, %eax
	movl	%eax, screen_x
.L62:
	leave
	ret
	.size	forward_cursor, .-forward_cursor
.globl get_cursor
	.type	get_cursor, @function
get_cursor:
	pushl	%ebp
	movl	%esp, %ebp
	movl	screen_x, %edx
	movl	8(%ebp), %eax
	movl	%edx, (%eax)
	movl	screen_y, %edx
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	popl	%ebp
	ret
	.size	get_cursor, .-get_cursor
.globl set_cursor
	.type	set_cursor, @function
set_cursor:
	pushl	%ebp
	movl	%esp, %ebp
	cmpl	$0, 8(%ebp)
	js	.L67
	cmpl	$79, 8(%ebp)
	jg	.L67
	cmpl	$0, 12(%ebp)
	js	.L67
	cmpl	$24, 12(%ebp)
	jg	.L67
	movl	8(%ebp), %eax
	movl	%eax, screen_x
	movl	12(%ebp), %eax
	movl	%eax, screen_y
.L67:
	popl	%ebp
	ret
	.size	set_cursor, .-set_cursor
.globl early_video_putc
	.type	early_video_putc, @function
early_video_putc:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	pushl	%ebx
	subl	$36, %esp
	movl	8(%ebp), %eax
	movb	%al, -28(%ebp)
	movl	12(%ebp), %eax
	movl	%eax, %edx
	andl	$15, %edx
	movl	16(%ebp), %eax
	sall	$4, %eax
	orl	%edx, %eax
	movb	%al, -9(%ebp)
	movl	$0, -16(%ebp)
	movl	$0, -20(%ebp)
	leal	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	leal	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	get_cursor
	movsbl	-28(%ebp),%eax
	movl	%eax, -36(%ebp)
	cmpl	$10, -36(%ebp)
	je	.L71
	cmpl	$13, -36(%ebp)
	je	.L72
	cmpl	$9, -36(%ebp)
	je	.L70
	jmp	.L79
.L71:
	movl	$0, screen_x
	movl	screen_y, %eax
	movl	%eax, -32(%ebp)
	cmpl	$0, -32(%ebp)
	js	.L80
	cmpl	$23, -32(%ebp)
	jle	.L74
	cmpl	$24, -32(%ebp)
	je	.L75
	jmp	.L78
.L74:
	movl	screen_y, %eax
	addl	$1, %eax
	movl	%eax, screen_y
	nop
	jmp	.L78
.L75:
	movl	$1, (%esp)
	call	scroll_up
	jmp	.L78
.L70:
	movl	screen_x, %eax
	addl	$8, %eax
	cmpl	$79, %eax
	jg	.L81
	movl	screen_x, %eax
	addl	$8, %eax
	movl	%eax, screen_x
	jmp	.L78
.L72:
	movl	$0, screen_x
	jmp	.L78
.L79:
	movl	-16(%ebp), %eax
	movl	-20(%ebp), %edx
	movzbl	-28(%ebp), %ebx
	movzbl	-9(%ebp), %ecx
#APP
# 329 "io.c" 1
	pusha                    
	xorl %esi, %esi        
	imull $80, %edx         
	add %eax, %edx         
	mov %edx, %esi         
	shll $1, %esi           
	movb %bl, %es: (%esi) 
	inc %esi                
	movb %cl, %es: (%esi) 
	popa                     
	
# 0 "" 2
#NO_APP
	call	forward_cursor
	jmp	.L78
.L80:
	nop
	jmp	.L78
.L81:
	nop
.L78:
	addl	$36, %esp
	popl	%ebx
	popl	%esi
	popl	%ebp
	ret
	.size	early_video_putc, .-early_video_putc
	.comm	__early_print_buf,1024,32
	.comm	__early_buf,1024,32
	.ident	"GCC: (GNU) 4.3.2 20081105 (Red Hat 4.3.2-7)"
	.section	.note.GNU-stack,"",@progbits
