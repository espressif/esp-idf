/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/lcd_periph.h"
#include "soc/gpio_sig_map.h"

const soc_lcd_i80_signal_desc_t soc_lcd_i80_signals[1] = {
    [0] = {
        .module = PERIPH_LCD_CAM_MODULE,
        .irq_id = ETS_LCD_CAM_INTR_SOURCE,
        .data_sigs = {
            LCD_DATA_OUT0_IDX,
            LCD_DATA_OUT1_IDX,
            LCD_DATA_OUT2_IDX,
            LCD_DATA_OUT3_IDX,
            LCD_DATA_OUT4_IDX,
            LCD_DATA_OUT5_IDX,
            LCD_DATA_OUT6_IDX,
            LCD_DATA_OUT7_IDX,
            LCD_DATA_OUT8_IDX,
            LCD_DATA_OUT9_IDX,
            LCD_DATA_OUT10_IDX,
            LCD_DATA_OUT11_IDX,
            LCD_DATA_OUT12_IDX,
            LCD_DATA_OUT13_IDX,
            LCD_DATA_OUT14_IDX,
            LCD_DATA_OUT15_IDX,
        },
        .cs_sig = LCD_CS_IDX,
        .dc_sig = LCD_DC_IDX,
        .wr_sig = LCD_PCLK_IDX
    }
};

const soc_lcd_rgb_signal_desc_t soc_lcd_rgb_signals[1] = {
    [0] = {
        .module = PERIPH_LCD_CAM_MODULE,
        .irq_id = ETS_LCD_CAM_INTR_SOURCE,
        .data_sigs = {
            LCD_DATA_OUT0_IDX,
            LCD_DATA_OUT1_IDX,
            LCD_DATA_OUT2_IDX,
            LCD_DATA_OUT3_IDX,
            LCD_DATA_OUT4_IDX,
            LCD_DATA_OUT5_IDX,
            LCD_DATA_OUT6_IDX,
            LCD_DATA_OUT7_IDX,
            LCD_DATA_OUT8_IDX,
            LCD_DATA_OUT9_IDX,
            LCD_DATA_OUT10_IDX,
            LCD_DATA_OUT11_IDX,
            LCD_DATA_OUT12_IDX,
            LCD_DATA_OUT13_IDX,
            LCD_DATA_OUT14_IDX,
            LCD_DATA_OUT15_IDX,
        },
        .hsync_sig = LCD_H_SYNC_IDX,
        .vsync_sig = LCD_V_SYNC_IDX,
        .pclk_sig = LCD_PCLK_IDX,
        .de_sig = LCD_H_ENABLE_IDX,
        .disp_sig = SIG_GPIO_OUT_IDX,
    }
};
