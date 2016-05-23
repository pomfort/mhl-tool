HASHING_WRONG_SIZE = 1
HASHING_WRONG_HASH = 2
CHECK_NO_OPTIONS = 3
MHLFILE_INCORRECT_INPUT = 4
MHLHASH_CORRECT_OPTIONS = 5
MHLFILE_FILE_INPUT = 6
MHLFILE_CORRECT_OPTIONS = 7
INCORRECT_OPTIONS = 8
FILE_PATHS = 9
ABSOLUTE_FILE_PATHS = 10
LONG_NAMES = 11
NON_ASCII_TEST = 12
MHL_CHARSETS_TEST = 13
SEQUENCES_TEST = 14
BIG_FILES_TEST = 15
MANY_FILES_TEST = 16
MHLFILE_MULTIPLE_OUTDIR = 17
MHLFILE_BASIC = 18
RECURSIVE_DIR_PROCESS = 19

scen_defs = {
"Deleted last bytes": HASHING_WRONG_SIZE,
"Change last bytes": HASHING_WRONG_HASH,
"No options in a given command": CHECK_NO_OPTIONS,
"mhlfile: correct options and incorrect input": MHLFILE_INCORRECT_INPUT,
"mhlhash: correct options in different order": MHLHASH_CORRECT_OPTIONS,
"mhlfile: input from file": MHLFILE_FILE_INPUT,
"mhlfile: correct options in different order": MHLFILE_CORRECT_OPTIONS,
"mhlfile: multiple '--output-folder' specification": MHLFILE_MULTIPLE_OUTDIR,
"mhlhash: recursive directory calculate": RECURSIVE_DIR_PROCESS,
"mhlseal and mhlverify: recursive directory and multiple '-o' testing": MHLFILE_MULTIPLE_OUTDIR,
"Incorrect options": INCORRECT_OPTIONS,
"Test for mhl file": MHLFILE_BASIC,
"Test for mhl seal": MHLFILE_BASIC,
"mhl hash and file: Files and folders paths and asterisk": FILE_PATHS,
"mhl seal and verify: Files and folders paths and asterisk": FILE_PATHS,
"mhl hash and file: Absolute filepaths": ABSOLUTE_FILE_PATHS,
"mhl seal and verify: Absolute filepaths": ABSOLUTE_FILE_PATHS,
"Long file name, directory name, MHL file name": LONG_NAMES,
"Only non-ASCII characters in filepaths, system default NFC & NFD": NON_ASCII_TEST,
"relative test non-ASCII characters in filepaths, system default NFC & NFD": RECURSIVE_DIR_PROCESS,
"Mixed ASCII and non-ASCII characters in filepaths NFC": NON_ASCII_TEST,
"Mixed ASCII and non-ASCII characters in filepaths NFD": NON_ASCII_TEST,
"Non-ASCII European characters in filepaths NFC": NON_ASCII_TEST,
"Non-ASCII European characters in filepaths NFD": NON_ASCII_TEST,
"mhlhash: different encodings in MHL file": MHL_CHARSETS_TEST,
"mhlsign: different encodings in MHL file": MHL_CHARSETS_TEST,
"Sequences parsing failure": SEQUENCES_TEST,
"Gaps in sequences failure": SEQUENCES_TEST,
"Success work with sequences": SEQUENCES_TEST,
"Absolute test for file sizes 2Gb +2b, 4Gb +2b, 8Gb +2b": BIG_FILES_TEST,
"Relative test for big files": BIG_FILES_TEST,
"mhlhash: work with large number of files": MANY_FILES_TEST,
"Work with large number of files": MANY_FILES_TEST
}

def get_test_case_id(name):
    scen_id = scen_defs[name]
    return scen_id
#    return 0
