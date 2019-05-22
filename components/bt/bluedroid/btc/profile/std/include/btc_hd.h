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

#include <stdint.h>
#include "bta/bta_hd_api.h"
#include "btc/btc_task.h"

typedef enum {
  BTC_HD_INIT_EVT = 0,
  BTC_HD_CLEANUP_EVT,
  BTC_HD_REGISTER_EVT,
  BTC_HD_UNREGISTER_EVT,
  BTC_HD_CONNECT_EVT,
  BTC_HD_DISCONNECT_EVT,
  BTC_HD_SEND_REPORT_EVT,
  BTC_HD_REPORT_ERROR_EVT,
  BTC_HD_UNPLUG_EVT,
} BTC_HD_EVT;

typedef enum {
  BTC_HD_DISABLED = 0,
  BTC_HD_ENABLED,
  BTC_HD_DISABLING
} BTC_HD_STATUS;
/* BTIF-HD control block */
typedef struct {
  BTC_HD_STATUS status;
  bool app_registered;
  bool service_dereg_active;
  bool forced_disc;
} btc_hd_cb_t;

#ifdef __cplusplus
extern "C" {
#endif

void btc_hd_call_handler(btc_msg_t *msg);
void btc_hd_cb_handler(btc_msg_t *msg);

//extern btc_hd_cb_t btc_hd_cb;
//extern void btc_hd_remove_device(bt_bdaddr_t bd_addr);
//extern void btc_hd_service_registration();

#ifdef __cplusplus
}
#endif

#endif