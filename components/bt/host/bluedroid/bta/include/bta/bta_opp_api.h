/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"
#include "stack/obex_api.h"
#include "bta/bta_api.h"

#if BTA_OPP_INCLUDED

/* Object Push Profile version */
#define OPP_VERSION                     0x0102      /* v1.2 */
#define OPP_DEFAULT_MTU                 L2CAP_MTU_SIZE  /* OBEX MTU when the caller passes 0 */
#define OPP_MIN_MTU                     255         /* minimum OBEX packet length */
#define OPP_MAX_NAME_LEN                255         /* maximum object name length (ASCII bytes) */
#define OPP_MAX_TYPE_LEN                64          /* maximum MIME type length */

/* OPP callback events */
#define BTA_OPP_SERVER_START_EVT        2           /* OPP server started listening */
#define BTA_OPP_SERVER_STOP_EVT         3           /* OPP server stopped listening */
#define BTA_OPP_CONN_OPEN_EVT           4           /* OPP connection opened */
#define BTA_OPP_CONN_CLOSE_EVT          5           /* OPP connection closed */
#define BTA_OPP_INCOMING_OBJECT_EVT     6           /* peer started pushing an object */
#define BTA_OPP_DATA_EVT                7           /* incoming object body data */
#define BTA_OPP_PROGRESS_EVT            8           /* object transfer progress */
#define BTA_OPP_TRANSFER_COMPLETE_EVT   9           /* object transfer finished */
#define BTA_OPP_SERVER_ENABLE_EVT       10          /* OPP server enabled */
#define BTA_OPP_SERVER_DISABLE_EVT      11          /* OPP server disabled */
#define BTA_OPP_CLIENT_ENABLE_EVT       12          /* OPP client enabled */
#define BTA_OPP_CLIENT_DISABLE_EVT      13          /* OPP client disabled */

typedef enum {
    BTA_OPP_OK = 0,                                 /* operation success */
    BTA_OPP_FAIL,                                   /* general failure */
    BTA_OPP_NO_RESOURCE,                            /* no resource */
    BTA_OPP_NOT_FOUND,                              /* object or connection not found */
    BTA_OPP_BUSY,                                   /* operation already in progress */
    BTA_OPP_NOT_SUPPORTED,                          /* operation not supported */
    BTA_OPP_ABORTED,                                /* transfer aborted or response timeout */
    BTA_OPP_SDP_FAIL,                               /* SDP failed */
    BTA_OPP_OBEX_FAIL,                              /* OBEX/GOEP failed */
    BTA_OPP_FORBIDDEN,                              /* peer refused the operation */
} tBTA_OPP_STATUS;

typedef enum {
    BTA_OPP_ROLE_CLIENT,                            /* local device is the OPP client */
    BTA_OPP_ROLE_SERVER,                            /* local device is the OPP server */
} tBTA_OPP_ROLE;

/* data associated with BTA_OPP_SERVER_START_EVT / BTA_OPP_SERVER_STOP_EVT */
typedef struct {
    tBTA_OPP_STATUS status;                         /* operation status */
    UINT8 scn;                                      /* RFCOMM server channel */
} tBTA_OPP_SERVER;

/* data associated with BTA_OPP_CONN_OPEN_EVT / BTA_OPP_CONN_CLOSE_EVT */
typedef struct {
    UINT16 handle;                                  /* connection handle */
    tBTA_OPP_ROLE role;                             /* local role */
    tBTA_OPP_STATUS status;                         /* operation status */
    BOOLEAN connected;                              /* TRUE on open, FALSE on close */
    BD_ADDR bd_addr;                                /* peer BD address */
} tBTA_OPP_CONN;

/* data associated with BTA_OPP_INCOMING_OBJECT_EVT */
typedef struct {
    UINT16 handle;                                  /* connection handle */
    const char *name;                               /* object name */
    const char *type;                               /* MIME type */
    UINT32 len;                                     /* declared object length */
    BOOLEAN needs_accept;                           /* TRUE if app must call BTA_OppAccept/Reject */
} tBTA_OPP_OBJECT;

/* data associated with BTA_OPP_DATA_EVT */
typedef struct {
    UINT16 handle;                                  /* connection handle */
    UINT8 *data;                                    /* body bytes in this PUT */
    UINT16 data_len;                                /* body length */
    BOOLEAN final;                                  /* TRUE if this is the last body chunk */
    BT_HDR *pkt;                                    /* raw buffer holding data; free before return */
} tBTA_OPP_DATA;

/* data associated with BTA_OPP_PROGRESS_EVT */
typedef struct {
    UINT16 handle;                                  /* connection handle */
    tBTA_OPP_ROLE role;                             /* local role */
    UINT32 transferred;                             /* bytes transferred so far */
    UINT32 total;                                   /* declared object length */
} tBTA_OPP_PROGRESS;

/* data associated with BTA_OPP_TRANSFER_COMPLETE_EVT */
typedef struct {
    UINT16 handle;                                  /* connection handle */
    tBTA_OPP_ROLE role;                             /* local role */
    tBTA_OPP_STATUS status;                         /* transfer result */
    UINT32 transferred;                             /* bytes transferred */
} tBTA_OPP_COMPLETE;

/* union of data associated with OPP callback */
typedef union {
    tBTA_OPP_SERVER server;                         /* SERVER_START_EVT, SERVER_STOP_EVT */
    tBTA_OPP_CONN conn;                             /* CONN_OPEN_EVT, CONN_CLOSE_EVT */
    tBTA_OPP_OBJECT object;                         /* INCOMING_OBJECT_EVT */
    tBTA_OPP_DATA data;                             /* DATA_EVT */
    tBTA_OPP_PROGRESS progress;                     /* PROGRESS_EVT */
    tBTA_OPP_COMPLETE complete;                     /* TRANSFER_COMPLETE_EVT */
} tBTA_OPP;

typedef UINT8 tBTA_OPP_EVT;

/* OPP callback. p_data member depends on event; see tBTA_OPP. */
typedef void (tBTA_OPP_CBACK)(tBTA_OPP_EVT event, tBTA_OPP *p_data);

/* OPP server start configuration */
typedef struct {
    tBTA_SEC sec_mask;                              /* security setting for the server */
    UINT16 mtu;                                     /* preferred OBEX packet length; 0 uses OPP_DEFAULT_MTU */
    const char *service_name;                       /* SDP service name; NULL uses the stack default */
    const UINT8 *supported_formats;                 /* GOEP Supported Formats List; NULL means any type */
    UINT8 supported_formats_len;                    /* number of bytes in supported_formats */
    BOOLEAN auto_accept;                            /* TRUE: accept every object without BTA_OppAccept */
} tBTA_OPP_SERVER_CFG;

/* OPP client connect parameters */
typedef struct {
    BD_ADDR bd_addr;                                /* peer BD address */
    tBTA_SEC sec_mask;                              /* security setting for the connection */
    UINT16 mtu;                                     /* preferred OBEX packet length; 0 uses OPP_DEFAULT_MTU */
} tBTA_OPP_CONNECT_PARAM;

/* OPP client object open parameters */
typedef struct {
    UINT16 handle;                                  /* connection handle */
    char *name;                                     /* object name */
    char *type;                                     /* MIME type */
    UINT32 len;                                     /* declared object length */
} tBTA_OPP_SEND_PARAM;

#if BTA_OPP_SERVER_INCLUDED
/*******************************************************************************
**
** Function         BTA_OppServerEnable
**
** Description      Enable the OPP server. When the enable operation is
**                  complete the callback function will be called with a
**                  BTA_OPP_SERVER_ENABLE_EVT. This function must be called
**                  before other functions in the OPP server API are called.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppServerEnable(tBTA_OPP_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_OppServerDisable
**
** Description      Disable the OPP server. When the disable operation is
**                  complete the callback function will be called with a
**                  BTA_OPP_SERVER_DISABLE_EVT.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppServerDisable(void);

/*******************************************************************************
**
** Function         BTA_OppStartServer
**
** Description      Start the OPP server listening for incoming connections.
**                  When the operation is complete the callback function will
**                  be called with a BTA_OPP_SERVER_START_EVT.
**
** Returns          BT_STATUS_SUCCESS if the request is queued.
**                  BT_STATUS_NOMEM if the message could not be allocated.
**
*******************************************************************************/
bt_status_t BTA_OppStartServer(tBTA_OPP_SERVER_CFG *cfg);

/*******************************************************************************
**
** Function         BTA_OppStopServer
**
** Description      Stop the OPP server. When the operation is complete the
**                  callback function will be called with a
**                  BTA_OPP_SERVER_STOP_EVT.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppStopServer(void);

/*******************************************************************************
**
** Function         BTA_OppAccept
**
** Description      Accept the incoming object that reported needs_accept.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppAccept(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_OppReject
**
** Description      Reject the incoming object that reported needs_accept.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppReject(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_OppServerCancel
**
** Description      Abort the incoming transfer on this connection.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppServerCancel(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_OppServerRxReady
**
** Description      Give OBEX RX credit: send a deferred Continue after the
**                  VFS has drained. Called from the OPP VFS layer when the
**                  server RX queue has space again.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppServerRxReady(UINT16 handle);
#endif

#if BTA_OPP_CLIENT_INCLUDED
/*******************************************************************************
**
** Function         BTA_OppClientEnable
**
** Description      Enable the OPP client. When the enable operation is
**                  complete the callback function will be called with a
**                  BTA_OPP_CLIENT_ENABLE_EVT. This function must be called
**                  before other functions in the OPP client API are called.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppClientEnable(tBTA_OPP_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_OppClientDisable
**
** Description      Disable the OPP client. When the disable operation is
**                  complete the callback function will be called with a
**                  BTA_OPP_CLIENT_DISABLE_EVT.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppClientDisable(void);

/*******************************************************************************
**
** Function         BTA_OppClientConnect
**
** Description      Connect to a peer OPP server. When the connection is
**                  opened the callback function will be called with a
**                  BTA_OPP_CONN_OPEN_EVT.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppClientConnect(tBTA_OPP_CONNECT_PARAM *param);

/*******************************************************************************
**
** Function         BTA_OppClientDisconnect
**
** Description      Disconnect an OPP client connection. When the connection
**                  is closed the callback function will be called with a
**                  BTA_OPP_CONN_CLOSE_EVT.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppClientDisconnect(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_OppOpenObject
**
** Description      Start a PUT of one object on an open client connection.
**
** Returns          BT_STATUS_SUCCESS if the request is queued.
**                  BT_STATUS_NOMEM if the message could not be allocated.
**
*******************************************************************************/
bt_status_t BTA_OppOpenObject(tBTA_OPP_SEND_PARAM *param);

/*******************************************************************************
**
** Function         BTA_OppClientTxReady
**
** Description      Tell the client that the TX queue has space for the next
**                  PUT body.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppClientTxReady(UINT16 handle);

/*******************************************************************************
**
** Function         BTA_OppClientCancel
**
** Description      Abort the outgoing transfer on this connection.
**
** Returns          void
**
*******************************************************************************/
void BTA_OppClientCancel(UINT16 handle);
#endif

#endif /* BTA_OPP_INCLUDED */
