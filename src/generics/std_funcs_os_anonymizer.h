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
 * @file: std_funcs_os_anonymizer.h
 * 
 * Definitions of generic functions have different 
 * implementation on different OS
 * 
 */
#ifndef _MHL_TOOLS_GENERICS_STD_FUNCS_OS_ANONYMIZER_H_
#define _MHL_TOOLS_GENERICS_STD_FUNCS_OS_ANONYMIZER_H_

#include <wchar.h>
#include <generics/os_check.h>

/* 
 * OS specific version of "strdup".
 * strdup POSIX function is deprecated beginning in Visual C++ 2005. 
 */
char* mhlosi_strdup(const char* s);

/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
int mhlosi_strcasecmp(const char* s1, const char* s2);

/* 
 * OS specific version of "strncasecmp". strncasecmp is GNU specific function.
 */
int mhlosi_strncasecmp(const char* s1, const char* s2, size_t n);

/*
 * OS specific version of "strcasestr". strcasestr is GNU specific function.
 */
char* mhlosi_strcasestr(const char* haystack, const char* needle);

/*
 * OS specific version of "strtoull". strtoull is Linux specific function.
 */
unsigned long long mhlosi_strtoull(const char *nptr, char **endptr, int base);

/*
 * Windows has finction _snprintf, MacOS X and Linux has function snprintf.
 * Parameters are identical for all OS versions
 */
#ifdef WIN
#define mhlosi_snprintf _snprintf  
#else
#define mhlosi_snprintf snprintf
#endif

//
// wide char functions
//
/* 
 * OS specific version of "wide char string duplication".
 */
wchar_t* mhlosi_wstrdup(const wchar_t* s);

/* 
 * OS specific version of "n chars from wide char string duplication".
 */
wchar_t* mhlosi_wstrndup(const wchar_t* s, size_t s_sz);

/* 
 * OS specific version of "strcmp" for wide strings .
 */
int mhlosi_wstrcmp(const wchar_t* s1, const wchar_t* s2);

/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
int mhlosi_wstrcasecmp(const wchar_t* s1, const wchar_t* s2);

/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
int mhlosi_wstrncasecmp(const wchar_t* ws1, const wchar_t* ws2, size_t wsz);

/* 
 * OS specific version of "strcasecmp". strcasecmp is GNU specific function.
 */
int mhlosi_wstrncmp(const wchar_t* s1, const wchar_t* s2, size_t n);

/*
 * OS specific version of "strcasestr". strcasestr is GNU specific function.
 */
wchar_t* mhlosi_wstrstr(const wchar_t* haystack, const wchar_t* needle);

/*
 * OS specific and safe version of wcscat. 
 * Appends at least n wchars from str to dst.
 * NOTE: buffer with dst string should have enough space for appended n wchars!
 *
 * Returns pointer to destination string. 0 means error.
 */
wchar_t* mhlosi_wstrncat(const wchar_t* src, wchar_t* dst, size_t count);

#endif // _MHL_TOOLS_GENERICS_STD_FUNCS_OS_ANONYMIZER_H_
