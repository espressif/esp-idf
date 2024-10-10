/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/uart_periph.h"
#include "soc/uart_reg.h"

/*
 Bunch of constants for every UART peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const uart_signal_conn_t uart_periph_signal[SOC_UART_NUM] = {
   {
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

    {
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
};

/**
 * UART registers to be saved during sleep retention
 *
 * Reset TXFIFO and RXFIFO
 * UART registers require set the reg_update bit to make the configuration take effect
 *
 * UART_INT_ENA_REG, UART_CLKDIV_SYNC_REG, UART_RX_FILT_REG, UART_CONF0_SYNC_REG, UART_CONF1_REG,
 * UART_HWFC_CONF_SYNC_REG, UART_SLEEP_CONF0_REG, UART_SLEEP_CONF1_REG, UART_SLEEP_CONF2_REG,
 * UART_SWFC_CONF0_SYNC_REG, UART_SWFC_CONF1_REG, UART_TXBRK_CONF_SYNC_REG, UART_IDLE_CONF_SYNC_REG,
 * UART_RS485_CONF_SYNC_REG, UART_AT_CMD_PRECNT_SYNC_REG, UART_AT_CMD_POSTCNT_SYNC_REG, UART_AT_CMD_GAPTOUT_SYNC_REG,
 * UART_AT_CMD_CHAR_SYNC_REG, UART_MEM_CONF_REG, UART_TOUT_CONF_SYNC_REG, UART_ID_REG
 */
#define UART_RETENTION_ADDR_MAP_REGS_CNT 21
#define UART_RETENTION_REGS_BASE(i) UART_INT_ENA_REG(i)
static const uint32_t uart_regs_map[4] = {0x7fff6d, 0x10, 0x0, 0x0};
#define UART_SLEEP_RETENTION_ENTRIES(uart_num) { \
    [0] = {.config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_UART_LINK(0x00), \
                                               UART_RETENTION_REGS_BASE(uart_num), UART_RETENTION_REGS_BASE(uart_num), \
                                               UART_RETENTION_ADDR_MAP_REGS_CNT, 0, 0, \
                                               uart_regs_map[0], uart_regs_map[1], \
                                               uart_regs_map[2], uart_regs_map[3] \
                                              ), \
           .owner = ENTRY(0) | ENTRY(2) }, \
    [1] = {.config = REGDMA_LINK_WRITE_INIT(REGDMA_UART_LINK(0x01), \
                                            UART_REG_UPDATE_REG(uart_num), UART_REG_UPDATE, \
                                            UART_REG_UPDATE_M, 1, 0 \
                                           ), \
           .owner = ENTRY(0) | ENTRY(2) }, \
    [2] = {.config = REGDMA_LINK_WAIT_INIT(REGDMA_UART_LINK(0x02), \
                                           UART_REG_UPDATE_REG(uart_num), 0x0, \
                                           UART_REG_UPDATE_M, 1, 0 \
                                          ), \
           .owner = ENTRY(0) | ENTRY(2) }, \
}

static const regdma_entries_config_t uart0_regdma_entries[] = UART_SLEEP_RETENTION_ENTRIES(0);
static const regdma_entries_config_t uart1_regdma_entries[] = UART_SLEEP_RETENTION_ENTRIES(1);

const uart_reg_retention_info_t uart_reg_retention_info[SOC_UART_HP_NUM] = {
    [0] = {
        .module = SLEEP_RETENTION_MODULE_UART0,
        .regdma_entry_array = uart0_regdma_entries,
        .array_size = ARRAY_SIZE(uart0_regdma_entries),
    },
    [1] = {
        .module = SLEEP_RETENTION_MODULE_UART1,
        .regdma_entry_array = uart1_regdma_entries,
        .array_size = ARRAY_SIZE(uart1_regdma_entries),
    },
};
