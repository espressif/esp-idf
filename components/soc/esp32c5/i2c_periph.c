/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i2c_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2C peripheral: GPIO signals, irqs, hw addr of registers etc
*/
typedef enum
{
    LP_I2C_MUX_FUNC         = 0,
    LP_GPIO_MUX_FUNC        = 1,
    LP_IO_MUX_FUNC_NUM      = 2,
    LP_MUX_FUNC_NOT_USED    = 0xFF,
} lp_io_mux_func_t;

static_assert(SOC_I2C_NUM == (SOC_HP_I2C_NUM + SOC_LP_I2C_NUM));

const i2c_signal_conn_t i2c_periph_signal[SOC_I2C_NUM] = {
    /* I2C_NUM_0*/
    {
        .sda_out_sig = I2CEXT0_SDA_OUT_IDX,
        .sda_in_sig = I2CEXT0_SDA_IN_IDX,
        .scl_out_sig = I2CEXT0_SCL_OUT_IDX,
        .scl_in_sig = I2CEXT0_SCL_IN_IDX,
        .iomux_func = (uint8_t)LP_MUX_FUNC_NOT_USED,
        .irq = ETS_I2C_EXT0_INTR_SOURCE,
        .module = PERIPH_I2C0_MODULE,
    },
    /* LP_I2C_NUM_0*/
    {
        .sda_out_sig = 0,
        .sda_in_sig = 0,
        .scl_out_sig = 0,
        .scl_in_sig = 0,
        .iomux_func = (uint8_t)LP_I2C_MUX_FUNC,
        .irq = ETS_LP_I2C_INTR_SOURCE,
        .module = PERIPH_LP_I2C0_MODULE,
    },
};
