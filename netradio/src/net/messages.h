#ifndef MESSAGES_H
#define MESSAGES_H

#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>

#define END_MSG_LENGTH 2
#define NUM_DIFF_LENGTH 2
#define IP_INNER_LENGTH 3
#define DEFAULT_TOKENS_CAPACITY 4
#define NUM_MESSAGE_LENGTH 4
#define PORT_LENGTH 4
#define TYPE_LENGTH 4
#define ID_LENGTH 8
#define MSG_TYPES_LENGTH 14
#define IP_LENGTH 15
#define MESSAGE_LENGTH 140
#define NB_MSG_SIZE 3

#define RETURN_CHECK_LENGTH 6 ///< unique field response.

#define UNIQUE_TYPE_FIELD_SIZE 1
#define ASKING_TYPE_TOKENS_SIZE 2
#define MESS_FIELD_SIZE 3
#define MSG_RELATED_FIELD_SIZE 4
#define MANAGER_RELATED_FIELD_SIZE 6

#define END_MSG "\r\n"

#define HASH_TOKEN '#'

/** Enumeration used for the message types management. */
enum types_id {
	DIFF,
	MESS,
	ACKM,
	LAST,
	OLDM,
	ENDM,
	REGI,
	REOK,
	RENO,
	RUOK,
	IMOK,
	LIST,
	LINB,
	ITEM
};

/**
 *  Models the messages to be sent or received.
 *
 *  `owner_id` is the author's id.
 *  `num_mess` is the message id.
 *  `content` is the text of the message.
 */
typedef struct {
	char owner_id[ID_LENGTH]; ///< is the author's id.
	int num_mess; ///< is the message id.
	char content[MESSAGE_LENGTH]; ///< is the text of the message.
} message_t;

/**
 *  Completes the string given in argument with hash(es) until reaching
 *  the message_length.
 *
 *  @param message is the message to be completed.
 *  @param message_length is the maximum length of the message.
 *
 *  @return the message with hash(es).
 */
char *complete_with_hashes(const char *message, const ssize_t message_length);

/**
 *  Adds some zero(s) to the address to reach 15 bytes.
 *
 *  @param address is the address to be converted.
 *
 *  @return the converted address.
 */
char *addr_to_str(struct in_addr address);

/**
 *  Adds some zero(s) to the integer to reach the desirated length.
 *
 *  @param value is the integer to be converted.
 *  @param length is the desirated length of the string.
 *  @return the integer in string format with additional(s) zero(s).
 */
char *integer_to_str(const int value, const int length);

/**
 *  Verifies if the type is legal or not.
 *  @note The legal types can be found in the `msg_types` array.
 *
 *  @param type is the attribute to be verified.
 *
 *  @return ERR if it's illegal and the index in the `msg_types`
 * 				array otherwise.
 */
int is_legal_type(const char *type);

/**
 *  Transforms a string into an array of string.
 *
 *  @param message is the string to be splited.
 *  @param delimiter is the delimiter used to split the char.
 *  @param size is the final size of the resulting array.
 *
 *  @return an array of string.
 */
char **split_on_char(const char *message, const char *delimiter, ssize_t *size);

/**
 *  Creates a new message.
 *
 *  @param owner is the id of the message's owner.
 *  @param num is the message's id.
 *  @param content is the text of the message.
 *
 *  @return a message.
 */
message_t *create_msg(const char *owner, const int num, const char *content);

/**
 * Extracts the message content from a request.
 *
 * Exemple:
 *
 *     get_mess_content(res, "TYPE ID MESS", 2) -> OK with res = "MESS"
 *     get_mess_content(res, "TYPE ID MESS", 3) -> ERR
 *
 * @param res is an allocated of MESSAGE_LENGTH size buffer,
 * 				which will contains the message content.
 * @param buff is the buffer containing the request content.
 * @param nb_whites_to_skip is the number of white spaces to skip
 * 				before collecting the message content.
 *
 * @return OK if the res could be fill correctly, otherwise, ERR.
 */
int net_get_mess_content(char *res, const char *buff, const int nb_whites_to_skip);

/**
 *  Frees the array and the content.
 *
 *  @param arr is the array to be freed.
 *  @param size if the size of the array.
 */
void free_array(char **arr, const int size);

/**
 * Removes the HASHING_TOKEN of a message.
 *
 * @param msg is the message in which remove the HASHING_TOKEN.
 *
 * @return msg without HASHING_TOKEN.
*/
char* remove_hashes(char *msg);

/**
 *  Removes leading zeros from the given string address.
 *
 *  @param internet_address is the string to be changed.
 *
 *  @return the new internet address without any leading zero.
 */
char *remove_leading_zero_from_address(char *internet_address);

#endif
