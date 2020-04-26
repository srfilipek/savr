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

#include <avr/io.h>
#include <inttypes.h>

#include <savr/sci.h>

#ifndef SAVR_NO_SCI

using namespace savr;

char
sci::min::get_char() {
    for(;;) {
        uint8_t status = __CTRLA;
        // Wait for data
        if ((status & _BV(__CTRLA_RXC)) == 0) {
            continue;
        }

        // Reject errors
        if (status & _BV(__CTRLA_FE)) {
            (void)__DATAR;
            continue;
        }
        break;
    }

    return __DATAR;
}


void
sci::min::put_char(char c) {
    // Wait for space
    while ((__CTRLA & _BV(__CTRLA_UDRE)) == 0) {
    }

    __DATAR = c;
}

#endif
