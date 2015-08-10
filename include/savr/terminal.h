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
#ifndef _savr_terminal_h_included_
#define _savr_terminal_h_included_

/**
 * @file terminal.h
 *
 * Terminal interface
 *
 * This provides a terminal-like interface to the user, relying
 * on libc and properly configured stdin and stdout.
 *
 * Supported special keys:
 *   - Backspace or Delete
 *   - Ctrl+U (clear line)
 */

#include <stdint.h>
#include <stddef.h>

#include <savr/cpp_pgmspace.h>
#include <savr/command.h>

namespace savr {
namespace term {

static const uint8_t LINESIZE = 64;    ///< Line size for work/run()


/**
 * Initializes the terminal settings and prints a welcome message
 *
 * @param message a pointer to the welcome message
 * @param prompt a pointer to the prompt string
 * @param commandList the list of supported commands
 * @param length the length of the commandList
 */
void init(PGM_P message, PGM_P prompt,
          const CMD::CommandList commandList,
          size_t length);


/**
 * Run the Terminal subsystem, never returning
 *
 * This continually gets lines from standard in and runs the
 * associated command, if any.
 */
void run(void) __attribute__ ((noreturn));


/**
 * Do some work, if needed, for the terminal interface
 *
 * This checks to see if there is any new input from standard in and runs
 * the associated command, if any and if needed.
 *
 * This can be used to provide a terminal interface while still performing
 * other tasks in the main execution loop.
 */
void work(void);


/**
 * Reads a line of text from stdin
 *
 * Has some basic features, such as CTRL+U to clear a line
 *
 * @param string the user supplied buffer to store the input line
 * @param max_length the maximum number of characters to read
 */
void read_line(char * string, uint8_t max_length);

}
}

#endif /* _savr_terminal_h_included_ */

