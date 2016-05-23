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

#ifndef _MHL_TOOLS_MHLTOOLS_COMMON_HASHING_H_
#define _MHL_TOOLS_MHLTOOLS_COMMON_HASHING_H_

#include <wchar.h>
#include <mhltools_common/logging.h>
#include <stdint.h>

#define MHL_MD5_HASH_BYTES_SZ  16 
#define MHL_SHA1_HASH_BYTES_SZ 20
#define MHL_XXHASH_HASH_BYTES_SZ 5
#define MHL_XXHASH64_HASH_BYTES_SZ 8
#define MHL_XXHASH64BE_HASH_BYTES_SZ 8

#define MD5_HASH_PREFIX "MD5:"
#define SHA1_HASH_PREFIX "SHA1:"
#define XXHASH_HASH_PREFIX "xxHash:"
#define XXHASH64_HASH_PREFIX "xxHash64:"
#define XXHASH64BE_HASH_PREFIX "xxHash64BE:"

//
// MD5 functions
//

/* Calculates MD5 hash for given file.
 *
 * @param fname    - Name of file, which hash need to be calculated
 * @param hash_str - out string representation of hash value
 *
 * @return in case of success: 0,  
 *         in case of failure: non zero value with error code 
 */
int wcalculate_md5_hash(
                       const wchar_t* wfname, 
                       unsigned char** hash_data,
                       unsigned long long* total_bytes_read,
                       st_logging_data* log_data);

// Caller is responsible for free pinter returned in hash_str;
int md5_hash_data_to_string(
                            unsigned char* hash_data, 
                            char** hash_str,
                            size_t* hash_str_sz);

int wcalculate_md5_hash_string(
  const wchar_t* wfname, 
  char** hash_str,
  size_t* hash_str_sz,
  unsigned long long* total_bytes,
  st_logging_data* log_data);

int are_hashes_equal(
                     unsigned char* hash_data, 
                     size_t hash_data_sz,
                     const char* hash_str,
                     size_t hash_str_sz);

//
// SHA1 functions
//
/* Calculates SHA1 hash for given file.
 *
 * @param fname    - Name of file, which hash need to be calculated
 * @param hash_str - out string representation of hash value
 *
 * @return in case of success: 0,  
 *         in case of failure: non zero value with error code 
 */
int wcalculate_sha1_hash(
  const wchar_t* wfname, 
  unsigned char** hash_data,
  unsigned long long* total_bytes_read,
  st_logging_data* log_data);

// Caller is responsible for free pointer returned in hash_str;
int sha1_hash_data_to_string(
  unsigned char* hash_data, 
  char** hash_str,
  size_t* hash_str_sz);

int wcalculate_sha1_hash_string(
  const wchar_t* wfname, 
  char** hash_str,
  size_t* hash_str_sz,
  unsigned long long* total_bytes,
  st_logging_data* log_data);


//
// XXHASH functions
//
/* Calculates XXhash for given file.
 *
 * @param fname    - Name of file, which hash need to be calculated
 * @param hash_str - out string representation of hash value
 *
 * @return in case of success: 0,
 *         in case of failure: non zero value with error code
 */
int wcalculate_xx_hash(
                         const wchar_t* wfname,
                         uint32_t* hash_data,//unsigned char** hash_data,
                         unsigned long long* total_bytes_read,
                         st_logging_data* log_data);

// Caller is responsible for free pointer returned in hash_str;
int xx_hash_data_to_string(
                             uint32_t hash_data,//unsigned char* hash_data,
                             char** hash_str,
                             size_t* hash_str_sz);

int wcalculate_xx_hash_string(
                                const wchar_t* wfname, 
                                char** hash_str,
                                size_t* hash_str_sz,
                                unsigned long long* total_bytes,
                                st_logging_data* log_data);

/* Calculates XXhash64 for given file.
 *
 * @param fname    - Name of file, which hash need to be calculated
 * @param hash_str - out string representation of hash value
 *
 * @return in case of success: 0,
 *         in case of failure: non zero value with error code
 */
int wcalculate_xx64_hash(
                       const wchar_t* wfname,
                       uint64_t* hash_data,//unsigned char** hash_data,
                       unsigned long long* total_bytes_read,
                       st_logging_data* log_data);

// Caller is responsible for free pointer returned in hash_str;
int xx64_hash_data_to_string(
                           uint64_t hash_data,
                           char** hash_str,
                           size_t* hash_str_sz);

int wcalculate_xx64_hash_string(
                              const wchar_t* wfname,
                              char** hash_str,
                              size_t* hash_str_sz,
                              unsigned long long* total_bytes,
                              st_logging_data* log_data);

/* Calculates XXhash64 for given file.
 *
 * @param fname    - Name of file, which hash need to be calculated
 * @param hash_str - out string representation of hash value
 *
 * @return in case of success: 0,
 *         in case of failure: non zero value with error code
 */
int wcalculate_xx64be_hash(
                         const wchar_t* wfname,
                         uint64_t* hash_data,//unsigned char** hash_data,
                         unsigned long long* total_bytes_read,
                         st_logging_data* log_data);

// Caller is responsible for free pointer returned in hash_str;
int xx64be_hash_data_to_string(
                             uint64_t hash_data,
                             char** hash_str,
                             size_t* hash_str_sz);

int wcalculate_xx64be_hash_string(
                                const wchar_t* wfname,
                                char** hash_str,
                                size_t* hash_str_sz,
                                unsigned long long* total_bytes,
                                st_logging_data* log_data);


#endif // _MHL_TOOLS_MHLTOOLS_COMMON_HELP_PRINTING_H_