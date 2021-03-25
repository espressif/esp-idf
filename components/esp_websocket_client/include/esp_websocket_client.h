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

#ifndef _ESP_WEBSOCKET_CLIENT_H_
#define _ESP_WEBSOCKET_CLIENT_H_


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_event.h"
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_websocket_client *esp_websocket_client_handle_t;

ESP_EVENT_DECLARE_BASE(WEBSOCKET_EVENTS);         // declaration of the task events family

/**
 * @brief Websocket Client events id
 */
typedef enum {
    WEBSOCKET_EVENT_ANY = -1,
    WEBSOCKET_EVENT_ERROR = 0,      /*!< This event occurs when there are any errors during execution */
    WEBSOCKET_EVENT_CONNECTED,      /*!< Once the Websocket has been connected to the server, no data exchange has been performed */
    WEBSOCKET_EVENT_DISCONNECTED,   /*!< The connection has been disconnected */
    WEBSOCKET_EVENT_DATA,           /*!< When receiving data from the server, possibly multiple portions of the packet */
    WEBSOCKET_EVENT_CLOSED,         /*!< The connection has been closed cleanly */
    WEBSOCKET_EVENT_MAX
} esp_websocket_event_id_t;

/**
 * @brief Websocket event data
 */
typedef struct {
    const char *data_ptr;                   /*!< Data pointer */
    int data_len;                           /*!< Data length */
    uint8_t op_code;                        /*!< Received opcode */
    esp_websocket_client_handle_t client;   /*!< esp_websocket_client_handle_t context */
    void *user_context;                     /*!< user_data context, from esp_websocket_client_config_t user_data */
    int payload_len;                        /*!< Total payload length, payloads exceeding buffer will be posted through multiple events */
    int payload_offset;                     /*!< Actual offset for the data associated with this event */
} esp_websocket_event_data_t;

/**
 * @brief Websocket Client transport
 */
typedef enum {
    WEBSOCKET_TRANSPORT_UNKNOWN = 0x0,  /*!< Transport unknown */
    WEBSOCKET_TRANSPORT_OVER_TCP,       /*!< Transport over tcp */
    WEBSOCKET_TRANSPORT_OVER_SSL,       /*!< Transport over ssl */
} esp_websocket_transport_t;

/**
 * @brief Websocket client setup configuration
 */
typedef struct {
    const char                  *uri;                       /*!< Websocket URI, the information on the URI can be overrides the other fields below, if any */
    const char                  *host;                      /*!< Domain or IP as string */
    int                         port;                       /*!< Port to connect, default depend on esp_websocket_transport_t (80 or 443) */
    const char                  *username;                  /*!< Using for Http authentication - Not supported for now */
    const char                  *password;                  /*!< Using for Http authentication - Not supported for now */
    const char                  *path;                      /*!< HTTP Path, if not set, default is `/` */
    bool                        disable_auto_reconnect;     /*!< Disable the automatic reconnect function when disconnected */
    void                        *user_context;              /*!< HTTP user data context */
    int                         task_prio;                  /*!< Websocket task priority */
    int                         task_stack;                 /*!< Websocket task stack */
    int                         buffer_size;                /*!< Websocket buffer size */
    const char                  *cert_pem;                  /*!< Pointer to certificate data in PEM or DER format for server verify (with SSL), default is NULL, not required to verify the server. PEM-format must have a terminating NULL-character. DER-format requires the length to be passed in cert_len. */
    size_t                      cert_len;                   /*!< Length of the buffer pointed to by cert_pem. May be 0 for null-terminated pem */
    const char                  *client_cert;               /*!< Pointer to certificate data in PEM or DER format for SSL mutual authentication, default is NULL, not required if mutual authentication is not needed. If it is not NULL, also `client_key` has to be provided. PEM-format must have a terminating NULL-character. DER-format requires the length to be passed in client_cert_len. */
    size_t                      client_cert_len;            /*!< Length of the buffer pointed to by client_cert. May be 0 for null-terminated pem */
    const char                  *client_key;                /*!< Pointer to private key data in PEM or DER format for SSL mutual authentication, default is NULL, not required if mutual authentication is not needed. If it is not NULL, also `client_cert` has to be provided. PEM-format must have a terminating NULL-character. DER-format requires the length to be passed in client_key_len */
    size_t                      client_key_len;             /*!< Length of the buffer pointed to by client_key_pem. May be 0 for null-terminated pem */
    esp_websocket_transport_t   transport;                  /*!< Websocket transport type, see `esp_websocket_transport_t */
    char                        *subprotocol;               /*!< Websocket subprotocol */
    char                        *user_agent;                /*!< Websocket user-agent */
    char                        *headers;                   /*!< Websocket additional headers */
    int                         pingpong_timeout_sec;       /*!< Period before connection is aborted due to no PONGs received */
    bool                        disable_pingpong_discon;    /*!< Disable auto-disconnect due to no PONG received within pingpong_timeout_sec */
    bool                        use_global_ca_store;        /*!< Use a global ca_store for all the connections in which this bool is set. */
    bool                        skip_cert_common_name_check;/*!< Skip any validation of server certificate CN field */
    bool                        keep_alive_enable;          /*!< Enable keep-alive timeout */
    int                         keep_alive_idle;            /*!< Keep-alive idle time. Default is 5 (second) */
    int                         keep_alive_interval;        /*!< Keep-alive interval time. Default is 5 (second) */
    int                         keep_alive_count;           /*!< Keep-alive packet retry send count. Default is 3 counts */
    size_t                      ping_interval_sec;          /*!< Websocket ping interval, defaults to 10 seconds if not set */
    struct ifreq                *if_name;                   /*!< The name of interface for data to go through. Use the default interface without setting */
} esp_websocket_client_config_t;

/**
 * @brief      Start a Websocket session
 *             This function must be the first function to call,
 *             and it returns a esp_websocket_client_handle_t that you must use as input to other functions in the interface.
 *             This call MUST have a corresponding call to esp_websocket_client_destroy when the operation is complete.
 *
 * @param[in]  config  The configuration
 *
 * @return
 *     - `esp_websocket_client_handle_t`
 *     - NULL if any errors
 */
esp_websocket_client_handle_t esp_websocket_client_init(const esp_websocket_client_config_t *config);

/**
 * @brief      Set URL for client, when performing this behavior, the options in the URL will replace the old ones
 *             Must stop the WebSocket client before set URI if the client has been connected
 *
 * @param[in]  client  The client
 * @param[in]  uri     The uri
 *
 * @return     esp_err_t
 */
esp_err_t esp_websocket_client_set_uri(esp_websocket_client_handle_t client, const char *uri);

/**
 * @brief      Open the WebSocket connection
 *
 * @param[in]  client  The client
 *
 * @return     esp_err_t
 */
esp_err_t esp_websocket_client_start(esp_websocket_client_handle_t client);

/**
 * @brief      Stops the WebSocket connection without websocket closing handshake
 *
 * This API stops ws client and closes TCP connection directly without sending
 * close frames. It is a good practice to close the connection in a clean way
 * using esp_websocket_client_close().
 *
 *  Notes:
 *  - Cannot be called from the websocket event handler
 *
 * @param[in]  client  The client
 *
 * @return     esp_err_t
 */
esp_err_t esp_websocket_client_stop(esp_websocket_client_handle_t client);

/**
 * @brief      Destroy the WebSocket connection and free all resources.
 *             This function must be the last function to call for an session.
 *             It is the opposite of the esp_websocket_client_init function and must be called with the same handle as input that a esp_websocket_client_init call returned.
 *             This might close all connections this handle has used.
 *
 *  Notes:
 *  - Cannot be called from the websocket event handler
 *
 * @param[in]  client  The client
 *
 * @return     esp_err_t
 */
esp_err_t esp_websocket_client_destroy(esp_websocket_client_handle_t client);

/**
 * @brief      Generic write data to the WebSocket connection; defaults to binary send
 *
 * @param[in]  client  The client
 * @param[in]  data    The data
 * @param[in]  len     The length
 * @param[in]  timeout Write data timeout in RTOS ticks
 *
 * @return
 *     - Number of data was sent
 *     - (-1) if any errors
 */
int esp_websocket_client_send(esp_websocket_client_handle_t client, const char *data, int len, TickType_t timeout);

/**
 * @brief      Write binary data to the WebSocket connection (data send with WS OPCODE=02, i.e. binary)
 *
 * @param[in]  client  The client
 * @param[in]  data    The data
 * @param[in]  len     The length
 * @param[in]  timeout Write data timeout in RTOS ticks
 *
 * @return
 *     - Number of data was sent
 *     - (-1) if any errors
 */
int esp_websocket_client_send_bin(esp_websocket_client_handle_t client, const char *data, int len, TickType_t timeout);

/**
 * @brief      Write textual data to the WebSocket connection (data send with WS OPCODE=01, i.e. text)
 *
 * @param[in]  client  The client
 * @param[in]  data    The data
 * @param[in]  len     The length
 * @param[in]  timeout Write data timeout in RTOS ticks
 *
 * @return
 *     - Number of data was sent
 *     - (-1) if any errors
 */
int esp_websocket_client_send_text(esp_websocket_client_handle_t client, const char *data, int len, TickType_t timeout);

/**
 * @brief      Close the WebSocket connection in a clean way
 *
 * Sequence of clean close initiated by client:
 * * Client sends CLOSE frame
 * * Client waits until server echos the CLOSE frame
 * * Client waits until server closes the connection
 * * Client is stopped the same way as by the `esp_websocket_client_stop()`
 *
 *  Notes:
 *  - Cannot be called from the websocket event handler
 *
 * @param[in]  client  The client
 * @param[in]  timeout Timeout in RTOS ticks for waiting
 *
 * @return     esp_err_t
 */
esp_err_t esp_websocket_client_close(esp_websocket_client_handle_t client, TickType_t timeout);

/**
 * @brief      Close the WebSocket connection in a clean way with custom code/data
 *             Closing sequence is the same as for esp_websocket_client_close()
 *
 *  Notes:
 *  - Cannot be called from the websocket event handler
 *
 * @param[in]  client  The client
 * @param[in]  code    Close status code as defined in RFC6455 section-7.4
 * @param[in]  data    Additional data to closing message
 * @param[in]  len     The length of the additional data
 * @param[in]  timeout Timeout in RTOS ticks for waiting
 *
 * @return     esp_err_t
 */
esp_err_t esp_websocket_client_close_with_code(esp_websocket_client_handle_t client, int code, const char *data, int len, TickType_t timeout);

/**
 * @brief      Check the WebSocket client connection state
 *
 * @param[in]  client  The client handle
 *
 * @return
 *     - true
 *     - false
 */
bool esp_websocket_client_is_connected(esp_websocket_client_handle_t client);

/**
 * @brief Register the Websocket Events
 *
 * @param client            The client handle
 * @param event             The event id
 * @param event_handler     The callback function
 * @param event_handler_arg User context
 * @return esp_err_t
 */
esp_err_t esp_websocket_register_events(esp_websocket_client_handle_t client,
                                        esp_websocket_event_id_t event,
                                        esp_event_handler_t event_handler,
                                        void *event_handler_arg);

#ifdef __cplusplus
}
#endif

#endif
