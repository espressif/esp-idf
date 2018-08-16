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

#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <http_parser.h>
#include <sdkconfig.h>
#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HTTPD_DEFAULT_CONFIG() {                        \
        .task_priority      = tskIDLE_PRIORITY+5,       \
        .stack_size         = 4096,                     \
        .server_port        = 80,                       \
        .ctrl_port          = 32768,                    \
        .max_open_sockets   = 7,                        \
        .max_uri_handlers   = 8,                        \
        .max_resp_headers   = 8,                        \
        .backlog_conn       = 5,                        \
        .lru_purge_enable   = false,                    \
        .recv_wait_timeout  = 5,                        \
        .send_wait_timeout  = 5,                        \
};

#define ESP_ERR_HTTPD_BASE              (0x8000)                    /*!< Starting number of HTTPD error codes */
#define ESP_ERR_HTTPD_HANDLERS_FULL     (ESP_ERR_HTTPD_BASE +  1)   /*!< All slots for registering URI handlers have been consumed */
#define ESP_ERR_HTTPD_HANDLER_EXISTS    (ESP_ERR_HTTPD_BASE +  2)   /*!< URI handler with same method and target URI already registered */
#define ESP_ERR_HTTPD_INVALID_REQ       (ESP_ERR_HTTPD_BASE +  3)   /*!< Invalid request pointer */
#define ESP_ERR_HTTPD_RESULT_TRUNC      (ESP_ERR_HTTPD_BASE +  4)   /*!< Result string truncated */
#define ESP_ERR_HTTPD_RESP_HDR          (ESP_ERR_HTTPD_BASE +  5)   /*!< Response header field larger than supported */
#define ESP_ERR_HTTPD_RESP_SEND         (ESP_ERR_HTTPD_BASE +  6)   /*!< Error occured while sending response packet */
#define ESP_ERR_HTTPD_ALLOC_MEM         (ESP_ERR_HTTPD_BASE +  7)   /*!< Failed to dynamically allocate memory for resource */
#define ESP_ERR_HTTPD_TASK              (ESP_ERR_HTTPD_BASE +  8)   /*!< Failed to launch server task/thread */

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

    /**
     * TCP Port number for receiving and transmitting HTTP traffic
     */
    uint16_t    server_port;

    /**
     * UDP Port number for asynchronously exchanging control signals
     * between various components of the server
     */
    uint16_t    ctrl_port;

    uint16_t    max_open_sockets;   /*!< Max number of sockets/clients connected at any time*/
    uint16_t    max_uri_handlers;   /*!< Maximum allowed uri handlers */
    uint16_t    max_resp_headers;   /*!< Maximum allowed additional headers in HTTP response */
    uint16_t    backlog_conn;       /*!< Number of backlog connections */
    bool        lru_purge_enable;   /*!< Purge "Least Recently Used" connection */
    uint16_t    recv_wait_timeout;  /*!< Timeout for recv function (in seconds)*/
    uint16_t    send_wait_timeout;  /*!< Timeout for send function (in seconds)*/
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
 * @param[in]  config : Configuration for new instance of the server
 * @param[out] handle : Handle to newly created instance of the server. NULL on error
 * @return
 *  - ESP_OK    : Instance created successfully
 *  - ESP_ERR_INVALID_ARG      : Null argument(s)
 *  - ESP_ERR_HTTPD_ALLOC_MEM  : Failed to allocate memory for instance
 *  - ESP_ERR_HTTPD_TASK       : Failed to launch server task
 */
esp_err_t  httpd_start(httpd_handle_t *handle, const httpd_config_t *config);

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

/**
 * @brief Function type for freeing context data (if any)
 */
typedef void (*httpd_free_sess_ctx_fn_t)(void *sess_ctx);

/* Max supported HTTP request header length */
#define HTTPD_MAX_REQ_HDR_LEN CONFIG_HTTPD_MAX_REQ_HDR_LEN

/* Max supported HTTP request URI length */
#define HTTPD_MAX_URI_LEN CONFIG_HTTPD_MAX_URI_LEN

/**
 * @brief HTTP Request Data Structure
 */
typedef struct httpd_req {
    httpd_handle_t  handle;                     /*!< Handle to server instance */
    int             method;                     /*!< The type of HTTP request, -1 if unsupported method */
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
    httpd_free_sess_ctx_fn_t free_ctx;
} httpd_req_t;

/**
 * @brief Structure for URI handler
 */
typedef struct httpd_uri {
    const char       *uri;    /*!< The URI to handle */
    httpd_method_t    method; /*!< Method supported by the URI */

    /**
     * Handler to call for supported request method. This must
     * return ESP_OK, or else the underlying socket will be closed.
     */
    esp_err_t (*handler)(httpd_req_t *r);

    /**
     * Pointer to user context data which will be available to handler
     */
    void *user_ctx;
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

/* ************** Group: TX/RX ************** */
/** @name TX / RX
 * Prototype for HTTPDs low-level send/recv functions
 * @{
 */

/**
 * @brief  Prototype for HTTPDs low-level send function
 * @return
 *  - Bytes : The number of bytes sent successfully
 *  - -VE   : In case of error
 */
typedef int (*httpd_send_func_t)(int sockfd, const char *buf, size_t buf_len, int flags);

/**
 * @brief  Prototype for HTTPDs low-level recv function
 * @return
 *  - Bytes : The number of bytes received successfully
 *  - -VE   : In case of error
 */
typedef int (*httpd_recv_func_t)(int sockfd, char *buf, size_t buf_len, int flags);

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
 * @brief   Override web server's receive function
 *
 * This function overrides the web server's receive function. This same function is
 * used to read and parse HTTP headers as well as body.
 *
 * @note    This API is supposed to be called only from the context of
 *          a URI handler where httpd_req_t* request pointer is valid.
 *
 * @param[in] r         The request being responded to
 * @param[in] recv_func The receive function to be set for this request
 *
 * @return
 *  - ESP_OK : On successfully registering override
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_set_recv_override(httpd_req_t *r, httpd_recv_func_t recv_func);

/**
 * @brief   Override web server's send function
 *
 * This function overrides the web server's send function. This same function is
 * used to send out any response to any HTTP request.
 *
 * @note    This API is supposed to be called only from the context of
 *          a URI handler where httpd_req_t* request pointer is valid.
 *
 * @param[in] r         The request being responded to
 * @param[in] send_func The send function to be set for this request
 *
 * @return
 *  - ESP_OK : On successfully registering override
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_set_send_override(httpd_req_t *r, httpd_send_func_t send_func);

/**
 * @brief   Get the Socket Descriptor from the HTTP request
 *
 * This API will return the socket descriptor of the session for
 * which URI handler was executed on reception of HTTP request.
 * This is useful when user wants to call functions that require
 * session socket fd, from within a URI handler, ie. :
 *      httpd_sess_get_ctx(),
 *      httpd_trigger_sess_close(),
 *      httpd_sess_update_timestamp().
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
 *  - Bytes    : Number of bytes read into the buffer successfully
 *  - Zero     : When no more data is left for read
 *  - -1       : On raw recv error / Null arguments / Request pointer is invalid
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
 *  - Zero      : Query not found / Null arguments / Invalid request
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
 *  - Use httpd_req_get_url_query_len() to know the right buffer length
 *
 * @param[in]  r         The request being responded to
 * @param[out] buf       Pointer to the buffer into which the query string will be copied (if found)
 * @param[in]  buf_len   Length of output buffer
 *
 * @return
 *  - ESP_OK : Query is found in the request URL and copied to buffer
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
 * @param[in] buf_len   Length of the buffer
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet
 *  - ESP_ERR_INVALID_ARG : Null request pointer
 *  - ESP_ERR_HTTPD_RESP_HDR    : Essential headers are too large for internal buffer
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request
 */
esp_err_t httpd_resp_send(httpd_req_t *r, const char *buf, size_t buf_len);

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
 * @param[in] buf_len   Length of the data from the buffer that should be sent out
 *
 * @return
 *  - ESP_OK : On successfully sending the response packet chunk
 *  - ESP_ERR_INVALID_ARG : Null request pointer
 *  - ESP_ERR_HTTPD_RESP_HDR    : Essential headers are too large for internal buffer
 *  - ESP_ERR_HTTPD_RESP_SEND   : Error in raw send
 *  - ESP_ERR_HTTPD_INVALID_REQ : Invalid request pointer
 */
esp_err_t httpd_resp_send_chunk(httpd_req_t *r, const char *buf, size_t buf_len);

/* Some commonly used status codes */
#define HTTPD_200      "200 OK"                     /*!< HTTP Response 200 */
#define HTTPD_204      "204 No Content"             /*!< HTTP Response 204 */
#define HTTPD_207      "207 Multi-Status"           /*!< HTTP Response 207 */
#define HTTPD_400      "400 Bad Request"            /*!< HTTP Response 400 */
#define HTTPD_404      "404 Not Found"              /*!< HTTP Response 404 */
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
esp_err_t httpd_resp_send_404(httpd_req_t *r);

/**
 * @brief   Raw HTTP send
 *
 * Call this API if you wish to construct your custom response packet.
 * When using this, all essential header, eg. HTTP version, Status Code,
 * Content Type and Length, Encoding, etc. will have to be constructed
 * manually, and HTTP delimeters (CRLF) will need to be placed correctly
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
 *  - -1    : Error in raw send / Invalid request / Null arguments
 */
int httpd_send(httpd_req_t *r, const char *buf, size_t buf_len);

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
esp_err_t httpd_trigger_sess_close(httpd_handle_t handle, int sockfd);

/**
 * @brief   Update timestamp for a given socket
 *
 * Timestamps are internally associated with each session to monitor
 * how recently a session exchanged traffic. When LRU purge is enabled,
 * if a client is requesting for connection but maximum number of
 * sockets/sessions is reached, then the session having the earliest
 * timestamp is closed automatically.
 *
 * Updating the timestamp manually prevents the socket from being purged
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
 * @param[in] sockfd    The socket descriptor of the session for which timestamp
 *                      is to be updated
 *
 * @return
 *  - ESP_OK : Socket found and timestamp updated
 *  - ESP_ERR_NOT_FOUND   : Socket not found
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t httpd_sess_update_timestamp(httpd_handle_t handle, int sockfd);

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

#ifdef __cplusplus
}
#endif

#endif /* ! _HTTP_SERVER_H_ */
