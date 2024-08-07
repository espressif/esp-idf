/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief ISP coordinate type
 *
 */
typedef struct {
    uint32_t    x;      ///< X coordinate of the point
    uint32_t    y;      ///< Y coordinate of the point
} isp_coordinate_t;

/**
 * @brief ISP window type
 *
 */
typedef struct {
    isp_coordinate_t top_left;       ///< The top left point coordinate
    isp_coordinate_t btm_right;      ///< The bottom right point coordinate
} isp_window_t;

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
                      DVP
---------------------------------------------------------------*/
#if SOC_ISP_DVP_DATA_WIDTH_MAX
#define ISP_DVP_DATA_SIG_NUM   SOC_ISP_DVP_DATA_WIDTH_MAX // The ISP DVP data signal number
#else
#define ISP_DVP_DATA_SIG_NUM   0
#endif

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
#if SOC_ISP_AF_WINDOW_NUMS
#define ISP_AF_WINDOW_NUM   SOC_ISP_AF_WINDOW_NUMS      // The AF window number for sampling
#else
#define ISP_AF_WINDOW_NUM   0
#endif

/*---------------------------------------------------------------
                      BF
---------------------------------------------------------------*/
#if SOC_ISP_BF_SUPPORTED
#define ISP_BF_TEMPLATE_X_NUMS    SOC_ISP_BF_TEMPLATE_X_NUMS    // BF template x field nums
#define ISP_BF_TEMPLATE_Y_NUMS    SOC_ISP_BF_TEMPLATE_Y_NUMS    // BF template y field nums
#else
#define ISP_BF_TEMPLATE_X_NUMS    0
#define ISP_BF_TEMPLATE_Y_NUMS    0
#endif

/**
 * @brief ISP BF edge padding mode
 */
typedef enum {
    ISP_BF_EDGE_PADDING_MODE_SRND_DATA,      ///< Fill BF edge padding data with surrounding pixel data
    ISP_BF_EDGE_PADDING_MODE_CUSTOM_DATA,    ///< Fill BF edge padding data with custom pixel data
} isp_bf_edge_padding_mode_t;

/*---------------------------------------------------------------
                      CCM
---------------------------------------------------------------*/
#if SOC_ISP_CCM_SUPPORTED
#define ISP_CCM_DIMENSION   SOC_ISP_CCM_DIMENSION  ///< ISP Color Correction Matrix dimension
#else
#define ISP_CCM_DIMENSION   0                      ///< Not support CCM
#endif

/*---------------------------------------------------------------
                      AWB
---------------------------------------------------------------*/

/**
 * @brief ISP AWB sample point in the ISP pipeline
 *
 */
typedef enum {
    ISP_AWB_SAMPLE_POINT_BEFORE_CCM,       ///< Sample AWB data before CCM (Color Correction Matrix)
    ISP_AWB_SAMPLE_POINT_AFTER_CCM,        ///< Sample AWB data after CCM (Color Correction Matrix)
} isp_awb_sample_point_t;

/*---------------------------------------------------------------
                      AE
---------------------------------------------------------------*/

#if (SOC_ISP_AE_BLOCK_X_NUMS && SOC_ISP_AE_BLOCK_Y_NUMS)
#define ISP_AE_BLOCK_X_NUM   SOC_ISP_AE_BLOCK_X_NUMS      // The AF window number for sampling
#define ISP_AE_BLOCK_Y_NUM   SOC_ISP_AE_BLOCK_Y_NUMS      // The AF window number for sampling
#else
#define ISP_AE_BLOCK_X_NUM   0
#define ISP_AE_BLOCK_Y_NUM   0
#endif

/**
 * @brief ISP AE input data source
 *
 */
typedef enum {
    ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC,          ///< AE input data after demosaic
    ISP_AE_SAMPLE_POINT_AFTER_GAMMA,             ///< AE input data after gamma
} isp_ae_sample_point_t;

#ifdef __cplusplus
}
#endif
