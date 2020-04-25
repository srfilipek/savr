/*******************************************************************************
 Copyright (C) 2018 by Stefan Filipek

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
#ifndef _savr_rfm69_h_included_
#define _savr_rfm69_h_included_

/**
 * @file rfm69.h
 */

#include <stdint.h>
#include <stddef.h>

#include <savr/gpio.h>
#include <savr/rfm69_const.h>
#include <savr/rfm69_settings.h>

namespace savr {
namespace rfm69 {

/**
 * Initialize the radio.
 *
 * This reconfigures most registers and sets up the modulation properties.
 * See set_fsk_params for more details.
 *
 * @param bitrate: Data rate in bps
 * @param center_freq: Center frequency in Hz
 * @param freq_dev: FSK deviation frequency in Hz
 */
void
init(uint32_t bitrate = 5000,
     uint32_t center_freq = 915000000uLL,
     uint32_t freq_dev = 0);

/**
 * Receive a packet data unit (PDU)
 *
 * This function will only return PDUs that pass the CRC check. It expects the
 * PDU to conform to the MPDU size, else it will be dropped.
 *
 * @param[out] dst: Destination buffer
 * @param[in] length: Maximum number of bytes to receive
 * @return Number of bytes actually in packet
 *
 * The packet may be truncated if the destination is not large enough. There
 * is an absolute maximum of MTU.
 */
size_t
rx_pdu(void *dst, size_t length);

/**
 * Send a packet data unit (PDU)
 *
 * @param[in] src: Source buffer
 * @param[in] length: Number of bytes to send (from src)
 *
 * If the packet is larger than MTU it will be truncated.
 */
void
tx_pdu(void *src, size_t length);

/**
 * Send a packet data unit (PDU) and retry
 *
 * This employs a Stop-And-Wait ARQ.
 *
 * @param[in] src: Source buffer
 * @param[in] length: Number of bytes to send (from src)
 * @param[in] timeout: ACK receive timeout in milliseconds
 * @param[in] retry: Number of times to retry
 * @return True if sent successfully, false otherwise
 *
 * If the packet is larger than MTU it will be truncated.
 */
//bool
//tx_pdu_arq(void *src, size_t length, size_t timeout, uint8_t retry);

/**
 * Configure the FSK parameters.
 *
 * This function will automatically adjust a number of settings on the radio
 * depending on the given parameters.
 *
 * Calculated values:
 *  - Frequency deviation (if 0) to bitrate x MODULATION_INDEX_TARGET / 2
 *  - RxBw: Receive bandwidth (filter), set to be > freq_dev + bitrate / 2
 *  - RxBwAfc: Same as above, plus some slop for oscillator offsets
 *
 * @param bitrate: Data rate in bps
 * @param center_freq: Center frequency in Hz
 * @param freq: Frequency deviation in Hz
 */
void
set_fsk_params(uint32_t bitrate, uint32_t center_freq, uint32_t freq_dev);

/**
 * Set the mode
 *
 * @param mode: Desired mode.
 * @param wait: Wait for the mode to be ready
 */
void
set_mode(uint8_t mode, bool wait = false);

/**
 * Set transmit power
 *
 * @param power: Power level from 0 - 31
 */
void
set_transmit_power(uint8_t power);

/**
 * Get the currently set transmit power
 *
 * @return Power level from 0 - 31
 */
uint8_t
get_transmit_power();

/**
 * Raw register read access.
 *
 * @param reg: Register address
 *
 * @return Register value read
 */
uint8_t
read_reg(rfm69::Reg reg);

/**
 * Stream register reads
 *
 * @param[in] reg: Register address
 * @param[out] dst: Destination buffer
 * @param[in] length: Number of bytes to read (into dst)
 */
void
read_reg(rfm69::Reg reg, void *dst, size_t length);

/**
 * Raw register write access.
 *
 * @param reg: Register address
 * @param value: Register value
 */
void
write_reg(rfm69::Reg reg, uint8_t value);

/**
 * Stream register writes.
 *
 * @param[in] reg: Register address
 * @param[in] src: Source buffer
 * @param[in] length: Number of bytes to write (from src)
 */
void
write_reg(rfm69::Reg reg, void *src, size_t length);

/**
 * Get the RSSI from the last rx_pdu
 *
 * @return Measured RSSI during rx
 */
short
get_last_rssi();

/**
 * Get the gain used during the last rx_pdu
 *
 * @return Gain value from the LNA
 */
uint8_t
get_last_gain();

/**
 * Perform an RSSI measurement.
 *
 * By default, this will trillger an RSSI measurement. Note that the mode is
 * not set. The caller must ensure the radio is in the proper mode.
 *
 * @param read_only: Set true to skip the trigger/wait
 * @return Read RSSI value
 */
short
sample_rssi(bool read_only=false);

/**
 * Wait for all given bits to be set in the given register
 *
 * @param[in] reg: Register address
 * @param[in] mask: bitmask to match
 */
uint8_t
poll_all_bits(rfm69::Reg reg, uint8_t mask);

/**
 * Wait for any given bits to be set in the given register
 *
 * @param[in] reg: Register address
 * @param[in] mask: bitmask to match
 */
uint8_t
poll_any_bits(rfm69::Reg reg, uint8_t mask);

/**
 * Check a particular set of bits in the given register
 *
 * @param[in] reg: Register address
 * @param[in] mask: bitmask to match
 * @return true if the mask is set, false otherwise
 */
bool
check_reg(rfm69::Reg reg, uint8_t mask);

}
}

#endif
