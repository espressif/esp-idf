/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _CACHE_MEMORY_H_
#define _CACHE_MEMORY_H_

#include "esp_bit_defs.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/*IRAM0 is connected with Cache IBUS0*/
#define IRAM0_CACHE_ADDRESS_LOW             0x42000000
#define IRAM0_CACHE_ADDRESS_HIGH(page_size) (IRAM0_CACHE_ADDRESS_LOW + ((page_size) * 128)) // MMU has 256 pages, first 128 for instruction
#define IRAM0_ADDRESS_LOW                   0x40000000
#define IRAM0_ADDRESS_HIGH(page_size)       IRAM0_CACHE_ADDRESS_HIGH(page_size)

/*DRAM0 is connected with Cache DBUS0*/
#define DRAM0_ADDRESS_LOW               0x42000000
#define DRAM0_ADDRESS_HIGH              0x43000000
#define DRAM0_CACHE_ADDRESS_LOW         IRAM0_CACHE_ADDRESS_HIGH(CONFIG_MMU_PAGE_SIZE) // ESP32C6-TODO after fixed, also need to remove the sdkconfig.h inclusion
#define DRAM0_CACHE_ADDRESS_HIGH(page_size)        (IRAM0_CACHE_ADDRESS_HIGH(page_size) + ((page_size) * 128)) // MMU has 256 pages, second 128 for data
#define DRAM0_CACHE_OPERATION_HIGH(page_size)      DRAM0_CACHE_ADDRESS_HIGH(page_size)
#define ESP_CACHE_TEMP_ADDR             0x42000000

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
#define CACHE_IBUS_MMU_END              0x200

#define CACHE_DBUS                      1
#define CACHE_DBUS_MMU_START            0
#define CACHE_DBUS_MMU_END              0x200

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
#define MMU_ACCESS_SPIRAM               MMU_MSPI_ACCESS_SPIRAM
#define MMU_VALID                       MMU_MSPI_VALID
#define MMU_SENSITIVE                   MMU_MSPI_SENSITIVE

// ESP32C6-TODO
#define MMU_INVALID_MASK                MMU_MSPI_VALID
#define MMU_INVALID                     MMU_MSPI_INVALID



#define CACHE_MAX_SYNC_NUM 0x400000
#define CACHE_MAX_LOCK_NUM 0x8000

#define FLASH_MMU_TABLE ((volatile uint32_t*) DR_REG_MMU_TABLE)
#define FLASH_MMU_TABLE_SIZE (ICACHE_MMU_SIZE/sizeof(uint32_t))

/**
 * MMU entry valid bit mask for mapping value. For an entry:
 * valid bit + value bits
 * valid bit is BIT(9), so value bits are 0x1ff
 */
#define MMU_VALID_VAL_MASK 0x1ff
/**
 * Max MMU available paddr page num.
 * `MMU_MAX_PADDR_PAGE_NUM * CONFIG_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 256 * 64KB, means MMU can support 16MB paddr at most
 */
#define MMU_MAX_PADDR_PAGE_NUM    256
//MMU entry num
#define MMU_ENTRY_NUM   256

/**
 * This is the mask used for mapping. e.g.:
 * 0x4200_0000 & MMU_VADDR_MASK
 */
#define MMU_VADDR_MASK(page_size)                 ((page_size) * MMU_ENTRY_NUM - 1)

#define CACHE_MEMORY_IBANK0_ADDR        0x40800000

#ifdef __cplusplus
}
#endif

#endif /*_CACHE_MEMORY_H_ */
