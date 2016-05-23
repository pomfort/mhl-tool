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
#include <string.h>
#include <wchar.h>

#include <generics/os_check.h>
#ifdef WIN
#include <Windows.h>
#include <direct.h>
#endif 

#include <facade_info/error_codes.h>
#include <generics/filesystem_handlers/public_interface.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/file_path_decomposition.h>

#ifdef WIN

// Absolute long path prefix for windows
#define LONG_ABSOLUTE_ROOT_PATH_STR L"\\\\?\\"

// this not native separator is used by MacOS and Linux
#define WPATH_NOT_NATIVE_SEPARATOR L'/'

/** This function is introduced mainly for windows.
 *  Windows supports 2 kinds of path separators "/" and "\"
 *  This function converts all paths separators to WPATH_SEPARATOR ("\").
 *  On linux and MacOS this function does nothing.
 */
void make_wpath_os_specific(wchar_t* wpath)
{
  if (wpath == 0 || *wpath == L'\0')
  {
    // do nothing in case of error
    return;
  }

  for(; *wpath != L'\0'; ++wpath)
  {
    if (*wpath == WPATH_NOT_NATIVE_SEPARATOR)
    {
      *wpath = WPATH_SEPARATOR;
    }
  }
}

/**
 * @returns: 
 *   if path contains root path prefix ("C:", "d:\"), 
 *   then returns size of path prefix
 *   if path does not contains root path prefix, then returns 0
 */
unsigned char
is_root_wpass_prefix(const wchar_t* wpath, size_t path_sz)
{
    wchar_t* os_specific_wpath = 0;    
    const wchar_t* wpath_ptr = 0;
    unsigned char path_prefix_sz = 0;
    
    if (wpath == 0 || path_sz < 2)
    {
        return 0;
    }
    
    os_specific_wpath = mhlosi_wstrndup(wpath, path_sz);
    if (os_specific_wpath == 0)
    {
      // no memory, what we can do here return 0
      return 0;
    }

    make_wpath_os_specific(os_specific_wpath);

    //LONG_ABSOLUTE_ROOT_PATH_STR

    if (mhlosi_wstrcmp(os_specific_wpath, LONG_ABSOLUTE_ROOT_PATH_STR) == 0)
    {
      path_prefix_sz = (unsigned char) wcslen(LONG_ABSOLUTE_ROOT_PATH_STR);
      wpath_ptr = os_specific_wpath + path_prefix_sz;
    }
    else
    {
      wpath_ptr = os_specific_wpath;
    }

    if (!iswalpha(wpath_ptr[0]))
    {
        free(os_specific_wpath);
        return path_prefix_sz;
    }
    
    if (wpath_ptr[1] != L':')
    {
        free(os_specific_wpath);
        return path_prefix_sz;
    }
    
    path_prefix_sz += 2;
    wpath_ptr += 2;
    
    if (*wpath_ptr == L'\0')
    {
        free(os_specific_wpath);
        return path_prefix_sz;
    }

    if (*wpath_ptr == WPATH_SEPARATOR)
    {
      path_prefix_sz += 1;        
    }
    
    free(os_specific_wpath);
    return path_prefix_sz;
}

static unsigned char
are_wpaths_items_equal(const wchar_t* wpil, const wchar_t* wpir)
{
    if (wpil == 0 || wpir == 0)
    {
        return 0;
    }
    
    while (*wpil != L'\0' && *wpir != L'\0')
    {
        if (toupper(*wpil) != toupper(*wpir))
        {
            return 0;
        }
        
        ++wpil;
        ++wpir;
    } 
    
    return *wpil != *wpir ? 0 : 1;
}

#else
// Linux, MAC OS

// this not native separator is used by Windows
#define WPATH_NOT_NATIVE_SEPARATOR L'\\'

void make_wpath_os_specific(wchar_t* wpath)
{
  return;
}

/**
 * @returns: 
 *   if path contains root path prefix ("/"), 
 *   then returns size of path prefix, it is always 1
 *   if path does not contain root path prefix, then returns 0
 */
unsigned char
is_root_wpass_prefix(const wchar_t* wpath, size_t wpath_sz)
{
    if (wpath == 0 || wpath_sz  == 0)
    {
        return 0;
    }
    
    return wpath[0] == L'/' ? 1 : 0;
}

unsigned char
are_wpaths_items_equal(const wchar_t* wpil, const wchar_t* wpir)
{
    if (wpil == 0 || wpir == 0)
    {
        return 0;
    }
    
    return wcscmp(wpil, wpir) == 0 ? 1 : 0;
}

#endif

int 
add_sz_witem_to_fs_wpath(
  const wchar_t* wnm, 
  size_t wnm_sz, 
  st_fs_wpath* p_wpath)
{
    wchar_t**  new_witems;
    size_t witems_cnt;
    
    if (wnm == 0 || wnm_sz == 0 || p_wpath == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    witems_cnt = p_wpath->items_cnt;
    new_witems = (wchar_t**) realloc(p_wpath->items, (witems_cnt + 1) * sizeof(wchar_t*));
    if (new_witems == 0)
    {
        return ERRCODE_OUT_OF_MEM;
    } 
    p_wpath->items = new_witems;
    
    p_wpath->items[witems_cnt] = (wchar_t*) calloc(wnm_sz + 1, sizeof(wchar_t));
    if (p_wpath->items[witems_cnt] == 0)
    {
        return ERRCODE_OUT_OF_MEM;
    }
    // We already have L'\0' at the end of string due to calloc
    wcsncpy(p_wpath->items[witems_cnt], wnm, wnm_sz);
    p_wpath->items_cnt = witems_cnt+1;
    
    return 0;  
}

int 
add_witem_to_fs_wpath(const wchar_t* wnm, st_fs_wpath* p_wpath)
{
    size_t sz;
    
    if (wnm == 0 || p_wpath == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    sz = wcslen(wnm);
    if (sz == 0)
    {
        return 0;
    }
    
    return add_sz_witem_to_fs_wpath(wnm, sz, p_wpath);
}

int 
remove_last_witem_from_fs_wpath(st_fs_wpath* p_wpath)
{
    if (p_wpath == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    if (p_wpath->items_cnt == 0)
    {
        return ERRCODE_WRONG_FILE_LOCATION;
    }
    
    free(p_wpath->items[p_wpath->items_cnt-1]);
    --p_wpath->items_cnt;
    return 0;
}


int init_fs_wpath(const wchar_t* wpath, st_fs_wpath* p_fs_wpath)
{
    int res;
    unsigned char root_prefix_len;
    const wchar_t* wptr;
    size_t wsz;
    
    if (p_fs_wpath == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    memset(p_fs_wpath, 0, sizeof(st_fs_wpath) / sizeof(char));
    wsz = wcslen(wpath);
    root_prefix_len = is_root_wpass_prefix(wpath, wsz);
    if (root_prefix_len)
    {
      p_fs_wpath->is_absolute = 1;

      // if length of root prefix more than 1 wide character or 
      //    last character in the root prefix is not parth separartor, 
      // then it will be added to st_fs_wpath
      // Only windows root paths ("C:\" or "\\?\C:" are fit in this category
      // If last character in the path prefix is filepasth separator, 
      // it will be skipped.
      // Later when path string will be constructed from content of 
      // st_fs_wpath structure, skipped filepath separator, will be
      // added authomatically during filepath string construction
      if (wpath[root_prefix_len - 1] != WPATH_SEPARATOR || root_prefix_len > 1)
      {
        res = 
          add_sz_witem_to_fs_wpath(
            wpath, 
            wpath[root_prefix_len - 1] != WPATH_SEPARATOR ? root_prefix_len : root_prefix_len - 1, 
            p_fs_wpath);

        if (res != 0)
        {
          free_fs_wpath(p_fs_wpath);
          return res;
        }
      }

      wpath += root_prefix_len;
    }
    
    wptr = wcschr(wpath, WPATH_SEPARATOR);
    while (wptr)
    {
        wsz = (size_t) (wptr - wpath);
        if (wsz != 0)
        {
            res = add_sz_witem_to_fs_wpath(wpath, wsz, p_fs_wpath);
            if (res != 0)
            {
              free_fs_wpath(p_fs_wpath);
              return res;
            }
        }
        wpath += wsz + 1;
        wptr = wcschr(wpath, WPATH_SEPARATOR);
    }
    
    res = 0;
    if (*wpath != L'\0')
    {
        res = add_witem_to_fs_wpath(wpath, p_fs_wpath);
        if (res != 0)
        {
          free_fs_wpath(p_fs_wpath);
        }
    }
    
    return res;
}

void free_fs_wpath(st_fs_wpath* p_fs_wpath)
{
  size_t i;
  if (p_fs_wpath == 0)
  {
    return;
  }
    
  for (i = 0; i < p_fs_wpath->items_cnt; ++i)
  {
    free(p_fs_wpath->items[i]);
  }
  free(p_fs_wpath->items);
  memset(p_fs_wpath, 0, sizeof(st_fs_wpath) / sizeof(char));
}

/*
 * Add path to destination, 
 * added path will be normalized during adding process.
 */
int add_normalized(st_fs_wpath* p_src_wpath, st_fs_wpath* p_dst_wpath)
{
    int res;
    size_t i;
    
    for (i = 0; i < p_src_wpath->items_cnt; ++i)
    {
        if (p_src_wpath->items[i] == 0 ||
            p_src_wpath->items[i][0] == L'\0' ||
            wcscmp(p_src_wpath->items[i], L".") == 0)
        {
            continue;
        }
        
        if (wcscmp(p_src_wpath->items[i], L"..") == 0)
        {
            res = remove_last_witem_from_fs_wpath(p_dst_wpath);
            if (res != 0)
            {
                return res;
            }
        }
        else 
        {     
            res = add_witem_to_fs_wpath(p_src_wpath->items[i], p_dst_wpath);
            if (res != 0)
            {
                return res;
            }
        }
    }
    
    return 0;
}

int convert_fs_wpath_to_absolute(
  st_fs_wpath* p_base_wdir, 
  st_fs_wpath* p_fs_wpath)
{
    int res;
    st_fs_wpath merged_fs_wpath;
    
    if (p_fs_wpath && p_fs_wpath->is_absolute)
    {
        // target path is already absolute
        return 0;
    }
    
    if (p_base_wdir == 0 &&  p_fs_wpath == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    if (p_base_wdir->is_absolute == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    // Create and fill temporary merged_fs_path
    memset(&merged_fs_wpath, 0, sizeof(st_fs_wpath) / sizeof(char));
    
    res = add_normalized(p_base_wdir, &merged_fs_wpath);
    if (res != 0)
    {
        free_fs_wpath(&merged_fs_wpath);
        return res;
    }
    
    res = add_normalized(p_fs_wpath, &merged_fs_wpath);
    if (res != 0)
    {
        free_fs_wpath(&merged_fs_wpath);
        return res;
    }
    
    // move merged_fs_path into fs_path
    free_fs_wpath(p_fs_wpath);
    p_fs_wpath->is_absolute = 1;
    p_fs_wpath->is_normalized = 1;
    p_fs_wpath->items = merged_fs_wpath.items;
    p_fs_wpath->items_cnt = merged_fs_wpath.items_cnt;
    
    return 0;
}

int normalize_fs_wpath(st_fs_wpath* p_fs_wpath)
{
    unsigned char is_absolute;
    int res;
    st_fs_wpath merged_fs_wpath;
    
    if (p_fs_wpath == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    if (p_fs_wpath->is_normalized)
    {
        return 0;
    }
    
    // Create and fill temporary merged_fs_path
    memset(&merged_fs_wpath, 0, sizeof(st_fs_wpath) / sizeof(char));
    
    res = add_normalized(p_fs_wpath, &merged_fs_wpath);
    if (res != 0)
    {
        return res;
    }
    
    // swap fs_path objects
    is_absolute = p_fs_wpath->is_absolute;
    free_fs_wpath(p_fs_wpath);
    p_fs_wpath->is_absolute = is_absolute;
    p_fs_wpath->is_normalized = 1;
    p_fs_wpath->items = merged_fs_wpath.items;
    p_fs_wpath->items_cnt = merged_fs_wpath.items_cnt;
    
    return 0;
}

/**
 * @return If p_fs_base is subpath of p_fs_check, then rerurn 1
 If p_fs_base is not subpath of p_fs_check, then rerurn 0
 */
unsigned char
is_nested_fs_wpath(st_fs_wpath* p_fs_wbase, st_fs_wpath* p_fs_wcheck)
{
    unsigned int i;
    if (p_fs_wbase == 0 || p_fs_wcheck == 0 ||
        p_fs_wbase->is_absolute == 0 || p_fs_wbase->is_normalized == 0 ||
        p_fs_wcheck->is_absolute == 0 || p_fs_wcheck->is_normalized == 0)
    {
        return 0;
    }
    
    if (p_fs_wcheck->items_cnt < p_fs_wbase->items_cnt)
    {
        return 0;
    }
    
    for (i = 0; i < p_fs_wbase->items_cnt; ++i)
    {
        if (!are_wpaths_items_equal(
                p_fs_wbase->items[i], p_fs_wcheck->items[i]))
        {
            return 0;
        }   
    }
    
    return 1;
}


int
fs_wpath_to_wstring(st_fs_wpath* p_rel_wpath, wchar_t** p_file_rel_wpath)
{
  size_t wbuf_len = 0;
  size_t i;
  size_t wln;
  wchar_t* wpointer;
  wchar_t** witems = p_rel_wpath->items;
  
  if (p_file_rel_wpath == NULL)
  {
    //fprintf(stderr, "Buffer for filename is not empty.\n");
    return ERRCODE_INTERNAL_ERROR;
  }
  
  if (p_rel_wpath->items_cnt == 0)
  {
    //fprintf(stderr, "Empty relative path to file.\n");
    return ERRCODE_INTERNAL_ERROR;
  }
  
  if (p_rel_wpath->is_absolute)
  {
    wbuf_len = 1;
  }
  
  for (i=0; i < p_rel_wpath->items_cnt; ++i)
  {
    wln = wcslen(witems[i]);
    if (wln == 0)
    {
      //fprintf(
      //    stderr, 
      //    "Unknown internal error: empty directory or file name "
      //    "in path.\n");
      return ERRCODE_INTERNAL_ERROR;
    }
    wbuf_len += wln + 1; // add 1 symbol for PATH_SEPARATOR or 
    // '\0' at the end
  }
  
  *p_file_rel_wpath = (wchar_t*)calloc(wbuf_len, sizeof(wchar_t));
  if (*p_file_rel_wpath == NULL)
  {
    //fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }
  
  
  // Construct relative path to file as a one string 
  wpointer = *p_file_rel_wpath;

#ifndef WIN
  // In Linux and MacOSX root path inital slash is not in "items" list
  if (p_rel_wpath->is_absolute)
  {
    *wpointer = WPATH_SEPARATOR;
    ++wpointer;
  }
#endif
  
  for (i=0; i < p_rel_wpath->items_cnt - 1; ++i)
  {
    wln = wcslen(witems[i]);
    if (wln)
    {
      wcsncpy(wpointer, witems[i], wln);
      
      wpointer[wln] = WPATH_SEPARATOR;
      wpointer += wln + 1;
    }
  }
  
  wln = wcslen(witems[i]);
  wcsncpy(wpointer, witems[i], wln);
  // We already have '\0' at the end of line due to calloc
  
  return 0;
}

//
// Paths p_base_path and p_file_path must be absolute
int
extract_relative_fs_wpath(
  st_fs_wpath* p_base_wpath, 
  st_fs_wpath* p_file_wpath, 
  st_fs_wpath* p_rel_wpath)
{
    size_t i;
    int res;
    
    if (p_base_wpath == 0 || p_file_wpath == 0 || p_rel_wpath == 0)
    {
        return ERRCODE_INTERNAL_ERROR;
    }
    
    if (!is_nested_fs_wpath(p_base_wpath, p_file_wpath))
    {
        return ERRCODE_WRONG_FILE_LOCATION;
    }
    
    for (i = p_base_wpath->items_cnt; i < p_file_wpath->items_cnt; ++i)
    {
        res = add_witem_to_fs_wpath(p_file_wpath->items[i], p_rel_wpath);
        if (res != 0)
        {
            return res;
        }
    }
    
    return 0;
}

int
extract_relative_wpath(
  st_fs_wpath* p_base_wpath, 
  st_fs_wpath* p_file_wpath,
  wchar_t** p_file_rel_wpath)
{
    int res;
    st_fs_wpath rel_wpath;
    
    memset(&rel_wpath, 0, sizeof(rel_wpath) / sizeof(char));
    
    res = extract_relative_fs_wpath(p_base_wpath, p_file_wpath, &rel_wpath);
    if (res != 0)
    {
      return res;
    }
    
    res = fs_wpath_to_wstring(&rel_wpath, p_file_rel_wpath);
    if (res != 0)
    {
      return res;
    }
    
    free_fs_wpath(&rel_wpath);
    
    return 0;
}

/* Note! Caller is responsible for freeing allocated string 
 *
 * Create path string from first n items of st_fs_wpath.
 * String with path will be allocated and stored in p_wpath.
 */
int
fs_wspath_first_nitems_to_str(
  st_fs_wpath* p_fs_wpath,
  int first_n,
  wchar_t** p_dst_wpath)
{
  size_t i;
  size_t wln;
  size_t dst_wpath_sz;
  int real_n;
  wchar_t** witems;
  wchar_t* wpointer;
  unsigned char is_item_added = 0;
  
  if (p_fs_wpath == 0 || p_dst_wpath == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  // calculate requires size of destination string
  real_n = p_fs_wpath->items_cnt > (size_t) first_n ? (size_t) first_n : p_fs_wpath->items_cnt;
  dst_wpath_sz = 0;
  for (i = 0; i < (size_t) real_n; ++i)
  {
    dst_wpath_sz += wcslen(p_fs_wpath->items[i]);
  }
  dst_wpath_sz += (real_n + 2); 
  
  // alloacte wstring
  *p_dst_wpath = calloc(dst_wpath_sz, sizeof(wchar_t));
  if (*p_dst_wpath)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  // construct answer
  witems = p_fs_wpath->items;
  wpointer = *p_dst_wpath;
  
#ifndef WIN  
  // In Linux and MacOSX root path inital slash is not in "items" list
  if (p_fs_wpath->is_absolute)
  {
    *wpointer = WPATH_SEPARATOR;
    ++wpointer;
  }
#endif
  
  for (i = 0; i < (size_t) real_n; ++i)
  {    
    wln = wcslen(witems[i]);
    if (wln)
    {
      wcsncpy(wpointer, witems[i], wln);
      wpointer[wln] = WPATH_SEPARATOR;
      wpointer += wln + 1;
      is_item_added = 1;
    }
  }
  
  // remove last WPATH_SEPARATOR
  if (is_item_added)
  {
    *(wpointer - 1) = L'\0';
  }
  
  return 0;
}

/* 
 * Create st_fs_wpath structure started from n-th item of p_fs_wpath.
 */
int
fs_wspath_from_nitems_to_fs_wspath(
  st_fs_wpath* p_fs_wpath,
  int from_n,
  st_fs_wpath* p_rest_wpath)
{ 
  int res;
  size_t i;
  
  if (p_fs_wpath == 0 || p_rest_wpath == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  memset(p_rest_wpath, 0, sizeof(*p_rest_wpath) / sizeof(char));
  if ((size_t) from_n >= p_fs_wpath->items_cnt)
  {
    // nothing to copy from_n is beyond the border
    return 0; 
  }
  
  for (i = from_n; i < p_fs_wpath->items_cnt; ++i)
  {
    res = 
      add_sz_witem_to_fs_wpath(
        p_fs_wpath->items[i], 
        wcslen(p_fs_wpath->items[i]),
        p_rest_wpath);
    
    if (res != 0)
    {
      free_fs_wpath(p_rest_wpath);
      return res;
    }
  }
  
  return 0;
}

