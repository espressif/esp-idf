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

#include <stdint.h>

/*IRAM0 is connected with Cache IBUS0*/
#define IRAM0_ADDRESS_LOW               0x40000000
#define IRAM0_ADDRESS_HIGH(page_size)              IRAM0_CACHE_ADDRESS_HIGH(page_size)
#define IRAM0_CACHE_ADDRESS_LOW         0x42000000
#define IRAM0_CACHE_ADDRESS_HIGH(page_size)        (IRAM0_CACHE_ADDRESS_LOW + ((page_size) * 64)) // MMU has 64 pages

/*DRAM0 is connected with Cache DBUS0*/
#define DRAM0_ADDRESS_LOW               0x3C000000
#define DRAM0_ADDRESS_HIGH              0x40000000
#define DRAM0_CACHE_ADDRESS_LOW         0x3C000000
#define DRAM0_CACHE_ADDRESS_HIGH(page_size)        (DRAM0_CACHE_ADDRESS_LOW + ((page_size) * 64))
#define DRAM0_CACHE_OPERATION_HIGH(page_size)      DRAM0_CACHE_ADDRESS_HIGH(page_size)
#define ESP_CACHE_TEMP_ADDR             0x3C000000

#define BUS_SIZE(bus_name, page_size)                 (bus_name##_ADDRESS_HIGH(page_size) - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr, page_size)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH(page_size))

#define ADDRESS_IN_IRAM0(vaddr, page_size)            ADDRESS_IN_BUS(IRAM0, vaddr, page_size)
#define ADDRESS_IN_IRAM0_CACHE(vaddr, page_size)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr, page_size)
#define ADDRESS_IN_DRAM0(vaddr, page_size)            ADDRESS_IN_BUS(DRAM0, vaddr, page_size)
#define ADDRESS_IN_DRAM0_CACHE(vaddr, page_size)      ADDRESS_IN_BUS(DRAM0_CACHE, vaddr, page_size)

#define BUS_IRAM0_CACHE_SIZE(page_size)              BUS_SIZE(IRAM0_CACHE, page_size)
#define BUS_DRAM0_CACHE_SIZE(page_size)              BUS_SIZE(DRAM0_CACHE, page_size)

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

#define FLASH_MMU_TABLE ((volatile uint32_t*) DR_REG_MMU_TABLE)
#define FLASH_MMU_TABLE_SIZE (ICACHE_MMU_SIZE/sizeof(uint32_t))

/**
 * MMU entry valid bit mask for mapping value. For an entry:
 * valid bit + value bits
 * valid bit is BIT(6), so value bits are 0x3f
 */
#define MMU_VALID_VAL_MASK 		        0x3f

/**
 * Max MMU available paddr page num.
 * `MMU_MAX_PADDR_PAGE_NUM * CONFIG_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 64 * 64KB, means MMU can support 4MB paddr at most
 */
#define MMU_MAX_PADDR_PAGE_NUM    64
/**
 * This is the mask used for mapping. e.g.:
 * 0x4200_0000 & MMU_VADDR_MASK
 */
#define MMU_VADDR_MASK(page_size)                 ((page_size) * 64 - 1)
//MMU entry num
#define MMU_ENTRY_NUM  64

#define BUS_PMS_MASK  0xffffff

#define CACHE_ICACHE_LOW_SHIFT         0
#define CACHE_ICACHE_HIGH_SHIFT        2
#define CACHE_DCACHE_LOW_SHIFT         4
#define CACHE_DCACHE_HIGH_SHIFT        6

#define CACHE_MEMORY_IBANK0_ADDR        0x4037C000


#ifdef __cplusplus
}
#endif

#endif /*_CACHE_MEMORY_H_ */
