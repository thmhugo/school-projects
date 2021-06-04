#include "inc/cd.h"

return_handler cd(s_tsh_state *tsh_state, char *path) {
	const int prev_tar_fd = tsh_state->cwd->tar_fd;

 	switch (commons_tsh_update_state_from_path(tsh_state, path)) {
		case ERR:
			return create_error_return_handler(ERR_CDTAR, ERR_NO_FILE, path);
		case FD_NOTDEF:
			{
				/* Not in tarball.*/
				if (0 < prev_tar_fd) {
					/* Going out from tarball. */
					close(prev_tar_fd);
				}
				if (OK == chdir(tsh_state->cwd->outside_path)) {
					return create_successful_return_handler();
				}
				return create_error_return_handler(ERR_CDTAR, strerror(errno), path);
			}
		default:
			{
				/* In tarball. */
				if (OK == chdir(dirname(strdup(tsh_state->cwd->outside_path)))) {
					return create_successful_return_handler();
				}
				return create_error_return_handler(ERR_CDTAR, strerror(errno), tsh_state->cwd->outside_path);
			}
	}
}
