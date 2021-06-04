#ifndef MVTAR_H
#define MVTAR_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "commons_tar.h"
#include "commons_tsh.h"
#include "commons.h"
#include "cptar.h"
#include "rmtar.h"
#include "tar.h"

/**
 *  Moves the corresponding file in the corresponding location.
 *
 * @param source_path is the path of the file to be moved.
 * @param destination_path is the path of the destination.
 *
 * @return a return_handler with following values:
* 				ERR in these cases:     
* 			 		the source file does not exist.
* 					writing or reading errors.
* 		   		else OK.
 */
return_handler mvtar(const s_tsh_path source_path, const s_tsh_path destination_path);

/**
 * Calls with right arguments and format all cmd->args.
 *
 * @param curr_state is the current tsh state.
 * @param cmd is the s_command to execute.
 *
 * @return a return_handler from mvtar.
 */
return_handler mvtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd);

#endif
