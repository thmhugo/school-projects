#ifndef ERROR_H
#define ERROR_H

#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

#include "commons.h"

/* Macros associated to the errors messages. */
#define ERR_CHKSUM "Header checksum is invalid"
#define ERR_CMD_NT_FOUND "command not found"
#define ERR_DIRECTORY "Is a directory"
#define ERR_DIR_NT_EPY "Directory not empty"
#define ERR_FILE_EXISTS "File exists"
#define ERR_FILE_TOO_LONG "File name too long"
#define ERR_INVALID_ARG "Invalid argument"
#define ERR_IS_TAR "Is a tarball"
#define ERR_MANY_ARGS "Too many arguments"
#define ERR_MISSING_OPERAND "Missing operand"
#define ERR_NO_FILE "No such file or directory"
#define ERR_NOT_DIR "Not a directory"
#define ERR_NOT_SUP_CMD "Command not supported in tarball"
#define ERR_NOT_SUP_OPT "Option not supported in tarball"
#define ERR_OMIT_DIR "Directory omited"
#define ERR_OPENING "Error opening a file descriptor"
#define ERR_PARSING_LINE_EPY_CMD "tsh: error while parsing commands: command line contains an empty command.\n"
#define ERR_PARSING_PIPE_LINE_FORMAT "parsing: wrong pipe format.\n"
#define ERR_PARSING_RED_CHAR_POS "tsh: error while parsing commands: redirection syntax error.\n"
#define ERR_PARSING_TOO_MUCH_RED "tsh: error while parsing commands: too much redirections.\n"
#define ERR_RDWR "Error reading or writing to a file descriptor"
#define ERR_READING "Error reading a file descriptor"
#define ERR_REMOVING "Error during removing"
#define ERR_UPD_INVALID "Updated path is invalid"
#define ERR_WRITING "Error writing to a file descriptor"

/* Macros associated to the name of the command. */
#define ERR_APPEND "commons_tar_append_file: "
#define ERR_APPEND_DIR "commons_tar_append_dir: "
#define ERR_BLCKMV "commons_tar_blockmove: "
#define ERR_CATTAR "cat: "
#define ERR_CDTAR "cd: "
#define ERR_CPTAR_CNT_CPY "cp: cannot copy a directory '.', into itself, "
#define ERR_CPTAR "cp: "
#define ERR_CPTAR_NT_FOUND "cp: cannot stat "
#define ERR_CPTAR_NT_OVWR_F "cp: cannot overwrite non-directory "
#define ERR_CPTAR_NT_SPEC "cp: -r not specified; "
#define ERR_LSTAR "ls: cannot access "
#define ERR_MKDIRTAR "mkdir: cannot create directory "
#define ERR_MVTAR "mv: "
#define ERR_MVTAR_NT_FOUND "mv: cannot stat "
#define ERR_RMDIRTAR_REFUSE "rmdir: refusing to remove '.' or '..' directory: "
#define ERR_RMDIRTAR "rmdir: failed to remove "
#define ERR_RMTAR "rm: cannot remove "

/**
 *  Models the return value of a command.
 *
 *  `verdict` is the value indicating if it's an error or not.
 *  `message` is the error message to print.
 */
typedef struct return_handler {
    int verdict;            ///< ERR if it is an error and else OK.
	char message[PATH_MAX];
} return_handler;

/**
 * Creates a return_handler associated to the command and the message.
 *
 * @param command is the name of the command.
 * @param error_msg is the error message associated.
 * @param file_path is the path of the non existing file.
 *
 * @return a return_handler.
 */
return_handler create_error_return_handler(
    const char *command,
    const char *error_msg,
    const char *file_path
);

/**
 * Creates a return_handler with a success verdict and an empty message.
 *
 * @return a return_handler.
 */
return_handler create_successful_return_handler();

/**
 * Prints an error message.
 *
 * @param message is the message to print
 */
void print_error(const char *message);

#endif
