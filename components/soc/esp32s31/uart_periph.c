/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/uart_periph.h"
#include "soc/lp_gpio_sig_map.h"
#include "soc/uart_reg.h"

// TODO: to be checked IDF-14789

/*
    Bunch of constants for every UART peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM] = {
    {
        // HP UART0
        .pins = {
            [SOC_UART_PERIPH_SIGNAL_TX] = {
                .default_gpio = U0TXD_GPIO_NUM,
                .iomux_func = U0TXD_MUX_FUNC,
                .input = 0,
                .signal = UART0_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RX] = {
                .default_gpio = U0RXD_GPIO_NUM,
                .iomux_func = U0RXD_MUX_FUNC,
                .input = 1,
                .signal = UART0_RXD_PAD_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RTS] = {
                .default_gpio = U0RTS_GPIO_NUM,
                .iomux_func = U0RTS_MUX_FUNC,
                .input = 0,
                .signal = UART0_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_CTS] = {
                .default_gpio = U0CTS_GPIO_NUM,
                .iomux_func = U0CTS_MUX_FUNC,
                .input = 1,
                .signal = UART0_CTS_PAD_IN_IDX,
            }
        },
        .irq = ETS_UART0_INTR_SOURCE,
    },

    {
        // HP UART1
        .pins = {
            [SOC_UART_PERIPH_SIGNAL_TX] = {
                .default_gpio = U1TXD_GPIO_NUM,
                .iomux_func = U1TXD_MUX_FUNC,
                .input = 0,
                .signal = UART1_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RX] = {
                .default_gpio = U1RXD_GPIO_NUM,
                .iomux_func = U1RXD_MUX_FUNC,
                .input = 1,
                .signal = UART1_RXD_PAD_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RTS] = {
                .default_gpio = U1RTS_GPIO_NUM,
                .iomux_func = U1RTS_MUX_FUNC,
                .input = 0,
                .signal = UART1_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_CTS] = {
                .default_gpio = U1CTS_GPIO_NUM,
                .iomux_func = U1CTS_MUX_FUNC,
                .input = 1,
                .signal = UART1_CTS_PAD_IN_IDX,
            },
        },
        .irq = ETS_UART1_INTR_SOURCE,
    },

    {
        // HP UART2
        .pins = {
            [SOC_UART_PERIPH_SIGNAL_TX] = {
                .default_gpio = U2TXD_GPIO_NUM,
                .iomux_func = U2TXD_MUX_FUNC,
                .input = 0,
                .signal = UART2_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RX] = {
                .default_gpio = U2RXD_GPIO_NUM,
                .iomux_func = U2RXD_MUX_FUNC,
                .input = 1,
                .signal = UART2_RXD_PAD_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RTS] = {
                .default_gpio = U2RTS_GPIO_NUM,
                .iomux_func = U2RTS_MUX_FUNC,
                .input = 0,
                .signal = UART2_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_CTS] = {
                .default_gpio = U2CTS_GPIO_NUM,
                .iomux_func = U2CTS_MUX_FUNC,
                .input = 1,
                .signal = UART2_CTS_PAD_IN_IDX,
            },
        },
        .irq = ETS_UART2_INTR_SOURCE,
    },
    {
        // HP UART3
        .pins = {
            [SOC_UART_PERIPH_SIGNAL_TX] = {
                .default_gpio = U3TXD_GPIO_NUM,
                .iomux_func = U3TXD_MUX_FUNC,
                .input = 0,
                .signal = -1,
            },

            [SOC_UART_PERIPH_SIGNAL_RX] = {
                .default_gpio = U3RXD_GPIO_NUM,
                .iomux_func = U3RXD_MUX_FUNC,
                .input = 1,
                .signal = -1,
            },

            [SOC_UART_PERIPH_SIGNAL_RTS] = {
                .default_gpio = U3RTS_GPIO_NUM,
                .iomux_func = U3RTS_MUX_FUNC,
                .input = 0,
                .signal = -1,
            },

            [SOC_UART_PERIPH_SIGNAL_CTS] = {
                .default_gpio = U3CTS_GPIO_NUM,
                .iomux_func = U3CTS_MUX_FUNC,
                .input = 1,
                .signal = -1,
            },
        },
        .irq = ETS_UART3_INTR_SOURCE,
    },
};
