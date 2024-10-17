/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "common/bt_target.h"

#if (OBEX_INCLUDED == TRUE)

/* API function return value result codes. */
#define OBEX_SUCCESS                0       /* Operation successful */
#define OBEX_FAILURE                1       /* Operation failed */
#define OBEX_NO_RESOURCES           3       /* Not enough resources */
#define OBEX_BAD_HANDLE             4       /* Bad handle */
#define OBEX_INVALID_PARAM          5       /* Invalid parameter */
#define OBEX_NOT_OPEN               6       /* Connection not open */
#define OBEX_PACKET_TOO_LARGE       7       /* Packet size large than MTU */
#define OBEX_ERROR_TL               8       /* Operation failed in transport layer */
#define OBEX_TRY_AGAIN              9       /* Operation failed, connection congestion, try again */


/*
* OBEX profile definitions
*/
#define OBEX_OPCODE_CONNECT             0x80
#define OBEX_OPCODE_DISCONNECT          0x81
#define OBEX_OPCODE_PUT                 0x02
#define OBEX_OPCODE_PUT_FINAL           0x82
#define OBEX_OPCODE_GET                 0x03
#define OBEX_OPCODE_GET_FINAL           0x83
#define OBEX_OPCODE_SETPATH             0x85
#define OBEX_OPCODE_ACTION              0x06
#define OBEX_OPCODE_SESSION             0x87
#define OBEX_OPCODE_ABORT               0xFF

#define OBEX_RESPONSE_CODE_CONTINUE                         0x10
#define OBEX_RESPONSE_CODE_OK                               0x20
#define OBEX_RESPONSE_CODE_CREATED                          0x21
#define OBEX_RESPONSE_CODE_ACCEPTED                         0x22
#define OBEX_RESPONSE_CODE_NON_AUTHORITATIVE_INFO           0x23
#define OBEX_RESPONSE_CODE_NO_CONTENT                       0x24
#define OBEX_RESPONSE_CODE_RESET_CONTENT                    0x25
#define OBEX_RESPONSE_CODE_PARTIAL_CONTENT                  0x26
#define OBEX_RESPONSE_CODE_MULTIPLE_CHOICES                 0x30
#define OBEX_RESPONSE_CODE_MOVED_PERMANENTLY                0x31
#define OBEX_RESPONSE_CODE_MOVED_TEMPORARILY                0x31
#define OBEX_RESPONSE_CODE_SEE_OHTER                        0x33
#define OBEX_RESPONSE_CODE_NOT_MODIFIED                     0x34
#define OBEX_RESPONSE_CODE_USE_PROXY                        0x35
#define OBEX_RESPONSE_CODE_BAD_REQUEST                      0x40
#define OBEX_RESPONSE_CODE_UNAUTHORIZED                     0x41
#define OBEX_RESPONSE_CODE_PAYMENT_REQUIRED                 0x42
#define OBEX_RESPONSE_CODE_FORBIDDEN                        0x43
#define OBEX_RESPONSE_CODE_NOT_FOUND                        0x44
#define OBEX_RESPONSE_CODE_METHOD_NOT_ALLOWED               0x45
#define OBEX_RESPONSE_CODE_NOT_ACCEPTABLE                   0x46
#define OBEX_RESPONSE_CODE_PROXY_AUTHENTICATION_REQUIRED    0x47
#define OBEX_RESPONSE_CODE_REQUEST_TIME_OUT                 0x48
#define OBEX_RESPONSE_CODE_CONFLICT                         0x49
#define OBEX_RESPONSE_CODE_GONE                             0x4A
#define OBEX_RESPONSE_CODE_LENGTH_REQUIRED                  0x4B
#define OBEX_RESPONSE_CODE_PRECONDITION_FAILED              0x4C
#define OBEX_RESPONSE_CODE_REQUESTED_ENTITY_TOO_LARGE       0x4D
#define OBEX_RESPONSE_CODE_REQUEST_URL_TOO_LARGE            0x4E
#define OBEX_RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE           0x4F
#define OBEX_RESPONSE_CODE_INTERNAL_SERVER_ERROR            0x50
#define OBEX_RESPONSE_CODE_NOT_IMPLEMENTED                  0x51
#define OBEX_RESPONSE_CODE_BAD_GATEWAY                      0x52
#define OBEX_RESPONSE_CODE_SERVICE_UNAVAILABLE              0x53
#define OBEX_RESPONSE_CODE_GATEWAY_TIMEOUT                  0x54
#define OBEX_RESPONSE_CODE_HTTP_VERSION_NOT_SUPPORTED       0x55
#define OBEX_RESPONSE_CODE_DATABASE_FULL                    0x60
#define OBEX_RESPONSE_CODE_DATABASE_LOCKED                  0x61

#define OBEX_FINAL_BIT_MASK             0x80

#define OBEX_CONNECT_FLAGS              0x01                /* support multiple link */
#define OBEX_SETPATH_FLAGS              0x03                /* default flags */

#define OBEX_PACKET_LENGTH_MAX          (0xFFFF-1)
#define OBEX_PACKET_LENGTH_MIN          255

#define OBEX_VERSION_NUMBER             0x15

/* Header identifiers */
#define OBEX_HEADER_ID_U2B_MASK             0xC0            /* upper 2 bits of header ID are user to indicate the header encoding */
#define OBEX_HEADER_ID_U2B_TYPE1            0x00            /* null terminated Unicode text, length prefixed with 2 byte unsigned integer */
#define OBEX_HEADER_ID_U2B_TYPE2            0x40            /* byte sequence, length prefixed with 2 byte unsigned integer */
#define OBEX_HEADER_ID_U2B_TYPE3            0x80            /* 1 byte quantity */
#define OBEX_HEADER_ID_U2B_TYPE4            0xC0            /* 4 byte quantity - transmitted in network byte order (high byte first) */

#define OBEX_HEADER_ID_COUNT                0xC0
#define OBEX_HEADER_ID_NAME                 0x01
#define OBEX_HEADER_ID_TYPE                 0x42
#define OBEX_HEADER_ID_LENGTH               0xC3
#define OBEX_HEADER_ID_TIME_ISO8601         0x44
#define OBEX_HEADER_ID_TIME_4BYTE           0xC4
#define OBEX_HEADER_ID_DESCRIPTION          0x05
#define OBEX_HEADER_ID_TARGET               0x46
#define OBEX_HEADER_ID_HTTP                 0x47
#define OBEX_HEADER_ID_BODY                 0x48
#define OBEX_HEADER_ID_END_OF_BODY          0x49
#define OBEX_HEADER_ID_WHO                  0x4A
#define OBEX_HEADER_ID_CONNECTION_ID        0xCB
#define OBEX_HEADER_ID_APP_PARAM            0x4C
#define OBEX_HEADER_ID_AUTH_CHALLENGE       0x4D
#define OBEX_HEADER_ID_AUTH_RESPONSE        0x4E
#define OBEX_HEADER_ID_CREATOR_ID           0xCF
#define OBEX_HEADER_ID_WAN_UUID             0x50
#define OBEX_HEADER_ID_OBJECT_CLASS         0x51
#define OBEX_HEADER_ID_SESSION_PARAM        0x52
#define OBEX_HEADER_ID_SESSION_SEQ_NUM      0x93
#define OBEX_HEADER_ID_ACTION_ID            0x94
#define OBEX_HEADER_ID_DESTNAME             0x15
#define OBEX_HEADER_ID_PERMISSIONS          0xD6
#define OBEX_HEADER_ID_SRM                  0x97
#define OBEX_HEADER_ID_SRM_PARAM            0x98
/* Reserved for future use: 0x19 to 0x2F */
/* User defined: 0x30 to 0x3F */

#define OBEX_ACTION_ID_COPY                 0x00
#define OBEX_ACTION_ID_MOVE_RENAME          0x01
#define OBEX_ACTION_ID_SET_PERMISSIONS      0x02

#define OBEX_SRM_DISABLE                    0x00
#define OBEX_SRM_ENABLE                     0x01
#define OBEX_SRM_SUPPORT                    0x02

#define OBEX_SRMP_ADD_PKT                   0x00
#define OBEX_SRMP_WAIT                      0x01
#define OBEX_SRMP_ADD_PKT_WAIT              0x02

#define OBEX_MIN_PACKET_SIZE                3

enum {
    /* client event */
    OBEX_CONNECT_EVT,                   /* connection opened */
    OBEX_DISCONNECT_EVT,                /* connection disconnected */
    /* server event */
    OBEX_CONN_INCOME_EVT,               /* an incoming connection */
    /* client or server event */
    OBEX_MTU_CHANGE_EVT,                /* connection mtu changed */
    OBEX_CONGEST_EVT,                   /* connection congested */
    OBEX_UNCONGEST_EVT,                 /* connection is not congested */
    OBEX_DATA_EVT                       /* data received */
};

enum {
    OBEX_OVER_L2CAP = 0,
    OBEX_OVER_RFCOMM,
    OBEX_NUM_TL
};

typedef struct
{
    UINT16 psm;                         /* l2cap psm */
    UINT16 sec_mask;                    /* security mask */
    UINT16 pref_mtu;                    /* preferred mtu, limited by L2CAP_MTU_SIZE */
    BD_ADDR addr;                       /* peer bluetooth device address */
} tOBEX_OVER_L2CAP_SVR;

typedef struct
{
    UINT8 tl;                           /* transport type, OBEX_OVER_L2CAP or OBEX_OVER_RFCOMM */
    union
    {
        tOBEX_OVER_L2CAP_SVR l2cap;
    };
} tOBEX_SVR_INFO;

typedef struct {
    UINT8   opcode;
    UINT8   response_code;
    /* Connect */
    UINT8   obex_version_number;
    UINT16  max_packet_length;
    /* Connect or SetPath */
    UINT8   flags;
    /* Internal use */
    UINT16  next_header_pos;
} tOBEX_PARSE_INFO;

typedef union {
    struct {
        UINT16 peer_mtu;
        UINT16 our_mtu;
    } connect;

    struct {
        UINT16 svr_handle;
        UINT16 peer_mtu;
        UINT16 our_mtu;
    } conn_income;

    struct {
        UINT16 peer_mtu;
        UINT16 our_mtu;
    } mtu_change;

    struct {
        BT_HDR *pkt;
    } data;
} tOBEX_MSG;

typedef void (tOBEX_MSG_CBACK)(UINT16 handle, UINT8 event, tOBEX_MSG *msg);

/*******************************************************************************
*       The following APIs are called by bluetooth stack automatically
*******************************************************************************/

extern UINT16 OBEX_Init(void);

extern void OBEX_Deinit(void);

/*******************************************************************************
*               The following APIs must be executed in btu task
*******************************************************************************/

extern UINT16 OBEX_CreateConn(tOBEX_SVR_INFO *server, tOBEX_MSG_CBACK callback, UINT16 *out_handle);

extern UINT16 OBEX_RemoveConn(UINT16 handle);

extern UINT16 OBEX_SendPacket(UINT16 handle, BT_HDR *pkt);

extern UINT16 OBEX_RegisterServer(tOBEX_SVR_INFO *server, tOBEX_MSG_CBACK callback, UINT16 *out_svr_handle);

extern UINT16 OBEX_DeregisterServer(UINT16 svr_handle);

/*******************************************************************************
*    The following APIs are util function, can be executed in btu or btc task
*******************************************************************************/

extern UINT16 OBEX_BuildRequest(tOBEX_PARSE_INFO *info, UINT16 buff_size, BT_HDR **out_pkt);

extern UINT16 OBEX_BuildResponse(tOBEX_PARSE_INFO *info, UINT16 buff_size, BT_HDR **out_pkt);

extern UINT16 OBEX_AppendHeader(BT_HDR *pkt, const UINT8 *header);

extern UINT16 OBEX_AppendHeaderRaw(BT_HDR *pkt, UINT8 header_id, const UINT8 *data, UINT16 data_len);

extern UINT16 OBEX_AppendHeaderSRM(BT_HDR *pkt, UINT8 value);

extern UINT16 OBEX_AppendHeaderSRMP(BT_HDR *pkt, UINT8 value);

extern UINT16 OBEX_GetPacketFreeSpace(BT_HDR *pkt);

extern UINT16 OBEX_GetPacketLength(BT_HDR *pkt);

extern UINT16 OBEX_ParseRequest(BT_HDR *pkt, tOBEX_PARSE_INFO *info);

extern UINT16 OBEX_ParseResponse(BT_HDR *pkt, UINT8 opcode, tOBEX_PARSE_INFO *info);

extern BOOLEAN OBEX_CheckFinalBit(BT_HDR *pkt);

extern BOOLEAN OBEX_CheckContinueResponse(BT_HDR *pkt);

extern UINT8 *OBEX_GetNextHeader(BT_HDR *pkt, tOBEX_PARSE_INFO *info);

extern UINT16 OBEX_GetHeaderLength(UINT8 *header);

#endif /* #if (OBEX_INCLUDED == TRUE) */
