/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once


#include <stdint.h>
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


/*IRAM0 is connected with Cache IBUS0*/
#define IRAM0_ADDRESS_LOW                      0x4037C000
#define IRAM0_ADDRESS_HIGH                     0x403C0000
#define IRAM0_CACHE_ADDRESS_LOW                0x42000000
#define IRAM0_CACHE_ADDRESS_HIGH               (IRAM0_CACHE_ADDRESS_LOW + ((SOC_MMU_PAGE_SIZE) * MMU_ENTRY_NUM)) // MMU has 64 pages

/*DRAM0 is connected with Cache DBUS0*/
#define DRAM0_ADDRESS_LOW                      0x3FCA0000
#define DRAM0_ADDRESS_HIGH                     0x3FCE0000
#define DRAM0_CACHE_ADDRESS_LOW                0x3C000000
#define DRAM0_CACHE_ADDRESS_HIGH               (DRAM0_CACHE_ADDRESS_LOW + ((SOC_MMU_PAGE_SIZE) * MMU_ENTRY_NUM)) // MMU has 64 pages
#define DRAM0_CACHE_OPERATION_HIGH             DRAM0_CACHE_ADDRESS_HIGH

#define BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define ADDRESS_IN_IRAM0(vaddr)            ADDRESS_IN_BUS(IRAM0, vaddr)
#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM0(vaddr)            ADDRESS_IN_BUS(DRAM0, vaddr)
#define ADDRESS_IN_DRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM0_CACHE, vaddr)

#define BUS_IRAM0_CACHE_SIZE              BUS_SIZE(IRAM0_CACHE)
#define BUS_DRAM0_CACHE_SIZE              BUS_SIZE(DRAM0_CACHE)

#define CACHE_IBUS                      0
#define CACHE_IBUS_MMU_START            0
#define CACHE_IBUS_MMU_END              0x100

#define CACHE_DBUS                      1
#define CACHE_DBUS_MMU_START            0
#define CACHE_DBUS_MMU_END              0x100

//TODO, remove these cache function dependencies
#define CACHE_IROM_MMU_START            0
#define CACHE_IROM_MMU_END              Cache_Get_IROM_MMU_End()
#define CACHE_IROM_MMU_SIZE             (CACHE_IROM_MMU_END - CACHE_IROM_MMU_START)

#define CACHE_DROM_MMU_START            CACHE_IROM_MMU_END
#define CACHE_DROM_MMU_END              Cache_Get_DROM_MMU_End()
#define CACHE_DROM_MMU_SIZE             (CACHE_DROM_MMU_END - CACHE_DROM_MMU_START)

#define CACHE_DROM_MMU_MAX_END          0x100

#define ICACHE_MMU_SIZE                 0x100
#define DCACHE_MMU_SIZE                 0x100

#define MMU_BUS_START(i)                0
#define MMU_BUS_SIZE(i)                 0x100

#define MMU_INVALID                     BIT(6)
#define MMU_VALID                       0
#define MMU_TYPE                        0
#define MMU_ACCESS_FLASH                0

#define CACHE_MAX_SYNC_NUM 0x400000
#define CACHE_MAX_LOCK_NUM 0x8000

/**
 * MMU entry valid bit mask for mapping value. For an entry:
 * valid bit + value bits
 * valid bit is BIT(6), so value bits are 0x3f
 */
#define MMU_VALID_VAL_MASK 		        0x3f

/**
 * Max MMU available paddr page num.
 * `MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 64 * 64KB, means MMU can support 4MB paddr at most
 */
#define MMU_MAX_PADDR_PAGE_NUM    64
/**
 * This is the mask used for mapping. e.g.:
 * 0x4200_0000 & MMU_VADDR_MASK
 */
#define MMU_VADDR_MASK                 ((SOC_MMU_PAGE_SIZE) * 64 - 1)
//MMU entry num
#define MMU_ENTRY_NUM  64

#define BUS_PMS_MASK  0xffffff

#define CACHE_ICACHE_LOW_SHIFT         0
#define CACHE_ICACHE_HIGH_SHIFT        2
#define CACHE_DCACHE_LOW_SHIFT         4
#define CACHE_DCACHE_HIGH_SHIFT        6

#define CACHE_MEMORY_IBANK0_ADDR        0x4037C000

#define SOC_MMU_DBUS_VADDR_BASE               0x3C000000
#define SOC_MMU_IBUS_VADDR_BASE               0x42000000

/*------------------------------------------------------------------------------
 * MMU Linear Address
 *----------------------------------------------------------------------------*/
#if (SOC_MMU_PAGE_SIZE == 0x10000)
/**
 * - 64KB MMU page size: the last 0xFFFF, which is the offset
 * - 64 MMU entries, needs 0x3F to hold it.
 *
 * Therefore, 0x3F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x3FFFFF

#elif (SOC_MMU_PAGE_SIZE == 0x8000)
/**
 * - 32KB MMU page size: the last 0x7FFF, which is the offset
 * - 64 MMU entries, needs 0x3F to hold it.
 *
 * Therefore, 0x1F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x1FFFFF

#elif (SOC_MMU_PAGE_SIZE == 0x4000)
/**
 * - 16KB MMU page size: the last 0x3FFF, which is the offset
 * - 64 MMU entries, needs 0x3F to hold it.
 *
 * Therefore, 0xF,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0xFFFFF
#endif  //SOC_MMU_PAGE_SIZE

/**
 * - If high linear address isn't 0, this means MMU can recognize these addresses
 * - If high linear address is 0, this means MMU linear address range is equal or smaller than vaddr range.
 *   Under this condition, we use the max linear space.
 */
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW      (IRAM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((IRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH     (IRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif

#define SOC_MMU_DRAM0_LINEAR_ADDRESS_LOW      (DRAM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#if ((DRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK) > 0)
#define SOC_MMU_DRAM0_LINEAR_ADDRESS_HIGH     (DRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)
#else
#define SOC_MMU_DRAM0_LINEAR_ADDRESS_HIGH     (SOC_MMU_LINEAR_ADDR_MASK + 1)
#endif

/**
 * I/D share the MMU linear address range
 */
_Static_assert(SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW == SOC_MMU_DRAM0_LINEAR_ADDRESS_LOW, "IRAM0 and DRAM0 linear address should be same");


#ifdef __cplusplus
}
#endif
