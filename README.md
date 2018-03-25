# Introduction #

SAVR is designed to be a cross-micro C++ library for the Atmel AVR 8-bit family.
This is primarily developed on the Arduino, but modified to be cross-platform
(cross-micro) to other AVRs I have laying around.

This has been a growing collection of code over many years. After reaching a
substantial size, and finding that it was (at least somewhat) useful to others
that I knew, I decided it was best to host the code publicly for all.

This library includes:

* Pin-based GPIO interface
* Millisecond clock
* Interfaces for various buses:
  * SPI
  * SCI (UART)
  * TWI (I2C)
  * 1-Wire
* SCI/UART binding to stdin and stdout
* Terminal interface
  * Simple command interface
  * Command history with up/down arrow navigation
* Various Peripherals:
  * ST7066/HD44780 based character LCDs
  * SD Cards over SPI
  * DS18B2x and DS182x 1-Wire temp sensors
  * RFM69 wireless module

The library has been at least somewhat tested with the following:

* ATmega328P (Arduino)
* ATmega1284P
* ATmega8
* ATmega88
* ATmega8515
* ATmega16
* ATmega32
* ATmega644P

Variants to the devices listed above, if they share the same specification
document, are also considered as supported. For instance, the ATmega48, 88,
and 168. Adding support for new devices is usually trivial.

The SAVR library compiles for nearly every AVR that's supported by GCC<span
title="OK, so I have no data to back that up. I think that's the case, and it
sure does sound good, right?"><sup>[+]</sup></span>. There are some
micro-specific sections that rely on knowledge of the exact AVR in use, such as
the SCI, SPI, and TWI interfaces. If the target micro isn't known to the SAVR
library, those sections will be removed.

This library relies on avr-libc and AVR GCC, such as distributed through WinAVR
and CrossPack for AVR.

# Compiling the Library #

## Requirements ##
This has been tested and built with GCC for AVR. The only pre-packaged systems
used/supported are WinAVR for Windows, and CrossPack for OS X.

The makefile creates a static library named `libsavr_MCU_SPEED.a`. For the
most common Arduinos, this will be `libsavr_atmega328p_16000000.a`.

## Building ##
Command line:

1. CD to the lib/ directory
1. Run something like `make MCU=atmega328p F_CPU=16000000`
1. Substitute the MCU with whatever you will be using (the GCC supported target
for `-mmcu=xxxx`)
1. Substitute the CPU clock speed with your target's speed
1. If your MCU isn't supported by my library, let me know, or better yet, create
a patch!

Eclipse:

1. Create a "Makefile project with existing code"
1. Make the existing code location be the root of the Git repository
1. Go into the project properties
1. Find "C/C++ Build"
1. Uncheck "Use default build command"
1. Modify the build command to be `make -C lib MCU=atmega328p F_CPU=16000000`
  * Change the MCU and frequency accordingly


# Linking to the Library #
Test projects are included under the tests/ directory. This also has a Test.mk
makefile which shows how I normally build.

This is no different than linking against any other library. In this case, use
`-lsavr_MCU_SPEED` at the linking stage. For instance,
`-lsavr_atmega328p_16000000`.

# Using the Library #

## Including the header files ##
I recommend adding the include/ folder to the include path when compiling. That
way you can limit header file naming collision and match the style used by the
library itself and the tests provided. Including SAVR header files should look
like:
```
#include <savr/w1.h>
#include <savr/dstherm.h>
...
```

## Code organization ##
The library is grouped into either classes or namespaces. Namespaces are used in
situations where there is no need for multiple objects (for instance, a single
terminal, a single UART for stdin/stdout binding, etc).

## Frequency dependence ##
Version 0.3 was compile-time frequency independent, meaning that the actual
clock frequency needed to be passed in to many constructors or initializers.
This was for a grand idea that you could possibly change the system clock on the
fly with an external PLL or oscillator. Coding like this for the one guy who
might actually end up doing this, it wasn't worth it -- the code was getting too
bloated and too heavy. This independence was removed in version 1.0, and the
library is now compiled for a specific target frequency. The code optimization
allows for it to run on smaller, slower devices.


## Global objects ##
There is no 'new' or 'delete' support. This means all objects must be globally
created using static initialization, have a global pointer to a persistent
stack object, or simply have references or pointers passed around as function
arguments.

Static initialization:
```c++
static W1 wire(gpio::C0);

...

foo() {
    wire.reset();
    ...
}
```

Global pointer to persistent object:

```c++
static W1 *wire;

...

foo() {
    wire->reset();
    ...
}

...

int main(void) {
    W1 local_wire(gpio::C0);
    wire = &local_wire;

    ...

    while(1) {
        ...
    }
}
```

Function argument (by reference):
```c++
foo(W1 &wire) {
    wire.reset();
    ...
}

...

int main(void) {
    W1 wire(gpio::C0);
    foo(wire);
}
```

# Hardware Tested Devices #
The library has been tested with hardware in the loop (HIL) for:
  * ATmega328P (Arduino)
  * ATmega8
  * ATmega88
  * ATmega8515
  * ATmega16
  * ATmega32
  * ATmega644P
  * atmega1284p

# Compilation Support #
In addition to HIL testing many additional micros have been "compile tested".
That is, I've just made sure that the compilation doesn't fail. The following
are the names specified to GCC with `-mmcu=`:
  * atmega8
  * atmega16
  * atmega32
  * atmega644
  * atmega8515
  * atmega48
  * atmega88
  * atmega168
  * atmega48p
  * atmega88p
  * atmega168p
  * atmega88pa
  * atmega328p
  * atmega164p
  * atmega324p
  * atmega644p
  * atmega164a
  * atmega324a
  * atmega324pa
  * atmega644a
  * atmega644pa
  * atmega1284p
