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
#include <mhl_file/input_parse_mode.h>
#include <mhl_file/mhl_file_parse_mode.h>
#include <mhl_file/mhl_file.h>

typedef enum _en_file_mode
{
  MD_NOT_SET = 0,
  MD_INPUT_PARSE = 1,
  MD_MHL_FILE_PARSE  = 2
} en_file_mode;

static
void define_file_working_mode(int argc, const char * argv[], en_file_mode* p_file_mode)
{
  int i = 1;

  *p_file_mode = MD_NOT_SET;

  while ((i < argc) && (*p_file_mode == MD_NOT_SET))
  {
    if ( (strcmp(argv[i], "-s") == 0) || (strcmp(argv[i], "--stdin") == 0) ||
         (strcmp(argv[i], "-f") == 0) || (strcmp(argv[i], "--file") == 0) )
    {
      *p_file_mode = MD_INPUT_PARSE;
    }
    else if ( (strcmp(argv[i], "-p") == 0) || (strcmp(argv[i], "--parse") == 0) )
    {
      *p_file_mode = MD_MHL_FILE_PARSE;
    }
    ++i;
  }
}

int run_mhl_file(int argc, const char* argv[])
{
  int res;
  en_file_mode file_mode = MD_NOT_SET;
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
 
  define_file_working_mode(argc, argv, &file_mode);
  if (file_mode == MD_NOT_SET)
  {
    print_error("Failed to define 'mhl file' working mode\n");
    mhlfile_usage();
    free_st_conversion_settings(&css);
    return ERRCODE_WRONG_ARGUMENTS;
  }

  if (file_mode == MD_INPUT_PARSE)
  {
    res = run_input_parse_mode(argc, argv, &css); 
  }
  else if (file_mode == MD_MHL_FILE_PARSE)
  {
    res = run_mhl_file_parse_mode(argc, argv, &css);
  }
  free_st_conversion_settings(&css);

  return res;
}