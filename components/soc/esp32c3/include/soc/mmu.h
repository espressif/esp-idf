// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdint.h>
#include "soc/cache_memory.h"
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
#define SOC_MMU_INVALID_ENTRY_VAL               MMU_TABLE_INVALID_VAL
#define SOC_MMU_ADDR_MASK                       MMU_ADDRESS_MASK
#define SOC_MMU_PAGE_IN_FLASH(page)             (page) //Always in Flash
#define SOC_MMU_DPORT_PRO_FLASH_MMU_TABLE       FLASH_MMU_TABLE
#define SOC_MMU_VADDR1_START_ADDR               IRAM0_CACHE_ADDRESS_LOW
#define SOC_MMU_PRO_IRAM0_FIRST_USABLE_PAGE     SOC_MMU_IROM0_PAGES_START
#define SOC_MMU_VADDR0_START_ADDR               (SOC_DROM_LOW + (SOC_MMU_DROM0_PAGES_START * SPI_FLASH_MMU_PAGE_SIZE))
#define SOC_MMU_VADDR1_FIRST_USABLE_ADDR        SOC_IROM_LOW

#ifdef __cplusplus
}
#endif
