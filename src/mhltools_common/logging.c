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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <facade_info/error_codes.h>
#include <generics/memory_management.h>
#include <mhltools_common/logging.h>

void print_error(const char* err_msg)
{
  if (err_msg)
  {
    fprintf(stderr, "%s", err_msg);
  }
  else
  {
    fprintf(stderr, "Unknown run-time error, possibly out of memory");
  }
  fprintf(stderr, "\n");
}

// Do nothing if v_data is NULL, or log_str is NULL.
// Otherwise, free log_str, set log_str_capacity and log_str_len to 0,
// assign NULL to log_str
void clean_log_str(st_verbose_data* v_data)
{
  if (v_data == NULL || v_data->log_str == NULL)
  {
    return;
  }

  if (v_data->log_str_capacity == 0)
  {
    v_data->log_str = NULL;
    v_data->log_str_len = 0;
  }
  else
  {
    free(v_data->log_str);
    v_data->log_str_capacity = 0;
    v_data->log_str = NULL;
    v_data->log_str_len = 0;
  }
}

int logit(st_verbose_data* v_data, const char* format_str, ...)
{
  int str_len;
  va_list argptr;
  size_t min_required_capacity;
  size_t new_str_capacity;
  int res;
 
  va_start(argptr, format_str);

  str_len = vprintf(format_str, argptr);
  if (str_len < 0)
  {
    fprintf(stderr, "WARNING: vprintf failed. Further logging may be broken.\n");
    return ERRCODE_UNKNOWN_ERROR;
  }

  if (v_data->log_str == NULL)
  {
    v_data->log_str_capacity = 0;
  }

  min_required_capacity = v_data->log_str_len + str_len + 1;

  // increase allocated memory
  if (v_data->log_str_capacity < min_required_capacity)
  {
    new_str_capacity = v_data->log_str_capacity ? v_data->log_str_capacity * 2 : BUFF_SZ;
    while (new_str_capacity < min_required_capacity)
    {
      new_str_capacity *= 2;
    }

    res = increase_allocated_memory(
      (void**)&v_data->log_str,
      &v_data->log_str_capacity,
      new_str_capacity,
      sizeof(char));

    if (res != 0)
    {
      fprintf(stderr, "Out of memory.\n");
      fprintf(stderr, 
        "Failed to allocate buffer of size %lu bytes for log string.",
        (long unsigned int)new_str_capacity * sizeof(char));
      return res;
    }
  }

  va_end(argptr);
  va_start(argptr, format_str);
  
  str_len = vsprintf(v_data->log_str + v_data->log_str_len, format_str, argptr);
  if (str_len < 0)
  {
    fprintf(stderr, "WARNING: vsprintf failed. Further logging may be broken.\n");
    free(v_data->log_str);
    v_data->log_str = NULL;
    return ERRCODE_UNKNOWN_ERROR;
  }

  v_data->log_str_len += str_len;

  va_end(argptr);
  return 0;
}

void
print_minor_separator(FILE * file)
{
    fprintf(file, "----------------------\n");
}

void
print_major_separator(FILE * file)
{
    fprintf(file, "======================\n");
}

typedef enum _size_items
{
  BYTE = 0,
  KB = 1,
  MB = 2,
  GB = 3,
  TB = 4
} size_items;


void
print_start_message(const char* message_head, st_logging_data* logging_data)
{
  unsigned long long total_sz;
  unsigned long long tmp;
  unsigned char item;
  const char* item_names[5];

  item_names[BYTE] = "B";
  item_names[KB] = "KB";
  item_names[MB] = "MB";
  item_names[GB] = "GB";
  item_names[TB] = "TB";
    
  print_minor_separator(stderr);
  printf("%s for: \n", message_head);
  if (logging_data->progress_data.n_seqs)
  {
    printf("   %lu sequence(s) \n", logging_data->progress_data.n_seqs);
  }
  if (logging_data->progress_data.n_files)
  {
    printf("   %lu file(s) \n", logging_data->progress_data.n_files);
  }

  total_sz = logging_data->progress_data.total_sz;
  tmp = 1;
  for (item = BYTE; item < TB && tmp >= 1; ++item)
  {
    tmp = total_sz / 1024;
    if (tmp >= 1)
    {
      total_sz = tmp;
    }
  }

  printf("   with total filesize of %llu %s (%llu bytes)\n",
    total_sz, item_names[item - 1], logging_data->progress_data.total_sz);
  print_minor_separator(stderr);
}

void
print_finish_message(const char* message_head, st_logging_data* logging_data)
{
  print_start_message(message_head, logging_data);
    printf("Summary:\n");
  if (logging_data->progress_data.n_seqs)
  {
    if (logging_data->progress_data.n_seqs_failed)
    {
      printf("   %lu of %lu sequences FAILED\n",
        logging_data->progress_data.n_seqs_failed, logging_data->progress_data.n_seqs);
    }
    if (logging_data->progress_data.n_seqs_ok)
    {
      printf("   %lu of %lu sequences SUCCEEDED\n",
        logging_data->progress_data.n_seqs_ok, logging_data->progress_data.n_seqs);
    }
  }
  if (logging_data->progress_data.n_files)
  {
    if (logging_data->progress_data.n_files_failed)
    {
      printf("   %lu of %lu files FAILED\n",
        logging_data->progress_data.n_files_failed, logging_data->progress_data.n_files);
    }
    if (logging_data->progress_data.n_files_ok)
    {
      printf("   %lu of %lu files SUCCEEDED\n",
        logging_data->progress_data.n_files_ok, logging_data->progress_data.n_files);
    }
  }  
}

void
print_progress_message(st_logging_data* logging_data)
{
  unsigned long long total_sz_mb;
  unsigned long long processed_sz_mb;

  logging_data->progress_data.logged_sz = logging_data->progress_data.processed_sz;
  if (logging_data->v_data.verbose_level < VL_VERY_VERBOSE) {
    return;
  }
  
  printf("Processed ");
  if (logging_data->progress_data.n_seqs)
  {
    printf("%lu of %lu sequences, ",
      logging_data->progress_data.n_seqs_processed, logging_data->progress_data.n_seqs);
  }
  if (logging_data->progress_data.n_files)
  {
    printf("%lu of %lu files, ",
      logging_data->progress_data.n_files_processed, logging_data->progress_data.n_files);
  }
  total_sz_mb = logging_data->progress_data.total_sz / (1024*1024);
  processed_sz_mb = logging_data->progress_data.processed_sz / (1024*1024);

  printf("%llu of %llu MB (%llu bytes)\n",
    processed_sz_mb, total_sz_mb, logging_data->progress_data.processed_sz);
}

void
print_machine_progress_message(FILE* file, const st_logging_data* logging_data, unsigned long num_bytes)
{
  if (logging_data->v_data.machine_output) {
    fprintf(file, "%s|progress|%lu\n", logging_data->tool_name, num_bytes);
    fflush(file);
  }
}

void
print_output_meta_info(FILE* file,
                       const wchar_t* abs_file_name,
                       const st_logging_data* logging_data,
                       MHL_HASH_TYPE hash_type,
                       const char* u8str_hash_sum,
                       unsigned long long file_sz
                       ) {
  if (logging_data->v_data.machine_output) {
    fprintf(file, "%s|meta|%ls|HASHTYPE=%s,HASH=%s,FILESIZE=%lld\n", logging_data->tool_name, abs_file_name, mhl_hash_type_name(hash_type), u8str_hash_sum, file_sz);
    fflush(file);
  }
}

void
print_error_missing_file(FILE* file, const wchar_t* file_name, const st_logging_data* logging_data)
{
  if (logging_data->v_data.machine_output) {
    fprintf(file, "%s|error|%d|%ls|File does not exist\n", logging_data->tool_name, ERRCODE_NO_SUCH_FILE, file_name);
    fflush(file);
  }
}

void
print_output_verify_success(FILE* file, const st_logging_data* logging_data, const wchar_t* file_name)
{
  if (logging_data->v_data.machine_output) {
    fprintf(file, "%s|output|compare|%ls|OK\n", logging_data->tool_name, file_name);
    fflush(file);
  }
}

void
print_output_verify_failure(FILE* file, const wchar_t* file_name, const wchar_t* mhl_file_name, int err_code, const st_logging_data* logging_data)
{
  if (logging_data->v_data.machine_output) {
    fprintf(file, "%s|error|%d|%ls|%s\n", logging_data->tool_name, err_code, file_name, mhl_error_code_description(err_code));
  } else {
    fprintf(
            stderr,
            "File: '%ls' has not passed check for MHL file '%ls'.\n"
            "Description: '%s'\n",
            file_name,
            mhl_file_name,
            mhl_error_code_description(err_code));
  }
}

