/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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

void btc_dev_call_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);

    switch (msg->act) {
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    case BTC_DEV_ACT_CFG_COEX_STATUS:
#endif
        break;
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_dev_call_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    BTC_TRACE_DEBUG("%s \n", __func__);
    UNUSED(p_dest);
    UNUSED(p_src);

    switch (msg->act) {
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    case BTC_DEV_ACT_CFG_COEX_STATUS:
#endif
        break;
    default:
        BTC_TRACE_ERROR("Unhandled deep copy %d\n", msg->act);
        break;
    }
}

void btc_dev_cb_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_bt_dev_cb_param_t *src = (esp_bt_dev_cb_param_t *)p_src;
    esp_bt_dev_cb_param_t *dst = (esp_bt_dev_cb_param_t *) p_dest;

    switch (msg->act) {
    case ESP_BT_DEV_NAME_RES_EVT:{
        dst->name_res.name = (char *)osi_malloc((BTC_MAX_LOC_BD_NAME_LEN + 1) * sizeof(char));
        if (dst->name_res.name) {
            BCM_STRNCPY_S(dst->name_res.name, src->name_res.name, BTC_MAX_LOC_BD_NAME_LEN);
            dst->name_res.name[BTC_MAX_LOC_BD_NAME_LEN] = '\0';
        } else {
            BTC_TRACE_ERROR("%s, malloc failed\n", __func__);
        }
        break;
    }
    default:
       BTC_TRACE_ERROR("%s, Unhandled deep copy %d\n", __func__, msg->act);
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
    BTC_TRACE_DEBUG("%s act %d", __func__, msg->act);

    switch (msg->act) {
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    case BTC_DEV_ACT_CFG_COEX_STATUS:
        btc_dev_args_t *arg = (btc_dev_args_t *)msg->arg;
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

    BTC_TRACE_DEBUG("%s act %d", __func__, msg->act);

    if (msg->act < ESP_BT_DEV_EVT_MAX) {
        btc_dev_cb_to_app(msg->act, param);
    } else {
        BTC_TRACE_ERROR("%s, unknown msg->act = %d", __func__, msg->act);
    }

    btc_dev_cb_arg_deep_free(msg);
}
