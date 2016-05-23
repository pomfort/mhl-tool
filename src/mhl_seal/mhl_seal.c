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
#include <string.h>
#include <time.h>

#include <facade_info/error_codes.h>
#include <generics/memory_management.h>
#include <generics/char_conversions.h>
#include <mhltools_common/logging.h>
#include <mhltools_common/usage_printing.h>

#include <mhltools_common/controlling_data.h>
#include <args_fileslist_support/aux_funcs.h>
#include <mhltools_common/hashing.h>
#include <mhltools_common/options.h>

#include <mhl_hash/hash_calculate.h>
#include <printmhl/mhl_creator.h>

#include <mhl_seal/mhl_seal.h>

typedef struct _st_seal_control_options
{
  // common options for all applications
  st_controlling_data common;

  unsigned char opt_md5;
  unsigned char opt_sha1;
  unsigned char opt_xxhash;
  unsigned char opt_xxhash64;
  unsigned char opt_xxhash64be;
} st_seal_control_options;

typedef struct _st_aux_calculate_and_print_hash_data
{
  st_seal_control_options* p_opts;
  st_mhlcreate_data* p_mhlcreate_data;
  st_conversion_settings* p_cs;
} st_aux_calculate_and_fill_hash_data;

static
int init_st_seal_control_options(st_seal_control_options* p_opt)
{
  if (p_opt == 0)
  {
    return ERRCODE_UNKNOWN_ERROR;
  }
  
  memset((void*) p_opt, 0, sizeof(*p_opt) / sizeof(char));

  return 0;
}

static
void free_st_seal_control_options(st_seal_control_options* p_opt)
{
  if (p_opt == 0)
  {
    return;
  }

  clean_log_str(&p_opt->common.logging_data.v_data);

  memset((void*) p_opt, 0, sizeof(*p_opt) / sizeof(char));  
  return;
}

static
int
prepare_data_for_mhl_create(const wchar_t* wfilename, const char* md5_hash_str,
  const char* sha1_hash_str, const char* xx_hash_str, const char* xx64_hash_str, const char* xx64be_hash_str, st_mhlcreate_data* p_mhlcreate_data,
  st_conversion_settings* p_cs)
{
  int res;
  st_files_data* p_files_data;
  size_t idx;

  if (p_mhlcreate_data == NULL)
  {
    fprintf(
      stderr, 
      "prepare_data_for_mhl_create: internal error - some data has been lost\n");

    return ERRCODE_INTERNAL_ERROR;
  }

  p_files_data = &p_mhlcreate_data->input_data;

  if (p_files_data == NULL ||
      p_files_data->files_data_array == NULL ||
      p_files_data->files_data_capacity == 0)
  {
    fprintf(
      stderr, 
      "prepare_data_for_mhl_create: internal error - some data has been lost\n");

    return ERRCODE_INTERNAL_ERROR;
  }

  if ( p_files_data->files_data_cnt == p_files_data->files_data_capacity)
  {
    // increase allocated memory twice
    res = increase_allocated_memory(
      (void**)&p_files_data->files_data_array,
        &p_files_data->files_data_capacity,
        p_files_data->files_data_capacity * 2,
        sizeof(st_file_data_ext));

    if (res != 0)
    {
      fprintf(stderr, "Out of memory.\n");
      return res;
    }
  }

  idx = p_files_data->files_data_cnt;

  res = fill_data_directly(wfilename, md5_hash_str, sha1_hash_str, xx_hash_str, xx64_hash_str, xx64be_hash_str,
    p_files_data->files_data_array + idx);

  ++p_files_data->files_data_cnt;
  if (res != 0)
  {
    return res;
  }

  res = process_file(
    p_files_data->files_data_array + idx, 
    &(p_mhlcreate_data->workdir_wpath),
    p_cs);

  if (res != 0)
  {
    return res;
  }

  res = add_data_to_containing_folders(&(p_mhlcreate_data->mhl_paths),
    p_files_data->files_data_array + idx, idx);

  if (res != 0)
  {
    return res;
  }

  return 0;
}

static int
calculate_and_fill_hash(const wchar_t* wfilename, void* data)
{
  int res;
  unsigned long long total_bytes = 0;
  char* sha1_hash_str = NULL;
  char* md5_hash_str = NULL;
  char* xx_hash_str = NULL;
  char* xx64_hash_str = NULL;
  char* xx64be_hash_str = NULL;

  char* filename;
  size_t hash_str_sz;
  st_aux_calculate_and_fill_hash_data* p_data;
  
  if (wfilename == 0 || wfilename[0] == L'\0' || data == 0)
  {
    fprintf(
      stderr, 
      "calculate_and_fill_hash: internal error - some data has been lost\n");

    return ERRCODE_INTERNAL_ERROR;
  }

  p_data = (st_aux_calculate_and_fill_hash_data*) data;

  if (p_data->p_opts == NULL || p_data->p_mhlcreate_data == NULL)
  {
    fprintf(
      stderr, 
      "calculate_and_fill_hash: internal error - some data has been lost\n");

    return ERRCODE_INTERNAL_ERROR;
  }
 
  filename = strdup_and_convert_from_wchar_to_locale(wfilename,
    p_data->p_cs, &res);

  if (res != 0)
  {
    filename = NULL;
  }

  if (p_data->p_mhlcreate_data->p_v_data->verbose_level)
  {
    if (filename == NULL)
    {
      logit(p_data->p_mhlcreate_data->p_v_data,
        "Processing file with unconvertible to locale encoding file name\n");
    }
    else
    {
      logit(p_data->p_mhlcreate_data->p_v_data, "Processing '%s'\n", filename);
    }
  }

  if (p_data->p_opts->opt_md5)
  {
    res =
      wcalculate_md5_hash_string(wfilename, &md5_hash_str, &hash_str_sz,
                                 &total_bytes, &p_data->p_opts->common.logging_data);

    if (res != 0)
    {
      fprintf(
        stderr, 
        "Cannot calculate MD5 hash for the file: %s\n"
        "Description: %s\n",
         filename == NULL ? "unconvertible to locale encoding file name" : filename,
         mhl_error_code_description(res));

      free(filename);
      return res;
    }

    if (p_data->p_opts->common.logging_data.v_data.verbose_level >=
        VL_VERY_VERBOSE)
    {
      printf("%s: calculated md5 hash, read %llu bytes\n",
        filename == NULL ? "unconvertible to locale encoding file name" : filename,
        total_bytes);
    }
  }

  if (p_data->p_opts->opt_sha1)
  {
    res = 
      wcalculate_sha1_hash_string(wfilename, &sha1_hash_str, &hash_str_sz,
                                  &total_bytes, &p_data->p_opts->common.logging_data);

    if (res != 0)
    {
      fprintf(
        stderr, 
        "Cannot calculate SHA1 hash for the file: '%s'\n"
        "Description: %s\n",
        filename == NULL ? "unconvertible to locale encoding file name" : filename,
        mhl_error_code_description(res));

      free(filename);
      return res;
    }

    if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
    {
      printf("%s: calculated sha1 hash, read %llu bytes\n",
        filename == NULL ? "unconvertible to locale encoding file name" : filename,
        total_bytes);
    }
  }

  if (p_data->p_opts->opt_xxhash)
  {
      res =
        wcalculate_xx_hash_string(wfilename, &xx_hash_str, &hash_str_sz,
                                  &total_bytes, &p_data->p_opts->common.logging_data);
      
      if (res != 0)
      {
          fprintf(
                  stderr,
                  "Cannot calculate XX hash for the file: '%s'\n"
                  "Description: %s\n",
                  filename == NULL ? "unconvertible to locale encoding file name" : filename,
                  mhl_error_code_description(res));
          
          free(filename);
          return res;
      }
      
      if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
      {
          printf("%s: calculated xx hash, read %llu bytes\n",
                 filename == NULL ? "unconvertible to locale encoding file name" : filename,
                 total_bytes);
      }
      
  }
  if (p_data->p_opts->opt_xxhash64)
  {
      res =
        wcalculate_xx64_hash_string(wfilename, &xx64_hash_str, &hash_str_sz,
                                    &total_bytes, &p_data->p_opts->common.logging_data);

      if (res != 0)
      {
          fprintf(
                  stderr,
                  "Cannot calculate XX64 hash for the file: '%s'\n"
                  "Description: %s\n",
                  filename == NULL ? "unconvertible to locale encoding file name" : filename,
                  mhl_error_code_description(res));

          free(filename);
          return res;
      }

      if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
      {
          printf("%s: calculated xx64 hash, read %llu bytes\n",
                 filename == NULL ? "unconvertible to locale encoding file name" : filename,
                 total_bytes);
      }

  }
  if (p_data->p_opts->opt_xxhash64be)
  {
    res =
      wcalculate_xx64be_hash_string(wfilename, &xx64be_hash_str, &hash_str_sz,
                                    &total_bytes, &p_data->p_opts->common.logging_data);

    if (res != 0)
    {
        fprintf(
                stderr,
                "Cannot calculate XX64BE hash for the file: '%s'\n"
                "Description: %s\n",
                filename == NULL ? "unconvertible to locale encoding file name" : filename,
                mhl_error_code_description(res));

        free(filename);
        return res;
    }

    if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
    {
        printf("%s: calculated xx64be hash, read %llu bytes\n",
               filename == NULL ? "unconvertible to locale encoding file name" : filename,
               total_bytes);
    }

  }

  res = prepare_data_for_mhl_create(wfilename, md5_hash_str, sha1_hash_str, xx_hash_str, xx64_hash_str, xx64be_hash_str,
    p_data->p_mhlcreate_data, p_data->p_cs);

  free(md5_hash_str);
  free(sha1_hash_str);
  free(xx_hash_str);
  free(xx64_hash_str);

  if (res != 0)
  {
    free(filename);
    return res;
  }

  if (p_data->p_mhlcreate_data->p_v_data->verbose_level)
  {
    logit(p_data->p_mhlcreate_data->p_v_data, "Done '%s'\n",
      filename == NULL ? "unconvertible to locale encoding file name" : filename);
  }

  free(filename);
  return 0;
}

static 
int
process_calculate_and_fill_hash(
  int argc, 
  const char * argv[], 
  st_seal_control_options* opts,
  st_mhlcreate_data* p_mhlcreate_data,
  st_conversion_settings* p_cs)
{
  int res = 0;
  st_aux_calculate_and_fill_hash_data cph_data;
  st_progress_data* p_progress_data;
  size_t hashed_size;
  const char hash_opts[] = {opts->opt_sha1, opts->opt_md5, opts->opt_xxhash, opts->opt_xxhash64, opts->opt_xxhash64be};
  const int count_hash_opts = sizeof(hash_opts)/sizeof(hash_opts[0]);
  int i;

  p_progress_data = &opts->common.logging_data.progress_data;
 
  p_progress_data->total_sz = 0;

  res = run_func_on_args(argc, argv, 
    &opts->common,
    p_cs, 
    (void*) &p_progress_data->total_sz, // pass callback data
    calculate_total_sz); // pass callback function

  if (res != 0)
  {
    return res;
  }

  // The previous call went well, so we processed all the files and 
  // sequences and know the total number of them.
  p_progress_data->n_files = p_progress_data->n_files_processed;
  p_progress_data->n_seqs = p_progress_data->n_seqs_processed;
    hashed_size = 0;
    // Double or triple the total size to process depending the hashes are needed
    for(i = 0; i < count_hash_opts; ++i) {
        if(hash_opts[i]) {
            hashed_size += p_progress_data->total_sz;
        }
    }
    p_progress_data->total_sz = hashed_size;
    
    
    
  // Print start message
  if (opts->common.logging_data.v_data.verbose_level >= VL_VERBOSE)
  {
    print_start_message("Started generating checksums", &opts->common.logging_data);
  }

  p_mhlcreate_data->input_data.files_data_array = 
    (st_file_data_ext*)calloc(INITIAL_FILES_CAPACITY, sizeof(st_file_data_ext));

  if (p_mhlcreate_data->input_data.files_data_array == NULL)
  {
    p_mhlcreate_data->input_data.files_data_cnt = 0;
    fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }

  p_mhlcreate_data->input_data.files_data_capacity = INITIAL_FILES_CAPACITY;
  p_mhlcreate_data->input_data.files_data_cnt = 0;

  p_progress_data->processed_sz = 0;
  p_progress_data->logged_sz = 0;
  cph_data.p_cs = p_cs;
  cph_data.p_opts = opts;
  cph_data.p_mhlcreate_data = p_mhlcreate_data;
  
  res = run_func_on_args(argc, argv,
    &opts->common,
    p_cs,
    (void*) &cph_data, // pass callback data
    calculate_and_fill_hash); // pass callback function

  // Print finish message
  if (opts->common.logging_data.v_data.verbose_level >= VL_VERBOSE)
  {
    print_finish_message("Finished generating checksums", &opts->common.logging_data);
  }

  return res;
}

static
int parse_seal_params(int argc, const char* argv[],
  st_seal_control_options* opts,
  st_mhlcreate_data* data,
  st_conversion_settings* p_cs)
{
  int i;
  en_opts res;
  st_mhl_file_data* mhl_f_data;
  size_t wstr_sz;
  int res1;

  if (argc < 2)
  {
    print_error(
      "Arguments error: "
      "Incorrect number of arguments.\n");
    return ERRCODE_WRONG_ARGUMENTS;
  }

  // traverse through arguments
  i = 1;
  do
  {
    res = recognise_option(argv[i]); 
    switch (res)
    {
    case OPT_V:
      if (opts->common.logging_data.v_data.verbose_level == VL_VERY_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      opts->common.logging_data.v_data.verbose_level = VL_VERBOSE;
      break;

    case OPT_VV:
      if (opts->common.logging_data.v_data.verbose_level == VL_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      opts->common.logging_data.v_data.verbose_level = VL_VERY_VERBOSE;
      break;

    case OPT_Y:
      opts->common.logging_data.v_data.machine_output = 1;
      break;

    case OPT_SEQ:
      opts->common.use_sequences = 1;
      break;

    case OPT_T:
      if ( i + 1 >= argc)
      {
        print_error(
        "Arguments error: "
        "'md5' and/or 'sha1' and/or 'xxhash' and/or 'xxhash64' and/or 'xxhash64be' hash-type argument must follow the '-t' "
        "option.\n");

        return ERRCODE_WRONG_ARGUMENTS;
      }

      ++i;
      res = recognise_option(argv[i]);
      // no break here, analyze hash type

    case OPT_MD5:
    case OPT_SHA1:
    case OPT_XXHASH:
    case OPT_XXHASH64:
    case OPT_XXHASH64BE:
      if (res == OPT_MD5)
      {
        opts->opt_md5 = 1;
      }
      else if (res == OPT_SHA1)
      {
        opts->opt_sha1 = 1;
      }
      else if (res == OPT_XXHASH)
      {
          opts->opt_xxhash = 1;
      }
      else if (res == OPT_XXHASH64)
      {
          opts->opt_xxhash64 = 1;
      }
      else if (res == OPT_XXHASH64BE)
      {
          opts->opt_xxhash64be = 1;
      }
      else
      {
        print_error(
        "Arguments error: "
        "'md5' and/or 'sha1' and/or 'xxhash'  and/or 'xxhash64be' and/or 'xxhash64' hash-type argument must follow the '-t' "
        "option.\n");

        return ERRCODE_WRONG_ARGUMENTS;
      }
      break;

    case OPT_O:
      if (data->mhl_paths.mhl_files_data_cnt == data->mhl_paths.mhl_files_data_capacity)
      {
        res1 = increase_allocated_memory(
          (void**)&data->mhl_paths.mhl_files_data,
          &data->mhl_paths.mhl_files_data_capacity,
          data->mhl_paths.mhl_files_data_capacity? data->mhl_paths.mhl_files_data_capacity * 2 : 1,
          sizeof(st_mhl_file_data));

        if (res1 != 0)
        {
          fprintf(stderr, "Out of memory.\n");
          return res1;
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

      res1 = convert_composed_from_locale_to_wchar(
        argv[i],
        strlen(argv[i]),
        &mhl_f_data->mhl_wdirname,
        &wstr_sz,
        p_cs);

      if (res1 != 0)
      {
        fprintf(stderr, "Failed to convert directory name from locale encoding into "
          "wide char: %s.\n", argv[i]);
        return res1;
      }
      break;

    case NOT_OPT:
      opts->common.files_argv_index = i;
      if (!opts->opt_md5 && !opts->opt_sha1 && !opts->opt_xxhash && !opts->opt_xxhash64 && !opts->opt_xxhash64be)
      {
         opts->opt_md5 = 1;
      }
      res = NULL_OPT;
      break;

    case NULL_OPT:
    default:
      print_error(
        "Arguments error: "
        "Incorrect parameters order or number\n");
      return ERRCODE_WRONG_ARGUMENTS;
    }
    ++i;
  } while (i < argc && (res != NULL_OPT));

  return 0; 
}

int run_mhl_seal(int argc, const char* argv[])
{
  int res;
  st_seal_control_options seal_opts;
  st_mhlcreate_data mhlcreate_data;
  st_conversion_settings css;
  struct tm start_gmtm;
  struct tm finish_gmtm;

  mhlosi_setlocale();
  res = init_st_conversion_settings(&css);
  if (res != 0)
  {
    fprintf(
      stderr, 
      "Cannot init conversion settings: %s\n", 
      mhl_error_code_description(res));
    
    return res;
  }

  res = init_st_seal_control_options(&seal_opts);
  if (res != 0)
  {
    fprintf(
        stderr, 
        "Initialisation error: %s\n", 
        mhl_error_code_description(res));

    free_st_conversion_settings(&css);
    return res;
  }
  
  res = init_mhlcreate_data(&mhlcreate_data,
                            &seal_opts.common.logging_data.v_data);
  if (res != 0)
  {
    return res;
  }  

  res = get_xml_date(&(mhlcreate_data.creator_data.startdate_str), &start_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Getting or processing of startdate failed.\n");
    finalize_mhlcreate_data(&mhlcreate_data);
    return res;
  }
  
  res = date_to_log_str(&(mhlcreate_data.creator_data.startdate_log_str), &start_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Processing of startdate failed.\n");
    finalize_mhlcreate_data(&mhlcreate_data);
    return res;
  }

  res = parse_seal_params(argc, argv, &seal_opts, &mhlcreate_data, &css);
  if (res != 0)
  {
    free_st_seal_control_options(&seal_opts);
    free_st_conversion_settings(&css);
    mhlseal_usage();
    return res;
  }

  // This call shall be done after parameters parsing and before
  // any further work with mhlcreate_data
  res = preprocess_mhlcreate_data(&mhlcreate_data, &start_gmtm, &css);

  if (res != 0)
  {
    free_st_conversion_settings(&css);
    finalize_mhlcreate_data(&mhlcreate_data);
    free_st_seal_control_options(&seal_opts);
    return res;
  }

  if (mhlcreate_data.p_v_data->verbose_level)
  {
    logit(mhlcreate_data.p_v_data, "-------------------\n");
    logit(mhlcreate_data.p_v_data, 
          "Start date in UTC: %s.\n",
          mhlcreate_data.creator_data.startdate_log_str);
    logit(mhlcreate_data.p_v_data, "-------------------\n");
        logit(mhlcreate_data.p_v_data, "Calculating hash sums\n");
  }

  res = process_calculate_and_fill_hash(argc, argv, &seal_opts, &mhlcreate_data, &css); 
  if (res != 0)
  {
    fprintf(
        stderr, 
        "Error while calculating hashes: %s\n", 
        mhl_error_code_description(res));

    free_st_conversion_settings(&css);
    finalize_mhlcreate_data(&mhlcreate_data);
    free_st_seal_control_options(&seal_opts);
    return res;
  }

  res = get_xml_date(&(mhlcreate_data.creator_data.finishdate_str), &finish_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Getting or processing of finishdate failed.\n");
    free_st_conversion_settings(&css);
    finalize_mhlcreate_data(&mhlcreate_data);
    free_st_seal_control_options(&seal_opts);
    return res;
  }

  res = date_to_log_str(&(mhlcreate_data.creator_data.finishdate_log_str), &finish_gmtm);
  if (res != 0)
  {
    fprintf(stderr, "Processing of finishdate failed.\n");
    free_st_conversion_settings(&css);
    finalize_mhlcreate_data(&mhlcreate_data);
    free_st_seal_control_options(&seal_opts);
    return res;
  }

  res = create_mhl_files(&mhlcreate_data, &css);
  finalize_mhlcreate_data(&mhlcreate_data);
  free_st_conversion_settings(&css);
  free_st_seal_control_options(&seal_opts);
  return res;
}
