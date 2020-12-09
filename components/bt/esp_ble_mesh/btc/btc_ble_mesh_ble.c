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

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN

static void btc_ble_mesh_ble_copy_req_data(btc_msg_t *msg, void *p_dst, void *p_src)
{
    esp_ble_mesh_ble_cb_param_t *p_dst_data = (esp_ble_mesh_ble_cb_param_t *)p_dst;
    esp_ble_mesh_ble_cb_param_t *p_src_data = (esp_ble_mesh_ble_cb_param_t *)p_src;

    if (!msg || !p_src_data || !p_dst_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT:
        if (p_src_data->scan_ble_adv_pkt.data && p_src_data->scan_ble_adv_pkt.length) {
            p_dst_data->scan_ble_adv_pkt.length = p_src_data->scan_ble_adv_pkt.length;
            p_dst_data->scan_ble_adv_pkt.data = bt_mesh_calloc(p_src_data->scan_ble_adv_pkt.length);
            if (p_dst_data->scan_ble_adv_pkt.data) {
                memcpy(p_dst_data->scan_ble_adv_pkt.data, p_src_data->scan_ble_adv_pkt.data,
                       p_src_data->scan_ble_adv_pkt.length);
            } else {
                BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            }
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_ble_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_ble_cb_param_t *arg = NULL;

    if (!msg || !msg->arg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_ble_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT:
        if (arg->scan_ble_adv_pkt.data) {
            bt_mesh_free(arg->scan_ble_adv_pkt.data);
        }
        break;
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

void bt_mesh_ble_scan_cb_evt_to_btc(const bt_mesh_addr_t *addr,
                                    uint8_t adv_type, uint8_t data[],
                                    uint16_t length, int8_t rssi)
{
    esp_ble_mesh_ble_cb_param_t param = {0};

    if (addr == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    memcpy(param.scan_ble_adv_pkt.addr, addr->val, sizeof(addr->val));
    param.scan_ble_adv_pkt.addr_type = addr->type;
    if (data && length) {
        param.scan_ble_adv_pkt.data = data;
        param.scan_ble_adv_pkt.length = length;
    }
    param.scan_ble_adv_pkt.adv_type = adv_type;
    param.scan_ble_adv_pkt.rssi = rssi;

    btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT);
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
    case BTC_BLE_MESH_ACT_START_BLE_SCAN:
        param.start_ble_scan_comp.err_code =
            bt_mesh_start_ble_scan((struct bt_mesh_ble_scan_param *)&arg->start_ble_scan.param);
        btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_START_BLE_SCANNING_COMP_EVT);
        break;
    case BTC_BLE_MESH_ACT_STOP_BLE_SCAN:
        param.stop_ble_scan_comp.err_code = bt_mesh_stop_ble_scan();
        btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_STOP_BLE_SCANNING_COMP_EVT);
        break;
    default:
        return;
    }
}

static inline void btc_ble_mesh_ble_cb_to_app(esp_ble_mesh_ble_cb_event_t event,
                                              esp_ble_mesh_ble_cb_param_t *param)
{
    esp_ble_mesh_ble_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_ble_cb_t)btc_profile_cb_get(BTC_PID_BLE_MESH_BLE_COEX);
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

#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */
