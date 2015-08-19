/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

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
*********************************************************************************/

#include <avr/io.h>
#include <stdio.h>

#include <savr/gpio.h>
#include <savr/optimized.h>

using namespace savr;

/**
 * @par Implementation notes:
 */
void
gpio::set(gpio::Pin pin, uint8_t set)
{
    if(set) {
        high(pin);
    }else{
        low(pin);
    }
}


/**
 * @par Implementation notes:
 */
void
gpio::toggle(gpio::Pin pin)
{
    uint8_t _port = pin / 8;
    uint8_t _pin  = opt::bit_val(pin % 8);

    if(PORTOF(_port) & _pin) {
        low(pin);
    }else{
        high(pin);
    }
}


/**
 * @par Implementation notes:
 */
uint8_t
gpio::get(gpio::Pin pin)
{
    uint8_t _port = pin / 8;
    uint8_t _pin  = opt::bit_val(pin % 8);
    return ((PINOF(_port) & _pin) ? 1 : 0);
}


/**
 * @par Implementation notes:
 */
void
gpio::high(gpio::Pin pin)
{
    uint8_t _port = pin / 8;
    uint8_t _pin  = opt::bit_val(pin % 8);
    PORTOF(_port) |= _pin;
}


/**
 * @par Implementation notes:
 */
void
gpio::low(gpio::Pin pin)
{
    uint8_t _port = pin / 8;
    uint8_t _pin  = opt::bit_val(pin % 8);
    PORTOF(_port) &= ~_pin;
}


/**
 * @par Implementation notes:
 */
void
gpio::in(gpio::Pin pin)
{
    uint8_t _port = pin / 8;
    uint8_t _pin  = opt::bit_val(pin % 8);
    DDROF(_port) &= ~_pin;
}


/**
 * @par Implementation notes:
 */
void
gpio::out(gpio::Pin pin)
{
    uint8_t _port = pin / 8;
    uint8_t _pin  = opt::bit_val(pin % 8);
    DDROF(_port) |= _pin;
}

