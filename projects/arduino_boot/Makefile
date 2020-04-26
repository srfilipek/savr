MCU     ?= atmega328p
F_CPU   ?= 16000000
SERIAL_PORT ?= /dev/ttyUSB0

ifneq "$(MAKECMDGOALS)" "clean"
ifndef MCU
$(error MCU not defined)
endif
endif


## General Flags
DIRNAME     = $(shell pwd | sed "s/.*\\///g")
TARGET      = $(DIRNAME).elf
CC          = avr-gcc
AR          = avr-ar
LD          = avr-gcc
CXX         = avr-g++
TARGETS     = $(TARGET) $(TARGET:%.elf=%.lss) $(TARGET:%.elf=%.hex) $(TARGET:%.elf=%.eep)

## Options common to compile, link and assembly rules
LIBNAME     = savr_$(MCU)_$(F_CPU)
LIBDIR		= ../../lib
LIBFILE		= $(LIBDIR)/$(LIBNAME).a

## Include/lib Directories
INCLUDES   += -I../../include
LIBS       += -l$(LIBNAME)
LIBDIRS    += -L$(LIBDIR)

## Options common to compile, link and assembly rules
ARCHFLAGS = -mmcu=$(MCU)
OPTFLAGS  = -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mrelax -ffreestanding
COMMON    = $(ARCHFLAGS) -Wall -g -DMCU=$(MCU) -DF_CPU=$(F_CPU)UL $(OPTFLAGS)


## Compile options common for all C compilation units.
CXXFLAGS  = $(COMMON) -std=c++17 -fno-exceptions $(INCLUDES)
CFLAGS    = $(COMMON) -std=gnu99 $(INCLUDES)
SECTIONS  = -Wl,--section-start=.text=0x7800
LDFLAGS   = $(ARCHFLAGS) -Os -Wl,--relax,-Map=$(DIRNAME).map -nostartfiles $(SECTIONS)

## Objects that must be built in order to link
CINPUTS     = $(wildcard *.c)
CXXINPUTS   = $(wildcard *.cpp)
OBJECTS     = $(CINPUTS:%.c=%.o) $(CXXINPUTS:%.cpp=%.o)


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## Objects explicitly added by the user
LINKONLYOBJECTS = 

.PHONY: clean all size load load_isp cleanload lib info_isp
.INTERMEDIATE: $(OBJECTS)

## Build
all: $(TARGETS) size

%.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.a: $(OBJECTS)
	$(AR) -c -r $@ $? $(LIBS)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: $(TARGET)
	@echo
	@avr-size -C -t --mcu=$(MCU) $(TARGET)

lib:
	$(MAKE) -C $(LIBDIR)

load: $(TARGET:%.elf=%.hex)
	$(shell avrdude -c arduino -P $(SERIAL_PORT) -p $(MCU) -U $<)

load_isp: $(TARGET:%.elf=%.hex)
	$(shell avrdude -cavrisp2 -Pusb -p $(MCU) -U $<)

## Clean target
clean:
	-rm -rf *.o *.a *.lss *.hex *.elf *.map *.eep

cleanload:
	@$(MAKE) clean
	@$(MAKE) all
	@$(MAKE) load

