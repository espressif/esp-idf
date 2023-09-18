/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SIMPLE_BLE_
#define _SIMPLE_BLE_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>

typedef void (simple_ble_cb_t)(esp_gatts_cb_event_t event, esp_gatt_if_t p_gatts_if, esp_ble_gatts_cb_param_t *param);

/**
 * This structure is populated with the details required
 * to create an instance of BLE easily. It requires function
 * pointers, advertising parameters and gatt description table
 */
typedef struct {
    /** Name to be displayed to devices scanning for ESP32 */
    const char *device_name;
    /** Raw advertisement data */
    esp_ble_adv_data_t *adv_data_p;
    /** Raw scan response data */
    esp_ble_adv_data_t *scan_rsp_data_p;
    /** Parameters to configure the nature of advertising */
    esp_ble_adv_params_t adv_params;
    /** Descriptor table which consists of the configuration
     * required by services and characteristics */
    esp_gatts_attr_db_t *gatt_db;
    /** Number of entries in the gatt_db descriptor table */
    ssize_t gatt_db_count;
    /** BLE read callback */
    simple_ble_cb_t *read_fn;
    /** BLE write callback */
    simple_ble_cb_t *write_fn;
    /** BLE exec write callback */
    simple_ble_cb_t *exec_write_fn;
    /** Client disconnect callback */
    simple_ble_cb_t *disconnect_fn;
    /** Client connect callback */
    simple_ble_cb_t *connect_fn;
    /** MTU set callback */
    simple_ble_cb_t *set_mtu_fn;
    /** BLE bonding */
    unsigned ble_bonding:1;
    /** BLE Secure Connection flag */
    unsigned ble_sm_sc:1;
} simple_ble_cfg_t;


/** Initialize a simple ble connection
 *
 * This function allocates memory and returns a pointer to the
 * configuration structure.
 *
 * @return simple_ble_cfg_t* Pointer to configuration structure
 */
simple_ble_cfg_t *simple_ble_init(void);

/** Deallocates memory
 *
 * This function deallocate memory of the configuration structure.
 *
 * @return ESP_OK
 */
esp_err_t simple_ble_deinit(void);

/** Starts BLE service
 *
 * This function makes calls to the GATT and GAP APIs
 * to initialize the BLE service as per parameters stored
 * in the config structure. At the end of this function,
 * one should be able to scan and connect to the ESP32 device
 * using BLE.
 * This API sets the MTU size to 500 (this is not part of the config structure)
 *
 * @return ESP_OK on success, and appropriate error code for failure
 */
esp_err_t simple_ble_start(simple_ble_cfg_t *cfg);

/** Stops the BLE service
 *
 * This API is called to stop the BLE service.
 * This includes calls to disable and deinit bluedroid and bt controller.
 *
 * @return ESP_OK on success, and appropriate error code for failure
 */
esp_err_t simple_ble_stop(void);

/** Convert handle to 128 bit UUID of characteristic
 *
 * This function can be easily used to get the corresponding
 * UUID for a characteristic that has been created, and the one for
 * which we only have the handle for.
 *
 * @return Pointer to UUID of the characteristic
 *         NULL in case of invalid handle
 */
const uint8_t *simple_ble_get_uuid128(uint16_t handle);

/** Terminates connection
 *
 * This API is called to initiate disconnection
 *
 * @return ESP_OK on success, and appropriate error code for failure
 */
esp_err_t simple_ble_disconnect(void);
#endif /* _SIMPLE_BLE_ */
