#include "inc/cester.h"
#include "inc/test_lstar_aux.h"

CESTER_TEST(__________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________LSTAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(___________________________________, _, cester_assert_nothing();)

CESTER_TEST(Lstar_should_write_as_expected_from_the_root_of_valid_tarball, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected = "file4 dir1 dir2";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-04",
			"",
			FALSE,
			buffer
		)
	);
	/* -2 because of the extra " \n" at the end.*/
	cester_assert_int_eq(strlen(expected), strlen(buffer) - 2);
	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Lstar_should_write_as_expected_from_a_valid_dir_of_valid_tarball, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected = "file3 file2";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-04",
			"dir2/",
			FALSE,
			buffer
		)
	);
	/* -2 because of the extra " \n" at the end.*/
	cester_assert_int_eq(strlen(expected), strlen(buffer) - 2);
	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Lstar_should_write_as_expected_from_a_valid_dir_containing_only_dir_of_valid_tarball, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected = "dir122 dir121";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-12",
			"dir1/dir12/",
			FALSE,
			buffer
		)
	);
	/* -2 because of the extra " \n" at the end.*/
	cester_assert_int_eq(strlen(expected), strlen(buffer) - 2);
	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

/* test-tar-ls-8 doesn't contains a header named "dir/" at the root.
 * For a better understanding execute
 *
 *     tar -tvf test/resources/backup-test-tar-files/test-tar-ls-8
 * */
CESTER_TEST(Lstar_should_write_as_expected_from_a_non_header_dir_of_valid_tarball, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected = "dir";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-8",
			"",
			FALSE,
			buffer
		)
	);
	/* -2 because of the extra " \n" at the end.*/
	cester_assert_int_eq(strlen(expected), strlen(buffer) - 2);
	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Lstar_with_option_should_write_as_expected_from_a_valid_tarball_containing_only_one_file, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 4\n"
		"-rw-rw-r-- 1 emile emile 10240 Oct  8 20:22 file1\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-02",
			"",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_write_as_expected_from_a_valid_tarball_containing_dir_and_file, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 8\n"
		"drwxrwxr-x 2 emile emile 4096 Oct  8 14:06 build\n"
		"-rw-rw-r-- 1 emile emile 1181 Oct  6 10:08 Makefile\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-1",
			"",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_write_as_expected_from_a_valid_dir_of_a_valid_tarball_containing_only_files, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 24\n"
		"-rw-rw-r-- 1 emile emile 2801 Oct 10 13:26 commons_tar.h\n"
		"-rw-rw-r-- 1 emile emile 2246 Oct  6 12:33 tar.h\n"
		"-rw-rw-r-- 1 emile emile  319 Oct 10 11:14 commons.h\n"
		"-rw-rw-r-- 1 emile emile  259 Oct  8 14:43 tsh.h\n"
		"-rw-rw-r-- 1 emile emile 2313 Oct 10 12:53 lstar.h\n"
		"-rw-rw-r-- 1 emile emile  555 Oct 10 11:14 cattar.h\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-2",
			"src/inc/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_write_as_expected_from_a_valid_dir_of_a_valid_tarball_containing_a_symbolic_link, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 4\n"
		"-rw-rw-r-- 1 emile emile 1516 Oct 11 17:36 file\n"
		"lrwxrwxrwx 1 emile emile    0 Oct 11 17:36 lnsfile -> dir/file\n";


	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-3",
			"dir/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_write_as_expected_from_a_valid_dir_of_a_valid_tarball_containing_a_physical_link_and_multiple_linked_file, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 16\n"
		"-rw-rw-r-- 2 emile emile 1516 Oct 11 17:36 file\n"
		"drwxrwxr-x 2 emile emile 4096 Oct 11 19:37 dir2\n"
		"-rw-rw-r-- 2 emile emile 1516 Oct 11 17:36 lnpfile\n"
		"lrwxrwxrwx 1 emile emile    0 Oct 11 17:36 lnsfile -> dir/file\n"
		"drwxrwxr-x 2 emile emile 4096 Oct 11 19:36 dir1\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-4",
			"dir/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_write_as_expected_from_a_valid_dir_of_a_valid_tarball_containing_a_physical_link_and_multiple_linked_file_2, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 16\n"
		"-rw-rw-r-- 3 emile emile 1516 Oct 14 16:46 file\n"
		"drwxrwxr-x 3 emile emile 4096 Oct 15 11:11 dir2\n"
		"-rw-rw-r-- 3 emile emile 1516 Oct 14 16:46 lnpfile\n"
		"drwxrwxr-x 4 emile emile 4096 Oct 15 11:11 dir1\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-5",
			"dir/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_write_the_right_number_of_link_for_a_dir_containing_multiple_sub_dirs, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 4\n"
		"drwxrwxr-x 4 emile emile 4096 Oct 14 16:49 dir\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-5",
			"",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_write_as_expected_from_a_valid_dir_of_a_valid_tarball_containing_a_physical_link_to_a_file_of_an_other_dir, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 12\n"
		"lrwxrwxrwx 1 emile emile    0 Oct 11 19:36 lnsfile2 -> dir/file\n"
		"-rw-rw-r-- 3 emile emile 1516 Oct 14 16:46 filetest\n"
		"drwxrwxr-x 2 emile emile 4096 Oct 15 11:11 dir12\n"
		"drwxrwxr-x 2 emile emile 4096 Oct 15 11:11 dir11\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-5",
			"dir/dir1/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_align_usernames, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 16\n"
		"-rw-rw-r-- 3 nvidia-persistenced emile 1516 Oct 14 16:46 file\n"
		"drwxrwxr-x 3 emile               emile 4096 Oct 15 11:11 dir2\n"
		"-rw-rw-r-- 3 nvidia-persistenced emile 1516 Oct 14 16:46 lnpfile\n"
		"drwxrwxr-x 4 emile               emile 4096 Oct 15 11:11 dir1\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-6",
			"dir/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_align_groupnames, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 4\n"
		"lrwxrwxrwx 1 emile emile    0 Oct 11 19:37 lnsfile3 -> dir/file\n"
		"drwxrwxr-x 2 emile adm   4096 Oct 15 11:11 dir21\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-6",
			"dir/dir2/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_should_write_as_expected_from_a_non_header_dir_of_valid_tarball_containing_a_symbolic_link, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 12\n"
		"drwxrwxr-x 2 emile               adm   4096 Oct 15 11:11 dir11\n"
		"drwxrwxr-x 2 emile               emile 4096 Oct 15 11:11 dir12\n"
		"-rw-rw-r-- 3 nvidia-persistenced emile 1516 Oct 14 16:46 filetest\n"
		"lrwxrwxrwx 1 emile               emile    0 Oct 11 19:36 lnsfile2 -> dir/file\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-7",
			"dir/dir1/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_should_align_groupnames_and_usernames, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 12\n"
		"drwxrwxr-x 2 emile               adm   4096 Oct 15 11:11 dir11\n"
		"lrwxrwxrwx 1 emile               emile    0 Oct 11 19:36 lnsfile2 -> dir/file\n"
		"-rw-rw-r-- 1 nvidia-persistenced emile 1516 Oct 14 16:46 filetest\n"
		"drwxrwxr-x 2 emile               emile 4096 Oct 15 11:11 dir12\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-8",
			"dir/dir1/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_of_dir_that_contains_a_non_header_dir_should_print_question_marks, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 4\n"
		"d????????? 4 ? ? 4096 ??? ?? ??:?? dir\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-8",
			"",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_with_option_of_dir_that_contains_a_non_header_dir_and_other_files_should_print_aligned_question_marks, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"total 16\n"
		"d????????? 4 ?                   ?     4096 ??? ?? ??:?? dir1\n"
		"drwxrwxr-x 3 emile               emile 4096 Oct 15 11:11 dir2\n"
		"-rw-rw-r-- 3 nvidia-persistenced emile 1516 Oct 14 16:46 file\n"
		"-rw-rw-r-- 3 nvidia-persistenced emile 1516 Oct 14 16:46 lnpfile\n";

	cester_assert_true(
		test_lstar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-ls-7",
			"dir/",
			TRUE,
			buffer
		)
	);
	cester_assert_int_eq(strlen(expected), strlen(buffer));
	cester_assert_str_equal(
		buffer, expected
	);
)

CESTER_TEST(Lstar_should_not_access_an_unexisting_directory, _,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-ls-1", O_RDONLY);
	const char *path = "unexisting/";
	const return_handler rh = lstar(tar_fd, FALSE, path);

	cester_assert_equal(
		ERR, rh.verdict
	);
	cester_assert_str_equal(
		"ls: cannot access 'unexisting/': No such file or directory\n", rh.message
	);
	close(tar_fd);
)