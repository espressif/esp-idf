// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


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

#ifdef __cplusplus
extern "C" {
#endif

/* Size of request data block/chunk (not to be confused with chunked encoded data)
 * that is received and parsed in one turn of the parsing process. This should not
 * exceed the scratch buffer size and should at least be 8 bytes */
#define PARSER_BLOCK_SIZE  128

/* Calculate the maximum size needed for the scratch buffer */
#define HTTPD_SCRATCH_BUF  MAX(HTTPD_MAX_REQ_HDR_LEN, HTTPD_MAX_URI_LEN)

/* Formats a log string to prepend context function name */
#define LOG_FMT(x)      "%s: " x, __func__

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
    char pending_data[PARSER_BLOCK_SIZE];   /*!< Buffer for pending data to be received */
    size_t pending_len;                     /*!< Length of pending data to be received */
#ifdef CONFIG_HTTPD_WS_SUPPORT
    bool ws_handshake_done;                 /*!< True if it has done WebSocket handshake (if this socket is a valid WS) */
    bool ws_close;                          /*!< Set to true to close the socket later (when WS Close frame received) */
    esp_err_t (*ws_handler)(httpd_req_t *r);   /*!< WebSocket handler, leave to null if it's not WebSocket */
#endif
};

/**
 * @brief   Auxiliary data structure for use during reception and processing
 *          of requests and temporarily keeping responses
 */
struct httpd_req_aux {
    struct sock_db *sd;                             /*!< Pointer to socket database */
    char            scratch[HTTPD_SCRATCH_BUF + 1]; /*!< Temporary buffer for our operations (1 byte extra for null termination) */
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
    int msg_fd;                             /*!< Ctrl message sender FD */
    struct thread_data hd_td;               /*!< Information for the HTTPD thread */
    struct sock_db *hd_sd;                  /*!< The socket database */
    httpd_uri_t **hd_calls;                 /*!< Registered URI handlers */
    struct httpd_req hd_req;                /*!< The current HTTPD request */
    struct httpd_req_aux hd_req_aux;        /*!< Additional data about the HTTPD request kept unexposed */

    /* Array of registered error handler functions */
    httpd_err_handler_func_t *err_handler_fns;
};

/******************* Group : Session Management ********************/
/** @name Session Management
 * Functions related to HTTP session management
 * @{
 */

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
 * @param[in] hd    Server instance data
 * @param[in] clifd Descriptor of the client from which data is to be received
 *
 * @return
 *  - ESP_OK    : on successfully receiving, parsing and responding to a request
 *  - ESP_FAIL  : in case of failure in any of the stages of processing
 */
esp_err_t httpd_sess_process(struct httpd_data *hd, int clifd);

/**
 * @brief   Remove client descriptor from the session / socket database
 *          and close the connection for this client.
 *
 * @note    The returned descriptor should be used by httpd_sess_iterate()
 *          to continue the iteration correctly. This ensures that the
 *          iteration is not restarted abruptly which may cause reading from
 *          a socket which has been already processed and thus blocking
 *          the server loop until data appears on that socket.
 *
 * @param[in] hd    Server instance data
 * @param[in] clifd Descriptor of the client to be removed from the session.
 *
 * @return
 *  - +VE : Client descriptor preceding the one being deleted
 *  - -1  : No descriptor preceding the one being deleted
 */
int httpd_sess_delete(struct httpd_data *hd, int clifd);

/**
 * @brief   Free session context
 *
 * @param[in] ctx     Pointer to session context
 * @param[in] free_fn Free function to call on session context
 */
void httpd_sess_free_ctx(void *ctx, httpd_free_ctx_fn_t free_fn);

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
 * @brief   Iterates through the list of client fds in the session /socket database.
 *          Passing the value of a client fd returns the fd for the next client
 *          in the database. In order to iterate from the beginning pass -1 as fd.
 *
 * @param[in] hd    Server instance data
 * @param[in] fd    Last accessed client descriptor.
 *                  -1 to reset iterator to start of database.
 *
 * @return
 *  - +VE : Client descriptor next in the database
 *  - -1  : End of iteration
 */
int httpd_sess_iterate(struct httpd_data *hd, int fd);

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
 * @param[in] hd  Server instance data
 * @param[in] fd  Client descriptor
 *
 * @return True if there is any pending data
 */
bool httpd_sess_pending(struct httpd_data *hd, int fd);

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

/**
 * @brief   For handling HTTP errors by invoking registered
 *          error handler function
 *
 * @param[in] req     Pointer to the HTTP request for which error occurred
 * @param[in] error   Error type
 *
 * @return
 *  - ESP_OK    : error handled successful
 *  - ESP_FAIL  : failure indicates that the underlying socket needs to be closed
 */
esp_err_t httpd_req_handle_err(httpd_req_t *req, httpd_err_code_t error);

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
 *          processing of a new request. The option to halt after receiving pending
 *          data prevents the server from requesting more data than is needed for
 *          completing a packet in case when all the remaining part of the packet is
 *          in the pending buffer.
 *
 * @param[in]  req    Pointer to new HTTP request which only has the socket descriptor
 * @param[out] buf    Pointer to the buffer which will be filled with the received data
 * @param[in] buf_len Length of the buffer
 * @param[in] halt_after_pending When set true, halts immediately after receiving from
 *                               pending buffer
 *
 * @return
 *  - Length of data : if successful
 *  - ESP_FAIL       : if failed
 */
int httpd_recv_with_opt(httpd_req_t *r, char *buf, size_t buf_len, bool halt_after_pending);

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
 * @param[in] req    Pointer to handshake request that will be handled
 * @return
 *  - ESP_OK                        : When handshake is sucessful
 *  - ESP_ERR_NOT_FOUND             : When some headers (Sec-WebSocket-*) are not found
 *  - ESP_ERR_INVALID_VERSION       : The WebSocket version is not "13"
 *  - ESP_ERR_INVALID_STATE         : Handshake was done beforehand
 *  - ESP_ERR_INVALID_ARG           : Argument is invalid (null or non-WebSocket)
 *  - ESP_FAIL                      : Socket failures
 */
esp_err_t httpd_ws_respond_server_handshake(httpd_req_t *req);

/**
 * @brief   This function is for getting a frame type
 *          and responding a WebSocket control frame automatically
 *
 * @param[in] req    Pointer to handshake request that will be handled
 * @return
 *  - ESP_OK                        : When handshake is sucessful
 *  - ESP_ERR_INVALID_ARG           : Argument is invalid (null or non-WebSocket)
 *  - ESP_ERR_INVALID_STATE         : Received only some parts of a control frame
 *  - ESP_FAIL                      : Socket failures
 */
esp_err_t httpd_ws_get_frame_type(httpd_req_t *req);

/** End of WebSocket related functions
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ! _HTTPD_PRIV_H_ */
