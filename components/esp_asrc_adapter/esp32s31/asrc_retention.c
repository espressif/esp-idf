/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include "soc/asrc_reg.h"
#include "asrc_adapter_priv.h"

/**
 * ASRC Registers to be saved during sleep retention
 *
 * Retained R/W registers per channel (WT/RO registers excluded):
 * CHNL0: CFG0(0x00) CFG1(0x04) CFG2(0x08) CFG4(0x10) CFG5(0x14) CFG6(0x18) INT_ENA(0x28)
 * CHNL1: CFG0(0x3c) CFG1(0x40) CFG2(0x44) CFG4(0x4c) CFG5(0x50) CFG6(0x54) INT_ENA(0x64)
 *
 */
#define ASRC_RETENTION_REGS_CNT  14
#define ASRC_RETENTION_REGS_BASE (DR_REG_ASRC_BASE + 0x0)

static const uint32_t                asrc_regs_map[4]      = {0x023B8477, 0x0, 0x0, 0x0};
static const regdma_entries_config_t asrc_regdma_entries[] = {
    // backup stage: save configuration registers
    // restore stage: restore the configuration registers
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_ASRC_LINK(0x00),
                                            ASRC_RETENTION_REGS_BASE, ASRC_RETENTION_REGS_BASE,
                                            ASRC_RETENTION_REGS_CNT, 0, 0,
                                            asrc_regs_map[0], asrc_regs_map[1],
                                            asrc_regs_map[2], asrc_regs_map[3]),
        .owner  = ENTRY(0) | ENTRY(2),
    },
};

const asrc_reg_retention_info_t asrc_reg_retention_info[1] = {
    [0] = {
        .retention_module = SLEEP_RETENTION_MODULE_ASRC,
        .entry_array      = asrc_regdma_entries,
        .array_size       = ARRAY_SIZE(asrc_regdma_entries),
    },
};
