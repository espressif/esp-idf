/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#define XOR_MASK 0xDEADBEEF

/* I2C test params */
#define I2C_SLAVE_ADDRESS 0x28
#define DATA_LENGTH 200
#define RW_TEST_LENGTH       129  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/

/* LP UART test param */
#define UART_BUF_SIZE  1024

typedef enum {
    LP_CORE_READ_WRITE_TEST = 1,
    LP_CORE_DELAY_TEST,
    LP_CORE_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST,
    LP_CORE_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST,
    LP_CORE_LP_UART_WRITE_TEST,
    LP_CORE_LP_UART_READ_TEST,
    LP_CORE_NO_COMMAND,
} lp_core_test_commands_t;

typedef enum {
    LP_CORE_COMMAND_OK = 1,
    LP_CORE_COMMAND_NOK,
    LP_CORE_COMMAND_INVALID,
} lp_core_test_command_reply_t;
