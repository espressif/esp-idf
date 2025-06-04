/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread command line interface(CLI).
 *
 * @note There are two ways to initialize the OpenThread CLI:
 *       1. By creating a dedicated task via `esp_openthread_cli_create_task`
 *
 *       2. By registering a console command with the ESP console via
 *          `esp_openthread_cli_console_command_register`
 *          If using this approach, the user must initialize the interface used
 *          by the console and also initialize esp_console manually. Additionally,
 *          the `host_connection_mode` should be set to `HOST_CONNECTION_MODE_NONE`.
 */
void esp_openthread_cli_init(void);

/**
 * @brief This function feeds a line to the OpenThread CLI.
 *
 * @note This function is thread-safe, the line will be copied and posted to
 *       the OpenThread task queue.
 *
 * @param[in] line  The input line.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *
 */
esp_err_t esp_openthread_cli_input(const char *line);

/**
 * @brief This function launches an exclusive loop for the OpenThread CLI.
 *
 */
void esp_openthread_cli_create_task(void);

/**
 * @brief This function registers an ESP Console command for the OpenThread CLI.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 */
esp_err_t esp_openthread_cli_console_command_register(void);

/**
 * @brief This function deregisters the ESP Console command for the OpenThread CLI.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if command is not registered
 */
esp_err_t esp_openthread_cli_console_command_unregister(void);

#ifdef __cplusplus
}
#endif
