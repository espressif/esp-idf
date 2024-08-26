/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"
#if BTA_AV_CA_INCLUDED

#include <string.h>
#include "bta/bta_av_api.h"
#include "bta_av_int.h"
#include "stack/avdt_api.h"
#include "bta/utl.h"
#include "stack/l2c_api.h"
#include "osi/allocator.h"
#include "osi/list.h"

#include "common/bt_trace.h"

/* state machine states */
enum {
    BTA_AV_CA_INIT_ST,
    BTA_AV_CA_OPENING_ST,
    BTA_AV_CA_CONNECTING_ST,
    BTA_AV_CA_CONNECTED_ST,
    BTA_AV_CA_GETTING_ST,
    BTA_AV_CA_CLOSING_ST
};

/* state machine action enumeration list */
enum {
    BTA_AV_API_CA_OPEN,
    BTA_AV_API_CA_CLOSE,
    BTA_AV_API_CA_GET,
    BTA_AV_CA_RESPONSE,
    BTA_AV_CA_RESPONSE_FINAL,
    BTA_AV_CA_GOEP_CONNECT,
    BTA_AV_CA_GOEP_DISCONNECT,
    BTA_AV_CA_FORCE_DISCONNECT,
    BTA_AV_CA_NUM_ACTIONS
};

#define BTA_AV_CA_IGNORE       BTA_AV_CA_NUM_ACTIONS

/* type for action functions */
typedef void (*tBTA_AV_CA_ACTION)(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data);

/* action functions */
const tBTA_AV_CA_ACTION bta_av_ca_action[] = {
    bta_av_ca_api_open,
    bta_av_ca_api_close,
    bta_av_ca_api_get,
    bta_av_ca_response,
    bta_av_ca_response_final,
    bta_av_ca_goep_connect,
    bta_av_ca_goep_disconnect,
    bta_av_ca_force_disconnect,
    NULL
};

/* state table information */
#define BTA_AV_CA_ACTION_COL           0       /* position of actions */
#define BTA_AV_CA_NEXT_STATE           1       /* position of next state */
#define BTA_AV_CA_NUM_COLS             2       /* number of columns in state tables */

/* state table for init state */
static const UINT8 bta_av_ca_st_init[][BTA_AV_CA_NUM_COLS] = {
    /* Event                            Action 1                    Next state */
    /* API_CA_OPEN_EVT */           {BTA_AV_API_CA_OPEN,            BTA_AV_CA_OPENING_ST },
    /* API_CA_CLOSE_EVT */          {BTA_AV_CA_IGNORE,              BTA_AV_CA_INIT_ST },
    /* API_CA_GET_EVT */            {BTA_AV_CA_IGNORE,              BTA_AV_CA_INIT_ST },
    /* CA_RESPONSE_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_INIT_ST },
    /* CA_RESPONSE_FINAL_EVT */     {BTA_AV_CA_IGNORE,              BTA_AV_CA_INIT_ST },
    /* CA_GOEP_CONNECT_EVT */       {BTA_AV_CA_IGNORE,              BTA_AV_CA_INIT_ST },
    /* CA_GOEP_DISCONNECT_EVT */    {BTA_AV_CA_IGNORE,              BTA_AV_CA_INIT_ST },
};

/* state table for opening state */
static const UINT8 bta_av_ca_st_opening[][BTA_AV_CA_NUM_COLS] = {
    /* Event                            Action 1                    Next state */
    /* API_CA_OPEN_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_OPENING_ST },
    /* API_CA_CLOSE_EVT */          {BTA_AV_CA_FORCE_DISCONNECT,    BTA_AV_CA_INIT_ST },
    /* API_CA_GET_EVT */            {BTA_AV_CA_IGNORE,              BTA_AV_CA_OPENING_ST },
    /* CA_RESPONSE_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_OPENING_ST },
    /* CA_RESPONSE_FINAL_EVT */     {BTA_AV_CA_IGNORE,              BTA_AV_CA_OPENING_ST },
    /* CA_GOEP_CONNECT_EVT */       {BTA_AV_CA_GOEP_CONNECT,        BTA_AV_CA_CONNECTING_ST },
    /* CA_GOEP_DISCONNECT_EVT */    {BTA_AV_CA_GOEP_DISCONNECT,     BTA_AV_CA_INIT_ST },
};

/* state table for connecting state */
static const UINT8 bta_av_ca_st_connecting[][BTA_AV_CA_NUM_COLS] = {
    /* Event                            Action 1                    Next state */
    /* API_CA_OPEN_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTING_ST },
    /* API_CA_CLOSE_EVT */          {BTA_AV_CA_FORCE_DISCONNECT,    BTA_AV_CA_INIT_ST },
    /* API_CA_GET_EVT */            {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTING_ST },
    /* CA_RESPONSE_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTING_ST },
    /* CA_RESPONSE_FINAL_EVT */     {BTA_AV_CA_RESPONSE_FINAL,      BTA_AV_CA_CONNECTED_ST },
    /* CA_GOEP_CONNECT_EVT */       {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTING_ST },
    /* CA_GOEP_DISCONNECT_EVT */    {BTA_AV_CA_GOEP_DISCONNECT,     BTA_AV_CA_INIT_ST },
};

/* state table for connected state */
static const UINT8 bta_av_ca_st_connected[][BTA_AV_CA_NUM_COLS] = {
    /* Event                            Action 1                    Next state */
    /* API_CA_OPEN_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTED_ST },
    /* API_CA_CLOSE_EVT */          {BTA_AV_API_CA_CLOSE,           BTA_AV_CA_CLOSING_ST },
    /* API_CA_GET_EVT */            {BTA_AV_API_CA_GET,             BTA_AV_CA_GETTING_ST },
    /* CA_RESPONSE_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTED_ST },
    /* CA_RESPONSE_FINAL_EVT */     {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTED_ST },
    /* CA_GOEP_CONNECT_EVT */       {BTA_AV_CA_IGNORE,              BTA_AV_CA_CONNECTED_ST },
    /* CA_GOEP_DISCONNECT_EVT */    {BTA_AV_CA_GOEP_DISCONNECT,     BTA_AV_CA_INIT_ST },
};

/* state table for getting state */
static const UINT8 bta_av_ca_st_getting[][BTA_AV_CA_NUM_COLS] = {
    /* Event                            Action 1                    Next state */
    /* API_CA_OPEN_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_GETTING_ST },
    /* API_CA_CLOSE_EVT */          {BTA_AV_CA_FORCE_DISCONNECT,    BTA_AV_CA_INIT_ST },
    /* API_CA_GET_EVT */            {BTA_AV_CA_IGNORE,              BTA_AV_CA_GETTING_ST },
    /* CA_RESPONSE_EVT */           {BTA_AV_CA_RESPONSE,            BTA_AV_CA_GETTING_ST },
    /* CA_RESPONSE_FINAL_EVT */     {BTA_AV_CA_RESPONSE_FINAL,      BTA_AV_CA_CONNECTED_ST },
    /* CA_GOEP_CONNECT_EVT */       {BTA_AV_CA_IGNORE,              BTA_AV_CA_GETTING_ST },
    /* CA_GOEP_DISCONNECT_EVT */    {BTA_AV_CA_GOEP_DISCONNECT,     BTA_AV_CA_INIT_ST },
};

/* state table for closing state */
static const UINT8 bta_av_ca_st_closing[][BTA_AV_CA_NUM_COLS] = {
    /* Event                            Action 1                    Next state */
    /* API_CA_OPEN_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_CLOSING_ST },
    /* API_CA_CLOSE_EVT */          {BTA_AV_CA_FORCE_DISCONNECT,    BTA_AV_CA_INIT_ST },
    /* API_CA_GET_EVT */            {BTA_AV_CA_IGNORE,              BTA_AV_CA_CLOSING_ST },
    /* CA_RESPONSE_EVT */           {BTA_AV_CA_IGNORE,              BTA_AV_CA_CLOSING_ST },
    /* CA_RESPONSE_FINAL_EVT */     {BTA_AV_CA_RESPONSE_FINAL,      BTA_AV_CA_INIT_ST },
    /* CA_GOEP_CONNECT_EVT */       {BTA_AV_CA_IGNORE,              BTA_AV_CA_CLOSING_ST },
    /* CA_GOEP_DISCONNECT_EVT */    {BTA_AV_CA_GOEP_DISCONNECT,     BTA_AV_CA_INIT_ST },
};

/* type for state table */
typedef const UINT8 (*tBTA_AV_CA_ST_TBL)[BTA_AV_CA_NUM_COLS];

/* state table */
static const tBTA_AV_CA_ST_TBL bta_av_ca_st_tbl[] = {
    bta_av_ca_st_init,
    bta_av_ca_st_opening,
    bta_av_ca_st_connecting,
    bta_av_ca_st_connected,
    bta_av_ca_st_getting,
    bta_av_ca_st_closing
};

void bta_av_ca_sm_execute(tBTA_AV_RCB *p_rcb, UINT16 event, tBTA_AV_DATA *p_data)
{
    tBTA_AV_CA_ST_TBL   state_table;
    UINT8               action;
    /* look up the state table for the current state */
    state_table = bta_av_ca_st_tbl[p_rcb->cover_art_state];

    event -= BTA_AV_CA_FIRST_SM_EVT;

    /* set next state */
    p_rcb->cover_art_state = state_table[event][BTA_AV_CA_NEXT_STATE];

    /* execute action functions */
    if ((action = state_table[event][BTA_AV_CA_ACTION_COL]) != BTA_AV_CA_IGNORE) {
        (*bta_av_ca_action[action])(p_rcb, p_data);
    }
}

#endif /* BTA_AV_CA_INCLUDED */
