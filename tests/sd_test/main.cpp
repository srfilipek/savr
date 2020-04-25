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

#define enable_interrupts() sei()

// Terminal display
#define welcome_message PSTR("\n\nSD Test for the " SAVR_TARGET_STR ", SAVR " SAVR_VERSION_STR "\n")
#define prompt_string   PSTR("] ")

using namespace savr;

static uint8_t get(char*);
static uint8_t read(char*);
static uint8_t write(char*);
static uint8_t erase(char*);
static uint8_t scan(char*);
static uint8_t sdinit(char*);
static uint8_t help(char*);

// Command list
static cmd::CommandList cmd_list = {
    {"get", get, NULL},
    {"read", read, NULL},
    {"write", write, NULL},
    {"erase", erase, NULL},
    {"scan", scan, NULL},
    {"sdinit", sdinit, NULL},
};

static const gpio::Pin SD_SS = gpio::B0;


/**
 * Main
 */
int
main(void) {
    // Setup UART
    sci::init(250000uL);  // bps

    // Setup the SPI interface
    spi::init(F_CPU/2);

    // Enable interrupts for all services
    enable_interrupts();

    // Init UART terminal
    term::init(welcome_message, prompt_string,
               cmd_list, utils::array_size(cmd_list));

    // Run the terminal
    term::run();

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
    uint8_t read_byte;
    uint8_t read_length;

    read_length = (uint8_t) strtoul(args, (char**) NULL, 0);

    printf("Getting %u bytes\n", read_length);

    gpio::low<SD_SS>();
    while (read_length) {
        read_byte = spi::trx_byte(0xFF);
        printf("%02hX ", read_byte);
        read_length--;
    }
    gpio::high<SD_SS>();

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
    char * current_arg;
    uint32_t i;
    bool continuation = false;
    bool first = true;

    static const uint8_t BLOCKSIZE = 16;

    uint8_t curr[BLOCKSIZE];
    uint8_t prev[BLOCKSIZE];

    uint32_t addr = 0;
    uint32_t size = 0;

    // Convert arguments to longs
    current_arg = strtok_r(args, " ", &token);
    addr = strtoul(current_arg, (char**) NULL, 0);

    current_arg = strtok_r(NULL, " ", &token);
    size = strtoul(current_arg, (char**) NULL, 0);

    // Read from SD in 32-byte block sizes and print
    for (i = 0; i < size; i += BLOCKSIZE, addr += BLOCKSIZE) {

        if (!sd::read_block(addr, curr, BLOCKSIZE)) {
            printf("Error reading addr 0x%08lX\n", addr);
            break; // On error
        }

        // Only print the block if it differs from last line
        if(first || (memcmp(curr, prev, BLOCKSIZE) != 0)) {
            first = false;
            utils::print_block(curr, BLOCKSIZE, addr, 16);
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
    char * current_arg;
    uint32_t i;

    uint8_t buff[32];

    uint32_t addr = 0;
    uint16_t size = 0;

    // Convert args to longs
    current_arg = strtok_r(args, " ", &token);
    addr = strtoul(current_arg, (char**) NULL, 0);

    current_arg = strtok_r(NULL, " ", &token);
    size = (uint16_t) strtoul(current_arg, (char**) NULL, 0);

    printf("addr: %08lX, size: %08X\n", addr, size);

    for (i = 0; i < size; i += 32) {
        if (!sd::read_block(addr, buff, 32))
            break;

        utils::print_block(buff, 32, addr, 16);
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
    char * arg_end = strlen(args) + args;

    // Special tokenization
    message = args;
    while (*message != '\0' && *message != ' ')
        message++;
    *message = '\0';
    message++;

    if (message > arg_end)
        message = arg_end;

    addr = strtoul(args, (char**) NULL, 0);
    size = strlen(message);

    printf("addr: %08lX, size: %08lX\n", addr, size);

    // Write
    return sd::write_block(addr, (uint8_t*)message, size);
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
    char * current_arg;

    uint32_t addr = 0;
    uint32_t size = 0;

    char buff[16];

    // Convert args to longs
    current_arg = strtok_r(args, " ", &token);
    addr = strtoul(current_arg, (char**) NULL, 0);

    current_arg = strtok_r(NULL, " ", &token);
    size = strtoul(current_arg, (char**) NULL, 0);

    printf("Erasing addr: %08lX, size: %08lX\n", addr, size);
    printf("Type \"yes\" to confirm ");
    term::read_line(buff, 16);

    if (strcmp(buff, "yes") != 0) {
        printf("Canceled.\n");
    }

    sd::erase_block(addr, size);

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
    sd::init(SD_SS);
    return 1;
}


EMPTY_INTERRUPT(__vector_default)

