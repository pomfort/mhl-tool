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
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <facade_info/error_codes.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>
#include <args_fileslist_support/file_sequences.h>

#define SEQUENCE_ANYMARK L'#'
#define SEQUENCE_RANGEMARK L'-'

#define AUX_TEMP_BUFF_SZ 64
int 
create_file_wpath_for_sequence_number(
  int i,
  st_mhl_sequence* p_seq,
  wchar_t** file_wpath,
  st_conversion_settings* p_cs)
{
  size_t file_wpath_len = 0;
  wchar_t* file_wpath_component = 0;
  wchar_t* wres;
  int ires;
  wchar_t format_str[AUX_TEMP_BUFF_SZ];

  if (p_seq == 0 || file_wpath == 0 || p_cs == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  file_wpath_len = p_seq->start_len + p_seq->end_len + p_seq->number_len + 1;
  *file_wpath = calloc(file_wpath_len, sizeof(wchar_t));
  if (*file_wpath == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }

  //
  // add start string part of sequence result
  //
  file_wpath_component = *file_wpath;

  if (p_seq->start_len !=0)
  {
    wres = 
      mhlosi_wstrncat(
        p_seq->start_str, file_wpath_component, p_seq->start_len);
    if (wres == 0)
    {
      free(*file_wpath);
      *file_wpath = 0;
    
      return ERRCODE_INTERNAL_ERROR; 
    }

    file_wpath_component += p_seq->start_len;
  }

  // create format string
  memset(format_str, 8, AUX_TEMP_BUFF_SZ * sizeof(wchar_t) / sizeof(char));
  if (p_seq->padding)
  {
    ires = swprintf(format_str, AUX_TEMP_BUFF_SZ, L"%%0%dd", p_seq->number_len);
  }
  else
  {
    ires = swprintf(format_str, AUX_TEMP_BUFF_SZ, L"%%d");
  }
  
  if (ires == -1)
  {
    free(*file_wpath);
    *file_wpath = 0;

    return ERRCODE_INTERNAL_ERROR;
  }

  // create number string part of sequence result
  ires = swprintf(file_wpath_component, p_seq->number_len + 1, format_str, i);
  if (ires == -1)
  {
    free(*file_wpath);
    *file_wpath = 0;

    return ERRCODE_INTERNAL_ERROR;
  }
 
  // create end string part of sequence result
  if (p_seq->end_len !=0)
  {
    file_wpath_component = *file_wpath;
    // mhlosi_wstrncat() takes dst string (may ber not empty),
    // and prints to the end
    wres = 
      mhlosi_wstrncat(
        p_seq->end_str, file_wpath_component, p_seq->end_len);
    if (wres == 0)
    {
      free(*file_wpath);
      *file_wpath = 0;
    
      return ERRCODE_INTERNAL_ERROR; 
    }
  }

  //
  return 0;
}

//
//
//
int
init_mhl_sequence(
  const wchar_t* pattern,
  st_mhl_sequence* p_seq,
  st_conversion_settings* p_cs)
{
  int res;
  const wchar_t* p_beg;
  const wchar_t* p_next;
  size_t p_diff_sz;
  size_t p_len;
  char* loc_num_beg;
  size_t loc_num_beg_sz;
  char* p_tmp = 0;
  
  if (pattern == 0 || pattern[0] == L'\0' || p_seq == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  memset(p_seq, 0, sizeof(*p_seq) / sizeof(char));
  
  p_beg = pattern;
 
  p_next = wcschr(p_beg, SEQUENCE_ANYMARK);
  p_diff_sz = 
    p_next == NULL ? 
    wcslen(p_beg) : 
    (size_t) (p_next - p_beg);
    
  if (p_diff_sz != 0)
  {
    p_seq->start_str = mhlosi_wstrndup(p_beg, p_diff_sz);
    if (p_seq->start_str == 0)
    {
      return ERRCODE_OUT_OF_MEM;
    }
  }
  else
  {
    p_seq->start_str = NULL;
  }
   
  p_seq->start_len = p_diff_sz;

  if (p_next != NULL)
  {
    p_beg = p_next;

    while (*p_next == SEQUENCE_ANYMARK) 
    {
      ++p_next;
    }
    p_seq->number_len = p_next - p_beg;

    if (p_seq->number_len > 1 && *p_next == L'0')
    {
      p_seq->padding = 1;
      while (*p_next == L'0')
      {
        ++p_next;
      }
    }

    p_beg = p_next;  
  
    p_next = wcschr(p_beg, SEQUENCE_RANGEMARK);
    if (p_next == 0)
    {
      fini_mhl_sequence(p_seq);
      return ERRCODE_INVALID_SEQUENCE;
    }     

    p_diff_sz = (size_t) (p_next - p_beg);
   
    res = convert_from_wchar_to_utf8(
      p_beg,
      p_diff_sz,
      &loc_num_beg,
      &loc_num_beg_sz,
      p_cs);
 
    if (res != 0)
    {
      fini_mhl_sequence(p_seq);
      return res;
    }     

    p_seq->first_number = strtoul(loc_num_beg, &p_tmp, 10); 
    if (*p_tmp != '\0')
    {
      free(loc_num_beg);
      fini_mhl_sequence(p_seq);
      return ERRCODE_INVALID_SEQUENCE;
    }
    free(loc_num_beg);

    p_beg = p_next + 1;
    p_len = wcslen(p_beg);
    if (p_len == 0)
    {
      fini_mhl_sequence(p_seq);
      return ERRCODE_INVALID_SEQUENCE;
    }
    if (p_len < p_seq->number_len)
    {
      p_seq->padding = 1;
      p_diff_sz = p_len;
    }
    else
    {
      p_diff_sz = p_seq->number_len;
    }

    res = convert_from_wchar_to_utf8(
      p_beg,
      p_diff_sz,
      &loc_num_beg,
      &loc_num_beg_sz,
      p_cs);
 
    if (res != 0)
    {
      fini_mhl_sequence(p_seq);
      return res;
    }     

    p_seq->last_number = strtoul(loc_num_beg, &p_tmp, 10); 
    if (*p_tmp != '\0')
    {
      p_seq->padding = 1;
      p_beg += (size_t)(p_tmp - loc_num_beg);
    }
    else
    {
      p_beg += p_diff_sz;
    }
    free(loc_num_beg);

    if (p_seq->first_number >= p_seq->last_number)
    {
      fini_mhl_sequence(p_seq);
      return ERRCODE_INVALID_SEQUENCE;
    }

    p_seq->end_len = wcslen(p_beg);

    if (p_seq->end_len > 0)
    {
      p_seq->end_str = mhlosi_wstrndup(p_beg, p_seq->end_len);

      if (p_seq->end_str == 0)
      {
        fini_mhl_sequence(p_seq);
        return ERRCODE_OUT_OF_MEM;
      }
    }

    return 0;
  }

  return 0;
}

void
fini_mhl_sequence(
  st_mhl_sequence* p_seq)
{
  //size_t i;
  
  if (p_seq == 0)
  {
    return;
  }

  free(p_seq->start_str);
  free(p_seq->end_str);
  memset(p_seq, 0, sizeof(*p_seq) / sizeof(char));
}

/*
 * Return 1 if passed src string is sequence,
 *        0 if passed src string is not sequence (does ot contain '#' chars) 
 */
unsigned char 
is_sequence(const wchar_t* wsrc)
{
  if (wsrc == 0 || wsrc[0] == '\0')
  {
    return 0;
  }
  
  return wcschr(wsrc, SEQUENCE_ANYMARK) != 0 ? 1 : 0;
}

/*
 *
 */
int 
search_files_fit_to_sequence(
  const wchar_t* p_seq_path,
  st_conversion_settings* p_cs,
  void* data, // this data will be passed to callback_fn
  FileProcessingCallback callback_fn)
{
  int res;
  wchar_t* absolute_sequence_wpath;
  wchar_t* file_wpath;
  st_mhl_sequence seq;
  unsigned long i;
  FILE* fd;
  unsigned char gap_is_found = 0;
  
  if (p_seq_path == 0 || p_cs == 0 || callback_fn == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  //
  // 1. create absolute path
  //
  res = 
    convert_to_absolute_normalized_wpath(
      p_seq_path,                                       
      &absolute_sequence_wpath,
      p_cs);
  
  if (res != 0)
  {
    return res;
  }
  
  if (!is_sequence(absolute_sequence_wpath))
  {
    // It is not sequence. Check it and return.
    res = callback_fn(p_seq_path, data);
    free(absolute_sequence_wpath);
    
    return res;
  }
   
  //
  // 2. Parse sequence string
  //
  res = init_mhl_sequence(absolute_sequence_wpath, &seq, p_cs); 
  if (res != 0)
  {
    free(absolute_sequence_wpath);
    return res;
  }
  
/*
  res = 
    extract_wdir_from_wpath(
      absolute_sequence_wpath, 
      &parent_wdir);
*/  
  
  //
  // 3. enum files according to sequence
  //

  for (i = seq.first_number; i <= seq.last_number; ++i)
  {
    res = create_file_wpath_for_sequence_number(i, &seq, &file_wpath, p_cs);
    if (res != 0) 
    {
      fini_mhl_sequence(&seq);
      return res;
    }

    fd = fwopen_for_hash_check(file_wpath);
    free(file_wpath);
    if (fd == NULL)
    {
      gap_is_found = 1;

      fprintf(
        stderr,
        "Failed to open the file: '%ls'.\n",
        file_wpath);
    }
    else
    {
      fclose(fd);
    }
  }

  if (gap_is_found)
  {
    fini_mhl_sequence(&seq);
    return ERRCODE_GAP_IN_SEQUENCE;
  }

  for (i = seq.first_number; i <= seq.last_number; ++i)
  {
    res = create_file_wpath_for_sequence_number(i, &seq, &file_wpath, p_cs);
    if (res != 0) 
    {
      fini_mhl_sequence(&seq);
      return res;
    }
    res = callback_fn(file_wpath, data);
    free(file_wpath);
    if (res != 0) 
    {
      fini_mhl_sequence(&seq);
      return res;
    }
  }

  //
  // 4. clear data
  //
  fini_mhl_sequence(&seq);
  
  return 0;  
}
