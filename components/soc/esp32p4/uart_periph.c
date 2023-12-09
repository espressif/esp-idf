/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/uart_periph.h"
#include "soc/lp_gpio_sig_map.h"

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
                .signal = UART0_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U0RXD_GPIO_NUM,
                .iomux_func = U0RXD_MUX_FUNC,
                .input = 1,
                .signal = UART0_RXD_PAD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U0RTS_GPIO_NUM,
                .iomux_func = U0RTS_MUX_FUNC,
                .input = 0,
                .signal = UART0_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U0CTS_GPIO_NUM,
                .iomux_func = U0CTS_MUX_FUNC,
                .input = 1,
                .signal = UART0_CTS_PAD_IN_IDX,
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
                .signal = UART1_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U1RXD_GPIO_NUM,
                .iomux_func = U1RXD_MUX_FUNC,
                .input = 1,
                .signal = UART1_RXD_PAD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U1RTS_GPIO_NUM,
                .iomux_func = U1RTS_MUX_FUNC,
                .input = 0,
                .signal = UART1_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U1CTS_GPIO_NUM,
                .iomux_func = U1CTS_MUX_FUNC,
                .input = 1,
                .signal = UART1_CTS_PAD_IN_IDX,
            },
        },
        .irq = ETS_UART1_INTR_SOURCE,
        .module = PERIPH_UART1_MODULE,
    },

    { // HP UART2
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = U2TXD_GPIO_NUM,
                .iomux_func = U2TXD_MUX_FUNC,
                .input = 0,
                .signal = UART2_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U2RXD_GPIO_NUM,
                .iomux_func = U2RXD_MUX_FUNC,
                .input = 1,
                .signal = UART2_RXD_PAD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U2RTS_GPIO_NUM,
                .iomux_func = U2RTS_MUX_FUNC,
                .input = 0,
                .signal = UART2_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U2CTS_GPIO_NUM,
                .iomux_func = U2CTS_MUX_FUNC,
                .input = 1,
                .signal = UART2_CTS_PAD_IN_IDX,
            },
        },
        .irq = ETS_UART2_INTR_SOURCE,
        .module = PERIPH_UART2_MODULE,
    },

    { // HP UART3
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = U3TXD_GPIO_NUM,
                .iomux_func = U3TXD_MUX_FUNC,
                .input = 0,
                .signal = UART3_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U3RXD_GPIO_NUM,
                .iomux_func = U3RXD_MUX_FUNC,
                .input = 1,
                .signal = UART3_RXD_PAD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U3RTS_GPIO_NUM,
                .iomux_func = U3RTS_MUX_FUNC,
                .input = 0,
                .signal = UART3_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U3CTS_GPIO_NUM,
                .iomux_func = U3CTS_MUX_FUNC,
                .input = 1,
                .signal = UART3_CTS_PAD_IN_IDX,
            },
        },
        .irq = ETS_UART3_INTR_SOURCE,
        .module = PERIPH_UART3_MODULE,
    },

    { // HP UART4
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = U4TXD_GPIO_NUM,
                .iomux_func = U4TXD_MUX_FUNC,
                .input = 0,
                .signal = UART4_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = U4RXD_GPIO_NUM,
                .iomux_func = U4RXD_MUX_FUNC,
                .input = 1,
                .signal = UART4_RXD_PAD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = U4RTS_GPIO_NUM,
                .iomux_func = U4RTS_MUX_FUNC,
                .input = 0,
                .signal = UART4_RTS_PAD_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = U4CTS_GPIO_NUM,
                .iomux_func = U4CTS_MUX_FUNC,
                .input = 1,
                .signal = UART4_CTS_PAD_IN_IDX,
            },
        },
        .irq = ETS_UART4_INTR_SOURCE,
        .module = PERIPH_UART4_MODULE,
    },
    { // LP UART0
        .pins = {
            [SOC_UART_TX_PIN_IDX] = {
                .default_gpio = LP_U0TXD_GPIO_NUM,
                .iomux_func = LP_U0TXD_MUX_FUNC,
                .input = 0,
                .signal = LP_UART_TXD_PAD_OUT_IDX,
            },

            [SOC_UART_RX_PIN_IDX] = {
                .default_gpio = LP_U0RXD_GPIO_NUM,
                .iomux_func = LP_U0RXD_MUX_FUNC,
                .input = 1,
                .signal = LP_UART_RXD_PAD_IN_IDX,
            },

            [SOC_UART_RTS_PIN_IDX] = {
                .default_gpio = LP_U0RTS_GPIO_NUM,
                .iomux_func = LP_U0RTS_MUX_FUNC,
                .input = 0,
                .signal = LP_UART_RTSN_PAD_OUT_IDX,
            },

            [SOC_UART_CTS_PIN_IDX] = {
                .default_gpio = LP_U0CTS_GPIO_NUM,
                .iomux_func = LP_U0CTS_MUX_FUNC,
                .input = 1,
                .signal = LP_UART_CTSN_PAD_IN_IDX,
            },
        },
        .irq = ETS_LP_UART_INTR_SOURCE,
        .module = PERIPH_LP_UART0_MODULE,
    },
};
