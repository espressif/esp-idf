/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file example_af.h
 * @brief Auto Focus (AF) Module
 *
 * This module implements ISP auto focus functionality using hardware AF controller
 * and software focus algorithm.
 *
 * How it works:
 * - ISP hardware statistics luminance and definition (sharpness) in specified AF windows
 * - Environment detector triggers callback when environment changes are detected
 * - Focus algorithm adjusts lens focus based on definition values
 * - Uses Step-and-Approach (SA) search algorithm to find optimal focus point
 *
 * This example uses DW9714 VCM (Voice Coil Motor) to drive lens focus.
 */

#pragma once

#include "esp_err.h"
#include "driver/isp.h"
#include "esp_sccb_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AF task parameters
 */
typedef struct {
    isp_proc_handle_t isp_proc;           // ISP processor handle
    esp_sccb_io_handle_t dw9714_io_handle; // DW9714 VCM SCCB handle
} example_isp_af_task_param_t;

/**
 * @brief AF configuration structure
 */
typedef struct {
    int window_size;          // AF window size (centered window)
    int edge_thresh;          // Edge threshold for definition calculation
    int env_interval;         // Environment detector interval (frames)
    int first_step_val;       // First step value for SA scheme
    int first_approx_cycles;  // First approximation cycles
    int second_step_val;      // Second step value for SA scheme
    int second_approx_cycles; // Second approximation cycles
    int focus_val_max;        // Maximum focus value
} example_isp_af_config_t;

/**
 * @brief Initialize AF module
 *
 * This function creates and configures the AF controller, environment detector,
 * and SA (Step-and-Approach) scheme.
 *
 * @param[in] isp_proc ISP processor handle
 * @param[in] dw9714_io_handle DW9714 VCM SCCB handle
 * @param[in] config AF configuration (can be NULL for default values)
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 *         - ESP_ERR_NO_MEM: Out of memory
 */
esp_err_t example_isp_af_init(isp_proc_handle_t isp_proc,
                              esp_sccb_io_handle_t dw9714_io_handle,
                              const example_isp_af_config_t *config);

/**
 * @brief Start AF task
 *
 * This function creates a FreeRTOS task to handle AF processing.
 * The task will continuously monitor focus and adjust when environment changes.
 *
 * @param[in] task_priority Task priority
 * @param[in] core_id Core ID to pin the task
 * @return
 *         - ESP_OK: Success
 *         - ESP_ERR_INVALID_ARG: Invalid arguments
 */
esp_err_t example_isp_af_start(int task_priority, int core_id);

#ifdef __cplusplus
}
#endif
