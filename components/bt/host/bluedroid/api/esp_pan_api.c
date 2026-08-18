/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_pan.h"
#include "esp_pan_api.h"
#include "common/bt_target.h"

#if (defined BTC_PAN_INCLUDED && BTC_PAN_INCLUDED == TRUE)

_Static_assert(ESP_PAN_MAX_WRITE_LEN == PAN_MAX_WRITE_LEN,
               "ESP_PAN_MAX_WRITE_LEN must match PAN_MAX_WRITE_LEN");
_Static_assert(ESP_PAN_MAX_PROTOCOL_FILTERS == BNEP_MAX_PROT_FILTERS,
               "ESP_PAN_MAX_PROTOCOL_FILTERS must match BNEP_MAX_PROT_FILTERS");
_Static_assert(ESP_PAN_MAX_MULTICAST_FILTERS == BNEP_MAX_MULTI_FILTERS,
               "ESP_PAN_MAX_MULTICAST_FILTERS must match BNEP_MAX_MULTI_FILTERS");

esp_err_t esp_pan_register_callback(esp_pan_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    return (btc_profile_cb_set(BTC_PID_PAN, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_pan_init(const esp_pan_cfg_t *cfg)
{
    btc_msg_t msg = {0};
    btc_pan_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (cfg == NULL || cfg->role == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PAN;
    msg.act = BTC_PAN_ACT_INIT;

    arg.init.cfg = *cfg;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_pan_args_t),
                                 btc_pan_arg_deep_copy, btc_pan_arg_deep_free) == BT_STATUS_SUCCESS ?
            ESP_OK : ESP_FAIL);
}

esp_err_t esp_pan_deinit(void)
{
    btc_msg_t msg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PAN;
    msg.act = BTC_PAN_ACT_DEINIT;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_pan_connect(esp_bd_addr_t remote_bda, esp_pan_role_t local_role, esp_pan_role_t peer_role)
{
    btc_msg_t msg = {0};
    btc_pan_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (remote_bda == NULL ||
            (local_role != ESP_PAN_ROLE_PANU && local_role != ESP_PAN_ROLE_GN &&
             local_role != ESP_PAN_ROLE_NAP) ||
            (peer_role != ESP_PAN_ROLE_PANU && peer_role != ESP_PAN_ROLE_GN &&
             peer_role != ESP_PAN_ROLE_NAP)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PAN;
    msg.act = BTC_PAN_ACT_CONNECT;

    memcpy(arg.connect.bd_addr, remote_bda, ESP_BD_ADDR_LEN);
    arg.connect.local_role = local_role;
    arg.connect.peer_role = peer_role;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_pan_args_t), NULL, NULL) == BT_STATUS_SUCCESS ?
            ESP_OK : ESP_FAIL);
}

esp_err_t esp_pan_disconnect(uint16_t handle)
{
    btc_msg_t msg = {0};
    btc_pan_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (handle == ESP_PAN_INVALID_HANDLE) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PAN;
    msg.act = BTC_PAN_ACT_DISCONNECT;

    arg.disconnect.handle = handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_pan_args_t), NULL, NULL) == BT_STATUS_SUCCESS ?
            ESP_OK : ESP_FAIL);
}

esp_err_t esp_pan_write(uint16_t handle, const uint8_t *dst, const uint8_t *src, uint16_t protocol,
                        uint16_t len, const uint8_t *data, bool ext)
{
    btc_msg_t msg = {0};
    btc_pan_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (handle == ESP_PAN_INVALID_HANDLE || dst == NULL || src == NULL || data == NULL ||
            len == 0 || len > ESP_PAN_MAX_WRITE_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PAN;
    msg.act = BTC_PAN_ACT_WRITE;

    arg.write.handle = handle;
    memcpy(arg.write.dst, dst, ESP_BD_ADDR_LEN);
    memcpy(arg.write.src, src, ESP_BD_ADDR_LEN);
    arg.write.protocol = protocol;
    arg.write.len = len;
    arg.write.data = (uint8_t *)data;
    arg.write.ext = ext;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_pan_args_t),
                                 btc_pan_arg_deep_copy, btc_pan_arg_deep_free) == BT_STATUS_SUCCESS ?
            ESP_OK : ESP_FAIL);
}

esp_err_t esp_pan_set_protocol_filters(uint16_t handle, uint16_t num_filters,
                                       const uint16_t *start_array, const uint16_t *end_array)
{
    btc_msg_t msg = {0};
    btc_pan_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (handle == ESP_PAN_INVALID_HANDLE ||
            num_filters > ESP_PAN_MAX_PROTOCOL_FILTERS ||
            (num_filters > 0 && (start_array == NULL || end_array == NULL))) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PAN;
    msg.act = BTC_PAN_ACT_SET_PFILTER;

    arg.set_pfilter.handle = handle;
    arg.set_pfilter.num_filters = num_filters;
    arg.set_pfilter.start_array = (num_filters > 0) ? (uint16_t *)start_array : NULL;
    arg.set_pfilter.end_array = (num_filters > 0) ? (uint16_t *)end_array : NULL;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_pan_args_t),
                                 btc_pan_arg_deep_copy, btc_pan_arg_deep_free) == BT_STATUS_SUCCESS ?
            ESP_OK : ESP_FAIL);
}

esp_err_t esp_pan_set_multicast_filters(uint16_t handle, uint16_t num_filters,
                                        const esp_bd_addr_t *start_array, const esp_bd_addr_t *end_array)
{
    btc_msg_t msg = {0};
    btc_pan_args_t arg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (handle == ESP_PAN_INVALID_HANDLE ||
            num_filters > ESP_PAN_MAX_MULTICAST_FILTERS ||
            (num_filters > 0 && (start_array == NULL || end_array == NULL))) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_PAN;
    msg.act = BTC_PAN_ACT_SET_MFILTER;

    arg.set_mfilter.handle = handle;
    arg.set_mfilter.num_filters = num_filters;
    arg.set_mfilter.start_array = (num_filters > 0) ? (esp_bd_addr_t *)start_array : NULL;
    arg.set_mfilter.end_array = (num_filters > 0) ? (esp_bd_addr_t *)end_array : NULL;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_pan_args_t),
                                 btc_pan_arg_deep_copy, btc_pan_arg_deep_free) == BT_STATUS_SUCCESS ?
            ESP_OK : ESP_FAIL);
}

#endif /* BTC_PAN_INCLUDED */
