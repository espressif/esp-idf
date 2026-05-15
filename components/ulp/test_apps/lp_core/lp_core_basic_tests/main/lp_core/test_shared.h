/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#define XOR_MASK 0xDEADBEEF

/* I2C test params */
#define I2C_SLAVE_ADDRESS 0x28
#define DATA_LENGTH 200
#define RW_TEST_LENGTH       129  /*!<Data length for r/w test, any value from 0-DATA_LENGTH*/

/* LP UART test param */
#define UART_BUF_SIZE  1024

/*
 * LP UART read_bytes return-value test (HP + LP):
 * The burst must fit in the hardware RX FIFO yet typically exceed the LP UART
 * driver's default RX FIFO full threshold (LP_UART_FULL_THRESH_DEFAULT in
 * components/ulp/lp_core/lp_core/lp_core_uart.c). If either value changes,
 * revisit this guard.
 */
#if (SOC_LP_UART_FIFO_LEN) < 12
#error "SOC_LP_UART_FIFO_LEN too small for LP UART read_bytes return-value test"
#endif
#define LP_UART_READ_RETURN_VALUE_BURST_LEN   ((SOC_LP_UART_FIFO_LEN) - 1)

/* Small user buffer for read_bytes buffer-bounds test; HP sends RETURN_VALUE_BURST_LEN. */
#define LP_UART_READ_BOUNDS_USER_BUF_LEN      ((SOC_LP_UART_FIFO_LEN) / 4)

/* Guard memory on each side of the user buffer (worst case = full FIFO depth per side). */
#define LP_UART_READ_BOUNDS_GUARD_LEN         (2 * (SOC_LP_UART_FIFO_LEN))

#define LP_UART_READ_BOUNDS_GUARD_PATTERN     0xAA

typedef struct __attribute__((packed))
{
    uint8_t pre_guard[LP_UART_READ_BOUNDS_GUARD_LEN];
    uint8_t user_buf[LP_UART_READ_BOUNDS_USER_BUF_LEN];
    uint8_t post_guard[LP_UART_READ_BOUNDS_GUARD_LEN];
} lp_uart_read_bounds_guard_t;

_Static_assert(sizeof(lp_uart_read_bounds_guard_t) ==
               (LP_UART_READ_BOUNDS_GUARD_LEN + LP_UART_READ_BOUNDS_USER_BUF_LEN + LP_UART_READ_BOUNDS_GUARD_LEN),
               "lp_uart_read_bounds_guard_t layout must match LP/HP shared memory view");

typedef enum {
    LP_CORE_READ_WRITE_TEST = 1,
    LP_CORE_DELAY_TEST,
    LP_CORE_DELAY_CYCLES_CALIBRATION_TEST,
    LP_CORE_DELAY_US_CALIBRATION_TEST,
    LP_CORE_DEEP_SLEEP_WAKEUP_SHORT_DELAY_TEST,
    LP_CORE_DEEP_SLEEP_WAKEUP_LONG_DELAY_TEST,
    LP_CORE_LP_UART_WRITE_TEST,
    LP_CORE_LP_UART_READ_TEST,
    LP_CORE_LP_UART_MULTI_BYTE_READ_TEST,
    LP_CORE_LP_UART_PRINT_TEST,
    LP_CORE_LP_UART_READ_BYTES_RETURN_VALUE_TEST,
    LP_CORE_LP_UART_READ_BYTES_BOUNDS_TEST,
    LP_CORE_LP_SPI_WRITE_READ_TEST,
    LP_CORE_NO_COMMAND,
} lp_core_test_commands_t;

typedef enum {
    LP_CORE_COMMAND_OK = 1,
    LP_CORE_COMMAND_NOK,
    LP_CORE_COMMAND_INVALID,
} lp_core_test_command_reply_t;

typedef enum {
    LP_CORE_DELAY_SUBCMD_NONE = 0,
    LP_CORE_DELAY_SUBCMD_READY,
    LP_CORE_DELAY_SUBCMD_RUN,
} lp_core_delay_sub_command_t;
