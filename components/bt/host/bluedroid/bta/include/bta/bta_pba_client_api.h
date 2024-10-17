/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"
#include "stack/obex_api.h"
#include "bta/bta_pba_defs.h"

#if BTA_PBA_CLIENT_INCLUDED

/* Phone Book Access Profile (client) version */
#define PBAP_PCE_VERSION                0x102       /* v1.2 */
#define PBAP_PCE_SUPPORTED_FEATURES     0x0000003F  /* support all features */

/* PBA Client callback events */
#define BTA_PBA_CLIENT_ENABLE_EVT                   0   /* PBA Client enabled */
#define BTA_PBA_CLIENT_DISABLE_EVT                  1   /* PBA Client disabled */
#define BTA_PBA_CLIENT_REGISTER_EVT                 2   /* PBA Client registered */
#define BTA_PBA_CLIENT_DEREGISTER_EVT               3   /* PBA Client deregistered */
#define BTA_PBA_CLIENT_CONN_OPEN_EVT                4   /* PBA Client connection opened */
#define BTA_PBA_CLIENT_CONN_CLOSE_EVT               5   /* PBA Client connection closed */
#define BTA_PBA_CLIENT_PULL_PHONE_BOOK_RSP_EVT      6   /* PBA Client pull phone book response */
#define BTA_PBA_CLIENT_SET_PHONE_BOOK_RSP_EVT       7   /* PBA Client set phone book response */
#define BTA_PBA_CLIENT_PULL_VCARD_LISTING_RSP_EVT   8   /* PBA Client pull vCard listing response */
#define BTA_PBA_CLIENT_PULL_VCARD_ENTRY_RSP_EVT     9   /* PBA Client pull vCard entry response */

typedef enum {
    BTA_PBA_CLIENT_NO_ERROR,                        /* operation success */
    BTA_PBA_CLIENT_FAIL,                            /* general fail */
    BTA_PBA_CLIENT_ALREADY_CONN,                    /* connection to peer device already exist */
    BTA_PBA_CLIENT_NO_RESOURCE,                     /* no resource */
    BTA_PBA_CLIENT_SDP_ERROR,                       /* error in sdp */
    BTA_PBA_CLIENT_GOEP_ERROR,                      /* error in goep */
    BTA_PBA_CLIENT_AUTH_FAIL,                       /* authenticate failed */
    BTA_PBA_CLIENT_DISABLE,                         /* operation failed due to pba client is disable */

    /* these error code is related to OBEX */
    BTA_PBA_CLIENT_BAD_REQUEST            = 0xC0,   /* Server couldn't understand request */
    BTA_PBA_CLIENT_UNAUTHORIZED           = 0xC1,   /* Unauthorized */
    BTA_PBA_CLIENT_FORBIDDEN              = 0xC3,   /* Operation is understood but refused > */
    BTA_PBA_CLIENT_NOT_FOUND              = 0xC4,   /* Not found */
    BTA_PBA_CLIENT_NOT_ACCEPTABLE         = 0xC6,   /* Not Acceptable */
    BTA_PBA_CLIENT_PRECONDITION_FAILED    = 0xCC,   /* Precondition failed */
    BTA_PBA_CLIENT_NOT_IMPLEMENTED        = 0xD1,   /* Not implemented */
    BTA_PBA_CLIENT_SERVICE_UNAVAILABLE    = 0xD3,   /* Service unavailable */
} tBTA_PBA_CLIENT_ERR;

typedef struct {
    UINT16                      handle;                 /* connection handle */
    tBTA_PBA_CLIENT_ERR         error;                  /* error code */
    BD_ADDR                     bd_addr;                /* peer BD addr */
    UINT8                       peer_supported_repo;    /* peer supported repositories */
    UINT32                      peer_supported_feat;    /* peer supported feature */
} tBTA_PBA_CLIENT_CONN;

typedef struct {
    tBTA_PBA_CLIENT_ERR         status;
    UINT16                      handle;             /* connection handle */
    BOOLEAN                     final;              /* final data event */
    UINT8                      *data;               /* body data in response packet */
    UINT16                      data_len;           /* body data length */
    UINT8                      *app_param;          /* application parameters */
    UINT16                      app_param_len;      /* application parameters length */
    BT_HDR                     *pkt;                /* raw buff that store all data, should be freed before return */
} tBTA_PBA_CLIENT_RESPONSE;

/* union of data associated with PBA Client callback */
typedef union {
    tBTA_PBA_CLIENT_CONN        conn;               /* CONN_OPEN_EVT, CONN_CLOSE_EVT */
    tBTA_PBA_CLIENT_RESPONSE    response;           /* XXX_RSP_EVT */
} tBTA_PBA_CLIENT;

typedef UINT8 tBTA_PBA_CLIENT_EVT;

typedef void (tBTA_PBA_CLIENT_CBACK)(tBTA_PBA_CLIENT_EVT event, tBTA_PBA_CLIENT *p_data);

void BTA_PbaClientEnable(tBTA_PBA_CLIENT_CBACK *p_cback);
void BTA_PbaClientDisable(void);
void BTA_PbaClientRegister(const char *server_name);
void BTA_PbaClientDeregister(void);
void BTA_PbaClientOpen(BD_ADDR bd_addr, tBTA_SEC sec_mask, UINT32 supported_feat, UINT16 mtu);
void BTA_PbaClientClose(UINT16 handle);
void BTA_PbaClientPullPhoneBook(UINT16 handle, char *name, UINT8 *app_param, UINT16 app_param_len);
void BTA_PbaClientSetPhoneBook(UINT16 handle, UINT8 flags, char *name);
void BTA_PbaClientPullvCardListing(UINT16 handle, char *name, UINT8 *app_param, UINT16 app_param_len);
void BTA_PbaClientPullvCardEntry(UINT16 handle, char *name, UINT8 *app_param, UINT16 app_param_len);

#endif
