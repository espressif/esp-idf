/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#define LP_SLEEP_I2C_ADDR           0x28
#define LP_SLEEP_I2C_LEN            8
#define LP_SLEEP_SPI_LEN            8
#define LP_SLEEP_UART_LEN           12
#define LP_SLEEP_PRINT              "lp-sleep-print"
#define LP_SLEEP_UART_WAKE_SEQ      "hello"
#define LP_SLEEP_UART_WAKE_REPLY    "Hello world"

/* Extra wait inside LP main. The LP timer already fires after HP has started
 * deep sleep; this covers the tail of sleep entry before the bus is touched. */
#define LP_SLEEP_SETTLE_US          100000

/* Releasing a pad hold when HP wakes can produce one level change, so only a
 * pulse train proves that LP drove the pin while HP was asleep. */
#define LP_SLEEP_GPIO_PULSES        3
#define LP_SLEEP_GPIO_PULSE_US      100000
#define LP_SLEEP_GPIO_WAIT_US       3000000

/* Bound a transaction so a missing peer still wakes HP and reports the error. */
#define LP_SLEEP_IO_TIMEOUT_CYCLES  40000000

/* HP slave is queued only after SLEEPER_SLEEP, once LP SPI pins are configured.
 * Stay idle until that queue is in place, then clock. */
#define LP_SLEEP_SPI_MASTER_WAIT_US 500000

/* LP slave arms after the LP timer and the settle delay. The peer clocks
 * later, so the wait has to outlast that gap. */
#define LP_SLEEP_SPI_SLAVE_TIMEOUT_CYCLES 200000000

static inline void lp_sleep_fill_i2c_write(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_I2C_LEN; i++) {
        buf[i] = (uint8_t)(0xA0 + i);
    }
}

static inline void lp_sleep_fill_i2c_read(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_I2C_LEN; i++) {
        buf[i] = (uint8_t)(0x10 + (i * 3));
    }
}

static inline void lp_sleep_fill_spi_master_tx(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_SPI_LEN; i++) {
        buf[i] = (uint8_t)(0x50 + i);
    }
}

static inline void lp_sleep_fill_spi_master_rx(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_SPI_LEN; i++) {
        buf[i] = (uint8_t)(0x60 + i);
    }
}

static inline void lp_sleep_fill_spi_slave_tx(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_SPI_LEN; i++) {
        buf[i] = (uint8_t)(0x70 + i);
    }
}

static inline void lp_sleep_fill_spi_slave_rx(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_SPI_LEN; i++) {
        buf[i] = (uint8_t)(0x90 + i);
    }
}

static inline void lp_sleep_fill_uart_tx(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_UART_LEN; i++) {
        buf[i] = (uint8_t)(0x30 + i);
    }
}

static inline void lp_sleep_fill_uart_rx(uint8_t *buf)
{
    for (int i = 0; i < LP_SLEEP_UART_LEN; i++) {
        buf[i] = (uint8_t)(0x80 + i);
    }
}
