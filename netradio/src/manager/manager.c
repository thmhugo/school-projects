#include "../net/net.h"

/** Stores the informations of a multicaster. */
struct multicaster {
    char id[ID_LENGTH + 1]; ///< Is the multicaster identifier.
    char ip1[IP_LENGTH + 1]; ///< Is the multicaster casting address.
    char port1[PORT_LENGTH + 1]; ///< Is the multicaster port for multicasting.
    char ip2[IP_LENGTH + 1]; ///< Is the multicaster address for communication.
    char port2[PORT_LENGTH + 1]; ///< Is the multicaster port for communication.
} typedef multicaster_t;

/** ["REGI" ID IP1 PORT1 IP2 PORT2END] */
#define REGI_BUFF_SIZE TYPE_LENGTH + 1 + ID_LENGTH + 1 + (IP_LENGTH + 1 + PORT_LENGTH + 1) * 2 + 2

/** ["LINB" NUMEND] */
#define LINB_BUFF_SIZE TYPE_LENGTH + NUM_DIFF_LENGTH + END_MSG_LENGTH + 1

/** ["MESS" ID MESSEND] */
#define NEW_MESS_BUFF_SIZE TYPE_LENGTH + 1 + ID_LENGTH + 1 + MESSAGE_LENGTH + 2

/** Maximum size allowed for incomming message. */
#define COM_BUFF_SIZE NEW_MESS_BUFF_SIZE

/** Duration in seconds between each RUOK sending. */
#define TEST_MULTICASTER_TIMEOUT 15

/** Duration in seconds of the given time to the multicaster to answer back. */
#define ANSWER_MULTICASTER_TIMEOUT 5

/** Message associated to the MESS protocol used in the logs. */
#define MESSAGE_SERVICE "new message service"

/** Intializes global variables from the given arguments. */
static int init_global_variables(char **argv);

/** Frees the multicaster array. */
static void free_multicaster_arr();

/** Manager subroutine. */
static void *communication(void *arg);

/** Sends the list of multicaster. */
static int get_multicaster_list(const int socket);

/** Registers a new multicaster to the list. */
static void register_multicaster(char **arr, const int size, const int sock);

/** Checks if the id is already used in the list. */
static int is_id_already_used(const char *id);

/** Finds a free index in the array 'multicaster_arr_g'. */
static int find_first_free_index();

/** Creates a multicaster. */
static int create_multicaster(char **arr, multicaster_t *multicaster);

/** Tests if the multicaster is alive. */
static int is_multicaster_alive(const int multicaster_socket);

/**
 * Removes a specific multicaster thanks to the given index
 * and use the provided service for logging messages.
 */
static void remove_specific_multicaster(const int index, const char *service);

/** Closes the socket. */
static void *close_socket(void *arg);

/** Prints the error and close the socket. */
static void *err_and_close_socket(const int errid, void *arg);

/** Sends the MESS message to all the multicasters on the list. */
static void send_mess_to_multicasters(const char *buffer, const int client_socket);


/** GLOBAL VARIABLES */
static multicaster_t **multicaster_arr_g; ///< Is the array of multicasters.
static int nb_registered_multicaster_g = 0;  ///< Is the current size of the array.
static int max_multicaster_g = 10;  ///< Is the maximum multicaster number.
static pthread_mutex_t locker_g = PTHREAD_MUTEX_INITIALIZER;  ///< Is the thread locker.


/**
 * Manager's arguments:
 *
 *    ./manager PORT MLTICST_MAX
 */
int main(int argc, char *argv[]) {
	int port;
	int sock;
	struct sockaddr_in address_sock;

	if (3 != argc) {
		log_error(
			"[ initialization ] :> expecting (2) arguments got (%d)",
			argc - 1
		);
		return EXIT_FAILURE;
	}

	// Initalializes the server's port.
	port = atoi(argv[1]);
	if (0 >= port || 9999 < port) {
		log_error(
			"[ initialization ] :> port value must be between 1 and 9999, got (%d)",
			port
		);
		return EXIT_FAILURE;
	}

    // Initalializes global variables from arguments.
	if (OK != init_global_variables(argv)) {
		log_info(
			"[ initialization ] :> exiting: the multicaster can not be initialized"
		);
        return EXIT_FAILURE;
	}

	// Initalializes the server's socket.
	sock = socket(PF_INET, SOCK_STREAM, 0);
	net_init_sockaddrin(port, "", &address_sock);

	// Allows the socket to use the port even it's busy.
	if (OK != setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) {
        log_error("[ initialization ] :> setsockopt(SO_REUSEADDR) failed");
    }

	if (OK != bind(
		sock, (struct sockaddr*)&address_sock, sizeof(struct sockaddr_in)))
	{
		log_error(get_err_msg(BIND_ERR));
		free_multicaster_arr();
		return EXIT_FAILURE;
	}

	if (OK != listen(sock, 0)) {
		log_error(get_err_msg(LISTEN_ERR));
		free_multicaster_arr();
		return EXIT_FAILURE;
	}

	log_info("[ initialization ] :> manager successfully initialized");
	log_info("[ initialization ] :> listening at %d", port);

	while(TRUE) {
		// Manager main routine
		struct sockaddr_in caller;
		socklen_t size = sizeof(caller);
		int *com_sock;

		assert (NULL != (com_sock = malloc(sizeof(int))));
		*com_sock = accept(sock, (struct sockaddr *)&caller, &size);
		if (0 <= *com_sock) {
			pthread_t th;

			if (OK != pthread_create(&th, NULL, communication, com_sock)) {
				log_error(get_err_msg(PTHREAD_ERR));
				free_multicaster_arr();
				exit(EXIT_FAILURE);
			}
		}
	}

	return EXIT_SUCCESS;
}

static int init_global_variables(char **argv) {
	max_multicaster_g = atoi(argv[2]);

	if (0 >= max_multicaster_g || 99 < max_multicaster_g) {
		log_error(
			"[ initialization ] :> maximum number of registered multicasters must be \
			between 1 and 99, got (%d)",
			max_multicaster_g
		);
		return ERR;
	}

	multicaster_arr_g = malloc(sizeof(*multicaster_arr_g) * max_multicaster_g);
	assert(NULL != multicaster_arr_g);
	memset(
		multicaster_arr_g, NONECHAR, sizeof(*multicaster_arr_g) * max_multicaster_g
	);

	return OK;
}

static void free_multicaster_arr() {
	for (int i = 0; i < max_multicaster_g; i++) {
		if (multicaster_arr_g[i] != NULL) {
			free(multicaster_arr_g[i]);
		}
	}
	free(multicaster_arr_g);
}

static void *communication(void *arg) {
	int socket = *((int *) arg);
	int return_val;
	char buffer[COM_BUFF_SIZE + 1];
	char **arr_str = NULL;
	ssize_t size = 0;

	return_val = recv(socket, buffer, (COM_BUFF_SIZE) * sizeof(char), 0);
	if (0 > return_val) {
		return err_and_close_socket(RECV_ERR, arg);
	}
	buffer[return_val] = NONECHAR;

	arr_str = split_on_char(buffer, " ", &size);

	if (!strncmp("REGI", arr_str[0], TYPE_LENGTH) &&
		REGI_BUFF_SIZE == return_val)
	{
		register_multicaster(arr_str, size, socket);
	}

	else if (!strcmp("LIST\r\n", arr_str[0])) {
		if (OK != (return_val = get_multicaster_list(socket))) {
			log_error(get_err_msg(return_val));
		}
	}
	else if (!strncmp("MESS", arr_str[0], TYPE_LENGTH) &&
			NEW_MESS_BUFF_SIZE == return_val)
	{
		send_mess_to_multicasters(buffer, socket);

		if (ERR == send(socket, "ACKM\r\n", TYPE_LENGTH + END_MSG_LENGTH, 0)) {
			log_error(get_err_msg(SEND_ERR));
		}
	}
	else {
		log_error("%s", get_err_msg(INVALID_REQ_ERR));
	}

	free_array(arr_str, size);
	return close_socket(arg);
}

static int get_multicaster_list(const int socket) {
	char answer[LINB_BUFF_SIZE];
	int j = 0;

	sprintf(
		answer,
		"LINB %s%s",
		integer_to_str(nb_registered_multicaster_g, NUM_DIFF_LENGTH),
		END_MSG
	);

	if (0 > send(socket, answer, strlen(answer), 0)) {
		return SEND_ERR;
	}

	log_info("[ list service ] :> linb message sent");

	while (j < max_multicaster_g) {
		pthread_mutex_lock(&locker_g);
		if (multicaster_arr_g[j] != NULL) {
			char item_answer[REGI_BUFF_SIZE];

			sprintf(
				item_answer,
				"ITEM %s %s %s %s %s%s",
				multicaster_arr_g[j]->id,
				multicaster_arr_g[j]->ip1,
				multicaster_arr_g[j]->port1,
				multicaster_arr_g[j]->ip2,
				multicaster_arr_g[j]->port2,
				END_MSG
			);
			if (0 > send(socket, item_answer, strlen(item_answer), 0)) {
				pthread_mutex_unlock(&locker_g);
				return SEND_ERR;
			}
			log_info("[ item service ] :> item message sent");
		}
		j++;
		pthread_mutex_unlock(&locker_g);
	}
	log_info("[ list service ] :> list of multicasters sent");

	return OK;
}

static void send_mess_to_multicasters(
	const char *buffer,
	const int client_socket
) {
	char answer[TYPE_LENGTH + END_MSG_LENGTH + 1];
	int port;
	int multicaster_sock;
	char *multicaster_id;

	for (int i = 0; i < max_multicaster_g; i++) {
		if (NULL != multicaster_arr_g[i]) {
			pthread_mutex_lock(&locker_g);
			int is_alive = TRUE;
			struct sockaddr_in address_sock;
			char *internet_address;

			port = atoi(multicaster_arr_g[i]->port2);

			// Must remove excess zero from the address to avoid
			// timed out connection from connect.
			internet_address =  remove_leading_zero_from_address(
									multicaster_arr_g[i]->ip2
								);

			// Must open a new connection because MESS closes it when it's correctly received.
			net_init_sockaddrin(port, internet_address, &address_sock);
			multicaster_sock = socket(PF_INET, SOCK_STREAM, 0);
			multicaster_id = strdup(multicaster_arr_g[i]->id);

			// If there is an error during the communication with a multicaster,
			// the multicaster is removed from the multicasters array.
			log_info("[ new message service ] :> connecting to %s.", multicaster_id);
			if (ERR == connect(multicaster_sock, (struct sockaddr *)&address_sock,
							sizeof(struct sockaddr_in)))
			{
				log_error(
					"[ new message service ] :> message not sent to %s %s",
					multicaster_id, get_err_msg(CON_ERR)
				);
				log_debug(strerror(errno));
				is_alive = FALSE;
			}

			if (is_alive && ERR == send(multicaster_sock, buffer, NEW_MESS_BUFF_SIZE, 0)) {
				log_error(
					"[ new message service ] :> message not sent to %s %s",
					multicaster_id, get_err_msg(SEND_ERR)
				);
				is_alive = FALSE;
			}

			if (is_alive) {
				log_info(
					"[ new message service ] :> new message sent to %s",
					multicaster_id
				);
			}

			if (is_alive && ERR == recv(multicaster_sock, answer, sizeof(answer), 0)) {
				log_error(
					"[ new message service ] :> message not sent to %s %s",
					multicaster_id, get_err_msg(RECV_ERR)
				);
				is_alive = FALSE;
			}

			if (!is_alive) {
				remove_specific_multicaster(i, MESSAGE_SERVICE);
			}

			if (!strcmp("ACKM\r\n", answer)) {
				log_info(
					"[ new message service ] :> The multicaster %s \
					received the message correctly !",
					multicaster_id
				);
			}

			else {
				log_error(
					"[ new message service ] :> The multicaster %s \
					did not receive the message correctly !",
					multicaster_id
				);
			}

			free(multicaster_id);
			free(internet_address);
			close(multicaster_sock);
			pthread_mutex_unlock(&locker_g);
		}
	}
}

static void remove_specific_multicaster(const int index, const char *service) {
	// The locker is not in this function because in 'send_mess_to_multicasters'
	// the locker is already taken.
	if (multicaster_arr_g[index] != NULL) {
		log_info(
			"[ %s ] :> %s removed.", service, multicaster_arr_g[index]->id
		);
		free(multicaster_arr_g[index]);
		nb_registered_multicaster_g -= 1;
		multicaster_arr_g[index] = NULL;
	}
	else {
		log_error(
			"[ %s ] :> %s is not in the list !",
			service, multicaster_arr_g[index]->id
		);
	}
}

static void register_multicaster(char **arr, const int size, const int sock) {
	multicaster_t *multicaster;
	int index;
	char *multicaster_name;

	log_info("[ registration service ] :> new request received");
	if (nb_registered_multicaster_g >= max_multicaster_g) {
		log_error(
			"[ registration service ] :> maximum number of registered \
			multicaster reached"
		);
	}
	else if (MANAGER_RELATED_FIELD_SIZE != size) {
		log_error(
			"[ registration service ] :> invalid registration request"
		);
	}
	else if (is_id_already_used(arr[1])) {
		log_error(
			"[ registration service ] :> multicaster %s already registered",
			arr[1]
		);
	}
	else {
		// REGI subroutine.
		assert(NULL != (multicaster = malloc(sizeof(multicaster_t))));
		memset(multicaster, NONECHAR, sizeof(multicaster_t));
		index = find_first_free_index();
		if (ERR == create_multicaster(arr, multicaster)) {
			if (0 > send(sock, "RENO\r\n", TYPE_LENGTH + END_MSG_LENGTH, 0)) {
				log_error(get_err_msg(SEND_ERR));
			}
			else {
				log_info("[ registration service ] :> RENO sent");
			}
			free(multicaster);
			return;
		}
		multicaster_name = strdup(multicaster->id);

		pthread_mutex_lock(&locker_g);
		multicaster_arr_g[index] = multicaster;
		nb_registered_multicaster_g += 1;
		pthread_mutex_unlock(&locker_g);

		if (0 > send(sock, "REOK\r\n", TYPE_LENGTH + END_MSG_LENGTH, 0)) {
			log_error(get_err_msg(SEND_ERR));
			free(multicaster);
			free(multicaster_name);
			return;
		}
		log_info(
			"[ registration service ] :> multicaster %s registered",
			multicaster_arr_g[index]->id
		);

		while (TRUE) {
			// RUOK subroutine.
			sleep(TEST_MULTICASTER_TIMEOUT);

			log_info("[ multicaster status service ] :> sending request to %s", multicaster_name);

			if (NULL == multicaster_arr_g[index]) {
				// Only happens if the multicaster was removed during the manager's
				// MESS protocol
				log_warn(
					"[ multicaster status service ] :> %s is no longer alive",
					multicaster_name
				);
				free(multicaster);
				free(multicaster_name);
				return;
			}

			if (OK != is_multicaster_alive(sock)) {
				log_warn(
					"[ multicaster status service ] :> %s is no longer alive",
					multicaster_name
				);

				pthread_mutex_lock(&locker_g);
				remove_specific_multicaster(index, "RUOK");
				pthread_mutex_unlock(&locker_g);

				free(multicaster_name);
				return;
			}
		}
	}
	if (0 > send(sock, "RENO\r\n", TYPE_LENGTH + END_MSG_LENGTH, 0)) {
		log_error(get_err_msg(SEND_ERR));
	}
	else {
		log_info(
			"[ registration service ] :> RENO message sent: can not register the mulicaster"
		);
	}
}

static int is_id_already_used(const char *id) {
	for (int i = 0; i < max_multicaster_g; i++) {
		if (NULL != multicaster_arr_g[i] &&
			!strcmp(multicaster_arr_g[i]->id, id)) {
			return TRUE;
		}
	}

	return FALSE;
}

static int find_first_free_index() {
	for (int i = 0; i < max_multicaster_g; i++) {
		if (NULL == multicaster_arr_g[i]) {
			return i;
		}
	}
	return ERR;
}


static int create_multicaster(char **arr, multicaster_t *multicaster) {
	if (ID_LENGTH != strlen(arr[1])) {
		log_error(
			"[ registration service ] :> the multicaster's name (%s) \
			is illegal, expected length: %d, received: %d",
			arr[1],
			ID_LENGTH,
			strlen(arr[1])
		);
		return ERR;
	}

	if (!net_is_address_correct(arr[2])) {
		log_error(
			"[ registration service ] :> the multicaster's communication \
			address is illegal, received: %s",
			arr[2]
		);
		return ERR;
	}

	if (!net_str_contains_only_digit(arr[3], PORT_LENGTH)) {
		log_error(
			"[ registration service ] :> the multicaster's communication \
			port is illegal, received: %s",
			arr[3]
		);
		return ERR;
	}

	if (!net_is_address_correct(arr[4])) {
		log_error(
			"[ registration service ] :> the multicaster's casting address \
			is illegal, received: %s",
			arr[4]
		);
		return ERR;
	}

	if (!net_str_contains_only_digit(arr[5], PORT_LENGTH)) {
		log_error(
			"[ registration service ] :> the multicaster's casting port is \
			illegal, received: %s",
			arr[5]
		);
		return ERR;
	}

	memcpy(multicaster->id, arr[1], ID_LENGTH);
	memcpy(multicaster->ip1, arr[2], IP_LENGTH);
	memcpy(multicaster->port1, arr[3], PORT_LENGTH);
	memcpy(multicaster->ip2, arr[4], IP_LENGTH);
	memcpy(multicaster->port2, arr[5], PORT_LENGTH);

	return OK;
}

static int is_multicaster_alive(const int multicaster_socket) {
	char buffer[TYPE_LENGTH + END_MSG_LENGTH + 1];
	int return_val = 0;

	if (0 > send(multicaster_socket, "RUOK\r\n", TYPE_LENGTH + END_MSG_LENGTH, 0)) {
		log_error(get_err_msg(SEND_ERR));
		return SEND_ERR;
	}
	log_info("[ multicaster status service ] :> sent");

    while (TRUE) {
		// Initializes set of fd and a timeout.
		fd_set scktfds;
		struct timeval waiting = {ANSWER_MULTICASTER_TIMEOUT, 0};

		// Clears the set and add the multicaster socket to the set.
		FD_ZERO(&scktfds);
    	FD_SET(multicaster_socket, &scktfds);

		return_val = select(multicaster_socket + 1, &scktfds, NULL, NULL, &waiting);
		if (ERR == return_val) {
			return SELECT_ERR;
		}

		if (0 == return_val) {
			// Getting no answer from the current the fd.
			log_error("[ multicaster status service ] :> timeout");
			return MULTICASTER_DEAD;
		}

		if (FD_ISSET(multicaster_socket, &scktfds)) {
			return_val = read(multicaster_socket, buffer, TYPE_LENGTH + END_MSG_LENGTH);
			if (0 > return_val) {
				return RECV_ERR;
			}
			buffer[return_val] = NONECHAR;
			if (!strcmp("IMOK\r\n", buffer)) {
				log_info("[ multicaster status service ] :> the multicaster is alive");
				return OK;
			}
			else {
				return WRONG_TYPE_ERR;
			}
		}
    }
}

static void *close_socket(void *arg) {
	int socket = *((int *) arg);

	free(arg);
	close(socket);
	return NULL;
}

static void *err_and_close_socket(const int errid, void *arg) {
	log_error(get_err_msg(errid));
	return close_socket(arg);
}
