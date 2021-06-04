#include "inc/cattar.h"

static int read_and_write(
    const int tar_fd,
    char *buffer,
    const ssize_t total_read_size,
    const unsigned int filesize
) {
    ssize_t read_size = 0;

	read_size = read(tar_fd, buffer, TAR_BLOCKSIZE);

	if (ERR == read_size) {
		return ERR;
	}
	if (read_size > filesize - total_read_size) {
		read_size = filesize - total_read_size;
	}
	if (ERR == write(STDOUT_FILENO, buffer, read_size)) {
		return ERR;
	}

	return read_size;
}

return_handler cattar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd) {
	s_tsh_path *updated_path;
	return_handler rh;

    for (int i = 0; i < cmd->nb_args; ++i) {
        updated_path = commons_tsh_update_path_from(curr_state, cmd->args[i]);

        if (NULL != updated_path && FD_NOTDEF != updated_path->tar_fd) {
			if (commons_tsh_is_path_self_dir(cmd->args[i], LAST)) {
				return create_error_return_handler(ERR_CATTAR, ERR_DIRECTORY, cmd->args[i]);
        	}

			rh = cattar(updated_path->tar_fd, updated_path->inside_path);
            if (ERR == rh.verdict) {
				return rh;
			}

            if (cmd->nb_args - 1 > i) {
				write(STDOUT_FILENO, "\n", 1);
			}
            free(updated_path);
        }
        else {
            return create_error_return_handler(ERR_CATTAR, ERR_UPD_INVALID, "");
        }
    }
    return create_successful_return_handler();
}

return_handler cattar(const int tar_fd, const char *path_from_tar_root) {
	s_posix_header pheader;
	char read_result[TAR_BLOCKSIZE];
	unsigned int filesize;
	ssize_t total_read_size;
	ssize_t size;

	total_read_size = 0;

    if (!commons_tar_find_file_from(tar_fd, path_from_tar_root, &pheader)) {
		return create_error_return_handler(ERR_CATTAR, ERR_NO_FILE, path_from_tar_root);
	}

	if (TAR_DIR_F == pheader.typeflag) {
		return create_error_return_handler(ERR_CATTAR, ERR_DIRECTORY, path_from_tar_root);
	}

	if (TAR_HLINK_F == pheader.typeflag || TAR_SLINK_F == pheader.typeflag) {
		return cattar(tar_fd, pheader.linkname);
	}

	sscanf(pheader.size, "%o", &filesize);

	do {
		size = read_and_write(tar_fd, read_result, total_read_size, filesize);

		if (size == ERR) {
			return create_error_return_handler(ERR_CATTAR, ERR_RDWR, path_from_tar_root);
		}

		total_read_size += size;
	} while (total_read_size < filesize);

	lseek(STDOUT_FILENO, 0, SEEK_SET);

	return create_successful_return_handler();
}
