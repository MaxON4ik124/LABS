	.file	"test8.c"
 # GNU C17 (Rev2, Built by MSYS2 project) version 14.2.0 (x86_64-w64-mingw32)
 #	compiled by GNU C version 14.2.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.27-GMP

 # GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 # options passed: -mtune=generic -march=nocona -O3 -ffloat-store -fno-defer-pop -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops
	.text
	.p2align 4
	.globl	loop_unrolling
	.def	loop_unrolling;	.scl	2;	.type	32;	.endef
	.seh_proc	loop_unrolling
loop_unrolling:
	.seh_endprologue
 # test8.c:7:         ivector4[ i ] = 0;
	movq	$0, ivector4(%rip)	 #, MEM <char[1:12]> [(void *)&ivector4]
	movl	$0, 8+ivector4(%rip)	 #, MEM <char[1:12]> [(void *)&ivector4]
	movl	$6, i(%rip)	 #, i
 # test8.c:8: } /* Конец loop_unrolling */
	ret	
	.seh_endproc
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	subq	$40, %rsp	 #,
	.seh_stackalloc	40
	.seh_endprologue
 # test8.c:10: {
	call	__main	 #
 # test8.c:11:     loop_unrolling(7);
	movl	$7, %ecx	 #,
 # test8.c:12: }
	xorl	%eax, %eax	 #
 # test8.c:11:     loop_unrolling(7);
	call	loop_unrolling	 #
	nop	
 # test8.c:12: }
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.globl	ivector4
	.bss
	.align 8
ivector4:
	.space 12
	.globl	i
	.align 4
i:
	.space 4
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
