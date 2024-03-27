/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#define MUTEX_TEST_ITERATIONS 100000
#define XOR_MASK 0xDEADBEEF

/* I2C test params */
#define I2C_SLAVE_ADDRESS 0x28
#define DATA_LENGTH 200
// TODO: Updated the test to perform multi-byte read/write (IDFGH-11056)
#define RW_TEST_LENGTH       1  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/

typedef enum {
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
