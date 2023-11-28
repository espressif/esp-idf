/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of engines in PPA modules
 */
typedef enum {
    PPA_ENGINE_TYPE_SR,             /*!< PPA Scaling and Rotating (SR) engine, used to perform scale_and_rotate */
    PPA_ENGINE_TYPE_BLEND,          /*!< PPA Blending engine, used to perform blend or fill */
} ppa_engine_type_t;

/**
 * @brief Enumeration of PPA Scaling and Rotating available rotation angle (in the counterclockwise direction)
 */
typedef enum {
    PPA_SR_ROTATION_ANGLE_0,        /*!< Picture does no rotation */
    PPA_SR_ROTATION_ANGLE_90,       /*!< Picture rotates 90 degrees CCW */
    PPA_SR_ROTATION_ANGLE_180,      /*!< Picture rotates 180 degrees CCW */
    PPA_SR_ROTATION_ANGLE_270,      /*!< Picture rotates 270 degrees CCW */
} ppa_sr_rotation_angle_t;

/**
 * @brief Enumeration of PPA Scaling and Rotating available color mode
 */
typedef enum {
    PPA_SR_COLOR_MODE_ARGB8888 = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888),      /*!< PPA SR color mode: ARGB8888 */
    PPA_SR_COLOR_MODE_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),           /*!< PPA SR color mode: RGB888 */
    PPA_SR_COLOR_MODE_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),           /*!< PPA SR color mode: RGB565 */
    PPA_SR_COLOR_MODE_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420),           /*!< PPA SR color mode: YUV420 */
} ppa_sr_color_mode_t;

/**
 * @brief Enumeration of PPA Blending available color mode
 */
typedef enum {
    PPA_BLEND_COLOR_MODE_ARGB8888 = COLOR_TYPE_ID(COLOR_SPACE_ARGB, COLOR_PIXEL_ARGB8888),   /*!< PPA Blending color mode: ARGB8888 */
    PPA_BLEND_COLOR_MODE_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),        /*!< PPA Blending color mode: RGB888 */
    PPA_BLEND_COLOR_MODE_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),        /*!< PPA Blending color mode: RGB565 */
    PPA_BLEND_COLOR_MODE_L8 = COLOR_TYPE_ID(COLOR_SPACE_CLUT, COLOR_PIXEL_L8),               /*!< PPA Blending color mode: L8, only available on blending inputs */
    PPA_BLEND_COLOR_MODE_L4 = COLOR_TYPE_ID(COLOR_SPACE_CLUT, COLOR_PIXEL_L4),               /*!< PPA Blending color mode: L4, only available on blending inputs */
    PPA_BLEND_COLOR_MODE_A8 = COLOR_TYPE_ID(COLOR_SPACE_ALPHA, COLOR_PIXEL_A8),              /*!< PPA Blending color mode: A8, only available on blending foreground input */
    PPA_BLEND_COLOR_MODE_A4 = COLOR_TYPE_ID(COLOR_SPACE_ALPHA, COLOR_PIXEL_A4),              /*!< PPA Blending color mode: A4, only available on blending foreground input */
} ppa_blend_color_mode_t;

#ifdef __cplusplus
}
#endif
