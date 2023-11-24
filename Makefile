TARGET = odoconv
SOURCES = odoconv.ino
OBJECTS = $(SOURCES:.ino=.o)

# microcontroller and clockspeed [Hz]
MCU = attiny85
F_CPU ?= 1000000ul

# default settings for ArduinoAsISP programmer
PROGRAMMER ?= avrisp
PORT ?= /dev/ttyACM0
BAUD ?= 19200

CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -Os -Wall -Wextra -Wpedantic -Werror
CCFLAGS = -DF_CPU=$(F_CPU)
LDFLAGS = 

PRGFLAGS = -p $(MCU) -P $(PORT) -B $(BAUD) -c $(PROGRAMMER)


all: $(TARGET).hex


%.hex: %.elf
	avr-objcopy -O ihex -j .data -j .text $< $@


$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


%.o: %.ino
	$(CC) $(CFLAGS) $(CCFLAGS) -c -o $@ -x c++ $<


program: $(TARGET).hex
	avrdude $(PRGFLAGS) -U flash:w:$^


clean:
	rm -f *.o
	rm -f *.elf
	rm -f *.hex


.PHONY: clean program
