/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i2s_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2S peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_NUM] = {
    {
        .mck_out_sig  = I2S0_MCLK_OUT_IDX,
        .mck_in_sig   = I2S0_MCLK_IN_IDX,

        .m_tx_bck_sig = I2S0O_BCK_OUT_IDX,
        .m_rx_bck_sig = I2S0I_BCK_OUT_IDX,
        .m_tx_ws_sig  = I2S0O_WS_OUT_IDX,
        .m_rx_ws_sig  = I2S0I_WS_OUT_IDX,

        .s_tx_bck_sig = I2S0O_BCK_IN_IDX,
        .s_rx_bck_sig = I2S0I_BCK_IN_IDX,
        .s_tx_ws_sig  = I2S0O_WS_IN_IDX,
        .s_rx_ws_sig  = I2S0I_WS_IN_IDX,

        .data_out_sigs[0] = I2S0O_SD_OUT_IDX,
        .data_out_sigs[1] = I2S0O_SD1_OUT_IDX,
        .data_in_sigs[0]  = I2S0I_SD_IN_IDX,
        .data_in_sigs[1]  = I2S0I_SD1_IN_IDX,
        .data_in_sigs[2]  = I2S0I_SD2_IN_IDX,
        .data_in_sigs[3]  = I2S0I_SD3_IN_IDX,

        .irq          = ETS_I2S0_INTR_SOURCE,
    },
    {
        .mck_out_sig  = I2S1_MCLK_OUT_IDX,
        .mck_in_sig   = I2S1_MCLK_IN_IDX,

        .m_tx_bck_sig = I2S1O_BCK_OUT_IDX,
        .m_rx_bck_sig = I2S1I_BCK_OUT_IDX,
        .m_tx_ws_sig  = I2S1O_WS_OUT_IDX,
        .m_rx_ws_sig  = I2S1I_WS_OUT_IDX,

        .s_tx_bck_sig = I2S1O_BCK_IN_IDX,
        .s_rx_bck_sig = I2S1I_BCK_IN_IDX,
        .s_tx_ws_sig  = I2S1O_WS_IN_IDX,
        .s_rx_ws_sig  = I2S1I_WS_IN_IDX,

        .data_out_sigs[0] = I2S1O_SD_OUT_IDX,
        .data_out_sigs[1] = -1,
        .data_in_sigs[0]  = I2S1I_SD_IN_IDX,
        .data_in_sigs[1]  = -1,
        .data_in_sigs[2]  = -1,
        .data_in_sigs[3]  = -1,

        .irq          = ETS_I2S1_INTR_SOURCE,
    }
};
