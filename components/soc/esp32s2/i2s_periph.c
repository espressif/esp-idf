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
        .mck_out_sig  = CLK_I2S_MUX_IDX,

        .m_tx_bck_sig = I2S0O_BCK_OUT_IDX,
        .m_rx_bck_sig = I2S0I_BCK_OUT_IDX,
        .m_tx_ws_sig  = I2S0O_WS_OUT_IDX,
        .m_rx_ws_sig  = I2S0I_WS_OUT_IDX,

        .s_tx_bck_sig = I2S0O_BCK_IN_IDX,
        .s_rx_bck_sig = I2S0I_BCK_IN_IDX,
        .s_tx_ws_sig  = I2S0O_WS_IN_IDX,
        .s_rx_ws_sig  = I2S0I_WS_IN_IDX,

        .data_out_sig = I2S0O_DATA_OUT23_IDX,
        .data_in_sig  = I2S0I_DATA_IN15_IDX,

        .irq          = ETS_I2S0_INTR_SOURCE,
        .module       = PERIPH_I2S0_MODULE,
    }
};
