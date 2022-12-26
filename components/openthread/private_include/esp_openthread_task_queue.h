/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_openthread_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OpenThread task declaration
 *
 */
typedef void (*esp_openthread_task_t)(void *);

/**
 * @brief This function allocs and initializes the OpenThread task queue.
 *
 * @param[in] config     The platform configuration
 *
 * @return
 *   - ESP_OK on success
 *   - ESP_ERR_NO_MEM on queue allocation failure
 *   - ESP_FAIL on other failures
 *
 */
esp_err_t esp_openthread_task_queue_init(const esp_openthread_platform_config_t *config);

/**
 * @brief This function posts a task to the OpenThread task queue.
 *
 * @param[in] task   The task to execute.
 * @param[in] arg    The context argument to be passed to the task.
 *
 * @return
 *   - ESP_OK
 *   - ESP_FAIL
 *
 */
esp_err_t esp_openthread_task_queue_post(esp_openthread_task_t task, void *arg);

/**
 * @brief This function updates the task queue inner fd to the main loop.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_task_queue_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function drives the execution of the task queue.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
esp_err_t esp_openthread_task_queue_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function deinitializes the task queue.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
esp_err_t esp_openthread_task_queue_deinit(void);

#ifdef __cplusplus
}
#endif
