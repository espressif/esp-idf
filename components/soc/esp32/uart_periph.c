/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#include "soc/uart_periph.h"

/*
 Bunch of constants for every UART peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM] = {
    {
        .pins = {
            [SOC_UART_PERIPH_SIGNAL_TX] = {
                .default_gpio = U0TXD_GPIO_NUM,
                .iomux_func = U0TXD_MUX_FUNC,
                .input = 0,
                .signal = U0TXD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RX] = {
                .default_gpio = U0RXD_GPIO_NUM,
                .iomux_func = U0RXD_MUX_FUNC,
                .input = 1,
                .signal = U0RXD_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RTS] = {
                .default_gpio = U0RTS_GPIO_NUM,
                .iomux_func = U0RTS_MUX_FUNC,
                .input = 0,
                .signal = U0RTS_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_CTS] = {
                .default_gpio = U0CTS_GPIO_NUM,
                .iomux_func = U0CTS_MUX_FUNC,
                .input = 1,
                .signal = U0CTS_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_DTR] = {
                .default_gpio = U0DTR_GPIO_NUM,
                .iomux_func = U0DTR_MUX_FUNC,
                .input = 0,
                .signal = U0DTR_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_DSR] = {
                .default_gpio = U0DSR_GPIO_NUM,
                .iomux_func = U0DSR_MUX_FUNC,
                .input = 1,
                .signal = U0DSR_IN_IDX,
            },
        },
        .irq = ETS_UART0_INTR_SOURCE,
    },

    {
        .pins = {
            [SOC_UART_PERIPH_SIGNAL_TX] = {
                .default_gpio = U1TXD_GPIO_NUM,
                .iomux_func = U1TXD_MUX_FUNC,
                .input = 0,
                .signal = U1TXD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RX] = {
                .default_gpio = U1RXD_GPIO_NUM,
                .iomux_func = U1RXD_MUX_FUNC,
                .input = 1,
                .signal = U1RXD_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RTS] = {
                .default_gpio = U1RTS_GPIO_NUM,
                .iomux_func = U1RTS_MUX_FUNC,
                .input = 0,
                .signal = U1RTS_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_CTS] = {
                .default_gpio = U1CTS_GPIO_NUM,
                .iomux_func = U1CTS_MUX_FUNC,
                .input = 1,
                .signal = U1CTS_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_DTR] = {
                .default_gpio = U1DTR_GPIO_NUM,
                .iomux_func = U1DTR_MUX_FUNC,
                .input = 0,
                .signal = UINT8_MAX, // No DTR signal on ESP32 UART1
            },

            [SOC_UART_PERIPH_SIGNAL_DSR] = {
                .default_gpio = U1DSR_GPIO_NUM,
                .iomux_func = U1DSR_MUX_FUNC,
                .input = 1,
                .signal = UINT8_MAX, // No DSR signal on ESP32 UART1
            },
        },
        .irq = ETS_UART1_INTR_SOURCE,
    },

    {
        .pins = {
            [SOC_UART_PERIPH_SIGNAL_TX] = {
                .default_gpio = U2TXD_GPIO_NUM,
                .iomux_func = U2TXD_MUX_FUNC,
                .input = 0,
                .signal = U2TXD_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RX] = {
                .default_gpio = U2RXD_GPIO_NUM,
                .iomux_func = U2RXD_MUX_FUNC,
                .input = 1,
                .signal = U2RXD_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_RTS] = {
                .default_gpio = U2RTS_GPIO_NUM,
                .iomux_func = U2RTS_MUX_FUNC,
                .input = 0,
                .signal = U2RTS_OUT_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_CTS] = {
                .default_gpio = U2CTS_GPIO_NUM,
                .iomux_func = U2CTS_MUX_FUNC,
                .input = 1,
                .signal = U2CTS_IN_IDX,
            },

            [SOC_UART_PERIPH_SIGNAL_DTR] = {
                .default_gpio = U2DTR_GPIO_NUM,
                .iomux_func = U2DTR_MUX_FUNC,
                .input = 0,
                .signal = UINT8_MAX, // No DTR signal on ESP32 UART2
            },

            [SOC_UART_PERIPH_SIGNAL_DSR] = {
                .default_gpio = U2DSR_GPIO_NUM,
                .iomux_func = U2DSR_MUX_FUNC,
                .input = 1,
                .signal = UINT8_MAX, // No DSR signal on ESP32 UART2
            },
        },
        .irq = ETS_UART2_INTR_SOURCE,
    }
};
