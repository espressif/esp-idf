/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *  Copyright (C) 2019 Blake Felt
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
/************************************************************************************
 *
 *  Filename:      btc_hd.c
 *
 *  Description:   HID Device Profile Bluetooth Interface
 *
 *
 ***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bta/bta_api.h"
#include "bta/bta_hd_api.h"
#include "bta/bta_hh_api.h"
#include "bta/utl.h"
#include "btc/btc_storage.h"
#include "btc/btc_util.h"
#include "btc/btc_manage.h"
#include "btc_hd.h"

#include "osi/allocator.h"

#include "esp_hidd_api.h"

#if HID_DEV_INCLUDED == TRUE
#include "bta_dm_int.h"

/* HD request events */
typedef enum { BTC_HD_DUMMY_REQ_EVT = 0 } btc_hd_req_evt_t;

/*******************************************************************************
 *  Static variables
 ******************************************************************************/
btc_hd_cb_t btc_hd_cb = {0};

// static tBTA_HD_APP_INFO app_info;
// static tBTA_HD_QOS_INFO in_qos;
// static tBTA_HD_QOS_INFO out_qos;

/******************************************************************************
 *  Constants & Macros
 *****************************************************************************/
#define BTC_HD_APP_NAME_LEN 50
#define BTC_HD_APP_DESCRIPTION_LEN 50
#define BTC_HD_APP_PROVIDER_LEN 50
#define BTC_HD_APP_DESCRIPTOR_LEN 2048
#define COD_HID_KEYBOARD 0x0540
#define COD_HID_POINTING 0x0580
#define COD_HID_COMBO 0x05C0
#define COD_HID_MAJOR 0x0500

#define is_hidd_init() (btc_hd_cb.status > BTC_HD_DISABLED)
#define is_hidd_app_register() (btc_hd_cb.app_registered)

typedef void (bt_hid_copy_cb_t)(btc_msg_t *msg, void *p_dest, void *p_src);

static inline void btc_hd_cb_to_app(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param)
{
    esp_hd_cb_t btc_hd_cb = (esp_hd_cb_t)btc_profile_cb_get(BTC_PID_HD);
    if (btc_hd_cb) {
        btc_hd_cb(event, param);
    }
}

static void free_app_info_param(void)
{
    utl_freebuf((void **)&btc_hd_cb.app_info.descriptor.dsc_list);
    utl_freebuf((void **)&btc_hd_cb.app_info.p_provider);
    utl_freebuf((void **)&btc_hd_cb.app_info.p_description);
    utl_freebuf((void **)&btc_hd_cb.app_info.p_name);
}

static void bte_hd_arg_deep_copy(btc_msg_t *msg, void *p_dst, void *p_src)
{
    tBTA_HD *p_dst_data = (tBTA_HD *)p_dst;
    tBTA_HD *p_src_data = (tBTA_HD *)p_src;
    switch (msg->act)
    {
    case BTA_HD_SET_REPORT_EVT: {
        uint8_t *src_data = p_src_data->set_report.p_data;
        if (src_data) {
            p_dst_data->set_report.p_data = osi_malloc(p_src_data->set_report.len);
            if (p_dst_data->set_report.p_data == NULL) {
                BTC_TRACE_ERROR("%s malloc set_report data failed!", __func__);
                break;
            }
            memcpy(p_dst_data->set_report.p_data, src_data, p_src_data->set_report.len);
        }
        break;
    }
    case BTA_HD_INTR_DATA_EVT: {
        uint8_t *src_data = p_src_data->intr_data.p_data;
        if (src_data) {
            p_dst_data->intr_data.p_data = osi_malloc(p_src_data->intr_data.len);
            if (p_dst_data->intr_data.p_data == NULL) {
                BTC_TRACE_ERROR("%s malloc intr_data data failed!", __func__);
                break;
            }
            memcpy(p_dst_data->intr_data.p_data, src_data, p_src_data->intr_data.len);
        }
        break;
    }
    default:
        break;
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_remove_device
 *
 * Description      Removes plugged device
 *
 * Returns          void
 *
 ******************************************************************************/
void btc_hd_remove_device(bt_bdaddr_t bd_addr)
{
    BTA_HdRemoveDevice((uint8_t *)&bd_addr);
    // btc_storage_remove_hidd(&bd_addr);
}

/*******************************************************************************
 *
 * Function         bte_hd_evt
 *
 * Description      Switches context from BTE to BTC for all BT-HD events
 *
 * Returns          void
 *
 ******************************************************************************/
static void bte_hd_evt(tBTA_HD_EVT event, tBTA_HD *p_data)
{
    bt_status_t status;
    int param_len = 0;

    BTC_TRACE_API("%s event=%d", __func__, event);

    switch (event) {
    case BTA_HD_ENABLE_EVT:
    case BTA_HD_DISABLE_EVT:
    case BTA_HD_UNREGISTER_APP_EVT:
        param_len = sizeof(tBTA_HD_STATUS);
        break;
    case BTA_HD_REGISTER_APP_EVT:
        param_len = sizeof(tBTA_HD_REG_STATUS);
        break;
    case BTA_HD_OPEN_EVT:
    case BTA_HD_CLOSE_EVT:
    case BTA_HD_VC_UNPLUG_EVT:
        param_len = sizeof(tBTA_HD_CONN);
        break;
    case BTA_HD_GET_REPORT_EVT:
        param_len += sizeof(tBTA_HD_GET_REPORT);
        break;
    case BTA_HD_SET_REPORT_EVT:
        param_len = sizeof(tBTA_HD_SET_REPORT);
        break;
    case BTA_HD_SET_PROTOCOL_EVT:
        param_len += sizeof(p_data->set_protocol);
        break;
    case BTA_HD_INTR_DATA_EVT:
        param_len = sizeof(tBTA_HD_INTR_DATA);
        break;
    case BTA_HD_SEND_REPORT_EVT:
        param_len = sizeof(tBTA_HD_API_SEND_REPORT);
        break;
    case BTA_HD_REPORT_ERR_EVT:
        param_len = sizeof(tBTA_HD_API_REPORT_ERR);
        break;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HD;
    msg.act = event;

    status = btc_transfer_context(&msg, p_data, param_len, bte_hd_arg_deep_copy, btc_hd_cb_arg_deep_free);
    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("context transfer failed");
    }
}

/*******************************************************************************
 *
 * Function        btc_hd_init
 *
 * Description     Initializes BT-HD interface
 *
 * Returns         void
 *
 ******************************************************************************/
static void btc_hd_init(void)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;
    do {
        if (is_hidd_init()) {
            BTC_TRACE_ERROR("%s HD has been initiated, shall uninit first!", __func__);
            ret = ESP_HIDD_NEED_DEINIT;
            break;
        }
        memset(&btc_hd_cb, 0, sizeof(btc_hd_cb));
        /* enable HD */
        BTA_HdEnable(bte_hd_evt);
    } while (0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param;
        param.init.status = ret;
        btc_hd_cb_to_app(ESP_HIDD_INIT_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_deinit
 *
 * Description      de-initializes the hd interface
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_unregister_app(bool need_deinit);
static void btc_hd_deinit(void)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;
    do {
        if (!is_hidd_init()) {
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        }

        if (btc_hd_cb.status == BTC_HD_DISABLING) {
            BTC_TRACE_ERROR("%s is disabling, try later!", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        }

        btc_hd_cb.service_dereg_active = FALSE;
        // unresgister app will also relase the connection
        // and disable after receiving unregister event from lower layer
        if (is_hidd_app_register()) {
            btc_hd_unregister_app(true);
        } else {
            btc_hd_cb.status = BTC_HD_DISABLING;
            BTC_TRACE_WARNING("%s disabling hid device service now", __func__);
            BTA_HdDisable();
        }
    } while (0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param;
        param.deinit.status = ret;
        btc_hd_cb_to_app(ESP_HIDD_DEINIT_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_register_app
 *
 * Description      Registers HID Device application
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_register_app(esp_hidd_app_param_t *p_app_param, esp_hidd_qos_param_t *p_in_qos,
                                esp_hidd_qos_param_t *p_out_qos)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;
    do {
        if (!is_hidd_init()) {
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        } else if (btc_hd_cb.status == BTC_HD_DISABLING) {
            BTC_TRACE_ERROR("%s: deinit is in progress!", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        }

        if (is_hidd_app_register()) {
            BTC_TRACE_ERROR("%s: application already registered, shall deregister first!", __func__);
            ret = ESP_HIDD_NEED_DEREG;
            break;
        }

        if ((btc_hd_cb.app_info.p_name = (char *)osi_malloc(BTC_HD_APP_NAME_LEN)) == NULL ||
            (btc_hd_cb.app_info.p_description = (char *)osi_malloc(BTC_HD_APP_DESCRIPTION_LEN)) == NULL ||
            (btc_hd_cb.app_info.p_provider = (char *)osi_malloc(BTC_HD_APP_PROVIDER_LEN)) == NULL ||
            (btc_hd_cb.app_info.descriptor.dsc_list = (uint8_t *)osi_malloc(p_app_param->desc_list_len)) == NULL) {
            BTC_TRACE_ERROR(
                "%s malloc app_info failed! p_name:%p, p_description:%p, p_provider:%p, descriptor.dsc_list:%p",
                __func__, btc_hd_cb.app_info.p_name, btc_hd_cb.app_info.p_description, btc_hd_cb.app_info.p_provider,
                btc_hd_cb.app_info.descriptor.dsc_list);
            ret = ESP_HIDD_NO_RES;
            break;
        }
        memcpy(btc_hd_cb.app_info.p_name, p_app_param->name, BTC_HD_APP_NAME_LEN);
        memcpy(btc_hd_cb.app_info.p_description, p_app_param->description, BTC_HD_APP_DESCRIPTION_LEN);
        memcpy(btc_hd_cb.app_info.p_provider, p_app_param->provider, BTC_HD_APP_PROVIDER_LEN);
        memcpy(btc_hd_cb.app_info.descriptor.dsc_list, p_app_param->desc_list, p_app_param->desc_list_len);
        btc_hd_cb.app_info.subclass = p_app_param->subclass;
        btc_hd_cb.app_info.descriptor.dl_len = p_app_param->desc_list_len;

        btc_hd_cb.in_qos.service_type = p_in_qos->service_type;
        btc_hd_cb.in_qos.token_rate = p_in_qos->token_rate;
        btc_hd_cb.in_qos.token_bucket_size = p_in_qos->token_bucket_size;
        btc_hd_cb.in_qos.peak_bandwidth = p_in_qos->peak_bandwidth;
        btc_hd_cb.in_qos.access_latency = p_in_qos->access_latency;
        btc_hd_cb.in_qos.delay_variation = p_in_qos->delay_variation;
        btc_hd_cb.out_qos.service_type = p_out_qos->service_type;
        btc_hd_cb.out_qos.token_rate = p_out_qos->token_rate;
        btc_hd_cb.out_qos.token_bucket_size = p_out_qos->token_bucket_size;
        btc_hd_cb.out_qos.peak_bandwidth = p_out_qos->peak_bandwidth;
        btc_hd_cb.out_qos.access_latency = p_out_qos->access_latency;
        btc_hd_cb.out_qos.delay_variation = p_out_qos->delay_variation;

        BTA_HdRegisterApp(&btc_hd_cb.app_info, &btc_hd_cb.in_qos, &btc_hd_cb.out_qos);
    } while(0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param;
        param.register_app.status = ret;
        param.register_app.in_use = false;
        memset(param.register_app.bd_addr, 0, BD_ADDR_LEN);
        btc_hd_cb_to_app(ESP_HIDD_REGISTER_APP_EVT, &param);
    }
    free_app_info_param();
}

/*******************************************************************************
 *
 * Function         btc_hd_unregister_app
 *
 * Description      Unregisters HID Device application
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_unregister_app(bool need_deinit)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;
    do {
        if (!is_hidd_init()) {
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        } else if (btc_hd_cb.status == BTC_HD_DISABLING) {
            BTC_TRACE_ERROR("%s: deinit is in progress!", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        }

        if (!is_hidd_app_register()) {
            BTC_TRACE_ERROR("%s: application has not been registered, shall register first!", __func__);
            ret = ESP_HIDD_NEED_REG;
            break;
        }

        if (btc_hd_cb.service_dereg_active) {
            BTC_TRACE_ERROR("%s: BT-HD deregistering in progress", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        }
        btc_hd_cb.service_dereg_active = TRUE;

        if (need_deinit) {
            btc_hd_cb.status = BTC_HD_DISABLING;
        }

        BTA_HdUnregisterApp();
    } while(0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param = {0};
        param.unregister_app.status = ret;
        btc_hd_cb_to_app(ESP_HIDD_UNREGISTER_APP_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_connect
 *
 * Description      Connects to host
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_connect(BD_ADDR bd_addr)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;

    do {
        switch (btc_hd_cb.status) {
        case BTC_HD_DISABLED:
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        case BTC_HD_DISABLING:
            BTC_TRACE_ERROR("%s: deinit is in progress!", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        case BTC_HD_CONNECTING:
        case BTC_HD_DISCONNECTING:
            BTC_TRACE_ERROR("%s: busy now, status:%d, try later!", __func__, btc_hd_cb.status);
            ret = ESP_HIDD_BUSY;
            break;
        case BTC_HD_CONNECTED:
            BTC_TRACE_ERROR("%s: already connect to the other HID host!", __func__);
            ret = ESP_HIDD_NO_RES;
            break;
        default:
            break;
        }

        if (ret != ESP_HIDD_SUCCESS) {
            break;
        }

        if (!is_hidd_app_register()) {
            BTC_TRACE_ERROR("%s: application has not been registered, shall register first!", __func__);
            ret = ESP_HIDD_NEED_REG;
            break;
        }

        BTA_HdConnect(bd_addr);
        btc_hd_cb.status = BTC_HD_CONNECTING;
    } while (0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param = {0};
        param.open.status = ret;
        param.open.conn_status = ESP_HIDD_CONN_STATE_DISCONNECTED;
        memcpy(param.open.bd_addr, bd_addr, BD_ADDR_LEN);
        btc_hd_cb_to_app(ESP_HIDD_OPEN_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_disconnect
 *
 * Description      Disconnects from host
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_disconnect(void)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;

    do {
        switch (btc_hd_cb.status) {
        case BTC_HD_DISABLED:
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        case BTC_HD_DISABLING:
            BTC_TRACE_ERROR("%s: deinit is in progress!", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        case BTC_HD_CONNECTING:
        case BTC_HD_DISCONNECTING:
            BTC_TRACE_ERROR("%s: busy now, status:%d, try later!", __func__, btc_hd_cb.status);
            ret = ESP_HIDD_BUSY;
            break;
        case BTC_HD_ENABLED:
        case BTC_HD_DISCONNECTED:
            BTC_TRACE_ERROR("%s: no connection!", __func__);
            ret = ESP_HIDD_NO_CONNECTION;
            break;
        default:
            break;
        }

        if (ret != ESP_HIDD_SUCCESS) {
            break;
        }

        if (!is_hidd_app_register()) {
            BTC_TRACE_ERROR("%s: application has not been registered, shall register first!", __func__);
            ret = ESP_HIDD_NEED_REG;
            break;
        }

        BTA_HdDisconnect();
        btc_hd_cb.status = BTC_HD_DISCONNECTING;
    } while (0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param = {0};
        param.close.status = ret;
        param.close.conn_status = ESP_HIDD_CONN_STATE_DISCONNECTED;
        btc_hd_cb_to_app(ESP_HIDD_CLOSE_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_send_report
 *
 * Description      Sends Reports to hid host
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_send_report(esp_hidd_report_type_t type, uint8_t id, uint16_t len, uint8_t *p_data)
{
    BTC_TRACE_API("%s: type=%d id=%d len=%d", __func__, type, id, len);
    tBTA_HD_REPORT report = {0};
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;

    do {
        switch (btc_hd_cb.status) {
        case BTC_HD_DISABLED:
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        case BTC_HD_DISABLING:
            BTC_TRACE_ERROR("%s: deinit is in progress!", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        case BTC_HD_CONNECTING:
        case BTC_HD_DISCONNECTING:
            BTC_TRACE_ERROR("%s: busy now, status:%d, try later!", __func__, btc_hd_cb.status);
            ret = ESP_HIDD_BUSY;
            break;
        case BTC_HD_ENABLED:
        case BTC_HD_DISCONNECTED:
            if (type == ESP_HIDD_REPORT_TYPE_INTRDATA) {
                BTC_TRACE_WARNING("%s: no connection, try to reconnect!", __func__);
                btc_hd_cb.status = BTC_HD_CONNECTING;
            } else {
                BTC_TRACE_ERROR("%s: no connection!", __func__);
                ret = ESP_HIDD_NO_CONNECTION;
            }
            break;
        default:
            break;
        }

        if (ret != ESP_HIDD_SUCCESS) {
            break;
        }

        if (!is_hidd_app_register()) {
            BTC_TRACE_ERROR("%s: application has not been registered, shall register first!", __func__);
            ret = ESP_HIDD_NEED_REG;
            break;
        }

        if (type == ESP_HIDD_REPORT_TYPE_INTRDATA) {
            report.type = ESP_HIDD_REPORT_TYPE_INPUT;
            report.use_intr = TRUE;
        } else {
            report.type = (type & 0x03);
            report.use_intr = FALSE;
        }

        report.id = id;
        report.len = len;
        report.p_data = p_data;

        BTA_HdSendReport(&report);
    } while (0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param = {0};
        param.send_report.status = ret;
        param.send_report.reason = 0;
        param.send_report.report_type = report.type;
        param.send_report.report_id = report.id;
        btc_hd_cb_to_app(ESP_HIDD_SEND_REPORT_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_report_error
 *
 * Description      Sends HANDSHAKE with error info for invalid SET_REPORT
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_report_error(uint8_t error)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;
    do {
        if (!is_hidd_init()) {
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        }

        if (!is_hidd_app_register()) {
            BTC_TRACE_ERROR("%s: application has not been registered, shall register first!", __func__);
            ret = ESP_HIDD_NEED_REG;
            break;
        }

        if (btc_hd_cb.status != BTC_HD_CONNECTED) {
            BTC_TRACE_ERROR("%s: no connection!", __func__);
            ret = ESP_HIDD_NO_CONNECTION;
            break;
        }

        BTA_HdReportError(error);
    } while (0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param = {0};
        param.report_err.status = ret;
        param.report_err.reason = 0;
        btc_hd_cb_to_app(ESP_HIDD_REPORT_ERR_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hd_virtual_cable_unplug
 *
 * Description      Sends Virtual Cable Unplug to host
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hd_virtual_cable_unplug(void)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidd_status_t ret = ESP_HIDD_SUCCESS;

    do {
        switch (btc_hd_cb.status) {
        case BTC_HD_DISABLED:
            BTC_TRACE_ERROR("%s HD has not been initiated, shall init first!", __func__);
            ret = ESP_HIDD_NEED_INIT;
            break;
        case BTC_HD_DISABLING:
            BTC_TRACE_ERROR("%s: deinit is in progress!", __func__);
            ret = ESP_HIDD_BUSY;
            break;
        case BTC_HD_CONNECTING:
        case BTC_HD_DISCONNECTING:
            BTC_TRACE_ERROR("%s: busy now, status:%d, try later!", __func__, btc_hd_cb.status);
            ret = ESP_HIDD_BUSY;
            break;
        default:
            break;
        }

        if (ret != ESP_HIDD_SUCCESS) {
            break;
        }

        if (!is_hidd_app_register()) {
            BTC_TRACE_ERROR("%s: application has not been registered, shall register first!", __func__);
            ret = ESP_HIDD_NEED_REG;
            break;
        }

        BTA_HdVirtualCableUnplug();

        if (btc_hd_cb.status == BTC_HD_CONNECTED) {
            btc_hd_cb.status = BTC_HD_DISCONNECTING;
        }
    } while (0);

    if (ret != ESP_HIDD_SUCCESS) {
        esp_hidd_cb_param_t param = {0};
        param.vc_unplug.status = ret;
        param.vc_unplug.conn_status = ESP_HIDD_CONN_STATE_DISCONNECTED;
        btc_hd_cb_to_app(ESP_HIDD_VC_UNPLUG_EVT, &param);
    }
}

static void btc_hd_call_arg_deep_free(btc_msg_t *msg)
{
    btc_hidd_args_t *arg = (btc_hidd_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_HD_SEND_REPORT_EVT:
        utl_freebuf((void **)&arg->send_report.data);
        break;
    default:
        break;
    }
}

void btc_hd_call_handler(btc_msg_t *msg)
{
    btc_hidd_args_t *arg = (btc_hidd_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_HD_INIT_EVT:
        btc_hd_init();
        break;
    case BTC_HD_DEINIT_EVT:
        btc_hd_deinit();
        break;
    case BTC_HD_REGISTER_APP_EVT:
        btc_hd_register_app(arg->register_app.app_param, arg->register_app.in_qos, arg->register_app.out_qos);
        break;
    case BTC_HD_UNREGISTER_APP_EVT:
        btc_hd_unregister_app(false);
        break;
    case BTC_HD_CONNECT_EVT:
        btc_hd_connect(arg->connect.bd_addr);
        break;
    case BTC_HD_DISCONNECT_EVT:
        btc_hd_disconnect();
        break;
    case BTC_HD_SEND_REPORT_EVT:
        btc_hd_send_report(arg->send_report.type, arg->send_report.id, arg->send_report.len, arg->send_report.data);
        break;
    case BTC_HD_REPORT_ERROR_EVT:
        btc_hd_report_error(arg->error);
        break;
    case BTC_HD_UNPLUG_EVT:
        btc_hd_virtual_cable_unplug();
        break;
    default:
        BTC_TRACE_WARNING("unknown hidd action %i", msg->act);
        break;
    }
    btc_hd_call_arg_deep_free(msg);
}

void btc_hd_cb_arg_deep_free(btc_msg_t *msg)
{
    tBTA_HD *arg = (tBTA_HD *)msg->arg;

    switch (msg->act) {
    case BTA_HD_SET_REPORT_EVT:
        utl_freebuf((void **)&arg->set_report.p_data);
        break;
    case BTA_HD_INTR_DATA_EVT:
        utl_freebuf((void **)&arg->intr_data.p_data);
        break;
    default:
        break;
    }
}

void btc_hd_cb_handler(btc_msg_t *msg)
{
    uint16_t event = msg->act;
    tBTA_HD *p_data = (tBTA_HD *)msg->arg;
    esp_hidd_cb_param_t param = {0};
    BTC_TRACE_API("%s: event=%s", __func__, dump_hd_event(event));

    switch (event) {
    case BTA_HD_ENABLE_EVT:
        BTC_TRACE_DEBUG("%s: status=%d", __func__, p_data->status);
        if (p_data->status == BTA_HD_OK) {
            btc_storage_load_hidd();
            btc_hd_cb.status = BTC_HD_ENABLED;
        } else {
            btc_hd_cb.status = BTC_HD_DISABLED;
            BTC_TRACE_WARNING("Failed to enable BT-HD, status=%d", p_data->status);
        }
        param.init.status = p_data->status;
        btc_hd_cb_to_app(ESP_HIDD_INIT_EVT, &param);
        break;
    case BTA_HD_DISABLE_EVT:
        BTC_TRACE_DEBUG("%s: status=%d", __func__, p_data->status);
        if (p_data->status == BTA_HD_OK){
            btc_hd_cb.status = BTC_HD_DISABLED;
            if (btc_hd_cb.service_dereg_active) {
                btc_hd_cb.service_dereg_active = FALSE;
            }
            free_app_info_param();
            memset(&btc_hd_cb, 0, sizeof(btc_hd_cb));
        } else {
            BTC_TRACE_WARNING("Failed to disable BT-HD, status=%d", p_data->status);
        }
        param.deinit.status = p_data->status;
        btc_hd_cb_to_app(ESP_HIDD_DEINIT_EVT, &param);
        break;
    case BTA_HD_REGISTER_APP_EVT:
        if (p_data->reg_status.status == BTA_HD_OK) {
            btc_hd_cb.app_registered = TRUE;
        }
        param.register_app.status = p_data->reg_status.status;
        param.register_app.in_use = p_data->reg_status.in_use;
        if (!p_data->reg_status.in_use) {
            memset(param.register_app.bd_addr, 0, BD_ADDR_LEN);
        } else {
            memcpy(param.register_app.bd_addr, p_data->reg_status.bda, BD_ADDR_LEN);
        }
        btc_hd_cb_to_app(ESP_HIDD_REGISTER_APP_EVT, &param);
        break;
    case BTA_HD_UNREGISTER_APP_EVT:
        btc_hd_cb.app_registered = FALSE;
        param.unregister_app.status = p_data->status;
        btc_hd_cb_to_app(ESP_HIDD_UNREGISTER_APP_EVT, &param);
        if (btc_hd_cb.status == BTC_HD_DISABLING) {
            BTC_TRACE_WARNING("disabling hid device service now");
            BTA_HdDisable();
        }
        break;
    case BTA_HD_OPEN_EVT: {
        bt_bdaddr_t *addr = (bt_bdaddr_t *)&p_data->conn.bda;
        BTC_TRACE_EVENT("BTA_HD_OPEN_EVT, address (%02x:%02x:%02x:%02x:%02x:%02x)", addr->address[0], addr->address[1],
                        addr->address[2], addr->address[3], addr->address[4], addr->address[5]);
        if (p_data->conn.status == BTA_HD_OK && p_data->conn.conn_status == BTA_HD_CONN_STATE_CONNECTED) {
            // /* Check if the connection is from hid host and not hid device */
            // if (check_cod_hid(addr)) {
            //     /* Incoming connection from hid device, reject it */
            //     BTC_TRACE_WARNING("remote device is not hid host, disconnecting");
            //     btc_hd_cb.forced_disc = TRUE;
            //     BTA_HdDisconnect();
            //     break;
            // }
            // btc_storage_set_hidd((bt_bdaddr_t *)&p_data->conn.bda);
            btc_hd_cb.status = BTC_HD_CONNECTED;
        }
        param.open.status = p_data->conn.status;
        param.open.conn_status = p_data->conn.conn_status;
        memcpy(param.open.bd_addr, p_data->conn.bda, BD_ADDR_LEN);
        btc_hd_cb_to_app(ESP_HIDD_OPEN_EVT, &param);
        break;
    }
    case BTA_HD_CLOSE_EVT:
        if (p_data->conn.conn_status == BTA_HD_CONN_STATE_DISCONNECTED) {
            btc_hd_cb.status = BTC_HD_DISCONNECTED;
            if (btc_hd_cb.forced_disc) {
                bt_bdaddr_t *addr = (bt_bdaddr_t *)&p_data->conn.bda;
                BTC_TRACE_WARNING("remote device was forcefully disconnected");
                btc_hd_remove_device(*addr);
                btc_hd_cb.forced_disc = FALSE;
                break;
            }
        }

        param.close.status = p_data->conn.status;
        param.close.conn_status = p_data->conn.conn_status;
        btc_hd_cb_to_app(ESP_HIDD_CLOSE_EVT, &param);
        break;
    case BTA_HD_GET_REPORT_EVT:
        param.get_report.report_type = p_data->get_report.report_type;
        param.get_report.report_id = p_data->get_report.report_id;
        param.get_report.buffer_size = p_data->get_report.buffer_size;
        btc_hd_cb_to_app(ESP_HIDD_GET_REPORT_EVT, &param);
        break;
    case BTA_HD_SET_REPORT_EVT:
        param.set_report.report_type = p_data->set_report.report_type;
        param.set_report.report_id = p_data->set_report.report_id;
        param.set_report.len = p_data->set_report.len;
        param.set_report.data = p_data->set_report.p_data;
        btc_hd_cb_to_app(ESP_HIDD_SET_REPORT_EVT, &param);
        break;
    case BTA_HD_SET_PROTOCOL_EVT:
        switch (p_data->set_protocol) {
        case HID_PAR_PROTOCOL_BOOT_MODE:
            param.set_protocol.protocol_mode = ESP_HIDD_BOOT_MODE;
            break;
        case HID_PAR_PROTOCOL_REPORT:
            param.set_protocol.protocol_mode = ESP_HIDD_REPORT_MODE;
            break;
        default:
            param.set_protocol.protocol_mode = ESP_HIDD_UNSUPPORTED_MODE;
            break;
        }
        btc_hd_cb_to_app(ESP_HIDD_SET_PROTOCOL_EVT, &param);
        break;
    case BTA_HD_INTR_DATA_EVT:
        param.intr_data.report_id = p_data->intr_data.report_id;
        param.intr_data.len = p_data->intr_data.len;
        param.intr_data.data = p_data->intr_data.p_data;
        btc_hd_cb_to_app(ESP_HIDD_INTR_DATA_EVT, &param);
        break;
    case BTA_HD_VC_UNPLUG_EVT: {
        bt_bdaddr_t *bd_addr = (bt_bdaddr_t *)&p_data->conn.bda;
        if (bta_dm_check_if_only_hd_connected(p_data->conn.bda)) {
            BTC_TRACE_DEBUG("%s: Removing bonding as only HID profile connected", __func__);
            BTA_DmRemoveDevice((uint8_t *)&p_data->conn.bda, BT_TRANSPORT_BR_EDR);
        } else {
            BTC_TRACE_DEBUG("%s: Only removing HID data as some other profiles connected", __func__);
            btc_hd_remove_device(*bd_addr);
        }

        if (btc_hd_cb.status == BTC_HD_DISCONNECTING || btc_hd_cb.status == BTC_HD_CONNECTING ||
            btc_hd_cb.status == BTC_HD_CONNECTED) {
            btc_hd_cb.status = BTC_HD_DISCONNECTED;
            param.close.status = p_data->conn.status;
            param.close.conn_status = p_data->conn.conn_status;
            btc_hd_cb_to_app(ESP_HIDD_CLOSE_EVT, &param);
        }

        param.vc_unplug.status = p_data->conn.status;
        param.vc_unplug.conn_status = p_data->conn.conn_status;
        btc_hd_cb_to_app(ESP_HIDD_VC_UNPLUG_EVT, &param);
        break;
    }
    case BTA_HD_SEND_REPORT_EVT:
        param.send_report.status = p_data->send_report.status;
        param.send_report.reason = p_data->send_report.reason;
        param.send_report.report_type = p_data->send_report.report_type;
        param.send_report.report_id = p_data->send_report.report_id;
        btc_hd_cb_to_app(ESP_HIDD_SEND_REPORT_EVT, &param);
        break;
    case BTA_HD_REPORT_ERR_EVT:
        param.report_err.status = p_data->report_err.status;
        param.report_err.reason = p_data->report_err.reason;
        btc_hd_cb_to_app(ESP_HIDD_REPORT_ERR_EVT, &param);
        break;
    default:
        BTC_TRACE_WARNING("%s: unknown event (%d)", __func__, event);
        break;
    }
    btc_hd_cb_arg_deep_free(msg);
}

void btc_hd_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_hidd_args_t *dst = (btc_hidd_args_t *)p_dest;
    btc_hidd_args_t *src = (btc_hidd_args_t *)p_src;

    switch (msg->act) {
    case BTC_HD_SEND_REPORT_EVT:
        dst->send_report.data = (uint8_t *)osi_malloc(src->send_report.len);
        if (dst->send_report.data) {
            memcpy(dst->send_report.data, src->send_report.data, src->send_report.len);
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
        }
        break;
    default:
        break;
    }
}

#endif // HID_DEV_INCLUDED==TRUE
