/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "hal/mipi_csi_types.h"
#include "esp_cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP CAM controller max timeout value
 */
#define ESP_CAM_CTLR_MAX_DELAY UINT32_MAX

/**
 * @brief ESP CAM CSI controller configurations
 */
typedef struct {
    int ctlr_id;                                ///< CSI controller ID
    mipi_csi_phy_clock_source_t clk_src;        ///< CSI phy clock source
    uint32_t h_res;                             ///< Input horizontal resolution, i.e. the number of pixels in a line
    uint32_t v_res;                             ///< Input vertical resolution, i.e. the number of lines in a frame
    uint8_t data_lane_num;                      ///< Data lane num
    int clk_freq_hz;                            ///< Frequency of CLK, in Hz.
    mipi_csi_color_t input_data_color_type;     ///< Input color type
    mipi_csi_color_t output_data_color_type;    ///< Output color type
    bool byte_swap_en;                          ///< Enable byte swap
    int queue_items;                            ///< Queue itmes
} esp_cam_ctlr_csi_config_t;

/**
 * @brief New ESP CAM CSI controller
 *
 * @param[in]  config      CSI controller configurations
 * @param[out] ret_handle  Returned ESP CAM controller handle
 *
 * @return
 *        - ESP_OK
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_NO_MEM:        Out of memory
 *        - ESP_ERR_NOT_SUPPORTED: Currently not support modes or types
 *        - ESP_ERR_NOT_FOUND:     CSI is registered already
 */
esp_err_t esp_cam_new_csi_ctlr(const esp_cam_ctlr_csi_config_t *config, esp_cam_ctlr_handle_t *ret_handle);

#ifdef __cplusplus
}
#endif
