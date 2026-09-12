	.file	"test10.c"
	.text
	.globl	i3
	.bss
	.align 4
i3:
	.space 4
	.globl	i2
	.align 4
i2:
	.space 4
	.globl	j2
	.align 4
j2:
	.space 4
	.globl	k2
	.align 4
k2:
	.space 4
	.globl	i
	.data
	.align 4
i:
	.long	3
	.globl	i4
	.bss
	.align 4
i4:
	.space 4
	.globl	i5
	.align 4
i5:
	.space 4
	.globl	flt_1
	.align 8
flt_1:
	.space 8
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
	movl	$3, i3(%rip)
	movsd	.LC0(%rip), %xmm0
	movsd	%xmm0, flt_1(%rip)
	movl	$5, i2(%rip)
	movl	i(%rip), %eax
	movl	%eax, j2(%rip)
	movl	i(%rip), %eax
	movl	%eax, k2(%rip)
	movl	i(%rip), %eax
	movl	%eax, i4(%rip)
	movl	$0, i5(%rip)
	movl	$0, %eax
	addq	$32, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC0:
	.long	1717986918
	.long	1075930726
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
