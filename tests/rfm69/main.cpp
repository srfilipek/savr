/*************************************************************//**
 * @file main.c
 *
 * @author Stefan Filipek
 ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <savr/cpp_pgmspace.h>
#include <savr/sci.h>
#include <savr/terminal.h>
#include <savr/utils.h>
#include <savr/spi.h>
#include <savr/w1.h>
#include <savr/dstherm.h>
#include <savr/rfm69.h>

#define enable_interrupts() sei()

using namespace savr;


static W1 *wire = nullptr;

uint8_t int_to_byte(char* str) {
    return static_cast<uint8_t>(strtoul(str, nullptr, 0));
}

uint8_t hex_to_byte(char* str) {
    return static_cast<uint8_t>(strtoul(str, nullptr, 0));
}


static bool
parse_address(W1::Address &address, char *text) {
    uint8_t i = 8;
    char temp[3];
    temp[2] = 0;

    while (i && *text && *(text + 1)) {
        temp[0] = *text++;
        temp[1] = *text++;
        address.array[--i] = hex_to_byte(temp);
    }
    if (i != 0) {
        return false;
    }
    printf_P(PSTR("Address: "));
    W1::print_address(address);
    putchar('\n');
    return true;
}


uint8_t
wrap_w1_reset(char *args) {
    if (!wire->reset()) {
        printf_P(PSTR("No "));
    }
    printf_P(PSTR("Presence!\n"));

    return 0;
}

uint8_t
wrap_w1_search(char *args) {
    printf_P(PSTR("Devices found:\n"));

    W1::Address address;
    W1::Token token = W1::EMPTY_TOKEN;
    while (wire->search_rom(address, token)) {
        W1::print_address(address);
        putchar('\n');
    }

    return 0;
}

uint8_t
wrap_get_temp(char *args) {
    double dtemp;

    W1::Address address;

    if (!parse_address(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    DSTherm therm(*wire, address);

    therm.start_conversion();

    dtemp = therm.get_temp(false);
    printf_P(PSTR("  C Temp: %f\n"), dtemp);

    dtemp = 1.8 * dtemp + 32;
    printf_P(PSTR("  F Temp: %f\n"), dtemp);

    return 0;
}

uint8_t
wrap_set_mode(char *args) {
    char *token;
    char *current_arg;

    current_arg = strtok_r(args, " ", &token);
    uint8_t val = int_to_byte(current_arg);
    val = rfm69::MODE::set(val);

    rfm69::set_mode(val, true);
    printf_P(PSTR("Set mode register to 0x%02x\n"), val);
    return 0;
}

uint8_t
wrap_read_reg(char *args) {
    char *token;
    char *current_arg;

    current_arg = strtok_r(args, " ", &token);

    uint8_t reg = int_to_byte(current_arg);
    uint8_t val = rfm69::read_reg(static_cast<rfm69::Reg>(reg));
    printf_P(PSTR("Read 0x%02x from 0x%02x\n"), val, reg);
    return 0;
}

uint8_t
wrap_write_reg(char *args) {
    char *token;
    char *current_arg;

    current_arg = strtok_r(args, " ", &token);
    uint8_t reg = int_to_byte(current_arg);

    current_arg = strtok_r(nullptr, " ", &token);
    uint8_t val = hex_to_byte(current_arg);

    rfm69::write_reg(static_cast<rfm69::Reg>(reg), val);
    printf_P(PSTR("Wrote 0x%02x to 0x%02x\n"), val, reg);
    return 0;
}

uint8_t
wrap_rx_test(char *args) {
    uint8_t buff[rfm69::MTU];
    uint32_t received_count = 0;
    while(true) {
        uint8_t length = rfm69::rx_pdu(buff, sizeof(buff));
        received_count++;

        // Last 4 characters is the hex packet number
        unsigned long packet_number = strtoul(
            reinterpret_cast<char*>(buff + length - 4), nullptr, 16) + 1;

        savr::utils::print_block(buff, length);

        // Print out packet loss stats
        printf_P(PSTR("RSSI: %d\n"), rfm69::get_last_rssi());
        printf_P(PSTR("Gain: %d\n"), rfm69::get_last_gain());
        printf_P(PSTR("Received %lu / %lu (%.2f%%)\n"),
                 received_count, packet_number,
                 100.0 * received_count / packet_number);
    }
    return 0;
}

uint8_t
wrap_rx_str(char *args) {
    uint8_t buff[255];
    uint8_t length = rfm69::rx_pdu(buff, sizeof(buff));
    printf_P(PSTR("RSSI: %d\n"), rfm69::get_last_rssi());
    printf_P(PSTR("Gain: %d\n"), rfm69::get_last_gain());
    savr::utils::print_block(buff, length);
    return 0;
}

uint8_t
wrap_xmit_rssi(char *args) {

    rfm69::write_reg(rfm69::REG_DATA_MODUL,
                     rfm69::DATA_MODE_CONT_SYNC |
                     rfm69::MOD_TYPE_FSK);

    // This causes the preamble to be sent
    rfm69::set_mode(rfm69::MODE_TX);
    return 0;
}

uint8_t
wrap_poll_rssi(char *args) {

    rfm69::write_reg(rfm69::REG_DATA_MODUL,
                     rfm69::DATA_MODE_CONT_SYNC |
                     rfm69::MOD_TYPE_FSK);

    // Remove auto-gain control for better accuracy
    uint8_t reg = rfm69::read_reg(rfm69::REG_LNA);
    rfm69::LNA_GAIN_SELECT::raw_update(reg, rfm69::LNA_GAIN_HIGH);
    rfm69::write_reg(rfm69::REG_LNA, reg);

    // RSSI only available in rx mode
    rfm69::set_mode(rfm69::MODE_RX);

    while(true) {
        short rssi = rfm69::sample_rssi();
        printf_P(PSTR("RSSI: %d\n"), rssi);
        _delay_ms(100);
    }
    return 0;
}

uint8_t
wrap_tx_test(char *args) {
    char buff[255];
    size_t packet_num = 0;
    while(true) {
        size_t length = snprintf(
            buff, sizeof(buff), "Test packet %04x\n", packet_num++);
        printf(buff);
        rfm69::tx_pdu(buff, length-1); // Don't send the newline

        // Sleep a bit to let the other end process and whatnot
        _delay_ms(500);
    }
    return 0;
}

uint8_t
wrap_tx_str(char *args) {
    size_t length = strlen(args);
    rfm69::tx_pdu(reinterpret_cast<uint8_t *>(args), length);
    return 0;
}

uint8_t
wrap_sniff(char *args) {
    char *token;
    char *current_arg;

    current_arg = strtok_r(args, " ", &token);
    uint8_t rssi_thresh = int_to_byte(current_arg);
    if(rssi_thresh == 0) {
        rssi_thresh = 100;
    }

    rfm69::write_reg(
        rfm69::REG_DATA_MODUL,
        rfm69::DATA_MODE_CONT_NO_SYNC | rfm69::MOD_TYPE_FSK
    );

    rfm69::write_reg(
        rfm69::REG_SYNC_CONFIG,
        rfm69::SYNC_OFF | rfm69::FIFO_FILL_IF_FILL_COND);

    rfm69::write_reg(
        rfm69::REG_RSSI_THRESH,
        rssi_thresh*2
    );

    rfm69::set_mode(rfm69::MODE_RX, true);

    while(true) {
        rfm69::poll_all_bits(rfm69::REG_IRQ_FLAGS_2, rfm69::IRQ_2_FIFO_NOT_EMPTY);

        uint8_t data = rfm69::read_reg(rfm69::REG_FIFO);
        char c = isprint(data) ? static_cast<char>(data) : '.';
        printf_P(PSTR("0x%02x | %c | %d\n"), data, c, data);
    }
    return 0;
}


uint8_t
wrap_set_power(char *args) {
    char *token;
    char *current_arg;

    current_arg = strtok_r(args, " ", &token);
    auto power = static_cast<uint8_t>(strtoul(current_arg, nullptr, 0));

    printf_P(PSTR("Setting power to %u\n"), power);
    rfm69::set_transmit_power(power);

    return 0;
}

uint8_t
wrap_set_params(char *args) {
    char *token;
    char *current_arg;

    current_arg = strtok_r(args, " ", &token);
    auto bitrate = static_cast<uint32_t>(strtoul(current_arg, nullptr, 0));

    current_arg = strtok_r(nullptr, " ", &token);
    auto center_freq = static_cast<uint32_t>(strtoul(current_arg, nullptr, 0));
    if(center_freq == 0) {
        center_freq = 915000000uLL;
    }

    current_arg = strtok_r(nullptr, " ", &token);
    auto freq_dev = static_cast<uint32_t>(strtoul(current_arg, nullptr, 0));

    printf_P(PSTR("Bitrate, center, freq dev: %lu, %lu, %lu\n"),
             bitrate, center_freq, freq_dev);

    rfm69::set_fsk_params(bitrate, center_freq, freq_dev);
    return 0;
}

/**
 * Terminal command callbacks
 */

// Command list
static cmd::CommandList cmd_list = {
    {"reset",       wrap_w1_reset,      "Reset 1-Wire bus, look for presence."},
    {"search",      wrap_w1_search,     "Scan bus and print any addresses found"},
    {"gettemp",     wrap_get_temp,      "Setup a temp conversion and read the result (get_temp <address>)"},
    {"read-reg",    wrap_read_reg,      "Read radio register"},
    {"write-reg",   wrap_write_reg,     "Write radio register"},
    {"rx-test",     wrap_rx_test,       "Receive test (loop)"},
    {"rx-str",      wrap_rx_str,        "Receive a single string"},
    {"xmit-rssi",   wrap_xmit_rssi,     "Send a tone for RSSI"},
    {"poll-rssi",   wrap_poll_rssi,     "Measure RSSI (loop)"},
    {"tx-test",     wrap_tx_test,       "Transmit test (loop)"},
    {"tx-str",      wrap_tx_str,        "Transmit a single string"},
    {"sniff",       wrap_sniff,         "Sniff data directly from the air"},
    {"set-mode",    wrap_set_mode,      "Set the mode"},
    {"set-power",   wrap_set_power,     "Set transmit power"},
    {"set-params",  wrap_set_params,    "Set the bitrate, center freq (opt.), and freq dev (opt.)"},
};
static const size_t cmd_length = sizeof(cmd_list) / sizeof(cmd::CommandDef);


// Terminal display
#define welcome_message PSTR("\nRadio Test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int
main() {
    enable_interrupts();

    sci::init(250000uL);  // bps
    spi::init(1000000uLL);

    W1 local_wire(gpio::D6);
    wire = &local_wire;

    rfm69::init(50000, 903141593uL);

    term::init(welcome_message, prompt_string, cmd_list, cmd_length);

    gpio::out(gpio::D7);
    gpio::high(gpio::D7);

    term::run();

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)
