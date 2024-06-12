/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_cam_ctlr_dvp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP CAM DVP initialzie clock and GPIO.
 *
 * @param ctlr_id CAM DVP controller ID
 * @param clk_src CAM DVP clock source
 * @param pin     CAM DVP pin configuration
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_init(int ctlr_id, cam_clock_source_t clk_src, const esp_cam_ctlr_dvp_pin_config_t *pin);

/**
 * @brief ESP CAM DVP output hardware clock
 *
 * @param ctlr_id   CAM DVP controller ID
 * @param clk_src   CAM DVP clock source
 * @param xclk_freq CAM DVP output clock frequency in HZ
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_output_clock(int ctlr_id, cam_clock_source_t clk_src, uint32_t xclk_freq);

/**
 * @brief ESP CAM DVP de-initialzie.
 *
 * @param ctlr_id DVP controller ID
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_deinit(int ctlr_id);

#ifdef __cplusplus
}
#endif
