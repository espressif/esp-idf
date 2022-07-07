/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"

#if CONFIG_ESP_TASK_WDT

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Stop the Task Watchdog Timer (TWDT)
 *
 * This function will temporarily stop the timer until it is restarted/resumed by a call to esp_task_wdt_restart().

 * @note esp_task_wdt_stop() must not be called by multiple tasks simultaneously.
 * @return
 *  - ESP_OK: TWDT successfully stopped
 *  - Other: Failed to stop the TWDT
 */
esp_err_t esp_task_wdt_stop(void);

/**
 * @brief   Restart the Task Watchdog Timer (TWDT)
 *
 * This function will restart/resume the timer after it has been stopped by esp_task_wdt_stop().

 * @note esp_task_wdt_restart() must not be called by multiple tasks simultaneously.
 * @return
 *  - ESP_OK: TWDT successfully stopped
 *  - Other: Failed to stop the TWDT
 */
esp_err_t esp_task_wdt_restart(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_ESP_TASK_WDT
