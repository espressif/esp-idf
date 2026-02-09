/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: to be checked IDF-14669

#if !SOC_MMU_PAGE_SIZE
/**
 * We define `SOC_MMU_PAGE_SIZE` in soc/CMakeLists.txt.
 * Here we give a default definition, if SOC_MMU_PAGE_SIZE doesn't exist. This is to pass the check_public_headers.py
 */
#define SOC_MMU_PAGE_SIZE 0x10000
#endif

#define SOC_IRAM0_ADDRESS_LOW                        0x40000000
#define SOC_IRAM0_ADDRESS_HIGH                       0x60000000

#define SOC_DRAM0_ADDRESS_LOW                        SOC_IRAM0_ADDRESS_LOW
#define SOC_DRAM0_ADDRESS_HIGH                       SOC_IRAM0_ADDRESS_HIGH

#define SOC_IRAM0_CACHE_ADDRESS_LOW                  0x40000000
#define SOC_IRAM0_CACHE_ADDRESS_HIGH                 0x60000000

#define SOC_DRAM0_CACHE_ADDRESS_LOW                  SOC_IRAM0_CACHE_ADDRESS_LOW                //I/D share the same vaddr range
#define SOC_DRAM0_CACHE_ADDRESS_HIGH                 SOC_IRAM0_CACHE_ADDRESS_HIGH               //I/D share the same vaddr range

#define SOC_IRAM_FLASH_ADDRESS_LOW                   0x40000000
#define SOC_IRAM_FLASH_ADDRESS_HIGH                  0x44000000

#define SOC_DRAM_FLASH_ADDRESS_LOW                   SOC_IRAM_FLASH_ADDRESS_LOW
#define SOC_DRAM_FLASH_ADDRESS_HIGH                  SOC_IRAM_FLASH_ADDRESS_HIGH

#define SOC_IRAM_PSRAM_ADDRESS_LOW                   0x50000000
#define SOC_IRAM_PSRAM_ADDRESS_HIGH                  0x54000000

#define SOC_DRAM_PSRAM_ADDRESS_LOW                   SOC_IRAM_PSRAM_ADDRESS_LOW
#define SOC_DRAM_PSRAM_ADDRESS_HIGH                  SOC_IRAM_PSRAM_ADDRESS_HIGH

#define SOC_IRAM_FLASH_PSRAM_OFFSET                  (SOC_IRAM_PSRAM_ADDRESS_LOW - SOC_IRAM_FLASH_ADDRESS_LOW)
#define SOC_DRAM_FLASH_PSRAM_OFFSET                  SOC_IRAM_FLASH_PSRAM_OFFSET

#define SOC_BUS_SIZE(bus_name)                       (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define SOC_ADDRESS_IN_BUS(bus_name, vaddr)          ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define SOC_ADDRESS_IN_IRAM0_CACHE(vaddr)            SOC_ADDRESS_IN_BUS(SOC_IRAM0_CACHE, vaddr)
#define SOC_ADDRESS_IN_DRAM0_CACHE(vaddr)            SOC_ADDRESS_IN_BUS(SOC_DRAM0_CACHE, vaddr)
#define SOC_ADDRESS_IN_DRAM_FLASH(vaddr)             SOC_ADDRESS_IN_BUS(SOC_DRAM_FLASH, vaddr)
#define SOC_ADDRESS_IN_DRAM_PSRAM(vaddr)             SOC_ADDRESS_IN_BUS(SOC_DRAM_PSRAM, vaddr)

#define SOC_MMU_FLASH_VALID                 BIT(12)
#define SOC_MMU_FLASH_INVALID               0
#define SOC_MMU_PSRAM_VALID                 BIT(11)
#define SOC_MMU_PSRAM_INVALID               0

#define SOC_MMU_ACCESS_FLASH                0
#define SOC_MMU_ACCESS_PSRAM                BIT(10)

#define SOC_MMU_FLASH_SENSITIVE             BIT(13)
#define SOC_MMU_PSRAM_SENSITIVE             BIT(12)

/**
 * MMU entry valid bit mask for mapping value.
 * - For a Flash MMU entry:
 *   physical page number is BIT(0)~BIT(10), so value bits are 0x7ff
 * - For a PSRAM MMU entry:
 *   physical page number is BIT(0)~BIT(9), so value bits are 0x3ff
 */
#define SOC_MMU_FLASH_VALID_VAL_MASK        0x3ff
#define SOC_MMU_PSRAM_VALID_VAL_MASK        0x3ff
/**
 * Max MMU available paddr page num.
 * `SOC_MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 32768 * 64KB, means MMU can support 2GB paddr at most
 */
#define SOC_MMU_FLASH_MAX_PADDR_PAGE_NUM    32768
#define SOC_MMU_PSRAM_MAX_PADDR_PAGE_NUM    16384
//MMU entry num
#define SOC_MMU_ENTRY_NUM                   1024

/**
 * This is the mask used for mapping. e.g.:
     * 0x4000_0000 & SOC_MMU_VADDR_MASK
 */
#define SOC_MMU_VADDR_MASK                  ((SOC_MMU_PAGE_SIZE) * SOC_MMU_ENTRY_NUM - 1)

#define SOC_MMU_FLASH_VADDR_BASE            0x40000000
#define SOC_MMU_PSRAM_VADDR_BASE            0x50000000

/*------------------------------------------------------------------------------
 * MMU Linear Address
 *----------------------------------------------------------------------------*/
/**
 * - 64KB MMU page size: the last 0xFFFF, which is the offset
 * - 1024 MMU entries for flash, 1024 MMU entries for psram, needs 0xFFF to hold it.
 *
 * Therefore, 0x3F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK            0x1FFFFFFF //distinguish between flash and psram addresses

/**
 * - If high linear address isn't 0, this means MMU can recognize these addresses
 * - If high linear address is 0, this means MMU linear address range is equal or smaller than vaddr range.
 *   Under this condition, we use the max linear space.
 */
#define SOC_MMU_FLASH_LINEAR_ADDRESS_LOW      (SOC_DRAM_FLASH_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((SOC_DRAM_FLASH_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_FLASH_LINEAR_ADDRESS_HIGH     (SOC_DRAM_FLASH_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_FLASH_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif

#define SOC_MMU_PSRAM_LINEAR_ADDRESS_LOW      (SOC_DRAM_PSRAM_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((SOC_DRAM_PSRAM_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_PSRAM_LINEAR_ADDRESS_HIGH     (SOC_DRAM_PSRAM_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_PSRAM_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif

#ifdef __cplusplus
}
#endif
