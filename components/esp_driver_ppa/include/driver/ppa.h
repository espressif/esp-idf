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
 * @brief Enumeration of all PPA available operations
 */
typedef enum {
    PPA_OPERATION_SRM,              /*!< Do scale-rotate-mirror operation */
    PPA_OPERATION_BLEND,            /*!< Do blend operation */
    PPA_OPERATION_FILL,             /*!< Do fill operation */
    PPA_OPERATION_NUM,              /*!< Quantity of PPA operations */
} ppa_operation_t;

/**
 * @brief Type of PPA invoker handle
 */
typedef struct ppa_invoker_t *ppa_invoker_handle_t;

/**
 * @brief Type of PPA transaction handle
 */
typedef struct ppa_trans_t *ppa_trans_handle_t;

/**
 * @brief Type of PPA event data
 */
typedef struct {
    ;
} ppa_event_data_t;

/**
 * @brief Type of PPA event callback
 *
 * @param ppa_invoker PPA invoker handle
 * @param event_data PPA event data
 * @param user_data User registered data from calling `ppa_do_xxx` to perform an operation
 *
 * @return Whether a task switch is needed after the callback function returns, this is usually due to the callback
 *         wakes up some high priority task.
 */
typedef bool (*ppa_event_callback_t)(ppa_invoker_handle_t ppa_invoker, ppa_event_data_t *event_data, void *user_data);

/**
 * @brief A collection of configuration items that used for registering a PPA invoker
 */
typedef struct {
    ppa_operation_t oper_type;          /*!< The desired PPA operation for the invoker */
    ppa_event_callback_t done_cb;       /*!< Callback function to be executed when a PPA transaction finishes */
    uint32_t max_pending_trans_num;     /*!< The maximum number of pending transactions for the invoker.
                                             By default, it will be 1, which is sufficient if all transactions are performed with `PPA_TRANS_MODE_BLOCKING` */
} ppa_invoker_config_t;

/**
 * @brief Register a PPA invoker to do a specific PPA operation
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
 *      - ESP_ERR_INVALID_ARG: Unregister the PPA invoker failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Unregister the PPA invoker failed because there are unfinished transactions
 */
esp_err_t ppa_unregister_invoker(ppa_invoker_handle_t ppa_invoker);

/**
 * @brief A collection of configuration items for an input picture and the target block inside the picture
 */
typedef struct {
    void *buffer;                           /*!< Pointer to the input picture buffer */
    uint32_t pic_w;                         /*!< Input picture width (unit: pixel) */
    uint32_t pic_h;                         /*!< Input picture height (unit: pixel) */
    uint32_t block_w;                       /*!< Target block width (unit: pixel) */
    uint32_t block_h;                       /*!< Target block height (unit: pixel) */
    uint32_t block_offset_x;                /*!< Target block offset in x direction in the picture (unit: pixel) */
    uint32_t block_offset_y;                /*!< Target block offset in y direction in the picture (unit: pixel) */
    union {
        ppa_srm_color_mode_t srm_cm;        /*!< Color mode of the picture in a PPA SRM operation. Supported color mode in `ppa_srm_color_mode_t` */
        ppa_blend_color_mode_t blend_cm;    /*!< Color mode of the picture in a PPA blend operation. Supported color mode in `ppa_blend_color_mode_t` */
        ppa_fill_color_mode_t fill_cm;      /*!< Color mode of the picture in a PPA fill operation. Supported color mode in `ppa_fill_color_mode_t` */
    };
    color_range_t yuv_range;                /*!< When the color mode is any YUV color space, this field is to describe its color range */
    color_conv_std_rgb_yuv_t yuv_std;       /*!< When the color mode is any YUV color space, this field is to describe its YUV<->RGB conversion standard */
} ppa_in_pic_blk_config_t;

/**
 * @brief A collection of configuration items for an output picture and the target block inside the picture
 */
typedef struct {
    void *buffer;                           /*!< Pointer to the output picture buffer (requires align to the cache line size) */
    uint32_t buffer_size;                   /*!< Size of the output picture buffer (requires align to the cache line size) */
    uint32_t pic_w;                         /*!< Output picture width (unit: pixel) */
    uint32_t pic_h;                         /*!< Output picture height (unit: pixel) */
    uint32_t block_offset_x;                /*!< Target block offset in x direction in the picture (unit: pixel) */
    uint32_t block_offset_y;                /*!< Target block offset in y direction in the picture (unit: pixel) */
    union {
        ppa_srm_color_mode_t srm_cm;        /*!< Color mode of the picture in a PPA SRM operation. Supported color mode in `ppa_srm_color_mode_t` */
        ppa_blend_color_mode_t blend_cm;    /*!< Color mode of the picture in a PPA blend operation. Supported color mode in `ppa_blend_color_mode_t` */
        ppa_fill_color_mode_t fill_cm;      /*!< Color mode of the picture in a PPA fill operation. Supported color mode in `ppa_fill_color_mode_t` */
    };
    color_range_t yuv_range;                /*!< When the color mode is any YUV color space, this field is to describe its color range */
    color_conv_std_rgb_yuv_t yuv_std;       /*!< When the color mode is any YUV color space, this field is to describe its YUV<->RGB conversion standard */
} ppa_out_pic_blk_config_t;

/**
 * @brief Modes to perform the PPA operations
 */
typedef enum {
    PPA_TRANS_MODE_BLOCKING,        /*!< `ppa_do_xxx` function will block until the PPA operation is finished */
    PPA_TRANS_MODE_NON_BLOCKING,    /*!< `ppa_do_xxx` function will return immediately after the PPA operation is pushed to the internal queue */
} ppa_trans_mode_t;

/**
 * @brief A collection of configuration items to do a PPA SRM operation transaction
 */
typedef struct {
    ppa_in_pic_blk_config_t in;                 /*!< Information of the input picture and the target block */
    ppa_out_pic_blk_config_t out;               /*!< Information of the output picture and the target block */

    // scale-rotate-mirror manipulation
    ppa_srm_rotation_angle_t rotation_angle;    /*!< Rotation (counter-clockwise) to the target block, select from `ppa_srm_rotation_angle_t` */
    float scale_x;                              /*!< Scaling factor to the target block in the x direction */
    float scale_y;                              /*!< Scaling factor to the target block in the y direction */
    bool mirror_x;                              /*!< Whether to mirror the target block in the x direction */
    bool mirror_y;                              /*!< Whether to mirror the target block in the y direction */

    // input data manipulation
    bool rgb_swap;                              /*!< Whether to swap the input data in RGB (e.g. ARGB becomes BGRA, RGB becomes BGR) */
    bool byte_swap;                             /*!< Whether to swap the input data in byte. Only available feature if input picture color mode is ARGB8888 or RGB565 */
    ppa_alpha_update_mode_t alpha_update_mode;  /*!< Select whether the alpha channel of the input picture needs update */
    uint32_t alpha_value;                       /*!< Range: 0 ~ 255
                                                     When PPA_ALPHA_FIX_VALUE mode is selected, alpha_value is the alpha value to be replaced with (output_alpha = alpha_value)
                                                     When PPA_ALPHA_SCALE mode is selected, alpha_value/256 is the multiplier to the input alpha value (output_alpha = input_alpha * alpha_value / 256)
                                                     When other alpha modes are selected, this field is not used */

    ppa_trans_mode_t mode;                      /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
    // uint32_t timeout;
    void *user_data;                            /*!< User registered data to be passed into `done_cb` callback function */
} ppa_srm_oper_trans_config_t;

/**
 * @brief A collection of configuration items to perform a PPA transaction
 */
typedef struct {
    ppa_trans_mode_t mode;          /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
    void *user_data;                /*!< User registered data to be passed into `done_cb` callback function */
} ppa_trans_config_t;

/**
 * @brief A collection of configuration items to perform a PPA SRM operation
 */
typedef struct {
    void *in_buffer; /*!< TODO: could be a buffer list, link descriptors together, process a batch
    uint32_t batch_num; However, is it necessary? psram can not store too many pictures */
    uint32_t in_pic_w;
    uint32_t in_pic_h;
    uint32_t in_block_w;
    uint32_t in_block_h;
    uint32_t in_block_offset_x;
    uint32_t in_block_offset_y;

    void *out_buffer;
    uint32_t out_buffer_size;
    uint32_t out_pic_w;
    uint32_t out_pic_h;
    uint32_t out_block_offset_x;
    uint32_t out_block_offset_y;

    ppa_srm_rotation_angle_t rotation_angle;
    float scale_x;
    float scale_y;
    bool mirror_x;
    bool mirror_y;

    struct {
        ppa_srm_color_mode_t mode;
        color_range_t yuv_range;
        color_conv_std_rgb_yuv_t yuv_std;
        bool rgb_swap;
        bool byte_swap;
        ppa_alpha_update_mode_t alpha_update_mode;
        uint32_t alpha_value;                       /*!< When PPA_ALPHA_FIX_VALUE mode is selected, alpha_value is the alpha value to be replaced with (output_alpha = alpha_value)
                                                     When PPA_ALPHA_SCALE mode is selected, alpha_value/256 is the multiplier to the input alpha value (output_alpha = input_alpha * alpha_value / 256)
                                                     When other alpha modes are selected, this field is not used */
    } in_color;

    struct {
        ppa_srm_color_mode_t mode;
        color_range_t yuv_range;
        color_conv_std_rgb_yuv_t yuv_std;
    } out_color;
} ppa_srm_operation_config_t;

/**
 * @brief Perform a scaling-rotating-mirroring (SRM) operation to a picture
 *
 * @param[in] ppa_invoker PPA invoker handle that has been registered to do SRM operations
 * @param[in] oper_config Pointer to a collection of configurations for the SRM operation, ppa_srm_operation_config_t
 * @param[in] trans_config Pointer to a collection of configurations for the transaction, ppa_trans_config_t
 *
 * @return
 *      - ESP_OK: Perform a SRM operation successfully
 *      - ESP_ERR_INVALID_ARG: Perform a SRM operation failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Perform a SRM operation failed because out of memory
 *      - ESP_FAIL: Perform a SRM operation failed because the invoker cannot accept transaction now
 */
esp_err_t ppa_do_scale_rotate_mirror(ppa_invoker_handle_t ppa_invoker, const ppa_srm_operation_config_t *oper_config, const ppa_trans_config_t *trans_config);

/**
 * @brief A collection of configuration items to do a PPA blend operation transaction
 */
typedef struct {
    ppa_in_pic_blk_config_t in_bg;                 /*!< Information of the input background picture and the target block */
    ppa_in_pic_blk_config_t in_fg;                 /*!< Information of the input foreground picture and the target block */
    ppa_out_pic_blk_config_t out;                  /*!< Information of the output picture and the target block */

    // input data manipulation
    bool bg_rgb_swap;                              /*!< Whether to swap the background input data in RGB (e.g. ARGB becomes BGRA, RGB becomes BGR) */
    bool bg_byte_swap;                             /*!< Whether to swap the background input data in byte. Only available feature if input BG picture color mode is ARGB8888 or RGB565 */
    ppa_alpha_update_mode_t bg_alpha_update_mode;  /*!< Select whether the alpha channel of the input background picture needs update */
    uint32_t bg_alpha_value;                       /*!< Range: 0 ~ 255
                                                        When PPA_ALPHA_FIX_VALUE mode is selected, alpha_value is the alpha value to be replaced with (output_alpha = alpha_value)
                                                        When PPA_ALPHA_SCALE mode is selected, alpha_value/256 is the multiplier to the input alpha value (output_alpha = input_alpha * alpha_value / 256)
                                                        When other alpha modes are selected, this field is not used */
    bool fg_rgb_swap;                              /*!< Whether to swap the foreground input data in RGB (e.g. ARGB becomes BGRA, RGB becomes BGR) */
    bool fg_byte_swap;                             /*!< Whether to swap the foreground input data in byte. Only available feature if input FG picture color mode is ARGB8888 or RGB565 */
    ppa_alpha_update_mode_t fg_alpha_update_mode;  /*!< Select whether the alpha channel of the input foreground picture needs update */
    uint32_t fg_alpha_value;                       /*!< Range: 0 ~ 255
                                                        When PPA_ALPHA_FIX_VALUE mode is selected, alpha_value is the alpha value to be replaced with (output_alpha = alpha_value)
                                                        When PPA_ALPHA_SCALE mode is selected, alpha_value/256 is the multiplier to the input alpha value (output_alpha = input_alpha * alpha_value / 256)
                                                        When other alpha modes are selected, this field is not used */
    uint32_t fg_fix_rgb_val;                       /*!< When in_fg.blend_cm is PPA_BLEND_COLOR_MODE_A8/4, this field can be used to set a fixed color for the foreground, in RGB888 format (R[23:16], G[15: 8], B[7:0]) */

    // color-keying
    // A pixel, where its background element and foreground element are both out of their color-keying ranges, will follow Alpha Blending
    bool bg_ck_en;                                 /*!< Whether to enable color keying for background
                                                        If not enabled, all background pixels are considered as out of the color-keying range */
    uint32_t bg_ck_rgb_low_thres;                  /*!< The lower threshold of the color-keying range for the background, in RGB888 format (R[23:16], G[15: 8], B[7:0]) */
    uint32_t bg_ck_rgb_high_thres;                 /*!< The higher threshold of the color-keying range for the background, in RGB888 format (R[23:16], G[15: 8], B[7:0]) */
    bool fg_ck_en;                                 /*!< Whether to enable color keying for foreground
                                                        If not enabled, all foreground pixels are considered as out of the color-keying range */
    uint32_t fg_ck_rgb_low_thres;                  /*!< The lower threshold of the color-keying range for the foreground, in RGB888 format (R[23:16], G[15: 8], B[7:0]) */
    uint32_t fg_ck_rgb_high_thres;                 /*!< The higher threshold of the color-keying range for the foreground, in RGB888 format (R[23:16], G[15: 8], B[7:0]) */
    uint32_t ck_rgb_default_val;                   /*!< The color to overwrite when a pixel, where its background element and foreground element are both within their color-keying ranges, in RGB888 format (R[23:16], G[15: 8], B[7:0]) */
    bool ck_reverse_bg2fg;                         /*!< If this bit is set, in color-keying, for the pixel, where its background element is in the color range, but its foreground element is not in the color range, it will output the foreground element instead of the background element */

    ppa_trans_mode_t mode;                         /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
    // uint32_t timeout;
    void *user_data;                               /*!< User registered data to be passed into `done_cb` callback function */
} ppa_blend_oper_trans_config_t;

/**
 * @brief A collection of configuration items to perform a PPA blend operation
 */
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
    uint32_t out_buffer_size;
    uint32_t out_pic_w;
    uint32_t out_pic_h;
    uint32_t out_block_offset_x;
    uint32_t out_block_offset_y;

    struct {
        ppa_blend_color_mode_t mode;
        bool rgb_swap;
        bool byte_swap;
        ppa_alpha_update_mode_t alpha_update_mode;
        uint32_t alpha_value;
        bool ck_en;
        uint32_t ck_rgb_low_thres;              /*!< In RGB888 format (R[23:16], G[15: 8], B[7:0]) */
        uint32_t ck_rgb_high_thres;             /*!< In RGB888 format (R[23:16], G[15: 8], B[7:0]) */
    } in_bg_color;

    struct {
        ppa_blend_color_mode_t mode;
        bool rgb_swap;
        bool byte_swap;
        ppa_alpha_update_mode_t alpha_update_mode;
        uint32_t alpha_value;
        uint32_t fix_rgb_val;                   /*!< When in_fg_color.mode is PPA_BLEND_COLOR_MODE_A8/4, this field can be used to set a fixed color for the foreground. In RGB888 format (R[23:16], G[15: 8], B[7:0]). */
        bool ck_en;
        uint32_t ck_rgb_low_thres;              /*!< In RGB888 format (R[23:16], G[15: 8], B[7:0]) */
        uint32_t ck_rgb_high_thres;             /*!< In RGB888 format (R[23:16], G[15: 8], B[7:0]) */
    } in_fg_color;

    struct {
        ppa_blend_color_mode_t mode;
        uint32_t ck_rgb_default_val;            /*!< In RGB888 format (R[23:16], G[15: 8], B[7:0]) */
        bool ck_reverse_bg2fg;                  /*!< If this bit is set, in color keying, for the pixel, where its BG element is in the color range, but its FG element is not in the color range, it will output the FG element instead of the BG element */
    } out_color;
} ppa_blend_operation_config_t;

/**
 * @brief Perform a blending operation to a picture
 *
 * @param[in] ppa_invoker PPA invoker handle that has been registered to do blend operations
 * @param[in] oper_config Pointer to a collection of configurations for the blend operation, ppa_blend_operation_config_t
 * @param[in] trans_config Pointer to a collection of configurations for the transaction, ppa_trans_config_t
 *
 * @return
 *      - ESP_OK: Perform a blend operation successfully
 *      - ESP_ERR_INVALID_ARG: Perform a blend operation failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Perform a blend operation failed because out of memory
 *      - ESP_FAIL: Perform a blend operation failed because the invoker cannot accept transaction now
 */
esp_err_t ppa_do_blend(ppa_invoker_handle_t ppa_invoker, const ppa_blend_operation_config_t *oper_config, const ppa_trans_config_t *trans_config);

/**
 * @brief A collection of configuration items to do a PPA fill operation transaction
 */
typedef struct {
    ppa_out_pic_blk_config_t out;                  /*!< Information of the output picture and the target block */

    uint32_t fill_block_w;                         /*!< The width of the block to be filled (unit: pixel) */
    uint32_t fill_block_h;                         /*!< The height of the block to be filled (unit: pixel) */
    uint32_t fill_argb_color;                      /*!< The color to be filled, in ARGB8888 format ((A[31:24], R[23:16], G[15: 8], B[7:0])) */

    ppa_trans_mode_t mode;                         /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
    // uint32_t timeout;
    void *user_data;                               /*!< User registered data to be passed into `done_cb` callback function */
} ppa_fill_oper_trans_config_t;

/**
 * @brief A collection of configuration items to perform a PPA fill operation
 */
typedef struct {
    uint32_t fill_block_w;
    uint32_t fill_block_h;
    uint32_t fill_argb_color;                   /*!< The color to be filled, in ARGB8888 format ((A[31:24], R[23:16], G[15: 8], B[7:0])) */

    void *out_buffer;
    uint32_t out_buffer_size;
    uint32_t out_pic_w;
    uint32_t out_pic_h;
    uint32_t out_block_offset_x;
    uint32_t out_block_offset_y;

    struct {
        ppa_blend_color_mode_t mode;
    } out_color;
} ppa_fill_operation_config_t;

/**
 * @brief Perform a filling operation to a picture
 *
 * @param[in] ppa_invoker PPA invoker handle that has been registered to do fill operations
 * @param[in] oper_config Pointer to a collection of configurations for the fill operation, ppa_fill_operation_config_t
 * @param[in] trans_config Pointer to a collection of configurations for the transaction, ppa_trans_config_t
 *
 * @return
 *      - ESP_OK: Perform a fill operation successfully
 *      - ESP_ERR_INVALID_ARG: Perform a fill operation failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Perform a fill operation failed because out of memory
 *      - ESP_FAIL: Perform a fill operation failed because the invoker cannot accept transaction now
 */
esp_err_t ppa_do_fill(ppa_invoker_handle_t ppa_invoker, const ppa_fill_operation_config_t *oper_config, const ppa_trans_config_t *trans_config);

#ifdef __cplusplus
}
#endif
