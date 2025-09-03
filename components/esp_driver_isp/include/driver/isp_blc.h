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

/*---------------------------------------------------------------
                      BLC (Black Level Correction)
---------------------------------------------------------------*/
/**
 * @brief ISP BLC threshold configurations
 */
typedef struct {
    uint8_t top_left_chan_thresh;           ///< Black level threshold for top left channel of the raw Bayer image
    uint8_t top_right_chan_thresh;          ///< Black level threshold for top right channel of the raw Bayer image
    uint8_t bottom_left_chan_thresh;        ///< Black level threshold for bottom left channel of the raw Bayer image
    uint8_t bottom_right_chan_thresh;       ///< Black level threshold for bottom right channel of the raw Bayer image
} esp_isp_blc_thresh_t;

/**
 * @brief ISP BLC stretch configurations
 *
 * Enable this can stretch the pixel value to 0~255 after black level correction
 */
typedef struct {
    bool top_left_chan_stretch_en;          ///< Enable stretch for top left channel of the raw Bayer image
    bool top_right_chan_stretch_en;         ///< Enable stretch for top right channel of the raw Bayer image
    bool bottom_left_chan_stretch_en;       ///< Enable stretch for bottom left channel of the raw Bayer image
    bool bottom_right_chan_stretch_en;      ///< Enable stretch for bottom right channel of the raw Bayer image
} esp_isp_blc_stretch_t;

/**
 * @brief ISP BLC configurations
 */
typedef struct {
    isp_window_t          window;           ///< The sampling windows of BLC, only pixels within the window will be sampled
    esp_isp_blc_thresh_t  filter_threshold; ///< Black level threshold for each channel of the raw Bayer image
    bool                  filter_enable;    ///< Enable filter for BLC, if enabled, only pixels within the threshold will be sampled
    esp_isp_blc_stretch_t stretch;          ///< Stretch configurations for each channel of the raw Bayer image
} esp_isp_blc_config_t;

/**
 * @brief ISP BLC correction offset
 */
typedef struct {
    uint32_t top_left_chan_offset;          ///< Correction offset for top left channel of the raw Bayer image
    uint32_t top_right_chan_offset;         ///< Correction offset for top right channel of the raw Bayer image
    uint32_t bottom_left_chan_offset;       ///< Correction offset for bottom left channel of the raw Bayer image
    uint32_t bottom_right_chan_offset;      ///< Correction offset for bottom right channel of the raw Bayer image
} esp_isp_blc_offset_t;

/**
 * @brief ISP BLC configuration
 *
 * @note After calling this API, BLC doesn't take into effect until `esp_isp_blc_enable` is called
 *
 * @param[in] isp_proc    Processor handle
 * @param[in] config      BLC configurations
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  Not allowed to be called under current state
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 *        - ESP_ERR_NOT_SUPPORTED  Not supported
 */
esp_err_t esp_isp_blc_configure(isp_proc_handle_t isp_proc, const esp_isp_blc_config_t *config);

/**
 * @brief Enable ISP BLC function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_blc_enable(isp_proc_handle_t isp_proc);

/**
 * @brief Set the correction offset of ISP BLC function
 *
 * @param[in] isp_proc  Processor handle
 * @param[in] offset    Correction offset
 */
esp_err_t esp_isp_blc_set_correction_offset(isp_proc_handle_t isp_proc, esp_isp_blc_offset_t *offset);

/**
 * @brief Disable ISP BLC function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_blc_disable(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
