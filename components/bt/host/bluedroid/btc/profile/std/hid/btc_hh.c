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
#include "btc/btc_util.h"
#include "btc/btc_manage.h"
#include "device/bdaddr.h"
#include "btc/btc_storage.h"
#include "osi/allocator.h"
#include "bta/utl.h"
#include "bta/bta_hh_api.h"
#include "stack/l2c_api.h"
// #include "bta_dm_int.h"

#if HID_HOST_INCLUDED == TRUE
#include "btc_hh.h"


/*******************************************************************************
 *  Static variables
 ******************************************************************************/
btc_hh_cb_t btc_hh_cb;
static bdstr_t bdstr;

/******************************************************************************
 *  Constants & Macros
 *****************************************************************************/
#define COD_MASK 0x07FF

#define COD_UNCLASSIFIED ((0x1F) << 8)
#define COD_HID_KEYBOARD 0x0540
#define COD_HID_POINTING 0x0580
#define COD_HID_COMBO 0x05C0
#define COD_HID_MAJOR 0x0500
#define COD_HID_MASK 0x0700

#define is_hidh_init() (btc_hh_cb.status > BTC_HH_DISABLED)
#define BTC_TIMEOUT_VUP_MS (3 * 1000)

static inline void btc_hh_cb_to_app(esp_hidh_cb_event_t event, esp_hidh_cb_param_t *param)
{
    esp_hh_cb_t btc_hh_cb = (esp_hh_cb_t)btc_profile_cb_get(BTC_PID_HH);
    if (btc_hh_cb) {
        btc_hh_cb(event, param);
    }
}

/*******************************************************************************
 *
 * Function         proto_mode_change_to_lower_layer
 *
 * Description      Change the upper layer protocol mode definition to the lower layer protocol mode definition
 *
 * Returns          Lower layer protocol mode definition
 ******************************************************************************/
static inline tBTA_HH_PROTO_MODE proto_mode_change_to_lower_layer(esp_hidh_protocol_mode_t protocol_mode)
{
    tBTA_HH_PROTO_MODE proto_mode = BTA_HH_PROTO_UNKNOWN;

    switch (protocol_mode) {
    case ESP_HIDH_REPORT_MODE:
        proto_mode = BTA_HH_PROTO_RPT_MODE;
        break;
    case ESP_HIDH_BOOT_MODE:
        proto_mode = BTA_HH_PROTO_BOOT_MODE;
        break;
    default:
        break;
    }

    return proto_mode;
}

/*******************************************************************************
 *
 * Function         proto_mode_change_to_upper_layer
 *
 * Description      Change the lower layer protocol mode definition to the upper layer protocol mode definition
 *
 * Returns          Upper layer protocol mode definition
 ******************************************************************************/
static inline esp_hidh_protocol_mode_t proto_mode_change_to_upper_layer(tBTA_HH_PROTO_MODE proto_mode)
{
    esp_hidh_protocol_mode_t protocol_mode = ESP_HIDH_UNSUPPORTED_MODE;

    switch (proto_mode) {
    case BTA_HH_PROTO_RPT_MODE:
        protocol_mode = ESP_HIDH_REPORT_MODE;
        break;
    case BTA_HH_PROTO_BOOT_MODE:
        protocol_mode = ESP_HIDH_BOOT_MODE;
        break;
    default:
        break;
    }

    return protocol_mode;
}

/*******************************************************************************
 *
 * Function         btc_hh_find_connected_dev_by_handle
 *
 * Description      Return the connected device pointer of the specified device
 *                  handle
 *
 * Returns          Device entry pointer in the device table
 ******************************************************************************/
btc_hh_device_t *btc_hh_find_connected_dev_by_handle(uint8_t handle)
{
    uint32_t i;
    for (i = 0; i < BTC_HH_MAX_HID; i++) {
        if (btc_hh_cb.devices[i].dev_status == ESP_HIDH_CONN_STATE_CONNECTED && btc_hh_cb.devices[i].dev_handle == handle) {
            return &btc_hh_cb.devices[i];
        }
    }
    return NULL;
}

/*******************************************************************************
 *
 * Function         btc_hh_find_dev_by_bda
 *
 * Description      Return the device pointer of the specified bd_addr.
 *
 * Returns          Device entry pointer in the device table
 ******************************************************************************/
static btc_hh_device_t *btc_hh_find_dev_by_bda(BD_ADDR bd_addr)
{
    uint8_t i;
    for (i = 0; i < BTC_HH_MAX_HID; i++) {
        if (btc_hh_cb.devices[i].dev_status != ESP_HIDH_CONN_STATE_UNKNOWN &&
            memcmp(btc_hh_cb.devices[i].bd_addr, bd_addr, BD_ADDR_LEN) == 0) {
            return &btc_hh_cb.devices[i];
        }
    }
    return NULL;
}

/*******************************************************************************
 *
 * Function         btc_hh_find_connected_dev_by_bda
 *
 * Description      Return the connected device pointer of the specified
 *                  RawAddress.
 *
 * Returns          Device entry pointer in the device table
 ******************************************************************************/
static btc_hh_device_t *btc_hh_find_connected_dev_by_bda(BD_ADDR bd_addr)
{
    uint32_t i;
    for (i = 0; i < BTC_HH_MAX_HID; i++) {
        if (btc_hh_cb.devices[i].dev_status == ESP_HIDH_CONN_STATE_CONNECTED &&
            memcmp(btc_hh_cb.devices[i].bd_addr, bd_addr, BD_ADDR_LEN) == 0) {
            return &btc_hh_cb.devices[i];
        }
    }
    return NULL;
}

/*******************************************************************************
 *
 * Function         btc_hh_stop_vup_timer
 *
 * Description      stop vitual unplug timer
 *
 * Returns          void
 ******************************************************************************/
void btc_hh_stop_vup_timer(BD_ADDR bd_addr)
{
    BTIF_TRACE_API("%s", __func__);
    btc_hh_device_t *p_dev = btc_hh_find_connected_dev_by_bda(bd_addr);

    if (p_dev != NULL) {
        BTC_TRACE_DEBUG("stop VUP timer");
        if (p_dev->vup_timer) {
            osi_alarm_free(p_dev->vup_timer);
            p_dev->vup_timer = NULL;
        }
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_timer_timeout
 *
 * Description      Process timer timeout
 *
 * Returns          void
 ******************************************************************************/
void btc_hh_timer_timeout(void *data)
{
    btc_hh_device_t *p_dev = (btc_hh_device_t *)data;
    bt_status_t status;
    tBTA_HH p_data;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HH;
    msg.act = BTA_HH_VC_UNPLUG_EVT;

    BTC_TRACE_API("%s", __func__);
    if (p_dev->dev_status != ESP_HIDH_CONN_STATE_CONNECTED){
        BTC_TRACE_ERROR("%s Device[%s] is not connected!", __func__,
                        bdaddr_to_string((const bt_bdaddr_t *)p_dev->bd_addr, bdstr, sizeof(bdstr)));
        return;
    }

    memset(&p_data, 0, sizeof(tBTA_HH));
    p_data.dev_status.status = BTA_HH_ERR;
    p_data.dev_status.handle = p_dev->dev_handle;

    /* switch context to btif task context */
    status = btc_transfer_context(&msg, &p_data, sizeof(tBTA_HH), NULL, NULL);
    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s context transfer failed", __func__);
    }
}

/*******************************************************************************
 *
 * Function      btc_hh_start_vup_timer
 *
 * Description  start virtual unplug timer
 *
 * Returns      void
 ******************************************************************************/
void btc_hh_start_vup_timer(BD_ADDR bd_addr)
{
    BTC_TRACE_API("%s", __func__);

    btc_hh_device_t *p_dev = btc_hh_find_connected_dev_by_bda(bd_addr);
    if (p_dev == NULL) {
        BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
        return;
    }

    if (p_dev->vup_timer) {
        osi_alarm_free(p_dev->vup_timer);
        p_dev->vup_timer = NULL;
    }
    if ((p_dev->vup_timer = osi_alarm_new("btc_hh.vup_timer", btc_hh_timer_timeout, p_dev, BTC_TIMEOUT_VUP_MS)) ==
        NULL) {
        BTC_TRACE_ERROR("%s unable to malloc vup_timer!", __func__);
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_add_added_dev
 *
 * Description      Add a new device to the added device list.
 *
 * Returns          true if add successfully, otherwise false.
 ******************************************************************************/
bool btc_hh_add_added_dev(BD_ADDR bd_addr, tBTA_HH_ATTR_MASK attr_mask)
{
    int i;
    for (i = 0; i < BTC_HH_MAX_ADDED_DEV; i++) {
        if (memcmp(btc_hh_cb.added_devices[i].bd_addr, bd_addr, BD_ADDR_LEN) == 0) {
            return false;
        }
    }
    for (i = 0; i < BTC_HH_MAX_ADDED_DEV; i++) {
        if (memcmp(btc_hh_cb.added_devices[i].bd_addr, bd_addr_null, BD_ADDR_LEN) == 0) {
            memcpy(btc_hh_cb.added_devices[i].bd_addr, bd_addr, BD_ADDR_LEN);
            btc_hh_cb.added_devices[i].dev_handle = BTA_HH_INVALID_HANDLE;
            btc_hh_cb.added_devices[i].attr_mask = attr_mask;
            return true;
        }
    }

    BTC_TRACE_ERROR("%s: Error, out of space to add device", __func__);
    return false;
}

/*******************************************************************************
 **
 ** Function         btc_hh_remove_device
 **
 ** Description      Remove an added device from the stack.
 **
 ** Returns          void
 ******************************************************************************/
void btc_hh_remove_device(BD_ADDR bd_addr)
{
    int i;
    btc_hh_device_t *p_dev;
    btc_hh_added_device_t *p_added_dev;

    for (i = 0; i < BTC_HH_MAX_ADDED_DEV; i++) {
        p_added_dev = &btc_hh_cb.added_devices[i];
        if (p_added_dev->bd_addr == bd_addr) {
            BTA_HhRemoveDev(p_added_dev->dev_handle);
            btc_storage_remove_hid_info((bt_bdaddr_t *)p_added_dev->bd_addr);
            memset(p_added_dev->bd_addr, 0, 6);
            p_added_dev->dev_handle = BTA_HH_INVALID_HANDLE;
            break;
        }
    }

    p_dev = btc_hh_find_dev_by_bda(bd_addr);
    if (p_dev == NULL) {
        BTC_TRACE_ERROR("%s Oops, can't find device", __func__);
        return;
    }

    /* need to notify up-layer device is disconnected to avoid state out of sync
     * with up-layer */ //[boblane]
    // HAL_CBACK(bt_hh_callbacks, connection_state_cb, &(p_dev->bd_addr), BTHH_CONN_STATE_DISCONNECTED);

    p_dev->dev_status = ESP_HIDH_CONN_STATE_UNKNOWN;
    p_dev->dev_handle = BTA_HH_INVALID_HANDLE;
    p_dev->ready_for_data = false;

    if (btc_hh_cb.device_num > 0) {
        btc_hh_cb.device_num--;
    } else {
        BTC_TRACE_WARNING("%s: device_num = 0", __func__);
    }
}

static void bte_hh_arg_deep_copy(btc_msg_t *msg, void *p_dst, void *p_src)
{
    tBTA_HH *p_dst_data = (tBTA_HH *)p_dst;
    tBTA_HH *p_src_data = (tBTA_HH *)p_src;
    switch (msg->act)
    {
    case BTA_HH_GET_RPT_EVT: {
        BT_HDR *src_hdr = p_src_data->hs_data.rsp_data.p_rpt_data;
        if (src_hdr) {
            p_dst_data->hs_data.rsp_data.p_rpt_data = osi_malloc(sizeof(BT_HDR) + src_hdr->offset + src_hdr->len);
            if (p_dst_data->hs_data.rsp_data.p_rpt_data == NULL) {
                BTC_TRACE_ERROR("%s malloc p_rpt_data failed!", __func__);
                p_dst_data->hs_data.status = ESP_HIDH_ERR_NO_RES;
                break;
            }
            memcpy(p_dst_data->hs_data.rsp_data.p_rpt_data, src_hdr, sizeof(BT_HDR) + src_hdr->offset + src_hdr->len);
        }
        break;
    }
    default:
        break;
    }
}

static void bte_hh_evt(tBTA_HH_EVT event, tBTA_HH *p_data)
{
    bt_status_t status;
    int param_len = 0;

    BTC_TRACE_API("%s event=%d", __func__, event);

    switch (event) {
    case BTA_HH_ENABLE_EVT:
        param_len = sizeof(tBTA_HH_STATUS);
        break;
    case BTA_HH_DISABLE_EVT:
        param_len = sizeof(tBTA_HH_STATUS);
        break;
    case BTA_HH_OPEN_EVT:
        param_len = sizeof(tBTA_HH_CONN);
        break;
    case BTA_HH_CLOSE_EVT:
        param_len = sizeof(tBTA_HH_CBDATA);
        break;
    case BTA_HH_GET_RPT_EVT:
        param_len = sizeof(tBTA_HH_HSDATA);
        break;
    case BTA_HH_SET_RPT_EVT:
        param_len = sizeof(tBTA_HH_CBDATA);
        break;
    case BTA_HH_GET_PROTO_EVT:
        param_len = sizeof(tBTA_HH_HSDATA);
        break;
    case BTA_HH_SET_PROTO_EVT:
        param_len = sizeof(tBTA_HH_CBDATA);
        break;
    case BTA_HH_GET_IDLE_EVT:
        param_len = sizeof(tBTA_HH_HSDATA);
        break;
    case BTA_HH_SET_IDLE_EVT:
        param_len = sizeof(tBTA_HH_CBDATA);
        break;
    case BTA_HH_GET_DSCP_EVT:
        param_len = sizeof(tBTA_HH_DEV_DSCP_INFO);
        break;
    case BTA_HH_ADD_DEV_EVT:
        param_len = sizeof(tBTA_HH_DEV_INFO);
        break;
    case BTA_HH_RMV_DEV_EVT:
        param_len = sizeof(tBTA_HH_DEV_INFO);
        break;
    case BTA_HH_VC_UNPLUG_EVT:
        param_len = sizeof(tBTA_HH_CBDATA);
        break;
    case BTA_HH_DATA_EVT:
        param_len = sizeof(tBTA_HH_API_SENDDATA);
        break;
    case BTA_HH_API_ERR_EVT:
        param_len = 0;
        break;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_HH;
    msg.act = event;

    status = btc_transfer_context(&msg, p_data, param_len, bte_hh_arg_deep_copy, btc_hh_cb_arg_deep_free);
    assert(status == BT_STATUS_SUCCESS);
}

/*******************************************************************************
 *
 * Function         btc_hh_init
 *
 * Description      initializes the hh interface
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_init(void)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    do {
        if (is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has been initiated, shall uninit first!", __func__);
            ret = ESP_HIDH_NEED_DEINIT;
            break;
        }

        memset(&btc_hh_cb, 0, sizeof(btc_hh_cb));
        for (uint8_t i = 0; i < BTC_HH_MAX_HID; i++) {
            btc_hh_cb.devices[i].dev_status = ESP_HIDH_CONN_STATE_UNKNOWN;
        }
        BTA_HhEnable(BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT, bte_hh_evt);
    } while (0);

    if (ret != ESP_HIDH_OK) {
        esp_hidh_cb_param_t param;
        param.init.status = ret;
        btc_hh_cb_to_app(ESP_HIDH_INIT_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_deinit
 *
 * Description      de-initializes the hh interface
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_deinit(void)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        // close all connections
        for (uint8_t i = 0; i < BTC_HH_MAX_HID; i++) {
            if(btc_hh_cb.devices[i].dev_status == ESP_HIDH_CONN_STATE_CONNECTED){
                BTA_HhClose(btc_hh_cb.devices[i].dev_handle);
            }
        }
        btc_hh_cb.service_dereg_active = TRUE;
        btc_hh_cb.status = BTC_HH_DISABLING;
        BTA_HhDisable();
    } while (0);

    if (ret != ESP_HIDH_OK) {
        esp_hidh_cb_param_t param;
        param.deinit.status = ret;
        btc_hh_cb_to_app(ESP_HIDH_DEINIT_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_connect
 *
 * Description      connection initiated from the BTC thread context
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_connect(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    btc_hh_added_device_t* added_dev = NULL;
    btc_hh_device_t* dev = NULL;
    esp_hidh_cb_param_t param;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        if (btc_hh_cb.status == BTC_HH_DEV_CONNECTING) {
            BTC_TRACE_ERROR("%s HH is connecting, ignore!", __func__);
            ret = ESP_HIDH_BUSY;
            break;
        }

        dev = btc_hh_find_dev_by_bda(arg->connect.bd_addr);
        if (!dev && btc_hh_cb.device_num >= BTC_HH_MAX_HID) {
            BTC_TRACE_ERROR("%s exceeded the maximum supported HID device number %d!", __func__, BTC_HH_MAX_HID);
            ret = ESP_HIDH_ERR_NO_RES;
            break;
        }

        for (int i = 0; i < BTC_HH_MAX_ADDED_DEV; i++) {
            if (memcmp(btc_hh_cb.added_devices[i].bd_addr, arg->connect.bd_addr, BD_ADDR_LEN) == 0) {
                added_dev = &btc_hh_cb.added_devices[i];
                BTC_TRACE_WARNING("%s Device[%s] already added, attr_mask = 0x%x", __func__,
                                  bdaddr_to_string((const bt_bdaddr_t *)arg->connect.bd_addr, bdstr, sizeof(bdstr)),
                                  added_dev->attr_mask);
            }
        }

        if (added_dev != NULL) {
            if (added_dev->dev_handle == BTA_HH_INVALID_HANDLE) {
                // No space for more HID device now.
                BTC_TRACE_ERROR("device added but addition failed");
                memset(added_dev->bd_addr, 0, sizeof(added_dev->bd_addr));
                ret = ESP_HIDH_ERR;
                break;
            }
        }

        /* Not checking the NORMALLY_Connectible flags from sdp record, and anyways
        sending this request from host, for subsequent user initiated connection. If the remote is
        not in pagescan mode, we will do 2 retries to connect before giving up */
        btc_hh_cb.status = BTC_HH_DEV_CONNECTING;
        memcpy(btc_hh_cb.pending_conn_address, arg->connect.bd_addr, BD_ADDR_LEN);
        BTA_HhOpen(arg->connect.bd_addr, BTA_HH_PROTO_RPT_MODE, (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT));
        param.open.conn_status = ESP_HIDH_CONN_STATE_CONNECTING;
        ret = ESP_HIDH_OK;
    } while (0);

    if (ret != ESP_HIDH_OK) {
        param.open.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
    }
    param.open.status = ret;
    param.open.handle = BTA_HH_INVALID_HANDLE;
    memcpy(param.open.bd_addr, arg->connect.bd_addr, BD_ADDR_LEN);
    param.open.is_orig = true;
    btc_hh_cb_to_app(ESP_HIDH_OPEN_EVT, &param);
}

/*******************************************************************************
 *
 * Function         btc_hh_disconnect
 *
 * Description      disconnection initiated from the BTC thread context
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_disconnect(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    btc_hh_device_t *p_dev;
    esp_hidh_cb_param_t param;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }
        p_dev = btc_hh_find_connected_dev_by_bda(arg->disconnect.bd_addr);
        if (p_dev != NULL) {
            BTA_HhClose(p_dev->dev_handle);
            param.close.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTING;
            param.close.handle = p_dev->dev_handle;
        } else {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
        }

    } while (0);

    if (ret != ESP_HIDH_OK) {
        param.close.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
        param.close.handle = BTA_HH_INVALID_HANDLE;
    }
    param.close.status = ret;
    btc_hh_cb_to_app(ESP_HIDH_CLOSE_EVT, &param);
}

/*******************************************************************************
 *
 * Function         btc_hh_virtual_unplug
 *
 * Description      Virtual unplug initiated from the BTC thread context
 *                  Special handling for HID mouse-
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_virtual_unplug(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    btc_hh_device_t *p_dev;
    esp_hidh_cb_param_t param;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }
        p_dev = btc_hh_find_dev_by_bda(arg->unplug.bd_addr);
        if ((p_dev != NULL) && (p_dev->dev_status == ESP_HIDH_CONN_STATE_CONNECTED) &&
            (p_dev->attr_mask & HID_VIRTUAL_CABLE)) {
            BTC_TRACE_DEBUG("%s: Sending BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG", __func__);
            /* start the timer */
            btc_hh_start_vup_timer(arg->unplug.bd_addr);
            p_dev->local_vup = true;
            BTA_HhSendCtrl(p_dev->dev_handle, BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG);

            param.unplug.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTING;
            param.unplug.handle = p_dev->dev_handle;
        } else if ((p_dev != NULL) && (p_dev->dev_status == ESP_HIDH_CONN_STATE_CONNECTED)) {
            BTC_TRACE_WARNING("%s: Virtual unplug not suported, disconnecting device", __func__);
            /* start the timer */
            btc_hh_start_vup_timer(arg->unplug.bd_addr);
            p_dev->local_vup = true;
            BTA_HhClose(p_dev->dev_handle);

            param.unplug.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTING;
            param.unplug.handle = p_dev->dev_handle;
        } else {
            BTC_TRACE_ERROR("%s: Error, device not opened, status = %d", __func__, btc_hh_cb.status);
            ret = ESP_HIDH_NO_CONNECTION;
            if (memcmp(btc_hh_cb.pending_conn_address, arg->unplug.bd_addr, BD_ADDR_LEN) == 0 &&
                (btc_hh_cb.status == BTC_HH_DEV_CONNECTING)) {
                btc_hh_cb.status = (BTC_HH_STATUS)BTC_HH_DEV_DISCONNECTED;
                memset(btc_hh_cb.pending_conn_address, 0, BD_ADDR_LEN);
            }
        }
    } while (0);

    if (ret != ESP_HIDH_OK) {
        param.unplug.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
        param.unplug.handle = BTA_HH_INVALID_HANDLE;
    }
    param.unplug.status = ret;
    btc_hh_cb_to_app(ESP_HIDH_VC_UNPLUG_EVT, &param);
}

/*******************************************************************************
 *
 * Function         btc_hh_set_info
 *
 * Description      Set the HID device descriptor for the specified HID device.
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_set_info(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    tBTA_HH_DEV_DSCP_INFO dscp_info;

    BTC_TRACE_DEBUG("%s: sub_class = 0x%02x, app_id = %d, vendor_id = 0x%04x, "
                    "product_id = 0x%04x, version= 0x%04x",
                    __func__, arg->set_info.hid_info->sub_class, arg->set_info.hid_info->app_id,
                    arg->set_info.hid_info->vendor_id, arg->set_info.hid_info->product_id,
                    arg->set_info.hid_info->version);
    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        memset(&dscp_info, 0, sizeof(dscp_info));
        dscp_info.vendor_id = arg->set_info.hid_info->vendor_id;
        dscp_info.product_id = arg->set_info.hid_info->product_id;
        dscp_info.version = arg->set_info.hid_info->version;
        dscp_info.ctry_code = arg->set_info.hid_info->ctry_code;

        dscp_info.descriptor.dl_len = arg->set_info.hid_info->dl_len;
        dscp_info.descriptor.dsc_list = (uint8_t *)osi_malloc(dscp_info.descriptor.dl_len);
        if (dscp_info.descriptor.dsc_list == NULL) {
            BTC_TRACE_ERROR("%s malloc dsc_list failed!", __func__);
            ret = ESP_HIDH_ERR_NO_RES;
            break;
        }
        memcpy(dscp_info.descriptor.dsc_list, arg->set_info.hid_info->dsc_list, dscp_info.descriptor.dl_len);

        if (btc_hh_add_added_dev(arg->set_info.bd_addr, arg->set_info.hid_info->attr_mask)) {
            btc_hh_cb.add_event = BTC_HH_SET_INFO_EVT;
            BTA_HhAddDev(arg->set_info.bd_addr, arg->set_info.hid_info->attr_mask, arg->set_info.hid_info->sub_class,
                         arg->set_info.hid_info->app_id, dscp_info);
        } else {
            BTC_TRACE_ERROR("%s malloc dsc_list failed!", __func__);
            ret = ESP_HIDH_ERR;
            break;
        }
    } while(0);
    utl_freebuf((void **)&dscp_info.descriptor.dsc_list);

    if (ret != ESP_HIDH_OK) {
        param.set_info.status = ret;
        param.set_info.handle = BTA_HH_INVALID_HANDLE;
        memcpy(param.set_info.bd_addr, arg->set_info.bd_addr, BD_ADDR_LEN);
        btc_hh_cb_to_app(ESP_HIDH_SET_INFO_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_get_protocol
 *
 * Description      Get the HID proto mode.
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_get_protocol(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    btc_hh_device_t *p_dev;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }
        p_dev = btc_hh_find_connected_dev_by_bda(arg->get_protocol.bd_addr);
        if (p_dev == NULL) {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
            break;
        }
        BTA_HhGetProtoMode(p_dev->dev_handle);
    } while(0);

    if (ret != ESP_HIDH_OK) {
        param.get_proto.proto_mode = ESP_HIDH_UNSUPPORTED_MODE;
        param.get_proto.handle = BTA_HH_INVALID_HANDLE;
        param.get_proto.status = ret;
        btc_hh_cb_to_app(ESP_HIDH_GET_PROTO_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_set_protocol
 *
 * Description      Set the HID proto mode.
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_set_protocol(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    btc_hh_device_t *p_dev;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }
        p_dev = btc_hh_find_connected_dev_by_bda(arg->set_protocol.bd_addr);
        if (p_dev == NULL) {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
            break;
        } else if (arg->set_protocol.protocol_mode != ESP_HIDH_REPORT_MODE && arg->set_protocol.protocol_mode != ESP_HIDH_BOOT_MODE) {
            BTC_TRACE_ERROR("%s: Error, device proto_mode = %d.", __func__, arg->set_protocol.protocol_mode);
            ret = ESP_HIDH_HS_INVALID_PARAM;
            break;
        } else {
            BTA_HhSetProtoMode(p_dev->dev_handle, proto_mode_change_to_lower_layer(arg->set_protocol.protocol_mode));
        }
    } while (0);

    if (ret != ESP_HIDH_OK) {
        param.set_proto.handle = BTA_HH_INVALID_HANDLE;
        param.set_proto.status = ret;
        btc_hh_cb_to_app(ESP_HIDH_SET_PROTO_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_get_report
 *
 * Description      Send a GET_REPORT to HID device.
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_get_report(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    btc_hh_device_t *p_dev;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        p_dev = btc_hh_find_connected_dev_by_bda(arg->get_report.bd_addr);
        if (p_dev == NULL) {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
            break;
        } else if (((int)arg->get_report.report_type) <= BTA_HH_RPTT_RESRV ||
                   ((int)arg->get_report.report_type) > BTA_HH_RPTT_FEATURE) {
            BTC_TRACE_ERROR("%s Error: report type=%d not supported!", __func__, arg->get_report.report_type);
            ret = ESP_HIDH_HS_INVALID_PARAM;
            break;
        } else {
            BTA_HhGetReport(p_dev->dev_handle, arg->get_report.report_type, arg->get_report.report_id,
                            arg->get_report.buffer_size);
        }
    } while (0);

    if (ret != ESP_HIDH_OK) {
        param.get_rpt.handle = BTA_HH_INVALID_HANDLE;
        param.get_rpt.status = ret;
        param.get_rpt.len = 0;
        param.get_rpt.data = NULL;
        btc_hh_cb_to_app(ESP_HIDH_GET_RPT_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         create_pbuf
 *
 * Description      Helper function to create p_buf for send_data or set_report
 *
 * Returns          BT_HDR *
 *
 ******************************************************************************/
static BT_HDR *create_pbuf(uint16_t len, uint8_t *data)
{
    uint8_t *pbuf_data;
    BT_HDR *p_buf = (BT_HDR *)osi_malloc(len + BTA_HH_MIN_OFFSET + sizeof(BT_HDR));
    if (p_buf == NULL) {
        BTC_TRACE_ERROR("%s failed!", __func__);
        return NULL;
    }
    p_buf->len = len;
    p_buf->offset = BTA_HH_MIN_OFFSET;

    pbuf_data = (uint8_t *)(p_buf + 1) + p_buf->offset;
    memcpy(pbuf_data, data, len);

    return p_buf;
}

/*******************************************************************************
 *
 * Function         btc_hh_set_report
 *
 * Description      Send a SET_REPORT to HID device.
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_set_report(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    btc_hh_device_t *p_dev;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        p_dev = btc_hh_find_connected_dev_by_bda(arg->set_report.bd_addr);
        if (p_dev == NULL) {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
            break;
        } else if (((int)arg->set_report.report_type) <= BTA_HH_RPTT_RESRV ||
                   ((int)arg->set_report.report_type) > BTA_HH_RPTT_FEATURE) {
            BTC_TRACE_ERROR("%s Error: report type=%d not supported!", __func__, arg->set_report.report_type);
            ret = ESP_HIDH_HS_INVALID_PARAM;
            break;
        } else if (arg->set_report.report == NULL || arg->set_report.len == 0) {
            BTC_TRACE_ERROR("%s Error: report is empty!", __func__);
            ret = ESP_HIDH_HS_INVALID_PARAM;
            break;
        } else {
            BT_HDR* p_buf = create_pbuf(arg->set_report.len, arg->set_report.report);
            if (p_buf == NULL) {
                ret = ESP_HIDH_ERR_NO_RES;
                break;
            }
            BTA_HhSetReport(p_dev->dev_handle, arg->set_report.report_type, p_buf);
        }
    } while(0);

    if (ret != ESP_HIDH_OK) {
        param.set_rpt.handle = BTA_HH_INVALID_HANDLE;
        param.set_rpt.status = ret;
        btc_hh_cb_to_app(ESP_HIDH_SET_RPT_EVT, &param);
    }
}

/*******************************************************************************
 *
 * Function         btc_hh_send_data
 *
 * Description      Send a SEND_DATA to HID device.
 *
 * Returns          void
 *
 ******************************************************************************/
static void btc_hh_send_data(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    btc_hh_device_t *p_dev;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        p_dev = btc_hh_find_connected_dev_by_bda(arg->send_data.bd_addr);
        if (p_dev == NULL) {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
            break;
        } else if (arg->send_data.data == NULL || arg->send_data.len == 0) {
            BTC_TRACE_ERROR("%s Error: send data is empty!", __func__);
            ret = ESP_HIDH_HS_INVALID_PARAM;
            break;
        } else {
            BT_HDR *p_buf = create_pbuf(arg->send_data.len, arg->send_data.data);
            if (p_buf == NULL) {
                ret = ESP_HIDH_ERR_NO_RES;
                break;
            }
            p_buf->layer_specific = BTA_HH_RPTT_OUTPUT;
            BTA_HhSendData(p_dev->dev_handle, arg->send_data.bd_addr, p_buf);
        }
    } while(0);

    if (ret != ESP_HIDH_OK) {
        param.send_data.handle = BTA_HH_INVALID_HANDLE;
        param.send_data.status = ret;
        param.send_data.reason = 0;
        btc_hh_cb_to_app(ESP_HIDH_DATA_EVT, &param);
    }
}

/*******************************************************************************
**
** Function         btc_hh_get_idle_time
**
** Description      Get the HID idle time
**
** Returns          void
**
*******************************************************************************/
static void btc_hh_get_idle_time(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    btc_hh_device_t *p_dev;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        p_dev = btc_hh_find_connected_dev_by_bda(arg->get_idle.bd_addr);
        if (p_dev == NULL) {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
            break;
        }
        BTA_HhGetIdle(p_dev->dev_handle);
    } while (0);

    if (ret != ESP_HIDH_OK) {
        param.get_idle.handle = BTA_HH_INVALID_HANDLE;
        param.get_idle.status = ret;
        param.get_idle.idle_rate = 0;
        btc_hh_cb_to_app(ESP_HIDH_GET_IDLE_EVT, &param);
    }
}

/*******************************************************************************
**
** Function         btc_hh_set_idle_time
**
** Description      Set the HID idle time
**
** Returns          void
**
*******************************************************************************/
static void btc_hh_set_idle_time(btc_hidh_args_t *arg)
{
    BTC_TRACE_API("%s", __func__);
    esp_hidh_status_t ret = ESP_HIDH_OK;
    esp_hidh_cb_param_t param;
    btc_hh_device_t *p_dev;

    do {
        if (!is_hidh_init()) {
            BTC_TRACE_ERROR("%s HH has not been initiated, shall init first!", __func__);
            ret = ESP_HIDH_NEED_INIT;
            break;
        }

        p_dev = btc_hh_find_connected_dev_by_bda(arg->set_idle.bd_addr);
        if (p_dev == NULL) {
            ret = ESP_HIDH_NO_CONNECTION;
            BTC_TRACE_ERROR("%s Error: device not connected!", __func__);
            break;
        }
        BTA_HhSetIdle(p_dev->dev_handle, arg->set_idle.idle_time);
    } while (0);

    if (ret != ESP_HIDH_OK) {
        param.set_idle.handle = BTA_HH_INVALID_HANDLE;
        param.set_idle.status = ret;
        btc_hh_cb_to_app(ESP_HIDH_SET_IDLE_EVT, &param);
    }
}

static void btc_hh_call_arg_deep_free(btc_msg_t *msg)
{
    btc_hidh_args_t *arg = (btc_hidh_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_HH_SET_INFO_EVT:
        utl_freebuf((void **)&arg->set_info.hid_info);
        break;
    case BTC_HH_SET_REPORT_EVT:
        utl_freebuf((void **)&arg->set_report.report);
        break;
    case BTC_HH_SEND_DATA_EVT:
        utl_freebuf((void **)&arg->send_data.data);
        break;
    default:
        break;
    }
}

void btc_hh_call_handler(btc_msg_t *msg)
{
    btc_hidh_args_t *arg = (btc_hidh_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_HH_INIT_EVT:
        btc_hh_init();
        break;
    case BTC_HH_CONNECT_EVT:
        btc_hh_connect(arg);
        break;
    case BTC_HH_DISCONNECT_EVT:
        btc_hh_disconnect(arg);
        break;
    case BTC_HH_UNPLUG_EVT:
        btc_hh_virtual_unplug(arg);
        break;
    case BTC_HH_SET_INFO_EVT:
        btc_hh_set_info(arg);
        break;
    case BTC_HH_GET_PROTO_EVT:
        btc_hh_get_protocol(arg);
        break;
    case BTC_HH_SET_PROTO_EVT:
        btc_hh_set_protocol(arg);
        break;
    case BTC_HH_GET_IDLE_EVT:
        btc_hh_get_idle_time(arg);
        break;
    case BTC_HH_SET_IDLE_EVT:
        btc_hh_set_idle_time(arg);
        break;
    case BTC_HH_GET_REPORT_EVT:
        btc_hh_get_report(arg);
        break;
    case BTC_HH_SET_REPORT_EVT:
        btc_hh_set_report(arg);
        break;
    case BTC_HH_SEND_DATA_EVT:
        btc_hh_send_data(arg);
        break;
    case BTC_HH_DEINIT_EVT:
        btc_hh_deinit();
        break;
    default:
        BTC_TRACE_WARNING("unknown hidh action %d", msg->act);
        break;
    }
    btc_hh_call_arg_deep_free(msg);
}

void btc_hh_cb_arg_deep_free(btc_msg_t *msg)
{
    tBTA_HH *arg = (tBTA_HH *)msg->arg;

    switch (msg->act) {
    case BTA_HH_GET_RPT_EVT:
        utl_freebuf((void **)&arg->hs_data.rsp_data.p_rpt_data);
        break;
    case BTA_HH_DATA_IND_EVT:
        utl_freebuf((void **)&arg->int_data.p_data);
        break;
    default:
        break;
    }
}

bool btc_hh_copy_hid_info(tBTA_HH_DEV_DSCP_INFO *dest, tBTA_HH_DEV_DSCP_INFO *src)
{
    dest->descriptor.dl_len = 0;
    if (src->descriptor.dl_len > 0) {
        dest->descriptor.dsc_list = (uint8_t *)osi_malloc(src->descriptor.dl_len);
    }
    if (dest->descriptor.dsc_list) {
        memcpy(dest->descriptor.dsc_list, src->descriptor.dsc_list, src->descriptor.dl_len);
        dest->descriptor.dl_len = src->descriptor.dl_len;
    }
    dest->vendor_id = src->vendor_id;
    dest->product_id = src->product_id;
    dest->version = src->version;
    dest->ctry_code = src->ctry_code;
    dest->ssr_max_latency = src->ssr_max_latency;
    dest->ssr_min_tout = src->ssr_min_tout;
    return true;
}

bool btc_hh_cb_copy_hid_info(esp_hidh_cb_param_t *param, tBTA_HH_DEV_DSCP_INFO *src)
{
    param->dscp.dl_len = 0;
    if (src->descriptor.dl_len > 0) {
        param->dscp.dsc_list = (uint8_t *)osi_malloc(src->descriptor.dl_len);
    }
    if (param->dscp.dsc_list) {
        memcpy(param->dscp.dsc_list, src->descriptor.dsc_list, src->descriptor.dl_len);
        param->dscp.dl_len = src->descriptor.dl_len;
    }
    param->dscp.vendor_id = src->vendor_id;
    param->dscp.product_id = src->product_id;
    param->dscp.version = src->version;
    param->dscp.ctry_code = src->ctry_code;
    param->dscp.ssr_max_latency = src->ssr_max_latency;
    param->dscp.ssr_min_tout = src->ssr_min_tout;
    return true;
}

void btc_hh_cb_handler(btc_msg_t *msg)
{
    esp_hidh_cb_param_t param = {0};
    tBTA_HH *p_data = (tBTA_HH *)msg->arg;
    btc_hh_device_t *p_dev = NULL;
    int len, i;
    BTC_TRACE_DEBUG("%s: event=%s dereg = %d", __func__, dump_hh_event(msg->act), btc_hh_cb.service_dereg_active);
    switch (msg->act) {
    case BTA_HH_ENABLE_EVT:
        if (p_data->status == BTA_HH_OK) {
            btc_hh_cb.status = BTC_HH_ENABLED;
            BTC_TRACE_DEBUG("Loading added devices");
            /* Add hid descriptors for already bonded hid devices*/
            // btc_storage_load_bonded_hid_info();
        } else {
            btc_hh_cb.status = BTC_HH_DISABLED;
            BTC_TRACE_ERROR("Error, HH enabling failed, status = %d", p_data->status);
        }
        param.init.status = p_data->status;
        btc_hh_cb_to_app(ESP_HIDH_INIT_EVT, &param);
        break;
    case BTA_HH_DISABLE_EVT:
        btc_hh_cb.status = BTC_HH_DISABLED;
        if (btc_hh_cb.service_dereg_active) {
            BTIF_TRACE_DEBUG("BTA_HH_DISABLE_EVT: enabling HID Device service");
            // btif_hd_service_registration();
            btc_hh_cb.service_dereg_active = FALSE;
        }
        if (p_data->status == BTA_HH_OK) {
            // Clear the control block
            for (uint8_t i = 0; i < BTC_HH_MAX_HID; i++) {
                if (btc_hh_cb.devices[i].vup_timer) {
                    osi_alarm_free(btc_hh_cb.devices[i].vup_timer);
                }
            }
            memset(&btc_hh_cb, 0, sizeof(btc_hh_cb));
            for (i = 0; i < BTC_HH_MAX_HID; i++) {
                btc_hh_cb.devices[i].dev_status = ESP_HIDH_CONN_STATE_UNKNOWN;
            }
        } else {
            BTC_TRACE_ERROR("Error, HH disabling failed, status = %d", p_data->status);
        }
        param.deinit.status = p_data->status;
        btc_hh_cb_to_app(ESP_HIDH_DEINIT_EVT, &param);
        break;
    case BTA_HH_OPEN_EVT:
        BTC_TRACE_DEBUG("handle=%d, status =%d", p_data->conn.handle, p_data->conn.status);
        memset(btc_hh_cb.pending_conn_address, 0, BD_ADDR_LEN);
        if (p_data->conn.status == BTA_HH_OK) {
            p_dev = btc_hh_find_connected_dev_by_handle(p_data->conn.handle);
            if (p_dev == NULL) {
                BTC_TRACE_ERROR("Error, cannot find device with handle %d", p_data->conn.handle);
                btc_hh_cb.status = (BTC_HH_STATUS)BTC_HH_DEV_DISCONNECTED;
                // The connect request must come from device side and exceeded the
                // connected HID device number.
                BTA_HhClose(p_data->conn.handle);

                param.open.status = ESP_HIDH_ERR;
                param.open.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
            } else {
                BTC_TRACE_DEBUG("Found device...Getting dscp info for handle "
                                "... %d",
                                p_data->conn.handle);
                memcpy(p_dev->bd_addr, p_data->conn.bda, BD_ADDR_LEN);
                btc_hh_cb.status = (BTC_HH_STATUS)BTC_HH_DEV_CONNECTED;
                // Send set_idle if the peer_device is a keyboard [boblane]
                // if (check_cod(&p_data->conn.bda, COD_HID_KEYBOARD) || check_cod(&p_data->conn.bda, COD_HID_COMBO))
                //     BTA_HhSetIdle(p_data->conn.handle, 0);
                btc_hh_cb.p_curr_dev = btc_hh_find_connected_dev_by_handle(p_data->conn.handle);
                BTA_HhGetDscpInfo(p_data->conn.handle);
                p_dev->dev_status = ESP_HIDH_CONN_STATE_CONNECTED;

                param.open.status = ESP_HIDH_OK;
                param.open.conn_status = ESP_HIDH_CONN_STATE_CONNECTED;
            }
        } else {
            p_dev = btc_hh_find_dev_by_bda(p_data->conn.bda);
            if (p_dev != NULL) {
                btc_hh_stop_vup_timer(p_dev->bd_addr);
                p_dev->dev_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
            }
            btc_hh_cb.status = (BTC_HH_STATUS)BTC_HH_DEV_DISCONNECTED;

            param.open.status = p_data->conn.status;
            param.open.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
        }
        param.open.handle = p_data->conn.handle;
        param.open.is_orig = p_data->conn.is_orig;
        memcpy(param.open.bd_addr, p_data->conn.bda, BD_ADDR_LEN);
        btc_hh_cb_to_app(ESP_HIDH_OPEN_EVT, &param);
        break;
    case BTA_HH_GET_DSCP_EVT:
        len = p_data->dscp_info.descriptor.dl_len;
        BTC_TRACE_DEBUG("len = %d", len);
        do {
            param.dscp.status = ESP_HIDH_OK;
            param.dscp.handle = BTA_HH_INVALID_HANDLE;
            param.dscp.added = false;
            p_dev = btc_hh_cb.p_curr_dev;
            if (p_dev == NULL) {
                BTC_TRACE_ERROR("No HID device is currently connected");
                param.dscp.status = ESP_HIDH_NO_CONNECTION;
                break;
            }

            if (btc_hh_add_added_dev(p_dev->bd_addr, p_dev->attr_mask)) {
                tBTA_HH_DEV_DSCP_INFO dscp_info;
                bt_status_t ret;
                btc_hh_copy_hid_info(&dscp_info, &p_data->dscp_info);
                btc_hh_cb.add_event = BTC_HH_CONNECT_EVT;
                BTA_HhAddDev(p_dev->bd_addr, p_dev->attr_mask, p_dev->sub_class, p_dev->app_id, dscp_info);
                // write hid info to nvs
                ret = btc_storage_add_hid_device_info((bt_bdaddr_t *)p_dev->bd_addr, p_dev->attr_mask, p_dev->sub_class, p_dev->app_id,
                                                      p_data->dscp_info.vendor_id, p_data->dscp_info.product_id,
                                                      p_data->dscp_info.version, p_data->dscp_info.ctry_code,
                                                      p_data->dscp_info.ssr_max_latency, p_data->dscp_info.ssr_min_tout,
                                                      len, p_data->dscp_info.descriptor.dsc_list);

                if (ret != BT_STATUS_SUCCESS) {
                    BTC_TRACE_ERROR("write hid info to nvs failed!");
                }
                // Free buffer created for dscp_info;
                if (dscp_info.descriptor.dl_len > 0 && dscp_info.descriptor.dsc_list != NULL) {
                    utl_freebuf((void **)&dscp_info.descriptor.dsc_list);
                    dscp_info.descriptor.dl_len = 0;
                }
            } else {
                // Device already added.
                BTC_TRACE_WARNING("%s: Device already added ", __func__);
                param.dscp.added = true;
            }
            btc_hh_cb_copy_hid_info(&param, &p_data->dscp_info);
            param.dscp.handle = p_dev->dev_handle;
        } while(0);
        btc_hh_cb_to_app(ESP_HIDH_GET_DSCP_EVT, &param);
        if (param.dscp.dl_len > 0 && param.dscp.dsc_list != NULL) {
            utl_freebuf((void **)&param.dscp.dsc_list);
            param.dscp.dl_len = 0;
        }
        break;
    case BTA_HH_CLOSE_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->dev_status.status,
                         p_data->dev_status.handle);
        p_dev = btc_hh_find_connected_dev_by_handle(p_data->dev_status.handle);
        if (p_dev != NULL) {
            BTC_TRACE_DEBUG("uhid local_vup=%d", p_dev->local_vup);
            btc_hh_stop_vup_timer(p_dev->bd_addr);
            /* If this is a locally initiated VUP, remove the bond as ACL got
             *  disconnected while VUP being processed.
             */
            if (p_dev->local_vup) {
                p_dev->local_vup = false;
                BTA_DmRemoveDevice(p_dev->bd_addr, BT_TRANSPORT_BR_EDR);
            }

            btc_hh_cb.status = (BTC_HH_STATUS)BTC_HH_DEV_DISCONNECTED;
            p_dev->dev_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
            param.close.status = p_data->dev_status.status;
        } else {
            BTC_TRACE_ERROR("Error: cannot find device with handle %d", p_data->dev_status.handle);
            param.close.status = ESP_HIDH_NO_CONNECTION;
        }
        param.close.handle = p_data->dev_status.handle;
        param.close.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
        btc_hh_cb_to_app(ESP_HIDH_CLOSE_EVT, &param);
        break;
    case BTA_HH_VC_UNPLUG_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->dev_status.status,
                        p_data->dev_status.handle);
        p_dev = btc_hh_find_connected_dev_by_handle(p_data->dev_status.handle);
        btc_hh_cb.status = (BTC_HH_STATUS)BTC_HH_DEV_DISCONNECTED;
        if (p_dev != NULL) {
            /* Stop the VUP timer */
            btc_hh_stop_vup_timer(p_dev->bd_addr);
            p_dev->dev_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
            BTC_TRACE_DEBUG("%s---Sending connection state change", __func__);
            param.close.status = ESP_HIDH_OK;
            param.close.handle = p_data->dev_status.handle;
            param.close.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
            btc_hh_cb_to_app(ESP_HIDH_CLOSE_EVT, &param);
            BTC_TRACE_DEBUG("%s---Removing HID bond", __func__);
            /* If it is locally initiated VUP or remote device has its major COD as
            Peripheral removed the bond.*/
            // [boblane]
            if (p_dev->local_vup) {
                p_dev->local_vup = false;
                BTA_DmRemoveDevice(p_dev->bd_addr, BT_TRANSPORT_BR_EDR);
            } else {
                btc_hh_remove_device(p_dev->bd_addr);
            }
            param.unplug.status = p_data->dev_status.status;
        } else {
            BTC_TRACE_ERROR("Error: cannot find device with handle %d", p_data->dev_status.handle);
            param.unplug.status = ESP_HIDH_NO_CONNECTION;
        }
        param.unplug.handle = p_data->dev_status.handle;
        param.unplug.conn_status = ESP_HIDH_CONN_STATE_DISCONNECTED;
        btc_hh_cb_to_app(ESP_HIDH_VC_UNPLUG_EVT, &param);
        break;
    case BTA_HH_DATA_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->send_data.status,
                         p_data->send_data.handle);
        param.send_data.handle = p_data->send_data.handle;
        param.send_data.status = p_data->send_data.status;
        param.send_data.reason = p_data->send_data.reason;
        btc_hh_cb_to_app(ESP_HIDH_DATA_EVT, &param);
        break;
    case BTA_HH_GET_PROTO_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d, proto = [%d], %s", p_data->hs_data.status,
                        p_data->hs_data.handle, p_data->hs_data.rsp_data.proto_mode,
                        (p_data->hs_data.rsp_data.proto_mode == BTA_HH_PROTO_RPT_MODE)
                            ? "Report Mode"
                            : (p_data->hs_data.rsp_data.proto_mode == BTA_HH_PROTO_BOOT_MODE) ? "Boot Mode"
                                                                                              : "Unsupported");
        param.get_proto.proto_mode = proto_mode_change_to_upper_layer(p_data->hs_data.rsp_data.proto_mode);
        param.get_proto.handle = p_data->hs_data.handle;
        param.get_proto.status = p_data->hs_data.status;
        btc_hh_cb_to_app(ESP_HIDH_GET_PROTO_EVT, &param);
        break;
    case BTA_HH_SET_PROTO_EVT:
        BTIF_TRACE_DEBUG("status = %d, handle = %d", p_data->dev_status.status,
                         p_data->dev_status.handle);
        param.set_proto.handle = p_data->dev_status.handle;
        param.set_proto.status = p_data->dev_status.status;
        btc_hh_cb_to_app(ESP_HIDH_SET_PROTO_EVT, &param);
        break;
    case BTA_HH_GET_RPT_EVT: {
        BT_HDR *hdr = p_data->hs_data.rsp_data.p_rpt_data;
        uint8_t *data = NULL;
        uint16_t len = 0;
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->hs_data.status, p_data->hs_data.handle);
        /* p_rpt_data is NULL in HANDSHAKE response case */
        if (hdr) {
            data = (uint8_t *)(hdr + 1) + hdr->offset;
            len = hdr->len;
        }
        param.get_rpt.handle = p_data->hs_data.handle;
        param.get_rpt.status = p_data->hs_data.status;
        param.get_rpt.len = len;
        param.get_rpt.data = data;
        btc_hh_cb_to_app(ESP_HIDH_GET_RPT_EVT, &param);
        break;
    }
    case BTA_HH_SET_RPT_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->dev_status.status,
                         p_data->dev_status.handle);
        param.set_rpt.handle = p_data->dev_status.handle;
        param.set_rpt.status = p_data->dev_status.status;
        btc_hh_cb_to_app(ESP_HIDH_SET_RPT_EVT, &param);
        break;
    case BTA_HH_GET_IDLE_EVT:
        BTC_TRACE_DEBUG("handle = %d, status = %d, rate = %d", p_data->hs_data.handle, p_data->hs_data.status,
                        p_data->hs_data.rsp_data.idle_rate);
        param.get_idle.handle = p_data->hs_data.handle;
        param.get_idle.status = p_data->hs_data.status;
        param.get_idle.idle_rate = p_data->hs_data.rsp_data.idle_rate;
        btc_hh_cb_to_app(ESP_HIDH_GET_IDLE_EVT, &param);
        break;
    case BTA_HH_SET_IDLE_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->dev_status.status, p_data->dev_status.handle);
        param.set_idle.handle = p_data->dev_status.handle;
        param.set_idle.status = p_data->dev_status.status;
        btc_hh_cb_to_app(BTA_HH_SET_IDLE_EVT, &param);
        break;
    case BTA_HH_ADD_DEV_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->dev_info.status, p_data->dev_info.handle);
        for (uint8_t i = 0; i < BTC_HH_MAX_ADDED_DEV; i++) {
            if (memcmp(btc_hh_cb.added_devices[i].bd_addr, p_data->dev_info.bda, BD_ADDR_LEN) == 0) {
                if (p_data->dev_info.status == BTA_HH_OK) {
                    btc_hh_cb.added_devices[i].dev_handle = p_data->dev_info.handle;
                } else {
                    memset(btc_hh_cb.added_devices[i].bd_addr, 0, BD_ADDR_LEN);
                    btc_hh_cb.added_devices[i].dev_handle = BTA_HH_INVALID_HANDLE;
                }
                break;
            }
        }
        if (btc_hh_cb.add_event == BTC_HH_SET_INFO_EVT) {
            param.set_info.handle = p_data->dev_info.handle;
            param.set_info.status = p_data->dev_info.status;
            memcpy(param.set_info.bd_addr, p_data->dev_info.bda, BD_ADDR_LEN);
            btc_hh_cb_to_app(ESP_HIDH_SET_INFO_EVT, &param);
        } else {
            param.add_dev.handle = p_data->dev_info.handle;
            param.add_dev.status = p_data->dev_info.status;
            memcpy(param.add_dev.bd_addr, p_data->dev_info.bda, BD_ADDR_LEN);
            btc_hh_cb_to_app(ESP_HIDH_ADD_DEV_EVT, &param);
        }
        break;
    case BTA_HH_RMV_DEV_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->dev_info.status, p_data->dev_info.handle);
        param.rmv_dev.handle = p_data->dev_info.status;
        param.rmv_dev.status = p_data->dev_info.handle;
        memcpy(param.rmv_dev.bd_addr, p_data->dev_info.bda, BD_ADDR_LEN);
        btc_hh_cb_to_app(ESP_HIDH_RMV_DEV_EVT, &param);
        break;
    case BTA_HH_DATA_IND_EVT:
        BTC_TRACE_DEBUG("status = %d, handle = %d", p_data->int_data.status, p_data->int_data.handle);
        if (p_data->int_data.status == BTA_HH_OK && p_data->int_data.p_data) {
            param.data_ind.len = p_data->int_data.p_data->len;
            param.data_ind.data = p_data->int_data.p_data->data + p_data->int_data.p_data->offset;
        }
        param.data_ind.handle = p_data->int_data.handle;
        param.data_ind.status = p_data->int_data.status;
        param.data_ind.proto_mode = proto_mode_change_to_upper_layer(p_data->int_data.proto_mode);
        btc_hh_cb_to_app(ESP_HIDH_DATA_IND_EVT, &param);
        break;
    case BTA_HH_API_ERR_EVT:
        break;
    default:
        BTC_TRACE_WARNING("%s: Unhandled event: %d", __func__, msg->act);
        break;
    }
    btc_hh_cb_arg_deep_free(msg);
}

void btc_hh_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_hidh_args_t *dst = (btc_hidh_args_t *)p_dest;
    btc_hidh_args_t *src = (btc_hidh_args_t *)p_src;

    switch (msg->act) {
    case BTC_HH_SET_INFO_EVT:
        dst->set_info.hid_info = (esp_hidh_hid_info_t *)osi_malloc(sizeof(esp_hidh_hid_info_t));
        if (dst->set_info.hid_info) {
            memcpy(dst->set_info.hid_info, src->set_info.hid_info, sizeof(esp_hidh_hid_info_t));
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
        }
        break;
    case BTC_HH_SET_REPORT_EVT:
        dst->set_report.report = (uint8_t *)osi_malloc(src->set_report.len);
        if (dst->set_report.report) {
            memcpy(dst->set_report.report, src->set_report.report, src->set_report.len);
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
        }
        break;
    case BTC_HH_SEND_DATA_EVT:
        dst->send_data.data = (uint8_t *)osi_malloc(src->send_data.len);
        if (dst->send_data.data) {
            memcpy(dst->send_data.data, src->send_data.data, src->send_data.len);
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
        }
        break;
    default:
        break;
    }
}

#endif // HID_HOST_INCLUDED == TRUE
