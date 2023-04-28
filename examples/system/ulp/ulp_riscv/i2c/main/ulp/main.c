/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP RISC-V RTC I2C example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP RISC-V coprocessor
*/

#include <stdint.h>
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_i2c_ulp_core.h"
#include "../bmp180_defs.h"

/************************************************
 * Shared data between main CPU and ULP
 ************************************************/
int16_t ut_data = 0;
int32_t up_data = 0;
int32_t ut_threshold = EXAMPLE_UT_THRESHOLD;
int32_t up_threshold = EXAMPLE_UP_THRESHOLD;
oss_mode_t oss_mode = EXAMPLE_OSS_MODE;

int main (void)
{
    /* Read uncompensated temperature */
    bmp180_read_ut_data(&ut_data);

    /* Read uncompensated pressure */
    bmp180_read_up_data(&up_data, oss_mode);

    /* Wakeup the main CPU if either the uncompensated temperature or uncompensated pressure values
     * are more than their respective threshold values.
     */
    if ((ut_data > ut_threshold) || (up_data > up_threshold)) {
        ulp_riscv_wakeup_main_processor();
    }

    return 0;
}

static void bmp180_read16(uint16_t *data_out, uint32_t reg_msb, uint32_t reg_lsb)
{
    uint8_t data_rd = 0;
    *data_out = 0;

    ulp_riscv_i2c_master_set_slave_reg_addr(reg_msb);
    ulp_riscv_i2c_master_read_from_device(&data_rd, 1);
    *data_out |= (uint16_t)(data_rd << 8);
    ulp_riscv_i2c_master_set_slave_reg_addr(reg_lsb);
    data_rd = 0;
    ulp_riscv_i2c_master_read_from_device(&data_rd, 1);
    *data_out |= (uint16_t)(data_rd);
}

static void bmp180_read_ut_data(int16_t *ut_data)
{
    /* Set slave register address to the control register */
    ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_CTRL_REG);

    /* Setup control register to read temperature */
    uint8_t cmd = BMP180_SENSOR_CMD_READ_TEMPERATURE;
    ulp_riscv_i2c_master_write_to_device(&cmd, 1);

    /* Wait at least 4.5 milliseconds for the sensor to complete the reading */
    ulp_riscv_delay_cycles(5 * ULP_RISCV_CYCLES_PER_MS);

    /* Read uncompensated temperature data */
    bmp180_read16((uint16_t *)ut_data, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_MSB, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_LSB);
}

static void bmp180_read_up_data(int32_t *up_data, oss_mode_t oss_mode)
{
    uint16_t press_high;
    uint8_t press_low;

    /* Set slave register address to the control register */
    ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_CTRL_REG);

    /* Setup control register to read pressure */
    uint8_t cmd = 0;
    uint8_t wait = 0;
    switch(oss_mode)
    {
        case OSS_0:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_0;
            wait = 5; // Wait atleast 4.5 msec
            break;
        case OSS_1:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_1;
            wait = 8; // Wait atleast 7.5 msec
            break;
        case OSS_2:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_2;
            wait = 14; // Wait atleast 13.5 msec
            break;
        case OSS_3:
            cmd = BMP180_SENSOR_CMD_READ_PRESSURE_OSS_3;
            wait = 26; // Wait atleast 25.5 msec
            break;
    }

    ulp_riscv_i2c_master_write_to_device(&cmd, 1);

    /* Wait for the required amount of time for the sensor to complete the reading */
    ulp_riscv_delay_cycles(wait * ULP_RISCV_CYCLES_PER_MS);

    /* Read uncompensated temperature data */

    /* Read MSB + LSB */
    bmp180_read16(&press_high, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_MSB, BMP180_SENSOR_REG_ADDR_SENSOR_DATA_LSB);

    /* Read XLSB */
    ulp_riscv_i2c_master_set_slave_reg_addr(BMP180_SENSOR_REG_ADDR_SENSOR_DATA_XLSB);
    ulp_riscv_i2c_master_read_from_device(&press_low, 1);

    *up_data = (((uint32_t)press_high << 8) + (uint32_t)press_low) >> (8 - oss_mode);
}
