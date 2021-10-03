// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include "soc/soc.h"
#include "soc/sensitive_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

//IRAM0 interrupt status bitmasks
#define IRAM0_INTR_ST_OP_TYPE_BIT               BIT(1)      //instruction: 0, data: 1
#define IRAM0_INTR_ST_OP_RW_BIT                 BIT(0)      //read: 0, write: 1

#define CONF_REG_ADDRESS_SHIFT                  2

//IRAM0 range
#define IRAM0_SRAM_BASE_ADDRESS                 0x40000000
#define IRAM0_SRAM_ADDRESS_LOW                  0x40020000
#define IRAM0_SRAM_ADDRESS_HIGH                 0x4006FFFF

//IRAM0 unified managemnt blocks
#define IRAM0_SRAM_TOTAL_UNI_BLOCKS             4
#define IRAM0_SRAM_UNI_BLOCK_0                  0
#define IRAM0_SRAM_UNI_BLOCK_1                  1
#define IRAM0_SRAM_UNI_BLOCK_2                  2
#define IRAM0_SRAM_UNI_BLOCK_3                  3

//unified management addr range (blocks 0-3)
#define IRAM0_SRAM_UNI_BLOCK_0_LOW              0x40020000
#define IRAM0_SRAM_UNI_BLOCK_1_LOW              0x40022000
#define IRAM0_SRAM_UNI_BLOCK_2_LOW              0x40024000
#define IRAM0_SRAM_UNI_BLOCK_3_LOW              0x40026000

//split management addr range (blocks 4-21)
#define IRAM0_SRAM_SPL_BLOCK_LOW                0x40028000 //block 4 low
#define IRAM0_SRAM_SPL_BLOCK_HIGH               0x4006FFFF //block 21 high

#define IRAM0_INTR_ST_FAULTADDR_M               0x003FFFFC  //bits 21:6 in the reg, as well as in real address
#define IRAM0_SRAM_INTR_ST_FAULTADDR_HI         0x40000000  //high nonsignificant bits 31:22 of the faulting address - constant

#define IRAM0_SRAM_ADDR_TO_CONF_REG(addr)       (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_IRAM0_SRAM_4_SPLTADDR) << DPORT_PMS_PRO_IRAM0_SRAM_4_SPLTADDR_S)

//IRAM0 RTCFAST
#define IRAM0_RTCFAST_ADDRESS_LOW               0x40070000
#define IRAM0_RTCFAST_ADDRESS_HIGH              0x40071FFF
#define IRAM0_RTCFAST_INTR_ST_FAULTADDR_HI      0x40070000  //RTCFAST faulting address high bits (31:22, constant)

#define IRAM0_RTCFAST_ADDR_TO_CONF_REG(addr)    (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_IRAM0_RTCFAST_SPLTADDR) << DPORT_PMS_PRO_IRAM0_RTCFAST_SPLTADDR_S)

//DRAM0 interrupt status bitmasks
#define DRAM0_INTR_ST_FAULTADDR_M               0x03FFFFC0  //(bits 25:6 in the reg)
#define DRAM0_INTR_ST_FAULTADDR_S               0x4         //(bits 21:2 of real address)
#define DRAM0_INTR_ST_OP_RW_BIT                 BIT(4)      //read: 0, write: 1
#define DRAM0_INTR_ST_OP_ATOMIC_BIT             BIT(5)      //non-atomic: 0, atomic: 1

#define DRAM0_SRAM_ADDRESS_LOW                  0x3FFB0000
#define DRAM0_SRAM_ADDRESS_HIGH                 0x3FFFFFFF

#define DRAM0_SRAM_TOTAL_UNI_BLOCKS             4
#define DRAM0_SRAM_UNI_BLOCK_0                  0
#define DRAM0_SRAM_UNI_BLOCK_1                  1
#define DRAM0_SRAM_UNI_BLOCK_2                  2
#define DRAM0_SRAM_UNI_BLOCK_3                  3

//unified management (SRAM blocks 0-3)
#define DRAM0_SRAM_UNI_BLOCK_0_LOW              0x3FFB0000
#define DRAM0_SRAM_UNI_BLOCK_1_LOW              0x3FFB2000
#define DRAM0_SRAM_UNI_BLOCK_2_LOW              0x3FFB4000
#define DRAM0_SRAM_UNI_BLOCK_3_LOW              0x3FFB6000

//split management (SRAM blocks 4-21)
#define DRAM0_SRAM_SPL_BLOCK_HIGH               0x3FFFFFFF  //block 21 high
#define DRAM0_SRAM_INTR_ST_FAULTADDR_HI         0x3FF00000  //SRAM high bits 31:22 of the faulting address - constant

#define DRAM0_SRAM_ADDR_TO_CONF_REG(addr)       (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_DRAM0_SRAM_4_SPLTADDR) << DPORT_PMS_PRO_DRAM0_SRAM_4_SPLTADDR_S)

//DRAM0 RTCFAST
#define DRAM0_RTCFAST_ADDRESS_LOW               0x3FF9E000
#define DRAM0_RTCFAST_ADDRESS_HIGH              0x3FF9FFFF
#define DRAM0_RTCFAST_INTR_ST_FAULTADDR_HI      0x3FF00000  //RTCFAST high bits 31:22 of the faulting address - constant
#define DRAM0_RTCFAST_ADDR_TO_CONF_REG(addr)    (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_DRAM0_RTCFAST_SPLTADDR) << DPORT_PMS_PRO_DRAM0_RTCFAST_SPLTADDR_S)

//RTCSLOW
#define RTCSLOW_MEMORY_SIZE                     0x00002000

//PeriBus1 interrupt status bitmasks
#define PERI1_INTR_ST_OP_TYPE_BIT               BIT(4)      //0: non-atomic, 1: atomic
#define PERI1_INTR_ST_OP_HIGH_BITS              BIT(5)      //0: high bits = unchanged, 1: high bits = 0x03F40000
#define PERI1_INTR_ST_FAULTADDR_M               0x03FFFFC0  //(bits 25:6 in the reg)
#define PERI1_INTR_ST_FAULTADDR_S               0x4         //(bits 21:2 of real address)

#define PERI1_RTCSLOW_ADDRESS_BASE              0x3F421000
#define PERI1_RTCSLOW_ADDRESS_LOW               PERI1_RTCSLOW_ADDRESS_BASE
#define PERI1_RTCSLOW_ADDRESS_HIGH              PERI1_RTCSLOW_ADDRESS_LOW + RTCSLOW_MEMORY_SIZE
#define PERI1_RTCSLOW_INTR_ST_FAULTADDR_HI_0    0x3F400000

#define PERI1_RTCSLOW_ADDR_TO_CONF_REG(addr)    (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_DPORT_RTCSLOW_SPLTADDR) << DPORT_PMS_PRO_DPORT_RTCSLOW_SPLTADDR_S)

//PeriBus2 interrupt status bitmasks
#define PERI2_INTR_ST_OP_TYPE_BIT               BIT(1)      //instruction: 0, data: 1
#define PERI2_INTR_ST_OP_RW_BIT                 BIT(0)      //read: 0, write: 1
#define PERI2_INTR_ST_FAULTADDR_M               0xFFFFFFFC  //(bits 31:2 in the reg)

#define PERI2_RTCSLOW_0_ADDRESS_BASE            0x50000000
#define PERI2_RTCSLOW_0_ADDRESS_LOW             PERI2_RTCSLOW_0_ADDRESS_BASE
#define PERI2_RTCSLOW_0_ADDRESS_HIGH            PERI2_RTCSLOW_0_ADDRESS_LOW + RTCSLOW_MEMORY_SIZE

#define PERI2_RTCSLOW_0_ADDR_TO_CONF_REG(addr)  (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_AHB_RTCSLOW_0_SPLTADDR) << DPORT_PMS_PRO_AHB_RTCSLOW_0_SPLTADDR_S)

#define PERI2_RTCSLOW_1_ADDRESS_BASE            0x60021000
#define PERI2_RTCSLOW_1_ADDRESS_LOW             PERI2_RTCSLOW_1_ADDRESS_BASE
#define PERI2_RTCSLOW_1_ADDRESS_HIGH            PERI2_RTCSLOW_1_ADDRESS_LOW + RTCSLOW_MEMORY_SIZE

#define PERI2_RTCSLOW_1_ADDR_TO_CONF_REG(addr)  (((addr >> CONF_REG_ADDRESS_SHIFT) & DPORT_PMS_PRO_AHB_RTCSLOW_1_SPLTADDR) << DPORT_PMS_PRO_AHB_RTCSLOW_1_SPLTADDR_S)

#ifdef __cplusplus
}
#endif
