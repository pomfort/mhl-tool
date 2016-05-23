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

#ifndef _MHL_TOOLS_MHLTOOLS_COMMON_FILES_DATA_H_
#define _MHL_TOOLS_MHLTOOLS_COMMON_FILES_DATA_H_

#include <generics/char_conversions.h>
#include <generics/filesystem_handlers/public_interface.h>
#include <mhltools_common/mhl_types.h>

// hash  sizes
#define MD5_HASH_SIGN "MD5"
#define MD5_HASH_SIGN_SMALL "md5"
#define MD5_HASH_SIGN_SZ 3
#define MD5_HASH_LENGTH 32

#define SHA1_HASH_SIGN "SHA1"
#define SHA1_HASH_SIGN_SMALL "sha1"
#define SHA1_HASH_SIGN_SZ 4
#define SHA1_HASH_LENGTH 40

#define XXHASH_HASH_SIGN "xxHash"
#define XXHASH_HASH_SIGN_SMALL "xxhash"
#define XXHASH_HASH_SIGN_SZ 6
#define XXHASH_HASH_LENGTH 10

#define XXHASH64_HASH_SIGN "xxHash64"
#define XXHASH64_HASH_SIGN_SMALL "xxhash64"
#define XXHASH64_HASH_SIGN_SZ 8
#define XXHASH64_HASH_LENGTH 16

#define XXHASH64BE_HASH_SIGN "xxHash64BE"
#define XXHASH64BE_HASH_SIGN_SMALL "xxhash64be"
#define XXHASH64BE_HASH_SIGN_SZ 10
#define XXHASH64BE_HASH_LENGTH 16

#define NULL_HASH_SIGN "NULL"
#define NULL_HASH_SIGN_SMALL "null"
#define NULL_HASH_SIGN_SZ 4

/*
This structure is not used currently, 
it is a prototype for future to make 
file's data more light for usage in all modes

typedef struct _st_file_data
{
  MHL_HASH_TYPE hash_type; 
  wchar_t* orig_wfilename;
  st_fs_wpath file_wpath;
  unsigned long long file_sz;
  const char* hash_type_str;
  char* hash_sum;
  unsigned int hash_sum_sz;
} st_file_data;
*/

typedef struct _st_hash_data
{
  MHL_HASH_TYPE hash_type; 
  const char* hash_type_str;
  char* hash_sum;
  unsigned int hash_sum_sz;
} st_hash_data;

typedef struct _st_file_data_ext
{
  wchar_t* orig_wfilename;
  st_fs_wpath file_wpath;
  unsigned long long file_sz;
  char* creationdate_str;
  char* lastmodificationdate_str;
  char* hashdate_str;
  st_hash_data major_hash;
  st_hash_data aux_hash;
} st_file_data_ext;

typedef struct _st_files_data
{
  st_file_data_ext* files_data_array;
  size_t files_data_capacity;
  unsigned int files_data_cnt;
} st_files_data;

#define INITIAL_FILES_CAPACITY 10

int
fill_data_from_input(
  const char* input_buf, 
  st_file_data_ext* file_data,
  st_conversion_settings* p_cs);

int
fill_data_directly(
  const wchar_t* wfilename,
  const char* md5_hash_str,
  const char* sha1_hash_str,
  const char* xx_hash_str,
  const char* xx64_hash_str,
  const char* xx64be_hash_str,
  st_file_data_ext* file_data);

#endif // _MHL_TOOLS_MHLTOOLS_COMMON_FILES_DATA_H_