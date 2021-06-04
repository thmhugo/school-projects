#include "inc/cptar.h"
/* Writes the content from a tarbal to a tarball
 * (can be the same). */
static return_handler cptar_from_tarball_to_tarball(
	s_tsh_path source_path,
	s_tsh_path destination_path
);

/* Returns true if the source_path corresponds to a file and the
 * destination_path corresponds to a directory. */
static int is_from_file_to_dir(
	s_tsh_path source_path,
	s_tsh_path destination_path
);

/* Manages the file descriptor before calling cptar_to_another_file. */
static return_handler cptar_outside(
	s_tsh_path source_path,
	s_tsh_path destination_path,
	s_posix_header pheader
);

/* Writes the content to another file. */
static return_handler cptar_to_another_file(
	const int src_fd,
	const int dest_fd,
	s_posix_header pheader
);

/* Manages cp cases without option. */
static return_handler cptar_without_option(
	const s_tsh_path source_path,
	const s_tsh_path destination_path
);

/* Manages cp cases with option. */
static return_handler cptar_with_option(
	const s_tsh_path source_path,
	const s_tsh_path destination_path
);

/* Creates the new desination file name. */
static char* create_new_destination_name(
	char *new_file_name_root,
	const char *current_file_inside_path,
	char *header_name
);

/* Iterates over a tarball to read files and copy if the path corresponds. */
static return_handler cptar_children(
	const s_tsh_path source_path,
	const s_tsh_path destination_path,
	s_posix_header pheader,
	const int to_outside
);

/* Factorisation of the call to functions that create dir or file into a tarball. */
static return_handler cptar_file_or_dir_into_tarball(
	const s_tsh_path source_path,
	const s_tsh_path original_destination_path,
	char *new_destination_file_name,
	s_posix_header pheader
);

/* Factorisation of the call to functions that create dir or file outside a tarball. */
static return_handler cptar_file_or_dir_outside(
	const s_tsh_path source_path,
	const s_tsh_path original_destination_path,
	char *new_destination_file_name,
	s_posix_header pheader
);

/* Recursively explores a directory to copy it completely into a tarball. */
static return_handler read_dir_and_tar_append(
	const s_tsh_path source_path,
	const s_tsh_path destination_path,
	s_posix_header pheader
);

/* Creates the file intermediate names, used when exploring a directory. */
static void create_intermediate_name(char *new_path, const char *root, char *end);

/* Get the intermediates source and destination file names
 * makes the copy when running a tarball.
 */
static return_handler cptar_internal_tarball_content(
	const s_tsh_path source_path,
	const s_tsh_path destination_path,
	char *new_file_name_root,
	s_posix_header pheader,
	const int to_outside
);

/* Used when destination is at the tar root for cp with option :
 * put the source file name at the beginning of the destination name.
 */
static void update_destination_name_with_source_name(
	s_tsh_path source_path,
	s_tsh_path *destination_path
);

/* Verifies if the source path is a dir or not (in a tarball or outside). */
static int file_is_a_dir(const s_tsh_path source_path, s_posix_header pheader);

/* Returns TRUE is the given path is ended by "/". */
static int ends_by_path_delimiter_char(const char *path);

return_handler cptar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd) {
	s_tsh_path *src_updated_path;
	s_tsh_path *dest_updated_path;
	return_handler rh;

	if (2 > cmd->nb_args) {
		return create_error_return_handler(
			 ERR_CPTAR, ERR_MANY_ARGS, curr_state->cwd->inside_path
		);
	}

	/* Get the s_tsh_path representations of the destination path. */
	dest_updated_path =	commons_tsh_update_path_from(
							curr_state,
							cmd->args[cmd->nb_args-1]
						);

	if (NULL == dest_updated_path) {
		return create_error_return_handler(ERR_CPTAR, ERR_UPD_INVALID, "");
	}

    for (int i = 0; i < cmd->nb_args - 1; ++i) {
        src_updated_path = commons_tsh_update_path_from(curr_state, cmd->args[i]);
        if (NULL != src_updated_path) {
			/* Case where the source is the self directory and the target is in the source. */
			if (commons_tsh_is_path_self_dir(cmd->args[i], FIRST)
				&& commons_tar_is_same_ancestor(
					src_updated_path->tar_fd,
					src_updated_path->inside_path,
					dest_updated_path->inside_path
				)) {

				free(src_updated_path);
				if (cmd->is_with_tarball_option) {
					return create_error_return_handler(ERR_CPTAR_CNT_CPY, "", cmd->args[i]);
				}

				return create_error_return_handler(
					ERR_CPTAR_NT_SPEC, ERR_OMIT_DIR, cmd->args[i]
				);
			}

			/* Case where the last dir in the source path is self dir. */
			if (2 < strlen(cmd->args[i]) && commons_tsh_is_path_self_dir(cmd->args[i], LAST)) {
				free(dest_updated_path);
				return create_error_return_handler(ERR_CPTAR_NT_FOUND, ERR_NOT_DIR, cmd->args[i]);
			}

			rh = cptar(*src_updated_path, *dest_updated_path, cmd->is_with_tarball_option);

            if (ERR == rh.verdict) {
				free(src_updated_path);
				return rh;
			}
			free(src_updated_path);
        }
        else {
			free(dest_updated_path);
            return create_error_return_handler(ERR_CPTAR, ERR_UPD_INVALID, "");
        }
    }
	free(dest_updated_path);
	return create_successful_return_handler();
}

return_handler cptar(
	const s_tsh_path source_path,
	const s_tsh_path destination_path,
	const int with_option
) {
	if (with_option) {
		return cptar_with_option(source_path, destination_path);
	}

	return cptar_without_option(source_path, destination_path);
}

static return_handler cptar_without_option(
	const s_tsh_path source_path,
	s_tsh_path destination_path
) {
	s_posix_header pheader;

	if ((commons_tar_is_valid(source_path.tar_fd)
		&& !commons_tar_find_file_from(source_path.tar_fd, source_path.inside_path, &pheader)))
	{
		return create_error_return_handler(
			ERR_CPTAR_NT_FOUND, ERR_NO_FILE, source_path.inside_path
		);
	}

	if (commons_tar_is_dir(pheader)) {
		return create_error_return_handler(
			ERR_CPTAR_NT_SPEC, ERR_OMIT_DIR, source_path.inside_path
		);
	}

	/* Case : from a tarball to a tarball. */
	if (commons_tar_is_valid(source_path.tar_fd)
		&& commons_tar_is_valid(destination_path.tar_fd))
	{
		return cptar_from_tarball_to_tarball(
			source_path,
			destination_path
		);
	}

	/* Case : from a tarball to a file outside. */
	if (commons_tar_is_valid(source_path.tar_fd)
		&& !commons_tar_is_valid(destination_path.tar_fd))
	{
		return cptar_outside(source_path, destination_path, pheader);
	}

	/* Case : from a file outside to a tarball. */
	if (commons_is_file(source_path)) {
		if (is_from_file_to_dir(source_path, destination_path)) {
			strcat(destination_path.inside_path,
					commons_tar_format_name_according_min_depth(
						source_path.outside_path,
						commons_tar_get_depth(source_path.outside_path)-1
					)
			);
		}
		return commons_tar_append_extern_file_to(
			source_path.outside_path,
			destination_path.tar_fd,
			destination_path.inside_path);
	}

	if (!commons_is_dir(source_path)) {
		return create_error_return_handler(
			ERR_CPTAR, ERR_NO_FILE, source_path.outside_path
		);
	}

	return create_error_return_handler(
		ERR_CPTAR_NT_SPEC, ERR_DIRECTORY, source_path.outside_path
	);
}

static return_handler cptar_with_option(
	const s_tsh_path source_path,
	s_tsh_path destination_path
) {
	s_posix_header pheader;
	return_handler rh;

	if (file_is_a_dir(source_path, pheader)) {
		return cptar_without_option(source_path, destination_path);
	}

    /* Case : from a tarball to a tarball. */
	if (commons_tar_is_valid(source_path.tar_fd)
		&& commons_tar_is_valid(destination_path.tar_fd))
	{
		return cptar_children(source_path, destination_path, pheader, FALSE);
	}

	/* Case : from a tarball to a file outside. */
	if (commons_tar_is_valid(source_path.tar_fd)
		&& !commons_tar_is_valid(destination_path.tar_fd))
	{
		return cptar_children(source_path, destination_path, pheader, TRUE);
	}

	ssize_t inside_path_len = strlen(destination_path.inside_path);

	/* Append '/' if needed. */
	if (!ends_by_path_delimiter_char(destination_path.inside_path)) {
		destination_path.inside_path[inside_path_len] = PATH_DELIMITER_CHR;
	}

	/* Case : from outside into a tarball
	 * Need to append the source directory before read_dir_and_tar_append,
	 * because read_dir_and_tar_append goes directly into the directory.
	 */
	/* If the destination path is the tar root, we need to add the source name
	 * at the beginning of the destination name. */
	if (!strcmp(PATH_DELIMITER_STR, destination_path.inside_path)) {
		update_destination_name_with_source_name(
			source_path,
			&destination_path
		);
	}
	if (ERR == (rh = commons_tar_append_file(
		destination_path.tar_fd, source_path, destination_path.inside_path)).verdict)
	{
		return rh;
	}
	return read_dir_and_tar_append(source_path, destination_path, pheader);
}

static void update_destination_name_with_source_name(
	s_tsh_path source_path,
	s_tsh_path *destination_path
) {
	commons_safe_str_cpy(
			destination_path->inside_path,
			commons_tar_format_name_according_min_depth(
					source_path.outside_path,
					commons_tar_get_depth(source_path.outside_path)-1
		));
}

static return_handler cptar_children(
	const s_tsh_path source_path,
	const s_tsh_path destination_path,
	s_posix_header pheader,
	const int to_outside
) {
	char new_file_name_root[TAR_NAME_SIZE];
	return_handler rh;

	if (to_outside) {
		commons_safe_str_cpy(new_file_name_root, destination_path.outside_path);
	}
	else {
		commons_safe_str_cpy(new_file_name_root, destination_path.inside_path);
	}

	lseek(source_path.tar_fd, 0, SEEK_SET);

	while (0 < read(source_path.tar_fd, &pheader, TAR_BLOCKSIZE) &&  NONECHAR != pheader.name[0]) {
		/* If the read header has source_path.inside_path as ancestor : it is copied. */
		if (
			commons_tar_is_same_ancestor(
				source_path.tar_fd,
				source_path.inside_path,
				pheader.name))
		{
			const off_t saved_offset = lseek(source_path.tar_fd, 0, SEEK_CUR);

			if (ERR == (rh = cptar_internal_tarball_content(
				source_path,
				destination_path,
				new_file_name_root,
				pheader,
				to_outside
			)).verdict) {
				return rh;
			}

			lseek(source_path.tar_fd, saved_offset, SEEK_SET);
		}

		commons_tar_lseek_size_of_file(source_path.tar_fd, &pheader);
	}
	return create_successful_return_handler();
}

static return_handler cptar_internal_tarball_content(
	const s_tsh_path source_path,
	const s_tsh_path destination_path,
	char *new_file_name_root,
	s_posix_header pheader,
	const int to_outside
) {
	char *new_destination_file_name;
	s_tsh_path *new_source_path;
	return_handler rh;

	/* Creation of the new file destination name and associated tsh_path. */
	new_destination_file_name =
		create_new_destination_name(
			new_file_name_root,
			source_path.inside_path,
			pheader.name
		);

	/**
 	 * The given s_tsh_path corresponds to the source directory.
	 * As we iterate of all files, we need to get the tsh_path of
	 * the copied file or directory.
	 */
	new_source_path =
		commons_tsh_create_s_tsh_path(
			source_path.outside_path,
			pheader.name,
			source_path.tar_fd);

	if (to_outside) {
		if (ERR == (rh = cptar_file_or_dir_outside(
			*new_source_path,
			destination_path,
			new_destination_file_name,
			pheader
		)).verdict) {
			return rh;
		}
	}
	else {
		if (ERR == (rh = cptar_file_or_dir_into_tarball(
			*new_source_path,
			destination_path,
			new_destination_file_name,
			pheader
		)).verdict) {
			return rh;
		}
	}
	free(new_source_path);
	return create_successful_return_handler();
}

static return_handler read_dir_and_tar_append(
	const s_tsh_path source_path,
	const s_tsh_path destination_path,
	s_posix_header pheader
) {
	char new_source_file_name[PATH_MAX];
	char new_destination_file_name[TAR_NAME_SIZE];
	s_tsh_path *new_source_path;
	struct dirent *entry;
	DIR *dirp;
	return_handler rh;

	if (!commons_is_dir(source_path)) {
		/* The source is not a directory. */
		return commons_tar_append_extern_file_to(
			source_path.outside_path,
			destination_path.tar_fd,
			destination_path.inside_path
		);
	}

	if (NULL == (dirp = opendir(source_path.outside_path))) {
		return create_error_return_handler(
			ERR_CPTAR_NT_FOUND, ERR_NO_FILE, source_path.outside_path
		);
	}

	while ((entry = readdir(dirp))) {
		if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)) {
			continue;
		}

		if (DT_DIR == entry->d_type) {
			strcat(entry->d_name, "/");
		}

		create_intermediate_name(new_destination_file_name,  destination_path.inside_path, entry->d_name);
		create_intermediate_name(new_source_file_name,  source_path.outside_path, entry->d_name);

		new_source_path =
			commons_tsh_create_s_tsh_path(
				new_source_file_name,
				source_path.inside_path,
				source_path.tar_fd
		);

		if (ERR == (rh = cptar_file_or_dir_into_tarball(
			*new_source_path,
			destination_path,
			new_destination_file_name,
			pheader
		)).verdict) {
			closedir(dirp);
			return rh;
		}

		/* Recursive call inside a directory. */
		if (DT_DIR == entry->d_type) {
			if (ERR == (rh = read_dir_and_tar_append(
						*new_source_path,
						*commons_tsh_create_s_tsh_path(
							destination_path.outside_path,
							new_destination_file_name,
							destination_path.tar_fd),
						pheader)).verdict)
			{
				closedir(dirp);
				return rh;
			}
		}
	}

	closedir(dirp);
	return create_successful_return_handler();
}

static return_handler cptar_file_or_dir_outside(
	const s_tsh_path source_path,
	const s_tsh_path original_destination_path,
	char *new_destination_file_name,
	s_posix_header pheader
) {
	s_tsh_path *new_destination_path;
	return_handler rh;

	new_destination_path =
		commons_tsh_create_s_tsh_path(
			new_destination_file_name,
			"",
			FD_NOTDEF
		);

	if (commons_tar_is_dir(pheader)) {
		mkdir(new_destination_file_name,  S_IRWXU);
	}
	else {
		if (ERR == (rh = cptar_outside(source_path, *new_destination_path, pheader)).verdict) {
			return rh;
		}
	}
	free(new_destination_path);

	return create_successful_return_handler();
}

static return_handler cptar_file_or_dir_into_tarball(
	const s_tsh_path source_path,
	const s_tsh_path original_destination_path,
	char *new_destination_file_name,
	s_posix_header pheader
) {
	s_tsh_path *new_destination_path;
	return_handler rh;

	new_destination_path =
		commons_tsh_create_s_tsh_path(
			original_destination_path.outside_path,
			new_destination_file_name,
			original_destination_path.tar_fd
	);

	if (commons_tar_is_dir(pheader)) {
		mkdirtar(new_destination_path->tar_fd, new_destination_file_name);
	}
	else {
		rh = cptar_from_tarball_to_tarball(source_path, *new_destination_path);
	}

	free(new_destination_path);

	return rh;
}

static return_handler cptar_outside(
	s_tsh_path source_path,
	s_tsh_path destination_path,
	s_posix_header pheader
) {
	int destination_fd;
	return_handler cptar_return;

	if (is_from_file_to_dir(source_path, destination_path)) {
		/* Corresponds to "cp file folder/" :
		 * need to transform the destination name "folder/" into "folder/file". */
		strcat(destination_path.outside_path,
				commons_tar_format_name_according_min_depth(
					source_path.inside_path,
					commons_tar_get_depth(source_path.inside_path)-1
				)
		);
	}

	if (ERR == (destination_fd = open(destination_path.outside_path, O_RDWR | O_CREAT, S_IRWXU))) {
		return create_error_return_handler(
			ERR_CPTAR_NT_OVWR_F, "with directory", destination_path.outside_path
		);
	}

	cptar_return =
		cptar_to_another_file(
			source_path.tar_fd,
			destination_fd,
			pheader
		);

	close(destination_fd);
	return cptar_return;
}

static int ends_by_path_delimiter_char(const char *path) {
	return PATH_DELIMITER_CHR == path[strlen(path)-1];
}

static int is_from_file_to_dir(
	const s_tsh_path source_path,
	const s_tsh_path destination_path
) {
	int is_from_file_to_dir_to_outside;
	int is_from_file_to_dir_to_inside;

	is_from_file_to_dir_to_outside =
		commons_is_dir(destination_path)
		&& !ends_by_path_delimiter_char(source_path.inside_path);

	is_from_file_to_dir_to_inside =
		(ends_by_path_delimiter_char(destination_path.inside_path)
		/* destination path is a tar root*/
		|| (!strcmp(destination_path.inside_path, "")
			&& FD_NOTDEF != destination_path.tar_fd))
		&& ((commons_is_file(source_path))
			|| !ends_by_path_delimiter_char(source_path.inside_path));

	return is_from_file_to_dir_to_outside
		|| is_from_file_to_dir_to_inside;
}

static return_handler cptar_from_tarball_to_tarball(
	const s_tsh_path source_path,
	s_tsh_path destination_path
) {
	int tmp_destination_tar_fd;
	return_handler append_return_value;

	if (is_from_file_to_dir(source_path, destination_path)) {
		/* Corresponds to "cp file folder/" :
		 * need to transform the destination name "folder/" into "folder/file". */
		strcat(destination_path.inside_path,
				commons_tar_format_name_according_min_depth(
					source_path.inside_path,
					commons_tar_get_depth(source_path.inside_path)-1
				)
		);
	}

	if (source_path.tar_fd == destination_path.tar_fd) {
		char *tar_path;

		/* Removes last '/'. */
		if (ends_by_path_delimiter_char(destination_path.outside_path)) {
			tar_path = strndup(
					destination_path.outside_path,
					strlen(destination_path.outside_path)-1
			);
		}
		else {
			tar_path = strdup(destination_path.outside_path);
		}
		/* Try to open the tarball in order to get a new offset at the begining. */
		tmp_destination_tar_fd = open(tar_path, O_RDWR);
		if (ERR == tmp_destination_tar_fd) {
			return create_error_return_handler(
				ERR_CPTAR, ERR_OPENING, destination_path.outside_path
			);
		}
	}
	else {
		tmp_destination_tar_fd = destination_path.tar_fd;
	}

	commons_tar_lseek_to_the_end(tmp_destination_tar_fd);

	append_return_value =
		commons_tar_append_file(
			tmp_destination_tar_fd,
			source_path,
			destination_path.inside_path
	);

	if (source_path.tar_fd == destination_path.tar_fd) {
		close(tmp_destination_tar_fd);
	}

	return append_return_value;
}

static return_handler cptar_to_another_file(
	const int src_fd,
	const int dest_fd,
	s_posix_header pheader
) {
	char buffer[TAR_BLOCKSIZE];
	ssize_t read_size;
	ssize_t write_size;

	int nb_source_blocks = commons_tar_get_content_nb_blocks(pheader);
	while (0 < nb_source_blocks--) {
		memset(buffer, NONECHAR, TAR_BLOCKSIZE);
		read_size = read(src_fd, buffer, TAR_BLOCKSIZE);
		write_size = write(dest_fd, buffer, TAR_BLOCKSIZE);

		if (read_size != write_size) {
			return create_error_return_handler(ERR_CPTAR, ERR_RDWR, "");
		}
	}

	return create_successful_return_handler();
}

static char* create_new_destination_name(
	char *new_file_name_root,
	const char *current_file_inside_path,
	char *header_name
) {
	char *new_destination_file_name;
	char *new_file_name_end =
		commons_tar_format_name_according_min_depth(
			header_name,
			commons_tar_get_depth(
				current_file_inside_path
			)
		);

	assert(NULL != (new_destination_file_name = malloc(TAR_NAME_SIZE)));
	create_intermediate_name(new_destination_file_name, new_file_name_root, new_file_name_end);

	return new_destination_file_name;
}

static void create_intermediate_name(char *new_path, const char *root, char *end) {
	commons_safe_str_cpy(new_path, root);
	if (!ends_by_path_delimiter_char(new_path)) {
		strcat(new_path, PATH_DELIMITER_STR);
	}
	strcat(new_path, end);
}

static int file_is_a_dir(const s_tsh_path source_path, s_posix_header pheader) {
  const int fd = source_path.tar_fd;
  const char *path = source_path.inside_path;

  return (commons_tar_is_valid(fd) &&
          !commons_tar_find_dir_from(fd, path, &pheader)) ||
         (!commons_tar_is_valid(fd) && !commons_is_dir(source_path));
}
