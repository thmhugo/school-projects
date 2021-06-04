#include <sys/ioctl.h>
#include <sys/types.h>
#include <curl/curl.h>

#include "../net/net.h"

/** ["REGI" ID IP1 PORT1 IP2 PORT2END] */
#define REGISTER_BUFF_SIZE TYPE_LENGTH + 1 + ID_LENGTH + 1 + (IP_LENGTH + 1 + PORT_LENGTH + 1) * 2 + 2

/** ["DIFF" NUMMESS ID MESSEND] */
#define DIFF_BUFF_SIZE TYPE_LENGTH + 1 + NUM_MESSAGE_LENGTH + 1 + ID_LENGTH + 1 + MESSAGE_LENGTH + 2

/** ["MESS" ID MESSEND] */
#define NEW_MESS_BUFF_SIZE TYPE_LENGTH + 1 + ID_LENGTH + 1 + MESSAGE_LENGTH + 2

/** ["LAST" NB_MSGEND] */
#define LASTMSG_BUFF_SIZE TYPE_LENGTH + 1 + NB_MSG_SIZE + 2

/** ["WETC" MESSAGEEND] */
#define WETC_BUFF_SIZE TYPE_LENGTH + 1 + MESSAGE_LENGTH + 2

/** The wttr api base url. */
#define WTTR_URL_BASE "https://wttr.in/\0"

/** Maximum length of the location in a REWT message. */
#define WETR_LOCATION_LENGTH 50

/** ["RWET" LOCATIONEND] */
#define WETR_MSG_SIZE TYPE_LENGTH + 1 + WETR_LOCATION_LENGTH + 2

/**
 * It's used to receive request from the communication port.
 * And LASTMSG_BUFF_SIZE < NEW_MESS_BUFF_SIZE.
 */
#define COM_BUFF_SIZE NEW_MESS_BUFF_SIZE

/** Maximum number of messages storable by the multicaster. */
#define MAX_NBMSG 99999

/** Duration in seconds between each message casting. */
#define TIME_DURING_MSG_CASTING 2

/** Prints in stdout the content of the last registered message. */
static void print_current_msg();

/** Sends properly the `buff` or closes `sock` and returns ERR. */
static int send_or_close(
    int *sock, const char *buff, const ssize_t buff_size, const char *fmt
);

/** Initializes global variables from the given arguments. */
static int init_global_variables(char **argv);

/** Extracts messages from the given file path. */
static int init_messages(char *path);

/** Replaces the last '\n' character by '\0'. */
static void remove_last_newline(char *str, const ssize_t len);

/** Initializes the registration buffer. */
static char* create_register_buff();

/** Registers to a manager. */
static int registerto(const char *manager_addr);

/** Multicasting subroutine. */
static void* cast_subroutine(void *arg);

/** Communication subroutine. */
static void* com_subroutine(void *arg);

/** Manages communication connections (LAST, MESS and RUOK). */
static void* com_manage_connexion(void *arg);

/** Adds a new message in `messages_g` according to the content of a request buffer. */
static int add_new_message(const char *mess_buff);

/** Manages the LAST request. */
static int send_last_message(const int com_sock, const char *req_buff);

/** Manages the WETR request. */
static int weather_report_service(const int callsock, const char *req_buff);

/** RUOK subroutine. */
static void* ruok_subroutine(void *arg);

/** Performs the request that get the full weather report. */
static int perform_full_weather_request(char* location, const int callsock);

/** Performs the condensed request that will be added into messages_g. */
static int perform_condensed_weather_request(char *location);

/** Sends the content of a weather report file. */
static int send_weather_file(char *location, const int callsock);

/** Manages the RUOK request. */
static int send_RUOK_response(int socket);

static char id_g[ID_LENGTH]; ///< Is the multicaster identifier.

static int comport_g = 0; ///< Is the multicaster port for communication.
static in_addr_t comaddr_g; ///< Is the multicaster address for communication.

static int castport_g = 0; ///< Is the multicaster port for multicasting.
static struct sockaddr castaddr_g; ///< Is the multicaster casting address.

static message_t messages_g[MAX_NBMSG]; ///< Is the stored message.
static int messages_size_g = 0; ///< Is the messages's tail index.
static int curr_msg_i = 0; ///< Is the current `messages_g` index used for casting messages.

static int manager_sock_g = ERR; ///< Is the manager socket initialized during the registration.

static struct winsize wins_g; ///< Contains terminal's window dimensions.

static pthread_mutex_t locker_g = PTHREAD_MUTEX_INITIALIZER;

/**
 * Multicaster's arguments:
 *
 *    ./multicast id comport castaddr:castport manageraddr:managerport messlist
 *
 * argv    [0]   [1]   [2]          [3]                  [4]            [5]
 */
int main(int argc, char *argv[]) {
    int com_sock;
    int cast_sock;
    struct sockaddr_in address_sock;
    pthread_t ruok_thread;
    pthread_t com_thread;
    pthread_t casting_thread;

    if (6 > argc) {
        log_error("expecting (5) arguments got (%d)", argc - 1);
        return ERR;
    }

    if (7 == argc && 0 == strcmp(argv[6], "--quiet")) {
        log_set_quiet(true);
    }

    // Initalializes global variables from arguments.
    if (OK != init_global_variables(argv)) {
        log_info(
            "[ initialization ] :> exiting: the multicaster can not be initialized"
        );
        return ERR;
    }
    log_info(
        "[ initialization ] :> global variables successfully initialized"
    );

    // Tries to register with the given manager.
    if (OK != registerto(argv[4])) {
        log_info(
            "[ initialization ] :> exiting: the multicaster can not be registered"
        );
        return ERR;
    }
    log_info("[ initialization ] :> successfully registered");

    // Creates the RUOK subroutine.
    if (OK != pthread_create(&ruok_thread, NULL, ruok_subroutine, NULL)) {
        net_perror(PTHREAD_ERR);
        close(manager_sock_g);
        return ERR;
    }

    // Creates the casting subroutine.
    cast_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (OK != pthread_create(&casting_thread, NULL, cast_subroutine, &cast_sock)) {
        net_perror(PTHREAD_ERR);
        close(cast_sock);
        return ERR;
    }

    // Creates the new message registration subroutine.
    com_sock = socket(PF_INET, SOCK_STREAM, 0);

    address_sock.sin_family = AF_INET;
    address_sock.sin_port = comport_g;
    address_sock.sin_addr.s_addr = htonl(INADDR_ANY);
    if (OK != setsockopt(com_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) {
        log_error("[ initialization ] :> setsockopt(SO_REUSEADDR) failed");
    }
    if (OK != bind(com_sock, (struct sockaddr*) &address_sock, sizeof(struct sockaddr_in))) {
        net_perror(BIND_ERR);
        close(com_sock);
        return ERR;
    }
    if (OK != listen(com_sock,0)) {
        net_perror(LISTEN_ERR);
        close(com_sock);
        return ERR;
    }
    if (OK != pthread_create(&com_thread, NULL, com_subroutine, &com_sock)) {
        net_perror(PTHREAD_ERR);
        close(com_sock);
        return ERR;
    }

    pthread_join(casting_thread, NULL);

    if (ERR != manager_sock_g) {
        close(manager_sock_g);
    }

    return 0;
}

static int init_global_variables(char **argv) {
    char *id = NULL;

    // Gets the terminal window size.
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wins_g);

    // Initializes the multicaster's id.
    memset(id_g, NONECHAR, ID_LENGTH);
    if (ID_LENGTH <= strlen(argv[1])) {
        id = strndup(argv[1], ID_LENGTH);
    }
    else {
        id = complete_with_hashes(argv[1], ID_LENGTH);
    }
    strncpy(id_g, id, ID_LENGTH);
    free(id);

    // Initializes the multicaster's communication port.
    if (PORT_LENGTH != strlen(argv[2]) ||
        FALSE == net_str_contains_only_digit(argv[2], strlen(argv[2])))
    {
        log_error("%s: '%s'", get_err_msg(PORT_ERR), argv[2]);
        return ERR;
    }
    comport_g = htons(atoi(argv[2]));

    // Initializes the multicaster's casting port.
    if (OK != net_explode_sockaddr(argv[3], &castaddr_g, &castport_g)) {
        return ERR;
    }

    // Initializes the multicaster's messages list.
    if (OK != init_messages(argv[5])) {
        return ERR;
    }

    // Gets the local machine address, used for communication.
    char localhostname[HOST_NAME_MAX];
    gethostname(localhostname, HOST_NAME_MAX);
    net_get_inaddr(localhostname, NULL, &comaddr_g);

    return OK;
}

static int init_messages(char *path) {
    FILE *file;

    if (NULL == (file = fopen(path, "r"))) {
        net_perror(OPEN_FILE_ERR);
        return ERR;
    }
    memset(messages_g, NONECHAR, sizeof(message_t) * MAX_NBMSG);

    log_info("[ initialization ] :> file %s opened successfully", path);
    while (fgets(messages_g[messages_size_g].content, MESSAGE_LENGTH, file)) {
        memcpy(messages_g[messages_size_g].owner_id, id_g, ID_LENGTH);
        messages_g[messages_size_g].num_mess = messages_size_g;

        remove_last_newline(messages_g[messages_size_g].content, MESSAGE_LENGTH);
        char *completed_content = complete_with_hashes(
            messages_g[messages_size_g].content, MESSAGE_LENGTH
        );
        memcpy(
            messages_g[messages_size_g].content,
            completed_content,
            MESSAGE_LENGTH
        );
        free(completed_content);
        log_info("[ initialization ] :> new message added:");
        print_current_msg();
        messages_size_g = (messages_size_g + 1) % MAX_NBMSG;
    }
    log_info(
        "[ initialization ] :> %d messages successfully read",
        messages_size_g
    );

    fclose(file);
    return OK;
}

static void remove_last_newline(char *str, const ssize_t len) {
    for (int i = len - 1; i >= 0; --i) {
        if ('\n' == str[i]) {
            str[i] = NONECHAR;
            return;
        }
    }
}

static int registerto(const char *manager_addr) {
    in_addr_t addr;
    int port = 0;

    // Extracts the manager address.
    if (OK != net_explode_inaddr(manager_addr, &addr, &port)) {
        return ERR;
    }
    log_info("[ initialization ] :> manager address extracted");

    // Connects to the manager.
    struct sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = port;
    memcpy(&sock.sin_addr, &addr, sizeof(in_addr_t));

    manager_sock_g = socket(PF_INET, SOCK_STREAM, 0);

    if (ERR == connect(manager_sock_g, (struct sockaddr *) &sock, sizeof(struct sockaddr_in))) {
        net_perror(CON_ERR);
        close(manager_sock_g);
        return ERR;
    }

    log_info("[ initialization ] :> connected to %s", manager_addr);

    // Sends the registration request.
    log_info(
        "[ initialization ] :> sending register protocol to the manager at %s",
        manager_addr
    );
    char *register_buff = create_register_buff();

    if (ERR == send(manager_sock_g, register_buff, sizeof(char) * REGISTER_BUFF_SIZE, 0)) {
        close(manager_sock_g);
        return ERR;
    }

    log_info("[ initialization ] :> sended request: '%s'", register_buff);
    free(register_buff);

    // Waits for the response.
    log_info("[ initialization ] :> waiting for a response");
    char resp_buff[RETURN_CHECK_LENGTH + 1] = "";
    ssize_t recv_size = recv(manager_sock_g, &resp_buff, RETURN_CHECK_LENGTH, 0);

    // Manages the response.
    if (0 < recv_size) {
        log_info("[ initialization ] :> received response: '%s'", resp_buff);
        if (0 != strncmp("REOK", resp_buff, TYPE_LENGTH)) {
            if (!strncmp("RENO", resp_buff, TYPE_LENGTH)) {
                log_error("[ initialization ] :> the registration failed");
            }
            else {
                log_error(
                    "[ initialization ] :> unexpected response: '%s'",
                    resp_buff
                );
            }
            close(manager_sock_g);
            return ERR;
        }
    }
    else {
        log_error("[ initialization ] :> invalid response");
        close(manager_sock_g);
        return ERR;
    }

    return OK;
}

static char* create_register_buff() {
    char *register_buff = NULL;
    char *id = strndup(id_g, ID_LENGTH);
    char *cast_addr = addr_to_str(((struct sockaddr_in *) &castaddr_g)->sin_addr);
    char *com_addr = addr_to_str(*(struct in_addr *) &comaddr_g);

    assert(
        NULL != (register_buff = malloc(sizeof(char) * REGISTER_BUFF_SIZE))
    );
    memset(register_buff, NONECHAR, REGISTER_BUFF_SIZE);
    sprintf(
        register_buff,
        "%s %s %s %d %s %d%s",
        "REGI",
        id,
        cast_addr,
        ntohs(castport_g),
        com_addr,
        ntohs(comport_g),
        END_MSG
    );

    free(id);
    free(cast_addr);
    free(com_addr);
    return register_buff;
}

static void* ruok_subroutine(void *arg) {
    char ruok_buff[RETURN_CHECK_LENGTH];

    while (TRUE) {
        memset(ruok_buff, NONECHAR, RETURN_CHECK_LENGTH);
        if (RETURN_CHECK_LENGTH == recv(
            manager_sock_g, ruok_buff, RETURN_CHECK_LENGTH, 0)
        ) {
            if (!strcmp("RUOK\r\n", ruok_buff)) {
                if (SEND_ERR == send_RUOK_response(manager_sock_g)) {
                    return NULL;
                }
            }
        }
    }
    return NULL;
}

static int send_RUOK_response(int socket) {
    char ruok_buff[RETURN_CHECK_LENGTH];

    log_info("[ RUOK ] :> well received");
    memset(ruok_buff, NONECHAR, RETURN_CHECK_LENGTH);
    memcpy(ruok_buff, "IMOK\r\n", RETURN_CHECK_LENGTH);
    if (ERR == send_or_close(
        &socket, ruok_buff, RETURN_CHECK_LENGTH, "[ RUOK ] :> %s")
    ) {
        return SEND_ERR;
    }
    return OK;
}

static void* cast_subroutine(void *arg) {
    char diff_buff[DIFF_BUFF_SIZE];
    int sock = *(int *) arg;

    log_info(
        "[ casting service ] :> running at %s:%d",
        inet_ntoa(((struct sockaddr_in *) &castaddr_g)->sin_addr),
        ntohs(castport_g)
    );
    while (TRUE) {
        pthread_mutex_lock(&locker_g);
        if (0 < messages_size_g) {
            char *owner_id = strndup(messages_g[curr_msg_i].owner_id, ID_LENGTH);
            char *content = strndup(messages_g[curr_msg_i].content, MESSAGE_LENGTH);

            memset(diff_buff, NONECHAR, DIFF_BUFF_SIZE);
            sprintf(
                diff_buff,
                "DIFF %s %s %s%s",
                integer_to_str(messages_g[curr_msg_i].num_mess, 4),
                owner_id,
                content,
                END_MSG
            );
            sendto(
                sock,
                diff_buff,
                DIFF_BUFF_SIZE,
                0,
                &castaddr_g,
                (socklen_t)sizeof(struct sockaddr_in)
            );
            log_info(
                "[ casting service ] :> message #%d casted",
                messages_g[curr_msg_i].num_mess
            );
            curr_msg_i = (curr_msg_i + 1) % messages_size_g;
            free(owner_id);
            free(content);
        }
        pthread_mutex_unlock(&locker_g);
        sleep(TIME_DURING_MSG_CASTING);
    }
    return NULL;
}

static void* com_subroutine(void *arg) {
    int com_sock = *(int *) arg;

    log_info(
        "[ communication service ] :> running at %s:%d",
        inet_ntoa(*(struct in_addr *) &comaddr_g),
        ntohs(comport_g)
    );

    while (TRUE) {
        struct sockaddr_in caller;
        socklen_t size = sizeof(caller);
        int *callsock;

        assert(NULL != (callsock = (int *) malloc(sizeof(int))));
        *callsock = accept(com_sock, (struct sockaddr *) &caller, &size);
        if (0 <= *callsock) {
            // New connexion.
            pthread_t thread;

            char *sinaddr = addr_to_str(caller.sin_addr);
            log_info("[ communication service ] :> new connexion from %s", sinaddr);
            free(sinaddr);
            if (OK != pthread_create(
                &thread, NULL, com_manage_connexion, callsock)
            ) {
                net_perror(PTHREAD_ERR);
                close(*callsock);
                free(callsock);
            }
        }
    }
    return NULL;
}

static int send_or_close(
    int *sock,
    const char *buff,
    const ssize_t buff_size,
    const char *fmt
) {
    if (ERR == send(*sock, buff, buff_size, 0)) {
        log_error(fmt, get_err_msg(SEND_ERR));
        close(*sock);
        free(sock);
        return ERR;
    }
    return OK;
}

static void* com_manage_connexion(void *arg) {
    char com_buff[COM_BUFF_SIZE];
    ssize_t recv_size;
    int *callsock = (int*) arg;

    memset(com_buff, NONECHAR, COM_BUFF_SIZE);
    recv_size = recv(*callsock, com_buff, COM_BUFF_SIZE, 0);
    if (NEW_MESS_BUFF_SIZE == recv_size) {
        // Potential MESS request received.
        if (ERR == add_new_message(com_buff)) {
            log_error("[ new message service ] :> rejected new message");
        }
        else {
            // Sends back.
            char resp[RETURN_CHECK_LENGTH] = "ACKM\r\n";
            if (ERR == send_or_close(
                callsock, resp, RETURN_CHECK_LENGTH, "[ new message service ] :> %s"))
            {
                return NULL;
            }
        }
    }
    else if (LASTMSG_BUFF_SIZE == recv_size) {
        // Potential LAST request received.
        if (OK == send_last_message(*callsock, com_buff)) {
            // Sends back.
            char resp[RETURN_CHECK_LENGTH] = "ENDM\r\n";
            if (ERR == send_or_close(
                callsock, resp, RETURN_CHECK_LENGTH, "[ last message service ] :> %s"))
            {
                return NULL;
            }
        }
    }
    else if (WETR_MSG_SIZE == recv_size) {
        if (OK != weather_report_service(*callsock, com_buff)) {
            if (ERR == send_or_close(callsock, "WERR\r\n", RETURN_CHECK_LENGTH,
                                    "[ weather service ] :> communication error"))
            {
                return NULL;
            }
        }
    }
    else if (RETURN_CHECK_LENGTH == recv_size) {
        // Potential RUOK from a client received.
        if (!strcmp("RUOK\r\n", com_buff)) {
            if (SEND_ERR == send_RUOK_response(*callsock)) {
                return NULL;
            }
        }
    }
    else {
        log_error(
            "[ communication service ] :> %s, %s",
            get_err_msg(INVALID_REQ_ERR), com_buff
        );
    }

    close(*callsock);
    free(callsock);
    return NULL;
}

static int add_new_message(const char *mess_buff ) {
    char **split_msg = NULL;
    ssize_t arr_size = 0;

    split_msg = split_on_char(mess_buff, " ", &arr_size);
    if (3 <= arr_size) {
        // Manage the request.
        if (!strcmp(split_msg[0], "MESS")) {
            pthread_mutex_lock(&locker_g);
            if (ID_LENGTH != strlen(split_msg[1])) {
                log_error(
                    "[ new message service ] :> wrong id length: '%s'",
                    split_msg[1]
                );
                free_array(split_msg, arr_size);
                pthread_mutex_unlock(&locker_g);
                return ERR;
            }
            char *completed_id = complete_with_hashes(split_msg[1], ID_LENGTH);
            memcpy(
                messages_g[messages_size_g].owner_id,
                completed_id,
                ID_LENGTH
            );
            free(completed_id);
            messages_g[messages_size_g].num_mess = messages_size_g;
            char content[MESSAGE_LENGTH];

            if (OK != net_get_mess_content(content, mess_buff, 2)) {
                pthread_mutex_unlock(&locker_g);
                log_error(
                    "[ new message service ] :> %s", get_err_msg(INVALID_REQ_ERR)
                );
                free_array(split_msg, arr_size);
                return ERR;
            }

            memcpy(
                messages_g[messages_size_g].content,
                content,
                MESSAGE_LENGTH
            );
            log_info("[ new message service ] :> new message registered:");
            print_current_msg();
            messages_size_g = (messages_size_g + 1) % MAX_NBMSG;
            pthread_mutex_unlock(&locker_g);
            free_array(split_msg, arr_size);
            return OK;
        }
        else {
            log_error(
                "[ new message service ] :> unexpected message type: %s",
                split_msg[0]
            );
        }
    }
    else {
        log_error(
            "[ new message service ] :> %s",
            get_err_msg(INVALID_REQ_ERR)
        );
    }
    free_array(split_msg, arr_size);
    return ERR;
}

static int send_last_message(const int callsock, const char *req_buff) {
    int nb_mess = 0;
    char **split_msg = NULL;
    ssize_t arr_size = 0;
    int i = 0;
    char diff_buff[DIFF_BUFF_SIZE];

    split_msg = split_on_char(req_buff, " ", &arr_size);
    if (2 <= arr_size) {
        if (!strcmp("LAST", split_msg[0])) {
            if (5 == strlen(split_msg[1]) &&
                net_str_contains_only_digit(split_msg[1], 3))
            {
                nb_mess = atoi(split_msg[1]);
                pthread_mutex_lock(&locker_g);
                i = curr_msg_i;
                pthread_mutex_unlock(&locker_g);
                while (0 < nb_mess-- && 0 < i--) {
                    pthread_mutex_lock(&locker_g);
                    char *owner_id = strndup(messages_g[i].owner_id, ID_LENGTH);
                    char *content = strndup(messages_g[i].content, MESSAGE_LENGTH);
                    char *num_mess = integer_to_str(messages_g[i].num_mess, 4);

                    memset(diff_buff, NONECHAR, DIFF_BUFF_SIZE);
                    sprintf(
                        diff_buff,
                        "OLDM %s %s %s%s",
                        num_mess,
                        owner_id,
                        content,
                        END_MSG
                    );
                    if (ERR == send(callsock, diff_buff, DIFF_BUFF_SIZE, 0)) {
                        log_error(
                            "[ last message service ] :> message #%d not sent.",
                            i
                        );
                        return ERR;
                    }
                    else {
                        log_info(
                            "[ last message service ] :> message #%d sent",
                            i
                        );
                    }
                    free(owner_id);
                    free(content);
                    free(num_mess);
                    pthread_mutex_unlock(&locker_g);
                }
                free_array(split_msg, arr_size);
                return OK;
            }
            else {
                log_error(
                    "[ last messages service ] :> invalid message number: '%s'",
                    split_msg[1]
                );
            }
        }
        else {
            log_error(
                "[ last messages service ] :> unexpected message type: '%s'",
                split_msg[0]
            );
        }
    }
    else {
        log_error(
            "[ last messages service ] :> %s", get_err_msg(INVALID_REQ_ERR)
        );
    }
    free_array(split_msg, arr_size);
    return ERR;
}

static void print_current_msg() {
    char *owner_id = strndup(messages_g[messages_size_g].owner_id, ID_LENGTH);
    char *content =
        strndup(
            messages_g[messages_size_g].content,
            wins_g.ws_col - strlen("        content: '...'")
        );
    printf(
        "~  messages_g[%d] =\n"
        "~  \towner_id: '%s',\n"
        "~  \tnum_mess: %d,\n"
        "~  \tcontent: '%s...'\n",
        messages_size_g,
        owner_id,
        messages_g[messages_size_g].num_mess,
        content
    );
    free(owner_id);
    free(content);
}

static CURL *init_curl_request(char *rurl) {
    CURL *curl;
    curl = curl_easy_init();
    if (curl) {
        // URL to work on.
        curl_easy_setopt(curl, CURLOPT_URL, rurl);
        // Follow HTTP redirects.
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        // Shut off the progress meter.
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        // Set the user-agent of the HTTP request.
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
        return curl;
    }
    return NULL;
}

static int perform_curl_request(CURL *curl) {
    CURLcode res;

    res = curl_easy_perform(curl);
    if (CURLE_OK != res) {
        log_error(
            "[ weather report service ] :> request failed: %s",
            curl_easy_strerror(res)
        );
        return ERR;
    }
    curl_easy_cleanup(curl);
    return OK;
}

size_t static callback_writing_fun(
    void *buffer,
    size_t size,
    size_t nmemb,
    void *userp)
{
    size_t realsize = size * nmemb;
    char **response_ptr = (char **)userp;
    *response_ptr = strndup(buffer, realsize);
    return realsize;
}

static int get_file_size(FILE *f) {
    ssize_t size = 0;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    return size;
}

static int send_weather_file(char *location, const int callsock) {
    FILE *fp = NULL;
    char str[MESSAGE_LENGTH];
    int i = 0;
    char message[WETC_BUFF_SIZE];

    if (NULL == (fp = fopen(location, "r"))) {
        log_error(
            "[ weather report service ] :> could not open file %s",
            location
        );
        return ERR;
    }
    if (!get_file_size(fp)) {
        log_error(
            "[ weather report service ] :> %s is an empty file, \
            the location seems wrong",
            location
        );
        fclose(fp);
        return ERR;
    }

    // Limits to 7 to only keep the header.
    while (fgets(str, MESSAGE_LENGTH, fp) != NULL && i < 7) {
        str[strcspn(str, "\n")] = ' ';

        sprintf(
            message,
            "WETC %s%s",
            complete_with_hashes(str, MESSAGE_LENGTH),
            END_MSG
        );
        if (ERR == send(callsock, message, WETC_BUFF_SIZE, 0)) {
            fclose(fp);
            return ERR;
        }
        i++;
    }

    sprintf(message, "PEOF%s", END_MSG);
    fclose(fp);

    if (ERR == send(callsock, message, 6, 0)) {
        return ERR;
    }
    return OK;
}

static int perform_full_weather_request(
    char* location,
    const int callsock)
{
    FILE *file;
    CURL *curl;
    long http_code = 0;
    char request_url[strlen(WTTR_URL_BASE)+WETR_LOCATION_LENGTH];

    sprintf(request_url, "%s%s%s", WTTR_URL_BASE, location, "?T");

    log_info(
        "[ weather report service ] :> about to perform request to : %s",
        request_url
    );

    if (NULL == (file = fopen(location, "w"))) {
        log_error(
            "[ weather report service ] :> failed opening %s",
            location
        );
        return ERR;
    }

    if (NULL == (curl = init_curl_request(request_url))) {
        log_error("[ weather report service ] :> curl initialization failed");
        fclose(file);
        return ERR;
    }

    // To write the request output into a file.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    if (ERR == perform_curl_request(curl)) {
        fclose(file);
        return ERR;
    }
    fclose(file);

    // Manages the http response code.
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (200 != http_code || ERR == send_weather_file(location, callsock)) {
        return ERR;
    }

    return OK;
}


static int perform_condensed_weather_request(char *location) {
    CURL *curl;
    char *response;
    char request_url[strlen(WTTR_URL_BASE)+WETR_LOCATION_LENGTH];

    sprintf(request_url, "%s%s%s", WTTR_URL_BASE, location, "?format=3");
    curl_easy_setopt(curl, CURLOPT_URL, request_url);

    if (NULL == (curl = init_curl_request(request_url))) {
        log_error("[ weather report service ] :> curl initialization failed");
        return ERR;
    }

    assert(NULL != (response = (char *)malloc(MESSAGE_LENGTH)));
    bzero(response, MESSAGE_LENGTH);

    // Setting a callback function to return the data.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_writing_fun);

    // Giving the pointer to the response as the callback parameter.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    if (ERR == perform_curl_request(curl)) {
        return ERR;
    }

    remove_last_newline(response, strlen(response));

    // Register the second response.
    pthread_mutex_lock(&locker_g);

    memcpy(
        messages_g[messages_size_g].owner_id,
        "WEATHER#",
        ID_LENGTH
    );

    memcpy(
        messages_g[messages_size_g].content,
        complete_with_hashes(response, MESSAGE_LENGTH),
        MESSAGE_LENGTH
    );

    messages_g[messages_size_g].num_mess = messages_size_g;

    log_info("[ weather service ] :> new message registered:");
    print_current_msg();
    messages_size_g = (messages_size_g + 1) % MAX_NBMSG;
    pthread_mutex_unlock(&locker_g);

    free(response);

    return OK;
}

static int weather_report_service(
    const int callsock,
    const char *req_buff
) {
    char *location = NULL;
    char **split_msg = NULL;
    ssize_t arr_size = 0;

    split_msg = split_on_char(req_buff, " ", &arr_size);

    if (2 == arr_size) {
        if (!strcmp("RWET", split_msg[0])) {
            // +2 because of the trailing "\r\n".
            if (WETR_LOCATION_LENGTH + 2 == strlen(split_msg[1])) {
                assert(
                    NULL != (location = malloc(WETR_LOCATION_LENGTH * sizeof(char)))
                );
                bzero(location, WETR_LOCATION_LENGTH * sizeof(char));
                strcpy(location, remove_hashes(split_msg[1]));

                if (ERR == perform_condensed_weather_request(location) ||
                    ERR == perform_full_weather_request(location, callsock))
                {
                    remove(location);
                    free(split_msg);
                    free(location);
                    return ERR;
                }

                remove(location);
                free(split_msg);
                free(location);
                return OK;
            }
            else {
                log_error(
                    "[ weather report service ] :> incorrect location size (%d)",
                    strlen(split_msg[1])
                );
                return ERR;
            }
        }
        else {
            log_error(
                "[ weather report service ] :> not a WETR message (%s)",
                split_msg[0]
            );
            return ERR;
        }
    }
    else {
        log_error(
            "[ weather report service ] :> wrong WETR message \
            format: expected 2 elts, got %d)",
            arr_size
        );
        return ERR;
    }
}
