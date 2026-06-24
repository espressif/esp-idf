/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Camera Controller Color Type, in FourCC format
 */
typedef enum {
    CAM_CTLR_COLOR_RAW8   = ESP_COLOR_FOURCC_RAW8,       ///< RAW8
    CAM_CTLR_COLOR_RAW10  = ESP_COLOR_FOURCC_RAW10,     ///< RAW10
    CAM_CTLR_COLOR_RAW12  = ESP_COLOR_FOURCC_RAW12,     ///< RAW12
    CAM_CTLR_COLOR_RGB565 = ESP_COLOR_FOURCC_RGB16,     ///< RGB565
    CAM_CTLR_COLOR_RGB888 = ESP_COLOR_FOURCC_RGB24,     ///< RGB888
    CAM_CTLR_COLOR_YUV420 = ESP_COLOR_FOURCC_OUYY_EVYY, ///< YUV420
    CAM_CTLR_COLOR_YUV422_YVYU = ESP_COLOR_FOURCC_YVYU, ///< YUV422, YVYU
    CAM_CTLR_COLOR_YUV422_YUYV = ESP_COLOR_FOURCC_YUYV, ///< YUV422, YUYV
    CAM_CTLR_COLOR_YUV422_UYVY = ESP_COLOR_FOURCC_UYVY, ///< YUV422, UYVY
    CAM_CTLR_COLOR_YUV422_VYUY = ESP_COLOR_FOURCC_VYUY, ///< YUV422, VYUY
    CAM_CTLR_COLOR_GRAY8  = ESP_COLOR_FOURCC_GREY,      ///< GREY8
} cam_ctlr_color_t;

/**
 * @brief Camera Controller Data Width
 */
typedef enum {
    CAM_CTLR_DATA_WIDTH_8 = 8,      ///< 8-bit data width
    CAM_CTLR_DATA_WIDTH_10 = 10,    ///< 10-bit data width
    CAM_CTLR_DATA_WIDTH_12 = 12,    ////< 12-bit data width
    CAM_CTLR_DATA_WIDTH_16 = 16,    ///< 16-bit data width
} cam_ctlr_data_width_t;

/**
 * @brief Camera Controller format conversion configuration
 */
typedef struct {
    cam_ctlr_color_t src_format;           /*!< Source format */
    cam_ctlr_color_t dst_format;           /*!< Destination format */
    color_conv_std_rgb_yuv_t conv_std;     /*!< Conversion standard */
    uint32_t data_width;                   /*!< Data width in bits */
    color_range_t input_range;             /*!< Input color range */
    color_range_t output_range;            /*!< Output color range */
} cam_ctlr_format_conv_config_t;

#ifdef __cplusplus
}
#endif
