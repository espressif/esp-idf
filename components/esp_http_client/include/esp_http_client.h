// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_HTTP_CLIENT_H
#define _ESP_HTTP_CLIENT_H

#include "freertos/FreeRTOS.h"
#include "http_parser.h"
#include "sdkconfig.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_HTTP_BUF_SIZE (512)

typedef struct esp_http_client *esp_http_client_handle_t;
typedef struct esp_http_client_event *esp_http_client_event_handle_t;

/**
 * @brief   HTTP Client events id
 */
typedef enum {
    HTTP_EVENT_ERROR = 0,       /*!< This event occurs when there are any errors during execution */
    HTTP_EVENT_ON_CONNECTED,    /*!< Once the HTTP has been connected to the server, no data exchange has been performed */
    HTTP_EVENT_HEADER_SENT,     /*!< After sending all the headers to the server */
    HTTP_EVENT_ON_HEADER,       /*!< Occurs when receiving each header sent from the server */
    HTTP_EVENT_ON_DATA,         /*!< Occurs when receiving data from the server, possibly multiple portions of the packet */
    HTTP_EVENT_ON_FINISH,       /*!< Occurs when finish a HTTP session */
    HTTP_EVENT_DISCONNECTED,    /*!< The connection has been disconnected */
} esp_http_client_event_id_t;

/**
 * @brief      HTTP Client events data
 */
typedef struct esp_http_client_event {
    esp_http_client_event_id_t event_id;    /*!< event_id, to know the cause of the event */
    esp_http_client_handle_t client;        /*!< esp_http_client_handle_t context */
    void *data;                             /*!< data of the event */
    int data_len;                           /*!< data length of data */
    void *user_data;                        /*!< user_data context, from esp_http_client_config_t user_data */
    char *header_key;                       /*!< For HTTP_EVENT_ON_HEADER event_id, it's store current http header key */
    char *header_value;                     /*!< For HTTP_EVENT_ON_HEADER event_id, it's store current http header value */
} esp_http_client_event_t;


/**
 * @brief      HTTP Client transport
 */
typedef enum {
    HTTP_TRANSPORT_UNKNOWN = 0x0,   /*!< Unknown */
    HTTP_TRANSPORT_OVER_TCP,        /*!< Transport over tcp */
    HTTP_TRANSPORT_OVER_SSL,        /*!< Transport over ssl */
} esp_http_client_transport_t;

typedef esp_err_t (*http_event_handle_cb)(esp_http_client_event_t *evt);

/**
 * @brief HTTP method
 */
typedef enum {
    HTTP_METHOD_GET = 0,    /*!< HTTP GET Method */
    HTTP_METHOD_POST,       /*!< HTTP POST Method */
    HTTP_METHOD_PUT,        /*!< HTTP PUT Method */
    HTTP_METHOD_PATCH,      /*!< HTTP PATCH Method */
    HTTP_METHOD_DELETE,     /*!< HTTP DELETE Method */
    HTTP_METHOD_HEAD,       /*!< HTTP HEAD Method */
    HTTP_METHOD_NOTIFY,     /*!< HTTP NOTIFY Method */
    HTTP_METHOD_SUBSCRIBE,  /*!< HTTP SUBSCRIBE Method */
    HTTP_METHOD_UNSUBSCRIBE,/*!< HTTP UNSUBSCRIBE Method */
    HTTP_METHOD_OPTIONS,    /*!< HTTP OPTIONS Method */
    HTTP_METHOD_MAX,
} esp_http_client_method_t;

/**
 * @brief HTTP Authentication type
 */
typedef enum {
    HTTP_AUTH_TYPE_NONE = 0,    /*!< No authention */
    HTTP_AUTH_TYPE_BASIC,       /*!< HTTP Basic authentication */
    HTTP_AUTH_TYPE_DIGEST,      /*!< HTTP Disgest authentication */
} esp_http_client_auth_type_t;

/**
 * @brief HTTP configuration
 */
typedef struct {
    const char                  *url;                /*!< HTTP URL, the information on the URL is most important, it overrides the other fields below, if any */
    const char                  *host;               /*!< Domain or IP as string */
    int                         port;                /*!< Port to connect, default depend on esp_http_client_transport_t (80 or 443) */
    const char                  *username;           /*!< Using for Http authentication */
    const char                  *password;           /*!< Using for Http authentication */
    esp_http_client_auth_type_t auth_type;           /*!< Http authentication type, see `esp_http_client_auth_type_t` */
    const char                  *path;               /*!< HTTP Path, if not set, default is `/` */
    const char                  *query;              /*!< HTTP query */
    const char                  *cert_pem;           /*!< SSL Certification, PEM format as string, if the client requires to verify server */
    esp_http_client_method_t    method;                   /*!< HTTP Method */
    int                         timeout_ms;               /*!< Network timeout in milliseconds */
    bool                        disable_auto_redirect;    /*!< Disable HTTP automatic redirects */
    int                         max_redirection_count;    /*!< Max redirection number, using default value if zero*/
    http_event_handle_cb        event_handler;             /*!< HTTP Event Handle */
    esp_http_client_transport_t transport_type;           /*!< HTTP transport type, see `esp_http_client_transport_t` */
    int                         buffer_size;              /*!< HTTP buffer size (both send and receive) */
    void                        *user_data;               /*!< HTTP user_data context */
    bool                        is_async;                 /*!< Set asynchronous mode, only supported with HTTPS for now */
} esp_http_client_config_t;


#define ESP_ERR_HTTP_BASE               (0x7000)                    /*!< Starting number of HTTP error codes */
#define ESP_ERR_HTTP_MAX_REDIRECT       (ESP_ERR_HTTP_BASE + 1)     /*!< The error exceeds the number of HTTP redirects */
#define ESP_ERR_HTTP_CONNECT            (ESP_ERR_HTTP_BASE + 2)     /*!< Error open the HTTP connection */
#define ESP_ERR_HTTP_WRITE_DATA         (ESP_ERR_HTTP_BASE + 3)     /*!< Error write HTTP data */
#define ESP_ERR_HTTP_FETCH_HEADER       (ESP_ERR_HTTP_BASE + 4)     /*!< Error read HTTP header from server */
#define ESP_ERR_HTTP_INVALID_TRANSPORT  (ESP_ERR_HTTP_BASE + 5)     /*!< There are no transport support for the input scheme */
#define ESP_ERR_HTTP_CONNECTING         (ESP_ERR_HTTP_BASE + 6)     /*!< HTTP connection hasn't been established yet */
#define ESP_ERR_HTTP_EAGAIN             (ESP_ERR_HTTP_BASE + 7)     /*!< Mapping of errno EAGAIN to esp_err_t */

/**
 * @brief      Start a HTTP session
 *             This function must be the first function to call,
 *             and it returns a esp_http_client_handle_t that you must use as input to other functions in the interface.
 *             This call MUST have a corresponding call to esp_http_client_cleanup when the operation is complete.
 *
 * @param[in]  config   The configurations, see `http_client_config_t`
 *
 * @return
 *     - `esp_http_client_handle_t`
 *     - NULL if any errors
 */
esp_http_client_handle_t esp_http_client_init(const esp_http_client_config_t *config);

/**
 * @brief      Invoke this function after `esp_http_client_init` and all the options calls are made, and will perform the
 *             transfer as described in the options. It must be called with the same esp_http_client_handle_t as input as the esp_http_client_init call returned.
 *             esp_http_client_perform performs the entire request in either blocking or non-blocking manner. By default, the API performs request in a blocking manner and returns when done,
 *             or if it failed, and in non-blocking manner, it returns if EAGAIN/EWOULDBLOCK or EINPROGRESS is encountered, or if it failed. And in case of non-blocking request,
 *             the user may call this API multiple times unless request & response is complete or there is a failure. To enable non-blocking esp_http_client_perform(), `is_async` member of esp_http_client_config_t
 *             must be set while making a call to esp_http_client_init() API.
 *             You can do any amount of calls to esp_http_client_perform while using the same esp_http_client_handle_t. The underlying connection may be kept open if the server allows it.
 *             If you intend to transfer more than one file, you are even encouraged to do so.
 *             esp_http_client will then attempt to re-use the same connection for the following transfers, thus making the operations faster, less CPU intense and using less network resources.
 *             Just note that you will have to use `esp_http_client_set_**` between the invokes to set options for the following esp_http_client_perform.
 *
 * @note       You must never call this function simultaneously from two places using the same client handle.
 *             Let the function return first before invoking it another time.
 *             If you want parallel transfers, you must use several esp_http_client_handle_t.
 *             This function include `esp_http_client_open` -> `esp_http_client_write` -> `esp_http_client_fetch_headers` -> `esp_http_client_read` (and option) `esp_http_client_close`.
 *
 * @param      client  The esp_http_client handle
 *
 * @return
 *  - ESP_OK on successful
 *  - ESP_FAIL on error
 */
esp_err_t esp_http_client_perform(esp_http_client_handle_t client);

/**
 * @brief      Set URL for client, when performing this behavior, the options in the URL will replace the old ones
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  url     The url
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_set_url(esp_http_client_handle_t client, const char *url);

/**
 * @brief      Set post data, this function must be called before `esp_http_client_perform`.
 *             Note: The data parameter passed to this function is a pointer and this function will not copy the data
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  data    post data pointer
 * @param[in]  len     post length
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_set_post_field(esp_http_client_handle_t client, const char *data, int len);

/**
 * @brief      Get current post field information
 *
 * @param[in]  client  The client
 * @param[out] data    Point to post data pointer
 *
 * @return     Size of post data
 */
int esp_http_client_get_post_field(esp_http_client_handle_t client, char **data);

/**
 * @brief      Set http request header, this function must be called after esp_http_client_init and before any
 *             perform function
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  key     The header key
 * @param[in]  value   The header value
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_set_header(esp_http_client_handle_t client, const char *key, const char *value);

/**
 * @brief      Get http request header.
 *             The value parameter will be set to NULL if there is no header which is same as
 *             the key specified, otherwise the address of header value will be assigned to value parameter.
 *             This function must be called after `esp_http_client_init`.
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  key     The header key
 * @param[out] value   The header value
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_http_client_get_header(esp_http_client_handle_t client, const char *key, char **value);

/**
 * @brief      Set http request method
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  method  The method
 *
 * @return     ESP_OK
 */
esp_err_t esp_http_client_set_method(esp_http_client_handle_t client, esp_http_client_method_t method);

/**
 * @brief      Delete http request header
 *
 * @param[in]  client  The esp_http_client handle
 * @param[in]  key     The key
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_delete_header(esp_http_client_handle_t client, const char *key);

/**
 * @brief      This function will be open the connection, write all header strings and return
 *
 * @param[in]  client     The esp_http_client handle
 * @param[in]  write_len  HTTP Content length need to write to the server
 *
 * @return
 *  - ESP_OK
 *  - ESP_FAIL
 */
esp_err_t esp_http_client_open(esp_http_client_handle_t client, int write_len);

/**
 * @brief     This function will write data to the HTTP connection previously opened by esp_http_client_open()
 *
 * @param[in]  client  The esp_http_client handle
 * @param      buffer  The buffer
 * @param[in]  len     This value must not be larger than the write_len parameter provided to esp_http_client_open()
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data written
 */
int esp_http_client_write(esp_http_client_handle_t client, const char *buffer, int len);

/**
 * @brief      This function need to call after esp_http_client_open, it will read from http stream, process all receive headers
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - (0) if stream doesn't contain content-length header, or chunked encoding (checked by `esp_http_client_is_chunked` response)
 *     - (-1: ESP_FAIL) if any errors
 *     - Download data length defined by content-length header
 */
int esp_http_client_fetch_headers(esp_http_client_handle_t client);


/**
 * @brief      Check response data is chunked
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return     true or false
 */
bool esp_http_client_is_chunked_response(esp_http_client_handle_t client);

/**
 * @brief      Read data from http stream
 *
 * @param[in]  client  The esp_http_client handle
 * @param      buffer  The buffer
 * @param[in]  len     The length
 *
 * @return
 *     - (-1) if any errors
 *     - Length of data was read
 */
int esp_http_client_read(esp_http_client_handle_t client, char *buffer, int len);


/**
 * @brief      Get http response status code, the valid value if this function invoke after `esp_http_client_perform`
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return     Status code
 */
int esp_http_client_get_status_code(esp_http_client_handle_t client);

/**
 * @brief      Get http response content length (from header Content-Length)
 *             the valid value if this function invoke after `esp_http_client_perform`
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - (-1) Chunked transfer
 *     - Content-Length value as bytes
 */
int esp_http_client_get_content_length(esp_http_client_handle_t client);

/**
 * @brief      Close http connection, still kept all http request resources
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_http_client_close(esp_http_client_handle_t client);

/**
 * @brief      This function must be the last function to call for an session.
 *             It is the opposite of the esp_http_client_init function and must be called with the same handle as input that a esp_http_client_init call returned.
 *             This might close all connections this handle has used and possibly has kept open until now.
 *             Don't call this function if you intend to transfer more files, re-using handles is a key to good performance with esp_http_client.
 *
 * @param[in]  client  The esp_http_client handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_http_client_cleanup(esp_http_client_handle_t client);

/**
 * @brief      Get transport type
 *
 * @param[in]  client   The esp_http_client handle
 *
 * @return
 *     - HTTP_TRANSPORT_UNKNOWN
 *     - HTTP_TRANSPORT_OVER_TCP
 *     - HTTP_TRANSPORT_OVER_SSL
 */
esp_http_client_transport_t esp_http_client_get_transport_type(esp_http_client_handle_t client);


#ifdef __cplusplus
}
#endif


#endif
