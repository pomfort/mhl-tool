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

#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <facade_info/error_codes.h>
#include <generics/char_conversions.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/logging.h>
#include <mhltools_common/controlling_data.h>
#include <args_fileslist_support/file_sequences.h>
#include "aux_funcs.h"

/*
 * This function runs a callback function for all arguments,
 * and counts statistics into common_data->progress_data.
 * It updates the following st_progress_data fields:
 *   n_items,
 *   n_files_failed,
 *   n_files_ok,
 *   n_seqs_failed,
 *   n_seqs_ok,
 *   n_files_processed,
 *   n_seqs_processed
 */
int
run_func_on_args(
  int argc, 
  const char * argv[], 
  st_controlling_data* common_data,
  st_conversion_settings* p_cs,
  void* data, // this data will be passed to callback_fn
  FileProcessingCallback callback_fn)
{
  int i, res, overall_res = 0, files_count;
  wchar_t* wargv;
  size_t wargv_sz;
  st_progress_data* p_progress_data;
  DIR_ENTRY_TYPE_FLAGS ent_type;

  // Processing files
  files_count = argc - common_data->files_argv_index;
  p_progress_data = &common_data->logging_data.progress_data;
  p_progress_data->n_items = files_count;
  p_progress_data->n_files_failed = 0;
  p_progress_data->n_files_ok = 0;
  p_progress_data->n_seqs_failed = 0;
  p_progress_data->n_seqs_ok = 0;
  p_progress_data->n_files_processed = 0;
  p_progress_data->n_seqs_processed = 0;

  for (i = common_data->files_argv_index; i < argc; ++i)
  {
    res = 
      convert_composed_from_locale_to_wchar(
        argv[i], 
        strlen(argv[i]), 
        &wargv,
        &wargv_sz,
        p_cs);

    if (res != 0)
    {
      fprintf(
        stderr,
        "Cannot convert %s name %s, from locale to UTF32\n",
        common_data->use_sequences ? "file sequence" : "file"	,
        argv[i]);

      return res;
    }
    
    make_wpath_os_specific(wargv);

    if (common_data->use_sequences && is_sequence(wargv))
    {
      res = 
        search_files_fit_to_sequence(
          wargv,
          p_cs,
          data, // pass callback data
          callback_fn); // pass callback function
      
      ++p_progress_data->n_seqs_processed;
      if (res != 0 && res != ERRCODE_STOP_SEARCH)
      {
        fprintf(
          stderr,
          "Error while files sequence processing.\n"
          "Description: %s\n",
          mhl_error_code_description(res));

        ++p_progress_data->n_seqs_failed;
        overall_res = res;
      }
      else
      {
        ++p_progress_data->n_seqs_ok;
      }
    }
    else
    {
      res = get_file_type(wargv, &ent_type);
      if (res != 0)
      {
        ++p_progress_data->n_files_processed;
        ++p_progress_data->n_files_failed;
        overall_res = res;
      }
      else if (ent_type == DETF_DIR)
      { 
        res =
          process_files_recurs(
            wargv,
            p_cs,
            1, // stop on error
            &p_progress_data->n_files_processed,
            &p_progress_data->n_files_failed,
            &p_progress_data->n_files_ok,
            data,
            callback_fn);

        if (res != 0 && res != ERRCODE_STOP_SEARCH)
        {
          overall_res = res;
        }
      }
      else if (ent_type == DETF_FILE) 
      {
        res = 
          callback_fn(wargv, data);
      
        ++p_progress_data->n_files_processed;
        if (res != 0 && res != ERRCODE_STOP_SEARCH)
        {
          ++p_progress_data->n_files_failed;
          overall_res = res;
        }
        else
        {
          ++p_progress_data->n_files_ok;
        }
      }
      else 
      {
        fprintf(
          stderr,
          "Warning: the path is not a valid directory or file:\n%s\n"
          "         ",
          argv[i]);

        print_ent_type(ent_type);
        fprintf(stderr, " Ignoring...\n\n");
      }
    }
    free(wargv);
  } //for
  
  return overall_res;
}

int
calculate_total_sz(const wchar_t* wfilename, void* data)
{
  int res;
  unsigned long long* p_total_sz;
  unsigned long long f_sz;
  
  if (wfilename == 0 || wfilename[0] == L'\0' || data == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  p_total_sz = (unsigned long long*) data;
  
  res = get_wfile_size(wfilename, &f_sz);
  if (res != 0)
  {
    print_minor_separator(stderr);
    if (res == ERRCODE_NO_SUCH_FILE)
    {
     // print_error_missing_file(stderr, el->abs_item_wfilename, &p_verify_data->p_common->logging_data);
      fprintf(stderr, "Error: File does not exist: '%ls'.\n",
              wfilename);
    }
    else
    {
      fprintf(stderr, "Error:  Cannot get filesize for file: "
              "%ls\nReason: %s\n",
              wfilename, strerror(errno));
    }
    print_minor_separator(stderr);
    return res;
  }

  *p_total_sz += f_sz;

  return 0;
}
