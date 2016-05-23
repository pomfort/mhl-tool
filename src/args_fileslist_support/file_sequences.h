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

#ifndef _MHL_TOOLS_ARGS_FILESLIST_SUPPORT_FILE_SEQUENCES_H_
#define _MHL_TOOLS_ARGS_FILESLIST_SUPPORT_FILE_SEQUENCES_H_

#include <wchar.h>
#include <generics/char_conversions.h>

/*
 * Implementation of file sequences.
 * File sequene is something like this: sequence-##01-22.mp3,
 * which will be replaced with the files:
 *  sequence-01.mp3 equence-02.mp3 ... equence-22.mp3
 * The number of '#' means the number of digits in the range end number.
 *
 */

typedef struct _st_mhl_sequence
{
  wchar_t* start_str;
  size_t start_len;
  unsigned long first_number;
  unsigned long last_number;
  unsigned long current_number;
  unsigned char padding;
  unsigned char number_len;
  wchar_t* end_str;
  size_t end_len;
} st_mhl_sequence;

/*
 * Parse sequence_pattern into beginning of filename, range start and end 
 * numbers, and ending of filename.
 */
int
init_mhl_sequence(
  const wchar_t* sequence_pattern,
  st_mhl_sequence* p_seq,
  st_conversion_settings* p_cs);

/*
 * clear the st_mhl_sequence structure
 */
                   
void
fini_mhl_sequence(
  st_mhl_sequence* p_seq);

/*
 * FileProcessingCallback see definition of this callback 
 * in "os_file_handlers.h"
 *
 */
int search_files_fit_to_sequence(
  const wchar_t* p_seq_path,
  st_conversion_settings* p_cs,
  void* data, // this data will be passed to callback_fn
  FileProcessingCallback callback_fn);

/*
 * Return 1 if passed src string is sequence,
 *        0 if passed src string is not sequence (does ot contain '#' chars) 
 */
unsigned char 
is_sequence(const wchar_t* wsrc);

#endif //_MHL_TOOLS_ARGS_FILESLIST_SUPPORT_FILE_SEQUENCES_H_
