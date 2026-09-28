/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "bta/bta_sys.h"
#include "stack/goeps_api.h"

#if BTA_OPP_SERVER_INCLUDED

enum {
    /* these events are handled by the state machine */
    BTA_OPP_SERVER_GOEP_REQ_CONNECT_EVT = BTA_SYS_EVT_START(BTA_ID_OPS),
    BTA_OPP_SERVER_GOEP_REQ_PUT_EVT,
    BTA_OPP_SERVER_GOEP_REQ_DISCONNECT_EVT,
    BTA_OPP_SERVER_GOEP_REQ_ABORT_EVT,
    BTA_OPP_SERVER_GOEP_REQ_OTHER_EVT,
    BTA_OPP_SERVER_GOEP_OPEN_EVT,
    BTA_OPP_SERVER_GOEP_CLOSE_EVT,
    BTA_OPP_SERVER_GOEP_MTU_EVT,
    BTA_OPP_SERVER_API_ACCEPT_EVT,
    BTA_OPP_SERVER_API_REJECT_EVT,
    BTA_OPP_SERVER_API_CANCEL_EVT,

    /* these events are handled outside of the state machine */
    BTA_OPP_SERVER_API_RX_READY_EVT,
    BTA_OPP_SERVER_API_ENABLE_EVT,
    BTA_OPP_SERVER_API_DISABLE_EVT,
    BTA_OPP_SERVER_API_START_EVT,
    BTA_OPP_SERVER_API_STOP_EVT,
    BTA_OPP_SERVER_GOEP_CONN_INCOME_EVT,
};

typedef struct {
    BT_HDR hdr;
    tBTA_OPP_CBACK *p_cback;
} tBTA_OPP_SERVER_API_ENABLE;

typedef struct {
    BT_HDR hdr;
    tBTA_OPP_SERVER_CFG cfg;
} tBTA_OPP_SERVER_API_START;

typedef struct {
    BT_HDR hdr;
    UINT16 peer_mtu;
    UINT16 our_mtu;
    UINT8 opcode;
    BOOLEAN final;
    BT_HDR *pkt;
    BD_ADDR bd_addr;
} tBTA_OPP_SERVER_GOEP_MSG;

typedef union {
    BT_HDR hdr;
    tBTA_OPP_SERVER_API_ENABLE api_enable;
    tBTA_OPP_SERVER_API_START api_start;
    tBTA_OPP_SERVER_GOEP_MSG goep;
} tBTA_OPP_SERVER_DATA;

BOOLEAN bta_opp_server_hdl_event(BT_HDR *p_msg);

UINT32 bta_opp_server_create_sdp_record(const tBTA_OPP_SERVER_CFG *cfg);
void bta_opp_server_del_sdp_record(void);

#endif /* BTA_OPP_SERVER_INCLUDED */
