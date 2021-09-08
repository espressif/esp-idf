/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
 /**
 * Theoretically, the limit for max manufacturer length remains same as BLE
 * device name i.e. 31 bytes (max scan response size) - 1 byte (length) - 1
 * byte (type) = 29 bytes
 * However, manufacturer data goes along with BLE device name in scan response.
 * So, it is important to understand the actual length should be smaller than
 * (29 - (BLE device name length) - 2). */
#define MAX_BLE_MANUFACTURER_DATA_LEN 29

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
     * BLE device manufacturer data pointer in advertisement
     */
    uint8_t      *manufacturer_data;

    /**
     * BLE device manufacturer data length in advertisement
     */
    ssize_t      manufacturer_data_len;

    /**
     * Number of entries in the Name-UUID lookup table
     */
    ssize_t      nu_lookup_count;

    /**
     * Pointer to the Name-UUID lookup table
     */
    protocomm_ble_name_uuid_t *nu_lookup;

    /* BLE bonding */
    unsigned ble_bonding:1;

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
