from lettuce import step, before, after
import os.path
import re
import string
import shutil
import subprocess
import sys
from test_env import *
from funcs import *
from test_consistency import *
import time
from lxml import etree

class Testvars(object): pass

# The below strict attributes checking is added:
# - to avoid keeping in mind attribute names for each scenario
#   (as it was when attributes were introduced dynamically in test steps),
# - and to prevent mistakes in attribute names,
# - and for proper attributes cleaning after each scenario
#   to prevent wrong test results and for memory management reasons

class ScenarioSpecificVars(object):
    default_allowed_attrs = {'allowed_attrs'}

    def __init__(self, scen_id):
        self.allowed_attrs = {'scen_id'}
        self.scen_id = scen_id
        self.initialize()

    def __setattr__(self, attr, value):
        if attr not in self.default_allowed_attrs and attr not in self.allowed_attrs:
            raise Exception ("No such attribute: %s" % attr)
        super(ScenarioSpecificVars, self).__setattr__(attr, value)

    def clean(self):
        del self.fileslist[:]
        for dst in self.dst_dirs:
            clean_test_dir(dst)
        self.dst_dirs.clear()

        if self.scen_id in (MHLHASH_CORRECT_OPTIONS,
                            MHLFILE_FILE_INPUT,
                            MHLFILE_CORRECT_OPTIONS,
                            SEQUENCES_TEST,
                            MANY_FILES_TEST,
                            MHLFILE_BASIC):
            del self.test_files[:]

        elif self.scen_id in (FILE_PATHS,
                              ABSOLUTE_FILE_PATHS,
                              MHLFILE_MULTIPLE_OUTDIR,
                              RECURSIVE_DIR_PROCESS):
            self.filenames_dict.clear()
            self.aux_filepaths_dict.clear()
            self.test_filepaths_dict.clear()

        elif self.scen_id == NON_ASCII_TEST:
            self.filenames_dict.clear()
            self.aux_filepaths_dict.clear()
            self.dirnames_dict.clear()

        self.initialize()

    def initialize(self):
        self.allowed_attrs.add('fileslist')
        self.fileslist = []
        self.allowed_attrs.add('dst_dirs')
        self.dst_dirs = {test_dir}
        self.allowed_attrs.add('retcode')
        self.retcode = -1
        self.allowed_attrs.add('use_longnames')
        self.use_longnames = False
        self.allowed_attrs.add('use_abspaths')
        self.use_abspaths = False
        self.allowed_attrs.add('use_testnames')
        self.use_testnames = False
        self.allowed_attrs.add('locale_charset')
        self.locale_charset = None

        if self.scen_id in (HASHING_WRONG_SIZE, HASHING_WRONG_HASH):
            more_attrs = {'filename', 'aux_filepath', 'mhl_filename',
                          'aux_mhl_filepath', 'test_filepath',
                          'test_mhl_filepath'}
            self.allowed_attrs |= more_attrs
            self.filename = ""
            self.aux_filepath = ""
            self.mhl_filename = ""
            self.aux_mhl_filepath = ""
            self.test_filepath = "" # copied file, used for running the util
            self.test_mhl_filepath = "" # copied file, used for running the util

        elif self.scen_id in (CHECK_NO_OPTIONS, MHLFILE_INCORRECT_INPUT,
                              INCORRECT_OPTIONS):
            more_attrs = {'util', 'util_path', 'filename', 'aux_filepath',
                          'test_filepath'}
            self.allowed_attrs |= more_attrs
            self.util = ""
            self.util_path = ""
            self.filename = ""
            self.aux_filepath = ""
            self.test_filepath = "" # copied file, used for running the util

        elif self.scen_id in (MHLHASH_CORRECT_OPTIONS,
                              MHLFILE_FILE_INPUT,
                              MHLFILE_CORRECT_OPTIONS,
                              MHLFILE_BASIC):
            more_attrs = {'util', 'util_path', 'mhl_filename',
                          'aux_mhl_filepath', 'test_mhl_filepath',
                          'test_files', 'created_mhl_filepath'}
            self.allowed_attrs |= more_attrs
            self.util = ""
            self.util_path = ""
            self.mhl_filename = ""
            self.aux_mhl_filepath = ""
            self.test_mhl_filepath = "" # copied file, used for running the util
            self.test_files = []
            self.created_mhl_filepath = "" # the result of mhlfile

        elif self.scen_id in (FILE_PATHS,
                              ABSOLUTE_FILE_PATHS,
                              MHLFILE_MULTIPLE_OUTDIR):
            more_attrs = {'filenames_dict', 'aux_filepaths_dict',
                          'test_filepaths_dict', 'created_mhl_filepath', 'util'}
            self.allowed_attrs |= more_attrs
            self.filenames_dict = {}
            self.aux_filepaths_dict = {}
            self.test_filepaths_dict = {} # copied files, used for_util running
            self.created_mhl_filepath ="" # the result of mhlfile
            self.util = ""
            if self.scen_id == ABSOLUTE_FILE_PATHS:
                self.use_abspaths = True

        elif self.scen_id == LONG_NAMES:
            more_attrs = { 'created_mhl_filepath', 'test_mhl_filepath', 'util'}
            self.allowed_attrs |= more_attrs
            self.use_longnames = True
            self.created_mhl_filepath =""
            self.test_mhl_filepath =""
            self.util = ""

        elif self.scen_id == NON_ASCII_TEST:
            more_attrs = {'filenames_dict', 'aux_filepaths_dict',
                          'dirnames_dict', 'created_mhl_filepath', 'util'}
            self.allowed_attrs |= more_attrs
            self.filenames_dict = {}
            self.aux_filepaths_dict = {}
            self.dirnames_dict = {}
            self.use_testnames = True
            self.created_mhl_filepath ="" # the result of mhlfile
            self.util = ""

        elif self.scen_id == MHL_CHARSETS_TEST:
            more_attrs = {'dirname', 'aux_dirpath', 'mhl_filename',
                          'aux_mhl_filepath', 'util', 'util_path',
                          'test_mhl_filepath'}
            self.allowed_attrs |= more_attrs
            self.dirname = ""
            self.aux_dirpath = ""
            self.mhl_filename = ""
            self.aux_mhl_filepath = ""
            self.util = ""
            self.util_path = ""
            self.test_mhl_filepath = "" # copied file, used for util running

        elif self.scen_id in (SEQUENCES_TEST, MANY_FILES_TEST):
            more_attrs = {'util', 'util_path', 'filename',
                          'aux_filepath', 'test_files', 'mhlhash_output',
                          'created_mhl_filepath'}
            self.allowed_attrs |= more_attrs
            self.util = ""
            self.util_path = ""
            self.filename = ""
            self.aux_filepath = ""
            self.test_files = []
            self.mhlhash_output = ""
            self.created_mhl_filepath = ""

        elif self.scen_id == BIG_FILES_TEST:
            more_attrs = {'util', 'util_path', 'filename', 'test_filepath',
                          'created_mhl_filepath', 'openssl_output',
                          'mhlhash_output'}
            self.allowed_attrs |= more_attrs
            self.util = ""
            self.util_path = ""
            self.filename = "" # created file, used for running the util
            self.test_filepath = "" # created file, used for running the util
            self.created_mhl_filepath ="" # the result of mhlfile
            self.openssl_output = ""
            self.mhlhash_output = ""

        elif self.scen_id == RECURSIVE_DIR_PROCESS:
            more_attrs = {'util', 'util_path',
                          'filenames_dict', 'aux_filepaths_dict',
                          'test_filepaths_dict',
                          'openssl_output', 'mhlhash_output'}
            self.allowed_attrs |= more_attrs
            self.util = ""
            self.util_path = ""
            self.openssl_output = ""
            self.mhlhash_output = ""
            self.filenames_dict = {}
            self.aux_filepaths_dict = {}
            self.test_filepaths_dict = {} # copied files, used for_util running

tv = Testvars()

def clean_everything():
    tv.specific.clean()

# Unfortunatelly, the @after calls (nor scenario, nor step) are not repeated in
# Scenario Outline between Examples, so try to handle this with extra
# try: except: finally: cleaning statements.

@before.each_scenario
def setup_scenario_validation(scenario):
    tv.specific = ScenarioSpecificVars(get_test_case_id(scenario.name))
    if not os.path.isdir(test_dir):
        os.mkdir(test_dir)
    return

@step('I have the file "(.+)" and the corresponding mhl file "(.+)"')
def have_file_and_mhl(step, filename1, filename2):
    step.given('the file is "' + filename1 + '"')
    step.given('the mhl file is "' + filename2 + '"')

@step('the file is "(.+)"')
def have_file(step, filename):
    try:
        filepath = os.path.join(aux_dir, filename)
        logging.debug("Check the file \"" + filepath + "\"\n")
        assert os.path.isfile(filepath), "No such file \"" + filepath + "\"\n"

        tv.specific.filename = filename
        tv.specific.aux_filepath = filepath
    except:
        clean_everything()
        raise

@step(u'the MHL file is "(.+)"')
def have_mhl_file(step, filename):
    try:
        filepath = os.path.join(aux_dir, filename)
        logging.debug("Check the mhl file \"" + filepath + "\"\n")
        assert (re.match(".+\.(mhl|md5|sha1)$", filepath.lower()) and
                os.path.isfile(filepath)), \
               ("No such file \"" + filepath + "\", or the file has wrong " \
                "extension (only .mhl, .md5, .sha1 are valid).\n")

        tv.specific.mhl_filename = filename
        tv.specific.aux_mhl_filepath = filepath
    except:
        clean_everything()
        raise

@step(u'I have the directory "(.+)"')
def have_directory(step, dir_name):
    try:
        dirpath = os.path.join(aux_dir, dir_name)
        logging.debug("Check the directory \"" + dirpath + "\"\n")
        assert os.path.isdir(dirpath), \
               ("No such directory \"" + dirpath + "\"\n")

        tv.specific.dirname = dir_name
        tv.specific.aux_dirpath = dirpath
    except:
        clean_everything()
        raise

@step(u"I'm using '(.+)' charset in filepaths")
def use_locale(step, locale_charset):
    tv.specific.locale_charset = locale_charset

@step('I delete (\d+) bytes at the end of test file')
def delete_N_bytes_at_the_end(step, number):
    try:
        n_bytes = int(number)
        logging.debug("Number of bytes to delete: %d\n", n_bytes)
        assert n_bytes > 0, "The number shall be positive\n"

        with open(tv.specific.test_filepath, 'a') as tmpf:
            tmpf.seek(-n_bytes, os.SEEK_END)
            tmpf.truncate()

        logging.debug("The file \"" + tv.specific.test_filepath + \
                      "\" is truncated for %d bytes\n" % n_bytes)
    except:
        clean_everything()
        raise

@step('I duplicate the given file into "(.+)" directory')
def duplicate_the_given_file(step, dst_dir):
    try:
        dst_dir = test_process_location(dst_dir)
        tv.specific.test_filepath = pure_duplicate_file(
            tv.specific.aux_filepath, dst_dir)

    except:
        clean_everything()
        raise

@step('I duplicate the given files into "(.+)"')
def duplicate_the_given_files(step, dst_dir):
    try:
        dst_dir = test_process_location(dst_dir)

        tv.specific.test_files = []
        for filepath in tv.specific.fileslist:
            test_filepath = pure_duplicate_file(filepath, dst_dir)
            tv.specific.test_files.append(test_filepath)

    except:
        clean_everything()
        raise

@step('I duplicate the given file to the files sequence "(.+)" \(N from \[(.+)\]\) in the "(.+)" directory')
def duplicate_to_sequence(step, seq_pattern, fileslist, dst_dir):
    try:
        dst_dir = test_process_location(dst_dir)

        nums_splitted = fileslist.split(', ')
        for nums in nums_splitted:
            num_range = nums.split('-')
            num_len = len(num_range[0])
            for num in range(int(num_range[0]),int(num_range[len(num_range)-1]) + 1):
                dst_name = re.sub('{N}', str(num).zfill(num_len), seq_pattern)
                dst_path = os.path.join(dst_dir, dst_name)

                test_filepath = pure_duplicate_file(tv.specific.aux_filepath,
                    dst_path, 'file')

                tv.specific.test_files.append(test_filepath)

#        exit()
    except:
#        exit()
        clean_everything()
        raise


@step('I duplicate the (given|created) MHL file (?:into|to) "(.+)" (directory|file).*')
def duplicate_mhlfile(step, file_status, dst_name, dst_type):
    try:
        if file_status == 'given':
            src_mhl_filepath = tv.specific.aux_mhl_filepath
        else:
            src_mhl_filepath = tv.specific.created_mhl_filepath

        if tv.specific.use_longnames:
            dst_name = create_long_name_if_needed(dst_name)

        if dst_type == 'directory':
            dst_name = test_process_location(dst_name)
        else:
            dst_name = os.path.join(os.path.dirname(src_mhl_filepath),
                                    dst_name)

        tv.specific.test_mhl_filepath = pure_duplicate_file(src_mhl_filepath,
                                                            dst_name, dst_type)

    except:
        clean_everything()
        raise

@step(u'I duplicate the given directory into "(.+)" directory')
def duplicate_dir(step, dst_name):
    try:
        assert len(tv.specific.dirname) > 0, "The directory name for copiing "\
            "must be specified before this step\n"

        dst_dir = test_process_location(dst_name)
        dstpath = os.path.join(dst_dir, tv.specific.dirname)

        assert not os.path.exists(dstpath), "The name \"" + dstpath + \
            "\" is already exist\n"

        shutil.copytree(tv.specific.aux_dirpath, dstpath)

    except:
        clean_everything()
        raise

@step('mhl verification check fails due non-matched file size')
def check_file_size_with_mhlverify(step):
    try:
        run_util_and_check_return_code(bin_name("mhl"),
            "verify -e -f " + tv.specific.test_mhl_filepath + " " + tv.specific.test_filepath,
            ERRCODE_MHL_CHECK_FILE_SIZE_FAILED)
    finally:
        clean_everything()

@step(u"I change (\d+) byte(?:s)? at the (end|beginning) of test file to '(\S+)'")
def change_N_bytes(step, number, pos, chars_expr):
    try:
        n_bytes = int(number)
        logging.debug("Number of bytes to change: %d\n", n_bytes)
        assert n_bytes > 0, "The number shall be positive\n"

        chars = chars_expr.decode('string-escape')
        assert len(chars) > 0, "There shall be at least one symbol specified " \
                               "for substituting bytes\n"

        with open(tv.specific.test_filepath, 'r+') as tmpf:
            if pos == 'end':
                tmpf.seek(-n_bytes, os.SEEK_END)
                tmpf.truncate()
            else:
                tmpf.seek(0, os.SEEK_SET)
            n_times, remain = divmod(n_bytes, len(chars))
            tmpf.write(chars*n_times + chars[:remain])

        if pos == 'end':
            logging.debug("The file \"" + tv.specific.test_filepath + "\" " \
                          "is modified: last %d bytes are changed to '%s', " \
                          "repeated the needed number of times\n" \
                          % (n_bytes, chars_expr))
        else:
            logging.debug("The file \"" + tv.specific.test_filepath + "\" " \
                          "is modified: first %d bytes are changed to '%s', " \
                          "repeated the needed number of times\n" \
                          % (n_bytes, chars_expr))
    except:
        clean_everything()
        raise

@step('I create a "(.+)" file in "(.+)" of (\d+) bytes starting from \'(\S+)\', filled with \'(\S+)\', and ending with \'(\S+)\'')
def create_file_of_size(step, f_name, dst_dir, f_size, start_expr, fill_expr, end_expr):
    try:
        dst_dir = test_process_location(dst_dir)
        tv.specific.filename = f_name
        tv.specific.test_filepath = os.path.join(dst_dir, f_name)

        n_bytes = int(f_size)
        logging.debug("Number of bytes to write: %d\n", n_bytes)
        assert n_bytes > 0, "The number shall be positive\n"

        start_orig = start_expr.decode('string-escape')
        start_len_orig = len(start_orig)
        fill_orig = fill_expr.decode('string-escape')
        fill_len_orig = len(fill_orig)
        end_orig = end_expr.decode('string-escape')
        end_len_orig = len(end_orig)
        assert start_len_orig > 0 or fill_len_orig > 0 or end_len_orig > 0, \
               "There shall be at least one symbol " \
               "specified for writing into the file\n"

        chunk_len = 1024

        with open(tv.specific.test_filepath, 'w') as tmpf:
            if start_len_orig + end_len_orig >= n_bytes:
                tmpf.write((start_orig + end_orig)[:n_bytes])

            else:
                tmpf.write(start_orig)
                written = start_len_orig

                n_times, remain = divmod(chunk_len, fill_len_orig)
                chars = fill_orig*n_times + fill_orig[:remain]

                reserve_to_check = chunk_len + end_len_orig
                check_size = written + reserve_to_check

                while check_size < n_bytes:
                    tmpf.write(chars)
                    written += chunk_len
                    check_size += chunk_len

                remain = n_bytes - written
                if remain > end_len_orig:
                    tmpf.write(chars[:(remain - end_len_orig)])
                tmpf.write(end_orig)

        logging.debug("%d bytes are written to the new file " \
                      "\"" + tv.specific.test_filepath + "\"\n", n_bytes)

    except:
        clean_everything()
        raise

@step('mhl verification check fails due non-matched hash sum')
def check_hash_sum_with_mhlverify(step):
    try:
        run_util_and_check_return_code(bin_name("mhl"),
            "verify -f " + tv.specific.test_mhl_filepath + " " + tv.specific.test_filepath,
            ERRCODE_MHL_CHECK_HASH_FAILED)
    finally:
        clean_everything()

@step('mhlhash fails due to invalid sequence specification.')
def check_invalid_sequence_failure(step):
    try:
        check_retcode(tv.specific.util, tv.specific.retcode,
            ERRCODE_INVALID_SEQUENCE)

    finally:
        clean_everything()

@step('mhlhash fails due to gaps in files sequence.')
def check_invalid_sequence_failure(step):
    try:
        check_retcode(tv.specific.util, tv.specific.retcode,
            ERRCODE_GAP_IN_SEQUENCE)

    finally:
        clean_everything()

@step('I have the tool (.+)')
def have_tool(step, util):
    try:
        tv.specific.util = bin_name(util)
        tv.specific.util_path = os.path.join(utils_dir, tv.specific.util)
        assert os.path.isfile(tv.specific.util_path), "No such tool \"" + \
                                                     tv.specific.util_path + "\"\n"
    except:
        clean_everything()
        raise

@step('I have the tools')
def have_tools(step):
    try:
        for recs in step.hashes:
            util_path = os.path.join(utils_dir, bin_name(recs['tool']))
            assert os.path.isfile(util_path), "No such tool \"" + util_path + \
                                              "\"\n"

    except:
        clean_everything()
        raise

@step("I run it (?:without options|with '(.+)')(?: from )?((?<= from ).+)?")
def run_util_with_opts(step, args_str, work_dir):
    try:
        if work_dir:
            work_dir = test_process_location(work_dir)

            tv.specific.retcode = run_tool_with_opts_and_input_from_dir(
                                    tv.specific.util, args_str, work_dir,
                                    locale_charset=tv.specific.locale_charset)

        else:
            tv.specific.retcode = pure_run_util(tv.specific.util_path, args_str,
                                                tv.specific.locale_charset)
    except:
        clean_everything()
        raise

@step('the return code is (\d+)(\.)?')
def check_return_code(step, exp_retcode, final_step):
    try:
        expected_retcode = int(exp_retcode)
        check_retcode(tv.specific.util, tv.specific.retcode, expected_retcode)

    except:
        clean_everything()
#        exit()
        raise

    else:
        if final_step:
            clean_everything()
        else:
            time.sleep(1)

@step("the outputs of 'mhl hash' and openssl are the same")
def compare_mhlhash_and_openssl_outputs(step):
    try:
        openssl_lns = tv.specific.openssl_output.splitlines()
        mhlhash_lns = tv.specific.mhlhash_output.splitlines()
        openssl_len = len(openssl_lns)
        mhlhash_len = len(mhlhash_lns)
        assert openssl_len == mhlhash_len, "The number of strings in openssl "\
               "and mhlhash output is different\n"

        for i in range(openssl_len):
            assert openssl_lns[i] == mhlhash_lns[i], \
            "The outputs of openssl and mhlhash differ in string " \
            + str(i) + ":\n" \
            "openssl output: \"\n" \
            + openssl_lns[i] + "\"\nmhlhash_output: \"\n" \
            + mhlhash_lns[i] + "\"\n"

    except:
        clean_everything()
#        exit()
        raise

    else:
        clean_everything()

@step("I run 'mhl file' (?:without options|with '(.+)')(?: from )?((?<= from ).+)? and provide it '(.+)'")
def run_mhlfile_with_opts_with_input(step, args_str, work_dir, inp_str):
    try:
        tv.specific.util = bin_name("mhl")
        args_str = "file " + args_str
        work_dir = test_process_location(work_dir)

        assert inp_str, "Input string must be specified for this step\n"

        tv.specific.retcode = run_tool_with_opts_and_input_from_dir(
            tv.specific.util, opts=args_str, work_dir=work_dir, inp_str=inp_str,
            locale_charset=tv.specific.locale_charset)
    except:
        clean_everything()
        raise

@step('the files are:')
def the_files_are(step):
    try:
        tv.specific.fileslist = [os.path.join(aux_dir, recs['filename']) for recs in step.hashes]

        logging.debug("Check the files \"" + '", "'.join(tv.specific.fileslist) +\
                      "\"\n")

        for filename in tv.specific.fileslist:
            assert os.path.isfile(filename), "No such file \"" + filename + \
                                             "\"\n"

    except:
        clean_everything()
        raise

@step(u'I have the files')
def have_the_files(step):
    try:
        tv.specific.filenames_dict = {recs['orig_number']: recs['filename'] \
            for recs in step.hashes}

        tv.specific.aux_filepaths_dict = {recs['orig_number']: os.path.join(aux_dir, recs['filename']) \
            for recs in step.hashes}

        for num, filename in tv.specific.aux_filepaths_dict.iteritems():
            logging.debug("Checking the file " + num + ": \"" + filename + \
                          "\"\n")
            assert os.path.isfile(filename), "No such file \"" + filename + \
                          "\"\n"

    except:
        clean_everything()
        raise

@step(u'I am using the following directory names')
def have_dirnames(step):
    try:
        tv.specific.dirnames_dict = {recs['orig_number']: recs['dirname'] \
            for recs in step.hashes}

    except:
        clean_everything()
        raise

@step("I run 'mhl(.*)' from \"(.+)\" with '(.*)'")
def run_util_from_dir_with_opts(step, command, work_dir, args_str):
    try:
        tv.specific.util = bin_name("mhl")
        assert len(args_str)<>0, "Arguments must be specified in this test\n"
        args_str = command + " " + args_str

        logging.debug("initial work_dir = '" + work_dir + "'")

        if tv.specific.use_longnames:
            work_dir = create_long_name_if_needed(work_dir)

        if tv.specific.use_testnames:
            work_dir = get_testname_if_needed(work_dir, dir_pattern,
                                              tv.specific.dirnames_dict)

        work_dir = test_process_location(work_dir)
        logging.debug("final work_dir = '" + work_dir + "'")

        if tv.specific.use_longnames:
            args_str = create_long_name_if_needed(args_str)

        if tv.specific.use_testnames:
            args_str = get_testname_if_needed(args_str, dir_pattern,
                                              tv.specific.dirnames_dict)
            args_str = get_testname_if_needed(args_str, file_pattern,
                                              tv.specific.filenames_dict)

        tv.specific.retcode = run_tool_with_opts_and_input_from_dir(
            tv.specific.util, opts=args_str, work_dir=work_dir,
            abspath_in_opts=tv.specific.use_abspaths,
            locale_charset=tv.specific.locale_charset)

    except:
        clean_everything()
        raise

@step("the '(.+)' file is created in \"([^\"]+)\"(?: relative to \"([^\"]+)\")?")
def check_file_in_dir(step, file_end_pattern, file_dir, rel_dir):
    try:
        if tv.specific.use_longnames:
            file_dir = create_long_name_if_needed(file_dir)

        if tv.specific.use_testnames:
            file_dir = get_testname_if_needed(file_dir, dir_pattern,
                                              tv.specific.dirnames_dict)

        if rel_dir:
            if tv.specific.use_longnames:
                rel_dir = create_long_name_if_needed(rel_dir)

            if tv.specific.use_testnames:
                rel_dir = get_testname_if_needed(rel_dir, dir_pattern,
                                                 tv.specific.dirnames_dict)

            file_dir = os.path.join(rel_dir, file_dir)
        elif tv.specific.use_abspaths:
            (file_dir, n_of_subs) = rel_path_to_absolute_if_needed(file_dir)

        file_dir = test_process_location(file_dir)

        logging.debug("Checking %s for the file matching pattern '%s'",
                      file_dir, file_end_pattern)

        file_found = False
        for node in os.listdir(file_dir):
            nodepath = os.path.join(file_dir, node)
            if (os.path.isfile(nodepath) and
                re.match(".+" + re.escape(file_end_pattern) , node)):

                file_found = True
                logging.info("The '%s' file is found in %s\n", node, file_dir)

                if re.match(MHL_end_pattern, file_end_pattern):
                    tv.specific.created_mhl_filepath = os.path.join(file_dir,
                                                                    node)

                break

        assert file_found, "FAILED: The '%s' file is not found.\n" \
                           % file_end_pattern

#        exit()
    except:
#        exit()
        clean_everything()
        raise

@step("put the given files into special directories(?: with '(.+)' as marker of absolute filepath usage in openssl command)?")
def put_files_into_directories(step, abs_path_marker):
    try:
        assert len(tv.specific.filenames_dict) > 0, "The list of files must be " \
                                                "specified before this step\n"

        for rec in step.hashes:
            orig_file_no = rec['orig_number']
            aux_filepath = tv.specific.aux_filepaths_dict.get(orig_file_no)
            assert aux_filepath, "There is no a file associated with the number " +\
                             orig_file_no + " in the original files list.\n"

            dst_dir = test_process_location(rec['directory_name'])

            new_filepath = pure_duplicate_file(aux_filepath, dst_dir)

            if abs_path_marker and rec[abs_path_marker] == "1":
                new_filepath = os.path.abspath(new_filepath)

            tv.specific.test_filepaths_dict[rec['filenumber']] = new_filepath

    except:
        clean_everything()
        raise



@step('I create the directory tree with the following structure in "(.+)"')
def create_directory_tree(step, dst_dir):
    try:
        assert len(tv.specific.filenames_dict) > 0, "The list of files must be " \
                                                "specified before this step\n"

        assert len(tv.specific.dirnames_dict) > 0, "The list of directories names must be " \
                                              "specified before this step\n"

        dst_dir = test_process_location(dst_dir)

        for rec in step.hashes:
            dirpath = dst_dir
            dirs_splitted = rec['directory_numbered_path'].split('/')
            numbers = re.compile('[0-9]+')

            for dir_handle in dirs_splitted:
                if numbers.match(dir_handle):
                    dirname = tv.specific.dirnames_dict.get(dir_handle)

                    assert dirname, "There is no a directory name associated " \
                           "with the record " + dir_handle + " in the " \
                           "directories list.\n"
                else:
                    dirname = dir_handle

                dirpath = os.path.join(dirpath, dirname)

            if not os.path.isdir(dirpath):
                os.makedirs(dirpath)

            files_splitted = rec['file_numbers'].split(', ')
            for file_handle in files_splitted:
                if numbers.match(file_handle):
                    aux_filepath = tv.specific.aux_filepaths_dict.get(file_handle)
                    assert aux_filepath, "There is no a file associated with "+ \
                           "the record " + file_handle + " in the files list.\n"
                else:
                    aux_filepath = file_handle

                new_file_path = pure_duplicate_file(aux_filepath, dirpath)
                logging.debug("The file '" +aux_filepath+ "' is placed into '" \
                              + dirpath + "'")
                tv.specific.fileslist.append(new_file_path)

    except:
        clean_everything()
        raise

@step("for files \[(.+)\] I run openssl from \"(.+)\" to make (md5|sha1) hash and redirect output to 'mhl file -s (.+)' as input")
def for_files_run_openssl_and_redirect_to_mhlfile(step, fileslist, work_dir,
    hash_type, mhl_dirs):
    try:
        if tv.specific.use_longnames:
            work_dir = create_long_name_if_needed(work_dir)

        work_dir = test_process_location(work_dir)
        args_str = "file -s"
        outpath_option = '-o'

        mhldirs_splitted = mhl_dirs.split('-o ')
        if len(mhldirs_splitted) <= 1:
            mhldirs_splitted = mhl_dirs.split('--output-folder ')
            outpath_option = '--output-folder'

        assert mhldirs_splitted > 1, "Incorrect mhl file options.\n"

        mhldirs_splitted = mhldirs_splitted[1:]

        for mhl_dir in mhldirs_splitted:
            mhl_dir = mhl_dir.strip().strip('"')

            if tv.specific.use_longnames:
                mhl_dir = create_long_name_if_needed(mhl_dir)

            if tv.specific.use_abspaths:
                (mhl_dir, n_of_subs) = rel_path_to_absolute_if_needed(mhl_dir)

                if n_of_subs > 0:
                    test_process_location(mhl_dir)
                else:
                    test_process_location(os.path.join(work_dir, mhl_dir))
            else:
                test_process_location(os.path.join(work_dir, mhl_dir))

            args_str += " " + outpath_option + " \"" + mhl_dir + "\""

        files = []
        files_splitted = fileslist.split(', ')
        numbers = re.compile('[0-9]+')

        for file_handle in files_splitted:
            if numbers.match(file_handle):
                test_filepath = tv.specific.test_filepaths_dict.get(file_handle)
                assert test_filepath, "There is no a file associated with " + \
                                 "the number " + file_handle + " in files list.\n"

                if not os.path.isabs(test_filepath):
                    test_filepath = os.path.relpath(test_filepath, work_dir)

            else:
                if tv.specific.use_longnames:
                    test_filepath = create_long_name_if_needed(file_handle)
                else:
                    test_filepath = file_handle

            files.append(test_filepath)

        tv.specific.retcode = run_openssl_mhlfile(files, work_dir,
                                                  args_str, hash_type)

    except:
        clean_everything()
        raise

@step("for the given files I run openssl from \"(.+)\" to make (md5|sha1) hash and redirect output to 'mhl file -s -o \"(.+)\"' as input")
def for_given_files_run_openssl_and_redirect_to_mhlfile(step, work_dir,
    hash_type, mhl_dir):

    try:
        logging.debug("initial work_dir = '" + work_dir + "'")

        if tv.specific.use_testnames:
            work_dir = get_testname_if_needed(work_dir, dir_pattern,
                                              tv.specific.dirnames_dict)

        work_dir = test_process_location(work_dir)
        logging.debug("final work_dir = '" + work_dir + "'")

        if tv.specific.use_testnames:
            mhl_dir = get_testname_if_needed(mhl_dir, dir_pattern,
                                             tv.specific.dirnames_dict)

        test_process_location(os.path.join(work_dir, mhl_dir))

        args_str = "file -s -o \"" + mhl_dir + "\""

        files = []
        for file_name in tv.specific.fileslist:
            if not os.path.isabs(file_name):
                filename = os.path.relpath(file_name, work_dir)

            else:
                filename = file_name

            files.append(filename)

        tv.specific.retcode = run_openssl_mhlfile(files, work_dir,
                                                  args_str, hash_type)

    except:
        clean_everything()
        raise

@step(ur"from '(?P<work_dir>.+)' I run openssl to make (?P<hashes_spec>(\w+)(\s*,\s*(\w+)\s*?)*(\s+and\s+(\w+))?) hashes for (?P<files_num>the given file|files in specified order) and I run 'mhl hash' with '(?P<hash_opts>.+)'")
def openssl_and_mhlhash_to_compare(step, work_dir, hashes_spec, files_num, hash_opts):
    try:
        work_dir = test_process_location(work_dir)
        hashes = parse_hash_spec(hashes_spec)

        if files_num == "the given file":
            tv.specific.openssl_output = ""
            for hash in hashes:
                openssl_output = run_openssl_for_file(work_dir, hash,
                    tv.specific.filename, tv.specific.locale_charset)

                tv.specific.openssl_output += openssl_output
        else:
            tv.specific.openssl_output = ""
            for rec in step.hashes:
                os_types = rec['OS_type'].split(', ')

                if system_type in os_types:

                    files_splitted = rec['files_order'].split(', ')
                    numbers = re.compile('[0-9]+')

                    for file_handle in files_splitted:
                        if numbers.match(file_handle):
                            test_filepath = tv.specific.test_filepaths_dict.get(
                                            file_handle)

                            assert test_filepath, "There is no a file " + \
                                   "associated with " + \
                                   "the number " + file_handle + \
                                   " in files list.\n"

                        else:
                            test_filepath = file_handle

                        if not os.path.isabs(test_filepath):
                            test_filepath = os.path.relpath(test_filepath,
                                                            work_dir)
                            test_filepath = os.path.join(".", test_filepath)

                        for hash in hashes:
                            openssl_output = run_openssl_for_file(work_dir, hash,
                                test_filepath, tv.specific.locale_charset)

                            tv.specific.openssl_output += openssl_output

        assert len(tv.specific.openssl_output) <>0, "Got an empty output " \
            "string from openssl\n"

        logging.debug("The output string from openssl is:\n\"\n" \
                      + tv.specific.openssl_output + "\"\n")

        tv.specific.mhlhash_output = run_mhlhash_and_return_output(
            work_dir, hash_opts, tv.specific.locale_charset)

        assert len(tv.specific.mhlhash_output) <>0, "Got an empty output " \
            "string from mhlhash\n"

        logging.debug("The output string from mhlhash is:\n\"\n" \
                      + tv.specific.mhlhash_output + "\"\n")

    except:
        clean_everything()
        raise

@step("I run 'mhl hash' from '(.+)' with '(.+)' and redirect output to 'mhl file' as input")
def run_mhlhash_and_redirect_to_mhlfile(step, work_dir, hash_opts):
    try:
        work_dir = test_process_location(work_dir)

        tv.specific.mhlhash_output = run_mhlhash_and_return_output(
            work_dir, hash_opts, tv.specific.locale_charset)

        assert len(tv.specific.mhlhash_output) <>0, "Got an empty input " \
            "string for mhl file\n"

        logging.debug("The input string for mhlfile is:\n\"\n" \
                      + tv.specific.mhlhash_output + "\"\n")

        tv.specific.util = bin_name("mhl")

        tv.specific.retcode = run_tool_with_opts_and_input_from_dir(
            tv.specific.util, opts='file -s', work_dir=work_dir,
            inp_str=tv.specific.mhlhash_output,
            locale_charset=tv.specific.locale_charset)

    except:
        clean_everything()
        raise

def run_openssl_mhlfile(fileslist, work_dir, args_str, hash_type):
    inp_str = ""
    for filename in fileslist:
        openssl_output = run_openssl_for_file(work_dir, hash_type, filename,
            tv.specific.locale_charset)

        inp_str += openssl_output

    assert len(inp_str) <> 0, "Got empty input string for mhl file\n"
    logging.debug("The input string for mhl file is:\n\"\n" + inp_str + \
                   "\"\n")

    tv.specific.util = bin_name("mhl")

    retcode = run_tool_with_opts_and_input_from_dir(
       tv.specific.util, opts=args_str, work_dir=work_dir, inp_str=inp_str,
       locale_charset=tv.specific.locale_charset)

    return retcode


@step("I create in \"(.+)\" a \"(.+)\" (directory|file) .* to get maximal filename length.*")
def create_node_with_long_name(step, dir_name, target_name, target_type):
    dir_name = create_long_name_if_needed(dir_name)
    dir_name = os.path.normpath(dir_name)
    target_name = create_long_name_if_needed(target_name)

    logging.debug("dir_name='" + dir_name + "'")
    logging.debug("target_name='" + target_name + "'")

    try:
        if system_type == 'Windows':
            target_path = u"\\\\?\\" + os.getcwd() + u"\\" + dir_name + u"\\" + target_name
        else:
            target_path = os.getcwd() + u"/" + dir_name + u"/" + target_name

        if target_type == 'directory':
            if not os.path.isdir(target_path):
                os.makedirs(target_path)

        elif not os.path.isfile(target_path):
            fileobj = open(target_path, 'w')
            fileobj.close()

    except (os.error, IOError) as e:
        clean_everything()

        assert False,"Description: " + str(e) + "\n" \
                     "Test error: failed to create the needed test " \
                     "environment: target_path = '" + target_path + "', " \
                     "target_type = '" + target_type + "', " \
                     "dir_name = '" + dir_name + "', " \
                     "target_name = '" + target_name + "'"

    except:
        clean_everything()
        raise

@step(u"the lines of MHL file contain the following substrings:")
def match_MHL_lines(step):
    try:
        with open(tv.specific.created_mhl_filepath) as mhl:
            f_lines = mhl.readlines()

        for rec in step.hashes:
            os_types = rec['OS_type'].split(', ')
            if system_type in os_types:
                lns_splitted = [int(num) for num in rec['line_numbers'].split(', ')]
                lns_splitted.sort()

                lns_num = len(f_lines)
                for ln_no in lns_splitted:
                    assert ln_no < lns_num, ("No line with the number %d in " \
                           "MHL. \'"%(ln_no) +tv.specific.created_mhl_filepath \
                           + "\' contains %d lines\n" % lns_num)

                    assert f_lines[ln_no].find(rec['substring']) >= 0, \
                           ("Line %d of '" % ln_no \
                           + tv.specific.created_mhl_filepath + "' doesn't " \
                           "contain the needed substring.\nThe line is '" \
                           + f_lines[ln_no] + "'\nThe substring is '" \
                           + rec['substring'] + "'\n")

    except:
#        exit()
        clean_everything()
        raise


@step(u"the MHL files contains the following XML tags for the paths:")
def match_MHL_hashes(step):
    try:
        with open(tv.specific.created_mhl_filepath) as mhl:
            mhl_root = etree.parse(mhl)
        for rec in step.hashes:
            expected_tag = rec['tag']
            file = rec['file']
            expected_count = int(rec['count'])

            actual_tag_matches = mhl_root.xpath(u".//%(tag)s[../file[text() = \"%(file)s\"]]" %
            {
                "tag": expected_tag,
                "file": file
            }
            )
            actual_count = len(actual_tag_matches)
            assert actual_count > 0, u"No '%s' tag found for file '%s'" % (expected_tag, file)
            assert actual_count == expected_count, u"Expected to find %d '%s' tags, but found %d" % (expected_count, actual_count, file)
    except:
        clean_everything()
        raise


@step(u"the MHL files contains the following hashes for the paths:")
def match_MHL_hashes(step):
    try:
        with open(tv.specific.created_mhl_filepath) as mhl:
            mhl_root = etree.parse(mhl)
        for rec in step.hashes:
            hashtype = rec['hashtype']
            expected_hash = rec['hash']
            file = rec['file']

            actual_hash_matches = mhl_root.xpath(u".//%(hashtype)s[../file[text() = \"%(file)s\"]]" %
            {
                "hashtype": hashtype,
                "file": file
            }
            )

            assert len(actual_hash_matches) > 0, u"No %s hash found for file '%s'" % (hashtype, file)
            assert len(actual_hash_matches) == 1, u"More than one %s hash found for file '%s'" % (hashtype, file)

            actual_hash = actual_hash_matches[0].text

            assert actual_hash == expected_hash, u"Wrong %s hash for file %s: Expected %s, got %s instead" % (hashtype, file, expected_hash, actual_hash)
    except:
        clean_everything()
        raise

@step(u"the MHL files contains the following file sizes for the paths:")
def match_MHL_hashes(step):
    try:
        with open(tv.specific.created_mhl_filepath) as mhl:
            mhl_root = etree.parse(mhl)
        for rec in step.hashes:
            expected_filesize = int(rec['filesize'])
            file = rec['file']

            actual_filesize_matches = mhl_root.xpath(u".//size[../file[text() = \"%(file)s\"]]" %
                                                     {
                                                        "file": file
                                                     }
            )

            assert len(actual_filesize_matches) > 0, u"No file size found for file '%s'" % (file,)
            actual_filesizes = set([int(actual_filesize_match.text) for actual_filesize_match in actual_filesize_matches])
            assert len(actual_filesizes) == 1, u"Different file sizes found for file '%s': %s" % (file, str.join(",", actual_filesizes))

            actual_filesize = next(iter(actual_filesizes)) # get the first (and only) entry

            assert actual_filesize == expected_filesize, u"Wrong file size for file %s: Expected %d, got %d instead" % (file, expected_filesize, actual_filesize)
    except:
        clean_everything()
        raise

def test_process_location(dst_dir):
    dst_dir = configure_dir(dst_dir)

    tv.specific.dst_dirs.add(dst_dir)

    return dst_dir
