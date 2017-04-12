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

#include <string.h>

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "bta_api.h"
#include "bt_trace.h"
#include "btc_manage.h"
#include "btc_gap_ble.h"


esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback)
{
    if (esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
        return ESP_ERR_INVALID_STATE;
    }
    return (btc_profile_cb_set(BTC_PID_GAP_BLE, callback) == 0 ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_config_adv_data(esp_ble_adv_data_t *adv_data)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;
    
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (adv_data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (adv_data->service_uuid_len & 0xf) { //not 16*n
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_ADV_DATA;
    memcpy(&arg.cfg_adv_data.adv_data, adv_data, sizeof(esp_ble_adv_data_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}


esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;
    
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (scan_params == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_SCAN_PARAM;
    memcpy(&arg.set_scan_param.scan_params, scan_params, sizeof(esp_ble_scan_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_start_scanning(uint32_t duration)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_START_SCAN;
    arg.start_scan.duration = duration;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_stop_scanning(void)
{
    btc_msg_t msg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_STOP_SCAN;
    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_start_advertising(esp_ble_adv_params_t *adv_params)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {       
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_START_ADV;
    memcpy(&arg.start_adv.adv_params, adv_params, sizeof(esp_ble_adv_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_stop_advertising(void)
{
    btc_msg_t msg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_STOP_ADV;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM;
    memcpy(&arg.conn_update_params.conn_params, params, sizeof(esp_ble_conn_update_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_set_pkt_data_len(esp_bd_addr_t remote_device, uint16_t tx_data_length)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {    
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN;
    arg.set_pkt_data_len.tx_data_length = tx_data_length;
    memcpy(arg.set_pkt_data_len.remote_device, remote_device, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_RAND_ADDRESS;
    memcpy(arg.set_rand_addr.rand_addr, rand_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_config_local_privacy (bool privacy_enable)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY;
    arg.cfg_local_privacy.privacy_enable = privacy_enable;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_set_device_name(const char *name)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (strlen(name) > ESP_GAP_DEVICE_NAME_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_DEV_NAME;
    strcpy(arg.set_dev_name.device_name, name);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

uint8_t *esp_ble_resolve_adv_data( uint8_t *adv_data, uint8_t type, uint8_t *length)
{
    if (((type < ESP_BLE_AD_TYPE_FLAG) || (type > ESP_BLE_AD_TYPE_128SERVICE_DATA)) &&
            (type != ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE)) {
        LOG_ERROR("the eir type not define, type = %x\n", type);
        return NULL;
    }

    if (adv_data == NULL) {
        LOG_ERROR("Invalid p_eir data.\n");
        return NULL;
    }

    return (BTM_CheckAdvData( adv_data, type, length));
}

esp_err_t esp_ble_gap_config_adv_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (raw_data == NULL
            || (raw_data_len <= 0 || raw_data_len > ESP_BLE_ADV_DATA_LEN_MAX)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW;
    arg.cfg_adv_data_raw.raw_adv = raw_data;
    arg.cfg_adv_data_raw.raw_adv_len = raw_data_len;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_config_scan_rsp_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (raw_data == NULL
            || (raw_data_len <= 0 || raw_data_len > ESP_BLE_SCAN_RSP_DATA_LEN_MAX)) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW;
    arg.cfg_scan_rsp_data_raw.raw_scan_rsp = raw_data;
    arg.cfg_scan_rsp_data_raw.raw_scan_rsp_len = raw_data_len;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}
