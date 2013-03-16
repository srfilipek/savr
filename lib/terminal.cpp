/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

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
static PGM_P welcomeMessage;
static PGM_P promptString;

static StringHistory<Term::LINESIZE> history;


///! File-scope terminal state
struct TermState {
    uint8_t  size;
    uint8_t  maxLength;
    char     *dest;
};
static TermState state;


static char stringBuf[Term::LINESIZE];

/**
 * @par Implementation notes:
 */
static void
Backspace()
{
    putchar(BACKSPACE_CHAR);
    putchar(' ');
    putchar(BACKSPACE_CHAR);
}


/**
 * @par Implementation notes:
 */
static bool
AddChar(char c)
{
    /* Echo back */
    putchar(c);

    /* If there's room, store the char, else, backup over it */
    if (state.size < (state.maxLength - 1)) {
        state.dest[state.size] = c;
        state.size++;
        return true;
    } else {
        Backspace();
        return false;
    }
}


/**
 * @par Implementation notes:
 */
static void
ClearLine()
{
    while (state.size) {
        putchar(BACKSPACE_CHAR);
        putchar(' ');
        putchar(BACKSPACE_CHAR);
        state.size--;
    }
}


static void
SetLine(const char *line)
{
    ClearLine();

    if(line == NULL) return;

    while(*line && AddChar(*line)) {
        line++;
    }
}


/**
 * @par Implementation notes:
 */
static void
HandleEsc()
{
    char next = getchar();
    if(next != '[') {
        AddChar(next);
        return;
    }
    next = getchar();
    switch(next) {
    case 'A':
        SetLine(history.Older());
        break;
    case 'B':
        SetLine(history.Newer());
        break;
    default:
        AddChar('[');
        AddChar(next);
        return;
    }
}


/**
 * Handle the given character as a user input
 *
 * @return true if a line is completed, false otherwise
 */
bool
HandleChar(char c) {

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
        ClearLine();
        break;

    case DEL_CHAR: /* Fall through */
    case BACKSPACE_CHAR:
        if (state.size) {
            Backspace();
            state.size--;
        }
        break;

    case ESC_CHAR:
        HandleEsc();
        break;

    /* All others, check for non-special character. */
    default:
        if (c >= 0x20 && c <= 0x7E) {
            AddChar(c);
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
Term::GetLine(char * string, uint8_t maxLength)
{
    while(!HandleChar(getchar())) { /* Nothing */ }
    strncpy(string, state.dest, maxLength);
}



/**
 * @par Implementation Notes:
 */
void
Term::Init(PGM_P message, PGM_P prompt, const CMD::CommandList commandList, size_t length)
{
    welcomeMessage = message;
    promptString = prompt;

    printf_P(welcomeMessage);
    CMD::Init(commandList, length);

    state.size      = 0;
    state.dest      = stringBuf;
    state.maxLength = Term::LINESIZE;

    printf_P(promptString);
}



/**
 * @par Implementation Notes:
 */
void
Term::Run(void)
{
    while (1) {
        while(!HandleChar(getchar())) { /* Nothing */ }
        if(state.size) {
            history.Add(stringBuf);
            CMD::RunCommand(stringBuf);
            state.size = 0;
        }
        printf_P(promptString);
    }
}



/**
 * @par Implementation Notes:
 */
void
Term::Work(void)
{
    while(SCI::Size(stdin)) {
        if(HandleChar(getchar())) {
            if(state.size) {
                history.Add(stringBuf);
                CMD::RunCommand(stringBuf);
                state.size = 0;
            }
            printf_P(promptString);
        }
    }
}

