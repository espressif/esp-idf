/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file video_pattern.c
 * @brief  Video pattern generation and processing implementation
 *
 *         This file implements functions for generating test video patterns
 *         and displaying conversion results for H264 codec testing.
 */

#include "video_pattern.h"

#define GET_RGB565_R(x) (((((x) >> 11) & 0x1F) << 3) | (((x) >> 8) & 0x07))
#define GET_RGB565_G(x) (((((x) >> 5) & 0x3F) << 2) | (((x) >> 3) & 0x03))
#define GET_RGB565_B(x) ((((x) & 0x1F) << 3) | (((x) & 0x1C) >> 2))
#define SWAP_EDIAN(x)   (((x) << 8) | ((x) >> 8))
#define CLAMP(x)        ((x) < 0 ? 0 : ((x) > 255 ? 255 : (x)))

/**
 * @brief  YUV pixel structure
 */
typedef struct {
    uint8_t y;  /*!< Y (luma) component */
    uint8_t u;  /*!< U (chroma) component */
    uint8_t v;  /*!< V (chroma) component */
} yuv_pixel_t;

/**
 * @brief  RGB888 pixel structure
 */
typedef struct {
    uint8_t r;  /*!< Red component */
    uint8_t g;  /*!< Green component */
    uint8_t b;  /*!< Blue component */
} rgb888_pixel_t;

static void yuv_to_rgb(uint8_t y, uint8_t u, uint8_t v, rgb888_pixel_t *pixel)
{
    int c = y - 16;
    int d = u - 128;
    int e = v - 128;
    int r_temp = (298 * c + 409 * e + 128) >> 8;            // R = Y + 1.403 * (V-128)
    int g_temp = (298 * c - 100 * d - 208 * e + 128) >> 8;  // G = Y - 0.344 * (U-128) - 0.714 * (V-128)
    int b_temp = (298 * c + 516 * d + 128) >> 8;            // B = Y + 1.770 * (U-128)
    pixel->r = CLAMP(r_temp);
    pixel->g = CLAMP(g_temp);
    pixel->b = CLAMP(b_temp);
}

static void get_pixel(pattern_info_t *info, rgb888_pixel_t *pixel, int x, int y)
{
    uint8_t *data = info->pixel;
    switch (info->format_id) {
    case ESP_H264_RAW_FMT_YUYV: {
        x = (x >> 1 << 1);
        uint8_t *yuyv = data + y * info->res.width * 2 + x * 2;
        yuv_to_rgb(yuyv[0], yuyv[1], yuyv[3], pixel);
        break;
    }
    case ESP_H264_RAW_FMT_I420: {
        uint8_t *py = data + y * info->res.width + x;
        y >>= 1;
        x >>= 1;
        uint8_t *pu = data + info->res.height * info->res.width + y * info->res.width / 2 + x;
        uint8_t *pv = data + info->res.height * info->res.width * 5 / 4 + y * info->res.width / 2 + x;
        yuv_to_rgb(py[0], pu[0], pv[0], pixel);
        break;
    }
    case ESP_H264_RAW_FMT_O_UYY_E_VYY: {
        uint8_t *uyy = data + (y >> 1) * info->res.width * 3 + (x >> 1) * 3;
        uint8_t *vyy = uyy + info->res.width * 3 / 2;
        uint8_t y_pixel = (y & 1) ? vyy[1 + (x & 1)] : uyy[1 + (x & 1)];
        yuv_to_rgb(y_pixel, uyy[0], vyy[0], pixel);
        break;
    }
    default:
        break;
    }
}

esp_err_t gen_pattern_color_bar(pattern_info_t *info)
{
    uint8_t *pixel = info->pixel;
    bool vertical = info->vertical;
    uint8_t n = info->bar_count;

    switch (info->format_id) {
    case ESP_H264_RAW_FMT_I420: {
        yuv_pixel_t *color = (yuv_pixel_t *)malloc(n * sizeof(yuv_pixel_t));
        if (color == NULL) {
            return ESP_FAIL;
        }
        for (int i = 0; i < n; i++) {
            color[i].y = (uint8_t)(rand() & 0xFF);
            color[i].u = (uint8_t)(rand() & 0xFF);
            color[i].v = (uint8_t)(rand() & 0xFF);
        }
        if (vertical) {
            uint32_t bar_w = (info->res.width / n) >> 1 << 1;
            uint32_t last_bar_w = info->res.width - bar_w * (n - 1);
            // Fill Y firstly
            for (int y = 0; y < info->res.height; y++) {
                for (int i = 0; i < n; i++) {
                    uint32_t bytes = (i == n - 1 ? last_bar_w : bar_w);
                    memset(pixel, color[i].y, bytes);
                    pixel += bytes;
                }
            }
            // Fill U
            for (int y = 0; y < info->res.height >> 1; y++) {
                for (int i = 0; i < n; i++) {
                    uint32_t bytes = (i == n - 1 ? last_bar_w : bar_w) >> 1;
                    memset(pixel, color[i].u, bytes);
                    pixel += bytes;
                }
            }
            // Fill V
            for (int y = 0; y < info->res.height >> 1; y++) {
                for (int i = 0; i < n; i++) {
                    uint32_t bytes = (i == n - 1 ? last_bar_w : bar_w) >> 1;
                    memset(pixel, color[i].v, bytes);
                    pixel += bytes;
                }
            }
        } else {
            uint32_t bar_h = (info->res.height / n) >> 1 << 1;
            uint32_t last_bar_h = info->res.height - bar_h * (n - 1);
            // Fill Y firstly
            for (int i = 0; i < n; i++) {
                uint32_t bytes = (i == n - 1 ? last_bar_h : bar_h) * info->res.width;
                memset(pixel, color[i].y, bytes);
                pixel += bytes;
            }
            // Fill U
            for (int i = 0; i < n; i++) {
                uint32_t bytes = (i == n - 1 ? last_bar_h : bar_h) * info->res.width >> 2;
                memset(pixel, color[i].u, bytes);
                pixel += bytes;
            }
            // Fill V
            for (int i = 0; i < n; i++) {
                uint32_t bytes = (i == n - 1 ? last_bar_h : bar_h) * info->res.width >> 2;
                memset(pixel, color[i].v, bytes);
                pixel += bytes;
            }
        }
        free(color);
    } break;
    case ESP_H264_RAW_FMT_YUYV: {
        yuv_pixel_t *color = (yuv_pixel_t *)malloc(n * sizeof(yuv_pixel_t));
        if (color == NULL) {
            return ESP_FAIL;
        }
        for (int i = 0; i < n; i++) {
            color[i].y = (uint8_t)(rand() & 0xFF);
            color[i].u = (uint8_t)(rand() & 0xFF);
            color[i].v = (uint8_t)(rand() & 0xFF);
        }
        if (vertical) {
            uint32_t bar_w = (info->res.width / n) >> 1 << 1;
            // Fill Y firstly
            for (int y = 0; y < info->res.height; y++) {
                int bar_filled = 0;
                int i = 0;
                for (int x = 0; x < (info->res.width >> 1); x++) {
                    *pixel++ = color[i].y;
                    *pixel++ = color[i].u;
                    *pixel++ = color[i].y;
                    *pixel++ = color[i].v;
                    bar_filled += 2;
                    if (bar_filled >= bar_w) {
                        bar_filled = 0;
                        if (i < n - 1) {
                            i++;
                        }
                    }
                }
            }
        } else {
            uint32_t bar_h = (info->res.height / n) >> 1 << 1;
            uint32_t last_bar_h = info->res.height - bar_h * (n - 1);
            // Fill Y firstly
            for (int i = 0; i < n; i++) {
                uint32_t bytes = (i == n - 1 ? last_bar_h : bar_h) * info->res.width * 3 / 2;
                while (bytes > 0) {
                    *pixel++ = color[i].y;
                    *pixel++ = color[i].u;
                    *pixel++ = color[i].y;
                    *pixel++ = color[i].v;
                    bytes -= 3;
                }
            }
        }
        free(color);
    } break;
    case ESP_H264_RAW_FMT_O_UYY_E_VYY: {
        yuv_pixel_t *color = (yuv_pixel_t *)malloc(n * sizeof(yuv_pixel_t));
        if (color == NULL) {
            return ESP_FAIL;
        }
        for (int i = 0; i < n; i++) {
            color[i].y = (uint8_t)(rand() & 0xFF);
            color[i].u = (uint8_t)(rand() & 0xFF);
            color[i].v = (uint8_t)(rand() & 0xFF);
        }
        if (vertical) {
            uint32_t bar_w = (info->res.width / n) >> 1 << 1;
            // Fill Y firstly
            for (int y = 0; y < (info->res.height >> 1); y++) {
                int bar_filled = 0;
                int i = 0;
                for (int x = 0; x < (info->res.width >> 1); x++) {
                    *pixel++ = color[i].u;
                    *pixel++ = color[i].y;
                    *pixel++ = color[i].y;
                    bar_filled += 2;
                    if (bar_filled >= bar_w) {
                        bar_filled = 0;
                        if (i < n - 1) {
                            i++;
                        }
                    }
                }
                bar_filled = 0;
                i = 0;
                for (int x = 0; x < (info->res.width >> 1); x++) {
                    *pixel++ = color[i].v;
                    *pixel++ = color[i].y;
                    *pixel++ = color[i].y;
                    bar_filled += 2;
                    if (bar_filled >= bar_w) {
                        bar_filled = 0;
                        if (i < n - 1) {
                            i++;
                        }
                    }
                }
            }
        } else {
            uint32_t bar_h = (info->res.height / n) >> 1 << 1;
            uint32_t last_bar_h = info->res.height - bar_h * (n - 1);
            // Fill Y firstly
            for (int i = 0; i < n; i++) {
                uint32_t height = (i == n - 1 ? last_bar_h : bar_h);
                uint32_t width = info->res.width >> 1;
                for (int y = 0; y < (height >> 1); y++) {
                    for (int x = 0; x < width; x++) {
                        *pixel++ = color[i].u;
                        *pixel++ = color[i].y;
                        *pixel++ = color[i].y;
                    }
                    for (int x = 0; x < width; x++) {
                        *pixel++ = color[i].v;
                        *pixel++ = color[i].y;
                        *pixel++ = color[i].y;
                    }
                }
            }
        }
        free(color);
    }
    default:
        break;
    }
    return ESP_OK;
}

void draw_convert_result(pattern_info_t *a, pattern_info_t *b)
{
    if (a->bar_count == 0) {
        return;
    }
    printf("\n");
    int y = 0;
    rgb888_pixel_t block_start = {};
    rgb888_pixel_t block_end = {};

    if (a->bar_count == b->bar_count) {
        int n = a->bar_count;
        uint32_t bar_w = (a->res.width / n) >> 1 << 1;
        uint32_t last_bar_w = a->res.width - bar_w * (n - 1);
        uint32_t bar_h = a->res.height / n;

        uint32_t bar_w_b = (b->res.width / n) >> 1 << 1;
        uint32_t last_bar_w_b = b->res.width - bar_w_b * (n - 1);
        uint32_t bar_h_b = b->res.height / n;
        int y_b = 0;

        for (int col = 0; col < n; col++) {
            int x = 0;
            for (int row = 0; row < n; row++) {
                get_pixel(a, &block_start, x, y);
                x += (row == n - 1 ? last_bar_w : bar_w);
                get_pixel(a, &block_end, x - 1, y);
                printf("\033[48;2;%d;%d;%dm%c\033[0m", block_start.r, block_start.g, block_start.b, ' ');
                printf("\033[48;2;%d;%d;%dm%c\033[0m", block_end.r, block_end.g, block_end.b, ' ');
            }
            if (b->pixel) {
                printf("  |  ");
                x = 0;
                for (int row = 0; row < n; row++) {
                    get_pixel(b, &block_start, x, y_b);
                    x += (row == n - 1 ? last_bar_w_b : bar_w_b);
                    get_pixel(b, &block_end, x - 1, y_b);
                    printf("\033[48;2;%d;%d;%dm%c\033[0m", block_start.r, block_start.g, block_start.b, ' ');
                    printf("\033[48;2;%d;%d;%dm%c\033[0m", block_end.r, block_end.g, block_end.b, ' ');
                }
                y_b += bar_h_b;
            }
            y += bar_h;
            printf("\n");
        }
    } else {
        // Draw image
        int n = a->bar_count;
        uint32_t bar_w = (a->res.width / n) >> 1 << 1;
        uint32_t last_bar_w = a->res.width - bar_w * (n - 1);
        uint32_t bar_h = a->res.height / n;

        printf("A:\n");
        for (int col = 0, y = 0; col < n; col++, y += bar_h) {
            int x = 0;
            printf("║ ");
            for (int row = 0; row < n; row++) {
                get_pixel(a, &block_start, x, y);
                x += (row == n - 1 ? last_bar_w : bar_w);
                get_pixel(a, &block_end, x - 1, y);
                printf("\033[48;2;%d;%d;%dm%c\033[0m", block_start.r, block_start.g, block_start.b, ' ');
                printf("\033[48;2;%d;%d;%dm%c\033[0m", block_end.r, block_end.g, block_end.b, ' ');
            }
            printf("\n");
        }
        if (b->pixel) {
            uint32_t bar_w = (b->res.width / n) >> 1 << 1;
            uint32_t last_bar_w = b->res.width - bar_w * (n - 1);
            uint32_t bar_h = b->res.height / n;

            printf("B:\n");
            for (int col = 0, y = 0; col < n; col++, y += bar_h) {
                int x = 0;
                printf("║ ");
                for (int row = 0; row < n; row++) {
                    get_pixel(b, &block_start, x, y);
                    x += (row == n - 1 ? last_bar_w : bar_w);
                    get_pixel(b, &block_end, x - 1, y);
                    printf("\033[48;2;%d;%d;%dm%c\033[0m", block_start.r, block_start.g, block_start.b, ' ');
                    printf("\033[48;2;%d;%d;%dm%c\033[0m", block_end.r, block_end.g, block_end.b, ' ');
                }
                printf("\n");
            }
            printf("\n");
        }
    }
}
