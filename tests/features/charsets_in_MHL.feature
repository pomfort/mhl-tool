Feature: Check how mhlhash work with Non-ASCII characters and different encodings inside MHL file

    Scenario Outline: mhlhash: different encodings in MHL file
        Given I have the directory "<dir_name>"
        And the mhl file is "<MHL_filename>"
        And I have the tool mhl
        When I duplicate the given directory into "test_dir" directory
        And I duplicate the given MHL file into "test_dir" directory
        And I run 'mhl verify' from "test_dir" with '-v -f *.mhl'
        Then the return code is 0.

    Examples:
        | dir_name                   | MHL_filename |
        | mixed_корейский_한국의_dir | aux_files_utf-8_unixln_mixed_2012-12-14_165427.mhl |
        | mixed_корейский_한국의_dir | aux_files_utf-16be_winln_mixed_2012-12-14_165427.mhl |
        | mixed_корейский_한국의_dir | aux_files_utf-16le_macln_mixed_2012-12-14_165427.mhl |
#        | mixed_корейский_한국의_dir | aux_files_utf-32_unixln_mixed_2012-12-14_165427.mhl |
        | Umlaute_und_so_weiter_dir  | aux_files_iso-8859-15_unixln_Umlaute_2012-12-15_174005.mhl |
        | русский_dir                | aux_files_windows-1251_winln_Russian_2012-12-14_170634.mhl |
        | دليل_dir                   | aux_files_iso-8859-6_winln_Arabic_2012-12-15_174827.mhl |
        | 日本人_dir                 | aux_files_euc-jp_macln_Japanese_2012-12-15_180223.mhl |
        | 한국의_dir                 | aux_files_euc-kr_winln_Korean_2012-12-15_181135.mhl |

#    Scenario Outline: mhlsign: different encodings in MHL file
#        Given I have the directory "<dir_name>"
#        And the mhl file is "<MHL_filename>"
#        And I have the tool mhlsign
#        When I duplicate the given directory into "test_dir" directory
#        And I duplicate the given MHL file into "test_dir" directory
#        And I run mhlsign from "test_dir" with '*.mhl'
#        Then the return code is 0.
#
#    Examples:
#        | dir_name                   | MHL_filename |
#        | mixed_корейский_한국의_dir | aux_files_utf-8_unixln_mixed_2012-12-14_165427.mhl |
#        | mixed_корейский_한국의_dir | aux_files_utf-16be_winln_mixed_2012-12-14_165427.mhl |
#        | mixed_корейский_한국의_dir | aux_files_utf-16le_macln_mixed_2012-12-14_165427.mhl |
#        | Umlaute_und_so_weiter_dir  | aux_files_iso-8859-15_unixln_Umlaute_2012-12-15_174005.mhl |
#        | русский_dir                | aux_files_windows-1251_winln_Russian_2012-12-14_170634.mhl |
#        | دليل_dir                   | aux_files_iso-8859-6_winln_Arabic_2012-12-15_174827.mhl |
#        | 日本人_dir                 | aux_files_euc-jp_macln_Japanese_2012-12-15_180223.mhl |
#        | 한국의_dir                 | aux_files_euc-kr_winln_Korean_2012-12-15_181135.mhl |
