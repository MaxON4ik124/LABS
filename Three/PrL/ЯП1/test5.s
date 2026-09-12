	.file	"test5.c"
	.text
	.globl	i
	.bss
	.align 4
i:
	.space 4
	.globl	ivector5
	.align 32
ivector5:
	.space 400
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
	movl	$0, i(%rip)
	jmp	.L2
.L3:
	movl	i(%rip), %eax
	addl	%eax, %eax
	addl	$3, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector5(%rip), %rax
	movl	$5, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L2:
	movl	i(%rip), %eax
	cmpl	$99, %eax
	jle	.L3
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
