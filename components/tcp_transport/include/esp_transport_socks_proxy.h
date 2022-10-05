/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum socks_version_t {SOCKS4 = 4} socks_version_t;

typedef enum socks_transport_response_t {
    // The following values correspond to transport operation
    SOCKS_RESPONSE_TARGET_NOT_FOUND = 0xF0,
    SOCKS_RESPONSE_PROXY_UNREACHABLE = 0xF1,
    SOCKS_TIMEOUT = 0xF2,
    // The following values are defined by the SOCKS4 protocol
    SOCKS_RESPONSE_SUCCESS = 0x5a,
    SOCKS_RESPONSE_REQUEST_REJECTED = 0x5B,
    SOCKS_RESPONSE_NOT_RUNNING_IDENTD = 0x5c,
    SOCKS_RESPONSE_COULD_NOT_CONFIRM_ID = 0x5d,
} socks_transport_error_t;

/*
  * Socks configuration structure
  */
typedef struct esp_transport_socks_proxy_config_t {
    const socks_version_t version; /*!< Socks protocol version.*/
    const char *address;/*!< Proxy address*/
    const int port; /*< Proxy port*/
} esp_transport_socks_proxy_config_t;

/**
* @brief Create a proxy transport
* @param parent_handle Handle for the parent transport
* @param config Pointer to the configuration structure to use
*
* @return
*    - transport Handler for the created transport.
*    - NULL in case of failure
*/
esp_transport_handle_t esp_transport_socks_proxy_init(esp_transport_handle_t parent_handle, const esp_transport_socks_proxy_config_t *config);

/**
* @brief Changes the configuration of the proxy
* @param socks_transport Handle for the transport
* @param config Pointer to the configuration structure to use
*
* @return
*    - ESP_OK on success
*/
esp_err_t esp_transport_socks_proxy_set_config(esp_transport_handle_t socks_transport, const esp_transport_socks_proxy_config_t *config);

#ifdef __cplusplus
}
#endif
