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
LCD::_setDataOut(void)
{
    GPIO::low(_pinD4);
    GPIO::low(_pinD5);
    GPIO::low(_pinD6);
    GPIO::low(_pinD7);
    GPIO::out(_pinD4);
    GPIO::out(_pinD5);
    GPIO::out(_pinD6);
    GPIO::out(_pinD7);
}


/**
 * @par Implementation notes:
 */
void
LCD::_setDataIn(void)
{
    GPIO::low(_pinD4);
    GPIO::low(_pinD5);
    GPIO::low(_pinD6);
    GPIO::low(_pinD7);
    GPIO::in(_pinD4);
    GPIO::in(_pinD5);
    GPIO::in(_pinD6);
    GPIO::in(_pinD7);
}


/**
 * @par Implementation notes:
 */
uint8_t
LCD::_readDataNibble(void)
{
    uint8_t ret = GPIO::get(_pinD4);
    ret |= GPIO::get(_pinD5) << 1;
    ret |= GPIO::get(_pinD6) << 2;
    ret |= GPIO::get(_pinD7) << 3;
    return ret;
}


/**
 * @par Implementation notes:
 */
void
LCD::_setDataNibble(uint8_t nibble)
{
    GPIO::set(_pinD4, nibble & _BV(0));
    GPIO::set(_pinD5, nibble & _BV(1));
    GPIO::set(_pinD6, nibble & _BV(2));
    GPIO::set(_pinD7, nibble & _BV(3));
}



/**
 * @par Implementation notes:
 */
void
LCD::__LCD( GPIO::Pin d4, GPIO::Pin d5, GPIO::Pin d6, GPIO::Pin d7,
            GPIO::Pin rs, GPIO::Pin rw, GPIO::Pin e)
{

    _pinD4 = d4;
    _pinD5 = d5;
    _pinD6 = d6;
    _pinD7 = d7;
    _pinRS = rs;
    _pinRW = rw;
    _pinE  = e;

    GPIO::out(_pinE);
    GPIO::out(_pinRS);
    GPIO::out(_pinRW);
    GPIO::low(_pinE);
    GPIO::low(_pinRS);
    GPIO::low(_pinRW);

    _setDataOut();

    // Init for 4 data lines...
    _delay_ms(50); // Power-on delay
    _outNib(0x03);
    _delay_ms(5);
    _outNib(0x03);
    _delay_ms(5);
    _outNib(0x03);

    _outNib(0x02);
    _wait();

    _functionSet    = LCD_FUNCTION  | LCD_FUNCTION_2LINE | LCD_FUNCTION_5x8 | LCD_FUNCTION_4BIT;
    _entryMode      = LCD_ENTRYMODE | LCD_ENTRYMODE_INC | LCD_ENTRYMODE_DISPLAYSHIFT_OFF;
    _displayCtrl    = LCD_DISPLAY   | LCD_DISPLAY_DISPLAY_ON | LCD_DISPLAY_BLINK_OFF | LCD_DISPLAY_CURSOR_OFF;

    outCmd(_functionSet);
    outCmd(_entryMode);
    outCmd(_displayCtrl);
    outCmd(_displayShift);
    clear();

}


/**
 * @par Implementation notes:
 */
void
LCD::setCursor(bool cursor)
{
    if(cursor) {
        _displayCtrl &= ~LCD_DISPLAY_CURSOR_OFF;
        _displayCtrl |=  LCD_DISPLAY_CURSOR_ON;
    }else{
        _displayCtrl &= ~LCD_DISPLAY_CURSOR_ON;
        _displayCtrl |=  LCD_DISPLAY_CURSOR_OFF;
    }
    outCmd(_displayCtrl);
}


/**
 * @par Implementation notes:
 */
void
LCD::setBlink(bool blink)
{
    if(blink) {
        _displayCtrl &= ~LCD_DISPLAY_BLINK_OFF;
        _displayCtrl |=  LCD_DISPLAY_BLINK_ON;
    }else{
        _displayCtrl &= ~LCD_DISPLAY_BLINK_ON;
        _displayCtrl |=  LCD_DISPLAY_BLINK_OFF;
    }
    outCmd(_displayCtrl);
}


/**
 * @par Implementation notes:
 */
void
LCD::setDisplay(bool on)
{
    if(on) {
        _displayCtrl &= ~LCD_DISPLAY_DISPLAY_OFF;
        _displayCtrl |=  LCD_DISPLAY_DISPLAY_ON;
    }else{
        _displayCtrl &= ~LCD_DISPLAY_DISPLAY_ON;
        _displayCtrl |=  LCD_DISPLAY_DISPLAY_OFF;
    }
    outCmd(_displayCtrl);
}


/**
 * @par Implementation notes:
 */
uint8_t
LCD::_getByte(uint8_t mode)
{
    uint8_t x;

    _setDataIn();

    GPIO::high(_pinRW);
    if(mode) GPIO::high(_pinRS);


    GPIO::high(_pinE);
    x = _readDataNibble() << 4;
    GPIO::low(_pinE);

    // GPIO:X function call is enough delay...

    GPIO::high(_pinE);
    x |= _readDataNibble();
    GPIO::low(_pinE);

    GPIO::low(_pinRW);
    GPIO::low(_pinRS);

    _setDataOut();

    return x;
}


/**
 * @par Implementation notes:
 */
void
LCD::outByte(uint8_t byte, uint8_t mode)
{
    _wait();
    _outNib(byte >> 4, mode);
    _outNib(byte,      mode);
}


/**
 * @par Implementation notes:
 */
void
LCD::outString(const char * string)
{
    while(*string)
        outChar(*string++);
}


/**
 * @par Implementation notes:
 */
void
LCD::_wait(void)
{
    while(_getByte() & LCD_READ_BUSYFLAG) ;
}


/**
 * @par Implementation notes:
 */
void
LCD::_outNib(uint8_t nib, uint8_t mode)
{
    GPIO::high(_pinE);

    _setDataNibble(nib);
    if (mode) GPIO::high(_pinRS);

    GPIO::low(_pinE);
    GPIO::low(_pinRS);
}

