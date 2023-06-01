/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_i2c.h"
#include "ulp_lp_core_utils.h"
#include "../bh1750_defs.h"

#define LP_I2C_TRANS_TIMEOUT_CYCLES 5000
#define LP_I2C_TRANS_WAIT_FOREVER   -1

#define LUX_THRESHOLD_LOW           5
#define LUX_THRESHOLD_HIGH          1000

static uint32_t sensor_on = 0;
static uint32_t res_update_done = 0;
volatile uint32_t lux = 0;

static void bh1750_read()
{
    uint8_t data_rd[2];
    esp_err_t ret = lp_core_i2c_master_read_from_device(LP_I2C_NUM_0, BH1750_I2C_ADDR, data_rd, sizeof(data_rd), LP_I2C_TRANS_TIMEOUT_CYCLES);
    if (ret != ESP_OK) {
        // Skip this round of calculation and return
        return;
    }

    /* Calculate light intensity value */
    uint16_t level = ((data_rd[0] << 8) | data_rd[1]);
    lux = (level * 10) / 12;

    /* Wakeup main CPU if the Lux breaches the thresholds */
    if (lux < LUX_THRESHOLD_LOW || lux > LUX_THRESHOLD_HIGH) {
        ulp_lp_core_wakeup_main_processor();
    }
}

int main (void)
{
    uint8_t data_wr = 0;
    esp_err_t ret = ESP_OK;

    while (1) {
        if (!sensor_on) {
            /* Power ON the sensor */
            data_wr = BH1750_POWER_ON;
            ret = lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, BH1750_I2C_ADDR, &data_wr, sizeof(data_wr), LP_I2C_TRANS_WAIT_FOREVER);
            if (ret != ESP_OK) {
                // Bail and try again
                continue;
            }
            sensor_on = 1;
        }

        if (!res_update_done) {
            data_wr = EXAMPLE_RES_MODE;
            ret = lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, BH1750_I2C_ADDR, &data_wr, sizeof(data_wr), LP_I2C_TRANS_WAIT_FOREVER);
            if (ret != ESP_OK) {
                // Bail and try again
                continue;
            }
            res_update_done = 1;
        }

        /* Read BH1750 sensor data */
        bh1750_read();
    }

    return 0;
}
