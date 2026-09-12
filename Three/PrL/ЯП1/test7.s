	.file	"test7.c"
	.text
	.globl	i
	.bss
	.align 4
i:
	.space 4
	.globl	k5
	.align 4
k5:
	.space 4
	.globl	j5
	.align 4
j5:
	.space 4
	.globl	i5
	.align 4
i5:
	.space 4
	.text
	.globl	loop_jamming
	.def	loop_jamming;	.scl	2;	.type	32;	.endef
	.seh_proc	loop_jamming
loop_jamming:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	$0, i(%rip)
	jmp	.L2
.L3:
	movl	j5(%rip), %edx
	movl	i(%rip), %eax
	imull	%eax, %edx
	movl	16(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, k5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L2:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L3
	movl	$0, i(%rip)
	jmp	.L4
.L5:
	movl	k5(%rip), %eax
	imull	16(%rbp), %eax
	movl	%eax, %edx
	movl	i(%rip), %eax
	imull	%edx, %eax
	movl	%eax, i5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L4:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L5
	nop
	nop
	popq	%rbp
	ret
	.seh_endproc
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
	movl	$7, %ecx
	call	loop_jamming
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
