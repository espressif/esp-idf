// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_SYSCON_REG_H_
#define _SOC_SYSCON_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"
#define SYSCON_SYSCLK_CONF_REG          (DR_REG_SYSCON_BASE + 0x000)
/* SYSCON_RST_TICK_CNT : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define SYSCON_RST_TICK_CNT  (BIT(12))
#define SYSCON_RST_TICK_CNT_M  (BIT(12))
#define SYSCON_RST_TICK_CNT_V  0x1
#define SYSCON_RST_TICK_CNT_S  12
/* SYSCON_CLK_EN : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define SYSCON_CLK_EN  (BIT(11))
#define SYSCON_CLK_EN_M  (BIT(11))
#define SYSCON_CLK_EN_V  0x1
#define SYSCON_CLK_EN_S  11
/* SYSCON_CLK_320M_EN : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define SYSCON_CLK_320M_EN  (BIT(10))
#define SYSCON_CLK_320M_EN_M  (BIT(10))
#define SYSCON_CLK_320M_EN_V  0x1
#define SYSCON_CLK_320M_EN_S  10

#define SYSCON_TICK_CONF_REG          (DR_REG_SYSCON_BASE + 0x004)
/* SYSCON_TICK_ENABLE : R/W ;bitpos:[16] ;default: 1'd1 ; */
/*description: */
#define SYSCON_TICK_ENABLE  (BIT(16))
#define SYSCON_TICK_ENABLE_M  (BIT(16))
#define SYSCON_TICK_ENABLE_V  0x1
#define SYSCON_TICK_ENABLE_S  16
/* SYSCON_CK8M_TICK_NUM : R/W ;bitpos:[15:8] ;default: 8'd7 ; */
/*description: */
#define SYSCON_CK8M_TICK_NUM  0x000000FF
#define SYSCON_CK8M_TICK_NUM_M  ((SYSCON_CK8M_TICK_NUM_V)<<(SYSCON_CK8M_TICK_NUM_S))
#define SYSCON_CK8M_TICK_NUM_V  0xFF
#define SYSCON_CK8M_TICK_NUM_S  8
/* SYSCON_XTAL_TICK_NUM : R/W ;bitpos:[7:0] ;default: 8'd39 ; */
/*description: */
#define SYSCON_XTAL_TICK_NUM  0x000000FF
#define SYSCON_XTAL_TICK_NUM_M  ((SYSCON_XTAL_TICK_NUM_V)<<(SYSCON_XTAL_TICK_NUM_S))
#define SYSCON_XTAL_TICK_NUM_V  0xFF
#define SYSCON_XTAL_TICK_NUM_S  0

#define SYSCON_CLK_OUT_EN_REG          (DR_REG_SYSCON_BASE + 0x008)
/* SYSCON_CLK_XTAL_OEN : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_XTAL_OEN  (BIT(10))
#define SYSCON_CLK_XTAL_OEN_M  (BIT(10))
#define SYSCON_CLK_XTAL_OEN_V  0x1
#define SYSCON_CLK_XTAL_OEN_S  10
/* SYSCON_CLK40X_BB_OEN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK40X_BB_OEN  (BIT(9))
#define SYSCON_CLK40X_BB_OEN_M  (BIT(9))
#define SYSCON_CLK40X_BB_OEN_V  0x1
#define SYSCON_CLK40X_BB_OEN_S  9
/* SYSCON_CLK_DAC_CPU_OEN : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_DAC_CPU_OEN  (BIT(8))
#define SYSCON_CLK_DAC_CPU_OEN_M  (BIT(8))
#define SYSCON_CLK_DAC_CPU_OEN_V  0x1
#define SYSCON_CLK_DAC_CPU_OEN_S  8
/* SYSCON_CLK_ADC_INF_OEN : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_ADC_INF_OEN  (BIT(7))
#define SYSCON_CLK_ADC_INF_OEN_M  (BIT(7))
#define SYSCON_CLK_ADC_INF_OEN_V  0x1
#define SYSCON_CLK_ADC_INF_OEN_S  7
/* SYSCON_CLK_320M_OEN : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_320M_OEN  (BIT(6))
#define SYSCON_CLK_320M_OEN_M  (BIT(6))
#define SYSCON_CLK_320M_OEN_V  0x1
#define SYSCON_CLK_320M_OEN_S  6
/* SYSCON_CLK160_OEN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK160_OEN  (BIT(5))
#define SYSCON_CLK160_OEN_M  (BIT(5))
#define SYSCON_CLK160_OEN_V  0x1
#define SYSCON_CLK160_OEN_S  5
/* SYSCON_CLK80_OEN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK80_OEN  (BIT(4))
#define SYSCON_CLK80_OEN_M  (BIT(4))
#define SYSCON_CLK80_OEN_V  0x1
#define SYSCON_CLK80_OEN_S  4
/* SYSCON_CLK_BB_OEN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK_BB_OEN  (BIT(3))
#define SYSCON_CLK_BB_OEN_M  (BIT(3))
#define SYSCON_CLK_BB_OEN_V  0x1
#define SYSCON_CLK_BB_OEN_S  3
/* SYSCON_CLK44_OEN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK44_OEN  (BIT(2))
#define SYSCON_CLK44_OEN_M  (BIT(2))
#define SYSCON_CLK44_OEN_V  0x1
#define SYSCON_CLK44_OEN_S  2
/* SYSCON_CLK22_OEN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK22_OEN  (BIT(1))
#define SYSCON_CLK22_OEN_M  (BIT(1))
#define SYSCON_CLK22_OEN_V  0x1
#define SYSCON_CLK22_OEN_S  1
/* SYSCON_CLK20_OEN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSCON_CLK20_OEN  (BIT(0))
#define SYSCON_CLK20_OEN_M  (BIT(0))
#define SYSCON_CLK20_OEN_V  0x1
#define SYSCON_CLK20_OEN_S  0

#define SYSCON_HOST_INF_SEL_REG          (DR_REG_SYSCON_BASE + 0x00C)
/* SYSCON_PERI_IO_SWAP : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define SYSCON_PERI_IO_SWAP  0x000000FF
#define SYSCON_PERI_IO_SWAP_M  ((SYSCON_PERI_IO_SWAP_V)<<(SYSCON_PERI_IO_SWAP_S))
#define SYSCON_PERI_IO_SWAP_V  0xFF
#define SYSCON_PERI_IO_SWAP_S  0

#define SYSCON_EXT_MEM_PMS_LOCK_REG          (DR_REG_SYSCON_BASE + 0x010)
/* SYSCON_EXT_MEM_PMS_LOCK : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSCON_EXT_MEM_PMS_LOCK  (BIT(0))
#define SYSCON_EXT_MEM_PMS_LOCK_M  (BIT(0))
#define SYSCON_EXT_MEM_PMS_LOCK_V  0x1
#define SYSCON_EXT_MEM_PMS_LOCK_S  0

#define SYSCON_FLASH_ACE0_ATTR_REG          (DR_REG_SYSCON_BASE + 0x014)
/* SYSCON_FLASH_ACE0_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE0_ATTR  0x00000007
#define SYSCON_FLASH_ACE0_ATTR_M  ((SYSCON_FLASH_ACE0_ATTR_V)<<(SYSCON_FLASH_ACE0_ATTR_S))
#define SYSCON_FLASH_ACE0_ATTR_V  0x7
#define SYSCON_FLASH_ACE0_ATTR_S  0

#define SYSCON_FLASH_ACE1_ATTR_REG          (DR_REG_SYSCON_BASE + 0x018)
/* SYSCON_FLASH_ACE1_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE1_ATTR  0x00000007
#define SYSCON_FLASH_ACE1_ATTR_M  ((SYSCON_FLASH_ACE1_ATTR_V)<<(SYSCON_FLASH_ACE1_ATTR_S))
#define SYSCON_FLASH_ACE1_ATTR_V  0x7
#define SYSCON_FLASH_ACE1_ATTR_S  0

#define SYSCON_FLASH_ACE2_ATTR_REG          (DR_REG_SYSCON_BASE + 0x01C)
/* SYSCON_FLASH_ACE2_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE2_ATTR  0x00000007
#define SYSCON_FLASH_ACE2_ATTR_M  ((SYSCON_FLASH_ACE2_ATTR_V)<<(SYSCON_FLASH_ACE2_ATTR_S))
#define SYSCON_FLASH_ACE2_ATTR_V  0x7
#define SYSCON_FLASH_ACE2_ATTR_S  0

#define SYSCON_FLASH_ACE3_ATTR_REG          (DR_REG_SYSCON_BASE + 0x020)
/* SYSCON_FLASH_ACE3_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_FLASH_ACE3_ATTR  0x00000007
#define SYSCON_FLASH_ACE3_ATTR_M  ((SYSCON_FLASH_ACE3_ATTR_V)<<(SYSCON_FLASH_ACE3_ATTR_S))
#define SYSCON_FLASH_ACE3_ATTR_V  0x7
#define SYSCON_FLASH_ACE3_ATTR_S  0

#define SYSCON_FLASH_ACE0_ADDR_REG          (DR_REG_SYSCON_BASE + 0x024)
/* SYSCON_FLASH_ACE0_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_FLASH_ACE0_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE0_ADDR_S_M  ((SYSCON_FLASH_ACE0_ADDR_S_V)<<(SYSCON_FLASH_ACE0_ADDR_S_S))
#define SYSCON_FLASH_ACE0_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE0_ADDR_S_S  0

#define SYSCON_FLASH_ACE1_ADDR_REG          (DR_REG_SYSCON_BASE + 0x028)
/* SYSCON_FLASH_ACE1_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h10000000 ; */
/*description: */
#define SYSCON_FLASH_ACE1_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE1_ADDR_S_M  ((SYSCON_FLASH_ACE1_ADDR_S_V)<<(SYSCON_FLASH_ACE1_ADDR_S_S))
#define SYSCON_FLASH_ACE1_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE1_ADDR_S_S  0

#define SYSCON_FLASH_ACE2_ADDR_REG          (DR_REG_SYSCON_BASE + 0x02C)
/* SYSCON_FLASH_ACE2_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h20000000 ; */
/*description: */
#define SYSCON_FLASH_ACE2_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE2_ADDR_S_M  ((SYSCON_FLASH_ACE2_ADDR_S_V)<<(SYSCON_FLASH_ACE2_ADDR_S_S))
#define SYSCON_FLASH_ACE2_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE2_ADDR_S_S  0

#define SYSCON_FLASH_ACE3_ADDR_REG          (DR_REG_SYSCON_BASE + 0x030)
/* SYSCON_FLASH_ACE3_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h30000000 ; */
/*description: */
#define SYSCON_FLASH_ACE3_ADDR_S  0xFFFFFFFF
#define SYSCON_FLASH_ACE3_ADDR_S_M  ((SYSCON_FLASH_ACE3_ADDR_S_V)<<(SYSCON_FLASH_ACE3_ADDR_S_S))
#define SYSCON_FLASH_ACE3_ADDR_S_V  0xFFFFFFFF
#define SYSCON_FLASH_ACE3_ADDR_S_S  0

#define SYSCON_FLASH_ACE0_SIZE_REG          (DR_REG_SYSCON_BASE + 0x034)
/* SYSCON_FLASH_ACE0_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE0_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE0_SIZE_M  ((SYSCON_FLASH_ACE0_SIZE_V)<<(SYSCON_FLASH_ACE0_SIZE_S))
#define SYSCON_FLASH_ACE0_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE0_SIZE_S  0

#define SYSCON_FLASH_ACE1_SIZE_REG          (DR_REG_SYSCON_BASE + 0x038)
/* SYSCON_FLASH_ACE1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE1_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE1_SIZE_M  ((SYSCON_FLASH_ACE1_SIZE_V)<<(SYSCON_FLASH_ACE1_SIZE_S))
#define SYSCON_FLASH_ACE1_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE1_SIZE_S  0

#define SYSCON_FLASH_ACE2_SIZE_REG          (DR_REG_SYSCON_BASE + 0x03C)
/* SYSCON_FLASH_ACE2_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE2_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE2_SIZE_M  ((SYSCON_FLASH_ACE2_SIZE_V)<<(SYSCON_FLASH_ACE2_SIZE_S))
#define SYSCON_FLASH_ACE2_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE2_SIZE_S  0

#define SYSCON_FLASH_ACE3_SIZE_REG          (DR_REG_SYSCON_BASE + 0x040)
/* SYSCON_FLASH_ACE3_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_FLASH_ACE3_SIZE  0x0000FFFF
#define SYSCON_FLASH_ACE3_SIZE_M  ((SYSCON_FLASH_ACE3_SIZE_V)<<(SYSCON_FLASH_ACE3_SIZE_S))
#define SYSCON_FLASH_ACE3_SIZE_V  0xFFFF
#define SYSCON_FLASH_ACE3_SIZE_S  0

#define SYSCON_SRAM_ACE0_ATTR_REG          (DR_REG_SYSCON_BASE + 0x044)
/* SYSCON_SRAM_ACE0_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE0_ATTR  0x00000007
#define SYSCON_SRAM_ACE0_ATTR_M  ((SYSCON_SRAM_ACE0_ATTR_V)<<(SYSCON_SRAM_ACE0_ATTR_S))
#define SYSCON_SRAM_ACE0_ATTR_V  0x7
#define SYSCON_SRAM_ACE0_ATTR_S  0

#define SYSCON_SRAM_ACE1_ATTR_REG          (DR_REG_SYSCON_BASE + 0x048)
/* SYSCON_SRAM_ACE1_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE1_ATTR  0x00000007
#define SYSCON_SRAM_ACE1_ATTR_M  ((SYSCON_SRAM_ACE1_ATTR_V)<<(SYSCON_SRAM_ACE1_ATTR_S))
#define SYSCON_SRAM_ACE1_ATTR_V  0x7
#define SYSCON_SRAM_ACE1_ATTR_S  0

#define SYSCON_SRAM_ACE2_ATTR_REG          (DR_REG_SYSCON_BASE + 0x04C)
/* SYSCON_SRAM_ACE2_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE2_ATTR  0x00000007
#define SYSCON_SRAM_ACE2_ATTR_M  ((SYSCON_SRAM_ACE2_ATTR_V)<<(SYSCON_SRAM_ACE2_ATTR_S))
#define SYSCON_SRAM_ACE2_ATTR_V  0x7
#define SYSCON_SRAM_ACE2_ATTR_S  0

#define SYSCON_SRAM_ACE3_ATTR_REG          (DR_REG_SYSCON_BASE + 0x050)
/* SYSCON_SRAM_ACE3_ATTR : R/W ;bitpos:[2:0] ;default: 3'h7 ; */
/*description: */
#define SYSCON_SRAM_ACE3_ATTR  0x00000007
#define SYSCON_SRAM_ACE3_ATTR_M  ((SYSCON_SRAM_ACE3_ATTR_V)<<(SYSCON_SRAM_ACE3_ATTR_S))
#define SYSCON_SRAM_ACE3_ATTR_V  0x7
#define SYSCON_SRAM_ACE3_ATTR_S  0

#define SYSCON_SRAM_ACE0_ADDR_REG          (DR_REG_SYSCON_BASE + 0x054)
/* SYSCON_SRAM_ACE0_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_SRAM_ACE0_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE0_ADDR_S_M  ((SYSCON_SRAM_ACE0_ADDR_S_V)<<(SYSCON_SRAM_ACE0_ADDR_S_S))
#define SYSCON_SRAM_ACE0_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE0_ADDR_S_S  0

#define SYSCON_SRAM_ACE1_ADDR_REG          (DR_REG_SYSCON_BASE + 0x058)
/* SYSCON_SRAM_ACE1_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h10000000 ; */
/*description: */
#define SYSCON_SRAM_ACE1_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE1_ADDR_S_M  ((SYSCON_SRAM_ACE1_ADDR_S_V)<<(SYSCON_SRAM_ACE1_ADDR_S_S))
#define SYSCON_SRAM_ACE1_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE1_ADDR_S_S  0

#define SYSCON_SRAM_ACE2_ADDR_REG          (DR_REG_SYSCON_BASE + 0x05C)
/* SYSCON_SRAM_ACE2_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h20000000 ; */
/*description: */
#define SYSCON_SRAM_ACE2_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE2_ADDR_S_M  ((SYSCON_SRAM_ACE2_ADDR_S_V)<<(SYSCON_SRAM_ACE2_ADDR_S_S))
#define SYSCON_SRAM_ACE2_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE2_ADDR_S_S  0

#define SYSCON_SRAM_ACE3_ADDR_REG          (DR_REG_SYSCON_BASE + 0x060)
/* SYSCON_SRAM_ACE3_ADDR_S : R/W ;bitpos:[31:0] ;default: 32'h30000000 ; */
/*description: */
#define SYSCON_SRAM_ACE3_ADDR_S  0xFFFFFFFF
#define SYSCON_SRAM_ACE3_ADDR_S_M  ((SYSCON_SRAM_ACE3_ADDR_S_V)<<(SYSCON_SRAM_ACE3_ADDR_S_S))
#define SYSCON_SRAM_ACE3_ADDR_S_V  0xFFFFFFFF
#define SYSCON_SRAM_ACE3_ADDR_S_S  0

#define SYSCON_SRAM_ACE0_SIZE_REG          (DR_REG_SYSCON_BASE + 0x064)
/* SYSCON_SRAM_ACE0_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE0_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE0_SIZE_M  ((SYSCON_SRAM_ACE0_SIZE_V)<<(SYSCON_SRAM_ACE0_SIZE_S))
#define SYSCON_SRAM_ACE0_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE0_SIZE_S  0

#define SYSCON_SRAM_ACE1_SIZE_REG          (DR_REG_SYSCON_BASE + 0x068)
/* SYSCON_SRAM_ACE1_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE1_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE1_SIZE_M  ((SYSCON_SRAM_ACE1_SIZE_V)<<(SYSCON_SRAM_ACE1_SIZE_S))
#define SYSCON_SRAM_ACE1_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE1_SIZE_S  0

#define SYSCON_SRAM_ACE2_SIZE_REG          (DR_REG_SYSCON_BASE + 0x06C)
/* SYSCON_SRAM_ACE2_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE2_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE2_SIZE_M  ((SYSCON_SRAM_ACE2_SIZE_V)<<(SYSCON_SRAM_ACE2_SIZE_S))
#define SYSCON_SRAM_ACE2_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE2_SIZE_S  0

#define SYSCON_SRAM_ACE3_SIZE_REG          (DR_REG_SYSCON_BASE + 0x070)
/* SYSCON_SRAM_ACE3_SIZE : R/W ;bitpos:[15:0] ;default: 16'h1000 ; */
/*description: */
#define SYSCON_SRAM_ACE3_SIZE  0x0000FFFF
#define SYSCON_SRAM_ACE3_SIZE_M  ((SYSCON_SRAM_ACE3_SIZE_V)<<(SYSCON_SRAM_ACE3_SIZE_S))
#define SYSCON_SRAM_ACE3_SIZE_V  0xFFFF
#define SYSCON_SRAM_ACE3_SIZE_S  0

#define SYSCON_SPI_MEM_PMS_CTRL_REG          (DR_REG_SYSCON_BASE + 0x074)
/* SYSCON_SPI_MEM_REJECT_CDE : RO ;bitpos:[6:2] ;default: 5'h0 ; */
/*description: */
#define SYSCON_SPI_MEM_REJECT_CDE  0x0000001F
#define SYSCON_SPI_MEM_REJECT_CDE_M  ((SYSCON_SPI_MEM_REJECT_CDE_V)<<(SYSCON_SPI_MEM_REJECT_CDE_S))
#define SYSCON_SPI_MEM_REJECT_CDE_V  0x1F
#define SYSCON_SPI_MEM_REJECT_CDE_S  2
/* SYSCON_SPI_MEM_REJECT_CLR : WOD ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI_MEM_REJECT_CLR  (BIT(1))
#define SYSCON_SPI_MEM_REJECT_CLR_M  (BIT(1))
#define SYSCON_SPI_MEM_REJECT_CLR_V  0x1
#define SYSCON_SPI_MEM_REJECT_CLR_S  1
/* SYSCON_SPI_MEM_REJECT_INT : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSCON_SPI_MEM_REJECT_INT  (BIT(0))
#define SYSCON_SPI_MEM_REJECT_INT_M  (BIT(0))
#define SYSCON_SPI_MEM_REJECT_INT_V  0x1
#define SYSCON_SPI_MEM_REJECT_INT_S  0

#define SYSCON_SPI_MEM_REJECT_ADDR_REG          (DR_REG_SYSCON_BASE + 0x078)
/* SYSCON_SPI_MEM_REJECT_ADDR : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_SPI_MEM_REJECT_ADDR  0xFFFFFFFF
#define SYSCON_SPI_MEM_REJECT_ADDR_M  ((SYSCON_SPI_MEM_REJECT_ADDR_V)<<(SYSCON_SPI_MEM_REJECT_ADDR_S))
#define SYSCON_SPI_MEM_REJECT_ADDR_V  0xFFFFFFFF
#define SYSCON_SPI_MEM_REJECT_ADDR_S  0

#define SYSCON_SDIO_CTRL_REG          (DR_REG_SYSCON_BASE + 0x07C)
/* SYSCON_SDIO_WIN_ACCESS_EN : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define SYSCON_SDIO_WIN_ACCESS_EN  (BIT(0))
#define SYSCON_SDIO_WIN_ACCESS_EN_M  (BIT(0))
#define SYSCON_SDIO_WIN_ACCESS_EN_V  0x1
#define SYSCON_SDIO_WIN_ACCESS_EN_S  0

#define SYSCON_REDCY_SIG0_REG          (DR_REG_SYSCON_BASE + 0x080)
/* SYSCON_REDCY_ANDOR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SYSCON_REDCY_ANDOR  (BIT(31))
#define SYSCON_REDCY_ANDOR_M  (BIT(31))
#define SYSCON_REDCY_ANDOR_V  0x1
#define SYSCON_REDCY_ANDOR_S  31
/* SYSCON_REDCY_SIG0 : R/W ;bitpos:[30:0] ;default: 31'h0 ; */
/*description: */
#define SYSCON_REDCY_SIG0  0x7FFFFFFF
#define SYSCON_REDCY_SIG0_M  ((SYSCON_REDCY_SIG0_V)<<(SYSCON_REDCY_SIG0_S))
#define SYSCON_REDCY_SIG0_V  0x7FFFFFFF
#define SYSCON_REDCY_SIG0_S  0

#define SYSCON_REDCY_SIG1_REG          (DR_REG_SYSCON_BASE + 0x084)
/* SYSCON_REDCY_NANDOR : RO ;bitpos:[31] ;default: 1'h0 ; */
/*description: */
#define SYSCON_REDCY_NANDOR  (BIT(31))
#define SYSCON_REDCY_NANDOR_M  (BIT(31))
#define SYSCON_REDCY_NANDOR_V  0x1
#define SYSCON_REDCY_NANDOR_S  31
/* SYSCON_REDCY_SIG1 : R/W ;bitpos:[30:0] ;default: 31'h0 ; */
/*description: */
#define SYSCON_REDCY_SIG1  0x7FFFFFFF
#define SYSCON_REDCY_SIG1_M  ((SYSCON_REDCY_SIG1_V)<<(SYSCON_REDCY_SIG1_S))
#define SYSCON_REDCY_SIG1_V  0x7FFFFFFF
#define SYSCON_REDCY_SIG1_S  0

#define SYSCON_WIFI_BB_CFG_REG          (DR_REG_SYSCON_BASE + 0x088)
/* SYSCON_WIFI_BB_CFG : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_WIFI_BB_CFG  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_M  ((SYSCON_WIFI_BB_CFG_V)<<(SYSCON_WIFI_BB_CFG_S))
#define SYSCON_WIFI_BB_CFG_V  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_S  0

#define SYSCON_WIFI_BB_CFG_2_REG          (DR_REG_SYSCON_BASE + 0x08C)
/* SYSCON_WIFI_BB_CFG_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_WIFI_BB_CFG_2  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_2_M  ((SYSCON_WIFI_BB_CFG_2_V)<<(SYSCON_WIFI_BB_CFG_2_S))
#define SYSCON_WIFI_BB_CFG_2_V  0xFFFFFFFF
#define SYSCON_WIFI_BB_CFG_2_S  0

#define SYSCON_WIFI_CLK_EN_REG          (DR_REG_SYSCON_BASE + 0x090)
/* SYSCON_WIFI_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'hfffce030 ; */
/*description: */
#define SYSCON_WIFI_CLK_EN  0xFFFFFFFF
#define SYSCON_WIFI_CLK_EN_M  ((SYSCON_WIFI_CLK_EN_V)<<(SYSCON_WIFI_CLK_EN_S))
#define SYSCON_WIFI_CLK_EN_V  0xFFFFFFFF
#define SYSCON_WIFI_CLK_EN_S  0

#define SYSCON_WIFI_RST_EN_REG          (DR_REG_SYSCON_BASE + 0x094)
/* SYSCON_WIFI_RST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define SYSCON_WIFI_RST  0xFFFFFFFF
#define SYSCON_WIFI_RST_M  ((SYSCON_WIFI_RST_V)<<(SYSCON_WIFI_RST_S))
#define SYSCON_WIFI_RST_V  0xFFFFFFFF
#define SYSCON_WIFI_RST_S  0

#define DPORT_WIFI_CLK_EN_REG          SYSCON_WIFI_CLK_EN_REG
/* DPORT_WIFI_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'hfffce030 ; */
/*description: */
#define DPORT_WIFI_CLK_EN  0xFFFFFFFF
#define DPORT_WIFI_CLK_EN_M  ((DPORT_WIFI_CLK_EN_V)<<(DPORT_WIFI_CLK_EN_S))
#define DPORT_WIFI_CLK_EN_V  0xFFFFFFFF
#define DPORT_WIFI_CLK_EN_S  0

/* Mask for all Wifi clock bits - 0, 1, 2, 3, 6, 7, 8, 9, 10, 15, 19, 20, 21
   Bit15 not included here because of the bit now can't be cleared */
#define DPORT_WIFI_CLK_WIFI_EN  0x003807cf
#define DPORT_WIFI_CLK_WIFI_EN_M  ((DPORT_WIFI_CLK_WIFI_EN_V)<<(DPORT_WIFI_CLK_WIFI_EN_S))
#define DPORT_WIFI_CLK_WIFI_EN_V  0x7cf
#define DPORT_WIFI_CLK_WIFI_EN_S  0
/* Mask for all Bluetooth clock bits - 11, 16, 17 */
#define DPORT_WIFI_CLK_BT_EN  0x61
#define DPORT_WIFI_CLK_BT_EN_M  ((DPORT_WIFI_CLK_BT_EN_V)<<(DPORT_WIFI_CLK_BT_EN_S))
#define DPORT_WIFI_CLK_BT_EN_V  0x61
#define DPORT_WIFI_CLK_BT_EN_S  11
/* Mask for clock bits used by both WIFI and Bluetooth, bit 0, 3, 6, 7, 8, 9 */
#define DPORT_WIFI_CLK_WIFI_BT_COMMON_M 0x000003c9

/* Digital team to check */
//bluetooth baseband bit11
#define DPORT_BT_BASEBAND_EN  BIT(11)
//bluetooth LC bit16 and bit17
#define DPORT_BT_LC_EN  (BIT(16)|BIT(17))

/* Remaining single bit clock masks */
#define DPORT_WIFI_CLK_SDIOSLAVE_EN  BIT(4)
#define DPORT_WIFI_CLK_UNUSED_BIT5  BIT(5)
#define DPORT_WIFI_CLK_UNUSED_BIT12  BIT(12)
#define DPORT_WIFI_CLK_SDIO_HOST_EN  BIT(13)
#define DPORT_WIFI_CLK_EMAC_EN  BIT(14)
#define DPORT_WIFI_CLK_RNG_EN  BIT(15)

#define DPORT_CORE_RST_EN_REG         DPORT_WIFI_RST_EN_REG
#define DPORT_WIFI_RST_EN_REG         SYSCON_WIFI_RST_EN_REG
/* DPORT_WIFI_RST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_WIFI_RST  0xFFFFFFFF
#define DPORT_WIFI_RST_M  ((DPORT_WIFI_RST_V)<<(DPORT_WIFI_RST_S))
#define DPORT_WIFI_RST_V  0xFFFFFFFF
#define DPORT_WIFI_RST_S  0
#define DPORT_RW_BTLP_RST (BIT(10))
#define DPORT_RW_BTMAC_RST (BIT(9))
#define DPORT_MACPWR_RST (BIT(8))
#define DPORT_EMAC_RST (BIT(7))
#define DPORT_SDIO_HOST_RST (BIT(6))
#define DPORT_SDIO_RST (BIT(5))
#define DPORT_BTMAC_RST (BIT(4))
#define DPORT_BT_RST (BIT(3))
#define DPORT_MAC_RST (BIT(2))
#define DPORT_FE_RST (BIT(1))
#define DPORT_BB_RST (BIT(0))

#define SYSCON_FRONT_END_MEM_PD_REG          (DR_REG_SYSCON_BASE + 0x098)
/* SYSCON_DC_MEM_FORCE_PD : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SYSCON_DC_MEM_FORCE_PD  (BIT(5))
#define SYSCON_DC_MEM_FORCE_PD_M  (BIT(5))
#define SYSCON_DC_MEM_FORCE_PD_V  0x1
#define SYSCON_DC_MEM_FORCE_PD_S  5
/* SYSCON_DC_MEM_FORCE_PU : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define SYSCON_DC_MEM_FORCE_PU  (BIT(4))
#define SYSCON_DC_MEM_FORCE_PU_M  (BIT(4))
#define SYSCON_DC_MEM_FORCE_PU_V  0x1
#define SYSCON_DC_MEM_FORCE_PU_S  4
/* SYSCON_PBUS_MEM_FORCE_PD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SYSCON_PBUS_MEM_FORCE_PD  (BIT(3))
#define SYSCON_PBUS_MEM_FORCE_PD_M  (BIT(3))
#define SYSCON_PBUS_MEM_FORCE_PD_V  0x1
#define SYSCON_PBUS_MEM_FORCE_PD_S  3
/* SYSCON_PBUS_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define SYSCON_PBUS_MEM_FORCE_PU  (BIT(2))
#define SYSCON_PBUS_MEM_FORCE_PU_M  (BIT(2))
#define SYSCON_PBUS_MEM_FORCE_PU_V  0x1
#define SYSCON_PBUS_MEM_FORCE_PU_S  2
/* SYSCON_AGC_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSCON_AGC_MEM_FORCE_PD  (BIT(1))
#define SYSCON_AGC_MEM_FORCE_PD_M  (BIT(1))
#define SYSCON_AGC_MEM_FORCE_PD_V  0x1
#define SYSCON_AGC_MEM_FORCE_PD_S  1
/* SYSCON_AGC_MEM_FORCE_PU : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSCON_AGC_MEM_FORCE_PU  (BIT(0))
#define SYSCON_AGC_MEM_FORCE_PU_M  (BIT(0))
#define SYSCON_AGC_MEM_FORCE_PU_V  0x1
#define SYSCON_AGC_MEM_FORCE_PU_S  0

#define SYSCON_DATE_REG          (DR_REG_SYSCON_BASE + 0x3FC)
/* SYSCON_DATE : R/W ;bitpos:[31:0] ;default: 32'h1907010 ; */
/*description: */
#define SYSCON_DATE  0xFFFFFFFF
#define SYSCON_DATE_M  ((SYSCON_DATE_V)<<(SYSCON_DATE_S))
#define SYSCON_DATE_V  0xFFFFFFFF
#define SYSCON_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_SYSCON_REG_H_ */
