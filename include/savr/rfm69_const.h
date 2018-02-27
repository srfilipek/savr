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
#ifndef _savr_rfm69_const_h_included_
#define _savr_rfm69_const_h_included_

/**
 * @file rfm69_const.h
 */

#include <stdint.h>
#include <stddef.h>
#include <avr/sfr_defs.h>

#include <savr/utils.h>

namespace savr {
namespace rfm69 {

using savr::utils::Bitfield;

/**
 * Constants regarding the external RFM69 module
 */

/// Oscillator frequency in Hz
const uint32_t F_XOSC = 32000000uLL;

/// Frequency granularity: F_XOSC / 2^19
const uint32_t F_STEP = F_XOSC / (static_cast<uint32_t>(1) << 19);

/// Maximum frequency deviation setting (15bits x F_STEP)
const uint32_t F_DEV_MAX = F_STEP * 0x3FFF;


/**
 * General register addresses
 */
enum Reg : uint8_t {
    REG_FIFO            = 0x00,
    REG_OP_MODE         = 0x01,
    REG_DATA_MODUL      = 0X02,
    REG_BITRATE_MSB     = 0x03,
    REG_BITRATE_LSB     = 0x04,
    REG_FDEV_MSB        = 0x05,
    REG_FDEV_LSB        = 0x06,
    REG_FRF_MSB         = 0x07,
    REG_FRF_MID         = 0x08,
    REG_FRF_LSB         = 0x09,
    REG_OSC_1           = 0x0a,
    REG_AFC_CTRL        = 0x0b,
    REG_LISTEN_1        = 0x0d,
    REG_LISTEN_2        = 0x0e,
    REG_LISTEN_3        = 0x0f,

    REG_VERSION         = 0x10,
    REG_PA_LEVEL        = 0x11,
    REG_PA_RAMP         = 0x12,
    REG_OCP             = 0x13,
    REG_LNA             = 0x18,
    REG_RX_BW           = 0x19,
    REG_AFC_BW          = 0x1a,
    REG_OOK_PEAK        = 0x1b,
    REG_OOK_AVG         = 0x1c,
    REG_OOK_FIX         = 0x1d,
    REG_AFC_FEI         = 0x1e,
    REG_AFC_MSB         = 0x1f,

    REG_AFC_LSB         = 0x20,
    REG_FEI_MSB         = 0x21,
    REG_FEI_LSB         = 0x22,
    REG_RSSI_CONFIG     = 0x23,
    REG_RSSI_VALUE      = 0x24,
    REG_DIO_MAP_1       = 0x25,
    REG_DIO_MAP_2       = 0x26,
    REG_IRQ_FLAGS_1     = 0x27,
    REG_IRQ_FLAGS_2     = 0x28,
    REG_RSSI_THRESH     = 0x29,
    REG_RX_TIMEOUT_1    = 0x2a,
    REG_RX_TIMEOUT_2    = 0x2b,
    REG_PREAMBLE_MSB    = 0x2c,
    REG_PREAMBLE_LSB    = 0x2d,
    REG_SYNC_CONFIG     = 0x2e,
    REG_SYNC_VALUE_1    = 0x2f,

    REG_SYNC_VALUE_2    = 0x30,
    REG_SYNC_VALUE_3    = 0x31,
    REG_SYNC_VALUE_4    = 0x32,
    REG_SYNC_VALUE_5    = 0x33,
    REG_SYNC_VALUE_6    = 0x34,
    REG_SYNC_VALUE_7    = 0x35,
    REG_SYNC_VALUE_8    = 0x36,
    REG_PACKET_CONFIG_1 = 0x37,
    REG_PAYLOAD_LENGTH  = 0x38,
    REG_NODE_ADRS       = 0x39,
    REG_BROADCAST_ADRS  = 0x3a,
    REG_AUTO_MODES      = 0x3b,
    REG_FIFO_THRESH     = 0x3c,
    REG_PACKET_CONFIG_2 = 0x3d,
    REG_AES_KEY_1       = 0x3e,
    REG_AES_KEY_2       = 0x3f,

    REG_AES_KEY_3       = 0x40,
    REG_AES_KEY_4       = 0x41,
    REG_AES_KEY_5       = 0x42,
    REG_AES_KEY_6       = 0x43,
    REG_AES_KEY_7       = 0x44,
    REG_AES_KEY_8       = 0x45,
    REG_AES_KEY_9       = 0x46,
    REG_AES_KEY_11      = 0x47,
    REG_AES_KEY_12      = 0x48,
    REG_AES_KEY_13      = 0x4a,
    REG_AES_KEY_14      = 0x4b,
    REG_AES_KEY_15      = 0x4c,
    REG_AES_KEY_16      = 0x4d,
    REG_TEMP_1          = 0x4e,
    REG_TEMP_2          = 0x4f,

    REG_TEST_LNA        = 0x58,
    REG_TEST_PA_1       = 0x5a,
    REG_TEST_PA_2       = 0x5c,

    REG_TEST_DAGC       = 0x6f,

    REG_TEST_AFC        = 0x71,
};

const uint8_t REG_WRITE = _BV(7);

/**
 * For REG_OP_MODE
 */
class SEQUENCER : public Bitfield<7, 1>{};
const uint8_t SEQUENCER_ON = SEQUENCER::set(0);
const uint8_t SEQUENCER_OFF = SEQUENCER::set(1);
class LISTEN : public Bitfield<6, 1>{};
const uint8_t LISTEN_ON = LISTEN::set(1);
const uint8_t LISTEN_OFF = LISTEN::set(0);
const uint8_t LISTEN_ABORT = _BV(5);
class MODE : public Bitfield<2, 3>{};
const uint8_t MODE_SLEEP = MODE::set(0x00);
const uint8_t MODE_STDBY = MODE::set(0x01);
const uint8_t MODE_FS = MODE::set(0x02);
const uint8_t MODE_TX = MODE::set(0x03);
const uint8_t MODE_RX = MODE::set(0x04);


/**
 * For REG_DATA_MODUL
 */
class DATA_MODE : public Bitfield<5, 2>{};
const uint8_t DATA_MODE_PACKET = DATA_MODE::set(0x00);
const uint8_t DATA_MODE_CONT_SYNC = DATA_MODE::set(0x02);
const uint8_t DATA_MODE_CONT_NO_SYNC = DATA_MODE::set(0x03);

class MOD_TYPE : public Bitfield<3, 2>{};
const uint8_t MOD_TYPE_FSK = MOD_TYPE::set(0x00);
const uint8_t MOD_TYPE_OOK = MOD_TYPE::set(0x01);

class MOD_SHAPE : public Bitfield<0, 2>{};
const uint8_t MOD_SHAPE_FSK_NONE = MOD_SHAPE::set(0x00);
const uint8_t MOD_SHAPE_FSK_GAUS_BT_10 = MOD_SHAPE::set(0x01);
const uint8_t MOD_SHAPE_FSK_GAUS_BT_05 = MOD_SHAPE::set(0x02);
const uint8_t MOD_SHAPE_FSK_GAUS_BT_03 = MOD_SHAPE::set(0x03);
const uint8_t MOD_SHAPE_OOK_NONE = MOD_SHAPE::set(0x00);
const uint8_t MOD_SHAPE_OOK_CUTOFF_BR = MOD_SHAPE::set(0x01);
const uint8_t MOD_SHAPE_OOK_CUTOFF_2BR = MOD_SHAPE::set(0x02);

/**
 * For REG_OSC_1
 */
const uint8_t RC_CAL_START = _BV(7);
const uint8_t RC_CAL_DONE = _BV(6);

/**
 * For REG_AFC_CTRL
 */
const uint8_t AFC_LOW_BETA_ON = _BV(5);

/**
 * For REG_LISTEN_1
 */
class LISTEN_RESOL_IDLE : public Bitfield<6, 2>{};
const uint8_t LISTEN_RESOL_IDLE_64_US = LISTEN_RESOL_IDLE::set(0x01);
const uint8_t LISTEN_RESOL_IDLE_4100_US = LISTEN_RESOL_IDLE::set(0x02);
const uint8_t LISTEN_RESOL_IDLE_262000_US = LISTEN_RESOL_IDLE::set(0x03);
class LISTEN_RESOL_RX : public Bitfield<4, 2>{};
const uint8_t LISTEN_RESOL_RX_64_US = LISTEN_RESOL_RX::set(0x01);
const uint8_t LISTEN_RESOL_RX_4100_US = LISTEN_RESOL_RX::set(0x02);
const uint8_t LISTEN_RESOL_RX_262000_US = LISTEN_RESOL_RX::set(0x03);
class LISTEN_CRITERIA : public Bitfield<3, 1>{};
const uint8_t LISTEN_CRITERIA_RSSI = LISTEN_CRITERIA::set(0);
const uint8_t LISTEN_CRITERIA_RSSI_ADDR = LISTEN_CRITERIA::set(1);
class LISTEN_END : public Bitfield<0, 1>{};
const uint8_t LISTEN_END_TO_RX = LISTEN_END::set(0);
const uint8_t LISTEN_END_TO_MODE = LISTEN_END::set(0x01);
const uint8_t LISTEN_END_TO_IDLE = LISTEN_END::set(0x02);

/**
 * For REG_PA_LEVEL
 */
class PA0_FIELD : public Bitfield<7, 1>{};
class PA1_FIELD : public Bitfield<6, 1>{};
class PA2_FIELD : public Bitfield<5, 1>{};
const uint8_t PA0_ON   = _BV(7);
const uint8_t PA0_OFF  = 0;
const uint8_t PA1_ON   = _BV(6);
const uint8_t PA1_OFF  = 0;
const uint8_t PA2_ON   = _BV(5);
const uint8_t PA2_OFF  = 0;
class OUTPUT_POWER : public Bitfield<0, 5>{};

/**
 * For REG_PA_RAMP
 */
class PA_RAMP : public Bitfield<0, 4>{};
const uint8_t PA_RAMP_3400_US = PA_RAMP::set(0x00);
const uint8_t PA_RAMP_2000_US = PA_RAMP::set(0x01);
const uint8_t PA_RAMP_1000_US = PA_RAMP::set(0x02);
const uint8_t PA_RAMP_500_US = PA_RAMP::set(0x03);
const uint8_t PA_RAMP_250_US = PA_RAMP::set(0x04);
const uint8_t PA_RAMP_125_US = PA_RAMP::set(0x05);
const uint8_t PA_RAMP_100_US = PA_RAMP::set(0x06);
const uint8_t PA_RAMP_62_US = PA_RAMP::set(0x07);
const uint8_t PA_RAMP_50_US = PA_RAMP::set(0x08);
const uint8_t PA_RAMP_40_US = PA_RAMP::set(0x09);
const uint8_t PA_RAMP_31_US = PA_RAMP::set(0x0a);
const uint8_t PA_RAMP_25_US = PA_RAMP::set(0x0b);
const uint8_t PA_RAMP_20_US = PA_RAMP::set(0x0c);
const uint8_t PA_RAMP_15_US = PA_RAMP::set(0x0d);
const uint8_t PA_RAMP_12_US = PA_RAMP::set(0x0e);
const uint8_t PA_RAMP_10_US = PA_RAMP::set(0x0f);

/**
 * For REG_OCP_ON
 */
class OCP : public Bitfield<4, 1>{};
const uint8_t OCP_ON = OCP::set(1);
const uint8_t OCP_OFF = OCP::set(0);
class OCP_TRIM : public Bitfield<0, 4>{};

/**
 * For REG_LNA
 */
class LNA_ZIN : public Bitfield<7, 1>{};
const uint8_t LNA_ZIN_50_OHM = LNA_ZIN::set(0);
const uint8_t LNA_ZIN_200_OHM = LNA_ZIN::set(1);
class LNA_CURRENT_GAIN : public Bitfield<3, 3>{};
class LNA_GAIN_SELECT : public Bitfield<0, 3>{};
const uint8_t LNA_GAIN_AUTO = LNA_GAIN_SELECT::set(0x00);
const uint8_t LNA_GAIN_HIGH = LNA_GAIN_SELECT::set(0x01);
const uint8_t LNA_GAIN_MINUS_6 = LNA_GAIN_SELECT::set(0x02);
const uint8_t LNA_GAIN_MINUS_12 = LNA_GAIN_SELECT::set(0x03);
const uint8_t LNA_GAIN_MINUS_24 = LNA_GAIN_SELECT::set(0x04);
const uint8_t LNA_GAIN_MINUS_36 = LNA_GAIN_SELECT::set(0x05);
const uint8_t LNA_GAIN_MINUS_48 = LNA_GAIN_SELECT::set(0x06);

/**
 * For REG_RX_BW
 */
class DCC_FREQ : public Bitfield<5, 3>{};
const uint8_t DCC_FREQ_16 = DCC_FREQ::set(0x00);
const uint8_t DCC_FREQ_8 = DCC_FREQ::set(0x01);
const uint8_t DCC_FREQ_4 = DCC_FREQ::set(0x02);
const uint8_t DCC_FREQ_2 = DCC_FREQ::set(0x03);
const uint8_t DCC_FREQ_1 = DCC_FREQ::set(0x04);
const uint8_t DCC_FREQ_0p5 = DCC_FREQ::set(0x05);
const uint8_t DCC_FREQ_0p25 = DCC_FREQ::set(0x06);
const uint8_t DCC_FREQ_0p125 = DCC_FREQ::set(0x07);
class RX_BW_MANT : public Bitfield<3, 2>{};
const uint8_t RX_BW_MANT_16 = RX_BW_MANT::set(0x00);
const uint8_t RX_BW_MANT_20 = RX_BW_MANT::set(0x01);
const uint8_t RX_BW_MANT_24 = RX_BW_MANT::set(0x02);
class RX_BW_EXP : public Bitfield<0, 3>{};
const uint8_t RX_BW_EXP_0 = RX_BW_EXP::set(0x00);
const uint8_t RX_BW_EXP_1 = RX_BW_EXP::set(0x01);
const uint8_t RX_BW_EXP_2 = RX_BW_EXP::set(0x02);
const uint8_t RX_BW_EXP_3 = RX_BW_EXP::set(0x03);
const uint8_t RX_BW_EXP_4 = RX_BW_EXP::set(0x04);
const uint8_t RX_BW_EXP_5 = RX_BW_EXP::set(0x05);
const uint8_t RX_BW_EXP_6 = RX_BW_EXP::set(0x06);
const uint8_t RX_BW_EXP_7 = RX_BW_EXP::set(0x07);



/**
 * For REG_AFC_BW
 */
class DCC_FREQ_AFC : public Bitfield<5, 3>{};
class RX_BW_MANT_AFC : public Bitfield<3, 2>{};
class RX_BW_EXP_AFC : public Bitfield<0, 3>{};

/**
 * For REG_AFC_FEI
 */
const uint8_t FEI_DONE = _BV(6);
const uint8_t FEI_START = _BV(5);
const uint8_t AFC_DONE = _BV(4);
class AFC_AUTOCLEAR : public Bitfield<3, 1>{};
const uint8_t AFC_AUTOCLEAR_ON = AFC_AUTOCLEAR::set(1);
const uint8_t AFC_AUTOCLEAR_OFF = AFC_AUTOCLEAR::set(0);
class AFC_AUTO :  public Bitfield<2, 1>{};
const uint8_t AFC_AUTO_ON = AFC_AUTO::set(1);
const uint8_t AFC_AUTO_OFF = AFC_AUTO::set(0);
const uint8_t AFC_CLEAR = _BV(1);
const uint8_t AFC_START = _BV(0);

/**
 * For REG_RSSI_CONFIG
 */
const uint8_t RSSI_DONE = _BV(1);
const uint8_t RSSI_START = _BV(0);

/**
 * For REG_DIO_MAP_1
 */
class DIO0_MAPPING : public Bitfield<6, 2>{};
const uint8_t DIO0_CONT_ANY_MODE_READY = DIO0_MAPPING::set(0b11);
const uint8_t DIO0_CONT_FS_PLL_LOCK = DIO0_MAPPING::set(0b00);
const uint8_t DIO0_CONT_RX_SYNC_ADDRESS = DIO0_MAPPING::set(0b00);
const uint8_t DIO0_CONT_RX_TIMEOUT = DIO0_MAPPING::set(0b01);
const uint8_t DIO0_CONT_RX_RSSI = DIO0_MAPPING::set(0b10);
const uint8_t DIO0_CONT_TX_PLL_LOCK = DIO0_MAPPING::set(0b00);
const uint8_t DIO0_CONT_TX_TX_READY = DIO0_MAPPING::set(0b01);

const uint8_t DIO0_PKT_FS_PLL_LOCK = DIO0_MAPPING::set(0b11);
const uint8_t DIO0_PKT_RX_CRC_OK = DIO0_MAPPING::set(0b00);
const uint8_t DIO0_PKT_RX_PAYLOAD_READY = DIO0_MAPPING::set(0b01);
const uint8_t DIO0_PKT_RX_SYNC_ADDR = DIO0_MAPPING::set(0b10);
const uint8_t DIO0_PKT_RX_RSSI = DIO0_MAPPING::set(0b11);
const uint8_t DIO0_PKT_TX_PACKET_SENT = DIO0_MAPPING::set(0b00);
const uint8_t DIO0_PKT_TX_TX_READY = DIO0_MAPPING::set(0b01);
const uint8_t DIO0_PKT_TX_PLL_LOCK = DIO0_MAPPING::set(0b11);

class DIO1_MAPPING : public Bitfield<4, 2>{};
class DIO2_MAPPING : public Bitfield<2, 2>{};
class DIO3_MAPPING : public Bitfield<0, 2>{};

/**
 * For REG_DIO_MAP_2
 */
class DIO4_MAPPING : public Bitfield<6, 2>{};
class DIO5_MAPPING : public Bitfield<4, 2>{};
class CLK_OUT : public Bitfield<0, 3>{};
const uint8_t CLK_OUT_F_XOSC = CLK_OUT::set(0x00);
const uint8_t CLK_OUT_F_XOSC_2 = CLK_OUT::set(0x01);
const uint8_t CLK_OUT_F_XOSC_4 = CLK_OUT::set(0x02);
const uint8_t CLK_OUT_F_XOSC_8 = CLK_OUT::set(0x03);
const uint8_t CLK_OUT_F_XOSC_16 = CLK_OUT::set(0x04);
const uint8_t CLK_OUT_F_XOSC_32 = CLK_OUT::set(0x05);
const uint8_t CLK_OUT_RC = CLK_OUT::set(0x06);
const uint8_t CLK_OUT_OFF = CLK_OUT::set(0x07);

/**
 * For REG_IRQ_FLAGS_1
 */
const uint8_t IRQ_1_MODE_READY = _BV(7);
const uint8_t IRQ_1_RX_READY = _BV(6);
const uint8_t IRQ_1_TX_READY = _BV(5);
const uint8_t IRQ_1_PLL_LOCK = _BV(4);
const uint8_t IRQ_1_RSSI = _BV(3);
const uint8_t IRQ_1_TIMEOUT = _BV(2);
const uint8_t IRQ_1_AUTO_MODE = _BV(1);
const uint8_t IRQ_1_SYNC_ADDR_MATCH = _BV(0);

/**
 * For REG_IRQ_FLAGS_2
 */
const uint8_t IRQ_2_FIFO_FULL = _BV(7);
const uint8_t IRQ_2_FIFO_NOT_EMPTY = _BV(6);
const uint8_t IRQ_2_FIFO_LEVEL = _BV(5);
const uint8_t IRQ_2_FIFO_OVERRUN = _BV(4);
const uint8_t IRQ_2_PACKET_SENT = _BV(3);
const uint8_t IRQ_2_PAYLOAD_READY = _BV(2);
const uint8_t IRQ_2_CRC_OK = _BV(1);

/**
 * For REG_SYNC_CONFIG
 */
class SYNC : public Bitfield<7, 1>{};
const uint8_t SYNC_ON = SYNC::set(1);
const uint8_t SYNC_OFF = SYNC::set(0);
class FIFO_FILL : public Bitfield<6, 1>{};
const uint8_t FIFO_FILL_IF_SYNC_ADDR = FIFO_FILL::set(0);
const uint8_t FIFO_FILL_IF_FILL_COND = FIFO_FILL::set(1);
class SYNC_SIZE : public Bitfield<3, 3>{};
class SYNC_ERROR_TOL : public Bitfield<0, 3>{};

/**
 * For REG_PACKET_CONFIG_1
 */
class PACKET_LENGTH : public Bitfield<7, 1>{};
const uint8_t PACKET_LENGTH_FIXED = PACKET_LENGTH::set(0);
const uint8_t PACKET_LENGTH_VARIABLE = PACKET_LENGTH::set(1);
class PACKET_DC_FREE : public Bitfield<5, 2>{};
const uint8_t PACKET_DC_FREE_NONE = PACKET_DC_FREE::set(0x00);
const uint8_t PACKET_DC_FREE_MANCHESTER = PACKET_DC_FREE::set(0x01);
const uint8_t PACKET_DC_FREE_WHITENING = PACKET_DC_FREE::set(0x02);
class PACKET_CRC : public Bitfield<4, 1>{};
const uint8_t PACKET_CRC_ON = PACKET_CRC::set(1);
const uint8_t PACKET_CRC_OFF = PACKET_CRC::set(0);
class PACKET_CRC_AUTO_CLEAR : public Bitfield<3, 1>{};
const uint8_t PACKET_CRC_AUTO_CLEAR_ON = PACKET_CRC_AUTO_CLEAR::set(0);
const uint8_t PACKET_CRC_AUTO_CLEAR_OFF = PACKET_CRC_AUTO_CLEAR::set(1);
class PACKET_ADDR_FILTER : public Bitfield<1, 2>{};
const uint8_t PACKET_ADDR_FILTER_OFF = PACKET_ADDR_FILTER::set(0x00);
const uint8_t PACKET_ADDR_FILTER_NODE = PACKET_ADDR_FILTER::set(0x01);
const uint8_t PACKET_ADDR_FILTER_NODE_BROADCAST = PACKET_ADDR_FILTER::set(0x02);

/**
 * For REG_AUTO_MODES
 */
class ENTER_COND : public Bitfield<5, 3>{};
const uint8_t ENTER_COND_NONE = ENTER_COND::set(0x00);
const uint8_t ENTER_COND_FIFO_NOT_EMPTY = ENTER_COND::set(0x01);
const uint8_t ENTER_COND_FIFO_LEVEL = ENTER_COND::set(0x02);
const uint8_t ENTER_COND_CRC_OK = ENTER_COND::set(0x03);
const uint8_t ENTER_COND_PAYLOAD_READY = ENTER_COND::set(0x04);
const uint8_t ENTER_COND_SYNC_ADDRESS = ENTER_COND::set(0x05);
const uint8_t ENTER_COND_PACKET_SENT = ENTER_COND::set(0x06);
const uint8_t ENTER_COND_FIFO_EMPTY = ENTER_COND::set(0x07);

class EXIT_COND : public Bitfield<2, 3>{};
const uint8_t EXIT_COND_NONE = EXIT_COND::set(0x00);
const uint8_t EXIT_COND_FIFO_NOT_EMPTY = EXIT_COND::set(0x01);
const uint8_t EXIT_COND_FIFO_LEVEL = EXIT_COND::set(0x02);
const uint8_t EXIT_COND_CRC_OK = EXIT_COND::set(0x03);
const uint8_t EXIT_COND_PAYLOAD_READY = EXIT_COND::set(0x04);
const uint8_t EXIT_COND_SYNC_ADDRESS = EXIT_COND::set(0x05);
const uint8_t EXIT_COND_PACKET_SENT = EXIT_COND::set(0x06);
const uint8_t EXIT_COND_FIFO_EMPTY = EXIT_COND::set(0x07);

class INTER_MODE : public Bitfield<0, 2>{};
const uint8_t INTER_MODE_SLEEP = 0x00;
const uint8_t INTER_MODE_STDBY = 0x01;
const uint8_t INTER_MODE_RX = 0x02;
const uint8_t INTER_MODE_TX = 0x03;

/**
 * For REG_FIFO_THRESH
 */
const uint8_t TX_START_COND_FIFO_LEVEL = 0;
const uint8_t TX_START_COND_FIFO_NOT_EMPTY = _BV(7);
class FIFO_THRESH : public Bitfield<0, 7>{};

/**
 * For REG_PACKET_CONFIG_2
 */
class INTER_PACKET_RX_DELAY : public Bitfield<4, 4>{};
const uint8_t RESTART_RX = _BV(2);
class AUTO_RESTART_RX : public Bitfield<1, 1>{};
const uint8_t AUTO_RX_RESTART_ON = _BV(1);
const uint8_t AUTO_RX_RESTART_OFF = 0;
class AES : public Bitfield<0, 1>{};
const uint8_t AES_ON = _BV(0);
const uint8_t AES_OFF = 0;

}
}


#endif

