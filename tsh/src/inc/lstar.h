#ifndef LSTAR_H
#define LSTAR_H

#include <assert.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "commons.h"
#include "commons_tar.h"
#include "commons_tsh.h"
#include "tar.h"
#include "uthash.h"

#define LSTAR_DEFAULT_DIR_SIZE 4096
#define LSTAR_DEFAULT_DIR_LEN  4
#define LSTAR_SPACE_CHAR ' '

#define LSTAR_UNKNOWN_MODE   "?????????"
#define LSTAR_UNKNOWN_MODE_S "???"
#define LSTAR_UNKNOWN_NAME   "?"
#define LSTAR_UNKNOWN_TIME   "??? ?? ??:??"

/**
 * Hash map entry for `g_lstar_hashmap`.
 *
 * @note We choosed to use an hash map in order to reduce the global complexity of `lstar`
 * by reading the entire tarball one time and then have a constant time to access to a header
 * instead of potentially reading the entire tarball for each file inside the aimed dir.
 * Moreover, using an hash map reduce the number of system calls.
 * O (n2) -> Θ (n).
 */
typedef struct {
	char            name[PATH_MAX]; ///< Is equal to `pheader->name` and is the entry key.
	s_posix_header *pheader;
	/**
	 * If the file is a directory,
	 * 		`nb_links` will correspond to its number of subdirectory.
	 * Else,
	 * 		`nb_links` will correspond to the number of physical link aiming to the file.
	 */
	int             nb_links;
	UT_hash_handle  hh;
} s_lstar_hm_item;

/**
 * Writes in stdout all the file names of a supposed valid tarball.
 * If the '-l' option is specified (`with_option == TRUE`),
 * write more informations about the files inside `path_from_tar_root`.
 *
 * @see https://www.garron.me/en/go2linux/ls-file-permissions.html
 *
 * @param tar_fd is the file descriptor corresponding to the tarball.
 * @param with_option is equal to TRUE (1) if the '-l' option is wanted else FALSE (0)
 * @param path_from_tar_root is the path to the directory inside a tarball.
 *
 * @return a return_handler with the following verdict value: ERR if the file does not exist and else OK.
 */
return_handler lstar(const int tar_fd, const int with_option, const char *path_from_tar_root);

/**
 * Calls with right arguments and format all cmd->args.
 *
 * @param curr_state is the current tsh state.
 * @param cmd is the s_command to execute.
 *
 * @return a return_handler from lstar.
 */
return_handler lstar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd);

#endif
