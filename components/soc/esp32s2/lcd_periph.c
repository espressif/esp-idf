/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/lcd_periph.h"
#include "soc/gpio_sig_map.h"

const lcd_signal_conn_t lcd_periph_signals = {
    .buses = {
        [0] = {
            .module = PERIPH_I2S0_MODULE,
            .irq_id = ETS_I2S0_INTR_SOURCE,
            .data_sigs = {
                I2S0O_DATA_OUT0_IDX,
                I2S0O_DATA_OUT1_IDX,
                I2S0O_DATA_OUT2_IDX,
                I2S0O_DATA_OUT3_IDX,
                I2S0O_DATA_OUT4_IDX,
                I2S0O_DATA_OUT5_IDX,
                I2S0O_DATA_OUT6_IDX,
                I2S0O_DATA_OUT7_IDX,
                I2S0O_DATA_OUT8_IDX,
                I2S0O_DATA_OUT9_IDX,
                I2S0O_DATA_OUT10_IDX,
                I2S0O_DATA_OUT11_IDX,
                I2S0O_DATA_OUT12_IDX,
                I2S0O_DATA_OUT13_IDX,
                I2S0O_DATA_OUT14_IDX,
                I2S0O_DATA_OUT15_IDX,
                I2S0O_DATA_OUT16_IDX,
                I2S0O_DATA_OUT17_IDX,
                I2S0O_DATA_OUT18_IDX,
                I2S0O_DATA_OUT19_IDX,
                I2S0O_DATA_OUT20_IDX,
                I2S0O_DATA_OUT21_IDX,
                I2S0O_DATA_OUT22_IDX,
                I2S0O_DATA_OUT23_IDX,
            },
            .wr_sig = I2S0O_WS_OUT_IDX,
        }
    }
};
