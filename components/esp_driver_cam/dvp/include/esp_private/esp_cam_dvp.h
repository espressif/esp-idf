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
 * @brief ESP CAM DVP output hardware clock, the function "esp_cam_ctlr_dvp_init" should be called first
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
 * @brief Initialize ESP CAM DVP clock pin (other DVP GPIO pins excluded the clock pins will not be initialized)
 *        and hardware clock, then output clock signal with given frequency
 *
 * @note The function is implemented based on "esp_cam_ctlr_dvp_init" and "esp_cam_ctlr_dvp_output_clock",
 *       so calling "esp_cam_ctlr_dvp_init" and "esp_cam_ctlr_dvp_output_clock" is not required
 *
 * @param ctlr_id DVP controller ID
 * @param pin     DVP clock pin
 * @param clk_src DVP clock source
 * @param xclk_freq DVP clock frequency
 *
 * @return
 *      - ESP_OK on success
 *      - Others if failed
 */
esp_err_t esp_cam_ctlr_dvp_start_clock(int ctlr_id, gpio_num_t pin, cam_clock_source_t clk_src, uint32_t xclk_freq);

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
