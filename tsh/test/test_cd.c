#include "inc/cester.h"
#include "inc/test_commons_tsh_aux.h"

#include "../src/inc/cd.h"

CESTER_TEST(_______________________, _, cester_assert_nothing();)
CESTER_TEST(______________CD_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(________________________, _, cester_assert_nothing();)

CESTER_TEST(Cd_with_a_valid_simple_path, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "src/";

	test_commons_tsh_init(cwd, &tsh_state);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	if (0 != strcmp(cwd, PATH_DELIMITER_STR)) {
		strcat(cwd, PATH_DELIMITER_STR);
	}
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_a_valid_path_with_parents_references, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "src/../test/resources/../../doc/png/";

	test_commons_tsh_init(cwd, &tsh_state);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	if (0 != strcmp(cwd, PATH_DELIMITER_STR)) {
		strcat(cwd, PATH_DELIMITER_STR);
	}
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_a_valid_path_with_parents_references_leading_to_a_parent_dir, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "src/../../../";

	test_commons_tsh_init(cwd, &tsh_state);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	if (0 != strcmp(cwd, PATH_DELIMITER_STR)) {
		strcat(cwd, PATH_DELIMITER_STR);
	}
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_a_valid_absolute_path_with_parents_references, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "/home/../etc/";

	test_commons_tsh_init(cwd, &tsh_state);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	if (0 != strcmp(cwd, PATH_DELIMITER_STR)) {
		strcat(cwd, PATH_DELIMITER_STR);
	}
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_an_absolute_path_starting_with_an_invalid_dir, _,
	s_tsh_state tsh_state;
	s_tsh_state tsh_state_cpy;
	char cwd[PATH_MAX];
	char *path = "/adsfa/../etc/";

	test_commons_tsh_init(cwd, &tsh_state);
	commons_tsh_copy_state(&tsh_state, &tsh_state_cpy);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(ERR, hd.verdict);
	cester_assert_str_equal("cd: '/adsfa/../etc/': No such file or directory\n", hd.message);
	cester_assert_str_equal(tsh_state_cpy.cwd->outside_path, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_an_absolute_path_leading_to_an_invalid_dir, _,
	s_tsh_state tsh_state;
	s_tsh_state tsh_state_cpy;
	char cwd[PATH_MAX];
	char *path = "/home/../asdfa/";

	test_commons_tsh_init(cwd, &tsh_state);
	commons_tsh_copy_state(&tsh_state, &tsh_state_cpy);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(ERR, hd.verdict);
	cester_assert_str_equal("cd: '/home/../asdfa/': No such file or directory\n", hd.message);
	cester_assert_str_equal(tsh_state_cpy.cwd->outside_path, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_a_relative_path_leading_to_valid_tar_file, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/../test/resources/current-test-tar-files/test-tar-01/doc";
	char *expected_path_from_tar_root = "doc/";

	test_commons_tsh_init(cwd, &tsh_state);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	strcat(cwd, "/test-tar-01/");
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_str_equal(expected_path_from_tar_root, tsh_state.cwd->inside_path);
	cester_assert_true(0 < tsh_state.cwd->tar_fd);

	close(tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_a_relative_path_leading_to_valid_tar_file_with_parents_ref, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/../test/resources/current-test-tar-files/test-tar-01/doc/../doc/png/";
	char *expected_path_from_tar_root = "doc/png/";

	test_commons_tsh_init(cwd, &tsh_state);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	strcat(cwd, "/test-tar-01/");
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_str_equal(expected_path_from_tar_root, tsh_state.cwd->inside_path);
	cester_assert_true(0 < tsh_state.cwd->tar_fd);

	close(tsh_state.cwd->tar_fd);
)

CESTER_TEST(Cd_with_a_relative_path_from_a_tar_file_to_a_valid_file_with_parents_ref, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "png/../../../";

	assert(NULL != cwd);
	test_commons_tsh_init2(cwd, &tsh_state);
	assert(ERR != chdir("test/resources/current-test-tar-files/"));
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	if (0 != strcmp(cwd, PATH_DELIMITER_STR)) {
		strcat(cwd, PATH_DELIMITER_STR);
	}
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);

	test_commons_tsh_clean(&tsh_state);
)

CESTER_TEST(Cd_with_a_relative_path_leading_to_a_valid_tar_file_with_parents_ref_and_other_valid_tar_file, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path =
		"test/resources/current-test-tar-files/test-tar-01/doc/../../test-tar-04/dir1/";
	char *expected_path_from_tar_root = "dir1/";
	int tar_fd = FD_NOTDEF;

	assert(NULL != cwd);

	test_commons_tsh_init(cwd, &tsh_state);
	const return_handler hd = cd(&tsh_state, path);

	cester_assert_equal(OK, hd.verdict);
	getcwd(cwd, PATH_MAX);
	strcat(cwd, "/test-tar-04/");
	cester_assert_str_equal(expected_path_from_tar_root, tsh_state.cwd->inside_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_true(0 < tsh_state.cwd->tar_fd);

	close(tsh_state.cwd->tar_fd);
)
