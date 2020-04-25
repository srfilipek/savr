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

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>

#include <savr/utils.h>
#include <savr/optimized.h>
#include <savr/clock.h>


using namespace savr;

#if defined(TCCR2)
#define __PRESCALE_REG TCCR2
#define __PRESCALE_BIT(n) _BV(CS2##n)
#define __WGM_REG TCCR2
#define __WGM_BIT(n) _BV(WGM2##n)
#define __OCR_REG OCR2
#define __INT_MSK_REG TIMSK
#define __INT_FLG_REG TIFR
#define __INT_MSK_MASK _BV(OCIE2)
#define __INT_FLG_MASK _BV(OCF2)
#define __INT_VECT TIMER2_COMP_vect

#elif defined(TCCR2B)
#define __PRESCALE_REG TCCR2B
#define __PRESCALE_BIT(n) _BV(CS2##n)
#define __WGM_REG TCCR2A
#define __WGM_BIT(n) _BV(WGM2##n)
#define __OCR_REG OCR2A
#define __INT_MSK_REG TIMSK2
#define __INT_FLG_REG TIFR2
#define __INT_MSK_MASK _BV(OCIE2A)
#define __INT_FLG_MASK _BV(OCF2A)
#define __INT_VECT TIMER2_COMPA_vect

#elif defined(TCCR0)
#define __PRESCALE_REG TCCR0
#define __PRESCALE_BIT(n) _BV(CS0##n)
#define __WGM_REG TCCR0
#define __WGM_BIT(n) _BV(WGM0##n)
#define __OCR_REG OCR0
#define __INT_MSK_REG TIMSK
#define __INT_FLG_REG TIFR
#define __INT_MSK_MASK _BV(OCIE0)
#define __INT_FLG_MASK _BV(OCF0)
#define __INT_VECT TIMER0_COMP_vect

#else
#warning Unsupported AVR target for clock system
#define SAVR_NO_CLOCK
#endif

#ifndef SAVR_NO_CLOCK

/**
 * Calculate the clock prescale register value based on clock::CLOCK_SCALE
 * @return Register value for the prescaler (0 if it could not be found)
 */
constexpr uint8_t prescale_reg_val() {
    switch(clock::CLOCK_SCALE) {
        case 64:
            return __PRESCALE_BIT(2);
        case 32:
            return __PRESCALE_BIT(1) | __PRESCALE_BIT(0);
        case 8:
            return __PRESCALE_BIT(1);
        default:
            // Fall through to end of function
            break;
    }
    return 0;
}
static_assert(prescale_reg_val() != 0, "Invalid clock prescale");


/// Millisecond counter storage
volatile uint32_t _ticks;


/**
 * Initialize the clock system.
 */
void
clock::init() {
    _ticks = 0;

    // Set both registers to zero and OR in the necessary bits. This covers the
    // case when both registers are actually the same (ATMega8, 32, 8515, ...)
    // We also don't have to worry about the odd (mis)placement of the WGM22 bit
    // since we don't use it...
    __PRESCALE_REG = 0;
    __WGM_REG = 0;

    __OCR_REG = clock::OCR_VALUE;
    __INT_MSK_REG = __INT_MSK_MASK;

    // Enable the counter last
    __WGM_REG |= __WGM_BIT(1); // CTC mode so the counter is cleared on match
    __PRESCALE_REG |= prescale_reg_val();
}

uint32_t
clock::ticks() {
    uint32_t ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        ret = _ticks;
    }
    return ret;
}


uint8_t
clock::ticks_byte() {
    static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__);
    return *reinterpret_cast<volatile uint8_t*>(&_ticks);
}


ISR(__INT_VECT) {
    _ticks += 1;
    // Flag is automatically cleared for us
}

#endif
