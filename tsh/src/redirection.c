#include "inc/redirection.h"

static int const_std[3] = { STDOUT_FILENO, STDIN_FILENO, STDERR_FILENO };
static int saved_std[3] = { ERR, ERR, ERR };

static int redirect_std_to(const int index, const int fd) {
	assert(0 <= index && 3 > index);

	saved_std[index] = dup(const_std[index]);

	if (ERR == dup2(fd, const_std[index])) {
		return ERR;
	}
	return OK;
}

static int reset_std(const int index) {
	assert(0 <= index && 3 > index);

	if (0 >= saved_std[index]
		|| ERR == close(const_std[index])
		|| ERR == dup2(saved_std[index], const_std[index]))
	{
		return ERR;
	}
	return OK;
}

int redirection_redirect_stdout_to(const int fd) {
	return redirect_std_to(0, fd);
}

int redirection_redirect_stdin_to(const int fd) {
	return redirect_std_to(1, fd);
}

int redirection_redirect_stderr_to(const int fd) {
	return redirect_std_to(2, fd);
}

int redirection_reset_stdout() {
	return reset_std(0);
}

int redirection_reset_stdin() {
	return reset_std(1);
}

int redirection_reset_stderr() {
	return reset_std(2);
}
