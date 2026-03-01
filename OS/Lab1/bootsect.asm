.code16

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

    movb $0x0E, %ah
    movb $'A', %al
    int $0x10



load_kernel:
    movw $0x1000, %ax
    movw %ax, %es
    xorw %bx, %bx

    movb $0x0E, %ah
    movb $'R', %al
    int $0x10

    movb $0x01, %dl
    movb $0x00, %dh
    movb $0x00, %ch
    movb $0x02, %cl
    movb $0x01, %al
    movb $0x02, %ah
    int $0x13
    jc disk_error
    movb $0x0E, %ah
    movb $'C', %al
    int $0x10

    call a20
    cli
    lgdt gdt_info

    movl %cr0, %eax 
    orb $0x01, %al
    movl %eax, %cr0

    movb $0x0E, %ah
    movb $'P', %al
    int $0x10

    ljmp $0x08, $prot_mode_entry
disk_error:
    movb $0x0E, %ah
    movb $'E', %al
    int $0x10
    hlt
    jmp disk_error
a20:
    inb $0x92, %al
    orb $0x02, %al
    outb %al, $0x92
    ret
.code32
prot_mode_entry:
    movw $0x10, %ax
    movw %ax, %es
    movw %ax, %ds
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs
    movl $0x90000, %esp

    jmp 0x00010000

.balign 8
gdt:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF

gdt_info:
    .word (gdt_end - gdt - 1)
    .long gdt
gdt_end:
boot_drive:
    .byte 1


.org 510
.word 0xAA55


    


    