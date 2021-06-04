#include "inc/parser.h"

/**
 * Updates cmd->is_with_tarball_option if option is equal to tarball_option and returns TRUE
 * else, returns FALSE.
 */
static int update_command_is_with_tarball_option(
	s_command *cmd,
	char *option,
	const int curr_indx,
	const char tarball_option
);

/* Initilizes a s_command from a command and append it to the s_command linked list. */
static int append_command(s_command **head, char *command);

/* Manages redirections and options of a command. */
static int manages_redirections_and_options(
	s_command *cmd,
	char **args,
	const int nb_args
);

/* Updates command is_with_ attributs and according the current args and a tarball_option.
 * If is a tarball option, set args[index] to NULL. */
static void update_command_option(
	s_command *cmd,
	char *args[],
	const int index,
	const char tarball_option
);

/* Returns the index of a given args in g_redirect_chars if it is a redirection character. */
static int arg_is_a_redirection_character(const char *args);

/* Prepares the parameter to call add_redirection and call it. */
static int init_right_redirection_according_to_redirection_character(
	s_command **cmd,
	const int redirect_char_index,
	char *path
);

/* Initilizes the s_redirection according a given path and type. */
static void parser_init_redirection(
	s_redirection *redirection,
	const char *path,
	const redirect_type type
);

/**
 * Returns TRUE if the line does not begin by a pipe, does not contains consecutive pipes
 * and does not finish with a pipe.
 */
static int line_has_right_format(const char *line);

const char *g_redirect_chars[NB_REDIRCT_SYMB] = {">", ">>", "<", "<<", "2>", "2>>"};

/* Is the path the to current working dir. */
static s_tsh_path g_cwd;

s_command *parse(const char *line, const s_tsh_path *cwd) {
	s_command *head = NULL;
	char **commands = NULL;
	int nb_commands = 0;

	commons_tsh_copy_path(cwd, &g_cwd);

	if (!strcmp(line, "")) {
		return NULL;
	}

	if (!line_has_right_format(line)) {
		print_error(ERR_PARSING_PIPE_LINE_FORMAT);
		return NULL;
	}

	if (NULL == (commands = tokenize(line, PIPE_CHR, &nb_commands))) {
		return NULL;
	}

	for (int i = 0; i < nb_commands; i++) {
		if (ERR == append_command(&head, commands[i])) {
			return NULL;
		}
	}

	return head;
}

static int line_has_right_format(const char *line) {
	const ssize_t str_len = strlen(line);

	return 0 != str_len
		&& PIPE_CHR != line[0]
		&& PIPE_CHR != line[str_len-1]
		&& NULL == strstr(line, "||");
}

static int append_command(s_command **head, char *command) {
	int nb_args = 0;
	s_command *cmd = NULL;
	char **args = tokenize(command, ' ', &nb_args);

	assert (NULL != (cmd = malloc(sizeof(s_command))));

	commons_safe_str_cpy(cmd->cmd, args[0]);

	cmd->nb_args = nb_args - 1;
	assert(NULL !=(cmd->args = malloc(nb_args * sizeof(char*))));
	cmd->redirections = NULL;
	cmd->is_with_outside_option = FALSE;
	cmd->is_with_tarball_option = FALSE;

	if (ERR == manages_redirections_and_options(cmd, args, nb_args)) {
		return ERR;
	}

	LL_APPEND(*head, cmd);

	return OK;
}

/**
 * The index of the argument in g_redirect_chars is used to define the redirection type.
 * @see init_right_redirection_according_to_redirection_character).
 */
static int arg_is_a_redirection_character(const char *args) {
	for (int i = 0; i < NB_REDIRCT_SYMB; i++) {
		if (0 == strcmp(args, g_redirect_chars[i])) {
			return i;
		}
	}
	return ERR;
}

static int manages_redirections_and_options(s_command *cmd, char **args, const int nb_args) {
	int cmd_args_offset = 0;
	int redirect_char_index;

	for (int j = 0; j < nb_args; j++) {
		/* Look to find redirections or option char. */
		if (MAX_REDIRECT_SYMB_SIZE >= strlen(args[j])) {
			/* Manages redirections. */
			if (ERR != (redirect_char_index = arg_is_a_redirection_character(args[j]))) {
				/* If the redirection character is the last one return an error. */
				if (j >= nb_args - 1) {
					print_error(ERR_PARSING_RED_CHAR_POS);
					return ERR;
				}
				if (NULL == cmd->redirections) {
					assert(NULL != (cmd->redirections = malloc(sizeof(s_command_redirections))));
					cmd->redirections->err = NULL;
					cmd->redirections->out = NULL;
					cmd->redirections->in = NULL;
				}
				if (ERR == init_right_redirection_according_to_redirection_character(
							&cmd, redirect_char_index, args[j+1])
				) {
					return ERR;
				}

				/* Command is like '> path cmd' */
				if (0 == j) {
					if (3 > nb_args) {
						print_error(ERR_PARSING_RED_CHAR_POS);
						return ERR;
					}
					else {
						commons_safe_str_cpy(cmd->cmd, args[j+2]);
						for (int i = 0; i < 3; i++) {
							args[i] = NULL;
						}
						j += 3;
						cmd->nb_args -= 2;
					}
				}
				else {
					j += 1;
					cmd->nb_args -= 2;
					continue;
				}
			}
		}

		/* Manages options. */
		if (j < nb_args && '-' == args[j][0]) {
			if (0 == strcmp(cmd->cmd, "ls")) {
				update_command_option(cmd, args, j, 'l');
			}
			else if (0 == strcmp(cmd->cmd, "cp") || 0 == strcmp(cmd->cmd, "rm")) {
				update_command_option(cmd, args, j, 'r');
			}
			else {
				cmd->is_with_outside_option = TRUE;
			}
		}

		/* The current arg is not equal to NULL means that we add it to the
		 * array of arguments of the current s_command.
		 */
		if (NULL != args[j] && 0 != j) {
			assert(NULL !=
				(cmd->args[cmd_args_offset] = (char*)malloc(CMD_MAX_SIZE * sizeof(char)))
			);
			commons_safe_str_cpy(cmd->args[cmd_args_offset], args[j]);
			cmd_args_offset++;
		}
	}
	return OK;
}

static void update_command_option(
	s_command *cmd,
	char *args[],
	const int index,
	const char tarball_option)
{
	if (update_command_is_with_tarball_option(cmd, args[index], index, tarball_option)) {
		args[index] = NULL;
	}
	else {
		cmd->is_with_outside_option = TRUE;
	}
}

static redirect_type get_redirection_type(const int redirect_char_index) {
	if (redirect_char_index > NB_REDIRCT_SYMB) {
		return ERR;
	}
	/* Given an index i, if i is even, the redirection flag is O_TRUNC
	 * @see g_redirect_chars.
	 */
	if (0 == redirect_char_index % 2)  {
		return REDIRECT_TRUNC;
	}

	return REDIRECT_APPEND;
}

static int init_right_redirection_according_to_redirection_character(
	s_command **cmd,
	const int redirect_char_index,
	char *path)
{
	redirect_type redirection_type;

	if (ERR == (redirection_type = get_redirection_type(redirect_char_index))) {
		return ERR;
	}

	switch (g_redirect_chars[redirect_char_index][0]) {
		case '>':
			if (NULL == (*cmd)->redirections->out) {
				assert(NULL != ((*cmd)->redirections->out = malloc(sizeof(s_redirection))));
				parser_init_redirection((*cmd)->redirections->out, path, redirection_type);
				return OK;
			}
			print_error(ERR_PARSING_TOO_MUCH_RED);
			return ERR;
		break;
		case '<':
			if (NULL == (*cmd)->redirections->in) {
				assert(NULL != ((*cmd)->redirections->in = malloc(sizeof(s_redirection))));
				parser_init_redirection((*cmd)->redirections->in, path, redirection_type);
				return OK;
			}
			print_error(ERR_PARSING_TOO_MUCH_RED);
			return ERR;
		break;
		case '2':
			if (NULL == (*cmd)->redirections->err) {
				assert(NULL != ((*cmd)->redirections->err = malloc(sizeof(s_redirection))));
				parser_init_redirection((*cmd)->redirections->err, path, redirection_type);
				return OK;
			}
			print_error(ERR_PARSING_TOO_MUCH_RED);
			return ERR;
		break;
	}

	return OK;
}

static void parser_init_redirection(
	s_redirection *redirection,
	const char *path,
	const redirect_type type)
{
	assert(NULL != (redirection->path = malloc(strlen(path))));
	commons_safe_str_cpy(redirection->path, path);
	redirection->type = type;
}

static int update_command_is_with_tarball_option(
	s_command *cmd,
	char *option,
	const int curr_indx,
	const char tarball_option)
{
	if (tarball_option == option[1] && 2 == strlen(option)) {
		cmd->is_with_tarball_option = TRUE;
		cmd->nb_args--;
		return TRUE;
	}
	return FALSE;
}

char **tokenize(const char *line, const char delimiter, int *nb_args) {
	char **tokens;
	char *tok = strtok(strdup(line), &delimiter);
	ssize_t curr_tokens_capacity = DEFAULT_NB_COMMANDS_ARGS;
	int curr_tokens_size = 0;

	assert(NULL != (tokens = malloc(curr_tokens_capacity * sizeof(char*))));
	memset(tokens, NONECHAR, curr_tokens_capacity * sizeof(char*));

	while (NULL != tok) {
		if (is_str_contain_only_spaces(tok)) {
			print_error(ERR_PARSING_LINE_EPY_CMD);
			return NULL;
		}
		if (curr_tokens_size >= curr_tokens_capacity) {
			curr_tokens_capacity += DEFAULT_NB_COMMANDS_ARGS;
			assert(NULL != (tokens = realloc(tokens, curr_tokens_capacity * sizeof(char*))));
			memset(tokens+curr_tokens_capacity * sizeof(char*), 0, curr_tokens_capacity * sizeof(char*));
		}
		tokens[curr_tokens_size] = tok;
		curr_tokens_size++;
		tok = strtok(NULL, &delimiter);
	}

	*nb_args = curr_tokens_size;

	free(tok);

	return tokens;
}
