/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#define XOR_MASK 0xDEADBEEF

typedef enum{
    LP_CORE_READ_WRITE_TEST = 1,
    LP_CORE_DELAY_TEST,
    LP_CORE_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST,
    LP_CORE_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST,
    LP_CORE_NO_COMMAND,
} lp_core_test_commands_t;

typedef enum {
    LP_CORE_COMMAND_OK = 1,
    LP_CORE_COMMAND_NOK,
    LP_CORE_COMMAND_INVALID,
} lp_core_test_command_reply_t;
