/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/**
 * @brief BLE Encrypted Advertising Data Central Example
 *
 * This example demonstrates how to:
 * 1. Scan for devices broadcasting encrypted advertising data
 * 2. Connect to read Key Material characteristic
 * 3. Decrypt the advertising data using the obtained key
 *
 * Based on Bluetooth Core Specification Version 5.4 - Encrypted Advertising Data
 */

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
#include "ble_ead.h"

#define TAG "ENC_ADV_CENT"

/* Service and characteristic UUIDs */
#define GAP_SERVICE_UUID        0x1800  /* GAP Service UUID */
#define KEY_MATERIAL_CHAR_UUID  0x2B88  /* Key Material Characteristic UUID */

/* Profile configuration */
#define PROFILE_NUM             1
#define PROFILE_APP_ID          0
#define INVALID_HANDLE          0

/* Maximum peers to track */
#define MAX_PEERS               5

/* Peer information structure */
typedef struct {
    bool valid;
    esp_bd_addr_t addr;
    bool key_material_exist;
    ble_ead_key_material_t key_material;
} peer_info_t;

static peer_info_t peers[MAX_PEERS] = {0};

/* GATT client state */
static bool is_connected = false;
static bool get_server = false;
static uint16_t conn_id_stored = 0;
static uint16_t service_start_handle = 0;
static uint16_t service_end_handle = 0;
static uint16_t key_material_char_handle = INVALID_HANDLE;
static esp_bd_addr_t current_peer_addr = {0};

/* GATT interface */
static esp_gatt_if_t gattc_if_stored = ESP_GATT_IF_NONE;

/* Scan parameters */
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type          = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval      = 0x50,
    .scan_window        = 0x30,
    .scan_duplicate     = BLE_SCAN_DUPLICATE_DISABLE,
};

/* Forward declarations */
static void start_scan(void);

/**
 * @brief Find peer by address
 */
static int find_peer(const esp_bd_addr_t addr)
{
    for (int i = 0; i < MAX_PEERS; i++) {
        if (peers[i].valid && memcmp(peers[i].addr, addr, sizeof(esp_bd_addr_t)) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Add or update peer
 */
static int add_peer(const esp_bd_addr_t addr)
{
    int idx = find_peer(addr);
    if (idx >= 0) {
        return idx;  /* Already exists */
    }

    /* Find empty slot */
    for (int i = 0; i < MAX_PEERS; i++) {
        if (!peers[i].valid) {
            peers[i].valid = true;
            memcpy(peers[i].addr, addr, sizeof(esp_bd_addr_t));
            peers[i].key_material_exist = false;
            return i;
        }
    }
    return -1;  /* No space */
}

/**
 * @brief Decrypt encrypted advertising data
 */
static void decrypt_enc_adv_data(const uint8_t *adv_data, uint8_t adv_len, const esp_bd_addr_t addr)
{
    int peer_idx = find_peer(addr);
    if (peer_idx < 0 || !peers[peer_idx].key_material_exist) {
        ESP_LOGW(TAG, "No key material for peer, cannot decrypt");
        return;
    }

    uint8_t offset = 0;
    while (offset < adv_len) {
        uint8_t len = adv_data[offset];
        if (len == 0 || offset + len >= adv_len) {
            break;
        }

        uint8_t type = adv_data[offset + 1];
        if (type == ESP_BLE_AD_TYPE_ENC_ADV_DATA) {
            /* Found encrypted advertising data */
            const uint8_t *enc_data = &adv_data[offset + 2];
            uint8_t enc_data_len = len - 1;  /* Exclude type byte */

            if (enc_data_len < BLE_EAD_RANDOMIZER_SIZE + BLE_EAD_MIC_SIZE) {
                ESP_LOGW(TAG, "Encrypted data too short");
                break;
            }

            uint8_t dec_data[32];  /* Buffer for decrypted data */
            size_t dec_len = BLE_EAD_DECRYPTED_PAYLOAD_SIZE(enc_data_len);

            int rc = ble_ead_decrypt(
                peers[peer_idx].key_material.session_key,
                peers[peer_idx].key_material.iv,
                enc_data, enc_data_len,
                dec_data);

            if (rc == 0) {
                ESP_LOGI(TAG, "Decryption successful!");
                ESP_LOGI(TAG, "Decrypted data:");
                ESP_LOG_BUFFER_HEX(TAG, dec_data, dec_len);

                /* Parse decrypted advertising structure */
                if (dec_len >= 2) {
                    uint8_t dec_type = dec_data[1];
                    if (dec_type == ESP_BLE_AD_TYPE_NAME_CMPL || dec_type == ESP_BLE_AD_TYPE_NAME_SHORT) {
                        char name[32] = {0};
                        size_t name_len = dec_data[0] - 1;
                        if (name_len < sizeof(name)) {
                            memcpy(name, &dec_data[2], name_len);
                            ESP_LOGI(TAG, "Decrypted device name: %s", name);
                        }
                    }
                }
            } else {
                ESP_LOGE(TAG, "Decryption failed: %d", rc);
            }
            break;
        }
        offset += len + 1;
    }
}

/**
 * @brief Check if device advertises GAP service UUID
 */
static bool should_connect(const uint8_t *adv_data, uint8_t adv_len)
{
    uint8_t offset = 0;
    while (offset < adv_len) {
        uint8_t len = adv_data[offset];
        if (len == 0 || offset + len >= adv_len) {
            break;
        }

        uint8_t type = adv_data[offset + 1];
        if (type == ESP_BLE_AD_TYPE_16SRV_CMPL || type == ESP_BLE_AD_TYPE_16SRV_PART) {
            /* Check for GAP service UUID */
            for (int i = 0; i < len - 1; i += 2) {
                uint16_t uuid = adv_data[offset + 2 + i] | (adv_data[offset + 3 + i] << 8);
                if (uuid == GAP_SERVICE_UUID) {
                    return true;
                }
            }
        }
        offset += len + 1;
    }
    return false;
}

/**
 * @brief Start scanning
 */
static void start_scan(void)
{
    esp_ble_gap_start_scanning(30);  /* Scan for 30 seconds */
}

/**
 * @brief GAP event handler
 */
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        ESP_LOGI(TAG, "Scan parameters set");
        start_scan();
        break;

    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(TAG, "Scan start failed: %d", param->scan_start_cmpl.status);
        } else {
            ESP_LOGI(TAG, "Scanning started");
        }
        break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT: {
        esp_ble_gap_cb_param_t *scan_result = param;
        if (scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
            uint8_t *adv_data = scan_result->scan_rst.ble_adv;
            uint8_t adv_len = scan_result->scan_rst.adv_data_len;

            if (should_connect(adv_data, adv_len)) {
                ESP_LOGI(TAG, "Found target device: "ESP_BD_ADDR_STR"",
                         ESP_BD_ADDR_HEX(scan_result->scan_rst.bda));

                int peer_idx = find_peer(scan_result->scan_rst.bda);
                if (peer_idx >= 0 && peers[peer_idx].key_material_exist) {
                    /* Already have key, try to decrypt */
                    ESP_LOGI(TAG, "Have key material, decrypting...");
                    decrypt_enc_adv_data(adv_data, adv_len, scan_result->scan_rst.bda);
                } else {
                    /* Need to connect and get key */
                    if (!is_connected) {
                        ESP_LOGI(TAG, "Connecting to get key material...");
                        add_peer(scan_result->scan_rst.bda);
                        memcpy(current_peer_addr, scan_result->scan_rst.bda, sizeof(esp_bd_addr_t));

                        esp_ble_gap_stop_scanning();

                        esp_ble_gatt_creat_conn_params_t conn_params = {0};
                        memcpy(conn_params.remote_bda, scan_result->scan_rst.bda, ESP_BD_ADDR_LEN);
                        conn_params.remote_addr_type = scan_result->scan_rst.ble_addr_type;
                        conn_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
                        conn_params.is_direct = true;
                        conn_params.is_aux = false;
                        esp_ble_gattc_enh_open(gattc_if_stored, &conn_params);
                    }
                }
            }
        } else if (scan_result->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
            ESP_LOGI(TAG, "Scan complete");
            if (!is_connected) {
                start_scan();  /* Restart scanning */
            }
        }
        break;
    }

    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        ESP_LOGI(TAG, "Scan stopped");
        break;

    case ESP_GAP_BLE_SEC_REQ_EVT:
        ESP_LOGI(TAG, "Security request");
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;

    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        if (param->ble_security.auth_cmpl.success) {
            ESP_LOGI(TAG, "Authentication success");
        } else {
            ESP_LOGW(TAG, "Authentication failed: 0x%x", param->ble_security.auth_cmpl.fail_reason);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief GATTC event handler
 */
static void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                 esp_ble_gattc_cb_param_t *param)
{
    switch (event) {
    case ESP_GATTC_REG_EVT:
        ESP_LOGI(TAG, "GATT client registered, status %d, if %d", param->reg.status, gattc_if);
        gattc_if_stored = gattc_if;
        esp_ble_gap_set_scan_params(&ble_scan_params);
        break;

    case ESP_GATTC_CONNECT_EVT:
        ESP_LOGI(TAG, "Connected, conn_id %d", param->connect.conn_id);
        conn_id_stored = param->connect.conn_id;
        is_connected = true;

        /* Request MTU exchange */
        esp_ble_gattc_send_mtu_req(gattc_if, param->connect.conn_id);
        break;

    case ESP_GATTC_OPEN_EVT:
        if (param->open.status != ESP_GATT_OK) {
            ESP_LOGE(TAG, "Open failed: %d", param->open.status);
            is_connected = false;
            start_scan();
        }
        break;

    case ESP_GATTC_CFG_MTU_EVT:
        ESP_LOGI(TAG, "MTU configured: %d", param->cfg_mtu.mtu);
        break;

    case ESP_GATTC_DIS_SRVC_CMPL_EVT:
        ESP_LOGI(TAG, "Service discovery complete");
        /* Search for GAP service that contains Key Material characteristic */
        ESP_LOGI(TAG, "Searching for GAP service UUID 0x%04X", GAP_SERVICE_UUID);
        esp_bt_uuid_t gap_uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid = {.uuid16 = GAP_SERVICE_UUID},
        };
        esp_ble_gattc_search_service(gattc_if, param->dis_srvc_cmpl.conn_id, &gap_uuid);
        break;

    case ESP_GATTC_SEARCH_RES_EVT:
        ESP_LOGI(TAG, "Service found, UUID 0x%04X, start_handle %d, end_handle %d",
                 param->search_res.srvc_id.uuid.uuid.uuid16,
                 param->search_res.start_handle, param->search_res.end_handle);
        if (param->search_res.srvc_id.uuid.len == ESP_UUID_LEN_16 &&
            param->search_res.srvc_id.uuid.uuid.uuid16 == GAP_SERVICE_UUID) {
            get_server = true;
            service_start_handle = param->search_res.start_handle;
            service_end_handle = param->search_res.end_handle;
        }
        break;

    case ESP_GATTC_SEARCH_CMPL_EVT:
        ESP_LOGI(TAG, "Service search complete");
        if (get_server) {
            /* Get characteristics */
            uint16_t count = 0;
            esp_ble_gattc_get_attr_count(gattc_if, conn_id_stored,
                                          ESP_GATT_DB_CHARACTERISTIC,
                                          service_start_handle,
                                          service_end_handle,
                                          INVALID_HANDLE, &count);

            if (count > 0) {
                esp_gattc_char_elem_t *char_elem = malloc(sizeof(esp_gattc_char_elem_t) * count);
                if (char_elem) {
                    esp_bt_uuid_t km_uuid = {
                        .len = ESP_UUID_LEN_16,
                        .uuid = {.uuid16 = KEY_MATERIAL_CHAR_UUID},
                    };
                    esp_ble_gattc_get_char_by_uuid(gattc_if, conn_id_stored,
                                                    service_start_handle,
                                                    service_end_handle,
                                                    km_uuid, char_elem, &count);

                    if (count > 0) {
                        key_material_char_handle = char_elem[0].char_handle;
                        ESP_LOGI(TAG, "Key Material characteristic found, handle %d", key_material_char_handle);

                        /* Read characteristic with encryption requirement
                         * GATT layer will automatically trigger encryption if needed */
                        ESP_LOGI(TAG, "Reading key material (will trigger encryption if needed)...");
                        esp_ble_gattc_read_char(gattc_if, conn_id_stored,
                                                key_material_char_handle, ESP_GATT_AUTH_REQ_NO_MITM);
                    }
                    free(char_elem);
                }
            }
        }
        break;

    case ESP_GATTC_READ_CHAR_EVT:
        if (param->read.status == ESP_GATT_OK) {
            ESP_LOGI(TAG, "Read characteristic success, handle %d, len %d",
                     param->read.handle, param->read.value_len);

            if (param->read.handle == key_material_char_handle &&
                param->read.value_len == sizeof(ble_ead_key_material_t)) {
                /* Store key material */
                int peer_idx = find_peer(current_peer_addr);
                if (peer_idx >= 0) {
                    memcpy(&peers[peer_idx].key_material, param->read.value,
                           sizeof(ble_ead_key_material_t));
                    peers[peer_idx].key_material_exist = true;

                    ESP_LOGI(TAG, "Key material received:");
                    ESP_LOG_BUFFER_HEX(TAG, &peers[peer_idx].key_material,
                                        sizeof(ble_ead_key_material_t));
                }

                /* Disconnect and resume scanning */
                esp_ble_gattc_close(gattc_if, conn_id_stored);
            }
        } else {
            ESP_LOGE(TAG, "Read failed: %d", param->read.status);
        }
        break;

    case ESP_GATTC_DISCONNECT_EVT:
        ESP_LOGI(TAG, "Disconnected, reason 0x%02x", param->disconnect.reason);
        is_connected = false;
        get_server = false;
        key_material_char_handle = INVALID_HANDLE;
        start_scan();
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
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gap register error: %x", ret);
        return;
    }

    ret = esp_ble_gattc_register_callback(gattc_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "gattc register error: %x", ret);
        return;
    }

    ret = esp_ble_gattc_app_register(PROFILE_APP_ID);
    if (ret) {
        ESP_LOGE(TAG, "gattc app register error: %x", ret);
        return;
    }

    /* Set MTU */
    esp_ble_gatt_set_local_mtu(500);

    /* Configure security parameters
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

    ESP_LOGI(TAG, "Encrypted Advertising Data Central started");
}
