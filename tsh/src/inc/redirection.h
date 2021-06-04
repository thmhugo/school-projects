#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "commons.h"

#define MAX_REDIRECT_SYMB_SIZE 3
#define NB_REDIRCT_SYMB  6

/**
 * Redirects STDIN_FILENO to the given fd.
 *
 * @param fd is the fd to dup2 with STDIN_FILENO.
 *
 * @return ERR if dup2 or close fails, else, OK.
 */
int redirection_redirect_stdin_to(const int fd);

/**
 * Redirects STDOUT_FILENO to the given fd.
 *
 * @param fd is the fd to dup2 with STDOUT_FILENO.
 *
 * @return ERR if dup2 or close fails, else, OK.
 */
int redirection_redirect_stdout_to(const int fd);

/**
 * Redirects STDERR_FILENO to the given fd.
 *
 * @param fd is the fd to dup2 with STDERR_FILENO.
 *
 * @return ERR if dup2 or close fails, else, OK.
 */
int redirection_redirect_stderr_to(const int fd);

/**
 * Reset STDOUT_FILENO to its default value.
 *
 * @return ERR if dup2 fails, else, OK.
 */
int redirection_reset_stdout();

/**
 * Reset STDIN_FILENO to its default value.
 *
 * @return ERR if dup2 fails, else, OK.
 */
int redirection_reset_stdin();

/**
 * Reset STDERR_FILENO to its default value.
 *
 * @return ERR if dup2 fails, else, OK.
 */
int redirection_reset_stderr();

#endif
