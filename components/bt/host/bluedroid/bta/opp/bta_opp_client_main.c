/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "common/bt_target.h"
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#include "bta_opp_int.h"
#include "btc_opp.h"

#if BTA_OPP_CLIENT_INCLUDED

/* state machine states */
enum {
    BTA_OPP_CLIENT_INIT_ST,         /* freshly allocated ccb, not connected */
    BTA_OPP_CLIENT_OPENING_ST,      /* SDP discovery and transport opening */
    BTA_OPP_CLIENT_CONNECTING_ST,   /* OBEX CONNECT request sent, awaiting response */
    BTA_OPP_CLIENT_CONNECTED_ST,    /* OBEX connected, idle */
    BTA_OPP_CLIENT_PUTTING_ST,      /* object PUT in progress */
    BTA_OPP_CLIENT_CLOSING_ST,      /* OBEX DISCONNECT request sent, awaiting response */
};

/* state machine action enumeration list */
enum {
    BTA_OPP_CLIENT_ACT_CONNECT,
    BTA_OPP_CLIENT_ACT_DISCONNECT,
    BTA_OPP_CLIENT_ACT_OPEN_OBJECT,
    BTA_OPP_CLIENT_ACT_SEND_BUSY,
    BTA_OPP_CLIENT_ACT_CLOSE_FAIL,
    BTA_OPP_CLIENT_ACT_CLOSE_ABORT,
    BTA_OPP_CLIENT_ACT_DO_OPEN,
    BTA_OPP_CLIENT_ACT_SEND_CONNECT,
    BTA_OPP_CLIENT_ACT_CONNECT_RSP,
    BTA_OPP_CLIENT_ACT_PUT_RSP,
    BTA_OPP_CLIENT_ACT_CLOSE_RSP,
    BTA_OPP_CLIENT_ACT_FREE_RSP,
    BTA_OPP_CLIENT_ACT_CLOSED,
    BTA_OPP_CLIENT_ACT_TX_READY,
    BTA_OPP_CLIENT_ACT_RSP_TIMEOUT,
    BTA_OPP_CLIENT_NUM_ACTIONS
};

#define BTA_OPP_CLIENT_IGNORE       BTA_OPP_CLIENT_NUM_ACTIONS

/* forward declaration of action functions */
static void bta_opp_client_act_connect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_disconnect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_open_object(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_send_busy(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_close_fail(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_close_abort(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_do_open(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_send_connect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_connect_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_put_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_close_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_free_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_closed(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_tx_ready(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);
static void bta_opp_client_act_rsp_timeout(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);

/* type for action functions */
typedef void (*tBTA_OPP_CLIENT_ACTION)(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data);

/* action functions table, indexed with action enum */
static const tBTA_OPP_CLIENT_ACTION bta_opp_client_action[] = {
    bta_opp_client_act_connect,         /* BTA_OPP_CLIENT_ACT_CONNECT */
    bta_opp_client_act_disconnect,      /* BTA_OPP_CLIENT_ACT_DISCONNECT */
    bta_opp_client_act_open_object,     /* BTA_OPP_CLIENT_ACT_OPEN_OBJECT */
    bta_opp_client_act_send_busy,       /* BTA_OPP_CLIENT_ACT_SEND_BUSY */
    bta_opp_client_act_close_fail,      /* BTA_OPP_CLIENT_ACT_CLOSE_FAIL */
    bta_opp_client_act_close_abort,     /* BTA_OPP_CLIENT_ACT_CLOSE_ABORT */
    bta_opp_client_act_do_open,         /* BTA_OPP_CLIENT_ACT_DO_OPEN */
    bta_opp_client_act_send_connect,    /* BTA_OPP_CLIENT_ACT_SEND_CONNECT */
    bta_opp_client_act_connect_rsp,     /* BTA_OPP_CLIENT_ACT_CONNECT_RSP */
    bta_opp_client_act_put_rsp,         /* BTA_OPP_CLIENT_ACT_PUT_RSP */
    bta_opp_client_act_close_rsp,       /* BTA_OPP_CLIENT_ACT_CLOSE_RSP */
    bta_opp_client_act_free_rsp,        /* BTA_OPP_CLIENT_ACT_FREE_RSP */
    bta_opp_client_act_closed,          /* BTA_OPP_CLIENT_ACT_CLOSED */
    bta_opp_client_act_tx_ready,        /* BTA_OPP_CLIENT_ACT_TX_READY */
    bta_opp_client_act_rsp_timeout,     /* BTA_OPP_CLIENT_ACT_RSP_TIMEOUT */
};

/* state table information */
#define BTA_OPP_CLIENT_ACTION_COL       0       /* position of action */
#define BTA_OPP_CLIENT_NEXT_STATE_COL   1       /* position of next state */
#define BTA_OPP_CLIENT_NUM_COLS         2       /* number of columns */

/* State machine events are indexed by (event & 0xff), which yields the
 * following order for the events declared in bta_opp_client_int.h:
 *   0 CONNECT  1 DISCONNECT  2 OPEN_OBJECT  3 CANCEL
 *   4 DISC_RES 5 GOEP_OPEN   6 GOEP_RSP     7 GOEP_CLOSE
 *   8 TX_READY 9 RSP_TIMEOUT
 */
static const UINT8 bta_opp_client_st_init[][BTA_OPP_CLIENT_NUM_COLS] = {
    /* CONNECT */       {BTA_OPP_CLIENT_ACT_CONNECT,      BTA_OPP_CLIENT_OPENING_ST},
    /* DISCONNECT */    {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_INIT_ST},
    /* OPEN_OBJECT */   {BTA_OPP_CLIENT_ACT_SEND_BUSY,    BTA_OPP_CLIENT_INIT_ST},
    /* CANCEL */        {BTA_OPP_CLIENT_ACT_CLOSE_ABORT,  BTA_OPP_CLIENT_INIT_ST},
    /* DISC_RES */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_INIT_ST},
    /* GOEP_OPEN */     {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_INIT_ST},
    /* GOEP_RSP */      {BTA_OPP_CLIENT_ACT_FREE_RSP,     BTA_OPP_CLIENT_INIT_ST},
    /* GOEP_CLOSE */    {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_INIT_ST},
    /* TX_READY */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_INIT_ST},
    /* RSP_TIMEOUT */   {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_INIT_ST},
};

static const UINT8 bta_opp_client_st_opening[][BTA_OPP_CLIENT_NUM_COLS] = {
    /* CONNECT */       {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_OPENING_ST},
    /* DISCONNECT */    {BTA_OPP_CLIENT_ACT_CLOSE_FAIL,   BTA_OPP_CLIENT_INIT_ST},
    /* OPEN_OBJECT */   {BTA_OPP_CLIENT_ACT_SEND_BUSY,    BTA_OPP_CLIENT_OPENING_ST},
    /* CANCEL */        {BTA_OPP_CLIENT_ACT_CLOSE_ABORT,  BTA_OPP_CLIENT_INIT_ST},
    /* DISC_RES */      {BTA_OPP_CLIENT_ACT_DO_OPEN,      BTA_OPP_CLIENT_OPENING_ST},
    /* GOEP_OPEN */     {BTA_OPP_CLIENT_ACT_SEND_CONNECT, BTA_OPP_CLIENT_CONNECTING_ST},
    /* GOEP_RSP */      {BTA_OPP_CLIENT_ACT_FREE_RSP,     BTA_OPP_CLIENT_OPENING_ST},
    /* GOEP_CLOSE */    {BTA_OPP_CLIENT_ACT_CLOSED,       BTA_OPP_CLIENT_INIT_ST},
    /* TX_READY */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_OPENING_ST},
    /* RSP_TIMEOUT */   {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_OPENING_ST},
};

static const UINT8 bta_opp_client_st_connecting[][BTA_OPP_CLIENT_NUM_COLS] = {
    /* CONNECT */       {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTING_ST},
    /* DISCONNECT */    {BTA_OPP_CLIENT_ACT_CLOSE_FAIL,   BTA_OPP_CLIENT_INIT_ST},
    /* OPEN_OBJECT */   {BTA_OPP_CLIENT_ACT_SEND_BUSY,    BTA_OPP_CLIENT_CONNECTING_ST},
    /* CANCEL */        {BTA_OPP_CLIENT_ACT_CLOSE_ABORT,  BTA_OPP_CLIENT_INIT_ST},
    /* DISC_RES */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTING_ST},
    /* GOEP_OPEN */     {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTING_ST},
    /* GOEP_RSP */      {BTA_OPP_CLIENT_ACT_CONNECT_RSP,  BTA_OPP_CLIENT_CONNECTING_ST},
    /* GOEP_CLOSE */    {BTA_OPP_CLIENT_ACT_CLOSED,       BTA_OPP_CLIENT_INIT_ST},
    /* TX_READY */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTING_ST},
    /* RSP_TIMEOUT */   {BTA_OPP_CLIENT_ACT_RSP_TIMEOUT,  BTA_OPP_CLIENT_INIT_ST},
};

static const UINT8 bta_opp_client_st_connected[][BTA_OPP_CLIENT_NUM_COLS] = {
    /* CONNECT */       {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTED_ST},
    /* DISCONNECT */    {BTA_OPP_CLIENT_ACT_DISCONNECT,   BTA_OPP_CLIENT_CLOSING_ST},
    /* OPEN_OBJECT */   {BTA_OPP_CLIENT_ACT_OPEN_OBJECT,  BTA_OPP_CLIENT_PUTTING_ST},
    /* CANCEL */        {BTA_OPP_CLIENT_ACT_CLOSE_ABORT,  BTA_OPP_CLIENT_INIT_ST},
    /* DISC_RES */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTED_ST},
    /* GOEP_OPEN */     {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTED_ST},
    /* GOEP_RSP */      {BTA_OPP_CLIENT_ACT_FREE_RSP,     BTA_OPP_CLIENT_CONNECTED_ST},
    /* GOEP_CLOSE */    {BTA_OPP_CLIENT_ACT_CLOSED,       BTA_OPP_CLIENT_INIT_ST},
    /* TX_READY */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTED_ST},
    /* RSP_TIMEOUT */   {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CONNECTED_ST},
};

static const UINT8 bta_opp_client_st_putting[][BTA_OPP_CLIENT_NUM_COLS] = {
    /* CONNECT */       {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_PUTTING_ST},
    /* DISCONNECT */    {BTA_OPP_CLIENT_ACT_CLOSE_ABORT,  BTA_OPP_CLIENT_INIT_ST},
    /* OPEN_OBJECT */   {BTA_OPP_CLIENT_ACT_SEND_BUSY,    BTA_OPP_CLIENT_PUTTING_ST},
    /* CANCEL */        {BTA_OPP_CLIENT_ACT_CLOSE_ABORT,  BTA_OPP_CLIENT_INIT_ST},
    /* DISC_RES */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_PUTTING_ST},
    /* GOEP_OPEN */     {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_PUTTING_ST},
    /* GOEP_RSP */      {BTA_OPP_CLIENT_ACT_PUT_RSP,      BTA_OPP_CLIENT_PUTTING_ST},
    /* GOEP_CLOSE */    {BTA_OPP_CLIENT_ACT_CLOSED,       BTA_OPP_CLIENT_INIT_ST},
    /* TX_READY */      {BTA_OPP_CLIENT_ACT_TX_READY,     BTA_OPP_CLIENT_PUTTING_ST},
    /* RSP_TIMEOUT */   {BTA_OPP_CLIENT_ACT_RSP_TIMEOUT,  BTA_OPP_CLIENT_INIT_ST},
};

static const UINT8 bta_opp_client_st_closing[][BTA_OPP_CLIENT_NUM_COLS] = {
    /* CONNECT */       {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CLOSING_ST},
    /* DISCONNECT */    {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CLOSING_ST},
    /* OPEN_OBJECT */   {BTA_OPP_CLIENT_ACT_SEND_BUSY,    BTA_OPP_CLIENT_CLOSING_ST},
    /* CANCEL */        {BTA_OPP_CLIENT_ACT_CLOSE_ABORT,  BTA_OPP_CLIENT_INIT_ST},
    /* DISC_RES */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CLOSING_ST},
    /* GOEP_OPEN */     {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CLOSING_ST},
    /* GOEP_RSP */      {BTA_OPP_CLIENT_ACT_CLOSE_RSP,    BTA_OPP_CLIENT_INIT_ST},
    /* GOEP_CLOSE */    {BTA_OPP_CLIENT_ACT_CLOSED,       BTA_OPP_CLIENT_INIT_ST},
    /* TX_READY */      {BTA_OPP_CLIENT_IGNORE,           BTA_OPP_CLIENT_CLOSING_ST},
    /* RSP_TIMEOUT */   {BTA_OPP_CLIENT_ACT_RSP_TIMEOUT,  BTA_OPP_CLIENT_INIT_ST},
};

/* type for state table */
typedef const UINT8 (*tBTA_OPP_CLIENT_ST_TBL)[BTA_OPP_CLIENT_NUM_COLS];

/* state table, indexed with state enum */
static const tBTA_OPP_CLIENT_ST_TBL bta_opp_client_st_tbl[] = {
    bta_opp_client_st_init,
    bta_opp_client_st_opening,
    bta_opp_client_st_connecting,
    bta_opp_client_st_connected,
    bta_opp_client_st_putting,
    bta_opp_client_st_closing,
};

/******************************************************************************
 * Helpers
 *****************************************************************************/

static void bta_opp_client_stop_rsp_timer(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb != NULL && p_ccb->rsp_timer_on) {
        bta_sys_stop_timer(&p_ccb->rsp_timer);
        p_ccb->rsp_timer_on = FALSE;
    }
}

static void bta_opp_client_rsp_timer_cback(TIMER_LIST_ENT *p_tle)
{
    tBTA_OPP_CCB *p_ccb;
    BT_HDR *p_buf;

    if (p_tle == NULL) {
        return;
    }

    p_ccb = (tBTA_OPP_CCB *)p_tle->param;
    if (p_ccb == NULL || !p_ccb->allocated || p_ccb->role != BTA_OPP_ROLE_CLIENT) {
        return;
    }

    p_ccb->rsp_timer_on = FALSE;
    if (p_ccb->state != BTA_OPP_CLIENT_CONNECTING_ST &&
        p_ccb->state != BTA_OPP_CLIENT_PUTTING_ST &&
        p_ccb->state != BTA_OPP_CLIENT_CLOSING_ST) {
        return;
    }

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_CLIENT_RSP_TIMEOUT_EVT;
        p_buf->layer_specific = p_ccb->handle;
        bta_sys_sendmsg(p_buf);
    }
}

static void bta_opp_client_start_rsp_timer(tBTA_OPP_CCB *p_ccb)
{
    bta_opp_client_stop_rsp_timer(p_ccb);
    p_ccb->rsp_timer.p_cback = (TIMER_CBACK *)&bta_opp_client_rsp_timer_cback;
    p_ccb->rsp_timer.param = (TIMER_PARAM_TYPE)p_ccb;
    bta_sys_start_timer(&p_ccb->rsp_timer, 0, BTA_OPP_CLIENT_RSP_TIMEOUT_MS);
    p_ccb->rsp_timer_on = TRUE;
}

void bta_opp_client_cleanup_ccb(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL) {
        return;
    }
    bta_opp_client_stop_rsp_timer(p_ccb);
    bta_sys_free_timer(&p_ccb->rsp_timer);
}

static BOOLEAN ascii_to_utf16be(const char *src, UINT8 *dst, UINT16 dst_size, UINT16 *out_len)
{
    size_t src_len;
    size_t need;
    UINT16 pos = 0;

    if (src == NULL || dst == NULL || out_len == NULL) {
        return FALSE;
    }

    src_len = strlen(src) + 1;
    need = src_len * 2;
    if (need > dst_size || need > UINT16_MAX) {
        return FALSE;
    }

    for (size_t i = 0; i < src_len; i++) {
        dst[pos++] = 0;
        dst[pos++] = (UINT8)src[i];
    }
    *out_len = pos;
    return TRUE;
}

static UINT16 add_name_header(UINT16 goep_handle, const char *name)
{
    size_t name_len;
    size_t utf16_size;
    UINT16 utf16_len = 0;
    UINT8 *utf16;
    UINT16 ret;

    if (name == NULL) {
        return GOEP_SUCCESS;
    }

    name_len = strlen(name);
    if (name_len > OPP_MAX_NAME_LEN) {
        return GOEP_INVALID_PARAM;
    }

    utf16_size = (name_len + 1) * 2;
    utf16 = (UINT8 *)osi_malloc(utf16_size);
    if (utf16 == NULL) {
        return GOEP_NO_RESOURCES;
    }

    if (!ascii_to_utf16be(name, utf16, (UINT16)utf16_size, &utf16_len)) {
        osi_free(utf16);
        return GOEP_INVALID_PARAM;
    }

    ret = GOEPC_RequestAddHeader(goep_handle, OBEX_HEADER_ID_NAME, utf16, utf16_len);
    osi_free(utf16);
    return ret;
}

static UINT16 append_u32_header(UINT16 goep_handle, UINT8 header_id, UINT32 value)
{
    UINT8 buf[4];
    UINT8 *p = buf;
    UINT32_TO_BE_STREAM(p, value);
    return GOEPC_RequestAddHeader(goep_handle, header_id, buf, sizeof(buf));
}

static void bta_opp_client_free_send_param(tBTA_OPP_SEND_PARAM *param)
{
    if (param->name) {
        osi_free(param->name);
        param->name = NULL;
    }
    if (param->type) {
        osi_free(param->type);
        param->type = NULL;
    }
}

static void free_client_transfer_state(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb->name) {
        osi_free(p_ccb->name);
        p_ccb->name = NULL;
    }
    if (p_ccb->type) {
        osi_free(p_ccb->type);
        p_ccb->type = NULL;
    }
    p_ccb->tx_len = 0;
    p_ccb->tx_offset = 0;
    p_ccb->tx_waiting = FALSE;
    p_ccb->total_len = 0;
    p_ccb->transferred = 0;
}

static BOOLEAN send_connect_request(tBTA_OPP_CCB *p_ccb)
{
    tOBEX_PARSE_INFO info = {0};
    UINT16 ret;

    info.opcode = OBEX_OPCODE_CONNECT;
    info.obex_version_number = OBEX_VERSION_NUMBER;
    info.flags = 0;
    info.max_packet_length = p_ccb->max_rx;
    ret = GOEPC_PrepareRequest(p_ccb->goep_handle, &info, p_ccb->max_tx);
    if (ret != GOEP_SUCCESS) {
        return FALSE;
    }

    if (GOEPC_SendRequest(p_ccb->goep_handle) != GOEP_SUCCESS) {
        return FALSE;
    }
    bta_opp_client_start_rsp_timer(p_ccb);
    return TRUE;
}

static BOOLEAN send_disconnect_request(tBTA_OPP_CCB *p_ccb)
{
    tOBEX_PARSE_INFO info = {0};
    UINT16 ret;

    info.opcode = OBEX_OPCODE_DISCONNECT;
    ret = GOEPC_PrepareRequest(p_ccb->goep_handle, &info, p_ccb->max_tx);
    if (ret != GOEP_SUCCESS) {
        return FALSE;
    }

    if (GOEPC_SendRequest(p_ccb->goep_handle) != GOEP_SUCCESS) {
        return FALSE;
    }
    bta_opp_client_start_rsp_timer(p_ccb);
    return TRUE;
}

static BOOLEAN estimate_put_overhead(tBTA_OPP_CCB *p_ccb, UINT16 *out_overhead)
{
    size_t overhead = 3;

    if (out_overhead == NULL) {
        return FALSE;
    }

    if (p_ccb->tx_offset == 0) {
        if (p_ccb->name) {
            size_t name_len = strlen(p_ccb->name);
            if (name_len > OPP_MAX_NAME_LEN) {
                return FALSE;
            }
            overhead += 3 + (name_len + 1) * 2;
        }
        if (p_ccb->type) {
            size_t type_len = strlen(p_ccb->type);
            if (type_len > OPP_MAX_TYPE_LEN) {
                return FALSE;
            }
            overhead += 3 + type_len + 1;
        }
        /* Length is TYPE4: 1-byte HI + 4-byte value. */
        overhead += 5;
    }
    if (overhead > UINT16_MAX) {
        return FALSE;
    }
    *out_overhead = (UINT16)overhead;
    return TRUE;
}

/* Return values for streaming PUT helper. */
#define OPP_PUT_OK       0
#define OPP_PUT_WAIT     1
#define OPP_PUT_FAIL     2

static int send_next_put(tBTA_OPP_CCB *p_ccb)
{
    tOBEX_PARSE_INFO info = {0};
    UINT16 ret;
    UINT16 overhead;
    UINT16 chunk;
    UINT8 body_id;
    UINT8 *body_buf = NULL;
    UINT16 pulled = 0;
    bool is_final = false;
    bool waiting = false;

    if (!estimate_put_overhead(p_ccb, &overhead) ||
            p_ccb->max_tx <= overhead || (p_ccb->max_tx - overhead) <= 3) {
        return OPP_PUT_FAIL;
    }
    chunk = p_ccb->max_tx - overhead - 3;

    body_buf = (UINT8 *)osi_malloc(chunk ? chunk : 1);
    if (body_buf == NULL) {
        return OPP_PUT_FAIL;
    }

    if (btc_opp_vfs_pull_tx(p_ccb->handle, body_buf, chunk, &pulled, &is_final, &waiting) != 0) {
        osi_free(body_buf);
        return OPP_PUT_FAIL;
    }
    if (waiting) {
        osi_free(body_buf);
        /* Waiting on app write, not peer response. */
        bta_opp_client_stop_rsp_timer(p_ccb);
        p_ccb->tx_waiting = TRUE;
        return OPP_PUT_WAIT;
    }

    /* Final empty body is valid when declared length is 0. */
    body_id = is_final ? OBEX_HEADER_ID_END_OF_BODY : OBEX_HEADER_ID_BODY;
    if (!is_final && pulled == 0) {
        osi_free(body_buf);
        return OPP_PUT_FAIL;
    }
    info.opcode = is_final ? OBEX_OPCODE_PUT_FINAL : OBEX_OPCODE_PUT;
    ret = GOEPC_PrepareRequest(p_ccb->goep_handle, &info, p_ccb->max_tx);
    if (ret != GOEP_SUCCESS) {
        osi_free(body_buf);
        return OPP_PUT_FAIL;
    }

    if (p_ccb->tx_offset == 0) {
        ret |= add_name_header(p_ccb->goep_handle, p_ccb->name);
        if (p_ccb->type) {
            ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_TYPE,
                                          (const UINT8 *)p_ccb->type, strlen(p_ccb->type) + 1);
        }
        ret |= append_u32_header(p_ccb->goep_handle, OBEX_HEADER_ID_LENGTH, p_ccb->tx_len);
        if (ret != GOEP_SUCCESS) {
            GOEPC_DropRequest(p_ccb->goep_handle);
            osi_free(body_buf);
            return OPP_PUT_FAIL;
        }
    }

    /* GOEP requires data==NULL when data_len==0. */
    ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, body_id,
                                  pulled ? body_buf : NULL, pulled);
    osi_free(body_buf);
    if (ret != GOEP_SUCCESS) {
        GOEPC_DropRequest(p_ccb->goep_handle);
        return OPP_PUT_FAIL;
    }

    p_ccb->tx_offset += pulled;
    p_ccb->transferred = p_ccb->tx_offset;
    p_ccb->tx_waiting = FALSE;
    if (GOEPC_SendRequest(p_ccb->goep_handle) != GOEP_SUCCESS) {
        return OPP_PUT_FAIL;
    }
    bta_opp_client_start_rsp_timer(p_ccb);
    bta_opp_report_progress(p_ccb);
    return OPP_PUT_OK;
}

static void bta_opp_client_goep_cback(UINT16 handle, UINT8 event, tGOEPC_MSG *p_msg)
{
    tBTA_OPP_CLIENT_GOEP_MSG *p_buf = NULL;

    switch (event) {
    case GOEPC_OPENED_EVT:
        p_buf = (tBTA_OPP_CLIENT_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_CLIENT_GOEP_MSG));
        if (p_buf) {
            memset(p_buf, 0, sizeof(tBTA_OPP_CLIENT_GOEP_MSG));
            p_buf->hdr.event = BTA_OPP_CLIENT_GOEP_OPEN_EVT;
            p_buf->hdr.layer_specific = handle;
            p_buf->peer_mtu = p_msg->opened.peer_mtu;
            p_buf->our_mtu = p_msg->opened.our_mtu;
        }
        break;
    case GOEPC_CLOSED_EVT:
        p_buf = (tBTA_OPP_CLIENT_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_CLIENT_GOEP_MSG));
        if (p_buf) {
            memset(p_buf, 0, sizeof(tBTA_OPP_CLIENT_GOEP_MSG));
            p_buf->hdr.event = BTA_OPP_CLIENT_GOEP_CLOSE_EVT;
            p_buf->hdr.layer_specific = handle;
            p_buf->reason = p_msg->closed.reason;
        }
        break;
    case GOEPC_RESPONSE_EVT:
        p_buf = (tBTA_OPP_CLIENT_GOEP_MSG *)osi_malloc(sizeof(tBTA_OPP_CLIENT_GOEP_MSG));
        if (p_buf) {
            memset(p_buf, 0, sizeof(tBTA_OPP_CLIENT_GOEP_MSG));
            p_buf->hdr.event = BTA_OPP_CLIENT_GOEP_RSP_EVT;
            p_buf->hdr.layer_specific = handle;
            p_buf->opcode = p_msg->response.opcode;
            p_buf->final = p_msg->response.final;
            p_buf->pkt = p_msg->response.pkt;
        } else if (p_msg && p_msg->response.pkt) {
            osi_free(p_msg->response.pkt);
        }
        break;
    case GOEPC_MTU_CHANGED_EVT:
    case GOEPC_CONGEST_EVT:
    case GOEPC_UNCONGEST_EVT:
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

static void bta_opp_client_act_connect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    tBTA_OPP_CONNECT_PARAM *param = &p_data->api_connect.param;

    p_ccb->role = BTA_OPP_ROLE_CLIENT;
    p_ccb->sec_mask = param->sec_mask;
    p_ccb->max_tx = (param->mtu >= OPP_MIN_MTU) ? param->mtu : OPP_DEFAULT_MTU;
    p_ccb->max_rx = p_ccb->max_tx;
    bdcpy(p_ccb->bd_addr, param->bd_addr);

    if (!bta_opp_client_do_disc(p_ccb)) {
        bta_opp_report_conn(p_ccb, FALSE, BTA_OPP_SDP_FAIL);
        bta_opp_free_ccb(p_ccb);
    }
}

static void bta_opp_client_act_disconnect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    if (!send_disconnect_request(p_ccb)) {
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
    }
}

static void bta_opp_client_act_open_object(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    tBTA_OPP_SEND_PARAM *param = &p_data->api_send.param;
    int put_ret;

    p_ccb->name = param->name;
    p_ccb->type = param->type;
    p_ccb->tx_len = param->len;
    p_ccb->total_len = param->len;
    p_ccb->tx_offset = 0;
    p_ccb->transferred = 0;
    p_ccb->tx_waiting = FALSE;

    bta_opp_pm_busy(p_ccb);
    put_ret = send_next_put(p_ccb);
    if (put_ret == OPP_PUT_FAIL) {
        bta_opp_report_complete(p_ccb, BTA_OPP_OBEX_FAIL);
        free_client_transfer_state(p_ccb);
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
    }
}

static void bta_opp_client_act_send_busy(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    bta_opp_client_free_send_param(&p_data->api_send.param);
    bta_opp_report_complete(p_ccb, BTA_OPP_BUSY);
}

static void bta_opp_client_act_tx_ready(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    int put_ret;

    UNUSED(p_data);
    if (!p_ccb->tx_waiting) {
        return;
    }

    put_ret = send_next_put(p_ccb);
    if (put_ret == OPP_PUT_FAIL) {
        bta_opp_report_complete(p_ccb, BTA_OPP_OBEX_FAIL);
        free_client_transfer_state(p_ccb);
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
    }
}

static void bta_opp_client_act_close_fail(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    UNUSED(p_data);
    bta_opp_close_connection(p_ccb, BTA_OPP_FAIL, TRUE);
}

static void bta_opp_client_act_close_abort(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    UNUSED(p_data);
    if (p_ccb->name != NULL || p_ccb->tx_len != 0 || p_ccb->tx_offset != 0 || p_ccb->tx_waiting) {
        bta_opp_report_complete(p_ccb, BTA_OPP_ABORTED);
        free_client_transfer_state(p_ccb);
    }
    bta_opp_close_connection(p_ccb, BTA_OPP_ABORTED, TRUE);
}

static void bta_opp_client_act_do_open(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    tOBEX_SVR_INFO server = {0};

    if (p_data->disc_result.status != SDP_SUCCESS || !bta_opp_client_sdp_find_attr(p_ccb) ||
        p_ccb->peer_rfcomm_scn == 0) {
        bta_opp_close_connection(p_ccb, BTA_OPP_SDP_FAIL, TRUE);
        return;
    }
    bta_opp_client_free_sdp_db(p_ccb);

    if (p_ccb->peer_l2cap_psm != 0) {
        server.tl = OBEX_OVER_L2CAP;
        server.l2cap.psm = p_ccb->peer_l2cap_psm;
        server.l2cap.sec_mask = p_ccb->sec_mask;
        server.l2cap.pref_mtu = p_ccb->max_tx;
        bdcpy(server.l2cap.addr, p_ccb->bd_addr);
    } else {
        server.tl = OBEX_OVER_RFCOMM;
        server.rfcomm.scn = p_ccb->peer_rfcomm_scn;
        server.rfcomm.sec_mask = p_ccb->sec_mask;
        server.rfcomm.pref_mtu = p_ccb->max_tx;
        bdcpy(server.rfcomm.addr, p_ccb->bd_addr);
    }

    if (GOEPC_Open(&server, bta_opp_client_goep_cback, &p_ccb->goep_handle) != GOEP_SUCCESS) {
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
    }
}

static void bta_opp_client_act_send_connect(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    if (p_ccb->max_rx > p_data->goep.our_mtu) {
        p_ccb->max_rx = p_data->goep.our_mtu;
    }
    if (p_ccb->max_tx > p_data->goep.peer_mtu) {
        p_ccb->max_tx = p_data->goep.peer_mtu;
    }
    if (!send_connect_request(p_ccb)) {
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
    }
}

static void bta_opp_client_act_connect_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    tOBEX_PARSE_INFO info = {0};
    UINT16 parse_ret;

    bta_opp_client_stop_rsp_timer(p_ccb);
    parse_ret = OBEX_ParseResponse(p_data->goep.pkt, OBEX_OPCODE_CONNECT, &info);
    osi_free(p_data->goep.pkt);
    if (parse_ret != OBEX_SUCCESS ||
        info.response_code != (OBEX_RESPONSE_CODE_OK | OBEX_FINAL_BIT_MASK)) {
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
        return;
    }
    if (info.max_packet_length >= OPP_MIN_MTU && p_ccb->max_tx > info.max_packet_length) {
        p_ccb->max_tx = info.max_packet_length;
    }
    p_ccb->state = BTA_OPP_CLIENT_CONNECTED_ST;
    bta_opp_report_conn(p_ccb, TRUE, BTA_OPP_OK);
}

static void bta_opp_client_act_put_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    tOBEX_PARSE_INFO info = {0};
    UINT16 parse_ret;
    int put_ret;

    bta_opp_client_stop_rsp_timer(p_ccb);
    parse_ret = OBEX_ParseResponse(p_data->goep.pkt, p_data->goep.opcode, &info);
    osi_free(p_data->goep.pkt);
    if (parse_ret != OBEX_SUCCESS) {
        bta_opp_report_complete(p_ccb, BTA_OPP_OBEX_FAIL);
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
        return;
    }
    if (info.response_code == (OBEX_RESPONSE_CODE_CONTINUE | OBEX_FINAL_BIT_MASK) ||
        info.response_code == OBEX_RESPONSE_CODE_CONTINUE) {
        put_ret = send_next_put(p_ccb);
        if (put_ret == OPP_PUT_FAIL) {
            bta_opp_report_complete(p_ccb, BTA_OPP_OBEX_FAIL);
            bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
        }
    } else if (info.response_code == (OBEX_RESPONSE_CODE_OK | OBEX_FINAL_BIT_MASK)) {
        bta_opp_report_complete(p_ccb, BTA_OPP_OK);
        free_client_transfer_state(p_ccb);
        p_ccb->state = BTA_OPP_CLIENT_CONNECTED_ST;
    } else {
        bta_opp_report_complete(p_ccb, BTA_OPP_OBEX_FAIL);
        bta_opp_close_connection(p_ccb, BTA_OPP_OBEX_FAIL, TRUE);
    }
}

static void bta_opp_client_act_close_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    bta_opp_client_stop_rsp_timer(p_ccb);
    if (p_data->goep.pkt) {
        osi_free(p_data->goep.pkt);
    }
    bta_opp_close_connection(p_ccb, BTA_OPP_OK, TRUE);
}

static void bta_opp_client_act_rsp_timeout(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    UNUSED(p_data);
    bta_opp_client_stop_rsp_timer(p_ccb);
    /* SM may already have moved to INIT; detect in-flight PUT via transfer fields. */
    if (p_ccb->name != NULL || p_ccb->tx_len != 0 || p_ccb->tx_offset != 0 || p_ccb->tx_waiting) {
        bta_opp_report_complete(p_ccb, BTA_OPP_ABORTED);
        free_client_transfer_state(p_ccb);
    }
    bta_opp_close_connection(p_ccb, BTA_OPP_ABORTED, TRUE);
}

static void bta_opp_client_act_free_rsp(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    UNUSED(p_ccb);
    if (p_data->goep.pkt) {
        osi_free(p_data->goep.pkt);
    }
}

static void bta_opp_client_act_closed(tBTA_OPP_CCB *p_ccb, tBTA_OPP_CLIENT_DATA *p_data)
{
    /* Unexpected transport close (connect/idle): FAIL, aligned with CLOSE_FAIL.
     * Local disconnect completing via GOEP_CLOSE: hdl_event sets goep.final. */
    tBTA_OPP_STATUS status = (p_data != NULL && p_data->goep.final) ? BTA_OPP_OK : BTA_OPP_FAIL;

    bta_opp_client_stop_rsp_timer(p_ccb);
    p_ccb->goep_handle = 0;
    /* Peer/link drop during PUT: match RSP_TIMEOUT / CANCEL / DISCONNECT semantics. */
    if (p_ccb->name != NULL || p_ccb->tx_len != 0 || p_ccb->tx_offset != 0 || p_ccb->tx_waiting) {
        bta_opp_report_complete(p_ccb, BTA_OPP_ABORTED);
        free_client_transfer_state(p_ccb);
        status = BTA_OPP_ABORTED;
    }
    bta_opp_report_conn(p_ccb, FALSE, status);
    bta_opp_free_ccb(p_ccb);
}

/******************************************************************************
 * Module (non state machine) handlers
 *****************************************************************************/

static void bta_opp_client_api_enable(tBTA_OPP_CLIENT_DATA *p_data)
{
    if (!bta_opp_cb.client_registered) {
        if (bta_opp_cb.p_cback == NULL) {
            bta_opp_cb.p_cback = p_data->api_enable.p_cback;
        }
        bta_opp_cb.client_registered = TRUE;
        if (bta_opp_cb.p_cback) {
            bta_opp_cb.p_cback(BTA_OPP_CLIENT_ENABLE_EVT, NULL);
        }
    }
}

static void bta_opp_client_api_disable(tBTA_OPP_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    for (int i = 0; i < BTA_OPP_MAX_CONNECTION; i++) {
        if (bta_opp_cb.ccb[i].allocated && bta_opp_cb.ccb[i].role == BTA_OPP_ROLE_CLIENT) {
            bta_opp_close_connection(&bta_opp_cb.ccb[i], BTA_OPP_FAIL, TRUE);
        }
    }

    tBTA_OPP_CBACK *p_cback = bta_opp_cb.p_cback;
    bta_opp_cb.client_registered = FALSE;
#if BTA_OPP_SERVER_INCLUDED
    if (!bta_opp_cb.server_registered) {
        bta_opp_cb.p_cback = NULL;
    }
#else
    bta_opp_cb.p_cback = NULL;
#endif
    if (bta_sys_is_register(BTA_ID_OPC)) {
        bta_sys_deregister(BTA_ID_OPC);
    }
    if (p_cback) {
        p_cback(BTA_OPP_CLIENT_DISABLE_EVT, NULL);
    }
}

/******************************************************************************
 * State machine engine and event dispatcher
 *****************************************************************************/

static void bta_opp_client_sm_execute(tBTA_OPP_CCB *p_ccb, UINT16 event, tBTA_OPP_CLIENT_DATA *p_data)
{
    tBTA_OPP_CLIENT_ST_TBL state_table;
    UINT8 action;

    state_table = bta_opp_client_st_tbl[p_ccb->state];

    event &= 0xff;

    p_ccb->state = state_table[event][BTA_OPP_CLIENT_NEXT_STATE_COL];

    if ((action = state_table[event][BTA_OPP_CLIENT_ACTION_COL]) != BTA_OPP_CLIENT_IGNORE) {
        (*bta_opp_client_action[action])(p_ccb, p_data);
    }
}

BOOLEAN bta_opp_client_hdl_event(BT_HDR *p_msg)
{
    tBTA_OPP_CLIENT_DATA *p_data = (tBTA_OPP_CLIENT_DATA *)p_msg;
    tBTA_OPP_CCB *p_ccb = NULL;
    BOOLEAN execute_sm = FALSE;

    switch (p_msg->event) {
    case BTA_OPP_CLIENT_API_ENABLE_EVT:
        bta_opp_client_api_enable(p_data);
        break;
    case BTA_OPP_CLIENT_API_DISABLE_EVT:
        bta_opp_client_api_disable(p_data);
        break;
    case BTA_OPP_CLIENT_API_CONNECT_EVT:
        p_ccb = bta_opp_allocate_ccb();
        if (p_ccb == NULL) {
            if (bta_opp_cb.p_cback) {
                tBTA_OPP data;
                memset(&data, 0, sizeof(data));
                data.conn.handle = 0;
                data.conn.role = BTA_OPP_ROLE_CLIENT;
                data.conn.status = BTA_OPP_NO_RESOURCE;
                data.conn.connected = FALSE;
                bdcpy(data.conn.bd_addr, p_data->api_connect.param.bd_addr);
                bta_opp_cb.p_cback(BTA_OPP_CONN_CLOSE_EVT, &data);
            }
            break;
        }
        execute_sm = TRUE;
        break;
    case BTA_OPP_CLIENT_API_OPEN_OBJECT_EVT:
        p_ccb = bta_opp_find_ccb_by_handle(p_data->api_send.param.handle);
        if (p_ccb == NULL || p_ccb->role != BTA_OPP_ROLE_CLIENT) {
            bta_opp_client_free_send_param(&p_data->api_send.param);
            break;
        }
        execute_sm = TRUE;
        break;
    case BTA_OPP_CLIENT_GOEP_OPEN_EVT:
    case BTA_OPP_CLIENT_GOEP_RSP_EVT:
    case BTA_OPP_CLIENT_GOEP_CLOSE_EVT:
        p_ccb = bta_opp_find_ccb_by_goep_handle(BTA_OPP_ROLE_CLIENT, p_msg->layer_specific);
        if (p_ccb == NULL) {
            if (p_msg->event == BTA_OPP_CLIENT_GOEP_RSP_EVT && p_data->goep.pkt) {
                osi_free(p_data->goep.pkt);
            }
            break;
        }
        if (p_msg->event == BTA_OPP_CLIENT_GOEP_CLOSE_EVT) {
            /* SM moves to INIT before act_closed; remember local disconnect. */
            p_data->goep.final = (p_ccb->state == BTA_OPP_CLIENT_CLOSING_ST);
        }
        execute_sm = TRUE;
        break;
    default:
        /* DISCONNECT / CANCEL / DISC_RES: located by connection handle */
        p_ccb = bta_opp_find_ccb_by_handle(p_msg->layer_specific);
        if (p_ccb == NULL || p_ccb->role != BTA_OPP_ROLE_CLIENT) {
            break;
        }
        execute_sm = TRUE;
        break;
    }

    if (execute_sm) {
        bta_opp_client_sm_execute(p_ccb, p_msg->event, p_data);
    }

    return TRUE;
}

#endif /* BTA_OPP_CLIENT_INCLUDED */
