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

#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()



/**
 * Terminal command callbacks
 */

// Command list
static CMD::CommandList cmdList = {

};

static const size_t cmdLength = sizeof(cmdList) / sizeof(CMD::CommandDef);


// Terminal display
#define welcomeMessage PSTR("\nLCD Test\n")
#define promptString   PSTR("] ")


static LCD *lcd = NULL;

/**
 * For binding to a file stream
 */
static int PutChar(char input, FILE * stream) {
    lcd->outChar(input);
    return 0;
}


/**
 * Main
 */
int main(void) {

    SCI::init(38400);  // bps

    GPIO::out(GPIO::B1);
    GPIO::low(GPIO::B1);
    GPIO::out(GPIO::B2);
    GPIO::low(GPIO::B2);

    LCD locallcd(GPIO::D3, GPIO::D5, GPIO::D2, GPIO::D4, GPIO::B0, GPIO::D7, GPIO::D6);
    lcd = &locallcd;
    locallcd.setBlink(false);
    locallcd.setCursor(false);

    locallcd.outString("Hello world!");

    Term::init(welcomeMessage, promptString);

    Interrupts_Enable();

    Term::run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)
