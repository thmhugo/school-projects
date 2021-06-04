#include "../cester.h"
#include "../../src/net/messages.h"

CESTER_TEST(complete_with_hashes_on_id, _,
	const char id[ID_LENGTH + 1] = "RADIO";
	char *result_id = complete_with_hashes(id, ID_LENGTH);

	cester_assert_str_equal("RADIO###", result_id);

	free(result_id);
)

CESTER_TEST(complete_with_hashes_on_full_id, _,
	const char id[ID_LENGTH + 1] = "RADIO123";
	char *result_id = complete_with_hashes(id, ID_LENGTH);

	cester_assert_str_equal(id, result_id);

	free(result_id);
)

CESTER_TEST(complete_with_hashes_on_message, _,
	const char msg[MESSAGE_LENGTH] = "Lorem ipsum";
	const char expected_msg[MESSAGE_LENGTH + 1] = "Lorem ipsum"
	"################################################################"
	"#################################################################";
	char *result_msg = complete_with_hashes(msg, MESSAGE_LENGTH);

	cester_assert_str_equal(expected_msg, result_msg);

	free(result_msg);
)

CESTER_TEST(complete_with_hashes_on_full_message, _,
	const char msg[MESSAGE_LENGTH + 1] = "Lorem ipsum dolor sit amet, consectetuer "
	"adipiscing elit. Aenean commodo ligula get dolor. Ae nean massa. "
	"Cuo ciis natoque penatibus et mate";
	char *result_msg = complete_with_hashes(msg, MESSAGE_LENGTH);

	cester_assert_str_equal(msg, result_msg);

	free(result_msg);
)

CESTER_TEST(addr_to_str_on_address, _,
	struct in_addr address;
	inet_aton("127.0.0.1", &address);
	char *str_addr = addr_to_str(address);

	cester_assert_str_equal("127.000.000.001", str_addr);

	free(str_addr);
)

CESTER_TEST(integer_to_str_on_integer, _,
	const int value = 120;
	char *result = integer_to_str(value, NUM_MESSAGE_LENGTH);

	cester_assert_str_equal("0120", result);

	free(result);
)

CESTER_TEST(integer_to_str_on_integer_2, _,
	const int value = 1;
	char *result = integer_to_str(value, NUM_MESSAGE_LENGTH);

	cester_assert_str_equal("0001", result);

	free(result);
)

CESTER_TEST(integer_to_str_on_full_integer, _,
	const int value = 1234;
	char *result = integer_to_str(value, NUM_MESSAGE_LENGTH);

	cester_assert_str_equal("1234", result);

	free(result);
)

CESTER_TEST(is_legal_type, _,
	const char value[5] = "DIFF";

	cester_assert_int_eq(0, is_legal_type(value));
)

CESTER_TEST(is_legal_type_on_wrong_one, _,
	const char value[5] = "DIFO";

	cester_assert_int_eq(ERR, is_legal_type(value));
)

CESTER_TEST(split_on_char_on_longer_string, _,
	const char value[] = "MESS id Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
	ssize_t size = 0;

	char **result =  split_on_char(value, " ", &size);
	cester_assert_str_equal("MESS", result[0]);
	cester_assert_str_equal("id", result[1]);
	cester_assert_str_equal("Lorem", result[2]);
	cester_assert_str_equal("ipsum", result[3]);
	cester_assert_str_equal("dolor", result[4]);
	cester_assert_str_equal("sit", result[5]);
	cester_assert_str_equal("amet,", result[6]);
	cester_assert_str_equal("consectetur", result[7]);
	cester_assert_str_equal("adipiscing", result[8]);
	cester_assert_str_equal("elit.", result[9]);
	cester_assert_int_eq(10, size);

	free_array(result, size);
)

CESTER_TEST(create_message, _,
	message_t *msg = create_msg(
		"Lorem",
		0,
		"dolor sit amet, consectetur adipiscing elit. Fusce at ligula eu sem pharetra gravida."
	);

	cester_assert_str_equal("Lorem", msg->owner_id);
	cester_assert_equal(0, msg->num_mess);
	cester_assert_str_equal(
		"dolor sit amet, consectetur adipiscing elit. Fusce at ligula eu sem pharetra gravida."
		, msg->content
	);

	free(msg);
)

CESTER_TEST(net_get_mess_content, _,
	char *content = "MESS ID###### Lorem dolor sit amet, consectetur adipiscing elit. Fusce at ligula eu sem pharetra gravida.";
	char *expected = "Lorem dolor sit amet, consectetur adipiscing elit. Fusce at ligula eu sem pharetra gravida.";
	char *result;
	assert(NULL != (result = malloc(MESSAGE_LENGTH * sizeof(char) + 1)));

	cester_assert_equal(OK, net_get_mess_content(result, content, 2));
	result[MESSAGE_LENGTH] = NONECHAR;
	cester_assert_str_equal(expected, result);

	if (0 != strcmp(expected, result)) {
		/* Double check of the result string. */
   		CESTER_REPORT_FAILURE_REGARDLESS();
	}

	free(result);
)

CESTER_TEST(net_get_mess_content_should_fail, _,
	char *content = "MESS ID###### Wrong_test";
	char *expected = "Wrong_test";
	char *result;
	assert(NULL != (result = malloc(MESSAGE_LENGTH * sizeof(char) + 1)));

	cester_assert_equal(ERR, net_get_mess_content(result, content, 3));
	result[MESSAGE_LENGTH] = NONECHAR;
	cester_assert_str_not_equal(expected, result);

	free(result);
)


CESTER_TEST(remove_leading_zero_from_address, _,
	char *address = "127.000.000.002";
	char *result = remove_leading_zero_from_address(address);

	cester_string_equals(address, result);

	free(result);
)

CESTER_TEST(remove_leading_zero_from_address_2, _,
	char *address = "128.111.111.111";
	char *result = remove_leading_zero_from_address(address);

	cester_string_equals(address, result);

	free(result);
)