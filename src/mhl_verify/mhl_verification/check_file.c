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

#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#include <facade_info/error_codes.h>
#include <generics/os_check.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/hashing.h>

#include <mhltools_common/controlling_data.h>
#include <parsemhl/mhl_file_handlers.h>
#include <mhl_verify/verify_options.h>

#include "check_file.h"

//---------------------------------------------------------
//
// Set of functions for checking against MHL file content
//
//---------------------------------------------------------
static int
aux_check_md5_hash(st_mhl_file_check_wdata* p_check_wdata,
                   st_controlling_data* p_common)
{
  int res;
  unsigned char* hash_data;
  unsigned long long total_bytes_read;
  char* hash_str;
  size_t hash_str_sz;
  size_t mhl_hash_sz;

  //
  if (p_check_wdata->u8str_hash_sum == NULL)
  {
    return ERRCODE_WRONG_MHL_FORMAT;
  }

  //
  res = 
    wcalculate_md5_hash(
      p_check_wdata->abs_item_wfilename, 
      &hash_data,
      &total_bytes_read,
      &p_common->logging_data);
  
  if (res != 0)
  {
    return res;
  }
  
  res = 
    md5_hash_data_to_string(
      hash_data, 
      &hash_str,
      &hash_str_sz);

  free(hash_data);
  
  if (res != 0)
  {
    return res;
  }
      
  mhl_hash_sz = strlen(p_check_wdata->u8str_hash_sum);
  if (mhl_hash_sz != hash_str_sz)
  {
    free(hash_str);    
    return ERRCODE_MHL_CHECK_HASH_FAILED;
  }
  
  res = 
    mhlosi_strcasecmp(p_check_wdata->u8str_hash_sum, hash_str) == 0 ? 
    0 : ERRCODE_MHL_CHECK_HASH_FAILED;
  
  free(hash_str);
  
  return res;
}

//
static int
aux_check_sha1_hash(st_mhl_file_check_wdata* p_check_wdata,
                    st_controlling_data* p_common)
{
  int res;
  unsigned char* hash_data;
  unsigned long long total_bytes_read;
  char* hash_str;
  size_t hash_str_sz;
  size_t mhl_hash_sz;
  
  //
  if (p_check_wdata->u8str_hash_sum == NULL)
  {
    return ERRCODE_WRONG_MHL_FORMAT;
  }
  
  //
  res = 
    wcalculate_sha1_hash(
      p_check_wdata->abs_item_wfilename, 
      &hash_data,
      &total_bytes_read,
      &p_common->logging_data);
  
  if (res != 0)
  {
    return res;
  }
  
  res = 
    sha1_hash_data_to_string(
      hash_data, 
      &hash_str,
      &hash_str_sz);
  
  free(hash_data);
  
  if (res != 0)
  {
    return res;
  }
  
  mhl_hash_sz = strlen(p_check_wdata->u8str_hash_sum);
  if (mhl_hash_sz != hash_str_sz)
  {
    free(hash_str);    
    return ERRCODE_MHL_CHECK_HASH_FAILED;
  }
  
  res = 
    mhlosi_strcasecmp(p_check_wdata->u8str_hash_sum, hash_str) == 0 ? 
    0 : ERRCODE_MHL_CHECK_HASH_FAILED;
  
  free(hash_str);
  
  return res;
}


//
static int
aux_check_xx_hash(st_mhl_file_check_wdata* p_check_wdata,
                    st_controlling_data* p_common)
{
    int res;
    uint32_t hash_data;
    unsigned long long total_bytes_read;
    char* hash_str;
    size_t hash_str_sz;
    size_t mhl_hash_sz;
    
    //
    if (p_check_wdata->u8str_hash_sum == NULL)
    {
        return ERRCODE_WRONG_MHL_FORMAT;
    }
    
    //
    res =
    wcalculate_xx_hash(
                         p_check_wdata->abs_item_wfilename,
                         &hash_data,
                         &total_bytes_read,
                         &p_common->logging_data);
    
    if (res != 0)
    {
        return res;
    }
    
    res =
    xx_hash_data_to_string(
                             hash_data,
                             &hash_str,
                             &hash_str_sz);
    
    if (res != 0)
    {
        return res;
    }
    
    mhl_hash_sz = strlen(p_check_wdata->u8str_hash_sum);
    if (mhl_hash_sz != hash_str_sz)
    {
        free(hash_str);
        return ERRCODE_MHL_CHECK_HASH_FAILED;
    }
    
    res = 
    mhlosi_strcasecmp(p_check_wdata->u8str_hash_sum, hash_str) == 0 ? 
    0 : ERRCODE_MHL_CHECK_HASH_FAILED;
    
    free(hash_str);
    
    return res;
}

//
static int
aux_check_xx64_hash(st_mhl_file_check_wdata* p_check_wdata,
                  st_controlling_data* p_common)
{
    int res;
    uint64_t hash_data;
    unsigned long long total_bytes_read;
    char* hash_str;
    size_t hash_str_sz;
    size_t mhl_hash_sz;

    //
    if (p_check_wdata->u8str_hash_sum == NULL)
    {
        return ERRCODE_WRONG_MHL_FORMAT;
    }

    //
    res =
    wcalculate_xx64_hash(
                       p_check_wdata->abs_item_wfilename,
                       &hash_data,
                       &total_bytes_read,
                       &p_common->logging_data);

    if (res != 0)
    {
        return res;
    }

    res =
    xx64_hash_data_to_string(
                           hash_data,
                           &hash_str,
                           &hash_str_sz);

    if (res != 0)
    {
        return res;
    }

    mhl_hash_sz = strlen(p_check_wdata->u8str_hash_sum);
    if (mhl_hash_sz != hash_str_sz)
    {
        free(hash_str);
        return ERRCODE_MHL_CHECK_HASH_FAILED;
    }

    res =
    mhlosi_strcasecmp(p_check_wdata->u8str_hash_sum, hash_str) == 0 ?
    0 : ERRCODE_MHL_CHECK_HASH_FAILED;

    free(hash_str);

    return res;
}

//
static int
aux_check_xx64be_hash(st_mhl_file_check_wdata* p_check_wdata,
                    st_controlling_data* p_common)
{
    int res;
    uint64_t hash_data;
    unsigned long long total_bytes_read;
    char* hash_str;
    size_t hash_str_sz;
    size_t mhl_hash_sz;

    //
    if (p_check_wdata->u8str_hash_sum == NULL)
    {
        return ERRCODE_WRONG_MHL_FORMAT;
    }

    //
    res =
    wcalculate_xx64be_hash(
                         p_check_wdata->abs_item_wfilename,
                         &hash_data,
                         &total_bytes_read,
                         &p_common->logging_data);

    if (res != 0)
    {
        return res;
    }

    res =
    xx64be_hash_data_to_string(
                             hash_data,
                             &hash_str,
                             &hash_str_sz);

    if (res != 0)
    {
        return res;
    }

    mhl_hash_sz = strlen(p_check_wdata->u8str_hash_sum);
    if (mhl_hash_sz != hash_str_sz)
    {
        free(hash_str);
        return ERRCODE_MHL_CHECK_HASH_FAILED;
    }

    res =
    mhlosi_strcasecmp(p_check_wdata->u8str_hash_sum, hash_str) == 0 ?
    0 : ERRCODE_MHL_CHECK_HASH_FAILED;

    free(hash_str);

    return res;
}

//
// Check is real file "fingerprints" are equal to
// "fingerprints" from "hash" tag of MHL file.
//
int check_file_against_mhl_file_witem(
  st_mhl_file_check_wdata* p_check_wdata, 
  unsigned char check_existence,
  st_controlling_data* p_common)
{
  int res;
  unsigned long long ull_fsz;
  
  if (p_check_wdata == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  // Check file sizes
  ull_fsz = 0;
  res = get_wfile_size(p_check_wdata->abs_item_wfilename, &ull_fsz);
  if (res != 0)
  {
    return res;
  }
  
  if (ull_fsz != p_check_wdata->file_sz)
  {
    return ERRCODE_MHL_CHECK_FILE_SIZE_FAILED;
  }
  
  if (check_existence != 0)
  {
    // Checks only file existence and filesize equality.
    return 0;
  }
  
  // Check file's hashusm
  switch (p_check_wdata->hash_type) 
  {
    case MHL_HT_MD5:
      res = aux_check_md5_hash(p_check_wdata, p_common);      
      break;

    case MHL_HT_SHA1:
      res = aux_check_sha1_hash(p_check_wdata, p_common); 
      break;

    case MHL_HT_XXHASH:
      res = aux_check_xx_hash(p_check_wdata, p_common);
      break;

    case MHL_HT_XXHASH64:
      res = aux_check_xx64_hash(p_check_wdata, p_common);
      break;

    case MHL_HT_XXHASH64BE:
      res = aux_check_xx64be_hash(p_check_wdata, p_common);
      break;
    case MHL_HT_NULL:
      res = 0;
      break;

    default:
      res = ERRCODE_NOT_IMPLEMENTED;
      break;
  }
  
  // TODO: May be some code result processing should be added here. 
  //       Think about it 
  return res;
}

//
//
//
int check_file_against_mhl_wcontent(
      const wchar_t* abs_wfilename, 
      st_mhl_file_wcontent* p_wcontent, 
      unsigned char check_existence,
      st_controlling_data* p_common)
{
  int res;
  st_mhl_file_check_wdata* p_switem;

  
  if (abs_wfilename == 0 || abs_wfilename[0] == L'\0' ||
      p_wcontent == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  p_switem = 
    search_for_mhl_file_check_wdata(p_wcontent->check_witems, abs_wfilename);
  if (p_switem == 0)
  {
    return ERRCODE_MHL_CHECK_NO_MHL_ENTRY;
  }

  res = check_file_against_mhl_file_witem(p_switem, check_existence, p_common);    
  return res;
}
