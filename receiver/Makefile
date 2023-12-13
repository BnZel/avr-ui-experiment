CC = avr-gcc 
OBJCPY = avr-objcopy
SIZE = avr-size
MCU = atmega328
F_CPU = 16000000
CFLAGS = \
	-mmcu=$(MCU) \
	-DF_CPU=$(F_CPU)UL \
	-Os \
	-std=gnu99 \
	-Werror \
	-ffunction-sections \
	-fdata-sections 
LDFLAGS = \
	-Wl,--gc-sections \
	-mmcu=$(MCU)

AVRDUDE=avrdude

PORT=$(shell pavr2cmd --prog-port)

SRC = $(shell ls ./*.c) 

OBJ = $(SRC:.c=.o)

main.hex: main.elf
	$(OBJCPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature main.elf main.hex

main.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

size: main.elf
	$(SIZE) --mcu=$(MCU) --format=avr main.elf

clean:
	rm -f $(OBJ) main.elf main.hex

upload: main.hex
	avrdude -c stk500v2 -P "$(PORT)" -p $(MCU) -U flash:w:$<:i -B 0.5 

memory:
	avr-size -C --mcu=atmega328 -f 16000000 main.elf

auto: clean
	make && make upload
	avr-size -C --mcu=atmega328 main.elf