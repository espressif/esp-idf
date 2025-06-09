/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
                CAM_DATA_IN0_IDX,
                CAM_DATA_IN1_IDX,
                CAM_DATA_IN2_IDX,
                CAM_DATA_IN3_IDX,
                CAM_DATA_IN4_IDX,
                CAM_DATA_IN5_IDX,
                CAM_DATA_IN6_IDX,
                CAM_DATA_IN7_IDX,
                CAM_DATA_IN8_IDX,
                CAM_DATA_IN9_IDX,
                CAM_DATA_IN10_IDX,
                CAM_DATA_IN11_IDX,
                CAM_DATA_IN12_IDX,
                CAM_DATA_IN13_IDX,
                CAM_DATA_IN14_IDX,
                CAM_DATA_IN15_IDX
            },
            .hsync_sig = CAM_H_SYNC_IDX,
            .vsync_sig = CAM_V_SYNC_IDX,
            .pclk_sig = CAM_PCLK_IDX,
            .de_sig = CAM_H_ENABLE_IDX,
            .clk_sig = CAM_CLK_IDX
        }
    }
};
