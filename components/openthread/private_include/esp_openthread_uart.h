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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Initializes an uart port with the given config.
 *
 * @note   The user still needs to open the file descriptor by self.
 *
 * @param[in]   config  The uart configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_uart_init_port(const esp_openthread_uart_config_t *config);

/**
 * @brief  Initializes the uart for OpenThread host connection.
 *
 * @param[in]   config  The uart configuration.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_uart_init(const esp_openthread_platform_config_t *config);

/**
 * @brief  Deintializes the uart for OpenThread host connection.
 *
 */
void esp_openthread_uart_deinit(void);

/**
 * @brief  Deintializes the uart for OpenThread host connection.
 *
 * @param[inout]    mainloop    The main loop context.
 *
 */
void esp_openthread_uart_update(esp_openthread_mainloop_context_t *context);

/**
 * @brief  Performs the uart I/O for OpenThread.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERROR on failure
 *
 */
esp_err_t esp_openthread_uart_process(void);

#ifdef __cplusplus
}
#endif
