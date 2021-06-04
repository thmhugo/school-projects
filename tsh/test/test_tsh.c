#include "inc/cester.h"

#include "../src/inc/commons_tsh.h"
#include "inc/test_commons_tsh_aux.h"

CESTER_TEST(______________________, _, cester_assert_nothing();)
CESTER_TEST(______________TSH_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(_________________, _, cester_assert_nothing();)

/**
 * Bug#1 (in tarball) :
 * > cd test/resources/backup-test-tar-files
 * > cd test-tar-bug1.tar
 * > ls src
 * 	=> not valid tar path.
 */
CESTER_TEST(Bug1_with_a_specific_functions_calls_combinaison, _,
	char cwd[PATH_MAX];
	s_tsh_state state;
	s_tsh_path *return_path;
	int fd = open("test/resources/current-test-tar-files/tmp-test", O_RDWR);

	redirection_redirect_stdout_to(fd);

	// Inits the current state.
	getcwd(cwd, PATH_MAX);
	strcat(cwd,"/test/resources/current-test-tar-files/");
	state.cwd = commons_tsh_create_s_tsh_path(cwd, "", FD_NOTDEF);

	// Simulates the cd into tarball.
	cd(&state, "test-tar-bug1.tar");

	strcat(cwd, "test-tar-bug1.tar/");
	cester_assert_str_equal(cwd, state.cwd->outside_path);

	// Simulates the separates_args call.
	cester_assert_true(commons_tsh_is_a_valid_tarball_path(&state, "src"));

	return_path = commons_tsh_update_path_from(&state, "src");
	cester_assert_not_null(return_path);
	cester_assert_str_equal("src/", return_path->inside_path);

	// Simulates the ls call.
	char *args[1] = { "src" };
	s_command *cmd = create_s_command("ls", 1, args, FALSE, FALSE, NULL);
	cester_assert_int_eq(OK, lstar_from_cmd(&state, cmd).verdict);

	redirection_reset_stdout();
	free(return_path);
	free(cmd);
)

/**
 * Bug#2 (not in tarball) : cp from of an outside file into inside path doesn't work.
 */
CESTER_TEST(Bug2_with_cptar_from_cmd_outside_to_tarball, _,
	char cwd[PATH_MAX];
	s_tsh_state state;
	s_posix_header pheader;
	int fd = open("test/resources/current-test-tar-files/test-tar-bug1.tar", O_RDWR);

	// Inits the current state.
	getcwd(cwd, PATH_MAX);
	strcat(cwd,"/test/resources/current-test-tar-files/");
	state.cwd = commons_tsh_create_s_tsh_path(cwd, "", FD_NOTDEF);

	cester_assert_false(commons_tar_find_file_from(fd, "normal-file-02-cpy", &pheader));

	// Simulates the cp call.
	char *args[2] = { "normal-file-02", "test-tar-bug1.tar/normal-file-02-cpy" };
	s_command *cmd = create_s_command("cp", 2, args, FALSE, FALSE, NULL);
	cester_assert_equal(OK, cptar_from_cmd(&state, cmd).verdict);

	cester_assert_true(commons_tar_find_file_from(fd, "normal-file-02-cpy", &pheader));

	close(fd);
	free(cmd);
)

/**
 * Bug#2 (not in tarball) : cp from tarball to the same tarball return ERR.
 */
CESTER_TEST(Bug3_with_cptar_from_cmd_tarball_to_tarball, _,
	char cwd[PATH_MAX];
	s_tsh_state state;
	s_posix_header pheader;
	int fd = open("test/resources/current-test-tar-files/test-tar-bug1.tar", O_RDWR);

	// Inits the current state.
	getcwd(cwd, PATH_MAX);
	strcat(cwd,"/test/resources/current-test-tar-files/test-tar-bug1.tar");
	state.cwd = commons_tsh_create_s_tsh_path(cwd, "", fd);

	cester_assert_false(commons_tar_find_file_from(fd, "cdcpy", &pheader));

	// Simulates the cp call.
	char *args[2] = { "src/cd.c", "cdcpy" };
	s_command *cmd = create_s_command("cp", 2, args, FALSE, FALSE, NULL);
	cester_assert_equal(OK, cptar_from_cmd(&state, cmd).verdict);

	cester_assert_true(commons_tar_find_file_from(fd, "cdcpy", &pheader));

	close(fd);
	free(cmd);
)
