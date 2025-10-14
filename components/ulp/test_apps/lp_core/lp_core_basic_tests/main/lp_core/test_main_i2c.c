/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "test_shared.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_i2c.h"
#include "ulp_lp_core_interrupts.h"

#define LP_I2C_TRANS_WAIT_FOREVER   -1

volatile lp_core_test_command_reply_t read_test_reply = LP_CORE_COMMAND_INVALID;
volatile lp_core_test_command_reply_t write_test_cmd = LP_CORE_COMMAND_INVALID;

uint8_t data_rd[DATA_LENGTH] = {};
uint8_t data_wr[DATA_LENGTH] = {};

int main(void)
{
    /* Enable interrupts.
     * This does not affect how the LP I2C functions
     * but it will add extra test coverage to make sure
     * the interrupt handler does not cause any issues.
     */
    ulp_lp_core_intr_enable();

    lp_core_i2c_master_read_from_device(LP_I2C_NUM_0, I2C_SLAVE_ADDRESS, data_rd, RW_TEST_LENGTH, LP_I2C_TRANS_WAIT_FOREVER);
    read_test_reply = LP_CORE_COMMAND_OK;

    /* Wait for write command from main CPU */
    while (write_test_cmd != LP_CORE_COMMAND_OK) {
    }

    lp_core_i2c_master_write_to_device(LP_I2C_NUM_0, I2C_SLAVE_ADDRESS, data_wr, RW_TEST_LENGTH, LP_I2C_TRANS_WAIT_FOREVER);

    write_test_cmd = LP_CORE_COMMAND_NOK;

    while (1) {
    }
}
