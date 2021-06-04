#ifndef CD_H
#define CD_H

#include "commons_tar.h"
#include "commons_tsh.h"
#include "tsh.h"
#include <errno.h>

/**
 * Updates the current tsh_state and the cwd according to the given path.
 *
 * @param tsh_state is the current tsh state to modify.
 * @param path is the path to the wanted dir.
 *
 * @return a return_handler with the following verdict value: 
 * 			ERR if the path does not exist else OK.
 */
return_handler cd(s_tsh_state *tsh_state, char *path);

#endif
