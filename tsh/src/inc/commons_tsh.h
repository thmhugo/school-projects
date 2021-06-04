#ifndef COMMONS_TSH_H
#define COMMONS_TSH_H

#include <assert.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <string.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "commons.h"
#include "commons_tar.h"
#include "tar.h"
#include "tsh.h"

/**
 * Modifies the tsh_state according the given path while it's correct.
 *
 * At the end, if all the directories in the path are correct :
 *
 * 	- tsh_state.current_pwd :
 * 		is the new current_pwd from the root.
 *
 * 	- tsh_state.current_path_from_tar_root :
 * 		if the path leads to a tarball, is the the new path from the tar root,
 * 		else, "".
 *
 * 	- tsh_state.tar_fd :
 * 		if the path leads to a tarball, is the the new fd of the tarball
 * 		else, FD_NOTDEF.
 *
 * @param tsh_state is the current tsh state.
 * @param path 		is the path to simplify.
 *
 * @return ERR 				If the path is not valid.
 * 		   FD_NOTDEF 		If the path leads to a folder that is not inside a tarball.
 * 		   the tarball fd 	If the path leads to a folder inside of this tarball.
 */
int commons_tsh_update_state_from_path(s_tsh_state *tsh_state, char *path);

/**
 * Compares a s_tsh_path to a string path representation.
 *
 * @see commons_tsh_get_path_from.
 *
 * @param tsh_path is the s_tsh_path to compare.
 * @param path is the string representation of the path to compare to.
 *
 * @return TRUE the s_tsh_path has the same string representation of path.
 */
int commons_tsh_path_is_equal_to(const s_tsh_path *tsh_path, const char *path);

/**
 * Copies tsh_state into tsh_state_cpy.
 *
 * @param tsh_state 	is the tsh_state src.
 * @param tsh_state_cpy is the tsh_state dest.
 */
void commons_tsh_copy_state(
	const s_tsh_state *tsh_state, s_tsh_state *tsh_state_cpy
);

/**
 * Copies tsh_path into tsh_path_cpy.
 *
 * @param src_path 	is the src.
 * @param dest_path is the dest.
 */
void commons_tsh_copy_path(
	const s_tsh_path *src_path, s_tsh_path *dest_path
);

/**
 * Creates a tsh_state with a given outside path.
 *
 * @param path is the given path.
 *
 * @return the new tsh_state.
 */
s_tsh_state *commons_tsh_create_state_from_outside_path(const char *path);

/**
 * Returns a s_tsh_path given its args.
 *
 * @param out is the outside_path.
 * @param in is the inside_path.
 * @param fd is the tar_fd.
 *
 * @return a s_tsh_path.
 */
s_tsh_path *commons_tsh_create_s_tsh_path(const char *out, const char *in, const int fd);

/**
 * Concatenates the outside and inside path of a tsh_path to get the path in one string.
 *
 * @param tsh_path	is the s_tsh_path.
 *
 * @return the path.
 */
char *commons_tsh_get_path_from(const s_tsh_path *tsh_path);

/**
 * Frees the given s_redirection list.
 *
 * @param head is the first element of the list.
 */
void commons_tsh_free_command_ll(s_command *head);

/**
 * Check if the `s_command` contains redirections or arguments contains valid tarball path.
 *
 * @param cmd is the s_command to check.
 * @param curr_state is the current s_tsh_state of the shell.
 *
 * @return TRUE if the commands contains valid tarball path, otherwise FALSE.
 */
int commons_tsh_is_tarball_involved(const s_tsh_state *curr_state, const s_command *cmd);

/**
 * Verifies if the given path leads to a file or directory inside a valid tarball
 * according to a current tsh state.
 *
 * @param curr_state is the current tsh state.
 * @param path is the path to check.
 *
 * @return TRUE if the path is valid, otherwise, FALSE.
 */
int commons_tsh_is_a_valid_tarball_path(const s_tsh_state *curr_state, const char *path);

/**
 * Returns a s_command given its args (see src/inc/parser.h).
 */
s_command *create_s_command(
    const char *cmd,
    const int nb_args,
    char *args[],
    const int with_option,
    const int tarball_option,
    s_command_redirections *redirections
);

/**
 * Creates a new string with the filename added to the parent_path.
 *
 * @param parent_path is the parent path of the wanted path.
 * @param filename is the name to append to the parent_path.
 *
 * @return the new full path.
 */
char *commons_tsh_create_full_path(const char *parent_path, const char *filename);

/**
 * Creates the s_tsh_path corresponding to the simplified string path from a tsh state.
 * If the path isn't valid tries with the parent path.
 *
 * @param path is the path to update from.
 * @param curr_state is the current s_tsh_state of the shell.
 *
 * @return the new tsh path if the path leads is valid, otherwise, NULL.
 */
s_tsh_path *commons_tsh_update_path_from(const s_tsh_state *curr_state, const char *path);

/**
 * Verifies in the given path if the first or the last file is a self dir "."
 *
 * @param path is the path to verify.
 * @param value specify if the test has to be done for the first dir or the last one.
 *
 * @return TRUE if the first or the last of path is a self dir, FALSE otherwise.
 */
int commons_tsh_is_path_self_dir(const char *path, const int value);

#endif
