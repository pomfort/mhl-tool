from __future__ import print_function
import unittest
from collections import Counter

__package__ = "mhl_unittests"

from .tools import mhl
from .tools.testdirs import TestDir


class TestMHLHash(unittest.TestCase):

    def setUp(self):
        self._testDirs = []

    def tearDown(self):
        for testDir in self._testDirs:
            testDir.delete()

    def test_mhl_hash_md5(self):
        testDir = TestDir("test_mhl_hash_md5")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes("MD5", {
            "file0.txt": "91cae4eeb20d47ca70abbbf058a74e80",
            "file1.txt": "9a83a5d3846361c721515b94189e70a0",
            "file2.txt": "ff178d1f75054ca4d9be03662d0bfdff",
        }, cwd=testDir.abspath)

    def test_mhl_hash_md5_verify(self):
        testDir = TestDir("test_mhl_hash_md5_verify")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes_verify("MD5", {
            "file0.txt": "91cae4eeb20d47ca70abbbf058a74e80",
            "file1.txt": "9a83a5d3846361c721515b94189e70a0",
            "file2.txt": "ff178d1f75054ca4d9be03662d0bfdff",
        }, cwd=testDir.abspath)
        self._assert_mhl_hashes_verify_fail("MD5", {
            "file0.txt": "01234567890abcdef01234567890abcd",
            "file1.txt": "11111111111111111111111111111111",
            "file2.txt": "ff178d1f75054ca4d9be03662d0bfdfa",
        }, cwd=testDir.abspath)

    def test_mhl_hash_sha1(self):
        testDir = TestDir("test_mhl_hash_sha1")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes("SHA1", {
            "file0.txt": "64b18391ceb254455bc3dfe1200cbfa134fa1a3f",
            "file1.txt": "55f554955b29f9cd118ef253e8bab03169e063aa",
            "file2.txt": "1b210443622c8a3db64c1a1180a59ed6a001f2ce",
        }, cwd=testDir.abspath)

    def test_mhl_hash_sha1_verify(self):
        testDir = TestDir("test_mhl_hash_sha1_verify")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes_verify("SHA1", {
            "file0.txt": "64b18391ceb254455bc3dfe1200cbfa134fa1a3f",
            "file1.txt": "55f554955b29f9cd118ef253e8bab03169e063aa",
            "file2.txt": "1b210443622c8a3db64c1a1180a59ed6a001f2ce",
        }, cwd=testDir.abspath)
        self._assert_mhl_hashes_verify_fail("SHA1", {
            "file0.txt": "01234567890abcdef01234567890abcdef012345",
            "file1.txt": "1111111111111111111111111111111111111111",
            "file2.txt": "1b210443622c8a3db64c1a1180a59ed6a001f2cd",
        }, cwd=testDir.abspath)

    def test_mhl_hash_xxhash(self):
        testDir = TestDir("test_mhl_hash_xxhash")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes("XXHash", {
            "file0.txt": "3204945916",
            "file1.txt": "2253396016",
            "file2.txt": "1846681932",
        }, cwd=testDir.abspath)

    def test_mhl_hash_xxhash_verify(self):
        testDir = TestDir("test_mhl_hash_xxhash_verify")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes_verify("XXHash", {
            "file0.txt": "3204945916",
            "file1.txt": "2253396016",
            "file2.txt": "1846681932",
        }, cwd=testDir.abspath)
        self._assert_mhl_hashes_verify_fail("XXHash", {
            "file0.txt": "0123456789",
            "file1.txt": "1111111111",
            "file2.txt": "1846681933",
        }, cwd=testDir.abspath)

    def test_mhl_hash_xxhash64(self):
        testDir = TestDir("test_mhl_hash_xxhash64")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes("XXHash64", {
            "file0.txt": "e338d99e661114f6",
            "file1.txt": "c531ae724519225f",
            "file2.txt": "6de51e667bee045a",
        }, cwd=testDir.abspath)

    def test_mhl_hash_xxhash64_verify(self):
        testDir = TestDir("test_mhl_hash_xxhash64_verify")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes_verify("XXHash64", {
            "file0.txt": "e338d99e661114f6",
            "file1.txt": "c531ae724519225f",
            "file2.txt": "6de51e667bee045a",
        }, cwd=testDir.abspath)
        self._assert_mhl_hashes_verify_fail("XXHash64", {
            "file0.txt": "0123456789abcdef",
            "file1.txt": "1111111111111111",
            "file2.txt": "6de51e667bee045b",
        }, cwd=testDir.abspath)

    def test_mhl_hash_xxhash64be(self):
        testDir = TestDir("test_mhl_hash_xxhash64be")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes("XXHash64BE", {
            "file0.txt": "f61411669ed938e3",
            "file1.txt": "5f22194572ae31c5",
            "file2.txt": "5a04ee7b661ee56d",
        }, cwd=testDir.abspath)

    def test_mhl_hash_xxhash64be_verify(self):
        testDir = TestDir("test_mhl_hash_xxhash64be_verify")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["file0.txt", "file1.txt", "file2.txt"])
        self._assert_mhl_hashes_verify("XXHash64BE", {
            "file0.txt": "f61411669ed938e3",
            "file1.txt": "5f22194572ae31c5",
            "file2.txt": "5a04ee7b661ee56d",
        }, cwd=testDir.abspath)
        self._assert_mhl_hashes_verify_fail("XXHash64BE", {
            "file0.txt": "0123456789abcdef",
            "file1.txt": "1111111111111111",
            "file2.txt": "5a04ee7b661ee56e",
        }, cwd=testDir.abspath)

    def _assert_mhl_hashes(self, hashtype, files_to_hash_with_hashes, cwd):
        mhl_hash_args = ["-t", hashtype.lower()]
        files_to_hash = files_to_hash_with_hashes.keys()
        expected_file_hashes = files_to_hash_with_hashes.values()

        expected_hash_results = [mhl.MHLHashSpec(hashtype=hashtype,
                                                       filepath=file_to_hash,
                                                       hash=expected_file_hash)
                                 for file_to_hash, expected_file_hash in zip(files_to_hash, expected_file_hashes)]

        mhl_hash_results = mhl.mhl_hash.hashes_for_files(args=mhl_hash_args,
                                                         files=files_to_hash,
                                                         cwd=cwd)

        # check that no file path appears multiple times
        filepath_counter = Counter([result.filepath for result in mhl_hash_results])
        self.assertEquals(1, max(filepath_counter.values()), msg="filepath appears multiple times")

        # now check if result values are equal to expected values
        mhl_hash_results_lookup = {result.filepath: result for result in mhl_hash_results}

        for expected_hash_result in expected_hash_results:
            hash_result = mhl_hash_results_lookup.get(expected_hash_result.filepath, None)

            self.assertIsNotNone(hash_result, msg="No hash result for '%s'" % expected_hash_result.filepath)
            self.assertEquals(expected_hash_result.hashtype,
                              hash_result.hashtype)
            self.assertEquals(expected_hash_result.hash,
                              hash_result.hash)

    def _assert_mhl_hashes_verify(self, hashtype, files_to_verify_with_hashes, cwd):
        for file, hash in files_to_verify_with_hashes.iteritems():
            self.assertTrue(mhl.mhl_hash.verify_file(file, hash, hashtype=hashtype.lower(), cwd=cwd),
                            msg="Failed to verify '%s' with %s hash %s" % (file, hashtype, hash))

    def _assert_mhl_hashes_verify_fail(self, hashtype, files_to_verify_with_hashes, cwd):
        for file, hash in files_to_verify_with_hashes.iteritems():
            self.assertFalse(mhl.mhl_hash.verify_file(file, hash, hashtype=hashtype.lower(), cwd=cwd),
                             msg="Verifying '%s' with %s hash %s worked, but we expected it to fail" % (file, hashtype, hash))

if __name__ == '__main__':
    unittest.main()
