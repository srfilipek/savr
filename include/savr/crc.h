/*******************************************************************************
 Copyright (C) 2020 by Stefan Filipek

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
#ifndef _savr_crc_h_included_
#define _savr_crc_h_included_

/**
 * @file crc.h
 *
 * Compact CRC calculations for various polynomials.
 *
 * Each function can take a polynomial and a starting CRC state. The highest
 * bit of the polynomial is assumed and should not be set. For instance, the
 * CRC-7 polynomial (x^7 + x^3 + 1) should be 0x09 (x^3 + 1). See crc_8 for
 * how to make a CRC-7.
 */

#include <stdint.h>
#include <stddef.h>

namespace savr {
namespace crc {

/**
 * A CRC 8 calculation
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data
 * @param starting crc (or previous for continuations)
 * @param poly the polynomial to use (highest bit is assumed)
 *
 * @return the resultant CRC (8bit)
 *
 * To make a CRC 7, feed in a shifted polynomial and unshift the result.
 * Intermediate CRC values should remain unshifted.
 *
 * Example for SD cards:
 *
 * uint8_t result = CRC::crc_8(data, length, 0, 0x09 << 1) >> 1;
 *
 */
uint8_t
crc_8(const uint8_t *bytes, size_t length, uint8_t crc, uint8_t poly);


/**
 * A CRC-8 calculation, input is bit-reversed
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data
 * @param starting crc (or previous for continuations)
 * @param poly the polynomial to use (highest bit is assumed)
 *
 * @return the resultant CRC (8bit)
 */
uint8_t
crc_8_rev_in(const uint8_t *bytes, size_t length, uint8_t crc, uint8_t poly);


/**
 * A CRC-8 calculation, both input and output is bit-reversed
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data
 * @param starting crc (or previous for continuations)
 * @param poly the polynomial to use (highest bit is assumed)
 *
 * @return the resultant CRC (8bit)
 */
uint8_t
crc_8_rev_both(const uint8_t *bytes, size_t length, uint8_t crc, uint8_t poly);


/**
 * A CRC-16 calculation
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data
 * @param starting crc (or previous for continuations)
 * @param poly the polynomial to use (highest bit is assumed)
 *
 * @return the resultant CRC
 */
uint16_t
crc_16(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly);


/**
 * A CRC-16 calculation, input is bit-reversed
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data
 * @param starting crc (or previous for continuations)
 * @param poly the polynomial to use (highest bit is assumed)
 *
 * @return the resultant CRC
 */
uint16_t
crc_16_rev_in(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly);


/**
 * A CRC-16 calculation, both input and othput is bit-reversed
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data
 * @param starting crc (or previous for continuations)
 * @param poly the polynomial to use (highest bit is assumed)
 *
 * @return the resultant CRC
 */
uint16_t
crc_16_rev_both(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly);

}
}

#endif /* _savr_crc_h_included_ */
