#include "inc/test_commons_tsh_aux.h"

void test_commons_tsh_init(char *cwd, s_tsh_state *tsh_state) {
	getcwd(cwd, PATH_MAX);

	strcat(cwd, "/");
	assert(NULL != (tsh_state->cwd = malloc(sizeof(s_tsh_path))));
	strcpy(tsh_state->cwd->outside_path, cwd);
	memset(tsh_state->cwd->inside_path, NONECHAR, PATH_MAX);
	tsh_state->cwd->tar_fd = FD_NOTDEF;
}

void test_commons_tsh_init2(char *cwd, s_tsh_state *tsh_state) {
	int tar_fd = open("test/resources/current-test-tar-files/test-tar-01", O_RDONLY);

	chdir("test/resources/current-test-tar-files/test-tar-01");

	assert(ERR != tar_fd);

	getcwd(cwd, PATH_MAX);

	strcat(cwd, "/test/resources/current-test-tar-files/test-tar-01/");
	assert(NULL != (tsh_state->cwd = malloc(sizeof(s_tsh_path))));
	strcpy(tsh_state->cwd->outside_path, cwd);
	strcpy(tsh_state->cwd->inside_path, "doc/");
	tsh_state->cwd->tar_fd = tar_fd;
}

void test_commons_tsh_clean(s_tsh_state *tsh_state) {
	if (0 < tsh_state->cwd->tar_fd) {
		close(tsh_state->cwd->tar_fd);
	}
}

s_tsh_state *test_commons_tsh_create_state_from_cwd(void) {
	char cwd[PATH_MAX];

	getcwd(cwd, PATH_MAX);
	strcat(cwd, PATH_DELIMITER_STR);

	return commons_tsh_create_state_from_outside_path(cwd);
}
