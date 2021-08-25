// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _CACHE_MEMORY_H_
#define _CACHE_MEMORY_H_

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

// #define MMU_SIZE                        0x600
#define ICACHE_MMU_SIZE                 0x300
#define DCACHE_MMU_SIZE                 0x300

#define MMU_BUS_START(i)                ((i) * 0x100)
#define MMU_BUS_SIZE                    0x100

#define MMU_INVALID                     BIT(14)
#define MMU_ACCESS_FLASH                BIT(15)
#define MMU_ACCESS_SPIRAM               BIT(16)

#define FLASH_MMU_TABLE ((volatile uint32_t*) DR_REG_MMU_TABLE)
#define FLASH_MMU_TABLE_SIZE (ICACHE_MMU_SIZE/sizeof(uint32_t))

#define MMU_TABLE_INVALID_VAL 0x4000
#define FLASH_MMU_TABLE_INVALID_VAL DPORT_MMU_TABLE_INVALID_VAL
#define MMU_ADDRESS_MASK 0x3fff
#define MMU_PAGE_SIZE 0x10000

#define BUS_ADDR_SIZE 0x400000
#define BUS_ADDR_MASK (BUS_ADDR_SIZE - 1)
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

#ifdef __cplusplus
}
#endif

#endif /*_CACHE_MEMORY_H_ */
