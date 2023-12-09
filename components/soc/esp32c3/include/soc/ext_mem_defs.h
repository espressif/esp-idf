/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*IRAM0 is connected with Cache IBUS0*/
#define SOC_IRAM0_ADDRESS_LOW               0x4037C000
#define SOC_IRAM0_ADDRESS_HIGH              0x403E0000
#define SOC_IRAM0_CACHE_ADDRESS_LOW	        0x42000000
#define SOC_IRAM0_CACHE_ADDRESS_HIGH        0x42800000

/*DRAM0 is connected with Cache DBUS0*/
#define SOC_DRAM0_ADDRESS_LOW               0x3FC80000
#define SOC_DRAM0_ADDRESS_HIGH              0x3FCE0000
#define SOC_DRAM0_CACHE_ADDRESS_LOW         0x3C000000
#define SOC_DRAM0_CACHE_ADDRESS_HIGH        0x3C800000

#define SOC_IRAM_FLASH_ADDRESS_LOW          SOC_IRAM0_CACHE_ADDRESS_LOW
#define SOC_IRAM_FLASH_ADDRESS_HIGH         SOC_IRAM0_CACHE_ADDRESS_HIGH

#define SOC_DRAM_FLASH_ADDRESS_LOW          SOC_DRAM0_CACHE_ADDRESS_LOW
#define SOC_DRAM_FLASH_ADDRESS_HIGH         SOC_DRAM0_CACHE_ADDRESS_HIGH

#define SOC_BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define SOC_ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define SOC_ADDRESS_IN_IRAM0(vaddr)                SOC_ADDRESS_IN_BUS(SOC_IRAM0, vaddr)
#define SOC_ADDRESS_IN_IRAM0_CACHE(vaddr)          SOC_ADDRESS_IN_BUS(SOC_IRAM0_CACHE, vaddr)
#define SOC_ADDRESS_IN_DRAM0(vaddr)                SOC_ADDRESS_IN_BUS(SOC_DRAM0, vaddr)
#define SOC_ADDRESS_IN_DRAM0_CACHE(vaddr)          SOC_ADDRESS_IN_BUS(SOC_DRAM0_CACHE, vaddr)

#define SOC_MMU_INVALID                     BIT(8)
#define SOC_MMU_VALID                       0
#define SOC_MMU_TYPE                        0
#define SOC_MMU_ACCESS_FLASH                0

/**
 * MMU entry valid bit mask for mapping value. For an entry:
 * valid bit + value bits
 * valid bit is BIT(8), so value bits are 0xff
 */
#define SOC_MMU_VALID_VAL_MASK 0xff
/**
 * Max MMU available paddr page num.
 * `SOC_MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 256 * 64KB, means MMU can support 16MB paddr at most
 */
#define SOC_MMU_MAX_PADDR_PAGE_NUM    256
/**
 * This is the mask used for mapping. e.g.:
 * 0x4200_0000 & SOC_MMU_VADDR_MASK
 */
#define SOC_MMU_VADDR_MASK  0x7FFFFF
//MMU entry num
#define SOC_MMU_ENTRY_NUM   128

#define SOC_MMU_DBUS_VADDR_BASE               0x3C000000
#define SOC_MMU_IBUS_VADDR_BASE               0x42000000

/*------------------------------------------------------------------------------
 * MMU Linear Address
 *----------------------------------------------------------------------------*/
/**
 * - 64KB MMU page size: the last 0xFFFF, which is the offset
 * - 128 MMU entries, needs 0x7F to hold it.
 *
 * Therefore, 0x7F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x7FFFFF

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


/**
 * ROM flash mmap driver needs below definitions
 */
#define BUS_IRAM0_CACHE_SIZE              SOC_BUS_SIZE(SOC_IRAM0_CACHE)
#define BUS_DRAM0_CACHE_SIZE              SOC_BUS_SIZE(SOC_DRAM0_CACHE)

#define CACHE_IBUS                      0
#define CACHE_IBUS_MMU_START            0
#define CACHE_IBUS_MMU_END              0x200

#define CACHE_DBUS                      1
#define CACHE_DBUS_MMU_START            0
#define CACHE_DBUS_MMU_END              0x200

#define CACHE_IROM_MMU_START            0
#define CACHE_IROM_MMU_END              Cache_Get_IROM_MMU_End()
#define CACHE_IROM_MMU_SIZE             (CACHE_IROM_MMU_END - CACHE_IROM_MMU_START)

#define CACHE_DROM_MMU_START            CACHE_IROM_MMU_END
#define CACHE_DROM_MMU_END              Cache_Get_DROM_MMU_End()
#define CACHE_DROM_MMU_SIZE             (CACHE_DROM_MMU_END - CACHE_DROM_MMU_START)

#define CACHE_DROM_MMU_MAX_END          0x200

#define ICACHE_MMU_SIZE                 0x200
#define DCACHE_MMU_SIZE                 0x200

#define MMU_BUS_START(i)                0
#define MMU_BUS_SIZE(i)                 0x200

#ifdef __cplusplus
}
#endif
