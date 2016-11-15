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
 *  This file contains HID HOST internal definitions
 *
 ******************************************************************************/

#ifndef HIDH_INT_H
#define HIDH_INT_H

#include "hidh_api.h"
#include "hid_conn.h"
#include "l2c_api.h"

enum {
    HID_DEV_NO_CONN,
    HID_DEV_CONNECTED
};

typedef struct per_device_ctb
{
    BOOLEAN        in_use;
    BD_ADDR        addr;  /* BD-Addr of the host device */
    UINT16         attr_mask; /* 0x01- virtual_cable; 0x02- normally_connectable; 0x03- reconn_initiate;
    			                 0x04- sdp_disable; */
    UINT8          state;  /* Device state if in HOST-KNOWN mode */
    UINT8          conn_substate;
    UINT8          conn_tries; /* Remembers to the number of connection attempts while CONNECTING */

    tHID_CONN      conn; /* L2CAP channel info */
} tHID_HOST_DEV_CTB;

typedef struct host_ctb
{
    tHID_HOST_DEV_CTB       devices[HID_HOST_MAX_DEVICES];
    tHID_HOST_DEV_CALLBACK  *callback;             /* Application callbacks */
    tL2CAP_CFG_INFO         l2cap_cfg;

#define MAX_SERVICE_DB_SIZE    4000

    BOOLEAN                 sdp_busy;
    tHID_HOST_SDP_CALLBACK  *sdp_cback;
    tSDP_DISCOVERY_DB       *p_sdp_db;
    tHID_DEV_SDP_INFO       sdp_rec;
    BOOLEAN                 reg_flag;
    UINT8                   trace_level;
} tHID_HOST_CTB;

extern tHID_STATUS hidh_conn_snd_data(UINT8 dhandle, UINT8 trans_type, UINT8 param, \
                                      UINT16 data,UINT8 rpt_id, BT_HDR *buf);
extern tHID_STATUS hidh_conn_reg (void);
extern void hidh_conn_dereg( void );
extern tHID_STATUS hidh_conn_disconnect (UINT8 dhandle);
extern tHID_STATUS hidh_conn_initiate (UINT8 dhandle);
extern void hidh_proc_repage_timeout (TIMER_LIST_ENT *p_tle);

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
** Main Control Block
*******************************************************************************/
#if HID_DYNAMIC_MEMORY == FALSE
extern tHID_HOST_CTB  hh_cb;
#else
extern tHID_HOST_CTB *hidh_cb_ptr;
#define hh_cb (*hidh_cb_ptr)
#endif

#ifdef __cplusplus
}
#endif

#endif
