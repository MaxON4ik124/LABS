	.file	"test9.c"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	call	__main
	movl	$5, 32(%rsp)
	movl	$4, %r9d
	movl	$3, %r8d
	movl	$2, %edx
	movl	$1, %ecx
	call	jump_compression
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	jump_compression
	.def	jump_compression;	.scl	2;	.type	32;	.endef
	.seh_proc	jump_compression
jump_compression:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	%r8d, 32(%rbp)
	movl	%r9d, 40(%rbp)
.L4:
	movl	16(%rbp), %eax
	cmpl	24(%rbp), %eax
	jge	.L5
	movl	24(%rbp), %eax
	cmpl	32(%rbp), %eax
	jge	.L6
	movl	32(%rbp), %eax
	cmpl	40(%rbp), %eax
	jge	.L7
	movl	40(%rbp), %eax
	cmpl	48(%rbp), %eax
	jge	.L12
	movl	48(%rbp), %eax
	addl	%eax, 40(%rbp)
	jmp	.L9
.L7:
	movl	40(%rbp), %eax
	addl	%eax, 32(%rbp)
	jmp	.L9
.L6:
	movl	32(%rbp), %eax
	addl	%eax, 24(%rbp)
	jmp	.L4
.L12:
	nop
.L10:
	jmp	.L4
.L5:
	movl	24(%rbp), %eax
	addl	%eax, 16(%rbp)
.L9:
	movl	16(%rbp), %edx
	movl	24(%rbp), %eax
	addl	%eax, %edx
	movl	32(%rbp), %eax
	addl	%eax, %edx
	movl	40(%rbp), %eax
	addl	%eax, %edx
	movl	48(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
