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

#define interrupts_disable() cli()
#define interrupts_enable() sei()


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
static CMD::CommandList cmd_list = {
    {"write",    write_char,    "Write to the LCD"}
};

static const size_t cmd_length = sizeof(cmd_list) / sizeof(CMD::CommandDef);


// Terminal display
#define welcome_message PSTR("\nLCD Test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::init(38400);  // bps

    GPIO::out(GPIO::B1);
    GPIO::low(GPIO::B1);
    GPIO::out(GPIO::B2);
    GPIO::low(GPIO::B2);

    LCD local_lcd(GPIO::D3, GPIO::D5, GPIO::D2, GPIO::D4, GPIO::B0, GPIO::D7, GPIO::D6);
    lcd = &local_lcd;
    local_lcd.set_blink(false);
    local_lcd.set_cursor(false);

    local_lcd.write_string("Hello world!");

    interrupts_enable();

    Term::init(welcome_message, prompt_string, cmd_list, cmd_length);

    Term::run();

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)

