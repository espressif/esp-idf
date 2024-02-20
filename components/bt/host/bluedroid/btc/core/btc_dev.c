/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "stack/bt_types.h"
#include "bta/bta_api.h"
#include "btc/btc_task.h"
#include "btc/btc_manage.h"
#include "btc/btc_util.h"
#include "btc/btc_dev.h"

static inline void btc_dev_cb_to_app(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param)
{
    esp_bt_dev_cb_t btc_bt_dev_cb = (esp_bt_dev_cb_t)btc_profile_cb_get(BTC_PID_DEV);
    if (btc_bt_dev_cb) {
        btc_bt_dev_cb(event, param);
    }
}

static void btc_dev_get_dev_name_callback(UINT8 status, char *name)
{
    esp_bt_dev_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg = {0};

    memset(&param, 0, sizeof(esp_bt_dev_cb_param_t));

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DEV;
    msg.act = ESP_BT_DEV_NAME_RES_EVT;

    param.name_res.status = btc_btm_status_to_esp_status(status);
    param.name_res.name = (char *)osi_malloc(BTC_MAX_LOC_BD_NAME_LEN + 1);
    if (param.name_res.name) {
        BCM_STRNCPY_S(param.name_res.name, name, BTC_MAX_LOC_BD_NAME_LEN);
        param.name_res.name[BTC_MAX_LOC_BD_NAME_LEN] = '\0';
    } else {
        param.name_res.status = ESP_BT_STATUS_NOMEM;
    }

    ret = btc_transfer_context(&msg, &param, sizeof(esp_bt_dev_cb_param_t), NULL, NULL);
    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

void btc_dev_call_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);

    switch (msg->act) {
    case BTC_DEV_ACT_SET_DEVICE_NAME:{
        char *device_name = ((btc_dev_args_t *)msg->arg)->set_dev_name.device_name;
        if (device_name) {
            osi_free(device_name);
        }
        break;
    }
    case BTC_DEV_ACT_GET_DEVICE_NAME:
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    case BTC_DEV_ACT_CFG_COEX_STATUS:
#endif
        break;
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_dev_cb_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);

    switch (msg->act) {
    case ESP_BT_DEV_NAME_RES_EVT:{
        char *name = ((esp_bt_dev_cb_param_t *)msg->arg)->name_res.name;
        if (name) {
            osi_free(name);
        }
        break;
    }
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_dev_call_handler(btc_msg_t *msg)
{
    btc_dev_args_t *arg = (btc_dev_args_t *)msg->arg;

    BTC_TRACE_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

    switch (msg->act) {
    case BTC_DEV_ACT_SET_DEVICE_NAME:
        BTA_DmSetDeviceName(arg->set_dev_name.device_name);
        break;
    case BTC_DEV_ACT_GET_DEVICE_NAME:
        BTA_DmGetDeviceName(btc_dev_get_dev_name_callback);
        break;
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    case BTC_DEV_ACT_CFG_COEX_STATUS:
        BTA_DmCfgCoexStatus(arg->cfg_coex_status.op,
                            arg->cfg_coex_status.type,
                            arg->cfg_coex_status.status);
        break;
#endif
    default:
        break;
    }

    btc_dev_call_arg_deep_free(msg);
}

void btc_dev_cb_handler(btc_msg_t *msg)
{
    esp_bt_dev_cb_param_t *param = (esp_bt_dev_cb_param_t *)msg->arg;

    if (msg->act < ESP_BT_DEV_EVT_MAX) {
        btc_dev_cb_to_app(msg->act, param);
    } else {
        BTC_TRACE_ERROR("%s, unknow msg->act = %d", __func__, msg->act);
    }

    btc_dev_cb_arg_deep_free(msg);
}
