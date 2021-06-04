#ifndef NET_H
#define NET_H

#include <arpa/inet.h>
#include <assert.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>

#include "errors.h"
#include "log.h"
#include "messages.h"

#define TRUE 1
#define FALSE 0

#define NET_SADDRIN_SIZE sizeof(struct sockaddr_in)

#define NONECHAR '\0'

/**
 * Temporary function to test the static library
 */
void say(char *str);

/**
 * Initializes a socket address.
 *
 * @note The address family is AF_INET and the internet
 * 		address is `INADDR_ANY` if `internet_address` is empty.
 *
 * @param port is port in the network.
 * @param saddrin is the socket's address.
 * @param internet_address is the internet address.
 */
void net_init_sockaddrin(
	const in_port_t port,
	const char *internet_address,
	struct sockaddr_in *saddrin
);

/**
 * Uses `getaddrinfo` in order to get the IPv4 address of the `hostname`.
 *
 * @param hostname is the target hostname.
 * @param service is the target service name.
 * @param addr is the pointer that will store the IPv4 address.
 *
 * @return OK if an address was found, otherwise ADDR_ERR.
 */
int net_get_inaddr(const char *hostname, const char *service, in_addr_t *addr);

/**
 * Iterates through the `addrinfo` returned by `getaddrinfo`
 * and applies `consume` to them.
 *
 * @param node Is the wanted internet host.
 * @param service Is the wanted internet service.
 * @param family Is the address family (IPv4 or IPv6).
 * @param to_all Is a boolean which specified if `consume` needs
 * 				to be applied to all `addrinfo`.
 * @param consume Is the pointer to the function to apply.
 * @param args Is the argument which will be given to `consume`
 * 				with the `addrinfo->ai_addr`.
 *
 * @return OK if an address was found, otherwise ERR.
 */
int net_applyto_sockaddr(
	const char *node,
	const char *service,
	const int family,
	const int to_all,
	void (*consume) (struct sockaddr *, void *),
	void *args
);

/**
 * Extracts the address and the port from an string with the following format :
 *
 *     <addr>:<port> (Ex: localhost:4242 or 127.0.0.1:4242)
 *
 * @param addrport is the string to explode.
 * @param addr is the `in_addr_t` corresponding to `<addr>`.
 * @param port is the port number (in bigendian) corresponding to `<port>`.
 *
 * @return an `err_id` if an error occured during the extraction.
 */
int net_explode_inaddr(const char *addrport, in_addr_t *addr, int *port);

/**
 * Extracts the address and the port from an string with the following format :
 *
 *     <addr>:<port> (Ex: localhost:4242 or 127.0.0.1:4242)
 *
 * @param addrport is the string to explode.
 * @param addr is the `struct sockaddr*` corresponding to `<addr>`.
 * @param port is the port number (in bigendian) corresponding to `<port>`.
 *
 * @return an `err_id` if an error occured during the extraction.
 */
int net_explode_sockaddr(const char *addrport, struct sockaddr *addr, int *port);

/**
 * Verifies if a given string contains only digit characters.
 *
 * @param str is the string to verify.
 * @param len is the size of `str`.
 *
 * @return TRUE if `str` contains only digit characters, otherwise FALSE.
 */
int net_str_contains_only_digit(const char *str, const ssize_t len);

/**
 * Verifies if the given address is correctly written.
 *
 * @param str is the string to verify.
 *
 * @return TRUE if `address` is correct, otherwise FALSE.
 */
int net_is_address_correct(const char *address);


#endif
