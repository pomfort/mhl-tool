Feature: Check hashing by removing or changing the last bytes

    Scenario: Deleted last bytes
        Given I have the file "hash-list.txt" and the corresponding mhl file "aux_files_2012-11-24_103236.mhl"
        When I duplicate the given MHL file into "test_dir" directory
        And I duplicate the given file into "test_dir" directory
        And I delete 5 bytes at the end of test file
        Then mhl verification check fails due non-matched file size

    Scenario: Change last bytes
        Given I have the file "hash-list.txt" and the corresponding mhl file "aux_files_2012-11-24_103236.mhl"
        When I duplicate the given MHL file into "test_dir" directory
        And I duplicate the given file into "test_dir" directory
        And I change 5 bytes at the end of test file to '\0'
        Then mhl verification check fails due non-matched hash sum
