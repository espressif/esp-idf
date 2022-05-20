/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
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
 * @param[in] priority  The priority of the created task.
 *
 */
void esp_openthread_cli_create_task(void);

#ifdef __cplusplus
}
#endif
