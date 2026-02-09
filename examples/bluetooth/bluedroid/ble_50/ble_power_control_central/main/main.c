/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE Power Control feature on Central side.
* It scans for BLE devices and connects to a peripheral device. After connection,
* it reads transmit power levels, sets path loss reporting parameters, and
* enables power reporting to monitor and control power consumption.
*
****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "BLE_POWER_CONTROL_CENTRAL"
#define REMOTE_DEVICE_NAME "ESP_PWR_CTL_PRH"
#define PROFILE_NUM      1
#define PROFILE_A_APP_ID 0

static bool connect = false;
static uint16_t conn_handle = 0xFFFF;

static esp_ble_ext_scan_params_t ext_scan_params = {
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE,
    .cfg_mask = ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK,
    .uncoded_cfg = {BLE_SCAN_TYPE_ACTIVE, 0x40, 0x40},
    .coded_cfg = {BLE_SCAN_TYPE_ACTIVE, 0x40, 0x40},
};

// Connection parameters for different PHYs
static const esp_ble_conn_params_t phy_1m_conn_params = {
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .interval_min = 0x18,
    .interval_max = 0x28,
    .latency = 0,
    .supervision_timeout = 0x1F4,
    .min_ce_len = 0,
    .max_ce_len = 0,
};

static const esp_ble_conn_params_t phy_2m_conn_params = {
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .interval_min = 0x18,
    .interval_max = 0x28,
    .latency = 0,
    .supervision_timeout = 0x1F4,
    .min_ce_len = 0,
    .max_ce_len = 0,
};

static const esp_ble_conn_params_t phy_coded_conn_params = {
    .scan_interval = 0x40,
    .scan_window = 0x40,
    .interval_min = 0x18,
    .interval_max = 0x28,
    .latency = 0,
    .supervision_timeout = 0x1F4,
    .min_ce_len = 0,
    .max_ce_len = 0,
};

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    esp_bd_addr_t remote_bda;
};

static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = NULL,
        .gattc_if = ESP_GATT_IF_NONE,
    },
};

/**
 * @brief Initialize power control features after connection
 */
static void init_power_control(uint16_t conn_handle)
{
    if (conn_handle == 0xFFFF) {
        ESP_LOGE(TAG, "Invalid connection handle");
        return;
    }

    ESP_LOGI(TAG, "Initializing power control features...");

    // Read local transmit power level (LE 1M PHY)
    esp_err_t ret = esp_ble_gap_enhanced_read_transmit_power_level(conn_handle, ESP_BLE_CONN_TX_POWER_PHY_1M);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read local transmit power level, error = 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "Read local transmit power level request sent");
    }

    // Read remote transmit power level (LE 1M PHY)
    ret = esp_ble_gap_read_remote_transmit_power_level(conn_handle, ESP_BLE_CONN_TX_POWER_PHY_1M);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read remote transmit power level, error = 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "Read remote transmit power level request sent");
    }

    // Enable transmit power reporting (both local and remote)
    ret = esp_ble_gap_set_transmit_power_reporting_enable(conn_handle, true, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable transmit power reporting, error = 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "Transmit power reporting enabled");
    }

    // Set path loss reporting parameters
    esp_ble_path_loss_rpt_params_t path_loss_params = {
        .conn_handle = conn_handle,
        .high_threshold = 60,  // High threshold in dB
        .high_hysteresis = 10,  // High hysteresis in dB
        .low_threshold = 30,  // Low threshold in dB
        .low_hysteresis = 10,  // Low hysteresis in dB
        .min_time_spent = 2,  // Minimum time spent in dB
    };
    ret = esp_ble_gap_set_path_loss_reporting_params(&path_loss_params);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set path loss reporting parameters, error = 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "Path loss reporting parameters set");
    }

    // Enable path loss reporting
    ret = esp_ble_gap_set_path_loss_reporting_enable(conn_handle, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable path loss reporting, error = 0x%x", ret);
    } else {
        ESP_LOGI(TAG, "Path loss reporting enabled");
    }
}

/**
 * @brief GATT client event handler
 */
static void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(TAG, "GATTC event handler received NULL param");
        return;
    }

    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(TAG, "GATT client register, status %d, app_id %d, gattc_if %d",
                 p_data->reg.status, p_data->reg.app_id, gattc_if);
        gl_profile_tab[PROFILE_A_APP_ID].gattc_if = gattc_if;
        esp_err_t scan_ret = esp_ble_gap_set_ext_scan_params(&ext_scan_params);
        if (scan_ret) {
            ESP_LOGE(TAG, "set ext scan params error, error code = %x", scan_ret);
        }
        break;
    case ESP_GATTC_CONNECT_EVT:
        ESP_LOGI(TAG, "Connected, conn_id %d, remote "ESP_BD_ADDR_STR"",
                 p_data->connect.conn_id, ESP_BD_ADDR_HEX(p_data->connect.remote_bda));
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = p_data->connect.conn_id;
        memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_handle = p_data->connect.conn_id;
        // Initialize power control after connection
        init_power_control(conn_handle);
        break;
    case ESP_GATTC_DISCONNECT_EVT:
        connect = false;
        conn_handle = 0xFFFF;
        ESP_LOGI(TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(p_data->disconnect.remote_bda), p_data->disconnect.reason);
        break;
    default:
        break;
    }
}

/**
 * @brief GAP event handler
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(TAG, "GAP event handler received NULL param");
        return;
    }

    uint8_t *adv_name = NULL;
    uint8_t adv_name_len = 0;

    switch (event) {
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        ESP_LOGI(TAG, "Extended scan parameters set, starting scan...");
        esp_ble_gap_start_ext_scan(0, 0);  // Scan indefinitely
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        if (param->ext_scan_start.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Extended scan start failed, status %x", param->ext_scan_start.status);
            break;
        }
        ESP_LOGI(TAG, "Extended scan started successfully");
        break;
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT: {
        adv_name = esp_ble_resolve_adv_data_by_type(param->ext_adv_report.params.adv_data,
                                                    param->ext_adv_report.params.adv_data_len,
                                                    ESP_BLE_AD_TYPE_NAME_CMPL,
                                                    &adv_name_len);
        if (adv_name != NULL && adv_name_len > 0) {
            ESP_LOGI(TAG, "Found device: "ESP_BD_ADDR_STR", name: %.*s",
                     ESP_BD_ADDR_HEX(param->ext_adv_report.params.addr), adv_name_len, adv_name);

            if (adv_name_len == strlen(REMOTE_DEVICE_NAME) &&
                strncmp((char *)adv_name, REMOTE_DEVICE_NAME, adv_name_len) == 0) {
                ESP_LOGI(TAG, "Target device found, connecting...");
                if (!connect) {
                    connect = true;
                    esp_ble_gap_stop_ext_scan();
                    esp_ble_gatt_creat_conn_params_t creat_conn_params = {0};
                    memcpy(&creat_conn_params.remote_bda, param->ext_adv_report.params.addr, ESP_BD_ADDR_LEN);
                    creat_conn_params.remote_addr_type = param->ext_adv_report.params.addr_type;
                    creat_conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
                    creat_conn_params.is_direct = true;
                    creat_conn_params.is_aux = true;
                    creat_conn_params.phy_mask = ESP_BLE_PHY_1M_PREF_MASK | ESP_BLE_PHY_2M_PREF_MASK | ESP_BLE_PHY_CODED_PREF_MASK;
                    creat_conn_params.phy_1m_conn_params = &phy_1m_conn_params;
                    creat_conn_params.phy_2m_conn_params = &phy_2m_conn_params;
                    creat_conn_params.phy_coded_conn_params = &phy_coded_conn_params;
                    esp_ble_gattc_enh_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, &creat_conn_params);
                }
            }
        }
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        ESP_LOGI(TAG, "Extended scan stopped");
        break;
    case ESP_GAP_BLE_ENH_READ_TRANS_PWR_LEVEL_EVT:
        ESP_LOGI(TAG, "Enhanced read transmit power level: conn_handle=%d, status=%d, phy=%d, cur_tx_pwr=%d, max_tx_pwr=%d",
                 param->enh_trans_pwr_level_cmpl.conn_handle,
                 param->enh_trans_pwr_level_cmpl.status,
                 param->enh_trans_pwr_level_cmpl.phy,
                 param->enh_trans_pwr_level_cmpl.cur_tx_pwr_level,
                 param->enh_trans_pwr_level_cmpl.max_tx_pwr_level);
        break;
    case ESP_GAP_BLE_READ_REMOTE_TRANS_PWR_LEVEL_EVT:
        ESP_LOGI(TAG, "Read remote transmit power level complete, status %d", param->read_remote_trans_pwr_level_cmpl.status);
        break;
    case ESP_GAP_BLE_SET_PATH_LOSS_RPTING_PARAMS_EVT:
        ESP_LOGI(TAG, "Set path loss reporting parameters complete, status %d, conn_handle=%d",
                 param->set_path_loss_rpting_params.status,
                 param->set_path_loss_rpting_params.conn_handle);
        break;
    case ESP_GAP_BLE_SET_PATH_LOSS_RPTING_ENABLE_EVT:
        ESP_LOGI(TAG, "Set path loss reporting enable complete, status %d, conn_handle=%d",
                 param->set_path_loss_rpting_enable.status,
                 param->set_path_loss_rpting_enable.conn_handle);
        break;
    case ESP_GAP_BLE_SET_TRANS_PWR_RPTING_ENABLE_EVT:
        ESP_LOGI(TAG, "Set transmit power reporting enable complete, status %d, conn_handle=%d",
                 param->set_trans_pwr_rpting_enable.status,
                 param->set_trans_pwr_rpting_enable.conn_handle);
        break;
    case ESP_GAP_BLE_PATH_LOSS_THRESHOLD_EVT:
        ESP_LOGI(TAG, "Path loss threshold event: conn_handle=%d, cur_path_loss=%d, zone_entered=%d",
                 param->path_loss_thres_evt.conn_handle,
                 param->path_loss_thres_evt.cur_path_loss,
                 param->path_loss_thres_evt.zone_entered);
        break;
    case ESP_GAP_BLE_TRANS_PWR_RPTING_EVT:
        ESP_LOGI(TAG, "Transmit power reporting event: conn_handle=%d, status=%d, reason=%d, phy=%d, power_level=%d, power_level_flag=%d, delta=%d",
                 param->trans_power_report_evt.conn_handle,
                 param->trans_power_report_evt.status,
                 param->trans_power_report_evt.reason,
                 param->trans_power_report_evt.phy,
                 param->trans_power_report_evt.tx_power_level,
                 param->trans_power_report_evt.tx_power_level_flag,
                 param->trans_power_report_evt.delta);
        break;
    default:
        break;
    }
}

void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Bluetooth Controller
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller init failed, error = %x", ret);
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller enable failed, error = %x", ret);
        return;
    }

    // Initialize Bluedroid
    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid init failed, error = %x", ret);
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid enable failed, error = %x", ret);
        return;
    }

    // Register callbacks
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "GAP register callback failed, error = %x", ret);
        return;
    }

    ret = esp_ble_gattc_register_callback(gattc_profile_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "GATTC register callback failed, error = %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (ret) {
        ESP_LOGE(TAG, "GATTC app register failed, error = %x", ret);
        return;
    }

    ESP_LOGI(TAG, "BLE Power Control Central example started");
}
