/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "ulp_riscv.h"
#include "ulp_riscv_utils.h"
#include "ulp_riscv_gpio.h"
#include "ulp_riscv_uart_ulp_core.h"

/* We calculate the bit duration at compile time to speed up and avoid pulling in soft-float libs */
#define BIT_DURATION_CYCLES ( ULP_RISCV_CYCLES_PER_US * ((1000*1000) / CONFIG_ULP_RISCV_UART_BAUDRATE) )

void ulp_riscv_uart_init(ulp_riscv_uart_t *uart, const ulp_riscv_uart_cfg_t *cfg)
{
    uart->tx_pin = cfg->tx_pin;
    /* 1 bit duration with length given in clock cycles */
    uart->bit_duration_cycles = BIT_DURATION_CYCLES;

    /* Setup GPIO used for uart TX */
    ulp_riscv_gpio_init(cfg->tx_pin);
    ulp_riscv_gpio_output_enable(cfg->tx_pin);
    ulp_riscv_gpio_set_output_mode(cfg->tx_pin, RTCIO_MODE_OUTPUT_OD);
    ulp_riscv_gpio_pullup(cfg->tx_pin);
    ulp_riscv_gpio_pulldown_disable(cfg->tx_pin);
    ulp_riscv_gpio_output_level(cfg->tx_pin, 1);

}

void ulp_riscv_uart_putc(const ulp_riscv_uart_t *uart, const char c)
{
    ulp_riscv_gpio_output_level(uart->tx_pin, 0);

    for (int i = 0; i<8; i++) {
        /* Offset the delay to account for cycles spent setting the bit */
        ulp_riscv_delay_cycles(uart->bit_duration_cycles - 100);
        if ( (1 <<  i) & c) {
            ulp_riscv_gpio_output_level(uart->tx_pin, 1);
        } else {
            ulp_riscv_gpio_output_level(uart->tx_pin, 0);
        }

    }

    ulp_riscv_delay_cycles(uart->bit_duration_cycles - 20);
    ulp_riscv_gpio_output_level(uart->tx_pin, 1);
    ulp_riscv_delay_cycles(uart->bit_duration_cycles);
}
