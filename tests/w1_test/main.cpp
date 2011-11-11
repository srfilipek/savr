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

#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()


// Choose features!
// Base feature set is Reset + Search

#define FEATURESET_1            // MatchROM + ReadByte + WriteByte
#define FEATURESET_2            // Alarm + GetTemp + GetAll + PollTemp + PollAll
#define INCLUDE_DESCRIPTIONS    // May save space by removing command descriptions

#if defined(INCLUDE_DESCRIPTIONS)
#define DESC(x) x
#else
#define DESC(x) NULL
#endif


static W1 *wire = NULL;


static bool parseAddress(W1::Address &address, char *text) {
    uint8_t i=0;
    char temp[3];
    temp[2] = 0;

    while(i<8 && *text && *(text+1)) {
        temp[0] = *text++;
        temp[1] = *text++;
        address.array[i++] = strtoul(temp, NULL, 16);
    }
    if(i != 8) {
        return false;
    }
    printf_P(PSTR("Address: "));
    W1::printAddress(address);
    putchar('\n');
    return true;
}


uint8_t wrap_W1_Reset(char *args) {
    if(!wire->reset()) {
        printf_P(PSTR("No "));
    }
    printf_P(PSTR("Presence!\n"));

    return 0;
}

uint8_t wrap_W1_Search(char *args) {

    printf_P(PSTR("Devices found:\n"));

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->searchROM(address, token)) {
        W1::printAddress(address);
        putchar('\n');
    }


    return 0;
}

#if defined(FEATURESET_1)
uint8_t wrap_W1_MatchROM(char *args) {
    W1::Address address;

    if(!parseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    wire->matchROM(address);
    return 0;
}

uint8_t wrap_W1_ReadByte(char *args) {
    char *token;
    char *currentArg;

    uint8_t b = 1;
    currentArg = strtok_r(args, " ", &token);
    if(currentArg != NULL) {
        b = strtoul(currentArg, (char**) NULL, 0);
    }

    printf_P(PSTR("Reading %d bytes:"), b);
    while(b --> 0) {
        printf_P(PSTR(" 0x%02X"), wire->readByte());
    }
    putchar('\n');
    return 0;
}

uint8_t wrap_W1_WriteByte(char *args) {
    char *token;
    char *currentArg;

    uint8_t b;
    currentArg = strtok_r(args, " ", &token);


    while(currentArg != NULL) {
        b = strtoul(currentArg, (char**) NULL, 0);
        currentArg = strtok_r(NULL, " ", &token);

        printf_P(PSTR("Sending: 0x%02X\n"), b);
        wire->writeByte(b);
    }
    return 0;
}
#endif


#if defined(FEATURESET_2)
uint8_t wrap_W1_Alarm(char *args) {

    printf_P(PSTR("Devices found (Alarm):\n"));

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->alarmSearch(address, token)) {
        W1::printAddress(address);
        putchar('\n');
    }


    return 0;
}

uint8_t
wrap_GetTemp(char *args)
{
    double dtemp;

    W1::Address address;

    if(!parseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    DSTherm therm(*wire, address);

    therm.startConversion();

    dtemp = therm.getTemp(false);
    printf_P(PSTR("  C Temp: %f\n"), dtemp);

    dtemp = 1.8*dtemp + 32;
    printf_P(PSTR("  F Temp: %f\n"), dtemp);

    return 0;
}


uint8_t wrap_GetAll(char *args) {
    const size_t MAX_COUNT = 5000;
    size_t count = 0;
    double dtemp;


    if(!wire->reset()) {
        printf_P(PSTR("No presence detected.\n"));
        return 1;
    }

    // Make all devices start a conversion
    wire->writeByte(0xCC);
    wire->writeByte(0x44);


    while(wire->readByte() == 0x00) {
        if(count++ > MAX_COUNT) {
            printf_P(PSTR("Device took too long to perform measurement.\n"));
            return 1;
        }
    }

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->searchROM(address, token)) {
        W1::printAddress(address);
        DSTherm therm(*wire, address);
        dtemp = therm.getTemp(true);
        printf_P(PSTR(": %f F\n"), dtemp);
    }
    return 0;
}


uint8_t wrap_PollTemp(char *args) {
    double dtemp;

    W1::Address address;

    if(!parseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }

    DSTherm therm(*wire, address);
    while(1) {
        therm.startConversion();
        dtemp = therm.getTemp(true);
        printf_P(PSTR("Temp: %f F\n"), dtemp);
    }
}


uint8_t wrap_PollAll(char *args) {
    while(1) {
        wrap_GetAll(args);
        putchar('\n');
    }
    return 0;
}



#endif



/**
 * Terminal command callbacks
 */

// Command list
static CMD::CommandList cmdList = {
    {"reset",           wrap_W1_Reset,          DESC("Reset 1-Wire bus, look for presence.")},
    {"search",          wrap_W1_Search,         DESC("Scan bus and print any addresses found")},

#if defined(FEATURESET_1)
    {"match",           wrap_W1_MatchROM,       DESC("Select device using MatchROM (match <address>)")},
    {"read",            wrap_W1_ReadByte,       DESC("Read a byte (read [num bytes])")},
    {"write",           wrap_W1_WriteByte,      DESC("Write one+ byte to the bus (write <byte> [byte] ..)")},
#endif

#if defined(FEATURESET_2)
    {"alarm",           wrap_W1_Alarm,          DESC("Scan bus and print addresses found (alarmSearch)")},
    {"gettemp",         wrap_GetTemp,           DESC("Setup a temp conversion and read the result (gettemp <address>)")},
    {"getall",          wrap_GetAll,            DESC("Get temps from all devices once")},
    {"polltemp",        wrap_PollTemp,          DESC("Continually perform temp conversion -- never returns (polltemp <address>)")},
    {"pollall",         wrap_PollAll,           DESC("Continually get temps from all devices -- never returns")},
#endif


};
static const size_t cmdLength = sizeof(cmdList) / sizeof(CMD::CommandDef);


// Terminal display
#define welcomeMessage PSTR("\n1-Wire Test for " MYAVR "\n")
#define promptString   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::init(38400);  // bps

    W1 local_wire(GPIO::C3);
    wire = &local_wire;

    Term::init(welcomeMessage, promptString);

    Interrupts_Enable();

    Term::run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)
