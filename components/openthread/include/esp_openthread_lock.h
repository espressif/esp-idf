// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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
// limitations under the License

#pragma once

#include <stdbool.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread API lock.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *      - ESP_ERR_INVALID_STATE if already initialized
 *
 */
esp_err_t esp_openthread_lock_init(void);

/**
 * This function deinitializes the OpenThread API lock.
 *
 */
void esp_openthread_lock_deinit(void);

/**
 * @brief This functions acquires the OpenThread API lock.
 *
 * @note Every OT APIs that takes an otInstance argument MUST be protected with this API lock
 *       except that the call site is in OT callbacks.
 *
 * @param[in] block_ticks   The maxinum number of RTOS ticks to wait for the lock.
 *
 * @return
 *      - True on lock acquired
 *      - False on failing to acquire the lock with the timeout.
 *
 */
bool esp_openthread_lock_acquire(TickType_t block_ticks);

/**
 * @brief This function releases the OpenThread API lock.
 *
 */
void esp_openthread_lock_release(void);


#ifdef __cplusplus
}
#endif
