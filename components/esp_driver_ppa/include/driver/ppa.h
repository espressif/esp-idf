/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "hal/ppa_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of PPA engine handle
 */
typedef struct ppa_engine_t *ppa_engine_handle_t;

typedef struct {
    ppa_engine_type_t engine;
} ppa_engine_config_t;

esp_err_t ppa_engine_acquire(const ppa_engine_config_t *config, ppa_engine_handle_t *ret_engine);

esp_err_t ppa_engine_release(ppa_engine_handle_t ppa_engine);

typedef enum {
    PPA_TRANS_MODE_BLOCKING,
    PPA_TRANS_MODE_NON_BLOCKING,
} ppa_trans_mode_t;

typedef struct {
    void *in_buffer; // TODO: could be a buffer list, link descriptors together, process a batch
    // uint32_t batch_num; // However, is it necessary? psram can not store too many pictures
    uint32_t in_pic_w;
    uint32_t in_pic_h;
    uint32_t in_block_w;
    uint32_t in_block_h;
    uint32_t in_block_offset_x;
    uint32_t in_block_offset_y;

    void *out_buffer; // TODO: alignment restriction
    uint32_t out_pic_w;
    uint32_t out_pic_h;
    uint32_t out_block_offset_x;
    uint32_t out_block_offset_y;

    ppa_sr_rotation_angle_t rotation_angle;
    float scale_x;
    float scale_y;
    bool mirror_x;
    bool mirror_y;

    struct {
        ppa_sr_color_mode_t mode;
        color_range_t yuv_range;
        color_conv_std_rgb_yuv_t yuv_std;
        bool rgb_swap;
        bool byte_swap;
    } in_color;

    struct {
        ppa_sr_color_mode_t mode;
        color_range_t yuv_range;
        color_conv_std_rgb_yuv_t yuv_std;
    } out_color;

    ppa_alpha_mode_t alpha_mode;
    uint32_t alpha_value;                       /*!< When PPA_ALPHA_FIX_VALUE mode is selected, alpha_value is the alpha value to be replaced with (output_alpha = alpha_value)
                                                     When PPA_ALPHA_SCALE mode is selected, alpha_value/256 is the multiplier to the input alpha value (output_alpha = input_alpha * alpha_value / 256)
                                                     When other alpha modes are selected, this field is not used*/
} ppa_sr_trans_config_t;

/**
 * @brief Perform a scaling-and-rotating (SR) operation to a picture
 *
 * @param[in] ppa_engine PPA engine handle with `PPA_ENGINE_TYPE_SR` as the engine type
 * @param[in] config Pointer to a collection of configurations for the SR operation, ppa_sr_trans_config_t
 * @param[in] mode Select one mode from ppa_trans_mode_t
 *
 * @return
 *      - ESP_OK:
 *      - ESP_ERR_INVALID_ARG:
 *      - ESP_ERR_NO_MEM:
 *      - ESP_FAIL:
 */
esp_err_t ppa_do_scale_and_rotate(ppa_engine_handle_t ppa_engine, const ppa_sr_trans_config_t *config, ppa_trans_mode_t mode);

typedef struct {
    void *in_bg_buffer;
    void *in_fg_buffer;
    void *out_buffer;
} ppa_blend_trans_config_t;

/**
 * @brief Perform a blending operation to a picture
 *
 * @param[in] ppa_engine PPA engine handle with `PPA_ENGINE_TYPE_BLEND` as the engine type
 * @param[in] config Pointer to a collection of configurations for the blending operation, ppa_blend_trans_config_t
 * @param[in] mode Select one mode from ppa_trans_mode_t
 *
 * @return
 *      - ESP_OK:
 *      - ESP_ERR_INVALID_ARG:
 *      - ESP_ERR_NO_MEM:
 *      - ESP_FAIL:
 */
esp_err_t ppa_do_blend(ppa_engine_handle_t ppa_engine, const ppa_blend_trans_config_t *config, ppa_trans_mode_t mode);

typedef struct {
    void *out_buffer;
} ppa_fill_trans_config_t;

/**
 * @brief Perform a filling operation to a picture
 *
 * @param[in] ppa_engine PPA engine handle with `PPA_ENGINE_TYPE_BLEND` as the engine type
 * @param[in] config Pointer to a collection of configurations for the filling operation, ppa_fill_trans_config_t
 * @param[in] mode Select one mode from ppa_trans_mode_t
 *
 * @return
 *      - ESP_OK:
 *      - ESP_ERR_INVALID_ARG:
 *      - ESP_ERR_NO_MEM:
 *      - ESP_FAIL:
 */
esp_err_t ppa_do_fill(ppa_engine_handle_t ppa_engine, const ppa_fill_trans_config_t *config, ppa_trans_mode_t mode);

// SR and Blending are independent, can work at the same time
// Fill is in blend, so fill and blend cannot work at the same time

// Consider blocking and non-blocking options
// Non-blocking may require notification of process done event

// dma2d done/eof callback, when/how to free dma2d transaction content

// cache writeback/invalidate

#ifdef __cplusplus
}
#endif
