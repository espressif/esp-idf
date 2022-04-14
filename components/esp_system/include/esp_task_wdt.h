/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initialize the Task Watchdog Timer (TWDT)
 *
 * This function configures and initializes the TWDT. If the TWDT is already initialized when this function is called,
 * this function will update the TWDT's timeout period and panic configurations instead. After initializing the TWDT,
 * any task can elect to be watched by the TWDT by subscribing to it using esp_task_wdt_add().
 *
 * @note esp_task_wdt_init() must only be called after the scheduler started
 * @param[in] timeout Timeout period of TWDT in seconds
 * @param[in] panic Flag that controls whether the panic handler will be executed when the TWDT times out
 * @return
 *  - ESP_OK: Initialization was successful
 *  - ESP_ERR_NO_MEM: Initialization failed due insufficient memory
 */
esp_err_t esp_task_wdt_init(uint32_t timeout, bool panic);

/**
 * @brief   Deinitialize the Task Watchdog Timer (TWDT)
 *
 * This function will deinitialize the TWDT. Calling this function whilst tasks are still subscribed to the TWDT, or
 * when the TWDT is already deinitialized, will result in an error code being returned.
 *
 * @return
 *      - ESP_OK: TWDT successfully deinitialized
 *      - ESP_ERR_INVALID_STATE: TWDT was never initialized, or tasks are still subscribed
 */
esp_err_t esp_task_wdt_deinit(void);

/**
 * @brief Subscribe a task to the Task Watchdog Timer (TWDT)
 *
 * This function subscribes a task to the TWDT. Each subscribed task must periodically call esp_task_wdt_reset() to
 * prevent the TWDT from elapsing its timeout period. Failure to do so will result in a TWDT timeout. If the task being
 * subscribed is one of the Idle Tasks, this function will automatically enable esp_task_wdt_reset() to called from the
 * Idle Hook of the Idle Task.
 *
 * Calling this function whilst the TWDT is uninitialized or attempting to subscribe an already subscribed task will
 * result in an error code being returned.
 *
 * @param handle Handle of the task. Input NULL to subscribe the current running task to the TWDT
 * @return
 *  - ESP_OK: Successfully subscribed the task to the TWDT
 *  - ESP_ERR_INVALID_ARG: The task is already subscribed
 *  - ESP_ERR_NO_MEM: Could not subscribe the insufficient memory
 *  - ESP_ERR_INVALID_STATE: TWDT was never initialized
 */
esp_err_t esp_task_wdt_add(TaskHandle_t handle);

/**
 * @brief Reset the Task Watchdog Timer (TWDT) on behalf of the currently running task
 *
 * This function will reset the TWDT on behalf of the currently running task. Each subscribed task must periodically
 * call this function to prevent the TWDT from timing out. If one or more subscribed tasks fail to reset the TWDT on
 * their own behalf, a TWDT timeout will occur. If the IDLE tasks have been subscribed to the TWDT, they will
 * automatically call this function from their idle hooks. Calling this function from a task that has not subscribed to
 * the TWDT, or when the TWDT is uninitialized will result in an error code being returned.
 *
 * @return
 *  - ESP_OK: Successfully reset the TWDT on behalf of the currently running task
 *  - ESP_ERR_NOT_FOUND: The task is not subscribed
 *  - ESP_ERR_INVALID_STATE: TWDT was never initialized
 */
esp_err_t esp_task_wdt_reset(void);

/**
 * @brief Unsubscribes a task from the Task Watchdog Timer (TWDT)
 *
 * This function will unsubscribe a task from the TWDT. After being unsubscribed, the task should no longer call
 * esp_task_wdt_reset(). If the task is an IDLE task, this function will automatically disable the calling of
 * esp_task_wdt_reset() from the Idle Hook. Calling this function whilst the TWDT is uninitialized or attempting to
 * unsubscribe an already unsubscribed task from the TWDT will result in an error code being returned.
 *
 * @param[in] handle Handle of the task. Input NULL to unsubscribe the current running task.
 * @return
 *  - ESP_OK: Successfully unsubscribed the task from the TWDT
 *  - ESP_ERR_NOT_FOUND: The task is not subscribed
 *  - ESP_ERR_INVALID_STATE: TWDT was never initialized
 */
esp_err_t esp_task_wdt_delete(TaskHandle_t handle);

/**
 * @brief Query whether a task is subscribed to the Task Watchdog Timer (TWDT)
 *
 * This function will query whether a task is currently subscribed to the TWDT, or whether the TWDT is initialized.
 *
 * @param[in] handle Handle of the task. Input NULL to query the current running task.
 * @return:
 *  - ESP_OK: The task is currently subscribed to the TWDT
 *  - ESP_ERR_NOT_FOUND: The task is not subscribed
 *  - ESP_ERR_INVALID_STATE: TWDT was never initialized
 */
esp_err_t esp_task_wdt_status(TaskHandle_t handle);

#ifdef __cplusplus
}
#endif
