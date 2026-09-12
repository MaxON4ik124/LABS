	.file	"test3.c"
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
 # test3.c:3: {
	call	__main	 #
 # test3.c:6: }
	xorl	%eax, %eax	 #
 # test3.c:4:             k3 = 1;
	movl	$1, k3(%rip)	 #, k3
 # test3.c:6: }
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.globl	k3
	.bss
	.align 4
k3:
	.space 4
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
