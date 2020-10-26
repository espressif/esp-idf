// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <string.h>

#include "btc_ble_mesh_ble.h"
#include "adv.h"
#include "mesh_bearer_adapt.h"
#include "esp_ble_mesh_ble_api.h"

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV

static void btc_ble_mesh_ble_copy_req_data(btc_msg_t *msg, void *p_dst, void *p_src)
{
    esp_ble_mesh_ble_cb_param_t *p_dst_data = (esp_ble_mesh_ble_cb_param_t *)p_dst;
    esp_ble_mesh_ble_cb_param_t *p_src_data = (esp_ble_mesh_ble_cb_param_t *)p_src;

    if (!msg || !p_src_data || !p_dst_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    default:
        break;
    }
}

static void btc_ble_mesh_ble_free_req_data(btc_msg_t *msg)
{
    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    default:
        break;
    }
}

static void btc_ble_mesh_ble_callback(esp_ble_mesh_ble_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_BLE_MESH_BLE_COEX)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_BLE_MESH_BLE_COEX;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, sizeof(esp_ble_mesh_ble_cb_param_t),
                         btc_ble_mesh_ble_copy_req_data);
}

void btc_ble_mesh_ble_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_ble_cb_param_t param = {0};
    btc_ble_mesh_ble_args_t *arg = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_ble_args_t *)msg->arg;

    switch (msg->act) {
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
    case BTC_BLE_MESH_ACT_START_BLE_ADV: {
        struct bt_mesh_ble_adv_param *set = (struct bt_mesh_ble_adv_param *)&arg->start_ble_adv.param;
        struct bt_mesh_ble_adv_data *data = NULL;
        if (arg->start_ble_adv.data.adv_data_len || arg->start_ble_adv.data.scan_rsp_data_len) {
            data = (struct bt_mesh_ble_adv_data *)&arg->start_ble_adv.data;
        }

        param.start_ble_advertising_comp.err_code =
            bt_mesh_start_ble_advertising(set, data, &param.start_ble_advertising_comp.index);

        btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_START_BLE_ADVERTISING_COMP_EVT);
        break;
    }
    case BTC_BLE_MESH_ACT_STOP_BLE_ADV:
        param.stop_ble_advertising_comp.index = arg->stop_ble_adv.index;
        param.stop_ble_advertising_comp.err_code =
            bt_mesh_stop_ble_advertising(arg->stop_ble_adv.index);

        btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_STOP_BLE_ADVERTISING_COMP_EVT);
        break;
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */
    default:
        return;
    }
}

static inline void btc_ble_mesh_ble_cb_to_app(esp_ble_mesh_ble_cb_event_t event,
                                              esp_ble_mesh_ble_cb_param_t *param)
{
    esp_ble_mesh_ble_cb_t btc_ble_mesh_cb = (esp_ble_mesh_ble_cb_t)btc_profile_cb_get(BTC_PID_BLE_MESH_BLE_COEX);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_ble_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_ble_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_ble_cb_param_t *)msg->arg;

    if (msg->act < ESP_BLE_MESH_BLE_EVT_MAX) {
        btc_ble_mesh_ble_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_ble_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */
