/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initializes an uart port with the given config.
 *
 * @note   The user still needs to open the file descriptor by self.
 *
 * @param[in]   config  The uart configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_uart_init_port(const esp_openthread_uart_config_t *config);

/**
 * @brief  Initializes the console UART for OpenThread host connection.
 *
 * @param[in]   config  The platform configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_host_cli_uart_init(const esp_openthread_platform_config_t *config);

/**
 * @brief  Initializes the console USB JTAG for OpenThread host connection.
 *
 * @param[in]   config  The platform configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_host_cli_usb_init(const esp_openthread_platform_config_t *config);

/**
 * @brief  Initializes the RCP UART for OpenThread host connection.
 *
 * @param[in]   config  The platform configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_host_rcp_uart_init(const esp_openthread_platform_config_t *config);

/**
 * @brief  Deintializes the uart for OpenThread host connection.
 *
 */
void esp_openthread_uart_deinit(void);

/**
 * @brief  Deintializes the uart for OpenThread host connection.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_uart_update(esp_openthread_mainloop_context_t *context);

/**
 * @brief  Performs the uart I/O for OpenThread.
 *
 * @param[in]   instance  The Openthread instance.
 * @param[in]   mainloop  The main loop context.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_uart_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

#ifdef __cplusplus
}
#endif
