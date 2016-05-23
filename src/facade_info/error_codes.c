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
#include <facade_info/error_codes.h>

const char* 
mhl_error_code_description(int error_code)
{
  switch (error_code) 
  {
    case ERRCODE_WRONG_ARGUMENTS:
      return "Wrong or incompatible arguments are passed.";
    
    case ERRCODE_NO_SUCH_FILE:
      return "File does not exist.";
      
    case ERRCODE_IO_ERROR:
      return "IO error occured.";
      
    case ERRCODE_OUT_OF_MEM:
      return "Out of memory.";
      
    case ERRCODE_WRONG_INPUT_FORMAT:
      return "Wrong or incompatible input data.";
      
    case ERRCODE_UNRECOGNIZED_TIME:
      return "Unknown time format.";
      
    case ERRCODE_INTERNAL_ERROR:
      return "Internal error occured.";
      
    case ERRCODE_WRONG_FILE_LOCATION:
      return "Wrong file location.";
      
    case ERRCODE_MHL_NOT_FOUND:
      return "MHL file is not found.";
      
    case ERRCODE_WRONG_MHL_FORMAT:
      return "Wrong or unsupported MHL file format.";
      
    case ERRCODE_MHL_CHECK_FILE_SIZE_FAILED:
      return "Real file size and the size contained "
             "in MHL file record are not equal.";

    case ERRCODE_MHL_CHECK_HASH_FAILED:
      return "Calculated hash of file and "
             "hash from corresponding MHL file record are not equal.";
      
    case ERRCODE_MHL_PARSE_ERROR_UNSUPPORTED_ENCODING:
      return "One or more items of MHL file "
             "contain an unsupported hash encoding";
      
    case ERRCODE_CHARS_CONVERSION_ERROR:
      return "Error during conversion of character encodings";

    case ERRCODE_NOT_FILE:
      return "File not found.";
      
    case ERRCODE_OPENSSL_ERROR:
      return "Error in crypto library occured.";
      
    case ERRCODE_UNKNOWN_MODE:
      return "Unknown mode.";
      
    case ERRCODE_NOT_IMPLEMENTED:
      return "Functionality is not implemented yet.";

    case ERRCODE_UNKNOWN_ERROR:
      return "Unknown error.";
      
    case ERRCODE_STOP_SEARCH:
      return "Search stopped.";
     
    case ERRCODE_INVALID_SEQUENCE:
      return "Invalid sequence specification. Format: <start of file name>#...#<number1>-<number2><end of file name>";
   
    case ERRCODE_GAP_IN_SEQUENCE:
      return "Gap detected in the file sequence. One or more files are missing or can't be opened.";

    case ERRCODE_MHL_CHECK_NO_MHL_ENTRY:
      return "File is not listed in MHL file.";
 
    default:
      return "The code is not used, probably reserved for future.";
  }
}
