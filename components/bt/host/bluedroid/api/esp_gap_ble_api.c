/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "bta/bta_api.h"
#include "common/bt_trace.h"
#include "btc/btc_manage.h"
#include "btc_gap_ble.h"
#include "btc/btc_ble_storage.h"


esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_GAP_BLE, callback) == 0 ? ESP_OK : ESP_FAIL);
}

esp_gap_ble_cb_t esp_ble_gap_get_callback(void)
{
    return (esp_gap_ble_cb_t) btc_profile_cb_get(BTC_PID_GAP_BLE);
}

#if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gap_config_adv_data(esp_ble_adv_data_t *adv_data)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

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

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free)== BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (scan_params == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_SCAN_PARAM;
    memcpy(&arg.set_scan_param.scan_params, scan_params, sizeof(esp_ble_scan_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_start_scanning(uint32_t duration)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_START_SCAN;
    arg.start_scan.duration = duration;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_stop_scanning(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_STOP_SCAN;
    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_start_advertising(esp_ble_adv_params_t *adv_params)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_START_ADV;
    memcpy(&arg.start_adv.adv_params, adv_params, sizeof(esp_ble_adv_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_stop_advertising(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_STOP_ADV;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_clear_advertising(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CLEAR_ADV;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);
    if(!params) {
        LOG_ERROR("%s,params is NULL", __func__);
        return ESP_FAIL;
    }

    if (ESP_BLE_IS_VALID_PARAM(params->min_int, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
        ESP_BLE_IS_VALID_PARAM(params->max_int, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
        ESP_BLE_IS_VALID_PARAM(params->timeout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
        (params->latency <= ESP_BLE_CONN_LATENCY_MAX) &&
        ((params->timeout * 10) >= ((1 + params->latency) * ((params->max_int * 5) >> 1))) && params->min_int <= params->max_int) {

        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BLE;
        msg.act = BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM;
        memcpy(&arg.conn_update_params.conn_params, params, sizeof(esp_ble_conn_update_params_t));

        return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
    } else {
        LOG_ERROR("%s,invalid connection params:min_int = %d, max_int = %d, latency = %d, timeout = %d",\
                            __func__, params->min_int, params->max_int, params->latency, params->timeout);
        return ESP_FAIL;
    }
}

esp_err_t esp_ble_gap_set_pkt_data_len(esp_bd_addr_t remote_device, uint16_t tx_data_length)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN;
    arg.set_pkt_data_len.tx_data_length = tx_data_length;
    memcpy(arg.set_pkt_data_len.remote_device, remote_device, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}


esp_err_t esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_RAND_ADDRESS;
    memcpy(arg.set_rand_addr.rand_addr, rand_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_clear_rand_addr(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CLEAR_RAND_ADDRESS;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_config_local_privacy (bool privacy_enable)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY;
    arg.cfg_local_privacy.privacy_enable = privacy_enable;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_config_local_icon (uint16_t icon)
{
    esp_err_t ret;
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    switch (icon) {
    case ESP_BLE_APPEARANCE_GENERIC_PHONE:
    case ESP_BLE_APPEARANCE_GENERIC_COMPUTER:
    case ESP_BLE_APPEARANCE_GENERIC_REMOTE:
    case ESP_BLE_APPEARANCE_GENERIC_THERMOMETER:
    case ESP_BLE_APPEARANCE_THERMOMETER_EAR:
    case ESP_BLE_APPEARANCE_GENERIC_HEART_RATE:
    case ESP_BLE_APPEARANCE_HEART_RATE_BELT:
    case ESP_BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE:
    case ESP_BLE_APPEARANCE_BLOOD_PRESSURE_ARM:
    case ESP_BLE_APPEARANCE_BLOOD_PRESSURE_WRIST:
    case ESP_BLE_APPEARANCE_GENERIC_PULSE_OXIMETER:
    case ESP_BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP:
    case ESP_BLE_APPEARANCE_PULSE_OXIMETER_WRIST:
    case ESP_BLE_APPEARANCE_GENERIC_GLUCOSE:
    case ESP_BLE_APPEARANCE_GENERIC_WEIGHT:
    case ESP_BLE_APPEARANCE_GENERIC_WALKING:
    case ESP_BLE_APPEARANCE_WALKING_IN_SHOE:
    case ESP_BLE_APPEARANCE_WALKING_ON_SHOE:
    case ESP_BLE_APPEARANCE_WALKING_ON_HIP:
    case ESP_BLE_APPEARANCE_GENERIC_WATCH:
    case ESP_BLE_APPEARANCE_SPORTS_WATCH:
    case ESP_BLE_APPEARANCE_GENERIC_EYEGLASSES:
    case ESP_BLE_APPEARANCE_GENERIC_DISPLAY:
    case ESP_BLE_APPEARANCE_GENERIC_MEDIA_PLAYER:
    case ESP_BLE_APPEARANCE_GENERIC_BARCODE_SCANNER:
    case ESP_BLE_APPEARANCE_HID_BARCODE_SCANNER:
    case ESP_BLE_APPEARANCE_GENERIC_HID:
    case ESP_BLE_APPEARANCE_HID_KEYBOARD:
    case ESP_BLE_APPEARANCE_HID_MOUSE:
    case ESP_BLE_APPEARANCE_HID_JOYSTICK:
    case ESP_BLE_APPEARANCE_HID_GAMEPAD:
    case ESP_BLE_APPEARANCE_HID_DIGITIZER_TABLET:
    case ESP_BLE_APPEARANCE_HID_CARD_READER:
    case ESP_BLE_APPEARANCE_HID_DIGITAL_PEN:
    case ESP_BLE_APPEARANCE_UNKNOWN:
    case ESP_BLE_APPEARANCE_GENERIC_CLOCK:
    case ESP_BLE_APPEARANCE_GENERIC_TAG:
    case ESP_BLE_APPEARANCE_GENERIC_KEYRING:
    case ESP_BLE_APPEARANCE_GENERIC_CYCLING:
    case ESP_BLE_APPEARANCE_CYCLING_COMPUTER:
    case ESP_BLE_APPEARANCE_CYCLING_SPEED:
    case ESP_BLE_APPEARANCE_CYCLING_CADENCE:
    case ESP_BLE_APPEARANCE_CYCLING_POWER:
    case ESP_BLE_APPEARANCE_CYCLING_SPEED_CADENCE:
    case ESP_BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE:
    case ESP_BLE_APPEARANCE_POWERED_WHEELCHAIR:
    case ESP_BLE_APPEARANCE_MOBILITY_SCOOTER:
    case ESP_BLE_APPEARANCE_GENERIC_CONTINUOUS_GLUCOSE_MONITOR:
    case ESP_BLE_APPEARANCE_GENERIC_INSULIN_PUMP:
    case ESP_BLE_APPEARANCE_INSULIN_PUMP_DURABLE_PUMP:
    case ESP_BLE_APPEARANCE_INSULIN_PUMP_PATCH_PUMP:
    case ESP_BLE_APPEARANCE_INSULIN_PEN:
    case ESP_BLE_APPEARANCE_GENERIC_MEDICATION_DELIVERY:
    case ESP_BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS:
    case ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION:
    case ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV:
    case ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD:
    case ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV:
        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BLE;
        msg.act = BTC_GAP_BLE_ACT_CONFIG_LOCAL_ICON;
        arg.cfg_local_icon.icon = icon;
        ret = (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
        break;
    default:
        ret = ESP_ERR_INVALID_ARG;
        break;
    }
    return ret;
}

esp_err_t esp_ble_gap_update_whitelist(bool add_remove, esp_bd_addr_t remote_bda, esp_ble_wl_addr_type_t wl_addr_type)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!remote_bda){
        return ESP_ERR_INVALID_SIZE;
    }
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_UPDATE_WHITE_LIST;
    arg.update_white_list.add_remove = add_remove;
    arg.update_white_list.wl_addr_type = wl_addr_type;
    memcpy(arg.update_white_list.remote_bda, remote_bda, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
                == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_clear_whitelist(void)
{
    btc_msg_t msg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CLEAR_WHITE_LIST;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_get_whitelist_size(uint16_t *length)
{
    if (length == NULL) {
        return ESP_FAIL;
    }
    btc_get_whitelist_size(length);

    return ESP_OK;
}
#if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gap_set_prefer_conn_params(esp_bd_addr_t bd_addr,
                                                                 uint16_t min_conn_int, uint16_t max_conn_int,
                                                                 uint16_t slave_latency, uint16_t supervision_tout)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (ESP_BLE_IS_VALID_PARAM(min_conn_int, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
        ESP_BLE_IS_VALID_PARAM(max_conn_int, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
        ESP_BLE_IS_VALID_PARAM(supervision_tout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
        (slave_latency <= ESP_BLE_CONN_LATENCY_MAX) &&
        ((supervision_tout * 10) >= ((1 + slave_latency) * ((max_conn_int * 5) >> 1))) && min_conn_int <= max_conn_int) {

        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BLE;
        msg.act = BTC_GAP_BLE_ACT_SET_CONN_PARAMS;
        arg.set_conn_params.min_conn_int = min_conn_int;
        arg.set_conn_params.max_conn_int = max_conn_int;
        arg.set_conn_params.slave_latency = slave_latency;
        arg.set_conn_params.supervision_tout = supervision_tout;
        memcpy(arg.set_conn_params.bd_addr, bd_addr, sizeof(esp_bd_addr_t));

        return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
                    == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
    } else {
        LOG_ERROR("%s,invalid connection params:min_int = %d, max_int = %d, latency = %d, timeout = %d",\
                            __func__, min_conn_int, max_conn_int, slave_latency, supervision_tout);
        return ESP_FAIL;
    }
}
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

esp_err_t esp_ble_gap_set_device_name(const char *name)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    return esp_bt_dev_set_device_name(name);
}

esp_err_t esp_ble_gap_get_device_name(void)
{
    btc_msg_t msg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_GET_DEV_NAME;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_get_local_used_addr(esp_bd_addr_t local_used_addr, uint8_t * addr_type)
{
    if(esp_bluedroid_get_status() != (ESP_BLUEDROID_STATUS_ENABLED)) {
        LOG_ERROR("%s, bluedroid status error", __func__);
        return ESP_FAIL;
    }
    if(!BTM_BleGetCurrentAddress(local_used_addr, addr_type)) {
        return ESP_FAIL;
    }
    return ESP_OK;
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
#if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gap_config_adv_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((raw_data_len != 0 && raw_data == NULL) || raw_data_len > ESP_BLE_ADV_DATA_LEN_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW;
    arg.cfg_adv_data_raw.raw_adv = raw_data;
    arg.cfg_adv_data_raw.raw_adv_len = raw_data_len;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gap_read_rssi(esp_bd_addr_t remote_addr)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_READ_RSSI;
    memcpy(arg.read_rssi.remote_addr, remote_addr, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_gap_config_scan_rsp_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((raw_data_len != 0 && raw_data == NULL) || raw_data_len > ESP_BLE_ADV_DATA_LEN_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW;
    arg.cfg_scan_rsp_data_raw.raw_scan_rsp = raw_data;
    arg.cfg_scan_rsp_data_raw.raw_scan_rsp_len = raw_data_len;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_add_duplicate_scan_exceptional_device(esp_ble_duplicate_exceptional_info_type_t type, esp_duplicate_info_t device_info)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!device_info && type <= ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_LINK_ID) {
        return ESP_ERR_INVALID_SIZE;
    }
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST;
    arg.update_duplicate_exceptional_list.subcode = ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_ADD;
    arg.update_duplicate_exceptional_list.info_type = type;
    if (device_info) {
        memcpy(arg.update_duplicate_exceptional_list.device_info, device_info, sizeof(esp_bd_addr_t));
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
                == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_remove_duplicate_scan_exceptional_device(esp_ble_duplicate_exceptional_info_type_t type, esp_duplicate_info_t device_info)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!device_info && type <= ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_LINK_ID) {
        return ESP_ERR_INVALID_SIZE;
    }
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST;
    arg.update_duplicate_exceptional_list.subcode = ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_REMOVE;
    arg.update_duplicate_exceptional_list.info_type = type;
    if (device_info) {
        memcpy(arg.update_duplicate_exceptional_list.device_info, device_info, sizeof(esp_bd_addr_t));
    }

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
                == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_clean_duplicate_scan_exceptional_list(esp_duplicate_scan_exceptional_list_type_t list_type)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST;
    arg.update_duplicate_exceptional_list.subcode = ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_CLEAN;
    arg.update_duplicate_exceptional_list.info_type = list_type;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
                == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
#if (SMP_INCLUDED == TRUE)
esp_err_t esp_ble_gap_set_security_param(esp_ble_sm_param_t param_type,
        void *value, uint8_t len)
{
    if(param_type >= ESP_BLE_SM_MAX_PARAM) {
        return ESP_ERR_INVALID_ARG;
    }
    if((param_type != ESP_BLE_SM_CLEAR_STATIC_PASSKEY) && ( value == NULL || len < sizeof(uint8_t) || len > sizeof(uint32_t))) {
        return ESP_ERR_INVALID_ARG;
    }
    if(param_type == ESP_BLE_SM_SET_STATIC_PASSKEY) {
        uint32_t passkey = 0;
        for(uint8_t i = 0; i < len; i++)
        {
            passkey += (((uint8_t *)value)[i]<<(8*i));
        }
        if(passkey > 999999) {
            return ESP_ERR_INVALID_ARG;
        }
    }
    if (param_type == ESP_BLE_APP_ENC_KEY_SIZE) {
        LOG_ERROR("ESP_BLE_APP_ENC_KEY_SIZE is deprecated, use ESP_GATT_PERM_ENCRYPT_KEY_SIZE in characteristic definition");
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (param_type == ESP_BLE_SM_MAX_KEY_SIZE || param_type == ESP_BLE_SM_MIN_KEY_SIZE) {
        if (((uint8_t *)value)[0] > 16 || ((uint8_t *)value)[0] < 7) {
            return ESP_ERR_INVALID_ARG;
        }
    }
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_SECURITY_PARAM_EVT;
    arg.set_security_param.param_type = param_type;
    arg.set_security_param.len = len;
    arg.set_security_param.value = value;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_set_encryption(esp_bd_addr_t bd_addr, esp_ble_sec_act_t sec_act)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_ENCRYPTION_EVT;
    arg.set_encryption.sec_act = sec_act;
    memcpy(arg.set_encryption.bd_addr, bd_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_security_rsp(esp_bd_addr_t bd_addr, bool accept)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SECURITY_RSP_EVT;
    arg.sec_rsp.accept = accept;
    memcpy(arg.sec_rsp.bd_addr, bd_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_passkey_reply(esp_bd_addr_t bd_addr, bool accept, uint32_t passkey)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PASSKEY_REPLY_EVT;
    arg.enc_passkey_replay.accept = accept;
    arg.enc_passkey_replay.passkey = passkey;
    memcpy(arg.enc_passkey_replay.bd_addr, bd_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_confirm_reply(esp_bd_addr_t bd_addr, bool accept)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_CONFIRM_REPLY_EVT;
    arg.enc_comfirm_replay.accept = accept;
    memcpy(arg.enc_comfirm_replay.bd_addr, bd_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_remove_bond_device(esp_bd_addr_t bd_addr)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_REMOVE_BOND_DEV_EVT;
    memcpy(arg.remove_bond_device.bd_addr, bd_addr, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

int esp_ble_get_bond_device_num(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_FAIL;
    }

    return btc_storage_get_num_ble_bond_devices();
}

esp_err_t esp_ble_get_bond_device_list(int *dev_num, esp_ble_bond_dev_t *dev_list)
{
    int ret;
    int dev_num_total;

    if (dev_num == NULL || dev_list == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    dev_num_total = btc_storage_get_num_ble_bond_devices();
    if (*dev_num > dev_num_total) {
        *dev_num = dev_num_total;
    }

    ret = btc_storage_get_bonded_ble_devices_list(dev_list, *dev_num);

    return (ret == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_oob_req_reply(esp_bd_addr_t bd_addr, uint8_t *TK, uint8_t len)
{
    if(len != ESP_BT_OCTET16_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_OOB_REQ_REPLY_EVT;
    memcpy(arg.oob_req_reply.bd_addr, bd_addr, ESP_BD_ADDR_LEN);
    arg.oob_req_reply.len = len;
    arg.oob_req_reply.p_value = TK;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_sc_oob_req_reply(esp_bd_addr_t bd_addr, uint8_t p_c[16], uint8_t p_r[16])
{
    if (!p_c || !p_r) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SC_OOB_REQ_REPLY_EVT;
    memcpy(arg.sc_oob_req_reply.bd_addr, bd_addr, ESP_BD_ADDR_LEN);
    arg.sc_oob_req_reply.p_c = p_c;
    arg.sc_oob_req_reply.p_r = p_r;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_create_sc_oob_data(void)
{
    btc_msg_t msg = {0};

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SC_CR_OOB_DATA_EVT;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif /* #if (SMP_INCLUDED == TRUE) */

esp_err_t esp_ble_gap_disconnect(esp_bd_addr_t remote_device)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_DISCONNECT_EVT;
    memcpy(arg.disconnect.remote_device, remote_device, ESP_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_get_current_conn_params(esp_bd_addr_t bd_addr, esp_gap_conn_params_t *conn_params)
{
    if(!bd_addr || !conn_params) {
        return ESP_ERR_INVALID_ARG;
    }
    if(BTM_GetCurrentConnParams(bd_addr, &conn_params->interval, &conn_params->latency, &conn_params->timeout)) {
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t esp_gap_ble_set_channels(esp_gap_ble_channels channels)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_AFH_CHANNELS;

    memcpy(&arg.set_channels.channels, channels, ESP_GAP_BLE_CHANNELS_LEN);
    arg.set_channels.channels[ESP_GAP_BLE_CHANNELS_LEN -1] &= 0x1F;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_gap_ble_set_authorization(esp_bd_addr_t bd_addr, bool authorize)
{
    if (!bd_addr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (BTM_Ble_Authorization(bd_addr, authorize)) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

#if (BLE_42_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_dtm_tx_start(const esp_ble_dtm_tx_t *tx_params)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (!tx_params) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_DTM_TX_START;

    memcpy(&arg.dtm_tx_start, tx_params, sizeof(esp_ble_dtm_tx_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_dtm_rx_start(const esp_ble_dtm_rx_t *rx_params)
{
    btc_msg_t msg = {0};
    btc_ble_gap_args_t arg;

    if (!rx_params) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_DTM_RX_START;

    memcpy(&arg.dtm_rx_start, rx_params, sizeof(esp_ble_dtm_rx_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
esp_err_t esp_ble_dtm_enh_tx_start(const esp_ble_dtm_enh_tx_t *tx_params)
{
    btc_msg_t msg = {0};
    btc_ble_5_gap_args_t arg;

    if (!tx_params) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_DTM_ENH_TX_START;

    memcpy(&arg.dtm_enh_tx_start, tx_params, sizeof(esp_ble_dtm_enh_tx_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_dtm_enh_rx_start(const esp_ble_dtm_enh_rx_t *rx_params)
{
    btc_msg_t msg = {0};
    btc_ble_5_gap_args_t arg;

    if (!rx_params) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_DTM_ENH_RX_START;

    memcpy(&arg.dtm_enh_rx_start, rx_params, sizeof(esp_ble_dtm_enh_rx_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

esp_err_t esp_ble_dtm_stop(void)
{
    btc_msg_t msg = {0};

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_DTM_STOP;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#if (BLE_50_FEATURE_SUPPORT == TRUE)

esp_err_t esp_ble_gap_read_phy(esp_bd_addr_t bd_addr)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_READ_PHY;

    memcpy(arg.read_phy.bd_addr, bd_addr, ESP_BD_ADDR_LEN);
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_set_preferred_default_phy(esp_ble_gap_phy_mask_t tx_phy_mask,
                                               esp_ble_gap_phy_mask_t rx_phy_mask)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_PREFERED_DEF_PHY;
    arg.set_perf_def_phy.tx_phy_mask = tx_phy_mask;
    arg.set_perf_def_phy.rx_phy_mask = rx_phy_mask;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_set_preferred_phy(esp_bd_addr_t bd_addr,
                                       esp_ble_gap_all_phys_t all_phys_mask,
                                       esp_ble_gap_phy_mask_t tx_phy_mask,
                                       esp_ble_gap_phy_mask_t rx_phy_mask,
                                       esp_ble_gap_prefer_phy_options_t phy_options)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_DEF_PHY;
    memcpy(arg.set_def_phy.bd_addr, bd_addr, ESP_BD_ADDR_LEN);
    arg.set_def_phy.all_phys_mask = all_phys_mask;
    arg.set_def_phy.tx_phy_mask = tx_phy_mask;
    arg.set_def_phy.rx_phy_mask = rx_phy_mask;
    arg.set_def_phy.phy_options = phy_options;
    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_ext_adv_set_rand_addr(uint8_t instance, esp_bd_addr_t rand_addr)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_EXT_ADV_RAND_ADDR;
    arg.ext_adv_set_rand_addr.instance = instance;
    memcpy(arg.ext_adv_set_rand_addr.rand_addr, rand_addr, BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
             == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_ext_adv_set_params(uint8_t instance,
                                                           const esp_ble_gap_ext_adv_params_t *params)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_EXT_ADV_PARAMS;

    if (ESP_BLE_IS_VALID_PARAM(params->interval_min, ESP_BLE_PRIM_ADV_INT_MIN, ESP_BLE_PRIM_ADV_INT_MAX) &&
        ESP_BLE_IS_VALID_PARAM(params->interval_max, ESP_BLE_PRIM_ADV_INT_MIN, ESP_BLE_PRIM_ADV_INT_MAX) &&
        ESP_BLE_IS_VALID_PARAM(params->peer_addr_type, BLE_ADDR_TYPE_PUBLIC, BLE_ADDR_TYPE_RANDOM) &&
        ESP_BLE_IS_VALID_PARAM(params->own_addr_type, BLE_ADDR_TYPE_PUBLIC, BLE_ADDR_TYPE_RPA_RANDOM) &&
        ESP_BLE_IS_VALID_PARAM(params->filter_policy, ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY, ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST) &&
        (params->interval_min <= params->interval_max) && (params->channel_map > 0) && (params->channel_map <= ADV_CHNL_ALL) &&
        ((params->primary_phy == ESP_BLE_GAP_PRI_PHY_1M) || (params->primary_phy == ESP_BLE_GAP_PRI_PHY_CODED)) &&
        ((params->secondary_phy == ESP_BLE_GAP_PHY_1M) || (params->secondary_phy == ESP_BLE_GAP_PHY_2M) ||
        (params->secondary_phy == ESP_BLE_GAP_PHY_CODED))){
        memcpy(&arg.ext_adv_set_params.params, params, sizeof(esp_ble_gap_ext_adv_params_t));
    } else {
        LOG_ERROR("%s,invalid ext adv params", __func__);
        return ESP_ERR_INVALID_ARG;
    }

    arg.ext_adv_set_params.instance = instance;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_config_ext_adv_data_raw(uint8_t instance, uint16_t length,
                                                                   const uint8_t *data)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_CFG_EXT_ADV_DATA_RAW;

    arg.ext_adv_cfg_data.instance = instance;
    arg.ext_adv_cfg_data.length = length;
    arg.ext_adv_cfg_data.data = (uint8_t *)data;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_config_ext_scan_rsp_data_raw(uint8_t instance, uint16_t length,
                                                                   const uint8_t *scan_rsp_data)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_CFG_EXT_SCAN_RSP_DATA_RAW;

    arg.cfg_scan_rsp.instance = instance;
    arg.cfg_scan_rsp.length = length;
    arg.cfg_scan_rsp.data = (uint8_t *)scan_rsp_data;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_ext_adv_start(uint8_t num_adv, const esp_ble_gap_ext_adv_t *ext_adv)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_EXT_ADV_START;

    arg.ext_adv_start.num_adv = num_adv;
    arg.ext_adv_start.ext_adv = (esp_ble_gap_ext_adv_t *)ext_adv;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_ext_adv_stop(uint8_t num_adv, const uint8_t *ext_adv_inst)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_EXT_ADV_STOP;
    arg.ext_adv_stop.num_adv = num_adv;
    arg.ext_adv_stop.ext_adv_inst = (uint8_t *)ext_adv_inst;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_ext_adv_set_remove(uint8_t instance)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_EXT_ADV_SET_REMOVE;
    arg.ext_adv_set_remove.instance = instance;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
             == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_ext_adv_set_clear(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_EXT_ADV_SET_CLEAR;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_periodic_adv_set_params(uint8_t instance, const esp_ble_gap_periodic_adv_params_t *params)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_PERIODIC_ADV_PARAMS;

    arg.peridic_adv_set_params.instance = instance;
    memcpy(&arg.peridic_adv_set_params.params, params, sizeof(esp_ble_gap_periodic_adv_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
esp_err_t esp_ble_gap_config_periodic_adv_data_raw(uint8_t instance, uint16_t length,
                                                                           const uint8_t *data, bool only_update_did)
#else
esp_err_t esp_ble_gap_config_periodic_adv_data_raw(uint8_t instance, uint16_t length,
                                                                           const uint8_t *data)
#endif
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_CFG_PERIODIC_ADV_DATA_RAW;

    arg.periodic_adv_cfg_data.instance = instance;
    arg.periodic_adv_cfg_data.len = length;
    arg.periodic_adv_cfg_data.data = (uint8_t *)data;
#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
    arg.periodic_adv_cfg_data.only_update_did = only_update_did;
#else
    arg.periodic_adv_cfg_data.only_update_did = false;
#endif

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), btc_gap_ble_arg_deep_copy,
                btc_gap_ble_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
esp_err_t esp_ble_gap_periodic_adv_start(uint8_t instance,bool include_adi)
#else
esp_err_t esp_ble_gap_periodic_adv_start(uint8_t instance)
#endif
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_START;

    #if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
    arg.periodic_adv_start.include_adi = include_adi;
    #else
    arg.periodic_adv_start.include_adi = false;
    #endif
    arg.periodic_adv_start.instance = instance;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_periodic_adv_stop(uint8_t instance)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_STOP;

    arg.periodic_adv_stop.instance = instance;

   return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_periodic_adv_create_sync(const esp_ble_gap_periodic_adv_sync_params_t *params)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_CREATE_SYNC;

    memcpy(&arg.periodic_adv_create_sync.params, params, sizeof(esp_ble_gap_periodic_adv_sync_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_periodic_adv_sync_cancel(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_periodic_adv_sync_terminate(uint16_t sync_handle)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE;

    arg.periodic_adv_sync_term.sync_handle = sync_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_periodic_adv_add_dev_to_list(esp_ble_addr_type_t addr_type,
                                                                         esp_bd_addr_t addr,
                                                                         uint8_t sid)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_ADD_DEV_TO_LIST;

    arg.periodic_adv_add_dev.addr_type = addr_type;
    arg.periodic_adv_add_dev.sid = sid;


    memcpy(arg.periodic_adv_add_dev.addr, addr, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_periodic_adv_remove_dev_from_list(esp_ble_addr_type_t addr_type,
                                                                         esp_bd_addr_t addr,
                                                                         uint8_t sid)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_REMOVE_ADD_DEV_FROM_LIST;

    arg.periodic_adv_remove_dev.addr_type = addr_type;
    arg.periodic_adv_remove_dev.sid = sid;


    memcpy(arg.periodic_adv_remove_dev.addr, addr, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_periodic_adv_clear_dev(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_CLEAR_DEV;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

esp_err_t esp_ble_gap_set_ext_scan_params(const esp_ble_ext_scan_params_t *params)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    if (!params) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_EXT_SCAN_PARAMS;

    memcpy(&arg.set_ext_scan_params.params, params, sizeof(esp_ble_ext_scan_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_start_ext_scan(uint32_t duration, uint16_t period)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_START_EXT_SCAN;

    arg.start_ext_scan.duration = duration;
    arg.start_ext_scan.period = period;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_stop_ext_scan(void)
{
    btc_msg_t msg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_STOP_EXT_SCAN;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_prefer_ext_connect_params_set(esp_bd_addr_t addr,
                                                    esp_ble_gap_phy_mask_t phy_mask,
                                                    const esp_ble_gap_conn_params_t *phy_1m_conn_params,
                                                    const esp_ble_gap_conn_params_t *phy_2m_conn_params,
                                                    const esp_ble_gap_conn_params_t *phy_coded_conn_params)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_EXT_PEFER_CONNET_PARAMS;

    arg.set_ext_conn_params.phy_mask = phy_mask;
    if (phy_mask & ESP_BLE_GAP_PHY_1M_PREF_MASK) {
        if (!phy_1m_conn_params) {
            return BT_STATUS_PARM_INVALID;
        }

        if (ESP_BLE_IS_VALID_PARAM(phy_1m_conn_params->interval_min, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(phy_1m_conn_params->interval_max, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(phy_1m_conn_params->supervision_timeout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
            (phy_1m_conn_params->latency <= ESP_BLE_CONN_LATENCY_MAX) &&
            ((phy_1m_conn_params->supervision_timeout * 10) >= ((1 + phy_1m_conn_params->latency) * ((phy_1m_conn_params->interval_max * 5) >> 1))) &&
            (phy_1m_conn_params->interval_min <= phy_1m_conn_params->interval_max)) {

            memcpy(&arg.set_ext_conn_params.phy_1m_conn_params, phy_1m_conn_params, sizeof(esp_ble_gap_conn_params_t));
        } else {
            LOG_ERROR("%s,invalid connection params:min_int = %d, max_int = %d, latency = %d, timeout = %d", __func__,
                                    phy_1m_conn_params->interval_min,
                                    phy_1m_conn_params->interval_max,
                                    phy_1m_conn_params->latency,
                                    phy_1m_conn_params->supervision_timeout);

            return ESP_ERR_INVALID_ARG;
        }
    }

    if (phy_mask & ESP_BLE_GAP_PHY_2M_PREF_MASK) {
        if (!phy_2m_conn_params) {
            return BT_STATUS_PARM_INVALID;
        }

        if (ESP_BLE_IS_VALID_PARAM(phy_2m_conn_params->interval_min, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(phy_2m_conn_params->interval_max, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(phy_2m_conn_params->supervision_timeout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
            (phy_2m_conn_params->latency <= ESP_BLE_CONN_LATENCY_MAX) &&
            ((phy_2m_conn_params->supervision_timeout * 10) >= ((1 + phy_2m_conn_params->latency) * ((phy_2m_conn_params->interval_max * 5) >> 1))) &&
            (phy_2m_conn_params->interval_min <= phy_2m_conn_params->interval_max)) {

            memcpy(&arg.set_ext_conn_params.phy_2m_conn_params, phy_2m_conn_params, sizeof(esp_ble_gap_conn_params_t));
        } else {
            LOG_ERROR("%s,invalid connection params:min_int = %d, max_int = %d, latency = %d, timeout = %d", __func__,
                                    phy_2m_conn_params->interval_min,
                                    phy_2m_conn_params->interval_max,
                                    phy_2m_conn_params->latency,
                                    phy_2m_conn_params->supervision_timeout);

            return ESP_ERR_INVALID_ARG;
        }
    }

    if (phy_mask & ESP_BLE_GAP_PHY_CODED_PREF_MASK) {
        if (!phy_coded_conn_params) {
            return BT_STATUS_PARM_INVALID;
        }

        if (ESP_BLE_IS_VALID_PARAM(phy_coded_conn_params->interval_min, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(phy_coded_conn_params->interval_max, ESP_BLE_CONN_INT_MIN, ESP_BLE_CONN_INT_MAX) &&
            ESP_BLE_IS_VALID_PARAM(phy_coded_conn_params->supervision_timeout, ESP_BLE_CONN_SUP_TOUT_MIN, ESP_BLE_CONN_SUP_TOUT_MAX) &&
            (phy_coded_conn_params->latency <= ESP_BLE_CONN_LATENCY_MAX) &&
            ((phy_coded_conn_params->supervision_timeout * 10) >= ((1 + phy_coded_conn_params->latency) * ((phy_coded_conn_params->interval_max * 5) >> 1))) &&
            (phy_coded_conn_params->interval_min <= phy_coded_conn_params->interval_max)) {

            memcpy(&arg.set_ext_conn_params.phy_coded_conn_params, phy_coded_conn_params, sizeof(esp_ble_gap_conn_params_t));
        } else {
            LOG_ERROR("%s,invalid connection params:min_int = %d, max_int = %d, latency = %d, timeout = %d", __func__,
                                    phy_coded_conn_params->interval_min,
                                    phy_coded_conn_params->interval_max,
                                    phy_coded_conn_params->latency,
                                    phy_coded_conn_params->supervision_timeout);

            return ESP_ERR_INVALID_ARG;
        }
    }

    memcpy(arg.set_ext_conn_params.addr, addr, sizeof(esp_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);

}

#endif //#if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
esp_err_t esp_ble_gap_periodic_adv_recv_enable(uint16_t sync_handle, uint8_t enable)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_RECV_ENABLE;

    arg.periodic_adv_recv_en.sync_handle = sync_handle;
    arg.periodic_adv_recv_en.enable = enable;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_periodic_adv_sync_trans(esp_bd_addr_t addr, uint16_t service_data, uint16_t sync_handle)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_SYNC_TRANS;

    memcpy(arg.periodic_adv_sync_trans.addr, addr, sizeof(esp_bd_addr_t));
    arg.periodic_adv_sync_trans.service_data = service_data;
    arg.periodic_adv_sync_trans.sync_handle = sync_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_periodic_adv_set_info_trans(esp_bd_addr_t addr, uint16_t service_data, uint8_t adv_handle)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS;

    memcpy(arg.periodic_adv_set_info_trans.addr, addr, sizeof(esp_bd_addr_t));
    arg.periodic_adv_set_info_trans.service_data = service_data;
    arg.periodic_adv_set_info_trans.adv_handle = adv_handle;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_ble_gap_set_periodic_adv_sync_trans_params(esp_bd_addr_t addr, const esp_ble_gap_past_params_t *params)
{
    btc_msg_t msg;
    btc_ble_5_gap_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (params == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_PERIODIC_ADV_SYNC_TRANS_PARAMS;

    if (addr) {
        memcpy(arg.set_periodic_adv_sync_trans_params.addr, addr, sizeof(esp_bd_addr_t));
    } else {
        memset(arg.set_periodic_adv_sync_trans_params.addr, 0, sizeof(esp_bd_addr_t));
    }
    memcpy(&arg.set_periodic_adv_sync_trans_params.params, params, sizeof(esp_ble_gap_past_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_5_gap_args_t), NULL, NULL)
            == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif //#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
