# SAVR 2.2
  * New, minimal SCI interface
  * Improve SCI baud rate calculation
  * More CRC calculations
  * New watchdog interface

# SAVR 2.1
  * Move to C++17
  * Adding RFM69 module library
  * Adding millisecond system clock
  * Slight optimization to runtime GPIO library
  * Additional optimized routines for byte access and nibble swapping
  * Additional template-based bitfield access

# SAVR 2.0
  * Adding a CRC library for 7, 8, and 16-bit CRCs
  * Bugfix in the SCI library when used with bootloaders
  * Coding style update, namespacing (the real reason for the major verion bump)

# SAVR 1.2
  * SAVR can now be built for any micro without an explicit port. Micro-dependent code (SCI/SPI/TWI) will be removed if necessary.
  * Terminal interface can be used in a non-blocking mode, allowing it to be integrated into a work loop.
  * Reduced overhead of 1-wire driver.
  * Added optimization file for assembly level optimizations when necessary.
  * TWI can be used with or without internal pull-up resistors.
  * Added SD card test app.
  * Other library improvements and documentation updates.

# SAVR 1.1
  * Added support for ATmega128P and related devices.
  * Added command line history with up/down arrow navigation.
  * GPIO interface has template functions for compile-time optimization, reducing function calls to a single instruction.
  * Version string macros added to version.h.
  * Queue is now a template, taking type and size as parameters.
  * Minor optimizations here and there.

# SAVR 1.0
  * Removed run-time sysclock frequency dependence. CPU frequency must be specified at compile time.
  * Added "Hello World!" tests and the like.
  * Code optimizations and memory use reductions.
  * Added support for a number of different AVRs.

# SAVR 0.3
  * Initial public release.
