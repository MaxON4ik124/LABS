	.file	"test4.c"
 # GNU C17 (Rev2, Built by MSYS2 project) version 14.2.0 (x86_64-w64-mingw32)
 #	compiled by GNU C version 14.2.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.27-GMP

 # GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 # options passed: -mtune=generic -march=nocona -O3 -ffloat-store -fno-defer-pop -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops
	.text
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	subq	$40, %rsp	 #,
	.seh_stackalloc	40
	.seh_endprologue
 # test4.c:5: {
	call	__main	 #
 # test4.c:6:     	k2 = 4 * j5;
	movl	j5(%rip), %eax	 # j5, tmp111
 # test4.c:8: 		ivector4[ i ] = i * 2;
	movq	.LC0(%rip), %rdx	 #, tmp106
	movl	$655368, 8+ivector4(%rip)	 #, MEM <vector(2) short int> [(short int *)&ivector4 + 8B]
	movl	$6, i(%rip)	 #, i
 # test4.c:6:     	k2 = 4 * j5;
	sall	$2, %eax	 #, _2
 # test4.c:8: 		ivector4[ i ] = i * 2;
	movq	%rdx, ivector4(%rip)	 # tmp106, MEM <vector(4) short int> [(short int *)&ivector4]
 # test4.c:6:     	k2 = 4 * j5;
	movl	%eax, k2(%rip)	 # _2, k2
 # test4.c:9: }
	xorl	%eax, %eax	 #
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.globl	ivector4
	.bss
	.align 16
ivector4:
	.space 12
	.globl	i
	.data
	.align 4
i:
	.long	3
	.globl	j5
	.bss
	.align 4
j5:
	.space 4
	.globl	k2
	.data
	.align 4
k2:
	.long	3
	.section .rdata,"dr"
	.align 8
.LC0:
	.word	0
	.word	2
	.word	4
	.word	6
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
