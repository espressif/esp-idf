/******************************************************************************
 *
 *  Copyright (C) 2011-2012 Broadcom Corporation
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
 *  This is the implementation of the API for the advanced audio/video (AV)
 *  subsystem of BTA, Broadcom's Bluetooth application layer for mobile
 *  phones.
 *
 ******************************************************************************/

#include "bt_target.h"
#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

#include "bta_api.h"
#include "bta_sys.h"
#include "bta_av_api.h"
#include "bta_av_int.h"
#include "gki.h"
#include <string.h>

/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_av_reg =
{
    bta_av_hdl_event,
    BTA_AvDisable
};

/*******************************************************************************
**
** Function         BTA_AvEnable
**
** Description      Enable the advanced audio/video service. When the enable
**                  operation is complete the callback function will be
**                  called with a BTA_AV_ENABLE_EVT. This function must
**                  be called before other function in the AV API are
**                  called.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvEnable(tBTA_SEC sec_mask, tBTA_AV_FEAT features, tBTA_AV_CBACK *p_cback)
{
    tBTA_AV_API_ENABLE  *p_buf;

    /* register with BTA system manager */
    bta_sys_register(BTA_ID_AV, &bta_av_reg);

    if ((p_buf = (tBTA_AV_API_ENABLE *) GKI_getbuf(sizeof(tBTA_AV_API_ENABLE))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_ENABLE_EVT;
        p_buf->p_cback  = p_cback;
        p_buf->features = features;
        p_buf->sec_mask = sec_mask;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvDisable
**
** Description      Disable the advanced audio/video service.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvDisable(void)
{
    BT_HDR  *p_buf;

    bta_sys_deregister(BTA_ID_AV);
    if ((p_buf = (BT_HDR *) GKI_getbuf(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_AV_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvRegister
**
** Description      Register the audio or video service to stack. When the
**                  operation is complete the callback function will be
**                  called with a BTA_AV_REGISTER_EVT. This function must
**                  be called before AVDT stream is open.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AvRegister(tBTA_AV_CHNL chnl, const char *p_service_name, UINT8 app_id, tBTA_AV_DATA_CBACK  *p_data_cback)
{
    tBTA_AV_API_REG  *p_buf;


    if ((p_buf = (tBTA_AV_API_REG *) GKI_getbuf(sizeof(tBTA_AV_API_REG))) != NULL)
    {
        p_buf->hdr.layer_specific   = chnl;
        p_buf->hdr.event = BTA_AV_API_REGISTER_EVT;
        if(p_service_name)
        {
            BCM_STRNCPY_S(p_buf->p_service_name, sizeof(p_buf->p_service_name), p_service_name, BTA_SERVICE_NAME_LEN);
            p_buf->p_service_name[BTA_SERVICE_NAME_LEN-1] = 0;
        }
        else
        {
            p_buf->p_service_name[0] = 0;
        }
        p_buf->app_id = app_id;
        p_buf->p_app_data_cback = p_data_cback;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvDeregister
**
** Description      Deregister the audio or video service
**
** Returns          void
**
*******************************************************************************/
void BTA_AvDeregister(tBTA_AV_HNDL hndl)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) GKI_getbuf(sizeof(BT_HDR))) != NULL)
    {
        p_buf->layer_specific   = hndl;
        p_buf->event = BTA_AV_API_DEREGISTER_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvOpen
**
** Description      Opens an advanced audio/video connection to a peer device.
**                  When connection is open callback function is called
**                  with a BTA_AV_OPEN_EVT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvOpen(BD_ADDR bd_addr, tBTA_AV_HNDL handle, BOOLEAN use_rc, tBTA_SEC sec_mask,
                                                                             UINT16 uuid)
{
    tBTA_AV_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_AV_API_OPEN *) GKI_getbuf(sizeof(tBTA_AV_API_OPEN))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_OPEN_EVT;
        p_buf->hdr.layer_specific   = handle;
        bdcpy(p_buf->bd_addr, bd_addr);
        p_buf->use_rc = use_rc;
        p_buf->sec_mask = sec_mask;
        p_buf->switch_res = BTA_AV_RS_NONE;
        p_buf->uuid = uuid;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvClose
**
** Description      Close the current streams.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvClose(tBTA_AV_HNDL handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) GKI_getbuf(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_AV_API_CLOSE_EVT;
        p_buf->layer_specific   = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvDisconnect
**
** Description      Close the connection to the address.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvDisconnect(BD_ADDR bd_addr)
{
    tBTA_AV_API_DISCNT  *p_buf;

    if ((p_buf = (tBTA_AV_API_DISCNT *) GKI_getbuf(sizeof(tBTA_AV_API_DISCNT))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_DISCONNECT_EVT;
        bdcpy(p_buf->bd_addr, bd_addr);
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvStart
**
** Description      Start audio/video stream data transfer.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvStart(void)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) GKI_getbuf(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_AV_API_START_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvEnable_Sink
**
** Description      Enable/Disable A2DP Sink..
**
** Returns          void
**
*******************************************************************************/
void BTA_AvEnable_Sink(int enable)
{
#if (BTA_AV_SINK_INCLUDED == TRUE)
    BT_HDR  *p_buf;
    if ((p_buf = (BT_HDR *) GKI_getbuf(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_AV_API_SINK_ENABLE_EVT;
        p_buf->layer_specific = enable;
        bta_sys_sendmsg(p_buf);
    }
#else
    return;
#endif
}

/*******************************************************************************
**
** Function         BTA_AvStop
**
** Description      Stop audio/video stream data transfer.
**                  If suspend is TRUE, this function sends AVDT suspend signal
**                  to the connected peer(s).
**
** Returns          void
**
*******************************************************************************/
void BTA_AvStop(BOOLEAN suspend)
{
    tBTA_AV_API_STOP  *p_buf;

    if ((p_buf = (tBTA_AV_API_STOP *) GKI_getbuf(sizeof(tBTA_AV_API_STOP))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_STOP_EVT;
        p_buf->flush   = TRUE;
        p_buf->suspend = suspend;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvReconfig
**
** Description      Reconfigure the audio/video stream.
**                  If suspend is TRUE, this function tries the suspend/reconfigure
**                  procedure first.
**                  If suspend is FALSE or when suspend/reconfigure fails,
**                  this function closes and re-opens the AVDT connection.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvReconfig(tBTA_AV_HNDL hndl, BOOLEAN suspend, UINT8 sep_info_idx,
                    UINT8 *p_codec_info, UINT8 num_protect, UINT8 *p_protect_info)
{
    tBTA_AV_API_RCFG  *p_buf;

    if ((p_buf = (tBTA_AV_API_RCFG *) GKI_getbuf((UINT16) (sizeof(tBTA_AV_API_RCFG) + num_protect))) != NULL)
    {
        p_buf->hdr.layer_specific   = hndl;
        p_buf->hdr.event    = BTA_AV_API_RECONFIG_EVT;
        p_buf->num_protect  = num_protect;
        p_buf->suspend      = suspend;
        p_buf->sep_info_idx = sep_info_idx;
        p_buf->p_protect_info = (UINT8 *)(p_buf + 1);
        memcpy(p_buf->codec_info, p_codec_info, AVDT_CODEC_SIZE);
        memcpy(p_buf->p_protect_info, p_protect_info, num_protect);
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvProtectReq
**
** Description      Send a content protection request.  This function can only
**                  be used if AV is enabled with feature BTA_AV_FEAT_PROTECT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvProtectReq(tBTA_AV_HNDL hndl, UINT8 *p_data, UINT16 len)
{
    tBTA_AV_API_PROTECT_REQ  *p_buf;

    if ((p_buf = (tBTA_AV_API_PROTECT_REQ *) GKI_getbuf((UINT16) (sizeof(tBTA_AV_API_PROTECT_REQ) + len))) != NULL)
    {
        p_buf->hdr.layer_specific   = hndl;
        p_buf->hdr.event = BTA_AV_API_PROTECT_REQ_EVT;
        p_buf->len       = len;
        if (p_data == NULL)
        {
            p_buf->p_data = NULL;
        }
        else
        {
            p_buf->p_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->p_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvProtectRsp
**
** Description      Send a content protection response.  This function must
**                  be called if a BTA_AV_PROTECT_REQ_EVT is received.
**                  This function can only be used if AV is enabled with
**                  feature BTA_AV_FEAT_PROTECT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvProtectRsp(tBTA_AV_HNDL hndl, UINT8 error_code, UINT8 *p_data, UINT16 len)
{
    tBTA_AV_API_PROTECT_RSP  *p_buf;

    if ((p_buf = (tBTA_AV_API_PROTECT_RSP *) GKI_getbuf((UINT16) (sizeof(tBTA_AV_API_PROTECT_RSP) + len))) != NULL)
    {
        p_buf->hdr.layer_specific   = hndl;
        p_buf->hdr.event    = BTA_AV_API_PROTECT_RSP_EVT;
        p_buf->len          = len;
        p_buf->error_code   = error_code;
        if (p_data == NULL)
        {
            p_buf->p_data = NULL;
        }
        else
        {
            p_buf->p_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->p_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvRemoteCmd
**
** Description      Send a remote control command.  This function can only
**                  be used if AV is enabled with feature BTA_AV_FEAT_RCCT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvRemoteCmd(UINT8 rc_handle, UINT8 label, tBTA_AV_RC rc_id, tBTA_AV_STATE key_state)
{
    tBTA_AV_API_REMOTE_CMD  *p_buf;

    if ((p_buf = (tBTA_AV_API_REMOTE_CMD *) GKI_getbuf(sizeof(tBTA_AV_API_REMOTE_CMD))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_REMOTE_CMD_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->msg.op_id = rc_id;
        p_buf->msg.state = key_state;
        p_buf->msg.p_pass_data = NULL;
        p_buf->msg.pass_len = 0;
        p_buf->label = label;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvVendorCmd
**
** Description      Send a vendor dependent remote control command.  This
**                  function can only be used if AV is enabled with feature
**                  BTA_AV_FEAT_VENDOR.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvVendorCmd(UINT8 rc_handle, UINT8 label, tBTA_AV_CODE cmd_code, UINT8 *p_data, UINT16 len)
{
    tBTA_AV_API_VENDOR  *p_buf;

    if ((p_buf = (tBTA_AV_API_VENDOR *) GKI_getbuf((UINT16) (sizeof(tBTA_AV_API_VENDOR) + len))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_VENDOR_CMD_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->msg.hdr.ctype = cmd_code;
        p_buf->msg.hdr.subunit_type = AVRC_SUB_PANEL;
        p_buf->msg.hdr.subunit_id = 0;
        p_buf->msg.company_id = p_bta_av_cfg->company_id;
        p_buf->label = label;
        p_buf->msg.vendor_len = len;
        if (p_data == NULL)
        {
            p_buf->msg.p_vendor_data = NULL;
        }
        else
        {
            p_buf->msg.p_vendor_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->msg.p_vendor_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvVendorRsp
**
** Description      Send a vendor dependent remote control response.
**                  This function must be called if a BTA_AV_VENDOR_CMD_EVT
**                  is received. This function can only be used if AV is
**                  enabled with feature BTA_AV_FEAT_VENDOR.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvVendorRsp(UINT8 rc_handle, UINT8 label, tBTA_AV_CODE rsp_code, UINT8 *p_data, UINT16 len, UINT32 company_id)
{
    tBTA_AV_API_VENDOR  *p_buf;

    if ((p_buf = (tBTA_AV_API_VENDOR *) GKI_getbuf((UINT16) (sizeof(tBTA_AV_API_VENDOR) + len))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_VENDOR_RSP_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->msg.hdr.ctype = rsp_code;
        p_buf->msg.hdr.subunit_type = AVRC_SUB_PANEL;
        p_buf->msg.hdr.subunit_id = 0;
        if(company_id)
            p_buf->msg.company_id = company_id;
        else
            p_buf->msg.company_id = p_bta_av_cfg->company_id;
        p_buf->label = label;
        p_buf->msg.vendor_len = len;
        if (p_data == NULL)
        {
            p_buf->msg.p_vendor_data = NULL;
        }
        else
        {
            p_buf->msg.p_vendor_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->msg.p_vendor_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvOpenRc
**
** Description      Open an AVRCP connection toward the device with the
**                  specified handle
**
** Returns          void
**
*******************************************************************************/
void BTA_AvOpenRc(tBTA_AV_HNDL handle)
{
    tBTA_AV_API_OPEN_RC  *p_buf;

    if ((p_buf = (tBTA_AV_API_OPEN_RC *) GKI_getbuf(sizeof(tBTA_AV_API_OPEN_RC))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_RC_OPEN_EVT;
        p_buf->hdr.layer_specific   = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvCloseRc
**
** Description      Close an AVRCP connection
**
** Returns          void
**
*******************************************************************************/
void BTA_AvCloseRc(UINT8 rc_handle)
{
    tBTA_AV_API_CLOSE_RC  *p_buf;

    if ((p_buf = (tBTA_AV_API_CLOSE_RC *) GKI_getbuf(sizeof(tBTA_AV_API_CLOSE_RC))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_RC_CLOSE_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvMetaRsp
**
** Description      Send a Metadata/Advanced Control response. The message contained
**                  in p_pkt can be composed with AVRC utility functions.
**                  This function can only be used if AV is enabled with feature
**                  BTA_AV_FEAT_METADATA.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvMetaRsp(UINT8 rc_handle, UINT8 label, tBTA_AV_CODE rsp_code,
                               BT_HDR *p_pkt)
{
    tBTA_AV_API_META_RSP  *p_buf;

    if ((p_buf = (tBTA_AV_API_META_RSP *) GKI_getbuf((UINT16) (sizeof(tBTA_AV_API_META_RSP)))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_META_RSP_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->rsp_code = rsp_code;
        p_buf->p_pkt = p_pkt;
        p_buf->is_rsp = TRUE;
        p_buf->label = label;

        bta_sys_sendmsg(p_buf);
    } else if (p_pkt) {
        GKI_freebuf(p_pkt);
    }
}

/*******************************************************************************
**
** Function         BTA_AvMetaCmd
**
** Description      Send a Metadata/Advanced Control command. The message contained
**                  in p_pkt can be composed with AVRC utility functions.
**                  This function can only be used if AV is enabled with feature
**                  BTA_AV_FEAT_METADATA.
**                  This message is sent only when the peer supports the TG role.
*8                  The only command makes sense right now is the absolute volume command.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvMetaCmd(UINT8 rc_handle, UINT8 label, tBTA_AV_CMD cmd_code, BT_HDR *p_pkt)
{
    tBTA_AV_API_META_RSP  *p_buf;

    if ((p_buf = (tBTA_AV_API_META_RSP *) GKI_getbuf((UINT16) (sizeof(tBTA_AV_API_META_RSP)))) != NULL)
    {
        p_buf->hdr.event = BTA_AV_API_META_RSP_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->p_pkt = p_pkt;
        p_buf->rsp_code = cmd_code;
        p_buf->is_rsp = FALSE;
        p_buf->label = label;

        bta_sys_sendmsg(p_buf);
    }
}

#endif /* BTA_AV_INCLUDED */
