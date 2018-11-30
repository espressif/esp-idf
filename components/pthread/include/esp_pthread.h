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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN    CONFIG_PTHREAD_STACK_MIN
#endif

/** pthread configuration structure that influences pthread creation */
typedef struct {
    size_t stack_size;    ///< the stack size of the pthread
    size_t prio;          ///< the thread's priority
    bool inherit_cfg;     ///< inherit this configuration further
} esp_pthread_cfg_t;

/**
 * @brief Configure parameters for creating pthread
 *
 * This API allows you to configure how the subsequent
 * pthread_create() call will behave. This call can be used to setup
 * configuration parameters like stack size, priority, configuration
 * inheritance etc.
 *
 * If the 'inherit' flag in the configuration structure is enabled,
 * then the same configuration is also inherited in the thread
 * subtree.
 *
 * @note Passing non-NULL attributes to pthread_create() will override
 *       the stack_size parameter set using this API
 *
 * @param cfg The pthread config parameters
 *
 * @return
 *      - ESP_OK if configuration was successfully set
 *      - ESP_ERR_NO_MEM if out of memory
 *      - ESP_ERR_INVALID_ARG if stack_size is less than PTHREAD_STACK_MIN
 */
esp_err_t esp_pthread_set_cfg(const esp_pthread_cfg_t *cfg);

/**
 * @brief Get current pthread creation configuration
 *
 * This will retrieve the current configuration that will be used for
 * creating threads.
 *
 * @param p Pointer to the pthread config structure that will be
 * updated with the currently configured parameters
 *
 * @return
 *      - ESP_OK if the configuration was available
 *      - ESP_ERR_NOT_FOUND if a configuration wasn't previously set
 */
esp_err_t esp_pthread_get_cfg(esp_pthread_cfg_t *p);

#ifdef __cplusplus
}
#endif
