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
#include "openthread/instance.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function initializes the OpenThread network interface glue.
 *
 * @return
 *      - glue pointer on success
 *      - NULL on failure
 *
 */
void *esp_openthread_netif_glue_init(void);

/**
 * @brief This function deinitializes the OpenThread network interface glue.
 *
 */
void esp_openthread_netif_glue_deinit(void);

/**
 * @brief This function updates the netif fds and timeouts to the main loop.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_netif_glue_update(esp_openthread_mainloop_context_t *mainloop);

/**
 * @brief This function performs the netif process.
 *
 * @param[in]    instance   The OpenThread instance.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL on OpenThread failure
 *      - ESP_ERR_NO_MEM on memory allocation failure
 *
 */
esp_err_t esp_openthread_netif_glue_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop);

#ifdef __cplusplus
}
#endif
