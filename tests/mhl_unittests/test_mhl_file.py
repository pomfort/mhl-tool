from __future__ import print_function
import unittest

__package__ = "mhl_unittests"

from .tools import mhl
from .tools.testdirs import TestDir

class TestMHLFile(unittest.TestCase):

    def setUp(self):
        self._testDirs = []

    def tearDown(self):
        for testDir in self._testDirs:
            testDir.delete()

    def test_mhl_file_all_hashes(self):
        testDir = TestDir("test_mhl_file_all_hashes")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_file_generic"])

        # convert the hash list to MHL
        hashspecs_path = testDir.abspath_for("mhl_file_generic/hashes.txt")
        mhl.mhl_file.convert_hashspecs_to_mhl(hashspecs_path=hashspecs_path,
                                              output_folder=testDir.abspath_for("mhl_file_generic"),
                                              cwd=testDir.abspath_for("mhl_file_generic"))
        mhl_file_paths = testDir.list("mhl_file_generic", "*.mhl")
        self.assertGreater(len(mhl_file_paths), 0, msg="No MHL file found")
        self.assertEquals(len(mhl_file_paths), 1, msg="More than one MHL file found")
        mhl_file_path = mhl_file_paths[0]
        # check the hashes in the mhl file by comparing them to the hashes in hashes.txt
        hashspecs = mhl.MHLHashSpecList.fromfile(hashspecs_path)
        mhl_file = mhl.MHLFile(mhl_file_path)
        for file in hashspecs.files():
            entries_for_file = hashspecs.entries_for_file(file)
            for entry in entries_for_file:
                mhl_hashes = mhl_file.hashes_for_file(file, hashtype=entry.hashtype.lower())
                self.assertEqual(len(set(mhl_hashes)), 1, msg="Different hashes for %s found in MHL file: %r" % (file, mhl_hashes))
                self.assertEqual(mhl_hashes[0], entry.hash, msg="Hashes in MHL file and hash spec are different for %s" % file)

        # check if we do not have any files in the MHL file that does not exist in hashes.txt
        hashspec_files = set(hashspecs.files())
        for file in mhl_file.files():
            self.assertIn(file, hashspec_files, msg="MHL file has a file that does not appear in the hash spec list: %s" % file)
