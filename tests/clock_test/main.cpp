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
#include <savr/gpio.h>

#define interrupts_disable() cli()
#define interrupts_enable() sei()

uint8_t scan_twi(char *args) {
    uint8_t i=0;
    uint8_t res=0;
    uint8_t state=0;

    printf_P(PSTR("Devices found:\n"));
    for(i=0; i<0x7F; ++i) {
        // Try to read from the address...
        res = TWI::address(i, 1);
        if(res == 0) {
            printf_P(PSTR("  0x%02X\n"), i);
        }

        // If we get an ACK, read out a byte or else errors can occur
        // Else, we can do a repeated Start and look at the next address...
        state = TWI::state();
        if(state == TW_MR_SLA_ACK) {
            TWI::get();
            TWI::stop();
        }
    }
    TWI::stop();
    return 0;
}

uint8_t __to_bcd_nib(char value) {
    if(value < '0' || value > '9') return 0;
    return value - '0';
}


uint8_t to_bcd(const char* value) {
    uint8_t ret = 0;

    ret |= __to_bcd_nib(value[0]);
    ret <<= 4;
    ret |= __to_bcd_nib(value[1]);

    return ret;
}

uint8_t set_time(char *args) {
    uint8_t res;
    char *token;
    char *current_arg;
    uint8_t addr;

    current_arg = strtok_r(args, " ", &token);
    addr = strtoul(current_arg, (char**) NULL, 0);

    // Write mode first to set the pointer to 0
    res = TWI::address(addr, 0);
    if(res != 0) {
        printf_P(PSTR("Failed to address 0x%02X\n"), addr);
        TWI::print_state();
        return 1;
    }
    TWI::send(0);

    current_arg = strtok_r(NULL, " ", &token);
    uint8_t year    = to_bcd(current_arg+0);
    uint8_t month   = to_bcd(current_arg+2);
    uint8_t day     = to_bcd(current_arg+4);
    uint8_t hour    = to_bcd(current_arg+6);
    uint8_t minute  = to_bcd(current_arg+8);
    uint8_t second  = to_bcd(current_arg+10);

    TWI::send(second);
    TWI::send(minute);
    TWI::send(hour);
    TWI::send(0);
    TWI::send(day);
    TWI::send(month);
    TWI::send(year);

    TWI::stop();

    return 0;
}

uint8_t get_time(char *args) {
    uint8_t res;
    uint8_t i;
    uint8_t temp;
    char *token;
    char *current_arg;
    uint8_t addr;

    uint8_t buff[8];

    current_arg = strtok_r(args, " ", &token);
    addr = strtoul(current_arg, (char**) NULL, 0);

    // Write mode first to set the pointer to 0
    res = TWI::address(addr, 0);
    if(res != 0) {
        printf_P(PSTR("Failed to address 0x%02X\n"), addr);
        TWI::print_state();
        return 1;
    }
    TWI::send(0);
    TWI::stop();


    res = TWI::address(addr, 1);
    if(res != 0) {
        printf_P(PSTR("Failed to address 0x%02X\n"), addr);
        TWI::print_state();
        return 1;
    }

    for(i=0; i<sizeof(buff)-1; ++i) {
        buff[i] = TWI::get_ack();
    }
    buff[7] = TWI::get(); // Last read has no ACK

    printf_P(PSTR("Raw: "));
    Utils::print_hex(buff, 8);
    putchar('\n');

    printf_P(PSTR(" Year    :   xx%02x\n"), buff[6]);
    printf_P(PSTR(" Month   :   %02x\n"),   buff[5]);
    printf_P(PSTR(" Day (M) :   %02x\n"),   buff[4]);
    printf_P(PSTR(" Day (W) :   %x\n"),     buff[3]);
    if((buff[2] & 0x40) == 0) {
        // Military time
        printf_P(PSTR(" Hours   :   %02x (Mil)\n"), buff[2]);
    }else{
        buff[2] &= ~0x40;
        temp = (buff[2] & 0x20) == 0x20; // 1 for PM
        printf_P(PSTR(" Hours   :   %02x (%cM)\n"), buff[2], (temp ? 'P' : 'A'));
    }
    printf_P(PSTR(" Minutes :   %02x\n"),   buff[1]);
    buff[0] &= 0x7F; // Mask out CH bit
    printf_P(PSTR(" Seconds :   %02x\n"),   buff[0]);

    TWI::stop();
    return 0;
}

uint8_t wrap_twi_print_state(char *args) {
    TWI::print_state();
    return 0;
}

uint8_t wrap_twi_address(char *args) {
    uint8_t res = 0;
    char *token;
    char *current_arg;
    uint8_t b, rw;

    current_arg = strtok_r(args, " ", &token);
    b = strtoul(current_arg, (char**) NULL, 0);

    current_arg = strtok_r(NULL, " ", &token);
    rw = strtoul(current_arg, (char**) NULL, 0);
    printf_P(PSTR("Addressing 0x%02X, %S\n"), b, (rw?PSTR("Read"):PSTR("Write")));

    res = TWI::address(b, rw);
    printf_P(PSTR("Res: 0x%02X\n"), res);
    TWI::print_state();
    return 0;
}

uint8_t wrap_twi_get_ack(char *args) {
    printf_P(PSTR("Res: 0x%02X\n"), TWI::get_ack());
    TWI::print_state();
    return 0;
}

uint8_t wrap_twi_get(char *args) {
    printf_P(PSTR("Res: 0x%02X\n"), TWI::get());
    TWI::print_state();
    return 0;
}

uint8_t wrap_twi_send(char *args) {
    char *token;
    char *current_arg;

    uint8_t b;
    current_arg = strtok_r(args, " ", &token);
    b = strtoul(current_arg, (char**) NULL, 0);

    printf_P(PSTR("Sending: 0x%02X\n"), b);
    TWI::send(b);
    TWI::print_state();
    return 0;
}

uint8_t wrap_twi_stop(char *args) {
    TWI::stop();
    TWI::print_state();
    return 0;
}

uint8_t wrap_twi_start(char *args) {
    TWI::start();
    TWI::wait();
    TWI::print_state();
    return 0;
}

uint8_t wrap_twi_state(char *args) {
    printf_P(PSTR("Res: 0x%02X\n"), TWI::state());
    TWI::print_state();
    return 0;
}

uint8_t wrap_pins(char *args) {
    //printf_P(PSTR("Port %c: 0x%02X\n"), 'A', PORTA);
    printf_P(PSTR("PIN %c: 0x%02X\n"), 'B', PINB);
    printf_P(PSTR("PIN %c: 0x%02X\n"), 'C', PINC);
    printf_P(PSTR("PIN %c: 0x%02X\n"), 'D', PIND);
    return 0;
}


/**
 * Terminal command callbacks
 */

// Command list
static CMD::CommandList cmd_list = {
    {"gettime",         get_time,               "Gets the time: gettime [addr]"},
    {"settime",         set_time,               "Sets the time: settime [addr] [YYMMDDHHMMSS]"},
    {"scan",            scan_twi,               "Scans the bus and prints any addresses found"},
    {"printstate",      wrap_twi_print_state,   "Prints current bus state"},
    {"addr",            wrap_twi_address,       "Starts bus and address a device: addr [addr] [1=read, 0=write]"},
    {"getack",          wrap_twi_get_ack,       "Read a byte with ack"},
    {"get",             wrap_twi_get,           "Read a byte without ack"},
    {"send",            wrap_twi_send,          "Send a byte: send [byte]"},
    {"start",           wrap_twi_start,         "Bus Start (rarely needed)"},
    {"stop",            wrap_twi_stop,          "Bus Stop"},
    {"state",           wrap_twi_state,         "Get bus status byte"},
    {"pins",            wrap_pins,              "Print state of all GPIO pins"},


};
static const size_t cmd_length = sizeof(cmd_list) / sizeof(CMD::CommandDef);


// Terminal display
#define welcome_message PSTR("\nTWI Clock Test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::init(38400);  // bps

    // Enable internal pullups for the TWI bus
    TWI::init(100000, true); // Bus freq in Hz

    interrupts_enable();

    Term::init(welcome_message, prompt_string, cmd_list, cmd_length);

    Term::run();

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)

