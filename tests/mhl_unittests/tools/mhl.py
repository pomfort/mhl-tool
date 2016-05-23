import os.path
import sys
import subprocess
from collections import namedtuple
import re
import exceptions

from lxml import etree


def assertEquals(expected, actual, what=None, msg=None):
    if msg is None and what is None:
        msg = "Expected %r, got %r instead" % (expected, actual)
    elif msg is None:
        msg = "Expected %r for %s, got %r instead" % (expected, what, actual)
    assert expected == actual, msg


def get_mhl_path():
    bin_platform_paths = {
        "darwin": "MacOSX/Release"
    }

    base_bin_path = os.path.join(os.path.dirname(__file__), "../../../bin")
    mhl_path = os.path.join(
        base_bin_path,
        bin_platform_paths[sys.platform],
        "mhl"
    )

    if not os.path.exists(mhl_path) or not os.path.isfile(mhl_path):
        raise ValueError("mhl tool not found in path '%s'", mhl_path)

    return mhl_path


class MHLCommandException(exceptions.Exception):
    def __init__(self, cause):
        super(MHLCommandException, self).__init__()
        self.cause = cause
        self.message = "MHL Tool returned non-zero exit code %d: %s" % (cause.returncode, cause.output)

    @property
    def returncode(self):
        return self.cause.returncode

    @property
    def output(self):
        return self.cause.output

    def __repr__(self):
        return "<%s: %s>" % (self.__class__.__name__, self.message)

    def __str__(self):
        return "%s" % (self.message)


def run_mhl(args, cwd=None):
    call_args = [get_mhl_path()] + args
    try:
        return subprocess.check_output(call_args, stderr=subprocess.STDOUT, cwd=cwd)
    except subprocess.CalledProcessError as e:
        raise MHLCommandException(e)


MHLHashSpec = namedtuple("HashFileResult", ("hashtype", "filepath", "hash"))


class MHLHashSpecList(object):
    def __init__(self, hashpecs_string):
        self._entries = self._parse_hash_specs(hashpecs_string)

    def _parse_hash_specs(self, hashpecs):
        result_pattern = re.compile(r"^(?P<hashtype>\w+)\((?P<filepath>.*)\)\s*=\s*(?P<hash>[a-fA-F0-9]+)$", re.MULTILINE)
        return [MHLHashSpec(hashtype=match.group("hashtype"),
                                  filepath=match.group("filepath"),
                                  hash=match.group("hash"))
                for match in result_pattern.finditer(hashpecs)]

    def entries_for_file(self, file):
        return [entry for entry in self._entries if entry.filepath == file]

    def hashes_for_file(self, file, hashtype):
        return [entry for entry in self._entries if entry.filepath == file and entry.hashtype == hashtype]

    def files(self):
        return list(set(entry.filepath for entry in self._entries))

    @property
    def entries(self):
        return self._entries

    @staticmethod
    def fromstring(hashpecs_string):
        return MHLHashSpecList(hashpecs_string)

    @staticmethod
    def fromfile(hashpecs_path):
        with open(hashpecs_path) as f:
            data = f.read()
            return MHLHashSpecList(data)


class mhl_hash(object):
    @staticmethod
    def hashes_for_files(files, hashtype=None, args=None, cwd=None):
        args = args if args is not None else []
        if hashtype is not None:
            args += ["-t", hashtype]
        args += files

        output = run_mhl(["hash"] + args, cwd=cwd)

        return MHLHashSpecList.fromstring(output).entries

    @staticmethod
    def verify_file(file, hash, hashtype=None, cwd=None):
        if hashtype is None:
            args = ["-f", file, "-h", hash]
        else:
            args = ["-f", file, "-h", hashtype, hash]

        try:
            run_mhl(["hash"] + args, cwd=cwd)
            return True
        except MHLCommandException as e:
            if e.returncode == 16:
                return False
            else:
                raise e


class mhl_verify(object):
    @staticmethod
    def verify(*args, **kwargs):
        try:
            mhl_verify._exec(*args, **kwargs)
            return True
        except MHLCommandException as e:
            # 15: file size not equal, 16: file hash does not match, 3: file does not exist
            if e.returncode in [15, 16, 3]:
                return False
            else:
                raise e

    @staticmethod
    def verify_with_machine_readable_output(*args, **kwargs):
        kwargs["machinereadable"] = True

        try:
            output = mhl_verify._exec(*args, **kwargs)
            return (0, output)
        except MHLCommandException as e:
            return (e.returncode, e.output)

    @staticmethod
    def _exec(mhl_file, args=None, only_verify_existence=False, machinereadable=False, continue_on_error=False, cwd=None):
        args = args if args is not None else []
        if only_verify_existence:
            args += ["-e"]
        if machinereadable:
            args += ["-y"]
        if continue_on_error:
            args += ["-c"]
        args += ["-f", mhl_file]

        return run_mhl(["verify"] + args, cwd=cwd)


class mhl_seal(object):
    @staticmethod
    def seal(folder, hashtype=None, output_folder=None):
        args = []
        if output_folder is not None:
            args += ["-o", output_folder]
        if hashtype is not None:
            args += ["-t", hashtype]
        args += [folder]

        run_mhl(["seal"] + args)


class mhl_file(object):
    @staticmethod
    def convert_hashspecs_to_mhl(hashspecs_path, output_folder=None, cwd=None):
        args = []
        if output_folder is not None:
            args += ["-o", output_folder]
        args += ["-f", hashspecs_path]

        run_mhl(["file"] + args, cwd=cwd)


class MHLFile(object):
    def __init__(self, mhl_file_path):
        self._mhl_file_path = mhl_file_path
        with open(mhl_file_path) as mhl:
            self._root = etree.parse(mhl)

    @property
    def mhl_file_path(self):
        return self._mhl_file_path

    def hashes_for_file(self, file, hashtype):
        xpath = u".//%(hashtype)s[../file[text() = \"%(file)s\"]]" % {
            "hashtype": hashtype,
            "file": file
        }
        hash_matches = self._root.xpath(xpath)

        return [hash_match.text for hash_match in hash_matches]

    def hash_for_file(self, file, hashtype):
        hashes = self.hashes_for_file(file, hashtype)
        if len(hashes) > 0:
            return hashes[-1]
        else:
            return None

    def sizes_for_file(self, file):
        xpath = u".//size[../file[text() = \"%(file)s\"]]" % {
            "file": file
        }
        size_matches = self._root.xpath(xpath)

        return [int(size_match.text) for size_match in size_matches]

    def size_for_file(self, file, hashtype):
        sizes = self.sizes_for_file(file, hashtype)
        if len(sizes) > 0:
            return sizes[-1]
        else:
            return None

    def files(self):
        xpath = u".//file"
        file_matches = self._root.xpath(xpath)

        return [file_match.text for file_match in file_matches]
