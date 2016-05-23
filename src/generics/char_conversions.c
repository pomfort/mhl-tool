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
#include <stdlib.h>
#include <stdio.h>
#include <wctype.h>
#include <wchar.h> 
#include <string.h>
 
#include <facade_info/error_codes.h>

#include <generics/os_check.h>

#ifdef WIN
#include "Windows.h"
#else 
#include <locale.h>
#endif

#include <generics/std_funcs_os_anonymizer.h>
#include <generics/char_conversions.h>

#ifdef WIN
//
// Windows variant
//

char* mhlosi_setlocale()
{
  // Do nothing here on windows
  return "";
}

/* Convert from locale encoding to wchar
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
  st_conversion_settings* p_cs)
{
  int res;
  UINT cp;  
  
  if (src == 0 || wdst == 0 || wdst_sz == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  // Get current encoding
  cp = GetACP();
    
  // Get required size of WCHAR buffer
  *wdst_sz = MultiByteToWideChar(cp, MB_PRECOMPOSED, src, -1, NULL, 0);
  if (*wdst_sz == 0)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
    
  *wdst = (wchar_t*) malloc((*wdst_sz + 1) * sizeof(wchar_t));
  if (*wdst == 0)
  {
    *wdst_sz = 0;
    return ERRCODE_OUT_OF_MEM;
  }
    
  // Convert from local encoding to WCHAR
  res = MultiByteToWideChar(cp, MB_PRECOMPOSED, src, -1, *wdst, *wdst_sz);
  if (res == 0)
  {
    free(*wdst);
    *wdst = 0;
    *wdst_sz = 0;

    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
   
  // remove trailing szero
  (*wdst_sz)--;
  return 0;
}

static
int convert_from_wchar_to_locale(
  const wchar_t* wsrc, 
  size_t wsrc_sz, 
  char** dst, 
  size_t* dst_sz,
  st_conversion_settings* p_cs)
{
  int res;
  UINT cp;
  BOOL used_default;
  
  if (wsrc == 0 || dst == 0 || dst_sz == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  // Get current encoding
  cp = GetACP();
    
  // Get required size of buffer
  *dst_sz = WideCharToMultiByte(cp, WC_COMPOSITECHECK|WC_SEPCHARS, wsrc, -1, NULL, 0, NULL, &used_default);
  if (*dst_sz == 0)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
    
  *dst = (char*) malloc((*dst_sz + 1) * sizeof(char));
  if (*dst == 0)
  {
    *dst_sz = 0;
    return ERRCODE_OUT_OF_MEM;
  }
    
  // Convert from WCHAR to local encoding
  res = WideCharToMultiByte(cp, WC_COMPOSITECHECK|WC_SEPCHARS, wsrc, -1, *dst, *dst_sz, NULL, &used_default);
  if (res == 0)
  {
    free(*dst);
    *dst = 0;
    *dst_sz = 0;
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
   
  // remove trailing szero
  (*dst_sz)--;
  return 0;
}

#else
//
// Linux and MAC_OS_X variant
//

char* mhlosi_setlocale()
{
  return setlocale(LC_CTYPE, "");  
}

int
convert_from_locale_to_wchar(
  const char* p_src, 
  size_t src_sz, 
  wchar_t** wdst, 
  size_t* wdst_sz,
  st_conversion_settings* p_css)
{
  size_t dest_sz;
  //size_t conversion_sz;
  mbstate_t p_cs;
  const char** pp_src;

  if (p_src == 0 || wdst == 0 || wdst_sz == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  if (p_src[0] == '\0')
  {
    *wdst = (wchar_t*) calloc(1, sizeof(wchar_t));
    if (*wdst == 0)
    {
      return ERRCODE_OUT_OF_MEM;
    }

    *wdst_sz = 0;
    *wdst = L'\0';
    return 0;
  }

  //
  //
  //
  memset(&p_cs, 0, sizeof(p_cs) / sizeof(char));
  pp_src = &p_src;
  dest_sz = mbsrtowcs(0, pp_src, 0, &p_cs);
  if (dest_sz == (size_t) -1)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
    
  *wdst = calloc(dest_sz + 1, sizeof(wchar_t));
  if (*wdst == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  pp_src = &p_src;
  *wdst_sz = mbsrtowcs(*wdst, pp_src, dest_sz, &p_cs);
  if (*wdst_sz == (size_t) -1)
  {
    free(*wdst);

    *wdst_sz = 0;
    *wdst = 0;
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }

  (*wdst)[*wdst_sz] = L'\0';
  return 0;
}

int
convert_from_wchar_to_locale(
  const wchar_t* p_wsrc, 
  size_t wsrc_sz, 
  char** dst, 
  size_t* dst_sz,
  st_conversion_settings* p_css)
{
  size_t dest_sz;
  //size_t conversion_sz;
  mbstate_t p_cs;
  const wchar_t** pp_wsrc;

  if (p_wsrc == 0 || dst == 0 || dst_sz == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  if (p_wsrc[0] == L'\0')
  {
    *dst = (char*) calloc(1, sizeof(char));
    if (*dst == 0)
    {
      return ERRCODE_OUT_OF_MEM;
    }

    *dst_sz = 0;
    *dst = '\0';
    return 0;
  }

  //
  //
  //
  memset(&p_cs, 0, sizeof(p_cs) / sizeof(char));
  pp_wsrc = &p_wsrc;
  dest_sz = wcsrtombs(0, pp_wsrc, 0, &p_cs);
  if (dest_sz == (size_t) -1)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
    
  *dst = calloc(dest_sz + 1, sizeof(char));
  if (*dst == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  pp_wsrc = &p_wsrc;
  *dst_sz = wcsrtombs(*dst, pp_wsrc, dest_sz, &p_cs);
  if (*dst_sz == (size_t) -1)
  {
    free(*dst);

    *dst_sz = 0;
    *dst = 0;
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }

  (*dst)[*dst_sz] = '\0';
  return 0;
}

#endif

#ifdef MAC_OS_X
int
convert_composed_from_locale_to_wchar(
                             const char* p_src, 
                             size_t src_sz, 
                             wchar_t** wdst, 
                             size_t* wdst_sz,
                             st_conversion_settings* p_css)
{
  char* u8_dst = 0;
  size_t u8_dst_sz;
  int res = 0;
  
  *wdst = L'\0';
  *wdst_sz = 0;
  
  res = 
    convert_from_locale_to_utf8(p_src, src_sz, &u8_dst, &u8_dst_sz, p_css); 
  
  if (res != 0 )
  {
    free(u8_dst);
    return res;
  }
  
  res = 
    convert_from_utf8_to_wchar(u8_dst, u8_dst_sz, wdst, wdst_sz, p_css);  
  
  free(u8_dst);
  if (res != 0 )
  {
    free(*wdst);
    *wdst = L'\0';
    *wdst_sz = 0;
  }
  
  return res;
}
#endif

int 
init_st_conversion_settings(st_conversion_settings* p_cs)
{
  const char* UTF8_ENCODING = "UTF-8";
#ifdef WIN
  const char* WCHAR_ENCODING = "UCS-2-INTERNAL";
#else // MAC_OS_X and LINUX
  const char* WCHAR_ENCODING = "UCS-4LE";
#endif

  // We need sometimes additional extra conversions into UTF-8 at Mac OS X,
  // because HFS+ uses and returns always filenames in de-composed version 
  // of UTF-8 encoding for symbols like umlauts, while other systems and 
  // programs if support UTF-8 usually use it's composed version.
#ifdef MAC_OS_X
  const char* UTF8_MAC = "UTF-8-MAC";
#endif
  
  if (p_cs == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  p_cs->iconv_utf8_to_wchar = (iconv_t) -1;
  p_cs->iconv_wchar_to_utf8 = (iconv_t) -1;
  
  p_cs->iconv_utf8_to_wchar = iconv_open(WCHAR_ENCODING, UTF8_ENCODING);  
  if (p_cs->iconv_utf8_to_wchar == (iconv_t) -1)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }

  p_cs->iconv_wchar_to_utf8 = iconv_open(UTF8_ENCODING, WCHAR_ENCODING);
  if (p_cs->iconv_wchar_to_utf8 == (iconv_t) -1)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }

#ifdef MAC_OS_X  
  p_cs->iconv_utf8d_to_utf8c = (iconv_t) -1;
  
  p_cs->iconv_utf8d_to_utf8c = iconv_open(UTF8_ENCODING, UTF8_MAC);  
  if (p_cs->iconv_utf8d_to_utf8c == (iconv_t) -1)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
#endif  
  
  return 0;
}

void free_st_conversion_settings(st_conversion_settings* p_cs)
{
  if (p_cs == 0)
  {
    return;
  }
  
  if (p_cs->iconv_utf8_to_wchar != (iconv_t) -1)
  {
    iconv_close(p_cs->iconv_utf8_to_wchar);
  }

  if (p_cs->iconv_wchar_to_utf8 == (iconv_t) -1)
  {
    iconv_close(p_cs->iconv_wchar_to_utf8);
  }
  
#ifdef MAC_OS_X
  if (p_cs->iconv_utf8d_to_utf8c == (iconv_t) -1)
  {
    iconv_close(p_cs->iconv_utf8d_to_utf8c);
  }
#endif
}

//
//
//
/*
int 
aux_convert_from_wchar_to_uint32_t(
  const wchar_t* p_wsrc,
  size_t wsrc_chars_sz,
  unsigned long** p_u32dst,
  size_t* p_u32dst_chars_sz)
{
  size_t i;

  if (p_wsrc == 0 || p_u32dst == 0 || p_u32dst_chars_sz == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  *p_u32dst = (unsigned long*) calloc(wsrc_chars_sz + 1, sizeof(unsigned long));
  if (*p_u32dst == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  for (i = 0; i < wsrc_chars_sz; ++i)
  {
    (*p_u32dst)[i] = (unsigned long) p_wsrc[i];
  }

  (*p_u32dst)[wsrc_chars_sz] = 0;
  *p_u32dst_chars_sz = wsrc_chars_sz;
  return 0;
}
*/

int
convert_from_utf8_to_wchar(
  const char* p_src, 
  size_t src_sz, 
  wchar_t** p_wdst, 
  size_t* wdst_sz,
  st_conversion_settings* p_convs)
{
  size_t res;
  size_t tmp_src_sz;
  size_t dst_sz;
  wchar_t* p_wdst_init;
  size_t dst_sz_init;
  char** pp_src;

  if (p_src == 0 || p_wdst == 0 || wdst_sz == 0 || 
      p_convs->iconv_utf8_to_wchar == (iconv_t) -1)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  // MAC OS adds FEFF extra char
  *p_wdst = (wchar_t*) calloc(src_sz + 2, sizeof(wchar_t));
  if (*p_wdst == NULL)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  dst_sz = (src_sz + 1) * sizeof(wchar_t);

  pp_src = (char**) &p_src;
  p_wdst_init = *p_wdst;
  dst_sz_init = dst_sz;
  tmp_src_sz = src_sz;
  res = 
    iconv(
      p_convs->iconv_utf8_to_wchar, 
      pp_src, 
      &tmp_src_sz, 
      (char**) p_wdst, 
      &dst_sz);

  if (res == (size_t) -1)
  {
    free(p_wdst_init);
    *p_wdst = 0;
    *wdst_sz = 0;

    return ERRCODE_CHARS_CONVERSION_ERROR;
  }

  *wdst_sz = dst_sz_init > dst_sz ? (dst_sz_init - dst_sz) / sizeof(wchar_t) : 0;
  *p_wdst = p_wdst_init;
  // last char is always '\0' (calloc(src_sz + 1, ...)

  return 0;
}

int 
convert_from_wchar_to_utf8(
  const wchar_t* p_wsrc, 
  size_t wsrc_sz,
  char** p_u8_dst,
  size_t* p_u8_dst_sz,
  st_conversion_settings* p_convs)
{
  size_t res;
  size_t src_sz;
  size_t dst_sz;
  size_t dst_sz_init;
  char* p_u8_tmp_dst;
  char* p_u8_tmp_dst_init;
#ifdef MAC_OS_X
  char* p_u8_dst_init;
#endif    
  
  wchar_t** p_wsrc_init = (wchar_t**) &p_wsrc;

  if (p_wsrc == 0 || p_u8_dst == 0 || p_u8_dst_sz == 0 ||
      p_convs->iconv_wchar_to_utf8 == (iconv_t) -1)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
#ifdef MAC_OS_X
  if (p_convs->iconv_utf8d_to_utf8c == (iconv_t) -1)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
#endif    
    
  dst_sz_init = (wsrc_sz + 1) * MAX_UTF8_CHAR_SZ;

  p_u8_tmp_dst = (char*) calloc(dst_sz_init + 1, sizeof(char));
  if (p_u8_tmp_dst == NULL)
  {
    *p_u8_dst = 0;
    *p_u8_dst_sz = 0;
    return ERRCODE_OUT_OF_MEM;
  }  
  
  p_u8_tmp_dst_init = p_u8_tmp_dst;
  dst_sz = dst_sz_init;
  
  src_sz = wsrc_sz * sizeof(wchar_t);
  res = 
    iconv(
        p_convs->iconv_wchar_to_utf8, 
        (char**) p_wsrc_init,
        &src_sz, 
        &p_u8_tmp_dst_init, 
        &dst_sz);
  
  if (res == (size_t) -1)
  {
    free(p_u8_tmp_dst);
    *p_u8_dst = 0;
    *p_u8_dst_sz = 0;
    
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
  dst_sz = dst_sz_init > dst_sz ? dst_sz_init - dst_sz : 0;  
  
#ifdef MAC_OS_X
  
  *p_u8_dst = (char*) calloc(dst_sz_init + 1, sizeof(char));
  if (*p_u8_dst == NULL)
  {
    free(p_u8_tmp_dst);
    *p_u8_dst = 0;
    *p_u8_dst_sz = 0;
    return ERRCODE_OUT_OF_MEM;
  }
  
  p_u8_tmp_dst_init = p_u8_tmp_dst;
  p_u8_dst_init = *p_u8_dst;
  src_sz = dst_sz;  
  dst_sz = dst_sz_init;
  
  res = 
    iconv(
        p_convs->iconv_utf8d_to_utf8c, 
        &p_u8_tmp_dst_init,
        &src_sz, 
        &p_u8_dst_init, 
        &dst_sz);
  
  free(p_u8_tmp_dst);
  
  if (res == (size_t) -1)
  {
    free(*p_u8_dst);
    *p_u8_dst = 0;
    *p_u8_dst_sz = 0;
    
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }
  
  *p_u8_dst_sz = dst_sz_init > dst_sz ? dst_sz_init - dst_sz : 0;
  
#else
  *p_u8_dst_sz = dst_sz;
  *p_u8_dst = p_u8_tmp_dst;
#endif
  
  return 0;
}

int 
convert_from_locale_to_utf8(
  const char* src, 
  size_t src_sz,
  char** p_u8_dst,
  size_t* u8_dst_sz,
  st_conversion_settings* p_cs)
{
  wchar_t* p_wtmp = 0;
  size_t wtmp_sz;
  int res;

  res = convert_from_locale_to_wchar(src, src_sz, &p_wtmp, &wtmp_sz, p_cs);
  if (res != 0 || p_wtmp == 0)
  {
    return res ? res : ERRCODE_CHARS_CONVERSION_ERROR;
  }

  res = 
    convert_from_wchar_to_utf8(
      p_wtmp, 
      wtmp_sz, 
      p_u8_dst, 
      u8_dst_sz, 
      p_cs);

  free(p_wtmp);
  return res;
}

/* Convert UTF8 string to lower in place
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int 
tolower_in_place_utf8(
  char** pp_src,
  size_t* p_src_sz,
  st_conversion_settings* p_convs)
{
  int res;
  wchar_t* wtmp = 0;
  size_t wtmp_sz = 0;
  //size_t new_sz = 0;
  char* p_low_src = 0;
  size_t low_src_sz = 0;

  if (pp_src == 0 || p_src_sz == 0 || p_convs == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  res = 
    convert_from_utf8_to_wchar(*pp_src, *p_src_sz, &wtmp, &wtmp_sz, p_convs);
  if (res != 0)
  {
    return res;
  }

  res = tolower_in_place_wchar(wtmp, wtmp_sz);
  if (res != 0)
  {
    free(wtmp);
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }

  res = 
    convert_from_wchar_to_utf8(
      wtmp, 
      wtmp_sz, 
      &p_low_src, 
      &low_src_sz, 
      p_convs);

  free(wtmp);

  if (res != 0)
  {
    return res;
  }

  // reassign
  free(*pp_src);
  *pp_src = p_low_src;
  *p_src_sz = low_src_sz;

  return 0;
}

/* Convert wchar_t string to lower in place
 *
 * @return 
 *    In case of success: 0
 *    In case of errors : non zero value indicating the error
 */
int 
tolower_in_place_wchar(
  wchar_t* pp_src,
  size_t src_chars_sz)
{
  size_t i;
  wchar_t wc;

  for(i = 0; i < src_chars_sz; ++i)
  {
    wc = (wchar_t) towlower(pp_src[i]);
    pp_src[i] = wc;
  }

  return 0;
}

char* 
strdup_and_convert_from_locale_to_utf8(
  const char* p_src, 
  st_conversion_settings* p_convs, 
  int* p_res)
{
  size_t src_sz = 0;
  char* p_u8_dst = 0;
  size_t u8_dst_sz = 0;

  if (p_src == 0 || p_res == 0)
  {
    if (p_res != 0)
    {
      *p_res = ERRCODE_WRONG_ARGUMENTS;
      return 0;
    }
  }

  src_sz = strlen(p_src);
  *p_res  = 
    convert_from_locale_to_utf8(
      p_src, 
      src_sz, 
      &p_u8_dst , 
      &u8_dst_sz,
      p_convs);

  return *p_res == 0 ? p_u8_dst : 0;
}

/* 
 * 
 * NOTE: Caller is responsible for free returned pointer when needed
 * @returns: In case of success: new string in wchar_t encoding.
 *           In case of error: 0, and error code in p_res parameter
 */
wchar_t* 
strdup_and_convert_from_locale_to_wchar(
  const char* p_src, 
  st_conversion_settings* p_convs, 
  int* p_res)
{
  size_t src_sz = 0;
  wchar_t* p_wdst = 0;
  size_t wdst_sz = 0;

  if (p_src == 0 || p_res == 0)
  {
    if (p_res != 0)
    {
      *p_res = ERRCODE_WRONG_ARGUMENTS;
      return 0;
    }
  }

  src_sz = strlen(p_src);
  *p_res  = 
    convert_from_locale_to_wchar(
      p_src, 
      src_sz, 
      &p_wdst, 
      &wdst_sz,
      p_convs);

  return *p_res == 0 ? p_wdst : 0;

}

#ifdef MAC_OS_X
wchar_t* 
strdup_and_convert_composed_from_locale_to_wchar(
                                        const char* p_src, 
                                        st_conversion_settings* p_convs, 
                                        int* p_res)
{
  size_t src_sz = 0;
  wchar_t* p_wdst = 0;
  size_t wdst_sz = 0;
  
  if (p_src == 0 || p_res == 0)
  {
    if (p_res != 0)
    {
      *p_res = ERRCODE_WRONG_ARGUMENTS;
      return 0;
    }
  }
  
  src_sz = strlen(p_src);
  *p_res  = 
  convert_composed_from_locale_to_wchar(
                               p_src, 
                               src_sz, 
                               &p_wdst, 
                               &wdst_sz,
                               p_convs);
  
  return *p_res == 0 ? p_wdst : 0;
  
}
#endif

char* 
strdup_and_convert_from_wchar_to_locale(
  const wchar_t* p_wsrc, 
  st_conversion_settings* p_convs, 
  int* p_res)
{
  size_t wsrc_sz = 0;
  char* p_dst = 0;
  size_t dst_sz = 0;

  if (p_wsrc == 0 || p_res == 0)
  {
    if (p_res != 0)
    {
      *p_res = ERRCODE_WRONG_ARGUMENTS;
      return 0;
    }
  }

  wsrc_sz = wcslen(p_wsrc);
  *p_res  = 
    convert_from_wchar_to_locale(
      p_wsrc, 
      wsrc_sz, 
      &p_dst, 
      &dst_sz,
      p_convs);

  return *p_res == 0 ? p_dst : 0;
}


/**
 * @return 
 *    In case of success: 
 *     new alloacted buffer with source in wchar_t. 
 *     Caller is responsible for freeing it.
 */
/*
WCHAR* 
convert_from_locale_to_wchar(char* src, size_t src_sz)
{
    int wres;
    UINT cp;  
    WCHAR* wt_src;
    size_t wt_src_sz;
    
    // Get current encoding
    cp = GetACP();
    
    // Get required size of WCHAR buffer
    wt_src_sz = MultiByteToWideChar(cp, MB_PRECOMPOSED, src, -1, NULL, 0);
    if (wt_src_sz == 0)
    {
        return NULL;
    }
    
    wt_src = (WCHAR*) malloc((wt_src_sz + 1) * sizeof(WCHAR));
    if (wt_src == 0)
    {
        return NULL;
    }
    
    // Convert from local encoding to WCHAR
    wres = MultiByteToWideChar(cp, MB_PRECOMPOSED, src, -1, wt_src, wt_src_sz);
    if (wres == 0)
    {
        free(wt_src);
        return NULL;
    }
    
    return wt_src;
}
*/
/**
 * @return 
 *  In case of success: 
 *    new alloacted buffer with source in UTF8. 
 *    Caller is responsible for freeing it.
 *  In case of error: returns NULL.
 *  wt_src_sz is the length of wt_src, not including the 
 *    terminating zero bytes.
 */
/*
char* 
convert_from_wchar_to_utf8(WCHAR* wt_src, size_t wt_src_sz)
{  
    int res;
    size_t utf8_sz;
    char* utf8_str;
    
    // Get required size of buffer for UTF8 chars
    utf8_sz = 
    WideCharToMultiByte(
                        CP_UTF8, 
                        0, 
                        wt_src, 
                        wt_src_sz, 
                        NULL, 
                        0, 
                        NULL, 
                        NULL);
    
    if (utf8_sz == 0)
    {
        return NULL;
    }
    
    // Convert from WCHAR to UTF8
    utf8_str = (char*) calloc(utf8_sz + 1, sizeof(char));
    if (utf8_str == NULL)
    {
        return NULL;
    }
    
    res = 
    WideCharToMultiByte(
                        CP_UTF8, 
                        0, 
                        wt_src, 
                        wt_src_sz, 
                        utf8_str, 
                        utf8_sz, 
                        NULL, 
                        NULL);
    
    if (res == 0)
    {    
        free(utf8_str);
        return NULL;
    }
    
    return utf8_str;
}
*/
/**
 * @return 
 *    In case of success: 
 *     new allocated buffer with source in UTF8. 
 *     Caller is responsible for freeing it.
 *   In case of error: returns NULL.
 */
/*
char* 
convert_from_locale_to_utf8(char* src, size_t src_sz)
{
    WCHAR* wt_src;
    size_t wt_src_sz;
    char* utf8_str;
    
    //
    wt_src = convert_from_locale_to_wchar(src, src_sz);
    if (wt_src == NULL)
    {
        return NULL;
    }
    
    wt_src_sz = wcslen(wt_src);
    
    utf8_str = convert_from_wchar_to_utf8(wt_src, wt_src_sz);
    
    free(wt_src);
    return utf8_str;
}
*/
