#ifndef _savr_spi_h_included_
#define _savr_spi_h_included_
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

/**
 * @file spi.h
 *
 * A simple SPI interface.
 *
 * Notes:
 *  The SS line must be manually set by the user.
 */

namespace SPI {


    /**
     * Initialize the SPI subsystem
     *
     * @param spiFreq   Desired SPI clock frequency
     */
    void init(uint32_t spiFreq);


    /**
     * Write (send) a block of data over the SPI
     *
     * @param input a pointer to the source data
     * @param length the size of the source data
     */
    void write_block(const uint8_t *input, size_t length);


    /**
     * Reads a block of data from the SPI
     *
     * @param input a pointer to the destination buffer
     * @param length the number of bytes to read
     * @param filler a byte to send continuously while reading
     */
    void read_block(uint8_t *input, size_t length, uint8_t filler);


    /**
     * Tx/Rx a byte
     *
     * @param input the byte to send
     *
     * @return the byte read from the SPI line
     */
    uint8_t trx_byte(uint8_t input);


    /**
     * Set the default SS line for this chip high
     */
    void ss_high(void);

    /**
     * Set the default SS line for this chip low
     */
    void ss_low(void);

};

#endif /* _savr_spi_h_included_ */

