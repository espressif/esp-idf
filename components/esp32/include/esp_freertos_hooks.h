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
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 Definitions for the tickhook and idlehook callbacks
*/
typedef bool (*esp_freertos_idle_cb_t)();
typedef void (*esp_freertos_tick_cb_t)();

/**
  * @brief  Register a callback to be called on the freertos idle hook
  *         The callback should return true if it's okay for the core to
  *         sleep until an interrupt (or FreeRTOS tick) happens and false
  *         if it should be called again as fast as possible.
  *
  * @warning Idle callbacks MUST NOT, UNDER ANY CIRCUMSTANCES, CALL 
  *          A FUNCTION THAT MIGHT BLOCK.
  *
  * @param  esp_freertos_idle_cb_t new_idle_cb : Callback to be called
  *
  * @return ESP_OK : Callback registered
  * @return ESP_ERR_NO_MEM : No more space to register hook
  */
esp_err_t esp_register_freertos_idle_hook(esp_freertos_idle_cb_t new_idle_cb);

/**
  * @brief  Register a callback to be called on the freertos tick hook
  *
  * @param  esp_freertos_tick_cb_t new_tick_cb : Callback to be called
  *
  * @return ESP_OK : Callback registered
  * @return ESP_ERR_NO_MEM : No more space to register hook
  */
esp_err_t esp_register_freertos_tick_hook(esp_freertos_tick_cb_t tick_cb);


/**
  * @brief  Unregister an idle callback registered earlier
  *
  * @param  esp_freertos_idle_cb_t new_idle_cb : Callback to be unregistered
  *
  * @return void
  */
void esp_deregister_freertos_idle_hook(esp_freertos_idle_cb_t old_idle_cb);


/**
  * @brief  Unregister a tick callback registered earlier
  *
  * @param  esp_freertos_idle_cb_t new_idle_cb : Callback to be unregistered
  *
  * @return void
  */
void esp_deregister_freertos_tick_hook(esp_freertos_tick_cb_t old_tick_cb);

#ifdef __cplusplus
}
#endif


#endif