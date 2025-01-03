/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_HTTP_SERVER_H_
#define _ESP_HTTP_SERVER_H_

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <http_parser.h>
#include <sdkconfig.h>
#include <esp_err.h>
#include <esp_event.h>
#include <esp_event_base.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_HTTPD_DEF_CTRL_PORT         (32768)    /*!< HTTP Server control socket port*/

ESP_EVENT_DECLARE_BASE(ESP_HTTP_SERVER_EVENT);

/**
 * @brief   HTTP Server events id
 */
typedef enum {
    HTTP_SERVER_EVENT_ERROR = 0,       /*!< This event occurs when there are any errors during execution */
    HTTP_SERVER_EVENT_START,           /*!< This event occurs when HTTP Server is started */
    HTTP_SERVER_EVENT_ON_CONNECTED,    /*!< Once the HTTP Server has been connected to the client, no data exchange has been performed */
    HTTP_SERVER_EVENT_ON_HEADER,       /*!< Occurs when receiving each header sent from the client */
    HTTP_SERVER_EVENT_HEADERS_SENT,     /*!< After sending all the headers to the client */
    HTTP_SERVER_EVENT_ON_DATA,         /*!< Occurs when receiving data from the client */
    HTTP_SERVER_EVENT_SENT_DATA,       /*!< Occurs when an ESP HTTP server session is finished */
    HTTP_SERVER_EVENT_DISCONNECTED,    /*!< The connection has been disconnected */
    HTTP_SERVER_EVENT_STOP,            /*!< This event occurs when HTTP Server is stopped */
} esp_http_server_event_id_t;

/** Argument structure for HTTP_SERVER_EVENT_ON_DATA and HTTP_SERVER_EVENT_SENT_DATA event */
typedef struct {
    int fd;         /*!< Session socket file descriptor */
    int data_len;   /*!< Data length */
} esp_http_server_event_data;

/*
note: esp_https_server.h includes a customized copy of this
initializer that should be kept in sync
*/
#define HTTPD_DEFAULT_CONFIG() {                        \
        .task_priority      = tskIDLE_PRIORITY+5,       \
        .stack_size         = 4096,                     \
        .core_id            = tskNO_AFFINITY,           \
        .task_caps          = (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),       \
        .server_port        = 80,                       \
        .ctrl_port          = ESP_HTTPD_DEF_CTRL_PORT,  \
        .max_open_sockets   = 7,                        \
        .max_uri_handlers   = 8,                        \
        .max_resp_headers   = 8,                        \
        .backlog_conn       = 5,                        \
        .lru_purge_enable   = false,                    \
        .recv_wait_timeout  = 5,                        \
        .send_wait_timeout  = 5,                        \
        .global_user_ctx = NULL,                        \
        .global_user_ctx_free_fn = NULL,                \
        .global_transport_ctx = NULL,                   \
        .global_transport_ctx_free_fn = NULL,           \
        .enable_so_linger = false,                      \
        .linger_timeout = 0,                            \
        .keep_alive_enable = false,                     \
        .keep_alive_idle = 0,                           \
        .keep_alive_interval = 0,                       \
        .keep_alive_count = 0,                          \
        .open_fn = NULL,                                \
        .close_fn = NULL,                               \
        .uri_match_fn = NULL                            \
}

#define ESP_ERR_HTTPD_BASE              (0xb000)                    /*!< Starting number of HTTPD error codes */
#define ESP_ERR_HTTPD_HANDLERS_FULL     (ESP_ERR_HTTPD_BASE +  1)   /*!< All slots for registering URI handlers have been consumed */
#define ESP_ERR_HTTPD_HANDLER_EXISTS    (ESP_ERR_HTTPD_BASE +  2)   /*!< URI handler with same method and target URI already registered */
#define ESP_ERR_HTTPD_INVALID_REQ       (ESP_ERR_HTTPD_BASE +  3)   /*!< Invalid request pointer */
#define ESP_ERR_HTTPD_RESULT_TRUNC      (ESP_ERR_HTTPD_BASE +  4)   /*!< Result string truncated */
#define ESP_ERR_HTTPD_RESP_HDR          (ESP_ERR_HTTPD_BASE +  5)   /*!< Response header field larger than supported */
#define ESP_ERR_HTTPD_RESP_SEND         (ESP_ERR_HTTPD_BASE +  6)   /*!< Error occurred while sending response packet */
#define ESP_ERR_HTTPD_ALLOC_MEM         (ESP_ERR_HTTPD_BASE +  7)   /*!< Failed to dynamically allocate memory for resource */
#define ESP_ERR_HTTPD_TASK              (ESP_ERR_HTTPD_BASE +  8)   /*!< Failed to launch server task/thread */

/* Symbol to be used as length parameter in httpd_resp_send APIs
 * for setting buffer length to string length */
#define HTTPD_RESP_USE_STRLEN -1

/* ************** Group: Initialization ************** */
/** @name Initialization
 * APIs related to the Initialization of the web server
 * @{
 */

/**
 * @brief   HTTP Server Instance Handle
 *
 * Every instance of the server will have a unique handle.
 */
typedef void* httpd_handle_t;

/**
 * @brief   HTTP Method Type wrapper over "enum http_method"
 *          available in "http_parser" library
 */
typedef enum http_method httpd_method_t;

#define HTTP_ANY INT_MAX

/**
 * @brief  Prototype for freeing context data (if any)
 * @param[in] ctx   object to free
 */
typedef void (*httpd_free_ctx_fn_t)(void *ctx);

/**
 * @brief  Function prototype for opening a session.
 *
 * Called immediately after the socket was opened to set up the send/recv functions and
 * other parameters of the socket.
 *
 * @param[in] hd       server instance
 * @param[in] sockfd   session socket file descriptor
 * @return
 *  - ESP_OK   : On success
 *  - Any value other than ESP_OK will signal the server to close the socket immediately
 */
typedef esp_err_t (*httpd_open_func_t)(httpd_handle_t hd, int sockfd);

/**
 * @brief  Function prototype for closing a session.
 *
 * @note   It's possible that the socket descriptor is invalid at this point, the function
 *         is called for all terminated sessions. Ensure proper handling of return codes.
 *
 * @param[in] hd   server instance
 * @param[in] sockfd   session socket file descriptor
 */
typedef void (*httpd_close_func_t)(httpd_handle_t hd, int sockfd);

/**
 * @brief  Function prototype for URI matching.
 *
 * @param[in] reference_uri   URI/template with respect to which the other URI is matched
 * @param[in] uri_to_match    URI/template being matched to the reference URI/template
 * @param[in] match_upto      For specifying the actual length of `uri_to_match` up to
 *                            which the matching algorithm is to be applied (The maximum
 *                            value is `strlen(uri_to_match)`, independent of the length
 *                            of `reference_uri`)
 * @return true on match
 */
typedef bool (*httpd_uri_match_func_t)(const char *reference_uri,
                                       const char *uri_to_match,
                                       size_t match_upto);

/**
 * @brief   HTTP Server Configuration Structure
 *
 * @note    Use HTTPD_DEFAULT_CONFIG() to initialize the configuration
 *          to a default value and then modify only those fields that are
 *          specifically determined by the use case.
 */
typedef struct httpd_config {
    unsigned    task_priority;      /*!< Priority of FreeRTOS task which runs the server */
    size_t      stack_size;         /*!< The maximum stack size allowed for the server task */
    BaseType_t  core_id;            /*!< The core the HTTP server task will run on */
    uint32_t    task_caps;          /*!< The memory capabilities to use when allocating the HTTP server task's stack */

    /**
     * TCP Port number for receiving and transmitting HTTP traffic
     */
    uint16_t    server_port;

    /**
     * UDP Port number for asynchronously exchanging control signals
     * between various components of the server
     */
    uint16_t    ctrl_port;

    uint16_t    max_open_sockets;   /*!< Max number of sockets/clients connected at any time (3 sockets are reserved for internal working of the HTTP server) */
    uint16_t    max_uri_handlers;   /*!< Maximum allowed uri handlers */
    uint16_t    max_resp_headers;   /*!< Maximum allowed additional headers in HTTP response */
    uint16_t    backlog_conn;       /*!< Number of backlog connections */
    bool        lru_purge_enable;   /*!< Purge "Least Recently Used" connection */
    uint16_t    recv_wait_timeout;  /*!< Timeout for recv function (in seconds)*/
    uint16_t    send_wait_timeout;  /*!< Timeout for send function (in seconds)*/

    /**
     * Global user context.
     *
     * This field can be used to store arbitrary user data within the server context.
     * The value can be retrieved using the server handle, available e.g. in the httpd_req_t struct.
     *
     * When shutting down, the server frees up the user context by
     * calling free() on the global_user_ctx field. If you wish to use a custom
     * function for freeing the global user context, please specify that here.
     */
    void * global_user_ctx;

    /**
     * Free function for global user context
     */
    httpd_free_ctx_fn_t global_user_ctx_free_fn;

    /**
     * Global transport context.
     *
     * Similar to global_user_ctx, but used for session encoding or encryption (e.g. to hold the SSL context).
     * It will be freed using free(), unless global_transport_ctx_free_fn is specified.
     */
    void * global_transport_ctx;

    /**
     * Free function for global transport context
     */
    httpd_free_ctx_fn_t global_transport_ctx_free_fn;

    bool enable_so_linger;  /*!< bool to enable/disable linger */
    int linger_timeout;     /*!< linger timeout (in seconds) */
    bool keep_alive_enable; /*!< Enable keep-alive timeout */
    int keep_alive_idle;    /*!< Keep-alive idle time. Default is 5 (second) */
    int keep_alive_interval;/*!< Keep-alive interval time. Default is 5 (second) */
    int keep_alive_count;   /*!< Keep-alive packet retry send count. Default is 3 counts */
    /**
     * Custom session opening callback.
     *
     * Called on a new session socket just after accept(), but before reading any data.
     *
     * This is an opportunity to set up e.g. SSL encryption using global_transport_ctx
     * and the send/recv/pending session overrides.
     *
     * If a context needs to be maintained between these functions, store it in the session using
     * httpd_sess_set_transport_ctx() and retrieve it later with httpd_sess_get_transport_ctx()
     *
     * Returning a value other than ESP_OK will immediately close the new socket.
     */
    httpd_open_func_t open_fn;

    /**
     * Custom session closing callback.
     *
     * Called when a session is deleted, before freeing user and transport contexts and before
     * closing the socket. This is a place for custom de-init code common to all sockets.
     *
     * The server will only close the socket if no custom session closing callback is set.
     * If a custom callback is used, `close(sockfd)` should be called in here for most cases.
     *
     * Set the user or transport context to NULL if it was freed here, so the server does not
     * try to free it again.
     *
     * This function is run for all terminated sessions, including sessions where the socket
     * was closed by the network stack - that is, the file descriptor may not be valid anymore.
     */
    httpd_close_func_t close_fn;

    /**
     * URI matcher function.
     *
     * Called when searching for a matching URI:
     *     1) whose request handler is to be executed right
     *        after an HTTP request is successfully parsed
     *     2) in order to prevent duplication while registering
     *        a new URI handler using `httpd_register_uri_handler()`
     *
     * Available options are:
     *     1) NULL : Internally do basic matching using `strncmp()`
     *     2) `httpd_uri_match_wildcard()` : URI wildcard matcher
     *
     * Users can implement their own matching functions (See description
     * of the `httpd_uri_match_func_t` function prototype)
     */
    httpd_uri_match_func_t uri_match_fn;
} httpd_config_t;

/**
 * @brief Starts the web server
 *
 * Create an instance of HTTP server and allocate memory/resources for it
 * depending upon the specified configuration.
 *
 * Example usage:
 * @code{c}
 *
 * //Function for starting the webserver
 * httpd_handle_t start_webserver(void)
 * {
 *      // Generate default configuration
 *      httpd_config_t config = HTTPD_DEFAULT_CONFIG();
 *
 *      // Empty handle to http_server
 *      httpd_handle_t server = NULL;
 *
 *      // Start the httpd server
 *      if (httpd_start(&server, &config) == ESP_OK) {
 *          // Register URI handlers
 *          httpd_register_uri_handler(server, &uri_get);
 *          httpd_register_uri_handler(server, &uri_post);
 *      }
 *      // If server failed to start, handle will be NULL
 *      return server;
 * }
 *
 * @endcode
 *
 * @param[in]  config   Configuration for new instance of the server
 * @param[out] handle   Handle to newly created instance of the server. NULL on error
 * @return
 *  - ESP_OK    : Instance created successfully
 *  - ESP_ERR_INVALID_ARG      : Null argument(s)
 *  - ESP_ERR_HTTPD_ALLOC_MEM  : Failed to allocate memory for instance
 *  - ESP_ERR_HTTPD_TASK       : Failed to launch server task
 */
esp_err_t httpd_start(httpd_handle_t *handle, const httpd_config_t *config);

/**
 * @brief Stops the web server
 *
 * Deallocates memory/resources used by an HTTP server instance and
 * deletes it. Once deleted the handle can no longer be used for accessing
 * the instance.
 *
 * Example usage:
 * @code{c}
 *
 * // Function for stopping the webserver
 * void stop_webserver(httpd_handle_t server)
 * {
 *      // Ensure handle is non NULL
 *      if (server != NULL) {
 *          // Stop the httpd server
 *          httpd_stop(server);
 *      }
 * }
 *
 * @endcode
 *
 * @param[in] handle Handle to server returned by httpd_start
 * @return
 *  - ESP_OK : Server stopped successfully
 *  - ESP_ERR_INVALID_ARG : Handle argument is Null
 */
esp_err_t httpd_stop(httpd_handle_t handle);

/** End of Group Initialization
 * @}
 */

/* ************** Group: URI Handlers ************** */
/** @name URI Handlers
 * APIs related to the URI handlers
 * @{
 */

/* Max supported HTTP request header length */
#define HTTPD_MAX_REQ_HDR_LEN CONFIG_HTTPD_MAX_REQ_HDR_LEN

/* Max supported HTTP request URI length */
#define HTTPD_MAX_URI_LEN CONFIG_HTTPD_MAX_URI_LEN

/**
 * @brief HTTP Request Data Structure
 */
typedef struct httpd_req {
    httpd_handle_t  handle;                     /*!< Handle to server instance */
    int             method;                     /*!< The type of HTTP request, -1 if unsupported method, HTTP_ANY for wildcard method to support every method */
    const char      uri[HTTPD_MAX_URI_LEN + 1]; /*!< The URI of this request (1 byte extra for null termination) */
    size_t          content_len;                /*!< Length of the request body */
    void           *aux;                        /*!< Internally used members */

    /**
     * User context pointer passed during URI registration.
     */
    void *user_ctx;

    /**
     * Session Context Pointer
     *
     * A session context. Contexts are maintained across 'sessions' for a
     * given open TCP connection. One session could have multiple request
     * responses. The web server will ensure that the context persists
     * across all these request and responses.
     *
     * By default, this is NULL. URI Handlers can set this to any meaningful
     * value.
     *
     * If the underlying socket gets closed, and this pointer is non-NULL,
     * the web server will free up the context by calling free(), unless
     * free_ctx function is set.
     */
    void *sess_ctx;

    /**
     * Pointer to free context hook
     *
     * Function to free session context
     *
     * If the web server's socket closes, it frees up the session context by
     * calling free() on the sess_ctx member. If you wish to use a custom
     * function for freeing the session context, please specify that here.
     */
    httpd_free_ctx_fn_t free_ctx;

    /**
     * Flag indicating if Session Context changes should be ignored
     *
     * By default, if you change the sess_ctx in some URI handler, the http server
     * will internally free the earlier context (if non NULL), after the URI handler
     * returns. If you want to manage the allocation/reallocation/freeing of
     * sess_ctx yourself, set this flag to true, so that the server will not
     * perform any checks on it. The context will be cleared by the server
     * (by calling free_ctx or free()) only if the socket gets closed.
     */
    bool ignore_sess_ctx_changes;
} httpd_req_t;

/**
 * @brief Structure for URI handler
 */
typedef struct httpd_uri {
    const char       *uri;    /*!< The URI to handle */
    httpd_method_t    method; /*!< Method supported by the URI, HTTP_ANY for wildcard method to support all methods*/

    /**
     * Handler to call for supported request method. This must
     * return ESP_OK, or else the underlying socket will be closed.
     */
    esp_err_t (*handler)(httpd_req_t *r);

    /**
     * Pointer to user context data which will be available to handler
     */
    void *user_ctx;

#ifdef CONFIG_HTTPD_WS_SUPPORT
    /**
     * Flag for indicating a WebSocket endpoint.
     * If this flag is true, then method must be HTTP_GET. Otherwise the handshake will not be handled.
     */
    bool is_websocket;

    /**
     * Flag indicating that control frames (PING, PONG, CLOSE) are also passed to the handler
     * This is used if a custom processing of the control frames is needed
     */
    bool handle_ws_control_frames;

    /**
     * Pointer to subprotocol supported by URI
     */
    const char *supported_subprotocol;
#endif
} httpd_uri_t;

/**
 * @brief   Registers a URI handler
 *
 * @note    URI handlers can be registered in real time as long as the
 *          server handle is valid.
 *
 * Example usage:
 * @code{c}
 *
 * esp_err_t my_uri_handler(httpd_req_t* req)
 * {
 *     // Recv , Process and Send
 *     ....
 *     ....
 *     ....
 *
 *     // Fail condition
 *     if (....) {
 *         // Return fail to close session //
 *         return ESP_FAIL;
 *     }
 *
 *     // On success
 *     return ESP_OK;
 * }
 *
 * // URI handler structure
 * httpd_uri_t my_uri {
 *     .uri      = "/my_uri/path/xyz",
 *     .method   = HTTPD_GET,
 *     .handler  = my_uri_handler,
 *     .user_ctx = NULL
 * };
 *
 * // Register handler
 * if (httpd_register_uri_handler(server_handle, &my_uri) != ESP_OK) {
 *    // If failed to register handler
 *    ....
 * }
 *
 * @endcode
 *
 * @param[in] handle      handle to HTTPD server instance
 * @param[in] uri_handler pointer to handler that needs to be registered
 *
 * @return
 *  - ESP_OK : On successfully registering the handler
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_HANDLERS_FULL  : If no slots left for new handler
 *  - ESP_ERR_HTTPD_HANDLER_EXISTS : If handler with same URI and
 *                                   method is already registered
 */
esp_err_t httpd_register_uri_handler(httpd_handle_t handle,
                                     const httpd_uri_t *uri_handler);

/**
 * @brief   Unregister a URI handler
 *
 * @param[in] handle    handle to HTTPD server instance
 * @param[in] uri       URI string
 * @param[in] method    HTTP method
 *
 * @return
 *  - ESP_OK : On successfully deregistering the handler
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_NOT_FOUND   : Handler with specified URI and method not found
 */
esp_err_t httpd_unregister_uri_handler(httpd_handle_t handle,
                                       const char *uri, httpd_method_t method);

/**
 * @brief   Unregister all URI handlers with the specified uri string
 *
 * @param[in] handle   handle to HTTPD server instance
 * @param[in] uri      uri string specifying all handlers that need
 *                     to be deregisterd
 *
 * @return
 *  - ESP_OK : On successfully deregistering all such handlers
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_NOT_FOUND   : No handler registered with specified uri string
 */
esp_err_t httpd_unregister_uri(httpd_handle_t handle, const char* uri);

/** End of URI Handlers
 * @}
 */

/* ************** Group: HTTP Error ************** */
/** @name HTTP Error
 * Prototype for HTTP errors and error handling functions
 * @{
 */

/**
 * @brief Error codes sent as HTTP response in case of errors
 *        encountered during processing of an HTTP request
 */
typedef enum {
    /* For any unexpected errors during parsing, like unexpected
     * state transitions, or unhandled errors.
     */
    HTTPD_500_INTERNAL_SERVER_ERROR = 0,

    /* For methods not supported by http_parser. Presently
     * http_parser halts parsing when such methods are
     * encountered and so the server responds with 400 Bad
     * Request error instead.
     */
    HTTPD_501_METHOD_NOT_IMPLEMENTED,

    /* When HTTP version is not 1.1 or 1.0*/
    HTTPD_505_VERSION_NOT_SUPPORTED,

    /* Returned when http_parser halts parsing due to incorrect
     * syntax of request, unsupported method in request URI or
     * due to chunked encoding / upgrade field present in headers
     */
    HTTPD_400_BAD_REQUEST,

    /* This response means the client must authenticate itself
     * to get the requested response.
     */
    HTTPD_401_UNAUTHORIZED,

    /* The client does not have access rights to the content,
     * so the server is refusing to give the requested resource.
     * Unlike 401, the client's identity is known to the server.
     */
    HTTPD_403_FORBIDDEN,

    /* When requested URI is not found */
    HTTPD_404_NOT_FOUND,

    /* When URI found, but method has no handler registered */
    HTTPD_405_METHOD_NOT_ALLOWED,

    /* Intended for recv timeout. Presently it's being sent
     * for other recv errors as well. Client should expect the
     * server to immediately close the connection after
     * responding with this.
     */
    HTTPD_408_REQ_TIMEOUT,

    /* Intended for responding to chunked encoding, which is
     * not supported currently. Though unhandled http_parser
     * callback for chunked request returns "400 Bad Request"
     */
    HTTPD_411_LENGTH_REQUIRED,

    /* Incoming payload is too large */
    HTTPD_413_CONTENT_TOO_LARGE,

    /* URI length greater than CONFIG_HTTPD_MAX_URI_LEN */
    HTTPD_414_URI_TOO_LONG,

    /* Headers section larger than CONFIG_HTTPD_MAX_REQ_HDR_LEN */
    HTTPD_431_REQ_HDR_FIELDS_TOO_LARGE,

    /* Used internally for retrieving the total count of errors */
    HTTPD_ERR_CODE_MAX
} httpd_err_code_t;

/**
 * @brief  Function prototype for HTTP error handling.
 *
 * This function is executed upon HTTP errors generated during
 * internal processing of an HTTP request. This is used to override
 * the default behavior on error, which is to send HTTP error response
 * and close the underlying socket.
 *
 * @note
 *  - If implemented, the server will not automatically send out HTTP
 *    error response codes, therefore, httpd_resp_send_err() must be
 *    invoked inside this function if user wishes to generate HTTP
 *    error responses.
 *  - When invoked, the validity of `uri`, `method`, `content_len`
 *    and `user_ctx` fields of the httpd_req_t parameter is not
 *    guaranteed as the HTTP request may be partially received/parsed.
 *  - The function must return ESP_OK if underlying socket needs to
 *    be kept open. Any other value will ensure that the socket is
 *    closed. The return value is ignored when error is of type
 *    `HTTPD_500_INTERNAL_SERVER_ERROR` and the socket closed anyway.
 *
 * @param[in] req    HTTP request for which the error needs to be handled
 * @param[in] error  Error type
 *
 * @return
 *  - ESP_OK   : error handled successful
 *  - ESP_FAIL : failure indicates that the underlying socket needs to be closed
 */
typedef esp_err_t (*httpd_err_handler_func_t)(httpd_req_t *req,
                                              httpd_err_code_t error);

/**
 * @brief  Function for registering HTTP error handlers
 *
 * This function maps a handler function to any supported error code
 * given by `httpd_err_code_t`. See prototype `httpd_err_handler_func_t`
 * above for details.
 *
 * @param[in] handle     HTTP server handle
 * @param[in] error      Error type
 * @param[in] handler_fn User implemented handler function
 *                       (Pass NULL to unset any previously set handler)
 *
 * @return
 *  - ESP_OK : handler registered successfully
 *  - ESP_ERR_INVALID_ARG : invalid error code or server handle
 */
esp_err_t httpd_register_err_handler(httpd_handle_t handle,
                                     httpd_err_code_t error,
                                     httpd_err_handler_func_t handler_fn);

/** End of HTTP Error
 * @}
 */

/* ************** Group: TX/RX ************** */
/** @name TX / RX
 * Prototype for HTTPDs low-level send/recv functions
 * @{
 */

#define HTTPD_SOCK_ERR_FAIL      -1
#define HTTPD_SOCK_ERR_INVALID   -2
#define HTTPD_SOCK_ERR_TIMEOUT   -3

/**
 * @brief  Prototype for HTTPDs low-level send function
 *
 * @note   User specified send function must handle errors internally,
 *         depending upon the set value of errno, and return specific
 *         HTTPD_SOCK_ERR_ codes, which will eventually be conveyed as
 *         return value of httpd_send() function
 *
 * @param[in] hd        server instance
 * @param[in] sockfd    session socket file descriptor
 * @param[in] buf       buffer with bytes to send
 * @param[in] buf_len   data size
 * @param[in] flags     flags for the send() function
 * @return
 *  - Bytes : The number of bytes sent successfully
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket send()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket send()
 */
typedef int (*httpd_send_func_t)(httpd_handle_t hd, int sockfd, const char *buf, size_t buf_len, int flags);

/**
 * @brief  Prototype for HTTPDs low-level recv function
 *
 * @note   User specified recv function must handle errors internally,
 *         depending upon the set value of errno, and return specific
 *         HTTPD_SOCK_ERR_ codes, which will eventually be conveyed as
 *         return value of httpd_req_recv() function
 *
 * @param[in] hd        server instance
 * @param[in] sockfd    session socket file descriptor
 * @param[in] buf       buffer with bytes to send
 * @param[in] buf_len   data size
 * @param[in] flags     flags for the send() function
 * @return
 *  - Bytes : The number of bytes received successfully
 *  - 0     : Buffer length parameter is zero / connection closed by peer
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket recv()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket recv()
 */
typedef int (*httpd_recv_func_t)(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags);

/**
 * @brief  Prototype for HTTPDs low-level "get pending bytes" function
 *
 * @note   User specified pending function must handle errors internally,
 *         depending upon the set value of errno, and return specific
 *         HTTPD_SOCK_ERR_ codes, which will be handled accordingly in
 *         the server task.
 *
 * @param[in] hd       server instance
 * @param[in] sockfd   session socket file descriptor
 * @return
 *  - Bytes : The number of bytes waiting to be received
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket pending()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket pending()
 */
typedef int (*httpd_pending_func_t)(httpd_handle_t hd, int sockfd);

/** End of TX / RX
 * @}
 */

/* ************** Group: Request/Response ************** */
/** @name Request / Response
 * APIs related to the data send/receive by URI handlers.
 * These APIs are supposed to be called only from the context of
 * a URI handler where httpd_req_t* request pointer is valid.
 * @{
 */

/**
 * @brief   Override web server's receive function (by session FD)
 *
 * This function overrides the web server's receive function. This same function is
 * used to read HTTP request packets.
 *
 * @note    This API is supposed to be called either from the context of
 *          - an http session APIs where sockfd is a valid parameter
 *          - a URI handler where sockfd is obtained using httpd_req_to_sockfd()
 *
 * @param[in] hd        HTTPD instance handle
 * @param[in] sockfd    Session socket FD
 * @param[in] recv_func The receive function to be set for this session
 *
 * @return
 *  - ESP_OK : On successfully registering override
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_sess_set_recv_override(httpd_handle_t hd, int sockfd, httpd_recv_func_t recv_func);

/**
 * @brief   Override web server's send function (by session FD)
 *
 * This function overrides the web server's send function. This same function is
 * used to send out any response to any HTTP request.
 *
 * @note    This API is supposed to be called either from the context of
 *          - an http session APIs where sockfd is a valid parameter
 *          - a URI handler where sockfd is obtained using httpd_req_to_sockfd()
 *
 * @param[in] hd        HTTPD instance handle
 * @param[in] sockfd    Session socket FD
 * @param[in] send_func The send function to be set for this session
 *
 * @return
 *  - ESP_OK : On successfully registering override
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_sess_set_send_override(httpd_handle_t hd, int sockfd, httpd_send_func_t send_func);

/**
 * @brief   Override web server's pending function (by session FD)
 *
 * This function overrides the web server's pending function. This function is
 * used to test for pending bytes in a socket.
 *
 * @note    This API is supposed to be called either from the context of
 *          - an http session APIs where sockfd is a valid parameter
 *          - a URI handler where sockfd is obtained using httpd_req_to_sockfd()
 *
 * @param[in] hd           HTTPD instance handle
 * @param[in] sockfd       Session socket FD
 * @param[in] pending_func The receive function to be set for this session
 *
 * @return
 *  - ESP_OK : On successfully registering override
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_sess_set_pending_override(httpd_handle_t hd, int sockfd, httpd_pending_func_t pending_func);

/**
 * @brief   Start an asynchronous request. This function can be called
 *          in a request handler to get a request copy that can be used on a async thread.
 *
 * @note
 * - This function is necessary in order to handle multiple requests simultaneously.
 * See examples/async_requests for example usage.
 * - You must call httpd_req_async_handler_complete() when you are done with the request.
 *
 * @param[in]   r       The request to create an async copy of
 * @param[out]  out     A newly allocated request which can be used on an async thread
 *
 * @return
 *  - ESP_OK : async request object created
 */
esp_err_t httpd_req_async_handler_begin(httpd_req_t *r, httpd_req_t **out);

/**
 * @brief   Mark an asynchronous request as completed. This will
 *  - free the request memory
 *  - relinquish ownership of the underlying socket, so it can be reused.
 *  - allow the http server to close our socket if needed (lru_purge_enable)
 *
 * @note If async requests are not marked completed, eventually the server
 * will no longer accept incoming connections. The server will log a
 * "httpd_accept_conn: error in accept (23)" message if this happens.
 *
 * @param[in]   r   The request to mark async work as completed
 *
 * @return
 *  - ESP_OK : async request was marked completed
 */
esp_err_t httpd_req_async_handler_complete(httpd_req_t *r);

/**
 * @brief   Get the Socket Descriptor from the HTTP request
 *
 * This API will return the socket descriptor of the session for
 * which URI handler was executed on reception of HTTP request.
 * This is useful when user wants to call functions that require
 * session socket fd, from within a URI handler, ie. :
 *      httpd_sess_get_ctx(),
 *      httpd_sess_trigger_close(),
 *      httpd_sess_update_lru_counter().
 *
 * @note    This API is supposed to be called only from the context of
 *          a URI handler where httpd_req_t* request pointer is valid.
 *
 * @param[in] r The request whose socket descriptor should be found
 *
 * @return
 *  - Socket descriptor : The socket descriptor for this request
 *  - -1 : Invalid/NULL request pointer
 */
int httpd_req_to_sockfd(httpd_req_t *r);

/**
 * @brief   API to read content data from the HTTP request
 *
 * This API will read HTTP content data from the HTTP request into
 * provided buffer. Use content_len provided in httpd_req_t structure
 * to know the length of data to be fetched. If content_len is too
 * large for the buffer then user may have to make multiple calls to
 * this function, each time fetching 'buf_len' number of bytes,
 * while the pointer to content data is incremented internally by
 * the same number.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - If an error is returned, the URI handler must further return an error.
 *    This will ensure that the erroneous socket is closed and cleaned up by
 *    the web server.
 *  - Presently Chunked Encoding is not supported
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Pointer to a buffer that the data will be read into
 * @param[in] buf_len   Length of the buffer
 *
 * @return
 *  - Bytes : Number of bytes read into the buffer successfully
 *  - 0     : Buffer length parameter is zero / connection closed by peer
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket recv()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket recv()
 */
int httpd_req_recv(httpd_req_t *r, char *buf, size_t buf_len);

/**
 * @brief   Search for a field in request headers and
 *          return the string length of it's value
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once httpd_resp_send() API is called all request headers
 *    are purged, so request headers need be copied into separate
 *    buffers if they are required later.
 *
 * @param[in]  r        The request being responded to
 * @param[in]  field    The header field to be searched in the request
 *
 * @return
 *  - Length    : If field is found in the request URL
 *  - Zero      : Field not found / Invalid request / Null arguments
 */
size_t httpd_req_get_hdr_value_len(httpd_req_t *r, const char *field);

/**
 * @brief   Get the value string of a field from the request headers
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once httpd_resp_send() API is called all request headers
 *    are purged, so request headers need be copied into separate
 *    buffers if they are required later.
 *  - If output size is greater than input, then the value is truncated,
 *    accompanied by truncation error as return value.
 *  - Use httpd_req_get_hdr_value_len() to know the right buffer length
 *
 * @param[in]  r        The request being responded to
 * @param[in]  field    The field to be searched in the header
 * @param[out] val      Pointer to the buffer into which the value will be copied if the field is found
 * @param[in]  val_size Size of the user buffer "val"
 *
 * @return
 *  - ESP_OK : Field found in the request header and value string copied
 *  - ESP_ERR_NOT_FOUND          : Key not found
 *  - ESP_ERR_INVALID_ARG        : Null arguments
 *  - ESP_ERR_HTTPD_INVALID_REQ  : Invalid HTTP request pointer
 *  - ESP_ERR_HTTPD_RESULT_TRUNC : Value string truncated
 */
esp_err_t httpd_req_get_hdr_value_str(httpd_req_t *r, const char *field, char *val, size_t val_size);

/**
 * @brief   Get Query string length from the request URL
 *
 * @note    This API is supposed to be called only from the context of
 *          a URI handler where httpd_req_t* request pointer is valid
 *
 * @param[in]  r    The request being responded to
 *
 * @return
 *  - Length    : Query is found in the request URL
 *  - Zero      : Query not found / Null arguments / Invalid request / uri is empty
 */
size_t httpd_req_get_url_query_len(httpd_req_t *r);

/**
 * @brief   Get Query string from the request URL
 *
 * @note
 *  - Presently, the user can fetch the full URL query string, but decoding
 *    will have to be performed by the user. Request headers can be read using
 *    httpd_req_get_hdr_value_str() to know the 'Content-Type' (eg. Content-Type:
 *    application/x-www-form-urlencoded) and then the appropriate decoding
 *    algorithm needs to be applied.
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid
 *  - If output size is greater than input, then the value is truncated,
 *    accompanied by truncation error as return value
 *  - Prior to calling this function, one can use httpd_req_get_url_query_len()
 *    to know the query string length beforehand and hence allocate the buffer
 *    of right size (usually query string length + 1 for null termination)
 *    for storing the query string
 *
 * @param[in]  r         The request being responded to
 * @param[out] buf       Pointer to the buffer into which the query string will be copied (if found)
 * @param[in]  buf_len   Length of output buffer
 *
 * @return
 *  - ESP_OK : Query is found in the request URL and copied to buffer
 *  - ESP_FAIL                   : uri is empty
 *  - ESP_ERR_NOT_FOUND          : Query not found
 *  - ESP_ERR_INVALID_ARG        : Null arguments
 *  - ESP_ERR_HTTPD_INVALID_REQ  : Invalid HTTP request pointer
 *  - ESP_ERR_HTTPD_RESULT_TRUNC : Query string truncated
 */
esp_err_t httpd_req_get_url_query_str(httpd_req_t *r, char *buf, size_t buf_len);

/**
 * @brief   Helper function to get a URL query tag from a query
 *          string of the type param1=val1&param2=val2
 *
 * @note
 *  - The components of URL query string (keys and values) are not URLdecoded.
 *    The user must check for 'Content-Type' field in the request headers and
 *    then depending upon the specified encoding (URLencoded or otherwise) apply
 *    the appropriate decoding algorithm.
 *  - If actual value size is greater than val_size, then the value is truncated,
 *    accompanied by truncation error as return value.
 *
 * @param[in]  qry       Pointer to query string
 * @param[in]  key       The key to be searched in the query string
 * @param[out] val       Pointer to the buffer into which the value will be copied if the key is found
 * @param[in]  val_size  Size of the user buffer "val"
 *
 * @return
 *  - ESP_OK : Key is found in the URL query string and copied to buffer
 *  - ESP_ERR_NOT_FOUND          : Key not found
 *  - ESP_ERR_INVALID_ARG        : Null arguments
 *  - ESP_ERR_HTTPD_RESULT_TRUNC : Value string truncated
 */
esp_err_t httpd_query_key_value(const char *qry, const char *key, char *val, size_t val_size);

/**
 * @brief   Get the value string of a cookie value from the "Cookie" request headers by cookie name.
 *
 * @param[in]       req             Pointer to the HTTP request
 * @param[in]       cookie_name     The cookie name to be searched in the request
 * @param[out]      val             Pointer to the buffer into which the value of cookie will be copied if the cookie is found
 * @param[inout]    val_size        Pointer to size of the user buffer "val". This variable will contain cookie length if
 *                                  ESP_OK is returned and required buffer length in case ESP_ERR_HTTPD_RESULT_TRUNC is returned.
 *
 * @return
 *  - ESP_OK : Key is found in the cookie string and copied to buffer
 *  - ESP_ERR_NOT_FOUND          : Key not found
 *  - ESP_ERR_INVALID_ARG        : Null arguments
 *  - ESP_ERR_HTTPD_RESULT_TRUNC : Value string truncated
 *  - ESP_ERR_NO_MEM             : Memory allocation failure
 */
esp_err_t httpd_req_get_cookie_val(httpd_req_t *req, const char *cookie_name, char *val, size_t *val_size);

/**
 * @brief Test if a URI matches the given wildcard template.
 *
 * Template may end with '?' to make the previous character optional (typically a slash),
 * '*' for a wildcard match, and '?*' to make the previous character optional, and if present,
 * allow anything to follow.
 *
 * Example:
 *   - * matches everything
 *   - /api/? matches /api and /api/
 *   - /api/\* (sans the backslash) matches /api/ and /api/status, but not /api or /ap
 *   - /api/?* or /api/\*?  (sans the backslash) matches /api/, /api/status, and also /api, but not /apix or /ap
 *
 * The special characters '?' and '*' anywhere else in the template will be taken literally.
 *
 * @param[in] uri_template   URI template (pattern)
 * @param[in] uri_to_match   URI to be matched
 * @param[in] match_upto     how many characters of the URI buffer to test
 *                          (there may be trailing query string etc.)
 *
 * @return true if a match was found
 */
bool httpd_uri_match_wildcard(const char *uri_template, const char *uri_to_match, size_t match_upto);

/**
 * @brief   API to send a complete HTTP response.
 *
 * This API will send the data as an HTTP response to the request.
 * This assumes that you have the entire response ready in a single
 * buffer. If you wish to send response in incremental chunks use
 * httpd_resp_send_chunk() instead.
 *
 * If no status code and content-type were set, by default this
 * will send 200 OK status code and content type as text/html.
 * You may call the following functions before this API to configure
 * the response headers :
 *      httpd_resp_set_status() - for setting the HTTP status string,
 *      httpd_resp_set_type()   - for setting the Content Type,
 *      httpd_resp_set_hdr()    - for appending any additional field
 *                                value entries in the response header
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, the request has been responded to.
 *  - No additional data can then be sent for the request.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Buffer from where the content is to be fetched
 * @param[in] buf_len   Length of the buffer, HTTPD_RESP_USE_STRLEN to use strlen()
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null request pointer
 *  - ESP_ERR_HTTPD_RESP_HDR    : Essential headers are too large for internal buffer
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request
 */
esp_err_t httpd_resp_send(httpd_req_t *r, const char *buf, ssize_t buf_len);

/**
 * @brief   API to send one HTTP chunk
 *
 * This API will send the data as an HTTP response to the
 * request. This API will use chunked-encoding and send the response
 * in the form of chunks. If you have the entire response contained in
 * a single buffer, please use httpd_resp_send() instead.
 *
 * If no status code and content-type were set, by default this will
 * send 200 OK status code and content type as text/html. You may
 * call the following functions before this API to configure the
 * response headers
 *      httpd_resp_set_status() - for setting the HTTP status string,
 *      httpd_resp_set_type()   - for setting the Content Type,
 *      httpd_resp_set_hdr()    - for appending any additional field
 *                                value entries in the response header
 *
 * @note
 * - This API is supposed to be called only from the context of
 *   a URI handler where httpd_req_t* request pointer is valid.
 * - When you are finished sending all your chunks, you must call
 *   this function with buf_len as 0.
 * - Once this API is called, all request headers are purged, so
 *   request headers need be copied into separate buffers if they
 *   are required later.
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Pointer to a buffer that stores the data
 * @param[in] buf_len   Length of the buffer, HTTPD_RESP_USE_STRLEN to use strlen()
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet chunk
 *  - ESP_ERR_INVALID_ARG : Null request pointer
 *  - ESP_ERR_HTTPD_RESP_HDR    : Essential headers are too large for internal buffer
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_resp_send_chunk(httpd_req_t *r, const char *buf, ssize_t buf_len);

/**
 * @brief   API to send a complete string as HTTP response.
 *
 * This API simply calls http_resp_send with buffer length
 * set to string length assuming the buffer contains a null
 * terminated string
 *
 * @param[in] r         The request being responded to
 * @param[in] str       String to be sent as response body
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null request pointer
 *  - ESP_ERR_HTTPD_RESP_HDR    : Essential headers are too large for internal buffer
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request
 */
static inline esp_err_t httpd_resp_sendstr(httpd_req_t *r, const char *str) {
    return httpd_resp_send(r, str, (str == NULL) ? 0 : HTTPD_RESP_USE_STRLEN);
}

/**
 * @brief   API to send a string as an HTTP response chunk.
 *
 * This API simply calls http_resp_send_chunk with buffer length
 * set to string length assuming the buffer contains a null
 * terminated string
 *
 * @param[in] r    The request being responded to
 * @param[in] str  String to be sent as response body (NULL to finish response packet)
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null request pointer
 *  - ESP_ERR_HTTPD_RESP_HDR    : Essential headers are too large for internal buffer
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request
 */
static inline esp_err_t httpd_resp_sendstr_chunk(httpd_req_t *r, const char *str) {
    return httpd_resp_send_chunk(r, str, (str == NULL) ? 0 : HTTPD_RESP_USE_STRLEN);
}

/* Some commonly used status codes */
#define HTTPD_200      "200 OK"                     /*!< HTTP Response 200 */
#define HTTPD_204      "204 No Content"             /*!< HTTP Response 204 */
#define HTTPD_207      "207 Multi-Status"           /*!< HTTP Response 207 */
#define HTTPD_400      "400 Bad Request"            /*!< HTTP Response 400 */
#define HTTPD_404      "404 Not Found"              /*!< HTTP Response 404 */
#define HTTPD_408      "408 Request Timeout"        /*!< HTTP Response 408 */
#define HTTPD_500      "500 Internal Server Error"  /*!< HTTP Response 500 */

/**
 * @brief   API to set the HTTP status code
 *
 * This API sets the status of the HTTP response to the value specified.
 * By default, the '200 OK' response is sent as the response.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - This API only sets the status to this value. The status isn't
 *    sent out until any of the send APIs is executed.
 *  - Make sure that the lifetime of the status string is valid till
 *    send function is called.
 *
 * @param[in] r         The request being responded to
 * @param[in] status    The HTTP status code of this response
 *
 * @return
 *  - ESP_OK : On success
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_resp_set_status(httpd_req_t *r, const char *status);

/* Some commonly used content types */
#define HTTPD_TYPE_JSON   "application/json"            /*!< HTTP Content type JSON */
#define HTTPD_TYPE_TEXT   "text/html"                   /*!< HTTP Content type text/HTML */
#define HTTPD_TYPE_OCTET  "application/octet-stream"    /*!< HTTP Content type octext-stream */

/**
 * @brief   API to set the HTTP content type
 *
 * This API sets the 'Content Type' field of the response.
 * The default content type is 'text/html'.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - This API only sets the content type to this value. The type
 *    isn't sent out until any of the send APIs is executed.
 *  - Make sure that the lifetime of the type string is valid till
 *    send function is called.
 *
 * @param[in] r     The request being responded to
 * @param[in] type  The Content Type of the response
 *
 * @return
 *  - ESP_OK   : On success
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_resp_set_type(httpd_req_t *r, const char *type);

/**
 * @brief   API to append any additional headers
 *
 * This API sets any additional header fields that need to be sent in the response.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - The header isn't sent out until any of the send APIs is executed.
 *  - The maximum allowed number of additional headers is limited to
 *    value of max_resp_headers in config structure.
 *  - Make sure that the lifetime of the field value strings are valid till
 *    send function is called.
 *
 * @param[in] r     The request being responded to
 * @param[in] field The field name of the HTTP header
 * @param[in] value The value of this HTTP header
 *
 * @return
 *  - ESP_OK : On successfully appending new header
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_RESP_HDR    : Total additional headers exceed max allowed
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_resp_set_hdr(httpd_req_t *r, const char *field, const char *value);

/**
 * @brief   For sending out error code in response to HTTP request.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *  - If you wish to send additional data in the body of the
 *    response, please use the lower-level functions directly.
 *
 * @param[in] req     Pointer to the HTTP request for which the response needs to be sent
 * @param[in] error   Error type to send
 * @param[in] msg     Error message string (pass NULL for default message)
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_resp_send_err(httpd_req_t *req, httpd_err_code_t error, const char *msg);

/**
 * @brief   For sending out custom error code in response to HTTP request.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *  - If you wish to send additional data in the body of the
 *    response, please use the lower-level functions directly.
 *
 * @param[in] req     Pointer to the HTTP request for which the response needs to be sent
 * @param[in] status  Error status to send
 * @param[in] msg     Error message string
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_resp_send_custom_err(httpd_req_t *req, const char *status, const char *msg);

/**
 * @brief   Helper function for HTTP 404
 *
 * Send HTTP 404 message. If you wish to send additional data in the body of the
 * response, please use the lower-level functions directly.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r The request being responded to
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
static inline esp_err_t httpd_resp_send_404(httpd_req_t *r) {
    return httpd_resp_send_err(r, HTTPD_404_NOT_FOUND, NULL);
}

/**
 * @brief   Helper function for HTTP 408
 *
 * Send HTTP 408 message. If you wish to send additional data in the body of the
 * response, please use the lower-level functions directly.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r The request being responded to
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
static inline esp_err_t httpd_resp_send_408(httpd_req_t *r) {
    return httpd_resp_send_err(r, HTTPD_408_REQ_TIMEOUT, NULL);
}

/**
 * @brief   Helper function for HTTP 500
 *
 * Send HTTP 500 message. If you wish to send additional data in the body of the
 * response, please use the lower-level functions directly.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Once this API is called, all request headers are purged, so
 *    request headers need be copied into separate buffers if
 *    they are required later.
 *
 * @param[in] r The request being responded to
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
static inline esp_err_t httpd_resp_send_500(httpd_req_t *r) {
    return httpd_resp_send_err(r, HTTPD_500_INTERNAL_SERVER_ERROR, NULL);
}

/**
 * @brief   Raw HTTP send
 *
 * Call this API if you wish to construct your custom response packet.
 * When using this, all essential header, eg. HTTP version, Status Code,
 * Content Type and Length, Encoding, etc. will have to be constructed
 * manually, and HTTP delimiters (CRLF) will need to be placed correctly
 * for separating sub-sections of the HTTP response packet.
 *
 * If the send override function is set, this API will end up
 * calling that function eventually to send data out.
 *
 * @note
 *  - This API is supposed to be called only from the context of
 *    a URI handler where httpd_req_t* request pointer is valid.
 *  - Unless the response has the correct HTTP structure (which the
 *    user must now ensure) it is not guaranteed that it will be
 *    recognized by the client. For most cases, you wouldn't have
 *    to call this API, but you would rather use either of :
 *          httpd_resp_send(),
 *          httpd_resp_send_chunk()
 *
 * @param[in] r         The request being responded to
 * @param[in] buf       Buffer from where the fully constructed packet is to be read
 * @param[in] buf_len   Length of the buffer
 *
 * @return
 *  - Bytes : Number of bytes that were sent successfully
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket send()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket send()
 */
int httpd_send(httpd_req_t *r, const char *buf, size_t buf_len);

/**
 * A low level API to send data on a given socket
 *
 * @note This API is not recommended to be used in any request handler.
 * Use this only for advanced use cases, wherein some asynchronous
 * data is to be sent over a socket.
 *
 * This internally calls the default send function, or the function registered by
 * httpd_sess_set_send_override().
 *
 * @param[in] hd        server instance
 * @param[in] sockfd    session socket file descriptor
 * @param[in] buf       buffer with bytes to send
 * @param[in] buf_len   data size
 * @param[in] flags     flags for the send() function
 * @return
 *  - Bytes : The number of bytes sent successfully
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket send()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket send()
 */
int httpd_socket_send(httpd_handle_t hd, int sockfd, const char *buf, size_t buf_len, int flags);

/**
 * A low level API to receive data from a given socket
 *
 * @note This API is not recommended to be used in any request handler.
 * Use this only for advanced use cases, wherein some asynchronous
 * communication is required.
 *
 * This internally calls the default recv function, or the function registered by
 * httpd_sess_set_recv_override().
 *
 * @param[in] hd        server instance
 * @param[in] sockfd    session socket file descriptor
 * @param[in] buf       buffer with bytes to send
 * @param[in] buf_len   data size
 * @param[in] flags     flags for the send() function
 * @return
 *  - Bytes : The number of bytes received successfully
 *  - 0     : Buffer length parameter is zero / connection closed by peer
 *  - HTTPD_SOCK_ERR_INVALID  : Invalid arguments
 *  - HTTPD_SOCK_ERR_TIMEOUT  : Timeout/interrupted while calling socket recv()
 *  - HTTPD_SOCK_ERR_FAIL     : Unrecoverable error while calling socket recv()
 */
int httpd_socket_recv(httpd_handle_t hd, int sockfd, char *buf, size_t buf_len, int flags);

/** End of Request / Response
 * @}
 */

/* ************** Group: Session ************** */
/** @name Session
 * Functions for controlling sessions and accessing context data
 * @{
 */

/**
 * @brief   Get session context from socket descriptor
 *
 * Typically if a session context is created, it is available to URI handlers
 * through the httpd_req_t structure. But, there are cases where the web
 * server's send/receive functions may require the context (for example, for
 * accessing keying information etc). Since the send/receive function only have
 * the socket descriptor at their disposal, this API provides them with a way to
 * retrieve the session context.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 *
 * @return
 *  - void* : Pointer to the context associated with this session
 *  - NULL  : Empty context / Invalid handle / Invalid socket fd
 */
void *httpd_sess_get_ctx(httpd_handle_t handle, int sockfd);

/**
 * @brief   Set session context by socket descriptor
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 * @param[in] ctx       Context object to assign to the session
 * @param[in] free_fn   Function that should be called to free the context
 */
void httpd_sess_set_ctx(httpd_handle_t handle, int sockfd, void *ctx, httpd_free_ctx_fn_t free_fn);

/**
 * @brief   Get session 'transport' context by socket descriptor
 * @see     httpd_sess_get_ctx()
 *
 * This context is used by the send/receive functions, for example to manage SSL context.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 * @return
 *  - void* : Pointer to the transport context associated with this session
 *  - NULL  : Empty context / Invalid handle / Invalid socket fd
 */
void *httpd_sess_get_transport_ctx(httpd_handle_t handle, int sockfd);

/**
 * @brief   Set session 'transport' context by socket descriptor
 * @see     httpd_sess_set_ctx()
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor for which the context should be extracted.
 * @param[in] ctx       Transport context object to assign to the session
 * @param[in] free_fn   Function that should be called to free the transport context
 */
void httpd_sess_set_transport_ctx(httpd_handle_t handle, int sockfd, void *ctx, httpd_free_ctx_fn_t free_fn);

/**
 * @brief   Get HTTPD global user context (it was set in the server config struct)
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @return global user context
 */
void *httpd_get_global_user_ctx(httpd_handle_t handle);

/**
 * @brief   Get HTTPD global transport context (it was set in the server config struct)
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @return global transport context
 */
void *httpd_get_global_transport_ctx(httpd_handle_t handle);

/**
 * @brief   Trigger an httpd session close externally
 *
 * @note    Calling this API is only required in special circumstances wherein
 *          some application requires to close an httpd client session asynchronously.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor of the session to be closed
 *
 * @return
 *  - ESP_OK    : On successfully initiating closure
 *  - ESP_FAIL  : Failure to queue work
 *  - ESP_ERR_NOT_FOUND   : Socket fd not found
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_sess_trigger_close(httpd_handle_t handle, int sockfd);

/**
 * @brief   Update LRU counter for a given socket
 *
 * LRU Counters are internally associated with each session to monitor
 * how recently a session exchanged traffic. When LRU purge is enabled,
 * if a client is requesting for connection but maximum number of
 * sockets/sessions is reached, then the session having the earliest
 * LRU counter is closed automatically.
 *
 * Updating the LRU counter manually prevents the socket from being purged
 * due to the Least Recently Used (LRU) logic, even though it might not
 * have received traffic for some time. This is useful when all open
 * sockets/session are frequently exchanging traffic but the user specifically
 * wants one of the sessions to be kept open, irrespective of when it last
 * exchanged a packet.
 *
 * @note    Calling this API is only necessary if the LRU Purge Enable option
 *          is enabled.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] sockfd    The socket descriptor of the session for which LRU counter
 *                      is to be updated
 *
 * @return
 *  - ESP_OK : Socket found and LRU counter updated
 *  - ESP_ERR_NOT_FOUND   : Socket not found
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_sess_update_lru_counter(httpd_handle_t handle, int sockfd);

/**
 * @brief   Returns list of current socket descriptors of active sessions
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in,out] fds   In: Size of provided client_fds array
 *                      Out: Number of valid client fds returned in client_fds,
 * @param[out] client_fds  Array of client fds
 *
 * @note Size of provided array has to be equal or greater then maximum number of opened
 *       sockets, configured upon initialization with max_open_sockets field in
 *       httpd_config_t structure.
 *
 * @return
 *  - ESP_OK              : Successfully retrieved session list
 *  - ESP_ERR_INVALID_ARG : Wrong arguments or list is longer than provided array
 */
esp_err_t httpd_get_client_list(httpd_handle_t handle, size_t *fds, int *client_fds);

/** End of Session
 * @}
 */

/* ************** Group: Work Queue ************** */
/** @name Work Queue
 * APIs related to the HTTPD Work Queue
 * @{
 */

/**
 * @brief   Prototype of the HTTPD work function
 *          Please refer to httpd_queue_work() for more details.
 * @param[in] arg   The arguments for this work function
 */
typedef void (*httpd_work_fn_t)(void *arg);

/**
 * @brief   Queue execution of a function in HTTPD's context
 *
 * This API queues a work function for asynchronous execution
 *
 * @note    Some protocols require that the web server generate some asynchronous data
 *          and send it to the persistently opened connection. This facility is for use
 *          by such protocols.
 *
 * @param[in] handle    Handle to server returned by httpd_start
 * @param[in] work      Pointer to the function to be executed in the HTTPD's context
 * @param[in] arg       Pointer to the arguments that should be passed to this function
 *
 * @return
 *  - ESP_OK   : On successfully queueing the work
 *  - ESP_FAIL : Failure in ctrl socket
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_queue_work(httpd_handle_t handle, httpd_work_fn_t work, void *arg);

/** End of Group Work Queue
 * @}
 */

/* ************** Group: WebSocket ************** */
/** @name WebSocket
 * Functions and structs for WebSocket server
 * @{
 */
#ifdef CONFIG_HTTPD_WS_SUPPORT
/**
 * @brief Enum for WebSocket packet types (Opcode in the header)
 * @note Please refer to RFC6455 Section 5.4 for more details
 */
typedef enum {
    HTTPD_WS_TYPE_CONTINUE   = 0x0,
    HTTPD_WS_TYPE_TEXT       = 0x1,
    HTTPD_WS_TYPE_BINARY     = 0x2,
    HTTPD_WS_TYPE_CLOSE      = 0x8,
    HTTPD_WS_TYPE_PING       = 0x9,
    HTTPD_WS_TYPE_PONG       = 0xA
} httpd_ws_type_t;

/**
 * @brief Enum for client info description
 */
typedef enum {
    HTTPD_WS_CLIENT_INVALID        = 0x0,
    HTTPD_WS_CLIENT_HTTP           = 0x1,
    HTTPD_WS_CLIENT_WEBSOCKET      = 0x2,
} httpd_ws_client_info_t;

/**
 * @brief WebSocket frame format
 */
typedef struct httpd_ws_frame {
    bool final;                 /*!< Final frame:
                                     For received frames this field indicates whether the `FIN` flag was set.
                                     For frames to be transmitted, this field is only used if the `fragmented`
                                         option is set as well. If `fragmented` is false, the `FIN` flag is set
                                         by default, marking the ws_frame as a complete/unfragmented message
                                         (esp_http_server doesn't automatically fragment messages) */
    bool fragmented;            /*!< Indication that the frame allocated for transmission is a message fragment,
                                     so the `FIN` flag is set manually according to the `final` option.
                                     This flag is never set for received messages */
    httpd_ws_type_t type;       /*!< WebSocket frame type */
    uint8_t *payload;           /*!< Pre-allocated data buffer */
    size_t len;                 /*!< Length of the WebSocket data */
} httpd_ws_frame_t;

/**
 * @brief Transfer complete callback
 */
typedef void (*transfer_complete_cb)(esp_err_t err, int socket, void *arg);

/**
 * @brief Receive and parse a WebSocket frame
 *
 * @note    Calling httpd_ws_recv_frame() with max_len as 0 will give actual frame size in pkt->len.
 *          The user can dynamically allocate space for pkt->payload as per this length and call httpd_ws_recv_frame() again to get the actual data.
 *          Please refer to the corresponding example for usage.
 *
 * @param[in]   req         Current request
 * @param[out]  pkt         WebSocket packet
 * @param[in]   max_len     Maximum length for receive
 * @return
 *  - ESP_OK                    : On successful
 *  - ESP_FAIL                  : Socket errors occurs
 *  - ESP_ERR_INVALID_STATE     : Handshake was already done beforehand
 *  - ESP_ERR_INVALID_ARG       : Argument is invalid (null or non-WebSocket)
 */
esp_err_t httpd_ws_recv_frame(httpd_req_t *req, httpd_ws_frame_t *pkt, size_t max_len);

/**
 * @brief Construct and send a WebSocket frame
 * @param[in]   req     Current request
 * @param[in]   pkt     WebSocket frame
 * @return
 *  - ESP_OK                    : On successful
 *  - ESP_FAIL                  : When socket errors occurs
 *  - ESP_ERR_INVALID_STATE     : Handshake was already done beforehand
 *  - ESP_ERR_INVALID_ARG       : Argument is invalid (null or non-WebSocket)
 */
esp_err_t httpd_ws_send_frame(httpd_req_t *req, httpd_ws_frame_t *pkt);

/**
 * @brief Low level send of a WebSocket frame out of the scope of current request
 * using internally configured httpd send function
 *
 * This API should rarely be called directly, with an exception of asynchronous send using httpd_queue_work.
 *
 * @param[in] hd      Server instance data
 * @param[in] fd      Socket descriptor for sending data
 * @param[in] frame     WebSocket frame
 * @return
 *  - ESP_OK                    : On successful
 *  - ESP_FAIL                  : When socket errors occurs
 *  - ESP_ERR_INVALID_STATE     : Handshake was already done beforehand
 *  - ESP_ERR_INVALID_ARG       : Argument is invalid (null or non-WebSocket)
 */
esp_err_t httpd_ws_send_frame_async(httpd_handle_t hd, int fd, httpd_ws_frame_t *frame);

/**
 * @brief Checks the supplied socket descriptor if it belongs to any active client
 * of this server instance and if the websoket protocol is active
 *
 * @param[in] hd      Server instance data
 * @param[in] fd      Socket descriptor
 * @return
 *  - HTTPD_WS_CLIENT_INVALID   : This fd is not a client of this httpd
 *  - HTTPD_WS_CLIENT_HTTP      : This fd is an active client, protocol is not WS
 *  - HTTPD_WS_CLIENT_WEBSOCKET : This fd is an active client, protocol is WS
 */
httpd_ws_client_info_t httpd_ws_get_fd_info(httpd_handle_t hd, int fd);

/**
 * @brief Sends data to to specified websocket synchronously
 *
 * @param[in] handle  Server instance data
 * @param[in] socket  Socket descriptor
 * @param[in] frame   Websocket frame
 * @return
 *  - ESP_OK                    : On successful
 *  - ESP_FAIL                  : When socket errors occurs
 *  - ESP_ERR_NO_MEM            : Unable to allocate memory
 */
esp_err_t httpd_ws_send_data(httpd_handle_t handle, int socket, httpd_ws_frame_t *frame);

/**
 * @brief Sends data to to specified websocket asynchronously
 *
 * @param[in] handle    Server instance data
 * @param[in] socket    Socket descriptor
 * @param[in] frame     Websocket frame
 * @param[in] callback  Callback invoked after sending data
 * @param[in] arg       User data passed to provided callback
 * @return
 *  - ESP_OK                    : On successful
 *  - ESP_FAIL                  : When socket errors occurs
 *  - ESP_ERR_NO_MEM            : Unable to allocate memory
 */
esp_err_t httpd_ws_send_data_async(httpd_handle_t handle, int socket, httpd_ws_frame_t *frame,
                                   transfer_complete_cb callback, void *arg);

#endif /* CONFIG_HTTPD_WS_SUPPORT */
/** End of WebSocket related stuff
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ! _ESP_HTTP_SERVER_H_ */
