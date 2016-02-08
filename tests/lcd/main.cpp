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
#include <savr/lcd.h>
#include <savr/sci.h>
#include <savr/terminal.h>
#include <savr/utils.h>
#include <savr/twi.h>
#include <savr/w1.h>
#include <savr/gpio.h>

#define enable_interrupts() sei()

using namespace savr;

static LCD *lcd = NULL;

/**
 * For binding to a file stream
 */
uint8_t
write_char(char *input) {
    lcd->clear();
    lcd->write_string(input);
    return 0;
}


/**
 * Terminal command callbacks
 */

// Command list
static cmd::CommandList cmd_list = {
    {"write",    write_char,    "Write to the LCD"}
};

static const size_t cmd_length = sizeof(cmd_list) / sizeof(cmd::CommandDef);


// Terminal display
#define welcome_message PSTR("\nLCD Test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int main(void) {

    sci::init(38400);  // bps

    gpio::out(gpio::B1);
    gpio::low(gpio::B1);
    gpio::out(gpio::B2);
    gpio::low(gpio::B2);

    LCD local_lcd(gpio::D3, gpio::D5, gpio::D2, gpio::D4, gpio::B0, gpio::D7, gpio::D6);
    lcd = &local_lcd;
    local_lcd.set_blink(false);
    local_lcd.set_cursor(false);

    local_lcd.write_string("Hello world!");

    enable_interrupts();

    term::init(welcome_message, prompt_string, cmd_list, cmd_length);

    term::run();

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)

