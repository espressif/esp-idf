/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "ulp_riscv_gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    gpio_num_t tx_pin;  // TX pin number
} ulp_riscv_uart_cfg_t; // Config for the driver

typedef struct {
    uint32_t bit_duration_cycles; // Number of cycles to hold the line for each bit
    gpio_num_t tx_pin;            // TX pin number
} ulp_riscv_uart_t;               // Context for the driver, initialized by ulp_riscv_uart_init


/**
 * @brief Initialize the bit-banged UART driver
 *
 * @note Will also initialize the underlying HW, i.e. the RTC GPIO used.
 *
 * @param uart Pointer to the struct that will contain the initialized context
 * @param cfg  Pointer to the config struct which will be used to initialize the driver
 */
void ulp_riscv_uart_init(ulp_riscv_uart_t *uart, const ulp_riscv_uart_cfg_t *cfg);

/**
 * @brief Outputs a single byte on the tx pin
 *
 * @param uart Pointer to the initialized driver context
 * @param c    Byte to output
 */
void ulp_riscv_uart_putc(const ulp_riscv_uart_t *uart, const char c);

#ifdef __cplusplus
}
#endif
