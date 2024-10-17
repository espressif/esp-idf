#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_device.h"

#define TAG "BLE_GAP"

// Structure to hold BLE device information
typedef struct {
    esp_bd_addr_t bda;
    bool device_found;
    uint8_t adv_data_len;
    uint8_t adv_data[ESP_BLE_ADV_DATA_LEN_MAX + 1];
    int rssi;
} ble_device_info_t;

static ble_device_info_t ble_device_info = {0};

// Callback for GAP BLE events
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "Scan parameters set, starting scan...");
        esp_ble_gap_start_scanning(30);  // 30 seconds of scanning
        break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "BLE scan started successfully.");
        } else {
            ESP_LOGE(TAG, "Failed to start BLE scan, error code: %d", param->scan_start_cmpl.status);
        }
        break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
        if (scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
            char bda_str[18];
            esp_log_buffer_hex(TAG, scan_result->scan_rst.bda, ESP_BD_ADDR_LEN);
            snprintf(bda_str, sizeof(bda_str), "%02x:%02x:%02x:%02x:%02x:%02x",
                     scan_result->scan_rst.bda[0], scan_result->scan_rst.bda[1], scan_result->scan_rst.bda[2],
                     scan_result->scan_rst.bda[3], scan_result->scan_rst.bda[4], scan_result->scan_rst.bda[5]);

            ESP_LOGI(TAG, "Device found: %s RSSI: %d", bda_str, scan_result->scan_rst.rssi);

            // Save device info if not already found
            if (!ble_device_info.device_found) {
                memcpy(ble_device_info.bda, scan_result->scan_rst.bda, ESP_BD_ADDR_LEN);
                ble_device_info.device_found = true;
                ble_device_info.rssi = scan_result->scan_rst.rssi;
                ble_device_info.adv_data_len = scan_result->scan_rst.adv_data_len;
                memcpy(ble_device_info.adv_data, scan_result->scan_rst.ble_adv, scan_result->scan_rst.adv_data_len);
            }
        } else if (scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
            ESP_LOGI(TAG, "BLE scan complete.");
            if (ble_device_info.device_found) {
                ESP_LOGI(TAG, "Device discovered: RSSI %d", ble_device_info.rssi);
                esp_ble_gap_stop_scanning();
            }
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Scan stopped successfully.");
        } else {
            ESP_LOGE(TAG, "Failed to stop scan, error code: %d", param->scan_stop_cmpl.status);
        }
        break;

    default:
        ESP_LOGI(TAG, "Unhandled GAP event: %d", event);
        break;
    }
}

void app_main() {
    // Initialize NVS flash
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize the BT controller with default BLE configuration
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    // Initialize Bluedroid stack
    ESP_ERROR_CHECK(esp_bluedroid_init());
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    // Register the GAP callback handler
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));

    // Set the scan parameters
    esp_ble_scan_params_t scan_params = {
        .scan_type              = BLE_SCAN_TYPE_PASSIVE,
        .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval          = 0x50,
        .scan_window            = 0x30
    };
    ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&scan_params));
}
