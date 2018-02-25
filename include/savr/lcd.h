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
#ifndef _savr_lcd_h_included_
#define _savr_lcd_h_included_

/**
 * @file lcd.h
 *
 * LCD Interface
 */

#include <stdint.h>
#include <stddef.h>

#include <savr/gpio.h>

namespace savr {

class LCD {
public:

    static const uint8_t READ_BUSYFLAG = 0x80;

    /**
     * Initialize the LCD
     *
     * @param d4        gpio::Pin for DB4
     * @param d5        gpio::Pin for DB5
     * @param d6        gpio::Pin for DB6
     * @param d7        gpio::Pin for DB7
     * @param rs        gpio::Pin for the RS line
     * @param rw        gpio::Pin for the R/W line
     * @param e         gpio::Pin for the Enable line
     */
    LCD(gpio::Pin d4, gpio::Pin d5, gpio::Pin d6, gpio::Pin d7,
        gpio::Pin rs, gpio::Pin rw, gpio::Pin e);


    /**
     * Turn on/off the cursor
     *
     * @param cursor Boolean true/false (true = show cursor)
     */
    void
    set_cursor(bool cursor);


    /**
     * Turn on/off the cursor blink
     *
     * @param blink Boolean true/false (true = blink)
     */
    void
    set_blink(bool blink);


    /**
     * Turn on/off the entire display
     *
     * @param on Boolean true/false (true = on)
     */
    void
    set_display(bool on);


    /**
     * Write a full string to the display
     *
     * @param string The null-terminated string to display
     */
    void
    write_string(const char *string);


    /**
     * Send a raw byte to the display
     *
     * @param byte  The byte to send
     * @param mode  RS line control
     */
    void
    write_byte(uint8_t byte, uint8_t mode = 0);


    /**
     * Send a command to the display
     *
     * @param cmd  The command byte to send
     */
    inline void
    write_cmd(uint8_t cmd) {
        write_byte(cmd);
    }


    /**
     * Write a single character to the display
     *
     * @param c  The character to write
     */
    inline void
    write_char(char c) {
        write_byte(c, 1);
    }


    /**
     * Clear the display, cursor to home position
     */
    inline void
    clear(void) {
        write_cmd(0x01);
    }


    /**
     * Set the cursor to the home position
     */
    inline void
    home(void) {
        write_cmd(0x02);
    }


    /**
     * Manually set the cursor position
     *
     * @param pos The DDRAM address
     */
    inline void
    set_pos(uint8_t pos) {
        write_cmd(0x80 | pos);
    }


    /**
     * Manually set the cursor position
     *
     * @return The current cursor position
     */
    inline uint8_t
    get_pos(void) {
        return _get_byte() & ~READ_BUSYFLAG;
    }


private:
    /**
     * Send a single nibble down with the given mode (RS)
     * @param nib  Nibble (least significant 4 bits)
     * @param mode RS control
     */
    void
    _write_nib(uint8_t nib, uint8_t mode = 0);


    /**
     * Wait for the busy flag to not be set
     */
    void
    _wait(void);


    /**
     * Change data direction
     */
    void
    _set_data_out(void);


    /**
     * Change data direction
     */
    void
    _set_data_in(void);


    /**
     * Read a nibble from the data lines
     */
    uint8_t
    _read_data_nibble(void);


    /**
     * Write a nibble to the data lines
     */
    void
    _set_data_nibble(uint8_t nibble);


    /**
     * Read a byte from the data lines
     *
     * @param mode  RS control. 0 for address/busy flag. 1 for data.
     */
    uint8_t
    _get_byte(uint8_t mode = 0);


    uint8_t _entry_mode;
    uint8_t _display_ctrl;
    uint8_t _display_shift;
    uint8_t _function_set;

    gpio::Pin _pin_d4;
    gpio::Pin _pin_d5;
    gpio::Pin _pin_d6;
    gpio::Pin _pin_d7;

    gpio::Pin _pin_rw;
    gpio::Pin _pin_e;
    gpio::Pin _pin_rs;

};
}

#endif /* _savr_lcd_h_included_ */
