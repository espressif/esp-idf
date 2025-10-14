/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "esp_openthread_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Registers the callback for RCP failure.
 *
 */
void esp_openthread_register_rcp_failure_handler(esp_openthread_rcp_failure_handler handler);

/**
 * @brief   Registers the callback for spinel compatibility error.
 *
 * @note This function should be called before esp_openthread_init.
 *
 * @param[in]  callback   The callback.
 *
 */
void esp_openthread_set_compatibility_error_callback(esp_openthread_compatibility_error_callback callback);

/**
 * @brief   Registers the callback for co-processor reset failure.
 *
 * @note This function should be called before esp_openthread_init.
 *
 * @param[in]  callback   The callback.
 *
 */
void esp_openthread_set_coprocessor_reset_failure_callback(esp_openthread_coprocessor_reset_failure_callback callback);

/**
 * @brief   Deinitializes the connection to RCP.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if fail to deinitialize RCP
 *
 */
esp_err_t esp_openthread_rcp_deinit(void);

/**
 * @brief   Initializes the connection to RCP.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if fail to initialize RCP
 *
 */
esp_err_t esp_openthread_rcp_init(void);

/**
 * @brief   Set the RCP version string.
 *
 * @note This function should be called before esp_openthread_init. When the RCP version string is not an empty string,
 *       compatibility checks will be performed during the initialization of the ESP OpenThread radio spinel.
 *       The `esp_openthread_compatibility_error_callback` will be triggered if the desired RCP version does not match
 *       the actual version running on the RCP. If needed, a NULL parameter can be passed to clear the version string.
 *
 * @param[in]  version_str  The pointer to RCP version string.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL if fail to set RCP version string
 *
 */
esp_err_t esp_openthread_rcp_version_set(const char *version_str);

#ifdef __cplusplus
}
#endif
