#ifndef CPTAR_H
#define CPTAR_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include "tar.h"
#include "commons_tar.h"
#include "commons_tsh.h"
#include "mkdirtar.h"

/**
 * cp implementation for the tarballs.
 *
 * @param source_path is the s_tsh_path to the source file to copy.
 * @param destination_path is the s_tsh_path to the destination file
 * @param with_option specify if the option "-r" is needed.
 *
 * @return a return_handler with the following verdict value:
 *        		ERR in these cases:
 * 					the source file does not exist
 * 					a writing/reading error occured
 *   			else OK.
 */
return_handler cptar(
    const s_tsh_path source_path,
    const s_tsh_path destination_path,
    const int with_option
);

/**
 * Calls with right arguments and format all cmd->args.
 *
 * @param curr_state is the current tsh state.
 * @param cmd is the s_command to execute.
 *
 * @return a return_handler from cptar.
 */
return_handler cptar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd);

#endif
