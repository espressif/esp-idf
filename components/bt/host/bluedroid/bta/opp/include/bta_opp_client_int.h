/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "bta/bta_sys.h"
#include "stack/goepc_api.h"

#if BTA_OPP_CLIENT_INCLUDED

enum {
    /* these events are handled by the state machine */
    BTA_OPP_CLIENT_API_CONNECT_EVT = BTA_SYS_EVT_START(BTA_ID_OPC),
    BTA_OPP_CLIENT_API_DISCONNECT_EVT,
    BTA_OPP_CLIENT_API_OPEN_OBJECT_EVT,
    BTA_OPP_CLIENT_API_CANCEL_EVT,
    BTA_OPP_CLIENT_DISC_RES_EVT,
    BTA_OPP_CLIENT_GOEP_OPEN_EVT,
    BTA_OPP_CLIENT_GOEP_RSP_EVT,
    BTA_OPP_CLIENT_GOEP_CLOSE_EVT,
    BTA_OPP_CLIENT_API_TX_READY_EVT,
    BTA_OPP_CLIENT_RSP_TIMEOUT_EVT,

    /* these events are handled outside of the state machine */
    BTA_OPP_CLIENT_API_ENABLE_EVT,
    BTA_OPP_CLIENT_API_DISABLE_EVT,
};

typedef struct {
    BT_HDR hdr;
    tBTA_OPP_CBACK *p_cback;
} tBTA_OPP_CLIENT_API_ENABLE;

typedef struct {
    BT_HDR hdr;
    tBTA_OPP_CONNECT_PARAM param;
} tBTA_OPP_CLIENT_API_CONNECT;

typedef struct {
    BT_HDR hdr;
    tBTA_OPP_SEND_PARAM param;
} tBTA_OPP_CLIENT_API_SEND_OBJECT;

typedef struct {
    BT_HDR hdr;
    UINT16 status;
} tBTA_OPP_CLIENT_DISC_RESULT;

typedef struct {
    BT_HDR hdr;
    UINT16 peer_mtu;
    UINT16 our_mtu;
    UINT8 reason;
    UINT8 opcode;
    BOOLEAN final;
    BT_HDR *pkt;
} tBTA_OPP_CLIENT_GOEP_MSG;

typedef union {
    BT_HDR hdr;
    tBTA_OPP_CLIENT_API_ENABLE      api_enable;
    tBTA_OPP_CLIENT_API_CONNECT     api_connect;
    tBTA_OPP_CLIENT_API_SEND_OBJECT api_send;
    tBTA_OPP_CLIENT_DISC_RESULT     disc_result;
    tBTA_OPP_CLIENT_GOEP_MSG        goep;
} tBTA_OPP_CLIENT_DATA;

BOOLEAN bta_opp_client_hdl_event(BT_HDR *p_msg);

BOOLEAN bta_opp_client_do_disc(tBTA_OPP_CCB *p_ccb);
BOOLEAN bta_opp_client_sdp_find_attr(tBTA_OPP_CCB *p_ccb);
void bta_opp_client_free_sdp_db(tBTA_OPP_CCB *p_ccb);

#endif /* BTA_OPP_CLIENT_INCLUDED */
