	.file	"test1.c"
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
 # test1.c:5: {
	call	__main	 #
 # test1.c:6:     for(i = 0; i < 3 ; i++) ivector[ i ] = 1;
	movq	.LC0(%rip), %rax	 #, tmp100
	movl	$1, 8+ivector(%rip)	 #, ivector[2]
	movl	$3, i(%rip)	 #, i
	movq	%rax, ivector(%rip)	 # tmp100, MEM <vector(2) int> [(int *)&ivector]
 # test1.c:7: }
	xorl	%eax, %eax	 #
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.globl	ivector
	.bss
	.align 16
ivector:
	.space 12
	.globl	i
	.align 4
i:
	.space 4
	.section .rdata,"dr"
	.align 8
.LC0:
	.long	1
	.long	1
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
