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
 *  This file contains HID connection internal definitions
 *
 ******************************************************************************/

#ifndef HID_CONN_H
#define HID_CONN_H

#include "common/bt_defs.h"
#if (HID_HOST_INCLUDED == TRUE)

/* Define the HID Connection Block
*/
typedef struct hid_conn {
#define HID_CONN_STATE_UNUSED           (0)
#define HID_CONN_STATE_CONNECTING_CTRL  (1)
#define HID_CONN_STATE_CONNECTING_INTR  (2)
#define HID_CONN_STATE_CONFIG           (3)
#define HID_CONN_STATE_CONNECTED        (4)
#define HID_CONN_STATE_DISCONNECTING    (5)
#define HID_CONN_STATE_SECURITY         (6)

    UINT8             conn_state;

#define HID_CONN_FLAGS_IS_ORIG              (0x01)
#define HID_CONN_FLAGS_HIS_CTRL_CFG_DONE    (0x02)
#define HID_CONN_FLAGS_MY_CTRL_CFG_DONE     (0x04)
#define HID_CONN_FLAGS_HIS_INTR_CFG_DONE    (0x08)
#define HID_CONN_FLAGS_MY_INTR_CFG_DONE     (0x10)
#define HID_CONN_FLAGS_ALL_CONFIGURED       (0x1E)        /* All the config done */
#define HID_CONN_FLAGS_CONGESTED            (0x20)
#define HID_CONN_FLAGS_INACTIVE             (0x40)

    UINT8             conn_flags;

    UINT8             ctrl_id;
    UINT16            ctrl_cid;
    UINT16            intr_cid;
    UINT16            rem_mtu_size;
    UINT16            disc_reason;                       /* Reason for disconnecting (for HID_HDEV_EVT_CLOSE) */
    TIMER_LIST_ENT    timer_entry;
} tHID_CONN;

#define HID_SEC_CHN   1
#define HID_NOSEC_CHN 2

#define HIDD_SEC_CHN    3
#define HIDD_NOSEC_CHN  4

#endif  ///HID_HOST_INCLUDED == TRUE
#endif
