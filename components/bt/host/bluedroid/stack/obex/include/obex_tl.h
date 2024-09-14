/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#if (OBEX_INCLUDED == TRUE)

/* Return code of obex_tl_send_data */
#define OBEX_TL_FAILED              FALSE
#define OBEX_TL_SUCCESS             TRUE
#define OBEX_TL_CONGESTED           2

typedef enum {
    OBEX_TL_CONN_OPEN_EVT,
    OBEX_TL_CONN_INCOME_EVT,
    OBEX_TL_DIS_CONN_EVT,
    OBEX_TL_CONGEST_EVT,
    OBEX_TL_UNCONGEST_EVT,
    OBEX_TL_MTU_CHANGE_EVT,
    OBEX_TL_DATA_EVT
} tOBEX_TL_EVT;

typedef union {
    /* general struct, used to retrieve handle */
    struct {
        UINT16 hdl;
    } any;

    /* struct for OBEX_TL_CONN_OPEN_EVT */
    struct {
        UINT16 hdl;
        UINT16 peer_mtu;
        UINT16 our_mtu;
    } conn_open;

    /* struct for OBEX_TL_CONN_INCOME_EVT */
    struct {
        UINT16 hdl;
        UINT16 peer_mtu;
        UINT16 our_mtu;
        UINT16 svr_hdl;
    } conn_income;

    /* struct for OBEX_TL_MTU_CHANGE_EVT */
    struct {
        UINT16 hdl;
        UINT16 peer_mtu;
        UINT16 our_mtu;
    } mtu_chg;

    /* struct for OBEX_TL_DATA_EVT */
    struct {
        UINT16 hdl;
        BT_HDR *p_buf;
    } data;
} tOBEX_TL_MSG;

typedef struct
{
    UINT16  psm;                /* l2cap psm */
    UINT16  sec_mask;           /* security mask */
    UINT16  pref_mtu;           /* preferred mtu, limited by L2CAP_MTU_SIZE */
    BD_ADDR addr;               /* peer bluetooth device address */
} tOBEX_TL_L2CAP_SVR;

typedef union
{
    tOBEX_TL_L2CAP_SVR l2cap;
} tOBEX_TL_SVR_INFO;

typedef void (tOBEX_TL_CBACK)(tOBEX_TL_EVT evt, tOBEX_TL_MSG *msg);

typedef struct {
    void (*init)(tOBEX_TL_CBACK *callback);
    void (*deinit)(void);
    UINT16 (*connect)(tOBEX_TL_SVR_INFO *server);
    void (*disconnect)(UINT16 tl_hdl);
    UINT16 (*send)(UINT16 tl_hdl, BT_HDR *p_buf);
    UINT16 (*bind)(tOBEX_TL_SVR_INFO *server);
    void (*unbind)(UINT16 tl_hdl);
} tOBEX_TL_OPS;

#endif /* #if (OBEX_INCLUDED == TRUE) */
