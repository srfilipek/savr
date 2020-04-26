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
#ifndef __savr_sci_defs_h_included__
#define __savr_sci_defs_h_included__

#include <avr/io.h>
#include <avr/interrupt.h>

#include <savr/utils.h>

#if     ISAVR(ATmega8)      || \
        ISAVR(ATmega16)     || \
        ISAVR(ATmega32)
    #define __BAUD_HIGH    UBRRH
    #define __BAUD_LOW     UBRRL
    #define __CTRLA        UCSRA
    #define __CTRLB        UCSRB
    #define __CTRLC        UCSRC
    #define __CTRLA_RXC    RXC
    #define __CTRLA_TXC    TXC
    #define __CTRLA_UDRE   UDRE
    #define __CTRLA_FE     FE
    #define __CTRLA_DOR    DOR
    #define __CTRLA_UPE    UPE
    #define __CTRLB_UDRIE  UDRIE
    #define __CTRLB_RXCIE  RXCIE
    #define __CTRLB_RXEN   RXEN
    #define __CTRLB_TXEN   TXEN
    #define __CTRLC_ENABLE _BV(URSEL)
    #define __CTRLC_UCSZ1  UCSZ1
    #define __CTRLC_UCSZ0  UCSZ0
    #define __DATAR        UDR
    #define __RX_VECT      USART_RXC_vect
    #define __TX_VECT      USART_UDRE_vect

#elif   ISAVR(ATmega8515)
    #define __BAUD_HIGH    UBRRH
    #define __BAUD_LOW     UBRRL
    #define __CTRLA        UCSRA
    #define __CTRLB        UCSRB
    #define __CTRLC        UCSRC
    #define __CTRLA_RXC    RXC
    #define __CTRLA_TXC    TXC
    #define __CTRLA_UDRE   UDRE
    #define __CTRLA_FE     FE
    #define __CTRLA_DOR    DOR
    #define __CTRLA_UPE    UPE
    #define __CTRLB_UDRIE  UDRIE
    #define __CTRLB_RXCIE  RXCIE
    #define __CTRLB_RXEN   RXEN
    #define __CTRLB_TXEN   TXEN
    #define __CTRLC_ENABLE _BV(URSEL)
    #define __CTRLC_UCSZ1  UCSZ1
    #define __CTRLC_UCSZ0  UCSZ0
    #define __DATAR        UDR
    #define __RX_VECT      USART_RX_vect
    #define __TX_VECT      USART_UDRE_vect

#elif   ISAVR(ATmega48)     || ISAVR(ATmega88)      || ISAVR(ATmega168)     || \
        ISAVR(ATmega48P)    || ISAVR(ATmega88P)     || ISAVR(ATmega168P)    || \
        ISAVR(ATmega48PA)   || ISAVR(ATmega88PA)    || ISAVR(ATmega168PA)   || ISAVR(ATmega328P)
    #define __BAUD_HIGH    UBRR0H
    #define __BAUD_LOW     UBRR0L
    #define __CTRLA        UCSR0A
    #define __CTRLB        UCSR0B
    #define __CTRLC        UCSR0C
    #define __CTRLA_RXC    RXC0
    #define __CTRLA_TXC    TXC0
    #define __CTRLA_UDRE   UDRE0
    #define __CTRLA_FE     FE0
    #define __CTRLA_DOR    DOR0
    #define __CTRLA_UPE    UPE0
    #define __CTRLB_UDRIE  UDRIE0
    #define __CTRLB_RXCIE  RXCIE0
    #define __CTRLB_RXEN   RXEN0
    #define __CTRLB_TXEN   TXEN0
    #define __CTRLC_ENABLE 0
    #define __CTRLC_UCSZ1  UCSZ01
    #define __CTRLC_UCSZ0  UCSZ00
    #define __DATAR        UDR0
    #define __RX_VECT      USART_RX_vect
    #define __TX_VECT      USART_UDRE_vect


#elif   ISAVR(ATmega644)    || \
        ISAVR(ATmega164P)   || ISAVR(ATmega324P)    || ISAVR(ATmega644P)    || \
        ISAVR(ATmega164A)   || ISAVR(ATmega164PA)   || ISAVR(ATmega324A)    || ISAVR(ATmega324PA)   || \
        ISAVR(ATmega644A)   || ISAVR(ATmega644PA)   || ISAVR(ATmega1284)    || ISAVR(ATmega1284P)
    #define __BAUD_HIGH    UBRR0H
    #define __BAUD_LOW     UBRR0L
    #define __CTRLA        UCSR0A
    #define __CTRLB        UCSR0B
    #define __CTRLC        UCSR0C
    #define __CTRLA_RXC    RXC0
    #define __CTRLA_TXC    TXC0
    #define __CTRLA_UDRE   UDRE0
    #define __CTRLA_FE     FE0
    #define __CTRLA_DOR    DOR0
    #define __CTRLA_UPE    UPE0
    #define __CTRLB_UDRIE  UDRIE0
    #define __CTRLB_RXCIE  RXCIE0
    #define __CTRLB_RXEN   RXEN0
    #define __CTRLB_TXEN   TXEN0
    #define __CTRLC_ENABLE 0
    #define __CTRLC_UCSZ1  UCSZ01
    #define __CTRLC_UCSZ0  UCSZ00
    #define __DATAR        UDR0
    #define __RX_VECT      USART0_RX_vect
    #define __TX_VECT      USART0_UDRE_vect

#else
#warning Unsupported AVR target for SCI interface
#define SAVR_NO_SCI
#endif

#define __GETBAUD(base, baud) ((base)/16/(baud)-1)

#endif
