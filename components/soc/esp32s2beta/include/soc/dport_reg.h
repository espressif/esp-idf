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
#ifndef _SOC_DPORT_REG_H_
#define _SOC_DPORT_REG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include"extmem_reg.h"
#include"interrupt_reg.h"
#include"system_reg.h"
#include "sensitive_reg.h"
#include "soc.h"

/*IRAM0 connected with Cache IBUS0*/
#define IRAM0_ADDRESS_LOW		0x40000000
#define IRAM0_ADDRESS_HIGH		0x40400000
#define IRAM0_CACHE_ADDRESS_LOW		0x40080000
#define IRAM0_CACHE_ADDRESS_HIGH	0x40400000

/*IRAM1 and AHB_IBUS1 connected with Cache IBUS1, alternative*/
#define IRAM1_ADDRESS_LOW		0x40400000
#define IRAM1_ADDRESS_HIGH		0x40800000
#define AHB_IBUS1_ADDRESS_LOW		0x60400000
#define AHB_IBUS1_ADDRESS_HIGH		0x60800000

/*IROM0 and AHB_IBUS2 connected with Cache IBUS2, alternative*/
#define IROM0_ADDRESS_LOW		0x40800000
#define IROM0_ADDRESS_HIGH		0x40c00000
#define AHB_IBUS2_ADDRESS_LOW		0x60800000
#define AHB_IBUS2_ADDRESS_HIGH		0x60c00000

/*DROM0 and AHB_IBUS2 connected with Cache IBUS3, alternative*/
/*DROM0 and AHB_DBUS2 connected with Cache DBUS3, alternative*/
#define DROM0_ADDRESS_LOW		0x3f000000
#define DROM0_ADDRESS_HIGH		0x3f400000
#define AHB_IBUS3_ADDRESS_LOW		0x60c00000
#define AHB_IBUS3_ADDRESS_HIGH		0x61000000
#define AHB_DBUS3_ADDRESS_LOW		0x61800000
#define AHB_DBUS3_ADDRESS_HIGH		0x61c00000

/*DRAM0 and AHB_DBUS2 connected with Cache DBUS0, alternative*/
#define DRAM0_ADDRESS_LOW       	0x3fc00000
#define DRAM0_ADDRESS_HIGH      	0x40000000
#define DRAM0_CACHE_ADDRESS_LOW       	0x3fc00000
#define DRAM0_CACHE_ADDRESS_HIGH      	0x3ff90000
#define AHB_DBUS0_ADDRESS_LOW		0x61000000
#define AHB_DBUS0_ADDRESS_HIGH		0x61400000

/*DRAM1 connected with Cache DBUS1*/
#define DRAM1_ADDRESS_LOW       	0x3f800000
#define DRAM1_ADDRESS_HIGH      	0x3fc00000

/*DPORT and AHB_DBUS2 connected with Cache DBUS2, alternative*/
#define DPORT_ADDRESS_LOW       	0x3f400000
#define DPORT_ADDRESS_HIGH      	0x3f800000
#define DPORT_CACHE_ADDRESS_LOW       	0x3f500000
#define DPORT_CACHE_ADDRESS_HIGH      	0x3f800000
#define AHB_DBUS2_ADDRESS_LOW		0x61400000
#define AHB_DBUS2_ADDRESS_HIGH		0x61800000

#define BUS_SIZE(bus_name)                 (bus_name##_ADDRESS_HIGH - bus_name##_ADDRESS_LOW)
#define ADDRESS_IN_BUS(bus_name, vaddr)    ((vaddr) >= bus_name##_ADDRESS_LOW && (vaddr) < bus_name##_ADDRESS_HIGH)

#define ADDRESS_IN_IRAM0(vaddr)            ADDRESS_IN_BUS(IRAM0, vaddr)
#define ADDRESS_IN_IRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(IRAM0_CACHE, vaddr)
#define ADDRESS_IN_IRAM1(vaddr)            ADDRESS_IN_BUS(IRAM1, vaddr)
#define ADDRESS_IN_AHB_IBUS1(vaddr)        ADDRESS_IN_BUS(AHB_IBUS1, vaddr)
#define ADDRESS_IN_IROM0(vaddr)            ADDRESS_IN_BUS(IROM0, vaddr)
#define ADDRESS_IN_AHB_IBUS2(vaddr)        ADDRESS_IN_BUS(AHB_IBUS2, vaddr)
#define ADDRESS_IN_DROM0(vaddr)            ADDRESS_IN_BUS(DROM0, vaddr)
#define ADDRESS_IN_AHB_IBUS3(vaddr)        ADDRESS_IN_BUS(AHB_IBUS3, vaddr)
#define ADDRESS_IN_AHB_DBUS3(vaddr)        ADDRESS_IN_BUS(AHB_DBUS3, vaddr)
#define ADDRESS_IN_DRAM0(vaddr)            ADDRESS_IN_BUS(DRAM0, vaddr)
#define ADDRESS_IN_DRAM0_CACHE(vaddr)      ADDRESS_IN_BUS(DRAM0_CACHE, vaddr)
#define ADDRESS_IN_AHB_DBUS0(vaddr)        ADDRESS_IN_BUS(AHB_DBUS0, vaddr)
#define ADDRESS_IN_DRAM1(vaddr)            ADDRESS_IN_BUS(DRAM1, vaddr)
#define ADDRESS_IN_DPORT(vaddr)            ADDRESS_IN_BUS(DPORT, vaddr)
#define ADDRESS_IN_DPORT_CACHE(vaddr)      ADDRESS_IN_BUS(DPORT_CACHE, vaddr)
#define ADDRESS_IN_AHB_DBUS2(vaddr)        ADDRESS_IN_BUS(AHB_DBUS2, vaddr)

#define BUS_IRAM0_CACHE_SIZE              BUS_SIZE(IRAM0_CACHE)
#define BUS_IRAM1_CACHE_SIZE              BUS_SIZE(IRAM1)
#define BUS_IROM0_CACHE_SIZE              BUS_SIZE(IROM0)
#define BUS_DROM0_CACHE_SIZE              BUS_SIZE(DROM0)
#define BUS_DRAM0_CACHE_SIZE              BUS_SIZE(DRAM0_CACHE)
#define BUS_DRAM1_CACHE_SIZE              BUS_SIZE(DRAM1)
#define BUS_DPORT_CACHE_SIZE              BUS_SIZE(DPORT)

#define BUS_AHB_IBUS1_CACHE_SIZE          BUS_SIZE(AHB_IBUS1)
#define BUS_AHB_IBUS2_CACHE_SIZE          BUS_SIZE(AHB_IBUS2)
#define BUS_AHB_IBUS3_CACHE_SIZE          BUS_SIZE(AHB_IBUS3)
#define BUS_AHB_DBUS0_CACHE_SIZE          BUS_SIZE(AHB_DBUS0)
#define BUS_AHB_DBUS2_CACHE_SIZE          BUS_SIZE(AHB_DBUS2)
#define BUS_AHB_DBUS3_CACHE_SIZE          BUS_SIZE(AHB_DBUS3)



#define PRO_CACHE_IBUS0			0
#define PRO_CACHE_IBUS0_MMU_START 	0
#define PRO_CACHE_IBUS0_MMU_END 	0x100

#define PRO_CACHE_IBUS1			1
#define PRO_CACHE_IBUS1_MMU_START	0x100
#define PRO_CACHE_IBUS1_MMU_END		0x200

#define PRO_CACHE_IBUS2			2
#define PRO_CACHE_IBUS2_MMU_START	0x200
#define PRO_CACHE_IBUS2_MMU_END		0x300

#define PRO_CACHE_IBUS3			3
#define PRO_CACHE_IBUS3_MMU_START	0x300
#define PRO_CACHE_IBUS3_MMU_END		0x400

#define PRO_CACHE_DBUS0			4
#define PRO_CACHE_DBUS0_MMU_START	0x400
#define PRO_CACHE_DBUS0_MMU_END		0x500

#define PRO_CACHE_DBUS1			5
#define PRO_CACHE_DBUS1_MMU_START	0x500
#define PRO_CACHE_DBUS1_MMU_END		0x600

#define PRO_CACHE_DBUS2			6
#define PRO_CACHE_DBUS2_MMU_START	0x600
#define PRO_CACHE_DBUS2_MMU_END		0x700

#define PRO_CACHE_DBUS3			7
#define PRO_CACHE_DBUS3_MMU_START	0x700
#define PRO_CACHE_DBUS3_MMU_END		0x800

#define DPORT_MMU_SIZE			0x800
#define DPORT_ICACHE_MMU_SIZE		0x400
#define DPORT_DCACHE_MMU_SIZE		0x400

#define DPORT_MMU_BUS_START(i)		((i) * 0x100)
#define DPORT_MMU_BUS_SIZE		0x100

#define DPORT_MMU_INVALID		BIT(14)
#define DPORT_MMU_ACCESS_FLASH		BIT(15)
#define DPORT_MMU_ACCESS_SPIRAM		BIT(16)

/* Flash MMU table for PRO CPU */
#define DPORT_PRO_FLASH_MMU_TABLE ((volatile uint32_t*) DR_REG_MMU_TABLE)

#define DPORT_FLASH_MMU_TABLE_SIZE (DPORT_ICACHE_MMU_SIZE/sizeof(uint32_t))

#define DPORT_MMU_TABLE_INVALID_VAL 0x4000
#define DPORT_FLASH_MMU_TABLE_INVALID_VAL DPORT_MMU_TABLE_INVALID_VAL
#define DPORT_MMU_ADDRESS_MASK 0x3fff

#define BUS_ADDR_SIZE 0x400000
#define BUS_ADDR_MASK (BUS_ADDR_SIZE - 1)
#define BUS_NUM_MASK  0x3

#define CACHE_MEMORY_BLOCK_SIZE         8192
#define CACHE_MEMORY_BLOCK_NUM          4
#define CACHE_MEMORY_BLOCK_NUM_MASK     0x3
#define CACHE_MEMORY_LAYOUT_SHIFT       4
#define CACHE_MEMORY_LAYOUT_SHIFT0      0
#define CACHE_MEMORY_LAYOUT_SHIFT1      4
#define CACHE_MEMORY_LAYOUT_SHIFT2      8
#define CACHE_MEMORY_LAYOUT_SHIFT3      12
#define CACHE_MEMORY_LAYOUT_MASK        0xf
#define CACHE_MEMORY_BLOCK0_ADDR        0x40020000
#define CACHE_MEMORY_BLOCK1_ADDR        0x40022000
#define CACHE_MEMORY_BLOCK2_ADDR        0x40024000
#define CACHE_MEMORY_BLOCK3_ADDR        0x40026000

#define DPORT_DATE_REG SYSTEM_DATE_REG

#ifndef __ASSEMBLER__
#include "dport_access.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /*_SOC_DPORT_REG_H_ */


