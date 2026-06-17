/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "parlio_priv.h"

/**
 * PARLIO Registers to be saved during sleep retention
 * - Configuration registers, e.g.: PARL_IO_RX_CFG0_REG, PARL_IO_RX_CFG1_REG, PARL_IO_TX_CFG0_REG, PARL_IO_TX_CFG1_REG, PARL_IO_CLK_REG
 * - Interrupt enable registers, e.g.: PARL_IO_INT_ENA_REG
*/
#define PARLIO_RETENTION_REGS_CNT 6
#define PARLIO_RETENTION_REGS_BASE (DR_REG_PARL_IO_BASE + 0x0)
static const uint32_t parlio_regs_map[4] = {0x2f, 0x0, 0x100, 0x0};
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

const parlio_retention_desc_t parlio_retention_infos[1] = {
    [0] = {
        .regdma_entry_array = parlio_regs_retention,
        .array_size = ARRAY_SIZE(parlio_regs_retention),
        .retention_module = SLEEP_RETENTION_MODULE_PARLIO0
    },
};
