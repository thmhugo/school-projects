#include "inc/rmtar.h"

/** Removes `path_from_tar_root` children. */
static return_handler rmtar_children(const int tar_fd, const char *path_from_tar_root);

return_handler rmtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd) {
    s_tsh_path *target_path;
	return_handler rh;

    if (commons_operands_are_missing(cmd)) {
        return create_error_return_handler(
            ERR_RMTAR, ERR_MISSING_OPERAND, ""
        );
    }

    for (int i = 0; i < cmd->nb_args; ++i) {
        target_path = commons_tsh_update_path_from(
                        curr_state,
                        cmd->args[i]
                    );

        if (NULL == target_path) {
            return create_error_return_handler(
                ERR_RMTAR, ERR_UPD_INVALID, ""
            );
        }

        if (commons_tsh_is_path_self_dir(cmd->args[i], LAST)) {
            return cmd->is_with_tarball_option ?
                create_error_return_handler(ERR_RMDIRTAR_REFUSE, "skipped", cmd->args[i]):
                create_error_return_handler(ERR_RMTAR, ERR_DIRECTORY, cmd->args[i]);
        }

        rh = rmtar(target_path->tar_fd, cmd->is_with_tarball_option, target_path->inside_path);

        if (ERR == rh.verdict) {
            return rh;
        }
        free(target_path);
    }

    return create_successful_return_handler();
}

return_handler rmtar(const int tar_fd, const int with_option, const char *path_from_tar_root) {
    s_posix_header pheader;
    ssize_t dest_offset;
    ssize_t src_offset;

    if (!commons_tar_find_file_from(tar_fd, path_from_tar_root, &pheader)) {
        return create_error_return_handler(ERR_RMTAR, ERR_NO_FILE, path_from_tar_root);
    }

    if (with_option) {
        lseek(tar_fd, 0, SEEK_SET);
        return rmtar_children(tar_fd, path_from_tar_root);
    }

	/* Should return an error when the '-r' option is not specified but the wanted file is a directory. */
    if (commons_tar_is_dir(pheader)) {
        return create_error_return_handler(ERR_RMTAR, ERR_DIRECTORY, path_from_tar_root);
    }

    commons_tar_init_offsets(tar_fd, &src_offset, &dest_offset, &pheader);

    return commons_tar_blockmove(tar_fd, dest_offset, src_offset);
}

static return_handler rmtar_children(const int tar_fd, const char *path_from_tar_root) {
    s_posix_header pheader;
    ssize_t dest_offset;
    ssize_t src_offset;

    while (0 < read(tar_fd, &pheader, TAR_BLOCKSIZE) && NONECHAR != pheader.name[0]) {
        /* Checks all the headers and if the name contains path_from_tar_root, removes the file.*/
        if (commons_tar_is_same_ancestor(tar_fd, path_from_tar_root, pheader.name)) {
            commons_tar_init_offsets(tar_fd, &src_offset, &dest_offset, &pheader);

            if (ERR == (commons_tar_blockmove(tar_fd, dest_offset, src_offset)).verdict) {
                return create_error_return_handler(ERR_RMTAR, ERR_REMOVING, path_from_tar_root);
            }

            /* Reposition the file offset to the correct location after deleting a file. */
            lseek(tar_fd, dest_offset, SEEK_SET);
        }
        else {
            commons_tar_lseek_size_of_file(tar_fd, &pheader);
        }
    }

    return create_successful_return_handler();
}
