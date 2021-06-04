#include "inc/mvtar.h"

return_handler mvtar_from_cmd(const s_tsh_state *curr_state, const s_command *cmd) {
	s_tsh_path *src_updated_path;
	s_tsh_path *dest_updated_path;
	return_handler rh;

	if (2 > cmd->nb_args) {
		return create_error_return_handler(
			ERR_MVTAR, ERR_MANY_ARGS, curr_state->cwd->inside_path
		);
	}

	/* Get the s_tsh_path representations of the destination path. */
	dest_updated_path =	commons_tsh_update_path_from(
							curr_state,
							cmd->args[cmd->nb_args-1]
						);

	if (NULL == dest_updated_path) {
		return create_error_return_handler(ERR_MVTAR, ERR_UPD_INVALID, "");
	}

    for (int i = 0; i < cmd->nb_args - 1; ++i) {
        src_updated_path = commons_tsh_update_path_from(curr_state, cmd->args[i]);

		if (NULL != src_updated_path) {
			if (commons_tsh_is_path_self_dir(cmd->args[i], LAST)) {
				free(dest_updated_path);
				return create_error_return_handler(ERR_MVTAR_NT_FOUND, ERR_NOT_DIR, cmd->args[i]);
			}

			rh = mvtar(*src_updated_path, *dest_updated_path);

            if (ERR == rh.verdict) {
				free(src_updated_path);
				return rh;
			}
			free(src_updated_path);
        }
        else {
			free(dest_updated_path);
            return create_error_return_handler(ERR_MVTAR, ERR_UPD_INVALID, "");
        }
    }
	free(dest_updated_path);

	return create_successful_return_handler();
}

return_handler mvtar(const s_tsh_path source_path, const s_tsh_path destination_path) {
	if (ERR == (cptar(source_path, destination_path, TRUE)).verdict) {
		if (FD_NOTDEF == source_path.tar_fd) {
			return create_error_return_handler(ERR_MVTAR_NT_FOUND, ERR_NO_FILE, source_path.outside_path);
		}

		return create_error_return_handler(ERR_MVTAR_NT_FOUND, ERR_NO_FILE, source_path.inside_path);
	}

	if (commons_tar_is_valid(source_path.tar_fd)) {
		return rmtar(source_path.tar_fd, TRUE, source_path.inside_path);
	}

	if (OK == commons_remove_file(source_path.outside_path)) {
		return create_successful_return_handler();
	}

	return create_error_return_handler(ERR_MVTAR, ERR_REMOVING, source_path.outside_path);
}
