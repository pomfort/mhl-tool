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

#ifndef _MHL_TOOLS_MHLTOOLS_COMMON_LOGGING_H_
#define _MHL_TOOLS_MHLTOOLS_COMMON_LOGGING_H_

#include <stdio.h>
#include <stdlib.h>
#include <mhltools_common/mhl_types.h>

#define BUFF_SZ (10 *1024)

void print_error(const char* err_msg);

typedef enum _en_verbose_level
{
  VL_NO_VERBOSE = 0,
  VL_VERBOSE,
  VL_VERY_VERBOSE
} en_verbose_level;

typedef struct _st_verbose_data
{
  en_verbose_level verbose_level; //has two levels for ON: 1 and 2. Level 0 is used for verbose OFF

  // is used to flag if the output shall be in a machine-readable form
  unsigned char machine_output;

  char* log_str;
  size_t log_str_capacity;
  size_t log_str_len;
} st_verbose_data;

int logit(st_verbose_data* v_data, const char* format_str, ...);

// Do nothing if v_data is NULL, or log_str is NULL.
// Otherwise, free log_str, set log_str_capacity and log_str_len to 0,
// assign NULL to log_str
void clean_log_str(st_verbose_data* v_data);

// statistical data, needed only for logging
typedef struct _st_progress_data
{
  unsigned long long total_sz;
  unsigned long long processed_sz;
  unsigned long long logged_sz;
  unsigned long n_items;
  unsigned long n_files;
  unsigned long n_seqs;
  unsigned long n_files_failed;
  unsigned long n_files_ok;
  unsigned long n_seqs_failed;
  unsigned long n_seqs_ok;
  unsigned long n_files_processed;
  unsigned long n_seqs_processed;
  unsigned long n_files_in_seq_processed;
} st_progress_data;

// common options for all modes
typedef struct _st_logging_data
{
  st_verbose_data v_data;
  st_progress_data progress_data;
  const char* tool_name;
} st_logging_data;

void 
print_start_message(const char* message_head, st_logging_data* logging_data);

void 
print_finish_message(const char* message_head, st_logging_data* logging_data);

void
print_progress_message(st_logging_data* logging_data);

void
print_machine_progress_message(FILE* file, const st_logging_data* logging_data, unsigned long num_bytes);

void
print_minor_separator(FILE * file);

void
print_major_separator(FILE * file);

void
print_error_missing_file(FILE* file, const wchar_t* file_name, const st_logging_data* logging_data);

void
print_output_verify_success(FILE* file, const st_logging_data* logging_data, const wchar_t* file_name);

void
print_output_meta_info(FILE* file,
                       const wchar_t* abs_file_name,
                       const st_logging_data* logging_data,
                       MHL_HASH_TYPE hash_type,
                       const char* u8str_hash_sum,
                       unsigned long long file_sz
                       );

void
print_output_verify_failure(FILE* file, const wchar_t* file_name, const wchar_t* mhl_file_name, int err_code, const st_logging_data* logging_data);

#endif // _MHL_TOOLS_MHLTOOLS_COMMON_LOGGING_H_