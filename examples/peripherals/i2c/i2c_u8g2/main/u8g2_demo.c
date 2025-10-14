/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/**
 * @file u8g2_demo.c
 * @brief U8G2 Demo Functions Implementation
 *
 * This file contains all U8G2 demo function implementations that showcase
 * various display capabilities including text rendering, geometric shapes,
 * pixel manipulation, progress bars, animations, and bitmap display.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "u8g2_demo.h"
#include "u8g2.h"

static const char *TAG = "DEMO";

/**
 * @brief Text display demonstration
 *
 * This demo showcases different font sizes and text rendering capabilities
 * of the U8G2 library. It displays text with large, medium, and small fonts.
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 */
void demo_text_display(u8g2_t *u8g2)
{
    ESP_LOGI(TAG, "Text Display");

    u8g2_ClearBuffer(u8g2);

    /* Large title font */
    u8g2_SetFont(u8g2, u8g2_font_ncenB14_tr);
    u8g2_DrawStr(u8g2, 0, 16, "U8G2 Demo");

    /* Medium font */
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, 0, 32, "ESP32 + Display");

    /* Small font */
    u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
    u8g2_DrawStr(u8g2, 0, 44, "Multiple fonts");
    u8g2_DrawStr(u8g2, 0, 54, "Text demo");

    u8g2_SendBuffer(u8g2);
    vTaskDelay(pdMS_TO_TICKS(3000));    /* delay for showing static display */
}

/**
 * @brief Geometric shapes demonstration
 *
 * This demo displays various geometric shapes including filled and outlined
 * rectangles, circles, lines, and triangles to showcase U8G2's drawing primitives.
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 */
void demo_shapes(u8g2_t *u8g2)
{
    ESP_LOGI(TAG, "Geometric Shapes");

    u8g2_ClearBuffer(u8g2);

    /* Title */
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, 35, 12, "Shapes");

    /* Filled rectangle */
    u8g2_DrawBox(u8g2, 10, 20, 20, 15);

    /* Outlined rectangle */
    u8g2_DrawFrame(u8g2, 35, 20, 20, 15);

    /* Circle outline */
    u8g2_DrawCircle(u8g2, 70, 27, 8, U8G2_DRAW_ALL);

    /* Filled circle */
    u8g2_DrawDisc(u8g2, 95, 27, 6, U8G2_DRAW_ALL);

    /* Lines */
    u8g2_DrawLine(u8g2, 10, 45, 50, 45);
    u8g2_DrawLine(u8g2, 10, 50, 30, 60);
    u8g2_DrawLine(u8g2, 30, 50, 50, 60);

    /* Triangle */
    u8g2_DrawTriangle(u8g2, 70, 45, 85, 60, 55, 60);

    u8g2_SendBuffer(u8g2);
    vTaskDelay(pdMS_TO_TICKS(3000));    /* delay for showing static display */
}

/**
 * @brief Animated progress bar demonstration
 *
 * This demo creates an animated progress bar that fills from 0% to 100%
 * in 10% increments, demonstrating dynamic graphics updates.
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 */
void demo_progress_bar(u8g2_t *u8g2)
{
    ESP_LOGI(TAG, "Progress Bar");

    for (int progress = 0; progress <= 100; progress += 10) {
        u8g2_ClearBuffer(u8g2);

        /* Title */
        u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
        u8g2_DrawStr(u8g2, 35, 16, "Progress");

        /* Progress bar frame */
        u8g2_DrawFrame(u8g2, 10, 25, 108, 12);

        /* Progress bar fill */
        int fill_width = (progress * 106) / 100;
        u8g2_DrawBox(u8g2, 11, 26, fill_width, 10);

        /* Percentage text */
        char progress_str[16];
        snprintf(progress_str, sizeof(progress_str), "%d%%", progress);
        u8g2_SetFont(u8g2, u8g2_font_5x7_tr);
        u8g2_DrawStr(u8g2, 55, 50, progress_str);

        u8g2_SendBuffer(u8g2);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/**
 * @brief Bouncing ball animation demonstration
 *
 * This demo creates a bouncing ball animation with collision detection
 * against the display boundaries, demonstrating real-time animation capabilities.
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 */
void demo_animation(u8g2_t *u8g2)
{
    ESP_LOGI(TAG, "Animation Effects");

    int ball_x = 15, ball_y = 30;  /*!< Ball position coordinates */
    int vel_x = 2, vel_y = 1;      /*!< Ball velocity components */

    for (int frame = 0; frame < 80; frame++) {
        u8g2_ClearBuffer(u8g2);

        /* Title */
        u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
        u8g2_DrawStr(u8g2, 40, 12, "Animation");

        /* Animation boundary */
        u8g2_DrawFrame(u8g2, 5, 15, 118, 45);

        /* Bouncing ball */
        u8g2_DrawDisc(u8g2, ball_x, ball_y, 4, U8G2_DRAW_ALL);

        /* Update ball position */
        ball_x += vel_x;
        ball_y += vel_y;

        /* Collision detection and velocity reversal */
        if (ball_x <= 9 || ball_x >= 119) {
            vel_x = -vel_x;
        }
        if (ball_y <= 19 || ball_y >= 56) {
            vel_y = -vel_y;
        }

        u8g2_SendBuffer(u8g2);
        vTaskDelay(pdMS_TO_TICKS(80));
    }
}

/**
 * @brief Bitmap display demonstration
 *
 * This demo displays custom bitmap images including a 16x16 pixel smiley face
 * and smaller 8x8 pixel icons, demonstrating bitmap rendering capabilities.
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 */
void demo_bitmap(u8g2_t *u8g2)
{
    ESP_LOGI(TAG, "Bitmap Display");

    const uint8_t esp_bitmap[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBB,
        0x01, 0x00, 0x00, 0x00, 0x01, 0xDA, 0x0E, 0x00, 0x00, 0x40, 0x02, 0x70, 0x05, 0x00, 0x80, 0x43,
        0xCC, 0xE0, 0x37, 0x00, 0x80, 0x51, 0x3B, 0x07, 0xC4, 0x00, 0x40, 0xA0, 0x4F, 0x0D, 0x0E, 0x01,
        0xE0, 0x90, 0x75, 0x0D, 0x94, 0x01, 0x20, 0x70, 0xC3, 0x33, 0xB0, 0x04, 0x10, 0x00, 0x00, 0x5D,
        0xE0, 0x05, 0x30, 0x00, 0xB0, 0xF7, 0x20, 0x0E, 0x30, 0x1A, 0x00, 0x97, 0x86, 0x00, 0x10, 0x6E,
        0x05, 0x64, 0x89, 0x1B, 0x00, 0x02, 0x19, 0xE0, 0x0B, 0x3B, 0xCC, 0xB7, 0x4D, 0x30, 0x0C, 0x0C,
        0x04, 0xB9, 0x7C, 0x61, 0x03, 0x3C, 0x8C, 0xEB, 0xC9, 0x43, 0x79, 0x1C, 0x8C, 0xC0, 0x3F, 0x8F,
        0x18, 0x38, 0x08, 0x82, 0xCE, 0x0C, 0x2F, 0x68, 0x04, 0x00, 0x08, 0x3F, 0xFA, 0x61, 0x04, 0x00,
        0xA0, 0x3C, 0xB4, 0x50, 0x8C, 0x2D, 0x81, 0x58, 0xD8, 0x23, 0xE0, 0xDE, 0x06, 0xB3, 0x10, 0x01,
        0xAC, 0xDB, 0x0E, 0x7C, 0x78, 0x06, 0x44, 0xA4, 0x09, 0xA8, 0x41, 0x05, 0xC4, 0x75, 0x69, 0xB8,
        0x11, 0x00, 0x8C, 0x0D, 0xC2, 0xEC, 0x31, 0x0F, 0x98, 0x41, 0x6C, 0xF8, 0x42, 0x0A, 0x10, 0x00,
        0x3E, 0xF8, 0x63, 0x08, 0x20, 0x00, 0xB8, 0xD0, 0xC2, 0x09, 0x20, 0x30, 0xAC, 0x61, 0x82, 0x0E,
        0x10, 0xA0, 0xE0, 0xE2, 0xC3, 0x07, 0x60, 0x70, 0xF0, 0x01, 0xC4, 0x06, 0x20, 0xB4, 0x78, 0x51,
        0xC4, 0x03, 0xC0, 0x68, 0xE8, 0x00, 0xC7, 0x03, 0x80, 0x31, 0xE0, 0x6D, 0x05, 0x00, 0x00, 0x21,
        0x40, 0x22, 0x07, 0x00, 0x00, 0x00, 0x30, 0x72, 0x00, 0x00, 0x00, 0x02, 0x40, 0xE2, 0x03, 0x03,
        0x00, 0x20, 0x40, 0xD2, 0x40, 0x03, 0x00, 0x40, 0x00, 0x7F, 0xE0, 0x00, 0x00, 0x80, 0x03, 0x00,
        0x7C, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x78, 0xBC, 0x04, 0x00, 0x00, 0x00,
        0x20, 0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    u8g2_ClearBuffer(u8g2);

    /* Title */
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, 45, 12, "Bitmap");

    /* Display various sized bitmaps */
    u8g2_DrawXBM(u8g2, 40, 16, 48, 48, esp_bitmap);

    u8g2_SendBuffer(u8g2);
    vTaskDelay(pdMS_TO_TICKS(3000));    /* delay for showing static display */
}

/**
 * @brief Pixel manipulation demonstration
 *
 * This demo showcases individual pixel drawing capabilities by creating
 * dot patterns and pseudo-random pixel arrangements.
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 */
void demo_pixels(u8g2_t *u8g2)
{
    ESP_LOGI(TAG, "Pixel Operations");

    u8g2_ClearBuffer(u8g2);

    /* Title */
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, 45, 12, "Pixels");

    /* Draw pixel grid pattern */
    for (int x = 20; x < 108; x += 4) {
        for (int y = 20; y < 60; y += 4) {
            if ((x + y) % 8 == 0) {
                u8g2_DrawPixel(u8g2, x, y);
            }
        }
    }

    /* Draw pseudo-random pixels */
    for (int i = 0; i < 50; i++) {
        int x = 10 + (i * 17) % 108;
        int y = 16 + (i * 11) % 40;
        u8g2_DrawPixel(u8g2, x, y);
    }

    u8g2_SendBuffer(u8g2);
    vTaskDelay(pdMS_TO_TICKS(3000));    /* delay for showing static display */
}
