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
#include <errno.h>

#include <facade_info/error_codes.h>
#include <generics/char_conversions.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/logging.h>
#include <mhltools_common/hashing.h>
#include <mhltools_common/options.h>
#include <mhltools_common/usage_printing.h>

#include <mhl_hash/file_verify.h>

typedef struct _st_file_check_options
{
  // common options for all applications
  st_logging_data logging_data;

  unsigned char check_hash;

  const char* hash_val;
  wchar_t* f_wname;
  unsigned char opt_md5;
  unsigned char opt_sha1;
  unsigned char opt_xxhash;
    unsigned char opt_xxhash64;
    unsigned char opt_xxhash64be;
} st_file_check_options;

// returns in case of success: 0
//         in case of failure: error code, and print error message to stderr
int parse_file_check_params(
  int argc, 
  const char * argv[], 
  st_file_check_options* opts, 
  st_conversion_settings* p_cs)
{
  int i;
  int ires;
  en_opts res;
  en_opts res1;
  size_t str_sz;

  if (argc < 4)
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
      if (opts->logging_data.v_data.verbose_level == VL_VERY_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      opts->logging_data.v_data.verbose_level = VL_VERBOSE;
      break;

    case OPT_VV:
      if (opts->logging_data.v_data.verbose_level == VL_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      opts->logging_data.v_data.verbose_level = VL_VERY_VERBOSE;
      break;

    case OPT_Y:
      opts->logging_data.v_data.machine_output = 1;
      break;

    case OPT_H:  
      if (opts->check_hash)
      {
        print_error(
          "Arguments error: "
          "Only the one '-h' option can be used in hash checking mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }

      opts->check_hash = 1;

      res1 = recognise_option(argv[++i]);
      if (res1 == OPT_MD5)
      { 
        opts->opt_md5 = 1;
        res1 = recognise_option(argv[++i]);
      }
      else if (res1 == OPT_SHA1)
      {
        opts->opt_sha1 = 1;
        res1 = recognise_option(argv[++i]);
      }
      else if (res1 == OPT_XXHASH)
      {
          opts->opt_xxhash = 1;
          res1 = recognise_option(argv[++i]);
      }
      else if (res1 == OPT_XXHASH64)
      {
          opts->opt_xxhash64 = 1;
          res1 = recognise_option(argv[++i]);
      }
      else if (res1 == OPT_XXHASH64BE)
      {
          opts->opt_xxhash64be = 1;
          res1 = recognise_option(argv[++i]);
      }
            
      if (res1 != NOT_OPT)
      {
        print_error(
          "Arguments error: "
          "HASH value must be specified after the '-h' option "
          "in hash checking mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }

      str_sz = strlen(argv[i]);

      if (opts->opt_md5)
      {
        if ( str_sz != (MHL_MD5_HASH_BYTES_SZ * 2) &&
             str_sz != ((MHL_MD5_HASH_BYTES_SZ * 2)+ strlen(MD5_HASH_PREFIX)) )
        {
          print_error(
            "Arguments error: "
            "Incorrect length of md5 HASH value\n");
          return ERRCODE_WRONG_ARGUMENTS;
        }
      }
      else if (opts->opt_sha1)
      {
        
        if ( str_sz != (MHL_SHA1_HASH_BYTES_SZ * 2) && 
             str_sz != ((MHL_SHA1_HASH_BYTES_SZ * 2)+ strlen(SHA1_HASH_PREFIX)) )
        {
          print_error(
            "Arguments error: "
            "Incorrect length of sha1 HASH value\n");
          return ERRCODE_WRONG_ARGUMENTS;
        }
      }
      else if (opts->opt_xxhash)
      {
          
          if ( str_sz != (MHL_XXHASH_HASH_BYTES_SZ * 2) &&
              str_sz != ((MHL_XXHASH_HASH_BYTES_SZ * 2)+ strlen(XXHASH_HASH_PREFIX)) )
          {
              print_error(
                          "Arguments error: "
                          "Incorrect length of xxHash HASH value\n");
              return ERRCODE_WRONG_ARGUMENTS;
          }
      }
      else if (opts->opt_xxhash64 || opts->opt_xxhash64be)
      {

          if ( str_sz != (MHL_XXHASH64_HASH_BYTES_SZ * 2) &&
              str_sz != ((MHL_XXHASH64_HASH_BYTES_SZ * 2)+ strlen(XXHASH64_HASH_PREFIX)) )
          {
              print_error(
                          "Arguments error: "
                          "Incorrect length of xxHash64 HASH value\n");
              return ERRCODE_WRONG_ARGUMENTS;
          }
      }
      else
      {
        if (str_sz == (MHL_MD5_HASH_BYTES_SZ * 2) || 
            str_sz == ((MHL_MD5_HASH_BYTES_SZ * 2)+ strlen(MD5_HASH_PREFIX)))
        {
          opts->opt_md5 = 1;
        }
        else if (str_sz == (MHL_SHA1_HASH_BYTES_SZ * 2) || 
                 str_sz == ((MHL_SHA1_HASH_BYTES_SZ * 2) + 
                            strlen(SHA1_HASH_PREFIX)))
        {
          opts->opt_sha1 = 1;
        }
        else if (str_sz == (MHL_XXHASH_HASH_BYTES_SZ * 2) ||
                 str_sz == ((MHL_XXHASH_HASH_BYTES_SZ * 2) +
                            strlen(XXHASH_HASH_PREFIX)))
        {
            opts->opt_xxhash = 1;
        }
        else if (str_sz == (MHL_XXHASH64_HASH_BYTES_SZ * 2) ||
                 str_sz == ((MHL_XXHASH64_HASH_BYTES_SZ * 2) +
                            strlen(XXHASH64_HASH_PREFIX)))
        {
            opts->opt_xxhash64 = 1;
        }
        else if (str_sz == ((MHL_XXHASH64BE_HASH_BYTES_SZ * 2) +
                            strlen(XXHASH64BE_HASH_PREFIX)))
        {
            opts->opt_xxhash64be = 1;
        }
        else // not md5, sha1 or xxhsh length
        {
          print_error(
            "Arguments error: "
            "Hash value doesn't has the length of MD5, SHA1, xxHash or xxHash64\n");

          return ERRCODE_WRONG_ARGUMENTS;
        }
      }

      if (opts->opt_md5 && str_sz == ((MHL_MD5_HASH_BYTES_SZ * 2)+ strlen(MD5_HASH_PREFIX)))
      {
        opts->hash_val = argv[i] + strlen(MD5_HASH_PREFIX);
      }
      else if (opts->opt_sha1 && str_sz == ((MHL_SHA1_HASH_BYTES_SZ * 2)+ strlen(SHA1_HASH_PREFIX)))
      {
        opts->hash_val = argv[i] + strlen(SHA1_HASH_PREFIX);
      }
      else if (opts->opt_xxhash && str_sz == ((MHL_XXHASH_HASH_BYTES_SZ * 2)+ strlen(XXHASH_HASH_PREFIX)))
      {
          opts->hash_val = argv[i] + strlen(XXHASH_HASH_PREFIX);
      }
      else if (opts->opt_xxhash64 && str_sz == ((MHL_XXHASH64_HASH_BYTES_SZ * 2)+ strlen(XXHASH64_HASH_PREFIX)))
      {
          opts->hash_val = argv[i] + strlen(XXHASH64_HASH_PREFIX);
      }
      else if (opts->opt_xxhash64be && str_sz == ((MHL_XXHASH64BE_HASH_BYTES_SZ * 2)+ strlen(XXHASH64BE_HASH_PREFIX)))
      {
          opts->hash_val = argv[i] + strlen(XXHASH64BE_HASH_PREFIX);
      }
      else
      {
        opts->hash_val = argv[i];
      }

      break;

    case OPT_F:
      if ((i + 1) == argc)
      {
        print_error(
          "Arguments error: "
          "a file name must follow after the '-f' parameter\n");

        return ERRCODE_WRONG_ARGUMENTS;
      }

      ++i;
          
      opts->f_wname = 
        strdup_and_convert_from_locale_to_wchar(argv[i], p_cs, &ires);

      if (opts->f_wname == NULL)
      {
        return ires == 0 ? ERRCODE_OUT_OF_MEM : ires;
      }

      // change paths separators (makes sense only for windows)
      make_wpath_os_specific(opts->f_wname); 
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


int
run_check_hash(st_file_check_options* opts)
{
  int res;
  unsigned long long total_bytes = 0;
  char* hash_str;
  size_t hash_str_sz;
  size_t passed_hash_str_sz;
  unsigned long long total_sz_mb;
  
  if (!opts->opt_md5 && !opts->opt_sha1 && !opts->opt_xxhash && !opts->opt_xxhash64 && !opts->opt_xxhash64be)
  {
    fprintf(
      stderr, 
      "Cannot check hash for the file '%ls', "
      "hash type is empty\n",
      opts->f_wname);
    
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  res = get_wfile_size(opts->f_wname, &opts->logging_data.progress_data.total_sz);
  if (res != 0)
  {
    if (res == ERRCODE_NO_SUCH_FILE)
    {
      fprintf(stderr, "Error: File does not exist: '%ls'.\n",
              opts->f_wname);
    }
    else
    {
      fprintf(stderr, "Error: Cannot get file's size for file: " 
              "%ls. Reason:%s\n",
              opts->f_wname, strerror(errno));
    }
    return res;
  }
  total_sz_mb = opts->logging_data.progress_data.total_sz / (1024*1024);
  opts->logging_data.progress_data.logged_sz = 0;
  opts->logging_data.progress_data.processed_sz = 0;
  opts->logging_data.progress_data.n_files = 1;
  opts->logging_data.progress_data.n_seqs = 0;

  passed_hash_str_sz = strlen(opts->hash_val);

  // Processing file
  if (opts->opt_md5)
  {
    if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
    {
      print_minor_separator(stderr);
      printf("Started checking MD5 hash for the file '%ls'\n"
        "   with the size of %llu MB (%llu bytes)\n",
        opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
      print_minor_separator(stderr);
    }

    // Note: size of passed hash is checed on argument parsing stage
    
    res = 
      wcalculate_md5_hash_string(opts->f_wname, &hash_str, &hash_str_sz,
                                 &total_bytes, &opts->logging_data);
    	
    if (res != 0)
    {
      fprintf(
        stderr, 
        "Cannot calculate MD5 hash for the file: '%ls'\n"
        "Description: %s\n",
        opts->f_wname,
        mhl_error_code_description(res));

      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
        print_minor_separator(stderr);
        printf("Finished checking MD5 hash for the file '%ls'\n"
          "   with the size of %llu MB (%llu bytes)\n",
          opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
        print_minor_separator(stderr);
        printf("Summary: FAILED\n");
      }
      
      return res;
    }
  
    if (passed_hash_str_sz != hash_str_sz)
    {
      free(hash_str);
      
      fprintf(
        stderr, 
        "Check MD5 hash for file '%ls' failed.\n"
        "Size of specified hash does not equal "
        "size of calculated hash. (%lu != %lu)\n",
        opts->f_wname,
        (unsigned long)passed_hash_str_sz,
        (unsigned long)hash_str_sz);

      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
        print_minor_separator(stderr);
        printf("Finished checking MD5 hash for the file '%ls'\n"
          "   with the size of %llu MB (%llu bytes)\n",
          opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
        print_minor_separator(stderr);
        printf("Summary: FAILED\n");
      }
      
      return ERRCODE_MHL_CHECK_HASH_FAILED;          
    }
    
    if (mhlosi_strncasecmp(hash_str, opts->hash_val, hash_str_sz) != 0)
    {
      fprintf(
        stderr, 
        "Check MD5 hash for file '%ls' failed.\n"
        "Passed hash sum does not match to calculated hash:\n"
        "%s != %s\n",
        opts->f_wname,
        opts->hash_val,
        hash_str);
     
      free(hash_str);
      
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
        print_minor_separator(stderr);
        printf("Finished checking MD5 hash for the file '%ls'\n"
          "   with the size of %llu MB (%llu bytes)\n",
          opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
        print_minor_separator(stderr);
        printf("Summary: FAILED\n");
      }
      
      return ERRCODE_MHL_CHECK_HASH_FAILED;          
    }

    free(hash_str);
    
    if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
    {
      print_minor_separator(stderr);
      printf("Finished checking MD5 hash for the file '%ls'\n"
        "   with the size of %llu MB (%llu bytes)\n",
        opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
      print_minor_separator(stderr);
      printf("Summary: SUCCEEDED\n");
    }
  }
  else if (opts->opt_sha1)
  {
    if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
    {
      print_minor_separator(stderr);
      printf("Started checking SHA1 hash for the file '%ls'\n"
        "   with the size of %llu MB (%llu bytes)\n",
        opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
      print_minor_separator(stderr);
    }
   
    // Note: size of passed hash is checed on argument parsing stage
    
    res = 
      wcalculate_sha1_hash_string(opts->f_wname, &hash_str, &hash_str_sz,
                                  &total_bytes, &opts->logging_data);

    if (res != 0)
    {
      fprintf(
        stderr, 
        "Cannot calculate SHA1 hash for the file: '%ls'\n"
        "Description: %s\n",
        opts->f_wname,
        mhl_error_code_description(res));
      
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
        print_minor_separator(stderr);
        printf("Finished checking SHA1 hash for the file '%ls'\n"
          "   with the size of %llu MB (%llu bytes)\n",
          opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
        print_minor_separator(stderr);
        printf("Summary: FAILED\n");
      }
      
      return res;
    }
    
    if (passed_hash_str_sz != hash_str_sz)
    {
      free(hash_str);
      
      fprintf(
        stderr, 
        "Check SHA1 hash for file '%ls' failed.\n"
        "Size of passed hash sum does not equal to "
        "size of calculated hash. (%lu != %lu)\n", 
        opts->f_wname,
        (unsigned long)passed_hash_str_sz,
        (unsigned long)hash_str_sz);
      
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
        print_minor_separator(stderr);
        printf("Finished checking SHA1 hash for the file '%ls'\n"
          "   with the size of %llu MB (%llu bytes)\n",
          opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
        print_minor_separator(stderr);
        printf("Summary: FAILED\n");
      }
      
      return ERRCODE_MHL_CHECK_HASH_FAILED;          
    }
    
    if (mhlosi_strncasecmp(hash_str, opts->hash_val, hash_str_sz) != 0)
    {
      fprintf(
        stderr, 
        "Check SHA1 hash for file '%ls' failed.\n"
        "Passed hash sum does not match to calculated hash: %s\n", 
        opts->f_wname,
        hash_str);
     
      free(hash_str);
      
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
        print_minor_separator(stderr);
        printf("Finished checking SHA1 hash for the file '%ls'\n"
          "   with the size of %llu MB (%llu bytes)\n",
          opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
        print_minor_separator(stderr);
        printf("Summary: FAILED\n");
      }
      
      return ERRCODE_MHL_CHECK_HASH_FAILED;          
    }
    
    free(hash_str);
    
    if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
    {
      print_minor_separator(stderr);
      printf("Finished checking SHA1 hash for the file '%ls'\n"
        "   with the size of %llu MB (%llu bytes)\n",
        opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
      printf("Summary: SUCCEEDED\n");
    }
  }
  else if (opts->opt_xxhash)
  {
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
          print_minor_separator(stderr);
          printf("Started checking XX hash for the file '%ls'\n"
                 "   with the size of %llu MB (%llu bytes)\n",
                 opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
          print_minor_separator(stderr);
      }
      
      // Note: size of passed hash is checed on argument parsing stage
      
      res =
      wcalculate_xx_hash_string(opts->f_wname, &hash_str, &hash_str_sz,
                                  &total_bytes, &opts->logging_data);
      
      if (res != 0)
      {
          fprintf(
                  stderr,
                  "Cannot calculate XX hash for the file: '%ls'\n"
                  "Description: %s\n",
                  opts->f_wname,
                  mhl_error_code_description(res));
          
          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }
          
          return res;
      }
      
      if (passed_hash_str_sz != hash_str_sz)
      {
          free(hash_str);
          
          fprintf(
                  stderr,
                  "Check XX hash for file '%ls' failed.\n"
                  "Size of passed hash sum does not equal to "
                  "size of calculated hash. (%lu != %lu)\n",
                  opts->f_wname,
                  (unsigned long)passed_hash_str_sz,
                  (unsigned long)hash_str_sz);
          
          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }
          
          return ERRCODE_MHL_CHECK_HASH_FAILED;
      }
      
      if (mhlosi_strncasecmp(hash_str, opts->hash_val, hash_str_sz) != 0)
      {
          fprintf(
                  stderr,
                  "Check XX hash for file '%ls' failed.\n"
                  "Passed hash sum does not match to calculated hash: %s\n",
                  opts->f_wname,
                  hash_str);
          
          free(hash_str);
          
          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }
          
          return ERRCODE_MHL_CHECK_HASH_FAILED;
      }
      
      free(hash_str);
      
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
          print_minor_separator(stderr);
          printf("Finished checking XX hash for the file '%ls'\n"
                 "   with the size of %llu MB (%llu bytes)\n",
                 opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
          print_minor_separator(stderr);
          printf("Summary: SUCCEEDED\n");
      }
  }
  else if (opts->opt_xxhash64)
  {
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
          print_minor_separator(stderr);
          printf("Started checking XX64 hash for the file '%ls'\n"
                 "   with the size of %llu MB (%llu bytes)\n",
                 opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
          print_minor_separator(stderr);
      }

      // Note: size of passed hash is checed on argument parsing stage

      res =
      wcalculate_xx64_hash_string(opts->f_wname, &hash_str, &hash_str_sz,
                                &total_bytes, &opts->logging_data);

      if (res != 0)
      {
          fprintf(
                  stderr,
                  "Cannot calculate XX64 hash for the file: '%ls'\n"
                  "Description: %s\n",
                  opts->f_wname,
                  mhl_error_code_description(res));

          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }

          return res;
      }

      if (passed_hash_str_sz != hash_str_sz)
      {
          free(hash_str);

          fprintf(
                  stderr,
                  "Check XX64 hash for file '%ls' failed.\n"
                  "Size of passed hash sum does not equal to "
                  "size of calculated hash. (%lu != %lu)\n",
                  opts->f_wname,
                  (unsigned long)passed_hash_str_sz,
                  (unsigned long)hash_str_sz);

          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX64 hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }

          return ERRCODE_MHL_CHECK_HASH_FAILED;
      }

      if (mhlosi_strncasecmp(hash_str, opts->hash_val, hash_str_sz) != 0)
      {
          fprintf(
                  stderr,
                  "Check XX64 hash for file '%ls' failed.\n"
                  "Passed hash sum does not match to calculated hash: %s\n",
                  opts->f_wname,
                  hash_str);

          free(hash_str);

          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX64 hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }

          return ERRCODE_MHL_CHECK_HASH_FAILED;
      }

      free(hash_str);

      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
          print_minor_separator(stderr);
          printf("Finished checking XX64 hash for the file '%ls'\n"
                 "   with the size of %llu MB (%llu bytes)\n",
                 opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
          print_minor_separator(stderr);
          printf("Summary: SUCCEEDED\n");
      }
  }
  else if (opts->opt_xxhash64be)
  {
      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
          print_minor_separator(stderr);
          printf("Started checking XX64BE hash for the file '%ls'\n"
                 "   with the size of %llu MB (%llu bytes)\n",
                 opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
          print_minor_separator(stderr);
      }

      // Note: size of passed hash is checed on argument parsing stage

      res =
      wcalculate_xx64be_hash_string(opts->f_wname, &hash_str, &hash_str_sz,
                                  &total_bytes, &opts->logging_data);

      if (res != 0)
      {
          fprintf(
                  stderr,
                  "Cannot calculate XX64BE hash for the file: '%ls'\n"
                  "Description: %s\n",
                  opts->f_wname,
                  mhl_error_code_description(res));

          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX64BE hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }

          return res;
      }

      if (passed_hash_str_sz != hash_str_sz)
      {
          free(hash_str);

          fprintf(
                  stderr,
                  "Check XX64BE hash for file '%ls' failed.\n"
                  "Size of passed hash sum does not equal to "
                  "size of calculated hash. (%lu != %lu)\n",
                  opts->f_wname,
                  (unsigned long)passed_hash_str_sz,
                  (unsigned long)hash_str_sz);

          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX64BE hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }

          return ERRCODE_MHL_CHECK_HASH_FAILED;
      }

      if (mhlosi_strncasecmp(hash_str, opts->hash_val, hash_str_sz) != 0)
      {
          fprintf(
                  stderr,
                  "Check XX64BE hash for file '%ls' failed.\n"
                  "Passed hash sum does not match to calculated hash: %s\n",
                  opts->f_wname,
                  hash_str);

          free(hash_str);

          if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
          {
              print_minor_separator(stderr);
              printf("Finished checking XX64BE hash for the file '%ls'\n"
                     "   with the size of %llu MB (%llu bytes)\n",
                     opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
              print_minor_separator(stderr);
              printf("Summary: FAILED\n");
          }

          return ERRCODE_MHL_CHECK_HASH_FAILED;
      }

      free(hash_str);

      if (opts->logging_data.v_data.verbose_level >= VL_VERBOSE)
      {
          print_minor_separator(stderr);
          printf("Finished checking XX64BE hash for the file '%ls'\n"
                 "   with the size of %llu MB (%llu bytes)\n",
                 opts->f_wname, total_sz_mb, opts->logging_data.progress_data.total_sz);
          print_minor_separator(stderr);
          printf("Summary: SUCCEEDED\n");
      }
  }
    
  return 0;
}

int run_file_verify(int argc, const char* argv[],
  st_conversion_settings* p_cs)
{
  st_file_check_options opts;
  int res;

  memset((void*) &opts, 0, sizeof(opts) / sizeof(char));

  res = parse_file_check_params(argc, argv, &opts, p_cs);
  if (res != 0)
  {
    free(opts.f_wname);
    mhlhash_usage();
    return res;
  }

  res = run_check_hash(&opts);
  free(opts.f_wname);

  return res;
}