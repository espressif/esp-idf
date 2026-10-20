/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: LicenseRef-Included
 *
 * Zigbee light driver example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#include <stdbool.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/* light intensity level */
#define LIGHT_DEFAULT_ON  1
#define LIGHT_DEFAULT_OFF 0

/* LED strip configuration */
#define CONFIG_EXAMPLE_STRIP_LED_GPIO   CONFIG_GPIO_LED_ON_DEVKIT
#define CONFIG_EXAMPLE_STRIP_LED_NUMBER 1

/** Convert Hue,Saturation,V to RGB
 * RGB - [0..0xffff]
 * hue - [0..0xff]
 * Sat - [0..0xff]
 * V always = (ZB_UINT16_MAX-1)
 */
#define HSV_to_RGB(h, s, v, r, g, b)                                                 \
    {                                                                                \
        uint8_t i;                                                                   \
        uint8_t sector = UINT8_MAX / 6;                                              \
        float   f, p, q, t;                                                          \
        if (s == 0) { /* achromatic (grey)*/                                         \
            r = g = b = (v);                                                         \
        } else {                                                                     \
            i = h / sector; /* sector 0 to 5 */                                      \
            f = h % sector; /* factorial part of h*/                                 \
            p = (float)(v * (1.0 - (float)s / UINT8_MAX));                           \
            q = (float)(v * (1.0 - (float)s / UINT8_MAX * f / (float)sector));       \
            t = (float)(v * (1.0 - (float)s / UINT8_MAX * (1 - f / (float)sector))); \
            switch (i) {                                                             \
            case 0:                                                                  \
                r = (v);                                                             \
                g = t;                                                               \
                b = p;                                                               \
                break;                                                               \
            case 1:                                                                  \
                r = q;                                                               \
                g = (v);                                                             \
                b = p;                                                               \
                break;                                                               \
            case 2:                                                                  \
                r = p;                                                               \
                g = (v);                                                             \
                b = t;                                                               \
                break;                                                               \
            case 3:                                                                  \
                r = p;                                                               \
                g = q;                                                               \
                b = (v);                                                             \
                break;                                                               \
            case 4:                                                                  \
                r = t;                                                               \
                g = p;                                                               \
                b = (v);                                                             \
                break;                                                               \
            case 5:                                                                  \
            default:                                                                 \
                r = (v);                                                             \
                g = p;                                                               \
                b = q;                                                               \
                break;                                                               \
            }                                                                        \
        }                                                                            \
    }

#define XYZ_to_RGB(X, Y, Z, r, g, b)                                    \
    {                                                                   \
        r = (float)(3.240479 * (X)-1.537150 * (Y)-0.498535 * (Z));      \
        g = (float)(-0.969256 * (X) + 1.875992 * (Y) + 0.041556 * (Z)); \
        b = (float)(0.055648 * (X)-0.204043 * (Y) + 1.057311 * (Z));    \
        if (r > 1) {                                                    \
            r = 1;                                                      \
        }                                                               \
        if (g > 1) {                                                    \
            g = 1;                                                      \
        }                                                               \
        if (b > 1) {                                                    \
            b = 1;                                                      \
        }                                                               \
    }

/**
 * @brief Set light power (on/off).
 *
 * @param power The light power value to be set
 */
void light_driver_set_power(bool power);

/**
 * @brief color light driver init, be invoked where you want to use color light
 *
 * @param power power on/off
 */
void light_driver_init(bool power);

/**
 * @brief Set light level
 *
 * @param level The light level value to be set
 */
void light_driver_set_level(uint8_t level);

/**
 * @brief Set light color from RGB
 *
 * @param red The red color value to be set
 * @param green The green color value to be set
 * @param blue The blue color value to be set
 */
void light_driver_set_color_RGB(uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Set light color from color xy
 *
 * @param color_current_x The color x value to be set
 * @param color_current_y The color y value to be set
 */
void light_driver_set_color_xy(uint16_t color_current_x, uint16_t color_current_y);

/**
 * @brief Set light color from hue saturation
 *
 * @param hue The hue value to be set
 * @param sat The saturation value to be set
 */
void light_driver_set_color_hue_sat(uint8_t hue, uint8_t sat);

#ifdef __cplusplus
} // extern "C"
#endif
