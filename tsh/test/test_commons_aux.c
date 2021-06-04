#include "inc/commons_test.h"

static int fd_stdout_saved;
static int fd_tmp;

void test_init(const char *path_to_src, const char *path_to_tmp, int *fd_src) {
	*fd_src = open(path_to_src, O_RDONLY);
	fd_tmp = open(path_to_tmp, O_RDWR | O_TRUNC);

	assert(OK == redirection_redirect_stdout_to(fd_tmp));
}

void test_clean(int *fd_src) {
	close(*fd_src);
	assert(OK == redirection_reset_stdout());
}

void test_blockmove_get_src_and_dest_offset(
		const int tar_fd,
		const s_posix_header *pheader,
		ssize_t *src_offset,
		ssize_t *dest_offset
) {
	/* `dest_offset` = the beggining of the 'doc/guideline.md' header. */
	*dest_offset = lseek(tar_fd, -TAR_BLOCKSIZE, SEEK_CUR);

	/* Puts the offset to the the beggining of the 'doc/guideline.md' content. */
	lseek(tar_fd, TAR_BLOCKSIZE, SEEK_CUR);
	commons_tar_lseek_size_of_file(tar_fd, pheader);

	/* `src_offset` = the beggining of the next header. */
	*src_offset = lseek(tar_fd, 0, SEEK_CUR);
}
