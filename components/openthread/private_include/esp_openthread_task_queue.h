/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
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
 * @brief OpenThread task queue type.
 *
 */
typedef enum {
    ESP_OPENTHREAD_TASK_QUEUE_TYPE_NORMAL,   /*!< Tasks that can be dropped when the queue is full */
    ESP_OPENTHREAD_TASK_QUEUE_TYPE_PRIO,     /*!< Tasks that must not be dropped */
} esp_openthread_task_queue_type_t;

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
 * @brief This function posts a task to an OpenThread task queue.
 *
 * Normal tasks are dropped when the queue is full.
 * Priority tasks must not be dropped and will assert if the queue is full.
 * Increase CONFIG_OPENTHREAD_PRIORITY_TASK_QUEUE_SIZE as needed, or use
 * a normal task if the task can be safely dropped.
 *
 * @param[in] task   The task to execute.
 * @param[in] arg    The context argument to be passed to the task.
 * @param[in] type   The task queue type.
 *
 * @return
 *   - ESP_OK
 *   - ESP_FAIL
 *   - ESP_ERR_INVALID_ARG if @p type is invalid
 *
 */
esp_err_t esp_openthread_task_queue_post(esp_openthread_task_t task, void *arg,
                                         esp_openthread_task_queue_type_t type);

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
