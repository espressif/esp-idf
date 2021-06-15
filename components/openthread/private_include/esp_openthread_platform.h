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

#include "esp_err.h"
#include "esp_openthread_types.h"
#include "openthread/error.h"
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initializes the platform-specific support for the OpenThread stack.
 *
 * @note This function is not called by and will not call the OpenThread library.
 *       The user needs to call otInstanceInitSingle to intialize the OpenThread
 *       stack after calling this function.
 *
 * @param[in]  init_config      The initialization configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NO_MEM if allocation has failed
 *      - ESP_ERR_INVALID_ARG if radio or host connection mode not supported
 *      - ESP_ERR_INVALID_STATE if already initialized
 *
 */
esp_err_t esp_openthread_platform_init(const esp_openthread_platform_config_t *init_config);

/**
 * This function performs all platform-specific deinitialization for OpenThread's drivers.
 *
 * @note This function is not called by the OpenThread library. Instead, the user should
 *       call this function when deinitialization of OpenThread's drivers is most appropriate.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if not initialized
 *
 */
esp_err_t esp_openthread_platform_deinit(void);

/**
 * @brief This function updates the platform fds and timeouts
 *
 * @note This function will not update the OpenThread core stack pending events.
 *       The users need to call `otTaskletsArePending` to check whether there being
 *       pending OpenThread tasks.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_platform_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function performs the OpenThread related platform process (radio, uart, alarm etc.)
 *
 * @note This function will call the OpenThread core stack process functions.
 *       The users need to call `otTaskletsProcess` by self.
 *
 * @param[in]    instance   The OpenThread instance.
 * @param[in]    mainloop   The main loop context.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on failure
 *
 */
esp_err_t esp_openthread_platform_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

#ifdef __cplusplus
} // end of extern "C"
#endif
