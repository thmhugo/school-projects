#include "inc/cester.h"
#include "inc/test_parser_aux.h"

CESTER_TEST(________________________________________________, _, cester_assert_nothing();)
CESTER_TEST(______________PARSER_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(__________________________________________________, _, cester_assert_nothing();)

CESTER_TEST(Parse_an_empty_string_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd(""));
	redirection_reset_stderr();
)

CESTER_TEST(Parse_a_simple_piped_command_string, _,
	s_command *given_command_list = parse_with_the_cwd("cat|grep");
	s_command *expected_command_tab[2];

	expected_command_tab[0] = create_s_command(
		"cat",  // cmd
		0,      // nb_args
		NULL,   // args
		FALSE,  // is_with_outside_option
		FALSE,  // is_with_tarball_option
		NULL    // redirections
	);
	expected_command_tab[1] = create_s_command(
		"grep", // cmd
		0,      // nb_args
		NULL,   // args
		FALSE,  // is_with_outside_options
		FALSE,  // is_with_tarball_option
		NULL    // redirections
	);

	cester_assert_true(all_commands_are_equal(given_command_list, expected_command_tab, 2));

	commons_tsh_free_command_ll(given_command_list);
)

CESTER_TEST(Parse_a_command_string_with_multiple_command_args_and_pipe, _,
	s_command *given_command_list = parse_with_the_cwd("ls  path/to/file -l| grep");
	s_command *expected_command_tab[2];
	char *ls_args[1] = { "path/to/file" };

	expected_command_tab[0] = create_s_command(
		"ls",    // cmd
		1,       // nb_args
		ls_args, // args
		FALSE,    // is_with_outside_option
		TRUE,    // is_with_tarball_option
		NULL     // redirections
	);
	expected_command_tab[1] = create_s_command(
		"grep", // cmd
		0,      // nb_args
		NULL,   // args
		FALSE,  // is_with_outside_option
		FALSE,  // is_with_tarball_option
		NULL    // redirections
	);

	cester_assert_true(all_commands_are_equal(given_command_list, expected_command_tab, 2));

	commons_tsh_free_command_ll(given_command_list);
)

CESTER_TEST(Parse_a_single_command_with_mutiple_args_and_option, _,
	s_command *given_command_list = parse_with_the_cwd("ls this file -l");
	char *ls_args[2] = { "this", "file" };
	s_command *expected_command = create_s_command(
										"ls",    // cmd
										2,       // nb_args
										ls_args, // args
										FALSE,    // is_with_outside_option
										TRUE,    // is_with_tarball_option
										NULL     // redirections
									);

	cester_assert_true(s_command_are_equal(given_command_list, expected_command));
	
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(given_command_list->next);
	redirection_reset_stderr();
	free(expected_command);
	free(given_command_list);
)

CESTER_TEST(Parse_a_single_command_with_mutiple_args_and_two_options, _,
	s_command *given_command_list = parse_with_the_cwd("ls -a this -l file");
	char *ls_args[3] = { "-a", "this", "file" };
	s_command *expected_command = create_s_command(
											"ls",    // cmd
											3,       // nb_args
											ls_args, // args
											TRUE,    // is_with_outside_option
											TRUE,   // is_with_tarball_option
											NULL     // redirections
										);

	cester_assert_true(s_command_are_equal(given_command_list, expected_command));
	cester_assert_null(given_command_list->next);
	redirection_reset_stderr();
	
	free(given_command_list);
	free(expected_command);
)

CESTER_TEST(Parse_a_single_command_with_mutiple_args_and_two_options_2, _,
	s_command *given_command_list = parse_with_the_cwd("ls -l this -a file");
	char *ls_args[3] = { "this", "-a", "file" };
	s_command *expected_command = create_s_command(
											"ls",    // cmd
											3,       // nb_args
											ls_args, // args
											TRUE,    // is_with_outside_option
											TRUE,    // is_with_tarball_option
											NULL     // redirections
										);

	cester_assert_true(s_command_are_equal(given_command_list, expected_command));

	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(given_command_list->next);
	redirection_reset_stderr();

	free(given_command_list);
	free(expected_command);
)

CESTER_TEST(Parse_a_single_command_with_a_non_tarball_option, _,
	s_command *given_command_list = parse_with_the_cwd("ls -la");
	char *ls_args[1] = { "-la" };
	s_command *expected_command = create_s_command(
											"ls",    // cmd
											1,       // nb_args
											ls_args, // args
											TRUE,    // is_with_outside_option
											FALSE,   // is_with_tarball_option
											NULL     // redirections
										);

	cester_assert_true(s_command_are_equal(given_command_list, expected_command));

	free(given_command_list);
	free(expected_command);
)

CESTER_TEST(Parse_a_command_with_several_pipes_and_args, _,
	s_command *given_command_list = parse_with_the_cwd(
										"ls path/to/file | cat arg arg arg  |yes"
									);
	s_command *cmd;
	s_command *expected_command_tab[3];
	char *ls_args[1] = { "path/to/file" };
	char *cat_args[3] = { "arg", "arg", "arg" };

	expected_command_tab[0] = create_s_command("ls", 1, ls_args, FALSE, FALSE, NULL);
	expected_command_tab[1] = create_s_command("cat", 3, cat_args, FALSE, FALSE, NULL);
	expected_command_tab[2] = create_s_command("yes", 0, NULL, FALSE, FALSE, NULL);

	cester_assert_true(all_commands_are_equal(given_command_list, expected_command_tab, 3));

	commons_tsh_free_command_ll(given_command_list);
)

CESTER_TEST(Parse_an_invalid_redirection_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd("ls path/to/file >"));
	redirection_reset_stderr();
)

CESTER_TEST(Parse_an_invalid_pipe_at_the_beginning_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd("| ls path/to/file"));
	redirection_reset_stderr();
)

CESTER_TEST(Parse_an_invalid_pipe_after_some_spaces_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd("   | ls path/to/file"));
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_an_invalid_pipe_at_the_end_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd("ls path/to/file |"));
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_an_invalid_pipe_at_the_end_before_some_spaces_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd("ls path/to/file |     "));
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_two_pipes_in_a_row_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd("cmd arg || cmd"));
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_two_pipes_in_a_row_separate_by_spaces_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(parse_with_the_cwd("cmd arg |  | cmd"));
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_a_cmd_with_mutliple_redirection_in_the_same_stream_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(
		parse_with_the_cwd(
			"cmd > current-test-tar-files/empty-file > current-test-tar-files/empty-file "
		)
	);
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_a_cmd_with_mutliple_redirection_in_the_same_stream_with_different_type_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(
		parse_with_the_cwd(
			"cmd > current-test-tar-files/empty-file >> current-test-tar-files/empty-file"
		)
	);
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_a_cmd_with_mutliple_redirection_in_the_same_stream_with_different_type_and_order_should_return_NULL, _,
	assert(OK == redirection_redirect_stderr_to(open("/dev/null", O_RDWR)));
	cester_assert_null(
		parse_with_the_cwd(
			"cmd < current-test-tar-files/empty-file > current-test-tar-files/empty-file"
			" < current-test-tar-files/empty-file"
		)
	);
	redirection_reset_stderr(); 
)

CESTER_TEST(Parse_command_with_redirection_path_and_tarball_option, _,
	char *ls_args[1]= { "path/to/file" };

	s_command *given_command_list =
		parse_with_the_cwd("ls -l path/to/file > current-test-tar-files/test-tar-01/doc");

	/* Creates expected redirection. */
	s_redirection *expected_out_redirection =
		create_s_redirection("current-test-tar-files/test-tar-01/doc", REDIRECT_TRUNC);

	s_command_redirections *expected_command_redirections =
		create_s_command_redirections(NULL, expected_out_redirection, NULL);

	/* Creates the expected command. */
	s_command *expected_command =
		create_s_command("ls", 1, ls_args, FALSE, TRUE, expected_command_redirections);

	cester_assert_true(s_command_are_equal(given_command_list, expected_command));

	free(expected_command);
	free(expected_command_redirections);
	free(expected_out_redirection);
	commons_tsh_free_command_ll(given_command_list);
)

CESTER_TEST(Parse_command_with_multiple_args_and_one_redirection, _,
	s_command *given_command_list = parse_with_the_cwd(
										"ls path/to/file "
										"> current-test-tar-files/test-tar-01/doc"
										" path/to/another/file "
									);
	s_command *expected_command;
	char *ls_args[2]= { "path/to/file", "path/to/another/file" };
	s_command_redirections *expected_command_redirections;

	/* Creates expected redirection. */
	s_redirection *expected_out_redirection =
		create_s_redirection(
			"current-test-tar-files/test-tar-01/doc",
			REDIRECT_TRUNC
	);

	expected_command_redirections =
		create_s_command_redirections(
			NULL,
			expected_out_redirection,
			NULL
	);

	/* Creates the expected command. */
	expected_command = create_s_command(
			"ls", 2, ls_args, FALSE, FALSE, expected_command_redirections
		);

	cester_assert_true(s_command_are_equal(given_command_list, expected_command));

	free(expected_command);
	free(expected_out_redirection);
	free(expected_command_redirections);
	commons_tsh_free_command_ll(given_command_list);
)

CESTER_TEST(Parse_command_with_redirection_at_the_beginning, _,
	s_command *given_command_list = parse_with_the_cwd(
		"> current-test-tar-files/test-tar-01/doc ls -l path/to/file"
	);
	s_command *expected_command = NULL;
	char *ls_args[1]= {"path/to/file"};
	s_command_redirections *expected_command_redirections;

	/* Creates expected redirection. */
	s_redirection *expected_out_redirection =
		create_s_redirection(
			"current-test-tar-files/test-tar-01/doc",
			O_TRUNC
	);

	expected_command_redirections =
		create_s_command_redirections(
			NULL,
			expected_out_redirection,
			NULL
	);

	/* Creates the expected command. */
	 expected_command = create_s_command(
			"ls", 1, ls_args, FALSE, TRUE, expected_command_redirections
		);

	cester_assert_true(s_command_are_equal(given_command_list, expected_command));

	free(expected_command);
	free(expected_out_redirection);
	free(expected_command_redirections);
	commons_tsh_free_command_ll(given_command_list);
)


CESTER_TEST(Parse_multiple_commands_with_single_redirection, _,
	s_command *given_command_list = parse_with_the_cwd(
		"ls -l path/to/file >> current-test-tar-files/empty-file "
		"| cat < current-test-tar-files/empty-file"
	);
	s_command *expected_command_tab[2];

	/* 'ls' expected redirection & args initialization. */

	char *ls_args[1] = { "path/to/file" };
	s_command_redirections *ls_expected_command_redirections;

	s_redirection *ls_expected_out_redirection =
		create_s_redirection(
			"current-test-tar-files/empty-file",
			O_APPEND
	);

	ls_expected_command_redirections =
		create_s_command_redirections(
			NULL,
			ls_expected_out_redirection,
			NULL
	);

	expected_command_tab[0] = create_s_command(
		 "ls", 1, ls_args, FALSE, TRUE, ls_expected_command_redirections
	);

	/* 'cat' expected redirection & args initialization*/

	s_command_redirections *cat_expected_command_redirections;

	s_redirection *cat_expected_in_redirection =
		create_s_redirection(
			"current-test-tar-files/empty-file",
			O_TRUNC
	);

	cat_expected_command_redirections =
		create_s_command_redirections(
			cat_expected_in_redirection,
			NULL,
			NULL
	);

	expected_command_tab[1] = create_s_command(
			"cat", 0, NULL, FALSE, FALSE, cat_expected_command_redirections
	);

	cester_assert_true(all_commands_are_equal(given_command_list, expected_command_tab, 2));

	free(ls_expected_command_redirections);
	free(ls_expected_out_redirection);
	free(cat_expected_command_redirections);
	free(cat_expected_in_redirection);
	commons_tsh_free_command_ll(given_command_list);
)

CESTER_TEST(Parse_multiple_commands_with_multiple_redirections, _,
	s_command *given_command_list = parse_with_the_cwd(
		"ls -l path/to/file > current-test-tar-files/empty-file"
		"  | cat < current-test-tar-files/empty-file > current-test-tar-files/empty-file"
	);

	s_command *expected_command_tab[2];
	const int red_expected_type[3] = {O_TRUNC, O_TRUNC , O_TRUNC};

	/* 'ls' expected redirection & args initialization. */
	char *ls_args[1]= {"path/to/file"};

	s_redirection *expected_ls_out_redirection =
		create_s_redirection("current-test-tar-files/empty-file", O_TRUNC);

	s_command_redirections *ls_expected_command_redirections =
		create_s_command_redirections(
			NULL,
			expected_ls_out_redirection,
			NULL);

	expected_command_tab[0] =
		create_s_command("ls", 1, ls_args, FALSE, TRUE, ls_expected_command_redirections);

	/* 'cat' expected redirection & args initialization*/

	s_redirection *cat_in_redirection =
		create_s_redirection("current-test-tar-files/empty-file", O_TRUNC);

	s_redirection *cat_out_redirection =
		create_s_redirection("current-test-tar-files/empty-file", O_TRUNC);

	s_command_redirections *cat_expected_command_redirections =
		create_s_command_redirections(
			cat_in_redirection,
			cat_out_redirection,
			NULL);

	expected_command_tab[1] =
		create_s_command("cat", 0, NULL, FALSE, FALSE, cat_expected_command_redirections);

	cester_assert_true(all_commands_are_equal(given_command_list, expected_command_tab, 2));

	free(ls_expected_command_redirections);
	free(cat_expected_command_redirections);
	free(cat_in_redirection);
	free(cat_out_redirection);
	commons_tsh_free_command_ll(given_command_list);
)

CESTER_TEST(Parse_a_command_with_more_than_DEFAULT_NB_COMMANDS_ARGS, _,
	s_command *given_command_ll = parse_with_the_cwd(
										"cmd a a a a a a a a a a a a a a a a a a a a a b"
									);
	const int nb_args = given_command_ll->nb_args;


	cester_assert_not_null(given_command_ll);
	cester_assert_equal(22, nb_args);
	cester_assert_str_equal("b", given_command_ll->args[nb_args-1]);
	cester_assert_str_equal("a", given_command_ll->args[nb_args-2]);

	commons_tsh_free_command_ll(given_command_ll);
)
