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

#ifndef _ESP_TRANSPORT_H_
#define _ESP_TRANSPORT_H_

#include <esp_err.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief Keep alive parameters structure
*/
typedef struct esp_transport_keepalive {
    bool            keep_alive_enable;      /*!< Enable keep-alive timeout */
    int             keep_alive_idle;        /*!< Keep-alive idle time (second) */
    int             keep_alive_interval;    /*!< Keep-alive interval time (second) */
    int             keep_alive_count;       /*!< Keep-alive packet retry send count */
} esp_transport_keep_alive_t;

typedef struct esp_transport_internal* esp_transport_list_handle_t;
typedef struct esp_transport_item_t* esp_transport_handle_t;

typedef int (*connect_func)(esp_transport_handle_t t, const char *host, int port, int timeout_ms);
typedef int (*io_func)(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms);
typedef int (*io_read_func)(esp_transport_handle_t t, char *buffer, int len, int timeout_ms);
typedef int (*trans_func)(esp_transport_handle_t t);
typedef int (*poll_func)(esp_transport_handle_t t, int timeout_ms);
typedef int (*connect_async_func)(esp_transport_handle_t t, const char *host, int port, int timeout_ms);
typedef esp_transport_handle_t (*payload_transfer_func)(esp_transport_handle_t);

typedef struct esp_tls_last_error* esp_tls_error_handle_t;

/**
 * @brief      Create transport list
 *
 * @return     A handle can hold all transports
 */
esp_transport_list_handle_t esp_transport_list_init(void);

/**
 * @brief      Cleanup and free all transports, include itself,
 *             this function will invoke esp_transport_destroy of every transport have added this the list
 *
 * @param[in]  list  The list
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_transport_list_destroy(esp_transport_list_handle_t list);

/**
 * @brief      Add a transport to the list, and define a scheme to indentify this transport in the list
 *
 * @param[in]  list    The list
 * @param[in]  t       The Transport
 * @param[in]  scheme  The scheme
 *
 * @return
 *     - ESP_OK
 */
esp_err_t esp_transport_list_add(esp_transport_list_handle_t list, esp_transport_handle_t t, const char *scheme);

/**
 * @brief      This function will remove all transport from the list,
 *             invoke esp_transport_destroy of every transport have added this the list
 *
 * @param[in]  list  The list
 *
 * @return
 *     - ESP_OK
 *     - ESP_ERR_INVALID_ARG
 */
esp_err_t esp_transport_list_clean(esp_transport_list_handle_t list);

/**
 * @brief      Get the transport by scheme, which has been defined when calling function `esp_transport_list_add`
 *
 * @param[in]  list  The list
 * @param[in]  tag   The tag
 *
 * @return     The transport handle
 */
esp_transport_handle_t esp_transport_list_get_transport(esp_transport_list_handle_t list, const char *scheme);

/**
 * @brief      Initialize a transport handle object
 *
 * @return     The transport handle
 */
esp_transport_handle_t esp_transport_init(void);

/**
 * @brief      Cleanup and free memory the transport
 *
 * @param[in]  t     The transport handle
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_transport_destroy(esp_transport_handle_t t);

/**
 * @brief      Get default port number used by this transport
 *
 * @param[in]  t     The transport handle
 *
 * @return     the port number
 */
int esp_transport_get_default_port(esp_transport_handle_t t);

/**
 * @brief      Set default port number that can be used by this transport
 *
 * @param[in]  t     The transport handle
 * @param[in]  port  The port number
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_transport_set_default_port(esp_transport_handle_t t, int port);

/**
 * @brief      Transport connection function, to make a connection to server
 *
 * @param      t           The transport handle
 * @param[in]  host        Hostname
 * @param[in]  port        Port
 * @param[in]  timeout_ms  The timeout milliseconds (-1 indicates wait forever)
 *
 * @return
 * - socket for will use by this transport
 * - (-1) if there are any errors, should check errno
 */
int esp_transport_connect(esp_transport_handle_t t, const char *host, int port, int timeout_ms);

/**
 * @brief      Non-blocking transport connection function, to make a connection to server
 *
 * @param      t           The transport handle
 * @param[in]  host        Hostname
 * @param[in]  port        Port
 * @param[in]  timeout_ms  The timeout milliseconds (-1 indicates wait forever)
 *
 * @return
 * - socket for will use by this transport
 * - (-1) if there are any errors, should check errno
 */
int esp_transport_connect_async(esp_transport_handle_t t, const char *host, int port, int timeout_ms);

/**
 * @brief      Transport read function
 *
 * @param      t           The transport handle
 * @param      buffer      The buffer
 * @param[in]  len         The length
 * @param[in]  timeout_ms  The timeout milliseconds (-1 indicates wait forever)
 *
 * @return
 *  - Number of bytes was read
 *  - (-1) if there are any errors, should check errno
 */
int esp_transport_read(esp_transport_handle_t t, char *buffer, int len, int timeout_ms);

/**
 * @brief      Poll the transport until readable or timeout
 *
 * @param[in]  t           The transport handle
 * @param[in]  timeout_ms  The timeout milliseconds (-1 indicates wait forever)
 *
 * @return
 *     - 0      Timeout
 *     - (-1)   If there are any errors, should check errno
 *     - other  The transport can read
 */
int esp_transport_poll_read(esp_transport_handle_t t, int timeout_ms);

/**
 * @brief      Transport write function
 *
 * @param      t           The transport handle
 * @param      buffer      The buffer
 * @param[in]  len         The length
 * @param[in]  timeout_ms  The timeout milliseconds (-1 indicates wait forever)
 *
 * @return
 *  - Number of bytes was written
 *  - (-1) if there are any errors, should check errno
 */
int esp_transport_write(esp_transport_handle_t t, const char *buffer, int len, int timeout_ms);

/**
 * @brief      Poll the transport until writeable or timeout
 *
 * @param[in]  t           The transport handle
 * @param[in]  timeout_ms  The timeout milliseconds (-1 indicates wait forever)
 *
 * @return
 *     - 0      Timeout
 *     - (-1)   If there are any errors, should check errno
 *     - other  The transport can write
 */
int esp_transport_poll_write(esp_transport_handle_t t, int timeout_ms);

/**
 * @brief      Transport close
 *
 * @param      t     The transport handle
 *
 * @return
 * - 0 if ok
 * - (-1) if there are any errors, should check errno
 */
int esp_transport_close(esp_transport_handle_t t);

/**
 * @brief      Get user data context of this transport
 *
 * @param[in]  t        The transport handle
 *
 * @return     The user data context
 */
void *esp_transport_get_context_data(esp_transport_handle_t t);

/**
 * @brief      Get transport handle of underlying protocol
 *             which can access this protocol payload directly
 *             (used for receiving longer msg multiple times)
 *
 * @param[in]  t        The transport handle
 *
 * @return     Payload transport handle
 */
esp_transport_handle_t esp_transport_get_payload_transport_handle(esp_transport_handle_t t);

/**
 * @brief      Set the user context data for this transport
 *
 * @param[in]  t        The transport handle
 * @param      data     The user data context
 *
 * @return
 *     - ESP_OK
 */
esp_err_t esp_transport_set_context_data(esp_transport_handle_t t, void *data);

/**
 * @brief      Set transport functions for the transport handle
 *
 * @param[in]  t            The transport handle
 * @param[in]  _connect     The connect function pointer
 * @param[in]  _read        The read function pointer
 * @param[in]  _write       The write function pointer
 * @param[in]  _close       The close function pointer
 * @param[in]  _poll_read   The poll read function pointer
 * @param[in]  _poll_write  The poll write function pointer
 * @param[in]  _destroy     The destroy function pointer
 *
 * @return
 *     - ESP_OK
 */
esp_err_t esp_transport_set_func(esp_transport_handle_t t,
                             connect_func _connect,
                             io_read_func _read,
                             io_func _write,
                             trans_func _close,
                             poll_func _poll_read,
                             poll_func _poll_write,
                             trans_func _destroy);


/**
 * @brief      Set transport functions for the transport handle
 *
 * @param[in]  t                    The transport handle
 * @param[in]  _connect_async_func  The connect_async function pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_transport_set_async_connect_func(esp_transport_handle_t t, connect_async_func _connect_async_func);

/**
 * @brief      Set parent transport function to the handle
 *
 * @param[in]  t                    The transport handle
 * @param[in]  _parent_transport    The underlying transport getter pointer
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t esp_transport_set_parent_transport_func(esp_transport_handle_t t, payload_transfer_func _parent_transport);

/**
 * @brief      Returns esp_tls error handle.
 *             Warning: The returned pointer is valid only as long as esp_transport_handle_t exists. Once transport
 *             handle gets destroyed, this value (esp_tls_error_handle_t) is freed automatically.
 *
 * @param[in]  A transport handle
 *
 * @return
 *            - valid pointer of esp_error_handle_t
 *            - NULL if invalid transport handle
 */
esp_tls_error_handle_t esp_transport_get_error_handle(esp_transport_handle_t t);

/**
 * @brief      Get and clear last captured socket errno
 *
 * Socket errno is internally stored whenever any of public facing API
 * for reading, writing, polling or connection fails returning negative return code.
 * The error code corresponds to the `SO_ERROR` value retrieved from the underlying
 * transport socket using `getsockopt()` API. After reading the captured errno,
 * the internal value is cleared to 0.
 *
 * @param[in] t The transport handle
 *
 * @return
 *   - >=0 Last captured socket errno
 *   - -1  Invalid transport handle or invalid transport's internal error storage
 */
int esp_transport_get_errno(esp_transport_handle_t t);

#ifdef __cplusplus
}
#endif
#endif /* _ESP_TRANSPORT_ */
