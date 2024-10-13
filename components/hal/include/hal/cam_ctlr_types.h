/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief Camera Controller Color Type
 */
typedef enum {
    CAM_CTLR_COLOR_RAW8   = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW8),   ///< RAW8
    CAM_CTLR_COLOR_RAW10  = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW10),  ///< RAW10
    CAM_CTLR_COLOR_RAW12  = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW12),  ///< RAW12
    CAM_CTLR_COLOR_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565), ///< RGB565
    CAM_CTLR_COLOR_RGB666 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB666), ///< RGB666
    CAM_CTLR_COLOR_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888), ///< RGB888
    CAM_CTLR_COLOR_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420), ///< YUV420
    CAM_CTLR_COLOR_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422), ///< YUV422
    CAM_CTLR_COLOR_GRAY4  = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY4), ///< GRAY4
    CAM_CTLR_COLOR_GRAY8  = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8), ///< GRAY8
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

#ifdef __cplusplus
}
#endif
