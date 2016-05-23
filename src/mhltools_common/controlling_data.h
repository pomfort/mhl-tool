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
#ifndef _MHL_TOOLS_MHLTOOLS_COMMON_CONTROLLING_DATA_H_
#define _MHL_TOOLS_MHLTOOLS_COMMON_CONTROLLING_DATA_H_

#include <mhltools_common/logging.h>

typedef struct _st_controlling_data
{
  st_logging_data logging_data;

  int files_argv_index;
  unsigned char stop_on_error;
  unsigned char use_sequences;
} st_controlling_data;

#endif // _MHL_TOOLS_MHLTOOLS_COMMON_CONTROLLING_DATA_H_
