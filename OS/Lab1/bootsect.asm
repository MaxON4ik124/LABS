.code16
; org 0x7C00
.section .text
.globl _start

_start:
    cli
    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7C00, %sp
    sti
    jmp load_kernel



load_kernel:

    movw $0x1000, %ax
    movw %ax, %es
    xorw %bx, %bx

    movb $0x00, %dh
    movb $0x00, %ch
    movb $0x01, %cl
    movb $0x01, %al
    movb $0x02, %ah

    movb %dl, boot_drive

    call a20
    cli
    lgdt (gdt_info)

    movl %cr0, %eax ; cr0 -> eax : Защищенный режим
    orb $0x01, %al
    movl %cr0, %cr0

    ljmp $0x08, $prot_mode_entry
a20:
    inb $0x92, %al
    orb $0x02, %al
    outb %al, 0x92
    ret
prot_mode_entry:
    movw $0x10, %ax
    movw %ax, %es
    movw %ax, %ds
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs
    jmp *$0x00010000

.align 8
gdt:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF

gdt_info:
    .word (gdt_info - gdt - 1)
    .long gdt
boot_drive:
    .byte 0


.org 510
.word 0xAA55


    


    