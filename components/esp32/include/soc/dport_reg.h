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

#include "soc.h"

#define DPORT_PRO_BOOT_REMAP_CTRL_REG          (DR_REG_DPORT_BASE + 0x000)
/* DPORT_PRO_BOOT_REMAP : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_BOOT_REMAP  (BIT(0))
#define DPORT_PRO_BOOT_REMAP_M  (BIT(0))
#define DPORT_PRO_BOOT_REMAP_V  0x1
#define DPORT_PRO_BOOT_REMAP_S  0

#define DPORT_APP_BOOT_REMAP_CTRL_REG          (DR_REG_DPORT_BASE + 0x004)
/* DPORT_APP_BOOT_REMAP : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_BOOT_REMAP  (BIT(0))
#define DPORT_APP_BOOT_REMAP_M  (BIT(0))
#define DPORT_APP_BOOT_REMAP_V  0x1
#define DPORT_APP_BOOT_REMAP_S  0

#define DPORT_ACCESS_CHECK_REG          (DR_REG_DPORT_BASE + 0x008)
/* DPORT_ACCESS_CHECK_APP : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_ACCESS_CHECK_APP  (BIT(8))
#define DPORT_ACCESS_CHECK_APP_M  (BIT(8))
#define DPORT_ACCESS_CHECK_APP_V  0x1
#define DPORT_ACCESS_CHECK_APP_S  8
/* DPORT_ACCESS_CHECK_PRO : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_ACCESS_CHECK_PRO  (BIT(0))
#define DPORT_ACCESS_CHECK_PRO_M  (BIT(0))
#define DPORT_ACCESS_CHECK_PRO_V  0x1
#define DPORT_ACCESS_CHECK_PRO_S  0

#define DPORT_PRO_DPORT_APB_MASK0_REG          (DR_REG_DPORT_BASE + 0x00C)
/* DPORT_PRODPORT_APB_MASK0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PRODPORT_APB_MASK0  0xFFFFFFFF
#define DPORT_PRODPORT_APB_MASK0_M  ((DPORT_PRODPORT_APB_MASK0_V)<<(DPORT_PRODPORT_APB_MASK0_S))
#define DPORT_PRODPORT_APB_MASK0_V  0xFFFFFFFF
#define DPORT_PRODPORT_APB_MASK0_S  0

#define DPORT_PRO_DPORT_APB_MASK1_REG          (DR_REG_DPORT_BASE + 0x010)
/* DPORT_PRODPORT_APB_MASK1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PRODPORT_APB_MASK1  0xFFFFFFFF
#define DPORT_PRODPORT_APB_MASK1_M  ((DPORT_PRODPORT_APB_MASK1_V)<<(DPORT_PRODPORT_APB_MASK1_S))
#define DPORT_PRODPORT_APB_MASK1_V  0xFFFFFFFF
#define DPORT_PRODPORT_APB_MASK1_S  0

#define DPORT_APP_DPORT_APB_MASK0_REG          (DR_REG_DPORT_BASE + 0x014)
/* DPORT_APPDPORT_APB_MASK0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_APPDPORT_APB_MASK0  0xFFFFFFFF
#define DPORT_APPDPORT_APB_MASK0_M  ((DPORT_APPDPORT_APB_MASK0_V)<<(DPORT_APPDPORT_APB_MASK0_S))
#define DPORT_APPDPORT_APB_MASK0_V  0xFFFFFFFF
#define DPORT_APPDPORT_APB_MASK0_S  0

#define DPORT_APP_DPORT_APB_MASK1_REG          (DR_REG_DPORT_BASE + 0x018)
/* DPORT_APPDPORT_APB_MASK1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_APPDPORT_APB_MASK1  0xFFFFFFFF
#define DPORT_APPDPORT_APB_MASK1_M  ((DPORT_APPDPORT_APB_MASK1_V)<<(DPORT_APPDPORT_APB_MASK1_S))
#define DPORT_APPDPORT_APB_MASK1_V  0xFFFFFFFF
#define DPORT_APPDPORT_APB_MASK1_S  0

#define DPORT_PERI_CLK_EN_REG          (DR_REG_DPORT_BASE + 0x01C)
/* DPORT_PERI_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PERI_CLK_EN  0xFFFFFFFF
#define DPORT_PERI_CLK_EN_M  ((DPORT_PERI_CLK_EN_V)<<(DPORT_PERI_CLK_EN_S))
#define DPORT_PERI_CLK_EN_V  0xFFFFFFFF
#define DPORT_PERI_CLK_EN_S  0

#define DPORT_PERI_RST_EN_REG          (DR_REG_DPORT_BASE + 0x020)
/* DPORT_PERI_RST_EN : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PERI_RST_EN  0xFFFFFFFF
#define DPORT_PERI_RST_EN_M  ((DPORT_PERI_RST_EN_V)<<(DPORT_PERI_RST_EN_S))
#define DPORT_PERI_RST_EN_V  0xFFFFFFFF
#define DPORT_PERI_RST_EN_S  0

/* The following bits apply to DPORT_PERI_CLK_EN_REG, DPORT_PERI_RST_EN_REG
 */
#define DPORT_PERI_EN_AES (1<<0)
#define DPORT_PERI_EN_SHA (1<<1)
#define DPORT_PERI_EN_RSA (1<<2)
/* NB: Secure boot reset will hold SHA & AES in reset */
#define DPORT_PERI_EN_SECUREBOOT (1<<3)
/* NB: Digital signature reset will hold AES & RSA in reset */
#define DPORT_PERI_EN_DIGITAL_SIGNATURE (1<<4)

#define DPORT_WIFI_BB_CFG_REG          (DR_REG_DPORT_BASE + 0x024)
/* DPORT_WIFI_BB_CFG : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_WIFI_BB_CFG  0xFFFFFFFF
#define DPORT_WIFI_BB_CFG_M  ((DPORT_WIFI_BB_CFG_V)<<(DPORT_WIFI_BB_CFG_S))
#define DPORT_WIFI_BB_CFG_V  0xFFFFFFFF
#define DPORT_WIFI_BB_CFG_S  0

#define DPORT_WIFI_BB_CFG_2_REG          (DR_REG_DPORT_BASE + 0x028)
/* DPORT_WIFI_BB_CFG_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_WIFI_BB_CFG_2  0xFFFFFFFF
#define DPORT_WIFI_BB_CFG_2_M  ((DPORT_WIFI_BB_CFG_2_V)<<(DPORT_WIFI_BB_CFG_2_S))
#define DPORT_WIFI_BB_CFG_2_V  0xFFFFFFFF
#define DPORT_WIFI_BB_CFG_2_S  0

#define DPORT_APPCPU_CTRL_A_REG          (DR_REG_DPORT_BASE + 0x02C)
/* DPORT_APPCPU_RESETTING : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_APPCPU_RESETTING  (BIT(0))
#define DPORT_APPCPU_RESETTING_M  (BIT(0))
#define DPORT_APPCPU_RESETTING_V  0x1
#define DPORT_APPCPU_RESETTING_S  0

#define DPORT_APPCPU_CTRL_B_REG          (DR_REG_DPORT_BASE + 0x030)
/* DPORT_APPCPU_CLKGATE_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_APPCPU_CLKGATE_EN  (BIT(0))
#define DPORT_APPCPU_CLKGATE_EN_M  (BIT(0))
#define DPORT_APPCPU_CLKGATE_EN_V  0x1
#define DPORT_APPCPU_CLKGATE_EN_S  0

#define DPORT_APPCPU_CTRL_C_REG          (DR_REG_DPORT_BASE + 0x034)
/* DPORT_APPCPU_RUNSTALL : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_APPCPU_RUNSTALL  (BIT(0))
#define DPORT_APPCPU_RUNSTALL_M  (BIT(0))
#define DPORT_APPCPU_RUNSTALL_V  0x1
#define DPORT_APPCPU_RUNSTALL_S  0

#define DPORT_APPCPU_CTRL_D_REG          (DR_REG_DPORT_BASE + 0x038)
/* DPORT_APPCPU_BOOT_ADDR : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_APPCPU_BOOT_ADDR  0xFFFFFFFF
#define DPORT_APPCPU_BOOT_ADDR_M  ((DPORT_APPCPU_BOOT_ADDR_V)<<(DPORT_APPCPU_BOOT_ADDR_S))
#define DPORT_APPCPU_BOOT_ADDR_V  0xFFFFFFFF
#define DPORT_APPCPU_BOOT_ADDR_S  0

#define DPORT_CPU_PER_CONF_REG          (DR_REG_DPORT_BASE + 0x03C)
/* DPORT_FAST_CLK_RTC_SEL : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_FAST_CLK_RTC_SEL  (BIT(3))
#define DPORT_FAST_CLK_RTC_SEL_M  (BIT(3))
#define DPORT_FAST_CLK_RTC_SEL_V  0x1
#define DPORT_FAST_CLK_RTC_SEL_S  3
/* DPORT_LOWSPEED_CLK_SEL : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_LOWSPEED_CLK_SEL  (BIT(2))
#define DPORT_LOWSPEED_CLK_SEL_M  (BIT(2))
#define DPORT_LOWSPEED_CLK_SEL_V  0x1
#define DPORT_LOWSPEED_CLK_SEL_S  2
/* DPORT_CPUPERIOD_SEL : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define DPORT_CPUPERIOD_SEL  0x00000003
#define DPORT_CPUPERIOD_SEL_M  ((DPORT_CPUPERIOD_SEL_V)<<(DPORT_CPUPERIOD_SEL_S))
#define DPORT_CPUPERIOD_SEL_V  0x3
#define DPORT_CPUPERIOD_SEL_S  0

#define DPORT_PRO_CACHE_CTRL_REG          (DR_REG_DPORT_BASE + 0x040)
/* DPORT_PRO_DRAM_HL : R/W ;bitpos:[16] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_DRAM_HL  (BIT(16))
#define DPORT_PRO_DRAM_HL_M  (BIT(16))
#define DPORT_PRO_DRAM_HL_V  0x1
#define DPORT_PRO_DRAM_HL_S  16
/* DPORT_SLAVE_REQ : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define DPORT_SLAVE_REQ  (BIT(15))
#define DPORT_SLAVE_REQ_M  (BIT(15))
#define DPORT_SLAVE_REQ_V  0x1
#define DPORT_SLAVE_REQ_S  15
/* DPORT_AHB_SPI_REQ : RO ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHB_SPI_REQ  (BIT(14))
#define DPORT_AHB_SPI_REQ_M  (BIT(14))
#define DPORT_AHB_SPI_REQ_V  0x1
#define DPORT_AHB_SPI_REQ_S  14
/* DPORT_PRO_SLAVE_REQ : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_SLAVE_REQ  (BIT(13))
#define DPORT_PRO_SLAVE_REQ_M  (BIT(13))
#define DPORT_PRO_SLAVE_REQ_V  0x1
#define DPORT_PRO_SLAVE_REQ_S  13
/* DPORT_PRO_AHB_SPI_REQ : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_AHB_SPI_REQ  (BIT(12))
#define DPORT_PRO_AHB_SPI_REQ_M  (BIT(12))
#define DPORT_PRO_AHB_SPI_REQ_V  0x1
#define DPORT_PRO_AHB_SPI_REQ_S  12
/* DPORT_PRO_DRAM_SPLIT : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_DRAM_SPLIT  (BIT(11))
#define DPORT_PRO_DRAM_SPLIT_M  (BIT(11))
#define DPORT_PRO_DRAM_SPLIT_V  0x1
#define DPORT_PRO_DRAM_SPLIT_S  11
/* DPORT_PRO_SINGLE_IRAM_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_SINGLE_IRAM_ENA  (BIT(10))
#define DPORT_PRO_SINGLE_IRAM_ENA_M  (BIT(10))
#define DPORT_PRO_SINGLE_IRAM_ENA_V  0x1
#define DPORT_PRO_SINGLE_IRAM_ENA_S  10
/* DPORT_PRO_CACHE_LOCK_3_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_3_EN  (BIT(9))
#define DPORT_PRO_CACHE_LOCK_3_EN_M  (BIT(9))
#define DPORT_PRO_CACHE_LOCK_3_EN_V  0x1
#define DPORT_PRO_CACHE_LOCK_3_EN_S  9
/* DPORT_PRO_CACHE_LOCK_2_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_2_EN  (BIT(8))
#define DPORT_PRO_CACHE_LOCK_2_EN_M  (BIT(8))
#define DPORT_PRO_CACHE_LOCK_2_EN_V  0x1
#define DPORT_PRO_CACHE_LOCK_2_EN_S  8
/* DPORT_PRO_CACHE_LOCK_1_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_1_EN  (BIT(7))
#define DPORT_PRO_CACHE_LOCK_1_EN_M  (BIT(7))
#define DPORT_PRO_CACHE_LOCK_1_EN_V  0x1
#define DPORT_PRO_CACHE_LOCK_1_EN_S  7
/* DPORT_PRO_CACHE_LOCK_0_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_0_EN  (BIT(6))
#define DPORT_PRO_CACHE_LOCK_0_EN_M  (BIT(6))
#define DPORT_PRO_CACHE_LOCK_0_EN_V  0x1
#define DPORT_PRO_CACHE_LOCK_0_EN_S  6
/* DPORT_PRO_CACHE_FLUSH_DONE : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_FLUSH_DONE  (BIT(5))
#define DPORT_PRO_CACHE_FLUSH_DONE_M  (BIT(5))
#define DPORT_PRO_CACHE_FLUSH_DONE_V  0x1
#define DPORT_PRO_CACHE_FLUSH_DONE_S  5
/* DPORT_PRO_CACHE_FLUSH_ENA : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_FLUSH_ENA  (BIT(4))
#define DPORT_PRO_CACHE_FLUSH_ENA_M  (BIT(4))
#define DPORT_PRO_CACHE_FLUSH_ENA_V  0x1
#define DPORT_PRO_CACHE_FLUSH_ENA_S  4
/* DPORT_PRO_CACHE_ENABLE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_ENABLE  (BIT(3))
#define DPORT_PRO_CACHE_ENABLE_M  (BIT(3))
#define DPORT_PRO_CACHE_ENABLE_V  0x1
#define DPORT_PRO_CACHE_ENABLE_S  3
/* DPORT_PRO_CACHE_MODE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_MODE  (BIT(2))
#define DPORT_PRO_CACHE_MODE_M  (BIT(2))
#define DPORT_PRO_CACHE_MODE_V  0x1
#define DPORT_PRO_CACHE_MODE_S  2

#define DPORT_PRO_CACHE_CTRL1_REG          (DR_REG_DPORT_BASE + 0x044)
/* DPORT_PRO_CACHE_MMU_IA_CLR : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_MMU_IA_CLR  (BIT(13))
#define DPORT_PRO_CACHE_MMU_IA_CLR_M  (BIT(13))
#define DPORT_PRO_CACHE_MMU_IA_CLR_V  0x1
#define DPORT_PRO_CACHE_MMU_IA_CLR_S  13
/* DPORT_PRO_CMMU_PD : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CMMU_PD  (BIT(12))
#define DPORT_PRO_CMMU_PD_M  (BIT(12))
#define DPORT_PRO_CMMU_PD_V  0x1
#define DPORT_PRO_CMMU_PD_S  12
/* DPORT_PRO_CMMU_FORCE_ON : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CMMU_FORCE_ON  (BIT(11))
#define DPORT_PRO_CMMU_FORCE_ON_M  (BIT(11))
#define DPORT_PRO_CMMU_FORCE_ON_V  0x1
#define DPORT_PRO_CMMU_FORCE_ON_S  11
/* DPORT_PRO_CMMU_FLASH_PAGE_MODE : R/W ;bitpos:[10:9] ;default: 2'b0 ; */
/*description: */
#define DPORT_PRO_CMMU_FLASH_PAGE_MODE  0x00000003
#define DPORT_PRO_CMMU_FLASH_PAGE_MODE_M  ((DPORT_PRO_CMMU_FLASH_PAGE_MODE_V)<<(DPORT_PRO_CMMU_FLASH_PAGE_MODE_S))
#define DPORT_PRO_CMMU_FLASH_PAGE_MODE_V  0x3
#define DPORT_PRO_CMMU_FLASH_PAGE_MODE_S  9
/* DPORT_PRO_CMMU_SRAM_PAGE_MODE : R/W ;bitpos:[8:6] ;default: 3'd3 ; */
/*description: */
#define DPORT_PRO_CMMU_SRAM_PAGE_MODE  0x00000007
#define DPORT_PRO_CMMU_SRAM_PAGE_MODE_M  ((DPORT_PRO_CMMU_SRAM_PAGE_MODE_V)<<(DPORT_PRO_CMMU_SRAM_PAGE_MODE_S))
#define DPORT_PRO_CMMU_SRAM_PAGE_MODE_V  0x7
#define DPORT_PRO_CMMU_SRAM_PAGE_MODE_S  6
/* DPORT_PRO_CACHE_MASK_OPSDRAM : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_MASK_OPSDRAM  (BIT(5))
#define DPORT_PRO_CACHE_MASK_OPSDRAM_M  (BIT(5))
#define DPORT_PRO_CACHE_MASK_OPSDRAM_V  0x1
#define DPORT_PRO_CACHE_MASK_OPSDRAM_S  5
/* DPORT_PRO_CACHE_MASK_DROM0 : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_MASK_DROM0  (BIT(4))
#define DPORT_PRO_CACHE_MASK_DROM0_M  (BIT(4))
#define DPORT_PRO_CACHE_MASK_DROM0_V  0x1
#define DPORT_PRO_CACHE_MASK_DROM0_S  4
/* DPORT_PRO_CACHE_MASK_DRAM1 : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_MASK_DRAM1  (BIT(3))
#define DPORT_PRO_CACHE_MASK_DRAM1_M  (BIT(3))
#define DPORT_PRO_CACHE_MASK_DRAM1_V  0x1
#define DPORT_PRO_CACHE_MASK_DRAM1_S  3
/* DPORT_PRO_CACHE_MASK_IROM0 : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_MASK_IROM0  (BIT(2))
#define DPORT_PRO_CACHE_MASK_IROM0_M  (BIT(2))
#define DPORT_PRO_CACHE_MASK_IROM0_V  0x1
#define DPORT_PRO_CACHE_MASK_IROM0_S  2
/* DPORT_PRO_CACHE_MASK_IRAM1 : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_MASK_IRAM1  (BIT(1))
#define DPORT_PRO_CACHE_MASK_IRAM1_M  (BIT(1))
#define DPORT_PRO_CACHE_MASK_IRAM1_V  0x1
#define DPORT_PRO_CACHE_MASK_IRAM1_S  1
/* DPORT_PRO_CACHE_MASK_IRAM0 : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_MASK_IRAM0  (BIT(0))
#define DPORT_PRO_CACHE_MASK_IRAM0_M  (BIT(0))
#define DPORT_PRO_CACHE_MASK_IRAM0_V  0x1
#define DPORT_PRO_CACHE_MASK_IRAM0_S  0

#define DPORT_PRO_CACHE_LOCK_0_ADDR_REG          (DR_REG_DPORT_BASE + 0x048)
/* DPORT_PRO_CACHE_LOCK_0_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MAX  0x0000000F
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MAX_M  ((DPORT_PRO_CACHE_LOCK_0_ADDR_MAX_V)<<(DPORT_PRO_CACHE_LOCK_0_ADDR_MAX_S))
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MAX_V  0xF
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MAX_S  18
/* DPORT_PRO_CACHE_LOCK_0_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MIN  0x0000000F
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MIN_M  ((DPORT_PRO_CACHE_LOCK_0_ADDR_MIN_V)<<(DPORT_PRO_CACHE_LOCK_0_ADDR_MIN_S))
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MIN_V  0xF
#define DPORT_PRO_CACHE_LOCK_0_ADDR_MIN_S  14
/* DPORT_PRO_CACHE_LOCK_0_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_0_ADDR_PRE  0x00003FFF
#define DPORT_PRO_CACHE_LOCK_0_ADDR_PRE_M  ((DPORT_PRO_CACHE_LOCK_0_ADDR_PRE_V)<<(DPORT_PRO_CACHE_LOCK_0_ADDR_PRE_S))
#define DPORT_PRO_CACHE_LOCK_0_ADDR_PRE_V  0x3FFF
#define DPORT_PRO_CACHE_LOCK_0_ADDR_PRE_S  0

#define DPORT_PRO_CACHE_LOCK_1_ADDR_REG          (DR_REG_DPORT_BASE + 0x04C)
/* DPORT_PRO_CACHE_LOCK_1_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MAX  0x0000000F
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MAX_M  ((DPORT_PRO_CACHE_LOCK_1_ADDR_MAX_V)<<(DPORT_PRO_CACHE_LOCK_1_ADDR_MAX_S))
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MAX_V  0xF
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MAX_S  18
/* DPORT_PRO_CACHE_LOCK_1_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MIN  0x0000000F
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MIN_M  ((DPORT_PRO_CACHE_LOCK_1_ADDR_MIN_V)<<(DPORT_PRO_CACHE_LOCK_1_ADDR_MIN_S))
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MIN_V  0xF
#define DPORT_PRO_CACHE_LOCK_1_ADDR_MIN_S  14
/* DPORT_PRO_CACHE_LOCK_1_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_1_ADDR_PRE  0x00003FFF
#define DPORT_PRO_CACHE_LOCK_1_ADDR_PRE_M  ((DPORT_PRO_CACHE_LOCK_1_ADDR_PRE_V)<<(DPORT_PRO_CACHE_LOCK_1_ADDR_PRE_S))
#define DPORT_PRO_CACHE_LOCK_1_ADDR_PRE_V  0x3FFF
#define DPORT_PRO_CACHE_LOCK_1_ADDR_PRE_S  0

#define DPORT_PRO_CACHE_LOCK_2_ADDR_REG          (DR_REG_DPORT_BASE + 0x050)
/* DPORT_PRO_CACHE_LOCK_2_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MAX  0x0000000F
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MAX_M  ((DPORT_PRO_CACHE_LOCK_2_ADDR_MAX_V)<<(DPORT_PRO_CACHE_LOCK_2_ADDR_MAX_S))
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MAX_V  0xF
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MAX_S  18
/* DPORT_PRO_CACHE_LOCK_2_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MIN  0x0000000F
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MIN_M  ((DPORT_PRO_CACHE_LOCK_2_ADDR_MIN_V)<<(DPORT_PRO_CACHE_LOCK_2_ADDR_MIN_S))
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MIN_V  0xF
#define DPORT_PRO_CACHE_LOCK_2_ADDR_MIN_S  14
/* DPORT_PRO_CACHE_LOCK_2_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_2_ADDR_PRE  0x00003FFF
#define DPORT_PRO_CACHE_LOCK_2_ADDR_PRE_M  ((DPORT_PRO_CACHE_LOCK_2_ADDR_PRE_V)<<(DPORT_PRO_CACHE_LOCK_2_ADDR_PRE_S))
#define DPORT_PRO_CACHE_LOCK_2_ADDR_PRE_V  0x3FFF
#define DPORT_PRO_CACHE_LOCK_2_ADDR_PRE_S  0

#define DPORT_PRO_CACHE_LOCK_3_ADDR_REG          (DR_REG_DPORT_BASE + 0x054)
/* DPORT_PRO_CACHE_LOCK_3_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MAX  0x0000000F
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MAX_M  ((DPORT_PRO_CACHE_LOCK_3_ADDR_MAX_V)<<(DPORT_PRO_CACHE_LOCK_3_ADDR_MAX_S))
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MAX_V  0xF
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MAX_S  18
/* DPORT_PRO_CACHE_LOCK_3_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MIN  0x0000000F
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MIN_M  ((DPORT_PRO_CACHE_LOCK_3_ADDR_MIN_V)<<(DPORT_PRO_CACHE_LOCK_3_ADDR_MIN_S))
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MIN_V  0xF
#define DPORT_PRO_CACHE_LOCK_3_ADDR_MIN_S  14
/* DPORT_PRO_CACHE_LOCK_3_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_PRO_CACHE_LOCK_3_ADDR_PRE  0x00003FFF
#define DPORT_PRO_CACHE_LOCK_3_ADDR_PRE_M  ((DPORT_PRO_CACHE_LOCK_3_ADDR_PRE_V)<<(DPORT_PRO_CACHE_LOCK_3_ADDR_PRE_S))
#define DPORT_PRO_CACHE_LOCK_3_ADDR_PRE_V  0x3FFF
#define DPORT_PRO_CACHE_LOCK_3_ADDR_PRE_S  0

#define DPORT_APP_CACHE_CTRL_REG          (DR_REG_DPORT_BASE + 0x058)
/* DPORT_APP_DRAM_HL : R/W ;bitpos:[14] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_DRAM_HL  (BIT(14))
#define DPORT_APP_DRAM_HL_M  (BIT(14))
#define DPORT_APP_DRAM_HL_V  0x1
#define DPORT_APP_DRAM_HL_S  14
/* DPORT_APP_SLAVE_REQ : RO ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_SLAVE_REQ  (BIT(13))
#define DPORT_APP_SLAVE_REQ_M  (BIT(13))
#define DPORT_APP_SLAVE_REQ_V  0x1
#define DPORT_APP_SLAVE_REQ_S  13
/* DPORT_APP_AHB_SPI_REQ : RO ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_AHB_SPI_REQ  (BIT(12))
#define DPORT_APP_AHB_SPI_REQ_M  (BIT(12))
#define DPORT_APP_AHB_SPI_REQ_V  0x1
#define DPORT_APP_AHB_SPI_REQ_S  12
/* DPORT_APP_DRAM_SPLIT : R/W ;bitpos:[11] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_DRAM_SPLIT  (BIT(11))
#define DPORT_APP_DRAM_SPLIT_M  (BIT(11))
#define DPORT_APP_DRAM_SPLIT_V  0x1
#define DPORT_APP_DRAM_SPLIT_S  11
/* DPORT_APP_SINGLE_IRAM_ENA : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_SINGLE_IRAM_ENA  (BIT(10))
#define DPORT_APP_SINGLE_IRAM_ENA_M  (BIT(10))
#define DPORT_APP_SINGLE_IRAM_ENA_V  0x1
#define DPORT_APP_SINGLE_IRAM_ENA_S  10
/* DPORT_APP_CACHE_LOCK_3_EN : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_3_EN  (BIT(9))
#define DPORT_APP_CACHE_LOCK_3_EN_M  (BIT(9))
#define DPORT_APP_CACHE_LOCK_3_EN_V  0x1
#define DPORT_APP_CACHE_LOCK_3_EN_S  9
/* DPORT_APP_CACHE_LOCK_2_EN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_2_EN  (BIT(8))
#define DPORT_APP_CACHE_LOCK_2_EN_M  (BIT(8))
#define DPORT_APP_CACHE_LOCK_2_EN_V  0x1
#define DPORT_APP_CACHE_LOCK_2_EN_S  8
/* DPORT_APP_CACHE_LOCK_1_EN : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_1_EN  (BIT(7))
#define DPORT_APP_CACHE_LOCK_1_EN_M  (BIT(7))
#define DPORT_APP_CACHE_LOCK_1_EN_V  0x1
#define DPORT_APP_CACHE_LOCK_1_EN_S  7
/* DPORT_APP_CACHE_LOCK_0_EN : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_0_EN  (BIT(6))
#define DPORT_APP_CACHE_LOCK_0_EN_M  (BIT(6))
#define DPORT_APP_CACHE_LOCK_0_EN_V  0x1
#define DPORT_APP_CACHE_LOCK_0_EN_S  6
/* DPORT_APP_CACHE_FLUSH_DONE : RO ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_FLUSH_DONE  (BIT(5))
#define DPORT_APP_CACHE_FLUSH_DONE_M  (BIT(5))
#define DPORT_APP_CACHE_FLUSH_DONE_V  0x1
#define DPORT_APP_CACHE_FLUSH_DONE_S  5
/* DPORT_APP_CACHE_FLUSH_ENA : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_FLUSH_ENA  (BIT(4))
#define DPORT_APP_CACHE_FLUSH_ENA_M  (BIT(4))
#define DPORT_APP_CACHE_FLUSH_ENA_V  0x1
#define DPORT_APP_CACHE_FLUSH_ENA_S  4
/* DPORT_APP_CACHE_ENABLE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_ENABLE  (BIT(3))
#define DPORT_APP_CACHE_ENABLE_M  (BIT(3))
#define DPORT_APP_CACHE_ENABLE_V  0x1
#define DPORT_APP_CACHE_ENABLE_S  3
/* DPORT_APP_CACHE_MODE : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_MODE  (BIT(2))
#define DPORT_APP_CACHE_MODE_M  (BIT(2))
#define DPORT_APP_CACHE_MODE_V  0x1
#define DPORT_APP_CACHE_MODE_S  2

#define DPORT_APP_CACHE_CTRL1_REG          (DR_REG_DPORT_BASE + 0x05C)
/* DPORT_APP_CACHE_MMU_IA_CLR : R/W ;bitpos:[13] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_MMU_IA_CLR  (BIT(13))
#define DPORT_APP_CACHE_MMU_IA_CLR_M  (BIT(13))
#define DPORT_APP_CACHE_MMU_IA_CLR_V  0x1
#define DPORT_APP_CACHE_MMU_IA_CLR_S  13
/* DPORT_APP_CMMU_PD : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CMMU_PD  (BIT(12))
#define DPORT_APP_CMMU_PD_M  (BIT(12))
#define DPORT_APP_CMMU_PD_V  0x1
#define DPORT_APP_CMMU_PD_S  12
/* DPORT_APP_CMMU_FORCE_ON : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CMMU_FORCE_ON  (BIT(11))
#define DPORT_APP_CMMU_FORCE_ON_M  (BIT(11))
#define DPORT_APP_CMMU_FORCE_ON_V  0x1
#define DPORT_APP_CMMU_FORCE_ON_S  11
/* DPORT_APP_CMMU_FLASH_PAGE_MODE : R/W ;bitpos:[10:9] ;default: 2'b0 ; */
/*description: */
#define DPORT_APP_CMMU_FLASH_PAGE_MODE  0x00000003
#define DPORT_APP_CMMU_FLASH_PAGE_MODE_M  ((DPORT_APP_CMMU_FLASH_PAGE_MODE_V)<<(DPORT_APP_CMMU_FLASH_PAGE_MODE_S))
#define DPORT_APP_CMMU_FLASH_PAGE_MODE_V  0x3
#define DPORT_APP_CMMU_FLASH_PAGE_MODE_S  9
/* DPORT_APP_CMMU_SRAM_PAGE_MODE : R/W ;bitpos:[8:6] ;default: 3'd3 ; */
/*description: */
#define DPORT_APP_CMMU_SRAM_PAGE_MODE  0x00000007
#define DPORT_APP_CMMU_SRAM_PAGE_MODE_M  ((DPORT_APP_CMMU_SRAM_PAGE_MODE_V)<<(DPORT_APP_CMMU_SRAM_PAGE_MODE_S))
#define DPORT_APP_CMMU_SRAM_PAGE_MODE_V  0x7
#define DPORT_APP_CMMU_SRAM_PAGE_MODE_S  6
/* DPORT_APP_CACHE_MASK_OPSDRAM : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_MASK_OPSDRAM  (BIT(5))
#define DPORT_APP_CACHE_MASK_OPSDRAM_M  (BIT(5))
#define DPORT_APP_CACHE_MASK_OPSDRAM_V  0x1
#define DPORT_APP_CACHE_MASK_OPSDRAM_S  5
/* DPORT_APP_CACHE_MASK_DROM0 : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_MASK_DROM0  (BIT(4))
#define DPORT_APP_CACHE_MASK_DROM0_M  (BIT(4))
#define DPORT_APP_CACHE_MASK_DROM0_V  0x1
#define DPORT_APP_CACHE_MASK_DROM0_S  4
/* DPORT_APP_CACHE_MASK_DRAM1 : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_MASK_DRAM1  (BIT(3))
#define DPORT_APP_CACHE_MASK_DRAM1_M  (BIT(3))
#define DPORT_APP_CACHE_MASK_DRAM1_V  0x1
#define DPORT_APP_CACHE_MASK_DRAM1_S  3
/* DPORT_APP_CACHE_MASK_IROM0 : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_MASK_IROM0  (BIT(2))
#define DPORT_APP_CACHE_MASK_IROM0_M  (BIT(2))
#define DPORT_APP_CACHE_MASK_IROM0_V  0x1
#define DPORT_APP_CACHE_MASK_IROM0_S  2
/* DPORT_APP_CACHE_MASK_IRAM1 : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_MASK_IRAM1  (BIT(1))
#define DPORT_APP_CACHE_MASK_IRAM1_M  (BIT(1))
#define DPORT_APP_CACHE_MASK_IRAM1_V  0x1
#define DPORT_APP_CACHE_MASK_IRAM1_S  1
/* DPORT_APP_CACHE_MASK_IRAM0 : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_MASK_IRAM0  (BIT(0))
#define DPORT_APP_CACHE_MASK_IRAM0_M  (BIT(0))
#define DPORT_APP_CACHE_MASK_IRAM0_V  0x1
#define DPORT_APP_CACHE_MASK_IRAM0_S  0

#define DPORT_APP_CACHE_LOCK_0_ADDR_REG          (DR_REG_DPORT_BASE + 0x060)
/* DPORT_APP_CACHE_LOCK_0_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_0_ADDR_MAX  0x0000000F
#define DPORT_APP_CACHE_LOCK_0_ADDR_MAX_M  ((DPORT_APP_CACHE_LOCK_0_ADDR_MAX_V)<<(DPORT_APP_CACHE_LOCK_0_ADDR_MAX_S))
#define DPORT_APP_CACHE_LOCK_0_ADDR_MAX_V  0xF
#define DPORT_APP_CACHE_LOCK_0_ADDR_MAX_S  18
/* DPORT_APP_CACHE_LOCK_0_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_0_ADDR_MIN  0x0000000F
#define DPORT_APP_CACHE_LOCK_0_ADDR_MIN_M  ((DPORT_APP_CACHE_LOCK_0_ADDR_MIN_V)<<(DPORT_APP_CACHE_LOCK_0_ADDR_MIN_S))
#define DPORT_APP_CACHE_LOCK_0_ADDR_MIN_V  0xF
#define DPORT_APP_CACHE_LOCK_0_ADDR_MIN_S  14
/* DPORT_APP_CACHE_LOCK_0_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_0_ADDR_PRE  0x00003FFF
#define DPORT_APP_CACHE_LOCK_0_ADDR_PRE_M  ((DPORT_APP_CACHE_LOCK_0_ADDR_PRE_V)<<(DPORT_APP_CACHE_LOCK_0_ADDR_PRE_S))
#define DPORT_APP_CACHE_LOCK_0_ADDR_PRE_V  0x3FFF
#define DPORT_APP_CACHE_LOCK_0_ADDR_PRE_S  0

#define DPORT_APP_CACHE_LOCK_1_ADDR_REG          (DR_REG_DPORT_BASE + 0x064)
/* DPORT_APP_CACHE_LOCK_1_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_1_ADDR_MAX  0x0000000F
#define DPORT_APP_CACHE_LOCK_1_ADDR_MAX_M  ((DPORT_APP_CACHE_LOCK_1_ADDR_MAX_V)<<(DPORT_APP_CACHE_LOCK_1_ADDR_MAX_S))
#define DPORT_APP_CACHE_LOCK_1_ADDR_MAX_V  0xF
#define DPORT_APP_CACHE_LOCK_1_ADDR_MAX_S  18
/* DPORT_APP_CACHE_LOCK_1_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_1_ADDR_MIN  0x0000000F
#define DPORT_APP_CACHE_LOCK_1_ADDR_MIN_M  ((DPORT_APP_CACHE_LOCK_1_ADDR_MIN_V)<<(DPORT_APP_CACHE_LOCK_1_ADDR_MIN_S))
#define DPORT_APP_CACHE_LOCK_1_ADDR_MIN_V  0xF
#define DPORT_APP_CACHE_LOCK_1_ADDR_MIN_S  14
/* DPORT_APP_CACHE_LOCK_1_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_1_ADDR_PRE  0x00003FFF
#define DPORT_APP_CACHE_LOCK_1_ADDR_PRE_M  ((DPORT_APP_CACHE_LOCK_1_ADDR_PRE_V)<<(DPORT_APP_CACHE_LOCK_1_ADDR_PRE_S))
#define DPORT_APP_CACHE_LOCK_1_ADDR_PRE_V  0x3FFF
#define DPORT_APP_CACHE_LOCK_1_ADDR_PRE_S  0

#define DPORT_APP_CACHE_LOCK_2_ADDR_REG          (DR_REG_DPORT_BASE + 0x068)
/* DPORT_APP_CACHE_LOCK_2_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_2_ADDR_MAX  0x0000000F
#define DPORT_APP_CACHE_LOCK_2_ADDR_MAX_M  ((DPORT_APP_CACHE_LOCK_2_ADDR_MAX_V)<<(DPORT_APP_CACHE_LOCK_2_ADDR_MAX_S))
#define DPORT_APP_CACHE_LOCK_2_ADDR_MAX_V  0xF
#define DPORT_APP_CACHE_LOCK_2_ADDR_MAX_S  18
/* DPORT_APP_CACHE_LOCK_2_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_2_ADDR_MIN  0x0000000F
#define DPORT_APP_CACHE_LOCK_2_ADDR_MIN_M  ((DPORT_APP_CACHE_LOCK_2_ADDR_MIN_V)<<(DPORT_APP_CACHE_LOCK_2_ADDR_MIN_S))
#define DPORT_APP_CACHE_LOCK_2_ADDR_MIN_V  0xF
#define DPORT_APP_CACHE_LOCK_2_ADDR_MIN_S  14
/* DPORT_APP_CACHE_LOCK_2_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_2_ADDR_PRE  0x00003FFF
#define DPORT_APP_CACHE_LOCK_2_ADDR_PRE_M  ((DPORT_APP_CACHE_LOCK_2_ADDR_PRE_V)<<(DPORT_APP_CACHE_LOCK_2_ADDR_PRE_S))
#define DPORT_APP_CACHE_LOCK_2_ADDR_PRE_V  0x3FFF
#define DPORT_APP_CACHE_LOCK_2_ADDR_PRE_S  0

#define DPORT_APP_CACHE_LOCK_3_ADDR_REG          (DR_REG_DPORT_BASE + 0x06C)
/* DPORT_APP_CACHE_LOCK_3_ADDR_MAX : R/W ;bitpos:[21:18] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_3_ADDR_MAX  0x0000000F
#define DPORT_APP_CACHE_LOCK_3_ADDR_MAX_M  ((DPORT_APP_CACHE_LOCK_3_ADDR_MAX_V)<<(DPORT_APP_CACHE_LOCK_3_ADDR_MAX_S))
#define DPORT_APP_CACHE_LOCK_3_ADDR_MAX_V  0xF
#define DPORT_APP_CACHE_LOCK_3_ADDR_MAX_S  18
/* DPORT_APP_CACHE_LOCK_3_ADDR_MIN : R/W ;bitpos:[17:14] ;default: 4'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_3_ADDR_MIN  0x0000000F
#define DPORT_APP_CACHE_LOCK_3_ADDR_MIN_M  ((DPORT_APP_CACHE_LOCK_3_ADDR_MIN_V)<<(DPORT_APP_CACHE_LOCK_3_ADDR_MIN_S))
#define DPORT_APP_CACHE_LOCK_3_ADDR_MIN_V  0xF
#define DPORT_APP_CACHE_LOCK_3_ADDR_MIN_S  14
/* DPORT_APP_CACHE_LOCK_3_ADDR_PRE : R/W ;bitpos:[13:0] ;default: 14'h0 ; */
/*description: */
#define DPORT_APP_CACHE_LOCK_3_ADDR_PRE  0x00003FFF
#define DPORT_APP_CACHE_LOCK_3_ADDR_PRE_M  ((DPORT_APP_CACHE_LOCK_3_ADDR_PRE_V)<<(DPORT_APP_CACHE_LOCK_3_ADDR_PRE_S))
#define DPORT_APP_CACHE_LOCK_3_ADDR_PRE_V  0x3FFF
#define DPORT_APP_CACHE_LOCK_3_ADDR_PRE_S  0

#define DPORT_TRACEMEM_MUX_MODE_REG          (DR_REG_DPORT_BASE + 0x070)
/* DPORT_TRACEMEM_MUX_MODE : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define DPORT_TRACEMEM_MUX_MODE  0x00000003
#define DPORT_TRACEMEM_MUX_MODE_M  ((DPORT_TRACEMEM_MUX_MODE_V)<<(DPORT_TRACEMEM_MUX_MODE_S))
#define DPORT_TRACEMEM_MUX_MODE_V  0x3
#define DPORT_TRACEMEM_MUX_MODE_S  0

#define DPORT_PRO_TRACEMEM_ENA_REG          (DR_REG_DPORT_BASE + 0x074)
/* DPORT_PRO_TRACEMEM_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_TRACEMEM_ENA  (BIT(0))
#define DPORT_PRO_TRACEMEM_ENA_M  (BIT(0))
#define DPORT_PRO_TRACEMEM_ENA_V  0x1
#define DPORT_PRO_TRACEMEM_ENA_S  0

#define DPORT_APP_TRACEMEM_ENA_REG          (DR_REG_DPORT_BASE + 0x078)
/* DPORT_APP_TRACEMEM_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_TRACEMEM_ENA  (BIT(0))
#define DPORT_APP_TRACEMEM_ENA_M  (BIT(0))
#define DPORT_APP_TRACEMEM_ENA_V  0x1
#define DPORT_APP_TRACEMEM_ENA_S  0

#define DPORT_CACHE_MUX_MODE_REG          (DR_REG_DPORT_BASE + 0x07C)
/* DPORT_CACHE_MUX_MODE : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define DPORT_CACHE_MUX_MODE  0x00000003
#define DPORT_CACHE_MUX_MODE_M  ((DPORT_CACHE_MUX_MODE_V)<<(DPORT_CACHE_MUX_MODE_S))
#define DPORT_CACHE_MUX_MODE_V  0x3
#define DPORT_CACHE_MUX_MODE_S  0

#define DPORT_IMMU_PAGE_MODE_REG          (DR_REG_DPORT_BASE + 0x080)
/* DPORT_IMMU_PAGE_MODE : R/W ;bitpos:[2:1] ;default: 2'b0 ; */
/*description: */
#define DPORT_IMMU_PAGE_MODE  0x00000003
#define DPORT_IMMU_PAGE_MODE_M  ((DPORT_IMMU_PAGE_MODE_V)<<(DPORT_IMMU_PAGE_MODE_S))
#define DPORT_IMMU_PAGE_MODE_V  0x3
#define DPORT_IMMU_PAGE_MODE_S  1
/* DPORT_INTERNAL_SRAM_IMMU_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_INTERNAL_SRAM_IMMU_ENA  (BIT(0))
#define DPORT_INTERNAL_SRAM_IMMU_ENA_M  (BIT(0))
#define DPORT_INTERNAL_SRAM_IMMU_ENA_V  0x1
#define DPORT_INTERNAL_SRAM_IMMU_ENA_S  0

#define DPORT_DMMU_PAGE_MODE_REG          (DR_REG_DPORT_BASE + 0x084)
/* DPORT_DMMU_PAGE_MODE : R/W ;bitpos:[2:1] ;default: 2'b0 ; */
/*description: */
#define DPORT_DMMU_PAGE_MODE  0x00000003
#define DPORT_DMMU_PAGE_MODE_M  ((DPORT_DMMU_PAGE_MODE_V)<<(DPORT_DMMU_PAGE_MODE_S))
#define DPORT_DMMU_PAGE_MODE_V  0x3
#define DPORT_DMMU_PAGE_MODE_S  1
/* DPORT_INTERNAL_SRAM_DMMU_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_INTERNAL_SRAM_DMMU_ENA  (BIT(0))
#define DPORT_INTERNAL_SRAM_DMMU_ENA_M  (BIT(0))
#define DPORT_INTERNAL_SRAM_DMMU_ENA_V  0x1
#define DPORT_INTERNAL_SRAM_DMMU_ENA_S  0

#define DPORT_ROM_MPU_ENA_REG          (DR_REG_DPORT_BASE + 0x088)
/* DPORT_APP_ROM_MPU_ENA : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_ROM_MPU_ENA  (BIT(2))
#define DPORT_APP_ROM_MPU_ENA_M  (BIT(2))
#define DPORT_APP_ROM_MPU_ENA_V  0x1
#define DPORT_APP_ROM_MPU_ENA_S  2
/* DPORT_PRO_ROM_MPU_ENA : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_ROM_MPU_ENA  (BIT(1))
#define DPORT_PRO_ROM_MPU_ENA_M  (BIT(1))
#define DPORT_PRO_ROM_MPU_ENA_V  0x1
#define DPORT_PRO_ROM_MPU_ENA_S  1
/* DPORT_SHARE_ROM_MPU_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_SHARE_ROM_MPU_ENA  (BIT(0))
#define DPORT_SHARE_ROM_MPU_ENA_M  (BIT(0))
#define DPORT_SHARE_ROM_MPU_ENA_V  0x1
#define DPORT_SHARE_ROM_MPU_ENA_S  0

#define DPORT_MEM_PD_MASK_REG          (DR_REG_DPORT_BASE + 0x08C)
/* DPORT_LSLP_MEM_PD_MASK : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_LSLP_MEM_PD_MASK  (BIT(0))
#define DPORT_LSLP_MEM_PD_MASK_M  (BIT(0))
#define DPORT_LSLP_MEM_PD_MASK_V  0x1
#define DPORT_LSLP_MEM_PD_MASK_S  0

#define DPORT_ROM_PD_CTRL_REG          (DR_REG_DPORT_BASE + 0x090)
/* DPORT_SHARE_ROM_PD : R/W ;bitpos:[7:2] ;default: 6'h0 ; */
/*description: */
#define DPORT_SHARE_ROM_PD  0x0000003F
#define DPORT_SHARE_ROM_PD_M  ((DPORT_SHARE_ROM_PD_V)<<(DPORT_SHARE_ROM_PD_S))
#define DPORT_SHARE_ROM_PD_V  0x3F
#define DPORT_SHARE_ROM_PD_S  2
/* DPORT_APP_ROM_PD : R/W ;bitpos:[1] ;default: 1'h0 ; */
/*description: */
#define DPORT_APP_ROM_PD  (BIT(1))
#define DPORT_APP_ROM_PD_M  (BIT(1))
#define DPORT_APP_ROM_PD_V  0x1
#define DPORT_APP_ROM_PD_S  1
/* DPORT_PRO_ROM_PD : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define DPORT_PRO_ROM_PD  (BIT(0))
#define DPORT_PRO_ROM_PD_M  (BIT(0))
#define DPORT_PRO_ROM_PD_V  0x1
#define DPORT_PRO_ROM_PD_S  0

#define DPORT_ROM_FO_CTRL_REG          (DR_REG_DPORT_BASE + 0x094)
/* DPORT_SHARE_ROM_FO : R/W ;bitpos:[7:2] ;default: 6'h0 ; */
/*description: */
#define DPORT_SHARE_ROM_FO  0x0000003F
#define DPORT_SHARE_ROM_FO_M  ((DPORT_SHARE_ROM_FO_V)<<(DPORT_SHARE_ROM_FO_S))
#define DPORT_SHARE_ROM_FO_V  0x3F
#define DPORT_SHARE_ROM_FO_S  2
/* DPORT_APP_ROM_FO : R/W ;bitpos:[1] ;default: 1'h1 ; */
/*description: */
#define DPORT_APP_ROM_FO  (BIT(1))
#define DPORT_APP_ROM_FO_M  (BIT(1))
#define DPORT_APP_ROM_FO_V  0x1
#define DPORT_APP_ROM_FO_S  1
/* DPORT_PRO_ROM_FO : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: */
#define DPORT_PRO_ROM_FO  (BIT(0))
#define DPORT_PRO_ROM_FO_M  (BIT(0))
#define DPORT_PRO_ROM_FO_V  0x1
#define DPORT_PRO_ROM_FO_S  0

#define DPORT_SRAM_PD_CTRL_0_REG          (DR_REG_DPORT_BASE + 0x098)
/* DPORT_SRAM_PD_0 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_SRAM_PD_0  0xFFFFFFFF
#define DPORT_SRAM_PD_0_M  ((DPORT_SRAM_PD_0_V)<<(DPORT_SRAM_PD_0_S))
#define DPORT_SRAM_PD_0_V  0xFFFFFFFF
#define DPORT_SRAM_PD_0_S  0

#define DPORT_SRAM_PD_CTRL_1_REG          (DR_REG_DPORT_BASE + 0x09C)
/* DPORT_SRAM_PD_1 : R/W ;bitpos:[0] ;default: 1'h0 ; */
/*description: */
#define DPORT_SRAM_PD_1  (BIT(0))
#define DPORT_SRAM_PD_1_M  (BIT(0))
#define DPORT_SRAM_PD_1_V  0x1
#define DPORT_SRAM_PD_1_S  0

#define DPORT_SRAM_FO_CTRL_0_REG          (DR_REG_DPORT_BASE + 0x0A0)
/* DPORT_SRAM_FO_0 : R/W ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: */
#define DPORT_SRAM_FO_0  0xFFFFFFFF
#define DPORT_SRAM_FO_0_M  ((DPORT_SRAM_FO_0_V)<<(DPORT_SRAM_FO_0_S))
#define DPORT_SRAM_FO_0_V  0xFFFFFFFF
#define DPORT_SRAM_FO_0_S  0

#define DPORT_SRAM_FO_CTRL_1_REG          (DR_REG_DPORT_BASE + 0x0A4)
/* DPORT_SRAM_FO_1 : R/W ;bitpos:[0] ;default: 1'h1 ; */
/*description: */
#define DPORT_SRAM_FO_1  (BIT(0))
#define DPORT_SRAM_FO_1_M  (BIT(0))
#define DPORT_SRAM_FO_1_V  0x1
#define DPORT_SRAM_FO_1_S  0

#define DPORT_IRAM_DRAM_AHB_SEL_REG          (DR_REG_DPORT_BASE + 0x0A8)
/* DPORT_MAC_DUMP_MODE : R/W ;bitpos:[6:5] ;default: 2'h0 ; */
/*description: */
#define DPORT_MAC_DUMP_MODE  0x00000003
#define DPORT_MAC_DUMP_MODE_M  ((DPORT_MAC_DUMP_MODE_V)<<(DPORT_MAC_DUMP_MODE_S))
#define DPORT_MAC_DUMP_MODE_V  0x3
#define DPORT_MAC_DUMP_MODE_S  5
/* DPORT_MASK_AHB : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_MASK_AHB  (BIT(4))
#define DPORT_MASK_AHB_M  (BIT(4))
#define DPORT_MASK_AHB_V  0x1
#define DPORT_MASK_AHB_S  4
/* DPORT_MASK_APP_DRAM : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_MASK_APP_DRAM  (BIT(3))
#define DPORT_MASK_APP_DRAM_M  (BIT(3))
#define DPORT_MASK_APP_DRAM_V  0x1
#define DPORT_MASK_APP_DRAM_S  3
/* DPORT_MASK_PRO_DRAM : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_MASK_PRO_DRAM  (BIT(2))
#define DPORT_MASK_PRO_DRAM_M  (BIT(2))
#define DPORT_MASK_PRO_DRAM_V  0x1
#define DPORT_MASK_PRO_DRAM_S  2
/* DPORT_MASK_APP_IRAM : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_MASK_APP_IRAM  (BIT(1))
#define DPORT_MASK_APP_IRAM_M  (BIT(1))
#define DPORT_MASK_APP_IRAM_V  0x1
#define DPORT_MASK_APP_IRAM_S  1
/* DPORT_MASK_PRO_IRAM : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_MASK_PRO_IRAM  (BIT(0))
#define DPORT_MASK_PRO_IRAM_M  (BIT(0))
#define DPORT_MASK_PRO_IRAM_V  0x1
#define DPORT_MASK_PRO_IRAM_S  0

#define DPORT_TAG_FO_CTRL_REG          (DR_REG_DPORT_BASE + 0x0AC)
/* DPORT_APP_CACHE_TAG_PD : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_TAG_PD  (BIT(9))
#define DPORT_APP_CACHE_TAG_PD_M  (BIT(9))
#define DPORT_APP_CACHE_TAG_PD_V  0x1
#define DPORT_APP_CACHE_TAG_PD_S  9
/* DPORT_APP_CACHE_TAG_FORCE_ON : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CACHE_TAG_FORCE_ON  (BIT(8))
#define DPORT_APP_CACHE_TAG_FORCE_ON_M  (BIT(8))
#define DPORT_APP_CACHE_TAG_FORCE_ON_V  0x1
#define DPORT_APP_CACHE_TAG_FORCE_ON_S  8
/* DPORT_PRO_CACHE_TAG_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_TAG_PD  (BIT(1))
#define DPORT_PRO_CACHE_TAG_PD_M  (BIT(1))
#define DPORT_PRO_CACHE_TAG_PD_V  0x1
#define DPORT_PRO_CACHE_TAG_PD_S  1
/* DPORT_PRO_CACHE_TAG_FORCE_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CACHE_TAG_FORCE_ON  (BIT(0))
#define DPORT_PRO_CACHE_TAG_FORCE_ON_M  (BIT(0))
#define DPORT_PRO_CACHE_TAG_FORCE_ON_V  0x1
#define DPORT_PRO_CACHE_TAG_FORCE_ON_S  0

#define DPORT_AHB_LITE_MASK_REG          (DR_REG_DPORT_BASE + 0x0B0)
/* DPORT_AHB_LITE_SDHOST_PID_REG : R/W ;bitpos:[13:11] ;default: 3'b0 ; */
/*description: */
#define DPORT_AHB_LITE_SDHOST_PID_REG  0x00000007
#define DPORT_AHB_LITE_SDHOST_PID_REG_M  ((DPORT_AHB_LITE_SDHOST_PID_REG_V)<<(DPORT_AHB_LITE_SDHOST_PID_REG_S))
#define DPORT_AHB_LITE_SDHOST_PID_REG_V  0x7
#define DPORT_AHB_LITE_SDHOST_PID_REG_S  11
/* DPORT_AHB_LITE_MASK_APPDPORT : R/W ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHB_LITE_MASK_APPDPORT  (BIT(10))
#define DPORT_AHB_LITE_MASK_APPDPORT_M  (BIT(10))
#define DPORT_AHB_LITE_MASK_APPDPORT_V  0x1
#define DPORT_AHB_LITE_MASK_APPDPORT_S  10
/* DPORT_AHB_LITE_MASK_PRODPORT : R/W ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHB_LITE_MASK_PRODPORT  (BIT(9))
#define DPORT_AHB_LITE_MASK_PRODPORT_M  (BIT(9))
#define DPORT_AHB_LITE_MASK_PRODPORT_V  0x1
#define DPORT_AHB_LITE_MASK_PRODPORT_S  9
/* DPORT_AHB_LITE_MASK_SDIO : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHB_LITE_MASK_SDIO  (BIT(8))
#define DPORT_AHB_LITE_MASK_SDIO_M  (BIT(8))
#define DPORT_AHB_LITE_MASK_SDIO_V  0x1
#define DPORT_AHB_LITE_MASK_SDIO_S  8
/* DPORT_AHB_LITE_MASK_APP : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHB_LITE_MASK_APP  (BIT(4))
#define DPORT_AHB_LITE_MASK_APP_M  (BIT(4))
#define DPORT_AHB_LITE_MASK_APP_V  0x1
#define DPORT_AHB_LITE_MASK_APP_S  4
/* DPORT_AHB_LITE_MASK_PRO : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHB_LITE_MASK_PRO  (BIT(0))
#define DPORT_AHB_LITE_MASK_PRO_M  (BIT(0))
#define DPORT_AHB_LITE_MASK_PRO_V  0x1
#define DPORT_AHB_LITE_MASK_PRO_S  0

#define DPORT_AHB_MPU_TABLE_0_REG          (DR_REG_DPORT_BASE + 0x0B4)
/* DPORT_AHB_ACCESS_GRANT_0 : R/W ;bitpos:[31:0] ;default: 32'hffffffff ; */
/*description: */
#define DPORT_AHB_ACCESS_GRANT_0  0xFFFFFFFF
#define DPORT_AHB_ACCESS_GRANT_0_M  ((DPORT_AHB_ACCESS_GRANT_0_V)<<(DPORT_AHB_ACCESS_GRANT_0_S))
#define DPORT_AHB_ACCESS_GRANT_0_V  0xFFFFFFFF
#define DPORT_AHB_ACCESS_GRANT_0_S  0

#define DPORT_AHB_MPU_TABLE_1_REG          (DR_REG_DPORT_BASE + 0x0B8)
/* DPORT_AHB_ACCESS_GRANT_1 : R/W ;bitpos:[8:0] ;default: 9'h1ff ; */
/*description: */
#define DPORT_AHB_ACCESS_GRANT_1  0x000001FF
#define DPORT_AHB_ACCESS_GRANT_1_M  ((DPORT_AHB_ACCESS_GRANT_1_V)<<(DPORT_AHB_ACCESS_GRANT_1_S))
#define DPORT_AHB_ACCESS_GRANT_1_V  0x1FF
#define DPORT_AHB_ACCESS_GRANT_1_S  0

#define DPORT_HOST_INF_SEL_REG          (DR_REG_DPORT_BASE + 0x0BC)
/* DPORT_LINK_DEVICE_SEL : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: */
#define DPORT_LINK_DEVICE_SEL  0x000000FF
#define DPORT_LINK_DEVICE_SEL_M  ((DPORT_LINK_DEVICE_SEL_V)<<(DPORT_LINK_DEVICE_SEL_S))
#define DPORT_LINK_DEVICE_SEL_V  0xFF
#define DPORT_LINK_DEVICE_SEL_S  8
/* DPORT_PERI_IO_SWAP : R/W ;bitpos:[7:0] ;default: 8'h0 ; */
/*description: */
#define DPORT_PERI_IO_SWAP  0x000000FF
#define DPORT_PERI_IO_SWAP_M  ((DPORT_PERI_IO_SWAP_V)<<(DPORT_PERI_IO_SWAP_S))
#define DPORT_PERI_IO_SWAP_V  0xFF
#define DPORT_PERI_IO_SWAP_S  0

#define DPORT_PERIP_CLK_EN_REG          (DR_REG_DPORT_BASE + 0x0C0)
/* DPORT_PERIP_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'hf9c1e06f ; */
/*description: */
#define DPORT_PERIP_CLK_EN  0xFFFFFFFF
#define DPORT_PERIP_CLK_EN_M  ((DPORT_PERIP_CLK_EN_V)<<(DPORT_PERIP_CLK_EN_S))
#define DPORT_PERIP_CLK_EN_V  0xFFFFFFFF
#define DPORT_PERIP_CLK_EN_S  0

#define DPORT_PWM3_CLK_EN (BIT(26))
#define DPORT_PWM2_CLK_EN (BIT(25))
#define DPORT_UART_MEM_CLK_EN   (BIT(24))
#define DPORT_UART2_CLK_EN   (BIT(23))
#define DPORT_SPI_DMA_CLK_EN   (BIT(22))
#define DPORT_I2S1_CLK_EN   (BIT(21))
#define DPORT_PWM1_CLK_EN   (BIT(20))
#define DPORT_CAN_CLK_EN   (BIT(19))
#define DPORT_I2C_EXT1_CLK_EN   (BIT(18))
#define DPORT_PWM0_CLK_EN   (BIT(17))
#define DPORT_SPI_CLK_EN   (BIT(16))
#define DPORT_TIMERGROUP1_CLK_EN   (BIT(15))
#define DPORT_EFUSE_CLK_EN   (BIT(14))
#define DPORT_TIMERGROUP_CLK_EN   (BIT(13))
#define DPORT_UHCI1_CLK_EN   (BIT(12))
#define DPORT_LEDC_CLK_EN   (BIT(11))
#define DPORT_PCNT_CLK_EN   (BIT(10))
#define DPORT_RMT_CLK_EN   (BIT(9))
#define DPORT_UHCI0_CLK_EN   (BIT(8))
#define DPORT_I2C_EXT0_CLK_EN   (BIT(7))
#define DPORT_SPI_CLK_EN_2   (BIT(6))
#define DPORT_UART1_CLK_EN   (BIT(5))
#define DPORT_I2S0_CLK_EN   (BIT(4))
#define DPORT_WDG_CLK_EN   (BIT(3))
#define DPORT_UART_CLK_EN   (BIT(2))
#define DPORT_SPI_CLK_EN_1   (BIT(1))
#define DPORT_TIMERS_CLK_EN   (BIT(0))
#define DPORT_PERIP_RST_EN_REG          (DR_REG_DPORT_BASE + 0x0C4)
/* DPORT_PERIP_RST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PERIP_RST  0xFFFFFFFF
#define DPORT_PERIP_RST_M  ((DPORT_PERIP_RST_V)<<(DPORT_PERIP_RST_S))
#define DPORT_PERIP_RST_V  0xFFFFFFFF
#define DPORT_PERIP_RST_S  0
#define DPORT_PWM3_RST (BIT(26))
#define DPORT_PWM2_RST (BIT(25))
#define DPORT_UART_MEM_RST   (BIT(24))
#define DPORT_UART2_RST   (BIT(23))
#define DPORT_SPI_DMA_RST   (BIT(22))
#define DPORT_I2S1_RST   (BIT(21))
#define DPORT_PWM1_RST   (BIT(20))
#define DPORT_CAN_RST   (BIT(19))
#define DPORT_I2C_EXT1_RST   (BIT(18))
#define DPORT_PWM0_RST   (BIT(17))
#define DPORT_SPI_RST   (BIT(16))
#define DPORT_TIMERGROUP1_RST   (BIT(15))
#define DPORT_EFUSE_RST   (BIT(14))
#define DPORT_TIMERGROUP_RST   (BIT(13))
#define DPORT_UHCI1_RST   (BIT(12))
#define DPORT_LEDC_RST   (BIT(11))
#define DPORT_PCNT_RST   (BIT(10))
#define DPORT_RMT_RST   (BIT(9))
#define DPORT_UHCI0_RST   (BIT(8))
#define DPORT_I2C_EXT0_RST   (BIT(7))
#define DPORT_SPI_RST_2   (BIT(6))
#define DPORT_UART1_RST   (BIT(5))
#define DPORT_I2S0_RST   (BIT(4))
#define DPORT_WDG_RST   (BIT(3))
#define DPORT_UART_RST   (BIT(2))
#define DPORT_SPI_RST_1   (BIT(1))
#define DPORT_TIMERS_RST   (BIT(0))
#define DPORT_SLAVE_SPI_CONFIG_REG          (DR_REG_DPORT_BASE + 0x0C8)
/* DPORT_SPI_DECRYPT_ENABLE : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_SPI_DECRYPT_ENABLE  (BIT(12))
#define DPORT_SPI_DECRYPT_ENABLE_M  (BIT(12))
#define DPORT_SPI_DECRYPT_ENABLE_V  0x1
#define DPORT_SPI_DECRYPT_ENABLE_S  12
/* DPORT_SPI_ENCRYPT_ENABLE : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_SPI_ENCRYPT_ENABLE  (BIT(8))
#define DPORT_SPI_ENCRYPT_ENABLE_M  (BIT(8))
#define DPORT_SPI_ENCRYPT_ENABLE_V  0x1
#define DPORT_SPI_ENCRYPT_ENABLE_S  8
/* DPORT_SLAVE_SPI_MASK_APP : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_SLAVE_SPI_MASK_APP  (BIT(4))
#define DPORT_SLAVE_SPI_MASK_APP_M  (BIT(4))
#define DPORT_SLAVE_SPI_MASK_APP_V  0x1
#define DPORT_SLAVE_SPI_MASK_APP_S  4
/* DPORT_SLAVE_SPI_MASK_PRO : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_SLAVE_SPI_MASK_PRO  (BIT(0))
#define DPORT_SLAVE_SPI_MASK_PRO_M  (BIT(0))
#define DPORT_SLAVE_SPI_MASK_PRO_V  0x1
#define DPORT_SLAVE_SPI_MASK_PRO_S  0

#define DPORT_WIFI_CLK_EN_REG          (DR_REG_DPORT_BASE + 0x0CC)
/* DPORT_WIFI_CLK_EN : R/W ;bitpos:[31:0] ;default: 32'hfffce030 ; */
/*description: */
#define DPORT_WIFI_CLK_EN  0xFFFFFFFF
#define DPORT_WIFI_CLK_EN_M  ((DPORT_WIFI_CLK_EN_V)<<(DPORT_WIFI_CLK_EN_S))
#define DPORT_WIFI_CLK_EN_V  0xFFFFFFFF
#define DPORT_WIFI_CLK_EN_S  0

#define DPORT_CORE_RST_EN_REG          (DR_REG_DPORT_BASE + 0x0D0)
/* DPORT_CORE_RST : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPROT_RW_BTLP_RST (BIT(10))
#define DPROT_RW_BTMAC_RST (BIT(9))
#define DPORT_MACPWR_RST (BIT(8))
#define DPORT_EMAC_RST (BIT(7))
#define DPORT_SDIO_HOST_RST (BIT(6))
#define DPORT_SDIO_RST (BIT(5))
#define DPORT_BTMAC_RST (BIT(4))
#define DPORT_BT_RST (BIT(3))
#define DPORT_MAC_RST (BIT(2))
#define DPORT_FE_RST (BIT(1))
#define DPORT_BB_RST (BIT(0))  

#define DPORT_BT_LPCK_DIV_INT_REG          (DR_REG_DPORT_BASE + 0x0D4)
/* DPORT_BTEXTWAKEUP_REQ : R/W ;bitpos:[12] ;default: 1'b0 ; */
/*description: */
#define DPORT_BTEXTWAKEUP_REQ  (BIT(12))
#define DPORT_BTEXTWAKEUP_REQ_M  (BIT(12))
#define DPORT_BTEXTWAKEUP_REQ_V  0x1
#define DPORT_BTEXTWAKEUP_REQ_S  12
/* DPORT_BT_LPCK_DIV_NUM : R/W ;bitpos:[11:0] ;default: 12'd255 ; */
/*description: */
#define DPORT_BT_LPCK_DIV_NUM  0x00000FFF
#define DPORT_BT_LPCK_DIV_NUM_M  ((DPORT_BT_LPCK_DIV_NUM_V)<<(DPORT_BT_LPCK_DIV_NUM_S))
#define DPORT_BT_LPCK_DIV_NUM_V  0xFFF
#define DPORT_BT_LPCK_DIV_NUM_S  0

#define DPORT_BT_LPCK_DIV_FRAC_REG          (DR_REG_DPORT_BASE + 0x0D8)
/* DPORT_LPCLK_SEL_XTAL32K : R/W ;bitpos:[27] ;default: 1'b0 ; */
/*description: */
#define DPORT_LPCLK_SEL_XTAL32K  (BIT(27))
#define DPORT_LPCLK_SEL_XTAL32K_M  (BIT(27))
#define DPORT_LPCLK_SEL_XTAL32K_V  0x1
#define DPORT_LPCLK_SEL_XTAL32K_S  27
/* DPORT_LPCLK_SEL_XTAL : R/W ;bitpos:[26] ;default: 1'b0 ; */
/*description: */
#define DPORT_LPCLK_SEL_XTAL  (BIT(26))
#define DPORT_LPCLK_SEL_XTAL_M  (BIT(26))
#define DPORT_LPCLK_SEL_XTAL_V  0x1
#define DPORT_LPCLK_SEL_XTAL_S  26
/* DPORT_LPCLK_SEL_8M : R/W ;bitpos:[25] ;default: 1'b1 ; */
/*description: */
#define DPORT_LPCLK_SEL_8M  (BIT(25))
#define DPORT_LPCLK_SEL_8M_M  (BIT(25))
#define DPORT_LPCLK_SEL_8M_V  0x1
#define DPORT_LPCLK_SEL_8M_S  25
/* DPORT_LPCLK_SEL_RTC_SLOW : R/W ;bitpos:[24] ;default: 1'b0 ; */
/*description: */
#define DPORT_LPCLK_SEL_RTC_SLOW  (BIT(24))
#define DPORT_LPCLK_SEL_RTC_SLOW_M  (BIT(24))
#define DPORT_LPCLK_SEL_RTC_SLOW_V  0x1
#define DPORT_LPCLK_SEL_RTC_SLOW_S  24
/* DPORT_BT_LPCK_DIV_A : R/W ;bitpos:[23:12] ;default: 12'd1 ; */
/*description: */
#define DPORT_BT_LPCK_DIV_A  0x00000FFF
#define DPORT_BT_LPCK_DIV_A_M  ((DPORT_BT_LPCK_DIV_A_V)<<(DPORT_BT_LPCK_DIV_A_S))
#define DPORT_BT_LPCK_DIV_A_V  0xFFF
#define DPORT_BT_LPCK_DIV_A_S  12
/* DPORT_BT_LPCK_DIV_B : R/W ;bitpos:[11:0] ;default: 12'd1 ; */
/*description: */
#define DPORT_BT_LPCK_DIV_B  0x00000FFF
#define DPORT_BT_LPCK_DIV_B_M  ((DPORT_BT_LPCK_DIV_B_V)<<(DPORT_BT_LPCK_DIV_B_S))
#define DPORT_BT_LPCK_DIV_B_V  0xFFF
#define DPORT_BT_LPCK_DIV_B_S  0

#define DPORT_CPU_INTR_FROM_CPU_0_REG          (DR_REG_DPORT_BASE + 0x0DC)
/* DPORT_CPU_INTR_FROM_CPU_0 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_0  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_0_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_0_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_0_S  0

#define DPORT_CPU_INTR_FROM_CPU_1_REG          (DR_REG_DPORT_BASE + 0x0E0)
/* DPORT_CPU_INTR_FROM_CPU_1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_1  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_1_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_1_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_1_S  0

#define DPORT_CPU_INTR_FROM_CPU_2_REG          (DR_REG_DPORT_BASE + 0x0E4)
/* DPORT_CPU_INTR_FROM_CPU_2 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_2  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_2_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_2_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_2_S  0

#define DPORT_CPU_INTR_FROM_CPU_3_REG          (DR_REG_DPORT_BASE + 0x0E8)
/* DPORT_CPU_INTR_FROM_CPU_3 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_CPU_INTR_FROM_CPU_3  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_3_M  (BIT(0))
#define DPORT_CPU_INTR_FROM_CPU_3_V  0x1
#define DPORT_CPU_INTR_FROM_CPU_3_S  0

#define DPORT_PRO_INTR_STATUS_0_REG          (DR_REG_DPORT_BASE + 0x0EC)
/* DPORT_PRO_INTR_STATUS_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PRO_INTR_STATUS_0  0xFFFFFFFF
#define DPORT_PRO_INTR_STATUS_0_M  ((DPORT_PRO_INTR_STATUS_0_V)<<(DPORT_PRO_INTR_STATUS_0_S))
#define DPORT_PRO_INTR_STATUS_0_V  0xFFFFFFFF
#define DPORT_PRO_INTR_STATUS_0_S  0

#define DPORT_PRO_INTR_STATUS_1_REG          (DR_REG_DPORT_BASE + 0x0F0)
/* DPORT_PRO_INTR_STATUS_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PRO_INTR_STATUS_1  0xFFFFFFFF
#define DPORT_PRO_INTR_STATUS_1_M  ((DPORT_PRO_INTR_STATUS_1_V)<<(DPORT_PRO_INTR_STATUS_1_S))
#define DPORT_PRO_INTR_STATUS_1_V  0xFFFFFFFF
#define DPORT_PRO_INTR_STATUS_1_S  0

#define DPORT_PRO_INTR_STATUS_2_REG          (DR_REG_DPORT_BASE + 0x0F4)
/* DPORT_PRO_INTR_STATUS_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_PRO_INTR_STATUS_2  0xFFFFFFFF
#define DPORT_PRO_INTR_STATUS_2_M  ((DPORT_PRO_INTR_STATUS_2_V)<<(DPORT_PRO_INTR_STATUS_2_S))
#define DPORT_PRO_INTR_STATUS_2_V  0xFFFFFFFF
#define DPORT_PRO_INTR_STATUS_2_S  0

#define DPORT_APP_INTR_STATUS_0_REG          (DR_REG_DPORT_BASE + 0x0F8)
/* DPORT_APP_INTR_STATUS_0 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_APP_INTR_STATUS_0  0xFFFFFFFF
#define DPORT_APP_INTR_STATUS_0_M  ((DPORT_APP_INTR_STATUS_0_V)<<(DPORT_APP_INTR_STATUS_0_S))
#define DPORT_APP_INTR_STATUS_0_V  0xFFFFFFFF
#define DPORT_APP_INTR_STATUS_0_S  0

#define DPORT_APP_INTR_STATUS_1_REG          (DR_REG_DPORT_BASE + 0x0FC)
/* DPORT_APP_INTR_STATUS_1 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_APP_INTR_STATUS_1  0xFFFFFFFF
#define DPORT_APP_INTR_STATUS_1_M  ((DPORT_APP_INTR_STATUS_1_V)<<(DPORT_APP_INTR_STATUS_1_S))
#define DPORT_APP_INTR_STATUS_1_V  0xFFFFFFFF
#define DPORT_APP_INTR_STATUS_1_S  0

#define DPORT_APP_INTR_STATUS_2_REG          (DR_REG_DPORT_BASE + 0x100)
/* DPORT_APP_INTR_STATUS_2 : RO ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: */
#define DPORT_APP_INTR_STATUS_2  0xFFFFFFFF
#define DPORT_APP_INTR_STATUS_2_M  ((DPORT_APP_INTR_STATUS_2_V)<<(DPORT_APP_INTR_STATUS_2_S))
#define DPORT_APP_INTR_STATUS_2_V  0xFFFFFFFF
#define DPORT_APP_INTR_STATUS_2_S  0

#define DPORT_PRO_MAC_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x104)
/* DPORT_PRO_MAC_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_MAC_INTR_MAP  0x0000001F
#define DPORT_PRO_MAC_INTR_MAP_M  ((DPORT_PRO_MAC_INTR_MAP_V)<<(DPORT_PRO_MAC_INTR_MAP_S))
#define DPORT_PRO_MAC_INTR_MAP_V  0x1F
#define DPORT_PRO_MAC_INTR_MAP_S  0

#define DPORT_PRO_MAC_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x108)
/* DPORT_PRO_MAC_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_MAC_NMI_MAP  0x0000001F
#define DPORT_PRO_MAC_NMI_MAP_M  ((DPORT_PRO_MAC_NMI_MAP_V)<<(DPORT_PRO_MAC_NMI_MAP_S))
#define DPORT_PRO_MAC_NMI_MAP_V  0x1F
#define DPORT_PRO_MAC_NMI_MAP_S  0

#define DPORT_PRO_BB_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x10C)
/* DPORT_PRO_BB_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_BB_INT_MAP  0x0000001F
#define DPORT_PRO_BB_INT_MAP_M  ((DPORT_PRO_BB_INT_MAP_V)<<(DPORT_PRO_BB_INT_MAP_S))
#define DPORT_PRO_BB_INT_MAP_V  0x1F
#define DPORT_PRO_BB_INT_MAP_S  0

#define DPORT_PRO_BT_MAC_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x110)
/* DPORT_PRO_BT_MAC_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_BT_MAC_INT_MAP  0x0000001F
#define DPORT_PRO_BT_MAC_INT_MAP_M  ((DPORT_PRO_BT_MAC_INT_MAP_V)<<(DPORT_PRO_BT_MAC_INT_MAP_S))
#define DPORT_PRO_BT_MAC_INT_MAP_V  0x1F
#define DPORT_PRO_BT_MAC_INT_MAP_S  0

#define DPORT_PRO_BT_BB_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x114)
/* DPORT_PRO_BT_BB_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_BT_BB_INT_MAP  0x0000001F
#define DPORT_PRO_BT_BB_INT_MAP_M  ((DPORT_PRO_BT_BB_INT_MAP_V)<<(DPORT_PRO_BT_BB_INT_MAP_S))
#define DPORT_PRO_BT_BB_INT_MAP_V  0x1F
#define DPORT_PRO_BT_BB_INT_MAP_S  0

#define DPORT_PRO_BT_BB_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x118)
/* DPORT_PRO_BT_BB_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_BT_BB_NMI_MAP  0x0000001F
#define DPORT_PRO_BT_BB_NMI_MAP_M  ((DPORT_PRO_BT_BB_NMI_MAP_V)<<(DPORT_PRO_BT_BB_NMI_MAP_S))
#define DPORT_PRO_BT_BB_NMI_MAP_V  0x1F
#define DPORT_PRO_BT_BB_NMI_MAP_S  0

#define DPORT_PRO_RWBT_IRQ_MAP_REG          (DR_REG_DPORT_BASE + 0x11C)
/* DPORT_PRO_RWBT_IRQ_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_RWBT_IRQ_MAP  0x0000001F
#define DPORT_PRO_RWBT_IRQ_MAP_M  ((DPORT_PRO_RWBT_IRQ_MAP_V)<<(DPORT_PRO_RWBT_IRQ_MAP_S))
#define DPORT_PRO_RWBT_IRQ_MAP_V  0x1F
#define DPORT_PRO_RWBT_IRQ_MAP_S  0

#define DPORT_PRO_RWBLE_IRQ_MAP_REG          (DR_REG_DPORT_BASE + 0x120)
/* DPORT_PRO_RWBLE_IRQ_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_RWBLE_IRQ_MAP  0x0000001F
#define DPORT_PRO_RWBLE_IRQ_MAP_M  ((DPORT_PRO_RWBLE_IRQ_MAP_V)<<(DPORT_PRO_RWBLE_IRQ_MAP_S))
#define DPORT_PRO_RWBLE_IRQ_MAP_V  0x1F
#define DPORT_PRO_RWBLE_IRQ_MAP_S  0

#define DPORT_PRO_RWBT_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x124)
/* DPORT_PRO_RWBT_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_RWBT_NMI_MAP  0x0000001F
#define DPORT_PRO_RWBT_NMI_MAP_M  ((DPORT_PRO_RWBT_NMI_MAP_V)<<(DPORT_PRO_RWBT_NMI_MAP_S))
#define DPORT_PRO_RWBT_NMI_MAP_V  0x1F
#define DPORT_PRO_RWBT_NMI_MAP_S  0

#define DPORT_PRO_RWBLE_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x128)
/* DPORT_PRO_RWBLE_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_RWBLE_NMI_MAP  0x0000001F
#define DPORT_PRO_RWBLE_NMI_MAP_M  ((DPORT_PRO_RWBLE_NMI_MAP_V)<<(DPORT_PRO_RWBLE_NMI_MAP_S))
#define DPORT_PRO_RWBLE_NMI_MAP_V  0x1F
#define DPORT_PRO_RWBLE_NMI_MAP_S  0

#define DPORT_PRO_SLC0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x12C)
/* DPORT_PRO_SLC0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SLC0_INTR_MAP  0x0000001F
#define DPORT_PRO_SLC0_INTR_MAP_M  ((DPORT_PRO_SLC0_INTR_MAP_V)<<(DPORT_PRO_SLC0_INTR_MAP_S))
#define DPORT_PRO_SLC0_INTR_MAP_V  0x1F
#define DPORT_PRO_SLC0_INTR_MAP_S  0

#define DPORT_PRO_SLC1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x130)
/* DPORT_PRO_SLC1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SLC1_INTR_MAP  0x0000001F
#define DPORT_PRO_SLC1_INTR_MAP_M  ((DPORT_PRO_SLC1_INTR_MAP_V)<<(DPORT_PRO_SLC1_INTR_MAP_S))
#define DPORT_PRO_SLC1_INTR_MAP_V  0x1F
#define DPORT_PRO_SLC1_INTR_MAP_S  0

#define DPORT_PRO_UHCI0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x134)
/* DPORT_PRO_UHCI0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_UHCI0_INTR_MAP  0x0000001F
#define DPORT_PRO_UHCI0_INTR_MAP_M  ((DPORT_PRO_UHCI0_INTR_MAP_V)<<(DPORT_PRO_UHCI0_INTR_MAP_S))
#define DPORT_PRO_UHCI0_INTR_MAP_V  0x1F
#define DPORT_PRO_UHCI0_INTR_MAP_S  0

#define DPORT_PRO_UHCI1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x138)
/* DPORT_PRO_UHCI1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_UHCI1_INTR_MAP  0x0000001F
#define DPORT_PRO_UHCI1_INTR_MAP_M  ((DPORT_PRO_UHCI1_INTR_MAP_V)<<(DPORT_PRO_UHCI1_INTR_MAP_S))
#define DPORT_PRO_UHCI1_INTR_MAP_V  0x1F
#define DPORT_PRO_UHCI1_INTR_MAP_S  0

#define DPORT_PRO_TG_T0_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x13C)
/* DPORT_PRO_TG_T0_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_T0_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG_T0_LEVEL_INT_MAP_M  ((DPORT_PRO_TG_T0_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG_T0_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG_T0_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG_T0_LEVEL_INT_MAP_S  0

#define DPORT_PRO_TG_T1_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x140)
/* DPORT_PRO_TG_T1_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_T1_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG_T1_LEVEL_INT_MAP_M  ((DPORT_PRO_TG_T1_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG_T1_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG_T1_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG_T1_LEVEL_INT_MAP_S  0

#define DPORT_PRO_TG_WDT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x144)
/* DPORT_PRO_TG_WDT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_WDT_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG_WDT_LEVEL_INT_MAP_M  ((DPORT_PRO_TG_WDT_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG_WDT_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG_WDT_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG_WDT_LEVEL_INT_MAP_S  0

#define DPORT_PRO_TG_LACT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x148)
/* DPORT_PRO_TG_LACT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_LACT_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG_LACT_LEVEL_INT_MAP_M  ((DPORT_PRO_TG_LACT_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG_LACT_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG_LACT_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG_LACT_LEVEL_INT_MAP_S  0

#define DPORT_PRO_TG1_T0_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x14C)
/* DPORT_PRO_TG1_T0_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_T0_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_T0_LEVEL_INT_MAP_M  ((DPORT_PRO_TG1_T0_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG1_T0_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG1_T0_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_T0_LEVEL_INT_MAP_S  0

#define DPORT_PRO_TG1_T1_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x150)
/* DPORT_PRO_TG1_T1_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_T1_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_T1_LEVEL_INT_MAP_M  ((DPORT_PRO_TG1_T1_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG1_T1_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG1_T1_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_T1_LEVEL_INT_MAP_S  0

#define DPORT_PRO_TG1_WDT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x154)
/* DPORT_PRO_TG1_WDT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_WDT_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_WDT_LEVEL_INT_MAP_M  ((DPORT_PRO_TG1_WDT_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG1_WDT_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG1_WDT_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_WDT_LEVEL_INT_MAP_S  0

#define DPORT_PRO_TG1_LACT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x158)
/* DPORT_PRO_TG1_LACT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_LACT_LEVEL_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_LACT_LEVEL_INT_MAP_M  ((DPORT_PRO_TG1_LACT_LEVEL_INT_MAP_V)<<(DPORT_PRO_TG1_LACT_LEVEL_INT_MAP_S))
#define DPORT_PRO_TG1_LACT_LEVEL_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_LACT_LEVEL_INT_MAP_S  0

#define DPORT_PRO_GPIO_INTERRUPT_MAP_REG          (DR_REG_DPORT_BASE + 0x15C)
/* DPORT_PRO_GPIO_INTERRUPT_PRO_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_GPIO_INTERRUPT_PRO_MAP  0x0000001F
#define DPORT_PRO_GPIO_INTERRUPT_PRO_MAP_M  ((DPORT_PRO_GPIO_INTERRUPT_PRO_MAP_V)<<(DPORT_PRO_GPIO_INTERRUPT_PRO_MAP_S))
#define DPORT_PRO_GPIO_INTERRUPT_PRO_MAP_V  0x1F
#define DPORT_PRO_GPIO_INTERRUPT_PRO_MAP_S  0

#define DPORT_PRO_GPIO_INTERRUPT_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x160)
/* DPORT_PRO_GPIO_INTERRUPT_PRO_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_GPIO_INTERRUPT_PRO_NMI_MAP  0x0000001F
#define DPORT_PRO_GPIO_INTERRUPT_PRO_NMI_MAP_M  ((DPORT_PRO_GPIO_INTERRUPT_PRO_NMI_MAP_V)<<(DPORT_PRO_GPIO_INTERRUPT_PRO_NMI_MAP_S))
#define DPORT_PRO_GPIO_INTERRUPT_PRO_NMI_MAP_V  0x1F
#define DPORT_PRO_GPIO_INTERRUPT_PRO_NMI_MAP_S  0

#define DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP_REG          (DR_REG_DPORT_BASE + 0x164)
/* DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP  0x0000001F
#define DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP_M  ((DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP_V)<<(DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP_S))
#define DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP_V  0x1F
#define DPORT_PRO_CPU_INTR_FROM_CPU_0_MAP_S  0

#define DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP_REG          (DR_REG_DPORT_BASE + 0x168)
/* DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP  0x0000001F
#define DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP_M  ((DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP_V)<<(DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP_S))
#define DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP_V  0x1F
#define DPORT_PRO_CPU_INTR_FROM_CPU_1_MAP_S  0

#define DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP_REG          (DR_REG_DPORT_BASE + 0x16C)
/* DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP  0x0000001F
#define DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP_M  ((DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP_V)<<(DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP_S))
#define DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP_V  0x1F
#define DPORT_PRO_CPU_INTR_FROM_CPU_2_MAP_S  0

#define DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP_REG          (DR_REG_DPORT_BASE + 0x170)
/* DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP  0x0000001F
#define DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP_M  ((DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP_V)<<(DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP_S))
#define DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP_V  0x1F
#define DPORT_PRO_CPU_INTR_FROM_CPU_3_MAP_S  0

#define DPORT_PRO_SPI_INTR_0_MAP_REG          (DR_REG_DPORT_BASE + 0x174)
/* DPORT_PRO_SPI_INTR_0_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SPI_INTR_0_MAP  0x0000001F
#define DPORT_PRO_SPI_INTR_0_MAP_M  ((DPORT_PRO_SPI_INTR_0_MAP_V)<<(DPORT_PRO_SPI_INTR_0_MAP_S))
#define DPORT_PRO_SPI_INTR_0_MAP_V  0x1F
#define DPORT_PRO_SPI_INTR_0_MAP_S  0

#define DPORT_PRO_SPI_INTR_1_MAP_REG          (DR_REG_DPORT_BASE + 0x178)
/* DPORT_PRO_SPI_INTR_1_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SPI_INTR_1_MAP  0x0000001F
#define DPORT_PRO_SPI_INTR_1_MAP_M  ((DPORT_PRO_SPI_INTR_1_MAP_V)<<(DPORT_PRO_SPI_INTR_1_MAP_S))
#define DPORT_PRO_SPI_INTR_1_MAP_V  0x1F
#define DPORT_PRO_SPI_INTR_1_MAP_S  0

#define DPORT_PRO_SPI_INTR_2_MAP_REG          (DR_REG_DPORT_BASE + 0x17C)
/* DPORT_PRO_SPI_INTR_2_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SPI_INTR_2_MAP  0x0000001F
#define DPORT_PRO_SPI_INTR_2_MAP_M  ((DPORT_PRO_SPI_INTR_2_MAP_V)<<(DPORT_PRO_SPI_INTR_2_MAP_S))
#define DPORT_PRO_SPI_INTR_2_MAP_V  0x1F
#define DPORT_PRO_SPI_INTR_2_MAP_S  0

#define DPORT_PRO_SPI_INTR_3_MAP_REG          (DR_REG_DPORT_BASE + 0x180)
/* DPORT_PRO_SPI_INTR_3_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SPI_INTR_3_MAP  0x0000001F
#define DPORT_PRO_SPI_INTR_3_MAP_M  ((DPORT_PRO_SPI_INTR_3_MAP_V)<<(DPORT_PRO_SPI_INTR_3_MAP_S))
#define DPORT_PRO_SPI_INTR_3_MAP_V  0x1F
#define DPORT_PRO_SPI_INTR_3_MAP_S  0

#define DPORT_PRO_I2S0_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x184)
/* DPORT_PRO_I2S0_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_I2S0_INT_MAP  0x0000001F
#define DPORT_PRO_I2S0_INT_MAP_M  ((DPORT_PRO_I2S0_INT_MAP_V)<<(DPORT_PRO_I2S0_INT_MAP_S))
#define DPORT_PRO_I2S0_INT_MAP_V  0x1F
#define DPORT_PRO_I2S0_INT_MAP_S  0

#define DPORT_PRO_I2S1_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x188)
/* DPORT_PRO_I2S1_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_I2S1_INT_MAP  0x0000001F
#define DPORT_PRO_I2S1_INT_MAP_M  ((DPORT_PRO_I2S1_INT_MAP_V)<<(DPORT_PRO_I2S1_INT_MAP_S))
#define DPORT_PRO_I2S1_INT_MAP_V  0x1F
#define DPORT_PRO_I2S1_INT_MAP_S  0

#define DPORT_PRO_UART_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x18C)
/* DPORT_PRO_UART_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_UART_INTR_MAP  0x0000001F
#define DPORT_PRO_UART_INTR_MAP_M  ((DPORT_PRO_UART_INTR_MAP_V)<<(DPORT_PRO_UART_INTR_MAP_S))
#define DPORT_PRO_UART_INTR_MAP_V  0x1F
#define DPORT_PRO_UART_INTR_MAP_S  0

#define DPORT_PRO_UART1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x190)
/* DPORT_PRO_UART1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_UART1_INTR_MAP  0x0000001F
#define DPORT_PRO_UART1_INTR_MAP_M  ((DPORT_PRO_UART1_INTR_MAP_V)<<(DPORT_PRO_UART1_INTR_MAP_S))
#define DPORT_PRO_UART1_INTR_MAP_V  0x1F
#define DPORT_PRO_UART1_INTR_MAP_S  0

#define DPORT_PRO_UART2_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x194)
/* DPORT_PRO_UART2_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_UART2_INTR_MAP  0x0000001F
#define DPORT_PRO_UART2_INTR_MAP_M  ((DPORT_PRO_UART2_INTR_MAP_V)<<(DPORT_PRO_UART2_INTR_MAP_S))
#define DPORT_PRO_UART2_INTR_MAP_V  0x1F
#define DPORT_PRO_UART2_INTR_MAP_S  0

#define DPORT_PRO_SDIO_HOST_INTERRUPT_MAP_REG          (DR_REG_DPORT_BASE + 0x198)
/* DPORT_PRO_SDIO_HOST_INTERRUPT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SDIO_HOST_INTERRUPT_MAP  0x0000001F
#define DPORT_PRO_SDIO_HOST_INTERRUPT_MAP_M  ((DPORT_PRO_SDIO_HOST_INTERRUPT_MAP_V)<<(DPORT_PRO_SDIO_HOST_INTERRUPT_MAP_S))
#define DPORT_PRO_SDIO_HOST_INTERRUPT_MAP_V  0x1F
#define DPORT_PRO_SDIO_HOST_INTERRUPT_MAP_S  0

#define DPORT_PRO_EMAC_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x19C)
/* DPORT_PRO_EMAC_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_EMAC_INT_MAP  0x0000001F
#define DPORT_PRO_EMAC_INT_MAP_M  ((DPORT_PRO_EMAC_INT_MAP_V)<<(DPORT_PRO_EMAC_INT_MAP_S))
#define DPORT_PRO_EMAC_INT_MAP_V  0x1F
#define DPORT_PRO_EMAC_INT_MAP_S  0

#define DPORT_PRO_PWM0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1A0)
/* DPORT_PRO_PWM0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_PWM0_INTR_MAP  0x0000001F
#define DPORT_PRO_PWM0_INTR_MAP_M  ((DPORT_PRO_PWM0_INTR_MAP_V)<<(DPORT_PRO_PWM0_INTR_MAP_S))
#define DPORT_PRO_PWM0_INTR_MAP_V  0x1F
#define DPORT_PRO_PWM0_INTR_MAP_S  0

#define DPORT_PRO_PWM1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1A4)
/* DPORT_PRO_PWM1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_PWM1_INTR_MAP  0x0000001F
#define DPORT_PRO_PWM1_INTR_MAP_M  ((DPORT_PRO_PWM1_INTR_MAP_V)<<(DPORT_PRO_PWM1_INTR_MAP_S))
#define DPORT_PRO_PWM1_INTR_MAP_V  0x1F
#define DPORT_PRO_PWM1_INTR_MAP_S  0

#define DPORT_PRO_PWM2_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1A8)
/* DPORT_PRO_PWM2_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_PWM2_INTR_MAP  0x0000001F
#define DPORT_PRO_PWM2_INTR_MAP_M  ((DPORT_PRO_PWM2_INTR_MAP_V)<<(DPORT_PRO_PWM2_INTR_MAP_S))
#define DPORT_PRO_PWM2_INTR_MAP_V  0x1F
#define DPORT_PRO_PWM2_INTR_MAP_S  0

#define DPORT_PRO_PWM3_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1AC)
/* DPORT_PRO_PWM3_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_PWM3_INTR_MAP  0x0000001F
#define DPORT_PRO_PWM3_INTR_MAP_M  ((DPORT_PRO_PWM3_INTR_MAP_V)<<(DPORT_PRO_PWM3_INTR_MAP_S))
#define DPORT_PRO_PWM3_INTR_MAP_V  0x1F
#define DPORT_PRO_PWM3_INTR_MAP_S  0

#define DPORT_PRO_LEDC_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1B0)
/* DPORT_PRO_LEDC_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_LEDC_INT_MAP  0x0000001F
#define DPORT_PRO_LEDC_INT_MAP_M  ((DPORT_PRO_LEDC_INT_MAP_V)<<(DPORT_PRO_LEDC_INT_MAP_S))
#define DPORT_PRO_LEDC_INT_MAP_V  0x1F
#define DPORT_PRO_LEDC_INT_MAP_S  0

#define DPORT_PRO_EFUSE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1B4)
/* DPORT_PRO_EFUSE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_EFUSE_INT_MAP  0x0000001F
#define DPORT_PRO_EFUSE_INT_MAP_M  ((DPORT_PRO_EFUSE_INT_MAP_V)<<(DPORT_PRO_EFUSE_INT_MAP_S))
#define DPORT_PRO_EFUSE_INT_MAP_V  0x1F
#define DPORT_PRO_EFUSE_INT_MAP_S  0

#define DPORT_PRO_CAN_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1B8)
/* DPORT_PRO_CAN_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_CAN_INT_MAP  0x0000001F
#define DPORT_PRO_CAN_INT_MAP_M  ((DPORT_PRO_CAN_INT_MAP_V)<<(DPORT_PRO_CAN_INT_MAP_S))
#define DPORT_PRO_CAN_INT_MAP_V  0x1F
#define DPORT_PRO_CAN_INT_MAP_S  0

#define DPORT_PRO_RTC_CORE_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1BC)
/* DPORT_PRO_RTC_CORE_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_RTC_CORE_INTR_MAP  0x0000001F
#define DPORT_PRO_RTC_CORE_INTR_MAP_M  ((DPORT_PRO_RTC_CORE_INTR_MAP_V)<<(DPORT_PRO_RTC_CORE_INTR_MAP_S))
#define DPORT_PRO_RTC_CORE_INTR_MAP_V  0x1F
#define DPORT_PRO_RTC_CORE_INTR_MAP_S  0

#define DPORT_PRO_RMT_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1C0)
/* DPORT_PRO_RMT_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_RMT_INTR_MAP  0x0000001F
#define DPORT_PRO_RMT_INTR_MAP_M  ((DPORT_PRO_RMT_INTR_MAP_V)<<(DPORT_PRO_RMT_INTR_MAP_S))
#define DPORT_PRO_RMT_INTR_MAP_V  0x1F
#define DPORT_PRO_RMT_INTR_MAP_S  0

#define DPORT_PRO_PCNT_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1C4)
/* DPORT_PRO_PCNT_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_PCNT_INTR_MAP  0x0000001F
#define DPORT_PRO_PCNT_INTR_MAP_M  ((DPORT_PRO_PCNT_INTR_MAP_V)<<(DPORT_PRO_PCNT_INTR_MAP_S))
#define DPORT_PRO_PCNT_INTR_MAP_V  0x1F
#define DPORT_PRO_PCNT_INTR_MAP_S  0

#define DPORT_PRO_I2C_EXT0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1C8)
/* DPORT_PRO_I2C_EXT0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_I2C_EXT0_INTR_MAP  0x0000001F
#define DPORT_PRO_I2C_EXT0_INTR_MAP_M  ((DPORT_PRO_I2C_EXT0_INTR_MAP_V)<<(DPORT_PRO_I2C_EXT0_INTR_MAP_S))
#define DPORT_PRO_I2C_EXT0_INTR_MAP_V  0x1F
#define DPORT_PRO_I2C_EXT0_INTR_MAP_S  0

#define DPORT_PRO_I2C_EXT1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1CC)
/* DPORT_PRO_I2C_EXT1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_I2C_EXT1_INTR_MAP  0x0000001F
#define DPORT_PRO_I2C_EXT1_INTR_MAP_M  ((DPORT_PRO_I2C_EXT1_INTR_MAP_V)<<(DPORT_PRO_I2C_EXT1_INTR_MAP_S))
#define DPORT_PRO_I2C_EXT1_INTR_MAP_V  0x1F
#define DPORT_PRO_I2C_EXT1_INTR_MAP_S  0

#define DPORT_PRO_RSA_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x1D0)
/* DPORT_PRO_RSA_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_RSA_INTR_MAP  0x0000001F
#define DPORT_PRO_RSA_INTR_MAP_M  ((DPORT_PRO_RSA_INTR_MAP_V)<<(DPORT_PRO_RSA_INTR_MAP_S))
#define DPORT_PRO_RSA_INTR_MAP_V  0x1F
#define DPORT_PRO_RSA_INTR_MAP_S  0

#define DPORT_PRO_SPI1_DMA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1D4)
/* DPORT_PRO_SPI1_DMA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SPI1_DMA_INT_MAP  0x0000001F
#define DPORT_PRO_SPI1_DMA_INT_MAP_M  ((DPORT_PRO_SPI1_DMA_INT_MAP_V)<<(DPORT_PRO_SPI1_DMA_INT_MAP_S))
#define DPORT_PRO_SPI1_DMA_INT_MAP_V  0x1F
#define DPORT_PRO_SPI1_DMA_INT_MAP_S  0

#define DPORT_PRO_SPI2_DMA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1D8)
/* DPORT_PRO_SPI2_DMA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SPI2_DMA_INT_MAP  0x0000001F
#define DPORT_PRO_SPI2_DMA_INT_MAP_M  ((DPORT_PRO_SPI2_DMA_INT_MAP_V)<<(DPORT_PRO_SPI2_DMA_INT_MAP_S))
#define DPORT_PRO_SPI2_DMA_INT_MAP_V  0x1F
#define DPORT_PRO_SPI2_DMA_INT_MAP_S  0

#define DPORT_PRO_SPI3_DMA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1DC)
/* DPORT_PRO_SPI3_DMA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_SPI3_DMA_INT_MAP  0x0000001F
#define DPORT_PRO_SPI3_DMA_INT_MAP_M  ((DPORT_PRO_SPI3_DMA_INT_MAP_V)<<(DPORT_PRO_SPI3_DMA_INT_MAP_S))
#define DPORT_PRO_SPI3_DMA_INT_MAP_V  0x1F
#define DPORT_PRO_SPI3_DMA_INT_MAP_S  0

#define DPORT_PRO_WDG_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1E0)
/* DPORT_PRO_WDG_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_WDG_INT_MAP  0x0000001F
#define DPORT_PRO_WDG_INT_MAP_M  ((DPORT_PRO_WDG_INT_MAP_V)<<(DPORT_PRO_WDG_INT_MAP_S))
#define DPORT_PRO_WDG_INT_MAP_V  0x1F
#define DPORT_PRO_WDG_INT_MAP_S  0

#define DPORT_PRO_TIMER_INT1_MAP_REG          (DR_REG_DPORT_BASE + 0x1E4)
/* DPORT_PRO_TIMER_INT1_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TIMER_INT1_MAP  0x0000001F
#define DPORT_PRO_TIMER_INT1_MAP_M  ((DPORT_PRO_TIMER_INT1_MAP_V)<<(DPORT_PRO_TIMER_INT1_MAP_S))
#define DPORT_PRO_TIMER_INT1_MAP_V  0x1F
#define DPORT_PRO_TIMER_INT1_MAP_S  0

#define DPORT_PRO_TIMER_INT2_MAP_REG          (DR_REG_DPORT_BASE + 0x1E8)
/* DPORT_PRO_TIMER_INT2_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TIMER_INT2_MAP  0x0000001F
#define DPORT_PRO_TIMER_INT2_MAP_M  ((DPORT_PRO_TIMER_INT2_MAP_V)<<(DPORT_PRO_TIMER_INT2_MAP_S))
#define DPORT_PRO_TIMER_INT2_MAP_V  0x1F
#define DPORT_PRO_TIMER_INT2_MAP_S  0

#define DPORT_PRO_TG_T0_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1EC)
/* DPORT_PRO_TG_T0_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_T0_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG_T0_EDGE_INT_MAP_M  ((DPORT_PRO_TG_T0_EDGE_INT_MAP_V)<<(DPORT_PRO_TG_T0_EDGE_INT_MAP_S))
#define DPORT_PRO_TG_T0_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG_T0_EDGE_INT_MAP_S  0

#define DPORT_PRO_TG_T1_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1F0)
/* DPORT_PRO_TG_T1_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_T1_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG_T1_EDGE_INT_MAP_M  ((DPORT_PRO_TG_T1_EDGE_INT_MAP_V)<<(DPORT_PRO_TG_T1_EDGE_INT_MAP_S))
#define DPORT_PRO_TG_T1_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG_T1_EDGE_INT_MAP_S  0

#define DPORT_PRO_TG_WDT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1F4)
/* DPORT_PRO_TG_WDT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_WDT_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG_WDT_EDGE_INT_MAP_M  ((DPORT_PRO_TG_WDT_EDGE_INT_MAP_V)<<(DPORT_PRO_TG_WDT_EDGE_INT_MAP_S))
#define DPORT_PRO_TG_WDT_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG_WDT_EDGE_INT_MAP_S  0

#define DPORT_PRO_TG_LACT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1F8)
/* DPORT_PRO_TG_LACT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG_LACT_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG_LACT_EDGE_INT_MAP_M  ((DPORT_PRO_TG_LACT_EDGE_INT_MAP_V)<<(DPORT_PRO_TG_LACT_EDGE_INT_MAP_S))
#define DPORT_PRO_TG_LACT_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG_LACT_EDGE_INT_MAP_S  0

#define DPORT_PRO_TG1_T0_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x1FC)
/* DPORT_PRO_TG1_T0_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_T0_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_T0_EDGE_INT_MAP_M  ((DPORT_PRO_TG1_T0_EDGE_INT_MAP_V)<<(DPORT_PRO_TG1_T0_EDGE_INT_MAP_S))
#define DPORT_PRO_TG1_T0_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_T0_EDGE_INT_MAP_S  0

#define DPORT_PRO_TG1_T1_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x200)
/* DPORT_PRO_TG1_T1_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_T1_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_T1_EDGE_INT_MAP_M  ((DPORT_PRO_TG1_T1_EDGE_INT_MAP_V)<<(DPORT_PRO_TG1_T1_EDGE_INT_MAP_S))
#define DPORT_PRO_TG1_T1_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_T1_EDGE_INT_MAP_S  0

#define DPORT_PRO_TG1_WDT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x204)
/* DPORT_PRO_TG1_WDT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_WDT_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_WDT_EDGE_INT_MAP_M  ((DPORT_PRO_TG1_WDT_EDGE_INT_MAP_V)<<(DPORT_PRO_TG1_WDT_EDGE_INT_MAP_S))
#define DPORT_PRO_TG1_WDT_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_WDT_EDGE_INT_MAP_S  0

#define DPORT_PRO_TG1_LACT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x208)
/* DPORT_PRO_TG1_LACT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_TG1_LACT_EDGE_INT_MAP  0x0000001F
#define DPORT_PRO_TG1_LACT_EDGE_INT_MAP_M  ((DPORT_PRO_TG1_LACT_EDGE_INT_MAP_V)<<(DPORT_PRO_TG1_LACT_EDGE_INT_MAP_S))
#define DPORT_PRO_TG1_LACT_EDGE_INT_MAP_V  0x1F
#define DPORT_PRO_TG1_LACT_EDGE_INT_MAP_S  0

#define DPORT_PRO_MMU_IA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x20C)
/* DPORT_PRO_MMU_IA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_MMU_IA_INT_MAP  0x0000001F
#define DPORT_PRO_MMU_IA_INT_MAP_M  ((DPORT_PRO_MMU_IA_INT_MAP_V)<<(DPORT_PRO_MMU_IA_INT_MAP_S))
#define DPORT_PRO_MMU_IA_INT_MAP_V  0x1F
#define DPORT_PRO_MMU_IA_INT_MAP_S  0

#define DPORT_PRO_MPU_IA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x210)
/* DPORT_PRO_MPU_IA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_MPU_IA_INT_MAP  0x0000001F
#define DPORT_PRO_MPU_IA_INT_MAP_M  ((DPORT_PRO_MPU_IA_INT_MAP_V)<<(DPORT_PRO_MPU_IA_INT_MAP_S))
#define DPORT_PRO_MPU_IA_INT_MAP_V  0x1F
#define DPORT_PRO_MPU_IA_INT_MAP_S  0

#define DPORT_PRO_CACHE_IA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x214)
/* DPORT_PRO_CACHE_IA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_PRO_CACHE_IA_INT_MAP  0x0000001F
#define DPORT_PRO_CACHE_IA_INT_MAP_M  ((DPORT_PRO_CACHE_IA_INT_MAP_V)<<(DPORT_PRO_CACHE_IA_INT_MAP_S))
#define DPORT_PRO_CACHE_IA_INT_MAP_V  0x1F
#define DPORT_PRO_CACHE_IA_INT_MAP_S  0

#define DPORT_APP_MAC_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x218)
/* DPORT_APP_MAC_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_MAC_INTR_MAP  0x0000001F
#define DPORT_APP_MAC_INTR_MAP_M  ((DPORT_APP_MAC_INTR_MAP_V)<<(DPORT_APP_MAC_INTR_MAP_S))
#define DPORT_APP_MAC_INTR_MAP_V  0x1F
#define DPORT_APP_MAC_INTR_MAP_S  0

#define DPORT_APP_MAC_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x21C)
/* DPORT_APP_MAC_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_MAC_NMI_MAP  0x0000001F
#define DPORT_APP_MAC_NMI_MAP_M  ((DPORT_APP_MAC_NMI_MAP_V)<<(DPORT_APP_MAC_NMI_MAP_S))
#define DPORT_APP_MAC_NMI_MAP_V  0x1F
#define DPORT_APP_MAC_NMI_MAP_S  0

#define DPORT_APP_BB_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x220)
/* DPORT_APP_BB_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_BB_INT_MAP  0x0000001F
#define DPORT_APP_BB_INT_MAP_M  ((DPORT_APP_BB_INT_MAP_V)<<(DPORT_APP_BB_INT_MAP_S))
#define DPORT_APP_BB_INT_MAP_V  0x1F
#define DPORT_APP_BB_INT_MAP_S  0

#define DPORT_APP_BT_MAC_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x224)
/* DPORT_APP_BT_MAC_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_BT_MAC_INT_MAP  0x0000001F
#define DPORT_APP_BT_MAC_INT_MAP_M  ((DPORT_APP_BT_MAC_INT_MAP_V)<<(DPORT_APP_BT_MAC_INT_MAP_S))
#define DPORT_APP_BT_MAC_INT_MAP_V  0x1F
#define DPORT_APP_BT_MAC_INT_MAP_S  0

#define DPORT_APP_BT_BB_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x228)
/* DPORT_APP_BT_BB_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_BT_BB_INT_MAP  0x0000001F
#define DPORT_APP_BT_BB_INT_MAP_M  ((DPORT_APP_BT_BB_INT_MAP_V)<<(DPORT_APP_BT_BB_INT_MAP_S))
#define DPORT_APP_BT_BB_INT_MAP_V  0x1F
#define DPORT_APP_BT_BB_INT_MAP_S  0

#define DPORT_APP_BT_BB_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x22C)
/* DPORT_APP_BT_BB_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_BT_BB_NMI_MAP  0x0000001F
#define DPORT_APP_BT_BB_NMI_MAP_M  ((DPORT_APP_BT_BB_NMI_MAP_V)<<(DPORT_APP_BT_BB_NMI_MAP_S))
#define DPORT_APP_BT_BB_NMI_MAP_V  0x1F
#define DPORT_APP_BT_BB_NMI_MAP_S  0

#define DPORT_APP_RWBT_IRQ_MAP_REG          (DR_REG_DPORT_BASE + 0x230)
/* DPORT_APP_RWBT_IRQ_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_RWBT_IRQ_MAP  0x0000001F
#define DPORT_APP_RWBT_IRQ_MAP_M  ((DPORT_APP_RWBT_IRQ_MAP_V)<<(DPORT_APP_RWBT_IRQ_MAP_S))
#define DPORT_APP_RWBT_IRQ_MAP_V  0x1F
#define DPORT_APP_RWBT_IRQ_MAP_S  0

#define DPORT_APP_RWBLE_IRQ_MAP_REG          (DR_REG_DPORT_BASE + 0x234)
/* DPORT_APP_RWBLE_IRQ_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_RWBLE_IRQ_MAP  0x0000001F
#define DPORT_APP_RWBLE_IRQ_MAP_M  ((DPORT_APP_RWBLE_IRQ_MAP_V)<<(DPORT_APP_RWBLE_IRQ_MAP_S))
#define DPORT_APP_RWBLE_IRQ_MAP_V  0x1F
#define DPORT_APP_RWBLE_IRQ_MAP_S  0

#define DPORT_APP_RWBT_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x238)
/* DPORT_APP_RWBT_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_RWBT_NMI_MAP  0x0000001F
#define DPORT_APP_RWBT_NMI_MAP_M  ((DPORT_APP_RWBT_NMI_MAP_V)<<(DPORT_APP_RWBT_NMI_MAP_S))
#define DPORT_APP_RWBT_NMI_MAP_V  0x1F
#define DPORT_APP_RWBT_NMI_MAP_S  0

#define DPORT_APP_RWBLE_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x23C)
/* DPORT_APP_RWBLE_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_RWBLE_NMI_MAP  0x0000001F
#define DPORT_APP_RWBLE_NMI_MAP_M  ((DPORT_APP_RWBLE_NMI_MAP_V)<<(DPORT_APP_RWBLE_NMI_MAP_S))
#define DPORT_APP_RWBLE_NMI_MAP_V  0x1F
#define DPORT_APP_RWBLE_NMI_MAP_S  0

#define DPORT_APP_SLC0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x240)
/* DPORT_APP_SLC0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SLC0_INTR_MAP  0x0000001F
#define DPORT_APP_SLC0_INTR_MAP_M  ((DPORT_APP_SLC0_INTR_MAP_V)<<(DPORT_APP_SLC0_INTR_MAP_S))
#define DPORT_APP_SLC0_INTR_MAP_V  0x1F
#define DPORT_APP_SLC0_INTR_MAP_S  0

#define DPORT_APP_SLC1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x244)
/* DPORT_APP_SLC1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SLC1_INTR_MAP  0x0000001F
#define DPORT_APP_SLC1_INTR_MAP_M  ((DPORT_APP_SLC1_INTR_MAP_V)<<(DPORT_APP_SLC1_INTR_MAP_S))
#define DPORT_APP_SLC1_INTR_MAP_V  0x1F
#define DPORT_APP_SLC1_INTR_MAP_S  0

#define DPORT_APP_UHCI0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x248)
/* DPORT_APP_UHCI0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_UHCI0_INTR_MAP  0x0000001F
#define DPORT_APP_UHCI0_INTR_MAP_M  ((DPORT_APP_UHCI0_INTR_MAP_V)<<(DPORT_APP_UHCI0_INTR_MAP_S))
#define DPORT_APP_UHCI0_INTR_MAP_V  0x1F
#define DPORT_APP_UHCI0_INTR_MAP_S  0

#define DPORT_APP_UHCI1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x24C)
/* DPORT_APP_UHCI1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_UHCI1_INTR_MAP  0x0000001F
#define DPORT_APP_UHCI1_INTR_MAP_M  ((DPORT_APP_UHCI1_INTR_MAP_V)<<(DPORT_APP_UHCI1_INTR_MAP_S))
#define DPORT_APP_UHCI1_INTR_MAP_V  0x1F
#define DPORT_APP_UHCI1_INTR_MAP_S  0

#define DPORT_APP_TG_T0_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x250)
/* DPORT_APP_TG_T0_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_T0_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG_T0_LEVEL_INT_MAP_M  ((DPORT_APP_TG_T0_LEVEL_INT_MAP_V)<<(DPORT_APP_TG_T0_LEVEL_INT_MAP_S))
#define DPORT_APP_TG_T0_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG_T0_LEVEL_INT_MAP_S  0

#define DPORT_APP_TG_T1_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x254)
/* DPORT_APP_TG_T1_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_T1_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG_T1_LEVEL_INT_MAP_M  ((DPORT_APP_TG_T1_LEVEL_INT_MAP_V)<<(DPORT_APP_TG_T1_LEVEL_INT_MAP_S))
#define DPORT_APP_TG_T1_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG_T1_LEVEL_INT_MAP_S  0

#define DPORT_APP_TG_WDT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x258)
/* DPORT_APP_TG_WDT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_WDT_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG_WDT_LEVEL_INT_MAP_M  ((DPORT_APP_TG_WDT_LEVEL_INT_MAP_V)<<(DPORT_APP_TG_WDT_LEVEL_INT_MAP_S))
#define DPORT_APP_TG_WDT_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG_WDT_LEVEL_INT_MAP_S  0

#define DPORT_APP_TG_LACT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x25C)
/* DPORT_APP_TG_LACT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_LACT_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG_LACT_LEVEL_INT_MAP_M  ((DPORT_APP_TG_LACT_LEVEL_INT_MAP_V)<<(DPORT_APP_TG_LACT_LEVEL_INT_MAP_S))
#define DPORT_APP_TG_LACT_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG_LACT_LEVEL_INT_MAP_S  0

#define DPORT_APP_TG1_T0_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x260)
/* DPORT_APP_TG1_T0_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_T0_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG1_T0_LEVEL_INT_MAP_M  ((DPORT_APP_TG1_T0_LEVEL_INT_MAP_V)<<(DPORT_APP_TG1_T0_LEVEL_INT_MAP_S))
#define DPORT_APP_TG1_T0_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG1_T0_LEVEL_INT_MAP_S  0

#define DPORT_APP_TG1_T1_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x264)
/* DPORT_APP_TG1_T1_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_T1_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG1_T1_LEVEL_INT_MAP_M  ((DPORT_APP_TG1_T1_LEVEL_INT_MAP_V)<<(DPORT_APP_TG1_T1_LEVEL_INT_MAP_S))
#define DPORT_APP_TG1_T1_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG1_T1_LEVEL_INT_MAP_S  0

#define DPORT_APP_TG1_WDT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x268)
/* DPORT_APP_TG1_WDT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_WDT_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG1_WDT_LEVEL_INT_MAP_M  ((DPORT_APP_TG1_WDT_LEVEL_INT_MAP_V)<<(DPORT_APP_TG1_WDT_LEVEL_INT_MAP_S))
#define DPORT_APP_TG1_WDT_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG1_WDT_LEVEL_INT_MAP_S  0

#define DPORT_APP_TG1_LACT_LEVEL_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x26C)
/* DPORT_APP_TG1_LACT_LEVEL_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_LACT_LEVEL_INT_MAP  0x0000001F
#define DPORT_APP_TG1_LACT_LEVEL_INT_MAP_M  ((DPORT_APP_TG1_LACT_LEVEL_INT_MAP_V)<<(DPORT_APP_TG1_LACT_LEVEL_INT_MAP_S))
#define DPORT_APP_TG1_LACT_LEVEL_INT_MAP_V  0x1F
#define DPORT_APP_TG1_LACT_LEVEL_INT_MAP_S  0

#define DPORT_APP_GPIO_INTERRUPT_MAP_REG          (DR_REG_DPORT_BASE + 0x270)
/* DPORT_APP_GPIO_INTERRUPT_APP_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_GPIO_INTERRUPT_APP_MAP  0x0000001F
#define DPORT_APP_GPIO_INTERRUPT_APP_MAP_M  ((DPORT_APP_GPIO_INTERRUPT_APP_MAP_V)<<(DPORT_APP_GPIO_INTERRUPT_APP_MAP_S))
#define DPORT_APP_GPIO_INTERRUPT_APP_MAP_V  0x1F
#define DPORT_APP_GPIO_INTERRUPT_APP_MAP_S  0

#define DPORT_APP_GPIO_INTERRUPT_NMI_MAP_REG          (DR_REG_DPORT_BASE + 0x274)
/* DPORT_APP_GPIO_INTERRUPT_APP_NMI_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_GPIO_INTERRUPT_APP_NMI_MAP  0x0000001F
#define DPORT_APP_GPIO_INTERRUPT_APP_NMI_MAP_M  ((DPORT_APP_GPIO_INTERRUPT_APP_NMI_MAP_V)<<(DPORT_APP_GPIO_INTERRUPT_APP_NMI_MAP_S))
#define DPORT_APP_GPIO_INTERRUPT_APP_NMI_MAP_V  0x1F
#define DPORT_APP_GPIO_INTERRUPT_APP_NMI_MAP_S  0

#define DPORT_APP_CPU_INTR_FROM_CPU_0_MAP_REG          (DR_REG_DPORT_BASE + 0x278)
/* DPORT_APP_CPU_INTR_FROM_CPU_0_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_CPU_INTR_FROM_CPU_0_MAP  0x0000001F
#define DPORT_APP_CPU_INTR_FROM_CPU_0_MAP_M  ((DPORT_APP_CPU_INTR_FROM_CPU_0_MAP_V)<<(DPORT_APP_CPU_INTR_FROM_CPU_0_MAP_S))
#define DPORT_APP_CPU_INTR_FROM_CPU_0_MAP_V  0x1F
#define DPORT_APP_CPU_INTR_FROM_CPU_0_MAP_S  0

#define DPORT_APP_CPU_INTR_FROM_CPU_1_MAP_REG          (DR_REG_DPORT_BASE + 0x27C)
/* DPORT_APP_CPU_INTR_FROM_CPU_1_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_CPU_INTR_FROM_CPU_1_MAP  0x0000001F
#define DPORT_APP_CPU_INTR_FROM_CPU_1_MAP_M  ((DPORT_APP_CPU_INTR_FROM_CPU_1_MAP_V)<<(DPORT_APP_CPU_INTR_FROM_CPU_1_MAP_S))
#define DPORT_APP_CPU_INTR_FROM_CPU_1_MAP_V  0x1F
#define DPORT_APP_CPU_INTR_FROM_CPU_1_MAP_S  0

#define DPORT_APP_CPU_INTR_FROM_CPU_2_MAP_REG          (DR_REG_DPORT_BASE + 0x280)
/* DPORT_APP_CPU_INTR_FROM_CPU_2_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_CPU_INTR_FROM_CPU_2_MAP  0x0000001F
#define DPORT_APP_CPU_INTR_FROM_CPU_2_MAP_M  ((DPORT_APP_CPU_INTR_FROM_CPU_2_MAP_V)<<(DPORT_APP_CPU_INTR_FROM_CPU_2_MAP_S))
#define DPORT_APP_CPU_INTR_FROM_CPU_2_MAP_V  0x1F
#define DPORT_APP_CPU_INTR_FROM_CPU_2_MAP_S  0

#define DPORT_APP_CPU_INTR_FROM_CPU_3_MAP_REG          (DR_REG_DPORT_BASE + 0x284)
/* DPORT_APP_CPU_INTR_FROM_CPU_3_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_CPU_INTR_FROM_CPU_3_MAP  0x0000001F
#define DPORT_APP_CPU_INTR_FROM_CPU_3_MAP_M  ((DPORT_APP_CPU_INTR_FROM_CPU_3_MAP_V)<<(DPORT_APP_CPU_INTR_FROM_CPU_3_MAP_S))
#define DPORT_APP_CPU_INTR_FROM_CPU_3_MAP_V  0x1F
#define DPORT_APP_CPU_INTR_FROM_CPU_3_MAP_S  0

#define DPORT_APP_SPI_INTR_0_MAP_REG          (DR_REG_DPORT_BASE + 0x288)
/* DPORT_APP_SPI_INTR_0_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SPI_INTR_0_MAP  0x0000001F
#define DPORT_APP_SPI_INTR_0_MAP_M  ((DPORT_APP_SPI_INTR_0_MAP_V)<<(DPORT_APP_SPI_INTR_0_MAP_S))
#define DPORT_APP_SPI_INTR_0_MAP_V  0x1F
#define DPORT_APP_SPI_INTR_0_MAP_S  0

#define DPORT_APP_SPI_INTR_1_MAP_REG          (DR_REG_DPORT_BASE + 0x28C)
/* DPORT_APP_SPI_INTR_1_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SPI_INTR_1_MAP  0x0000001F
#define DPORT_APP_SPI_INTR_1_MAP_M  ((DPORT_APP_SPI_INTR_1_MAP_V)<<(DPORT_APP_SPI_INTR_1_MAP_S))
#define DPORT_APP_SPI_INTR_1_MAP_V  0x1F
#define DPORT_APP_SPI_INTR_1_MAP_S  0

#define DPORT_APP_SPI_INTR_2_MAP_REG          (DR_REG_DPORT_BASE + 0x290)
/* DPORT_APP_SPI_INTR_2_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SPI_INTR_2_MAP  0x0000001F
#define DPORT_APP_SPI_INTR_2_MAP_M  ((DPORT_APP_SPI_INTR_2_MAP_V)<<(DPORT_APP_SPI_INTR_2_MAP_S))
#define DPORT_APP_SPI_INTR_2_MAP_V  0x1F
#define DPORT_APP_SPI_INTR_2_MAP_S  0

#define DPORT_APP_SPI_INTR_3_MAP_REG          (DR_REG_DPORT_BASE + 0x294)
/* DPORT_APP_SPI_INTR_3_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SPI_INTR_3_MAP  0x0000001F
#define DPORT_APP_SPI_INTR_3_MAP_M  ((DPORT_APP_SPI_INTR_3_MAP_V)<<(DPORT_APP_SPI_INTR_3_MAP_S))
#define DPORT_APP_SPI_INTR_3_MAP_V  0x1F
#define DPORT_APP_SPI_INTR_3_MAP_S  0

#define DPORT_APP_I2S0_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x298)
/* DPORT_APP_I2S0_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_I2S0_INT_MAP  0x0000001F
#define DPORT_APP_I2S0_INT_MAP_M  ((DPORT_APP_I2S0_INT_MAP_V)<<(DPORT_APP_I2S0_INT_MAP_S))
#define DPORT_APP_I2S0_INT_MAP_V  0x1F
#define DPORT_APP_I2S0_INT_MAP_S  0

#define DPORT_APP_I2S1_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x29C)
/* DPORT_APP_I2S1_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_I2S1_INT_MAP  0x0000001F
#define DPORT_APP_I2S1_INT_MAP_M  ((DPORT_APP_I2S1_INT_MAP_V)<<(DPORT_APP_I2S1_INT_MAP_S))
#define DPORT_APP_I2S1_INT_MAP_V  0x1F
#define DPORT_APP_I2S1_INT_MAP_S  0

#define DPORT_APP_UART_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2A0)
/* DPORT_APP_UART_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_UART_INTR_MAP  0x0000001F
#define DPORT_APP_UART_INTR_MAP_M  ((DPORT_APP_UART_INTR_MAP_V)<<(DPORT_APP_UART_INTR_MAP_S))
#define DPORT_APP_UART_INTR_MAP_V  0x1F
#define DPORT_APP_UART_INTR_MAP_S  0

#define DPORT_APP_UART1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2A4)
/* DPORT_APP_UART1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_UART1_INTR_MAP  0x0000001F
#define DPORT_APP_UART1_INTR_MAP_M  ((DPORT_APP_UART1_INTR_MAP_V)<<(DPORT_APP_UART1_INTR_MAP_S))
#define DPORT_APP_UART1_INTR_MAP_V  0x1F
#define DPORT_APP_UART1_INTR_MAP_S  0

#define DPORT_APP_UART2_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2A8)
/* DPORT_APP_UART2_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_UART2_INTR_MAP  0x0000001F
#define DPORT_APP_UART2_INTR_MAP_M  ((DPORT_APP_UART2_INTR_MAP_V)<<(DPORT_APP_UART2_INTR_MAP_S))
#define DPORT_APP_UART2_INTR_MAP_V  0x1F
#define DPORT_APP_UART2_INTR_MAP_S  0

#define DPORT_APP_SDIO_HOST_INTERRUPT_MAP_REG          (DR_REG_DPORT_BASE + 0x2AC)
/* DPORT_APP_SDIO_HOST_INTERRUPT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SDIO_HOST_INTERRUPT_MAP  0x0000001F
#define DPORT_APP_SDIO_HOST_INTERRUPT_MAP_M  ((DPORT_APP_SDIO_HOST_INTERRUPT_MAP_V)<<(DPORT_APP_SDIO_HOST_INTERRUPT_MAP_S))
#define DPORT_APP_SDIO_HOST_INTERRUPT_MAP_V  0x1F
#define DPORT_APP_SDIO_HOST_INTERRUPT_MAP_S  0

#define DPORT_APP_EMAC_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2B0)
/* DPORT_APP_EMAC_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_EMAC_INT_MAP  0x0000001F
#define DPORT_APP_EMAC_INT_MAP_M  ((DPORT_APP_EMAC_INT_MAP_V)<<(DPORT_APP_EMAC_INT_MAP_S))
#define DPORT_APP_EMAC_INT_MAP_V  0x1F
#define DPORT_APP_EMAC_INT_MAP_S  0

#define DPORT_APP_PWM0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2B4)
/* DPORT_APP_PWM0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_PWM0_INTR_MAP  0x0000001F
#define DPORT_APP_PWM0_INTR_MAP_M  ((DPORT_APP_PWM0_INTR_MAP_V)<<(DPORT_APP_PWM0_INTR_MAP_S))
#define DPORT_APP_PWM0_INTR_MAP_V  0x1F
#define DPORT_APP_PWM0_INTR_MAP_S  0

#define DPORT_APP_PWM1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2B8)
/* DPORT_APP_PWM1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_PWM1_INTR_MAP  0x0000001F
#define DPORT_APP_PWM1_INTR_MAP_M  ((DPORT_APP_PWM1_INTR_MAP_V)<<(DPORT_APP_PWM1_INTR_MAP_S))
#define DPORT_APP_PWM1_INTR_MAP_V  0x1F
#define DPORT_APP_PWM1_INTR_MAP_S  0

#define DPORT_APP_PWM2_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2BC)
/* DPORT_APP_PWM2_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_PWM2_INTR_MAP  0x0000001F
#define DPORT_APP_PWM2_INTR_MAP_M  ((DPORT_APP_PWM2_INTR_MAP_V)<<(DPORT_APP_PWM2_INTR_MAP_S))
#define DPORT_APP_PWM2_INTR_MAP_V  0x1F
#define DPORT_APP_PWM2_INTR_MAP_S  0

#define DPORT_APP_PWM3_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2C0)
/* DPORT_APP_PWM3_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_PWM3_INTR_MAP  0x0000001F
#define DPORT_APP_PWM3_INTR_MAP_M  ((DPORT_APP_PWM3_INTR_MAP_V)<<(DPORT_APP_PWM3_INTR_MAP_S))
#define DPORT_APP_PWM3_INTR_MAP_V  0x1F
#define DPORT_APP_PWM3_INTR_MAP_S  0

#define DPORT_APP_LEDC_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2C4)
/* DPORT_APP_LEDC_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_LEDC_INT_MAP  0x0000001F
#define DPORT_APP_LEDC_INT_MAP_M  ((DPORT_APP_LEDC_INT_MAP_V)<<(DPORT_APP_LEDC_INT_MAP_S))
#define DPORT_APP_LEDC_INT_MAP_V  0x1F
#define DPORT_APP_LEDC_INT_MAP_S  0

#define DPORT_APP_EFUSE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2C8)
/* DPORT_APP_EFUSE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_EFUSE_INT_MAP  0x0000001F
#define DPORT_APP_EFUSE_INT_MAP_M  ((DPORT_APP_EFUSE_INT_MAP_V)<<(DPORT_APP_EFUSE_INT_MAP_S))
#define DPORT_APP_EFUSE_INT_MAP_V  0x1F
#define DPORT_APP_EFUSE_INT_MAP_S  0

#define DPORT_APP_CAN_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2CC)
/* DPORT_APP_CAN_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_CAN_INT_MAP  0x0000001F
#define DPORT_APP_CAN_INT_MAP_M  ((DPORT_APP_CAN_INT_MAP_V)<<(DPORT_APP_CAN_INT_MAP_S))
#define DPORT_APP_CAN_INT_MAP_V  0x1F
#define DPORT_APP_CAN_INT_MAP_S  0

#define DPORT_APP_RTC_CORE_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2D0)
/* DPORT_APP_RTC_CORE_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_RTC_CORE_INTR_MAP  0x0000001F
#define DPORT_APP_RTC_CORE_INTR_MAP_M  ((DPORT_APP_RTC_CORE_INTR_MAP_V)<<(DPORT_APP_RTC_CORE_INTR_MAP_S))
#define DPORT_APP_RTC_CORE_INTR_MAP_V  0x1F
#define DPORT_APP_RTC_CORE_INTR_MAP_S  0

#define DPORT_APP_RMT_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2D4)
/* DPORT_APP_RMT_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_RMT_INTR_MAP  0x0000001F
#define DPORT_APP_RMT_INTR_MAP_M  ((DPORT_APP_RMT_INTR_MAP_V)<<(DPORT_APP_RMT_INTR_MAP_S))
#define DPORT_APP_RMT_INTR_MAP_V  0x1F
#define DPORT_APP_RMT_INTR_MAP_S  0

#define DPORT_APP_PCNT_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2D8)
/* DPORT_APP_PCNT_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_PCNT_INTR_MAP  0x0000001F
#define DPORT_APP_PCNT_INTR_MAP_M  ((DPORT_APP_PCNT_INTR_MAP_V)<<(DPORT_APP_PCNT_INTR_MAP_S))
#define DPORT_APP_PCNT_INTR_MAP_V  0x1F
#define DPORT_APP_PCNT_INTR_MAP_S  0

#define DPORT_APP_I2C_EXT0_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2DC)
/* DPORT_APP_I2C_EXT0_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_I2C_EXT0_INTR_MAP  0x0000001F
#define DPORT_APP_I2C_EXT0_INTR_MAP_M  ((DPORT_APP_I2C_EXT0_INTR_MAP_V)<<(DPORT_APP_I2C_EXT0_INTR_MAP_S))
#define DPORT_APP_I2C_EXT0_INTR_MAP_V  0x1F
#define DPORT_APP_I2C_EXT0_INTR_MAP_S  0

#define DPORT_APP_I2C_EXT1_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2E0)
/* DPORT_APP_I2C_EXT1_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_I2C_EXT1_INTR_MAP  0x0000001F
#define DPORT_APP_I2C_EXT1_INTR_MAP_M  ((DPORT_APP_I2C_EXT1_INTR_MAP_V)<<(DPORT_APP_I2C_EXT1_INTR_MAP_S))
#define DPORT_APP_I2C_EXT1_INTR_MAP_V  0x1F
#define DPORT_APP_I2C_EXT1_INTR_MAP_S  0

#define DPORT_APP_RSA_INTR_MAP_REG          (DR_REG_DPORT_BASE + 0x2E4)
/* DPORT_APP_RSA_INTR_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_RSA_INTR_MAP  0x0000001F
#define DPORT_APP_RSA_INTR_MAP_M  ((DPORT_APP_RSA_INTR_MAP_V)<<(DPORT_APP_RSA_INTR_MAP_S))
#define DPORT_APP_RSA_INTR_MAP_V  0x1F
#define DPORT_APP_RSA_INTR_MAP_S  0

#define DPORT_APP_SPI1_DMA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2E8)
/* DPORT_APP_SPI1_DMA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SPI1_DMA_INT_MAP  0x0000001F
#define DPORT_APP_SPI1_DMA_INT_MAP_M  ((DPORT_APP_SPI1_DMA_INT_MAP_V)<<(DPORT_APP_SPI1_DMA_INT_MAP_S))
#define DPORT_APP_SPI1_DMA_INT_MAP_V  0x1F
#define DPORT_APP_SPI1_DMA_INT_MAP_S  0

#define DPORT_APP_SPI2_DMA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2EC)
/* DPORT_APP_SPI2_DMA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SPI2_DMA_INT_MAP  0x0000001F
#define DPORT_APP_SPI2_DMA_INT_MAP_M  ((DPORT_APP_SPI2_DMA_INT_MAP_V)<<(DPORT_APP_SPI2_DMA_INT_MAP_S))
#define DPORT_APP_SPI2_DMA_INT_MAP_V  0x1F
#define DPORT_APP_SPI2_DMA_INT_MAP_S  0

#define DPORT_APP_SPI3_DMA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2F0)
/* DPORT_APP_SPI3_DMA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_SPI3_DMA_INT_MAP  0x0000001F
#define DPORT_APP_SPI3_DMA_INT_MAP_M  ((DPORT_APP_SPI3_DMA_INT_MAP_V)<<(DPORT_APP_SPI3_DMA_INT_MAP_S))
#define DPORT_APP_SPI3_DMA_INT_MAP_V  0x1F
#define DPORT_APP_SPI3_DMA_INT_MAP_S  0

#define DPORT_APP_WDG_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x2F4)
/* DPORT_APP_WDG_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_WDG_INT_MAP  0x0000001F
#define DPORT_APP_WDG_INT_MAP_M  ((DPORT_APP_WDG_INT_MAP_V)<<(DPORT_APP_WDG_INT_MAP_S))
#define DPORT_APP_WDG_INT_MAP_V  0x1F
#define DPORT_APP_WDG_INT_MAP_S  0

#define DPORT_APP_TIMER_INT1_MAP_REG          (DR_REG_DPORT_BASE + 0x2F8)
/* DPORT_APP_TIMER_INT1_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TIMER_INT1_MAP  0x0000001F
#define DPORT_APP_TIMER_INT1_MAP_M  ((DPORT_APP_TIMER_INT1_MAP_V)<<(DPORT_APP_TIMER_INT1_MAP_S))
#define DPORT_APP_TIMER_INT1_MAP_V  0x1F
#define DPORT_APP_TIMER_INT1_MAP_S  0

#define DPORT_APP_TIMER_INT2_MAP_REG          (DR_REG_DPORT_BASE + 0x2FC)
/* DPORT_APP_TIMER_INT2_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TIMER_INT2_MAP  0x0000001F
#define DPORT_APP_TIMER_INT2_MAP_M  ((DPORT_APP_TIMER_INT2_MAP_V)<<(DPORT_APP_TIMER_INT2_MAP_S))
#define DPORT_APP_TIMER_INT2_MAP_V  0x1F
#define DPORT_APP_TIMER_INT2_MAP_S  0

#define DPORT_APP_TG_T0_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x300)
/* DPORT_APP_TG_T0_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_T0_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG_T0_EDGE_INT_MAP_M  ((DPORT_APP_TG_T0_EDGE_INT_MAP_V)<<(DPORT_APP_TG_T0_EDGE_INT_MAP_S))
#define DPORT_APP_TG_T0_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG_T0_EDGE_INT_MAP_S  0

#define DPORT_APP_TG_T1_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x304)
/* DPORT_APP_TG_T1_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_T1_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG_T1_EDGE_INT_MAP_M  ((DPORT_APP_TG_T1_EDGE_INT_MAP_V)<<(DPORT_APP_TG_T1_EDGE_INT_MAP_S))
#define DPORT_APP_TG_T1_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG_T1_EDGE_INT_MAP_S  0

#define DPORT_APP_TG_WDT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x308)
/* DPORT_APP_TG_WDT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_WDT_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG_WDT_EDGE_INT_MAP_M  ((DPORT_APP_TG_WDT_EDGE_INT_MAP_V)<<(DPORT_APP_TG_WDT_EDGE_INT_MAP_S))
#define DPORT_APP_TG_WDT_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG_WDT_EDGE_INT_MAP_S  0

#define DPORT_APP_TG_LACT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x30C)
/* DPORT_APP_TG_LACT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG_LACT_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG_LACT_EDGE_INT_MAP_M  ((DPORT_APP_TG_LACT_EDGE_INT_MAP_V)<<(DPORT_APP_TG_LACT_EDGE_INT_MAP_S))
#define DPORT_APP_TG_LACT_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG_LACT_EDGE_INT_MAP_S  0

#define DPORT_APP_TG1_T0_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x310)
/* DPORT_APP_TG1_T0_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_T0_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG1_T0_EDGE_INT_MAP_M  ((DPORT_APP_TG1_T0_EDGE_INT_MAP_V)<<(DPORT_APP_TG1_T0_EDGE_INT_MAP_S))
#define DPORT_APP_TG1_T0_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG1_T0_EDGE_INT_MAP_S  0

#define DPORT_APP_TG1_T1_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x314)
/* DPORT_APP_TG1_T1_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_T1_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG1_T1_EDGE_INT_MAP_M  ((DPORT_APP_TG1_T1_EDGE_INT_MAP_V)<<(DPORT_APP_TG1_T1_EDGE_INT_MAP_S))
#define DPORT_APP_TG1_T1_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG1_T1_EDGE_INT_MAP_S  0

#define DPORT_APP_TG1_WDT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x318)
/* DPORT_APP_TG1_WDT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_WDT_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG1_WDT_EDGE_INT_MAP_M  ((DPORT_APP_TG1_WDT_EDGE_INT_MAP_V)<<(DPORT_APP_TG1_WDT_EDGE_INT_MAP_S))
#define DPORT_APP_TG1_WDT_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG1_WDT_EDGE_INT_MAP_S  0

#define DPORT_APP_TG1_LACT_EDGE_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x31C)
/* DPORT_APP_TG1_LACT_EDGE_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_TG1_LACT_EDGE_INT_MAP  0x0000001F
#define DPORT_APP_TG1_LACT_EDGE_INT_MAP_M  ((DPORT_APP_TG1_LACT_EDGE_INT_MAP_V)<<(DPORT_APP_TG1_LACT_EDGE_INT_MAP_S))
#define DPORT_APP_TG1_LACT_EDGE_INT_MAP_V  0x1F
#define DPORT_APP_TG1_LACT_EDGE_INT_MAP_S  0

#define DPORT_APP_MMU_IA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x320)
/* DPORT_APP_MMU_IA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_MMU_IA_INT_MAP  0x0000001F
#define DPORT_APP_MMU_IA_INT_MAP_M  ((DPORT_APP_MMU_IA_INT_MAP_V)<<(DPORT_APP_MMU_IA_INT_MAP_S))
#define DPORT_APP_MMU_IA_INT_MAP_V  0x1F
#define DPORT_APP_MMU_IA_INT_MAP_S  0

#define DPORT_APP_MPU_IA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x324)
/* DPORT_APP_MPU_IA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_MPU_IA_INT_MAP  0x0000001F
#define DPORT_APP_MPU_IA_INT_MAP_M  ((DPORT_APP_MPU_IA_INT_MAP_V)<<(DPORT_APP_MPU_IA_INT_MAP_S))
#define DPORT_APP_MPU_IA_INT_MAP_V  0x1F
#define DPORT_APP_MPU_IA_INT_MAP_S  0

#define DPORT_APP_CACHE_IA_INT_MAP_REG          (DR_REG_DPORT_BASE + 0x328)
/* DPORT_APP_CACHE_IA_INT_MAP : R/W ;bitpos:[4:0] ;default: 5'd16 ; */
/*description: */
#define DPORT_APP_CACHE_IA_INT_MAP  0x0000001F
#define DPORT_APP_CACHE_IA_INT_MAP_M  ((DPORT_APP_CACHE_IA_INT_MAP_V)<<(DPORT_APP_CACHE_IA_INT_MAP_S))
#define DPORT_APP_CACHE_IA_INT_MAP_V  0x1F
#define DPORT_APP_CACHE_IA_INT_MAP_S  0

#define DPORT_AHBLITE_MPU_TABLE_UART_REG          (DR_REG_DPORT_BASE + 0x32C)
/* DPORT_UART_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_UART_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_UART_ACCESS_GRANT_CONFIG_M  ((DPORT_UART_ACCESS_GRANT_CONFIG_V)<<(DPORT_UART_ACCESS_GRANT_CONFIG_S))
#define DPORT_UART_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_UART_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SPI1_REG          (DR_REG_DPORT_BASE + 0x330)
/* DPORT_SPI1_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SPI1_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SPI1_ACCESS_GRANT_CONFIG_M  ((DPORT_SPI1_ACCESS_GRANT_CONFIG_V)<<(DPORT_SPI1_ACCESS_GRANT_CONFIG_S))
#define DPORT_SPI1_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SPI1_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SPI0_REG          (DR_REG_DPORT_BASE + 0x334)
/* DPORT_SPI0_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SPI0_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SPI0_ACCESS_GRANT_CONFIG_M  ((DPORT_SPI0_ACCESS_GRANT_CONFIG_V)<<(DPORT_SPI0_ACCESS_GRANT_CONFIG_S))
#define DPORT_SPI0_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SPI0_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_GPIO_REG          (DR_REG_DPORT_BASE + 0x338)
/* DPORT_GPIO_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_GPIO_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_GPIO_ACCESS_GRANT_CONFIG_M  ((DPORT_GPIO_ACCESS_GRANT_CONFIG_V)<<(DPORT_GPIO_ACCESS_GRANT_CONFIG_S))
#define DPORT_GPIO_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_GPIO_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_FE2_REG          (DR_REG_DPORT_BASE + 0x33C)
/* DPORT_FE2_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_FE2_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_FE2_ACCESS_GRANT_CONFIG_M  ((DPORT_FE2_ACCESS_GRANT_CONFIG_V)<<(DPORT_FE2_ACCESS_GRANT_CONFIG_S))
#define DPORT_FE2_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_FE2_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_FE_REG          (DR_REG_DPORT_BASE + 0x340)
/* DPORT_FE_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_FE_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_FE_ACCESS_GRANT_CONFIG_M  ((DPORT_FE_ACCESS_GRANT_CONFIG_V)<<(DPORT_FE_ACCESS_GRANT_CONFIG_S))
#define DPORT_FE_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_FE_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_TIMER_REG          (DR_REG_DPORT_BASE + 0x344)
/* DPORT_TIMER_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_TIMER_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_TIMER_ACCESS_GRANT_CONFIG_M  ((DPORT_TIMER_ACCESS_GRANT_CONFIG_V)<<(DPORT_TIMER_ACCESS_GRANT_CONFIG_S))
#define DPORT_TIMER_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_TIMER_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_RTC_REG          (DR_REG_DPORT_BASE + 0x348)
/* DPORT_RTC_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_RTC_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_RTC_ACCESS_GRANT_CONFIG_M  ((DPORT_RTC_ACCESS_GRANT_CONFIG_V)<<(DPORT_RTC_ACCESS_GRANT_CONFIG_S))
#define DPORT_RTC_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_RTC_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_IO_MUX_REG          (DR_REG_DPORT_BASE + 0x34C)
/* DPORT_IOMUX_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_IOMUX_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_IOMUX_ACCESS_GRANT_CONFIG_M  ((DPORT_IOMUX_ACCESS_GRANT_CONFIG_V)<<(DPORT_IOMUX_ACCESS_GRANT_CONFIG_S))
#define DPORT_IOMUX_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_IOMUX_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_WDG_REG          (DR_REG_DPORT_BASE + 0x350)
/* DPORT_WDG_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_WDG_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_WDG_ACCESS_GRANT_CONFIG_M  ((DPORT_WDG_ACCESS_GRANT_CONFIG_V)<<(DPORT_WDG_ACCESS_GRANT_CONFIG_S))
#define DPORT_WDG_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_WDG_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_HINF_REG          (DR_REG_DPORT_BASE + 0x354)
/* DPORT_HINF_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_HINF_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_HINF_ACCESS_GRANT_CONFIG_M  ((DPORT_HINF_ACCESS_GRANT_CONFIG_V)<<(DPORT_HINF_ACCESS_GRANT_CONFIG_S))
#define DPORT_HINF_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_HINF_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_UHCI1_REG          (DR_REG_DPORT_BASE + 0x358)
/* DPORT_UHCI1_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_UHCI1_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_UHCI1_ACCESS_GRANT_CONFIG_M  ((DPORT_UHCI1_ACCESS_GRANT_CONFIG_V)<<(DPORT_UHCI1_ACCESS_GRANT_CONFIG_S))
#define DPORT_UHCI1_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_UHCI1_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_MISC_REG          (DR_REG_DPORT_BASE + 0x35C)
/* DPORT_MISC_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_MISC_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_MISC_ACCESS_GRANT_CONFIG_M  ((DPORT_MISC_ACCESS_GRANT_CONFIG_V)<<(DPORT_MISC_ACCESS_GRANT_CONFIG_S))
#define DPORT_MISC_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_MISC_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_I2C_REG          (DR_REG_DPORT_BASE + 0x360)
/* DPORT_I2C_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_I2C_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_I2C_ACCESS_GRANT_CONFIG_M  ((DPORT_I2C_ACCESS_GRANT_CONFIG_V)<<(DPORT_I2C_ACCESS_GRANT_CONFIG_S))
#define DPORT_I2C_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_I2C_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_I2S0_REG          (DR_REG_DPORT_BASE + 0x364)
/* DPORT_I2S0_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_I2S0_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_I2S0_ACCESS_GRANT_CONFIG_M  ((DPORT_I2S0_ACCESS_GRANT_CONFIG_V)<<(DPORT_I2S0_ACCESS_GRANT_CONFIG_S))
#define DPORT_I2S0_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_I2S0_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_UART1_REG          (DR_REG_DPORT_BASE + 0x368)
/* DPORT_UART1_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_UART1_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_UART1_ACCESS_GRANT_CONFIG_M  ((DPORT_UART1_ACCESS_GRANT_CONFIG_V)<<(DPORT_UART1_ACCESS_GRANT_CONFIG_S))
#define DPORT_UART1_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_UART1_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_BT_REG          (DR_REG_DPORT_BASE + 0x36C)
/* DPORT_BT_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_BT_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_BT_ACCESS_GRANT_CONFIG_M  ((DPORT_BT_ACCESS_GRANT_CONFIG_V)<<(DPORT_BT_ACCESS_GRANT_CONFIG_S))
#define DPORT_BT_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_BT_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_BT_BUFFER_REG          (DR_REG_DPORT_BASE + 0x370)
/* DPORT_BTBUFFER_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_BTBUFFER_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_BTBUFFER_ACCESS_GRANT_CONFIG_M  ((DPORT_BTBUFFER_ACCESS_GRANT_CONFIG_V)<<(DPORT_BTBUFFER_ACCESS_GRANT_CONFIG_S))
#define DPORT_BTBUFFER_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_BTBUFFER_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_I2C_EXT0_REG          (DR_REG_DPORT_BASE + 0x374)
/* DPORT_I2CEXT0_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_I2CEXT0_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_I2CEXT0_ACCESS_GRANT_CONFIG_M  ((DPORT_I2CEXT0_ACCESS_GRANT_CONFIG_V)<<(DPORT_I2CEXT0_ACCESS_GRANT_CONFIG_S))
#define DPORT_I2CEXT0_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_I2CEXT0_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_UHCI0_REG          (DR_REG_DPORT_BASE + 0x378)
/* DPORT_UHCI0_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_UHCI0_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_UHCI0_ACCESS_GRANT_CONFIG_M  ((DPORT_UHCI0_ACCESS_GRANT_CONFIG_V)<<(DPORT_UHCI0_ACCESS_GRANT_CONFIG_S))
#define DPORT_UHCI0_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_UHCI0_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SLCHOST_REG          (DR_REG_DPORT_BASE + 0x37C)
/* DPORT_SLCHOST_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SLCHOST_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SLCHOST_ACCESS_GRANT_CONFIG_M  ((DPORT_SLCHOST_ACCESS_GRANT_CONFIG_V)<<(DPORT_SLCHOST_ACCESS_GRANT_CONFIG_S))
#define DPORT_SLCHOST_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SLCHOST_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_RMT_REG          (DR_REG_DPORT_BASE + 0x380)
/* DPORT_RMT_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_RMT_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_RMT_ACCESS_GRANT_CONFIG_M  ((DPORT_RMT_ACCESS_GRANT_CONFIG_V)<<(DPORT_RMT_ACCESS_GRANT_CONFIG_S))
#define DPORT_RMT_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_RMT_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_PCNT_REG          (DR_REG_DPORT_BASE + 0x384)
/* DPORT_PCNT_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_PCNT_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_PCNT_ACCESS_GRANT_CONFIG_M  ((DPORT_PCNT_ACCESS_GRANT_CONFIG_V)<<(DPORT_PCNT_ACCESS_GRANT_CONFIG_S))
#define DPORT_PCNT_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_PCNT_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SLC_REG          (DR_REG_DPORT_BASE + 0x388)
/* DPORT_SLC_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SLC_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SLC_ACCESS_GRANT_CONFIG_M  ((DPORT_SLC_ACCESS_GRANT_CONFIG_V)<<(DPORT_SLC_ACCESS_GRANT_CONFIG_S))
#define DPORT_SLC_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SLC_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_LEDC_REG          (DR_REG_DPORT_BASE + 0x38C)
/* DPORT_LEDC_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_LEDC_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_LEDC_ACCESS_GRANT_CONFIG_M  ((DPORT_LEDC_ACCESS_GRANT_CONFIG_V)<<(DPORT_LEDC_ACCESS_GRANT_CONFIG_S))
#define DPORT_LEDC_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_LEDC_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_EFUSE_REG          (DR_REG_DPORT_BASE + 0x390)
/* DPORT_EFUSE_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_EFUSE_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_EFUSE_ACCESS_GRANT_CONFIG_M  ((DPORT_EFUSE_ACCESS_GRANT_CONFIG_V)<<(DPORT_EFUSE_ACCESS_GRANT_CONFIG_S))
#define DPORT_EFUSE_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_EFUSE_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SPI_ENCRYPT_REG          (DR_REG_DPORT_BASE + 0x394)
/* DPORT_SPI_ENCRYPY_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SPI_ENCRYPY_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SPI_ENCRYPY_ACCESS_GRANT_CONFIG_M  ((DPORT_SPI_ENCRYPY_ACCESS_GRANT_CONFIG_V)<<(DPORT_SPI_ENCRYPY_ACCESS_GRANT_CONFIG_S))
#define DPORT_SPI_ENCRYPY_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SPI_ENCRYPY_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_BB_REG          (DR_REG_DPORT_BASE + 0x398)
/* DPORT_BB_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_BB_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_BB_ACCESS_GRANT_CONFIG_M  ((DPORT_BB_ACCESS_GRANT_CONFIG_V)<<(DPORT_BB_ACCESS_GRANT_CONFIG_S))
#define DPORT_BB_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_BB_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_PWM0_REG          (DR_REG_DPORT_BASE + 0x39C)
/* DPORT_PWM0_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_PWM0_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_PWM0_ACCESS_GRANT_CONFIG_M  ((DPORT_PWM0_ACCESS_GRANT_CONFIG_V)<<(DPORT_PWM0_ACCESS_GRANT_CONFIG_S))
#define DPORT_PWM0_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_PWM0_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_TIMERGROUP_REG          (DR_REG_DPORT_BASE + 0x3A0)
/* DPORT_TIMERGROUP_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_TIMERGROUP_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_TIMERGROUP_ACCESS_GRANT_CONFIG_M  ((DPORT_TIMERGROUP_ACCESS_GRANT_CONFIG_V)<<(DPORT_TIMERGROUP_ACCESS_GRANT_CONFIG_S))
#define DPORT_TIMERGROUP_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_TIMERGROUP_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_TIMERGROUP1_REG          (DR_REG_DPORT_BASE + 0x3A4)
/* DPORT_TIMERGROUP1_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_TIMERGROUP1_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_TIMERGROUP1_ACCESS_GRANT_CONFIG_M  ((DPORT_TIMERGROUP1_ACCESS_GRANT_CONFIG_V)<<(DPORT_TIMERGROUP1_ACCESS_GRANT_CONFIG_S))
#define DPORT_TIMERGROUP1_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_TIMERGROUP1_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SPI2_REG          (DR_REG_DPORT_BASE + 0x3A8)
/* DPORT_SPI2_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SPI2_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SPI2_ACCESS_GRANT_CONFIG_M  ((DPORT_SPI2_ACCESS_GRANT_CONFIG_V)<<(DPORT_SPI2_ACCESS_GRANT_CONFIG_S))
#define DPORT_SPI2_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SPI2_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SPI3_REG          (DR_REG_DPORT_BASE + 0x3AC)
/* DPORT_SPI3_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SPI3_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SPI3_ACCESS_GRANT_CONFIG_M  ((DPORT_SPI3_ACCESS_GRANT_CONFIG_V)<<(DPORT_SPI3_ACCESS_GRANT_CONFIG_S))
#define DPORT_SPI3_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SPI3_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_APB_CTRL_REG          (DR_REG_DPORT_BASE + 0x3B0)
/* DPORT_APBCTRL_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_APBCTRL_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_APBCTRL_ACCESS_GRANT_CONFIG_M  ((DPORT_APBCTRL_ACCESS_GRANT_CONFIG_V)<<(DPORT_APBCTRL_ACCESS_GRANT_CONFIG_S))
#define DPORT_APBCTRL_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_APBCTRL_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_I2C_EXT1_REG          (DR_REG_DPORT_BASE + 0x3B4)
/* DPORT_I2CEXT1_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_I2CEXT1_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_I2CEXT1_ACCESS_GRANT_CONFIG_M  ((DPORT_I2CEXT1_ACCESS_GRANT_CONFIG_V)<<(DPORT_I2CEXT1_ACCESS_GRANT_CONFIG_S))
#define DPORT_I2CEXT1_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_I2CEXT1_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_SDIO_HOST_REG          (DR_REG_DPORT_BASE + 0x3B8)
/* DPORT_SDIOHOST_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_SDIOHOST_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_SDIOHOST_ACCESS_GRANT_CONFIG_M  ((DPORT_SDIOHOST_ACCESS_GRANT_CONFIG_V)<<(DPORT_SDIOHOST_ACCESS_GRANT_CONFIG_S))
#define DPORT_SDIOHOST_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_SDIOHOST_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_EMAC_REG          (DR_REG_DPORT_BASE + 0x3BC)
/* DPORT_EMAC_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_EMAC_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_EMAC_ACCESS_GRANT_CONFIG_M  ((DPORT_EMAC_ACCESS_GRANT_CONFIG_V)<<(DPORT_EMAC_ACCESS_GRANT_CONFIG_S))
#define DPORT_EMAC_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_EMAC_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_CAN_REG          (DR_REG_DPORT_BASE + 0x3C0)
/* DPORT_CAN_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_CAN_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_CAN_ACCESS_GRANT_CONFIG_M  ((DPORT_CAN_ACCESS_GRANT_CONFIG_V)<<(DPORT_CAN_ACCESS_GRANT_CONFIG_S))
#define DPORT_CAN_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_CAN_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_PWM1_REG          (DR_REG_DPORT_BASE + 0x3C4)
/* DPORT_PWM1_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_PWM1_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_PWM1_ACCESS_GRANT_CONFIG_M  ((DPORT_PWM1_ACCESS_GRANT_CONFIG_V)<<(DPORT_PWM1_ACCESS_GRANT_CONFIG_S))
#define DPORT_PWM1_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_PWM1_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_I2S1_REG          (DR_REG_DPORT_BASE + 0x3C8)
/* DPORT_I2S1_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_I2S1_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_I2S1_ACCESS_GRANT_CONFIG_M  ((DPORT_I2S1_ACCESS_GRANT_CONFIG_V)<<(DPORT_I2S1_ACCESS_GRANT_CONFIG_S))
#define DPORT_I2S1_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_I2S1_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_UART2_REG          (DR_REG_DPORT_BASE + 0x3CC)
/* DPORT_UART2_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_UART2_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_UART2_ACCESS_GRANT_CONFIG_M  ((DPORT_UART2_ACCESS_GRANT_CONFIG_V)<<(DPORT_UART2_ACCESS_GRANT_CONFIG_S))
#define DPORT_UART2_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_UART2_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_PWM2_REG          (DR_REG_DPORT_BASE + 0x3D0)
/* DPORT_PWM2_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_PWM2_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_PWM2_ACCESS_GRANT_CONFIG_M  ((DPORT_PWM2_ACCESS_GRANT_CONFIG_V)<<(DPORT_PWM2_ACCESS_GRANT_CONFIG_S))
#define DPORT_PWM2_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_PWM2_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_PWM3_REG          (DR_REG_DPORT_BASE + 0x3D4)
/* DPORT_PWM3_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_PWM3_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_PWM3_ACCESS_GRANT_CONFIG_M  ((DPORT_PWM3_ACCESS_GRANT_CONFIG_V)<<(DPORT_PWM3_ACCESS_GRANT_CONFIG_S))
#define DPORT_PWM3_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_PWM3_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_RWBT_REG          (DR_REG_DPORT_BASE + 0x3D8)
/* DPORT_RWBT_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_RWBT_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_RWBT_ACCESS_GRANT_CONFIG_M  ((DPORT_RWBT_ACCESS_GRANT_CONFIG_V)<<(DPORT_RWBT_ACCESS_GRANT_CONFIG_S))
#define DPORT_RWBT_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_RWBT_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_BTMAC_REG          (DR_REG_DPORT_BASE + 0x3DC)
/* DPORT_BTMAC_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_BTMAC_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_BTMAC_ACCESS_GRANT_CONFIG_M  ((DPORT_BTMAC_ACCESS_GRANT_CONFIG_V)<<(DPORT_BTMAC_ACCESS_GRANT_CONFIG_S))
#define DPORT_BTMAC_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_BTMAC_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_WIFIMAC_REG          (DR_REG_DPORT_BASE + 0x3E0)
/* DPORT_WIFIMAC_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_WIFIMAC_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_WIFIMAC_ACCESS_GRANT_CONFIG_M  ((DPORT_WIFIMAC_ACCESS_GRANT_CONFIG_V)<<(DPORT_WIFIMAC_ACCESS_GRANT_CONFIG_S))
#define DPORT_WIFIMAC_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_WIFIMAC_ACCESS_GRANT_CONFIG_S  0

#define DPORT_AHBLITE_MPU_TABLE_PWR_REG          (DR_REG_DPORT_BASE + 0x3E4)
/* DPORT_PWR_ACCESS_GRANT_CONFIG : R/W ;bitpos:[5:0] ;default: 6'b0 ; */
/*description: */
#define DPORT_PWR_ACCESS_GRANT_CONFIG  0x0000003F
#define DPORT_PWR_ACCESS_GRANT_CONFIG_M  ((DPORT_PWR_ACCESS_GRANT_CONFIG_V)<<(DPORT_PWR_ACCESS_GRANT_CONFIG_S))
#define DPORT_PWR_ACCESS_GRANT_CONFIG_V  0x3F
#define DPORT_PWR_ACCESS_GRANT_CONFIG_S  0

#define DPORT_MEM_ACCESS_DBUG0_REG          (DR_REG_DPORT_BASE + 0x3E8)
/* DPORT_INTERNAL_SRAM_MMU_MULTI_HIT : RO ;bitpos:[29:26] ;default: 4'b0 ; */
/*description: */
#define DPORT_INTERNAL_SRAM_MMU_MULTI_HIT  0x0000000F
#define DPORT_INTERNAL_SRAM_MMU_MULTI_HIT_M  ((DPORT_INTERNAL_SRAM_MMU_MULTI_HIT_V)<<(DPORT_INTERNAL_SRAM_MMU_MULTI_HIT_S))
#define DPORT_INTERNAL_SRAM_MMU_MULTI_HIT_V  0xF
#define DPORT_INTERNAL_SRAM_MMU_MULTI_HIT_S  26
/* DPORT_INTERNAL_SRAM_IA : RO ;bitpos:[25:14] ;default: 12'b0 ; */
/*description: */
#define DPORT_INTERNAL_SRAM_IA  0x00000FFF
#define DPORT_INTERNAL_SRAM_IA_M  ((DPORT_INTERNAL_SRAM_IA_V)<<(DPORT_INTERNAL_SRAM_IA_S))
#define DPORT_INTERNAL_SRAM_IA_V  0xFFF
#define DPORT_INTERNAL_SRAM_IA_S  14
/* DPORT_INTERNAL_SRAM_MMU_AD : RO ;bitpos:[13:10] ;default: 4'b0 ; */
/*description: */
#define DPORT_INTERNAL_SRAM_MMU_AD  0x0000000F
#define DPORT_INTERNAL_SRAM_MMU_AD_M  ((DPORT_INTERNAL_SRAM_MMU_AD_V)<<(DPORT_INTERNAL_SRAM_MMU_AD_S))
#define DPORT_INTERNAL_SRAM_MMU_AD_V  0xF
#define DPORT_INTERNAL_SRAM_MMU_AD_S  10
/* DPORT_SHARE_ROM_IA : RO ;bitpos:[9:6] ;default: 4'b0 ; */
/*description: */
#define DPORT_SHARE_ROM_IA  0x0000000F
#define DPORT_SHARE_ROM_IA_M  ((DPORT_SHARE_ROM_IA_V)<<(DPORT_SHARE_ROM_IA_S))
#define DPORT_SHARE_ROM_IA_V  0xF
#define DPORT_SHARE_ROM_IA_S  6
/* DPORT_SHARE_ROM_MPU_AD : RO ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: */
#define DPORT_SHARE_ROM_MPU_AD  0x00000003
#define DPORT_SHARE_ROM_MPU_AD_M  ((DPORT_SHARE_ROM_MPU_AD_V)<<(DPORT_SHARE_ROM_MPU_AD_S))
#define DPORT_SHARE_ROM_MPU_AD_V  0x3
#define DPORT_SHARE_ROM_MPU_AD_S  4
/* DPORT_APP_ROM_IA : RO ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_ROM_IA  (BIT(3))
#define DPORT_APP_ROM_IA_M  (BIT(3))
#define DPORT_APP_ROM_IA_V  0x1
#define DPORT_APP_ROM_IA_S  3
/* DPORT_APP_ROM_MPU_AD : RO ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_ROM_MPU_AD  (BIT(2))
#define DPORT_APP_ROM_MPU_AD_M  (BIT(2))
#define DPORT_APP_ROM_MPU_AD_V  0x1
#define DPORT_APP_ROM_MPU_AD_S  2
/* DPORT_PRO_ROM_IA : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_ROM_IA  (BIT(1))
#define DPORT_PRO_ROM_IA_M  (BIT(1))
#define DPORT_PRO_ROM_IA_V  0x1
#define DPORT_PRO_ROM_IA_S  1
/* DPORT_PRO_ROM_MPU_AD : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_ROM_MPU_AD  (BIT(0))
#define DPORT_PRO_ROM_MPU_AD_M  (BIT(0))
#define DPORT_PRO_ROM_MPU_AD_V  0x1
#define DPORT_PRO_ROM_MPU_AD_S  0

#define DPORT_MEM_ACCESS_DBUG1_REG          (DR_REG_DPORT_BASE + 0x3EC)
/* DPORT_AHBLITE_IA : RO ;bitpos:[10] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHBLITE_IA  (BIT(10))
#define DPORT_AHBLITE_IA_M  (BIT(10))
#define DPORT_AHBLITE_IA_V  0x1
#define DPORT_AHBLITE_IA_S  10
/* DPORT_AHBLITE_ACCESS_DENY : RO ;bitpos:[9] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHBLITE_ACCESS_DENY  (BIT(9))
#define DPORT_AHBLITE_ACCESS_DENY_M  (BIT(9))
#define DPORT_AHBLITE_ACCESS_DENY_V  0x1
#define DPORT_AHBLITE_ACCESS_DENY_S  9
/* DPORT_AHB_ACCESS_DENY : RO ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define DPORT_AHB_ACCESS_DENY  (BIT(8))
#define DPORT_AHB_ACCESS_DENY_M  (BIT(8))
#define DPORT_AHB_ACCESS_DENY_V  0x1
#define DPORT_AHB_ACCESS_DENY_S  8
/* DPORT_PIDGEN_IA : RO ;bitpos:[7:6] ;default: 2'b0 ; */
/*description: */
#define DPORT_PIDGEN_IA  0x00000003
#define DPORT_PIDGEN_IA_M  ((DPORT_PIDGEN_IA_V)<<(DPORT_PIDGEN_IA_S))
#define DPORT_PIDGEN_IA_V  0x3
#define DPORT_PIDGEN_IA_S  6
/* DPORT_ARB_IA : RO ;bitpos:[5:4] ;default: 2'b0 ; */
/*description: */
#define DPORT_ARB_IA  0x00000003
#define DPORT_ARB_IA_M  ((DPORT_ARB_IA_V)<<(DPORT_ARB_IA_S))
#define DPORT_ARB_IA_V  0x3
#define DPORT_ARB_IA_S  4
/* DPORT_INTERNAL_SRAM_MMU_MISS : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: */
#define DPORT_INTERNAL_SRAM_MMU_MISS  0x0000000F
#define DPORT_INTERNAL_SRAM_MMU_MISS_M  ((DPORT_INTERNAL_SRAM_MMU_MISS_V)<<(DPORT_INTERNAL_SRAM_MMU_MISS_S))
#define DPORT_INTERNAL_SRAM_MMU_MISS_V  0xF
#define DPORT_INTERNAL_SRAM_MMU_MISS_S  0

#define DPORT_PRO_DCACHE_DBUG0_REG          (DR_REG_DPORT_BASE + 0x3F0)
/* DPORT_PRO_RX_END : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_RX_END  (BIT(23))
#define DPORT_PRO_RX_END_M  (BIT(23))
#define DPORT_PRO_RX_END_V  0x1
#define DPORT_PRO_RX_END_S  23
/* DPORT_PRO_SLAVE_WDATA_V : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_SLAVE_WDATA_V  (BIT(22))
#define DPORT_PRO_SLAVE_WDATA_V_M  (BIT(22))
#define DPORT_PRO_SLAVE_WDATA_V_V  0x1
#define DPORT_PRO_SLAVE_WDATA_V_S  22
/* DPORT_PRO_SLAVE_WR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_SLAVE_WR  (BIT(21))
#define DPORT_PRO_SLAVE_WR_M  (BIT(21))
#define DPORT_PRO_SLAVE_WR_V  0x1
#define DPORT_PRO_SLAVE_WR_S  21
/* DPORT_PRO_TX_END : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_TX_END  (BIT(20))
#define DPORT_PRO_TX_END_M  (BIT(20))
#define DPORT_PRO_TX_END_V  0x1
#define DPORT_PRO_TX_END_S  20
/* DPORT_PRO_WR_BAK_TO_READ : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_WR_BAK_TO_READ  (BIT(19))
#define DPORT_PRO_WR_BAK_TO_READ_M  (BIT(19))
#define DPORT_PRO_WR_BAK_TO_READ_V  0x1
#define DPORT_PRO_WR_BAK_TO_READ_S  19
/* DPORT_PRO_CACHE_STATE : RO ;bitpos:[18:7] ;default: 12'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_STATE  0x00000FFF
#define DPORT_PRO_CACHE_STATE_M  ((DPORT_PRO_CACHE_STATE_V)<<(DPORT_PRO_CACHE_STATE_S))
#define DPORT_PRO_CACHE_STATE_V  0xFFF
#define DPORT_PRO_CACHE_STATE_S  7
/* DPORT_PRO_CACHE_IA : RO ;bitpos:[6:1] ;default: 6'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_IA  0x0000003F
#define DPORT_PRO_CACHE_IA_M  ((DPORT_PRO_CACHE_IA_V)<<(DPORT_PRO_CACHE_IA_S))
#define DPORT_PRO_CACHE_IA_V  0x3F
#define DPORT_PRO_CACHE_IA_S  1
/* DPORT_PRO_CACHE_MMU_IA : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_MMU_IA  (BIT(0))
#define DPORT_PRO_CACHE_MMU_IA_M  (BIT(0))
#define DPORT_PRO_CACHE_MMU_IA_V  0x1
#define DPORT_PRO_CACHE_MMU_IA_S  0

#define DPORT_PRO_DCACHE_DBUG1_REG          (DR_REG_DPORT_BASE + 0x3F4)
/* DPORT_PRO_CTAG_RAM_RDATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_PRO_CTAG_RAM_RDATA  0xFFFFFFFF
#define DPORT_PRO_CTAG_RAM_RDATA_M  ((DPORT_PRO_CTAG_RAM_RDATA_V)<<(DPORT_PRO_CTAG_RAM_RDATA_S))
#define DPORT_PRO_CTAG_RAM_RDATA_V  0xFFFFFFFF
#define DPORT_PRO_CTAG_RAM_RDATA_S  0

#define DPORT_PRO_DCACHE_DBUG2_REG          (DR_REG_DPORT_BASE + 0x3F8)
/* DPORT_PRO_CACHE_VADDR : RO ;bitpos:[26:0] ;default: 27'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_VADDR  0x07FFFFFF
#define DPORT_PRO_CACHE_VADDR_M  ((DPORT_PRO_CACHE_VADDR_V)<<(DPORT_PRO_CACHE_VADDR_S))
#define DPORT_PRO_CACHE_VADDR_V  0x7FFFFFF
#define DPORT_PRO_CACHE_VADDR_S  0

#define DPORT_PRO_DCACHE_DBUG3_REG          (DR_REG_DPORT_BASE + 0x3FC)
/* DPORT_PRO_CACHE_IRAM0_PID_ERROR : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CACHE_IRAM0_PID_ERROR  (BIT(15))
#define DPORT_PRO_CACHE_IRAM0_PID_ERROR_M  (BIT(15))
#define DPORT_PRO_CACHE_IRAM0_PID_ERROR_V  0x1
#define DPORT_PRO_CACHE_IRAM0_PID_ERROR_S  15
/* DPORT_PRO_CPU_DISABLED_CACHE_IA : RO ;bitpos:[14:9] ;default: 6'b0 ; */
/*description: */
#define DPORT_PRO_CPU_DISABLED_CACHE_IA  0x0000003F
#define DPORT_PRO_CPU_DISABLED_CACHE_IA_M  ((DPORT_PRO_CPU_DISABLED_CACHE_IA_V)<<(DPORT_PRO_CPU_DISABLED_CACHE_IA_S))
#define DPORT_PRO_CPU_DISABLED_CACHE_IA_V  0x3F
#define DPORT_PRO_CPU_DISABLED_CACHE_IA_S  9
/* DPORT_PRO_MMU_RDATA : RO ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: */
#define DPORT_PRO_MMU_RDATA  0x000001FF
#define DPORT_PRO_MMU_RDATA_M  ((DPORT_PRO_MMU_RDATA_V)<<(DPORT_PRO_MMU_RDATA_S))
#define DPORT_PRO_MMU_RDATA_V  0x1FF
#define DPORT_PRO_MMU_RDATA_S  0

#define DPORT_PRO_DCACHE_DBUG4_REG          (DR_REG_DPORT_BASE + 0x400)
/* DPORT_PRO_DRAM1ADDR0_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_PRO_DRAM1ADDR0_IA  0x000FFFFF
#define DPORT_PRO_DRAM1ADDR0_IA_M  ((DPORT_PRO_DRAM1ADDR0_IA_V)<<(DPORT_PRO_DRAM1ADDR0_IA_S))
#define DPORT_PRO_DRAM1ADDR0_IA_V  0xFFFFF
#define DPORT_PRO_DRAM1ADDR0_IA_S  0

#define DPORT_PRO_DCACHE_DBUG5_REG          (DR_REG_DPORT_BASE + 0x404)
/* DPORT_PRO_DROM0ADDR0_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_PRO_DROM0ADDR0_IA  0x000FFFFF
#define DPORT_PRO_DROM0ADDR0_IA_M  ((DPORT_PRO_DROM0ADDR0_IA_V)<<(DPORT_PRO_DROM0ADDR0_IA_S))
#define DPORT_PRO_DROM0ADDR0_IA_V  0xFFFFF
#define DPORT_PRO_DROM0ADDR0_IA_S  0

#define DPORT_PRO_DCACHE_DBUG6_REG          (DR_REG_DPORT_BASE + 0x408)
/* DPORT_PRO_IRAM0ADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_PRO_IRAM0ADDR_IA  0x000FFFFF
#define DPORT_PRO_IRAM0ADDR_IA_M  ((DPORT_PRO_IRAM0ADDR_IA_V)<<(DPORT_PRO_IRAM0ADDR_IA_S))
#define DPORT_PRO_IRAM0ADDR_IA_V  0xFFFFF
#define DPORT_PRO_IRAM0ADDR_IA_S  0

#define DPORT_PRO_DCACHE_DBUG7_REG          (DR_REG_DPORT_BASE + 0x40C)
/* DPORT_PRO_IRAM1ADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_PRO_IRAM1ADDR_IA  0x000FFFFF
#define DPORT_PRO_IRAM1ADDR_IA_M  ((DPORT_PRO_IRAM1ADDR_IA_V)<<(DPORT_PRO_IRAM1ADDR_IA_S))
#define DPORT_PRO_IRAM1ADDR_IA_V  0xFFFFF
#define DPORT_PRO_IRAM1ADDR_IA_S  0

#define DPORT_PRO_DCACHE_DBUG8_REG          (DR_REG_DPORT_BASE + 0x410)
/* DPORT_PRO_IROM0ADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_PRO_IROM0ADDR_IA  0x000FFFFF
#define DPORT_PRO_IROM0ADDR_IA_M  ((DPORT_PRO_IROM0ADDR_IA_V)<<(DPORT_PRO_IROM0ADDR_IA_S))
#define DPORT_PRO_IROM0ADDR_IA_V  0xFFFFF
#define DPORT_PRO_IROM0ADDR_IA_S  0

#define DPORT_PRO_DCACHE_DBUG9_REG          (DR_REG_DPORT_BASE + 0x414)
/* DPORT_PRO_OPSDRAMADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_PRO_OPSDRAMADDR_IA  0x000FFFFF
#define DPORT_PRO_OPSDRAMADDR_IA_M  ((DPORT_PRO_OPSDRAMADDR_IA_V)<<(DPORT_PRO_OPSDRAMADDR_IA_S))
#define DPORT_PRO_OPSDRAMADDR_IA_V  0xFFFFF
#define DPORT_PRO_OPSDRAMADDR_IA_S  0

#define DPORT_APP_DCACHE_DBUG0_REG          (DR_REG_DPORT_BASE + 0x418)
/* DPORT_APP_RX_END : RO ;bitpos:[23] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_RX_END  (BIT(23))
#define DPORT_APP_RX_END_M  (BIT(23))
#define DPORT_APP_RX_END_V  0x1
#define DPORT_APP_RX_END_S  23
/* DPORT_APP_SLAVE_WDATA_V : RO ;bitpos:[22] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_SLAVE_WDATA_V  (BIT(22))
#define DPORT_APP_SLAVE_WDATA_V_M  (BIT(22))
#define DPORT_APP_SLAVE_WDATA_V_V  0x1
#define DPORT_APP_SLAVE_WDATA_V_S  22
/* DPORT_APP_SLAVE_WR : RO ;bitpos:[21] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_SLAVE_WR  (BIT(21))
#define DPORT_APP_SLAVE_WR_M  (BIT(21))
#define DPORT_APP_SLAVE_WR_V  0x1
#define DPORT_APP_SLAVE_WR_S  21
/* DPORT_APP_TX_END : RO ;bitpos:[20] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_TX_END  (BIT(20))
#define DPORT_APP_TX_END_M  (BIT(20))
#define DPORT_APP_TX_END_V  0x1
#define DPORT_APP_TX_END_S  20
/* DPORT_APP_WR_BAK_TO_READ : RO ;bitpos:[19] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_WR_BAK_TO_READ  (BIT(19))
#define DPORT_APP_WR_BAK_TO_READ_M  (BIT(19))
#define DPORT_APP_WR_BAK_TO_READ_V  0x1
#define DPORT_APP_WR_BAK_TO_READ_S  19
/* DPORT_APP_CACHE_STATE : RO ;bitpos:[18:7] ;default: 12'b0 ; */
/*description: */
#define DPORT_APP_CACHE_STATE  0x00000FFF
#define DPORT_APP_CACHE_STATE_M  ((DPORT_APP_CACHE_STATE_V)<<(DPORT_APP_CACHE_STATE_S))
#define DPORT_APP_CACHE_STATE_V  0xFFF
#define DPORT_APP_CACHE_STATE_S  7
/* DPORT_APP_CACHE_IA : RO ;bitpos:[6:1] ;default: 6'b0 ; */
/*description: */
#define DPORT_APP_CACHE_IA  0x0000003F
#define DPORT_APP_CACHE_IA_M  ((DPORT_APP_CACHE_IA_V)<<(DPORT_APP_CACHE_IA_S))
#define DPORT_APP_CACHE_IA_V  0x3F
#define DPORT_APP_CACHE_IA_S  1
/* DPORT_APP_CACHE_MMU_IA : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_MMU_IA  (BIT(0))
#define DPORT_APP_CACHE_MMU_IA_M  (BIT(0))
#define DPORT_APP_CACHE_MMU_IA_V  0x1
#define DPORT_APP_CACHE_MMU_IA_S  0

#define DPORT_APP_DCACHE_DBUG1_REG          (DR_REG_DPORT_BASE + 0x41C)
/* DPORT_APP_CTAG_RAM_RDATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_APP_CTAG_RAM_RDATA  0xFFFFFFFF
#define DPORT_APP_CTAG_RAM_RDATA_M  ((DPORT_APP_CTAG_RAM_RDATA_V)<<(DPORT_APP_CTAG_RAM_RDATA_S))
#define DPORT_APP_CTAG_RAM_RDATA_V  0xFFFFFFFF
#define DPORT_APP_CTAG_RAM_RDATA_S  0

#define DPORT_APP_DCACHE_DBUG2_REG          (DR_REG_DPORT_BASE + 0x420)
/* DPORT_APP_CACHE_VADDR : RO ;bitpos:[26:0] ;default: 27'b0 ; */
/*description: */
#define DPORT_APP_CACHE_VADDR  0x07FFFFFF
#define DPORT_APP_CACHE_VADDR_M  ((DPORT_APP_CACHE_VADDR_V)<<(DPORT_APP_CACHE_VADDR_S))
#define DPORT_APP_CACHE_VADDR_V  0x7FFFFFF
#define DPORT_APP_CACHE_VADDR_S  0

#define DPORT_APP_DCACHE_DBUG3_REG          (DR_REG_DPORT_BASE + 0x424)
/* DPORT_APP_CACHE_IRAM0_PID_ERROR : RO ;bitpos:[15] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CACHE_IRAM0_PID_ERROR  (BIT(15))
#define DPORT_APP_CACHE_IRAM0_PID_ERROR_M  (BIT(15))
#define DPORT_APP_CACHE_IRAM0_PID_ERROR_V  0x1
#define DPORT_APP_CACHE_IRAM0_PID_ERROR_S  15
/* DPORT_APP_CPU_DISABLED_CACHE_IA : RO ;bitpos:[14:9] ;default: 6'b0 ; */
/*description: */
#define DPORT_APP_CPU_DISABLED_CACHE_IA  0x0000003F
#define DPORT_APP_CPU_DISABLED_CACHE_IA_M  ((DPORT_APP_CPU_DISABLED_CACHE_IA_V)<<(DPORT_APP_CPU_DISABLED_CACHE_IA_S))
#define DPORT_APP_CPU_DISABLED_CACHE_IA_V  0x3F
#define DPORT_APP_CPU_DISABLED_CACHE_IA_S  9
/* DPORT_APP_MMU_RDATA : RO ;bitpos:[8:0] ;default: 9'h0 ; */
/*description: */
#define DPORT_APP_MMU_RDATA  0x000001FF
#define DPORT_APP_MMU_RDATA_M  ((DPORT_APP_MMU_RDATA_V)<<(DPORT_APP_MMU_RDATA_S))
#define DPORT_APP_MMU_RDATA_V  0x1FF
#define DPORT_APP_MMU_RDATA_S  0

#define DPORT_APP_DCACHE_DBUG4_REG          (DR_REG_DPORT_BASE + 0x428)
/* DPORT_APP_DRAM1ADDR0_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_APP_DRAM1ADDR0_IA  0x000FFFFF
#define DPORT_APP_DRAM1ADDR0_IA_M  ((DPORT_APP_DRAM1ADDR0_IA_V)<<(DPORT_APP_DRAM1ADDR0_IA_S))
#define DPORT_APP_DRAM1ADDR0_IA_V  0xFFFFF
#define DPORT_APP_DRAM1ADDR0_IA_S  0

#define DPORT_APP_DCACHE_DBUG5_REG          (DR_REG_DPORT_BASE + 0x42C)
/* DPORT_APP_DROM0ADDR0_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_APP_DROM0ADDR0_IA  0x000FFFFF
#define DPORT_APP_DROM0ADDR0_IA_M  ((DPORT_APP_DROM0ADDR0_IA_V)<<(DPORT_APP_DROM0ADDR0_IA_S))
#define DPORT_APP_DROM0ADDR0_IA_V  0xFFFFF
#define DPORT_APP_DROM0ADDR0_IA_S  0

#define DPORT_APP_DCACHE_DBUG6_REG          (DR_REG_DPORT_BASE + 0x430)
/* DPORT_APP_IRAM0ADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_APP_IRAM0ADDR_IA  0x000FFFFF
#define DPORT_APP_IRAM0ADDR_IA_M  ((DPORT_APP_IRAM0ADDR_IA_V)<<(DPORT_APP_IRAM0ADDR_IA_S))
#define DPORT_APP_IRAM0ADDR_IA_V  0xFFFFF
#define DPORT_APP_IRAM0ADDR_IA_S  0

#define DPORT_APP_DCACHE_DBUG7_REG          (DR_REG_DPORT_BASE + 0x434)
/* DPORT_APP_IRAM1ADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_APP_IRAM1ADDR_IA  0x000FFFFF
#define DPORT_APP_IRAM1ADDR_IA_M  ((DPORT_APP_IRAM1ADDR_IA_V)<<(DPORT_APP_IRAM1ADDR_IA_S))
#define DPORT_APP_IRAM1ADDR_IA_V  0xFFFFF
#define DPORT_APP_IRAM1ADDR_IA_S  0

#define DPORT_APP_DCACHE_DBUG8_REG          (DR_REG_DPORT_BASE + 0x438)
/* DPORT_APP_IROM0ADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_APP_IROM0ADDR_IA  0x000FFFFF
#define DPORT_APP_IROM0ADDR_IA_M  ((DPORT_APP_IROM0ADDR_IA_V)<<(DPORT_APP_IROM0ADDR_IA_S))
#define DPORT_APP_IROM0ADDR_IA_V  0xFFFFF
#define DPORT_APP_IROM0ADDR_IA_S  0

#define DPORT_APP_DCACHE_DBUG9_REG          (DR_REG_DPORT_BASE + 0x43C)
/* DPORT_APP_OPSDRAMADDR_IA : RO ;bitpos:[19:0] ;default: 20'b0 ; */
/*description: */
#define DPORT_APP_OPSDRAMADDR_IA  0x000FFFFF
#define DPORT_APP_OPSDRAMADDR_IA_M  ((DPORT_APP_OPSDRAMADDR_IA_V)<<(DPORT_APP_OPSDRAMADDR_IA_S))
#define DPORT_APP_OPSDRAMADDR_IA_V  0xFFFFF
#define DPORT_APP_OPSDRAMADDR_IA_S  0

#define DPORT_PRO_CPU_RECORD_CTRL_REG          (DR_REG_DPORT_BASE + 0x440)
/* DPORT_PRO_CPU_PDEBUG_ENABLE : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_CPU_PDEBUG_ENABLE  (BIT(8))
#define DPORT_PRO_CPU_PDEBUG_ENABLE_M  (BIT(8))
#define DPORT_PRO_CPU_PDEBUG_ENABLE_V  0x1
#define DPORT_PRO_CPU_PDEBUG_ENABLE_S  8
/* DPORT_PRO_CPU_RECORD_DISABLE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CPU_RECORD_DISABLE  (BIT(4))
#define DPORT_PRO_CPU_RECORD_DISABLE_M  (BIT(4))
#define DPORT_PRO_CPU_RECORD_DISABLE_V  0x1
#define DPORT_PRO_CPU_RECORD_DISABLE_S  4
/* DPORT_PRO_CPU_RECORD_ENABLE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CPU_RECORD_ENABLE  (BIT(0))
#define DPORT_PRO_CPU_RECORD_ENABLE_M  (BIT(0))
#define DPORT_PRO_CPU_RECORD_ENABLE_V  0x1
#define DPORT_PRO_CPU_RECORD_ENABLE_S  0

#define DPORT_PRO_CPU_RECORD_STATUS_REG          (DR_REG_DPORT_BASE + 0x444)
/* DPORT_PRO_CPU_RECORDING : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_PRO_CPU_RECORDING  (BIT(0))
#define DPORT_PRO_CPU_RECORDING_M  (BIT(0))
#define DPORT_PRO_CPU_RECORDING_V  0x1
#define DPORT_PRO_CPU_RECORDING_S  0

#define DPORT_PRO_CPU_RECORD_PID_REG          (DR_REG_DPORT_BASE + 0x448)
/* DPORT_RECORD_PRO_PID : RO ;bitpos:[2:0] ;default: 3'd0 ; */
/*description: */
#define DPORT_RECORD_PRO_PID  0x00000007
#define DPORT_RECORD_PRO_PID_M  ((DPORT_RECORD_PRO_PID_V)<<(DPORT_RECORD_PRO_PID_S))
#define DPORT_RECORD_PRO_PID_V  0x7
#define DPORT_RECORD_PRO_PID_S  0

#define DPORT_PRO_CPU_RECORD_PDEBUGINST_REG          (DR_REG_DPORT_BASE + 0x44C)
/* DPORT_RECORD_PRO_PDEBUGINST : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_PRO_PDEBUGINST  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGINST_M  ((DPORT_RECORD_PRO_PDEBUGINST_V)<<(DPORT_RECORD_PRO_PDEBUGINST_S))
#define DPORT_RECORD_PRO_PDEBUGINST_V  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGINST_S  0

#define DPORT_PRO_CPU_RECORD_PDEBUGSTATUS_REG          (DR_REG_DPORT_BASE + 0x450)
/* DPORT_RECORD_PRO_PDEBUGSTATUS : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: */
#define DPORT_RECORD_PRO_PDEBUGSTATUS  0x000000FF
#define DPORT_RECORD_PRO_PDEBUGSTATUS_M  ((DPORT_RECORD_PRO_PDEBUGSTATUS_V)<<(DPORT_RECORD_PRO_PDEBUGSTATUS_S))
#define DPORT_RECORD_PRO_PDEBUGSTATUS_V  0xFF
#define DPORT_RECORD_PRO_PDEBUGSTATUS_S  0

#define DPORT_PRO_CPU_RECORD_PDEBUGDATA_REG          (DR_REG_DPORT_BASE + 0x454)
/* DPORT_RECORD_PRO_PDEBUGDATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_PRO_PDEBUGDATA  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGDATA_M  ((DPORT_RECORD_PRO_PDEBUGDATA_V)<<(DPORT_RECORD_PRO_PDEBUGDATA_S))
#define DPORT_RECORD_PRO_PDEBUGDATA_V  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGDATA_S  0

#define DPORT_PRO_CPU_RECORD_PDEBUGPC_REG          (DR_REG_DPORT_BASE + 0x458)
/* DPORT_RECORD_PRO_PDEBUGPC : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_PRO_PDEBUGPC  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGPC_M  ((DPORT_RECORD_PRO_PDEBUGPC_V)<<(DPORT_RECORD_PRO_PDEBUGPC_S))
#define DPORT_RECORD_PRO_PDEBUGPC_V  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGPC_S  0

#define DPORT_PRO_CPU_RECORD_PDEBUGLS0STAT_REG          (DR_REG_DPORT_BASE + 0x45C)
/* DPORT_RECORD_PRO_PDEBUGLS0STAT : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_PRO_PDEBUGLS0STAT  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGLS0STAT_M  ((DPORT_RECORD_PRO_PDEBUGLS0STAT_V)<<(DPORT_RECORD_PRO_PDEBUGLS0STAT_S))
#define DPORT_RECORD_PRO_PDEBUGLS0STAT_V  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGLS0STAT_S  0

#define DPORT_PRO_CPU_RECORD_PDEBUGLS0ADDR_REG          (DR_REG_DPORT_BASE + 0x460)
/* DPORT_RECORD_PRO_PDEBUGLS0ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_PRO_PDEBUGLS0ADDR  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGLS0ADDR_M  ((DPORT_RECORD_PRO_PDEBUGLS0ADDR_V)<<(DPORT_RECORD_PRO_PDEBUGLS0ADDR_S))
#define DPORT_RECORD_PRO_PDEBUGLS0ADDR_V  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGLS0ADDR_S  0

#define DPORT_PRO_CPU_RECORD_PDEBUGLS0DATA_REG          (DR_REG_DPORT_BASE + 0x464)
/* DPORT_RECORD_PRO_PDEBUGLS0DATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_PRO_PDEBUGLS0DATA  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGLS0DATA_M  ((DPORT_RECORD_PRO_PDEBUGLS0DATA_V)<<(DPORT_RECORD_PRO_PDEBUGLS0DATA_S))
#define DPORT_RECORD_PRO_PDEBUGLS0DATA_V  0xFFFFFFFF
#define DPORT_RECORD_PRO_PDEBUGLS0DATA_S  0

#define DPORT_APP_CPU_RECORD_CTRL_REG          (DR_REG_DPORT_BASE + 0x468)
/* DPORT_APP_CPU_PDEBUG_ENABLE : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_CPU_PDEBUG_ENABLE  (BIT(8))
#define DPORT_APP_CPU_PDEBUG_ENABLE_M  (BIT(8))
#define DPORT_APP_CPU_PDEBUG_ENABLE_V  0x1
#define DPORT_APP_CPU_PDEBUG_ENABLE_S  8
/* DPORT_APP_CPU_RECORD_DISABLE : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CPU_RECORD_DISABLE  (BIT(4))
#define DPORT_APP_CPU_RECORD_DISABLE_M  (BIT(4))
#define DPORT_APP_CPU_RECORD_DISABLE_V  0x1
#define DPORT_APP_CPU_RECORD_DISABLE_S  4
/* DPORT_APP_CPU_RECORD_ENABLE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CPU_RECORD_ENABLE  (BIT(0))
#define DPORT_APP_CPU_RECORD_ENABLE_M  (BIT(0))
#define DPORT_APP_CPU_RECORD_ENABLE_V  0x1
#define DPORT_APP_CPU_RECORD_ENABLE_S  0

#define DPORT_APP_CPU_RECORD_STATUS_REG          (DR_REG_DPORT_BASE + 0x46C)
/* DPORT_APP_CPU_RECORDING : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_APP_CPU_RECORDING  (BIT(0))
#define DPORT_APP_CPU_RECORDING_M  (BIT(0))
#define DPORT_APP_CPU_RECORDING_V  0x1
#define DPORT_APP_CPU_RECORDING_S  0

#define DPORT_APP_CPU_RECORD_PID_REG          (DR_REG_DPORT_BASE + 0x470)
/* DPORT_RECORD_APP_PID : RO ;bitpos:[2:0] ;default: 3'd0 ; */
/*description: */
#define DPORT_RECORD_APP_PID  0x00000007
#define DPORT_RECORD_APP_PID_M  ((DPORT_RECORD_APP_PID_V)<<(DPORT_RECORD_APP_PID_S))
#define DPORT_RECORD_APP_PID_V  0x7
#define DPORT_RECORD_APP_PID_S  0

#define DPORT_APP_CPU_RECORD_PDEBUGINST_REG          (DR_REG_DPORT_BASE + 0x474)
/* DPORT_RECORD_APP_PDEBUGINST : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_APP_PDEBUGINST  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGINST_M  ((DPORT_RECORD_APP_PDEBUGINST_V)<<(DPORT_RECORD_APP_PDEBUGINST_S))
#define DPORT_RECORD_APP_PDEBUGINST_V  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGINST_S  0

#define DPORT_APP_CPU_RECORD_PDEBUGSTATUS_REG          (DR_REG_DPORT_BASE + 0x478)
/* DPORT_RECORD_APP_PDEBUGSTATUS : RO ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: */
#define DPORT_RECORD_APP_PDEBUGSTATUS  0x000000FF
#define DPORT_RECORD_APP_PDEBUGSTATUS_M  ((DPORT_RECORD_APP_PDEBUGSTATUS_V)<<(DPORT_RECORD_APP_PDEBUGSTATUS_S))
#define DPORT_RECORD_APP_PDEBUGSTATUS_V  0xFF
#define DPORT_RECORD_APP_PDEBUGSTATUS_S  0

#define DPORT_APP_CPU_RECORD_PDEBUGDATA_REG          (DR_REG_DPORT_BASE + 0x47C)
/* DPORT_RECORD_APP_PDEBUGDATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_APP_PDEBUGDATA  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGDATA_M  ((DPORT_RECORD_APP_PDEBUGDATA_V)<<(DPORT_RECORD_APP_PDEBUGDATA_S))
#define DPORT_RECORD_APP_PDEBUGDATA_V  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGDATA_S  0

#define DPORT_APP_CPU_RECORD_PDEBUGPC_REG          (DR_REG_DPORT_BASE + 0x480)
/* DPORT_RECORD_APP_PDEBUGPC : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_APP_PDEBUGPC  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGPC_M  ((DPORT_RECORD_APP_PDEBUGPC_V)<<(DPORT_RECORD_APP_PDEBUGPC_S))
#define DPORT_RECORD_APP_PDEBUGPC_V  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGPC_S  0

#define DPORT_APP_CPU_RECORD_PDEBUGLS0STAT_REG          (DR_REG_DPORT_BASE + 0x484)
/* DPORT_RECORD_APP_PDEBUGLS0STAT : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_APP_PDEBUGLS0STAT  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGLS0STAT_M  ((DPORT_RECORD_APP_PDEBUGLS0STAT_V)<<(DPORT_RECORD_APP_PDEBUGLS0STAT_S))
#define DPORT_RECORD_APP_PDEBUGLS0STAT_V  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGLS0STAT_S  0

#define DPORT_APP_CPU_RECORD_PDEBUGLS0ADDR_REG          (DR_REG_DPORT_BASE + 0x488)
/* DPORT_RECORD_APP_PDEBUGLS0ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_APP_PDEBUGLS0ADDR  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGLS0ADDR_M  ((DPORT_RECORD_APP_PDEBUGLS0ADDR_V)<<(DPORT_RECORD_APP_PDEBUGLS0ADDR_S))
#define DPORT_RECORD_APP_PDEBUGLS0ADDR_V  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGLS0ADDR_S  0

#define DPORT_APP_CPU_RECORD_PDEBUGLS0DATA_REG          (DR_REG_DPORT_BASE + 0x48C)
/* DPORT_RECORD_APP_PDEBUGLS0DATA : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define DPORT_RECORD_APP_PDEBUGLS0DATA  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGLS0DATA_M  ((DPORT_RECORD_APP_PDEBUGLS0DATA_V)<<(DPORT_RECORD_APP_PDEBUGLS0DATA_S))
#define DPORT_RECORD_APP_PDEBUGLS0DATA_V  0xFFFFFFFF
#define DPORT_RECORD_APP_PDEBUGLS0DATA_S  0

#define DPORT_RSA_PD_CTRL_REG          (DR_REG_DPORT_BASE + 0x490)
/* DPORT_RSA_PD : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_RSA_PD  (BIT(0))
#define DPORT_RSA_PD_M  (BIT(0))
#define DPORT_RSA_PD_V  0x1
#define DPORT_RSA_PD_S  0

#define DPORT_ROM_MPU_TABLE0_REG          (DR_REG_DPORT_BASE + 0x494)
/* DPORT_ROM_MPU_TABLE0 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_ROM_MPU_TABLE0  0x00000003
#define DPORT_ROM_MPU_TABLE0_M  ((DPORT_ROM_MPU_TABLE0_V)<<(DPORT_ROM_MPU_TABLE0_S))
#define DPORT_ROM_MPU_TABLE0_V  0x3
#define DPORT_ROM_MPU_TABLE0_S  0

#define DPORT_ROM_MPU_TABLE1_REG          (DR_REG_DPORT_BASE + 0x498)
/* DPORT_ROM_MPU_TABLE1 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_ROM_MPU_TABLE1  0x00000003
#define DPORT_ROM_MPU_TABLE1_M  ((DPORT_ROM_MPU_TABLE1_V)<<(DPORT_ROM_MPU_TABLE1_S))
#define DPORT_ROM_MPU_TABLE1_V  0x3
#define DPORT_ROM_MPU_TABLE1_S  0

#define DPORT_ROM_MPU_TABLE2_REG          (DR_REG_DPORT_BASE + 0x49C)
/* DPORT_ROM_MPU_TABLE2 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_ROM_MPU_TABLE2  0x00000003
#define DPORT_ROM_MPU_TABLE2_M  ((DPORT_ROM_MPU_TABLE2_V)<<(DPORT_ROM_MPU_TABLE2_S))
#define DPORT_ROM_MPU_TABLE2_V  0x3
#define DPORT_ROM_MPU_TABLE2_S  0

#define DPORT_ROM_MPU_TABLE3_REG          (DR_REG_DPORT_BASE + 0x4A0)
/* DPORT_ROM_MPU_TABLE3 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_ROM_MPU_TABLE3  0x00000003
#define DPORT_ROM_MPU_TABLE3_M  ((DPORT_ROM_MPU_TABLE3_V)<<(DPORT_ROM_MPU_TABLE3_S))
#define DPORT_ROM_MPU_TABLE3_V  0x3
#define DPORT_ROM_MPU_TABLE3_S  0

#define DPORT_SHROM_MPU_TABLE0_REG          (DR_REG_DPORT_BASE + 0x4A4)
/* DPORT_SHROM_MPU_TABLE0 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE0  0x00000003
#define DPORT_SHROM_MPU_TABLE0_M  ((DPORT_SHROM_MPU_TABLE0_V)<<(DPORT_SHROM_MPU_TABLE0_S))
#define DPORT_SHROM_MPU_TABLE0_V  0x3
#define DPORT_SHROM_MPU_TABLE0_S  0

#define DPORT_SHROM_MPU_TABLE1_REG          (DR_REG_DPORT_BASE + 0x4A8)
/* DPORT_SHROM_MPU_TABLE1 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE1  0x00000003
#define DPORT_SHROM_MPU_TABLE1_M  ((DPORT_SHROM_MPU_TABLE1_V)<<(DPORT_SHROM_MPU_TABLE1_S))
#define DPORT_SHROM_MPU_TABLE1_V  0x3
#define DPORT_SHROM_MPU_TABLE1_S  0

#define DPORT_SHROM_MPU_TABLE2_REG          (DR_REG_DPORT_BASE + 0x4AC)
/* DPORT_SHROM_MPU_TABLE2 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE2  0x00000003
#define DPORT_SHROM_MPU_TABLE2_M  ((DPORT_SHROM_MPU_TABLE2_V)<<(DPORT_SHROM_MPU_TABLE2_S))
#define DPORT_SHROM_MPU_TABLE2_V  0x3
#define DPORT_SHROM_MPU_TABLE2_S  0

#define DPORT_SHROM_MPU_TABLE3_REG          (DR_REG_DPORT_BASE + 0x4B0)
/* DPORT_SHROM_MPU_TABLE3 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE3  0x00000003
#define DPORT_SHROM_MPU_TABLE3_M  ((DPORT_SHROM_MPU_TABLE3_V)<<(DPORT_SHROM_MPU_TABLE3_S))
#define DPORT_SHROM_MPU_TABLE3_V  0x3
#define DPORT_SHROM_MPU_TABLE3_S  0

#define DPORT_SHROM_MPU_TABLE4_REG          (DR_REG_DPORT_BASE + 0x4B4)
/* DPORT_SHROM_MPU_TABLE4 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE4  0x00000003
#define DPORT_SHROM_MPU_TABLE4_M  ((DPORT_SHROM_MPU_TABLE4_V)<<(DPORT_SHROM_MPU_TABLE4_S))
#define DPORT_SHROM_MPU_TABLE4_V  0x3
#define DPORT_SHROM_MPU_TABLE4_S  0

#define DPORT_SHROM_MPU_TABLE5_REG          (DR_REG_DPORT_BASE + 0x4B8)
/* DPORT_SHROM_MPU_TABLE5 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE5  0x00000003
#define DPORT_SHROM_MPU_TABLE5_M  ((DPORT_SHROM_MPU_TABLE5_V)<<(DPORT_SHROM_MPU_TABLE5_S))
#define DPORT_SHROM_MPU_TABLE5_V  0x3
#define DPORT_SHROM_MPU_TABLE5_S  0

#define DPORT_SHROM_MPU_TABLE6_REG          (DR_REG_DPORT_BASE + 0x4BC)
/* DPORT_SHROM_MPU_TABLE6 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE6  0x00000003
#define DPORT_SHROM_MPU_TABLE6_M  ((DPORT_SHROM_MPU_TABLE6_V)<<(DPORT_SHROM_MPU_TABLE6_S))
#define DPORT_SHROM_MPU_TABLE6_V  0x3
#define DPORT_SHROM_MPU_TABLE6_S  0

#define DPORT_SHROM_MPU_TABLE7_REG          (DR_REG_DPORT_BASE + 0x4C0)
/* DPORT_SHROM_MPU_TABLE7 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE7  0x00000003
#define DPORT_SHROM_MPU_TABLE7_M  ((DPORT_SHROM_MPU_TABLE7_V)<<(DPORT_SHROM_MPU_TABLE7_S))
#define DPORT_SHROM_MPU_TABLE7_V  0x3
#define DPORT_SHROM_MPU_TABLE7_S  0

#define DPORT_SHROM_MPU_TABLE8_REG          (DR_REG_DPORT_BASE + 0x4C4)
/* DPORT_SHROM_MPU_TABLE8 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE8  0x00000003
#define DPORT_SHROM_MPU_TABLE8_M  ((DPORT_SHROM_MPU_TABLE8_V)<<(DPORT_SHROM_MPU_TABLE8_S))
#define DPORT_SHROM_MPU_TABLE8_V  0x3
#define DPORT_SHROM_MPU_TABLE8_S  0

#define DPORT_SHROM_MPU_TABLE9_REG          (DR_REG_DPORT_BASE + 0x4C8)
/* DPORT_SHROM_MPU_TABLE9 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE9  0x00000003
#define DPORT_SHROM_MPU_TABLE9_M  ((DPORT_SHROM_MPU_TABLE9_V)<<(DPORT_SHROM_MPU_TABLE9_S))
#define DPORT_SHROM_MPU_TABLE9_V  0x3
#define DPORT_SHROM_MPU_TABLE9_S  0

#define DPORT_SHROM_MPU_TABLE10_REG          (DR_REG_DPORT_BASE + 0x4CC)
/* DPORT_SHROM_MPU_TABLE10 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE10  0x00000003
#define DPORT_SHROM_MPU_TABLE10_M  ((DPORT_SHROM_MPU_TABLE10_V)<<(DPORT_SHROM_MPU_TABLE10_S))
#define DPORT_SHROM_MPU_TABLE10_V  0x3
#define DPORT_SHROM_MPU_TABLE10_S  0

#define DPORT_SHROM_MPU_TABLE11_REG          (DR_REG_DPORT_BASE + 0x4D0)
/* DPORT_SHROM_MPU_TABLE11 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE11  0x00000003
#define DPORT_SHROM_MPU_TABLE11_M  ((DPORT_SHROM_MPU_TABLE11_V)<<(DPORT_SHROM_MPU_TABLE11_S))
#define DPORT_SHROM_MPU_TABLE11_V  0x3
#define DPORT_SHROM_MPU_TABLE11_S  0

#define DPORT_SHROM_MPU_TABLE12_REG          (DR_REG_DPORT_BASE + 0x4D4)
/* DPORT_SHROM_MPU_TABLE12 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE12  0x00000003
#define DPORT_SHROM_MPU_TABLE12_M  ((DPORT_SHROM_MPU_TABLE12_V)<<(DPORT_SHROM_MPU_TABLE12_S))
#define DPORT_SHROM_MPU_TABLE12_V  0x3
#define DPORT_SHROM_MPU_TABLE12_S  0

#define DPORT_SHROM_MPU_TABLE13_REG          (DR_REG_DPORT_BASE + 0x4D8)
/* DPORT_SHROM_MPU_TABLE13 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE13  0x00000003
#define DPORT_SHROM_MPU_TABLE13_M  ((DPORT_SHROM_MPU_TABLE13_V)<<(DPORT_SHROM_MPU_TABLE13_S))
#define DPORT_SHROM_MPU_TABLE13_V  0x3
#define DPORT_SHROM_MPU_TABLE13_S  0

#define DPORT_SHROM_MPU_TABLE14_REG          (DR_REG_DPORT_BASE + 0x4DC)
/* DPORT_SHROM_MPU_TABLE14 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE14  0x00000003
#define DPORT_SHROM_MPU_TABLE14_M  ((DPORT_SHROM_MPU_TABLE14_V)<<(DPORT_SHROM_MPU_TABLE14_S))
#define DPORT_SHROM_MPU_TABLE14_V  0x3
#define DPORT_SHROM_MPU_TABLE14_S  0

#define DPORT_SHROM_MPU_TABLE15_REG          (DR_REG_DPORT_BASE + 0x4E0)
/* DPORT_SHROM_MPU_TABLE15 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE15  0x00000003
#define DPORT_SHROM_MPU_TABLE15_M  ((DPORT_SHROM_MPU_TABLE15_V)<<(DPORT_SHROM_MPU_TABLE15_S))
#define DPORT_SHROM_MPU_TABLE15_V  0x3
#define DPORT_SHROM_MPU_TABLE15_S  0

#define DPORT_SHROM_MPU_TABLE16_REG          (DR_REG_DPORT_BASE + 0x4E4)
/* DPORT_SHROM_MPU_TABLE16 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE16  0x00000003
#define DPORT_SHROM_MPU_TABLE16_M  ((DPORT_SHROM_MPU_TABLE16_V)<<(DPORT_SHROM_MPU_TABLE16_S))
#define DPORT_SHROM_MPU_TABLE16_V  0x3
#define DPORT_SHROM_MPU_TABLE16_S  0

#define DPORT_SHROM_MPU_TABLE17_REG          (DR_REG_DPORT_BASE + 0x4E8)
/* DPORT_SHROM_MPU_TABLE17 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE17  0x00000003
#define DPORT_SHROM_MPU_TABLE17_M  ((DPORT_SHROM_MPU_TABLE17_V)<<(DPORT_SHROM_MPU_TABLE17_S))
#define DPORT_SHROM_MPU_TABLE17_V  0x3
#define DPORT_SHROM_MPU_TABLE17_S  0

#define DPORT_SHROM_MPU_TABLE18_REG          (DR_REG_DPORT_BASE + 0x4EC)
/* DPORT_SHROM_MPU_TABLE18 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE18  0x00000003
#define DPORT_SHROM_MPU_TABLE18_M  ((DPORT_SHROM_MPU_TABLE18_V)<<(DPORT_SHROM_MPU_TABLE18_S))
#define DPORT_SHROM_MPU_TABLE18_V  0x3
#define DPORT_SHROM_MPU_TABLE18_S  0

#define DPORT_SHROM_MPU_TABLE19_REG          (DR_REG_DPORT_BASE + 0x4F0)
/* DPORT_SHROM_MPU_TABLE19 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE19  0x00000003
#define DPORT_SHROM_MPU_TABLE19_M  ((DPORT_SHROM_MPU_TABLE19_V)<<(DPORT_SHROM_MPU_TABLE19_S))
#define DPORT_SHROM_MPU_TABLE19_V  0x3
#define DPORT_SHROM_MPU_TABLE19_S  0

#define DPORT_SHROM_MPU_TABLE20_REG          (DR_REG_DPORT_BASE + 0x4F4)
/* DPORT_SHROM_MPU_TABLE20 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE20  0x00000003
#define DPORT_SHROM_MPU_TABLE20_M  ((DPORT_SHROM_MPU_TABLE20_V)<<(DPORT_SHROM_MPU_TABLE20_S))
#define DPORT_SHROM_MPU_TABLE20_V  0x3
#define DPORT_SHROM_MPU_TABLE20_S  0

#define DPORT_SHROM_MPU_TABLE21_REG          (DR_REG_DPORT_BASE + 0x4F8)
/* DPORT_SHROM_MPU_TABLE21 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE21  0x00000003
#define DPORT_SHROM_MPU_TABLE21_M  ((DPORT_SHROM_MPU_TABLE21_V)<<(DPORT_SHROM_MPU_TABLE21_S))
#define DPORT_SHROM_MPU_TABLE21_V  0x3
#define DPORT_SHROM_MPU_TABLE21_S  0

#define DPORT_SHROM_MPU_TABLE22_REG          (DR_REG_DPORT_BASE + 0x4FC)
/* DPORT_SHROM_MPU_TABLE22 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE22  0x00000003
#define DPORT_SHROM_MPU_TABLE22_M  ((DPORT_SHROM_MPU_TABLE22_V)<<(DPORT_SHROM_MPU_TABLE22_S))
#define DPORT_SHROM_MPU_TABLE22_V  0x3
#define DPORT_SHROM_MPU_TABLE22_S  0

#define DPORT_SHROM_MPU_TABLE23_REG          (DR_REG_DPORT_BASE + 0x500)
/* DPORT_SHROM_MPU_TABLE23 : R/W ;bitpos:[1:0] ;default: 2'b1 ; */
/*description: */
#define DPORT_SHROM_MPU_TABLE23  0x00000003
#define DPORT_SHROM_MPU_TABLE23_M  ((DPORT_SHROM_MPU_TABLE23_V)<<(DPORT_SHROM_MPU_TABLE23_S))
#define DPORT_SHROM_MPU_TABLE23_V  0x3
#define DPORT_SHROM_MPU_TABLE23_S  0

#define DPORT_IMMU_TABLE0_REG          (DR_REG_DPORT_BASE + 0x504)
/* DPORT_IMMU_TABLE0 : R/W ;bitpos:[6:0] ;default: 7'd0 ; */
/*description: */
#define DPORT_IMMU_TABLE0  0x0000007F
#define DPORT_IMMU_TABLE0_M  ((DPORT_IMMU_TABLE0_V)<<(DPORT_IMMU_TABLE0_S))
#define DPORT_IMMU_TABLE0_V  0x7F
#define DPORT_IMMU_TABLE0_S  0

#define DPORT_IMMU_TABLE1_REG          (DR_REG_DPORT_BASE + 0x508)
/* DPORT_IMMU_TABLE1 : R/W ;bitpos:[6:0] ;default: 7'd1 ; */
/*description: */
#define DPORT_IMMU_TABLE1  0x0000007F
#define DPORT_IMMU_TABLE1_M  ((DPORT_IMMU_TABLE1_V)<<(DPORT_IMMU_TABLE1_S))
#define DPORT_IMMU_TABLE1_V  0x7F
#define DPORT_IMMU_TABLE1_S  0

#define DPORT_IMMU_TABLE2_REG          (DR_REG_DPORT_BASE + 0x50C)
/* DPORT_IMMU_TABLE2 : R/W ;bitpos:[6:0] ;default: 7'd2 ; */
/*description: */
#define DPORT_IMMU_TABLE2  0x0000007F
#define DPORT_IMMU_TABLE2_M  ((DPORT_IMMU_TABLE2_V)<<(DPORT_IMMU_TABLE2_S))
#define DPORT_IMMU_TABLE2_V  0x7F
#define DPORT_IMMU_TABLE2_S  0

#define DPORT_IMMU_TABLE3_REG          (DR_REG_DPORT_BASE + 0x510)
/* DPORT_IMMU_TABLE3 : R/W ;bitpos:[6:0] ;default: 7'd3 ; */
/*description: */
#define DPORT_IMMU_TABLE3  0x0000007F
#define DPORT_IMMU_TABLE3_M  ((DPORT_IMMU_TABLE3_V)<<(DPORT_IMMU_TABLE3_S))
#define DPORT_IMMU_TABLE3_V  0x7F
#define DPORT_IMMU_TABLE3_S  0

#define DPORT_IMMU_TABLE4_REG          (DR_REG_DPORT_BASE + 0x514)
/* DPORT_IMMU_TABLE4 : R/W ;bitpos:[6:0] ;default: 7'd4 ; */
/*description: */
#define DPORT_IMMU_TABLE4  0x0000007F
#define DPORT_IMMU_TABLE4_M  ((DPORT_IMMU_TABLE4_V)<<(DPORT_IMMU_TABLE4_S))
#define DPORT_IMMU_TABLE4_V  0x7F
#define DPORT_IMMU_TABLE4_S  0

#define DPORT_IMMU_TABLE5_REG          (DR_REG_DPORT_BASE + 0x518)
/* DPORT_IMMU_TABLE5 : R/W ;bitpos:[6:0] ;default: 7'd5 ; */
/*description: */
#define DPORT_IMMU_TABLE5  0x0000007F
#define DPORT_IMMU_TABLE5_M  ((DPORT_IMMU_TABLE5_V)<<(DPORT_IMMU_TABLE5_S))
#define DPORT_IMMU_TABLE5_V  0x7F
#define DPORT_IMMU_TABLE5_S  0

#define DPORT_IMMU_TABLE6_REG          (DR_REG_DPORT_BASE + 0x51C)
/* DPORT_IMMU_TABLE6 : R/W ;bitpos:[6:0] ;default: 7'd6 ; */
/*description: */
#define DPORT_IMMU_TABLE6  0x0000007F
#define DPORT_IMMU_TABLE6_M  ((DPORT_IMMU_TABLE6_V)<<(DPORT_IMMU_TABLE6_S))
#define DPORT_IMMU_TABLE6_V  0x7F
#define DPORT_IMMU_TABLE6_S  0

#define DPORT_IMMU_TABLE7_REG          (DR_REG_DPORT_BASE + 0x520)
/* DPORT_IMMU_TABLE7 : R/W ;bitpos:[6:0] ;default: 7'd7 ; */
/*description: */
#define DPORT_IMMU_TABLE7  0x0000007F
#define DPORT_IMMU_TABLE7_M  ((DPORT_IMMU_TABLE7_V)<<(DPORT_IMMU_TABLE7_S))
#define DPORT_IMMU_TABLE7_V  0x7F
#define DPORT_IMMU_TABLE7_S  0

#define DPORT_IMMU_TABLE8_REG          (DR_REG_DPORT_BASE + 0x524)
/* DPORT_IMMU_TABLE8 : R/W ;bitpos:[6:0] ;default: 7'd8 ; */
/*description: */
#define DPORT_IMMU_TABLE8  0x0000007F
#define DPORT_IMMU_TABLE8_M  ((DPORT_IMMU_TABLE8_V)<<(DPORT_IMMU_TABLE8_S))
#define DPORT_IMMU_TABLE8_V  0x7F
#define DPORT_IMMU_TABLE8_S  0

#define DPORT_IMMU_TABLE9_REG          (DR_REG_DPORT_BASE + 0x528)
/* DPORT_IMMU_TABLE9 : R/W ;bitpos:[6:0] ;default: 7'd9 ; */
/*description: */
#define DPORT_IMMU_TABLE9  0x0000007F
#define DPORT_IMMU_TABLE9_M  ((DPORT_IMMU_TABLE9_V)<<(DPORT_IMMU_TABLE9_S))
#define DPORT_IMMU_TABLE9_V  0x7F
#define DPORT_IMMU_TABLE9_S  0

#define DPORT_IMMU_TABLE10_REG          (DR_REG_DPORT_BASE + 0x52C)
/* DPORT_IMMU_TABLE10 : R/W ;bitpos:[6:0] ;default: 7'd10 ; */
/*description: */
#define DPORT_IMMU_TABLE10  0x0000007F
#define DPORT_IMMU_TABLE10_M  ((DPORT_IMMU_TABLE10_V)<<(DPORT_IMMU_TABLE10_S))
#define DPORT_IMMU_TABLE10_V  0x7F
#define DPORT_IMMU_TABLE10_S  0

#define DPORT_IMMU_TABLE11_REG          (DR_REG_DPORT_BASE + 0x530)
/* DPORT_IMMU_TABLE11 : R/W ;bitpos:[6:0] ;default: 7'd11 ; */
/*description: */
#define DPORT_IMMU_TABLE11  0x0000007F
#define DPORT_IMMU_TABLE11_M  ((DPORT_IMMU_TABLE11_V)<<(DPORT_IMMU_TABLE11_S))
#define DPORT_IMMU_TABLE11_V  0x7F
#define DPORT_IMMU_TABLE11_S  0

#define DPORT_IMMU_TABLE12_REG          (DR_REG_DPORT_BASE + 0x534)
/* DPORT_IMMU_TABLE12 : R/W ;bitpos:[6:0] ;default: 7'd12 ; */
/*description: */
#define DPORT_IMMU_TABLE12  0x0000007F
#define DPORT_IMMU_TABLE12_M  ((DPORT_IMMU_TABLE12_V)<<(DPORT_IMMU_TABLE12_S))
#define DPORT_IMMU_TABLE12_V  0x7F
#define DPORT_IMMU_TABLE12_S  0

#define DPORT_IMMU_TABLE13_REG          (DR_REG_DPORT_BASE + 0x538)
/* DPORT_IMMU_TABLE13 : R/W ;bitpos:[6:0] ;default: 7'd13 ; */
/*description: */
#define DPORT_IMMU_TABLE13  0x0000007F
#define DPORT_IMMU_TABLE13_M  ((DPORT_IMMU_TABLE13_V)<<(DPORT_IMMU_TABLE13_S))
#define DPORT_IMMU_TABLE13_V  0x7F
#define DPORT_IMMU_TABLE13_S  0

#define DPORT_IMMU_TABLE14_REG          (DR_REG_DPORT_BASE + 0x53C)
/* DPORT_IMMU_TABLE14 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: */
#define DPORT_IMMU_TABLE14  0x0000007F
#define DPORT_IMMU_TABLE14_M  ((DPORT_IMMU_TABLE14_V)<<(DPORT_IMMU_TABLE14_S))
#define DPORT_IMMU_TABLE14_V  0x7F
#define DPORT_IMMU_TABLE14_S  0

#define DPORT_IMMU_TABLE15_REG          (DR_REG_DPORT_BASE + 0x540)
/* DPORT_IMMU_TABLE15 : R/W ;bitpos:[6:0] ;default: 7'd15 ; */
/*description: */
#define DPORT_IMMU_TABLE15  0x0000007F
#define DPORT_IMMU_TABLE15_M  ((DPORT_IMMU_TABLE15_V)<<(DPORT_IMMU_TABLE15_S))
#define DPORT_IMMU_TABLE15_V  0x7F
#define DPORT_IMMU_TABLE15_S  0

#define DPORT_DMMU_TABLE0_REG          (DR_REG_DPORT_BASE + 0x544)
/* DPORT_DMMU_TABLE0 : R/W ;bitpos:[6:0] ;default: 7'd0 ; */
/*description: */
#define DPORT_DMMU_TABLE0  0x0000007F
#define DPORT_DMMU_TABLE0_M  ((DPORT_DMMU_TABLE0_V)<<(DPORT_DMMU_TABLE0_S))
#define DPORT_DMMU_TABLE0_V  0x7F
#define DPORT_DMMU_TABLE0_S  0

#define DPORT_DMMU_TABLE1_REG          (DR_REG_DPORT_BASE + 0x548)
/* DPORT_DMMU_TABLE1 : R/W ;bitpos:[6:0] ;default: 7'd1 ; */
/*description: */
#define DPORT_DMMU_TABLE1  0x0000007F
#define DPORT_DMMU_TABLE1_M  ((DPORT_DMMU_TABLE1_V)<<(DPORT_DMMU_TABLE1_S))
#define DPORT_DMMU_TABLE1_V  0x7F
#define DPORT_DMMU_TABLE1_S  0

#define DPORT_DMMU_TABLE2_REG          (DR_REG_DPORT_BASE + 0x54C)
/* DPORT_DMMU_TABLE2 : R/W ;bitpos:[6:0] ;default: 7'd2 ; */
/*description: */
#define DPORT_DMMU_TABLE2  0x0000007F
#define DPORT_DMMU_TABLE2_M  ((DPORT_DMMU_TABLE2_V)<<(DPORT_DMMU_TABLE2_S))
#define DPORT_DMMU_TABLE2_V  0x7F
#define DPORT_DMMU_TABLE2_S  0

#define DPORT_DMMU_TABLE3_REG          (DR_REG_DPORT_BASE + 0x550)
/* DPORT_DMMU_TABLE3 : R/W ;bitpos:[6:0] ;default: 7'd3 ; */
/*description: */
#define DPORT_DMMU_TABLE3  0x0000007F
#define DPORT_DMMU_TABLE3_M  ((DPORT_DMMU_TABLE3_V)<<(DPORT_DMMU_TABLE3_S))
#define DPORT_DMMU_TABLE3_V  0x7F
#define DPORT_DMMU_TABLE3_S  0

#define DPORT_DMMU_TABLE4_REG          (DR_REG_DPORT_BASE + 0x554)
/* DPORT_DMMU_TABLE4 : R/W ;bitpos:[6:0] ;default: 7'd4 ; */
/*description: */
#define DPORT_DMMU_TABLE4  0x0000007F
#define DPORT_DMMU_TABLE4_M  ((DPORT_DMMU_TABLE4_V)<<(DPORT_DMMU_TABLE4_S))
#define DPORT_DMMU_TABLE4_V  0x7F
#define DPORT_DMMU_TABLE4_S  0

#define DPORT_DMMU_TABLE5_REG          (DR_REG_DPORT_BASE + 0x558)
/* DPORT_DMMU_TABLE5 : R/W ;bitpos:[6:0] ;default: 7'd5 ; */
/*description: */
#define DPORT_DMMU_TABLE5  0x0000007F
#define DPORT_DMMU_TABLE5_M  ((DPORT_DMMU_TABLE5_V)<<(DPORT_DMMU_TABLE5_S))
#define DPORT_DMMU_TABLE5_V  0x7F
#define DPORT_DMMU_TABLE5_S  0

#define DPORT_DMMU_TABLE6_REG          (DR_REG_DPORT_BASE + 0x55C)
/* DPORT_DMMU_TABLE6 : R/W ;bitpos:[6:0] ;default: 7'd6 ; */
/*description: */
#define DPORT_DMMU_TABLE6  0x0000007F
#define DPORT_DMMU_TABLE6_M  ((DPORT_DMMU_TABLE6_V)<<(DPORT_DMMU_TABLE6_S))
#define DPORT_DMMU_TABLE6_V  0x7F
#define DPORT_DMMU_TABLE6_S  0

#define DPORT_DMMU_TABLE7_REG          (DR_REG_DPORT_BASE + 0x560)
/* DPORT_DMMU_TABLE7 : R/W ;bitpos:[6:0] ;default: 7'd7 ; */
/*description: */
#define DPORT_DMMU_TABLE7  0x0000007F
#define DPORT_DMMU_TABLE7_M  ((DPORT_DMMU_TABLE7_V)<<(DPORT_DMMU_TABLE7_S))
#define DPORT_DMMU_TABLE7_V  0x7F
#define DPORT_DMMU_TABLE7_S  0

#define DPORT_DMMU_TABLE8_REG          (DR_REG_DPORT_BASE + 0x564)
/* DPORT_DMMU_TABLE8 : R/W ;bitpos:[6:0] ;default: 7'd8 ; */
/*description: */
#define DPORT_DMMU_TABLE8  0x0000007F
#define DPORT_DMMU_TABLE8_M  ((DPORT_DMMU_TABLE8_V)<<(DPORT_DMMU_TABLE8_S))
#define DPORT_DMMU_TABLE8_V  0x7F
#define DPORT_DMMU_TABLE8_S  0

#define DPORT_DMMU_TABLE9_REG          (DR_REG_DPORT_BASE + 0x568)
/* DPORT_DMMU_TABLE9 : R/W ;bitpos:[6:0] ;default: 7'd9 ; */
/*description: */
#define DPORT_DMMU_TABLE9  0x0000007F
#define DPORT_DMMU_TABLE9_M  ((DPORT_DMMU_TABLE9_V)<<(DPORT_DMMU_TABLE9_S))
#define DPORT_DMMU_TABLE9_V  0x7F
#define DPORT_DMMU_TABLE9_S  0

#define DPORT_DMMU_TABLE10_REG          (DR_REG_DPORT_BASE + 0x56C)
/* DPORT_DMMU_TABLE10 : R/W ;bitpos:[6:0] ;default: 7'd10 ; */
/*description: */
#define DPORT_DMMU_TABLE10  0x0000007F
#define DPORT_DMMU_TABLE10_M  ((DPORT_DMMU_TABLE10_V)<<(DPORT_DMMU_TABLE10_S))
#define DPORT_DMMU_TABLE10_V  0x7F
#define DPORT_DMMU_TABLE10_S  0

#define DPORT_DMMU_TABLE11_REG          (DR_REG_DPORT_BASE + 0x570)
/* DPORT_DMMU_TABLE11 : R/W ;bitpos:[6:0] ;default: 7'd11 ; */
/*description: */
#define DPORT_DMMU_TABLE11  0x0000007F
#define DPORT_DMMU_TABLE11_M  ((DPORT_DMMU_TABLE11_V)<<(DPORT_DMMU_TABLE11_S))
#define DPORT_DMMU_TABLE11_V  0x7F
#define DPORT_DMMU_TABLE11_S  0

#define DPORT_DMMU_TABLE12_REG          (DR_REG_DPORT_BASE + 0x574)
/* DPORT_DMMU_TABLE12 : R/W ;bitpos:[6:0] ;default: 7'd12 ; */
/*description: */
#define DPORT_DMMU_TABLE12  0x0000007F
#define DPORT_DMMU_TABLE12_M  ((DPORT_DMMU_TABLE12_V)<<(DPORT_DMMU_TABLE12_S))
#define DPORT_DMMU_TABLE12_V  0x7F
#define DPORT_DMMU_TABLE12_S  0

#define DPORT_DMMU_TABLE13_REG          (DR_REG_DPORT_BASE + 0x578)
/* DPORT_DMMU_TABLE13 : R/W ;bitpos:[6:0] ;default: 7'd13 ; */
/*description: */
#define DPORT_DMMU_TABLE13  0x0000007F
#define DPORT_DMMU_TABLE13_M  ((DPORT_DMMU_TABLE13_V)<<(DPORT_DMMU_TABLE13_S))
#define DPORT_DMMU_TABLE13_V  0x7F
#define DPORT_DMMU_TABLE13_S  0

#define DPORT_DMMU_TABLE14_REG          (DR_REG_DPORT_BASE + 0x57C)
/* DPORT_DMMU_TABLE14 : R/W ;bitpos:[6:0] ;default: 7'd14 ; */
/*description: */
#define DPORT_DMMU_TABLE14  0x0000007F
#define DPORT_DMMU_TABLE14_M  ((DPORT_DMMU_TABLE14_V)<<(DPORT_DMMU_TABLE14_S))
#define DPORT_DMMU_TABLE14_V  0x7F
#define DPORT_DMMU_TABLE14_S  0

#define DPORT_DMMU_TABLE15_REG          (DR_REG_DPORT_BASE + 0x580)
/* DPORT_DMMU_TABLE15 : R/W ;bitpos:[6:0] ;default: 7'd15 ; */
/*description: */
#define DPORT_DMMU_TABLE15  0x0000007F
#define DPORT_DMMU_TABLE15_M  ((DPORT_DMMU_TABLE15_V)<<(DPORT_DMMU_TABLE15_S))
#define DPORT_DMMU_TABLE15_V  0x7F
#define DPORT_DMMU_TABLE15_S  0

#define DPORT_PRO_INTRUSION_CTRL_REG          (DR_REG_DPORT_BASE + 0x584)
/* DPORT_PRO_INTRUSION_RECORD_RESET_N : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_PRO_INTRUSION_RECORD_RESET_N  (BIT(0))
#define DPORT_PRO_INTRUSION_RECORD_RESET_N_M  (BIT(0))
#define DPORT_PRO_INTRUSION_RECORD_RESET_N_V  0x1
#define DPORT_PRO_INTRUSION_RECORD_RESET_N_S  0

#define DPORT_PRO_INTRUSION_STATUS_REG          (DR_REG_DPORT_BASE + 0x588)
/* DPORT_PRO_INTRUSION_RECORD : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: */
#define DPORT_PRO_INTRUSION_RECORD  0x0000000F
#define DPORT_PRO_INTRUSION_RECORD_M  ((DPORT_PRO_INTRUSION_RECORD_V)<<(DPORT_PRO_INTRUSION_RECORD_S))
#define DPORT_PRO_INTRUSION_RECORD_V  0xF
#define DPORT_PRO_INTRUSION_RECORD_S  0

#define DPORT_APP_INTRUSION_CTRL_REG          (DR_REG_DPORT_BASE + 0x58C)
/* DPORT_APP_INTRUSION_RECORD_RESET_N : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_APP_INTRUSION_RECORD_RESET_N  (BIT(0))
#define DPORT_APP_INTRUSION_RECORD_RESET_N_M  (BIT(0))
#define DPORT_APP_INTRUSION_RECORD_RESET_N_V  0x1
#define DPORT_APP_INTRUSION_RECORD_RESET_N_S  0

#define DPORT_APP_INTRUSION_STATUS_REG          (DR_REG_DPORT_BASE + 0x590)
/* DPORT_APP_INTRUSION_RECORD : RO ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: */
#define DPORT_APP_INTRUSION_RECORD  0x0000000F
#define DPORT_APP_INTRUSION_RECORD_M  ((DPORT_APP_INTRUSION_RECORD_V)<<(DPORT_APP_INTRUSION_RECORD_S))
#define DPORT_APP_INTRUSION_RECORD_V  0xF
#define DPORT_APP_INTRUSION_RECORD_S  0

#define DPORT_FRONT_END_MEM_PD_REG          (DR_REG_DPORT_BASE + 0x594)
/* DPORT_PBUS_MEM_FORCE_PD : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define DPORT_PBUS_MEM_FORCE_PD  (BIT(3))
#define DPORT_PBUS_MEM_FORCE_PD_M  (BIT(3))
#define DPORT_PBUS_MEM_FORCE_PD_V  0x1
#define DPORT_PBUS_MEM_FORCE_PD_S  3
/* DPORT_PBUS_MEM_FORCE_PU : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define DPORT_PBUS_MEM_FORCE_PU  (BIT(2))
#define DPORT_PBUS_MEM_FORCE_PU_M  (BIT(2))
#define DPORT_PBUS_MEM_FORCE_PU_V  0x1
#define DPORT_PBUS_MEM_FORCE_PU_S  2
/* DPORT_AGC_MEM_FORCE_PD : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define DPORT_AGC_MEM_FORCE_PD  (BIT(1))
#define DPORT_AGC_MEM_FORCE_PD_M  (BIT(1))
#define DPORT_AGC_MEM_FORCE_PD_V  0x1
#define DPORT_AGC_MEM_FORCE_PD_S  1
/* DPORT_AGC_MEM_FORCE_PU : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define DPORT_AGC_MEM_FORCE_PU  (BIT(0))
#define DPORT_AGC_MEM_FORCE_PU_M  (BIT(0))
#define DPORT_AGC_MEM_FORCE_PU_V  0x1
#define DPORT_AGC_MEM_FORCE_PU_S  0

#define DPORT_MMU_IA_INT_EN_REG          (DR_REG_DPORT_BASE + 0x598)
/* DPORT_MMU_IA_INT_EN : R/W ;bitpos:[23:0] ;default: 24'b0 ; */
/*description: */
#define DPORT_MMU_IA_INT_EN  0x00FFFFFF
#define DPORT_MMU_IA_INT_EN_M  ((DPORT_MMU_IA_INT_EN_V)<<(DPORT_MMU_IA_INT_EN_S))
#define DPORT_MMU_IA_INT_EN_V  0xFFFFFF
#define DPORT_MMU_IA_INT_EN_S  0

#define DPORT_MPU_IA_INT_EN_REG          (DR_REG_DPORT_BASE + 0x59C)
/* DPORT_MPU_IA_INT_EN : R/W ;bitpos:[16:0] ;default: 17'b0 ; */
/*description: */
#define DPORT_MPU_IA_INT_EN  0x0001FFFF
#define DPORT_MPU_IA_INT_EN_M  ((DPORT_MPU_IA_INT_EN_V)<<(DPORT_MPU_IA_INT_EN_S))
#define DPORT_MPU_IA_INT_EN_V  0x1FFFF
#define DPORT_MPU_IA_INT_EN_S  0

#define DPORT_CACHE_IA_INT_EN_REG          (DR_REG_DPORT_BASE + 0x5A0)
/* DPORT_CACHE_IA_INT_EN : R/W ;bitpos:[27:0] ;default: 28'b0 ; */
/*description: */
#define DPORT_CACHE_IA_INT_EN  0x0FFFFFFF
#define DPORT_CACHE_IA_INT_EN_M  ((DPORT_CACHE_IA_INT_EN_V)<<(DPORT_CACHE_IA_INT_EN_S))
#define DPORT_CACHE_IA_INT_EN_V  0xFFFFFFF
#define DPORT_CACHE_IA_INT_EN_S  0

#define DPORT_SECURE_BOOT_CTRL_REG          (DR_REG_DPORT_BASE + 0x5A4)
/* DPORT_SW_BOOTLOADER_SEL : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define DPORT_SW_BOOTLOADER_SEL  (BIT(0))
#define DPORT_SW_BOOTLOADER_SEL_M  (BIT(0))
#define DPORT_SW_BOOTLOADER_SEL_V  0x1
#define DPORT_SW_BOOTLOADER_SEL_S  0

#define DPORT_SPI_DMA_CHAN_SEL_REG          (DR_REG_DPORT_BASE + 0x5A8)
/* DPORT_SPI3_DMA_CHAN_SEL : R/W ;bitpos:[5:4] ;default: 2'b00 ; */
/*description: */
#define DPORT_SPI3_DMA_CHAN_SEL  0x00000003
#define DPORT_SPI3_DMA_CHAN_SEL_M  ((DPORT_SPI3_DMA_CHAN_SEL_V)<<(DPORT_SPI3_DMA_CHAN_SEL_S))
#define DPORT_SPI3_DMA_CHAN_SEL_V  0x3
#define DPORT_SPI3_DMA_CHAN_SEL_S  4
/* DPORT_SPI2_DMA_CHAN_SEL : R/W ;bitpos:[3:2] ;default: 2'b00 ; */
/*description: */
#define DPORT_SPI2_DMA_CHAN_SEL  0x00000003
#define DPORT_SPI2_DMA_CHAN_SEL_M  ((DPORT_SPI2_DMA_CHAN_SEL_V)<<(DPORT_SPI2_DMA_CHAN_SEL_S))
#define DPORT_SPI2_DMA_CHAN_SEL_V  0x3
#define DPORT_SPI2_DMA_CHAN_SEL_S  2
/* DPORT_SPI1_DMA_CHAN_SEL : R/W ;bitpos:[1:0] ;default: 2'b00 ; */
/*description: */
#define DPORT_SPI1_DMA_CHAN_SEL  0x00000003
#define DPORT_SPI1_DMA_CHAN_SEL_M  ((DPORT_SPI1_DMA_CHAN_SEL_V)<<(DPORT_SPI1_DMA_CHAN_SEL_S))
#define DPORT_SPI1_DMA_CHAN_SEL_V  0x3
#define DPORT_SPI1_DMA_CHAN_SEL_S  0

#define DPORT_PRO_VECBASE_CTRL_REG          (DR_REG_DPORT_BASE + 0x5AC)
/* DPORT_PRO_OUT_VECBASE_SEL : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define DPORT_PRO_OUT_VECBASE_SEL  0x00000003
#define DPORT_PRO_OUT_VECBASE_SEL_M  ((DPORT_PRO_OUT_VECBASE_SEL_V)<<(DPORT_PRO_OUT_VECBASE_SEL_S))
#define DPORT_PRO_OUT_VECBASE_SEL_V  0x3
#define DPORT_PRO_OUT_VECBASE_SEL_S  0

#define DPORT_PRO_VECBASE_SET_REG          (DR_REG_DPORT_BASE + 0x5B0)
/* DPORT_PRO_OUT_VECBASE_REG : R/W ;bitpos:[21:0] ;default: 22'b0 ; */
/*description: */
#define DPORT_PRO_OUT_VECBASE_REG  0x003FFFFF
#define DPORT_PRO_OUT_VECBASE_REG_M  ((DPORT_PRO_OUT_VECBASE_REG_V)<<(DPORT_PRO_OUT_VECBASE_REG_S))
#define DPORT_PRO_OUT_VECBASE_REG_V  0x3FFFFF
#define DPORT_PRO_OUT_VECBASE_REG_S  0

#define DPORT_APP_VECBASE_CTRL_REG          (DR_REG_DPORT_BASE + 0x5B4)
/* DPORT_APP_OUT_VECBASE_SEL : R/W ;bitpos:[1:0] ;default: 2'b0 ; */
/*description: */
#define DPORT_APP_OUT_VECBASE_SEL  0x00000003
#define DPORT_APP_OUT_VECBASE_SEL_M  ((DPORT_APP_OUT_VECBASE_SEL_V)<<(DPORT_APP_OUT_VECBASE_SEL_S))
#define DPORT_APP_OUT_VECBASE_SEL_V  0x3
#define DPORT_APP_OUT_VECBASE_SEL_S  0

#define DPORT_APP_VECBASE_SET_REG          (DR_REG_DPORT_BASE + 0x5B8)
/* DPORT_APP_OUT_VECBASE_REG : R/W ;bitpos:[21:0] ;default: 22'b0 ; */
/*description: */
#define DPORT_APP_OUT_VECBASE_REG  0x003FFFFF
#define DPORT_APP_OUT_VECBASE_REG_M  ((DPORT_APP_OUT_VECBASE_REG_V)<<(DPORT_APP_OUT_VECBASE_REG_S))
#define DPORT_APP_OUT_VECBASE_REG_V  0x3FFFFF
#define DPORT_APP_OUT_VECBASE_REG_S  0

#define DPORT_DATE_REG          (DR_REG_DPORT_BASE + 0xFFC)
/* DPORT_DATE : R/W ;bitpos:[27:0] ;default: 28'h1605190 ; */
/*description: */
#define DPORT_DATE  0x0FFFFFFF
#define DPORT_DATE_M  ((DPORT_DATE_V)<<(DPORT_DATE_S))
#define DPORT_DATE_V  0xFFFFFFF
#define DPORT_DATE_S  0
#define DPORT_DPORT_DATE_VERSION 0x1605190

/* Flash MMU table for PRO CPU */
#define DPORT_PRO_FLASH_MMU_TABLE ((volatile uint32_t*) 0x3FF10000)

/* Flash MMU table for APP CPU */
#define DPORT_APP_FLASH_MMU_TABLE ((volatile uint32_t*) 0x3FF12000)



#endif /*_SOC_DPORT_REG_H_ */


