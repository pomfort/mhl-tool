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

#ifndef _MHL_TOOLS_PRINTMHL_CREATE_MHL_FILES_DATA_H_
#define _MHL_TOOLS_PRINTMHL_CREATE_MHL_FILES_DATA_H_

typedef struct _st_creator_data
{
  char* login_name_str;
  char* full_name_str;
  char* host_name_str;
  char* startdate_str;
  char* startdate_log_str;
  char* finishdate_str;
  char* finishdate_log_str;
  char* log_str;
} st_creator_data;

typedef struct _st_files_refs
{
  unsigned int file_data_idx;
  wchar_t* relative_wfilename;

  struct _st_files_refs* next;
  struct _st_files_refs* prev;
} st_files_refs;

typedef struct _st_mhl_file_data
{
  wchar_t* mhl_wdirname;
  st_fs_wpath mhl_dir_wpath;
  wchar_t* mhl_wpath;
  FILE* fl_descr;

  // The list of references to files in this directory or it's subdirectories
  st_files_refs* files_inside_dir;
  st_files_refs* last;
} st_mhl_file_data;

typedef struct _st_mhl_dirs_data
{
  st_mhl_file_data* mhl_files_data;
  size_t mhl_files_data_capacity;
  unsigned int mhl_files_data_cnt;
} st_mhl_dirs_data;

#define MHLCREATE_NAME "mhl"

#endif // _MHL_TOOLS_PRINTMHL_CREATE_MHL_FILES_DATA_H_
