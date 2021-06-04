#include "inc/cester.h"
#include "inc/commons_test.h"
#include "../src/inc/rmtar.h"

CESTER_TEST(____________________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________RMTAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(_____________________________________________, _, cester_assert_nothing();)

CESTER_TEST(Rmtar_should_remove_a_normal_file, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, FALSE, "file1");

    cester_assert_equal(OK, rh.verdict);
    cester_assert_false(commons_tar_find_file_from(tar_fd, "file1", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmtar_with_option_should_remove_recursively_all_files_of_a_dir, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, TRUE, "folder2/folder3/");

    cester_assert_equal(OK, rh.verdict);
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder2/folder3/", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder2/folder3/file2", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder2/folder3/file3", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmtar_with_option_should_remove_recursively_all_files_and_subdir_of_a_dir, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-02.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, TRUE, "folder/");

    cester_assert_equal(OK, rh.verdict);
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder/folder1/folder4", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder/folder1/folder5", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder/folder1/folder5/file1", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder/folder2/folder3", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder/folder2/folder6", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder/folder2/folder6/file2", &pheader));
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder/folder2/folder6/file3", &pheader));
    cester_assert_true(commons_tar_find_file_from(tar_fd, "file4", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmtar_with_option_should_remove_an_empty_dir, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, TRUE, "folder1/");

    cester_assert_equal(OK, rh.verdict);
    cester_assert_false(commons_tar_find_file_from(tar_fd, "folder1/", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmtar_should_remove_a_symbolic_link_file, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, TRUE, "symbolicfile4");

    cester_assert_equal(OK, rh.verdict);
    cester_assert_false(commons_tar_find_file_from(tar_fd, "symbolicfile4", &pheader));

    close(tar_fd);
)

CESTER_TEST(Rmtar_should_not_remove_a_non_existing_file, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, FALSE, "file500");

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal("rm: cannot remove 'file500': No such file or directory\n", rh.message);

    close(tar_fd);
)

CESTER_TEST(Rmtar_with_option_should_not_remove_a_non_existing_file, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, TRUE, "file500");

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal("rm: cannot remove 'file500': No such file or directory\n", rh.message);

    close(tar_fd);
)

CESTER_TEST(Rmtar_without_option_should_not_remove_dir, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-rmtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler rh = rmtar(tar_fd, FALSE, "folder2/");

    cester_assert_equal(ERR, rh.verdict);
    cester_assert_str_equal("rm: cannot remove 'folder2/': Is a directory\n", rh.message);
    cester_assert_true(commons_tar_find_file_from(tar_fd, "folder2/", &pheader));

    close(tar_fd);
)
