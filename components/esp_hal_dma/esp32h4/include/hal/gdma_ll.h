/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/pcr_struct.h"
#include "hal/ahb_dma_ll.h"

#define GDMA_LL_GET(_attr)          GDMA_LL_ ## _attr

#define GDMA_LL_INST_NUM            1
#define GDMA_LL_PAIRS_PER_INST      GDMA_LL_AHB_PAIRS_PER_GROUP

#define GDMA_LL_AHB_PSRAM_CAPABLE         1

#define GDMA_LL_AHB_BURST_SIZE_ADJUSTABLE 1  // AHB GDMA supports adjustable burst size
#define GDMA_LL_MAX_BURST_SIZE_PSRAM      64 // PSRAM support INCR16
#define GDMA_LL_ACCESS_ENCRYPTION_MEM_ALIGNMENT 16 // The alignment of the memory and size when DMA accesses encrypted memory

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for the DMA module
 */
static inline void gdma_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    PCR.gdma_conf.gdma_clk_en = enable;
}

/**
 * @brief Reset the DMA module
 */
static inline void _gdma_ll_reset_register(int group_id)
{
    (void)group_id;
    PCR.gdma_conf.gdma_rst_en = 1;
    PCR.gdma_conf.gdma_rst_en = 0;
}

#define gdma_ll_reset_register(...) _gdma_ll_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
