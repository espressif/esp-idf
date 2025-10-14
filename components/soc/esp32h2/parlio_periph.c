/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/parlio_periph.h"
#include "soc/gpio_sig_map.h"

const parlio_signal_conn_t parlio_periph_signals = {
    .groups = {
        [0] = {
            .module_name = "PARLIO0",
            .module = PERIPH_PARLIO_MODULE,
            .tx_irq_id = ETS_PARL_IO_TX_INTR_SOURCE,
            .rx_irq_id = ETS_PARL_IO_RX_INTR_SOURCE,
            .tx_units = {
                [0] = {
                    .data_sigs = {
                        PARL_TX_DATA0_IDX,
                        PARL_TX_DATA1_IDX,
                        PARL_TX_DATA2_IDX,
                        PARL_TX_DATA3_IDX,
                        PARL_TX_DATA4_IDX,
                        PARL_TX_DATA5_IDX,
                        PARL_TX_DATA6_IDX,
                        PARL_TX_DATA7_IDX,
                    },
                    .clk_out_sig = PARL_TX_CLK_OUT_IDX,
                    .clk_in_sig = PARL_TX_CLK_IN_IDX,
                    .cs_sig = -1,
                }
            },
            .rx_units = {
                [0] = {
                    .data_sigs = {
                        PARL_RX_DATA0_IDX,
                        PARL_RX_DATA1_IDX,
                        PARL_RX_DATA2_IDX,
                        PARL_RX_DATA3_IDX,
                        PARL_RX_DATA4_IDX,
                        PARL_RX_DATA5_IDX,
                        PARL_RX_DATA6_IDX,
                        PARL_RX_DATA7_IDX,
                    },
                    .clk_out_sig = PARL_RX_CLK_OUT_IDX,
                    .clk_in_sig = PARL_RX_CLK_IN_IDX,
                }
            }
        },
    },
};

/**
 * PARLIO Registers to be saved during sleep retention
 * - Tx Configuration registers, e.g.: PARL_IO_TX_DATA_CFG_REG, PARL_IO_TX_GENRL_CFG_REG
 * - Rx Configuration registers, e.g.: PARL_IO_RX_MODE_CFG_REG, PARL_IO_RX_DATA_CFG_REG, PARL_IO_RX_GENRL_CFG_REG
 * - CLK Configuration registers, e.g.: PARL_IO_RX_CLK_CFG_REG, PARL_IO_TX_CLK_CFG_REG
 * - Interrupt enable registers, e.g.: PARL_IO_INT_ENA_REG
*/
#define PARLIO_RETENTION_REGS_CNT 8
#define PARLIO_RETENTION_REGS_BASE (DR_REG_PARL_IO_BASE + 0x0)
static const uint32_t parlio_regs_map[4] = {0x60457, 0x0, 0x0, 0x0};
static const regdma_entries_config_t parlio_regs_retention[] = {
    // backup stage: save configuration registers
    // restore stage: restore the configuration registers
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_PARLIO_LINK(0x00), \
                                            PARLIO_RETENTION_REGS_BASE, PARLIO_RETENTION_REGS_BASE, \
                                            PARLIO_RETENTION_REGS_CNT, 0, 0, \
                                            parlio_regs_map[0], parlio_regs_map[1], \
                                            parlio_regs_map[2], parlio_regs_map[3]), \
        .owner = ENTRY(0) | ENTRY(2)
    }, \
};
const parlio_reg_retention_info_t parlio_reg_retention_info[SOC_PARLIO_GROUPS] = {
    [0] = {
        .regdma_entry_array = parlio_regs_retention,
        .array_size = ARRAY_SIZE(parlio_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_PARLIO0
    },
};
