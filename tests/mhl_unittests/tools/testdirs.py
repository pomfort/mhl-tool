import os
import shutil
import fnmatch

TESTDIR_BASE_PATH = os.path.normpath(os.path.join(
                                     os.path.dirname(__file__),
                                     os.path.pardir,
                                     os.path.pardir,
                                     "test_dir_unittest"
                                     ))

AUXFILES_BASE_PATH = os.path.normpath(os.path.join(
                                      os.path.dirname(__file__),
                                      os.path.pardir,
                                      os.path.pardir,
                                      "aux_files"
                                      ))


class TestDir(object):
    def __init__(self, path):
        self._path = path
        self._abspath = os.path.abspath(os.path.join(TESTDIR_BASE_PATH, path))

        os.makedirs(self._abspath)

    @property
    def path(self):
        return self._path

    @property
    def abspath(self):
        return self._abspath

    def abspath_for(self, subpath):
        return os.path.join(self._abspath, subpath)

    def list(self, path=".", pattern=None):
        abspath = os.path.join(self._abspath, path)
        content = os.listdir(abspath)
        if pattern is not None:
            content = fnmatch.filter(content, pattern)
        return [os.path.join(abspath, entry) for entry in content]

    def list_not(self, pattern, path="."):
        abspath = os.path.join(self._abspath, path)
        content = os.listdir(abspath)
        content = [entry for entry in content if not fnmatch.fnmatch(entry, pattern)]
        return [os.path.join(abspath, entry) for entry in content]

    def delete(self):
        shutil.rmtree(self._abspath)

    def copy_from_aux_files(self, paths, renames={}):
        destpaths = [renames.get(path, path) for path in paths]
        abspaths = [os.path.abspath(os.path.join(AUXFILES_BASE_PATH, path)) for path in paths]

        for abspath, destpath in zip(abspaths, destpaths):
            absdestpath = os.path.join(self._abspath, destpath)
            if os.path.isdir(abspath):
                shutil.copytree(abspath, absdestpath)
            else:
                shutil.copyfile(abspath, absdestpath)
