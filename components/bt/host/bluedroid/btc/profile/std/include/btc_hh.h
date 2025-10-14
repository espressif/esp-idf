/******************************************************************************
 *
 *  Copyright (C) 2016 The Android Open Source Project
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *  Copyright (C) 2019 Blake Felt
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
#ifndef BTC_HH_H
#define BTC_HH_H

#include <stdint.h>
#include "bta/bta_hh_api.h"
#include "btc/btc_task.h"
#include "osi/alarm.h"
#include "esp_hidh_api.h"

#define BTC_HH_MAX_HID 8
#define BTC_HH_MAX_ADDED_DEV 32

#define BTC_HH_MAX_KEYSTATES 3
#define BTC_HH_KEYSTATE_MASK_NUMLOCK 0x01
#define BTC_HH_KEYSTATE_MASK_CAPSLOCK 0x02
#define BTC_HH_KEYSTATE_MASK_SCROLLLOCK 0x04

#define BTC_HH_MAX_POLLING_ATTEMPTS 10
#define BTC_HH_POLLING_SLEEP_DURATION_US 5000

/*******************************************************************************
 *  Type definitions and return values
 ******************************************************************************/
typedef enum {
    BTC_HH_INIT_EVT = 0,
    BTC_HH_CONNECT_EVT,
    BTC_HH_DISCONNECT_EVT,
    BTC_HH_UNPLUG_EVT,
    BTC_HH_SET_INFO_EVT,
    BTC_HH_GET_PROTO_EVT,
    BTC_HH_SET_PROTO_EVT,
    BTC_HH_GET_IDLE_EVT,
    BTC_HH_SET_IDLE_EVT,
    BTC_HH_GET_REPORT_EVT,
    BTC_HH_SET_REPORT_EVT,
    BTC_HH_SEND_DATA_EVT,
    BTC_HH_DEINIT_EVT,
} BTC_HH_EVT;

typedef enum {
    BTC_HH_DISABLED = 0,
    BTC_HH_ENABLED,
    BTC_HH_DISABLING,
    BTC_HH_DEV_UNKNOWN,
    BTC_HH_DEV_CONNECTING,
    BTC_HH_DEV_CONNECTED,
    BTC_HH_DEV_DISCONNECTED
} BTC_HH_STATUS;

typedef struct {
    esp_hidh_connection_state_t dev_status;
    uint8_t dev_handle;
    BD_ADDR bd_addr;
    uint16_t attr_mask;
    uint8_t sub_class;
    uint8_t app_id;
    bool ready_for_data;
    osi_alarm_t *vup_timer;
    bool local_vup; // Indicated locally initiated VUP
} btc_hh_device_t;

/* Control block to maintain properties of devices */
typedef struct {
    uint8_t dev_handle;
    BD_ADDR bd_addr;
    uint16_t attr_mask;
} btc_hh_added_device_t;

/**
 * BTC-HH control block to maintain added devices and currently
 * connected hid devices
 */
typedef struct {
    BTC_HH_STATUS status;
    btc_hh_device_t devices[BTC_HH_MAX_HID];
    uint32_t device_num;
    BTC_HH_EVT add_event;
    btc_hh_added_device_t added_devices[BTC_HH_MAX_ADDED_DEV];
    btc_hh_device_t *p_curr_dev;
    bool service_dereg_active;
    BD_ADDR pending_conn_address;
} btc_hh_cb_t;

/* btc_spp_args_t */
typedef union {
    // BTC_HH_CONNECT_EVT
    struct hh_connect_arg {
        BD_ADDR bd_addr;
    } connect;

    // BTC_HH_DISCONNECT_EVT
    struct disconnect_arg {
        BD_ADDR bd_addr;
    } disconnect;

    // BTC_HH_UNPLUG_EVT
    struct unplug_arg {
        BD_ADDR bd_addr;
    } unplug;

    // BTC_HH_SET_INFO_EVT
    struct set_info_arg {
        BD_ADDR bd_addr;
        esp_hidh_hid_info_t *hid_info;
    } set_info;

    // BTC_HH_GET_PROTO_EVT
    struct get_protocol_arg {
        BD_ADDR bd_addr;
    } get_protocol;

    // BTC_HH_SET_PROTO_EVT
    struct set_protocol_arg {
        BD_ADDR bd_addr;
        esp_hidh_protocol_mode_t protocol_mode;
    } set_protocol;

    // BTC_HH_GET_IDLE_EVT
    struct get_idle_arg {
        BD_ADDR bd_addr;
    } get_idle;

    // BTC_HH_SET_IDLE_EVT
    struct set_idle_arg {
        BD_ADDR bd_addr;
        uint16_t idle_time;
    } set_idle;

    // BTC_HH_GET_REPORT_EVT
    struct get_report_arg {
        BD_ADDR bd_addr;
        esp_hidh_report_type_t report_type;
        uint8_t report_id;
        int buffer_size;
    } get_report;

    // BTC_HH_SET_REPORT_EVT
    struct set_report_arg {
        BD_ADDR bd_addr;
        esp_hidh_report_type_t report_type;
        size_t len;
        uint8_t *report;
    } set_report;

    // BTC_HH_SEND_DATA_EVT
    struct send_data_arg {
        BD_ADDR bd_addr;
        size_t len;
        uint8_t *data;
    } send_data;
} btc_hidh_args_t;
/*******************************************************************************
 *  Variables
 ******************************************************************************/
extern btc_hh_cb_t btc_hh_cb;
/*******************************************************************************
 *  Functions
 ******************************************************************************/

void btc_hh_call_handler(btc_msg_t *msg);

void btc_hh_cb_handler(btc_msg_t *msg);

void btc_hh_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

void btc_hh_cb_arg_deep_free(btc_msg_t *msg);

bool btc_hh_add_added_dev(BD_ADDR bd_addr, uint16_t attr_mask);

void btc_hh_get_profile_status(esp_hidh_profile_status_t *param);

#endif /* BTC_HH_H */
