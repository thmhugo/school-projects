#ifndef RMTAR_H
#define RMTAR_H

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
 *  Removes the corresponding file in the tarball.
 *  If the '-r' option is specified ('with_option' == TRUE)
 *  and the file is a directory, it removes all the files in the targetted dir.
 *
 * @param tar_fd is the file descriptor corresponding to the tarball.
 * @param with_option is equal to TRUE if the '-r' option is wanted else FALSE
 * @param path_from_tar_root is the searched file in the tarball.
 *
 * @return a return_handler with the following verdict value:
 * 			ERR in these cases:
 *          	the file does not exist
 *          	the file is folder and with_option is equal to FALSE
 *         	else OK.
 */
return_handler rmtar(const int tar_fd, const int with_option, const char *path_from_tar_root);


/**
 * Calls with right arguments and format all cmd->args.
 *
 * @param curr_state is the current tsh state.
 * @param cmd is the s_command to execute.
 *
 * @return a return_handler from rmtar.
 */
return_handler rmtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd);

#endif
