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
#ifndef _savr_gpio_h_included_
#define _savr_gpio_h_included_

/**
 * @file gpio.h
 */

#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>

#include <savr/utils.h>

namespace savr {
namespace gpio {

/**
 * This data structure must be defined here to allow the template functions to
 * reduce down to sbi/cbi/sbis/sbic instructions.
 */
volatile uint8_t *const PORT_BANKS[] = {
#if defined(PORTA)
    &PORTA,
#endif
#if defined(PORTB)
    &PORTB,
#endif
#if defined(PORTC)
    &PORTC,
#endif
#if defined(PORTD)
    &PORTD,
#endif
#if defined(PORTE)
    &PORTE,
#endif
#if defined(PORTF)
    &PORTF,
#endif
#if defined(PORTG)
    &PORTG,
#endif
#if defined(PORTH)
    &PORTH,
#endif
#if defined(PORTJ)
    &PORTJ,
#endif
#if defined(PORTK)
    &PORTK,
#endif
#if defined(PORTL)
    &PORTL,
#endif
};

/**
 * This enumeration is an index into PORT_BANKS above.
 */
enum PortIdx : uint8_t {
#if defined(PORTA)
    PORTA_IDX,
#endif
#if defined(PORTB)
    PORTB_IDX,
#endif
#if defined(PORTC)
    PORTC_IDX,
#endif
#if defined(PORTD)
    PORTD_IDX,
#endif
#if defined(PORTE)
    PORTE_IDX,
#endif
#if defined(PORTF)
    PORTF_IDX,
#endif
#if defined(PORTG)
    PORTG_IDX,
#endif
#if defined(PORTH)
    PORTH_IDX,
#endif
#if defined(PORTJ)
    PORTJ_IDX,
#endif
#if defined(PORTK)
    PORTK_IDX,
#endif
#if defined(PORTL)
    PORTL_IDX,
#endif
};

#define MAKEPIN(x, y) (uint8_t)(((PORT##x##_IDX) << 4) | (y))

#define MAKEPINS(x)\
x##0 = MAKEPIN(x, 0),\
x##1 = MAKEPIN(x, 1),\
x##2 = MAKEPIN(x, 2),\
x##3 = MAKEPIN(x, 3),\
x##4 = MAKEPIN(x, 4),\
x##5 = MAKEPIN(x, 5),\
x##6 = MAKEPIN(x, 6),\
x##7 = MAKEPIN(x, 7),

/**
 * GPIO Pin Constants for the compiled system.
 *
 * For each port on the target, there will be a corresponding
 * port and pin definition in the form X# where 'X' is the
 * port letter and '#' is the pin number, 0-7.
 *
 * Examples are gpio::C3 or gpio::A0
 */
enum Pin: uint8_t {
#if defined(PORTA)
    MAKEPINS(A)
#endif
#if defined(PORTB)
    MAKEPINS(B)
#endif
#if defined(PORTC)
    MAKEPINS(C)
#endif
#if defined(PORTD)
    MAKEPINS(D)
#endif
#if defined(PORTE)
    MAKEPINS(E)
#endif
#if defined(PORTF)
    MAKEPINS(F)
#endif
#if defined(PORTG)
    MAKEPINS(G)
#endif
#if defined(PORTH)
    MAKEPINS(H)
#endif
#if defined(PORTJ)
    MAKEPINS(J)
#endif
#if defined(PORTK)
    MAKEPINS(K)
#endif
#if defined(PORTL)
    MAKEPINS(L)
#endif
    // Do not pass into gpio::*() functions(!!), but use for boundary checks
    NONE,
};


/* Internal functions */
inline volatile uint8_t *
PORTOF(uint8_t x) {
    return PORT_BANKS[x];
}

inline volatile uint8_t *
DDROF(uint8_t x) {
    return PORT_BANKS[x] - 1;
}

inline volatile uint8_t *
PINOF(uint8_t x) {
    return PORT_BANKS[(x)] - 2;
}


/**
 * Get the pin's value from the PIN register
 *
 * @param pin   The gpio::Pin to read
 * @return 0 if the pin is logic low, 1 if logic high
 */
uint8_t
get(gpio::Pin pin);


/**
 * Get the pin's value from the PIN register
 *
 * @tparam pin   The gpio::Pin to read
 * @return 0 if the pin is logic low, 1 if logic high
 */
template<gpio::Pin pin>
FORCE_INLINE uint8_t
get() {
    uint8_t _port = pin >> 4;
    uint8_t _pin = _BV(pin & 0xf);
    return ((*PINOF(_port) & _pin) ? 1 : 0);
}


/**
 * Set the pin (PORT register) high
 *
 * @param pin   The gpio::Pin to control
 */
void
high(gpio::Pin pin);


/**
 * Set the pin (PORT register) high
 *
 * @tparam pin   The gpio::Pin to control
 */
template<gpio::Pin pin>
FORCE_INLINE void
high() {
    uint8_t _port = pin >> 4;
    uint8_t _pin = _BV(pin & 0xf);
    *PORTOF(_port) |= _pin;
}


/**
 * Set the pin (PORT register) low
 *
 * @param pin   The gpio::Pin to control
 */
void
low(gpio::Pin pin);


/**
 * Set the pin (PORT register) low
 *
 * @tparam pin   The gpio::Pin to control
 */
template<gpio::Pin pin>
FORCE_INLINE void
low() {
    uint8_t _port = pin >> 4;
    uint8_t _pin = _BV(pin & 0xf);
    *PORTOF(_port) &= ~_pin;
}


/**
 * Set the pin direction to be an input
 *
 * @param pin   The gpio::Pin to control
 */
void
in(gpio::Pin pin);


/**
 * Set the pin direction to be an input
 *
 * @tparam pin   The gpio::Pin to control
 */
template<gpio::Pin pin>
FORCE_INLINE void
in() {
    uint8_t _port = pin >> 4;
    uint8_t _pin = _BV(pin & 0xf);
    *DDROF(_port) &= ~_pin;
}


/**
 * Set the pin direction to be an output
 *
 * @param pin   The gpio::Pin to control
 */
void
out(gpio::Pin pin);


/**
 * Set the pin direction to be an output
 *
 * @tparam pin   The gpio::Pin to control
 */
template<gpio::Pin pin>
FORCE_INLINE void
out() {
    uint8_t _port = pin >> 4;
    uint8_t _pin = _BV(pin & 0xf);
    *DDROF(_port) |= _pin;
}


/**
 * Set the pin high or low.
 *
 * @param pin   The gpio::Pin to control
 * @param set   zero = Low, non-zero = High
 */
void
set(gpio::Pin pin, uint8_t set);


/**
 * Set the pin high or low.
 *
 * @tparam pin  The gpio::Pin to control
 * @param set   zero = Low, non-zero = High
 */
template<gpio::Pin pin>
void
set(uint8_t set) {
    if (set) {
        high<pin>();
    } else {
        low<pin>();
    }
}


/**
 * Toggle the pin high or low.
 *
 * If the PORT is currently low, set to high, and vice versa.
 *
 * @param pin   The gpio::Pin to control
 */
void
toggle(gpio::Pin pin);


/**
 * Toggle the pin high or low.
 *
 * If the PORT is currently low, set to high, and vice versa.
 *
 * @tparam pin  The gpio::Pin to control
 */
template<gpio::Pin pin>
void
toggle() {
    uint8_t _port = pin >> 4;
    uint8_t _pin = _BV(pin & 0xf);

    if (*PORTOF(_port) & _pin) {
        low<pin>();
    } else {
        high<pin>();
    }
}

}
}

#endif /* _savr_gpio_h_included_ */
