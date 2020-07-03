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
#ifndef _savr_optimized_h_included_
#define _savr_optimized_h_included_

/**
 * @file optimized.h
 *
 * Optimized math, bit twiddling, assembly
 *
 * Some common operations are not fully optimized by the GCC compiler. This
 * header file provides functionality that has been optimized at the
 * instruction set level.
 */

#include <stdint.h>
#include <stddef.h>

#include <savr/utils.h>

namespace savr {
namespace opt {

/**
 * This is the same thing as _BV() bit shift macro, but it keeps it
 * contained in a single byte, instead of automatic 16bit promotion.
 *
 * Obviously, there are restrictions to the size of "bit". Anything
 * above 7 is undefined. Well, not really; you know what'll happen...
 *
 * @param bit The bit number to set
 * @return A byte with the corresponding bit set
 */
FORCE_INLINE uint8_t
bit_val(uint8_t bit) {
    uint8_t ret;
    asm volatile (
    "ldi %0, 0x01\n"
        "rjmp 2f\n"
        "1: lsl %0\n"
        "2: dec %1\n"
        "brpl 1b"
    :"=r" (ret)
    :"r"  (bit)
    );
    return ret;
}


/**
 * Use the SWAP instruction to swap nibbles
 */
FORCE_INLINE uint8_t
swap_nibbles(uint8_t val) {
    asm volatile (
    "swap %0"
    :"=r" (val)
    :"0"  (val)
    );
    return val;
}

/**
 * Return the highest byte of a 32-bit integer
 */
FORCE_INLINE uint8_t
byte_3(uint32_t val) {
    uint8_t ret;
    asm volatile (
    "mov %0, %D1"
    :"=r" (ret)
    :"r"  (val)
    );
    return ret;
}

/**
 * Return the second to highest byte of a 32-bit integer
 */
FORCE_INLINE uint8_t
byte_2(uint32_t val) {
    uint8_t ret;
    asm volatile (
    "mov %0, %C1"
    :"=r" (ret)
    :"r"  (val)
    );
    return ret;
}

/**
 * Return the second to lowest byte of a 32-bit integer
 */
FORCE_INLINE uint8_t
byte_1(uint32_t val) {
    uint8_t ret;
    asm volatile (
    "mov %0, %B1"
    :"=r" (ret)
    :"r"  (val)
    );
    return ret;
}

/**
 * Return the lowest byte of a 32-bit integer
 */
FORCE_INLINE uint8_t
byte_0(uint32_t val) {
    uint8_t ret;
    asm volatile (
    "mov %0, %A1"
    :"=r" (ret)
    :"r"  (val)
    );
    return ret;
}

/**
 * Return the highest byte of a 16-bit integer
 */
FORCE_INLINE uint8_t
byte_1(uint16_t val) {
    uint8_t ret;
    asm volatile (
    "mov %0, %B1"
    :"=r" (ret)
    :"r"  (val)
    );
    return ret;
}

/**
 * Return the lowest byte of a 16-bit integer
 */
FORCE_INLINE uint8_t
byte_0(uint16_t val) {
    uint8_t ret;
    asm volatile (
    "mov %0, %A1"
    :"=r" (ret)
    :"r"  (val)
    );
    return ret;
}

constexpr uint8_t
bit_reverse(uint8_t x) {
    x = (((x & 0xaa) >> 1) | ((x & 0x55) << 1));
    x = (((x & 0xcc) >> 2) | ((x & 0x33) << 2));
    x = (((x & 0xf0) >> 4) | ((x & 0x0f) << 4));
    return x;
}

constexpr uint16_t
bit_reverse(uint16_t x) {
    x = (((x & 0xaaaa) >> 1) | ((x & 0x5555) << 1));
    x = (((x & 0xcccc) >> 2) | ((x & 0x3333) << 2));
    x = (((x & 0xf0f0) >> 4) | ((x & 0x0f0f) << 4));
    x = (x >> 8) | (x << 8);
    return x;
}

constexpr uint32_t
bit_reverse(uint32_t x) {
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    x = (x >> 16) | (x << 16);
    return x;
}

// Implementation checks
static_assert(bit_reverse((uint8_t)0x31) == (uint8_t)0x8c);
static_assert(bit_reverse((uint8_t)0xa5) == (uint8_t)0xa5);
static_assert(bit_reverse((uint16_t)0x813a) == 0x5c81);
static_assert(bit_reverse((uint32_t)0x0013a5ff) == 0xffa5c800);

}
}

#endif
