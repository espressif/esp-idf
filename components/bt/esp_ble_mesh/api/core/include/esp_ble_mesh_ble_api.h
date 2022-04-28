/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_BLE_API_H_
#define _ESP_BLE_MESH_BLE_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This enum value is the event of BLE operations */
typedef enum {
    ESP_BLE_MESH_START_BLE_ADVERTISING_COMP_EVT, /*!< Start BLE advertising completion event */
    ESP_BLE_MESH_STOP_BLE_ADVERTISING_COMP_EVT,  /*!< Stop BLE advertising completion event */
    ESP_BLE_MESH_START_BLE_SCANNING_COMP_EVT,    /*!< Start BLE scanning completion event */
    ESP_BLE_MESH_STOP_BLE_SCANNING_COMP_EVT,     /*!< Stop BLE scanning completion event */
    ESP_BLE_MESH_SCAN_BLE_ADVERTISING_PKT_EVT,   /*!< Scanning BLE advertising packets event */
    ESP_BLE_MESH_BLE_EVT_MAX,
} esp_ble_mesh_ble_cb_event_t;

/** BLE operation callback parameters */
typedef union {
    /**
     * @brief ESP_BLE_MESH_START_BLE_ADVERTISING_COMP_EVT
     */
    struct {
        int err_code;             /*!< Indicate the result of starting BLE advertising */
        uint8_t index;            /*!< Index of the BLE advertising */
    } start_ble_advertising_comp; /*!< Event parameters of ESP_BLE_MESH_START_BLE_ADVERTISING_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_STOP_BLE_ADVERTISING_COMP_EVT
     */
    struct {
        int err_code;            /*!< Indicate the result of stopping BLE advertising */
        uint8_t index;           /*!< Index of the BLE advertising */
    } stop_ble_advertising_comp; /*!< Event parameters of ESP_BLE_MESH_STOP_BLE_ADVERTISING_COMP_EVT */
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

/** Count for sending BLE advertising packet infinitely */
#define ESP_BLE_MESH_BLE_ADV_INFINITE   0xFFFF

/*!< This enum value is the priority of BLE advertising packet */
typedef enum {
    ESP_BLE_MESH_BLE_ADV_PRIO_LOW,
    ESP_BLE_MESH_BLE_ADV_PRIO_HIGH,
} esp_ble_mesh_ble_adv_priority_t;

/** Context of BLE advertising parameters. */
typedef struct {
    uint16_t interval;               /*!< BLE advertising interval */
    uint8_t  adv_type;               /*!< BLE advertising type */
    uint8_t  own_addr_type;          /*!< Own address type */
    uint8_t  peer_addr_type;         /*!< Peer address type */
    uint8_t  peer_addr[BD_ADDR_LEN]; /*!< Peer address */
    uint16_t duration;               /*!< Duration is milliseconds */
    uint16_t period;                 /*!< Period in milliseconds */
    uint16_t count;                  /*!< Number of advertising duration */
    uint8_t  priority:2;             /*!< Priority of BLE advertising packet */
} esp_ble_mesh_ble_adv_param_t;

/** Context of BLE advertising data. */
typedef struct {
    uint8_t adv_data_len;      /*!< Advertising data length */
    uint8_t adv_data[31];      /*!< Advertising data */
    uint8_t scan_rsp_data_len; /*!< Scan response data length */
    uint8_t scan_rsp_data[31]; /*!< Scan response data */
} esp_ble_mesh_ble_adv_data_t;

/**
 * @brief         This function is called to start BLE advertising with the corresponding data
 *                and parameters while BLE Mesh is working at the same time.
 *
 * @note          1. When this function is called, the BLE advertising packet will be posted to
 *                the BLE mesh adv queue in the mesh stack and waited to be sent.
 *                2. In the BLE advertising parameters, the "duration" means the time used for
 *                sending the BLE advertising packet each time, it shall not be smaller than the
 *                advertising interval. When the packet is sent successfully, it will be posted
 *                to the adv queue again after the "period" time if the "count" is bigger than 0.
 *                The "count" means how many durations the packet will be sent after it is sent
 *                successfully for the first time. And if the "count" is set to 0xFFFF, which
 *                means the packet will be sent infinitely.
 *                3. The "priority" means the priority of BLE advertising packet compared with
 *                BLE Mesh packets. Currently two options (i.e. low/high) are provided. If the
 *                "priority" is high, the BLE advertising packet will be posted to the front of
 *                adv queue. Otherwise it will be posted to the back of adv queue.
 *
 * @param[in]     param: Pointer to the BLE advertising parameters
 * @param[in]     data:  Pointer to the BLE advertising data and scan response data
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_start_ble_advertising(const esp_ble_mesh_ble_adv_param_t *param,
                                             const esp_ble_mesh_ble_adv_data_t *data);

/**
 * @brief         This function is called to stop BLE advertising with the corresponding index.
 *
 * @param[in]     index: Index of BLE advertising
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_stop_ble_advertising(uint8_t index);

/** Context of BLE scanning parameters. */
typedef struct {
    uint32_t duration; /*!< Duration used to scan normal BLE advertising packets */
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
