/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "esp_radio_spinel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Multipan host radio backend.
 *
 * One physical UART RCP link is owned by this module. Incoming spinel frames
 * are fanned out into per-client queues.
 */
typedef struct {
    esp_openthread_radio_mode_t radio_mode;           /*!< Must be RADIO_MODE_UART_RCP */
    esp_radio_spinel_uart_config_t radio_uart_config; /*!< UART RCP */
} esp_radio_spinel_multipan_radio_config_t;

/**
 * @brief Initialize the shared RCP backend.
 *
 * Currently only `RADIO_MODE_UART_RCP` is supported. This function is expected
 * to be called once; a second call is rejected. Call this before starting the
 * OpenThread or Zigbee stacks so they can attach as clients.
 *
 * @param[in]  radio_config  Radio backend configuration. Must not be NULL.
 *                           `radio_mode` must be `RADIO_MODE_UART_RCP`.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if `radio_config` is NULL
 *      - ESP_ERR_INVALID_STATE if the backend is already initialized
 *      - ESP_ERR_NOT_SUPPORTED if `radio_mode` is not `RADIO_MODE_UART_RCP`
 *      - ESP_ERR_NO_MEM if a lock, queue, or task cannot be created
 *      - ESP_FAIL on other failures
 */
esp_err_t esp_radio_spinel_multipan_init(const esp_radio_spinel_multipan_radio_config_t *radio_config);

/**
 * @brief Tear down the backend. All client fds must already be closed.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if the backend is not initialized, or if any
 *        client is still open
 */
esp_err_t esp_radio_spinel_multipan_deinit(void);

#ifdef __cplusplus
}
#endif
