/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/dma2d_periph.h"
#include "hal/dma2d_types.h"
#include "soc/interrupts.h"

const int dma2d_csc_param_yuv2rgb_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT601;
const int dma2d_csc_param_yuv2rgb_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT709;
const int dma2d_csc_param_rgb2yuv_bt601_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT601;
const int dma2d_csc_param_rgb2yuv_bt709_table[3][4] = DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT709;

const dma2d_signal_conn_t dma2d_periph_signals = {
    .groups = {
        [0] = {
            .module_name = "dma2d0",
            .tx_irq_id = {
                [0] = ETS_DMA2D_OUT_CH0_INTR_SOURCE,
                [1] = ETS_DMA2D_OUT_CH1_INTR_SOURCE,
                [2] = ETS_DMA2D_OUT_CH2_INTR_SOURCE,
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
                [3] = ETS_DMA2D_OUT_CH3_INTR_SOURCE,
#endif
            },
            .rx_irq_id = {
                [0] = ETS_DMA2D_IN_CH0_INTR_SOURCE,
                [1] = ETS_DMA2D_IN_CH1_INTR_SOURCE,
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
                [2] = ETS_DMA2D_IN_CH2_INTR_SOURCE,
#endif
            }
        }
    }
};
