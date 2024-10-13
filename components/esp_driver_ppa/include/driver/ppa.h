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
    PPA_OPERATION_FILL,             /*!< Do fill operation, use one constant pixel to fill a target window */
    PPA_OPERATION_INVALID,          /*!< Invalid PPA operations, indicates the quantity of available PPA operations */
} ppa_operation_t;

/**
 * @brief Type of PPA client handle
 */
typedef struct ppa_client_t *ppa_client_handle_t;

/**
 * @brief A collection of configuration items that used for registering a PPA client
 */
typedef struct {
    ppa_operation_t oper_type;                  /*!< The desired PPA operation for the client */
    uint32_t max_pending_trans_num;             /*!< The maximum number of pending transactions for the client.
                                                     By default, it will be 1, which is sufficient if all transactions are performed with `PPA_TRANS_MODE_BLOCKING` */
    ppa_data_burst_length_t data_burst_length;  /*!< The desired data burst length for all the transactions of the client.
                                                     Use a small burst length will decrease PPA performance, but can save burst bandwidth for other peripheral usages.
                                                     By default, it will be at the maximum burst length, `PPA_DATA_BURST_LENGTH_128` */
} ppa_client_config_t;

/**
 * @brief Register a PPA client to do a specific PPA operation
 *
 * @param[in] config Pointer to a collection of configurations for the client
 * @param[out] ret_client Returned client handle
 *
 * @return
 *      - ESP_OK: Register the PPA client successfully
 *      - ESP_ERR_INVALID_ARG: Register the PPA client failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Register the PPA client failed because out of memory
 *      - ESP_FAIL: Register the PPA client failed because of other error
 */
esp_err_t ppa_register_client(const ppa_client_config_t *config, ppa_client_handle_t *ret_client);

/**
 * @brief Unregister a PPA client
 *
 * @note This will also free the resources occupied by the client
 *
 * @param[in] ppa_client PPA client handle, allocated by `ppa_register_client`
 *
 * @return
 *      - ESP_OK: Unregister the PPA client successfully
 *      - ESP_ERR_INVALID_ARG: Unregister the PPA client failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Unregister the PPA client failed because there are unfinished transactions
 */
esp_err_t ppa_unregister_client(ppa_client_handle_t ppa_client);

/**
 * @brief Type of PPA event data
 */
typedef struct {
} ppa_event_data_t;

/**
 * @brief Type of PPA event callback
 *
 * @param[in] ppa_client PPA client handle
 * @param[in] event_data PPA event data
 * @param[in] user_data User registered data from calling `ppa_do_xxx` to perform an operation
 *
 * @return Whether a task switch is needed after the callback function returns, this is usually due to the callback
 *         wakes up some high priority task.
 */
typedef bool (*ppa_event_callback_t)(ppa_client_handle_t ppa_client, ppa_event_data_t *event_data, void *user_data);

/**
 * @brief Group of supported PPA callbacks
 */
typedef struct {
    ppa_event_callback_t on_trans_done;     /*!< Invoked when a PPA transaction finishes */
} ppa_event_callbacks_t;

/**
 * @brief Register event callbacks for a PPA client
 *
 * @param[in] ppa_client PPA client handle
 * @param[in] cbs Structure with all PPA callbacks
 *
 * @note Any user private data that wants to be passed directly to callback's user_data is provided per PPA transaction.
 *       Please check the `user_data` field in `ppa_xxx_oper_config_t` structure.
 *
 * @return
 *      - ESP_OK: Register event callbacks for the PPA client successfully
 *      - ESP_ERR_INVALID_ARG: Register event callbacks for the PPA client failed because of invalid argument
 */
esp_err_t ppa_client_register_event_callbacks(ppa_client_handle_t ppa_client, const ppa_event_callbacks_t *cbs);

/**
 * @brief A collection of configuration items for an input picture and the target block inside the picture
 */
typedef struct {
    const void *buffer;                     /*!< Pointer to the input picture buffer */
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
    ppa_color_range_t yuv_range;            /*!< When the color mode is any YUV color space, this field is to describe its color range */
    ppa_color_conv_std_rgb_yuv_t yuv_std;   /*!< When the color mode is any YUV color space, this field is to describe its YUV<->RGB conversion standard */
} ppa_in_pic_blk_config_t;

/**
 * @brief A collection of configuration items for an output picture and the target block inside the picture
 */
typedef struct {
    void *buffer;                           /*!< Pointer to the output picture buffer (requires alignment: internal memory needs align to L1 cache line size, external memory needs align to L1 and L2 cache line size) */
    uint32_t buffer_size;                   /*!< Size of the output picture buffer (requires alignment: internal memory needs align to L1 cache line size, external memory needs align to L1 and L2 cache line size) */
    uint32_t pic_w;                         /*!< Output picture width (unit: pixel) */
    uint32_t pic_h;                         /*!< Output picture height (unit: pixel) */
    uint32_t block_offset_x;                /*!< Target block offset in x direction in the picture (unit: pixel) */
    uint32_t block_offset_y;                /*!< Target block offset in y direction in the picture (unit: pixel) */
    union {
        ppa_srm_color_mode_t srm_cm;        /*!< Color mode of the picture in a PPA SRM operation. Supported color mode in `ppa_srm_color_mode_t` */
        ppa_blend_color_mode_t blend_cm;    /*!< Color mode of the picture in a PPA blend operation. Supported color mode in `ppa_blend_color_mode_t` */
        ppa_fill_color_mode_t fill_cm;      /*!< Color mode of the picture in a PPA fill operation. Supported color mode in `ppa_fill_color_mode_t` */
    };
    ppa_color_range_t yuv_range;            /*!< When the color mode is any YUV color space, this field is to describe its color range */
    ppa_color_conv_std_rgb_yuv_t yuv_std;   /*!< When the color mode is any YUV color space, this field is to describe its YUV<->RGB conversion standard */
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
    union {
        uint32_t alpha_fix_val;                 /*!< Range: [0, 255]
                                                     When PPA_ALPHA_FIX_VALUE mode is selected, alpha_fix_val is the new alpha value to replace the input alpha value (output_alpha = alpha_fix_val) */
        float alpha_scale_ratio;                /*!< Range: (0, 1)
                                                     When PPA_ALPHA_SCALE mode is selected, alpha_scale_ratio is the multiplier to the input alpha value (output_alpha = alpha_scale_ratio * input_alpha)
                                                     Ratio resolution is 1/256 */
    };

    ppa_trans_mode_t mode;                      /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
    void *user_data;                            /*!< User registered data to be passed into `done_cb` callback function */
} ppa_srm_oper_config_t;

/**
 * @brief Perform a scaling-rotating-mirroring (SRM) operation to a picture
 *
 * @param[in] ppa_client PPA client handle that has been registered to do SRM operations
 * @param[in] config Pointer to a collection of configurations for the SRM operation transaction, ppa_srm_oper_config_t
 *
 * @return
 *      - ESP_OK: Perform a SRM operation successfully
 *      - ESP_ERR_INVALID_ARG: Perform a SRM operation failed because of invalid argument
 *      - ESP_FAIL: Perform a SRM operation failed because the client's pending transactions has reached its maximum capacity
 */
esp_err_t ppa_do_scale_rotate_mirror(ppa_client_handle_t ppa_client, const ppa_srm_oper_config_t *config);

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
    union {
        uint32_t bg_alpha_fix_val;                 /*!< Range: [0, 255]
                                                        When PPA_ALPHA_FIX_VALUE mode is selected, alpha_fix_val is the new alpha value to replace the input alpha value (output_alpha = alpha_fix_val) */
        float bg_alpha_scale_ratio;                /*!< Range: (0, 1)
                                                        When PPA_ALPHA_SCALE mode is selected, alpha_scale_ratio is the multiplier to the input alpha value (output_alpha = alpha_scale_ratio * input_alpha)
                                                        Ratio resolution is 1/256 */
    };
    bool fg_rgb_swap;                              /*!< Whether to swap the foreground input data in RGB (e.g. ARGB becomes BGRA, RGB becomes BGR) */
    bool fg_byte_swap;                             /*!< Whether to swap the foreground input data in byte. Only available feature if input FG picture color mode is ARGB8888 or RGB565 */
    ppa_alpha_update_mode_t fg_alpha_update_mode;  /*!< Select whether the alpha channel of the input foreground picture needs update */
    union {
        uint32_t fg_alpha_fix_val;                 /*!< Range: [0, 255]
                                                        When PPA_ALPHA_FIX_VALUE mode is selected, alpha_fix_val is the new alpha value to replace the input alpha value (output_alpha = alpha_fix_val) */
        float fg_alpha_scale_ratio;                /*!< Range: (0, 1)
                                                        When PPA_ALPHA_SCALE mode is selected, alpha_scale_ratio is the multiplier to the input alpha value (output_alpha = alpha_scale_ratio * input_alpha)
                                                        Ratio resolution is 1/256 */
    };
    color_pixel_rgb888_data_t fg_fix_rgb_val;      /*!< When in_fg.blend_cm is PPA_BLEND_COLOR_MODE_A8/4, this field can be used to set a fixed color for the foreground, in RGB888 format */

    // color-keying
    // A pixel, where its background element and foreground element are both out of their color-keying ranges, will follow Alpha Blending
    bool bg_ck_en;                                 /*!< Whether to enable color keying for background
                                                        If not enabled, all background pixels are considered as out of the color-keying range */
    color_pixel_rgb888_data_t bg_ck_rgb_low_thres; /*!< The lower threshold of the color-keying range for the background, in RGB888 format */
    color_pixel_rgb888_data_t bg_ck_rgb_high_thres;/*!< The higher threshold of the color-keying range for the background, in RGB888 format */
    bool fg_ck_en;                                 /*!< Whether to enable color keying for foreground
                                                        If not enabled, all foreground pixels are considered as out of the color-keying range */
    color_pixel_rgb888_data_t fg_ck_rgb_low_thres; /*!< The lower threshold of the color-keying range for the foreground, in RGB888 format */
    color_pixel_rgb888_data_t fg_ck_rgb_high_thres;/*!< The higher threshold of the color-keying range for the foreground, in RGB888 format */
    color_pixel_rgb888_data_t ck_rgb_default_val;  /*!< The color to overwrite when a pixel, where its background element and foreground element are both within their color-keying ranges, in RGB888 format */
    bool ck_reverse_bg2fg;                         /*!< If this bit is set, in color-keying, for the pixel, where its background element is in the color range, but its foreground element is not in the color range, it will output the foreground element instead of the background element */

    ppa_trans_mode_t mode;                         /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
    void *user_data;                               /*!< User registered data to be passed into `done_cb` callback function */
} ppa_blend_oper_config_t;

/**
 * @brief Perform a blending operation to a picture
 *
 * @param[in] ppa_client PPA client handle that has been registered to do blend operations
 * @param[in] config Pointer to a collection of configurations for the blend operation transaction, ppa_blend_oper_config_t
 *
 * @return
 *      - ESP_OK: Perform a blend operation successfully
 *      - ESP_ERR_INVALID_ARG: Perform a blend operation failed because of invalid argument
 *      - ESP_FAIL: Perform a blend operation failed because the client's pending transactions has reached its maximum capacity
 */
esp_err_t ppa_do_blend(ppa_client_handle_t ppa_client, const ppa_blend_oper_config_t *config);

/**
 * @brief A collection of configuration items to do a PPA fill operation transaction
 */
typedef struct {
    ppa_out_pic_blk_config_t out;                  /*!< Information of the output picture and the target block */

    uint32_t fill_block_w;                         /*!< The width of the block to be filled (unit: pixel) */
    uint32_t fill_block_h;                         /*!< The height of the block to be filled (unit: pixel) */
    color_pixel_argb8888_data_t fill_argb_color;   /*!< The color to be filled, in ARGB8888 format */

    ppa_trans_mode_t mode;                         /*!< Determines whether to block inside the operation functions, see `ppa_trans_mode_t` */
    void *user_data;                               /*!< User registered data to be passed into `done_cb` callback function */
} ppa_fill_oper_config_t;

/**
 * @brief Perform a filling operation to a picture
 *
 * @param[in] ppa_client PPA client handle that has been registered to do fill operations
 * @param[in] config Pointer to a collection of configurations for the fill operation transaction, ppa_fill_oper_config_t
 *
 * @return
 *      - ESP_OK: Perform a fill operation successfully
 *      - ESP_ERR_INVALID_ARG: Perform a fill operation failed because of invalid argument
 *      - ESP_FAIL: Perform a fill operation failed because the client's pending transactions has reached its maximum capacity
 */
esp_err_t ppa_do_fill(ppa_client_handle_t ppa_client, const ppa_fill_oper_config_t *config);

#ifdef __cplusplus
}
#endif
