	.file	"test6.c"
 # GNU C17 (Rev2, Built by MSYS2 project) version 14.2.0 (x86_64-w64-mingw32)
 #	compiled by GNU C version 14.2.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.27-GMP

 # GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
 # options passed: -mtune=generic -march=nocona -O3 -ffloat-store -fno-defer-pop -fno-inline -finline-functions -fkeep-inline-functions -fno-function-cse -ffast-math -funroll-loops -funroll-all-loops
	.text
	.p2align 4
	.def	fseeko;	.scl	3;	.type	32;	.endef
	.seh_proc	fseeko
fseeko:
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:565:   }
 # C:/msys64/ucrt64/include/stdio.h:564:     return fseek(_File, _Offset, _Origin);
	jmp	fseek	 #
	.seh_endproc
	.p2align 4
	.def	fseeko64;	.scl	3;	.type	32;	.endef
	.seh_proc	fseeko64
fseeko64:
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:568:   }
 # C:/msys64/ucrt64/include/stdio.h:567:     return _fseeki64(_File, _Offset, _Origin);
	rex.W jmp	*__imp__fseeki64(%rip)	 #
	.seh_endproc
	.p2align 4
	.def	ftello;	.scl	3;	.type	32;	.endef
	.seh_proc	ftello
ftello:
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:571:   }
 # C:/msys64/ucrt64/include/stdio.h:570:     return ftell(_File);
	jmp	ftell	 #
	.seh_endproc
	.p2align 4
	.def	ftello64;	.scl	3;	.type	32;	.endef
	.seh_proc	ftello64
ftello64:
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:574:   }
 # C:/msys64/ucrt64/include/stdio.h:573:     return _ftelli64(_File);
	rex.W jmp	*__imp__ftelli64(%rip)	 #
	.seh_endproc
	.p2align 4
	.def	fwscanf;	.scl	3;	.type	32;	.endef
	.seh_proc	fwscanf
fwscanf:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1063:     __builtin_va_start(__ap, _Format);
	leaq	96(%rsp), %rax	 #, tmp102
 # C:/msys64/ucrt64/include/stdio.h:1060:   {
	movq	%r8, 96(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1064:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, _File, _Format, NULL, __ap);
	movq	%rdx, %r8	 # tmp106,
	movq	%rcx, %rdx	 # _File,
	movq	%rax, 32(%rsp)	 # tmp102,
	movl	$2, %ecx	 #,
 # C:/msys64/ucrt64/include/stdio.h:1060:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1064:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, _File, _Format, NULL, __ap);
	xorl	%r9d, %r9d	 #
 # C:/msys64/ucrt64/include/stdio.h:1063:     __builtin_va_start(__ap, _Format);
	movq	%rax, 56(%rsp)	 # tmp102, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1064:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, _File, _Format, NULL, __ap);
	call	__stdio_common_vfwscanf	 #
 # C:/msys64/ucrt64/include/stdio.h:1067:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	swscanf;	.scl	3;	.type	32;	.endef
	.seh_proc	swscanf
swscanf:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1073:     __builtin_va_start(__ap, _Format);
	leaq	96(%rsp), %rax	 #, tmp102
 # C:/msys64/ucrt64/include/stdio.h:1070:   {
	movq	%r8, 96(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1074:     __ret = __stdio_common_vswscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, _Src, (size_t)-1, _Format, NULL, __ap);
	movq	$-1, %r8	 #,
	movq	%rax, 40(%rsp)	 # tmp102,
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1070:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1074:     __ret = __stdio_common_vswscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, _Src, (size_t)-1, _Format, NULL, __ap);
	movq	%rdx, %r9	 # tmp106,
	movq	%rcx, %rdx	 # _Src,
	movl	$2, %ecx	 #,
 # C:/msys64/ucrt64/include/stdio.h:1073:     __builtin_va_start(__ap, _Format);
	movq	%rax, 56(%rsp)	 # tmp102, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1074:     __ret = __stdio_common_vswscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, _Src, (size_t)-1, _Format, NULL, __ap);
	call	__stdio_common_vswscanf	 #
 # C:/msys64/ucrt64/include/stdio.h:1077:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	wscanf;	.scl	3;	.type	32;	.endef
	.seh_proc	wscanf
wscanf:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1083:     __builtin_va_start(__ap, _Format);
	leaq	104(%rsp), %rsi	 #, tmp102
 # C:/msys64/ucrt64/include/stdio.h:1080:   {
	movq	%rdx, 104(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp105, _Format
 # C:/msys64/ucrt64/include/stdio.h:1084:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, _Format, NULL, __ap);
	xorl	%ecx, %ecx	 #
 # C:/msys64/ucrt64/include/stdio.h:1080:   {
	movq	%r8, 112(%rsp)	 #,
	movq	%r9, 120(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1083:     __builtin_va_start(__ap, _Format);
	movq	%rsi, 56(%rsp)	 # tmp102, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1084:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, _Format, NULL, __ap);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/stdio.h:1084:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, _Format, NULL, __ap);
	movq	%rsi, 32(%rsp)	 # tmp102,
	xorl	%r9d, %r9d	 #
	movq	%rbx, %r8	 # _Format,
 # C:/msys64/ucrt64/include/stdio.h:1084:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, _Format, NULL, __ap);
	movq	%rax, %rdx	 # tmp106, _2
 # C:/msys64/ucrt64/include/stdio.h:1084:     __ret = __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, _Format, NULL, __ap);
	movl	$2, %ecx	 #,
	call	__stdio_common_vfwscanf	 #
 # C:/msys64/ucrt64/include/stdio.h:1087:   }
	addq	$72, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	vfwscanf;	.scl	3;	.type	32;	.endef
	.seh_proc	vfwscanf
vfwscanf:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1092:     return __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, __stream, __format, NULL, __local_argv);
	xorl	%r9d, %r9d	 #
	movq	%r8, 32(%rsp)	 # tmp105,
	movq	%rdx, %r8	 # tmp104,
	movq	%rcx, %rdx	 # __stream,
	movl	$2, %ecx	 #,
	call	__stdio_common_vfwscanf	 #
 # C:/msys64/ucrt64/include/stdio.h:1093:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vswscanf;	.scl	3;	.type	32;	.endef
	.seh_proc	vswscanf
vswscanf:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1099:     return __stdio_common_vswscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, __source, (size_t)-1, __format, NULL, __local_argv);
	movq	%r8, 40(%rsp)	 # tmp105,
 # C:/msys64/ucrt64/include/stdio.h:1098:   {
	movq	%rdx, %r9	 # __format, tmp104
 # C:/msys64/ucrt64/include/stdio.h:1099:     return __stdio_common_vswscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, __source, (size_t)-1, __format, NULL, __local_argv);
	movq	$-1, %r8	 #,
	movq	%rcx, %rdx	 # __source,
	movq	$0, 32(%rsp)	 #,
	movl	$2, %ecx	 #,
	call	__stdio_common_vswscanf	 #
 # C:/msys64/ucrt64/include/stdio.h:1100:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vwscanf;	.scl	3;	.type	32;	.endef
	.seh_proc	vwscanf
vwscanf:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1104:   {
	movq	%rdx, %rsi	 # tmp105, __local_argv
	movq	%rcx, %rbx	 # tmp104, __format
 # C:/msys64/ucrt64/include/stdio.h:1105:     return __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, __format, NULL, __local_argv);
	xorl	%ecx, %ecx	 #
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/stdio.h:1105:     return __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, __format, NULL, __local_argv);
	movq	%rsi, 32(%rsp)	 # __local_argv,
	xorl	%r9d, %r9d	 #
	movq	%rbx, %r8	 # __format,
 # C:/msys64/ucrt64/include/stdio.h:1105:     return __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, __format, NULL, __local_argv);
	movq	%rax, %rdx	 # tmp106, _1
 # C:/msys64/ucrt64/include/stdio.h:1105:     return __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS, stdin, __format, NULL, __local_argv);
	movl	$2, %ecx	 #,
	call	__stdio_common_vfwscanf	 #
 # C:/msys64/ucrt64/include/stdio.h:1106:   }
	addq	$56, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	wprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	wprintf
wprintf:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1114:     __builtin_va_start(__ap, _Format);
	leaq	104(%rsp), %rsi	 #, tmp102
 # C:/msys64/ucrt64/include/stdio.h:1111:   {
	movq	%rdx, 104(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp105, _Format
 # C:/msys64/ucrt64/include/stdio.h:1115:     __ret = __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, __ap);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/stdio.h:1111:   {
	movq	%r8, 112(%rsp)	 #,
	movq	%r9, 120(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1114:     __builtin_va_start(__ap, _Format);
	movq	%rsi, 56(%rsp)	 # tmp102, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1115:     __ret = __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, __ap);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/stdio.h:1115:     __ret = __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, __ap);
	movq	%rsi, 32(%rsp)	 # tmp102,
	xorl	%r9d, %r9d	 #
	movq	%rbx, %r8	 # _Format,
 # C:/msys64/ucrt64/include/stdio.h:1115:     __ret = __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, __ap);
	movq	%rax, %rdx	 # tmp106, _2
 # C:/msys64/ucrt64/include/stdio.h:1115:     __ret = __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, __ap);
	movl	$36, %ecx	 #,
	call	__stdio_common_vfwprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1118:   }
	addq	$72, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	vfwprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	vfwprintf
vfwprintf:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1122:     return __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _File, _Format, NULL, _ArgList);
	xorl	%r9d, %r9d	 #
	movq	%r8, 32(%rsp)	 # tmp105,
	movq	%rdx, %r8	 # tmp104,
	movq	%rcx, %rdx	 # _File,
	movl	$36, %ecx	 #,
	call	__stdio_common_vfwprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1123:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vwprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	vwprintf
vwprintf:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1126:   {
	movq	%rdx, %rsi	 # tmp105, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/stdio.h:1127:     return __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/stdio.h:1127:     return __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, _ArgList);
	movq	%rsi, 32(%rsp)	 # _ArgList,
	xorl	%r9d, %r9d	 #
	movq	%rbx, %r8	 # _Format,
 # C:/msys64/ucrt64/include/stdio.h:1127:     return __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, _ArgList);
	movq	%rax, %rdx	 # tmp106, _1
 # C:/msys64/ucrt64/include/stdio.h:1127:     return __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, stdout, _Format, NULL, _ArgList);
	movl	$36, %ecx	 #,
	call	__stdio_common_vfwprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1128:   }
	addq	$56, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_scwprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	_scwprintf
_scwprintf:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1190:     __builtin_va_start(__ap, _Format);
	leaq	88(%rsp), %rax	 #, tmp101
 # C:/msys64/ucrt64/include/stdio.h:1187:   {
	movq	%rdx, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1191:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, NULL, __ap);
	xorl	%edx, %edx	 #
	movq	%rax, 40(%rsp)	 # tmp101,
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1187:   {
	movq	%r8, 96(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1191:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, NULL, __ap);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/stdio.h:1187:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1191:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, NULL, __ap);
	movq	%rcx, %r9	 # tmp104,
	movl	$38, %ecx	 #,
 # C:/msys64/ucrt64/include/stdio.h:1190:     __builtin_va_start(__ap, _Format);
	movq	%rax, 56(%rsp)	 # tmp101, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1191:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, NULL, __ap);
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1194:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	swprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	swprintf
swprintf:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1212:     if (_Dest == NULL && _Count == 0)
	movq	%rcx, %rax	 # _Dest, tmp115
 # C:/msys64/ucrt64/include/stdio.h:1201:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1212:     if (_Dest == NULL && _Count == 0)
	orq	%rdx, %rax	 # _Count, tmp115
	je	.L18	 #,
 # C:/msys64/ucrt64/include/stdio.h:1215:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, _Count, _Format, NULL, __ap);
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1214:     __builtin_va_start(__ap, _Format);
	leaq	104(%rsp), %r9	 #, tmp107
 # C:/msys64/ucrt64/include/stdio.h:1215:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, _Count, _Format, NULL, __ap);
	movq	%r9, 40(%rsp)	 # tmp107,
 # C:/msys64/ucrt64/include/stdio.h:1214:     __builtin_va_start(__ap, _Format);
	movq	%r9, 56(%rsp)	 # tmp107, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1215:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, _Count, _Format, NULL, __ap);
	movq	%r8, %r9	 # _Format,
	movq	%rdx, %r8	 # _Count,
	movq	%rcx, %rdx	 # _Dest,
	movl	$36, %ecx	 #,
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1217:     return __ret < 0 ? -1 : __ret;
	movl	$-1, %edx	 #, tmp114
	testl	%eax, %eax	 # tmp113
	cmovs	%edx, %eax	 # tmp113,, tmp114, <retval>
.L16:
 # C:/msys64/ucrt64/include/stdio.h:1218:   }
	addq	$72, %rsp	 #,
	ret	
	.p2align 4,,10
	.p2align 3
.L18:
 # C:/msys64/ucrt64/include/stdio.h:1213:       return -1;
	movl	$-1, %eax	 #, <retval>
	jmp	.L16	 #
	.seh_endproc
	.p2align 4
	.def	vswprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	vswprintf
vswprintf:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1231:     if (_Dest == NULL && _Count == 0)
	movq	%rcx, %rax	 # _Dest, tmp114
	orq	%rdx, %rax	 # _Count, tmp114
	je	.L21	 #,
 # C:/msys64/ucrt64/include/stdio.h:1233:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, _Count, _Format, NULL, _Args);
	movq	%r9, 40(%rsp)	 # _Args,
	movq	%r8, %r9	 # _Format,
	movq	%rdx, %r8	 # _Count,
	movq	%rcx, %rdx	 # _Dest,
	movq	$0, 32(%rsp)	 #,
	movl	$36, %ecx	 #,
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1234:     return __ret < 0 ? -1 : __ret;
	movl	$-1, %edx	 #, tmp113
	testl	%eax, %eax	 # tmp112
	cmovs	%edx, %eax	 # tmp112,, tmp113, <retval>
.L19:
 # C:/msys64/ucrt64/include/stdio.h:1235:   }
	addq	$56, %rsp	 #,
	ret	
	.p2align 4,,10
	.p2align 3
.L21:
 # C:/msys64/ucrt64/include/stdio.h:1232:       return -1;
	movl	$-1, %eax	 #, <retval>
	jmp	.L19	 #
	.seh_endproc
	.p2align 4
	.def	snwprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	snwprintf
snwprintf:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1242:     __builtin_va_start(__ap, format);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/stdio.h:1239:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1243:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, s, n, format, NULL, __ap);
	movq	%r8, %r9	 # tmp108,
	movq	%rdx, %r8	 # tmp107,
	movq	%rax, 40(%rsp)	 # tmp103,
	movq	%rcx, %rdx	 # s,
	movl	$38, %ecx	 #,
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1242:     __builtin_va_start(__ap, format);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1243:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, s, n, format, NULL, __ap);
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1246:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vsnwprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	vsnwprintf
vsnwprintf:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1250:     return __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, s, n, format, NULL, arg);
	movq	%r9, 40(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # s,
	movq	$0, 32(%rsp)	 #,
	movl	$38, %ecx	 #,
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1251:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_swprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	_swprintf
_swprintf:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1259:     __builtin_va_start(__ap, _Format);
	leaq	96(%rsp), %rax	 #, tmp102
 # C:/msys64/ucrt64/include/stdio.h:1256:   {
	movq	%r8, 96(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1260:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, (size_t)-1, _Format, NULL, __ap);
	movq	$-1, %r8	 #,
	movq	%rax, 40(%rsp)	 # tmp102,
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1256:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/stdio.h:1260:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, (size_t)-1, _Format, NULL, __ap);
	movq	%rdx, %r9	 # tmp106,
	movq	%rcx, %rdx	 # _Dest,
	movl	$36, %ecx	 #,
 # C:/msys64/ucrt64/include/stdio.h:1259:     __builtin_va_start(__ap, _Format);
	movq	%rax, 56(%rsp)	 # tmp102, MEM[(char * *)&__ap]
 # C:/msys64/ucrt64/include/stdio.h:1260:     __ret = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, (size_t)-1, _Format, NULL, __ap);
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1263:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vswprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	_vswprintf
_vswprintf:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1267:     return __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, (size_t)-1, _Format, NULL, _Args);
	movq	%r8, 40(%rsp)	 # tmp105,
 # C:/msys64/ucrt64/include/stdio.h:1266:   {
	movq	%rdx, %r9	 # _Format, tmp104
 # C:/msys64/ucrt64/include/stdio.h:1267:     return __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Dest, (size_t)-1, _Format, NULL, _Args);
	movq	$-1, %r8	 #,
	movq	%rcx, %rdx	 # _Dest,
	movq	$0, 32(%rsp)	 #,
	movl	$36, %ecx	 #,
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1268:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vscwprintf;	.scl	3;	.type	32;	.endef
	.seh_proc	_vscwprintf
_vscwprintf:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/stdio.h:1273:       int _Result = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
	movq	%rdx, 40(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/stdio.h:1272:   {
	movq	%rcx, %r9	 # _Format, tmp103
 # C:/msys64/ucrt64/include/stdio.h:1273:       int _Result = __stdio_common_vswprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, NULL, _ArgList);
	xorl	%edx, %edx	 #
	movl	$38, %ecx	 #,
	movq	$0, 32(%rsp)	 #,
	call	__stdio_common_vswprintf	 #
 # C:/msys64/ucrt64/include/stdio.h:1274:       return _Result < 0 ? -1 : _Result;
	movl	$-1, %edx	 #, tmp106
	testl	%eax, %eax	 # tmp105
	cmovs	%edx, %eax	 # tmp105,, tmp106, _6
 # C:/msys64/ucrt64/include/stdio.h:1275:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfscanf_s_l
_vfscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:42:     return __stdio_common_vfscanf(_CRT_INTERNAL_SCANF_SECURECRT, _File, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # _File,
	movl	$1, %ecx	 #,
	call	__stdio_common_vfscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:43:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vfscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vfscanf_s
vfscanf_s:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:47:     return _vfscanf_s_l(_File, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp104,
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:48:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:47:     return _vfscanf_s_l(_File, _Format, NULL, _ArgList);
	jmp	_vfscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_vscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vscanf_s_l
_vscanf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$32, %rsp	 #,
	.seh_stackalloc	32
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:51:   {
	movq	%rdx, %rsi	 # tmp105, _Locale
	movq	%r8, %rdi	 # tmp106, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:52:     return _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	xorl	%ecx, %ecx	 #
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:52:     return _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # _ArgList,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:52:     return _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp107, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:53:   }
	addq	$32, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:52:     return _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	jmp	_vfscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	vscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vscanf_s
vscanf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$40, %rsp	 #,
	.seh_stackalloc	40
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:56:   {
	movq	%rdx, %rsi	 # tmp104, _ArgList
	movq	%rcx, %rbx	 # tmp103, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:57:     return _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	xorl	%ecx, %ecx	 #
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:57:     return _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # _ArgList,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:57:     return _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp105, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:58:   }
	addq	$40, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:57:     return _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	jmp	_vfscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_fscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_fscanf_s_l
_fscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:61:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:64:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:65:     _Ret = _vfscanf_s_l(_File, _Format, _Locale, _ArgList);
	call	_vfscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:68:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	fscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	fscanf_s
fscanf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:71:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:75:     _Ret = _vfscanf_s_l(_File, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:71:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:74:     __builtin_va_start(_ArgList, _Format);
	leaq	80(%rsp), %r9	 #, tmp102
	movq	%r9, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:75:     _Ret = _vfscanf_s_l(_File, _Format, NULL, _ArgList);
	call	_vfscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:78:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_scanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_scanf_s_l
_scanf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$48, %rsp	 #,
	.seh_stackalloc	48
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:84:     __builtin_va_start(_ArgList, _Locale);
	leaq	96(%rsp), %rdi	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:81:   {
	movq	%rdx, %rsi	 # tmp107, _Locale
	movq	%r8, 96(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp106, _Format
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:85:     _Ret = _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	xorl	%ecx, %ecx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:84:     __builtin_va_start(_ArgList, _Locale);
	movq	%rdi, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:85:     _Ret = _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:85:     _Ret = _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # tmp103,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:85:     _Ret = _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp108, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:85:     _Ret = _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	call	_vfscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:88:   }
	addq	$48, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	scanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	scanf_s
scanf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:94:     __builtin_va_start(_ArgList, _Format);
	leaq	88(%rsp), %rsi	 #, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:91:   {
	movq	%rdx, 88(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp105, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:95:     _Ret = _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	xorl	%ecx, %ecx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:91:   {
	movq	%r8, 96(%rsp)	 #,
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:94:     __builtin_va_start(_ArgList, _Format);
	movq	%rsi, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:95:     _Ret = _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:95:     _Ret = _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # tmp102,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:95:     _Ret = _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp106, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:95:     _Ret = _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	call	_vfscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:98:   }
	addq	$56, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfscanf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfscanf_l
_vfscanf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:102:     return __stdio_common_vfscanf(0, _File, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # _File,
	xorl	%ecx, %ecx	 #
	call	__stdio_common_vfscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:103:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vscanf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vscanf_l
_vscanf_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$32, %rsp	 #,
	.seh_stackalloc	32
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:106:   {
	movq	%rdx, %rsi	 # tmp105, _Locale
	movq	%r8, %rdi	 # tmp106, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:107:     return _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	xorl	%ecx, %ecx	 #
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:107:     return _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # _ArgList,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:107:     return _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp107, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:108:   }
	addq	$32, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:107:     return _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	jmp	_vfscanf_l	 #
	.seh_endproc
	.p2align 4
	.def	_fscanf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_fscanf_l
_fscanf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:111:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:114:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:115:     _Ret = _vfscanf_l(_File, _Format, _Locale, _ArgList);
	call	_vfscanf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:118:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_scanf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_scanf_l
_scanf_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$48, %rsp	 #,
	.seh_stackalloc	48
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:123:     __builtin_va_start(_ArgList, _Locale);
	leaq	96(%rsp), %rdi	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:120:   {
	movq	%rdx, %rsi	 # tmp107, _Locale
	movq	%r8, 96(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp106, _Format
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:124:     _Ret = _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	xorl	%ecx, %ecx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:123:     __builtin_va_start(_ArgList, _Locale);
	movq	%rdi, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:124:     _Ret = _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:124:     _Ret = _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # tmp103,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:124:     _Ret = _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp108, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:124:     _Ret = _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	call	_vfscanf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:127:   }
	addq	$48, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsscanf_s_l
_vsscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:131:     return __stdio_common_vsscanf(_CRT_INTERNAL_SCANF_SECURECRT, _Src, (size_t)-1, _Format, _Locale, _ArgList);
	movq	%r9, 40(%rsp)	 # tmp107,
	movq	%rdx, %r9	 # tmp105,
	movq	%rcx, %rdx	 # _Src,
	movl	$1, %ecx	 #,
	movq	%r8, 32(%rsp)	 # tmp106,
	movq	$-1, %r8	 #,
	call	__stdio_common_vsscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:132:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vsscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vsscanf_s
vsscanf_s:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:135:     return _vsscanf_s_l(_Src, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp104,
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:136:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:135:     return _vsscanf_s_l(_Src, _Format, NULL, _ArgList);
	jmp	_vsscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_sscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_sscanf_s_l
_sscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:138:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:141:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:142:     _Ret = _vsscanf_s_l(_Src, _Format, _Locale, _ArgList);
	call	_vsscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:145:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	sscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	sscanf_s
sscanf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:147:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:151:     _Ret = _vsscanf_s_l(_Src, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:147:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:150:     __builtin_va_start(_ArgList, _Format);
	leaq	80(%rsp), %r9	 #, tmp102
	movq	%r9, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:151:     _Ret = _vsscanf_s_l(_Src, _Format, NULL, _ArgList);
	call	_vsscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:154:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsscanf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsscanf_l
_vsscanf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:158:     return __stdio_common_vsscanf(0, _Src, (size_t)-1, _Format, _Locale, _ArgList);
	movq	%r9, 40(%rsp)	 # tmp107,
	movq	%rdx, %r9	 # tmp105,
	movq	%rcx, %rdx	 # _Src,
	xorl	%ecx, %ecx	 #
	movq	%r8, 32(%rsp)	 # tmp106,
	movq	$-1, %r8	 #,
	call	__stdio_common_vsscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:159:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_sscanf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_sscanf_l
_sscanf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:161:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:164:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:165:     _Ret = _vsscanf_l(_Src, _Format, _Locale, _ArgList);
	call	_vsscanf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:168:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_snscanf_s_l
_snscanf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:175:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:176:     _Ret = __stdio_common_vsscanf(_CRT_INTERNAL_SCANF_SECURECRT, _Src, _MaxCount, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp110,
	movq	%r8, %r9	 # tmp109,
	movq	%rdx, %r8	 # tmp108,
	movq	%rax, 40(%rsp)	 # tmp104,
	movq	%rcx, %rdx	 # _Src,
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:175:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:176:     _Ret = __stdio_common_vsscanf(_CRT_INTERNAL_SCANF_SECURECRT, _Src, _MaxCount, _Format, _Locale, _ArgList);
	call	__stdio_common_vsscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:179:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	_snscanf_s
_snscanf_s:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:184:     __builtin_va_start(_ArgList, _Format);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:181:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:185:     _Ret = __stdio_common_vsscanf(_CRT_INTERNAL_SCANF_SECURECRT, _Src, _MaxCount, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp108,
	movq	%rdx, %r8	 # tmp107,
	movq	%rax, 40(%rsp)	 # tmp103,
	movq	%rcx, %rdx	 # _Src,
	movl	$1, %ecx	 #,
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:184:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:185:     _Ret = __stdio_common_vsscanf(_CRT_INTERNAL_SCANF_SECURECRT, _Src, _MaxCount, _Format, NULL, _ArgList);
	call	__stdio_common_vsscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:188:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snscanf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_snscanf_l
_snscanf_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:195:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:196:     _Ret = __stdio_common_vsscanf(0, _Src, _MaxCount, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp110,
	movq	%r8, %r9	 # tmp109,
	movq	%rdx, %r8	 # tmp108,
	movq	%rax, 40(%rsp)	 # tmp104,
	movq	%rcx, %rdx	 # _Src,
	xorl	%ecx, %ecx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:195:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:196:     _Ret = __stdio_common_vsscanf(0, _Src, _MaxCount, _Format, _Locale, _ArgList);
	call	__stdio_common_vsscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:199:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfprintf_s_l
_vfprintf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:204:     return __stdio_common_vfprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _File, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # _File,
	movl	$36, %ecx	 #,
	call	__stdio_common_vfprintf_s	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:205:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vfprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vfprintf_s
vfprintf_s:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:208:     return _vfprintf_s_l(_File, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp104,
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:209:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:208:     return _vfprintf_s_l(_File, _Format, NULL, _ArgList);
	jmp	_vfprintf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_vprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vprintf_s_l
_vprintf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$32, %rsp	 #,
	.seh_stackalloc	32
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:211:   {
	movq	%rdx, %rsi	 # tmp105, _Locale
	movq	%r8, %rdi	 # tmp106, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:212:     return _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:212:     return _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # _ArgList,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:212:     return _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp107, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:213:   }
	addq	$32, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:212:     return _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	jmp	_vfprintf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	vprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vprintf_s
vprintf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$40, %rsp	 #,
	.seh_stackalloc	40
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:215:   {
	movq	%rdx, %rsi	 # tmp104, _ArgList
	movq	%rcx, %rbx	 # tmp103, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:216:     return _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:216:     return _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # _ArgList,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:216:     return _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp105, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:217:   }
	addq	$40, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:216:     return _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	jmp	_vfprintf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_fprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_fprintf_s_l
_fprintf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:219:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:222:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:223:     _Ret = _vfprintf_s_l(_File, _Format, _Locale, _ArgList);
	call	_vfprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:226:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_printf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_printf_s_l
_printf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$48, %rsp	 #,
	.seh_stackalloc	48
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:231:     __builtin_va_start(_ArgList, _Locale);
	leaq	96(%rsp), %rdi	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:228:   {
	movq	%rdx, %rsi	 # tmp107, _Locale
	movq	%r8, 96(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp106, _Format
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:232:     _Ret = _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:231:     __builtin_va_start(_ArgList, _Locale);
	movq	%rdi, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:232:     _Ret = _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:232:     _Ret = _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # tmp103,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:232:     _Ret = _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp108, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:232:     _Ret = _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	call	_vfprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:235:   }
	addq	$48, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	fprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	fprintf_s
fprintf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:237:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:241:     _Ret = _vfprintf_s_l(_File, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:237:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:240:     __builtin_va_start(_ArgList, _Format);
	leaq	80(%rsp), %r9	 #, tmp102
	movq	%r9, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:241:     _Ret = _vfprintf_s_l(_File, _Format, NULL, _ArgList);
	call	_vfprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:244:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	printf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	printf_s
printf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:249:     __builtin_va_start(_ArgList, _Format);
	leaq	88(%rsp), %rsi	 #, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:246:   {
	movq	%rdx, 88(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp105, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:250:     _Ret = _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:246:   {
	movq	%r8, 96(%rsp)	 #,
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:249:     __builtin_va_start(_ArgList, _Format);
	movq	%rsi, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:250:     _Ret = _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:250:     _Ret = _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # tmp102,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:250:     _Ret = _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp106, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:250:     _Ret = _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	call	_vfprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:253:   }
	addq	$56, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnprintf_c_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnprintf_c_l
_vsnprintf_c_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:257:     return __stdio_common_vsprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	%r9, 32(%rsp)	 # tmp108,
	movq	%r8, %r9	 # tmp107,
	movq	%rdx, %r8	 # tmp106,
	movq	%rcx, %rdx	 # _DstBuf,
	movq	%rax, 40(%rsp)	 # _ArgList,
	movl	$36, %ecx	 #,
	call	__stdio_common_vsprintf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:258:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnprintf_c;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnprintf_c
_vsnprintf_c:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:261:     return _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, NULL, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	xorl	%r9d, %r9d	 #
	call	_vsnprintf_c_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:262:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snprintf_c_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_snprintf_c_l
_snprintf_c_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:267:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:268:     _Ret = _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	movq	%rax, 32(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:267:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:268:     _Ret = _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	call	_vsnprintf_c_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:271:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snprintf_c;	.scl	3;	.type	32;	.endef
	.seh_proc	_snprintf_c
_snprintf_c:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:276:     __builtin_va_start(_ArgList, _Format);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:273:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:277:     _Ret = _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, NULL, _ArgList);
	xorl	%r9d, %r9d	 #
	movq	%rax, 32(%rsp)	 # tmp103,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:276:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:277:     _Ret = _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, NULL, _ArgList);
	call	_vsnprintf_c_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:280:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnprintf_s_l
_vsnprintf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:284:     return __stdio_common_vsnprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	movq	120(%rsp), %rax	 # _ArgList, _ArgList
	movq	112(%rsp), %r10	 # _Locale, _Locale
	movq	%r9, 32(%rsp)	 # tmp109,
	movq	%r8, %r9	 # tmp108,
	movq	%rdx, %r8	 # tmp107,
	movq	%rcx, %rdx	 # _DstBuf,
	movq	%rax, 48(%rsp)	 # _ArgList,
	movl	$36, %ecx	 #,
	movq	%r10, 40(%rsp)	 # _Locale,
	call	__stdio_common_vsnprintf_s	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:285:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vsnprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vsnprintf_s
vsnprintf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:288:     return _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	$0, 32(%rsp)	 #,
	movq	%rax, 40(%rsp)	 # _ArgList,
	call	_vsnprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:289:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnprintf_s
_vsnprintf_s:
	.seh_endprologue
	jmp	vsnprintf_s	 #
	.seh_endproc
	.p2align 4
	.def	_snprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_snprintf_s_l
_snprintf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:299:     _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	movq	112(%rsp), %r10	 # _Locale, _Locale
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:298:     __builtin_va_start(_ArgList, _Locale);
	leaq	120(%rsp), %rax	 #, tmp105
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:299:     _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	movq	%r10, 32(%rsp)	 # _Locale,
	movq	%rax, 40(%rsp)	 # tmp105,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:298:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp105, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:299:     _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	call	_vsnprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:302:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	_snprintf_s
_snprintf_s:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:308:     _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:307:     __builtin_va_start(_ArgList, _Format);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:308:     _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	movq	%rax, 40(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:307:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:308:     _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	call	_vsnprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:311:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsprintf_s_l
_vsprintf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:315:     return __stdio_common_vsprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _DstBuf, _DstSize, _Format, _Locale, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	%r9, 32(%rsp)	 # tmp108,
	movq	%r8, %r9	 # tmp107,
	movq	%rdx, %r8	 # tmp106,
	movq	%rcx, %rdx	 # _DstBuf,
	movq	%rax, 40(%rsp)	 # _ArgList,
	movl	$36, %ecx	 #,
	call	__stdio_common_vsprintf_s	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:316:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vsprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vsprintf_s
vsprintf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:319:     return _vsprintf_s_l(_DstBuf, _Size, _Format, NULL, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	xorl	%r9d, %r9d	 #
	call	_vsprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:320:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_sprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_sprintf_s_l
_sprintf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:325:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:326:     _Ret = _vsprintf_s_l(_DstBuf, _DstSize, _Format, _Locale, _ArgList);
	movq	%rax, 32(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:325:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:326:     _Ret = _vsprintf_s_l(_DstBuf, _DstSize, _Format, _Locale, _ArgList);
	call	_vsprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:329:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	sprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	sprintf_s
sprintf_s:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:334:     __builtin_va_start(_ArgList, _Format);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:331:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:335:     _Ret = _vsprintf_s_l(_DstBuf, _DstSize, _Format, NULL, _ArgList);
	xorl	%r9d, %r9d	 #
	movq	%rax, 32(%rsp)	 # tmp103,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:334:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:335:     _Ret = _vsprintf_s_l(_DstBuf, _DstSize, _Format, NULL, _ArgList);
	call	_vsprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:338:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfprintf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfprintf_p_l
_vfprintf_p_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:342:     return __stdio_common_vfprintf_p(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _File, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # _File,
	movl	$36, %ecx	 #,
	call	__stdio_common_vfprintf_p	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:343:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfprintf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfprintf_p
_vfprintf_p:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:346:     return _vfprintf_p_l(_File, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp104,
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:347:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:346:     return _vfprintf_p_l(_File, _Format, NULL, _ArgList);
	jmp	_vfprintf_p_l	 #
	.seh_endproc
	.p2align 4
	.def	_vprintf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vprintf_p_l
_vprintf_p_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$32, %rsp	 #,
	.seh_stackalloc	32
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:349:   {
	movq	%rdx, %rsi	 # tmp105, _Locale
	movq	%r8, %rdi	 # tmp106, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:350:     return _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:350:     return _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # _ArgList,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:350:     return _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp107, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:351:   }
	addq	$32, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:350:     return _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	jmp	_vfprintf_p_l	 #
	.seh_endproc
	.p2align 4
	.def	_vprintf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_vprintf_p
_vprintf_p:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$40, %rsp	 #,
	.seh_stackalloc	40
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:353:   {
	movq	%rdx, %rsi	 # tmp104, _ArgList
	movq	%rcx, %rbx	 # tmp103, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:354:     return _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:354:     return _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # _ArgList,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:354:     return _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp105, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:355:   }
	addq	$40, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:354:     return _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	jmp	_vfprintf_p_l	 #
	.seh_endproc
	.p2align 4
	.def	_fprintf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_fprintf_p_l
_fprintf_p_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:360:     __builtin_va_start(_ArgList, _Locale);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:357:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:361:     _Ret = __stdio_common_vfprintf_p(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _File, _Format, _Locale, _ArgList);
	movq	%r8, %r9	 # tmp108,
	movq	%rdx, %r8	 # tmp107,
	movq	%rax, 32(%rsp)	 # tmp103,
	movq	%rcx, %rdx	 # _File,
	movl	$36, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:360:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:361:     _Ret = __stdio_common_vfprintf_p(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _File, _Format, _Locale, _ArgList);
	call	__stdio_common_vfprintf_p	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:364:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_fprintf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_fprintf_p
_fprintf_p:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:366:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:370:     _Ret = _vfprintf_p_l(_File, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:366:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:369:     __builtin_va_start(_ArgList, _Format);
	leaq	80(%rsp), %r9	 #, tmp102
	movq	%r9, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:370:     _Ret = _vfprintf_p_l(_File, _Format, NULL, _ArgList);
	call	_vfprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:373:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_printf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_printf_p_l
_printf_p_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$48, %rsp	 #,
	.seh_stackalloc	48
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:378:     __builtin_va_start(_ArgList, _Locale);
	leaq	96(%rsp), %rdi	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:375:   {
	movq	%rdx, %rsi	 # tmp107, _Locale
	movq	%r8, 96(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp106, _Format
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:379:     _Ret = _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:378:     __builtin_va_start(_ArgList, _Locale);
	movq	%rdi, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:379:     _Ret = _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:379:     _Ret = _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # tmp103,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:379:     _Ret = _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp108, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:379:     _Ret = _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	call	_vfprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:382:   }
	addq	$48, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_printf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_printf_p
_printf_p:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:387:     __builtin_va_start(_ArgList, _Format);
	leaq	88(%rsp), %rsi	 #, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:384:   {
	movq	%rdx, 88(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp105, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:388:     _Ret = _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:384:   {
	movq	%r8, 96(%rsp)	 #,
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:387:     __builtin_va_start(_ArgList, _Format);
	movq	%rsi, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:388:     _Ret = _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:388:     _Ret = _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # tmp102,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:388:     _Ret = _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp106, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:388:     _Ret = _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	call	_vfprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:391:   }
	addq	$56, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsprintf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsprintf_p_l
_vsprintf_p_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:395:     return __stdio_common_vsprintf_p(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	%r9, 32(%rsp)	 # tmp108,
	movq	%r8, %r9	 # tmp107,
	movq	%rdx, %r8	 # tmp106,
	movq	%rcx, %rdx	 # _DstBuf,
	movq	%rax, 40(%rsp)	 # _ArgList,
	movl	$36, %ecx	 #,
	call	__stdio_common_vsprintf_p	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:396:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsprintf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsprintf_p
_vsprintf_p:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:399:     return _vsprintf_p_l(_Dst, _MaxCount, _Format, NULL, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	xorl	%r9d, %r9d	 #
	call	_vsprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:400:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_sprintf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_sprintf_p_l
_sprintf_p_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:405:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:406:     _Ret = _vsprintf_p_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	movq	%rax, 32(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:405:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:406:     _Ret = _vsprintf_p_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	call	_vsprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:409:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_sprintf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_sprintf_p
_sprintf_p:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:414:     __builtin_va_start(_ArgList, _Format);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:411:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:415:     _Ret = _vsprintf_p_l(_Dst, _MaxCount, _Format, NULL, _ArgList);
	xorl	%r9d, %r9d	 #
	movq	%rax, 32(%rsp)	 # tmp103,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:414:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:415:     _Ret = _vsprintf_p_l(_Dst, _MaxCount, _Format, NULL, _ArgList);
	call	_vsprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:418:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vscprintf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vscprintf_p_l
_vscprintf_p_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:422:     return __stdio_common_vsprintf_p(_CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, _Locale, _ArgList);
	movq	%r8, 40(%rsp)	 # tmp105,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:421:   {
	movq	%rcx, %r9	 # _Format, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:422:     return __stdio_common_vsprintf_p(_CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, _Locale, _ArgList);
	xorl	%r8d, %r8d	 #
	movl	$2, %ecx	 #,
	movq	%rdx, 32(%rsp)	 # tmp104,
	xorl	%edx, %edx	 #
	call	__stdio_common_vsprintf_p	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:423:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vscprintf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_vscprintf_p
_vscprintf_p:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:425:   {
	movq	%rdx, %r8	 # _ArgList, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:426:     return _vscprintf_p_l(_Format, NULL, _ArgList);
	xorl	%edx, %edx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:427:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:426:     return _vscprintf_p_l(_Format, NULL, _ArgList);
	jmp	_vscprintf_p_l	 #
	.seh_endproc
	.p2align 4
	.def	_scprintf_p_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_scprintf_p_l
_scprintf_p_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:429:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:432:     __builtin_va_start(_ArgList, _Locale);
	leaq	80(%rsp), %r8	 #, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:429:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:432:     __builtin_va_start(_ArgList, _Locale);
	movq	%r8, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:433:     _Ret = _vscprintf_p_l(_Format, _Locale, _ArgList);
	call	_vscprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:436:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_scprintf_p;	.scl	3;	.type	32;	.endef
	.seh_proc	_scprintf_p
_scprintf_p:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:438:   {
	movq	%rdx, 72(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:442:     _Ret = _vscprintf_p_l(_Format, NULL, _ArgList);
	xorl	%edx, %edx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:438:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:441:     __builtin_va_start(_ArgList, _Format);
	leaq	72(%rsp), %r8	 #, tmp101
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:438:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:441:     __builtin_va_start(_ArgList, _Format);
	movq	%r8, 40(%rsp)	 # tmp101, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:442:     _Ret = _vscprintf_p_l(_Format, NULL, _ArgList);
	call	_vscprintf_p_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:445:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfprintf_l
_vfprintf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:449:     return __stdio_common_vfprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _File, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # _File,
	movl	$36, %ecx	 #,
	call	__stdio_common_vfprintf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:450:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vprintf_l
_vprintf_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$32, %rsp	 #,
	.seh_stackalloc	32
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:452:   {
	movq	%rdx, %rsi	 # tmp105, _Locale
	movq	%r8, %rdi	 # tmp106, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:453:     return _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:453:     return _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # _ArgList,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:453:     return _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp107, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:454:   }
	addq	$32, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:453:     return _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	jmp	_vfprintf_l	 #
	.seh_endproc
	.p2align 4
	.def	_fprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_fprintf_l
_fprintf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:456:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:459:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:460:     _Ret = _vfprintf_l(_File, _Format, _Locale, _ArgList);
	call	_vfprintf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:463:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_printf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_printf_l
_printf_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$48, %rsp	 #,
	.seh_stackalloc	48
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:468:     __builtin_va_start(_ArgList, _Locale);
	leaq	96(%rsp), %rdi	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:465:   {
	movq	%rdx, %rsi	 # tmp107, _Locale
	movq	%r8, 96(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp106, _Format
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:469:     _Ret = _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:468:     __builtin_va_start(_ArgList, _Locale);
	movq	%rdi, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:469:     _Ret = _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:469:     _Ret = _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # tmp103,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:469:     _Ret = _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp108, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:469:     _Ret = _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	call	_vfprintf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:472:   }
	addq	$48, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnprintf_l
_vsnprintf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:476:     return __stdio_common_vsprintf(_CRT_INTERNAL_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION, _DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	%r9, 32(%rsp)	 # tmp108,
	movq	%r8, %r9	 # tmp107,
	movq	%rdx, %r8	 # tmp106,
	movq	%rcx, %rdx	 # _DstBuf,
	movq	%rax, 40(%rsp)	 # _ArgList,
	movl	$1, %ecx	 #,
	call	__stdio_common_vsprintf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:477:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_snprintf_l
_snprintf_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:482:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:483:     _Ret = _vsnprintf_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	movq	%rax, 32(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:482:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:483:     _Ret = _vsnprintf_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
	call	_vsnprintf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:486:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsprintf_l
_vsprintf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:489:     return _vsnprintf_l(_DstBuf, (size_t)-1, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	$-1, %rdx	 #,
	call	_vsnprintf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:490:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_sprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_sprintf_l
_sprintf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:492:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:495:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:496:     _Ret = _vsprintf_l(_DstBuf, _Format, _Locale, _ArgList);
	call	_vsprintf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:499:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vscprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vscprintf_l
_vscprintf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:503:     return __stdio_common_vsprintf(_CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, _Locale, _ArgList);
	movq	%r8, 40(%rsp)	 # tmp105,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:502:   {
	movq	%rcx, %r9	 # _Format, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:503:     return __stdio_common_vsprintf(_CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR, NULL, 0, _Format, _Locale, _ArgList);
	xorl	%r8d, %r8d	 #
	movl	$2, %ecx	 #,
	movq	%rdx, 32(%rsp)	 # tmp104,
	xorl	%edx, %edx	 #
	call	__stdio_common_vsprintf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:504:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_scprintf_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_scprintf_l
_scprintf_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:506:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:509:     __builtin_va_start(_ArgList, _Locale);
	leaq	80(%rsp), %r8	 #, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:506:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:509:     __builtin_va_start(_ArgList, _Locale);
	movq	%r8, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:510:     _Ret = _vscprintf_l(_Format, _Locale, _ArgList);
	call	_vscprintf_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:513:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfwscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfwscanf_s_l
_vfwscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:613:     return __stdio_common_vfwscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT, _File, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # _File,
	movl	$3, %ecx	 #,
	call	__stdio_common_vfwscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:614:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vfwscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vfwscanf_s
vfwscanf_s:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:618:     return _vfwscanf_s_l(_File, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp104,
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:619:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:618:     return _vfwscanf_s_l(_File, _Format, NULL, _ArgList);
	jmp	_vfwscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_vwscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vwscanf_s_l
_vwscanf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$32, %rsp	 #,
	.seh_stackalloc	32
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:622:   {
	movq	%rdx, %rsi	 # tmp105, _Locale
	movq	%r8, %rdi	 # tmp106, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:623:     return _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	xorl	%ecx, %ecx	 #
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:623:     return _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # _ArgList,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:623:     return _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp107, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:624:   }
	addq	$32, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:623:     return _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	jmp	_vfwscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	vwscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vwscanf_s
vwscanf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$40, %rsp	 #,
	.seh_stackalloc	40
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:627:   {
	movq	%rdx, %rsi	 # tmp104, _ArgList
	movq	%rcx, %rbx	 # tmp103, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:628:     return _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	xorl	%ecx, %ecx	 #
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:628:     return _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # _ArgList,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:628:     return _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp105, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:629:   }
	addq	$40, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:628:     return _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	jmp	_vfwscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_fwscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_fwscanf_s_l
_fwscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:632:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:635:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:636:     _Ret = _vfwscanf_s_l(_File, _Format, _Locale, _ArgList);
	call	_vfwscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:639:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	fwscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	fwscanf_s
fwscanf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:642:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:646:     _Ret = _vfwscanf_s_l(_File, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:642:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:645:     __builtin_va_start(_ArgList, _Format);
	leaq	80(%rsp), %r9	 #, tmp102
	movq	%r9, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:646:     _Ret = _vfwscanf_s_l(_File, _Format, NULL, _ArgList);
	call	_vfwscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:649:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_wscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_wscanf_s_l
_wscanf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$48, %rsp	 #,
	.seh_stackalloc	48
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:655:     __builtin_va_start(_ArgList, _Locale);
	leaq	96(%rsp), %rdi	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:652:   {
	movq	%rdx, %rsi	 # tmp107, _Locale
	movq	%r8, 96(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp106, _Format
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:656:     _Ret = _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	xorl	%ecx, %ecx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:655:     __builtin_va_start(_ArgList, _Locale);
	movq	%rdi, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:656:     _Ret = _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:656:     _Ret = _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # tmp103,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:656:     _Ret = _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp108, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:656:     _Ret = _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	call	_vfwscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:659:   }
	addq	$48, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	wscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	wscanf_s
wscanf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:665:     __builtin_va_start(_ArgList, _Format);
	leaq	88(%rsp), %rsi	 #, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:662:   {
	movq	%rdx, 88(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp105, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:666:     _Ret = _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	xorl	%ecx, %ecx	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:662:   {
	movq	%r8, 96(%rsp)	 #,
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:665:     __builtin_va_start(_ArgList, _Format);
	movq	%rsi, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:666:     _Ret = _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:666:     _Ret = _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # tmp102,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:666:     _Ret = _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp106, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:666:     _Ret = _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	call	_vfwscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:669:   }
	addq	$56, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_vswscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vswscanf_s_l
_vswscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:673:     return __stdio_common_vswscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT, _Src, (size_t)-1, _Format, _Locale, _ArgList);
	movq	%r9, 40(%rsp)	 # tmp107,
	movq	%rdx, %r9	 # tmp105,
	movq	%rcx, %rdx	 # _Src,
	movl	$3, %ecx	 #,
	movq	%r8, 32(%rsp)	 # tmp106,
	movq	$-1, %r8	 #,
	call	__stdio_common_vswscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:674:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vswscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vswscanf_s
vswscanf_s:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:678:     return _vswscanf_s_l(_Src, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp104,
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:679:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:678:     return _vswscanf_s_l(_Src, _Format, NULL, _ArgList);
	jmp	_vswscanf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_swscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_swscanf_s_l
_swscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:682:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:685:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:686:     _Ret = _vswscanf_s_l(_Src, _Format, _Locale, _ArgList);
	call	_vswscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:689:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	swscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	swscanf_s
swscanf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:691:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:695:     _Ret = _vswscanf_s_l(_Src, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:691:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:694:     __builtin_va_start(_ArgList, _Format);
	leaq	80(%rsp), %r9	 #, tmp102
	movq	%r9, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:695:     _Ret = _vswscanf_s_l(_Src, _Format, NULL, _ArgList);
	call	_vswscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:698:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnwscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnwscanf_s_l
_vsnwscanf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:702:     return __stdio_common_vswscanf(_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT, _Src, _MaxCount, _Format, _Locale, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	%r9, 32(%rsp)	 # tmp108,
	movq	%r8, %r9	 # tmp107,
	movq	%rdx, %r8	 # tmp106,
	movq	%rcx, %rdx	 # _Src,
	movq	%rax, 40(%rsp)	 # _ArgList,
	movl	$3, %ecx	 #,
	call	__stdio_common_vswscanf	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:703:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snwscanf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_snwscanf_s_l
_snwscanf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:708:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:709:     _Ret = _vsnwscanf_s_l(_Src, _MaxCount, _Format, _Locale, _ArgList);
	movq	%rax, 32(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:708:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:709:     _Ret = _vsnwscanf_s_l(_Src, _MaxCount, _Format, _Locale, _ArgList);
	call	_vsnwscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:712:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snwscanf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	_snwscanf_s
_snwscanf_s:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:717:     __builtin_va_start(_ArgList, _Format);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:714:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:718:     _Ret = _vsnwscanf_s_l(_Src, _MaxCount, _Format, NULL, _ArgList);
	xorl	%r9d, %r9d	 #
	movq	%rax, 32(%rsp)	 # tmp103,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:717:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:718:     _Ret = _vsnwscanf_s_l(_Src, _MaxCount, _Format, NULL, _ArgList);
	call	_vsnwscanf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:721:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vfwprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vfwprintf_s_l
_vfwprintf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:725:     return __stdio_common_vfwprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _File, _Format, _Locale, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	movq	%r8, %r9	 # tmp106,
	movq	%rdx, %r8	 # tmp105,
	movq	%rcx, %rdx	 # _File,
	movl	$36, %ecx	 #,
	call	__stdio_common_vfwprintf_s	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:726:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vwprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vwprintf_s_l
_vwprintf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$32, %rsp	 #,
	.seh_stackalloc	32
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:728:   {
	movq	%rdx, %rsi	 # tmp105, _Locale
	movq	%r8, %rdi	 # tmp106, _ArgList
	movq	%rcx, %rbx	 # tmp104, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:729:     return _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:729:     return _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # _ArgList,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:729:     return _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp107, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:730:   }
	addq	$32, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:729:     return _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	jmp	_vfwprintf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	vfwprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vfwprintf_s
vfwprintf_s:
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:733:     return _vfwprintf_s_l(_File, _Format, NULL, _ArgList);
	movq	%r8, %r9	 # tmp104,
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:734:   }
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:733:     return _vfwprintf_s_l(_File, _Format, NULL, _ArgList);
	jmp	_vfwprintf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	vwprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vwprintf_s
vwprintf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$40, %rsp	 #,
	.seh_stackalloc	40
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:736:   {
	movq	%rdx, %rsi	 # tmp104, _ArgList
	movq	%rcx, %rbx	 # tmp103, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:737:     return _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	movl	$1, %ecx	 #,
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:737:     return _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # _ArgList,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:737:     return _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp105, _1
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:738:   }
	addq	$40, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:737:     return _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	jmp	_vfwprintf_s_l	 #
	.seh_endproc
	.p2align 4
	.def	_fwprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_fwprintf_s_l
_fwprintf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:740:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:743:     __builtin_va_start(_ArgList, _Locale);
	leaq	88(%rsp), %r9	 #, tmp103
	movq	%r9, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:744:     _Ret = _vfwprintf_s_l(_File, _Format, _Locale, _ArgList);
	call	_vfwprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:747:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_wprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_wprintf_s_l
_wprintf_s_l:
	pushq	%rdi	 #
	.seh_pushreg	%rdi
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$48, %rsp	 #,
	.seh_stackalloc	48
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:752:     __builtin_va_start(_ArgList, _Locale);
	leaq	96(%rsp), %rdi	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:749:   {
	movq	%rdx, %rsi	 # tmp107, _Locale
	movq	%r8, 96(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp106, _Format
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:753:     _Ret = _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:752:     __builtin_va_start(_ArgList, _Locale);
	movq	%rdi, 40(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:753:     _Ret = _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:753:     _Ret = _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rdi, %r9	 # tmp103,
	movq	%rsi, %r8	 # _Locale,
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:753:     _Ret = _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	movq	%rax, %rcx	 # tmp108, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:753:     _Ret = _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	call	_vfwprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:756:   }
	addq	$48, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	popq	%rdi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	fwprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	fwprintf_s
fwprintf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:758:   {
	movq	%r8, 80(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:762:     _Ret = _vfwprintf_s_l(_File, _Format, NULL, _ArgList);
	xorl	%r8d, %r8d	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:758:   {
	movq	%r9, 88(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:761:     __builtin_va_start(_ArgList, _Format);
	leaq	80(%rsp), %r9	 #, tmp102
	movq	%r9, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:762:     _Ret = _vfwprintf_s_l(_File, _Format, NULL, _ArgList);
	call	_vfwprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:765:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	wprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	wprintf_s
wprintf_s:
	pushq	%rsi	 #
	.seh_pushreg	%rsi
	pushq	%rbx	 #
	.seh_pushreg	%rbx
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:770:     __builtin_va_start(_ArgList, _Format);
	leaq	88(%rsp), %rsi	 #, tmp102
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:767:   {
	movq	%rdx, 88(%rsp)	 #,
	movq	%rcx, %rbx	 # tmp105, _Format
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:771:     _Ret = _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	movl	$1, %ecx	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:767:   {
	movq	%r8, 96(%rsp)	 #,
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:770:     __builtin_va_start(_ArgList, _Format);
	movq	%rsi, 40(%rsp)	 # tmp102, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:771:     _Ret = _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	call	*__imp___acrt_iob_func(%rip)	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:771:     _Ret = _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rsi, %r9	 # tmp102,
	xorl	%r8d, %r8d	 #
	movq	%rbx, %rdx	 # _Format,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:771:     _Ret = _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	movq	%rax, %rcx	 # tmp106, _2
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:771:     _Ret = _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	call	_vfwprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:774:   }
	addq	$56, %rsp	 #,
	popq	%rbx	 #
	popq	%rsi	 #
	ret	
	.seh_endproc
	.p2align 4
	.def	_vswprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vswprintf_s_l
_vswprintf_s_l:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:778:     return __stdio_common_vswprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _DstBuf, _DstSize, _Format, _Locale, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	%r9, 32(%rsp)	 # tmp108,
	movq	%r8, %r9	 # tmp107,
	movq	%rdx, %r8	 # tmp106,
	movq	%rcx, %rdx	 # _DstBuf,
	movq	%rax, 40(%rsp)	 # _ArgList,
	movl	$36, %ecx	 #,
	call	__stdio_common_vswprintf_s	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:779:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	vswprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	vswprintf_s
vswprintf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:782:     return _vswprintf_s_l(_DstBuf, _DstSize, _Format, NULL, _ArgList);
	movq	%r9, 32(%rsp)	 # tmp107,
	xorl	%r9d, %r9d	 #
	call	_vswprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:783:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_swprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_swprintf_s_l
_swprintf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:788:     __builtin_va_start(_ArgList, _Locale);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:789:     _Ret = _vswprintf_s_l(_DstBuf, _DstSize, _Format, _Locale, _ArgList);
	movq	%rax, 32(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:788:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:789:     _Ret = _vswprintf_s_l(_DstBuf, _DstSize, _Format, _Locale, _ArgList);
	call	_vswprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:792:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	swprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	swprintf_s
swprintf_s:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:797:     __builtin_va_start(_ArgList, _Format);
	leaq	104(%rsp), %rax	 #, tmp103
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:794:   {
	movq	%r9, 104(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:798:     _Ret = _vswprintf_s_l(_DstBuf, _DstSize, _Format, NULL, _ArgList);
	xorl	%r9d, %r9d	 #
	movq	%rax, 32(%rsp)	 # tmp103,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:797:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp103, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:798:     _Ret = _vswprintf_s_l(_DstBuf, _DstSize, _Format, NULL, _ArgList);
	call	_vswprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:801:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnwprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnwprintf_s_l
_vsnwprintf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:805:     return __stdio_common_vsnwprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	movq	120(%rsp), %rax	 # _ArgList, _ArgList
	movq	112(%rsp), %r10	 # _Locale, _Locale
	movq	%r9, 32(%rsp)	 # tmp109,
	movq	%r8, %r9	 # tmp108,
	movq	%rdx, %r8	 # tmp107,
	movq	%rcx, %rdx	 # _DstBuf,
	movq	%rax, 48(%rsp)	 # _ArgList,
	movl	$36, %ecx	 #,
	movq	%r10, 40(%rsp)	 # _Locale,
	call	__stdio_common_vsnwprintf_s	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:806:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_vsnwprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	_vsnwprintf_s
_vsnwprintf_s:
	subq	$56, %rsp	 #,
	.seh_stackalloc	56
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:809:     return _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	movq	96(%rsp), %rax	 # _ArgList, _ArgList
	movq	$0, 32(%rsp)	 #,
	movq	%rax, 40(%rsp)	 # _ArgList,
	call	_vsnwprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:810:   }
	addq	$56, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snwprintf_s_l;	.scl	3;	.type	32;	.endef
	.seh_proc	_snwprintf_s_l
_snwprintf_s_l:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:816:     _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	movq	112(%rsp), %r10	 # _Locale, _Locale
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:815:     __builtin_va_start(_ArgList, _Locale);
	leaq	120(%rsp), %rax	 #, tmp105
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:816:     _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	movq	%r10, 32(%rsp)	 # _Locale,
	movq	%rax, 40(%rsp)	 # tmp105,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:815:     __builtin_va_start(_ArgList, _Locale);
	movq	%rax, 56(%rsp)	 # tmp105, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:816:     _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
	call	_vsnwprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:819:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.def	_snwprintf_s;	.scl	3;	.type	32;	.endef
	.seh_proc	_snwprintf_s
_snwprintf_s:
	subq	$72, %rsp	 #,
	.seh_stackalloc	72
	.seh_endprologue
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:825:     _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	movq	$0, 32(%rsp)	 #,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:824:     __builtin_va_start(_ArgList, _Format);
	leaq	112(%rsp), %rax	 #, tmp104
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:825:     _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	movq	%rax, 40(%rsp)	 # tmp104,
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:824:     __builtin_va_start(_ArgList, _Format);
	movq	%rax, 56(%rsp)	 # tmp104, MEM[(char * *)&_ArgList]
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:825:     _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, NULL, _ArgList);
	call	_vsnwprintf_s_l	 #
 # C:/msys64/ucrt64/include/sec_api/stdio_s.h:828:   }
	addq	$72, %rsp	 #,
	ret	
	.seh_endproc
	.p2align 4
	.globl	dead_code
	.def	dead_code;	.scl	2;	.type	32;	.endef
	.seh_proc	dead_code
dead_code:
	.seh_endprologue
 # test6.c:12:     } /* Конец dead_code */
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
 # test6.c:15: {
	call	__main	 #
 # test6.c:17: }
	xorl	%eax, %eax	 #
	addq	$40, %rsp	 #,
	ret	
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev2, Built by MSYS2 project) 14.2.0"
	.def	fseek;	.scl	2;	.type	32;	.endef
	.def	ftell;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vfwscanf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vswscanf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vfwprintf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vswprintf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vfscanf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vsscanf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vfprintf_s;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vsprintf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vsnprintf_s;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vsprintf_s;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vfprintf_p;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vsprintf_p;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vfprintf;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vfwprintf_s;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vswprintf_s;	.scl	2;	.type	32;	.endef
	.def	__stdio_common_vsnwprintf_s;	.scl	2;	.type	32;	.endef
