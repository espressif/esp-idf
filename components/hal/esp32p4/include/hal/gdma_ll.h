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

#ifdef __cplusplus
extern "C" {
#endif

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

__attribute__((always_inline))
static inline uint8_t _bitwise_reverse(uint8_t n)
{
    n = ((n & 0xf0) >> 4) | ((n & 0x0f) << 4);
    n = ((n & 0xcc) >> 2) | ((n & 0x33) << 2);
    n = ((n & 0xaa) >> 1) | ((n & 0x55) << 1);
    return n;
}

#ifdef __cplusplus
}
#endif
