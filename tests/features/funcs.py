from test_env import *
import os.path
import re
import string
import shutil
import subprocess
import sys
import logging
import pipes

def run_tool_with_opts_and_input_from_dir(tool, opts=None,
                                          work_dir=None, inp_str=None,
                                          abspath_in_opts=False,
                                          locale_charset=None):
    tool_path = os.path.abspath(os.path.join(utils_dir, tool))
    assert os.path.isfile(tool_path), "No such tool \"" + tool_path + "\"\n"

    cur_dir = os.getcwd()
    if not work_dir:
        work_dir = cur_dir
    assert os.path.isdir(work_dir), "No such directory \"" + work_dir + "\"\n"
    os.chdir(work_dir)

    if opts:
        # Don't move this outside, as this shall be performed after os.chdir(work_dir)
        if abspath_in_opts:
            (opts, n_of_subs) = rel_path_to_absolute_if_needed(opts, quote=True)

        cmd = pipes.quote(tool_path) + " " + opts
    else:
        cmd = pipes.quote(tool_path)

    try:
        if system_type == 'Windows':
            close_fds=False
        else:
            close_fds=True

        sys.stdout.flush()
        sys.stderr.flush()
        if inp_str:
            logging.info("Input string=\'" + inp_str + "\'\n")
            logging.info("Running the command \'" + cmd + "\' from %s\n",
                          work_dir)
            if locale_charset:
                cmd = cmd.encode(locale_charset)

            p = subprocess.Popen(cmd, stdin=subprocess.PIPE,
                                 stderr=subprocess.STDOUT,
                                 shell=True, close_fds=close_fds)

            p.stdin.write(inp_str)
            p.stdin.close()
        else:
            logging.info("Running the command \'" + cmd + "\' from %s\n",
                          work_dir)

            if locale_charset:
                cmd = cmd.encode(locale_charset)

            p = subprocess.Popen(cmd,
                                 stderr=subprocess.STDOUT,
                                 shell=True, close_fds=close_fds)

        retcode = p.wait()
        print "\n",
        sys.stdout.flush()
    finally:
        os.chdir(cur_dir)

    return retcode

def clean_test_dir(tst_dir=test_dir):
    if not os.path.isdir(tst_dir):
        return

    logging.debug("Cleaning the directory %s", tst_dir)
#    return

    for root, dirs, files in os.walk(tst_dir):
        for f in files:
            os.unlink(os.path.join(root,f))
        for d in dirs:
            shutil.rmtree(os.path.normpath(os.path.join(root,d)))
    logging.debug("The directory %s is cleaned\n", tst_dir)

def run_openssl_for_file(work_dir, hash_type, filename, locale_charset=None):
    assert os.path.isdir(work_dir), "No such directory \"" + work_dir + "\"\n"
    cur_dir = os.getcwd()
    os.chdir(work_dir)

    try:
        cmd = openssl_path + " " + hash_type + " \"" + filename + "\""

        logging.info("Running the command \'" + cmd + "\' from %s\n",
                      work_dir)

        if system_type == 'Windows':
            close_fds=False
        else:
            close_fds=True

        if locale_charset:
            cmd = cmd.encode(locale_charset)

        sys.stdout.flush()
        sys.stderr.flush()

        p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,
                             shell=True, close_fds=close_fds)
        retcode = p.wait()
        out_str = p.stdout.read()
        err_str = p.stderr.read()
        p.stdout.close()
        p.stderr.close()

        logging.debug("Openssl out_str:\n\"\n" + out_str + "\"\n")

        assert (retcode == 0 and len(err_str) == 0), "Openssl returned with " \
            "an error, the return code is %d, the error output is \"\n%s\"\n" \
            % (retcode, err_str)

    finally:
        os.chdir(cur_dir)

    return out_str

def run_mhlhash_and_return_output(work_dir, hash_opts, locale_charset=None):
    assert os.path.isdir(work_dir), "No such directory \"" + work_dir + "\"\n"

    tool_path = os.path.abspath(os.path.join(utils_dir, bin_name('mhl')))
    assert os.path.isfile(tool_path), "No such tool \"" + tool_path + "\"\n"

    cur_dir = os.getcwd()
    os.chdir(work_dir)

    try:
        cmd = pipes.quote(tool_path) + " hash " + hash_opts

        logging.info("Running the command \'" + cmd + "\' from %s\n",
                      work_dir)

        if system_type == 'Windows':
            close_fds=False
        else:
            close_fds=True

        if locale_charset:
            cmd = cmd.encode(locale_charset)

        sys.stdout.flush()
        sys.stderr.flush()

        p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,
                             shell=True, close_fds=close_fds)
        (out_str, err_str) = p.communicate()
        retcode = p.returncode

        logging.debug("'mhl hash' out_str:\n\"\n" + out_str + "\"\n")

        assert (retcode == 0 and len(err_str) == 0), "'mhl hash' returned with " \
            "an error, the return code is %d, the error output is \"\n%s\"\n" \
            % (retcode, err_str)

    finally:
        os.chdir(cur_dir)

    return out_str

def run_util_and_check_return_code(util, args_str,
                                   expected_retcode, work_dir=None):
    if work_dir:
        retcode = run_tool_with_opts_and_input_from_dir(
                    util, args_str, work_dir)
    else:
        util_path = os.path.join(utils_dir, util)
        retcode = pure_run_util(util_path, args_str)

    check_retcode(util, retcode, expected_retcode)

def pure_run_util(tool_path, args_str=None, locale_charset=None):
    if args_str:
        cmd = pipes.quote(tool_path) + " " + args_str
    else:
        cmd = pipes.quote(tool_path)

    logging.info("Running command '" + cmd + "':\n")

    if locale_charset:
        cmd = cmd.encode(locale_charset)

    sys.stdout.flush()
    sys.stderr.flush()

    retcode = subprocess.call(cmd, stderr=subprocess.STDOUT, shell=True)

    print "\n",
    sys.stdout.flush()
    return retcode

def pure_duplicate_file(filepath, dst, dst_type = None):
    if ((not dst_type) and os.path.isdir(dst)) or dst_type == 'directory':
        new_filepath = os.path.join(dst, os.path.basename(filepath))
    else:
        new_filepath = dst

    shutil.copy(filepath, dst)
    logging.debug("The file \"" + filepath + \
                  "\" is copied to \"" + dst + "\"\n")

    return new_filepath

def check_retcode(utilname, retcode, expected_retcode):
    assert retcode == expected_retcode, "\n Returned code check FAILED: " \
        "%s shall terminate with the %d error code, " \
        "but the return code is %d.\n" \
        % (utilname, expected_retcode, retcode)

def configure_dir(dst_dir):
    if not dst_dir:
        dst_dir = test_dir

    logging.debug("The destination directory is '" + dst_dir + "'.\n")
    if not os.path.isdir(dst_dir):
        os.makedirs(dst_dir)
        logging.debug("The directory '" + dst_dir + "' is created.\n")

    return dst_dir

def rel_path_to_absolute_if_needed(path, quote=False):
    return re.subn('absp\((.[^\)]+)\)', lambda match: my_abspath(match, quoted=quote), path)

def my_abspath(matchobj, quoted=False):
    rel_path = matchobj.group(1)

    logging.debug("Find absolute path for relative path '%s'", rel_path)
    abs_path = os.path.abspath(rel_path)
    logging.debug("Done. The absolute path is '%s'", abs_path)
    if quoted:
        return quote_path_but_keep_globs(abs_path)
    else:
        return abs_path

def quote_path_but_keep_globs(path):
    components = re.split(r"(\*|\?)", path)
    quoted_components = []
    for component in components:
        if component == "*" or component == "?":
            # do not quote wildcard characters
            quoted_component = component
        else:
            quoted_component = pipes.quote(component)
        quoted_components.append(quoted_component)
    quoted_path = str.join("", quoted_components)
    print quoted_path
    return quoted_path

def create_long_name_if_needed(target_name):
    return re.sub('testname\((.[^\)]+)\)', create_long_name, target_name)

def create_long_name(matchobj):
    longname_rule = matchobj.group(1)

    if not longname_len:
        define_longname_len()

    symbols_matchobj = re.match("\'([^\']+)\'(?:.*\'([^\']+)\')?", longname_rule)

    logging.debug('longname_rule = "' + longname_rule + '"')

    if not symbols_matchobj:
        assert False, "Internal error!"

    longname = ""

    logging.debug('symbols_matchobj.lastindex = %d', symbols_matchobj.lastindex)
    logging.debug('symbols_matchobj.group(0) = "%s"', symbols_matchobj.group(0))
    logging.debug('symbols_matchobj.group(1) = "%s"', symbols_matchobj.group(1))
    logging.debug('symbols_matchobj.group(2) = "%s"', symbols_matchobj.group(2))

    if symbols_matchobj.lastindex == 2:
        end_len = len(symbols_matchobj.group(2))
        longname = symbols_matchobj.group(1) * (longname_len - end_len)
        longname += symbols_matchobj.group(2)
    else:
        longname = symbols_matchobj.group(1) * longname_len

    return longname

def define_longname_len():
    global longname_len
    if system_type == 'Windows':
        return longname_len

    test_file = os.path.join(test_dir, "foo.txt")

    fd = os.open(test_file, os.O_RDWR | os.O_CREAT)
    try:
        info = os.fstatvfs(fd)
        longname_len = int(info.f_namemax)

        logging.debug("The length of maximal filename is determined as %d" % \
                      longname_len)

    finally:
        os.close(fd)

def get_testname_if_needed(testnames_str, pattern, nameslist):
    testname_placeholder = re.compile(pattern)
    for matchobj in testname_placeholder.finditer(testnames_str):
        logging.debug("matchobj.group(0) = '" + matchobj.group(0) + "'")
        logging.debug("matchobj.group(1) = '" + matchobj.group(1) + "'")
        name_handle = matchobj.group(1)
        assert name_handle, "Internal error: no group in pattern '" + pattern + "'\n"

        name = nameslist.get(name_handle)
        assert name, "There is no an item associated with " + \
                     "the record " + name_handle + " in the list.\n"

        testnames_str = re.sub(re.escape(matchobj.group(0)), name, testnames_str)
        logging.debug("testnames_str = '" + testnames_str + "'")

    return testnames_str

def bin_name(tool):
   if system_type == 'Windows':
       tool += '.exe'
   return tool

def parse_hash_spec(hash_spec):
    # hash specs have the form "hash1, hash2 and hash3"
    # note that "hash1" and "hash1, hash2" and "hash1 and hash2" should also be valid
    split_at_and = hash_spec.split("and")
    if len(split_at_and) == 2:
        rest, last_part = split_at_and
    else:
        rest = split_at_and[0]
        last_part = ""

    splitted_rest = rest.split(",")

    hashes = splitted_rest + [last_part]
    hashes = [h.strip() for h in hashes]
    hashes = [h for h in hashes if len(h) > 0]

    return hashes
