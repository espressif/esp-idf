// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp_err.h"
#include "lwip/ip_addr.h"

/**
* @brief Type of "ping" session handle
*
*/
typedef void *esp_ping_handle_t;

/**
* @brief Type of "ping" callback functions
*
*/
typedef struct {
    /**
    * @brief arguments for callback functions
    *
    */
    void *cb_args;

    /**
    * @brief Invoked by internal ping thread when received ICMP echo reply packet
    *
    */
    void (*on_ping_success)(esp_ping_handle_t hdl, void *args);

    /**
    * @brief Invoked by internal ping thread when receive ICMP echo reply packet timeout
    *
    */
    void (*on_ping_timeout)(esp_ping_handle_t hdl, void *args);

    /**
    * @brief Invoked by internal ping thread when a ping session is finished
    *
    */
    void (*on_ping_end)(esp_ping_handle_t hdl, void *args);
} esp_ping_callbacks_t;

/**
* @brief Type of "ping" configuration
*
*/
typedef struct {
    uint32_t count;           /*!< A "ping" session contains count procedures */
    uint32_t interval_ms;     /*!< Milliseconds between each ping procedure */
    uint32_t timeout_ms;      /*!< Timeout value (in milliseconds) of each ping procedure */
    uint32_t data_size;       /*!< Size of the data next to ICMP packet header */
    uint8_t tos;              /*!< Type of Service, a field specified in the IP header */
    ip_addr_t target_addr;    /*!< Target IP address, either IPv4 or IPv6 */
    uint32_t task_stack_size; /*!< Stack size of internal ping task */
    uint32_t task_prio;       /*!< Priority of internal ping task */
    uint32_t interface;       /*!< Netif index, interface=0 means NETIF_NO_INDEX*/
} esp_ping_config_t;

/**
 * @brief Default ping configuration
 *
 */
#define ESP_PING_DEFAULT_CONFIG()        \
    {                                    \
        .count = 5,                      \
        .interval_ms = 1000,             \
        .timeout_ms = 1000,              \
        .data_size = 64,                 \
        .tos = 0,                        \
        .target_addr = *(IP_ANY_TYPE),   \
        .task_stack_size = 2048,         \
        .task_prio = 2,                  \
        .interface = 0,\
    }

#define ESP_PING_COUNT_INFINITE (0) /*!< Set ping count to zero will ping target infinitely */

/**
* @brief Profile of ping session
*
*/
typedef enum {
    ESP_PING_PROF_SEQNO,   /*!< Sequence number of a ping procedure */
    ESP_PING_PROF_TTL,     /*!< Time to live of a ping procedure */
    ESP_PING_PROF_REQUEST, /*!< Number of request packets sent out */
    ESP_PING_PROF_REPLY,   /*!< Number of reply packets received */
    ESP_PING_PROF_IPADDR,  /*!< IP address of replied target */
    ESP_PING_PROF_SIZE,    /*!< Size of received packet */
    ESP_PING_PROF_TIMEGAP, /*!< Elapsed time between request and reply packet */
    ESP_PING_PROF_DURATION /*!< Elapsed time of the whole ping session */
} esp_ping_profile_t;

/**
 * @brief Create a ping session
 *
 * @param config ping configuration
 * @param cbs a bunch of callback functions invoked by internal ping task
 * @param hdl_out handle of ping session
 * @return
 *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. configuration is null, etc)
 *      - ESP_ERR_NO_MEM: out of memory
 *      - ESP_FAIL: other internal error (e.g. socket error)
 *      - ESP_OK: create ping session successfully, user can take the ping handle to do follow-on jobs
 */
esp_err_t esp_ping_new_session(const esp_ping_config_t *config, const esp_ping_callbacks_t *cbs, esp_ping_handle_t *hdl_out);

/**
 * @brief Delete a ping session
 *
 * @param hdl handle of ping session
 * @return
 *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
 *      - ESP_OK: delete ping session successfully
 */
esp_err_t esp_ping_delete_session(esp_ping_handle_t hdl);

/**
 * @brief Start the ping session
 *
 * @param hdl handle of ping session
 * @return
 *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
 *      - ESP_OK: start ping session successfully
 */
esp_err_t esp_ping_start(esp_ping_handle_t hdl);

/**
 * @brief Stop the ping session
 *
 * @param hdl handle of ping session
 * @return
 *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
 *      - ESP_OK: stop ping session successfully
 */
esp_err_t esp_ping_stop(esp_ping_handle_t hdl);

/**
 * @brief Get runtime profile of ping session
 *
 * @param hdl handle of ping session
 * @param profile type of profile
 * @param data profile data
 * @param size profile data size
 * @return
 *      - ESP_ERR_INVALID_ARG: invalid parameters (e.g. ping handle is null, etc)
 *      - ESP_ERR_INVALID_SIZE: the actual profile data size doesn't match the "size" parameter
 *      - ESP_OK: get profile successfully
 */
esp_err_t esp_ping_get_profile(esp_ping_handle_t hdl, esp_ping_profile_t profile, void *data, uint32_t size);

#ifdef __cplusplus
}
#endif
