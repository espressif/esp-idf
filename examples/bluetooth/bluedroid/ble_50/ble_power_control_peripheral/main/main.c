/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
*
* This demo showcases BLE Power Control feature on Peripheral side.
* It advertises and waits for connection from a central device. After connection,
* it reads remote transmit power level and enables power reporting to monitor
* power consumption.
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
#include "power_control_service.h"

#define TAG "BLE_POWER_CONTROL_PERIPHERAL"
#define DEVICE_NAME "ESP_PWR_CTL_PRH"

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

static SemaphoreHandle_t test_sem = NULL;
static uint16_t conn_handle = 0xFFFF;

uint16_t power_control_handle_table[POWER_CONTROL_IDX_NB];

/* UUIDs */
static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid  = ESP_GATT_UUID_CHAR_DECLARE;

/* Service UUID - must be a variable, not a macro, to take address */
static const uint16_t power_control_service_uuid  = BLE_UUID_POWER_CONTROL_SERVICE_VAL;
static const uint16_t power_level_char_uuid       = BLE_UUID_POWER_CONTROL_SERVICE_VAL;

/* Characteristic properties */
static const uint8_t char_prop_read                = ESP_GATT_CHAR_PROP_BIT_READ;

/* Power Level value */
static uint8_t power_level_value[1] = {0x00};

/* Full Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t gatt_db[POWER_CONTROL_IDX_NB] = {
    // Service Declaration
    [IDX_POWER_CONTROL_SVC] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      sizeof(uint16_t), sizeof(power_control_service_uuid), (uint8_t *)&power_control_service_uuid}},

    /* Power Level Characteristic Declaration */
    [IDX_POWER_LEVEL_CHAR] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      sizeof(uint8_t), sizeof(uint8_t), (uint8_t *)&char_prop_read}},

    /* Power Level Characteristic Value */
    [IDX_POWER_LEVEL_VAL] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&power_level_char_uuid, ESP_GATT_PERM_READ,
      sizeof(power_level_value), sizeof(power_level_value), (uint8_t *)power_level_value}},
};

// Extended advertising data (includes device name)
// Format: [length][AD type][data...]
// Flag: 0x02 (length) + 0x01 (AD type) + 0x06 (flag value) = 3 bytes
// Name: 0x10 (length) + 0x09 (AD type) + 15 bytes (name) = 17 bytes
static uint8_t raw_ext_adv_data[] = {
    0x02, ESP_BLE_AD_TYPE_FLAG, 0x06,
    0x10, ESP_BLE_AD_TYPE_NAME_CMPL, 'E','S','P','_','P','W','R','_','C','T','L','_','P','R','H'
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
 * @brief Initialize power control features after connection
 */
static void init_power_control(uint16_t conn_handle)
{
    if (conn_handle == 0xFFFF) {
        ESP_LOGE(TAG, "Invalid connection handle");
        return;
    }

    ESP_LOGI(TAG, "Initializing power control features...");

    // Read remote transmit power level (LE 1M PHY)
    esp_err_t ret = esp_ble_gap_read_remote_transmit_power_level(conn_handle, ESP_BLE_CONN_TX_POWER_PHY_1M);
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

    switch (event) {
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising params set, status %d, instance %d",
                 param->ext_adv_set_params.status, param->ext_adv_set_params.instance);
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
        ESP_LOGI(TAG, "Extended advertising data set, status %d, instance %d",
                 param->ext_adv_data_set.status, param->ext_adv_data_set.instance);
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        xSemaphoreGive(test_sem);
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
    case ESP_GAP_BLE_READ_REMOTE_TRANS_PWR_LEVEL_EVT:
        ESP_LOGI(TAG, "Read remote transmit power level complete, status %d", param->read_remote_trans_pwr_level_cmpl.status);
        break;
    case ESP_GAP_BLE_SET_TRANS_PWR_RPTING_ENABLE_EVT:
        ESP_LOGI(TAG, "Set transmit power reporting enable complete, status %d, conn_handle=%d",
                 param->set_trans_pwr_rpting_enable.status,
                 param->set_trans_pwr_rpting_enable.conn_handle);
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

        // Create attribute table
        esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, POWER_CONTROL_IDX_NB, SVC_INST_ID);
        if (create_attr_ret) {
            ESP_LOGE(TAG, "create attr table failed, error code = %x", create_attr_ret);
        }
        break;
    }
    case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
        if (param->add_attr_tab.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        } else if (param->add_attr_tab.num_handle != POWER_CONTROL_IDX_NB) {
            ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) doesn't equal to POWER_CONTROL_IDX_NB(%d)",
                     param->add_attr_tab.num_handle, POWER_CONTROL_IDX_NB);
        } else {
            ESP_LOGI(TAG, "create attribute table successfully, the number handle = %d", param->add_attr_tab.num_handle);
            // Validate handles pointer before memcpy
            if (param->add_attr_tab.handles) {
                memcpy(power_control_handle_table, param->add_attr_tab.handles, sizeof(power_control_handle_table));
                esp_ble_gatts_start_service(power_control_handle_table[IDX_POWER_CONTROL_SVC]);
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
        // Initialize power control after connection
        init_power_control(conn_handle);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason);
        conn_handle = 0xFFFF;
        // Restart extended advertising
        FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_start(NUM_EXT_ADV, ext_adv), test_sem);
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

    // Create semaphore for extended advertising
    test_sem = xSemaphoreCreateBinary();

    // Start extended advertising
    esp_bd_addr_t addr;
    esp_ble_gap_addr_create_static(addr);
    ESP_LOGI(TAG, "Device Address: "ESP_BD_ADDR_STR"", ESP_BD_ADDR_HEX(addr));

    // Set extended advertising parameters
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_set_params(EXT_ADV_HANDLE, &ext_adv_params), test_sem);

    // Set extended advertising data
    FUNC_SEND_WAIT_SEM(esp_ble_gap_config_ext_adv_data_raw(EXT_ADV_HANDLE,
                                                           sizeof(raw_ext_adv_data),
                                                           raw_ext_adv_data), test_sem);

    // Start extended advertising
    FUNC_SEND_WAIT_SEM(esp_ble_gap_ext_adv_start(NUM_EXT_ADV, ext_adv), test_sem);

    ESP_LOGI(TAG, "BLE Power Control Peripheral example started");
}
