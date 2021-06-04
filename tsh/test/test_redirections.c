#include "inc/cester.h"
#include "inc/commons_test.h"
#include "../src/inc/redirection.h"

CESTER_TEST(____________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________REDIRECTION_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(_____________________________________, _, cester_assert_nothing();)

/* TESTS associated to stdout redirection. */

CESTER_TEST(RedirectStdoutTo_a_valid_file_should_return_OK, _,
    const char *path = "test/resources/current-test-tar-files/tmp-dup-stdout-test";
    int fd = open(path, O_RDWR | O_TRUNC, STDOUT_FILENO);

    cester_assert_equal(OK, redirection_redirect_stdout_to(fd));
    redirection_reset_stdout();
    close(fd);
)

CESTER_TEST(RedirectStdoutTo_a_nonexistent_file_should_return_ERR, _,
    const char *path = "test/resources/current-test-tar-files/not_a_file";
    int fd = open(path, O_RDWR | O_TRUNC, STDOUT_FILENO);

    cester_assert_equal(ERR, redirection_redirect_stdout_to(fd));
    redirection_reset_stdout();
    close(fd);
)

CESTER_TEST(RedirectStdoutTo_a_valid_file_should_work_with_io_functions, _,
    const char *path = "test/resources/current-test-tar-files/tmp-dup-stdout-test";
    int fd = open(path, O_RDWR | O_TRUNC);
    char *text = "Lorem ipsum dolor sit amet\n";

    redirection_redirect_stdout_to(fd);
    int write_size = write(STDOUT_FILENO, text, strlen(text));

    char *result = malloc(strlen(text));
    int redirected_file_fd = open(path, O_RDONLY | O_APPEND);

    int read_size = read(redirected_file_fd, result, strlen(text));

    cester_assert_equal(write_size, strlen(text));
    cester_assert_equal(read_size, strlen(text));
    cester_assert_equal(0, strcmp(text, result));

    close(redirected_file_fd);
    redirection_reset_stdout();
    close(fd);
    free(result);
)

CESTER_TEST(ResetStdout_without_redirection_should_return_ERR, _,
    cester_assert_equal(ERR, redirection_reset_stdout());
)

/* TESTS associated to stdin redirection. */

CESTER_TEST(______________________________________, _, cester_assert_nothing();)

CESTER_TEST(RedirectStdinTo_a_valid_file_should_return_OK, _,
    const char *path = "test/resources/current-test-tar-files/tmp-dup-stdout-test";
    int fd = open(path, O_RDWR | O_TRUNC, STDOUT_FILENO);

    cester_assert_equal(OK, redirection_redirect_stdin_to(fd));

    redirection_reset_stdin();
    close(fd);
)

CESTER_TEST(RedirectStdinTo_a_nonexistent_file_should_return_ERR, _,
    const char *path = "test/resources/current-test-tar-files/not_a_file";
    int fd = open(path, O_RDWR | O_TRUNC, STDOUT_FILENO);

    cester_assert_equal(ERR, redirection_redirect_stdin_to(fd));
    redirection_reset_stdout();
    close(fd);
)

CESTER_TEST(RedirectStdinTo_a_valid_file_should_work_with_io_functions, _,
    const char *path = "test/resources/current-test-tar-files/tmp-dup-stdin-test";
    int fd = open(path, O_RDWR | O_TRUNC);
    char *text = "Lorem ipsum dolor sit amet\n";

    int write_size = write(fd, text, strlen(text));
    lseek(fd, 0, SEEK_SET);

    redirection_redirect_stdin_to(fd);

    char *result = malloc(strlen(text));

    int read_size = read(STDIN_FILENO, result, strlen(text));

    cester_assert_equal(write_size, strlen(text));
    cester_assert_equal(read_size, strlen(text));
    cester_assert_equal(0, strcmp(text, result));

    redirection_reset_stdin();
    close(fd);
    free(result);
)

CESTER_TEST(ResetStdin_without_redirection_should_return_ERR, _,
    cester_assert_equal(ERR, redirection_reset_stdin());
)
