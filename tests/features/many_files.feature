Feature: Check MHL tools work with large number of files

    Scenario Outline: mhlhash: work with large number of files
        Given I have the tool mhl
        And the file is "hash-list.txt"
        When I duplicate the given file to the files sequence "test_{N}.tif" (N from [1-<filesnumber>]) in the "test_dir/test_dir_1" directory
        And I run 'mhl hash' from "test_dir/test_dir_1" with '<options>'
        Then the return code is 0.

    Examples:
        | filesnumber | options                         |
        | 100         | -# test_###1-100.tif         |
        | 8191        | -v test_*.tif    |
        | 32767       | -vv -# test_#####1-32767.tif |
        | 65535       | -# test_#####1-65535.tif     |

    Scenario Outline: Work with large number of files
        Given I have the tool mhl
        And the file is "hash-list.txt"
        When I duplicate the given file to the files sequence "test_{N}.tif" (N from [1-<filesnumber>]) in the "test_dir/test_dir_1" directory
        And I run 'mhl hash' from 'test_dir' with '<options1>' and redirect output to 'mhl file' as input
        Then the '.mhl' file is created in "test_dir"
        And the return code is 0
        And I run 'mhl verify' from "test_dir" with '<options2>'
        And the return code is 0.

    Examples:
        | filesnumber | options1                                 | options2 |
        | 100         | -# test_dir_1/test_###1-100.tif          | -v test_dir_1/* |
        | 1000        | md5 -# test_dir_1/test_####1-1000.tif    | -v test_dir_1/* |
        | 32767       | -# sha1 test_dir_1/test_#####1-32767.tif | -vv -f *.mhl |
        | 65535       | -# md5 test_dir_1/test_#####1-65535.tif  | -v -f *.mhl |
