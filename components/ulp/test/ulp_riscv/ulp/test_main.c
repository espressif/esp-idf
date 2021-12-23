/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_riscv/ulp_riscv.h"
#include "ulp_riscv/ulp_riscv_utils.h"

typedef enum{
    RISCV_READ_WRITE_TEST = 1,
    RISCV_DEEP_SLEEP_WAKEUP_TEST,
    RISCV_LIGHT_SLEEP_WAKEUP_TEST,
    RISCV_NO_COMMAND,
} riscv_test_commands_t;

typedef enum {
    RISCV_COMMAND_OK = 1,
    RISCV_COMMAND_NOK,
    RISCV_COMMAND_INVALID,
} riscv_test_command_reply_t;

#define XOR_MASK 0xDEADBEEF

volatile riscv_test_commands_t main_cpu_command = RISCV_NO_COMMAND;
volatile riscv_test_command_reply_t main_cpu_reply = RISCV_COMMAND_INVALID;
volatile riscv_test_commands_t command_resp = RISCV_NO_COMMAND;
volatile uint32_t riscv_test_data_in = 0;
volatile uint32_t riscv_test_data_out = 0;

void handle_commands(riscv_test_commands_t cmd)
{
    switch (cmd) {
        case RISCV_READ_WRITE_TEST:
            /* Echo the command ID back to the main CPU */
            command_resp = RISCV_READ_WRITE_TEST;

            /* Process test data */
            riscv_test_data_out = riscv_test_data_in ^ XOR_MASK;

            /* Set the command reply status */
            main_cpu_reply = RISCV_COMMAND_OK;

            /* Wakeup the main CPU */
            ulp_riscv_wakeup_main_processor();
            break;

        case RISCV_DEEP_SLEEP_WAKEUP_TEST:
            /* Echo the command ID back to the main CPU */
            command_resp = RISCV_DEEP_SLEEP_WAKEUP_TEST;

            /* Set the command reply status */
            main_cpu_reply = RISCV_COMMAND_OK;

            /* Wakeup the main CPU */
            ulp_riscv_wakeup_main_processor();
            break;

        case RISCV_LIGHT_SLEEP_WAKEUP_TEST:
            /* Echo the command ID back to the main CPU */
            command_resp = RISCV_LIGHT_SLEEP_WAKEUP_TEST;

            /* Set the command reply status */
            main_cpu_reply = RISCV_COMMAND_OK;

            /* Wakeup the main CPU */
            ulp_riscv_wakeup_main_processor();
            break;

        case RISCV_NO_COMMAND:
            main_cpu_reply = RISCV_COMMAND_OK;
            break;

        default:
            main_cpu_reply = RISCV_COMMAND_NOK;
            break;
    }
}

int main (void)
{
    while (1) {
        handle_commands(main_cpu_command);
        break;
    }

    /* ulp_riscv_shutdown() is called automatically when main exits */
    return 0;
}
