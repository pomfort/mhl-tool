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
#include <mhltools_common/usage_printing.h>
#include <mhltools_common/logging.h>

#include <mhl_help/help_topics.h>
#include <mhl_help/mhl_help.h>

int run_mhl_help(int argc, const char* argv[])
{
  int i;
  int res = 0;

  if (argc < 1 || argc > 2)
  {
    print_error(
      "Arguments error: "
      "Incorrect number of arguments.\n");
    mhl_usage();
    return ERRCODE_WRONG_ARGUMENTS;
  }

  if (argc == 1)
  {
    mhl_usage();
    return 0;
  }
  
  i = 1;
  // commands help
  if (strcmp(argv[i], "seal") == 0)
  {
    res = print_mhlseal_help();
  }
  else if (strcmp(argv[i], "verify") == 0)
  {
    res = print_mhlverify_help();
  }
  else if (strcmp(argv[i], "hash") == 0)
  {
    res = print_mhlhash_help();
  }
  else if (strcmp(argv[i], "file") == 0)
  {
    res = print_mhlfile_help();
  }

  // other help topics
  else if (strcmp(argv[i], "hash_syntax") == 0)
  {
    res = print_hash_syntax_help();
  }
  else if (strcmp(argv[i], "sequence_syntax") == 0)
  {
    res = print_sequence_syntax_help();
  }
  else if (strcmp(argv[i], "mhl_files") == 0)
  {
    res = print_mhl_files_help();
  }
  else if (strcmp(argv[i], "machine_output") == 0)
  {
    res = print_machine_output_help();
  }
  else if (strcmp(argv[i], "exit_codes") == 0)
  {
    res = print_exit_codes_help();
  }
  else if (strcmp(argv[i], "ignore_patterns") == 0)
  {
    res = print_ignore_patterns_help();
  }
  else if (strcmp(argv[i], "config_file") == 0)
  {
    res = print_config_file_help();
  }
  else
  {
    print_error("Unknown help topic is requested\n");
    mhl_usage();
    return ERRCODE_WRONG_ARGUMENTS;
  }

  if (res < 0) {
    print_error("Unknown error occured while printing a help topic\n");
    res = ERRCODE_UNKNOWN_ERROR;
  }
  else
  {
    res = 0;
  }

  return res;
}
