/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/cam_periph.h"
#include "soc/gpio_sig_map.h"

const cam_signal_conn_t cam_periph_signals = {
    .buses = {
        [0] = {
            .module = PERIPH_LCD_CAM_MODULE,
            .irq_id = ETS_LCD_CAM_INTR_SOURCE,
            .data_sigs = {
                CAM_DATA0_IN_PAD_IN_IDX,
                CAM_DATA1_IN_PAD_IN_IDX,
                CAM_DATA2_IN_PAD_IN_IDX,
                CAM_DATA3_IN_PAD_IN_IDX,
                CAM_DATA4_IN_PAD_IN_IDX,
                CAM_DATA5_IN_PAD_IN_IDX,
                CAM_DATA6_IN_PAD_IN_IDX,
                CAM_DATA7_IN_PAD_IN_IDX,
                CAM_DATA8_IN_PAD_IN_IDX,
                CAM_DATA9_IN_PAD_IN_IDX,
                CAM_DATA10_IN_PAD_IN_IDX,
                CAM_DATA11_IN_PAD_IN_IDX,
                CAM_DATA12_IN_PAD_IN_IDX,
                CAM_DATA13_IN_PAD_IN_IDX,
                CAM_DATA14_IN_PAD_IN_IDX,
                CAM_DATA15_IN_PAD_IN_IDX,
            },
            .hsync_sig = CAM_H_SYNC_PAD_IN_IDX,
            .vsync_sig = CAM_V_SYNC_PAD_IN_IDX,
            .pclk_sig = CAM_PCLK_PAD_IN_IDX,
            .de_sig = CAM_H_ENABLE_PAD_IN_IDX,
            .clk_sig = CAM_CLK_PAD_OUT_IDX
        }
    }
};
