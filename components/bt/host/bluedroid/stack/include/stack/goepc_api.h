/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#include "stack/goep_common.h"
#include "stack/obex_api.h"

#if (GOEPC_INCLUDED == TRUE)

enum {
    GOEPC_OPENED_EVT,               /* connection open */
    GOEPC_CLOSED_EVT,               /* disconnect unexpected */
    GOEPC_MTU_CHANGED_EVT,          /* lower layer MTU change */
    GOEPC_CONGEST_EVT,              /* lower layer connection congest */
    GOEPC_UNCONGEST_EVT,            /* lower layer connection uncongest */
    GOEPC_RESPONSE_EVT              /* response from server */
};

typedef struct {
    UINT16      peer_mtu;           /* peer mtu of lower level connection */
    UINT16      our_mtu;            /* our mtu of lower level connection */
} tGOEPC_MSG_OPENED;

typedef struct {
    UINT8   reason;                 /* connection close reason */
} tGOEPC_MSG_CLOSED;

typedef struct {
    UINT16  peer_mtu;               /* peer mtu of lower level connection */
    UINT16  our_mtu;                /* our mtu of lower level connection */
} tGOEPC_MSG_MTU_CHANGED;

typedef struct {
    UINT8   opcode;                 /* which opcode that this packet response to */
    BOOLEAN final;                  /* whether this is a final packet */
    BOOLEAN srm_en;                 /* whether srm is enable */
    BOOLEAN srm_wait;               /* whether srm wait is set, set by peer or by us */
    BT_HDR  *pkt;                   /* pointer to response packet */
} tGOEPC_MSG_RESPONSE;

typedef union {
    tGOEPC_MSG_OPENED       opened;
    tGOEPC_MSG_CLOSED       closed;
    tGOEPC_MSG_MTU_CHANGED  mtu_changed;
    tGOEPC_MSG_RESPONSE     response;
} tGOEPC_MSG;

typedef void (tGOEPC_EVT_CBACK)(UINT16 handle, UINT8 event, tGOEPC_MSG *msg);

/*******************************************************************************
*       The following APIs are called by bluetooth stack automatically
*******************************************************************************/

extern UINT16 GOEPC_Init(void);

extern void GOEPC_Deinit(void);

/*******************************************************************************
*               The following APIs must be executed in btu task
*******************************************************************************/

extern UINT16 GOEPC_Open(tOBEX_SVR_INFO *p_svr, tGOEPC_EVT_CBACK callback, UINT16 *out_handle);

extern UINT16 GOEPC_Close(UINT16 handle);

extern UINT16 GOEPC_SendRequest(UINT16 handle);

extern UINT16 GOEPC_PrepareRequest(UINT16 handle, tOBEX_PARSE_INFO *info, UINT16 buff_size);

extern UINT16 GOEPC_DropRequest(UINT16 handle);

extern UINT16 GOEPC_RequestSetSRM(UINT16 handle, BOOLEAN srm_en, BOOLEAN srm_wait);

extern UINT16 GOEPC_RequestAddHeader(UINT16 handle, UINT8 header_id, const UINT8 *data, UINT16 data_len);

#endif /* #if (GOEPC_INCLUDED == TRUE) */
