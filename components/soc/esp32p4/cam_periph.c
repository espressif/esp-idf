/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gpio_sig_map.h"
#include "soc/cam_periph.h"

const cam_signal_conn_t cam_periph_signals = {
    .buses = {
        [0] = {
            .module = PERIPH_LCD_CAM_MODULE,
            .irq_id = ETS_LCD_CAM_INTR_SOURCE,
            .data_sigs = {
                CAM_DATA_IN_PAD_IN0_IDX,
                CAM_DATA_IN_PAD_IN1_IDX,
                CAM_DATA_IN_PAD_IN2_IDX,
                CAM_DATA_IN_PAD_IN3_IDX,
                CAM_DATA_IN_PAD_IN4_IDX,
                CAM_DATA_IN_PAD_IN5_IDX,
                CAM_DATA_IN_PAD_IN6_IDX,
                CAM_DATA_IN_PAD_IN7_IDX,
                CAM_DATA_IN_PAD_IN8_IDX,
                CAM_DATA_IN_PAD_IN9_IDX,
                CAM_DATA_IN_PAD_IN10_IDX,
                CAM_DATA_IN_PAD_IN11_IDX,
                CAM_DATA_IN_PAD_IN12_IDX,
                CAM_DATA_IN_PAD_IN13_IDX,
                CAM_DATA_IN_PAD_IN14_IDX,
                CAM_DATA_IN_PAD_IN15_IDX,
            },
            .hsync_sig = CAM_H_SYNC_PAD_IN_IDX,
            .vsync_sig = CAM_V_SYNC_PAD_IN_IDX,
            .pclk_sig = CAM_PCLK_PAD_IN_IDX,
            .de_sig = CAM_H_ENABLE_PAD_IN_IDX,
            .clk_sig = CAM_CLK_PAD_OUT_IDX
        }
    }
};
