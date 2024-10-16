/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RGB_PANEL_SWAP_XY  0
#define RGB_PANEL_MIRROR_Y 1
#define RGB_PANEL_MIRROR_X 2

typedef enum {
    ROTATE_MASK_SWAP_XY = BIT(RGB_PANEL_SWAP_XY),
    ROTATE_MASK_MIRROR_Y = BIT(RGB_PANEL_MIRROR_Y),
    ROTATE_MASK_MIRROR_X = BIT(RGB_PANEL_MIRROR_X),
} panel_rotate_mask_t;

__attribute__((always_inline))
static inline void copy_pixel_8bpp(uint8_t *to, const uint8_t *from)
{
    *to++ = *from++;
}

__attribute__((always_inline))
static inline void copy_pixel_16bpp(uint8_t *to, const uint8_t *from)
{
    *to++ = *from++;
    *to++ = *from++;
}

__attribute__((always_inline))
static inline void copy_pixel_24bpp(uint8_t *to, const uint8_t *from)
{
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
}

#define COPY_PIXEL_CODE_BLOCK(_bpp)                                                               \
    switch (rgb_panel->rotate_mask)                                                               \
    {                                                                                             \
    case 0:                                                                                       \
    {                                                                                             \
        uint8_t *to = fb + (y_start * h_res + x_start) * bytes_per_pixel;                         \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            memcpy(to, from, copy_bytes_per_line);                                                \
            to += bytes_per_line;                                                                 \
            from += copy_bytes_per_line;                                                          \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + y_start * bytes_per_line;                                                \
    }                                                                                             \
    break;                                                                                        \
    case ROTATE_MASK_MIRROR_X:                                                                    \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            uint32_t index = (y * h_res + (h_res - 1 - x_start)) * bytes_per_pixel;               \
            for (size_t x = x_start; x < x_end; x++)                                              \
            {                                                                                     \
                copy_pixel_##_bpp##bpp(to + index, from);                                         \
                index -= bytes_per_pixel;                                                         \
                from += bytes_per_pixel;                                                          \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + y_start * bytes_per_line;                                                \
        break;                                                                                    \
    case ROTATE_MASK_MIRROR_Y:                                                                    \
    {                                                                                             \
        uint8_t *to = fb + ((v_res - 1 - y_start) * h_res + x_start) * bytes_per_pixel;           \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            memcpy(to, from, copy_bytes_per_line);                                                \
            to -= bytes_per_line;                                                                 \
            from += copy_bytes_per_line;                                                          \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - y_end) * bytes_per_line;                                        \
    }                                                                                             \
    break;                                                                                        \
    case ROTATE_MASK_MIRROR_X | ROTATE_MASK_MIRROR_Y:                                             \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            uint32_t index = ((v_res - 1 - y) * h_res + (h_res - 1 - x_start)) * bytes_per_pixel; \
            for (size_t x = x_start; x < x_end; x++)                                              \
            {                                                                                     \
                copy_pixel_##_bpp##bpp(to + index, from);                                         \
                index -= bytes_per_pixel;                                                         \
                from += bytes_per_pixel;                                                          \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (y_end - y_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - y_end) * bytes_per_line;                                        \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY:                                                                     \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = (x * h_res + y) * bytes_per_pixel;                                   \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + x_start * bytes_per_line;                                                \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY | ROTATE_MASK_MIRROR_X:                                              \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = (x * h_res + h_res - 1 - y) * bytes_per_pixel;                       \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + x_start * bytes_per_line;                                                \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY | ROTATE_MASK_MIRROR_Y:                                              \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = ((v_res - 1 - x) * h_res + y) * bytes_per_pixel;                     \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - x_end) * bytes_per_line;                                        \
        break;                                                                                    \
    case ROTATE_MASK_SWAP_XY | ROTATE_MASK_MIRROR_X | ROTATE_MASK_MIRROR_Y:                       \
        for (int y = y_start; y < y_end; y++)                                                     \
        {                                                                                         \
            for (int x = x_start; x < x_end; x++)                                                 \
            {                                                                                     \
                uint32_t j = y * copy_bytes_per_line + x * bytes_per_pixel - offset;              \
                uint32_t i = ((v_res - 1 - x) * h_res + h_res - 1 - y) * bytes_per_pixel;         \
                copy_pixel_##_bpp##bpp(to + i, from + j);                                         \
            }                                                                                     \
        }                                                                                         \
        bytes_to_flush = (x_end - x_start) * bytes_per_line;                                      \
        flush_ptr = fb + (v_res - x_end) * bytes_per_line;                                        \
        break;                                                                                    \
    default:                                                                                      \
        break;                                                                                    \
    }

#ifdef __cplusplus
}
#endif
