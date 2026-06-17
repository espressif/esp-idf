/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/jpeg_periph.h"

// JPEG_CONFIG_REG JPEG_INT_ENA_REG
#define JPEG_RETENTION_REGS_CNT 2
#define JPEG_RETENTION_REGS_BASE (DR_REG_JPEG_BASE + 0x0)
static const uint32_t jpeg_regs_map[4] = {0x8001, 0x0, 0x0, 0x0};
static const regdma_entries_config_t jpeg_regdma_entries[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_JPEG_LINK(0x00),
                                            JPEG_RETENTION_REGS_BASE, JPEG_RETENTION_REGS_BASE,
                                            JPEG_RETENTION_REGS_CNT, 0, 0,
                                            jpeg_regs_map[0], jpeg_regs_map[1],
                                            jpeg_regs_map[2], jpeg_regs_map[3]),
        .owner = ENTRY(0) | ENTRY(2),
    },
};

const jpeg_reg_ctx_link_t jpeg_regs_retention = {
    .link_list = jpeg_regdma_entries,
    .link_num = ARRAY_SIZE(jpeg_regdma_entries),
    .module_id = SLEEP_RETENTION_MODULE_JPEG,
};
