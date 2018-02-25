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
#ifndef _savr_w1_h_included_
#define _savr_w1_h_included_

/**
 * @file w1.h
 *
 * @brief 1-Wire interface using a single GPIO pin.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <savr/cpp_pgmspace.h>
#include <savr/gpio.h>

namespace savr {
class W1 {

public:
    static const uint8_t EMPTY_TOKEN = 0;

    /**
     * @brief 64-bit 1-Wire device address type
     *
     * Internally, the address is stored in proper bit order in a 64bit
     * unsigned integer. This results in the 8-byte address ordering, when
     * accessed through the array field, to be:
     *   Field: [ CRC | 48-bit serial | Family ]
     *   Index:    7     6 5 4 3 2 1      0
     *
     * The ordering of bytes is a consequence of GCC's implementation of this
     * systems "endianness".
     *
     * The PrintAddress() function prints the hexadecimal characters in proper
     * address order, from 7 down to 0, to match the above field order.
     */
    typedef union {
        uint64_t raw;
        uint8_t array[8];
        struct {
            uint8_t family;
            uint8_t serial[6];
            uint8_t crc;
        };
    } Address;

    // Token for reentrant bus searches
    typedef uint8_t Token;


    /**
     * Create a 1-Wire interface on the given pin.
     *
     * @param pin       GPIO Pin to use for the bus.
     */
    explicit W1(gpio::Pin pin);


    /**
     * Cleans up and sets bit to high-impedance.
     */
    ~W1();


    /**
     * Reset the 1-Wire bus and detect presence.
     *
     * @return true if presence found, false otherwise.
     */
    bool
    reset();


    /**
     * Select the device with the given address using a Match ROM.
     *
     * @param address   The address of the source
     */
    void
    match_rom(const Address &address);


    /**
     * Select all devices on the bus.
     */
    void
    skip_rom();


    /**
     * Scan the bus for devices (0xF0 Search)
     *
     * This can be called multiple times while it returns True. The same
     * address must be passed back in, as it is used for state between
     * calls. For instance:
     *
     * @code
     *  W1::Address address;
     *  W1::Token   token = W1::EMPTY_TOKEN;
     *  while(wire->SearchROM(address, token)) {
     *      W1::PrintAddress(address);
     *      putchar('\n');
     *  }
     * @endcode
     *
     * @param address   An address to manipulate and update
     * @param token     Token that keeps state between calls. This must be 0 on
     *                  the first call. It must never be modified by the caller.
     *
     * @return true if we found a device, false otherwise.
     */
    bool
    search_rom(Address &address, Token &token);


    /**
     * Scan the bus for devices that are in an Alarm state (0xEC Search)
     *
     * This can be called multiple times while it returns True. The same
     * address must be passed back in, as it is used for state between
     * calls. For instance:
     *
     * @code
     *  W1::Address address;
     *  W1::Token   token = W1::EMPTY_TOKEN;
     *  while(wire->AlarmSearch(address, token)) {
     *      W1::PrintAddress(address);
     *      putchar('\n');
     *  }
     * @endcode
     *
     * @param address   An address to manipulate and update
     * @param token     Token that keeps state between calls. This must be 0 on
     *                  the first call. It must never be modified by the caller.
     *
     * @return true if we found a device, false otherwise.
     */
    bool
    alarm_search(Address &address, Token &token);


    /**
     * Read a bit from the bus
     *
     * @return 1 if bit is high, 0 otherwise
     */
    uint8_t
    read_bit();


    /**
     * Write a bit to the bus
     *
     * Writes a 1 if 'bit' is non-zero
     *
     * @param bit   Bit to write
     */
    void
    write_bit(bool bit);


    /**
     * Read a full byte from the bus
     * @return The byte read
     */
    uint8_t
    read_byte();


    /**
     * Write a full byte to the bus
     * @param byte  The byte write
     */
    void
    write_byte(uint8_t byte);


    /**
     * Read a series of bytes from the bus
     *
     * @param byte  Pointer to a destination byte array
     * @param size  Number of bytes to read
     */
    void
    read_bytes(uint8_t *byte, size_t size);


    /**
     * Write a series of bytes to the bus
     *
     * @param byte  Pointer to a source byte array
     * @param size  Number of bytes to write
     *
     * Writes bytes starting at 0, to size-1
     */
    void
    write_bytes(const uint8_t *byte, size_t size);


    /**
     * Set a bit in the address
     *
     * @param address   The address to manipulate
     * @param bit_num   Bit number to set (0 is LSB)
     * @param set       Set the bit to 1 if true, 0 otherwise
     */
    static void
    set_bit(Address &address, uint8_t bit_num, bool set);


    /**
     * Get a bit in the address
     *
     * @param address   The address to examine
     * @param bit_num   Bit number to get (0 is LSB)
     *
     * @return 0 if the bit is 0, 1 if the bit is 1...
     */
    static uint8_t
    get_bit(const Address &address, uint8_t bit_num);


    /**
     * Print the address to standard out
     *
     * @param address  The address to print
     */
    static void
    print_address(const Address &address);

protected:

    bool
    _searcher(uint8_t command, Address &address, Token &token);

    void
    _drive_low();

    void
    _release();

    bool
    _read_state();

private:
    gpio::Pin _pin; ///< GPIO Pin to control for this bus
};
}

#endif /* _savr_w1_h_included_ */
