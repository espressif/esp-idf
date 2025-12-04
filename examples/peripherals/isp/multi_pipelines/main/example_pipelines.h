/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file example_pipeline.h
 * @brief ISP Pipeline Processing Module
 *
 * This module implements configuration for various ISP image processing pipeline units, including:
 * - BLC (Black Level Correction): Black level correction
 * - BF (Bayer Filter): Denoising processing
 * - LSC (Lens Shading Correction): Lens shading correction
 * - DEMOSAIC: Demosaicing, converting RAW data to RGB
 * - CCM (Color Correction Matrix): Color correction matrix
 * - GAMMA: Gamma correction
 * - SHARPEN: Sharpening processing
 * - COLOR: Color adjustment (contrast, saturation, hue, brightness)
 * - CROP: Image cropping
 *
 * These functions process image data sequentially according to the ISP hardware pipeline order.
 */

#pragma once

#include "esp_err.h"
#include "driver/isp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ISP processor
 *
 * @param[in] config ISP processor configuration
 * @param[out] isp_proc Output ISP processor handle
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_NO_MEM: Out of memory
 */
esp_err_t example_create_isp_processor(const esp_isp_processor_cfg_t *config, isp_proc_handle_t *isp_proc);

/**
 * @brief Initialize all ISP pipeline modules with default configuration
 *
 * This function configures all available ISP modules in the correct order.
 *
 * @param[in] isp_proc ISP processor handle
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t example_isp_init_all_pipelines(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
