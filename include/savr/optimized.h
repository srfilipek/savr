#ifndef _savr_optimized_h_Included_
#define _savr_optimized_h_Included_
/*********************************************************************************
 Copyright (C) 2013 by Stefan Filipek

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

/**
 * @file optimized.h
 *
 * Optimized math, bit twiddling, assembly
 *
 * Some common operations are not fully optimized by the GCC compiler. This
 * header file provides functionality that has been optimized at the
 * instruction set level.
 */

namespace Opt {


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
inline uint8_t
BitVal(uint8_t bit)
{
    uint8_t ret;
    asm volatile (
            "ldi %0, 0x01\n"
            "rjmp 2f\n"
            "1: lsl %0\n"
            "2: dec %1\n"
            "brpl 1b"
         :"=a" (ret)
         :"b"  (bit)
    );
    return ret;
}


}


#endif
