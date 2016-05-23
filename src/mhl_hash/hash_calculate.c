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

#include <facade_info/error_codes.h>
#include <generics/char_conversions.h>
#include <mhltools_common/logging.h>
#include <mhltools_common/usage_printing.h>

#include <mhltools_common/controlling_data.h>
#include <args_fileslist_support/aux_funcs.h>
#include <mhltools_common/hashing.h>
#include <mhltools_common/options.h>

#include <mhl_hash/hash_calculate.h>

typedef struct _st_calculate_options
{
  // common options for all applications
  st_controlling_data common;

  unsigned char opt_md5;
  unsigned char opt_sha1;
  unsigned char opt_xxhash;
  unsigned char opt_xxhash64;
    unsigned char opt_xxhash64be;
  unsigned char mhlformat_compatible;
} st_calculate_options;

typedef struct _st_aux_calculate_and_print_hash_data
{
  st_calculate_options* p_opts;
  st_conversion_settings* p_cs;  
} st_aux_calculate_and_print_hash_data;

static
int init_st_calculate_options(st_calculate_options* p_opt)
{
  if (p_opt == 0)
  {
    return ERRCODE_UNKNOWN_ERROR;
  }
  
  memset((void*) p_opt, 0, sizeof(*p_opt) / sizeof(char));

  return 0;
}

static
void free_st_calculate_options(st_calculate_options* p_opt)
{
  if (p_opt == 0)
  {
    return;
  }

  memset((void*) p_opt, 0, sizeof(*p_opt) / sizeof(char));  
  return;
}

static int
calculate_and_print_hash(const wchar_t* wfilename, void* data)
{
  int res;
  unsigned long long total_bytes = 0;
  char* hash_str;
  size_t hash_str_sz;
  st_aux_calculate_and_print_hash_data* p_data;
  char* filename;
  
  if (wfilename == 0 || wfilename[0] == L'\0' || data == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  p_data = (st_aux_calculate_and_print_hash_data*) data;

  filename = strdup_and_convert_from_wchar_to_locale(wfilename,
    p_data->p_cs, &res);

  if (res != 0)
  {
    printf("Failed to convert a filename from multi-byte characters to locale\n");
    return res;
  }

  if (p_data->p_opts->opt_md5)
  {
    res = 
      wcalculate_md5_hash_string(wfilename, &hash_str, &hash_str_sz,
                                 &total_bytes, &p_data->p_opts->common.logging_data);

    if (res != 0)
    {
      fprintf(
        stderr, 
        "Cannot calculate MD5 hash for the file: '%s'\n"
        "Description: %s\n",
         filename,
         mhl_error_code_description(res));

      free(filename);
      return res;
    }
    else 
    {       
      printf("MD5(%s)= %s\n", filename, hash_str);
      
      if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
      {
        printf("%s: read %llu bytes\n", filename, total_bytes);
      }
      
      free(hash_str);
    }
  }
  
  //
  if (p_data->p_opts->opt_sha1)
  {
    res = 
      wcalculate_sha1_hash_string(wfilename, &hash_str, &hash_str_sz,
                                  &total_bytes, &p_data->p_opts->common.logging_data);

    if (res != 0)
    {
      fprintf(
        stderr, 
        "Cannot calculate SHA1 hash for the file: '%s'\n"
        "Description: %s\n",
        filename,
        mhl_error_code_description(res));

      free(filename);
      return res;
    }
    else 
    {       
      printf("SHA1(%s)= %s\n", filename, hash_str);

      if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
      {
        printf("%s: read %llu bytes\n", filename, total_bytes);
      }

      free((void*) hash_str);
    }
  }
  
  if (p_data->p_opts->opt_xxhash)
  {
      res =
      wcalculate_xx_hash_string(wfilename, &hash_str, &hash_str_sz,
                                  &total_bytes, &p_data->p_opts->common.logging_data);

      if (res != 0)
      {
          fprintf(
                  stderr,
                  "Cannot calculate XX hash for the file: '%s'\n"
                  "Description: %s\n",
                  filename,
                  mhl_error_code_description(res));

          free(filename);
          return res;
      }
      else
      {
          printf("XXHash(%s)= %s\n", filename, hash_str);

          if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
          {
              printf("%s: read %llu bytes\n", filename, total_bytes);
          }

          free((void*) hash_str);
      }
  }
  if (p_data->p_opts->opt_xxhash64)
  {
      res =
      wcalculate_xx64_hash_string(wfilename, &hash_str, &hash_str_sz,
                                &total_bytes, &p_data->p_opts->common.logging_data);

      if (res != 0)
      {
          fprintf(
                  stderr,
                  "Cannot calculate XX64 hash for the file: '%s'\n"
                  "Description: %s\n",
                  filename,
                  mhl_error_code_description(res));

          free(filename);
          return res;
      }
      else
      {
          printf("XXHash64(%s)= %s\n", filename, hash_str);

          if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
          {
              printf("%s: read %llu bytes\n", filename, total_bytes);
          }

          free((void*) hash_str);
      }
  }
  if (p_data->p_opts->opt_xxhash64be)
  {
      res =
            wcalculate_xx64be_hash_string(wfilename, &hash_str, &hash_str_sz,
                                          &total_bytes, &p_data->p_opts->common.logging_data);

      if (res != 0)
      {
            fprintf(
                    stderr,
                    "Cannot calculate XX64BE hash for the file: '%s'\n"
                    "Description: %s\n",
                    filename,
                    mhl_error_code_description(res));

            free(filename);
            return res;
      }
      else
      {
          printf("XXHash64BE(%s)= %s\n", filename, hash_str);

          if (p_data->p_opts->common.logging_data.v_data.verbose_level >= VL_VERY_VERBOSE)
          {
              printf("%s: read %llu bytes\n", filename, total_bytes);
          }

          free((void*) hash_str);
      }
  }

  free(filename);
  return 0;
}

static 
int
process_calculate_hash(
  int argc, 
  const char * argv[], 
  st_calculate_options* opts,
  st_conversion_settings* p_cs)
{
  int res = 0;
  st_aux_calculate_and_print_hash_data cph_data;
  st_progress_data* p_progress_data;
  const char hash_opts[] = {opts->opt_sha1, opts->opt_md5, opts->opt_xxhash, opts->opt_xxhash64};
  const int count_hash_opts = sizeof(hash_opts)/sizeof(hash_opts[0]);
  size_t hashed_size = 0;
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

  // Double or triple the total size to process depending on the number of hashes that are needed
  for(i=0; i < count_hash_opts; ++i) {
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

  p_progress_data->processed_sz = 0;
  p_progress_data->logged_sz = 0;
  cph_data.p_cs = p_cs;
  cph_data.p_opts = opts;
  
  res = run_func_on_args(argc, argv,
    &opts->common,
    p_cs,
    (void*) &cph_data, // pass callback data
    calculate_and_print_hash); // pass callback function

  // Print finish message
  if (opts->common.logging_data.v_data.verbose_level >= VL_VERBOSE)
  {
    print_finish_message("Finished generating checksums", &opts->common.logging_data);
  }

  return res;
}

static
int parse_hash_calculate_params(int argc, const char* argv[],
  st_calculate_options* opts,
  st_conversion_settings* p_cs)
{
  int i;
  en_opts res;

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

    case OPT_M:
      opts->mhlformat_compatible = 1;
      break;

    case OPT_SEQ:
      opts->common.use_sequences = 1;
      break;


    case OPT_T:
      if ( i + 1 >= argc)
      {
        print_error(
        "Arguments error: "
        "'md5' and/or 'sha1' and/or 'xxhash' and/or 'xxhash64' hash-type argument must follow the '-t' "
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
        "'md5' and/or 'sha1' and/or 'xxhash' hash-type argument must follow the '-t' "
        "option.\n");

        return ERRCODE_WRONG_ARGUMENTS;
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

int run_calculate_hash(int argc, const char* argv[],
  st_conversion_settings* p_cs)
{
  int res;
  st_calculate_options calc_opts;
  
  res = init_st_calculate_options(&calc_opts);
  if (res != 0)
  {
      fprintf(
        stderr, 
        "Initialisation error: %s\n", 
        mhl_error_code_description(res));

      return res;
  }

  res = parse_hash_calculate_params(argc, argv, &calc_opts, p_cs);
  if (res != 0)
  {
    free_st_calculate_options(&calc_opts);
    mhlhash_usage();
    return res;
  }

  res = process_calculate_hash(argc, argv, &calc_opts, p_cs); 

  free_st_calculate_options(&calc_opts);
  return res;
}
