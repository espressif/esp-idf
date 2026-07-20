/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "common/bt_target.h"
#include "stack/bt_types.h"
#include "stack/goep_common.h"
#include "stack/goeps_api.h"
#include "stack/l2c_api.h"
#include "stack/btm_api.h"
#include "bta_opp_int.h"

#if BTA_OPP_SERVER_INCLUDED

/* state machine states */
enum {
    BTA_OPP_SERVER_CONNECTING_ST,   /* transport up, awaiting OBEX CONNECT / OPENED */
    BTA_OPP_SERVER_CONNECTED_ST,    /* OBEX connected, idle between objects */
    BTA_OPP_SERVER_PENDING_ST,      /* incoming object reported, awaiting accept/reject */
    BTA_OPP_SERVER_RECEIVING_ST,    /* receiving object body */
};

/* state machine action enumeration list */
enum {
    BTA_OPP_SERVER_ACT_OPENED,
    BTA_OPP_SERVER_ACT_REQ_CONNECT,
    BTA_OPP_SERVER_ACT_PUT_FIRST,
    BTA_OPP_SERVER_ACT_PUT_UNEXPECTED,
    BTA_OPP_SERVER_ACT_PUT_DATA,
    BTA_OPP_SERVER_ACT_REQ_DISCONNECT,
    BTA_OPP_SERVER_ACT_REQ_ABORT,
    BTA_OPP_SERVER_ACT_REQ_OTHER,
    BTA_OPP_SERVER_ACT_ACCEPT,
    BTA_OPP_SERVER_ACT_REJECT,
    BTA_OPP_SERVER_ACT_CANCEL,
    BTA_OPP_SERVER_ACT_CLOSED,
    BTA_OPP_SERVER_ACT_MTU,
    BTA_OPP_SERVER_NUM_ACTIONS
};

#define BTA_OPP_SERVER_IGNORE       BTA_OPP_SERVER_NUM_ACTIONS

/* forward declaration of action functions */
static void bta_opp_server_act_opened(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_req_connect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_put_first(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_put_unexpected(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_put_data(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_req_disconnect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_req_abort(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_req_other(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_accept(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_reject(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_cancel(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_closed(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);
static void bta_opp_server_act_mtu(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);

/* type for action functions */
typedef void (*tBTA_OPP_SERVER_ACTION)(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data);

/* action functions table, indexed with action enum */
static const tBTA_OPP_SERVER_ACTION bta_opp_server_action[] = {
    bta_opp_server_act_opened,          /* BTA_OPP_SERVER_ACT_OPENED */
    bta_opp_server_act_req_connect,     /* BTA_OPP_SERVER_ACT_REQ_CONNECT */
    bta_opp_server_act_put_first,       /* BTA_OPP_SERVER_ACT_PUT_FIRST */
    bta_opp_server_act_put_unexpected,  /* BTA_OPP_SERVER_ACT_PUT_UNEXPECTED */
    bta_opp_server_act_put_data,        /* BTA_OPP_SERVER_ACT_PUT_DATA */
    bta_opp_server_act_req_disconnect,  /* BTA_OPP_SERVER_ACT_REQ_DISCONNECT */
    bta_opp_server_act_req_abort,       /* BTA_OPP_SERVER_ACT_REQ_ABORT */
    bta_opp_server_act_req_other,       /* BTA_OPP_SERVER_ACT_REQ_OTHER */
    bta_opp_server_act_accept,          /* BTA_OPP_SERVER_ACT_ACCEPT */
    bta_opp_server_act_reject,          /* BTA_OPP_SERVER_ACT_REJECT */
    bta_opp_server_act_cancel,          /* BTA_OPP_SERVER_ACT_CANCEL */
    bta_opp_server_act_closed,          /* BTA_OPP_SERVER_ACT_CLOSED */
    bta_opp_server_act_mtu,             /* BTA_OPP_SERVER_ACT_MTU */
};

/* state table information */
#define BTA_OPP_SERVER_ACTION_COL       0       /* position of action */
#define BTA_OPP_SERVER_NEXT_STATE_COL   1       /* position of next state */
#define BTA_OPP_SERVER_NUM_COLS         2       /* number of columns */

/* State machine events are indexed by (event & 0xff), which yields the
 * following order for the events declared in bta_opp_server_int.h:
 *   0 REQ_CONNECT 1 REQ_PUT   2 REQ_DISCONNECT 3 REQ_ABORT  4 REQ_OTHER
 *   5 GOEP_OPEN   6 GOEP_CLOSE 7 GOEP_MTU       8 ACCEPT     9 REJECT     10 CANCEL
 */
static const UINT8 bta_opp_server_st_connecting[][BTA_OPP_SERVER_NUM_COLS] = {
    /* REQ_CONNECT */    {BTA_OPP_SERVER_ACT_REQ_CONNECT,    BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_PUT */        {BTA_OPP_SERVER_ACT_PUT_FIRST,      BTA_OPP_SERVER_CONNECTED_ST},
    /* REQ_DISCONNECT */ {BTA_OPP_SERVER_ACT_REQ_DISCONNECT, BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_ABORT */      {BTA_OPP_SERVER_ACT_REQ_ABORT,      BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_OTHER */      {BTA_OPP_SERVER_ACT_REQ_OTHER,      BTA_OPP_SERVER_CONNECTING_ST},
    /* GOEP_OPEN */      {BTA_OPP_SERVER_ACT_OPENED,         BTA_OPP_SERVER_CONNECTED_ST},
    /* GOEP_CLOSE */     {BTA_OPP_SERVER_ACT_CLOSED,         BTA_OPP_SERVER_CONNECTING_ST},
    /* GOEP_MTU */       {BTA_OPP_SERVER_ACT_MTU,            BTA_OPP_SERVER_CONNECTING_ST},
    /* API_ACCEPT */     {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_CONNECTING_ST},
    /* API_REJECT */     {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_CONNECTING_ST},
    /* API_CANCEL */     {BTA_OPP_SERVER_ACT_CANCEL,         BTA_OPP_SERVER_CONNECTING_ST},
};

static const UINT8 bta_opp_server_st_connected[][BTA_OPP_SERVER_NUM_COLS] = {
    /* REQ_CONNECT */    {BTA_OPP_SERVER_ACT_REQ_CONNECT,    BTA_OPP_SERVER_CONNECTED_ST},
    /* REQ_PUT */        {BTA_OPP_SERVER_ACT_PUT_FIRST,      BTA_OPP_SERVER_CONNECTED_ST},
    /* REQ_DISCONNECT */ {BTA_OPP_SERVER_ACT_REQ_DISCONNECT, BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_ABORT */      {BTA_OPP_SERVER_ACT_REQ_ABORT,      BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_OTHER */      {BTA_OPP_SERVER_ACT_REQ_OTHER,      BTA_OPP_SERVER_CONNECTED_ST},
    /* GOEP_OPEN */      {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_CONNECTED_ST},
    /* GOEP_CLOSE */     {BTA_OPP_SERVER_ACT_CLOSED,         BTA_OPP_SERVER_CONNECTING_ST},
    /* GOEP_MTU */       {BTA_OPP_SERVER_ACT_MTU,            BTA_OPP_SERVER_CONNECTED_ST},
    /* API_ACCEPT */     {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_CONNECTED_ST},
    /* API_REJECT */     {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_CONNECTED_ST},
    /* API_CANCEL */     {BTA_OPP_SERVER_ACT_CANCEL,         BTA_OPP_SERVER_CONNECTING_ST},
};

static const UINT8 bta_opp_server_st_pending[][BTA_OPP_SERVER_NUM_COLS] = {
    /* REQ_CONNECT */    {BTA_OPP_SERVER_ACT_REQ_CONNECT,    BTA_OPP_SERVER_PENDING_ST},
    /* REQ_PUT */        {BTA_OPP_SERVER_ACT_PUT_UNEXPECTED, BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_DISCONNECT */ {BTA_OPP_SERVER_ACT_REQ_DISCONNECT, BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_ABORT */      {BTA_OPP_SERVER_ACT_REQ_ABORT,      BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_OTHER */      {BTA_OPP_SERVER_ACT_REQ_OTHER,      BTA_OPP_SERVER_PENDING_ST},
    /* GOEP_OPEN */      {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_PENDING_ST},
    /* GOEP_CLOSE */     {BTA_OPP_SERVER_ACT_CLOSED,         BTA_OPP_SERVER_CONNECTING_ST},
    /* GOEP_MTU */       {BTA_OPP_SERVER_ACT_MTU,            BTA_OPP_SERVER_PENDING_ST},
    /* API_ACCEPT */     {BTA_OPP_SERVER_ACT_ACCEPT,         BTA_OPP_SERVER_RECEIVING_ST},
    /* API_REJECT */     {BTA_OPP_SERVER_ACT_REJECT,         BTA_OPP_SERVER_CONNECTING_ST},
    /* API_CANCEL */     {BTA_OPP_SERVER_ACT_CANCEL,         BTA_OPP_SERVER_CONNECTING_ST},
};

static const UINT8 bta_opp_server_st_receiving[][BTA_OPP_SERVER_NUM_COLS] = {
    /* REQ_CONNECT */    {BTA_OPP_SERVER_ACT_REQ_CONNECT,    BTA_OPP_SERVER_RECEIVING_ST},
    /* REQ_PUT */        {BTA_OPP_SERVER_ACT_PUT_DATA,       BTA_OPP_SERVER_RECEIVING_ST},
    /* REQ_DISCONNECT */ {BTA_OPP_SERVER_ACT_REQ_DISCONNECT, BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_ABORT */      {BTA_OPP_SERVER_ACT_REQ_ABORT,      BTA_OPP_SERVER_CONNECTING_ST},
    /* REQ_OTHER */      {BTA_OPP_SERVER_ACT_REQ_OTHER,      BTA_OPP_SERVER_RECEIVING_ST},
    /* GOEP_OPEN */      {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_RECEIVING_ST},
    /* GOEP_CLOSE */     {BTA_OPP_SERVER_ACT_CLOSED,         BTA_OPP_SERVER_CONNECTING_ST},
    /* GOEP_MTU */       {BTA_OPP_SERVER_ACT_MTU,            BTA_OPP_SERVER_RECEIVING_ST},
    /* API_ACCEPT */     {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_RECEIVING_ST},
    /* API_REJECT */     {BTA_OPP_SERVER_IGNORE,             BTA_OPP_SERVER_RECEIVING_ST},
    /* API_CANCEL */     {BTA_OPP_SERVER_ACT_CANCEL,         BTA_OPP_SERVER_CONNECTING_ST},
};

/* type for state table */
typedef const UINT8 (*tBTA_OPP_SERVER_ST_TBL)[BTA_OPP_SERVER_NUM_COLS];

/* state table, indexed with state enum */
static const tBTA_OPP_SERVER_ST_TBL bta_opp_server_st_tbl[] = {
    bta_opp_server_st_connecting,
    bta_opp_server_st_connected,
    bta_opp_server_st_pending,
    bta_opp_server_st_receiving,
};

/******************************************************************************
 * Helpers
 *****************************************************************************/

static char *utf16be_to_ascii(const UINT8 *src, UINT16 len)
{
    UINT16 chars = len / 2;
    char *out = (char *)osi_malloc(chars + 1);
    if (out == NULL) {
        return NULL;
    }

    for (UINT16 i = 0; i < chars; i++) {
        out[i] = (char)src[i * 2 + 1];
    }
    out[chars] = 0;
    return out;
}

static void send_simple_response(UINT16 goep_handle, UINT8 opcode, UINT8 response_code, UINT16 max_rx)
{
    tOBEX_PARSE_INFO info = {0};

    info.opcode = opcode;
    info.response_code = response_code | OBEX_FINAL_BIT_MASK;
    info.obex_version_number = OBEX_VERSION_NUMBER;
    info.max_packet_length = max_rx;
    if (GOEPS_PrepareResponse(goep_handle, &info, max_rx) == GOEP_SUCCESS) {
        GOEPS_SendResponse(goep_handle);
    }
}

static void bta_opp_server_reset_transfer(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL) {
        return;
    }

    if (p_ccb->name) {
        osi_free(p_ccb->name);
        p_ccb->name = NULL;
    }
    if (p_ccb->type) {
        osi_free(p_ccb->type);
        p_ccb->type = NULL;
    }
    p_ccb->total_len = 0;
    p_ccb->transferred = 0;
    p_ccb->rx_flow_blocked = FALSE;
    p_ccb->deferred_rsp_opcode = 0;
    p_ccb->state = BTA_OPP_SERVER_CONNECTED_ST;
}

static void bta_opp_server_stop_accept_timer(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb != NULL && p_ccb->accept_timer_on) {
        bta_sys_stop_timer(&p_ccb->accept_timer);
        p_ccb->accept_timer_on = FALSE;
    }
}

static void bta_opp_server_free_hold_put(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL) {
        return;
    }
    if (p_ccb->has_hold_put && p_ccb->hold_put.pkt != NULL) {
        osi_free(p_ccb->hold_put.pkt);
        p_ccb->hold_put.pkt = NULL;
    }
    p_ccb->has_hold_put = FALSE;
    p_ccb->hold_put.opcode = 0;
    p_ccb->hold_put.req_final = FALSE;
}

void bta_opp_server_cleanup_ccb(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL) {
        return;
    }

    bta_opp_server_stop_accept_timer(p_ccb);
    bta_sys_free_timer(&p_ccb->accept_timer);
    bta_opp_server_free_hold_put(p_ccb);
}

static void bta_opp_server_accept_timer_cback(TIMER_LIST_ENT *p_tle)
{
    tBTA_OPP_CCB *p_ccb;
    BT_HDR *p_buf;

    if (p_tle == NULL) {
        return;
    }

    p_ccb = (tBTA_OPP_CCB *)p_tle->param;
    if (p_ccb == NULL || !p_ccb->allocated || p_ccb->role != BTA_OPP_ROLE_SERVER) {
        return;
    }

    p_ccb->accept_timer_on = FALSE;
    /* Ignore stale timeouts once accept/reject already moved us out of PENDING. */
    if (p_ccb->state != BTA_OPP_SERVER_PENDING_ST || !p_ccb->has_hold_put) {
        return;
    }
    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_SERVER_API_REJECT_EVT;
        p_buf->layer_specific = p_ccb->handle;
        bta_sys_sendmsg(p_buf);
    }
}

static void bta_opp_server_start_accept_timer(tBTA_OPP_CCB *p_ccb)
{
    bta_opp_server_stop_accept_timer(p_ccb);
    p_ccb->accept_timer.p_cback = (TIMER_CBACK *)&bta_opp_server_accept_timer_cback;
    p_ccb->accept_timer.param = (TIMER_PARAM_TYPE)p_ccb;
    bta_sys_start_timer(&p_ccb->accept_timer, 0, BTA_OPP_SERVER_ACCEPT_TIMEOUT_MS);
    p_ccb->accept_timer_on = TRUE;
}

static BOOLEAN bta_opp_server_hold_put(tBTA_OPP_CCB *p_ccb, UINT8 opcode, BOOLEAN req_final, BT_HDR *pkt)
{
    if (p_ccb->has_hold_put) {
        return FALSE;
    }

    p_ccb->hold_put.pkt = pkt;
    p_ccb->hold_put.opcode = opcode;
    p_ccb->hold_put.req_final = req_final;
    p_ccb->has_hold_put = TRUE;
    /* No OBEX response until accept / reject / timeout (matches Android/BlueZ). */
    return TRUE;
}

static void parse_put_headers(tBTA_OPP_CCB *p_ccb, BT_HDR *pkt, tOBEX_PARSE_INFO *info,
                              UINT8 **body, UINT16 *body_len, BOOLEAN *final);

static void bta_opp_server_report_incoming_object(tBTA_OPP_CCB *p_ccb, BOOLEAN needs_accept)
{
    tBTA_OPP object;

    memset(&object, 0, sizeof(object));
    object.object.handle = p_ccb->handle;
    object.object.name = p_ccb->name;
    object.object.type = p_ccb->type;
    object.object.len = p_ccb->total_len;
    object.object.needs_accept = needs_accept;
    bta_opp_pm_busy(p_ccb);
    if (bta_opp_cb.p_cback) {
        bta_opp_cb.p_cback(BTA_OPP_INCOMING_OBJECT_EVT, &object);
    }
}

static void bta_opp_server_process_put_data(tBTA_OPP_CCB *p_ccb, BOOLEAN req_final, BT_HDR *pkt)
{
    tOBEX_PARSE_INFO info = {0};
    UINT8 *body = NULL;
    UINT16 body_len = 0;
    BOOLEAN final = req_final;

    if (OBEX_ParseRequest(pkt, &info) != OBEX_SUCCESS) {
        osi_free(pkt);
        send_simple_response(p_ccb->goep_handle, OBEX_OPCODE_PUT, OBEX_RESPONSE_CODE_BAD_REQUEST, p_ccb->max_rx);
        return;
    }

    parse_put_headers(p_ccb, pkt, &info, &body, &body_len, &final);
    if (body != NULL || final) {
        tBTA_OPP data;

        memset(&data, 0, sizeof(data));
        data.data.handle = p_ccb->handle;
        data.data.data = body;
        data.data.data_len = body_len;
        data.data.final = final || info.opcode == OBEX_OPCODE_PUT_FINAL;
        data.data.pkt = pkt;
        p_ccb->transferred += body_len;
        if (data.data.final) {
            if (bta_opp_cb.p_cback) {
                bta_opp_cb.p_cback(BTA_OPP_DATA_EVT, &data);
            } else {
                osi_free(pkt);
            }
            /* Skip progress when the final PUT carries no new body (common empty
             * End-of-Body after the last data packet already reported 100%). */
            if (body_len > 0) {
                bta_opp_report_progress(p_ccb);
            }
            bta_opp_report_complete(p_ccb, BTA_OPP_OK);
            send_simple_response(p_ccb->goep_handle, info.opcode, OBEX_RESPONSE_CODE_OK, p_ccb->max_rx);
            bta_opp_server_reset_transfer(p_ccb);
        } else {
            /* Mark blocked before posting DATA so a racing RxReady cannot be lost.
             * Continue is sent from BTA_OppServerRxReady after VFS accepts credit. */
            p_ccb->rx_flow_blocked = TRUE;
            p_ccb->deferred_rsp_opcode = info.opcode;
            if (bta_opp_cb.p_cback) {
                bta_opp_cb.p_cback(BTA_OPP_DATA_EVT, &data);
            } else {
                osi_free(pkt);
                send_simple_response(p_ccb->goep_handle, info.opcode, OBEX_RESPONSE_CODE_CONTINUE, p_ccb->max_rx);
                p_ccb->rx_flow_blocked = FALSE;
                p_ccb->deferred_rsp_opcode = 0;
            }
            if (body_len > 0) {
                bta_opp_report_progress(p_ccb);
            }
        }
    } else {
        osi_free(pkt);
        send_simple_response(p_ccb->goep_handle, info.opcode, OBEX_RESPONSE_CODE_CONTINUE, p_ccb->max_rx);
    }
}

static void bta_opp_server_process_hold_put(tBTA_OPP_CCB *p_ccb)
{
    BOOLEAN req_final;
    BT_HDR *pkt;

    if (!p_ccb->has_hold_put) {
        return;
    }

    req_final = p_ccb->hold_put.req_final;
    pkt = p_ccb->hold_put.pkt;
    p_ccb->hold_put.pkt = NULL;
    p_ccb->has_hold_put = FALSE;
    p_ccb->state = BTA_OPP_SERVER_RECEIVING_ST;
    bta_opp_server_process_put_data(p_ccb, req_final, pkt);
}

static void bta_opp_server_reject_pending_object(tBTA_OPP_CCB *p_ccb)
{
    UINT8 opcode = OBEX_OPCODE_PUT;

    bta_opp_server_stop_accept_timer(p_ccb);
    p_ccb->session_accepted = FALSE;
    if (p_ccb->has_hold_put) {
        if (p_ccb->hold_put.opcode != 0) {
            opcode = p_ccb->hold_put.opcode;
        }
        send_simple_response(p_ccb->goep_handle, opcode, OBEX_RESPONSE_CODE_FORBIDDEN, p_ccb->max_rx);
        bta_opp_server_free_hold_put(p_ccb);
    }
    bta_opp_report_complete(p_ccb, BTA_OPP_FORBIDDEN);
    bta_opp_server_reset_transfer(p_ccb);
    bta_opp_close_connection(p_ccb, BTA_OPP_FORBIDDEN, TRUE);
}

static void parse_put_headers(tBTA_OPP_CCB *p_ccb, BT_HDR *pkt, tOBEX_PARSE_INFO *info,
                              UINT8 **body, UINT16 *body_len, BOOLEAN *final)
{
    UINT8 *header = NULL;
    UINT8 *pkt_data = (UINT8 *)(pkt + 1) + pkt->offset;
    UINT8 *pkt_end = pkt_data + pkt->len;

    while ((header = OBEX_GetNextHeader(pkt, info)) != NULL) {
        UINT16 header_len = OBEX_GetHeaderLength(header, pkt_end);
        switch (*header) {
        case OBEX_HEADER_ID_NAME:
            if (header_len > 3) {
                if (p_ccb->name) {
                    osi_free(p_ccb->name);
                }
                p_ccb->name = utf16be_to_ascii(header + 3, header_len - 3);
            }
            break;
        case OBEX_HEADER_ID_TYPE:
            if (header_len > 3) {
                if (p_ccb->type) {
                    osi_free(p_ccb->type);
                }
                p_ccb->type = (char *)osi_malloc(header_len - 2);
                if (p_ccb->type) {
                    memcpy(p_ccb->type, header + 3, header_len - 3);
                    p_ccb->type[header_len - 3] = 0;
                }
            }
            break;
        case OBEX_HEADER_ID_LENGTH:
            if (header_len == 5) {
                p_ccb->total_len = ((UINT32)header[1] << 24) | ((UINT32)header[2] << 16) |
                                   ((UINT32)header[3] << 8) | header[4];
            }
            break;
        case OBEX_HEADER_ID_BODY:
        case OBEX_HEADER_ID_END_OF_BODY:
            if (header_len >= 3) {
                *body = header + 3;
                *body_len = header_len - 3;
                *final = (*header == OBEX_HEADER_ID_END_OF_BODY);
            }
            break;
        default:
            break;
        }
    }
}

static void bta_opp_server_goep_cback(UINT16 conn_handle, UINT8 event, tGOEPS_MSG *p_msg)
{
    tBTA_OPP_SERVER_GOEP_MSG *p_buf = NULL;

    switch (event) {
    case GOEPS_CONN_INCOME_EVT:
        p_buf = (tBTA_OPP_SERVER_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_SERVER_GOEP_MSG));
        if (p_buf) {
            memset(p_buf, 0, sizeof(tBTA_OPP_SERVER_GOEP_MSG));
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_CONN_INCOME_EVT;
            p_buf->hdr.layer_specific = conn_handle;
            p_buf->peer_mtu = p_msg->conn_income.peer_mtu;
            p_buf->our_mtu = p_msg->conn_income.our_mtu;
            bdcpy(p_buf->bd_addr, p_msg->conn_income.addr);
        } else {
            GOEPS_CloseConn(conn_handle);
        }
        break;
    case GOEPS_OPENED_EVT:
        p_buf = (tBTA_OPP_SERVER_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_SERVER_GOEP_MSG));
        if (p_buf) {
            memset(p_buf, 0, sizeof(tBTA_OPP_SERVER_GOEP_MSG));
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_OPEN_EVT;
            p_buf->hdr.layer_specific = conn_handle;
            p_buf->peer_mtu = p_msg->opened.peer_mtu;
            p_buf->our_mtu = p_msg->opened.our_mtu;
        }
        break;
    case GOEPS_CLOSED_EVT:
        p_buf = (tBTA_OPP_SERVER_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_SERVER_GOEP_MSG));
        if (p_buf) {
            memset(p_buf, 0, sizeof(tBTA_OPP_SERVER_GOEP_MSG));
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_CLOSE_EVT;
            p_buf->hdr.layer_specific = conn_handle;
        } else {
            /* OOM: free CCB here or the slot leaks until reboot (GOEP already gone). */
            tBTA_OPP_CCB *p_ccb = bta_opp_find_ccb_by_goep_handle(BTA_OPP_ROLE_SERVER, conn_handle);
            if (p_ccb != NULL) {
                bta_opp_server_act_closed(p_ccb, NULL);
            }
        }
        break;
    case GOEPS_MTU_CHANGED_EVT:
        p_buf = (tBTA_OPP_SERVER_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_SERVER_GOEP_MSG));
        if (p_buf) {
            memset(p_buf, 0, sizeof(tBTA_OPP_SERVER_GOEP_MSG));
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_MTU_EVT;
            p_buf->hdr.layer_specific = conn_handle;
            p_buf->peer_mtu = p_msg->mtu_changed.peer_mtu;
            p_buf->our_mtu = p_msg->mtu_changed.our_mtu;
        }
        break;
    case GOEPS_REQUEST_EVT:
        p_buf = (tBTA_OPP_SERVER_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_SERVER_GOEP_MSG));
        if (p_buf == NULL) {
            if (p_msg && p_msg->request.pkt) {
                osi_free(p_msg->request.pkt);
            }
            break;
        }
        memset(p_buf, 0, sizeof(tBTA_OPP_SERVER_GOEP_MSG));
        p_buf->hdr.layer_specific = conn_handle;
        p_buf->opcode = p_msg->request.opcode;
        p_buf->final = p_msg->request.final;
        p_buf->pkt = p_msg->request.pkt;
        switch (p_msg->request.opcode) {
        case OBEX_OPCODE_CONNECT:
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_REQ_CONNECT_EVT;
            break;
        case OBEX_OPCODE_PUT:
        case OBEX_OPCODE_PUT_FINAL:
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_REQ_PUT_EVT;
            break;
        case OBEX_OPCODE_DISCONNECT:
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_REQ_DISCONNECT_EVT;
            break;
        case OBEX_OPCODE_ABORT:
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_REQ_ABORT_EVT;
            break;
        default:
            p_buf->hdr.event = BTA_OPP_SERVER_GOEP_REQ_OTHER_EVT;
            break;
        }
        break;
    case GOEPS_CONGEST_EVT:
    case GOEPS_UNCONGEST_EVT:
    default:
        break;
    }

    if (p_buf) {
        bta_sys_sendmsg(p_buf);
    }
}

/******************************************************************************
 * State machine action functions
 *****************************************************************************/

static void bta_opp_server_act_opened(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    UNUSED(p_data);
    bta_opp_report_conn(p_ccb, TRUE, BTA_OPP_OK);
}

static void bta_opp_server_act_req_connect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    tOBEX_PARSE_INFO info = {0};
    BT_HDR *pkt = p_data->goep.pkt;

    OBEX_ParseRequest(pkt, &info);
    p_ccb->max_tx = info.max_packet_length >= OPP_MIN_MTU ? info.max_packet_length : p_ccb->max_tx;
    send_simple_response(p_ccb->goep_handle, OBEX_OPCODE_CONNECT, OBEX_RESPONSE_CODE_OK, p_ccb->max_rx);
    osi_free(pkt);
}

static void bta_opp_server_act_put_first(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    tOBEX_PARSE_INFO info = {0};
    BT_HDR *pkt = p_data->goep.pkt;
    UINT8 *body = NULL;
    UINT16 body_len = 0;
    BOOLEAN final = p_data->goep.final;

    OBEX_ParseRequest(pkt, &info);
    parse_put_headers(p_ccb, pkt, &info, &body, &body_len, &final);

    /* A content-less PUT arriving between objects (e.g. a stray empty
     * End-of-Body after a completed transfer) carries no name, length
     * or body, and must not be reported as a new incoming object. */
    if (body_len == 0 && p_ccb->total_len == 0 && p_ccb->name == NULL) {
        osi_free(pkt);
        /* Type-only header must not stick on CCB for the next object. */
        if (p_ccb->type) {
            osi_free(p_ccb->type);
            p_ccb->type = NULL;
        }
        send_simple_response(p_ccb->goep_handle, info.opcode, OBEX_RESPONSE_CODE_OK, p_ccb->max_rx);
        return;
    }

    /* auto_accept: all objects; else first object per connection needs accept,
     * subsequent objects on an authorized session are auto-accepted. */
    BOOLEAN needs_accept = !bta_opp_cb.server_auto_accept && !p_ccb->session_accepted;

    bta_opp_server_report_incoming_object(p_ccb, needs_accept);
    if (needs_accept) {
        p_ccb->state = BTA_OPP_SERVER_PENDING_ST;
        if (!bta_opp_server_hold_put(p_ccb, info.opcode, p_data->goep.final, pkt)) {
            osi_free(pkt);
            send_simple_response(p_ccb->goep_handle, info.opcode, OBEX_RESPONSE_CODE_FORBIDDEN, p_ccb->max_rx);
            bta_opp_server_reject_pending_object(p_ccb);
            return;
        }
        bta_opp_server_start_accept_timer(p_ccb);
        return;
    }
    p_ccb->state = BTA_OPP_SERVER_RECEIVING_ST;
    bta_opp_server_process_put_data(p_ccb, p_data->goep.final, pkt);
}

static void bta_opp_server_act_put_unexpected(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    /* Standard clients wait for Continue before sending the next PUT.
     * A second PUT while the first is still held means the peer is misbehaving. */
    send_simple_response(p_ccb->goep_handle, p_data->goep.opcode, OBEX_RESPONSE_CODE_FORBIDDEN, p_ccb->max_rx);
    osi_free(p_data->goep.pkt);
    bta_opp_server_reject_pending_object(p_ccb);
}

static void bta_opp_server_act_put_data(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    bta_opp_server_process_put_data(p_ccb, p_data->goep.final, p_data->goep.pkt);
}

static void bta_opp_server_act_req_disconnect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    send_simple_response(p_ccb->goep_handle, OBEX_OPCODE_DISCONNECT, OBEX_RESPONSE_CODE_OK, p_ccb->max_rx);
    osi_free(p_data->goep.pkt);
    bta_opp_server_stop_accept_timer(p_ccb);
    bta_opp_server_free_hold_put(p_ccb);
    bta_opp_close_connection(p_ccb, BTA_OPP_OK, TRUE);
}

static void bta_opp_server_act_req_abort(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    osi_free(p_data->goep.pkt);
    send_simple_response(p_ccb->goep_handle, OBEX_OPCODE_ABORT, OBEX_RESPONSE_CODE_OK, p_ccb->max_rx);
    bta_opp_server_stop_accept_timer(p_ccb);
    bta_opp_server_free_hold_put(p_ccb);
    p_ccb->rx_flow_blocked = FALSE;
    p_ccb->deferred_rsp_opcode = 0;
    bta_opp_report_complete(p_ccb, BTA_OPP_ABORTED);
    bta_opp_close_connection(p_ccb, BTA_OPP_ABORTED, TRUE);
}

static void bta_opp_server_act_req_other(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    osi_free(p_data->goep.pkt);
    send_simple_response(p_ccb->goep_handle, p_data->goep.opcode, OBEX_RESPONSE_CODE_NOT_IMPLEMENTED, p_ccb->max_rx);
}

static void bta_opp_server_act_accept(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    UNUSED(p_data);
    bta_opp_server_stop_accept_timer(p_ccb);
    p_ccb->session_accepted = TRUE;
    bta_opp_server_process_hold_put(p_ccb);
}

static void bta_opp_server_act_reject(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    UNUSED(p_data);
    if (!p_ccb->has_hold_put) {
        return;
    }
    bta_opp_server_reject_pending_object(p_ccb);
}

static void bta_opp_server_act_cancel(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    UNUSED(p_data);
    bta_opp_server_stop_accept_timer(p_ccb);
    bta_opp_server_free_hold_put(p_ccb);
    p_ccb->rx_flow_blocked = FALSE;
    p_ccb->deferred_rsp_opcode = 0;
    bta_opp_close_connection(p_ccb, BTA_OPP_ABORTED, TRUE);
}

static void bta_opp_server_act_closed(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    tBTA_OPP_STATUS status = BTA_OPP_OK;

    UNUSED(p_data);
    if (p_ccb->goep_handle == 0) {
        return;
    }
    p_ccb->goep_handle = 0;

    /* Peer/link drop during push (pending accept or receiving): match ABORT /
     * client act_closed — report TRANSFER_COMPLETE before CONN_CLOSE. */
    if (p_ccb->name != NULL || p_ccb->has_hold_put || p_ccb->transferred != 0) {
        bta_opp_report_complete(p_ccb, BTA_OPP_ABORTED);
        status = BTA_OPP_ABORTED;
    }
    bta_opp_report_conn(p_ccb, FALSE, status);
    /* free_ccb runs server cleanup (accept timer / hold_put). */
    bta_opp_free_ccb(p_ccb);
}

static void bta_opp_server_act_mtu(tBTA_OPP_CCB *p_ccb, tBTA_OPP_SERVER_DATA *p_data)
{
    p_ccb->max_tx = p_data->goep.peer_mtu;
    p_ccb->max_rx = p_data->goep.our_mtu;
}

/******************************************************************************
 * Module (non state machine) handlers
 *****************************************************************************/

static void bta_opp_server_api_enable(tBTA_OPP_SERVER_DATA *p_data)
{
    if (!bta_opp_cb.server_registered) {
        if (bta_opp_cb.p_cback == NULL) {
            bta_opp_cb.p_cback = p_data->api_enable.p_cback;
        }
        bta_opp_cb.server_registered = TRUE;
        if (bta_opp_cb.p_cback) {
            bta_opp_cb.p_cback(BTA_OPP_SERVER_ENABLE_EVT, NULL);
        }
    }
}

static void bta_opp_server_free_scn(void)
{
    if (bta_opp_cb.server_scn != 0) {
        BTM_FreeSCN(bta_opp_cb.server_scn);
        bta_opp_cb.server_scn = 0;
    }
}

static void bta_opp_server_api_stop(tBTA_OPP_SERVER_DATA *p_data)
{
    UNUSED(p_data);
    tBTA_OPP data;
    memset(&data, 0, sizeof(data));

    for (int i = 0; i < BTA_OPP_MAX_CONNECTION; i++) {
        if (bta_opp_cb.ccb[i].allocated && bta_opp_cb.ccb[i].role == BTA_OPP_ROLE_SERVER) {
            bta_opp_close_connection(&bta_opp_cb.ccb[i], BTA_OPP_FAIL, TRUE);
        }
    }

    bta_opp_server_del_sdp_record();
    if (bta_opp_cb.goep_l2cap_svr_handle != 0) {
        GOEPS_Deregister(bta_opp_cb.goep_l2cap_svr_handle);
        bta_opp_cb.goep_l2cap_svr_handle = 0;
        bta_opp_cb.server_l2cap_psm = 0;
    }
    if (bta_opp_cb.goep_svr_handle != 0) {
        GOEPS_Deregister(bta_opp_cb.goep_svr_handle);
        bta_opp_cb.goep_svr_handle = 0;
    }
    bta_opp_server_free_scn();
    data.server.status = BTA_OPP_OK;
    if (bta_opp_cb.p_cback) {
        bta_opp_cb.p_cback(BTA_OPP_SERVER_STOP_EVT, &data);
    }
}

static void bta_opp_server_api_disable(tBTA_OPP_SERVER_DATA *p_data)
{
    bta_opp_server_api_stop(p_data);

    tBTA_OPP_CBACK *p_cback = bta_opp_cb.p_cback;
    bta_opp_cb.server_registered = FALSE;
#if BTA_OPP_CLIENT_INCLUDED
    if (!bta_opp_cb.client_registered) {
        bta_opp_cb.p_cback = NULL;
    }
#else
    bta_opp_cb.p_cback = NULL;
#endif
    if (bta_sys_is_register(BTA_ID_OPS)) {
        bta_sys_deregister(BTA_ID_OPS);
    }
    if (p_cback) {
        p_cback(BTA_OPP_SERVER_DISABLE_EVT, NULL);
    }
}

static void bta_opp_server_api_start(tBTA_OPP_SERVER_DATA *p_data)
{
    tBTA_OPP data;
    tOBEX_SVR_INFO server = {0};
    UINT16 svr_handle = 0;
    UINT16 l2cap_svr_handle = 0;
    tBTA_OPP_STATUS status = BTA_OPP_FAIL;
    tBTA_OPP_SERVER_CFG *cfg = &p_data->api_start.cfg;
    UINT8 scn;

    memset(&data, 0, sizeof(data));

    if (bta_opp_cb.goep_svr_handle != 0) {
        status = BTA_OPP_BUSY;
        goto done;
    }

    scn = BTM_AllocateSCN();
    if (scn == 0) {
        status = BTA_OPP_NO_RESOURCE;
        goto done;
    }
    bta_opp_cb.server_scn = scn;

    bta_opp_cb.server_l2cap_psm = 0;

    server.tl = OBEX_OVER_RFCOMM;
    server.rfcomm.scn = scn;
    server.rfcomm.sec_mask = cfg->sec_mask;
    server.rfcomm.pref_mtu = cfg->mtu;
    if (GOEPS_Register(&server, bta_opp_server_goep_cback, &svr_handle) != GOEP_SUCCESS) {
        bta_opp_server_free_scn();
        status = BTA_OPP_OBEX_FAIL;
        goto done;
    }

    bta_opp_cb.goep_svr_handle = svr_handle;
    bta_opp_cb.server_mtu = cfg->mtu ? cfg->mtu : OPP_DEFAULT_MTU;
    bta_opp_cb.server_sec_mask = cfg->sec_mask;
    bta_opp_cb.server_auto_accept = cfg->auto_accept;

    memset(&server, 0, sizeof(server));
    server.tl = OBEX_OVER_L2CAP;
    server.l2cap.psm = L2CA_AllocatePSM();
    server.l2cap.sec_mask = cfg->sec_mask;
    server.l2cap.pref_mtu = cfg->mtu;
    if (GOEPS_Register(&server, bta_opp_server_goep_cback, &l2cap_svr_handle) == GOEP_SUCCESS) {
        bta_opp_cb.goep_l2cap_svr_handle = l2cap_svr_handle;
        bta_opp_cb.server_l2cap_psm = server.l2cap.psm;
    }

    bta_opp_cb.sdp_handle = bta_opp_server_create_sdp_record(cfg);
    if (bta_opp_cb.sdp_handle == 0) {
        if (bta_opp_cb.goep_l2cap_svr_handle != 0) {
            GOEPS_Deregister(bta_opp_cb.goep_l2cap_svr_handle);
            bta_opp_cb.goep_l2cap_svr_handle = 0;
            bta_opp_cb.server_l2cap_psm = 0;
        }
        GOEPS_Deregister(svr_handle);
        bta_opp_cb.goep_svr_handle = 0;
        bta_opp_server_free_scn();
        status = BTA_OPP_SDP_FAIL;
        goto done;
    }
    status = BTA_OPP_OK;

done:
    data.server.status = status;
    data.server.scn = bta_opp_cb.server_scn;
    if (bta_opp_cb.p_cback) {
        bta_opp_cb.p_cback(BTA_OPP_SERVER_START_EVT, &data);
    }
    if (cfg->service_name) {
        osi_free((void *)cfg->service_name);
        cfg->service_name = NULL;
    }
    if (cfg->supported_formats) {
        osi_free((void *)cfg->supported_formats);
        cfg->supported_formats = NULL;
    }
}

/******************************************************************************
 * State machine engine and event dispatcher
 *****************************************************************************/

static void bta_opp_server_sm_execute(tBTA_OPP_CCB *p_ccb, UINT16 event, tBTA_OPP_SERVER_DATA *p_data)
{
    tBTA_OPP_SERVER_ST_TBL state_table;
    UINT8 action;

    state_table = bta_opp_server_st_tbl[p_ccb->state];

    event &= 0xff;

    p_ccb->state = state_table[event][BTA_OPP_SERVER_NEXT_STATE_COL];

    if ((action = state_table[event][BTA_OPP_SERVER_ACTION_COL]) != BTA_OPP_SERVER_IGNORE) {
        (*bta_opp_server_action[action])(p_ccb, p_data);
    }
}

static void bta_opp_server_handle_conn_income(tBTA_OPP_SERVER_DATA *p_data)
{
    UINT16 conn_handle = p_data->goep.hdr.layer_specific;
    tBTA_OPP_CCB *p_ccb = bta_opp_allocate_ccb();

    if (p_ccb == NULL) {
        GOEPS_CloseConn(conn_handle);
        return;
    }
    p_ccb->role = BTA_OPP_ROLE_SERVER;
    p_ccb->goep_handle = conn_handle;
    p_ccb->max_tx = p_data->goep.peer_mtu;
    p_ccb->max_rx = p_data->goep.our_mtu;
    bdcpy(p_ccb->bd_addr, p_data->goep.bd_addr);
    /* state remains BTA_OPP_SERVER_CONNECTING_ST until the OBEX session is opened */
}

BOOLEAN bta_opp_server_hdl_event(BT_HDR *p_msg)
{
    tBTA_OPP_SERVER_DATA *p_data = (tBTA_OPP_SERVER_DATA *)p_msg;
    tBTA_OPP_CCB *p_ccb;

    switch (p_msg->event) {
    case BTA_OPP_SERVER_API_ENABLE_EVT:
        bta_opp_server_api_enable(p_data);
        break;
    case BTA_OPP_SERVER_API_DISABLE_EVT:
        bta_opp_server_api_disable(p_data);
        break;
    case BTA_OPP_SERVER_API_START_EVT:
        bta_opp_server_api_start(p_data);
        break;
    case BTA_OPP_SERVER_API_STOP_EVT:
        bta_opp_server_api_stop(p_data);
        break;
    case BTA_OPP_SERVER_GOEP_CONN_INCOME_EVT:
        bta_opp_server_handle_conn_income(p_data);
        break;
    case BTA_OPP_SERVER_API_RX_READY_EVT:
        p_ccb = bta_opp_find_ccb_by_handle(p_msg->layer_specific);
        if (p_ccb != NULL && p_ccb->role == BTA_OPP_ROLE_SERVER && p_ccb->rx_flow_blocked) {
            send_simple_response(p_ccb->goep_handle, p_ccb->deferred_rsp_opcode,
                                 OBEX_RESPONSE_CODE_CONTINUE, p_ccb->max_rx);
            p_ccb->rx_flow_blocked = FALSE;
            p_ccb->deferred_rsp_opcode = 0;
        }
        break;
    case BTA_OPP_SERVER_GOEP_REQ_CONNECT_EVT:
    case BTA_OPP_SERVER_GOEP_REQ_PUT_EVT:
    case BTA_OPP_SERVER_GOEP_REQ_DISCONNECT_EVT:
    case BTA_OPP_SERVER_GOEP_REQ_ABORT_EVT:
    case BTA_OPP_SERVER_GOEP_REQ_OTHER_EVT:
    case BTA_OPP_SERVER_GOEP_OPEN_EVT:
    case BTA_OPP_SERVER_GOEP_CLOSE_EVT:
    case BTA_OPP_SERVER_GOEP_MTU_EVT:
        p_ccb = bta_opp_find_ccb_by_goep_handle(BTA_OPP_ROLE_SERVER, p_msg->layer_specific);
        if (p_ccb == NULL) {
            if (p_data->goep.pkt) {
                osi_free(p_data->goep.pkt);
            }
            break;
        }
        bta_opp_server_sm_execute(p_ccb, p_msg->event, p_data);
        break;
    default:
        /* ACCEPT / REJECT / CANCEL: located by connection handle */
        p_ccb = bta_opp_find_ccb_by_handle(p_msg->layer_specific);
        if (p_ccb == NULL || p_ccb->role != BTA_OPP_ROLE_SERVER) {
            break;
        }
        bta_opp_server_sm_execute(p_ccb, p_msg->event, p_data);
        break;
    }
    return TRUE;
}

#endif /* BTA_OPP_SERVER_INCLUDED */
