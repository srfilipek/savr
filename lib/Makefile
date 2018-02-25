ifneq "$(MAKECMDGOALS)" "clean"
ifndef MCU
$(error MCU not defined)
endif
ifndef F_CPU
$(error F_CPU not defined)
endif
endif


## General Flags
DIRNAME     = $(shell pwd | sed "s/.*\\///g")
TARGET      = libsavr_$(MCU)_$(F_CPU).a
CC          = avr-gcc
AR          = avr-ar
CXX         = avr-g++
LSS_TARGET  = $(TARGET:%.a=%.lss)

## Options common to compile, link and assembly rules
COMMON  = -mmcu=$(MCU) -Wall -Wextra -Wno-expansion-to-defined -g -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -DF_CPU=$(F_CPU)UL

## Include Directories
INCLUDES += -I../include

## Compile options common for all C compilation units.
CXXFLAGS  = $(COMMON) -std=gnu++14 -fno-exceptions $(INCLUDES)

CFLAGS    = $(COMMON) -std=gnu99 $(INCLUDES)

## Objects that must be built in order to link
CINPUTS     = $(wildcard *.c)
CXXINPUTS   = $(wildcard *.cpp)
OBJECTS     = $(CINPUTS:%.c=%.o) $(CXXINPUTS:%.cpp=%.o)

## Objects explicitly added by the user
LINKONLYOBJECTS = 

.PHONY: clean all size
.INTERMEDIATE: $(OBJECTS)

## Build
all: $(TARGET) $(LSS_TARGET) size

##Link
$(TARGET): $(OBJECTS)
	$(AR) -c -r $@ $? $(LIBS)

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: $(TARGET)
	@echo
	@avr-size -B -t --mcu=$(MCU) $(TARGET)

## Clean target
clean:
	-rm -rf *.o *.a *.lss

