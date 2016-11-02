/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
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
 *  This module contains API of the audio/video distribution transport
 *  protocol.
 *
 ******************************************************************************/

#include <string.h>
#include "bt_types.h"
#include "bt_target.h"
#include "avdt_api.h"
#include "avdtc_api.h"
#include "avdt_int.h"
#include "l2c_api.h"
#include "btm_api.h"
#include "btu.h"


/* Control block for AVDT */
#if AVDT_DYNAMIC_MEMORY == FALSE
tAVDT_CB avdt_cb;
#endif


/*******************************************************************************
**
** Function         avdt_process_timeout
**
** Description      This function is called by BTU when an AVDTP timer
**                  expires.  The function sends a timer event to the
**                  appropriate CCB or SCB state machine.
**
**                  This function is for use internal to the stack only.
**
**
** Returns          void
**
*******************************************************************************/
void avdt_process_timeout(TIMER_LIST_ENT *p_tle)
{
    UINT8   event = 0;
    UINT8   err_code = AVDT_ERR_TIMEOUT;

    switch (p_tle->event)
    {
        case BTU_TTYPE_AVDT_CCB_RET:
            event = AVDT_CCB_RET_TOUT_EVT + AVDT_CCB_MKR;
            break;

        case BTU_TTYPE_AVDT_CCB_RSP:
            event = AVDT_CCB_RSP_TOUT_EVT + AVDT_CCB_MKR;
            break;

        case BTU_TTYPE_AVDT_CCB_IDLE:
            event = AVDT_CCB_IDLE_TOUT_EVT + AVDT_CCB_MKR;
            break;

        case BTU_TTYPE_AVDT_SCB_TC:
            event = AVDT_SCB_TC_TOUT_EVT;
            break;

        default:
            break;
    }

    if (event & AVDT_CCB_MKR)
    {
        avdt_ccb_event((tAVDT_CCB *) p_tle->param, (UINT8) (event & ~AVDT_CCB_MKR),
                       (tAVDT_CCB_EVT *) &err_code);
    }
    else
    {
        avdt_scb_event((tAVDT_SCB *) p_tle->param, event, NULL);
    }
}

/*******************************************************************************
**
** Function         AVDT_Register
**
** Description      This is the system level registration function for the
**                  AVDTP protocol.  This function initializes AVDTP and
**                  prepares the protocol stack for its use.  This function
**                  must be called once by the system or platform using AVDTP
**                  before the other functions of the API an be used.
**
**
** Returns          void
**
*******************************************************************************/
void AVDT_Register(tAVDT_REG *p_reg, tAVDT_CTRL_CBACK *p_cback)
{
    /* register PSM with L2CAP */
    L2CA_Register(AVDT_PSM, (tL2CAP_APPL_INFO *) &avdt_l2c_appl);

    /* set security level */
    BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_AVDTP, p_reg->sec_mask,
        AVDT_PSM, BTM_SEC_PROTO_AVDT, AVDT_CHAN_SIG);
    BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_AVDTP, p_reg->sec_mask,
        AVDT_PSM, BTM_SEC_PROTO_AVDT, AVDT_CHAN_SIG);

    /* do not use security on the media channel */
    BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_AVDTP_NOSEC, BTM_SEC_NONE,
        AVDT_PSM, BTM_SEC_PROTO_AVDT, AVDT_CHAN_MEDIA);
    BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_AVDTP_NOSEC, BTM_SEC_NONE,
        AVDT_PSM, BTM_SEC_PROTO_AVDT, AVDT_CHAN_MEDIA);

#if AVDT_REPORTING == TRUE
    /* do not use security on the reporting channel */
    BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_AVDTP_NOSEC, BTM_SEC_NONE,
        AVDT_PSM, BTM_SEC_PROTO_AVDT, AVDT_CHAN_REPORT);
    BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_AVDTP_NOSEC, BTM_SEC_NONE,
        AVDT_PSM, BTM_SEC_PROTO_AVDT, AVDT_CHAN_REPORT);
#endif

    /* initialize AVDTP data structures */
    avdt_scb_init();
    avdt_ccb_init();
    avdt_ad_init();

    /* copy registration struct */
    memcpy(&avdt_cb.rcb, p_reg, sizeof(tAVDT_REG));
    avdt_cb.p_conn_cback = p_cback;
}

/*******************************************************************************
**
** Function         AVDT_Deregister
**
** Description      This function is called to deregister use AVDTP protocol.
**                  It is called when AVDTP is no longer being used by any
**                  application in the system.  Before this function can be
**                  called, all streams must be removed with AVDT_RemoveStream().
**
**
** Returns          void
**
*******************************************************************************/
void AVDT_Deregister(void)
{
    /* deregister PSM with L2CAP */
    L2CA_Deregister(AVDT_PSM);
}

/*******************************************************************************
**
** Function         AVDT_SINK_Activate
**
** Description      Activate SEP of A2DP Sink. In Use parameter is adjusted.
**                  In Use will be made false in case of activation. A2DP SRC
**                  will receive in_use as false and can open A2DP Sink
**                  connection
**
** Returns          void.
**
*******************************************************************************/
void AVDT_SINK_Activate()
{
    tAVDT_SCB           *p_scb = &avdt_cb.scb[0];
    int                 i;
    AVDT_TRACE_DEBUG("AVDT_SINK_Activate");
    /* for all allocated scbs */
    for (i = 0; i < AVDT_NUM_SEPS; i++, p_scb++)
    {
        if ((p_scb->allocated) && (p_scb->cs.tsep == AVDT_TSEP_SNK))
        {
            AVDT_TRACE_DEBUG("AVDT_SINK_Activate found scb");
            p_scb->sink_activated = TRUE;
            /* update in_use */
            p_scb->in_use = FALSE;
            break;
        }
    }
}

/*******************************************************************************
**
** Function         AVDT_SINK_Deactivate
**
** Description      Deactivate SEP of A2DP Sink. In Use parameter is adjusted.
**                  In Use will be made TRUE in case of activation. A2DP SRC
**                  will receive in_use as true and will not open A2DP Sink
**                  connection
**
** Returns          void.
**
*******************************************************************************/
void AVDT_SINK_Deactivate()
{
    tAVDT_SCB           *p_scb = &avdt_cb.scb[0];
    int                 i;
    AVDT_TRACE_DEBUG("AVDT_SINK_Deactivate");
    /* for all allocated scbs */
    for (i = 0; i < AVDT_NUM_SEPS; i++, p_scb++)
    {
        if ((p_scb->allocated) && (p_scb->cs.tsep == AVDT_TSEP_SNK))
        {
            AVDT_TRACE_DEBUG("AVDT_SINK_Deactivate, found scb");
            p_scb->sink_activated = FALSE;
            /* update in_use */
            p_scb->in_use = TRUE;
            break;
        }
    }
}

void AVDT_AbortReq(UINT8 handle)
{
    AVDT_TRACE_ERROR("%s", __func__);

    tAVDT_SCB *p_scb = avdt_scb_by_hdl(handle);
    if (p_scb != NULL)
    {
        avdt_scb_event(p_scb, AVDT_SCB_API_ABORT_REQ_EVT, NULL);
    } else {
        AVDT_TRACE_ERROR("%s Improper SCB, can not abort the stream", __func__);
    }
}

/*******************************************************************************
**
** Function         AVDT_CreateStream
**
** Description      Create a stream endpoint.  After a stream endpoint is
**                  created an application can initiate a connection between
**                  this endpoint and an endpoint on a peer device.  In
**                  addition, a peer device can discover, get the capabilities,
**                  and connect to this endpoint.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_CreateStream(UINT8 *p_handle, tAVDT_CS *p_cs)
{
    UINT16      result = AVDT_SUCCESS;
    tAVDT_SCB   *p_scb;

    /* Verify parameters; if invalid, return failure */
    if (((p_cs->cfg.psc_mask & (~AVDT_PSC)) != 0) || (p_cs->p_ctrl_cback == NULL))
    {
        result = AVDT_BAD_PARAMS;
    }
    /* Allocate scb; if no scbs, return failure */
    else if ((p_scb = avdt_scb_alloc(p_cs)) == NULL)
    {
        result = AVDT_NO_RESOURCES;
    }
    else
    {
        *p_handle = avdt_scb_to_hdl(p_scb);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_RemoveStream
**
** Description      Remove a stream endpoint.  This function is called when
**                  the application is no longer using a stream endpoint.
**                  If this function is called when the endpoint is connected
**                  the connection is closed and then the stream endpoint
**                  is removed.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_RemoveStream(UINT8 handle)
{
    UINT16      result = AVDT_SUCCESS;
    tAVDT_SCB   *p_scb;

    /* look up scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    else
    {
        /* send remove event to scb */
        avdt_scb_event(p_scb, AVDT_SCB_API_REMOVE_EVT, NULL);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_DiscoverReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and discovers
**                  the stream endpoints on the peer device.  (Please note
**                  that AVDTP discovery is unrelated to SDP discovery).
**                  This function can be called at any time regardless of whether
**                  there is an AVDTP connection to the peer device.
**
**                  When discovery is complete, an AVDT_DISCOVER_CFM_EVT
**                  is sent to the application via its callback function.
**                  The application must not call AVDT_GetCapReq() or
**                  AVDT_DiscoverReq() again to the same device until
**                  discovery is complete.
**
**                  The memory addressed by sep_info is allocated by the
**                  application.  This memory is written to by AVDTP as part
**                  of the discovery procedure.  This memory must remain
**                  accessible until the application receives the
**                  AVDT_DISCOVER_CFM_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_DiscoverReq(BD_ADDR bd_addr, tAVDT_SEP_INFO *p_sep_info,
                        UINT8 max_seps, tAVDT_CTRL_CBACK *p_cback)
{
    tAVDT_CCB       *p_ccb;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_CCB_EVT   evt;

    /* find channel control block for this bd addr; if none, allocate one */
    if ((p_ccb = avdt_ccb_by_bd(bd_addr)) == NULL)
    {
        if ((p_ccb = avdt_ccb_alloc(bd_addr)) == NULL)
        {
            /* could not allocate channel control block */
            result = AVDT_NO_RESOURCES;
        }
    }

    if (result == AVDT_SUCCESS)
    {
        /* make sure no discovery or get capabilities req already in progress */
        if (p_ccb->proc_busy)
        {
            result = AVDT_BUSY;
        }
        /* send event to ccb */
        else
        {
            evt.discover.p_sep_info = p_sep_info;
            evt.discover.num_seps = max_seps;
            evt.discover.p_cback = p_cback;
            avdt_ccb_event(p_ccb, AVDT_CCB_API_DISCOVER_REQ_EVT, &evt);
        }
    }
    return result;
}

/*******************************************************************************
**
** Function         avdt_get_cap_req
**
** Description      internal function to serve both AVDT_GetCapReq and
**                  AVDT_GetAllCapReq
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
static UINT16 avdt_get_cap_req(BD_ADDR bd_addr, tAVDT_CCB_API_GETCAP *p_evt)
{
    tAVDT_CCB       *p_ccb = NULL;
    UINT16          result = AVDT_SUCCESS;

    /* verify SEID */
    if ((p_evt->single.seid < AVDT_SEID_MIN) || (p_evt->single.seid > AVDT_SEID_MAX))
    {
        AVDT_TRACE_ERROR("seid: %d", p_evt->single.seid);
        result = AVDT_BAD_PARAMS;
    }
    /* find channel control block for this bd addr; if none, allocate one */
    else if ((p_ccb = avdt_ccb_by_bd(bd_addr)) == NULL)
    {
        if ((p_ccb = avdt_ccb_alloc(bd_addr)) == NULL)
        {
            /* could not allocate channel control block */
            result = AVDT_NO_RESOURCES;
        }
    }

    if (result == AVDT_SUCCESS)
    {
        /* make sure no discovery or get capabilities req already in progress */
        if (p_ccb->proc_busy)
        {
            result = AVDT_BUSY;
        }
        /* send event to ccb */
        else
        {
            avdt_ccb_event(p_ccb, AVDT_CCB_API_GETCAP_REQ_EVT, (tAVDT_CCB_EVT *)p_evt);
        }
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_GetCapReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and gets the
**                  capabilities of a stream endpoint on the peer device.
**                  This function can be called at any time regardless of
**                  whether there is an AVDTP connection to the peer device.
**
**                  When the procedure is complete, an AVDT_GETCAP_CFM_EVT is
**                  sent to the application via its callback function.  The
**                  application must not call AVDT_GetCapReq() or
**                  AVDT_DiscoverReq() again until the procedure is complete.
**
**                  The memory pointed to by p_cfg is allocated by the
**                  application.  This memory is written to by AVDTP as part
**                  of the get capabilities procedure.  This memory must
**                  remain accessible until the application receives
**                  the AVDT_GETCAP_CFM_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_GetCapReq(BD_ADDR bd_addr, UINT8 seid, tAVDT_CFG *p_cfg, tAVDT_CTRL_CBACK *p_cback)
{
    tAVDT_CCB_API_GETCAP    getcap;

    getcap.single.seid = seid;
    getcap.single.sig_id = AVDT_SIG_GETCAP;
    getcap.p_cfg = p_cfg;
    getcap.p_cback = p_cback;
    return avdt_get_cap_req (bd_addr, &getcap);
}

/*******************************************************************************
**
** Function         AVDT_GetAllCapReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and gets the
**                  capabilities of a stream endpoint on the peer device.
**                  This function can be called at any time regardless of
**                  whether there is an AVDTP connection to the peer device.
**
**                  When the procedure is complete, an AVDT_GETCAP_CFM_EVT is
**                  sent to the application via its callback function.  The
**                  application must not call AVDT_GetCapReq() or
**                  AVDT_DiscoverReq() again until the procedure is complete.
**
**                  The memory pointed to by p_cfg is allocated by the
**                  application.  This memory is written to by AVDTP as part
**                  of the get capabilities procedure.  This memory must
**                  remain accessible until the application receives
**                  the AVDT_GETCAP_CFM_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_GetAllCapReq(BD_ADDR bd_addr, UINT8 seid, tAVDT_CFG *p_cfg, tAVDT_CTRL_CBACK *p_cback)
{
    tAVDT_CCB_API_GETCAP    getcap;

    getcap.single.seid = seid;
    getcap.single.sig_id = AVDT_SIG_GET_ALLCAP;
    getcap.p_cfg = p_cfg;
    getcap.p_cback = p_cback;
    return avdt_get_cap_req (bd_addr, &getcap);
}

/*******************************************************************************
**
** Function         AVDT_DelayReport
**
** Description      This functions sends a Delay Report to the peer device
**                  that is associated with a particular SEID.
**                  This function is called by SNK device.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_DelayReport(UINT8 handle, UINT8 seid, UINT16 delay)
{
    tAVDT_SCB       *p_scb;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_SCB_EVT   evt;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    else
    /* send event to scb */
    {
        evt.apidelay.hdr.seid   = seid;
        evt.apidelay.delay      = delay;
        avdt_scb_event(p_scb, AVDT_SCB_API_DELAY_RPT_REQ_EVT, &evt);
    }

    return result;
}

/*******************************************************************************
**
** Function         AVDT_OpenReq
**
** Description      This function initiates a connection to the AVDTP service
**                  on the peer device, if not already present, and connects
**                  to a stream endpoint on a peer device.  When the connection
**                  is completed, an AVDT_OPEN_CFM_EVT is sent to the
**                  application via the control callback function for this handle.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_OpenReq(UINT8 handle, BD_ADDR bd_addr, UINT8 seid, tAVDT_CFG *p_cfg)
{
    tAVDT_CCB       *p_ccb = NULL;
    tAVDT_SCB       *p_scb = NULL;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_SCB_EVT   evt;

    /* verify SEID */
    if ((seid < AVDT_SEID_MIN) || (seid > AVDT_SEID_MAX))
    {
        result = AVDT_BAD_PARAMS;
    }
    /* map handle to scb */
    else if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    /* find channel control block for this bd addr; if none, allocate one */
    else if ((p_ccb = avdt_ccb_by_bd(bd_addr)) == NULL)
    {
        if ((p_ccb = avdt_ccb_alloc(bd_addr)) == NULL)
        {
            /* could not allocate channel control block */
            result = AVDT_NO_RESOURCES;
        }
    }

    /* send event to scb */
    if (result == AVDT_SUCCESS)
    {
        evt.msg.config_cmd.hdr.seid = seid;
        evt.msg.config_cmd.hdr.ccb_idx = avdt_ccb_to_idx(p_ccb);
        evt.msg.config_cmd.int_seid = handle;
        evt.msg.config_cmd.p_cfg = p_cfg;
        avdt_scb_event(p_scb, AVDT_SCB_API_SETCONFIG_REQ_EVT, &evt);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_ConfigRsp
**
** Description      Respond to a configure request from the peer device.  This
**                  function must be called if the application receives an
**                  AVDT_CONFIG_IND_EVT through its control callback.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_ConfigRsp(UINT8 handle, UINT8 label, UINT8 error_code, UINT8 category)
{
    tAVDT_SCB       *p_scb;
    tAVDT_SCB_EVT   evt;
    UINT16          result = AVDT_SUCCESS;
    UINT8           event_code;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    /* handle special case when this function is called but peer has not send
    ** a configuration cmd; ignore and return error result
    */
    else if (!p_scb->in_use)
    {
        result = AVDT_BAD_HANDLE;
    }
    /* send event to scb */
    else
    {
        evt.msg.hdr.err_code = error_code;
        evt.msg.hdr.err_param = category;
        evt.msg.hdr.label = label;
        if (error_code == 0)
        {
            event_code = AVDT_SCB_API_SETCONFIG_RSP_EVT;
        }
        else
        {
            event_code = AVDT_SCB_API_SETCONFIG_REJ_EVT;
        }
        avdt_scb_event(p_scb, event_code, &evt);
    }

    return result;
}

/*******************************************************************************
**
** Function         AVDT_StartReq
**
** Description      Start one or more stream endpoints.  This initiates the
**                  transfer of media packets for the streams.  All stream
**                  endpoints must previously be opened.  When the streams
**                  are started, an AVDT_START_CFM_EVT is sent to the
**                  application via the control callback function for each stream.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_StartReq(UINT8 *p_handles, UINT8 num_handles)
{
    tAVDT_SCB       *p_scb = NULL;
    tAVDT_CCB_EVT   evt;
    UINT16          result = AVDT_SUCCESS;
    int             i;

    if ((num_handles == 0) || (num_handles > AVDT_NUM_SEPS))
    {
        result = AVDT_BAD_PARAMS;
    }
    else
    {
        /* verify handles */
        for (i = 0; i < num_handles; i++)
        {
            if ((p_scb = avdt_scb_by_hdl(p_handles[i])) == NULL)
            {
                result = AVDT_BAD_HANDLE;
                break;
            }
        }
    }

    if (result == AVDT_SUCCESS)
    {
        if (p_scb->p_ccb == NULL)
        {
            result = AVDT_BAD_HANDLE;
        }
        else
        {
            /* send event to ccb */
            memcpy(evt.msg.multi.seid_list, p_handles, num_handles);
            evt.msg.multi.num_seps = num_handles;
            avdt_ccb_event(p_scb->p_ccb, AVDT_CCB_API_START_REQ_EVT, &evt);
        }
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_SuspendReq
**
** Description      Suspend one or more stream endpoints. This suspends the
**                  transfer of media packets for the streams.  All stream
**                  endpoints must previously be open and started.  When the
**                  streams are suspended, an AVDT_SUSPEND_CFM_EVT is sent to
**                  the application via the control callback function for
**                  each stream.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_SuspendReq(UINT8 *p_handles, UINT8 num_handles)
{
    tAVDT_SCB       *p_scb = NULL;
    tAVDT_CCB_EVT   evt;
    UINT16          result = AVDT_SUCCESS;
    int             i;

    if ((num_handles == 0) || (num_handles > AVDT_NUM_SEPS))
    {
        result = AVDT_BAD_PARAMS;
    }
    else
    {
        /* verify handles */
        for (i = 0; i < num_handles; i++)
        {
            if ((p_scb = avdt_scb_by_hdl(p_handles[i])) == NULL)
            {
                result = AVDT_BAD_HANDLE;
                break;
            }
        }
    }

    if (result == AVDT_SUCCESS)
    {
        if (p_scb->p_ccb == NULL)
        {
            result = AVDT_BAD_HANDLE;
        }
        else
        {
            /* send event to ccb */
            memcpy(evt.msg.multi.seid_list, p_handles, num_handles);
            evt.msg.multi.num_seps = num_handles;
            avdt_ccb_event(p_scb->p_ccb, AVDT_CCB_API_SUSPEND_REQ_EVT, &evt);
        }
    }

    return result;
}

/*******************************************************************************
**
** Function         AVDT_CloseReq
**
** Description      Close a stream endpoint.  This stops the transfer of media
**                  packets and closes the transport channel associated with
**                  this stream endpoint.  When the stream is closed, an
**                  AVDT_CLOSE_CFM_EVT is sent to the application via the
**                  control callback function for this handle.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_CloseReq(UINT8 handle)
{
    tAVDT_SCB       *p_scb;
    UINT16          result = AVDT_SUCCESS;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    else
    /* send event to scb */
    {
        avdt_scb_event(p_scb, AVDT_SCB_API_CLOSE_REQ_EVT, NULL);
    }

    return result;
}

/*******************************************************************************
**
** Function         AVDT_ReconfigReq
**
** Description      Reconfigure a stream endpoint.  This allows the application
**                  to change the codec or content protection capabilities of
**                  a stream endpoint after it has been opened.  This function
**                  can only be called if the stream is opened but not started
**                  or if the stream has been suspended.  When the procedure
**                  is completed, an AVDT_RECONFIG_CFM_EVT is sent to the
**                  application via the control callback function for this handle.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_ReconfigReq(UINT8 handle, tAVDT_CFG *p_cfg)
{
    tAVDT_SCB       *p_scb;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_SCB_EVT   evt;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    /* send event to scb */
    else
    {
        /* force psc_mask to zero */
        p_cfg->psc_mask = 0;

        evt.msg.reconfig_cmd.p_cfg = p_cfg;
        avdt_scb_event(p_scb, AVDT_SCB_API_RECONFIG_REQ_EVT, &evt);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_ReconfigRsp
**
** Description      Respond to a reconfigure request from the peer device.
**                  This function must be called if the application receives
**                  an AVDT_RECONFIG_IND_EVT through its control callback.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_ReconfigRsp(UINT8 handle, UINT8 label, UINT8 error_code, UINT8 category)
{
    tAVDT_SCB       *p_scb;
    tAVDT_SCB_EVT   evt;
    UINT16          result = AVDT_SUCCESS;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    /* send event to scb */
    else
    {
        evt.msg.hdr.err_code = error_code;
        evt.msg.hdr.err_param = category;
        evt.msg.hdr.label = label;
        avdt_scb_event(p_scb, AVDT_SCB_API_RECONFIG_RSP_EVT, &evt);
    }

    return result;
}

/*******************************************************************************
**
** Function         AVDT_SecurityReq
**
** Description      Send a security request to the peer device.  When the
**                  security procedure is completed, an AVDT_SECURITY_CFM_EVT
**                  is sent to the application via the control callback function
**                  for this handle.  (Please note that AVDTP security procedures
**                  are unrelated to Bluetooth link level security.)
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_SecurityReq(UINT8 handle, UINT8 *p_data, UINT16 len)
{
    tAVDT_SCB       *p_scb;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_SCB_EVT   evt;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    /* send event to scb */
    else
    {
        evt.msg.security_rsp.p_data = p_data;
        evt.msg.security_rsp.len = len;
        avdt_scb_event(p_scb, AVDT_SCB_API_SECURITY_REQ_EVT, &evt);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_SecurityRsp
**
** Description      Respond to a security request from the peer device.
**                  This function must be called if the application receives
**                  an AVDT_SECURITY_IND_EVT through its control callback.
**                  (Please note that AVDTP security procedures are unrelated
**                  to Bluetooth link level security.)
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_SecurityRsp(UINT8 handle, UINT8 label, UINT8 error_code,
                        UINT8 *p_data, UINT16 len)
{
    tAVDT_SCB       *p_scb;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_SCB_EVT   evt;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    /* send event to scb */
    else
    {
        evt.msg.security_rsp.hdr.err_code = error_code;
        evt.msg.security_rsp.hdr.label = label;
        evt.msg.security_rsp.p_data = p_data;
        evt.msg.security_rsp.len = len;
        avdt_scb_event(p_scb, AVDT_SCB_API_SECURITY_RSP_EVT, &evt);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_WriteReqOpt
**
** Description      Send a media packet to the peer device.  The stream must
**                  be started before this function is called.  Also, this
**                  function can only be called if the stream is a SRC.
**
**                  When AVDTP has sent the media packet and is ready for the
**                  next packet, an AVDT_WRITE_CFM_EVT is sent to the
**                  application via the control callback.  The application must
**                  wait for the AVDT_WRITE_CFM_EVT before it makes the next
**                  call to AVDT_WriteReq().  If the applications calls
**                  AVDT_WriteReq() before it receives the event the packet
**                  will not be sent.  The application may make its first call
**                  to AVDT_WriteReq() after it receives an AVDT_START_CFM_EVT
**                  or AVDT_START_IND_EVT.
**
**                  The application passes the packet using the BT_HDR structure.
**                  This structure is described in section 2.1.  The offset
**                  field must be equal to or greater than AVDT_MEDIA_OFFSET
**                  (if NO_RTP is specified, L2CAP_MIN_OFFSET can be used).
**                  This allows enough space in the buffer for the L2CAP and
**                  AVDTP headers.
**
**                  The memory pointed to by p_pkt must be a GKI buffer
**                  allocated by the application.  This buffer will be freed
**                  by the protocol stack; the application must not free
**                  this buffer.
**
**                  The opt parameter allows passing specific options like:
**                  - NO_RTP : do not add the RTP header to buffer
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_WriteReqOpt(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp, UINT8 m_pt, tAVDT_DATA_OPT_MASK opt)
{
    tAVDT_SCB       *p_scb;
    tAVDT_SCB_EVT   evt;
    UINT16          result = AVDT_SUCCESS;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    else
    {
        evt.apiwrite.p_buf = p_pkt;
        evt.apiwrite.time_stamp = time_stamp;
        evt.apiwrite.m_pt = m_pt;
        evt.apiwrite.opt = opt;
#if AVDT_MULTIPLEXING == TRUE
        GKI_init_q (&evt.apiwrite.frag_q);
#endif
        avdt_scb_event(p_scb, AVDT_SCB_API_WRITE_REQ_EVT, &evt);
    }

    return result;
}

/*******************************************************************************
**
** Function         AVDT_WriteReq
**
** Description      Send a media packet to the peer device.  The stream must
**                  be started before this function is called.  Also, this
**                  function can only be called if the stream is a SRC.
**
**                  When AVDTP has sent the media packet and is ready for the
**                  next packet, an AVDT_WRITE_CFM_EVT is sent to the
**                  application via the control callback.  The application must
**                  wait for the AVDT_WRITE_CFM_EVT before it makes the next
**                  call to AVDT_WriteReq().  If the applications calls
**                  AVDT_WriteReq() before it receives the event the packet
**                  will not be sent.  The application may make its first call
**                  to AVDT_WriteReq() after it receives an AVDT_START_CFM_EVT
**                  or AVDT_START_IND_EVT.
**
**                  The application passes the packet using the BT_HDR structure.
**                  This structure is described in section 2.1.  The offset
**                  field must be equal to or greater than AVDT_MEDIA_OFFSET.
**                  This allows enough space in the buffer for the L2CAP and
**                  AVDTP headers.
**
**                  The memory pointed to by p_pkt must be a GKI buffer
**                  allocated by the application.  This buffer will be freed
**                  by the protocol stack; the application must not free
**                  this buffer.
**
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_WriteReq(UINT8 handle, BT_HDR *p_pkt, UINT32 time_stamp, UINT8 m_pt)
{
    return AVDT_WriteReqOpt(handle, p_pkt, time_stamp, m_pt, AVDT_DATA_OPT_NONE);
}

/*******************************************************************************
**
** Function         AVDT_ConnectReq
**
** Description      This function initiates an AVDTP signaling connection
**                  to the peer device.  When the connection is completed, an
**                  AVDT_CONNECT_IND_EVT is sent to the application via its
**                  control callback function.  If the connection attempt fails
**                  an AVDT_DISCONNECT_IND_EVT is sent.  The security mask
**                  parameter overrides the outgoing security mask set in
**                  AVDT_Register().
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_ConnectReq(BD_ADDR bd_addr, UINT8 sec_mask, tAVDT_CTRL_CBACK *p_cback)
{
    tAVDT_CCB       *p_ccb = NULL;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_CCB_EVT   evt;

    /* find channel control block for this bd addr; if none, allocate one */
    if ((p_ccb = avdt_ccb_by_bd(bd_addr)) == NULL)
    {
        if ((p_ccb = avdt_ccb_alloc(bd_addr)) == NULL)
        {
            /* could not allocate channel control block */
            result = AVDT_NO_RESOURCES;
        }
    }
    else if (p_ccb->ll_opened == FALSE)
    {
        AVDT_TRACE_WARNING("AVDT_ConnectReq: CCB LL is in the middle of opening");

        /* ccb was already allocated for the incoming signalling. */
        result = AVDT_BUSY;
    }

    if (result == AVDT_SUCCESS)
    {
        /* send event to ccb */
        evt.connect.p_cback = p_cback;
        evt.connect.sec_mask = sec_mask;
        avdt_ccb_event(p_ccb, AVDT_CCB_API_CONNECT_REQ_EVT, &evt);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_DisconnectReq
**
** Description      This function disconnect an AVDTP signaling connection
**                  to the peer device.  When disconnected an
**                  AVDT_DISCONNECT_IND_EVT is sent to the application via its
**                  control callback function.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
UINT16 AVDT_DisconnectReq(BD_ADDR bd_addr, tAVDT_CTRL_CBACK *p_cback)
{
    tAVDT_CCB       *p_ccb = NULL;
    UINT16          result = AVDT_SUCCESS;
    tAVDT_CCB_EVT   evt;

    /* find channel control block for this bd addr; if none, error */
    if ((p_ccb = avdt_ccb_by_bd(bd_addr)) == NULL)
    {
        result = AVDT_BAD_PARAMS;
    }

    if (result == AVDT_SUCCESS)
    {
        /* send event to ccb */
        evt.disconnect.p_cback = p_cback;
        avdt_ccb_event(p_ccb, AVDT_CCB_API_DISCONNECT_REQ_EVT, &evt);
    }
    return result;
}

/*******************************************************************************
**
** Function         AVDT_GetL2CapChannel
**
** Description      Get the L2CAP CID used by the handle.
**
** Returns          CID if successful, otherwise 0.
**
*******************************************************************************/
UINT16 AVDT_GetL2CapChannel(UINT8 handle)
{
    tAVDT_SCB       *p_scb;
    tAVDT_CCB       *p_ccb;
    UINT8           tcid;
    UINT16          lcid = 0;

    /* map handle to scb */
    if (((p_scb = avdt_scb_by_hdl(handle)) != NULL)
     && ((p_ccb = p_scb->p_ccb) != NULL))
    {
        /* get tcid from type, scb */
        tcid = avdt_ad_type_to_tcid(AVDT_CHAN_MEDIA, p_scb);

        lcid = avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][tcid].lcid;
    }

    return (lcid);
}

/*******************************************************************************
**
** Function         AVDT_GetSignalChannel
**
** Description      Get the L2CAP CID used by the signal channel of the given handle.
**
** Returns          CID if successful, otherwise 0.
**
*******************************************************************************/
UINT16 AVDT_GetSignalChannel(UINT8 handle, BD_ADDR bd_addr)
{
    tAVDT_SCB       *p_scb;
    tAVDT_CCB       *p_ccb;
    UINT8           tcid = 0; /* tcid is always 0 for signal channel */
    UINT16          lcid = 0;

    /* map handle to scb */
    if (((p_scb = avdt_scb_by_hdl(handle)) != NULL)
     && ((p_ccb = p_scb->p_ccb) != NULL))
    {
        lcid = avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][tcid].lcid;
    }
    else if ((p_ccb = avdt_ccb_by_bd(bd_addr)) != NULL)
    {
        lcid = avdt_cb.ad.rt_tbl[avdt_ccb_to_idx(p_ccb)][tcid].lcid;
    }

    return (lcid);
}

#if AVDT_MULTIPLEXING == TRUE
/*******************************************************************************
**
** Function         AVDT_WriteDataReq
**
** Description      Send a media packet to the peer device.  The stream must
**                  be started before this function is called.  Also, this
**                  function can only be called if the stream is a SRC.
**
**                  When AVDTP has sent the media packet and is ready for the
**                  next packet, an AVDT_WRITE_CFM_EVT is sent to the
**                  application via the control callback.  The application must
**                  wait for the AVDT_WRITE_CFM_EVT before it makes the next
**                  call to AVDT_WriteDataReq().  If the applications calls
**                  AVDT_WriteDataReq() before it receives the event the packet
**                  will not be sent.  The application may make its first call
**                  to AVDT_WriteDataReq() after it receives an
**                  AVDT_START_CFM_EVT or AVDT_START_IND_EVT.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_WriteDataReq(UINT8 handle, UINT8 *p_data, UINT32 data_len,
                                UINT32 time_stamp, UINT8 m_pt, UINT8 marker)
{

    tAVDT_SCB       *p_scb;
    tAVDT_SCB_EVT   evt;
    UINT16          result = AVDT_SUCCESS;

    do
    {
        /* check length of media frame */
        if(data_len > AVDT_MAX_MEDIA_SIZE)
        {
            result = AVDT_BAD_PARAMS;
            break;
        }
        /* map handle to scb */
        if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
        {
            result = AVDT_BAD_HANDLE;
            break;
        }
        AVDT_TRACE_WARNING("mux_tsid_media:%d", p_scb->curr_cfg.mux_tsid_media);

        if (p_scb->p_pkt != NULL
            || p_scb->p_ccb == NULL
            || !GKI_queue_is_empty(&p_scb->frag_q)
            || p_scb->frag_off != 0
            || p_scb->curr_cfg.mux_tsid_media == 0)
        {
            result = AVDT_ERR_BAD_STATE;
            AVDT_TRACE_WARNING("p_scb->p_pkt=%x, p_scb->p_ccb=%x, IsQueueEmpty=%x, p_scb->frag_off=%x",
                p_scb->p_pkt, p_scb->p_ccb, GKI_queue_is_empty(&p_scb->frag_q), p_scb->frag_off);
            break;
        }
        evt.apiwrite.p_buf = 0; /* it will indicate using of fragments queue frag_q */
        /* create queue of media fragments */
        GKI_init_q (&evt.apiwrite.frag_q);

        /* compose fragments from media payload and put fragments into gueue */
        avdt_scb_queue_frags(p_scb, &p_data, &data_len, &evt.apiwrite.frag_q);

        if(GKI_queue_is_empty(&evt.apiwrite.frag_q))
        {
            AVDT_TRACE_WARNING("AVDT_WriteDataReq out of GKI buffers");
            result = AVDT_ERR_RESOURCE;
            break;
        }
        evt.apiwrite.data_len = data_len;
        evt.apiwrite.p_data = p_data;

        /* process the fragments queue */
        evt.apiwrite.time_stamp = time_stamp;
        evt.apiwrite.m_pt = m_pt | (marker<<7);
        avdt_scb_event(p_scb, AVDT_SCB_API_WRITE_REQ_EVT, &evt);
    } while (0);

#if (BT_USE_TRACES == TRUE)
    if(result != AVDT_SUCCESS)
    {
        AVDT_TRACE_WARNING("*** AVDT_WriteDataReq failed result=%d",result);
    }
#endif
    return result;
}
#endif

#if AVDT_MULTIPLEXING == TRUE
/*******************************************************************************
**
** Function         AVDT_SetMediaBuf
**
** Description      Assigns buffer for media packets or forbids using of assigned
**                  buffer if argument p_buf is NULL. This function can only
**                  be called if the stream is a SNK.
**
**                  AVDTP uses this buffer to reassemble fragmented media packets.
**                  When AVDTP receives a complete media packet, it calls the
**                  p_media_cback assigned by AVDT_CreateStream().
**                  This function can be called during callback to assign a
**                  different buffer for next media packet or can leave the current
**                  buffer for next packet.
**
** Returns          AVDT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
extern UINT16 AVDT_SetMediaBuf(UINT8 handle, UINT8 *p_buf, UINT32 buf_len)
{
    tAVDT_SCB       *p_scb;
    UINT16          result = AVDT_SUCCESS;

    /* map handle to scb */
    if ((p_scb = avdt_scb_by_hdl(handle)) == NULL)
    {
        result = AVDT_BAD_HANDLE;
    }
    else
    {
        if(p_buf && p_scb->cs.p_media_cback == NULL)
            result = AVDT_NO_RESOURCES;
        else
        {
            p_scb->p_media_buf = p_buf;
            p_scb->media_buf_len = buf_len;
        }
    }

    return result;
}
#endif

#if AVDT_REPORTING == TRUE
/*******************************************************************************
**
** Function         AVDT_SendReport
**
** Description
**
**
**
** Returns
**
*******************************************************************************/
UINT16 AVDT_SendReport(UINT8 handle, AVDT_REPORT_TYPE type,
                       tAVDT_REPORT_DATA *p_data)
{
    tAVDT_SCB       *p_scb;
    UINT16          result = AVDT_BAD_PARAMS;
    BT_HDR          *p_pkt;
    tAVDT_TC_TBL    *p_tbl;
    UINT8           *p, *plen, *pm1, *p_end;
#if AVDT_MULTIPLEXING == TRUE
    UINT8           *p_al=NULL, u;
#endif
    UINT32  ssrc;
    UINT16  len;

    /* map handle to scb && verify parameters */
    if (((p_scb = avdt_scb_by_hdl(handle)) != NULL)
     && (p_scb->p_ccb != NULL)
     && (((type == AVDT_RTCP_PT_SR) && (p_scb->cs.tsep == AVDT_TSEP_SRC)) ||
        ((type == AVDT_RTCP_PT_RR) && (p_scb->cs.tsep == AVDT_TSEP_SNK)) ||
        (type == AVDT_RTCP_PT_SDES)) )
    {
        result = AVDT_NO_RESOURCES;

        /* build SR - assume fit in one packet */
        p_tbl = avdt_ad_tc_tbl_by_type(AVDT_CHAN_REPORT, p_scb->p_ccb, p_scb);
        if((p_tbl->state == AVDT_AD_ST_OPEN) &&
            (p_pkt = (BT_HDR *)GKI_getbuf(p_tbl->peer_mtu)) != NULL)
        {
            p_pkt->offset = L2CAP_MIN_OFFSET;
            p = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
#if AVDT_MULTIPLEXING == TRUE
            if(p_scb->curr_cfg.psc_mask & AVDT_PSC_MUX)
            {
                /* Adaptation Layer header later */
                p_al = p;
                p += 2;
            }
#endif
            pm1 = p;
            *p++ = AVDT_MEDIA_OCTET1 | 1;
            *p++ = type;
            /* save the location for length */
            plen = p;
            p+= 2;
            ssrc = avdt_scb_gen_ssrc(p_scb);
            UINT32_TO_BE_STREAM(p, ssrc);

            switch(type)
            {
            case AVDT_RTCP_PT_SR:   /* Sender Report */
                *pm1 = AVDT_MEDIA_OCTET1;
                UINT32_TO_BE_STREAM(p, p_data->sr.ntp_sec);
                UINT32_TO_BE_STREAM(p, p_data->sr.ntp_frac);
                UINT32_TO_BE_STREAM(p, p_data->sr.rtp_time);
                UINT32_TO_BE_STREAM(p, p_data->sr.pkt_count);
                UINT32_TO_BE_STREAM(p, p_data->sr.octet_count);
                break;

            case AVDT_RTCP_PT_RR:   /* Receiver Report */
                *p++ = p_data->rr.frag_lost;
                AVDT_TRACE_API("packet_lost: %d", p_data->rr.packet_lost);
                p_data->rr.packet_lost &= 0xFFFFFF;
                AVDT_TRACE_API("packet_lost: %d", p_data->rr.packet_lost);
                UINT24_TO_BE_STREAM(p, p_data->rr.packet_lost);
                UINT32_TO_BE_STREAM(p, p_data->rr.seq_num_rcvd);
                UINT32_TO_BE_STREAM(p, p_data->rr.jitter);
                UINT32_TO_BE_STREAM(p, p_data->rr.lsr);
                UINT32_TO_BE_STREAM(p, p_data->rr.dlsr);
                break;

            case AVDT_RTCP_PT_SDES: /* Source Description */
                *p++ = AVDT_RTCP_SDES_CNAME;
                len = strlen((char *)p_data->cname);
                if(len > AVDT_MAX_CNAME_SIZE)
                    len = AVDT_MAX_CNAME_SIZE;
                *p++ = (UINT8)len;
                BCM_STRNCPY_S((char *)p, len+1, (char *)p_data->cname, len+1);
                p += len;
                break;
            }
            p_end = p;
            len = p - pm1 - 1;
            UINT16_TO_BE_STREAM(plen, len);

#if AVDT_MULTIPLEXING == TRUE
            if(p_scb->curr_cfg.psc_mask & AVDT_PSC_MUX)
            {
                /* Adaptation Layer header */
                p = p_al;
                len++;
                UINT16_TO_BE_STREAM(p_al, len );
                /* TSID, no-fragment bit and coding of length(9-bit length field) */
                u = *p;
                *p = (p_scb->curr_cfg.mux_tsid_report<<3) | AVDT_ALH_LCODE_9BITM0;
                if(u)
                    *p |= AVDT_ALH_LCODE_9BITM1;
            }
#endif

            /* set the actual payload length */
            p_pkt->len = p_end - p;
            /* send the packet */
            if(L2CAP_DW_FAILED != avdt_ad_write_req(AVDT_CHAN_REPORT, p_scb->p_ccb, p_scb, p_pkt))
                result = AVDT_SUCCESS;
        }
    }

    return result;
}
#endif

/******************************************************************************
**
** Function         AVDT_SetTraceLevel
**
** Description      Sets the trace level for AVDT. If 0xff is passed, the
**                  current trace level is returned.
**
**                  Input Parameters:
**                      new_level:  The level to set the AVDT tracing to:
**                      0xff-returns the current setting.
**                      0-turns off tracing.
**                      >= 1-Errors.
**                      >= 2-Warnings.
**                      >= 3-APIs.
**                      >= 4-Events.
**                      >= 5-Debug.
**
** Returns          The new trace level or current trace level if
**                  the input parameter is 0xff.
**
******************************************************************************/
UINT8 AVDT_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF)
        avdt_cb.trace_level = new_level;

    return (avdt_cb.trace_level);
}
