/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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
 *  this file contains functions relating to BLE management.
 *
 ******************************************************************************/

#include <string.h>
#include "bt_target.h"
//#include "bt_utils.h"
#include "l2cdefs.h"
#include "l2c_int.h"
#include "btu.h"
#include "btm_int.h"
#include "hcimsgs.h"
#include "controller.h"

#if (BLE_INCLUDED == TRUE)
static void l2cble_start_conn_update (tL2C_LCB *p_lcb);

/*******************************************************************************
**
**  Function        L2CA_CancelBleConnectReq
**
**  Description     Cancel a pending connection attempt to a BLE device.
**
**  Parameters:     BD Address of remote
**
**  Return value:   TRUE if connection was cancelled
**
*******************************************************************************/
BOOLEAN L2CA_CancelBleConnectReq (BD_ADDR rem_bda)
{
    tL2C_LCB *p_lcb;

    /* There can be only one BLE connection request outstanding at a time */
    if (btm_ble_get_conn_st() == BLE_CONN_IDLE) {
        L2CAP_TRACE_WARNING ("L2CA_CancelBleConnectReq - no connection pending");
        return (FALSE);
    }

    if (memcmp (rem_bda, l2cb.ble_connecting_bda, BD_ADDR_LEN)) {
        L2CAP_TRACE_WARNING ("L2CA_CancelBleConnectReq - different  BDA Connecting: %08x%04x  Cancel: %08x%04x",
                             (l2cb.ble_connecting_bda[0] << 24) + (l2cb.ble_connecting_bda[1] << 16) + (l2cb.ble_connecting_bda[2] << 8) + l2cb.ble_connecting_bda[3],
                             (l2cb.ble_connecting_bda[4] << 8) + l2cb.ble_connecting_bda[5],
                             (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3], (rem_bda[4] << 8) + rem_bda[5]);

        return (FALSE);
    }

    if (btsnd_hcic_ble_create_conn_cancel()) {
        p_lcb = l2cu_find_lcb_by_bd_addr(rem_bda, BT_TRANSPORT_LE);
        /* Do not remove lcb if an LE link is already up as a peripheral */
        if (p_lcb != NULL &&
                !(p_lcb->link_role == HCI_ROLE_SLAVE && BTM_ACL_IS_CONNECTED(rem_bda))) {
            p_lcb->disc_reason = L2CAP_CONN_CANCEL;
            l2cu_release_lcb (p_lcb);
        }
        /* update state to be cancel, wait for connection cancel complete */
        btm_ble_set_conn_st (BLE_CONN_CANCEL);

        return (TRUE);
    } else {
        return (FALSE);
    }
}

/*******************************************************************************
**
**  Function        L2CA_UpdateBleConnParams
**
**  Description     Update BLE connection parameters.
**
**  Parameters:     BD Address of remote
**
**  Return value:   TRUE if update started
**
*******************************************************************************/
BOOLEAN L2CA_UpdateBleConnParams (BD_ADDR rem_bda, UINT16 min_int, UINT16 max_int,
                                  UINT16 latency, UINT16 timeout)
{
    tL2C_LCB            *p_lcb;
    tACL_CONN           *p_acl_cb = btm_bda_to_acl(rem_bda, BT_TRANSPORT_LE);

    /* See if we have a link control block for the remote device */
    p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_LE);

    /* If we don't have one, create one and accept the connection. */
    if (!p_lcb || !p_acl_cb) {
        L2CAP_TRACE_WARNING ("L2CA_UpdateBleConnParams - unknown BD_ADDR %08x%04x",
                             (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                             (rem_bda[4] << 8) + rem_bda[5]);
        return (FALSE);
    }

    if (p_lcb->transport != BT_TRANSPORT_LE) {
        L2CAP_TRACE_WARNING ("L2CA_UpdateBleConnParams - BD_ADDR %08x%04x not LE",
                             (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                             (rem_bda[4] << 8) + rem_bda[5]);
        return (FALSE);
    }

    p_lcb->min_interval = min_int;
    p_lcb->max_interval = max_int;
    p_lcb->latency = latency;
    p_lcb->timeout = timeout;
    p_lcb->conn_update_mask |= L2C_BLE_NEW_CONN_PARAM;

    l2cble_start_conn_update(p_lcb);

    return (TRUE);
}


/*******************************************************************************
**
**  Function        L2CA_EnableUpdateBleConnParams
**
**  Description     Enable or disable update based on the request from the peer
**
**  Parameters:     BD Address of remote
**
**  Return value:   TRUE if update started
**
*******************************************************************************/
BOOLEAN L2CA_EnableUpdateBleConnParams (BD_ADDR rem_bda, BOOLEAN enable)
{
    tL2C_LCB            *p_lcb;

    /* See if we have a link control block for the remote device */
    p_lcb = l2cu_find_lcb_by_bd_addr (rem_bda, BT_TRANSPORT_LE);

    if (!p_lcb) {
        L2CAP_TRACE_WARNING ("L2CA_EnableUpdateBleConnParams - unknown BD_ADDR %08x%04x",
                             (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                             (rem_bda[4] << 8) + rem_bda[5]);
        return (FALSE);
    }

    L2CAP_TRACE_API ("%s - BD_ADDR %08x%04x enable %d current upd state 0x%02x", __FUNCTION__,
                     (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                     (rem_bda[4] << 8) + rem_bda[5], enable, p_lcb->conn_update_mask);

    if (p_lcb->transport != BT_TRANSPORT_LE) {
        L2CAP_TRACE_WARNING ("%s - BD_ADDR %08x%04x not LE (link role %d)", __FUNCTION__,
                             (rem_bda[0] << 24) + (rem_bda[1] << 16) + (rem_bda[2] << 8) + rem_bda[3],
                             (rem_bda[4] << 8) + rem_bda[5], p_lcb->link_role);
        return (FALSE);
    }

    if (enable) {
        p_lcb->conn_update_mask &= ~L2C_BLE_CONN_UPDATE_DISABLE;
    } else {
        p_lcb->conn_update_mask |= L2C_BLE_CONN_UPDATE_DISABLE;
    }

    l2cble_start_conn_update(p_lcb);

    return (TRUE);
}


/*******************************************************************************
**
** Function         L2CA_GetBleConnRole
**
** Description      This function returns the connection role.
**
** Returns          link role.
**
*******************************************************************************/
UINT8 L2CA_GetBleConnRole (BD_ADDR bd_addr)
{
    UINT8       role = HCI_ROLE_UNKNOWN;

    tL2C_LCB *p_lcb;

    if ((p_lcb = l2cu_find_lcb_by_bd_addr (bd_addr, BT_TRANSPORT_LE)) != NULL) {
        role = p_lcb->link_role;
    }

    return role;
}
/*******************************************************************************
**
** Function         L2CA_GetDisconnectReason
**
** Description      This function returns the disconnect reason code.
**
** Returns          disconnect reason
**
*******************************************************************************/
UINT16 L2CA_GetDisconnectReason (BD_ADDR remote_bda, tBT_TRANSPORT transport)
{
    tL2C_LCB            *p_lcb;
    UINT16              reason = 0;

    if ((p_lcb = l2cu_find_lcb_by_bd_addr (remote_bda, transport)) != NULL) {
        reason = p_lcb->disc_reason;
    }

    L2CAP_TRACE_DEBUG ("L2CA_GetDisconnectReason=%d ", reason);

    return reason;
}

/*******************************************************************************
**
** Function l2cble_notify_le_connection
**
** Description This function notifiy the l2cap connection to the app layer
**
** Returns none
**
*******************************************************************************/
void l2cble_notify_le_connection (BD_ADDR bda)
{
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr (bda, BT_TRANSPORT_LE);
    tACL_CONN *p_acl = btm_bda_to_acl(bda, BT_TRANSPORT_LE) ;

    if (p_lcb != NULL && p_acl != NULL && p_lcb->link_state != LST_CONNECTED) {
        /* update link status */
        btm_establish_continue(p_acl);
        /* update l2cap link status and send callback */
        p_lcb->link_state = LST_CONNECTED;
        l2cu_process_fixed_chnl_resp (p_lcb);
    }
}

/*******************************************************************************
**
** Function         l2cble_scanner_conn_comp
**
** Description      This function is called when an HCI Connection Complete
**                  event is received while we are a scanner (so we are master).
**
** Returns          void
**
*******************************************************************************/
void l2cble_scanner_conn_comp (UINT16 handle, BD_ADDR bda, tBLE_ADDR_TYPE type,
                               UINT16 conn_interval, UINT16 conn_latency, UINT16 conn_timeout)
{
    tL2C_LCB            *p_lcb;
    tBTM_SEC_DEV_REC    *p_dev_rec = btm_find_or_alloc_dev (bda);

    L2CAP_TRACE_DEBUG ("l2cble_scanner_conn_comp: HANDLE=%d addr_type=%d conn_interval=%d slave_latency=%d supervision_tout=%d",
                       handle,  type, conn_interval, conn_latency, conn_timeout);

    l2cb.is_ble_connecting = FALSE;

    /* See if we have a link control block for the remote device */
    p_lcb = l2cu_find_lcb_by_bd_addr (bda, BT_TRANSPORT_LE);

    /* If we don't have one, create one. this is auto connection complete. */
    if (!p_lcb) {
        p_lcb = l2cu_allocate_lcb (bda, FALSE, BT_TRANSPORT_LE);
        if (!p_lcb) {
            btm_sec_disconnect (handle, HCI_ERR_NO_CONNECTION);
            L2CAP_TRACE_ERROR ("l2cble_scanner_conn_comp - failed to allocate LCB");
            return;
        } else {
            if (!l2cu_initialize_fixed_ccb (p_lcb, L2CAP_ATT_CID, &l2cb.fixed_reg[L2CAP_ATT_CID - L2CAP_FIRST_FIXED_CHNL].fixed_chnl_opts)) {
                btm_sec_disconnect (handle, HCI_ERR_NO_CONNECTION);
                L2CAP_TRACE_WARNING ("l2cble_scanner_conn_comp - LCB but no CCB");
                return ;
            }
        }
    } else if (p_lcb->link_state != LST_CONNECTING) {
        L2CAP_TRACE_ERROR ("L2CAP got BLE scanner conn_comp in bad state: %d", p_lcb->link_state);
        return;
    }
    btu_stop_timer(&p_lcb->timer_entry);

    /* Save the handle */
    p_lcb->handle = handle;

    /* Connected OK. Change state to connected, we were scanning so we are master */
    p_lcb->link_role  = HCI_ROLE_MASTER;
    p_lcb->transport  = BT_TRANSPORT_LE;

    /* update link parameter, set slave link as non-spec default upon link up */
    p_lcb->min_interval =  p_lcb->max_interval = conn_interval;
    p_lcb->timeout      =  conn_timeout;
    p_lcb->latency      =  conn_latency;
    p_lcb->conn_update_mask = L2C_BLE_NOT_DEFAULT_PARAM;

    /* If there are any preferred connection parameters, set them now */
    if ( (p_dev_rec->conn_params.min_conn_int     >= BTM_BLE_CONN_INT_MIN ) &&
            (p_dev_rec->conn_params.min_conn_int     <= BTM_BLE_CONN_INT_MAX ) &&
            (p_dev_rec->conn_params.max_conn_int     >= BTM_BLE_CONN_INT_MIN ) &&
            (p_dev_rec->conn_params.max_conn_int     <= BTM_BLE_CONN_INT_MAX ) &&
            (p_dev_rec->conn_params.slave_latency    <= BTM_BLE_CONN_LATENCY_MAX ) &&
            (p_dev_rec->conn_params.supervision_tout >= BTM_BLE_CONN_SUP_TOUT_MIN) &&
            (p_dev_rec->conn_params.supervision_tout <= BTM_BLE_CONN_SUP_TOUT_MAX) &&
            ((conn_interval < p_dev_rec->conn_params.min_conn_int &&
              p_dev_rec->conn_params.min_conn_int != BTM_BLE_CONN_PARAM_UNDEF) ||
             (conn_interval > p_dev_rec->conn_params.max_conn_int) ||
             (conn_latency > p_dev_rec->conn_params.slave_latency) ||
             (conn_timeout > p_dev_rec->conn_params.supervision_tout))) {
        L2CAP_TRACE_ERROR ("upd_ll_conn_params: HANDLE=%d min_conn_int=%d max_conn_int=%d slave_latency=%d supervision_tout=%d",
                           handle, p_dev_rec->conn_params.min_conn_int, p_dev_rec->conn_params.max_conn_int,
                           p_dev_rec->conn_params.slave_latency, p_dev_rec->conn_params.supervision_tout);

        p_lcb->min_interval = p_dev_rec->conn_params.min_conn_int;
        p_lcb->max_interval = p_dev_rec->conn_params.max_conn_int;
        p_lcb->timeout      = p_dev_rec->conn_params.supervision_tout;
        p_lcb->latency      = p_dev_rec->conn_params.slave_latency;

        btsnd_hcic_ble_upd_ll_conn_params (handle,
                                           p_dev_rec->conn_params.min_conn_int,
                                           p_dev_rec->conn_params.max_conn_int,
                                           p_dev_rec->conn_params.slave_latency,
                                           p_dev_rec->conn_params.supervision_tout,
                                           0, 0);
    }

    /* Tell BTM Acl management about the link */
    btm_acl_created (bda, NULL, p_dev_rec->sec_bd_name, handle, p_lcb->link_role, BT_TRANSPORT_LE);

    p_lcb->peer_chnl_mask[0] = L2CAP_FIXED_CHNL_ATT_BIT | L2CAP_FIXED_CHNL_BLE_SIG_BIT | L2CAP_FIXED_CHNL_SMP_BIT;

    btm_ble_set_conn_st(BLE_CONN_IDLE);

#if BLE_PRIVACY_SPT == TRUE
    btm_ble_disable_resolving_list(BTM_BLE_RL_INIT, TRUE);
#endif
}


/*******************************************************************************
**
** Function         l2cble_advertiser_conn_comp
**
** Description      This function is called when an HCI Connection Complete
**                  event is received while we are an advertiser (so we are slave).
**
** Returns          void
**
*******************************************************************************/
void l2cble_advertiser_conn_comp (UINT16 handle, BD_ADDR bda, tBLE_ADDR_TYPE type,
                                  UINT16 conn_interval, UINT16 conn_latency, UINT16 conn_timeout)
{
    tL2C_LCB            *p_lcb;
    tBTM_SEC_DEV_REC    *p_dev_rec;
    UNUSED(type);
    UNUSED(conn_interval);
    UNUSED(conn_latency);
    UNUSED(conn_timeout);

    /* See if we have a link control block for the remote device */
    p_lcb = l2cu_find_lcb_by_bd_addr (bda, BT_TRANSPORT_LE);

    /* If we don't have one, create one and accept the connection. */
    if (!p_lcb) {
        p_lcb = l2cu_allocate_lcb (bda, FALSE, BT_TRANSPORT_LE);
        if (!p_lcb) {
            btm_sec_disconnect (handle, HCI_ERR_NO_CONNECTION);
            L2CAP_TRACE_ERROR ("l2cble_advertiser_conn_comp - failed to allocate LCB");
            return;
        } else {
            if (!l2cu_initialize_fixed_ccb (p_lcb, L2CAP_ATT_CID, &l2cb.fixed_reg[L2CAP_ATT_CID - L2CAP_FIRST_FIXED_CHNL].fixed_chnl_opts)) {
                btm_sec_disconnect (handle, HCI_ERR_NO_CONNECTION);
                L2CAP_TRACE_WARNING ("l2cble_scanner_conn_comp - LCB but no CCB");
                return ;
            }
        }
    }

    /* Save the handle */
    p_lcb->handle = handle;

    /* Connected OK. Change state to connected, we were advertising, so we are slave */
    p_lcb->link_role  = HCI_ROLE_SLAVE;
    p_lcb->transport  = BT_TRANSPORT_LE;

    /* update link parameter, set slave link as non-spec default upon link up */
    p_lcb->min_interval = p_lcb->max_interval = conn_interval;
    p_lcb->timeout      =  conn_timeout;
    p_lcb->latency      =  conn_latency;
    p_lcb->conn_update_mask = L2C_BLE_NOT_DEFAULT_PARAM;

    /* Tell BTM Acl management about the link */
    p_dev_rec = btm_find_or_alloc_dev (bda);

    btm_acl_created (bda, NULL, p_dev_rec->sec_bd_name, handle, p_lcb->link_role, BT_TRANSPORT_LE);

#if BLE_PRIVACY_SPT == TRUE
    btm_ble_disable_resolving_list(BTM_BLE_RL_ADV, TRUE);
#endif

    p_lcb->peer_chnl_mask[0] = L2CAP_FIXED_CHNL_ATT_BIT | L2CAP_FIXED_CHNL_BLE_SIG_BIT | L2CAP_FIXED_CHNL_SMP_BIT;

    if (!HCI_LE_SLAVE_INIT_FEAT_EXC_SUPPORTED(controller_get_interface()->get_features_ble()->as_array)) {
        p_lcb->link_state = LST_CONNECTED;
        l2cu_process_fixed_chnl_resp (p_lcb);
    }

    /* when adv and initiating are both active, cancel the direct connection */
    if (l2cb.is_ble_connecting && memcmp(bda, l2cb.ble_connecting_bda, BD_ADDR_LEN) == 0) {
        L2CA_CancelBleConnectReq(bda);
    }
}

/*******************************************************************************
**
** Function         l2cble_conn_comp
**
** Description      This function is called when an HCI Connection Complete
**                  event is received.
**
** Returns          void
**
*******************************************************************************/
void l2cble_conn_comp(UINT16 handle, UINT8 role, BD_ADDR bda, tBLE_ADDR_TYPE type,
                      UINT16 conn_interval, UINT16 conn_latency, UINT16 conn_timeout)
{
    btm_ble_update_link_topology_mask(role, TRUE);

    if (role == HCI_ROLE_MASTER) {
        l2cble_scanner_conn_comp(handle, bda, type, conn_interval, conn_latency, conn_timeout);
    } else {
        l2cble_advertiser_conn_comp(handle, bda, type, conn_interval, conn_latency, conn_timeout);
    }
}

/*******************************************************************************
**
**  Function        l2cble_start_conn_update
**
**  Description     start BLE connection parameter update process based on status
**
**  Parameters:     lcb : l2cap link control block
**
**  Return value:   none
**
*******************************************************************************/
static void l2cble_start_conn_update (tL2C_LCB *p_lcb)
{
    UINT16 min_conn_int, max_conn_int, slave_latency, supervision_tout;
    tACL_CONN *p_acl_cb = btm_bda_to_acl(p_lcb->remote_bd_addr, BT_TRANSPORT_LE);

    if (p_lcb->conn_update_mask & L2C_BLE_UPDATE_PENDING) {
        return;
    }

    if (p_lcb->conn_update_mask & L2C_BLE_CONN_UPDATE_DISABLE) {
        /* application requests to disable parameters update.
           If parameters are already updated, lets set them
           up to what has been requested during connection establishement */
        if (p_lcb->conn_update_mask & L2C_BLE_NOT_DEFAULT_PARAM &&
                /* current connection interval is greater than default min */
                p_lcb->min_interval > BTM_BLE_CONN_INT_MIN) {
            /* use 7.5 ms as fast connection parameter, 0 slave latency */
            min_conn_int = max_conn_int = BTM_BLE_CONN_INT_MIN;
            slave_latency = BTM_BLE_CONN_SLAVE_LATENCY_DEF;
            supervision_tout = BTM_BLE_CONN_TIMEOUT_DEF;

            /* if both side 4.1, or we are master device, send HCI command */
            if (p_lcb->link_role == HCI_ROLE_MASTER
#if (defined BLE_LLT_INCLUDED) && (BLE_LLT_INCLUDED == TRUE)
                    || (HCI_LE_CONN_PARAM_REQ_SUPPORTED(controller_get_interface()->get_features_ble()->as_array) &&
                        HCI_LE_CONN_PARAM_REQ_SUPPORTED(p_acl_cb->peer_le_features))
#endif
               ) {
                btsnd_hcic_ble_upd_ll_conn_params(p_lcb->handle, min_conn_int, max_conn_int,
                                                  slave_latency, supervision_tout, 0, 0);
                p_lcb->conn_update_mask |= L2C_BLE_UPDATE_PENDING;
            } else {
                l2cu_send_peer_ble_par_req (p_lcb, min_conn_int, max_conn_int, slave_latency, supervision_tout);
            }
            p_lcb->conn_update_mask &= ~L2C_BLE_NOT_DEFAULT_PARAM;
            p_lcb->conn_update_mask |=  L2C_BLE_NEW_CONN_PARAM;
        }
    } else {
        /* application allows to do update, if we were delaying one do it now */
        if (p_lcb->conn_update_mask & L2C_BLE_NEW_CONN_PARAM) {
            /* if both side 4.1, or we are master device, send HCI command */
            if (p_lcb->link_role == HCI_ROLE_MASTER
#if (defined BLE_LLT_INCLUDED) && (BLE_LLT_INCLUDED == TRUE)
                    || (HCI_LE_CONN_PARAM_REQ_SUPPORTED(controller_get_interface()->get_features_ble()->as_array) &&
                        HCI_LE_CONN_PARAM_REQ_SUPPORTED(p_acl_cb->peer_le_features))
#endif
               ) {
                btsnd_hcic_ble_upd_ll_conn_params(p_lcb->handle, p_lcb->min_interval,
                                                  p_lcb->max_interval, p_lcb->latency, p_lcb->timeout, 0, 0);
                p_lcb->conn_update_mask |= L2C_BLE_UPDATE_PENDING;
            } else {
                l2cu_send_peer_ble_par_req (p_lcb, p_lcb->min_interval, p_lcb->max_interval,
                                            p_lcb->latency, p_lcb->timeout);
            }
            p_lcb->conn_update_mask &= ~L2C_BLE_NEW_CONN_PARAM;
            p_lcb->conn_update_mask |= L2C_BLE_NOT_DEFAULT_PARAM;
        }
    }
}

/*******************************************************************************
**
** Function         l2cble_process_conn_update_evt
**
** Description      This function enables the connection update request from remote
**                  after a successful connection update response is received.
**
** Returns          void
**
*******************************************************************************/
void l2cble_process_conn_update_evt (UINT16 handle, UINT8 status)
{
    tL2C_LCB *p_lcb;

    L2CAP_TRACE_DEBUG("l2cble_process_conn_update_evt");

    /* See if we have a link control block for the remote device */
    p_lcb = l2cu_find_lcb_by_handle(handle);
    if (!p_lcb) {
        L2CAP_TRACE_WARNING("l2cble_process_conn_update_evt: Invalid handle: %d", handle);
        return;
    }

    p_lcb->conn_update_mask &= ~L2C_BLE_UPDATE_PENDING;

    if (status != HCI_SUCCESS) {
        L2CAP_TRACE_WARNING("l2cble_process_conn_update_evt: Error status: %d", status);
    }

    l2cble_start_conn_update(p_lcb);

    L2CAP_TRACE_DEBUG("l2cble_process_conn_update_evt: conn_update_mask=%d", p_lcb->conn_update_mask);
}
/*******************************************************************************
**
** Function         l2cble_process_sig_cmd
**
** Description      This function is called when a signalling packet is received
**                  on the BLE signalling CID
**
** Returns          void
**
*******************************************************************************/
void l2cble_process_sig_cmd (tL2C_LCB *p_lcb, UINT8 *p, UINT16 pkt_len)
{
    UINT8           *p_pkt_end;
    UINT8           cmd_code, id;
    UINT16          cmd_len;
    UINT16          min_interval, max_interval, latency, timeout;

    p_pkt_end = p + pkt_len;

    STREAM_TO_UINT8  (cmd_code, p);
    STREAM_TO_UINT8  (id, p);
    STREAM_TO_UINT16 (cmd_len, p);

    /* Check command length does not exceed packet length */
    if ((p + cmd_len) > p_pkt_end) {
        L2CAP_TRACE_WARNING ("L2CAP - LE - format error, pkt_len: %d  cmd_len: %d  code: %d", pkt_len, cmd_len, cmd_code);
        return;
    }

    switch (cmd_code) {
    case L2CAP_CMD_REJECT:
    case L2CAP_CMD_ECHO_RSP:
    case L2CAP_CMD_INFO_RSP:
        p += 2;
        break;
    case L2CAP_CMD_ECHO_REQ:
    case L2CAP_CMD_INFO_REQ:
        l2cu_send_peer_cmd_reject (p_lcb, L2CAP_CMD_REJ_NOT_UNDERSTOOD, id, 0, 0);
        break;

    case L2CAP_CMD_BLE_UPDATE_REQ:
        STREAM_TO_UINT16 (min_interval, p); /* 0x0006 - 0x0C80 */
        STREAM_TO_UINT16 (max_interval, p); /* 0x0006 - 0x0C80 */
        STREAM_TO_UINT16 (latency, p);  /* 0x0000 - 0x03E8 */
        STREAM_TO_UINT16 (timeout, p);  /* 0x000A - 0x0C80 */
        /* If we are a master, the slave wants to update the parameters */
        if (p_lcb->link_role == HCI_ROLE_MASTER) {
            if (min_interval < BTM_BLE_CONN_INT_MIN_LIMIT) {
                min_interval = BTM_BLE_CONN_INT_MIN_LIMIT;
            }

            if (min_interval < BTM_BLE_CONN_INT_MIN || min_interval > BTM_BLE_CONN_INT_MAX ||
                    max_interval < BTM_BLE_CONN_INT_MIN || max_interval > BTM_BLE_CONN_INT_MAX ||
                    latency  > BTM_BLE_CONN_LATENCY_MAX ||
                    /*(timeout >= max_interval && latency > (timeout * 10/(max_interval * 1.25) - 1)) ||*/
                    timeout < BTM_BLE_CONN_SUP_TOUT_MIN || timeout > BTM_BLE_CONN_SUP_TOUT_MAX ||
                    max_interval < min_interval) {
                l2cu_send_peer_ble_par_rsp (p_lcb, L2CAP_CFG_UNACCEPTABLE_PARAMS, id);
            } else {

                l2cu_send_peer_ble_par_rsp (p_lcb, L2CAP_CFG_OK, id);

                p_lcb->min_interval = min_interval;
                p_lcb->max_interval = max_interval;
                p_lcb->latency = latency;
                p_lcb->timeout = timeout;
                p_lcb->conn_update_mask |= L2C_BLE_NEW_CONN_PARAM;

                l2cble_start_conn_update(p_lcb);
            }
        } else {
            l2cu_send_peer_cmd_reject (p_lcb, L2CAP_CMD_REJ_NOT_UNDERSTOOD, id, 0, 0);
        }
        break;

    case L2CAP_CMD_BLE_UPDATE_RSP:
        p += 2;
        break;

    default:
        L2CAP_TRACE_WARNING ("L2CAP - LE - unknown cmd code: %d", cmd_code);
        l2cu_send_peer_cmd_reject (p_lcb, L2CAP_CMD_REJ_NOT_UNDERSTOOD, id, 0, 0);
        return;
    }
}

/*******************************************************************************
**
** Function         l2cble_init_direct_conn
**
** Description      This function is to initate a direct connection
**
** Returns          TRUE connection initiated, FALSE otherwise.
**
*******************************************************************************/
BOOLEAN l2cble_init_direct_conn (tL2C_LCB *p_lcb)
{
    tBTM_SEC_DEV_REC *p_dev_rec = btm_find_or_alloc_dev (p_lcb->remote_bd_addr);
    tBTM_BLE_CB *p_cb = &btm_cb.ble_ctr_cb;
    UINT16 scan_int;
    UINT16 scan_win;
    BD_ADDR peer_addr;
    UINT8 peer_addr_type = BLE_ADDR_PUBLIC;
    UINT8 own_addr_type = BLE_ADDR_PUBLIC;

    /* There can be only one BLE connection request outstanding at a time */
    if (p_dev_rec == NULL) {
        L2CAP_TRACE_WARNING ("unknown device, can not initate connection");
        return (FALSE);
    }

    scan_int = (p_cb->scan_int == BTM_BLE_SCAN_PARAM_UNDEF) ? BTM_BLE_SCAN_FAST_INT : p_cb->scan_int;
    scan_win = (p_cb->scan_win == BTM_BLE_SCAN_PARAM_UNDEF) ? BTM_BLE_SCAN_FAST_WIN : p_cb->scan_win;

    peer_addr_type = p_lcb->ble_addr_type;
    memcpy(peer_addr, p_lcb->remote_bd_addr, BD_ADDR_LEN);

#if ( (defined BLE_PRIVACY_SPT) && (BLE_PRIVACY_SPT == TRUE))
    own_addr_type = btm_cb.ble_ctr_cb.privacy_mode ? BLE_ADDR_RANDOM : BLE_ADDR_PUBLIC;
    if (p_dev_rec->ble.in_controller_list & BTM_RESOLVING_LIST_BIT) {
        if (btm_cb.ble_ctr_cb.privacy_mode >=  BTM_PRIVACY_1_2) {
            own_addr_type |= BLE_ADDR_TYPE_ID_BIT;
        }

        btm_ble_enable_resolving_list(BTM_BLE_RL_INIT);
        btm_random_pseudo_to_identity_addr(peer_addr, &peer_addr_type);
    } else {
        btm_ble_disable_resolving_list(BTM_BLE_RL_INIT, TRUE);
    }
#endif

    if (!btm_ble_topology_check(BTM_BLE_STATE_INIT)) {
        l2cu_release_lcb (p_lcb);
        L2CAP_TRACE_ERROR("initate direct connection fail, topology limitation");
        return FALSE;
    }

    if (!btsnd_hcic_ble_create_ll_conn (scan_int,/* UINT16 scan_int      */
                                        scan_win, /* UINT16 scan_win      */
                                        FALSE,                   /* UINT8 white_list     */
                                        peer_addr_type,          /* UINT8 addr_type_peer */
                                        peer_addr,               /* BD_ADDR bda_peer     */
                                        own_addr_type,         /* UINT8 addr_type_own  */
                                        (UINT16) ((p_dev_rec->conn_params.min_conn_int != BTM_BLE_CONN_PARAM_UNDEF) ?
                                                p_dev_rec->conn_params.min_conn_int : BTM_BLE_CONN_INT_MIN_DEF),  /* UINT16 conn_int_min  */
                                        (UINT16) ((p_dev_rec->conn_params.max_conn_int != BTM_BLE_CONN_PARAM_UNDEF) ?
                                                p_dev_rec->conn_params.max_conn_int : BTM_BLE_CONN_INT_MAX_DEF),  /* UINT16 conn_int_max  */
                                        (UINT16) ((p_dev_rec->conn_params.slave_latency != BTM_BLE_CONN_PARAM_UNDEF) ?
                                                p_dev_rec->conn_params.slave_latency : BTM_BLE_CONN_SLAVE_LATENCY_DEF), /* UINT16 conn_latency  */
                                        (UINT16) ((p_dev_rec->conn_params.supervision_tout != BTM_BLE_CONN_PARAM_UNDEF) ?
                                                p_dev_rec->conn_params.supervision_tout : BTM_BLE_CONN_TIMEOUT_DEF), /* conn_timeout */
                                        0,                       /* UINT16 min_len       */
                                        0)) {                    /* UINT16 max_len       */
        l2cu_release_lcb (p_lcb);
        L2CAP_TRACE_ERROR("initate direct connection fail, no resources");
        return (FALSE);
    } else {
        p_lcb->link_state = LST_CONNECTING;
        l2cb.is_ble_connecting = TRUE;
        memcpy (l2cb.ble_connecting_bda, p_lcb->remote_bd_addr, BD_ADDR_LEN);
        btu_start_timer (&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK, L2CAP_BLE_LINK_CONNECT_TOUT);
        btm_ble_set_conn_st (BLE_DIR_CONN);

        return (TRUE);
    }
}

/*******************************************************************************
**
** Function         l2cble_create_conn
**
** Description      This function initiates an acl connection via HCI
**
** Returns          TRUE if successful, FALSE if connection not started.
**
*******************************************************************************/
BOOLEAN l2cble_create_conn (tL2C_LCB *p_lcb)
{
    tBTM_BLE_CONN_ST     conn_st = btm_ble_get_conn_st();
    BOOLEAN         rt = FALSE;

    /* There can be only one BLE connection request outstanding at a time */
    if (conn_st == BLE_CONN_IDLE) {
        rt = l2cble_init_direct_conn(p_lcb);
    } else {
        L2CAP_TRACE_WARNING ("L2CAP - LE - cannot start new connection at conn st: %d", conn_st);

        btm_ble_enqueue_direct_conn_req(p_lcb);

        if (conn_st == BLE_BG_CONN) {
            btm_ble_suspend_bg_conn();
        }

        rt = TRUE;
    }
    return rt;
}

/*******************************************************************************
**
** Function         l2c_link_processs_ble_num_bufs
**
** Description      This function is called when a "controller buffer size"
**                  event is first received from the controller. It updates
**                  the L2CAP values.
**
** Returns          void
**
*******************************************************************************/
void l2c_link_processs_ble_num_bufs (UINT16 num_lm_ble_bufs)
{
    if (num_lm_ble_bufs == 0) {
        num_lm_ble_bufs = L2C_DEF_NUM_BLE_BUF_SHARED;
        l2cb.num_lm_acl_bufs -= L2C_DEF_NUM_BLE_BUF_SHARED;
    }

    l2cb.num_lm_ble_bufs = l2cb.controller_le_xmit_window = num_lm_ble_bufs;
}

/*******************************************************************************
**
** Function         l2c_ble_link_adjust_allocation
**
** Description      This function is called when a link is created or removed
**                  to calculate the amount of packets each link may send to
**                  the HCI without an ack coming back.
**
**                  Currently, this is a simple allocation, dividing the
**                  number of Controller Packets by the number of links. In
**                  the future, QOS configuration should be examined.
**
** Returns          void
**
*******************************************************************************/
void l2c_ble_link_adjust_allocation (void)
{
    UINT16      qq, yy, qq_remainder;
    tL2C_LCB    *p_lcb;
    UINT16      hi_quota, low_quota;
    UINT16      num_lowpri_links = 0;
    UINT16      num_hipri_links  = 0;
    UINT16      controller_xmit_quota = l2cb.num_lm_ble_bufs;
    UINT16      high_pri_link_quota = L2CAP_HIGH_PRI_MIN_XMIT_QUOTA_A;

    /* If no links active, reset buffer quotas and controller buffers */
    if (l2cb.num_ble_links_active == 0) {
        l2cb.controller_le_xmit_window = l2cb.num_lm_ble_bufs;
        l2cb.ble_round_robin_quota = l2cb.ble_round_robin_unacked = 0;
        return;
    }

    /* First, count the links */
    for (yy = 0, p_lcb = &l2cb.lcb_pool[0]; yy < MAX_L2CAP_LINKS; yy++, p_lcb++) {
        if (p_lcb->in_use && p_lcb->transport == BT_TRANSPORT_LE) {
            if (p_lcb->acl_priority == L2CAP_PRIORITY_HIGH) {
                num_hipri_links++;
            } else {
                num_lowpri_links++;
            }
        }
    }

    /* now adjust high priority link quota */
    low_quota = num_lowpri_links ? 1 : 0;
    while ( (num_hipri_links * high_pri_link_quota + low_quota) > controller_xmit_quota ) {
        high_pri_link_quota--;
    }


    /* Work out the xmit quota and buffer quota high and low priorities */
    hi_quota  = num_hipri_links * high_pri_link_quota;
    low_quota = (hi_quota < controller_xmit_quota) ? controller_xmit_quota - hi_quota : 1;

    /* Work out and save the HCI xmit quota for each low priority link */

    /* If each low priority link cannot have at least one buffer */
    if (num_lowpri_links > low_quota) {
        l2cb.ble_round_robin_quota = low_quota;
        qq = qq_remainder = 0;
    }
    /* If each low priority link can have at least one buffer */
    else if (num_lowpri_links > 0) {
        l2cb.ble_round_robin_quota = 0;
        l2cb.ble_round_robin_unacked = 0;
        qq = low_quota / num_lowpri_links;
        qq_remainder = low_quota % num_lowpri_links;
    }
    /* If no low priority link */
    else {
        l2cb.ble_round_robin_quota = 0;
        l2cb.ble_round_robin_unacked = 0;
        qq = qq_remainder = 0;
    }
    L2CAP_TRACE_EVENT ("l2c_ble_link_adjust_allocation  num_hipri: %u  num_lowpri: %u  low_quota: %u  round_robin_quota: %u  qq: %u",
                       num_hipri_links, num_lowpri_links, low_quota,
                       l2cb.ble_round_robin_quota, qq);

    /* Now, assign the quotas to each link */
    for (yy = 0, p_lcb = &l2cb.lcb_pool[0]; yy < MAX_L2CAP_LINKS; yy++, p_lcb++) {
        if (p_lcb->in_use && p_lcb->transport == BT_TRANSPORT_LE) {
            if (p_lcb->acl_priority == L2CAP_PRIORITY_HIGH) {
                p_lcb->link_xmit_quota   = high_pri_link_quota;
            } else {
                /* Safety check in case we switched to round-robin with something outstanding */
                /* if sent_not_acked is added into round_robin_unacked then don't add it again */
                /* l2cap keeps updating sent_not_acked for exiting from round robin */
                if (( p_lcb->link_xmit_quota > 0 ) && ( qq == 0 )) {
                    l2cb.ble_round_robin_unacked += p_lcb->sent_not_acked;
                }

                p_lcb->link_xmit_quota   = qq;
                if (qq_remainder > 0) {
                    p_lcb->link_xmit_quota++;
                    qq_remainder--;
                }
            }

            L2CAP_TRACE_EVENT("l2c_ble_link_adjust_allocation LCB %d   Priority: %d  XmitQuota: %d",
                              yy, p_lcb->acl_priority, p_lcb->link_xmit_quota);

            L2CAP_TRACE_EVENT("        SentNotAcked: %d  RRUnacked: %d",
                              p_lcb->sent_not_acked, l2cb.round_robin_unacked);

            /* There is a special case where we have readjusted the link quotas and  */
            /* this link may have sent anything but some other link sent packets so  */
            /* so we may need a timer to kick off this link's transmissions.         */
            if ( (p_lcb->link_state == LST_CONNECTED)
                    && (!list_is_empty(p_lcb->link_xmit_data_q))
                    && (p_lcb->sent_not_acked < p_lcb->link_xmit_quota) ) {
                btu_start_timer (&p_lcb->timer_entry, BTU_TTYPE_L2CAP_LINK, L2CAP_LINK_FLOW_CONTROL_TOUT);
            }
        }
    }
}

#if (defined BLE_LLT_INCLUDED) && (BLE_LLT_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         l2cble_process_rc_param_request_evt
**
** Description      process LE Remote Connection Parameter Request Event.
**
** Returns          void
**
*******************************************************************************/
void l2cble_process_rc_param_request_evt(UINT16 handle, UINT16 int_min, UINT16 int_max,
        UINT16 latency, UINT16 timeout)
{
    tL2C_LCB    *p_lcb = l2cu_find_lcb_by_handle (handle);

    if (p_lcb != NULL) {
        p_lcb->min_interval = int_min;
        p_lcb->max_interval = int_max;
        p_lcb->latency = latency;
        p_lcb->timeout = timeout;

        /* if update is enabled, always accept connection parameter update */
        if ((p_lcb->conn_update_mask & L2C_BLE_CONN_UPDATE_DISABLE) == 0) {
            btsnd_hcic_ble_rc_param_req_reply(handle, int_min, int_max, latency, timeout, 0, 0);
        } else {
            L2CAP_TRACE_EVENT ("L2CAP - LE - update currently disabled");
            p_lcb->conn_update_mask |= L2C_BLE_NEW_CONN_PARAM;
            btsnd_hcic_ble_rc_param_req_neg_reply (handle, HCI_ERR_UNACCEPT_CONN_INTERVAL);
        }

    } else {
        L2CAP_TRACE_WARNING("No link to update connection parameter")
    }
}
#endif

/*******************************************************************************
**
** Function         l2cble_update_data_length
**
** Description      This function update link tx data length if applicable
**
** Returns          void
**
*******************************************************************************/
void l2cble_update_data_length(tL2C_LCB *p_lcb)
{
    UINT16 tx_mtu = 0;
    UINT16 i = 0;

    L2CAP_TRACE_DEBUG("%s", __FUNCTION__);

    /* See if we have a link control block for the connection */
    if (p_lcb == NULL) {
        return;
    }

    for (i = 0; i < L2CAP_NUM_FIXED_CHNLS; i++) {
        if (i + L2CAP_FIRST_FIXED_CHNL != L2CAP_BLE_SIGNALLING_CID) {
            if ((p_lcb->p_fixed_ccbs[i] != NULL) &&
                    (tx_mtu < (p_lcb->p_fixed_ccbs[i]->tx_data_len + L2CAP_PKT_OVERHEAD))) {
                tx_mtu = p_lcb->p_fixed_ccbs[i]->tx_data_len + L2CAP_PKT_OVERHEAD;
            }
        }
    }

    if (tx_mtu > BTM_BLE_DATA_SIZE_MAX) {
        tx_mtu = BTM_BLE_DATA_SIZE_MAX;
    }

    /* update TX data length if changed */
    if (p_lcb->tx_data_len != tx_mtu) {
        BTM_SetBleDataLength(p_lcb->remote_bd_addr, tx_mtu);
    }

}

/*******************************************************************************
**
** Function         l2cble_process_data_length_change_evt
**
** Description      This function process the data length change event
**
** Returns          void
**
*******************************************************************************/
void l2cble_process_data_length_change_event(UINT16 handle, UINT16 tx_data_len, UINT16 rx_data_len)
{
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_handle(handle);

    L2CAP_TRACE_DEBUG("%s TX data len = %d", __FUNCTION__, tx_data_len);
    if (p_lcb == NULL) {
        return;
    }

    if (tx_data_len > 0) {
        p_lcb->tx_data_len = tx_data_len;
    }

    /* ignore rx_data len for now */
}

/*******************************************************************************
**
** Function         l2cble_set_fixed_channel_tx_data_length
**
** Description      This function update max fixed channel tx data length if applicable
**
** Returns          void
**
*******************************************************************************/
void l2cble_set_fixed_channel_tx_data_length(BD_ADDR remote_bda, UINT16 fix_cid, UINT16 tx_mtu)
{
    tL2C_LCB *p_lcb = l2cu_find_lcb_by_bd_addr(remote_bda, BT_TRANSPORT_LE);
    UINT16 cid = fix_cid - L2CAP_FIRST_FIXED_CHNL;

    L2CAP_TRACE_DEBUG("%s TX MTU = %d", __FUNCTION__, tx_mtu);

    if (!controller_get_interface()->supports_ble_packet_extension()) {
        L2CAP_TRACE_WARNING("%s, request not supported", __FUNCTION__);
        return;
    }

    /* See if we have a link control block for the connection */
    if (p_lcb == NULL) {
        return;
    }

    if (p_lcb->p_fixed_ccbs[cid] != NULL) {
        if (tx_mtu > BTM_BLE_DATA_SIZE_MAX) {
            tx_mtu = BTM_BLE_DATA_SIZE_MAX;
        }

        p_lcb->p_fixed_ccbs[cid]->tx_data_len = tx_mtu;
    }

    l2cble_update_data_length(p_lcb);
}

#endif /* (BLE_INCLUDED == TRUE) */
