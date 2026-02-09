/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
* SPDX-License-Identifier: Unlicense OR CC0-1.0
*/

#ifndef H_BLE_CHAN_INITIATOR_
#define H_BLE_CHAN_INITIATOR_

#include "modlog/modlog.h"
#include "esp_central.h"
#ifdef __cplusplus
extern "C" {
#endif

struct ble_hs_adv_fields;
struct ble_gap_conn_desc;
struct ble_hs_cfg;
union ble_store_value;
union ble_store_key;


#define BLE_HCI_LE_CS_SUBEVENT_DONE_STATUS_COMPLETE   0x0
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

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);

#ifdef __cplusplus
}
#endif

#endif
