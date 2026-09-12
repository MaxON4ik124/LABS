	.file	"test5.c"
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
 # test5.c:5: {
	call	__main	 #
	leaq	12+ivector5(%rip), %rax	 #, ivtmp.14
	leaq	800(%rax), %rdx	 #, _19
	.p2align 4
	.p2align 3
.L2:
 # test5.c:7:                 ivector5[ i * 2 + 3 ] = 5;
	movl	$5, (%rax)	 #, MEM[(int *)_15]
 # test5.c:6:                 for( i = 0; i < 100; i++ )
	addq	$80, %rax	 #, ivtmp.14
 # test5.c:7:                 ivector5[ i * 2 + 3 ] = 5;
	movl	$5, -72(%rax)	 #, MEM[(int *)_15]
	movl	$5, -64(%rax)	 #, MEM[(int *)_15]
	movl	$5, -56(%rax)	 #, MEM[(int *)_15]
	movl	$5, -48(%rax)	 #, MEM[(int *)_15]
	movl	$5, -40(%rax)	 #, MEM[(int *)_15]
	movl	$5, -32(%rax)	 #, MEM[(int *)_15]
	movl	$5, -24(%rax)	 #, MEM[(int *)_15]
	movl	$5, -16(%rax)	 #, MEM[(int *)_15]
	movl	$5, -8(%rax)	 #, MEM[(int *)_15]
 # test5.c:6:                 for( i = 0; i < 100; i++ )
	cmpq	%rdx, %rax	 # _19, ivtmp.14
	jne	.L2	 #,
	movl	$100, i(%rip)	 #, i
 # test5.c:8: }
	xorl	%eax, %eax	 #
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.globl	ivector5
	.bss
	.align 32
ivector5:
	.space 400
	.globl	i
	.align 4
i:
	.space 4
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
