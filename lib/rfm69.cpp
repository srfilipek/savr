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
#include <stdio.h>
#include <math.h>
#include <avr/pgmspace.h>

#include <savr/gpio.h>
#include <savr/spi.h>
#include <savr/rfm69.h>
#include <savr/utils.h>
#include <savr/optimized.h>

using namespace savr;


namespace {
/**
 * Common code for setting or reading registers
 *
 * @param[in] reg: Register address
 * @param[in] tx: Value to send
 *
 * @return Value read
 */
uint8_t
_common_trx(rfm69::Reg reg, uint8_t tx, bool write) {
    if (write) {
        reg = static_cast<rfm69::Reg>(reg | rfm69::REG_WRITE);
    }

    gpio::low<rfm69::PIN_SS>();
    spi::trx_byte(reg);
    uint8_t res = spi::trx_byte(tx);
    gpio::high<rfm69::PIN_SS>();

    return res;
}

constexpr uint32_t _calc_frf_reg(uint32_t center_freq) {
    auto frf = static_cast<uint32_t>(roundf(center_freq / rfm69::F_STEP));
    return frf;
}

constexpr uint16_t _calc_bitrate_reg(uint32_t bitrate) {
    return static_cast<uint16_t>(roundf(rfm69::F_XOSC / bitrate));
}

constexpr uint16_t _calc_fdev_reg(uint32_t freq_dev) {
    return static_cast<uint16_t>(roundf(freq_dev / rfm69::F_STEP));
}

/// Ensure our calculations match expected values from the docs
static_assert(_calc_frf_reg(915000000uL) == 0xe4c000);
static_assert(_calc_bitrate_reg(1200) == 0x682b);
static_assert(_calc_bitrate_reg(4800) == 0x1a0b);
static_assert(_calc_bitrate_reg(76738uL) == 0x01a1);
static_assert(_calc_bitrate_reg(200000uL) == 0x00a0);
static_assert(_calc_bitrate_reg(300000uL) == 0x006b);
static_assert(_calc_fdev_reg(5000) == 0x52);


/**
 * The RSSI measurement during the last Rx
 */
short _last_rssi;

/**
 * The gain used during the last Rx
 */
uint8_t _last_gain;
}

void
rfm69::init(uint32_t bitrate, uint32_t center_freq, uint32_t freq_dev) {
    gpio::high<PIN_SS>();
    gpio::out<PIN_SS>();

    gpio::in<PIN_DIO0>();
    gpio::low<PIN_DIO0>();

    for (auto i : INIT_REG) {
        write_reg(i.reg, i.val);
    }

    // Setup the FSK parameters
    set_fsk_params(bitrate, center_freq, freq_dev);
}

size_t
rfm69::rx_pdu(void *dst, size_t length) {
    write_reg(REG_DIO_MAP_1, DIO0_PKT_RX_PAYLOAD_READY);

    set_mode(MODE_RX, true);

    // Wait for a packet
    while (gpio::get<PIN_DIO0>() == 0) {};

    _last_rssi = sample_rssi(true);
    _last_gain = LNA_CURRENT_GAIN::get(read_reg(REG_LNA));

    // Go back to sleep once done receiving
    set_mode(MODE_SLEEP, false);

    // Read out the packet from the FIFO
    uint8_t packet_length;
    packet_length = read_reg(REG_FIFO);
    read_reg(REG_FIFO,
             dst, length < packet_length ? length : packet_length);

    return packet_length;
}

void
rfm69::tx_pdu(void *src, size_t length) {
    // Fill up the FIFO in sleep/stdby mode
    set_mode(MODE_STDBY, true);
    write_reg(REG_DIO_MAP_1, DIO0_PKT_TX_PACKET_SENT);

    if (length > MTU) {
        length = MTU;
    }

    // Payload is length + data
    write_reg(REG_FIFO, length);
    write_reg(REG_FIFO, src, length);

    // Trigger the transmit
    set_mode(MODE_TX, true);

    // Go back to sleep once done sending
    while (gpio::get<PIN_DIO0>() == 0) {};
    set_mode(MODE_SLEEP, false);
}

//bool
//rfm69::tx_pdu_arq(void *src, size_t length, size_t timeout,
//                  uint8_t retry) {
//    bool successful = false;
//
//    // Fill up the FIFO in sleep/stdby mode
//    set_mode(MODE_STDBY, true);
//    write_reg(REG_DIO_MAP_1, DIO0_PKT_TX_PACKET_SENT);
//
//    // Payload is length + data
//    write_reg(REG_FIFO, length);
//    write_reg(REG_FIFO, src, length);
//
//    set_mode(MODE_TX, true);
//
//    // Wait for it to finish sending
//    while (gpio::get<PIN_DIO0>() == 0) {};
//
//
//    // Go back to sleep when done
//    set_mode(MODE_SLEEP, false);
//    return successful;
//}

void
rfm69::set_fsk_params(uint32_t bitrate, uint32_t center_freq,
                      uint32_t freq_dev) {
    // If the freq_dev is not set, auto-set it based on a particular mod index
    if (freq_dev == 0) {
        freq_dev = (bitrate * MODULATION_INDEX_TARGET) / 2;
    }

    // Datasheet has a hard minimum, plus some fudge
    if (freq_dev < FREQ_DEV_MIN) {
        freq_dev = FREQ_DEV_MIN;
    }
    if (freq_dev > F_DEV_MAX) {
        freq_dev = F_DEV_MAX;
    }
    printf_P(PSTR("F_DEV: %lu\n"), freq_dev);

    /**
     * The RFM69 docs seem to be a bit misleading. The total bandwidth is not
     * just a function of the bitrate, but also the frequency deviation. The
     * RxBw filter must take both into account.
     *
     * BW_20db = 2 x (F_DEV + BR / 2)
     *
     * So for single-side RxBw, make it at least F_DEV + BR / 2
     */
    uint32_t min_rxbw = freq_dev + bitrate / 2;
    printf_P(PSTR("Min RxBw: %lu\n"), min_rxbw);

    // Find the table entry for the RxBw settings
    // Something something binary search is faster but bigger something
    size_t idx ;
    for (idx = 0; idx < (utils::array_size(RXBW_FSK) - 1); ++idx) {
        if (RXBW_FSK[idx].freq >= min_rxbw) break;
    }
    uint8_t rxbw = RXBW_FSK[idx].rxbw_val;
    printf_P(PSTR("RxBw act %lu: 0x%02x\n"), RXBW_FSK[idx].freq, rxbw);

    // Adjust F_DEV bases on the actual rxbw
    freq_dev += (RXBW_FSK[idx].freq - min_rxbw);
    printf_P(PSTR("New F_DEV: %lu\n"), freq_dev);

#if defined(ENABLE_AFC)
    // For the AFC functionality, add in extra slop for the oscillator offsets

    uint32_t lo_offset_fudge = center_freq / PPM_20_DIV;
    printf_P(PSTR("LO Fudge: %lu\n"), lo_offset_fudge);

    uint32_t min_rxbw_afc = min_rxbw + lo_offset_fudge;
    printf_P(PSTR("Min RxBwAfc: %lu\n"), min_rxbw_afc);

    // Since it's always >= min_rxbw, just continue the loop from the last idx
    for (; idx < (utils::array_size(RXBW_FSK) - 1); ++idx) {
        if (RXBW_FSK[idx].freq >= min_rxbw_afc) break;
    }
    uint8_t rxbw_afc = RXBW_FSK[idx].rxbw_val;
    printf_P(PSTR("RxBwAfc act %lu: 0x%02x\n"), RXBW_FSK[idx].freq,
             rxbw_afc);
#endif

    // A change in the center frequency will only be taken into account when the
    // least significant byte FrfLsb in RegFrfLsb is written.
    auto frf = _calc_frf_reg(center_freq);
    write_reg(REG_FRF_MSB, opt::byte_2(frf));
    write_reg(REG_FRF_MID, opt::byte_1(frf));
    write_reg(REG_FRF_LSB, opt::byte_0(frf));

    auto bitrate_reg = _calc_bitrate_reg(bitrate);
    write_reg(REG_BITRATE_MSB, opt::byte_1(bitrate_reg));
    write_reg(REG_BITRATE_LSB, opt::byte_0(bitrate_reg));

    auto freq_dev_reg = _calc_fdev_reg(freq_dev);
    write_reg(REG_FDEV_MSB, opt::byte_1(freq_dev_reg));
    write_reg(REG_FDEV_LSB, opt::byte_0(freq_dev_reg));

    write_reg(REG_RX_BW, rxbw);
#if defined(ENABLE_AFC)
    write_reg(REG_AFC_BW, rxbw_afc);
#endif
}

void
rfm69::set_mode(uint8_t mode, bool wait) {
    uint8_t reg = read_reg(REG_OP_MODE);
    reg = MODE::raw_update(reg, mode);
    write_reg(REG_OP_MODE, reg);

    // Don't return until the mode is ready
    if (wait) {
        poll_all_bits(REG_IRQ_FLAGS_1, IRQ_1_MODE_READY);
    }
}

void
rfm69::set_transmit_power(uint8_t power) {
    uint8_t reg = read_reg(REG_PA_LEVEL);
    reg = OUTPUT_POWER::value_update(reg, power);
    write_reg(REG_PA_LEVEL, reg);
}

uint8_t
rfm69::get_transmit_power() {
    uint8_t reg = read_reg(REG_PA_LEVEL);
    return OUTPUT_POWER::get(reg);
}

uint8_t
rfm69::read_reg(rfm69::Reg reg) {
    return _common_trx(reg, 0, false);
}

void
rfm69::read_reg(rfm69::Reg reg, void *dst, size_t length) {
    gpio::low<PIN_SS>();
    spi::trx_byte(reg);
    spi::read_block(reinterpret_cast<uint8_t *>(dst), length, 0);
    gpio::high<PIN_SS>();
}

void
rfm69::write_reg(rfm69::Reg reg, uint8_t value) {
    (void) _common_trx(reg, value, true);
}

void
rfm69::write_reg(rfm69::Reg reg, void *src, size_t length) {
    reg = static_cast<Reg>(reg | REG_WRITE);

    gpio::low<PIN_SS>();
    spi::trx_byte(reg);
    spi::write_block(reinterpret_cast<uint8_t *>(src), length);
    gpio::high<PIN_SS>();
}

uint8_t
rfm69::poll_all_bits(rfm69::Reg reg, uint8_t mask) {
    uint8_t value;
    do {
        value = _common_trx(reg, 0, false);
    } while ((value & mask) != mask);

    return value;
}

uint8_t
rfm69::poll_any_bits(rfm69::Reg reg, uint8_t mask) {
    uint8_t value;
    do {
        value = _common_trx(reg, 0, false);
    } while ((value & mask) == 0);

    return value;
}

bool
rfm69::check_reg(rfm69::Reg reg, uint8_t mask) {
    return (read_reg(reg) & mask) == mask;
}

short
rfm69::get_last_rssi() {
    return _last_rssi;
}

uint8_t
rfm69::get_last_gain() {
    return _last_gain;
}

short
rfm69::sample_rssi(bool read_only) {
    if (!read_only) {
        write_reg(REG_RSSI_CONFIG, RSSI_START);
        poll_all_bits(REG_RSSI_CONFIG, RSSI_DONE);
    }
    return -static_cast<short>(read_reg(REG_RSSI_VALUE)) / 2;
}