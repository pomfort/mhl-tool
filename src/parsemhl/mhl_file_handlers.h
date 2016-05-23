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

#ifndef _MHL_TOOLS_PARSEMHL_MHL_FILE_HANDLERS_H_
#define _MHL_TOOLS_PARSEMHL_MHL_FILE_HANDLERS_H_

#include <third_party/uthash.h>
#include <generics/char_conversions.h>

typedef enum MHL_ITEM_TYPE
{
  MHL_IT_REGULAR_FILE = 0,
  MHL_IT_MHL_FILE
} MHL_ITEM_TYPE;

//
// This structure contains data according to "<hash>" tag from 
// MHL file, plus some helper data
//
typedef struct _st_mhl_file_check_wdata
{
  MHL_ITEM_TYPE data_type;
  //
  wchar_t* abs_item_wfilename; //key in the hash table
  wchar_t* item_wfilename; //filename from <hash> tag of mhl file

  //
  wchar_t* parent_mhl_wfilename;

  //
  MHL_HASH_TYPE hash_type;
  unsigned int hash_bytes_sz;
  char* u8str_hash_sum;
    
  //
  unsigned long long file_sz;
  unsigned char is_file_sz_set;

  time_t lastmodification_seconds;

  //
  UT_hash_handle hh; // structutre is hashable now
} st_mhl_file_check_wdata;

//
// List of st_mhl_file_item_data, according to list of "<hash>"
// tags from MHL file.
//
typedef struct _st_mhl_file_wcontent
{
  st_mhl_file_check_wdata* check_witems;     /* important! initialize to NULL */
} st_mhl_file_wcontent;

//------------------------------------------------------------------------
//
// Structures for working with 
//
//------------------------------------------------------------------------

/*
 * Inits instance of st_mhl_file_item_data
 * 
 * @param item - pointer to item to be inited
 * @return In case of success: 0,
 *         in case of failure: non zero value with error code
 */
int init_mhl_file_check_wdata(st_mhl_file_check_wdata* p_check_witem);

/*
 * Frees memory used in st_mhl_file_item_data structure
 * @param item - poiner to st_mhl_file_item_data struct, which
 *               wil be freed
 */
void free_mhl_file_check_wdata(st_mhl_file_check_wdata* p_check_witem);

/*
 * Adds item to st_mhl_file_check_data hash
 *
 * Note: Memory for item MUST BE ALLOCATED, dont use stack memory here
 * Note: after call to this function, ownership over "p_check_data" will be 
 *       passed to "hashtale" 
 *       Plesase do not free 'p_check_data" after successfull call to this 
 *       function.
 *
 * @param p_hash_root pointer to root of st_mhl_file_check_data hashtable
 * @param p_check_data pointer to new instance of 
 *                     st_mhl_file_check_data structure
 *
 * @return In case of success: 0, 
 *         in case of failure: non zero value with error code
 */
int add_to_mhl_file_check_wdata(
      st_mhl_file_check_wdata** p_hash_wroot,
      st_mhl_file_check_wdata* p_check_wdata);    


/*
 * Search item in st_mhl_file_check_data hash
 *
 * Note: Don free memory, pointed to returned pointer from tis function
 *
 * @param p_hash_root pointer to root of st_mhl_file_check_data hashtable
 * @param key hash item key
 *
 * @return In case of success: pointer to st_mhl_file_check_data with this key 
 *         in case of failure: NULL
 */
st_mhl_file_check_wdata* search_for_mhl_file_check_wdata(
      st_mhl_file_check_wdata* p_hash_wroot,
      const wchar_t* wkey);    

//---------------------------------------------------------
//
// Set of functions for working with st_mhl_file_content
// structures and its substructures
//
//---------------------------------------------------------

/* Init st_mhl_file_content struct.
 * @pararm content - pointer to st_mhl_file_content structure, 
 *         which will be inited
 * @return In case of success: 0.
 *         In case of failure: non zero value with error code.
 */
int init_mhl_file_wcontent(st_mhl_file_wcontent* wcontent);

/* Free memoryused bty instance of st_mhl_file_content structure
 * @param item - poiner to st_mhl_file_content structure,
 *               which will be freed.
 */
void free_mhl_file_wcontent(st_mhl_file_wcontent* wcontent);


//---------------------------------------------------------
//
// Set of functions for search for and parsing of MHL files 
//
//---------------------------------------------------------

int search_mhl_wfile(
  const wchar_t* file_wpath, 
  wchar_t** mhl_file_wpath,
  st_conversion_settings* p_cs);

int parse_mhl_wfile(
  const wchar_t* mhl_file_wpath, 
  st_mhl_file_wcontent* mhl_wcontent, 
  st_conversion_settings* p_cs);

#endif //_MHL_TOOLS_PARSEMHL_MHL_FILE_HANDLERS_H_
