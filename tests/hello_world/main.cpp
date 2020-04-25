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

#define enable_interrupts() sei()

using namespace savr;


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
static cmd::CommandList cmd_list = {
        {"echo", echo, "Prints the given argument string"},
};


// Terminal display
#define welcome_message PSTR("Hello World Test\n")
#define prompt_string   PSTR("] ")


/**
 * Main
 */
int main(void) {

    sci::init(250000uL); // bps

    enable_interrupts();

    term::init(welcome_message, prompt_string,
               cmd_list, utils::array_size(cmd_list));

    term::run();

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)

