/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"

#if SOC_LCDCAM_LCD_SUPPORT_SLEEP_RETENTION
#include "soc/retention_periph_defs.h"
#include "soc/lcd_cam_reg.h"
#endif

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
        const int disp_sig;
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

#if SOC_PAU_SUPPORTED && SOC_LCDCAM_I80_LCD_SUPPORTED
// Only LCDCAM I80 LCD supports sleep retention
typedef struct {
    const periph_retention_module_t retention_module;
    const regdma_entries_config_t *regdma_entry_array;
    uint32_t array_size;
} soc_i80_lcd_retention_info_t;

extern const soc_i80_lcd_retention_info_t soc_i80_lcd_retention_info[SOC_LCDCAM_I80_NUM_BUSES];
#endif // SOC_PAU_SUPPORTED && SOC_LCDCAM_I80_LCD_SUPPORTED

#ifdef __cplusplus
}
#endif
