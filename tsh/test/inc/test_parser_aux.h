#ifndef TEST_PARSER_AUX_H
#define TEST_PARSER_AUX_H

#include <assert.h>
#include <string.h>

#include "../../src/inc/commons.h"
#include "../../src/inc/parser.h"
#include "../../src/inc/utlist.h"

/**
 * Compares two given s_command.
 *
 * @param given_cmd is the result of parse(), must be equal to `expected`
 * @param expected_cmd is the expected control model
 *
 * @return TRUE if the given command is equal to the expected one, else FALSE.
 */
int s_command_are_equal(const s_command *given_cmd, const s_command *expected_cmd);

/**
 * Compares each given command to its expected value.
 *
 * @param given_cmd_list is the list returned by `parse`.
 * @param expected_cmds is the tab containing the expected commands.
 *
 * @return TRUE if all commands are equals.
 */
int all_commands_are_equal(
	s_command *given_cmd_list,
	s_command **expected_cmds,
	const int expected_nb_cmds
);

/**
 * Creates the current working dir path before calling `parse`.
 *
 * @param `cmd` is the command line to parse.
 *
 * @return the result of parse(cmd, cwd).
 */
s_command *parse_with_the_cwd(const char *cmd);

// NOTE: Creates functions should be in their respective header file ?

/** Returns a s_command_redirections given its args (see src/inc/parser.h). */
s_command_redirections *create_s_command_redirections(
    s_redirection *in,
	s_redirection *out,
	s_redirection *err
);

/** Returns a s_redirection given its args (see src/inc/tsh.h). */
s_redirection *create_s_redirection(const char *path, const redirect_type type);

char *create_expected_path_from_sys_root(const char *path);

#endif
