PORT ?= tty0

all: test.hex

program: upload

test.elf: test.c lcd.c util.c
	avr-g++ -mmcu=atmega324p -DF_CPU=16000000 -Wall -Os -o $@ $^

test.hex: test.elf
	avr-objcopy -j .text -j .data -O ihex test.elf test.hex
	avr-size test.elf

upload: test.hex
	avrdude -c arduino -P $(PORT) -b 57600 -p atmega324p -U flash:w:$<:a

clean:
	rm -rf test.elf test.hex

.PHONY: all clean program upload

