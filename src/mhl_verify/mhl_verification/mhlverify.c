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
#include <stdio.h>
#include <errno.h>

#include <facade_info/error_codes.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/controlling_data.h>
#include <args_fileslist_support/aux_funcs.h>
#include <parsemhl/mhl_file_handlers.h>
#include <mhl_verify/verify_options.h>
#include <mhl_verify/mhl_verification/check_file.h>

#include "mhlverify.h"

typedef struct _st_file_verify_data
{
  // common options for all applications
  st_controlling_data* p_common;
  
  // options for mhl verification
  st_mhl_verify_options* p_verify;

  st_conversion_settings* p_cs;

  // MHL file's data
  st_mhl_file_wcontent* p_mhl_file_wcontent;
  wchar_t* abs_mhl_wpath;
} st_file_verify_data;

static
int 
check_passed_file(const wchar_t* wargv, void* p_data)
{
  st_file_verify_data* p_verify_data;
  st_controlling_data* p_mco;
  st_mhl_verify_options* p_mvo;
  st_conversion_settings* p_cs;
  int res;
  wchar_t* abs_mhl_entity_wpath;

  p_verify_data = (st_file_verify_data*)p_data;

  p_mco = p_verify_data->p_common;
  p_mvo = p_verify_data->p_verify;
  p_cs = p_verify_data->p_cs;

  res =
    convert_to_absolute_normalized_wpath(
      wargv,
      &abs_mhl_entity_wpath,
      p_cs);

  if (res != 0)
  {
    fprintf(
            stderr, 
            "Cannot create absolute path for source file: '%ls' "
            "and MHL file: '%ls'\n",
            wargv,
            p_verify_data->abs_mhl_wpath);
      
    if (p_mco->logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
    {
      printf("\tChecking failed.\n");
    }
  }
  else
  {
    res = 
      check_file_against_mhl_wcontent(
                                      abs_mhl_entity_wpath,
                                      p_verify_data->p_mhl_file_wcontent,
                                      p_mvo->existence,
                                      p_mco);

    if (res != 0)
    {
      print_output_verify_failure(stderr,
                                  abs_mhl_entity_wpath,
                                  p_verify_data->abs_mhl_wpath,
                                  res,
                                  &p_verify_data->p_common->logging_data);      
      if (p_mco->logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
      {
        printf("\tChecking failed.\n");
      }
    }
  }

  free(abs_mhl_entity_wpath);
    
  return res;
}

static
int 
check_passed_files(
  int argc,
  const char* argv[],
  st_file_verify_data* p_verify_data)
{
  int res;
  st_logging_data* p_logging;

  p_logging = &p_verify_data->p_common->logging_data;
  p_logging->progress_data.total_sz = 0;

  res = run_func_on_args(argc, argv,
    p_verify_data->p_common,
    p_verify_data->p_cs,
    (void*) &p_logging->progress_data.total_sz, // pass callback data
    calculate_total_sz); // pass callback function

  if (res != 0)
  {
    return res;
  }

  // The previous call went well, so we processed all the files and
  // sequences and know the total number of them.
  p_logging->progress_data.n_files = p_logging->progress_data.n_files_processed;
  p_logging->progress_data.n_seqs = p_logging->progress_data.n_seqs_processed;
  p_logging->progress_data.processed_sz = 0;
  p_logging->progress_data.logged_sz = 0;

  // Print start message
  if (p_logging->v_data.verbose_level >= VL_VERBOSE)
  {
    print_start_message("Started checking of passed files against MHL file",
                        p_logging);
  }

  res = run_func_on_args(
    argc,
    argv,
    p_verify_data->p_common,
    p_verify_data->p_cs,
    (void*)p_verify_data, // this data will be passed to check_passed_file
    check_passed_file);

  // Print finish message
  if (p_logging->v_data.verbose_level >= VL_VERBOSE)
  {
    print_finish_message("\nFinished checking of passed files against MHL file",
                         p_logging);
  }

  return res;
}

static
int
compare_lastmodification(st_mhl_file_check_wdata* data1,
                         st_mhl_file_check_wdata* data2)
{
  if (data1->lastmodification_seconds < data2->lastmodification_seconds)
  {
    return -1;
  }
  else if (data1->lastmodification_seconds == data2->lastmodification_seconds)
  {
    return 0;
  }
  return 1;
}

static
int
check_files_from_mhl(st_file_verify_data* p_verify_data)
{
  int res;
  int full_res;
  st_mhl_file_check_wdata* el;
  st_mhl_file_check_wdata* tmp;
  st_mhl_file_wcontent* p_mhl_file_wcontent;
  st_controlling_data* p_common;
  st_progress_data* p_progress;
  st_verbose_data* p_verbose;
  st_mhl_verify_options* p_verify;
  st_mhlosi_stat wfl_stat;


  p_common = p_verify_data->p_common;
  p_progress = &p_common->logging_data.progress_data;
  p_verbose = &p_common->logging_data.v_data;
  p_verify = p_verify_data->p_verify;
  p_mhl_file_wcontent = p_verify_data->p_mhl_file_wcontent;

  p_progress->total_sz = 0;
  p_progress->n_seqs = 0;
  p_progress->n_files = 0;
  p_progress->n_files_processed = 0;
  full_res = 0;
  HASH_ITER(hh, p_mhl_file_wcontent->check_witems, el, tmp)
  {
    res = get_wfile_stat_data(el->abs_item_wfilename, &wfl_stat);
    if (res != 0)
    {
      if (res == ERRCODE_NO_SUCH_FILE)
      {
        if (p_verify->continue_on_error == 0) { // avoid diuble logging when continuing later
          print_error_missing_file(stderr, el->abs_item_wfilename, &p_verify_data->p_common->logging_data);
        }
        fprintf(stderr,
                "Error: File does not exist: '%ls'.\n",
                el->abs_item_wfilename);
      }
      else
      {
        fprintf(stderr,
                "Error: Cannot get file's data, stat() failed for file: %ls. "
                "Errno=%d. Error:%s\n",
                el->abs_item_wfilename, errno, strerror(errno));
      }
      full_res = res;
    }
    else
    {
      el->lastmodification_seconds = wfl_stat.st_data.st_mtime;
    }

    res = calculate_total_sz(el->abs_item_wfilename,
                             (void*)&p_progress->total_sz);
    
    if (res != 0)
    {
      full_res = res;
    }
    ++p_progress->n_files_processed;
  }
  p_progress->n_files = p_progress->n_files_processed;

  if (full_res != 0 && p_verify->continue_on_error == 0)
  {
      return full_res;
  }

  HASH_SORT(p_mhl_file_wcontent->check_witems, compare_lastmodification); 

  // Print start message
  if (p_verbose->verbose_level >= VL_VERBOSE)
  {
    print_start_message("Started checking of files from MHL file",
      &p_common->logging_data);
  }

  p_progress->n_files_processed = 0;
  p_progress->n_files_failed = 0;
  p_progress->n_files_ok = 0;
  p_progress->processed_sz = 0;
  p_progress->logged_sz = 0;

  HASH_ITER(hh, p_mhl_file_wcontent->check_witems, el, tmp)
  {
    if (p_verbose->verbose_level >= VL_VERY_VERBOSE)
    {
      printf("\tFile %ls\n", el->abs_item_wfilename);
    }
    print_output_meta_info(stderr,
                           el->abs_item_wfilename,
                           &p_verify_data->p_common->logging_data,
                           el->hash_type,
                           el->u8str_hash_sum,
                           el->file_sz);
    res = 
      check_file_against_mhl_wcontent(
        el->abs_item_wfilename,
        p_mhl_file_wcontent,
        p_verify->existence,
        p_common);

    ++p_progress->n_files_processed;

    if (res != 0)
    {
      ++p_progress->n_files_failed;

      full_res = res;
      
      print_output_verify_failure(stderr,
                                  el->abs_item_wfilename,
                                  p_verify_data->abs_mhl_wpath,
                                  res,
                                  &p_verify_data->p_common->logging_data);
      
      if (p_verbose->verbose_level >= VL_VERY_VERBOSE)
      {
        printf("\tCheck failed.\n");
      }        
    }  
    else
    {
      ++p_progress->n_files_ok;
      if (p_verbose->verbose_level >= VL_VERY_VERBOSE)
      {
        printf("\tCheck passed.\n");
      }
      print_output_verify_success(stderr,
                                  &p_verify_data->p_common->logging_data,
                                  el->abs_item_wfilename);
    }
  }

  // Print finish message
  if (p_verbose->verbose_level >= VL_VERBOSE)
  {
    print_finish_message("Finished checking of files from MHL file",
      &p_common->logging_data);
  }

  return full_res;
}


int
verify_mhl(
  int argc, 
  const char * argv[], 
  st_controlling_data* p_mco,
  st_mhl_verify_options* p_mvo, 
  st_conversion_settings* p_cs)
{
  int i, res;
  st_mhl_file_wcontent mhl_file_wcontent;
  wchar_t* abs_mhl_wpath;
  wchar_t* abs_arg_wpath;
  wchar_t* wargv;
  size_t wargv_sz;
  unsigned char mhl_not_found = 1;

  // This structure doesn't store any information itself,
  // it is used only to simplify passing the pointers to data structures
  // between functions,
  // it is not responsible for any memory management
  st_file_verify_data verify_data;
  
  if (p_mvo->f_option == 0)
  {
    // search for mhl file 
    for (i = p_mco->files_argv_index; i < argc && mhl_not_found; ++i)
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
        return res;
      }
      
      make_wpath_os_specific(wargv);

      res = convert_to_absolute_normalized_wpath(wargv, &abs_arg_wpath, p_cs);
      if (res != 0)
      {
        fprintf(
            stderr, 
            "Cannot convert path to MHL file ('%ls') to absolute path.\n"
            "Description: %s\n",
            wargv,
            mhl_error_code_description(res));
    
        free(wargv);
        return res;    
      }
      free(wargv);

      res = search_mhl_wfile(abs_arg_wpath, &p_mvo->f_wmhl, p_cs);
      
      if (res != 0)
      {
        fprintf(stderr, "MHL file is not found for the source file: %s ('%ls')\n",
                argv[i], abs_arg_wpath);
        free(abs_arg_wpath);
        return res;
      }
      free(abs_arg_wpath);

      // TODO: check for all the rest files, 
      // that the found MHL is correctly located!

      mhl_not_found = 0;
    }
  }
  
  res = convert_to_absolute_normalized_wpath(p_mvo->f_wmhl, &abs_mhl_wpath, p_cs);
  if (res != 0)
  {
    fprintf(
            stderr, 
            "Cannot convert path to MHL file ('%ls') to absolute path.\n"
            "Description: %s\n",
            p_mvo->f_wmhl,      
            mhl_error_code_description(res));
    
    return res;    
  }
  
  res = init_mhl_file_wcontent(&mhl_file_wcontent);
  if (res != 0)
  {
    fprintf(
            stderr, 
            "Internal error. Cannot init structures for MHL checking.\n"
            "Description: %s\n",
            mhl_error_code_description(res));
    
    free(abs_mhl_wpath);
    return res;
  }
  
  //
  res = parse_mhl_wfile(abs_mhl_wpath, &mhl_file_wcontent, p_cs);
  if (res != 0)
  {
    fprintf(
            stderr, 
            "Error occured during parsing of MHL file '%ls'.\n"
            "Description: %s\n",
            abs_mhl_wpath,
            mhl_error_code_description(res));
    
    free(abs_mhl_wpath);
    free_mhl_file_wcontent(&mhl_file_wcontent);
    return res;
  }
  
  //
  // check files 
  //
  verify_data.abs_mhl_wpath = abs_mhl_wpath;
  verify_data.p_common = p_mco;
  verify_data.p_verify = p_mvo;
  verify_data.p_cs = p_cs;
  verify_data.p_mhl_file_wcontent = &mhl_file_wcontent;

  if (p_mco->files_argv_index != 0)
  {
    // files for checking are passed via args
    res =  
      check_passed_files(argc, argv, &verify_data);
  }
  else 
  {
    res =
      check_files_from_mhl(&verify_data);
  }
  
  free(abs_mhl_wpath);
  free_mhl_file_wcontent(&mhl_file_wcontent);
  
  if (p_mco->logging_data.v_data.verbose_level >= VL_VERBOSE)
  {
    if (res == 0)
    {
      printf("Checking of MHL file content successful.\n");
    }
    else 
    {
      printf("Checking of MHL file content failed.\n");
    }
  }              
  
  return res;
}
