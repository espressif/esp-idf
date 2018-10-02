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

#define PROTOCOMM_HTTPD_DEFAULT_CONFIG() {   \
    .port           = 80,                    \
    .stack_size     = 4096,                  \
    .task_priority  = tskIDLE_PRIORITY + 5,  \
}

/**
 * @brief   Config parameters for protocomm HTTP server
 */
typedef struct {
    uint16_t port;          /*!< Port on which the http server will listen */

    /**
     * Stack size of server task, adjusted depending
     * upon stack usage of endpoint handler
     */
    size_t   stack_size;
    unsigned task_priority; /*!< Priority of server task */
} protocomm_httpd_config_t;

/**
 * @brief   Start HTTPD protocomm transport
 *
 * This API internally creates a framework to allow endpoint registration and security
 * configuration for the protocomm.
 *
 * @note    This is a singleton. ie. Protocomm can have multiple instances, but only
 *          one instance can be bound to an HTTP transport layer.
 *
 * @param[in] pc        Protocomm instance pointer obtained from protocomm_new()
 * @param[in] config    Pointer to config structure for initialising HTTP server
 *
 * @return
 *  - ESP_OK : Server started succefully
 *  - ESP_ERR_INVALID_ARG : Null arguments
 *  - ESP_ERR_NOT_SUPPORTED : Transport layer bound to another protocomm instance
 *  - ESP_ERR_INVALID_STATE : Transport layer already bound to this protocomm instance
 *  - ESP_ERR_NO_MEM : Memory allocation for server resource failed
 *  - ESP_ERR_HTTPD_* : HTTP server error on start
 */
esp_err_t protocomm_httpd_start(protocomm_t *pc, const protocomm_httpd_config_t *config);

/**
 * @brief   Stop HTTPD protocomm transport
 *
 * This API cleans up the HTTPD transport protocomm and frees all the handlers registered
 * with the protocomm.
 *
 * @param[in] pc    Same protocomm instance that was passed to protocomm_httpd_start()
 *
 * @return
 *  - ESP_OK : Server stopped succefully
 *  - ESP_ERR_INVALID_ARG : Null / incorrect protocomm instance pointer
 */
esp_err_t protocomm_httpd_stop(protocomm_t *pc);
