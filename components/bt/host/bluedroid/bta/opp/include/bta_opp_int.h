/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"
#include "stack/sdp_api.h"
#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "bta/bta_sys.h"
#include "bta/bta_opp_api.h"

#if BTA_OPP_INCLUDED

#define BTA_OPP_MAX_CONNECTION          2
#define BTA_OPP_DEFAULT_SERVER_NAME     "OBEX Object Push"
#define BTA_OPP_DEFAULT_FORMAT          0xFF

/* accept timeout in milliseconds */
#define BTA_OPP_SERVER_ACCEPT_TIMEOUT_MS       (30 * 1000)
/* client OBEX request-response timeout (aligned with server accept hold) */
#define BTA_OPP_CLIENT_RSP_TIMEOUT_MS          (30 * 1000)

typedef struct {
    BT_HDR *pkt;
    UINT8 opcode;
    BOOLEAN req_final;
} tBTA_OPP_HOLD_PUT;

typedef struct {
    UINT8 allocated;
    UINT16 handle;
    UINT16 goep_handle;
    UINT8 role;
    UINT8 state;
    BD_ADDR bd_addr;
    UINT16 max_tx;
    UINT16 max_rx;
    UINT32 total_len;
    UINT32 transferred;
    char *name;
    char *type;
    UINT32 tx_len;
    UINT32 tx_offset;
    BOOLEAN tx_waiting;
    tSDP_DISCOVERY_DB *p_disc_db;
    UINT8 peer_rfcomm_scn;
    UINT16 peer_l2cap_psm;
    tBTA_SEC sec_mask;
    /* Client: wait for OBEX CONNECT/PUT/DISCONNECT response. */
    TIMER_LIST_ENT rsp_timer;
    BOOLEAN rsp_timer_on;
    /* Single PUT on hold while awaiting accept/reject. */
    BOOLEAN has_hold_put;
    tBTA_OPP_HOLD_PUT hold_put;
    TIMER_LIST_ENT accept_timer;
    BOOLEAN accept_timer_on;
    BOOLEAN session_accepted;           /*!< Connection authorized after first accept. */
    BOOLEAN rx_flow_blocked;            /*!< Waiting to send Continue until VFS has credit. */
    UINT8 deferred_rsp_opcode;          /*!< Opcode for the deferred Continue response. */
    BOOLEAN pm_open;                    /*!< TRUE after bta_sys_conn_open until conn_close. */
} tBTA_OPP_CCB;

typedef struct {
    tBTA_OPP_CBACK *p_cback;
#if BTA_OPP_SERVER_INCLUDED
    BOOLEAN server_registered;
    UINT16 goep_svr_handle;
    UINT16 goep_l2cap_svr_handle;
    UINT32 sdp_handle;
    UINT8 server_scn;
    UINT16 server_l2cap_psm;
    UINT16 server_mtu;
    tBTA_SEC server_sec_mask;
    BOOLEAN server_auto_accept;
#endif
#if BTA_OPP_CLIENT_INCLUDED
    BOOLEAN client_registered;
#endif
    tBTA_OPP_CCB ccb[BTA_OPP_MAX_CONNECTION];
} tBTA_OPP_CB;

#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_OPP_CB bta_opp_cb;
#else
extern tBTA_OPP_CB *bta_opp_cb_ptr;
#define bta_opp_cb (*bta_opp_cb_ptr)
#endif

tBTA_OPP_CCB *bta_opp_allocate_ccb(void);
tBTA_OPP_CCB *bta_opp_find_ccb_by_handle(UINT16 handle);
tBTA_OPP_CCB *bta_opp_find_ccb_by_goep_handle(UINT8 role, UINT16 goep_handle);
void bta_opp_free_ccb(tBTA_OPP_CCB *p_ccb);
UINT8 bta_opp_get_conn_num(void);

void bta_opp_report_conn(tBTA_OPP_CCB *p_ccb, BOOLEAN connected, tBTA_OPP_STATUS status);
void bta_opp_report_complete(tBTA_OPP_CCB *p_ccb, tBTA_OPP_STATUS status);
void bta_opp_report_progress(tBTA_OPP_CCB *p_ccb);
void bta_opp_close_connection(tBTA_OPP_CCB *p_ccb, tBTA_OPP_STATUS status, BOOLEAN report);
void bta_opp_pm_busy(tBTA_OPP_CCB *p_ccb);

#if BTA_OPP_SERVER_INCLUDED
#include "bta_opp_server_int.h"
void bta_opp_server_cleanup_ccb(tBTA_OPP_CCB *p_ccb);
#endif

#if BTA_OPP_CLIENT_INCLUDED
#include "bta_opp_client_int.h"
void bta_opp_client_cleanup_ccb(tBTA_OPP_CCB *p_ccb);
#endif

#endif /* BTA_OPP_INCLUDED */
