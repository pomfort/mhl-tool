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

#ifndef _MHL_TOOLS_PRINTMHL_PRINT_MHL_H_
#define _MHL_TOOLS_PRINTMHL_PRINT_MHL_H_

#include <mhltools_common/files_data.h>
#include "create_mhl_files_data.h"

int
create_mhl(st_creator_data* creator_data, st_verbose_data* v_data,
           st_mhl_file_data* mhl_file, st_files_data* files_data,
           st_conversion_settings* p_cs);

#endif // _MHL_TOOLS_PRINTMHL_PRINT_MHL_H_
