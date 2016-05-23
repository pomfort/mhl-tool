Feature: Check tools work with big files

    Scenario: Absolute test for file sizes 2Gb +2b, 4Gb +2b, 8Gb +2b
        Given I have the tool mhl
        When I create a "test.avi" file in "test_dir" of <size> bytes starting from 'b', filled with 'a', and ending with 'c'
        And I run 'mhl seal' from "test_dir" with 'test.avi'
        Then the '.mhl' file is created in "test_dir"
        And the return code is 0
        And the MHL files contains the following XML tags for the paths:
          | file       | tag            | count  |
          | test.avi   | size           |       1|
          | test.avi   | md5            |       1|
        And I run 'mhl verify' from "test_dir" with '-vv test.avi'
        And the return code is 0.

    Examples:
        | size       | md5_hash                         | sha1_hash |
        | 2147483650 | b6b44ef5af047f6d767686194b1dfd74 | a4d342b8f8017f914a3f5062b4db5f4e2a59af71 |
        | 4294967298 | 564c99adeded958b28b201d745541afe | 725084039181007ac02d6784852bb1a41ac0ef73 |
        | 8589934594 | 3b1bfb8b60084dfbaa96d2c4691211a7 | a14a0337bee1b2a361e79e6396500a4cf8bc68c8 |


    Scenario: Relative test for big files
        Given I have the tool mhl
        When I create a "test.avi" file in "test_dir" of <size> bytes starting from <start_substr>, filled with <fill_substr>, and ending with <end_substr>
        And from 'test_dir' I run openssl to make md5 hashes for the given file and I run 'mhl hash' with 'test.avi'
        Then the outputs of 'mhl hash' and openssl are the same

    Examples:
        | size       | fill_substr | start_substr | end_substr |
        | 2147483650 | 'bcd'       | 'e'          | 'f'        |
        | 4294967298 | 'gh'        | 'i'          | 'j'        |
        | 8589934594 | 'a'         | 'b'          | 'c'        |
