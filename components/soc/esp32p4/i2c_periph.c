/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i2c_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2C peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i2c_signal_conn_t i2c_periph_signal[SOC_I2C_NUM] = {
    {
        .sda_out_sig = I2C0_SDA_PAD_OUT_IDX,
        .sda_in_sig = I2C0_SDA_PAD_IN_IDX,
        .scl_out_sig = I2C0_SCL_PAD_OUT_IDX,
        .scl_in_sig = I2C0_SCL_PAD_IN_IDX,
        .irq = ETS_I2C0_INTR_SOURCE,
        .module = PERIPH_I2C0_MODULE,
    },
    {
        .sda_out_sig = I2C1_SDA_PAD_OUT_IDX,
        .sda_in_sig = I2C1_SDA_PAD_IN_IDX,
        .scl_out_sig = I2C1_SCL_PAD_OUT_IDX,
        .scl_in_sig = I2C1_SCL_PAD_IN_IDX,
        .irq = ETS_I2C1_INTR_SOURCE,
        .module = PERIPH_I2C1_MODULE,
    },
};
