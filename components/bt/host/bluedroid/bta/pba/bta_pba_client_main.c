/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#include "bta_pba_client_int.h"

#if BTA_PBA_CLIENT_INCLUDED

/* state machine states */
enum {
    BTA_PBA_CLIENT_INIT_ST,
    BTA_PBA_CLIENT_OPENING_ST,
    BTA_PBA_CLIENT_OPENED_ST,
    BTA_PBA_CLIENT_REQUESTING_ST,
    BTA_PBA_CLIENT_CLOSING_ST
};

/* state machine action enumeration list */
enum {
    BTA_PBA_CLIENT_API_OPEN,
    BTA_PBA_CLIENT_API_CLOSE,
    BTA_PBA_CLIENT_API_REQ,
    BTA_PBA_CLIENT_DO_CONNECT,
    BTA_PBA_CLIENT_AUTHENTICATE,
    BTA_PBA_CLIENT_CONNECT,
    BTA_PBA_CLIENT_RESPONSE,
    BTA_PBA_CLIENT_RESPONSE_FINAL,
    BTA_PBA_CLIENT_GOEP_CONNECT,
    BTA_PBA_CLIENT_GOEP_DISCONNECT,
    BTA_PBA_CLIENT_FORCE_DISCONNECT,
    BTA_PBA_CLIENT_FREE_RESPONSE,
    BTA_PBA_CLIENT_NUM_ACTIONS
};

#define BTA_PBA_CLIENT_IGNORE       BTA_PBA_CLIENT_NUM_ACTIONS

/* type for action functions */
typedef void (*tBTA_PBA_CLIENT_ACTION)(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data);

/* action functions table, indexed with action enum */
const tBTA_PBA_CLIENT_ACTION bta_pba_client_action[] = {
    /* BTA_PBA_CLIENT_API_OPEN */      bta_pba_client_api_open,
    /* BTA_PBA_CLIENT_API_CLOSE */    bta_pba_client_api_close,
    /* BTA_PBA_CLIENT_API_REQ */   bta_pba_client_api_req,
    /* BTA_PBA_CLIENT_DO_CONNECT */  bta_pba_client_do_connect,
    /* BTA_PBA_CLIENT_AUTHENTICATE */   bta_pba_client_authenticate,
    /* BTA_PBA_CLIENT_CONNECT */    bta_pba_client_connect,
    /* BTA_PBA_CLIENT_RESPONSE */  bta_pba_client_response,
    /* BTA_PBA_CLIENT_RESPONSE_FINAL */    bta_pba_client_response_final,
    /* BTA_PBA_CLIENT_GOEP_CONNECT */ bta_pba_client_goep_connect,
    /* BTA_PBA_CLIENT_GOEP_DISCONNECT*/ bta_pba_client_goep_disconnect,
    /* BTA_PBA_CLIENT_FORCE_DISCONNECT */      bta_pba_client_force_disconnect,
    /* BTA_PBA_CLIENT_FREE_RESPONSE */     bta_pba_client_free_response,
};

/* state table information */
#define BTA_PBA_CLIENT_ACTION       0       /* position of action */
#define BTA_PBA_CLIENT_NEXT_STATE   1       /* position of next state */
#define BTA_PBA_CLIENT_NUM_COLS     2       /* number of columns */

const uint8_t bta_pba_client_st_init[][BTA_PBA_CLIENT_NUM_COLS] = {
    /* Event                                    Action                              Next state */
    /* BTA_PBA_CLIENT_API_OPEN_EVT */           {BTA_PBA_CLIENT_API_OPEN,           BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_API_CLOSE_EVT */          {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_API_REQ_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_DISC_RES_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_AUTHENTICATE_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_CONNECT_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_RESPONSE_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_RESPONSE_FINAL_EVT */     {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_GOEP_CONNECT_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT */    {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_INIT_ST},
};

const uint8_t bta_pba_client_st_opening[][BTA_PBA_CLIENT_NUM_COLS] = {
    /* Event                                    Action                              Next state */
    /* BTA_PBA_CLIENT_API_OPEN_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_API_CLOSE_EVT */          {BTA_PBA_CLIENT_FORCE_DISCONNECT,   BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_API_REQ_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_DISC_RES_EVT */           {BTA_PBA_CLIENT_DO_CONNECT,         BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_AUTHENTICATE_EVT */       {BTA_PBA_CLIENT_AUTHENTICATE,       BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_CONNECT_EVT */            {BTA_PBA_CLIENT_CONNECT,            BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_RESPONSE_EVT */           {BTA_PBA_CLIENT_FREE_RESPONSE,      BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_RESPONSE_FINAL_EVT */     {BTA_PBA_CLIENT_RESPONSE_FINAL,     BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_GOEP_CONNECT_EVT */       {BTA_PBA_CLIENT_GOEP_CONNECT,       BTA_PBA_CLIENT_OPENING_ST},
    /* BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT */    {BTA_PBA_CLIENT_GOEP_DISCONNECT,    BTA_PBA_CLIENT_INIT_ST},
};

const uint8_t bta_pba_client_st_opened[][BTA_PBA_CLIENT_NUM_COLS] = {
    /* Event                                    Action                              Next state */
    /* BTA_PBA_CLIENT_API_OPEN_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_API_CLOSE_EVT */          {BTA_PBA_CLIENT_API_CLOSE,          BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_API_REQ_EVT */            {BTA_PBA_CLIENT_API_REQ,            BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_DISC_RES_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_AUTHENTICATE_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_CONNECT_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_RESPONSE_EVT */           {BTA_PBA_CLIENT_FREE_RESPONSE,      BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_RESPONSE_FINAL_EVT */     {BTA_PBA_CLIENT_FREE_RESPONSE,      BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_GOEP_CONNECT_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT */    {BTA_PBA_CLIENT_GOEP_DISCONNECT,    BTA_PBA_CLIENT_INIT_ST},
};

const uint8_t bta_pba_client_st_getting[][BTA_PBA_CLIENT_NUM_COLS] = {
    /* Event                                    Action                              Next state */
    /* BTA_PBA_CLIENT_API_OPEN_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_API_CLOSE_EVT */          {BTA_PBA_CLIENT_FORCE_DISCONNECT,   BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_API_REQ_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_DISC_RES_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_AUTHENTICATE_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_CONNECT_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_RESPONSE_EVT */           {BTA_PBA_CLIENT_RESPONSE,           BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_RESPONSE_FINAL_EVT */     {BTA_PBA_CLIENT_RESPONSE_FINAL,     BTA_PBA_CLIENT_OPENED_ST},
    /* BTA_PBA_CLIENT_GOEP_CONNECT_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_REQUESTING_ST},
    /* BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT */    {BTA_PBA_CLIENT_GOEP_DISCONNECT,    BTA_PBA_CLIENT_INIT_ST},
};

const uint8_t bta_pba_client_st_closing[][BTA_PBA_CLIENT_NUM_COLS] = {
    /* Event                                    Action                              Next state */
    /* BTA_PBA_CLIENT_API_OPEN_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_API_CLOSE_EVT */          {BTA_PBA_CLIENT_FORCE_DISCONNECT,   BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_API_REQ_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_DISC_RES_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_AUTHENTICATE_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_CONNECT_EVT */            {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_RESPONSE_EVT */           {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_RESPONSE_FINAL_EVT */     {BTA_PBA_CLIENT_RESPONSE_FINAL,     BTA_PBA_CLIENT_INIT_ST},
    /* BTA_PBA_CLIENT_GOEP_CONNECT_EVT */       {BTA_PBA_CLIENT_IGNORE,             BTA_PBA_CLIENT_CLOSING_ST},
    /* BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT */    {BTA_PBA_CLIENT_GOEP_DISCONNECT,    BTA_PBA_CLIENT_INIT_ST},
};

/* type for state table */
typedef const UINT8 (*tBTA_PBA_CLIENT_ST_TBL)[BTA_PBA_CLIENT_NUM_COLS];

/* state table */
const tBTA_PBA_CLIENT_ST_TBL bta_pba_client_st_tbl[] = {
    bta_pba_client_st_init,
    bta_pba_client_st_opening,
    bta_pba_client_st_opened,
    bta_pba_client_st_getting,
    bta_pba_client_st_closing
};

/* PBA Client control block */
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_PBA_CLIENT_CB  bta_pba_client_cb;
#else
tBTA_PBA_CLIENT_CB  *bta_pba_client_cb_ptr;
#endif

static tBTA_PBA_CLIENT_CCB *allocate_ccb(void)
{
    tBTA_PBA_CLIENT_CCB *p_ccb = NULL;
    for (int i = 0; i < PBA_CLIENT_MAX_CONNECTION; ++i) {
        if (bta_pba_client_cb.ccb[i].allocated == 0) {
            bta_pba_client_cb.ccb[i].allocated = i + 1;
            p_ccb = &bta_pba_client_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static tBTA_PBA_CLIENT_CCB *find_ccb_by_handle(UINT16 handle)
{
    tBTA_PBA_CLIENT_CCB *p_ccb = NULL;
    for (int i = 0; i < PBA_CLIENT_MAX_CONNECTION; ++i) {
        if (bta_pba_client_cb.ccb[i].allocated != 0 && bta_pba_client_cb.ccb[i].allocated == handle) {
            p_ccb = &bta_pba_client_cb.ccb[i];
        }
    }
    return p_ccb;
}

static tBTA_PBA_CLIENT_CCB *find_ccb_by_goep_handle(UINT16 goep_handle)
{
    tBTA_PBA_CLIENT_CCB *p_ccb = NULL;
    for (int i = 0; i < PBA_CLIENT_MAX_CONNECTION; ++i) {
        if (bta_pba_client_cb.ccb[i].allocated != 0 && bta_pba_client_cb.ccb[i].goep_handle == goep_handle) {
            p_ccb = &bta_pba_client_cb.ccb[i];
        }
    }
    return p_ccb;
}

static tBTA_PBA_CLIENT_CCB *find_ccb_by_bd_addr(BD_ADDR bd_addr)
{
    tBTA_PBA_CLIENT_CCB *p_ccb = NULL;
    for (int i = 0; i < PBA_CLIENT_MAX_CONNECTION; ++i) {
        if (bta_pba_client_cb.ccb[i].allocated != 0 && bdcmp(bta_pba_client_cb.ccb[i].bd_addr, bd_addr) == 0) {
            p_ccb = &bta_pba_client_cb.ccb[i];
        }
    }
    return p_ccb;
}

void bta_pba_client_sm_execute(tBTA_PBA_CLIENT_CCB *p_ccb, UINT16 event, tBTA_PBA_CLIENT_DATA *p_data)
{
    tBTA_PBA_CLIENT_ST_TBL  state_table;
    UINT8                   action;

    state_table = bta_pba_client_st_tbl[p_ccb->state];

    event &= 0xff;

    p_ccb->state = state_table[event][BTA_PBA_CLIENT_NEXT_STATE];

    if ((action = state_table[event][BTA_PBA_CLIENT_ACTION]) != BTA_PBA_CLIENT_IGNORE) {
        (*bta_pba_client_action[action])(p_ccb, p_data);
    }

    return;
}

BOOLEAN bta_pba_client_hdl_event(BT_HDR *p_msg)
{
    tBTA_PBA_CLIENT_CCB *p_ccb = NULL;
    BOOLEAN execute_sm = FALSE;
    tBTA_PBA_CLIENT_CONN conn = {0};
    tBTA_PBA_CLIENT_DATA *p_data = (tBTA_PBA_CLIENT_DATA *)p_msg;

    switch (p_msg->event) {
    case BTA_PBA_CLIENT_API_ENABLE_EVT:
        bta_pba_client_api_enable(p_data);
        break;
    case BTA_PBA_CLIENT_API_DISABLE_EVT:
        bta_pba_client_api_disable(p_data);
        break;
    case BTA_PBA_CLIENT_API_REGISTER_EVT:
        bta_pba_client_api_register(p_data);
        break;
    case BTA_PBA_CLIENT_API_DEREGISTER_EVT:
        bta_pba_client_api_deregister(p_data);
        break;
    case BTA_PBA_CLIENT_API_OPEN_EVT:
        if (find_ccb_by_bd_addr(p_data->api_open.bd_addr) != NULL) {
            /* already connected */
            conn.handle = 0;
            conn.error = BTA_PBA_CLIENT_ALREADY_CONN;
            bdcpy(conn.bd_addr, p_data->api_open.bd_addr);
            bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_CONN_OPEN_EVT, (tBTA_PBA_CLIENT *)&conn);
            /* break, don't execute sm */
            break;
        }
        p_ccb = allocate_ccb();
        if (p_ccb == NULL) {
            /* no resource to allocate ccb */
            conn.handle = 0;
            conn.error = BTA_PBA_CLIENT_NO_RESOURCE;
            bdcpy(conn.bd_addr, p_data->api_open.bd_addr);
            bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_CONN_OPEN_EVT, (tBTA_PBA_CLIENT *)&conn);
            /* break, don't execute sm */
            break;
        }
        execute_sm = TRUE;
        break;
    case BTA_PBA_CLIENT_GOEP_CONNECT_EVT:
    case BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT:
    case BTA_PBA_CLIENT_RESPONSE_EVT:
    case BTA_PBA_CLIENT_RESPONSE_FINAL_EVT:
        p_ccb = find_ccb_by_goep_handle(p_msg->layer_specific);
        if (p_ccb == NULL) {
            /* ignore event with invalid goep handle */
            break;
        }
        execute_sm = TRUE;
        break;
    default:
        p_ccb = find_ccb_by_handle(p_msg->layer_specific);
        if (p_ccb == NULL) {
            /* ignore event with invalid handle */
            break;
        }
        execute_sm = TRUE;
    }

    if (execute_sm) {
        bta_pba_client_sm_execute(p_ccb, p_msg->event, (tBTA_PBA_CLIENT_DATA *) p_msg);
    }

    return TRUE;
}

#endif
