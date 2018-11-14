# Przykladowy plik makefile (C++)


#Programy uzywane do kompilacji
CXX     = arm-elf-c++
CP      = arm-elf-objcopy
CC	= arm-elf-gcc
ISPPROG = lpc21isp


#Opcje kompilatora
FLAGS = -Os -mcpu=arm7tdmi -Wall -gstabs

#Opcje specyficzne dla C++
CXXFLAGS  = $(FLAGS)

#Opcje specyficzne dla C
CFLAGS 	= $(FLAGS) -std=gnu99


#Opcje specyficzne dla assemblera
ASFLAGS = $(FLAGS) -Wa,-mapcs-32

#Opcje specyficzne dla linkera
LDFLAGS  =  $(FLAGS) -nostartfiles -Tlpc2142-rom.ld

#Format pliku wyjsciowego ihex lub binary
CPFLAGS = -O ihex -S

#Nazwa docelowego pliku wyjsciowego
all:	ledtest.hex

#Tworzenie projektu
ledtest.hex:	ledtest.elf
	$(CP) $(CPFLAGS) $^ $@

ledtest.elf:	led.o boot.o | lpc2142-rom.ld
	$(CXX) $(LDFLAGS) $^ -o $@ 

led.o:	led.cpp lpc214x.h


boot.o:	boot.S

#Czyszczenie projektu
clean:
	rm -f *.o
	rm -f ledtest.elf
	rm -rf ledtest.hex


#Programowanie procesora
program:
	lpc21isp -control ledtest.hex com1 19200 12000

