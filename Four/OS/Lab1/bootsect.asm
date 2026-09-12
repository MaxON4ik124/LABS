.att_syntax prefix
.code16
.section .text
.globl _start

.set COLOR_ADDR, 0x0500

_start:
    cli
    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw $0x7C00, %sp
    sti

    movb $0, sel

redraw:
    movb $0x00, %ah
    movb $0x03, %al
    int  $0x10

    movw $msg, %si
    call puts

    xorw %bx, %bx
    movb sel, %bl
    shlw $1, %bx
    movw names(%bx), %si
    call puts
putc:
    movb $0x0E, %ah
    int  $0x10
    ret

puts:
1:
    lodsb
    testb %al, %al
    jz 2f
    call putc
    jmp 1b
2:
    ret

wait_key:
    movb $0x00, %ah
    int  $0x16

    cmpb $0x0D, %al
    je boot_now

    cmpb $0x00, %al
    jne wait_key

    cmpb $0x48, %ah
    je key_up

    cmpb $0x50, %ah
    je key_down

    jmp wait_key

key_up:
    movb sel, %al
    testb %al, %al
    jnz 1f
    movb $5, sel
    jmp redraw
1:
    decb sel
    jmp redraw

key_down:
    movb sel, %al
    cmpb $5, %al
    jne 1f
    movb $0, sel
    jmp redraw
1:
    incb sel
    jmp redraw

boot_now:
    xorw %bx, %bx
    movb sel, %bl
    movb colors(%bx), %al
    movb %al, COLOR_ADDR

    movw $0x1000, %ax
    movw %ax, %es
    xorw %bx, %bx

    xorw %ax, %ax
    int  $0x13
    jc disk_error

    movb $0x01, %dl 
    movb $0x00, %dh
    movb $0x00, %ch
    movb $0x02, %cl
    movb $0x10, %al
    movb $0x02, %ah
    int  $0x13
    jc disk_error

    inb  $0x92, %al
    orb  $0x02, %al
    outb %al, $0x92

    cli
    lgdt (gdt_info)

    movl %cr0, %eax
    orb  $0x01, %al
    movl %eax, %cr0

    ljmp $0x08, $pmode

disk_error:
    hlt
    jmp disk_error


.code32
pmode:
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw %ax, %gs
    movl $0x00010000, %eax
    jmp *%eax

.code16
gdt:
    .quad 0x0000000000000000
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF

gdt_info:
    .word gdt_end - gdt - 1
    .long gdt
gdt_end:

sel:
    .byte 0

colors:
    .byte 0x07, 0x0F, 0x0E, 0x09, 0x0C, 0x0A

names:
    .word n0, n1, n2, n3, n4, n5

msg: .asciz "Color: "
n0:  .asciz "gray"
n1:  .asciz "white"
n2:  .asciz "yellow"
n3:  .asciz "blue"
n4:  .asciz "red"
n5:  .asciz "green"

.org 510
.word 0xAA55