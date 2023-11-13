/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/lcd_periph.h"
#include "soc/gpio_sig_map.h"

const lcd_signal_conn_t lcd_periph_signals = {
    .buses = {
        [0] = {
            .module = PERIPH_LCD_CAM_MODULE,
            .irq_id = ETS_LCD_CAM_INTR_SOURCE,
            .data_sigs = {
                LCD_DATA_OUT_PAD_OUT0_IDX,
                LCD_DATA_OUT_PAD_OUT1_IDX,
                LCD_DATA_OUT_PAD_OUT2_IDX,
                LCD_DATA_OUT_PAD_OUT3_IDX,
                LCD_DATA_OUT_PAD_OUT4_IDX,
                LCD_DATA_OUT_PAD_OUT5_IDX,
                LCD_DATA_OUT_PAD_OUT6_IDX,
                LCD_DATA_OUT_PAD_OUT7_IDX,
                LCD_DATA_OUT_PAD_OUT8_IDX,
                LCD_DATA_OUT_PAD_OUT9_IDX,
                LCD_DATA_OUT_PAD_OUT10_IDX,
                LCD_DATA_OUT_PAD_OUT11_IDX,
                LCD_DATA_OUT_PAD_OUT12_IDX,
                LCD_DATA_OUT_PAD_OUT13_IDX,
                LCD_DATA_OUT_PAD_OUT14_IDX,
                LCD_DATA_OUT_PAD_OUT15_IDX,
                LCD_DATA_OUT_PAD_OUT16_IDX,
                LCD_DATA_OUT_PAD_OUT17_IDX,
                LCD_DATA_OUT_PAD_OUT18_IDX,
                LCD_DATA_OUT_PAD_OUT19_IDX,
                LCD_DATA_OUT_PAD_OUT20_IDX,
                LCD_DATA_OUT_PAD_OUT21_IDX,
                LCD_DATA_OUT_PAD_OUT22_IDX,
                LCD_DATA_OUT_PAD_OUT23_IDX,
            },
            .cs_sig = LCD_CS_PAD_OUT_IDX,
            .dc_sig = LCD_DC_PAD_OUT_IDX,
            .wr_sig = LCD_PCLK_PAD_OUT_IDX
        }
    },
    .panels = {
        [0] = {
            .module = PERIPH_LCD_CAM_MODULE,
            .irq_id = ETS_LCD_CAM_INTR_SOURCE,
            .data_sigs = {
                LCD_DATA_OUT_PAD_OUT0_IDX,
                LCD_DATA_OUT_PAD_OUT1_IDX,
                LCD_DATA_OUT_PAD_OUT2_IDX,
                LCD_DATA_OUT_PAD_OUT3_IDX,
                LCD_DATA_OUT_PAD_OUT4_IDX,
                LCD_DATA_OUT_PAD_OUT5_IDX,
                LCD_DATA_OUT_PAD_OUT6_IDX,
                LCD_DATA_OUT_PAD_OUT7_IDX,
                LCD_DATA_OUT_PAD_OUT8_IDX,
                LCD_DATA_OUT_PAD_OUT9_IDX,
                LCD_DATA_OUT_PAD_OUT10_IDX,
                LCD_DATA_OUT_PAD_OUT11_IDX,
                LCD_DATA_OUT_PAD_OUT12_IDX,
                LCD_DATA_OUT_PAD_OUT13_IDX,
                LCD_DATA_OUT_PAD_OUT14_IDX,
                LCD_DATA_OUT_PAD_OUT15_IDX,
                LCD_DATA_OUT_PAD_OUT16_IDX,
                LCD_DATA_OUT_PAD_OUT17_IDX,
                LCD_DATA_OUT_PAD_OUT18_IDX,
                LCD_DATA_OUT_PAD_OUT19_IDX,
                LCD_DATA_OUT_PAD_OUT20_IDX,
                LCD_DATA_OUT_PAD_OUT21_IDX,
                LCD_DATA_OUT_PAD_OUT22_IDX,
                LCD_DATA_OUT_PAD_OUT23_IDX,
            },
            .hsync_sig = LCD_H_SYNC_PAD_OUT_IDX,
            .vsync_sig = LCD_V_SYNC_PAD_OUT_IDX,
            .pclk_sig = LCD_PCLK_PAD_OUT_IDX,
            .de_sig = LCD_H_ENABLE_PAD_OUT_IDX,
        }
    }
};
