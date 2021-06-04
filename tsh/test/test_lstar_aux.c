#include "inc/test_lstar_aux.h"

int test_lstar_aux_is_returning_as_expected(
	const char *path_to_src,
	const char *path_from_tar_root,
	const int   with_option,
	char 		*buffer
) {
	ssize_t current_line_size;
	int fd_src;

	test_init(path_to_src, "test/resources/current-test-tar-files/tmp-test", &fd_src);

	if (ERR == lstar(fd_src, with_option, path_from_tar_root).verdict) {
		return FALSE;
	}

	lseek(STDOUT_FILENO, 0, SEEK_SET);

	current_line_size = read(STDOUT_FILENO, buffer, COMMONS_TEST_MAX_LINE_SIZE);

	assert(ERR != current_line_size);

	test_clean(&fd_src);

	return TRUE;
}
