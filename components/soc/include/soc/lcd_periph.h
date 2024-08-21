/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCDCAM_I80_LCD_SUPPORTED
typedef struct {
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCDCAM_I80_BUS_WIDTH];
        const int cs_sig;
        const int dc_sig;
        const int wr_sig;
    } buses[SOC_LCDCAM_I80_NUM_BUSES];
} lcd_i80_signal_conn_t;

extern const lcd_i80_signal_conn_t lcd_periph_i80_signals;
#endif // SOC_LCDCAM_I80_LCD_SUPPORTED

#if SOC_LCDCAM_RGB_LCD_SUPPORTED
typedef struct {
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCDCAM_RGB_DATA_WIDTH];
        const int hsync_sig;
        const int vsync_sig;
        const int pclk_sig;
        const int de_sig;
    } panels[SOC_LCDCAM_RGB_NUM_PANELS];
} lcd_rgb_signal_conn_t;

extern const lcd_rgb_signal_conn_t lcd_periph_rgb_signals;
#endif // SOC_LCDCAM_RGB_LCD_SUPPORTED

#if SOC_I2S_SUPPORTS_LCD_CAMERA
typedef struct {
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_I2S_MAX_DATA_WIDTH];
        const int wr_sig;
    } buses[SOC_LCD_I80_BUSES];
} lcd_i2s_signal_conn_t;

extern const lcd_i2s_signal_conn_t lcd_periph_i2s_signals;
#endif // SOC_I2S_SUPPORTS_LCD_CAMERA

#ifdef __cplusplus
}
#endif
