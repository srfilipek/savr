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

#include <savr/crc.h>
#include <savr/optimized.h>

/**
 * CRC-8
 */
constexpr
uint8_t
_crc_8(const uint8_t *data, size_t length, uint8_t crc, uint8_t poly) {
    while (length-- > 0) {
        crc ^= *data++;

        for (uint8_t ibit = 0; ibit < 8; ibit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }

    }
    return crc;
}

/**
 * CRC-8 with input bit-reversed
 */
constexpr
uint8_t
_crc_8_rev_in(const uint8_t *data, size_t length, uint8_t crc, uint8_t poly) {
    while (length-- > 0) {
        uint8_t data_byte = *data++;
        uint8_t mask = 0x01;

        for (uint8_t ibit = 0; ibit < 8; ibit++) {
            bool crc_set = crc & 0x80;
            bool data_set = data_byte & mask;

            crc <<= 1;
            mask <<= 1;
            if (data_set != crc_set) {
                crc ^= poly;
            }
        }
    }
    return crc;
}

/**
 * CRC-8 with both input and output bit-reversed
 */
constexpr
uint8_t
_crc_8_rev_both(const uint8_t *data, size_t length, uint8_t crc, uint8_t poly) {
    return savr::opt::bit_reverse(_crc_8_rev_in(data, length, crc, poly));
}

/**
 * CRC-16
 */
constexpr
static uint16_t
_crc_16(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly) {
    while (length-- > 0) {
        crc ^= ((uint16_t) *data++) << 8;

        for (uint8_t ibit = 0; ibit < 8; ibit++) {
            bool xor_poly = crc & 0x8000;

            crc <<= 1;
            if (xor_poly) {
                crc ^= poly;
            }
        }
    }
    return crc;
}

/**
 * CRC-16 with input bit-reversed
 */
constexpr
static uint16_t
_crc_16_rev_in(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly) {
    while (length-- > 0) {
        uint8_t data_byte = *data++;
        uint8_t mask = 0x01;

        for (uint8_t ibit = 0; ibit < 8; ibit++) {
            bool crc_set = crc & 0x8000;
            bool data_set = data_byte & mask;

            crc <<= 1;
            mask <<= 1;
            if (data_set != crc_set) {
                crc ^= poly;
            }
        }
    }
    return crc;
}

/**
 * CRC-16 with both input and output bit-reversed
 */
constexpr
static uint16_t
_crc_16_rev_both(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly) {
    return savr::opt::bit_reverse(_crc_16_rev_in(data, length, crc, poly));
}

/**
 * Ensure our implementations produce known results
 */
constexpr uint8_t data[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0x00, 0xa5, 0xff};

// CRC-8
static_assert(_crc_8(data, sizeof(data), 0x00,  0x07) == 0x73);
// CRC-8 CMDA2000
static_assert(_crc_8(data, sizeof(data), 0xFF,  0x9B) == 0x4C);
// CRC-8/MAXIM
static_assert(_crc_8_rev_both(data, sizeof(data), 0x00,  0x31) == 0x38);
// CRC-16 CCITT-FALSE
static_assert(_crc_16(data, sizeof(data), 0xFFFF,  0x1021) == 0xc35d);
// CRC-16 XMODEM
static_assert(_crc_16(data, sizeof(data), 0x0000,  0x1021) == 0xF263);
// CRC-16 CDMA2000
static_assert(_crc_16(data, sizeof(data), 0xFFFF,  0xC867) == 0x2CBA);
// CRC-16 Microchip, ATECC608 (not standard)
static_assert(_crc_16_rev_in(data, sizeof(data), 0, 0x8005) == 0xF25C);
// CRC-16 RIELLO 
static_assert(_crc_16_rev_both(data, sizeof(data), 0xB2AA, 0x1021) == 0x93D3);


namespace savr {
namespace crc {

/**
 * @par Implementation Notes:
 */
uint8_t
crc_8(const uint8_t *data, size_t length, uint8_t crc, uint8_t poly) {
    return _crc_8(data, length, crc, poly);
}

/**
 * @par Implementation Notes:
 */
uint8_t
crc_8_rev_in(const uint8_t *data, size_t length, uint8_t crc, uint8_t poly) {
    return _crc_8_rev_in(data, length, crc, poly);
}

/**
 * @par Implementation Notes:
 */
uint8_t
crc_8_rev_both(const uint8_t *data, size_t length, uint8_t crc, uint8_t poly) {
    return _crc_8_rev_both(data, length, crc, poly);
}

/**
 * @par Implementation Notes:
 */
uint16_t
crc_16(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly) {
    return _crc_16(data, length, crc, poly);
}

/**
 * @par Implementation Notes:
 */
uint16_t
crc_16_rev_in(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly) {
    return _crc_16_rev_in(data, length, crc, poly);
}

/**
 * @par Implementation Notes:
 */
uint16_t
crc_16_rev_both(const uint8_t *data, size_t length, uint16_t crc, uint16_t poly) {
    return _crc_16_rev_both(data, length, crc, poly);
}

}
}

