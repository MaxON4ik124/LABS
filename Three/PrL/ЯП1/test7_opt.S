	.file	"test7.c"
 # GNU C17 (Rev2, Built by MSYS2 project) version 14.2.0 (x86_64-w64-mingw32)
 #	compiled by GNU C version 14.2.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.27-GMP

 # GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 # options passed: -mtune=generic -march=nocona -O3 -ffloat-store -fno-defer-pop -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops
	.text
	.p2align 4
	.globl	loop_jamming
	.def	loop_jamming;	.scl	2;	.type	32;	.endef
	.seh_proc	loop_jamming
loop_jamming:
	.seh_endprologue
 # test7.c:8:     k5 = x + j5 * i;
	movl	j5(%rip), %eax	 # j5, j5
	movl	$5, i(%rip)	 #, i
	leal	(%rcx,%rax,4), %edx	 #, _3
	movl	%edx, k5(%rip)	 # _3, k5
 # test7.c:10:     i5 = x * k5 * i;
	imull	%ecx, %edx	 # x, _7
 # test7.c:10:     i5 = x * k5 * i;
	sall	$2, %edx	 #, tmp107
	movl	%edx, i5(%rip)	 # tmp107, i5
 # test7.c:11: } /* Конец loop_jamming */
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
 # test7.c:13: {
	call	__main	 #
 # test7.c:14:     loop_jamming(7);
	movl	$7, %ecx	 #,
	call	loop_jamming	 #
 # test7.c:15: }
	xorl	%eax, %eax	 #
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.globl	i5
	.bss
	.align 4
i5:
	.space 4
	.globl	j5
	.align 4
j5:
	.space 4
	.globl	k5
	.align 4
k5:
	.space 4
	.globl	i
	.align 4
i:
	.space 4
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
