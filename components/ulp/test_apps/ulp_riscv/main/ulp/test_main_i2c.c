/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_test_shared.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_i2c_ulp_core.h"
#include "esp_err.h"

volatile riscv_test_command_reply_t read_test_reply = RISCV_COMMAND_INVALID;
volatile riscv_test_command_reply_t write_test_cmd = RISCV_COMMAND_INVALID;

uint8_t data_rd[DATA_LENGTH] = {};
uint8_t data_wr[DATA_LENGTH] = {};

int main(void)
{
    /* Set I2C slave device address */
    ulp_riscv_i2c_master_set_slave_addr(I2C_SLAVE_ADDRESS);

    /* Read from the I2C slave device */
    esp_err_t ret = ulp_riscv_i2c_master_read_from_device(data_rd, RW_TEST_LENGTH);

    /* Signal the main CPU with actual result */
    read_test_reply = (ret == ESP_OK) ? RISCV_COMMAND_OK : RISCV_COMMAND_NOK;

    /* Wait for write command from main CPU */
    while (write_test_cmd != RISCV_COMMAND_OK) {
    }

    /* Write to the I2C slave device */
    ulp_riscv_i2c_master_write_to_device(data_wr, RW_TEST_LENGTH);

    /* Signal the main CPU once write is done */
    write_test_cmd = RISCV_COMMAND_NOK;

    while (1) {
    }
}
