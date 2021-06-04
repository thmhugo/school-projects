#include "inc/commons_tsh.h"

static int 	commons_tsh_is_a_valid_dir(const char *path);
static int 	commons_tsh_is_a_valid_tar(const char *simplified_path);
static int 	commons_tsh_try_to_find_file_in_tar(
		char *simplified_path, s_tsh_state *tsh_state_cpy
);
static void commons_tsh_init_simplified_path(char *simplified_path, char *path);
static void commons_tsh_parse_path(
		char *parsed_path[], int *nb_parsed_dir, const char *path
);
static void commons_tsh_manage_path_to_parent(char *simplified_path);
static void commons_tsh_simplified_path_is_now_from_current_pwd(char *simplified_path);
static void commons_tsh_simplified_path_is_now_from_tar_root(
		char *simplified_path, int *is_first_dir_from_tar_root
);
static void commons_tsh_update_tsh_state(char *simplified_path);

s_tsh_state *commons_tsh_create_state_from_outside_path(const char *path) {
    s_tsh_state *tmp_tsh_state;

    assert(NULL != (tmp_tsh_state = malloc(sizeof(s_tsh_state))));
    assert(NULL != (tmp_tsh_state->cwd = malloc(sizeof(s_tsh_path))));

    commons_safe_str_cpy(tmp_tsh_state->cwd->outside_path, path);
    memset(tmp_tsh_state->cwd->inside_path, NONECHAR, PATH_MAX);
    tmp_tsh_state-> cwd->tar_fd = FD_NOTDEF;

    return tmp_tsh_state;
}

void commons_tsh_copy_state(
		const s_tsh_state *tsh_state, s_tsh_state *tsh_state_cpy
) {
	assert(NULL != (tsh_state_cpy->cwd = malloc(sizeof(s_tsh_path))));
	commons_tsh_copy_path(tsh_state->cwd, tsh_state_cpy->cwd);
}

void commons_tsh_copy_path(
		const s_tsh_path *src_path, s_tsh_path *dest_path
) {
	commons_safe_str_cpy(dest_path->outside_path, src_path->outside_path);
	commons_safe_str_cpy(dest_path->inside_path, src_path->inside_path);
	dest_path->tar_fd = src_path->tar_fd;
}

int commons_tsh_path_is_equal_to(const s_tsh_path *tsh_path, const char *path) {
	return 0 == strcmp(commons_tsh_get_path_from(tsh_path), path);
}

static int g_tar_fd;
static s_tsh_state *g_tsh_state;

int commons_tsh_update_state_from_path(s_tsh_state *tsh_state, char *path) {
	char *parsed_path[PATH_MAX];
	char *simplified_path;
	int nb_parsed_dir = 0;
	s_tsh_state tsh_state_cpy;

	g_tar_fd = tsh_state->cwd->tar_fd;
	g_tsh_state = tsh_state;

	commons_tsh_copy_state(tsh_state, &tsh_state_cpy);

	/* Manages the special case "/" independently. */
	if (!strcmp(PATH_DELIMITER_STR, path)) {
		g_tar_fd = FD_NOTDEF;
		/* Need to directly modify 'tsh_path' for 'cd' .*/
		tsh_state->cwd->tar_fd = FD_NOTDEF;
		commons_tsh_update_tsh_state(PATH_DELIMITER_STR);
		return g_tar_fd;
	}

	commons_tsh_parse_path(parsed_path, &nb_parsed_dir, path);

	assert(NULL != (simplified_path = malloc(PATH_MAX)));
	commons_tsh_init_simplified_path(simplified_path, path);

	for (int i = 0; i < nb_parsed_dir; i++) {
		int is_first_dir_from_tar_root = FALSE;

		if (0 == strcmp(parsed_path[i], PATH_TO_PARENT_DIR)) {
			commons_tsh_manage_path_to_parent(simplified_path);
		}
		else if (0 == strcmp(parsed_path[i], PATH_TO_SELF_DIR));
		else {
			strcat(simplified_path, parsed_path[i]);

			if (FD_NOTDEF == g_tar_fd && !commons_tsh_is_a_valid_dir(simplified_path)) {
				if (commons_tsh_is_a_valid_tar(simplified_path)) {
					commons_tsh_simplified_path_is_now_from_tar_root(
							simplified_path,
							&is_first_dir_from_tar_root
					);
				}
				else {
					commons_tsh_copy_state(&tsh_state_cpy, g_tsh_state);
					return ERR;
				}
			}
			if (!is_first_dir_from_tar_root) {
				strcat(simplified_path, PATH_DELIMITER_STR);
			}
			if (0 < g_tar_fd) {
				/* In tarball. */
				if (ERR == commons_tsh_try_to_find_file_in_tar(simplified_path, &tsh_state_cpy)) {
					return ERR;
				}
			}
		}
	}

	commons_tsh_update_tsh_state(simplified_path);

	return g_tar_fd;
}

static void commons_tsh_init_simplified_path(char *simplified_path, char *path) {
	if (FD_NOTDEF == g_tar_fd) {
		if (PATH_DELIMITER_CHR == path[0]) {
			strcpy(simplified_path, "");
		}
		else {
			strcpy(simplified_path, g_tsh_state->cwd->outside_path);
		}
	}
	else {
		strcpy(simplified_path, g_tsh_state->cwd->inside_path);
	}
}

static void commons_tsh_parse_path(
		char *parsed_path[], int *nb_parsed_dir, const char *path
) {
	char path_cpy[strlen(path)];
	char *token;
	char *token_tmp;

	strcpy(path_cpy, path);

	if (PATH_DELIMITER_CHR == path[0]) {
		assert(NULL != (token = malloc(PATH_MAX)));
		token_tmp = strtok(path_cpy, PATH_DELIMITER_STR);
		token[0] = PATH_DELIMITER_CHR;
		strcpy(token + 1, token_tmp);
	}
	else {
		token = strtok(path_cpy, PATH_DELIMITER_STR);
	}

	while (NULL != token) {
		assert(NULL != (parsed_path[*nb_parsed_dir] = malloc(strlen(token))));
		strcpy(parsed_path[(*nb_parsed_dir)++], token);
		token = strtok(NULL, PATH_DELIMITER_STR);
	}
}

static void commons_tsh_simplified_path_is_now_from_tar_root(
		char *simplified_path, int *is_first_dir_from_tar_root
) {
	g_tsh_state->cwd->tar_fd = g_tar_fd;

	strcpy(g_tsh_state->cwd->outside_path, simplified_path);
	strcat(g_tsh_state->cwd->outside_path, "/");

	strcpy(simplified_path, "");
	*is_first_dir_from_tar_root = TRUE;
}

static void commons_tsh_simplified_path_is_now_from_current_pwd(char *simplified_path) {
	g_tsh_state->cwd->tar_fd = FD_NOTDEF;
	g_tar_fd = FD_NOTDEF;

	strcpy(g_tsh_state->cwd->inside_path, "");
	strcpy(simplified_path, g_tsh_state->cwd->outside_path);
	strcpy(simplified_path, commons_tar_get_parent_path(simplified_path));
}

static void commons_tsh_manage_path_to_parent(char *simplified_path) {
	if (0 == strcmp(simplified_path, "") && 0 < g_tsh_state->cwd->tar_fd) {
		/* Going outside tarball. */
		commons_tsh_simplified_path_is_now_from_current_pwd(simplified_path);
	}
	else {
		strcpy(simplified_path, commons_tar_get_parent_path(simplified_path));
	}
}

static int commons_tsh_is_a_valid_tar(const char *simplified_path) {
	return (
		ERR != (g_tar_fd = open(simplified_path, O_RDWR))
		&& commons_tar_is_valid(g_tar_fd)
	);
}

static int commons_tsh_is_a_valid_dir(const char *path) {
	struct stat s;

	return OK == stat(path, &s) && S_ISDIR(s.st_mode);
}

static int commons_tsh_try_to_find_file_in_tar(
		char *simplified_path,
		s_tsh_state *tsh_state_cpy
) {
	s_posix_header tmp_pheader;
	int find = commons_tar_find_file_from(g_tar_fd, simplified_path, &tmp_pheader);

	if (FALSE == find) {
		commons_tsh_copy_state(tsh_state_cpy, g_tsh_state);
		return ERR;
	}

	return OK;
}

static void commons_tsh_update_tsh_state(char *simplified_path) {
	if (0 < g_tar_fd) {
		commons_safe_str_cpy(g_tsh_state->cwd->inside_path, simplified_path);
	}
	else {
		commons_safe_str_cpy(g_tsh_state->cwd->outside_path, simplified_path);
		commons_safe_str_cpy(g_tsh_state->cwd->inside_path, "");
	}
}

char *commons_tsh_get_path_from(const s_tsh_path *tsh_path) {
	char *path;

	assert(NULL != (path = malloc(PATH_MAX)));

	commons_safe_str_cpy(path, tsh_path->outside_path);
	strcat(path, tsh_path->inside_path);

	return path;
}

void commons_tsh_free_command_ll(s_command *head) {
    s_command *elt;
    s_command *tmp;

    LL_FOREACH_SAFE(head, elt, tmp) {
        LL_DELETE(head, elt);
        free(elt);
    }
}

s_tsh_path *commons_tsh_create_s_tsh_path(const char *out, const char *in, const int fd) {
    s_tsh_path *tsh_path;

	assert(NULL != (tsh_path = malloc(sizeof(s_tsh_path))));
    if (NULL != out) {
        commons_safe_str_cpy(tsh_path->outside_path, out);
	}
    if (NULL != in) {
        commons_safe_str_cpy(tsh_path->inside_path, in);
	}
    tsh_path->tar_fd = fd;

    return tsh_path;
}

static int commons_tsh_is_tarball_involved_in_redirection(
	const s_tsh_state *curr_state,
	const s_redirection *redirect)
{
	s_tsh_path *updated_path = NULL;
	int tar_fd;

	if (NULL != redirect) {
		updated_path = commons_tsh_update_path_from(curr_state, redirect->path);
		if (NULL != updated_path) {
			tar_fd = updated_path->tar_fd;

			free(updated_path);
			close(tar_fd);
			return 0 < tar_fd;
		}
	}
	return FALSE;
}

int commons_tsh_is_tarball_involved(const s_tsh_state *curr_state, const s_command *cmd) {
	/* Checks in args. */
	for (int i = 0; i < cmd->nb_args; ++i) {
		if (commons_tsh_is_a_valid_tarball_path(curr_state, cmd->args[i])) {
			return TRUE;
		}
	}

	/* Checks in redirections. */
	if (NULL != cmd->redirections) {
		return
			commons_tsh_is_tarball_involved_in_redirection(curr_state, cmd->redirections->out)
			|| commons_tsh_is_tarball_involved_in_redirection(curr_state, cmd->redirections->err)
			|| commons_tsh_is_tarball_involved_in_redirection(curr_state, cmd->redirections->in);
	}

	return FALSE;
}

char *commons_tsh_create_full_path(const char *parent_path, const char *filename) {
	char *full_path;

	assert(NULL != (full_path = malloc(strlen(parent_path) + strlen(filename))));

	commons_safe_str_cpy(full_path, parent_path);
	strcat(full_path, filename);

	return full_path;
}

int commons_tsh_is_a_valid_tarball_path(const s_tsh_state *curr_state, const char *path) {
	s_tsh_path *updated_path = commons_tsh_update_path_from(curr_state, path);

	if (NULL != updated_path && 0 < updated_path->tar_fd) {
		free(updated_path);
		return TRUE;
	}
	if (NULL != updated_path) {
		free(updated_path);
	}
	return FALSE;
}

s_tsh_path *commons_tsh_update_path_from(const s_tsh_state *curr_state, const char *path) {
	s_tsh_state state_cpy;

	commons_tsh_copy_state(curr_state, &state_cpy);
	if (ERR == commons_tsh_update_state_from_path(&state_cpy, strdup(path))) {
		// Path not leading to a valid dir.
		char *parent_path = commons_tar_get_parent_path(path);

		if (ERR == commons_tsh_update_state_from_path(&state_cpy, parent_path)) {
			// Path not leading to a valid parent dir.
			return NULL;
		}
		else if (FD_NOTDEF == state_cpy.cwd->tar_fd) {
			// Path leading to a valid outside parent dir.
			strcat(state_cpy.cwd->outside_path, basename(strdup(path)));
		}
		else {
			// Path leading to a valid inside parent dir.
			strcat(state_cpy.cwd->inside_path, basename(strdup(path)));
		}
	}

	return state_cpy.cwd;
}

s_command *create_s_command(
	const char *cmd,
	const int nb_args,
	char *args[],
	const int is_with_outside_option,
	const int is_with_tarball_option,
	s_command_redirections *redirections
) {
	s_command *s_cmd;

	assert(NULL != (s_cmd = malloc(sizeof(s_command))));

	commons_safe_str_cpy(s_cmd->cmd, cmd);
	s_cmd->nb_args = nb_args;

	/* Copies arguments. */
	assert(NULL != (s_cmd->args = malloc(sizeof(char*)*nb_args)));
	if (nb_args > 0) {
		for (int i = 0; i < nb_args; i++) {
			assert(NULL != (s_cmd->args[i] = malloc(strlen(args[i]))));
			commons_safe_str_cpy(s_cmd->args[i], args[i]);
		}
	}

	s_cmd->is_with_outside_option = is_with_outside_option;
	s_cmd->is_with_tarball_option = is_with_tarball_option;
	s_cmd->redirections = redirections;

	return s_cmd;
}

int commons_tsh_is_path_self_dir(const char *path, const int value) {
	char *parsed_path[PATH_MAX];
	int nb_parsed_dir = 0;

	commons_tsh_parse_path(parsed_path, &nb_parsed_dir, path);

	if (FIRST == value) {
		return 0 == strcmp(PATH_TO_SELF_DIR, parsed_path[0]);
	}

	return 0 == strcmp(PATH_TO_SELF_DIR, parsed_path[nb_parsed_dir - 1]);
}
