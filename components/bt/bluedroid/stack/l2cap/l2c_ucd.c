/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
 *  This file contains the L2CAP UCD code
 *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

#include "bt_types.h"
#include "hcidefs.h"
#include "hcimsgs.h"
#include "l2cdefs.h"
#include "l2c_int.h"
#include "btu.h"
#include "btm_api.h"
#include "btm_int.h"

#if (L2CAP_UCD_INCLUDED == TRUE)
static BOOLEAN l2c_ucd_connect ( BD_ADDR rem_bda );

/*******************************************************************************
**
** Function         l2c_ucd_discover_cback
**
** Description      UCD Discover callback
**
** Returns          void
**
*******************************************************************************/
static void l2c_ucd_discover_cback (BD_ADDR rem_bda, UINT8 info_type, UINT32 data)
{
    tL2C_RCB    *p_rcb = &l2cb.rcb_pool[0];
    UINT16      xx;

    L2CAP_TRACE_DEBUG ("L2CAP - l2c_ucd_discover_cback");

    for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++) {
        if (p_rcb->in_use) {
            /* if this application is waiting UCD reception info */
            if (( info_type == L2CAP_UCD_INFO_TYPE_RECEPTION )
                    && ( p_rcb->ucd.state & L2C_UCD_STATE_W4_RECEPTION )) {
                p_rcb->ucd.cb_info.pL2CA_UCD_Discover_Cb (rem_bda, info_type, data);
                p_rcb->ucd.state &= ~(L2C_UCD_STATE_W4_RECEPTION);
            }

            /* if this application is waiting UCD MTU info */
            if (( info_type == L2CAP_UCD_INFO_TYPE_MTU )
                    && ( p_rcb->ucd.state & L2C_UCD_STATE_W4_MTU )) {
                p_rcb->ucd.cb_info.pL2CA_UCD_Discover_Cb (rem_bda, info_type, data);
                p_rcb->ucd.state &= ~(L2C_UCD_STATE_W4_MTU);
            }
        }
    }
}

/*******************************************************************************
**
** Function         l2c_ucd_data_ind_cback
**
** Description      UCD Data callback
**
** Returns          void
**
*******************************************************************************/
static void l2c_ucd_data_ind_cback (BD_ADDR rem_bda, BT_HDR *p_buf)
{
    UINT8 *p;
    UINT16 psm;
    tL2C_RCB    *p_rcb;

    L2CAP_TRACE_DEBUG ("L2CAP - l2c_ucd_data_ind_cback");

    p = (UINT8 *)(p_buf + 1) + p_buf->offset;
    STREAM_TO_UINT16(psm, p)

    p_buf->offset += L2CAP_UCD_OVERHEAD;
    p_buf->len    -= L2CAP_UCD_OVERHEAD;

    if ((p_rcb = l2cu_find_rcb_by_psm (psm)) == NULL) {
        L2CAP_TRACE_ERROR ("L2CAP - no RCB for l2c_ucd_data_ind_cback, PSM: 0x%04x", psm);
        osi_free (p_buf);
    } else {
        p_rcb->ucd.cb_info.pL2CA_UCD_Data_Cb(rem_bda, p_buf);
    }
}

/*******************************************************************************
**
** Function         l2c_ucd_congestion_status_cback
**
** Description      UCD Congestion Status callback
**
** Returns          void
**
*******************************************************************************/
static void l2c_ucd_congestion_status_cback (BD_ADDR rem_bda, BOOLEAN is_congested)
{
    tL2C_RCB    *p_rcb = &l2cb.rcb_pool[0];
    UINT16      xx;

    L2CAP_TRACE_DEBUG ("L2CAP - l2c_ucd_congestion_status_cback");

    for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++) {
        if (( p_rcb->in_use )
                && ( p_rcb->ucd.state != L2C_UCD_STATE_UNUSED )) {
            if ( p_rcb->ucd.cb_info.pL2CA_UCD_Congestion_Status_Cb ) {
                L2CAP_TRACE_DEBUG ("L2CAP - Calling UCDCongestionStatus_Cb (%d), PSM=0x%04x, BDA: %08x%04x,",
                                   is_congested, p_rcb->psm,
                                   (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                                   (rem_bda[4] << 8) + rem_bda[5]);

                p_rcb->ucd.cb_info.pL2CA_UCD_Congestion_Status_Cb ( rem_bda, is_congested );
            }
        }
    }
}

/*******************************************************************************
**
** Function         l2c_ucd_disconnect_ind_cback
**
** Description      UCD disconnect callback (This prevent to access null pointer)
**
** Returns          void
**
*******************************************************************************/
static void l2c_ucd_disconnect_ind_cback (UINT16 cid, BOOLEAN result)
{
    /* do nothing */
}

/*******************************************************************************
**
** Function         l2c_ucd_config_ind_cback
**
** Description      UCD config callback (This prevent to access null pointer)
**
** Returns          void
**
*******************************************************************************/
static void l2c_ucd_config_ind_cback (UINT16 cid, tL2CAP_CFG_INFO *p_cfg)
{
    /* do nothing */
}

/*******************************************************************************
**
** Function         l2c_ucd_config_cfm_cback
**
** Description      UCD config callback (This prevent to access null pointer)
**
** Returns          void
**
*******************************************************************************/
static void l2c_ucd_config_cfm_cback (UINT16 cid, tL2CAP_CFG_INFO *p_cfg)
{
    /* do nothing */
}

/*******************************************************************************
**
**  Function        L2CA_UcdRegister
**
**  Description     Register PSM on UCD.
**
**  Parameters:     tL2CAP_UCD_CB_INFO
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
BOOLEAN L2CA_UcdRegister ( UINT16 psm, tL2CAP_UCD_CB_INFO *p_cb_info )
{
    tL2C_RCB             *p_rcb;

    L2CAP_TRACE_API  ("L2CA_UcdRegister()  PSM: 0x%04x", psm);

    if ((!p_cb_info->pL2CA_UCD_Discover_Cb)
            || (!p_cb_info->pL2CA_UCD_Data_Cb)) {
        L2CAP_TRACE_ERROR ("L2CAP - no callback registering PSM(0x%04x) on UCD", psm);
        return (FALSE);
    }

    if ((p_rcb = l2cu_find_rcb_by_psm (psm)) == NULL) {
        L2CAP_TRACE_ERROR ("L2CAP - no RCB for L2CA_UcdRegister, PSM: 0x%04x", psm);
        return (FALSE);
    }

    p_rcb->ucd.state   = L2C_UCD_STATE_W4_DATA;
    p_rcb->ucd.cb_info = *p_cb_info;

    /* check if master rcb is created for UCD */
    if ((p_rcb = l2cu_find_rcb_by_psm (L2C_UCD_RCB_ID)) == NULL) {
        if ((p_rcb = l2cu_allocate_rcb (L2C_UCD_RCB_ID)) == NULL) {
            L2CAP_TRACE_ERROR ("L2CAP - no RCB available for L2CA_UcdRegister");
            return (FALSE);
        } else {
            /* these callback functions will forward data to each UCD application */
            p_rcb->ucd.cb_info.pL2CA_UCD_Discover_Cb            = l2c_ucd_discover_cback;
            p_rcb->ucd.cb_info.pL2CA_UCD_Data_Cb                = l2c_ucd_data_ind_cback;
            p_rcb->ucd.cb_info.pL2CA_UCD_Congestion_Status_Cb   = l2c_ucd_congestion_status_cback;

            memset (&p_rcb->api, 0, sizeof(tL2CAP_APPL_INFO));
            p_rcb->api.pL2CA_DisconnectInd_Cb        = l2c_ucd_disconnect_ind_cback;

            /* This will make L2CAP check UCD congestion callback */
            p_rcb->api.pL2CA_CongestionStatus_Cb     = NULL;

            /* do nothing but prevent crash */
            p_rcb->api.pL2CA_ConfigInd_Cb            = l2c_ucd_config_ind_cback;
            p_rcb->api.pL2CA_ConfigCfm_Cb            = l2c_ucd_config_cfm_cback;
        }
    }

    return (TRUE);
}

/*******************************************************************************
**
**  Function        L2CA_UcdDeregister
**
**  Description     Deregister PSM on UCD.
**
**  Parameters:     PSM
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
BOOLEAN L2CA_UcdDeregister ( UINT16 psm )
{
    tL2C_CCB    *p_ccb;
    tL2C_RCB    *p_rcb;
    UINT16      xx;

    L2CAP_TRACE_API  ("L2CA_UcdDeregister()  PSM: 0x%04x", psm);

    if ((p_rcb = l2cu_find_rcb_by_psm (psm)) == NULL) {
        L2CAP_TRACE_ERROR ("L2CAP - no RCB for L2CA_UcdDeregister, PSM: 0x%04x", psm);
        return (FALSE);
    }

    p_rcb->ucd.state = L2C_UCD_STATE_UNUSED;

    /* check this was the last UCD registration */
    p_rcb = &l2cb.rcb_pool[0];

    for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++) {
        if ((p_rcb->in_use) && (p_rcb->ucd.state != L2C_UCD_STATE_UNUSED)) {
            return (TRUE);
        }
    }

    /* delete master rcb for UCD */
    if ((p_rcb = l2cu_find_rcb_by_psm (L2C_UCD_RCB_ID)) != NULL) {
        l2cu_release_rcb (p_rcb);
    }

    /* delete CCB for UCD */
    p_ccb = l2cb.ccb_pool;
    for ( xx = 0; xx < MAX_L2CAP_CHANNELS; xx++ ) {
        if (( p_ccb->in_use )
                && ( p_ccb->local_cid == L2CAP_CONNECTIONLESS_CID )) {
            l2cu_release_ccb (p_ccb);
        }
        p_ccb++;
    }

    return (TRUE);
}

/*******************************************************************************
**
**  Function        L2CA_UcdDiscover
**
**  Description     Discover UCD of remote device.
**
**  Parameters:     PSM
**                  BD_ADDR of remote device
**                  info_type : L2CAP_UCD_INFO_TYPE_RECEPTION
**                              L2CAP_UCD_INFO_TYPE_MTU
**
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
BOOLEAN L2CA_UcdDiscover ( UINT16 psm, BD_ADDR rem_bda, UINT8 info_type )
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;
    tL2C_RCB        *p_rcb;

    L2CAP_TRACE_API ("L2CA_UcdDiscover()  PSM: 0x%04x  BDA: %08x%04x, InfoType=0x%02x", psm,
                     (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                     (rem_bda[4] << 8) + rem_bda[5], info_type);

    /* Fail if the PSM is not registered */
    if (((p_rcb = l2cu_find_rcb_by_psm (psm)) == NULL)
            || ( p_rcb->ucd.state == L2C_UCD_STATE_UNUSED )) {
        L2CAP_TRACE_WARNING ("L2CAP - no RCB for L2CA_UcdDiscover, PSM: 0x%04x", psm);
        return (FALSE);
    }

    /* First, see if we already have a link to the remote */
    /* then find the channel control block for UCD. */
    if (((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_BR_EDR)) == NULL)
            || ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID)) == NULL)) {
        if ( l2c_ucd_connect (rem_bda) == FALSE ) {
            return (FALSE);
        }
    }

    /* set waiting flags in rcb */

    if ( info_type & L2CAP_UCD_INFO_TYPE_RECEPTION ) {
        p_rcb->ucd.state |= L2C_UCD_STATE_W4_RECEPTION;
    }

    if ( info_type & L2CAP_UCD_INFO_TYPE_MTU ) {
        p_rcb->ucd.state |= L2C_UCD_STATE_W4_MTU;
    }

    /* if link is already established */
    if ((p_lcb) && (p_lcb->link_state == LST_CONNECTED)) {
        if (!p_ccb) {
            p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID);
        }
        l2c_ucd_check_pending_info_req(p_ccb);
    }
    return (TRUE);
}

/*******************************************************************************
**
**  Function        L2CA_UcdDataWrite
**
**  Description     Send UCD to remote device
**
**  Parameters:     PSM
**                  BD Address of remote
**                  Pointer to buffer of type BT_HDR
**                  flags : L2CAP_FLUSHABLE_CH_BASED
**                          L2CAP_FLUSHABLE_PKT
**                          L2CAP_NON_FLUSHABLE_PKT
**
** Return value     L2CAP_DW_SUCCESS, if data accepted
**                  L2CAP_DW_FAILED,  if error
**
*******************************************************************************/
UINT16 L2CA_UcdDataWrite (UINT16 psm, BD_ADDR rem_bda, BT_HDR *p_buf, UINT16 flags)
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;
    tL2C_RCB        *p_rcb;
    UINT8           *p;

    L2CAP_TRACE_API ("L2CA_UcdDataWrite()  PSM: 0x%04x  BDA: %08x%04x", psm,
                     (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                     (rem_bda[4] << 8) + rem_bda[5]);

    /* Fail if the PSM is not registered */
    if (((p_rcb = l2cu_find_rcb_by_psm (psm)) == NULL)
            || ( p_rcb->ucd.state == L2C_UCD_STATE_UNUSED )) {
        L2CAP_TRACE_WARNING ("L2CAP - no RCB for L2CA_UcdDataWrite, PSM: 0x%04x", psm);
        osi_free (p_buf);
        return (L2CAP_DW_FAILED);
    }

    /* First, see if we already have a link to the remote */
    /*  then find the channel control block for UCD */
    if (((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_BR_EDR)) == NULL)
            || ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID)) == NULL)) {
        if ( l2c_ucd_connect (rem_bda) == FALSE ) {
            osi_free (p_buf);
            return (L2CAP_DW_FAILED);
        }

        /* If we still don't have lcb and ccb after connect attempt, then can't proceed */
        if (((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_BR_EDR)) == NULL)
                || ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID)) == NULL)) {
            osi_free (p_buf);
            return (L2CAP_DW_FAILED);
        }
    }

    /* write PSM */
    p_buf->offset -= L2CAP_UCD_OVERHEAD;
    p_buf->len += L2CAP_UCD_OVERHEAD;
    p = (UINT8 *)(p_buf + 1) + p_buf->offset;

    UINT16_TO_STREAM (p, psm);

    /* UCD MTU check */
    if ((p_lcb->ucd_mtu) && (p_buf->len > p_lcb->ucd_mtu)) {
        L2CAP_TRACE_WARNING ("L2CAP - Handle: 0x%04x  UCD bigger than peer's UCD mtu size cannot be sent", p_lcb->handle);
        osi_free (p_buf);
        return (L2CAP_DW_FAILED);
    }

    /* If already congested, do not accept any more packets */
    if (p_ccb->cong_sent) {
        L2CAP_TRACE_ERROR ("L2CAP - Handle: 0x%04x UCD cannot be sent, already congested count: %u  buff_quota: %u",
                           p_lcb->handle,
                           (fixed_queue_length(p_ccb->xmit_hold_q) +
                            fixed_queue_length(p_lcb->ucd_out_sec_pending_q)),
                           p_ccb->buff_quota);

        osi_free (p_buf);
        return (L2CAP_DW_FAILED);
    }

    /* channel based, packet based flushable or non-flushable */
    p_buf->layer_specific = flags;

    l2c_csm_execute (p_ccb, L2CEVT_L2CA_DATA_WRITE, p_buf);

    if (p_ccb->cong_sent) {
        return (L2CAP_DW_CONGESTED);
    } else {
        return (L2CAP_DW_SUCCESS);
    }
}

/*******************************************************************************
**
**  Function        L2CA_UcdSetIdleTimeout
**
**  Description     Set UCD Idle timeout.
**
**  Parameters:     BD Addr
**                  Timeout in second
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
BOOLEAN L2CA_UcdSetIdleTimeout ( BD_ADDR rem_bda, UINT16 timeout )
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;

    L2CAP_TRACE_API ("L2CA_UcdSetIdleTimeout()  Timeout: 0x%04x  BDA: %08x%04x", timeout,
                     (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                     (rem_bda[4] << 8) + rem_bda[5]);

    /* First, see if we already have a link to the remote */
    /* then find the channel control block. */
    if (((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_BR_EDR)) == NULL)
            || ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID)) == NULL)) {
        L2CAP_TRACE_WARNING ("L2CAP - no UCD channel");
        return (FALSE);
    } else {
        p_ccb->fixed_chnl_idle_tout = timeout;
        return (TRUE);
    }
}

/*******************************************************************************
**
** Function         L2CA_UCDSetTxPriority
**
** Description      Sets the transmission priority for a connectionless channel.
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
BOOLEAN L2CA_UCDSetTxPriority ( BD_ADDR rem_bda, tL2CAP_CHNL_PRIORITY priority )
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;

    L2CAP_TRACE_API ("L2CA_UCDSetTxPriority()  priority: 0x%02x  BDA: %08x%04x", priority,
                     (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                     (rem_bda[4] << 8) + rem_bda[5]);

    if ((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_BR_EDR)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no LCB for L2CA_UCDSetTxPriority");
        return (FALSE);
    }

    /* Find the channel control block */
    if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID)) == NULL) {
        L2CAP_TRACE_WARNING ("L2CAP - no CCB for L2CA_UCDSetTxPriority");
        return (FALSE);
    }

    /* it will update the order of CCB in LCB by priority and update round robin service variables */
    l2cu_change_pri_ccb (p_ccb, priority);

    return (TRUE);
}

/*******************************************************************************
**
**  Function        l2c_ucd_connect
**
**  Description     Connect UCD to remote device.
**
**  Parameters:     BD_ADDR of remote device
**
**  Return value:   TRUE if successs
**
*******************************************************************************/
static BOOLEAN l2c_ucd_connect ( BD_ADDR rem_bda )
{
    tL2C_LCB        *p_lcb;
    tL2C_CCB        *p_ccb;
    tL2C_RCB        *p_rcb;

    L2CAP_TRACE_DEBUG ("l2c_ucd_connect()  BDA: %08x%04x",
                       (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                       (rem_bda[4] << 8) + rem_bda[5]);

    /* Fail if we have not established communications with the controller */
    if (!BTM_IsDeviceUp()) {
        L2CAP_TRACE_WARNING ("l2c_ucd_connect - BTU not ready");
        return (FALSE);
    }

    /* First, see if we already have a link to the remote */
    if ((p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_BR_EDR)) == NULL) {
        /* No link. Get an LCB and start link establishment */
        if ( ((p_lcb = l2cu_allocate_lcb (rem_bda, FALSE, BT_TRANSPORT_BR_EDR)) == NULL)
                ||  (l2cu_create_conn(p_lcb, BT_TRANSPORT_BR_EDR) == FALSE) ) {
            L2CAP_TRACE_WARNING ("L2CAP - conn not started l2c_ucd_connect");
            return (FALSE);
        }
    } else if ( p_lcb->info_rx_bits & (1 << L2CAP_EXTENDED_FEATURES_INFO_TYPE) ) {
        if (!(p_lcb->peer_ext_fea & L2CAP_EXTFEA_UCD_RECEPTION)) {
            L2CAP_TRACE_WARNING ("L2CAP - UCD is not supported by peer, l2c_ucd_connect");
            return (FALSE);
        }
    }

    /* Find the channel control block. */
    if ((p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID)) == NULL) {
        /* Allocate a channel control block */
        if ((p_ccb = l2cu_allocate_ccb (p_lcb, 0)) == NULL) {
            L2CAP_TRACE_WARNING ("L2CAP - no CCB for l2c_ucd_connect");
            return (FALSE);
        } else {
            /* Set CID for the connection */
            p_ccb->local_cid  = L2CAP_CONNECTIONLESS_CID;
            p_ccb->remote_cid = L2CAP_CONNECTIONLESS_CID;

            /* Set the default idle timeout value to use */
            p_ccb->fixed_chnl_idle_tout = L2CAP_UCD_IDLE_TIMEOUT;

            /* Set the default channel priority value to use */
            l2cu_change_pri_ccb (p_ccb, L2CAP_UCD_CH_PRIORITY);

            if ((p_rcb = l2cu_find_rcb_by_psm (L2C_UCD_RCB_ID)) == NULL) {
                L2CAP_TRACE_WARNING ("L2CAP - no UCD registered, l2c_ucd_connect");
                return (FALSE);
            }
            /* Save UCD registration info */
            p_ccb->p_rcb = p_rcb;

            /* There is no configuration, so if the link is up, the channel is up */
            if (p_lcb->link_state == LST_CONNECTED) {
                p_ccb->chnl_state = CST_OPEN;
            }
        }
    }

    return (TRUE);
}

/*******************************************************************************
**
**  Function        l2c_ucd_delete_sec_pending_q
**
** Description      discard all of UCD packets in security pending queue
**
** Returns          None
**
*******************************************************************************/
void l2c_ucd_delete_sec_pending_q(tL2C_LCB  *p_lcb)
{
    /* clean up any security pending UCD */
    while (p_lcb->ucd_out_sec_pending_q.p_first) {
        osi_free(fixed_queue_try_dequeue(p_lcb->ucd_out_sec_pending_q));
	}
    fixed_queue_free(p_lcb->ucd_out_sec_pending_q, NULL);
    p_lcb->ucd_out_sec_pending_q = NULL;

    while (! fixed_queue_is_empty(p_lcb->ucd_in_sec_pending_q)) {
        osi_free(fixed_queue_try_dequeue(p_lcb->ucd_in_sec_pending_q));
	}
    fixed_queue_free(p_lcb->ucd_in_sec_pending_q);
    p_lcb->ucd_in_sec_pending_q = NULL;
}

/*******************************************************************************
**
**  Function        l2c_ucd_check_pending_info_req
**
** Description      check if any application is waiting for UCD information
**
**  Return          TRUE if any pending UCD info request
**
*******************************************************************************/
BOOLEAN l2c_ucd_check_pending_info_req(tL2C_CCB  *p_ccb)
{
    tL2C_RCB    *p_rcb = &l2cb.rcb_pool[0];
    UINT16      xx;
    BOOLEAN     pending = FALSE;

    if (p_ccb == NULL) {
        L2CAP_TRACE_ERROR ("L2CAP - NULL p_ccb in l2c_ucd_check_pending_info_req");
        return (FALSE);
    }

    for (xx = 0; xx < MAX_L2CAP_CLIENTS; xx++, p_rcb++) {
        if (p_rcb->in_use) {
            /* if application is waiting UCD reception info */
            if (p_rcb->ucd.state & L2C_UCD_STATE_W4_RECEPTION) {
                /* if this information is available */
                if ( p_ccb->p_lcb->info_rx_bits & (1 << L2CAP_EXTENDED_FEATURES_INFO_TYPE) ) {
                    if (!(p_ccb->p_lcb->peer_ext_fea & L2CAP_EXTFEA_UCD_RECEPTION)) {
                        L2CAP_TRACE_WARNING ("L2CAP - UCD is not supported by peer, l2c_ucd_check_pending_info_req");

                        l2c_ucd_delete_sec_pending_q(p_ccb->p_lcb);
                        l2cu_release_ccb (p_ccb);
                    }

                    p_ccb->p_rcb->ucd.cb_info.pL2CA_UCD_Discover_Cb (p_ccb->p_lcb->remote_bd_addr,
                            L2CAP_UCD_INFO_TYPE_RECEPTION,
                            p_ccb->p_lcb->peer_ext_fea & L2CAP_EXTFEA_UCD_RECEPTION);
                } else {
                    pending = TRUE;
                    if (p_ccb->p_lcb->w4_info_rsp == FALSE) {
                        l2cu_send_peer_info_req (p_ccb->p_lcb, L2CAP_EXTENDED_FEATURES_INFO_TYPE);
                    }
                }
            }

            /* if application is waiting for UCD MTU */
            if (p_rcb->ucd.state & L2C_UCD_STATE_W4_MTU) {
                /* if this information is available */
                if ( p_ccb->p_lcb->info_rx_bits & (1 << L2CAP_CONNLESS_MTU_INFO_TYPE)) {
                    p_ccb->p_rcb->ucd.cb_info.pL2CA_UCD_Discover_Cb (p_ccb->p_lcb->remote_bd_addr,
                            L2CAP_UCD_INFO_TYPE_MTU,
                            p_ccb->p_lcb->ucd_mtu);
                } else {
                    pending = TRUE;
                    if (p_ccb->p_lcb->w4_info_rsp == FALSE) {
                        l2cu_send_peer_info_req (p_ccb->p_lcb, L2CAP_CONNLESS_MTU_INFO_TYPE);
                    }
                }
            }
        }
    }
    return (pending);
}

/*******************************************************************************
**
**  Function        l2c_ucd_enqueue_pending_out_sec_q
**
**  Description     enqueue outgoing UCD packet into security pending queue
**                  and check congestion
**
**  Return          None
**
*******************************************************************************/
void l2c_ucd_enqueue_pending_out_sec_q(tL2C_CCB  *p_ccb, void *p_data)
{
    fixed_queue_enqueue(p_ccb->p_lcb->ucd_out_sec_pending_q, p_data);
    l2cu_check_channel_congestion (p_ccb);
}

/*******************************************************************************
**
**  Function        l2c_ucd_check_pending_out_sec_q
**
**  Description     check outgoing security
**
**  Return          TRUE if any UCD packet for security
**
*******************************************************************************/
BOOLEAN l2c_ucd_check_pending_out_sec_q(tL2C_CCB  *p_ccb)
{
    BT_HDR *p_buf = (BT_HDR*)fixed_queue_try_peek_first(p_ccb->p_lcb->ucd_out_sec_pending_q);

    if (p_buf != NULL) {
        UINT16 psm;
        UINT8 *p = (UINT8 *)(p_buf + 1) + p_buf->offset;

        STREAM_TO_UINT16(psm, p)

        p_ccb->chnl_state = CST_ORIG_W4_SEC_COMP;
        btm_sec_l2cap_access_req (p_ccb->p_lcb->remote_bd_addr, psm,
                                  p_ccb->p_lcb->handle, CONNLESS_ORIG, &l2c_link_sec_comp, p_ccb);

        return (TRUE);
    }
    return (FALSE);
}

/*******************************************************************************
**
**  Function        l2c_ucd_send_pending_out_sec_q
**
**  Description     dequeue UCD packet from security pending queue and
**                  enqueue it into CCB
**
**  Return          None
**
*******************************************************************************/
void l2c_ucd_send_pending_out_sec_q(tL2C_CCB  *p_ccb)
{
    BT_HDR *p_buf = (BT_HDR*)fixed_queue_try_dequeue(p_ccb->p_lcb->ucd_out_sec_pending_q);

    if (p_buf != NULL) {
        l2c_enqueue_peer_data (p_ccb, (BT_HDR *)p_buf);
        l2c_link_check_send_pkts (p_ccb->p_lcb, NULL, NULL);
    }
}

/*******************************************************************************
**
**  Function        l2c_ucd_discard_pending_out_sec_q
**
**  Description     dequeue UCD packet from security pending queue and
**                  discard it.
**
**  Return          None
**
*******************************************************************************/
void l2c_ucd_discard_pending_out_sec_q(tL2C_CCB  *p_ccb)
{
    BT_HDR *p_buf = (BT_HDR*)fixed_queue_try_dequeue(p_ccb->p_lcb->ucd_out_sec_pending_q);

    /* we may need to report to application */

    if (p_buf) {
        osi_free (p_buf);
    }
}

/*******************************************************************************
**
**  Function        l2c_ucd_check_pending_in_sec_q
**
**  Description     check incoming security
**
**  Return          TRUE if any UCD packet for security
**
*******************************************************************************/
BOOLEAN l2c_ucd_check_pending_in_sec_q(tL2C_CCB  *p_ccb)
{
    BT_HDR *p_buf = (BT_HDR*)fixed_queue_try_dequeue(p_ccb->p_lcb->ucd_in_sec_pending_q);

    if (p_buf != NULL) {
        UINT16 psm;
        UINT8 *p = (UINT8 *)(p_buf + 1) + p_buf->offset;
        STREAM_TO_UINT16(psm, p)

        p_ccb->chnl_state = CST_TERM_W4_SEC_COMP;
        btm_sec_l2cap_access_req (p_ccb->p_lcb->remote_bd_addr, psm,
                                  p_ccb->p_lcb->handle, CONNLESS_TERM, &l2c_link_sec_comp, p_ccb);

        return (TRUE);
    }
    return (FALSE);
}

/*******************************************************************************
**
**  Function        l2c_ucd_send_pending_in_sec_q
**
**  Description     dequeue UCD packet from security pending queue and
**                  send it to application
**
**  Return          None
**
*******************************************************************************/
void l2c_ucd_send_pending_in_sec_q(tL2C_CCB  *p_ccb)
{
    BT_HDR *p_buf = (BT_HDR*)fixed_queue_try_dequeue(p_ccb->p_lcb->ucd_in_sec_pending_q)

    if (p_buf != NULL) {
        p_ccb->p_rcb->ucd.cb_info.pL2CA_UCD_Data_Cb(p_ccb->p_lcb->remote_bd_addr, (BT_HDR *)p_buf);
    }
}

/*******************************************************************************
**
**  Function        l2c_ucd_discard_pending_in_sec_q
**
**  Description     dequeue UCD packet from security pending queue and
**                  discard it.
**
**  Return          None
**
*******************************************************************************/
void l2c_ucd_discard_pending_in_sec_q(tL2C_CCB  *p_ccb)
{
    BT_HDR *p_buf = (BT_HDR*)fixed_queue_try_dequeue(p_ccb->p_lcb->ucd_in_sec_pending_q);

    if (p_buf) {
        osi_free (p_buf);
    }
}

/*******************************************************************************
**
**  Function        l2c_ucd_check_rx_pkts
**
**  Description     Check if UCD reception is registered.
**                  Process received UCD packet if application is expecting.
**
**  Return          TRUE if UCD reception is registered
**
*******************************************************************************/
BOOLEAN l2c_ucd_check_rx_pkts(tL2C_LCB  *p_lcb, BT_HDR *p_msg)
{
    tL2C_CCB   *p_ccb;
    tL2C_RCB   *p_rcb;

    if (((p_ccb = l2cu_find_ccb_by_cid (p_lcb, L2CAP_CONNECTIONLESS_CID)) != NULL)
            || ((p_rcb = l2cu_find_rcb_by_psm (L2C_UCD_RCB_ID)) != NULL)) {
        if (p_ccb == NULL) {
            /* Allocate a channel control block */
            if ((p_ccb = l2cu_allocate_ccb (p_lcb, 0)) == NULL) {
                L2CAP_TRACE_WARNING ("L2CAP - no CCB for UCD reception");
                osi_free (p_msg);
                return TRUE;
            } else {
                /* Set CID for the connection */
                p_ccb->local_cid  = L2CAP_CONNECTIONLESS_CID;
                p_ccb->remote_cid = L2CAP_CONNECTIONLESS_CID;

                /* Set the default idle timeout value to use */
                p_ccb->fixed_chnl_idle_tout = L2CAP_UCD_IDLE_TIMEOUT;

                /* Set the default channel priority value to use */
                l2cu_change_pri_ccb (p_ccb, L2CAP_UCD_CH_PRIORITY);

                /* Save registration info */
                p_ccb->p_rcb = p_rcb;

                p_ccb->chnl_state = CST_OPEN;
            }
        }
        l2c_csm_execute(p_ccb, L2CEVT_L2CAP_DATA, p_msg);
        return TRUE;
    } else {
        return FALSE;
    }
}

/*******************************************************************************
**
**  Function        l2c_ucd_process_event
**
**  Description     This is called from main state machine when LCID is connectionless
**                  Process the event if it is for UCD.
**
**  Return          TRUE if the event is consumed by UCD
**                  FALSE if the event needs to be processed by main state machine
**
*******************************************************************************/
BOOLEAN l2c_ucd_process_event(tL2C_CCB *p_ccb, UINT16 event, void *p_data)
{
    /* if the event is not processed by this function, this variable will be set to FALSE */
    BOOLEAN done = TRUE;

    switch (p_ccb->chnl_state) {
    case CST_CLOSED:
        switch (event) {
        case L2CEVT_LP_CONNECT_CFM:     /* Link came up         */
            /* check if waiting for UCD info */
            if (!l2c_ucd_check_pending_info_req (p_ccb)) {
                /* check if any outgoing UCD packet is waiting security check */
                if (!l2c_ucd_check_pending_out_sec_q(p_ccb)) {
                    p_ccb->chnl_state = CST_OPEN;
                }
            }
            break;

        case L2CEVT_L2CAP_DATA:         /* Peer data packet rcvd    */
            fixed_queue_enqueue(p_ccb->p_lcb->ucd_in_sec_pending_q, p_data);
            break;

        case L2CEVT_L2CA_DATA_WRITE:    /* Upper layer data to send */
            l2c_ucd_enqueue_pending_out_sec_q(p_ccb, p_data);
            break;

        case L2CEVT_L2CAP_INFO_RSP:
            /* check if waiting for UCD info */
            if (!l2c_ucd_check_pending_info_req (p_ccb)) {
                /* check if any outgoing UCD packet is waiting security check */
                if (!l2c_ucd_check_pending_out_sec_q(p_ccb)) {
                    p_ccb->chnl_state = CST_OPEN;
                }
            }
            break;

        default:
            done = FALSE;   /* main state machine continues to process event */
            break;
        }
        break;

    case CST_ORIG_W4_SEC_COMP:
        switch (event) {
        case L2CEVT_SEC_RE_SEND_CMD:    /* BTM has enough info to proceed */
            /* check if any outgoing UCD packet is waiting security check */
            if (!l2c_ucd_check_pending_out_sec_q(p_ccb)) {
                p_ccb->chnl_state = CST_OPEN;
            }
            break;

        case L2CEVT_SEC_COMP:           /* Security completed success */
            p_ccb->chnl_state = CST_OPEN;
            l2c_ucd_send_pending_out_sec_q(p_ccb);

            if (! fixed_queue_is_empty(p_ccb->p_lcb->ucd_out_sec_pending_q))
            {
                /* start a timer to send next UCD packet in OPEN state */
                /* it will prevent stack overflow */
                btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, 0);
            } else {
                /* start a timer for idle timeout of UCD */
                btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, p_ccb->fixed_chnl_idle_tout);
            }
            break;

        case L2CEVT_SEC_COMP_NEG:
            p_ccb->chnl_state = CST_OPEN;
            l2c_ucd_discard_pending_out_sec_q(p_ccb);

            /* start a timer for idle timeout of UCD */
            btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, p_ccb->fixed_chnl_idle_tout);
            break;

        case L2CEVT_L2CA_DATA_WRITE:    /* Upper layer data to send */
            l2c_ucd_enqueue_pending_out_sec_q(p_ccb, p_data);
            break;

        case L2CEVT_L2CAP_DATA:         /* Peer data packet rcvd    */
            fixed_queue_enqueue(p_ccb->p_lcb->ucd_in_sec_pending_q, p_data);
            break;

        case L2CEVT_L2CAP_INFO_RSP:
            /* check if waiting for UCD info */
            l2c_ucd_check_pending_info_req (p_ccb);
            break;

        default:
            done = FALSE;   /* main state machine continues to process event */
            break;
        }
        break;


    case CST_TERM_W4_SEC_COMP:
        switch (event) {
        case L2CEVT_SEC_COMP:
            p_ccb->chnl_state = CST_OPEN;
            l2c_ucd_send_pending_in_sec_q (p_ccb);

            if (! fixed_queue_is_empty(p_ccb->p_lcb->ucd_in_sec_pending_q)) {
                /* start a timer to check next UCD packet in OPEN state */
                /* it will prevent stack overflow */
                btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, 0);
            } else {
                /* start a timer for idle timeout of UCD */
                btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, p_ccb->fixed_chnl_idle_tout);
            }
            break;

        case L2CEVT_SEC_COMP_NEG:
            if (((tL2C_CONN_INFO *)p_data)->status == BTM_DELAY_CHECK) {
                done = FALSE;
                break;
            }
            p_ccb->chnl_state = CST_OPEN;
            l2c_ucd_discard_pending_in_sec_q (p_ccb);

            /* start a timer for idle timeout of UCD */
            btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_L2CAP_CHNL, p_ccb->fixed_chnl_idle_tout);
            break;

        case L2CEVT_L2CA_DATA_WRITE:        /* Upper layer data to send */
            l2c_ucd_enqueue_pending_out_sec_q(p_ccb, p_data);
            break;

        case L2CEVT_L2CAP_DATA:             /* Peer data packet rcvd    */
            fixed_queue_enqueue(p_ccb->p_lcb->ucd_in_sec_pending_q, p_data);
            break;

        case L2CEVT_SEC_RE_SEND_CMD:        /* BTM has enough info to proceed */
            /* check if any incoming UCD packet is waiting security check */
            if (!l2c_ucd_check_pending_in_sec_q(p_ccb)) {
                p_ccb->chnl_state = CST_OPEN;
            }
            break;

        case L2CEVT_L2CAP_INFO_RSP:
            /* check if waiting for UCD info */
            l2c_ucd_check_pending_info_req (p_ccb);
            break;

        default:
            done = FALSE;   /* main state machine continues to process event */
            break;
        }
        break;

    case CST_OPEN:
        switch (event) {
        case L2CEVT_L2CAP_DATA:             /* Peer data packet rcvd    */
            /* stop idle timer of UCD */
            btu_stop_timer (&p_ccb->timer_entry);

            fixed_queue_enqueue(p_ccb->p_lcb->ucd_in_sec_pending_q, p_data);
            l2c_ucd_check_pending_in_sec_q (p_ccb);
            break;

        case L2CEVT_L2CA_DATA_WRITE:        /* Upper layer data to send */
            /* stop idle timer of UCD */
            btu_stop_timer (&p_ccb->timer_entry);

            l2c_ucd_enqueue_pending_out_sec_q(p_ccb, p_data);

            /* coverity[check_return] */ /* coverity[unchecked_value] */
            /* success changes state, failure stays in current state */
            l2c_ucd_check_pending_out_sec_q (p_ccb);
            break;

        case L2CEVT_TIMEOUT:
            /* check if any UCD packet is waiting security check */
            if ((!l2c_ucd_check_pending_in_sec_q(p_ccb))
                    && (!l2c_ucd_check_pending_out_sec_q(p_ccb))) {
                l2cu_release_ccb (p_ccb);
            }
            break;

        case L2CEVT_L2CAP_INFO_RSP:
            /* check if waiting for UCD info */
            l2c_ucd_check_pending_info_req (p_ccb);
            break;

        default:
            done = FALSE;   /* main state machine continues to process event */
            break;
        }
        break;

    default:
        done = FALSE;   /* main state machine continues to process event */
        break;
    }

    return done;
}
#endif /* (L2CAP_UCD_INCLUDED == TRUE) */
