/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include <savr/gpio.h>
#include <savr/lcd.h>

using namespace savr;

#define LCD_ENTRYMODE_DISPLAYSHIFT_ON   _BV(0)
#define LCD_ENTRYMODE_DISPLAYSHIFT_OFF  0
#define LCD_ENTRYMODE_INC               _BV(1)
#define LCD_ENTRYMODE_DEC               0
#define LCD_ENTRYMODE                   _BV(2)

#define LCD_DISPLAY_BLINK_ON            _BV(0)
#define LCD_DISPLAY_BLINK_OFF           0
#define LCD_DISPLAY_CURSOR_ON           _BV(1)
#define LCD_DISPLAY_CURSOR_OFF          0
#define LCD_DISPLAY_DISPLAY_ON          _BV(2)
#define LCD_DISPLAY_DISPLAY_OFF         0
#define LCD_DISPLAY                     _BV(3)

#define LCD_SHIFT_LEFT                  _BV(2)
#define LCD_SHIFT_RIGHT                 0
#define LCD_SHIFT_SCREEN                _BV(2)
#define LCD_SHIFT_CURSOR                0
#define LCD_SHIFT                       _BV(4)

#define LCD_FUNCTION_5x8                _BV(2)
#define LCD_FUNCTION_5x11               0
#define LCD_FUNCTION_2LINE              _BV(3)
#define LCD_FUNCTION_1LINE              0
#define LCD_FUNCTION_8BIT               _BV(4)
#define LCD_FUNCTION_4BIT               0
#define LCD_FUNCTION                    _BV(5)



#define MIN(x, y) ((x) < (y) ? (x) : (y))


/**
 * @par Implementation notes:
 */
void
LCD::_set_data_out(void)
{
    gpio::low(_pin_d4);
    gpio::low(_pin_d5);
    gpio::low(_pin_d6);
    gpio::low(_pin_d7);
    gpio::out(_pin_d4);
    gpio::out(_pin_d5);
    gpio::out(_pin_d6);
    gpio::out(_pin_d7);
}


/**
 * @par Implementation notes:
 */
void
LCD::_set_data_in(void)
{
    gpio::low(_pin_d4);
    gpio::low(_pin_d5);
    gpio::low(_pin_d6);
    gpio::low(_pin_d7);
    gpio::in(_pin_d4);
    gpio::in(_pin_d5);
    gpio::in(_pin_d6);
    gpio::in(_pin_d7);
}


/**
 * @par Implementation notes:
 */
uint8_t
LCD::_read_data_nibble(void)
{
    uint8_t ret = 0;
    ret |= gpio::get(_pin_d7);
    ret <<= 1;
    ret |= gpio::get(_pin_d6);
    ret <<= 1;
    ret |= gpio::get(_pin_d5);
    ret <<= 1;
    ret |= gpio::get(_pin_d4);
    return ret;
}


/**
 * @par Implementation notes:
 */
void
LCD::_set_data_nibble(uint8_t nibble)
{
    gpio::set(_pin_d4, nibble & _BV(0));
    gpio::set(_pin_d5, nibble & _BV(1));
    gpio::set(_pin_d6, nibble & _BV(2));
    gpio::set(_pin_d7, nibble & _BV(3));
}



/**
 * @par Implementation notes:
 */
void
LCD::__LCD( gpio::Pin d4, gpio::Pin d5, gpio::Pin d6, gpio::Pin d7,
            gpio::Pin rs, gpio::Pin rw, gpio::Pin e)
{

    _pin_d4 = d4;
    _pin_d5 = d5;
    _pin_d6 = d6;
    _pin_d7 = d7;
    _pin_rs = rs;
    _pin_rw = rw;
    _pin_e  = e;

    gpio::out(_pin_e);
    gpio::out(_pin_rs);
    gpio::out(_pin_rw);
    gpio::low(_pin_e);
    gpio::low(_pin_rs);
    gpio::low(_pin_rw);

    _set_data_out();

    // Init for 4 data lines...
    _delay_ms(50); // Power-on delay
    _write_nib(0x03);
    _delay_ms(5);
    _write_nib(0x03);
    _delay_ms(5);
    _write_nib(0x03);

    _write_nib(0x02);
    _wait();

    _function_set   = LCD_FUNCTION  | LCD_FUNCTION_2LINE | LCD_FUNCTION_5x8 | LCD_FUNCTION_4BIT;
    _entry_mode     = LCD_ENTRYMODE | LCD_ENTRYMODE_INC | LCD_ENTRYMODE_DISPLAYSHIFT_OFF;
    _display_ctrl   = LCD_DISPLAY   | LCD_DISPLAY_DISPLAY_ON | LCD_DISPLAY_BLINK_OFF | LCD_DISPLAY_CURSOR_OFF;

    write_cmd(_function_set);
    write_cmd(_entry_mode);
    write_cmd(_display_ctrl);
    write_cmd(_display_shift);
    clear();

}


/**
 * @par Implementation notes:
 */
void
LCD::set_cursor(bool cursor)
{
    if(cursor) {
        _display_ctrl &= ~LCD_DISPLAY_CURSOR_OFF;
        _display_ctrl |=  LCD_DISPLAY_CURSOR_ON;
    }else{
        _display_ctrl &= ~LCD_DISPLAY_CURSOR_ON;
        _display_ctrl |=  LCD_DISPLAY_CURSOR_OFF;
    }
    write_cmd(_display_ctrl);
}


/**
 * @par Implementation notes:
 */
void
LCD::set_blink(bool blink)
{
    if(blink) {
        _display_ctrl &= ~LCD_DISPLAY_BLINK_OFF;
        _display_ctrl |=  LCD_DISPLAY_BLINK_ON;
    }else{
        _display_ctrl &= ~LCD_DISPLAY_BLINK_ON;
        _display_ctrl |=  LCD_DISPLAY_BLINK_OFF;
    }
    write_cmd(_display_ctrl);
}


/**
 * @par Implementation notes:
 */
void
LCD::set_display(bool on)
{
    if(on) {
        _display_ctrl &= ~LCD_DISPLAY_DISPLAY_OFF;
        _display_ctrl |=  LCD_DISPLAY_DISPLAY_ON;
    }else{
        _display_ctrl &= ~LCD_DISPLAY_DISPLAY_ON;
        _display_ctrl |=  LCD_DISPLAY_DISPLAY_OFF;
    }
    write_cmd(_display_ctrl);
}


/**
 * @par Implementation notes:
 */
uint8_t
LCD::_get_byte(uint8_t mode)
{
    uint8_t x;

    _set_data_in();

    gpio::high(_pin_rw);
    if(mode) gpio::high(_pin_rs);


    gpio::high(_pin_e);
    x = _read_data_nibble() << 4;
    gpio::low(_pin_e);

    // GPIO:X function call is enough delay...

    gpio::high(_pin_e);
    x |= _read_data_nibble();
    gpio::low(_pin_e);

    gpio::low(_pin_rw);
    gpio::low(_pin_rs);

    _set_data_out();

    return x;
}


/**
 * @par Implementation notes:
 */
void
LCD::write_byte(uint8_t byte, uint8_t mode)
{
    _wait();
    _write_nib(byte >> 4, mode);
    _write_nib(byte,      mode);
}


/**
 * @par Implementation notes:
 */
void
LCD::write_string(const char * string)
{
    while(*string) {
        write_char(*string++);
    }
}


/**
 * @par Implementation notes:
 */
void
LCD::_wait(void)
{
    while(_get_byte() & READ_BUSYFLAG) {
        // Nothing
    }
}


/**
 * @par Implementation notes:
 */
void
LCD::_write_nib(uint8_t nib, uint8_t mode)
{
    gpio::high(_pin_e);

    _set_data_nibble(nib);
    if(mode) gpio::high(_pin_rs);

    gpio::low(_pin_e);
    gpio::low(_pin_rs);
}

