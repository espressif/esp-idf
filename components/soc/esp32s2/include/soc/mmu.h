/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/ext_mem_defs.h"
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defined for flash mmap */
#define SOC_MMU_REGIONS_COUNT                   6
#define SOC_MMU_PAGES_PER_REGION                64
#define SOC_MMU_IROM0_PAGES_START               (PRO_CACHE_IBUS0_MMU_START / sizeof(uint32_t))
#define SOC_MMU_IROM0_PAGES_END                 (PRO_CACHE_IBUS1_MMU_END / sizeof(uint32_t))
#define SOC_MMU_DROM0_PAGES_START               (PRO_CACHE_IBUS2_MMU_START / sizeof(uint32_t))
#define SOC_MMU_DROM0_PAGES_END                 (PRO_CACHE_IBUS2_MMU_END / sizeof(uint32_t))
#define SOC_MMU_ADDR_MASK                       SOC_MMU_VALID_VAL_MASK
#define SOC_MMU_PAGE_IN_FLASH(page)             ((page) | SOC_MMU_ACCESS_FLASH)
#define SOC_MMU_VADDR1_START_ADDR               SOC_IROM_MASK_LOW
#define SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE     ((SOC_MMU_VADDR1_FIRST_USABLE_ADDR - SOC_MMU_VADDR1_START_ADDR) / SPI_FLASH_MMU_PAGE_SIZE + SOC_MMU_IROM0_PAGES_START)
#define SOC_MMU_VADDR0_START_ADDR               SOC_DROM_LOW
#define SOC_MMU_VADDR1_FIRST_USABLE_ADDR        SOC_IROM_LOW

#ifdef __cplusplus
}
#endif
