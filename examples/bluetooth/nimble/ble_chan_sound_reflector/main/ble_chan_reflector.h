/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_BLE_CHAN_REFLECTOR_
#define H_BLE_CHAN_REFLECTOR_

#include <stdbool.h>
#include "nimble/ble.h"
#include "modlog/modlog.h"
#include "esp_peripheral.h"
#ifdef __cplusplus
extern "C" {
#endif


#define BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE  0x0
#define BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_PARTIAL   0x1
#define BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_ABORTED   0xF

#define LOCAL_PROCEDURE_MEM  1024  // Replace with an appropriate constant value
struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

/** GATT server. */

#define BLE_UUID_RANGING_SERVICE_VAL (0x185B)

/** @brief UUID of the RAS Features Characteristic. **/
#define BLE_UUID_RAS_FEATURES_VAL (0x2C14)

/** @brief UUID of the Real-time Ranging Data Characteristic. **/
#define BLE_UUID_RAS_REALTIME_RD_VAL (0x2C15)

/** @brief UUID of the On-demand Ranging Data Characteristic. **/
#define BLE_UUID_RAS_ONDEMAND_RD_VAL (0x2C16)

/** @brief UUID of the RAS Control Point Characteristic. **/
#define BLE_UUID_RAS_CP_VAL (0x2C17)

/** @brief UUID of the Ranging Data Ready Characteristic. **/
#define BLE_UUID_RAS_RD_READY_VAL (0x2C18)

/** @brief UUID of the Ranging Data Overwritten Characteristic. **/
#define BLE_UUID_RAS_RD_OVERWRITTEN_VAL (0x2C19)

void custom_gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int custom_gatt_svr_init(void);

#ifdef __cplusplus
}
#endif

#endif
