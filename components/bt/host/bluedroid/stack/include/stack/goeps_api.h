/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#include "stack/goep_common.h"
#include "stack/obex_api.h"

#if (GOEPS_INCLUDED == TRUE)

enum {
    GOEPS_CONN_INCOME_EVT,          /* transport connected, OBEX session not established */
    GOEPS_OPENED_EVT,               /* OBEX Connect handshake completed */
    GOEPS_CLOSED_EVT,               /* connection closed */
    GOEPS_MTU_CHANGED_EVT,          /* lower layer MTU change */
    GOEPS_CONGEST_EVT,              /* lower layer connection congest */
    GOEPS_UNCONGEST_EVT,            /* lower layer connection uncongest */
    GOEPS_REQUEST_EVT               /* request from client */
};

typedef struct {
    UINT16      svr_handle;         /* GOEPS listener handle */
    UINT16      peer_mtu;           /* peer mtu of lower level connection */
    UINT16      our_mtu;            /* our mtu of lower level connection */
    BD_ADDR     addr;               /* peer bluetooth device address */
} tGOEPS_MSG_CONN_INCOME;

typedef struct {
    UINT16      peer_mtu;           /* peer mtu of lower level connection */
    UINT16      our_mtu;            /* our mtu of lower level connection */
} tGOEPS_MSG_OPENED;

typedef struct {
    UINT8       reason;             /* connection close reason */
} tGOEPS_MSG_CLOSED;

typedef struct {
    UINT16      peer_mtu;           /* peer mtu of lower level connection */
    UINT16      our_mtu;            /* our mtu of lower level connection */
} tGOEPS_MSG_MTU_CHANGED;

typedef struct {
    UINT8       opcode;             /* OBEX request opcode */
    BOOLEAN     final;              /* whether this is a final request packet */
    BOOLEAN     srm_en;             /* whether srm is enable */
    BOOLEAN     srm_wait;           /* whether srm wait is set by peer */
    BT_HDR      *pkt;               /* pointer to request packet, caller must free */
} tGOEPS_MSG_REQUEST;

typedef union {
    tGOEPS_MSG_CONN_INCOME   conn_income;
    tGOEPS_MSG_OPENED        opened;
    tGOEPS_MSG_CLOSED        closed;
    tGOEPS_MSG_MTU_CHANGED   mtu_changed;
    tGOEPS_MSG_REQUEST       request;
} tGOEPS_MSG;

typedef void (tGOEPS_EVT_CBACK)(UINT16 conn_handle, UINT8 event, tGOEPS_MSG *msg);

/*******************************************************************************
*       The following APIs are called by bluetooth stack automatically
*******************************************************************************/

extern UINT16 GOEPS_Init(void);

extern void GOEPS_Deinit(void);

/*******************************************************************************
*               The following APIs must be executed in btu task
*******************************************************************************/

extern UINT16 GOEPS_Register(tOBEX_SVR_INFO *p_listen, tGOEPS_EVT_CBACK callback, UINT16 *out_svr_handle);

extern UINT16 GOEPS_Deregister(UINT16 svr_handle);

extern UINT16 GOEPS_CloseConn(UINT16 conn_handle);

extern UINT16 GOEPS_SendResponse(UINT16 conn_handle);

extern UINT16 GOEPS_PrepareResponse(UINT16 conn_handle, tOBEX_PARSE_INFO *info, UINT16 buff_size);

extern UINT16 GOEPS_DropResponse(UINT16 conn_handle);

extern UINT16 GOEPS_ResponseSetSRM(UINT16 conn_handle, BOOLEAN srm_en, BOOLEAN srm_wait);

extern UINT16 GOEPS_ResponseAddHeader(UINT16 conn_handle, UINT8 header_id, const UINT8 *data, UINT16 data_len);

#endif /* #if (GOEPS_INCLUDED == TRUE) */
