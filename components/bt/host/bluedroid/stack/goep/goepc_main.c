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
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#include "goep_int.h"

#if (GOEPC_INCLUDED == TRUE)

#if GOEP_DYNAMIC_MEMORY == FALSE
tGOEPC_CB goepc_cb;
#else
tGOEPC_CB *goepc_cb_ptr = NULL;
#endif

tGOEPC_CCB *goepc_allocate_ccb(void)
{
    tGOEPC_CCB *p_ccb = NULL;
    for (int i = 0; i < GOEPC_MAX_CONNECTION; ++i) {
        if (!goepc_cb.ccb[i].allocated) {
            goepc_cb.ccb[i].allocated = i + 1;
            p_ccb = &goepc_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

void goepc_free_ccb(tGOEPC_CCB *p_ccb)
{
    if (p_ccb->pkt != NULL) {
        osi_free(p_ccb->pkt);
    }
    memset(p_ccb, 0, sizeof(tGOEPC_CCB));
}

BOOLEAN goepc_check_obex_req_param(tOBEX_PARSE_INFO *info)
{
    BOOLEAN ret = TRUE;
    switch (info->opcode)
    {
    case OBEX_OPCODE_CONNECT:
        if (info->max_packet_length < 255 || info->obex_version_number == 0) {
            ret = FALSE;
        }
        break;
    case OBEX_OPCODE_DISCONNECT:
    case OBEX_OPCODE_PUT:
    case OBEX_OPCODE_PUT_FINAL:
    case OBEX_OPCODE_GET:
    case OBEX_OPCODE_GET_FINAL:
    case OBEX_OPCODE_SETPATH:
    case OBEX_OPCODE_ACTION:
    case OBEX_OPCODE_SESSION:
        /* opcode allowed */
        break;
    case OBEX_OPCODE_ABORT:
    default:
        ret = FALSE;
        /* opcode not allowed */
        break;
    }

    return ret;
}

static tGOEPC_CCB *find_ccb_by_obex_handle(UINT16 obex_handle)
{
    tGOEPC_CCB *p_ccb = NULL;
    for (int i = 0; i < GOEPC_MAX_CONNECTION; ++i) {
        if (goepc_cb.ccb[i].allocated && goepc_cb.ccb[i].obex_handle == obex_handle) {
            p_ccb = &goepc_cb.ccb[i];
        }
    }
    return p_ccb;
}

static void goepc_extra_srm_rsp(UINT8 opcode, BT_HDR *pkt, BOOLEAN *srm_en, BOOLEAN *srm_wait)
{
    tOBEX_PARSE_INFO info;
    BOOLEAN srm_found = FALSE;
    BOOLEAN srmp_found = FALSE;
    if (OBEX_ParseResponse(pkt, opcode, &info) == OBEX_SUCCESS) {
        UINT8 *header = NULL;
        while((header = OBEX_GetNextHeader(pkt, &info)) != NULL) {
            switch (*header)
            {
            case OBEX_HEADER_ID_SRM:
                if (header[1] == OBEX_SRM_ENABLE) {
                    *srm_en = TRUE;
                }
                srm_found = TRUE;
                break;
            case OBEX_HEADER_ID_SRM_PARAM:
                switch (header[1])
                {
                case OBEX_SRMP_ADD_PKT:
                    /* goep should not use this */
                    break;
                case OBEX_SRMP_WAIT:
                    *srm_wait = TRUE;
                    break;
                case OBEX_SRMP_ADD_PKT_WAIT:
                    /* goep should not use this */
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
}

static void goepc_act_congest(tGOEPC_CCB *p_ccb)
{
    p_ccb->congest = TRUE;
    p_ccb->callback(p_ccb->allocated, GOEPC_CONGEST_EVT, NULL);
}

static void goepc_act_uncongest(tGOEPC_CCB *p_ccb)
{
    p_ccb->congest = FALSE;
    p_ccb->callback(p_ccb->allocated, GOEPC_UNCONGEST_EVT, NULL);
}

static void goepc_act_mtu_chg(tGOEPC_CCB *p_ccb, tGOEPC_MTU_CHG *mtu_chg)
{
    tGOEPC_MSG msg;
    msg.mtu_changed.peer_mtu = mtu_chg->peer_mtu;
    msg.mtu_changed.our_mtu = mtu_chg->our_mtu;
    p_ccb->peer_mtu = mtu_chg->peer_mtu;
    p_ccb->our_mtu = mtu_chg->our_mtu;
    p_ccb->callback(p_ccb->allocated, GOEPC_MTU_CHANGED_EVT, &msg);
}

void goepc_obex_callback(UINT16 handle, UINT8 event, tOBEX_MSG *msg)
{
    tGOEPC_DATA data;
    UINT8 goepc_sm_event = GOEPC_SM_EVENT_DISCONNECT;
    BOOLEAN exec_sm = FALSE;
    tGOEPC_CCB *p_ccb = find_ccb_by_obex_handle(handle);
    if (p_ccb == NULL) {
        GOEPC_TRACE_ERROR("goepc_obex_callback can not find a ccb\n");
        /* can not find a ccb in goepc, free resource and remove this connection */
        if (event == OBEX_DATA_EVT && msg->data.pkt) {
            osi_free(msg->data.pkt);
        }
        OBEX_RemoveConn(handle);
        return;
    }

    switch (event)
    {
    case OBEX_CONNECT_EVT:
        data.connected.peer_mtu = msg->connect.peer_mtu;
        data.connected.our_mtu = msg->connect.our_mtu;
        goepc_sm_event = GOEPC_SM_EVENT_CONNECT;
        exec_sm = TRUE;
        break;
    case OBEX_MTU_CHANGE_EVT:
        data.mtu_chg.peer_mtu = msg->mtu_change.peer_mtu;
        data.mtu_chg.our_mtu = msg->mtu_change.our_mtu;
        goepc_act_mtu_chg(p_ccb, &data.mtu_chg);
        break;
    case OBEX_DISCONNECT_EVT:
        /* when we received this event, obex connection already disconnect */
        p_ccb->obex_handle = 0;
        goepc_sm_event = GOEPC_SM_EVENT_DISCONNECT;;
        exec_sm = TRUE;
        break;
    case OBEX_CONGEST_EVT:
        goepc_act_congest(p_ccb);
        break;
    case OBEX_UNCONGEST_EVT:
        goepc_act_uncongest(p_ccb);
        break;
    case OBEX_DATA_EVT:
        data.pkt = msg->data.pkt;
        if (OBEX_CheckContinueResponse(data.pkt)) {
            /* in OBEX 1.0, final bit of response code will always set, we need to check this */
            goepc_sm_event = GOEPC_SM_EVENT_RSP;
        }
        else if (OBEX_CheckFinalBit(data.pkt)) {
            goepc_sm_event = GOEPC_SM_EVENT_RSP_FB;
        }
        else {
            goepc_sm_event = GOEPC_SM_EVENT_RSP;
        }
        exec_sm = TRUE;
        break;
    default:
        /* other event, ignore */
        break;
    }

    if (exec_sm) {
        goepc_sm_execute(p_ccb, goepc_sm_event, &data);
    }
}

static void goepc_sm_act_connect(tGOEPC_CCB *p_ccb, tGOEPC_CONNECTED *connected)
{
    tGOEPC_MSG msg;
    msg.opened.peer_mtu = connected->peer_mtu;
    msg.opened.our_mtu = connected->our_mtu;
    p_ccb->peer_mtu = connected->peer_mtu;
    p_ccb->our_mtu = connected->our_mtu;

    /* main state machine transfer to OPENED_IDLE */
    p_ccb->state = GOEPC_STATE_OPENED_IDLE;
    p_ccb->callback(p_ccb->allocated, GOEPC_OPENED_EVT, &msg);
}

static void goepc_sm_act_disconnect(tGOEPC_CCB *p_ccb)
{
    tGOEPC_MSG msg;
    if (p_ccb->obex_handle) {
        OBEX_RemoveConn(p_ccb->obex_handle);
    }
    msg.closed.reason = GOEP_TL_ERROR;
    p_ccb->callback(p_ccb->allocated, GOEPC_CLOSED_EVT, &msg);
    /* free ccb, main state machine end */
    goepc_free_ccb(p_ccb);
}

static void goepc_sm_act_send_req(tGOEPC_CCB *p_ccb, BT_HDR *pkt)
{
    UINT16 ret = OBEX_SendPacket(p_ccb->obex_handle, pkt);
    if (ret == OBEX_SUCCESS) {
        /* main state machine transfer to OPENED_REQ */
        p_ccb->state = GOEPC_STATE_OPENED_REQ;
    }
    else {
        /* send failed, something error in transport layer, disconnect */
        goepc_sm_act_disconnect(p_ccb);
    }
}

static void goepc_sm_act_send_req_fb(tGOEPC_CCB *p_ccb, BT_HDR *pkt)
{
    UINT16 ret = OBEX_SendPacket(p_ccb->obex_handle, pkt);
    if (ret == OBEX_SUCCESS) {
        /* main state machine transfer to OPENED_RSP */
        p_ccb->state = GOEPC_STATE_OPENED_RSP;
    }
    else {
        /* send failed, something error in transport layer, disconnect */
        goepc_sm_act_disconnect(p_ccb);
    }
}

static void goepc_sm_act_rsp(tGOEPC_CCB *p_ccb, BT_HDR *pkt)
{
    /* handle srm state transfer */
    BOOLEAN srm_en = FALSE;
    BOOLEAN srm_wait = FALSE;
    goepc_extra_srm_rsp(p_ccb->last_pkt_opcode, pkt, &srm_en, &srm_wait);
    goepc_srm_sm_execute(p_ccb, FALSE, srm_en, srm_wait);
     /* main state machine not change */

    tGOEPC_MSG msg;
    msg.response.opcode = p_ccb->last_pkt_opcode;
    msg.response.final = FALSE;
    msg.response.srm_en = (p_ccb->srm_state == GOEPC_SRM_STATE_ENABLE_WAIT || p_ccb->srm_state == GOEPC_SRM_STATE_ENABLE);
    msg.response.srm_wait = (p_ccb->srm_state == GOEPC_SRM_STATE_ENABLE_WAIT);
    msg.response.pkt = pkt;
    p_ccb->callback(p_ccb->allocated, GOEPC_RESPONSE_EVT, &msg);
}

static void goepc_sm_act_rsp_fb(tGOEPC_CCB *p_ccb, BT_HDR *pkt)
{
    tGOEPC_MSG msg;
    msg.response.opcode = p_ccb->last_pkt_opcode;
    msg.response.final = TRUE;
    msg.response.srm_en = FALSE;
    msg.response.srm_wait = FALSE;
    msg.response.pkt = pkt;
    /* operation complete, reset srm state */
    p_ccb->srm_state = GOEPC_SRM_STATE_IDLE;
    /* main state machine transfer to OPENED_IDLE */
    p_ccb->state = GOEPC_STATE_OPENED_IDLE;
    p_ccb->callback(p_ccb->allocated, GOEPC_RESPONSE_EVT, &msg);
}


static void goepc_sm_state_opening(tGOEPC_CCB *p_ccb, UINT8 event, tGOEPC_DATA *p_data)
{
    switch (event)
    {
    case GOEPC_SM_EVENT_CONNECT:
        goepc_sm_act_connect(p_ccb, &p_data->connected);
        break;
    case GOEPC_SM_EVENT_DISCONNECT:
        goepc_sm_act_disconnect(p_ccb);
        break;
    case GOEPC_SM_EVENT_RSP:
    case GOEPC_SM_EVENT_RSP_FB:
        GOEPC_TRACE_ERROR("goepc_sm_state_opening received unexpected response from peer\n");
        if (p_data->pkt != NULL) {
            osi_free(p_data->pkt);
        }
        goepc_sm_act_disconnect(p_ccb);
        break;
    default:
        GOEPC_TRACE_ERROR("goepc_sm_state_opening unexpected event: 0x%x\n", event);
        break;
    }
}

static void goepc_sm_state_opened_idle(tGOEPC_CCB *p_ccb, UINT8 event, tGOEPC_DATA *p_data)
{
    switch (event)
    {
    case GOEPC_SM_EVENT_DISCONNECT:
        goepc_sm_act_disconnect(p_ccb);
        break;
    case GOEPC_SM_EVENT_REQ:
        goepc_sm_act_send_req(p_ccb, p_data->pkt);
        break;
    case GOEPC_SM_EVENT_REQ_FB:
        goepc_sm_act_send_req_fb(p_ccb, p_data->pkt);
        break;
    case GOEPC_SM_EVENT_RSP:
    case GOEPC_SM_EVENT_RSP_FB:
        GOEPC_TRACE_ERROR("goepc_sm_state_opened_idle received unexpected response from peer\n");
        /* peer sent a packet to us when we didn't request */
        if (p_data->pkt != NULL) {
            osi_free(p_data->pkt);
        }
        goepc_sm_act_disconnect(p_ccb);
        break;
    default:
        GOEPC_TRACE_ERROR("goepc_sm_state_opened_idle unexpected event: 0x%x\n", event);
        break;
    }
}

static void goepc_sm_state_opened_req(tGOEPC_CCB *p_ccb, UINT8 event, tGOEPC_DATA *p_data)
{
    switch (event)
    {
    case GOEPC_SM_EVENT_DISCONNECT:
        goepc_sm_act_disconnect(p_ccb);
        break;
    case GOEPC_SM_EVENT_REQ:
        goepc_sm_act_send_req(p_ccb, p_data->pkt);
        break;
    case GOEPC_SM_EVENT_REQ_FB:
        goepc_sm_act_send_req_fb(p_ccb, p_data->pkt);
        break;
    case GOEPC_SM_EVENT_RSP:
        goepc_sm_act_rsp(p_ccb, p_data->pkt);
        break;
    case GOEPC_SM_EVENT_RSP_FB:
        goepc_sm_act_rsp_fb(p_ccb, p_data->pkt);
        break;
    default:
        GOEPC_TRACE_ERROR("goepc_sm_state_opened_req unexpected event: 0x%x\n", event);
        break;
    }
}

static void goepc_sm_state_opened_rsp(tGOEPC_CCB *p_ccb, UINT8 event, tGOEPC_DATA *p_data)
{
    switch (event)
    {
    case GOEPC_SM_EVENT_DISCONNECT:
        goepc_sm_act_disconnect(p_ccb);
        break;
    case GOEPC_SM_EVENT_REQ_FB:
        goepc_sm_act_send_req_fb(p_ccb, p_data->pkt);
        break;
    case GOEPC_SM_EVENT_RSP:
        goepc_sm_act_rsp(p_ccb, p_data->pkt);
        break;
    case GOEPC_SM_EVENT_RSP_FB:
        goepc_sm_act_rsp_fb(p_ccb, p_data->pkt);
        break;
    default:
        GOEPC_TRACE_ERROR("goepc_sm_state_opened_rsp unexpected event: 0x%x\n", event);
        break;
    }
}

BOOLEAN goepc_check_obex_req_allow(UINT8 state, BOOLEAN final)
{
    BOOLEAN ret = FALSE;
    if (final) {
        switch (state)
        {
        case GOEPC_STATE_OPENED_IDLE:
        case GOEPC_STATE_OPENED_REQ:
        case GOEPC_STATE_OPENED_RSP:
            ret = TRUE;
            break;
        default:
            break;
        }
    }
    else {
        switch (state)
        {
        case GOEPC_STATE_OPENED_IDLE:
        case GOEPC_STATE_OPENED_REQ:
            ret = TRUE;
            break;
        default:
            break;
        }
    }
    return ret;
}

void goepc_sm_execute(tGOEPC_CCB *p_ccb, UINT8 event, tGOEPC_DATA *p_data)
{
    switch (p_ccb->state)
    {
    case GOEPC_STATE_INIT:
        /* do nothing */
        break;
    case GOEPC_STATE_OPENING:
        goepc_sm_state_opening(p_ccb, event, p_data);
        break;
    case GOEPC_STATE_OPENED_IDLE:
        goepc_sm_state_opened_idle(p_ccb, event, p_data);
        break;
    case GOEPC_STATE_OPENED_REQ:
        goepc_sm_state_opened_req(p_ccb, event, p_data);
        break;
    case GOEPC_STATE_OPENED_RSP:
        goepc_sm_state_opened_rsp(p_ccb, event, p_data);
        break;
    default:
        GOEPC_TRACE_ERROR("goepc_sm_execute unexpected state: 0x%x\n", p_ccb->state);
        break;
    }
}

static void goepc_srm_sm_act_req(tGOEPC_CCB *p_ccb, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    switch (p_ccb->srm_state)
    {
    case GOEPC_SRM_STATE_IDLE:
        if (srm_en) {
            p_ccb->srm_state = GOEPC_SRM_STATE_REQ;
            p_ccb->srm_wait = srm_wait;
        }
        else {
            p_ccb->srm_state = GOEPC_SRM_STATE_DISABLE;
        }
        break;
    case GOEPC_SRM_STATE_ENABLE_WAIT:
        if (!srm_wait) {
            p_ccb->srm_wait = FALSE;
        }
        if (!p_ccb->srm_wait && !p_ccb->srm_peer_wait) {
            /* no more wait, transfer to ENABLE */
            p_ccb->srm_state = GOEPC_SRM_STATE_ENABLE;
        }
        break;
    default:
        break;
    }
}

static void goepc_srm_sm_act_rsp(tGOEPC_CCB *p_ccb, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    switch (p_ccb->srm_state)
    {
    case GOEPC_SRM_STATE_IDLE:
        /* peer can not request to enable srm, ignore */
        break;
    case GOEPC_SRM_STATE_REQ:
        if (srm_en) {
            p_ccb->srm_peer_wait = srm_wait;
            if (p_ccb->srm_wait || p_ccb->srm_peer_wait) {
                p_ccb->srm_state = GOEPC_SRM_STATE_ENABLE_WAIT;
            }
            else {
                p_ccb->srm_state = GOEPC_SRM_STATE_ENABLE;
            }
        }
        else {
            p_ccb->srm_state = GOEPC_SRM_STATE_DISABLE;
        }
        break;
    case GOEPC_SRM_STATE_ENABLE_WAIT:
        if (!srm_wait) {
            p_ccb->srm_peer_wait = FALSE;
        }
        if (!p_ccb->srm_wait && !p_ccb->srm_peer_wait) {
            /* no more wait, transfer to ENABLE */
            p_ccb->srm_state = GOEPC_SRM_STATE_ENABLE;
        }
        break;
    default:
        break;
    }
}

void goepc_srm_sm_execute(tGOEPC_CCB *p_ccb, BOOLEAN is_req, BOOLEAN srm_en, BOOLEAN srm_wait)
{
    if (is_req) {
        goepc_srm_sm_act_req(p_ccb, srm_en, srm_wait);
    }
    else {
        goepc_srm_sm_act_rsp(p_ccb, srm_en, srm_wait);
    }
}

#endif /* #if (GOEPC_INCLUDED == TRUE) */
