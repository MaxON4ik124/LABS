as --32 bootsect.asm -o bootsect.o
ld -m elf_i386 -Ttext 0x7C00 -nostdlib --nmagic -o bootsect.elf bootsect.o
objcopy -O binary bootsect.elf bootsect.bin
g++ -ffreestanding -fno-pie -O3 -g0 -m32 -o kernel.o -c kernel.cpp
ld --oformat binary -n -Ttext 0x10000 -o kernel.bin --entry=kmain -m elf_i386 kernel.o -T link.ld
