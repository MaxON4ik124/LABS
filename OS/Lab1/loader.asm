.code16
; org 0x7C00
.section .text
.globl _start

_start:
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7C00, %sp
    sti

    movw $msg, %si
    call puts

hang:
    jmp hang
    