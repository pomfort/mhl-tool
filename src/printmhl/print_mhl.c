/*
 The MIT License (MIT)
 
 Copyright (c) 2016 Pomfort GmbH GmbH
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
#include <stdio.h>

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
#endif

#include <errno.h>
#include <string.h>

#include <facade_info/version.h>
#include <facade_info/error_codes.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <mhltools_common/logging.h>
#include <mhltools_common/files_data.h>
#include "create_mhl_files_data.h"

#include "print_mhl.h"

#ifdef WIN
#define MAX_USER_NAME_LEN 1024
#endif
#define MAX_HOST_NAME_LEN 4096

static int xml_puts(const char *string, FILE *stream)
{
  int i = 0, c, status;
  while ((c = string[i++])) {
    switch (c) {
      case '"':
        status = fputs("&quot;", stream);
        break;
      case '\'':
        status = fputs("&apos;", stream);
        break;
      case '<':
        status = fputs("&lt;", stream);
        break;
      case '>':
        status = fputs("&gt;", stream);
        break;
      case '&':
        status = fputs("&amp;", stream);
        break;
      default:
        status = putc(c, stream);
        break;
    }
    if (EOF == status) {
      return 0;
    }
  }
  return 1;
}

static int xml_puts_node(const char* data, const char* node, FILE *stream) {
  int r;
  fprintf(stream, "<%s>", node);
  r = xml_puts(data, stream);
  fprintf(stream, "</%s>\n", node);
  return r;
}

int
print_xml_and_hashlist_header(FILE* fl_descr)
{
  int res;
  res = fprintf(fl_descr, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                          "<hashlist version=\"1.1\">\n\n");
  if (res == 0)
  {
    print_error("IO error: failed to print information into .mhl file");
    return ERRCODE_IO_ERROR;
  }

  return 0;
}

#ifdef WIN
int fill_user_and_host_info(st_creator_data* creator_data, 
                            st_verbose_data* v_data,
                            st_conversion_settings* p_cs)
{
  WCHAR buf[MAX_USER_NAME_LEN];
  WCHAR host_buf[MAX_HOST_NAME_LEN];
  unsigned long bufsize;
  int res;
  HRESULT hr;
  IADsADSystemInfo* pSys;
  BSTR bstr;
  int len;
  int i;
  int j;
  size_t str_sz;

  // Get Active Directory Logon user name and Computer name
  hr = CoInitialize(NULL);

  hr = CoCreateInstance(&CLSID_ADSystemInfo,
                        NULL,
                        CLSCTX_INPROC_SERVER,
                        &IID_IADsADSystemInfo,
                        (void**)&pSys);

   if (SUCCEEDED(hr)) 
   {  
     hr = pSys->lpVtbl->get_UserName(pSys, &bstr);
     if (SUCCEEDED(hr)) 
     {
       len = SysStringLen(bstr);

       // data starts in 4 bytes after BSTR beginning
       res = 
         convert_from_wchar_to_utf8(
          bstr + 4/sizeof(WCHAR), 
          len,
          &creator_data->login_name_str,
          &str_sz,
          p_cs);

       SysFreeString(bstr);
     }

     hr = pSys->lpVtbl->get_ComputerName(pSys, &bstr);
     if (SUCCEEDED(hr))
     {
       len = SysStringLen(bstr);

       // data starts in 4 bytes after BSTR beginning
       res = 
         convert_from_wchar_to_utf8(
          bstr + 4/sizeof(WCHAR), 
          len,
          &creator_data->host_name_str,
          &str_sz,
          p_cs);

       //creator_data->host_name_str = convert_from_wchar_to_utf8(
       //  bstr + 4/sizeof(WCHAR), len);

       SysFreeString(bstr);
     }

     if(pSys) 
     {
       pSys->lpVtbl->Release(pSys);
     }
   }

   CoUninitialize();

   if (creator_data->login_name_str == NULL)
   {
     // Get Display user name
     if (UNLEN < MAX_USER_NAME_LEN)
     {
       bufsize = UNLEN + 1;
     }
     else
     {
       bufsize = MAX_USER_NAME_LEN;
     }
     res = GetUserNameW(buf, &bufsize);
     if (res != 0)
     {
       //creator_data->login_name_str = convert_from_wchar_to_utf8(buf, bufsize);
       res = 
         convert_from_wchar_to_utf8(
          buf, 
          bufsize,
          &creator_data->login_name_str,
          &str_sz,
          p_cs);
     }
   }

   if (creator_data->login_name_str == NULL)
   {
     // Get the first available user name
     for (i = 1; i < NameDnsDomain + 1; ++i)
     {
       bufsize = MAX_USER_NAME_LEN;
       res = GetUserNameExW(i, buf, &bufsize);
       if (res != 0)
       {
         //creator_data->login_name_str = convert_from_wchar_to_utf8(buf, bufsize);
         res = 
           convert_from_wchar_to_utf8(
            buf, 
            bufsize,
            &creator_data->login_name_str,
            &str_sz,
            p_cs);

         break;
       }
     }

     if (creator_data->login_name_str == NULL)
     {
       fprintf(stderr, "Failed to get any available user name.\n");
       return ERRCODE_UNKNOWN_ERROR;
     }
  }

  // Get Full user name
  bufsize = MAX_USER_NAME_LEN;
  res = GetUserNameExW(NameFullyQualifiedDN, buf, &bufsize);
  if (res != 0)
  {
    //creator_data->full_name_str = convert_from_wchar_to_utf8(buf, bufsize);
    res = 
      convert_from_wchar_to_utf8(
      buf, 
      bufsize,
      &creator_data->full_name_str,
      &str_sz,
      p_cs);
  }

  // Get computer name
  if (creator_data->host_name_str == NULL)
  {
    bufsize = MAX_HOST_NAME_LEN;
    res = GetComputerNameExW(ComputerNamePhysicalDnsFullyQualified, 
      host_buf, &bufsize);
    if (res != 0)
    {
      //creator_data->host_name_str = 
      //  convert_from_wchar_to_utf8(host_buf, bufsize);
      res = 
        convert_from_wchar_to_utf8(
        host_buf, 
        bufsize,
        &creator_data->host_name_str,
        &str_sz,
        p_cs);
    }
  }

  if (creator_data->host_name_str == NULL)
  {
    if (MAX_COMPUTERNAME_LENGTH < MAX_HOST_NAME_LEN)
    {
      bufsize = MAX_COMPUTERNAME_LENGTH + 1;
    }
    else
    {
      bufsize = MAX_HOST_NAME_LEN;
    }
    res = GetComputerNameW(host_buf, &bufsize);
    if (res != 0)
    {
      //creator_data->host_name_str = 
      //  convert_from_wchar_to_utf8(host_buf, bufsize);
      res = 
        convert_from_wchar_to_utf8(
        host_buf, 
        bufsize,
        &creator_data->host_name_str,
        &str_sz,
        p_cs);

    }
  }

  // Get any available computer name
  if (creator_data->host_name_str == NULL)
  {
    for (j = 1; j < ComputerNameMax; ++j)
    {

      bufsize = MAX_HOST_NAME_LEN;
      res = GetComputerNameExW(j, host_buf, &bufsize);
      if (res != 0)
      {
        //creator_data->host_name_str = 
        //  convert_from_wchar_to_utf8(host_buf, bufsize);
        res = 
          convert_from_wchar_to_utf8(
          host_buf, 
          bufsize,
          &creator_data->host_name_str,
          &str_sz,
          p_cs);

        break;
      }
    }

    if (creator_data->host_name_str == NULL)
    {
      fprintf(stderr, "Failed to get any available host name.\n");
      return ERRCODE_UNKNOWN_ERROR;
    }
  }

  return 0;
}

#else // Linux, Mac OS X

int fill_user_and_host_info(
  st_creator_data* creator_data,
  st_verbose_data* v_data,
  st_conversion_settings* p_cs)
{
  uid_t uid;
  struct passwd pwd;
  struct passwd *result;
  char* buf;
  size_t bufsize;
  long max_sz;
  int res;
  int login_len;
  int name_len;
  int host_len;

  max_sz = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (max_sz == -1)    
  {
    bufsize = BUFF_SZ;
  }
  else
  {
    bufsize = max_sz;
  }

  buf = malloc(bufsize);
  if (buf == NULL)
  {
    fprintf(stderr,
            "fill_user_and_host_info: Failed to allocate memory of "
            "size %lu bytes. Errno=%d. Error:%s\n",
            (long unsigned int)bufsize, errno, strerror(errno));
    return ERRCODE_OUT_OF_MEM;
  }

  uid = getuid();
  res = getpwuid_r(uid, &pwd, buf, bufsize, &result);
  if (result == NULL)
  {
    if (res == 0)
    {
      fprintf(stderr, "Information record for user with id = %d is not "
                      "found in the system.\n", uid);
    }
    else
    {
      fprintf(stderr,
              "fill_user_and_host_info: Failed to get user info for user with "
              "id = %d. Errno=%d. Error:%s\n",
              uid, errno, strerror(errno));
    }

    free(buf);
    return ERRCODE_UNKNOWN_ERROR;
  }

  name_len = strlen(pwd.pw_gecos);
  if (name_len != 0)
  {
    creator_data->full_name_str = (char*)calloc(name_len + 1, sizeof(char));
    if (creator_data->full_name_str == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for user name string. "
                  "Out of memory.\n");
      return ERRCODE_OUT_OF_MEM;
    }
    strncpy(creator_data->full_name_str, pwd.pw_gecos, name_len);
  }

  login_len = strlen(pwd.pw_name);
  if (login_len == 0)
  {
    fprintf(stderr, "WARNING: User with id = %d doesn't has a login name. "
            "username tag will not be printed into .mhl file\n", uid);
    if (v_data->verbose_level)
    {
      logit(v_data, "WARNING: User with id = %d doesn't has a login name. "
            "username tag will not be printed into .mhl file\n", uid);
    }
  }
  else
  {
    creator_data->login_name_str = (char*)calloc(login_len + 1, sizeof(char));
    if (creator_data->login_name_str == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for user login string. "
                 "Out of memory.\n");
      return ERRCODE_OUT_OF_MEM;
    }
    strncpy(creator_data->login_name_str, pwd.pw_name, login_len);
  }  
  free(buf);
  
  host_len = sysconf(_SC_HOST_NAME_MAX);  
  if (host_len == -1)    
  {
    host_len = MAX_HOST_NAME_LEN;
  }
  
  buf = (char*)calloc(host_len+1, sizeof(char));
  if (buf == NULL)
  {
    fprintf(stderr,
            "fill_user_and_host_info: Failed to allocate memory of "
            "size %lu bytes. Errno=%d. Error:%s\n",
            (long unsigned int)(host_len+1)*sizeof(char),
            errno, strerror(errno));
    return ERRCODE_OUT_OF_MEM;
  }
  
  res = gethostname(buf, host_len +1);
  if (res != 0)
  {
    fprintf(stderr,
            "fill_user_and_host_info: Failed to get host name. "
            "Errno=%d. Error:%s\n",
            errno, strerror(errno));
        
    free(buf);
    return ERRCODE_UNKNOWN_ERROR;
  }
  
  host_len = strlen(buf);
  if (host_len != 0)
  {
    creator_data->host_name_str = (char*)calloc(host_len + 1, sizeof(char));
    if (creator_data->host_name_str == NULL)
    {
      fprintf(stderr, "Failed to allocate memory for host name string. "
              "Out of memory.\n");
      return ERRCODE_OUT_OF_MEM;
    }
    strncpy(creator_data->host_name_str, buf, host_len);
  }
  
  free(buf);  
  
  return 0;
}

#endif // ifdef WIN else

int
print_creator_info(
  FILE* fl_descr, 
  st_creator_data* creator_data,
  st_verbose_data* v_data,
  st_conversion_settings* p_cs)
{
  int res;
  char* u8str;
  size_t u8str_sz;

  res = fill_user_and_host_info(creator_data, v_data, p_cs);
  if (res != 0)
  {
    return res;
  }

  res = fprintf(fl_descr,
    "  <creatorinfo>\n");
  if (res == 0)
  {
    print_error("IO error: failed to print information into .mhl file");
    return ERRCODE_IO_ERROR;
  }

  if (creator_data->full_name_str != NULL)
  {
    fprintf(fl_descr, "    ");
    res = xml_puts_node(creator_data->full_name_str, "name", fl_descr);
    if (res == 0)
    {
      print_error("IO error: failed to print information into .mhl file");
      return ERRCODE_IO_ERROR;
    }
  }
  
  if (creator_data->login_name_str != NULL)
  {
    fprintf(fl_descr, "    ");
    res = xml_puts_node(creator_data->login_name_str, "username", fl_descr);
    if (res == 0)
    {
      print_error("IO error: failed to print information into .mhl file");
      return ERRCODE_IO_ERROR;
    }
  }  

  res = xml_puts_node(creator_data->host_name_str, "hostname", fl_descr);
  
  res = fprintf(fl_descr,
    "    <tool>%s ver. %s</tool>\n"
    "    <startdate>%s</startdate>\n"
    "    <finishdate>%s</finishdate>\n",
    MHLCREATE_NAME, VERSION,
    creator_data->startdate_str, 
    creator_data->finishdate_str);
  if (res == 0)
  {
    print_error("IO error: failed to print information into .mhl file");
    return ERRCODE_IO_ERROR;
  }

  if (v_data->verbose_level && v_data->log_str_len != 0)
  {
    
    res = 
      convert_from_locale_to_utf8(
        v_data->log_str, 
        v_data->log_str_len,
        &u8str,
        &u8str_sz,
        p_cs);
    
    if (res != 0)
    {
      fprintf(stderr, "Failed to convert log string from locale to UTF-8, "
              "left it as is\n");
    }
    else
    {
      free(v_data->log_str);
      v_data->log_str = u8str;
    }
    
    res = fprintf(fl_descr,
      "    <log><![CDATA[%s]]>\n"
      "    </log>\n",
      creator_data->log_str ? creator_data->log_str : v_data->log_str);

    if (res == 0)
    {
      print_error("IO error: failed to print information into .mhl file");
      return ERRCODE_IO_ERROR;
    }
  }

  res = fprintf(fl_descr,
    "  </creatorinfo>\n\n");
  if (res == 0)
  {
    print_error("IO error: failed to print information into .mhl file");
    return ERRCODE_IO_ERROR;
  }

  return 0;
}

int
print_file_hash_info(
  FILE* fl_descr, 
  st_file_data_ext* file_data,
  const wchar_t* relative_wfilename,
  st_conversion_settings* p_cs)
{
  int res;
  char* u8_fname;
  size_t u8_fname_sz;
  wchar_t* w_fname;

  w_fname = mhlosi_wstrdup(
    relative_wfilename ?
    relative_wfilename :
    file_data->orig_wfilename);
  if (w_fname == 0)
  {
    print_error("Cannot alloacte memory for file name");
    return ERRCODE_OUT_OF_MEM;
  }

  //printf("w_fname in print hash: %ls\n", w_fname);
  
  // before writing filename to MHL file, os specific filepath separators 
  // will be changed to uniform separators
  make_wpath_uniform(w_fname);

  //printf("w_fname in print hash uniform: %ls\n", w_fname);
  
  res = 
    convert_from_wchar_to_utf8(
      w_fname, 
      wcslen(w_fname), 
      &u8_fname, 
      &u8_fname_sz, p_cs);

  //printf("filename in utf-8: %s len= %d\n", u8_fname, strlen(u8_fname));
  
  free(w_fname);

  if (res != 0)
  {
    print_error(
      "Char conversion error: cannot convert filename from wchar to UTF8");
    
    return ERRCODE_IO_ERROR;
  }
  
  res = fprintf(fl_descr,
                "  <hash>\n");
  fprintf(fl_descr, "    ");
  res = xml_puts_node(u8_fname, "file", fl_descr);

  res = fprintf(fl_descr,
    "    <size>%llu</size>\n"
#ifdef WIN
    "    <creationdate>%s</creationdate>\n"
#endif
    "    <lastmodificationdate>%s</lastmodificationdate>\n"
    "    <%s>%s</%s>\n",
    file_data->file_sz,
#ifdef WIN
    file_data->creationdate_str,
#endif
    file_data->lastmodificationdate_str,
    file_data->major_hash.hash_type_str,
    file_data->major_hash.hash_sum, file_data->major_hash.hash_type_str);

  free(u8_fname);

  if (res == 0)
  {
    print_error("IO error: failed to print information into .mhl file");
    return ERRCODE_IO_ERROR;
  } 

  if (file_data->aux_hash.hash_sum != NULL)
  {
    res = fprintf(fl_descr,
      "    <%s>%s</%s>\n",
    file_data->aux_hash.hash_type_str,
    file_data->aux_hash.hash_sum, file_data->aux_hash.hash_type_str);

    if (res == 0)
    {
      print_error("IO error: failed to print information into .mhl file");
      return ERRCODE_IO_ERROR;
    } 
  }

  res = fprintf(fl_descr,
    "    <hashdate>%s</hashdate>\n"
    "  </hash>\n\n",
    file_data->hashdate_str);

  if (res == 0)
  {
    print_error("IO error: failed to print information into .mhl file");
    return ERRCODE_IO_ERROR;
  }

  return 0;
}

int 
print_hashlist_footer(FILE* fl_descr)
{
  int res;

  res = fprintf(fl_descr, "</hashlist>\n");
  if (res == 0)
  {
    print_error("IO error: failed to print information into .mhl file");
    return ERRCODE_IO_ERROR;
  }

  return 0;
}

int
create_mhl(st_creator_data* creator_data, st_verbose_data* v_data,
           st_mhl_file_data* mhl_file, st_files_data* files_data,
           st_conversion_settings* p_cs)
{
  int res;
  st_files_refs* fl_data_ptr;

  res = print_xml_and_hashlist_header(mhl_file->fl_descr);
  if (res != 0)
  {
    return res;
  }

  res = print_creator_info(
    mhl_file->fl_descr,
    creator_data, 
    v_data,
    p_cs);

  if (res != 0)
  {
    return res;
  }

  fl_data_ptr = mhl_file->files_inside_dir;
  while (fl_data_ptr != NULL)
  {
    res = print_file_hash_info(
      mhl_file->fl_descr,
      files_data->files_data_array + fl_data_ptr->file_data_idx,
      fl_data_ptr->relative_wfilename,
      p_cs);

    if (res != 0)
    {
      return res;
    }

    fl_data_ptr = fl_data_ptr->next;
  } 

  res = print_hashlist_footer(mhl_file->fl_descr);
  
  return 0;
}
