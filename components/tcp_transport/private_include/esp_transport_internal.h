/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_TRANSPORT_INTERNAL_H_
#define _ESP_TRANSPORT_INTERNAL_H_

#include "sys/time.h"
#include "esp_transport.h"
#include "sys/socket.h"
#include "sys/queue.h"
#include "esp_log.h"
#include "esp_tls.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef int (*get_socket_func)(esp_transport_handle_t t);

/**
 * Transport layer error structure including
 * * esp-tls last error storage
 * * sock-errno
 */
typedef struct esp_transport_error_storage {
    struct esp_tls_last_error esp_tls_err_h_base;   /*!< esp-tls last error container */
    // additional fields
    int    sock_errno;                              /*!< last socket error captured for this transport */
} esp_transport_error_storage;
typedef struct esp_foundation_transport {
    esp_transport_error_storage  *error_handle;           /*!< Pointer to the transport error container */
} esp_foundation_transport_t;

/**
 * Transport layer structure, which will provide functions, basic properties for transport types
 */
struct esp_transport_item_t {
    int             port;
    char            *scheme;        /*!< Tag name */
    void            *data;          /*!< Additional transport data */
    connect_func    _connect;       /*!< Connect function of this transport */
    io_read_func    _read;          /*!< Read */
    io_func         _write;         /*!< Write */
    trans_func      _close;         /*!< Close */
    poll_func       _poll_read;     /*!< Poll and read */
    poll_func       _poll_write;    /*!< Poll and write */
    trans_func      _destroy;       /*!< Destroy and free transport */
    connect_async_func _connect_async;      /*!< non-blocking connect function of this transport */
    payload_transfer_func  _parent_transfer;        /*!< Function returning underlying transport layer */
    get_socket_func        _get_socket;             /*!< Function returning the transport's socket */
    esp_transport_keep_alive_t *keep_alive_cfg;     /*!< TCP keep-alive config */
    struct esp_foundation_transport *foundation;          /*!< Foundation transport pointer available from each transport */

    STAILQ_ENTRY(esp_transport_item_t) next;
};

/**
 * @brief      Utility macro to be used for NULL ptr check after malloc
 *
 */
#define ESP_TRANSPORT_MEM_CHECK(TAG, a, action) if (!(a)) {                                         \
        ESP_LOGE(TAG,"%s(%d): %s", __FUNCTION__, __LINE__, "Memory exhausted");                     \
        action;                                                                                     \
        }

/**
 * @brief      Utility macro for checking the error code of esp_err_t
 */
#define ESP_TRANSPORT_ERR_OK_CHECK(TAG, err, action) \
        {                                                     \
            esp_err_t _esp_transport_err_to_check = err;      \
            if (_esp_transport_err_to_check != ESP_OK) {      \
                ESP_LOGE(TAG,"%s(%d): Expected ESP_OK; reported: %d", __FUNCTION__, __LINE__, _esp_transport_err_to_check); \
                action;                                       \
            }                                                 \
        }


/**
 * @brief      Convert milliseconds to timeval struct for valid timeouts, otherwise
 *             (if "wait forever" requested by timeout_ms=-1) timeval structure is not updated and NULL returned
 *
 * @param[in]  timeout_ms  The timeout value in milliseconds or -1 to waiting forever
 * @param[out] tv          Pointer to timeval struct
 *
 * @return
 * - NULL if timeout_ms=-1 (wait forever)
 * - pointer to the updated timeval structure (provided as "tv" argument) with recalculated timeout value
 */
struct timeval* esp_transport_utils_ms_to_timeval(int timeout_ms, struct timeval *tv);

/**
 * @brief  Initialize foundation struct
 *
 * @return
 *        esp_foundation_transport_t
 *        NULL in case of errors
 */
esp_foundation_transport_t * esp_transport_init_foundation_transport(void);

void esp_transport_destroy_foundation_transport(esp_foundation_transport_t *foundation);

/**
 * @brief      Captures internal tcp connection error
 *
 * This is internally translated to esp-tls return codes of esp_err_t type, since the esp-tls
 * will be used as TCP transport layer
 *
 * @param[in] t The transport handle
 * @param[in] error Internal tcp-transport's error
 *
 */
void capture_tcp_transport_error(esp_transport_handle_t t, enum esp_tcp_transport_err_t error);

/**
 * @brief Returns underlying socket for the supplied transport handle
 *
 * @param t Transport handle
 *
 * @return Socket file descriptor in case of success
 *         -1 in case of error
 */
int esp_transport_get_socket(esp_transport_handle_t t);

/**
 * @brief      Captures the current errno
 *
 * @param[in] t The transport handle
 * @param[in] sock_errno Socket errno to store in internal transport structures
 *
 */
void esp_transport_capture_errno(esp_transport_handle_t t, int sock_errno);

/**
 * @brief      Sets error to common transport handle
 *
 *             Note: This function copies the supplied error handle object to tcp_transport's internal
 *             error handle object
 *
 * @param[in]  A transport handle
 *
 */
void esp_transport_set_errors(esp_transport_handle_t t, const esp_tls_error_handle_t error_handle);

#ifdef __cplusplus
}
#endif

#endif //_ESP_TRANSPORT_INTERNAL_H_
