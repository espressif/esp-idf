/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_ping.h"

#include "lwip/ip_addr.h"
typedef struct _ping_option {
    ip_addr_t ping_target;
    uint32_t ping_count;
    uint32_t ping_rcv_timeout;
    uint32_t ping_delay;
    uint32_t interface;
    size_t ping_data_len;
    uint16_t ping_id;
    u8_t ping_tos;
    u8_t ping_ttl;
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
        ipaddr_aton(opt_val, &(ping_option_info->ping_target));
        break;
    case PING_TARGET_IP_ADDRESS_COUNT:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->ping_count = *(uint32_t *)opt_val;
        break;
    case PING_TARGET_IF_INDEX:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->interface = *(uint32_t *)opt_val;
        break;
    case PING_TARGET_RCV_TIMEO:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->ping_rcv_timeout = (*(uint32_t *)opt_val);
        break;
    case PING_TARGET_DELAY_TIME:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        ping_option_info->ping_delay = (*(uint32_t *)opt_val);
        break;
    case PING_TARGET_DATA_LEN:
        ESP_PING_CHECK_OPTLEN(opt_len, size_t);
        ping_option_info->ping_data_len = (*(size_t *)opt_val);
        break;
    case PING_TARGET_ID:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        ping_option_info->ping_id = *(uint16_t *)opt_val;
        break;
    case PING_TARGET_IP_TOS:
        ESP_PING_CHECK_OPTLEN(opt_len, u8_t);
        ping_option_info->ping_tos = *(u8_t *)opt_val;
        break;
    case PING_TARGET_RES_FN:
        ping_option_info->ping_res_fn = opt_val;
        break;
    case PING_TARGET_RES_RESET:
        memset(&ping_option_info->ping_res, 0, sizeof(ping_option_info->ping_res));
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
        ip_addr_copy(*(ip_addr_t*)opt_val, ping_option_info->ping_target);
        break;
    case PING_TARGET_IP_ADDRESS_COUNT:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->ping_count;
        break;
    case PING_TARGET_IF_INDEX:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->interface;
        break;
    case PING_TARGET_RCV_TIMEO:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->ping_rcv_timeout;
        break;
    case PING_TARGET_DELAY_TIME:
        ESP_PING_CHECK_OPTLEN(opt_len, uint32_t);
        *(uint32_t *)opt_val = ping_option_info->ping_delay;
        break;
    case PING_TARGET_DATA_LEN:
        ESP_PING_CHECK_OPTLEN(opt_len, size_t);
        *(size_t *)opt_val = ping_option_info->ping_data_len;
        break;
    case PING_TARGET_ID:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        *(uint16_t *)opt_val = ping_option_info->ping_id;
        break;
    case PING_TARGET_IP_TOS:
        ESP_PING_CHECK_OPTLEN(opt_len, uint16_t);
        *(uint16_t *)opt_val = ping_option_info->ping_tos;
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

    ping_option_info->ping_res.ping_err = res_val;

    if (res_val != PING_RES_FINISH) {
        ping_option_info->ping_res.bytes = ping_len;
        ping_option_info->ping_res.resp_time = ping_time;
        ping_option_info->ping_res.total_bytes += ping_len;
        ping_option_info->ping_res.send_count ++;

        if (res_val == PING_RES_TIMEOUT) {
            ping_option_info->ping_res.timeout_count ++;
        } else {
            if (!ping_option_info->ping_res.min_time || (ping_time < ping_option_info->ping_res.min_time)) {
                ping_option_info->ping_res.min_time = ping_time;
            }

            if (ping_time > ping_option_info->ping_res.max_time) {
                ping_option_info->ping_res.max_time = ping_time;
            }


            ping_option_info->ping_res.total_time += ping_time;
            ping_option_info->ping_res.recv_count ++;
        }
    }

    if (ping_option_info->ping_res_fn) {
        ping_option_info->ping_res_fn(PING_TARGET_RES_FN, &ping_option_info->ping_res);
        if (res_val == PING_RES_FINISH) {
            memset(&ping_option_info->ping_res, 0, sizeof(esp_ping_found));
        }
    }

    return ret;
}
