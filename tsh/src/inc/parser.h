#ifndef PARSER_H
#define PARSER_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commons.h"
#include "commons_tar.h"
#include "commons_tsh.h"
#include "redirection.h"
#include "tsh.h"
#include "utlist.h"
#include "error.h"


/**
 * Parses a tsh input (string representations of commands).
 *
 * @note With hindsight, implementing the parser with a library like Argp would have been
 * less laborious, more understandable and stable. However, we are running out
 * of time and we need to keep our parser implemented by hand.
 *
 * @param line is string representations of commands.
 * @param cwd is the path to the current working dir.
 *
 * @return a linked list of s_command.
 */
s_command *parse(const char *line, const s_tsh_path *cwd);

/**
 * Transforms a line (often a path) in an array given a delimiter.
 *
 * @param line is the string to tokenize.
 * @param delimiter is the token delimiter.
 * @param nb_args is the size of the returned array.
 *
 * @return an array of tokens.
 */
char **tokenize(const char *line, const char delimiter, int *nb_args);

#endif
