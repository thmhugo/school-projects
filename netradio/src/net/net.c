#include "net.h"
#include "errors.h"

/**
 * Copies the `saddr->sin_addr` to `addr`.
 *
 * @note Is made for being used with `net_applyto_sockaddr`.
 */
static void net_get_inaddr_aux(struct sockaddr *saddr, void *addr);

/**
 * Copies the `saddr` to `addr`.
 *
 * @note Is made for being used with `net_applyto_sockaddr`.
 */
static void net_get_sockaddr_aux(struct sockaddr *saddr, void *addr);

void net_init_sockaddrin(
	const in_port_t port,
	const char *internet_address,
	struct sockaddr_in *saddrin)
{
	assert(NULL != saddrin);
	saddrin->sin_family = AF_INET;
	saddrin->sin_port = htons(port);
	if (0 != strlen(internet_address)) {
		if (0 == inet_aton(internet_address, &saddrin->sin_addr)) {
			log_error("inet_aton: invalid internet ip.");
		}
	}
	else {
		saddrin->sin_addr.s_addr = htonl(INADDR_ANY);
	}
}

int net_get_inaddr(const char *hostname, const char *service, in_addr_t *addr) {
	return net_applyto_sockaddr(
		hostname, service, AF_INET, FALSE, net_get_inaddr_aux, addr
	);
}

int net_get_sockaddr(const char *hostname, const char *service, struct sockaddr *addr) {
	return net_applyto_sockaddr(
		hostname, service, AF_INET, FALSE, net_get_sockaddr_aux, addr
	);
}

int net_applyto_sockaddr(
	const char *node,
	const char *service,
	const int family,
	const int to_all,
	void (*consume) (struct sockaddr *, void *),
	void *args)
{
	struct addrinfo *hd_info;
	struct addrinfo *info;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = family;
	hints.ai_socktype = SOCK_DGRAM;

	if (OK != getaddrinfo(node, service, &hints, &hd_info)) {
		return ADDR_ERR;
	}

	info = hd_info;
	while (NULL != info) {
		consume(info->ai_addr, args);
		info = info->ai_next;
		if (!to_all) {
			break;
		}
	}

	freeaddrinfo(hd_info);
	return OK;
}

static void net_get_inaddr_aux(struct sockaddr *saddr, void *addr) {
	memcpy(
		(struct in_addr *) addr,
		&(((struct sockaddr_in *) saddr)->sin_addr),
		sizeof(struct in_addr)
	);
}

static void net_get_sockaddr_aux(struct sockaddr *saddr, void *addr) {
	memcpy(
		(struct sockaddr *) addr,
		saddr,
		sizeof(struct sockaddr)
	);
}

int net_explode_inaddr(const char *addrport, in_addr_t *addr, int *port) {
	int retval;
	struct sockaddr sockaddr;

	if (OK != (retval = net_explode_sockaddr(addrport, &sockaddr, port))) {
		return retval;
	}
	net_get_inaddr_aux(&sockaddr, addr);

	return OK;
}

int net_explode_sockaddr(const char *addrport, struct sockaddr *saddr, int *port) {
	ssize_t size = 0;
	ssize_t port_size = 0;
	char **splited_array = NULL;

	// Gets left and right side of the string <addr>:<port>.
	splited_array = split_on_char(addrport, ":", &size);
	if (2 != size) {
		log_error("invalid <address:port>: '%s'", addrport);
		free_array(splited_array, size);
		return ADDR_ERR;
	}

	// Gets the port number.
	port_size = strlen(splited_array[1]);
	if (4 != port_size ||
		FALSE == net_str_contains_only_digit(splited_array[1], port_size))
	{
		log_error("%s: '%s'", get_err_msg(PORT_ERR), splited_array[1]);
		free_array(splited_array, size);
		return PORT_ERR;
	}
	*port = htons(atoi(splited_array[1]));

	// Gets the sockaddr.
	if (OK != net_get_sockaddr(splited_array[0], splited_array[1], saddr)) {
		log_error("invalid address: '%s'", splited_array[0]);
		free_array(splited_array, size);
		return ADDR_ERR;
	}

	free_array(splited_array, size);
	return OK;
}

int net_str_contains_only_digit(const char *str, const ssize_t len) {
	for (int i = 0; i < len; ++i) {
		if ('0' > str[i] || '9' < str[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

int net_is_address_correct(const char *address) {
	ssize_t size = 0;
	int str_len = 0;
	char **splited_array;

	splited_array =  split_on_char(address, ".", &size);

	if (4 != size || IP_LENGTH != strlen(address)) {
		free_array(splited_array, size);
		return FALSE;
	}

	for (int i = 0; i < size; i++) {
		str_len = strlen(splited_array[i]);

		if(IP_INNER_LENGTH != str_len ||
			!net_str_contains_only_digit(splited_array[i], str_len)
		) {
			free_array(splited_array, size);
			return FALSE;
		}
	}

	free_array(splited_array, size);
	return TRUE;
}
