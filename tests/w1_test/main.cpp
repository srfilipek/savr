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


static bool ParseAddress(W1::Address &address, char *text) {
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
    W1::PrintAddress(address);
    putchar('\n');
    return true;
}


uint8_t wrap_W1_Reset(char *args) {
    if(!wire->Reset()) {
        printf_P(PSTR("No "));
    }
    printf_P(PSTR("Presence!\n"));

    return 0;
}

uint8_t wrap_W1_Search(char *args) {

    printf_P(PSTR("Devices found:\n"));

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->SearchROM(address, token)) {
        W1::PrintAddress(address);
        putchar('\n');
    }


    return 0;
}

#if defined(FEATURESET_1)
uint8_t wrap_W1_MatchROM(char *args) {
    W1::Address address;

    if(!ParseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    wire->MatchROM(address);
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
        printf_P(PSTR(" 0x%02X"), wire->ReadByte());
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
        wire->WriteByte(b);
    }
    return 0;
}
#endif


#if defined(FEATURESET_2)
uint8_t wrap_W1_Alarm(char *args) {

    printf_P(PSTR("Devices found (Alarm):\n"));

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->AlarmSearch(address, token)) {
        W1::PrintAddress(address);
        putchar('\n');
    }


    return 0;
}

uint8_t
wrap_GetTemp(char *args)
{
    double dtemp;

    W1::Address address;

    if(!ParseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }
    DSTherm therm(*wire, address);

    therm.StartConversion();

    dtemp = therm.GetTemp(false);
    printf_P(PSTR("  C Temp: %f\n"), dtemp);

    dtemp = 1.8*dtemp + 32;
    printf_P(PSTR("  F Temp: %f\n"), dtemp);

    return 0;
}


uint8_t wrap_GetAll(char *args) {
    const size_t MAX_COUNT = 5000;
    size_t count = 0;
    double dtemp;


    if(!wire->Reset()) {
        printf_P(PSTR("No presence detected.\n"));
        return 1;
    }

    // Make all devices start a conversion
    wire->WriteByte(0xCC);
    wire->WriteByte(0x44);


    while(wire->ReadByte() == 0x00) {
        if(count++ > MAX_COUNT) {
            printf_P(PSTR("Device took too long to perform measurement.\n"));
            return 1;
        }
    }

    W1::Address address;
    W1::Token   token = W1_EMPTY_TOKEN;
    while(wire->SearchROM(address, token)) {
        W1::PrintAddress(address);
        DSTherm therm(*wire, address);
        dtemp = therm.GetTemp(true);
        printf_P(PSTR(": %f F\n"), dtemp);
    }
    return 0;
}


uint8_t wrap_PollTemp(char *args) {
    double dtemp;

    W1::Address address;

    if(!ParseAddress(address, args)) {
        printf_P(PSTR("Invalid address\n"));
        return 1;
    }

    DSTherm therm(*wire, address);
    while(1) {
        therm.StartConversion();
        dtemp = therm.GetTemp(true);
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
    {"Reset",           wrap_W1_Reset,          DESC("Reset 1-Wire bus, look for presence.")},
    {"Search",          wrap_W1_Search,         DESC("Scan bus and print any addresses found")},

#if defined(FEATURESET_1)
    {"Match",           wrap_W1_MatchROM,       DESC("Select device using MatchROM (Select <address>)")},
    {"Read",            wrap_W1_ReadByte,       DESC("Read a byte (Read [num bytes])")},
    {"Write",           wrap_W1_WriteByte,      DESC("Write one+ byte to the bus (Wrte <byte> [byte] ..)")},
#endif

#if defined(FEATURESET_2)
    {"Alarm",           wrap_W1_Alarm,          DESC("Scan bus and print addresses found (AlarmSearch)")},
    {"GetTemp",         wrap_GetTemp,           DESC("Setup a temp conversion and read the result (GetTemp <address>)")},
    {"GetAll",          wrap_GetAll,            DESC("Get temps from all devices once")},
    {"PollTemp",        wrap_PollTemp,          DESC("Continually perform temp conversion -- never returns (PollTemp <address>)")},
    {"PollAll",         wrap_PollAll,           DESC("Continually get temps from all devices -- never returns")},
#endif


};
static const size_t cmdLength = sizeof(cmdList) / sizeof(CMD::CommandDef);


// Terminal display
#define welcomeMessage PSTR("\n1-Wire Test\n")
#define promptString   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::Init(38400);  // bps

    W1 localWire(GPIO::D7);
    wire = &localWire;

    Term::Init(welcomeMessage, promptString);

    Interrupts_Enable();

    Term::Run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)
