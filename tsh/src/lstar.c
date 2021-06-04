#include "inc/lstar.h"

static void lstar_init(
		const int tar_fd,
		const char *path_from_tar_root,
		const int path_depth
);
static void lstar_update_global_variables(const s_posix_header pheader);
static void lstar_update_hashmap_nblinks(const s_posix_header *pheader);
static void lstar_update_g_nbfiles_with_missing_dirs(const char *path_from_tar_root);
static void lstar_write(
		const int with_option,
		const int path_depth,
		const s_lstar_hm_item *item
);
static void lstar_simple_write(const char *str);
static void lstar_write_total_size(void);
static void lstar_write_with_details(
		const char *formatted_name,
		const s_lstar_hm_item *item
);
static void lstar_write_posix_header_flag(char line_buffer[], const char flag);
static void lstar_write_posix_header_mode(
		char line_buffer[],
		int *buffer_pos,
		const char *mode
);
static void lstar_write_posix_header_nb_links(
		char line_buffer[],
		int *buffer_pos,
		int nb_links
);
static void lstar_write_formatted_owner_name(
		char line_buffer[],
		int *buffer_pos,
		const char *name,
		const unsigned int max_name_length
);
static void lstar_write_posix_header_size(
		char line_buffer[],
		int *buffer_pos,
		const s_posix_header *pheader,
		const s_posix_header *linkfile_pheader
);
static void lstar_write_posix_header_mtime(
		char line_buffer[],
		int *buffer_pos,
		const char *mtime
);
static void lstar_write_formatted_file_name(
		char line_buffer[],
		int *buffer_pos,
		const char *name
);
static void lstar_write_posix_header_linkname(
		char line_buffer[],
		int *buffer_pos,
		const s_posix_header *pheader
);
static void lstar_free_hashmap(void);

static char *lstar_get_rights(const char mode_decimal);


static s_lstar_hm_item  *g_lstar_hashmap = NULL;
static size_t            g_max_file_size = 1;
static size_t            g_max_user_name_length = 1;
static size_t            g_max_group_name_length = 1;
static int               g_nb_files = 0;

return_handler lstar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd) {
	s_tsh_path *updated_path;
	return_handler rh;
	s_posix_header pheader;

    for (int i = 0; i < cmd->nb_args; ++i) {
        updated_path = commons_tsh_update_path_from(curr_state, cmd->args[i]);
        if (NULL != updated_path && 0 < updated_path->tar_fd) {
			if (1 < cmd->nb_args &&
				commons_tar_find_dir_from(updated_path->tar_fd, updated_path->inside_path, &pheader)
			) {
				/* Prints args when there is at least two arguments. */
				char name[strlen(cmd->args[i]) + 2];
				strcpy(name, cmd->args[i]);
				strcat(name, ":\n");
				write(STDOUT_FILENO, name, strlen(name));
			}

			rh = lstar(updated_path->tar_fd, cmd->is_with_tarball_option, updated_path->inside_path);
            if (ERR == rh.verdict) {
				return rh;
			}

			free(updated_path);
			if (cmd->nb_args - 1 > i) {
				write(STDOUT_FILENO, "\n", 1);
			}
        }
        else {
            return create_error_return_handler(ERR_LSTAR, ERR_UPD_INVALID, "");
        }
    }
    return create_successful_return_handler();
}

return_handler lstar(const int tar_fd, const int with_option, const char *path_from_tar_root) {
	const int path_depth = commons_tar_get_depth(path_from_tar_root);
	s_posix_header pheader;
	s_lstar_hm_item *tmp = NULL;
	s_lstar_hm_item *item = NULL;

	if (0 < strlen(path_from_tar_root)) {
		if (!commons_tar_find_dir_from(tar_fd, path_from_tar_root, &pheader)) {
			return create_error_return_handler(ERR_LSTAR, ERR_NO_FILE, path_from_tar_root);
		}
	}

	lseek(tar_fd, 0, SEEK_SET);

	lstar_init(tar_fd, path_from_tar_root, path_depth);

	if (with_option) {
		/* @note lstar total size is not always accurates, we don't really know why... */
		lstar_write_total_size();
	}

	HASH_ITER(hh, g_lstar_hashmap, item, tmp) {
		if (0 == strcmp(commons_tar_get_parent_path(item->name), path_from_tar_root)) {
			lstar_write(
					with_option,
					path_depth,
					item
			);
		}
	}
	if (!with_option && !commons_tar_is_dir_empty(tar_fd, path_from_tar_root)) {
		write(STDOUT_FILENO, "\n", 1);
	}

	lstar_free_hashmap();

	return create_successful_return_handler();
}

static void lstar_init(
		const int tar_fd,
		const char *path_from_tar_root,
		const int path_depth
) {
	s_posix_header pheader;
	s_lstar_hm_item *hm_item = NULL;

	while (0 < read(tar_fd, &pheader, TAR_BLOCKSIZE) && NONECHAR != pheader.name[0]) {
		const char *parent_path = commons_tar_get_parent_path(pheader.name);

		HASH_FIND_STR(g_lstar_hashmap, pheader.name, hm_item);

		if (0 == strcmp(parent_path, path_from_tar_root) && NULL == hm_item) {
			lstar_update_global_variables(pheader);
		}
		/* Add a new item in `g_lstar_hashmap` according `pheader`.
		 *
		 * NOTE:
		 *     When putting following code into a static function,
		 *     `memcpy(hm_item->pheader, &pheader, TAR_BLOCKSIZE)` doesn't work.
		 */
		if (NULL == hm_item) {
			assert(NULL != (hm_item = malloc(sizeof(s_lstar_hm_item))));
			assert(NULL != (hm_item->pheader = malloc(TAR_BLOCKSIZE)));

			memcpy(hm_item->pheader, &pheader, TAR_BLOCKSIZE);
			strcpy(hm_item->name, pheader.name);
			hm_item->nb_links = 0;

			HASH_ADD_STR(g_lstar_hashmap, name, hm_item);
		}
		/* A subdir was read before or duplicated file. */
		else {
			assert(NULL != (hm_item->pheader = malloc(TAR_BLOCKSIZE)));
			memcpy(hm_item->pheader, &pheader, TAR_BLOCKSIZE);
		}
		lstar_update_hashmap_nblinks(&pheader);
		commons_tar_lseek_size_of_file(tar_fd, &pheader);
	}
	/* NOTE: Maybe, there is a way to avoid to iterate through all the hashmap in order
	 * to find missing dirs. */
	lstar_update_g_nbfiles_with_missing_dirs(path_from_tar_root);

	lseek(tar_fd, 0, SEEK_SET);
}

static void lstar_update_global_variables(const s_posix_header pheader) {
	unsigned int curr_size = 0;

	if (TAR_DIR_F == pheader.typeflag) {
		curr_size = LSTAR_DEFAULT_DIR_SIZE;
	}
	else {
		sscanf(pheader.size, "%o", &curr_size);
	}

	if (g_max_file_size < curr_size) {
		g_max_file_size = curr_size;
	}

	g_nb_files += (TAR_SLINK_F == pheader.typeflag) ? 0 : 1;

	if (g_max_user_name_length < strlen(pheader.uname)) {
		g_max_user_name_length = strlen(pheader.uname);
	}
	if (g_max_group_name_length < strlen(pheader.gname)) {
		g_max_group_name_length = strlen(pheader.gname);
	}
}

static void lstar_update_hashmap_nblinks(const s_posix_header *pheader) {
	s_lstar_hm_item *hm_item = NULL;

	if (TAR_HLINK_F == pheader->typeflag || TAR_DIR_F == pheader->typeflag) {
		const char *parent_dir_name = commons_tar_format_name_according_max_depth(
											pheader->name,
											commons_tar_get_depth(pheader->name) - 1
										);
		if (TAR_DIR_F != pheader->typeflag || 0 != strlen(parent_dir_name)) {
			const char *key_name =
				TAR_HLINK_F == pheader->typeflag ? pheader->linkname : parent_dir_name;

			HASH_FIND_STR(g_lstar_hashmap, key_name, hm_item);

			if (NULL != hm_item) {
				hm_item->nb_links++;
			}
			else {
				assert(NULL != (hm_item = malloc(sizeof(s_lstar_hm_item))));

				strcpy(hm_item->name, key_name);
				hm_item->pheader = NULL;
				hm_item->nb_links = 1;

				HASH_ADD_STR(g_lstar_hashmap, name, hm_item);
			}
		}
	}
}

static void lstar_update_g_nbfiles_with_missing_dirs(const char *path_from_tar_root) {
	s_lstar_hm_item *hm_item = NULL;
	s_lstar_hm_item *tmp = NULL;

	HASH_ITER(hh, g_lstar_hashmap, hm_item, tmp) {
		const char *parent_path = commons_tar_get_parent_path(hm_item->name);
		if (0 == strcmp(parent_path, path_from_tar_root) && NULL == hm_item->pheader) {
			++g_nb_files;
		}
	}
}

static void lstar_write(
		const int with_option,
		const int path_depth,
		const s_lstar_hm_item *item
) {
	char *formatted_name = commons_tar_format_name_according_min_depth(
								item->name, path_depth
							);
	if (0 < strlen(formatted_name)) {
		if (with_option) {
			lstar_write_with_details(
					formatted_name,
					item
			);
		}
		else {
			lstar_simple_write(formatted_name);
		}
	}
}

static void lstar_simple_write(const char *str) {
	const int str_len = strlen(str);
	const int nb_char_to_cpy = '/' == str[str_len-1] ? str_len - 1 : str_len;

	write(STDOUT_FILENO, str, nb_char_to_cpy);
	write(STDOUT_FILENO, " ", 1);
}

static void lstar_write_total_size(void) {
	char first_line_buffer[NAME_MAX];

	strcpy(first_line_buffer, "total ");
	sprintf(first_line_buffer + 6, "%d", g_nb_files * 4);
	strcpy(first_line_buffer + strlen(first_line_buffer), "\n");
	write(STDOUT_FILENO, first_line_buffer, strlen(first_line_buffer));
}

static void lstar_write_with_details(
		const char *formatted_name,
		const s_lstar_hm_item *item
) {
	const s_posix_header *pheader = item->pheader;
	const int typeflag = NULL == pheader ? TAR_DIR_F : pheader->typeflag;
	const char *name_to_compare =
		TAR_HLINK_F == typeflag ? pheader->linkname : item->name;

	s_lstar_hm_item *linked_item = NULL;
	char line_buffer[NAME_MAX];
	int buffer_pos = 0;
	int nb_links = TAR_DIR_F == typeflag ? 2 : 1;

	lstar_write_posix_header_flag(line_buffer, typeflag);
	line_buffer[++buffer_pos] = ' ';
	lstar_write_posix_header_mode(
			line_buffer,
			&buffer_pos,
			NULL == pheader ? LSTAR_UNKNOWN_MODE : pheader->mode
	);
	line_buffer[buffer_pos++] = ' ';

	HASH_FIND_STR(g_lstar_hashmap, name_to_compare, linked_item);
	if (NULL != linked_item) {
		nb_links += linked_item->nb_links;
	}
	lstar_write_posix_header_nb_links(line_buffer, &buffer_pos, nb_links);
	line_buffer[buffer_pos++] = ' ';

	lstar_write_formatted_owner_name(
			line_buffer,
			&buffer_pos,
			NULL == pheader ? LSTAR_UNKNOWN_NAME : pheader->uname,
			g_max_user_name_length
	);
	line_buffer[buffer_pos++] = ' ';
	lstar_write_formatted_owner_name(
			line_buffer,
			&buffer_pos,
			NULL == pheader ? LSTAR_UNKNOWN_NAME : pheader->gname,
			g_max_group_name_length
	);
	line_buffer[buffer_pos++] = ' ';
	lstar_write_posix_header_size(
			line_buffer,
			&buffer_pos,
			pheader,
			NULL != linked_item ? linked_item->pheader : NULL
	);
	line_buffer[buffer_pos++] = ' ';
	lstar_write_posix_header_mtime(
			line_buffer,
			&buffer_pos,
			NULL == pheader ? NULL : pheader->mtime
	);
	line_buffer[buffer_pos++] = ' ';
	lstar_write_formatted_file_name(line_buffer, &buffer_pos, formatted_name);
	lstar_write_posix_header_linkname(line_buffer, &buffer_pos, pheader);
	line_buffer[buffer_pos++] = '\n';

	write(STDOUT_FILENO, line_buffer, buffer_pos);
}

static void lstar_write_posix_header_flag(char line_buffer[], const char flag) {
	switch (flag) {
		case TAR_SLINK_F:
			line_buffer[0] = TAR_SLINK_V;
		break;
		case TAR_SPECIALCHAR_F:
			line_buffer[0] = TAR_SPECIALCHAR_V;
		break;
		case TAR_SPECIALBLOC_F:
			line_buffer[0] = TAR_SPECIALBLOC_V;
		break;
		case TAR_DIR_F:
			line_buffer[0] = TAR_DIR_V;
		break;
		case TAR_FIFO_F:
			line_buffer[0] = TAR_FIFO_V;
		break;
		default:

			line_buffer[0] = TAR_NORMALFILE_V;
		break;
	}
}

static void lstar_write_posix_header_mode(
		char line_buffer[],
		int *buffer_pos,
		const char *mode
) {
	char *owner = lstar_get_rights(mode[4]);
	char *group = lstar_get_rights(mode[5]);
	char *all = lstar_get_rights(mode[6]);

	strcpy(line_buffer + *buffer_pos, owner);
	*buffer_pos = *buffer_pos + 3;
	strcpy(line_buffer + *buffer_pos, group);
	*buffer_pos = *buffer_pos + 3;
	strcpy(line_buffer + *buffer_pos, all);
	*buffer_pos = *buffer_pos + 3;
}

static void lstar_write_posix_header_nb_links(
		char line_buffer[],
		int *buffer_pos,
		int nb_links
) {
	char nb_links_char[NAME_MAX];

	sprintf(nb_links_char, "%d", nb_links);
	strcpy(line_buffer + *buffer_pos, nb_links_char);
	*buffer_pos = *buffer_pos + strlen(nb_links_char);
}

/* NOTE: it could have been smarter by using mode_decimal bits representation,
 * but we are running out of time. */
static char *lstar_get_rights(const char mode_decimal) {
	char *rights = NULL;

	switch (mode_decimal) {
		case '0':
			rights = "---";
		break;
		case '1':
			rights = "--x";
		break;
		case '2':
			rights = "-w-";
		break;
		case '3':
			rights = "-wx";
		break;
		case '4':
			rights = "r--";
		break;
		case '5':
			rights = "r-x";
		break;
		case '6':
			rights = "rw-";
		break;
		case '7':
			rights = "rwx";
		break;

		default:
			rights = LSTAR_UNKNOWN_MODE_S;
		break;
	}
	return rights;
}

static void lstar_write_formatted_owner_name(
		char line_buffer[],
		int *buffer_pos,
		const char *name,
		const unsigned int max_name_length
) {
	size_t name_len = strlen(name);

	strcpy(line_buffer + *buffer_pos, name);
	*buffer_pos = *buffer_pos + name_len;

	for (int i = 0; max_name_length - name_len > i; ++i) {
		line_buffer[*buffer_pos+i] = LSTAR_SPACE_CHAR;
	}
	*buffer_pos = *buffer_pos + max_name_length - name_len;
}

static void lstar_write_posix_header_size(
		char line_buffer[],
		int *buffer_pos,
		const s_posix_header *pheader,
		const s_posix_header *linkfile_pheader
) {
	const int typeflag = NULL == pheader ? TAR_DIR_F : pheader->typeflag;
	char size_char[NAME_MAX];
	char max_size_char[NAME_MAX];
	unsigned int size_int;
	unsigned int max_size_char_len;
	unsigned int size_char_len;

	switch (typeflag) {
		case TAR_DIR_F:
			sprintf(size_char, "%d", LSTAR_DEFAULT_DIR_SIZE);
		break;
		case TAR_HLINK_F:
			sscanf(linkfile_pheader->size, "%o", &size_int);
			sprintf(size_char, "%d", size_int);
		break;
		default:
			sscanf(pheader->size, "%o", &size_int);
			sprintf(size_char, "%d", size_int);
		break;
	}

	sprintf(max_size_char, "%ld", g_max_file_size);
	max_size_char_len = pheader != NULL ? strlen(max_size_char) : LSTAR_DEFAULT_DIR_LEN;
	size_char_len = strlen(size_char);
	for (int i = 0; i < max_size_char_len - size_char_len; ++i) {
		line_buffer[*buffer_pos] = ' ';
		*buffer_pos = *buffer_pos + 1;
	}
	strcpy(line_buffer + *buffer_pos, size_char);
	*buffer_pos = *buffer_pos + size_char_len;
}

static void lstar_write_posix_header_mtime(
		char line_buffer[],
		int *buffer_pos,
		const char *mtime_octal
) {
	char mtime_buff[NAME_MAX];
	char formatted_time[NAME_MAX];
	int time_int;
	time_t time_long;

	if (NULL != mtime_octal) {
		sscanf(mtime_octal, "%o", &time_int);
		time_long = time_int;
		strftime(mtime_buff, NAME_MAX, "%b %e %R", localtime(&time_long));
		strcpy(formatted_time, mtime_buff);
	}
	else {
		strcpy(formatted_time, LSTAR_UNKNOWN_TIME);
	}

	strcpy(line_buffer + *buffer_pos, formatted_time);

	*buffer_pos = *buffer_pos + strlen(formatted_time);
}

static void lstar_write_formatted_file_name(
		char line_buffer[],
		int *buffer_pos,
		const char *name
) {
	const int name_len = strlen(name);
	const int nb_char_to_cpy = '/' == name[name_len-1] ? name_len - 1 : name_len;

	strncpy(
		line_buffer + *buffer_pos, name, nb_char_to_cpy
	);
	*buffer_pos = *buffer_pos + nb_char_to_cpy;
}

static void lstar_write_posix_header_linkname(
		char line_buffer[],
		int *buffer_pos,
		const s_posix_header *pheader
) {
	if (NULL != pheader && TAR_SLINK_F == pheader->typeflag) {
		strcpy(line_buffer + *buffer_pos, " -> ");
		*buffer_pos = *buffer_pos + 4;
		strcpy(line_buffer + *buffer_pos, pheader->linkname);
		*buffer_pos = *buffer_pos + strlen(pheader->linkname);
	}
}

static void lstar_free_hashmap(void) {
	s_lstar_hm_item *tmp = NULL;
	s_lstar_hm_item *item = NULL;

	HASH_ITER(hh, g_lstar_hashmap, item, tmp) {
		HASH_DEL(g_lstar_hashmap, item);
		free(item);
	}
}
