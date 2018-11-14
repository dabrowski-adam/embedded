
#-d  w³¹cza tryb szczegó³owego œledzenia
#-f  plik_sterujacy umo¿liwia stosowanie innych ni¿ standardowe nazw plików steruj¹cych
#-i  powoduje ignorowanie b³êdów kompilacji (stosowaæ z ostro¿noœci¹!)
#-n  powoduje wypisanie poleceñ na ekran zamiast ich wykonania
#-p  powoduje wypisanie makrodefinicji i regu³ transformacji
#-s  wy³¹cza wypisywanie treœci polecenia przed jego wykonaniem

#BAT powinien ustawiæ nastêpuj¹ce zmienne œrodowiskowe
#USE_THUMB_MODE=[YES|NO]
#set DEBUG=[1|0]
#set RUN_MODE=[RUN_FROM_ROM|RUN_FROM_ROM]
#set LDSCRIPT=<np. lpc2148-rom.ld>

ifeq ($(USE_THUMB_MODE), undefined)
	USE_THUMB_MODE = YES
endif
ifeq ($(DEBUG), undefined)
	DEBUG = 0
endif
ifeq ($(RUN_MODE), undefined)
	RUN_MODE = RUN_FROM_ROM
endif
ifeq ($(LDSCRIPT), undefined)
	@echo "Default library!"
	LDSCRIPT = lpc2148-rom.ld
endif




###########################################################################################
EXECNAME=eduboard
###########################################################################################


CC=arm-elf-gcc
OBJCOPY=arm-elf-objcopy
ARCH=arm-elf-ar
CRT0=boot.s
AWK             = gawk
SIZE            = arm-elf-size
STRIP           = arm-elf-strip

WARNINGS  = -Wall
WARNINGS += -Wextra
WARNINGS += -Wshadow
WARNINGS += -Wpointer-arith
WARNINGS += -Wcast-align
WARNINGS += -Wsign-compare
WARNINGS += -Waggregate-return
WARNINGS += -Wunused

CPP_WARNINGS = $(WARNINGS)

C_WARNINGS = $(WARNINGS)
C_WARNINGS += -Wbad-function-cast
C_WARNINGS += -Wstrict-prototypes
C_WARNINGS += -Wmissing-prototypes
C_WARNINGS += -Wmissing-declarations

#
# CFLAGS common to both the THUMB and ARM mode builds
#
RTOS_SOURCE_DIR=./FreeRTOS

#DEBUG = 1

#OFlags = -Os , to conserve space, -O3, to speed
ifeq ($(DEBUG),1)
	DBFLAGS += -gdwarf-2
	OFLAGS = -Os
else
	DBFLAGS +=
	OFLAGS = -O3
endif



CFLAGS=            -D $(RUN_MODE) -D GCC_ARM7 -I. -I$(RTOS_SOURCE_DIR)/include \
		   $(DBFLAGS) -mcpu=arm7tdmi -T$(LDSCRIPT) \
		   $(OFLAGS) -fomit-frame-pointer -fno-strict-aliasing

LSTFLAGS = -Wa,-ahlms=$(<:.c=.lst)

CPP_LSTFLAGS = -Wa,-ahlms=$(<:.cc=.lst)


ifeq ($(USE_THUMB_MODE),YES)
	CFLAGS += -mthumb-interwork -D THUMB_INTERWORK
	THUMB_FLAGS=-mthumb
endif

#LST = $(SRC:.c=.lst) $(ASRC:.asm=.lst)

LINKER_FLAGS  = -Xlinker -o$(EXECNAME).elf -Xlinker -M -Xlinker -Map=$(EXECNAME).map

#
# Source files that can be built to THUMB mode.
#
THUMB_CPP_SRC =
#service.cc main.cc InitBoard.cc Service_LEDMatrix.cc



THUMB_SRC = \
main.c \
serial/serial.c \
testLEDMatrix.c \
testRGB.c \
testLCD.c \
testUART.c \
testMotor.c \
testADC.c \
$(RTOS_SOURCE_DIR)/tasks.c \
$(RTOS_SOURCE_DIR)/queue.c \
$(RTOS_SOURCE_DIR)/list.c \
portable/MemMang/heap_2.c \
portable/port.c

#
# Source files that must be built to ARM mode.
#
ARM_SRC = \
portable/portISR.c \
spi_write.c \
serial/serialISR.c


#
# Define all object files.
#
ARM_OBJ = $(ARM_SRC:.c=.o)
#ARM_CPP_OBJ = $(ARM_SRC:.cc=.o)
THUMB_OBJ = $(THUMB_SRC:.c=.o)
THUMB_CPP_OBJ = $(THUMB_CPP_SRC:.cc=.o)
ASM_OBJ = $(CRT0:.s=.o)

$(EXECNAME).hex : $(EXECNAME).elf
	$(OBJCOPY) $(EXECNAME).elf -O ihex $(EXECNAME).hex
	@echo ""
	@echo "=== Result ==============================================="
	@echo ""
	@echo " TEXT: Code size"
	@echo " DATA: Initialized data"
	@echo " BSS:  Uninitialized data"
	@echo " ROM:  Size in non-volatile program memory (TEXT + DATA)"
	@echo " RAM:  Size in volatile data memory (DATA + BSS)"
	@echo "=== Total size (including external libraries) ============"
	@echo ""
	@echo "   Code compiled with optimization switch: " $(CFLAGS)
	@echo "   (other settings may produce different sizes)"
	@echo "   (external libraries may be compiled with different settings)"
	@echo ""
	@echo "   TEXT    DATA     BSS     ROM     RAM FILENAME"
	@echo "   ====    ====     ===     ===     === ========"
	@$(SIZE) $(EXECNAME).elf \
	| $(AWK) 'NR > 1 { printf "%7d %7d %7d %7d %7d %s\n", \
			   $$1,$$2,$$3,$$1+$$2,$$2+$$3,"$(EXECNAME).elf" }'
	@echo ""
ifeq ($(RUN_MODE), $(RUN_FROM_RAM))
	@echo "                          |===========|"
	@echo "                               ^^^"
	@echo "   Everything placed in RAM since LD_RAMROM = RAM"
	@echo ""
endif
#
	@$(SIZE) $(EXECNAME).hex \
	| $(AWK) 'NR > 1 { t+=$$1; d+=$$2; b+=$$3; } \
		  END { ro=t+d; ra=d+b; printf "%7d %7d %7d %7d %7d %s\n", \
					t,d,b,ro,ra,"$(EXECNAME).hex" }'


$(EXECNAME).elf : $(ARM_OBJ) $(THUMB_OBJ) $(THUMB_CPP_OBJ) $(ARM_CPP_OBJ)  $(CRT0) Makefile
	$(CC) $(CFLAGS) $(ARM_OBJ) $(ARM_CPP_OBJ) $(THUMB_OBJ) $(THUMB_CPP_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)

$(THUMB_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c $(C_WARNINGS) $(THUMB_FLAGS) $(CFLAGS) $(LSTFLAGS) $< -o $@

$(THUMB_CPP_OBJ) : %.o : %.cc $(LDSCRIPT) Makefile
	$(CC) -c $(CPP_WARNINGS) $(THUMB_FLAGS) $(CFLAGS) $(CPP_LSTFLAGS) $< -o $@

$(ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c  $(C_WARNINGS) $(CFLAGS)  $(LSTFLAGS) $< -o $@

#$(ARM_CPP_OBJ) : %.o : %.cc $(LDSCRIPT) Makefile
#	$(CC) -c $(CFLAGS) $< -o $@

#$(ASM_OBJ) : %.o : %.s $(LDSCRIPT) Makefile
#	$(CC) -c $(CFLAGS) $< -o $@

clean :
	touch Makefile











