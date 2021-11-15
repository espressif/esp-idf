/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2004-2012 Broadcom Corporation
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
 *  This file contains the audio gateway functions controlling the RFCOMM
 *  connections.
 *
 ******************************************************************************/

#include <string.h>
#include "common/bt_defs.h"
#include "bta/bta_api.h"
#include "bta_hf_client_int.h"
#include "stack/port_api.h"
#include "osi/allocator.h"

#if (BTA_HF_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_hf_client_port_cback
**
** Description      RFCOMM Port callback
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_port_cback(UINT32 code, UINT16 port_handle)
{
    BT_HDR      *p_buf;
    UNUSED(code);

    /* ignore port events for port handles other than connected handle */
    if (port_handle != bta_hf_client_cb.scb.conn_handle) {
        APPL_TRACE_DEBUG("bta_hf_client_port_cback ignoring handle:%d conn_handle = %d",
                         port_handle, bta_hf_client_cb.scb.conn_handle);
        return;
    }

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_HF_CLIENT_RFC_DATA_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_mgmt_cback
**
** Description      RFCOMM management callback
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_mgmt_cback(UINT32 code, UINT16 port_handle, void* data)
{
    tBTA_HF_CLIENT_RFC     *p_buf;
    UINT16                  event;

    APPL_TRACE_DEBUG("bta_hf_client_mgmt_cback : code = %d, port_handle = %d, conn_handle = %d, serv_handle = %d",
                     code, port_handle, bta_hf_client_cb.scb.conn_handle, bta_hf_client_cb.scb.serv_handle);

    /* ignore close event for port handles other than connected handle */
    if ((code != PORT_SUCCESS) && (port_handle != bta_hf_client_cb.scb.conn_handle)) {
        APPL_TRACE_DEBUG("bta_hf_client_mgmt_cback ignoring handle:%d", port_handle);
        return;
    }

    if (code == PORT_SUCCESS) {
        if ((bta_hf_client_cb.scb.conn_handle && (port_handle == bta_hf_client_cb.scb.conn_handle)) || /* outgoing connection */
                (port_handle == bta_hf_client_cb.scb.serv_handle)) {                     /* incoming connection */
            event = BTA_HF_CLIENT_RFC_OPEN_EVT;
        } else {
            APPL_TRACE_ERROR ("bta_hf_client_mgmt_cback: PORT_SUCCESS, ignoring handle = %d", port_handle);
            return;
        }
    }
    /* distinguish server close events */
    else if (port_handle == bta_hf_client_cb.scb.conn_handle) {
        event = BTA_HF_CLIENT_RFC_CLOSE_EVT;
    } else {
        event = BTA_HF_CLIENT_RFC_SRV_CLOSE_EVT;
    }

    if ((p_buf = (tBTA_HF_CLIENT_RFC *) osi_malloc(sizeof(tBTA_HF_CLIENT_RFC))) != NULL) {
        p_buf->hdr.event = event;
        p_buf->port_handle = port_handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_setup_port
**
** Description      Setup RFCOMM port for use by HF Client.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_setup_port(UINT16 handle)
{
    PORT_SetEventMask(handle, PORT_EV_RXCHAR);
    PORT_SetEventCallback(handle, bta_hf_client_port_cback);
}

/*******************************************************************************
**
** Function         bta_hf_client_start_server
**
** Description      Setup RFCOMM server for use by HF Client.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_start_server(void)
{
    int port_status;

    if (bta_hf_client_cb.scb.serv_handle > 0) {
        APPL_TRACE_DEBUG("%s already started, handle: %d", __FUNCTION__, bta_hf_client_cb.scb.serv_handle);
        return;
    }

    BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_HF_HANDSFREE, bta_hf_client_cb.scb.serv_sec_mask,
                         BT_PSM_RFCOMM, BTM_SEC_PROTO_RFCOMM, bta_hf_client_cb.scn);

    port_status =  RFCOMM_CreateConnection(UUID_SERVCLASS_HF_HANDSFREE, bta_hf_client_cb.scn,
                                           TRUE, BTA_HF_CLIENT_MTU, (UINT8 *) bd_addr_any, &(bta_hf_client_cb.scb.serv_handle),
                                           bta_hf_client_mgmt_cback);

    if (port_status  == PORT_SUCCESS) {
        bta_hf_client_setup_port(bta_hf_client_cb.scb.serv_handle);
    } else {
        /* TODO: can we handle this better? */
        APPL_TRACE_DEBUG("bta_hf_client_start_server: RFCOMM_CreateConnection returned error:%d", port_status);
    }

    APPL_TRACE_DEBUG("bta_hf_client_start_server handle: %d", bta_hf_client_cb.scb.serv_handle);
}

/*******************************************************************************
**
** Function         bta_hf_client_close_server
**
** Description      Close RFCOMM server port for use by HF Client.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_close_server(void)
{
    APPL_TRACE_DEBUG("%s %d", __FUNCTION__, bta_hf_client_cb.scb.serv_handle);

    if (bta_hf_client_cb.scb.serv_handle == 0) {
        APPL_TRACE_DEBUG("%s already stopped", __FUNCTION__);
        return;
    }

    RFCOMM_RemoveServer(bta_hf_client_cb.scb.serv_handle);
    bta_hf_client_cb.scb.serv_handle = 0;
}

/*******************************************************************************
**
** Function         bta_hf_client_rfc_do_open
**
** Description      Open an RFCOMM connection to the peer device.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_rfc_do_open(tBTA_HF_CLIENT_DATA *p_data)
{
    BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_HF_HANDSFREE,
                         bta_hf_client_cb.scb.cli_sec_mask, BT_PSM_RFCOMM,
                         BTM_SEC_PROTO_RFCOMM, bta_hf_client_cb.scb.peer_scn);

    if (RFCOMM_CreateConnection(UUID_SERVCLASS_HF_HANDSFREE, bta_hf_client_cb.scb.peer_scn,
                                FALSE, BTA_HF_CLIENT_MTU, bta_hf_client_cb.scb.peer_addr, &(bta_hf_client_cb.scb.conn_handle),
                                bta_hf_client_mgmt_cback) == PORT_SUCCESS) {
        bta_hf_client_setup_port(bta_hf_client_cb.scb.conn_handle);
        APPL_TRACE_DEBUG("bta_hf_client_rfc_do_open : conn_handle = %d", bta_hf_client_cb.scb.conn_handle);
    }
    /* RFCOMM create connection failed; send ourselves RFCOMM close event */
    else {
        bta_hf_client_sm_execute(BTA_HF_CLIENT_RFC_CLOSE_EVT, p_data);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_rfc_do_close
**
** Description      Close RFCOMM connection.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_rfc_do_close(tBTA_HF_CLIENT_DATA *p_data)
{
    tBTA_HF_CLIENT_RFC     *p_buf;
    UNUSED(p_data);

    if (bta_hf_client_cb.scb.conn_handle) {
        RFCOMM_RemoveConnection(bta_hf_client_cb.scb.conn_handle);
    } else {
        /* Close API was called while HF Client is in Opening state.        */
        /* Need to trigger the state machine to send callback to the app    */
        /* and move back to INIT state.                                     */
        if ((p_buf = (tBTA_HF_CLIENT_RFC *) osi_malloc(sizeof(tBTA_HF_CLIENT_RFC))) != NULL) {
            p_buf->hdr.event = BTA_HF_CLIENT_RFC_CLOSE_EVT;
            bta_sys_sendmsg(p_buf);
        }

        /* Cancel SDP if it had been started. */
        if (bta_hf_client_cb.scb.p_disc_db) {
            (void)SDP_CancelServiceSearch (bta_hf_client_cb.scb.p_disc_db);
            bta_hf_client_free_db(NULL);
        }
    }
}
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
