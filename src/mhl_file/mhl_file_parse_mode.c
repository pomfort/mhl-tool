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

#include <facade_info/error_codes.h>
#include <generics/char_conversions.h>
#include <mhltools_common/logging.h>

#include <mhl_file/mhl_file_parse_mode.h>

/*
    else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--parse") == 0)
    {
      if ((data->mode != MD_NOT_SET) && (data->mode != MD_3_FILE_PARSE))
      {
        print_error(
          "Arguments error: "
          "The '-p' option can be used only in parsing MHL file mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      data->mode = MD_3_FILE_PARSE;
      ++i;
      if (i == argc)
      {
        print_error(
          "Arguments error: "
          "A file name must follow the '-p' option.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      data->filename = argv[i];
      ++i;
    }
*/

int run_mhl_file_parse_mode(int argc, const char* argv[],
  st_conversion_settings* p_cs)
{
  print_error("MHL file parsing mode is not implemented yet.\n");
  return ERRCODE_NOT_IMPLEMENTED;
}