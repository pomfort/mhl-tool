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
 * @file: os_filesystem_elements.h
 * 
 * Implementation of elementary functions for working with 
 * filesystem entries.
 */

#include <sys/types.h> 
#include <sys/stat.h> 
#include <stdlib.h>
#include <wchar.h>
#include <fcntl.h> 
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <generics/os_check.h>
#ifdef WIN
#include <sys/types.h> 
#include <io.h> 
#include <share.h> 
#else
#include <unistd.h>
#endif 

#include <facade_info/error_codes.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/char_conversions.h>
#include <generics/filesystem_handlers/public_interface.h>

#ifndef WIN
// 
// Linux and MacOS only
//
/*
 * Note: Caller is responsible for freeing returned pointer
 *
 * In case of error: NULL wil; be returned
 *
 */
char* wfilename_to_locale_filename(const wchar_t* wfn)
{
  char* locenc_buf = 0;
  size_t locenc_buf_len = 0;
  mbstate_t ps;
  size_t res_sz;
  size_t wfn_len;

  if (wfn == 0 || wfn[0] == L'\0')
  {
    return 0;
  }

  wfn_len = wcslen(wfn);
  locenc_buf_len = (wfn_len + 1) * MAX_UTF8_CHAR_SZ;
  locenc_buf = calloc(locenc_buf_len , sizeof(char));
  if (locenc_buf == 0)
  {
    return 0;
  }
  locenc_buf_len -= MAX_UTF8_CHAR_SZ; // reserve space for \0

  memset(&ps, 0, sizeof(ps) / sizeof(char));
  res_sz = wcsnrtombs(locenc_buf, &wfn, wfn_len, locenc_buf_len, &ps);
  if (res_sz == (size_t) -1)
  {
    free(locenc_buf);
    return 0;
  }

  return locenc_buf;
}
#endif

/* Gets file stat mode. 
 *
 * @return: Success: 0, Error: non zero value with error code
 */
int get_wfile_stat_data(
  const wchar_t* wpath, st_mhlosi_stat* stat_data)
{
  int res;

#if defined MAC_OS_X || defined LINUX
  char* locencfn = 0;
#endif 

  if (wpath == 0 || wpath[0] == L'\0' || stat_data == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  memset(stat_data, 0, sizeof(*stat_data) / sizeof(char));

#ifdef WIN
  res = _wstati64(wpath, &stat_data->st_data);
#else

  locencfn = wfilename_to_locale_filename(wpath);
  if (locencfn == NULL)
  {
    return ERRCODE_CHARS_CONVERSION_ERROR;
  }

#ifdef MAC_OS_X
  res = lstat(locencfn, &stat_data->st_data);
#else
  res = lstat64(locencfn, &stat_data->st_data);
#endif
  free(locencfn);
#endif

  if (res != 0)
  {
    if (errno == ENOENT)
    {
      return ERRCODE_NO_SUCH_FILE;
    }
    
    return ERRCODE_IO_ERROR;
  }

  return 0;
}

/* Gets file stat mode. 
 *
 * @return: Success: 0, Error: non zero value with error code
 */
int get_wfile_stat_mode(
  const wchar_t* wpath, unsigned long* mode)
{
  int res;
  st_mhlosi_stat stat_data;

  res = get_wfile_stat_data(wpath, &stat_data);
  if (res != 0)
  {
    return res;
  }

  *mode = stat_data.st_data.st_mode;
  return 0;
}


/* Check is given path is a directrory
 *
 * @return: If it is directory 1 will be returned,
 *          if not directory 0 will be returned
 */
static
unsigned char aux_is_directory(unsigned long mode)
{
#ifdef WIN
  return (mode & _S_IFDIR) == _S_IFDIR ? 1 : 0;
#else
  return S_ISDIR(mode) ? 1 : 0;
#endif
}

/* Check is given path is a regular file
 *
 * @return: If it is regular file 1 will be returned,
 *          if not directory 0 will be returned
 */
static
unsigned char aux_is_regular_file(unsigned long mode)
{
#ifdef WIN
  return (mode & _S_IFREG) == _S_IFREG ? 1 : 0;
#else
  return S_ISREG(mode) ? 1 : 0;
#endif
}

unsigned char is_directory(const wchar_t* wpath)
{
  unsigned long mode;
  int res;

  res = get_wfile_stat_mode(wpath, &mode);
  if (res != 0)
  {
    return 0;
  }

  return aux_is_directory(mode);
}

unsigned char is_regular_file(const wchar_t* wpath)
{
  unsigned long mode;
  int res;

  res = get_wfile_stat_mode(wpath, &mode);
  if (res != 0)
  {
    return 0;
  }

  return aux_is_regular_file(mode);
}

int get_file_type(const wchar_t* wpath, DIR_ENTRY_TYPE_FLAGS* t_flag)
{
  unsigned long mode;
  int res;

  res = get_wfile_stat_mode(wpath, &mode);
  if (res != 0)
  {
    return res;
  }

  *t_flag = DETF_UNK;

  if (aux_is_directory(mode))
  {
    *t_flag = DETF_DIR;
  }
  else if (aux_is_regular_file(mode))
  {
    *t_flag = DETF_FILE;
  }

#ifndef WIN
  switch (mode & S_IFMT)
  {
    case S_IFBLK:
      *t_flag = DETF_BLK;

    case S_IFCHR:
      *t_flag = DETF_CHR;

    case S_IFIFO:
      *t_flag = DETF_FIFO;
 
    case S_IFLNK:
      *t_flag = DETF_LNK;

    case S_IFSOCK:
      *t_flag = DETF_SOCK;
  }
#endif

  return 0;
}

void print_ent_type(DIR_ENTRY_TYPE_FLAGS ent_type)
{
  switch(ent_type)
  {
    case DETF_FILE:
          fprintf(stderr, "This is a regular file.");
          break;

    case DETF_DIR:
          fprintf(stderr, "This is a directory.");
          break;

    case DETF_BLK:
          fprintf(stderr, "This is a block device.");
          break;

    case DETF_CHR:
          fprintf(stderr, "This is a character device.");
          break;

    case DETF_FIFO:
          fprintf(stderr, "This is a named pipe (FIFO).");
          break;

    case DETF_LNK:
          fprintf(stderr, "This is a symbolic link.");
          break;

    case DETF_SOCK:
          fprintf(stderr, "This is a UNIX domain socket.");
          break;

    case DETF_UNK:
    default:
        fprintf(stderr, "The file type is unknown.");
  }
}

/*
 * @return: If it is a readable entry, 1 will be returned,
 *          otherwise, 0 will be returned
 */
unsigned char does_wpath_exist(const wchar_t* wpath)
{
  int res;
  unsigned long mode = 0;

  if (wpath == 0 || wpath[0] == L'\0')
  {
    return 0;
  }  

  res = get_wfile_stat_mode(wpath, &mode);
  return res == 0 ? 1 : 0;
}

unsigned char does_wfilenames_equal(
  const wchar_t* wfn1,
  const wchar_t* wfn2)
{
  if (wfn1 == 0 || wfn1[0] == L'\0' || wfn2 == 0 || wfn2[0] == L'\0')
  {
    return 0;
  }
#ifdef WIN
  return mhlosi_wstrcasecmp(wfn1, wfn2) == 0 ? 1 : 0;
#else
  return mhlosi_wstrcmp(wfn1, wfn2) == 0 ? 1 : 0;
#endif
}

/* Gets file size. 
 *
 * @return: Success: 0, Error: non zero value with error code
 */
int get_wfile_size(const wchar_t* wfn, unsigned long long* p_fsz)
{
  int res;
  st_mhlosi_stat stat_data;

  res = get_wfile_stat_data(wfn, &stat_data);
  if (res != 0)
  {
    return res;
  }

  *p_fsz = stat_data.st_data.st_size;
  return 0;
}

FILE* fwopen_for_hash_check(const wchar_t* wfn)
{
#ifdef WIN
  return _wfopen(wfn, L"rb");
#else
  FILE* wfd;
  char* locencfn = 0;

  if (wfn == 0 || wfn[0] == L'\0')
  {
    return 0;
  }

  locencfn = wfilename_to_locale_filename(wfn);
  if (locencfn == NULL)
  {
    return 0;
  }

  wfd = fopen(locencfn, "r");
  free(locencfn);
  return wfd;
#endif
}

FILE* fwopen_for_hash_create(const wchar_t* wfn)
{
#ifdef WIN
  return _wfopen(wfn, L"w");
#else
  FILE* wfd;
  char* locencfn = 0;

  if (wfn == 0 || wfn[0] == L'\0')
  {
    return 0;
  }

  locencfn = wfilename_to_locale_filename(wfn);
  if (locencfn == NULL)
  {
    return 0;
  }

  wfd = fopen(locencfn, "w");
  free(locencfn);
  return wfd;
#endif
}

/* The same functiomnality as open from C stdlib
 *
 * On windows in order to open binary _wsopen_s should be used
 */
int wopen_for_read(const wchar_t* wfn, int* p_fd)
{
#ifdef WIN
  errno_t err;
#else 
  char* locencfn = 0;
#endif

  if (p_fd == 0 || wfn == 0 || wfn[0] == L'\0')
  {
     return ERRCODE_WRONG_ARGUMENTS;
  }

#ifdef WIN
  err = _wsopen_s(p_fd, wfn, _O_BINARY | _O_RDONLY, _SH_DENYRD, 0);
  return err != 0 ? ERRCODE_IO_ERROR : 0;
#else
  locencfn = wfilename_to_locale_filename(wfn);
  if (locencfn == 0)
  {
    return ERRCODE_IO_ERROR;
  }

  *p_fd = open(locencfn, O_RDONLY);
  free(locencfn);
  if (*p_fd == -1)
  {
    *p_fd = 0;
    return ERRCODE_IO_ERROR;
  }
  
  return 0;
#endif
}

int mhlosi_close(int fd)
{
#ifdef WIN
  return _close(fd);
#else
  return close(fd);
#endif
}


