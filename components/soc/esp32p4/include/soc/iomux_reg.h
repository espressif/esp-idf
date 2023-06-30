/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** IO_MUX_gpio0_REG register
 *  iomux control register for gpio0
 */
#define IO_MUX_GPIO0_REG (DR_REG_IO_MUX_BASE + 0x4)
/** IO_MUX_GPIO0_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_OE    (BIT(0))
#define IO_MUX_GPIO0_MCU_OE_M  (IO_MUX_GPIO0_MCU_OE_V << IO_MUX_GPIO0_MCU_OE_S)
#define IO_MUX_GPIO0_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_OE_S  0
/** IO_MUX_GPIO0_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO0_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO0_SLP_SEL_M  (IO_MUX_GPIO0_SLP_SEL_V << IO_MUX_GPIO0_SLP_SEL_S)
#define IO_MUX_GPIO0_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO0_SLP_SEL_S  1
/** IO_MUX_GPIO0_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO0_MCU_WPD_M  (IO_MUX_GPIO0_MCU_WPD_V << IO_MUX_GPIO0_MCU_WPD_S)
#define IO_MUX_GPIO0_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPD_S  2
/** IO_MUX_GPIO0_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO0_MCU_WPU_M  (IO_MUX_GPIO0_MCU_WPU_V << IO_MUX_GPIO0_MCU_WPU_S)
#define IO_MUX_GPIO0_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPU_S  3
/** IO_MUX_GPIO0_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO0_MCU_IE    (BIT(4))
#define IO_MUX_GPIO0_MCU_IE_M  (IO_MUX_GPIO0_MCU_IE_V << IO_MUX_GPIO0_MCU_IE_S)
#define IO_MUX_GPIO0_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_IE_S  4
/** IO_MUX_GPIO0_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO0_MCU_DRV    0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_M  (IO_MUX_GPIO0_MCU_DRV_V << IO_MUX_GPIO0_MCU_DRV_S)
#define IO_MUX_GPIO0_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_S  5
/** IO_MUX_GPIO0_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO0_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO0_FUN_WPD_M  (IO_MUX_GPIO0_FUN_WPD_V << IO_MUX_GPIO0_FUN_WPD_S)
#define IO_MUX_GPIO0_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPD_S  7
/** IO_MUX_GPIO0_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO0_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO0_FUN_WPU_M  (IO_MUX_GPIO0_FUN_WPU_V << IO_MUX_GPIO0_FUN_WPU_S)
#define IO_MUX_GPIO0_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPU_S  8
/** IO_MUX_GPIO0_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO0_FUN_IE    (BIT(9))
#define IO_MUX_GPIO0_FUN_IE_M  (IO_MUX_GPIO0_FUN_IE_V << IO_MUX_GPIO0_FUN_IE_S)
#define IO_MUX_GPIO0_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO0_FUN_IE_S  9
/** IO_MUX_GPIO0_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO0_FUN_DRV    0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_M  (IO_MUX_GPIO0_FUN_DRV_V << IO_MUX_GPIO0_FUN_DRV_S)
#define IO_MUX_GPIO0_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_S  10
/** IO_MUX_GPIO0_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO0_MCU_SEL    0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_M  (IO_MUX_GPIO0_MCU_SEL_V << IO_MUX_GPIO0_MCU_SEL_S)
#define IO_MUX_GPIO0_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_S  12
/** IO_MUX_GPIO0_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO0_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO0_FILTER_EN_M  (IO_MUX_GPIO0_FILTER_EN_V << IO_MUX_GPIO0_FILTER_EN_S)
#define IO_MUX_GPIO0_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO0_FILTER_EN_S  15

/** IO_MUX_gpio1_REG register
 *  iomux control register for gpio1
 */
#define IO_MUX_GPIO1_REG (DR_REG_IO_MUX_BASE + 0x8)
/** IO_MUX_GPIO1_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_OE    (BIT(0))
#define IO_MUX_GPIO1_MCU_OE_M  (IO_MUX_GPIO1_MCU_OE_V << IO_MUX_GPIO1_MCU_OE_S)
#define IO_MUX_GPIO1_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_OE_S  0
/** IO_MUX_GPIO1_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO1_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO1_SLP_SEL_M  (IO_MUX_GPIO1_SLP_SEL_V << IO_MUX_GPIO1_SLP_SEL_S)
#define IO_MUX_GPIO1_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO1_SLP_SEL_S  1
/** IO_MUX_GPIO1_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO1_MCU_WPD_M  (IO_MUX_GPIO1_MCU_WPD_V << IO_MUX_GPIO1_MCU_WPD_S)
#define IO_MUX_GPIO1_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPD_S  2
/** IO_MUX_GPIO1_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO1_MCU_WPU_M  (IO_MUX_GPIO1_MCU_WPU_V << IO_MUX_GPIO1_MCU_WPU_S)
#define IO_MUX_GPIO1_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPU_S  3
/** IO_MUX_GPIO1_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO1_MCU_IE    (BIT(4))
#define IO_MUX_GPIO1_MCU_IE_M  (IO_MUX_GPIO1_MCU_IE_V << IO_MUX_GPIO1_MCU_IE_S)
#define IO_MUX_GPIO1_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_IE_S  4
/** IO_MUX_GPIO1_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO1_MCU_DRV    0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_M  (IO_MUX_GPIO1_MCU_DRV_V << IO_MUX_GPIO1_MCU_DRV_S)
#define IO_MUX_GPIO1_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_S  5
/** IO_MUX_GPIO1_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO1_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO1_FUN_WPD_M  (IO_MUX_GPIO1_FUN_WPD_V << IO_MUX_GPIO1_FUN_WPD_S)
#define IO_MUX_GPIO1_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPD_S  7
/** IO_MUX_GPIO1_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO1_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO1_FUN_WPU_M  (IO_MUX_GPIO1_FUN_WPU_V << IO_MUX_GPIO1_FUN_WPU_S)
#define IO_MUX_GPIO1_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPU_S  8
/** IO_MUX_GPIO1_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO1_FUN_IE    (BIT(9))
#define IO_MUX_GPIO1_FUN_IE_M  (IO_MUX_GPIO1_FUN_IE_V << IO_MUX_GPIO1_FUN_IE_S)
#define IO_MUX_GPIO1_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO1_FUN_IE_S  9
/** IO_MUX_GPIO1_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO1_FUN_DRV    0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_M  (IO_MUX_GPIO1_FUN_DRV_V << IO_MUX_GPIO1_FUN_DRV_S)
#define IO_MUX_GPIO1_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_S  10
/** IO_MUX_GPIO1_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO1_MCU_SEL    0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_M  (IO_MUX_GPIO1_MCU_SEL_V << IO_MUX_GPIO1_MCU_SEL_S)
#define IO_MUX_GPIO1_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_S  12
/** IO_MUX_GPIO1_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO1_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO1_FILTER_EN_M  (IO_MUX_GPIO1_FILTER_EN_V << IO_MUX_GPIO1_FILTER_EN_S)
#define IO_MUX_GPIO1_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO1_FILTER_EN_S  15

/** IO_MUX_gpio2_REG register
 *  iomux control register for gpio2
 */
#define IO_MUX_GPIO2_REG (DR_REG_IO_MUX_BASE + 0xc)
/** IO_MUX_GPIO2_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_OE    (BIT(0))
#define IO_MUX_GPIO2_MCU_OE_M  (IO_MUX_GPIO2_MCU_OE_V << IO_MUX_GPIO2_MCU_OE_S)
#define IO_MUX_GPIO2_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_OE_S  0
/** IO_MUX_GPIO2_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO2_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO2_SLP_SEL_M  (IO_MUX_GPIO2_SLP_SEL_V << IO_MUX_GPIO2_SLP_SEL_S)
#define IO_MUX_GPIO2_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO2_SLP_SEL_S  1
/** IO_MUX_GPIO2_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO2_MCU_WPD_M  (IO_MUX_GPIO2_MCU_WPD_V << IO_MUX_GPIO2_MCU_WPD_S)
#define IO_MUX_GPIO2_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPD_S  2
/** IO_MUX_GPIO2_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO2_MCU_WPU_M  (IO_MUX_GPIO2_MCU_WPU_V << IO_MUX_GPIO2_MCU_WPU_S)
#define IO_MUX_GPIO2_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPU_S  3
/** IO_MUX_GPIO2_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO2_MCU_IE    (BIT(4))
#define IO_MUX_GPIO2_MCU_IE_M  (IO_MUX_GPIO2_MCU_IE_V << IO_MUX_GPIO2_MCU_IE_S)
#define IO_MUX_GPIO2_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_IE_S  4
/** IO_MUX_GPIO2_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO2_MCU_DRV    0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_M  (IO_MUX_GPIO2_MCU_DRV_V << IO_MUX_GPIO2_MCU_DRV_S)
#define IO_MUX_GPIO2_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_S  5
/** IO_MUX_GPIO2_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO2_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO2_FUN_WPD_M  (IO_MUX_GPIO2_FUN_WPD_V << IO_MUX_GPIO2_FUN_WPD_S)
#define IO_MUX_GPIO2_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPD_S  7
/** IO_MUX_GPIO2_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO2_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO2_FUN_WPU_M  (IO_MUX_GPIO2_FUN_WPU_V << IO_MUX_GPIO2_FUN_WPU_S)
#define IO_MUX_GPIO2_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPU_S  8
/** IO_MUX_GPIO2_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO2_FUN_IE    (BIT(9))
#define IO_MUX_GPIO2_FUN_IE_M  (IO_MUX_GPIO2_FUN_IE_V << IO_MUX_GPIO2_FUN_IE_S)
#define IO_MUX_GPIO2_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO2_FUN_IE_S  9
/** IO_MUX_GPIO2_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO2_FUN_DRV    0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_M  (IO_MUX_GPIO2_FUN_DRV_V << IO_MUX_GPIO2_FUN_DRV_S)
#define IO_MUX_GPIO2_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_S  10
/** IO_MUX_GPIO2_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO2_MCU_SEL    0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_M  (IO_MUX_GPIO2_MCU_SEL_V << IO_MUX_GPIO2_MCU_SEL_S)
#define IO_MUX_GPIO2_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_S  12
/** IO_MUX_GPIO2_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO2_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO2_FILTER_EN_M  (IO_MUX_GPIO2_FILTER_EN_V << IO_MUX_GPIO2_FILTER_EN_S)
#define IO_MUX_GPIO2_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO2_FILTER_EN_S  15

/** IO_MUX_gpio3_REG register
 *  iomux control register for gpio3
 */
#define IO_MUX_GPIO3_REG (DR_REG_IO_MUX_BASE + 0x10)
/** IO_MUX_GPIO3_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_OE    (BIT(0))
#define IO_MUX_GPIO3_MCU_OE_M  (IO_MUX_GPIO3_MCU_OE_V << IO_MUX_GPIO3_MCU_OE_S)
#define IO_MUX_GPIO3_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_OE_S  0
/** IO_MUX_GPIO3_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO3_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO3_SLP_SEL_M  (IO_MUX_GPIO3_SLP_SEL_V << IO_MUX_GPIO3_SLP_SEL_S)
#define IO_MUX_GPIO3_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO3_SLP_SEL_S  1
/** IO_MUX_GPIO3_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO3_MCU_WPD_M  (IO_MUX_GPIO3_MCU_WPD_V << IO_MUX_GPIO3_MCU_WPD_S)
#define IO_MUX_GPIO3_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPD_S  2
/** IO_MUX_GPIO3_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO3_MCU_WPU_M  (IO_MUX_GPIO3_MCU_WPU_V << IO_MUX_GPIO3_MCU_WPU_S)
#define IO_MUX_GPIO3_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPU_S  3
/** IO_MUX_GPIO3_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO3_MCU_IE    (BIT(4))
#define IO_MUX_GPIO3_MCU_IE_M  (IO_MUX_GPIO3_MCU_IE_V << IO_MUX_GPIO3_MCU_IE_S)
#define IO_MUX_GPIO3_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_IE_S  4
/** IO_MUX_GPIO3_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO3_MCU_DRV    0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_M  (IO_MUX_GPIO3_MCU_DRV_V << IO_MUX_GPIO3_MCU_DRV_S)
#define IO_MUX_GPIO3_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_S  5
/** IO_MUX_GPIO3_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO3_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO3_FUN_WPD_M  (IO_MUX_GPIO3_FUN_WPD_V << IO_MUX_GPIO3_FUN_WPD_S)
#define IO_MUX_GPIO3_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPD_S  7
/** IO_MUX_GPIO3_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO3_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO3_FUN_WPU_M  (IO_MUX_GPIO3_FUN_WPU_V << IO_MUX_GPIO3_FUN_WPU_S)
#define IO_MUX_GPIO3_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPU_S  8
/** IO_MUX_GPIO3_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO3_FUN_IE    (BIT(9))
#define IO_MUX_GPIO3_FUN_IE_M  (IO_MUX_GPIO3_FUN_IE_V << IO_MUX_GPIO3_FUN_IE_S)
#define IO_MUX_GPIO3_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO3_FUN_IE_S  9
/** IO_MUX_GPIO3_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO3_FUN_DRV    0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_M  (IO_MUX_GPIO3_FUN_DRV_V << IO_MUX_GPIO3_FUN_DRV_S)
#define IO_MUX_GPIO3_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_S  10
/** IO_MUX_GPIO3_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO3_MCU_SEL    0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_M  (IO_MUX_GPIO3_MCU_SEL_V << IO_MUX_GPIO3_MCU_SEL_S)
#define IO_MUX_GPIO3_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_S  12
/** IO_MUX_GPIO3_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO3_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO3_FILTER_EN_M  (IO_MUX_GPIO3_FILTER_EN_V << IO_MUX_GPIO3_FILTER_EN_S)
#define IO_MUX_GPIO3_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO3_FILTER_EN_S  15

/** IO_MUX_gpio4_REG register
 *  iomux control register for gpio4
 */
#define IO_MUX_GPIO4_REG (DR_REG_IO_MUX_BASE + 0x14)
/** IO_MUX_GPIO4_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_OE    (BIT(0))
#define IO_MUX_GPIO4_MCU_OE_M  (IO_MUX_GPIO4_MCU_OE_V << IO_MUX_GPIO4_MCU_OE_S)
#define IO_MUX_GPIO4_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_OE_S  0
/** IO_MUX_GPIO4_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO4_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO4_SLP_SEL_M  (IO_MUX_GPIO4_SLP_SEL_V << IO_MUX_GPIO4_SLP_SEL_S)
#define IO_MUX_GPIO4_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO4_SLP_SEL_S  1
/** IO_MUX_GPIO4_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO4_MCU_WPD_M  (IO_MUX_GPIO4_MCU_WPD_V << IO_MUX_GPIO4_MCU_WPD_S)
#define IO_MUX_GPIO4_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPD_S  2
/** IO_MUX_GPIO4_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO4_MCU_WPU_M  (IO_MUX_GPIO4_MCU_WPU_V << IO_MUX_GPIO4_MCU_WPU_S)
#define IO_MUX_GPIO4_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPU_S  3
/** IO_MUX_GPIO4_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO4_MCU_IE    (BIT(4))
#define IO_MUX_GPIO4_MCU_IE_M  (IO_MUX_GPIO4_MCU_IE_V << IO_MUX_GPIO4_MCU_IE_S)
#define IO_MUX_GPIO4_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_IE_S  4
/** IO_MUX_GPIO4_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO4_MCU_DRV    0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_M  (IO_MUX_GPIO4_MCU_DRV_V << IO_MUX_GPIO4_MCU_DRV_S)
#define IO_MUX_GPIO4_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_S  5
/** IO_MUX_GPIO4_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO4_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO4_FUN_WPD_M  (IO_MUX_GPIO4_FUN_WPD_V << IO_MUX_GPIO4_FUN_WPD_S)
#define IO_MUX_GPIO4_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPD_S  7
/** IO_MUX_GPIO4_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO4_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO4_FUN_WPU_M  (IO_MUX_GPIO4_FUN_WPU_V << IO_MUX_GPIO4_FUN_WPU_S)
#define IO_MUX_GPIO4_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPU_S  8
/** IO_MUX_GPIO4_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO4_FUN_IE    (BIT(9))
#define IO_MUX_GPIO4_FUN_IE_M  (IO_MUX_GPIO4_FUN_IE_V << IO_MUX_GPIO4_FUN_IE_S)
#define IO_MUX_GPIO4_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO4_FUN_IE_S  9
/** IO_MUX_GPIO4_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO4_FUN_DRV    0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_M  (IO_MUX_GPIO4_FUN_DRV_V << IO_MUX_GPIO4_FUN_DRV_S)
#define IO_MUX_GPIO4_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_S  10
/** IO_MUX_GPIO4_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO4_MCU_SEL    0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_M  (IO_MUX_GPIO4_MCU_SEL_V << IO_MUX_GPIO4_MCU_SEL_S)
#define IO_MUX_GPIO4_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_S  12
/** IO_MUX_GPIO4_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO4_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO4_FILTER_EN_M  (IO_MUX_GPIO4_FILTER_EN_V << IO_MUX_GPIO4_FILTER_EN_S)
#define IO_MUX_GPIO4_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO4_FILTER_EN_S  15

/** IO_MUX_gpio5_REG register
 *  iomux control register for gpio5
 */
#define IO_MUX_GPIO5_REG (DR_REG_IO_MUX_BASE + 0x18)
/** IO_MUX_GPIO5_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_OE    (BIT(0))
#define IO_MUX_GPIO5_MCU_OE_M  (IO_MUX_GPIO5_MCU_OE_V << IO_MUX_GPIO5_MCU_OE_S)
#define IO_MUX_GPIO5_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_OE_S  0
/** IO_MUX_GPIO5_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO5_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO5_SLP_SEL_M  (IO_MUX_GPIO5_SLP_SEL_V << IO_MUX_GPIO5_SLP_SEL_S)
#define IO_MUX_GPIO5_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO5_SLP_SEL_S  1
/** IO_MUX_GPIO5_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO5_MCU_WPD_M  (IO_MUX_GPIO5_MCU_WPD_V << IO_MUX_GPIO5_MCU_WPD_S)
#define IO_MUX_GPIO5_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPD_S  2
/** IO_MUX_GPIO5_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO5_MCU_WPU_M  (IO_MUX_GPIO5_MCU_WPU_V << IO_MUX_GPIO5_MCU_WPU_S)
#define IO_MUX_GPIO5_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPU_S  3
/** IO_MUX_GPIO5_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO5_MCU_IE    (BIT(4))
#define IO_MUX_GPIO5_MCU_IE_M  (IO_MUX_GPIO5_MCU_IE_V << IO_MUX_GPIO5_MCU_IE_S)
#define IO_MUX_GPIO5_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_IE_S  4
/** IO_MUX_GPIO5_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO5_MCU_DRV    0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_M  (IO_MUX_GPIO5_MCU_DRV_V << IO_MUX_GPIO5_MCU_DRV_S)
#define IO_MUX_GPIO5_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_S  5
/** IO_MUX_GPIO5_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO5_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO5_FUN_WPD_M  (IO_MUX_GPIO5_FUN_WPD_V << IO_MUX_GPIO5_FUN_WPD_S)
#define IO_MUX_GPIO5_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPD_S  7
/** IO_MUX_GPIO5_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO5_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO5_FUN_WPU_M  (IO_MUX_GPIO5_FUN_WPU_V << IO_MUX_GPIO5_FUN_WPU_S)
#define IO_MUX_GPIO5_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPU_S  8
/** IO_MUX_GPIO5_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO5_FUN_IE    (BIT(9))
#define IO_MUX_GPIO5_FUN_IE_M  (IO_MUX_GPIO5_FUN_IE_V << IO_MUX_GPIO5_FUN_IE_S)
#define IO_MUX_GPIO5_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO5_FUN_IE_S  9
/** IO_MUX_GPIO5_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO5_FUN_DRV    0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_M  (IO_MUX_GPIO5_FUN_DRV_V << IO_MUX_GPIO5_FUN_DRV_S)
#define IO_MUX_GPIO5_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_S  10
/** IO_MUX_GPIO5_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO5_MCU_SEL    0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_M  (IO_MUX_GPIO5_MCU_SEL_V << IO_MUX_GPIO5_MCU_SEL_S)
#define IO_MUX_GPIO5_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_S  12
/** IO_MUX_GPIO5_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO5_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO5_FILTER_EN_M  (IO_MUX_GPIO5_FILTER_EN_V << IO_MUX_GPIO5_FILTER_EN_S)
#define IO_MUX_GPIO5_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO5_FILTER_EN_S  15

/** IO_MUX_gpio6_REG register
 *  iomux control register for gpio6
 */
#define IO_MUX_GPIO6_REG (DR_REG_IO_MUX_BASE + 0x1c)
/** IO_MUX_GPIO6_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_OE    (BIT(0))
#define IO_MUX_GPIO6_MCU_OE_M  (IO_MUX_GPIO6_MCU_OE_V << IO_MUX_GPIO6_MCU_OE_S)
#define IO_MUX_GPIO6_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_OE_S  0
/** IO_MUX_GPIO6_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO6_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO6_SLP_SEL_M  (IO_MUX_GPIO6_SLP_SEL_V << IO_MUX_GPIO6_SLP_SEL_S)
#define IO_MUX_GPIO6_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO6_SLP_SEL_S  1
/** IO_MUX_GPIO6_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO6_MCU_WPD_M  (IO_MUX_GPIO6_MCU_WPD_V << IO_MUX_GPIO6_MCU_WPD_S)
#define IO_MUX_GPIO6_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPD_S  2
/** IO_MUX_GPIO6_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO6_MCU_WPU_M  (IO_MUX_GPIO6_MCU_WPU_V << IO_MUX_GPIO6_MCU_WPU_S)
#define IO_MUX_GPIO6_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPU_S  3
/** IO_MUX_GPIO6_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO6_MCU_IE    (BIT(4))
#define IO_MUX_GPIO6_MCU_IE_M  (IO_MUX_GPIO6_MCU_IE_V << IO_MUX_GPIO6_MCU_IE_S)
#define IO_MUX_GPIO6_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_IE_S  4
/** IO_MUX_GPIO6_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO6_MCU_DRV    0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_M  (IO_MUX_GPIO6_MCU_DRV_V << IO_MUX_GPIO6_MCU_DRV_S)
#define IO_MUX_GPIO6_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_S  5
/** IO_MUX_GPIO6_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO6_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO6_FUN_WPD_M  (IO_MUX_GPIO6_FUN_WPD_V << IO_MUX_GPIO6_FUN_WPD_S)
#define IO_MUX_GPIO6_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPD_S  7
/** IO_MUX_GPIO6_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO6_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO6_FUN_WPU_M  (IO_MUX_GPIO6_FUN_WPU_V << IO_MUX_GPIO6_FUN_WPU_S)
#define IO_MUX_GPIO6_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPU_S  8
/** IO_MUX_GPIO6_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO6_FUN_IE    (BIT(9))
#define IO_MUX_GPIO6_FUN_IE_M  (IO_MUX_GPIO6_FUN_IE_V << IO_MUX_GPIO6_FUN_IE_S)
#define IO_MUX_GPIO6_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO6_FUN_IE_S  9
/** IO_MUX_GPIO6_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO6_FUN_DRV    0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_M  (IO_MUX_GPIO6_FUN_DRV_V << IO_MUX_GPIO6_FUN_DRV_S)
#define IO_MUX_GPIO6_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_S  10
/** IO_MUX_GPIO6_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO6_MCU_SEL    0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_M  (IO_MUX_GPIO6_MCU_SEL_V << IO_MUX_GPIO6_MCU_SEL_S)
#define IO_MUX_GPIO6_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_S  12
/** IO_MUX_GPIO6_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO6_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO6_FILTER_EN_M  (IO_MUX_GPIO6_FILTER_EN_V << IO_MUX_GPIO6_FILTER_EN_S)
#define IO_MUX_GPIO6_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO6_FILTER_EN_S  15

/** IO_MUX_gpio7_REG register
 *  iomux control register for gpio7
 */
#define IO_MUX_GPIO7_REG (DR_REG_IO_MUX_BASE + 0x20)
/** IO_MUX_GPIO7_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_OE    (BIT(0))
#define IO_MUX_GPIO7_MCU_OE_M  (IO_MUX_GPIO7_MCU_OE_V << IO_MUX_GPIO7_MCU_OE_S)
#define IO_MUX_GPIO7_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_OE_S  0
/** IO_MUX_GPIO7_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO7_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO7_SLP_SEL_M  (IO_MUX_GPIO7_SLP_SEL_V << IO_MUX_GPIO7_SLP_SEL_S)
#define IO_MUX_GPIO7_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO7_SLP_SEL_S  1
/** IO_MUX_GPIO7_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO7_MCU_WPD_M  (IO_MUX_GPIO7_MCU_WPD_V << IO_MUX_GPIO7_MCU_WPD_S)
#define IO_MUX_GPIO7_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPD_S  2
/** IO_MUX_GPIO7_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO7_MCU_WPU_M  (IO_MUX_GPIO7_MCU_WPU_V << IO_MUX_GPIO7_MCU_WPU_S)
#define IO_MUX_GPIO7_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPU_S  3
/** IO_MUX_GPIO7_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO7_MCU_IE    (BIT(4))
#define IO_MUX_GPIO7_MCU_IE_M  (IO_MUX_GPIO7_MCU_IE_V << IO_MUX_GPIO7_MCU_IE_S)
#define IO_MUX_GPIO7_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_IE_S  4
/** IO_MUX_GPIO7_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO7_MCU_DRV    0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_M  (IO_MUX_GPIO7_MCU_DRV_V << IO_MUX_GPIO7_MCU_DRV_S)
#define IO_MUX_GPIO7_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_S  5
/** IO_MUX_GPIO7_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO7_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO7_FUN_WPD_M  (IO_MUX_GPIO7_FUN_WPD_V << IO_MUX_GPIO7_FUN_WPD_S)
#define IO_MUX_GPIO7_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPD_S  7
/** IO_MUX_GPIO7_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO7_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO7_FUN_WPU_M  (IO_MUX_GPIO7_FUN_WPU_V << IO_MUX_GPIO7_FUN_WPU_S)
#define IO_MUX_GPIO7_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPU_S  8
/** IO_MUX_GPIO7_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO7_FUN_IE    (BIT(9))
#define IO_MUX_GPIO7_FUN_IE_M  (IO_MUX_GPIO7_FUN_IE_V << IO_MUX_GPIO7_FUN_IE_S)
#define IO_MUX_GPIO7_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO7_FUN_IE_S  9
/** IO_MUX_GPIO7_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO7_FUN_DRV    0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_M  (IO_MUX_GPIO7_FUN_DRV_V << IO_MUX_GPIO7_FUN_DRV_S)
#define IO_MUX_GPIO7_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_S  10
/** IO_MUX_GPIO7_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO7_MCU_SEL    0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_M  (IO_MUX_GPIO7_MCU_SEL_V << IO_MUX_GPIO7_MCU_SEL_S)
#define IO_MUX_GPIO7_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_S  12
/** IO_MUX_GPIO7_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO7_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO7_FILTER_EN_M  (IO_MUX_GPIO7_FILTER_EN_V << IO_MUX_GPIO7_FILTER_EN_S)
#define IO_MUX_GPIO7_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO7_FILTER_EN_S  15

/** IO_MUX_gpio8_REG register
 *  iomux control register for gpio8
 */
#define IO_MUX_GPIO8_REG (DR_REG_IO_MUX_BASE + 0x24)
/** IO_MUX_GPIO8_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_OE    (BIT(0))
#define IO_MUX_GPIO8_MCU_OE_M  (IO_MUX_GPIO8_MCU_OE_V << IO_MUX_GPIO8_MCU_OE_S)
#define IO_MUX_GPIO8_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_OE_S  0
/** IO_MUX_GPIO8_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO8_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO8_SLP_SEL_M  (IO_MUX_GPIO8_SLP_SEL_V << IO_MUX_GPIO8_SLP_SEL_S)
#define IO_MUX_GPIO8_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO8_SLP_SEL_S  1
/** IO_MUX_GPIO8_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO8_MCU_WPD_M  (IO_MUX_GPIO8_MCU_WPD_V << IO_MUX_GPIO8_MCU_WPD_S)
#define IO_MUX_GPIO8_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPD_S  2
/** IO_MUX_GPIO8_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO8_MCU_WPU_M  (IO_MUX_GPIO8_MCU_WPU_V << IO_MUX_GPIO8_MCU_WPU_S)
#define IO_MUX_GPIO8_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPU_S  3
/** IO_MUX_GPIO8_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO8_MCU_IE    (BIT(4))
#define IO_MUX_GPIO8_MCU_IE_M  (IO_MUX_GPIO8_MCU_IE_V << IO_MUX_GPIO8_MCU_IE_S)
#define IO_MUX_GPIO8_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_IE_S  4
/** IO_MUX_GPIO8_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO8_MCU_DRV    0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_M  (IO_MUX_GPIO8_MCU_DRV_V << IO_MUX_GPIO8_MCU_DRV_S)
#define IO_MUX_GPIO8_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_S  5
/** IO_MUX_GPIO8_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO8_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO8_FUN_WPD_M  (IO_MUX_GPIO8_FUN_WPD_V << IO_MUX_GPIO8_FUN_WPD_S)
#define IO_MUX_GPIO8_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPD_S  7
/** IO_MUX_GPIO8_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO8_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO8_FUN_WPU_M  (IO_MUX_GPIO8_FUN_WPU_V << IO_MUX_GPIO8_FUN_WPU_S)
#define IO_MUX_GPIO8_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPU_S  8
/** IO_MUX_GPIO8_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO8_FUN_IE    (BIT(9))
#define IO_MUX_GPIO8_FUN_IE_M  (IO_MUX_GPIO8_FUN_IE_V << IO_MUX_GPIO8_FUN_IE_S)
#define IO_MUX_GPIO8_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO8_FUN_IE_S  9
/** IO_MUX_GPIO8_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO8_FUN_DRV    0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_M  (IO_MUX_GPIO8_FUN_DRV_V << IO_MUX_GPIO8_FUN_DRV_S)
#define IO_MUX_GPIO8_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_S  10
/** IO_MUX_GPIO8_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO8_MCU_SEL    0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_M  (IO_MUX_GPIO8_MCU_SEL_V << IO_MUX_GPIO8_MCU_SEL_S)
#define IO_MUX_GPIO8_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_S  12
/** IO_MUX_GPIO8_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO8_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO8_FILTER_EN_M  (IO_MUX_GPIO8_FILTER_EN_V << IO_MUX_GPIO8_FILTER_EN_S)
#define IO_MUX_GPIO8_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO8_FILTER_EN_S  15

/** IO_MUX_gpio9_REG register
 *  iomux control register for gpio9
 */
#define IO_MUX_GPIO9_REG (DR_REG_IO_MUX_BASE + 0x28)
/** IO_MUX_GPIO9_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_OE    (BIT(0))
#define IO_MUX_GPIO9_MCU_OE_M  (IO_MUX_GPIO9_MCU_OE_V << IO_MUX_GPIO9_MCU_OE_S)
#define IO_MUX_GPIO9_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_OE_S  0
/** IO_MUX_GPIO9_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO9_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO9_SLP_SEL_M  (IO_MUX_GPIO9_SLP_SEL_V << IO_MUX_GPIO9_SLP_SEL_S)
#define IO_MUX_GPIO9_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO9_SLP_SEL_S  1
/** IO_MUX_GPIO9_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO9_MCU_WPD_M  (IO_MUX_GPIO9_MCU_WPD_V << IO_MUX_GPIO9_MCU_WPD_S)
#define IO_MUX_GPIO9_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPD_S  2
/** IO_MUX_GPIO9_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO9_MCU_WPU_M  (IO_MUX_GPIO9_MCU_WPU_V << IO_MUX_GPIO9_MCU_WPU_S)
#define IO_MUX_GPIO9_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPU_S  3
/** IO_MUX_GPIO9_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO9_MCU_IE    (BIT(4))
#define IO_MUX_GPIO9_MCU_IE_M  (IO_MUX_GPIO9_MCU_IE_V << IO_MUX_GPIO9_MCU_IE_S)
#define IO_MUX_GPIO9_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_IE_S  4
/** IO_MUX_GPIO9_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO9_MCU_DRV    0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_M  (IO_MUX_GPIO9_MCU_DRV_V << IO_MUX_GPIO9_MCU_DRV_S)
#define IO_MUX_GPIO9_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_S  5
/** IO_MUX_GPIO9_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO9_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO9_FUN_WPD_M  (IO_MUX_GPIO9_FUN_WPD_V << IO_MUX_GPIO9_FUN_WPD_S)
#define IO_MUX_GPIO9_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPD_S  7
/** IO_MUX_GPIO9_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO9_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO9_FUN_WPU_M  (IO_MUX_GPIO9_FUN_WPU_V << IO_MUX_GPIO9_FUN_WPU_S)
#define IO_MUX_GPIO9_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPU_S  8
/** IO_MUX_GPIO9_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO9_FUN_IE    (BIT(9))
#define IO_MUX_GPIO9_FUN_IE_M  (IO_MUX_GPIO9_FUN_IE_V << IO_MUX_GPIO9_FUN_IE_S)
#define IO_MUX_GPIO9_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO9_FUN_IE_S  9
/** IO_MUX_GPIO9_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO9_FUN_DRV    0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_M  (IO_MUX_GPIO9_FUN_DRV_V << IO_MUX_GPIO9_FUN_DRV_S)
#define IO_MUX_GPIO9_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_S  10
/** IO_MUX_GPIO9_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO9_MCU_SEL    0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_M  (IO_MUX_GPIO9_MCU_SEL_V << IO_MUX_GPIO9_MCU_SEL_S)
#define IO_MUX_GPIO9_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_S  12
/** IO_MUX_GPIO9_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO9_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO9_FILTER_EN_M  (IO_MUX_GPIO9_FILTER_EN_V << IO_MUX_GPIO9_FILTER_EN_S)
#define IO_MUX_GPIO9_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO9_FILTER_EN_S  15

/** IO_MUX_gpio10_REG register
 *  iomux control register for gpio10
 */
#define IO_MUX_GPIO10_REG (DR_REG_IO_MUX_BASE + 0x2c)
/** IO_MUX_GPIO10_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_OE    (BIT(0))
#define IO_MUX_GPIO10_MCU_OE_M  (IO_MUX_GPIO10_MCU_OE_V << IO_MUX_GPIO10_MCU_OE_S)
#define IO_MUX_GPIO10_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_OE_S  0
/** IO_MUX_GPIO10_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO10_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO10_SLP_SEL_M  (IO_MUX_GPIO10_SLP_SEL_V << IO_MUX_GPIO10_SLP_SEL_S)
#define IO_MUX_GPIO10_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO10_SLP_SEL_S  1
/** IO_MUX_GPIO10_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO10_MCU_WPD_M  (IO_MUX_GPIO10_MCU_WPD_V << IO_MUX_GPIO10_MCU_WPD_S)
#define IO_MUX_GPIO10_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPD_S  2
/** IO_MUX_GPIO10_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO10_MCU_WPU_M  (IO_MUX_GPIO10_MCU_WPU_V << IO_MUX_GPIO10_MCU_WPU_S)
#define IO_MUX_GPIO10_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPU_S  3
/** IO_MUX_GPIO10_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO10_MCU_IE    (BIT(4))
#define IO_MUX_GPIO10_MCU_IE_M  (IO_MUX_GPIO10_MCU_IE_V << IO_MUX_GPIO10_MCU_IE_S)
#define IO_MUX_GPIO10_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_IE_S  4
/** IO_MUX_GPIO10_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO10_MCU_DRV    0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_M  (IO_MUX_GPIO10_MCU_DRV_V << IO_MUX_GPIO10_MCU_DRV_S)
#define IO_MUX_GPIO10_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_S  5
/** IO_MUX_GPIO10_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO10_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO10_FUN_WPD_M  (IO_MUX_GPIO10_FUN_WPD_V << IO_MUX_GPIO10_FUN_WPD_S)
#define IO_MUX_GPIO10_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPD_S  7
/** IO_MUX_GPIO10_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO10_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO10_FUN_WPU_M  (IO_MUX_GPIO10_FUN_WPU_V << IO_MUX_GPIO10_FUN_WPU_S)
#define IO_MUX_GPIO10_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPU_S  8
/** IO_MUX_GPIO10_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO10_FUN_IE    (BIT(9))
#define IO_MUX_GPIO10_FUN_IE_M  (IO_MUX_GPIO10_FUN_IE_V << IO_MUX_GPIO10_FUN_IE_S)
#define IO_MUX_GPIO10_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO10_FUN_IE_S  9
/** IO_MUX_GPIO10_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO10_FUN_DRV    0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_M  (IO_MUX_GPIO10_FUN_DRV_V << IO_MUX_GPIO10_FUN_DRV_S)
#define IO_MUX_GPIO10_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_S  10
/** IO_MUX_GPIO10_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO10_MCU_SEL    0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_M  (IO_MUX_GPIO10_MCU_SEL_V << IO_MUX_GPIO10_MCU_SEL_S)
#define IO_MUX_GPIO10_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_S  12
/** IO_MUX_GPIO10_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO10_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO10_FILTER_EN_M  (IO_MUX_GPIO10_FILTER_EN_V << IO_MUX_GPIO10_FILTER_EN_S)
#define IO_MUX_GPIO10_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO10_FILTER_EN_S  15

/** IO_MUX_gpio11_REG register
 *  iomux control register for gpio11
 */
#define IO_MUX_GPIO11_REG (DR_REG_IO_MUX_BASE + 0x30)
/** IO_MUX_GPIO11_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_OE    (BIT(0))
#define IO_MUX_GPIO11_MCU_OE_M  (IO_MUX_GPIO11_MCU_OE_V << IO_MUX_GPIO11_MCU_OE_S)
#define IO_MUX_GPIO11_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_OE_S  0
/** IO_MUX_GPIO11_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO11_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO11_SLP_SEL_M  (IO_MUX_GPIO11_SLP_SEL_V << IO_MUX_GPIO11_SLP_SEL_S)
#define IO_MUX_GPIO11_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO11_SLP_SEL_S  1
/** IO_MUX_GPIO11_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO11_MCU_WPD_M  (IO_MUX_GPIO11_MCU_WPD_V << IO_MUX_GPIO11_MCU_WPD_S)
#define IO_MUX_GPIO11_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPD_S  2
/** IO_MUX_GPIO11_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO11_MCU_WPU_M  (IO_MUX_GPIO11_MCU_WPU_V << IO_MUX_GPIO11_MCU_WPU_S)
#define IO_MUX_GPIO11_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPU_S  3
/** IO_MUX_GPIO11_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO11_MCU_IE    (BIT(4))
#define IO_MUX_GPIO11_MCU_IE_M  (IO_MUX_GPIO11_MCU_IE_V << IO_MUX_GPIO11_MCU_IE_S)
#define IO_MUX_GPIO11_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_IE_S  4
/** IO_MUX_GPIO11_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO11_MCU_DRV    0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_M  (IO_MUX_GPIO11_MCU_DRV_V << IO_MUX_GPIO11_MCU_DRV_S)
#define IO_MUX_GPIO11_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_S  5
/** IO_MUX_GPIO11_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO11_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO11_FUN_WPD_M  (IO_MUX_GPIO11_FUN_WPD_V << IO_MUX_GPIO11_FUN_WPD_S)
#define IO_MUX_GPIO11_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPD_S  7
/** IO_MUX_GPIO11_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO11_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO11_FUN_WPU_M  (IO_MUX_GPIO11_FUN_WPU_V << IO_MUX_GPIO11_FUN_WPU_S)
#define IO_MUX_GPIO11_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPU_S  8
/** IO_MUX_GPIO11_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO11_FUN_IE    (BIT(9))
#define IO_MUX_GPIO11_FUN_IE_M  (IO_MUX_GPIO11_FUN_IE_V << IO_MUX_GPIO11_FUN_IE_S)
#define IO_MUX_GPIO11_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO11_FUN_IE_S  9
/** IO_MUX_GPIO11_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO11_FUN_DRV    0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_M  (IO_MUX_GPIO11_FUN_DRV_V << IO_MUX_GPIO11_FUN_DRV_S)
#define IO_MUX_GPIO11_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_S  10
/** IO_MUX_GPIO11_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO11_MCU_SEL    0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_M  (IO_MUX_GPIO11_MCU_SEL_V << IO_MUX_GPIO11_MCU_SEL_S)
#define IO_MUX_GPIO11_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_S  12
/** IO_MUX_GPIO11_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO11_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO11_FILTER_EN_M  (IO_MUX_GPIO11_FILTER_EN_V << IO_MUX_GPIO11_FILTER_EN_S)
#define IO_MUX_GPIO11_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO11_FILTER_EN_S  15

/** IO_MUX_gpio12_REG register
 *  iomux control register for gpio12
 */
#define IO_MUX_GPIO12_REG (DR_REG_IO_MUX_BASE + 0x34)
/** IO_MUX_GPIO12_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_OE    (BIT(0))
#define IO_MUX_GPIO12_MCU_OE_M  (IO_MUX_GPIO12_MCU_OE_V << IO_MUX_GPIO12_MCU_OE_S)
#define IO_MUX_GPIO12_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_OE_S  0
/** IO_MUX_GPIO12_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO12_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO12_SLP_SEL_M  (IO_MUX_GPIO12_SLP_SEL_V << IO_MUX_GPIO12_SLP_SEL_S)
#define IO_MUX_GPIO12_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO12_SLP_SEL_S  1
/** IO_MUX_GPIO12_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO12_MCU_WPD_M  (IO_MUX_GPIO12_MCU_WPD_V << IO_MUX_GPIO12_MCU_WPD_S)
#define IO_MUX_GPIO12_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPD_S  2
/** IO_MUX_GPIO12_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO12_MCU_WPU_M  (IO_MUX_GPIO12_MCU_WPU_V << IO_MUX_GPIO12_MCU_WPU_S)
#define IO_MUX_GPIO12_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPU_S  3
/** IO_MUX_GPIO12_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO12_MCU_IE    (BIT(4))
#define IO_MUX_GPIO12_MCU_IE_M  (IO_MUX_GPIO12_MCU_IE_V << IO_MUX_GPIO12_MCU_IE_S)
#define IO_MUX_GPIO12_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_IE_S  4
/** IO_MUX_GPIO12_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO12_MCU_DRV    0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_M  (IO_MUX_GPIO12_MCU_DRV_V << IO_MUX_GPIO12_MCU_DRV_S)
#define IO_MUX_GPIO12_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_S  5
/** IO_MUX_GPIO12_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO12_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO12_FUN_WPD_M  (IO_MUX_GPIO12_FUN_WPD_V << IO_MUX_GPIO12_FUN_WPD_S)
#define IO_MUX_GPIO12_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPD_S  7
/** IO_MUX_GPIO12_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO12_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO12_FUN_WPU_M  (IO_MUX_GPIO12_FUN_WPU_V << IO_MUX_GPIO12_FUN_WPU_S)
#define IO_MUX_GPIO12_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPU_S  8
/** IO_MUX_GPIO12_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO12_FUN_IE    (BIT(9))
#define IO_MUX_GPIO12_FUN_IE_M  (IO_MUX_GPIO12_FUN_IE_V << IO_MUX_GPIO12_FUN_IE_S)
#define IO_MUX_GPIO12_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO12_FUN_IE_S  9
/** IO_MUX_GPIO12_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO12_FUN_DRV    0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_M  (IO_MUX_GPIO12_FUN_DRV_V << IO_MUX_GPIO12_FUN_DRV_S)
#define IO_MUX_GPIO12_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_S  10
/** IO_MUX_GPIO12_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO12_MCU_SEL    0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_M  (IO_MUX_GPIO12_MCU_SEL_V << IO_MUX_GPIO12_MCU_SEL_S)
#define IO_MUX_GPIO12_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_S  12
/** IO_MUX_GPIO12_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO12_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO12_FILTER_EN_M  (IO_MUX_GPIO12_FILTER_EN_V << IO_MUX_GPIO12_FILTER_EN_S)
#define IO_MUX_GPIO12_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO12_FILTER_EN_S  15

/** IO_MUX_gpio13_REG register
 *  iomux control register for gpio13
 */
#define IO_MUX_GPIO13_REG (DR_REG_IO_MUX_BASE + 0x38)
/** IO_MUX_GPIO13_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_OE    (BIT(0))
#define IO_MUX_GPIO13_MCU_OE_M  (IO_MUX_GPIO13_MCU_OE_V << IO_MUX_GPIO13_MCU_OE_S)
#define IO_MUX_GPIO13_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_OE_S  0
/** IO_MUX_GPIO13_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO13_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO13_SLP_SEL_M  (IO_MUX_GPIO13_SLP_SEL_V << IO_MUX_GPIO13_SLP_SEL_S)
#define IO_MUX_GPIO13_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO13_SLP_SEL_S  1
/** IO_MUX_GPIO13_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO13_MCU_WPD_M  (IO_MUX_GPIO13_MCU_WPD_V << IO_MUX_GPIO13_MCU_WPD_S)
#define IO_MUX_GPIO13_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPD_S  2
/** IO_MUX_GPIO13_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO13_MCU_WPU_M  (IO_MUX_GPIO13_MCU_WPU_V << IO_MUX_GPIO13_MCU_WPU_S)
#define IO_MUX_GPIO13_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPU_S  3
/** IO_MUX_GPIO13_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO13_MCU_IE    (BIT(4))
#define IO_MUX_GPIO13_MCU_IE_M  (IO_MUX_GPIO13_MCU_IE_V << IO_MUX_GPIO13_MCU_IE_S)
#define IO_MUX_GPIO13_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_IE_S  4
/** IO_MUX_GPIO13_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO13_MCU_DRV    0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_M  (IO_MUX_GPIO13_MCU_DRV_V << IO_MUX_GPIO13_MCU_DRV_S)
#define IO_MUX_GPIO13_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_S  5
/** IO_MUX_GPIO13_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO13_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO13_FUN_WPD_M  (IO_MUX_GPIO13_FUN_WPD_V << IO_MUX_GPIO13_FUN_WPD_S)
#define IO_MUX_GPIO13_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPD_S  7
/** IO_MUX_GPIO13_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO13_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO13_FUN_WPU_M  (IO_MUX_GPIO13_FUN_WPU_V << IO_MUX_GPIO13_FUN_WPU_S)
#define IO_MUX_GPIO13_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPU_S  8
/** IO_MUX_GPIO13_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO13_FUN_IE    (BIT(9))
#define IO_MUX_GPIO13_FUN_IE_M  (IO_MUX_GPIO13_FUN_IE_V << IO_MUX_GPIO13_FUN_IE_S)
#define IO_MUX_GPIO13_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO13_FUN_IE_S  9
/** IO_MUX_GPIO13_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO13_FUN_DRV    0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_M  (IO_MUX_GPIO13_FUN_DRV_V << IO_MUX_GPIO13_FUN_DRV_S)
#define IO_MUX_GPIO13_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_S  10
/** IO_MUX_GPIO13_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO13_MCU_SEL    0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_M  (IO_MUX_GPIO13_MCU_SEL_V << IO_MUX_GPIO13_MCU_SEL_S)
#define IO_MUX_GPIO13_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_S  12
/** IO_MUX_GPIO13_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO13_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO13_FILTER_EN_M  (IO_MUX_GPIO13_FILTER_EN_V << IO_MUX_GPIO13_FILTER_EN_S)
#define IO_MUX_GPIO13_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO13_FILTER_EN_S  15

/** IO_MUX_gpio14_REG register
 *  iomux control register for gpio14
 */
#define IO_MUX_GPIO14_REG (DR_REG_IO_MUX_BASE + 0x3c)
/** IO_MUX_GPIO14_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_OE    (BIT(0))
#define IO_MUX_GPIO14_MCU_OE_M  (IO_MUX_GPIO14_MCU_OE_V << IO_MUX_GPIO14_MCU_OE_S)
#define IO_MUX_GPIO14_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_OE_S  0
/** IO_MUX_GPIO14_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO14_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO14_SLP_SEL_M  (IO_MUX_GPIO14_SLP_SEL_V << IO_MUX_GPIO14_SLP_SEL_S)
#define IO_MUX_GPIO14_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO14_SLP_SEL_S  1
/** IO_MUX_GPIO14_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO14_MCU_WPD_M  (IO_MUX_GPIO14_MCU_WPD_V << IO_MUX_GPIO14_MCU_WPD_S)
#define IO_MUX_GPIO14_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPD_S  2
/** IO_MUX_GPIO14_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO14_MCU_WPU_M  (IO_MUX_GPIO14_MCU_WPU_V << IO_MUX_GPIO14_MCU_WPU_S)
#define IO_MUX_GPIO14_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPU_S  3
/** IO_MUX_GPIO14_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO14_MCU_IE    (BIT(4))
#define IO_MUX_GPIO14_MCU_IE_M  (IO_MUX_GPIO14_MCU_IE_V << IO_MUX_GPIO14_MCU_IE_S)
#define IO_MUX_GPIO14_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_IE_S  4
/** IO_MUX_GPIO14_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO14_MCU_DRV    0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_M  (IO_MUX_GPIO14_MCU_DRV_V << IO_MUX_GPIO14_MCU_DRV_S)
#define IO_MUX_GPIO14_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_S  5
/** IO_MUX_GPIO14_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO14_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO14_FUN_WPD_M  (IO_MUX_GPIO14_FUN_WPD_V << IO_MUX_GPIO14_FUN_WPD_S)
#define IO_MUX_GPIO14_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPD_S  7
/** IO_MUX_GPIO14_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO14_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO14_FUN_WPU_M  (IO_MUX_GPIO14_FUN_WPU_V << IO_MUX_GPIO14_FUN_WPU_S)
#define IO_MUX_GPIO14_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPU_S  8
/** IO_MUX_GPIO14_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO14_FUN_IE    (BIT(9))
#define IO_MUX_GPIO14_FUN_IE_M  (IO_MUX_GPIO14_FUN_IE_V << IO_MUX_GPIO14_FUN_IE_S)
#define IO_MUX_GPIO14_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO14_FUN_IE_S  9
/** IO_MUX_GPIO14_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO14_FUN_DRV    0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_M  (IO_MUX_GPIO14_FUN_DRV_V << IO_MUX_GPIO14_FUN_DRV_S)
#define IO_MUX_GPIO14_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_S  10
/** IO_MUX_GPIO14_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO14_MCU_SEL    0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_M  (IO_MUX_GPIO14_MCU_SEL_V << IO_MUX_GPIO14_MCU_SEL_S)
#define IO_MUX_GPIO14_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_S  12
/** IO_MUX_GPIO14_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO14_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO14_FILTER_EN_M  (IO_MUX_GPIO14_FILTER_EN_V << IO_MUX_GPIO14_FILTER_EN_S)
#define IO_MUX_GPIO14_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO14_FILTER_EN_S  15

/** IO_MUX_gpio15_REG register
 *  iomux control register for gpio15
 */
#define IO_MUX_GPIO15_REG (DR_REG_IO_MUX_BASE + 0x40)
/** IO_MUX_GPIO15_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_OE    (BIT(0))
#define IO_MUX_GPIO15_MCU_OE_M  (IO_MUX_GPIO15_MCU_OE_V << IO_MUX_GPIO15_MCU_OE_S)
#define IO_MUX_GPIO15_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_OE_S  0
/** IO_MUX_GPIO15_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO15_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO15_SLP_SEL_M  (IO_MUX_GPIO15_SLP_SEL_V << IO_MUX_GPIO15_SLP_SEL_S)
#define IO_MUX_GPIO15_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO15_SLP_SEL_S  1
/** IO_MUX_GPIO15_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO15_MCU_WPD_M  (IO_MUX_GPIO15_MCU_WPD_V << IO_MUX_GPIO15_MCU_WPD_S)
#define IO_MUX_GPIO15_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPD_S  2
/** IO_MUX_GPIO15_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO15_MCU_WPU_M  (IO_MUX_GPIO15_MCU_WPU_V << IO_MUX_GPIO15_MCU_WPU_S)
#define IO_MUX_GPIO15_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPU_S  3
/** IO_MUX_GPIO15_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO15_MCU_IE    (BIT(4))
#define IO_MUX_GPIO15_MCU_IE_M  (IO_MUX_GPIO15_MCU_IE_V << IO_MUX_GPIO15_MCU_IE_S)
#define IO_MUX_GPIO15_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_IE_S  4
/** IO_MUX_GPIO15_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO15_MCU_DRV    0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_M  (IO_MUX_GPIO15_MCU_DRV_V << IO_MUX_GPIO15_MCU_DRV_S)
#define IO_MUX_GPIO15_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_S  5
/** IO_MUX_GPIO15_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO15_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO15_FUN_WPD_M  (IO_MUX_GPIO15_FUN_WPD_V << IO_MUX_GPIO15_FUN_WPD_S)
#define IO_MUX_GPIO15_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPD_S  7
/** IO_MUX_GPIO15_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO15_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO15_FUN_WPU_M  (IO_MUX_GPIO15_FUN_WPU_V << IO_MUX_GPIO15_FUN_WPU_S)
#define IO_MUX_GPIO15_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPU_S  8
/** IO_MUX_GPIO15_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO15_FUN_IE    (BIT(9))
#define IO_MUX_GPIO15_FUN_IE_M  (IO_MUX_GPIO15_FUN_IE_V << IO_MUX_GPIO15_FUN_IE_S)
#define IO_MUX_GPIO15_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO15_FUN_IE_S  9
/** IO_MUX_GPIO15_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO15_FUN_DRV    0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_M  (IO_MUX_GPIO15_FUN_DRV_V << IO_MUX_GPIO15_FUN_DRV_S)
#define IO_MUX_GPIO15_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_S  10
/** IO_MUX_GPIO15_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO15_MCU_SEL    0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_M  (IO_MUX_GPIO15_MCU_SEL_V << IO_MUX_GPIO15_MCU_SEL_S)
#define IO_MUX_GPIO15_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_S  12
/** IO_MUX_GPIO15_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO15_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO15_FILTER_EN_M  (IO_MUX_GPIO15_FILTER_EN_V << IO_MUX_GPIO15_FILTER_EN_S)
#define IO_MUX_GPIO15_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO15_FILTER_EN_S  15

/** IO_MUX_gpio16_REG register
 *  iomux control register for gpio16
 */
#define IO_MUX_GPIO16_REG (DR_REG_IO_MUX_BASE + 0x44)
/** IO_MUX_GPIO16_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_OE    (BIT(0))
#define IO_MUX_GPIO16_MCU_OE_M  (IO_MUX_GPIO16_MCU_OE_V << IO_MUX_GPIO16_MCU_OE_S)
#define IO_MUX_GPIO16_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_OE_S  0
/** IO_MUX_GPIO16_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO16_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO16_SLP_SEL_M  (IO_MUX_GPIO16_SLP_SEL_V << IO_MUX_GPIO16_SLP_SEL_S)
#define IO_MUX_GPIO16_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO16_SLP_SEL_S  1
/** IO_MUX_GPIO16_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO16_MCU_WPD_M  (IO_MUX_GPIO16_MCU_WPD_V << IO_MUX_GPIO16_MCU_WPD_S)
#define IO_MUX_GPIO16_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPD_S  2
/** IO_MUX_GPIO16_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO16_MCU_WPU_M  (IO_MUX_GPIO16_MCU_WPU_V << IO_MUX_GPIO16_MCU_WPU_S)
#define IO_MUX_GPIO16_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPU_S  3
/** IO_MUX_GPIO16_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO16_MCU_IE    (BIT(4))
#define IO_MUX_GPIO16_MCU_IE_M  (IO_MUX_GPIO16_MCU_IE_V << IO_MUX_GPIO16_MCU_IE_S)
#define IO_MUX_GPIO16_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_IE_S  4
/** IO_MUX_GPIO16_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO16_MCU_DRV    0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_M  (IO_MUX_GPIO16_MCU_DRV_V << IO_MUX_GPIO16_MCU_DRV_S)
#define IO_MUX_GPIO16_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_S  5
/** IO_MUX_GPIO16_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO16_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO16_FUN_WPD_M  (IO_MUX_GPIO16_FUN_WPD_V << IO_MUX_GPIO16_FUN_WPD_S)
#define IO_MUX_GPIO16_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPD_S  7
/** IO_MUX_GPIO16_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO16_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO16_FUN_WPU_M  (IO_MUX_GPIO16_FUN_WPU_V << IO_MUX_GPIO16_FUN_WPU_S)
#define IO_MUX_GPIO16_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPU_S  8
/** IO_MUX_GPIO16_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO16_FUN_IE    (BIT(9))
#define IO_MUX_GPIO16_FUN_IE_M  (IO_MUX_GPIO16_FUN_IE_V << IO_MUX_GPIO16_FUN_IE_S)
#define IO_MUX_GPIO16_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO16_FUN_IE_S  9
/** IO_MUX_GPIO16_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO16_FUN_DRV    0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_M  (IO_MUX_GPIO16_FUN_DRV_V << IO_MUX_GPIO16_FUN_DRV_S)
#define IO_MUX_GPIO16_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_S  10
/** IO_MUX_GPIO16_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO16_MCU_SEL    0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_M  (IO_MUX_GPIO16_MCU_SEL_V << IO_MUX_GPIO16_MCU_SEL_S)
#define IO_MUX_GPIO16_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_S  12
/** IO_MUX_GPIO16_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO16_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO16_FILTER_EN_M  (IO_MUX_GPIO16_FILTER_EN_V << IO_MUX_GPIO16_FILTER_EN_S)
#define IO_MUX_GPIO16_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO16_FILTER_EN_S  15

/** IO_MUX_gpio17_REG register
 *  iomux control register for gpio17
 */
#define IO_MUX_GPIO17_REG (DR_REG_IO_MUX_BASE + 0x48)
/** IO_MUX_GPIO17_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_OE    (BIT(0))
#define IO_MUX_GPIO17_MCU_OE_M  (IO_MUX_GPIO17_MCU_OE_V << IO_MUX_GPIO17_MCU_OE_S)
#define IO_MUX_GPIO17_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_OE_S  0
/** IO_MUX_GPIO17_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO17_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO17_SLP_SEL_M  (IO_MUX_GPIO17_SLP_SEL_V << IO_MUX_GPIO17_SLP_SEL_S)
#define IO_MUX_GPIO17_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO17_SLP_SEL_S  1
/** IO_MUX_GPIO17_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO17_MCU_WPD_M  (IO_MUX_GPIO17_MCU_WPD_V << IO_MUX_GPIO17_MCU_WPD_S)
#define IO_MUX_GPIO17_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPD_S  2
/** IO_MUX_GPIO17_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO17_MCU_WPU_M  (IO_MUX_GPIO17_MCU_WPU_V << IO_MUX_GPIO17_MCU_WPU_S)
#define IO_MUX_GPIO17_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPU_S  3
/** IO_MUX_GPIO17_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO17_MCU_IE    (BIT(4))
#define IO_MUX_GPIO17_MCU_IE_M  (IO_MUX_GPIO17_MCU_IE_V << IO_MUX_GPIO17_MCU_IE_S)
#define IO_MUX_GPIO17_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_IE_S  4
/** IO_MUX_GPIO17_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO17_MCU_DRV    0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_M  (IO_MUX_GPIO17_MCU_DRV_V << IO_MUX_GPIO17_MCU_DRV_S)
#define IO_MUX_GPIO17_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_S  5
/** IO_MUX_GPIO17_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO17_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO17_FUN_WPD_M  (IO_MUX_GPIO17_FUN_WPD_V << IO_MUX_GPIO17_FUN_WPD_S)
#define IO_MUX_GPIO17_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPD_S  7
/** IO_MUX_GPIO17_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO17_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO17_FUN_WPU_M  (IO_MUX_GPIO17_FUN_WPU_V << IO_MUX_GPIO17_FUN_WPU_S)
#define IO_MUX_GPIO17_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPU_S  8
/** IO_MUX_GPIO17_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO17_FUN_IE    (BIT(9))
#define IO_MUX_GPIO17_FUN_IE_M  (IO_MUX_GPIO17_FUN_IE_V << IO_MUX_GPIO17_FUN_IE_S)
#define IO_MUX_GPIO17_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO17_FUN_IE_S  9
/** IO_MUX_GPIO17_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO17_FUN_DRV    0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_M  (IO_MUX_GPIO17_FUN_DRV_V << IO_MUX_GPIO17_FUN_DRV_S)
#define IO_MUX_GPIO17_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_S  10
/** IO_MUX_GPIO17_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO17_MCU_SEL    0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_M  (IO_MUX_GPIO17_MCU_SEL_V << IO_MUX_GPIO17_MCU_SEL_S)
#define IO_MUX_GPIO17_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_S  12
/** IO_MUX_GPIO17_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO17_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO17_FILTER_EN_M  (IO_MUX_GPIO17_FILTER_EN_V << IO_MUX_GPIO17_FILTER_EN_S)
#define IO_MUX_GPIO17_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO17_FILTER_EN_S  15

/** IO_MUX_gpio18_REG register
 *  iomux control register for gpio18
 */
#define IO_MUX_GPIO18_REG (DR_REG_IO_MUX_BASE + 0x4c)
/** IO_MUX_GPIO18_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_OE    (BIT(0))
#define IO_MUX_GPIO18_MCU_OE_M  (IO_MUX_GPIO18_MCU_OE_V << IO_MUX_GPIO18_MCU_OE_S)
#define IO_MUX_GPIO18_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_OE_S  0
/** IO_MUX_GPIO18_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO18_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO18_SLP_SEL_M  (IO_MUX_GPIO18_SLP_SEL_V << IO_MUX_GPIO18_SLP_SEL_S)
#define IO_MUX_GPIO18_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO18_SLP_SEL_S  1
/** IO_MUX_GPIO18_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO18_MCU_WPD_M  (IO_MUX_GPIO18_MCU_WPD_V << IO_MUX_GPIO18_MCU_WPD_S)
#define IO_MUX_GPIO18_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPD_S  2
/** IO_MUX_GPIO18_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO18_MCU_WPU_M  (IO_MUX_GPIO18_MCU_WPU_V << IO_MUX_GPIO18_MCU_WPU_S)
#define IO_MUX_GPIO18_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPU_S  3
/** IO_MUX_GPIO18_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO18_MCU_IE    (BIT(4))
#define IO_MUX_GPIO18_MCU_IE_M  (IO_MUX_GPIO18_MCU_IE_V << IO_MUX_GPIO18_MCU_IE_S)
#define IO_MUX_GPIO18_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_IE_S  4
/** IO_MUX_GPIO18_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO18_MCU_DRV    0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_M  (IO_MUX_GPIO18_MCU_DRV_V << IO_MUX_GPIO18_MCU_DRV_S)
#define IO_MUX_GPIO18_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_S  5
/** IO_MUX_GPIO18_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO18_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO18_FUN_WPD_M  (IO_MUX_GPIO18_FUN_WPD_V << IO_MUX_GPIO18_FUN_WPD_S)
#define IO_MUX_GPIO18_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPD_S  7
/** IO_MUX_GPIO18_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO18_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO18_FUN_WPU_M  (IO_MUX_GPIO18_FUN_WPU_V << IO_MUX_GPIO18_FUN_WPU_S)
#define IO_MUX_GPIO18_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPU_S  8
/** IO_MUX_GPIO18_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO18_FUN_IE    (BIT(9))
#define IO_MUX_GPIO18_FUN_IE_M  (IO_MUX_GPIO18_FUN_IE_V << IO_MUX_GPIO18_FUN_IE_S)
#define IO_MUX_GPIO18_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO18_FUN_IE_S  9
/** IO_MUX_GPIO18_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO18_FUN_DRV    0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_M  (IO_MUX_GPIO18_FUN_DRV_V << IO_MUX_GPIO18_FUN_DRV_S)
#define IO_MUX_GPIO18_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_S  10
/** IO_MUX_GPIO18_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO18_MCU_SEL    0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_M  (IO_MUX_GPIO18_MCU_SEL_V << IO_MUX_GPIO18_MCU_SEL_S)
#define IO_MUX_GPIO18_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_S  12
/** IO_MUX_GPIO18_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO18_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO18_FILTER_EN_M  (IO_MUX_GPIO18_FILTER_EN_V << IO_MUX_GPIO18_FILTER_EN_S)
#define IO_MUX_GPIO18_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO18_FILTER_EN_S  15

/** IO_MUX_gpio19_REG register
 *  iomux control register for gpio19
 */
#define IO_MUX_GPIO19_REG (DR_REG_IO_MUX_BASE + 0x50)
/** IO_MUX_GPIO19_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_OE    (BIT(0))
#define IO_MUX_GPIO19_MCU_OE_M  (IO_MUX_GPIO19_MCU_OE_V << IO_MUX_GPIO19_MCU_OE_S)
#define IO_MUX_GPIO19_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_OE_S  0
/** IO_MUX_GPIO19_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO19_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO19_SLP_SEL_M  (IO_MUX_GPIO19_SLP_SEL_V << IO_MUX_GPIO19_SLP_SEL_S)
#define IO_MUX_GPIO19_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO19_SLP_SEL_S  1
/** IO_MUX_GPIO19_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO19_MCU_WPD_M  (IO_MUX_GPIO19_MCU_WPD_V << IO_MUX_GPIO19_MCU_WPD_S)
#define IO_MUX_GPIO19_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPD_S  2
/** IO_MUX_GPIO19_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO19_MCU_WPU_M  (IO_MUX_GPIO19_MCU_WPU_V << IO_MUX_GPIO19_MCU_WPU_S)
#define IO_MUX_GPIO19_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPU_S  3
/** IO_MUX_GPIO19_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO19_MCU_IE    (BIT(4))
#define IO_MUX_GPIO19_MCU_IE_M  (IO_MUX_GPIO19_MCU_IE_V << IO_MUX_GPIO19_MCU_IE_S)
#define IO_MUX_GPIO19_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_IE_S  4
/** IO_MUX_GPIO19_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO19_MCU_DRV    0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_M  (IO_MUX_GPIO19_MCU_DRV_V << IO_MUX_GPIO19_MCU_DRV_S)
#define IO_MUX_GPIO19_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_S  5
/** IO_MUX_GPIO19_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO19_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO19_FUN_WPD_M  (IO_MUX_GPIO19_FUN_WPD_V << IO_MUX_GPIO19_FUN_WPD_S)
#define IO_MUX_GPIO19_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPD_S  7
/** IO_MUX_GPIO19_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO19_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO19_FUN_WPU_M  (IO_MUX_GPIO19_FUN_WPU_V << IO_MUX_GPIO19_FUN_WPU_S)
#define IO_MUX_GPIO19_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPU_S  8
/** IO_MUX_GPIO19_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO19_FUN_IE    (BIT(9))
#define IO_MUX_GPIO19_FUN_IE_M  (IO_MUX_GPIO19_FUN_IE_V << IO_MUX_GPIO19_FUN_IE_S)
#define IO_MUX_GPIO19_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO19_FUN_IE_S  9
/** IO_MUX_GPIO19_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO19_FUN_DRV    0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_M  (IO_MUX_GPIO19_FUN_DRV_V << IO_MUX_GPIO19_FUN_DRV_S)
#define IO_MUX_GPIO19_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_S  10
/** IO_MUX_GPIO19_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO19_MCU_SEL    0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_M  (IO_MUX_GPIO19_MCU_SEL_V << IO_MUX_GPIO19_MCU_SEL_S)
#define IO_MUX_GPIO19_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_S  12
/** IO_MUX_GPIO19_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO19_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO19_FILTER_EN_M  (IO_MUX_GPIO19_FILTER_EN_V << IO_MUX_GPIO19_FILTER_EN_S)
#define IO_MUX_GPIO19_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO19_FILTER_EN_S  15

/** IO_MUX_gpio20_REG register
 *  iomux control register for gpio20
 */
#define IO_MUX_GPIO20_REG (DR_REG_IO_MUX_BASE + 0x54)
/** IO_MUX_GPIO20_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_OE    (BIT(0))
#define IO_MUX_GPIO20_MCU_OE_M  (IO_MUX_GPIO20_MCU_OE_V << IO_MUX_GPIO20_MCU_OE_S)
#define IO_MUX_GPIO20_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_OE_S  0
/** IO_MUX_GPIO20_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO20_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO20_SLP_SEL_M  (IO_MUX_GPIO20_SLP_SEL_V << IO_MUX_GPIO20_SLP_SEL_S)
#define IO_MUX_GPIO20_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO20_SLP_SEL_S  1
/** IO_MUX_GPIO20_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO20_MCU_WPD_M  (IO_MUX_GPIO20_MCU_WPD_V << IO_MUX_GPIO20_MCU_WPD_S)
#define IO_MUX_GPIO20_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPD_S  2
/** IO_MUX_GPIO20_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO20_MCU_WPU_M  (IO_MUX_GPIO20_MCU_WPU_V << IO_MUX_GPIO20_MCU_WPU_S)
#define IO_MUX_GPIO20_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPU_S  3
/** IO_MUX_GPIO20_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO20_MCU_IE    (BIT(4))
#define IO_MUX_GPIO20_MCU_IE_M  (IO_MUX_GPIO20_MCU_IE_V << IO_MUX_GPIO20_MCU_IE_S)
#define IO_MUX_GPIO20_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_IE_S  4
/** IO_MUX_GPIO20_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO20_MCU_DRV    0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_M  (IO_MUX_GPIO20_MCU_DRV_V << IO_MUX_GPIO20_MCU_DRV_S)
#define IO_MUX_GPIO20_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_S  5
/** IO_MUX_GPIO20_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO20_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO20_FUN_WPD_M  (IO_MUX_GPIO20_FUN_WPD_V << IO_MUX_GPIO20_FUN_WPD_S)
#define IO_MUX_GPIO20_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPD_S  7
/** IO_MUX_GPIO20_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO20_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO20_FUN_WPU_M  (IO_MUX_GPIO20_FUN_WPU_V << IO_MUX_GPIO20_FUN_WPU_S)
#define IO_MUX_GPIO20_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPU_S  8
/** IO_MUX_GPIO20_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO20_FUN_IE    (BIT(9))
#define IO_MUX_GPIO20_FUN_IE_M  (IO_MUX_GPIO20_FUN_IE_V << IO_MUX_GPIO20_FUN_IE_S)
#define IO_MUX_GPIO20_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO20_FUN_IE_S  9
/** IO_MUX_GPIO20_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO20_FUN_DRV    0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_M  (IO_MUX_GPIO20_FUN_DRV_V << IO_MUX_GPIO20_FUN_DRV_S)
#define IO_MUX_GPIO20_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_S  10
/** IO_MUX_GPIO20_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO20_MCU_SEL    0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_M  (IO_MUX_GPIO20_MCU_SEL_V << IO_MUX_GPIO20_MCU_SEL_S)
#define IO_MUX_GPIO20_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_S  12
/** IO_MUX_GPIO20_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO20_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO20_FILTER_EN_M  (IO_MUX_GPIO20_FILTER_EN_V << IO_MUX_GPIO20_FILTER_EN_S)
#define IO_MUX_GPIO20_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO20_FILTER_EN_S  15

/** IO_MUX_gpio21_REG register
 *  iomux control register for gpio21
 */
#define IO_MUX_GPIO21_REG (DR_REG_IO_MUX_BASE + 0x58)
/** IO_MUX_GPIO21_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_OE    (BIT(0))
#define IO_MUX_GPIO21_MCU_OE_M  (IO_MUX_GPIO21_MCU_OE_V << IO_MUX_GPIO21_MCU_OE_S)
#define IO_MUX_GPIO21_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_OE_S  0
/** IO_MUX_GPIO21_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO21_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO21_SLP_SEL_M  (IO_MUX_GPIO21_SLP_SEL_V << IO_MUX_GPIO21_SLP_SEL_S)
#define IO_MUX_GPIO21_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO21_SLP_SEL_S  1
/** IO_MUX_GPIO21_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO21_MCU_WPD_M  (IO_MUX_GPIO21_MCU_WPD_V << IO_MUX_GPIO21_MCU_WPD_S)
#define IO_MUX_GPIO21_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPD_S  2
/** IO_MUX_GPIO21_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO21_MCU_WPU_M  (IO_MUX_GPIO21_MCU_WPU_V << IO_MUX_GPIO21_MCU_WPU_S)
#define IO_MUX_GPIO21_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPU_S  3
/** IO_MUX_GPIO21_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO21_MCU_IE    (BIT(4))
#define IO_MUX_GPIO21_MCU_IE_M  (IO_MUX_GPIO21_MCU_IE_V << IO_MUX_GPIO21_MCU_IE_S)
#define IO_MUX_GPIO21_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_IE_S  4
/** IO_MUX_GPIO21_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO21_MCU_DRV    0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_M  (IO_MUX_GPIO21_MCU_DRV_V << IO_MUX_GPIO21_MCU_DRV_S)
#define IO_MUX_GPIO21_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_S  5
/** IO_MUX_GPIO21_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO21_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO21_FUN_WPD_M  (IO_MUX_GPIO21_FUN_WPD_V << IO_MUX_GPIO21_FUN_WPD_S)
#define IO_MUX_GPIO21_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPD_S  7
/** IO_MUX_GPIO21_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO21_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO21_FUN_WPU_M  (IO_MUX_GPIO21_FUN_WPU_V << IO_MUX_GPIO21_FUN_WPU_S)
#define IO_MUX_GPIO21_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPU_S  8
/** IO_MUX_GPIO21_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO21_FUN_IE    (BIT(9))
#define IO_MUX_GPIO21_FUN_IE_M  (IO_MUX_GPIO21_FUN_IE_V << IO_MUX_GPIO21_FUN_IE_S)
#define IO_MUX_GPIO21_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO21_FUN_IE_S  9
/** IO_MUX_GPIO21_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO21_FUN_DRV    0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_M  (IO_MUX_GPIO21_FUN_DRV_V << IO_MUX_GPIO21_FUN_DRV_S)
#define IO_MUX_GPIO21_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_S  10
/** IO_MUX_GPIO21_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO21_MCU_SEL    0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_M  (IO_MUX_GPIO21_MCU_SEL_V << IO_MUX_GPIO21_MCU_SEL_S)
#define IO_MUX_GPIO21_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_S  12
/** IO_MUX_GPIO21_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO21_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO21_FILTER_EN_M  (IO_MUX_GPIO21_FILTER_EN_V << IO_MUX_GPIO21_FILTER_EN_S)
#define IO_MUX_GPIO21_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO21_FILTER_EN_S  15

/** IO_MUX_gpio22_REG register
 *  iomux control register for gpio22
 */
#define IO_MUX_GPIO22_REG (DR_REG_IO_MUX_BASE + 0x5c)
/** IO_MUX_GPIO22_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_OE    (BIT(0))
#define IO_MUX_GPIO22_MCU_OE_M  (IO_MUX_GPIO22_MCU_OE_V << IO_MUX_GPIO22_MCU_OE_S)
#define IO_MUX_GPIO22_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_OE_S  0
/** IO_MUX_GPIO22_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO22_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO22_SLP_SEL_M  (IO_MUX_GPIO22_SLP_SEL_V << IO_MUX_GPIO22_SLP_SEL_S)
#define IO_MUX_GPIO22_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO22_SLP_SEL_S  1
/** IO_MUX_GPIO22_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO22_MCU_WPD_M  (IO_MUX_GPIO22_MCU_WPD_V << IO_MUX_GPIO22_MCU_WPD_S)
#define IO_MUX_GPIO22_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPD_S  2
/** IO_MUX_GPIO22_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO22_MCU_WPU_M  (IO_MUX_GPIO22_MCU_WPU_V << IO_MUX_GPIO22_MCU_WPU_S)
#define IO_MUX_GPIO22_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPU_S  3
/** IO_MUX_GPIO22_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO22_MCU_IE    (BIT(4))
#define IO_MUX_GPIO22_MCU_IE_M  (IO_MUX_GPIO22_MCU_IE_V << IO_MUX_GPIO22_MCU_IE_S)
#define IO_MUX_GPIO22_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_IE_S  4
/** IO_MUX_GPIO22_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO22_MCU_DRV    0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_M  (IO_MUX_GPIO22_MCU_DRV_V << IO_MUX_GPIO22_MCU_DRV_S)
#define IO_MUX_GPIO22_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_S  5
/** IO_MUX_GPIO22_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO22_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO22_FUN_WPD_M  (IO_MUX_GPIO22_FUN_WPD_V << IO_MUX_GPIO22_FUN_WPD_S)
#define IO_MUX_GPIO22_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPD_S  7
/** IO_MUX_GPIO22_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO22_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO22_FUN_WPU_M  (IO_MUX_GPIO22_FUN_WPU_V << IO_MUX_GPIO22_FUN_WPU_S)
#define IO_MUX_GPIO22_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPU_S  8
/** IO_MUX_GPIO22_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO22_FUN_IE    (BIT(9))
#define IO_MUX_GPIO22_FUN_IE_M  (IO_MUX_GPIO22_FUN_IE_V << IO_MUX_GPIO22_FUN_IE_S)
#define IO_MUX_GPIO22_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO22_FUN_IE_S  9
/** IO_MUX_GPIO22_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO22_FUN_DRV    0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_M  (IO_MUX_GPIO22_FUN_DRV_V << IO_MUX_GPIO22_FUN_DRV_S)
#define IO_MUX_GPIO22_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_S  10
/** IO_MUX_GPIO22_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO22_MCU_SEL    0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_M  (IO_MUX_GPIO22_MCU_SEL_V << IO_MUX_GPIO22_MCU_SEL_S)
#define IO_MUX_GPIO22_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_S  12
/** IO_MUX_GPIO22_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO22_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO22_FILTER_EN_M  (IO_MUX_GPIO22_FILTER_EN_V << IO_MUX_GPIO22_FILTER_EN_S)
#define IO_MUX_GPIO22_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO22_FILTER_EN_S  15

/** IO_MUX_gpio23_REG register
 *  iomux control register for gpio23
 */
#define IO_MUX_GPIO23_REG (DR_REG_IO_MUX_BASE + 0x60)
/** IO_MUX_GPIO23_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_OE    (BIT(0))
#define IO_MUX_GPIO23_MCU_OE_M  (IO_MUX_GPIO23_MCU_OE_V << IO_MUX_GPIO23_MCU_OE_S)
#define IO_MUX_GPIO23_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_OE_S  0
/** IO_MUX_GPIO23_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO23_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO23_SLP_SEL_M  (IO_MUX_GPIO23_SLP_SEL_V << IO_MUX_GPIO23_SLP_SEL_S)
#define IO_MUX_GPIO23_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO23_SLP_SEL_S  1
/** IO_MUX_GPIO23_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO23_MCU_WPD_M  (IO_MUX_GPIO23_MCU_WPD_V << IO_MUX_GPIO23_MCU_WPD_S)
#define IO_MUX_GPIO23_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPD_S  2
/** IO_MUX_GPIO23_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO23_MCU_WPU_M  (IO_MUX_GPIO23_MCU_WPU_V << IO_MUX_GPIO23_MCU_WPU_S)
#define IO_MUX_GPIO23_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPU_S  3
/** IO_MUX_GPIO23_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO23_MCU_IE    (BIT(4))
#define IO_MUX_GPIO23_MCU_IE_M  (IO_MUX_GPIO23_MCU_IE_V << IO_MUX_GPIO23_MCU_IE_S)
#define IO_MUX_GPIO23_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_IE_S  4
/** IO_MUX_GPIO23_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO23_MCU_DRV    0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_M  (IO_MUX_GPIO23_MCU_DRV_V << IO_MUX_GPIO23_MCU_DRV_S)
#define IO_MUX_GPIO23_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_S  5
/** IO_MUX_GPIO23_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO23_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO23_FUN_WPD_M  (IO_MUX_GPIO23_FUN_WPD_V << IO_MUX_GPIO23_FUN_WPD_S)
#define IO_MUX_GPIO23_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPD_S  7
/** IO_MUX_GPIO23_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO23_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO23_FUN_WPU_M  (IO_MUX_GPIO23_FUN_WPU_V << IO_MUX_GPIO23_FUN_WPU_S)
#define IO_MUX_GPIO23_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPU_S  8
/** IO_MUX_GPIO23_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO23_FUN_IE    (BIT(9))
#define IO_MUX_GPIO23_FUN_IE_M  (IO_MUX_GPIO23_FUN_IE_V << IO_MUX_GPIO23_FUN_IE_S)
#define IO_MUX_GPIO23_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO23_FUN_IE_S  9
/** IO_MUX_GPIO23_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO23_FUN_DRV    0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_M  (IO_MUX_GPIO23_FUN_DRV_V << IO_MUX_GPIO23_FUN_DRV_S)
#define IO_MUX_GPIO23_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_S  10
/** IO_MUX_GPIO23_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO23_MCU_SEL    0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_M  (IO_MUX_GPIO23_MCU_SEL_V << IO_MUX_GPIO23_MCU_SEL_S)
#define IO_MUX_GPIO23_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_S  12
/** IO_MUX_GPIO23_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO23_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO23_FILTER_EN_M  (IO_MUX_GPIO23_FILTER_EN_V << IO_MUX_GPIO23_FILTER_EN_S)
#define IO_MUX_GPIO23_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO23_FILTER_EN_S  15

/** IO_MUX_gpio24_REG register
 *  iomux control register for gpio24
 */
#define IO_MUX_GPIO24_REG (DR_REG_IO_MUX_BASE + 0x64)
/** IO_MUX_GPIO24_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_OE    (BIT(0))
#define IO_MUX_GPIO24_MCU_OE_M  (IO_MUX_GPIO24_MCU_OE_V << IO_MUX_GPIO24_MCU_OE_S)
#define IO_MUX_GPIO24_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_OE_S  0
/** IO_MUX_GPIO24_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO24_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO24_SLP_SEL_M  (IO_MUX_GPIO24_SLP_SEL_V << IO_MUX_GPIO24_SLP_SEL_S)
#define IO_MUX_GPIO24_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO24_SLP_SEL_S  1
/** IO_MUX_GPIO24_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO24_MCU_WPD_M  (IO_MUX_GPIO24_MCU_WPD_V << IO_MUX_GPIO24_MCU_WPD_S)
#define IO_MUX_GPIO24_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPD_S  2
/** IO_MUX_GPIO24_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO24_MCU_WPU_M  (IO_MUX_GPIO24_MCU_WPU_V << IO_MUX_GPIO24_MCU_WPU_S)
#define IO_MUX_GPIO24_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPU_S  3
/** IO_MUX_GPIO24_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO24_MCU_IE    (BIT(4))
#define IO_MUX_GPIO24_MCU_IE_M  (IO_MUX_GPIO24_MCU_IE_V << IO_MUX_GPIO24_MCU_IE_S)
#define IO_MUX_GPIO24_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_IE_S  4
/** IO_MUX_GPIO24_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO24_MCU_DRV    0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_M  (IO_MUX_GPIO24_MCU_DRV_V << IO_MUX_GPIO24_MCU_DRV_S)
#define IO_MUX_GPIO24_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_S  5
/** IO_MUX_GPIO24_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO24_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO24_FUN_WPD_M  (IO_MUX_GPIO24_FUN_WPD_V << IO_MUX_GPIO24_FUN_WPD_S)
#define IO_MUX_GPIO24_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPD_S  7
/** IO_MUX_GPIO24_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO24_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO24_FUN_WPU_M  (IO_MUX_GPIO24_FUN_WPU_V << IO_MUX_GPIO24_FUN_WPU_S)
#define IO_MUX_GPIO24_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPU_S  8
/** IO_MUX_GPIO24_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO24_FUN_IE    (BIT(9))
#define IO_MUX_GPIO24_FUN_IE_M  (IO_MUX_GPIO24_FUN_IE_V << IO_MUX_GPIO24_FUN_IE_S)
#define IO_MUX_GPIO24_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO24_FUN_IE_S  9
/** IO_MUX_GPIO24_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO24_FUN_DRV    0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_M  (IO_MUX_GPIO24_FUN_DRV_V << IO_MUX_GPIO24_FUN_DRV_S)
#define IO_MUX_GPIO24_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_S  10
/** IO_MUX_GPIO24_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO24_MCU_SEL    0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_M  (IO_MUX_GPIO24_MCU_SEL_V << IO_MUX_GPIO24_MCU_SEL_S)
#define IO_MUX_GPIO24_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_S  12
/** IO_MUX_GPIO24_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO24_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO24_FILTER_EN_M  (IO_MUX_GPIO24_FILTER_EN_V << IO_MUX_GPIO24_FILTER_EN_S)
#define IO_MUX_GPIO24_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO24_FILTER_EN_S  15

/** IO_MUX_gpio25_REG register
 *  iomux control register for gpio25
 */
#define IO_MUX_GPIO25_REG (DR_REG_IO_MUX_BASE + 0x68)
/** IO_MUX_GPIO25_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_OE    (BIT(0))
#define IO_MUX_GPIO25_MCU_OE_M  (IO_MUX_GPIO25_MCU_OE_V << IO_MUX_GPIO25_MCU_OE_S)
#define IO_MUX_GPIO25_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_OE_S  0
/** IO_MUX_GPIO25_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO25_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO25_SLP_SEL_M  (IO_MUX_GPIO25_SLP_SEL_V << IO_MUX_GPIO25_SLP_SEL_S)
#define IO_MUX_GPIO25_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO25_SLP_SEL_S  1
/** IO_MUX_GPIO25_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO25_MCU_WPD_M  (IO_MUX_GPIO25_MCU_WPD_V << IO_MUX_GPIO25_MCU_WPD_S)
#define IO_MUX_GPIO25_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPD_S  2
/** IO_MUX_GPIO25_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO25_MCU_WPU_M  (IO_MUX_GPIO25_MCU_WPU_V << IO_MUX_GPIO25_MCU_WPU_S)
#define IO_MUX_GPIO25_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPU_S  3
/** IO_MUX_GPIO25_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO25_MCU_IE    (BIT(4))
#define IO_MUX_GPIO25_MCU_IE_M  (IO_MUX_GPIO25_MCU_IE_V << IO_MUX_GPIO25_MCU_IE_S)
#define IO_MUX_GPIO25_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_IE_S  4
/** IO_MUX_GPIO25_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO25_MCU_DRV    0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_M  (IO_MUX_GPIO25_MCU_DRV_V << IO_MUX_GPIO25_MCU_DRV_S)
#define IO_MUX_GPIO25_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_S  5
/** IO_MUX_GPIO25_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO25_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO25_FUN_WPD_M  (IO_MUX_GPIO25_FUN_WPD_V << IO_MUX_GPIO25_FUN_WPD_S)
#define IO_MUX_GPIO25_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPD_S  7
/** IO_MUX_GPIO25_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO25_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO25_FUN_WPU_M  (IO_MUX_GPIO25_FUN_WPU_V << IO_MUX_GPIO25_FUN_WPU_S)
#define IO_MUX_GPIO25_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPU_S  8
/** IO_MUX_GPIO25_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO25_FUN_IE    (BIT(9))
#define IO_MUX_GPIO25_FUN_IE_M  (IO_MUX_GPIO25_FUN_IE_V << IO_MUX_GPIO25_FUN_IE_S)
#define IO_MUX_GPIO25_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO25_FUN_IE_S  9
/** IO_MUX_GPIO25_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO25_FUN_DRV    0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_M  (IO_MUX_GPIO25_FUN_DRV_V << IO_MUX_GPIO25_FUN_DRV_S)
#define IO_MUX_GPIO25_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_S  10
/** IO_MUX_GPIO25_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO25_MCU_SEL    0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_M  (IO_MUX_GPIO25_MCU_SEL_V << IO_MUX_GPIO25_MCU_SEL_S)
#define IO_MUX_GPIO25_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_S  12
/** IO_MUX_GPIO25_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO25_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO25_FILTER_EN_M  (IO_MUX_GPIO25_FILTER_EN_V << IO_MUX_GPIO25_FILTER_EN_S)
#define IO_MUX_GPIO25_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO25_FILTER_EN_S  15

/** IO_MUX_gpio26_REG register
 *  iomux control register for gpio26
 */
#define IO_MUX_GPIO26_REG (DR_REG_IO_MUX_BASE + 0x6c)
/** IO_MUX_GPIO26_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_OE    (BIT(0))
#define IO_MUX_GPIO26_MCU_OE_M  (IO_MUX_GPIO26_MCU_OE_V << IO_MUX_GPIO26_MCU_OE_S)
#define IO_MUX_GPIO26_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_OE_S  0
/** IO_MUX_GPIO26_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO26_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO26_SLP_SEL_M  (IO_MUX_GPIO26_SLP_SEL_V << IO_MUX_GPIO26_SLP_SEL_S)
#define IO_MUX_GPIO26_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO26_SLP_SEL_S  1
/** IO_MUX_GPIO26_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO26_MCU_WPD_M  (IO_MUX_GPIO26_MCU_WPD_V << IO_MUX_GPIO26_MCU_WPD_S)
#define IO_MUX_GPIO26_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPD_S  2
/** IO_MUX_GPIO26_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO26_MCU_WPU_M  (IO_MUX_GPIO26_MCU_WPU_V << IO_MUX_GPIO26_MCU_WPU_S)
#define IO_MUX_GPIO26_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPU_S  3
/** IO_MUX_GPIO26_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO26_MCU_IE    (BIT(4))
#define IO_MUX_GPIO26_MCU_IE_M  (IO_MUX_GPIO26_MCU_IE_V << IO_MUX_GPIO26_MCU_IE_S)
#define IO_MUX_GPIO26_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_IE_S  4
/** IO_MUX_GPIO26_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO26_MCU_DRV    0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_M  (IO_MUX_GPIO26_MCU_DRV_V << IO_MUX_GPIO26_MCU_DRV_S)
#define IO_MUX_GPIO26_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_S  5
/** IO_MUX_GPIO26_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO26_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO26_FUN_WPD_M  (IO_MUX_GPIO26_FUN_WPD_V << IO_MUX_GPIO26_FUN_WPD_S)
#define IO_MUX_GPIO26_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPD_S  7
/** IO_MUX_GPIO26_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO26_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO26_FUN_WPU_M  (IO_MUX_GPIO26_FUN_WPU_V << IO_MUX_GPIO26_FUN_WPU_S)
#define IO_MUX_GPIO26_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPU_S  8
/** IO_MUX_GPIO26_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO26_FUN_IE    (BIT(9))
#define IO_MUX_GPIO26_FUN_IE_M  (IO_MUX_GPIO26_FUN_IE_V << IO_MUX_GPIO26_FUN_IE_S)
#define IO_MUX_GPIO26_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO26_FUN_IE_S  9
/** IO_MUX_GPIO26_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO26_FUN_DRV    0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_M  (IO_MUX_GPIO26_FUN_DRV_V << IO_MUX_GPIO26_FUN_DRV_S)
#define IO_MUX_GPIO26_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_S  10
/** IO_MUX_GPIO26_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO26_MCU_SEL    0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_M  (IO_MUX_GPIO26_MCU_SEL_V << IO_MUX_GPIO26_MCU_SEL_S)
#define IO_MUX_GPIO26_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_S  12
/** IO_MUX_GPIO26_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO26_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO26_FILTER_EN_M  (IO_MUX_GPIO26_FILTER_EN_V << IO_MUX_GPIO26_FILTER_EN_S)
#define IO_MUX_GPIO26_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO26_FILTER_EN_S  15

/** IO_MUX_gpio27_REG register
 *  iomux control register for gpio27
 */
#define IO_MUX_GPIO27_REG (DR_REG_IO_MUX_BASE + 0x70)
/** IO_MUX_GPIO27_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_OE    (BIT(0))
#define IO_MUX_GPIO27_MCU_OE_M  (IO_MUX_GPIO27_MCU_OE_V << IO_MUX_GPIO27_MCU_OE_S)
#define IO_MUX_GPIO27_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO27_MCU_OE_S  0
/** IO_MUX_GPIO27_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO27_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO27_SLP_SEL_M  (IO_MUX_GPIO27_SLP_SEL_V << IO_MUX_GPIO27_SLP_SEL_S)
#define IO_MUX_GPIO27_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO27_SLP_SEL_S  1
/** IO_MUX_GPIO27_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO27_MCU_WPD_M  (IO_MUX_GPIO27_MCU_WPD_V << IO_MUX_GPIO27_MCU_WPD_S)
#define IO_MUX_GPIO27_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO27_MCU_WPD_S  2
/** IO_MUX_GPIO27_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO27_MCU_WPU_M  (IO_MUX_GPIO27_MCU_WPU_V << IO_MUX_GPIO27_MCU_WPU_S)
#define IO_MUX_GPIO27_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO27_MCU_WPU_S  3
/** IO_MUX_GPIO27_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO27_MCU_IE    (BIT(4))
#define IO_MUX_GPIO27_MCU_IE_M  (IO_MUX_GPIO27_MCU_IE_V << IO_MUX_GPIO27_MCU_IE_S)
#define IO_MUX_GPIO27_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO27_MCU_IE_S  4
/** IO_MUX_GPIO27_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO27_MCU_DRV    0x00000003U
#define IO_MUX_GPIO27_MCU_DRV_M  (IO_MUX_GPIO27_MCU_DRV_V << IO_MUX_GPIO27_MCU_DRV_S)
#define IO_MUX_GPIO27_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO27_MCU_DRV_S  5
/** IO_MUX_GPIO27_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO27_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO27_FUN_WPD_M  (IO_MUX_GPIO27_FUN_WPD_V << IO_MUX_GPIO27_FUN_WPD_S)
#define IO_MUX_GPIO27_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO27_FUN_WPD_S  7
/** IO_MUX_GPIO27_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO27_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO27_FUN_WPU_M  (IO_MUX_GPIO27_FUN_WPU_V << IO_MUX_GPIO27_FUN_WPU_S)
#define IO_MUX_GPIO27_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO27_FUN_WPU_S  8
/** IO_MUX_GPIO27_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO27_FUN_IE    (BIT(9))
#define IO_MUX_GPIO27_FUN_IE_M  (IO_MUX_GPIO27_FUN_IE_V << IO_MUX_GPIO27_FUN_IE_S)
#define IO_MUX_GPIO27_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO27_FUN_IE_S  9
/** IO_MUX_GPIO27_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO27_FUN_DRV    0x00000003U
#define IO_MUX_GPIO27_FUN_DRV_M  (IO_MUX_GPIO27_FUN_DRV_V << IO_MUX_GPIO27_FUN_DRV_S)
#define IO_MUX_GPIO27_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO27_FUN_DRV_S  10
/** IO_MUX_GPIO27_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO27_MCU_SEL    0x00000007U
#define IO_MUX_GPIO27_MCU_SEL_M  (IO_MUX_GPIO27_MCU_SEL_V << IO_MUX_GPIO27_MCU_SEL_S)
#define IO_MUX_GPIO27_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO27_MCU_SEL_S  12
/** IO_MUX_GPIO27_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO27_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO27_FILTER_EN_M  (IO_MUX_GPIO27_FILTER_EN_V << IO_MUX_GPIO27_FILTER_EN_S)
#define IO_MUX_GPIO27_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO27_FILTER_EN_S  15

/** IO_MUX_gpio28_REG register
 *  iomux control register for gpio28
 */
#define IO_MUX_GPIO28_REG (DR_REG_IO_MUX_BASE + 0x74)
/** IO_MUX_GPIO28_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_OE    (BIT(0))
#define IO_MUX_GPIO28_MCU_OE_M  (IO_MUX_GPIO28_MCU_OE_V << IO_MUX_GPIO28_MCU_OE_S)
#define IO_MUX_GPIO28_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO28_MCU_OE_S  0
/** IO_MUX_GPIO28_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO28_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO28_SLP_SEL_M  (IO_MUX_GPIO28_SLP_SEL_V << IO_MUX_GPIO28_SLP_SEL_S)
#define IO_MUX_GPIO28_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO28_SLP_SEL_S  1
/** IO_MUX_GPIO28_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO28_MCU_WPD_M  (IO_MUX_GPIO28_MCU_WPD_V << IO_MUX_GPIO28_MCU_WPD_S)
#define IO_MUX_GPIO28_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO28_MCU_WPD_S  2
/** IO_MUX_GPIO28_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO28_MCU_WPU_M  (IO_MUX_GPIO28_MCU_WPU_V << IO_MUX_GPIO28_MCU_WPU_S)
#define IO_MUX_GPIO28_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO28_MCU_WPU_S  3
/** IO_MUX_GPIO28_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO28_MCU_IE    (BIT(4))
#define IO_MUX_GPIO28_MCU_IE_M  (IO_MUX_GPIO28_MCU_IE_V << IO_MUX_GPIO28_MCU_IE_S)
#define IO_MUX_GPIO28_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO28_MCU_IE_S  4
/** IO_MUX_GPIO28_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO28_MCU_DRV    0x00000003U
#define IO_MUX_GPIO28_MCU_DRV_M  (IO_MUX_GPIO28_MCU_DRV_V << IO_MUX_GPIO28_MCU_DRV_S)
#define IO_MUX_GPIO28_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO28_MCU_DRV_S  5
/** IO_MUX_GPIO28_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO28_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO28_FUN_WPD_M  (IO_MUX_GPIO28_FUN_WPD_V << IO_MUX_GPIO28_FUN_WPD_S)
#define IO_MUX_GPIO28_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO28_FUN_WPD_S  7
/** IO_MUX_GPIO28_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO28_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO28_FUN_WPU_M  (IO_MUX_GPIO28_FUN_WPU_V << IO_MUX_GPIO28_FUN_WPU_S)
#define IO_MUX_GPIO28_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO28_FUN_WPU_S  8
/** IO_MUX_GPIO28_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO28_FUN_IE    (BIT(9))
#define IO_MUX_GPIO28_FUN_IE_M  (IO_MUX_GPIO28_FUN_IE_V << IO_MUX_GPIO28_FUN_IE_S)
#define IO_MUX_GPIO28_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO28_FUN_IE_S  9
/** IO_MUX_GPIO28_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO28_FUN_DRV    0x00000003U
#define IO_MUX_GPIO28_FUN_DRV_M  (IO_MUX_GPIO28_FUN_DRV_V << IO_MUX_GPIO28_FUN_DRV_S)
#define IO_MUX_GPIO28_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO28_FUN_DRV_S  10
/** IO_MUX_GPIO28_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO28_MCU_SEL    0x00000007U
#define IO_MUX_GPIO28_MCU_SEL_M  (IO_MUX_GPIO28_MCU_SEL_V << IO_MUX_GPIO28_MCU_SEL_S)
#define IO_MUX_GPIO28_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO28_MCU_SEL_S  12
/** IO_MUX_GPIO28_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO28_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO28_FILTER_EN_M  (IO_MUX_GPIO28_FILTER_EN_V << IO_MUX_GPIO28_FILTER_EN_S)
#define IO_MUX_GPIO28_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO28_FILTER_EN_S  15

/** IO_MUX_gpio29_REG register
 *  iomux control register for gpio29
 */
#define IO_MUX_GPIO29_REG (DR_REG_IO_MUX_BASE + 0x78)
/** IO_MUX_GPIO29_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_OE    (BIT(0))
#define IO_MUX_GPIO29_MCU_OE_M  (IO_MUX_GPIO29_MCU_OE_V << IO_MUX_GPIO29_MCU_OE_S)
#define IO_MUX_GPIO29_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO29_MCU_OE_S  0
/** IO_MUX_GPIO29_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO29_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO29_SLP_SEL_M  (IO_MUX_GPIO29_SLP_SEL_V << IO_MUX_GPIO29_SLP_SEL_S)
#define IO_MUX_GPIO29_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO29_SLP_SEL_S  1
/** IO_MUX_GPIO29_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO29_MCU_WPD_M  (IO_MUX_GPIO29_MCU_WPD_V << IO_MUX_GPIO29_MCU_WPD_S)
#define IO_MUX_GPIO29_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO29_MCU_WPD_S  2
/** IO_MUX_GPIO29_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO29_MCU_WPU_M  (IO_MUX_GPIO29_MCU_WPU_V << IO_MUX_GPIO29_MCU_WPU_S)
#define IO_MUX_GPIO29_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO29_MCU_WPU_S  3
/** IO_MUX_GPIO29_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO29_MCU_IE    (BIT(4))
#define IO_MUX_GPIO29_MCU_IE_M  (IO_MUX_GPIO29_MCU_IE_V << IO_MUX_GPIO29_MCU_IE_S)
#define IO_MUX_GPIO29_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO29_MCU_IE_S  4
/** IO_MUX_GPIO29_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO29_MCU_DRV    0x00000003U
#define IO_MUX_GPIO29_MCU_DRV_M  (IO_MUX_GPIO29_MCU_DRV_V << IO_MUX_GPIO29_MCU_DRV_S)
#define IO_MUX_GPIO29_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO29_MCU_DRV_S  5
/** IO_MUX_GPIO29_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO29_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO29_FUN_WPD_M  (IO_MUX_GPIO29_FUN_WPD_V << IO_MUX_GPIO29_FUN_WPD_S)
#define IO_MUX_GPIO29_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO29_FUN_WPD_S  7
/** IO_MUX_GPIO29_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO29_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO29_FUN_WPU_M  (IO_MUX_GPIO29_FUN_WPU_V << IO_MUX_GPIO29_FUN_WPU_S)
#define IO_MUX_GPIO29_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO29_FUN_WPU_S  8
/** IO_MUX_GPIO29_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO29_FUN_IE    (BIT(9))
#define IO_MUX_GPIO29_FUN_IE_M  (IO_MUX_GPIO29_FUN_IE_V << IO_MUX_GPIO29_FUN_IE_S)
#define IO_MUX_GPIO29_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO29_FUN_IE_S  9
/** IO_MUX_GPIO29_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO29_FUN_DRV    0x00000003U
#define IO_MUX_GPIO29_FUN_DRV_M  (IO_MUX_GPIO29_FUN_DRV_V << IO_MUX_GPIO29_FUN_DRV_S)
#define IO_MUX_GPIO29_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO29_FUN_DRV_S  10
/** IO_MUX_GPIO29_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO29_MCU_SEL    0x00000007U
#define IO_MUX_GPIO29_MCU_SEL_M  (IO_MUX_GPIO29_MCU_SEL_V << IO_MUX_GPIO29_MCU_SEL_S)
#define IO_MUX_GPIO29_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO29_MCU_SEL_S  12
/** IO_MUX_GPIO29_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO29_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO29_FILTER_EN_M  (IO_MUX_GPIO29_FILTER_EN_V << IO_MUX_GPIO29_FILTER_EN_S)
#define IO_MUX_GPIO29_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO29_FILTER_EN_S  15

/** IO_MUX_gpio30_REG register
 *  iomux control register for gpio30
 */
#define IO_MUX_GPIO30_REG (DR_REG_IO_MUX_BASE + 0x7c)
/** IO_MUX_GPIO30_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_OE    (BIT(0))
#define IO_MUX_GPIO30_MCU_OE_M  (IO_MUX_GPIO30_MCU_OE_V << IO_MUX_GPIO30_MCU_OE_S)
#define IO_MUX_GPIO30_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO30_MCU_OE_S  0
/** IO_MUX_GPIO30_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO30_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO30_SLP_SEL_M  (IO_MUX_GPIO30_SLP_SEL_V << IO_MUX_GPIO30_SLP_SEL_S)
#define IO_MUX_GPIO30_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO30_SLP_SEL_S  1
/** IO_MUX_GPIO30_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO30_MCU_WPD_M  (IO_MUX_GPIO30_MCU_WPD_V << IO_MUX_GPIO30_MCU_WPD_S)
#define IO_MUX_GPIO30_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO30_MCU_WPD_S  2
/** IO_MUX_GPIO30_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO30_MCU_WPU_M  (IO_MUX_GPIO30_MCU_WPU_V << IO_MUX_GPIO30_MCU_WPU_S)
#define IO_MUX_GPIO30_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO30_MCU_WPU_S  3
/** IO_MUX_GPIO30_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO30_MCU_IE    (BIT(4))
#define IO_MUX_GPIO30_MCU_IE_M  (IO_MUX_GPIO30_MCU_IE_V << IO_MUX_GPIO30_MCU_IE_S)
#define IO_MUX_GPIO30_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO30_MCU_IE_S  4
/** IO_MUX_GPIO30_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO30_MCU_DRV    0x00000003U
#define IO_MUX_GPIO30_MCU_DRV_M  (IO_MUX_GPIO30_MCU_DRV_V << IO_MUX_GPIO30_MCU_DRV_S)
#define IO_MUX_GPIO30_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO30_MCU_DRV_S  5
/** IO_MUX_GPIO30_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO30_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO30_FUN_WPD_M  (IO_MUX_GPIO30_FUN_WPD_V << IO_MUX_GPIO30_FUN_WPD_S)
#define IO_MUX_GPIO30_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO30_FUN_WPD_S  7
/** IO_MUX_GPIO30_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO30_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO30_FUN_WPU_M  (IO_MUX_GPIO30_FUN_WPU_V << IO_MUX_GPIO30_FUN_WPU_S)
#define IO_MUX_GPIO30_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO30_FUN_WPU_S  8
/** IO_MUX_GPIO30_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO30_FUN_IE    (BIT(9))
#define IO_MUX_GPIO30_FUN_IE_M  (IO_MUX_GPIO30_FUN_IE_V << IO_MUX_GPIO30_FUN_IE_S)
#define IO_MUX_GPIO30_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO30_FUN_IE_S  9
/** IO_MUX_GPIO30_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO30_FUN_DRV    0x00000003U
#define IO_MUX_GPIO30_FUN_DRV_M  (IO_MUX_GPIO30_FUN_DRV_V << IO_MUX_GPIO30_FUN_DRV_S)
#define IO_MUX_GPIO30_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO30_FUN_DRV_S  10
/** IO_MUX_GPIO30_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO30_MCU_SEL    0x00000007U
#define IO_MUX_GPIO30_MCU_SEL_M  (IO_MUX_GPIO30_MCU_SEL_V << IO_MUX_GPIO30_MCU_SEL_S)
#define IO_MUX_GPIO30_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO30_MCU_SEL_S  12
/** IO_MUX_GPIO30_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO30_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO30_FILTER_EN_M  (IO_MUX_GPIO30_FILTER_EN_V << IO_MUX_GPIO30_FILTER_EN_S)
#define IO_MUX_GPIO30_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO30_FILTER_EN_S  15

/** IO_MUX_gpio31_REG register
 *  iomux control register for gpio31
 */
#define IO_MUX_GPIO31_REG (DR_REG_IO_MUX_BASE + 0x80)
/** IO_MUX_GPIO31_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_OE    (BIT(0))
#define IO_MUX_GPIO31_MCU_OE_M  (IO_MUX_GPIO31_MCU_OE_V << IO_MUX_GPIO31_MCU_OE_S)
#define IO_MUX_GPIO31_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO31_MCU_OE_S  0
/** IO_MUX_GPIO31_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO31_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO31_SLP_SEL_M  (IO_MUX_GPIO31_SLP_SEL_V << IO_MUX_GPIO31_SLP_SEL_S)
#define IO_MUX_GPIO31_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO31_SLP_SEL_S  1
/** IO_MUX_GPIO31_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO31_MCU_WPD_M  (IO_MUX_GPIO31_MCU_WPD_V << IO_MUX_GPIO31_MCU_WPD_S)
#define IO_MUX_GPIO31_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO31_MCU_WPD_S  2
/** IO_MUX_GPIO31_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO31_MCU_WPU_M  (IO_MUX_GPIO31_MCU_WPU_V << IO_MUX_GPIO31_MCU_WPU_S)
#define IO_MUX_GPIO31_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO31_MCU_WPU_S  3
/** IO_MUX_GPIO31_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO31_MCU_IE    (BIT(4))
#define IO_MUX_GPIO31_MCU_IE_M  (IO_MUX_GPIO31_MCU_IE_V << IO_MUX_GPIO31_MCU_IE_S)
#define IO_MUX_GPIO31_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO31_MCU_IE_S  4
/** IO_MUX_GPIO31_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO31_MCU_DRV    0x00000003U
#define IO_MUX_GPIO31_MCU_DRV_M  (IO_MUX_GPIO31_MCU_DRV_V << IO_MUX_GPIO31_MCU_DRV_S)
#define IO_MUX_GPIO31_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO31_MCU_DRV_S  5
/** IO_MUX_GPIO31_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO31_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO31_FUN_WPD_M  (IO_MUX_GPIO31_FUN_WPD_V << IO_MUX_GPIO31_FUN_WPD_S)
#define IO_MUX_GPIO31_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO31_FUN_WPD_S  7
/** IO_MUX_GPIO31_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO31_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO31_FUN_WPU_M  (IO_MUX_GPIO31_FUN_WPU_V << IO_MUX_GPIO31_FUN_WPU_S)
#define IO_MUX_GPIO31_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO31_FUN_WPU_S  8
/** IO_MUX_GPIO31_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO31_FUN_IE    (BIT(9))
#define IO_MUX_GPIO31_FUN_IE_M  (IO_MUX_GPIO31_FUN_IE_V << IO_MUX_GPIO31_FUN_IE_S)
#define IO_MUX_GPIO31_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO31_FUN_IE_S  9
/** IO_MUX_GPIO31_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO31_FUN_DRV    0x00000003U
#define IO_MUX_GPIO31_FUN_DRV_M  (IO_MUX_GPIO31_FUN_DRV_V << IO_MUX_GPIO31_FUN_DRV_S)
#define IO_MUX_GPIO31_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO31_FUN_DRV_S  10
/** IO_MUX_GPIO31_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO31_MCU_SEL    0x00000007U
#define IO_MUX_GPIO31_MCU_SEL_M  (IO_MUX_GPIO31_MCU_SEL_V << IO_MUX_GPIO31_MCU_SEL_S)
#define IO_MUX_GPIO31_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO31_MCU_SEL_S  12
/** IO_MUX_GPIO31_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO31_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO31_FILTER_EN_M  (IO_MUX_GPIO31_FILTER_EN_V << IO_MUX_GPIO31_FILTER_EN_S)
#define IO_MUX_GPIO31_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO31_FILTER_EN_S  15

/** IO_MUX_gpio32_REG register
 *  iomux control register for gpio32
 */
#define IO_MUX_GPIO32_REG (DR_REG_IO_MUX_BASE + 0x84)
/** IO_MUX_GPIO32_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_OE    (BIT(0))
#define IO_MUX_GPIO32_MCU_OE_M  (IO_MUX_GPIO32_MCU_OE_V << IO_MUX_GPIO32_MCU_OE_S)
#define IO_MUX_GPIO32_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO32_MCU_OE_S  0
/** IO_MUX_GPIO32_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO32_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO32_SLP_SEL_M  (IO_MUX_GPIO32_SLP_SEL_V << IO_MUX_GPIO32_SLP_SEL_S)
#define IO_MUX_GPIO32_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO32_SLP_SEL_S  1
/** IO_MUX_GPIO32_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO32_MCU_WPD_M  (IO_MUX_GPIO32_MCU_WPD_V << IO_MUX_GPIO32_MCU_WPD_S)
#define IO_MUX_GPIO32_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO32_MCU_WPD_S  2
/** IO_MUX_GPIO32_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO32_MCU_WPU_M  (IO_MUX_GPIO32_MCU_WPU_V << IO_MUX_GPIO32_MCU_WPU_S)
#define IO_MUX_GPIO32_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO32_MCU_WPU_S  3
/** IO_MUX_GPIO32_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO32_MCU_IE    (BIT(4))
#define IO_MUX_GPIO32_MCU_IE_M  (IO_MUX_GPIO32_MCU_IE_V << IO_MUX_GPIO32_MCU_IE_S)
#define IO_MUX_GPIO32_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO32_MCU_IE_S  4
/** IO_MUX_GPIO32_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO32_MCU_DRV    0x00000003U
#define IO_MUX_GPIO32_MCU_DRV_M  (IO_MUX_GPIO32_MCU_DRV_V << IO_MUX_GPIO32_MCU_DRV_S)
#define IO_MUX_GPIO32_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO32_MCU_DRV_S  5
/** IO_MUX_GPIO32_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO32_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO32_FUN_WPD_M  (IO_MUX_GPIO32_FUN_WPD_V << IO_MUX_GPIO32_FUN_WPD_S)
#define IO_MUX_GPIO32_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO32_FUN_WPD_S  7
/** IO_MUX_GPIO32_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO32_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO32_FUN_WPU_M  (IO_MUX_GPIO32_FUN_WPU_V << IO_MUX_GPIO32_FUN_WPU_S)
#define IO_MUX_GPIO32_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO32_FUN_WPU_S  8
/** IO_MUX_GPIO32_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO32_FUN_IE    (BIT(9))
#define IO_MUX_GPIO32_FUN_IE_M  (IO_MUX_GPIO32_FUN_IE_V << IO_MUX_GPIO32_FUN_IE_S)
#define IO_MUX_GPIO32_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO32_FUN_IE_S  9
/** IO_MUX_GPIO32_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO32_FUN_DRV    0x00000003U
#define IO_MUX_GPIO32_FUN_DRV_M  (IO_MUX_GPIO32_FUN_DRV_V << IO_MUX_GPIO32_FUN_DRV_S)
#define IO_MUX_GPIO32_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO32_FUN_DRV_S  10
/** IO_MUX_GPIO32_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO32_MCU_SEL    0x00000007U
#define IO_MUX_GPIO32_MCU_SEL_M  (IO_MUX_GPIO32_MCU_SEL_V << IO_MUX_GPIO32_MCU_SEL_S)
#define IO_MUX_GPIO32_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO32_MCU_SEL_S  12
/** IO_MUX_GPIO32_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO32_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO32_FILTER_EN_M  (IO_MUX_GPIO32_FILTER_EN_V << IO_MUX_GPIO32_FILTER_EN_S)
#define IO_MUX_GPIO32_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO32_FILTER_EN_S  15
/** IO_MUX_GPIO32_RUE_I3C : R/W; bitpos: [16]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO32_RUE_I3C    (BIT(16))
#define IO_MUX_GPIO32_RUE_I3C_M  (IO_MUX_GPIO32_RUE_I3C_V << IO_MUX_GPIO32_RUE_I3C_S)
#define IO_MUX_GPIO32_RUE_I3C_V  0x00000001U
#define IO_MUX_GPIO32_RUE_I3C_S  16
/** IO_MUX_GPIO32_RU_I3C : R/W; bitpos: [18:17]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO32_RU_I3C    0x00000003U
#define IO_MUX_GPIO32_RU_I3C_M  (IO_MUX_GPIO32_RU_I3C_V << IO_MUX_GPIO32_RU_I3C_S)
#define IO_MUX_GPIO32_RU_I3C_V  0x00000003U
#define IO_MUX_GPIO32_RU_I3C_S  17
/** IO_MUX_GPIO32_RUE_SEL_I3C : R/W; bitpos: [19]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO32_RUE_SEL_I3C    (BIT(19))
#define IO_MUX_GPIO32_RUE_SEL_I3C_M  (IO_MUX_GPIO32_RUE_SEL_I3C_V << IO_MUX_GPIO32_RUE_SEL_I3C_S)
#define IO_MUX_GPIO32_RUE_SEL_I3C_V  0x00000001U
#define IO_MUX_GPIO32_RUE_SEL_I3C_S  19

/** IO_MUX_gpio33_REG register
 *  iomux control register for gpio33
 */
#define IO_MUX_GPIO33_REG (DR_REG_IO_MUX_BASE + 0x88)
/** IO_MUX_GPIO33_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_OE    (BIT(0))
#define IO_MUX_GPIO33_MCU_OE_M  (IO_MUX_GPIO33_MCU_OE_V << IO_MUX_GPIO33_MCU_OE_S)
#define IO_MUX_GPIO33_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO33_MCU_OE_S  0
/** IO_MUX_GPIO33_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO33_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO33_SLP_SEL_M  (IO_MUX_GPIO33_SLP_SEL_V << IO_MUX_GPIO33_SLP_SEL_S)
#define IO_MUX_GPIO33_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO33_SLP_SEL_S  1
/** IO_MUX_GPIO33_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO33_MCU_WPD_M  (IO_MUX_GPIO33_MCU_WPD_V << IO_MUX_GPIO33_MCU_WPD_S)
#define IO_MUX_GPIO33_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO33_MCU_WPD_S  2
/** IO_MUX_GPIO33_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO33_MCU_WPU_M  (IO_MUX_GPIO33_MCU_WPU_V << IO_MUX_GPIO33_MCU_WPU_S)
#define IO_MUX_GPIO33_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO33_MCU_WPU_S  3
/** IO_MUX_GPIO33_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO33_MCU_IE    (BIT(4))
#define IO_MUX_GPIO33_MCU_IE_M  (IO_MUX_GPIO33_MCU_IE_V << IO_MUX_GPIO33_MCU_IE_S)
#define IO_MUX_GPIO33_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO33_MCU_IE_S  4
/** IO_MUX_GPIO33_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO33_MCU_DRV    0x00000003U
#define IO_MUX_GPIO33_MCU_DRV_M  (IO_MUX_GPIO33_MCU_DRV_V << IO_MUX_GPIO33_MCU_DRV_S)
#define IO_MUX_GPIO33_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO33_MCU_DRV_S  5
/** IO_MUX_GPIO33_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO33_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO33_FUN_WPD_M  (IO_MUX_GPIO33_FUN_WPD_V << IO_MUX_GPIO33_FUN_WPD_S)
#define IO_MUX_GPIO33_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO33_FUN_WPD_S  7
/** IO_MUX_GPIO33_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO33_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO33_FUN_WPU_M  (IO_MUX_GPIO33_FUN_WPU_V << IO_MUX_GPIO33_FUN_WPU_S)
#define IO_MUX_GPIO33_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO33_FUN_WPU_S  8
/** IO_MUX_GPIO33_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO33_FUN_IE    (BIT(9))
#define IO_MUX_GPIO33_FUN_IE_M  (IO_MUX_GPIO33_FUN_IE_V << IO_MUX_GPIO33_FUN_IE_S)
#define IO_MUX_GPIO33_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO33_FUN_IE_S  9
/** IO_MUX_GPIO33_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO33_FUN_DRV    0x00000003U
#define IO_MUX_GPIO33_FUN_DRV_M  (IO_MUX_GPIO33_FUN_DRV_V << IO_MUX_GPIO33_FUN_DRV_S)
#define IO_MUX_GPIO33_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO33_FUN_DRV_S  10
/** IO_MUX_GPIO33_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO33_MCU_SEL    0x00000007U
#define IO_MUX_GPIO33_MCU_SEL_M  (IO_MUX_GPIO33_MCU_SEL_V << IO_MUX_GPIO33_MCU_SEL_S)
#define IO_MUX_GPIO33_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO33_MCU_SEL_S  12
/** IO_MUX_GPIO33_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO33_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO33_FILTER_EN_M  (IO_MUX_GPIO33_FILTER_EN_V << IO_MUX_GPIO33_FILTER_EN_S)
#define IO_MUX_GPIO33_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO33_FILTER_EN_S  15
/** IO_MUX_GPIO33_RUE_I3C : R/W; bitpos: [16]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO33_RUE_I3C    (BIT(16))
#define IO_MUX_GPIO33_RUE_I3C_M  (IO_MUX_GPIO33_RUE_I3C_V << IO_MUX_GPIO33_RUE_I3C_S)
#define IO_MUX_GPIO33_RUE_I3C_V  0x00000001U
#define IO_MUX_GPIO33_RUE_I3C_S  16
/** IO_MUX_GPIO33_RU_I3C : R/W; bitpos: [18:17]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO33_RU_I3C    0x00000003U
#define IO_MUX_GPIO33_RU_I3C_M  (IO_MUX_GPIO33_RU_I3C_V << IO_MUX_GPIO33_RU_I3C_S)
#define IO_MUX_GPIO33_RU_I3C_V  0x00000003U
#define IO_MUX_GPIO33_RU_I3C_S  17
/** IO_MUX_GPIO33_RUE_SEL_I3C : R/W; bitpos: [19]; default: 0;
 *  NA
 */
#define IO_MUX_GPIO33_RUE_SEL_I3C    (BIT(19))
#define IO_MUX_GPIO33_RUE_SEL_I3C_M  (IO_MUX_GPIO33_RUE_SEL_I3C_V << IO_MUX_GPIO33_RUE_SEL_I3C_S)
#define IO_MUX_GPIO33_RUE_SEL_I3C_V  0x00000001U
#define IO_MUX_GPIO33_RUE_SEL_I3C_S  19

/** IO_MUX_gpio34_REG register
 *  iomux control register for gpio34
 */
#define IO_MUX_GPIO34_REG (DR_REG_IO_MUX_BASE + 0x8c)
/** IO_MUX_GPIO34_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_OE    (BIT(0))
#define IO_MUX_GPIO34_MCU_OE_M  (IO_MUX_GPIO34_MCU_OE_V << IO_MUX_GPIO34_MCU_OE_S)
#define IO_MUX_GPIO34_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO34_MCU_OE_S  0
/** IO_MUX_GPIO34_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO34_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO34_SLP_SEL_M  (IO_MUX_GPIO34_SLP_SEL_V << IO_MUX_GPIO34_SLP_SEL_S)
#define IO_MUX_GPIO34_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO34_SLP_SEL_S  1
/** IO_MUX_GPIO34_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO34_MCU_WPD_M  (IO_MUX_GPIO34_MCU_WPD_V << IO_MUX_GPIO34_MCU_WPD_S)
#define IO_MUX_GPIO34_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO34_MCU_WPD_S  2
/** IO_MUX_GPIO34_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO34_MCU_WPU_M  (IO_MUX_GPIO34_MCU_WPU_V << IO_MUX_GPIO34_MCU_WPU_S)
#define IO_MUX_GPIO34_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO34_MCU_WPU_S  3
/** IO_MUX_GPIO34_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO34_MCU_IE    (BIT(4))
#define IO_MUX_GPIO34_MCU_IE_M  (IO_MUX_GPIO34_MCU_IE_V << IO_MUX_GPIO34_MCU_IE_S)
#define IO_MUX_GPIO34_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO34_MCU_IE_S  4
/** IO_MUX_GPIO34_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO34_MCU_DRV    0x00000003U
#define IO_MUX_GPIO34_MCU_DRV_M  (IO_MUX_GPIO34_MCU_DRV_V << IO_MUX_GPIO34_MCU_DRV_S)
#define IO_MUX_GPIO34_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO34_MCU_DRV_S  5
/** IO_MUX_GPIO34_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO34_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO34_FUN_WPD_M  (IO_MUX_GPIO34_FUN_WPD_V << IO_MUX_GPIO34_FUN_WPD_S)
#define IO_MUX_GPIO34_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO34_FUN_WPD_S  7
/** IO_MUX_GPIO34_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO34_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO34_FUN_WPU_M  (IO_MUX_GPIO34_FUN_WPU_V << IO_MUX_GPIO34_FUN_WPU_S)
#define IO_MUX_GPIO34_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO34_FUN_WPU_S  8
/** IO_MUX_GPIO34_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO34_FUN_IE    (BIT(9))
#define IO_MUX_GPIO34_FUN_IE_M  (IO_MUX_GPIO34_FUN_IE_V << IO_MUX_GPIO34_FUN_IE_S)
#define IO_MUX_GPIO34_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO34_FUN_IE_S  9
/** IO_MUX_GPIO34_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO34_FUN_DRV    0x00000003U
#define IO_MUX_GPIO34_FUN_DRV_M  (IO_MUX_GPIO34_FUN_DRV_V << IO_MUX_GPIO34_FUN_DRV_S)
#define IO_MUX_GPIO34_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO34_FUN_DRV_S  10
/** IO_MUX_GPIO34_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO34_MCU_SEL    0x00000007U
#define IO_MUX_GPIO34_MCU_SEL_M  (IO_MUX_GPIO34_MCU_SEL_V << IO_MUX_GPIO34_MCU_SEL_S)
#define IO_MUX_GPIO34_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO34_MCU_SEL_S  12
/** IO_MUX_GPIO34_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO34_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO34_FILTER_EN_M  (IO_MUX_GPIO34_FILTER_EN_V << IO_MUX_GPIO34_FILTER_EN_S)
#define IO_MUX_GPIO34_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO34_FILTER_EN_S  15

/** IO_MUX_gpio35_REG register
 *  iomux control register for gpio35
 */
#define IO_MUX_GPIO35_REG (DR_REG_IO_MUX_BASE + 0x90)
/** IO_MUX_GPIO35_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_OE    (BIT(0))
#define IO_MUX_GPIO35_MCU_OE_M  (IO_MUX_GPIO35_MCU_OE_V << IO_MUX_GPIO35_MCU_OE_S)
#define IO_MUX_GPIO35_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO35_MCU_OE_S  0
/** IO_MUX_GPIO35_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO35_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO35_SLP_SEL_M  (IO_MUX_GPIO35_SLP_SEL_V << IO_MUX_GPIO35_SLP_SEL_S)
#define IO_MUX_GPIO35_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO35_SLP_SEL_S  1
/** IO_MUX_GPIO35_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO35_MCU_WPD_M  (IO_MUX_GPIO35_MCU_WPD_V << IO_MUX_GPIO35_MCU_WPD_S)
#define IO_MUX_GPIO35_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO35_MCU_WPD_S  2
/** IO_MUX_GPIO35_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO35_MCU_WPU_M  (IO_MUX_GPIO35_MCU_WPU_V << IO_MUX_GPIO35_MCU_WPU_S)
#define IO_MUX_GPIO35_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO35_MCU_WPU_S  3
/** IO_MUX_GPIO35_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO35_MCU_IE    (BIT(4))
#define IO_MUX_GPIO35_MCU_IE_M  (IO_MUX_GPIO35_MCU_IE_V << IO_MUX_GPIO35_MCU_IE_S)
#define IO_MUX_GPIO35_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO35_MCU_IE_S  4
/** IO_MUX_GPIO35_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO35_MCU_DRV    0x00000003U
#define IO_MUX_GPIO35_MCU_DRV_M  (IO_MUX_GPIO35_MCU_DRV_V << IO_MUX_GPIO35_MCU_DRV_S)
#define IO_MUX_GPIO35_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO35_MCU_DRV_S  5
/** IO_MUX_GPIO35_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO35_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO35_FUN_WPD_M  (IO_MUX_GPIO35_FUN_WPD_V << IO_MUX_GPIO35_FUN_WPD_S)
#define IO_MUX_GPIO35_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO35_FUN_WPD_S  7
/** IO_MUX_GPIO35_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO35_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO35_FUN_WPU_M  (IO_MUX_GPIO35_FUN_WPU_V << IO_MUX_GPIO35_FUN_WPU_S)
#define IO_MUX_GPIO35_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO35_FUN_WPU_S  8
/** IO_MUX_GPIO35_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO35_FUN_IE    (BIT(9))
#define IO_MUX_GPIO35_FUN_IE_M  (IO_MUX_GPIO35_FUN_IE_V << IO_MUX_GPIO35_FUN_IE_S)
#define IO_MUX_GPIO35_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO35_FUN_IE_S  9
/** IO_MUX_GPIO35_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO35_FUN_DRV    0x00000003U
#define IO_MUX_GPIO35_FUN_DRV_M  (IO_MUX_GPIO35_FUN_DRV_V << IO_MUX_GPIO35_FUN_DRV_S)
#define IO_MUX_GPIO35_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO35_FUN_DRV_S  10
/** IO_MUX_GPIO35_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO35_MCU_SEL    0x00000007U
#define IO_MUX_GPIO35_MCU_SEL_M  (IO_MUX_GPIO35_MCU_SEL_V << IO_MUX_GPIO35_MCU_SEL_S)
#define IO_MUX_GPIO35_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO35_MCU_SEL_S  12
/** IO_MUX_GPIO35_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO35_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO35_FILTER_EN_M  (IO_MUX_GPIO35_FILTER_EN_V << IO_MUX_GPIO35_FILTER_EN_S)
#define IO_MUX_GPIO35_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO35_FILTER_EN_S  15

/** IO_MUX_gpio36_REG register
 *  iomux control register for gpio36
 */
#define IO_MUX_GPIO36_REG (DR_REG_IO_MUX_BASE + 0x94)
/** IO_MUX_GPIO36_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_OE    (BIT(0))
#define IO_MUX_GPIO36_MCU_OE_M  (IO_MUX_GPIO36_MCU_OE_V << IO_MUX_GPIO36_MCU_OE_S)
#define IO_MUX_GPIO36_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO36_MCU_OE_S  0
/** IO_MUX_GPIO36_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO36_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO36_SLP_SEL_M  (IO_MUX_GPIO36_SLP_SEL_V << IO_MUX_GPIO36_SLP_SEL_S)
#define IO_MUX_GPIO36_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO36_SLP_SEL_S  1
/** IO_MUX_GPIO36_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO36_MCU_WPD_M  (IO_MUX_GPIO36_MCU_WPD_V << IO_MUX_GPIO36_MCU_WPD_S)
#define IO_MUX_GPIO36_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO36_MCU_WPD_S  2
/** IO_MUX_GPIO36_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO36_MCU_WPU_M  (IO_MUX_GPIO36_MCU_WPU_V << IO_MUX_GPIO36_MCU_WPU_S)
#define IO_MUX_GPIO36_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO36_MCU_WPU_S  3
/** IO_MUX_GPIO36_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO36_MCU_IE    (BIT(4))
#define IO_MUX_GPIO36_MCU_IE_M  (IO_MUX_GPIO36_MCU_IE_V << IO_MUX_GPIO36_MCU_IE_S)
#define IO_MUX_GPIO36_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO36_MCU_IE_S  4
/** IO_MUX_GPIO36_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO36_MCU_DRV    0x00000003U
#define IO_MUX_GPIO36_MCU_DRV_M  (IO_MUX_GPIO36_MCU_DRV_V << IO_MUX_GPIO36_MCU_DRV_S)
#define IO_MUX_GPIO36_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO36_MCU_DRV_S  5
/** IO_MUX_GPIO36_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO36_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO36_FUN_WPD_M  (IO_MUX_GPIO36_FUN_WPD_V << IO_MUX_GPIO36_FUN_WPD_S)
#define IO_MUX_GPIO36_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO36_FUN_WPD_S  7
/** IO_MUX_GPIO36_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO36_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO36_FUN_WPU_M  (IO_MUX_GPIO36_FUN_WPU_V << IO_MUX_GPIO36_FUN_WPU_S)
#define IO_MUX_GPIO36_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO36_FUN_WPU_S  8
/** IO_MUX_GPIO36_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO36_FUN_IE    (BIT(9))
#define IO_MUX_GPIO36_FUN_IE_M  (IO_MUX_GPIO36_FUN_IE_V << IO_MUX_GPIO36_FUN_IE_S)
#define IO_MUX_GPIO36_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO36_FUN_IE_S  9
/** IO_MUX_GPIO36_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO36_FUN_DRV    0x00000003U
#define IO_MUX_GPIO36_FUN_DRV_M  (IO_MUX_GPIO36_FUN_DRV_V << IO_MUX_GPIO36_FUN_DRV_S)
#define IO_MUX_GPIO36_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO36_FUN_DRV_S  10
/** IO_MUX_GPIO36_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO36_MCU_SEL    0x00000007U
#define IO_MUX_GPIO36_MCU_SEL_M  (IO_MUX_GPIO36_MCU_SEL_V << IO_MUX_GPIO36_MCU_SEL_S)
#define IO_MUX_GPIO36_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO36_MCU_SEL_S  12
/** IO_MUX_GPIO36_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO36_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO36_FILTER_EN_M  (IO_MUX_GPIO36_FILTER_EN_V << IO_MUX_GPIO36_FILTER_EN_S)
#define IO_MUX_GPIO36_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO36_FILTER_EN_S  15

/** IO_MUX_gpio37_REG register
 *  iomux control register for gpio37
 */
#define IO_MUX_GPIO37_REG (DR_REG_IO_MUX_BASE + 0x98)
/** IO_MUX_GPIO37_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_OE    (BIT(0))
#define IO_MUX_GPIO37_MCU_OE_M  (IO_MUX_GPIO37_MCU_OE_V << IO_MUX_GPIO37_MCU_OE_S)
#define IO_MUX_GPIO37_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO37_MCU_OE_S  0
/** IO_MUX_GPIO37_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO37_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO37_SLP_SEL_M  (IO_MUX_GPIO37_SLP_SEL_V << IO_MUX_GPIO37_SLP_SEL_S)
#define IO_MUX_GPIO37_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO37_SLP_SEL_S  1
/** IO_MUX_GPIO37_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO37_MCU_WPD_M  (IO_MUX_GPIO37_MCU_WPD_V << IO_MUX_GPIO37_MCU_WPD_S)
#define IO_MUX_GPIO37_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO37_MCU_WPD_S  2
/** IO_MUX_GPIO37_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO37_MCU_WPU_M  (IO_MUX_GPIO37_MCU_WPU_V << IO_MUX_GPIO37_MCU_WPU_S)
#define IO_MUX_GPIO37_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO37_MCU_WPU_S  3
/** IO_MUX_GPIO37_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO37_MCU_IE    (BIT(4))
#define IO_MUX_GPIO37_MCU_IE_M  (IO_MUX_GPIO37_MCU_IE_V << IO_MUX_GPIO37_MCU_IE_S)
#define IO_MUX_GPIO37_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO37_MCU_IE_S  4
/** IO_MUX_GPIO37_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO37_MCU_DRV    0x00000003U
#define IO_MUX_GPIO37_MCU_DRV_M  (IO_MUX_GPIO37_MCU_DRV_V << IO_MUX_GPIO37_MCU_DRV_S)
#define IO_MUX_GPIO37_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO37_MCU_DRV_S  5
/** IO_MUX_GPIO37_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO37_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO37_FUN_WPD_M  (IO_MUX_GPIO37_FUN_WPD_V << IO_MUX_GPIO37_FUN_WPD_S)
#define IO_MUX_GPIO37_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO37_FUN_WPD_S  7
/** IO_MUX_GPIO37_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO37_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO37_FUN_WPU_M  (IO_MUX_GPIO37_FUN_WPU_V << IO_MUX_GPIO37_FUN_WPU_S)
#define IO_MUX_GPIO37_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO37_FUN_WPU_S  8
/** IO_MUX_GPIO37_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO37_FUN_IE    (BIT(9))
#define IO_MUX_GPIO37_FUN_IE_M  (IO_MUX_GPIO37_FUN_IE_V << IO_MUX_GPIO37_FUN_IE_S)
#define IO_MUX_GPIO37_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO37_FUN_IE_S  9
/** IO_MUX_GPIO37_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO37_FUN_DRV    0x00000003U
#define IO_MUX_GPIO37_FUN_DRV_M  (IO_MUX_GPIO37_FUN_DRV_V << IO_MUX_GPIO37_FUN_DRV_S)
#define IO_MUX_GPIO37_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO37_FUN_DRV_S  10
/** IO_MUX_GPIO37_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO37_MCU_SEL    0x00000007U
#define IO_MUX_GPIO37_MCU_SEL_M  (IO_MUX_GPIO37_MCU_SEL_V << IO_MUX_GPIO37_MCU_SEL_S)
#define IO_MUX_GPIO37_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO37_MCU_SEL_S  12
/** IO_MUX_GPIO37_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO37_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO37_FILTER_EN_M  (IO_MUX_GPIO37_FILTER_EN_V << IO_MUX_GPIO37_FILTER_EN_S)
#define IO_MUX_GPIO37_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO37_FILTER_EN_S  15

/** IO_MUX_gpio38_REG register
 *  iomux control register for gpio38
 */
#define IO_MUX_GPIO38_REG (DR_REG_IO_MUX_BASE + 0x9c)
/** IO_MUX_GPIO38_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_OE    (BIT(0))
#define IO_MUX_GPIO38_MCU_OE_M  (IO_MUX_GPIO38_MCU_OE_V << IO_MUX_GPIO38_MCU_OE_S)
#define IO_MUX_GPIO38_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO38_MCU_OE_S  0
/** IO_MUX_GPIO38_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO38_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO38_SLP_SEL_M  (IO_MUX_GPIO38_SLP_SEL_V << IO_MUX_GPIO38_SLP_SEL_S)
#define IO_MUX_GPIO38_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO38_SLP_SEL_S  1
/** IO_MUX_GPIO38_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO38_MCU_WPD_M  (IO_MUX_GPIO38_MCU_WPD_V << IO_MUX_GPIO38_MCU_WPD_S)
#define IO_MUX_GPIO38_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO38_MCU_WPD_S  2
/** IO_MUX_GPIO38_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO38_MCU_WPU_M  (IO_MUX_GPIO38_MCU_WPU_V << IO_MUX_GPIO38_MCU_WPU_S)
#define IO_MUX_GPIO38_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO38_MCU_WPU_S  3
/** IO_MUX_GPIO38_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO38_MCU_IE    (BIT(4))
#define IO_MUX_GPIO38_MCU_IE_M  (IO_MUX_GPIO38_MCU_IE_V << IO_MUX_GPIO38_MCU_IE_S)
#define IO_MUX_GPIO38_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO38_MCU_IE_S  4
/** IO_MUX_GPIO38_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO38_MCU_DRV    0x00000003U
#define IO_MUX_GPIO38_MCU_DRV_M  (IO_MUX_GPIO38_MCU_DRV_V << IO_MUX_GPIO38_MCU_DRV_S)
#define IO_MUX_GPIO38_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO38_MCU_DRV_S  5
/** IO_MUX_GPIO38_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO38_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO38_FUN_WPD_M  (IO_MUX_GPIO38_FUN_WPD_V << IO_MUX_GPIO38_FUN_WPD_S)
#define IO_MUX_GPIO38_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO38_FUN_WPD_S  7
/** IO_MUX_GPIO38_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO38_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO38_FUN_WPU_M  (IO_MUX_GPIO38_FUN_WPU_V << IO_MUX_GPIO38_FUN_WPU_S)
#define IO_MUX_GPIO38_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO38_FUN_WPU_S  8
/** IO_MUX_GPIO38_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO38_FUN_IE    (BIT(9))
#define IO_MUX_GPIO38_FUN_IE_M  (IO_MUX_GPIO38_FUN_IE_V << IO_MUX_GPIO38_FUN_IE_S)
#define IO_MUX_GPIO38_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO38_FUN_IE_S  9
/** IO_MUX_GPIO38_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO38_FUN_DRV    0x00000003U
#define IO_MUX_GPIO38_FUN_DRV_M  (IO_MUX_GPIO38_FUN_DRV_V << IO_MUX_GPIO38_FUN_DRV_S)
#define IO_MUX_GPIO38_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO38_FUN_DRV_S  10
/** IO_MUX_GPIO38_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO38_MCU_SEL    0x00000007U
#define IO_MUX_GPIO38_MCU_SEL_M  (IO_MUX_GPIO38_MCU_SEL_V << IO_MUX_GPIO38_MCU_SEL_S)
#define IO_MUX_GPIO38_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO38_MCU_SEL_S  12
/** IO_MUX_GPIO38_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO38_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO38_FILTER_EN_M  (IO_MUX_GPIO38_FILTER_EN_V << IO_MUX_GPIO38_FILTER_EN_S)
#define IO_MUX_GPIO38_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO38_FILTER_EN_S  15

/** IO_MUX_gpio39_REG register
 *  iomux control register for gpio39
 */
#define IO_MUX_GPIO39_REG (DR_REG_IO_MUX_BASE + 0xa0)
/** IO_MUX_GPIO39_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_OE    (BIT(0))
#define IO_MUX_GPIO39_MCU_OE_M  (IO_MUX_GPIO39_MCU_OE_V << IO_MUX_GPIO39_MCU_OE_S)
#define IO_MUX_GPIO39_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO39_MCU_OE_S  0
/** IO_MUX_GPIO39_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO39_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO39_SLP_SEL_M  (IO_MUX_GPIO39_SLP_SEL_V << IO_MUX_GPIO39_SLP_SEL_S)
#define IO_MUX_GPIO39_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO39_SLP_SEL_S  1
/** IO_MUX_GPIO39_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO39_MCU_WPD_M  (IO_MUX_GPIO39_MCU_WPD_V << IO_MUX_GPIO39_MCU_WPD_S)
#define IO_MUX_GPIO39_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO39_MCU_WPD_S  2
/** IO_MUX_GPIO39_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO39_MCU_WPU_M  (IO_MUX_GPIO39_MCU_WPU_V << IO_MUX_GPIO39_MCU_WPU_S)
#define IO_MUX_GPIO39_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO39_MCU_WPU_S  3
/** IO_MUX_GPIO39_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO39_MCU_IE    (BIT(4))
#define IO_MUX_GPIO39_MCU_IE_M  (IO_MUX_GPIO39_MCU_IE_V << IO_MUX_GPIO39_MCU_IE_S)
#define IO_MUX_GPIO39_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO39_MCU_IE_S  4
/** IO_MUX_GPIO39_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO39_MCU_DRV    0x00000003U
#define IO_MUX_GPIO39_MCU_DRV_M  (IO_MUX_GPIO39_MCU_DRV_V << IO_MUX_GPIO39_MCU_DRV_S)
#define IO_MUX_GPIO39_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO39_MCU_DRV_S  5
/** IO_MUX_GPIO39_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO39_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO39_FUN_WPD_M  (IO_MUX_GPIO39_FUN_WPD_V << IO_MUX_GPIO39_FUN_WPD_S)
#define IO_MUX_GPIO39_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO39_FUN_WPD_S  7
/** IO_MUX_GPIO39_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO39_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO39_FUN_WPU_M  (IO_MUX_GPIO39_FUN_WPU_V << IO_MUX_GPIO39_FUN_WPU_S)
#define IO_MUX_GPIO39_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO39_FUN_WPU_S  8
/** IO_MUX_GPIO39_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO39_FUN_IE    (BIT(9))
#define IO_MUX_GPIO39_FUN_IE_M  (IO_MUX_GPIO39_FUN_IE_V << IO_MUX_GPIO39_FUN_IE_S)
#define IO_MUX_GPIO39_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO39_FUN_IE_S  9
/** IO_MUX_GPIO39_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO39_FUN_DRV    0x00000003U
#define IO_MUX_GPIO39_FUN_DRV_M  (IO_MUX_GPIO39_FUN_DRV_V << IO_MUX_GPIO39_FUN_DRV_S)
#define IO_MUX_GPIO39_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO39_FUN_DRV_S  10
/** IO_MUX_GPIO39_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO39_MCU_SEL    0x00000007U
#define IO_MUX_GPIO39_MCU_SEL_M  (IO_MUX_GPIO39_MCU_SEL_V << IO_MUX_GPIO39_MCU_SEL_S)
#define IO_MUX_GPIO39_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO39_MCU_SEL_S  12
/** IO_MUX_GPIO39_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO39_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO39_FILTER_EN_M  (IO_MUX_GPIO39_FILTER_EN_V << IO_MUX_GPIO39_FILTER_EN_S)
#define IO_MUX_GPIO39_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO39_FILTER_EN_S  15

/** IO_MUX_gpio40_REG register
 *  iomux control register for gpio40
 */
#define IO_MUX_GPIO40_REG (DR_REG_IO_MUX_BASE + 0xa4)
/** IO_MUX_GPIO40_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_OE    (BIT(0))
#define IO_MUX_GPIO40_MCU_OE_M  (IO_MUX_GPIO40_MCU_OE_V << IO_MUX_GPIO40_MCU_OE_S)
#define IO_MUX_GPIO40_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO40_MCU_OE_S  0
/** IO_MUX_GPIO40_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO40_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO40_SLP_SEL_M  (IO_MUX_GPIO40_SLP_SEL_V << IO_MUX_GPIO40_SLP_SEL_S)
#define IO_MUX_GPIO40_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO40_SLP_SEL_S  1
/** IO_MUX_GPIO40_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO40_MCU_WPD_M  (IO_MUX_GPIO40_MCU_WPD_V << IO_MUX_GPIO40_MCU_WPD_S)
#define IO_MUX_GPIO40_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO40_MCU_WPD_S  2
/** IO_MUX_GPIO40_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO40_MCU_WPU_M  (IO_MUX_GPIO40_MCU_WPU_V << IO_MUX_GPIO40_MCU_WPU_S)
#define IO_MUX_GPIO40_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO40_MCU_WPU_S  3
/** IO_MUX_GPIO40_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO40_MCU_IE    (BIT(4))
#define IO_MUX_GPIO40_MCU_IE_M  (IO_MUX_GPIO40_MCU_IE_V << IO_MUX_GPIO40_MCU_IE_S)
#define IO_MUX_GPIO40_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO40_MCU_IE_S  4
/** IO_MUX_GPIO40_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO40_MCU_DRV    0x00000003U
#define IO_MUX_GPIO40_MCU_DRV_M  (IO_MUX_GPIO40_MCU_DRV_V << IO_MUX_GPIO40_MCU_DRV_S)
#define IO_MUX_GPIO40_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO40_MCU_DRV_S  5
/** IO_MUX_GPIO40_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO40_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO40_FUN_WPD_M  (IO_MUX_GPIO40_FUN_WPD_V << IO_MUX_GPIO40_FUN_WPD_S)
#define IO_MUX_GPIO40_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO40_FUN_WPD_S  7
/** IO_MUX_GPIO40_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO40_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO40_FUN_WPU_M  (IO_MUX_GPIO40_FUN_WPU_V << IO_MUX_GPIO40_FUN_WPU_S)
#define IO_MUX_GPIO40_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO40_FUN_WPU_S  8
/** IO_MUX_GPIO40_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO40_FUN_IE    (BIT(9))
#define IO_MUX_GPIO40_FUN_IE_M  (IO_MUX_GPIO40_FUN_IE_V << IO_MUX_GPIO40_FUN_IE_S)
#define IO_MUX_GPIO40_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO40_FUN_IE_S  9
/** IO_MUX_GPIO40_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO40_FUN_DRV    0x00000003U
#define IO_MUX_GPIO40_FUN_DRV_M  (IO_MUX_GPIO40_FUN_DRV_V << IO_MUX_GPIO40_FUN_DRV_S)
#define IO_MUX_GPIO40_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO40_FUN_DRV_S  10
/** IO_MUX_GPIO40_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO40_MCU_SEL    0x00000007U
#define IO_MUX_GPIO40_MCU_SEL_M  (IO_MUX_GPIO40_MCU_SEL_V << IO_MUX_GPIO40_MCU_SEL_S)
#define IO_MUX_GPIO40_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO40_MCU_SEL_S  12
/** IO_MUX_GPIO40_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO40_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO40_FILTER_EN_M  (IO_MUX_GPIO40_FILTER_EN_V << IO_MUX_GPIO40_FILTER_EN_S)
#define IO_MUX_GPIO40_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO40_FILTER_EN_S  15

/** IO_MUX_gpio41_REG register
 *  iomux control register for gpio41
 */
#define IO_MUX_GPIO41_REG (DR_REG_IO_MUX_BASE + 0xa8)
/** IO_MUX_GPIO41_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_OE    (BIT(0))
#define IO_MUX_GPIO41_MCU_OE_M  (IO_MUX_GPIO41_MCU_OE_V << IO_MUX_GPIO41_MCU_OE_S)
#define IO_MUX_GPIO41_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO41_MCU_OE_S  0
/** IO_MUX_GPIO41_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO41_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO41_SLP_SEL_M  (IO_MUX_GPIO41_SLP_SEL_V << IO_MUX_GPIO41_SLP_SEL_S)
#define IO_MUX_GPIO41_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO41_SLP_SEL_S  1
/** IO_MUX_GPIO41_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO41_MCU_WPD_M  (IO_MUX_GPIO41_MCU_WPD_V << IO_MUX_GPIO41_MCU_WPD_S)
#define IO_MUX_GPIO41_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO41_MCU_WPD_S  2
/** IO_MUX_GPIO41_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO41_MCU_WPU_M  (IO_MUX_GPIO41_MCU_WPU_V << IO_MUX_GPIO41_MCU_WPU_S)
#define IO_MUX_GPIO41_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO41_MCU_WPU_S  3
/** IO_MUX_GPIO41_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO41_MCU_IE    (BIT(4))
#define IO_MUX_GPIO41_MCU_IE_M  (IO_MUX_GPIO41_MCU_IE_V << IO_MUX_GPIO41_MCU_IE_S)
#define IO_MUX_GPIO41_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO41_MCU_IE_S  4
/** IO_MUX_GPIO41_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO41_MCU_DRV    0x00000003U
#define IO_MUX_GPIO41_MCU_DRV_M  (IO_MUX_GPIO41_MCU_DRV_V << IO_MUX_GPIO41_MCU_DRV_S)
#define IO_MUX_GPIO41_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO41_MCU_DRV_S  5
/** IO_MUX_GPIO41_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO41_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO41_FUN_WPD_M  (IO_MUX_GPIO41_FUN_WPD_V << IO_MUX_GPIO41_FUN_WPD_S)
#define IO_MUX_GPIO41_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO41_FUN_WPD_S  7
/** IO_MUX_GPIO41_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO41_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO41_FUN_WPU_M  (IO_MUX_GPIO41_FUN_WPU_V << IO_MUX_GPIO41_FUN_WPU_S)
#define IO_MUX_GPIO41_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO41_FUN_WPU_S  8
/** IO_MUX_GPIO41_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO41_FUN_IE    (BIT(9))
#define IO_MUX_GPIO41_FUN_IE_M  (IO_MUX_GPIO41_FUN_IE_V << IO_MUX_GPIO41_FUN_IE_S)
#define IO_MUX_GPIO41_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO41_FUN_IE_S  9
/** IO_MUX_GPIO41_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO41_FUN_DRV    0x00000003U
#define IO_MUX_GPIO41_FUN_DRV_M  (IO_MUX_GPIO41_FUN_DRV_V << IO_MUX_GPIO41_FUN_DRV_S)
#define IO_MUX_GPIO41_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO41_FUN_DRV_S  10
/** IO_MUX_GPIO41_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO41_MCU_SEL    0x00000007U
#define IO_MUX_GPIO41_MCU_SEL_M  (IO_MUX_GPIO41_MCU_SEL_V << IO_MUX_GPIO41_MCU_SEL_S)
#define IO_MUX_GPIO41_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO41_MCU_SEL_S  12
/** IO_MUX_GPIO41_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO41_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO41_FILTER_EN_M  (IO_MUX_GPIO41_FILTER_EN_V << IO_MUX_GPIO41_FILTER_EN_S)
#define IO_MUX_GPIO41_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO41_FILTER_EN_S  15

/** IO_MUX_gpio42_REG register
 *  iomux control register for gpio42
 */
#define IO_MUX_GPIO42_REG (DR_REG_IO_MUX_BASE + 0xac)
/** IO_MUX_GPIO42_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_OE    (BIT(0))
#define IO_MUX_GPIO42_MCU_OE_M  (IO_MUX_GPIO42_MCU_OE_V << IO_MUX_GPIO42_MCU_OE_S)
#define IO_MUX_GPIO42_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO42_MCU_OE_S  0
/** IO_MUX_GPIO42_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO42_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO42_SLP_SEL_M  (IO_MUX_GPIO42_SLP_SEL_V << IO_MUX_GPIO42_SLP_SEL_S)
#define IO_MUX_GPIO42_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO42_SLP_SEL_S  1
/** IO_MUX_GPIO42_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO42_MCU_WPD_M  (IO_MUX_GPIO42_MCU_WPD_V << IO_MUX_GPIO42_MCU_WPD_S)
#define IO_MUX_GPIO42_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO42_MCU_WPD_S  2
/** IO_MUX_GPIO42_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO42_MCU_WPU_M  (IO_MUX_GPIO42_MCU_WPU_V << IO_MUX_GPIO42_MCU_WPU_S)
#define IO_MUX_GPIO42_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO42_MCU_WPU_S  3
/** IO_MUX_GPIO42_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO42_MCU_IE    (BIT(4))
#define IO_MUX_GPIO42_MCU_IE_M  (IO_MUX_GPIO42_MCU_IE_V << IO_MUX_GPIO42_MCU_IE_S)
#define IO_MUX_GPIO42_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO42_MCU_IE_S  4
/** IO_MUX_GPIO42_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO42_MCU_DRV    0x00000003U
#define IO_MUX_GPIO42_MCU_DRV_M  (IO_MUX_GPIO42_MCU_DRV_V << IO_MUX_GPIO42_MCU_DRV_S)
#define IO_MUX_GPIO42_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO42_MCU_DRV_S  5
/** IO_MUX_GPIO42_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO42_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO42_FUN_WPD_M  (IO_MUX_GPIO42_FUN_WPD_V << IO_MUX_GPIO42_FUN_WPD_S)
#define IO_MUX_GPIO42_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO42_FUN_WPD_S  7
/** IO_MUX_GPIO42_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO42_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO42_FUN_WPU_M  (IO_MUX_GPIO42_FUN_WPU_V << IO_MUX_GPIO42_FUN_WPU_S)
#define IO_MUX_GPIO42_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO42_FUN_WPU_S  8
/** IO_MUX_GPIO42_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO42_FUN_IE    (BIT(9))
#define IO_MUX_GPIO42_FUN_IE_M  (IO_MUX_GPIO42_FUN_IE_V << IO_MUX_GPIO42_FUN_IE_S)
#define IO_MUX_GPIO42_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO42_FUN_IE_S  9
/** IO_MUX_GPIO42_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO42_FUN_DRV    0x00000003U
#define IO_MUX_GPIO42_FUN_DRV_M  (IO_MUX_GPIO42_FUN_DRV_V << IO_MUX_GPIO42_FUN_DRV_S)
#define IO_MUX_GPIO42_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO42_FUN_DRV_S  10
/** IO_MUX_GPIO42_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO42_MCU_SEL    0x00000007U
#define IO_MUX_GPIO42_MCU_SEL_M  (IO_MUX_GPIO42_MCU_SEL_V << IO_MUX_GPIO42_MCU_SEL_S)
#define IO_MUX_GPIO42_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO42_MCU_SEL_S  12
/** IO_MUX_GPIO42_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO42_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO42_FILTER_EN_M  (IO_MUX_GPIO42_FILTER_EN_V << IO_MUX_GPIO42_FILTER_EN_S)
#define IO_MUX_GPIO42_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO42_FILTER_EN_S  15

/** IO_MUX_gpio43_REG register
 *  iomux control register for gpio43
 */
#define IO_MUX_GPIO43_REG (DR_REG_IO_MUX_BASE + 0xb0)
/** IO_MUX_GPIO43_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_OE    (BIT(0))
#define IO_MUX_GPIO43_MCU_OE_M  (IO_MUX_GPIO43_MCU_OE_V << IO_MUX_GPIO43_MCU_OE_S)
#define IO_MUX_GPIO43_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO43_MCU_OE_S  0
/** IO_MUX_GPIO43_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO43_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO43_SLP_SEL_M  (IO_MUX_GPIO43_SLP_SEL_V << IO_MUX_GPIO43_SLP_SEL_S)
#define IO_MUX_GPIO43_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO43_SLP_SEL_S  1
/** IO_MUX_GPIO43_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO43_MCU_WPD_M  (IO_MUX_GPIO43_MCU_WPD_V << IO_MUX_GPIO43_MCU_WPD_S)
#define IO_MUX_GPIO43_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO43_MCU_WPD_S  2
/** IO_MUX_GPIO43_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO43_MCU_WPU_M  (IO_MUX_GPIO43_MCU_WPU_V << IO_MUX_GPIO43_MCU_WPU_S)
#define IO_MUX_GPIO43_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO43_MCU_WPU_S  3
/** IO_MUX_GPIO43_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO43_MCU_IE    (BIT(4))
#define IO_MUX_GPIO43_MCU_IE_M  (IO_MUX_GPIO43_MCU_IE_V << IO_MUX_GPIO43_MCU_IE_S)
#define IO_MUX_GPIO43_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO43_MCU_IE_S  4
/** IO_MUX_GPIO43_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO43_MCU_DRV    0x00000003U
#define IO_MUX_GPIO43_MCU_DRV_M  (IO_MUX_GPIO43_MCU_DRV_V << IO_MUX_GPIO43_MCU_DRV_S)
#define IO_MUX_GPIO43_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO43_MCU_DRV_S  5
/** IO_MUX_GPIO43_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO43_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO43_FUN_WPD_M  (IO_MUX_GPIO43_FUN_WPD_V << IO_MUX_GPIO43_FUN_WPD_S)
#define IO_MUX_GPIO43_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO43_FUN_WPD_S  7
/** IO_MUX_GPIO43_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO43_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO43_FUN_WPU_M  (IO_MUX_GPIO43_FUN_WPU_V << IO_MUX_GPIO43_FUN_WPU_S)
#define IO_MUX_GPIO43_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO43_FUN_WPU_S  8
/** IO_MUX_GPIO43_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO43_FUN_IE    (BIT(9))
#define IO_MUX_GPIO43_FUN_IE_M  (IO_MUX_GPIO43_FUN_IE_V << IO_MUX_GPIO43_FUN_IE_S)
#define IO_MUX_GPIO43_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO43_FUN_IE_S  9
/** IO_MUX_GPIO43_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO43_FUN_DRV    0x00000003U
#define IO_MUX_GPIO43_FUN_DRV_M  (IO_MUX_GPIO43_FUN_DRV_V << IO_MUX_GPIO43_FUN_DRV_S)
#define IO_MUX_GPIO43_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO43_FUN_DRV_S  10
/** IO_MUX_GPIO43_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO43_MCU_SEL    0x00000007U
#define IO_MUX_GPIO43_MCU_SEL_M  (IO_MUX_GPIO43_MCU_SEL_V << IO_MUX_GPIO43_MCU_SEL_S)
#define IO_MUX_GPIO43_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO43_MCU_SEL_S  12
/** IO_MUX_GPIO43_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO43_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO43_FILTER_EN_M  (IO_MUX_GPIO43_FILTER_EN_V << IO_MUX_GPIO43_FILTER_EN_S)
#define IO_MUX_GPIO43_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO43_FILTER_EN_S  15

/** IO_MUX_gpio44_REG register
 *  iomux control register for gpio44
 */
#define IO_MUX_GPIO44_REG (DR_REG_IO_MUX_BASE + 0xb4)
/** IO_MUX_GPIO44_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_OE    (BIT(0))
#define IO_MUX_GPIO44_MCU_OE_M  (IO_MUX_GPIO44_MCU_OE_V << IO_MUX_GPIO44_MCU_OE_S)
#define IO_MUX_GPIO44_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO44_MCU_OE_S  0
/** IO_MUX_GPIO44_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO44_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO44_SLP_SEL_M  (IO_MUX_GPIO44_SLP_SEL_V << IO_MUX_GPIO44_SLP_SEL_S)
#define IO_MUX_GPIO44_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO44_SLP_SEL_S  1
/** IO_MUX_GPIO44_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO44_MCU_WPD_M  (IO_MUX_GPIO44_MCU_WPD_V << IO_MUX_GPIO44_MCU_WPD_S)
#define IO_MUX_GPIO44_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO44_MCU_WPD_S  2
/** IO_MUX_GPIO44_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO44_MCU_WPU_M  (IO_MUX_GPIO44_MCU_WPU_V << IO_MUX_GPIO44_MCU_WPU_S)
#define IO_MUX_GPIO44_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO44_MCU_WPU_S  3
/** IO_MUX_GPIO44_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO44_MCU_IE    (BIT(4))
#define IO_MUX_GPIO44_MCU_IE_M  (IO_MUX_GPIO44_MCU_IE_V << IO_MUX_GPIO44_MCU_IE_S)
#define IO_MUX_GPIO44_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO44_MCU_IE_S  4
/** IO_MUX_GPIO44_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO44_MCU_DRV    0x00000003U
#define IO_MUX_GPIO44_MCU_DRV_M  (IO_MUX_GPIO44_MCU_DRV_V << IO_MUX_GPIO44_MCU_DRV_S)
#define IO_MUX_GPIO44_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO44_MCU_DRV_S  5
/** IO_MUX_GPIO44_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO44_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO44_FUN_WPD_M  (IO_MUX_GPIO44_FUN_WPD_V << IO_MUX_GPIO44_FUN_WPD_S)
#define IO_MUX_GPIO44_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO44_FUN_WPD_S  7
/** IO_MUX_GPIO44_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO44_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO44_FUN_WPU_M  (IO_MUX_GPIO44_FUN_WPU_V << IO_MUX_GPIO44_FUN_WPU_S)
#define IO_MUX_GPIO44_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO44_FUN_WPU_S  8
/** IO_MUX_GPIO44_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO44_FUN_IE    (BIT(9))
#define IO_MUX_GPIO44_FUN_IE_M  (IO_MUX_GPIO44_FUN_IE_V << IO_MUX_GPIO44_FUN_IE_S)
#define IO_MUX_GPIO44_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO44_FUN_IE_S  9
/** IO_MUX_GPIO44_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO44_FUN_DRV    0x00000003U
#define IO_MUX_GPIO44_FUN_DRV_M  (IO_MUX_GPIO44_FUN_DRV_V << IO_MUX_GPIO44_FUN_DRV_S)
#define IO_MUX_GPIO44_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO44_FUN_DRV_S  10
/** IO_MUX_GPIO44_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO44_MCU_SEL    0x00000007U
#define IO_MUX_GPIO44_MCU_SEL_M  (IO_MUX_GPIO44_MCU_SEL_V << IO_MUX_GPIO44_MCU_SEL_S)
#define IO_MUX_GPIO44_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO44_MCU_SEL_S  12
/** IO_MUX_GPIO44_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO44_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO44_FILTER_EN_M  (IO_MUX_GPIO44_FILTER_EN_V << IO_MUX_GPIO44_FILTER_EN_S)
#define IO_MUX_GPIO44_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO44_FILTER_EN_S  15

/** IO_MUX_gpio45_REG register
 *  iomux control register for gpio45
 */
#define IO_MUX_GPIO45_REG (DR_REG_IO_MUX_BASE + 0xb8)
/** IO_MUX_GPIO45_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_OE    (BIT(0))
#define IO_MUX_GPIO45_MCU_OE_M  (IO_MUX_GPIO45_MCU_OE_V << IO_MUX_GPIO45_MCU_OE_S)
#define IO_MUX_GPIO45_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO45_MCU_OE_S  0
/** IO_MUX_GPIO45_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO45_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO45_SLP_SEL_M  (IO_MUX_GPIO45_SLP_SEL_V << IO_MUX_GPIO45_SLP_SEL_S)
#define IO_MUX_GPIO45_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO45_SLP_SEL_S  1
/** IO_MUX_GPIO45_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO45_MCU_WPD_M  (IO_MUX_GPIO45_MCU_WPD_V << IO_MUX_GPIO45_MCU_WPD_S)
#define IO_MUX_GPIO45_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO45_MCU_WPD_S  2
/** IO_MUX_GPIO45_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO45_MCU_WPU_M  (IO_MUX_GPIO45_MCU_WPU_V << IO_MUX_GPIO45_MCU_WPU_S)
#define IO_MUX_GPIO45_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO45_MCU_WPU_S  3
/** IO_MUX_GPIO45_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO45_MCU_IE    (BIT(4))
#define IO_MUX_GPIO45_MCU_IE_M  (IO_MUX_GPIO45_MCU_IE_V << IO_MUX_GPIO45_MCU_IE_S)
#define IO_MUX_GPIO45_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO45_MCU_IE_S  4
/** IO_MUX_GPIO45_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO45_MCU_DRV    0x00000003U
#define IO_MUX_GPIO45_MCU_DRV_M  (IO_MUX_GPIO45_MCU_DRV_V << IO_MUX_GPIO45_MCU_DRV_S)
#define IO_MUX_GPIO45_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO45_MCU_DRV_S  5
/** IO_MUX_GPIO45_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO45_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO45_FUN_WPD_M  (IO_MUX_GPIO45_FUN_WPD_V << IO_MUX_GPIO45_FUN_WPD_S)
#define IO_MUX_GPIO45_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO45_FUN_WPD_S  7
/** IO_MUX_GPIO45_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO45_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO45_FUN_WPU_M  (IO_MUX_GPIO45_FUN_WPU_V << IO_MUX_GPIO45_FUN_WPU_S)
#define IO_MUX_GPIO45_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO45_FUN_WPU_S  8
/** IO_MUX_GPIO45_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO45_FUN_IE    (BIT(9))
#define IO_MUX_GPIO45_FUN_IE_M  (IO_MUX_GPIO45_FUN_IE_V << IO_MUX_GPIO45_FUN_IE_S)
#define IO_MUX_GPIO45_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO45_FUN_IE_S  9
/** IO_MUX_GPIO45_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO45_FUN_DRV    0x00000003U
#define IO_MUX_GPIO45_FUN_DRV_M  (IO_MUX_GPIO45_FUN_DRV_V << IO_MUX_GPIO45_FUN_DRV_S)
#define IO_MUX_GPIO45_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO45_FUN_DRV_S  10
/** IO_MUX_GPIO45_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO45_MCU_SEL    0x00000007U
#define IO_MUX_GPIO45_MCU_SEL_M  (IO_MUX_GPIO45_MCU_SEL_V << IO_MUX_GPIO45_MCU_SEL_S)
#define IO_MUX_GPIO45_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO45_MCU_SEL_S  12
/** IO_MUX_GPIO45_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO45_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO45_FILTER_EN_M  (IO_MUX_GPIO45_FILTER_EN_V << IO_MUX_GPIO45_FILTER_EN_S)
#define IO_MUX_GPIO45_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO45_FILTER_EN_S  15

/** IO_MUX_gpio46_REG register
 *  iomux control register for gpio46
 */
#define IO_MUX_GPIO46_REG (DR_REG_IO_MUX_BASE + 0xbc)
/** IO_MUX_GPIO46_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_OE    (BIT(0))
#define IO_MUX_GPIO46_MCU_OE_M  (IO_MUX_GPIO46_MCU_OE_V << IO_MUX_GPIO46_MCU_OE_S)
#define IO_MUX_GPIO46_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO46_MCU_OE_S  0
/** IO_MUX_GPIO46_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO46_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO46_SLP_SEL_M  (IO_MUX_GPIO46_SLP_SEL_V << IO_MUX_GPIO46_SLP_SEL_S)
#define IO_MUX_GPIO46_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO46_SLP_SEL_S  1
/** IO_MUX_GPIO46_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO46_MCU_WPD_M  (IO_MUX_GPIO46_MCU_WPD_V << IO_MUX_GPIO46_MCU_WPD_S)
#define IO_MUX_GPIO46_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO46_MCU_WPD_S  2
/** IO_MUX_GPIO46_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO46_MCU_WPU_M  (IO_MUX_GPIO46_MCU_WPU_V << IO_MUX_GPIO46_MCU_WPU_S)
#define IO_MUX_GPIO46_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO46_MCU_WPU_S  3
/** IO_MUX_GPIO46_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO46_MCU_IE    (BIT(4))
#define IO_MUX_GPIO46_MCU_IE_M  (IO_MUX_GPIO46_MCU_IE_V << IO_MUX_GPIO46_MCU_IE_S)
#define IO_MUX_GPIO46_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO46_MCU_IE_S  4
/** IO_MUX_GPIO46_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO46_MCU_DRV    0x00000003U
#define IO_MUX_GPIO46_MCU_DRV_M  (IO_MUX_GPIO46_MCU_DRV_V << IO_MUX_GPIO46_MCU_DRV_S)
#define IO_MUX_GPIO46_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO46_MCU_DRV_S  5
/** IO_MUX_GPIO46_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO46_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO46_FUN_WPD_M  (IO_MUX_GPIO46_FUN_WPD_V << IO_MUX_GPIO46_FUN_WPD_S)
#define IO_MUX_GPIO46_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO46_FUN_WPD_S  7
/** IO_MUX_GPIO46_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO46_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO46_FUN_WPU_M  (IO_MUX_GPIO46_FUN_WPU_V << IO_MUX_GPIO46_FUN_WPU_S)
#define IO_MUX_GPIO46_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO46_FUN_WPU_S  8
/** IO_MUX_GPIO46_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO46_FUN_IE    (BIT(9))
#define IO_MUX_GPIO46_FUN_IE_M  (IO_MUX_GPIO46_FUN_IE_V << IO_MUX_GPIO46_FUN_IE_S)
#define IO_MUX_GPIO46_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO46_FUN_IE_S  9
/** IO_MUX_GPIO46_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO46_FUN_DRV    0x00000003U
#define IO_MUX_GPIO46_FUN_DRV_M  (IO_MUX_GPIO46_FUN_DRV_V << IO_MUX_GPIO46_FUN_DRV_S)
#define IO_MUX_GPIO46_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO46_FUN_DRV_S  10
/** IO_MUX_GPIO46_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO46_MCU_SEL    0x00000007U
#define IO_MUX_GPIO46_MCU_SEL_M  (IO_MUX_GPIO46_MCU_SEL_V << IO_MUX_GPIO46_MCU_SEL_S)
#define IO_MUX_GPIO46_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO46_MCU_SEL_S  12
/** IO_MUX_GPIO46_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO46_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO46_FILTER_EN_M  (IO_MUX_GPIO46_FILTER_EN_V << IO_MUX_GPIO46_FILTER_EN_S)
#define IO_MUX_GPIO46_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO46_FILTER_EN_S  15

/** IO_MUX_gpio47_REG register
 *  iomux control register for gpio47
 */
#define IO_MUX_GPIO47_REG (DR_REG_IO_MUX_BASE + 0xc0)
/** IO_MUX_GPIO47_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_OE    (BIT(0))
#define IO_MUX_GPIO47_MCU_OE_M  (IO_MUX_GPIO47_MCU_OE_V << IO_MUX_GPIO47_MCU_OE_S)
#define IO_MUX_GPIO47_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO47_MCU_OE_S  0
/** IO_MUX_GPIO47_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO47_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO47_SLP_SEL_M  (IO_MUX_GPIO47_SLP_SEL_V << IO_MUX_GPIO47_SLP_SEL_S)
#define IO_MUX_GPIO47_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO47_SLP_SEL_S  1
/** IO_MUX_GPIO47_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO47_MCU_WPD_M  (IO_MUX_GPIO47_MCU_WPD_V << IO_MUX_GPIO47_MCU_WPD_S)
#define IO_MUX_GPIO47_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO47_MCU_WPD_S  2
/** IO_MUX_GPIO47_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO47_MCU_WPU_M  (IO_MUX_GPIO47_MCU_WPU_V << IO_MUX_GPIO47_MCU_WPU_S)
#define IO_MUX_GPIO47_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO47_MCU_WPU_S  3
/** IO_MUX_GPIO47_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO47_MCU_IE    (BIT(4))
#define IO_MUX_GPIO47_MCU_IE_M  (IO_MUX_GPIO47_MCU_IE_V << IO_MUX_GPIO47_MCU_IE_S)
#define IO_MUX_GPIO47_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO47_MCU_IE_S  4
/** IO_MUX_GPIO47_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO47_MCU_DRV    0x00000003U
#define IO_MUX_GPIO47_MCU_DRV_M  (IO_MUX_GPIO47_MCU_DRV_V << IO_MUX_GPIO47_MCU_DRV_S)
#define IO_MUX_GPIO47_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO47_MCU_DRV_S  5
/** IO_MUX_GPIO47_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO47_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO47_FUN_WPD_M  (IO_MUX_GPIO47_FUN_WPD_V << IO_MUX_GPIO47_FUN_WPD_S)
#define IO_MUX_GPIO47_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO47_FUN_WPD_S  7
/** IO_MUX_GPIO47_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO47_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO47_FUN_WPU_M  (IO_MUX_GPIO47_FUN_WPU_V << IO_MUX_GPIO47_FUN_WPU_S)
#define IO_MUX_GPIO47_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO47_FUN_WPU_S  8
/** IO_MUX_GPIO47_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO47_FUN_IE    (BIT(9))
#define IO_MUX_GPIO47_FUN_IE_M  (IO_MUX_GPIO47_FUN_IE_V << IO_MUX_GPIO47_FUN_IE_S)
#define IO_MUX_GPIO47_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO47_FUN_IE_S  9
/** IO_MUX_GPIO47_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO47_FUN_DRV    0x00000003U
#define IO_MUX_GPIO47_FUN_DRV_M  (IO_MUX_GPIO47_FUN_DRV_V << IO_MUX_GPIO47_FUN_DRV_S)
#define IO_MUX_GPIO47_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO47_FUN_DRV_S  10
/** IO_MUX_GPIO47_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO47_MCU_SEL    0x00000007U
#define IO_MUX_GPIO47_MCU_SEL_M  (IO_MUX_GPIO47_MCU_SEL_V << IO_MUX_GPIO47_MCU_SEL_S)
#define IO_MUX_GPIO47_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO47_MCU_SEL_S  12
/** IO_MUX_GPIO47_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO47_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO47_FILTER_EN_M  (IO_MUX_GPIO47_FILTER_EN_V << IO_MUX_GPIO47_FILTER_EN_S)
#define IO_MUX_GPIO47_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO47_FILTER_EN_S  15

/** IO_MUX_gpio48_REG register
 *  iomux control register for gpio48
 */
#define IO_MUX_GPIO48_REG (DR_REG_IO_MUX_BASE + 0xc4)
/** IO_MUX_GPIO48_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_OE    (BIT(0))
#define IO_MUX_GPIO48_MCU_OE_M  (IO_MUX_GPIO48_MCU_OE_V << IO_MUX_GPIO48_MCU_OE_S)
#define IO_MUX_GPIO48_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO48_MCU_OE_S  0
/** IO_MUX_GPIO48_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO48_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO48_SLP_SEL_M  (IO_MUX_GPIO48_SLP_SEL_V << IO_MUX_GPIO48_SLP_SEL_S)
#define IO_MUX_GPIO48_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO48_SLP_SEL_S  1
/** IO_MUX_GPIO48_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO48_MCU_WPD_M  (IO_MUX_GPIO48_MCU_WPD_V << IO_MUX_GPIO48_MCU_WPD_S)
#define IO_MUX_GPIO48_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO48_MCU_WPD_S  2
/** IO_MUX_GPIO48_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO48_MCU_WPU_M  (IO_MUX_GPIO48_MCU_WPU_V << IO_MUX_GPIO48_MCU_WPU_S)
#define IO_MUX_GPIO48_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO48_MCU_WPU_S  3
/** IO_MUX_GPIO48_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO48_MCU_IE    (BIT(4))
#define IO_MUX_GPIO48_MCU_IE_M  (IO_MUX_GPIO48_MCU_IE_V << IO_MUX_GPIO48_MCU_IE_S)
#define IO_MUX_GPIO48_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO48_MCU_IE_S  4
/** IO_MUX_GPIO48_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO48_MCU_DRV    0x00000003U
#define IO_MUX_GPIO48_MCU_DRV_M  (IO_MUX_GPIO48_MCU_DRV_V << IO_MUX_GPIO48_MCU_DRV_S)
#define IO_MUX_GPIO48_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO48_MCU_DRV_S  5
/** IO_MUX_GPIO48_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO48_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO48_FUN_WPD_M  (IO_MUX_GPIO48_FUN_WPD_V << IO_MUX_GPIO48_FUN_WPD_S)
#define IO_MUX_GPIO48_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO48_FUN_WPD_S  7
/** IO_MUX_GPIO48_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO48_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO48_FUN_WPU_M  (IO_MUX_GPIO48_FUN_WPU_V << IO_MUX_GPIO48_FUN_WPU_S)
#define IO_MUX_GPIO48_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO48_FUN_WPU_S  8
/** IO_MUX_GPIO48_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO48_FUN_IE    (BIT(9))
#define IO_MUX_GPIO48_FUN_IE_M  (IO_MUX_GPIO48_FUN_IE_V << IO_MUX_GPIO48_FUN_IE_S)
#define IO_MUX_GPIO48_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO48_FUN_IE_S  9
/** IO_MUX_GPIO48_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO48_FUN_DRV    0x00000003U
#define IO_MUX_GPIO48_FUN_DRV_M  (IO_MUX_GPIO48_FUN_DRV_V << IO_MUX_GPIO48_FUN_DRV_S)
#define IO_MUX_GPIO48_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO48_FUN_DRV_S  10
/** IO_MUX_GPIO48_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO48_MCU_SEL    0x00000007U
#define IO_MUX_GPIO48_MCU_SEL_M  (IO_MUX_GPIO48_MCU_SEL_V << IO_MUX_GPIO48_MCU_SEL_S)
#define IO_MUX_GPIO48_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO48_MCU_SEL_S  12
/** IO_MUX_GPIO48_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO48_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO48_FILTER_EN_M  (IO_MUX_GPIO48_FILTER_EN_V << IO_MUX_GPIO48_FILTER_EN_S)
#define IO_MUX_GPIO48_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO48_FILTER_EN_S  15

/** IO_MUX_gpio49_REG register
 *  iomux control register for gpio49
 */
#define IO_MUX_GPIO49_REG (DR_REG_IO_MUX_BASE + 0xc8)
/** IO_MUX_GPIO49_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_OE    (BIT(0))
#define IO_MUX_GPIO49_MCU_OE_M  (IO_MUX_GPIO49_MCU_OE_V << IO_MUX_GPIO49_MCU_OE_S)
#define IO_MUX_GPIO49_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO49_MCU_OE_S  0
/** IO_MUX_GPIO49_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO49_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO49_SLP_SEL_M  (IO_MUX_GPIO49_SLP_SEL_V << IO_MUX_GPIO49_SLP_SEL_S)
#define IO_MUX_GPIO49_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO49_SLP_SEL_S  1
/** IO_MUX_GPIO49_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO49_MCU_WPD_M  (IO_MUX_GPIO49_MCU_WPD_V << IO_MUX_GPIO49_MCU_WPD_S)
#define IO_MUX_GPIO49_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO49_MCU_WPD_S  2
/** IO_MUX_GPIO49_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO49_MCU_WPU_M  (IO_MUX_GPIO49_MCU_WPU_V << IO_MUX_GPIO49_MCU_WPU_S)
#define IO_MUX_GPIO49_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO49_MCU_WPU_S  3
/** IO_MUX_GPIO49_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO49_MCU_IE    (BIT(4))
#define IO_MUX_GPIO49_MCU_IE_M  (IO_MUX_GPIO49_MCU_IE_V << IO_MUX_GPIO49_MCU_IE_S)
#define IO_MUX_GPIO49_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO49_MCU_IE_S  4
/** IO_MUX_GPIO49_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO49_MCU_DRV    0x00000003U
#define IO_MUX_GPIO49_MCU_DRV_M  (IO_MUX_GPIO49_MCU_DRV_V << IO_MUX_GPIO49_MCU_DRV_S)
#define IO_MUX_GPIO49_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO49_MCU_DRV_S  5
/** IO_MUX_GPIO49_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO49_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO49_FUN_WPD_M  (IO_MUX_GPIO49_FUN_WPD_V << IO_MUX_GPIO49_FUN_WPD_S)
#define IO_MUX_GPIO49_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO49_FUN_WPD_S  7
/** IO_MUX_GPIO49_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO49_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO49_FUN_WPU_M  (IO_MUX_GPIO49_FUN_WPU_V << IO_MUX_GPIO49_FUN_WPU_S)
#define IO_MUX_GPIO49_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO49_FUN_WPU_S  8
/** IO_MUX_GPIO49_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO49_FUN_IE    (BIT(9))
#define IO_MUX_GPIO49_FUN_IE_M  (IO_MUX_GPIO49_FUN_IE_V << IO_MUX_GPIO49_FUN_IE_S)
#define IO_MUX_GPIO49_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO49_FUN_IE_S  9
/** IO_MUX_GPIO49_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO49_FUN_DRV    0x00000003U
#define IO_MUX_GPIO49_FUN_DRV_M  (IO_MUX_GPIO49_FUN_DRV_V << IO_MUX_GPIO49_FUN_DRV_S)
#define IO_MUX_GPIO49_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO49_FUN_DRV_S  10
/** IO_MUX_GPIO49_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO49_MCU_SEL    0x00000007U
#define IO_MUX_GPIO49_MCU_SEL_M  (IO_MUX_GPIO49_MCU_SEL_V << IO_MUX_GPIO49_MCU_SEL_S)
#define IO_MUX_GPIO49_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO49_MCU_SEL_S  12
/** IO_MUX_GPIO49_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO49_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO49_FILTER_EN_M  (IO_MUX_GPIO49_FILTER_EN_V << IO_MUX_GPIO49_FILTER_EN_S)
#define IO_MUX_GPIO49_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO49_FILTER_EN_S  15

/** IO_MUX_gpio50_REG register
 *  iomux control register for gpio50
 */
#define IO_MUX_GPIO50_REG (DR_REG_IO_MUX_BASE + 0xcc)
/** IO_MUX_GPIO50_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_OE    (BIT(0))
#define IO_MUX_GPIO50_MCU_OE_M  (IO_MUX_GPIO50_MCU_OE_V << IO_MUX_GPIO50_MCU_OE_S)
#define IO_MUX_GPIO50_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO50_MCU_OE_S  0
/** IO_MUX_GPIO50_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO50_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO50_SLP_SEL_M  (IO_MUX_GPIO50_SLP_SEL_V << IO_MUX_GPIO50_SLP_SEL_S)
#define IO_MUX_GPIO50_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO50_SLP_SEL_S  1
/** IO_MUX_GPIO50_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO50_MCU_WPD_M  (IO_MUX_GPIO50_MCU_WPD_V << IO_MUX_GPIO50_MCU_WPD_S)
#define IO_MUX_GPIO50_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO50_MCU_WPD_S  2
/** IO_MUX_GPIO50_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO50_MCU_WPU_M  (IO_MUX_GPIO50_MCU_WPU_V << IO_MUX_GPIO50_MCU_WPU_S)
#define IO_MUX_GPIO50_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO50_MCU_WPU_S  3
/** IO_MUX_GPIO50_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO50_MCU_IE    (BIT(4))
#define IO_MUX_GPIO50_MCU_IE_M  (IO_MUX_GPIO50_MCU_IE_V << IO_MUX_GPIO50_MCU_IE_S)
#define IO_MUX_GPIO50_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO50_MCU_IE_S  4
/** IO_MUX_GPIO50_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO50_MCU_DRV    0x00000003U
#define IO_MUX_GPIO50_MCU_DRV_M  (IO_MUX_GPIO50_MCU_DRV_V << IO_MUX_GPIO50_MCU_DRV_S)
#define IO_MUX_GPIO50_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO50_MCU_DRV_S  5
/** IO_MUX_GPIO50_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO50_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO50_FUN_WPD_M  (IO_MUX_GPIO50_FUN_WPD_V << IO_MUX_GPIO50_FUN_WPD_S)
#define IO_MUX_GPIO50_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO50_FUN_WPD_S  7
/** IO_MUX_GPIO50_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO50_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO50_FUN_WPU_M  (IO_MUX_GPIO50_FUN_WPU_V << IO_MUX_GPIO50_FUN_WPU_S)
#define IO_MUX_GPIO50_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO50_FUN_WPU_S  8
/** IO_MUX_GPIO50_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO50_FUN_IE    (BIT(9))
#define IO_MUX_GPIO50_FUN_IE_M  (IO_MUX_GPIO50_FUN_IE_V << IO_MUX_GPIO50_FUN_IE_S)
#define IO_MUX_GPIO50_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO50_FUN_IE_S  9
/** IO_MUX_GPIO50_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO50_FUN_DRV    0x00000003U
#define IO_MUX_GPIO50_FUN_DRV_M  (IO_MUX_GPIO50_FUN_DRV_V << IO_MUX_GPIO50_FUN_DRV_S)
#define IO_MUX_GPIO50_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO50_FUN_DRV_S  10
/** IO_MUX_GPIO50_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO50_MCU_SEL    0x00000007U
#define IO_MUX_GPIO50_MCU_SEL_M  (IO_MUX_GPIO50_MCU_SEL_V << IO_MUX_GPIO50_MCU_SEL_S)
#define IO_MUX_GPIO50_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO50_MCU_SEL_S  12
/** IO_MUX_GPIO50_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO50_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO50_FILTER_EN_M  (IO_MUX_GPIO50_FILTER_EN_V << IO_MUX_GPIO50_FILTER_EN_S)
#define IO_MUX_GPIO50_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO50_FILTER_EN_S  15

/** IO_MUX_gpio51_REG register
 *  iomux control register for gpio51
 */
#define IO_MUX_GPIO51_REG (DR_REG_IO_MUX_BASE + 0xd0)
/** IO_MUX_GPIO51_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_OE    (BIT(0))
#define IO_MUX_GPIO51_MCU_OE_M  (IO_MUX_GPIO51_MCU_OE_V << IO_MUX_GPIO51_MCU_OE_S)
#define IO_MUX_GPIO51_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO51_MCU_OE_S  0
/** IO_MUX_GPIO51_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO51_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO51_SLP_SEL_M  (IO_MUX_GPIO51_SLP_SEL_V << IO_MUX_GPIO51_SLP_SEL_S)
#define IO_MUX_GPIO51_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO51_SLP_SEL_S  1
/** IO_MUX_GPIO51_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO51_MCU_WPD_M  (IO_MUX_GPIO51_MCU_WPD_V << IO_MUX_GPIO51_MCU_WPD_S)
#define IO_MUX_GPIO51_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO51_MCU_WPD_S  2
/** IO_MUX_GPIO51_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO51_MCU_WPU_M  (IO_MUX_GPIO51_MCU_WPU_V << IO_MUX_GPIO51_MCU_WPU_S)
#define IO_MUX_GPIO51_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO51_MCU_WPU_S  3
/** IO_MUX_GPIO51_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO51_MCU_IE    (BIT(4))
#define IO_MUX_GPIO51_MCU_IE_M  (IO_MUX_GPIO51_MCU_IE_V << IO_MUX_GPIO51_MCU_IE_S)
#define IO_MUX_GPIO51_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO51_MCU_IE_S  4
/** IO_MUX_GPIO51_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO51_MCU_DRV    0x00000003U
#define IO_MUX_GPIO51_MCU_DRV_M  (IO_MUX_GPIO51_MCU_DRV_V << IO_MUX_GPIO51_MCU_DRV_S)
#define IO_MUX_GPIO51_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO51_MCU_DRV_S  5
/** IO_MUX_GPIO51_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO51_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO51_FUN_WPD_M  (IO_MUX_GPIO51_FUN_WPD_V << IO_MUX_GPIO51_FUN_WPD_S)
#define IO_MUX_GPIO51_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO51_FUN_WPD_S  7
/** IO_MUX_GPIO51_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO51_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO51_FUN_WPU_M  (IO_MUX_GPIO51_FUN_WPU_V << IO_MUX_GPIO51_FUN_WPU_S)
#define IO_MUX_GPIO51_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO51_FUN_WPU_S  8
/** IO_MUX_GPIO51_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO51_FUN_IE    (BIT(9))
#define IO_MUX_GPIO51_FUN_IE_M  (IO_MUX_GPIO51_FUN_IE_V << IO_MUX_GPIO51_FUN_IE_S)
#define IO_MUX_GPIO51_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO51_FUN_IE_S  9
/** IO_MUX_GPIO51_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO51_FUN_DRV    0x00000003U
#define IO_MUX_GPIO51_FUN_DRV_M  (IO_MUX_GPIO51_FUN_DRV_V << IO_MUX_GPIO51_FUN_DRV_S)
#define IO_MUX_GPIO51_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO51_FUN_DRV_S  10
/** IO_MUX_GPIO51_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO51_MCU_SEL    0x00000007U
#define IO_MUX_GPIO51_MCU_SEL_M  (IO_MUX_GPIO51_MCU_SEL_V << IO_MUX_GPIO51_MCU_SEL_S)
#define IO_MUX_GPIO51_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO51_MCU_SEL_S  12
/** IO_MUX_GPIO51_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO51_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO51_FILTER_EN_M  (IO_MUX_GPIO51_FILTER_EN_V << IO_MUX_GPIO51_FILTER_EN_S)
#define IO_MUX_GPIO51_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO51_FILTER_EN_S  15

/** IO_MUX_gpio52_REG register
 *  iomux control register for gpio52
 */
#define IO_MUX_GPIO52_REG (DR_REG_IO_MUX_BASE + 0xd4)
/** IO_MUX_GPIO52_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_OE    (BIT(0))
#define IO_MUX_GPIO52_MCU_OE_M  (IO_MUX_GPIO52_MCU_OE_V << IO_MUX_GPIO52_MCU_OE_S)
#define IO_MUX_GPIO52_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO52_MCU_OE_S  0
/** IO_MUX_GPIO52_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO52_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO52_SLP_SEL_M  (IO_MUX_GPIO52_SLP_SEL_V << IO_MUX_GPIO52_SLP_SEL_S)
#define IO_MUX_GPIO52_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO52_SLP_SEL_S  1
/** IO_MUX_GPIO52_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO52_MCU_WPD_M  (IO_MUX_GPIO52_MCU_WPD_V << IO_MUX_GPIO52_MCU_WPD_S)
#define IO_MUX_GPIO52_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO52_MCU_WPD_S  2
/** IO_MUX_GPIO52_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO52_MCU_WPU_M  (IO_MUX_GPIO52_MCU_WPU_V << IO_MUX_GPIO52_MCU_WPU_S)
#define IO_MUX_GPIO52_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO52_MCU_WPU_S  3
/** IO_MUX_GPIO52_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO52_MCU_IE    (BIT(4))
#define IO_MUX_GPIO52_MCU_IE_M  (IO_MUX_GPIO52_MCU_IE_V << IO_MUX_GPIO52_MCU_IE_S)
#define IO_MUX_GPIO52_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO52_MCU_IE_S  4
/** IO_MUX_GPIO52_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO52_MCU_DRV    0x00000003U
#define IO_MUX_GPIO52_MCU_DRV_M  (IO_MUX_GPIO52_MCU_DRV_V << IO_MUX_GPIO52_MCU_DRV_S)
#define IO_MUX_GPIO52_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO52_MCU_DRV_S  5
/** IO_MUX_GPIO52_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO52_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO52_FUN_WPD_M  (IO_MUX_GPIO52_FUN_WPD_V << IO_MUX_GPIO52_FUN_WPD_S)
#define IO_MUX_GPIO52_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO52_FUN_WPD_S  7
/** IO_MUX_GPIO52_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO52_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO52_FUN_WPU_M  (IO_MUX_GPIO52_FUN_WPU_V << IO_MUX_GPIO52_FUN_WPU_S)
#define IO_MUX_GPIO52_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO52_FUN_WPU_S  8
/** IO_MUX_GPIO52_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO52_FUN_IE    (BIT(9))
#define IO_MUX_GPIO52_FUN_IE_M  (IO_MUX_GPIO52_FUN_IE_V << IO_MUX_GPIO52_FUN_IE_S)
#define IO_MUX_GPIO52_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO52_FUN_IE_S  9
/** IO_MUX_GPIO52_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO52_FUN_DRV    0x00000003U
#define IO_MUX_GPIO52_FUN_DRV_M  (IO_MUX_GPIO52_FUN_DRV_V << IO_MUX_GPIO52_FUN_DRV_S)
#define IO_MUX_GPIO52_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO52_FUN_DRV_S  10
/** IO_MUX_GPIO52_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO52_MCU_SEL    0x00000007U
#define IO_MUX_GPIO52_MCU_SEL_M  (IO_MUX_GPIO52_MCU_SEL_V << IO_MUX_GPIO52_MCU_SEL_S)
#define IO_MUX_GPIO52_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO52_MCU_SEL_S  12
/** IO_MUX_GPIO52_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO52_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO52_FILTER_EN_M  (IO_MUX_GPIO52_FILTER_EN_V << IO_MUX_GPIO52_FILTER_EN_S)
#define IO_MUX_GPIO52_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO52_FILTER_EN_S  15

/** IO_MUX_gpio53_REG register
 *  iomux control register for gpio53
 */
#define IO_MUX_GPIO53_REG (DR_REG_IO_MUX_BASE + 0xd8)
/** IO_MUX_GPIO53_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_OE    (BIT(0))
#define IO_MUX_GPIO53_MCU_OE_M  (IO_MUX_GPIO53_MCU_OE_V << IO_MUX_GPIO53_MCU_OE_S)
#define IO_MUX_GPIO53_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO53_MCU_OE_S  0
/** IO_MUX_GPIO53_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO53_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO53_SLP_SEL_M  (IO_MUX_GPIO53_SLP_SEL_V << IO_MUX_GPIO53_SLP_SEL_S)
#define IO_MUX_GPIO53_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO53_SLP_SEL_S  1
/** IO_MUX_GPIO53_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO53_MCU_WPD_M  (IO_MUX_GPIO53_MCU_WPD_V << IO_MUX_GPIO53_MCU_WPD_S)
#define IO_MUX_GPIO53_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO53_MCU_WPD_S  2
/** IO_MUX_GPIO53_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO53_MCU_WPU_M  (IO_MUX_GPIO53_MCU_WPU_V << IO_MUX_GPIO53_MCU_WPU_S)
#define IO_MUX_GPIO53_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO53_MCU_WPU_S  3
/** IO_MUX_GPIO53_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO53_MCU_IE    (BIT(4))
#define IO_MUX_GPIO53_MCU_IE_M  (IO_MUX_GPIO53_MCU_IE_V << IO_MUX_GPIO53_MCU_IE_S)
#define IO_MUX_GPIO53_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO53_MCU_IE_S  4
/** IO_MUX_GPIO53_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO53_MCU_DRV    0x00000003U
#define IO_MUX_GPIO53_MCU_DRV_M  (IO_MUX_GPIO53_MCU_DRV_V << IO_MUX_GPIO53_MCU_DRV_S)
#define IO_MUX_GPIO53_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO53_MCU_DRV_S  5
/** IO_MUX_GPIO53_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO53_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO53_FUN_WPD_M  (IO_MUX_GPIO53_FUN_WPD_V << IO_MUX_GPIO53_FUN_WPD_S)
#define IO_MUX_GPIO53_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO53_FUN_WPD_S  7
/** IO_MUX_GPIO53_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO53_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO53_FUN_WPU_M  (IO_MUX_GPIO53_FUN_WPU_V << IO_MUX_GPIO53_FUN_WPU_S)
#define IO_MUX_GPIO53_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO53_FUN_WPU_S  8
/** IO_MUX_GPIO53_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO53_FUN_IE    (BIT(9))
#define IO_MUX_GPIO53_FUN_IE_M  (IO_MUX_GPIO53_FUN_IE_V << IO_MUX_GPIO53_FUN_IE_S)
#define IO_MUX_GPIO53_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO53_FUN_IE_S  9
/** IO_MUX_GPIO53_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO53_FUN_DRV    0x00000003U
#define IO_MUX_GPIO53_FUN_DRV_M  (IO_MUX_GPIO53_FUN_DRV_V << IO_MUX_GPIO53_FUN_DRV_S)
#define IO_MUX_GPIO53_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO53_FUN_DRV_S  10
/** IO_MUX_GPIO53_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO53_MCU_SEL    0x00000007U
#define IO_MUX_GPIO53_MCU_SEL_M  (IO_MUX_GPIO53_MCU_SEL_V << IO_MUX_GPIO53_MCU_SEL_S)
#define IO_MUX_GPIO53_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO53_MCU_SEL_S  12
/** IO_MUX_GPIO53_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO53_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO53_FILTER_EN_M  (IO_MUX_GPIO53_FILTER_EN_V << IO_MUX_GPIO53_FILTER_EN_S)
#define IO_MUX_GPIO53_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO53_FILTER_EN_S  15

/** IO_MUX_gpio54_REG register
 *  iomux control register for gpio54
 */
#define IO_MUX_GPIO54_REG (DR_REG_IO_MUX_BASE + 0xdc)
/** IO_MUX_GPIO54_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_OE    (BIT(0))
#define IO_MUX_GPIO54_MCU_OE_M  (IO_MUX_GPIO54_MCU_OE_V << IO_MUX_GPIO54_MCU_OE_S)
#define IO_MUX_GPIO54_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO54_MCU_OE_S  0
/** IO_MUX_GPIO54_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO54_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO54_SLP_SEL_M  (IO_MUX_GPIO54_SLP_SEL_V << IO_MUX_GPIO54_SLP_SEL_S)
#define IO_MUX_GPIO54_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO54_SLP_SEL_S  1
/** IO_MUX_GPIO54_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO54_MCU_WPD_M  (IO_MUX_GPIO54_MCU_WPD_V << IO_MUX_GPIO54_MCU_WPD_S)
#define IO_MUX_GPIO54_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO54_MCU_WPD_S  2
/** IO_MUX_GPIO54_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO54_MCU_WPU_M  (IO_MUX_GPIO54_MCU_WPU_V << IO_MUX_GPIO54_MCU_WPU_S)
#define IO_MUX_GPIO54_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO54_MCU_WPU_S  3
/** IO_MUX_GPIO54_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO54_MCU_IE    (BIT(4))
#define IO_MUX_GPIO54_MCU_IE_M  (IO_MUX_GPIO54_MCU_IE_V << IO_MUX_GPIO54_MCU_IE_S)
#define IO_MUX_GPIO54_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO54_MCU_IE_S  4
/** IO_MUX_GPIO54_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO54_MCU_DRV    0x00000003U
#define IO_MUX_GPIO54_MCU_DRV_M  (IO_MUX_GPIO54_MCU_DRV_V << IO_MUX_GPIO54_MCU_DRV_S)
#define IO_MUX_GPIO54_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO54_MCU_DRV_S  5
/** IO_MUX_GPIO54_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO54_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO54_FUN_WPD_M  (IO_MUX_GPIO54_FUN_WPD_V << IO_MUX_GPIO54_FUN_WPD_S)
#define IO_MUX_GPIO54_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO54_FUN_WPD_S  7
/** IO_MUX_GPIO54_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO54_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO54_FUN_WPU_M  (IO_MUX_GPIO54_FUN_WPU_V << IO_MUX_GPIO54_FUN_WPU_S)
#define IO_MUX_GPIO54_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO54_FUN_WPU_S  8
/** IO_MUX_GPIO54_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO54_FUN_IE    (BIT(9))
#define IO_MUX_GPIO54_FUN_IE_M  (IO_MUX_GPIO54_FUN_IE_V << IO_MUX_GPIO54_FUN_IE_S)
#define IO_MUX_GPIO54_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO54_FUN_IE_S  9
/** IO_MUX_GPIO54_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO54_FUN_DRV    0x00000003U
#define IO_MUX_GPIO54_FUN_DRV_M  (IO_MUX_GPIO54_FUN_DRV_V << IO_MUX_GPIO54_FUN_DRV_S)
#define IO_MUX_GPIO54_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO54_FUN_DRV_S  10
/** IO_MUX_GPIO54_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO54_MCU_SEL    0x00000007U
#define IO_MUX_GPIO54_MCU_SEL_M  (IO_MUX_GPIO54_MCU_SEL_V << IO_MUX_GPIO54_MCU_SEL_S)
#define IO_MUX_GPIO54_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO54_MCU_SEL_S  12
/** IO_MUX_GPIO54_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO54_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO54_FILTER_EN_M  (IO_MUX_GPIO54_FILTER_EN_V << IO_MUX_GPIO54_FILTER_EN_S)
#define IO_MUX_GPIO54_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO54_FILTER_EN_S  15

/** IO_MUX_gpio55_REG register
 *  iomux control register for gpio55
 */
#define IO_MUX_GPIO55_REG (DR_REG_IO_MUX_BASE + 0xe0)
/** IO_MUX_GPIO55_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_OE    (BIT(0))
#define IO_MUX_GPIO55_MCU_OE_M  (IO_MUX_GPIO55_MCU_OE_V << IO_MUX_GPIO55_MCU_OE_S)
#define IO_MUX_GPIO55_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO55_MCU_OE_S  0
/** IO_MUX_GPIO55_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO55_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO55_SLP_SEL_M  (IO_MUX_GPIO55_SLP_SEL_V << IO_MUX_GPIO55_SLP_SEL_S)
#define IO_MUX_GPIO55_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO55_SLP_SEL_S  1
/** IO_MUX_GPIO55_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO55_MCU_WPD_M  (IO_MUX_GPIO55_MCU_WPD_V << IO_MUX_GPIO55_MCU_WPD_S)
#define IO_MUX_GPIO55_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO55_MCU_WPD_S  2
/** IO_MUX_GPIO55_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO55_MCU_WPU_M  (IO_MUX_GPIO55_MCU_WPU_V << IO_MUX_GPIO55_MCU_WPU_S)
#define IO_MUX_GPIO55_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO55_MCU_WPU_S  3
/** IO_MUX_GPIO55_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO55_MCU_IE    (BIT(4))
#define IO_MUX_GPIO55_MCU_IE_M  (IO_MUX_GPIO55_MCU_IE_V << IO_MUX_GPIO55_MCU_IE_S)
#define IO_MUX_GPIO55_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO55_MCU_IE_S  4
/** IO_MUX_GPIO55_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO55_MCU_DRV    0x00000003U
#define IO_MUX_GPIO55_MCU_DRV_M  (IO_MUX_GPIO55_MCU_DRV_V << IO_MUX_GPIO55_MCU_DRV_S)
#define IO_MUX_GPIO55_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO55_MCU_DRV_S  5
/** IO_MUX_GPIO55_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO55_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO55_FUN_WPD_M  (IO_MUX_GPIO55_FUN_WPD_V << IO_MUX_GPIO55_FUN_WPD_S)
#define IO_MUX_GPIO55_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO55_FUN_WPD_S  7
/** IO_MUX_GPIO55_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO55_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO55_FUN_WPU_M  (IO_MUX_GPIO55_FUN_WPU_V << IO_MUX_GPIO55_FUN_WPU_S)
#define IO_MUX_GPIO55_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO55_FUN_WPU_S  8
/** IO_MUX_GPIO55_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO55_FUN_IE    (BIT(9))
#define IO_MUX_GPIO55_FUN_IE_M  (IO_MUX_GPIO55_FUN_IE_V << IO_MUX_GPIO55_FUN_IE_S)
#define IO_MUX_GPIO55_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO55_FUN_IE_S  9
/** IO_MUX_GPIO55_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO55_FUN_DRV    0x00000003U
#define IO_MUX_GPIO55_FUN_DRV_M  (IO_MUX_GPIO55_FUN_DRV_V << IO_MUX_GPIO55_FUN_DRV_S)
#define IO_MUX_GPIO55_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO55_FUN_DRV_S  10
/** IO_MUX_GPIO55_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO55_MCU_SEL    0x00000007U
#define IO_MUX_GPIO55_MCU_SEL_M  (IO_MUX_GPIO55_MCU_SEL_V << IO_MUX_GPIO55_MCU_SEL_S)
#define IO_MUX_GPIO55_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO55_MCU_SEL_S  12
/** IO_MUX_GPIO55_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO55_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO55_FILTER_EN_M  (IO_MUX_GPIO55_FILTER_EN_V << IO_MUX_GPIO55_FILTER_EN_S)
#define IO_MUX_GPIO55_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO55_FILTER_EN_S  15

/** IO_MUX_gpio56_REG register
 *  iomux control register for gpio56
 */
#define IO_MUX_GPIO56_REG (DR_REG_IO_MUX_BASE + 0xe4)
/** IO_MUX_GPIO56_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  output enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_OE    (BIT(0))
#define IO_MUX_GPIO56_MCU_OE_M  (IO_MUX_GPIO56_MCU_OE_V << IO_MUX_GPIO56_MCU_OE_S)
#define IO_MUX_GPIO56_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO56_MCU_OE_S  0
/** IO_MUX_GPIO56_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  io sleep mode enable. set 1 to enable sleep mode.
 */
#define IO_MUX_GPIO56_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO56_SLP_SEL_M  (IO_MUX_GPIO56_SLP_SEL_V << IO_MUX_GPIO56_SLP_SEL_S)
#define IO_MUX_GPIO56_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO56_SLP_SEL_S  1
/** IO_MUX_GPIO56_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  pull-down enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO56_MCU_WPD_M  (IO_MUX_GPIO56_MCU_WPD_V << IO_MUX_GPIO56_MCU_WPD_S)
#define IO_MUX_GPIO56_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO56_MCU_WPD_S  2
/** IO_MUX_GPIO56_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  pull-up enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO56_MCU_WPU_M  (IO_MUX_GPIO56_MCU_WPU_V << IO_MUX_GPIO56_MCU_WPU_S)
#define IO_MUX_GPIO56_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO56_MCU_WPU_S  3
/** IO_MUX_GPIO56_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  input enable on sleep mode
 */
#define IO_MUX_GPIO56_MCU_IE    (BIT(4))
#define IO_MUX_GPIO56_MCU_IE_M  (IO_MUX_GPIO56_MCU_IE_V << IO_MUX_GPIO56_MCU_IE_S)
#define IO_MUX_GPIO56_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO56_MCU_IE_S  4
/** IO_MUX_GPIO56_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  select drive strenth on sleep mode
 */
#define IO_MUX_GPIO56_MCU_DRV    0x00000003U
#define IO_MUX_GPIO56_MCU_DRV_M  (IO_MUX_GPIO56_MCU_DRV_V << IO_MUX_GPIO56_MCU_DRV_S)
#define IO_MUX_GPIO56_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO56_MCU_DRV_S  5
/** IO_MUX_GPIO56_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  pull-down enable
 */
#define IO_MUX_GPIO56_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO56_FUN_WPD_M  (IO_MUX_GPIO56_FUN_WPD_V << IO_MUX_GPIO56_FUN_WPD_S)
#define IO_MUX_GPIO56_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO56_FUN_WPD_S  7
/** IO_MUX_GPIO56_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  pull-up enable
 */
#define IO_MUX_GPIO56_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO56_FUN_WPU_M  (IO_MUX_GPIO56_FUN_WPU_V << IO_MUX_GPIO56_FUN_WPU_S)
#define IO_MUX_GPIO56_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO56_FUN_WPU_S  8
/** IO_MUX_GPIO56_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  input enable
 */
#define IO_MUX_GPIO56_FUN_IE    (BIT(9))
#define IO_MUX_GPIO56_FUN_IE_M  (IO_MUX_GPIO56_FUN_IE_V << IO_MUX_GPIO56_FUN_IE_S)
#define IO_MUX_GPIO56_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO56_FUN_IE_S  9
/** IO_MUX_GPIO56_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  select drive strenth, 0:5mA, 1:10mA, 2:20mA, 3:40mA
 */
#define IO_MUX_GPIO56_FUN_DRV    0x00000003U
#define IO_MUX_GPIO56_FUN_DRV_M  (IO_MUX_GPIO56_FUN_DRV_V << IO_MUX_GPIO56_FUN_DRV_S)
#define IO_MUX_GPIO56_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO56_FUN_DRV_S  10
/** IO_MUX_GPIO56_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  0:select function0, 1:select function1 ...
 */
#define IO_MUX_GPIO56_MCU_SEL    0x00000007U
#define IO_MUX_GPIO56_MCU_SEL_M  (IO_MUX_GPIO56_MCU_SEL_V << IO_MUX_GPIO56_MCU_SEL_S)
#define IO_MUX_GPIO56_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO56_MCU_SEL_S  12
/** IO_MUX_GPIO56_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  input filter enable
 */
#define IO_MUX_GPIO56_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO56_FILTER_EN_M  (IO_MUX_GPIO56_FILTER_EN_V << IO_MUX_GPIO56_FILTER_EN_S)
#define IO_MUX_GPIO56_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO56_FILTER_EN_S  15

/** IO_MUX_DATE_REG register
 *  iomux version
 */
#define IO_MUX_DATE_REG (DR_REG_IO_MUX_BASE + 0x104)
/** IO_MUX_DATE : R/W; bitpos: [27:0]; default: 2101794;
 *  csv date
 */
#define IO_MUX_DATE    0x0FFFFFFFU
#define IO_MUX_DATE_M  (IO_MUX_DATE_V << IO_MUX_DATE_S)
#define IO_MUX_DATE_V  0x0FFFFFFFU
#define IO_MUX_DATE_S  0

#ifdef __cplusplus
}
#endif
