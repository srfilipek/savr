/*******************************************************************************
 Copyright (C) 2018 by Stefan Filipek

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
#ifndef _savr_clock_h_included_
#define _savr_clock_h_included_

#include <stddef.h>
#include <stdint.h>

namespace savr {
namespace clock {

// The goal is to get a tick every 1ms
constexpr uint32_t TICKS_PER_SEC = 1000;

// Rough clock scaling
#if F_CPU >= 8000000
constexpr uint32_t CLOCK_SCALE = 64;
#elif F_CPU >= 4000000
constexpr uint32_t CLOCK_SCALE = 32;
#else
constexpr uint32_t CLOCK_SCALE = 8;
#endif

constexpr uint32_t OCR_VALUE = F_CPU / CLOCK_SCALE / TICKS_PER_SEC;

// Ensure our result didn't get rounded somewhere
static_assert((OCR_VALUE * CLOCK_SCALE * TICKS_PER_SEC) == F_CPU,
              "CPU frequency does not produce an integer counter");

/**
 * Initialize the clock subsystem
 */
void init();

/**
 * Get the number of ticks elapsed so far
 *
 * @return System ticks (milliseconds)
 */
uint32_t ticks();

/**
 * Get the least significant byte of the number of ticks elapsed
 *
 * This is useful if you only need to delay for a short period of time and don't
 * want to overhead of a 32bit integer and the associated interrupt management.
 *
 * @return Least significant byte of the system ticks (milliseconds)
 */
uint8_t ticks_byte();

}
}

#endif // _savr_clock_h_included_
