/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_i2c.h"
#include "lp_sleep_payload.h"
#include "lp_sleep_finish.h"

volatile int32_t i2c_sleep_read_ret;
volatile int32_t i2c_sleep_write_ret;
uint8_t i2c_sleep_data_rd[LP_SLEEP_I2C_LEN];

int main(void)
{
    uint8_t data_wr[LP_SLEEP_I2C_LEN];

    ulp_lp_core_delay_us(LP_SLEEP_SETTLE_US);
    lp_sleep_fill_i2c_write(data_wr);

    i2c_sleep_read_ret = lp_core_i2c_master_read_from_device(LP_I2C_NUM_0, LP_SLEEP_I2C_ADDR,
                                                             i2c_sleep_data_rd, LP_SLEEP_I2C_LEN,
                                                             LP_SLEEP_IO_TIMEOUT_CYCLES);
    i2c_sleep_write_ret = lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, LP_SLEEP_I2C_ADDR,
                                                             data_wr, LP_SLEEP_I2C_LEN,
                                                             LP_SLEEP_IO_TIMEOUT_CYCLES);
    lp_sleep_finish();
}
