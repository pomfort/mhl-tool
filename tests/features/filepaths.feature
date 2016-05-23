Feature: Check how MHL tools deal with arbitrary file path input

    Scenario Outline: mhl hash and file: Files and folders paths and asterisk
        Given I have the files
        | orig_number | filename       |
        | 1           | hash-list1.txt |
        | 2           | hash-list2.txt |
        | 3           | hash-list3.txt |
        | 4           | hash-list4.txt |
        When I put the given files into special directories
        | filenumber | orig_number | directory_name |
        | 1          | 1           | test_dir/test_dir_1/test_dir_1_1 |
        | 2          | 1           | test_dir/test_dir_1/test_dir_1_2 |
        | 3          | 2           | test_dir/test_dir_2/test_dir_2_1 |
        | 4          | 2           | test_dir/test_dir_2 |
        | 5          | 3           | test_dir/test_dir_1 |
        | 6          | 3           | test_dir/test_dir_2 |
        | 7          | 4           | test_dir/test_dir_1 |
        | 8          | 4           | test_dir/test_dir_2/test_dir_2_1 |
        And for files [<filenums>] I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        And the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        Then I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | filenums | work_d1 | mhl_dir | command | work_d2 | options |
        | 1, 2     | test_dir| test_dir_1 | mhl verify | test_dir/test_dir_1 | -v -f *.mhl |
        | 3, 4, 8  | test_dir| . | mhl verify | test_dir/test_dir_2 | -v -f ../*.mhl test_dir_2_1 |
        | 3, 4, 8  | test_dir| . | mhl verify | test_dir/test_dir_2 | -v test_dir_2_1/* hash-list2.txt |
        | 3, 4, 6, 7 | test_dir/test_dir_2/test_dir_2_1 | ../.. | mhl verify | test_dir/test_dir_1/test_dir_1_1 | -v ../hash-list4.txt ../../test_dir_2/test_dir_2_1/hash-list2.txt |
        | 4, 5, 6, 7 | test_dir/test_dir_1 | .. | mhl verify | test_dir/test_dir_2 | -v ../test_dir_1/*.txt *.txt |
        | 1, 3, 4, 5, 6, 7, 8 | test_dir/test_dir_1 | .. | mhl verify | test_dir/test_dir_2 | -v -f ../test_dir_1/./../*.mhl test_dir_2_1/../../test_dir_2/./test_dir_2_1 ../test_dir_1/*.txt |
        | 2        | test_dir| . | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f test_dir_1/test_dir_1_2/* |
        | 2        | test_dir| . | mhl hash | test_dir | -f test_dir_1/.././../test_dir/test_dir_1/../test_dir_1/test_dir_1_2/hash-list1.txt -h d29593e2cf81c621fb3c4089f9bbde4a |
        | 2        | test_dir| . | mhl hash | test_dir/test_dir_1/test_dir_1_2 | -h d29593e2cf81c621fb3c4089f9bbde4a -f ./hash-list1.txt |
        | 1, 3, 8  | test_dir| . | mhl hash | test_dir | test_dir_2/test_dir_2_1/* test_dir_1/./../test_dir_1/test_dir_1_1/hash-list1.txt |
        | 4, 6     | test_dir| . | mhl hash | test_dir | test_dir_2 |
        | 7        | test_dir| . | mhl hash | test_dir/test_dir_1/test_dir_1_1 | ../hash-list4.txt |

    Scenario Outline: mhl seal and verify: Files and folders paths and asterisk
        Given I have the files
        | orig_number | filename       |
        | 1           | hash-list1.txt |
        | 2           | hash-list2.txt |
        | 3           | hash-list3.txt |
        | 4           | hash-list4.txt |
        When I put the given files into special directories
        | filenumber | orig_number | directory_name |
        | 1          | 1           | test_dir/test_dir_1/test_dir_1_1 |
        | 2          | 1           | test_dir/test_dir_1/test_dir_1_2 |
        | 3          | 2           | test_dir/test_dir_2/test_dir_2_1 |
        | 4          | 2           | test_dir/test_dir_2 |
        | 5          | 3           | test_dir/test_dir_1 |
        | 6          | 3           | test_dir/test_dir_2 |
        | 7          | 4           | test_dir/test_dir_1 |
        | 8          | 4           | test_dir/test_dir_2/test_dir_2_1 |
        And I run 'mhl seal' from "<work_d1>" with '<args1>'
        And the return code is 0
        And the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        Then I run 'mhl verify' from "<work_d2>" with '<args2>'
        And the return code is 0.

    Examples:
        | work_d1  | args1 | mhl_dir | work_d2 | args2 |
        | test_dir | -o test_dir_1 test_dir_1/test_dir_1_2/* | test_dir_1 | test_dir/test_dir_1 | -v -f *.mhl |
        | test_dir | -o test_dir_1/.././../test_dir/test_dir_1/../test_dir_1 test_dir_1/.././../test_dir/test_dir_1/../test_dir_1/test_dir_1_2/hash-list1.txt | test_dir_1 | test_dir | -v test_dir_1/test_dir_1_2/hash-list1.txt |
        | test_dir/test_dir_2 | test_dir_2_1 hash-list2.txt | . | test_dir/test_dir_2 | -v test_dir_2_1/* hash-list2.txt |
        | test_dir/test_dir_1/test_dir_1_1 | -o ../.. ../../test_dir_2 ../hash-list4.txt | ../.. | test_dir/test_dir_1/test_dir_1_1 | -v ../hash-list4.txt ../../test_dir_2/test_dir_2_1/hash-list2.txt |
        | test_dir/test_dir_1 | -o .. ../test_dir_1/*.txt ../test_dir_2 | .. | test_dir/test_dir_2 | -v ../test_dir_1/*.txt *.txt |
        | test_dir | -o . test_dir_2/test_dir_2_1/../../test_dir_2/./test_dir_2_1/* | . | test_dir/test_dir_2 | -v -f ../test_dir_1/./../*.mhl test_dir_2_1/../../test_dir_2/./test_dir_2_1/* |


    Scenario Outline: mhl hash and file: Absolute filepaths
        Given I have the files
        | orig_number | filename       |
        | 1           | hash-list1.txt |
        | 2           | hash-list2.txt |
        | 3           | hash-list3.txt |
        | 4           | hash-list4.txt |
        When I put the given files into special directories with 'filepath_abs' as marker of absolute filepath usage in openssl command
        | filenumber | orig_number | directory_name | filepath_abs |
        | 1          | 1           | test_dir/test_dir_1/test_dir_1_1 | 0 |
        | 2          | 1           | test_dir/test_dir_1/test_dir_1_2 | 1 |
        | 3          | 2           | test_dir/test_dir_2/test_dir_2_1 | 1 |
        | 4          | 2           | test_dir/test_dir_2 | 1 |
        | 5          | 3           | test_dir/test_dir_1 | 0 |
        | 6          | 3           | test_dir/test_dir_2 | 1 |
        | 7          | 4           | test_dir/test_dir_1 | 1 |
        | 8          | 4           | test_dir/test_dir_2/test_dir_2_1 | 0 |
        And for files [<filenums>] I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        And the '.mhl' file is created in "<mhl_dir>"
        Then I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | filenums | work_d1 | mhl_dir | command | work_d2 | options |
        | 1, 2     | test_dir| absp(test_dir/test_dir_1) | mhl verify | test_dir/test_dir_1 | -v -f absp(*.mhl) |
        | 3, 4, 8  | test_dir| absp(test_dir) | mhl verify | test_dir/test_dir_2 | -v -f absp(../*.mhl) absp(test_dir_2_1/*) |
        | 3, 4, 8  | test_dir| absp(test_dir) | mhl verify | test_dir/test_dir_2 | -v test_dir_2_1/* absp(hash-list2.txt) |
        | 3, 4, 6, 7 | test_dir/test_dir_2/test_dir_2_1 | absp(test_dir) | mhl verify | test_dir/test_dir_1/test_dir_1_1 | -v absp(../hash-list4.txt) absp(../../test_dir_2/test_dir_2_1/hash-list2.txt) |
        | 4, 5, 6, 7 | test_dir/test_dir_1 | absp(test_dir) | mhl verify | test_dir/test_dir_2 | -v absp(../test_dir_1/*.txt) *.txt |
        | 1, 3, 4, 5, 6, 7, 8 | test_dir/test_dir_1 | absp(test_dir) | mhl verify | test_dir/test_dir_2 | -v -f ../test_dir_1/./../*.mhl absp(test_dir_2_1/*) ../test_dir_1/*.txt |
        | 2        | test_dir| absp(test_dir) | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f absp(test_dir_1/test_dir_1_2/*) |
        | 2        | test_dir| absp(test_dir) | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f absp(test_dir_1/test_dir_1_2/hash-list1.txt) |
        | 1, 3, 8  | test_dir| absp(test_dir) | mhl hash | test_dir | absp(test_dir_2/test_dir_2_1/*) test_dir_1/./../test_dir_1/test_dir_1_1/hash-list1.txt |
        | 4, 6     | test_dir| absp(test_dir) | mhl hash | test_dir | absp(test_dir_2/*.txt) |
        | 7        | test_dir| absp(test_dir) | mhl hash | test_dir/test_dir_1/test_dir_1_1 | absp(../hash-list4.txt) |


    Scenario Outline: mhl seal and verify: Absolute filepaths
        Given I have the files
        | orig_number | filename       |
        | 1           | hash-list1.txt |
        | 2           | hash-list2.txt |
        | 3           | hash-list3.txt |
        | 4           | hash-list4.txt |
        When I put the given files into special directories
        | filenumber | orig_number | directory_name |
        | 1          | 1           | test_dir/test_dir_1/test_dir_1_1 |
        | 2          | 1           | test_dir/test_dir_1/test_dir_1_2 |
        | 3          | 2           | test_dir/test_dir_2/test_dir_2_1 |
        | 4          | 2           | test_dir/test_dir_2 |
        | 5          | 3           | test_dir/test_dir_1 |
        | 6          | 3           | test_dir/test_dir_2 |
        | 7          | 4           | test_dir/test_dir_1 |
        | 8          | 4           | test_dir/test_dir_2/test_dir_2_1 |
        And I run 'mhl seal' from "<work_d1>" with '<args1>'
        And the return code is 0
        And the '.mhl' file is created in "<mhl_dir>"
        Then I run 'mhl verify' from "<work_d2>" with '<args2>'
        And the return code is 0.

    Examples:
        | work_d1  | args1 | mhl_dir | work_d2 | args2 |
        | test_dir | -o absp(test_dir_1) test_dir_1 | absp(test_dir/test_dir_1) | test_dir/test_dir_1 | -v -f absp(*.mhl) |
        | test_dir | -o . absp(test_dir_2) | test_dir | test_dir/test_dir_2 | -v -f absp(../*.mhl) absp(test_dir_2_1/*) |
        | test_dir | test_dir_2 | absp(test_dir) | test_dir/test_dir_2 | -v test_dir_2_1 absp(hash-list2.txt) |
        | test_dir | absp(test_dir_2/*) absp(test_dir_1/hash-list4.txt) | absp(test_dir) | test_dir/test_dir_1/test_dir_1_1 | -v absp(../hash-list4.txt) absp(../../test_dir_2/test_dir_2_1/hash-list2.txt) |
        | test_dir/test_dir_1 | -o absp(..) ../* | absp(test_dir) | test_dir/test_dir_2 | -v absp(../test_dir_1/*.txt) *.txt |
