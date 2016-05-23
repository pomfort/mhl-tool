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
#include <stdio.h>

#include <facade_info/error_codes.h>

#include <mhl_help/help_topics.h>
#include <mhltools_common/logging.h>

int print_mhlseal_help()
{
  //print_error("NOTE: corresponding functionality is not implemented yet.\n");
  return printf("NAME\n"
      "   mhl-seal -- Seal folders or files\n\n"
      "SYNOPSIS\n"
     "   1. mhl seal [-vv] FOLDER\n"
      "   2. mhl seal [-vv] [-#] FILEPATTERN\n"
      "   3. mhl seal [-vv] [-#] -o MHL_FOLDER FILEPATTERN\n\n"
      "DESCRIPTION\n"
      "   In the first synopsis form 'mhl seal' takes a folder as an argument "
      "and creates an MHL file in that folder. This is the preferred way to "
      "seal folders.\n"
//      "The MHL file references all files contained by the folder and its "
//      "subfolders. File patterns contained by the '.mhlignore' file are "
//      "excluded. Each excluded file is logged as a warning to stdout.\n"
//      "If the FOLDER already contains an MHL file, a warning is logged.\n"
      "   In the second synopsis form 'mhl seal' takes file(s) as arguments and "
      "creates an MHL file at the lowest common subfolder.\n"
      "   In the third synopsis form 'mhl seal' takes file(s) as arguments and "
      "creates an MHL file in the MHL_FOLDER.\n\n"
      "EXAMPLES\n"
      "   Seal a folder:\n"
      "      $ mhl seal -v /path/to/folder\n"
/*      "      > MHL file path(s):\n"*/
/*      "      > ./<folderName>_<date>_<time>.mhl\n"*/
      "   Create a MHL file for all 'mov' files within a folder:\n"
      "      $ mhl seal -v /path/to/folder/*.mov\n"
/*      "      > MHL file path(s):\n" */
/*      "      > ./<folderName>_<date>_<time>.mhl\n" */
      "   Create a MHL file for all files in a subfolder recursively and "
      "create the MHL file in the containing folder:\n"
      "      $ mhl seal -v  -o /path/to/folder /path/to/folder/subfolder\n"
/*     "      > MHL file path(s):\n" */
/*      "      > /path/to/folder/<folderName>_<date>_<time>.mhl\n\n" */
      "\n"
      "ARGUMENTS\n"
      "   FOLDER\n"
      "      Folder to seal. This will create an MHL file for all files in "
      "the folder, recursively. (e.g. dir for dir/file1 and dir/file2).\n"
      "   FILEPATTERN\n"
      "      Files to create MHL files for. Fileglobs (e.g. *.mov) can be "
      "given to specify multiple matching files.\n"
      "      If the '-#' option is given, the FILEPATTERN is interpreted "
      "as a file sequence with the syntax described in help topic "
      "'sequence_syntax'.\n"
      "      Symbolic links are not followed. Sockets, FIFOs, etc. are "
      "ignored.\n"
      "   MHL_FOLDER\n"
      "      A path to a folder. The command 'mhl create' will store a MHL "
      "file in the given folder. This folder must to be located above all "
      "files.\n\n"
      "OPTIONS\n"
      "   -o, --output-folder\n"
      "      Creates a MHL file in the given MHL_FOLDER. Multiple "
      "--output-folder options can be given to create multiple MHL files. "
      "In this case, each MHL file only contains hashes for the files which "
      "are located in the corresponding MHL_FOLDER or one of its subfolders. "
      "If relative paths are given which are located outside of all "
      "MHL_FOLDER(S), an error is thrown.\n"
      "   -#, --file-sequence\n"
      "      Looks for a file sequence as described in \"FILE SEQUENCE FORMAT\".\n"
      "   -v, --verbose\n"
      "      Prints status and result.\n"
      "   -vv, --very-verbose\n"
      "      Same as -v, additionally prints progress.\n"
//      "   -y\n"
//      "      Produce an output in a machine readable format. See help on output.\n\n"
      "\n"
      "DIAGNOSTICS\n"
      "   The 'mhl seal' command exits 0 on success, and >0 if an error occurs.\n\n"
      "SEE ALSO\n"
      "   mhl-verify, mhl-hash, mhl-file, pfncopy, openssl\n\n"
      "MHL\n"
      "   Part of the mhl suite\n\n");
}

int print_mhlverify_help()
{
  return printf("NAME\n"
      "   mhl-verify -- Verify folders and Media Hash List (MHL) files\n\n"
      "SYNOPSIS\n"
//      "   1. mhl verify [-vv] [-an] FOLDER\n"
      "   1. mhl verify [-vv] -f "/*[-anc] */"MHL_FILE\n"
      "   2. mhl verify [-vv] -e -f "/*[-ac] */"MHL_FILE\n\n"
      "DESCRIPTION\n"
/*      "   In the first synopsis form 'mhl verify' ensures the completeness "
      "and the consistency of the given FOLDER. This is the preferred way to "
      "verify sealed folders.\n"
      "      1. 'mhl verify' recursively searches for all MHL files "
      "contained by the FOLDER. It fails if no MHL files are found.\n"
      "      2. 'mhl verify' recursively searches for all other files "
      "contained by the FOLDER. It fails if any files are found which are not "
      "referenced by a MHL file. Files contained by the 'file ignores' section "
      "of the ~/.mhlconfig file are ignored.\n"
      "      3. 'mhl verify' compares the hashes stored in the MHL files with "
      "the referenced files on disk. It fails if there is a mismatch in any of "
      "the files.\n"
      "      4. 'mhl verify' creates a new MHL file, which references the "
      "found MHL mfiles and therefore reseals the folder.\n"
*/
      "   In the first synopsis form 'mhl verify' verifies the hashes stored "
      "in the MHL_FILE against the referenced files on disk.\n"/* A new MHL file is "
      "created which references the MHL_FILE.\n" */
      "   In the second synopsis form 'mhl verify' only checks if the files "
      "referenced by MHL_FILE are existent on disk.\n" /* No new MHL file is "
      "created.\n\n"*/
      "\n"
      "EXAMPLES\n"
 /*     "   Verify the completeness and consistency of a folder:\n"
      "      $ mhl verify -v /path/to/folder\n"
      "      > Found MHL file at path /path/to/folder/<folderName>_<date>_<time>.mhl\n"
      "      > Found unreferenced file at path /path/to/folder/example.mov\n"
      "      > Completeness broken, Seal damaged\n"
*/
      "   Verifies the contents of a MHL file:\n"
      "      $ mhl verify -f /path/to/file.mhl\n"
/*      "      > Checking of MHL file content successful.\n" */
      "   Verify the existence of all files references by a MHL file.\n" /* and "
      "checks if there are unreferenced files in the folder containing the "
      "MHL file:\n"*/
/*      "      $ mhl verify -v -e -f /path/to/file.mhl\n" */
/*      "      > Checking of MHL file content successful.\n" */
/*      "      > 3 files are not referenced by the MHL file.\n\n" */
      "\n"
      "ARGUMENTS\n"
//      "   FOLDER\n"
//      "      Folder to verify\n"
      "   MHL_FILE\n"
      "      A path to a MHL file. The MHL file must adhere to the MHL format "
      "(see help topic 'mhl_format')\n\n"
      "OPTIONS\n"
/*      "   -c, --completeness\n"
      "      Checks if there are unreferenced files in the folder containing "
      "the MHL file. This is the default when verifying folders, but must be "
      "explicitly stated when verifying MHL files.\n"
      "   -a, --abort-on-error\n"
      "      If multiple files are given, 'mhl verify' by default continues "
      "if the verification of individual files fails. By specifying this "
      "option, 'mhl verify' will abort execution after the first encountered "
      "error.\n"
      "   -n, --no-reseal\n"
      "      Does not create a new MHL file upon completion. By default a new "
      "MHL file is created if the verification was successful.\n"
*/
      "   -e, --existence\n"
      "      Checks if the files referenced by MHL_FILE are existent on disk "
      "but does not compare hashes. Resealing is not possible if this option "
      "is passed.\n"
      "   -v, --verbose\n"
      "      Prints status and result.\n"
      "   -vv, --very-verbose\n"
      "      Same as -v, additionally prints progress.\n"
//      "   -y\n"
//      "      Produce an output in a machine readable format. See help on output.\n"
      "DIAGNOSTICS\n"
      "   The 'mhl verify' utility exits 0 on success, and >0 if an error occurs.\n\n"
      "SEE ALSO\n"
      "   mhl-seal, mhl-hash, mhl-file, openssl\n\n"
      "MHL\n"
      "   Part of the mhl suite\n\n");
}

int print_mhlhash_help()
{
  return printf("NAME\n"
      "   mhl-hash -- Creates and verifies files against hash values.\n\n"
      "SYNOPSIS\n"
      "   1. mhl hash [-vvm] [-#] [-t TYPES] FILEPATTERN\n"
      "   2. mhl hash [-vvm] -f FILE -h HASH\n"
      "   3. mhl hash [-vvm] "/*[-a] */"-s\n\n"
      "DESCRIPTION\n"
      "   In the first synopsis form 'mhl hash' creates and prints hash "
      "values from the given files. If no explicit hash format is given, "
      "creates md5 hashes. The results are printed to standard out.\n"
      "   In the second synopsis form 'mhl hash' compares the hash value "
      "of the FILE with the given HASH.\n"
      "   In the third synopsis form 'mhl hash' reads hash values "
      "as described in help topic 'hash_syntax' from stdin and compares "
      "them to the corresponding files.\n\n"
      "EXAMPLES\n"
      "   Create MD5 hashes for all movie files in a folder:\n"
      "      $ mhl hash -s -t md5 /path/to/files/*.mov\n"
      "      > MD5(/path/to/file3.mov)= ed7aa19a907a105af61f047db8d0b228\n"
      "   Create MD5 hashes for a file sequence:\n"
      "      $ mhl hash -# /path/to/sequence###12-33.dpx\n"
      "      > MD5(/path/to/sequence012.dpx)= ed7aa19a907a105af61f047db8d0b228\n"
      "      > MD5(/path/to/sequence013.dpx)= c95098251e22b102806ed17e0995f477\n"
      "      > ...\n"
      "   Verify single movie file with a given hash value:\n"
      "      $ mhl hash -v -c /path/to/file.mov -h SHA1:a79302bfa825e1a57af2695177fe50c57984ec10\n"
      "      > Summary: SUCCEEDED\n\n"
      "ARGUMENTS\n"
      "   FILEPATTERN\n"
      "      Files to create MHL files for. Fileglobs (e.g. *.mov) can be "
      "given to specify multiple matching files.\n"
      "      Also a leading directory name can be given to create an MHL file "
      "for all files in the directory, recursively. (e.g. dir for dir/file1 "
      "and dir/file2).\n"
      "      If the '-#' option is given, the FILEPATTERN is interpreted "
      "as a file sequence with the syntax described in help topic "
      "'sequence_syntax'.\n"
      "      Symbolic links are not followed. Sockets, FIFOs, etc. are "
      "ignored.\n"
      "   FILE\n"
      "      A path to a file.\n"
      "   HASH\n"
      "      A hash string in either MD5, SHA1, xxHash, xxHash64 or xxHash64BE format.\n"
      "   TYPES\n"
      "      A list of hash types. Possible types are \"md5\", \"sha1\", \"xxHash\", \"xxHash64\" and \"xxHash64BE\".\n\n"
      "OPTIONS\n"
      "   -s, --stdin\n"
      "      Causes 'mhl hash' to read hash values from stdin and compare \n"
      "them to the corresponding files. This is especially useful for \n"
      "comparing the hash values of file sequences\n"
/*      "   -a, --abort-on-error\n"
      "      By default, if 'mhl hash' is reading hashes via stdin (-s) it "
      "continues running if the verification of individual files fails. "
      "By specifying this option, 'mhl hash' will abort execution after the "
      "first encountered error.\n"
*/
      "   -f, --file\n"
      "      Scans the given FILE and compares its hash value with the "
      "given HASH. If no explicit hash format (md5, sha1, xxhash, xxhash64, xxhash64be) is given, mhlhash "
      "will automatically determine the hash format by the length of the "
      "HASH.\n"
      "   -v, --verbose\n"
      "      Prints status and result.\n"
      "   -vv, --very-verbose\n"
      "      Same as -v, additionally prints progress.\n"
//      "   -m, --machine-readable\n"
//      "      Outputs machine readable log messages. A detailed description "
//      "of output format can be found in help topic 'machine_output'.\n"
      "   -#, --file-sequence\n"
      "      Looks for a file sequence as described in \"FILE SEQUENCE FORMAT\".\n"
//      "   -p, --print-all\n"
//      "      Prints all output to stdout. By default, only output messages are "
//      "logged to stdout, error and status messages are not.\n"
//      "   -t, --types\n"
//      "     Scans the given PATH and outputs hash value of the given TYPES.\n\n"
      "DIAGNOSTICS\n"
      "   The 'mhl hash' command exits 0 on success, and >0 if an error occurs.\n\n"
      "SEE ALSO\n"
      "   mhl-seal, mhl-verify, mhl-file, pfncopy, openssl\n\n"
      "MHL\n"
      "   Part of the mhl suite\n\n");
}

int print_mhlfile_help()
{
  return printf("NAME\n"
      "   mhl-file -- Create and parse Media Hash List (MHL) files\n\n"
      "SYNOPSIS\n"
      "   1. mhl file [-vv] -s [-o MHL_FOLDER]\n"
      "   2. mhl file [-vv] -f FILE [-o MHL_FOLDER]\n"
 //     "   3. mhl file [-vv] -p MHL_FILE\n\n"
      "DESCRIPTION\n"
      "   In the first synopsis form 'mhl file' takes input through stdin. "
      "The input syntax is described in help topic 'hash_syntax'\n"
      "   In the second synopsis form 'mhl file' reads input from the FILE. "
      "The file must contain hash values as described in the help topic "
      "\"hash_syntax\", separated by newlines.\n\n"
//      "   In the third synopsis form 'mhl file' reads the MHL_FILE and outputs the hash values "
//      "of the corresponding files in the format described in help topic "
//      "\"hash_syntax\", separated by newlines.\n"
      "EXAMPLES\n"
      "   Create a MHL file for media files in a folder with use of "
      "'mhl hash':\n"
      "      $ mhl hash -c /path/to/folder/*.mov | mhl file -s -v -o /path/to/folder\n"
      "      > MHL file path(s):\n"
      "      > /path/to/folder/<folderName>_<date>_<time>.mhl\n"
      "   Create a MHL file for media files in a folder with use of openssl:\n"
      "      $ openssl dgst -md5 /path/to/files/ -name \"*.mov\" | "
      "mhl file -s -v -o /path/to/folder\n"
      "      > MHL file path(s):\n"
      "      > /path/to/folder/<folderName>_<date>_<time>.mhl\n\n"
      "ARGUMENTS\n"
      "   FILE\n"
      "      A path to a file. Symbolic links are not followed. Sockets, FIFOs, etc. are ignored.\n"
//      "   MHL_FILE\n"
//      "      A path to a MHL file. The MHL file must adhere to the MHL format "
//      "(see help topic 'mhl_format')\n"
      "   MHL_FOLDER\n"
      "      A path to a folder. 'mhl file' will store the MHL file in the "
      "given folder. This folder has to be above all files that will be given "
      "as relative paths via stdin.\n\n"
      "OPTIONS\n"
      "   -s, --stdin\n"
      "      Takes input from stdin\n"
      "   -f, --file\n"
      "      Takes input from the given FILE\n"
      "   -o, --output-folder\n"
      "      Creates a MHL file in the given MHL_FOLDER. Multiple "
      "--output-folder options can be given to create multiple MHL files. "
      "In this case, each MHL file only contains hashes for the files which "
      "are located in the corresponding MHL_FOLDER or one of its subfolders. "
      "If relative paths are given which are located outside of all "
      "MHL_FOLDER(S), an error is thrown.\n"
      "   -v, --verbose\n"
      "      Prints status and result\n"
      "   -vv, --very-verbose\n"
      "      Same as -v, additionally prints progress\n\n"
//      "   -y\n"
//      "      Produce an output in a machine readable format. See help on output.\n"
//      "   -p, --parse\n"
//      "      Parses the given MHL_FILE and outputs the hash values of the "
//      "corresponding files in the syntax described in help topic 'hash_syntax'.\n\n"
      "DIAGNOSTICS\n"
      "   The 'mhl file' utility exits 0 on success, and >0 if an error occurs.\n\n"
      "SEE ALSO\n"
      "   mhl-seal, mhl-verify, mhl-hash, pfncopy, openssl\n\n"
      "MHL\n"
      "   Part of the mhl suite\n\n");
}

int print_hash_syntax_help()
{
  return printf("hash_syntax\n\n"
    "mhlfile expects hash input to adhere to the following syntax:\n"
    "HASH_TYPE(PATH)=HASH_VALUE\n"
    "For example:\n"
    "MD5(/Users/csr/Desktop/Test/a_01.tif)= 95711be5982521a645ddf51c87b511a8\n"
    "This holds true for both input via stdin, file input, as well as output.\n"
    "Hash input with this syntax is generated by both the 'mhl hash' or "
    "openssl command.\n\n");
}

int print_sequence_syntax_help()
{
  return printf("sequence_format\n\n"
    "The file sequence format allows you to address sequences of files with "
    "one identifier. A file sequence identifier consists of four strings: "
    "prefix, padding, starting index, ending index and postfix.\n"
    "The identifier /sequences/scan_###1-300.dpx would be split up into the "
    "parts\n"
    "- Prefix /sequences/scan_\n"
    "- Padding ###\n"
    "- Starting index 1\n"
    "- Ending index 300\n"
    "- Postfix .dpx\n"
    "The padding specifies the number of leading zeros before the frame "
    "index.\n"
    "Three hash characters mean the index string is padded to length three.\n"
    "The Starting Index specifies the index of the first frame in the "
    "sequence.\n"
    "The Ending Index specifies the index of the last frame in the sequence.\n"
    "The example above would include the following files:\n"
    "/sequences/scan_001.dpx\n"
    "/sequences/scan_002.dpx\n"
    "...\n"
    "/sequences/scan_299.dpx\n"
    "/sequences/scan_300.dpx\n\n");
}

int print_mhl_files_help()
{
  return printf("mhl_files\n\n"
  "MHL FILE SYNTAX\n"
  "The given MHL_FILE must conform to the MHL format (draft 1.4), which can "
  "be found in the download section of mediahashlist.org. The syntax must "
  "adhere to the MHL XML schema, which can also be found in the download "
  "section of mediahashlist.org\n\n"
  "MHL FILE NAMING\n"
  "The MHL tool names the created MHL file as follows:\n"
  "<foldername>_<date>_<time>-<qualifier>.mhl\n"
  "where the placeholders are substituted with the following:\n"
  "<foldername> - name of the current directory, e.g. 'Movies'\n"
  "<date> - current date in format YYYY-MM-DD, e.g. '2013-05-10'\n"
  "<time> - current time in format HHMMSS, e.g. '111527'\n"
  "<qualifier>\n"
  "   - 'complete' if the MHL file was created with the first synopsis "
  "form of 'mhl seal'\n"
  "   - 'partial' if the MHL file was created with any other synopsis "
  "form of 'mhl seal'\n\n"
  "MHL FILE LOCATION\n"
  "The MHL tool requires the MHL file to be placed along the path of the "
  "referenced file. This allows for an easy discovery when looking for an MHL "
  "file for a specific file.\n"
  "For example, when creating a MHL file for the file\n"
  "   /Example/Movies/Clip1.mov\n"
  "the MHL file can be put in the following locations:\n"
  "   /Example/Movies/\n"
  "   /Example/\n"
  "   /\n"
  "By default, the MHL file is located in the current directory you were in "
  "when calling the MHL tool (see section 'MHL file naming' above). You can "
  "manually specify the location of the MHL file, by passing the '--outputpath' "
  "option to the MHL tool.\n\n");
}

int print_machine_output_help()
{
  print_error("NOTE: corresponding functionality is not implemented yet.\n");
  return printf("machine_output\n\n"
    "The machine readable output has the format | <column1> | <column2>| ... |\n"
    "where the value of the columns is as follows:\n\n"
    "Status messages\n"
    "   -Tool name\n"
    "   -Message type (output, status or error)\n"
    "   -Progress in percent (without percent sign)\n"
    "   -Files processed\n"
    "   -Files overall\n"
    "   -MB processed (without MB qualifier)\n"
    "   -MB overall (without MB qualifier)\n"
    "Example of a 'mhl hash' status message:\n"
    "mhl hash|status|93|3|4|454|829\n\n"
    "Error messages\n"
    "   -Tool name\n"
    "   -Message type (output, status or error)\n"
    "   -Error code\n"
    "   -Error message\n"
    "   -Failure description\n"
    "Example of a 'mhl hash' error message:\n"
    "mhl hash|error|101|File does not exist|The given file at path "
    "/Movies/A001R1KL.mov does not exist\n\n"
    "Output messages\n"
    "   -Tool name\n"
    "   -Message type (output, status or error)\n"
    "   -Message subtype (scan or compare)\n"
    "   -File path\n"
    "   -Output (checksum or comparison result)\n"
    "Examples of a 'mhl hash' output message:\n"
    "mhl hash|result|scan|/Movies/A001R1KL.mov|md5:95711be5982521a645ddf51c87b511a8|"
    "sha1:a79302bfa825e1a57af2695177fe50c57984ec10\n"
    "mhl hash|result|compare|/Movies/A001R1KL.mov|1\n\n");
}

int print_exit_codes_help()
{
  int res;
  int i_res = 0;
  int ex_code;

  res = printf("exit_codes\n\n"
               " 0  Success\n");

  for (ex_code = 1; ex_code <= TOTAL_CODES_NUM; ++ex_code)
  {
    i_res = printf("%2u  %s\n", ex_code, mhl_error_code_description(ex_code));
    res += i_res;
  }

  i_res = printf("\n");
  res += i_res;
  return res;
}

int print_ignore_patterns_help()
{
  print_error("NOTE: corresponding functionality is not implemented yet.\n");
  return printf("ignore_patterns\n\n"
    "Almost every operating system provides means to hide files from the "
    "user. For example, if a file name on Mac OS X and Linux begins with "
    "a dot (\".\") that file is hidden in the file browser. However, these "
    "files can usually be displayed when explicitly requested.\n\n"
    "Some operating systems write hidden files to store accompanying data "
    "for files and folders. For example, Mac OS X writes thumbnails of files "
    "to a file named .DS_Store. Since these files may be changed by the "
    "operating system at any time, they should not be referenced by a MHL "
    "file.\n\n"
    "Therefore, you can exclude files from the sealing and verification by "
    "adding them to the .mhlignore file in your user home directory. Each "
    "line in the .mhlignore file specifies a pattern in the 'shell glob' "
    "syntax:\n"
    "   * - matches zero or more of any character\n"
    "   ? - matches any one character\n"
    "   [list] - matches any one character in the list of characters; "
    "the list can contain a range of characters such as [a-zA-Z] "
    "and can be negated/complemented by using ^ or ! at the start, "
    "e.g. [^a-zA-Z] = not a letter; [!0-9] = not a digit\n\n"
    "If no .mhlignore file exists, 'mhl' will create one for you. By default, "
    "the following patterns are excluded:\n"
    "   .DS_STORE                  #Finder thumbnails\n"
    "   .fseventsd/                #Mac OS X Internals\n"
    "   .VolumeIcon.icns           #Mac OS X Volume Icons\n"
    "   .Trashes/                  #Mac OS X Disk Trash\n"
    "   .background/               #Mac OS X Internals\n"
    "   .hotfiles.btree/           #Mac OS X Internals\n"
    "   .DocumentRevisions-V100/   #Mac OS X Internals\n"
    "   .Spotlight-V100/           #Mac OS X Internals\n"
    "   .apdisk                    #Time Machine Internals\n"
    "   .TemporaryItems/           #Time Machine Internals\n"
    "   ._*                        #Mac OS X Resource Forks\n"
// TODO: enumerate
    "Also common Linux and Windows excludes are here\n\n");
}

int print_config_file_help()
{
  print_error("NOTE: corresponding functionality is not implemented yet.\n");
  return printf("config_file\n\n"
    "The .mhlconfig file contains a range of settings for the 'mhl' tool.\n"
    "The .mhlconfig file is located in your user home directory. "
    "It follows the syntax:\n"
    "   [<categoryName>]\n"
    "   <optionName> = <optionValue>\n"
    "Below is a list of options which can be configured in the .mhlconfig "
    "file.\n"
    "   Category : user\n"
    "      Option : name (string)\n"
    "      Option : email (string)\n"
    "      Option : address (string)\n"
    "      Option : phone (string)\n"
    "      Option : company (string)\n"
    "      Option : certificate (string : file path)\n"
    "   Category : project\n"
    "      Option : name (string)\n"
    "      ...\n\n");
}