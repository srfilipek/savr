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
#include <savr/clock.h>
#include <savr/gpio.h>

#define enable_interrupts() sei()

using namespace savr;


/**
 * Terminal command callbacks
 */
static uint8_t
tick(char* args)
{
    printf_P(PSTR("%lu\n"), clock::ticks());
    return 0;
}

/**
 * Terminal command callbacks
 */
static uint8_t
tick_low(char* args)
{
    printf_P(PSTR("%u\n"), clock::ticks_low());
    return 0;
}

/**
 * Terminal command callbacks
 */
static uint8_t
tock(char* args)
{
    while(true) {
        printf_P(PSTR("%lu\n"), clock::ticks());
    }
    return 0;
}

/**
 * Terminal command callbacks
 */
static uint8_t
tock_low(char* args)
{
    while(true) {
        printf_P(PSTR("%u\n"), clock::ticks_low());
    }
    return 0;
}

// Command list
static cmd::CommandList cmd_list = {
    {"tick", tick, "Prints the number of ticks elapsed"},
    {"tock", tock, "Continually prints the number of ticks elapsed"},
    {"tick-low", tick_low, "Prints the lowest byte of the number of ticks elapsed"},
    {"tock-low", tock_low, "Continually prints the lowest byte of number of ticks elapsed"},
};

static const size_t cmd_length = sizeof(cmd_list) / sizeof(cmd::CommandDef);


// Terminal display
#define welcome_message PSTR("Clock tick test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int main(void) {

    sci::init(250000uL);  // bps

    enable_interrupts();

    term::init(welcome_message, prompt_string, cmd_list, cmd_length);

    // LED output for the pulse
    gpio::out<gpio::D7>();
    gpio::low<gpio::D7>();

    clock::init();

    uint32_t last_tick = 0;

    while(true) {
        uint32_t now = clock::ticks();
        if((now - last_tick) > clock::TICKS_PER_SEC) {
            last_tick = now;
            gpio::toggle<gpio::D7>();
        }

        term::work();
    }

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)

