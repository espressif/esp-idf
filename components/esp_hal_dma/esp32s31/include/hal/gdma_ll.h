/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lp_peri_clkrst_struct.h"
#include "soc/soc_etm_source.h"

#define GDMA_LL_GET(_attr)          GDMA_LL_ ## _attr

#define GDMA_LL_INST_NUM            3

#define GDMA_LL_PAIRS_PER_INST      MAX(MAX(GDMA_LL_AHB_PAIRS_PER_GROUP, GDMA_LL_AXI_PAIRS_PER_GROUP), GDMA_LL_LP_AHB_PAIRS_PER_GROUP)

#define GDMA_LL_CHANNEL_MAX_PRIORITY 5 // supported priority levels: [0,5]

#define GDMA_LL_RX_EVENT_MASK       (0x7F)
#define GDMA_LL_TX_EVENT_MASK       (0x3F)

// the following event bits are identical for ahb-dma, axi-dma and lp-ahb-dma
#define GDMA_LL_EVENT_TX_FIFO_UDF   (1<<5)
#define GDMA_LL_EVENT_TX_FIFO_OVF   (1<<4)
#define GDMA_LL_EVENT_TX_TOTAL_EOF  (1<<3)
#define GDMA_LL_EVENT_TX_DESC_ERROR (1<<2)
#define GDMA_LL_EVENT_TX_EOF        (1<<1)
#define GDMA_LL_EVENT_TX_DONE       (1<<0)
#define GDMA_LL_EVENT_RX_FIFO_UDF   (1<<6)
#define GDMA_LL_EVENT_RX_FIFO_OVF   (1<<5)
#define GDMA_LL_EVENT_RX_DESC_EMPTY (1<<4)
#define GDMA_LL_EVENT_RX_DESC_ERROR (1<<3)
#define GDMA_LL_EVENT_RX_ERR_EOF    (1<<2)
#define GDMA_LL_EVENT_RX_SUC_EOF    (1<<1)
#define GDMA_LL_EVENT_RX_DONE       (1<<0)

#define GDMA_LL_AHB_GROUP_START_ID    0 // AHB GDMA group ID starts from 0
#define GDMA_LL_AHB_NUM_GROUPS        1 // Number of AHB GDMA groups
#define GDMA_LL_AHB_PAIRS_PER_GROUP   5 // Number of GDMA pairs in each AHB group

#define GDMA_LL_AXI_GROUP_START_ID    1 // AXI GDMA group ID starts from 1
#define GDMA_LL_AXI_NUM_GROUPS        1 // Number of AXI GDMA groups
#define GDMA_LL_AXI_PAIRS_PER_GROUP   3 // Number of GDMA pairs in each AXI group

#define GDMA_LL_LP_AHB_GROUP_START_ID 2 // LP AHB GDMA group ID starts from 2
#define GDMA_LL_LP_AHB_NUM_GROUPS     1 // Number of LP AHB GDMA groups
#define GDMA_LL_LP_AHB_PAIRS_PER_GROUP 2 // Number of GDMA pairs in each LP AHB group

#define GDMA_LL_AHB_PSRAM_CAPABLE     1
#define GDMA_LL_AXI_PSRAM_CAPABLE     1
#define GDMA_LL_LP_AHB_PSRAM_CAPABLE  1

#define GDMA_LL_AHB_BURST_SIZE_ADJUSTABLE 1  // AHB GDMA supports adjustable burst size
#define GDMA_LL_AHB_DESC_ALIGNMENT        4
#define GDMA_LL_AXI_DESC_ALIGNMENT        8
#define GDMA_LL_MAX_BURST_SIZE_PSRAM      128 // PSRAM controller doesn't support burst access with size > 128 bytes
#define GDMA_LL_ACCESS_ENCRYPTION_MEM_ALIGNMENT 16 // The alignment of the memory and size when DMA accesses encrypted memory

#define GDMA_LL_AHB_M2M_CAPABLE_PAIR_MASK   0x1F  // pair 0,1,2,3,4 are M2M capable
#define GDMA_LL_AXI_M2M_CAPABLE_PAIR_MASK   0x07  // pair 0,1,2 are M2M capable
#define GDMA_LL_LP_AHB_M2M_CAPABLE_PAIR_MASK 0x03 // pair 0,1 are M2M capable

#define GDMA_LL_TX_ETM_EVENT_TABLE(group, chan, event)                   \
    (uint32_t[GDMA_LL_INST_NUM][GDMA_ETM_EVENT_MAX]){                    \
        {                                                                \
            [GDMA_ETM_EVENT_EOF] = PDMA_AHB_EVT_OUT_EOF_CH0 + (chan),    \
        },                                                               \
        {                                                                \
            [GDMA_ETM_EVENT_EOF] = PDMA_AXI_EVT_OUT_EOF_CH0 + (chan),    \
        },                                                               \
        {                                                                \
            [GDMA_ETM_EVENT_EOF] = LP_PDMA_AHB_EVT_OUT_EOF_CH0 + (chan), \
        },                                                               \
    }[group][event]

#define GDMA_LL_RX_ETM_EVENT_TABLE(group, chan, event)                      \
    (uint32_t[GDMA_LL_INST_NUM][GDMA_ETM_EVENT_MAX]){                       \
        {                                                                   \
            [GDMA_ETM_EVENT_EOF] = PDMA_AHB_EVT_IN_SUC_EOF_CH0 + (chan),    \
        },                                                                  \
        {                                                                   \
            [GDMA_ETM_EVENT_EOF] = PDMA_AXI_EVT_IN_SUC_EOF_CH0 + (chan),    \
        },                                                                  \
        {                                                                   \
            [GDMA_ETM_EVENT_EOF] = LP_PDMA_AHB_EVT_IN_SUC_EOF_CH0 + (chan), \
        },                                                                  \
    }[group][event]

#define GDMA_LL_TX_ETM_TASK_TABLE(group, chan, task)                         \
    (uint32_t[GDMA_LL_INST_NUM][GDMA_ETM_TASK_MAX]){                         \
        {                                                                    \
            [GDMA_ETM_TASK_START] = PDMA_AHB_TASK_OUT_START_CH0 + (chan),    \
        },                                                                   \
        {                                                                    \
            [GDMA_ETM_TASK_START] = PDMA_AXI_TASK_OUT_START_CH0 + (chan),    \
        },                                                                   \
        {                                                                    \
            [GDMA_ETM_TASK_START] = LP_PDMA_AHB_TASK_OUT_START_CH0 + (chan), \
        },                                                                   \
    }[group][task]

#define GDMA_LL_RX_ETM_TASK_TABLE(group, chan, task)                        \
    (uint32_t[GDMA_LL_INST_NUM][GDMA_ETM_TASK_MAX]){                        \
        {                                                                   \
            [GDMA_ETM_TASK_START] = PDMA_AHB_TASK_IN_START_CH0 + (chan),    \
        },                                                                  \
        {                                                                   \
            [GDMA_ETM_TASK_START] = PDMA_AXI_TASK_IN_START_CH0 + (chan),    \
        },                                                                  \
        {                                                                   \
            [GDMA_ETM_TASK_START] = LP_PDMA_AHB_TASK_IN_START_CH0 + (chan), \
        },                                                                  \
    }[group][task]

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for the DMA module
 */
static inline void gdma_ll_enable_bus_clock(int group_id, bool enable)
{
    if (group_id == 0) {
        HP_SYS_CLKRST.ahb_pdma_ctrl0.reg_ahb_pdma_sys_clk_en = enable;
    } else if (group_id == 1) {
        HP_SYS_CLKRST.axi_pdma_ctrl0.reg_axi_pdma_sys_clk_en = enable;
    } else { // group_id == 2, LP AHB GDMA
        LP_PERI_CLKRST.ahb_dma_ctrl.lp_ahb_dma_clk_en = enable;
    }
}

/**
 * @brief Reset the DMA module
 */
static inline void _gdma_ll_reset_register(int group_id)
{
    if (group_id == 0) {
        HP_SYS_CLKRST.ahb_pdma_ctrl0.reg_ahb_pdma_rst_en = 1;
        HP_SYS_CLKRST.ahb_pdma_ctrl0.reg_ahb_pdma_rst_en = 0;
    } else if (group_id == 1) {
        HP_SYS_CLKRST.axi_pdma_ctrl0.reg_axi_pdma_rst_en = 1;
        HP_SYS_CLKRST.axi_pdma_ctrl0.reg_axi_pdma_rst_en = 0;
    } else {
        LP_PERI_CLKRST.ahb_dma_ctrl.lp_ahb_dma_rst_en = 1;
        LP_PERI_CLKRST.ahb_dma_ctrl.lp_ahb_dma_rst_en = 0;
    }
}

#define gdma_ll_reset_register(...) _gdma_ll_reset_register(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
