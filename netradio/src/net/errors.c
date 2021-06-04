#include "errors.h"

/** Corresponding error messages of the enum err_id. */
const char *err_msg[MAX_ERRID_SIZE] = {
    "",
    "while sending a message", ///< SEND_ERR
    "while receving a message", ///< RECV_ERR
    "while binding a message", ///< BIND_ERR
    "while listening to a socket", ///< LISTEN_ERR
    "while connecting", ///< CON_ERR
    "", ///< ADDR_ERR
    "invalid port number", ///< PORT_ERR
    "missing arguments for this type of message", ///< MISSING_ARGS_ERR
    "unknown type message", ///< WRONG_TYPE_ERR
    "while trying to open a file", ///< OPEN_FILE_ERR
    "invalid request", ///< INVALID_REQ_ERR
    "while creating a new thread", ///< PTHREAD_ERR
    "message is empty", ///< STR_EMPTY_ERR
    "while selecting fd",  ///< SELECT_ERR
    "multicaster is not alive" ///< MULTICASTER_DEAD
};

const char *get_err_msg(const int id) {
    assert(id >= 0 && id < MAX_ERRID_SIZE);
    return err_msg[id];
}
