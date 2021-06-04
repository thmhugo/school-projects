#include "messages.h"
#include "net.h"

/**
 * Completes the beginning of the line with some zero.
 *
 * @param line is the line to be filled.
 * @param length is the line length to reach by filling with zeros.
 */
static void complete_with_zero(char *line, const int length);

/**
 * Gets the associated message length according to the given message type.
 *
 * @param type is the message type.

 * @return the length of the message.
 */
static int get_expected_msg_length(const char *type);

/** List of all possible message types. */
const char *types_msg[MSG_TYPES_LENGTH] = {
	"DIFF",
	"MESS",
	"ACKM",
	"LAST",
	"OLDM",
	"ENDM",
	"REGI",
	"REOK",
	"RENO",
	"RUOK",
	"IMOK",
	"LIST",
	"LINB",
	"ITEM"
};

char *complete_with_hashes(const char *message, const ssize_t message_length) {
	char *result = NULL;

	assert(NULL != (result = malloc(sizeof(char) * message_length + 1)));

	memset(result, NONECHAR, message_length + 1);
	strncpy(result, message, message_length);
	memset(result + strlen(message), '#', message_length - strlen(message));
	result[message_length] = NONECHAR;

	return result;
}

char **split_on_char(const char *message, const char *delimiter, ssize_t *size) {
	char **tokens = NULL;
	char *message_dup = strdup(message);
	char *msg_tok = message_dup;
	char *tok = strtok_r(msg_tok, delimiter, &msg_tok);
	ssize_t max_tokens_capacity = DEFAULT_TOKENS_CAPACITY;
	int curr_tokens_size = 0;

	assert(NULL != (tokens = malloc(max_tokens_capacity * sizeof(char*))));
	while (NULL != tok) {
		if (curr_tokens_size >= max_tokens_capacity) {
			max_tokens_capacity *= 2;
			assert(
				NULL != (tokens = realloc(tokens, max_tokens_capacity * sizeof(char*)))
			);
		}

		tokens[curr_tokens_size] = strdup(tok);
		curr_tokens_size++;
		tok = strtok_r(NULL, delimiter, &msg_tok);
	}

	*size = curr_tokens_size;
	free(message_dup);

	return tokens;
}

char *addr_to_str(const struct in_addr address) {
	const char *str_address = inet_ntoa(address);
	ssize_t size = 0;
	char **tokens = split_on_char(str_address, ".", &size);
	char *result = NULL;

	assert(NULL != (result = malloc(sizeof(char) * IP_LENGTH + 1)));
	memset(result, NONECHAR, sizeof(char) * IP_LENGTH + 1);
	for (int i = 0; i < 4; i++) {
		char token[IP_INNER_LENGTH + 1];

		memset(token, NONECHAR, IP_INNER_LENGTH + 1);
		strcpy(token, tokens[i]);
		complete_with_zero(token, IP_INNER_LENGTH);
		strcat(result, token);
		if (IP_INNER_LENGTH != i) {
			strcat(result, ".");
		}
	}

	free_array(tokens, size);
	return result;
}

char *integer_to_str(const int value, const int length) {
	char *result;

	assert(NULL != (result = malloc(sizeof(char) * length + 1)));
	sprintf(result, "%d", value);

	if (length != strlen(result)) {
		complete_with_zero(result, length);
	}

	return result;
}

int is_legal_type(const char *type) {
	for (int i = 0; i < MSG_TYPES_LENGTH; i++) {
		if (0 == strcmp(type, types_msg[i])) {
			return i;
		}
	}
	return ERR;
}

message_t *create_msg(const char *owner, const int num, const char *content) {
	message_t *msg;

	assert(NULL != (msg = malloc(sizeof(message_t))));
	strcpy(msg->owner_id, owner);
	msg->num_mess = num;
	strcpy(msg->content, content);

	return msg;
}

void free_array(char **arr, const int size) {
	for (int i = 0; i < size; i++) {
		free(arr[i]);
	}
	free(arr);
}

static void complete_with_zero(char *line, const int length) {
	char tmp[strlen(line)];
	int index = length - strlen(line);

	strcpy(tmp, line);
	memset(line, '0', index);
	strcpy(line + index, tmp);
}

int net_get_mess_content(char *res, const char *buff, const int nb_whites_to_skip) {
	int nb_whites_seen = 0;
	ssize_t readed_size = 0;
	int i = -1;

	while (nb_whites_seen != nb_whites_to_skip && ++i < strlen(buff)) {
		if (' ' == buff[i]) {
			++nb_whites_seen;
		}
	}
	if (nb_whites_seen != nb_whites_to_skip) {
		return ERR;
	}

	memset(res, NONECHAR, MESSAGE_LENGTH);
	memcpy(res, buff + i + 1, MESSAGE_LENGTH);

	return OK;
}

char* remove_hashes(char *msg) {
	int i = strlen(msg);
	while(i > 0) {
		if (HASH_TOKEN == msg[i]) {
			msg[i]=NONECHAR;
		}
		i--;
	}
	return msg;
}

char *remove_leading_zero_from_address(char *internet_address) {
	char *new_address;
	ssize_t size = 0;
	char **arr = split_on_char(internet_address, ".", &size);

	assert(NULL != (new_address = malloc(sizeof(char) * IP_LENGTH + 1)));
	memset(new_address, NONECHAR, IP_LENGTH + 1);

	for (int i = 0; i < size; i++) {
		char inner_address[IP_INNER_LENGTH + 1];
		int inner_ip = atoi(arr[i]);

		memset(inner_address, NONECHAR, IP_INNER_LENGTH + 1);

		sprintf(inner_address, "%d", inner_ip);
		strcat(new_address, inner_address);

		if (IP_INNER_LENGTH != i) {
			strcat(new_address, ".");
		}
	}

	free_array(arr, size);
	return new_address;
}
