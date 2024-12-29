/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "btc_ble_mesh_ble.h"
#include "adv.h"
#include "scan.h"
#include "mesh/adapter.h"
#include "esp_ble_mesh_ble_api.h"

#if (CONFIG_BLE_MESH_BLE_COEX_SUPPORT || CONFIG_BLE_MESH_USE_BLE_50)
static void btc_ble_mesh_ble_copy_req_data(btc_msg_t *msg, void *p_dst, void *p_src)
{
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
    esp_ble_mesh_ble_cb_param_t *p_dst_data = (esp_ble_mesh_ble_cb_param_t *)p_dst;
    esp_ble_mesh_ble_cb_param_t *p_src_data = (esp_ble_mesh_ble_cb_param_t *)p_src;

    if (!msg || !p_src_data || !p_dst_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT:
        if (p_src_data->scan_ble_adv_pkt.data && p_src_data->scan_ble_adv_pkt.length) {
            memcpy(&p_dst_data->scan_ble_adv_pkt, &p_src_data->scan_ble_adv_pkt, sizeof(p_src_data->scan_ble_adv_pkt));
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
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */
}

static void btc_ble_mesh_ble_free_req_data(btc_msg_t *msg)
{
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
    esp_ble_mesh_ble_cb_param_t *arg = NULL;

    if (!msg) {
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
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */
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

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_ble_cb_param_t),
                         btc_ble_mesh_ble_copy_req_data, btc_ble_mesh_ble_free_req_data);
}

#if CONFIG_BT_NIMBLE_ENABLED && CONFIG_BLE_MESH_USE_BLE_50
void bt_mesh_ble_nimble_evt_to_btc(struct ble_gap_event *event, void *arg)
{
    esp_ble_mesh_ble_cb_param_t param = {0};

    if (event == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    memcpy(&param.nimble_gap_evt.event, event, sizeof(struct ble_gap_event));
    param.nimble_gap_evt.arg = arg;

    btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_NIMBLE_GAP_EVENT_EVT);
}
#endif /* CONFIG_BT_NIMBLE_ENABLED && CONFIG_BLE_MESH_USE_BLE_50 */

#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
void bt_mesh_ble_scan_cb_evt_to_btc(bt_mesh_ble_adv_report_t *adv_report)
{
    esp_ble_mesh_ble_cb_param_t param = {0};

    if (adv_report == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    memcpy(&param.scan_ble_adv_pkt, adv_report, sizeof(bt_mesh_ble_adv_report_t));

    btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT);
}
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */

void btc_ble_mesh_ble_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_ble_cb_param_t param = {0};
    btc_ble_mesh_ble_args_t *arg = NULL;

    if (!msg) {
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
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
    case BTC_BLE_MESH_ACT_START_BLE_SCAN:
        param.start_ble_scan_comp.err_code =
            bt_mesh_start_ble_scan((struct bt_mesh_ble_scan_param *)&arg->start_ble_scan.param);
        btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_START_BLE_SCANNING_COMP_EVT);
        break;
    case BTC_BLE_MESH_ACT_STOP_BLE_SCAN:
        param.stop_ble_scan_comp.err_code = bt_mesh_stop_ble_scan();
        btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_STOP_BLE_SCANNING_COMP_EVT);
        break;
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */
    case BTC_BLE_MESH_ACT_UPDATE_SCAN_PARAMS:
        struct bt_mesh_scan_param scan_param = {
            .interval = arg->scan_params.scan_interval,
            .window = arg->scan_params.uncoded_scan_window,
        };
        param.scan_params_update_comp.err_code = bt_mesh_scan_param_update(&scan_param);
        btc_ble_mesh_ble_callback(&param, ESP_BLE_MESH_SCAN_PARAMS_UPDATE_COMP_EVT);
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
#endif /* (CONFIG_BLE_MESH_BLE_COEX_SUPPORT || CONFIG_BLE_MESH_USE_BLE_50) */
