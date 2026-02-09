/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE Connection Subrating feature on Peripheral side.
* It advertises and waits for connection from a central device. After connection,
* it accepts subrate requests from the central to reduce power consumption.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "subrating_service.h"

#define TAG "BLE_SUBRATING_PERIPHERAL"
#define DEVICE_NAME "ESP_SUBRATE_PRH"

#define PROFILE_NUM      1
#define PROFILE_A_APP_ID 0
#define SVC_INST_ID      0

#define EXT_ADV_HANDLE      0
#define NUM_EXT_ADV         1

#define SEM_WAIT_TIMEOUT_MS     5000

#define FUNC_SEND_WAIT_SEM(func, sem) do {\
        esp_err_t __err_rc = (func);\
        if (__err_rc != ESP_OK) { \
            ESP_LOGE(TAG, "%s, message send fail, error = %d", __func__, __err_rc); \
        } \
        if (xSemaphoreTake(sem, pdMS_TO_TICKS(SEM_WAIT_TIMEOUT_MS)) != pdTRUE) { \
            ESP_LOGE(TAG, "%s, semaphore timeout", __func__); \
        } \
} while(0);

static uint16_t conn_handle = 0xFFFF;

uint16_t subrating_handle_table[SUBRATING_IDX_NB];

/* UUIDs */
static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid  = ESP_GATT_UUID_CHAR_DECLARE;

/* Service UUID - must be a variable, not a macro, to take address */
static const uint16_t subrating_service_uuid      = BLE_UUID_SUBRATING_SERVICE_VAL;
static const uint16_t subrating_info_char_uuid   = BLE_UUID_SUBRATING_SERVICE_VAL;

/* Characteristic properties */
static const uint8_t char_prop_read                = ESP_GATT_CHAR_PROP_BIT_READ;

/* Subrating Info value */
static uint8_t subrating_info_value[4] = {0x00, 0x00, 0x00, 0x00};

/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[SUBRATING_IDX_NB] = {
    // Service Declaration
    [IDX_SUBRATING_SVC] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(subrating_service_uuid), (uint8_t *)&subrating_service_uuid}},

    /* Subrating Info Characteristic Declaration */
    [IDX_SUBRATING_INFO_CHAR] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read}},

    /* Subrating Info Characteristic Value */
    [IDX_SUBRATING_INFO_VAL] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&subrating_info_char_uuid, ESP_GATT_PERM_READ,
      sizeof(subrating_info_value), sizeof(subrating_info_value), (uint8_t *)subrating_info_value}},
};

// Extended advertising data (includes device name)
static uint8_t raw_ext_adv_data[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x10, ESP_BLE_AD_TYPE_NAME_CMPL, 'E','S','P','_','S','U','B','R','A','T','E','_','P','R','H'
};

static esp_ble_gap_ext_adv_params_t ext_adv_params = {
    .type = ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE,
    .interval_min = 0x20,  // 32 * 0.625ms = 20ms
    .interval_max = 0x40,  // 64 * 0.625ms = 40ms
    .channel_map = ADV_CHNL_ALL,
    .filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    .primary_phy = ESP_BLE_GAP_PHY_1M,
    .max_skip = 0,
    .secondary_phy = ESP_BLE_GAP_PHY_1M,
    .sid = 0,
    .scan_req_notif = false,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .tx_power = EXT_ADV_TX_PWR_NO_PREFERENCE,
};

static esp_ble_gap_ext_adv_t ext_adv[1] = {
    [0] = {EXT_ADV_HANDLE, 0, 0},
};

/**
 * @brief GAP event handler
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(TAG, "GAP event handler received NULL param");
        return;
    }

    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        ESP_LOGI(TAG, "Extended advertising params set, status %d, instance %d",
                 param->ext_adv_set_params.status, param->ext_adv_set_params.instance);
        if (param->ext_adv_set_params.status == ESP_BT_STATUS_SUCCESS) {
            // Set extended advertising data
            esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE, sizeof(raw_ext_adv_data), raw_ext_adv_data);
        } else {
            ESP_LOGE(TAG, "Failed to set extended advertising params, status %d", param->ext_adv_set_params.status);
        }
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "Extended advertising data set, status %d, instance %d",
                 param->ext_adv_data_set.status, param->ext_adv_data_set.instance);
        if (param->ext_adv_data_set.status == ESP_BT_STATUS_SUCCESS) {
            // Start extended advertising
            esp_ble_gap_ext_adv_start(NUM_EXT_ADV, ext_adv);
        } else {
            ESP_LOGE(TAG, "Failed to set extended advertising data, status %d", param->ext_adv_data_set.status);
        }
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        if (param->ext_adv_start.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Extended advertising start failed, status %d", param->ext_adv_start.status);
            break;
        }
        ESP_LOGI(TAG, "Extended advertising start successfully, instance num %d",
                 param->ext_adv_start.instance_num);
        break;
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
        if (param->ext_adv_stop.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Extended advertising stop failed, status %d", param->ext_adv_stop.status);
            break;
        }
        ESP_LOGI(TAG, "Extended advertising stop successfully");
        break;
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(TAG, "Connection parameters updated: min_int=%d, max_int=%d, conn_int=%d, latency=%d, timeout=%d",
                 param->update_conn_params.min_int,
                 param->update_conn_params.max_int,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;
    case ESP_GAP_BLE_SUBRATE_CHANGE_EVT:
         ESP_LOGI(TAG, "Connection subrating update, status %d, conn_hdl %d, subrate_factor %d latency %d, continuation_number %d, timeout %d",
                  param->subrate_change_evt.status,
                  param->subrate_change_evt.conn_handle,
                  param->subrate_change_evt.subrate_factor,
                  param->subrate_change_evt.peripheral_latency,
                  param->subrate_change_evt.continuation_number,
                  param->subrate_change_evt.supervision_timeout);
        break;
    default:
        break;
    }
}

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
};

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static struct gatts_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    },
};

/**
 * @brief GATTS profile event handler
 */
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (param == NULL) {
        ESP_LOGE(TAG, "GATTS event handler received NULL param");
        return;
    }

    switch (event) {
    case ESP_GATTS_REG_EVT: {
        ESP_LOGI(TAG, "GATT server register, status %d, app_id %d, gatts_if %d",
                 param->reg.status, param->reg.app_id, gatts_if);

        esp_ble_gap_set_device_name(DEVICE_NAME);

        // Set extended advertising parameters
        esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params);

        // Create attribute table
        esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, SUBRATING_IDX_NB, SVC_INST_ID);
        if (create_attr_ret) {
            ESP_LOGE(TAG, "create attr table failed, error code = %x", create_attr_ret);
        }
        break;
    }
    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != SUBRATING_IDX_NB) {
            ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) doesn't equal to SUBRATING_IDX_NB(%d)",
                     param->add_attr_tab.num_handle, SUBRATING_IDX_NB);
        } else {
            ESP_LOGI(TAG, "create attribute table successfully, the number handle = %d", param->add_attr_tab.num_handle);
            // Validate handles pointer before memcpy
            if (param->add_attr_tab.handles) {
                memcpy(subrating_handle_table, param->add_attr_tab.handles, sizeof(subrating_handle_table));
                esp_ble_gatts_start_service(subrating_handle_table[IDX_SUBRATING_SVC]);
            } else {
                ESP_LOGE(TAG, "Invalid handles pointer");
            }
        }
        break;
    }
    case ESP_GATTS_START_EVT:
        ESP_LOGI(TAG, "Service start, status %d, service_handle %d",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_CONNECT_EVT: {
        ESP_LOGI(TAG, "Connected, conn_id %u, remote "ESP_BD_ADDR_STR"",
                 param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        gl_profile_tab[PROFILE_A_APP_ID].conn_id = param->connect.conn_id;
        conn_handle = param->connect.conn_id;

        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x20;
        conn_params.min_int = 0x10;
        conn_params.timeout = 400;
        esp_ble_gap_update_conn_params(&conn_params);

        // Stop extended advertising
        uint8_t adv_handle = EXT_ADV_HANDLE;
        esp_ble_gap_ext_adv_stop(1, &adv_handle);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason);
        conn_handle = 0xFFFF;
        // Restart extended advertising
        esp_ble_gap_ext_adv_start(NUM_EXT_ADV, ext_adv);
        break;
    case ESP_GATTS_READ_EVT:
        ESP_LOGI(TAG, "Read event, handle %d", param->read.handle);
        // Auto response is enabled, so no need to send response manually
        break;
    case ESP_GATTS_WRITE_EVT:
        ESP_LOGI(TAG, "Write event, handle %d, len %d", param->write.handle, param->write.len);
        // Auto response is enabled, so no need to send response manually
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(TAG, "MTU exchange, MTU %d", param->mtu.mtu);
        break;
    default:
        break;
    }
}

static void esp_gatts_cb(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        } else {
            ESP_LOGI(TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id, param->reg.status);
            return;
        }
    }

    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE ||
                    gatts_if == gl_profile_tab[idx].gatts_if) {
                if (gl_profile_tab[idx].gatts_cb) {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
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

    ret = esp_ble_gatts_register_callback(esp_gatts_cb);
    if (ret) {
        ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
        return;
    }

    ret = esp_ble_gatts_app_register(PROFILE_A_APP_ID);
    if (ret) {
        ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
        return;
    }

    ESP_LOGI(TAG, "BLE Connection Subrating Peripheral example started");
}
