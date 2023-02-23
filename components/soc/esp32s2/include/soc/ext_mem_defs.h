/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _CACHE_MEMORY_H_
#define _CACHE_MEMORY_H_

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*IRAM0 is connected with Cache IBUS0*/
#define IRAM0_ADDRESS_LOW		0x40000000
#define IRAM0_ADDRESS_HIGH		0x40400000
#define IRAM0_CACHE_ADDRESS_LOW		0x40080000
#define IRAM0_CACHE_ADDRESS_HIGH	0x40400000

/*IRAM1 is connected with Cache IBUS1*/
#define IRAM1_ADDRESS_LOW		0x40400000
#define IRAM1_ADDRESS_HIGH		0x40800000

/*DROM0 is connected with Cache IBUS2*/
#define DROM0_ADDRESS_LOW		0x3f000000
#define DROM0_ADDRESS_HIGH		0x3f400000

/*DRAM0 is connected with Cache DBUS0*/
#define DRAM0_ADDRESS_LOW       	0x3fc00000
#define DRAM0_ADDRESS_HIGH      	0x40000000
#define DRAM0_CACHE_ADDRESS_LOW       	0x3fc00000
#define DRAM0_CACHE_ADDRESS_HIGH      	0x3ff80000

/*DRAM1 is connected with Cache DBUS1*/
#define DRAM1_ADDRESS_LOW       	0x3f800000
#define DRAM1_ADDRESS_HIGH      	0x3fc00000

/*DPORT is connected with Cache DBUS2*/
#define DPORT_ADDRESS_LOW       	0x3f400000
#define DPORT_ADDRESS_HIGH      	0x3f800000
#define DPORT_CACHE_ADDRESS_LOW       	0x3f500000
#define DPORT_CACHE_ADDRESS_HIGH      	0x3f800000

#define BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define ADDRESS_IN_IRAM0(vaddr)            ADDRESS_IN_BUS(IRAM0, vaddr)
#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_IRAM1(vaddr)            ADDRESS_IN_BUS(IRAM1, vaddr)
#define ADDRESS_IN_DROM0(vaddr)            ADDRESS_IN_BUS(DROM0, vaddr)
#define ADDRESS_IN_DRAM0(vaddr)            ADDRESS_IN_BUS(DRAM0, vaddr)
#define ADDRESS_IN_DRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM0_CACHE, vaddr)
#define ADDRESS_IN_DRAM1(vaddr)            ADDRESS_IN_BUS(DRAM1, vaddr)
#define ADDRESS_IN_DPORT(vaddr)            ADDRESS_IN_BUS(DPORT, vaddr)
#define ADDRESS_IN_DPORT_CACHE(vaddr)      ADDRESS_IN_BUS(DPORT_CACHE, vaddr)

#define BUS_IRAM0_CACHE_SIZE              BUS_SIZE(IRAM0_CACHE)
#define BUS_IRAM1_CACHE_SIZE              BUS_SIZE(IRAM1)
#define BUS_IROM0_CACHE_SIZE              BUS_SIZE(IROM0)
#define BUS_DROM0_CACHE_SIZE              BUS_SIZE(DROM0)
#define BUS_DRAM0_CACHE_SIZE              BUS_SIZE(DRAM0_CACHE)
#define BUS_DRAM1_CACHE_SIZE              BUS_SIZE(DRAM1)
#define BUS_DPORT_CACHE_SIZE              BUS_SIZE(DPORT_CACHE)

#define PRO_CACHE_IBUS0                 0
#define PRO_CACHE_IBUS0_MMU_START       0
#define PRO_CACHE_IBUS0_MMU_END         0x100

#define PRO_CACHE_IBUS1                 1
#define PRO_CACHE_IBUS1_MMU_START       0x100
#define PRO_CACHE_IBUS1_MMU_END         0x200

#define PRO_CACHE_IBUS2                 2
#define PRO_CACHE_IBUS2_MMU_START       0x200
#define PRO_CACHE_IBUS2_MMU_END         0x300

#define PRO_CACHE_DBUS0                 3
#define PRO_CACHE_DBUS0_MMU_START       0x300
#define PRO_CACHE_DBUS0_MMU_END         0x400

#define PRO_CACHE_DBUS1                 4
#define PRO_CACHE_DBUS1_MMU_START       0x400
#define PRO_CACHE_DBUS1_MMU_END         0x500

#define PRO_CACHE_DBUS2                 5
#define PRO_CACHE_DBUS2_MMU_START       0x500
#define PRO_CACHE_DBUS2_MMU_END         0x600

#define ICACHE_MMU_SIZE                 0x300
#define DCACHE_MMU_SIZE                 0x300

#define MMU_BUS_START(i)                ((i) * 0x100)
#define MMU_BUS_SIZE                    0x100

#define MMU_INVALID                     BIT(14)
#define MMU_VALID                       0
#define MMU_ACCESS_FLASH                BIT(15)
#define MMU_ACCESS_SPIRAM               BIT(16)

/**
 * MMU entry valid bit mask for mapping value. For an entry:
 * valid bit + value bits
 * valid bit is BIT(14), so value bits are 0x3fff
 */
#define MMU_VALID_VAL_MASK 0x3fff
/**
 * Max MMU available paddr page num.
 * `MMU_MAX_PADDR_PAGE_NUM * SOC_MMU_PAGE_SIZE` means the max paddr address supported by the MMU. e.g.:
 * 16384 * 64KB, means MMU can support 1GB paddr at most
 */
#define MMU_MAX_PADDR_PAGE_NUM    16384
/**
 * This is the mask used for mapping. e.g.:
 * 0x4200_0000 & MMU_VADDR_MASK
 */
#define MMU_VADDR_MASK  0x3FFFFF
//MMU entry num
#define MMU_ENTRY_NUM   384

#define BUS_NUM_MASK  0x3

#define CACHE_MEMORY_BANK_SIZE         8192
#define CACHE_MEMORY_BANK_NUM          4
#define CACHE_MEMORY_BANK_NUM_MASK     0x3
#define CACHE_MEMORY_LAYOUT_SHIFT       4
#define CACHE_MEMORY_LAYOUT_SHIFT0      0
#define CACHE_MEMORY_LAYOUT_SHIFT1      4
#define CACHE_MEMORY_LAYOUT_SHIFT2      8
#define CACHE_MEMORY_LAYOUT_SHIFT3      12
#define CACHE_MEMORY_LAYOUT_MASK        0xf
#define CACHE_MEMORY_BANK0_ADDR        0x3FFB0000
#define CACHE_MEMORY_BANK1_ADDR        0x3FFB2000
#define CACHE_MEMORY_BANK2_ADDR        0x3FFB4000
#define CACHE_MEMORY_BANK3_ADDR        0x3FFB6000


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

#define SOC_MMU_IRAM1_LINEAR_ADDRESS_LOW      (IRAM1_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_IRAM1_LINEAR_ADDRESS_HIGH     (IRAM1_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DROM0_LINEAR_ADDRESS_LOW      (DROM0_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DROM0_LINEAR_ADDRESS_HIGH     (DROM0_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DPORT_LINEAR_ADDRESS_LOW      (DPORT_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DPORT_LINEAR_ADDRESS_HIGH     (DPORT_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DRAM1_LINEAR_ADDRESS_LOW      (DRAM1_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DRAM1_LINEAR_ADDRESS_HIGH     (DRAM1_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#define SOC_MMU_DRAM0_LINEAR_ADDRESS_LOW      (DRAM0_CACHE_ADDRESS_LOW & SOC_MMU_LINEAR_ADDR_MASK)
#define SOC_MMU_DRAM0_LINEAR_ADDRESS_HIGH     (DRAM0_CACHE_ADDRESS_HIGH & SOC_MMU_LINEAR_ADDR_MASK)

#ifdef __cplusplus
}
#endif

#endif /*_CACHE_MEMORY_H_ */
