/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
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


#include "bt_target.h"
#include "bt_defs.h"
#include "btu.h"
#include "gap_int.h"
#include "l2cdefs.h"
#include "l2c_int.h"
#include <string.h>
#if GAP_CONN_INCLUDED == TRUE
#include "btm_int.h"

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void gap_connect_ind (BD_ADDR  bd_addr, UINT16 l2cap_cid, UINT16 psm, UINT8 l2cap_id);
static void gap_connect_cfm (UINT16 l2cap_cid, UINT16 result);
static void gap_config_ind (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void gap_config_cfm (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg);
static void gap_disconnect_ind (UINT16 l2cap_cid, BOOLEAN ack_needed);
static void gap_data_ind (UINT16 l2cap_cid, BT_HDR *p_msg);
static void gap_congestion_ind (UINT16 lcid, BOOLEAN is_congested);

static tGAP_CCB *gap_find_ccb_by_cid (UINT16 cid);
static tGAP_CCB *gap_find_ccb_by_handle (UINT16 handle);
static tGAP_CCB *gap_allocate_ccb (void);
static void      gap_release_ccb (tGAP_CCB *p_ccb);

/*******************************************************************************
**
** Function         gap_conn_init
**
** Description      This function is called to initialize GAP connection management
**
** Returns          void
**
*******************************************************************************/
void gap_conn_init (void)
{
#if ((defined AMP_INCLUDED) && (AMP_INCLUDED == TRUE))
    gap_cb.conn.reg_info.pAMP_ConnectInd_Cb         = gap_connect_ind;
    gap_cb.conn.reg_info.pAMP_ConnectCfm_Cb         = gap_connect_cfm;
    gap_cb.conn.reg_info.pAMP_ConnectPnd_Cb         = NULL;
    gap_cb.conn.reg_info.pAMP_ConfigInd_Cb          = gap_config_ind;
    gap_cb.conn.reg_info.pAMP_ConfigCfm_Cb          = gap_config_cfm;
    gap_cb.conn.reg_info.pAMP_DisconnectInd_Cb      = gap_disconnect_ind;
    gap_cb.conn.reg_info.pAMP_DisconnectCfm_Cb      = NULL;
    gap_cb.conn.reg_info.pAMP_QoSViolationInd_Cb    = NULL;
    gap_cb.conn.reg_info.pAMP_DataInd_Cb            = gap_data_ind;
    gap_cb.conn.reg_info.pAMP_CongestionStatus_Cb   = gap_congestion_ind;
    gap_cb.conn.reg_info.pAMP_TxComplete_Cb         = NULL;
    gap_cb.conn.reg_info.pAMP_MoveInd_Cb            = NULL;
    gap_cb.conn.reg_info.pAMP_MoveRsp_Cb            = NULL;
    gap_cb.conn.reg_info.pAMP_MoveCfm_Cb            = NULL; //gap_move_cfm
    gap_cb.conn.reg_info.pAMP_MoveCfmRsp_Cb         = NULL; //gap_move_cfm_rsp

#else
    gap_cb.conn.reg_info.pL2CA_ConnectInd_Cb       = gap_connect_ind;
    gap_cb.conn.reg_info.pL2CA_ConnectCfm_Cb       = gap_connect_cfm;
    gap_cb.conn.reg_info.pL2CA_ConnectPnd_Cb       = NULL;
    gap_cb.conn.reg_info.pL2CA_ConfigInd_Cb        = gap_config_ind;
    gap_cb.conn.reg_info.pL2CA_ConfigCfm_Cb        = gap_config_cfm;
    gap_cb.conn.reg_info.pL2CA_DisconnectInd_Cb    = gap_disconnect_ind;
    gap_cb.conn.reg_info.pL2CA_DisconnectCfm_Cb    = NULL;
    gap_cb.conn.reg_info.pL2CA_QoSViolationInd_Cb  = NULL;
    gap_cb.conn.reg_info.pL2CA_DataInd_Cb          = gap_data_ind;
    gap_cb.conn.reg_info.pL2CA_CongestionStatus_Cb = gap_congestion_ind;
    gap_cb.conn.reg_info.pL2CA_TxComplete_Cb       = NULL;
#endif
}


/*******************************************************************************
**
** Function         GAP_ConnOpen
**
** Description      This function is called to open an L2CAP connection.
**
** Parameters:      is_server   - If TRUE, the connection is not created
**                                but put into a "listen" mode waiting for
**                                the remote side to connect.
**
**                  service_id  - Unique service ID from
**                                BTM_SEC_SERVICE_FIRST_EMPTY (6)
**                                to BTM_SEC_MAX_SERVICE_RECORDS (32)
**
**                  p_rem_bda   - Pointer to remote BD Address.
**                                If a server, and we don't care about the
**                                remote BD Address, then NULL should be passed.
**
**                  psm         - the PSM used for the connection
**
**                  p_config    - Optional pointer to configuration structure.
**                                If NULL, the default GAP configuration will
**                                be used.
**
**                  security    - security flags
**                  chan_mode_mask - (GAP_FCR_CHAN_OPT_BASIC, GAP_FCR_CHAN_OPT_ERTM,
**                                    GAP_FCR_CHAN_OPT_STREAM)
**
**                  p_cb        - Pointer to callback function for events.
**
** Returns          handle of the connection if successful, else GAP_INVALID_HANDLE
**
*******************************************************************************/
UINT16 GAP_ConnOpen (char *p_serv_name, UINT8 service_id, BOOLEAN is_server,
                     BD_ADDR p_rem_bda, UINT16 psm, tL2CAP_CFG_INFO *p_cfg,
                     tL2CAP_ERTM_INFO *ertm_info, UINT16 security, UINT8 chan_mode_mask,
                     tGAP_CONN_CALLBACK *p_cb)
{
    tGAP_CCB    *p_ccb;
    UINT16       cid;
    //tBT_UUID    bt_uuid = {2, {GAP_PROTOCOL_ID}};

    GAP_TRACE_EVENT ("GAP_CONN - Open Request");

    /* Allocate a new CCB. Return if none available. */
    if ((p_ccb = gap_allocate_ccb()) == NULL) {
        return (GAP_INVALID_HANDLE);
    }

    /* If caller specified a BD address, save it */
    if (p_rem_bda) {
        /* the bd addr is not BT_BD_ANY, then a bd address was specified */
        if (memcmp (p_rem_bda, BT_BD_ANY, BD_ADDR_LEN)) {
            p_ccb->rem_addr_specified = TRUE;
        }

        memcpy (&p_ccb->rem_dev_address[0], p_rem_bda, BD_ADDR_LEN);
    } else if (!is_server) {
        /* remore addr is not specified and is not a server -> bad */
        return (GAP_INVALID_HANDLE);
    }

    /* A client MUST have specified a bd addr to connect with */
    if (!p_ccb->rem_addr_specified && !is_server) {
        gap_release_ccb (p_ccb);
        GAP_TRACE_ERROR ("GAP ERROR: Client must specify a remote BD ADDR to connect to!");
        return (GAP_INVALID_HANDLE);
    }

    /* Check if configuration was specified */
    if (p_cfg) {
        p_ccb->cfg = *p_cfg;
    }

    p_ccb->p_callback     = p_cb;

    /* If originator, use a dynamic PSM */
#if ((defined AMP_INCLUDED) && (AMP_INCLUDED == TRUE))
    if (!is_server) {
        gap_cb.conn.reg_info.pAMP_ConnectInd_Cb  = NULL;
    } else {
        gap_cb.conn.reg_info.pAMP_ConnectInd_Cb  = gap_connect_ind;
    }
#else
    if (!is_server) {
        gap_cb.conn.reg_info.pL2CA_ConnectInd_Cb = NULL;
    } else {
        gap_cb.conn.reg_info.pL2CA_ConnectInd_Cb = gap_connect_ind;
    }
#endif

    /* Register the PSM with L2CAP */
    if ((p_ccb->psm = L2CA_REGISTER (psm, &gap_cb.conn.reg_info,
                                     AMP_AUTOSWITCH_ALLOWED | AMP_USE_AMP_IF_POSSIBLE)) == 0) {
        GAP_TRACE_ERROR ("GAP_ConnOpen: Failure registering PSM 0x%04x", psm);
        gap_release_ccb (p_ccb);
        return (GAP_INVALID_HANDLE);
    }

    /* Register with Security Manager for the specific security level */
    p_ccb->service_id = service_id;
    if (!BTM_SetSecurityLevel ((UINT8)!is_server, p_serv_name,
                               p_ccb->service_id, security, p_ccb->psm, 0, 0)) {
        GAP_TRACE_ERROR ("GAP_CONN - Security Error");
        gap_release_ccb (p_ccb);
        return (GAP_INVALID_HANDLE);
    }

    /* Fill in eL2CAP parameter data */
    if ( p_ccb->cfg.fcr_present ) {
        if (ertm_info == NULL) {
            p_ccb->ertm_info.preferred_mode = p_ccb->cfg.fcr.mode;
            p_ccb->ertm_info.user_rx_pool_id = GAP_DATA_POOL_ID;
            p_ccb->ertm_info.user_tx_pool_id = GAP_DATA_POOL_ID;
            p_ccb->ertm_info.fcr_rx_pool_id = L2CAP_DEFAULT_ERM_POOL_ID;
            p_ccb->ertm_info.fcr_tx_pool_id = L2CAP_DEFAULT_ERM_POOL_ID;
        } else {
            p_ccb->ertm_info = *ertm_info;
        }
    }

    /* optional FCR channel modes */
    if (ertm_info != NULL) {
        p_ccb->ertm_info.allowed_modes =
            (chan_mode_mask) ? chan_mode_mask : (UINT8)L2CAP_FCR_CHAN_OPT_BASIC;
    }

    if (is_server) {
        p_ccb->con_flags |= GAP_CCB_FLAGS_SEC_DONE; /* assume btm/l2cap would handle it */
        p_ccb->con_state = GAP_CCB_STATE_LISTENING;
        return (p_ccb->gap_handle);
    } else {
        /* We are the originator of this connection */
        p_ccb->con_flags = GAP_CCB_FLAGS_IS_ORIG;

        /* Transition to the next appropriate state, waiting for connection confirm. */
        p_ccb->con_state = GAP_CCB_STATE_CONN_SETUP;

        /* mark security done flag, when security is not required */
        if ((security & (BTM_SEC_OUT_AUTHORIZE | BTM_SEC_OUT_AUTHENTICATE | BTM_SEC_OUT_ENCRYPT) ) == 0) {
            p_ccb->con_flags |= GAP_CCB_FLAGS_SEC_DONE;
        }

        /* Check if L2CAP started the connection process */
        if (p_rem_bda && ((cid = L2CA_CONNECT_REQ (p_ccb->psm, p_rem_bda, &p_ccb->ertm_info, &bt_uuid)) != 0)) {
            p_ccb->connection_id = cid;
            return (p_ccb->gap_handle);
        } else {
            gap_release_ccb (p_ccb);
            return (GAP_INVALID_HANDLE);
        }
    }
}


/*******************************************************************************
**
** Function         GAP_ConnClose
**
** Description      This function is called to close a connection.
**
** Parameters:      handle      - Handle of the connection returned by GAP_ConnOpen
**
** Returns          BT_PASS             - closed OK
**                  GAP_ERR_BAD_HANDLE  - invalid handle
**
*******************************************************************************/
UINT16 GAP_ConnClose (UINT16 gap_handle)
{
    tGAP_CCB    *p_ccb = gap_find_ccb_by_handle (gap_handle);

    GAP_TRACE_EVENT ("GAP_CONN - close  handle: 0x%x", gap_handle);

    if (p_ccb) {
        /* Check if we have a connection ID */
        if (p_ccb->con_state != GAP_CCB_STATE_LISTENING) {
            L2CA_DISCONNECT_REQ (p_ccb->connection_id);
        }

        gap_release_ccb (p_ccb);

        return (BT_PASS);
    }

    return (GAP_ERR_BAD_HANDLE);
}



/*******************************************************************************
**
** Function         GAP_ConnReadData
**
** Description      Normally not GKI aware application will call this function
**                  after receiving GAP_EVT_RXDATA event.
**
** Parameters:      handle      - Handle of the connection returned in the Open
**                  p_data      - Data area
**                  max_len     - Byte count requested
**                  p_len       - Byte count received
**
** Returns          BT_PASS             - data read
**                  GAP_ERR_BAD_HANDLE  - invalid handle
**                  GAP_NO_DATA_AVAIL   - no data available
**
*******************************************************************************/
UINT16 GAP_ConnReadData (UINT16 gap_handle, UINT8 *p_data, UINT16 max_len, UINT16 *p_len)
{
    tGAP_CCB    *p_ccb = gap_find_ccb_by_handle (gap_handle);
    BT_HDR     *p_buf;
    UINT16      copy_len;

    if (!p_ccb) {
        return (GAP_ERR_BAD_HANDLE);
    }

    *p_len = 0;

    p_buf = (BT_HDR *)GKI_getfirst (&p_ccb->rx_queue);
    if (!p_buf) {
        return (GAP_NO_DATA_AVAIL);
    }

    GKI_disable();

    while (max_len && p_buf) {
        copy_len = (p_buf->len > max_len) ? max_len : p_buf->len;
        max_len -= copy_len;
        *p_len  += copy_len;
        if (p_data) {
            memcpy (p_data, (UINT8 *)(p_buf + 1) + p_buf->offset, copy_len);
            p_data += copy_len;
        }

        if (p_buf->len > copy_len) {
            p_buf->offset += copy_len;
            p_buf->len    -= copy_len;
            break;
        } else {
            if (max_len) {
                p_buf = (BT_HDR *)GKI_getnext (p_buf);
            }
            GKI_freebuf (GKI_dequeue (&p_ccb->rx_queue));
        }
    }

    p_ccb->rx_queue_size -= *p_len;

    GKI_enable();

    GAP_TRACE_EVENT ("GAP_ConnReadData - rx_queue_size left=%d, *p_len=%d",
                     p_ccb->rx_queue_size, *p_len);

    return (BT_PASS);
}

/*******************************************************************************
**
** Function         GAP_GetRxQueueCnt
**
** Description      This function return number of bytes on the rx queue.
**
** Parameters:      handle     - Handle returned in the GAP_ConnOpen
**                  p_rx_queue_count - Pointer to return queue count in.
**
**
*******************************************************************************/
int GAP_GetRxQueueCnt (UINT16 handle, UINT32 *p_rx_queue_count)
{
    tGAP_CCB    *p_ccb;
    int         rc = BT_PASS;

    /* Check that handle is valid */
    if (handle < GAP_MAX_CONNECTIONS) {
        p_ccb = &gap_cb.conn.ccb_pool[handle];

        if (p_ccb->con_state == GAP_CCB_STATE_CONNECTED) {
            *p_rx_queue_count = p_ccb->rx_queue_size;
        } else {
            rc = GAP_INVALID_HANDLE;
        }
    } else {
        rc = GAP_INVALID_HANDLE;
    }

    GAP_TRACE_EVENT ("GAP_GetRxQueueCnt - rc = 0x%04x, rx_queue_count=%d",
                     rc , *p_rx_queue_count);

    return (rc);
}

/*******************************************************************************
**
** Function         GAP_ConnBTRead
**
** Description      Bluetooth aware applications will call this function after receiving
**                  GAP_EVT_RXDATA event.
**
** Parameters:      handle      - Handle of the connection returned in the Open
**                  pp_buf      - pointer to address of buffer with data,
**
** Returns          BT_PASS             - data read
**                  GAP_ERR_BAD_HANDLE  - invalid handle
**                  GAP_NO_DATA_AVAIL   - no data available
**
*******************************************************************************/
UINT16 GAP_ConnBTRead (UINT16 gap_handle, BT_HDR **pp_buf)
{
    tGAP_CCB    *p_ccb = gap_find_ccb_by_handle (gap_handle);
    BT_HDR      *p_buf;

    if (!p_ccb) {
        return (GAP_ERR_BAD_HANDLE);
    }

    p_buf = (BT_HDR *)GKI_dequeue (&p_ccb->rx_queue);

    if (p_buf) {
        *pp_buf = p_buf;

        p_ccb->rx_queue_size -= p_buf->len;
        return (BT_PASS);
    } else {
        *pp_buf = NULL;
        return (GAP_NO_DATA_AVAIL);
    }
}


/*******************************************************************************
**
** Function         GAP_ConnBTWrite
**
** Description      Bluetooth Aware applications can call this function to write data.
**
** Parameters:      handle      - Handle of the connection returned in the Open
**                  p_buf      - pointer to address of buffer with data,
**
** Returns          BT_PASS                 - data read
**                  GAP_ERR_BAD_HANDLE      - invalid handle
**                  GAP_ERR_BAD_STATE       - connection not established
**                  GAP_INVALID_BUF_OFFSET  - buffer offset is invalid
*******************************************************************************/
UINT16 GAP_ConnBTWrite (UINT16 gap_handle, BT_HDR *p_buf)
{
    tGAP_CCB    *p_ccb = gap_find_ccb_by_handle (gap_handle);

    if (!p_ccb) {
        GKI_freebuf (p_buf);
        return (GAP_ERR_BAD_HANDLE);
    }

    if (p_ccb->con_state != GAP_CCB_STATE_CONNECTED) {
        GKI_freebuf (p_buf);
        return (GAP_ERR_BAD_STATE);
    }

    if (p_buf->offset < L2CAP_MIN_OFFSET) {
        GKI_freebuf (p_buf);
        return (GAP_ERR_BUF_OFFSET);
    }

    GKI_enqueue (&p_ccb->tx_queue, p_buf);

    if (p_ccb->is_congested) {
        return (BT_PASS);
    }

    /* Send the buffer through L2CAP */
#if (GAP_CONN_POST_EVT_INCLUDED == TRUE)
    gap_send_event (gap_handle);
#else
    while ((p_buf = (BT_HDR *)GKI_dequeue (&p_ccb->tx_queue)) != NULL) {
        UINT8 status = L2CA_DATA_WRITE (p_ccb->connection_id, p_buf);

        if (status == L2CAP_DW_CONGESTED) {
            p_ccb->is_congested = TRUE;
            break;
        } else if (status != L2CAP_DW_SUCCESS) {
            return (GAP_ERR_BAD_STATE);
        }
    }
#endif
    return (BT_PASS);
}


/*******************************************************************************
**
** Function         GAP_ConnWriteData
**
** Description      Normally not GKI aware application will call this function
**                  to send data to the connection.
**
** Parameters:      handle      - Handle of the connection returned in the Open
**                  p_data      - Data area
**                  max_len     - Byte count requested
**                  p_len       - Byte count received
**
** Returns          BT_PASS                 - data read
**                  GAP_ERR_BAD_HANDLE      - invalid handle
**                  GAP_ERR_BAD_STATE       - connection not established
**                  GAP_CONGESTION          - system is congested
**
*******************************************************************************/
UINT16 GAP_ConnWriteData (UINT16 gap_handle, UINT8 *p_data, UINT16 max_len, UINT16 *p_len)
{
    tGAP_CCB    *p_ccb = gap_find_ccb_by_handle (gap_handle);
    BT_HDR     *p_buf;

    *p_len = 0;

    if (!p_ccb) {
        return (GAP_ERR_BAD_HANDLE);
    }

    if (p_ccb->con_state != GAP_CCB_STATE_CONNECTED) {
        return (GAP_ERR_BAD_STATE);
    }

    while (max_len) {
        if (p_ccb->cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
            if ((p_buf = (BT_HDR *)GKI_getpoolbuf (p_ccb->ertm_info.user_tx_pool_id)) == NULL) {
                return (GAP_ERR_CONGESTED);
            }
        } else {
            if ((p_buf = (BT_HDR *)GKI_getpoolbuf (GAP_DATA_POOL_ID)) == NULL) {
                return (GAP_ERR_CONGESTED);
            }
        }

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = (p_ccb->rem_mtu_size < max_len) ? p_ccb->rem_mtu_size : max_len;
        p_buf->event = BT_EVT_TO_BTU_SP_DATA;

        memcpy ((UINT8 *)(p_buf + 1) + p_buf->offset, p_data, p_buf->len);

        *p_len  += p_buf->len;
        max_len -= p_buf->len;
        p_data  += p_buf->len;

        GAP_TRACE_EVENT ("GAP_WriteData %d bytes", p_buf->len);

        GKI_enqueue (&p_ccb->tx_queue, p_buf);
    }

    if (p_ccb->is_congested) {
        return (BT_PASS);
    }

    /* Send the buffer through L2CAP */
#if (GAP_CONN_POST_EVT_INCLUDED == TRUE)
    gap_send_event (gap_handle);
#else
    while ((p_buf = (BT_HDR *)GKI_dequeue (&p_ccb->tx_queue)) != NULL) {
        UINT8 status = L2CA_DATA_WRITE (p_ccb->connection_id, p_buf);

        if (status == L2CAP_DW_CONGESTED) {
            p_ccb->is_congested = TRUE;
            break;
        } else if (status != L2CAP_DW_SUCCESS) {
            return (GAP_ERR_BAD_STATE);
        }
    }
#endif
    return (BT_PASS);
}


/*******************************************************************************
**
** Function         GAP_ConnReconfig
**
** Description      Applications can call this function to reconfigure the connection.
**
** Parameters:      handle      - Handle of the connection
**                  p_cfg       - Pointer to new configuration
**
** Returns          BT_PASS                 - config process started
**                  GAP_ERR_BAD_HANDLE      - invalid handle
**
*******************************************************************************/
UINT16 GAP_ConnReconfig (UINT16 gap_handle, tL2CAP_CFG_INFO *p_cfg)
{
    tGAP_CCB    *p_ccb = gap_find_ccb_by_handle (gap_handle);

    if (!p_ccb) {
        return (GAP_ERR_BAD_HANDLE);
    }

    p_ccb->cfg = *p_cfg;

    if (p_ccb->con_state == GAP_CCB_STATE_CONNECTED) {
        L2CA_CONFIG_REQ (p_ccb->connection_id, p_cfg);
    }

    return (BT_PASS);
}



/*******************************************************************************
**
** Function         GAP_ConnSetIdleTimeout
**
** Description      Higher layers call this function to set the idle timeout for
**                  a connection, or for all future connections. The "idle timeout"
**                  is the amount of time that a connection can remain up with
**                  no L2CAP channels on it. A timeout of zero means that the
**                  connection will be torn down immediately when the last channel
**                  is removed. A timeout of 0xFFFF means no timeout. Values are
**                  in seconds.
**
** Parameters:      handle      - Handle of the connection
**                  timeout     - in secs
**                                0 = immediate disconnect when last channel is removed
**                                0xFFFF = no idle timeout
**
** Returns          BT_PASS                 - config process started
**                  GAP_ERR_BAD_HANDLE      - invalid handle
**
*******************************************************************************/
UINT16 GAP_ConnSetIdleTimeout (UINT16 gap_handle, UINT16 timeout)
{
    tGAP_CCB    *p_ccb;

    if ((p_ccb = gap_find_ccb_by_handle (gap_handle)) == NULL) {
        return (GAP_ERR_BAD_HANDLE);
    }

    if (L2CA_SetIdleTimeout (p_ccb->connection_id, timeout, FALSE)) {
        return (BT_PASS);
    } else {
        return (GAP_ERR_BAD_HANDLE);
    }
}



/*******************************************************************************
**
** Function         GAP_ConnGetRemoteAddr
**
** Description      This function is called to get the remote BD address
**                  of a connection.
**
** Parameters:      handle      - Handle of the connection returned by GAP_ConnOpen
**
** Returns          BT_PASS             - closed OK
**                  GAP_ERR_BAD_HANDLE  - invalid handle
**
*******************************************************************************/
UINT8 *GAP_ConnGetRemoteAddr (UINT16 gap_handle)
{
    tGAP_CCB    *p_ccb = gap_find_ccb_by_handle (gap_handle);

    GAP_TRACE_EVENT ("GAP_ConnGetRemoteAddr gap_handle = %d", gap_handle);

    if ((p_ccb) && (p_ccb->con_state > GAP_CCB_STATE_LISTENING)) {
        GAP_TRACE_EVENT("GAP_ConnGetRemoteAddr bda :0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n", \
                        p_ccb->rem_dev_address[0], p_ccb->rem_dev_address[1], p_ccb->rem_dev_address[2],
                        p_ccb->rem_dev_address[3], p_ccb->rem_dev_address[4], p_ccb->rem_dev_address[5]);
        return (p_ccb->rem_dev_address);
    } else {
        GAP_TRACE_EVENT ("GAP_ConnGetRemoteAddr return Error ");
        return (NULL);
    }
}


/*******************************************************************************
**
** Function         GAP_ConnGetRemMtuSize
**
** Description      Returns the remote device's MTU size
**
** Parameters:      handle      - Handle of the connection
**
** Returns          UINT16      - maximum size buffer that can be transmitted to the peer
**
*******************************************************************************/
UINT16 GAP_ConnGetRemMtuSize (UINT16 gap_handle)
{
    tGAP_CCB    *p_ccb;

    if ((p_ccb = gap_find_ccb_by_handle (gap_handle)) == NULL) {
        return (0);
    }

    return (p_ccb->rem_mtu_size);
}

/*******************************************************************************
**
** Function         GAP_ConnGetL2CAPCid
**
** Description      Returns the L2CAP channel id
**
** Parameters:      handle      - Handle of the connection
**
** Returns          UINT16      - The L2CAP channel id
**                  0, if error
**
*******************************************************************************/
UINT16 GAP_ConnGetL2CAPCid (UINT16 gap_handle)
{
    tGAP_CCB    *p_ccb;

    if ((p_ccb = gap_find_ccb_by_handle (gap_handle)) == NULL) {
        return (0);
    }

    return (p_ccb->connection_id);
}


/*******************************************************************************
**
** Function         gap_connect_ind
**
** Description      This function handles an inbound connection indication
**                  from L2CAP. This is the case where we are acting as a
**                  server.
**
** Returns          void
**
*******************************************************************************/
static void gap_connect_ind (BD_ADDR  bd_addr, UINT16 l2cap_cid, UINT16 psm, UINT8 l2cap_id)
{
    UINT16       xx;
    tGAP_CCB     *p_ccb;
    //tBT_UUID    bt_uuid = {2, {GAP_PROTOCOL_ID}};

    /* See if we have a CCB listening for the connection */
    for (xx = 0, p_ccb = gap_cb.conn.ccb_pool; xx < GAP_MAX_CONNECTIONS; xx++, p_ccb++) {
        if ((p_ccb->con_state == GAP_CCB_STATE_LISTENING)
                && (p_ccb->psm == psm)
                && ((p_ccb->rem_addr_specified == FALSE)
                    || (!memcmp (bd_addr, p_ccb->rem_dev_address, BD_ADDR_LEN)))) {
            break;
        }
    }

    if (xx == GAP_MAX_CONNECTIONS) {
        GAP_TRACE_WARNING("*******");
        GAP_TRACE_WARNING("WARNING: GAP Conn Indication for Unexpected Bd Addr...Disconnecting");
        GAP_TRACE_WARNING("*******");

        /* Disconnect because it is an unexpected connection */
        L2CA_DISCONNECT_REQ (l2cap_cid);
        return;
    }

    /* Transition to the next appropriate state, waiting for config setup. */
    p_ccb->con_state = GAP_CCB_STATE_CFG_SETUP;

    /* Save the BD Address and Channel ID. */
    memcpy (&p_ccb->rem_dev_address[0], bd_addr, BD_ADDR_LEN);
    p_ccb->connection_id = l2cap_cid;

    /* Send response to the L2CAP layer. */
    L2CA_CONNECT_RSP (bd_addr, l2cap_id, l2cap_cid, L2CAP_CONN_OK, L2CAP_CONN_OK, &p_ccb->ertm_info, &bt_uuid);

    GAP_TRACE_EVENT("GAP_CONN - Rcvd L2CAP conn ind, CID: 0x%x", p_ccb->connection_id);

    /* Send a Configuration Request. */
    L2CA_CONFIG_REQ (l2cap_cid, &p_ccb->cfg);
}

/*******************************************************************************
**
** Function         gap_checks_con_flags
**
** Description      This function processes the L2CAP configuration indication
**                  event.
**
** Returns          void
**
*******************************************************************************/
static void gap_checks_con_flags (tGAP_CCB    *p_ccb)
{
    GAP_TRACE_EVENT ("gap_checks_con_flags conn_flags:0x%x, ", p_ccb->con_flags);
    /* if all the required con_flags are set, report the OPEN event now */
    if ((p_ccb->con_flags & GAP_CCB_FLAGS_CONN_DONE) == GAP_CCB_FLAGS_CONN_DONE) {
        p_ccb->con_state = GAP_CCB_STATE_CONNECTED;

        p_ccb->p_callback (p_ccb->gap_handle, GAP_EVT_CONN_OPENED);
    }
}

/*******************************************************************************
**
** Function         gap_sec_check_complete
**
** Description      The function called when Security Manager finishes
**                  verification of the service side connection
**
** Returns          void
**
*******************************************************************************/
static void gap_sec_check_complete (BD_ADDR bd_addr, tBT_TRANSPORT transport, void *p_ref_data, UINT8 res)
{
    tGAP_CCB *p_ccb = (tGAP_CCB *)p_ref_data;
    UNUSED(bd_addr);
    UNUSED (transport);

    GAP_TRACE_EVENT ("gap_sec_check_complete conn_state:%d, conn_flags:0x%x, status:%d",
                     p_ccb->con_state, p_ccb->con_flags, res);
    if (p_ccb->con_state == GAP_CCB_STATE_IDLE) {
        return;
    }

    if (res == BTM_SUCCESS) {
        p_ccb->con_flags |= GAP_CCB_FLAGS_SEC_DONE;
        gap_checks_con_flags (p_ccb);
    } else {
        /* security failed - disconnect the channel */
        L2CA_DISCONNECT_REQ (p_ccb->connection_id);
    }
}

/*******************************************************************************
**
** Function         gap_connect_cfm
**
** Description      This function handles the connect confirm events
**                  from L2CAP. This is the case when we are acting as a
**                  client and have sent a connect request.
**
** Returns          void
**
*******************************************************************************/
static void gap_connect_cfm (UINT16 l2cap_cid, UINT16 result)
{
    tGAP_CCB    *p_ccb;

    /* Find CCB based on CID */
    if ((p_ccb = gap_find_ccb_by_cid (l2cap_cid)) == NULL) {
        return;
    }

    /* initiate security process, if needed */
    if ( (p_ccb->con_flags & GAP_CCB_FLAGS_SEC_DONE) == 0) {
        btm_sec_mx_access_request (p_ccb->rem_dev_address, p_ccb->psm, TRUE,
                                   0, 0, &gap_sec_check_complete, p_ccb);
    }

    /* If the connection response contains success status, then */
    /* Transition to the next state and startup the timer.      */
    if ((result == L2CAP_CONN_OK) && (p_ccb->con_state == GAP_CCB_STATE_CONN_SETUP)) {
        p_ccb->con_state = GAP_CCB_STATE_CFG_SETUP;

        /* Send a Configuration Request. */
        L2CA_CONFIG_REQ (l2cap_cid, &p_ccb->cfg);
    } else {
        /* Tell the user if he has a callback */
        if (p_ccb->p_callback) {
            (*p_ccb->p_callback) (p_ccb->gap_handle, GAP_EVT_CONN_CLOSED);
        }

        gap_release_ccb (p_ccb);
    }
}

/*******************************************************************************
**
** Function         gap_config_ind
**
** Description      This function processes the L2CAP configuration indication
**                  event.
**
** Returns          void
**
*******************************************************************************/
static void gap_config_ind (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    tGAP_CCB    *p_ccb;
    UINT16      local_mtu_size;

    /* Find CCB based on CID */
    if ((p_ccb = gap_find_ccb_by_cid (l2cap_cid)) == NULL) {
        return;
    }

    /* Remember the remote MTU size */

    if (p_ccb->cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
        local_mtu_size = GKI_get_pool_bufsize (p_ccb->ertm_info.user_tx_pool_id)
                         - sizeof(BT_HDR) - L2CAP_MIN_OFFSET;
    } else {
        local_mtu_size = L2CAP_MTU_SIZE;
    }

    if ((!p_cfg->mtu_present) || (p_cfg->mtu > local_mtu_size)) {
        p_ccb->rem_mtu_size = local_mtu_size;
    } else {
        p_ccb->rem_mtu_size = p_cfg->mtu;
    }

    /* For now, always accept configuration from the other side */
    p_cfg->flush_to_present = FALSE;
    p_cfg->mtu_present      = FALSE;
    p_cfg->result           = L2CAP_CFG_OK;
    p_cfg->fcs_present      = FALSE;

    L2CA_CONFIG_RSP (l2cap_cid, p_cfg);

    p_ccb->con_flags |= GAP_CCB_FLAGS_HIS_CFG_DONE;

    gap_checks_con_flags (p_ccb);
}


/*******************************************************************************
**
** Function         gap_config_cfm
**
** Description      This function processes the L2CAP configuration confirmation
**                  event.
**
** Returns          void
**
*******************************************************************************/
static void gap_config_cfm (UINT16 l2cap_cid, tL2CAP_CFG_INFO *p_cfg)
{
    tGAP_CCB    *p_ccb;

    /* Find CCB based on CID */
    if ((p_ccb = gap_find_ccb_by_cid (l2cap_cid)) == NULL) {
        return;
    }

    if (p_cfg->result == L2CAP_CFG_OK) {
        p_ccb->con_flags |= GAP_CCB_FLAGS_MY_CFG_DONE;


        if (p_ccb->cfg.fcr_present) {
            p_ccb->cfg.fcr.mode = p_cfg->fcr.mode;
        } else {
            p_ccb->cfg.fcr.mode = L2CAP_FCR_BASIC_MODE;
        }

        gap_checks_con_flags (p_ccb);
    } else {
        p_ccb->p_callback (p_ccb->gap_handle, GAP_EVT_CONN_CLOSED);
        gap_release_ccb (p_ccb);
    }
}


/*******************************************************************************
**
** Function         gap_disconnect_ind
**
** Description      This function handles a disconnect event from L2CAP. If
**                  requested to, we ack the disconnect before dropping the CCB
**
** Returns          void
**
*******************************************************************************/
static void gap_disconnect_ind (UINT16 l2cap_cid, BOOLEAN ack_needed)
{
    tGAP_CCB    *p_ccb;

    GAP_TRACE_EVENT ("GAP_CONN - Rcvd L2CAP disc, CID: 0x%x", l2cap_cid);

    /* Find CCB based on CID */
    if ((p_ccb = gap_find_ccb_by_cid (l2cap_cid)) == NULL) {
        return;
    }

    if (ack_needed) {
        L2CA_DISCONNECT_RSP (l2cap_cid);
    }

    p_ccb->p_callback (p_ccb->gap_handle, GAP_EVT_CONN_CLOSED);
    gap_release_ccb (p_ccb);
}


/*******************************************************************************
**
** Function         gap_data_ind
**
** Description      This function is called when data is received from L2CAP.
**
** Returns          void
**
*******************************************************************************/
static void gap_data_ind (UINT16 l2cap_cid, BT_HDR *p_msg)
{
    tGAP_CCB    *p_ccb;

    /* Find CCB based on CID */
    if ((p_ccb = gap_find_ccb_by_cid (l2cap_cid)) == NULL) {
        GKI_freebuf (p_msg);
        return;
    }

    if (p_ccb->con_state == GAP_CCB_STATE_CONNECTED) {
        GKI_enqueue (&p_ccb->rx_queue, p_msg);

        p_ccb->rx_queue_size += p_msg->len;
        /*
        GAP_TRACE_EVENT ("gap_data_ind - rx_queue_size=%d, msg len=%d",
                                       p_ccb->rx_queue_size, p_msg->len);
         */

        p_ccb->p_callback (p_ccb->gap_handle, GAP_EVT_CONN_DATA_AVAIL);
    } else {
        GKI_freebuf (p_msg);
    }
}


/*******************************************************************************
**
** Function         gap_congestion_ind
**
** Description      This is a callback function called by L2CAP when
**                  data L2CAP congestion status changes
**
*******************************************************************************/
static void gap_congestion_ind (UINT16 lcid, BOOLEAN is_congested)
{
    tGAP_CCB    *p_ccb;
    UINT16       event;
    BT_HDR      *p_buf;
    UINT8        status;

    GAP_TRACE_EVENT ("GAP_CONN - Rcvd L2CAP Is Congested (%d), CID: 0x%x",
                     is_congested, lcid);

    /* Find CCB based on CID */
    if ((p_ccb = gap_find_ccb_by_cid (lcid)) == NULL) {
        return;
    }

    p_ccb->is_congested = is_congested;

    event = (is_congested) ? GAP_EVT_CONN_CONGESTED : GAP_EVT_CONN_UNCONGESTED;
    p_ccb->p_callback (p_ccb->gap_handle, event);

    if (!is_congested) {
        while ((p_buf = (BT_HDR *)GKI_dequeue (&p_ccb->tx_queue)) != NULL) {
            status = L2CA_DATA_WRITE (p_ccb->connection_id, p_buf);

            if (status == L2CAP_DW_CONGESTED) {
                p_ccb->is_congested = TRUE;
                break;
            } else if (status != L2CAP_DW_SUCCESS) {
                break;
            }
        }
    }
}


/*******************************************************************************
**
** Function         gap_find_ccb_by_cid
**
** Description      This function searches the CCB table for an entry with the
**                  passed CID.
**
** Returns          the CCB address, or NULL if not found.
**
*******************************************************************************/
static tGAP_CCB *gap_find_ccb_by_cid (UINT16 cid)
{
    UINT16       xx;
    tGAP_CCB     *p_ccb;

    /* Look through each connection control block */
    for (xx = 0, p_ccb = gap_cb.conn.ccb_pool; xx < GAP_MAX_CONNECTIONS; xx++, p_ccb++) {
        if ((p_ccb->con_state != GAP_CCB_STATE_IDLE) && (p_ccb->connection_id == cid)) {
            return (p_ccb);
        }
    }

    /* If here, not found */
    return (NULL);
}


/*******************************************************************************
**
** Function         gap_find_ccb_by_handle
**
** Description      This function searches the CCB table for an entry with the
**                  passed handle.
**
** Returns          the CCB address, or NULL if not found.
**
*******************************************************************************/
static tGAP_CCB *gap_find_ccb_by_handle (UINT16 handle)
{
    tGAP_CCB     *p_ccb;

    /* Check that handle is valid */
    if (handle < GAP_MAX_CONNECTIONS) {
        p_ccb = &gap_cb.conn.ccb_pool[handle];

        if (p_ccb->con_state != GAP_CCB_STATE_IDLE) {
            return (p_ccb);
        }
    }

    /* If here, handle points to invalid connection */
    return (NULL);
}


/*******************************************************************************
**
** Function         gap_allocate_ccb
**
** Description      This function allocates a new CCB.
**
** Returns          CCB address, or NULL if none available.
**
*******************************************************************************/
static tGAP_CCB *gap_allocate_ccb (void)
{
    UINT16       xx;
    tGAP_CCB     *p_ccb;

    /* Look through each connection control block for a free one */
    for (xx = 0, p_ccb = gap_cb.conn.ccb_pool; xx < GAP_MAX_CONNECTIONS; xx++, p_ccb++) {
        if (p_ccb->con_state == GAP_CCB_STATE_IDLE) {
            memset (p_ccb, 0, sizeof (tGAP_CCB));

            p_ccb->gap_handle   = xx;
            p_ccb->rem_mtu_size = L2CAP_MTU_SIZE;

            return (p_ccb);
        }
    }

    /* If here, no free CCB found */
    return (NULL);
}


/*******************************************************************************
**
** Function         gap_release_ccb
**
** Description      This function releases a CCB.
**
** Returns          void
**
*******************************************************************************/
static void gap_release_ccb (tGAP_CCB *p_ccb)
{
    UINT16       xx;
    UINT16      psm = p_ccb->psm;
    UINT8       service_id = p_ccb->service_id;

    /* Drop any buffers we may be holding */
    p_ccb->rx_queue_size = 0;

    while (p_ccb->rx_queue._p_first) {
        GKI_freebuf (GKI_dequeue (&p_ccb->rx_queue));
    }

    while (p_ccb->tx_queue._p_first) {
        GKI_freebuf (GKI_dequeue (&p_ccb->tx_queue));
    }

    p_ccb->con_state = GAP_CCB_STATE_IDLE;

    /* If no-one else is using the PSM, deregister from L2CAP */
    for (xx = 0, p_ccb = gap_cb.conn.ccb_pool; xx < GAP_MAX_CONNECTIONS; xx++, p_ccb++) {
        if ((p_ccb->con_state != GAP_CCB_STATE_IDLE) && (p_ccb->psm == psm)) {
            return;
        }
    }

    /* Free the security record for this PSM */
    BTM_SecClrService(service_id);
    L2CA_DEREGISTER (psm);
}

#if (GAP_CONN_POST_EVT_INCLUDED == TRUE)

/*******************************************************************************
**
** Function     gap_send_event
**
** Description  Send BT_EVT_TO_GAP_MSG event to BTU task
**
** Returns      None
**
*******************************************************************************/
void gap_send_event (UINT16 gap_handle)
{
    BT_HDR  *p_msg;

    if ((p_msg = (BT_HDR *)GKI_getbuf(BT_HDR_SIZE)) != NULL) {
        p_msg->event  = BT_EVT_TO_GAP_MSG;
        p_msg->len    = 0;
        p_msg->offset = 0;
        p_msg->layer_specific = gap_handle;

        GKI_send_msg(BTU_TASK, BTU_HCI_RCV_MBOX, p_msg);
    } else {
        GAP_TRACE_ERROR("Unable to allocate message buffer for event.");
    }
}

/*******************************************************************************
**
** Function     gap_proc_btu_event
**
** Description  Event handler for BT_EVT_TO_GAP_MSG event from BTU task
**
** Returns      None
**
*******************************************************************************/
void gap_proc_btu_event(BT_HDR *p_msg)
{
    tGAP_CCB   *p_ccb = gap_find_ccb_by_handle (p_msg->layer_specific);
    UINT8       status;
    BT_HDR     *p_buf;

    if (!p_ccb) {
        return;
    }

    if (p_ccb->con_state != GAP_CCB_STATE_CONNECTED) {
        return;
    }

    if (p_ccb->is_congested) {
        return;
    }

    /* Send the buffer through L2CAP */

    while ((p_buf = (BT_HDR *)GKI_dequeue (&p_ccb->tx_queue)) != NULL) {
        status = L2CA_DATA_WRITE (p_ccb->connection_id, p_buf);

        if (status == L2CAP_DW_CONGESTED) {
            p_ccb->is_congested = TRUE;
            break;
        } else if (status != L2CAP_DW_SUCCESS) {
            break;
        }
    }

}
#endif /* (GAP_CONN_POST_EVT_INCLUDED == TRUE) */
#endif  /* GAP_CONN_INCLUDED */
