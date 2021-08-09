// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef ESP_EVENT_PRIVATE_H_
#define ESP_EVENT_PRIVATE_H_

#include <stdbool.h>
#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Searches handlers registered with an event loop to see if it has been registered.
 *
 * @param[in] event_loop the loop to search
 * @param[in] event_base the event base to search
 * @param[in] event_id the event id to search
 * @param[in] event_handler the event handler to look for
 *
 * @return true handler registered
 * @return false handler not registered
 *
 * @return
 *  - true: Handler registered
 *  - false:  Handler not registered
 */
bool esp_event_is_handler_registered(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler);

/**
 * @brief Deinitializes the event loop library
 *
 * @return
 *  - ESP_OK: Success
 *  - Others: Fail
 */
esp_err_t esp_event_loop_deinit(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef ESP_EVENT_PRIVATE_H_
