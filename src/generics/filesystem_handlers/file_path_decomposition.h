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

#ifndef _MHL_TOOLS_GENERICS_FILESYSTEM_HANDLERS_FILE_PATH_DECOMPOSITION_H_
#define _MHL_TOOLS_GENERICS_FILESYSTEM_HANDLERS_FILE_PATH_DECOMPOSITION_H_

#include <ctype.h>
#include <generics/filesystem_handlers/public_interface.h>
//
// In all function from this file _sz means count of wchar_t symbols
//

/**
 * @returns: 
 *   if path contains root path prefix (MacOS, Linux "/"; 
 *                                      Windows" "C:", "d:\"), 
 *   then returns size of path prefix, (it is always 1 on MacOS, Linux)
 *   if path does not contain root path prefix, then returns 0
 */
unsigned char
is_root_wpass_prefix(const wchar_t* wpath, size_t wpath_sz);

/**
 * @returns: 0 if paths are not equal, othervize 1
 *
 */
unsigned char
are_wpaths_items_equal(const wchar_t* wpil, const wchar_t* wpir);

int 
add_sz_witem_to_fs_wpath(
    const wchar_t* wnm, 
    size_t wnm_sz, 
    st_fs_wpath* p_wpath);

int 
add_witem_to_fs_wpath(
    const wchar_t* wnm, 
    st_fs_wpath* p_wpath);

int 
remove_last_witem_from_fs_wpath(
    st_fs_wpath* p_wpath);

int add_normalized(st_fs_wpath* p_src_wpath, st_fs_wpath* p_dst_wpath);

/**
 * @return If p_fs_base is subpath of p_fs_check, then rerurn 1
 If p_fs_base is not subpath of p_fs_check, then rerurn 0
 */
unsigned char
is_nested_fs_wpath(st_fs_wpath* p_wfs_base, st_fs_wpath* p_wfs_check);

int
fs_wpath_to_wstring(st_fs_wpath* p_rel_wpath, wchar_t** p_file_rel_wpath);

//
// Paths p_base_path and p_file_path must be absolute
int
extract_relative_fs_wpath(
  st_fs_wpath* p_base_wpath, 
  st_fs_wpath* p_file_wpath, 
  st_fs_wpath* p_rel_wpath);

/* Note! Caller is responsible for freeing allocated string 
 *
 * Create path string from first n items of st_fs_wpath.
 * String with path will be allocated and stored in p_wpath.
 */
int
fs_wspath_first_nitems_to_str(
  st_fs_wpath* p_fs_wpath,
  int first_n,
  wchar_t** p_wpath);

/* Create st_fs_wpath structure started from n-th item of p_fs_wpath.
 */
int
fs_wspath_from_nitems_to_fs_wspath(
  st_fs_wpath* p_fs_wpath,
  int from_n,
  st_fs_wpath* p_rest_wpath);

#endif //_MHL_TOOLS_GENERICS_FILESYSTEM_HANDLERS_FILE_PATH_DECOMPOSITION_H_
