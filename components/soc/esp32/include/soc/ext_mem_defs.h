/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _CACHE_MEMORY_H_
#define _CACHE_MEMORY_H_

#include "esp_bit_defs.h"


#ifdef __cplusplus
extern "C" {
#endif

#define IRAM0_CACHE_ADDRESS_LOW     0x400D0000
#define IRAM0_CACHE_ADDRESS_HIGH    0x40400000

#define IRAM1_CACHE_ADDRESS_LOW     0x40400000
#define IRAM1_CACHE_ADDRESS_HIGH    0x40800000

#define IROM0_CACHE_ADDRESS_LOW     0x40800000
#define IROM0_CACHE_ADDRESS_HIGH    0x40C00000

#define DRAM1_CACHE_ADDRESS_LOW     0x3F800000
#define DRAM1_CACHE_ADDRESS_HIGH    0x3FC00000

#define DROM0_CACHE_ADDRESS_LOW     0x3F400000
#define DROM0_CACHE_ADDRESS_HIGH    0x3F800000


#define BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)
#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_IRAM1_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM1_CACHE, vaddr)
#define ADDRESS_IN_IROM0_CACHE(vaddr)      ADDRESS_IN_BUS(IROM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM1_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM1_CACHE, vaddr)
#define ADDRESS_IN_DROM0_CACHE(vaddr)      ADDRESS_IN_BUS(DROM0_CACHE, vaddr)

#define MMU_INVALID                           BIT(8)

/**
 * Max MMU available paddr page num.
 * `MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 256 * 64KB, means MMU can support 16MB paddr at most
 */
#define MMU_MAX_PADDR_PAGE_NUM    256
/**
 * This is the mask used for mapping. e.g.:
 * 0x4008_0000 & MMU_VADDR_MASK
 */
#define MMU_VADDR_MASK  0x3FFFFF
//MMU entry num, 384 entries that are used in IDF for Flash
#define MMU_ENTRY_NUM                         384


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

#define SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW      (IRAM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH     (IRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_IRAM1_LINEAR_ADDRESS_LOW      (IRAM1_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_IRAM1_LINEAR_ADDRESS_HIGH     (IRAM1_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_IROM0_LINEAR_ADDRESS_LOW      (IROM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_IROM0_LINEAR_ADDRESS_HIGH     (IROM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DROM0_LINEAR_ADDRESS_LOW      (DROM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DROM0_LINEAR_ADDRESS_HIGH     (DROM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DRAM1_LINEAR_ADDRESS_LOW      (DRAM1_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DRAM1_LINEAR_ADDRESS_HIGH     (DRAM1_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)




#ifdef __cplusplus
}
#endif

#endif /*_CACHE_MEMORY_H_ */
