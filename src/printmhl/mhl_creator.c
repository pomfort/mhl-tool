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
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

#include <facade_info/version.h>
#include <facade_info/error_codes.h>
#include <generics/os_check.h>
#include <generics/char_conversions.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/memory_management.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/files_data.h>
#include <mhltools_common/logging.h>
#include <mhltools_common/usage_printing.h>

#include <printmhl/print_mhl.h>
#include <printmhl/create_mhl_files_data.h>
#include <printmhl/mhl_creator.h>


#define TIME_STR_SZ 21
#define MHLNAME_TIME_SUBSTR_LEN 17
#define MHLNAME_END_STR ".mhl"
#define MHLNAME_END_WSTR L".mhl"

#define DEFAULT_MHL_FILE_NAME "media-hash-list-file.mhl"


//
// Set of functions for working with directories
//

void finalize_mhlcreate_data(st_mhlcreate_data* data)
{
  unsigned int i;
  st_file_data_ext* fl_data; 
  st_mhl_file_data* mhl_data;
  st_files_refs* tmp_ref;

  for (i=0; i< data->mhl_paths.mhl_files_data_cnt; ++i)
  {
    mhl_data = data->mhl_paths.mhl_files_data + i;

    free(mhl_data->mhl_wdirname);
    free(mhl_data->mhl_wpath);
    free_fs_wpath(&(mhl_data->mhl_dir_wpath));

    if (mhl_data->fl_descr != NULL)
    {
      fclose(mhl_data->fl_descr);
    }

    // iterate through a list of files references, and clear it
    while (mhl_data->last != NULL)
    {
      tmp_ref = mhl_data->last->prev;
      free(mhl_data->last->relative_wfilename);
      free(mhl_data->last);
      mhl_data->last = tmp_ref;
    }
  }

  free(data->mhl_paths.mhl_files_data);

  free(data->wworkdir);
  free_fs_wpath(&(data->workdir_wpath));

  free(data->creator_data.login_name_str);
  free(data->creator_data.full_name_str);
  free(data->creator_data.host_name_str);
  free(data->creator_data.startdate_str);
  free(data->creator_data.startdate_log_str);
  free(data->creator_data.finishdate_str);
  free(data->creator_data.finishdate_log_str);
  free(data->creator_data.log_str);
  clean_log_str(data->p_v_data);
  data->p_v_data = NULL; // we didn't allocate p_v_data => no memory freeing

//  data->p_v_data->log_str = NULL;


  for (i=0; i< data->input_data.files_data_cnt; ++i)
  {
    fl_data = data->input_data.files_data_array + i;
    free(fl_data->orig_wfilename);
    free_fs_wpath(&(fl_data->file_wpath));
    free(fl_data->lastmodificationdate_str);

#ifdef WIN
    free(fl_data->creationdate_str);
#endif

    free(fl_data->hashdate_str);
    free(fl_data->major_hash.hash_sum);
    free(fl_data->aux_hash.hash_sum);
  }

  free(data->input_data.files_data_array);
}

int fill_mhl_path( 
  st_mhl_file_data* data, 
  const wchar_t* mhl_wdirname,
  const struct tm* start_gmtm,
  st_conversion_settings* p_cs)
{
  size_t wdir_len;
  size_t wend_len;
  size_t wmhlfile_len;
  size_t full_wmhlfile_len;
  int res;
  wchar_t* wname_shift_pointer;
  wchar_t* mhl_wfile_name;
  const wchar_t* containing_wdirname = NULL;
  const st_fs_wpath* mhl_dir_wpath = &(data->mhl_dir_wpath);
  char ftime_str[MHLNAME_TIME_SUBSTR_LEN + 1];
  wchar_t* wftime_str;
  size_t wftime_str_sz;

  if ( mhl_dir_wpath == NULL || mhl_dir_wpath->is_absolute == 0 || 
       mhl_dir_wpath->is_normalized == 0)
  {
    return ERRCODE_INTERNAL_ERROR;
  }

  if (mhl_dir_wpath->items_cnt != 0)
  {
#ifdef WIN
    // don't put disk name like 'C:' into MHL file name
    if (mhl_dir_wpath->items_cnt > 1)
    {
      containing_wdirname = mhl_dir_wpath->items[mhl_dir_wpath->items_cnt - 1];
    }
#else
    containing_wdirname = mhl_dir_wpath->items[mhl_dir_wpath->items_cnt - 1];
#endif
  }
 
  wend_len = wcslen(MHLNAME_END_WSTR);

  if (containing_wdirname != NULL)
  {
    wdir_len = wcslen(containing_wdirname);
    wmhlfile_len = wdir_len + 1 + MHLNAME_TIME_SUBSTR_LEN + wend_len + 1;

    mhl_wfile_name = (wchar_t*)calloc(wmhlfile_len, sizeof(wchar_t));
    if (mhl_wfile_name == NULL)
    {
      fprintf(stderr, "Failed to allocate %lu bytes for MHL file name. "
              "Out of memory.\n", (long unsigned int)wmhlfile_len * sizeof(wchar_t));
      return ERRCODE_OUT_OF_MEM;
    }

    wcsncpy(mhl_wfile_name, containing_wdirname, wdir_len);
    wname_shift_pointer = mhl_wfile_name + wdir_len;
    *wname_shift_pointer = (wchar_t)L'_';
    ++wname_shift_pointer;
  }
  else
  {
    wmhlfile_len = MHLNAME_TIME_SUBSTR_LEN + wend_len + 1;
    mhl_wfile_name = (wchar_t*)calloc(wmhlfile_len, sizeof(wchar_t));
    if (mhl_wfile_name == NULL)
    {
      fprintf(
        stderr, 
        "Failed to allocate %lu bytes for MHL file name. "
        "Out of memory.\n", 
        (long unsigned int)wmhlfile_len * sizeof(wchar_t));
      return ERRCODE_OUT_OF_MEM;
    }

    wname_shift_pointer = mhl_wfile_name;
  }

  res = strftime(ftime_str, MHLNAME_TIME_SUBSTR_LEN + 1, "%Y-%m-%d_%H%M%S", start_gmtm);
  if (res == 0)
  {
    free(mhl_wfile_name);
    return ERRCODE_UNRECOGNIZED_TIME;
  }

  res = 
    convert_from_utf8_to_wchar(
      ftime_str, 
      strlen(ftime_str), 
      &wftime_str, 
      &wftime_str_sz,
      p_cs);
  if (res != 0)
  {
    free(mhl_wfile_name);
    return ERRCODE_UNRECOGNIZED_TIME;
  }

  wcsncpy(wname_shift_pointer, wftime_str, wftime_str_sz);
  wname_shift_pointer += wftime_str_sz;
  free(wftime_str);

  if (wend_len != 0)
  {
    // We have already '\0' at the end of string due to calloc
    wcsncpy(wname_shift_pointer, MHLNAME_END_WSTR, wend_len);
  }

  // Now mhl_file_name contains created file name. 
  // Construct full path to the file.
  wdir_len = wcslen(mhl_wdirname);
  if (wdir_len == 0)
  {
    fprintf(stderr, "Empty MHL directory.\n");
    return ERRCODE_INTERNAL_ERROR;
  }

  // mhlfile_len contains extra 1 for space for '\0'
  // Consider new full_mhlfile_len as combination of mhldir length,
  // space for PATH_SEPARATOR, and mhlfile_len (file only)
  full_wmhlfile_len = wdir_len + 1 + wmhlfile_len;
  data->mhl_wpath = (wchar_t*)calloc(full_wmhlfile_len, sizeof(wchar_t));
  
  if (data->mhl_wpath == NULL)
  {
    fprintf(stderr, "Failed to allocate %lu bytes for MHL file name. "
            "Out of memory.\n",
            (long unsigned int)full_wmhlfile_len * sizeof(wchar_t));
    return ERRCODE_OUT_OF_MEM;
  }

  wcsncpy(data->mhl_wpath, mhl_wdirname, wdir_len);
    
  if (data->mhl_wpath[wdir_len-1] != WPATH_SEPARATOR)
  {
    data->mhl_wpath[wdir_len] = WPATH_SEPARATOR;
    wname_shift_pointer = data->mhl_wpath + wdir_len + 1;
  }
  else
  {
    wname_shift_pointer = data->mhl_wpath + wdir_len;
  }
    
  // We have already '\0' at the end of string due to calloc
  wcsncpy(wname_shift_pointer, mhl_wfile_name, wmhlfile_len - 1);

  free(mhl_wfile_name);

  return 0;
}

int
init_mhlcreate_data(st_mhlcreate_data* data,
                    st_verbose_data* p_v_data)
{
  memset((void*) data, 0, sizeof(st_mhlcreate_data) / sizeof(char));
  
  if (p_v_data == NULL)
  {
    fprintf(stderr, "init_mhlcreate_data(): Internal error - "
            "pointer to verbose data is null\n");
    return ERRCODE_INTERNAL_ERROR;
  }
  data->p_v_data = p_v_data;
  
  return 0;
}

// returns in case of success: 0
//         in case of failure: error code, and print error message to stderr
int preprocess_mhlcreate_data(
  st_mhlcreate_data* data,
  const struct tm* start_gmtm,
  st_conversion_settings* p_cs)
{
  int res;
  unsigned int i;
  st_mhl_file_data* mhl_f_data;

  // both mhl directory and workdir are in locale encoding
  data->wworkdir = get_wworkdir(p_cs);
  if (data->wworkdir == 0)
  {
    return ERRCODE_INTERNAL_ERROR; 
  }

  res = init_fs_wpath(data->wworkdir, &(data->workdir_wpath));
  if (res != 0)
  {
    return res;
  }

  // We got this path from system, so it is always normalized 
  data->workdir_wpath.is_normalized = 1;

  if (data->mhl_paths.mhl_files_data_cnt == 0)
  {
    if (data->mhl_paths.mhl_files_data_capacity == 0)
    {
      res = increase_allocated_memory(
        (void**)&data->mhl_paths.mhl_files_data,
        &data->mhl_paths.mhl_files_data_capacity,
        1,
        sizeof(st_mhl_file_data));

      if (res != 0)
      {
        fprintf(stderr, "Out of memory.\n");
        return res;
      }
    }

    data->mhl_paths.mhl_files_data_cnt = 1;
    data->mhl_paths.mhl_files_data->mhl_wdirname = mhlosi_wstrdup(data->wworkdir);
    if (data->mhl_paths.mhl_files_data->mhl_wdirname == NULL)
    {
      data->mhl_paths.mhl_files_data_cnt = 0;
      fprintf(stderr, "Out of memory.\n");
      return ERRCODE_OUT_OF_MEM;
    }
  }

  if (data->p_v_data->verbose_level)
  {
    logit(data->p_v_data, "===================\n");
    logit(data->p_v_data,
          "%s ver. %s started.\n"
          "Verbose mode is ON.\n"
          "Working directory: \"%ls\"\n"
          "MHL file directory(-es):\n",
          MHLCREATE_NAME, VERSION, data->wworkdir);
  }

  for (i = 0; i < data->mhl_paths.mhl_files_data_cnt; ++i)
  {
    mhl_f_data = data->mhl_paths.mhl_files_data + i;

    // change paths separators (makes sense only for windows)
    make_wpath_os_specific(mhl_f_data->mhl_wdirname); 

    if (data->p_v_data->verbose_level)
    {
      logit(data->p_v_data,
            "   %ls\n",
            mhl_f_data->mhl_wdirname);
    }

    res = init_fs_wpath(mhl_f_data->mhl_wdirname, &(mhl_f_data->mhl_dir_wpath));
    if (res != 0)
    {
      return res;
    }

    if (mhl_f_data->mhl_dir_wpath.is_absolute == 0)
    {
      res = convert_fs_wpath_to_absolute(
              &(data->workdir_wpath), &(mhl_f_data->mhl_dir_wpath));
      if (res != 0)
      {
        return res;
      }
    }
    else
    {
      // Even if mhl path is already absolute, we need to normalize it
      res = normalize_fs_wpath(&(mhl_f_data->mhl_dir_wpath));
      if (res != 0)
      {
        return res;
      }
    }

    res = fill_mhl_path(mhl_f_data, mhl_f_data->mhl_wdirname, start_gmtm, p_cs);
    if (res != 0)
    {
      return res;
    }
  }

  if (data->p_v_data->verbose_level)
  {
    logit(data->p_v_data, "-------------------\n");
  }

  return res; 
}

int
open_wfile(const wchar_t* file_wpath, FILE** fl_descr)
{
  *fl_descr = fwopen_for_hash_create(file_wpath);

  if (*fl_descr == NULL)
  {
    fprintf(stderr, "Cannot open file for writing: %ls. Errno=%d. Error:%s\n",
            file_wpath, errno, strerror(errno));
    return ERRCODE_IO_ERROR;
  }

  return 0;
}

int convert_time_to_gmtime(time_t seconds_since_epoche_start, struct tm* gmtm)
{

#ifdef WIN  
  int res;  
  res = gmtime_s(gmtm, &seconds_since_epoche_start);
  if ( res != 0)
  {
    return ERRCODE_UNRECOGNIZED_TIME;
  }
#else
  if (gmtime_r(&seconds_since_epoche_start, gmtm) == NULL)
  {
    return ERRCODE_UNRECOGNIZED_TIME;
  }
#endif

  return 0;
}

static
int 
timetostr(char** time_str, time_t seconds_since_epoche_start, 
          struct tm* cur_gmtm)
{
  int res;

  res = convert_time_to_gmtime(seconds_since_epoche_start, cur_gmtm);
  if (res != 0)
  {
    return res;
  }

  *time_str = (char*)calloc(TIME_STR_SZ, sizeof(char));
  if (*time_str == NULL)
  {
    fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }

  // 2011-03-10T07:49:21Z
  res = strftime(*time_str, TIME_STR_SZ, "%Y-%m-%dT%H:%M:%SZ", cur_gmtm);
  if (res == 0)
  {
    return ERRCODE_UNRECOGNIZED_TIME;
  }

  return 0;
}

int get_xml_date(char** date_str, struct tm* cur_gmtm)
{
  time_t cur_tm;
  int res;

  cur_tm = time(NULL);
  if (cur_tm < 0)
  {
    fprintf(stderr, "Unknown error, time() call failed. " 
            "Errno=%d. Error:%s\n",
            errno, strerror(errno));
    return ERRCODE_UNKNOWN_ERROR;
  }

  res = timetostr(date_str, cur_tm, cur_gmtm);
  if (res != 0)
  {
    return res;
  }

  return 0;
}

int date_to_log_str(char** date_str, const struct tm* gmtm)
{
  int res;

  *date_str = (char*)calloc(TIME_STR_SZ, sizeof(char));
  if (*date_str == NULL)
  {
    fprintf(stderr, "Out of memory.\n");
    return ERRCODE_OUT_OF_MEM;
  }

  // 2011-03-10 07:49:21
  res = strftime(*date_str, TIME_STR_SZ, "%Y-%m-%d %H:%M:%S", gmtm);
  if (res == 0)
  {
    return ERRCODE_UNRECOGNIZED_TIME;
  }

  return 0;
}

int
process_file(
  st_file_data_ext* file_data, 
  st_fs_wpath* work_wpath,
  st_conversion_settings* p_cs)
{
  struct tm gm_date;
  st_mhlosi_stat wfl_stat;
  int res;

  res = get_xml_date(&(file_data->hashdate_str), &gm_date);
  if (res != 0)
  {
    fprintf(stderr, "Getting or processing of hashdate failed.\n");
    return res;
  }

  // fill fs_path structures
  res = init_fs_wpath(file_data->orig_wfilename, &(file_data->file_wpath));
  if (res != 0)
  {
    return res;
  }

  if (!file_data->file_wpath.is_absolute)
  {
    res = 
      convert_fs_wpath_to_absolute(work_wpath, &(file_data->file_wpath));
    if (res != 0)
    {
      return res;
    }
  }
  else
  {
    // Even if path is already absolute, we need to normalize it 
    res = normalize_fs_wpath(&(file_data->file_wpath));
    if (res != 0)
    {
      return res;
    }
  }
 
  res = get_wfile_stat_data(file_data->orig_wfilename, &wfl_stat);
  if (res != 0)
  {
    if (res == ERRCODE_NO_SUCH_FILE)
    {
      fprintf(stderr, "Error: File does not exist: '%ls'.\n",
              file_data->orig_wfilename);
    }
    else
    {
      fprintf(stderr, "Error: Cannot get file's data, stat() failed for file: " 
              "%ls. Errno=%d. Error:%s\n",
              file_data->orig_wfilename, errno, strerror(errno));
    }
    return res;
  }

  file_data->file_sz = wfl_stat.st_data.st_size;
  res = timetostr(&(file_data->lastmodificationdate_str), 
                  wfl_stat.st_data.st_mtime, &gm_date);
  if (res != 0)
  {
    if (res == ERRCODE_UNRECOGNIZED_TIME)
    {
      fprintf(stderr, "Processing of lastmodificationdate failed for file: %ls.\n",
              file_data->orig_wfilename);
    }
    return res;
  }

#ifdef WIN
  res = timetostr(&(file_data->creationdate_str), wfl_stat.st_data.st_ctime,
                  &gm_date);
  if (res != 0)
  {
    if (res == ERRCODE_UNRECOGNIZED_TIME)
    {
      fprintf(stderr, "Processing of creationdate failed for file: %s.\n",
              file_data->orig_wfilename);
    }
    return res;
  }
#endif

  return 0;
}

int
add_data_to_containing_folders(st_mhl_dirs_data* mhl_paths_ref,
  st_file_data_ext* file_data, unsigned int file_data_idx)
{
  unsigned int i;
  unsigned char folder_found;
  int res;
  st_mhl_file_data* mhl_file_data;
  wchar_t* relative_wfilename;
  st_files_refs** file_in_dir;
  st_files_refs* prev_elem;

  folder_found = 0;

  for (i=0; i < mhl_paths_ref->mhl_files_data_cnt; ++i)
  {
    mhl_file_data = mhl_paths_ref->mhl_files_data + i;
    relative_wfilename = NULL;

    res = extract_relative_wpath(&(mhl_file_data->mhl_dir_wpath),
      &(file_data->file_wpath),
      &relative_wfilename);

    if (res == 0)
    {
      folder_found = 1;

      // Add record about a file as a new last element
      prev_elem = mhl_file_data->last;
      if (mhl_file_data->last == NULL)
      {
        file_in_dir = &mhl_file_data->files_inside_dir;
      }
      else
      {
        file_in_dir = &(mhl_file_data->last->next);
      }
      *file_in_dir = (st_files_refs*)calloc(1, sizeof(st_files_refs));
      if (*file_in_dir == NULL)
      {
        fprintf(stderr, "Out of memory.\n");
        free(relative_wfilename);
        return ERRCODE_OUT_OF_MEM;
      }

      //reassign;
      //now relative_wfilename from st_files_refs is responsible 
      //for holding the allocated memory
      (*file_in_dir)->relative_wfilename = relative_wfilename;
      (*file_in_dir)->file_data_idx = file_data_idx;

      (*file_in_dir)->prev = prev_elem;
      mhl_file_data->last = *file_in_dir;
    }
  }

  if (!folder_found)
  {
    fprintf(stderr, "File %ls is not in any of the specified with '-o' or '--output-folder' directory or "
            "it's subdirectory.\n", file_data->orig_wfilename);

    return ERRCODE_WRONG_FILE_LOCATION;
  }

  return 0;
}

int create_mhl_files(st_mhlcreate_data* data, st_conversion_settings* p_cs)
{
  unsigned int i;
  int res;
  st_mhl_file_data* mhl_f_data;

  if (data->p_v_data->verbose_level)
  {
    logit(data->p_v_data, "-------------------\n");
    logit(data->p_v_data,
          "End of input.\n"
          "Finish date in UTC: %s\n"
          "MHL file path(s):\n",
          data->creator_data.finishdate_log_str);

    for (i = 0; i < data->mhl_paths.mhl_files_data_cnt; ++i)
    {
      mhl_f_data = data->mhl_paths.mhl_files_data + i;
      logit(data->p_v_data,
            "   %ls\n",
            mhl_f_data->mhl_wpath);
    }
    logit(data->p_v_data, "===================\n");
  }

  for (i = 0; i < data->mhl_paths.mhl_files_data_cnt; ++i)
  {
    mhl_f_data = data->mhl_paths.mhl_files_data + i;

    if (does_wpath_exist(mhl_f_data->mhl_wpath))
    {
      fprintf(stderr,
              "Error, while writing MHL file: %ls\nFile already exist.\n",
              mhl_f_data->mhl_wpath);
      return ERRCODE_IO_ERROR;
    }

    res = open_wfile(mhl_f_data->mhl_wpath, &mhl_f_data->fl_descr);
    if (res != 0)
    {
      return res;
    }

    res = create_mhl(&data->creator_data, data->p_v_data, mhl_f_data, 
                     &data->input_data, p_cs);
    if (0 == res && data->p_v_data->machine_output) {
      fprintf(stderr, "%ls|OK\n", mhl_f_data->mhl_wpath);
    }
    if (res != 0)
    {
      fprintf(stderr, "Error, while writing MHL file: %ls\n", mhl_f_data->mhl_wpath);
      return res;
    }
  }

  return 0;
}
