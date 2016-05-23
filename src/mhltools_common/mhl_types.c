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

#include "mhl_types.h"
#include <assert.h>

const char*
mhl_hash_type_name(MHL_HASH_TYPE type) {
    switch (type) {
        case MHL_HT_UNRECOGNIZED:
            return "UNKNOWN";
        case MHL_HT_MD5:
            return "MD5";
        case MHL_HT_SHA1:
            return "SHA1";
        case MHL_HT_XXHASH:
            return "xxHash";
        case MHL_HT_XXHASH64:
            return "xxHash64";
        case MHL_HT_XXHASH64BE:
            return "xxHash64BE";
        case MHL_HT_NULL:
            return "null";
        default:
            break;
    }
    assert(0 && "unknown hash type");
    return "";
}
