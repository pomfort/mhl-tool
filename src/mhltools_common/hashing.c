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
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <wchar.h>

#include <facade_info/error_codes.h>
#include <generics/os_check.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/logging.h>
#include "hashing.h"
#include "xxhash.h"
#include "math.h"

#define FILE_DATA_BUFF_SZ (10 * 1024)

//
// The progress shall be printed every 2Mb
// that is why the corresponding byte value is useful
//
#define TWO_MB 2097152 // 2 * 1024 * 1024

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
                       st_logging_data* logging_data)
{
  int res;
  FILE* fd;
  size_t bytes_read;
  MD5_CTX md5_ctx; 
  //unsigned char md5_hash[MD5_DIGEST_LENGTH];
  unsigned char data_buff[FILE_DATA_BUFF_SZ];
  size_t bytesReadForLogging;
  
  // check params
  if (wfname == 0 || wfname[0] == L'\0' || hash_data == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  //
  fd = fwopen_for_hash_check(wfname);
  if (fd == NULL)
  {
    return ERRCODE_NO_SUCH_FILE;
  }
  
  //TODO: remoce these comments if they are not needed
  //   OPENSSL_init_library(); 
  //   OPENSSL_init();  
  res = MD5_Init(&md5_ctx);
  if (res != 1)
  {
    fclose(fd);
    return ERRCODE_OPENSSL_ERROR;
  }
  
  if (total_bytes_read)
  {
    *total_bytes_read = 0;
  }
  // read content file by chunks and pass chunks to md5 calculation function
  bytesReadForLogging = 0;
  while (!feof(fd))
  { 
    bytes_read = 
      fread(data_buff, sizeof(unsigned char), FILE_DATA_BUFF_SZ, fd);
    bytesReadForLogging += bytes_read;
    if (total_bytes_read)
    {
      *total_bytes_read += bytes_read;
    }

    if (bytes_read == 0)
    {  
      if (!feof(fd))
      {
        fclose(fd);
        return ERRCODE_IO_ERROR;
      }
      
      // eof
      break;
    }
    
    res = MD5_Update(&md5_ctx, data_buff, bytes_read);
    if (res != 1)
    {
      fclose(fd);
      return ERRCODE_OPENSSL_ERROR;
    }

    logging_data->progress_data.processed_sz += bytes_read;
    if (logging_data->progress_data.processed_sz >= logging_data->progress_data.logged_sz + (TWO_MB))
    {
      print_progress_message(logging_data);
      print_machine_progress_message(stderr, logging_data, bytesReadForLogging);
      bytesReadForLogging = 0;
    }
    
  }
  
  fclose(fd);
  
  *hash_data = calloc(MD5_DIGEST_LENGTH, sizeof(unsigned char));
  if (*hash_data == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  res = MD5_Final(*hash_data, &md5_ctx);
  if (res != 1)
  {
    return ERRCODE_OPENSSL_ERROR;
  }
  
  return 0;  
}

// Caller is responsible for free pinter returned in hash_str;
static int
aux_hash_data_to_string(
                    unsigned char* hash_data,
                    size_t hash_data_sz,
                    char** hash_str,
                    size_t* hash_str_sz)
{
  int res;
  size_t i, remain_str_sz;
  char* pbeg;

  if (hash_data == 0 || hash_data_sz == 0 ||
      hash_str == 0 || hash_str_sz == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  // two hex chars per byte
  *hash_str_sz = 2 * hash_data_sz;
  *hash_str = calloc(*hash_str_sz + 1, sizeof(char));
  if (*hash_str == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  pbeg = *hash_str;
  remain_str_sz = *hash_str_sz + 1;
  for (i = 0; i < (hash_data_sz) && remain_str_sz != 0; ++i)
  {
    res = mhlosi_snprintf(pbeg, remain_str_sz, "%02x", hash_data[i]);
    if (res < 0)
    {
      free((void*) *hash_str);
      *hash_str_sz = 0;

      return ERRCODE_INTERNAL_ERROR;
    }

    pbeg += 2;
    remain_str_sz = remain_str_sz >= 2 ? remain_str_sz - 2 : 0;
  }

  return 0;
}

// Caller is responsible for free pinter returned in hash_str;
int md5_hash_data_to_string(
                            unsigned char* hash_data, 
                            char** hash_str,
                            size_t* hash_str_sz)
{
  return 
    aux_hash_data_to_string(
      hash_data, MD5_DIGEST_LENGTH, hash_str, hash_str_sz);
}

int wcalculate_md5_hash_string(
  const wchar_t* wfname, 
  char** hash_str,
  size_t* hash_str_sz,
  unsigned long long* total_bytes,
  st_logging_data* logging_data)
{
  int res;
  unsigned char* hash_data;
  
  if (wfname == 0 || wfname[0] == L'\0' || 
      hash_str == 0 || hash_str_sz == 0 || total_bytes == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  res = wcalculate_md5_hash(wfname, &hash_data, total_bytes, logging_data);
  if (res != 0)
  {
    return res;
  }
  
  res = md5_hash_data_to_string(hash_data, hash_str, hash_str_sz);
  free(hash_data);
  
  return res;
}

int are_hashes_equal(
                     unsigned char* hash_data, 
                     size_t hash_data_sz,
                     const char* hash_str,
                     size_t hash_str_sz)
{
  int res;
  char* hash_data_str;
  size_t hash_data_str_sz;
  
  res = 
  aux_hash_data_to_string(
                          hash_data, 
                          hash_data_sz, 
                          &hash_data_str, 
                          &hash_data_str_sz);
  
  if (res != 0)
  {
    return res;
  }
  
  res = 
  hash_data_str_sz == hash_str_sz && 
  mhlosi_strncasecmp(hash_str, hash_data_str, hash_str_sz) == 0 ? 1 : 0;
  
  free((void*) hash_data_str);
  
  return res;
}


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
  st_logging_data* logging_data)
{
  int res;
  FILE* fd;
  size_t bytes_read;
  SHA_CTX sha1_ctx; 
  unsigned char data_buff[FILE_DATA_BUFF_SZ];
  size_t bytesReadForLogging;
  
    // check params
  if (wfname == 0 || wfname[0] == L'\0' || hash_data == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  //
  fd = fwopen_for_hash_check(wfname);
  if (fd == NULL)
  {
    return ERRCODE_NO_SUCH_FILE;
  }
  
  res = SHA1_Init(&sha1_ctx);
  if (res != 1)
  {
    fclose(fd);
    return ERRCODE_OPENSSL_ERROR;
  }
  
  if (total_bytes_read)
  {
    *total_bytes_read = 0;
  }
  // read content file by chunks and pass chunks to md5 calculation function
  
  bytesReadForLogging = 0;

  while (!feof(fd))
  { 
    bytes_read = 
    fread(data_buff, sizeof(unsigned char), FILE_DATA_BUFF_SZ, fd);
    
    bytesReadForLogging += bytes_read;
    if (total_bytes_read)
    {
      *total_bytes_read += bytes_read;
    }
    
    if (bytes_read == 0)
    {  
      if (!feof(fd))
      {
        fclose(fd);
        return ERRCODE_IO_ERROR;
      }
      
        // eof
      break;
    }
    
    res = SHA1_Update(&sha1_ctx, data_buff, bytes_read);
    if (res != 1)
    {
      fclose(fd);
      return ERRCODE_OPENSSL_ERROR;
    }

    logging_data->progress_data.processed_sz += bytes_read;
    if (logging_data->progress_data.processed_sz >= logging_data->progress_data.logged_sz + (TWO_MB))
    {
      print_progress_message(logging_data);
      print_machine_progress_message(stderr, logging_data, bytesReadForLogging);
      bytesReadForLogging = 0;
    }
  }
  
  fclose(fd);
  
  *hash_data = 
    (unsigned char*) calloc(SHA_DIGEST_LENGTH, sizeof(unsigned char));
  if (*hash_data == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  res = SHA1_Final(*hash_data, &sha1_ctx);
  if (res != 1)
  {
    return ERRCODE_OPENSSL_ERROR;
  }
  
  return 0;  
}

// Caller is responsible for free pointer returned in hash_str;
int sha1_hash_data_to_string(
  unsigned char* hash_data, 
  char** hash_str,
  size_t* hash_str_sz)
{
  return 
  aux_hash_data_to_string(
    hash_data, SHA_DIGEST_LENGTH, hash_str, hash_str_sz);
}

int wcalculate_sha1_hash_string(
  const wchar_t* wfname, 
  char** hash_str,
  size_t* hash_str_sz,
  unsigned long long* total_bytes,
  st_logging_data* logging_data)
{
  int res;
  unsigned char* hash_data;
  
  if (wfname == 0 || wfname[0] == L'\0' || 
      hash_str == 0 || hash_str_sz == 0 || total_bytes == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  res = wcalculate_sha1_hash(wfname, &hash_data, total_bytes, logging_data);
  if (res != 0)
  {
    return res;
  }
  
  res = sha1_hash_data_to_string(hash_data, hash_str, hash_str_sz);
  free(hash_data);
  
  return res;
}

//
// XXHash functions: Extremely fast non-cryptographic hash algorithm
// Based on https://code.google.com/p/xxhash/source/checkout
//

/* Calculates XX hash for given file.
 *
 * @param fname    - Name of file, which hash need to be calculated
 * @param hash_str - out string representation of hash value
 *
 * @return in case of success: 0,
 *         in case of failure: non zero value with error code
 */
int wcalculate_xx_hash(
                         const wchar_t* wfname,
                         //unsigned char** hash_data,
                         uint32_t * hash_data,
                         unsigned long long* total_bytes_read,
                         st_logging_data* logging_data)
{
    int res;
    FILE* fd;
    size_t bytes_read;
    //SHA_CTX sha1_ctx;
    XXH32_state_t* xxhash_state = NULL;
    unsigned char data_buff[FILE_DATA_BUFF_SZ];
    size_t bytesReadForLogging;
    
    // check params
    if (wfname == 0 || wfname[0] == L'\0' || hash_data == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }
    
    //
    fd = fwopen_for_hash_check(wfname);
    if (fd == NULL)
    {
        return ERRCODE_NO_SUCH_FILE;
    }
    
    xxhash_state = XXH32_createState();
    XXH32_reset(xxhash_state, 0);
    if (!xxhash_state)
    {
        fclose(fd);
        return ERRCODE_INITXXHASH_ERROR;
    }
    
    if (total_bytes_read)
    {
        *total_bytes_read = 0;
    }
    // read content file by chunks and pass chunks to md5 calculation function
  
    bytesReadForLogging = 0;

    while (!feof(fd))
    {
        bytes_read =
        fread(data_buff, sizeof(unsigned char), FILE_DATA_BUFF_SZ, fd);
        bytesReadForLogging += bytes_read;
        if (total_bytes_read)
        {
            *total_bytes_read += bytes_read;
        }
        
        if (bytes_read == 0)
        {
            if (!feof(fd))
            {
                fclose(fd);
                return ERRCODE_IO_ERROR;
            }
            
            // eof
            break;
        }
        
        //res = SHA1_Update(&sha1_ctx, data_buff, bytes_read);
        res = XXH32_update(xxhash_state, data_buff, (unsigned int)bytes_read);
        if (res != XXH_OK)
        {
            fclose(fd);
            return ERRCODE_OPENSSL_ERROR;
        }
        
      logging_data->progress_data.processed_sz += bytes_read;
      if (logging_data->progress_data.processed_sz >= logging_data->progress_data.logged_sz + (TWO_MB))
      {
        print_progress_message(logging_data);
        print_machine_progress_message(stderr, logging_data, bytesReadForLogging);
        bytesReadForLogging = 0;
      }
    }
    
    fclose(fd);
    
    //*hash_data =
    //(unsigned char*) calloc(MHL_XXHASH_HASH_BYTES_SZ, sizeof(unsigned char));
    //if (*hash_data == 0)
    //{
    //    return ERRCODE_OUT_OF_MEM;
    //}
    
    //res = SHA1_Final(*hash_data, &sha1_ctx);
    *hash_data = XXH32_digest(xxhash_state);
    XXH32_freeState(xxhash_state);
    
    //if (res != 1)
    //{
    //    return ERRCODE_OPENSSL_ERROR;
    //}
    
    return 0;
}

// Caller is responsible for free pointer returned in hash_str;
int xx_hash_data_to_string(
                             uint32_t hash_data,
                             char** hash_str,
                             size_t* hash_str_sz)
{
    int res;
    size_t remain_str_sz;
    
    if (hash_data == 0 ||
        hash_str == 0 || hash_str_sz == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }
    
    *hash_str_sz = 2 * MHL_XXHASH_HASH_BYTES_SZ;
    *hash_str = calloc(*hash_str_sz + 1, sizeof(char));
    if (*hash_str == 0)
    {
        return ERRCODE_OUT_OF_MEM;
    }
    
    remain_str_sz = *hash_str_sz + 1;
    res = sprintf(*hash_str, "%010u", hash_data);
    
    return 0;
}

int wcalculate_xx_hash_string(
                                const wchar_t* wfname,
                                char** hash_str,
                                size_t* hash_str_sz,
                                unsigned long long* total_bytes,
                                st_logging_data* logging_data)
{
    int res;
    uint32_t hash_data;
    
    if (wfname == 0 || wfname[0] == L'\0' || 
        hash_str == 0 || hash_str_sz == 0 || total_bytes == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }
    
    res = wcalculate_xx_hash(wfname, &hash_data, total_bytes, logging_data);
    if (res != 0)
    {
        return res;
    }
    
    res = xx_hash_data_to_string(hash_data, hash_str, hash_str_sz);
    //free(hash_data);
    
    return res;
}

int wcalculate_xx64_hash(
                         const wchar_t* wfname,
                         uint64_t* hash_data,//unsigned char** hash_data,
                         unsigned long long* total_bytes_read,
                         st_logging_data* logging_data)
{
    int res;
    FILE* fd;
    size_t bytes_read;
    //SHA_CTX sha1_ctx;
    XXH64_state_t* xxhash_state = NULL;
    unsigned char data_buff[FILE_DATA_BUFF_SZ];
    size_t bytesReadForLogging;

    // check params
    if (wfname == 0 || wfname[0] == L'\0' || hash_data == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }

    //
    fd = fwopen_for_hash_check(wfname);
    if (fd == NULL)
    {
        return ERRCODE_NO_SUCH_FILE;
    }

    xxhash_state = XXH64_createState();
    XXH64_reset(xxhash_state, 0);
    if (!xxhash_state)
    {
        fclose(fd);
        return ERRCODE_INITXXHASH_ERROR;
    }

    if (total_bytes_read)
    {
        *total_bytes_read = 0;
    }
    // read content file by chunks and pass chunks to md5 calculation function
  
    bytesReadForLogging = 0;

    while (!feof(fd))
    {
        bytes_read =
        fread(data_buff, sizeof(unsigned char), FILE_DATA_BUFF_SZ, fd);
        bytesReadForLogging += bytes_read;
        if (total_bytes_read)
        {
            *total_bytes_read += bytes_read;
        }

        if (bytes_read == 0)
        {
            if (!feof(fd))
            {
                fclose(fd);
                return ERRCODE_IO_ERROR;
            }

            // eof
            break;
        }

        res = XXH64_update(xxhash_state, data_buff, (unsigned int)bytes_read);
        if (res != XXH_OK)
        {
            fclose(fd);
            return ERRCODE_OPENSSL_ERROR;
        }

      logging_data->progress_data.processed_sz += bytes_read;
      if (logging_data->progress_data.processed_sz >= logging_data->progress_data.logged_sz + (TWO_MB))
      {
        print_progress_message(logging_data);
        print_machine_progress_message(stderr, logging_data, bytesReadForLogging);
        bytesReadForLogging = 0;
      }
    }

    fclose(fd);


    *hash_data = XXH64_digest(xxhash_state);
    XXH64_freeState(xxhash_state);

    return 0;
}

// Caller is responsible for free pointer returned in hash_str;
int xx64_hash_data_to_string(
                           uint64_t hash_data,
                           char** hash_str,
                           size_t* hash_str_sz)
{
    int res;
    size_t remain_str_sz;
    uint8_t* hash_bytes;

    if (hash_data == 0 ||
        hash_str == 0 || hash_str_sz == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }

    *hash_str_sz = 2 * MHL_XXHASH64_HASH_BYTES_SZ;
    *hash_str = calloc(*hash_str_sz + 1, sizeof(char));
    if (*hash_str == 0)
    {
        return ERRCODE_OUT_OF_MEM;
    }

    remain_str_sz = *hash_str_sz + 1;
    hash_bytes = (uint8_t*)&hash_data;
    res = sprintf(*hash_str, "%02x%02x%02x%02x%02x%02x%02x%02x",
                  hash_bytes[ 0], hash_bytes[ 1], hash_bytes[ 2], hash_bytes[ 3],
                  hash_bytes[ 4], hash_bytes[ 5], hash_bytes[ 6], hash_bytes[ 7]);

    return 0;
}

int wcalculate_xx64_hash_string(
                                const wchar_t* wfname,
                                char** hash_str,
                                size_t* hash_str_sz,
                                unsigned long long* total_bytes,
                                st_logging_data* logging_data) {
    int res;
    uint64_t hash_data;

    if (wfname == 0 || wfname[0] == L'\0' ||
        hash_str == 0 || hash_str_sz == 0 || total_bytes == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }

    res = wcalculate_xx64_hash(wfname, &hash_data, total_bytes, logging_data);
    if (res != 0)
    {
        return res;
    }

    res = xx64_hash_data_to_string(hash_data, hash_str, hash_str_sz);

    return res;
}

int wcalculate_xx64be_hash(
                         const wchar_t* wfname,
                         uint64_t* hash_data,//unsigned char** hash_data,
                         unsigned long long* total_bytes_read,
                         st_logging_data* logging_data)
{
    int res;
    FILE* fd;
    size_t bytes_read;
    //SHA_CTX sha1_ctx;
    XXH64_state_t* xxhash_state = NULL;
    unsigned char data_buff[FILE_DATA_BUFF_SZ];

    // check params
    if (wfname == 0 || wfname[0] == L'\0' || hash_data == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }

    //
    fd = fwopen_for_hash_check(wfname);
    if (fd == NULL)
    {
        return ERRCODE_NO_SUCH_FILE;
    }

    xxhash_state = XXH64_createState();
    XXH64_reset(xxhash_state, 0);
    if (!xxhash_state)
    {
        fclose(fd);
        return ERRCODE_INITXXHASH_ERROR;
    }

    if (total_bytes_read)
    {
        *total_bytes_read = 0;
    }
    // read content file by chunks and pass chunks to md5 calculation function
    while (!feof(fd))
    {
        bytes_read =
        fread(data_buff, sizeof(unsigned char), FILE_DATA_BUFF_SZ, fd);
        if (total_bytes_read)
        {
            *total_bytes_read += bytes_read;
        }

        if (bytes_read == 0)
        {
            if (!feof(fd))
            {
                fclose(fd);
                return ERRCODE_IO_ERROR;
            }

            // eof
            break;
        }

        res = XXH64_update(xxhash_state, data_buff, (unsigned int)bytes_read);
        if (res != XXH_OK)
        {
            fclose(fd);
            return ERRCODE_OPENSSL_ERROR;
        }

        if (logging_data->v_data.verbose_level >= VL_VERY_VERBOSE)
        {
            logging_data->progress_data.processed_sz += bytes_read;
            if (logging_data->progress_data.processed_sz >= logging_data->progress_data.logged_sz + TWO_MB)
            {
                print_progress_message(logging_data);
            }
        }
    }

    fclose(fd);


    *hash_data = XXH64_digest(xxhash_state);
    XXH64_freeState(xxhash_state);

    return 0;
}

// Caller is responsible for free pointer returned in hash_str;
int xx64be_hash_data_to_string(
                             uint64_t hash_data,
                             char** hash_str,
                             size_t* hash_str_sz)
{
    static const int one = 1;
    int is_little_endian = *((const char*)&one);

    int res;
    size_t remain_str_sz;
    uint8_t* hash_bytes;

    if (hash_data == 0 ||
        hash_str == 0 || hash_str_sz == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }

    *hash_str_sz = 2 * MHL_XXHASH64_HASH_BYTES_SZ;
    *hash_str = calloc(*hash_str_sz + 1, sizeof(char));
    if (*hash_str == 0)
    {
        return ERRCODE_OUT_OF_MEM;
    }

    remain_str_sz = *hash_str_sz + 1;
    hash_bytes = (uint8_t*)&hash_data;

    //swap by order to big endian
    if(is_little_endian) {
        res = sprintf(*hash_str, "%02x%02x%02x%02x%02x%02x%02x%02x",
                      hash_bytes[ 7], hash_bytes[ 6], hash_bytes[ 5], hash_bytes[ 4],
                      hash_bytes[ 3], hash_bytes[ 2], hash_bytes[ 1], hash_bytes[ 0]);
    }
    else {
        res = sprintf(*hash_str, "%02x%02x%02x%02x%02x%02x%02x%02x",
                      hash_bytes[ 0], hash_bytes[ 1], hash_bytes[ 2], hash_bytes[ 3],
                      hash_bytes[ 4], hash_bytes[ 5], hash_bytes[ 6], hash_bytes[ 7]);
    }

    return 0;
}

int wcalculate_xx64be_hash_string(
                                const wchar_t* wfname,
                                char** hash_str,
                                size_t* hash_str_sz,
                                unsigned long long* total_bytes,
                                st_logging_data* logging_data) {
    int res;
    uint64_t hash_data;

    if (wfname == 0 || wfname[0] == L'\0' ||
        hash_str == 0 || hash_str_sz == 0 || total_bytes == 0)
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }

    res = wcalculate_xx64be_hash(wfname, &hash_data, total_bytes, logging_data);
    if (res != 0)
    {
        return res;
    }

    res = xx64be_hash_data_to_string(hash_data, hash_str, hash_str_sz);

    return res;
}
