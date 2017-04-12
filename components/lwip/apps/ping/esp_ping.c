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

#include <string.h>
#include "esp_ping.h"

#include "lwip/ip_addr.h"

typedef struct _ping_option {
    ip4_addr_t ping_target;
    uint32_t ping_count;
    uint32_t ping_rcv_timeout;
    uint32_t ping_delay;
    uint16_t ping_id;
    esp_ping_found_fn ping_res_fn;
    esp_ping_found    ping_res;
    void    *ping_reserve;
} ping_option;

static ping_option ping_option_info[1];

esp_err_t esp_ping_set_target(ping_target_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    esp_err_t ret = ESP_OK;

    if (opt_val == NULL) {
        return ESP_ERR_PING_INVALID_PARAMS;
    }

    switch (opt_id) {
    case PING_TARGET_IP_ADDRESS:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->ping_target.addr = *(uint32_t *)opt_val;
        break;
    case PING_TARGET_IP_ADDRESS_COUNT:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->ping_count = *(uint32_t *)opt_val;
        break;
    case PING_TARGET_RCV_TIMEO:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->ping_rcv_timeout = (*(uint32_t *)opt_val) * 1000;
        break;
    case PING_TARGET_DELAY_TIME:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->ping_delay = (*(uint32_t *)opt_val) * 1000;
        break;
    case PING_TARGET_ID:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        ping_option_info->ping_id = *(uint16_t *)opt_val;
        break;
    case PING_TARGET_RES_FN:
        ping_option_info->ping_res_fn = opt_val;
        break;
    default:
        ret = ESP_ERR_PING_INVALID_PARAMS;
        break;
    }

    return ret;
}

esp_err_t esp_ping_get_target(ping_target_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    esp_err_t ret = ESP_OK;

    if (opt_val == NULL) {
        return ESP_ERR_PING_INVALID_PARAMS;
    }

    switch (opt_id) {
    case PING_TARGET_IP_ADDRESS:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->ping_target.addr;
        break;
    case PING_TARGET_IP_ADDRESS_COUNT:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->ping_count;
        break;
    case PING_TARGET_RCV_TIMEO:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->ping_rcv_timeout;
        break;
    case PING_TARGET_DELAY_TIME:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->ping_delay;
        break;
    case PING_TARGET_ID:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        *(uint16_t *)opt_val = ping_option_info->ping_id;
        break;
    default:
        ret = ESP_ERR_PING_INVALID_PARAMS;
        break;
    }

    return ret;
}

esp_err_t esp_ping_result(uint8_t res_val, uint16_t ping_len, uint32_t ping_time)
{
    esp_err_t ret = ESP_OK;

    ping_option_info->ping_res.bytes = ping_len;
    ping_option_info->ping_res.ping_err = res_val;
    ping_option_info->ping_res.resp_time = ping_time;
    ping_option_info->ping_res.total_time += ping_time;
    ping_option_info->ping_res.total_bytes += ping_len;

    if (res_val == 0) {
        ping_option_info->ping_res.timeout_count ++;
    }

    if (--ping_option_info->ping_count != 0 && ping_option_info->ping_res_fn) {
        ping_option_info->ping_res_fn(PING_TARGET_RES_FN, &ping_option_info->ping_res);
    } else {
        memset(&ping_option_info->ping_res, 0, sizeof(esp_ping_found));
    }

    return ret;
}
