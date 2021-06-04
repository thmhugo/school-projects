#include "inc/mkdirtar.h"

return_handler mkdirtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd) {
    s_tsh_path *target_path;
	return_handler rh;

    if (commons_operands_are_missing(cmd)) 
    {
        return create_error_return_handler(
            ERR_MKDIRTAR, ERR_MISSING_OPERAND, ""
        );
    }

    for (int i = 0; i < cmd->nb_args; ++i) {
        target_path = commons_tsh_update_path_from(
                        curr_state,
                        cmd->args[i]
                    );

        if (NULL == target_path) {
            return create_error_return_handler(
                ERR_MKDIRTAR, ERR_UPD_INVALID, ""
            );
        }

        if (commons_tsh_is_path_self_dir(cmd->args[i], LAST)) {
            return create_error_return_handler(ERR_MKDIRTAR, ERR_FILE_EXISTS, PATH_TO_SELF_DIR);
        }

        if (PATH_DELIMITER_CHR != target_path->inside_path[strlen(target_path->inside_path)-1]) {
            strcat(target_path->inside_path, "/");
	    }

        rh = mkdirtar(target_path->tar_fd, target_path->inside_path);
        
        if (ERR == rh.verdict) {
            return rh;
        }
        free(target_path);
    }

    return create_successful_return_handler();
}


return_handler mkdirtar(const int tar_fd, const char *path_from_tar_root) {
    s_posix_header pheader;

    if (TRUE == commons_tar_find_file_from(tar_fd, path_from_tar_root, &pheader)) {
        return create_error_return_handler(ERR_MKDIRTAR, ERR_FILE_EXISTS, path_from_tar_root);
    }

    return commons_tar_append_dir(tar_fd, path_from_tar_root);
}