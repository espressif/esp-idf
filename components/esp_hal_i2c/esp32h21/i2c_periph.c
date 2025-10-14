/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
        .module_name = "I2C0",
        .sda_out_sig = I2CEXT0_SDA_OUT_IDX,
        .sda_in_sig = I2CEXT0_SDA_IN_IDX,
        .scl_out_sig = I2CEXT0_SCL_OUT_IDX,
        .scl_in_sig = I2CEXT0_SCL_IN_IDX,
        .irq = ETS_I2C_EXT0_INTR_SOURCE,
    },
    {
        .module_name = "I2C1",
        .sda_out_sig = I2CEXT1_SDA_OUT_IDX,
        .sda_in_sig = I2CEXT1_SDA_IN_IDX,
        .scl_out_sig = I2CEXT1_SCL_OUT_IDX,
        .scl_in_sig = I2CEXT1_SCL_IN_IDX,
        .irq = ETS_I2C_EXT1_INTR_SOURCE,
    },
};
