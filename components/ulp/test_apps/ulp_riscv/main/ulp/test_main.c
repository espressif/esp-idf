/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ulp_riscv_utils.h"
#include "ulp_riscv_gpio.h"
#include "ulp_riscv_lock_ulp_core.h"
#include "ulp_test_shared.h"

volatile riscv_test_commands_t main_cpu_command = RISCV_NO_COMMAND;
volatile riscv_test_command_reply_t main_cpu_reply = RISCV_COMMAND_INVALID;
volatile riscv_test_commands_t command_resp = RISCV_NO_COMMAND;
volatile uint32_t riscv_test_data_in = 0;
volatile uint32_t riscv_test_data_out = 0;
volatile uint32_t riscv_counter = 0;

volatile uint32_t riscv_incrementer = 0;
ulp_riscv_lock_t lock;

void handle_commands(riscv_test_commands_t cmd)
{
    riscv_counter++;

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

    case RISCV_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = RISCV_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST;

        /* Set the command reply status */
        main_cpu_reply = RISCV_COMMAND_OK;

        ulp_riscv_delay_cycles(1000 * ULP_RISCV_CYCLES_PER_MS);

        /* Wakeup the main CPU */
        ulp_riscv_wakeup_main_processor();
        break;

    case RISCV_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = RISCV_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST;

        /* Set the command reply status */
        main_cpu_reply = RISCV_COMMAND_OK;

        ulp_riscv_delay_cycles(10000 * ULP_RISCV_CYCLES_PER_MS);

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

    case RISCV_STOP_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = RISCV_STOP_TEST;

        /* Set the command reply status */
        main_cpu_reply = RISCV_COMMAND_OK;

        /* Will never return from here */
        ulp_riscv_timer_stop();
        ulp_riscv_halt();

        break;

    case RISCV_MUTEX_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = RISCV_MUTEX_TEST;

        for (int i = 0; i < MUTEX_TEST_ITERATIONS; i++) {
            ulp_riscv_lock_acquire(&lock);
            riscv_incrementer++;
            ulp_riscv_lock_release(&lock);
        }
        /* Set the command reply status */
        main_cpu_reply = RISCV_COMMAND_OK;
        main_cpu_command = RISCV_NO_COMMAND;

        break;

    case RISCV_NO_COMMAND:
        main_cpu_reply = RISCV_COMMAND_OK;
        break;

    default:
        main_cpu_reply = RISCV_COMMAND_NOK;
        break;
    }
}

int main(void)
{

    while (1) {
        handle_commands(main_cpu_command);
        break;
    }

    /* ulp_riscv_halt() is called automatically when main exits */
    return 0;
}
