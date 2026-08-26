/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ppa_priv.h"
#include "soc/ppa_reg.h"

/** PPA Registers to be saved during sleep retention
 *
 * For CLUT mem, we shall choose not to power down the CLUT mem domain; Otherwise, there are two blocks of memory that needs to be restored by writing to mem addr directly.
 * Since PPA driver is operation based, most of the registers are configured per operation, so no need to save/restore.
 *
 * PPA_CLUT_CONF_REG,
 * PPA_INT_ENA_REG,
 * PPA_SR_MEM_PD_REG, PPA_REG_CONF_REG, PPA_SRAM_CTRL_REG,
 * PPA_RGB2GRAY_REG (> rev3 only, but ok to include it for < rev3 chips)
 */
#define PPA_RETENTION_REGS_CNT 6
#define PPA_RETENTION_REGS_BASE PPA_CLUT_CONF_REG
static const uint32_t ppa_regs_map[4] = {0x1800009, 0x9, 0x0, 0x0};
static const regdma_entries_config_t ppa_regdma_entries[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_PPA_LINK(0x00),
                                            PPA_RETENTION_REGS_BASE, PPA_RETENTION_REGS_BASE,
                                            PPA_RETENTION_REGS_CNT, 0, 0,
                                            ppa_regs_map[0], ppa_regs_map[1],
                                            ppa_regs_map[2], ppa_regs_map[3]),
        .owner = ENTRY(0),
    },
};

const ppa_retention_desc_t ppa_reg_retention_info = {
    .module = SLEEP_RETENTION_MODULE_PPA,
    .regdma_entry_array = ppa_regdma_entries,
    .array_size = ARRAY_SIZE(ppa_regdma_entries),
};
