import platform
import os.path

# TODO: Think how to auto-configure the variables below

#define ERRCODE_UNKNOWN_ERROR   1
ERRCODE_WRONG_ARGUMENTS = 2
ERRCODE_NO_SUCH_FILE = 3
#define ERRCODE_IO_ERROR        4
#define ERRCODE_OUT_OF_MEM      5
ERRCODE_WRONG_INPUT_FORMAT = 6
#define ERRCODE_UNRECOGNIZED_TIME 7
#define ERRCODE_INTERNAL_ERROR 8
#define ERRCODE_WRONG_FILE_LOCATION 9
#define ERRCODE_MHL_NOT_FOUND 10
#define ERRCODE_WRONG_MHL_FORMAT 11
#define ERRCODE_NOT_FILE 13
#define ERRCODE_OPENSSL_ERROR 14
ERRCODE_MHL_CHECK_FILE_SIZE_FAILED = 15
ERRCODE_MHL_CHECK_HASH_FAILED = 16
#define ERRCODE_MHL_PARSE_ERROR_UNSUPPORTED_ENCODING 17
#define ERRCODE_CHARS_CONVERSION_ERROR 18
#define ERRCODE_UNKNOWN_MODE 19
#define ERRCODE_NOT_IMPLEMENTED 20
#define ERRCODE_STOP_SEARCH 21
ERRCODE_INVALID_SEQUENCE = 22
ERRCODE_GAP_IN_SEQUENCE = 23


system_type = platform.system()
if system_type == 'Windows':
    bin_dir = 'Windows_7_x64'
    tmp_path = os.path.join("..", "dev_envs", bin_dir, "OpenSSL-Win32", "bin",
                            "openssl.exe")

    openssl_path = os.path.abspath(tmp_path)
    # This is an approximate value since Windows has the limit to filepath,
    # which is still possible to overcome. Completely the other test is needed
    # for Windows
    longname_len = 100
elif system_type == 'Linux':
    bin_dir = 'Ubuntu_12.04_x64'
    openssl_path = '/usr/bin/openssl'
    longname_len = 0
elif system_type == 'Darwin':
    bin_dir = 'MacOSX'
    openssl_path = '/usr/bin/openssl'
    longname_len = 0
else:
    assert False, "Unknown platfom: %s" % system

test_dir = u'test_dir'
aux_dir = 'aux_files'

utils_dir = os.path.join("..", "bin", bin_dir, "Release")
logger_name = 'mhltestslogger'
MHL_end_pattern = '.mhl'
dir_pattern = 'testname_dir\(([^\)]+)\)'
file_pattern = 'testname_file\(([^\)]+)\)'
