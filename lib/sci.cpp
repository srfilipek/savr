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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>

#include <savr/sci.h>
#include <savr/queue.h>
#include <savr/utils.h>

#include <savr/sci_defs.h>

#ifndef SAVR_NO_SCI

using namespace savr;

static FILE my_stdout;
static FILE my_stdin;

static int
write_char(char, FILE *);

static int
read_char(FILE *);

typedef Queue<uint8_t, 8> IOBuffer;

//! Circular receive buffer
static IOBuffer tx_buffer;

//! Circular transmit buffer
static IOBuffer rx_buffer;


/**
 * Put a character into the UART queue
 *
 * Blocking Function - Place a single character on the TxBuffer
 */
int
write_char(char input, FILE *stream) {
    uint8_t err;
    if (input == '\n')
        write_char('\r', stream);

    auto *buff = reinterpret_cast<IOBuffer *>(fdev_get_udata(stream));

    do {
        err = buff->enq(static_cast<uint8_t>(input));
    } while (err);
    __CTRLB |= _BV(__CTRLB_UDRIE);
    return 0;
}


/**
 * Get a character from the UART queue
 *
 * Blocking Function - Get next char on the RxBuffer
 */
int
read_char(FILE *stream) {
    char ret_val;
    uint8_t err;
    auto *buff = reinterpret_cast<IOBuffer *>(fdev_get_udata(stream));
    do {
        err = buff->deq((uint8_t *) &ret_val);
    } while (err); // Poll till something is in buffer
    return ret_val;
}


/**
 * Get the size of a stream.
 */
size_t
sci::size(FILE *stream) {
    auto *buff = reinterpret_cast<IOBuffer *>(fdev_get_udata(stream));
    return buff->size();
}


/**
 * Initialize the SCI
 *
 * This must be called to initialize the SCI and bind
 * stdin and stdout to the serial port.
 */
void
sci::init(uint32_t baud) {
    // Set Baud Rate.
    uint16_t brate = ubrr_setting(baud);
    __BAUD_HIGH = static_cast<uint8_t>(brate >> 8);
    __BAUD_LOW = static_cast<uint8_t>(brate);

    /* Frame Format - 8 data, no parity */
    /* NEED URSEL FOR MEGA16/32 */
    __CTRLA = _BV(__CTRLA_U2X);
    __CTRLC = __CTRLC_ENABLE | _BV(__CTRLC_UCSZ1) |
              _BV(__CTRLC_UCSZ0);// | _BV(UPM1) | _BV(UPM0);

    /* Enable Rx and Tx, and interrupt */
    __CTRLB = _BV(__CTRLB_RXCIE) | _BV(__CTRLB_RXEN) | _BV(__CTRLB_TXEN);

    stdout = &my_stdout;
    stdin = &my_stdin;
    fdev_setup_stream(stdout, write_char, nullptr, _FDEV_SETUP_WRITE);
    fdev_setup_stream(stdin, nullptr, read_char, _FDEV_SETUP_READ);
    fdev_set_udata(stdout, (void *) &tx_buffer);
    fdev_set_udata(stdin, (void *) &rx_buffer);
}


/**
 * Handle received data
 */
ISR(__RX_VECT) {
    uint8_t rx_data = __DATAR;
    rx_buffer.enq(rx_data); // Fail silently
}


/**
 * Handle transmitting data
 */
ISR(__TX_VECT) {
    uint8_t tx_data;
    uint8_t err;

    err = tx_buffer.deq(&tx_data);
    if (err)
        __CTRLB &= ~_BV(__CTRLB_UDRIE);
    else
        __DATAR = tx_data;
}

#endif
