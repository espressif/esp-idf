/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initializes the custom transport for OpenThread host connection.
 *
 * @param[in]   config  The platform configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if radio mode is invalid
 *      - ESP_FAIL or other error codes on failure
 *
 */
esp_err_t esp_openthread_host_rcp_transport_init(const esp_openthread_platform_config_t *config);

/**
 * @brief  Deinitializes the custom transport for OpenThread host connection.
 *
 */
void esp_openthread_host_rcp_transport_deinit(void);

#ifdef __cplusplus
}
#endif
