#ifndef ERRORS_H
#define ERRORS_H

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "log.h"

#define OK 0

#define MAX_ERRID_SIZE 17

#define net_perror(ID)  log_error("%s: %s", get_err_msg(ID), strerror(errno))

/** Enumeration used for error management. */
enum err_id {
  ERR = -1,
  SEND_ERR = 1, ///< An error occured while sending a message.
  RECV_ERR, ///< An error occured while receving a message.
  BIND_ERR, ///< An error occured while binding a socket.
  LISTEN_ERR, ///< An error occured while listening to a socket.
  CON_ERR, ///< An error occured while connection a socket.
  ADDR_ERR, ///< An error occured while manipulating address.
  PORT_ERR, ///< An error occured while manipulating a port number.
  MISSING_ARGS_ERR, ///< Not enough argument.
  WRONG_TYPE_ERR, ///< Wrong type of message.
  OPEN_FILE_ERR, ///< Wrong type of message.
  INVALID_REQ_ERR, ///< Invalid request.
  PTHREAD_ERR, ///< An error occured while creating a pthread.
  STR_EMPTY_ERR, ///< The string is empty.
  SELECT_ERR, ///< An error occured while selecting a fd.
  MULTICASTER_DEAD ///< Multicaster is not alive.
};

/**
 * Gets the corresponding error messages of the enum err_id.
 *
 * @param id is the err_id.
 * @return the corresponding message.
 */
const char *get_err_msg(const int id);

#endif
