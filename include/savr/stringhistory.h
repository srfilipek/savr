#ifndef _stringhistory_h_Included_
#define _stringhistory_h_Included_
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

/**
 * @file stringhistory.h
 *
 * Memory-compact history tracker
 *
 * This keeps track of strings added over time. This is used
 * for an up/down arrow command history navigation in the terminal.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <savr/utils.h>


/**
 * StringHistory
 *
 * This class keeps track of a set of strings that are added over time.
 *
 * A StringHistory object has internal state that is used to retrieve historical strings. This
 * can be manipulated with the accessor methods newer(), older(), newest(), oldest(). This state is
 * reset on every call to add().
 *
 * The template parameter MAX_SIZE controls the number of bytes dedicated to tracking history.
 *
 * Note that there is an issue of fragmentation, or rather, an issue of possible efficiency given
 * that this object does not fragment its memory internally. Since the goal is to maintain
 * contiguous strings that the user can quickly access, this will not break up a string too
 * fill available space, nor will it wrap a string around.
 */
template <size_t MAX_SIZE>
class StringHistory {

public:

    /**
     * Creates a string history object
     */
    StringHistory() :
        _next(0),
        _nav(0)
    {
        memset(_history, 0, sizeof(_history));
    }


    ~StringHistory() { }


    /**
     * Add a string to the history.
     *
     * This resets the state of newer/older/newest/oldest navigation.
     *
     * @param line  Null terminated string
     */
    void add(const char* line)
    {
        size_t remaining    = MAX_SIZE - _next;     // Number of bytes in history that can be copied
        size_t to_copy      = strlen(line) + 1;     // +1 for the NULL termination

        if (to_copy > MAX_SIZE) {
            to_copy = MAX_SIZE;
        }

        // If we can't fit it at the end (with NULL), wrap to the beginning
        if (to_copy >= remaining) {
            memset(&_history[_next], 0, remaining);
            _next = 0;

        } else if (_next != 0) {
            _next++;
        }

        _nav = _next;


        // Copy all but the NULL (to allow for special handling to remove overwritten strings)
        while(to_copy --> 1) {
            _history[_next] = *line;
            line++;
            _next++;
        }


        // Make sure we clear out the rest of the string we may have been overwriting
        char* nuller = &_history[_next];
        while(*nuller != 0 && nuller <= &_history[MAX_SIZE-1]) {
            *nuller = 0;
            nuller++;
        }


        if(_next >= (MAX_SIZE-1)) {
            _next = 0;
        }
    }


    /**
     * Get a newer string
     *
     * @return A pointer to a null-terminated string, or NULL if there are no entries
     *
     * This will wrap
     */
    const char* newer(void)
    {
        if(_history[_nav] == 0) return NULL;

        // Find the end of our current string
        while(_history[_nav] != 0) {
            _nav++;
        }

        // Find the start of the next string
        while(_nav < MAX_SIZE && _history[_nav] == 0) {
            _nav++;
        }

        // There is always a string at position 0 (if we ended up at the end of our buffer)
        if(_nav >= MAX_SIZE) {
            _nav = 0;
        }

        return &_history[_nav];
    }


    /**
     * Get an older string
     *
     * @return A pointer to a null-terminated string, or NULL if there are no entries
     *
     * This will wrap
     */
    const char* older(void)
    {
        if(_history[_nav] == 0) return NULL;

        // _nav points to the older string already. Store, then go back for the next call
        char* result = &_history[_nav];

        // There is always a string starting at position 0 (if we get here). If we're at 0, wrap
        if(_nav == 0) {
            _nav = MAX_SIZE;
        }
        _nav--;

        // Find the tail of a string
        while(_history[_nav] == 0) {
            _nav--;
        }

        // Find the start of the string
        while(_nav != 0 && _history[_nav-1] != 0) {
            _nav--;
        }

        return result;
    }

#if 0
    void __print_state(const char* location)
    {
        printf("\nSTATE @ %s\n", location);
        printf("  _next     : %d\n", _next);
        printf("  _nav      : %d\n", _nav);
        printf("  _history:\n");
        Utils::printBlock((void*)&_history, MAX_SIZE);
        printf("\n");
    }
#endif



private:
    char    _history[MAX_SIZE];     // The buffer to use for history

    size_t  _next;                  // Next place to put a string
                                    // Always points to a null character

    size_t  _nav;                   // The user-state from newe(r|st) and olde(r|st)
                                    // Always points to valid text, or NULL if there is no text
                                    // Always points to the next string to be returned by older()

};



#endif
