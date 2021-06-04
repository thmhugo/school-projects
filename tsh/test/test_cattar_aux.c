#include "inc/test_cattar_aux.h"

int test_cattar_aux_is_returning_as_expected(
	const char *path_to_src,
	const char *file_path,
	char 	   *buffer
) {
	ssize_t current_line_size;
	int fd_src;

	test_init(path_to_src, "test/resources/current-test-tar-files/tmp-cattar-test", &fd_src);

	if (ERR == (cattar(fd_src, file_path)).verdict) {
		return FALSE;
	}

	current_line_size = read(STDOUT_FILENO, buffer, COMMONS_TEST_MAX_LINE_SIZE);

	assert(ERR != current_line_size);

	test_clean(&fd_src);

	return TRUE;
}

