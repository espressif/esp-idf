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
 * @brief CSI customized data type configuration.
 *
 * @note By default (by setting this structure to 0), embedded data and standard data type will be supported.
 *       Under most conditions, you don't need to set this and can keep these to 0.
 *       Set this if you want to use customized data type.
 */
typedef struct {
    uint32_t bits_per_pixel;                                 /*!< Bits per pixel, by default it's 0, embedded data and standard data type will be supported */
    uint32_t data_type_min;                                  /*!< Data type, by default it's 0, embedded data and standard data type will be supported */
    uint32_t data_type_max;                                  /*!< Data type, by default it's 0, embedded data and standard data type will be supported */
} esp_cam_ctlr_csi_data_type_t;

/**
 * @brief ESP CAM CSI controller configurations.
 */
typedef struct {
    int ctlr_id;                                             /*!< CSI controller ID. */
    mipi_csi_phy_clock_source_t clk_src;                     /*!< CSI PHY clock source. */
    uint32_t h_res;                                          /*!< Input horizontal resolution. Measurement unit: pixels per line. */
    uint32_t v_res;                                          /*!< Input vertical resolution. Measurement unit: lines per frame. */
    uint8_t data_lane_num;                                   /*!< Number of data lanes. */
    int lane_bit_rate_mbps;                                  /*!< Lane bit rate. Measurement unit: Mbps. */
    cam_ctlr_color_t input_data_color_type;                  /*!< Input color type. */
    cam_ctlr_color_t output_data_color_type;                 /*!< Output color type. */
    esp_cam_ctlr_csi_data_type_t data_type;                  /*!< Data type configuration. */
    int queue_items;                                         /*!< Number of queue items. */
    struct {
        uint32_t input_8bit_swap_en    : 1;                  /*!< Set to 1 to enable input 8bit bit swap. [31:24] [23:16] [15:8] [7:0] -> [7:0] [15:8] [23:16] [31:24]*/
        uint32_t input_16bit_swap_en   : 1;                  /*!< Set to 1 to enable input 16bit bit swap. [31:16] [15:0] -> [15:0] [31:16] */
        uint32_t byte_swap_en          : 1;                  /*!< Set to 1 to enable output byte swap. */
        uint32_t bk_buffer_dis         : 1;                  /*!< Set to 1 to disable backup buffer. */
    };                                                       /*!< Boolean flags. */
} esp_cam_ctlr_csi_config_t;

/**
 * @brief Create a new ESP CAM CSI controller.
 *
 * @param[in]  config      CSI controller configurations.
 * @param[out] ret_handle  Returned ESP CAM controller handle.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_NO_MEM: Out of memory.
 *      - ESP_ERR_NOT_SUPPORTED: Currently unsupported modes or types.
 *      - ESP_ERR_NOT_FOUND: CSI is already registered.
 */
esp_err_t esp_cam_new_csi_ctlr(const esp_cam_ctlr_csi_config_t *config, esp_cam_ctlr_handle_t *ret_handle);

#ifdef __cplusplus
}
#endif
