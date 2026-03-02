as --32 bootsect.asm -o bootsect.o
objcopy -O binary bootsect.o bootsect.bin
g++ -ffreestanding -fno-pie -O3 -g0 -m32 -o kernel.o -c kernel.cpp
ld --oformat binary -n -Ttext 0x10000 -o kernel.bin -entry=kmain -m elf_i386 kernel.o -T link.ld
