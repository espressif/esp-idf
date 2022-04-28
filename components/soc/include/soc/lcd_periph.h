/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCDCAM_SUPPORTED
typedef struct {
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCD_I80_BUS_WIDTH];
        const int cs_sig;
        const int dc_sig;
        const int wr_sig;
    } buses[SOC_LCD_I80_BUSES];
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCD_RGB_DATA_WIDTH];
        const int hsync_sig;
        const int vsync_sig;
        const int pclk_sig;
        const int de_sig;
    } panels[SOC_LCD_RGB_PANELS];
} lcd_signal_conn_t;

#endif // SOC_LCDCAM_SUPPORTED

#if SOC_I2S_LCD_I80_VARIANT
typedef struct {
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCD_I80_BUS_WIDTH];
        const int wr_sig;
    } buses[SOC_LCD_I80_BUSES];
} lcd_signal_conn_t;
#endif // SOC_I2S_LCD_I80_VARIANT

extern const lcd_signal_conn_t lcd_periph_signals;

#ifdef __cplusplus
}
#endif
