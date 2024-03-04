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
 * @brief Type of PPA invoker handle
 */
typedef struct ppa_invoker_t *ppa_invoker_handle_t;

/**
 * @brief PPA operation type flags
 *
 * These flags are supposed to be used to specify the PPA operations that are going to be used by the invoker, so that
 * the corresponding engines can be acquired when registering the invoker with `ppa_register_invoker`.
 */
#define PPA_OPERATION_FLAG_SR       (1 << 0)
#define PPA_OPERATION_FLAG_BLEND    (1 << 1)
#define PPA_OPERATION_FLAG_FILL     (1 << 2)

/**
 * @brief A collection of configuration items that used for registering a PPA invoker
 */
typedef struct {
    uint32_t operation_flag;           /*!< Bitwise OR of `PPA_OPERATION_FLAG_*` flags indicating the required operations for the invoker */
    // done_cb
    // user_data
} ppa_invoker_config_t;

/**
 * @brief Register a PPA invoker
 *
 * @param[in] config Pointer to a collection of configurations for the invoker
 * @param[out] ret_invoker Returned invoker handle
 * @return
 *      - ESP_OK: Register the PPA invoker successfully
 *      - ESP_ERR_INVALID_ARG: Register the PPA invoker failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Register the PPA invoker failed because out of memory
 *      - ESP_FAIL: Register the PPA invoker failed because of other error
 */
esp_err_t ppa_register_invoker(const ppa_invoker_config_t *config, ppa_invoker_handle_t *ret_invoker);

/**
 * @brief Unregister a PPA invoker
 *
 * @param[in] ppa_invoker PPA invoker handle, allocated by `ppa_register_invoker`
 * @return
 *      - ESP_OK: Unregister the PPA invoker successfully
 */
esp_err_t ppa_unregister_invoker(ppa_invoker_handle_t ppa_invoker);

/**
 * @brief Modes to perform the PPA operations
 */
typedef enum {
    PPA_TRANS_MODE_BLOCKING,        /*!< `ppa_do_xxx` function will block until the PPA operation is finished */
    PPA_TRANS_MODE_NON_BLOCKING,    /*!< `ppa_do_xxx` function will return immediately after the PPA operation is pushed to the internal queue */
} ppa_trans_mode_t;

/**
 * @brief A collection of configuration items to perform a PPA transaction
 */
typedef struct {
    ppa_trans_mode_t mode;          /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
} ppa_trans_config_t;

#define PPA_SR_OPERATION_CONFIG struct { \
    void *in_buffer; /*!< TODO: could be a buffer list, link descriptors together, process a batch
    uint32_t batch_num; However, is it necessary? psram can not store too many pictures */ \
    uint32_t in_pic_w; \
    uint32_t in_pic_h; \
    uint32_t in_block_w; \
    uint32_t in_block_h; \
    uint32_t in_block_offset_x; \
    uint32_t in_block_offset_y; \
    \
    void *out_buffer; /*!< TODO: alignment restriction */ \
    uint32_t out_pic_w; \
    uint32_t out_pic_h; \
    uint32_t out_block_offset_x; \
    uint32_t out_block_offset_y; \
    \
    ppa_sr_rotation_angle_t rotation_angle; \
    float scale_x; \
    float scale_y; \
    bool mirror_x; \
    bool mirror_y; \
    \
    struct { \
        ppa_sr_color_mode_t mode; \
        color_range_t yuv_range; \
        color_conv_std_rgb_yuv_t yuv_std; \
        bool rgb_swap; \
        bool byte_swap; \
        ppa_alpha_mode_t alpha_mode; \
        uint32_t alpha_value;                       /*!< When PPA_ALPHA_FIX_VALUE mode is selected, alpha_value is the alpha value to be replaced with (output_alpha = alpha_value)
                                                     When PPA_ALPHA_SCALE mode is selected, alpha_value/256 is the multiplier to the input alpha value (output_alpha = input_alpha * alpha_value / 256)
                                                     When other alpha modes are selected, this field is not used */ \
    } in_color; \
    \
    struct { \
        ppa_sr_color_mode_t mode; \
        color_range_t yuv_range; \
        color_conv_std_rgb_yuv_t yuv_std; \
    } out_color; \
}

/**
 * @brief A collection of configuration items to perform a PPA SR operation
 */
typedef PPA_SR_OPERATION_CONFIG ppa_sr_operation_config_t;

/**
 * @brief Perform a scaling-and-rotating (SR) operation to a picture
 *
 * @param[in] ppa_invoker PPA invoker handle that has acquired the PPA SR engine
 * @param[in] oper_config Pointer to a collection of configurations for the SR operation, ppa_sr_operation_config_t
 * @param[in] trans_config Pointer to a collection of configurations for the transaction, ppa_trans_config_t
 *
 * @return
 *      - ESP_OK:
 *      - ESP_ERR_INVALID_ARG:
 *      - ESP_ERR_NO_MEM:
 *      - ESP_FAIL:
 */
esp_err_t ppa_do_scale_and_rotate(ppa_invoker_handle_t ppa_invoker, const ppa_sr_operation_config_t *oper_config, const ppa_trans_config_t *trans_config);

typedef struct {
    void *in_bg_buffer;
    uint32_t in_bg_pic_w;
    uint32_t in_bg_pic_h;
    uint32_t in_bg_block_offset_x;
    uint32_t in_bg_block_offset_y;

    void *in_fg_buffer;
    uint32_t in_fg_pic_w;
    uint32_t in_fg_pic_h;
    uint32_t in_fg_block_offset_x;
    uint32_t in_fg_block_offset_y;

    uint32_t in_bg_fg_block_w;
    uint32_t in_bg_fg_block_h;

    void *out_buffer;
    uint32_t out_pic_w;
    uint32_t out_pic_h;
    uint32_t out_block_offset_x;
    uint32_t out_block_offset_y;
    //out_block_w (auto or max/min(bg, fg))
    //out_block_h

    struct {
        ppa_blend_color_mode_t mode;
        bool rgb_swap;
        bool byte_swap;
        ppa_alpha_mode_t alpha_mode;
        uint32_t alpha_value;
    } in_bg_color;

    struct {
        ppa_blend_color_mode_t mode;
        bool rgb_swap;
        bool byte_swap;
        ppa_alpha_mode_t alpha_mode;
        uint32_t alpha_value;
        uint32_t fix_rgb_val;                   /*!< When in_fg_color.mode is PPA_BLEND_COLOR_MODE_A8/4, this field can be used to set a fixed color for the foreground. In RGB888 format (R[23:16], G[15: 8], B[7:0]). */
    } in_fg_color;

    struct {
        ppa_blend_color_mode_t mode;
    } out_color;

    // TODO: colorkey
} ppa_blend_operation_config_t;

/**
 * @brief Perform a blending operation to a picture
 *
 * @param[in] ppa_invoker PPA invoker handle that has acquired the PPA Blend engine
 * @param[in] oper_config Pointer to a collection of configurations for the blending operation, ppa_blend_operation_config_t
 * @param[in] trans_config Pointer to a collection of configurations for the transaction, ppa_trans_config_t
 *
 * @return
 *      - ESP_OK:
 *      - ESP_ERR_INVALID_ARG:
 *      - ESP_ERR_NO_MEM:
 *      - ESP_FAIL:
 */
esp_err_t ppa_do_blend(ppa_invoker_handle_t ppa_invoker, const ppa_blend_operation_config_t *oper_config, const ppa_trans_config_t *trans_config);

typedef struct {
    uint32_t fill_block_w;
    uint32_t fill_block_h;
    uint32_t fill_argb_color;                   /*!< The color to be filled, in ARGB8888 format ((A[31:24], R[23:16], G[15: 8], B[7:0])) */

    void *out_buffer;
    uint32_t out_pic_w;
    uint32_t out_pic_h;
    uint32_t out_block_offset_x;
    uint32_t out_block_offset_y;

    struct {
        ppa_blend_color_mode_t mode;
    } out_color;

    // colorkey???
} ppa_fill_operation_config_t;

/**
 * @brief Perform a filling operation to a picture
 *
 * @param[in] ppa_invoker PPA invoker handle that has acquired the PPA Blend engine
 * @param[in] oper_config Pointer to a collection of configurations for the filling operation, ppa_fill_operation_config_t
 * @param[in] trans_config Pointer to a collection of configurations for the transaction, ppa_trans_config_t
 *
 * @return
 *      - ESP_OK:
 *      - ESP_ERR_INVALID_ARG:
 *      - ESP_ERR_NO_MEM:
 *      - ESP_FAIL:
 */
esp_err_t ppa_do_fill(ppa_invoker_handle_t ppa_invoker, const ppa_fill_operation_config_t *oper_config, const ppa_trans_config_t *trans_config);

// argb color conversion (bypass blend)

// SR and Blending are independent, can work at the same time
// Fill is in blend, so fill and blend cannot work at the same time

// Consider blocking and non-blocking options
// Non-blocking may require notification of process done event

// cache writeback/invalidate

#ifdef __cplusplus
}
#endif
