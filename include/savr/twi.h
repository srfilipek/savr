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
#ifndef _savr_twi_h_included_
#define _savr_twi_h_included_

#include <stdint.h>
#include <stddef.h>

#include <util/twi.h>

#if defined(TWBR) && defined(TWCR) // Not everything has a TWI

namespace savr {
namespace twi {

static const bool RW_READ = true;
static const bool RW_WRITE = false;

/**
 * Initialize the TWI subsystem without internal pull-ups
 *
 * @param outputFreq    The desired TWI bus frequency in Hz
 */
void
init(uint32_t output_freq);


/**
 * Initialize the TWI subsystem with internal pull-up control
 *
 * @param outputFreq    The desired TWI bus frequency in Hz
 * @param pullups       Enable (true) or disable (false) internal pull-ups for SDA/SCL
 *
 * If pullups is false, the SDA and SCL lines are left untouched.
 */
void
init(uint32_t output_freq, bool pullups);


/**
 * Prints a textual description of the bus state
 */
void
print_state();


/**
 * Addresses the given endpoint for read or write
 *
 * @param address   The address of the endpoint
 * @param read      True to read, false to write
 * @return 0 on success, non-zero on error
 */
uint8_t
address(uint8_t address, bool read);


/**
 * Read a byte with acknowledgment
 *
 * @return The byte read
 */
uint8_t
get_ack();


/**
 * Read a byte with no acknowledgment
 *
 * @return The byte read
 */
uint8_t
get();


/**
 * Send a byte
 *
 * @param b     The byte to send
 */
void
send(uint8_t b);


/**
 * Send a byte without any waiting
 *
 * @param b     The byte to send
 */
void
send_async(uint8_t b);


/**
 * Send bus Start
 */
void
start();


/**
 * Send bus stop
 */
void
stop();


/**
 * Get the state of the bus
 *
 * @return The bus state byte
 *
 * See util/twi.h from the avr-libc for a state enumeration
 */
uint8_t
state();


/**
 * Polling wait on the TWI bus
 */
static inline void
wait() {
    while (!(TWCR & _BV(TWINT)));
}

}
}

#endif /* defined(TWBR) && defined(TWCR) */
#endif /* _savr_twi_h_included_ */
