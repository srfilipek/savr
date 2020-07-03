/*******************************************************************************
 Copyright (C) 2015 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*******************************************************************************/
#ifndef _savr_sci_h_included_
#define _savr_sci_h_included_

/**
 * @file sci.h
 *
 * SCI/UART interface
 *
 * This interface relies on avr-libc. It binds the compile-time
 * configured UART to stdin and stdout, allowing the user to
 * read/write data over the serial line using printf and similar
 * calls.
 *
 * This is intended to be one-per-system as the 'debug console'.
 */

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>

#include <savr/sci_defs.h>
#include <savr/utils.h>

#if !defined(SAVR_NO_SCI)

namespace savr {
namespace sci {

/**
 * Initialize the SCI subsystem
 *
 * @param baud The desired baud rate
 */
void
init(uint32_t baud);

size_t
size(FILE *stream);

/**
 * Calculate the UBRR setting based on the given baud
 *
 * @param baud The desired baud rate
 */
constexpr uint16_t ubrr_setting(uint32_t baud) {
    return static_cast<uint16_t>(F_CPU / 4 / baud - 1) / 2;
}

/**
 * Ensure our calculations match the recommendations from Atmel
 */
#if F_CPU == 20000000
static_assert(ubrr_setting(250000) == 9);
static_assert(ubrr_setting(115200) == 21);
static_assert(ubrr_setting(57600) == 42);
static_assert(ubrr_setting(38400) == 64);
static_assert(ubrr_setting(28800) == 86);
static_assert(ubrr_setting(14400) == 173);
static_assert(ubrr_setting(9600) == 259);

#elif F_CPU == 16000000
static_assert(ubrr_setting(250000) == 7);
static_assert(ubrr_setting(115200) == 16);
static_assert(ubrr_setting(57600) == 34);
static_assert(ubrr_setting(38400) == 51);
static_assert(ubrr_setting(28800) == 68);
static_assert(ubrr_setting(14400) == 138);
static_assert(ubrr_setting(9600) == 207);

#elif F_CPU == 8000000
static_assert(ubrr_setting(250000) == 3);
static_assert(ubrr_setting(115200) == 8);
static_assert(ubrr_setting(57600) == 16);
static_assert(ubrr_setting(38400) == 25);
static_assert(ubrr_setting(28800) == 34);
static_assert(ubrr_setting(14400) == 68);
static_assert(ubrr_setting(9600) == 103);

#elif F_CPU == 4000000
static_assert(ubrr_setting(250000) == 1);
static_assert(ubrr_setting(115200) == 3);
static_assert(ubrr_setting(57600) == 8);
static_assert(ubrr_setting(38400) == 12);
static_assert(ubrr_setting(28800) == 16);
static_assert(ubrr_setting(14400) == 34);
static_assert(ubrr_setting(9600) == 51);

#endif

namespace min {
/**
 * Minimal initialization of the UART
 *
 * This does NOT bind to stdin/stdout, nor does it enable interrupts.
 *
 * @param baud The desired baud rate
 */
template<uint32_t baud>
FORCE_INLINE void
init() {
    // Set Baud Rate.
    uint16_t brate = ubrr_setting(baud);
    __BAUD_HIGH = static_cast<uint8_t>(brate >> 8);
    __BAUD_LOW = static_cast<uint8_t>(brate);

    /* Frame Format - 8 data, no parity */
    /* NEED URSEL FOR MEGA16/32 */
    __CTRLA = _BV(__CTRLA_U2X);
    __CTRLC = __CTRLC_ENABLE | _BV(__CTRLC_UCSZ1) |
              _BV(__CTRLC_UCSZ0);// | _BV(UPM1) | _BV(UPM0);

    /* Enable Rx and Tx, no interrupt */
    __CTRLB = _BV(__CTRLB_RXEN) | _BV(__CTRLB_TXEN);
}

/**
 * Get a character form the UART, blocking forever until one is received.
 *
 * If there is an error, this will clear the flag and loop.
 */
char
get_char();

/**
 * Put a character into the UART send buffer.
 *
 * This will wait until there is room.
 */
void
put_char(char c);

}

}
}
#endif

#endif /* _savr_sci_h_included_ */
