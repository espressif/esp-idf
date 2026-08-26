/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file example_awb.h
 * @brief Auto White Balance (AWB) Functionality
 *
 * This module implements ISP white balance functionality using hardware AWB controller
 * for statistics and WBG module for gain adjustment.
 *
 * Please note that there are two methods to achieve auto white balance tuning:
 * 1. use AWB module to get white balance statistics + tune camera sensor gain registers
 * 2. use AWB module to get white balance statistics + tune the output gain of ISP pipeline by WBG module
 *
 * This example uses method 2 to achieve auto white balance tuning.
 *
 * How it works:
 * - ISP hardware AWB controller detects white patches in the configured window
 * - White patches are identified based on luminance and color ratio thresholds
 * - AWB controller provides statistics (sum_r, sum_g, sum_b) through continuous mode
 * - Statistics are sent to a FreeRTOS task via queue
 * - The task calculates white balance gains based on statistics
 * - Gains are applied through the WBG (White Balance Gain) module
 *
 * @note Please note that `AWB` and `WBG` refers to 2 different modules in the ISP pipeline, while this example uses both of them to achieve auto white balance tuning.
 */

#pragma once

#include "esp_err.h"
#include "driver/isp.h"
#include "driver/isp_awb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize White Balance module
 *
 * This function creates and configures the AWB controller for statistics and WBG module
 * for gain adjustment. It uses default configuration for AWB controller and WBG module.
 *
 * @param[in] isp_proc ISP processor handle
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_NO_MEM: Out of memory
 */
esp_err_t example_isp_awb_init(isp_proc_handle_t isp_proc);

/**
 * @brief Enable White Balance module
 *
 * @param[in] isp_proc ISP processor handle
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_INVALID_STATE: Invalid state
 */
esp_err_t example_isp_awb_start(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
