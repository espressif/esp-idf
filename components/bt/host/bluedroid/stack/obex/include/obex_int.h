/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "obex_tl.h"
#include "obex_tl_l2cap.h"

#if (OBEX_INCLUDED == TRUE)

#define OBEX_BT_HDR_MIN_OFFSET      OBEX_TL_L2CAP_BT_HDR_MIN_OFFSET      /* should set to max value of all transport layer */

#define OBEX_ROLE_CLIENT            0x01
#define OBEX_ROLE_SERVER            0x02

/* OBEX connection state */
#define OBEX_STATE_IDLE             0       /* No connection */
#define OBEX_STATE_OPENING          1       /* Starting to open a connection */
#define OBEX_STATE_OPENED           2       /* Connection opened */

/* Store 16 bits data in big endian format, not modify the p_buf */
#define STORE16BE(p_buf, data)          do { *p_buf = ((data)>>8)&0xff; \
                                             *(p_buf+1) = (data)&0xff;} while(0)

/* OBEX Connection Control block */
typedef struct {
    tOBEX_MSG_CBACK     *callback;      /* Connection msg callback function */
    UINT16              tl_hdl;         /* Transport layer non-zeros connection handle*/
    UINT16              tl_peer_mtu;    /* Transport layer peer mtu */
    UINT16              tl_our_mtu;     /* Transport layer our mtu */
    UINT8               tl_cong;        /* 1 if transport layer congestion, otherwise 0 */
    UINT8               tl;             /* OBEX_OVER_L2CAP or OBEX_OVER_RFCOMM */
    UINT8               allocated;      /* 0, not allocated. index+1, otherwise. equal to api handle */
    UINT8               state;          /* This OBEX connection state */
    UINT8               role;           /* This OBEX connection role */
} tOBEX_CCB;

/* OBEX Server Control block */
typedef struct {
    tOBEX_MSG_CBACK     *callback;      /* Connection msg callback function */
    UINT16              tl_hdl;         /* Transport layer non-zeros server handle*/
    UINT8               tl;             /* OBEX_OVER_L2CAP or OBEX_OVER_RFCOMM */
    UINT8               allocated;      /* 0, not allocated. index+1, otherwise. */
} tOBEX_SCB;

/* OBEX Control block */
typedef struct {
    tOBEX_CCB       ccb[OBEX_MAX_CONNECTION];   /* connection control blocks */
    tOBEX_SCB       scb[OBEX_MAX_SERVER];       /* server control blocks */
    tOBEX_TL_OPS    *tl_ops[OBEX_NUM_TL];       /* transport operation function pointer */
    UINT8           trace_level;                /* trace level */
} tOBEX_CB;

#if OBEX_DYNAMIC_MEMORY == FALSE
extern tOBEX_CB obex_cb;
#else
extern tOBEX_CB *obex_cb_ptr;
#define obex_cb (*obex_cb_ptr)
#endif

void obex_tl_l2cap_callback(tOBEX_TL_EVT evt, tOBEX_TL_MSG *msg);
tOBEX_CCB *obex_allocate_ccb(void);
tOBEX_SCB *obex_allocate_scb(void);
void obex_free_ccb(tOBEX_CCB *p_ccb);
void obex_free_scb(tOBEX_SCB *p_scb);

#endif /* #if (OBEX_INCLUDED == TRUE) */
