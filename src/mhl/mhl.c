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
#include <mhl_verify/mhl_verify.h>
#include <mhl_file/mhl_file.h>
#include <mhl_hash/mhl_hash.h>
#include <mhl_seal/mhl_seal.h>
#include <mhl_help/mhl_help.h>

typedef enum _en_main_mode
{
  MD_NOT_SET = 0,
  MD_SEAL,
  MD_VERIFY,
  MD_HASH,
  MD_FILE,
  MD_HELP
} en_main_mode;

typedef struct _st_subcommand
{
  en_main_mode mode;
  unsigned char print_version; // 1 - to print, 0 - to skip
  size_t next_idx;
} st_subcommand;

int parse_main_params(int argc, const char* argv[],
  st_subcommand* subcommand)
{
  int i;
  int res = 0;

  subcommand->mode = MD_NOT_SET;

  if (argc < 2)
  {
    print_error(
      "Arguments error: "
      "Incorrect number of arguments.\n");
    return ERRCODE_WRONG_ARGUMENTS;
  }

  i = 1;
  if ( strcmp(argv[i], "help") == 0
       || strcmp(argv[i], "-h") == 0
#ifdef WIN
       || strcmp(argv[i], "/?") == 0
#endif
     )
  {
    subcommand->mode = MD_HELP;
  }
  else
  {
    if (strcmp(argv[i], "--version") == 0)
    {
      subcommand->print_version = 1;
      ++i;
    }
    
    if (i < argc)
    {
      if (strcmp(argv[i], "seal") == 0)
      {
        subcommand->mode = MD_SEAL;
      }
      else if (strcmp(argv[i], "verify") == 0)
      {
        subcommand->mode = MD_VERIFY;
      }
      else if (strcmp(argv[i], "hash") == 0)
      {
        subcommand->mode = MD_HASH;
      }
      else if (strcmp(argv[i], "file") == 0)
      {
        subcommand->mode = MD_FILE;
      }
      else
      {
        fprintf(stderr,
          "Arguments error: "
          "Unknown argument '%s'.\n",
          argv[i]);
        res = ERRCODE_WRONG_ARGUMENTS;
      }
    }
    
  }

  subcommand->next_idx = i;

  return res;
}

int main(int argc, const char * argv[])
{
  st_subcommand subcommand;
  int res;

#ifndef WIN
  //setting line buffered mode for stdout to enable better parsing from Mac OS X GUI application
  setvbuf(stdout, NULL, _IOLBF, 0);
#endif
    
  memset((void*) &subcommand, 0, sizeof(st_subcommand) / sizeof(char));
  mhlosi_setlocale();

  res = parse_main_params(argc, argv, &subcommand);
  if (res != 0)
  {
    mhl_usage();
    return res;
  }

  if (subcommand.print_version)
  {
    print_version();
  }

  if (subcommand.mode == MD_SEAL)
  {
    res = run_mhl_seal(argc - subcommand.next_idx, argv + subcommand.next_idx);
  }
  else if (subcommand.mode == MD_VERIFY)
  {
    res = run_mhl_verify(argc - subcommand.next_idx, argv + subcommand.next_idx);
  }
  else if (subcommand.mode == MD_HASH)
  {
    res = run_mhl_hash(argc - subcommand.next_idx, argv + subcommand.next_idx);
  }
  else if (subcommand.mode == MD_FILE)
  {
    res = run_mhl_file(argc - subcommand.next_idx, argv + subcommand.next_idx);
  }
  else if (subcommand.mode == MD_HELP)
  {
    res = run_mhl_help(argc - subcommand.next_idx, argv + subcommand.next_idx);
  }
  else if (!subcommand.print_version) // subcommand.mode == MD_NOT_SET
  {
    // Really we should not come here after parameters parsing
    print_error("Unknown program running mode\n");
    mhl_usage();
    res = ERRCODE_UNKNOWN_MODE;
  }

  return res;
}
