/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/isp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ISP crop configurations
 */
typedef struct {
    isp_window_t window;    /*!< Crop window coordinates */
} esp_isp_crop_config_t;

/**
 * @brief ISP Crop configuration
 *
 * @note After calling this API, crop doesn't take into effect until `esp_isp_crop_enable` is called
 *
 * @param[in] isp_proc  Processor handle
 * @param[in] config    Crop configurations, set NULL to de-configure the ISP Crop
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_crop_configure(isp_proc_handle_t isp_proc, const esp_isp_crop_config_t *config);

/**
 * @brief Enable ISP crop function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_crop_enable(isp_proc_handle_t isp_proc);

/**
 * @brief Disable ISP crop function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_crop_disable(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
