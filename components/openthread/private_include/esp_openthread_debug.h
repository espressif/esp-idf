/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <esp_err.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_OPENTHREAD_TASK_BLOCK_MONITOR
/**
 * @brief This function creates the OpenThread task block monitor.
 *        The monitor task periodically checks whether the OpenThread mainloop
 *        has been blocked for more than `CONFIG_OPENTHREAD_TASK_BLOCK_MONITOR_TIMEOUT`
 *        seconds and prints backtraces when a block is detected.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if the monitor task cannot be created
 *
 */
esp_err_t esp_openthread_task_block_monitor_create(void);

/**
 * @brief This function deletes the OpenThread task block monitor.
 *
 * @return
 *      - ESP_OK on success
 *
 */
esp_err_t esp_openthread_task_block_monitor_delete(void);

/**
 * @brief This function enables or disables the OpenThread task block monitor.
 *
 * @note When enabled, the latest mainloop activity timestamp is updated and the
 *       monitor task starts checking for blocking. When disabled, monitoring is
 *       temporarily stopped.
 *
 * @param[in] enable  True to enable the monitor, false to disable it.
 *
 */
void esp_openthread_task_block_monitor_set(bool enable);
#endif  // CONFIG_OPENTHREAD_TASK_BLOCK_MONITOR

#ifdef __cplusplus
}
#endif
