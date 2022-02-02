/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _CACHE_MEMORY_H_
#define _CACHE_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "esp32c2/rom/cache.h"

/*IRAM0 is connected with Cache IBUS0*/
#define MMU_PAGE_MODE			MMU_Get_Page_Mode()

#define IRAM0_ADDRESS_LOW               0x40000000
#define IRAM0_ADDRESS_HIGH              IRAM0_CACHE_ADDRESS_HIGH
#define IRAM0_CACHE_ADDRESS_LOW         0x42000000
#define IRAM0_CACHE_ADDRESS_HIGH        (IRAM0_CACHE_ADDRESS_LOW + (0x100000 << (MMU_PAGE_MODE)))

/*DRAM0 is connected with Cache DBUS0*/
#define DRAM0_ADDRESS_LOW               0x3C000000
#define DRAM0_ADDRESS_HIGH              0x40000000
#define DRAM0_CACHE_ADDRESS_LOW         0x3C000000
#define DRAM0_CACHE_ADDRESS_HIGH        (DRAM0_CACHE_ADDRESS_LOW + (0x100000 << (MMU_PAGE_MODE)))
#define DRAM0_CACHE_OPERATION_HIGH      DRAM0_CACHE_ADDRESS_HIGH
#define ESP_CACHE_TEMP_ADDR             0x3C000000

#define BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define ADDRESS_IN_IRAM0(vaddr)            ADDRESS_IN_BUS(IRAM0, vaddr)
#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM0(vaddr)            ADDRESS_IN_BUS(DRAM0, vaddr)
#define ADDRESS_IN_DRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM0_CACHE, vaddr)

#define BUS_IRAM0_CACHE_SIZE              BUS_SIZE(IRAM0_CACHE)
#define BUS_DRAM0_CACHE_SIZE              BUS_SIZE(DRAM0_CACHE)

//IDF-3821
// #define MMU_SIZE                        0x100

#define CACHE_IBUS                      0
#define CACHE_IBUS_MMU_START            0
#define CACHE_IBUS_MMU_END              0x100

#define CACHE_DBUS                      1
#define CACHE_DBUS_MMU_START            0
#define CACHE_DBUS_MMU_END              0x100

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
#define MMU_TYPE                        0
#define MMU_ACCESS_FLASH                0

#define CACHE_MAX_SYNC_NUM 0x400000
#define CACHE_MAX_LOCK_NUM 0x8000

#define FLASH_MMU_TABLE ((volatile uint32_t*) DR_REG_MMU_TABLE)
#define FLASH_MMU_TABLE_SIZE (ICACHE_MMU_SIZE/sizeof(uint32_t))

#define MMU_TABLE_INVALID_VAL 		MMU_INVALID
#define FLASH_MMU_TABLE_INVALID_VAL DPORT_MMU_TABLE_INVALID_VAL
#define MMU_ADDRESS_MASK 		(MMU_TABLE_INVALID_VAL - 1)
#define MMU_PAGE_SIZE                   (0x4000 << (MMU_PAGE_MODE))
#define INVALID_PHY_PAGE                (MMU_PAGE_SIZE - 1)

#define BUS_ADDR_SIZE 			(0x100000 << (MMU_PAGE_MODE))
#define BUS_ADDR_MASK (BUS_ADDR_SIZE - 1)
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
