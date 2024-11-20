/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief LSC Gain array
 */
typedef struct {
    isp_lsc_gain_t *gain_r;     ///< Gain for R channel
    isp_lsc_gain_t *gain_gr;    ///< Gain for GR channel
    isp_lsc_gain_t *gain_gb;    ///< Gain for GB channel
    isp_lsc_gain_t *gain_b;     ///< Gain for B channel
} esp_isp_lsc_gain_array_t;

/**
 * @brief ISP LSC configurations
 */
typedef struct {
    esp_isp_lsc_gain_array_t *gain_array;    ///< Gain array
} esp_isp_lsc_config_t;

/**
 * @brief Helper function to allocate gain array for LSC
 *
 * @param[in] isp_proc                     Processor handle
 * @param[in] gain_array                   Gain array to be allocated
 * @param[out] out_array_size_per_channel  Array size
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  Not allowed to be called under current state
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 *        - ESP_ERR_NO_MEM         Out of memory
 */
esp_err_t esp_isp_lsc_allocate_gain_array(isp_proc_handle_t isp_proc, esp_isp_lsc_gain_array_t *gain_array, size_t *out_array_size_per_channel);

/**
 * @brief ISP LSC configuration
 *
 * @note After calling this API, LSC doesn't take into effect until `esp_isp_lsc_enable` is called
 *
 * @param[in] isp_proc    Processor handle
 * @param[in] config      LSC configurations
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  Not allowed to be called under current state
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 *        - ESP_ERR_NOT_SUPPORTED  Not supported
 */
esp_err_t esp_isp_lsc_configure(isp_proc_handle_t isp_proc, const esp_isp_lsc_config_t *config);

/**
 * @brief Enable ISP LSC function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_lsc_enable(isp_proc_handle_t isp_proc);

/**
 * @brief Disable ISP LSC function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_lsc_disable(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
