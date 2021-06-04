#ifndef CATTAR_H
#define CATTAR_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "commons_tar.h"
#include "commons_tsh.h"
#include "tar.h"

/**
 * Writes in stdout the contents of all the files given of a supposed valid tarball.
 *
 * @param tar_fd is the file descriptor corresponding to the tarball.
 * @param path_from_tar_root is the searched file in the tarball.
 *
 * @return a return_handler with the following verdict value: 
 * 			ERR in these cases:
 *                 	the file does not exist
 *                 	writing or reading error
 *         	else OK.
 */
return_handler cattar(const int tar_fd, const char *path_from_tar_root);

/**
 * Calls with right arguments and format all cmd->args.
 *
 * @param curr_state is the current tsh state.
 * @param cmd is the s_command to execute.
 *
 * @return a return_handler from cattar.
 */
return_handler cattar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd);

#endif
