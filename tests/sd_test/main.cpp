/*************************************************************//**
* \file main.c
*
* SD Card Tester
*
* This application provides a command-line-like interface
* to run simple commands against an SD card attached to
* the SPI line.
*
* Currently, this software has only been tested on the ATmega16.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <savr/version.h>
#include <savr/cpp_pgmspace.h>
#include <savr/sci.h>
#include <savr/spi.h>
#include <savr/sd.h>
#include <savr/terminal.h>
#include <savr/utils.h>
#include <savr/gpio.h>


#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()


// Terminal display
#define welcomeMessage PSTR("\n\nSD Test for the " SAVR_TARGET_STR ", SAVR " SAVR_VERSION_STR "\n")
#define promptString   PSTR("] ")

static uint8_t get(char*);
static uint8_t read(char*);
static uint8_t write(char*);
static uint8_t erase(char*);
static uint8_t scan(char*);
static uint8_t sdinit(char*);
static uint8_t help(char*);

// Command list
static CMD::CommandList cmdList = {
    {"get", get, NULL},
    {"read", read, NULL},
    {"write", write, NULL},
    {"erase", erase, NULL},
    {"scan", scan, NULL},
    {"sdinit", sdinit, NULL},
    {"help", help, NULL}
};
static const size_t cmdLength = sizeof(cmdList) / sizeof(CMD::CommandDef);


static const GPIO::Pin SD_SS = GPIO::B0;


/**
 * Main
 */
int
main(void) {
    // Setup UART
    SCI::Init(38400);  // bps

    // Setup the SPI interface
    SPI::Init(F_CPU/2);

    // Init UART terminal
    Term::Init(welcomeMessage, promptString);

    // Enable interrupts for all services
    Interrupts_Enable();

    // Run the terminal
    Term::Run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}


/**
 * Read data off the SPI line.
 *
 * @param args a space separated string containing the number
 * of bytes to read off, limited to 256.
 */
uint8_t get(char * args)
{
    uint8_t readByte;
    uint8_t readLength;

    readLength = (uint8_t) strtoul(args, (char**) NULL, 0);

    printf("Getting %u bytes\n", readLength);

    GPIO::Low<SD_SS>();
    while (readLength) {
        readByte = SPI::TrxByte(0xFF);
        printf("%02hX ", readByte);
        readLength--;
    }
    GPIO::High<SD_SS>();

    printf("\n");

    return 1;
}


/**
 * Scans through the SD card looking for non-blank sections.
 *
 * Scans across the SD card in 32-byte blocks and prints out
 * ones containing at least one non FF value. Formatted using
 * print_block() in utils.c.
 *
 * Contiguous blocks are printed as such. Any breaks are
 * signaled with an extra newline between blocks.
 *
 * @see print_block
 *
 * @param args a space separated string containing
 * an address followed by a length.
 *
 * @return 1, always
 */
uint8_t scan(char * args)
{
    char * token;
    char * currentArg;
    uint32_t i;
    bool continuation = false;
    bool first = true;

    static const uint8_t BLOCKSIZE = 16;

    uint8_t curr[BLOCKSIZE];
    uint8_t prev[BLOCKSIZE];

    uint32_t addr = 0;
    uint32_t size = 0;

    // Convert arguments to longs
    currentArg = strtok_r(args, " ", &token);
    addr = strtoul(currentArg, (char**) NULL, 0);

    currentArg = strtok_r(NULL, " ", &token);
    size = strtoul(currentArg, (char**) NULL, 0);

    // Read from SD in 32-byte block sizes and print
    for (i = 0; i < size; i += BLOCKSIZE, addr += BLOCKSIZE) {

        if (!SD::ReadBlock(addr, curr, BLOCKSIZE)) {
            printf("Error reading addr 0x%08lX\n", addr);
            break; // On error
        }

        // Only print the block if it differs from last line
        if(first || (memcmp(curr, prev, BLOCKSIZE) != 0)) {
            first = false;
            Utils::PrintBlock(curr, BLOCKSIZE, addr, 16);
            continuation = false;
            memcpy(prev, curr, BLOCKSIZE);

        }else if(!continuation){
            continuation = true;
            printf("...\n");
        }

    }

    return 1;
}


/**
 * Read a block of data from the SD card.
 *
 * Reads a block of data and prints it using the print_block
 * function in utils.c
 *
 * @see print_block
 *
 * @param args a space seperated string containing
 * an address (32bit) followed by a length (16bit).
 *
 * @return 1, always
 */
uint8_t read(char * args)
{
    char * token;
    char * currentArg;
    uint32_t i;

    uint8_t buff[32];

    uint32_t addr = 0;
    uint16_t size = 0;

    // Convert args to longs
    currentArg = strtok_r(args, " ", &token);
    addr = strtoul(currentArg, (char**) NULL, 0);

    currentArg = strtok_r(NULL, " ", &token);
    size = (uint16_t) strtoul(currentArg, (char**) NULL, 0);

    printf("addr: %08lX, size: %08X\n", addr, size);

    for (i = 0; i < size; i += 32) {
        if (!SD::ReadBlock(addr, buff, 32))
            break;

        Utils::PrintBlock(buff, 32, addr, 16);
        addr += 32;
    }

    return 1;
}


/**
 * Write some text to the SD card.
 *
 * Writes a block of text to the address specified. Right now
 * the address must be block aligned, or an error will occur.
 *
 * @param args a space seperated string containing
 * an address (32bit) followed by the string to write.
 *
 * @return 1 if successful, 0 otherwise
 */
uint8_t write(char * args)
{
    char * message;

    uint32_t addr = 0;
    uint32_t size = 0;
    char * argEnd = strlen(args) + args;

    // Special tokenization
    message = args;
    while (*message != '\0' && *message != ' ')
        message++;
    *message = '\0';
    message++;

    if (message > argEnd)
        message = argEnd;

    addr = strtoul(args, (char**) NULL, 0);
    size = strlen(message);

    printf("addr: %08lX, size: %08lX\n", addr, size);

    // Write
    return SD::WriteBlock(addr, (uint8_t*)message, size);
}



/**
 * Erase a block of data on the SD card.
 *
 * @param args a space seperated string containing
 * the start address followed by the length (both 32bit).
 *
 * @return 1 if successful, 0 otherwise
 */
uint8_t erase(char * args)
{
    char * token;
    char * currentArg;

    uint32_t addr = 0;
    uint32_t size = 0;

    char buff[16];

    // Convert args to longs
    currentArg = strtok_r(args, " ", &token);
    addr = strtoul(currentArg, (char**) NULL, 0);

    currentArg = strtok_r(NULL, " ", &token);
    size = strtoul(currentArg, (char**) NULL, 0);

    printf("Erasing addr: %08lX, size: %08lX\n", addr, size);
    printf("Type \"yes\" to confirm ");
    Term::GetLine(buff, 16);

    if (strcmp(buff, "yes") != 0) {
        printf("Canceled.\n");
    }

    SD::EraseBlock(addr, size);

    return 1;
}


/**
 * Initialize the SD card.
 *
 * @param args is not used
 * @return 1, always
 */
uint8_t sdinit(char * args)
{
    printf("Initializing SD Card...\n");
    SD::Init(SD_SS);
    return 1;
}


/**
 * Prints out a list of supported commands.
 *
 * @param args (not used)
 *
 * @return 1, always
 */
uint8_t help(char * args)
{
    uint16_t i;

    // List out all available commands
    printf("Available commands:\n");
    for (i = 0; i < cmdLength; i++) {
        printf("  %s\n", cmdList[i].commandName);
    }

    return 1;
}


EMPTY_INTERRUPT(__vector_default)
