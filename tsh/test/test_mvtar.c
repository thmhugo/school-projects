#include "inc/cester.h"
#include "inc/commons_test.h"
#include "../src/inc/commons_tsh.h"
#include "../src/inc/mvtar.h"

CESTER_TEST(______________________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________MVTAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(_______________________________________________, _, cester_assert_nothing();)

CESTER_TEST(Mvtar_should_move_a_normal_file_in_the_same_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-01.tar";
	const char *source_name = "file1";
	const char *destination_name = "folder2/folder3/file_t0";
	const int tar_fd = open(path, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path, destination_name, tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);
	
    cester_assert_equal(OK, rh.verdict);
	cester_assert_true(commons_tar_find_file_from(tar_fd, destination_name, &pheader));
	cester_assert_false(commons_tar_find_file_from(tar_fd, source_name, &pheader));

	close(tar_fd);
)

CESTER_TEST(Mvtar_should_move_a_normal_file_between_different_tarballs, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-01.tar";
	const char *path_target = "test/resources/current-test-tar-files/test-tar-mvtar-02.tar";
	const char *source_name = "folder2/folder3/file2";
	const char *destination_name = "folder1/file_t1";
	const int tar_fd = open(path, O_RDWR);
	const int target_tar_fd = open(path_target, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, destination_name, target_tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);
	
    cester_assert_equal(OK, rh.verdict);
	cester_assert_true(commons_tar_find_file_from(target_tar_fd, destination_name, &pheader));
	cester_assert_false(commons_tar_find_file_from(tar_fd, source_name, &pheader));

	close(tar_fd);
	close(target_tar_fd);
)

CESTER_TEST(Mvtar_should_move_a_normal_file_from_outside_to_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-mvtar/file1";
	const char *path_target = "test/resources/current-test-tar-files/test-tar-mvtar-02.tar";
	const char *destination_name = "folder1/file_t2";
	const int tar_fd = open(path_target, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, "", FD_NOTDEF);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, destination_name, tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);
	
    cester_assert_equal(OK, rh.verdict);
	cester_assert_true(commons_tar_find_file_from(tar_fd, destination_name, &pheader));
	cester_assert_equal(
		ERR, open("test/resources/current-test-tar-files/test-mvtar/file1", O_RDONLY)
	);

	close(tar_fd);
)

CESTER_TEST(Mvtar_should_move_a_normal_file_from_tarball_to_outside, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-02.tar";
	const char *path_target = "test/resources/current-test-tar-files/test-mvtar/file_t3";
	const char *source_name = "normal-file-01";
	const int tar_fd = open(path, O_RDWR);
	int tmp_fd;
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, "", FD_NOTDEF);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);
	
    cester_assert_equal(OK, rh.verdict);
	cester_assert_false(commons_tar_find_file_from(tar_fd, source_name, &pheader));
	cester_assert_not_equal(
		ERR, (tmp_fd = open("test/resources/current-test-tar-files/test-mvtar/file_t3", O_RDONLY))
	);

	close(tmp_fd);
	close(tar_fd);
)

CESTER_TEST(Mvtar_should_move_a_filled_dir_from_tarball_to_outside, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-03.tar";
	const char *path_target = "test/resources/current-test-tar-files/test-mvtar/folder_t4/";
	const char *source_name = "folder/folder2/";
	const int tar_fd = open(path, O_RDWR);
	DIR* tmp_dirp;
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, "", FD_NOTDEF);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);
	
    cester_assert_equal(OK, rh.verdict);
	cester_assert_false(commons_tar_find_file_from(tar_fd, source_name, &pheader));
	cester_assert_not_equal(
		NULL, (tmp_dirp = opendir("test/resources/current-test-tar-files/test-mvtar/folder_t4"))
	);

	closedir(tmp_dirp);
	close(tar_fd);
)

CESTER_TEST(Mvtar_should_move_a_filled_dir_from_outside_to_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-mvtar/folder2/";
	const char *path_target = "test/resources/current-test-tar-files/test-tar-mvtar-01.tar";
	const char *destination_name = "folder_t5/";
	const int tar_fd = open(path_target, O_RDWR);
	DIR* tmp_dirp;
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, "", FD_NOTDEF);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, destination_name, tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);
	
    cester_assert_equal(OK, rh.verdict);
	cester_assert_true(commons_tar_find_file_from(tar_fd, destination_name, &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t5/folder3/", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t5/folder3/file2", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t5/folder3/file3", &pheader));

	cester_assert_equal(
		NULL, (tmp_dirp = opendir("test/resources/current-test-tar-files/test-mvtar/folder2/"))
	);

	closedir(tmp_dirp);
	close(tar_fd);
)

CESTER_TEST(Mvtar_should_move_a_filled_dir_between_different_tarballs, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-03.tar";
	const char *path_target = "test/resources/current-test-tar-files/test-tar-mvtar-02.tar";
	const char *source_name = "folder/folder1/";
	const char *destination_name = "folder_t6/";
	const int tar_fd = open(path, O_RDWR);
	const int target_tar_fd = open(path_target, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, destination_name, target_tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);
	
    cester_assert_equal(OK, rh.verdict);
	cester_assert_true(commons_tar_find_file_from(target_tar_fd, destination_name, &pheader));
	cester_assert_true(commons_tar_find_file_from(target_tar_fd, "folder_t6/folder4/", &pheader));
	cester_assert_true(commons_tar_find_file_from(target_tar_fd, "folder_t6/folder5/", &pheader));
	cester_assert_true(commons_tar_find_file_from(target_tar_fd, "folder_t6/folder5/file1", &pheader));

	cester_assert_false(commons_tar_find_file_from(tar_fd, source_name, &pheader));

	close(tar_fd);
	close(target_tar_fd);
)

CESTER_TEST(Mvtar_should_move_a_filled_dir_in_the_same_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-04.tar";
	const char *source_name = "folder/";
	const char *destination_name = "folder_t7/";
	const int tar_fd = open(path, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path, destination_name, tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);

    cester_assert_equal(OK, rh.verdict);
	cester_assert_true(commons_tar_find_file_from(tar_fd, destination_name, &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder1/", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder1/folder4/", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder1/folder5/", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder1/folder5/file1", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder2/", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder2/folder3/", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder2/folder6/", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder2/folder6/file3", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "folder_t7/folder2/folder6/file2", &pheader));
	cester_assert_true(commons_tar_find_file_from(tar_fd, "file4", &pheader));
	cester_assert_false(commons_tar_find_file_from(tar_fd, source_name, &pheader));

	close(tar_fd);
)

CESTER_TEST(Mvtar_should_not_move_a_non_existing_file_in_the_same_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-04.tar";
	const char *source_name = "folder_0000/";
	const char *destination_name = "folder_t8/";
	const int tar_fd = open(path, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path, destination_name, tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal(
		"mv: cannot stat 'folder_0000/': No such file or directory\n", rh.message
	);
	cester_assert_false(commons_tar_find_file_from(tar_fd, destination_name, &pheader));

	close(tar_fd);
)

CESTER_TEST(Mvtar_should_not_move_a_non_existing_file_in_different_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-01.tar";
	const char *path_target = "test/resources/current-test-tar-files/test-tar-mvtar-04.tar";
	const char *source_name = "folder_0000/";
	const char *destination_name = "folder_t9/";
	const int tar_fd = open(path, O_RDWR);
	const int target_tar_fd = open(path_target, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, destination_name, target_tar_fd);
	const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal(
		"mv: cannot stat 'folder_0000/': No such file or directory\n", rh.message
	);
	cester_assert_false(commons_tar_find_file_from(target_tar_fd, destination_name, &pheader));

	close(tar_fd);
	close(target_tar_fd);
)

CESTER_TEST(Mvtar_should_not_move_a_non_existing_file_from_outside_to_tarball, _,
	const char *path = "test/resources/current-test-tar-files/test-mvtar/folder_0000/";
	const char *path_target = "test/resources/current-test-tar-files/test-tar-mvtar-04.tar";
	const char *destination_name = "folder_t10/";
	const int tar_fd = open(path_target, O_RDWR);
	s_posix_header pheader;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, "", FD_NOTDEF);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, destination_name, tar_fd);
    const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal(
		"mv: cannot stat "
		"'test/resources/current-test-tar-files/test-mvtar/folder_0000/': "
		"No such file or directory\n",
		rh.message
	);
	cester_assert_false(commons_tar_find_file_from(tar_fd, destination_name, &pheader));

	close(tar_fd);
)

CESTER_TEST(Mvtar_should_not_move_a_non_existing_file_from_tarball_to_outside, _,
	const char *path = "test/resources/current-test-tar-files/test-tar-mvtar-02.tar";
	const char *path_target = "test/resources/current-test-tar-files/test-mvtar/folder_t11/";
	const char *source_name = "folder_0000/";
	const int tar_fd = open(path, O_RDWR);
	s_posix_header pheader;
	DIR* tmp_dirp;
	s_tsh_path *source_tsh_path = commons_tsh_create_s_tsh_path(path, source_name, tar_fd);
	s_tsh_path *dest_tsh_path = commons_tsh_create_s_tsh_path(path_target, "", FD_NOTDEF);
    const return_handler rh = mvtar(*source_tsh_path, *dest_tsh_path);

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal(
		"mv: cannot stat 'folder_0000/': No such file or directory\n", rh.message
	);
	cester_assert_equal(
		NULL, (tmp_dirp = opendir("test/resources/current-test-tar-files/test-mvtar/folder_t11/"))
	);

	closedir(tmp_dirp);
	close(tar_fd);
)