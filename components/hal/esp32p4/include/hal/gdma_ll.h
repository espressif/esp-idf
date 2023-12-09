/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief The contents defined in this file are common for both AXI-DMA and AHB-DMA
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/hp_sys_clkrst_struct.h"

#define GDMA_LL_CHANNEL_MAX_PRIORITY 5 // supported priority levels: [0,5]

#define GDMA_LL_RX_EVENT_MASK       (0x1F)
#define GDMA_LL_TX_EVENT_MASK       (0x0F)

#define GDMA_LL_INVALID_PERIPH_ID   (0x3F)

#define GDMA_LL_EVENT_TX_TOTAL_EOF  (1<<3)
#define GDMA_LL_EVENT_TX_DESC_ERROR (1<<2)
#define GDMA_LL_EVENT_TX_EOF        (1<<1)
#define GDMA_LL_EVENT_TX_DONE       (1<<0)

#define GDMA_LL_EVENT_RX_DESC_EMPTY (1<<4)
#define GDMA_LL_EVENT_RX_DESC_ERROR (1<<3)
#define GDMA_LL_EVENT_RX_ERR_EOF    (1<<2)
#define GDMA_LL_EVENT_RX_SUC_EOF    (1<<1)
#define GDMA_LL_EVENT_RX_DONE       (1<<0)

#define GDMA_LL_AHB_GROUP_START_ID    0 // AHB GDMA group ID starts from 0
#define GDMA_LL_AHB_NUM_GROUPS        1 // Number of AHB GDMA groups
#define GDMA_LL_AHB_PAIRS_PER_GROUP   3 // Number of GDMA pairs in each AHB group

#define GDMA_LL_AXI_GROUP_START_ID    1 // AXI GDMA group ID starts from 1
#define GDMA_LL_AXI_NUM_GROUPS        1 // Number of AXI GDMA groups
#define GDMA_LL_AXI_PAIRS_PER_GROUP   3 // Number of GDMA pairs in each AXI group

#define GDMA_LL_PARALLEL_CRC_DATA_WIDTH 8  // Parallel CRC data width is fixed to 8bits
#define GDMA_LL_AHB_MAX_CRC_BIT_WIDTH   32 // Max CRC bit width supported by AHB GDMA
#define GDMA_LL_AXI_MAX_CRC_BIT_WIDTH   16 // Max CRC bit width supported by AXI GDMA

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for the DMA module
 */
static inline void gdma_ll_enable_bus_clock(int group_id, bool enable)
{
    if (group_id == 0) {
        HP_SYS_CLKRST.soc_clk_ctrl1.reg_ahb_pdma_sys_clk_en = enable;
    } else {
        HP_SYS_CLKRST.soc_clk_ctrl1.reg_axi_pdma_sys_clk_en = enable;
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define gdma_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; gdma_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the DMA module
 */
static inline void gdma_ll_reset_register(int group_id)
{
    if (group_id == 0) {
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_ahb_pdma = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_ahb_pdma = 0;
    } else {
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_axi_pdma = 1;
        HP_SYS_CLKRST.hp_rst_en1.reg_rst_en_axi_pdma = 0;
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define gdma_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; gdma_ll_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
