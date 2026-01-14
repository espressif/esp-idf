/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef _HTTPD_PRIV_H_
#define _HTTPD_PRIV_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <esp_log.h>
#include <esp_err.h>

#include <esp_http_server.h>
#include "osal.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_LIBC_NEWLIB_NANO_FORMAT
#define NEWLIB_NANO_COMPAT_FORMAT            PRIu32
#define NEWLIB_NANO_COMPAT_CAST(size_t_var)  (uint32_t)size_t_var
#else
#define NEWLIB_NANO_COMPAT_FORMAT            "zu"
#define NEWLIB_NANO_COMPAT_CAST(size_t_var)  size_t_var
#endif

/* Size of request data block/chunk (not to be confused with chunked encoded data)
 * that is received and parsed in one turn of the parsing process. */
#define PARSER_BLOCK_SIZE  128

/* Formats a log string to prepend context function name */
#define LOG_FMT(x)      "%s: " x, __func__

/**
 * @brief Control message data structure for internal use. Sent to control socket.
 */
struct httpd_ctrl_data {
    enum httpd_ctrl_msg {
        HTTPD_CTRL_SHUTDOWN,
        HTTPD_CTRL_WORK,
        HTTPD_CTRL_MAX,
    } hc_msg;
    httpd_work_fn_t hc_work;
    void *hc_work_arg;
};

/**
 * @brief Thread related data for internal use
 */
struct thread_data {
    othread_t handle;   /*!< Handle to thread/task */
    enum {
        THREAD_IDLE = 0,
        THREAD_RUNNING,
        THREAD_STOPPING,
        THREAD_STOPPED,
    } status;           /*!< State of the thread */
};

/**
 * @brief A database of all the open sockets in the system.
 */
struct sock_db {
    int fd;                                 /*!< The file descriptor for this socket */
    void *ctx;                              /*!< A custom context for this socket */
    bool ignore_sess_ctx_changes;           /*!< Flag indicating if session context changes should be ignored */
    void *transport_ctx;                    /*!< A custom 'transport' context for this socket, to be used by send/recv/pending */
    httpd_handle_t handle;                  /*!< Server handle */
    httpd_free_ctx_fn_t free_ctx;      /*!< Function for freeing the context */
    httpd_free_ctx_fn_t free_transport_ctx; /*!< Function for freeing the 'transport' context */
    httpd_send_func_t send_fn;              /*!< Send function for this socket */
    httpd_recv_func_t recv_fn;              /*!< Receive function for this socket */
    httpd_pending_func_t pending_fn;        /*!< Pending function for this socket */
    uint64_t lru_counter;                   /*!< LRU Counter indicating when the socket was last used */
    bool lru_socket;                        /*!< Flag indicating LRU socket */
    char pending_data[PARSER_BLOCK_SIZE];   /*!< Buffer for pending data to be received */
    size_t pending_len;                     /*!< Length of pending data to be received */
    bool for_async_req;                     /*!< If true, the socket will not be LRU purged */
#ifdef CONFIG_HTTPD_WS_SUPPORT
    bool ws_handshake_done;                 /*!< True if it has done WebSocket handshake (if this socket is a valid WS) */
    bool ws_close;                          /*!< Set to true to close the socket later (when WS Close frame received) */
    esp_err_t (*ws_handler)(httpd_req_t *r);   /*!< WebSocket handler, leave to null if it's not WebSocket */
    bool ws_control_frames;                         /*!< WebSocket flag indicating that control frames should be passed to user handlers */
    void *ws_user_ctx;                         /*!< Pointer to user context data which will be available to handler for websocket*/
#endif
};

/**
 * @brief   Auxiliary data structure for use during reception and processing
 *          of requests and temporarily keeping responses
 */
struct httpd_req_aux {
    struct sock_db *sd;                             /*!< Pointer to socket database */
    char           *scratch;                        /*!< Temporary buffer for our operations (1 byte extra for null termination) */
    size_t          scratch_size_limit;             /*!< Scratch buffer size limit (By default this value is set to CONFIG_HTTPD_MAX_REQ_HDR_LEN, overwrite is possible) */
    size_t          scratch_cur_size;               /*!< Scratch buffer cur size (By default this value is set to CONFIG_HTTPD_MAX_URI_LEN, overwrite is possible) */
    size_t          max_req_hdr_len;             /*!< Header buffer size limit */
    size_t          max_uri_len;             /*!< URI buffer size limit */
    size_t          remaining_len;                  /*!< Amount of data remaining to be fetched */
    char           *status;                         /*!< HTTP response's status code */
    char           *content_type;                   /*!< HTTP response's content type */
    bool            first_chunk_sent;               /*!< Used to indicate if first chunk sent */
    unsigned        req_hdrs_count;                 /*!< Count of total headers in request packet */
    unsigned        resp_hdrs_count;                /*!< Count of additional headers in response packet */
    struct resp_hdr {
        const char *field;
        const char *value;
    } *resp_hdrs;                                   /*!< Additional headers in response packet */
    struct http_parser_url url_parse_res;           /*!< URL parsing result, used for retrieving URL elements */
#ifdef CONFIG_HTTPD_WS_SUPPORT
    bool ws_handshake_detect;                       /*!< WebSocket handshake detection flag */
    httpd_ws_type_t ws_type;                        /*!< WebSocket frame type */
    bool ws_final;                                  /*!< WebSocket FIN bit (final frame or not) */
    uint8_t mask_key[4];                            /*!< WebSocket mask key for this payload */
#endif
};

/**
 * @brief   Server data for each instance. This is exposed publicly as
 *          httpd_handle_t but internal structure/members are kept private.
 */
struct httpd_data {
    httpd_config_t config;                  /*!< HTTPD server configuration */
    int listen_fd;                          /*!< Server listener FD */
    int ctrl_fd;                            /*!< Ctrl message receiver FD */
#if CONFIG_HTTPD_QUEUE_WORK_BLOCKING
    SemaphoreHandle_t ctrl_sock_semaphore;  /*!< Ctrl socket semaphore */
#endif
    int msg_fd;                             /*!< Ctrl message sender FD */
    struct thread_data hd_td;               /*!< Information for the HTTPD thread */
    struct sock_db *hd_sd;                  /*!< The socket database */
    int hd_sd_active_count;                 /*!< The number of the active sockets */
    httpd_uri_t **hd_calls;                 /*!< Registered URI handlers */
    struct httpd_req hd_req;                /*!< The current HTTPD request */
    struct httpd_req_aux hd_req_aux;        /*!< Additional data about the HTTPD request kept unexposed */
    uint64_t lru_counter;                   /*!< LRU counter */
    esp_http_server_event_id_t http_server_state;              /*!< HTTPD server state */

    /* Array of registered error handler functions */
    httpd_err_handler_func_t *err_handler_fns;
};

/**
 * @brief Options for receiving HTTP request data
 */
typedef enum {
    HTTPD_RECV_OPT_NONE               = 0,
    HTTPD_RECV_OPT_HALT_AFTER_PENDING = 1,   /*!< Halt immediately after receiving from pending buffer */
    HTTPD_RECV_OPT_BLOCKING           = 2,   /*!< Receive blocking (don't return partial length) */
} httpd_recv_opt_t;

/******************* Group : Session Management ********************/
/** @name Session Management
 * Functions related to HTTP session management
 * @{
 */

// Enum function, which will be called for each session
typedef int (*httpd_session_enum_function)(struct sock_db *session, void *context);

/**
 * @brief  Enumerates all sessions
 *
 * @param[in] hd            Server instance data
 * @param[in] enum_function Enumeration function, which will be called for each session
 * @param[in] context       Context, which will be passed to the enumeration function
 */
void httpd_sess_enum(struct httpd_data *hd, httpd_session_enum_function enum_function, void *context);

/**
 * @brief   Returns next free session slot (fd<0)
 *
 * @param[in] hd    Server instance data
 *
 * @return
 *  - +VE : Free session slot
 *  - NULL: End of iteration
 */
struct sock_db *httpd_sess_get_free(struct httpd_data *hd);

/**
 * @brief Retrieve a session by its descriptor
 *
 * @param[in] hd     Server instance data
 * @param[in] sockfd Socket FD
 * @return pointer into the socket DB, or NULL if not found
 */
struct sock_db *httpd_sess_get(struct httpd_data *hd, int sockfd);

/**
 * @brief Delete sessions whose FDs have became invalid.
 *        This is a recovery strategy e.g. after select() fails.
 *
 * @param[in] hd    Server instance data
 */
void httpd_sess_delete_invalid(struct httpd_data *hd);

/**
 * @brief   Initializes an http session by resetting the sockets database.
 *
 * @param[in] hd    Server instance data
 */
void httpd_sess_init(struct httpd_data *hd);

/**
 * @brief   Starts a new session for client requesting connection and adds
 *          it's descriptor to the socket database.
 *
 * @param[in] hd    Server instance data
 * @param[in] newfd Descriptor of the new client to be added to the session.
 *
 * @return
 *  - ESP_OK   : on successfully queuing the work
 *  - ESP_FAIL : in case of control socket error while sending
 */
esp_err_t httpd_sess_new(struct httpd_data *hd, int newfd);

/**
 * @brief   Processes incoming HTTP requests
 *
 * @param[in] hd      Server instance data
 * @param[in] session Session
 *
 * @return
 *  - ESP_OK    : on successfully receiving, parsing and responding to a request
 *  - ESP_FAIL  : in case of failure in any of the stages of processing
 */
esp_err_t httpd_sess_process(struct httpd_data *hd, struct sock_db *session);

/**
 * @brief   Remove client descriptor from the session / socket database
 *          and close the connection for this client.
 *
 * @param[in] hd      Server instance data
 * @param[in] session Session
 */
void httpd_sess_delete(struct httpd_data *hd, struct sock_db *session);

/**
 * @brief   Free session context
 *
 * @param[in] ctx     Pointer to session context
 * @param[in] free_fn Free function to call on session context
 */
void httpd_sess_free_ctx(void **ctx, httpd_free_ctx_fn_t free_fn);

/**
 * @brief   Add descriptors present in the socket database to an fdset and
 *          update the value of maxfd which are needed by the select function
 *          for looking through all available sockets for incoming data.
 *
 * @param[in]  hd    Server instance data
 * @param[out] fdset File descriptor set to be updated.
 * @param[out] maxfd Maximum value among all file descriptors.
 */
void httpd_sess_set_descriptors(struct httpd_data *hd, fd_set *fdset, int *maxfd);

/**
 * @brief   Checks if session can accept another connection from new client.
 *          If sockets database is full then this returns false.
 *
 * @param[in] hd  Server instance data
 *
 * @return True if session can accept new clients
 */
bool httpd_is_sess_available(struct httpd_data *hd);

/**
 * @brief   Checks if session has any pending data/packets
 *          for processing
 *
 * This is needed as httpd_unrecv may un-receive next
 * packet in the stream. If only partial packet was
 * received then select() would mark the fd for processing
 * as remaining part of the packet would still be in socket
 * recv queue. But if a complete packet got unreceived
 * then it would not be processed until further data is
 * received on the socket. This is when this function
 * comes in use, as it checks the socket's pending data
 * buffer.
 *
 * @param[in] hd      Server instance data
 * @param[in] session Session
 *
 * @return True if there is any pending data
 */
bool httpd_sess_pending(struct httpd_data *hd, struct sock_db *session);

/**
 * @brief   Removes the least recently used client from the session
 *
 * This may be useful if new clients are requesting for connection but
 * max number of connections is reached, in which case the client which
 * is inactive for the longest will be removed from the session.
 *
 * @param[in] hd  Server instance data
 *
 * @return
 *  - ESP_OK    : if session closure initiated successfully
 *  - ESP_FAIL  : if failed
 */
esp_err_t httpd_sess_close_lru(struct httpd_data *hd);

/**
 * @brief   Closes all sessions
 *
 * @param[in] hd  Server instance data
 *
 */
void httpd_sess_close_all(struct httpd_data *hd);

/** End of Group : Session Management
 * @}
 */

/****************** Group : URI Handling ********************/
/** @name URI Handling
 * Methods for accessing URI handlers
 * @{
 */

/**
 * @brief   For an HTTP request, searches through all the registered URI handlers
 *          and invokes the appropriate one if found
 *
 * @param[in] hd  Server instance data for which handler needs to be invoked
 *
 * @return
 *  - ESP_OK    : if handler found and executed successfully
 *  - ESP_FAIL  : otherwise
 */
esp_err_t httpd_uri(struct httpd_data *hd);

/**
 * @brief   Unregister all URI handlers
 *
 * @param[in] hd  Server instance data
 */
void httpd_unregister_all_uri_handlers(struct httpd_data *hd);

/**
 * @brief   Validates the request to prevent users from calling APIs, that are to
 *          be called only inside a URI handler, outside the handler context
 *
 * @param[in] req Pointer to HTTP request that needs to be validated
 *
 * @return
 *  - true  : if valid request
 *  - false : otherwise
 */
bool httpd_validate_req_ptr(httpd_req_t *r);

/* httpd_validate_req_ptr() adds some overhead to frequently used APIs,
 * and is useful mostly for debugging, so it's preferable to disable
 * the check by default and enable it only if necessary */
#ifdef CONFIG_HTTPD_VALIDATE_REQ
#define httpd_valid_req(r)  httpd_validate_req_ptr(r)
#else
#define httpd_valid_req(r)  true
#endif

/** End of Group : URI Handling
 * @}
 */

/****************** Group : Processing ********************/
/** @name Processing
 * Methods for processing HTTP requests
 * @{
 */

/**
 * @brief   Initiates the processing of HTTP request
 *
 * Receives incoming TCP packet on a socket, then parses the packet as
 * HTTP request and fills httpd_req_t data structure with the extracted
 * URI, headers are ready to be fetched from scratch buffer and calling
 * http_recv() after this reads the body of the request.
 *
 * @param[in] hd  Server instance data
 * @param[in] sd  Pointer to socket which is needed for receiving TCP packets.
 *
 * @return
 *  - ESP_OK    : if request packet is valid
 *  - ESP_FAIL  : otherwise
 */
esp_err_t httpd_req_new(struct httpd_data *hd, struct sock_db *sd);

/**
 * @brief   For an HTTP request, resets the resources allocated for it and
 *          purges any data left to be received
 *
 * @param[in] hd  Server instance data
 *
 * @return
 *  - ESP_OK    : if request packet deleted and resources cleaned.
 *  - ESP_FAIL  : otherwise.
 */
esp_err_t httpd_req_delete(struct httpd_data *hd);

/** End of Group : Parsing
 * @}
 */

/****************** Group : Send/Receive ********************/
/** @name Send and Receive
 * Methods for transmitting and receiving HTTP requests and responses
 * @{
 */

/**
 * @brief   For sending out data in response to an HTTP request.
 *
 * @param[in] req     Pointer to the HTTP request for which the response needs to be sent
 * @param[in] buf     Pointer to the buffer from where the body of the response is taken
 * @param[in] buf_len Length of the buffer
 *
 * @return
 *  - Length of data : if successful
 *  - ESP_FAIL       : if failed
 */
int httpd_send(httpd_req_t *req, const char *buf, size_t buf_len);

/**
 * @brief   For receiving HTTP request data
 *
 * @note    The exposed API httpd_recv() is simply this function with last parameter
 *          set as false. This function is used internally during reception and
 *          processing of a new request.
 *
 *          There are 2 options available that affect the behavior of the function:
 *          - HTTPD_RECV_OPT_HALT_AFTER_PENDING
 *            The option to halt after receiving pending data prevents the server from
 *            requesting more data than is needed for completing a packet in case when
 *            all the remaining part of the packet is in the pending buffer.
 *
 *          - HTTPD_RECV_OPT_BLOCKING
 *            The option to not return until the `buf_len` bytes have been read.
 *
 * @param[in]  req    Pointer to new HTTP request which only has the socket descriptor
 * @param[out] buf    Pointer to the buffer which will be filled with the received data
 * @param[in] buf_len Length of the buffer
 * @param[in] opt     Receive option
 *
 * @return
 *  - Length of data : if successful
 *  - ESP_FAIL       : if failed
 */
int httpd_recv_with_opt(httpd_req_t *r, char *buf, size_t buf_len, httpd_recv_opt_t opt);

/**
 * @brief   For un-receiving HTTP request data
 *
 * This function copies data into internal buffer pending_data so that
 * when httpd_recv is called, it first fetches this pending data and
 * then only starts receiving from the socket
 *
 * @note    If data is too large for the internal buffer then only
 *          part of the data is unreceived, reflected in the returned
 *          length. Make sure that such truncation is checked for and
 *          handled properly.
 *
 * @param[in] req     Pointer to new HTTP request which only has the socket descriptor
 * @param[in] buf     Pointer to the buffer from where data needs to be un-received
 * @param[in] buf_len Length of the buffer
 *
 * @return  Length of data copied into pending buffer
 */
size_t httpd_unrecv(struct httpd_req *r, const char *buf, size_t buf_len);

/**
 * @brief   This is the low level default send function of the HTTPD. This should
 *          NEVER be called directly. The semantics of this is exactly similar to
 *          send() of the BSD socket API.
 *
 * @param[in] hd      Server instance data
 * @param[in] sockfd  Socket descriptor for sending data
 * @param[in] buf     Pointer to the buffer from where the body of the response is taken
 * @param[in] buf_len Length of the buffer
 * @param[in] flags   Flags for mode selection
 *
 * @return
 *  - Length of data : if successful
 *  - -1             : if failed (appropriate errno is set)
 */
int httpd_default_send(httpd_handle_t hd, int sockfd, const char *buf, size_t buf_len, int flags);

/**
 * @brief   This is the low level default recv function of the HTTPD. This should
 *          NEVER be called directly. The semantics of this is exactly similar to
 *          recv() of the BSD socket API.
 *
 * @param[in] hd      Server instance data
 * @param[in] sockfd  Socket descriptor for sending data
 * @param[out] buf    Pointer to the buffer which will be filled with the received data
 * @param[in] buf_len Length of the buffer
 * @param[in] flags   Flags for mode selection
 *
 * @return
 *  - Length of data : if successful
 *  - -1             : if failed (appropriate errno is set)
 */
int httpd_default_recv(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags);

/** End of Group : Send and Receive
 * @}
 */

/* ************** Group: WebSocket ************** */
/** @name WebSocket
 * Functions for WebSocket header parsing
 * @{
 */


/**
 * @brief   This function is for responding a WebSocket handshake
 *
 * @param[in] req                       Pointer to handshake request that will be handled
 * @param[in] supported_subprotocol     Pointer to the subprotocol supported by this URI
 * @return
 *  - ESP_OK                        : When handshake is successful
 *  - ESP_ERR_NOT_FOUND             : When some headers (Sec-WebSocket-*) are not found
 *  - ESP_ERR_INVALID_VERSION       : The WebSocket version is not "13"
 *  - ESP_ERR_INVALID_STATE         : Handshake was done beforehand
 *  - ESP_ERR_INVALID_ARG           : Argument is invalid (null or non-WebSocket)
 *  - ESP_FAIL                      : Socket failures
 */
esp_err_t httpd_ws_respond_server_handshake(httpd_req_t *req, const char *supported_subprotocol);

/**
 * @brief   This function is for getting a frame type
 *          and responding a WebSocket control frame automatically
 *
 * @param[in] req    Pointer to handshake request that will be handled
 * @return
 *  - ESP_OK                        : When handshake is successful
 *  - ESP_ERR_INVALID_ARG           : Argument is invalid (null or non-WebSocket)
 *  - ESP_ERR_INVALID_STATE         : Received only some parts of a control frame
 *  - ESP_FAIL                      : Socket failures
 */
esp_err_t httpd_ws_get_frame_type(httpd_req_t *req);

/**
 * @brief   Trigger an httpd session close externally
 *
 * @note    Calling this API is only required in special circumstances wherein
 *          some application requires to close an httpd client session asynchronously.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] session   Session to be closed
 *
 * @return
 *  - ESP_OK    : On successfully initiating closure
 *  - ESP_FAIL  : Failure to queue work
 *  - ESP_ERR_NOT_FOUND   : Socket fd not found
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_sess_trigger_close_(httpd_handle_t handle, struct sock_db *session);

/**
 * @brief   Directly closes the least recently used session
 *
 * @param[in] hd  Server instance data
 *
 * @return
 *  - ESP_OK    : if session closed successfully
 */
esp_err_t httpd_sess_close_lru_direct(struct httpd_data *hd);

/** End of WebSocket related functions
 * @}
 */

#ifdef CONFIG_HTTPD_ENABLE_EVENTS

#if CONFIG_HTTPD_SERVER_EVENT_POST_TIMEOUT == -1
#define ESP_HTTP_SERVER_EVENT_POST_TIMEOUT portMAX_DELAY
#else
#define ESP_HTTP_SERVER_EVENT_POST_TIMEOUT pdMS_TO_TICKS(CONFIG_HTTPD_SERVER_EVENT_POST_TIMEOUT)
#endif

/**
 * @brief Function to dispatch events in default event loop
 *
 */
void esp_http_server_dispatch_event(int32_t event_id, const void* event_data, size_t event_data_size);

#else // CONFIG_HTTPD_ENABLE_EVENTS
static inline void esp_http_server_dispatch_event(int32_t event_id, const void* event_data, size_t event_data_size)
{
    // Events disabled, do nothing
    (void) event_id;
    (void) event_data;
    (void) event_data_size;
}
#endif // CONFIG_HTTPD_ENABLE_EVENTS

esp_err_t httpd_crypto_sha1(const uint8_t *data, size_t data_len, uint8_t *hash);

#ifdef __cplusplus
}
#endif

#endif /* ! _HTTPD_PRIV_H_ */
