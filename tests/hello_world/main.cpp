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

#define interrupts_disable() cli()
#define interrupts_enable() sei()



/**
 * Terminal command callbacks
 */
static uint8_t
echo(char* args)
{
    printf_P(PSTR("Echo got: '%s'\n"), args);
    return 0;
}


// Command list
static CMD::CommandList cmd_list = {
        {"echo", echo, "Prints the given argument string"},
};

static const size_t cmd_length = sizeof(cmd_list) / sizeof(CMD::CommandDef);


// Terminal display
#define welcome_message PSTR("Hello World Test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::init(38400); // bps

    interrupts_enable();

    Term::init(welcome_message, prompt_string, cmd_list, cmd_length);

    Term::run();

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)

