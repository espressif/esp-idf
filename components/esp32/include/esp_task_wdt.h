// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_TASK_WDT_H
#define __ESP_TASK_WDT_H

#include "freertos/task.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
This routine enables a more general-purpose task watchdog which uses the TIMERG0
WDT: All tasks subscribed to the task wdt must feed the wdt at least once
before the wdt times out or else the wdt will bark. The Idle tasks can also be
subscribed to the wdt. This allows for the detection of CPU starvation.

To enable/disable legacy behavior, toggle the CONFIG_TASK_WDT_LEGACY_BEHAVIOR
option in menuconfig. Under legacy behavior, a task will subscribe to the
wdt on its first call of esp_task_wdt_feed(). esp_task_wdt_add() SHOULD NOT BE
CALLED in an application if legacy behavior is enabled. esp_task_wdt_delete()
will only allow tasks to unsubscribe themselves from the task wdt under legacy
behavior. Enabling legacy behavior will also force the task wdt to be enabled on
startup.
*/

/**
  * @brief  Initialize the task watchdog timer
  *
  * This function will initialize the task watchdog timer. If the wdt
  * has already been initialized, calling this function again will change the
  * timeout and panic configuration, then reset the timer count. Once the watchdog
  * timer has been initialized, tasks can subscribe to the watchdog timer.
  *
  * @param[in]  timeout     Timeout(ms) period of watchdog timer
  * @param[in]  panic       Flag that if set, will cause the wdt to trigger the
  *                         panic handler when it times out
  * @return
  *     - ESP_OK:           Initialization was successful
  *     - ESP_ERR_NO_MEM:   Initialization was unsuccessful due to lack of
  *                         memory
  *
  * @note   esp_task_wdt_init() must only be called after the scheduler
  *         started
  */
esp_err_t esp_task_wdt_init(uint32_t timeout, bool panic);


/**
 * @brief   Deinitialize the task watchdog timer
 *
 * This function will deinitialize the task watchdog timer. This function should
 * be called when all tasks have unsubscribed form the watchdog timer.
 *
 * @return
 *      - ESP_OK:                 Watchdog timer successfully deinitialized
 *      - ESP_ERR_INVALID_STATE:  Error due to wdt not being initialized, or
 *                                if tasks are still subscribed to the wdt
 */
esp_err_t esp_task_wdt_deinit();


/**
  * @brief  Subscribes a tasks to the task watchdog timer
  *
  * This function subscribes a task to the task watchdog timer. Note that once
  * subscribed, a task must periodically feed the watchdog timer. If the task
  * is an IDLE task, this function will enable feeding from the idle hook
  * automatically.
  *
  * @param[in]  handle            Handle of the task to be added. Input NULL
  *                               to add the current running task
  *
  * @return
  *     - ESP_OK:                 Successfully subscribed the task to the wdt
  *     - ESP_ERR_INVALID_ARG:    Failed as task is already subscribed
  *     - ESP_ERR_INVALID_STATE:  Failed as wdt has not been initialized
  *     - ESP_ERR_NO_MEM:         Failed due to lack of memory
  *
  * @note   This function should not be called from application code if
  *         CONFIG_TASK_WDT_LEGACY_BEHAVIOR has been enabled. Legacy behavior
  *         uses esp_task_wdt_feed() to subscribe tasks to the wdt.
  */
esp_err_t esp_task_wdt_add(TaskHandle_t handle);

/**
  * @brief  Feed the current running task
  *
  * This function will feed the current running task if it has subscribed to the
  * task watchdog timer. All non-idle tasks subscribed to the wdt must call this
  * function at least once per watchdog timeout period. Idle tasks will feed
  * the task watchdog automatically from their idle hooks.
  *
  * @return
  *     - ESP_OK:                 Successfully fed the current running task
  *     - ESP_ERR_INVALID_STATE:  Failed to feed current running task as the
  *                               wdt has not been initialized, or the
  *                               current running task has not subscribed
  *
  * @note   If CONFIG_TASK_WDT_LEGACY_BEHAVIOR is enabled, tasks will subscribe
  *         to the watchdog on their first call of esp_task_wdt_feed().
  */
esp_err_t esp_task_wdt_feed();

/**
  * @brief  Unsubscribes a task from the task watchdog timer
  *
  * This function unsubscribes a task from the task watchdog. After unsubscribing,
  * the task should no longer feed the wdt. If the unsubscribing task is an idle
  * task, this function will disable feeding from the idle hook automatically.
  *
  * @param[in]  handle              Handle of the task to be deleted. Input NULL
  *                                 if deleting the current running task.
  *
  * @return
  *     - ESP_OK:                 Successfully unsubscribed task form the wdt
  *     - ESP_ERR_INVALID_ARG:    Failed to unsubscribe task as it was not
  *                               subscribed to begin with
  *     - ESP_ERR_INVALID_STATE:  Failed to unsubscribe task as wdt has not
  *                               been initialized yet
  *
  * @note   Legacy behavior only allows tasks to unsubscribe from the wdt on their
  *         own behalf. Therefore, if CONFIG_TASK_WDT_LEGACY_BEHAVIOR is
  *         enabled, this esp_task_wdt_delete() will accept no parameters and
  *         unsubscribe the calling task from the wdt.
  */
#ifdef CONFIG_TASK_WDT_LEGACY_BEHAVIOR
esp_err_t esp_task_wdt_delete();
#else
esp_err_t esp_task_wdt_delete(TaskHandle_t handle);
#endif      //CONFIG_TASK_WDT_LEGACY_BEHAVIOR

#ifdef __cplusplus
}
#endif

#endif      //__ESP_TASK_WDT_H
