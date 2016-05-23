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
#include <stdlib.h>
#include <string.h>

#include <facade_info/error_codes.h>
#include <generics/memory_management.h>

int increase_allocated_memory(void** buffer, size_t* current_capacity,
  size_t new_capacity, unsigned int item_size)
{
  void* new_buffer;

  // increase allocated memory
  new_buffer = realloc(*buffer, new_capacity * item_size);

  if (new_buffer == NULL)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  *buffer= new_buffer;

  if (new_capacity > *current_capacity)
  {
    // fill with 0 the additionally allocated memory
    // char* - typecast is used for correct arithmetic with pointer
    // sizeof(char) is probably a 1 byte, however divide to sizeof(char) to be sure
    // the work with buffer data is correct
    memset((char*)*buffer + *current_capacity * item_size / sizeof(char),
           0,
           (new_capacity - *current_capacity) * item_size / sizeof(char));
  }

  *current_capacity = new_capacity;
  return 0;
}
