Feature: Check how MHL tools work with Non-ASCII characters in file paths

    Scenario Outline: Only non-ASCII characters in filepaths, system default NFC & NFD
        Given I have the files
        | orig_number | filename             |
        | 1           | русский              |
        | 2           | файлнарусском        |
        | 3           | 日本人               |
        | 4           | 日本語ファイル       |
        | 5           | 한국의               |
        | 6           | 한국어의파일         |
        | 7           | русский日本人한국의  |
        | 8           | 한국의日本人русский  |
        And I am using the following directory names
        | orig_number | dirname              |
        | 1           | нарусскомкаталог     |
        | 2           | 한국어디렉토리       |
        | 3           | 日本のディレクトリ   |
        | 4           | русский日本人한국의  |
        When I create the directory tree with the following structure in "test_dir"
        | directory_numbered_path | file_numbers |
        | 4/1                       | 1, 2         |
        | 4/2                       | 3, 4         |
        | 4/3                       | 5, 6         |
        | 4/4                       | 7, 8         |
        | 4/1/2                     | 1, 2, 3, 8    |
        | 4/3/2/1                   | 1, 2, 3, 4, 5, 6   |
        | 4/4/3/2/1                 | 2, 3, 4, 5, 6, 7, 8 |
        | 4/4/3/1/2                 | 1, 4, 6, 7, 8    |
        | 4/4/3                     | 2, 3, 4, 5, 6    |
        | 4/4/3/2/1                 | 2, 4, 5, 6, 7, 8  |
        And for the given files I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        Then the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        And the return code is 0
        And I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | work_d1 | mhl_dir | command | work_d2 | options |
        | test_dir| . | mhl verify | test_dir | -v -f *.mhl |
        | test_dir/testname_dir(4) | . | mhl verify | test_dir | -v -f testname_dir(4)/*.mhl |
        | test_dir | testname_dir(4) | mhl verify | test_dir | -v -e -f testname_dir(4)/*.mhl testname_dir(4)/testname_dir(1)/testname_dir(2)/* |
        | test_dir| . | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(4)/testname_dir(2)/testname_file(3) |
        | test_dir| testname_dir(4) | mhl hash | test_dir/testname_dir(4) | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(3)/testname_dir(2)/testname_dir(1)/testname_file(5) |
        | test_dir| testname_dir(4) | mhl hash | test_dir/testname_dir(4) | testname_dir(3)/testname_dir(2)/testname_dir(1)/* testname_dir(4)/testname_dir(3)/日本* testname_dir(4)/testname_dir(3)/файлнарусском |
        | test_dir/testname_dir(4) | . | mhl seal | test_dir | -o testname_dir(4) testname_dir(4)/testname_dir(3) |


    Scenario Outline: relative test non-ASCII characters in filepaths, system default NFC & NFD
        And I have the files
        | orig_number | filename             |
        | 1           | русский              |
        | 2           | файлнарусском        |
        | 3           | русский_file.txt           |
        | 4           | russian_файлнарусском.тхт  |
        When I put the given files into special directories
        | filenumber | orig_number | directory_name |
        | 1          | 1           | test_dir/russian-каталог/russian-каталог |
        | 2          | 2           | test_dir/russian-каталог/russian-каталог |
        | 3          | 3           | test_dir/russian-каталог/нарусскомкаталог |
        | 4          | 4           | test_dir/russian-каталог/нарусскомкаталог |
        And from 'test_dir' I run openssl to make md5 hashes for files in specified order and I run 'mhl hash' with './russian-каталог'
        | OS_type         | files_order |
        | Windows, Darwin | 1, 2, 4, 3  |
        | Linux           | 1, 2, 4, 3  |
        Then the outputs of 'mhl hash' and openssl are the same


    Scenario Outline: Mixed ASCII and non-ASCII characters in filepaths NFC
        Given I have the files
        | orig_number | filename                         |
        | 1           | f_nfc_русский_file.txt           |
        | 2           | f_nfc_russian_файлнарусском.тхт  |
        | 3           | f_nfc_japanese_日本人            |
        | 4           | f_nfc_日本語ファ_イル.txt        |
        | 5           | f_nfc_korean_корейский_한국의    |
        | 6           | f_nfc_한국어F의파일.txt          |
        | 7           | f_nfc_русский日本人한국의        |
        | 8           | f_nfc_한국의日本人русский        |
        And I am using the following directory names
        | orig_number | dirname                    |
        | 1           | russian-каталог            |
        | 2           | 한국어디렉토리_dir         |
        | 3           | 日本のデ_F_ィレクトリ      |
        | 4           | русский_日本_人N한국의     |
        When I create the directory tree with the following structure in "test_dir"
        | directory_numbered_path | file_numbers |
        | 2/1                   | 1, 2          |
        | 2/2                   | 3, 4          |
        | 2/3                   | 5, 6          |
        | 2/4                   | 7, 8          |
        | 2/1/2                 | 1, 2, 3, 8    |
        | 2/3/2/1               | 1, 2, 3, 4, 5, 6  |
        | 2/4/3/2/1             | 2, 3, 4, 5, 6, 7, 8|
        | 2/4/3/1/2             | 1, 4, 6, 7, 8    |
        | 2/4/3                 | 2, 3, 4, 5, 6    |
        | 2/4/3/2/1             | 2, 4, 5, 6, 7, 8  |
        And for the given files I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        Then the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        And the return code is 0
        And I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | work_d1 | mhl_dir | command | work_d2 | options |
        | test_dir| . | mhl verify | test_dir | -v -f *.mhl |
        | test_dir/testname_dir(2) | . | mhl verify | test_dir | -v -f testname_dir(2)/*.mhl |
        | test_dir | testname_dir(2) | mhl verify | test_dir | -v -e -f testname_dir(2)/*.mhl testname_dir(2)/testname_dir(1)/testname_dir(2)/f_nfc_* |
        | test_dir| . | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(2)/testname_dir(2)/testname_file(3) |
        | test_dir| testname_dir(2) | mhl hash | test_dir/testname_dir(2) | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(3)/testname_dir(2)/testname_dir(1)/testname_file(5) |
        | test_dir| testname_dir(2) | mhl hash | test_dir/testname_dir(2) | testname_dir(3)/testname_dir(2)/testname_dir(1)/f_nfc_* testname_dir(4)/testname_dir(3)/f_nfc_* |
#        | test_dir/testname_dir(2) | . | mhl sign | test_dir | testname_dir(2)/*.mhl |

    Scenario Outline: Mixed ASCII and non-ASCII characters in filepaths NFD
        Given I have the files
        | orig_number | filename                         |
        | 1           | f_nfd_русский_file.txt           |
        | 2           | f_nfd_russian_файлнарусском.тхт  |
        | 3           | f_nfd_japanese_日本人            |
        | 4           | f_nfd_日本語ファ_イル.txt        |
        | 5           | f_nfd_korean_корейский_한국의    |
        | 6           | f_nfd_한국어F의파일.txt          |
        | 7           | f_nfd_русский日本人한국의        |
        | 8           | f_nfd_한국의日本人русский        |
        And I am using the following directory names
        | orig_number | dirname                    |
        | 1           | russian-каталог            |
        | 2           | 한국어디렉토리_dir         |
        | 3           | 日本のデ_F_ィレクトリ      |
        | 4           | русский_日本_人N한국의     |
        When I create the directory tree with the following structure in "test_dir"
        | directory_numbered_path | file_numbers |
        | 2/1                   | 1, 2          |
        | 2/2                   | 3, 4          |
        | 2/3                   | 5, 6          |
        | 2/4                   | 7, 8          |
        | 2/1/2                 | 1, 2, 3, 8    |
        | 2/3/2/1               | 1, 2, 3, 4, 5, 6  |
        | 2/4/3/2/1             | 2, 3, 4, 5, 6, 7, 8|
        | 2/4/3/1/2             | 1, 4, 6, 7, 8    |
        | 2/4/3                 | 2, 3, 4, 5, 6    |
        | 2/4/3/2/1             | 2, 4, 5, 6, 7, 8  |
        And for the given files I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        Then the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        And the return code is 0
        And I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | work_d1 | mhl_dir | command | work_d2 | options |
        | test_dir| . | mhl verify | test_dir | -v -f *.mhl |
        | test_dir/testname_dir(2) | . | mhl verify | test_dir | -v -f testname_dir(2)/*.mhl |
        | test_dir | testname_dir(2) | mhl verify | test_dir | -v -e -f testname_dir(2)/*.mhl testname_dir(2)/testname_dir(1)/testname_dir(2)/f_nfd_* |
        | test_dir| . | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(2)/testname_dir(2)/testname_file(3) |
        | test_dir| testname_dir(2) | mhl hash | test_dir/testname_dir(2) | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(3)/testname_dir(2)/testname_dir(1)/testname_file(5) |
        | test_dir| testname_dir(2) | mhl hash | test_dir/testname_dir(2) | testname_dir(3)/testname_dir(2)/testname_dir(1)/f_nfd_* testname_dir(4)/testname_dir(3)/f_nfd_* |
#        | test_dir/testname_dir(2) | . | mhl sign | test_dir | testname_dir(2)/*.mhl |

    Scenario: Non-ASCII European characters in filepaths NFC
        Given I have the files
        | orig_number | filename               |
        | 1           | f_nfc_Straße           |
        | 2           | f_nfc_Kölner_Dom.avi   |
        | 3           | f_nfc_CAFÉ.txt         |
        | 4           | f_nfc_haïku.txt        |
        | 5           | f_nfc_νερό             |
        | 6           | f_nfc_ήλιος.txt        |
        | 7           | f_nfc_dŵr.txt          |
        | 8           | f_nfc_słowo.avi        |
        And I am using the following directory names
        | orig_number | dirname          |
        | 1           | Mélanger         |
        | 2           | πίστωση          |
        | 3           | FER_À_CHEVAL     |
        | 4           | plošina          |
        When I create the directory tree with the following structure in "test_dir"
        | directory_numbered_path | file_numbers |
        | 1/1                   | 1, 2          |
        | 1/2                   | 3, 4          |
        | 1/3                   | 5, 6          |
        | 1/4                   | 7, 8          |
        | 1/1/2                 | 1, 2, 3, 8    |
        | 1/3/2/1               | 1, 2, 3, 4, 5, 6  |
        | 1/4/3/2/1             | 2, 3, 4, 5, 6, 7, 8|
        | 1/4/3/1/2             | 1, 4, 6, 7, 8    |
        | 1/4/3                 | 2, 3, 4, 5, 6    |
        | 1/4/3/2/1             | 2, 4, 5, 6, 7, 8  |
        And for the given files I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        Then the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        And the return code is 0
        And I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | work_d1 | mhl_dir | command | work_d2 | options |
        | test_dir| . | mhl verify | test_dir | -v -f *.mhl |
        | test_dir/testname_dir(1) | . | mhl verify | test_dir | -v -f testname_dir(1)/*.mhl |
        | test_dir | testname_dir(1) | mhl verify | test_dir | -v -e -f testname_dir(1)/*.mhl testname_dir(1)/testname_dir(1)/testname_dir(2)/f_nfc_* |
        | test_dir| . | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(1)/testname_dir(2)/testname_file(3) |
        | test_dir| testname_dir(1) | mhl hash | test_dir/testname_dir(1) | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(3)/testname_dir(2)/testname_dir(1)/testname_file(5) |
        | test_dir| testname_dir(1) | mhl hash | test_dir/testname_dir(1) | testname_dir(3)/testname_dir(2)/testname_dir(1)/f_nfc_* testname_dir(4)/testname_dir(3)/f_nfc_* |
#        | test_dir/testname_dir(1) | . | mhl sign | test_dir | testname_dir(1)/*.mhl |

    Scenario: Non-ASCII European characters in filepaths NFD
        Given I have the files
        | orig_number | filename               |
        | 1           | f_nfd_Straße           |
        | 2           | f_nfd_Kölner_Dom.avi   |
        | 3           | f_nfd_CAFÉ.txt         |
        | 4           | f_nfd_haïku.txt        |
        | 5           | f_nfd_νερό             |
        | 6           | f_nfd_ήλιος.txt        |
        | 7           | f_nfd_dŵr.txt          |
        | 8           | f_nfd_słowo.avi        |
        And I am using the following directory names
        | orig_number | dirname          |
        | 1           | Mélanger         |
        | 2           | πίστωση          |
        | 3           | FER_À_CHEVAL     |
        | 4           | plošina          |
        When I create the directory tree with the following structure in "test_dir"
        | directory_numbered_path | file_numbers |
        | 1/1                   | 1, 2          |
        | 1/2                   | 3, 4          |
        | 1/3                   | 5, 6          |
        | 1/4                   | 7, 8          |
        | 1/1/2                 | 1, 2, 3, 8    |
        | 1/3/2/1               | 1, 2, 3, 4, 5, 6  |
        | 1/4/3/2/1             | 2, 3, 4, 5, 6, 7, 8|
        | 1/4/3/1/2             | 1, 4, 6, 7, 8    |
        | 1/4/3                 | 2, 3, 4, 5, 6    |
        | 1/4/3/2/1             | 2, 4, 5, 6, 7, 8  |
        And for the given files I run openssl from "<work_d1>" to make md5 hash and redirect output to 'mhl file -s -o "<mhl_dir>"' as input
        Then the '.mhl' file is created in "<mhl_dir>" relative to "<work_d1>"
        And the return code is 0
        And I run '<command>' from "<work_d2>" with '<options>'
        And the return code is 0.

    Examples:
        | work_d1 | mhl_dir | command | work_d2 | options |
        | test_dir| . | mhl verify | test_dir | -v -f *.mhl |
        | test_dir/testname_dir(1) | . | mhl verify | test_dir | -v -f testname_dir(1)/*.mhl |
        | test_dir | testname_dir(1) | mhl verify | test_dir | -v -e -f testname_dir(1)/*.mhl testname_dir(1)/testname_dir(1)/testname_dir(2)/f_nfd_* |
        | test_dir| . | mhl hash | test_dir | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(1)/testname_dir(2)/testname_file(3) |
        | test_dir| testname_dir(1) | mhl hash | test_dir/testname_dir(1) | -h d29593e2cf81c621fb3c4089f9bbde4a -f testname_dir(3)/testname_dir(2)/testname_dir(1)/testname_file(5) |
        | test_dir| testname_dir(1) | mhl hash | test_dir/testname_dir(1) | testname_dir(3)/testname_dir(2)/testname_dir(1)/f_nfd_* testname_dir(4)/testname_dir(3)/f_nfd_* |
#        | test_dir/testname_dir(1) | . | mhl sign | test_dir | testname_dir(1)/*.mhl |
