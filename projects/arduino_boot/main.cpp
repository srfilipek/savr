/*************************************************************//**
 * @file main.c
 *
 * @author Stefan Filipek
 ******************************************************************/
#include <inttypes.h>
#include <stdlib.h>

#include <avr/boot.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <savr/gpio.h>
#include <savr/sci.h>
#include <savr/version.h>
#include <savr/wdt.h>

#include "stk500_commands.h"

using namespace savr;

/**
 * Handle some register naming issues
 */
#if defined(MCUCSR)
# define _MCUCSR MCUCSR
#else
# define _MCUCSR MCUSR
#endif

/**
 * Key assumptions
 */
static_assert(SPM_PAGESIZE <= 256);
static_assert(sizeof(uintptr_t) == sizeof(uint16_t));
static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);

/**
 * Will contain MCUCSR
 */
register uint8_t reset_reg asm ("r3");


/**
 * Application entry
 */
FORCE_INLINE
__attribute__((noreturn))
void
app_main(void) {
    asm volatile("jmp 0");
}


/**
 * End bootloader execution via watchdog reset.
 */
__attribute__((noreturn))
void
reset() {
    wdt::configure(wdt::WDT_RST, wdt::WDT_16ms);
    while(1) {
        // Nothing
    }
}


/**
 * Handle reading the command tail plus sending the proper response
 */
__attribute__((noinline))
void
command_end() {
    if (sci::min::get_char() != Sync_CRC_EOP) {
        sci::min::put_char(Resp_STK_NOSYNC);
        reset();
    }
    sci::min::put_char(Resp_STK_INSYNC);
    sci::min::put_char(Resp_STK_OK);
}


/**
 * Handle reading the command tail plus sending the proper response.
 *
 * But, do NOT send Resp_STK_OK;
 */
__attribute__((noinline))
void
command_sync() {
    if (sci::min::get_char() == Sync_CRC_EOP) {
        sci::min::put_char(Resp_STK_INSYNC);
        return;
    }

    sci::min::put_char(Resp_STK_NOSYNC);
    reset();
}


__attribute__((noinline))
void
burn_chars(uint8_t n) {
    while(n-- > 0) {
        (void)sci::min::get_char();
    }
}


FORCE_INLINE
void
handle_get_parameters() {
    uint8_t param = sci::min::get_char();
    command_sync();

    if (param == Parm_STK_SW_MAJOR) {
        sci::min::put_char(SAVR_MAJOR);
    } else if(param == Parm_STK_SW_MINOR) {
        sci::min::put_char(SAVR_MINOR);
    } else {
        sci::min::put_char(3);
    }
    sci::min::put_char(Resp_STK_OK);
}


FORCE_INLINE
static void
handle_set_device() {
    // Ignore
    burn_chars(20);
    command_end();
}


FORCE_INLINE
static void
handle_set_device_ext() {
    // Ignore
    burn_chars(5);
    command_end();
}

FORCE_INLINE
__attribute__((noreturn))
static void
handle_leave_progmode() {
    command_end();
    reset();
}


FORCE_INLINE
uintptr_t
handle_set_address() {
    // Sent as addr_low, addr_high
    uint8_t addr_low = sci::min::get_char();
    uint8_t addr_high = sci::min::get_char();

    command_end();

    // Given address is a 16-bit word address, convert to byte address
    return ((addr_high << 8) | addr_low) * 2;
}


FORCE_INLINE
void
handle_universal() {
    // Ignore
    burn_chars(4);
    command_sync();
    sci::min::put_char(0);
    sci::min::put_char(Resp_STK_OK);
}


/**
 * The STK500 interface doc states that the size for reading or programming
 * pages should be no larger than 256.
 */
FORCE_INLINE
uint16_t
get_size() {
    return (sci::min::get_char() << 8) | sci::min::get_char();
}


FORCE_INLINE
void
handle_page_op(uintptr_t address, bool write) {
    uint8_t buffer[256];

    uint16_t size = get_size();
    char mem_type = sci::min::get_char();

    if (write) {
        for (size_t i = 0; i < size; ++i) {
            buffer[i] = sci::min::get_char();
        }
    }

    command_sync();

    if (mem_type != 'F')
        reset();

    if (write) {
        boot_page_erase(address);
        boot_spm_busy_wait();

        for (size_t i = 0; i < size; i += 2) {
            // Little endian words
            uint16_t word = *reinterpret_cast<uint16_t*>(&buffer[i]);
            boot_page_fill(address + i, word);
        }

        boot_page_write(address);
        boot_spm_busy_wait();
        boot_rww_enable();
    } else {
        for (size_t i = 0; i < size; ++i) {
            sci::min::put_char(pgm_read_byte(address + i));
        }
    }

    sci::min::put_char(Resp_STK_OK);
}


FORCE_INLINE
void
handle_read_sign() {
    command_sync();

    // Signature defined in avr libc
    sci::min::put_char(SIGNATURE_0);
    sci::min::put_char(SIGNATURE_1);
    sci::min::put_char(SIGNATURE_2);

    sci::min::put_char(Resp_STK_OK);
}


/**
 * Main bootloader command loop.
 *
 * See AVR061 for the command reference.
 *
 * This loop is setup to match the expectations of avrdude.
 */
__attribute__((noreturn))
FORCE_INLINE
void
boot_loop(void) {
    uintptr_t address = 0;
    while(1) {
        uint8_t c = sci::min::get_char();
        wdt::kick();

        if (c == Cmnd_STK_GET_PARAMETER) {
            handle_get_parameters();

        } else if (c == Cmnd_STK_SET_DEVICE) {
            handle_set_device();

        } else if (c == Cmnd_STK_SET_DEVICE_EXT) {
            handle_set_device_ext();

        } else if (c == Cmnd_STK_LEAVE_PROGMODE) {
            handle_leave_progmode();

        } else if (c == Cmnd_STK_LOAD_ADDRESS) {
            address = handle_set_address();

        } else if (c == Cmnd_STK_UNIVERSAL) {
            handle_universal();

        } else if (c == Cmnd_STK_PROG_PAGE) {
            handle_page_op(address, true);

        } else if (c == Cmnd_STK_READ_PAGE) {
            handle_page_op(address, false);

        } else if (c == Cmnd_STK_READ_SIGN) {
            handle_read_sign();

        } else {
            // Handles:
            //  - Cmnd_STK_GET_SYNC
            //  - Cmnd_STK_ENTER_PROGMODE
            command_end();
        }
    }
    /* NOTREACHED */
}

/**
 * Entry to the bootloader.
 *
 * Place it in the .init9 section to ensure it gets placed properly.
 *
 * Declarding as OS_main sets up the stack for us. But, we must clear the zero
 * register and zero out the bss (if it exists... which it doesn't).
 */
__attribute__((noreturn, OS_main, section(".init9")))
void
entry(void) {
    // Zero reg must be zero
    asm volatile("clr __zero_reg__\n");

    // Clear the MCUCSR immediately
    reset_reg = _MCUCSR;
    _MCUCSR = 0;

    // Turn off the watchdog
    WDTCSR = _BV(WDCE) | _BV(WDE);
    WDTCSR = 0;

    // If the reset was not external, jump to app immediately
    if ((reset_reg & _BV(EXTRF)) == 0) {
        app_main();
    }

    // Other initialization
    gpio::out<gpio::B5>();
    gpio::high<gpio::B5>();
    sci::min::init<115200uL>(); // bps
    wdt::configure(wdt::WDT_RST, wdt::WDT_500ms);

    boot_loop();
    /* NOTREACHED */
}

