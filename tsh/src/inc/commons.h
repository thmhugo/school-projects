#ifndef COMMONS_H
#define COMMONS_H

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#include "tar.h"
#include "utlist.h"
#include "tsh.h"

/****** Useful macros and constants. ******/

#define NONECHAR '\0'

#define PATH_DELIMITER_STR 	"/"
#define PATH_DELIMITER_CHR 	'/'
#define PATH_TO_PARENT_DIR 	".."
#define PATH_TO_SELF_DIR 	"."
#define PIPE_STR            "|"
#define PIPE_CHR            '|'

#define ERR -1
#define OK   0

#define TRUE  1
#define FALSE 0

#define FIRST  1
#define LAST 0

#define CHECKSUM_ERR 0

#define FD_NOTDEF -2

#define MIN(x,y) x < y ? x : y

#define DB_STR(str) printf("<%s::l.%d> %s = '%s'\n", __FILE__, __LINE__, #str, str);
#define DB_CHR(char) printf("<%s::l.%d> %s = %c\n", __FILE__, __LINE__, #char, char);
#define DB_INT(nb)  printf("<%s::l.%d> %s = %d\n", __FILE__, __LINE__, #nb, (int)nb);

#define DEFAULT_NB_COMMANDS_ARGS 10

/****** Useful functions. ******/

/**
 * Copies the file content into a buffer from a given offset.
 *
 * @param file_path   is the path to the file.
 * @param offset      is the offset to copy from.
 * @param buffer_size is the buffer size.
 * @param buffer      is the buffer to copy to.
 *
 * @return the arrival offset, if the copy was successful ; else, ERR.
 */
ssize_t commons_copy_file_content_from(
		const char *file_path,
		const ssize_t offset,
		const ssize_t buffer_size,
		char *buffer
);

/**
 * Calculates the file size.
 *
 * @param fd is the file descriptor of the file.
 *
 * @return the file size.
 */
ssize_t commons_get_file_size(const int fd);

/**
 * Fills the destination string with NONECHAR by calling `memset` before copying.
 *
 * @param dest is the destination string.
 * @param src is the source string.
 *
 * @return the new destination string.
 */
char *commons_safe_str_cpy(char *dest, const char *src);

/**
 * Returns true if str contains only space char.
 * @see isspace().
 */
int is_str_contain_only_spaces(const char *str);

/**
 * Checks if a path corresponds to a directory.
 *
 * @param path is the path of the destination we want to check.
 *
 * @return TRUE if the outside_path of the given tsh_path corresponds to a directory.
 */
int commons_is_dir(const s_tsh_path path);

/**
 * Checks if a path corresponds to a file (using open).
 *
 * @param path is the path of the destination we want to check.
 *
 * @return TRUE if the outside_path of the given tsh_path corresponds to a directory.
 */
int commons_is_file(const s_tsh_path path);
/**
 * Removes the file at `file_path`. If it is a folder, removes recuresively all its
 * subdirs and files.
 *
 * @param file_path is the target path of the file to remove.
 *
 * @return OK if all the files are correctly removed and ERR in these cases:
 *			the file_path is incorrect.
 *			the folder could not be opened.
 *			a remove error occurs.
 */
int commons_remove_file(const char *file_path);

/**
 * Splits the file an the path to the file from a given path.
 *  ex : "path/to/file" -> dirs_without_file = "path/to/" & file = "file"
 * @param path is the path to split.
 * @param dirs_without_file is where the path to the file will be stored.
 * @param file is where the file name will be stored.
 *
 */
void split_dirs_and_file(char *path, char **dirs_without_file, char **file);

/**
 * Removes the last charcter of a string.
 *
 * @param str is the string.
 *
 * @return a copy of str without the last char.
 */
char *commons_remove_last_char(const char *str);

int commons_operands_are_missing(const s_command *cmd);

#endif
