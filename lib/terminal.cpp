/*********************************************************************************
 Copyright (C) 2015 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

#include <avr/io.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>

#include <savr/terminal.h>
#include <savr/stringhistory.h>
#include <savr/sci.h>

#define BACKSPACE_CHAR  0x08
#define DEL_CHAR        0x7F
#define CLR_CHAR        0x15
#define ESC_CHAR        0x1B

// Defined by user and linked
static PGM_P welcome_message;
static PGM_P prompt_string;

static StringHistory<Term::LINESIZE> history;


///! File-scope terminal state
struct TermState {
    uint8_t  size;
    uint8_t  max_length;
    char     *dest;
};
static TermState state;


static char string_buf[Term::LINESIZE];

/**
 * @par Implementation notes:
 */
static void
backspace()
{
    putchar(BACKSPACE_CHAR);
    putchar(' ');
    putchar(BACKSPACE_CHAR);
}


/**
 * @par Implementation notes:
 */
static bool
add_char(char c)
{
    /* Echo back */
    putchar(c);

    /* If there's room, store the char, else, backup over it */
    if (state.size < (state.max_length - 1)) {
        state.dest[state.size] = c;
        state.size++;
        return true;
    } else {
        backspace();
        return false;
    }
}


/**
 * @par Implementation notes:
 */
static void
clear_line()
{
    while (state.size) {
        putchar(BACKSPACE_CHAR);
        putchar(' ');
        putchar(BACKSPACE_CHAR);
        state.size--;
    }
}


static void
set_line(const char *line)
{
    clear_line();

    if(line == NULL) return;

    while(*line && add_char(*line)) {
        line++;
    }
}


/**
 * @par Implementation notes:
 */
static void
handle_esc()
{
    char next = getchar();
    if(next != '[') {
        add_char(next);
        return;
    }
    next = getchar();
    switch(next) {
    case 'A':
        set_line(history.older());
        break;
    case 'B':
        set_line(history.newer());
        break;
    default:
        add_char('[');
        add_char(next);
        return;
    }
}


/**
 * Handle the given character as a user input
 *
 * @return true if a line is completed, false otherwise
 */
bool
handle_char(char c) {

    switch (c) {

    /* End of line found. NULL out and return. */
    case '\r':
        state.dest[state.size] = '\0';
        putchar('\n');
        //puts(dest);
        return true;
        /* Not Reached */
        break;

    /* Clear line. Erase up to prompt. */
    case CLR_CHAR:
        clear_line();
        break;

    case DEL_CHAR: /* Fall through */
    case BACKSPACE_CHAR:
        if (state.size) {
            backspace();
            state.size--;
        }
        break;

    case ESC_CHAR:
        handle_esc();
        break;

    /* All others, check for non-special character. */
    default:
        if (c >= 0x20 && c <= 0x7E) {
            add_char(c);
        } else {
            //printf("0x%02X ", c);
        }

        break;
    }
    return false;
}



/**
 * @par Implementation Notes:
 */
void
Term::read_line(char * string, uint8_t max_length)
{
    while(!handle_char(getchar())) { /* Nothing */ }
    strncpy(string, state.dest, max_length);
}



/**
 * @par Implementation Notes:
 */
void
Term::init(PGM_P message, PGM_P prompt,
        const CMD::CommandList command_list, size_t length)
{
    welcome_message = message;
    prompt_string = prompt;

    printf_P(welcome_message);
    CMD::init(command_list, length);

    state.size      = 0;
    state.dest      = string_buf;
    state.max_length = Term::LINESIZE;

    printf_P(prompt_string);
}



/**
 * @par Implementation Notes:
 */
void
Term::run(void)
{
    while (1) {
        while(!handle_char(getchar())) { /* Nothing */ }
        if(state.size) {
            history.add(string_buf);
            CMD::run_command(string_buf);
            state.size = 0;
        }
        printf_P(prompt_string);
    }
}



/**
 * @par Implementation Notes:
 */
void
Term::work(void)
{
    while(SCI::size(stdin)) {
        if(handle_char(getchar())) {
            if(state.size) {
                history.add(string_buf);
                CMD::run_command(string_buf);
                state.size = 0;
            }
            printf_P(prompt_string);
        }
    }
}

