#include "inc/cester.h"

#include "inc/test_cattar_aux.h"


CESTER_TEST(______________CATTAR_TESTS_____________, _, cester_assert_nothing();)
CESTER_TEST(_, _, cester_assert_nothing();)

CESTER_TEST(Cattar_should_write_as_expected_with_a_valid_text_file, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected = 	"# Mode d'emploi\n\n"
							"## Compilation\n\n"
							"## Utilisation\n";

	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-01",
			"doc/user-manual.md",
			buffer
		)
	);

	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Cattar_should_write_as_expected_with_a_valid_empty_file, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected = "";

	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-02",
			"file1",
			buffer
		)
	);

	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Cattar_should_write_as_expected_with_a_valid_symbolic_link, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
	"Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
	"Aliquam eu purus porttitor, maximus tortor nec, dignissim risus.\n"
	"Sed lacinia ultricies est, et pretium ex dapibus vitae.\n"
	"Vivamus iaculis gravida erat sed finibus. Proin viverra vel lacus ornare consectetur.\n";

	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-13",
			"file1s",
			buffer
		)
	);

	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Cattar_should_write_as_expected_with_a_valid_physical_link, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
	"Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
	"Aliquam eu purus porttitor, maximus tortor nec, dignissim risus.\n"
	"Sed lacinia ultricies est, et pretium ex dapibus vitae.\n"
	"Vivamus iaculis gravida erat sed finibus. Proin viverra vel lacus ornare consectetur.\n";

	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-13",
			"file1p",
			buffer
		)
	);

	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Cattar_should_write_as_expected_with_a_valid_physical_link_from_another_folder, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const char *expected =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
		"Aliquam eu purus porttitor, maximus tortor nec, dignissim risus.\n"
		"Sed lacinia ultricies est, et pretium ex dapibus vitae.\n"
		"Vivamus iaculis gravida erat sed finibus. Proin viverra vel lacus ornare consectetur.\n";

	cester_assert_true(
		test_cattar_aux_is_returning_as_expected(
			"test/resources/current-test-tar-files/test-tar-14",
			"folder2/file1p",
			buffer
		)
	);

	cester_assert_equal(
		0, strncmp(buffer, expected, strlen(expected))
	);
)

CESTER_TEST(Cattar_should_return_ERR_with_a_path_leading_to_a_dir, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-01", O_RDONLY);
	const return_handler rh = cattar(tar_fd, "doc/");

	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal("cat: 'doc/': Is a directory\n", rh.message);

	close(tar_fd);
)

CESTER_TEST(Cattar_should_return_ERR_with_a_path_leading_to_a_symbolic_link_linked_to_an_invalid_file, _,
	char buffer[COMMONS_TEST_MAX_LINE_SIZE];
	const int tar_fd = open("test/resources/current-test-tar-files/test-tar-21", O_RDONLY);
	const return_handler rh = cattar(tar_fd, "file1s");

	cester_assert_equal(ERR, rh.verdict);
	cester_assert_str_equal("cat: 'file1': No such file or directory\n", rh.message);
)
