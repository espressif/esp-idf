// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __BTC_GAP_BLE_H__
#define __BTC_GAP_BLE_H__

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

#define BLE_ISVALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)) || ((x) == ESP_BLE_CONN_PARAM_UNDEF))

typedef enum {
    BTC_GAP_BLE_ACT_CFG_ADV_DATA = 0,
    BTC_GAP_BLE_ACT_SET_SCAN_PARAM,
    BTC_GAP_BLE_ACT_START_SCAN,
    BTC_GAP_BLE_ACT_STOP_SCAN,
    BTC_GAP_BLE_ACT_START_ADV,
    BTC_GAP_BLE_ACT_STOP_ADV,
    BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM,
    BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN,
    BTC_GAP_BLE_ACT_SET_RAND_ADDRESS,
    BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY,
    BTC_GAP_BLE_ACT_SET_DEV_NAME,
    BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW,
    BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW,
    BTC_GAP_BLE_SET_ENCRYPTION_EVT,
    BTC_GAP_BLE_SET_SECURITY_PARAM_EVT,
    BTC_GAP_BLE_SECURITY_RSP_EVT,
    BTC_GAP_BLE_PASSKEY_REPLY_EVT,
    BTC_GAP_BLE_CONFIRM_REPLY_EVT,
    BTC_GAP_BLE_DISCONNECT_EVT,
    BTC_GAP_BLE_REMOVE_BOND_DEV_EVT,
    BTC_GAP_BLE_CLEAR_BOND_DEV_EVT,
    BTC_GAP_BLE_GET_BOND_DEV_EVT,
} btc_gap_ble_act_t;

/* btc_ble_gap_args_t */
typedef union {
    //BTC_GAP_BLE_ACT_CFG_ADV_DATA = 0,
    struct config_adv_data_args {
        esp_ble_adv_data_t adv_data;
    } cfg_adv_data;
    //BTC_GAP_BLE_ACT_SET_SCAN_PARAM,
    struct set_scan_params_args {
        esp_ble_scan_params_t  scan_params;
    } set_scan_param;
    //BTC_GAP_BLE_ACT_START_SCAN,
    struct start_scan_args {
        uint32_t duration;
    } start_scan;
    //BTC_GAP_BLE_ACT_STOP_SCAN, no args
    //BTC_GAP_BLE_ACT_START_ADV,
    struct start_adv_args {
        esp_ble_adv_params_t adv_params;
    } start_adv;
    //BTC_GAP_BLE_ACT_STOP_ADV, no args
    //BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM,
    struct conn_update_params_args {
        esp_ble_conn_update_params_t conn_params;
    } conn_update_params;
    //BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN
    struct set_pkt_data_len_args {
        esp_bd_addr_t remote_device;
        uint16_t tx_data_length;
    } set_pkt_data_len;
    //BTC_GAP_BLE_ACT_SET_RAND_ADDRESS,
    struct set_rand_addr_args {
        esp_bd_addr_t rand_addr;
    } set_rand_addr;
    //BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY,
    struct cfg_local_privacy_args {
        bool privacy_enable;
    } cfg_local_privacy;    
    //BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW,
    struct config_adv_data_raw_args {
        uint8_t *raw_adv;
        uint32_t raw_adv_len;
    } cfg_adv_data_raw;
    //BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW,
    struct config_scan_rsp_data_raw_args {
        uint8_t *raw_scan_rsp;
        uint32_t raw_scan_rsp_len;
    } cfg_scan_rsp_data_raw;
    //BTC_GAP_BLE_SET_ENCRYPTION_EVT
    struct set_encryption_args {
        esp_bd_addr_t bd_addr;
        esp_ble_sec_act_t sec_act;
    } set_encryption;
    //BTC_GAP_BLE_SET_SECURITY_PARAM_EVT
    struct set_security_param_args {
        esp_ble_sm_param_t param_type;
        uint8_t len;
        uint8_t *value;
    } set_security_param;
    //BTC_GAP_BLE_SECURITY_RSP_EVT
    struct enc_rsp_args {
        esp_bd_addr_t bd_addr;
        bool accept;
    } sec_rsp;
    //BTC_GAP_BLE_PASSKEY_REPLY_EVT
    struct enc_passkey_reply_args {
        esp_bd_addr_t bd_addr;
        bool accept;
        uint32_t passkey;
    } enc_passkey_replay;
    //BTC_GAP_BLE_CONFIRM_REPLY_EVT
    struct enc_comfirm_reply_args {
        esp_bd_addr_t bd_addr;
        bool accept;
    } enc_comfirm_replay;
    //BTC_GAP_BLE_DISCONNECT_EVT
    struct disconnect_args {
        esp_bd_addr_t remote_device;
    } disconnect;
    //BTC_GAP_BLE_REMOVE_BOND_DEV_EVT
    struct remove_bond_device_args {
        esp_bd_addr_t bd_addr;
    } remove_bond_device;
} btc_ble_gap_args_t;

void btc_gap_ble_call_handler(btc_msg_t *msg);
void btc_gap_ble_cb_handler(btc_msg_t *msg);

void btc_gap_ble_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

#endif /* __BTC_GAP_BLE_H__ */
