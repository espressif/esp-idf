/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/uart_periph.h"

/*
 Bunch of constants for every UART peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM] = {
   { // HP UART0
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = U0TXD_GPIO_NUM,
                .iomux_func = U0TXD_MUX_FUNC,
                .input = 0,
                .signal = U0TXD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U0RXD_GPIO_NUM,
                .iomux_func = U0RXD_MUX_FUNC,
                .input = 1,
                .signal = U0RXD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U0RTS_GPIO_NUM,
                .iomux_func = U0RTS_MUX_FUNC,
                .input = 0,
                .signal = U0RTS_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U0CTS_GPIO_NUM,
                .iomux_func = U0CTS_MUX_FUNC,
                .input = 1,
                .signal = U0CTS_IN_IDX,
            }
        },
        .irq = ETS_UART0_INTR_SOURCE,
        .module = PERIPH_UART0_MODULE,
    },

    { // HP UART1
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = U1TXD_GPIO_NUM,
                .iomux_func = U1TXD_MUX_FUNC,
                .input = 0,
                .signal = U1TXD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U1RXD_GPIO_NUM,
                .iomux_func = U1RXD_MUX_FUNC,
                .input = 1,
                .signal = U1RXD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U1RTS_GPIO_NUM,
                .iomux_func = U1RTS_MUX_FUNC,
                .input = 0,
                .signal = U1RTS_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U1CTS_GPIO_NUM,
                .iomux_func = U1CTS_MUX_FUNC,
                .input = 1,
                .signal = U1CTS_IN_IDX,
            },
        },
        .irq = ETS_UART1_INTR_SOURCE,
        .module = PERIPH_UART1_MODULE,
    },

    { // LP UART0
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = LP_U0TXD_GPIO_NUM,
                .iomux_func = LP_U0TXD_MUX_FUNC,
                .input = 0,
                .signal = UINT8_MAX, // Signal not available in signal map
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = LP_U0RXD_GPIO_NUM,
                .iomux_func = LP_U0RXD_MUX_FUNC,
                .input = 1,
                .signal = UINT8_MAX, // Signal not available in signal map
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = LP_U0RTS_GPIO_NUM,
                .iomux_func = LP_U0RTS_MUX_FUNC,
                .input = 0,
                .signal = UINT8_MAX, // Signal not available in signal map
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = LP_U0CTS_GPIO_NUM,
                .iomux_func = LP_U0CTS_MUX_FUNC,
                .input = 1,
                .signal = UINT8_MAX, // Signal not available in signal map
            },
        },
        .irq = ETS_LP_UART_INTR_SOURCE,
        .module = PERIPH_LP_UART0_MODULE,
    },
};
