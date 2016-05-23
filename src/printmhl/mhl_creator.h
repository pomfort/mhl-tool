#ifndef _MHL_TOOLS_PRINTMHL_MHL_CREATOR_H_
#define _MHL_TOOLS_PRINTMHL_MHL_CREATOR_H_

#include <mhltools_common/files_data.h>
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

#include <mhltools_common/logging.h>

#include <printmhl/create_mhl_files_data.h>

typedef struct _st_mhlcreate_data
{
   wchar_t* wworkdir;
   st_fs_wpath workdir_wpath;

   st_creator_data creator_data;
   st_files_data input_data;
   st_mhl_dirs_data mhl_paths;
   //This is just a pointer, no responcibility for allocating or freeing memory
   // for this pointer
   st_verbose_data* p_v_data;
} st_mhlcreate_data;

/**
 * Before any work with the st_mhlcreate_data structure, it must be initialized
 * with this call.
 * @param [in] st_verbose_data* p_v_data - must be not null
 * @return 0 on success, non-zero error code otherwise
 */
int init_mhlcreate_data(st_mhlcreate_data* data,
                        st_verbose_data* p_v_data);

//
// Set of functions for working with directories
//

void finalize_mhlcreate_data(st_mhlcreate_data* data);

// returns in case of success: 0
//         in case of failure: error code, and print error message to stderr
// This call shall be done after parameters parsing and before
// any further work with mhlcreate_data
int preprocess_mhlcreate_data(
  st_mhlcreate_data* data,
  const struct tm* start_gmtm,
  st_conversion_settings* p_cs);

int get_xml_date(char** date_str, struct tm* cur_gmtm);

int date_to_log_str(char** date_str, const struct tm* gmtm);

int
process_file(
  st_file_data_ext* file_data, 
  st_fs_wpath* work_wpath,
  st_conversion_settings* p_cs);

int
add_data_to_containing_folders(st_mhl_dirs_data* mhl_paths_ref,
  st_file_data_ext* file_data, unsigned int file_data_idx);

int create_mhl_files(st_mhlcreate_data* data, st_conversion_settings* p_cs);

#endif // _MHL_TOOLS_PRINTMHL_MHL_CREATOR_H_
