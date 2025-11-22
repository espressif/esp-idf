/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "hal/color_types.h"
#include "hal/dma2d_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of engines in PPA modules
 */
typedef enum {
    PPA_ENGINE_TYPE_SRM,            /*!< PPA Scaling-Rotating-Mirroring (SRM) engine, used to perform scale, rotate, mirror */
    PPA_ENGINE_TYPE_BLEND,          /*!< PPA Blending engine, used to perform blend or fill */
} ppa_engine_type_t;

/**
 * @brief Enumeration of PPA Scaling-Rotating-Mirroring available rotation angle (in the counterclockwise direction)
 */
typedef enum {
    PPA_SRM_ROTATION_ANGLE_0,        /*!< Picture does no rotation */
    PPA_SRM_ROTATION_ANGLE_90,       /*!< Picture rotates 90 degrees CCW */
    PPA_SRM_ROTATION_ANGLE_180,      /*!< Picture rotates 180 degrees CCW */
    PPA_SRM_ROTATION_ANGLE_270,      /*!< Picture rotates 270 degrees CCW */
} ppa_srm_rotation_angle_t;

/**
 * @brief Enumeration of PPA Scaling-Rotating-Mirroring available color mode
 */
typedef enum {
    PPA_SRM_COLOR_MODE_ARGB8888 = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888),      /*!< PPA SRM color mode: ARGB8888 */
    PPA_SRM_COLOR_MODE_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),           /*!< PPA SRM color mode: RGB888 */
    PPA_SRM_COLOR_MODE_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),           /*!< PPA SRM color mode: RGB565 */
    PPA_SRM_COLOR_MODE_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420),           /*!< PPA SRM color mode: YUV420 */
    PPA_SRM_COLOR_MODE_YUV444 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV444),           /*!< PPA SRM color mode: YUV444 (limited range only)*/
    // YUV444 not supported by PPA hardware, but we can use 2D-DMA to do conversion before sending into and after coming out from the PPA module
    // If in_pic is YUV444, then TX DMA channel could do DMA2D_CSC_TX_YUV444_TO_RGB888_601/709, so PPA in_color_mode is RGB888
    // If out_pic is YUV444, then RX DMA channel could do DMA2D_CSC_RX_YUV420_TO_YUV444, so PPA out_color_mode is YUV420
    PPA_SRM_COLOR_MODE_YUV422_UYVY = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_UYVY422),     /*!< PPA SRM color mode: YUV422 */
    PPA_SRM_COLOR_MODE_YUV422_VYUY = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_VYUY422),     /*!< PPA SRM color mode: YUV422, only available on input */
    PPA_SRM_COLOR_MODE_YUV422_YUYV = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUYV422),     /*!< PPA SRM color mode: YUV422, only available on input */
    PPA_SRM_COLOR_MODE_YUV422_YVYU = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YVYU422),     /*!< PPA SRM color mode: YUV422, only available on input */
    PPA_SRM_COLOR_MODE_GRAY8 = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8),            /*!< PPA SRM color mode: GRAY8 */
} ppa_srm_color_mode_t;

/**
 * @brief Enumeration of PPA blend available color mode
 */
typedef enum {
    PPA_BLEND_COLOR_MODE_ARGB8888 = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888),   /*!< PPA blend color mode: ARGB8888 */
    PPA_BLEND_COLOR_MODE_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),        /*!< PPA blend color mode: RGB888 */
    PPA_BLEND_COLOR_MODE_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),        /*!< PPA blend color mode: RGB565 */
    PPA_BLEND_COLOR_MODE_A8 = COLOR_TYPE_ID(COLOR_SPACE_ALPHA, COLOR_PIXEL_A8),              /*!< PPA blend color mode: A8, only available on blend foreground input */
    PPA_BLEND_COLOR_MODE_A4 = COLOR_TYPE_ID(COLOR_SPACE_ALPHA, COLOR_PIXEL_A4),              /*!< PPA blend color mode: A4, only available on blend foreground input */
    PPA_BLEND_COLOR_MODE_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420),        /*!< PPA blend color mode: YUV420, only available on blend background input or on output */
    PPA_BLEND_COLOR_MODE_YUV422_UYVY = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_UYVY422),  /*!< PPA blend color mode: YUV422, only available on blend background input or on output */
    PPA_BLEND_COLOR_MODE_YUV422_VYUY = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_VYUY422),  /*!< PPA blend color mode: YUV422, only available on blend background input */
    PPA_BLEND_COLOR_MODE_YUV422_YUYV = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUYV422),  /*!< PPA blend color mode: YUV422, only available on blend background input */
    PPA_BLEND_COLOR_MODE_YUV422_YVYU = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YVYU422),  /*!< PPA blend color mode: YUV422, only available on blend background input */
    PPA_BLEND_COLOR_MODE_GRAY8 = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8),         /*!< PPA blend color mode: GRAY8, only available on blend background input or on output */
    // TODO: Support CLUT to support L4/L8 color mode
    // PPA_BLEND_COLOR_MODE_L8 = COLOR_TYPE_ID(COLOR_SPACE_CLUT, COLOR_PIXEL_L8),               /*!< PPA blend color mode: L8, only available on blend input */
    // PPA_BLEND_COLOR_MODE_L4 = COLOR_TYPE_ID(COLOR_SPACE_CLUT, COLOR_PIXEL_L4),               /*!< PPA blend color mode: L4, only available on blend input */
} ppa_blend_color_mode_t;

/**
 * @brief Enumeration of PPA fill available color mode
 */
typedef enum {
    PPA_FILL_COLOR_MODE_ARGB8888 = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888),    /*!< PPA fill color mode: ARGB8888 */
    PPA_FILL_COLOR_MODE_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),         /*!< PPA fill color mode: RGB888 */
    PPA_FILL_COLOR_MODE_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),         /*!< PPA fill color mode: RGB565 */
    // PPA_FILL_COLOR_MODE_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420),         /*!< PPA fill color mode: YUV420 */ // Non-typical YUV420, U and V components have to be the same value
    PPA_FILL_COLOR_MODE_YUV422_UYVY = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_UYVY422),   /*!< PPA fill color mode: YUV422 (w/ UYVY pack order) */
    PPA_FILL_COLOR_MODE_GRAY8 = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8),          /*!< PPA fill color mode: GRAY8 */
} ppa_fill_color_mode_t;

/**
 * @brief Enumeration of PPA alpha compositing update mode
 */
typedef enum {
    PPA_ALPHA_NO_CHANGE = 0,  /*!< Do not replace alpha value (A' = A).
                                   If input format does not contain alpha info, alpha value 255 will be used. */
    PPA_ALPHA_FIX_VALUE,      /*!< Replace the alpha value in received pixel with a new, fixed alpha value (A' = val) */
    PPA_ALPHA_SCALE,          /*!< Scale the alpha value in received pixel to be a new alpha value (A' = (A * val) >> 8).
                                   If input format does not contain alpha info, A' = (255 * val) >> 8. */
    PPA_ALPHA_INVERT,         /*!< Invert the alpha value in received pixel (A' = 255 - A).
                                   If input format does not contain alpha info, A' = 0, i.e. a layer with 0% opacity. */
} ppa_alpha_update_mode_t;

/**
 * @brief Enumeration of PPA supported color conversion standard between RGB and YUV (determines the YUV<->RGB conversion equation)
 */
typedef enum {
    PPA_COLOR_CONV_STD_RGB_YUV_BT601 = COLOR_CONV_STD_RGB_YUV_BT601,      /*!< YUV<->RGB conversion standard: BT.601 */
    PPA_COLOR_CONV_STD_RGB_YUV_BT709 = COLOR_CONV_STD_RGB_YUV_BT709,      /*!< YUV<->RGB conversion standard: BT.709 */
} ppa_color_conv_std_rgb_yuv_t;

/**
 * @brief Enumeration of PPA supported color range (determines the YUV<->RGB conversion equation)
 */
typedef enum {
    PPA_COLOR_RANGE_LIMIT = COLOR_RANGE_LIMIT,      /*!< Limited color range, 16 is the darkest black and 235 is the brightest white */
    PPA_COLOR_RANGE_FULL = COLOR_RANGE_FULL,        /*!< Full color range, 0 is the darkest black and 255 is the brightest white */
} ppa_color_range_t;

/**
 * @brief Enumeration of PPA supported data burst length
 */
typedef enum {
    PPA_DATA_BURST_LENGTH_8 = DMA2D_DATA_BURST_LENGTH_8,        /*!< Data burst length: 8 bytes */
    PPA_DATA_BURST_LENGTH_16 = DMA2D_DATA_BURST_LENGTH_16,      /*!< Data burst length: 16 bytes */
    PPA_DATA_BURST_LENGTH_32 = DMA2D_DATA_BURST_LENGTH_32,      /*!< Data burst length: 32 bytes */
    PPA_DATA_BURST_LENGTH_64 = DMA2D_DATA_BURST_LENGTH_64,      /*!< Data burst length: 64 bytes */
    PPA_DATA_BURST_LENGTH_128 = DMA2D_DATA_BURST_LENGTH_128,    /*!< Data burst length: 128 bytes */
} ppa_data_burst_length_t;

#ifdef __cplusplus
}
#endif
