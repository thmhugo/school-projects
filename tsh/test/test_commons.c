#include "inc/cester.h"

#include "../src/inc/commons.h"

CESTER_TEST(_____________________, _, cester_assert_nothing();)
CESTER_TEST(______________COMMONS_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(______________, _, cester_assert_nothing();)

/* TESTS associated to commons_copy_file_content. */

/* Normal text file. */
CESTER_TEST(CopyFileContentFrom_with_a_normal_text_file, _,
	const char *expected_content =
		"TestFile TestFile TestFile TestFile TestFile "
		"TestFile TestFile TestFile TestFile TestFile TestFile\n";
	const char *file_path = "test/resources/current-test-tar-files/normal-file-01";
	const int fd = open(file_path, O_RDONLY);
	char actual_content[BUFSIZ];

	cester_assert_equal(
		commons_get_file_size(fd),
		commons_copy_file_content_from(file_path, 0, BUFSIZ, actual_content)
	);
	cester_assert_str_equal(expected_content, actual_content);
	close(fd);
)

/* Empty file. */
CESTER_TEST(CopyFileContentFrom_with_an_empty_file, _,
	const char *file_path = "test/resources/current-test-tar-files/empty-file";
	char actual_content[BUFSIZ];

	cester_assert_equal(
		0,
		commons_copy_file_content_from(file_path, 0, BUFSIZ, actual_content)
	);
	cester_assert_str_equal("", actual_content);
)

/* None existing file. */
CESTER_TEST(CopyFileContentFrom_with_a_none_existing_file_should_return_ERR, _,
	const char *file_path = "test/resources/current-test-tar-files/none-existing";
	char actual_content[BUFSIZ];

	cester_assert_equal(
		ERR,
		commons_copy_file_content_from(file_path, 0, BUFSIZ, actual_content)
	);
	cester_assert_equal(0, strlen(actual_content));
)

/* Directory. */
CESTER_TEST(CopyFileContentFrom_with_a_directory_should_return_ERR, _,
	const char *file_path = "test/resources/current-test-tar-files/";
	char actual_content[BUFSIZ];

	cester_assert_equal(
		ERR,
		commons_copy_file_content_from(file_path, 0, BUFSIZ, actual_content)
	);
	cester_assert_equal(0, strlen(actual_content));
)

/* Normal text file with its size > BUFSIZ. */
CESTER_TEST(CopyFileContentFrom_with_a_big_text_file, _,
	const char *file_path = "test/resources/current-test-tar-files/normal-file-big";
	const int fd = open(file_path, O_RDONLY);
	const ssize_t file_size = commons_get_file_size(fd);
	char actual_content[BUFSIZ];
	ssize_t current_offset = 0;
 	ssize_t new_offset = 0;

	while (current_offset < file_size - BUFSIZ) {
		new_offset =
			commons_copy_file_content_from(file_path, current_offset, BUFSIZ, actual_content);
		cester_assert_equal(
			current_offset + BUFSIZ,
			new_offset
		);
		current_offset = new_offset;
	}
	char expected_content[file_size % BUFSIZ];
	memset(expected_content, 'a', file_size % BUFSIZ);
	expected_content[(file_size % BUFSIZ)-1] = '\n';

	memset(actual_content, NONECHAR, BUFSIZ);
	cester_assert_equal(
		file_size,
		commons_copy_file_content_from(file_path, current_offset, BUFSIZ, actual_content)
	);
	cester_assert_true(0 == strncmp(expected_content, actual_content, strlen(actual_content)));
	close(fd);
)

/* Symbolic link to a normal file. */
CESTER_TEST(CopyFileContentFrom_with_a_symbolic_link, _,
	const char *expected_content =
		"TestFile TestFile TestFile TestFile TestFile "
		"TestFile TestFile TestFile TestFile TestFile TestFile\n";
	const int fd = open("test/resources/current-test-tar-files/normal-file-01", O_RDONLY);
	const char *file_path = "test/resources/current-test-tar-files/sl-normal-file-01";
	char actual_content[BUFSIZ];

	cester_assert_equal(
		commons_get_file_size(fd),
		commons_copy_file_content_from(file_path, 0, BUFSIZ, actual_content)
	);
	cester_assert_str_equal(expected_content, actual_content);
	close(fd);
)


/* TESTS associated to commons_remove_file */
CESTER_TEST(_______________________________________, _, cester_assert_nothing();)

CESTER_TEST(RemoveFiles_with_a_normal_file, _,
		const char *file_path = "test/resources/current-test-tar-files/test-remove-files-00";

		cester_assert_equal(OK, commons_remove_file(file_path));
		cester_assert_equal(ERR, open(file_path, O_RDONLY));
)

CESTER_TEST(RemoveFiles_with_an_empty_folder, _,
		const char *file_path = "test/resources/current-test-tar-files/test-remove-files-01/";

		cester_assert_equal(OK, commons_remove_file(file_path));
		cester_assert_equal(NULL, opendir(file_path));
)

CESTER_TEST(RemoveFiles_with_a_folder_filled_with_two_subdir, _,
		const char *file_path = "test/resources/current-test-tar-files/test-remove-files-02/";

		cester_assert_equal(OK, commons_remove_file(file_path));
		cester_assert_equal(NULL, opendir(file_path));
)

CESTER_TEST(RemoveFiles_with_a_folder_filled_with_subdir_of_dir, _,
		const char *file_path = "test/resources/current-test-tar-files/test-remove-files-03/";

		cester_assert_equal(OK, commons_remove_file(file_path));
		cester_assert_equal(NULL, opendir(file_path));
)

CESTER_TEST(RemoveFiles_with_a_folder_filled_with_symbolic_and_hard_links, _,
		const char *file_path = "test/resources/current-test-tar-files/test-remove-files-04/";

		cester_assert_equal(OK, commons_remove_file(file_path));
		cester_assert_equal(NULL, opendir(file_path));
)

CESTER_TEST(RemoveFiles_with_a_none_existing_file_should_return_ERR, _,
		const char *file_path = "test/resources/current-test-tar-files/test-remove-files-999/";

		cester_assert_equal(ERR, commons_remove_file(file_path));
)