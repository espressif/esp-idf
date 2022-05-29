/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/dport_reg.h"
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Defined for flash mmap */
#define SOC_MMU_REGIONS_COUNT                   4
#define SOC_MMU_PAGES_PER_REGION                64
#define SOC_MMU_IROM0_PAGES_START               64
#define SOC_MMU_IROM0_PAGES_END                 256
#define SOC_MMU_DROM0_PAGES_START               0
#define SOC_MMU_DROM0_PAGES_END                 64
#define SOC_MMU_ADDR_MASK                       DPORT_MMU_ADDRESS_MASK
#define SOC_MMU_PAGE_IN_FLASH(page)             (page)
#define SOC_MMU_VADDR1_START_ADDR               SOC_IROM_MASK_LOW
#define SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE     ((SOC_MMU_VADDR1_FIRST_USABLE_ADDR - SOC_MMU_VADDR1_START_ADDR) / SPI_FLASH_MMU_PAGE_SIZE + SOC_MMU_IROM0_PAGES_START)
#define SOC_MMU_VADDR0_START_ADDR               SOC_DROM_LOW
#define SOC_MMU_VADDR1_FIRST_USABLE_ADDR        SOC_IROM_LOW

#ifdef __cplusplus
}
#endif
