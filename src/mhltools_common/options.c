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
#include <stdlib.h>
#include <string.h>
#include <facade_info/error_codes.h>
#include <generics/os_check.h>

#include "options.h"

// Returns: one of _en_opts
en_opts
recognise_option(const char* option_nm)
{
  if (option_nm == NULL)
  {
    return NULL_OPT;
  }
  if (strcmp(option_nm, "-e") == 0 || strcmp(option_nm, "--existence") == 0)
  {
    return OPT_E;
  }
  else if (strcmp(option_nm, "-f") == 0 || strcmp(option_nm, "--file") == 0)
  {
    return OPT_F;
  }
  else if (strcmp(option_nm, "-v") == 0 || strcmp(option_nm, "--verbose") == 0)
  {
    return OPT_V;
  }
  else if (strcmp(option_nm, "-vv") == 0 || strcmp(option_nm, "--very-verbose") == 0)
  {
    return OPT_VV;
  }
  else if (strcmp(option_nm, "-o") == 0 || strcmp(option_nm, "--output-folder") == 0)
  {
    return OPT_O;
  }
  else if (strcmp(option_nm, "-h") == 0 || strcmp(option_nm, "--hash-value") == 0)
  {
    return OPT_H;
  }
  else if (strcmp(option_nm, "-s") == 0 || strcmp(option_nm, "--stdin") == 0)
  {
    return OPT_S;
  }
  else if (strcmp(option_nm, "-#") == 0 || strcmp(option_nm, "--file-sequence") == 0)
  {
    return OPT_SEQ;
  }
  else if (strcmp(option_nm, "-t") == 0 || strcmp(option_nm, "--types") == 0)
  {
    return OPT_T;
  }
  else if (strcmp(option_nm, "md5") == 0 || strcmp(option_nm, "MD5") == 0)
  {
    return OPT_MD5;
  }
  else if (strcmp(option_nm, "sha1") == 0 || strcmp(option_nm, "SHA1") == 0)
  {
    return OPT_SHA1;
  }
  else if (strcmp(option_nm, "xxhash") == 0 || strcmp(option_nm, "xxHash") == 0 || strcmp(option_nm, "XXHASH") == 0)
  {
      return OPT_XXHASH;
  }
  else if (strcmp(option_nm, "xxhash64") == 0 || strcmp(option_nm, "xxHash64") == 0 || strcmp(option_nm, "XXHASH64") == 0)
  {
      return OPT_XXHASH64;
  }
  else if (strcmp(option_nm, "xxhash64be") == 0 || strcmp(option_nm, "xxHash64BE") == 0 || strcmp(option_nm, "XXHASH64BE") == 0)
  {
      return OPT_XXHASH64BE;
  }
  else if (strcmp(option_nm, "-y") == 0)
  {
    return OPT_Y;
  }
  else if (strcmp(option_nm, "-m") == 0 || strcmp(option_nm, "--mhlformat") == 0)
  {
    return OPT_M;
  }
  else if (strcmp(option_nm, "-c") == 0 || strcmp(option_nm, "--continue") == 0)
  {
    return OPT_C;
  }
#ifdef WIN
  else if (strcmp(option_nm, "/?") == 0)
  {
    return OPT_HELP;
  }
#else
  else if (strcmp(option_nm, "--help") == 0)
  {
    return OPT_HELP;
  }
#endif
  else if (strcmp(option_nm, "--version") == 0)
  {
    return OPT_VER;
  }
  
  return NOT_OPT;
}

