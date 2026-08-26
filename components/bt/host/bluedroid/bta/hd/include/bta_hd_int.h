/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
 *  Copyright (C) 2005-2012 Broadcom Corporation
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
 *  This file contains BTA HID Device internal definitions
 *
 ******************************************************************************/
#ifndef BTA_HD_INT_H
#define BTA_HD_INT_H

#include "bta/bta_hd_api.h"
#include "bta/bta_sys.h"
#include "stack/hiddefs.h"

enum {
    BTA_HD_API_REGISTER_APP_EVT = BTA_SYS_EVT_START(BTA_ID_HD),
    BTA_HD_API_UNREGISTER_APP_EVT,
    BTA_HD_API_CONNECT_EVT,
    BTA_HD_API_DISCONNECT_EVT,
    BTA_HD_API_ADD_DEVICE_EVT,
    BTA_HD_API_REMOVE_DEVICE_EVT,
    BTA_HD_API_SEND_REPORT_EVT,
    BTA_HD_API_REPORT_ERROR_EVT,
    BTA_HD_API_VC_UNPLUG_EVT,
    BTA_HD_INT_OPEN_EVT,
    BTA_HD_INT_CLOSE_EVT,
    BTA_HD_INT_INTR_DATA_EVT,
    BTA_HD_INT_GET_REPORT_EVT,
    BTA_HD_INT_SET_REPORT_EVT,
    BTA_HD_INT_SET_PROTOCOL_EVT,
    BTA_HD_INT_VC_UNPLUG_EVT,
    BTA_HD_INT_SUSPEND_EVT,
    BTA_HD_INT_EXIT_SUSPEND_EVT,
    /* handled outside state machine */
    BTA_HD_API_ENABLE_EVT,
    BTA_HD_API_DISABLE_EVT
};
typedef uint16_t tBTA_HD_INT_EVT;
#define BTA_HD_INVALID_EVT (BTA_HD_API_DISABLE_EVT + 1)
typedef struct {
    BT_HDR hdr;
    tBTA_HD_CBACK *p_cback;
} tBTA_HD_API_ENABLE;
#define BTA_HD_APP_NAME_LEN 50
#define BTA_HD_APP_DESCRIPTION_LEN 50
#define BTA_HD_APP_PROVIDER_LEN 50
#define BTA_HD_APP_DESCRIPTOR_LEN 2048
#define BTA_HD_STATE_DISABLED 0x00
#define BTA_HD_STATE_ENABLED 0x01
#define BTA_HD_STATE_IDLE 0x02
#define BTA_HD_STATE_CONNECTED 0x03
#define BTA_HD_STATE_DISABLING 0x04
#define BTA_HD_STATE_REMOVING 0x05
typedef struct {
    BT_HDR hdr;
    char name[BTA_HD_APP_NAME_LEN + 1];
    char description[BTA_HD_APP_DESCRIPTION_LEN + 1];
    char provider[BTA_HD_APP_PROVIDER_LEN + 1];
    uint8_t subclass;
    uint16_t d_len;
    uint8_t d_data[BTA_HD_APP_DESCRIPTOR_LEN];
    tBTA_HD_QOS_INFO in_qos;
    tBTA_HD_QOS_INFO out_qos;
} tBTA_HD_REGISTER_APP;

#define BTA_HD_REPORT_LEN HID_DEV_MTU_SIZE

typedef struct {
    BT_HDR hdr;
    bool use_intr;
    uint8_t type;
    uint8_t id;
    uint16_t len;
    uint8_t data[BTA_HD_REPORT_LEN];
} tBTA_HD_SEND_REPORT;

typedef struct {
    BT_HDR hdr;
    BD_ADDR addr;
} tBTA_HD_DEVICE_CTRL;

typedef struct {
    BT_HDR hdr;
    uint8_t error;
} tBTA_HD_REPORT_ERR;

/* union of all event data types */
typedef union {
    BT_HDR hdr;
    tBTA_HD_API_ENABLE api_enable;
    tBTA_HD_REGISTER_APP register_app;
    tBTA_HD_SEND_REPORT send_report;
    tBTA_HD_DEVICE_CTRL device_ctrl;
    tBTA_HD_REPORT_ERR report_err;
} tBTA_HD_DATA;

typedef struct {
    BT_HDR hdr;
    BD_ADDR addr;
    uint32_t data;
    BT_HDR *p_data;
} tBTA_HD_CBACK_DATA;

/******************************************************************************
 * Main Control Block
 ******************************************************************************/
typedef struct {
    tBTA_HD_CBACK *p_cback;
    uint32_t sdp_handle;
    uint8_t trace_level;
    uint8_t state;
    BD_ADDR bd_addr;
    bool use_report_id;
    bool boot_mode;
    bool vc_unplug;
    bool disable_w4_close;
} tBTA_HD_CB;

#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_HD_CB bta_hd_cb;
#else
extern tBTA_HD_CB *bta_hd_cb_ptr;
#define bta_hd_cb (*bta_hd_cb_ptr)
#endif

/*****************************************************************************
 *  Function prototypes
 ****************************************************************************/
extern bool bta_hd_hdl_event(BT_HDR *p_msg);
extern void bta_hd_api_enable(tBTA_HD_DATA *p_data);
extern void bta_hd_api_disable(void);
extern void bta_hd_register_act(tBTA_HD_DATA *p_data);
extern void bta_hd_unregister_act(tBTA_HD_DATA *p_data);
extern void bta_hd_unregister2_act(tBTA_HD_DATA *p_data);
extern void bta_hd_connect_act(tBTA_HD_DATA *p_data);
extern void bta_hd_disconnect_act(tBTA_HD_DATA *p_data);
extern void bta_hd_add_device_act(tBTA_HD_DATA *p_data);
extern void bta_hd_remove_device_act(tBTA_HD_DATA *p_data);
extern void bta_hd_send_report_act(tBTA_HD_DATA *p_data);
extern void bta_hd_report_error_act(tBTA_HD_DATA *p_data);
extern void bta_hd_vc_unplug_act(tBTA_HD_DATA *p_data);
extern void bta_hd_open_act(tBTA_HD_DATA *p_data);
extern void bta_hd_close_act(tBTA_HD_DATA *p_data);
extern void bta_hd_intr_data_act(tBTA_HD_DATA *p_data);
extern void bta_hd_get_report_act(tBTA_HD_DATA *p_data);
extern void bta_hd_set_report_act(tBTA_HD_DATA *p_data);
extern void bta_hd_set_protocol_act(tBTA_HD_DATA *p_data);
extern void bta_hd_vc_unplug_done_act(tBTA_HD_DATA *p_data);
extern void bta_hd_suspend_act(tBTA_HD_DATA *p_data);
extern void bta_hd_exit_suspend_act(tBTA_HD_DATA *p_data);
extern void bta_hd_open_failure(tBTA_HD_DATA *p_data);

#endif
