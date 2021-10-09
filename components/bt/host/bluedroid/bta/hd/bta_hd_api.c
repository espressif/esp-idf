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
 *  This file contains the HID DEVICE API in the subsystem of BTA.
 *
 ******************************************************************************/
#include "common/bt_target.h"

#if defined(BTA_HD_INCLUDED) && (BTA_HD_INCLUDED == TRUE)

#include "bta/bta_hd_api.h"
#include "bta_hd_int.h"
#include "osi/allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************
 *  Constants
 ****************************************************************************/
static const tBTA_SYS_REG bta_hd_reg = {bta_hd_hdl_event, BTA_HdDisable};
/*******************************************************************************
 *
 * Function         BTA_HdEnable
 *
 * Description      Enables HID device
 *
 * Returns          void
 *
 ******************************************************************************/
void BTA_HdEnable(tBTA_HD_CBACK *p_cback)
{
    tBTA_HD_API_ENABLE *p_buf;
    APPL_TRACE_API("%s", __func__);
    bta_sys_register(BTA_ID_HD, &bta_hd_reg);
    p_buf = (tBTA_HD_API_ENABLE *)osi_malloc((uint16_t)sizeof(tBTA_HD_API_ENABLE));
    if (p_buf != NULL) {
        memset(p_buf, 0, sizeof(tBTA_HD_API_ENABLE));
        p_buf->hdr.event = BTA_HD_API_ENABLE_EVT;
        p_buf->p_cback = p_cback;
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdDisable
 *
 * Description      Disables HID device.
 *
 * Returns          void
 *
 ******************************************************************************/
void BTA_HdDisable(void)
{
    BT_HDR *p_buf;
    APPL_TRACE_API("%s", __func__);
    bta_sys_deregister(BTA_ID_HD);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HD_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdRegisterApp
 *
 * Description      This function is called when application should be
 *registered
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdRegisterApp(tBTA_HD_APP_INFO *p_app_info, tBTA_HD_QOS_INFO *p_in_qos, tBTA_HD_QOS_INFO *p_out_qos)
{
    tBTA_HD_REGISTER_APP *p_buf;
    APPL_TRACE_API("%s", __func__);
    if ((p_buf = (tBTA_HD_REGISTER_APP *)osi_malloc(sizeof(tBTA_HD_REGISTER_APP))) != NULL) {
        p_buf->hdr.event = BTA_HD_API_REGISTER_APP_EVT;
        if (p_app_info->p_name) {
            strncpy(p_buf->name, p_app_info->p_name, BTA_HD_APP_NAME_LEN);
            p_buf->name[BTA_HD_APP_NAME_LEN] = '\0';
        } else {
            p_buf->name[0] = '\0';
        }
        if (p_app_info->p_description) {
            strncpy(p_buf->description, p_app_info->p_description, BTA_HD_APP_DESCRIPTION_LEN);
            p_buf->description[BTA_HD_APP_DESCRIPTION_LEN] = '\0';
        } else {
            p_buf->description[0] = '\0';
        }
        if (p_app_info->p_provider) {
            strncpy(p_buf->provider, p_app_info->p_provider, BTA_HD_APP_PROVIDER_LEN);
            p_buf->provider[BTA_HD_APP_PROVIDER_LEN] = '\0';
        } else {
            p_buf->provider[0] = '\0';
        }
        p_buf->subclass = p_app_info->subclass;
        p_buf->d_len = p_app_info->descriptor.dl_len;
        memcpy(p_buf->d_data, p_app_info->descriptor.dsc_list, p_app_info->descriptor.dl_len);
        // copy qos data as-is
        memcpy(&p_buf->in_qos, p_in_qos, sizeof(tBTA_HD_QOS_INFO));
        memcpy(&p_buf->out_qos, p_out_qos, sizeof(tBTA_HD_QOS_INFO));
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdUnregisterApp
 *
 * Description      This function is called when application should be
 *unregistered
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdUnregisterApp(void)
{
    BT_HDR *p_buf;
    APPL_TRACE_API("%s", __func__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HD_API_UNREGISTER_APP_EVT;
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdSendReport
 *
 * Description      This function is called when report is to be sent
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdSendReport(tBTA_HD_REPORT *p_report)
{
    tBTA_HD_SEND_REPORT *p_buf;
    APPL_TRACE_VERBOSE("%s", __func__);
    if (p_report->len > BTA_HD_REPORT_LEN) {
        APPL_TRACE_WARNING("%s, report len (%d) > MTU len (%d), can't send report."
                           " Increase value of HID_DEV_MTU_SIZE to send larger reports",
                           __func__, p_report->len, BTA_HD_REPORT_LEN);
        return;
    }
    if ((p_buf = (tBTA_HD_SEND_REPORT *)osi_malloc(sizeof(tBTA_HD_SEND_REPORT))) != NULL) {
        p_buf->hdr.event = BTA_HD_API_SEND_REPORT_EVT;
        p_buf->use_intr = p_report->use_intr;
        p_buf->type = p_report->type;
        p_buf->id = p_report->id;
        p_buf->len = p_report->len;
        memcpy(p_buf->data, p_report->p_data, p_report->len);
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdVirtualCableUnplug
 *
 * Description      This function is called when VCU shall be sent
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdVirtualCableUnplug(void)
{
    BT_HDR *p_buf;
    APPL_TRACE_API("%s", __func__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HD_API_VC_UNPLUG_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
 *
 * Function         BTA_HdConnect
 *
 * Description      This function is called when connection to host shall be
 *                  made
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdConnect(BD_ADDR addr)
{
    tBTA_HD_DEVICE_CTRL *p_buf;
    APPL_TRACE_API("%s", __func__);

    if ((p_buf = (tBTA_HD_DEVICE_CTRL *)osi_malloc(sizeof(tBTA_HD_DEVICE_CTRL))) != NULL) {
        p_buf->hdr.event = BTA_HD_API_CONNECT_EVT;
        memcpy(p_buf->addr, addr, sizeof(BD_ADDR));
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
 *
 * Function         BTA_HdDisconnect
 *
 * Description      This function is called when host shall be disconnected
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdDisconnect(void)
{
    BT_HDR *p_buf;
    APPL_TRACE_API("%s", __func__);
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HD_API_DISCONNECT_EVT;
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdAddDevice
 *
 * Description      This function is called when a device is virtually cabled
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdAddDevice(BD_ADDR addr)
{
    tBTA_HD_DEVICE_CTRL *p_buf;
    APPL_TRACE_API("%s", __func__);
    if ((p_buf = (tBTA_HD_DEVICE_CTRL *)osi_malloc(sizeof(tBTA_HD_DEVICE_CTRL))) != NULL) {
        p_buf->hdr.event = BTA_HD_API_ADD_DEVICE_EVT;
        memcpy(p_buf->addr, addr, sizeof(BD_ADDR));
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdRemoveDevice
 *
 * Description      This function is called when a device is virtually uncabled
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdRemoveDevice(BD_ADDR addr)
{
    tBTA_HD_DEVICE_CTRL *p_buf;
    APPL_TRACE_API("%s", __func__);
    if ((p_buf = (tBTA_HD_DEVICE_CTRL *)osi_malloc(sizeof(tBTA_HD_DEVICE_CTRL))) != NULL) {
        p_buf->hdr.event = BTA_HD_API_REMOVE_DEVICE_EVT;
        memcpy(p_buf->addr, addr, sizeof(BD_ADDR));
        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
 *
 * Function         BTA_HdReportError
 *
 * Description      This function is called when reporting error for set report
 *
 * Returns          void
 *
 ******************************************************************************/
extern void BTA_HdReportError(uint8_t error)
{
    tBTA_HD_REPORT_ERR *p_buf;
    APPL_TRACE_API("%s", __func__);
    if ((p_buf = (tBTA_HD_REPORT_ERR *)osi_malloc(sizeof(tBTA_HD_REPORT_ERR))) != NULL) {
        p_buf->hdr.event = BTA_HD_API_REPORT_ERROR_EVT;
        p_buf->error = error;
        bta_sys_sendmsg(p_buf);
    }
}
#endif /* BTA_HD_INCLUDED */
