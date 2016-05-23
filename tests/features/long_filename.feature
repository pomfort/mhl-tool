Feature: Check the resistance of MHL tools towards long filenames

    Scenario Outline: Long file name, directory name, MHL file name
        When I create in "test_dir" a "testname('b')" directory with the name from 'b' symbol repeated the needed number of times to get maximal filename length
        And I create in "test_dir/testname('b')" a "testname('a')" file with the name from 'a' symbol repeated the needed number of times to get maximal filename length
        And for files [<filenames>] I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        And the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        And I duplicate the created MHL file to "testname('c', '.mhl')" file with the name from 'c' symbols repeated the needed number of times to get maximal filename length with '.mhl' at the end
        Then I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | filenames      | work_d1 | mhl_dir | command | work_d2 | options |
        | testname('a') | test_dir/testname('b') | .. | mhl verify | test_dir/testname('b') | -v -f ../testname('c', '.mhl') |
        | testname('b')/testname('a') | test_dir | . | mhl verify | test_dir | -v -f testname('c', '.mhl') |
        | testname('a') | test_dir/testname('b') | .. | mhl verify | test_dir/testname('b') | -v -f ../testname('c', '.mhl') testname('a') |
        | testname('a') | test_dir/testname('b') | .. | mhl verify | test_dir | -v testname('b')/testname('a') |
#        | testname('a') | test_dir/testname('b') | .. | mhl sign | test_dir | testname('c', '.mhl') |
        | testname('a') | test_dir/testname('b') | .. | mhl hash | test_dir | -v -h d41d8cd98f00b204e9800998ecf8427e -f testname('b')/testname('a') |
        | testname('a') | test_dir/testname('b') | .. | mhl hash | test_dir/testname('b') | -v md5 testname('a') |
        | testname('a') | test_dir/testname('b') | .. | mhl hash | test_dir | -v sha1 testname('b')/testname('a') |
