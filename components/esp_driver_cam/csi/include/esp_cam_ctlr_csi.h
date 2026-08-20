/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
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
 * @brief ESP CAM CSI controller configurations
 */
typedef struct {
    int ctlr_id;                                ///< CSI controller ID
    mipi_csi_phy_clock_source_t clk_src;        ///< CSI phy clock source
    uint32_t h_res;                             ///< Input horizontal resolution, i.e. the number of pixels in a line
    uint32_t v_res;                             ///< Input vertical resolution, i.e. the number of lines in a frame
    uint8_t data_lane_num;                      ///< Data lane num
    int lane_bit_rate_mbps;                     ///< Lane bit rate in Mbps
    cam_ctlr_color_t input_data_color_type;     ///< Input color type
    cam_ctlr_color_t output_data_color_type;    ///< Output color type
    int queue_items;                            ///< Queue items
    struct {
        uint32_t input_8bit_swap_en    : 1;     /*!< Set to 1 to enable input 8bit bit swap. [31:24] [23:16] [15:8] [7:0] -> [7:0] [15:8] [23:16] [31:24]*/
        uint32_t input_16bit_swap_en   : 1;     /*!< Set to 1 to enable input 16bit bit swap. [31:16] [15:0] -> [15:0] [31:16] */
        uint32_t byte_swap_en          : 1;     /*!< Set to 1 to enable output byte swap. */
        uint32_t bk_buffer_dis         : 1;     /*!< Set to 1 to disable backup buffer. */
    };                                          /*!< Boolean flags. */
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
