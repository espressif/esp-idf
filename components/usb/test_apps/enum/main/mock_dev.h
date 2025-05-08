/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

//
// TinyUSB helpers to mock the device
//

typedef enum {
    MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC = 0x00,
    MOCK_DEV_STAGE_REQ_FULL_DEV_DESC,
    MOCK_DEV_STAGE_REQ_SHORT_CONFIG_DESC,
    MOCK_DEV_STAGE_REQ_FULL_CONFIG_DESC,
    MOCK_DEV_STAGE_REQ_SHORT_LANGID_TABLE,
    MOCK_DEV_STAGE_REQ_FULL_LANGID_TABLE,
    MOCK_DEV_STAGE_REQ_SHORT_MANU_STR_DESC,
    MOCK_DEV_STAGE_REQ_FULL_MANU_STR_DESC,
    MOCK_DEV_STAGE_REQ_SHORT_PROD_STR_DESC,
    MOCK_DEV_STAGE_REQ_FULL_PROD_STR_DESC,
    MOCK_DEV_STAGE_REQ_SHORT_SER_STR_DESC,
    MOCK_DEV_STAGE_REQ_FULL_SER_STR_DESC,
    MOCK_DEV_STAGE_SET_CONFIG,
    MOCK_DEV_STAGE_MAX
} mock_dev_stage_t;

typedef struct {
    union {
        const uint8_t *desc;
        const char *str_desc;
    };
    bool stop_test;
} mock_dev_stage_data_t;

typedef struct {
    // connection/disconnection emulation
    struct {
        bool enabled;
        uint32_t delay_ms;
    } conn_dconn;
    // enumeration logic
    struct {
        mock_dev_stage_t curr_stage;
        mock_dev_stage_data_t stage_config[MOCK_DEV_STAGE_MAX];
    } enumeration;
} mock_dev_cfg_t;

/**
 * @brief Setup the Mock device
 *
 * @param[in] mock_dev_cfg  Mock device configuration
 *
 * @note During the setup procedure:
 * - Create semaphore
 * - Init TinyUSB device stack
 * - Create task for device stack handling
 * - Configure the Mock device
 * - Set current stage to MOCK_DEV_STAGE_REQ_SHORT_DEV_DESC
 * - Insert Mock device (Emulate physical connection, if conn_dconn parameter was enabled during configuration)
 *
 * @return
 *    - true if mock device configured successfully and ready
 *    - false if mock device hasn't been configured
 */
bool mock_dev_setup(mock_dev_cfg_t *mock_dev_cfg);

/**
 * @brief Wait the stop flag
 *
 * @note The stop flag is the semaphore mock_device_test_stop, which is triggered by:
 * - Stop flag configuration parameter for mock device
 * - tud_mount_cb(void)
 *
 * @return Error code value
 *    - ESP_ERR_INVALID_STATE if mock device hasn't been started
 *    - ESP_ERR_TIMEOUT if timeout has occurred
 *    - ESP_OK if there was no error during test stop
 */
esp_err_t mock_dev_wait_flag(void);

/**
 * @brief Release the Mock device
 *
 * @note During the release process:
 * - Short delay to allow device to show up
 * - Remove mock USB device (Emulate physical disconnection, if conn_dconn parameter was enabled during configuration)
 * - Uninstall TinyUSB driver
 * - Delete device handling task
 * - Delete semaphore
 * - Flush Mock device configuration
 */
void mock_dev_release(void);
