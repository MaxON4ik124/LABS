	.file	"opt_max.c"
	.text
	.globl	i
	.bss
	.align 4
i:
	.space 4
	.globl	j
	.align 4
j:
	.space 4
	.globl	k
	.align 4
k:
	.space 4
	.globl	l
	.align 4
l:
	.space 4
	.globl	m
	.align 4
m:
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
	.globl	g3
	.align 4
g3:
	.space 4
	.globl	h3
	.align 4
h3:
	.space 4
	.globl	i3
	.align 4
i3:
	.space 4
	.globl	k3
	.align 4
k3:
	.space 4
	.globl	m3
	.align 4
m3:
	.space 4
	.globl	i4
	.align 4
i4:
	.space 4
	.globl	j4
	.align 4
j4:
	.space 4
	.globl	i5
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
	.globl	flt_1
	.align 8
flt_1:
	.space 8
	.globl	flt_2
	.align 8
flt_2:
	.space 8
	.globl	flt_3
	.align 8
flt_3:
	.space 8
	.globl	flt_4
	.align 8
flt_4:
	.space 8
	.globl	flt_5
	.align 8
flt_5:
	.space 8
	.globl	flt_6
	.align 8
flt_6:
	.space 8
	.globl	ivector
	.align 8
ivector:
	.space 12
	.globl	ivector2
ivector2:
	.space 3
	.globl	ivector4
	.align 8
ivector4:
	.space 12
	.globl	ivector5
	.align 32
ivector5:
	.space 400
	.section .rdata,"dr"
.LC0:
	.ascii "Hello\0"
	.align 8
.LC2:
	.ascii "Common subexpression elimination\0"
	.align 8
.LC3:
	.ascii "This line should not be printed\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	call	__main
	movl	$0, i(%rip)
	jmp	.L2
.L3:
	movl	i(%rip), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector(%rip), %rax
	movl	$1, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L2:
	movl	i(%rip), %eax
	cmpl	$2, %eax
	jle	.L3
	movl	$5, i2(%rip)
	movl	$6, j4(%rip)
	movl	j4(%rip), %eax
	movl	%eax, i2(%rip)
	movl	$2, j4(%rip)
	movl	i2(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L4
	movl	i4(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L4
	movl	$2, i2(%rip)
	leaq	.LC0(%rip), %rax
	movq	%rax, %rcx
	call	printf
.L4:
	movl	k5(%rip), %eax
	movl	%eax, j4(%rip)
	movl	i2(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L5
	movl	i4(%rip), %edx
	movl	j4(%rip), %eax
	cmpl	%eax, %edx
	jge	.L5
	movl	$3, i5(%rip)
	leaq	.LC0(%rip), %rax
	movq	%rax, %rcx
	call	printf
.L5:
	movl	$3, i3(%rip)
	movsd	.LC1(%rip), %xmm0
	movsd	%xmm0, flt_1(%rip)
	movl	$5, i2(%rip)
	movl	i(%rip), %eax
	movl	%eax, j2(%rip)
	movl	i(%rip), %eax
	movl	%eax, k2(%rip)
	movl	i(%rip), %eax
	movl	%eax, i4(%rip)
	movl	$0, i5(%rip)
	movl	$1, k3(%rip)
	movl	$1, k3(%rip)
	movl	j5(%rip), %eax
	sall	$2, %eax
	movl	%eax, k2(%rip)
	movl	$0, i(%rip)
	jmp	.L6
.L7:
	movl	i(%rip), %eax
	leal	(%rax,%rax), %edx
	movl	i(%rip), %eax
	movl	%edx, %ecx
	cltq
	leaq	(%rax,%rax), %rdx
	leaq	ivector4(%rip), %rax
	movw	%cx, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L6:
	movl	i(%rip), %eax
	cmpl	$5, %eax
	jle	.L7
	movl	$0, j5(%rip)
	movl	$10000, k5(%rip)
.L8:
	movl	k5(%rip), %eax
	subl	$1, %eax
	movl	%eax, k5(%rip)
	movl	j5(%rip), %eax
	addl	$1, %eax
	movl	%eax, j5(%rip)
	movl	k5(%rip), %edx
	movl	%edx, %eax
	addl	%eax, %eax
	leal	(%rax,%rdx), %ecx
	movl	j5(%rip), %edx
	movl	%edx, %eax
	sall	$2, %eax
	leal	(%rdx,%rax), %r10d
	movl	%ecx, %eax
	cltd
	idivl	%r10d
	movl	%eax, i5(%rip)
	movl	k5(%rip), %eax
	testl	%eax, %eax
	jg	.L8
	movl	$0, i(%rip)
	jmp	.L9
.L10:
	movl	i(%rip), %eax
	addl	%eax, %eax
	addl	$3, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector5(%rip), %rax
	movl	$5, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L9:
	movl	i(%rip), %eax
	cmpl	$99, %eax
	jle	.L10
	movl	i(%rip), %eax
	cmpl	$9, %eax
	jg	.L11
	movl	i5(%rip), %edx
	movl	i2(%rip), %eax
	addl	%edx, %eax
	movl	%eax, j5(%rip)
	jmp	.L12
.L11:
	movl	i5(%rip), %edx
	movl	i2(%rip), %eax
	addl	%edx, %eax
	movl	%eax, k5(%rip)
.L12:
	movl	$1, ivector(%rip)
	movl	i2(%rip), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector(%rip), %rax
	movl	$2, (%rdx,%rax)
	movl	i2(%rip), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	leaq	ivector(%rip), %rax
	movl	$2, (%rdx,%rax)
	movl	$3, 8+ivector(%rip)
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%edx, %eax
	testl	%eax, %eax
	js	.L13
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%edx, %eax
	cmpl	$5, %eax
	jle	.L14
.L13:
	leaq	.LC2(%rip), %rax
	movq	%rax, %rcx
	call	puts
	jmp	.L15
.L14:
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%edx, %eax
	movl	i3(%rip), %ecx
	cltd
	idivl	%ecx
	movl	%eax, m3(%rip)
	movl	h3(%rip), %edx
	movl	k3(%rip), %eax
	addl	%eax, %edx
	movl	i3(%rip), %eax
	addl	%edx, %eax
	movl	%eax, g3(%rip)
.L15:
	movl	$0, i4(%rip)
	jmp	.L16
.L17:
	leaq	.LC0(%rip), %rax
	movq	%rax, %rcx
	call	printf
	movl	j(%rip), %eax
	movl	%eax, %r8d
	movl	k(%rip), %eax
	movl	%eax, %edx
	movl	i4(%rip), %ecx
	movl	%r8d, %eax
	imull	%edx, %eax
	movl	%eax, %edx
	movslq	%ecx, %rax
	leaq	ivector2(%rip), %rcx
	movb	%dl, (%rax,%rcx)
	movl	i4(%rip), %eax
	addl	$1, %eax
	movl	%eax, i4(%rip)
.L16:
	movl	i4(%rip), %eax
	cmpl	$2, %eax
	jle	.L17
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdx
	movl	$1, %ecx
	call	dead_code
	call	unnecessary_loop
	movl	$7, %ecx
	call	loop_jamming
	movl	$7, %ecx
	call	loop_unrolling
	movl	$5, 32(%rsp)
	movl	$4, %r9d
	movl	$3, %r8d
	movl	$2, %edx
	movl	$1, %ecx
	call	jump_compression
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	dead_code
	.def	dead_code;	.scl	2;	.type	32;	.endef
	.seh_proc	dead_code
dead_code:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movq	%rdx, 24(%rbp)
	movl	16(%rbp), %eax
	movl	%eax, -4(%rbp)
	nop
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.globl	unnecessary_loop
	.def	unnecessary_loop;	.scl	2;	.type	32;	.endef
	.seh_proc	unnecessary_loop
unnecessary_loop:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movl	$0, -4(%rbp)
	movl	$0, i(%rip)
	jmp	.L21
.L22:
	movl	j5(%rip), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, k5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L21:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L22
	nop
	nop
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
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
	jmp	.L24
.L25:
	movl	j5(%rip), %edx
	movl	i(%rip), %eax
	imull	%eax, %edx
	movl	16(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, k5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L24:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L25
	movl	$0, i(%rip)
	jmp	.L26
.L27:
	movl	k5(%rip), %eax
	imull	16(%rbp), %eax
	movl	%eax, %edx
	movl	i(%rip), %eax
	imull	%edx, %eax
	movl	%eax, i5(%rip)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L26:
	movl	i(%rip), %eax
	cmpl	$4, %eax
	jle	.L27
	nop
	nop
	popq	%rbp
	ret
	.seh_endproc
	.globl	loop_unrolling
	.def	loop_unrolling;	.scl	2;	.type	32;	.endef
	.seh_proc	loop_unrolling
loop_unrolling:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	$0, i(%rip)
	jmp	.L29
.L30:
	movl	i(%rip), %eax
	cltq
	leaq	(%rax,%rax), %rdx
	leaq	ivector4(%rip), %rax
	movw	$0, (%rdx,%rax)
	movl	i(%rip), %eax
	addl	$1, %eax
	movl	%eax, i(%rip)
.L29:
	movl	i(%rip), %eax
	cmpl	$5, %eax
	jle	.L30
	nop
	nop
	popq	%rbp
	ret
	.seh_endproc
	.globl	jump_compression
	.def	jump_compression;	.scl	2;	.type	32;	.endef
	.seh_proc	jump_compression
jump_compression:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	.seh_endprologue
	movl	%ecx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	%r8d, 32(%rbp)
	movl	%r9d, 40(%rbp)
.L32:
	movl	16(%rbp), %eax
	cmpl	24(%rbp), %eax
	jge	.L33
	movl	24(%rbp), %eax
	cmpl	32(%rbp), %eax
	jge	.L34
	movl	32(%rbp), %eax
	cmpl	40(%rbp), %eax
	jge	.L35
	movl	40(%rbp), %eax
	cmpl	48(%rbp), %eax
	jge	.L40
	movl	48(%rbp), %eax
	addl	%eax, 40(%rbp)
	jmp	.L37
.L35:
	movl	40(%rbp), %eax
	addl	%eax, 32(%rbp)
	jmp	.L37
.L34:
	movl	32(%rbp), %eax
	addl	%eax, 24(%rbp)
	jmp	.L32
.L40:
	nop
.L38:
	jmp	.L32
.L33:
	movl	24(%rbp), %eax
	addl	%eax, 16(%rbp)
.L37:
	movl	16(%rbp), %edx
	movl	24(%rbp), %eax
	addl	%eax, %edx
	movl	32(%rbp), %eax
	addl	%eax, %edx
	movl	40(%rbp), %eax
	addl	%eax, %edx
	movl	48(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC1:
	.long	1717986918
	.long	1075930726
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
	.def	printf;	.scl	2;	.type	32;	.endef
	.def	puts;	.scl	2;	.type	32;	.endef
