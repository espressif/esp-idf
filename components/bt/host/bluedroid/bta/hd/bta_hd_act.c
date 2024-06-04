/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
 *  Copyright (C) 2005-2012 Broadcom Corporation
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
/******************************************************************************
 *
 *  This file contains the HID device action functions.
 *
 ******************************************************************************/
#include "common/bt_target.h"

#if defined(BTA_HD_INCLUDED) && (BTA_HD_INCLUDED == TRUE)

#include "bta/bta_sys.h"
#include "bta_hd_int.h"
#include "osi/allocator.h"
#include "osi/osi.h"
#include "stack/btm_api.h"
#include <string.h>

static void bta_hd_cback(BD_ADDR bd_addr, uint8_t event, uint32_t data, BT_HDR *pdata);

static bool check_descriptor(uint8_t *data, uint16_t length, bool *has_report_id)
{
    uint8_t *ptr = data;
    *has_report_id = FALSE;
    while (ptr < data + length) {
        uint8_t item = *ptr++;
        switch (item) {
        case 0xfe: // long item indicator
            if (ptr < data + length) {
                ptr += ((*ptr) + 2);
            } else {
                return false;
            }
            break;
        case 0x85: // Report ID
            *has_report_id = TRUE;
        default:
            ptr += (item & 0x03);
            break;
        }
    }
    return (ptr == data + length);
}

/*******************************************************************************
 *
 * Function         bta_hd_api_enable
 *
 * Description      Enables HID device
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_hd_api_enable(tBTA_HD_DATA *p_data)
{
    tBTA_HD_STATUS status = BTA_HD_ERROR;
    tHID_STATUS ret;

    APPL_TRACE_API("%s", __func__);

    HID_DevInit();

    memset(&bta_hd_cb, 0, sizeof(tBTA_HD_CB));

    HID_DevSetSecurityLevel(BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT);
    /* store parameters */
    bta_hd_cb.p_cback = p_data->api_enable.p_cback;

    ret = HID_DevRegister(bta_hd_cback);
    if (ret == HID_SUCCESS) {
        status = BTA_HD_OK;
    } else {
        APPL_TRACE_ERROR("%s: Failed to register HID device (%d)", __func__, ret);
    }

    /* signal BTA call back event */
    (*bta_hd_cb.p_cback)(BTA_HD_ENABLE_EVT, (tBTA_HD *)&status);
}

/*******************************************************************************
 *
 * Function         bta_hd_api_disable
 *
 * Description      Disables HID device
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_hd_api_disable(void)
{
    tBTA_HD_STATUS status = BTA_HD_ERROR;
    tHID_STATUS ret;

    APPL_TRACE_API("%s", __func__);

    /* service is not enabled */
    if (bta_hd_cb.p_cback == NULL)
        return;

    /* Remove service record */
    if (bta_hd_cb.sdp_handle != 0) {
        SDP_DeleteRecord(bta_hd_cb.sdp_handle);
        bta_sys_remove_uuid(UUID_SERVCLASS_HUMAN_INTERFACE);
    }

    /* Deregister with lower layer */
    ret = HID_DevDeregister();
    if (ret == HID_SUCCESS) {
        status = BTA_HD_OK;
    } else {
        APPL_TRACE_ERROR("%s: Failed to deregister HID device (%d)", __func__, ret);
    }

    (*bta_hd_cb.p_cback)(BTA_HD_DISABLE_EVT, (tBTA_HD *)&status);

    memset(&bta_hd_cb, 0, sizeof(tBTA_HD_CB));
}

/*******************************************************************************
 *
 * Function         bta_hd_register_act
 *
 * Description      Registers SDP record
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_hd_register_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD ret;
    tBTA_HD_REGISTER_APP *p_app_data = (tBTA_HD_REGISTER_APP *)p_data;
    bool use_report_id = FALSE;

    APPL_TRACE_API("%s", __func__);

    ret.reg_status.in_use = FALSE;

    /* Check if len doesn't exceed BTA_HD_APP_DESCRIPTOR_LEN and descriptor
     * itself is well-formed. Also check if descriptor has Report Id item so we
     * know if report will have prefix or not. */
    if (p_app_data->d_len > BTA_HD_APP_DESCRIPTOR_LEN ||
        !check_descriptor(p_app_data->d_data, p_app_data->d_len, &use_report_id)) {
        APPL_TRACE_ERROR("%s: Descriptor is too long or malformed", __func__);
        ret.reg_status.status = BTA_HD_ERROR;
        (*bta_hd_cb.p_cback)(BTA_HD_REGISTER_APP_EVT, &ret);
        return;
    }

    ret.reg_status.status = BTA_HD_OK;

    /* Remove old record if for some reason it's already registered */
    if (bta_hd_cb.sdp_handle != 0) {
        SDP_DeleteRecord(bta_hd_cb.sdp_handle);
    }

    bta_hd_cb.use_report_id = use_report_id;
    bta_hd_cb.sdp_handle = SDP_CreateRecord();
    HID_DevAddRecord(bta_hd_cb.sdp_handle, p_app_data->name, p_app_data->description, p_app_data->provider,
                     p_app_data->subclass, p_app_data->d_len, p_app_data->d_data);
    bta_sys_add_uuid(UUID_SERVCLASS_HUMAN_INTERFACE);

    HID_DevSetIncomingQos(p_app_data->in_qos.service_type, p_app_data->in_qos.token_rate,
                          p_app_data->in_qos.token_bucket_size, p_app_data->in_qos.peak_bandwidth,
                          p_app_data->in_qos.access_latency, p_app_data->in_qos.delay_variation);

    HID_DevSetOutgoingQos(p_app_data->out_qos.service_type, p_app_data->out_qos.token_rate,
                          p_app_data->out_qos.token_bucket_size, p_app_data->out_qos.peak_bandwidth,
                          p_app_data->out_qos.access_latency, p_app_data->out_qos.delay_variation);

    // application is registered so we can accept incoming connections
    HID_DevSetIncomingPolicy(TRUE);

    if (HID_DevGetDevice(&ret.reg_status.bda) == HID_SUCCESS) {
        ret.reg_status.in_use = TRUE;
    }

    (*bta_hd_cb.p_cback)(BTA_HD_REGISTER_APP_EVT, &ret);
}

/*******************************************************************************
 *
 * Function         bta_hd_unregister_act
 *
 * Description      Unregisters SDP record
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_hd_unregister_act(UNUSED_ATTR tBTA_HD_DATA *p_data)
{
    tBTA_HD_STATUS status = BTA_HD_OK;

    APPL_TRACE_API("%s", __func__);

    // application is no longer registered so we do not want incoming connections
    HID_DevSetIncomingPolicy(FALSE);

    if (bta_hd_cb.sdp_handle != 0) {
        SDP_DeleteRecord(bta_hd_cb.sdp_handle);
    }

    bta_hd_cb.sdp_handle = 0;
    bta_sys_remove_uuid(UUID_SERVCLASS_HUMAN_INTERFACE);

    (*bta_hd_cb.p_cback)(BTA_HD_UNREGISTER_APP_EVT, (tBTA_HD *)&status);
}

/*******************************************************************************
 *
 * Function         bta_hd_unregister2_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_hd_unregister2_act(tBTA_HD_DATA *p_data)
{
    APPL_TRACE_API("%s", __func__);

    // close first
    bta_hd_close_act(p_data);

    // then unregister
    bta_hd_unregister_act(p_data);

    if (bta_hd_cb.disable_w4_close) {
        bta_hd_api_disable();
    }
}

/*******************************************************************************
 *
 * Function         bta_hd_connect_act
 *
 * Description      Connect to device (must be virtually plugged)
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_connect_act(tBTA_HD_DATA *p_data)
{
    tHID_STATUS ret;
    tBTA_HD_DEVICE_CTRL *p_ctrl = (tBTA_HD_DEVICE_CTRL *)p_data;
    tBTA_HD cback_data;

    APPL_TRACE_API("%s", __func__);
    do {
        ret = HID_DevPlugDevice(p_ctrl->addr);
        if (ret != HID_SUCCESS) {
            APPL_TRACE_WARNING("%s: HID_DevPlugDevice returned %d", __func__, ret);
            return;
        }

        ret = HID_DevConnect();
        if (ret != HID_SUCCESS) {
            APPL_TRACE_WARNING("%s: HID_DevConnect returned %d", __func__, ret);
            return;
        }
    } while (0);

    bdcpy(cback_data.conn.bda, p_ctrl->addr);
    cback_data.conn.status = (ret == HID_SUCCESS ? BTA_HD_OK : BTA_HD_ERROR);
    cback_data.conn.conn_status = BTA_HD_CONN_STATE_CONNECTING;
    bta_hd_cb.p_cback(BTA_HD_OPEN_EVT, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_disconnect_act
 *
 * Description      Disconnect from device
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_disconnect_act(UNUSED_ATTR tBTA_HD_DATA *p_data)
{
    tHID_STATUS ret;
    tBTA_HD cback_data;

    APPL_TRACE_API("%s", __func__);

    ret = HID_DevDisconnect();

    if (ret != HID_SUCCESS) {
        APPL_TRACE_WARNING("%s: HID_DevDisconnect returned %d", __func__, ret);
        return;
    }

    if (HID_DevGetDevice(&cback_data.conn.bda) == HID_SUCCESS) {
        cback_data.conn.status = BTA_HD_OK;
        cback_data.conn.conn_status = BTA_HD_CONN_STATE_DISCONNECTING;
        bta_hd_cb.p_cback(BTA_HD_CLOSE_EVT, &cback_data);
    }
}

/*******************************************************************************
 *
 * Function         bta_hd_add_device_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_add_device_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_DEVICE_CTRL *p_ctrl = (tBTA_HD_DEVICE_CTRL *)p_data;

    APPL_TRACE_API("%s", __func__);

    HID_DevPlugDevice(p_ctrl->addr);
}

/*******************************************************************************
 *
 * Function         bta_hd_remove_device_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_remove_device_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_DEVICE_CTRL *p_ctrl = (tBTA_HD_DEVICE_CTRL *)p_data;

    APPL_TRACE_API("%s", __func__);

    HID_DevUnplugDevice(p_ctrl->addr);
}

/*******************************************************************************
 *
 * Function         bta_hd_send_report_act
 *
 * Description      Sends report
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_send_report_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_SEND_REPORT *p_report = (tBTA_HD_SEND_REPORT *)p_data;
    uint8_t channel;
    uint8_t report_id;
    tBTA_HD cback_data;
    tHID_STATUS ret;

    APPL_TRACE_VERBOSE("%s", __func__);

    channel = p_report->use_intr ? HID_CHANNEL_INTR : HID_CHANNEL_CTRL;
    report_id = (bta_hd_cb.use_report_id || bta_hd_cb.boot_mode) ? p_report->id : 0x00;

    ret = HID_DevSendReport(channel, p_report->type, report_id, p_report->len, p_report->data);

    /* trigger PM */
    bta_sys_busy(BTA_ID_HD, 1, bta_hd_cb.bd_addr);
    bta_sys_idle(BTA_ID_HD, 1, bta_hd_cb.bd_addr);

    cback_data.send_report.status = (ret == HID_SUCCESS ? BTA_HD_OK : BTA_HD_ERROR);
    cback_data.send_report.reason = ret;
    cback_data.send_report.report_id = report_id;
    cback_data.send_report.report_type = p_report->type;
    bta_hd_cb.p_cback(BTA_HD_SEND_REPORT_EVT, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_report_error_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_report_error_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_REPORT_ERR *p_report = (tBTA_HD_REPORT_ERR *)p_data;
    tHID_STATUS ret;
    tBTA_HD cback_data;

    APPL_TRACE_API("%s: error = %d", __func__, p_report->error);

    ret = HID_DevReportError(p_report->error);

    if (ret != HID_SUCCESS) {
        APPL_TRACE_WARNING("%s: HID_DevReportError returned %d", __func__, ret);
    }

    cback_data.report_err.status = (ret == HID_SUCCESS ? BTA_HD_OK : BTA_HD_ERROR);
    cback_data.report_err.reason = ret;
    bta_hd_cb.p_cback(BTA_HD_REPORT_ERR_EVT, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_vc_unplug_act
 *
 * Description      Sends Virtual Cable Unplug
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_vc_unplug_act(UNUSED_ATTR tBTA_HD_DATA *p_data)
{
    tHID_STATUS ret;
    tBTA_HD cback_data = {0};
    BD_ADDR plugged_addr = {0};

    APPL_TRACE_API("%s", __func__);

    bta_hd_cb.vc_unplug = TRUE;
    ret = HID_DevVirtualCableUnplug();

    if (ret == HID_ERR_NO_CONNECTION) {
        /* This is a local VUP without connection, set the vc_unplug to FALSE */
        bta_hd_cb.vc_unplug = FALSE;
        APPL_TRACE_WARNING("%s: HID_DevVirtualCableUnplug returned %d", __func__, ret);
        if (HID_DevGetDevice(&plugged_addr) == HID_SUCCESS) {
            HID_DevUnplugDevice(plugged_addr);
        }
        APPL_TRACE_DEBUG("%s local VUP, remove bda: %02x:%02x:%02x:%02x:%02x:%02x", __func__, plugged_addr[0],
                         plugged_addr[1], plugged_addr[2], plugged_addr[3], plugged_addr[4], plugged_addr[5]);
        cback_data.conn.status = BTA_HD_OK;
        cback_data.conn.conn_status = BTA_HD_CONN_STATE_DISCONNECTED;
        bta_hd_cb.p_cback(BTA_HD_VC_UNPLUG_EVT, &cback_data);
        return;
    } else if (ret != HID_SUCCESS) {
        APPL_TRACE_WARNING("%s: HID_DevVirtualCableUnplug returned %d", __func__, ret);
    }

    /* trigger PM */
    bta_sys_busy(BTA_ID_HD, 1, bta_hd_cb.bd_addr);
    bta_sys_idle(BTA_ID_HD, 1, bta_hd_cb.bd_addr);
}

/*******************************************************************************
 *
 * Function         bta_hd_open_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_open_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    tBTA_HD cback_data;

    APPL_TRACE_API("%s", __func__);

    HID_DevPlugDevice(p_cback->addr);
    bta_sys_conn_open(BTA_ID_HD, 1, p_cback->addr);

    bdcpy(cback_data.conn.bda, p_cback->addr);
    bdcpy(bta_hd_cb.bd_addr, p_cback->addr);
    cback_data.conn.status = BTA_HD_OK;
    cback_data.conn.conn_status = BTA_HD_CONN_STATE_CONNECTED;
    bta_hd_cb.p_cback(BTA_HD_OPEN_EVT, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_close_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_close_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    tBTA_HD cback_data;
    tBTA_HD_EVT cback_event = BTA_HD_CLOSE_EVT;

    APPL_TRACE_API("%s", __func__);

    bta_sys_conn_close(BTA_ID_HD, 1, p_cback->addr);

    if (bta_hd_cb.vc_unplug) {
        bta_hd_cb.vc_unplug = FALSE;
        HID_DevUnplugDevice(p_cback->addr);
        cback_event = BTA_HD_VC_UNPLUG_EVT;
    }

    bdcpy(cback_data.conn.bda, p_cback->addr);
    memset(bta_hd_cb.bd_addr, 0, sizeof(BD_ADDR));
    cback_data.conn.status = BTA_HD_OK;
    cback_data.conn.conn_status = BTA_HD_CONN_STATE_DISCONNECTED;
    bta_hd_cb.p_cback(cback_event, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_intr_data_act
 *
 * Description      Handles incoming DATA request on intr
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_intr_data_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    BT_HDR *p_msg = p_cback->p_data;
    uint16_t len = p_msg->len;
    uint8_t *p_buf = (uint8_t *)(p_msg + 1) + p_msg->offset;
    tBTA_HD_INTR_DATA ret;

    APPL_TRACE_API("%s", __func__);

    if (bta_hd_cb.use_report_id || bta_hd_cb.boot_mode) {
        ret.report_id = *p_buf;
        len--;
        p_buf++;
    } else {
        ret.report_id = 0;
    }

    ret.len = len;
    ret.p_data = p_buf;
    (*bta_hd_cb.p_cback)(BTA_HD_INTR_DATA_EVT, (tBTA_HD *)&ret);
    if (p_msg) {
        osi_free(p_msg);
    }
}

/*******************************************************************************
 *
 * Function         bta_hd_get_report_act
 *
 * Description      Handles incoming GET_REPORT request
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_get_report_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    bool rep_size_follows = p_cback->data;
    BT_HDR *p_msg = p_cback->p_data;
    uint8_t *p_buf = (uint8_t *)(p_msg + 1) + p_msg->offset;
    tBTA_HD_GET_REPORT ret = {0, 0, 0};
    uint16_t remaining_len = p_msg->len;

    APPL_TRACE_API("%s", __func__);
    if (remaining_len < 1) {
        APPL_TRACE_ERROR("%s invalid data, remaining_len:%d", __func__, remaining_len);
        return;
    }

    ret.report_type = *p_buf & HID_PAR_REP_TYPE_MASK;
    p_buf++;
    remaining_len--;

    if (bta_hd_cb.use_report_id) {
        if (remaining_len < 1) {
            APPL_TRACE_ERROR("%s invalid data, remaining_len:%d", __func__, remaining_len);
            return;
        }
        ret.report_id = *p_buf;
        p_buf++;
        remaining_len--;
    }

    if (rep_size_follows) {
        if (remaining_len < 2) {
            APPL_TRACE_ERROR("%s invalid data, remaining_len:%d", __func__, remaining_len);
            return;
        }
        ret.buffer_size = *p_buf | (*(p_buf + 1) << 8);
    }

    (*bta_hd_cb.p_cback)(BTA_HD_GET_REPORT_EVT, (tBTA_HD *)&ret);
    if (p_msg) {
        osi_free(p_msg);
    }
}

/*******************************************************************************
 *
 * Function         bta_hd_set_report_act
 *
 * Description      Handles incoming SET_REPORT request
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_set_report_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    BT_HDR *p_msg = p_cback->p_data;
    uint16_t len = p_msg->len;
    uint8_t *p_buf = (uint8_t *)(p_msg + 1) + p_msg->offset;
    tBTA_HD_SET_REPORT ret = {0, 0, 0, NULL};

    APPL_TRACE_API("%s", __func__);

    ret.report_type = *p_buf & HID_PAR_REP_TYPE_MASK;
    p_buf++;
    len--;

    if (bta_hd_cb.use_report_id || bta_hd_cb.boot_mode) {
        ret.report_id = *p_buf;
        len--;
        p_buf++;
    } else {
        ret.report_id = 0;
    }

    ret.len = len;
    ret.p_data = p_buf;
    (*bta_hd_cb.p_cback)(BTA_HD_SET_REPORT_EVT, (tBTA_HD *)&ret);
    if (p_msg) {
        osi_free(p_msg);
    }
}

/*******************************************************************************
 *
 * Function         bta_hd_set_protocol_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_set_protocol_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    tBTA_HD cback_data;

    APPL_TRACE_API("%s", __func__);

    bta_hd_cb.boot_mode = (p_cback->data == HID_PAR_PROTOCOL_BOOT_MODE);
    cback_data.set_protocol = p_cback->data;

    (*bta_hd_cb.p_cback)(BTA_HD_SET_PROTOCOL_EVT, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_vc_unplug_done_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_vc_unplug_done_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    tBTA_HD cback_data;

    APPL_TRACE_API("%s", __func__);

    bta_sys_conn_close(BTA_ID_HD, 1, p_cback->addr);

    HID_DevUnplugDevice(p_cback->addr);

    bdcpy(cback_data.conn.bda, p_cback->addr);
    bdcpy(bta_hd_cb.bd_addr, p_cback->addr);
    cback_data.conn.status = BTA_HD_OK;
    cback_data.conn.conn_status = BTA_HD_CONN_STATE_DISCONNECTED;
    (*bta_hd_cb.p_cback)(BTA_HD_VC_UNPLUG_EVT, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_suspend_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_suspend_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;

    APPL_TRACE_API("%s", __func__);

    bta_sys_idle(BTA_ID_HD, 1, p_cback->addr);
}

/*******************************************************************************
 *
 * Function         bta_hd_exit_suspend_act
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_exit_suspend_act(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;

    APPL_TRACE_API("%s", __func__);

    bta_sys_busy(BTA_ID_HD, 1, p_cback->addr);
    bta_sys_idle(BTA_ID_HD, 1, p_cback->addr);
}

/*******************************************************************************
 *
 * Function         bta_hd_open_failure
 *
 * Description
 *
 * Returns          void
 *
 ******************************************************************************/
extern void bta_hd_open_failure(tBTA_HD_DATA *p_data)
{
    tBTA_HD_CBACK_DATA *p_cback = (tBTA_HD_CBACK_DATA *)p_data;
    tBTA_HD cback_data = {0};

    bdcpy(cback_data.conn.bda, p_cback->addr);
    cback_data.conn.status = BTA_HD_ERROR;
    cback_data.conn.conn_status = BTA_HD_CONN_STATE_DISCONNECTED;
    bta_hd_cb.p_cback(BTA_HD_OPEN_EVT, &cback_data);
}

/*******************************************************************************
 *
 * Function         bta_hd_cback
 *
 * Description      BTA HD callback function
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_hd_cback(BD_ADDR bd_addr, uint8_t event, uint32_t data, BT_HDR *pdata)
{
    tBTA_HD_CBACK_DATA *p_buf = NULL;
    uint16_t sm_event = BTA_HD_INVALID_EVT;

    APPL_TRACE_API("%s: event=%d", __func__, event);

    switch (event) {
    case HID_DHOST_EVT_OPEN:
        sm_event = BTA_HD_INT_OPEN_EVT;
        break;

    case HID_DHOST_EVT_CLOSE:
        sm_event = BTA_HD_INT_CLOSE_EVT;
        break;

    case HID_DHOST_EVT_GET_REPORT:
        sm_event = BTA_HD_INT_GET_REPORT_EVT;
        break;

    case HID_DHOST_EVT_SET_REPORT:
        sm_event = BTA_HD_INT_SET_REPORT_EVT;
        break;

    case HID_DHOST_EVT_SET_PROTOCOL:
        sm_event = BTA_HD_INT_SET_PROTOCOL_EVT;
        break;

    case HID_DHOST_EVT_INTR_DATA:
        sm_event = BTA_HD_INT_INTR_DATA_EVT;
        break;

    case HID_DHOST_EVT_VC_UNPLUG:
        sm_event = BTA_HD_INT_VC_UNPLUG_EVT;
        break;

    case HID_DHOST_EVT_SUSPEND:
        sm_event = BTA_HD_INT_SUSPEND_EVT;
        break;

    case HID_DHOST_EVT_EXIT_SUSPEND:
        sm_event = BTA_HD_INT_EXIT_SUSPEND_EVT;
        break;
    }

    if (sm_event != BTA_HD_INVALID_EVT &&
        (p_buf = (tBTA_HD_CBACK_DATA *)osi_malloc(sizeof(tBTA_HD_CBACK_DATA) + sizeof(BT_HDR))) != NULL) {
        p_buf->hdr.event = sm_event;
        bdcpy(p_buf->addr, bd_addr);
        p_buf->data = data;
        p_buf->p_data = pdata;

        bta_sys_sendmsg(p_buf);
    }
}
#endif /* BTA_HD_INCLUDED */
