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

#include <mhl_verify/verify_options.h>

int
init_mhl_verify_options(st_mhl_verify_options* p_mvo)
{
  if (p_mvo == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  memset(p_mvo, 0, sizeof(*p_mvo) / sizeof(char));
  return 0;
}

void
fini_mhl_verify_options(st_mhl_verify_options* p_mvo)
{
  if (p_mvo == 0)
  {
    return;
  }
  
  free(p_mvo->f_wmhl);
  memset(p_mvo, 0, sizeof(*p_mvo) / sizeof(char));
}
