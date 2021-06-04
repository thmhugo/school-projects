#ifndef MKDIRTAR_H
#define MKDIRTAR_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "commons_tar.h"
#include "commons_tsh.h"
#include "tar.h"


/**
 * Creates a new folder in the tarball.
 *
 * @param tar_fd is the file descriptor corresponding to the tarball.
 * @param path_from_tar_root is the file to be created in the tarball.
 *
 * @return a return_handler with a verdict with the following value:
* 				ERR in these cases:
 *             		a file with the same name is present
 *              	or writing errors.
 *  			else OK.
 */
return_handler mkdirtar(const int tar_fd, const char *path_from_tar_root);

/**
 * Calls with right arguments and format all cmd->args.
 *
 * @param curr_state is the current tsh state.
 * @param cmd is the s_command to execute.
 *
 * @return a return_handler from mkdirtar.
 */
return_handler mkdirtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd);

#endif
