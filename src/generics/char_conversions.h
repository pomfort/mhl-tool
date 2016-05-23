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

#ifndef _MHL_TOOLS_GENERICS_CHAR_CONVERSIONS_H_
#define _MHL_TOOLS_GENERICS_CHAR_CONVERSIONS_H_

#include <wchar.h> 
#include <iconv.h>
#include <generics/os_check.h>

#define MAX_UTF8_CHAR_SZ 5

typedef struct _st_conversion_settings
{
  iconv_t iconv_utf8_to_wchar;
  iconv_t iconv_wchar_to_utf8;
#ifdef MAC_OS_X
  iconv_t iconv_utf8d_to_utf8c;
#endif
} st_conversion_settings;

int init_st_conversion_settings(st_conversion_settings* p_cs);
void free_st_conversion_settings(st_conversion_settings* p_cs);

//
//
//
char* mhlosi_setlocale();

/* Convert from locale encoding to wchar
 *
 * NOTE: If function successfull (returns 0)
 *       Caller is responsible for free *wdst buffer.
 *       Even if *wdst == 0
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int
convert_from_locale_to_wchar(
  const char* src, 
  size_t src_sz, 
  wchar_t** wdst, 
  size_t* wdst_sz,
  st_conversion_settings* p_cs);

#ifdef MAC_OS_X
int 
convert_composed_from_locale_to_wchar(
                                      const char* src, 
                                      size_t src_sz, 
                                      wchar_t** wdst, 
                                      size_t* wdst_sz,
                                      st_conversion_settings* p_cs);

#else
#define convert_composed_from_locale_to_wchar convert_from_locale_to_wchar
#endif

/* Convert from locale encoding to UTF8
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int 
convert_from_locale_to_utf8(
  const char* src, 
  size_t src_sz,
  char** u8_dst,
  size_t* u8_dst_sz,
  st_conversion_settings* p_cs);

/* Convert from UTF8 encoding to wchar
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int
convert_from_utf8_to_wchar(
  const char* src, 
  size_t src_sz, 
  wchar_t** wdst, 
  size_t* wdst_chars_sz,
  st_conversion_settings* p_convs);

/* Convert from wchar to UTF8
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int 
convert_from_wchar_to_utf8(
  const wchar_t* p_wsrc, 
  size_t wsrc_chars_sz,
  char** p_u8_dst,
  size_t* p_u8_dst_sz,
  st_conversion_settings* p_convs);

/* Convert UTF8 string to lower in place
 *
 * NOTE: *pp_src must be allocated.
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int 
tolower_in_place_utf8(
  char** pp_src,
  size_t* p_src_sz,
  st_conversion_settings* p_convs);

/* Convert wchar_t string to lower in place
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int 
tolower_in_place_wchar(
  wchar_t* pp_src,
  size_t src_chars_sz);

/* 
 * 
 * NOTE: Caller is fesponsible for free returned pointer when needed
 * @returns: In case of success: new string in UTF8 encoding.
 *           In case ogf error: 0, and error code in res parameret
 */
char* 
strdup_and_convert_from_locale_to_utf8(
  const char* src, 
  st_conversion_settings* p_convs, 
  int* res);

/* 
 * 
 * NOTE: Caller is fesponsible for free returned pointer when needed
 * @returns: In case of success: new string in wchar_t encoding.
 *           In case of error: 0, and error code in res parameter
 */
wchar_t* 
strdup_and_convert_from_locale_to_wchar(
  const char* src, 
  st_conversion_settings* p_convs, 
  int* res);

#ifdef MAC_OS_X
wchar_t* 
strdup_and_convert_composed_from_locale_to_wchar(
                                        const char* src, 
                                        st_conversion_settings* p_convs, 
                                        int* res);

#else
#define strdup_and_convert_composed_from_locale_to_wchar strdup_and_convert_from_locale_to_wchar
#endif

char* 
strdup_and_convert_from_wchar_to_locale(
  const wchar_t* src, 
  st_conversion_settings* p_convs, 
  int* res);

#endif //_MHL_TOOLS_GENERICS_CHAR_CONVERSIONS_H_
