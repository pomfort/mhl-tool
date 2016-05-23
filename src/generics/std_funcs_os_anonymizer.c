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

/*
 * @file: std_funcs_os_anonymizer.c
 * 
 * Definitions of generic functions have different 
 * implementation of different OS.
 *
 */
#include <string.h>
#include <stdlib.h> 
#include <wchar.h>

#include <generics/os_check.h>
#ifdef WIN
#include <ctype.h> 
#include <malloc.h>
#include <sys/types.h>
#elif defined MAC_OS_X 
#include <wctype.h> 
#endif

#include <generics/std_funcs_os_anonymizer.h>

#ifdef WIN
/*
 * OS specific version of "strcasestr". strcasestr is GNU specific function.
 */
char* mhlosi_strcasestr(const char* haystack, const char* needle)
{
  // This implementation is unoptimal.
  // May be it make sense to find more optimal implementation.

  errno_t err;
  char* haystack_lw;
  char* needle_lw;
  char* needle_lw_p;
  off_t needle_distance;
  char* res;

  haystack_lw = _strdup(haystack);
  if (haystack_lw == NULL)
  {
    return NULL;
  }

  needle_lw = _strdup(needle);
  if (needle_lw == NULL)
  {
    free(haystack_lw);
    return NULL;
  }

  //
  err = _strlwr_s(haystack_lw, strlen(haystack_lw));
  if (err != 0)
  {
    free(haystack_lw);
    free(needle_lw);

    return NULL;
  }

  err = _strlwr_s(needle_lw, strlen(needle_lw));
  if (err != 0)
  {
    free(haystack_lw);
    free(needle_lw);

    return NULL;
  }

  //
  needle_lw_p = strstr(haystack_lw, needle_lw);
  if (needle_lw_p == NULL)
  {
    free(haystack_lw);
    free(needle_lw);

    return NULL;
  }

  needle_distance = needle_lw_p - haystack_lw;
  free(haystack_lw);
  free(needle_lw);

  res = (char*) haystack + needle_distance;
  return res;
}

#else
/*
 * OS specific version of "strcasestr". strcasestr is GNU specific function.
 */
char* mhlosi_strcasestr(const char* haystack, const char* needle)
{
  return strcasestr(haystack, needle);
}
#endif

char* mhlosi_strdup(const char* s)
{
#ifdef WIN
  return _strdup(s);
#else
  return strdup(s);
#endif
}

/* 
 * OS specific version of "strcasecmp". 
 */
int mhlosi_strcasecmp(const char* s1, const char* s2)
{
#ifdef WIN
  return _stricmp(s1, s2);
#else
  return strcasecmp(s1, s2);
#endif
}

/* 
 * OS specific version of "strncasecmp".
 */
int mhlosi_strncasecmp(const char* s1, const char* s2, size_t n)
{
#ifdef WIN
  return _strnicmp(s1, s2, n);
#else
  return strncasecmp(s1, s2, n);
#endif
}

/*
 * OS specific version of "strtoull". 
 */
unsigned long long mhlosi_strtoull(const char *nptr, char **endptr, int base)
{
#ifdef WIN
  return (unsigned long long) _strtoui64(nptr, endptr, base);
#else
  return strtoull(nptr, endptr, base);
#endif
}

//
//
//

/* 
 * OS specific version of "strdup".
 * strdup POSIX function is deprecated since Visual C++ 2005. 
 */
wchar_t* mhlosi_wstrdup(const wchar_t* s)
{
#if defined WIN
  return _wcsdup(s);
#elif defined MAC_OS_X
  wchar_t* ws_dup;
  size_t ws_sz = wcslen(s);
  
  ws_dup = (wchar_t*) calloc(ws_sz + 1, sizeof(wchar_t));
  if (ws_dup == NULL)
  {
    return NULL;
  }
  
  wcsncpy(ws_dup, s, ws_sz);
  return ws_dup;
#else
  return wcsdup(s);
#endif
}

/* 
 * strdup POSIX function is deprecated since Visual C++ 2005. 
 */
wchar_t* mhlosi_wstrndup(const wchar_t* s, size_t s_sz)
{
  wchar_t* ws_dup;
  size_t real_s_sz = 0;
  
  real_s_sz = wcslen(s);
  real_s_sz = real_s_sz > s_sz ? s_sz : real_s_sz;
  ws_dup = calloc(real_s_sz + 1, sizeof(wchar_t));
  if (ws_dup == 0)
  {
    return 0; // no memeory
  }
  
  wcsncpy(ws_dup, s, real_s_sz);
  // ending L'\0' already set by calloc 
  
  return ws_dup;
}


/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
int mhlosi_wstrcmp(const wchar_t* s1, const wchar_t* s2)
{
  return wcscmp(s1, s2);
}

/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
int mhlosi_wstrncmp(const wchar_t* s1, const wchar_t* s2, size_t n)
{
  return wcsncmp(s1, s2, n);
}

/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
#ifdef MAC_OS_X
int mhlosi_wstrncasecmp(const wchar_t* ws1, const wchar_t* ws2, size_t wsz) 
{ 
  wchar_t* s1_lower; 
  wchar_t* s2_lower;
  int results; 
  int index; 
 		 
  s1_lower = (wchar_t*)malloc(sizeof(wchar_t) * wcslen(ws1)); 
  s2_lower = (wchar_t*)malloc(sizeof(wchar_t) * wcslen(ws2)); 
 		 
  wcscpy(s1_lower, ws1); 
  wcscpy(s2_lower, ws2); 
  index = 0; 
  while (s1_lower[index] != '\0') 
  { 
    s1_lower[index] = towlower(s1_lower[index]); 
    index++; 
  } 
 		 
 	index = 0; 
 	while (s2_lower[index] != '\0') 
  { 
    s2_lower[index] = towlower(s2_lower[index]); 
    index++; 
  } 
 	 
 	results = wcsncmp(s1_lower, s2_lower, wsz); 
 	free(s1_lower); 
 	free(s2_lower); 
 		 
 	return results; 
}
#else
int mhlosi_wstrncasecmp(const wchar_t* ws1, const wchar_t* ws2, size_t wsz)
{
#ifdef WIN
  return _wcsnicmp(ws1, ws2, wsz);
#else
  return wcsncasecmp(ws1, ws2, wsz);
#endif

}
#endif

/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
#ifdef MAC_OS_X
int mhlosi_wstrcasecmp(const wchar_t* ws1, const wchar_t* ws2) 
{ 
  wchar_t* s1_lower; 
  wchar_t* s2_lower;  		 
  int results; 
  int index; 
  
  s1_lower = (wchar_t*)malloc(sizeof(wchar_t) * wcslen(ws1)); 
  s2_lower = (wchar_t*)malloc(sizeof(wchar_t) * wcslen(ws2)); 
  
  wcscpy(s1_lower, ws1); 
  wcscpy(s2_lower, ws2); 
  index = 0; 
  while (s1_lower[index] != '\0') 
  { 
    s1_lower[index] = towlower(s1_lower[index]); 
    index++; 
  } 
  
 	index = 0; 
 	while (s2_lower[index] != '\0') 
  { 
    s2_lower[index] = towlower(s2_lower[index]); 
    index++; 
  } 
  
 	results = wcscmp(s1_lower, s2_lower); 
 	free(s1_lower); 
 	free(s2_lower); 
  
 	return results; 
}
#else

int mhlosi_wstrcasecmp(const wchar_t* s1, const wchar_t* s2)
{
#ifdef WIN
  return _wcsicmp(s1, s2);
#else
  return wcscasecmp(s1, s2);
#endif
}

#endif
/*
 * OS specific version of "strcasestr". strcasestr is GNU specific function.
 */
wchar_t* mhlosi_wstrstr(const wchar_t* haystack, const wchar_t* needle)
{
  return (wchar_t*) wcsstr(haystack, needle);
}

/*
 * OS specific and safe version of wcscat. 
 * Appends at least n wchars from str to dst.
 * NOTE: buffer with dst string should have enough space for appended n wchars!
 *
 * Returns pointer to destination string. 0 means error.
 */
wchar_t* 
mhlosi_wstrncat(const wchar_t* src, wchar_t* dst, size_t count)
{
  size_t i;
  size_t dst_sz;
  
  if (src == 0 || dst == 0 || count == 0)
  {
    return 0;
  }
  
  dst_sz = wcslen(dst);
  for (i = 0; i < count && src[i] != L'\0'; ++i)
  {
    dst[dst_sz + i] = src[i];
  }
  dst[dst_sz + i] = L'\0';
  
  return dst;
}

