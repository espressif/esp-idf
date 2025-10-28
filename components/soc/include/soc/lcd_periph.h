/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/soc_caps_full.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCDCAM_I80_LCD_SUPPORTED
typedef struct {
    struct {
        const shared_periph_module_t module;
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
        const shared_periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCDCAM_RGB_DATA_WIDTH];
        const int hsync_sig;
        const int vsync_sig;
        const int pclk_sig;
        const int de_sig;
        const int disp_sig;
    } panels[SOC_LCDCAM_RGB_NUM_PANELS];
} lcd_rgb_signal_conn_t;

extern const lcd_rgb_signal_conn_t lcd_periph_rgb_signals;
#endif // SOC_LCDCAM_RGB_LCD_SUPPORTED

#if SOC_I2S_SUPPORTS_LCD_CAMERA
typedef struct {
    struct {
        const int irq_id;
        const int data_sigs[SOC_MODULE_ATTR(I2S, MAX_DATA_WIDTH)];
        const int wr_sig;
    } buses[SOC_LCD_I80_BUSES];
} lcd_i2s_signal_conn_t;

extern const lcd_i2s_signal_conn_t lcd_periph_i2s_signals;
#endif // SOC_I2S_SUPPORTS_LCD_CAMERA

#ifdef __cplusplus
}
#endif
