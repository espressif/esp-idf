// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <protocomm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROTOCOMM_CONSOLE_DEFAULT_CONFIG() { \
    .stack_size     = 4096,                  \
    .task_priority  = tskIDLE_PRIORITY + 3,  \
}

/**
 * @brief   Config parameters for protocomm console
 */
typedef struct {
    size_t   stack_size;        /*!< Stack size of console task */
    unsigned task_priority;     /*!< Priority of console task */
} protocomm_console_config_t;

/**
 * @brief   Start console based protocomm transport
 *
 * @note    This is a singleton. ie. Protocomm can have multiple instances, but only
 *          one instance can be bound to a console based transport layer.
 *
 * @param[in] pc     Protocomm instance pointer obtained from protocomm_new()
 * @param[in] config Config param structure for protocomm console
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_NOT_SUPPORTED : Transport layer bound to another protocomm instance
 *  - ESP_ERR_INVALID_STATE : Transport layer already bound to this protocomm instance
 *  - ESP_FAIL : Failed to start console thread
 */
esp_err_t protocomm_console_start(protocomm_t *pc, const protocomm_console_config_t *config);

/**
 * @brief   Stop console protocomm transport
 *
 * @param[in] pc    Same protocomm instance that was passed to protocomm_console_start()
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_ERR_INVALID_ARG : Null / incorrect protocomm instance pointer
 */
esp_err_t protocomm_console_stop(protocomm_t *pc);

#ifdef __cplusplus
}
#endif
