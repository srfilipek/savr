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
#ifndef _savr_rfm69_settings_h_included_
#define _savr_rfm69_settings_h_included_

/**
 * @file rfm69_const.h
 */

#include <stdint.h>
#include <stddef.h>
#include <avr/sfr_defs.h>

#include <savr/rfm69_const.h>
#include <savr/gpio.h>

namespace savr {
namespace rfm69 {

//#define ENABLE_AFC

/**
 * Static configuration of the pin settings so we can take advantage of
 * interrupts and other optimizations.
 */

/// Slave select for SPI interface
const gpio::Pin PIN_SS = gpio::B4;

/// Used for interrupts and quick status polling
const gpio::Pin PIN_DIO0 = gpio::B2;

/// Maximum user payload size
const uint8_t MTU = 64;

/// Maximum overall size of a PDU
const uint8_t MPDU = MTU + 2;

/// Target index
const uint8_t MODULATION_INDEX_TARGET = 4;

/// Spec sheet, page 20, plus 25%
static const uint32_t FREQ_DEV_MIN = 750;

/// +- 10ppm
static const uint32_t PPM_20_DIV = (1000000 / 20);

/// Recommended by docs
static const uint8_t DEF_DCC_F = rfm69::DCC_FREQ_4;


/// For the RxBw lookup table, by frequency
struct RxBwConv {
    uint32_t freq;
    uint8_t rxbw_val;
};


/// For the init register settings
struct RegisterSetting {
    rfm69::Reg reg;
    uint8_t val;
};

/// Used to calculate the RxBw settings
static const RxBwConv RXBW_FSK[] = {
    {2600uL,   rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_7 | rfm69::DEF_DCC_F},
    {3100uL,   rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_7 | rfm69::DEF_DCC_F},
    {3900uL,   rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_7 | rfm69::DEF_DCC_F},
    {5200uL,   rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_6 | rfm69::DEF_DCC_F},
    {6300uL,   rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_6 | rfm69::DEF_DCC_F},
    {7800uL,   rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_6 | rfm69::DEF_DCC_F},
    {10400uL,  rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_5 | rfm69::DEF_DCC_F},
    {12500uL,  rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_5 | rfm69::DEF_DCC_F},
    {15600uL,  rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_5 | rfm69::DEF_DCC_F},
    {20800uL,  rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_4 | rfm69::DEF_DCC_F},
    {25000uL,  rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_4 | rfm69::DEF_DCC_F},
    {31300uL,  rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_4 | rfm69::DEF_DCC_F},
    {41700uL,  rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_3 | rfm69::DEF_DCC_F},
    {50000uL,  rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_3 | rfm69::DEF_DCC_F},
    {62500uL,  rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_3 | rfm69::DEF_DCC_F},
    {83300uL,  rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_2 | rfm69::DEF_DCC_F},
    {100000uL, rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_2 | rfm69::DEF_DCC_F},
    {125000uL, rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_2 | rfm69::DEF_DCC_F},
    {166700uL, rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_1 | rfm69::DEF_DCC_F},
    {200000uL, rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_1 | rfm69::DEF_DCC_F},
    {250000uL, rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_1 | rfm69::DEF_DCC_F},
    {333300uL, rfm69::RX_BW_MANT_24 | rfm69::RX_BW_EXP_0 | rfm69::DEF_DCC_F},
    {400000uL, rfm69::RX_BW_MANT_20 | rfm69::RX_BW_EXP_0 | rfm69::DEF_DCC_F},
    {500000uL, rfm69::RX_BW_MANT_16 | rfm69::RX_BW_EXP_0 | rfm69::DEF_DCC_F},
};
static const size_t RXBW_FSK_SIZE = sizeof(RXBW_FSK) / sizeof(RXBW_FSK[0]);

/// Init register settings
static const RegisterSetting INIT_REG[] = {
    {rfm69::REG_OP_MODE,         rfm69::SEQUENCER_ON | rfm69::MODE_SLEEP},
    {rfm69::REG_DATA_MODUL,      rfm69::DATA_MODE_PACKET | rfm69::MOD_TYPE_FSK},
    //{rfm69::REG_BITRATE_MSB, },
    //{rfm69::REG_BITRATE_LSB, },
    //{rfm69::REG_FDEV_MSB, },
    //{rfm69::REG_FDEV_LSB, },
    //{rfm69::REG_FRF_MSB, },
    //{rfm69::REG_FRF_MID, },
    //{rfm69::REG_FRF_LSB, },
    //{rfm69::REG_OSC_1, },
    //{rfm69::REG_AFC_CTRL, },
    //{rfm69::REG_LISTEN_1, },
    //{rfm69::REG_LISTEN_2, },
    //{rfm69::REG_LISTEN_3, },
    //{rfm69::REG_VERSION, },
    {rfm69::REG_PA_LEVEL,        rfm69::PA1_ON |
                                 rfm69::OUTPUT_POWER::set(31)
    },
    //{rfm69::REG_PA_RAMP, },
    //{rfm69::REG_OCP, },
    {rfm69::REG_LNA,             rfm69::LNA_ZIN_200_OHM |
                                 rfm69::LNA_GAIN_AUTO},
    //{rfm69::REG_RX_BW, },
    //{rfm69::REG_AFC_BW, },
    //{rfm69::REG_OOK_PEAK, },
    //{rfm69::REG_OOK_AVG, },
    //{rfm69::REG_OOK_FIX, },
    {rfm69::REG_AFC_FEI,         rfm69::AFC_AUTOCLEAR_OFF |
#if defined(ENABLE_AFC)
                                 rfm69::AFC_AUTO_ON
#else
                                 rfm69::AFC_AUTO_OFF
#endif
    },
    //{rfm69::REG_AFC_MSB, },
    //{rfm69::REG_AFC_LSB, },
    //{rfm69::REG_FEI_MSB, },
    //{rfm69::REG_FEI_LSB, },
    //{rfm69::REG_RSSI_CONFIG, },
    //{rfm69::REG_RSSI_VALUE, },
    //{rfm69::REG_DIO_MAP_1, },
    {rfm69::REG_DIO_MAP_2,       rfm69::CLK_OUT_OFF},
    //{rfm69::REG_IRQ_FLAGS_1, },
    //{rfm69::REG_IRQ_FLAGS_2, },
    {rfm69::REG_RSSI_THRESH,     0xe4},
    //{rfm69::REG_RX_TIMEOUT_1, },
    //{rfm69::REG_RX_TIMEOUT_2, },
    {rfm69::REG_PREAMBLE_MSB,    0},
    {rfm69::REG_PREAMBLE_LSB,    10},
    {rfm69::REG_SYNC_CONFIG,     rfm69::SYNC_ON | rfm69::SYNC_SIZE::set(2) |
                                 rfm69::FIFO_FILL_IF_SYNC_ADDR
    },
    {rfm69::REG_SYNC_VALUE_1,    0x81},
    {rfm69::REG_SYNC_VALUE_2,    0x24},
    //{rfm69::REG_SYNC_VALUE_3, },
    //{rfm69::REG_SYNC_VALUE_4, },
    //{rfm69::REG_SYNC_VALUE_5, },
    //{rfm69::REG_SYNC_VALUE_6, },
    //{rfm69::REG_SYNC_VALUE_7, },
    //{rfm69::REG_SYNC_VALUE_8, },
    {rfm69::REG_PACKET_CONFIG_1, rfm69::PACKET_LENGTH_VARIABLE |
                                 rfm69::PACKET_CRC_ON |
                                 rfm69::PACKET_DC_FREE_WHITENING |
                                 rfm69::PACKET_ADDR_FILTER_OFF |
                                 rfm69::PACKET_CRC_AUTO_CLEAR_ON
    },
    {rfm69::REG_PAYLOAD_LENGTH,  MPDU},
    //{rfm69::REG_NODE_ADRS, },
    //{rfm69::REG_BROADCAST_ADRS, },
    //{rfm69::REG_AUTO_MODES, },
    {rfm69::REG_FIFO_THRESH,     rfm69::TX_START_COND_FIFO_NOT_EMPTY},
    {rfm69::REG_PACKET_CONFIG_2, rfm69::AUTO_RX_RESTART_OFF | rfm69::AES_OFF},
    //{rfm69::REG_AES_KEY_1, },
    //{rfm69::REG_AES_KEY_2, },
    //{rfm69::REG_AES_KEY_3, },
    //{rfm69::REG_AES_KEY_4, },
    //{rfm69::REG_AES_KEY_5, },
    //{rfm69::REG_AES_KEY_6, },
    //{rfm69::REG_AES_KEY_7, },
    //{rfm69::REG_AES_KEY_8, },
    //{rfm69::REG_AES_KEY_9, },
    //{rfm69::REG_AES_KEY_11, },
    //{rfm69::REG_AES_KEY_12, },
    //{rfm69::REG_AES_KEY_13, },
    //{rfm69::REG_AES_KEY_14, },
    //{rfm69::REG_AES_KEY_15, },
    //{rfm69::REG_AES_KEY_16, },
    //{rfm69::REG_TEMP_1, },
    //{rfm69::REG_TEMP_2, },
    //{rfm69::REG_TEST_LNA, },
    //{rfm69::REG_TEST_PA_1, },
    //{rfm69::REG_TEST_PA_2, },
    //{rfm69::REG_TEST_DAGC, },
    //{rfm69::REG_TEST_AFC, },
};

}
}

#endif
