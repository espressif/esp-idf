/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_MEM_MONITOR_REG_H_
#define _SOC_MEM_MONITOR_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

#define MEM_MONITOR_LOG_SETTING_REG          (DR_REG_MEM_MONITOR_BASE + 0x0)
/* MEM_MONITOR_LOG_DMA_1_ENA : R/W ;bitpos:[31:24] ;default: 8'b0 ; */
/*description: enable dma_1 log.*/
#define MEM_MONITOR_LOG_DMA_1_ENA    0x000000FF
#define MEM_MONITOR_LOG_DMA_1_ENA_M  ((MEM_MONITOR_LOG_DMA_1_ENA_V)<<(MEM_MONITOR_LOG_DMA_1_ENA_S))
#define MEM_MONITOR_LOG_DMA_1_ENA_V  0xFF
#define MEM_MONITOR_LOG_DMA_1_ENA_S  24
/* MEM_MONITOR_LOG_DMA_0_ENA : R/W ;bitpos:[23:16] ;default: 8'b0 ; */
/*description: enable dma_0 log.*/
#define MEM_MONITOR_LOG_DMA_0_ENA    0x000000FF
#define MEM_MONITOR_LOG_DMA_0_ENA_M  ((MEM_MONITOR_LOG_DMA_0_ENA_V)<<(MEM_MONITOR_LOG_DMA_0_ENA_S))
#define MEM_MONITOR_LOG_DMA_0_ENA_V  0xFF
#define MEM_MONITOR_LOG_DMA_0_ENA_S  16
/* MEM_MONITOR_LOG_CORE_ENA : R/W ;bitpos:[15:8] ;default: 8'b0 ; */
/*description: enable core log.*/
#define MEM_MONITOR_LOG_CORE_ENA    0x000000FF
#define MEM_MONITOR_LOG_CORE_ENA_M  ((MEM_MONITOR_LOG_CORE_ENA_V)<<(MEM_MONITOR_LOG_CORE_ENA_S))
#define MEM_MONITOR_LOG_CORE_ENA_V  0xFF
#define MEM_MONITOR_LOG_CORE_ENA_S  8
/* MEM_MONITOR_LOG_MEM_LOOP_ENABLE : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: Set 1 enable mem_loop, it will loop write at the range of MEM_START and MEM_END.*/
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE    (BIT(4))
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_M  (BIT(4))
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_V  0x1
#define MEM_MONITOR_LOG_MEM_LOOP_ENABLE_S  4
/* MEM_MONITOR_LOG_MODE : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: Bit[0] : WR monitor, BIT[1]: WORD monitor, BIT[2]: HALFWORD monitor, BIT[3]: BYT
E monitor.*/
#define MEM_MONITOR_LOG_MODE    0x0000000F
#define MEM_MONITOR_LOG_MODE_M  ((MEM_MONITOR_LOG_MODE_V)<<(MEM_MONITOR_LOG_MODE_S))
#define MEM_MONITOR_LOG_MODE_V  0xF
#define MEM_MONITOR_LOG_MODE_S  0

#define MEM_MONITOR_LOG_SETTING1_REG          (DR_REG_MEM_MONITOR_BASE + 0x4)
/* MEM_MONITOR_LOG_DMA_3_ENA : R/W ;bitpos:[15:8] ;default: 8'b0 ; */
/*description: enable dma_3 log.*/
#define MEM_MONITOR_LOG_DMA_3_ENA    0x000000FF
#define MEM_MONITOR_LOG_DMA_3_ENA_M  ((MEM_MONITOR_LOG_DMA_3_ENA_V)<<(MEM_MONITOR_LOG_DMA_3_ENA_S))
#define MEM_MONITOR_LOG_DMA_3_ENA_V  0xFF
#define MEM_MONITOR_LOG_DMA_3_ENA_S  8
/* MEM_MONITOR_LOG_DMA_2_ENA : R/W ;bitpos:[7:0] ;default: 8'b0 ; */
/*description: enable dma_2 log.*/
#define MEM_MONITOR_LOG_DMA_2_ENA    0x000000FF
#define MEM_MONITOR_LOG_DMA_2_ENA_M  ((MEM_MONITOR_LOG_DMA_2_ENA_V)<<(MEM_MONITOR_LOG_DMA_2_ENA_S))
#define MEM_MONITOR_LOG_DMA_2_ENA_V  0xFF
#define MEM_MONITOR_LOG_DMA_2_ENA_S  0

#define MEM_MONITOR_LOG_CHECK_DATA_REG          (DR_REG_MEM_MONITOR_BASE + 0x8)
/* MEM_MONITOR_LOG_CHECK_DATA : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: The special check data, when write this special data, it will trigger logging..*/
#define MEM_MONITOR_LOG_CHECK_DATA    0xFFFFFFFF
#define MEM_MONITOR_LOG_CHECK_DATA_M  ((MEM_MONITOR_LOG_CHECK_DATA_V)<<(MEM_MONITOR_LOG_CHECK_DATA_S))
#define MEM_MONITOR_LOG_CHECK_DATA_V  0xFFFFFFFF
#define MEM_MONITOR_LOG_CHECK_DATA_S  0

#define MEM_MONITOR_LOG_DATA_MASK_REG          (DR_REG_MEM_MONITOR_BASE + 0xC)
/* MEM_MONITOR_LOG_DATA_MASK : R/W ;bitpos:[3:0] ;default: 4'b0 ; */
/*description: byte mask enable, BIT0 mask the first byte of MEM_MONITOR_LOG_CHECK_DATA, and BI
T1 mask second byte, and so on..*/
#define MEM_MONITOR_LOG_DATA_MASK    0x0000000F
#define MEM_MONITOR_LOG_DATA_MASK_M  ((MEM_MONITOR_LOG_DATA_MASK_V)<<(MEM_MONITOR_LOG_DATA_MASK_S))
#define MEM_MONITOR_LOG_DATA_MASK_V  0xF
#define MEM_MONITOR_LOG_DATA_MASK_S  0

#define MEM_MONITOR_LOG_MIN_REG          (DR_REG_MEM_MONITOR_BASE + 0x10)
/* MEM_MONITOR_LOG_MIN : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: the min address of log range.*/
#define MEM_MONITOR_LOG_MIN    0xFFFFFFFF
#define MEM_MONITOR_LOG_MIN_M  ((MEM_MONITOR_LOG_MIN_V)<<(MEM_MONITOR_LOG_MIN_S))
#define MEM_MONITOR_LOG_MIN_V  0xFFFFFFFF
#define MEM_MONITOR_LOG_MIN_S  0

#define MEM_MONITOR_LOG_MAX_REG          (DR_REG_MEM_MONITOR_BASE + 0x14)
/* MEM_MONITOR_LOG_MAX : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: the max address of log range.*/
#define MEM_MONITOR_LOG_MAX    0xFFFFFFFF
#define MEM_MONITOR_LOG_MAX_M  ((MEM_MONITOR_LOG_MAX_V)<<(MEM_MONITOR_LOG_MAX_S))
#define MEM_MONITOR_LOG_MAX_V  0xFFFFFFFF
#define MEM_MONITOR_LOG_MAX_S  0

#define MEM_MONITOR_LOG_MEM_START_REG          (DR_REG_MEM_MONITOR_BASE + 0x18)
/* MEM_MONITOR_LOG_MEM_START : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: the start address of writing logging message.*/
#define MEM_MONITOR_LOG_MEM_START    0xFFFFFFFF
#define MEM_MONITOR_LOG_MEM_START_M  ((MEM_MONITOR_LOG_MEM_START_V)<<(MEM_MONITOR_LOG_MEM_START_S))
#define MEM_MONITOR_LOG_MEM_START_V  0xFFFFFFFF
#define MEM_MONITOR_LOG_MEM_START_S  0

#define MEM_MONITOR_LOG_MEM_END_REG          (DR_REG_MEM_MONITOR_BASE + 0x1C)
/* MEM_MONITOR_LOG_MEM_END : R/W ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: the end address of writing logging message.*/
#define MEM_MONITOR_LOG_MEM_END    0xFFFFFFFF
#define MEM_MONITOR_LOG_MEM_END_M  ((MEM_MONITOR_LOG_MEM_END_V)<<(MEM_MONITOR_LOG_MEM_END_S))
#define MEM_MONITOR_LOG_MEM_END_V  0xFFFFFFFF
#define MEM_MONITOR_LOG_MEM_END_S  0

#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_REG          (DR_REG_MEM_MONITOR_BASE + 0x20)
/* MEM_MONITOR_LOG_MEM_CURRENT_ADDR : RO ;bitpos:[31:0] ;default: 32'b0 ; */
/*description: means next writing address.*/
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR    0xFFFFFFFF
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_M  ((MEM_MONITOR_LOG_MEM_CURRENT_ADDR_V)<<(MEM_MONITOR_LOG_MEM_CURRENT_ADDR_S))
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_V  0xFFFFFFFF
#define MEM_MONITOR_LOG_MEM_CURRENT_ADDR_S  0

#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_REG          (DR_REG_MEM_MONITOR_BASE + 0x24)
/* MEM_MONITOR_LOG_MEM_ADDR_UPDATE : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set 1 to updata MEM_MONITOR_LOG_MEM_CURRENT_ADDR, when set 1, MEM_MONITOR_LOG_ME
M_CURRENT_ADDR will update to MEM_MONITOR_LOG_MEM_START.*/
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE    (BIT(0))
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_M  (BIT(0))
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_V  0x1
#define MEM_MONITOR_LOG_MEM_ADDR_UPDATE_S  0

#define MEM_MONITOR_LOG_MEM_FULL_FLAG_REG          (DR_REG_MEM_MONITOR_BASE + 0x28)
/* MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set 1 to clr MEM_MONITOR_LOG_MEM_FULL_FLAG.*/
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG    (BIT(1))
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_M  (BIT(1))
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_V  0x1
#define MEM_MONITOR_CLR_LOG_MEM_FULL_FLAG_S  1
/* MEM_MONITOR_LOG_MEM_FULL_FLAG : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: 1 means memory write loop at least one time at the range of MEM_START and MEM_EN
D.*/
#define MEM_MONITOR_LOG_MEM_FULL_FLAG    (BIT(0))
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_M  (BIT(0))
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_V  0x1
#define MEM_MONITOR_LOG_MEM_FULL_FLAG_S  0

#define MEM_MONITOR_CLOCK_GATE_REG          (DR_REG_MEM_MONITOR_BASE + 0x2C)
/* MEM_MONITOR_CLK_EN : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set 1 to force on the clk of mem_monitor register.*/
#define MEM_MONITOR_CLK_EN    (BIT(0))
#define MEM_MONITOR_CLK_EN_M  (BIT(0))
#define MEM_MONITOR_CLK_EN_V  0x1
#define MEM_MONITOR_CLK_EN_S  0

#define MEM_MONITOR_DATE_REG          (DR_REG_MEM_MONITOR_BASE + 0x3FC)
/* MEM_MONITOR_DATE : R/W ;bitpos:[27:0] ;default: 28'h2302220 ; */
/*description: version register.*/
#define MEM_MONITOR_DATE    0x0FFFFFFF
#define MEM_MONITOR_DATE_M  ((MEM_MONITOR_DATE_V)<<(MEM_MONITOR_DATE_S))
#define MEM_MONITOR_DATE_V  0xFFFFFFF
#define MEM_MONITOR_DATE_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_MEM_MONITOR_REG_H_ */
