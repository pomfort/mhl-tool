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
#include <generics/os_check.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

#include <facade_info/version.h>
#include <facade_info/error_codes.h>
#include <generics/os_check.h>
#include <generics/char_conversions.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/memory_management.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/files_data.h>
#include <mhltools_common/logging.h>
#include <mhltools_common/usage_printing.h>

#include <printmhl/mhl_creator.h>
#include <printmhl/create_mhl_files_data.h>
#include <mhl_file/input_parse_mode.h>

typedef enum _en_input_modes
{
  MD_NOT_SET = 0,
  MD_STDIN = 1,
  MD_FILEIN  = 2
} en_input_modes;

typedef struct _st_input_parse_data
{
   en_input_modes mode;

   // filename of input file in MD_2_FILEIN,
   const char* filename;
} st_input_parse_data;

// returns in case of success: 0
//         in case of failure: error code, and print error message to stderr
static
int parse_printmhl_params(
  int argc, 
  const char * argv[],
  st_input_parse_data* mode_data,
  st_mhlcreate_data* data,
  st_conversion_settings* p_cs)
{
  int res;
  int i;
  st_mhl_file_data* mhl_f_data;
  size_t wstr_sz;

  if (argc < 2)
  {
    print_error(
      "Arguments error: "
      "Incorrect number of arguments.\n");
    return ERRCODE_WRONG_ARGUMENTS;
  }

  i = 1;
  do
  {
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
    {
      if (data->p_v_data->verbose_level == VL_VERY_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      data->p_v_data->verbose_level = VL_VERBOSE;
      ++i;
    }
    else if (strcmp(argv[i], "-vv") == 0 || strcmp(argv[i], "--very-verbose") == 0)
    {
      if (data->p_v_data->verbose_level == VL_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      data->p_v_data->verbose_level = VL_VERY_VERBOSE;
      ++i;
    }
    else if (strcmp(argv[i], "-y") == 0)
    {
      data->p_v_data->machine_output = 1;
      ++i;
    }
    else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stdin") == 0)
    {
      if ((mode_data->mode != MD_NOT_SET) && (mode_data->mode != MD_STDIN))
      {
        print_error(
          "Arguments error: "
          "The '-s' option can be used only in reading from standard input mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      mode_data->mode = MD_STDIN;
      ++i;
    }
    else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0)
    {
      if ((mode_data->mode != MD_NOT_SET) && (mode_data->mode != MD_FILEIN))
      {
        print_error(
          "Arguments error: "
          "The '-f' option can be used only in reading from file mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      mode_data->mode = MD_FILEIN;
      ++i;
      if (i == argc)
      {
        print_error(
          "Arguments error: "
          "A file name must follow the '-f' option.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      mode_data->filename = argv[i];
      ++i;
    }
    else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output-folder") == 0)
    {
      if (mode_data->mode >= 3) // mode is set, but not an MHL file producing
      {
        print_error(
          "Arguments error: "
          "The '-r' option can be used only in MHL file producing mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }

      if (data->mhl_paths.mhl_files_data_cnt == data->mhl_paths.mhl_files_data_capacity)
      {
        res = increase_allocated_memory(
          (void**)&data->mhl_paths.mhl_files_data,
          &data->mhl_paths.mhl_files_data_capacity,
          data->mhl_paths.mhl_files_data_capacity? data->mhl_paths.mhl_files_data_capacity * 2 : 1,
          sizeof(st_mhl_file_data));

        if (res != 0)
        {
          fprintf(stderr, "Out of memory.\n");
          return res;
        }
      }

      data->mhl_paths.mhl_files_data_cnt += 1;

      mhl_f_data = data->mhl_paths.mhl_files_data + data->mhl_paths.mhl_files_data_cnt - 1;
      ++i;

      if (i == argc)
      {
        print_error(
          "Arguments error: "
          "A directory name must follow the '-o' or '--output-folder' option.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }

      res = convert_composed_from_locale_to_wchar(
        argv[i],
        strlen(argv[i]),
        &mhl_f_data->mhl_wdirname,
        &wstr_sz,
        p_cs);

      if (res != 0)
      {
        fprintf(stderr, "Failed to convert directory name from locale encoding into "
          "wide char: %s.\n", argv[i]);
        return res;
      }

      ++i;
    }
    else
    {
      print_error(
        "Arguments error: "
        "Incorrect mhlfile usage.\n");
      return ERRCODE_WRONG_ARGUMENTS;
    }
  } while (i < argc);

  if (mode_data->mode == MD_NOT_SET)
  {
    print_error(
      "Arguments error: "
      "At least one of the '-s', '-f' options must be specified.\n");
    return ERRCODE_WRONG_ARGUMENTS;
  }

  return 0;
}

int
process_input(st_input_parse_data* mode_data, st_mhlcreate_data* data, st_conversion_settings* p_cs)
{
  char input_buf[BUFF_SZ];
  unsigned int i = 0;
  int res;
  char* str_res;
  FILE* input_fl;
//  input_fl = fopen("C:\\projects\\mhlhash\\mhl-1203h\\bin\\Windows_7_x64\\Debug\\TEST\\opssl_output", "r");
 
  data->input_data.files_data_array = 
    (st_file_data_ext*)calloc(INITIAL_FILES_CAPACITY, sizeof(st_file_data_ext));

  if (data->input_data.files_data_array == NULL)
  {
    data->input_data.files_data_cnt = 0;
    fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }

  data->input_data.files_data_capacity = 0;
  data->input_data.files_data_cnt = 0;

  if (mode_data->mode == MD_FILEIN)
  {
    input_fl = fopen(mode_data->filename, "r");
    if (input_fl == NULL)
    {
      fprintf(stderr, "Cannot open file for reading: %s. Errno=%d. Error:%s\n",
              mode_data->filename, errno, strerror(errno));
      return ERRCODE_IO_ERROR;
    }
  }
  else
  {
    input_fl = stdin;
  }

  while (!feof(input_fl))
  {
    if (i == data->input_data.files_data_capacity)
    {
      // increase allocated memory twice
      res = increase_allocated_memory(
        (void**)&data->input_data.files_data_array,
          &data->input_data.files_data_capacity,
          data->input_data.files_data_capacity? data->input_data.files_data_capacity * 2 : INITIAL_FILES_CAPACITY,
          sizeof(st_file_data_ext));

      if (res != 0)
      {
        fprintf(stderr, "Out of memory.\n");
        return res;
      }
    }

    input_buf[0] = '\0';

    str_res = fgets(input_buf, sizeof(char)*BUFF_SZ, input_fl);
    if (input_buf[0] == '\0' || str_res == NULL)
    {
      if (feof(input_fl))
      {
        break;
      }
      else 
      {
        print_error("Unrecognized data format: empty line is read.");
        return ERRCODE_WRONG_INPUT_FORMAT;
      }
    }
   
    if (data->p_v_data->verbose_level >= VL_VERY_VERBOSE)
    {
      logit(data->p_v_data, "%s", input_buf);
    }

    data->input_data.files_data_cnt += 1;

    res = fill_data_from_input(input_buf, data->input_data.files_data_array+i, p_cs);
    if (res != 0)
    {
      return res;
    }

    res = 
      process_file(
        data->input_data.files_data_array+i, 
        &(data->workdir_wpath),
        p_cs);

    if (res != 0)
    {
      return res;
    }

    res = add_data_to_containing_folders(&(data->mhl_paths),
      data->input_data.files_data_array+i, i);

    if (res != 0)
    {
       return res;
    }

    if (data->p_v_data->verbose_level >= VL_VERY_VERBOSE)
    {
      logit(data->p_v_data, "Done\n");
    }

    ++i;
  }

  if (mode_data->mode == MD_FILEIN)
  {
    fclose(input_fl);
  }

  if (i == 0)
  {
    print_error("Wrong input format: no hash sums were specified in stdin or input file");
    return ERRCODE_WRONG_INPUT_FORMAT;
  }
  data->input_data.files_data_cnt = i;  

  return 0;
}

int run_input_parse_mode(int argc, const char * argv[],
  st_conversion_settings* p_cs)
{
  st_mhlcreate_data data;
  st_input_parse_data mode_data;
  st_verbose_data v_data;
  struct tm start_gmtm;
  struct tm finish_gmtm;
  int res;

  memset((void*) &v_data, 0, sizeof(v_data) / sizeof(char));
  memset((void*) &mode_data, 0, sizeof(mode_data) / sizeof(char));
  
  res = init_mhlcreate_data(&data, &v_data);
  if (res != 0)
  {
    return res;
  }  
  
  res = get_xml_date(&(data.creator_data.startdate_str), &start_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Getting or processing of startdate failed.\n");
    finalize_mhlcreate_data(&data);
    return res;
  }

  res = date_to_log_str(&(data.creator_data.startdate_log_str), &start_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Processing of startdate failed.\n");
    finalize_mhlcreate_data(&data);
    return res;
  }

  res = parse_printmhl_params(argc, argv, &mode_data, &data, p_cs);
  if (res != 0)
  {
    mhlfile_usage();
    finalize_mhlcreate_data(&data);
    return res;
  }
  
  // This call shall be done after parameters parsing and before
  // any further work with mhlcreate_data
  res = preprocess_mhlcreate_data(&data, &start_gmtm, p_cs);
  if (res != 0)
  {
    finalize_mhlcreate_data(&data);
    return res;
  }
  
  if (data.p_v_data->verbose_level)
  {
    logit(data.p_v_data, "-------------------\n");
    logit(data.p_v_data, 
          "Start date in UTC: %s.\n",
          data.creator_data.startdate_log_str);
    logit(data.p_v_data, "-------------------\n");
    logit(data.p_v_data, "Processing input:\n");
  }

  res = process_input(&mode_data, &data, p_cs);
  if (res != 0)
  {
    finalize_mhlcreate_data(&data);
    return res;
  }

  res = get_xml_date(&(data.creator_data.finishdate_str), &finish_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Getting or processing of finishdate failed.\n");
    finalize_mhlcreate_data(&data);
    return res;
  }
    
  res = date_to_log_str(&(data.creator_data.finishdate_log_str), &finish_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Processing of finishdate failed.\n");
    finalize_mhlcreate_data(&data);
    return res;
  }

  res = create_mhl_files(&data, p_cs);
  finalize_mhlcreate_data(&data);
  
  return res;
}
 
