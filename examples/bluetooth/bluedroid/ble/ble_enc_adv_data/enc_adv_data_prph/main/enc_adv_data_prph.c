/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/**
 * @brief BLE Encrypted Advertising Data Peripheral Example
 *
 * This example demonstrates how to:
 * 1. Encrypt advertising data using AES-CCM
 * 2. Broadcast encrypted advertising data
 * 3. Provide Key Material characteristic for central devices to read
 *
 * Based on Bluetooth Core Specification Version 5.4 - Encrypted Advertising Data
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "ble_ead.h"

#define TAG "ENC_ADV_PRPH"

/* Device name */
#define DEVICE_NAME             "ENC_ADV_PRPH"
#define GAP_SERVICE_UUID        0x1800  /* GAP Service UUID */

/* Profile configuration */
#define PROFILE_NUM             1
#define PROFILE_APP_ID          0

/* Unencrypted advertising pattern to be encrypted */
static uint8_t unencrypted_adv_pattern[] = {
    0x05, 0x09, 'p', 'r', 'p', 'h'  /* Complete Local Name: "prph" */
};

/* Session key and IV for encryption - in real application, generate securely! */
static ble_ead_key_material_t key_material = {
    .session_key = {
        0x19, 0x6a, 0x0a, 0xd1, 0x2a, 0x61, 0x20, 0x1e,
        0x13, 0x6e, 0x2e, 0xd1, 0x12, 0xda, 0xa9, 0x57
    },
    .iv = {0x9E, 0x7a, 0x00, 0xef, 0xb1, 0x7a, 0xe7, 0x46},
};

/* GATT state */
static esp_gatt_if_t gatts_if_stored = ESP_GATT_IF_NONE;
static uint16_t conn_id_stored = 0;
static bool is_connected = false;

/* Advertising parameters */
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* Calculate encrypted payload size */
#define ENCRYPTED_ADV_DATA_LEN  BLE_EAD_ENCRYPTED_PAYLOAD_SIZE(sizeof(unencrypted_adv_pattern))

/**
 * @brief Encrypt advertising data and set raw advertising data
 */
static void set_encrypted_adv_data(void)
{
    esp_err_t ret;
    uint8_t encrypted_adv_data[ENCRYPTED_ADV_DATA_LEN];
    int rc;

    ESP_LOGI(TAG, "Data before encryption:");
    ESP_LOG_BUFFER_HEX(TAG, unencrypted_adv_pattern, sizeof(unencrypted_adv_pattern));

    /* Encrypt the advertising data */
    rc = ble_ead_encrypt(key_material.session_key, key_material.iv,
                          unencrypted_adv_pattern, sizeof(unencrypted_adv_pattern),
                          encrypted_adv_data);
    if (rc != 0) {
        ESP_LOGE(TAG, "Encryption of adv data failed: %d", rc);
        return;
    }

    ESP_LOGI(TAG, "Encryption of adv data done successfully");
    ESP_LOGI(TAG, "Data after encryption:");
    ESP_LOG_BUFFER_HEX(TAG, encrypted_adv_data, sizeof(encrypted_adv_data));

    /*
     * Build raw advertising data:
     * - Flags (3 bytes)
     * - Complete Local Name (device name)
     * - Complete 16-bit Service UUIDs (for central to recognize)
     * - Encrypted Advertising Data
     */
    uint8_t raw_adv_data[31];
    uint8_t pos = 0;

    /* Flags */
    raw_adv_data[pos++] = 0x02;  /* Length */
    raw_adv_data[pos++] = ESP_BLE_AD_TYPE_FLAG;
    raw_adv_data[pos++] = ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT;

    /* Complete Local Name - "key" (short name for recognition) */
    raw_adv_data[pos++] = 0x04;  /* Length */
    raw_adv_data[pos++] = ESP_BLE_AD_TYPE_NAME_CMPL;
    raw_adv_data[pos++] = 'k';
    raw_adv_data[pos++] = 'e';
    raw_adv_data[pos++] = 'y';

    /* Complete 16-bit Service UUIDs - GAP Service (0x1800) */
    raw_adv_data[pos++] = 0x03;  /* Length */
    raw_adv_data[pos++] = ESP_BLE_AD_TYPE_16SRV_CMPL;
    raw_adv_data[pos++] = GAP_SERVICE_UUID & 0xFF;
    raw_adv_data[pos++] = (GAP_SERVICE_UUID >> 8) & 0xFF;

    /* Encrypted Advertising Data */
    raw_adv_data[pos++] = ENCRYPTED_ADV_DATA_LEN + 1;  /* Length */
    raw_adv_data[pos++] = ESP_BLE_AD_TYPE_ENC_ADV_DATA;
    memcpy(&raw_adv_data[pos], encrypted_adv_data, ENCRYPTED_ADV_DATA_LEN);
    pos += ENCRYPTED_ADV_DATA_LEN;

    /* Set raw advertising data */
    ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, pos);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "config raw adv data failed: %s", esp_err_to_name(ret));
    }
}

/**
 * @brief Start advertising
 */
static void start_advertising(void)
{
    esp_err_t ret = esp_ble_gap_start_advertising(&adv_params);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "start advertising failed: %s", esp_err_to_name(ret));
    }
}

/**
 * @brief GAP event handler
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "Raw advertising data set complete");
        start_advertising();
        break;

    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising start failed: %d", param->adv_start_cmpl.status);
        } else {
            ESP_LOGI(TAG, "Advertising start successfully");
        }
        break;

    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Advertising stop failed: %d", param->adv_stop_cmpl.status);
        } else {
            ESP_LOGI(TAG, "Advertising stop successfully");
        }
        break;

    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        ESP_LOGI(TAG, "Connection params update, status %d, conn_int %d, latency %d, timeout %d",
                 param->update_conn_params.status,
                 param->update_conn_params.conn_int,
                 param->update_conn_params.latency,
                 param->update_conn_params.timeout);
        break;

    case ESP_GAP_BLE_SEC_REQ_EVT:
        ESP_LOGI(TAG, "Security request received");
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;

    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        ESP_LOGI(TAG, "Authentication complete, addr_type %d, addr "ESP_BD_ADDR_STR"",
                 param->ble_security.auth_cmpl.addr_type,
                 ESP_BD_ADDR_HEX(param->ble_security.auth_cmpl.bd_addr));
        if (param->ble_security.auth_cmpl.success) {
            ESP_LOGI(TAG, "Authentication success, auth_mode %d", param->ble_security.auth_cmpl.auth_mode);
        } else {
            ESP_LOGW(TAG, "Authentication failed, reason 0x%x", param->ble_security.auth_cmpl.fail_reason);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief GATTS profile event handler
 */
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                 esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(TAG, "GATT server register, status %d, app_id %d, gatts_if %d",
                 param->reg.status, param->reg.app_id, gatts_if);
        gatts_if_stored = gatts_if;

        /* Set device name */
        esp_ble_gap_set_device_name(DEVICE_NAME);

        /* Set encrypted advertising data */
        set_encrypted_adv_data();

        /* Set Key Material in GAP service
         * The Key Material characteristic is part of the built-in GAP service
         */
        ESP_LOGI(TAG, "Setting Key Material in GAP service");
        esp_ble_gap_set_key_material(key_material.session_key, key_material.iv);
        break;

    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(TAG, "Connected, conn_id %d, remote "ESP_BD_ADDR_STR"",
                 param->connect.conn_id, ESP_BD_ADDR_HEX(param->connect.remote_bda));
        conn_id_stored = param->connect.conn_id;
        is_connected = true;

        /* Update connection parameters */
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        conn_params.latency = 0;
        conn_params.max_int = 0x20;
        conn_params.min_int = 0x10;
        conn_params.timeout = 400;
        esp_ble_gap_update_conn_params(&conn_params);
        break;

    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(TAG, "Disconnected, remote "ESP_BD_ADDR_STR", reason 0x%02x",
                 ESP_BD_ADDR_HEX(param->disconnect.remote_bda), param->disconnect.reason);
        is_connected = false;

        /* Re-encrypt and restart advertising with new randomizer */
        set_encrypted_adv_data();
        break;

    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(TAG, "MTU exchange, MTU %d", param->mtu.mtu);
        break;

    default:
        break;
    }
}

void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Release memory for Classic BT */
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

    /* Initialize BT controller */
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "initialize controller failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "enable controller failed: %s", esp_err_to_name(ret));
        return;
    }

    /* Initialize Bluedroid */
    esp_bluedroid_config_t cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ret = esp_bluedroid_init_with_cfg(&cfg);
    if (ret) {
        ESP_LOGE(TAG, "init bluetooth failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "enable bluetooth failed: %s", esp_err_to_name(ret));
        return;
    }

    /* Register callbacks */
    ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gatts register error: %x", ret);
        return;
    }

    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gap register error: %x", ret);
        return;
    }

    /* Register GATT application */
    ret = esp_ble_gatts_app_register(PROFILE_APP_ID);
    if (ret) {
        ESP_LOGE(TAG, "gatts app register error: %x", ret);
        return;
    }

    /* Set MTU */
    esp_ble_gatt_set_local_mtu(500);

    /* Configure security parameters for Key Material characteristic access
     * Using SC (Secure Connections) with bonding, no MITM (since IO_CAP is NONE)
     */
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_BOND;  /* SC + Bond, no MITM */
    esp_ble_io_cap_t io_cap = ESP_IO_CAP_NONE;
    uint8_t key_size = 16;
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(auth_req));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &io_cap, sizeof(io_cap));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(key_size));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(init_key));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(rsp_key));

    ESP_LOGI(TAG, "Encrypted Advertising Data Peripheral started");
    ESP_LOGI(TAG, "Key Material (Session Key + IV):");
    ESP_LOG_BUFFER_HEX(TAG, &key_material, sizeof(key_material));
}
