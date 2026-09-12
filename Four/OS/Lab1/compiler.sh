as --32 -o bootsect.o bootsect.asm
ld -m elf_i386 -Ttext 0x7C00 -nostdlib --nmagic -o bootsect.elf bootsect.o
objcopy -O binary -j .text bootsect.elf bootsect.raw
truncate -s 512 bootsect.raw
g++ -m32 -ffreestanding -fno-pie -fno-pic -fno-stack-protector -nostdlib -c kernel.cpp -o kernel.o
ld -m elf_i386 -T link.ld --oformat=binary -o kernel.raw kernel.o


dd if=/dev/zero of=bootsect.bin bs=512 count=2880
dd if=bootsect.raw of=bootsect.bin conv=notrunc bs=512 seek=0

dd if=/dev/zero of=kernel.bin bs=512 count=2880
dd if=kernel.raw of=kernel.bin conv=notrunc bs=512 seek=1