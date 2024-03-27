/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
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
    param.name_res.name = name;

    ret = btc_transfer_context(&msg, &param, sizeof(esp_bt_dev_cb_param_t), btc_dev_cb_arg_deep_copy, btc_dev_cb_arg_deep_free);
    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_dev_vendor_hci_cmd_complete_callback(tBTA_VSC_CMPL *p_param)
{
    bool param_invalid = false;
    if ((!p_param) || (!p_param->param_len) || (!p_param->p_param_buf)) {
        BTC_TRACE_ERROR("%s param error\n", __func__);
        param_invalid = true;
    }

    esp_bt_dev_cb_param_t param = {0};
    bt_status_t ret;
    btc_msg_t msg = {0};

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_DEV;
    msg.act = ESP_BT_DEV_VENDOR_CMD_COMPLETE_EVT;
    if (!param_invalid) {
        param.vendor_cmd_cmpl.opcode = p_param->opcode;
        param.vendor_cmd_cmpl.param_len = p_param->param_len;
        param.vendor_cmd_cmpl.p_param_buf = p_param->p_param_buf;
    } else {
        if (p_param) {
            param.vendor_cmd_cmpl.opcode = p_param->opcode;
        } else {
            param.vendor_cmd_cmpl.opcode = 0;
        }
        param.vendor_cmd_cmpl.param_len = 0;
        param.vendor_cmd_cmpl.p_param_buf = NULL;
    }

    ret = btc_transfer_context(&msg, &param, sizeof(esp_bt_dev_cb_param_t), btc_dev_cb_arg_deep_copy, btc_dev_cb_arg_deep_free);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}


void btc_dev_call_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);
    btc_dev_args_t *arg = (btc_dev_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_DEV_ACT_SET_DEVICE_NAME:{
        if (arg->set_dev_name.device_name) {
            osi_free(arg->set_dev_name.device_name);
        }
        break;
    }
    case BTC_DEV_ACT_GET_DEVICE_NAME:
#if (ESP_COEX_VSC_INCLUDED == TRUE)
    case BTC_DEV_ACT_CFG_COEX_STATUS:
#endif
        break;
    case BTC_DEV_ACT_VENDOR_HCI_CMD_EVT: {
        uint8_t *p_param_buf = ((btc_dev_args_t *)msg->arg)->vendor_cmd_send.p_param_buf;
        if (p_param_buf) {
            osi_free(p_param_buf);
        }
        break;
    }
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_dev_call_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    BTC_TRACE_DEBUG("%s \n", __func__);
    btc_dev_args_t *dst = (btc_dev_args_t *)p_dest;
    btc_dev_args_t *src = (btc_dev_args_t *)p_src;

    switch (msg->act) {
    case BTC_DEV_ACT_VENDOR_HCI_CMD_EVT: {
        if (src->vendor_cmd_send.param_len) {
            dst->vendor_cmd_send.p_param_buf = osi_malloc(src->vendor_cmd_send.param_len);
            if (dst->vendor_cmd_send.p_param_buf) {
                memcpy(dst->vendor_cmd_send.p_param_buf, src->vendor_cmd_send.p_param_buf, src->vendor_cmd_send.param_len);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    }
    case BTC_DEV_ACT_SET_DEVICE_NAME:{
        dst->set_dev_name.device_name = (char *)osi_malloc((BTC_MAX_LOC_BD_NAME_LEN + 1) * sizeof(char));
        if (dst->set_dev_name.device_name) {
            BCM_STRNCPY_S(dst->set_dev_name.device_name, src->set_dev_name.device_name, BTC_MAX_LOC_BD_NAME_LEN);
            dst->set_dev_name.device_name[BTC_MAX_LOC_BD_NAME_LEN] = '\0';
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
        }
        break;
    }
    case BTC_DEV_ACT_GET_DEVICE_NAME:
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
    esp_bt_dev_cb_param_t  *dst = (esp_bt_dev_cb_param_t *) p_dest;

    switch (msg->act) {
    case ESP_BT_DEV_VENDOR_CMD_COMPLETE_EVT: {
        if (src->vendor_cmd_cmpl.param_len) {
            dst->vendor_cmd_cmpl.p_param_buf = osi_malloc(src->vendor_cmd_cmpl.param_len);
            if (dst->vendor_cmd_cmpl.p_param_buf) {
                memcpy(dst->vendor_cmd_cmpl.p_param_buf, src->vendor_cmd_cmpl.p_param_buf,
                   src->vendor_cmd_cmpl.param_len);
            } else {
                BTC_TRACE_ERROR("%s, malloc failed\n", __func__);
            }
        }
        break;
    }
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
    case ESP_BT_DEV_VENDOR_CMD_COMPLETE_EVT: {
        uint8_t *value = ((esp_bt_dev_cb_param_t *)msg->arg)->vendor_cmd_cmpl.p_param_buf;
        if (value) {
            osi_free(value);
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
    case BTC_DEV_ACT_VENDOR_HCI_CMD_EVT:
        BTA_DmsendVendorHciCmd(arg->vendor_cmd_send.opcode,
                                arg->vendor_cmd_send.param_len,
                                arg->vendor_cmd_send.p_param_buf,
                                btc_dev_vendor_hci_cmd_complete_callback);
        break;
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
