/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This is the private interface file for the BTA device manager.
 *
 ******************************************************************************/
#ifndef BTA_DM_GAP_H
#define BTA_DM_GAP_H

#include <stdint.h>
#include "common/bt_target.h"

#if BLE_INCLUDED == TRUE
typedef uint8_t tBTM_BLE_LEGACY_GAP_EVENT;

typedef struct {
    uint8_t status;
    uint16_t rx_len;
    uint16_t tx_len;
} tBTM_BLE_DATA_LEN_UP_EVT;

typedef struct {
    uint8_t   status;
    BD_ADDR remote_bd_addr;
    uint16_t  min_conn_int;
    uint16_t  max_conn_int;
    uint16_t  conn_int;
    uint16_t  slave_latency;
    uint16_t  supervision_tout;
} tBTM_BLE_CONN_PRAMS_UPDATE_EVT;

typedef struct {
    uint8_t   status;
    uint8_t   operation;
} tBTM_BLE_WHITE_LIST_UPDATE_EVT;

typedef struct {
    tBTM_STATUS status;
    uint8_t       hci_status;
    uint8_t       channel_map[5];
    BD_ADDR     rem_bda;
} tBTM_BLE_CH_MAP_EVT;

typedef struct {
    uint8_t status;
    uint8_t subcode;
    uint32_t length;
    uint8_t *device_info;
} tBTM_BLE_EXCEPTION_LIST_UPDATE_EVT;

typedef struct {
    tBTM_STATUS status;
    uint8_t       hci_status;
} tBTM_BLE_SET_CHANNELS_EVT;

typedef union {
    uint8_t status;
    tBTM_BLE_DATA_LEN_UP_EVT data_length_params;
    tBTM_BLE_CONN_PRAMS_UPDATE_EVT conn_params_update;
    tBTM_BLE_WHITE_LIST_UPDATE_EVT white_list_update;
    tBTM_BLE_CH_MAP_EVT            ch_map_results;
    tBTM_BLE_EXCEPTION_LIST_UPDATE_EVT exception_list_up;
    tBTM_BLE_SET_CHANNELS_EVT      set_channels;
} tBTM_BLE_LEGACY_GAP_CB_PARAMS;

#define BTM_BLE_LEGACY_GAP_ADV_START_COMPLETE_EVT             0x01
#define BTM_BLE_LEGACY_GAP_ADV_STOP_COMPLETE_EVT              0x02
#define BTM_BLE_LEGACY_GAP_ADV_DATA_SET_COMPLETE_EVT          0x03
#define BTM_BLE_LEGACY_GAP_ADV_RAW_SET_COMPLETE_EVT           0x04
#define BTM_BLE_LEGACY_GAP_SCAN_RSP_SET_COMPLETE_EVT          0x05
#define BTM_BLE_LEGACY_GAP_RSP_RAW_SET_COMPLETE_EVT           0x06
#define BTM_BLE_LEGACY_GAP_SCAN_PARAMS_SET_COMPLETE_EVT       0x07
#define BTM_BLE_LEGACY_GAP_SCAN_START_COMPLETE_EVT            0x08
#define BTM_BLE_LEGACY_GAP_SCAN_STOP_COMPLETE_EVT             0x09
#define BTM_BLE_LEGACY_GAP_DATA_LEN_SET_COMPLETE_EVT          0x0A
#define BTM_BLE_LEGACY_GAP_CONNECTION_PARAMS_UPDATE_EVT       0x0B
#define BTM_BLE_LEGACY_GAP_SET_RANDOM_ADDR_EVT                0x0C
#define BTM_BLE_LEGACY_GAP_RPA_TIMEOUT_EVT                    0x0D
#define BTM_BLE_LEGACY_GAP_ADD_DEV_TO_RPA_LIST_EVT            0x0E
#define BTM_BLE_LEGACY_GAP_SET_PRIVACY_EVT                    0x0F
#define BTM_BLE_LEGACY_GAP_WHITE_LIST_UPDATE_EVT              0x10
#define BTM_BLE_LEGACY_GAP_READ_CHANNEL_MAP_EVT               0x11
#define BTM_BLE_LEGACY_GAP_EXCEPTION_LIST_UPDATE_EVT          0x12
#define BTM_BLE_LEGACY_GAP_CLEAR_ADV_COMPLETE_EVT             0x13
#define BTM_BLE_LEGACY_GAP_SET_CSA_SUPPORT_COMPLETE_EVT       0x14
#define BTM_BLE_LEGACY_GAP_SET_VENDOR_EVT_MASK_COMPLETE_EVT   0x15
#define BTM_BLE_LEGACY_GAP_SET_CHANNELS_COMPLETE_EVT          0x16
#define BTM_BLE_LEGACY_GAP_SET_PRIVACY_MODE_COMPLETE_EVT      0x17
// BTM_BLE_LEGACY_GAP_MAX_EVT should not be modified
#define BTM_BLE_LEGACY_GAP_MAX_EVT                            0x40

typedef void (*tBTM_BLE_LEGACY_GAP_CBACK)(tBTM_BLE_LEGACY_GAP_EVENT event, tBTM_BLE_LEGACY_GAP_CB_PARAMS *params);
void BTM_BleLegacyGapRegisterCallback(tBTM_BLE_LEGACY_GAP_CBACK cb);
void BTM_LegacyBleCallbackTrigger(tBTM_BLE_LEGACY_GAP_EVENT event, tBTM_BLE_LEGACY_GAP_CB_PARAMS *params);

/* Arm legacy GAP one-shot: next HCI complete for this event may invoke the registered callback once. */
void BTM_BleLegacyGapOneshotArm(tBTM_BLE_LEGACY_GAP_EVENT event);
/* If armed, disarm and call BTM_LegacyBleCallbackTrigger; otherwise no-op. */
void BTM_BleLegacyGapOneshotFireIfArmed(tBTM_BLE_LEGACY_GAP_EVENT event, tBTM_BLE_LEGACY_GAP_CB_PARAMS *params);
#endif // #if BLE_INCLUDED == TRUE

#endif /* BTA_DM_GAP_H */
