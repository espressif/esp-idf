/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file example_dpc.h
 * @brief Dead Pixel Correction (DPC) Functionality
 */

#pragma once

#include "esp_err.h"
#include "driver/isp.h"
#include "driver/isp_dpc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize DPC module
 *
 * @param[in] isp_proc ISP processor handle
 * @param[in] calibration_ref Static calibration coordinate reference, or NULL to skip static correction
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_INVALID_STATE: DPC is already enabled
 *         - ESP_ERR_NOT_SUPPORTED: Not supported on this chip revision
 */
esp_err_t example_isp_dpc_init(isp_proc_handle_t isp_proc, const esp_isp_dpc_calibration_ref_t *calibration_ref);

/**
 * @brief Calibrate static DPC using the active CSI ISP processor
 *
 * The function prompts the user to present uniform white and black images to the
 * camera and press Enter after each image is ready, then captures one frame of
 * each to produce a merged calibration result.
 * DPC must be disabled before calling this function.
 *
 * @param[in] isp_proc           ISP processor handle receiving CSI input
 * @param[out] calibration_ref   Merged static calibration coordinate reference
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_TIMEOUT: Calibration frame was not received in time
 */
esp_err_t example_isp_dpc_calibrate_static(isp_proc_handle_t isp_proc, esp_isp_dpc_calibration_ref_t *calibration_ref);

/**
 * @brief Enable DPC module
 *
 * @param[in] isp_proc ISP processor handle
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t example_isp_dpc_enable(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
