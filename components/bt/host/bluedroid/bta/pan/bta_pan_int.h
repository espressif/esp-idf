/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
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
 *  This is the private interface file for the BTA data gateway.
 *
 ******************************************************************************/
#ifndef BTA_PAN_INT_H
#define BTA_PAN_INT_H

#include "common/bt_target.h"
#include "bta/bta_pan_api.h"
#include "bta/bta_sys.h"
#include "osi/fixed_queue.h"

#if (BTA_PAN_INCLUDED == TRUE)

#define fixed_queue_try_dequeue(q) fixed_queue_dequeue((q), 0)

/*****************************************************************************
 *  Constants
 ****************************************************************************/

/* PAN events */
enum {
    /* these events are handled by the state machine */
    BTA_PAN_API_CLOSE_EVT = BTA_SYS_EVT_START(BTA_ID_PAN),
    BTA_PAN_CI_TX_READY_EVT,
    BTA_PAN_CI_RX_READY_EVT,
    BTA_PAN_CI_TX_FLOW_EVT,
    BTA_PAN_CI_RX_WRITE_EVT,
    BTA_PAN_CI_RX_WRITEBUF_EVT,
    BTA_PAN_CONN_OPEN_EVT,
    BTA_PAN_CONN_CLOSE_EVT,
    BTA_PAN_BNEP_FLOW_ENABLE_EVT,
    BTA_PAN_RX_FROM_BNEP_READY_EVT,

    /* these events are handled outside of the state machine */
    BTA_PAN_API_ENABLE_EVT,
    BTA_PAN_API_DISABLE_EVT,
    BTA_PAN_API_SET_ROLE_EVT,
    BTA_PAN_API_OPEN_EVT,
    BTA_PAN_CI_SET_PFILTER_EVT,
    BTA_PAN_CI_SET_MFILTER_EVT
};

/* state machine states */
enum { BTA_PAN_IDLE_ST, BTA_PAN_OPEN_ST, BTA_PAN_CLOSING_ST };

/*****************************************************************************
 *  Data types
 ****************************************************************************/

/* data type for BTA_PAN_API_ENABLE_EVT */
typedef struct {
    BT_HDR hdr;              /* Event header */
    tBTA_PAN_CBACK *p_cback; /* PAN callback function */
} tBTA_PAN_API_ENABLE;

/* data type for BTA_PAN_API_REG_ROLE_EVT */
typedef struct {
    BT_HDR hdr;                               /* Event header */
    char user_name[BTA_SERVICE_NAME_LEN + 1]; /* Service name */
    char gn_name[BTA_SERVICE_NAME_LEN + 1];   /* Service name */
    char nap_name[BTA_SERVICE_NAME_LEN + 1];  /* Service name */
    tBTA_PAN_ROLE role;
    UINT8 user_app_id;
    UINT8 gn_app_id;
    UINT8 nap_app_id;
    tBTA_SEC user_sec_mask; /* Security mask */
    tBTA_SEC gn_sec_mask;   /* Security mask */
    tBTA_SEC nap_sec_mask;  /* Security mask */

} tBTA_PAN_API_SET_ROLE;

/* data type for BTA_PAN_API_OPEN_EVT */
typedef struct {
    BT_HDR hdr;               /* Event header */
    tBTA_PAN_ROLE local_role; /* local role */
    tBTA_PAN_ROLE peer_role;  /* peer role */
    BD_ADDR bd_addr;          /* peer bdaddr */
} tBTA_PAN_API_OPEN;

/* data type for BTA_PAN_CI_TX_FLOW_EVT */
typedef struct {
    BT_HDR hdr;     /* Event header */
    BOOLEAN enable; /* Flow control setting */
} tBTA_PAN_CI_TX_FLOW;

/* data type for BTA_PAN_CI_SET_PFILTER_EVT */
typedef struct {
    BT_HDR hdr;
    UINT16 num_filters;
    UINT16 start_array[BNEP_MAX_PROT_FILTERS];
    UINT16 end_array[BNEP_MAX_PROT_FILTERS];
} tBTA_PAN_CI_SET_PFILTER;

/* data type for BTA_PAN_CI_SET_MFILTER_EVT */
typedef struct {
    BT_HDR hdr;
    UINT16 num_filters;
    UINT8 start_array[BNEP_MAX_MULTI_FILTERS * BD_ADDR_LEN];
    UINT8 end_array[BNEP_MAX_MULTI_FILTERS * BD_ADDR_LEN];
} tBTA_PAN_CI_SET_MFILTER;

/* data type for BTA_PAN_CONN_OPEN_EVT */
typedef struct {
    BT_HDR hdr; /* Event header */
    tPAN_RESULT result;

} tBTA_PAN_CONN;

/* union of all data types */
typedef union {
    BT_HDR hdr;
    tBTA_PAN_API_ENABLE api_enable;
    tBTA_PAN_API_SET_ROLE api_set_role;
    tBTA_PAN_API_OPEN api_open;
    tBTA_PAN_CI_TX_FLOW ci_tx_flow;
    tBTA_PAN_CI_SET_PFILTER ci_set_pfilter;
    tBTA_PAN_CI_SET_MFILTER ci_set_mfilter;
    tBTA_PAN_CONN conn;
} tBTA_PAN_DATA;

/* state machine control block */
typedef struct {
    BD_ADDR bd_addr;          /* peer bdaddr */
    fixed_queue_t *
    data_queue;           /* Queue of buffers waiting to be passed to application */
    UINT16 handle;            /* BTA PAN/BNEP handle */
    BOOLEAN in_use;           /* scb in use */
    tBTA_SEC sec_mask;        /* Security mask */
    BOOLEAN pan_flow_enable;  /* BNEP flow control state */
    BOOLEAN app_flow_enable;  /* Application flow control state */
    UINT8 state;              /* State machine state */
    tBTA_PAN_ROLE local_role; /* local role */
    tBTA_PAN_ROLE peer_role;  /* peer role */
    UINT8 app_id;             /* application id for the connection */

} tBTA_PAN_SCB;

/* main control block */
typedef struct {
    tBTA_PAN_SCB scb[BTA_PAN_NUM_CONN]; /* state machine control blocks */
    tBTA_PAN_CBACK *p_cback;            /* PAN callback function */
    UINT8 app_id[3];                    /* application id for PAN roles */
    UINT8 flow_mask;                    /* Data flow mask */
    UINT8 q_level;                      /* queue level set by application for TX data */

} tBTA_PAN_CB;

/* pan data param */
typedef struct {
    BT_HDR hdr;
    BD_ADDR src;
    BD_ADDR dst;
    UINT16 protocol;
    BOOLEAN ext;
    BOOLEAN forward;

} tBTA_PAN_DATA_PARAMS;

/*****************************************************************************
 *  Global data
 ****************************************************************************/

/* PAN control block */
extern tBTA_PAN_CB bta_pan_cb;

/*****************************************************************************
 *  Function prototypes
 ****************************************************************************/
extern tBTA_PAN_SCB *bta_pan_scb_alloc(void);
extern void bta_pan_scb_dealloc(tBTA_PAN_SCB *p_scb);
extern UINT8 bta_pan_scb_to_idx(tBTA_PAN_SCB *p_scb);
extern tBTA_PAN_SCB *bta_pan_scb_by_handle(UINT16 handle);
extern BOOLEAN bta_pan_hdl_event(BT_HDR *p_msg);

/* action functions */
extern void bta_pan_enable(tBTA_PAN_DATA *p_data);
extern void bta_pan_disable(void);
extern void bta_pan_set_role(tBTA_PAN_DATA *p_data);
extern void bta_pan_open(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_api_close(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_set_shutdown(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_rx_path(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_tx_path(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_tx_flow(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_conn_open(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_conn_close(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_writebuf(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_write_buf(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);
extern void bta_pan_free_buf(tBTA_PAN_SCB *p_scb, tBTA_PAN_DATA *p_data);

#endif /* BTA_PAN_INCLUDED */

#endif /* BTA_PAN_INT_H */
