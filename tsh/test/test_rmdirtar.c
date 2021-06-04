#include "inc/cester.h"
#include "inc/commons_test.h"
#include "../src/inc/rmdirtar.h"

CESTER_TEST(________________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________RMDIRTAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(_________________________________________, _, cester_assert_nothing();)

CESTER_TEST(Rmdirtar_should_remove_empty_folder, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmdirtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmdirtar(tar_fd, "folder1/");

    cester_assert_equal(OK, rh.verdict);
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder1/", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmdirtar_should_not_remove_not_empty_folder, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmdirtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmdirtar(tar_fd, "folder3/");

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal("rmdir: failed to remove 'folder3/': Directory not empty\n", rh.message);
    cester_assert_true(commons_tar_find_file_from(tar_fd, "folder3/", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmdirtar_should_not_remove_normal_file, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmdirtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmdirtar(tar_fd, "file1");

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal("rmdir: failed to remove 'file1': Not a directory\n", rh.message);
    cester_assert_true(commons_tar_find_file_from(tar_fd, "file1", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmdirtar_should_not_remove_not_existing_file, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmdirtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmdirtar(tar_fd, "folder6/");

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal("rmdir: failed to remove 'folder6/': No such file or directory\n", rh.message);

    close(tar_fd);
)
