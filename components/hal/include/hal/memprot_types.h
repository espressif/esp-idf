/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/memprot_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Memprot LL error codes
 *
 */
typedef enum {
    MEMP_LL_OK = 0,
    MEMP_LL_ERR_SPLIT_ADDR_OUT_OF_RANGE = 2,
    MEMP_LL_ERR_SPLIT_ADDR_INVALID = 2,         /* temporary duplicate for S2 builds */
    MEMP_LL_ERR_SPLIT_ADDR_UNALIGNED = 3,
    MEMP_LL_ERR_UNI_BLOCK_INVALID = 4,
    MEMP_LL_ERR_AREA_INVALID = 5,
    MEMP_LL_ERR_WORLD_INVALID = 6,
    MEMP_LL_ERR_CORE_INVALID = 7,
    MEMP_LL_FAIL = -1,
} memprot_ll_err_t;

/**
 * @brief Memprot LL PMS World IDs
 *
 */
typedef enum {
    MEMP_LL_WORLD_NONE = 0x00,
    MEMP_LL_WORLD_0 = 0x01,
    MEMP_LL_WORLD_1 = 0x10
} memprot_ll_world_t;

/**
 * @brief Memprot LL PMS Area IDs
 *
 */
typedef enum {
    MEMP_LL_AREA_NONE = 0,
    MEMP_LL_AREA_LOW = 1,
    MEMP_LL_AREA_HIGH = 2
} memprot_ll_area_t;

//auxiliary macros & defines
#define SOC_I_D_OFFSET (SOC_DIRAM_IRAM_LOW - SOC_DIRAM_DRAM_LOW)
#define MAP_DRAM_TO_IRAM(addr) (addr + SOC_I_D_OFFSET)
#define MAP_IRAM_TO_DRAM(addr) (addr - SOC_I_D_OFFSET)

#define MEMP_LL_CHECK_IRAM_ADDR_IN_RANGE(x) if (x < SOC_DIRAM_IRAM_LOW || x >= SOC_DIRAM_IRAM_HIGH) { return MEMP_LL_ERR_SPLIT_ADDR_OUT_OF_RANGE; }
#define MEMP_LL_CHECK_DRAM_ADDR_IN_RANGE(x) if (x < SOC_DIRAM_DRAM_LOW || x >= SOC_DIRAM_DRAM_HIGH) { return MEMP_LL_ERR_SPLIT_ADDR_OUT_OF_RANGE; }
#define MEMP_LL_CHECK_SPLIT_ADDR_ALIGNED(x) if (x % I_D_SPLIT_LINE_ALIGN != 0) { return MEMP_LL_ERR_SPLIT_ADDR_UNALIGNED; }

#define MEMP_LL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_BELOW_SA  0x0 //0b00
#define MEMP_LL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA  0x2 //0b10
#define MEMP_LL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_ABOVE_SA  0x3 //0b11

#ifdef __cplusplus
}
#endif
