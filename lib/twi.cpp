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


#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <savr/utils.h>
#include <savr/cpp_pgmspace.h>
#include <savr/twi.h>
#include <savr/gpio.h>

using namespace savr;

static const char CPP_PROGMEM sent_data[]   = "Sent data, got";
static const char CPP_PROGMEM rcvd_data[]   = "Rcvd data and";
static const char CPP_PROGMEM slaw[]        = "SLA+W";
static const char CPP_PROGMEM slar[]        = "SLA+R";
static const char CPP_PROGMEM eack[]        = " ACK";
static const char CPP_PROGMEM enack[]       = " NACK";

#if     ISAVR(ATmega8)      || \
        ISAVR(ATmega48)     || ISAVR(ATmega88)      || ISAVR(ATmega168)     || \
        ISAVR(ATmega48P)    || ISAVR(ATmega88P)     || ISAVR(ATmega168P)    || \
        ISAVR(ATmega48PA)   || ISAVR(ATmega88PA)    || ISAVR(ATmega168PA)   || ISAVR(ATmega328P)

#define TWI_GPIO_SDA    gpio::C4
#define TWI_GPIO_SCL    gpio::C5

#elif   ISAVR(ATmega16)     || \
        ISAVR(ATmega32)     || \
        ISAVR(ATmega644)    || \
        ISAVR(ATmega164P)   || ISAVR(ATmega324P)    || ISAVR(ATmega644P)    || \
        ISAVR(ATmega164A)   || ISAVR(ATmega164PA)   || ISAVR(ATmega324A)    || ISAVR(ATmega324PA)   || \
        ISAVR(ATmega644A)   || ISAVR(ATmega644PA)   || ISAVR(ATmega1284)    || ISAVR(ATmega1284P)

#define TWI_GPIO_SDA    gpio::C1
#define TWI_GPIO_SCL    gpio::C0

#else
#warning Unsupported AVR target for TWI interface
#define SAVR_NO_TWI
#endif

#ifndef SAVR_NO_TWI

/**
 * @par Implementation notes:
 */
void
twi::init(uint32_t output_freq) {
    init(output_freq, false);
}


/**
 * @par Implementation notes:
 */
void
twi::init(uint32_t output_freq, bool pullup) {

    // SCL Freq = CPU Freq / ( 16 + 2(TWBR)(PrescalerValue) )
    // TWBR =  ((CPU Freq / SCL Freq) - 16) / 2 / (PrescalerValue)
    TWBR = ((F_CPU / output_freq) - 16) / 2;

    // Clear interrupt flag, enable the TWI
    TWCR = _BV(TWINT) | _BV(TWEN);

    if (pullup) {
        gpio::in<TWI_GPIO_SDA>();
        gpio::high<TWI_GPIO_SDA>();
        gpio::in<TWI_GPIO_SCL>();
        gpio::high<TWI_GPIO_SCL>();
    }
}


/**
 * @par Implementation notes:
 */
void
twi::print_state() {
    char temp[5];
    switch (twi::state()) {
        case TW_MT_DATA_ACK:
            puts_P(sent_data);
            puts_P(eack);
            break;
        case TW_MT_DATA_NACK:
            puts_P(sent_data);
            puts_P(enack);
            break;

        case TW_MT_SLA_ACK:
            puts_P(slaw);
            puts_P(eack);
            break;
        case TW_MT_SLA_NACK:
            puts_P(slaw);
            puts_P(enack);
            break;

        case TW_MR_ARB_LOST:
            puts_P(PSTR("Arb Lost"));
            break;

        case TW_MR_SLA_ACK:
            puts_P(slar);
            puts_P(eack);
            break;
        case TW_MR_SLA_NACK:
            puts_P(slar);
            puts_P(enack);
            break;

        case TW_REP_START:
            puts_P(PSTR("Rep Start"));
            break;
        case TW_START:
            puts_P(PSTR("Initial Start"));
            break;

        case TW_MR_DATA_ACK:
            puts_P(rcvd_data);
            puts_P(eack);
            break;
        case TW_MR_DATA_NACK:
            puts_P(rcvd_data);
            puts_P(enack);
            break;

        case TW_NO_INFO:
            puts_P(PSTR("No Info"));
            break;

        case TW_BUS_ERROR:
            puts_P(PSTR("Bus Error"));
            break;

        default:
            puts_P(PSTR("Status: "));
            printf(itoa((TWSR & TW_STATUS_MASK), temp, 16));
            break;
    }
}


/**
 * @par Implementation notes:
 */
void
twi::send(uint8_t b) {
    twi::wait();
    TWDR = b;
    TWCR = _BV(TWINT) | _BV(TWEN);
    twi::wait();
}


/**
 * @par Implementation notes:
 */
void
twi::send_async(uint8_t b) {
    twi::wait();
    TWDR = b;
    TWCR = _BV(TWINT) | _BV(TWEN);
}


/**
 * @par Implementation notes:
 */
uint8_t
twi::get_ack() {
    twi::wait();
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); // Enable ACK
    twi::wait();
    return TWDR;
}


/**
 * @par Implementation notes:
 */
uint8_t
twi::get() {
    twi::wait();
    TWCR = _BV(TWINT) | _BV(TWEN); // No ACK
    twi::wait();
    return TWDR;
}


/**
 * @par Implementation notes:
 */
uint8_t
twi::address(uint8_t address, bool read) {
    // Create start condition
    uint8_t state;
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    twi::wait();
    state = twi::state();
    if (state != TW_START && state != TW_REP_START) {
        return 1;
    }

    // Send address | RW (has waits...)
    twi::send((address << 1) | ((uint8_t) read));

    state = twi::state();
    if (state != TW_MR_SLA_ACK && state != TW_MT_SLA_ACK) {
        return 1;
    }

    return 0;
}


/**
 * @par Implementation notes:
 */
void
twi::stop() {
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}


/**
 * @par Implementation notes:
 */
void
twi::start() {
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
}


/**
 * @par Implementation notes:
 */
uint8_t
twi::state() {
    return (TWSR & TW_STATUS_MASK);
}

#endif
