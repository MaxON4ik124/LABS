	.file	"test2.c"
	.text
	.globl	i2
	.bss
	.align 4
i2:
	.space 4
	.globl	j4
	.align 4
j4:
	.space 4
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
	movl	$5, i2(%rip)
	movl	$6, j4(%rip)
	movl	j4(%rip), %eax
	movl	%eax, i2(%rip)
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
