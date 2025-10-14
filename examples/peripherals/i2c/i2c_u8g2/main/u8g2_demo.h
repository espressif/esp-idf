/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/**
 * @file u8g2_demo.h
 * @brief U8G2 Demo Functions Header
 *
 * This file contains declarations for all U8G2 demo functions that showcase
 * various display capabilities including text rendering, geometric shapes,
 * pixel manipulation, progress bars, animations, and bitmap display.
 *
 * @note All demo functions expect an initialized u8g2 display object and
 *       will automatically handle buffer clearing, drawing operations, and
 *       buffer transmission to the display.
 */

#pragma once

#include "u8g2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Text display demonstration
 *
 * Showcases different font sizes and text rendering capabilities of U8G2 library.
 * Displays text with large, medium, and small fonts to demonstrate font variety.
 *
 * Duration: ~3 seconds
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 *
 * @attention Ensure u8g2 object is properly initialized before calling this function
 */
void demo_text_display(u8g2_t *u8g2);

/**
 * @brief Geometric shapes demonstration
 *
 * Displays various geometric shapes including filled and outlined rectangles,
 * circles, lines, and triangles to showcase U8G2's drawing primitives.
 *
 * Duration: ~3 seconds
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 *
 * @attention Ensure u8g2 object is properly initialized before calling this function
 */
void demo_shapes(u8g2_t *u8g2);

/**
 * @brief Animated progress bar demonstration
 *
 * Creates an animated progress bar that fills from 0% to 100% in 10% increments.
 * Demonstrates dynamic graphics updates and percentage text display.
 *
 * Duration: ~3.2 seconds (11 frames * 200ms + 1s delay)
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 *
 * @attention Ensure u8g2 object is properly initialized before calling this function
 */
void demo_progress_bar(u8g2_t *u8g2);

/**
 * @brief Bouncing ball animation demonstration
 *
 * Creates a bouncing ball animation with collision detection against display
 * boundaries. Demonstrates real-time animation capabilities and physics simulation.
 *
 * Duration: ~6.4 seconds (80 frames * 80ms)
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 *
 * @attention Ensure u8g2 object is properly initialized before calling this function
 */
void demo_animation(u8g2_t *u8g2);

/**
 * @brief Bitmap display demonstration
 *
 * Displays custom bitmap images including a 16x16 pixel smiley face and
 * smaller 8x8 pixel icons. Demonstrates bitmap rendering capabilities.
 *
 * Duration: ~3 seconds
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 *
 * @attention Ensure u8g2 object is properly initialized before calling this function
 */
void demo_bitmap(u8g2_t *u8g2);

/**
 * @brief Pixel manipulation demonstration
 *
 * Showcases individual pixel drawing capabilities by creating dot patterns
 * and pseudo-random pixel arrangements. Demonstrates low-level pixel control.
 *
 * Duration: ~3 seconds
 *
 * @param[in] u8g2 Pointer to initialized u8g2 display object
 *
 * @attention Ensure u8g2 object is properly initialized before calling this function
 */
void demo_pixels(u8g2_t *u8g2);

#ifdef __cplusplus
}
#endif
