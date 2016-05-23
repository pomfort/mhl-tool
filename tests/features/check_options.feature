Feature: Check MHL tools reaction to different kinds of input

    Scenario Outline: No options in a given command
        Given I have the tool mhl
        When I run it without options
        Then the return code is 2.


    Scenario Outline: mhlfile: correct options and incorrect input
        Given the file is "hash-list.txt"
        When I duplicate the given file into "test_dir2" directory
        And I run 'mhl file' with '-s' from test_dir2 and provide it '<input>'
        Then the return code is <retcode>.

    Examples:
        | input                                                | retcode |
        | MD5(hash-litt.txt)= d29593e2cf81c621fb3c4089f9bbde4a | 3       |
        | MD5(hash-list.txt)= d29593e2cf81c621fb3c4089f9       | 6       |


    Scenario Outline: mhlhash: correct options in different order
        Given I have the tool mhl
        And the files are:
        | filename       |
        | hash-list.txt  |
        | hash-list1.txt |
        | hash-list2.txt |
        And the MHL file is "aux_files_2012-12-01_183454.mhl"
        When I duplicate the given files into "test_dir2"
        And I duplicate the given MHL file into "test_dir2" directory
        And I run '<command>' from "test_dir2" with '<options>'
        Then the return code is 0.

    Examples:
        | command    | options                                                   |
        | mhl        | --version                                                 |
        | mhl verify | -v -y -f aux_files_2012-12-01_183454.mhl hash-list.txt    |
        | mhl verify | -v -e -f aux_files_2012-12-01_183454.mhl hash-list.txt    |
        | mhl --version verify | -e -f aux_files_2012-12-01_183454.mhl                     |
        | mhl verify | -vv -f aux_files_2012-12-01_183454.mhl hash-list.txt hash-list1.txt |
        | mhl verify | -y -f aux_files_2012-12-01_183454.mhl -v -e hash-list.txt |
        | mhl verify | -vv -y -f aux_files_2012-12-01_183454.mhl                 |
        | mhl verify | -f aux_files_2012-12-01_183454.mhl -vv hash-list.txt      |
        | mhl verify | -e hash-list.txt                                          |
        | mhl hash   | md5 hash-list.txt                                      |
        | mhl hash   | -t md5 sha1 hash-list.txt                              |
        | mhl hash   | -v -t sha1 hash-list.txt                               |
        | mhl hash   | -t md5 -vv hash-list.txt                               |
        | mhl --version hash   | sha1 hash-list.txt                                     |
        | mhl hash   | -m -y sha1 hash-list.txt                               |
        | mhl hash   | -v -m hash-list1.txt hash-list2.txt                    |
        | mhl hash   | -vv -f hash-list.txt -h d29593e2cf81c621fb3c4089f9bbde4a |
        | mhl hash   | -vv -y -# hash-list#1-2.txt hash-list.txt              |

    Scenario Outline: mhlfile: input from file
        Given I have the tool mhl
        And the files are:
        | filename       |
        | hash-list.txt  |
        | hash-list1.txt |
        | openssl_output |
        When I duplicate the given files into "test_dir/test_dir2"
        And I run 'mhl file' from "<work_dir>" with '<options>'
        Then the '.mhl' file is created in "<mhl_dir>"
        And the return code is 0.

    Examples:
        | work_dir | options                                     | mhl_dir  |
        | test_dir |-f test_dir2/openssl_output                  | test_dir |
        | test_dir |-v -y -f test_dir2/openssl_output            | test_dir |
        | test_dir |-f test_dir2/openssl_output -vv              | test_dir |
        | test_dir |-vv -o test_dir2 -f test_dir2/openssl_output | test_dir/test_dir2 |

    Scenario Outline: mhlfile: correct options in different order
        Given I have the tool mhl
        And the files are:
        | filename       |
        | hash-list.txt  |
        | hash-list1.txt |
        When I duplicate the given files into "test_dir/test_dir2"
        And I run 'mhl file' with '<options>' from <work_dir> and provide it '<input>'
        Then the '.mhl' file is created in "<mhl_dir>"
        And the return code is 0.

    Examples:
        | work_dir | options     | input | mhl_dir |
        | test_dir |-s           | MD5(test_dir2/hash-list.txt)= d29593e2cf81c621fb3c4089f9bbde4a | test_dir |
        | test_dir |-v -y -s     | MD5(test_dir2/hash-list.txt)= d29593e2cf81c621fb3c4089f9bbde4a | test_dir |
        | test_dir |-v -s        | MD5(test_dir2/hash-list.txt)= d29593e2cf81c621fb3c4089f9bbde4a | test_dir |
        | test_dir/test_dir2 |-s -vv -o .. | MD5(hash-list.txt)= d29593e2cf81c621fb3c4089f9bbde4a | test_dir |

    Scenario Outline: mhlfile: multiple '--output-folder' specification
        Given I have the files
        | orig_number | filename       |
        | 1           | hash-list1.txt |
        | 2           | hash-list2.txt |
        | 3           | hash-list3.txt |
        | 4           | hash-list4.txt |
        When I put the given files into special directories
        | filenumber | orig_number | directory_name |
        | 1          | 1           | test_dir/test_dir_1/test_dir_1_1 |
        | 2          | 2           | test_dir/test_dir_1/test_dir_1_2 |
        | 3          | 3           | test_dir/test_dir_1/test_dir_1_2 |
        | 4          | 4           | test_dir/test_dir_1 |
        | 5          | 1           | test_dir/test_dir_2 |
        And for files [1, 2, 3, 4, 5] I run openssl from "test_dir" to make md5 hash and redirect output to 'mhl file -s --output-folder "test_dir_1" --output-folder "test_dir_1/test_dir_1_2" --output-folder "."' as input
        And the return code is 0
        And the '.mhl' file is created in "<mhl_dir>"
        Then I run 'mhl verify' from "<mhlverify_work_dir>" with '<options>'
        And the return code is 0.

    Examples:
        | mhl_dir                          | mhlverify_work_dir               | options |
        | test_dir/test_dir_1/test_dir_1_2 | test_dir/test_dir_1/test_dir_1_2 | -v -f *.mhl *.txt |
        | test_dir/test_dir_1              | test_dir/test_dir_1              | -v -f *.mhl *.txt test_dir_1_1/*.txt test_dir_1_2/*.txt |
        | test_dir                         | test_dir                         | -v -f *.mhl test_dir_1/*.txt test_dir_2/*.txt test_dir_1/test_dir_1_1/*.txt test_dir_1/test_dir_1_2/*.txt |

    Scenario Outline: mhlhash: recursive directory calculate
        Given I have the files
        | orig_number | filename       |
        | 1           | hash-list1.txt |
        | 2           | hash-list2.txt |
        | 3           | hash-list3.txt |
        | 4           | hash-list4.txt |
        When I put the given files into special directories
        | filenumber | orig_number | directory_name |
        | 1          | 1           | test_dir/test_dir_1/test_dir_1_1 |
        | 2          | 2           | test_dir/test_dir_1/test_dir_1_2 |
        | 3          | 3           | test_dir/test_dir_1/test_dir_1_2 |
        | 4          | 4           | test_dir/test_dir_1 |
        | 5          | 1           | test_dir/test_dir_2 |
        And from 'test_dir' I run openssl to make md5 hashes for files in specified order and I run 'mhl hash' with '.'
        | OS_type         | files_order   |
        | Windows, Darwin | 4, 1, 2, 3, 5 |
        | Linux           | 1, 3, 2, 4, 5 |
        Then the outputs of 'mhl hash' and openssl are the same

    Scenario Outline: mhlseal and mhlverify: recursive directory and multiple '-o' testing
        Given I have the files
        | orig_number | filename       |
        | 1           | hash-list1.txt |
        | 2           | hash-list2.txt |
        | 3           | hash-list3.txt |
        | 4           | hash-list4.txt |
        When I put the given files into special directories
        | filenumber | orig_number | directory_name |
        | 1          | 1           | test_dir/test_dir_1/test_dir_1_1 |
        | 2          | 2           | test_dir/test_dir_1/test_dir_1_2 |
        | 3          | 3           | test_dir/test_dir_1/test_dir_1_2 |
        | 4          | 4           | test_dir/test_dir_1 |
        | 5          | 1           | test_dir/test_dir_2 |
        And I run 'mhl seal' from "<mhlseal_wd>" with '<args1>'
        And the return code is 0
        And the '.mhl' file is created in "<mhl_dir>"
        Then I run 'mhl verify' from "<mhlverify_work_dir>" with '<args2>'
        And the return code is 0.

    Examples:
        | mhlseal_wd | args1                                                 | mhl_dir                          | mhlverify_work_dir               | args2 |
        | test_dir   | -o "test_dir_1" -o "test_dir_1/test_dir_1_2" -o "." * | test_dir/test_dir_1/test_dir_1_2 | test_dir/test_dir_1/test_dir_1_2 | -v -f *.mhl *.txt |
        | test_dir   | -o "test_dir_1" -o "test_dir_1/test_dir_1_2" -o "." * | test_dir/test_dir_1              | test_dir/test_dir_1              | -v -f *.mhl *.txt test_dir_1_1/*.txt test_dir_1_2/*.txt |
        | test_dir   | -o "test_dir_1" -o "test_dir_1/test_dir_1_2" -o "." * | test_dir                         | test_dir                         | -v -f *.mhl test_dir_1/*.txt test_dir_2/*.txt test_dir_1/test_dir_1_1/*.txt test_dir_1/test_dir_1_2/*.txt |
        | test_dir   | -o "." "test_dir_1"                                   | test_dir                         | test_dir                         | -v test_dir_1 |
        | test_dir   | *                                                     | test_dir                         | test_dir                         | -v -f *.mhl test_dir_2 |
        | test_dir   | -o test_dir_1 test_dir_1/*                            | test_dir/test_dir_1              | test_dir/test_dir_1              | -v -f *.mhl test_dir_1_1 test_dir_1_2 hash-list4.txt |


    Scenario Outline:  Incorrect options
        Given the file is "hash-list.txt"
        When I duplicate the given file into "test_dir" directory
        And I run '<command>' from "test_dir" with '<options>'
        Then the return code is <retcode>.

    Examples:
        | command    | options         | retcode |
        | mhl verify | -f -v tmp.mhl   | 2       |
        | mhl hash   | tmp.txt         | 3       |
        | mhl hash   | -h tmp.txt      | 2       |
        | mhl verify | -f -e tmp.mhl   | 2       |
        | mhl hash   | -vv -h d29593e2cf81c621fb3c4089f9bbde4b -f hash-list.txt | 16 |
        | mhl file   | -vt             | 2       |
        | mhl file   | -v -f           | 2       |
