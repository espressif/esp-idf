/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SYSTEM_REG_H_
#define _SOC_SYSTEM_REG_H_

#include "soc/soc.h"
#include "soc/clkrst_reg.h"

#ifdef __cplusplus
extern "C" {
#endif
#define SYSTEM_CPU_PERI_CLK_EN_REG          (DR_REG_SYSTEM_BASE + 0x000)
/* SYSTEM_CLK_EN_DEDICATED_GPIO : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_CLK_EN_DEDICATED_GPIO  (BIT(7))
#define SYSTEM_CLK_EN_DEDICATED_GPIO_M  (BIT(7))
#define SYSTEM_CLK_EN_DEDICATED_GPIO_V  0x1
#define SYSTEM_CLK_EN_DEDICATED_GPIO_S  7
/* SYSTEM_CLK_EN_ASSIST_DEBUG : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_CLK_EN_ASSIST_DEBUG  (BIT(6))
#define SYSTEM_CLK_EN_ASSIST_DEBUG_M  (BIT(6))
#define SYSTEM_CLK_EN_ASSIST_DEBUG_V  0x1
#define SYSTEM_CLK_EN_ASSIST_DEBUG_S  6

#define SYSTEM_CPU_PERI_RST_EN_REG          (DR_REG_SYSTEM_BASE + 0x004)
/* SYSTEM_RST_EN_DEDICATED_GPIO : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_RST_EN_DEDICATED_GPIO  (BIT(7))
#define SYSTEM_RST_EN_DEDICATED_GPIO_M  (BIT(7))
#define SYSTEM_RST_EN_DEDICATED_GPIO_V  0x1
#define SYSTEM_RST_EN_DEDICATED_GPIO_S  7
/* SYSTEM_RST_EN_ASSIST_DEBUG : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_RST_EN_ASSIST_DEBUG  (BIT(6))
#define SYSTEM_RST_EN_ASSIST_DEBUG_M  (BIT(6))
#define SYSTEM_RST_EN_ASSIST_DEBUG_V  0x1
#define SYSTEM_RST_EN_ASSIST_DEBUG_S  6

#define SYSTEM_CPU_PER_CONF_REG          (DR_REG_SYSTEM_BASE + 0x008)
/* SYSTEM_CPU_WAITI_DELAY_NUM : R/W ;bitpos:[7:4] ;default: 4'h0 ; */
/*description: */
#define SYSTEM_CPU_WAITI_DELAY_NUM  0x0000000F
#define SYSTEM_CPU_WAITI_DELAY_NUM_M  ((SYSTEM_CPU_WAITI_DELAY_NUM_V)<<(SYSTEM_CPU_WAITI_DELAY_NUM_S))
#define SYSTEM_CPU_WAITI_DELAY_NUM_V  0xF
#define SYSTEM_CPU_WAITI_DELAY_NUM_S  4
/* SYSTEM_CPU_WAIT_MODE_FORCE_ON : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_CPU_WAIT_MODE_FORCE_ON  (BIT(3))
#define SYSTEM_CPU_WAIT_MODE_FORCE_ON_M  (BIT(3))
#define SYSTEM_CPU_WAIT_MODE_FORCE_ON_V  0x1
#define SYSTEM_CPU_WAIT_MODE_FORCE_ON_S  3

#define SYSTEM_MEM_PD_MASK_REG          (DR_REG_SYSTEM_BASE + 0x00C)
/* SYSTEM_LSLP_MEM_PD_MASK : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_LSLP_MEM_PD_MASK  (BIT(0))
#define SYSTEM_LSLP_MEM_PD_MASK_M  (BIT(0))
#define SYSTEM_LSLP_MEM_PD_MASK_V  0x1
#define SYSTEM_LSLP_MEM_PD_MASK_S  0

#define SYSTEM_CPU_INTR_FROM_CPU_0_REG          (DR_REG_SYSTEM_BASE + 0x010)
/* SYSTEM_CPU_INTR_FROM_CPU_0 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_CPU_INTR_FROM_CPU_0  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_0_M  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_0_V  0x1
#define SYSTEM_CPU_INTR_FROM_CPU_0_S  0

#define SYSTEM_CPU_INTR_FROM_CPU_1_REG          (DR_REG_SYSTEM_BASE + 0x014)
/* SYSTEM_CPU_INTR_FROM_CPU_1 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_CPU_INTR_FROM_CPU_1  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_1_M  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_1_V  0x1
#define SYSTEM_CPU_INTR_FROM_CPU_1_S  0

#define SYSTEM_CPU_INTR_FROM_CPU_2_REG          (DR_REG_SYSTEM_BASE + 0x018)
/* SYSTEM_CPU_INTR_FROM_CPU_2 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_CPU_INTR_FROM_CPU_2  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_2_M  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_2_V  0x1
#define SYSTEM_CPU_INTR_FROM_CPU_2_S  0

#define SYSTEM_CPU_INTR_FROM_CPU_3_REG          (DR_REG_SYSTEM_BASE + 0x01C)
/* SYSTEM_CPU_INTR_FROM_CPU_3 : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_CPU_INTR_FROM_CPU_3  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_3_M  (BIT(0))
#define SYSTEM_CPU_INTR_FROM_CPU_3_V  0x1
#define SYSTEM_CPU_INTR_FROM_CPU_3_S  0

#define SYSTEM_RSA_PD_CTRL_REG          (DR_REG_SYSTEM_BASE + 0x020)
/* SYSTEM_RSA_MEM_FORCE_PD : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_RSA_MEM_FORCE_PD  (BIT(2))
#define SYSTEM_RSA_MEM_FORCE_PD_M  (BIT(2))
#define SYSTEM_RSA_MEM_FORCE_PD_V  0x1
#define SYSTEM_RSA_MEM_FORCE_PD_S  2
/* SYSTEM_RSA_MEM_FORCE_PU : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_RSA_MEM_FORCE_PU  (BIT(1))
#define SYSTEM_RSA_MEM_FORCE_PU_M  (BIT(1))
#define SYSTEM_RSA_MEM_FORCE_PU_V  0x1
#define SYSTEM_RSA_MEM_FORCE_PU_S  1
/* SYSTEM_RSA_MEM_PD : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_RSA_MEM_PD  (BIT(0))
#define SYSTEM_RSA_MEM_PD_M  (BIT(0))
#define SYSTEM_RSA_MEM_PD_V  0x1
#define SYSTEM_RSA_MEM_PD_S  0

#define SYSTEM_EDMA_CTRL_REG          (DR_REG_SYSTEM_BASE + 0x024)
/* SYSTEM_EDMA_RESET : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_EDMA_RESET  (BIT(1))
#define SYSTEM_EDMA_RESET_M  (BIT(1))
#define SYSTEM_EDMA_RESET_V  0x1
#define SYSTEM_EDMA_RESET_S  1
/* SYSTEM_EDMA_CLK_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_EDMA_CLK_ON  (BIT(0))
#define SYSTEM_EDMA_CLK_ON_M  (BIT(0))
#define SYSTEM_EDMA_CLK_ON_V  0x1
#define SYSTEM_EDMA_CLK_ON_S  0

#define SYSTEM_CACHE_CONTROL_REG          (DR_REG_SYSTEM_BASE + 0x028)
/* SYSTEM_DCACHE_RESET : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_DCACHE_RESET  (BIT(3))
#define SYSTEM_DCACHE_RESET_M  (BIT(3))
#define SYSTEM_DCACHE_RESET_V  0x1
#define SYSTEM_DCACHE_RESET_S  3
/* SYSTEM_DCACHE_CLK_ON : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_DCACHE_CLK_ON  (BIT(2))
#define SYSTEM_DCACHE_CLK_ON_M  (BIT(2))
#define SYSTEM_DCACHE_CLK_ON_V  0x1
#define SYSTEM_DCACHE_CLK_ON_S  2
/* SYSTEM_ICACHE_RESET : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_ICACHE_RESET  (BIT(1))
#define SYSTEM_ICACHE_RESET_M  (BIT(1))
#define SYSTEM_ICACHE_RESET_V  0x1
#define SYSTEM_ICACHE_RESET_S  1
/* SYSTEM_ICACHE_CLK_ON : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_ICACHE_CLK_ON  (BIT(0))
#define SYSTEM_ICACHE_CLK_ON_M  (BIT(0))
#define SYSTEM_ICACHE_CLK_ON_V  0x1
#define SYSTEM_ICACHE_CLK_ON_S  0

#define SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG          (DR_REG_SYSTEM_BASE + 0x02C)
/* SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT  (BIT(3))
#define SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_M  (BIT(3))
#define SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_V  0x1
#define SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT_S  3
/* SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT  (BIT(2))
#define SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_M  (BIT(2))
#define SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_V  0x1
#define SYSTEM_ENABLE_DOWNLOAD_G0CB_DECRYPT_S  2
/* SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT : R/W ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT  (BIT(1))
#define SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_M  (BIT(1))
#define SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_V  0x1
#define SYSTEM_ENABLE_DOWNLOAD_DB_ENCRYPT_S  1
/* SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT  (BIT(0))
#define SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_M  (BIT(0))
#define SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_V  0x1
#define SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT_S  0

#define SYSTEM_RTC_FASTMEM_CONFIG_REG          (DR_REG_SYSTEM_BASE + 0x030)
/* SYSTEM_RTC_MEM_CRC_FINISH : RO ;bitpos:[31] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_RTC_MEM_CRC_FINISH  (BIT(31))
#define SYSTEM_RTC_MEM_CRC_FINISH_M  (BIT(31))
#define SYSTEM_RTC_MEM_CRC_FINISH_V  0x1
#define SYSTEM_RTC_MEM_CRC_FINISH_S  31
/* SYSTEM_RTC_MEM_CRC_LEN : R/W ;bitpos:[30:20] ;default: 11'h7ff ; */
/*description: */
#define SYSTEM_RTC_MEM_CRC_LEN  0x000007FF
#define SYSTEM_RTC_MEM_CRC_LEN_M  ((SYSTEM_RTC_MEM_CRC_LEN_V)<<(SYSTEM_RTC_MEM_CRC_LEN_S))
#define SYSTEM_RTC_MEM_CRC_LEN_V  0x7FF
#define SYSTEM_RTC_MEM_CRC_LEN_S  20
/* SYSTEM_RTC_MEM_CRC_ADDR : R/W ;bitpos:[19:9] ;default: 11'h0 ; */
/*description: */
#define SYSTEM_RTC_MEM_CRC_ADDR  0x000007FF
#define SYSTEM_RTC_MEM_CRC_ADDR_M  ((SYSTEM_RTC_MEM_CRC_ADDR_V)<<(SYSTEM_RTC_MEM_CRC_ADDR_S))
#define SYSTEM_RTC_MEM_CRC_ADDR_V  0x7FF
#define SYSTEM_RTC_MEM_CRC_ADDR_S  9
/* SYSTEM_RTC_MEM_CRC_START : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_RTC_MEM_CRC_START  (BIT(8))
#define SYSTEM_RTC_MEM_CRC_START_M  (BIT(8))
#define SYSTEM_RTC_MEM_CRC_START_V  0x1
#define SYSTEM_RTC_MEM_CRC_START_S  8

#define SYSTEM_RTC_FASTMEM_CRC_REG          (DR_REG_SYSTEM_BASE + 0x034)
/* SYSTEM_RTC_MEM_CRC_RES : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: */
#define SYSTEM_RTC_MEM_CRC_RES  0xFFFFFFFF
#define SYSTEM_RTC_MEM_CRC_RES_M  ((SYSTEM_RTC_MEM_CRC_RES_V)<<(SYSTEM_RTC_MEM_CRC_RES_S))
#define SYSTEM_RTC_MEM_CRC_RES_V  0xFFFFFFFF
#define SYSTEM_RTC_MEM_CRC_RES_S  0

#define SYSTEM_REDUNDANT_ECO_CTRL_REG          (DR_REG_SYSTEM_BASE + 0x038)
/* SYSTEM_REDUNDANT_ECO_RESULT : RO ;bitpos:[1] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_REDUNDANT_ECO_RESULT  (BIT(1))
#define SYSTEM_REDUNDANT_ECO_RESULT_M  (BIT(1))
#define SYSTEM_REDUNDANT_ECO_RESULT_V  0x1
#define SYSTEM_REDUNDANT_ECO_RESULT_S  1
/* SYSTEM_REDUNDANT_ECO_DRIVE : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_REDUNDANT_ECO_DRIVE  (BIT(0))
#define SYSTEM_REDUNDANT_ECO_DRIVE_M  (BIT(0))
#define SYSTEM_REDUNDANT_ECO_DRIVE_V  0x1
#define SYSTEM_REDUNDANT_ECO_DRIVE_S  0

#define SYSTEM_CLOCK_GATE_REG          (DR_REG_SYSTEM_BASE + 0x03C)
/* SYSTEM_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: */
#define SYSTEM_CLK_EN  (BIT(0))
#define SYSTEM_CLK_EN_M  (BIT(0))
#define SYSTEM_CLK_EN_V  0x1
#define SYSTEM_CLK_EN_S  0

#define SYSTEM_MEM_PVT_REG          (DR_REG_SYSTEM_BASE + 0x040)
/* SYSTEM_MEM_VT_SEL : R/W ;bitpos:[23:22] ;default: 2'd0 ; */
/*description: */
#define SYSTEM_MEM_VT_SEL  0x00000003
#define SYSTEM_MEM_VT_SEL_M  ((SYSTEM_MEM_VT_SEL_V)<<(SYSTEM_MEM_VT_SEL_S))
#define SYSTEM_MEM_VT_SEL_V  0x3
#define SYSTEM_MEM_VT_SEL_S  22
/* SYSTEM_MEM_TIMING_ERR_CNT : RO ;bitpos:[21:6] ;default: 16'h0 ; */
/*description: */
#define SYSTEM_MEM_TIMING_ERR_CNT  0x0000FFFF
#define SYSTEM_MEM_TIMING_ERR_CNT_M  ((SYSTEM_MEM_TIMING_ERR_CNT_V)<<(SYSTEM_MEM_TIMING_ERR_CNT_S))
#define SYSTEM_MEM_TIMING_ERR_CNT_V  0xFFFF
#define SYSTEM_MEM_TIMING_ERR_CNT_S  6
/* SYSTEM_MEM_PVT_MONITOR_EN : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_MEM_PVT_MONITOR_EN  (BIT(5))
#define SYSTEM_MEM_PVT_MONITOR_EN_M  (BIT(5))
#define SYSTEM_MEM_PVT_MONITOR_EN_V  0x1
#define SYSTEM_MEM_PVT_MONITOR_EN_S  5
/* SYSTEM_MEM_ERR_CNT_CLR : WO ;bitpos:[4] ;default: 1'b0 ; */
/*description: */
#define SYSTEM_MEM_ERR_CNT_CLR  (BIT(4))
#define SYSTEM_MEM_ERR_CNT_CLR_M  (BIT(4))
#define SYSTEM_MEM_ERR_CNT_CLR_V  0x1
#define SYSTEM_MEM_ERR_CNT_CLR_S  4
/* SYSTEM_MEM_PATH_LEN : R/W ;bitpos:[3:0] ;default: 4'h3 ; */
/*description: */
#define SYSTEM_MEM_PATH_LEN  0x0000000F
#define SYSTEM_MEM_PATH_LEN_M  ((SYSTEM_MEM_PATH_LEN_V)<<(SYSTEM_MEM_PATH_LEN_S))
#define SYSTEM_MEM_PATH_LEN_V  0xF
#define SYSTEM_MEM_PATH_LEN_S  0

#define SYSTEM_DATE_REG          (DR_REG_SYSTEM_BASE + 0xFFC)
/* SYSTEM_DATE : R/W ;bitpos:[27:0] ;default: 28'h2103240 ; */
/*description: */
#define SYSTEM_DATE  0x0FFFFFFF
#define SYSTEM_DATE_M  ((SYSTEM_DATE_V)<<(SYSTEM_DATE_S))
#define SYSTEM_DATE_V  0xFFFFFFF
#define SYSTEM_DATE_S  0

#ifdef __cplusplus
}
#endif



#endif /*_SOC_SYSTEM_REG_H_ */
