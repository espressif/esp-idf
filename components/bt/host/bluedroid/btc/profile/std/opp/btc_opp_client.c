/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "common/bt_target.h"
#include "bta/bta_opp_api.h"
#include "btc/btc_manage.h"
#include "btc_opp.h"

#if BTC_OPP_CLIENT_INCLUDED

static bool s_btc_opp_client_init;
static bool s_btc_opp_client_vfs_held;

bool btc_opp_client_is_inited(void)
{
    return s_btc_opp_client_init;
}

void btc_opp_client_callback_to_app(esp_opp_client_cb_event_t event, esp_opp_client_param_t *param)
{
    esp_opp_client_callback_t callback = (esp_opp_client_callback_t)btc_profile_cb_get(BTC_PID_OPP_CLIENT);
    if (callback) {
        callback(event, param);
    }
}

static void btc_opp_client_init_module(void)
{
    esp_opp_client_param_t param = {0};

    if (s_btc_opp_client_init) {
        param.init.status = ESP_BT_STATUS_DONE;
        btc_opp_client_callback_to_app(ESP_OPP_CLIENT_INIT_EVT, &param);
        return;
    }

    if (!s_btc_opp_client_vfs_held) {
        if (btc_opp_vfs_acquire() != ESP_OK) {
            param.init.status = ESP_BT_STATUS_FAIL;
            btc_opp_client_callback_to_app(ESP_OPP_CLIENT_INIT_EVT, &param);
            return;
        }
        s_btc_opp_client_vfs_held = true;
    }

    BTA_OppClientEnable(bte_opp_evt);
}

static void btc_opp_client_deinit_module(void)
{
    if (!s_btc_opp_client_init && !s_btc_opp_client_vfs_held) {
        return;
    }

    if (s_btc_opp_client_init) {
        BTA_OppClientDisable();
    } else if (s_btc_opp_client_vfs_held) {
        (void)btc_opp_vfs_release();
        s_btc_opp_client_vfs_held = false;
        BTA_OppClientDisable();
        esp_opp_client_param_t param = {0};
        param.deinit.status = ESP_BT_STATUS_SUCCESS;
        btc_opp_client_callback_to_app(ESP_OPP_CLIENT_DEINIT_EVT, &param);
    }
}

void btc_opp_client_call_handler(btc_msg_t *msg)
{
    btc_opp_args_t *arg = (btc_opp_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_OPP_CLIENT_INIT_EVT:
        btc_opp_client_init_module();
        break;
    case BTC_OPP_CLIENT_DEINIT_EVT:
        btc_opp_client_deinit_module();
        break;
    default:
        if (!btc_opp_client_is_inited()) {
            if (msg->act == BTC_OPP_OPEN_OBJECT_EVT) {
                esp_opp_client_param_t param = {0};
                param.open.handle = arg->open_object.object.handle;
                param.open.fd = -1;
                param.open.status = ESP_BT_STATUS_FAIL;
                btc_opp_client_callback_to_app(ESP_OPP_CLIENT_OPEN_EVT, &param);
            }
            break;
        }
        switch (msg->act) {
        case BTC_OPP_CLIENT_CONNECT_EVT: {
            tBTA_OPP_CONNECT_PARAM param = {0};
            memcpy(param.bd_addr, arg->connect.param.bd_addr, sizeof(BD_ADDR));
            param.sec_mask = arg->connect.param.sec_mask;
            param.mtu = arg->connect.param.mtu;
            BTA_OppClientConnect(&param);
            break;
        }
        case BTC_OPP_CLIENT_DISCONNECT_EVT:
            BTA_OppClientDisconnect(arg->by_handle.handle);
            break;
        case BTC_OPP_OPEN_OBJECT_EVT: {
            esp_opp_client_param_t param = {0};
            tBTA_OPP_SEND_PARAM bta_param = {0};
            int fd = -1;

            param.open.handle = arg->open_object.object.handle;
            param.open.fd = -1;
            param.open.status = ESP_BT_STATUS_FAIL;

            if (btc_opp_vfs_alloc_client_slot(arg->open_object.object.handle,
                                              arg->open_object.object.len, &fd) != ESP_OK) {
                btc_opp_client_callback_to_app(ESP_OPP_CLIENT_OPEN_EVT, &param);
                break;
            }

            bta_param.handle = arg->open_object.object.handle;
            bta_param.name = arg->open_object.name;
            bta_param.type = arg->open_object.type;
            bta_param.len = arg->open_object.object.len;
            if (BTA_OppOpenObject(&bta_param) == BT_STATUS_SUCCESS) {
                /* Ownership transferred to BTA; keep deep_free from freeing twice. */
                arg->open_object.name = NULL;
                arg->open_object.type = NULL;
                param.open.status = ESP_BT_STATUS_SUCCESS;
                param.open.fd = fd;
            } else {
                btc_opp_vfs_free_slot_by_handle(bta_param.handle);
            }
            btc_opp_client_callback_to_app(ESP_OPP_CLIENT_OPEN_EVT, &param);
            break;
        }
        case BTC_OPP_CLIENT_CANCEL_EVT:
            BTA_OppClientCancel(arg->by_handle.handle);
            break;
        default:
            break;
        }
        break;
    }

    btc_opp_arg_deep_free(msg);
}

void btc_opp_client_cb_handler(btc_msg_t *msg)
{
    tBTA_OPP *bta_data = (tBTA_OPP *)(msg->arg);
    esp_opp_client_param_t param;

    memset(&param, 0, sizeof(param));
    switch (msg->act) {
    case BTA_OPP_CLIENT_ENABLE_EVT:
        if (!s_btc_opp_client_vfs_held) {
            BTA_OppClientDisable();
            break;
        }
        s_btc_opp_client_init = true;
        param.init.status = ESP_BT_STATUS_SUCCESS;
        btc_opp_client_callback_to_app(ESP_OPP_CLIENT_INIT_EVT, &param);
        break;
    case BTA_OPP_CLIENT_DISABLE_EVT:
        if (!s_btc_opp_client_init && !s_btc_opp_client_vfs_held) {
            break;
        }
        s_btc_opp_client_init = false;
        if (s_btc_opp_client_vfs_held) {
            (void)btc_opp_vfs_release();
            s_btc_opp_client_vfs_held = false;
        }
        param.deinit.status = ESP_BT_STATUS_SUCCESS;
        btc_opp_client_callback_to_app(ESP_OPP_CLIENT_DEINIT_EVT, &param);
        break;
    case BTA_OPP_CONN_OPEN_EVT:
    case BTA_OPP_CONN_CLOSE_EVT:
        if (!bta_data->conn.connected) {
            btc_opp_vfs_on_conn_close(bta_data->conn.handle);
        }
        param.conn.handle = bta_data->conn.handle;
        param.conn.state = bta_data->conn.connected ? ESP_OPP_CONNECTED : ESP_OPP_DISCONNECTED;
        param.conn.status = btc_opp_status_from_bta(bta_data->conn.status);
        memcpy(param.conn.bd_addr, bta_data->conn.bd_addr, sizeof(esp_bd_addr_t));
        btc_opp_client_callback_to_app(ESP_OPP_CLIENT_CONNECTION_STATE_EVT, &param);
        break;
    case BTA_OPP_PROGRESS_EVT:
        param.progress.handle = bta_data->progress.handle;
        param.progress.transferred = bta_data->progress.transferred;
        param.progress.total = bta_data->progress.total;
        btc_opp_client_callback_to_app(ESP_OPP_CLIENT_PROGRESS_EVT, &param);
        break;
    case BTA_OPP_TRANSFER_COMPLETE_EVT:
        btc_opp_vfs_on_transfer_complete(bta_data->complete.handle);
        param.complete.handle = bta_data->complete.handle;
        param.complete.status = btc_opp_status_from_bta(bta_data->complete.status);
        param.complete.transferred = bta_data->complete.transferred;
        btc_opp_client_callback_to_app(ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT, &param);
        break;
    default:
        break;
    }
}

#endif /* BTC_OPP_CLIENT_INCLUDED */
