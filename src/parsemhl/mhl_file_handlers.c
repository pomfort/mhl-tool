//
//  mhl_file_handlers.c
//
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
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

#include <libxml/parserInternals.h>

#include <facade_info/error_codes.h>
#include <generics/os_check.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/controlling_data.h>
#include <mhltools_common/hashing.h>

#include "mhl_file_handlers.h"

//
//
//
/* TODO: Use iconv here, in order to skip sprcific spaces (like &nbsp)
 *
 * NOTE! Caller is responsible for freeing returned pointer.
 * converts string from xmlChar to char and ƒ
 * skips leading and trailing whitespaces
 *
 * @param src - source xmlChar* string
 * @param dst - destination char* string.
 *              It is converted string without
 *              leading and trailing whitespaces.
 *              If passed source string cntains only whitespaces
 *              result destination string will be empty (only "\0")
 * @return In case of success:  0
 *         In case of error: NON zero value indicating error
 */
static int
aux_strdup_trimmed(const xmlChar* src, char** u8dst)
{
  int src_sz = 0;
  int dst_sz = 0;
  int li = 0, ri = 0;
  const xmlChar* beg;
  
  src_sz = xmlStrlen(src);
  if (src_sz)
  {
    for (li = 0; li < src_sz; ++li) 
    {
      if (!isspace(src[li]))
      {
        break;
      }
    }

    for (ri = src_sz - 1; ri >= li; --ri) 
    {
      if (!isspace(src[ri]))
      {
        break;
      }
    }
    
    if (ri >= li)
    {
      dst_sz = ri - li + 1;    
    }
  }
  
  *u8dst = calloc(dst_sz + 1, sizeof(char));
  if (*u8dst == 0)
  {
    return ERRCODE_OUT_OF_MEM; 
  }
  
  beg = src + li;
  strncpy(*u8dst, (const char*)	beg, dst_sz);
  
  return 0;
}

//
//
//
/* TODO: Use iconv here, in order to skip sprcific spaces (like &nbsp)
 *
 * NOTE! Caller is responsible for freeing returned pointer.
 * converts string from xmlChar to char and ƒ
 * skips leading and trailing whitespaces
 *
 * @param src - source xmlChar* string
 * @param dst - destination char* string.
 *              It is converted string without
 *              leading and trailing whitespaces.
 *              If passed source string cntains only whitespaces
 *              result destination string will be empty (only "\0")
 * @return In case of success:  0
 *         In case of error: NON zero value indicating error
 */
static int
aux_wstrdup_trimmed(
  const xmlChar* src, 
  wchar_t** wdst, 
  st_conversion_settings* p_cs)
{
  int res;
  int src_sz = 0;
  int dst_sz = 0;
  int li = 0, ri = 0;
  const xmlChar* beg;
  char* u8dst = 0;
  size_t wdst_chars_sz = 0;
  st_conversion_settings tmp_cs;
  unsigned char tmp_cs_inited = 0;
  
  src_sz = xmlStrlen(src);
  if (src_sz)
  {
    for (li = 0; li < src_sz; ++li) 
    {
      if (!isspace(src[li]))
      {
        break;
      }
    }

    for (ri = src_sz - 1; ri >= li; --ri) 
    {
      if (!isspace(src[ri]))
      {
        break;
      }
    }
    
    if (ri >= li)
    {
      dst_sz = ri - li + 1;    
    }
  }
  
  u8dst = (char*) calloc(dst_sz + 1, sizeof(char));
  if (u8dst == 0)
  {
    return ERRCODE_OUT_OF_MEM; 
  }
  
  beg = src + li;
  strncpy(u8dst, (const char*)	beg, dst_sz);

  if (p_cs == 0)
  {
    res = init_st_conversion_settings(&tmp_cs);
    if (res  != 0)
    {
      free(u8dst);
      return res;
    }
    p_cs = &tmp_cs;
    tmp_cs_inited = 1;
  }

  //printf("Filename in mhl '%s', len = %d\n", u8dst, strlen(u8dst));
  
  res = 
    convert_from_utf8_to_wchar(u8dst, dst_sz, wdst, &wdst_chars_sz, p_cs);

  free(u8dst);
  if (tmp_cs_inited)
  {
    free_st_conversion_settings(&tmp_cs);
  }

  return res;
}

//---------------------------------------------------------
//
// Set of functiopns for working with st_mhl_file_content
// structures and its substructures
//
//---------------------------------------------------------

int 
init_mhl_file_check_wdata(st_mhl_file_check_wdata* p_witem)
{
  if (p_witem == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  memset((void*)p_witem, 0, sizeof(*p_witem) / sizeof(char));
  return 0;
}

void free_mhl_file_check_wdata(st_mhl_file_check_wdata* p_witem)
{
  if (p_witem == 0)
  {
    return;
  }
  
  free(p_witem->item_wfilename);
  free(p_witem->abs_item_wfilename);
  free(p_witem->u8str_hash_sum);
  free(p_witem->parent_mhl_wfilename);
  
  memset((void*)p_witem, 0, sizeof(*p_witem) / sizeof(char));
}

int 
add_to_mhl_file_check_wdata(
  st_mhl_file_check_wdata** p_hash_wroot,
  st_mhl_file_check_wdata* p_check_wdata)
{
  if (p_hash_wroot == 0 || p_check_wdata == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }

  //HASH_ADD_STR(*p_hash_root, abs_item_filename, p_check_data);
  HASH_ADD_KEYPTR(
    hh,
    *p_hash_wroot, 
    p_check_wdata->abs_item_wfilename, 
    wcslen(p_check_wdata->abs_item_wfilename) * sizeof(wchar_t), 
    p_check_wdata);

  return 0;
}

st_mhl_file_check_wdata* 
search_for_mhl_file_check_wdata(
  st_mhl_file_check_wdata* p_hash_wroot,
  const wchar_t* wkey)
{
  st_mhl_file_check_wdata* ps_witem;
  HASH_FIND(hh, p_hash_wroot, wkey, (wcslen(wkey) * sizeof(wchar_t)), ps_witem);
  //HASH_FIND_STR(p_hash_wroot, wkey, s_witem);
  return ps_witem;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
int init_mhl_file_wcontent(st_mhl_file_wcontent* wcontent)
{
  if (wcontent == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  memset((void*)wcontent, 0, sizeof(*wcontent) / sizeof(char));
  return 0;
}


void free_mhl_file_wcontent(st_mhl_file_wcontent* wcontent)
{
  st_mhl_file_check_wdata* current_check_wdata;
  st_mhl_file_check_wdata* tmp_check_wdata;
  
  if (wcontent == 0)
  {
    return;
  }

  // remove items
  HASH_ITER(hh, wcontent->check_witems, current_check_wdata, tmp_check_wdata) 
  {
    HASH_DEL(wcontent->check_witems, current_check_wdata); //release content
    free_mhl_file_check_wdata(current_check_wdata);
    free(current_check_wdata); //release memory occuped by check data item
  }

  memset((void*)wcontent, 0, sizeof(*wcontent) / sizeof(char));
}

//---------------------------------------------------------
//
// Set of functions for search for and parsing of MHL files 
//
//---------------------------------------------------------
static
int 
receive_filename(const wchar_t* file_wname, void* data)
{
  wchar_t** mhl_file_wpath;
  
  if (file_wname == 0 || file_wname[0] == '\0' || data == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  mhl_file_wpath = (wchar_t**) data;
  *mhl_file_wpath = mhlosi_wstrdup(file_wname);
  if (*mhl_file_wpath == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  // First mhl file found. It will be parsed. No need other mhl files anymore. 
  return ERRCODE_STOP_SEARCH;
}

int search_mhl_wfile( 
  const wchar_t* file_wpath, 
  wchar_t** mhl_file_wpath,
  st_conversion_settings* p_cs)
{
  int res;
  const wchar_t* inner_wpath;
  wchar_t* outer_wpath;
  wchar_t* mhl_file_wname;
  wchar_t* tmp_wpath = NULL;
  unsigned char root_reached = 0;
  PATTERN_MATCHING_FLAGS mflags;
  DIR_ENTRY_TYPE_FLAGS entry_type;
  //char* path_to_dir;  
  
  inner_wpath = file_wpath;
  outer_wpath = NULL;
  mhl_file_wname = 0;
  *mhl_file_wpath = 0;
  
  while (!root_reached)
  {
    res = extract_wdir_from_wpath(inner_wpath, &outer_wpath);
    
    if (res != 0)
    {
      free(outer_wpath);
      return res;
    }
    
    if (outer_wpath != NULL)
    {
#ifdef WIN
      mflags = PMF_CASE_INSENSITIVE;
#else
      mflags = PMF_CASE_SENSITIVE;
#endif
      entry_type = DETF_FILE;
      
      res = 
        search_entries_in_wdir(
          outer_wpath, 
          MHL_FILE_WPATTERN, 
          PMP_MATCH_AT_THE_END,
          mflags, 
          entry_type, 
          p_cs,
          (void*) &mhl_file_wname,                     
          receive_filename);
      
      if (res || mhl_file_wname != 0)
      {
        // error or mhl found

        if (mhl_file_wname != 0)
        {
          if (res == 0)
          {
            res = concat_wpath_parts(outer_wpath, mhl_file_wname,
                                     mhl_file_wpath);
          }

          free(mhl_file_wname);
        }

        free(outer_wpath);
        break;
      }      
      
      free(tmp_wpath);
      tmp_wpath = outer_wpath;
      inner_wpath = outer_wpath;
    }
    else 
    {
      root_reached = 1;
    }
  } // while (!root_reached)
 
  free(tmp_wpath);
  
  if (res != 0 && res != ERRCODE_STOP_SEARCH)
  {
    return res;
  }

  return *mhl_file_wpath == 0 ? ERRCODE_MHL_NOT_FOUND : 0;
}

static int
aux_parse_name_wfile(
  xmlDocPtr doc, 
  xmlNodePtr cur, 
  const wchar_t* mhl_base_wdir, 
  st_mhl_file_check_wdata* p_witem,
  st_conversion_settings* p_cs)
{
  int res;
  xmlChar* data = 0;
  
  // get filename from mhl file
  data = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
  if (data == 0) 
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  // convert filename from xmlChar* to wchar_t*
  res = aux_wstrdup_trimmed(data, &p_witem->item_wfilename, p_cs);
  xmlFree(data);
  if (res != 0)
  {
    return res;
  }
  
  // convert filepath separators to os specific separators
  make_wpath_os_specific(p_witem->item_wfilename);

  // create absolute path
  res = 
    create_absolute_normalized_wpath(
      mhl_base_wdir,
      p_witem->item_wfilename,
      &p_witem->abs_item_wfilename);
  if (res != 0)
  {
    p_witem->abs_item_wfilename = 0;
    return res;
  }
  
  return  0;
}

static int
aux_parse_file_size(
  xmlDocPtr doc, 
  xmlNodePtr cur, 
  st_mhl_file_check_wdata* p_witem)
{
  int res = 0;
  xmlChar* data = 0;
  
  // get filename from mhl file
  data = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
  if (data == 0) 
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  errno = 0;
  p_witem->file_sz = mhlosi_strtoull((const char*) data, 0, 10);
  if (errno != 0)
  {
    // filesize conversion error, wrong number format
    p_witem->is_file_sz_set = 0;
    res = ERRCODE_WRONG_MHL_FORMAT;
  }
  else 
  {
    // filesize converted successfully
    p_witem->is_file_sz_set = 1;
    res = 0;
  }
  
  xmlFree(data);
  return res;
}


static int
aux_parse_hash_type(
  xmlDocPtr doc, 
  xmlNodePtr cur, 
  st_mhl_file_check_wdata* p_witem)
{
  int res = 0;
  xmlChar* data = 0;

    
  if (p_witem->hash_type == MHL_HT_SHA1)
  {
    // hash data is already filled with primary hash
    return 0;
  }

  if (!xmlStrcmp(cur->name, (const xmlChar *)"md5"))
  {
    p_witem->hash_type = MHL_HT_MD5;
    p_witem->hash_bytes_sz = MHL_MD5_HASH_BYTES_SZ;
  }
  else if (!xmlStrcmp(cur->name, (const xmlChar *)"sha1"))
  {
    p_witem->hash_type = MHL_HT_SHA1;
    p_witem->hash_bytes_sz = MHL_SHA1_HASH_BYTES_SZ;    
  }
  else if (!xmlStrcmp(cur->name, (const xmlChar *)"xxhash"))
  {
      p_witem->hash_type = MHL_HT_XXHASH;
      p_witem->hash_bytes_sz = MHL_XXHASH_HASH_BYTES_SZ;
  }
  else if (!xmlStrcmp(cur->name, (const xmlChar *)"xxhash64"))
  {
      p_witem->hash_type = MHL_HT_XXHASH64;
      p_witem->hash_bytes_sz = MHL_XXHASH64_HASH_BYTES_SZ;
  }
  else if (!xmlStrcmp(cur->name, (const xmlChar *)"xxhash64be"))
  {
      p_witem->hash_type = MHL_HT_XXHASH64BE;
      p_witem->hash_bytes_sz = MHL_XXHASH64BE_HASH_BYTES_SZ;
  }
  else if (!xmlStrcmp(cur->name, (const xmlChar *) "null"))
  {
    p_witem->hash_type = MHL_HT_NULL;
    p_witem->hash_bytes_sz = 0;
  }
  else
  {
    // unsupported hash type
    return ERRCODE_WRONG_MHL_FORMAT;
  }

  if (MHL_HT_NULL == p_witem->hash_type) {
    return 0;
  }
  // read hash value
  data = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
  if (data == 0) 
  {
    return ERRCODE_OUT_OF_MEM;
  }
  
  //
  // TODO: may be need to trim data string
  //
  res = xmlStrlen(data);
  if ((res / 2) != p_witem->hash_bytes_sz)
  {
    // read size of hash sum is not equal to
    // size of hash sum for given hash algorithm
    xmlFree(data);
    return ERRCODE_WRONG_MHL_FORMAT;
  }
  
  if (p_witem->u8str_hash_sum != NULL)
  {
    // hash string is already filled with previous hash sum
    // May be not primary hash md5 is written there
    free(p_witem->u8str_hash_sum);
    p_witem->u8str_hash_sum = NULL;
  }

  // convert hash value from xmlChar* to char*
  res = aux_strdup_trimmed(data, &p_witem->u8str_hash_sum);
  xmlFree(data);
  return res;  
}

static int
aux_parse_hash(
  const wchar_t* mhl_base_dir, 
  xmlDocPtr doc, 
  xmlNodePtr cur, 
  st_mhl_file_wcontent* p_mhl_wcontent,
  st_conversion_settings* p_cs)
{
  int res;
  unsigned char push_to_list;
  st_mhl_file_check_wdata* p_check_witem = NULL;
  st_mhl_file_check_wdata* p_search_witem = NULL;
  xmlChar* attr_value;
  wchar_t* parent_dir_wpath;
  
  //
  p_check_witem = 
    (st_mhl_file_check_wdata*) calloc(1, sizeof(st_mhl_file_check_wdata));

  if (p_check_witem == 0)
  {
    return ERRCODE_OUT_OF_MEM;
  }
  res = init_mhl_file_check_wdata(p_check_witem);
  if (res != 0)
  {
    free(p_check_witem);
  }

  p_check_witem->data_type = MHL_IT_REGULAR_FILE;
  //
  // check is MHL item is reference
  //
  // TODO: Need to clarify: is "referencehhashlist" correct name?
  //
  attr_value = xmlGetProp(cur, (const xmlChar*)"referencehhashlist");
  if (attr_value != NULL)
  {
    if (xmlStrcmp(attr_value, (const xmlChar*)"yes"))
    {
      p_check_witem->data_type = MHL_IT_MHL_FILE;
    }
    
    xmlFree(attr_value);
  }
  
  cur = cur->xmlChildrenNode;
  while (cur != NULL) 
  {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"file"))) 
    {
      //UTF8 - normalized (lowercased)
      res = aux_parse_name_wfile(doc, cur, mhl_base_dir, p_check_witem, p_cs);
      if (res != 0)
      {
        free_mhl_file_check_wdata(p_check_witem);
        free(p_check_witem);
        return res;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"size"))) 
    {
      res = aux_parse_file_size(doc, cur, p_check_witem);
      if (res != 0)
      {
        free_mhl_file_check_wdata(p_check_witem);
        free(p_check_witem);
        return res;
      }
    }
    else if ((!xmlStrcmp(cur->name, (const xmlChar *)"md5")) ||
             (!xmlStrcmp(cur->name, (const xmlChar *)"sha1")) ||
             (!xmlStrcmp(cur->name, (const xmlChar *)"xxhash")) ||
             (!xmlStrcmp(cur->name, (const xmlChar *)"xxhash64")) ||
             (!xmlStrcmp(cur->name, (const xmlChar *)"xxhash64be")) ||
             (!xmlStrcmp(cur->name, (const xmlChar *)"null")))
    {
      res = aux_parse_hash_type(doc, cur, p_check_witem);
      if (res != 0)
      {
        free_mhl_file_check_wdata(p_check_witem);
        free(p_check_witem);
        return res;
      }
    }

    cur = cur->next;
  }
  
  // check parsed values corectness
  if (p_check_witem->is_file_sz_set == 0 || p_check_witem->item_wfilename == 0 ||
      p_check_witem->abs_item_wfilename == 0 ||
      p_check_witem->hash_type == MHL_HT_UNRECOGNIZED || 
      (p_check_witem->u8str_hash_sum == 0 && MHL_HT_NULL != p_check_witem->hash_type))
  {
    free_mhl_file_check_wdata(p_check_witem);
    free(p_check_witem);
    return ERRCODE_WRONG_MHL_FORMAT;    
  }
  
  // Check existence
  p_search_witem = 
    search_for_mhl_file_check_wdata(
      p_mhl_wcontent->check_witems, 
      p_check_witem->abs_item_wfilename);

  res = 0;
  push_to_list = 1;
  if (p_search_witem)
  {
    // Already in list check it
    if (p_check_witem->data_type == MHL_IT_MHL_FILE)
    {
        // oops! cyclic reference
        res = ERRCODE_WRONG_MHL_FORMAT;
    }

    //TODO: clarify if file really need to be checked the same file several times?
    //      currently duplicates are skipped
    push_to_list = 0;
  }

  if (res != 0 || push_to_list == 0)
  {
    free_mhl_file_check_wdata(p_check_witem);
    free(p_check_witem);

    return res;
  }

  res = 
    add_to_mhl_file_check_wdata(
      &(p_mhl_wcontent->check_witems),
      p_check_witem);

  if (res != 0)
  {
    free_mhl_file_check_wdata(p_check_witem);
    free(p_check_witem);

    return res;
  }

  if (p_check_witem->data_type == MHL_IT_MHL_FILE)
  {
    //
    // load hash reference
    //

    // get base dir for hashreference
    res = 
      extract_wdir_from_wpath(
        p_check_witem->abs_item_wfilename, 
        &parent_dir_wpath);

    if (res == 0)
    {
      // load items from hash reference
      res = aux_parse_hash(parent_dir_wpath, doc, cur, p_mhl_wcontent, p_cs);
      free(parent_dir_wpath);
    }
  }

  return res;
}

int parse_mhl_wfile(
  const wchar_t* mhl_file_wpath, 
  st_mhl_file_wcontent* mhl_wcontent,
  st_conversion_settings* p_cs)
{
  int res;
  xmlDocPtr doc;
  xmlNodePtr cur;
  wchar_t* mhl_base_wdir = 0;
  int mhl_fd;

  // add debug info for libxml 
  LIBXML_TEST_VERSION
  
  if (does_wpath_exist(mhl_file_wpath) == 0)
  {
    return ERRCODE_MHL_NOT_FOUND;
  }
  
  // parse XML
  /*
  doc = xmlParseFile(mhl_file_path);
  if (doc == NULL) 
  {
    fprintf(stderr, "Failed to parse %s\n", mhl_file_path);
	  return ERRCODE_WRONG_MHL_FORMAT;
  }
  */
  res = wopen_for_read(mhl_file_wpath, &mhl_fd);
  if (res != 0)
  {
    return res;
  }

  doc = xmlReadFd(mhl_fd, "", NULL, XML_PARSE_HUGE);
  if (doc == NULL) 
  {
    fwprintf(stderr, L"Failed to parse %s\n", mhl_file_wpath);

    mhlosi_close(mhl_fd);
	  return ERRCODE_WRONG_MHL_FORMAT;
  }

  cur = xmlDocGetRootElement(doc);
  if (cur == NULL)
  {
    fwprintf(stderr, L"MHL file %s is empty\n", mhl_file_wpath);
	  
    xmlFreeDoc(doc);
    xmlCleanupParser(); // Cleanup function for the XML library
    mhlosi_close(mhl_fd);
	  return ERRCODE_WRONG_MHL_FORMAT;
  }
  
  if (xmlStrcmp(cur->name, (const xmlChar *) "hashlist")) 
  {
    fwprintf(
      stderr,
      L"MHL file %s of the wrong type, root node is not hashlist\n",
      mhl_file_wpath);
    
	  xmlFreeDoc(doc);
    xmlCleanupParser(); // Cleanup function for the XML library
    mhlosi_close(mhl_fd);
	  return ERRCODE_WRONG_MHL_FORMAT;
  } else {
    xmlChar* version = xmlGetProp(cur, (const xmlChar *) "version");

    double v = atof((const char *) version);
    if (v > 1.1) {
      fwprintf(
               stderr,
               L"MHL file version %s is not compatible with this tool. Please use a newer version.\n",
               version);
      return ERRCODE_WRONG_MHL_FORMAT;
    }
  }
  
  res = extract_wdir_from_wpath(mhl_file_wpath, &mhl_base_wdir);
  if (res != 0 && mhl_base_wdir == 0)
  {
    fwprintf(
      stderr,
      L"Cannot extract path from MHL file %s\n",
      mhl_file_wpath);
    
	  xmlFreeDoc(doc);
    xmlCleanupParser(); // Cleanup function for the XML library
    mhlosi_close(mhl_fd);

    return res != 0 ? res : ERRCODE_WRONG_FILE_LOCATION;
  }
  
  cur = cur->xmlChildrenNode; 
  while (cur != NULL) 
  {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"hash")))
    {
      res = aux_parse_hash(mhl_base_wdir, doc, cur, mhl_wcontent, p_cs);
      if (res != 0)
      {
        fwprintf(stderr, L"MHL file %s is empty\n", mhl_file_wpath);
        
        xmlFreeDoc(doc);
        xmlCleanupParser(); // Cleanup function for the XML library
        mhlosi_close(mhl_fd);
        return ERRCODE_WRONG_MHL_FORMAT;        
      }
    }
     
    cur = cur->next;
  }
  
  xmlFreeDoc(doc);
  xmlCleanupParser(); // Cleanup function for the XML library
  mhlosi_close(mhl_fd);

  return 0;
}
