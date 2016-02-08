/*************************************************************//**
 * @file main.c
 *
 * @author Stefan Filipek
 ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <savr/cpp_pgmspace.h>
#include <savr/sci.h>
#include <savr/terminal.h>
#include <savr/utils.h>
#include <savr/twi.h>
#include <savr/w1.h>
#include <savr/dstherm.h>

#define enable_interrupts() sei()

using namespace savr;


// Choose features!
// Base feature set is Reset + Search

#define FEATURESET_1            // match_rom + read_byte + write_byte
#define FEATURESET_2            // Alarm + get_temp + GetAll + PollTemp + PollAll
#define INCLUDE_DESCRIPTIONS    // May save space by removing command descriptions

#if defined(INCLUDE_DESCRIPTIONS)
#define DESC(x) x
#else
#define DESC(x) NULL
#endif


static W1 *wire = NULL;


static bool parse_address(W1::Address &address, char *text) {
    uint8_t i=8;
    char temp[3];
    temp[2] = 0;

    while(i && *text && *(text+1)) {
        temp[0] = *text++;
        temp[1] = *text++;
        address.array[--i] = strtoul(temp, NULL, 16);
    }
    if(i != 0) {
        return false;
    }
    printf_P(PSTR("Address: "));
    W1::print_address(address);
    putchar('\n');
    return true;
}


uint8_t wrap_w1_reset(char *args) {
    if(!wire->reset()) {
        printf_P(PSTR("No "));
    }
    printf_P(PSTR("Presence!\n"));

    return 0;
}

uint8_t wrap_w1_search(char *args) {

    printf_P(PSTR("Devices found:\n"));

    W1::Address address;
    W1::Token   token = W1::EMPTY_TOKEN;
    while(wire->search_rom(address, token)) {
        W1::print_address(address);
        putchar('\n');
    }


    return 0;
}

#if defined(FEATURESET_1)
uint8_t wrap_w1_match_rom(char *args) {
    W1::Address address;

    if(!parse_address(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    wire->match_rom(address);
    return 0;
}

uint8_t wrap_w1_read_byte(char *args) {
    char *token;
    char *current_arg;

    uint8_t b = 1;
    current_arg = strtok_r(args, " ", &token);
    if(current_arg != NULL) {
        b = strtoul(current_arg, (char**) NULL, 0);
    }

    printf_P(PSTR("Reading %d bytes:"), b);
    while(b --> 0) {
        printf_P(PSTR(" 0x%02X"), wire->read_byte());
    }
    putchar('\n');
    return 0;
}

uint8_t wrap_w1_write_byte(char *args) {
    char *token;
    char *current_arg;

    uint8_t b;
    current_arg = strtok_r(args, " ", &token);


    while(current_arg != NULL) {
        b = strtoul(current_arg, (char**) NULL, 0);
        current_arg = strtok_r(NULL, " ", &token);

        printf_P(PSTR("Sending: 0x%02X\n"), b);
        wire->write_byte(b);
    }
    return 0;
}
#endif


#if defined(FEATURESET_2)
uint8_t wrap_w1_alarm(char *args) {

    printf_P(PSTR("Devices found (Alarm):\n"));

    W1::Address address;
    W1::Token   token = W1::EMPTY_TOKEN;
    while(wire->alarm_search(address, token)) {
        W1::print_address(address);
        putchar('\n');
    }


    return 0;
}

uint8_t
wrap_get_temp(char *args)
{
    double dtemp;

    W1::Address address;

    if(!parse_address(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    DSTherm therm(*wire, address);

    therm.start_conversion();

    dtemp = therm.get_temp(false);
    printf_P(PSTR("  C Temp: %f\n"), dtemp);

    dtemp = 1.8*dtemp + 32;
    printf_P(PSTR("  F Temp: %f\n"), dtemp);

    return 0;
}


uint8_t wrap_get_all(char *args) {
    const size_t MAX_COUNT = 5000;
    size_t count = 0;
    double dtemp;


    if(!wire->reset()) {
        printf_P(PSTR("No presence detected.\n"));
        return 1;
    }

    // Make all devices start a conversion
    wire->write_byte(0xCC);
    wire->write_byte(0x44);


    while(wire->read_byte() == 0x00) {
        if(count++ > MAX_COUNT) {
            printf_P(PSTR("Device took too long to perform measurement.\n"));
            return 1;
        }
    }

    W1::Address address;
    W1::Token   token = W1::EMPTY_TOKEN;
    while(wire->search_rom(address, token)) {
        W1::print_address(address);
        DSTherm therm(*wire, address);
        dtemp = therm.get_temp(true);
        printf_P(PSTR(": %f F\n"), dtemp);
    }
    return 0;
}


uint8_t wrap_poll_temp(char *args) {
    double dtemp;

    W1::Address address;

    if(!parse_address(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }

    DSTherm therm(*wire, address);
    while(1) {
        therm.start_conversion();
        dtemp = therm.get_temp(true);
        printf_P(PSTR("Temp: %f F\n"), dtemp);
    }
}


uint8_t wrap_poll_all(char *args) {
    while(1) {
        wrap_get_all(args);
        putchar('\n');
    }
    return 0;
}



#endif



/**
 * Terminal command callbacks
 */

// Command list
static cmd::CommandList cmd_list = {
    {"reset",           wrap_w1_reset,          DESC("Reset 1-Wire bus, look for presence.")},
    {"search",          wrap_w1_search,         DESC("Scan bus and print any addresses found")},

#if defined(FEATURESET_1)
    {"match",           wrap_w1_match_rom,      DESC("Select device using match_rom (Select <address>)")},
    {"read",            wrap_w1_read_byte,      DESC("Read a byte (Read [num bytes])")},
    {"write",           wrap_w1_write_byte,     DESC("Write one+ byte to the bus (Wrte <byte> [byte] ..)")},
#endif

#if defined(FEATURESET_2)
    {"alarm",           wrap_w1_alarm,          DESC("Scan bus and print addresses found (AlarmSearch)")},
    {"gettemp",         wrap_get_temp,          DESC("Setup a temp conversion and read the result (get_temp <address>)")},
    {"getall",          wrap_get_all,           DESC("Get temps from all devices once")},
    {"polltemp",        wrap_poll_temp,         DESC("Continually perform temp conversion -- never returns (PollTemp <address>)")},
    {"pollall",         wrap_poll_all,          DESC("Continually get temps from all devices -- never returns")},
#endif


};
static const size_t cmd_length = sizeof(cmd_list) / sizeof(cmd::CommandDef);


// Terminal display
#define welcome_message PSTR("\n1-Wire Test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int main(void) {

    sci::init(38400);  // bps

    W1 local_wire(gpio::D6);
    wire = &local_wire;

    enable_interrupts();

    term::init(welcome_message, prompt_string, cmd_list, cmd_length);

    term::run();

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)

