/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "test_shared.h"
#include "ulp_lp_core_utils.h"

volatile lp_core_test_commands_t main_cpu_command = LP_CORE_NO_COMMAND;
volatile lp_core_test_command_reply_t main_cpu_reply = LP_CORE_COMMAND_INVALID;
volatile lp_core_test_commands_t command_resp = LP_CORE_NO_COMMAND;
volatile uint32_t test_data_in = 0;
volatile uint32_t test_data_out = 0;
volatile uint32_t delay_start_cycles = 0;
volatile uint32_t delay_end_cycles = 0;
volatile uint32_t delay_sub_command = LP_CORE_DELAY_SUBCMD_NONE;

volatile uint32_t incrementer = 0;

static void run_delay_calibration_test(lp_core_test_commands_t calibration_cmd, bool use_cycles_delay)
{
    register uint32_t start, end, overhead = 0;
    const uint32_t iterations = 5;

    for (int i = 0; i < iterations; i++) {
        start = ulp_lp_core_get_cpu_cycles();
        end = ulp_lp_core_get_cpu_cycles();
        overhead += end - start;
    }
    overhead /= iterations;

    command_resp = calibration_cmd;

    while (main_cpu_command == calibration_cmd) {
        delay_sub_command = LP_CORE_DELAY_SUBCMD_READY;
        while (delay_sub_command != LP_CORE_DELAY_SUBCMD_RUN) { };
        register uint32_t requested_delay = test_data_in;
        ulp_lp_core_delay_us(1);

        if (use_cycles_delay) {
            start = ulp_lp_core_get_cpu_cycles();
            ulp_lp_core_delay_cycles(requested_delay);
            end = ulp_lp_core_get_cpu_cycles();
        } else {
            start = ulp_lp_core_get_cpu_cycles();
            ulp_lp_core_delay_us(requested_delay);
            end = ulp_lp_core_get_cpu_cycles();
        }

        delay_sub_command = LP_CORE_DELAY_SUBCMD_NONE;

        delay_start_cycles = start;
        delay_end_cycles = end - overhead;
    }
}

void handle_commands(lp_core_test_commands_t cmd)
{

    switch (cmd) {
    case LP_CORE_READ_WRITE_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = LP_CORE_READ_WRITE_TEST;

        /* Process test data */
        test_data_out = test_data_in ^ XOR_MASK;

        /* Set the command reply status */
        main_cpu_reply = LP_CORE_COMMAND_OK;
        main_cpu_command = LP_CORE_NO_COMMAND;

        break;

    case LP_CORE_DELAY_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = LP_CORE_DELAY_TEST;

        ulp_lp_core_delay_us(test_data_in);
        main_cpu_reply = LP_CORE_COMMAND_OK;
        main_cpu_command = LP_CORE_NO_COMMAND;
        break;

    case LP_CORE_DELAY_CYCLES_CALIBRATION_TEST:
        run_delay_calibration_test(LP_CORE_DELAY_CYCLES_CALIBRATION_TEST, true);
        break;

    case LP_CORE_DELAY_US_CALIBRATION_TEST:
        run_delay_calibration_test(LP_CORE_DELAY_US_CALIBRATION_TEST, false);
        break;

    case LP_CORE_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = LP_CORE_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST;

        /* Set the command reply status */
        main_cpu_reply = LP_CORE_COMMAND_OK;
        main_cpu_command = LP_CORE_NO_COMMAND;

        ulp_lp_core_delay_us(1000 * 1000);
        ulp_lp_core_wakeup_main_processor();

        break;

    case LP_CORE_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST:
        /* Echo the command ID back to the main CPU */
        command_resp = LP_CORE_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST;

        /* Set the command reply status */
        main_cpu_reply = LP_CORE_COMMAND_OK;
        main_cpu_command = LP_CORE_NO_COMMAND;

        ulp_lp_core_delay_us(10000 * 1000);
        ulp_lp_core_wakeup_main_processor();

        break;

    case LP_CORE_NO_COMMAND:
        break;

    default:
        break;
    }
}

int main(void)
{
    while (1) {
        handle_commands(main_cpu_command);
    }

    return 0;
}
