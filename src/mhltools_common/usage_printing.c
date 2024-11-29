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

#include <facade_info/version.h>
#include <facade_info/error_codes.h>
#include "usage_printing.h"

void print_version()
{
  printf("Version: %s (Build: %d)\n", VERSION, BUILD_NUM);
}

void mhlfile_usage()
{
  printf("Usage: \n"
         "mhl file [-v | -vv] "/*[-y]*/" -s [-o <path>]...\n"
         "mhl file [-v | -vv] "/*[-y]*/" -f FILE [-o <path>]...\n\n");
//         "mhl file [-v | -vv] "/*[-y]*/" -p MHL_FILE\n"
}

void mhlhash_usage()
{
  printf("Usage: \n"
         "mhl hash [-v | -vv] "/*[-y]*/" -f FILE -h [md5|sha1|xxhash|xxhash64] HASH\n"
         "mhl hash [-v | -vv] "/*[-y]*/" [-m] [-#] [-t] [md5|sha1|xxhash|xxhash64] FILEPATTERNS...\n\n");
}

void mhlseal_usage()
{
  printf("Usage: \n"
         "mhl seal [-v | -vv] "/*[-y] [-m] */"[-#] [-t] [md5|sha1|xxhash|xxhash64] [-o <path>]... FILEPATTERNS... \n\n");
}

void mhlverify_usage()
{
  printf("Usage: \n"
         "mhl verify [-v | -vv] "/*[-y]*/" [-e] [-f MHL_FILE[.mhl|.md5|.sha1]] [FILE...]\n\n");
}

void mhl_usage()
{
  printf("Usage: \n\n"
    "1. mhl [--version] <command> [<args>]\n"
    "The available commands are:\n"
    "   seal - Seal folders and files\n"
    "   verify - Verify folders and MHL files\n"
/*    "   hash - Create and verify hashes (via stdin/stdout)\n" */
/*    "   file - Create and parse MHL files (via stdin/stdout)\n\n" */
    "\n"
    "2. See 'mhl help <command>|<topic>' for more information on a specific\n"
    "   command or a help topic.\n"
    "The available help topics are:\n"
/*    "   hash_syntax - Syntax of hash identifiers\n" */
//    "   sequence_syntax - Syntax of file sequence paths\n"
    "   mhl_files - Format, naming and location of MHL files\n"
//    "   machine_output - Format of the machine-readable output\n"
    "   exit_codes - List of exit codes for all commands\n"
//    "   ignore_patterns - Syntax of the MHL ignore patterns\n"
//    "   config_file - Syntax and options of the config file\n"
    "\n");
}
