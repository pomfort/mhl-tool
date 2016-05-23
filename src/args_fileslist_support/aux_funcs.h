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

#ifndef _MHL_TOOLS_ARGS_FILESLIST_SUPPORT_AUX_FUNCS_H_
#define _MHL_TOOLS_ARGS_FILESLIST_SUPPORT_AUX_FUNCS_H_

#include <generics/char_conversions.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/controlling_data.h>

/*
 * This function runs a callback function for all arguments,
 * and counts statistics into common_data.
 * It updates the following st_controlling_data fields:
 *   n_items,
 *   n_files_failed,
 *   n_files_ok,
 *   n_seqs_failed,
 *   n_seqs_ok,
 *   n_files_processed,
 *   n_seqs_processed
 *
 * FileProcessingCallback see definition of this callback 
 * in "os_file_handlers.h"
 */
int
run_func_on_args(
  int argc,
  const char * argv[],
  st_controlling_data* common_data,
  st_conversion_settings* p_cs,
  void* data, // this data will be passed to callback_fn
  FileProcessingCallback callback_fn);

/*
 * FileProcessingCallback function.
 * See definition of this callback in "os_file_handlers.h"
 * Incrementally increases total size passed in data on file's size
 * Parameters:
 * (void*) unsigned long long* data - total size to be incremented
 * const wchar_t* wfilename - file's name to get the size
 */
int
calculate_total_sz(const wchar_t* wfilename, void* data);


#endif //_MHL_TOOLS_ARGS_FILESLIST_SUPPORT_AUX_FUNCS_H_
