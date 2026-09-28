/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goeps_api.h"
#include "goeps_int.h"

#if (GOEPS_INCLUDED == TRUE)

#if GOEP_DYNAMIC_MEMORY == FALSE
tGOEPS_CB goeps_cb;
#else
tGOEPS_CB *goeps_cb_ptr = NULL;
#endif

tGOEPS_SCB *goeps_allocate_scb(void)
{
    tGOEPS_SCB *p_scb = NULL;
    for (int i = 0; i < GOEPS_MAX_SERVER; ++i) {
        if (!goeps_cb.scb[i].allocated) {
            goeps_cb.scb[i].allocated = i + 1;
            p_scb = &goeps_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

void goeps_free_scb(tGOEPS_SCB *p_scb)
{
    memset(p_scb, 0, sizeof(tGOEPS_SCB));
}

tGOEPS_CCB *goeps_allocate_ccb(UINT8 scb_idx)
{
    tGOEPS_CCB *p_ccb = NULL;
    for (int i = 0; i < GOEPS_MAX_CONNECTION; ++i) {
        if (!goeps_cb.ccb[i].allocated) {
            goeps_cb.ccb[i].allocated = i + 1;
            goeps_cb.ccb[i].scb_idx = scb_idx;
            goeps_cb.ccb[i].state = GOEPS_STATE_IDLE;
            p_ccb = &goeps_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

void goeps_free_ccb(tGOEPS_CCB *p_ccb)
{
    if (p_ccb->pkt != NULL) {
        osi_free(p_ccb->pkt);
    }
    memset(p_ccb, 0, sizeof(tGOEPS_CCB));
}

BOOLEAN goeps_check_obex_rsp_param(tOBEX_PARSE_INFO *info)
{
    if (info->response_code == 0) {
        return FALSE;
    }
    if (info->opcode == OBEX_OPCODE_CONNECT) {
        if (info->max_packet_length < OBEX_PACKET_LENGTH_MIN || info->obex_version_number == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

static tGOEPS_SCB *goeps_find_scb_by_obex_svr_handle(UINT16 svr_handle)
{
    UINT16 scb_idx = (svr_handle >> 8) - 1;
    if (scb_idx >= GOEPS_MAX_SERVER || !goeps_cb.scb[scb_idx].allocated) {
        return NULL;
    }
    if (goeps_cb.scb[scb_idx].obex_svr_handle != svr_handle) {
        return NULL;
    }
    return &goeps_cb.scb[scb_idx];
}

static tGOEPS_CCB *goeps_find_ccb_by_obex_handle(UINT16 obex_handle)
{
    tGOEPS_CCB *p_ccb = NULL;
    for (int i = 0; i < GOEPS_MAX_CONNECTION; ++i) {
        if (goeps_cb.ccb[i].allocated && goeps_cb.ccb[i].obex_handle == obex_handle) {
            p_ccb = &goeps_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static tGOEPS_EVT_CBACK *goeps_get_callback(tGOEPS_CCB *p_ccb)
{
    UINT8 scb_idx = p_ccb->scb_idx;
    if (scb_idx == 0 || scb_idx > GOEPS_MAX_SERVER) {
        return NULL;
    }
    return goeps_cb.scb[scb_idx - 1].callback;
}

static void goeps_extra_srm_req(BT_HDR *pkt, BOOLEAN *srm_en, BOOLEAN *srm_wait)
{
    tOBEX_PARSE_INFO info;
    BOOLEAN srm_found = FALSE;
    BOOLEAN srmp_found = FALSE;

    if (OBEX_ParseRequest(pkt, &info) != OBEX_SUCCESS) {
        return;
    }

    UINT8 *header = NULL;
    while ((header = OBEX_GetNextHeader(pkt, &info)) != NULL) {
        switch (*header) {
        case OBEX_HEADER_ID_SRM:
            if (header[1] == OBEX_SRM_ENABLE) {
                *srm_en = TRUE;
            }
            srm_found = TRUE;
            break;
        case OBEX_HEADER_ID_SRM_PARAM:
            switch (header[1]) {
            case OBEX_SRMP_WAIT:
                *srm_wait = TRUE;
                break;
            default:
                break;
            }
            srmp_found = TRUE;
            break;
        default:
            break;
        }
        if (srm_found && srmp_found) {
            break;
        }
    }
}

static void goeps_act_congest(tGOEPS_CCB *p_ccb)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    if (p_cback == NULL) {
        return;
    }
    p_ccb->congest = TRUE;
    p_cback(p_ccb->allocated, GOEPS_CONGEST_EVT, NULL);
}

static void goeps_act_uncongest(tGOEPS_CCB *p_ccb)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    if (p_cback == NULL) {
        return;
    }
    p_ccb->congest = FALSE;
    p_cback(p_ccb->allocated, GOEPS_UNCONGEST_EVT, NULL);
}

static void goeps_act_mtu_chg(tGOEPS_CCB *p_ccb, tGOEPS_MTU_CHG *mtu_chg)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    tGOEPS_MSG msg;

    if (p_cback == NULL) {
        return;
    }
    msg.mtu_changed.peer_mtu = mtu_chg->peer_mtu;
    msg.mtu_changed.our_mtu = mtu_chg->our_mtu;
    p_ccb->peer_mtu = mtu_chg->peer_mtu;
    p_ccb->our_mtu = mtu_chg->our_mtu;
    p_cback(p_ccb->allocated, GOEPS_MTU_CHANGED_EVT, &msg);
}

void goeps_obex_callback(UINT16 handle, UINT8 event, tOBEX_MSG *msg)
{
    tGOEPS_DATA data;
    UINT8 goeps_sm_event = GOEPS_SM_EVENT_DISCONNECT;
    BOOLEAN exec_sm = FALSE;
    tGOEPS_CCB *p_ccb = goeps_find_ccb_by_obex_handle(handle);
    tGOEPS_SCB *p_scb = NULL;

    switch (event) {
    case OBEX_CONN_INCOME_EVT:
        p_scb = goeps_find_scb_by_obex_svr_handle(msg->conn_income.svr_handle);
        if (p_scb == NULL) {
            GOEPS_TRACE_ERROR("goeps_obex_callback unknown server handle\n");
            OBEX_RemoveConn(handle);
            return;
        }
        p_ccb = goeps_allocate_ccb(p_scb->allocated);
        if (p_ccb == NULL) {
            GOEPS_TRACE_ERROR("goeps_obex_callback no ccb resource\n");
            OBEX_RemoveConn(handle);
            return;
        }
        p_ccb->obex_handle = handle;
        p_ccb->peer_mtu = msg->conn_income.peer_mtu;
        p_ccb->our_mtu = msg->conn_income.our_mtu;
        p_ccb->state = GOEPS_STATE_CONN_PENDING;

        data.conn_income.svr_handle = p_scb->obex_svr_handle;
        data.conn_income.peer_mtu = msg->conn_income.peer_mtu;
        data.conn_income.our_mtu = msg->conn_income.our_mtu;
        bdcpy(data.conn_income.addr, msg->conn_income.addr);
        goeps_sm_event = GOEPS_SM_EVENT_CONN_INCOME;
        exec_sm = TRUE;
        break;
    case OBEX_MTU_CHANGE_EVT:
        if (p_ccb == NULL) {
            break;
        }
        data.mtu_chg.peer_mtu = msg->mtu_change.peer_mtu;
        data.mtu_chg.our_mtu = msg->mtu_change.our_mtu;
        goeps_act_mtu_chg(p_ccb, &data.mtu_chg);
        break;
    case OBEX_DISCONNECT_EVT:
        if (p_ccb == NULL) {
            break;
        }
        p_ccb->obex_handle = 0;
        goeps_sm_event = GOEPS_SM_EVENT_DISCONNECT;
        exec_sm = TRUE;
        break;
    case OBEX_CONGEST_EVT:
        if (p_ccb != NULL) {
            goeps_act_congest(p_ccb);
        }
        break;
    case OBEX_UNCONGEST_EVT:
        if (p_ccb != NULL) {
            goeps_act_uncongest(p_ccb);
        }
        break;
    case OBEX_DATA_EVT:
        if (p_ccb == NULL) {
            GOEPS_TRACE_ERROR("goeps_obex_callback can not find a ccb\n");
            if (msg->data.pkt) {
                osi_free(msg->data.pkt);
            }
            OBEX_RemoveConn(handle);
            return;
        }
        data.pkt = msg->data.pkt;
        if (OBEX_CheckFinalBit(data.pkt)) {
            goeps_sm_event = GOEPS_SM_EVENT_REQ_FB;
        } else {
            goeps_sm_event = GOEPS_SM_EVENT_REQ;
        }
        exec_sm = TRUE;
        break;
    default:
        break;
    }

    if (exec_sm) {
        goeps_sm_execute(p_ccb, goeps_sm_event, &data);
    }
}

static void goeps_sm_act_conn_income(tGOEPS_CCB *p_ccb, tGOEPS_CONN_INCOME *conn_income)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    tGOEPS_MSG msg;

    if (p_cback == NULL) {
        return;
    }
    msg.conn_income.svr_handle = conn_income->svr_handle;
    msg.conn_income.peer_mtu = conn_income->peer_mtu;
    msg.conn_income.our_mtu = conn_income->our_mtu;
    bdcpy(msg.conn_income.addr, conn_income->addr);
    p_cback(p_ccb->allocated, GOEPS_CONN_INCOME_EVT, &msg);
}

static void goeps_sm_act_disconnect(tGOEPS_CCB *p_ccb)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    tGOEPS_MSG msg;

    if (p_ccb->obex_handle) {
        OBEX_RemoveConn(p_ccb->obex_handle);
    }

    if (p_cback != NULL) {
        msg.closed.reason = GOEP_TL_ERROR;
        p_cback(p_ccb->allocated, GOEPS_CLOSED_EVT, &msg);
    }
    goeps_free_ccb(p_ccb);
}

static void goeps_sm_act_req(tGOEPS_CCB *p_ccb, BT_HDR *pkt)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    tGOEPS_MSG msg;
    tOBEX_PARSE_INFO info;
    BOOLEAN srm_en = FALSE;
    BOOLEAN srm_wait = FALSE;

    if (p_cback == NULL) {
        osi_free(pkt);
        return;
    }

    if (OBEX_ParseRequest(pkt, &info) == OBEX_SUCCESS) {
        p_ccb->last_req_opcode = info.opcode;
        goeps_extra_srm_req(pkt, &srm_en, &srm_wait);
        goeps_srm_sm_execute(p_ccb, FALSE, srm_en, srm_wait);
    }

    msg.request.opcode = p_ccb->last_req_opcode;
    msg.request.final = FALSE;
    msg.request.srm_en = (p_ccb->srm_state == GOEPS_SRM_STATE_ENABLE_WAIT ||
                          p_ccb->srm_state == GOEPS_SRM_STATE_ENABLE);
    msg.request.srm_wait = (p_ccb->srm_state == GOEPS_SRM_STATE_ENABLE_WAIT);
    msg.request.pkt = pkt;
    p_cback(p_ccb->allocated, GOEPS_REQUEST_EVT, &msg);
}

static void goeps_sm_act_req_fb(tGOEPS_CCB *p_ccb, BT_HDR *pkt)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    tGOEPS_MSG msg;
    tOBEX_PARSE_INFO info;
    BOOLEAN srm_en = FALSE;
    BOOLEAN srm_wait = FALSE;

    if (p_cback == NULL) {
        osi_free(pkt);
        return;
    }

    if (OBEX_ParseRequest(pkt, &info) == OBEX_SUCCESS) {
        p_ccb->last_req_opcode = info.opcode;
        goeps_extra_srm_req(pkt, &srm_en, &srm_wait);
        goeps_srm_sm_execute(p_ccb, FALSE, srm_en, srm_wait);
    }

    msg.request.opcode = p_ccb->last_req_opcode;
    msg.request.final = TRUE;
    msg.request.srm_en = FALSE;
    msg.request.srm_wait = FALSE;
    msg.request.pkt = pkt;
    p_cback(p_ccb->allocated, GOEPS_REQUEST_EVT, &msg);
}

static void goeps_sm_act_send_rsp(tGOEPS_CCB *p_ccb, BT_HDR *pkt)
{
    UINT16 ret = OBEX_SendPacket(p_ccb->obex_handle, pkt);
    if (ret == OBEX_SUCCESS) {
        p_ccb->state = GOEPS_STATE_OPENED_RSP;
    } else {
        goeps_sm_act_disconnect(p_ccb);
    }
}

static void goeps_sm_act_send_rsp_fb(tGOEPS_CCB *p_ccb, BT_HDR *pkt)
{
    tGOEPS_EVT_CBACK *p_cback = goeps_get_callback(p_ccb);
    tGOEPS_MSG msg;
    UINT16 ret = OBEX_SendPacket(p_ccb->obex_handle, pkt);
    BOOLEAN was_connect = (p_ccb->last_req_opcode == OBEX_OPCODE_CONNECT);

    if (ret != OBEX_SUCCESS) {
        goeps_sm_act_disconnect(p_ccb);
        return;
    }

    p_ccb->srm_state = GOEPS_SRM_STATE_IDLE;
    p_ccb->state = GOEPS_STATE_OPENED_IDLE;

    if (was_connect && p_cback != NULL) {
        msg.opened.peer_mtu = p_ccb->peer_mtu;
        msg.opened.our_mtu = p_ccb->our_mtu;
        p_cback(p_ccb->allocated, GOEPS_OPENED_EVT, &msg);
    }
}

static void goeps_sm_state_conn_pending(tGOEPS_CCB *p_ccb, UINT8 event, tGOEPS_DATA *p_data)
{
    switch (event) {
    case GOEPS_SM_EVENT_CONN_INCOME:
        goeps_sm_act_conn_income(p_ccb, &p_data->conn_income);
        break;
    case GOEPS_SM_EVENT_DISCONNECT:
        goeps_sm_act_disconnect(p_ccb);
        break;
    case GOEPS_SM_EVENT_REQ:
        goeps_sm_act_req(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_REQ_FB:
        goeps_sm_act_req_fb(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_RSP:
        goeps_sm_act_send_rsp(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_RSP_FB:
        goeps_sm_act_send_rsp_fb(p_ccb, p_data->pkt);
        break;
    default:
        GOEPS_TRACE_ERROR("goeps_sm_state_conn_pending unexpected event: 0x%x\n", event);
        if (p_data->pkt != NULL) {
            osi_free(p_data->pkt);
            p_data->pkt = NULL;
        }
        break;
    }
}

static void goeps_sm_state_opened_idle(tGOEPS_CCB *p_ccb, UINT8 event, tGOEPS_DATA *p_data)
{
    switch (event) {
    case GOEPS_SM_EVENT_DISCONNECT:
        goeps_sm_act_disconnect(p_ccb);
        break;
    case GOEPS_SM_EVENT_REQ:
        goeps_sm_act_req(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_REQ_FB:
        goeps_sm_act_req_fb(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_RSP:
        goeps_sm_act_send_rsp(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_RSP_FB:
        goeps_sm_act_send_rsp_fb(p_ccb, p_data->pkt);
        break;
    default:
        GOEPS_TRACE_ERROR("goeps_sm_state_opened_idle unexpected event: 0x%x\n", event);
        if (p_data->pkt != NULL) {
            osi_free(p_data->pkt);
            p_data->pkt = NULL;
        }
        break;
    }
}

static void goeps_sm_state_opened_rsp(tGOEPS_CCB *p_ccb, UINT8 event, tGOEPS_DATA *p_data)
{
    switch (event) {
    case GOEPS_SM_EVENT_DISCONNECT:
        goeps_sm_act_disconnect(p_ccb);
        break;
    case GOEPS_SM_EVENT_REQ:
        goeps_sm_act_req(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_REQ_FB:
        goeps_sm_act_req_fb(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_RSP:
        goeps_sm_act_send_rsp(p_ccb, p_data->pkt);
        break;
    case GOEPS_SM_EVENT_RSP_FB:
        goeps_sm_act_send_rsp_fb(p_ccb, p_data->pkt);
        break;
    default:
        GOEPS_TRACE_ERROR("goeps_sm_state_opened_rsp unexpected event: 0x%x\n", event);
        if (p_data->pkt != NULL) {
            osi_free(p_data->pkt);
            p_data->pkt = NULL;
        }
        break;
    }
}

BOOLEAN goeps_check_obex_rsp_allow(UINT8 state, BOOLEAN final)
{
    BOOLEAN ret = FALSE;
    if (final) {
        switch (state) {
        case GOEPS_STATE_CONN_PENDING:
        case GOEPS_STATE_OPENED_IDLE:
        case GOEPS_STATE_OPENED_RSP:
            ret = TRUE;
            break;
        default:
            break;
        }
    } else {
        switch (state) {
        case GOEPS_STATE_CONN_PENDING:
        case GOEPS_STATE_OPENED_IDLE:
        case GOEPS_STATE_OPENED_RSP:
            ret = TRUE;
            break;
        default:
            break;
        }
    }
    return ret;
}

void goeps_sm_execute(tGOEPS_CCB *p_ccb, UINT8 event, tGOEPS_DATA *p_data)
{
    bool free_pkt = false;
    bool has_pkt = false;

    switch (p_ccb->state) {
    case GOEPS_STATE_CONN_PENDING:
        goeps_sm_state_conn_pending(p_ccb, event, p_data);
        break;
    case GOEPS_STATE_OPENED_IDLE:
        goeps_sm_state_opened_idle(p_ccb, event, p_data);
        break;
    case GOEPS_STATE_OPENED_RSP:
        goeps_sm_state_opened_rsp(p_ccb, event, p_data);
        break;
    default:
        free_pkt = true;
        GOEPS_TRACE_ERROR("goeps_sm_execute unexpected state: 0x%x\n", p_ccb->state);
        break;
    }

    switch (event) {
    case GOEPS_SM_EVENT_REQ:
    case GOEPS_SM_EVENT_REQ_FB:
    case GOEPS_SM_EVENT_RSP:
    case GOEPS_SM_EVENT_RSP_FB:
        has_pkt = true;
        break;
    default:
        has_pkt = false;
        break;
    }

    if (has_pkt && free_pkt) {
        if (p_data->pkt) {
            osi_free(p_data->pkt);
            p_data->pkt = NULL;
        }
    }
}

static void goeps_srm_sm_act_rsp(tGOEPS_CCB *p_ccb, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    switch (p_ccb->srm_state) {
    case GOEPS_SRM_STATE_IDLE:
        if (srm_en) {
            p_ccb->srm_state = GOEPS_SRM_STATE_REQ;
            p_ccb->srm_wait = srm_wait;
        } else {
            p_ccb->srm_state = GOEPS_SRM_STATE_DISABLE;
        }
        break;
    case GOEPS_SRM_STATE_ENABLE_WAIT:
        if (!srm_wait) {
            p_ccb->srm_wait = FALSE;
        }
        if (!p_ccb->srm_wait && !p_ccb->srm_peer_wait) {
            p_ccb->srm_state = GOEPS_SRM_STATE_ENABLE;
        }
        break;
    default:
        break;
    }
}

static void goeps_srm_sm_act_req(tGOEPS_CCB *p_ccb, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    switch (p_ccb->srm_state) {
    case GOEPS_SRM_STATE_IDLE:
        break;
    case GOEPS_SRM_STATE_REQ:
        if (srm_en) {
            p_ccb->srm_peer_wait = srm_wait;
            if (p_ccb->srm_wait || p_ccb->srm_peer_wait) {
                p_ccb->srm_state = GOEPS_SRM_STATE_ENABLE_WAIT;
            } else {
                p_ccb->srm_state = GOEPS_SRM_STATE_ENABLE;
            }
        } else {
            p_ccb->srm_state = GOEPS_SRM_STATE_DISABLE;
        }
        break;
    case GOEPS_SRM_STATE_ENABLE_WAIT:
        if (!srm_wait) {
            p_ccb->srm_peer_wait = FALSE;
        }
        if (!p_ccb->srm_wait && !p_ccb->srm_peer_wait) {
            p_ccb->srm_state = GOEPS_SRM_STATE_ENABLE;
        }
        break;
    default:
        break;
    }
}

void goeps_srm_sm_execute(tGOEPS_CCB *p_ccb, BOOLEAN is_rsp, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    if (is_rsp) {
        goeps_srm_sm_act_rsp(p_ccb, srm_en, srm_wait);
    } else {
        goeps_srm_sm_act_req(p_ccb, srm_en, srm_wait);
    }
}

#endif /* #if (GOEPS_INCLUDED == TRUE) */
