#ifndef RMDIRTAR_H
#define RMDIRTAR_H

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
 * Removes the corresponding empty folder in the tarball.
 *
 * @param tar_fd is the file descriptor corresponding to the tarball.
 * @param path_from_tar_root is the searched file in the tarball.
 *
 * @return a return_handler with the following verdict value:  
 *			ERR in these cases:
 *             		the file does not exist
 *             		the file is not a folder
 *             		the folder is not empty
 *  		else OK.
 */
return_handler rmdirtar(const int tar_fd, const char *path_from_tar_root);

/**
 * Calls with right arguments and format all cmd->args.
 *
 * @param curr_state is the current tsh state.
 * @param cmd is the s_command to execute.
 *
 * @return a return_handler from rmdirtar.
 */
return_handler rmdirtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd);

#endif
