/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                      Misc
---------------------------------------------------------------*/
#if SOC_ISP_SUPPORTED
typedef soc_periph_isp_clk_src_t    isp_clk_src_t;     ///< Clock source type of ISP
#else
typedef int                         isp_clk_src_t;     ///< Default type
#endif

/**
 * @brief ISP Input Source
 */
typedef enum {
    ISP_INPUT_DATA_SOURCE_CSI,       ///< Input data from CSI
    ISP_INPUT_DATA_SOURCE_DVP,       ///< Input data from DVP
    ISP_INPUT_DATA_SOURCE_DWGDMA,    ///< Input data from DW-GDMA
} isp_input_data_source_t;

/**
 * @brief ISP Color Type
 */
typedef enum {
    ISP_COLOR_RAW8   = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW8),   ///< RAW8
    ISP_COLOR_RAW10  = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW10),  ///< RAW10
    ISP_COLOR_RAW12  = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW12),  ///< RAW12
    ISP_COLOR_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888), ///< RGB888
    ISP_COLOR_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565), ///< RGB565
    ISP_COLOR_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422), ///< YUV422
    ISP_COLOR_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420), ///< YUV420
} isp_color_t;

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
/**
 * @brief ISP AF window
 */
typedef struct {
    uint32_t top_left_x;        ///< Top left x axis value
    uint32_t top_left_y;        ///< Top left y axis value
    uint32_t bottom_right_x;    ///< Bottom right x axis value
    uint32_t bottom_right_y;    ///< Bottom right y axis value
} isp_af_window_t;

/**
 * @brief ISP AF result
 */
typedef struct {
#if SOC_ISP_SUPPORTED
    int definition[SOC_ISP_AF_WINDOW_NUMS];    ///< Definition, it refers how clear and sharp an image is
    int luminance[SOC_ISP_AF_WINDOW_NUMS];     ///< Luminance, it refers how luminant an image is
#endif
} isp_af_result_t;

#ifdef __cplusplus
}
#endif
