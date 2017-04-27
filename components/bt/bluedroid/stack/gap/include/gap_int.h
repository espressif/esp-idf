/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
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


#ifndef GAP_INT_H
#define GAP_INT_H

#include "bt_target.h"
#include "gap_api.h"
#include "gki.h"
#include "gatt_api.h"
#define GAP_MAX_BLOCKS 2        /* Concurrent GAP commands pending at a time*/
/* Define the Generic Access Profile control structure */
typedef struct {
    void          *p_data;      /* Pointer to any data returned in callback */
    tGAP_CALLBACK *gap_cback;   /* Pointer to users callback function */
    tGAP_CALLBACK *gap_inq_rslt_cback; /* Used for inquiry results */
    UINT16         event;       /* Passed back in the callback */
    UINT8          index;       /* Index of this control block and callback */
    BOOLEAN        in_use;      /* True when structure is allocated */
} tGAP_INFO;

/* Define the control block for the FindAddrByName operation (Only 1 active at a time) */
typedef struct {
    tGAP_CALLBACK           *p_cback;
    tBTM_INQ_INFO           *p_cur_inq; /* Pointer to the current inquiry database entry */
    tGAP_FINDADDR_RESULTS    results;
    BOOLEAN                  in_use;
} tGAP_FINDADDR_CB;

/* Define the GAP Connection Control Block.
*/
typedef struct {
#define GAP_CCB_STATE_IDLE              0
#define GAP_CCB_STATE_LISTENING         1
#define GAP_CCB_STATE_CONN_SETUP        2
#define GAP_CCB_STATE_CFG_SETUP         3
#define GAP_CCB_STATE_WAIT_SEC          4
#define GAP_CCB_STATE_CONNECTED         5
    UINT8             con_state;

#define GAP_CCB_FLAGS_IS_ORIG           0x01
#define GAP_CCB_FLAGS_HIS_CFG_DONE      0x02
#define GAP_CCB_FLAGS_MY_CFG_DONE       0x04
#define GAP_CCB_FLAGS_SEC_DONE          0x08
#define GAP_CCB_FLAGS_CONN_DONE         0x0E
    UINT8             con_flags;

    UINT8             service_id;           /* Used by BTM                          */
    UINT16            gap_handle;           /* GAP handle                           */
    UINT16            connection_id;        /* L2CAP CID                            */
    BOOLEAN           rem_addr_specified;
    UINT8             chan_mode_mask;       /* Supported channel modes (FCR)        */
    BD_ADDR           rem_dev_address;
    UINT16            psm;
    UINT16            rem_mtu_size;

    BOOLEAN           is_congested;
    BUFFER_Q          tx_queue;             /* Queue of buffers waiting to be sent  */
    BUFFER_Q          rx_queue;             /* Queue of buffers waiting to be read  */

    UINT32            rx_queue_size;        /* Total data count in rx_queue         */

    tGAP_CONN_CALLBACK *p_callback;         /* Users callback function              */

    tL2CAP_CFG_INFO   cfg;                  /* Configuration                        */
    tL2CAP_ERTM_INFO  ertm_info;            /* Pools and modes for ertm */
} tGAP_CCB;

typedef struct {
#if ((defined AMP_INCLUDED) && (AMP_INCLUDED == TRUE))
    tAMP_APPL_INFO    reg_info;
#else
    tL2CAP_APPL_INFO  reg_info;                     /* L2CAP Registration info */
#endif
    tGAP_CCB    ccb_pool[GAP_MAX_CONNECTIONS];
} tGAP_CONN;


#if BLE_INCLUDED == TRUE
#define GAP_MAX_CHAR_NUM          4

typedef struct {
    UINT16                  handle;
    UINT16                  uuid;
    tGAP_BLE_ATTR_VALUE     attr_value;
} tGAP_ATTR;
#endif
/**********************************************************************
** M A I N   C O N T R O L   B L O C K
***********************************************************************/

#define GAP_MAX_CL GATT_CL_MAX_LCB

typedef struct {
    UINT16 uuid;
    tGAP_BLE_CMPL_CBACK *p_cback;
} tGAP_BLE_REQ;

typedef struct {
    BD_ADDR                 bda;
    tGAP_BLE_CMPL_CBACK     *p_cback;
    UINT16                  conn_id;
    UINT16                  cl_op_uuid;
    BOOLEAN                 in_use;
    BOOLEAN                 connected;
    BUFFER_Q                pending_req_q;

} tGAP_CLCB;

typedef struct {
    tGAP_INFO        blk[GAP_MAX_BLOCKS];
    tBTM_CMPL_CB    *btm_cback[GAP_MAX_BLOCKS];
    UINT8            trace_level;
    //tGAP_FINDADDR_CB findaddr_cb;   /* Contains the control block for finding a device addr */
    //tBTM_INQ_INFO   *cur_inqptr;

#if GAP_CONN_INCLUDED == TRUE
    tGAP_CONN        conn;
#endif

    /* LE GAP attribute database */
#if BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE
    tGAP_ATTR               gatt_attr[GAP_MAX_CHAR_NUM];
    tGAP_CLCB               clcb[GAP_MAX_CL]; /* connection link*/
    tGATT_IF                gatt_if;
#endif
} tGAP_CB;


extern tGAP_CB  gap_cb;
#if (GAP_CONN_INCLUDED == TRUE)
extern void gap_conn_init(void);
#endif
#if (BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
extern void gap_attr_db_init(void);
#endif

#endif
