/*
 The MIT License (MIT)
 
 Copyright (c) 2016 Pomfort GmbH
 https://github.com/pomfort/mhl-tool
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

// This file contains definitions for helper functions for work
// with such elementary structures as arrays, strings, memory buffer

#ifndef _MHL_TOOLS_GENERICS_MEMORY_MANAGEMENT_H_
#define _MHL_TOOLS_GENERICS_MEMORY_MANAGEMENT_H_

/*
 * Increases the memory allocated for buffer
 * buffer is changed to point to increased memory, current_capacity is changed accordingly 
 * new additional memory is filled with 0
 * If buffer is NULL, it is initially allocated
 * @param void** buffer pointer to buffer
 * @param size_t* current_capacity the number of elements, which may be placed into the buffer
 * @param size_t new_capacity the new number of elements, which the buffer shall be able to contain
 * if new number is less than current number, then the buffer is truncated
 * @param item_size the size in bytes for one item
 * @return In case of success returns 0, othewise: non zero error code, buffer and capacity remains unchanged
 */
int
increase_allocated_memory(void** buffer, size_t* current_capacity,
  size_t new_capacity, unsigned int item_size);

#endif //_MHL_TOOLS_GENERICS_MEMORY_MANAGEMENT_H_
