#include "inc/rmdirtar.h"

return_handler rmdirtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd) {
    s_tsh_path *target_path;
	return_handler rh;

    if (commons_operands_are_missing(cmd)) 
    {
        return create_error_return_handler(
            ERR_RMDIRTAR, ERR_MISSING_OPERAND, ""
        );
    }

    for (int i = 0; i < cmd->nb_args; ++i) {
        target_path = commons_tsh_update_path_from(
                        curr_state,
                        cmd->args[i]
                    );

        if (NULL == target_path) {
            return create_error_return_handler(
                ERR_RMDIRTAR, ERR_UPD_INVALID, ""
            );
        }

        if (commons_tsh_is_path_self_dir(cmd->args[i], LAST)) {
            return create_error_return_handler(ERR_RMDIRTAR, ERR_INVALID_ARG, cmd->args[i]);
        }

        rh = rmdirtar(target_path->tar_fd, target_path->inside_path);
        
        if (ERR == rh.verdict) {
            return rh;
        }
        free(target_path);
    }

    return create_successful_return_handler();
}

return_handler rmdirtar(const int tar_fd, const char *path_from_tar_root) {
    s_posix_header pheader;
    ssize_t dest_offset;
    ssize_t src_offset;

    if (commons_tar_find_file_from(tar_fd, path_from_tar_root, &pheader)) {
        if (TAR_DIR_F == pheader.typeflag &&
        commons_tar_is_dir_empty(tar_fd, path_from_tar_root)) {
            dest_offset = lseek(tar_fd, -TAR_BLOCKSIZE, SEEK_CUR);
            lseek(tar_fd, TAR_BLOCKSIZE, SEEK_CUR);
            src_offset = lseek(tar_fd, 0, SEEK_CUR);

            return commons_tar_blockmove(tar_fd, dest_offset, src_offset);
        }

        if (TAR_DIR_F == pheader.typeflag &&
        !commons_tar_is_dir_empty(tar_fd, path_from_tar_root)) {
            return create_error_return_handler(ERR_RMDIRTAR, ERR_DIR_NT_EPY, path_from_tar_root);
        }

        return create_error_return_handler(ERR_RMDIRTAR, ERR_NOT_DIR, path_from_tar_root);
    }

    return create_error_return_handler(ERR_RMDIRTAR, ERR_NO_FILE, path_from_tar_root);
}
