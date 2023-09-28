/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i2s_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2S peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_NUM] = {
    [0] = {
        .mck_out_sig  = I2S0_MCLK_PAD_OUT_IDX,
        .mck_in_sig   = I2S0_MCLK_PAD_IN_IDX,

        .m_tx_bck_sig = I2S0_O_BCK_PAD_OUT_IDX,
        .m_rx_bck_sig = I2S0_I_BCK_PAD_OUT_IDX,
        .m_tx_ws_sig  = I2S0_O_WS_PAD_OUT_IDX,
        .m_rx_ws_sig  = I2S0_I_WS_PAD_OUT_IDX,

        .s_tx_bck_sig = I2S0_O_BCK_PAD_IN_IDX,
        .s_rx_bck_sig = I2S0_I_BCK_PAD_IN_IDX,
        .s_tx_ws_sig  = I2S0_O_WS_PAD_IN_IDX,
        .s_rx_ws_sig  = I2S0_I_WS_PAD_IN_IDX,

        .data_out_sigs[0] = I2S0_O_SD_PAD_OUT_IDX,
        .data_out_sigs[1] = I2S0_O_SD1_PAD_OUT_IDX,
        .data_in_sigs[0]  = I2S0_I_SD_PAD_IN_IDX,
        .data_in_sigs[1]  = I2S0_I_SD1_PAD_IN_IDX,
        .data_in_sigs[2]  = I2S0_I_SD2_PAD_IN_IDX,
        .data_in_sigs[3]  = I2S0_I_SD3_PAD_IN_IDX,

        .irq          = ETS_I2S0_INTR_SOURCE,
        .module       = PERIPH_I2S0_MODULE,
    },
    [1] = {
        .mck_out_sig  = I2S1_MCLK_PAD_OUT_IDX,
        .mck_in_sig   = I2S1_MCLK_PAD_IN_IDX,

        .m_tx_bck_sig = I2S1_O_BCK_PAD_OUT_IDX,
        .m_rx_bck_sig = I2S1_I_BCK_PAD_OUT_IDX,
        .m_tx_ws_sig  = I2S1_O_WS_PAD_OUT_IDX,
        .m_rx_ws_sig  = I2S1_I_WS_PAD_OUT_IDX,

        .s_tx_bck_sig = I2S1_O_BCK_PAD_IN_IDX,
        .s_rx_bck_sig = I2S1_I_BCK_PAD_IN_IDX,
        .s_tx_ws_sig  = I2S1_O_WS_PAD_IN_IDX,
        .s_rx_ws_sig  = I2S1_I_WS_PAD_IN_IDX,

        .data_out_sigs[0] = I2S1_O_SD_PAD_OUT_IDX,
        .data_out_sigs[1] = -1,
        .data_in_sigs[0]  = I2S1_I_SD_PAD_IN_IDX,
        .data_in_sigs[1]  = -1,
        .data_in_sigs[2]  = -1,
        .data_in_sigs[3]  = -1,

        .irq          = ETS_I2S1_INTR_SOURCE,
        .module       = PERIPH_I2S1_MODULE,
    },
    [2] = {
        .mck_out_sig  = I2S2_MCLK_PAD_OUT_IDX,
        .mck_in_sig   = I2S2_MCLK_PAD_IN_IDX,

        .m_tx_bck_sig = I2S2_O_BCK_PAD_OUT_IDX,
        .m_rx_bck_sig = I2S2_I_BCK_PAD_OUT_IDX,
        .m_tx_ws_sig  = I2S2_O_WS_PAD_OUT_IDX,
        .m_rx_ws_sig  = I2S2_I_WS_PAD_OUT_IDX,

        .s_tx_bck_sig = I2S2_O_BCK_PAD_IN_IDX,
        .s_rx_bck_sig = I2S2_I_BCK_PAD_IN_IDX,
        .s_tx_ws_sig  = I2S2_O_WS_PAD_IN_IDX,
        .s_rx_ws_sig  = I2S2_I_WS_PAD_IN_IDX,

        .data_out_sigs[0] = I2S2_O_SD_PAD_OUT_IDX,
        .data_out_sigs[1] = -1,
        .data_in_sigs[0]  = I2S2_I_SD_PAD_IN_IDX,
        .data_in_sigs[1]  = -1,
        .data_in_sigs[2]  = -1,
        .data_in_sigs[3]  = -1,

        .irq          = ETS_I2S2_INTR_SOURCE,
        .module       = PERIPH_I2S2_MODULE,
    },
};
