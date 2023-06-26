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


#define IRAM0_CACHE_ADDRESS_LOW                  0x42000000
#define IRAM0_CACHE_ADDRESS_HIGH                 (IRAM0_CACHE_ADDRESS_LOW + ((SOC_MMU_PAGE_SIZE) * MMU_ENTRY_NUM))

#define DRAM0_CACHE_ADDRESS_LOW                  IRAM0_CACHE_ADDRESS_LOW                //I/D share the same vaddr range
#define DRAM0_CACHE_ADDRESS_HIGH                 IRAM0_CACHE_ADDRESS_HIGH               //I/D share the same vaddr range
#define DRAM0_CACHE_OPERATION_HIGH               DRAM0_CACHE_ADDRESS_HIGH

#define BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define ADDRESS_IN_IRAM0(vaddr)            ADDRESS_IN_BUS(IRAM0, vaddr)
#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM0(vaddr)            ADDRESS_IN_BUS(DRAM0, vaddr)
#define ADDRESS_IN_DRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM0_CACHE, vaddr)

#define BUS_IRAM0_CACHE_SIZE              BUS_SIZE(IRAM0_CACHE)
#define BUS_DRAM0_CACHE_SIZE              BUS_SIZE(DRAM0_CACHE)

//TODO, remove these cache function dependencies
#define CACHE_IROM_MMU_START            0
#define CACHE_IROM_MMU_END              Cache_Get_IROM_MMU_End()
#define CACHE_IROM_MMU_SIZE             (CACHE_IROM_MMU_END - CACHE_IROM_MMU_START)

#define CACHE_DROM_MMU_START            CACHE_IROM_MMU_END
#define CACHE_DROM_MMU_END              Cache_Get_DROM_MMU_End()
#define CACHE_DROM_MMU_SIZE             (CACHE_DROM_MMU_END - CACHE_DROM_MMU_START)

#define CACHE_DROM_MMU_MAX_END          0x400

#define ICACHE_MMU_SIZE                 0x200
#define DCACHE_MMU_SIZE                 0x200

#define MMU_BUS_START(i)                0
#define MMU_BUS_SIZE(i)                 0x200

#define MMU_MSPI_ACCESS_FLASH           0
#define MMU_MSPI_VALID                  BIT(9)
#define MMU_MSPI_INVALID                0
#define MMU_MSPI_SENSITIVE              BIT(10)

#define MMU_ACCESS_FLASH                MMU_MSPI_ACCESS_FLASH
#define MMU_VALID                       MMU_MSPI_VALID
#define MMU_SENSITIVE                   MMU_MSPI_SENSITIVE

#define MMU_INVALID_MASK                MMU_MSPI_VALID
#define MMU_INVALID                     MMU_MSPI_INVALID



#define CACHE_MAX_SYNC_NUM 0x400000
#define CACHE_MAX_LOCK_NUM 0x8000

/**
 * MMU entry valid bit mask for mapping value. For an entry:
 * valid bit + value bits
 * valid bit is BIT(9), so value bits are 0x1ff
 */
#define MMU_VALID_VAL_MASK 0x1ff
/**
 * Max MMU available paddr page num.
 * `MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 256 * 64KB, means MMU can support 16MB paddr at most
 */
#define MMU_MAX_PADDR_PAGE_NUM    256
//MMU entry num
#define MMU_ENTRY_NUM   256

/**
 * This is the mask used for mapping. e.g.:
 * 0x4200_0000 & MMU_VADDR_MASK
 */
#define MMU_VADDR_MASK                  ((SOC_MMU_PAGE_SIZE) * MMU_ENTRY_NUM - 1)

#define CACHE_MEMORY_IBANK0_ADDR        0x40800000


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
#define SOC_MMU_LINEAR_ADDR_MASK              0x7FFFFF

#elif (SOC_MMU_PAGE_SIZE == 0x8000)
/**
 * - 32KB MMU page size: the last 0x7FFF, which is the offset
 * - 128 MMU entries, needs 0x7F to hold it.
 *
 * Therefore, 0x3F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x3FFFFF

#elif (SOC_MMU_PAGE_SIZE == 0x4000)
/**
 * - 16KB MMU page size: the last 0x3FFF, which is the offset
 * - 128 MMU entries, needs 0x7F to hold it.
 *
 * Therefore, 0x1F,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x1FFFFF
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
