	.file	"test4.c"
	.text
	.globl	k2
	.data
	.align 4
k2:
	.long	3
	.globl	j5
	.bss
	.align 4
j5:
	.space 4
	.globl	i
	.data
	.align 4
i:
	.long	3
	.globl	ivector4
	.bss
	.align 8
ivector4:
	.space 12
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	call	__main
	movl	j5(%rip), %eax
	sall	$2, %eax
	movl	%eax, k2(%rip)
	movl	$0, i(%rip)
	jmp	.L2
.L3:
	movl	i(%rip), %eax
	leal	(%rax,%rax), %edx
	movl	i(%rip), %eax
	movl	%edx, %ecx
	cltq
	leaq	(%rax,%rax), %rdx
	leaq	ivector4(%rip), %rax
	movw	%cx, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L2:
	movl	i(%rip), %eax
	cmpl	$5, %eax
	jle	.L3
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
