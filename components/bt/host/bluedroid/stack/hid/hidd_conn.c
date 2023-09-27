/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
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
 *  this file contains the connection interface functions
 *
 ******************************************************************************/
#include "btm_int.h"
#include "hid_conn.h"
#include "hid_int.h"
#include "osi/allocator.h"
#include "osi/osi.h"
#include "stack/btm_api.h"
#include "stack/btu.h"
#include "stack/hidd_api.h"
#include "stack/hiddefs.h"
#include "stack/l2c_api.h"
#include "stack/l2cdefs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if (HID_DEV_INCLUDED == TRUE)

static void hidd_l2cif_connect_ind(BD_ADDR bd_addr, uint16_t cid, uint16_t psm, uint8_t id);
static void hidd_l2cif_connect_cfm(uint16_t cid, uint16_t result);
static void hidd_l2cif_config_ind(uint16_t cid, tL2CAP_CFG_INFO *p_cfg);
static void hidd_l2cif_config_cfm(uint16_t cid, tL2CAP_CFG_INFO *p_cfg);
static void hidd_l2cif_disconnect_ind(uint16_t cid, bool ack_needed);
static void hidd_l2cif_disconnect_cfm(uint16_t cid, uint16_t result);
static void hidd_l2cif_data_ind(uint16_t cid, BT_HDR *p_msg);
static void hidd_l2cif_cong_ind(uint16_t cid, bool congested);

static const tL2CAP_APPL_INFO dev_reg_info = {hidd_l2cif_connect_ind,
                                              hidd_l2cif_connect_cfm,
                                              NULL,
                                              hidd_l2cif_config_ind,
                                              hidd_l2cif_config_cfm,
                                              hidd_l2cif_disconnect_ind,
                                              hidd_l2cif_disconnect_cfm,
                                              NULL,
                                              hidd_l2cif_data_ind,
                                              hidd_l2cif_cong_ind,
                                              NULL};
/*******************************************************************************
 *
 * Function         hidd_check_config_done
 *
 * Description      Checks if connection is configured and callback can be fired
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_check_config_done(void)
{
    tHID_CONN *p_hcon;
    p_hcon = &hd_cb.device.conn;
    if (((p_hcon->conn_flags & HID_CONN_FLAGS_ALL_CONFIGURED) == HID_CONN_FLAGS_ALL_CONFIGURED) &&
        (p_hcon->conn_state == HID_CONN_STATE_CONFIG)) {
        p_hcon->conn_state = HID_CONN_STATE_CONNECTED;
        hd_cb.device.state = HIDD_DEV_CONNECTED;
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_OPEN, 0, NULL);
        // send outstanding data on intr
        if (hd_cb.pending_data) {
            L2CA_DataWrite(p_hcon->intr_cid, hd_cb.pending_data);
            hd_cb.pending_data = NULL;
        }
    }
}
/*******************************************************************************
 *
 * Function         hidh_sec_check_complete_term
 *
 * Description      HID security check complete callback function.
 *
 * Returns          Send L2CA_ConnectRsp OK if secutiry check succeed; otherwise
 *                  send security block L2C connection response.
 *
 ******************************************************************************/
static void hidd_sec_check_complete(UNUSED_ATTR BD_ADDR bd_addr, UNUSED_ATTR tBT_TRANSPORT transport, void *p_ref_data,
                                    uint8_t res)
{
    tHID_DEV_DEV_CTB *p_dev = (tHID_DEV_DEV_CTB *)p_ref_data;
    if (res == BTM_SUCCESS && p_dev->conn.conn_state == HID_CONN_STATE_SECURITY) {
        p_dev->conn.disc_reason = HID_SUCCESS;
        p_dev->conn.conn_state = HID_CONN_STATE_CONNECTING_INTR;
        L2CA_ConnectRsp(p_dev->addr, p_dev->conn.ctrl_id, p_dev->conn.ctrl_cid, L2CAP_CONN_OK, L2CAP_CONN_OK);
        L2CA_ConfigReq(p_dev->conn.ctrl_cid, &hd_cb.l2cap_cfg);
    } else if (res != BTM_SUCCESS) {
        HIDD_TRACE_WARNING("%s: connection rejected by security", __func__);
        p_dev->conn.disc_reason = HID_ERR_AUTH_FAILED;
        p_dev->conn.conn_state = HID_CONN_STATE_UNUSED;
        L2CA_ConnectRsp(p_dev->addr, p_dev->conn.ctrl_id, p_dev->conn.ctrl_cid, L2CAP_CONN_SECURITY_BLOCK,
                        L2CAP_CONN_OK);
        return;
    }
}
/*******************************************************************************
 *
 * Function         hidd_sec_check_complete_orig
 *
 * Description      HID security check complete callback function (device
 *originated)
 *
 * Returns          void
 *
 ******************************************************************************/
void hidd_sec_check_complete_orig(UNUSED_ATTR BD_ADDR bd_addr, UNUSED_ATTR tBT_TRANSPORT transport, void *p_ref_data,
                                  uint8_t res)
{
    tHID_DEV_DEV_CTB *p_dev = (tHID_DEV_DEV_CTB *)p_ref_data;
    if (p_dev->conn.conn_state != HID_CONN_STATE_SECURITY) {
        HIDD_TRACE_WARNING("%s: invalid state (%02x)", __func__, p_dev->conn.conn_state);
        return;
    }
    if (res == BTM_SUCCESS) {
        HIDD_TRACE_EVENT("%s: security ok", __func__);
        p_dev->conn.disc_reason = HID_SUCCESS;
        p_dev->conn.conn_state = HID_CONN_STATE_CONFIG;
        L2CA_ConfigReq(p_dev->conn.ctrl_cid, &hd_cb.l2cap_cfg);
    } else {
        HIDD_TRACE_WARNING("%s: security check failed (%02x)", __func__, res);
        p_dev->conn.disc_reason = HID_ERR_AUTH_FAILED;
        hidd_conn_disconnect();
    }
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_connect_ind
 *
 * Description      Handles incoming L2CAP connection (we act as server)
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_connect_ind(BD_ADDR bd_addr, uint16_t cid, uint16_t psm, uint8_t id)
{
    tHID_CONN *p_hcon;
    tHID_DEV_DEV_CTB *p_dev;
    bool accept = TRUE; // accept by default
    HIDD_TRACE_EVENT("%s: psm=%04x cid=%04x id=%02x", __func__, psm, cid, id);
    p_dev = &hd_cb.device;
    if (!hd_cb.allow_incoming) {
        HIDD_TRACE_WARNING("%s: incoming connections not allowed, rejecting", __func__);
        L2CA_ConnectRsp(bd_addr, id, cid, L2CAP_CONN_NO_RESOURCES, 0);
        return;
    }
    if (p_dev->in_use && memcmp(bd_addr, p_dev->addr, sizeof(BD_ADDR))) {
        HIDD_TRACE_WARNING("%s: incoming connections from different device, rejecting", __func__);
        L2CA_ConnectRsp(bd_addr, id, cid, L2CAP_CONN_NO_RESOURCES, 0);
        return;
    } else if (!p_dev->in_use) {
        p_dev->in_use = TRUE;
        memcpy(p_dev->addr, bd_addr, sizeof(BD_ADDR));
        p_dev->state = HIDD_DEV_NO_CONN;
    }
    p_hcon = &hd_cb.device.conn;
    switch (psm) {
    case HID_PSM_INTERRUPT:
        if (p_hcon->ctrl_cid == 0) {
            accept = FALSE;
            HIDD_TRACE_WARNING("%s: incoming INTR without CTRL, rejecting", __func__);
        }
        if (p_hcon->conn_state != HID_CONN_STATE_CONNECTING_INTR) {
            accept = FALSE;
            HIDD_TRACE_WARNING("%s: incoming INTR in invalid state (%d), rejecting", __func__, p_hcon->conn_state);
        }
        break;
    case HID_PSM_CONTROL:
        if (p_hcon->conn_state != HID_CONN_STATE_UNUSED) {
            accept = FALSE;
            HIDD_TRACE_WARNING("%s: incoming CTRL in invalid state (%d), rejecting", __func__, p_hcon->conn_state);
        }
        break;
    default:
        accept = FALSE;
        HIDD_TRACE_ERROR("%s: received invalid PSM, rejecting", __func__);
        break;
    }
    if (!accept) {
        L2CA_ConnectRsp(bd_addr, id, cid, L2CAP_CONN_NO_RESOURCES, 0);
        return;
    }
    // for CTRL we need to go through security and we reply in callback from there
    if (psm == HID_PSM_CONTROL) {
        p_hcon->conn_flags = 0;
        p_hcon->ctrl_cid = cid;
        p_hcon->ctrl_id = id;
        p_hcon->disc_reason = HID_L2CAP_CONN_FAIL;
        p_hcon->conn_state = HID_CONN_STATE_SECURITY;
        if (btm_sec_mx_access_request(p_dev->addr, HID_PSM_CONTROL, FALSE, BTM_SEC_PROTO_HID, HIDD_NOSEC_CHN,
                                      &hidd_sec_check_complete, p_dev) == BTM_CMD_STARTED) {
            L2CA_ConnectRsp(bd_addr, id, cid, L2CAP_CONN_PENDING, L2CAP_CONN_OK);
        }
        return;
    }
    // for INTR we go directly to config state
    p_hcon->conn_state = HID_CONN_STATE_CONFIG;
    p_hcon->intr_cid = cid;
    L2CA_ConnectRsp(bd_addr, id, cid, L2CAP_CONN_OK, L2CAP_CONN_OK);
    L2CA_ConfigReq(cid, &hd_cb.l2cap_intr_cfg);
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_connect_cfm
 *
 * Description      Handles L2CAP connection response (we act as client)
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_connect_cfm(uint16_t cid, uint16_t result)
{
    tHID_DEV_DEV_CTB *p_dev = &hd_cb.device;
    tHID_CONN *p_hcon = &hd_cb.device.conn;
    HIDD_TRACE_EVENT("%s: cid=%04x result=%d, conn_state=%d", __func__, cid, result, p_hcon->conn_state);
    if (p_hcon->ctrl_cid != cid && p_hcon->intr_cid != cid) {
        HIDD_TRACE_WARNING("%s: unknown cid", __func__);
        return;
    }
    if (!(p_hcon->conn_flags & HID_CONN_FLAGS_IS_ORIG) ||
        ((cid == p_hcon->ctrl_cid) && (p_hcon->conn_state != HID_CONN_STATE_CONNECTING_CTRL && (p_hcon->conn_state != HID_CONN_STATE_DISCONNECTING_INTR))) ||
        ((cid == p_hcon->intr_cid) && (p_hcon->conn_state != HID_CONN_STATE_CONNECTING_INTR) && (p_hcon->conn_state != HID_CONN_STATE_DISCONNECTING_CTRL))) {
        HIDD_TRACE_WARNING("%s: unexpected, cid:0x%04x, ctrl_cid:0x%04x, intr_cid:0x%04x, conn_state:%d", __func__, cid,
                           p_hcon->ctrl_cid, p_hcon->intr_cid, p_hcon->conn_state);
        return;
    }
    if (result != L2CAP_CONN_OK) {
        HIDD_TRACE_WARNING("%s: connection failed, now disconnect", __func__);
        if (cid == p_hcon->ctrl_cid)
            p_hcon->ctrl_cid = 0;
        else
            p_hcon->intr_cid = 0;
        hidd_conn_disconnect();
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_CLOSE, HID_L2CAP_CONN_FAIL | (uint32_t)result, NULL);
        return;
    }
    /* CTRL connect conf */
    if (cid == p_hcon->ctrl_cid) {
        p_hcon->conn_state = HID_CONN_STATE_SECURITY;
        p_hcon->disc_reason = HID_L2CAP_CONN_FAIL; /* in case disconnected before sec completed */
        btm_sec_mx_access_request(p_dev->addr, HID_PSM_CONTROL, TRUE, BTM_SEC_PROTO_HID, HIDD_SEC_CHN,
                                  &hidd_sec_check_complete_orig, p_dev);
    } else {
        p_hcon->conn_state = HID_CONN_STATE_CONFIG;
        L2CA_ConfigReq(cid, &hd_cb.l2cap_intr_cfg);
    }
    return;
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_config_ind
 *
 * Description      Handles incoming L2CAP configuration request
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_config_ind(uint16_t cid, tL2CAP_CFG_INFO *p_cfg)
{
    tHID_CONN *p_hcon;
    HIDD_TRACE_EVENT("%s: cid=%04x", __func__, cid);
    p_hcon = &hd_cb.device.conn;
    if (p_hcon->ctrl_cid != cid && p_hcon->intr_cid != cid) {
        HIDD_TRACE_WARNING("%s: unknown cid", __func__);
        return;
    }
    if ((!p_cfg->mtu_present) || (p_cfg->mtu > HID_DEV_MTU_SIZE))
        p_hcon->rem_mtu_size = HID_DEV_MTU_SIZE;
    else
        p_hcon->rem_mtu_size = p_cfg->mtu;
    // accept without changes
    p_cfg->flush_to_present = FALSE;
    p_cfg->mtu_present = FALSE;
    p_cfg->result = L2CAP_CFG_OK;
    if (cid == p_hcon->intr_cid && hd_cb.use_in_qos && !p_cfg->qos_present) {
        p_cfg->qos_present = TRUE;
        memcpy(&p_cfg->qos, &hd_cb.in_qos, sizeof(FLOW_SPEC));
    }
    L2CA_ConfigRsp(cid, p_cfg);
    // update flags
    if (cid == p_hcon->ctrl_cid) {
        p_hcon->conn_flags |= HID_CONN_FLAGS_HIS_CTRL_CFG_DONE;
        if ((p_hcon->conn_flags & HID_CONN_FLAGS_IS_ORIG) && (p_hcon->conn_flags & HID_CONN_FLAGS_MY_CTRL_CFG_DONE)) {
            p_hcon->disc_reason = HID_L2CAP_CONN_FAIL;
            if ((p_hcon->intr_cid = L2CA_ConnectReq(HID_PSM_INTERRUPT, hd_cb.device.addr)) == 0) {
                p_hcon->conn_state = HID_CONN_STATE_UNUSED;
                hidd_conn_disconnect();
                HIDD_TRACE_WARNING("%s: could not start L2CAP connection for INTR", __func__);
                hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_CLOSE, HID_ERR_L2CAP_FAILED, NULL);
                return;
            } else {
                p_hcon->conn_state = HID_CONN_STATE_CONNECTING_INTR;
            }
        }
    } else {
        p_hcon->conn_flags |= HID_CONN_FLAGS_HIS_INTR_CFG_DONE;
    }
    hidd_check_config_done();
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_config_cfm
 *
 * Description      Handles incoming L2CAP configuration response
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_config_cfm(uint16_t cid, tL2CAP_CFG_INFO *p_cfg)
{
    tHID_CONN *p_hcon;
    uint32_t reason;
    HIDD_TRACE_EVENT("%s: cid=%04x pcfg->result=%d", __func__, cid, p_cfg->result);
    p_hcon = &hd_cb.device.conn;
    if (p_hcon->ctrl_cid != cid && p_hcon->intr_cid != cid) {
        HIDD_TRACE_WARNING("%s: unknown cid", __func__);
        return;
    }
    if (p_hcon->intr_cid == cid && p_cfg->result == L2CAP_CFG_UNACCEPTABLE_PARAMS && p_cfg->qos_present) {
        tL2CAP_CFG_INFO new_qos;
        // QoS parameters not accepted for intr, try again with host proposal
        memcpy(&new_qos, &hd_cb.l2cap_intr_cfg, sizeof(new_qos));
        memcpy(&new_qos.qos, &p_cfg->qos, sizeof(FLOW_SPEC));
        new_qos.qos_present = TRUE;
        HIDD_TRACE_WARNING("%s: config failed, retry", __func__);
        L2CA_ConfigReq(cid, &new_qos);
        return;
    } else if (p_hcon->intr_cid == cid && p_cfg->result == L2CAP_CFG_UNKNOWN_OPTIONS) {
        // QoS not understood by remote device, try configuring without QoS
        HIDD_TRACE_WARNING("%s: config failed, retry without QoS", __func__);
        L2CA_ConfigReq(cid, &hd_cb.l2cap_cfg);
        return;
    } else if (p_cfg->result != L2CAP_CFG_OK) {
        HIDD_TRACE_WARNING("%s: config failed, disconnecting", __func__);
        hidd_conn_disconnect();
        reason = HID_L2CAP_CFG_FAIL | (uint32_t)p_cfg->result;
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_CLOSE, reason, NULL);
        return;
    }
    // update flags
    if (cid == p_hcon->ctrl_cid) {
        p_hcon->conn_flags |= HID_CONN_FLAGS_MY_CTRL_CFG_DONE;
        if ((p_hcon->conn_flags & HID_CONN_FLAGS_IS_ORIG) && (p_hcon->conn_flags & HID_CONN_FLAGS_HIS_CTRL_CFG_DONE)) {
            p_hcon->disc_reason = HID_L2CAP_CONN_FAIL;
            if ((p_hcon->intr_cid = L2CA_ConnectReq(HID_PSM_INTERRUPT, hd_cb.device.addr)) == 0) {
                p_hcon->conn_state = HID_CONN_STATE_UNUSED;
                hidd_conn_disconnect();
                HIDD_TRACE_WARNING("%s: could not start L2CAP connection for INTR", __func__);
                hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_CLOSE, HID_ERR_L2CAP_FAILED, NULL);
                return;
            } else {
                p_hcon->conn_state = HID_CONN_STATE_CONNECTING_INTR;
            }
        }
    } else {
        p_hcon->conn_flags |= HID_CONN_FLAGS_MY_INTR_CFG_DONE;
    }
    hidd_check_config_done();
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_disconnect_ind
 *
 * Description      Handler incoming L2CAP disconnection request
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_disconnect_ind(uint16_t cid, bool ack_needed)
{
    tHID_CONN *p_hcon;
    HIDD_TRACE_EVENT("%s: cid=%04x ack_needed=%d", __func__, cid, ack_needed);
    p_hcon = &hd_cb.device.conn;
    if (p_hcon->conn_state == HID_CONN_STATE_UNUSED || (p_hcon->ctrl_cid != cid && p_hcon->intr_cid != cid)) {
        HIDD_TRACE_WARNING("%s: unknown cid", __func__);
        return;
    }
    if (ack_needed)
        L2CA_DisconnectRsp(cid);
    if (cid == p_hcon->ctrl_cid) {
        p_hcon->ctrl_cid = 0;
        p_hcon->conn_state = HID_CONN_STATE_DISCONNECTING_CTRL;
    } else {
        p_hcon->intr_cid = 0;
        p_hcon->conn_state = HID_CONN_STATE_DISCONNECTING_INTR;
    }
    if ((p_hcon->ctrl_cid == 0) && (p_hcon->intr_cid == 0)) {
        HIDD_TRACE_EVENT("%s: INTR and CTRL disconnected", __func__);
        // clean any outstanding data on intr
        if (hd_cb.pending_data) {
            osi_free(hd_cb.pending_data);
            hd_cb.pending_data = NULL;
        }
        hd_cb.device.state = HIDD_DEV_NO_CONN;
        p_hcon->conn_state = HID_CONN_STATE_UNUSED;
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_CLOSE, p_hcon->disc_reason, NULL);
    }
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_disconnect_cfm
 *
 * Description      Handles L2CAP disconection response
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_disconnect_cfm(uint16_t cid, uint16_t result)
{
    tHID_CONN *p_hcon;
    HIDD_TRACE_EVENT("%s: cid=%04x result=%d", __func__, cid, result);
    p_hcon = &hd_cb.device.conn;
    if (p_hcon->conn_state == HID_CONN_STATE_UNUSED || (p_hcon->ctrl_cid != cid && p_hcon->intr_cid != cid)) {
        HIDD_TRACE_WARNING("%s: unknown cid", __func__);
        return;
    }
    if (cid == p_hcon->ctrl_cid) {
        p_hcon->ctrl_cid = 0;
    } else {
        p_hcon->intr_cid = 0;
        // now disconnect CTRL
        L2CA_DisconnectReq(p_hcon->ctrl_cid);
    }
    if ((p_hcon->ctrl_cid == 0) && (p_hcon->intr_cid == 0)) {
        HIDD_TRACE_EVENT("%s: INTR and CTRL disconnected", __func__);
        hd_cb.device.state = HIDD_DEV_NO_CONN;
        p_hcon->conn_state = HID_CONN_STATE_UNUSED;
        if (hd_cb.pending_vc_unplug) {
            hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_VC_UNPLUG, p_hcon->disc_reason, NULL);
            hd_cb.pending_vc_unplug = FALSE;
        } else {
            hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_CLOSE, p_hcon->disc_reason, NULL);
        }
    }
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_cong_ind
 *
 * Description      Handles L2CAP congestion status event
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_cong_ind(uint16_t cid, bool congested)
{
    tHID_CONN *p_hcon;
    HIDD_TRACE_EVENT("%s: cid=%04x congested=%d", __func__, cid, congested);
    p_hcon = &hd_cb.device.conn;
    if (p_hcon->conn_state == HID_CONN_STATE_UNUSED || (p_hcon->ctrl_cid != cid && p_hcon->intr_cid != cid)) {
        HIDD_TRACE_WARNING("%s: unknown cid", __func__);
        return;
    }
    if (congested) {
        p_hcon->conn_flags |= HID_CONN_FLAGS_CONGESTED;
    } else {
        p_hcon->conn_flags &= ~HID_CONN_FLAGS_CONGESTED;
    }
}
/*******************************************************************************
 *
 * Function         hidd_l2cif_data_ind
 *
 * Description      Handler incoming data on L2CAP channel
 *
 * Returns          void
 *
 ******************************************************************************/
static void hidd_l2cif_data_ind(uint16_t cid, BT_HDR *p_msg)
{
    tHID_CONN *p_hcon;
    uint8_t *p_data = (uint8_t *)(p_msg + 1) + p_msg->offset;
    uint8_t msg_type, param;
    bool err = FALSE;
    HIDD_TRACE_EVENT("%s: cid=%04x", __func__, cid);
    p_hcon = &hd_cb.device.conn;
    if (p_hcon->conn_state == HID_CONN_STATE_UNUSED || (p_hcon->ctrl_cid != cid && p_hcon->intr_cid != cid)) {
        HIDD_TRACE_WARNING("%s: unknown cid", __func__);
        osi_free(p_msg);
        return;
    }
    msg_type = HID_GET_TRANS_FROM_HDR(*p_data);
    param = HID_GET_PARAM_FROM_HDR(*p_data);
    if (msg_type == HID_TRANS_DATA && cid == p_hcon->intr_cid) {
        // skip HID header
        p_msg->offset++;
        p_msg->len--;
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_INTR_DATA, 0, p_msg);
        return;
    }
    switch (msg_type) {
    case HID_TRANS_GET_REPORT:
        // at this stage we don't know if Report Id shall be included in request
        // so we pass complete packet in callback and let other code analyze this
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_GET_REPORT, !!(param & HID_PAR_GET_REP_BUFSIZE_FOLLOWS), p_msg);
        break;
    case HID_TRANS_SET_REPORT:
        // as above
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_SET_REPORT, 0, p_msg);
        break;
    case HID_TRANS_GET_IDLE:
        hidd_conn_send_data(HID_CHANNEL_CTRL, HID_TRANS_DATA, HID_PAR_REP_TYPE_OTHER, hd_cb.device.idle_time, 0, NULL);
        osi_free(p_msg);
        break;
    case HID_TRANS_SET_IDLE:
        if (p_msg->len != 2) {
            HIDD_TRACE_ERROR("%s: invalid len (%d) set idle request received", __func__, p_msg->len);
            err = TRUE;
        } else {
            hd_cb.device.idle_time = p_data[1];
            HIDD_TRACE_DEBUG("%s: idle_time = %d", __func__, hd_cb.device.idle_time);
            if (hd_cb.device.idle_time) {
                HIDD_TRACE_WARNING("%s: idle_time of %d ms not supported by HID Device", __func__,
                                   (hd_cb.device.idle_time * 4));
                err = TRUE;
            }
        }
        if (!err) {
            hidd_conn_send_data(0, HID_TRANS_HANDSHAKE, HID_PAR_HANDSHAKE_RSP_SUCCESS, 0, 0, NULL);
        } else {
            hidd_conn_send_data(0, HID_TRANS_HANDSHAKE, HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM, 0, 0, NULL);
        }
        osi_free(p_msg);
        break;
    case HID_TRANS_GET_PROTOCOL:
        hidd_conn_send_data(HID_CHANNEL_CTRL, HID_TRANS_DATA, HID_PAR_REP_TYPE_OTHER, !hd_cb.device.boot_mode, 0, NULL);
        osi_free(p_msg);
        break;
    case HID_TRANS_SET_PROTOCOL:
        hd_cb.device.boot_mode = !(param & HID_PAR_PROTOCOL_MASK);
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_SET_PROTOCOL, param & HID_PAR_PROTOCOL_MASK, NULL);
        hidd_conn_send_data(0, HID_TRANS_HANDSHAKE, HID_PAR_HANDSHAKE_RSP_SUCCESS, 0, 0, NULL);
        osi_free(p_msg);
        break;
    case HID_TRANS_CONTROL:
        switch (param) {
        case HID_PAR_CONTROL_SUSPEND:
            hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_SUSPEND, 0, NULL);
            break;
        case HID_PAR_CONTROL_EXIT_SUSPEND:
            hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_EXIT_SUSPEND, 0, NULL);
            break;
        case HID_PAR_CONTROL_VIRTUAL_CABLE_UNPLUG:
            hidd_conn_disconnect();
            // set flag so we can notify properly when disconnected
            hd_cb.pending_vc_unplug = TRUE;
            break;
        }
        osi_free(p_msg);
        break;
    case HID_TRANS_DATA:
    default:
        HIDD_TRACE_WARNING("%s: got unsupported msg (%d)", __func__, msg_type);
        hidd_conn_send_data(0, HID_TRANS_HANDSHAKE, HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ, 0, 0, NULL);
        osi_free(p_msg);
        break;
    }
}
/*******************************************************************************
 *
 * Function         hidd_conn_reg
 *
 * Description      Registers L2CAP channels
 *
 * Returns          void
 *
 ******************************************************************************/
tHID_STATUS hidd_conn_reg(void)
{
    HIDD_TRACE_API("%s", __func__);
    memset(&hd_cb.l2cap_cfg, 0, sizeof(tL2CAP_CFG_INFO));
    hd_cb.l2cap_cfg.mtu_present = TRUE;
    hd_cb.l2cap_cfg.mtu = HID_DEV_MTU_SIZE;
    hd_cb.l2cap_cfg.flush_to_present = TRUE;
    hd_cb.l2cap_cfg.flush_to = HID_DEV_FLUSH_TO;
    memset(&hd_cb.l2cap_intr_cfg, 0, sizeof(tL2CAP_CFG_INFO));
    hd_cb.l2cap_intr_cfg.mtu_present = TRUE;
    hd_cb.l2cap_intr_cfg.mtu = HID_DEV_MTU_SIZE;
    hd_cb.l2cap_intr_cfg.flush_to_present = TRUE;
    hd_cb.l2cap_intr_cfg.flush_to = HID_DEV_FLUSH_TO;
    if (!L2CA_Register(HID_PSM_CONTROL, (tL2CAP_APPL_INFO *)&dev_reg_info)) {
        HIDD_TRACE_ERROR("HID Control (device) registration failed");
        return (HID_ERR_L2CAP_FAILED);
    }
    if (!L2CA_Register(HID_PSM_INTERRUPT, (tL2CAP_APPL_INFO *)&dev_reg_info)) {
        L2CA_Deregister(HID_PSM_CONTROL);
        HIDD_TRACE_ERROR("HID Interrupt (device) registration failed");
        return (HID_ERR_L2CAP_FAILED);
    }
    return (HID_SUCCESS);
}
/*******************************************************************************
 *
 * Function         hidd_conn_dereg
 *
 * Description      Deregisters L2CAP channels
 *
 * Returns          void
 *
 ******************************************************************************/
void hidd_conn_dereg(void)
{
    HIDD_TRACE_API("%s", __func__);
    L2CA_Deregister(HID_PSM_CONTROL);
    L2CA_Deregister(HID_PSM_INTERRUPT);
}
/*******************************************************************************
 *
 * Function         hidd_conn_initiate
 *
 * Description      Initiates HID connection to plugged device
 *
 * Returns          HID_SUCCESS
 *
 ******************************************************************************/
tHID_STATUS hidd_conn_initiate(void)
{
    tHID_DEV_DEV_CTB *p_dev = &hd_cb.device;
    HIDD_TRACE_API("%s", __func__);
    if (!p_dev->in_use) {
        HIDD_TRACE_WARNING("%s: no virtual cable established", __func__);
        return (HID_ERR_NOT_REGISTERED);
    }
    if (p_dev->conn.conn_state != HID_CONN_STATE_UNUSED) {
        HIDD_TRACE_WARNING("%s: connection already in progress", __func__);
        return (HID_ERR_CONN_IN_PROCESS);
    }
    p_dev->conn.ctrl_cid = 0;
    p_dev->conn.intr_cid = 0;
    p_dev->conn.disc_reason = HID_L2CAP_CONN_FAIL;
    p_dev->conn.conn_flags = HID_CONN_FLAGS_IS_ORIG;
    BTM_SetOutService(p_dev->addr, BTM_SEC_SERVICE_HIDD_SEC_CTRL, HIDD_SEC_CHN);
    /* Check if L2CAP started the connection process */
    if ((p_dev->conn.ctrl_cid = L2CA_ConnectReq(HID_PSM_CONTROL, p_dev->addr)) == 0) {
        HIDD_TRACE_WARNING("%s: could not start L2CAP connection", __func__);
        hd_cb.callback(hd_cb.device.addr, HID_DHOST_EVT_CLOSE, HID_ERR_L2CAP_FAILED, NULL);
    } else {
        p_dev->conn.conn_state = HID_CONN_STATE_CONNECTING_CTRL;
    }
    return (HID_SUCCESS);
}
/*******************************************************************************
 *
 * Function         hidd_conn_disconnect
 *
 * Description      Disconnects existing HID connection
 *
 * Returns          HID_SUCCESS
 *
 ******************************************************************************/
tHID_STATUS hidd_conn_disconnect(void)
{
    tHID_CONN *p_hcon;
    HIDD_TRACE_API("%s", __func__);
    // clean any outstanding data on intr
    if (hd_cb.pending_data) {
        osi_free(hd_cb.pending_data);
        hd_cb.pending_data = NULL;
    }
    p_hcon = &hd_cb.device.conn;
    if ((p_hcon->ctrl_cid != 0) || (p_hcon->intr_cid != 0)) {
        /* Set l2cap idle timeout to 0 (so ACL link is disconnected
         * immediately after last channel is closed) */
        L2CA_SetIdleTimeoutByBdAddr(hd_cb.device.addr, 0, BT_TRANSPORT_BR_EDR);
        if (p_hcon->intr_cid) {
            p_hcon->conn_state = HID_CONN_STATE_DISCONNECTING_INTR;
            L2CA_DisconnectReq(p_hcon->intr_cid);
        } else if (p_hcon->ctrl_cid) {
            p_hcon->conn_state = HID_CONN_STATE_DISCONNECTING_CTRL;
            L2CA_DisconnectReq(p_hcon->ctrl_cid);
        }
    } else {
        HIDD_TRACE_WARNING("%s: already disconnected", __func__);
        p_hcon->conn_state = HID_CONN_STATE_UNUSED;
    }
    return (HID_SUCCESS);
}
/*******************************************************************************
 *
 * Function         hidd_conn_send_data
 *
 * Description      Sends data to host
 *
 * Returns          tHID_STATUS
 *
 ******************************************************************************/
tHID_STATUS hidd_conn_send_data(uint8_t channel, uint8_t msg_type, uint8_t param, uint8_t data, uint16_t len,
                                uint8_t *p_data)
{
    tHID_CONN *p_hcon;
    BT_HDR *p_buf;
    uint8_t *p_out;
    uint16_t cid;
    uint16_t buf_size;

    HIDD_TRACE_VERBOSE("%s: channel(%d), msg_type(%d), len(%d)", __func__, channel, msg_type, len);

    p_hcon = &hd_cb.device.conn;
    if (p_hcon->conn_flags & HID_CONN_FLAGS_CONGESTED) {
        return HID_ERR_CONGESTED;
    }

    switch (msg_type) {
    case HID_TRANS_HANDSHAKE:
    case HID_TRANS_CONTROL:
        cid = p_hcon->ctrl_cid;
        buf_size = HID_CONTROL_BUF_SIZE;
        break;
    case HID_TRANS_DATA:
        if (channel == HID_CHANNEL_CTRL) {
            cid = p_hcon->ctrl_cid;
            buf_size = HID_CONTROL_BUF_SIZE;
        } else {
            cid = p_hcon->intr_cid;
            buf_size = HID_INTERRUPT_BUF_SIZE;
        }
        break;
    default:
        return (HID_ERR_INVALID_PARAM);
    }
    p_buf = (BT_HDR *)osi_malloc(buf_size);
    if (p_buf == NULL)
        return (HID_ERR_NO_RESOURCES);
    p_buf->offset = L2CAP_MIN_OFFSET;
    p_out = (uint8_t *)(p_buf + 1) + p_buf->offset;
    *p_out = HID_BUILD_HDR(msg_type, param);
    p_out++;
    p_buf->len = 1; // start with header only
    // add report id prefix only if non-zero (which is reserved)
    if (msg_type == HID_TRANS_DATA && (data || param == HID_PAR_REP_TYPE_OTHER)) {
        *p_out = data; // report_id
        p_out++;
        p_buf->len++;
    }
    if (len > 0 && p_data != NULL) {
        memcpy(p_out, p_data, len);
        p_buf->len += len;
    }
    // check if connected
    if (hd_cb.device.state != HIDD_DEV_CONNECTED) {
        // for DATA on intr we hold transfer and try to reconnect
        if (msg_type == HID_TRANS_DATA && cid == p_hcon->intr_cid) {
            // drop previous data, we do not queue it for now
            if (hd_cb.pending_data) {
                osi_free(hd_cb.pending_data);
            }
            hd_cb.pending_data = p_buf;
            if (hd_cb.device.conn.conn_state == HID_CONN_STATE_UNUSED) {
                HIDD_TRACE_WARNING("%s: try to reconnect!", __func__);
                return hidd_conn_initiate();
            }
            return HID_SUCCESS;
        }
        return HID_ERR_NO_CONNECTION;
    }
#ifdef REPORT_TRANSFER_TIMESTAMP
    if (report_transfer) {
        HIDD_TRACE_ERROR("%s: report sent", __func__);
    }
#endif
    HIDD_TRACE_VERBOSE("%s: report sent", __func__);
    if (L2CA_DataWrite(cid, p_buf) == L2CAP_DW_FAILED)
        return (HID_ERR_CONGESTED);
    return (HID_SUCCESS);
}

#endif
