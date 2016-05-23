Feature: Check mhl file work

    Scenario: Test for mhl file
        Given I have the tool mhl
        And the files are:
        | filename                 |
        | hash-list.txt            |
        | hash-list1.txt           |
        | hash-list-small.txt      |
        | openssl_mixed_hash_types |
        When I duplicate the given files into "test_dir/test_dir2"
        And I run 'mhl file' from "test_dir" with '-f test_dir2/openssl_mixed_hash_types -o .'
        Then the '.mhl' file is created in "test_dir"
        And the MHL files contains the following hashes for the paths:
          | file                          | hashtype | hash                                      |
          | test_dir2/hash-list.txt       | md5      | d29593e2cf81c621fb3c4089f9bbde4a          |
          | test_dir2/hash-list-small.txt | sha1     | d652f950ec410728593d2594e46321854fdcc831  |
          | test_dir2/hash-list-small.txt | md5      | 62b43e0a61871e2e0559ddf3fc5c2922          |
          | test_dir2/hash-list1.txt      | sha1     | 1cf879ba7d01a314d5cb737790873a7457f436fd  |
        And the MHL files contains the following file sizes for the paths:
          | file                          | filesize |
          | test_dir2/hash-list.txt       | 277      |
          | test_dir2/hash-list-small.txt | 128      |
          | test_dir2/hash-list1.txt      | 277      |
        And the return code is 0.


    Scenario: Test for mhl seal
        Given I have the tool mhl
        And the files are:
          | filename                 |
          | hash-list.txt            |
          | hash-list1.txt           |
          | hash-list-small.txt      |
        When I duplicate the given files into "test_dir/test_dir2"
        And I run 'mhl seal' from "test_dir" with 'test_dir2'
        Then the '.mhl' file is created in "test_dir"
        And the MHL files contains the following hashes for the paths:
          | file                          | hashtype | hash                                      |
          | test_dir2/hash-list.txt       | md5      | d29593e2cf81c621fb3c4089f9bbde4a          |
          | test_dir2/hash-list-small.txt | md5      | 62b43e0a61871e2e0559ddf3fc5c2922          |
          | test_dir2/hash-list1.txt      | md5      | d29593e2cf81c621fb3c4089f9bbde4a          |
        And the MHL files contains the following file sizes for the paths:
          | file                          | filesize |
          | test_dir2/hash-list.txt       | 277      |
          | test_dir2/hash-list-small.txt | 128      |
          | test_dir2/hash-list1.txt      | 277      |
        And the return code is 0.
