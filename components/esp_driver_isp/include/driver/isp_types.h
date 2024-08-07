/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "hal/isp_types.h"
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ISP unsigned integer range type
 * @note  Whether the edge value are included depends on the variable itself
 */
typedef struct {
    uint32_t min;       ///< Minimum unsigned int value
    uint32_t max;       ///< Maximum unsigned int value
} isp_u32_range_t;

/**
 * @brief ISP float range type
 * @note  Whether the edge value are included depends on the variable itself
 */
typedef struct {
    float min;          ///< Minimum float value
    float max;          ///< Maximum float value
} isp_float_range_t;

/**
 * @brief ISP AF result
 */
typedef struct {
    int definition[ISP_AF_WINDOW_NUM];    ///< Definition, it refers how clear and sharp an image is
    int luminance[ISP_AF_WINDOW_NUM];     ///< Luminance, it refers how luminant an image is
} isp_af_result_t;

/**
 * @brief ISP AWB result
 */
typedef struct {
    uint32_t white_patch_num;   ///< white patch number that counted by AWB in the window
    uint32_t sum_r;             ///< The sum of R channel of these white patches
    uint32_t sum_g;             ///< The sum of G channel of these white patches
    uint32_t sum_b;             ///< The sum of B channel of these white patches
} isp_awb_stat_result_t;

/**
 * @brief ISP AE result
 */
typedef struct {
    int luminance[ISP_AE_BLOCK_X_NUM][ISP_AE_BLOCK_Y_NUM];     ///< Luminance, it refers how luminant an image is
} isp_ae_result_t;

/**
 * @brief Type of ISP processor handle
 */
typedef struct isp_processor_t *isp_proc_handle_t;

/**
 * @brief Type of ISP AF controller handle
 */
typedef struct isp_af_controller_t *isp_af_ctlr_t;

/**
 * @brief Type of ISP AWB controller handle
 */
typedef struct isp_awb_controller_t *isp_awb_ctlr_t;

/**
 * @brief Type of ISP AE controller handle
 */
typedef struct isp_ae_controller_t *isp_ae_ctlr_t;

#ifdef __cplusplus
}
#endif
