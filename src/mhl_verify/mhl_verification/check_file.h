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
#ifndef _MHL_TOOLS_MHL_VERIFY_MHL_VERIFICATION_CHECK_FILE_H_
#define _MHL_TOOLS_MHL_VERIFY_MHL_VERIFICATION_CHECK_FILE_H_

#include <parsemhl/mhl_file_handlers.h>

//---------------------------------------------------------
//
// Set of functions for checking against MHL file content
//
//---------------------------------------------------------

//
// Check is real file "fingerprints" are equal to 
// "fingerprints" from "hash" tag of MHL file.
//
int check_file_against_mhl_wfile_item(
  st_mhl_file_check_wdata* p_mhl_file_witem, 
  unsigned char check_existence,
  st_controlling_data* p_common);


int check_file_against_mhl_wcontent(
  const wchar_t* abs_filename, 
  st_mhl_file_wcontent* p_mhl_file_wcontent, 
  unsigned char check_existence,
  st_controlling_data* p_common);

#endif //_MHL_TOOLS_MHL_VERIFY_MHL_VERIFICATION_CHECK_FILE_H_
