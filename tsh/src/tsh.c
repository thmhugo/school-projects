#include "inc/commons.h"
#include "inc/shell.h"

/* Initializes g_state from the cwd. */
static void init_starting_state();

/* Prints the prompt : "cwd/ >" */
static void print_prompt(const char *path);

/* Prints the path to the tsh current working dir. */
static void print_pwd();

/* Executes a linked list of s_command. */
static int exec(s_command *s_commands);

/* Executes a command and redirect stdout and stdin according pipe fd_in.
 * @return the pipe_fd reader for the next command */
static int exec_with_pipe(const s_command *cmd, const int fd_in, const int is_first);

/* Returns TRUE if the internal command could creates a new file. */
static int is_creating_new_file(const char *cmd);

/* Returns TRUE if its the last arg of the cmd->args. */
static int is_dest_arg(const s_command *cmd, const int index);

/* Executes the cd command and manages its errors. */
static void exec_cd(const s_command *cmd);

/* Executes on command. */
static void exec_command(const s_command *cmd);

/* Verifies if a command should be executed with exec_external_command. */
static int cmd_should_be_executed_as_an_external_cmd(
	char *args_without_tar[],
	const int nb_args_without_tar,
	const int nb_args_with_tar,
	const s_command *cmd);

/* Returns TRUE if the args contains only string starting by '-'. */
static int contains_only_options(char *args[], const int nb_args);

/* Executes a command with only external path in its cmd->args or unsported options for internal commands. */
static void exec_external_command(const s_command *cmd);

/* Executes a command with at least on tarball valid path in its cmd->args. */
static void exec_internal_command(const s_command *cmd);

/* Stores in two separated arrays src_cmd->args according if they are valid tarball path or not. */
static void separates_args(
		const s_command *src_cmd,
		char **args_with_tar,
		int *nb_args_with_tar,
		char **arg_without_tar,
		int *nb_args_without_tar);

/* Creates the argv array required by execvp from a s_command. */
static void create_argv_for_execvp(const s_command *cmd, const size_t argv_len, char **argv);

/* Manages 'normal' redirections and pipe redirections. */
static void manage_redirections(
	const s_command_redirections *red,
	const int fd_in,
	const int fd_out,
	const int is_first,
	const int is_last);

/* Redirects IO streams to the corresponding command redirections. */
static int exec_redirections(const s_command_redirections *redirections);

/* Redirects IO stream according the redirection. */
static int exec_redirection(
	const int is_stdin_red,
	const s_redirection *redirection,
	const int flags,
	char **tmpred_name,
	int *tmpred_fd,
	int (*redirect_fun) (const int fd)
);

/* Resets IO streams to their default values. */
static void reset_redirections(const s_command *cmd);

/* Writes the stored tempfile data into the corresponding tarball file. */
static void reset_tar_redirection(const s_redirection *red, int *src_fd, const char *src_file);

/* Models supported internal commands. */
const static char *g_internal_cmds[NB_INTERNALS_CMDS] = {
	"cat",
	"ls",
	"cp",
	"rm",
	"rmdir",
	"mkdir",
	"mv"};

/* Checks if it is an internal command. */
static int is_internal_cmd(const s_command *cmd);

/* Associates an implementation for each internal command in g_internal_cmds. */
static return_handler
(*g_internal_cmd_implementations[NB_INTERNALS_CMDS])
(const s_tsh_state *state, const s_command *cmd) = {
	cattar_from_cmd,
	lstar_from_cmd,
	cptar_from_cmd,
	rmtar_from_cmd,
	rmdirtar_from_cmd,
	mkdirtar_from_cmd,
	mvtar_from_cmd
	};

/* Global variables used for redirect into tarball file by using tempfile. */
static char *g_tmpin_name, *g_tmpout_name, *g_tmperr_name;
static int g_tmpin_fd = FD_NOTDEF;
static int g_tmpout_fd = FD_NOTDEF;
static int g_tmperr_fd = FD_NOTDEF;

/* Is the current tsh state. */
static s_tsh_state g_state;

int main(int argc, char *argv[]) {
	char command_line[CMD_MAX_SIZE];
	s_command *commands_ll;
	int shell_status = OK;

	init_starting_state();

	write(STDOUT_FILENO, WELCOME_MSG, strlen(WELCOME_MSG));

	while (EXIT != shell_status) {
		print_prompt(commons_remove_last_char(commons_tsh_get_path_from(g_state.cwd)));

		/* Reads the input command line. */
		memset(command_line, NONECHAR, CMD_MAX_SIZE);
		read(STDIN_FILENO, command_line, CMD_MAX_SIZE);

		/* Gets the corresponding s_command linked list. */
		commands_ll = parse(strndup(command_line, strlen(command_line)-1), g_state.cwd);

		/* If the parsing went well, executes commands. */
		if (NULL != commands_ll) {
			shell_status = exec(commands_ll);
		}

		free(commands_ll);
	}

	return OK;
}

static void init_starting_state() {
	char cwd[PATH_MAX];

	getcwd(cwd, PATH_MAX);
	strcat(cwd, PATH_DELIMITER_STR);

	g_state = *commons_tsh_create_state_from_outside_path(cwd);
}

static void print_prompt(const char *path) {
	write(STDOUT_FILENO, path, strlen(path));
	write(STDOUT_FILENO, PROMPT, PROMPT_LENGTH);
}

static int exec(s_command *s_commands) {
	s_command *cmd;
	int pipe_in = STDIN_FILENO;
	int is_first = TRUE;

	LL_FOREACH (s_commands, cmd) {
		if (!strcmp(cmd->cmd, "exit")) {
			return EXIT;
		}
		if (!strcmp(cmd->cmd, "cd")) {
			exec_cd(cmd);
		}
		else {
			pipe_in = exec_with_pipe(cmd, pipe_in, is_first);
			is_first = FALSE;
		}
	}
	return OK;
}

/**
 * Is is_first, fd_in == STDIN_FILENO, otherwise, fd_in == prev pipe_fd[READER] until
 * is_last where the pipe_fd[READER] is closed.
 */
static int exec_with_pipe(const s_command *cmd, const int fd_in, const int is_first) {
	const int is_last = NULL == cmd->next;
	int pipe_fd[2];

	if (pipe(pipe_fd)) {
		print_error("tsh: pipe cannot be initialized\n");
		return ERR;
	}

	switch (fork()) {
		case ERR:
			{
				print_error(FORK_ERR_MSG);
				exit(EXIT_FAILURE);
			}
		break;
		case OK:
			{
				manage_redirections(
					cmd->redirections, fd_in, pipe_fd[WRITER], is_first, is_last
				);

				if (!strcmp(cmd->cmd, "pwd")) {
					print_pwd();
				}
				else {
					exec_command(cmd);
				}

				reset_redirections(cmd);
				exit(EXIT_SUCCESS);
			}
		break;
		default:
		{
			wait(NULL);

			close(pipe_fd[WRITER]);

			if (0 < fd_in) {
				close(fd_in);
			}
			if (is_last) {
				close(pipe_fd[READER]);
			}
		}
		break;
	}
	return pipe_fd[READER];
}

static void manage_redirections(
	const s_command_redirections *red,
	const int fd_in,
	const int fd_out,
	const int is_first,
	const int is_last)
{
	/* Manages redirections */
	if (NULL != red && ERR == exec_redirections(red)) {
		print_error("tsh: invalid redirection\n");
		exit(EXIT_FAILURE);
	}

	/* Manages pipes */
	if (!is_last && (NULL == red || NULL == red->out)) {
		/* Not the last command and without stdout redirection. */
		redirection_redirect_stdout_to(fd_out);
	}
	if (!is_first && (NULL == red || NULL == red->in)) {
		/* Not the first command and without stdin redirection. */
		redirection_redirect_stdin_to(fd_in);
	}
}

static int exec_redirections(const s_command_redirections *redirections) {
	if (NULL != redirections->out) {
		if (ERR == exec_redirection(FALSE,
									redirections->out,
									redirections->out->type | O_WRONLY | O_CREAT,
									&g_tmpout_name,
									&g_tmpout_fd,
									&redirection_redirect_stdout_to))
		{
			return ERR;
		}
	}
	if (NULL != redirections->err) {
		if (ERR == exec_redirection(FALSE,
									redirections->err,
									redirections->err->type | O_WRONLY | O_CREAT,
									&g_tmperr_name,
									&g_tmperr_fd,
									&redirection_redirect_stderr_to))
		{
			return ERR;
		}
	}
	if (NULL != redirections->in) {
		if (ERR == exec_redirection(TRUE,
									redirections->in,
									O_RDONLY,
									&g_tmpin_name,
									&g_tmpin_fd,
									&redirection_redirect_stdin_to))
		{
			return ERR;
		}
	}
	return OK;
}

/* For tarball redirections, used a tempfile in order to redirect stdout and/or stderr
 * and then in reset_redirections, copies the content of the tempfile into a new file
 * inside the wanted tarball.
 * If is a stdin tarball redirection, copies the content of the tarball file into a
 * tempfile before redirecting STDIN_FILENO into this tempfile. */
static int exec_redirection(
	const int is_stdin_red,
	const s_redirection *redirection,
	const int flags,
	char **tmpred_name,
	int *tmpred_fd,
	int (*redirect_fun) (const int fd))
{
	int red_fd;
	s_tsh_path *updated_path;

	updated_path = commons_tsh_update_path_from(&g_state, redirection->path);
	if (NULL == updated_path) {
		return ERR;
	}

	/* A valid redirection could not redirect from or to a valid tarball. */
	const int test_tar_fd = open(commons_remove_last_char(updated_path->outside_path), O_RDONLY);
	if (commons_tar_is_valid(test_tar_fd)) {
		close(test_tar_fd);
		if (!strcmp(updated_path->inside_path, "")) {
			print_error(create_error_return_handler("tsh: ", ERR_IS_TAR, redirection->path).message);
			return ERR;
		}
	}

	if (0 < updated_path->tar_fd) {
		/* Tarball redirection. */
		*tmpred_name = strdup(TMP_NAME_TEMPLATE);
		/* Creates tempfile in order to stores redirections data before writing them
		 * into their corresponding tarball file. */
		if (ERR == (*tmpred_fd = mkstemp(*tmpred_name))) {
			*tmpred_fd = FD_NOTDEF;
			return ERR;
		}
		if (is_stdin_red) {
			return_handler rh;
			rh = cptar(*updated_path, *commons_tsh_create_state_from_outside_path(*tmpred_name)->cwd, FALSE);
			if (ERR == rh.verdict) {
				print_error(rh.message);
				unlink(*tmpred_name);
				close(*tmpred_fd);
				return ERR;
			}
		}
		red_fd = *tmpred_fd;
	}
	else {
		/* Normal redirection. */
		red_fd = open(commons_tsh_get_path_from(updated_path), flags, TSH_DEFAULT_MODE);
	}

	if (ERR == red_fd || ERR == redirect_fun(red_fd)) {
		return ERR;
	}

	return OK;
}

static void reset_redirections(const s_command *cmd) {
	if (NULL != cmd->redirections) {
		if (FD_NOTDEF != g_tmpout_fd) {
			reset_tar_redirection(cmd->redirections->out, &g_tmpout_fd, g_tmpout_name);
		}
		if (FD_NOTDEF != g_tmperr_fd) {
			reset_tar_redirection(cmd->redirections->err, &g_tmperr_fd, g_tmperr_name);
		}
		if (FD_NOTDEF != g_tmpin_fd) {
			unlink(g_tmpin_name);
			close(g_tmpin_fd);
			g_tmpin_fd = FD_NOTDEF;
		}
	}
}

static void reset_tar_redirection(const s_redirection *red, int *src_fd, const char *src_file) {
	s_tsh_path *updated_path = commons_tsh_update_path_from(&g_state, red->path);

	if (REDIRECT_TRUNC == red->type) {
		rmtar(updated_path->tar_fd, FALSE, updated_path->inside_path);
	}
	else {
		print_error("tsh: Unsupported redirection type for tarball: '>>' or '2>'\n");
	}

	print_error(commons_tar_append_extern_file_to(
								src_file,
								updated_path->tar_fd,
								updated_path->inside_path).message);

	unlink(src_file);
	close(*src_fd);
	*src_fd = FD_NOTDEF;
}

static void print_pwd() {
	char *cwd = commons_tsh_get_path_from(g_state.cwd);
	write(STDOUT_FILENO, cwd, strlen(cwd));
	write(STDOUT_FILENO, "\n", 1);
}

static void exec_cd(const s_command *cmd) {
	return_handler res;

	if (1 < cmd->nb_args) {
		write(STDOUT_FILENO, CD_ERR_MSG, strlen(CD_ERR_MSG));
		write(STDOUT_FILENO, cmd->args[0], strlen(cmd->args[0]));
		write(STDOUT_FILENO, "\n", 1);

		return;
	}
	if (0 == cmd->nb_args) {
		cmd->args[0] = getenv("HOME");
		g_state.cwd->tar_fd = FD_NOTDEF;
		memset(g_state.cwd->inside_path, NONECHAR, PATH_MAX);
	}

	res = cd(&g_state, cmd->args[0]);

	if (ERR == res.verdict) {
		print_error(res.message);
	}
}

static void exec_command(const s_command *cmd) {
	char *args_with_tar[cmd->nb_args];
	char *args_without_tar[cmd->nb_args];
	int nb_args_with_tar = 0;
	int nb_args_without_tar = 0;
	const int is_an_internal_cmd = is_internal_cmd(cmd);

	separates_args(
		cmd,
		args_with_tar,
		&nb_args_with_tar,
		args_without_tar,
		&nb_args_without_tar
	);

	if (cmd_should_be_executed_as_an_external_cmd(
			args_without_tar,
			nb_args_without_tar,
			nb_args_with_tar,
			cmd))
	{
		s_command *cmd_without_tar =
			create_s_command(
				cmd->cmd,
				nb_args_without_tar,
				args_without_tar,
				cmd->is_with_outside_option,
				cmd->is_with_tarball_option,
				cmd->redirections
			);
		exec_external_command(cmd_without_tar);
	}

	if ((0 == cmd->nb_args || contains_only_options(args_with_tar, nb_args_with_tar))
		&& 0 < g_state.cwd->tar_fd
		&& strcmp(cmd->cmd, "cat")
		&& is_an_internal_cmd)
	{
		/* CASE: A command without argument executed inside a tarball
		 * (excepted 'cat' which could be executed like an external command). */
		args_with_tar[nb_args_with_tar++] = "";
	}

	if (0 < nb_args_with_tar) {
		s_command *cmd_with_tar =
			create_s_command(
				cmd->cmd,
				nb_args_with_tar,
				args_with_tar,
				cmd->is_with_outside_option,
				cmd->is_with_tarball_option,
				cmd->redirections
			);
		exec_internal_command(cmd_with_tar);
	}
}

static int cmd_should_be_executed_as_an_external_cmd(
	char *args_without_tar[],
	const int nb_args_without_tar,
	const int nb_args_with_tar,
	const s_command *cmd)
{
	const int is_an_internal_cmd = is_internal_cmd(cmd);
	const int is_in_tar = 0 < g_state.cwd->tar_fd;

	return (
		(0 < nb_args_without_tar
			&& (!contains_only_options(args_without_tar, nb_args_without_tar)
			|| ((!is_in_tar || !is_an_internal_cmd)
				&& 0 == nb_args_with_tar)))
		|| (0 == cmd->nb_args
			&& (!is_in_tar
				|| !is_an_internal_cmd
				|| !strcmp(cmd->cmd, "cat")))
	);
}

static int is_internal_cmd(const s_command *cmd){
	for (int i = 0; i < NB_INTERNALS_CMDS; i++) {
		if (0 == strcmp(cmd->cmd, g_internal_cmds[i])) {
			return TRUE;
		}
	}

	return FALSE;
}

static int contains_only_options(char *args[], const int nb_args) {
	if (0 == nb_args) {
		return FALSE;
	}
	for (int i = 0; i < nb_args; ++i) {
		if ('-' != args[i][0]) {
			return FALSE;
		}
	}
	return TRUE;
}

static void separates_args(
		const s_command *src_cmd,
		char **args_with_tar,
		int *nb_args_with_tar,
		char **args_without_tar,
		int *nb_args_without_tar)
{
	s_tsh_path *updated_arg;
	int is_valid_dest_tarball_path = FALSE;

	/* CASE: Where the cmd is "mv | cp | mkdir" and the destination argument is a
	 * valid tarball path. */
	if (is_creating_new_file(src_cmd->cmd)
		&& 0 < src_cmd->nb_args
		&& (commons_tsh_is_a_valid_tarball_path(
				&g_state,
				commons_tar_get_parent_path(src_cmd->args[src_cmd->nb_args-1]))
		|| 	commons_tsh_is_a_valid_tarball_path(
				&g_state,
				src_cmd->args[src_cmd->nb_args-1]))
		)
	{
		is_valid_dest_tarball_path = TRUE;
	}
	for (int i = 0; i < src_cmd->nb_args; ++i) {
		/* To independently manage "/" that also represents the tar root.*/
		if (!strcmp(PATH_DELIMITER_STR, src_cmd->args[i])) {
			args_without_tar[(*nb_args_without_tar)++] = strdup(src_cmd->args[i]);
		}
		else {
			/* Simplifies the argument if its a path. */
			updated_arg = commons_tsh_update_path_from(&g_state, src_cmd->args[i]);

			if (is_valid_dest_tarball_path
				|| (NULL != updated_arg
					&& 0 < updated_arg->tar_fd
					&& is_internal_cmd(src_cmd)
					&& '-' != src_cmd->args[i][0])
				|| (is_dest_arg(src_cmd, i) && 0 < (*nb_args_with_tar)))
			{
				/* CASE: Is a valid tarball path or it is the destination argument and
				* the command contains at least on valid tarball argument. */
				args_with_tar[(*nb_args_with_tar)++] = strdup(src_cmd->args[i]);
			}
			else {
				/* CASE: Is not a valid tarball path. */
				args_without_tar[(*nb_args_without_tar)++] = strdup(src_cmd->args[i]);
			}
		}

	}
}

static int is_creating_new_file(const char *cmd) {
	return !strcmp(cmd, "cp")
		|| !strcmp(cmd, "mkdir")
		|| !strcmp(cmd, "mv");
}

static int is_dest_arg(const s_command *cmd, const int index) {
	return (!strcmp(cmd->cmd, "mv") || !strcmp(cmd->cmd, "cp"))
			&& index == cmd->nb_args - 1;
}

static void exec_external_command(const s_command *cmd) {
	return_handler rh;

	switch (fork()) {
		case ERR:
			{
				print_error(FORK_ERR_MSG);
				exit(EXIT_FAILURE);
			}
		break;
		case OK:
			{
				const int argv_len = cmd->nb_args + 2 + (cmd->is_with_tarball_option ? 1 : 0);
				char *argv[argv_len];

				create_argv_for_execvp(cmd, argv_len, argv);
				execvp(argv[0], argv);

				rh = create_error_return_handler("", ERR_CMD_NT_FOUND, cmd->cmd);
				print_error(rh.message);

				exit(EXIT_SUCCESS);
			}
		break;
		default:
			wait(NULL);
		break;
	}
}

static void create_argv_for_execvp(const s_command *cmd, const size_t argv_len, char **argv) {
	assert(NULL != (argv[0] = malloc(strlen(cmd->cmd))));
	commons_safe_str_cpy(argv[0], cmd->cmd);

	argv[argv_len-1] = NULL;
	for (int i = 1; i < argv_len - 1; ++i) {
		char *arg;

		/* Adds parsed tarball options. */
		if (argv_len - 2 == i && cmd->is_with_tarball_option) {
			arg = !strcmp(cmd->cmd, "ls") ? "-l" : "-r";
		}
		/* Simplified path containing '..'. */
		else if (NULL != strstr(cmd->args[i-1], "..")) {
			arg = commons_tsh_update_path_from(&g_state, cmd->args[i-1])->outside_path;
		}
		/* Simple argument. */
		else {
			arg = cmd->args[i-1];
		}
		assert(NULL != (argv[i] = malloc(strlen(arg))));
		commons_safe_str_cpy(argv[i], arg);
	}
}

static void exec_internal_command(const s_command *cmd) {
	return_handler rh;
	int is_internal_cmd = FALSE;

	if (cmd->is_with_outside_option) {
		print_error(create_error_return_handler("tsh: ", ERR_NOT_SUP_OPT, cmd->cmd).message);
		return;
	}

	for (int i = 0; i < NB_INTERNALS_CMDS && !is_internal_cmd; ++i) {
		if (0 == strcmp(cmd->cmd, g_internal_cmds[i])) {
			/* cmd->cmd corresponds to a supported internal command. */
			is_internal_cmd = TRUE;
			rh = (*g_internal_cmd_implementations[i]) (&g_state, cmd);
			if (ERR == rh.verdict) {
				print_error(rh.message);
			}
		}
	}

	if (!is_internal_cmd) {
		rh = create_error_return_handler("tsh: ", ERR_NOT_SUP_CMD, cmd->cmd);
		print_error(rh.message);
	}
}
