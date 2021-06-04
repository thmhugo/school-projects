#include "inc/error.h"

return_handler create_error_return_handler(const char *command, const char *error_msg, const char *file_path) {
	return_handler error;

	memset(error.message, NONECHAR, PATH_MAX);
	error.verdict = ERR;

	sprintf(error.message, "%s'%s': %s\n", command, file_path, error_msg);

	return error;
}

return_handler create_successful_return_handler() {
	return_handler success;

	memset(success.message, NONECHAR, PATH_MAX);
	success.verdict = OK;

	return success;
}

void print_error(const char *message) {
	write(STDERR_FILENO, message, strlen(message));
}