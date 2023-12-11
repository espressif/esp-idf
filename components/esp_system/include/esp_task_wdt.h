/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Task Watchdog Timer (TWDT) configuration structure
 */
typedef struct {
    uint32_t timeout_ms;        /**< TWDT timeout duration in milliseconds */
    uint32_t idle_core_mask;    /**< Bitmask of the core whose idle task should be subscribed on initialization where 1 << i means that core i's idle task will be monitored by the TWDT */
    bool trigger_panic;         /**< Trigger panic when timeout occurs */
} esp_task_wdt_config_t;

/**
 * @brief Task Watchdog Timer (TWDT) user handle
 */
typedef struct esp_task_wdt_user_handle_s * esp_task_wdt_user_handle_t;

/**
 * @brief  Initialize the Task Watchdog Timer (TWDT)
 *
 * This function configures and initializes the TWDT. This function will subscribe the idle tasks if
 * configured to do so. For other tasks, users can subscribe them using esp_task_wdt_add() or esp_task_wdt_add_user().
 * This function won't start the timer if no task have been registered yet.
 *
 * @note esp_task_wdt_init() must only be called after the scheduler is started. Moreover, it must not be called by
 *       multiple tasks simultaneously.
 * @param[in] config Configuration structure
 * @return
 *  - ESP_OK: Initialization was successful
 *  - ESP_ERR_INVALID_STATE: Already initialized
 *  - Other: Failed to initialize TWDT
 */
esp_err_t esp_task_wdt_init(const esp_task_wdt_config_t *config);

/**
 * @brief Reconfigure the Task Watchdog Timer (TWDT)
 *
 * The function reconfigures the running TWDT. It must already be initialized when this function is called.
 *
 * @note esp_task_wdt_reconfigure() must not be called by multiple tasks simultaneously.
 *
 * @param[in] config Configuration structure
 *
 * @return
 *  - ESP_OK: Reconfiguring was successful
 *  - ESP_ERR_INVALID_STATE: TWDT not initialized yet
 *  - Other: Failed to initialize TWDT
 */
esp_err_t esp_task_wdt_reconfigure(const esp_task_wdt_config_t *config);

/**
 * @brief   Deinitialize the Task Watchdog Timer (TWDT)
 *
 * This function will deinitialize the TWDT, and unsubscribe any idle tasks. Calling this function whilst other tasks
 * are still subscribed to the TWDT, or when the TWDT is already deinitialized, will result in an error code being
 * returned.
 *
 * @note esp_task_wdt_deinit() must not be called by multiple tasks simultaneously.
 * @return
 *  - ESP_OK: TWDT successfully deinitialized
 *  - Other: Failed to deinitialize TWDT
 */
esp_err_t esp_task_wdt_deinit(void);

/**
 * @brief Subscribe a task to the Task Watchdog Timer (TWDT)
 *
 * This function subscribes a task to the TWDT. Each subscribed task must periodically call esp_task_wdt_reset() to
 * prevent the TWDT from elapsing its timeout period. Failure to do so will result in a TWDT timeout.
 *
 * @param task_handle Handle of the task. Input NULL to subscribe the current running task to the TWDT
 * @return
 *  - ESP_OK: Successfully subscribed the task to the TWDT
 *  - Other: Failed to subscribe task
 */
esp_err_t esp_task_wdt_add(TaskHandle_t task_handle);

/**
 * @brief Subscribe a user to the Task Watchdog Timer (TWDT)
 *
 * This function subscribes a user to the TWDT. A user of the TWDT is usually a function that needs to run
 * periodically. Each subscribed user must periodically call esp_task_wdt_reset_user() to prevent the TWDT from elapsing
 * its timeout period. Failure to do so will result in a TWDT timeout.
 *
 * @param[in] user_name String to identify the user
 * @param[out] user_handle_ret Handle of the user
 * @return
 *  - ESP_OK: Successfully subscribed the user to the TWDT
 *  - Other: Failed to subscribe user
 */
esp_err_t esp_task_wdt_add_user(const char *user_name, esp_task_wdt_user_handle_t *user_handle_ret);

/**
 * @brief Reset the Task Watchdog Timer (TWDT) on behalf of the currently running task
 *
 * This function will reset the TWDT on behalf of the currently running task. Each subscribed task must periodically
 * call this function to prevent the TWDT from timing out. If one or more subscribed tasks fail to reset the TWDT on
 * their own behalf, a TWDT timeout will occur.
 *
 * @return
 *  - ESP_OK: Successfully reset the TWDT on behalf of the currently running task
 *  - Other: Failed to reset
 */
esp_err_t esp_task_wdt_reset(void);

/**
 * @brief Reset the Task Watchdog Timer (TWDT) on behalf of a user
 *
 * This function will reset the TWDT on behalf of a user. Each subscribed user must periodically call this function to
 * prevent the TWDT from timing out. If one or more subscribed users fail to reset the TWDT on their own behalf, a TWDT
 * timeout will occur.
 *
 * @param[in] user_handle User handle
 *  - ESP_OK: Successfully reset the TWDT on behalf of the user
 *  - Other: Failed to reset
 */
esp_err_t esp_task_wdt_reset_user(esp_task_wdt_user_handle_t user_handle);

/**
 * @brief Unsubscribes a task from the Task Watchdog Timer (TWDT)
 *
 * This function will unsubscribe a task from the TWDT. After being unsubscribed, the task should no longer call
 * esp_task_wdt_reset().
 *
 * @param[in] task_handle Handle of the task. Input NULL to unsubscribe the current running task.
 * @return
 *  - ESP_OK: Successfully unsubscribed the task from the TWDT
 *  - Other: Failed to unsubscribe task
 */
esp_err_t esp_task_wdt_delete(TaskHandle_t task_handle);

/**
 * @brief Unsubscribes a user from the Task Watchdog Timer (TWDT)
 *
 * This function will unsubscribe a user from the TWDT. After being unsubscribed, the user should no longer call
 * esp_task_wdt_reset_user().
 *
 * @param[in] user_handle User handle
 * @return
 *  - ESP_OK: Successfully unsubscribed the user from the TWDT
 *  - Other: Failed to unsubscribe user
 */
esp_err_t esp_task_wdt_delete_user(esp_task_wdt_user_handle_t user_handle);

/**
 * @brief Query whether a task is subscribed to the Task Watchdog Timer (TWDT)
 *
 * This function will query whether a task is currently subscribed to the TWDT, or whether the TWDT is initialized.
 *
 * @param[in] task_handle Handle of the task. Input NULL to query the current running task.
 * @return:
 *  - ESP_OK: The task is currently subscribed to the TWDT
 *  - ESP_ERR_NOT_FOUND: The task is not subscribed
 *  - ESP_ERR_INVALID_STATE: TWDT was never initialized
 */
esp_err_t esp_task_wdt_status(TaskHandle_t task_handle);

/**
 * @brief User ISR callback placeholder
 *
 * This function is called by task_wdt_isr function (ISR for when TWDT times out). It can be defined in user code to
 * handle TWDT events.
 *
 * @note It has the same limitations as the interrupt function. Do not use ESP_LOGx functions inside.
 */
void __attribute__((weak)) esp_task_wdt_isr_user_handler(void);

typedef void (*task_wdt_msg_handler)(void *opaque, const char *msg);

/**
 * @brief Prints or retrieves information about tasks/users that triggered the Task Watchdog Timeout.
 *
 * This function provides various operations to handle tasks/users that did not reset the Task Watchdog in time.
 * It can print detailed information about these tasks/users, such as their names, associated CPUs, and whether they have been reset.
 * Additionally, it can retrieve the total length of the printed information or the CPU affinity of the failing tasks.
 *
 * @param[in]  msg_handler Optional message handler function that will be called for each printed line.
 * @param[in]  opaque      Optional pointer to opaque data that will be passed to the message handler function.
 * @param[out] cpus_fail   Optional pointer to an integer where the CPU affinity of the failing tasks will be stored.
 *
 * @return
 *     - ESP_OK: The function executed successfully.
 *     - ESP_FAIL: No triggered tasks were found, and thus no information was printed or retrieved.
 *
 * @note
 *     - If `msg_handler` is not provided, the information will be printed to console using ESP_EARLY_LOGE.
 *     - If `msg_handler` is provided, the function will send the printed information to the provided message handler function.
 *     - If `cpus_fail` is provided, the function will store the CPU affinity of the failing tasks in the provided integer.
 *     - During the execution of this function, logging is allowed in critical sections, as TWDT timeouts are considered fatal errors.
 */
esp_err_t esp_task_wdt_print_triggered_tasks(task_wdt_msg_handler msg_handler, void *opaque, int *cpus_fail);

#ifdef __cplusplus
}
#endif
