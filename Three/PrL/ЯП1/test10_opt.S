	.file	"test10.c"
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
 # test10.c:5: {
	call	__main	 #
 # test10.c:7:     flt_1 = 2.4 + 6.3;
	movq	.LC0(%rip), %rax	 #, tmp103
 # test10.c:9:     j2 = i + 0;
	movl	i(%rip), %edx	 # i, i.0_1
 # test10.c:6:     i3 = 1 + 2;
	movl	$3, i3(%rip)	 #, i3
 # test10.c:8:     i2 = 5;
	movl	$5, i2(%rip)	 #, i2
 # test10.c:7:     flt_1 = 2.4 + 6.3;
	movq	%rax, flt_1(%rip)	 # tmp103, flt_1
 # test10.c:13: }
	xorl	%eax, %eax	 #
 # test10.c:9:     j2 = i + 0;
	movl	%edx, j2(%rip)	 # i.0_1, j2
 # test10.c:10:     k2 = i / 1;
	movl	%edx, k2(%rip)	 # i.0_1, k2
 # test10.c:11:     i4 = i * 1;
	movl	%edx, i4(%rip)	 # i.0_1, i4
 # test10.c:12:     i5 = i * 0;
	movl	$0, i5(%rip)	 #, i5
 # test10.c:13: }
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.globl	flt_1
	.bss
	.align 8
flt_1:
	.space 8
	.globl	i5
	.align 4
i5:
	.space 4
	.globl	i4
	.align 4
i4:
	.space 4
	.globl	i
	.data
	.align 4
i:
	.long	3
	.globl	k2
	.bss
	.align 4
k2:
	.space 4
	.globl	j2
	.align 4
j2:
	.space 4
	.globl	i2
	.align 4
i2:
	.space 4
	.globl	i3
	.align 4
i3:
	.space 4
	.section .rdata,"dr"
	.align 8
.LC0:
	.long	1717986918
	.long	1075930726
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
