/******************************************************************************
 *
 *  Copyright (C) 2006-2012 Broadcom Corporation
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
 *  This is the implementation of the JAVA API for Bluetooth Wireless
 *  Technology (JABWT) as specified by the JSR82 specification
 *
 ******************************************************************************/

#include "bta/bta_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_jv_api.h"
#include "bta_jv_int.h"
#include "osi/allocator.h"
#include <string.h>
#include "stack/port_api.h"
#include "stack/sdp_api.h"
#include "bta/utl.h"
#include "stack/gap_api.h"

#include "common/bt_target.h"
#include "stack/sdp_api.h"


#if (defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_jv_reg = {
    bta_jv_sm_execute,
    NULL
};

/*******************************************************************************
**
** Function         BTA_JvEnable
**
** Description      Enable the Java I/F service. When the enable
**                  operation is complete the callback function will be
**                  called with a BTA_JV_ENABLE_EVT. This function must
**                  be called before other function in the JV API are
**                  called.
**
** Returns          BTA_JV_SUCCESS if successful.
**                  BTA_JV_FAIL if internal failure.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvEnable(tBTA_JV_DM_CBACK *p_cback)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_ENABLE  *p_buf;
    int i;
    APPL_TRACE_API( "BTA_JvEnable");

#if BTA_DYNAMIC_MEMORY == TRUE
    /* Malloc buffer for JV configuration structure */
    p_bta_jv_cfg->p_sdp_raw_data = (UINT8 *)osi_malloc(p_bta_jv_cfg->sdp_raw_size);
    p_bta_jv_cfg->p_sdp_db = (tSDP_DISCOVERY_DB *)osi_malloc(p_bta_jv_cfg->sdp_db_size);
    if (p_bta_jv_cfg->p_sdp_raw_data == NULL || p_bta_jv_cfg->p_sdp_db == NULL) {
        return BTA_JV_NO_DATA;
    }
#endif

    if (p_cback && FALSE == bta_sys_is_register(BTA_ID_JV)) {
        memset(&bta_jv_cb, 0, sizeof(tBTA_JV_CB));
        /* set handle to invalid value by default */
        for (i = 0; i < BTA_JV_PM_MAX_NUM; i++) {
            bta_jv_cb.pm_cb[i].handle = BTA_JV_PM_HANDLE_CLEAR;
        }

        /* register with BTA system manager */
        bta_sys_register(BTA_ID_JV, &bta_jv_reg);

        if (p_cback && (p_buf = (tBTA_JV_API_ENABLE *) osi_malloc(sizeof(tBTA_JV_API_ENABLE))) != NULL) {
            p_buf->hdr.event = BTA_JV_API_ENABLE_EVT;
            p_buf->p_cback = p_cback;
            bta_sys_sendmsg(p_buf);
            status = BTA_JV_SUCCESS;
        }
    } else if (p_cback == NULL) {
        APPL_TRACE_ERROR("No p_cback.");
    } else {
        APPL_TRACE_WARNING("No need to Init again.");
        status = BTA_JV_ALREADY_DONE;
    }
    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvDisable
**
** Description      Disable the Java I/F
**
** Returns          void
**
*******************************************************************************/
void BTA_JvDisable(tBTA_JV_RFCOMM_CBACK *p_cback)
{
    tBTA_JV_API_DISABLE *p_buf;

    APPL_TRACE_API( "BTA_JvDisable");
    bta_sys_deregister(BTA_ID_JV);
    memset(&bta_jv_cb, 0, sizeof(tBTA_JV_CB));
    /* set handle to invalid value by default */
    for (int i = 0; i < BTA_JV_PM_MAX_NUM; i++) {
        bta_jv_cb.pm_cb[i].handle = BTA_JV_PM_HANDLE_CLEAR;
    }
    if ((p_buf = (tBTA_JV_API_DISABLE *) osi_malloc(sizeof(tBTA_JV_API_DISABLE))) != NULL) {
        p_buf->hdr.event = BTA_JV_API_DISABLE_EVT;
        p_buf->p_cback = p_cback;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_JvFree
**
** Description      Free JV configuration
**
** Returns          void
**
*******************************************************************************/
void BTA_JvFree(void)
{
#if BTA_DYNAMIC_MEMORY == TRUE
    /* Free buffer for JV configuration structure */
    osi_free(p_bta_jv_cfg->p_sdp_raw_data);
    osi_free(p_bta_jv_cfg->p_sdp_db);
    p_bta_jv_cfg->p_sdp_raw_data = NULL;
    p_bta_jv_cfg->p_sdp_db = NULL;
#endif
}

/*******************************************************************************
**
** Function         BTA_JvIsEnable
**
** Description      Get the JV registration status.
**
** Returns          TRUE, if registered
**
*******************************************************************************/
BOOLEAN BTA_JvIsEnable(void)
{
    return bta_sys_is_register(BTA_ID_JV);
}

/*******************************************************************************
**
** Function         BTA_JvIsEncrypted
**
** Description      This function checks if the link to peer device is encrypted
**
** Returns          TRUE if encrypted.
**                  FALSE if not.
**
*******************************************************************************/
BOOLEAN BTA_JvIsEncrypted(BD_ADDR bd_addr)
{
    BOOLEAN is_encrypted = FALSE;
    UINT8 sec_flags, le_flags;

    if (BTM_GetSecurityFlags(bd_addr, &sec_flags) &&
            BTM_GetSecurityFlagsByTransport(bd_addr, &le_flags, BT_TRANSPORT_LE)) {
        if (sec_flags & BTM_SEC_FLAG_ENCRYPTED ||
                le_flags & BTM_SEC_FLAG_ENCRYPTED) {
            is_encrypted = TRUE;
        }
    }
    return is_encrypted;
}
/*******************************************************************************
**
** Function         BTA_JvGetChannelId
**
** Description      This function reserves a SCN (server channel number) for
**                  applications running over RFCOMM, L2CAP of L2CAP_LE.
**                  It is primarily called by server profiles/applications to
**                  register their SCN into the SDP database. The SCN is reported
**                  by the tBTA_JV_DM_CBACK callback with a BTA_JV_GET_SCN_EVT
**                  for RFCOMM channels and BTA_JV_GET_PSM_EVT for L2CAP and LE.
**                  If the SCN/PSM reported is 0, that means all resources are
**                  exhausted.
** Parameters
**   conn_type      one of BTA_JV_CONN_TYPE_
**   user_data      Any uservalue - will be returned in the resulting event.
**   channel        Only used for RFCOMM - to try to allocate a specific RFCOMM
**                  channel.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvGetChannelId(int conn_type, void *user_data, INT32 channel)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_ALLOC_CHANNEL *p_msg;

    APPL_TRACE_API( "%s", __func__);
    if ((p_msg = (tBTA_JV_API_ALLOC_CHANNEL *)osi_malloc(sizeof(tBTA_JV_API_ALLOC_CHANNEL))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_GET_CHANNEL_EVT;
        p_msg->type      = conn_type;
        p_msg->channel   = channel;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvFreeChannel
**
** Description      This function frees a server channel number that was used
**                  by an application running over RFCOMM.
** Parameters
**   channel        The channel to free
**   conn_type      one of BTA_JV_CONN_TYPE_
**   p_cback        tBTA_JV_RFCOMM_CBACK is called with when server
**   user_data      indicate the RFCOMM server status
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvFreeChannel(UINT16 channel, int conn_type, tBTA_JV_RFCOMM_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_FREE_CHANNEL *p_msg;

    APPL_TRACE_API( "%s", __func__);
    if ((p_msg = (tBTA_JV_API_FREE_CHANNEL *)osi_malloc(sizeof(tBTA_JV_API_FREE_CHANNEL))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_FREE_SCN_EVT;
        p_msg->scn       = channel;
        p_msg->type      = conn_type;
        p_msg->p_cback   = p_cback;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvStartDiscovery
**
** Description      This function performs service discovery for the services
**                  provided by the given peer device. When the operation is
**                  complete the tBTA_JV_DM_CBACK callback function will be
**                  called with a BTA_JV_DISCOVERY_COMP_EVT.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvStartDiscovery(BD_ADDR bd_addr, UINT16 num_uuid,
                                    tSDP_UUID *p_uuid_list, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_START_DISCOVERY *p_msg;

    APPL_TRACE_API( "BTA_JvStartDiscovery");
    if ((num_uuid > BTA_JV_MAX_UUIDS) || ((num_uuid > 0) && (p_uuid_list == NULL))) {
        APPL_TRACE_ERROR("invalid uuid list: num_uuid=%u", num_uuid);
        return BTA_JV_FAILURE;
    }

    if ((p_msg = (tBTA_JV_API_START_DISCOVERY *)osi_malloc(sizeof(tBTA_JV_API_START_DISCOVERY))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_START_DISCOVERY_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->num_uuid = num_uuid;
        memcpy(p_msg->uuid_list, p_uuid_list, num_uuid * sizeof(tSDP_UUID));
        p_msg->num_attr = 0;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvCreateRecord
**
** Description      Create a service record in the local SDP database.
**                  When the operation is complete the tBTA_JV_DM_CBACK callback
**                  function will be called with a BTA_JV_CREATE_RECORD_EVT.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvCreateRecordByUser(const char *name, UINT32 channel, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_CREATE_RECORD *p_msg;

    APPL_TRACE_API( "BTA_JvCreateRecordByUser");
    if ((p_msg = (tBTA_JV_API_CREATE_RECORD *)osi_malloc(sizeof(tBTA_JV_API_CREATE_RECORD))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_CREATE_RECORD_EVT;
        p_msg->user_data = user_data;
        strcpy(p_msg->name, name);
        p_msg->channel = channel;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvDeleteRecord
**
** Description      Delete a service record in the local SDP database.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvDeleteRecord(UINT32 handle)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_ADD_ATTRIBUTE *p_msg;

    APPL_TRACE_API( "BTA_JvDeleteRecord");
    if ((p_msg = (tBTA_JV_API_ADD_ATTRIBUTE *)osi_malloc(sizeof(tBTA_JV_API_ADD_ATTRIBUTE))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_DELETE_RECORD_EVT;
        p_msg->handle = handle;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }
    return (status);
}

#if BTA_JV_L2CAP_INCLUDED
/*******************************************************************************
**
** Function         BTA_JvL2capConnectLE
**
** Description      Initiate an LE connection as a L2CAP client to the given BD
**                  Address.
**                  When the connection is initiated or failed to initiate,
**                  tBTA_JV_L2CAP_CBACK is called with BTA_JV_L2CAP_CL_INIT_EVT
**                  When the connection is established or failed,
**                  tBTA_JV_L2CAP_CBACK is called with BTA_JV_L2CAP_OPEN_EVT
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capConnectLE(tBTA_SEC sec_mask, tBTA_JV_ROLE role,
                                    const tL2CAP_ERTM_INFO *ertm_info, UINT16 remote_chan,
                                    UINT16 rx_mtu, tL2CAP_CFG_INFO *cfg,
                                    BD_ADDR peer_bd_addr, tBTA_JV_L2CAP_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_CONNECT *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if (p_cback &&
            (p_msg =
                 (tBTA_JV_API_L2CAP_CONNECT *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_CONNECT))) != NULL) {
        p_msg->hdr.event    = BTA_JV_API_L2CAP_CONNECT_LE_EVT;
        p_msg->sec_mask     = sec_mask;
        p_msg->role         = role;
        p_msg->remote_chan  = remote_chan;
        p_msg->rx_mtu       = rx_mtu;
        if (cfg != NULL) {
            p_msg->has_cfg = TRUE;
            p_msg->cfg = *cfg;
        } else {
            p_msg->has_cfg = FALSE;
        }
        if (ertm_info != NULL) {
            p_msg->has_ertm_info = TRUE;
            p_msg->ertm_info = *ertm_info;
        } else {
            p_msg->has_ertm_info = FALSE;
        }
        memcpy(p_msg->peer_bd_addr, peer_bd_addr, sizeof(BD_ADDR));
        p_msg->p_cback      = p_cback;
        p_msg->user_data    = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capConnect
**
** Description      Initiate a connection as a L2CAP client to the given BD
**                  Address.
**                  When the connection is initiated or failed to initiate,
**                  tBTA_JV_L2CAP_CBACK is called with BTA_JV_L2CAP_CL_INIT_EVT
**                  When the connection is established or failed,
**                  tBTA_JV_L2CAP_CBACK is called with BTA_JV_L2CAP_OPEN_EVT
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capConnect(tBTA_SEC sec_mask, tBTA_JV_ROLE role,
                                  const tL2CAP_ERTM_INFO *ertm_info, UINT16 remote_psm,
                                  UINT16 rx_mtu, tL2CAP_CFG_INFO *cfg,
                                  BD_ADDR peer_bd_addr, tBTA_JV_L2CAP_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_CONNECT *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if (p_cback &&
            (p_msg = (tBTA_JV_API_L2CAP_CONNECT *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_CONNECT))) != NULL) {
        p_msg->hdr.event    = BTA_JV_API_L2CAP_CONNECT_EVT;
        p_msg->sec_mask     = sec_mask;
        p_msg->role         = role;
        p_msg->remote_psm   = remote_psm;
        p_msg->rx_mtu       = rx_mtu;
        if (cfg != NULL) {
            p_msg->has_cfg = TRUE;
            p_msg->cfg = *cfg;
        } else {
            p_msg->has_cfg = FALSE;
        }
        if (ertm_info != NULL) {
            p_msg->has_ertm_info = TRUE;
            p_msg->ertm_info = *ertm_info;
        } else {
            p_msg->has_ertm_info = FALSE;
        }
        memcpy(p_msg->peer_bd_addr, peer_bd_addr, sizeof(BD_ADDR));
        p_msg->p_cback      = p_cback;
        p_msg->user_data    = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capClose
**
** Description      This function closes an L2CAP client connection
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capClose(UINT32 handle, tBTA_JV_L2CAP_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_CLOSE *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if (handle < BTA_JV_MAX_L2C_CONN && bta_jv_cb.l2c_cb[handle].p_cback &&
            (p_msg = (tBTA_JV_API_L2CAP_CLOSE *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_CLOSE))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_CLOSE_EVT;
        p_msg->handle = handle;
        p_msg->p_cb = &bta_jv_cb.l2c_cb[handle];
        p_msg->p_cback = p_cback;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capCloseLE
**
** Description      This function closes an L2CAP client connection for Fixed Channels
**                  Function is idempotent and no callbacks are called!
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capCloseLE(UINT32 handle)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_CLOSE *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if ((p_msg = (tBTA_JV_API_L2CAP_CLOSE *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_CLOSE))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_CLOSE_FIXED_EVT;
        p_msg->handle = handle;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capStartServer
**
** Description      This function starts an L2CAP server and listens for an L2CAP
**                  connection from a remote Bluetooth device.  When the server
**                  is started successfully, tBTA_JV_L2CAP_CBACK is called with
**                  BTA_JV_L2CAP_START_EVT.  When the connection is established,
**                  tBTA_JV_L2CAP_CBACK is called with BTA_JV_L2CAP_OPEN_EVT.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capStartServer(tBTA_SEC sec_mask, tBTA_JV_ROLE role,
                                      const tL2CAP_ERTM_INFO *ertm_info, UINT16 local_psm, UINT16 rx_mtu, tL2CAP_CFG_INFO *cfg,
                                      tBTA_JV_L2CAP_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_SERVER *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if (p_cback &&
            (p_msg = (tBTA_JV_API_L2CAP_SERVER *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_SERVER))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_START_SERVER_EVT;
        p_msg->sec_mask = sec_mask;
        p_msg->role = role;
        p_msg->local_psm = local_psm;
        p_msg->rx_mtu = rx_mtu;
        if (cfg != NULL) {
            p_msg->has_cfg = TRUE;
            p_msg->cfg = *cfg;
        } else {
            p_msg->has_cfg = FALSE;
        }
        if (ertm_info != NULL) {
            p_msg->has_ertm_info = TRUE;
            p_msg->ertm_info = *ertm_info;
        } else {
            p_msg->has_ertm_info = FALSE;
        }
        p_msg->p_cback = p_cback;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capStartServerLE
**
** Description      This function starts an LE L2CAP server and listens for an L2CAP
**                  connection from a remote Bluetooth device.  When the server
**                  is started successfully, tBTA_JV_L2CAP_CBACK is called with
**                  BTA_JV_L2CAP_START_EVT.  When the connection is established,
**                  tBTA_JV_L2CAP_CBACK is called with BTA_JV_L2CAP_OPEN_EVT.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capStartServerLE(tBTA_SEC sec_mask, tBTA_JV_ROLE role,
                                        const tL2CAP_ERTM_INFO *ertm_info, UINT16 local_chan, UINT16 rx_mtu, tL2CAP_CFG_INFO *cfg,
                                        tBTA_JV_L2CAP_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_SERVER *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if (p_cback &&
            (p_msg = (tBTA_JV_API_L2CAP_SERVER *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_SERVER))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_START_SERVER_LE_EVT;
        p_msg->sec_mask = sec_mask;
        p_msg->role = role;
        p_msg->local_chan = local_chan;
        p_msg->rx_mtu = rx_mtu;
        if (cfg != NULL) {
            p_msg->has_cfg = TRUE;
            p_msg->cfg = *cfg;
        } else {
            p_msg->has_cfg = FALSE;
        }
        if (ertm_info != NULL) {
            p_msg->has_ertm_info = TRUE;
            p_msg->ertm_info = *ertm_info;
        } else {
            p_msg->has_ertm_info = FALSE;
        }
        p_msg->p_cback = p_cback;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capStopServer
**
** Description      This function stops the L2CAP server. If the server has an
**                  active connection, it would be closed.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capStopServer(UINT16 local_psm, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_SERVER *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if ((p_msg = (tBTA_JV_API_L2CAP_SERVER *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_SERVER))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_STOP_SERVER_EVT;
        p_msg->local_psm = local_psm;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capStopServerLE
**
** Description      This function stops the LE L2CAP server. If the server has an
**                  active connection, it would be closed.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capStopServerLE(UINT16 local_chan, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_SERVER *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if ((p_msg = (tBTA_JV_API_L2CAP_SERVER *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_SERVER))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_STOP_SERVER_LE_EVT;
        p_msg->local_chan = local_chan;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capRead
**
** Description      This function reads data from an L2CAP connecti;
    tBTA_JV_RFC_CB  *p_cb = rc->p_cb;
on
**                  When the operation is complete, tBTA_JV_L2CAP_CBACK is
**                  called with BTA_JV_L2CAP_READ_EVT.
**
** Returns          Length of read data.
**
*******************************************************************************/
int BTA_JvL2capRead(UINT32 handle, UINT32 req_id, UINT8 *p_data, UINT16 len)
{
    tBTA_JV_L2CAP_READ evt_data = {0};

    APPL_TRACE_API( "%s", __func__);

    if (handle < BTA_JV_MAX_L2C_CONN && bta_jv_cb.l2c_cb[handle].p_cback) {
        evt_data.status = BTA_JV_FAILURE;
        evt_data.handle = handle;
        evt_data.req_id = req_id;
        evt_data.p_data = p_data;
        evt_data.len    = 0;

        if (BT_PASS == GAP_ConnReadData((UINT16)handle, p_data, len, &evt_data.len)) {
            evt_data.status = BTA_JV_SUCCESS;
        }
        bta_jv_cb.l2c_cb[handle].p_cback(
            BTA_JV_L2CAP_READ_EVT, (tBTA_JV *)&evt_data, bta_jv_cb.l2c_cb[handle].user_data);
    }

    return (evt_data.len);
}

/*******************************************************************************
**
** Function         BTA_JvL2capReceive
**
** Description      This function reads data from an L2CAP connection
**                  When the operation is complete, tBTA_JV_L2CAP_CBACK is
**                  called with BTA_JV_L2CAP_RECEIVE_EVT.
**                  If there are more data queued in L2CAP than len, the extra data will be discarded.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capReceive(UINT32 handle, UINT32 req_id, UINT8 *p_data, UINT16 len)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_L2CAP_RECEIVE evt_data;
    UINT32  left_over = 0;
    UINT16  max_len, read_len;

    APPL_TRACE_API( "%s", __func__);


    if (handle < BTA_JV_MAX_L2C_CONN && bta_jv_cb.l2c_cb[handle].p_cback) {
        status = BTA_JV_SUCCESS;
        evt_data.status = BTA_JV_FAILURE;
        evt_data.handle = handle;
        evt_data.req_id = req_id;
        evt_data.p_data = p_data;
        evt_data.len    = 0;

        if (BT_PASS == GAP_ConnReadData((UINT16)handle, p_data, len, &evt_data.len)) {
            evt_data.status = BTA_JV_SUCCESS;
            GAP_GetRxQueueCnt ((UINT16)handle, &left_over);
            while (left_over) {
                max_len = (left_over > 0xFFFF) ? 0xFFFF : left_over;
                GAP_ConnReadData ((UINT16)handle, NULL, max_len, &read_len);
                left_over -= read_len;
            }
        }
        bta_jv_cb.l2c_cb[handle].p_cback(
            BTA_JV_L2CAP_RECEIVE_EVT, (tBTA_JV *)&evt_data, bta_jv_cb.l2c_cb[handle].user_data);
    }

    return (status);
}
/*******************************************************************************
**
** Function         BTA_JvL2capReady
**
** Description      This function determined if there is data to read from
**                    an L2CAP connection
**
** Returns          BTA_JV_SUCCESS, if data queue size is in *p_data_size.
**                  BTA_JV_FAILURE, if error.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capReady(UINT32 handle, UINT32 *p_data_size)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;

    APPL_TRACE_API( "%s: %d", __func__, handle);
    if (p_data_size && handle < BTA_JV_MAX_L2C_CONN && bta_jv_cb.l2c_cb[handle].p_cback) {
        *p_data_size = 0;
        if (BT_PASS == GAP_GetRxQueueCnt((UINT16)handle, p_data_size) ) {
            status = BTA_JV_SUCCESS;
        }
    }

    return (status);
}


/*******************************************************************************
**
** Function         BTA_JvL2capWrite
**
** Description      This function writes data to an L2CAP connection
**                  When the operation is complete, tBTA_JV_L2CAP_CBACK is
**                  called with BTA_JV_L2CAP_WRITE_EVT. Works for
**                  PSM-based connections
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capWrite(UINT32 handle, UINT32 req_id, UINT8 *p_data,
                                UINT16 len, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_WRITE *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if (handle < BTA_JV_MAX_L2C_CONN && bta_jv_cb.l2c_cb[handle].p_cback &&
            (p_msg = (tBTA_JV_API_L2CAP_WRITE *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_WRITE))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_WRITE_EVT;
        p_msg->handle = handle;
        p_msg->req_id = req_id;
        p_msg->p_data = p_data;
        p_msg->p_cb = &bta_jv_cb.l2c_cb[handle];
        p_msg->len = len;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvL2capWriteFixed
**
** Description      This function writes data to an L2CAP connection
**                  When the operation is complete, tBTA_JV_L2CAP_CBACK is
**                  called with BTA_JV_L2CAP_WRITE_EVT. Works for
**                  fixed-channel connections
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvL2capWriteFixed(UINT16 channel, BD_ADDR *addr, UINT32 req_id,
                                     tBTA_JV_L2CAP_CBACK *p_cback, UINT8 *p_data, UINT16 len, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_L2CAP_WRITE_FIXED *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if ((p_msg =
                (tBTA_JV_API_L2CAP_WRITE_FIXED *)osi_malloc(sizeof(tBTA_JV_API_L2CAP_WRITE_FIXED))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_L2CAP_WRITE_FIXED_EVT;
        p_msg->channel = channel;
        memcpy(p_msg->addr, addr, sizeof(p_msg->addr));
        p_msg->req_id = req_id;
        p_msg->p_data = p_data;
        p_msg->p_cback = p_cback;
        p_msg->len = len;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}
#endif /* BTA_JV_L2CAP_INCLUDED */

#if BTA_JV_RFCOMM_INCLUDED
/*******************************************************************************
**
** Function         BTA_JvRfcommConfig
**
** Description      This function is to configure RFCOMM.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommConfig(BOOLEAN enable_l2cap_ertm)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_CONFIG *p_msg;

    APPL_TRACE_API( "%s", __func__);

    if ((p_msg = (tBTA_JV_API_RFCOMM_CONFIG *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_CONFIG))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_RFCOMM_CONFIG_EVT;
        p_msg->enable_l2cap_ertm = enable_l2cap_ertm;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommConnect
**
** Description      This function makes an RFCOMM connection to a remote BD
**                  Address.
**                  When the connection is initiated or failed to initiate,
**                  tBTA_JV_RFCOMM_CBACK is called with BTA_JV_RFCOMM_CL_INIT_EVT
**                  When the connection is established or failed,
**                  tBTA_JV_RFCOMM_CBACK is called with BTA_JV_RFCOMM_OPEN_EVT
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommConnect(tBTA_SEC sec_mask,
                                   tBTA_JV_ROLE role, UINT8 remote_scn, BD_ADDR peer_bd_addr,
                                   tBTA_JV_RFCOMM_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_CONNECT *p_msg;

    APPL_TRACE_API( "BTA_JvRfcommConnect");
    if (p_cback &&
            (p_msg = (tBTA_JV_API_RFCOMM_CONNECT *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_CONNECT))) != NULL) {
        p_msg->hdr.event    = BTA_JV_API_RFCOMM_CONNECT_EVT;
        p_msg->sec_mask     = sec_mask;
        p_msg->role         = role;
        p_msg->remote_scn   = remote_scn;
        memcpy(p_msg->peer_bd_addr, peer_bd_addr, sizeof(BD_ADDR));
        p_msg->p_cback      = p_cback;
        p_msg->user_data    = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommClose
**
** Description      This function closes an RFCOMM connection
**                  When the connection is established or failed,
**                  tBTA_JV_RFCOMM_CBACK is called with BTA_JV_RFCOMM_CLOSE_EVT
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommClose(UINT32 handle, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_CLOSE *p_msg;
    UINT32  hi = ((handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
    UINT32  si = BTA_JV_RFC_HDL_TO_SIDX(handle);

    APPL_TRACE_API( "%s", __func__);
    if (hi < BTA_JV_MAX_RFC_CONN && bta_jv_cb.rfc_cb[hi].p_cback &&
            si < BTA_JV_MAX_RFC_SR_SESSION && bta_jv_cb.rfc_cb[hi].rfc_hdl[si] &&
            (p_msg = (tBTA_JV_API_RFCOMM_CLOSE *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_CLOSE))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_RFCOMM_CLOSE_EVT;
        p_msg->handle = handle;
        p_msg->p_cb = &bta_jv_cb.rfc_cb[hi];
        p_msg->p_pcb = &bta_jv_cb.port_cb[p_msg->p_cb->rfc_hdl[si] - 1];
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommStartServer
**
** Description      This function starts listening for an RFCOMM connection
**                  request from a remote Bluetooth device.  When the server is
**                  started successfully, tBTA_JV_RFCOMM_CBACK is called
**                  with BTA_JV_RFCOMM_START_EVT.
**                  When the connection is established, tBTA_JV_RFCOMM_CBACK
**                  is called with BTA_JV_RFCOMM_OPEN_EVT.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommStartServer(tBTA_SEC sec_mask,
                                       tBTA_JV_ROLE role, UINT8 local_scn, UINT8 max_session,
                                       tBTA_JV_RFCOMM_CBACK *p_cback, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_SERVER *p_msg;

    APPL_TRACE_API( "BTA_JvRfcommStartServer");
    if (p_cback &&
            (p_msg = (tBTA_JV_API_RFCOMM_SERVER *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_SERVER))) != NULL) {
        if (max_session == 0) {
            max_session = 1;
        }
        if (max_session > BTA_JV_MAX_RFC_SR_SESSION) {
            APPL_TRACE_DEBUG( "max_session (%d) is too big. use max (%d)", max_session, BTA_JV_MAX_RFC_SR_SESSION);
            max_session = BTA_JV_MAX_RFC_SR_SESSION;
        }
        p_msg->hdr.event = BTA_JV_API_RFCOMM_START_SERVER_EVT;
        p_msg->sec_mask = sec_mask;
        p_msg->role = role;
        p_msg->local_scn = local_scn;
        p_msg->max_session = max_session;
        p_msg->p_cback = p_cback;
        p_msg->user_data = user_data; //caller's private data
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommStopServer
**
** Description      This function stops the RFCOMM server. If the server has an
**                  active connection, it would be closed.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommStopServer(UINT32 handle, void *user_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_SERVER *p_msg;
    APPL_TRACE_API( "BTA_JvRfcommStopServer");
    if ((p_msg = (tBTA_JV_API_RFCOMM_SERVER *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_SERVER))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_RFCOMM_STOP_SERVER_EVT;
        p_msg->handle = handle;
        p_msg->user_data = user_data; //caller's private data
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommRead
**
** Description      This function reads data from an RFCOMM connection
**                  The actual size of data read is returned in p_len.
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommRead(UINT32 handle, UINT32 req_id, UINT8 *p_data, UINT16 len)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_READ *p_msg;
    UINT32  hi = ((handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
    UINT32  si = BTA_JV_RFC_HDL_TO_SIDX(handle);

    APPL_TRACE_API( "BTA_JvRfcommRead");
    if (hi < BTA_JV_MAX_RFC_CONN && bta_jv_cb.rfc_cb[hi].p_cback &&
            si < BTA_JV_MAX_RFC_SR_SESSION && bta_jv_cb.rfc_cb[hi].rfc_hdl[si] &&
            (p_msg = (tBTA_JV_API_RFCOMM_READ *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_READ))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_RFCOMM_READ_EVT;
        p_msg->handle = handle;
        p_msg->req_id = req_id;
        p_msg->p_data = p_data;
        p_msg->len = len;
        p_msg->p_cb = &bta_jv_cb.rfc_cb[hi];
        p_msg->p_pcb = &bta_jv_cb.port_cb[p_msg->p_cb->rfc_hdl[si] - 1];
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommGetPortHdl
**
** Description    This function fetches the rfcomm port handle
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
UINT16 BTA_JvRfcommGetPortHdl(UINT32 handle)
{
    UINT32  hi = ((handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
    UINT32  si = BTA_JV_RFC_HDL_TO_SIDX(handle);

    if (hi < BTA_JV_MAX_RFC_CONN &&
            si < BTA_JV_MAX_RFC_SR_SESSION && bta_jv_cb.rfc_cb[hi].rfc_hdl[si]) {
        return bta_jv_cb.port_cb[bta_jv_cb.rfc_cb[hi].rfc_hdl[si] - 1].port_handle;
    } else {
        return 0xffff;
    }
}


/*******************************************************************************
**
** Function         BTA_JvRfcommReady
**
** Description      This function determined if there is data to read from
**                  an RFCOMM connection
**
** Returns          BTA_JV_SUCCESS, if data queue size is in *p_data_size.
**                  BTA_JV_FAILURE, if error.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommReady(UINT32 handle, UINT32 *p_data_size)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    UINT16          size = 0;
    UINT32  hi = ((handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
    UINT32  si = BTA_JV_RFC_HDL_TO_SIDX(handle);

    APPL_TRACE_API( "BTA_JvRfcommReady");
    if (hi < BTA_JV_MAX_RFC_CONN && bta_jv_cb.rfc_cb[hi].p_cback &&
            si < BTA_JV_MAX_RFC_SR_SESSION && bta_jv_cb.rfc_cb[hi].rfc_hdl[si]) {
        if (PORT_GetRxQueueCnt(bta_jv_cb.rfc_cb[hi].rfc_hdl[si], &size) == PORT_SUCCESS) {
            status = BTA_JV_SUCCESS;
        }
    }
    *p_data_size = size;
    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommWrite
**
** Description      This function writes data to an RFCOMM connection
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/

tBTA_JV_STATUS BTA_JvRfcommWrite(UINT32 handle, UINT32 req_id, int len, UINT8 *p_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_WRITE *p_msg;
    UINT32  hi = ((handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
    UINT32  si = BTA_JV_RFC_HDL_TO_SIDX(handle);

    APPL_TRACE_API( "BTA_JvRfcommWrite");
    APPL_TRACE_DEBUG( "handle:0x%x, hi:%d, si:%d", handle, hi, si);
    if (hi < BTA_JV_MAX_RFC_CONN && bta_jv_cb.rfc_cb[hi].p_cback &&
            si < BTA_JV_MAX_RFC_SR_SESSION && bta_jv_cb.rfc_cb[hi].rfc_hdl[si] &&
            (p_msg = (tBTA_JV_API_RFCOMM_WRITE *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_WRITE))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_RFCOMM_WRITE_EVT;
        p_msg->handle = handle;
        p_msg->req_id = req_id;
        p_msg->p_cb = &bta_jv_cb.rfc_cb[hi];
        p_msg->p_pcb = &bta_jv_cb.port_cb[p_msg->p_cb->rfc_hdl[si] - 1];
        p_msg->p_data = p_data;
        p_msg->len = len;
        APPL_TRACE_API( "write ok");
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }
    return (status);
}

/*******************************************************************************
**
** Function         BTA_JvRfcommFlowControl
**
** Description      This function gives credits to the peer
**
** Returns          BTA_JV_SUCCESS, if the request is being processed.
**                  BTA_JV_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_JV_STATUS BTA_JvRfcommFlowControl(UINT32 handle, UINT16 credits_given)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_RFCOMM_FLOW_CONTROL *p_msg;
    UINT32  hi = ((handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
    UINT32  si = BTA_JV_RFC_HDL_TO_SIDX(handle);

    APPL_TRACE_API( "BTA_JvRfcommFlowControl");
    APPL_TRACE_DEBUG( "handle:0x%x, hi:%d, si:%d", handle, hi, si);
    if (hi < BTA_JV_MAX_RFC_CONN && bta_jv_cb.rfc_cb[hi].p_cback &&
            si < BTA_JV_MAX_RFC_SR_SESSION && bta_jv_cb.rfc_cb[hi].rfc_hdl[si] &&
            (p_msg = (tBTA_JV_API_RFCOMM_FLOW_CONTROL *)osi_malloc(sizeof(tBTA_JV_API_RFCOMM_FLOW_CONTROL))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_RFCOMM_FLOW_CONTROL_EVT;
        p_msg->p_cb = &bta_jv_cb.rfc_cb[hi];
        p_msg->p_pcb = &bta_jv_cb.port_cb[p_msg->p_cb->rfc_hdl[si] - 1];
        p_msg->credits_given = credits_given;
        APPL_TRACE_API( "credits given %d", credits_given);
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }
    return (status);
}
#endif /* BTA_JV_RFCOMM_INCLUDED */

/*******************************************************************************
 **
 ** Function    BTA_JVSetPmProfile
 **
 ** Description This function set or free power mode profile for different JV application
 **
 ** Parameters:  handle,  JV handle from RFCOMM or L2CAP
 **              app_id:  app specific pm ID, can be BTA_JV_PM_ALL, see bta_dm_cfg.c for details
 **              BTA_JV_PM_ID_CLEAR: removes pm management on the handle. init_st is ignored and
 **              BTA_JV_CONN_CLOSE is called implicitly
 **              init_st:  state after calling this API. typically it should be BTA_JV_CONN_OPEN
 **
 ** Returns      BTA_JV_SUCCESS, if the request is being processed.
 **              BTA_JV_FAILURE, otherwise.
 **
 ** NOTE:        BTA_JV_PM_ID_CLEAR: In general no need to be called as jv pm calls automatically
 **              BTA_JV_CONN_CLOSE to remove in case of connection close!
 **
 *******************************************************************************/
tBTA_JV_STATUS BTA_JvSetPmProfile(UINT32 handle, tBTA_JV_PM_ID app_id, tBTA_JV_CONN_STATE init_st)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_SET_PM_PROFILE *p_msg;

    APPL_TRACE_API("BTA_JVSetPmProfile handle:0x%x, app_id:%d", handle, app_id);
    if ((p_msg = (tBTA_JV_API_SET_PM_PROFILE *)osi_malloc(sizeof(tBTA_JV_API_SET_PM_PROFILE)))
            != NULL) {
        p_msg->hdr.event = BTA_JV_API_SET_PM_PROFILE_EVT;
        p_msg->handle = handle;
        p_msg->app_id = app_id;
        p_msg->init_st = init_st;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }

    return (status);
}

#endif  ///defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE
