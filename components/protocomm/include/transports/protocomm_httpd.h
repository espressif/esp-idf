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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Config parameters for protocomm HTTP server
 */
typedef struct {

    uint16_t port;          /*!< Port on which the HTTP server will listen */

    /**
     * Stack size of server task, adjusted depending
     * upon stack usage of endpoint handler
     */
    size_t   stack_size;
    unsigned task_priority; /*!< Priority of server task */
} protocomm_http_server_config_t; /*!< HTTP Server Configuration, if HTTP Server has not been started already */

/** Protocomm HTTPD Configuration Data
 */
typedef union {
    /** HTTP Server Handle, if ext_handle_provided is set to true
     */
    void *handle;

    /** HTTP Server Configuration, if a server is not already active
     */
    protocomm_http_server_config_t config;
} protocomm_httpd_config_data_t;

/**
 * @brief   Config parameters for protocomm HTTP server
 */
typedef struct {
    /** Flag to indicate of an external HTTP Server Handle has been provided.
     * In such as case, protocomm will use the same HTTP Server and not start
     * a new one internally.
     */
    bool ext_handle_provided;
    /** Protocomm HTTPD Configuration Data */
    protocomm_httpd_config_data_t data;
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
 * @param[in] config    Pointer to config structure for initializing HTTP server
 *
 * @return
 *  - ESP_OK : Success
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
 *  - ESP_OK : Success
 *  - ESP_ERR_INVALID_ARG : Null / incorrect protocomm instance pointer
 */
esp_err_t protocomm_httpd_stop(protocomm_t *pc);

#ifdef __cplusplus
}
#endif
