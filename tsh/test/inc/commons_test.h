#ifndef COMMONS_TEST_H
#define COMMONS_TEST_H

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../src/inc/commons.h"
#include "../../src/inc/commons_tar.h"
#include "../../src/inc/redirection.h"

#define COMMONS_TEST_RESOURCES_PATH "test/resources/current-test-tar-files/"
#define COMMONS_TEST_MAX_LINE_SIZE 5000

void test_init(const char *path_to_src, const char *path_to_tmp, int *fd_src);
void test_clean(int *fd_src);

/* Initializes the source offset with the beginning of the current header and the destination
 * offset with the beggining of the next header.
 *
 * @param tar_fd     is the file descriptor of a valid tarball.
 * @param src_offset is the source offset.
 * @param src_offset is the destination offset.
 */
void test_blockmove_get_src_and_dest_offset(
		const int tar_fd,
		const s_posix_header *pheader,
		ssize_t *src_offset,
		ssize_t *dest_offset
);

#endif
