/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
    },
    { // HP UART2
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = U2TXD_GPIO_NUM,
                .iomux_func = U2TXD_MUX_FUNC,
                .input = 0,
                .signal = U2TXD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U2RXD_GPIO_NUM,
                .iomux_func = U2RXD_MUX_FUNC,
                .input = 1,
                .signal = U2RXD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U2RTS_GPIO_NUM,
                .iomux_func = U2RTS_MUX_FUNC,
                .input = 0,
                .signal = U2RTS_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U2CTS_GPIO_NUM,
                .iomux_func = U2CTS_MUX_FUNC,
                .input = 1,
                .signal = U2CTS_IN_IDX,
            },
        },
        .irq = ETS_UART2_INTR_SOURCE,
    },
};
