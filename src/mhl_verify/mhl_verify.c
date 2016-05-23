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
#include <string.h>
#include <errno.h>

#include <facade_info/error_codes.h>
#include <generics/os_check.h>
#include <generics/char_conversions.h>
#include <generics/std_funcs_os_anonymizer.h>
#include <generics/filesystem_handlers/public_interface.h>

#include <mhltools_common/hashing.h>

#include <mhltools_common/controlling_data.h>
#include <args_fileslist_support/aux_funcs.h>
#include <mhl_verify/verify_options.h>

#include <mhl_verify/mhl_verification/check_file.h>
#include <mhl_verify/mhl_verification/mhlverify.h>
#include <mhltools_common/usage_printing.h>
#include <mhltools_common/options.h>

typedef enum _en_verify_modes
{
  MD_NOT_SET = 0,
  MD_1_CHECK_MHL = 1
} en_verify_modes;

typedef struct _st_options
{
  // common options for all applications
  st_controlling_data common;

  // Common option for all modes
  en_verify_modes mode; // see en_modes enum
  
  // common options for mhl verification
  st_mhl_verify_options verify;

  // help params
  unsigned char help_topic; // see help_topics enum
} st_options;

int init_st_options(st_options* p_opt)
{
  int res;
  if (p_opt == 0)
  {
    return ERRCODE_WRONG_ARGUMENTS;
  }
  
  memset((void*) p_opt, 0, sizeof(*p_opt) / sizeof(char));
  
  res = init_mhl_verify_options(&p_opt->verify);
  if (res != 0)
  {
    return res;
  }
  
  return 0;
}

void free_st_options(st_options* p_opt)
{
  if (p_opt == 0)
  {
    return;
  }
  
  fini_mhl_verify_options(&p_opt->verify);
  
  memset((void*) p_opt, 0, sizeof(*p_opt) / sizeof(char));  
  return;
}

// returns in case of success: 0
//         in case of failure: error code, and print error message to stderr
int parse_mhlverify_params(
  int argc, 
  const char * argv[], 
  st_options* opts, 
  st_conversion_settings* p_cs)
{
  int i;
  int ires;
  en_opts res;
  en_opts res1;

  if (argc < 2)
  {
    print_error(
      "Arguments error: "
      "Incorrect number of arguments.\n");
    return ERRCODE_WRONG_ARGUMENTS;
  }
  opts->common.logging_data.tool_name = "mhl verify";
  // traverse through arguments
  i = 1;
  do
  {
    res = recognise_option(argv[i]); 
    switch (res)
    {
    case OPT_V:
      if (opts->common.logging_data.v_data.verbose_level == VL_VERY_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      opts->common.logging_data.v_data.verbose_level = VL_VERBOSE;
      break;

    case OPT_VV:
      if (opts->common.logging_data.v_data.verbose_level == VL_VERBOSE)
      {
        print_error(
          "Arguments error: "
          "Only one verbose option of '-v' or '-vv' "
          "may be specified.\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      opts->common.logging_data.v_data.verbose_level = VL_VERY_VERBOSE;
      break;

    case OPT_Y:
      opts->common.logging_data.v_data.machine_output = 1;
      break;

    case OPT_SEQ:
      if ((opts->mode == MD_NOT_SET) || (opts->mode == MD_1_CHECK_MHL))
      {
        opts->common.use_sequences = 1;
      }
      else
      {
         print_error(
           "Arguments error: "
           "The '-s' option may be used only in the synopsis mode 1\n");
         return ERRCODE_WRONG_ARGUMENTS;
      }
      break;

    case OPT_E:
      if ((opts->mode == MD_NOT_SET) || (opts->mode == MD_1_CHECK_MHL))
      {
        opts->verify.existence = 1;
        opts->mode = MD_1_CHECK_MHL;
      }
      else
      {
        print_error(
          "Arguments error: "
          "The '-e' option can be used only in mhl checking mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      break;

    case OPT_F:
      if ((opts->mode != MD_1_CHECK_MHL) && (opts->mode != MD_NOT_SET))
      {
        print_error(
          "Arguments error: "
          "The '-f' option can be used only in mhl checking mode\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      opts->verify.f_option = 1;
      opts->mode = MD_1_CHECK_MHL;

      res1 = recognise_option(argv[++i]);
      if (res1 == NOT_OPT) 
      {
        // ? TODO: check .md5|.sha1|.mhl
        //opts->f_mhl = mhlosi_strdup(argv[i]);
        
        // convert to UTF32
        opts->verify.f_wmhl = 
          strdup_and_convert_composed_from_locale_to_wchar(
            argv[i], p_cs, &ires);

        if (opts->verify.f_wmhl == NULL)
        {
          return ires == 0 ? ERRCODE_OUT_OF_MEM : ires;
        }          

        make_wpath_os_specific(opts->verify.f_wmhl);
      }
      else
      {
        print_error(
          "Arguments error: "
          "There must be MHL file name after the '-f' option\n");
        return ERRCODE_WRONG_ARGUMENTS;
      }
      break;

    case NOT_OPT:
       // (opts->mode == MD_NOT_SET) and NOT_OPT, which means mode 1,
       // or opts->mode == MD_1_CHECK_MHL

      // files name specification is started"
      // stop params parsing
      opts->mode = MD_1_CHECK_MHL;
      opts->common.files_argv_index = i;
      res = NULL_OPT;
      break;
    case OPT_C:
      opts->verify.continue_on_error = 1;
      break;

    case NULL_OPT:
    default:
      print_error(
        "Arguments error: "
        "Incorrect parameters order or number\n");
      return ERRCODE_WRONG_ARGUMENTS;
    }

    ++i;
  } while (i < argc && (res != NULL_OPT));

  return 0; 
}

int run_mhl_verify(int argc, const char * argv[])
{
  st_options opts;
  int res;
  st_conversion_settings css;

  mhlosi_setlocale();
  res = init_st_conversion_settings(&css);
  if (res != 0)
  {
    fprintf(
      stderr, 
      "Cannot init conversion settings: %s\n", 
      mhl_error_code_description(res));
    
    return res;
  }
  
  res = init_st_options(&opts);
  if (res != 0)
  {
    fprintf(
      stderr, 
      "Initialisation error: %s\n", 
      mhl_error_code_description(res));
    
    mhlverify_usage();

    free_st_conversion_settings(&css);
    return res;
  }
  
  res = parse_mhlverify_params(argc, argv, &opts, &css);
  if (res != 0)
  {
    free_st_options(&opts);
    mhlverify_usage();
    free_st_conversion_settings(&css);
    return res;
  }

  if (opts.mode == MD_1_CHECK_MHL)
  {
    res = verify_mhl(argc, argv, &opts.common, &opts.verify, &css);
  }
  else // opts->mode == MD_NOT_SET
  {
    // Really we should not come here after parameters parsing
    print_error("Unknown program running mode\n");
    mhlverify_usage();
    res = ERRCODE_UNKNOWN_MODE;
  }
  
  free_st_options(&opts);
  free_st_conversion_settings(&css);

  return res;
}
