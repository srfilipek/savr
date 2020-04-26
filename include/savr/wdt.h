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
#ifndef _savr_wdt_h_included_
#define _savr_wdt_h_included_

/**
 * @file wdt.h
 *
 * @brief Watchdog interface
 *
 * Watchdog interface that provides lightweight utility functions for
 * interacting with the built-in watchdog system.
 *
 * IMPORTANT IMPLEMENTATION NOTES
 *
 * This interface deviates from recommended practice for size considerations.
 *
 * First, no interrupt protection is performed. The caller is responsible for
 * disabling and re-enabling interrupts if needed before calling configure()
 * or off().
 *
 * Second, we do not attempt to maintain the current watchdog value when
 * modifying the configuration. This may cause a timeout during the operation.
 * This can be avoided by kicking the watchdog manually betfore calling
 * configure() or off(), if this is a concern.
 */

#include <avr/io.h>
#include <avr/wdt.h>

#include <savr/utils.h>

#if !defined(WDTCSR)
# if defined(WDTCR)
#  define WDTCSR WDTCR
# else
#  error Unknown watchdog register
# endif
#endif

namespace savr {
namespace wdt {

/**
 * Watchdog timeout
 */
typedef enum {
    WDT_16ms    = 0,
    WDT_32ms    = _BV(WDP0),
    WDT_64ms    = _BV(WDP1),
    WDT_125ms   = _BV(WDP0) | _BV(WDP1),
    WDT_250ms   = _BV(WDP2),
    WDT_500ms   = _BV(WDP0) | _BV(WDP2),
    WDT_1s      = _BV(WDP1) | _BV(WDP2),
    WDT_2s      = _BV(WDP0) | _BV(WDP1) | _BV(WDP2),
#if defined(WDP3)
    WDT_4s      = _BV(WDP3),
    WDT_8s      = _BV(WDP0) | _BV(WDP3),
#endif
} Timeout;

typedef enum {
    WDT_INT     = _BV(WDIE),
    WDT_RST     = _BV(WDE),
    WDT_INT_RST = _BV(WDIE) | _BV(WDE),
} Config;


/**
 * Kick the watchdog
 */
FORCE_INLINE
void
kick() {
    asm volatile("wdr");
}

/**
 * Configure the watchdog
 *
 * @param setting: Configuration setting
 * @param timeout: Timeout constant
 *
 * Note that this function will typically reduce to two instructions
 */
FORCE_INLINE
void
configure(Config setting, Timeout timeout) {
    //kick();
    WDTCSR = _BV(WDCE) | _BV(WDE);
    WDTCSR = setting | timeout;
}

/**
 * Disable the watchdog
 */
FORCE_INLINE
void
off() {
    //kick();
    MCUSR  &= ~_BV(WDRF);
    WDTCSR = _BV(WDCE) | _BV(WDE);
    WDTCSR = 0;
}

}
}

#endif /* _savr_wdt_h_included_ */
