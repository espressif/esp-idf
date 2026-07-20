/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "btc/btc_manage.h"
#include "btc_opp.h"
#include "bta_opp_int.h"

#if BTC_OPP_INCLUDED

esp_bt_status_t btc_opp_status_from_bta(tBTA_OPP_STATUS status)
{
    switch (status) {
    case BTA_OPP_OK:
        return ESP_BT_STATUS_SUCCESS;
    case BTA_OPP_NO_RESOURCE:
        return ESP_BT_STATUS_NOMEM;
    case BTA_OPP_NOT_FOUND:
        return ESP_BT_STATUS_OPP_NOT_FOUND;
    case BTA_OPP_BUSY:
        return ESP_BT_STATUS_BUSY;
    case BTA_OPP_NOT_SUPPORTED:
        return ESP_BT_STATUS_UNSUPPORTED;
    case BTA_OPP_ABORTED:
        return ESP_BT_STATUS_OPP_ABORTED;
    case BTA_OPP_SDP_FAIL:
        return ESP_BT_STATUS_OPP_SDP_FAIL;
    case BTA_OPP_OBEX_FAIL:
        return ESP_BT_STATUS_OPP_OBEX_FAIL;
    case BTA_OPP_FORBIDDEN:
        return ESP_BT_STATUS_OPP_FORBIDDEN;
    case BTA_OPP_FAIL:
    default:
        return ESP_BT_STATUS_FAIL;
    }
}

/* Route a BTA event to the client or server BTC profile id. Shared connection
 * events (conn/progress/complete) are dispatched by role so that each role's
 * handler only ever sees its own transfers. */
static uint8_t btc_opp_role_to_pid(tBTA_OPP_ROLE role)
{
#if BTC_OPP_CLIENT_INCLUDED
    if (role == BTA_OPP_ROLE_CLIENT) {
        return BTC_PID_OPP_CLIENT;
    }
#endif
#if BTC_OPP_SERVER_INCLUDED
    return BTC_PID_OPP_SERVER;
#else
    return BTC_PID_OPP_CLIENT;
#endif
}

#if BTC_OPP_SERVER_INCLUDED
/* Deep-copy INCOMING name/type so they remain valid if BTA frees p_ccb->name
 * before the BTC task processes the callback (session sticky / auto_accept). */
static void bte_opp_evt_deep_copy(btc_msg_t *msg, void *dst, void *src)
{
    tBTA_OPP *dst_data = (tBTA_OPP *)dst;
    tBTA_OPP *src_data = (tBTA_OPP *)src;

    if (msg->act != BTA_OPP_INCOMING_OBJECT_EVT || src_data == NULL || dst_data == NULL) {
        return;
    }

    dst_data->object.name = NULL;
    dst_data->object.type = NULL;
    if (src_data->object.name) {
        dst_data->object.name = osi_strdup(src_data->object.name);
    }
    if (src_data->object.type) {
        dst_data->object.type = osi_strdup(src_data->object.type);
    }
}
#endif

void btc_opp_cb_arg_deep_free(btc_msg_t *msg)
{
#if BTC_OPP_SERVER_INCLUDED
    tBTA_OPP *arg = (tBTA_OPP *)(msg->arg);

    if (arg == NULL || msg->act != BTA_OPP_INCOMING_OBJECT_EVT) {
        return;
    }
    if (arg->object.name) {
        osi_free((void *)arg->object.name);
        arg->object.name = NULL;
    }
    if (arg->object.type) {
        osi_free((void *)arg->object.type);
        arg->object.type = NULL;
    }
#else
    (void)msg;
#endif
}

void bte_opp_evt(tBTA_OPP_EVT event, tBTA_OPP *p_data)
{
    btc_msg_t msg;
    int param_len = 0;
    uint8_t pid;
    btc_arg_deep_copy_t copy_func = NULL;
    btc_arg_deep_free_t free_func = NULL;

    switch (event) {
#if BTC_OPP_SERVER_INCLUDED
    case BTA_OPP_SERVER_ENABLE_EVT:
    case BTA_OPP_SERVER_DISABLE_EVT:
        pid = BTC_PID_OPP_SERVER;
        param_len = 0;
        break;
    case BTA_OPP_SERVER_START_EVT:
    case BTA_OPP_SERVER_STOP_EVT:
    case BTA_OPP_DATA_EVT:
        pid = BTC_PID_OPP_SERVER;
        param_len = sizeof(tBTA_OPP);
        break;
    case BTA_OPP_INCOMING_OBJECT_EVT:
        pid = BTC_PID_OPP_SERVER;
        param_len = sizeof(tBTA_OPP);
        copy_func = bte_opp_evt_deep_copy;
        free_func = btc_opp_cb_arg_deep_free;
        break;
#endif
#if BTC_OPP_CLIENT_INCLUDED
    case BTA_OPP_CLIENT_ENABLE_EVT:
    case BTA_OPP_CLIENT_DISABLE_EVT:
        pid = BTC_PID_OPP_CLIENT;
        param_len = 0;
        break;
#endif
    case BTA_OPP_CONN_OPEN_EVT:
    case BTA_OPP_CONN_CLOSE_EVT:
        pid = btc_opp_role_to_pid(p_data->conn.role);
        param_len = sizeof(tBTA_OPP);
        break;
    case BTA_OPP_PROGRESS_EVT:
        pid = btc_opp_role_to_pid(p_data->progress.role);
        param_len = sizeof(tBTA_OPP);
        break;
    case BTA_OPP_TRANSFER_COMPLETE_EVT:
        pid = btc_opp_role_to_pid(p_data->complete.role);
        param_len = sizeof(tBTA_OPP);
        break;
    default:
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = pid;
    msg.act = event;
    if (btc_transfer_context(&msg, p_data, param_len, copy_func, free_func) != BT_STATUS_SUCCESS) {
        if (event == BTA_OPP_DATA_EVT && p_data && p_data->data.pkt) {
            osi_free(p_data->data.pkt);
        }
    }
}

void btc_opp_arg_deep_copy(btc_msg_t *msg, void *dst, void *src)
{
    btc_opp_args_t *dst_arg = (btc_opp_args_t *)dst;
    btc_opp_args_t *src_arg = (btc_opp_args_t *)src;

    memcpy(dst, src, sizeof(btc_opp_args_t));
#if BTC_OPP_SERVER_INCLUDED
    if (msg->act == BTC_OPP_START_SERVER_EVT) {
        if (src_arg->start_server.cfg.service_name) {
            dst_arg->start_server.service_name = osi_strdup(src_arg->start_server.cfg.service_name);
            dst_arg->start_server.cfg.service_name = dst_arg->start_server.service_name;
        }
        if (src_arg->start_server.cfg.supported_formats && src_arg->start_server.cfg.supported_formats_len) {
            dst_arg->start_server.supported_formats = osi_malloc(src_arg->start_server.cfg.supported_formats_len);
            if (dst_arg->start_server.supported_formats) {
                memcpy(dst_arg->start_server.supported_formats, src_arg->start_server.cfg.supported_formats,
                       src_arg->start_server.cfg.supported_formats_len);
                dst_arg->start_server.cfg.supported_formats = dst_arg->start_server.supported_formats;
            } else {
                dst_arg->start_server.supported_formats = NULL;
                dst_arg->start_server.cfg.supported_formats_len = 0;
            }
        }
    }
#endif
#if BTC_OPP_CLIENT_INCLUDED
    if (msg->act == BTC_OPP_OPEN_OBJECT_EVT) {
        if (src_arg->open_object.object.name) {
            dst_arg->open_object.name = osi_strdup(src_arg->open_object.object.name);
            dst_arg->open_object.object.name = dst_arg->open_object.name;
        }
        if (src_arg->open_object.object.type) {
            dst_arg->open_object.type = osi_strdup(src_arg->open_object.object.type);
            dst_arg->open_object.object.type = dst_arg->open_object.type;
        }
    }
#endif
}

void btc_opp_arg_deep_free(btc_msg_t *msg)
{
    btc_opp_args_t *arg = (btc_opp_args_t *)(msg->arg);

    if (arg == NULL) {
        return;
    }
#if BTC_OPP_SERVER_INCLUDED
    if (msg->act == BTC_OPP_START_SERVER_EVT) {
        if (arg->start_server.service_name) {
            osi_free(arg->start_server.service_name);
            arg->start_server.service_name = NULL;
        }
        if (arg->start_server.supported_formats) {
            osi_free(arg->start_server.supported_formats);
            arg->start_server.supported_formats = NULL;
        }
    }
#endif
#if BTC_OPP_CLIENT_INCLUDED
    if (msg->act == BTC_OPP_OPEN_OBJECT_EVT) {
        if (arg->open_object.name) {
            osi_free(arg->open_object.name);
            arg->open_object.name = NULL;
        }
        if (arg->open_object.type) {
            osi_free(arg->open_object.type);
            arg->open_object.type = NULL;
        }
    }
#endif
}

void btc_opp_get_profile_status(esp_opp_profile_status_t *param)
{
#if BTC_OPP_SERVER_INCLUDED
    param->opp_server_inited = btc_opp_server_is_inited();
#endif
#if BTC_OPP_CLIENT_INCLUDED
    param->opp_client_inited = btc_opp_client_is_inited();
#endif
    param->conn_num = bta_opp_get_conn_num();
}

#endif /* BTC_OPP_INCLUDED */
