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
#ifndef _MHL_TOOLS_GENERICS_FILESYSTEM_HANDLERS_PUBLIC_INTERFACE_H_
#define _MHL_TOOLS_GENERICS_FILESYSTEM_HANDLERS_PUBLIC_INTERFACE_H_

#include <sys/stat.h>
#include <stdlib.h>

#include <generics/os_check.h>
#include <generics/char_conversions.h>

/*****************************************************************************
 * File path operations
 *****************************************************************************/

#ifdef WIN
// OS specific filepath separator
#define WPATH_SEPARATOR L'\\'

#else // Linux, Mac OS X
// OS specific filepath separator
#define WPATH_SEPARATOR L'/'

#endif

/* This function changes file path 
 * seperators to os specific separators. 
 * In other words it changes all separators WPATH_NOT_NATIVE_SEPARATOR
 * to WPATH_SEPARATOR
 *
 * @param wpath null terminated wchar buffer with path inside
 */
void make_wpath_os_specific(wchar_t* wpath);

// this filepath separator is used in mhl files
#define WPATH_UNIFORM_SEPARATOR L'/'

/* This function changes file path 
 * seperators from os specific path separators to uniform path separators
 * 
 * In other words it changes all separators from WPATH_SEPARATOR
 * to WPATH_UNIFORM_SEPARATOR
 *
 * @param wpath null terminated wchar buffer with path inside
 */
void make_wpath_uniform(wchar_t* wpath);

typedef struct _st_fs_wpath
{
    unsigned char is_absolute;
    unsigned char is_normalized;
    wchar_t**     items;
    size_t        items_cnt;
} st_fs_wpath;

int init_fs_wpath(
    const wchar_t* wpath, 
    st_fs_wpath* p_fs_wpath);

void free_fs_wpath(st_fs_wpath* p_fs_wpath);

/*
 * result is absolute path p_base_dir + p_fs_path, normalized 
 * (".." and "." are converted to path).
 * result will be stored in p_fs_path
 */
int convert_fs_wpath_to_absolute(
        st_fs_wpath* p_base_wdir,
        st_fs_wpath* p_fs_wpath);

int normalize_fs_wpath(st_fs_wpath* p_fs_wpath);

int
extract_relative_wpath(
  st_fs_wpath* p_base_wpath, 
  st_fs_wpath* p_file_wpath,
  wchar_t** p_file_rel_wpath);

// Note! Caller is responsible for free returned pointer
wchar_t* get_wworkdir(st_conversion_settings* pcs);

/*
 * Note! Caller is responsible for free returned pointer,
 *       returned in merged_path param.
 * Relative wpath must be against working directory, as this function uses it
 * for making the absolute path
 */
int convert_to_absolute_normalized_wpath(
  const wchar_t* relative_wpath,
  wchar_t** abs_wpath,
  st_conversion_settings* p_cs);

/*
 * Note! Caller is responsible for free returned pointer,
 *       returned in merged_path param.
 *
 *
 */
int create_absolute_normalized_wpath(
    const wchar_t* parent_wdir,
    const wchar_t* relative_wpath,
    wchar_t** merged_wpath);

/*
 * Note! Caller is responsible for free pointer, returned in merged_path param.
 */
int concat_wpath_parts(
  const wchar_t* part1,
  const wchar_t* part2,
  wchar_t** merged_wpath);

/* Compare are filenames equal.
 *
 * @return: 1 if filenames are equal, 0 if filenames are not equal.
 */
unsigned char does_wfilenames_equal(
  const wchar_t* fn1,
  const wchar_t* fn2);

/* 
 * Note! Caller is responsible for free returned pointer,
 *       returned in dir_path param.
 * Extracts dir fom passed filename.
 * 
 * @param fpath Source filename
 * @param dir_path directory will be placed here
 * @return Success 0, Failure error code > 0
 */
int extract_wdir_from_wpath(
    const wchar_t* src_wpath, 
    wchar_t** parent_dir_wpath);

/*****************************************************************************
 * File and directory operations
 *****************************************************************************/

unsigned char does_wpath_exist(const wchar_t* wpath);

/* The same functiomnality as fopen from C stdlib
 *
 * On windows in order to open binary file "b" param should be added
 */
FILE* fwopen_for_hash_create(const wchar_t* wfilename);

/* The same functiomnality as fopen from C stdlib
 *
 * On windows in order to open binary file "b" param should be added
 */
FILE* fwopen_for_hash_check(const wchar_t* wfilename);

/* The same functionality as open from C stdlib
 *
 * On windows in order to open binary _wsopen_s should be used
 */
int wopen_for_read(const wchar_t* wfn, int* p_fd);

/* The same functiomnality as close from C stdlib
 *
 * On windows in order to open binary _close should be used
 */
int mhlosi_close(int fd);

/* Gets file size. 
 *
 * @return: Success: 0, Error: non zero value with error code
 */
int get_wfile_size(const wchar_t* wfn, unsigned long long* p_fsz);

typedef struct _st_mhlosi_stat
{
#ifdef WIN
  struct _stati64 st_data;
#elif defined MAC_OS_X
  struct stat st_data;
#else
  struct stat64 st_data;
#endif 
} st_mhlosi_stat;

/* Gets file stat mode. 
 *
 * @return: Success: 0, Error: non zero value with error code
 */
int get_wfile_stat_data(
  const wchar_t* wpath, st_mhlosi_stat* stat_data);

/*
 * 
 */
typedef enum _PATTERN_MATCH_POSITION
{
    PMP_MATCH_EXACT        = 0,
    PMP_MATCH_FROM_BEGIN   = 1,
    PMP_MATCH_AT_THE_END   = 2,
    PMP_MATCH_AT_ANY_PLACE = 3
} PATTERN_MATCH_POSITION;

typedef enum _PATTERN_MATCHING_FLAGS
{
    PMF_CASE_SENSITIVE     = 0x1,
    PMF_CASE_INSENSITIVE   = 0x2
} PATTERN_MATCHING_FLAGS;

typedef enum _DIR_ENTRY_TYPE_FLAGS
{
    DETF_FILE = 0x1,
    DETF_DIR  = 0x2,
    DETF_BLK  = 0x4,
    DETF_CHR  = 0x8,
    DETF_FIFO = 0x10,
    DETF_LNK  = 0x20,
    DETF_SOCK = 0x40,
    DETF_UNK  = 0x80,
} DIR_ENTRY_TYPE_FLAGS;

/*
 * Callback, called when file entity is found
 * If this function returns:
 * 0 - success, continue search
 * not null - means error in callback (see error_codes.h for description), 
 * stop search. If just stop search needed, 
 * return ERRCODE_STOP_SEARCH from callback
 */
typedef int (*FileProcessingCallback)(const wchar_t* file_wname, void* data);

/*
 * Search entries in given dir. Entries should match with given pattern.
 * Empty pattern string means any match
 * Found entry names are passed to callback function together with data.
 * Entry name is passed "as is", it is not prefixed with directory name.
 *
 * @param wpath_to_dir - path to directory, where search will be performed.
 * @param wpattern - matching pattern, empty pattern string means any match
 * @param mpos - matching position
 * @param mflags - bit combination of matching flags 
 *                 (PATTERN_MATCHING_FLAGS). 
 * @param entry_types - bit combination of DIR_ENTRY_TYPE_FLAGS. 
 *                      It shows what should be found.
 * @param p_cs - pointer to chars conversion settings
 * @param data - pointer to data structure, which will be passed to callback
 *               function
 * @param fileproc_callback - callback function
 *
 * @return Success: 0 (no macthed entries is success). 
 *         Failure: Non zero value with error code.
 */
int search_entries_in_wdir(
  const wchar_t* wpath_to_dir, 
  const wchar_t* wpattern,
  PATTERN_MATCH_POSITION mpos,
  unsigned int mflags,
  unsigned int entry_types,
  st_conversion_settings* p_cs,
  void* data, // this data will be passed to fileproc_callback
  FileProcessingCallback fileproc_callback);

#define MHL_FILE_PATTERN ".mhl"
#define MHL_FILE_WPATTERN L".mhl"

unsigned char is_directory(const wchar_t* wpath);
unsigned char is_regular_file(const wchar_t* wpath);
int get_file_type(const wchar_t* wpath, DIR_ENTRY_TYPE_FLAGS* t_flag);
void print_ent_type(DIR_ENTRY_TYPE_FLAGS ent_type);

/*
 * Search and procerss regular files entries in given dir.
 * Callback function is called for each file.
 * Full path name for each file is passed to callback function
 * together with data.
 *
 * @param wpath_to_dir - path to directory, where search will be performed.
 * @param p_cs - pointer to chars conversion settings
 * @param stop_on_error - any non zero value indicates, that the further files
 *                        search and processing must be stopped after a first
 *                        error, indicated by non-zero return code of
 *                        callback function
 * @param p_num_processed - [in-out] parameter - number of processed files,
 *                          increased from initial value
 * @param p_num_failed - [in-out] parameter - number of files, for which callback
 *                       returned error (increased from initial value)
 * @param p_num_ok - [in-out] parameter - number of files, for which callback
 *                   returned 0 (increased from initial value)
 * @param data - pointer to data structure, which will be passed to callback
 *               function
 * @param fileproc_callback - callback function
 *
 * @return Success: 0 (no matched entries is success).
 *         Failure: Non zero value with error code.
 */
int process_files_recurs(
  const wchar_t* wpath_to_dir,
  st_conversion_settings* p_cs,
  unsigned char stop_on_error,
  unsigned long* p_num_processed, // may be NULL if not needed
  unsigned long* p_num_failed, // may be NULL if not needed
  unsigned long* p_num_ok, // may be NULL if not needed
  void* data, // this data will be passed to fileproc_callback
  FileProcessingCallback fileproc_callback);

#endif //_MHL_TOOLS_GENERICS_FILESYSTEM_HANDLERS_PUBLIC_INTERFACE_H_
