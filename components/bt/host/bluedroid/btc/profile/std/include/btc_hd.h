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
#ifndef BTC_HD_H
#define BTC_HD_H

#if BTC_HD_INCLUDED == TRUE

#include <stdint.h>
#include "bta/bta_hd_api.h"
#include "btc/btc_task.h"
#include "esp_hidd_api.h"

typedef enum {
    BTC_HD_INIT_EVT = 0,
    BTC_HD_DEINIT_EVT,
    BTC_HD_REGISTER_APP_EVT,
    BTC_HD_UNREGISTER_APP_EVT,
    BTC_HD_CONNECT_EVT,
    BTC_HD_DISCONNECT_EVT,
    BTC_HD_SEND_REPORT_EVT,
    BTC_HD_REPORT_ERROR_EVT,
    BTC_HD_UNPLUG_EVT,
} BTC_HD_EVT;

typedef enum {
    BTC_HD_DISABLED = 0,
    BTC_HD_ENABLED,
    BTC_HD_DISABLING,
    BTC_HD_CONNECTING,
    BTC_HD_CONNECTED,
    BTC_HD_DISCONNECTING,
    BTC_HD_DISCONNECTED,
} BTC_HD_STATUS;

/* BTIF-HD control block */
typedef struct {
    BTC_HD_STATUS status;
    bool app_registered;
    bool service_dereg_active;
    bool forced_disc;
    bool in_use;
    tBTA_HD_APP_INFO app_info;
    tBTA_HD_QOS_INFO in_qos;
    tBTA_HD_QOS_INFO out_qos;
} btc_hd_cb_t;

/* btc_hidd_args_t */
typedef union {
    // BTC_HD_CONNECT_EVT
    struct hd_connect_arg {
        BD_ADDR bd_addr;
    } connect;

    // BTC_HD_REGISTER_APP_EVT
    struct register_app_arg {
        esp_hidd_app_param_t *app_param;
        esp_hidd_qos_param_t *in_qos;
        esp_hidd_qos_param_t *out_qos;
    } register_app;

    // BTC_HD_SEND_REPORT_EVT
    struct send_report_arg {
        esp_hidd_report_type_t type;
        uint8_t id;
        uint16_t len;
        uint8_t *data;
    } send_report;

    // BTC_HD_REPORT_ERROR_EVT
    uint8_t error;
} btc_hidd_args_t;

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Functions
 ******************************************************************************/
void btc_hd_call_handler(btc_msg_t *msg);

void btc_hd_cb_handler(btc_msg_t *msg);

void btc_hd_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);
void btc_hd_cb_arg_deep_free(btc_msg_t *msg);

void btc_hd_get_profile_status(esp_hidd_profile_status_t *param);

#ifdef __cplusplus
}
#endif

#endif /* BTC_HD_INCLUDED == TRUE */
#endif /* BTC_HD_H */
