/*********************************************************************************
 Copyright (C) 2015 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/
#ifndef _savr_queue_h_included_
#define _savr_queue_h_included_

/**
 * @file queue.h
 *
 * Small (<256 items) circular queue functionality of any type.
 */

#include <stdint.h>
#include <stddef.h>

#include <util/atomic.h>

namespace savr {

template <typename T, uint8_t MAX_SIZE>
class Queue {

private:
    T       _data[MAX_SIZE];        ///< Queue data
    uint8_t _top;                   ///< Index to top
    uint8_t _bottom;                ///< Index to bottom
    uint8_t _size;                  ///< Current size of the queue

public:

    /**
     * Allocate a queue of qsize bytes on the heap
     * @param qsize     Size of the desired queue, in bytes.
     */
    Queue() : _top(0), _bottom(0), _size(0) {
    }


    /**
     * Place data on to a Queue
     *
     * Non-blocking. Will return error if queue is full.
     *
     * @param input the byte to place on the Queue
     *
     * @return 1 if successful, 0 otherwise
     */
    uint8_t enq(T input)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            if(_size == MAX_SIZE) return 1;

            _data[_bottom++] = input;
            if(_bottom >= MAX_SIZE) _bottom = 0;
            _size++;
        }
        return 0;
    }


    /**
     * Grab data from a Queue
     *
     * Non-blocking. Will return error if queue is empty
     *
     * @param target a pointer to a byte to place the read value
     *
     * @return 1 if successful, 0 otherwise
     */
    uint8_t deq(T* target)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            if(_size == 0) return 1;

            *target = _data[_top++];
            if(_top >= MAX_SIZE) _top = 0;
            _size--;
        }
        return 0;
    }


    /**
     * Get the current number of elements in the queue.
     *
     * Non-blocking.
     *
     * @return Number of elements in the queue
     */
    uint8_t size(void)
    {
        return _size;
    }

};
}

#endif /* _savr_queue_h_included_ */

