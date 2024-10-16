/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once
#include "esp_pm.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/* Set the length of the event queue */
#define DEFAULT_EVENT_QUEUE_LEN             (CONFIG_EXAMPLE_EVENT_QUEUE_LEN)

/* Set the task's stack size */
#define DEFAULT_EVENT_TASK_STACK_SIZE       (CONFIG_EXAMPLE_EVENT_TASK_STACK_SIZE)

/* Set the priority of a task */
#define DEFAULT_EVENT_TASK_PRIORITY         (CONFIG_EXAMPLE_EVENT_TASK_PRIORITY)

/* Type of pm lock configured by menuconfig */
#if (CONFIG_EXAMPLE_PM_LOCK_TYPE == 0)
#define DEFAULT_PM_LOCK_TYPE                (ESP_PM_CPU_FREQ_MAX)
#elif (CONFIG_EXAMPLE_PM_LOCK_TYPE == 1)
#define DEFAULT_PM_LOCK_TYPE                (ESP_PM_APB_FREQ_MAX)
#else
#define DEFAULT_PM_LOCK_TYPE                (ESP_PM_NO_LIGHT_SLEEP)
#endif

/* System hold lock state */
#define RELEASED_LOCK                       (false)
#define ACQUIRED_LOCK                       (true)


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t example_register_power_config(esp_pm_lock_handle_t *pm_lock, bool *lock_state);

#ifdef __cplusplus
}
#endif