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
#define SOC_MMU_REGIONS_COUNT                   1
#define SOC_MMU_PAGES_PER_REGION                128
#define SOC_MMU_IROM0_PAGES_START               (CACHE_IROM_MMU_START / sizeof(uint32_t))
#define SOC_MMU_IROM0_PAGES_END                 (CACHE_IROM_MMU_END / sizeof(uint32_t))
#define SOC_MMU_DROM0_PAGES_START               (CACHE_DROM_MMU_START / sizeof(uint32_t))
#define SOC_MMU_DROM0_PAGES_END                 (CACHE_DROM_MMU_END / sizeof(uint32_t))
#define SOC_MMU_ADDR_MASK                       SOC_MMU_VALID_VAL_MASK
#define SOC_MMU_PAGE_IN_FLASH(page)             (page) //Always in Flash
#define SOC_MMU_VADDR1_START_ADDR               SOC_IRAM0_CACHE_ADDRESS_LOW
#define SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE     SOC_MMU_IROM0_PAGES_START
#define SOC_MMU_VADDR0_START_ADDR               (SOC_DROM_LOW + (SOC_MMU_DROM0_PAGES_START * SPI_FLASH_MMU_PAGE_SIZE))
#define SOC_MMU_VADDR1_FIRST_USABLE_ADDR        SOC_IROM_LOW

#ifdef __cplusplus
}
#endif
