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
 * @brief ESP CAM CSI controller configurations.
 */
typedef struct {
    int ctlr_id;                                /*!< CSI controller ID. */
    mipi_csi_phy_clock_source_t clk_src;        /*!< CSI PHY clock source. */
    uint32_t h_res;                             /*!< Input horizontal resolution. Measurement unit: pixels per line. */
    uint32_t v_res;                             /*!< Input vertical resolution. Measurement unit: lines per frame. */
    uint8_t data_lane_num;                      /*!< Number of data lanes. */
    int lane_bit_rate_mbps;                     /*!< Lane bit rate. Measurement unit: Mbps. */
    cam_ctlr_color_t input_data_color_type;     /*!< Input color type. */
    cam_ctlr_color_t output_data_color_type;    /*!< Output color type. */
    int queue_items;                            /*!< Number of queue items. */
    struct {
        uint32_t byte_swap_en   : 1;            /*!< Set to 1 to enable byte swap. */
        uint32_t bk_buffer_dis  : 1;            /*!< Set to 1 to disable backup buffer. */
    };                                          /*!< Boolean flags. */
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
