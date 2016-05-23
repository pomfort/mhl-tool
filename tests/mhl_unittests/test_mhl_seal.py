from __future__ import print_function
import unittest
import os
from lxml import etree

__package__ = "mhl_unittests"

from .tools import mhl
from .tools.testdirs import TestDir


class TestMHLSeal(unittest.TestCase):
    def setUp(self):
        self._testDirs = []

    def tearDown(self):
        for testDir in self._testDirs:
            testDir.delete()

    def _test_mhl_seal(self, hashtype, file_hashes):
        testDir = TestDir("test_mhl_seal_%s" % hashtype)
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_seal"])

        mhl.mhl_seal.seal(folder=testDir.abspath_for("mhl_seal"), output_folder=testDir.abspath, hashtype=hashtype)
        mhl_files = testDir.list(".", pattern="*.mhl")
        self.assertNotEquals(len(mhl_files), 0, msg="No mhl file created")
        self.assertEquals(len(mhl_files), 1, msg="Unexpected number of mhl files")

        mhl_file = mhl_files[0]

        expected_results = {"mhl_seal/%s" % file: hash for file, hash in file_hashes.iteritems()}
        self._assert_mhl_seal_hashes_match(mhl_file, hashtype, expected_results)
        self._assert_mhl_seal_file_sizes_match(mhl_file, testDir, expected_results.keys())

    def _assert_mhl_seal_hashes_match(self, mhl_file_path, hashtype, expected_file_hashes):
        mhl_file = mhl.MHLFile(mhl_file_path)
        for file, expected_hash in expected_file_hashes.iteritems():
            actual_hashes = mhl_file.hashes_for_file(file, hashtype=hashtype)

            self.assertGreater(len(actual_hashes), 0, msg="No %s hash found for file '%s'" % (hashtype, file))
            self.assertEquals(len(actual_hashes), 1, msg="More than one %s hash found for file '%s'" % (hashtype, file))

            actual_hash = actual_hashes[0]

            self.assertEquals(actual_hash, expected_hash, msg="Wrong %s hash for file %s: Expected %s, got %s instead" % (hashtype, file, expected_hash, actual_hash))

    def _assert_mhl_seal_file_sizes_match(self, mhl_file_path, testDir, expected_files):
        mhl_file = mhl.MHLFile(mhl_file_path)
        for file in expected_files:
            actual_sizes = set(mhl_file.sizes_for_file(file))

            self.assertGreater(len(actual_sizes), 0, msg="No size found for file '%s'" % file)
            self.assertEquals(len(actual_sizes), 1, msg="More than one size found for file '%s'" % (file))

            actual_size = next(iter(actual_sizes))
            expected_size = os.stat(testDir.abspath_for(file)).st_size
            self.assertEquals(actual_size, expected_size, msg="File size in MHL file is wrong")

# autogenerate test functions for each hash function
expected_file_hashes = {
    "file0.txt": {
        "md5": "91cae4eeb20d47ca70abbbf058a74e80",
        "sha1": "64b18391ceb254455bc3dfe1200cbfa134fa1a3f",
        "xxhash": "3204945916",
        "xxhash64": "e338d99e661114f6",
        "xxhash64be": "f61411669ed938e3",
    },
    "file1.txt": {
        "md5": "9a83a5d3846361c721515b94189e70a0",
        "sha1": "55f554955b29f9cd118ef253e8bab03169e063aa",
        "xxhash": "2253396016",
        "xxhash64": "c531ae724519225f",
        "xxhash64be": "5f22194572ae31c5",
    },
    "file2.txt": {
        "md5": "ff178d1f75054ca4d9be03662d0bfdff",
        "sha1": "1b210443622c8a3db64c1a1180a59ed6a001f2ce",
        "xxhash": "1846681932",
        "xxhash64": "6de51e667bee045a",
        "xxhash64be": "5a04ee7b661ee56d",
    }
}
for hashtype in ("md5", "sha1", "xxhash", "xxhash64", "xxhash64be"):
    expected_results = {file: hashes[hashtype] for file, hashes in expected_file_hashes.iteritems()}
    setattr(TestMHLSeal, "test_mhl_seal_%s" % hashtype, lambda self, hashtype=hashtype, expected_results=expected_results: self._test_mhl_seal(hashtype, expected_results))

del hashtype
del expected_file_hashes
