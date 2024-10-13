/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "soc/gpio_num.h"
#include "hal/cam_types.h"
#include "esp_cam_ctlr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP CAM DVP pins configuration
 */
typedef struct esp_cam_ctlr_dvp_pin_config {
    cam_ctlr_data_width_t data_width;           /*!< Number of data lines */
    gpio_num_t data_io[CAM_DVP_DATA_SIG_NUM];   /*!< DVP data pin number */
    gpio_num_t vsync_io;                        /*!< DVP V-Sync pin number */
    gpio_num_t de_io;                           /*!< DVP DE pin number */
    gpio_num_t pclk_io;                         /*!< DVP PCLK input pin number, clock is from camera sensor */
    gpio_num_t xclk_io;                         /*!< DVP output clock pin number, if using external XTAL, set xclk_io = GPIO_NUM_NC */
} esp_cam_ctlr_dvp_pin_config_t;

/**
 * @brief ESP CAM DVP controller configurations
 */
typedef struct esp_cam_ctlr_dvp_config {
    int ctlr_id;                                /*!< DVP controller ID */
    cam_clock_source_t clk_src;                 /*!< DVP clock source */
    uint32_t h_res;                             /*!< Input horizontal resolution, i.e. the number of pixels in a line */
    uint32_t v_res;                             /*!< Input vertical resolution, i.e. the number of lines in a frame */
    cam_ctlr_color_t input_data_color_type;     /*!< Input pixel format */
    struct {
        uint32_t byte_swap_en : 1;              /*!< Enable byte swap */
        uint32_t bk_buffer_dis  : 1;            /*!< Disable backup buffer */
        uint32_t pin_dont_init : 1;             /*!< Don't initialize DVP pins if users have called "esp_cam_ctlr_dvp_init" before */
        uint32_t pic_format_jpeg : 1;           /*!< Input picture format is JPEG, if set this flag and "input_data_color_type" will be ignored */
    };                                          /*!< Boolean Flags */

    uint32_t dma_burst_size;                    /*!< DVP DMA burst transmission block size, set to 0 means to disable the data burst,
                                                     other value must be power of 2, e.g., 4/8/16/32/64/128 */

    const esp_cam_ctlr_dvp_pin_config_t *pin;   /*!< DVP pin configuration, this will be ignored by "esp_cam_new_dvp_ctlr" if "pin_dont_init" is set */
} esp_cam_ctlr_dvp_config_t;

/**
 * @brief New ESP CAM DVP controller
 *
 * @param config      DVP controller configurations
 * @param ret_handle  Returned CAM controller handle
 *
 * @return
 *        - ESP_OK on success
 *        - ESP_ERR_INVALID_ARG:   Invalid argument
 *        - ESP_ERR_NO_MEM:        Out of memory
 *        - ESP_ERR_NOT_SUPPORTED: Currently not support modes or types
 *        - ESP_ERR_NOT_FOUND:     CSI is registered already
 */
esp_err_t esp_cam_new_dvp_ctlr(const esp_cam_ctlr_dvp_config_t *config, esp_cam_ctlr_handle_t *ret_handle);

#ifdef __cplusplus
}
#endif
