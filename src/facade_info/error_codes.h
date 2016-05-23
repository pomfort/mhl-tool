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
#ifndef _MHL_TOOLS_FACADE_INFO_ERROR_CODES_H_
#define _MHL_TOOLS_FACADE_INFO_ERROR_CODES_H_

//#define RETCODE_OK 0

// Error codes definitions
#define ERRCODE_UNKNOWN_ERROR   1
#define ERRCODE_WRONG_ARGUMENTS 2
#define ERRCODE_NO_SUCH_FILE    3
#define ERRCODE_IO_ERROR        4
#define ERRCODE_OUT_OF_MEM      5
#define ERRCODE_WRONG_INPUT_FORMAT 6
#define ERRCODE_UNRECOGNIZED_TIME 7
#define ERRCODE_INTERNAL_ERROR 8
#define ERRCODE_WRONG_FILE_LOCATION 9
#define ERRCODE_MHL_NOT_FOUND 10
#define ERRCODE_WRONG_MHL_FORMAT 11
#define ERRCODE_NOT_FILE 13
#define ERRCODE_OPENSSL_ERROR 14
#define ERRCODE_MHL_CHECK_FILE_SIZE_FAILED 15
#define ERRCODE_MHL_CHECK_HASH_FAILED 16
#define ERRCODE_MHL_PARSE_ERROR_UNSUPPORTED_ENCODING 17
#define ERRCODE_CHARS_CONVERSION_ERROR 18
#define ERRCODE_UNKNOWN_MODE 19
#define ERRCODE_NOT_IMPLEMENTED 20
#define ERRCODE_STOP_SEARCH 21
#define ERRCODE_INVALID_SEQUENCE 22
#define ERRCODE_GAP_IN_SEQUENCE 23
#define ERRCODE_MHL_CHECK_NO_MHL_ENTRY 24
#define ERRCODE_INITXXHASH_ERROR 25

#define TOTAL_CODES_NUM 25

const char* mhl_error_code_description(int error_code);

#endif //_MHL_TOOLS_FACADE_INFO_ERROR_CODES_H_
