/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BT_DEVICE_H__
#define __ESP_BT_DEVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

// coexist status for MESH
#define ESP_BT_DEV_COEX_BLE_ST_MESH_CONFIG        0x08
#define ESP_BT_DEV_COEX_BLE_ST_MESH_TRAFFIC       0x10
#define ESP_BT_DEV_COEX_BLE_ST_MESH_STANDBY       0x20
// coexist status for A2DP
#define ESP_BT_DEV_COEX_BT_ST_A2DP_STREAMING      0x10
#define ESP_BT_DEV_COEX_BT_ST_A2DP_PAUSED         0x20

// coexist operation
#define ESP_BT_DEV_COEX_OP_CLEAR                  0x00
#define ESP_BT_DEV_COEX_OP_SET                    0x01
typedef uint8_t esp_bt_dev_coex_op_t;

/**
 * @brief Bluetooth device coex type
 */
typedef enum {
    ESP_BT_DEV_COEX_TYPE_BLE = 1,
    ESP_BT_DEV_COEX_TYPE_BT,
} esp_bt_dev_coex_type_t;

/// BT device callback events
typedef enum {
    ESP_BT_DEV_NAME_RES_EVT = 0,                    /*!< Device name result event */
    ESP_BT_DEV_EVT_MAX,
} esp_bt_dev_cb_event_t;

/// BT device callback parameters
typedef union {
    /**
     * @brief ESP_BT_DEV_NAME_RES_EVT
     */
    struct name_res_param {
        esp_bt_status_t status;                /*!< Status of getting device name */
        char *name;                            /*!< Name of Bluetooth device */
    } name_res;                                /*!< discovery result parameter struct */
} esp_bt_dev_cb_param_t;

/**
 * @brief           bluetooth device callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter
 */
typedef void (* esp_bt_dev_cb_t)(esp_bt_dev_cb_event_t event, esp_bt_dev_cb_param_t *param);

/**
 * @brief           register callback function. This function should be called after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_dev_register_callback(esp_bt_dev_cb_t callback);

/**
 *
 * @brief      Get bluetooth device address.  Must use after "esp_bluedroid_enable".
 *
 * @return     bluetooth device address (six bytes), or NULL if bluetooth stack is not enabled
 */
const uint8_t *esp_bt_dev_get_address(void);


/**
 * @brief           Set bluetooth device name. This function should be called after esp_bluedroid_enable()
 *                  completes successfully.
 *
 *                  A BR/EDR/LE device type shall have a single Bluetooth device name which shall be
 *                  identical irrespective of the physical channel used to perform the name discovery procedure.
 *
 * @param[in]       name : device name to be set
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_ARG : if name is NULL pointer or empty, or string length out of limit
 *                  - ESP_ERR_INVALID_STATE : if bluetooth stack is not yet enabled
 *                  - ESP_FAIL : others
 */
esp_err_t esp_bt_dev_set_device_name(const char *name) __attribute__((deprecated("Please use esp_bt_gap_set_device_name or esp_ble_gap_set_device_name")));

/**
 * @brief           Get bluetooth device name. This function should be called after esp_bluedroid_enable()
 *                  completes successfully.
 *
 *                  A BR/EDR/LE device type shall have a single Bluetooth device name which shall be
 *                  identical irrespective of the physical channel used to perform the name discovery procedure.
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_STATE : if bluetooth stack is not yet enabled
 *                  - ESP_FAIL : others
 */
esp_err_t esp_bt_dev_get_device_name(void) __attribute__((deprecated("Please use esp_bt_gap_get_device_name or esp_ble_gap_get_device_name")));

/**
 * @brief           Config bluetooth device coexis status. This function should be called after esp_bluedroid_enable()
 *                  completes successfully.
 *
 * @param[in]       type   : coexist type to operate on
 * @param[in]       op     : clear or set coexist status
 * @param[in]       status : coexist status to be configured
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_ARG : if name is NULL pointer or empty, or string length out of limit
 *                  - ESP_ERR_INVALID_STATE : if bluetooth stack is not yet enabled
 *                  - ESP_FAIL : others
 */
esp_err_t esp_bt_dev_coex_status_config(esp_bt_dev_coex_type_t type, esp_bt_dev_coex_op_t op, uint8_t status);

/**
 * @brief           This function is used to update the path name of bluetooth bond keys saved in the NVS module
 *                  and need to be called before esp_bluedroid_init().
 * @param[in]       file_path: the name of config file path, the length of file_path should be less than NVS_NS_NAME_MAX_SIZE
 *
 * @return
 *                   - ESP_OK: success
 *                   - other: failed
 *
 */
esp_err_t esp_bt_config_file_path_update(const char *file_path);

#ifdef __cplusplus
}
#endif


#endif /* __ESP_BT_DEVICE_H__ */
