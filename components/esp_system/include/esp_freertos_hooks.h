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

#ifndef __ESP_FREERTOS_HOOKS_H__
#define __ESP_FREERTOS_HOOKS_H__

#include <stdbool.h>

#include "freertos/portmacro.h"

#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 Definitions for the tickhook and idlehook callbacks
*/
typedef bool (*esp_freertos_idle_cb_t)(void);
typedef void (*esp_freertos_tick_cb_t)(void);

/**
  * @brief  Register a callback to be called from the specified core's idle hook.
  *         The callback should return true if it should be called by the idle hook
  *         once per interrupt (or FreeRTOS tick), and return false if it should
  *         be called repeatedly as fast as possible by the idle hook.
  *
  * @warning Idle callbacks MUST NOT, UNDER ANY CIRCUMSTANCES, CALL
  *          A FUNCTION THAT MIGHT BLOCK.
  *
  * @param[in]  new_idle_cb     Callback to be called
  * @param[in]  cpuid           id of the core
  *
  * @return
  *     - ESP_OK:              Callback registered to the specified core's idle hook
  *     - ESP_ERR_NO_MEM:      No more space on the specified core's idle hook to register callback
  *     - ESP_ERR_INVALID_ARG: cpuid is invalid
  */
esp_err_t esp_register_freertos_idle_hook_for_cpu(esp_freertos_idle_cb_t new_idle_cb, UBaseType_t cpuid);

/**
  * @brief  Register a callback to the idle hook of the core that calls this function.
  *         The callback should return true if it should be called by the idle hook
  *         once per interrupt (or FreeRTOS tick), and return false if it should
  *         be called repeatedly as fast as possible by the idle hook.
  *
  * @warning Idle callbacks MUST NOT, UNDER ANY CIRCUMSTANCES, CALL
  *          A FUNCTION THAT MIGHT BLOCK.
  *
  * @param[in]  new_idle_cb     Callback to be called
  *
  * @return
  *     - ESP_OK:         Callback registered to the calling core's idle hook
  *     - ESP_ERR_NO_MEM: No more space on the calling core's idle hook to register callback
  */
esp_err_t esp_register_freertos_idle_hook(esp_freertos_idle_cb_t new_idle_cb);

/**
  * @brief  Register a callback to be called from the specified core's tick hook.
  *
  * @param[in]  new_tick_cb     Callback to be called
  * @param[in]  cpuid           id of the core
  *
  * @return
  *     - ESP_OK:              Callback registered to specified core's tick hook
  *     - ESP_ERR_NO_MEM:      No more space on the specified core's tick hook to register the callback
  *     - ESP_ERR_INVALID_ARG: cpuid is invalid
  */
esp_err_t esp_register_freertos_tick_hook_for_cpu(esp_freertos_tick_cb_t new_tick_cb, UBaseType_t cpuid);

/**
  * @brief  Register a callback to be called from the calling core's tick hook.
  *
  * @param[in]  new_tick_cb     Callback to be called
  *
  * @return
  *     - ESP_OK:         Callback registered to the calling core's tick hook
  *     - ESP_ERR_NO_MEM: No more space on the calling core's tick hook to register the callback
  */
esp_err_t esp_register_freertos_tick_hook(esp_freertos_tick_cb_t new_tick_cb);

/**
  * @brief  Unregister an idle callback from the idle hook of the specified core
  *
  * @param[in]  old_idle_cb     Callback to be unregistered
  * @param[in]  cpuid           id of the core
  */
void esp_deregister_freertos_idle_hook_for_cpu(esp_freertos_idle_cb_t old_idle_cb, UBaseType_t cpuid);

/**
  * @brief  Unregister an idle callback. If the idle callback is registered to
  *         the idle hooks of both cores, the idle hook will be unregistered from
  *         both cores
  *
  * @param[in]  old_idle_cb     Callback to be unregistered
  */
void esp_deregister_freertos_idle_hook(esp_freertos_idle_cb_t old_idle_cb);

/**
  * @brief  Unregister a tick callback from the tick hook of the specified core
  *
  * @param[in]  old_tick_cb     Callback to be unregistered
  * @param[in]  cpuid           id of the core
  */
void esp_deregister_freertos_tick_hook_for_cpu(esp_freertos_tick_cb_t old_tick_cb, UBaseType_t cpuid);

/**
  * @brief  Unregister a tick callback. If the tick callback is registered to the
  *         tick hooks of both cores, the tick hook will be unregistered from
  *         both cores
  *
  * @param[in]  old_tick_cb     Callback to be unregistered
  */
void esp_deregister_freertos_tick_hook(esp_freertos_tick_cb_t old_tick_cb);

#ifdef __cplusplus
}
#endif


#endif
