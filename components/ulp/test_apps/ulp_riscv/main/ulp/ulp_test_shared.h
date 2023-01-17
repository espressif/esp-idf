/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#define MUTEX_TEST_ITERATIONS 100000
#define XOR_MASK 0xDEADBEEF

typedef enum{
    RISCV_READ_WRITE_TEST = 1,
    RISCV_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST,
    RISCV_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST,
    RISCV_LIGHT_SLEEP_WAKEUP_TEST,
    RISCV_STOP_TEST,
    RISCV_MUTEX_TEST,
    RISCV_NO_COMMAND,
} riscv_test_commands_t;

typedef enum {
    RISCV_COMMAND_OK = 1,
    RISCV_COMMAND_NOK,
    RISCV_COMMAND_INVALID,
} riscv_test_command_reply_t;
