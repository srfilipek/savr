# Overview
This is an arduino-compatible bootloader, but under a permissive license (MIT).

# Interrupt Vectors
This bootloader is setup with the assumption that BOOTRST and IVSEL fuses are
both set (note: `0` is programmed). This causes the:
* reset address to be the bootloader start address and
* interrupt vectors to be in the application space.

Note: We still require that the RESET vector points to the application entry.

# Bootloader Invocation
The bootloader only activates after an external reset. Any other reset source
causes the main application to be immediately executed.

# Serial Port
The `savr::sci_min` library is utilized. See related files for more information.

# Bootloader Behavior
The bootloader performs the following register initialization:
* Watchdog
* Serial port

The watchdog is set to a 1s timeout. If no serial activity occurs within the
watchdog timeout, the system is automatically rebooted and the main application
started.

Upon successful completion, the watchdog is manually triggered, triggering a
system reset. The system will reboot, the watchdog will be disabled, and the
main application will run.

# Register States
The reset cause (MCUSR) is always cleared, but the original value is placed
into r3. The watchdog will always be disabled before application entry.

