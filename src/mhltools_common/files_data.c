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
#include <string.h>
#include <ctype.h>

#include <facade_info/error_codes.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <mhltools_common/logging.h>

#include <mhltools_common/files_data.h>

int
fill_data_from_input(
  const char* input_buf, 
  st_file_data_ext* file_data,
  st_conversion_settings* p_cs)
{
  const char* input_data_pointer;
  const char* input_data_pointer2;
  unsigned int i=0;
  char* loc_orig_fn;
  size_t worig_fn_sz;
  int res;

  input_data_pointer = input_buf;

  if (strncmp((const char*)MD5_HASH_SIGN, input_data_pointer,
              MD5_HASH_SIGN_SZ) == 0)
  {
    file_data->major_hash.hash_type = MHL_HT_MD5;
    file_data->major_hash.hash_type_str = MD5_HASH_SIGN_SMALL;
    input_data_pointer += MD5_HASH_SIGN_SZ;
  }
  else if (strncmp((const char*)SHA1_HASH_SIGN, input_data_pointer,
           SHA1_HASH_SIGN_SZ) == 0)
  {
    file_data->major_hash.hash_type = MHL_HT_SHA1;
    file_data->major_hash.hash_type_str = SHA1_HASH_SIGN_SMALL;
    input_data_pointer += SHA1_HASH_SIGN_SZ;
  }
  else if (strncmp((const char*)XXHASH64BE_HASH_SIGN, input_data_pointer,
                   XXHASH64BE_HASH_SIGN_SZ) == 0)
  {
      file_data->major_hash.hash_type = MHL_HT_XXHASH64BE;
      file_data->major_hash.hash_type_str = XXHASH64BE_HASH_SIGN_SMALL;
      input_data_pointer += XXHASH64BE_HASH_SIGN_SZ;
  }
  else if (strncmp((const char*)XXHASH64_HASH_SIGN, input_data_pointer,
                   XXHASH64_HASH_SIGN_SZ) == 0)
  {
      file_data->major_hash.hash_type = MHL_HT_XXHASH64;
      file_data->major_hash.hash_type_str = XXHASH64_HASH_SIGN_SMALL;
      input_data_pointer += XXHASH64_HASH_SIGN_SZ;
  }
  else if (strncmp((const char*)XXHASH_HASH_SIGN, input_data_pointer,
                   XXHASH_HASH_SIGN_SZ) == 0)
  {
      file_data->major_hash.hash_type = MHL_HT_XXHASH;
      file_data->major_hash.hash_type_str = XXHASH_HASH_SIGN_SMALL;
      input_data_pointer += XXHASH_HASH_SIGN_SZ;
  }
  else if (strncmp((const char*)NULL_HASH_SIGN, input_data_pointer,
                   NULL_HASH_SIGN_SZ) == 0)
  {
    file_data->major_hash.hash_type = MHL_HT_NULL;
    file_data->major_hash.hash_type_str = NULL_HASH_SIGN_SMALL;
    input_data_pointer += NULL_HASH_SIGN_SZ;
  }
  else
  {
    print_error("Wrong input format: unrecognized hash type");
    return ERRCODE_WRONG_INPUT_FORMAT;
  }

  if (*input_data_pointer != '(')
  {
    print_error("Wrong input format: must be '(' after hash type");
    return ERRCODE_WRONG_INPUT_FORMAT;
  }

  ++input_data_pointer;

  input_data_pointer2 = strrchr(input_data_pointer, ')');
  if (input_data_pointer2 == NULL)
  {
    print_error("Wrong input format: must be ')' after file name");
    return ERRCODE_WRONG_INPUT_FORMAT;
  }

  if (input_data_pointer2 == input_data_pointer)
  {
    print_error("Wrong input format: must be a file name after '('");
    return ERRCODE_WRONG_INPUT_FORMAT;
  }

  loc_orig_fn = (char*)calloc(
    input_data_pointer2 - input_data_pointer +1, sizeof(char));
  if (loc_orig_fn == NULL)
  {
    fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }

  // We have already '\0' at the end of string due to calloc
  strncpy(loc_orig_fn, input_data_pointer,
          input_data_pointer2 - input_data_pointer);

  //printf("Orig filename in locale: %s\n", loc_orig_fn);
  
  res = 
    convert_composed_from_locale_to_wchar(
      loc_orig_fn, 
      strlen(loc_orig_fn), 
      &file_data->orig_wfilename, 
      &worig_fn_sz,
      p_cs);

  if (res != 0)
  {
    fprintf(stderr, "Cannot convert filename '%s' from locale to wchar.\n", 
            loc_orig_fn);
    free(loc_orig_fn);
    return res;
  }

  //printf("Orig filename in wchar_t: %ls\n", file_data->orig_wfilename);
  
  make_wpath_os_specific(file_data->orig_wfilename);
  
  //printf("Orig filename in wchar_t after os_specific: %ls\n", 
  //       file_data->orig_wfilename);
  
  free(loc_orig_fn);

  input_data_pointer = input_data_pointer2 + 1;
  if (*input_data_pointer != '=')
  {
    print_error("Wrong input format: must be '=' after ')'");
    return ERRCODE_WRONG_INPUT_FORMAT;
  }
  ++input_data_pointer;

  if (*input_data_pointer != ' ')
  {
    print_error("Wrong input format: must be ' ' after '='");
    return ERRCODE_WRONG_INPUT_FORMAT;
  }
  ++input_data_pointer;

  if (file_data->major_hash.hash_type == MHL_HT_MD5)
  {
    file_data->major_hash.hash_sum_sz = MD5_HASH_LENGTH + 1;
  }
  else if (file_data->major_hash.hash_type == MHL_HT_SHA1)
  {
      file_data->major_hash.hash_sum_sz = SHA1_HASH_LENGTH + 1;
  }
  else if (file_data->major_hash.hash_type == MHL_HT_XXHASH)
  {
    file_data->major_hash.hash_sum_sz = XXHASH_HASH_LENGTH + 1;
  }
  else if (file_data->major_hash.hash_type == MHL_HT_XXHASH64)
  {
      file_data->major_hash.hash_sum_sz = XXHASH64_HASH_LENGTH + 1;
  }
  else if (file_data->major_hash.hash_type == MHL_HT_XXHASH64BE)
  {
      file_data->major_hash.hash_sum_sz = XXHASH64BE_HASH_LENGTH + 1;
  }
  else if (file_data->major_hash.hash_type == MHL_HT_NULL)
  {
    file_data->major_hash.hash_sum_sz = 0;
  }

  file_data->major_hash.hash_sum =
    (char*)calloc(file_data->major_hash.hash_sum_sz, sizeof(char));

  if (file_data->major_hash.hash_sum == NULL)
  {
    file_data->major_hash.hash_sum_sz = 0;
    fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }
  if (file_data->major_hash.hash_sum_sz) {
    for (i=0; i < file_data->major_hash.hash_sum_sz - 1; ++i)
    {
      if (isxdigit((unsigned char)input_data_pointer[i]) == 0)
      {
        print_error("Wrong input format: hash sum is not a message-digest");
        return ERRCODE_WRONG_INPUT_FORMAT;
      }
    }
    if (input_data_pointer[file_data->major_hash.hash_sum_sz - 1] != '\0' &&
        input_data_pointer[file_data->major_hash.hash_sum_sz - 1] != '\n' &&
        input_data_pointer[file_data->major_hash.hash_sum_sz - 1] != '\r')
    {
      print_error("Wrong input format: unexpected symbol after message-digest");
      return ERRCODE_WRONG_INPUT_FORMAT;
    }
    // We have already '\0' at the end of string due to calloc
    strncpy(file_data->major_hash.hash_sum, input_data_pointer,
            file_data->major_hash.hash_sum_sz - 1);
  }

  return 0;
}

static
int
fill_hash_data(st_hash_data* p_data, const char* hash_str,
               MHL_HASH_TYPE hash_type, unsigned int hash_length,
               const char* hash_type_str)
{
  p_data->hash_type = hash_type;
  p_data->hash_sum_sz = hash_length + 1;
  p_data->hash_type_str = hash_type_str;

  p_data->hash_sum =
    (char*)calloc(p_data->hash_sum_sz, sizeof(char));

  if (p_data->hash_sum == NULL)
  {
    p_data->hash_sum_sz = 0;
    fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }

  if (strlen(hash_str) != p_data->hash_sum_sz - 1)
  {
    print_error("Internal error: hash sum has been damaged");
    return ERRCODE_INTERNAL_ERROR;
  }

  // We have already '\0' at the end of string due to calloc
  strncpy(p_data->hash_sum, hash_str,
          p_data->hash_sum_sz - 1);

  return 0;
}

int
fill_data_directly(
  const wchar_t* wfilename,
  const char* md5_hash_str,
  const char* sha1_hash_str,
  const char* xx_hash_str,
  const char* xx64_hash_str,
  const char* xx64be_hash_str,
  st_file_data_ext* file_data)
{    
    int res;
    const char* hashes[] = {
        sha1_hash_str,
        md5_hash_str,
        xx_hash_str,
        xx64_hash_str,
        xx64be_hash_str,
    };
    const MHL_HASH_TYPE hash_types[] = {
        MHL_HT_SHA1,
        MHL_HT_MD5,
        MHL_HT_XXHASH,
        MHL_HT_XXHASH64,
        MHL_HT_XXHASH64BE,
    };
    const unsigned int hash_lengths[] = {
        SHA1_HASH_LENGTH,
        MD5_HASH_LENGTH,
        XXHASH_HASH_LENGTH,
        XXHASH64_HASH_LENGTH,
        XXHASH64BE_HASH_LENGTH,
    };
    const char* hash_sign_small[] = {
        SHA1_HASH_SIGN_SMALL,
        MD5_HASH_SIGN_SMALL,
        XXHASH_HASH_SIGN_SMALL,
        XXHASH64_HASH_SIGN_SMALL,
        XXHASH64BE_HASH_SIGN_SMALL,
    };
    
    size_t count_hashes = sizeof(hashes)/sizeof(hashes[0]);
    size_t i;

    if (md5_hash_str == NULL && sha1_hash_str == NULL && xx_hash_str == NULL && xx64_hash_str == NULL && xx64be_hash_str == NULL)
    {
        print_error("Internal error: fill_data_directly(): all hash strings are empty");
        return ERRCODE_INTERNAL_ERROR;
    }

    //set the first hash that is not NULL
    //as major hash and the second hash that
    //is not null as aux hash (if available)
    for(i = 0; i < count_hashes; ++i) {
        if(hashes[i] != NULL) {
            res = fill_hash_data(&file_data->major_hash, hashes[i], hash_types[i],
                                 hash_lengths[i], hash_sign_small[i]);
            if (res != 0)
            {
                return res;
            }
            break;
        }
    }
    //set the next non-null hash as aux hash
    for(i = i+1; i < count_hashes; ++i) {
        if(hashes[i] != NULL) {
            res = fill_hash_data(&file_data->aux_hash, hashes[i], hash_types[i],
                                 hash_lengths[i], hash_sign_small[i]);
            if (res != 0)
            {
                return res;
            }
            break;
        }
    }
    
    file_data->orig_wfilename = mhlosi_wstrdup(wfilename);
    if (file_data->orig_wfilename == NULL)
    {
        fprintf(stderr, "Out of memory.\n");
        return ERRCODE_OUT_OF_MEM;
    }

    make_wpath_os_specific(file_data->orig_wfilename);

    return 0;
}