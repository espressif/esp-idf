/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !SOC_MMU_PAGE_SIZE
/**
 * We define `SOC_MMU_PAGE_SIZE` in soc/CMakeLists.txt.
 * Here we give a default definition, if SOC_MMU_PAGE_SIZE doesn't exist. This is to pass the check_public_headers.py
 */
#define SOC_MMU_PAGE_SIZE 0x10000
#endif


#define IRAM0_CACHE_ADDRESS_LOW                  0x40000000
#define IRAM0_CACHE_ADDRESS_HIGH                 0x50000000

#define DRAM0_CACHE_ADDRESS_LOW                  IRAM0_CACHE_ADDRESS_LOW                //I/D share the same vaddr range
#define DRAM0_CACHE_ADDRESS_HIGH                 IRAM0_CACHE_ADDRESS_HIGH               //I/D share the same vaddr range

#define DRAM_FLASH_ADDRESS_LOW                   DRAM0_CACHE_ADDRESS_LOW
#define DRAM_FLASH_ADDRESS_HIGH                  0x44000000

#define DRAM_PSRAM_ADDRESS_LOW                   0x48000000
#define DRAM_PSRAM_ADDRESS_HIGH                  0x4C000000

#define BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM_FLASH(vaddr)       ADDRESS_IN_BUS(DRAM_FLASH, vaddr)
#define ADDRESS_IN_DRAM_PSRAM(vaddr)       ADDRESS_IN_BUS(DRAM_PSRAM, vaddr)

//TODO, remove these cache function dependencies
#define CACHE_IROM_MMU_START            0
#define CACHE_IROM_MMU_END              Cache_Get_IROM_MMU_End()
#define CACHE_IROM_MMU_SIZE             (CACHE_IROM_MMU_END - CACHE_IROM_MMU_START)

#define CACHE_DROM_MMU_START            CACHE_IROM_MMU_END
#define CACHE_DROM_MMU_END              Cache_Get_DROM_MMU_End()
#define CACHE_DROM_MMU_SIZE             (CACHE_DROM_MMU_END - CACHE_DROM_MMU_START)

#define CACHE_DROM_MMU_MAX_END          0x400

#define ICACHE_MMU_SIZE                 (0x400 * 4)
#define DCACHE_MMU_SIZE                 (0x400 * 4)

#define MMU_BUS_START(i)                0
#define MMU_BUS_SIZE(i)                 (0x400 * 4)


#define MMU_FLASH_VALID                 BIT(12)
#define MMU_FLASH_INVALID               0
#define MMU_PSRAM_VALID                 BIT(11)
#define MMU_PSRAM_INVALID               0

#define MMU_ACCESS_FLASH                0
#define MMU_ACCESS_PSRAM                BIT(10)

#define MMU_FLASH_SENSITIVE             BIT(13)
#define MMU_PSRAM_SENSITIVE             BIT(12)


#define CACHE_MAX_SYNC_NUM 0x400000
#define CACHE_MAX_LOCK_NUM 0x8000

/**
 * MMU entry valid bit mask for mapping value.
 * - For a Flash MMU entry:
 *   physical page number is BIT(0)~BIT(10), so value bits are 0x7ff
 * - For a PSRAM MMU entry:
 *   physical page number is BIT(0)~BIT(9), so value bits are 0x3ff
 */
#define MMU_FLASH_VALID_VAL_MASK 0x7ff
#define MMU_PSRAM_VALID_VAL_MASK 0x3ff
/**
 * Max MMU available paddr page num.
 * `MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 32768 * 64KB, means MMU can support 2GB paddr at most
 */
#define MMU_FLASH_MAX_PADDR_PAGE_NUM    32768
#define MMU_PSRAM_MAX_PADDR_PAGE_NUM    16384
//MMU entry num
#define MMU_ENTRY_NUM   1024

/**
 * This is the mask used for mapping. e.g.:
 * 0x4000_0000 & MMU_VADDR_MASK
 */
#define MMU_VADDR_MASK                  ((SOC_MMU_PAGE_SIZE) * MMU_ENTRY_NUM - 1)

#define SOC_MMU_FLASH_VADDR_BASE                   0x40000000
#define SOC_MMU_PSRAM_VADDR_BASE                   0x48000000

/*------------------------------------------------------------------------------
 * MMU Linear Address
 *----------------------------------------------------------------------------*/
/**
 * - 64KB MMU page size: the last 0xFFFF, which is the offset
 * - 1024 MMU entries for flash, 1024 MMU entries for psram, needs 0xFFF to hold it.
 *
 * Therefore, 0x3F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0xFFFFFFF

/**
 * - If high linear address isn't 0, this means MMU can recognize these addresses
 * - If high linear address is 0, this means MMU linear address range is equal or smaller than vaddr range.
 *   Under this condition, we use the max linear space.
 */
#define SOC_MMU_FLASH_LINEAR_ADDRESS_LOW      (DRAM_FLASH_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((DRAM_FLASH_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_FLASH_LINEAR_ADDRESS_HIGH     (DRAM_FLASH_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_FLASH_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif

#define SOC_MMU_PSRAM_LINEAR_ADDRESS_LOW      (DRAM_PSRAM_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((DRAM_PSRAM_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_PSRAM_LINEAR_ADDRESS_HIGH     (DRAM_PSRAM_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_PSRAM_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif


#ifdef __cplusplus
}
#endif
