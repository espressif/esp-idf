/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/**
 * @brief BLE Encrypted Advertising Data Central Example - No Connection Version
 *
 * This simplified example demonstrates decrypting advertising data WITHOUT connecting.
 * The key material is pre-configured (same as peripheral).
 *
 * Use case: When the key is pre-shared or provisioned out-of-band.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "ble_ead.h"

#define TAG "ENC_ADV_CENT_SIMPLE"

/* Custom service UUID to identify target device */
#define CUSTOM_SERVICE_UUID     0x2C01

/*
 * Pre-shared Key Material - MUST match the Peripheral!
 * In real applications, this would be provisioned securely.
 */
static const ble_ead_key_material_t pre_shared_key = {
    .session_key = {
        0x19, 0x6a, 0x0a, 0xd1, 0x2a, 0x61, 0x20, 0x1e,
        0x13, 0x6e, 0x2e, 0xd1, 0x12, 0xda, 0xa9, 0x57
    },
    .iv = {0x9E, 0x7a, 0x00, 0xef, 0xb1, 0x7a, 0xe7, 0x46},
};

/* Scan parameters */
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type          = BLE_SCAN_TYPE_PASSIVE,  /* Passive scan is enough */
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval      = 0x50,
    .scan_window        = 0x30,
    .scan_duplicate     = BLE_SCAN_DUPLICATE_DISABLE,
};

/**
 * @brief Check if device advertises our target service UUID
 */
static bool is_target_device(const uint8_t *adv_data, uint8_t adv_len)
{
    uint8_t offset = 0;
    while (offset < adv_len) {
        uint8_t len = adv_data[offset];
        if (len == 0 || offset + len >= adv_len) {
            break;
        }

        uint8_t type = adv_data[offset + 1];
        if (type == ESP_BLE_AD_TYPE_16SRV_CMPL || type == ESP_BLE_AD_TYPE_16SRV_PART) {
            for (int i = 0; i < len - 1; i += 2) {
                uint16_t uuid = adv_data[offset + 2 + i] | (adv_data[offset + 3 + i] << 8);
                if (uuid == CUSTOM_SERVICE_UUID) {
                    return true;
                }
            }
        }
        offset += len + 1;
    }
    return false;
}

/**
 * @brief Decrypt encrypted advertising data using pre-shared key
 *
 * No connection required!
 */
static void decrypt_adv_data_no_connect(const uint8_t *adv_data, uint8_t adv_len,
                                         const esp_bd_addr_t addr)
{
    uint8_t offset = 0;

    ESP_LOGI(TAG, "Processing advertising data from "ESP_BD_ADDR_STR"",
             ESP_BD_ADDR_HEX(addr));

    while (offset < adv_len) {
        uint8_t len = adv_data[offset];
        if (len == 0 || offset + len >= adv_len) {
            break;
        }

        uint8_t type = adv_data[offset + 1];

        /* Look for Encrypted Advertising Data (AD Type 0x31) */
        if (type == ESP_BLE_AD_TYPE_ENC_ADV_DATA) {
            const uint8_t *enc_data = &adv_data[offset + 2];
            uint8_t enc_data_len = len - 1;  /* Exclude type byte */

            ESP_LOGI(TAG, "Found encrypted advertising data (%d bytes)", enc_data_len);
            ESP_LOG_BUFFER_HEX(TAG, enc_data, enc_data_len);

            if (enc_data_len < BLE_EAD_RANDOMIZER_SIZE + BLE_EAD_MIC_SIZE) {
                ESP_LOGW(TAG, "Encrypted data too short");
                break;
            }

            /* Decrypt using pre-shared key */
            uint8_t dec_data[32];
            size_t dec_len = BLE_EAD_DECRYPTED_PAYLOAD_SIZE(enc_data_len);

            int rc = ble_ead_decrypt(
                pre_shared_key.session_key,
                pre_shared_key.iv,
                enc_data, enc_data_len,
                dec_data);

            if (rc == 0) {
                ESP_LOGI(TAG, "✅ Decryption successful (no connection needed!)");
                ESP_LOGI(TAG, "Decrypted data (%d bytes):", dec_len);
                ESP_LOG_BUFFER_HEX(TAG, dec_data, dec_len);

                /* Parse the decrypted advertising structure */
                if (dec_len >= 2) {
                    uint8_t inner_len = dec_data[0];
                    uint8_t inner_type = dec_data[1];

                    if (inner_type == ESP_BLE_AD_TYPE_NAME_CMPL ||
                        inner_type == ESP_BLE_AD_TYPE_NAME_SHORT) {
                        char name[32] = {0};
                        size_t name_len = inner_len - 1;
                        if (name_len < sizeof(name) && name_len <= dec_len - 2) {
                            memcpy(name, &dec_data[2], name_len);
                            ESP_LOGI(TAG, "📛 Decrypted device name: \"%s\"", name);
                        }
                    }
                }
            } else {
                ESP_LOGE(TAG, "❌ Decryption failed (rc=%d) - wrong key?", rc);
            }
            return;  /* Found and processed encrypted data */
        }
        offset += len + 1;
    }

    ESP_LOGW(TAG, "No encrypted advertising data found in this packet");
}

/**
 * @brief GAP event handler
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "Scan parameters set, starting scan...");
        esp_ble_gap_start_scanning(0);  /* Scan indefinitely */
        break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "🔍 Scanning started (no connection mode)");
            ESP_LOGI(TAG, "Looking for devices with UUID 0x%04X...", CUSTOM_SERVICE_UUID);
        } else {
            ESP_LOGE(TAG, "Scan start failed: %d", param->scan_start_cmpl.status);
        }
        break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = param;

        if (scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
            uint8_t *adv_data = scan_result->scan_rst.ble_adv;
            uint8_t adv_len = scan_result->scan_rst.adv_data_len;

            /* Check if this is our target device */
            if (is_target_device(adv_data, adv_len)) {
                /* Decrypt without connecting! */
                decrypt_adv_data_no_connect(adv_data, adv_len, scan_result->scan_rst.bda);
            }
        }
        break;
    }

    default:
        break;
    }
}

void app_main(void)
{
    esp_err_t ret;

    ESP_LOGI(TAG, "========================================");
    ESP_LOGI(TAG, "  EAD Central - No Connection Mode");
    ESP_LOGI(TAG, "========================================");

    /* Initialize NVS */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
    ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));

    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bluedroid_init_with_cfg(&cfg));
    ESP_ERROR_CHECK(esp_bluedroid_enable());

    ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_event_handler));

    /* Display pre-shared key */
    ESP_LOGI(TAG, "Using pre-shared key material:");
    ESP_LOGI(TAG, "  Session Key:");
    ESP_LOG_BUFFER_HEX(TAG, pre_shared_key.session_key, BLE_EAD_KEY_SIZE);
    ESP_LOGI(TAG, "  IV:");
    ESP_LOG_BUFFER_HEX(TAG, pre_shared_key.iv, BLE_EAD_IV_SIZE);

    /* Start scanning */
    ESP_ERROR_CHECK(esp_ble_gap_set_scan_params(&ble_scan_params));

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "⚡ This example decrypts WITHOUT connecting!");
    ESP_LOGI(TAG, "   Key must be pre-shared with peripheral.");
}
