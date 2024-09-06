/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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


#define SOC_IRAM0_CACHE_ADDRESS_LOW                  0x42000000
#define SOC_IRAM0_CACHE_ADDRESS_HIGH                 (SOC_IRAM0_CACHE_ADDRESS_LOW + ((SOC_MMU_PAGE_SIZE) * SOC_MMU_ENTRY_NUM))

#define SOC_DRAM0_CACHE_ADDRESS_LOW                  SOC_IRAM0_CACHE_ADDRESS_LOW                //I/D share the same vaddr range
#define SOC_DRAM0_CACHE_ADDRESS_HIGH                 SOC_IRAM0_CACHE_ADDRESS_HIGH               //I/D share the same vaddr range

#define SOC_IRAM_FLASH_ADDRESS_LOW                   SOC_IRAM0_CACHE_ADDRESS_LOW
#define SOC_IRAM_FLASH_ADDRESS_HIGH                  SOC_IRAM0_CACHE_ADDRESS_HIGH

#define SOC_DRAM_FLASH_ADDRESS_LOW                   SOC_DRAM0_CACHE_ADDRESS_LOW
#define SOC_DRAM_FLASH_ADDRESS_HIGH                  SOC_DRAM0_CACHE_ADDRESS_HIGH

#define SOC_IRAM_PSRAM_ADDRESS_LOW                   SOC_IRAM0_CACHE_ADDRESS_LOW
#define SOC_IRAM_PSRAM_ADDRESS_HIGH                  SOC_IRAM0_CACHE_ADDRESS_HIGH

#define SOC_DRAM_PSRAM_ADDRESS_LOW                   SOC_DRAM0_CACHE_ADDRESS_LOW
#define SOC_DRAM_PSRAM_ADDRESS_HIGH                  SOC_DRAM0_CACHE_ADDRESS_HIGH

#define SOC_BUS_SIZE(bus_name)                       (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define SOC_ADDRESS_IN_BUS(bus_name, vaddr)          ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define SOC_ADDRESS_IN_IRAM0(vaddr)                  SOC_ADDRESS_IN_BUS(SOC_IRAM0, vaddr)
#define SOC_ADDRESS_IN_IRAM0_CACHE(vaddr)            SOC_ADDRESS_IN_BUS(SOC_IRAM0_CACHE, vaddr)
#define SOC_ADDRESS_IN_DRAM0(vaddr)                  SOC_ADDRESS_IN_BUS(SOC_DRAM0, vaddr)
#define SOC_ADDRESS_IN_DRAM0_CACHE(vaddr)            SOC_ADDRESS_IN_BUS(SOC_DRAM0_CACHE, vaddr)

#define SOC_MMU_ACCESS_FLASH            0
#define SOC_MMU_ACCESS_SPIRAM           BIT(9)
#define SOC_MMU_VALID                   BIT(10)
#define SOC_MMU_SENSITIVE               BIT(11)
#define SOC_MMU_INVALID_MASK            BIT(10)
#define SOC_MMU_INVALID                 0

/**
 * MMU entry valid bit mask for mapping value. For an entry:
 * valid bit + value bits
 * valid bit is BIT(9), so value bits are 0x1ff
 */
#define SOC_MMU_VALID_VAL_MASK (SOC_MMU_ACCESS_SPIRAM - 1)
/**
 * Max MMU available paddr page num.
 * `SOC_MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 256 * 64KB, means MMU can support 16MB paddr at most
 */
#define SOC_MMU_MAX_PADDR_PAGE_NUM    512
//MMU entry num
#define SOC_MMU_ENTRY_NUM   512

/**
 * This is the mask used for mapping. e.g.:
 * 0x4200_0000 & SOC_MMU_VADDR_MASK
 */
#define SOC_MMU_VADDR_MASK                  ((SOC_MMU_PAGE_SIZE) * SOC_MMU_ENTRY_NUM - 1)

#define SOC_MMU_DBUS_VADDR_BASE               0x42000000
#define SOC_MMU_IBUS_VADDR_BASE               0x42000000

/*------------------------------------------------------------------------------
 * MMU Linear Address
 *----------------------------------------------------------------------------*/
#if (SOC_MMU_PAGE_SIZE == 0x10000)
/**
 * - 64KB MMU page size: the last 0xFFFF, which is the offset
 * - 128 MMU entries, needs 0x7F to hold it.
 *
 * Therefore, 0x7F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x1FFFFFF

#elif (SOC_MMU_PAGE_SIZE == 0x8000)
/**
 * - 32KB MMU page size: the last 0x7FFF, which is the offset
 * - 128 MMU entries, needs 0x7F to hold it.
 *
 * Therefore, 0x3F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0xFFFFFF

#elif (SOC_MMU_PAGE_SIZE == 0x4000)
/**
 * - 16KB MMU page size: the last 0x3FFF, which is the offset
 * - 128 MMU entries, needs 0x7F to hold it.
 *
 * Therefore, 0x1F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x7FFFFF
#endif  //SOC_MMU_PAGE_SIZE

/**
 * - If high linear address isn't 0, this means MMU can recognize these addresses
 * - If high linear address is 0, this means MMU linear address range is equal or smaller than vaddr range.
 *   Under this condition, we use the max linear space.
 */
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW      (SOC_IRAM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((SOC_IRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH     (SOC_IRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif

#define SOC_MMU_DRAM0_LINEAR_ADDRESS_LOW      (SOC_DRAM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((SOC_DRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_DRAM0_LINEAR_ADDRESS_HIGH     (SOC_DRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_DRAM0_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif

/**
 * I/D share the MMU linear address range
 */
#ifndef __cplusplus
_Static_assert(SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW == SOC_MMU_DRAM0_LINEAR_ADDRESS_LOW, "IRAM0 and DRAM0 linear address should be same");
#endif

#ifdef __cplusplus
}
#endif
