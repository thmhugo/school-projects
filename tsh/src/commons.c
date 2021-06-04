#include "inc/commons.h"
/* Don't know why but put this include in commons.h does not seem towork. */
#include "inc/commons_tar.h"


ssize_t commons_copy_file_content_from(
		const char *file_path,
		const ssize_t offset,
		const ssize_t buff_size,
		char *buffer
) {
	char filename[NAME_MAX];
	ssize_t new_offset = 0;
	int fd = open(file_path, O_RDONLY);

	if (ERR == fd) {
		/* If it's a symbolic link, try to open the symbolic link `filename`. */
		if (ERR == readlink(file_path, filename, NAME_MAX) || ERR == (fd = open(filename, O_RDONLY))) {
			return ERR;
		}
	}
	lseek(fd, offset, SEEK_SET);
	if (ERR == read(fd, buffer, BUFSIZ)) {
		close(fd);
		return ERR;
	}

	new_offset = lseek(fd, 0, SEEK_CUR);
	close(fd);
	return new_offset;
}

ssize_t commons_get_file_size(const int fd) {
	const ssize_t file_size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	return file_size;
}

char *commons_safe_str_cpy(char *dest, const char *src) {
	memset(dest, NONECHAR, strlen(dest));
	return strcpy(dest, src);
}

int is_str_contain_only_spaces(const char *str) {
	const ssize_t str_len = strlen(str);

	for (int i = 0; i < str_len; ++i) {
		if (!isspace(str[i])) {
			return FALSE;
		}
	}

	return TRUE;
}

int commons_is_dir(const s_tsh_path path) {
	struct stat buffer_stat;

	if (ERR == stat(path.outside_path, &buffer_stat)) {
		return FALSE;
	}

	return S_ISDIR(buffer_stat.st_mode);
}

int commons_is_file(const s_tsh_path path) {
	struct stat buffer_stat;

	if (ERR == stat(path.outside_path, &buffer_stat)) {
		return FALSE;
	}

	return S_ISREG(buffer_stat.st_mode);
}

int commons_remove_file(const char *file_path) {
	struct stat buffer_stat;
	struct dirent *entry;
	char path[PATH_MAX + 1];
	DIR *dirp;

	/* Handles first the case where the file is not a folder. */
	if (ERR == stat(file_path, &buffer_stat)) {
		return ERR;
	}

	if (!S_ISDIR(buffer_stat.st_mode)) {
		return unlink(file_path);
	}

	dirp = opendir(file_path);

	if (NULL == dirp) {
		return ERR;
	}

	while ((entry = readdir(dirp))) {
		if (0 == strcmp(".", entry->d_name) || 0 == strcmp("..", entry->d_name)) {
			continue;
		}

		sprintf(path, "%s/%s", file_path, entry->d_name);

		if (DT_DIR == entry->d_type) {
			if (ERR == commons_remove_file(path)) {
				closedir(dirp);
				return ERR;
			}
		}
		else {
			if (ERR == unlink(path)) {
				closedir(dirp);
				return ERR;
			}
		}
	}

	closedir(dirp);

	if (ERR == rmdir(file_path)) {
		return ERR;
	}

	return OK;
}

void split_dirs_and_file(char *path, char **dirs_without_file, char **file) {
	int depth = commons_tar_get_depth(path);

	commons_safe_str_cpy(
		*dirs_without_file,
		commons_tar_format_name_according_max_depth(
			path,
			depth-1
		)
	);

	commons_safe_str_cpy(
		*file,
		commons_tar_format_name_according_min_depth(
			path,
			commons_tar_get_depth(path)-1
		)
	);
}

char *commons_remove_last_char(const char *str) {
	return strndup(str, strlen(str)-1);
}

int commons_operands_are_missing(const s_command *cmd) {
	return 0 == cmd->nb_args || 0 == strcmp(cmd->args[0], "");
}
