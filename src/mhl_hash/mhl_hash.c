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
#include <mhltools_common/usage_printing.h>
#include <mhltools_common/logging.h>
#include <mhltools_common/options.h>
#include <mhl_hash/hash_calculate.h>
#include <mhl_hash/file_verify.h>
#include <mhl_hash/input_verify.h>
#include <mhl_hash/mhl_hash.h>

typedef enum _en_hash_mode
{
  MD_NOT_SET = 0,
  MD_CALCULATE,
  MD_FILE_VERIFY,
  MD_INPUT_VERIFY
} en_hash_mode;

// --------------------------------------------------------------------------

static
void define_hash_working_mode(int argc, const char * argv[],
                              en_hash_mode* p_hash_mode)
{
  int i=1;
  en_opts opt;

  *p_hash_mode = MD_NOT_SET;

  do
  {
    opt = recognise_option(argv[i]);
    if (opt == OPT_H)
    {
      *p_hash_mode = MD_FILE_VERIFY;
    }
    else if (opt == OPT_S)
    {
      *p_hash_mode = MD_INPUT_VERIFY;
    }

    ++i;
  } while (*p_hash_mode == MD_NOT_SET && i < argc);
  
  if (*p_hash_mode == MD_NOT_SET)
  {
      *p_hash_mode = MD_CALCULATE;
  }
}

int run_mhl_hash(int argc, const char* argv[])
{
  int res;
  en_hash_mode hash_mode = MD_NOT_SET;
  st_conversion_settings css;

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
 
  define_hash_working_mode(argc, argv, &hash_mode);
  if (hash_mode == MD_NOT_SET)
  {
    print_error("Failed to define 'mhl hash' working mode\n");
    mhlhash_usage();
    free_st_conversion_settings(&css);
    return ERRCODE_WRONG_ARGUMENTS;
  }

  if (hash_mode == MD_CALCULATE)
  {
    res = run_calculate_hash(argc, argv, &css); 
  }
  else if (hash_mode == MD_FILE_VERIFY)
  {
    res = run_file_verify(argc, argv, &css); 
  }
  else if (hash_mode == MD_INPUT_VERIFY)
  {
    res = run_input_verify(argc, argv, &css); 
  }

  free_st_conversion_settings(&css);

  return res;
}