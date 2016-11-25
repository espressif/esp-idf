/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains HID protocol definitions
 *
 ******************************************************************************/

#ifndef HIDDEFS_H
#define HIDDEFS_H

#include "sdp_api.h"
/*
** tHID_STATUS: HID result codes, returned by HID and device and host functions.
*/
enum {
    HID_SUCCESS,
    HID_ERR_NOT_REGISTERED,
    HID_ERR_ALREADY_REGISTERED,
    HID_ERR_NO_RESOURCES,
    HID_ERR_NO_CONNECTION,
    HID_ERR_INVALID_PARAM,
    HID_ERR_UNSUPPORTED,
    HID_ERR_UNKNOWN_COMMAND,
    HID_ERR_CONGESTED,
    HID_ERR_CONN_IN_PROCESS,
    HID_ERR_ALREADY_CONN,
    HID_ERR_DISCONNECTING,
    HID_ERR_SET_CONNABLE_FAIL,
    /* Device specific error codes */
    HID_ERR_HOST_UNKNOWN,
    HID_ERR_L2CAP_FAILED,
    HID_ERR_AUTH_FAILED,
    HID_ERR_SDP_BUSY,
    HID_ERR_GATT,

    HID_ERR_INVALID = 0xFF
};

typedef UINT8 tHID_STATUS;

#define    HID_L2CAP_CONN_FAIL (0x0100) /* Connection Attempt was made but failed */
#define    HID_L2CAP_REQ_FAIL  (0x0200)  /* L2CAP_ConnectReq API failed */
#define    HID_L2CAP_CFG_FAIL  (0x0400) /* L2CAP Configuration was rejected by peer */



/* Define the HID transaction types
*/
#define HID_TRANS_HANDSHAKE     (0)
#define HID_TRANS_CONTROL       (1)
#define HID_TRANS_GET_REPORT    (4)
#define HID_TRANS_SET_REPORT    (5)
#define HID_TRANS_GET_PROTOCOL  (6)
#define HID_TRANS_SET_PROTOCOL  (7)
#define HID_TRANS_GET_IDLE      (8)
#define HID_TRANS_SET_IDLE      (9)
#define HID_TRANS_DATA          (10)
#define HID_TRANS_DATAC         (11)

#define HID_GET_TRANS_FROM_HDR(x) ((x >> 4) & 0x0f)
#define HID_GET_PARAM_FROM_HDR(x) (x & 0x0f)
#define HID_BUILD_HDR(t,p)  (UINT8)((t << 4) | (p & 0x0f))


/* Parameters for Handshake
*/
#define HID_PAR_HANDSHAKE_RSP_SUCCESS               (0)
#define HID_PAR_HANDSHAKE_RSP_NOT_READY             (1)
#define HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID    (2)
#define HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ   (3)
#define HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM     (4)
#define HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN           (14)
#define HID_PAR_HANDSHAKE_RSP_ERR_FATAL             (15)


/* Parameters for Control
*/
#define HID_PAR_CONTROL_NOP                         (0)
#define HID_PAR_CONTROL_HARD_RESET                  (1)
#define HID_PAR_CONTROL_SOFT_RESET                  (2)
#define HID_PAR_CONTROL_SUSPEND                     (3)
#define HID_PAR_CONTROL_EXIT_SUSPEND                (4)
#define HID_PAR_CONTROL_VIRTUAL_CABLE_UNPLUG        (5)


/* Different report types in get, set, data
*/
#define HID_PAR_REP_TYPE_MASK                       (0x03)
#define HID_PAR_REP_TYPE_OTHER                      (0x00)
#define HID_PAR_REP_TYPE_INPUT                      (0x01)
#define HID_PAR_REP_TYPE_OUTPUT                     (0x02)
#define HID_PAR_REP_TYPE_FEATURE                    (0x03)

/* Parameters for Get Report
*/

/* Buffer size in two bytes after Report ID */
#define HID_PAR_GET_REP_BUFSIZE_FOLLOWS             (0x08)


/* Parameters for Protocol Type
*/
#define HID_PAR_PROTOCOL_MASK                       (0x01)
#define HID_PAR_PROTOCOL_REPORT                     (0x01)
#define HID_PAR_PROTOCOL_BOOT_MODE                  (0x00)

#define HID_PAR_REP_TYPE_MASK                       (0x03)

/* Descriptor types in the SDP record
*/
#define HID_SDP_DESCRIPTOR_REPORT                   (0x22)
#define HID_SDP_DESCRIPTOR_PHYSICAL                 (0x23)

typedef struct desc_info {
    UINT16 dl_len;
    UINT8 *dsc_list;
} tHID_DEV_DSCP_INFO;

#define HID_SSR_PARAM_INVALID    0xffff

typedef struct sdp_info {
    char svc_name[HID_MAX_SVC_NAME_LEN];   /*Service Name */
    char svc_descr[HID_MAX_SVC_DESCR_LEN]; /*Service Description*/
    char prov_name[HID_MAX_PROV_NAME_LEN]; /*Provider Name.*/
    UINT16    rel_num;    /*Release Number */
    UINT16    hpars_ver;  /*HID Parser Version.*/
    UINT16    ssr_max_latency; /* HIDSSRHostMaxLatency value, if HID_SSR_PARAM_INVALID not used*/
    UINT16    ssr_min_tout; /* HIDSSRHostMinTimeout value, if HID_SSR_PARAM_INVALID not used* */
    UINT8     sub_class;    /*Device Subclass.*/
    UINT8     ctry_code;     /*Country Code.*/
    UINT16    sup_timeout;/* Supervisory Timeout */

    tHID_DEV_DSCP_INFO  dscp_info;   /* Descriptor list and Report list to be set in the SDP record.
                                       This parameter is used if HID_DEV_USE_GLB_SDP_REC is set to FALSE.*/
    tSDP_DISC_REC       *p_sdp_layer_rec;
} tHID_DEV_SDP_INFO;

#endif

