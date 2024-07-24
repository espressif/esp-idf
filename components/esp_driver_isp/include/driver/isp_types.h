/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
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

/*---------------------------------------------
                Event Callbacks
----------------------------------------------*/
/**
 * @brief Event data structure
 */
typedef struct {
    uint8_t high_freq_pixel_max;  ///< high freq pixel max value
} esp_isp_sharpen_evt_data_t;

/**
 * @brief Prototype of ISP sharpen event callback
 *
 * @param[in] proc      Processor handle
 * @param[in] edata     ISP sharpen event data
 * @param[in] user_data User registered context, registered when in `esp_isp_register_event_callbacks()`
 *
 * @return Whether a high priority task is woken up by this function
 */
typedef bool (*esp_isp_sharpen_callback_t)(isp_proc_handle_t proc, const esp_isp_sharpen_evt_data_t *edata, void *user_data);

/**
 * @brief Group of ISP event callbacks
 *
 * @note These callbacks are all running in an ISR environment.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables should be in internal RAM as well.
 */
typedef struct {
    esp_isp_sharpen_callback_t on_sharpen_frame_done;       ///< Event callback, invoked when sharpen frame done
} esp_isp_evt_cbs_t;

#ifdef __cplusplus
}
#endif
