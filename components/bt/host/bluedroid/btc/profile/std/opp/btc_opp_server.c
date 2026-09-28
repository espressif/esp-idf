/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "common/bt_target.h"
#include "bta/bta_opp_api.h"
#include "btc/btc_manage.h"
#include "btc_opp.h"

#if BTC_OPP_SERVER_INCLUDED

static bool s_btc_opp_server_init;
static bool s_btc_opp_server_vfs_held;

bool btc_opp_server_is_inited(void)
{
    return s_btc_opp_server_init;
}

void btc_opp_server_callback_to_app(esp_opp_server_cb_event_t event, esp_opp_server_param_t *param)
{
    esp_opp_server_callback_t callback = (esp_opp_server_callback_t)btc_profile_cb_get(BTC_PID_OPP_SERVER);
    if (callback) {
        callback(event, param);
    }
}

static void btc_opp_server_init_module(void)
{
    esp_opp_server_param_t param = {0};

    if (s_btc_opp_server_init) {
        param.init.status = ESP_BT_STATUS_DONE;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_INIT_EVT, &param);
        return;
    }

    if (!s_btc_opp_server_vfs_held) {
        if (btc_opp_vfs_acquire() != ESP_OK) {
            param.init.status = ESP_BT_STATUS_FAIL;
            btc_opp_server_callback_to_app(ESP_OPP_SERVER_INIT_EVT, &param);
            return;
        }
        s_btc_opp_server_vfs_held = true;
    }

    BTA_OppServerEnable(bte_opp_evt);
}

static void btc_opp_server_deinit_module(void)
{
    if (!s_btc_opp_server_init && !s_btc_opp_server_vfs_held) {
        return;
    }

    if (s_btc_opp_server_init) {
        BTA_OppServerDisable();
    } else if (s_btc_opp_server_vfs_held) {
        (void)btc_opp_vfs_release();
        s_btc_opp_server_vfs_held = false;
        BTA_OppServerDisable();
        esp_opp_server_param_t param = {0};
        param.deinit.status = ESP_BT_STATUS_SUCCESS;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_DEINIT_EVT, &param);
    }
}

void btc_opp_server_call_handler(btc_msg_t *msg)
{
    btc_opp_args_t *arg = (btc_opp_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_OPP_SERVER_INIT_EVT:
        btc_opp_server_init_module();
        break;
    case BTC_OPP_SERVER_DEINIT_EVT:
        btc_opp_server_deinit_module();
        break;
    default:
        if (!btc_opp_server_is_inited()) {
            if (msg->act == BTC_OPP_SERVER_ACCEPT_EVT) {
                esp_opp_server_param_t param = {0};
                param.accept.handle = arg->by_handle.handle;
                param.accept.fd = -1;
                param.accept.status = ESP_BT_STATUS_FAIL;
                btc_opp_server_callback_to_app(ESP_OPP_SERVER_ACCEPT_EVT, &param);
            }
            break;
        }
        switch (msg->act) {
        case BTC_OPP_START_SERVER_EVT: {
            tBTA_OPP_SERVER_CFG cfg = {
                .sec_mask = arg->start_server.cfg.sec_mask,
                .mtu = arg->start_server.cfg.mtu,
                .service_name = arg->start_server.service_name,
                .supported_formats = arg->start_server.supported_formats,
                .supported_formats_len = arg->start_server.cfg.supported_formats_len,
                .auto_accept = arg->start_server.cfg.auto_accept,
            };
            if (BTA_OppStartServer(&cfg) == BT_STATUS_SUCCESS) {
                arg->start_server.service_name = NULL;
                arg->start_server.supported_formats = NULL;
            }
            break;
        }
        case BTC_OPP_STOP_SERVER_EVT:
            BTA_OppStopServer();
            break;
        case BTC_OPP_SERVER_ACCEPT_EVT: {
            esp_opp_server_param_t param = {0};
            int fd = -1;

            param.accept.handle = arg->by_handle.handle;
            param.accept.fd = -1;
            if (btc_opp_vfs_get_fd(arg->by_handle.handle, &fd) != ESP_OK) {
                param.accept.status = ESP_BT_STATUS_FAIL;
                btc_opp_server_callback_to_app(ESP_OPP_SERVER_ACCEPT_EVT, &param);
                break;
            }
            BTA_OppAccept(arg->by_handle.handle);
            param.accept.status = ESP_BT_STATUS_SUCCESS;
            param.accept.fd = fd;
            btc_opp_server_callback_to_app(ESP_OPP_SERVER_ACCEPT_EVT, &param);
            break;
        }
        case BTC_OPP_SERVER_REJECT_EVT:
            btc_opp_vfs_free_slot_by_handle(arg->by_handle.handle);
            BTA_OppReject(arg->by_handle.handle);
            break;
        case BTC_OPP_SERVER_CANCEL_EVT:
            btc_opp_vfs_free_slot_by_handle(arg->by_handle.handle);
            BTA_OppServerCancel(arg->by_handle.handle);
            break;
        default:
            break;
        }
        break;
    }

    btc_opp_arg_deep_free(msg);
}

void btc_opp_server_cb_handler(btc_msg_t *msg)
{
    tBTA_OPP *bta_data = (tBTA_OPP *)(msg->arg);
    esp_opp_server_param_t param;

    memset(&param, 0, sizeof(param));
    switch (msg->act) {
    case BTA_OPP_SERVER_ENABLE_EVT:
        if (!s_btc_opp_server_vfs_held) {
            BTA_OppServerDisable();
            break;
        }
        s_btc_opp_server_init = true;
        param.init.status = ESP_BT_STATUS_SUCCESS;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_INIT_EVT, &param);
        break;
    case BTA_OPP_SERVER_DISABLE_EVT:
        if (!s_btc_opp_server_init && !s_btc_opp_server_vfs_held) {
            break;
        }
        s_btc_opp_server_init = false;
        if (s_btc_opp_server_vfs_held) {
            (void)btc_opp_vfs_release();
            s_btc_opp_server_vfs_held = false;
        }
        param.deinit.status = ESP_BT_STATUS_SUCCESS;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_DEINIT_EVT, &param);
        break;
    case BTA_OPP_SERVER_START_EVT:
        param.start.status = btc_opp_status_from_bta(bta_data->server.status);
        param.start.scn = bta_data->server.scn;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_START_EVT, &param);
        break;
    case BTA_OPP_SERVER_STOP_EVT:
        param.stop.status = btc_opp_status_from_bta(bta_data->server.status);
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_STOP_EVT, &param);
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
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_CONNECTION_STATE_EVT, &param);
        break;
    case BTA_OPP_INCOMING_OBJECT_EVT: {
        int fd = -1;
        param.incoming.handle = bta_data->object.handle;
        param.incoming.name = bta_data->object.name;
        param.incoming.type = bta_data->object.type;
        param.incoming.len = bta_data->object.len;
        param.incoming.needs_accept = bta_data->object.needs_accept;
        if (btc_opp_vfs_alloc_server_slot(bta_data->object.handle, &fd) != ESP_OK) {
            fd = -1;
        }
        /* fd is only valid when already authorized; needs_accept delivers it in ACCEPT_EVT. */
        param.incoming.fd = param.incoming.needs_accept ? -1 : fd;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_INCOMING_OBJECT_EVT, &param);
        break;
    }
    case BTA_OPP_DATA_EVT:
        if (!btc_opp_vfs_enqueue_rx(bta_data->data.handle, bta_data->data.data,
                                    bta_data->data.data_len, bta_data->data.final)) {
            /* No VFS credit/slot: abort so the peer does not hang without Continue. */
            BTA_OppServerCancel(bta_data->data.handle);
        }
        if (bta_data->data.pkt) {
            osi_free(bta_data->data.pkt);
        }
        break;
    case BTA_OPP_PROGRESS_EVT:
        param.progress.handle = bta_data->progress.handle;
        param.progress.transferred = bta_data->progress.transferred;
        param.progress.total = bta_data->progress.total;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_PROGRESS_EVT, &param);
        break;
    case BTA_OPP_TRANSFER_COMPLETE_EVT:
        btc_opp_vfs_on_transfer_complete(bta_data->complete.handle);
        param.complete.handle = bta_data->complete.handle;
        param.complete.status = btc_opp_status_from_bta(bta_data->complete.status);
        param.complete.transferred = bta_data->complete.transferred;
        btc_opp_server_callback_to_app(ESP_OPP_SERVER_TRANSFER_COMPLETE_EVT, &param);
        break;
    default:
        break;
    }

    btc_opp_cb_arg_deep_free(msg);
}

#endif /* BTC_OPP_SERVER_INCLUDED */
