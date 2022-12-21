/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>

#include "ble_unit.h"
#include "sync.h"
#define TAG "BLE_UINT"

#define GAP_TRANS_DEFAULT_SHORT_TO      50  // 50ms for events expected to reported as soon as API called

static uint8_t default_adv_data[] = {
    0x02, 0x01, 0x06,
    0x02, 0x0a, 0xeb, 0x03, 0x03, 0xab, 0xcd
};
static uint8_t default_scan_rsp_data[] = {
    0x0f, 0x09, 0x45, 0x53, 0x50, 0x5f, 0x47, 0x41, 0x54, 0x54, 0x53, 0x5f, 0x43,
    0x4f, 0x45, 0x58
};

esp_ble_scan_params_t default_scan_param = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x100,
    .scan_window            = 0x100
};

esp_ble_adv_params_t default_adv_param = {
    .adv_int_min        = 0x40,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

ble_util_scan_count_t scan_count;

static void ble_gap_event_default_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: {
        if (param->scan_param_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "set scan parameter failed, error status = %x", param->scan_param_cmpl.status);
        }
        break;
    }
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT: {
        if (param->adv_data_raw_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "set row data failed, error status = %x", param->adv_data_raw_cmpl.status);
        }
        break;
    }
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT: {
        if (param->scan_rsp_data_raw_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "set scan response data failed, error status = %x", param->scan_rsp_data_raw_cmpl.status);
        }
        break;
    }
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT: {
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising start failed\n");
        }
        break;
    }
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising stop failed\n");
        }
        break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT: {
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "scan start failed, error status = %x", param->scan_start_cmpl.status);
        }
        break;
    }
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "scan stop failed, error status = %x", param->scan_stop_cmpl.status);
        }
        break;
    default:
        break;
    }
}

static void ble_gap_util_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    if (sync_obj.cmd_recv != NULL) {
        sync_obj.cmd_recv(param->scan_rst.ble_adv, param->scan_rst.adv_data_len);
    }

    switch (event) {
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
        switch (param->scan_rst.search_evt) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            if (param->scan_rst.adv_data_len > 0 \
                    &&  (memcmp(default_adv_data, param->scan_rst.ble_adv, sizeof(default_adv_data)) == 0)) {
                scan_count.adv_count += 1;
                if (scan_count.adv_count % 10 == 0) {
                    ESP_LOGI(TAG, "adv count:%" PRIu32 " scan_res count %" PRIu32, scan_count.adv_count, scan_count.scan_res_count);
                }
            }

            if (param->scan_rst.scan_rsp_len > 0 \
                    && (memcmp(default_scan_rsp_data, (param->scan_rst.ble_adv + param->scan_rst.adv_data_len), sizeof(default_scan_rsp_data)) == 0)) {
                scan_count.scan_res_count += 1;
            }
            break;
        default:
            break;
        }
        break;
    default:
        ble_gap_event_default_handler(event, param);
        break;
    }
}


esp_err_t ble_gap_util_set_adv_data(uint8_t *adv_data, uint32_t adv_data_len, uint8_t *scan_rsp_data, uint32_t scan_rsp_data_len)
{
    esp_err_t ret;

    ret = esp_ble_gap_config_adv_data_raw(adv_data, adv_data_len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_config_adv_data_raw error, %d", ret);
        return ret;
    }
    ret = esp_ble_gap_config_scan_rsp_data_raw(scan_rsp_data, scan_rsp_data_len);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_config_scan_rsp_data_raw error, %d", ret);
        return ret;
    }
    return 0;
}

esp_err_t ble_gap_util_set_default_adv_data(void)
{
    return ble_gap_util_set_adv_data(default_adv_data, sizeof(default_adv_data), default_scan_rsp_data, sizeof(default_scan_rsp_data));
}

esp_err_t ble_gap_util_start_adv(esp_ble_adv_params_t *adv_param)
{
    esp_err_t ret;
    ret = esp_ble_gap_start_advertising(adv_param);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_start_advertising error, %d", ret);
        return ret;
    }
    return 0;
}

esp_err_t ble_gap_util_start_adv_default(void)
{
    esp_err_t ret;

    ret = esp_ble_gap_start_advertising(&default_adv_param);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_start_advertising error, %d", ret);
        return ret;
    }
    return 0;
}

esp_err_t ble_gap_util_stop_adv(void)
{
    esp_err_t ret;
    ret = esp_ble_gap_stop_advertising();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_stop_advertising error, %d", ret);
        return ret;
    }
    return 0;
}

esp_err_t ble_gap_util_set_scan_param(esp_ble_scan_params_t *scan_param)
{
    esp_err_t ret;
    ret = esp_ble_gap_set_scan_params(scan_param);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_set_scan_params error, %d", ret);
        return ret;
    }
    return 0;
}

esp_err_t ble_gap_util_set_default_scan_param(void)
{
    return ble_gap_util_set_scan_param(&default_scan_param);
}

esp_err_t ble_gap_util_start_scan(uint32_t duration)
{
    esp_err_t ret;
    ret = esp_ble_gap_start_scanning(duration);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_start_scanning error, %d", ret);
        return ret;
    }
    return 0;
}

esp_err_t ble_gap_util_stop_scan(void)
{
    esp_err_t ret;
    ret = esp_ble_gap_stop_scanning();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_ble_gap_stop_scanning error, %d", ret);
        return ret;
    }
    return 0;
}


void ble_gap_util_stop(void)
{
    ble_gap_util_stop_adv();
    ble_gap_util_stop_scan();
}

esp_err_t init_ble_gap_test_util(void)
{
    esp_err_t ret;
    ret = esp_ble_gap_register_callback(ble_gap_util_handler);
    if (ret) {
        ESP_LOGE(TAG, "gap register error, error code = %x", ret);
    }
    return ret;
}

void bt_test_init(void)
{
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());
}

void bt_test_deinit(void)
{
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
}
