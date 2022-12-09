// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <protocomm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * BLE device name cannot be larger than this value
 * 31 bytes (max scan response size) - 1 byte (length) - 1 byte (type) = 29 bytes
 */
#define MAX_BLE_DEVNAME_LEN 29
#define BLE_UUID128_VAL_LENGTH  16

typedef enum {
    // Peer is connected without any security (i.e., no PoP has been entered)
    PROTOCOMM_BLE_PEER_CONNECTED,
    // Peer disconnected
    PROTOCOMM_BLE_PEER_DISCONNECTED,
    // Peer is connected securely with a valid PoP
    PROTOCOMM_BLE_PEER_CONNECTED_SECURE,
} ble_event;

/// type of function called when a peer device connects or disconnects
typedef void (*protocomm_ble_event_fn)(ble_event event);

/**
 * @brief   This structure maps handler required by protocomm layer to
 *          UUIDs which are used to uniquely identify BLE characteristics
 *          from a smartphone or a similar client device.
 */
typedef struct name_uuid {
    /**
     * Name of the handler, which is passed to protocomm layer
     */
    const char *name;

    /**
     * UUID to be assigned to the BLE characteristic which is
     * mapped to the handler
     */
    uint16_t uuid;
} protocomm_ble_name_uuid_t;

/**
 * @brief   Config parameters for protocomm BLE service
 */
typedef struct protocomm_ble_config {
    /**
     * BLE device name being broadcast at the time of provisioning
     */
    char         device_name[MAX_BLE_DEVNAME_LEN];

    /**
     * 128 bit UUID of the provisioning service
     */
    uint8_t      service_uuid[BLE_UUID128_VAL_LENGTH];

    /**
     * Number of entries in the Name-UUID lookup table
     */
    ssize_t      nu_lookup_count;

    /**
     * Pointer to the Name-UUID lookup table
     */
    protocomm_ble_name_uuid_t *nu_lookup;
} protocomm_ble_config_t;

/**
 * @brief   Start Bluetooth Low Energy based transport layer for provisioning
 *
 * Initialize and start required BLE service for provisioning. This includes
 * the initialization for characteristics/service for BLE.
 *
 * @param[in] pc        Protocomm instance pointer obtained from protocomm_new()
 * @param[in] config    Pointer to config structure for initializing BLE
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_FAIL : Simple BLE start error
 *  - ESP_ERR_NO_MEM : Error allocating memory for internal resources
 *  - ESP_ERR_INVALID_STATE : Error in ble config
 *  - ESP_ERR_INVALID_ARG : Null arguments
 */
esp_err_t protocomm_ble_start(protocomm_t *pc, const protocomm_ble_config_t *config);

esp_err_t protocomm_ble_set_manufacturer_data(uint8_t *data, uint8_t length);

/// Register a callback that is called when a BLE event occurs (peer connects or disconnects)
void protocomm_ble_register_ble_event_fn(protocomm_ble_event_fn fn);

/// Get the registered BLE event callback
///
/// @return protocomm_ble_event_fn callback, or NULL if no callback is registered.
protocomm_ble_event_fn protocomm_ble_get_ble_event_fn(void);

/**
 * @brief   Stop Bluetooth Low Energy based transport layer for provisioning
 *
 * Stops service/task responsible for BLE based interactions for provisioning
 *
 * @note    You might want to optionally reclaim memory from Bluetooth.
 *          Refer to the documentation of `esp_bt_mem_release` in that case.
 *
 * @param[in] pc    Same protocomm instance that was passed to protocomm_ble_start()
 *
 * @return
 *  - ESP_OK : Success
 *  - ESP_FAIL : Simple BLE stop error
 *  - ESP_ERR_INVALID_ARG : Null / incorrect protocomm instance
 */
esp_err_t protocomm_ble_stop(protocomm_t *pc);

#ifdef __cplusplus
}
#endif
