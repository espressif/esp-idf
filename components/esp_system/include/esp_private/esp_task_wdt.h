/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"

#if CONFIG_ESP_TASK_WDT_EN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type used to define the context of a Task WatchDog Timer implementation.
 *        This is used internally in the TWDT driver, it is implementation specific.
 */
typedef void* twdt_ctx_t;

/**
 * @brief Type of the function used as an ISR callback.
 */
typedef void (*twdt_isr_callback)(void*);

/**
 * @brief   Stop the Task Watchdog Timer (TWDT)
 *
 * This function will temporarily stop the timer until it is restarted by a call to esp_task_wdt_restart().

 * @note esp_task_wdt_stop() must not be called by multiple tasks simultaneously.
 * @return
 *  - ESP_OK: TWDT successfully stopped
 *  - Other: Failed to stop the TWDT
 */
esp_err_t esp_task_wdt_stop(void);

/**
 * @brief   Restart the Task Watchdog Timer (TWDT)
 *
 * This function will restart the timer after it has been stopped by esp_task_wdt_stop().

 * @note esp_task_wdt_restart() must not be called by multiple tasks simultaneously.
 * @return
 *  - ESP_OK: TWDT successfully stopped
 *  - Other: Failed to stop the TWDT
 */
esp_err_t esp_task_wdt_restart(void);

#ifdef __cplusplus
}
#endif

#endif // CONFIG_ESP_TASK_WDT_EN
