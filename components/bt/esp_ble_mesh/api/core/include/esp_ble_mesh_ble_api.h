// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_BLE_MESH_BLE_API_H_
#define _ESP_BLE_MESH_BLE_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This enum value is the event of BLE operations */
typedef enum {
    ESP_BLE_MESH_START_BLE_SCANNING_COMP_EVT,    /*!< Start BLE scanning completion event */
    ESP_BLE_MESH_STOP_BLE_SCANNING_COMP_EVT,     /*!< Stop BLE scanning completion event */
    ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT,   /*!< Scanning BLE advertising packets event */
    ESP_BLE_MESH_BLE_EVT_MAX,
} esp_ble_mesh_ble_cb_event_t;

/** BLE operation callback parameters */
typedef union {
    /**
     * @brief ESP_BLE_MESH_START_BLE_SCANNING_COMP_EVT
     */
    struct {
        int err_code;      /*!< Indicate the result of starting BLE scanning */
    } start_ble_scan_comp; /*!< Event parameters of ESP_BLE_MESH_START_BLE_SCANNING_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_STOP_BLE_SCANNING_COMP_EVT
     */
    struct {
        int err_code;     /*!< Indicate the result of stopping BLE scanning */
    } stop_ble_scan_comp; /*!< Event parameters of ESP_BLE_MESH_STOP_BLE_SCANNING_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT
     */
    struct {
        uint8_t  addr[6];   /*!< Device address */
        uint8_t  addr_type; /*!< Device address type */
        uint8_t  adv_type;  /*!< Advertising data type */
        uint8_t *data;      /*!< Advertising data */
        uint16_t length;    /*!< Advertising data length */
        int8_t   rssi;      /*!< RSSI of the advertising packet */
    } scan_ble_adv_pkt;     /*!< Event parameters of ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT */
} esp_ble_mesh_ble_cb_param_t;

/**
 * @brief   BLE scanning callback function type
 *
 * @param   event: BLE scanning callback event type
 * @param   param: BLE scanning callback parameter
 */
typedef void (* esp_ble_mesh_ble_cb_t)(esp_ble_mesh_ble_cb_event_t event,
                                       esp_ble_mesh_ble_cb_param_t *param);

/**
 * @brief       Register BLE scanning callback.
 *
 * @param[in]   callback: Pointer to the BLE scaning callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_ble_callback(esp_ble_mesh_ble_cb_t callback);

/** Context of BLE scanning parameters. */
typedef struct {
    uint8_t dummy;  /*!< Currently no parameters for BLE scanning */
} esp_ble_mesh_ble_scan_param_t;

/**
 * @brief         This function is called to start scanning normal BLE advertising packets
 *                and notifying the packets to the application layer.
 *
 * @param[in]     param: Pointer to the BLE scanning parameters
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_start_ble_scanning(esp_ble_mesh_ble_scan_param_t *param);

/**
 * @brief         This function is called to stop notifying normal BLE advertising packets
 *                to the application layer.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_stop_ble_scanning(void);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_BLE_API_H_ */
