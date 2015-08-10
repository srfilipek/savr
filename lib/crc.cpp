/*********************************************************************************
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
*********************************************************************************/

#include <savr/crc.h>


/**
 * @par Implementation Notes:
 */
uint8_t
crc_8(const uint8_t * data, size_t length, uint8_t crc, uint8_t poly) {
    while(length-->0) {
        crc ^= *data++;

        for(uint8_t ibit=0; ibit<8; ibit++) {
            if(crc & 0x80) {
                crc = (crc << 1) ^ poly;
            }else{
                crc <<= 1;
            }
        }

    }
    return crc;
}


/**
 * @par Implementation Notes:
 */
uint16_t
crc_16(const uint8_t * data, size_t length, uint16_t crc, uint16_t poly) {
    while(length-->0) {
        crc ^= ((uint16_t)*data++) << 8;

        for(uint8_t ibit=0; ibit<8; ibit++) {
            if(crc & 0x8000) {
                crc = (crc << 1) ^ poly;
            }else{
                crc <<= 1;
            }
        }

    }
    return crc;
}


