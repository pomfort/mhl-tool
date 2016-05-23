Feature: Check sequences

    Scenario Outline: Sequences parsing failure
        Given I have the tool mhl
        And the file is "hash-list.txt"
        When I duplicate the given file to the files sequence "hash-list-{N}.txt" (N from [<filenums>]) in the "test_dir" directory
        And I run 'mhl hash' from "test_dir" with '<options>'
        Then mhlhash fails due to invalid sequence specification.

    Examples:
        | filenums | options                  |
        | 1, 2     | -# hash-list-##2-1.txt   |
        | 1, 2     | -# hash-list-##1a-2.txt  |
        | 1        | -# hash-list-#1-1.txt    |


    Scenario Outline: Gaps in sequences failure
        Given I have the tool mhl
        And the file is "hash-list.txt"
        When I duplicate the given file to the files sequence "hash-list-{N}.txt" (N from [<filenums>]) in the "test_dir" directory
        And I run 'mhl hash' from "test_dir" with '<options>'
        Then mhlhash fails due to gaps in files sequence.

    Examples:
        | filenums   | options                     |
        | 1, 2       | -# hash-list-#1-3.txt    |
        | 08, 10, 11 | -# hash-list-##08-11.txt |
        | 09, 10, 11 | -# hash-list-##08-11.txt |


    Scenario Outline: Success work with sequences
        Given I have the tool mhl
        And the file is "hash-list.txt"
        When I duplicate the given file to the files sequence "<sequence_pattern>" (N from [<filenums>]) in the "test_dir" directory
        And I run '<command>' from "test_dir" with '<options>'
        Then the return code is 0.

    Examples:
        | command  | sequence_pattern | filenums    | options                    |
        | mhl hash | test_{N}.tif     | 01-04       | -# test_##01-04.tif     |
        | mhl hash | test_0{N}.tif    | 1-4         | -# test_0#1-4.tif       |
        | mhl hash | test_{N}.tif     | 08-11       | -# test_##08-11.tif     |
        | mhl hash | test_{N}.tif     | 4-9         | -# md5 test_#4-9.tif    |
        | mhl hash | test_{N}0.tif    | 4-9         | -v -# test_#4-90.tif    |
        | mhl hash | test_{N}.tif     | 8-12        | -# sha1 test_##8-12.tif |
        | mhl seal | test_0{N}.tif    | 1-7         | -# test_0#1-3.tif test_04.tif test_0#5-7.tif |
        | mhl seal | test_{N}.tif     | 08-11       | -o . -# -t md5 test_##08-11.tif |
        | mhl seal | {N}.tif          | 01-04       | -# "##01-04.tif"     |
        | mhl seal | test_{N}         | 01-04       | -# test_##01-04    |
        | mhl hash | {N}              | 01-04       | -# "##01-04"         |
        | mhl seal | test_{N}.tif    | 01-04       | -# test_##1-4.tif   |
        | mhl hash | test_{N}         | 002-033     | -# test_###2-33      |
