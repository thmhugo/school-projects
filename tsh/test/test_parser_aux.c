#include "inc/test_parser_aux.h"

/* Returns TRUE if both redirections are equal. */
static int s_redirection_are_equal(s_redirection *given, s_redirection *expected);

/* Returns the number of redirections in a linked list of redirections. */
static int count_redirection(s_redirection *redirection);

/* Returns TRUE if both s_tsh_path are equal. */
static int tsh_path_are_equal(const s_tsh_path *given, const s_tsh_path *expected);

char *create_expected_path_from_sys_root(const char *path) {
	char *expected_path = malloc(PATH_MAX);

	assert(NULL != expected_path);

	getcwd(expected_path, PATH_MAX);
	strcat(expected_path, PATH_DELIMITER_STR);
	strcat(expected_path, path);

	return expected_path;
}

s_command *parse_with_the_cwd(const char *cmd) {
	return parse(cmd, commons_tsh_create_s_tsh_path("test/resources/", "", FD_NOTDEF));
}

int all_commands_are_equal(
	s_command *given_cmd_list,
	s_command **expected_cmds,
	const int expected_nb_cmds)
{
	s_command *given_command_elm;
	int i = 0;

	if (NULL == given_cmd_list) {
		return FALSE;
	}

	LL_FOREACH(given_cmd_list, given_command_elm) {
		if (expected_nb_cmds > i) {
			if (!s_command_are_equal(given_command_elm, expected_cmds[i])) {
				return FALSE;
			}
			++i;
		}
		else {
			/* Should have the same number of commands. */
			return FALSE;
		}
	}
	return TRUE;
}

int s_command_are_equal(const s_command *given_cmd, const s_command *expected_cmd) {
	if (NULL == given_cmd || NULL == expected_cmd) {
		return FALSE;
	}
	/* They should have the same number of argument. */
	if (given_cmd->nb_args != expected_cmd->nb_args) {
		return FALSE;
	}

	/* They should have the same arguments. */
	for (int i = 0; i < given_cmd->nb_args; i++) {
		if (0 != strcmp(given_cmd->args[i], expected_cmd->args[i])) {
			return FALSE;
		}
	}

	/* They should have the same is_with_outside_option. */
	if (given_cmd->is_with_outside_option != expected_cmd->is_with_outside_option) {
		return FALSE;
	}

	/* They should have the same is_with_tarball_option. */
	if (given_cmd->is_with_tarball_option != expected_cmd->is_with_tarball_option) {
		return FALSE;
	}

	/* They should have the redirections. */
	if (NULL != given_cmd->redirections && NULL != expected_cmd->redirections) {
		if (NULL != given_cmd->redirections->out) {
			if (!s_redirection_are_equal(given_cmd->redirections->out, expected_cmd->redirections->out)) {
				return FALSE;
			}
		}
		if (NULL != given_cmd->redirections->in) {
			if (!s_redirection_are_equal(given_cmd->redirections->in, expected_cmd->redirections->in)) {
				return FALSE;
			}
		}
		if (NULL != given_cmd->redirections->err) {
			if (!s_redirection_are_equal(given_cmd->redirections->err, expected_cmd->redirections->err)) {
				return FALSE;
			}
		}
	}
	else if (given_cmd->redirections != expected_cmd->redirections) {
		return FALSE;
	}

	return TRUE;
}

static int s_redirection_are_equal(s_redirection *given, s_redirection *expected) {
	if (NULL == given && NULL != expected || NULL != given && NULL == expected) {
		return FALSE;
	}

	/* Their path should be equal. */
	if (strcmp(given->path, expected->path)) {
		return FALSE;
	}

	/* Their type should be equal. */
	if (given->type != expected->type) {
		return FALSE;
	}

	return TRUE;
}

static int tsh_path_are_equal(const s_tsh_path *given, const s_tsh_path *expected) {
	if (0 != strcmp(given->outside_path, expected->outside_path)) {
		return FALSE;
	}
	if (0 != strcmp(given->inside_path, expected->inside_path)) {
		return FALSE;
	}
	return TRUE;
}

s_command_redirections *create_s_command_redirections(
	s_redirection *in,
	s_redirection *out,
	s_redirection *err
) {
	s_command_redirections *command_redirection;
	assert(NULL != (command_redirection = malloc(sizeof(s_command_redirections))));

	if (NULL != in) {
		assert(NULL != (command_redirection -> in = malloc(sizeof(s_redirection))));
		command_redirection->in = in;
	}
	if (NULL != out) {
		assert(NULL != (command_redirection -> out = malloc(sizeof(s_redirection))));
		command_redirection->out = out;
	}
	if (NULL != err) {
		assert(NULL != (command_redirection -> err = malloc(sizeof(s_redirection))));
		command_redirection->err = err;
	}

	return command_redirection;
}

s_redirection *create_s_redirection(const char *path, const redirect_type type) {
	s_redirection *red;

	assert(NULL != (red = malloc(sizeof(s_redirection))));
	assert(NULL != (red->path = malloc(strlen(path))));
	commons_safe_str_cpy(red->path, path);
	red->type = type;

	return red;
}
