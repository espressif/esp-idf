/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "soc/soc_caps.h"
#if SOC_MEMPROT_SUPPORTED
#include "soc/memprot_defs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MEMPROT_SUPPORTED

/**
 * @brief Memprot LL error codes
 *
 */
typedef enum {
    MEMP_HAL_OK = 0,
    MEMP_HAL_ERR_SPLIT_ADDR_OUT_OF_RANGE = 2,
    MEMP_HAL_ERR_SPLIT_ADDR_INVALID = 2,         /* temporary duplicate for S2 builds */
    MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED = 3,
    MEMP_HAL_ERR_UNI_BLOCK_INVALID = 4,
    MEMP_HAL_ERR_AREA_INVALID = 5,
    MEMP_HAL_ERR_WORLD_INVALID = 6,
    MEMP_HAL_ERR_CORE_INVALID = 7,
    MEMP_HAL_FAIL = -1,
} memprot_hal_err_t;

/**
 * @brief Memprot LL PMS World IDs
 *
 */
typedef enum {
    MEMP_HAL_WORLD_NONE = 0x00,
    MEMP_HAL_WORLD_0 = 0x01,
    MEMP_HAL_WORLD_1 = 0x10
} memprot_hal_world_t;

/**
 * @brief Memprot LL PMS Area IDs
 *
 */
typedef enum {
    MEMP_HAL_AREA_NONE = 0,
    MEMP_HAL_AREA_LOW = 1,
    MEMP_HAL_AREA_HIGH = 2
} memprot_hal_area_t;

//auxiliary macros & defines

#define MEMP_HAL_CHECK_IRAM_ADDR_IN_RANGE(x) if (x < SOC_DIRAM_IRAM_LOW || x >= SOC_DIRAM_IRAM_HIGH) { return MEMP_HAL_ERR_SPLIT_ADDR_OUT_OF_RANGE; }
#define MEMP_HAL_CHECK_DRAM_ADDR_IN_RANGE(x) if (x < SOC_DIRAM_DRAM_LOW || x >= SOC_DIRAM_DRAM_HIGH) { return MEMP_HAL_ERR_SPLIT_ADDR_OUT_OF_RANGE; }
#define MEMP_HAL_CHECK_SPLIT_ADDR_ALIGNED(x) if (x % I_D_SPLIT_LINE_ALIGN != 0) { return MEMP_HAL_ERR_SPLIT_ADDR_UNALIGNED; }

#define MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_BELOW_SA  0x0 //0b00
#define MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_EQUAL_SA  0x2 //0b10
#define MEMP_HAL_CORE_X_IRAM0_DRAM0_DMA_SRAM_CATEGORY_BITS_ABOVE_SA  0x3 //0b11

#endif

#ifdef __cplusplus
}
#endif
