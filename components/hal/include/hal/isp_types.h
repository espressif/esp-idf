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

/**
 * @brief ISP color range
 */
typedef enum {
    ISP_COLOR_RANGE_LIMIT = COLOR_RANGE_LIMIT, /*!< Limited color range */
    ISP_COLOR_RANGE_FULL = COLOR_RANGE_FULL,   /*!< Full color range */
} isp_color_range_t;

/**
 * @brief The standard used for conversion between RGB and YUV
 */
typedef enum {
    ISP_YUV_CONV_STD_BT601 = COLOR_CONV_STD_RGB_YUV_BT601, /*!< YUV<->RGB conversion standard: BT.601 */
    ISP_YUV_CONV_STD_BT709 = COLOR_CONV_STD_RGB_YUV_BT709, /*!< YUV<->RGB conversion standard: BT.709 */
} isp_yuv_conv_std_t;

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

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/
#if SOC_ISP_AF_WINDOW_NUMS
#define ISP_AF_WINDOW_NUM   SOC_ISP_AF_WINDOW_NUMS      // The AF window number for sampling
#else
#define ISP_AF_WINDOW_NUM   0
#endif

/*---------------------------------------------------------------
                      AWB
---------------------------------------------------------------*/

/**
 * @brief ISP AWB sample point in the ISP pipeline
 */
typedef enum {
    ISP_AWB_SAMPLE_POINT_BEFORE_CCM,       ///< Sample AWB data before CCM (Color Correction Matrix)
    ISP_AWB_SAMPLE_POINT_AFTER_CCM,        ///< Sample AWB data after CCM (Color Correction Matrix)
} isp_awb_sample_point_t;

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
                      DVP
---------------------------------------------------------------*/
#if SOC_ISP_DVP_DATA_WIDTH_MAX
#define ISP_DVP_DATA_SIG_NUM   SOC_ISP_DVP_DATA_WIDTH_MAX // The ISP DVP data signal number
#else
#define ISP_DVP_DATA_SIG_NUM   0
#endif

/*---------------------------------------------------------------
                      Sharpen
---------------------------------------------------------------*/
#if SOC_ISP_SHARPEN_SUPPORTED
#define ISP_SHARPEN_TEMPLATE_X_NUMS         SOC_ISP_SHARPEN_TEMPLATE_X_NUMS         // Sharpen template x field nums
#define ISP_SHARPEN_TEMPLATE_Y_NUMS         SOC_ISP_SHARPEN_TEMPLATE_Y_NUMS         // Sharpen template y field nums
#define ISP_SHARPEN_H_FREQ_COEF_INT_BITS    SOC_ISP_SHARPEN_H_FREQ_COEF_INT_BITS
#define ISP_SHARPEN_H_FREQ_COEF_DEC_BITS    SOC_ISP_SHARPEN_H_FREQ_COEF_DEC_BITS
#define ISP_SHARPEN_H_FREQ_COEF_RES_BITS    SOC_ISP_SHARPEN_H_FREQ_COEF_RES_BITS
#define ISP_SHARPEN_M_FREQ_COEF_INT_BITS    SOC_ISP_SHARPEN_M_FREQ_COEF_INT_BITS
#define ISP_SHARPEN_M_FREQ_COEF_DEC_BITS    SOC_ISP_SHARPEN_M_FREQ_COEF_DEC_BITS
#define ISP_SHARPEN_M_FREQ_COEF_RES_BITS    SOC_ISP_SHARPEN_M_FREQ_COEF_RES_BITS
#else
#define ISP_SHARPEN_TEMPLATE_X_NUMS         0
#define ISP_SHARPEN_TEMPLATE_Y_NUMS         0
#define ISP_SHARPEN_H_FREQ_COEF_INT_BITS    8
#define ISP_SHARPEN_H_FREQ_COEF_DEC_BITS    8
#define ISP_SHARPEN_H_FREQ_COEF_RES_BITS    16
#define ISP_SHARPEN_M_FREQ_COEF_INT_BITS    8
#define ISP_SHARPEN_M_FREQ_COEF_DEC_BITS    8
#define ISP_SHARPEN_M_FREQ_COEF_RES_BITS    16
#endif


/**
 * @brief High freq pixel sharpeness coeff
 */
typedef union {
    struct {
        uint32_t decimal:ISP_SHARPEN_H_FREQ_COEF_DEC_BITS;    ///< Integer part
        uint32_t integer:ISP_SHARPEN_H_FREQ_COEF_INT_BITS;    ///< Decimal part
        uint32_t reserved:ISP_SHARPEN_H_FREQ_COEF_RES_BITS;   ///< Reserved
    };
    uint32_t val;                                             ///< 32-bit high freq pixel sharpeness coeff register value
} isp_sharpen_h_freq_coeff;

/**
 * @brief Medium freq pixel sharpeness coeff
 */
typedef union {
    struct {
        uint32_t decimal:ISP_SHARPEN_M_FREQ_COEF_DEC_BITS;    ///< Integer part
        uint32_t integer:ISP_SHARPEN_M_FREQ_COEF_INT_BITS;    ///< Decimal part
        uint32_t reserved:ISP_SHARPEN_M_FREQ_COEF_RES_BITS;   ///< Reserved
    };
    uint32_t val;                                             ///< 32-bit medium freq pixel sharpeness coeff register value
} isp_sharpen_m_freq_coeff;

/**
 * @brief ISP Sharpen edge padding mode
 */
typedef enum {
    ISP_SHARPEN_EDGE_PADDING_MODE_SRND_DATA,      ///< Fill Sharpen edge padding data with surrounding pixel data
    ISP_SHARPEN_EDGE_PADDING_MODE_CUSTOM_DATA,    ///< Fill Sharpen edge padding data with custom pixel data
} isp_sharpen_edge_padding_mode_t;

/*---------------------------------------------------------------
                      Gamma Correction
---------------------------------------------------------------*/
#define ISP_GAMMA_CURVE_POINTS_NUM      16      ///< Number of points to define a gamma correction curve

/**
 * @brief Structure that declares the points on an ISP gamma curve
 *
 * Constraint on pt[n].x:
 *  When n = 0, pt[n].x = 2 ^ a[n]
 *  When 0 < n < ISP_GAMMA_CURVE_POINTS_NUM-1, pt[n].x - pt[n-1].x = 2 ^ a[n]
 *  When n = ISP_GAMMA_CURVE_POINTS_NUM-1, pt[n].x = 255, (pt[n].x + 1) - pt[n-1].x = 2 ^ a[n]
 *  a[n] within [0, 7]
 */
typedef struct {
    struct {
        uint8_t x;                      ///< Raw value (0, 255]
        uint8_t y;                      ///< gamma-corrected value (0, 255]
    } pt[ISP_GAMMA_CURVE_POINTS_NUM];   ///< Point (x, y)
} isp_gamma_curve_points_t;

#ifdef __cplusplus
}
#endif
