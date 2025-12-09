/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/regdma.h"
#if SOC_HAS(I2S)
#include "hal/i2s_ll.h"
#endif
#if SOC_HAS(LCDCAM_I80_LCD) || SOC_HAS(LCDCAM_RGB_LCD)
#include "hal/lcd_ll.h"
#endif
#if SOC_HAS(PAU)
#include "soc/retention_periph_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(LCDCAM_RGB_LCD)
typedef struct {
    const shared_periph_module_t module;
    const int irq_id;
    const int data_sigs[LCD_LL_GET(RGB_BUS_WIDTH)];
    const int hsync_sig;
    const int vsync_sig;
    const int pclk_sig;
    const int de_sig;
    const int disp_sig;
} soc_lcd_rgb_signal_desc_t;

extern const soc_lcd_rgb_signal_desc_t soc_lcd_rgb_signals[LCD_LL_GET(RGB_PANEL_NUM)];
#endif // SOC_HAS(LCDCAM_RGB_LCD)

#if SOC_HAS(LCDCAM_I80_LCD)
typedef struct {
    const shared_periph_module_t module;
    const int irq_id;
    const int data_sigs[LCD_LL_GET(I80_BUS_WIDTH)];
    const int cs_sig;
    const int dc_sig;
    const int wr_sig;
} soc_lcd_i80_signal_desc_t;

extern const soc_lcd_i80_signal_desc_t soc_lcd_i80_signals[LCD_LL_GET(I80_BUS_NUM)];
#endif // SOC_HAS(LCDCAM_I80_LCD)

#if SOC_HAS(I2S_I80_LCD)
typedef struct {
    const int irq_id;
    const int data_sigs[I2S_LL_GET(BUS_WIDTH)];
    const int wr_sig;
} soc_lcd_i2s_signal_desc_t;

extern const soc_lcd_i2s_signal_desc_t soc_lcd_i2s_signals[I2S_LL_GET(INST_NUM)];
#endif // SOC_HAS(I2S_I80_LCD)

#ifdef __cplusplus
}
#endif
