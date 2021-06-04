/**
 * Contains the main function and functions related to the shell logic.
 */

#ifndef TSH_H
#define TSH_H

#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define TSH_DEFAULT_MODE 0644 ///< Is the default mode for new created files.

/**
 * Models a path in tsh.
 *
 * If the path doesn't involve tarball :
 *
 *  - `inside_path` is filled with NONECHARs
 *  - `tar_fd`      is equal to FD_NOTDEF
 */
typedef struct {
	char outside_path[PATH_MAX]; ///< Is the absolute path outside tarball.
	char inside_path[PATH_MAX];  ///< Is the absolute path from tarball root.
	int  tar_fd;                 ///< Is the file descriptor associated to the tarball.
} s_tsh_path;

/** Associates a redirection type to an open type. */
typedef enum {
	NOT_DEF         = -1,       ///< Is meaning that the redirection isn't setup yet.
	REDIRECT_TRUNC  = O_TRUNC,  ///< Is corresponding to `>>` or `<<`
	REDIRECT_APPEND = O_APPEND, ///< Is corresponding to `>`  or `<`
} redirect_type;

/** Models a stream redirection. */
typedef struct s_redirection {
	char          *path; ///< Is the path to the file to redirect to.
	redirect_type type;  ///< Is the redirection type.
} s_redirection;

/**
 * Models a tsh state.
 *
 * @note We chose to add a new structure in addition to `s_tsh_path` to add a
 * level of semantic and to offer the possibility to increment the shell.
 */
typedef struct {
	s_tsh_path *cwd; ///< Is the current working dir.
} s_tsh_state;

/** Represents all the redirections of a command. */
typedef struct s_command_redirections {
    s_redirection *in;   ///< STDIN_FILENO redirection.
    s_redirection *out;  ///< STDOUT_FILENO redirection.
    s_redirection *err;  ///< STDERR_FILENO redirection.
} s_command_redirections;

#define CMD_MAX_SIZE 1024 ///< Is the maximum length of the command name.

/**
 * Represents a s_command linked list.
 *
 * @see utlist.h for the linked list implementation.
 */
typedef struct s_command {
    char cmd[CMD_MAX_SIZE];                ///< Is the command name.
    char **args;                           ///< Is the command arguments.
    int nb_args;                           ///< Is the number of arguments of the command.
    int is_with_outside_option;            ///< Is equal to TRUE if the the command has an option not supported inside tarball.
    int is_with_tarball_option;            ///< Is equal to TRUE if all the command options are tarball implemented options.
    s_command_redirections *redirections;  ///< Represents all command redirections.
    struct s_command *next;                ///< Is the next command of the list.
} s_command;

#endif
