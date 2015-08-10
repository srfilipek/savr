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
#include <util/atomic.h>
#include <util/delay_basic.h>

#include <stdio.h>

#include <savr/w1.h>
#include <savr/optimized.h>


/**
 * After examining the code, and some testing, adjusting by the given number of
 * cycles accounts for call and setup time for delays and GPIO read/writes
 */
#define DELAY_CALL_ADJ      4
#define MAYBE_ADJ(val, by)  ((val) < (by) ? 0 : (val) - (by))
#define DELAY_FOR(x)        ((0.000001*F_CPU)/4 * (x))
#define CALC_DELAY(x)       MAYBE_ADJ(DELAY_FOR(x), DELAY_CALL_ADJ)

#define DELAY(x) _delay_loop_2(DELAY_##x)


static uint16_t DELAY_A = CALC_DELAY(6);
static uint16_t DELAY_B = CALC_DELAY(64);
static uint16_t DELAY_C = CALC_DELAY(60);
static uint16_t DELAY_D = CALC_DELAY(10);
static uint16_t DELAY_E = CALC_DELAY(9);
static uint16_t DELAY_F = CALC_DELAY(55);
static uint16_t DELAY_G = CALC_DELAY(0);
static uint16_t DELAY_H = CALC_DELAY(480);
static uint16_t DELAY_I = CALC_DELAY(70);
static uint16_t DELAY_J = CALC_DELAY(410);


/**
 * @par Implementation notes:
 * GCC creates two copies of the constructor and destructors. This is the 'real'
 * constructor code in a separate function to reduce code size.
 *
 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=3187
 */
void
W1::__W1(GPIO::Pin pin)
{
    _pin  = pin;

    // Set to tristate
    GPIO::low(_pin);
    GPIO::in(_pin);
}


/**
 * @par Implementation notes:
 */
W1::~W1()
{
    _release();
}


/**
 * @par Implementation notes:
 */
bool
W1::reset()
{
    bool presence = false;
    DELAY(G);
    _drive_low();
    DELAY(H);       // Must delay *at least* this amount
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _release();
        DELAY(I);
        presence = (_read_state() == 0);
    }
    DELAY(J);
    return presence;
}


/**
 * @par Implementation notes:
 */
void
W1::match_rom(const Address &address)
{
    write_byte(0x55);
    write_bytes(address.array, 8);
}


/**
 * @par Implementation notes:
 */
void
W1::skip_rom(void)
{
    write_byte(0xCC);
}


/**
 * @par Implementation notes:
 */
bool
W1::search_rom(Address &address, Token &token)
{
    return _searcher(0xF0, address, token);
}


/**
 * @par Implementation notes:
 */
bool
W1::alarm_search(Address &address, Token &token)
{
    return _searcher(0xEC, address, token);
}


/**
 * @par Implementation notes:
 */
bool
W1::_searcher(uint8_t command, Address &address, Token &token)
{

    // Note: 1-based bit counting
    uint8_t  last_zero_path = 0;    // Last zero taken at a discrepancy
    uint8_t  current_bit    = 0;    // Current bit counter
    uint8_t  search_dir     = 0;    // 1 or 0 for next bit?
    uint8_t  bits;


    // Did we finish last time? Reset state for next call.
    if(token == 0xFF) {
        token = 0;
        return false;
    }

    // Always reset when beginning a new search
    if(!reset()) {
        return false;
    }

    // Begin search
    write_byte(command);


    // Scan down 64 bits of the ROM...
    while(++current_bit <= 64) {

        bits  = read_bit();       // Next bit value
        bits |= read_bit() << 1;  // Complement

        switch(bits) {
        case 0: // Discrepancy
            if(current_bit == token) {

                // We took the 0 path, now take the 1 path
                search_dir = 1;

            }else if(current_bit > token) {

                // New discrepancy... take the 0 path
                search_dir      = 0;
                last_zero_path  = current_bit;

            }else{

                // Old discrepancy... take the old path from the address
                if(get_bit(address, current_bit-1) == 0) {
                    search_dir      = 0;
                    last_zero_path  = current_bit;
                }else{
                    search_dir      = 1;
                }

            }
            break;

        case 1: // Only a 1
            search_dir = 1;
            break;

        case 2: // Only a 0
            search_dir = 0;
            break;

        case 3: // No devices!
            // Fallthrough
        default:
            return false;
        }
        set_bit(address, current_bit-1, search_dir);
        write_bit(search_dir);
    }

    token = last_zero_path;

    // Check if we're done (no more 0 paths for which we still need to search
    // the 1's path)
    if(token == 0) {
        token = 0xFF;
    }

    return true;
}


/**
 * @par Implementation notes:
 */
uint8_t
W1::read_bit()
{
    bool state;

    // These operations are time sensitive...
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        _drive_low();
        DELAY(A);
        _release();
        DELAY(E);
        state = _read_state();
    }
    DELAY(F);
    return (uint8_t)state;
}


/**
 * @par Implementation notes:
 */
void
W1::write_bit(bool bit)
{
    // These operations are time sensitive...
    if(bit) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            _drive_low();
            DELAY(A);
            _release();
        }
        DELAY(B);
    }else{
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            _drive_low();
            DELAY(C);
            _release();
        }
        DELAY(D);
    }
}


/**
 * @par Implementation notes:
 * Read LSB first.
 */
uint8_t
W1::read_byte(void)
{
    uint8_t byte = 0;
    for(uint8_t i=0; i<8; ++i) {
        byte >>= 1;
        if(read_bit()) {
            byte |= 0x80;
        }
    }
    return byte;
}


/**
 * @par Implementation notes:
 * Write LSB first.
 */
void
W1::write_byte(uint8_t byte)
{
    for(uint8_t i=0; i<8; ++i) {
        write_bit(byte & 0x01);
        byte >>= 1;
    }
}


/**
 * @par Implementation notes:
 */
void
W1::read_bytes(uint8_t *byte, size_t size)
{
    for(size_t i=0; i<size; ++i) {
        byte[i] = read_byte();
    }
}


/**
 * @par Implementation notes:
 */
void
W1::write_bytes(const uint8_t *byte, size_t size)
{
    for(size_t i=0; i<size; ++i) {
        write_byte(byte[i]);
    }
}


/**
 * @par Implementation notes:
 */
__attribute__ ((noinline)) void
W1::_drive_low()
{
    // Tri-state to low, DDR to 1
    GPIO::out(_pin);
}


/**
 * @par Implementation notes:
 */
__attribute__ ((noinline)) void
W1::_release()
{
    // Low to tri-state, DDR to 0
    GPIO::in(_pin);
}


/**
 * @par Implementation notes:
 */
__attribute__ ((noinline)) bool
W1::_read_state()
{
    return static_cast<bool>(GPIO::get(_pin));
}


/**
 * @par Implementation notes:
 */
void
W1::set_bit(Address &address, uint8_t bitNum, bool set)
{
    uint8_t bit     = Opt::bit_val(bitNum%8);
    uint8_t byte    = bitNum / 8;

    if(set) {
        address.array[byte] |= bit;
    }else{
        address.array[byte] &= ~bit;
    }
}


/**
 * @par Implementation notes:
 */
uint8_t
W1::get_bit(const Address &address, uint8_t bitNum)
{
    return !!(address.array[bitNum/8] & Opt::bit_val(bitNum%8));
}


/**
 * @par Implementation notes:
 */
void
W1::print_address(const Address &address)
{
    uint8_t i=8;
    while(i-->0) {
        printf_P(PSTR("%02x"), address.array[i]);
    }
}

