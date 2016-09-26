/******************************************************************************
 *
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
 *  This file contains the HID HOST API in the subsystem of BTA.
 *
 ******************************************************************************/

#include "bt_target.h"

#if defined(BTA_HH_INCLUDED) && (BTA_HH_INCLUDED == TRUE)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bta_hh_api.h"
#include "bta_hh_int.h"
#include "l2c_api.h"
#include "utl.h"

#define LOG_TAG "bt_bta_hh"
#include "osi/include/log.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_hh_reg =
{
    bta_hh_hdl_event,
    BTA_HhDisable
};

/*******************************************************************************
**
** Function         BTA_HhEnable
**
** Description      Enable the HID host.  This function must be called before
**                  any other functions in the HID host API are called. When the
**                  enable operation is complete the callback function will be
**                  called with BTA_HH_ENABLE_EVT.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_HhEnable(tBTA_SEC sec_mask, tBTA_HH_CBACK *p_cback)
{
    tBTA_HH_API_ENABLE *p_buf;

    /* register with BTA system manager */
    bta_sys_register(BTA_ID_HH, &bta_hh_reg);

    LOG_INFO("%s sec_mask:0x%x p_cback:%p", __func__, sec_mask, p_cback);
    p_buf = (tBTA_HH_API_ENABLE *)GKI_getbuf((UINT16)sizeof(tBTA_HH_API_ENABLE));

    if (p_buf != NULL)
    {
        memset(p_buf, 0, sizeof(tBTA_HH_API_ENABLE));

        p_buf->hdr.event = BTA_HH_API_ENABLE_EVT;
        p_buf->p_cback = p_cback;
        p_buf->sec_mask = sec_mask;

        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_HhDisable
**
** Description      Disable the HID host. If the server is currently
**                  connected, the connection will be closed.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhDisable(void)
{
    BT_HDR  *p_buf;

    bta_sys_deregister(BTA_ID_HH);
    if ((p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_HH_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_HhClose
**
** Description      Disconnect a connection.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhClose(UINT8 dev_handle)
{
    BT_HDR    *p_buf;

    if ((p_buf = (BT_HDR *)GKI_getbuf((UINT16)sizeof(BT_HDR))) != NULL)
    {
        memset(p_buf, 0, sizeof(BT_HDR));
        p_buf->event            = BTA_HH_API_CLOSE_EVT;
        p_buf->layer_specific   = (UINT16) dev_handle;

        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_HhOpen
**
** Description      Connect to a device of specified BD address in specified
**                  protocol mode and security level.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhOpen(BD_ADDR dev_bda, tBTA_HH_PROTO_MODE mode, tBTA_SEC sec_mask)
{
    tBTA_HH_API_CONN *p_buf;

    p_buf = (tBTA_HH_API_CONN *)GKI_getbuf((UINT16)sizeof(tBTA_HH_API_CONN));

    if (p_buf!= NULL)
    {
        memset((void *)p_buf, 0, sizeof(tBTA_HH_API_CONN));

        p_buf->hdr.event            = BTA_HH_API_OPEN_EVT;
        p_buf->hdr.layer_specific   = BTA_HH_INVALID_HANDLE;
        p_buf->sec_mask             = sec_mask;
        p_buf->mode                 = mode;
        bdcpy(p_buf->bd_addr, dev_bda);

        bta_sys_sendmsg((void *)p_buf);
    }
    else
    {
        APPL_TRACE_ERROR("No resource to send HID host Connect request.");
    }
}

/*******************************************************************************
**
** Function  bta_hh_snd_write_dev
**
*******************************************************************************/
static void bta_hh_snd_write_dev(UINT8 dev_handle, UINT8 t_type, UINT8 param,
                                 UINT16 data, UINT8 rpt_id, BT_HDR  *p_data)
{
    tBTA_HH_CMD_DATA *p_buf;
    UINT16          len = (UINT16) (sizeof(tBTA_HH_CMD_DATA) );

    if ((p_buf = (tBTA_HH_CMD_DATA *)GKI_getbuf(len))!= NULL)
    {
        memset(p_buf, 0, sizeof(tBTA_HH_CMD_DATA));

        p_buf->hdr.event = BTA_HH_API_WRITE_DEV_EVT;
        p_buf->hdr.layer_specific   = (UINT16) dev_handle;
        p_buf->t_type   = t_type;
        p_buf->data     = data;
        p_buf->param    = param;
        p_buf->p_data   = p_data;
        p_buf->rpt_id   = rpt_id;

        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
**
** Function         BTA_HhSetReport
**
** Description      send SET_REPORT to device.
**
** Parameter        dev_handle: device handle
**                  r_type:     report type, could be BTA_HH_RPTT_OUTPUT or
**                              BTA_HH_RPTT_FEATURE.
** Returns          void
**
*******************************************************************************/
void BTA_HhSetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, BT_HDR *p_data)
{
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_SET_REPORT, r_type, 0, 0, p_data);
}
/*******************************************************************************
**
** Function         BTA_HhGetReport
**
** Description      Send a GET_REPORT to HID device.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhGetReport(UINT8 dev_handle, tBTA_HH_RPT_TYPE r_type, UINT8 rpt_id, UINT16 buf_size)
{
    UINT8 param = (buf_size) ? (r_type | 0x08) : r_type;

    bta_hh_snd_write_dev(dev_handle, HID_TRANS_GET_REPORT, param,
                        buf_size, rpt_id, NULL);
}
/*******************************************************************************
**
** Function         BTA_HhSetProtoMode
**
** Description      This function set the protocol mode at specified HID handle
**
** Returns          void
**
*******************************************************************************/
void BTA_HhSetProtoMode(UINT8 dev_handle, tBTA_HH_PROTO_MODE p_type)
{
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_SET_PROTOCOL, (UINT8)p_type,
                        0, 0, NULL);
}
/*******************************************************************************
**
** Function         BTA_HhGetProtoMode
**
** Description      This function get protocol mode information.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhGetProtoMode(UINT8 dev_handle)
{
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_GET_PROTOCOL, 0, 0, 0, NULL);
}
/*******************************************************************************
**
** Function         BTA_HhSetIdle
**
** Description      send SET_IDLE to device.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhSetIdle(UINT8 dev_handle, UINT16 idle_rate)
{
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_SET_IDLE, 0, idle_rate, 0, NULL);
}

/*******************************************************************************
**
** Function         BTA_HhGetIdle
**
** Description      Send a GET_IDLE from HID device.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhGetIdle(UINT8 dev_handle)
{
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_GET_IDLE, 0, 0, 0, NULL);
}
/*******************************************************************************
**
** Function         BTA_HhSendCtrl
**
** Description      Send a control command to HID device.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhSendCtrl(UINT8 dev_handle, tBTA_HH_TRANS_CTRL_TYPE c_type)
{
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_CONTROL, (UINT8)c_type, 0, 0, NULL);
}
/*******************************************************************************
**
** Function         BTA_HhSendData
**
** Description      This function send DATA transaction to HID device.
**
** Parameter        dev_handle: device handle
**                  dev_bda: remote device address
**                  p_data: data to be sent in the DATA transaction; or
**                          the data to be write into the Output Report of a LE HID
**                          device. The report is identified the report ID which is
**                          the value of the byte (UINT8 *)(p_buf + 1) + p_buf->offset.
**                          p_data->layer_specific needs to be set to the report type,
**                          it can be OUTPUT report, or FEATURE report.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhSendData(UINT8 dev_handle, BD_ADDR dev_bda, BT_HDR  *p_data)
{
    UNUSED(dev_bda);
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    if (p_data->layer_specific != BTA_HH_RPTT_OUTPUT)
    {
        APPL_TRACE_ERROR("ERROR! Wrong report type! Write Command only valid for output report!");
        return;
    }
#endif
    bta_hh_snd_write_dev(dev_handle, HID_TRANS_DATA, (UINT8)p_data->layer_specific, 0, 0, p_data);
}

/*******************************************************************************
**
** Function         BTA_HhGetDscpInfo
**
** Description      Get HID device report descriptor
**
** Returns          void
**
*******************************************************************************/
void BTA_HhGetDscpInfo(UINT8 dev_handle)
{
    BT_HDR    *p_buf;

    if ((p_buf = (BT_HDR *)GKI_getbuf((UINT16)sizeof(BT_HDR))) != NULL)
    {
        memset(p_buf, 0, sizeof(BT_HDR));
        p_buf->event            = BTA_HH_API_GET_DSCP_EVT;
        p_buf->layer_specific   = (UINT16) dev_handle;

        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_HhAddDev
**
** Description      Add a virtually cabled device into HID-Host device list
**                  to manage and assign a device handle for future API call,
**                  host applciation call this API at start-up to initialize its
**                  virtually cabled devices.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhAddDev(BD_ADDR bda, tBTA_HH_ATTR_MASK attr_mask, UINT8 sub_class,
                  UINT8 app_id, tBTA_HH_DEV_DSCP_INFO dscp_info)
{
    tBTA_HH_MAINT_DEV    *p_buf;
    UINT16  len = sizeof(tBTA_HH_MAINT_DEV) + dscp_info.descriptor.dl_len;

    p_buf = (tBTA_HH_MAINT_DEV *)GKI_getbuf(len);

    if (p_buf != NULL)
    {
        memset(p_buf, 0, sizeof(tBTA_HH_MAINT_DEV));

        p_buf->hdr.event            = BTA_HH_API_MAINT_DEV_EVT;
        p_buf->sub_event            = BTA_HH_ADD_DEV_EVT;
        p_buf->hdr.layer_specific   = BTA_HH_INVALID_HANDLE;

        p_buf->attr_mask            = (UINT16) attr_mask;
        p_buf->sub_class            = sub_class;
        p_buf->app_id               = app_id;
        bdcpy(p_buf->bda, bda);

        memcpy(&p_buf->dscp_info, &dscp_info, sizeof(tBTA_HH_DEV_DSCP_INFO));
        if ( dscp_info.descriptor.dl_len != 0 && dscp_info.descriptor.dsc_list)
        {
            p_buf->dscp_info.descriptor.dl_len =  dscp_info.descriptor.dl_len;
            p_buf->dscp_info.descriptor.dsc_list = (UINT8 *)(p_buf + 1);
            memcpy(p_buf->dscp_info.descriptor.dsc_list, dscp_info.descriptor.dsc_list, dscp_info.descriptor.dl_len);
        }
        else
        {
            p_buf->dscp_info.descriptor.dsc_list = NULL;
            p_buf->dscp_info.descriptor.dl_len = 0;
        }

        bta_sys_sendmsg(p_buf);
    }
}
/*******************************************************************************
**
** Function         BTA_HhRemoveDev
**
** Description      Remove a device from the HID host devices list.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhRemoveDev(UINT8 dev_handle )
{
    tBTA_HH_MAINT_DEV    *p_buf;

    p_buf = (tBTA_HH_MAINT_DEV *)GKI_getbuf((UINT16)sizeof(tBTA_HH_MAINT_DEV));

    if (p_buf != NULL)
    {
        memset(p_buf, 0, sizeof(tBTA_HH_MAINT_DEV));

        p_buf->hdr.event            = BTA_HH_API_MAINT_DEV_EVT;
        p_buf->sub_event            = BTA_HH_RMV_DEV_EVT;
        p_buf->hdr.layer_specific   = (UINT16) dev_handle;

        bta_sys_sendmsg(p_buf);
    }
}
#if BTA_HH_LE_INCLUDED == TRUE

/*******************************************************************************
**
** Function         BTA_HhUpdateLeScanParam
**
** Description      Update the scan paramteters if connected to a LE hid device as
**                  report host.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhUpdateLeScanParam(UINT8 dev_handle, UINT16 scan_int, UINT16 scan_win)
{
    tBTA_HH_SCPP_UPDATE    *p_buf;

    p_buf = (tBTA_HH_SCPP_UPDATE *)GKI_getbuf((UINT16)sizeof(tBTA_HH_SCPP_UPDATE));

    if (p_buf != NULL)
    {
        memset(p_buf, 0, sizeof(tBTA_HH_SCPP_UPDATE));

        p_buf->hdr.event            = BTA_HH_API_SCPP_UPDATE_EVT;
        p_buf->hdr.layer_specific   = (UINT16) dev_handle;
        p_buf->scan_int             =  scan_int;
        p_buf->scan_win             =  scan_win;

        bta_sys_sendmsg(p_buf);
    }
}
#endif
/*******************************************************************************/
/*                          Utility Function                                   */
/*******************************************************************************/

/*******************************************************************************
**
** Function         BTA_HhParseBootRpt
**
** Description      This utility function parse a boot mode report.
**                  For keyboard report, report data will carry the keycode max
**                  up to 6 key press in one report. Application need to convert
**                  the keycode into keypress character according to keyboard
**                  language.
**
** Returns          void
**
*******************************************************************************/
void BTA_HhParseBootRpt(tBTA_HH_BOOT_RPT *p_data, UINT8 *p_report,
                        UINT16 report_len)
{
    p_data->dev_type = BTA_HH_DEVT_UNKNOWN;

    if (p_report)
    {
        /* first byte is report ID */
        switch (p_report[0])
        {
        case BTA_HH_KEYBD_RPT_ID: /* key board report ID */
            p_data->dev_type = p_report[0];
            bta_hh_parse_keybd_rpt(p_data, p_report + 1, (UINT16)(report_len -1));
            break;

        case BTA_HH_MOUSE_RPT_ID: /* mouse report ID */
            p_data->dev_type = p_report[0];
            bta_hh_parse_mice_rpt(p_data, p_report + 1, (UINT16)(report_len - 1));
            break;

        default:
            APPL_TRACE_DEBUG("Unknown boot report: %d", p_report[0]);;
            break;
        }
    }

    return;
}

#endif /* BTA_HH_INCLUDED */
