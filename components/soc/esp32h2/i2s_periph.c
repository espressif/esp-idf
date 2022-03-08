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
        // TODO ESP32-H2 IDF-2098

        // .o_bck_in_sig = I2S0O_BCK_IN_IDX,
        // .o_ws_in_sig = I2S0O_WS_IN_IDX,
        // .o_bck_out_sig = I2S0O_BCK_OUT_IDX,
        // .o_ws_out_sig = I2S0O_WS_OUT_IDX,
        // .o_data_out_sig = I2S0O_SD_OUT_IDX,
        // .i_bck_in_sig = I2S0I_BCK_OUT_IDX,
        // .i_ws_in_sig = I2S0I_WS_OUT_IDX,
        // .i_bck_out_sig = I2S0I_BCK_IN_IDX,
        // .i_ws_out_sig = I2S0I_WS_IN_IDX,
        // .i_data_in_sig = I2S0I_SD_IN_IDX,
        .irq = ETS_I2S1_INTR_SOURCE,
        .module = PERIPH_I2S1_MODULE,
    }
};
