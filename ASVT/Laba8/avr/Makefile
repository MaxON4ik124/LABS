MCU      = atmega32
F_CPU    = 8000000UL
TARGET   = mcu_elgamal_v3
CC       = avr-gcc
OBJCOPY  = avr-objcopy
SIZE     = avr-size

CFLAGS = -std=c11 -Wall -Wextra -Werror -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
SRC = \
    main.c \
    uart.c \
    ringbuf.c \
    protocol.c \
    eeprom_store.c \
    crypto_elgamal.c

OBJ = $(SRC:.c=.o)

all: $(TARGET).hex

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)
	$(SIZE) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex
