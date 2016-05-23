from __future__ import print_function
import unittest
import os

__package__ = "mhl_unittests"

from .tools import mhl
from .tools.testdirs import TestDir


class TestMHLVerify(unittest.TestCase):
    def setUp(self):
        self._testDirs = []

    def tearDown(self):
        for testDir in self._testDirs:
            testDir.delete()

    def _test_mhl_verify(self, hashtype):
        testDir = TestDir("test_mhl_verify_%s" % hashtype)
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_verify_%s" % hashtype])
        self.assertTrue(mhl.mhl_verify.verify("%s.mhl" % hashtype, cwd=testDir.abspath_for("mhl_verify_%s" % hashtype)),
                        msg="Failed to verify with MHL file")

    def _test_mhl_verify_fail(self, hashtype):
        testDir = TestDir("test_mhl_verify_fail_%s" % hashtype)
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_verify_%s" % hashtype])

        # choose one of the files and break it
        files = testDir.list_not("*.mhl", path="mhl_verify_%s" % hashtype)
        file_to_break = files[-1]

        with open(file_to_break, "r") as f:
            data = f.read()
        data = "BROKEN" + data[len("BROKEN"):]
        with open(file_to_break, "w") as f:
            f.write(data)

        self.assertFalse(mhl.mhl_verify.verify("%s.mhl" % hashtype, cwd=testDir.abspath_for("mhl_verify_%s" % hashtype)),
                         msg="Verify with MHL file succeeded, although we expected it to fail")

    def test_mhl_verify_only_existence(self):
        testDir = TestDir("test_mhl_verify_only_existence")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_verify_generic"])

        self.assertTrue(mhl.mhl_verify.verify("generic.mhl", only_verify_existence=True, cwd=testDir.abspath_for("mhl_verify_generic")),
                        msg="Failed to verify with MHL file")

    def test_mhl_verify_missing_with_hash(self):
        testDir = TestDir("test_mhl_verify_missing_with_hash")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_verify_generic"])
        # choose one of the files and delete it
        files = testDir.list_not("*.mhl", path="mhl_verify_generic")
        file_to_delete = files[-1]
        os.unlink(file_to_delete)

        self.assertFalse(mhl.mhl_verify.verify("generic.mhl", cwd=testDir.abspath_for("mhl_verify_generic")),
                         msg="Verify with MHL file succeeded, although we expected it to fail")

    def test_mhl_verify_missing_with_existence(self):
        testDir = TestDir("test_mhl_verify_missing_with_existence")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_verify_generic"])
        # choose one of the files and delete it
        files = testDir.list_not("*.mhl", path="mhl_verify_generic")
        file_to_delete = files[-1]
        os.unlink(file_to_delete)

        self.assertFalse(mhl.mhl_verify.verify("generic.mhl", only_verify_existence=True, cwd=testDir.abspath_for("mhl_verify_generic")),
                         msg="Verify with MHL file succeeded, although we expected it to fail")

    def test_mhl_verify_machinereadable(self):
        testDir = TestDir("test_mhl_verify_machinereadable")
        self._testDirs += [testDir]

        testDir.copy_from_aux_files(["mhl_verify_machinereadable"])

        errorcode, output = mhl.mhl_verify.verify_with_machine_readable_output("generic.mhl", cwd=testDir.abspath_for("mhl_verify_machinereadable"))

        self.assertEquals(errorcode, 0, "mhl verify did not return 0 as exit code, instead returned %d" % errorcode)
        # TODO parse output and check if it matches the stuff in the MHL file



# autogenerate test functions for each hash function
for hashtype in ("md5", "sha1", "xxhash", "xxhash64", "xxhash64be", "multiple", "null"):
    setattr(TestMHLVerify, "test_mhl_verify_%s" % hashtype, lambda self, hashtype=hashtype: self._test_mhl_verify(hashtype))
for hashtype in ("md5", "sha1", "xxhash", "xxhash64", "xxhash64be", "multiple"):
    setattr(TestMHLVerify, "test_mhl_verify_fail_%s" % hashtype, lambda self, hashtype=hashtype: self._test_mhl_verify_fail(hashtype))

del hashtype
