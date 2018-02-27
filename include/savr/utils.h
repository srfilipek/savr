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
#ifndef _savr_utils_h_included_
#define _savr_utils_h_included_

/**
 * @file utils.h
 *
 * Random, useful utilities.
 *
 * Contains a random collection of utilities that I have found
 * useful.
 *
 */

#include <stddef.h>
#include <stdint.h>

/**
 * Helpful macros
 */
#define ISAVR(x) defined(__AVR_ ## x ## __)
#define FORCE_INLINE __attribute__((always_inline)) inline


namespace savr {
namespace utils {

/**
 * Register manipulation helper for different bitfields.
 *
 * This provides compile-time constant generation help as well as runtime bit
 * manipulation routines.
 */
template<uint32_t shift, uint32_t bits>
class Bitfield {
public:
    static_assert((bits >= 1) && (bits <= 8), "Invalid bit size");
    static_assert((shift >= 0) && (shift <= 7), "Invalid shift");

    static constexpr uint8_t MASK = static_cast<uint8_t>((_BV(bits) - 1) << shift);
    static constexpr uint8_t SHIFT = shift;

    /**
     * Return the value properly shifted and masked
     *
     * @param value Base value (lsb aligned)
     * @return Shifted and masked value
     */
    static constexpr uint8_t set(uint8_t value) noexcept {
        return (value << SHIFT) & MASK;
    }

    /**
     * Return the base value from the raw register
     *
     * @param reg The register
     * @return Masked register, shifted down to be lsb aligned
     */
    static constexpr uint8_t get(uint8_t reg) noexcept {
        return (reg & MASK) >> SHIFT;
    }

    /**
     * Update a register based on the value (lsb aligned from get())
     *
     * @param reg: Current register
     * @param value: New value for the bitfield
     * @return Updated register
     */
    static constexpr uint8_t value_update(uint8_t reg, uint8_t value) noexcept {
        return (reg & ~MASK) | set(value);
    }

    /**
     * Update a register value based on the raw bits (positioned from set())
     *
     * @param reg: Current register
     * @param value: Masked and shifted value from set()
     * @return Updated register
     */
    static constexpr uint8_t raw_update(uint8_t reg, uint8_t raw) noexcept {
        return (reg & ~MASK) | raw;
    }
};


/**
 * Prints a bunch of hex
 *
 * Prints each byte out in capital hex, zero padded, and
 * separated by spaces. For large amounts of data, see
 * print_block().
 *
 * Relies on avr-libc and proper binding of stdout
 *
 * @param data a pointer to the data to print
 * @param size the number of bytes to print
 */
void
print_hex(const void *data, size_t size);


/**
 * Prints a block of hex in a pretty way
 *
 * Prints a block of hex out along with the associated address
 * and ASCII characters. Any line that needs padding has byte
 * values represented by "--"
 *
 * Relies on avr-libc and proper binding of stdout
 *
 * @param data  a pointer to the data to print
 * @param size  the number of bytes to print
 * @param index a starting address to print with the data
 * @param width the number of bytes to print on a single line
 */
void
print_block(const void *data, size_t size, uint32_t index = 0,
            size_t width = 16);

}
}

#endif /* _savr_utils_h_included_ */
