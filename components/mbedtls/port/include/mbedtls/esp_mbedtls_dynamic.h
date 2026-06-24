/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "mbedtls/ssl.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the dynamic buffer rx statically after the handshake. This is to avoid frequent allocation and deallocation of dynamic buffer.
 *
 * @param ssl mbedtls ssl context
 * @return esp_err_t
 *         - ESP_OK: Successfully set the rx buffer to static
 *         - ESP_ERR_NO_MEM: Failed to allocate memory for the rx buffer
 */
esp_err_t esp_mbedtls_dynamic_set_rx_buf_static(mbedtls_ssl_context *ssl);

#ifdef __cplusplus
}
#endif
