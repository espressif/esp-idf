/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "dma2d_priv.h"
#include "soc/dma2d_reg.h"

/** DMA2D Registers to be saved during sleep retention
 *
 * DMA2D driver is implemented in the way that channels are used as a pool,
 * therefore, only general registers are needed to be saved during sleep retention.
 *
 * DMA2D_RST_CONF_REG,
 * DMA2D_INTR_MEM_START_ADDR_REG, DMA2D_INTR_MEM_END_ADDR_REG, DMA2D_EXTR_MEM_START_ADDR_REG, DMA2D_EXTR_MEM_END_ADDR_REG
 * DMA2D_OUT_ARB_CONFIG_REG, DMA2D_IN_ARB_CONFIG_REG
 */
#define DMA2D_RETENTION_REGS_CNT 7
#define DMA2D_RETENTION_REGS_BASE DMA2D_RST_CONF_REG
static const uint32_t dma2d_regs_map[4] = {0x7f, 0x0, 0x0, 0x0};
static const regdma_entries_config_t dma2d_regdma_entries[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_DMA2D_LINK(0x00),
                                            DMA2D_RETENTION_REGS_BASE, DMA2D_RETENTION_REGS_BASE,
                                            DMA2D_RETENTION_REGS_CNT, 0, 0,
                                            dma2d_regs_map[0], dma2d_regs_map[1],
                                            dma2d_regs_map[2], dma2d_regs_map[3]),
        .owner = ENTRY(0),
    },
};

const dma2d_retention_desc_t dma2d_reg_retention_info = {
    .module = SLEEP_RETENTION_MODULE_DMA2D,
    .regdma_entry_array = dma2d_regdma_entries,
    .array_size = ARRAY_SIZE(dma2d_regdma_entries),
};
