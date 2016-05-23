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

#include <generics/os_check.h>
#ifdef WIN
#define CINTERFACE
#define SECURITY_WIN32
#include <Windows.h>
#include <Security.h>
#include <Iads.h>
#include <direct.h>
#include <Lmcons.h>
#include <OleAuto.h>
#else
// Define it for explicit work with big files on Linux using 64-bits offset
// via struct stat64, and the stat64() function.
// With just stat() the code may not work properly on 32-bit Linux platforms
//#define _GNU_SOURCE
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <wctype.h> 
#endif

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#include <facade_info/error_codes.h>
#include <generics/filesystem_handlers/public_interface.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/char_conversions.h>
#include <generics/memory_management.h>
#include <generics/filesystem_handlers/file_path_decomposition.h>

#define MAX_WD_ITERS 10

#ifdef WIN
#define MAX_DIR_LEN 1024
#else
#define MAX_DIR_LEN PATH_MAX
#endif

// Note! Caller is responsible for free returned pointer
wchar_t* 
get_wworkdir(st_conversion_settings* pcs)
{
    char* cbuff = 0;
    wchar_t* wbuff = 0;
    size_t buff_sz = MAX_DIR_LEN;
    size_t wbuff_sz = 0;
    int i;
    int ires;
    char* res;
    st_conversion_settings tmp_cs;
    unsigned char tmp_cs_inited = 0;
    
    //
    for (i = 0; i < MAX_WD_ITERS; ++i)
    {
        cbuff = (char*)calloc(buff_sz * 2, sizeof(char));
        
        if (cbuff == 0)
        {
            return 0;
        }
        
        buff_sz *= 2;
        
#ifdef WIN
        res= _getcwd(cbuff, buff_sz);
#else
        res = getcwd(cbuff, buff_sz);
#endif
        if (res == 0)
        {
            free(cbuff);
            cbuff = 0;
            
            if (errno == ENAMETOOLONG || errno == ERANGE) 
            {
                continue;
            }
            
            return 0;
        }
        
        if (pcs == 0)
        {
          ires = init_st_conversion_settings(&tmp_cs);
          if (ires != 0)
          {
            free(cbuff);
            return 0;
          }

          pcs = &tmp_cs;
          tmp_cs_inited = 1;
        }

        ires = 
          convert_composed_from_locale_to_wchar(
            cbuff, 
            strlen(cbuff), 
            &wbuff, 
            &wbuff_sz,
            pcs);

        if (tmp_cs_inited)
        {
          free_st_conversion_settings(&tmp_cs);
        }

        free(cbuff);

        if (ires != 0)
        {    
            return 0;
        }

        make_wpath_os_specific(wbuff);

        return wbuff;
    }
    
    return 0;
}

void make_wpath_uniform(wchar_t* wpath)
{
  if (wpath == 0 || *wpath == L'\0')
  {
    // do nothing in case of error
    return;
  }

  for(; *wpath != L'\0'; ++wpath)
  {
    if (*wpath == WPATH_SEPARATOR)
    {
      *wpath = WPATH_UNIFORM_SEPARATOR;
    }
  }
}

//
// Relative wpath must be against working directory, as this functions uses it
// for making the absolute path
//
int convert_to_absolute_normalized_wpath(
  const wchar_t* relative_wpath,
  wchar_t** abs_wpath,
  st_conversion_settings* p_cs)
{
  int res;
  size_t wpath_sz;
  wchar_t* wworkdir;
  
  if (relative_wpath == 0 || relative_wpath[0] == L'\0' || abs_wpath == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  wpath_sz = wcslen(relative_wpath);
  if (is_root_wpass_prefix(relative_wpath, wpath_sz))
  {
    *abs_wpath = mhlosi_wstrdup(relative_wpath);
    if (*abs_wpath == 0)
    {
      return ERRCODE_OUT_OF_MEM;
    }
    
    make_wpath_os_specific(*abs_wpath);

    return 0;
  }
  
  wworkdir = get_wworkdir(p_cs);
  if (wworkdir == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  res = 
    create_absolute_normalized_wpath(wworkdir, relative_wpath, abs_wpath);

  free((void*) wworkdir);
  
  return res;
}


/* 
 * Note! Caller is responsible for free returned pointer,
 *       returned in dir_path param.
 * Extracts dir fom passed filename.
 * 
 * @param fpath Source filename
 * @param dir_path directory will be placed here
 * @return Success 0, Failure error code > 0
 */
int extract_wdir_from_wpath(const wchar_t* src_wpath, wchar_t** parent_dir_wpath)
{
    int res;
    st_fs_wpath fs_wpath;    
    
    // Check input params
    if (parent_dir_wpath == 0 || src_wpath == 0 || src_wpath[0] == L'\0')
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }
    //if (*dir_path != 0)
    //{
    //    return ERRCODE_INTERNAL_ERROR;
    //}
    *parent_dir_wpath = NULL;
        
    // Get directory
    res = init_fs_wpath(src_wpath, &fs_wpath);
    if (res != 0)
    {
        return res;
    }
    
    res = remove_last_witem_from_fs_wpath(&fs_wpath);
    if (res != 0)
    {
        free_fs_wpath(&fs_wpath);
        return res;
    }
    
    res = fs_wpath_to_wstring(&fs_wpath, parent_dir_wpath);
    free_fs_wpath(&fs_wpath);
    return res;
}

/*
 * Note! Caller is responsible for free pointer,
 *       returned in merged_path param.
 *
 *
 */
int create_absolute_normalized_wpath(
        const wchar_t* parent_wdir,
        const wchar_t* relative_wpath,
        wchar_t** merged_wpath)
{
    int res;
    st_fs_wpath fs_parent_wpath;
    st_fs_wpath fs_relative_wpath;
    
    if (parent_wdir == 0 || relative_wpath == 0 || merged_wpath == 0 ||
        (parent_wdir[0] == L'\0' && relative_wpath[0] == L'\0'))
    {
        return ERRCODE_WRONG_ARGUMENTS;
    }
    
    res = init_fs_wpath(parent_wdir, &fs_parent_wpath);
    if (res != 0)
    {
        return res;
    }

    res = init_fs_wpath(relative_wpath, &fs_relative_wpath);
    if (res != 0)
    {
        free_fs_wpath(&fs_parent_wpath);
        return res;
    }
    
    res = convert_fs_wpath_to_absolute(
            &fs_parent_wpath, 
            &fs_relative_wpath);
    if (res != 0)
    {
        free_fs_wpath(&fs_parent_wpath);
        free_fs_wpath(&fs_relative_wpath);
        
        return res;
    }
    
    res = fs_wpath_to_wstring(&fs_relative_wpath, merged_wpath);
    free_fs_wpath(&fs_parent_wpath);
    free_fs_wpath(&fs_relative_wpath);
    
    return res;
}
	
/*
 * @return: 1 if s2 is matched to s1 from right (s1 = "...s2"),
 *          otherwise 0
 */
static int
aux_wstr_right_match(
    const wchar_t* ws1, 
    const wchar_t* ws2, 
    unsigned char is_casesens)
{
    size_t i;
    size_t ws1_sz = wcslen(ws1);
    size_t ws2_sz = wcslen(ws2);
    
    if (ws2_sz > ws1_sz)
    {
        return 0;
    }
    
    for (i = 0; i < ws2_sz; ++i) 
    {
        if (is_casesens)
        {
            if (ws1[ws1_sz - i - 1] != ws2[ws2_sz - i - 1])
            {
                return 0;
            }
        }
        else 
        {
            if (towlower(ws1[ws1_sz - i - 1]) != towlower(ws2[ws2_sz - i - 1]))
            {
                return 0;
            }
        }
    }
    
    return 1;
}

/*
 * @return Success: 1 (pattern is found)
 *         Failure: 0 (pattern is not found)
 * empty pattern string means any match 
 */
static int 
aux_match_wpattern(
    const wchar_t* wpattern,
    PATTERN_MATCH_POSITION mpos,
    unsigned int mflags,
    const wchar_t* wdata)
{
    size_t wdata_sz;
    size_t wpattern_sz;
   
    // empty pattern string means any match 
    if (wpattern[0] == L'\0')
    {
      return 1;
    }
  
    switch (mpos) 
    {
        case PMP_MATCH_EXACT:
            wdata_sz = wcslen(wdata);
            wpattern_sz = wcslen(wpattern);
            
            if (wdata_sz != wpattern_sz)
            {
                return 0;
            }
            
            if (mflags & PMF_CASE_SENSITIVE)
            {
                return wcscmp(wdata, wpattern) == 0 ? 1 : 0;
            }
            else 
            {
                return mhlosi_wstrcasecmp(wdata, wpattern) == 0 ? 1 : 0;
            }
            break;    
        case PMP_MATCH_AT_ANY_PLACE:
            if (mflags & PMF_CASE_SENSITIVE)
            {
                return wcsstr(wdata, wpattern) == 0 ? 1 : 0;
            }
            else 
            {
                return mhlosi_wstrstr(wdata, wpattern) == 0 ? 1 : 0;
            }
            break; 
        case PMP_MATCH_FROM_BEGIN:
            wpattern_sz = wcslen(wpattern);
            wdata_sz = wcslen(wdata);
            if (wpattern_sz > wdata_sz)
            {
                return 0;
            }
            
            if (mflags & PMF_CASE_SENSITIVE)
            {
                return wcsncmp(wdata, wpattern, wpattern_sz) == 0 ? 1 : 0;
            }
            else 
            {
                return mhlosi_wstrncasecmp(wdata, wpattern, wpattern_sz) == 0 ? 1 : 0;
            }
            break;    
            
        case PMP_MATCH_AT_THE_END:
            return aux_wstr_right_match(
                    wdata, 
                    wpattern, 
                    mflags & PMF_CASE_SENSITIVE ? 1 : 0);
            
        default:
            return 0;
    }
}

/*
 * Note! Caller is responsible for free returned pointer,
 *       returned in merged_path param.
 *
 *
 */
int 
concat_wpath_parts(
  const wchar_t* part1,
  const wchar_t* part2,
  wchar_t** merged_wpath)
{
  size_t part1_sz;
  size_t part2_sz;
  size_t merged_sz;
  
  if (part1 == 0 || part1[0] == L'\0' || 
      part2 == 0 || part2[0] == L'\0' ||
      merged_wpath == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  part1_sz = wcslen(part1);
  part2_sz = wcslen(part2);
  merged_sz = part1_sz + part2_sz + 2; // 2 = WPATH_SEPARATOR + '\0'
  
  *merged_wpath = (wchar_t*) calloc(merged_sz, sizeof(wchar_t));
  if (*merged_wpath == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  wcsncpy(*merged_wpath, part1, part1_sz);
  
  if ((*merged_wpath)[part1_sz] != WPATH_SEPARATOR)                                                          
  {
    (*merged_wpath)[part1_sz] = WPATH_SEPARATOR;
  }
  
  wcsncat(*merged_wpath, part2, part2_sz);
  return 0;
}

#ifdef WIN
/* 
 * @return If entry type bit flag is matched to entry 
 *         type from dirent structure, result is 1.
 *         Otherwise 0 (dont matched)
 */
static int
aux_match_entry_types(unsigned int entry_types, WIN32_FIND_DATAW* p_wffd)
{
  // A directory 
  if ((entry_types & DETF_DIR) && 
      (p_wffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 
       FILE_ATTRIBUTE_DIRECTORY)
  {
    return 1;
  }

  // A regular file
  if ((entry_types & DETF_FILE) && 
      (p_wffd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
  {
    return 1;
  }
  
  return 0;
}

int 
search_entries_in_wdir(
  const wchar_t* wpath_to_dir, 
  const wchar_t* wpattern,
  PATTERN_MATCH_POSITION mpos,
  unsigned int mflags,
  unsigned int entry_types,
  st_conversion_settings* p_cs,
  void* data, // this data will be passed to fileproc_callback
  FileProcessingCallback fileproc_callback)
{
  int res;
  WIN32_FIND_DATAW wffd;
  wchar_t* corrected_wpath_to_wdir = 0;
  size_t wpath_to_dir_sz;
  HANDLE h_find = INVALID_HANDLE_VALUE;
  errno_t en = 0;
  
  if (wpath_to_dir == 0 || wpath_to_dir[0] == L'\0' || 
      wpattern == 0 || fileproc_callback == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  //
  // Prepare string for use with FindFile functions. 
  // Append "\*" to the path_to_dir
  //
  wpath_to_dir_sz = wcslen(wpath_to_dir);
  corrected_wpath_to_wdir = 
  (wchar_t*) calloc(wpath_to_dir_sz + 3, sizeof(wchar_t));
  
  if (corrected_wpath_to_wdir == NULL)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  // use more secure functions when possible
  en = 
    wcsncpy_s(
      corrected_wpath_to_wdir, 
      wpath_to_dir_sz + 2, 
      wpath_to_dir, 
      wpath_to_dir_sz);
  if (en != 0)
  {
    free(corrected_wpath_to_wdir);
    return ERRCODE_UNKNOWN_ERROR;
  }
  
  if (corrected_wpath_to_wdir[wpath_to_dir_sz - 1] != L'\\')
  {
    corrected_wpath_to_wdir[wpath_to_dir_sz++] = L'\\';
  }
  corrected_wpath_to_wdir[wpath_to_dir_sz] = '*';
  corrected_wpath_to_wdir[wpath_to_dir_sz + 1] = '\0';
  
  //
  h_find = FindFirstFileW(corrected_wpath_to_wdir, &wffd);
  if (h_find == INVALID_HANDLE_VALUE) 
  {
    free(corrected_wpath_to_wdir);
    return ERRCODE_NO_SUCH_FILE;
  } 
  
  for(;;)
  {
    if (aux_match_entry_types(entry_types, &wffd) && 
        aux_match_wpattern(wpattern, mpos, mflags, wffd.cFileName))
    {
      res = fileproc_callback(wffd.cFileName, data);      
      if (res != 0)
      {
        FindClose(h_find);
        free(corrected_wpath_to_wdir);
        
        return res == ERRCODE_STOP_SEARCH ? 0 : res;
      }
    }
    
    if (FindNextFileW(h_find, &wffd) == 0)
    {
      break;
    }
  }
  
  FindClose(h_find);
  free(corrected_wpath_to_wdir);
  return 0;  
}

static
int aux_process_entries_recurs(
  const wchar_t* wpath_to_dir,
  unsigned int entry_types,
  st_conversion_settings* p_cs,
  unsigned char stop_on_error,
  unsigned long* p_num_processed,
  unsigned long* p_num_failed,
  unsigned long* p_num_ok,
  void* data, // this data will be passed to fileproc_callback
  FileProcessingCallback fileproc_callback)
{
  int res;
  WIN32_FIND_DATAW wffd;
  wchar_t* corrected_wpath_to_wdir = 0;
  size_t wpath_to_dir_sz;
  wchar_t* entry_wpath;
  HANDLE h_find = INVALID_HANDLE_VALUE;
  errno_t en = 0;

  if (wpath_to_dir == 0 || wpath_to_dir[0] == L'\0')
  {
    return ERRCODE_UNKNOWN_ERROR;
  }

  //
  // Prepare string for use with FindFile functions.
  // Append "\*" to the path_to_dir
  //
  wpath_to_dir_sz = wcslen(wpath_to_dir);
  corrected_wpath_to_wdir =
  (wchar_t*) calloc(wpath_to_dir_sz + 3, sizeof(wchar_t));

  if (corrected_wpath_to_wdir == NULL)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  // use more secure functions when possible
  en =
    wcsncpy_s(
      corrected_wpath_to_wdir,
      wpath_to_dir_sz + 2,
      wpath_to_dir,
      wpath_to_dir_sz);
  if (en != 0)
  {
    free(corrected_wpath_to_wdir);
    return ERRCODE_UNKNOWN_ERROR;
  }

  if (corrected_wpath_to_wdir[wpath_to_dir_sz - 1] != L'\\')
  {
    corrected_wpath_to_wdir[wpath_to_dir_sz++] = L'\\';
  }
  corrected_wpath_to_wdir[wpath_to_dir_sz] = '*';
  corrected_wpath_to_wdir[wpath_to_dir_sz + 1] = '\0';

  //
  h_find = FindFirstFileW(corrected_wpath_to_wdir, &wffd);
  if (h_find == INVALID_HANDLE_VALUE)
  {
    free(corrected_wpath_to_wdir);
    return ERRCODE_NO_SUCH_FILE;
  }

  for(;;)
  {
    if (wcscmp(wffd.cFileName, L".") != 0 &&
        wcscmp(wffd.cFileName, L"..") != 0 )
    {
      res = concat_wpath_parts(wpath_to_dir, wffd.cFileName, &entry_wpath);
      if (res != 0)
      {
        FindClose(h_find);
        free(corrected_wpath_to_wdir);
        return res;
      }

      if (aux_match_entry_types(entry_types, &wffd))
      {
        res = fileproc_callback(entry_wpath, data);

        *p_num_processed += 1;

        if (res != 0)
        {
          *p_num_failed += 1;

          if (stop_on_error)
          {
            FindClose(h_find);
            free(corrected_wpath_to_wdir);
            free(entry_wpath);

            return res;
          }
        }
        else
        {
          *p_num_ok += 1;
        }
      }

      if (aux_match_entry_types(DETF_DIR, &wffd))
      {
        res = aux_process_entries_recurs(
          entry_wpath,
          entry_types,
          p_cs,
          stop_on_error,
          p_num_processed,
          p_num_failed,
          p_num_ok,
          data, // this data will be passed to fileproc_callback
          fileproc_callback);

        if (res != 0 && stop_on_error)
        {
          FindClose(h_find);
          free(corrected_wpath_to_wdir);
          free(entry_wpath);

          return res;
        }
      }

      free(entry_wpath);
    }

    if (FindNextFileW(h_find, &wffd) == 0)
    {
      break;
    }
  }

  FindClose(h_find);
  free(corrected_wpath_to_wdir);
  return 0;
}

#else
/* 
 * @return If entry type bit flag is matched to entry 
 *         type from dirent structure, result is 1.
 *         Otherwise 0 (isn't matched)
 */
static int
aux_match_entry_types(unsigned int entry_types, struct dirent* dent)
{
    // A regular file
    if ((entry_types & DETF_FILE) && dent->d_type == DT_REG)
    {
        return 1;
    }
    
    // A directory 
    if ((entry_types & DETF_DIR) && dent->d_type == DT_DIR)
    {
        return 1;
    }

    return 0;
}

static void
aux_print_ent_type(struct dirent* dent)
{
  switch(dent->d_type)
  {
    case DT_REG:
      print_ent_type(DETF_FILE);
      break;

    case DT_DIR:
      print_ent_type(DETF_DIR);
      break;

    case DT_BLK:
      print_ent_type(DETF_BLK);
      break;

    case DT_CHR:
      print_ent_type(DETF_CHR);
      break;

    case DT_FIFO:
      print_ent_type(DETF_FIFO);
      break;

    case DT_LNK:
      print_ent_type(DETF_LNK);
      break;

    case DT_SOCK:
      print_ent_type(DETF_SOCK);
      break;

    case DT_UNKNOWN:
    default:
      print_ent_type(DETF_UNK);
  }
}

int 
search_entries_in_wdir(
  const wchar_t* wpath_to_dir, 
  const wchar_t* wpattern,
  PATTERN_MATCH_POSITION mpos,
  unsigned int mflags,
  unsigned int entry_types,
  st_conversion_settings* p_cs,
  void* data, // this data will be passed to fileproc_callback
  FileProcessingCallback fileproc_callback)
{
  int res;
  DIR* dirp;
  struct dirent* dent;
  char* locpath_to_dir = 0;
  size_t locpath_to_dir_sz = 0;
  st_conversion_settings tmp_cs;
  wchar_t* wpath_to_direntry = 0;
  size_t wpath_to_direntry_sz = 0;
  
  
  unsigned char tmp_cs_inited = 0;
  
  if (wpath_to_dir == 0 || wpath_to_dir[0] == L'\0' || 
      wpattern == 0 || fileproc_callback == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
   
  if (p_cs == 0)
  {
    res = init_st_conversion_settings(&tmp_cs);
    if (res != 0)
    {
      return res;
    }
    
    p_cs = &tmp_cs;
    tmp_cs_inited = 1;
  }
  
  //TODO: check may be conversion to locale is needed
  res = 
    convert_from_wchar_to_utf8(
      wpath_to_dir,
      wcslen(wpath_to_dir),
      &locpath_to_dir,
      &locpath_to_dir_sz,
      p_cs);
  
  if (res != 0)
  {
    if (tmp_cs_inited)
    {
      free_st_conversion_settings(&tmp_cs);
      p_cs = 0;
    }
    return res;
  }
  
  dirp = opendir(locpath_to_dir);
  if (dirp == 0)
  {
    if (tmp_cs_inited)
    {
      free_st_conversion_settings(&tmp_cs);
      p_cs = 0;
    }
    free(locpath_to_dir);
    
    return ERRCODE_IO_ERROR;
  }
  
  while ((dent = readdir(dirp)) != NULL)
  {
    if (aux_match_entry_types(entry_types, dent) == 0)
    {
      continue;
    }
    
    res = 
      convert_composed_from_locale_to_wchar(
        dent->d_name,
        strlen(dent->d_name),
        &wpath_to_direntry,
        &wpath_to_direntry_sz,
        p_cs);
    
    if (res != 0)
    {
      if (tmp_cs_inited)
      {
        free_st_conversion_settings(&tmp_cs);
        p_cs = 0;
      }
      free(locpath_to_dir);
      closedir(dirp);
      
      return res;
    }
    
    if (aux_match_wpattern(wpattern, mpos, mflags, wpath_to_direntry) == 0)
    {
      free(wpath_to_direntry);
      continue;
    }
    
    res = fileproc_callback(wpath_to_direntry, data);
    if (res != 0)
    {
      if (tmp_cs_inited)
      {
        free_st_conversion_settings(&tmp_cs);
        p_cs = 0;
      }
      free(locpath_to_dir);
      free(wpath_to_direntry);
      
      closedir(dirp);
      
      return res == ERRCODE_STOP_SEARCH ? 0 : res;
    }
    
    free(wpath_to_direntry);
  }
  
  closedir(dirp);
  
  if (tmp_cs_inited)
  {
    free_st_conversion_settings(&tmp_cs);
    p_cs = 0;
  }
  free(locpath_to_dir);
  
  return 0;  
}

static
int aux_process_entries_recurs(
  const wchar_t* wpath_to_dir,
  unsigned int entry_types,
  st_conversion_settings* p_cs,
  unsigned char stop_on_error,
  unsigned long* p_num_processed,
  unsigned long* p_num_failed,
  unsigned long* p_num_ok,
  void* data, // this data will be passed to fileproc_callback
  FileProcessingCallback fileproc_callback)
{
  int res;
  DIR* dirp;
  struct dirent* dent;
  char* locpath_to_dir = 0;
  size_t locpath_to_dir_sz = 0;
  wchar_t* direntry_wname = 0;
  size_t direntry_wname_sz = 0;
  wchar_t* entry_wpath = 0;

  // no check for p_cs and fileproc_callback,
  // as this is checked in outer function

  if (wpath_to_dir == 0 || wpath_to_dir[0] == L'\0')
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  res =
    convert_from_wchar_to_utf8(
      wpath_to_dir,
      wcslen(wpath_to_dir),
      &locpath_to_dir,
      &locpath_to_dir_sz,
      p_cs);

  if (res != 0)
  {
    return res;
  }

  dirp = opendir(locpath_to_dir);
  if (dirp == 0)
  {
    free(locpath_to_dir);
    return ERRCODE_IO_ERROR;
  }

  while ((dent = readdir(dirp)) != NULL)
  {
    if (aux_match_entry_types(entry_types | DETF_DIR, dent) == 0)
    {
      fprintf(
        stderr,
        "Warning: the path is not a valid directory or file:\n%s/%s\n"
        "         ",
        locpath_to_dir, dent->d_name);

      aux_print_ent_type(dent);
      fprintf(stderr, " Ignoring...\n\n");

      continue;
    }

    res =
      convert_composed_from_locale_to_wchar(
        dent->d_name,
        strlen(dent->d_name),
        &direntry_wname,
        &direntry_wname_sz,
        p_cs);

    if (res != 0)
    {
      free(locpath_to_dir);
      closedir(dirp);
      return res;
    }
    
    if (wcscmp(direntry_wname, L".") == 0 ||
        wcscmp(direntry_wname, L"..") == 0 )
    {
      free(direntry_wname);
      continue;
    }
    
    res = concat_wpath_parts(wpath_to_dir, direntry_wname, &entry_wpath);
    if (res != 0)
    {
      free(locpath_to_dir);
      free(direntry_wname);
      closedir(dirp);
      
      return res;
    }
    
    if (aux_match_entry_types(entry_types, dent))
    {
      res = fileproc_callback(entry_wpath, data);
      *p_num_processed += 1;
      if (res != 0)
      {
        *p_num_failed += 1;
      }
      else
      {
        *p_num_ok += 1;
      }

      if (res != 0 && stop_on_error)
      {
        free(locpath_to_dir);
        free(direntry_wname);
        free(entry_wpath);
        closedir(dirp);

        return res;
      }
    }

    if (aux_match_entry_types(DETF_DIR, dent))
    {
      res = aux_process_entries_recurs(
        entry_wpath,
        entry_types,
        p_cs,
        stop_on_error,
        p_num_processed,
        p_num_failed,
        p_num_ok,
        data, // this data will be passed to fileproc_callback
        fileproc_callback);

      if (res != 0 && stop_on_error)
      {
        free(locpath_to_dir);
        free(direntry_wname);
        free(entry_wpath);
        closedir(dirp);

        return res;
      }
    }

    free(direntry_wname);
    free(entry_wpath);
  }

  closedir(dirp);
  free(locpath_to_dir);

  return 0;  
}
#endif //WIN

int process_files_recurs(
  const wchar_t* wpath_to_dir,
  st_conversion_settings* p_cs,
  unsigned char stop_on_error,
  unsigned long* p_num_processed,
  unsigned long* p_num_failed,
  unsigned long* p_num_ok,
  void* data, // this data will be passed to fileproc_callback
  FileProcessingCallback fileproc_callback)
{
  int res;
  unsigned int entry_types = DETF_FILE;
  unsigned long* num_processed;
  unsigned long* num_failed;
  unsigned long* num_ok;
  unsigned long tmp_processed = 0;
  unsigned long tmp_failed = 0;
  unsigned long tmp_ok = 0;

#ifndef WIN
  st_conversion_settings tmp_cs;
  unsigned char tmp_cs_inited = 0;
#endif

  if (fileproc_callback == 0)
  {
    return ERRCODE_INTERNAL_ERROR;
  }

  if (wpath_to_dir == 0 || wpath_to_dir[0] == L'\0')
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

#ifndef WIN
  if (p_cs == 0)
  {
    res = init_st_conversion_settings(&tmp_cs);
    if (res != 0)
    {
      return res;
    }

    p_cs = &tmp_cs;
    tmp_cs_inited = 1;
  }
#endif

  if (p_num_processed != NULL)
  {
    num_processed = p_num_processed;
  }
  else
  {
    num_processed = &tmp_processed;
  }

  if (p_num_failed != NULL)
  {
    num_failed = p_num_failed;
  }
  else
  {
    num_failed = &tmp_failed;
  }

  if (p_num_ok != NULL)
  {
    num_ok = p_num_ok;
  }
  else
  {
    num_ok = &tmp_ok;
  }

  res = aux_process_entries_recurs(
        wpath_to_dir,
        entry_types,
        p_cs,
        stop_on_error,
        num_processed,
        num_failed,
        num_ok,
        data, // this data will be passed to fileproc_callback
        fileproc_callback);

#ifndef WIN
  if (tmp_cs_inited)
  {
    free_st_conversion_settings(&tmp_cs);
    p_cs = 0;
  }
#endif

  return res;
}
