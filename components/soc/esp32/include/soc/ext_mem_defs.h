/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _CACHE_MEMORY_H_
#define _CACHE_MEMORY_H_

#include "esp_bit_defs.h"


#ifdef __cplusplus
extern "C" {
#endif

#define SOC_IRAM0_CACHE_ADDRESS_LOW     0x400D0000
#define SOC_IRAM0_CACHE_ADDRESS_HIGH    0x40400000

#define SOC_IRAM1_CACHE_ADDRESS_LOW     0x40400000
#define SOC_IRAM1_CACHE_ADDRESS_HIGH    0x40800000

#define SOC_IROM0_CACHE_ADDRESS_LOW     0x40800000
#define SOC_IROM0_CACHE_ADDRESS_HIGH    0x40C00000

#define SOC_DRAM1_CACHE_ADDRESS_LOW     0x3F800000
#define SOC_DRAM1_CACHE_ADDRESS_HIGH    0x3FC00000

#define SOC_DROM0_CACHE_ADDRESS_LOW     0x3F400000
#define SOC_DROM0_CACHE_ADDRESS_HIGH    0x3F800000

#define SOC_IRAM_FLASH_ADDRESS_LOW      SOC_IRAM0_CACHE_ADDRESS_LOW
#define SOC_IRAM_FLASH_ADDRESS_HIGH     SOC_IRAM0_CACHE_ADDRESS_HIGH

#define SOC_DRAM_FLASH_ADDRESS_LOW      SOC_DROM0_CACHE_ADDRESS_LOW
#define SOC_DRAM_FLASH_ADDRESS_HIGH     SOC_DROM0_CACHE_ADDRESS_HIGH

#define SOC_BUS_SIZE(bus_name)                     (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define SOC_ADDRESS_IN_BUS(bus_name, vaddr)        ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)
#define SOC_ADDRESS_IN_IRAM0_CACHE(vaddr)          SOC_ADDRESS_IN_BUS(SOC_IRAM0_CACHE, vaddr)
#define SOC_ADDRESS_IN_IRAM1_CACHE(vaddr)          SOC_ADDRESS_IN_BUS(SOC_IRAM1_CACHE, vaddr)
#define SOC_ADDRESS_IN_IROM0_CACHE(vaddr)          SOC_ADDRESS_IN_BUS(SOC_IROM0_CACHE, vaddr)
#define SOC_ADDRESS_IN_DRAM1_CACHE(vaddr)          SOC_ADDRESS_IN_BUS(SOC_DRAM1_CACHE, vaddr)
#define SOC_ADDRESS_IN_DROM0_CACHE(vaddr)          SOC_ADDRESS_IN_BUS(SOC_DROM0_CACHE, vaddr)

#define SOC_MMU_INVALID                       BIT(8)

/**
 * Max MMU available paddr page num.
 * `SOC_MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 256 * 64KB, means MMU can support 16MB paddr at most
 */
#define SOC_MMU_MAX_PADDR_PAGE_NUM            256
/**
 * This is the mask used for mapping. e.g.:
 * 0x4008_0000 & SOC_MMU_VADDR_MASK
 */
#define SOC_MMU_VADDR_MASK                    0x3FFFFF
//MMU entry num, 384 entries that are used in IDF for Flash
#define SOC_MMU_ENTRY_NUM                     384


#define SOC_MMU_DBUS_VADDR_BASE               0x3E000000
#define SOC_MMU_IBUS_VADDR_BASE               0x40000000

/*------------------------------------------------------------------------------
 * MMU Linear Address
 *----------------------------------------------------------------------------*/
/**
 * - 64KB MMU page size: the last 0xFFFF, which is the offset
 * - 384 MMU entries, needs 0x1FF to hold it.
 *
 * Therefore, 0x1FF,FFFF
 */
#define SOC_MMU_LINEAR_ADDR_MASK              0x1FFFFFF

#define SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW      (SOC_IRAM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH     (SOC_IRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_IRAM1_LINEAR_ADDRESS_LOW      (SOC_IRAM1_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_IRAM1_LINEAR_ADDRESS_HIGH     (SOC_IRAM1_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_IROM0_LINEAR_ADDRESS_LOW      (SOC_IROM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_IROM0_LINEAR_ADDRESS_HIGH     (SOC_IROM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DROM0_LINEAR_ADDRESS_LOW      (SOC_DROM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DROM0_LINEAR_ADDRESS_HIGH     (SOC_DROM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DRAM1_LINEAR_ADDRESS_LOW      (SOC_DRAM1_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DRAM1_LINEAR_ADDRESS_HIGH     (SOC_DRAM1_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)




#ifdef __cplusplus
}
#endif

#endif /*_CACHE_MEMORY_H_ */
