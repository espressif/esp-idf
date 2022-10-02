/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "../esp_task_wdt.h"
#include "esp_private/esp_task_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   Allocate and initialize the Task Watchdog Timer (TWDT) with the given configuration.
 *
 * @param[in] config Pointer to the configuration structure
 * @param[out] obj Abstract context for the current timer, this will be passed to all the other functions
 *
 * @return
 *  - ESP_OK: Successfully initialized and configured the timer
 *  - Other: Failed to initialize the timer
 */
esp_err_t esp_task_wdt_impl_timer_allocate(const esp_task_wdt_config_t *config,
                                           twdt_isr_callback callback,
                                           twdt_ctx_t *obj);


/**
 * @brief   Reconfigure a timer.
 *
 * The timer must be stopped when calling this function. The timer will not be restarted at the end of this
 * function.
 *
 * @param[in] config Pointer to the configuration structure
 *
 * @return
 *  - ESP_OK: Successfully reconfigured the timer
 *  - Other: Failed to reconfigure the timer
 */
esp_err_t esp_task_wdt_impl_timer_reconfigure(twdt_ctx_t obj, const esp_task_wdt_config_t *config);

/**
 * @brief   Free the Task Watchdog Timer (TWDT).
 *
 * @param[in] obj Abstract implementation context
 *
 */
void esp_task_wdt_impl_timer_free(twdt_ctx_t obj);


/**
 * @brief   Feed the Task Watchdog Timer (TWDT)
 *
 * Feed the timer underneath to prevent it from triggering for the next period (configured at initialization).
 *
 * @param[in] obj Abstract implementation context
 * @return
 *  - ESP_OK: timer successfully feeded
 *  - Other: failed to feed the timer
 */
esp_err_t esp_task_wdt_impl_timer_feed(twdt_ctx_t obj);


/**
 * @brief   Function invoked as soon as the Task Watchdog Timer (TWDT) ISR callback is called.
 *
 * @param[in] obj Abstract implementation context
 */
void esp_task_wdt_impl_timeout_triggered(twdt_ctx_t obj);


/**
 * @brief   Stop the Task Watchdog Timer (TWDT).
 *
 * @param[in] obj Abstract implementation context
 *
 */
esp_err_t esp_task_wdt_impl_timer_stop(twdt_ctx_t obj);


/**
 * @brief   Restart the Task Watchdog Timer (TWDT)
 *
 * This function will restart/resume the timer after it has been stopped.
 *
 * @param[in] obj Abstract implementation context
 * @return
 *  - ESP_OK: timer successfully stopped
 *  - Other: failed to stop the timer
 */
esp_err_t esp_task_wdt_impl_timer_restart(twdt_ctx_t obj);


#ifdef __cplusplus
}
#endif
