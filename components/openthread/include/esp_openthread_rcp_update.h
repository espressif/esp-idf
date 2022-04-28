/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_openthread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function triggers an RCP firmware update.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
esp_err_t esp_openthread_rcp_update_start(void);

/**
 * @brief This function writes sequential firmware update data to the RCP.
 *
 * @param[in] data  The firmware data slice
 * @param[in] size  The data slice size
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
esp_err_t esp_openthread_rcp_update_write(uint8_t *data, size_t size);

/**
 * @brief This function commits the firmware update and reboots the RCP.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *
 */
esp_err_t esp_openthread_rcp_update_commit(void);

#ifdef __cplusplus
}
#endif
