/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rmt_reg.h"
#include "rmt_private.h"

/**
 * RMT Registers to be saved during sleep retention
 * - Channel configuration registers, e.g.: RMT_CH0CONF0_REG, RMT_CH3CONF0_REG, RMT_CH3CONF1_REG, RMT_CH0_TX_LIM_REG, RMT_CH3_RX_LIM_REG
 * - TX synchronization registers, e.g.: RMT_TX_SIM_REG
 * - Interrupt enable registers, e.g.: RMT_INT_ENA_REG
 * - Carrier duty registers, e.g.: RMT_CH0CARRIER_DUTY_REG, RMT_CH3_RX_CARRIER_RM_REG
 * - Global configuration registers, e.g.: RMT_SYS_CONF_REG
 */
#define RMT_RETENTION_REGS_CNT 31
#define RMT_RETENTION_REGS_BASE (DR_REG_RMT_BASE + 0x20)
static const uint32_t rmt_regs_map[4] = {0xff400fff, 0x3ff, 0x0, 0x0};
static const regdma_entries_config_t rmt_regdma_entries[] = {
    // backup stage: save configuration registers
    // restore stage: restore the configuration registers
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_RMT_LINK(0x00),
                                            RMT_RETENTION_REGS_BASE, RMT_RETENTION_REGS_BASE,
                                            RMT_RETENTION_REGS_CNT, 0, 0,
                                            rmt_regs_map[0], rmt_regs_map[1],
                                            rmt_regs_map[2], rmt_regs_map[3]),
        .owner = ENTRY(0),
    },
};

const rmt_retention_desc_t rmt_retention_infos[1] = {
    [0] = {
        .module = SLEEP_RETENTION_MODULE_RMT0,
        .regdma_entry_array = rmt_regdma_entries,
        .array_size = ARRAY_SIZE(rmt_regdma_entries)
    },
};
