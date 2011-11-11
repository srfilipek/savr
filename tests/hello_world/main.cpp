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

#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()



static uint8_t
echo(char* args)
{
    printf_P(PSTR("Echo got: '%s'\n"), args);
    return 0;
}


/**
 * Terminal command callbacks
 */

// Command list
static CMD::CommandList cmdList = {
        {"echo", echo, "Prints the given argument string"},
};

static const size_t cmdLength = sizeof(cmdList) / sizeof(CMD::CommandDef);


// Terminal display
#define welcomeMessage PSTR("Hello World Test\n")
#define promptString   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::init(38400); // bps

    Term::init(welcomeMessage, promptString);

    Interrupts_Enable();

    Term::run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)
