/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MMU_MEM_CAP_EXEC  = BIT(0),
    MMU_MEM_CAP_READ  = BIT(1),
    MMU_MEM_CAP_WRITE = BIT(2),
    MMU_MEM_CAP_32BIT = BIT(3),
    MMU_MEM_CAP_8BIT  = BIT(4),
} mmu_mem_caps_t;

/**
 * MMU Page size
 */
typedef enum {
    MMU_PAGE_8KB  = 0x2000,
    MMU_PAGE_16KB = 0x4000,
    MMU_PAGE_32KB = 0x8000,
    MMU_PAGE_64KB = 0x10000,
} mmu_page_size_t;

/**
 * MMU virtual address flags type
 */
typedef enum {
    MMU_VADDR_DATA        = BIT(0),
    MMU_VADDR_INSTRUCTION = BIT(1),
} mmu_vaddr_t;

/**
 * External physical memory
 */
typedef enum {
    MMU_TARGET_FLASH0 = BIT(0),
    MMU_TARGET_PSRAM0 = BIT(1),
} mmu_target_t;

/**
 * MMU table id
 */
typedef enum {
    MMU_TABLE_CORE0,
    MMU_TABLE_CORE1,
} mmu_table_id_t;

#ifdef __cplusplus
}
#endif
