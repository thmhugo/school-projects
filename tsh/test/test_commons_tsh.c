#include "inc/cester.h"

#include "../src/inc/commons_tsh.h"
#include "inc/test_commons_tsh_aux.h"

CESTER_TEST(_____________________________, _, cester_assert_nothing();)
CESTER_TEST(______________COMMONS_TSH_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(_________________________________, _, cester_assert_nothing();)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_another_valid_outside_path, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "src/";
	char *expected_path = "src/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	strcat(cwd, expected_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_from_a_valid_outside_path_to_an_invalid_outside_path_should_return_ERR, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "asf/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(ERR, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_another_valid_2_depth_outside_path, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "src/inc/";
	char *expected_path = "src/inc/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	strcat(cwd, expected_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_from_a_valid_outside_path_to_an_invalid_2_depth_outside_path_should_return_ERR, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "src/notexistingdir/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(ERR, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_another_not_simplified_path, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/../src/";
	char *expected_path = "src/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	strcat(cwd, expected_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_another_not_simplified_path_leading_to_a_parent_dir, _,
	s_tsh_state tsh_state;
	char *cwd = malloc(PATH_MAX);
	char *path = "test/../../";

	assert(NULL != cwd);

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	cwd = dirname(cwd);
	strcat(cwd, PATH_DELIMITER_STR);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
	free(cwd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_another_not_simplified_path_2, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/inc/../resources/../../";
	char *expected_path = "";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_from_a_valid_outside_path_to_an_invalid_not_simplified_path_should_return_ERR, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/inc/../asdf/../../";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(ERR, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_a_valid_tarball, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/inc/../resources/current-test-tar-files/test-tar-01/";
	char *expected_path = "test/resources/current-test-tar-files/test-tar-01/";
	int tar_fd = FD_NOTDEF;

	test_commons_tsh_init(cwd, &tsh_state);
	tar_fd = commons_tsh_update_state_from_path(&tsh_state, path);

	strcat(cwd, expected_path);
	cester_assert_true(0 < tar_fd);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_true(tar_fd == tsh_state.cwd->tar_fd);

	assert(ERR != close(tar_fd));
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_a_valid_dir_inside_a_tarball, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/inc/../resources/current-test-tar-files/test-tar-01/doc/png/";
	char *expected_path = "test/resources/current-test-tar-files/test-tar-01/";
	char *expected_path_from_tar_root = "doc/png/";
	int tar_fd = FD_NOTDEF;

	test_commons_tsh_init(cwd, &tsh_state);
	tar_fd = commons_tsh_update_state_from_path(&tsh_state, path);

	cester_assert_true(0 < tar_fd);
	strcat(cwd, expected_path);
	cester_assert_str_equal(expected_path_from_tar_root, tsh_state.cwd->inside_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_true(tar_fd == tsh_state.cwd->tar_fd);

	assert(ERR != close(tar_fd));
)

CESTER_TEST(UpdateStateFrom_from_a_valid_outside_path_to_an_invalid_dir_inside_a_tarball_should_return_ERR, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/inc/../resources/current-test-tar-files/test-tar-01/doc/pdwng/";
	char *expected_path = "test/resources/current-test-tar-files/test-tar-01/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(ERR, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_a_valid_not_simplified_path_dir_inside_a_tarball, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "test/inc/../resources/current-test-tar-files/test-tar-01/doc/../doc/png/../";
	char *expected_path = "test/resources/current-test-tar-files/test-tar-01/";
	char *expected_path_from_tar_root = "doc/";
	int tar_fd = FD_NOTDEF;

	test_commons_tsh_init(cwd, &tsh_state);
	tar_fd = commons_tsh_update_state_from_path(&tsh_state, path);

	cester_assert_true(0 < tar_fd);
	strcat(cwd, expected_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_str_equal(expected_path_from_tar_root, tsh_state.cwd->inside_path);
	cester_assert_true(tar_fd == tsh_state.cwd->tar_fd);

	assert(ERR != close(tar_fd));
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_an_absolute_path, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "/home/";
	char *expected_path = "/home/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(expected_path, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_an_absolute_not_simplified_path, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "/home/../bin/../tmp/";
	char *expected_path = "/tmp/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(expected_path, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_outside_path_to_an_absolute_not_simplified_path_leading_to_the_root, _,
	s_tsh_state tsh_state;
	char cwd[PATH_MAX];
	char *path = "/home/../bin/../tmp/../";
	char *expected_path = "/";

	test_commons_tsh_init(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	cester_assert_str_equal(expected_path, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_tarball_path_to_another_valid_dir_of_the_same_tarball, _,
	s_tsh_state tsh_state;
	char *cwd = malloc(PATH_MAX);
	char *path = "../";
	char *expected_path = "";
	int tar_fd = FD_NOTDEF;

	assert(NULL != cwd);

	test_commons_tsh_init2(cwd, &tsh_state);
	tar_fd = commons_tsh_update_state_from_path(&tsh_state, path);

	cester_assert_true(0 < tar_fd);
	cester_assert_str_equal(expected_path, tsh_state.cwd->inside_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_true(tar_fd == tsh_state.cwd->tar_fd);

	assert(ERR != close(tar_fd));
	free(cwd);

	test_commons_tsh_clean(&tsh_state);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_tarball_path_to_a_valid_outside_dir, _,
	s_tsh_state tsh_state;
	char *cwd = malloc(PATH_MAX);
	char *path = "png/../../../";

	assert(NULL != cwd);

	test_commons_tsh_init2(cwd, &tsh_state);

	cester_assert_equal(FD_NOTDEF, commons_tsh_update_state_from_path(&tsh_state, path));
	cwd = dirname(cwd);
	strcat(cwd, PATH_DELIMITER_STR);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_equal(NONECHAR, tsh_state.cwd->inside_path[0]);
	cester_assert_equal(FD_NOTDEF, tsh_state.cwd->tar_fd);

	test_commons_tsh_clean(&tsh_state);
	free(cwd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_tarball_path_to_another_valid_dir_of_another_tarball, _,
	s_tsh_state tsh_state;
	char *cwd = malloc(PATH_MAX);
	char *path =
		"test/resources/current-test-tar-files/test-tar-01/doc/../../test-tar-04/dir1/";
	char *expected_path = "test/resources/current-test-tar-files/test-tar-04/";
	char *expected_path_from_tar_root = "dir1/";
	int tar_fd = FD_NOTDEF;

	assert(NULL != cwd);

	test_commons_tsh_init(cwd, &tsh_state);
	tar_fd = commons_tsh_update_state_from_path(&tsh_state, path);

	strcat(cwd, expected_path);

	cester_assert_true(0 < tar_fd);
	cester_assert_str_equal(expected_path_from_tar_root, tsh_state.cwd->inside_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_true(tar_fd == tsh_state.cwd->tar_fd);

	assert(ERR != close(tar_fd));
	free(cwd);
)

CESTER_TEST(UpdateStateFrom_should_update_the_state_from_a_valid_tarball_path_to_another_valid_dir_of_the_same_tarball_with_a_not_simplified_path_going_outside, _,
	s_tsh_state tsh_state;
	char *cwd = malloc(PATH_MAX);
	char *path = "png/../../../test-tar-04/dir1/";
	char *expected_path_from_tar_root = "dir1/";
	int tar_fd = FD_NOTDEF;

	assert(NULL != cwd);

	test_commons_tsh_init2(cwd, &tsh_state);

	tar_fd = commons_tsh_update_state_from_path(&tsh_state, path);

	cwd = dirname(cwd);
	strcat(cwd, "/test-tar-04/");

	cester_assert_true(0 < tar_fd);
	cester_assert_str_equal(expected_path_from_tar_root, tsh_state.cwd->inside_path);
	cester_assert_str_equal(cwd, tsh_state.cwd->outside_path);
	cester_assert_true(tar_fd == tsh_state.cwd->tar_fd);

	assert(ERR != close(tar_fd));
	free(cwd);
)

CESTER_TEST(_______, _, cester_assert_nothing();)

CESTER_TEST(IsTarballInvolved_should_return_TRUE_with_a_tarball_path_in_argument, _,
	char *ls_args[1] = { "test/resources/current-test-tar-files/test-tar-01" };
	s_command *cmd = create_s_command("ls", 1, ls_args, FALSE, FALSE, NULL);
	s_tsh_state *state = test_commons_tsh_create_state_from_cwd();

	cester_assert_true(commons_tsh_is_tarball_involved(state, cmd));

	free(cmd);
	free(state);
)

CESTER_TEST(IsTarballInvolved_should_return_TRUE_with_a_tarball_path_in_redirection, _,
	char *ls_args[1] = { "path/to/file" };
	s_tsh_state *state = test_commons_tsh_create_state_from_cwd();

	/* Creates expected redirection. */
	s_redirection *expected_out_redirection =
		create_s_redirection(
			"test/resources/current-test-tar-files/test-tar-01/doc",
			O_TRUNC
	);

	s_command_redirections *cmd_redirections =
		create_s_command_redirections(
			NULL,
			expected_out_redirection,
			NULL
	);

	/* Creates the expected command. */
	s_command *cmd = create_s_command("ls", 1, ls_args, FALSE, FALSE, cmd_redirections);

	cester_assert_true(commons_tsh_is_tarball_involved(state, cmd));

	free(cmd);
	free(state);
	free(expected_out_redirection);
	free(cmd_redirections);
)

CESTER_TEST(IsTarballInvolved_should_return_FALSE_with_no_tarball_path_involved, _,
	char *ls_args[1] = { "test/resources" };
	s_tsh_state *state = test_commons_tsh_create_state_from_cwd();

	/* Creates expected redirection. */
	s_redirection *expected_out_redirection =
		create_s_redirection(
			"test/",
			O_TRUNC
	);

	s_command_redirections *cmd_redirections =
		create_s_command_redirections(
			NULL,
			expected_out_redirection,
			NULL
	);

	/* Creates the expected command. */
	s_command *cmd = create_s_command("ls", 1, ls_args, FALSE, FALSE, cmd_redirections);

	cester_assert_false(commons_tsh_is_tarball_involved(state, cmd));

	free(cmd);
	free(state);
)
