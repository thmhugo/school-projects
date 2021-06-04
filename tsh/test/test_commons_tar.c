#include "inc/cester.h"

#include "inc/commons_test.h"

CESTER_TEST(_________________________, _, cester_assert_nothing();)
CESTER_TEST(______________COMMONS_TAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(__________________________, _, cester_assert_nothing();)

/* TESTS associated to commons_tar_find_file_from. */

CESTER_TEST(FindFileFrom_with_a_valid_text_file_should_return_true, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-11", O_RDONLY);
	const char *path_from_tar_root = "test-file.txt";
	s_posix_header header;

	cester_assert_true(commons_tar_find_file_from(fd, path_from_tar_root, &header));
	cester_assert_str_equal(path_from_tar_root, header.name);
	close(fd);
)

CESTER_TEST(FindFileFrom_with_a_valid_text_file_in_an_other_dir_should_return_true, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-11", O_RDONLY);
	const char *path_from_tar_root = "not-empty-dir/test-file2";
	s_posix_header header;

	cester_assert_true(commons_tar_find_file_from(fd, path_from_tar_root, &header));
	cester_assert_str_equal(path_from_tar_root, header.name);
	close(fd);
)

CESTER_TEST(FindFileFrom_with_a_valid_empty_dir_should_return_true, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-11", O_RDONLY);
	const char *path_from_tar_root = "empty-dir/";
	s_posix_header header;

	cester_assert_true(commons_tar_find_file_from(fd, path_from_tar_root, &header));
	cester_assert_str_equal(path_from_tar_root, header.name);
	close(fd);
)

CESTER_TEST(FindFileFrom_with_should_seek_to_the_beginning_before_searching, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-11", O_RDONLY);
	const char *path_from_tar_root = "empty-dir/";
	char buffer[2 * TAR_BLOCKSIZE];
	s_posix_header header;

	read(fd, buffer, 2 * TAR_BLOCKSIZE);
	cester_assert_true(commons_tar_find_file_from(fd, path_from_tar_root, &header));
	close(fd);
)

CESTER_TEST(FindFileFrom_with_an_inexisting_dir_should_return_false, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-11", O_RDONLY);
	const char *path_from_tar_root = "empty-dir";
	s_posix_header header;

	cester_assert_false(commons_tar_find_file_from(fd, path_from_tar_root, &header));
	close(fd);
)

CESTER_TEST(FindFileFrom_with_an_inexisting_file_should_return_false, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-11", O_RDONLY);
	const char *path_from_tar_root = "empty-dir/test-file";
	s_posix_header header;

	cester_assert_false(commons_tar_find_file_from(fd, path_from_tar_root, &header));
	close(fd);
)

CESTER_TEST(FindFileFrom_called_multiple_times_on_the_same_tarball_should_return_as_expected, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-11", O_RDONLY);
	const char *path_to_not_existing_file_from_tar_root = "empty-dir/test-file";
	const char *path_to_existing_file_from_tar_root = "not-empty-dir/test-file2";
	s_posix_header header;

	cester_assert_false(
		commons_tar_find_file_from(fd, path_to_not_existing_file_from_tar_root, &header)
	);
	cester_assert_true(
		commons_tar_find_file_from(fd, path_to_existing_file_from_tar_root, &header)
	);
	close(fd);
)

/* TESTS associated to commons_tar_is_valid. */

CESTER_TEST(___________________________, _, cester_assert_nothing();)

CESTER_TEST(IsValid_with_a_valid_tar_should_return_true, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-01", O_RDONLY);

	cester_assert_true(commons_tar_is_valid(fd));
	close(fd);
)

CESTER_TEST(IsValid_with_an_invalid_tar_should_return_false, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-03.tar", O_RDONLY);

	cester_assert_false(commons_tar_is_valid(fd));
	close(fd);
)

CESTER_TEST(IsValid_with_an_inexisting_tar_should_return_false, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-00.tar", O_RDONLY);

	cester_assert_false(commons_tar_is_valid(fd));
	close(fd);
)

CESTER_TEST(IsValid_with_a_valid_dir_should_return_false, _,
	const int fd = open("test/resources/current-test-tar-files", O_RDONLY);
	
	cester_assert_false(commons_tar_is_valid(fd));
	close(fd);
)

CESTER_TEST(IsValid_should_not_change_current_offset, _,
	const int fd = open("test/resources/current-test-tar-files/test-tar-01", O_RDONLY);
	ssize_t offset_before;
	ssize_t offset_after;

	offset_before = lseek(fd, TAR_BLOCKSIZE, SEEK_SET);
	cester_assert_true(commons_tar_is_valid(fd));
	
	offset_after = lseek(fd, 0, SEEK_CUR);
	cester_assert_equal(offset_before, offset_after);
	
	close(fd);
)

/* TESTS associated to commons_tar_format_name_according_max_depth. */

CESTER_TEST(_______________________________, _, cester_assert_nothing();)

CESTER_TEST(FormatNameAccordingMaxDepth_0_with_a_valid_name_should_return_an_empty_string, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"",
		commons_tar_format_name_according_max_depth(name, 0)
	);
)

CESTER_TEST(FormatNameAccordingMaxDepth_1_with_a_valid_name_should_return_the_root_dir, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"dir1/",
		commons_tar_format_name_according_max_depth(name, 1)
	);
)

CESTER_TEST(FormatNameAccordingMaxDepth_2_with_a_valid_name_should_return_a_name_of_2_depth, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"dir1/dir2/",
		commons_tar_format_name_according_max_depth(name, 2)
	);
)

CESTER_TEST(FormatNameAccordingMaxDepth_3_with_a_valid_3_depth_name_should_return_the_unchanged_name, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"dir1/dir2/file",
		commons_tar_format_name_according_max_depth(name, 3)
	);
)

CESTER_TEST(FormatNameAccordingMaxDepth_2_with_a_valid_1_depth_name_should_return_the_unchanged_name, _,
	const char *name = "file";

	cester_assert_str_equal(
		"file",
		commons_tar_format_name_according_max_depth(name, 2)
	);
)

CESTER_TEST(FormatNameAccordingMaxDepth_2_with_an_empty_name_should_return_an_empty_string, _,
	const char *name = "";

	cester_assert_str_equal(
		"",
		commons_tar_format_name_according_max_depth(name, 2)
	);
)

CESTER_TEST(FormatNameAccordingMaxDepth_with_a_negative_depth_should_return_an_empty_string, _,
	const char *name = "dir1/file";

	cester_assert_str_equal(
		"",
		commons_tar_format_name_according_max_depth(name, -1)
	);
)

/* TESTS associated to commons_tar_format_name_according_min_depth. */

CESTER_TEST(______________________________, _, cester_assert_nothing();)

CESTER_TEST(FormatNameAccordingMinDepth_0_with_a_valid_name_should_return_the_unchanged_name, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"dir1/dir2/file",
		commons_tar_format_name_according_min_depth(name, 0)
	);
)

CESTER_TEST(FormatNameAccordingMinDepth_1_with_a_valid_3_depth_name_should_return_the_filename_and_its_parent_dir, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"dir2/file",
		commons_tar_format_name_according_min_depth(name, 1)
	);
)

CESTER_TEST(FormatNameAccordingMinDepth_2_with_a_valid_3_depth_name_should_return_the_filename, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"file",
		commons_tar_format_name_according_min_depth(name, 2)
	);
)

CESTER_TEST(FormatNameAccordingMinDepth_3_with_a_valid_3_depth_name_should_return_an_empty_string, _,
	const char *name = "dir1/dir2/file";

	cester_assert_str_equal(
		"",
		commons_tar_format_name_according_min_depth(name, 3)
	);
)

CESTER_TEST(FormatNameAccordingMinDepth_with_an_empty_string_should_return_an_empty_string, _,
	const char *name = "";

	cester_assert_str_equal(
		"",
		commons_tar_format_name_according_min_depth(name, 2)
	);
)

CESTER_TEST(FormatNameAccordingMinDepth_with_a_negative_depth_should_return_the_unchanged_name, _,
	const char *name = "dir1/file";

	cester_assert_str_equal(
		"dir1/file",
		commons_tar_format_name_according_min_depth(name, -1)
	);
)

/* TESTS associated to commons_tar_get_depth. */

CESTER_TEST(____________________________, _, cester_assert_nothing();)

CESTER_TEST(GetDepth_with_a_filename_should_return_1, _,
	const char *name = "file";

	cester_assert_int_eq(1, commons_tar_get_depth(name));
)

CESTER_TEST(GetDepth_with_a_dir_filename_should_return_1, _,
	const char *name = "dir1/";

	cester_assert_int_eq(1, commons_tar_get_depth(name));
)

CESTER_TEST(GetDepth_with_a_2_depth_path_should_return_2, _,
	const char *name = "dir1/file";

	cester_assert_int_eq(2, commons_tar_get_depth(name));
)

CESTER_TEST(GetDepth_with_a_2_depth_path_leading_to_a_dir_should_return_2, _,
	const char *name = "dir1/dir2/";

	cester_assert_int_eq(2, commons_tar_get_depth(name));
)

CESTER_TEST(GetDepth_with_a_5_depth_path_should_return_5, _,
	const char *name = "dir1/dir2/dir3/dir4/file";

	cester_assert_int_eq(5, commons_tar_get_depth(name));
)

CESTER_TEST(GetDepth_with_an_empty_path_should_return_0, _,
	const char *name = "";

	cester_assert_int_eq(0, commons_tar_get_depth(name));
)

CESTER_TEST(GetDepth_with_a_self_ref_path_should_return_0, _,
	const char *name = ".";

	cester_assert_int_eq(0, commons_tar_get_depth(name));
)

/* TESTS associated to commons_tar_blockmove. */
CESTER_TEST(________________________________, _, cester_assert_nothing();)

CESTER_TEST(Blockmove_should_overwrite_an_existing_dir_in_a_valid_tarball, _,
	s_posix_header pheader;
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-blockmove-01", O_RDWR);
	ssize_t src_offset, dest_offset;

	cester_assert_true(ERR != tar_fd);
	cester_assert_true(commons_tar_find_file_from(tar_fd, "doc/png/", &pheader));

	test_blockmove_get_src_and_dest_offset(tar_fd, &pheader, &src_offset, &dest_offset);

    const return_handler rh = commons_tar_blockmove(tar_fd, dest_offset, src_offset);

	cester_assert_equal(OK, rh.verdict);
	cester_assert_false(commons_tar_find_file_from(tar_fd, "doc/png/", &pheader));
	close(tar_fd);
)

CESTER_TEST(Blockmove_should_overwrite_an_existing_file_in_a_valid_tarball, _,
	s_posix_header pheader;
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-blockmove-01", O_RDWR);
	ssize_t src_offset, dest_offset;

	cester_assert_true(ERR != tar_fd);
	cester_assert_true(commons_tar_find_file_from(tar_fd, "doc/guideline.md", &pheader));

	test_blockmove_get_src_and_dest_offset(tar_fd, &pheader, &src_offset, &dest_offset);

    const return_handler rh = commons_tar_blockmove(tar_fd, dest_offset, src_offset);

	cester_assert_equal(OK, rh.verdict);
	cester_assert_false(commons_tar_find_file_from(tar_fd, "doc/guideline.md", &pheader));
	close(tar_fd);
)

CESTER_TEST(Blockmove_should_again_overwrite_an_existing_file_in_a_valid_tarball, _,
	s_posix_header pheader;
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-blockmove-01", O_RDWR);
	ssize_t src_offset, dest_offset;

	cester_assert_true(ERR != tar_fd);
	cester_assert_true(
		commons_tar_find_file_from(tar_fd, "doc/ARCHITECTURE.md", &pheader)
	);

	test_blockmove_get_src_and_dest_offset(tar_fd, &pheader, &src_offset, &dest_offset);

	const return_handler rh = commons_tar_blockmove(tar_fd, dest_offset, src_offset);

	cester_assert_equal(OK, rh.verdict);
	cester_assert_false(
		commons_tar_find_file_from(tar_fd, "doc/ARCHITECTURE.md", &pheader)
	);
	close(tar_fd);
)

CESTER_TEST(Blockmove_with_outbounded_src_should_return_ERR, _,
	s_posix_header pheader;
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-blockmove-01", O_RDWR);

	cester_assert_true(ERR != tar_fd);

	const return_handler rh = commons_tar_blockmove(tar_fd, 0, 10000000);

	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal("commons_tar_blockmove: '': Error reading a file descriptor\n", rh.message);
	close(tar_fd);
)

CESTER_TEST(Blockmove_with_a_negative_dest_should_return_ERR, _,
	s_posix_header pheader;
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-blockmove-01", O_RDWR);

	cester_assert_true(ERR != tar_fd);

	const return_handler rh = commons_tar_blockmove(tar_fd, -1, 1000);

	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal("commons_tar_blockmove: '': Error during removing\n", rh.message);
	close(tar_fd);
)

CESTER_TEST(Blockmove_with_a_negative_dest_and_src_should_return_ERR, _,
	s_posix_header pheader;
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-blockmove-01", O_RDWR);

	cester_assert_true(ERR != tar_fd);

	const return_handler rh = commons_tar_blockmove(tar_fd, -1, -1000);

	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal("commons_tar_blockmove: '': Error during removing\n", rh.message);
	close(tar_fd);
)

CESTER_TEST(Blockmove_should_overwrite_the_last_file_of_a_valid_tarball, _,
	s_posix_header pheader;
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-blockmove-01", O_RDWR);
	ssize_t src_offset, dest_offset;

	cester_assert_true(ERR != tar_fd);
	cester_assert_true(commons_tar_find_file_from(tar_fd, "doc/AUTHORS.md", &pheader));

	test_blockmove_get_src_and_dest_offset(tar_fd, &pheader, &src_offset, &dest_offset);

	const return_handler rh = commons_tar_blockmove(tar_fd, dest_offset, src_offset);

	cester_assert_equal(OK, rh.verdict);
	cester_assert_false(commons_tar_find_file_from(tar_fd, "doc/AUTHORS.md", &pheader));
	close(tar_fd);
)

/* TESTS associated to commons_tar_append_file. */

CESTER_TEST(AppendFile_should_copy_properly_from_a_valid_normal_file, test_instance,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-append-01.tar", O_RDWR);
	s_tsh_path file_path;
	s_posix_header pheader;

	file_path.tar_fd = FD_NOTDEF;
	strcpy(file_path.outside_path, "test/resources/current-test-tar-files/normal-file-01");

	/* The file shouldn't be present in the tarball. */
	cester_assert_false(commons_tar_find_file_from(tar_fd, "normal-file-01", &pheader));

	const return_handler rh = commons_tar_append_file(tar_fd, file_path, NULL);

	cester_assert_equal(OK, rh.verdict);

	lseek(tar_fd, 0, SEEK_SET);
	/* The file should be present in the tarball. */
	cester_assert_true(commons_tar_find_file_from(tar_fd, "normal-file-01", &pheader));

	close(tar_fd);
)

CESTER_TEST(AppendFile_should_copy_properly_from_a_valid_tar_file, test_instance,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-append-01.tar", O_RDWR);
	s_tsh_path file_path;
	s_posix_header pheader;

	file_path.tar_fd = open("test/resources/current-test-tar-files/test-tar-14", O_RDWR);
	strcpy(file_path.outside_path, "test/resources/current-test-tar-files/test-tar-14");
	strcpy(file_path.inside_path, "folder1/file1");

	/* The file shouldn't be present in the tarball. */
	cester_assert_false(commons_tar_find_file_from(tar_fd, "folder1/file1", &pheader));

	const return_handler rh = commons_tar_append_file(tar_fd, file_path, NULL);

	cester_assert_equal(OK, rh.verdict);

	lseek(tar_fd, 0, SEEK_SET);
	lseek(file_path.tar_fd, 0, SEEK_SET);

	/* The file should be present in the tarball. */
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder1/file1", &pheader));

	lseek(tar_fd, 0, SEEK_SET);

	close(file_path.tar_fd);
	close(tar_fd);
)

CESTER_TEST(AppendFile_should_copy_properly_from_a_valid_symbolic_link, test_instance,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-append-01.tar", O_RDWR);
	s_tsh_path file_path;
	s_posix_header pheader;

	file_path.tar_fd = FD_NOTDEF;
	strcpy(file_path.outside_path, "test/resources/current-test-tar-files/sl-normal-file-01");

	/* The file shouldn't be present in the tarball. */
	cester_assert_false(commons_tar_find_file_from(tar_fd, "sl-normal-file-01", &pheader));

	const return_handler rh = commons_tar_append_file(tar_fd, file_path, NULL);

	cester_assert_equal(OK, rh.verdict);

	lseek(tar_fd, 0, SEEK_SET);
	/* The file should be present in the tarball. */
	cester_assert_true(commons_tar_find_file_from(tar_fd, "sl-normal-file-01", &pheader));

	close(tar_fd);
)

CESTER_TEST(AppendFile_should_copy_properly_from_a_valid_physical_link, test_instance,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-append-01.tar", O_RDWR);
	s_tsh_path file_path;
	s_posix_header pheader;

	file_path.tar_fd = FD_NOTDEF;
	strcpy(file_path.outside_path, "test/resources/current-test-tar-files/ln-normal-file-02");

	/* The file shouldn't be present in the tarball. */
	cester_assert_false(commons_tar_find_file_from(tar_fd, "ln-normal-file-02", &pheader));

	const return_handler rh = commons_tar_append_file(tar_fd, file_path, NULL);

	cester_assert_equal(OK, rh.verdict);

	lseek(tar_fd, 0, SEEK_SET);
	/* The file should be present in the tarball. */
	cester_assert_true(commons_tar_find_file_from(tar_fd, "ln-normal-file-02", &pheader));

	close(tar_fd);
)

CESTER_TEST(AppendFile_should_copy_properly_from_a_normal_dir, test_instance,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-append-01.tar", O_RDWR);
	s_tsh_path file_path;
	s_posix_header pheader;

	file_path.tar_fd = FD_NOTDEF;
	strcpy(file_path.outside_path, "test/resources/current-test-tar-files/normal-dir");

	/* The file shouldn't be present in the tarball. */
	cester_assert_false(commons_tar_find_file_from(tar_fd, "normal-dir", &pheader));

	const return_handler rh = commons_tar_append_file(tar_fd, file_path, NULL);

	cester_assert_equal(OK, rh.verdict);

	lseek(tar_fd, 0, SEEK_SET);
	/* The file should be present in the tarball. */
	cester_assert_true(commons_tar_find_file_from(tar_fd, "normal-dir", &pheader));

	close(tar_fd);
)

CESTER_TEST(AppendFile_should_not_copy_properly_from_a_non_valid_normal_file, test_instance,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-append-01.tar", O_RDWR);
	s_tsh_path file_path;
	s_posix_header pheader;

	file_path.tar_fd = FD_NOTDEF;
	strcpy(file_path.outside_path, "test/resources/current-test-tar-files/non-existing-file");

	const return_handler rh = commons_tar_append_file(tar_fd, file_path, NULL);

	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal(
		"commons_tar_append_file: 'test/resources/current-test-tar-files/non-existing-file': "
		"No such file or directory\n", 
		rh.message
	);

	close(tar_fd);
)

CESTER_TEST(AppendFile_should_not_copy_properly_from_a_non_valid_tar_file, test_instance,
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-append-01.tar", O_RDWR);
	s_tsh_path file_path;
	s_posix_header pheader;

	file_path.tar_fd = open("test/resources/current-test-tar-files/test-tar-14", O_RDWR);
	strcpy(file_path.outside_path, "test/resources/current-test-tar-files/test-tar-14");
	strcpy(file_path.inside_path, "folder1/non-existing-file");

	const return_handler rh = commons_tar_append_file(tar_fd, file_path, NULL);

	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal(
		"commons_tar_append_file: 'folder1/non-existing-file': No such file or directory\n", rh.message
	);

	close(file_path.tar_fd);
	close(tar_fd);
)
