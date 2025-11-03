/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/isp_types.h"
#include "hal/cam_ctlr_types.h"
#include "esp_cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ESP CAM ISP DVP controller configurations.
 */
typedef struct {
    cam_ctlr_data_width_t data_width;             /*!< Number of data lines. */
    int data_io[ISP_DVP_DATA_SIG_NUM];            /*!< ISP DVP data-in IO numbers. */
    int pclk_io;                                  /*!< ISP DVP pclk IO number. */
    int hsync_io;                                 /*!< ISP DVP hsync IO number. */
    int vsync_io;                                 /*!< ISP DVP vsync IO number. */
    int de_io;                                    /*!< ISP DVP de IO number. */
    struct {
        uint32_t pclk_invert: 1;                  /*!< Set to 1 to invert the pclk signal. */
        uint32_t hsync_invert: 1;                 /*!< Set to 1 to invert the hsync signal (i.e., active low). */
        uint32_t vsync_invert: 1;                 /*!< Set to 1 to invert the vsync signal (i.e., active high). */
        uint32_t de_invert: 1;                    /*!< Set to 1 to invert the de signal (i.e., active low). */
    } io_flags;                                   /*!< ISP DVP IO flags. */
    int queue_items;                              /*!< Queue items. */
    struct {
        uint32_t byte_swap_en   : 1;              /*!< Set to 1 to enable byte swap. */
        uint32_t bk_buffer_dis  : 1;              /*!< Set to 1 to disable backup buffer. */
    };
} esp_cam_ctlr_isp_dvp_cfg_t;

/**
 * @brief Create a new ESP CAM ISP DVP controller.
 *
 * @param[in]  isp_proc     Processor handle.
 * @param[in]  ctlr_config  ISP DVP controller configurations.
 * @param[out] ret_handle   Returned ESP CAM controller handle.
 *
 * @return
 *      - ESP_OK: Success.
 *      - ESP_ERR_INVALID_ARG: Invalid argument.
 *      - ESP_ERR_NO_MEM: Out of memory.
 *      - ESP_ERR_NOT_SUPPORTED: Currently not supported modes or types.
 *      - ESP_ERR_NOT_FOUND: ISP DVP is already registered.
 */
esp_err_t esp_cam_new_isp_dvp_ctlr(isp_proc_handle_t isp_proc, const esp_cam_ctlr_isp_dvp_cfg_t *ctlr_config, esp_cam_ctlr_handle_t *ret_handle);

#ifdef __cplusplus
}
#endif
