/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_HP_CLKRST_REG_H_
#define _SOC_HP_CLKRST_REG_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

#define HP_CLKRST_VER_DATE_REG          (DR_REG_HP_CLKRST_BASE + 0x0)
/* HP_CLKRST_VER_DATE : R/W ;bitpos:[31:0] ;default: 32'h20201229 ; */
/*description: .*/
#define HP_CLKRST_VER_DATE    0xFFFFFFFF
#define HP_CLKRST_VER_DATE_M  ((HP_CLKRST_VER_DATE_V)<<(HP_CLKRST_VER_DATE_S))
#define HP_CLKRST_VER_DATE_V  0xFFFFFFFF
#define HP_CLKRST_VER_DATE_S  0

#define HP_CLKRST_HP_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x4)
/* HP_CLKRST_HP_CPU_ROOT_CLK_SEL : R/W ;bitpos:[3:2] ;default: 2'h1 ; */
/*description: Hp cpu root clock source select; 2'h0: 20M RC OSC; 2'h1: 40M XTAL; 2'h2: HP CPU
PLL clock; 2'h3: HP system PLL clock.*/
#define HP_CLKRST_HP_CPU_ROOT_CLK_SEL    0x00000003
#define HP_CLKRST_HP_CPU_ROOT_CLK_SEL_M  ((HP_CLKRST_HP_CPU_ROOT_CLK_SEL_V)<<(HP_CLKRST_HP_CPU_ROOT_CLK_SEL_S))
#define HP_CLKRST_HP_CPU_ROOT_CLK_SEL_V  0x3
#define HP_CLKRST_HP_CPU_ROOT_CLK_SEL_S  2
/* HP_CLKRST_HP_SYS_ROOT_CLK_SEL : R/W ;bitpos:[1:0] ;default: 2'h1 ; */
/*description: Hp system root clock source select; 2'h0: 20M RC OSC; 2'h1: 40M XTAL; 2'h2: HP s
ystem PLL clock; 2'h3: HP CPU PLL clock.*/
#define HP_CLKRST_HP_SYS_ROOT_CLK_SEL    0x00000003
#define HP_CLKRST_HP_SYS_ROOT_CLK_SEL_M  ((HP_CLKRST_HP_SYS_ROOT_CLK_SEL_V)<<(HP_CLKRST_HP_SYS_ROOT_CLK_SEL_S))
#define HP_CLKRST_HP_SYS_ROOT_CLK_SEL_V  0x3
#define HP_CLKRST_HP_SYS_ROOT_CLK_SEL_S  0

#define HP_CLKRST_CPU_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x8)
/* HP_CLKRST_CPU_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define HP_CLKRST_CPU_CLK_CUR_DIV_NUM    0x000000FF
#define HP_CLKRST_CPU_CLK_CUR_DIV_NUM_M  ((HP_CLKRST_CPU_CLK_CUR_DIV_NUM_V)<<(HP_CLKRST_CPU_CLK_CUR_DIV_NUM_S))
#define HP_CLKRST_CPU_CLK_CUR_DIV_NUM_V  0xFF
#define HP_CLKRST_CPU_CLK_CUR_DIV_NUM_S  24
/* HP_CLKRST_CPU_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define HP_CLKRST_CPU_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_CPU_CLK_DIV_NUM_M  ((HP_CLKRST_CPU_CLK_DIV_NUM_V)<<(HP_CLKRST_CPU_CLK_DIV_NUM_S))
#define HP_CLKRST_CPU_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_CPU_CLK_DIV_NUM_S  8
/* HP_CLKRST_CPU_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define HP_CLKRST_CPU_CLK_EN    (BIT(0))
#define HP_CLKRST_CPU_CLK_EN_M  (BIT(0))
#define HP_CLKRST_CPU_CLK_EN_V  0x1
#define HP_CLKRST_CPU_CLK_EN_S  0

#define HP_CLKRST_SYS_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0xC)
/* HP_CLKRST_SYS_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define HP_CLKRST_SYS_CLK_CUR_DIV_NUM    0x000000FF
#define HP_CLKRST_SYS_CLK_CUR_DIV_NUM_M  ((HP_CLKRST_SYS_CLK_CUR_DIV_NUM_V)<<(HP_CLKRST_SYS_CLK_CUR_DIV_NUM_S))
#define HP_CLKRST_SYS_CLK_CUR_DIV_NUM_V  0xFF
#define HP_CLKRST_SYS_CLK_CUR_DIV_NUM_S  24
/* HP_CLKRST_SYS_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: phase offset compare to clock sync signal.*/
#define HP_CLKRST_SYS_CLK_PHASE_OFFSET    0x000000FF
#define HP_CLKRST_SYS_CLK_PHASE_OFFSET_M  ((HP_CLKRST_SYS_CLK_PHASE_OFFSET_V)<<(HP_CLKRST_SYS_CLK_PHASE_OFFSET_S))
#define HP_CLKRST_SYS_CLK_PHASE_OFFSET_V  0xFF
#define HP_CLKRST_SYS_CLK_PHASE_OFFSET_S  16
/* HP_CLKRST_SYS_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define HP_CLKRST_SYS_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_SYS_CLK_DIV_NUM_M  ((HP_CLKRST_SYS_CLK_DIV_NUM_V)<<(HP_CLKRST_SYS_CLK_DIV_NUM_S))
#define HP_CLKRST_SYS_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_SYS_CLK_DIV_NUM_S  8
/* HP_CLKRST_SYS_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define HP_CLKRST_SYS_CLK_FORCE_SYNC_EN    (BIT(2))
#define HP_CLKRST_SYS_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define HP_CLKRST_SYS_CLK_FORCE_SYNC_EN_V  0x1
#define HP_CLKRST_SYS_CLK_FORCE_SYNC_EN_S  2
/* HP_CLKRST_SYS_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define HP_CLKRST_SYS_CLK_SYNC_EN    (BIT(1))
#define HP_CLKRST_SYS_CLK_SYNC_EN_M  (BIT(1))
#define HP_CLKRST_SYS_CLK_SYNC_EN_V  0x1
#define HP_CLKRST_SYS_CLK_SYNC_EN_S  1
/* HP_CLKRST_SYS_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define HP_CLKRST_SYS_CLK_EN    (BIT(0))
#define HP_CLKRST_SYS_CLK_EN_M  (BIT(0))
#define HP_CLKRST_SYS_CLK_EN_V  0x1
#define HP_CLKRST_SYS_CLK_EN_S  0

#define HP_CLKRST_PERI1_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x10)
/* HP_CLKRST_PERI1_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define HP_CLKRST_PERI1_CLK_CUR_DIV_NUM    0x000000FF
#define HP_CLKRST_PERI1_CLK_CUR_DIV_NUM_M  ((HP_CLKRST_PERI1_CLK_CUR_DIV_NUM_V)<<(HP_CLKRST_PERI1_CLK_CUR_DIV_NUM_S))
#define HP_CLKRST_PERI1_CLK_CUR_DIV_NUM_V  0xFF
#define HP_CLKRST_PERI1_CLK_CUR_DIV_NUM_S  24
/* HP_CLKRST_PERI1_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: phase offset compare to clock sync signal.*/
#define HP_CLKRST_PERI1_CLK_PHASE_OFFSET    0x000000FF
#define HP_CLKRST_PERI1_CLK_PHASE_OFFSET_M  ((HP_CLKRST_PERI1_CLK_PHASE_OFFSET_V)<<(HP_CLKRST_PERI1_CLK_PHASE_OFFSET_S))
#define HP_CLKRST_PERI1_CLK_PHASE_OFFSET_V  0xFF
#define HP_CLKRST_PERI1_CLK_PHASE_OFFSET_S  16
/* HP_CLKRST_PERI1_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define HP_CLKRST_PERI1_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_PERI1_CLK_DIV_NUM_M  ((HP_CLKRST_PERI1_CLK_DIV_NUM_V)<<(HP_CLKRST_PERI1_CLK_DIV_NUM_S))
#define HP_CLKRST_PERI1_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_PERI1_CLK_DIV_NUM_S  8
/* HP_CLKRST_PERI1_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define HP_CLKRST_PERI1_CLK_FORCE_SYNC_EN    (BIT(2))
#define HP_CLKRST_PERI1_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define HP_CLKRST_PERI1_CLK_FORCE_SYNC_EN_V  0x1
#define HP_CLKRST_PERI1_CLK_FORCE_SYNC_EN_S  2
/* HP_CLKRST_PERI1_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define HP_CLKRST_PERI1_CLK_SYNC_EN    (BIT(1))
#define HP_CLKRST_PERI1_CLK_SYNC_EN_M  (BIT(1))
#define HP_CLKRST_PERI1_CLK_SYNC_EN_V  0x1
#define HP_CLKRST_PERI1_CLK_SYNC_EN_S  1
/* HP_CLKRST_PERI1_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define HP_CLKRST_PERI1_CLK_EN    (BIT(0))
#define HP_CLKRST_PERI1_CLK_EN_M  (BIT(0))
#define HP_CLKRST_PERI1_CLK_EN_V  0x1
#define HP_CLKRST_PERI1_CLK_EN_S  0

#define HP_CLKRST_PERI2_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x14)
/* HP_CLKRST_PERI2_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define HP_CLKRST_PERI2_CLK_CUR_DIV_NUM    0x000000FF
#define HP_CLKRST_PERI2_CLK_CUR_DIV_NUM_M  ((HP_CLKRST_PERI2_CLK_CUR_DIV_NUM_V)<<(HP_CLKRST_PERI2_CLK_CUR_DIV_NUM_S))
#define HP_CLKRST_PERI2_CLK_CUR_DIV_NUM_V  0xFF
#define HP_CLKRST_PERI2_CLK_CUR_DIV_NUM_S  24
/* HP_CLKRST_PERI2_CLK_PHASE_OFFSET : R/W ;bitpos:[23:16] ;default: 8'h0 ; */
/*description: phase offset compare to clock sync signal.*/
#define HP_CLKRST_PERI2_CLK_PHASE_OFFSET    0x000000FF
#define HP_CLKRST_PERI2_CLK_PHASE_OFFSET_M  ((HP_CLKRST_PERI2_CLK_PHASE_OFFSET_V)<<(HP_CLKRST_PERI2_CLK_PHASE_OFFSET_S))
#define HP_CLKRST_PERI2_CLK_PHASE_OFFSET_V  0xFF
#define HP_CLKRST_PERI2_CLK_PHASE_OFFSET_S  16
/* HP_CLKRST_PERI2_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: clock divider number.*/
#define HP_CLKRST_PERI2_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_PERI2_CLK_DIV_NUM_M  ((HP_CLKRST_PERI2_CLK_DIV_NUM_V)<<(HP_CLKRST_PERI2_CLK_DIV_NUM_S))
#define HP_CLKRST_PERI2_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_PERI2_CLK_DIV_NUM_S  8
/* HP_CLKRST_PERI2_CLK_FORCE_SYNC_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: clock force sync enable : clock output only available when clock is synced.*/
#define HP_CLKRST_PERI2_CLK_FORCE_SYNC_EN    (BIT(2))
#define HP_CLKRST_PERI2_CLK_FORCE_SYNC_EN_M  (BIT(2))
#define HP_CLKRST_PERI2_CLK_FORCE_SYNC_EN_V  0x1
#define HP_CLKRST_PERI2_CLK_FORCE_SYNC_EN_S  2
/* HP_CLKRST_PERI2_CLK_SYNC_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: clock sync enable : will adjust clock phase when receive clock sync signal.*/
#define HP_CLKRST_PERI2_CLK_SYNC_EN    (BIT(1))
#define HP_CLKRST_PERI2_CLK_SYNC_EN_M  (BIT(1))
#define HP_CLKRST_PERI2_CLK_SYNC_EN_V  0x1
#define HP_CLKRST_PERI2_CLK_SYNC_EN_S  1
/* HP_CLKRST_PERI2_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define HP_CLKRST_PERI2_CLK_EN    (BIT(0))
#define HP_CLKRST_PERI2_CLK_EN_M  (BIT(0))
#define HP_CLKRST_PERI2_CLK_EN_V  0x1
#define HP_CLKRST_PERI2_CLK_EN_S  0

#define HP_CLKRST_PSRAM_PHY_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x18)
/* HP_CLKRST_PSRAM_PHY_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define HP_CLKRST_PSRAM_PHY_CLK_CUR_DIV_NUM    0x000000FF
#define HP_CLKRST_PSRAM_PHY_CLK_CUR_DIV_NUM_M  ((HP_CLKRST_PSRAM_PHY_CLK_CUR_DIV_NUM_V)<<(HP_CLKRST_PSRAM_PHY_CLK_CUR_DIV_NUM_S))
#define HP_CLKRST_PSRAM_PHY_CLK_CUR_DIV_NUM_V  0xFF
#define HP_CLKRST_PSRAM_PHY_CLK_CUR_DIV_NUM_S  24
/* HP_CLKRST_PSRAM_PHY_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: .*/
#define HP_CLKRST_PSRAM_PHY_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_PSRAM_PHY_CLK_DIV_NUM_M  ((HP_CLKRST_PSRAM_PHY_CLK_DIV_NUM_V)<<(HP_CLKRST_PSRAM_PHY_CLK_DIV_NUM_S))
#define HP_CLKRST_PSRAM_PHY_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_PSRAM_PHY_CLK_DIV_NUM_S  8
/* HP_CLKRST_PSRAM_PHY_CLK_SEL : R/W ;bitpos:[2:1] ;default: 2'h1 ; */
/*description: .*/
#define HP_CLKRST_PSRAM_PHY_CLK_SEL    0x00000003
#define HP_CLKRST_PSRAM_PHY_CLK_SEL_M  ((HP_CLKRST_PSRAM_PHY_CLK_SEL_V)<<(HP_CLKRST_PSRAM_PHY_CLK_SEL_S))
#define HP_CLKRST_PSRAM_PHY_CLK_SEL_V  0x3
#define HP_CLKRST_PSRAM_PHY_CLK_SEL_S  1
/* HP_CLKRST_PSRAM_PHY_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define HP_CLKRST_PSRAM_PHY_CLK_EN    (BIT(0))
#define HP_CLKRST_PSRAM_PHY_CLK_EN_M  (BIT(0))
#define HP_CLKRST_PSRAM_PHY_CLK_EN_V  0x1
#define HP_CLKRST_PSRAM_PHY_CLK_EN_S  0

#define HP_CLKRST_DDR_PHY_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x1C)
/* HP_CLKRST_DDR_PHY_CLK_CUR_DIV_NUM : RO ;bitpos:[31:24] ;default: 8'h0 ; */
/*description: current clock divider number.*/
#define HP_CLKRST_DDR_PHY_CLK_CUR_DIV_NUM    0x000000FF
#define HP_CLKRST_DDR_PHY_CLK_CUR_DIV_NUM_M  ((HP_CLKRST_DDR_PHY_CLK_CUR_DIV_NUM_V)<<(HP_CLKRST_DDR_PHY_CLK_CUR_DIV_NUM_S))
#define HP_CLKRST_DDR_PHY_CLK_CUR_DIV_NUM_V  0xFF
#define HP_CLKRST_DDR_PHY_CLK_CUR_DIV_NUM_S  24
/* HP_CLKRST_DDR_PHY_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h0 ; */
/*description: .*/
#define HP_CLKRST_DDR_PHY_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_DDR_PHY_CLK_DIV_NUM_M  ((HP_CLKRST_DDR_PHY_CLK_DIV_NUM_V)<<(HP_CLKRST_DDR_PHY_CLK_DIV_NUM_S))
#define HP_CLKRST_DDR_PHY_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_DDR_PHY_CLK_DIV_NUM_S  8
/* HP_CLKRST_DDR_PHY_CLK_SEL : R/W ;bitpos:[2:1] ;default: 2'h1 ; */
/*description: .*/
#define HP_CLKRST_DDR_PHY_CLK_SEL    0x00000003
#define HP_CLKRST_DDR_PHY_CLK_SEL_M  ((HP_CLKRST_DDR_PHY_CLK_SEL_V)<<(HP_CLKRST_DDR_PHY_CLK_SEL_S))
#define HP_CLKRST_DDR_PHY_CLK_SEL_V  0x3
#define HP_CLKRST_DDR_PHY_CLK_SEL_S  1
/* HP_CLKRST_DDR_PHY_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: .*/
#define HP_CLKRST_DDR_PHY_CLK_EN    (BIT(0))
#define HP_CLKRST_DDR_PHY_CLK_EN_M  (BIT(0))
#define HP_CLKRST_DDR_PHY_CLK_EN_V  0x1
#define HP_CLKRST_DDR_PHY_CLK_EN_S  0

#define HP_CLKRST_MSPI_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x20)
/* HP_CLKRST_MSPI_SRC_CLK_SEL : R/W ;bitpos:[17:16] ;default: 2'h2 ; */
/*description: 2'b00:480MHz PLL; 2'b01: MSPI DLL CLK; 2'b1x: HP XTAL CLK.*/
#define HP_CLKRST_MSPI_SRC_CLK_SEL    0x00000003
#define HP_CLKRST_MSPI_SRC_CLK_SEL_M  ((HP_CLKRST_MSPI_SRC_CLK_SEL_V)<<(HP_CLKRST_MSPI_SRC_CLK_SEL_S))
#define HP_CLKRST_MSPI_SRC_CLK_SEL_V  0x3
#define HP_CLKRST_MSPI_SRC_CLK_SEL_S  16
/* HP_CLKRST_MSPI_CLK_DIV_NUM : R/W ;bitpos:[11:8] ;default: 4'h1 ; */
/*description: clock divider number.*/
#define HP_CLKRST_MSPI_CLK_DIV_NUM    0x0000000F
#define HP_CLKRST_MSPI_CLK_DIV_NUM_M  ((HP_CLKRST_MSPI_CLK_DIV_NUM_V)<<(HP_CLKRST_MSPI_CLK_DIV_NUM_S))
#define HP_CLKRST_MSPI_CLK_DIV_NUM_V  0xF
#define HP_CLKRST_MSPI_CLK_DIV_NUM_S  8
/* HP_CLKRST_MSPI_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define HP_CLKRST_MSPI_CLK_EN    (BIT(0))
#define HP_CLKRST_MSPI_CLK_EN_M  (BIT(0))
#define HP_CLKRST_MSPI_CLK_EN_V  0x1
#define HP_CLKRST_MSPI_CLK_EN_S  0

#define HP_CLKRST_DUAL_MSPI_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x24)
/* HP_CLKRST_DUAL_MSPI_SRC_CLK_SEL : R/W ;bitpos:[17:16] ;default: 2'h2 ; */
/*description: 2'b00:480MHz PLL; 2'b01: MSPI DLL CLK; 2'b1x: HP XTAL CLK.*/
#define HP_CLKRST_DUAL_MSPI_SRC_CLK_SEL    0x00000003
#define HP_CLKRST_DUAL_MSPI_SRC_CLK_SEL_M  ((HP_CLKRST_DUAL_MSPI_SRC_CLK_SEL_V)<<(HP_CLKRST_DUAL_MSPI_SRC_CLK_SEL_S))
#define HP_CLKRST_DUAL_MSPI_SRC_CLK_SEL_V  0x3
#define HP_CLKRST_DUAL_MSPI_SRC_CLK_SEL_S  16
/* HP_CLKRST_DUAL_MSPI_CLK_DIV_NUM : R/W ;bitpos:[11:8] ;default: 4'h1 ; */
/*description: clock divider number.*/
#define HP_CLKRST_DUAL_MSPI_CLK_DIV_NUM    0x0000000F
#define HP_CLKRST_DUAL_MSPI_CLK_DIV_NUM_M  ((HP_CLKRST_DUAL_MSPI_CLK_DIV_NUM_V)<<(HP_CLKRST_DUAL_MSPI_CLK_DIV_NUM_S))
#define HP_CLKRST_DUAL_MSPI_CLK_DIV_NUM_V  0xF
#define HP_CLKRST_DUAL_MSPI_CLK_DIV_NUM_S  8
/* HP_CLKRST_DUAL_MSPI_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: clock output enable.*/
#define HP_CLKRST_DUAL_MSPI_CLK_EN    (BIT(0))
#define HP_CLKRST_DUAL_MSPI_CLK_EN_M  (BIT(0))
#define HP_CLKRST_DUAL_MSPI_CLK_EN_V  0x1
#define HP_CLKRST_DUAL_MSPI_CLK_EN_S  0

#define HP_CLKRST_REF_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x28)
/* HP_CLKRST_REF_CLK2_DIV_NUM : R/W ;bitpos:[27:24] ;default: 4'h3 ; */
/*description: 120MHz reference clock divider number, used by i3c master.*/
#define HP_CLKRST_REF_CLK2_DIV_NUM    0x0000000F
#define HP_CLKRST_REF_CLK2_DIV_NUM_M  ((HP_CLKRST_REF_CLK2_DIV_NUM_V)<<(HP_CLKRST_REF_CLK2_DIV_NUM_S))
#define HP_CLKRST_REF_CLK2_DIV_NUM_V  0xF
#define HP_CLKRST_REF_CLK2_DIV_NUM_S  24
/* HP_CLKRST_USBPHY_CLK_DIV_NUM : R/W ;bitpos:[23:20] ;default: 4'h9 ; */
/*description: usbphy clock divider number.*/
#define HP_CLKRST_USBPHY_CLK_DIV_NUM    0x0000000F
#define HP_CLKRST_USBPHY_CLK_DIV_NUM_M  ((HP_CLKRST_USBPHY_CLK_DIV_NUM_V)<<(HP_CLKRST_USBPHY_CLK_DIV_NUM_S))
#define HP_CLKRST_USBPHY_CLK_DIV_NUM_V  0xF
#define HP_CLKRST_USBPHY_CLK_DIV_NUM_S  20
/* HP_CLKRST_LEDC_REF_CLK_DIV_NUM : R/W ;bitpos:[19:16] ;default: 4'h1 ; */
/*description: ledc reference clock divider number.*/
#define HP_CLKRST_LEDC_REF_CLK_DIV_NUM    0x0000000F
#define HP_CLKRST_LEDC_REF_CLK_DIV_NUM_M  ((HP_CLKRST_LEDC_REF_CLK_DIV_NUM_V)<<(HP_CLKRST_LEDC_REF_CLK_DIV_NUM_S))
#define HP_CLKRST_LEDC_REF_CLK_DIV_NUM_V  0xF
#define HP_CLKRST_LEDC_REF_CLK_DIV_NUM_S  16
/* HP_CLKRST_USB2_REF_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h27 ; */
/*description: usb2 phy reference clock divider number.*/
#define HP_CLKRST_USB2_REF_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_USB2_REF_CLK_DIV_NUM_M  ((HP_CLKRST_USB2_REF_CLK_DIV_NUM_V)<<(HP_CLKRST_USB2_REF_CLK_DIV_NUM_S))
#define HP_CLKRST_USB2_REF_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_USB2_REF_CLK_DIV_NUM_S  8
/* HP_CLKRST_REF_CLK_DIV_NUM : R/W ;bitpos:[4:1] ;default: 4'h2 ; */
/*description: reference clock divider number.*/
#define HP_CLKRST_REF_CLK_DIV_NUM    0x0000000F
#define HP_CLKRST_REF_CLK_DIV_NUM_M  ((HP_CLKRST_REF_CLK_DIV_NUM_V)<<(HP_CLKRST_REF_CLK_DIV_NUM_S))
#define HP_CLKRST_REF_CLK_DIV_NUM_V  0xF
#define HP_CLKRST_REF_CLK_DIV_NUM_S  1
/* HP_CLKRST_REF_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: reference clock output enable.*/
#define HP_CLKRST_REF_CLK_EN    (BIT(0))
#define HP_CLKRST_REF_CLK_EN_M  (BIT(0))
#define HP_CLKRST_REF_CLK_EN_V  0x1
#define HP_CLKRST_REF_CLK_EN_S  0

#define HP_CLKRST_TM_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x2C)
/* HP_CLKRST_TM_240M_CLK_EN : R/W ;bitpos:[7] ;default: 1'b1 ; */
/*description: 240M test mode clock enable.*/
#define HP_CLKRST_TM_240M_CLK_EN    (BIT(7))
#define HP_CLKRST_TM_240M_CLK_EN_M  (BIT(7))
#define HP_CLKRST_TM_240M_CLK_EN_V  0x1
#define HP_CLKRST_TM_240M_CLK_EN_S  7
/* HP_CLKRST_TM_200M_CLK_EN : R/W ;bitpos:[6] ;default: 1'b1 ; */
/*description: 200M test mode clock enable.*/
#define HP_CLKRST_TM_200M_CLK_EN    (BIT(6))
#define HP_CLKRST_TM_200M_CLK_EN_M  (BIT(6))
#define HP_CLKRST_TM_200M_CLK_EN_V  0x1
#define HP_CLKRST_TM_200M_CLK_EN_S  6
/* HP_CLKRST_TM_160M_CLK_EN : R/W ;bitpos:[5] ;default: 1'b1 ; */
/*description: 160M test mode clock enable.*/
#define HP_CLKRST_TM_160M_CLK_EN    (BIT(5))
#define HP_CLKRST_TM_160M_CLK_EN_M  (BIT(5))
#define HP_CLKRST_TM_160M_CLK_EN_V  0x1
#define HP_CLKRST_TM_160M_CLK_EN_S  5
/* HP_CLKRST_TM_120M_CLK_EN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: 120M test mode clock enable.*/
#define HP_CLKRST_TM_120M_CLK_EN    (BIT(4))
#define HP_CLKRST_TM_120M_CLK_EN_M  (BIT(4))
#define HP_CLKRST_TM_120M_CLK_EN_V  0x1
#define HP_CLKRST_TM_120M_CLK_EN_S  4
/* HP_CLKRST_TM_80M_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: 80M test mode clock enable.*/
#define HP_CLKRST_TM_80M_CLK_EN    (BIT(3))
#define HP_CLKRST_TM_80M_CLK_EN_M  (BIT(3))
#define HP_CLKRST_TM_80M_CLK_EN_V  0x1
#define HP_CLKRST_TM_80M_CLK_EN_S  3
/* HP_CLKRST_TM_48M_CLK_EN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: 48M test mode clock enable.*/
#define HP_CLKRST_TM_48M_CLK_EN    (BIT(2))
#define HP_CLKRST_TM_48M_CLK_EN_M  (BIT(2))
#define HP_CLKRST_TM_48M_CLK_EN_V  0x1
#define HP_CLKRST_TM_48M_CLK_EN_S  2
/* HP_CLKRST_TM_40M_CLK_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: 40M test mode clock enable.*/
#define HP_CLKRST_TM_40M_CLK_EN    (BIT(1))
#define HP_CLKRST_TM_40M_CLK_EN_M  (BIT(1))
#define HP_CLKRST_TM_40M_CLK_EN_V  0x1
#define HP_CLKRST_TM_40M_CLK_EN_S  1
/* HP_CLKRST_TM_20M_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: 20M test mode clock enabl.*/
#define HP_CLKRST_TM_20M_CLK_EN    (BIT(0))
#define HP_CLKRST_TM_20M_CLK_EN_M  (BIT(0))
#define HP_CLKRST_TM_20M_CLK_EN_V  0x1
#define HP_CLKRST_TM_20M_CLK_EN_S  0

#define HP_CLKRST_CORE_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x30)
/* HP_CLKRST_CORE0_GLOBAL_RSTN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: core0 software global reset.*/
#define HP_CLKRST_CORE0_GLOBAL_RSTN    (BIT(9))
#define HP_CLKRST_CORE0_GLOBAL_RSTN_M  (BIT(9))
#define HP_CLKRST_CORE0_GLOBAL_RSTN_V  0x1
#define HP_CLKRST_CORE0_GLOBAL_RSTN_S  9
/* HP_CLKRST_CORE1_GLOBAL_RSTN : R/W ;bitpos:[8] ;default: 1'b0 ; */
/*description: core1 software global reset.*/
#define HP_CLKRST_CORE1_GLOBAL_RSTN    (BIT(8))
#define HP_CLKRST_CORE1_GLOBAL_RSTN_M  (BIT(8))
#define HP_CLKRST_CORE1_GLOBAL_RSTN_V  0x1
#define HP_CLKRST_CORE1_GLOBAL_RSTN_S  8
/* HP_CLKRST_CORE0_FORCE_NORST : R/W ;bitpos:[7] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define HP_CLKRST_CORE0_FORCE_NORST    (BIT(7))
#define HP_CLKRST_CORE0_FORCE_NORST_M  (BIT(7))
#define HP_CLKRST_CORE0_FORCE_NORST_V  0x1
#define HP_CLKRST_CORE0_FORCE_NORST_S  7
/* HP_CLKRST_CORE1_FORCE_NORST : R/W ;bitpos:[6] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define HP_CLKRST_CORE1_FORCE_NORST    (BIT(6))
#define HP_CLKRST_CORE1_FORCE_NORST_M  (BIT(6))
#define HP_CLKRST_CORE1_FORCE_NORST_V  0x1
#define HP_CLKRST_CORE1_FORCE_NORST_S  6
/* HP_CLKRST_CORE2_FORCE_NORST : R/W ;bitpos:[5] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define HP_CLKRST_CORE2_FORCE_NORST    (BIT(5))
#define HP_CLKRST_CORE2_FORCE_NORST_M  (BIT(5))
#define HP_CLKRST_CORE2_FORCE_NORST_V  0x1
#define HP_CLKRST_CORE2_FORCE_NORST_S  5
/* HP_CLKRST_CORE3_FORCE_NORST : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: software force no reset.*/
#define HP_CLKRST_CORE3_FORCE_NORST    (BIT(4))
#define HP_CLKRST_CORE3_FORCE_NORST_M  (BIT(4))
#define HP_CLKRST_CORE3_FORCE_NORST_V  0x1
#define HP_CLKRST_CORE3_FORCE_NORST_S  4
/* HP_CLKRST_CORE0_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: hp core0 clock enable.*/
#define HP_CLKRST_CORE0_CLK_EN    (BIT(3))
#define HP_CLKRST_CORE0_CLK_EN_M  (BIT(3))
#define HP_CLKRST_CORE0_CLK_EN_V  0x1
#define HP_CLKRST_CORE0_CLK_EN_S  3
/* HP_CLKRST_CORE1_CLK_EN : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: hp core1 clock enable.*/
#define HP_CLKRST_CORE1_CLK_EN    (BIT(2))
#define HP_CLKRST_CORE1_CLK_EN_M  (BIT(2))
#define HP_CLKRST_CORE1_CLK_EN_V  0x1
#define HP_CLKRST_CORE1_CLK_EN_S  2
/* HP_CLKRST_CORE2_CLK_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: hp core2 clock enable.*/
#define HP_CLKRST_CORE2_CLK_EN    (BIT(1))
#define HP_CLKRST_CORE2_CLK_EN_M  (BIT(1))
#define HP_CLKRST_CORE2_CLK_EN_V  0x1
#define HP_CLKRST_CORE2_CLK_EN_S  1
/* HP_CLKRST_CORE3_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: hp core3 clock enable.*/
#define HP_CLKRST_CORE3_CLK_EN    (BIT(0))
#define HP_CLKRST_CORE3_CLK_EN_M  (BIT(0))
#define HP_CLKRST_CORE3_CLK_EN_V  0x1
#define HP_CLKRST_CORE3_CLK_EN_S  0

#define HP_CLKRST_CACHE_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x34)
/* HP_CLKRST_CACHE_CLK_DIV_NUM : R/W ;bitpos:[15:8] ;default: 8'h1 ; */
/*description: L2 cache clock divider number.*/
#define HP_CLKRST_CACHE_CLK_DIV_NUM    0x000000FF
#define HP_CLKRST_CACHE_CLK_DIV_NUM_M  ((HP_CLKRST_CACHE_CLK_DIV_NUM_V)<<(HP_CLKRST_CACHE_CLK_DIV_NUM_S))
#define HP_CLKRST_CACHE_CLK_DIV_NUM_V  0xFF
#define HP_CLKRST_CACHE_CLK_DIV_NUM_S  8
/* HP_CLKRST_HP_CACHE_RSTN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: cache software reset: low active.*/
#define HP_CLKRST_HP_CACHE_RSTN    (BIT(2))
#define HP_CLKRST_HP_CACHE_RSTN_M  (BIT(2))
#define HP_CLKRST_HP_CACHE_RSTN_V  0x1
#define HP_CLKRST_HP_CACHE_RSTN_S  2
/* HP_CLKRST_CACHE_APB_CLK_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: cache apb clock enable.*/
#define HP_CLKRST_CACHE_APB_CLK_EN    (BIT(1))
#define HP_CLKRST_CACHE_APB_CLK_EN_M  (BIT(1))
#define HP_CLKRST_CACHE_APB_CLK_EN_V  0x1
#define HP_CLKRST_CACHE_APB_CLK_EN_S  1
/* HP_CLKRST_CACHE_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: cache clock enable.*/
#define HP_CLKRST_CACHE_CLK_EN    (BIT(0))
#define HP_CLKRST_CACHE_CLK_EN_M  (BIT(0))
#define HP_CLKRST_CACHE_CLK_EN_V  0x1
#define HP_CLKRST_CACHE_CLK_EN_S  0

#define HP_CLKRST_CPU_PERI_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x38)
/* HP_CLKRST_L2_MEM_RSTN : R/W ;bitpos:[4] ;default: 1'b1 ; */
/*description: l2 memory software reset: low active.*/
#define HP_CLKRST_L2_MEM_RSTN    (BIT(4))
#define HP_CLKRST_L2_MEM_RSTN_M  (BIT(4))
#define HP_CLKRST_L2_MEM_RSTN_V  0x1
#define HP_CLKRST_L2_MEM_RSTN_S  4
/* HP_CLKRST_L2_MEM_CLK_EN : R/W ;bitpos:[3] ;default: 1'b1 ; */
/*description: l2 memory clock enable.*/
#define HP_CLKRST_L2_MEM_CLK_EN    (BIT(3))
#define HP_CLKRST_L2_MEM_CLK_EN_M  (BIT(3))
#define HP_CLKRST_L2_MEM_CLK_EN_V  0x1
#define HP_CLKRST_L2_MEM_CLK_EN_S  3
/* HP_CLKRST_TCM_RSTN : R/W ;bitpos:[2] ;default: 1'b1 ; */
/*description: tcm software reset: low active.*/
#define HP_CLKRST_TCM_RSTN    (BIT(2))
#define HP_CLKRST_TCM_RSTN_M  (BIT(2))
#define HP_CLKRST_TCM_RSTN_V  0x1
#define HP_CLKRST_TCM_RSTN_S  2
/* HP_CLKRST_TCM_CLK_EN : R/W ;bitpos:[1] ;default: 1'b1 ; */
/*description: tcm clock enable.*/
#define HP_CLKRST_TCM_CLK_EN    (BIT(1))
#define HP_CLKRST_TCM_CLK_EN_M  (BIT(1))
#define HP_CLKRST_TCM_CLK_EN_V  0x1
#define HP_CLKRST_TCM_CLK_EN_S  1
/* HP_CLKRST_CPU_CTRL_CLK_EN : R/W ;bitpos:[0] ;default: 1'b1 ; */
/*description: cpu control logic clock enable.*/
#define HP_CLKRST_CPU_CTRL_CLK_EN    (BIT(0))
#define HP_CLKRST_CPU_CTRL_CLK_EN_M  (BIT(0))
#define HP_CLKRST_CPU_CTRL_CLK_EN_V  0x1
#define HP_CLKRST_CPU_CTRL_CLK_EN_S  0

#define HP_CLKRST_SYNC_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x3C)
/* HP_CLKRST_CLK_EN : R/W ;bitpos:[17] ;default: 1'b1 ; */
/*description: .*/
#define HP_CLKRST_CLK_EN    (BIT(17))
#define HP_CLKRST_CLK_EN_M  (BIT(17))
#define HP_CLKRST_CLK_EN_V  0x1
#define HP_CLKRST_CLK_EN_S  17
/* HP_CLKRST_HP_ROOT_CLK_SYNC_EN : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: clock sync signal output enable.*/
#define HP_CLKRST_HP_ROOT_CLK_SYNC_EN    (BIT(16))
#define HP_CLKRST_HP_ROOT_CLK_SYNC_EN_M  (BIT(16))
#define HP_CLKRST_HP_ROOT_CLK_SYNC_EN_V  0x1
#define HP_CLKRST_HP_ROOT_CLK_SYNC_EN_S  16
/* HP_CLKRST_HP_ROOT_CLK_SYNC_PERID : R/W ;bitpos:[15:0] ;default: 16'h347 ; */
/*description: clock sync signal generation period.*/
#define HP_CLKRST_HP_ROOT_CLK_SYNC_PERID    0x0000FFFF
#define HP_CLKRST_HP_ROOT_CLK_SYNC_PERID_M  ((HP_CLKRST_HP_ROOT_CLK_SYNC_PERID_V)<<(HP_CLKRST_HP_ROOT_CLK_SYNC_PERID_S))
#define HP_CLKRST_HP_ROOT_CLK_SYNC_PERID_V  0xFFFF
#define HP_CLKRST_HP_ROOT_CLK_SYNC_PERID_S  0

#define HP_CLKRST_WFI_GATE_CLK_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x40)
/* HP_CLKRST_CPU_WFI_FORCE_CLKG3_ON : R/W ;bitpos:[18] ;default: 1'b1 ; */
/*description: force group3(L2 Memory) clock on after WFI.*/
#define HP_CLKRST_CPU_WFI_FORCE_CLKG3_ON    (BIT(18))
#define HP_CLKRST_CPU_WFI_FORCE_CLKG3_ON_M  (BIT(18))
#define HP_CLKRST_CPU_WFI_FORCE_CLKG3_ON_V  0x1
#define HP_CLKRST_CPU_WFI_FORCE_CLKG3_ON_S  18
/* HP_CLKRST_CPU_WFI_FORCE_CLKG2_ON : R/W ;bitpos:[17] ;default: 1'b1 ; */
/*description: force group2(HP TCM) clock on after WFI.*/
#define HP_CLKRST_CPU_WFI_FORCE_CLKG2_ON    (BIT(17))
#define HP_CLKRST_CPU_WFI_FORCE_CLKG2_ON_M  (BIT(17))
#define HP_CLKRST_CPU_WFI_FORCE_CLKG2_ON_V  0x1
#define HP_CLKRST_CPU_WFI_FORCE_CLKG2_ON_S  17
/* HP_CLKRST_CPU_WFI_FORCE_CLKG1_ON : R/W ;bitpos:[16] ;default: 1'b1 ; */
/*description: force group1(L1/L2 cache & trace & cpu_icm_ibus) clock on after WFI.*/
#define HP_CLKRST_CPU_WFI_FORCE_CLKG1_ON    (BIT(16))
#define HP_CLKRST_CPU_WFI_FORCE_CLKG1_ON_M  (BIT(16))
#define HP_CLKRST_CPU_WFI_FORCE_CLKG1_ON_V  0x1
#define HP_CLKRST_CPU_WFI_FORCE_CLKG1_ON_S  16
/* HP_CLKRST_CPU_WFI_DELAY_NUM : R/W ;bitpos:[3:0] ;default: 4'h0 ; */
/*description: This register indicates delayed clock cycles before auto gating HP cache/trace c
lock once WFI asserted.*/
#define HP_CLKRST_CPU_WFI_DELAY_NUM    0x0000000F
#define HP_CLKRST_CPU_WFI_DELAY_NUM_M  ((HP_CLKRST_CPU_WFI_DELAY_NUM_V)<<(HP_CLKRST_CPU_WFI_DELAY_NUM_S))
#define HP_CLKRST_CPU_WFI_DELAY_NUM_V  0xF
#define HP_CLKRST_CPU_WFI_DELAY_NUM_S  0

#define HP_CLKRST_PVT_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x44)
/* HP_CLKRST_PVT_APB_RSTN : R/W ;bitpos:[21] ;default: 1'h1 ; */
/*description: pvt apb resetn.*/
#define HP_CLKRST_PVT_APB_RSTN    (BIT(21))
#define HP_CLKRST_PVT_APB_RSTN_M  (BIT(21))
#define HP_CLKRST_PVT_APB_RSTN_V  0x1
#define HP_CLKRST_PVT_APB_RSTN_S  21
/* HP_CLKRST_PVT_PERI_GROUP2_RSTN : R/W ;bitpos:[20] ;default: 1'h1 ; */
/*description: pvt peri group2 resetn.*/
#define HP_CLKRST_PVT_PERI_GROUP2_RSTN    (BIT(20))
#define HP_CLKRST_PVT_PERI_GROUP2_RSTN_M  (BIT(20))
#define HP_CLKRST_PVT_PERI_GROUP2_RSTN_V  0x1
#define HP_CLKRST_PVT_PERI_GROUP2_RSTN_S  20
/* HP_CLKRST_PVT_PERI_GROUP1_RSTN : R/W ;bitpos:[19] ;default: 1'h1 ; */
/*description: pvt peri group1 resetn.*/
#define HP_CLKRST_PVT_PERI_GROUP1_RSTN    (BIT(19))
#define HP_CLKRST_PVT_PERI_GROUP1_RSTN_M  (BIT(19))
#define HP_CLKRST_PVT_PERI_GROUP1_RSTN_V  0x1
#define HP_CLKRST_PVT_PERI_GROUP1_RSTN_S  19
/* HP_CLKRST_PVT_CPU_GROUP2_RSTN : R/W ;bitpos:[18] ;default: 1'h1 ; */
/*description: pvt cpu group2 resetn.*/
#define HP_CLKRST_PVT_CPU_GROUP2_RSTN    (BIT(18))
#define HP_CLKRST_PVT_CPU_GROUP2_RSTN_M  (BIT(18))
#define HP_CLKRST_PVT_CPU_GROUP2_RSTN_V  0x1
#define HP_CLKRST_PVT_CPU_GROUP2_RSTN_S  18
/* HP_CLKRST_PVT_CPU_GROUP1_RSTN : R/W ;bitpos:[17] ;default: 1'h1 ; */
/*description: pvt cpu group1 resetn.*/
#define HP_CLKRST_PVT_CPU_GROUP1_RSTN    (BIT(17))
#define HP_CLKRST_PVT_CPU_GROUP1_RSTN_M  (BIT(17))
#define HP_CLKRST_PVT_CPU_GROUP1_RSTN_V  0x1
#define HP_CLKRST_PVT_CPU_GROUP1_RSTN_S  17
/* HP_CLKRST_PVT_TOP_RSTN : R/W ;bitpos:[16] ;default: 1'h1 ; */
/*description: pvt top resetn.*/
#define HP_CLKRST_PVT_TOP_RSTN    (BIT(16))
#define HP_CLKRST_PVT_TOP_RSTN_M  (BIT(16))
#define HP_CLKRST_PVT_TOP_RSTN_V  0x1
#define HP_CLKRST_PVT_TOP_RSTN_S  16
/* HP_CLKRST_PVT_APB_CLK_EN : R/W ;bitpos:[13] ;default: 1'b1 ; */
/*description: pvt apb clk en.*/
#define HP_CLKRST_PVT_APB_CLK_EN    (BIT(13))
#define HP_CLKRST_PVT_APB_CLK_EN_M  (BIT(13))
#define HP_CLKRST_PVT_APB_CLK_EN_V  0x1
#define HP_CLKRST_PVT_APB_CLK_EN_S  13
/* HP_CLKRST_PVT_PERI_GROUP2_CLK_EN : R/W ;bitpos:[12] ;default: 1'b1 ; */
/*description: pvt peri group2 clk en.*/
#define HP_CLKRST_PVT_PERI_GROUP2_CLK_EN    (BIT(12))
#define HP_CLKRST_PVT_PERI_GROUP2_CLK_EN_M  (BIT(12))
#define HP_CLKRST_PVT_PERI_GROUP2_CLK_EN_V  0x1
#define HP_CLKRST_PVT_PERI_GROUP2_CLK_EN_S  12
/* HP_CLKRST_PVT_PERI_GROUP1_CLK_EN : R/W ;bitpos:[11] ;default: 1'b1 ; */
/*description: pvt peri group1 clk en.*/
#define HP_CLKRST_PVT_PERI_GROUP1_CLK_EN    (BIT(11))
#define HP_CLKRST_PVT_PERI_GROUP1_CLK_EN_M  (BIT(11))
#define HP_CLKRST_PVT_PERI_GROUP1_CLK_EN_V  0x1
#define HP_CLKRST_PVT_PERI_GROUP1_CLK_EN_S  11
/* HP_CLKRST_PVT_CPU_GROUP2_CLK_EN : R/W ;bitpos:[10] ;default: 1'b1 ; */
/*description: pvt cpu group2 clk en.*/
#define HP_CLKRST_PVT_CPU_GROUP2_CLK_EN    (BIT(10))
#define HP_CLKRST_PVT_CPU_GROUP2_CLK_EN_M  (BIT(10))
#define HP_CLKRST_PVT_CPU_GROUP2_CLK_EN_V  0x1
#define HP_CLKRST_PVT_CPU_GROUP2_CLK_EN_S  10
/* HP_CLKRST_PVT_CPU_GROUP1_CLK_EN : R/W ;bitpos:[9] ;default: 1'b1 ; */
/*description: pvt cpu group1 clk en.*/
#define HP_CLKRST_PVT_CPU_GROUP1_CLK_EN    (BIT(9))
#define HP_CLKRST_PVT_CPU_GROUP1_CLK_EN_M  (BIT(9))
#define HP_CLKRST_PVT_CPU_GROUP1_CLK_EN_V  0x1
#define HP_CLKRST_PVT_CPU_GROUP1_CLK_EN_S  9
/* HP_CLKRST_PVT_TOP_CLK_EN : R/W ;bitpos:[8] ;default: 1'b1 ; */
/*description: pvt top clock en.*/
#define HP_CLKRST_PVT_TOP_CLK_EN    (BIT(8))
#define HP_CLKRST_PVT_TOP_CLK_EN_M  (BIT(8))
#define HP_CLKRST_PVT_TOP_CLK_EN_V  0x1
#define HP_CLKRST_PVT_TOP_CLK_EN_S  8
/* HP_CLKRST_PVT_CLK_DIV_NUM : R/W ;bitpos:[7:4] ;default: 4'h1 ; */
/*description: pvt clock div number.*/
#define HP_CLKRST_PVT_CLK_DIV_NUM    0x0000000F
#define HP_CLKRST_PVT_CLK_DIV_NUM_M  ((HP_CLKRST_PVT_CLK_DIV_NUM_V)<<(HP_CLKRST_PVT_CLK_DIV_NUM_S))
#define HP_CLKRST_PVT_CLK_DIV_NUM_V  0xF
#define HP_CLKRST_PVT_CLK_DIV_NUM_S  4
/* HP_CLKRST_PVT_CLK_SEL : R/W ;bitpos:[1:0] ;default: 2'h1 ; */
/*description: pvt clock sel.*/
#define HP_CLKRST_PVT_CLK_SEL    0x00000003
#define HP_CLKRST_PVT_CLK_SEL_M  ((HP_CLKRST_PVT_CLK_SEL_V)<<(HP_CLKRST_PVT_CLK_SEL_S))
#define HP_CLKRST_PVT_CLK_SEL_V  0x3
#define HP_CLKRST_PVT_CLK_SEL_S  0

#define HP_CLKRST_TEST_PLL_CTRL_REG          (DR_REG_HP_CLKRST_BASE + 0x48)
/* HP_CLKRST_TEST_PLL_DIV_NUM : R/W ;bitpos:[27:16] ;default: 12'h3e7 ; */
/*description: test pll divider number.*/
#define HP_CLKRST_TEST_PLL_DIV_NUM    0x00000FFF
#define HP_CLKRST_TEST_PLL_DIV_NUM_M  ((HP_CLKRST_TEST_PLL_DIV_NUM_V)<<(HP_CLKRST_TEST_PLL_DIV_NUM_S))
#define HP_CLKRST_TEST_PLL_DIV_NUM_V  0xFFF
#define HP_CLKRST_TEST_PLL_DIV_NUM_S  16
/* HP_CLKRST_TEST_PLL_SEL : R/W ;bitpos:[2:0] ;default: 3'h0 ; */
/*description: test pll source select; 3'h0: RSVD; 3'h1: system PLL; 3'h2: CPU PLL; 3'h3: MPSI
DLL; 3'h4: SDIO PLL CK0; 3'h5: SDIO PLL CK1; 3'h6: SDIO PLL CK2; 3'h7: AUDIO APL
L.*/
#define HP_CLKRST_TEST_PLL_SEL    0x00000007
#define HP_CLKRST_TEST_PLL_SEL_M  ((HP_CLKRST_TEST_PLL_SEL_V)<<(HP_CLKRST_TEST_PLL_SEL_S))
#define HP_CLKRST_TEST_PLL_SEL_V  0x7
#define HP_CLKRST_TEST_PLL_SEL_S  0


#ifdef __cplusplus
}
#endif



#endif /*_SOC_HP_CLKRST_REG_H_ */
