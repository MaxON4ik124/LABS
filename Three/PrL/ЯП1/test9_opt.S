	.file	"test9.c"
 # GNU C17 (Rev2, Built by MSYS2 project) version 14.2.0 (x86_64-w64-mingw32)
 #	compiled by GNU C version 14.2.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.27-GMP

 # GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 # options passed: -mtune=generic -march=nocona -O3 -ffloat-store -fno-defer-pop -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops
	.text
	.p2align 4
	.globl	jump_compression
	.def	jump_compression;	.scl	2;	.type	32;	.endef
	.seh_proc	jump_compression
jump_compression:
	.seh_endprologue
 # test9.c:9: {
	movl	40(%rsp), %r10d	 # m, m
 # test9.c:11:                if( i < j )
	cmpl	%ecx, %edx	 # i, j
	jle	.L2	 #,
	cmpl	%r9d, %r8d	 # l, k
	jge	.L3	 #,
	cmpl	%r10d, %r9d	 # m, l
	jl	.L4	 #,
.L6:
 # test9.c:12:                    if( j < k )
	cmpl	%edx, %r8d	 # j, k
	jle	.L18	 #,
 # test9.c:11:                if( i < j )
	cmpl	%edx, %ecx	 # j, i
	jge	.L2	 #,
.L8:
	jmp	.L8	 #
	.p2align 4,,10
	.p2align 3
.L18:
 # test9.c:21:                        j += k;
	addl	%r8d, %edx	 # k, j
 # test9.c:11:                if( i < j )
	cmpl	%edx, %ecx	 # j, i
	jl	.L6	 #,
.L2:
 # test9.c:26:                    i += j;
	addl	%edx, %ecx	 # j, i
 # test9.c:27:                return( i + j + k + l + m );
	leal	(%rcx,%rdx), %eax	 #, _1
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r8d, %eax	 # k, _2
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r9d, %eax	 # l, _3
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r10d, %eax	 # m, _18
 # test9.c:28:            } /* Конец jump_compression */
	ret	
	.p2align 4
	.p2align 4,,10
	.p2align 3
.L19:
 # test9.c:21:                        j += k;
	addl	%r8d, %edx	 # k, j
 # test9.c:11:                if( i < j )
	cmpl	%edx, %ecx	 # j, i
	jge	.L2	 #,
.L3:
 # test9.c:12:                    if( j < k )
	cmpl	%edx, %r8d	 # j, k
	jle	.L19	 #,
 # test9.c:19:                            k += l;
	addl	%r9d, %r8d	 # l, k
 # test9.c:27:                return( i + j + k + l + m );
	leal	(%rcx,%rdx), %eax	 #, _1
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r8d, %eax	 # k, _2
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r9d, %eax	 # l, _3
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r10d, %eax	 # m, _18
 # test9.c:28:            } /* Конец jump_compression */
	ret	
	.p2align 4
	.p2align 4,,10
	.p2align 3
.L10:
 # test9.c:21:                        j += k;
	addl	%r8d, %edx	 # k, j
 # test9.c:11:                if( i < j )
	cmpl	%edx, %ecx	 # j, i
	jge	.L2	 #,
.L4:
 # test9.c:12:                    if( j < k )
	cmpl	%edx, %r8d	 # j, k
	jle	.L10	 #,
 # test9.c:27:                return( i + j + k + l + m );
	leal	(%rcx,%rdx), %eax	 #, _1
 # test9.c:15:                                l += m;
	addl	%r10d, %r9d	 # m, l
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r8d, %eax	 # k, _2
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r9d, %eax	 # l, _3
 # test9.c:27:                return( i + j + k + l + m );
	addl	%r10d, %eax	 # m, _18
 # test9.c:28:            } /* Конец jump_compression */
	ret	
	.seh_endproc
	.section	.text.startup,"x"
	.p2align 4
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # test9.c:4: {
	call	__main	 #
 # test9.c:5:     jump_compression(1, 2, 3, 4, 5);
	movl	$4, %r9d	 #,
	movl	$3, %r8d	 #,
	movl	$5, 32(%rsp)	 #,
	movl	$2, %edx	 #,
	movl	$1, %ecx	 #,
	call	jump_compression	 #
 # test9.c:6: }
	xorl	%eax, %eax	 #
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
