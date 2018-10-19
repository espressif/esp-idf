// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef ESP_PING_H_
#define ESP_PING_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// gen_esp_err_to_name.py: include this as "esp_ping.h" because "components/lwip/include/apps/" is in the compiler path
// and not "components/lwip/include"

#define ESP_ERR_PING_BASE               0x6000

#define ESP_ERR_PING_INVALID_PARAMS     ESP_ERR_PING_BASE + 0x01
#define ESP_ERR_PING_NO_MEM             ESP_ERR_PING_BASE + 0x02

#define ESP_PING_CHECK_OPTLEN(optlen, opttype) do { if ((optlen) < sizeof(opttype)) { return ESP_ERR_PING_INVALID_PARAMS; }}while(0)

typedef struct _ping_found {
    uint32_t resp_time;
    uint32_t timeout_count;
    uint32_t send_count;
    uint32_t recv_count;
    uint32_t err_count;
    uint32_t bytes;
    uint32_t total_bytes;
    uint32_t total_time;
    uint32_t min_time;
    uint32_t max_time;
    int8_t  ping_err;
} esp_ping_found;

typedef enum {
    PING_TARGET_IP_ADDRESS          = 50,   /**< target IP address */
    PING_TARGET_IP_ADDRESS_COUNT    = 51,   /**< target IP address total counter */
    PING_TARGET_RCV_TIMEO           = 52,   /**< receive timeout in milliseconds */
    PING_TARGET_DELAY_TIME          = 53,   /**< delay time in milliseconds */
    PING_TARGET_ID                  = 54,   /**< identifier */
    PING_TARGET_RES_FN              = 55,   /**< ping result callback function */
    PING_TARGET_RES_RESET           = 56,   /**< ping result statistic reset */
    PING_TARGET_DATA_LEN            = 57,   /**< ping data length*/
    PING_TARGET_IP_TOS              = 58    /**< ping QOS*/
} ping_target_id_t;

typedef enum {
    PING_RES_TIMEOUT = 0,
    PING_RES_OK      = 1, 
    PING_RES_FINISH  = 2,
} ping_res_t;

typedef void (* esp_ping_found_fn)(ping_target_id_t found_id, esp_ping_found *found_val);

/**
 * @brief  Set PING function option
 *
 * @param[in]  opt_id: option index, 50 for IP, 51 for COUNT, 52 for RCV TIMEOUT, 53 for DELAY TIME, 54 for ID
 * @param[in]  opt_val: option parameter
 * @param[in]  opt_len: option length
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_PING_INVALID_PARAMS
 */
esp_err_t esp_ping_set_target(ping_target_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief  Get PING function option
 *
 * @param[in]  opt_id: option index, 50 for IP, 51 for COUNT, 52 for RCV TIMEOUT, 53 for DELAY TIME, 54 for ID
 * @param[in]  opt_val: option parameter
 * @param[in]  opt_len: option length
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_PING_INVALID_PARAMS
 */
esp_err_t esp_ping_get_target(ping_target_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief  Get PING function result action
 *
 * @param[in]  res_val: ping function action, 1 for successful, 0 for fail.
 *             res_len: response bytes
 *             res_time: response time
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_PING_INVALID_PARAMS
 */
esp_err_t esp_ping_result(uint8_t res_val, uint16_t res_len, uint32_t res_time);

#ifdef __cplusplus
}
#endif

#endif /* ESP_PING_H_ */
