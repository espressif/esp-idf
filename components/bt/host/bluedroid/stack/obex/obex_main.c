/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "obex_int.h"
#include "obex_tl.h"

#if (OBEX_INCLUDED == TRUE)

#if OBEX_DYNAMIC_MEMORY == FALSE
tOBEX_CB obex_cb;
#else
tOBEX_CB *obex_cb_ptr = NULL;
#endif

static tOBEX_CCB *obex_find_ccb_by_tl_hdl(UINT8 tl, UINT16 tl_hdl)
{
    tOBEX_CCB *p_ccb = NULL;
    for (int i = 0; i < OBEX_MAX_CONNECTION; ++i) {
        if (obex_cb.ccb[i].allocated && obex_cb.ccb[i].tl == tl && obex_cb.ccb[i].tl_hdl == tl_hdl) {
            p_ccb = &obex_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static tOBEX_SCB *obex_find_scb_by_tl_hdl(UINT8 tl, UINT16 tl_hdl)
{
    tOBEX_SCB *p_scb = NULL;
    for (int i = 0; i < OBEX_MAX_SERVER; ++i) {
        if (obex_cb.scb[i].allocated && obex_cb.scb[i].tl == tl && obex_cb.scb[i].tl_hdl == tl_hdl) {
            p_scb = &obex_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

tOBEX_CCB *obex_allocate_ccb(void)
{
    tOBEX_CCB *p_ccb = NULL;
    for (int i = 0; i < OBEX_MAX_CONNECTION; ++i) {
        if (!obex_cb.ccb[i].allocated) {
            obex_cb.ccb[i].allocated = i + 1;
            p_ccb = &obex_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

void obex_free_ccb(tOBEX_CCB *p_ccb)
{
    assert(p_ccb->allocated != 0);
    memset(p_ccb, 0, sizeof(tOBEX_CCB));
}

tOBEX_SCB *obex_allocate_scb(void)
{
    tOBEX_SCB *p_scb = NULL;
    for (int i = 0; i < OBEX_MAX_SERVER; ++i) {
        if (!obex_cb.scb[i].allocated) {
            obex_cb.scb[i].allocated = i + 1;
            p_scb = &obex_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

void obex_free_scb(tOBEX_SCB *p_scb)
{
    assert(p_scb->allocated != 0);
    memset(p_scb, 0, sizeof(tOBEX_SCB));
}

/* check whether connection mtu is valid, if not, disconnect it */
static bool check_conn_mtu_valid(tOBEX_CCB *p_ccb, BOOLEAN call_cb)
{
    if (p_ccb->tl_our_mtu < 255 || p_ccb->tl_peer_mtu < 255) {
        if (call_cb && p_ccb->callback) {
            p_ccb->callback(p_ccb->allocated, OBEX_DISCONNECT_EVT, NULL);
        }
        OBEX_TRACE_ERROR("Check OBEX transport layer MTU failed, disconnect");
        obex_cb.tl_ops[p_ccb->tl]->disconnect(p_ccb->tl_hdl);
        obex_free_ccb(p_ccb);
        return false;
    }
    return true;
}

void obex_tl_evt_handler(UINT8 tl, tOBEX_TL_EVT evt, tOBEX_TL_MSG *msg)
{
    UINT16 tl_hdl = msg->any.hdl;
    tOBEX_CCB *p_ccb = obex_find_ccb_by_tl_hdl(tl, tl_hdl);
    tOBEX_SCB *p_scb = NULL;
    tOBEX_MSG cb_msg = {0};

    switch (evt)
    {
    case OBEX_TL_CONN_OPEN_EVT:
        assert(p_ccb != NULL);
        p_ccb->tl_peer_mtu = msg->conn_open.peer_mtu;
        p_ccb->tl_our_mtu = msg->conn_open.our_mtu;
        if (!check_conn_mtu_valid(p_ccb, TRUE)) {
            break;
        }
        p_ccb->state = OBEX_STATE_OPENED;
        if (p_ccb->callback) {
            cb_msg.connect.peer_mtu = msg->conn_open.peer_mtu;
            cb_msg.connect.our_mtu = msg->conn_open.our_mtu;
            p_ccb->callback(p_ccb->allocated, OBEX_CONNECT_EVT, &cb_msg);
        }
        break;
    case OBEX_TL_DIS_CONN_EVT:
        if (p_ccb != NULL) {
            if (p_ccb->callback) {
                p_ccb->callback(p_ccb->allocated, OBEX_DISCONNECT_EVT, NULL);
            }
            obex_free_ccb(p_ccb);
        }
        break;
    case OBEX_TL_CONGEST_EVT:
        assert(p_ccb != NULL);
        if (p_ccb->callback) {
            p_ccb->callback(p_ccb->allocated, OBEX_CONGEST_EVT, NULL);
        }
        break;
    case OBEX_TL_UNCONGEST_EVT:
        assert(p_ccb != NULL);
        if (p_ccb->callback) {
            p_ccb->callback(p_ccb->allocated, OBEX_UNCONGEST_EVT, NULL);
        }
        break;
    case OBEX_TL_MTU_CHANGE_EVT:
        assert(p_ccb != NULL);
        p_ccb->tl_peer_mtu = msg->mtu_chg.peer_mtu;
        p_ccb->tl_our_mtu = msg->mtu_chg.our_mtu;
        if (!check_conn_mtu_valid(p_ccb, TRUE)) {
            break;
        }
        if (p_ccb->callback) {
            cb_msg.mtu_change.peer_mtu = msg->mtu_chg.peer_mtu;
            cb_msg.mtu_change.our_mtu = msg->mtu_chg.our_mtu;
            p_ccb->callback(p_ccb->allocated, OBEX_MTU_CHANGE_EVT, &cb_msg);
        }
        break;
    case OBEX_TL_DATA_EVT:
        assert(p_ccb != NULL);
        if (p_ccb->callback) {
            cb_msg.data.pkt = msg->data.p_buf;
            p_ccb->callback(p_ccb->allocated, OBEX_DATA_EVT, &cb_msg);
        }
        else {
            /* No callback, just free the packet */
            osi_free(msg->data.p_buf);
        }
        break;
    case OBEX_TL_CONN_INCOME_EVT:
        /* New connection, p_ccb should be NULL */
        assert(p_ccb == NULL);
        p_scb = obex_find_scb_by_tl_hdl(tl, msg->conn_income.svr_hdl);
        if (p_scb == NULL) {
            obex_cb.tl_ops[tl]->disconnect(tl_hdl);
            break;
        }

        if ((p_ccb = obex_allocate_ccb()) == NULL) {
            obex_cb.tl_ops[tl]->disconnect(tl_hdl);
            break;
        }

        p_ccb->tl = tl;
        p_ccb->tl_hdl = tl_hdl;
        p_ccb->role = OBEX_ROLE_SERVER;
        p_ccb->tl_peer_mtu = msg->conn_income.peer_mtu;
        p_ccb->tl_our_mtu = msg->conn_income.our_mtu;
        if (!check_conn_mtu_valid(p_ccb, FALSE)) {
            break;
        }
        p_ccb->state = OBEX_STATE_OPENED;
        p_ccb->callback = p_scb->callback;
        if (p_ccb->callback) {
            cb_msg.conn_income.svr_handle = p_scb->allocated << 8;
            cb_msg.conn_income.peer_mtu = msg->conn_income.peer_mtu;
            cb_msg.conn_income.our_mtu = msg->conn_income.our_mtu;
            p_ccb->callback(p_ccb->allocated, OBEX_CONN_INCOME_EVT, &cb_msg);
        }
        break;
    default:
        OBEX_TRACE_ERROR("Unknown OBEX transport event: 0x%x\n", evt);
        break;
    }
}

void obex_tl_l2cap_callback(tOBEX_TL_EVT evt, tOBEX_TL_MSG *msg)
{
    obex_tl_evt_handler(OBEX_OVER_L2CAP, evt, msg);
}

#endif /* #if (OBEX_INCLUDED == TRUE) */
