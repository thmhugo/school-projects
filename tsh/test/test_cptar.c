#include "inc/cester.h"
#include "../src/inc/cptar.h"
#include "../src/inc/commons_tsh.h"
#include "inc/test_cattar_aux.h"

CESTER_TEST(_____________________________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________CPTAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(______________________________________________________, _, cester_assert_nothing();)

CESTER_TEST(Cptar_should_copy_a_file_in_the_same_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *source_name = "file1";
	const char *destination_name = "cpfile";
	const int tar_fd = open(path, O_RDWR);
	s_posix_header pheader;
	const char *expected_long_text =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. \n"
		"Sed aliquet vestibulum eleifend. Aenean vestibulum in purus nec egestas. \n"
		"Aliquam quis arcu feugiat, finibus nulla in, molestie sem. Q\n"
		"Aliquam a laoreet sem. \n"
		"Fusce sed leo sagittis ligula cursus congue id vel leo. \n"
		"Maecenas in turpis venenatis, tincidunt arcu quis, sagittis neque. \n";
	/* Used to check 'cpfile' content with cattar. */
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			path,
			source_name, tar_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			path,
			destination_name,
			tar_fd
	);

	cester_assert_false(
		commons_tar_find_file_from(tar_fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_file_from(tar_fd, destination_name, &pheader)
	);
	cester_assert_true(
		commons_tar_is_valid(tar_fd)
	);

	/* Check the copied file content, see test_cattar.c. */
	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			path,
			destination_name,
			buffer
		)
	);
	cester_assert_equal(
		0, strncmp(buffer, expected_long_text, strlen(expected_long_text))
	);

	close(tar_fd);
)

CESTER_TEST(Cptar_should_copy_a_file_in_an_other_tarball, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-02.tar";
	const char *source_name = "file1";
	const char *destination_name = "cpfile";
	const int source_fd = open(source_path, O_RDWR);
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;
	const char *expected_long_text =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. \n"
		"Sed aliquet vestibulum eleifend. Aenean vestibulum in purus nec egestas. \n"
		"Aliquam quis arcu feugiat, finibus nulla in, molestie sem. Q\n"
		"Aliquam a laoreet sem. \n"
		"Fusce sed leo sagittis ligula cursus congue id vel leo. \n"
		"Maecenas in turpis venenatis, tincidunt arcu quis, sagittis neque. \n";
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	cester_assert_false(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd,destination_name, &pheader)
	);
	cester_assert_true(
		commons_tar_is_valid(destination_fd))
	;
	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			destination_path,
			destination_name,
			buffer
		)
	);
	cester_assert_equal(
		0, strncmp(buffer, expected_long_text, strlen(expected_long_text))
	);

	close(source_fd);
	close(destination_fd);
)

CESTER_TEST(Cptar_should_copy_a_file_in_an_other_tarball_when_destination_is_a_directory, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-02.tar";
	const char *source_name = "file1";
	const char *destination_name = "folder3/";
	const char *expected_destionation_name = "folder3/file1";
	const int source_fd = open(source_path, O_RDWR);
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;
	
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	cester_assert_false(
		commons_tar_find_file_from(destination_fd, expected_destionation_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd,expected_destionation_name, &pheader)
	);
	cester_assert_true(commons_tar_is_valid(destination_fd));

	close(source_fd);
	close(destination_fd);
)

CESTER_TEST(Cptar_should_not_copy_a_file_that_does_not_exist, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-02.tar";
	const char *source_name = "does-not-exist";
	const char *destination_name = "should-not-exist";
	const int source_fd = open(source_path, O_RDWR);
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	cester_assert_false(
		commons_tar_find_file_from(source_fd, source_name, &pheader)
	);
	cester_assert_false(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal(
		"cp: cannot stat 'does-not-exist': No such file or directory\n", rh.message
	);
	cester_assert_false(
		commons_tar_find_file_from(destination_fd,destination_name, &pheader)
	);

	close(source_fd);
	close(destination_fd);
)

CESTER_TEST(Cptar_should_copy_a_file_from_a_tarball_to_outside, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-02.tar";
	const char *destination_path = "test/resources/current-test-tar-files/normal-dir/cp-tar-test-file.txt";
	const char *source_name = "folder1/smallfile.txt";
	const int source_fd = open(source_path, O_RDONLY);
	const int expected_file_size = 10;
	int destination_file_fd;
	s_posix_header pheader;
	const char *expected_file_content = "small text";
	char read_file_content[expected_file_size+1];

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			"",
			FD_NOTDEF
	);

	memset(read_file_content, 0, expected_file_size+1);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(OK, rh.verdict);
	
	destination_file_fd = open(destination_path, O_RDONLY);
	read(destination_file_fd, read_file_content, expected_file_size);

	cester_assert_equal(
		0, strcmp(expected_file_content, read_file_content)
	);

	close(destination_file_fd);
	close(source_fd);
)


CESTER_TEST(Cptar_should_copy_a_file_from_outside_to_a_tarball, _,
	const char *source_path = "test/resources/current-test-tar-files/normal-dir/cp-tar-test-file.txt";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_name = "folder2/smallfile.txt";
	const int destination_fd = open(destination_path, O_RDWR | O_APPEND);
	s_posix_header pheader;
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected = "small text";

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			"",
			FD_NOTDEF
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	cester_assert_false(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);
	cester_assert_true(
		commons_tar_is_valid(destination_fd)
	);
	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			destination_path,
			destination_name,
			buffer
		)
	);
	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);

	close(destination_fd);
)

CESTER_TEST(Cptar_should_not_copy_a_file_from_inside_that_does_not_exist, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_path = "test/resources/current-test-tar-files/normal-dir/should-not-exist";
	const char *source_name = "does_not_exist";
	const int source_fd = open(source_path, O_RDWR | O_APPEND);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			"",
			FD_NOTDEF
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal(
		"cp: cannot stat 'does_not_exist': No such file or directory\n", rh.message
	);
	cester_assert_equal(
		ERR, open(destination_path, O_RDONLY)
	);

	close(source_fd);
)

CESTER_TEST(Cptar_should_not_copy_a_file_from_outside_that_does_not_exist, _,
	const char *source_path = "test/resources/current-test-tar-files/does-not-exist";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_name = "folder2/newfile.txt";
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			"",
			FD_NOTDEF
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal(
		"cp: 'test/resources/current-test-tar-files/does-not-exist': "
		"No such file or directory\n", 
		rh.message
	);
	cester_assert_false(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);

	close(destination_fd);
)

CESTER_TEST(Cptar_a_directory_without_option_should_return_ERR, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-03.tar";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-02.tar";
	const char *source_name = "folder3/";
	const char *destination_name = "cpfolder/";
	const int source_fd = open(source_path, O_RDWR);
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, FALSE);
	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal(
		"cp: -r not specified; 'folder3/': Directory omited\n", rh.message
	);

	close(source_fd);
	close(destination_fd);
)

/**
 * For the following tests, the content of the files is not tested
 * because in the prior tests, the content of the files has been
 * already checked and the content is the same here.
 */

CESTER_TEST(Cptar_should_copy_a_directory_from_a_tarball_into_an_other_tarball, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-03.tar";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-02.tar";
	const char *source_name = "folder3/";
	const char *destination_name = "cpfolder/";
	const int source_fd = open(source_path, O_RDWR);
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, TRUE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_dir_from(destination_fd, "cpfolder/sfolder3/", &pheader)
	);

	close(source_fd);
	close(destination_fd);
)

CESTER_TEST(Cptar_should_copy_a_directory_from_inside_a_tarball_to_outside, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-04.tar";
	const char *destination_path = "test/resources/current-test-tar-files/normal-dir/cpfolder";
	const char *source_name = "folder/";
	const char *destination_name = "";
	const int source_fd = open(source_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			FD_NOTDEF
	);

	cester_assert_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/")
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, TRUE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_not_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/")
	);
	cester_assert_not_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/folder2/")
	);
	cester_assert_not_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/folder1/")
	);
	cester_assert_not_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/folder2/folder6/")
	);
	cester_assert_not_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/folder2/folder3/")
	);
	cester_assert_not_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/folder1/folder4/")
	);
	cester_assert_not_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cpfolder/folder1/folder5/")
	);
	cester_assert_not_equal(
		ERR, open("test/resources/current-test-tar-files/normal-dir/cpfolder/folder1/folder5/file1", O_RDONLY)
	);
	cester_assert_not_equal(
		ERR, open("test/resources/current-test-tar-files/normal-dir/cpfolder/folder2/folder6/file2", O_RDONLY)
	);
	cester_assert_not_equal(
		ERR, open("test/resources/current-test-tar-files/normal-dir/cpfolder/folder2/folder6/file3", O_RDONLY)
	);

	close(source_fd);
)

CESTER_TEST(Cptar_should_copy_a_directory_from_outside_into_a_tarball, _,
	const char *source_path = "test/resources/current-test-tar-files/normal-dir/cptar-src/";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_name = "cp-from-outside/";
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			"",
			FD_NOTDEF
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	cester_assert_false(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, TRUE);
	cester_assert_equal(OK, rh.verdict);

	cester_assert_true(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd, "cp-from-outside/folder1/", &pheader)
	);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd, "cp-from-outside/folder1/folder3/", &pheader)
	);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd, "cp-from-outside/folder2/", &pheader)
	);
	cester_assert_true(
		commons_tar_is_dir_empty(destination_fd, "cp-from-outside/folder1/folder4/")
	);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd, "cp-from-outside/folder1/folder3/file1.txt", &pheader)
	);
	cester_assert_true(
		commons_tar_is_valid(destination_fd)
	);

	close(destination_fd);
)

CESTER_TEST(Cptar_should_copy_a_file_with_option_from_outside_into_a_tarball, _,
	const char *source_path = "test/resources/current-test-tar-files/normal-dir/cptar-src/folder1/folder3/file1.txt";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-01.tar";
	const char *destination_name = "cp-file-from-outside";
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			"",
			FD_NOTDEF
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	cester_assert_false(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, TRUE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);
	close(destination_fd);
)

CESTER_TEST(Cptar_should_copy_a_file_with_option_from_a_tarball_into_another_tarball, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-03.tar";
	const char *destination_path = "test/resources/current-test-tar-files/test-tar-cptar-04.tar";
	const char *source_name = "folder1/smallfile.txt";
	const char *destination_name = "cpfile-with-option";
	const int source_fd = open(source_path, O_RDWR);
	const int destination_fd = open(destination_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			destination_name,
			destination_fd
	);

	cester_assert_true(
		commons_tar_find_file_from(source_fd, source_name, &pheader)
	);
	cester_assert_false(commons_tar_find_file_from(
		destination_fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, TRUE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_file_from(destination_fd, destination_name, &pheader)
	);
	cester_assert_true(
		commons_tar_is_valid(destination_fd)
	);

	close(source_fd);
	close(destination_fd);
)

CESTER_TEST(Cptar_should_copy_a_directory_in_the_same_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-cptar-05.tar";
	const char *source_name = "folder/";
	const char *destination_name = "cp_folder/";
	const int fd = open(path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			path,
			source_name,
			fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			path,
			destination_name,
			fd
	);

	cester_assert_true(
		commons_tar_find_file_from(fd, source_name, &pheader)
	);
	cester_assert_false(commons_tar_find_file_from(
		fd, destination_name, &pheader)
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, TRUE);
	cester_assert_equal(OK, rh.verdict);
	cester_assert_true(
		commons_tar_find_file_from(fd, destination_name, &pheader)
	);
	cester_assert_true(
		commons_tar_is_valid(fd)
	);

	close(fd);
)

CESTER_TEST(Cptar_should_not_copy_a_directory_if_the_destination_is_a_file, _,
	const char *source_path = "test/resources/current-test-tar-files/test-tar-cptar-04.tar";
	const char *destination_path = "test/resources/current-test-tar-files/normal-dir/cptar-dest";
	const char *source_name = "folder/";
	const int source_fd = open(source_path, O_RDWR);
	s_posix_header pheader;

	s_tsh_path *source_tsh_path =
		commons_tsh_create_s_tsh_path(
			source_path,
			source_name,
			source_fd
	);
	s_tsh_path *destination_tsh_path =
		commons_tsh_create_s_tsh_path(
			destination_path,
			"",
			FD_NOTDEF
	);

	cester_assert_true(
		commons_tar_find_dir_from(source_fd,source_name, &pheader)
	);

	cester_assert_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cptar-dest/")
	);

	const return_handler rh = cptar(*source_tsh_path, *destination_tsh_path, TRUE);
	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal(
		"cp: cannot overwrite non-directory "
		"'test/resources/current-test-tar-files/normal-dir/cptar-dest/folder1/folder5/file1'"
		": with directory\n", 
		rh.message
	);
	cester_assert_null(
		opendir("test/resources/current-test-tar-files/normal-dir/cptar-dest/")
	);

	close(source_fd);
)
