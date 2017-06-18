all: connect4.hex

connect4.hex: connect4.elf
	avr-objcopy  -j .text -j .data -O ihex $^ $@
	avr-size connect4.elf

connect4.elf: connect4.c
	avr-g++ -mmcu=atmega324a -Os -Wall -o $@ $^

clean:
	rm -rf connect4.elf connect4.hex
