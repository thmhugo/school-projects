#include "inc/cester.h"
#include "inc/commons_test.h"
#include "inc/test_mkdirtar_aux.h"

CESTER_TEST(__________________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________MKDIRTAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(___________________________________________, _, cester_assert_nothing();)

CESTER_TEST(Mkdirtar_should_create_a_folder_in_the_tarball_root, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-mkdirtar-01.tar";
    s_posix_header pheader;
    char uid[8];
    char gid[8];
    char uname[32];
    char gname[32]; 
    const int tar_fd = open(path, O_RDWR);
    test_mkdirtar_init_local_var(uid, gid, uname, gname);
    const return_handler hd = mkdirtar(tar_fd, "folder1/");
    
    cester_assert_equal(OK, hd.verdict);
    cester_assert_true(ERR != commons_tar_find_file_from(tar_fd, "folder1/", &pheader));
    cester_assert_str_equal("folder1/", pheader.name);
    cester_assert_str_equal("0000755", pheader.mode);
    cester_assert_str_equal(uid, pheader.uid);
    cester_assert_str_equal(gid, pheader.gid);
    cester_assert_str_equal(TAR_DEFAULT_DIR_SIZE, pheader.size);
    cester_assert_equal(TRUE, tar_check_checksum(&pheader));
    cester_assert_equal(TAR_DIR_F, pheader.typeflag);
    cester_assert_str_equal("", pheader.linkname);
    cester_assert_str_equal(uname, pheader.uname);
    cester_assert_str_equal(gname, pheader.gname);

    close(tar_fd);
)

CESTER_TEST(Mkdirtar_should_create_a_folder_in_another_folder, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-mkdirtar-01.tar";
    s_posix_header pheader;
    char uid[8];
    char gid[8];
    char uname[32];
    char gname[32]; 
    const int tar_fd = open(path, O_RDWR);
    test_mkdirtar_init_local_var(uid, gid, uname, gname);
    const return_handler hd = mkdirtar(tar_fd, "folder2/folder4/");
    
    cester_assert_equal(OK, hd.verdict);
    cester_assert_true(ERR != commons_tar_find_file_from(tar_fd, "folder2/folder4/", &pheader));
    cester_assert_str_equal("folder2/folder4/", pheader.name);
    cester_assert_str_equal("0000755", pheader.mode);
    cester_assert_str_equal(uid, pheader.uid);
    cester_assert_str_equal(gid, pheader.gid);
    cester_assert_str_equal(TAR_DEFAULT_DIR_SIZE, pheader.size);
    cester_assert_equal(TRUE, tar_check_checksum(&pheader));
    cester_assert_equal(TAR_DIR_F, pheader.typeflag);
    cester_assert_str_equal("", pheader.linkname);
    cester_assert_str_equal(uname, pheader.uname);
    cester_assert_str_equal(gname, pheader.gname);

    close(tar_fd);
)

CESTER_TEST(Mkdirtar_should_not_create_an_existing_folder, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-mkdirtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler hd = mkdirtar(tar_fd, "folder2/");
    
    cester_assert_equal(ERR, hd.verdict);
    cester_assert_str_equal(
        "mkdir: cannot create directory 'folder2/': File exists\n", hd.message
    );

    close(tar_fd);
)

CESTER_TEST(Mkdirtar_should_not_create_an_existing_folder_freshly_created, _,
    const char *path = "test/resources/current-test-tar-files/test-tar-mkdirtar-01.tar";
    s_posix_header pheader;
    const int tar_fd = open(path, O_RDWR);
    const return_handler hd = mkdirtar(tar_fd, "folder1/");
    
    cester_assert_equal(ERR, hd.verdict);
    cester_assert_str_equal(
        "mkdir: cannot create directory 'folder1/': File exists\n", hd.message
    );

    close(tar_fd);
)

