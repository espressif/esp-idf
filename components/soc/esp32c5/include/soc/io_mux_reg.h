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

/** IO_MUX_PIN_CTRL_REG register
 *  Clock Output Configuration Register
 */
#define IO_MUX_PIN_CTRL_REG (DR_REG_IO_MUX_BASE + 0x0)
/** IO_MUX_CLK_OUT1 : R/W; bitpos: [4:0]; default: 15;
 *  If you want to output clock for I2S to CLK_OUT_out1, set this register to 0x0.
 *  CLK_OUT_out1 can be found in peripheral output signals.
 */
#define IO_MUX_CLK_OUT1    0x0000001FU
#define IO_MUX_CLK_OUT1_M  (IO_MUX_CLK_OUT1_V << IO_MUX_CLK_OUT1_S)
#define IO_MUX_CLK_OUT1_V  0x0000001FU
#define IO_MUX_CLK_OUT1_S  0
/** IO_MUX_CLK_OUT2 : R/W; bitpos: [9:5]; default: 15;
 *  If you want to output clock for I2S to CLK_OUT_out2, set this register to 0x0.
 *  CLK_OUT_out2 can be found in peripheral output signals.
 */
#define IO_MUX_CLK_OUT2    0x0000001FU
#define IO_MUX_CLK_OUT2_M  (IO_MUX_CLK_OUT2_V << IO_MUX_CLK_OUT2_S)
#define IO_MUX_CLK_OUT2_V  0x0000001FU
#define IO_MUX_CLK_OUT2_S  5
/** IO_MUX_CLK_OUT3 : R/W; bitpos: [14:10]; default: 7;
 *  If you want to output clock for I2S to CLK_OUT_out3, set this register to 0x0.
 *  CLK_OUT_out3 can be found in peripheral output signals.
 */
#define IO_MUX_CLK_OUT3    0x0000001FU
#define IO_MUX_CLK_OUT3_M  (IO_MUX_CLK_OUT3_V << IO_MUX_CLK_OUT3_S)
#define IO_MUX_CLK_OUT3_V  0x0000001FU
#define IO_MUX_CLK_OUT3_S  10

/** IO_MUX_GPIO0_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO0_REG (DR_REG_IO_MUX_BASE + 0x4)
/** IO_MUX_GPIO0_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO0_MCU_OE    (BIT(0))
#define IO_MUX_GPIO0_MCU_OE_M  (IO_MUX_GPIO0_MCU_OE_V << IO_MUX_GPIO0_MCU_OE_S)
#define IO_MUX_GPIO0_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_OE_S  0
/** IO_MUX_GPIO0_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO0_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO0_SLP_SEL_M  (IO_MUX_GPIO0_SLP_SEL_V << IO_MUX_GPIO0_SLP_SEL_S)
#define IO_MUX_GPIO0_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO0_SLP_SEL_S  1
/** IO_MUX_GPIO0_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO0_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO0_MCU_WPD_M  (IO_MUX_GPIO0_MCU_WPD_V << IO_MUX_GPIO0_MCU_WPD_S)
#define IO_MUX_GPIO0_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPD_S  2
/** IO_MUX_GPIO0_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO0_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO0_MCU_WPU_M  (IO_MUX_GPIO0_MCU_WPU_V << IO_MUX_GPIO0_MCU_WPU_S)
#define IO_MUX_GPIO0_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO0_MCU_WPU_S  3
/** IO_MUX_GPIO0_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO0_MCU_IE    (BIT(4))
#define IO_MUX_GPIO0_MCU_IE_M  (IO_MUX_GPIO0_MCU_IE_V << IO_MUX_GPIO0_MCU_IE_S)
#define IO_MUX_GPIO0_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO0_MCU_IE_S  4
/** IO_MUX_GPIO0_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO0_MCU_DRV    0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_M  (IO_MUX_GPIO0_MCU_DRV_V << IO_MUX_GPIO0_MCU_DRV_S)
#define IO_MUX_GPIO0_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO0_MCU_DRV_S  5
/** IO_MUX_GPIO0_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO0_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO0_FUN_WPD_M  (IO_MUX_GPIO0_FUN_WPD_V << IO_MUX_GPIO0_FUN_WPD_S)
#define IO_MUX_GPIO0_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPD_S  7
/** IO_MUX_GPIO0_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO0_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO0_FUN_WPU_M  (IO_MUX_GPIO0_FUN_WPU_V << IO_MUX_GPIO0_FUN_WPU_S)
#define IO_MUX_GPIO0_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO0_FUN_WPU_S  8
/** IO_MUX_GPIO0_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO0_FUN_IE    (BIT(9))
#define IO_MUX_GPIO0_FUN_IE_M  (IO_MUX_GPIO0_FUN_IE_V << IO_MUX_GPIO0_FUN_IE_S)
#define IO_MUX_GPIO0_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO0_FUN_IE_S  9
/** IO_MUX_GPIO0_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO0_FUN_DRV    0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_M  (IO_MUX_GPIO0_FUN_DRV_V << IO_MUX_GPIO0_FUN_DRV_S)
#define IO_MUX_GPIO0_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO0_FUN_DRV_S  10
/** IO_MUX_GPIO0_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO0_MCU_SEL    0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_M  (IO_MUX_GPIO0_MCU_SEL_V << IO_MUX_GPIO0_MCU_SEL_S)
#define IO_MUX_GPIO0_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO0_MCU_SEL_S  12
/** IO_MUX_GPIO0_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO0_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO0_FILTER_EN_M  (IO_MUX_GPIO0_FILTER_EN_V << IO_MUX_GPIO0_FILTER_EN_S)
#define IO_MUX_GPIO0_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO0_FILTER_EN_S  15
/** IO_MUX_GPIO0_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO0_HYS_EN    (BIT(16))
#define IO_MUX_GPIO0_HYS_EN_M  (IO_MUX_GPIO0_HYS_EN_V << IO_MUX_GPIO0_HYS_EN_S)
#define IO_MUX_GPIO0_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO0_HYS_EN_S  16
/** IO_MUX_GPIO0_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO0_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO0_HYS_SEL_M  (IO_MUX_GPIO0_HYS_SEL_V << IO_MUX_GPIO0_HYS_SEL_S)
#define IO_MUX_GPIO0_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO0_HYS_SEL_S  17

/** IO_MUX_GPIO1_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO1_REG (DR_REG_IO_MUX_BASE + 0x8)
/** IO_MUX_GPIO1_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO1_MCU_OE    (BIT(0))
#define IO_MUX_GPIO1_MCU_OE_M  (IO_MUX_GPIO1_MCU_OE_V << IO_MUX_GPIO1_MCU_OE_S)
#define IO_MUX_GPIO1_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_OE_S  0
/** IO_MUX_GPIO1_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO1_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO1_SLP_SEL_M  (IO_MUX_GPIO1_SLP_SEL_V << IO_MUX_GPIO1_SLP_SEL_S)
#define IO_MUX_GPIO1_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO1_SLP_SEL_S  1
/** IO_MUX_GPIO1_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO1_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO1_MCU_WPD_M  (IO_MUX_GPIO1_MCU_WPD_V << IO_MUX_GPIO1_MCU_WPD_S)
#define IO_MUX_GPIO1_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPD_S  2
/** IO_MUX_GPIO1_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO1_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO1_MCU_WPU_M  (IO_MUX_GPIO1_MCU_WPU_V << IO_MUX_GPIO1_MCU_WPU_S)
#define IO_MUX_GPIO1_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO1_MCU_WPU_S  3
/** IO_MUX_GPIO1_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO1_MCU_IE    (BIT(4))
#define IO_MUX_GPIO1_MCU_IE_M  (IO_MUX_GPIO1_MCU_IE_V << IO_MUX_GPIO1_MCU_IE_S)
#define IO_MUX_GPIO1_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO1_MCU_IE_S  4
/** IO_MUX_GPIO1_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO1_MCU_DRV    0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_M  (IO_MUX_GPIO1_MCU_DRV_V << IO_MUX_GPIO1_MCU_DRV_S)
#define IO_MUX_GPIO1_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO1_MCU_DRV_S  5
/** IO_MUX_GPIO1_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO1_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO1_FUN_WPD_M  (IO_MUX_GPIO1_FUN_WPD_V << IO_MUX_GPIO1_FUN_WPD_S)
#define IO_MUX_GPIO1_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPD_S  7
/** IO_MUX_GPIO1_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO1_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO1_FUN_WPU_M  (IO_MUX_GPIO1_FUN_WPU_V << IO_MUX_GPIO1_FUN_WPU_S)
#define IO_MUX_GPIO1_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO1_FUN_WPU_S  8
/** IO_MUX_GPIO1_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO1_FUN_IE    (BIT(9))
#define IO_MUX_GPIO1_FUN_IE_M  (IO_MUX_GPIO1_FUN_IE_V << IO_MUX_GPIO1_FUN_IE_S)
#define IO_MUX_GPIO1_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO1_FUN_IE_S  9
/** IO_MUX_GPIO1_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO1_FUN_DRV    0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_M  (IO_MUX_GPIO1_FUN_DRV_V << IO_MUX_GPIO1_FUN_DRV_S)
#define IO_MUX_GPIO1_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO1_FUN_DRV_S  10
/** IO_MUX_GPIO1_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO1_MCU_SEL    0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_M  (IO_MUX_GPIO1_MCU_SEL_V << IO_MUX_GPIO1_MCU_SEL_S)
#define IO_MUX_GPIO1_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO1_MCU_SEL_S  12
/** IO_MUX_GPIO1_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO1_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO1_FILTER_EN_M  (IO_MUX_GPIO1_FILTER_EN_V << IO_MUX_GPIO1_FILTER_EN_S)
#define IO_MUX_GPIO1_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO1_FILTER_EN_S  15
/** IO_MUX_GPIO1_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO1_HYS_EN    (BIT(16))
#define IO_MUX_GPIO1_HYS_EN_M  (IO_MUX_GPIO1_HYS_EN_V << IO_MUX_GPIO1_HYS_EN_S)
#define IO_MUX_GPIO1_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO1_HYS_EN_S  16
/** IO_MUX_GPIO1_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO1_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO1_HYS_SEL_M  (IO_MUX_GPIO1_HYS_SEL_V << IO_MUX_GPIO1_HYS_SEL_S)
#define IO_MUX_GPIO1_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO1_HYS_SEL_S  17

/** IO_MUX_GPIO2_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO2_REG (DR_REG_IO_MUX_BASE + 0xc)
/** IO_MUX_GPIO2_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO2_MCU_OE    (BIT(0))
#define IO_MUX_GPIO2_MCU_OE_M  (IO_MUX_GPIO2_MCU_OE_V << IO_MUX_GPIO2_MCU_OE_S)
#define IO_MUX_GPIO2_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_OE_S  0
/** IO_MUX_GPIO2_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO2_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO2_SLP_SEL_M  (IO_MUX_GPIO2_SLP_SEL_V << IO_MUX_GPIO2_SLP_SEL_S)
#define IO_MUX_GPIO2_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO2_SLP_SEL_S  1
/** IO_MUX_GPIO2_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO2_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO2_MCU_WPD_M  (IO_MUX_GPIO2_MCU_WPD_V << IO_MUX_GPIO2_MCU_WPD_S)
#define IO_MUX_GPIO2_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPD_S  2
/** IO_MUX_GPIO2_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO2_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO2_MCU_WPU_M  (IO_MUX_GPIO2_MCU_WPU_V << IO_MUX_GPIO2_MCU_WPU_S)
#define IO_MUX_GPIO2_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO2_MCU_WPU_S  3
/** IO_MUX_GPIO2_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO2_MCU_IE    (BIT(4))
#define IO_MUX_GPIO2_MCU_IE_M  (IO_MUX_GPIO2_MCU_IE_V << IO_MUX_GPIO2_MCU_IE_S)
#define IO_MUX_GPIO2_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO2_MCU_IE_S  4
/** IO_MUX_GPIO2_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO2_MCU_DRV    0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_M  (IO_MUX_GPIO2_MCU_DRV_V << IO_MUX_GPIO2_MCU_DRV_S)
#define IO_MUX_GPIO2_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO2_MCU_DRV_S  5
/** IO_MUX_GPIO2_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO2_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO2_FUN_WPD_M  (IO_MUX_GPIO2_FUN_WPD_V << IO_MUX_GPIO2_FUN_WPD_S)
#define IO_MUX_GPIO2_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPD_S  7
/** IO_MUX_GPIO2_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO2_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO2_FUN_WPU_M  (IO_MUX_GPIO2_FUN_WPU_V << IO_MUX_GPIO2_FUN_WPU_S)
#define IO_MUX_GPIO2_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO2_FUN_WPU_S  8
/** IO_MUX_GPIO2_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO2_FUN_IE    (BIT(9))
#define IO_MUX_GPIO2_FUN_IE_M  (IO_MUX_GPIO2_FUN_IE_V << IO_MUX_GPIO2_FUN_IE_S)
#define IO_MUX_GPIO2_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO2_FUN_IE_S  9
/** IO_MUX_GPIO2_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO2_FUN_DRV    0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_M  (IO_MUX_GPIO2_FUN_DRV_V << IO_MUX_GPIO2_FUN_DRV_S)
#define IO_MUX_GPIO2_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO2_FUN_DRV_S  10
/** IO_MUX_GPIO2_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO2_MCU_SEL    0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_M  (IO_MUX_GPIO2_MCU_SEL_V << IO_MUX_GPIO2_MCU_SEL_S)
#define IO_MUX_GPIO2_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO2_MCU_SEL_S  12
/** IO_MUX_GPIO2_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO2_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO2_FILTER_EN_M  (IO_MUX_GPIO2_FILTER_EN_V << IO_MUX_GPIO2_FILTER_EN_S)
#define IO_MUX_GPIO2_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO2_FILTER_EN_S  15
/** IO_MUX_GPIO2_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO2_HYS_EN    (BIT(16))
#define IO_MUX_GPIO2_HYS_EN_M  (IO_MUX_GPIO2_HYS_EN_V << IO_MUX_GPIO2_HYS_EN_S)
#define IO_MUX_GPIO2_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO2_HYS_EN_S  16
/** IO_MUX_GPIO2_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO2_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO2_HYS_SEL_M  (IO_MUX_GPIO2_HYS_SEL_V << IO_MUX_GPIO2_HYS_SEL_S)
#define IO_MUX_GPIO2_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO2_HYS_SEL_S  17

/** IO_MUX_GPIO3_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO3_REG (DR_REG_IO_MUX_BASE + 0x10)
/** IO_MUX_GPIO3_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO3_MCU_OE    (BIT(0))
#define IO_MUX_GPIO3_MCU_OE_M  (IO_MUX_GPIO3_MCU_OE_V << IO_MUX_GPIO3_MCU_OE_S)
#define IO_MUX_GPIO3_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_OE_S  0
/** IO_MUX_GPIO3_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO3_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO3_SLP_SEL_M  (IO_MUX_GPIO3_SLP_SEL_V << IO_MUX_GPIO3_SLP_SEL_S)
#define IO_MUX_GPIO3_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO3_SLP_SEL_S  1
/** IO_MUX_GPIO3_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO3_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO3_MCU_WPD_M  (IO_MUX_GPIO3_MCU_WPD_V << IO_MUX_GPIO3_MCU_WPD_S)
#define IO_MUX_GPIO3_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPD_S  2
/** IO_MUX_GPIO3_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO3_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO3_MCU_WPU_M  (IO_MUX_GPIO3_MCU_WPU_V << IO_MUX_GPIO3_MCU_WPU_S)
#define IO_MUX_GPIO3_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO3_MCU_WPU_S  3
/** IO_MUX_GPIO3_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO3_MCU_IE    (BIT(4))
#define IO_MUX_GPIO3_MCU_IE_M  (IO_MUX_GPIO3_MCU_IE_V << IO_MUX_GPIO3_MCU_IE_S)
#define IO_MUX_GPIO3_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO3_MCU_IE_S  4
/** IO_MUX_GPIO3_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO3_MCU_DRV    0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_M  (IO_MUX_GPIO3_MCU_DRV_V << IO_MUX_GPIO3_MCU_DRV_S)
#define IO_MUX_GPIO3_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO3_MCU_DRV_S  5
/** IO_MUX_GPIO3_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO3_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO3_FUN_WPD_M  (IO_MUX_GPIO3_FUN_WPD_V << IO_MUX_GPIO3_FUN_WPD_S)
#define IO_MUX_GPIO3_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPD_S  7
/** IO_MUX_GPIO3_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO3_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO3_FUN_WPU_M  (IO_MUX_GPIO3_FUN_WPU_V << IO_MUX_GPIO3_FUN_WPU_S)
#define IO_MUX_GPIO3_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO3_FUN_WPU_S  8
/** IO_MUX_GPIO3_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO3_FUN_IE    (BIT(9))
#define IO_MUX_GPIO3_FUN_IE_M  (IO_MUX_GPIO3_FUN_IE_V << IO_MUX_GPIO3_FUN_IE_S)
#define IO_MUX_GPIO3_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO3_FUN_IE_S  9
/** IO_MUX_GPIO3_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO3_FUN_DRV    0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_M  (IO_MUX_GPIO3_FUN_DRV_V << IO_MUX_GPIO3_FUN_DRV_S)
#define IO_MUX_GPIO3_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO3_FUN_DRV_S  10
/** IO_MUX_GPIO3_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO3_MCU_SEL    0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_M  (IO_MUX_GPIO3_MCU_SEL_V << IO_MUX_GPIO3_MCU_SEL_S)
#define IO_MUX_GPIO3_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO3_MCU_SEL_S  12
/** IO_MUX_GPIO3_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO3_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO3_FILTER_EN_M  (IO_MUX_GPIO3_FILTER_EN_V << IO_MUX_GPIO3_FILTER_EN_S)
#define IO_MUX_GPIO3_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO3_FILTER_EN_S  15
/** IO_MUX_GPIO3_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO3_HYS_EN    (BIT(16))
#define IO_MUX_GPIO3_HYS_EN_M  (IO_MUX_GPIO3_HYS_EN_V << IO_MUX_GPIO3_HYS_EN_S)
#define IO_MUX_GPIO3_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO3_HYS_EN_S  16
/** IO_MUX_GPIO3_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO3_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO3_HYS_SEL_M  (IO_MUX_GPIO3_HYS_SEL_V << IO_MUX_GPIO3_HYS_SEL_S)
#define IO_MUX_GPIO3_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO3_HYS_SEL_S  17

/** IO_MUX_GPIO4_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO4_REG (DR_REG_IO_MUX_BASE + 0x14)
/** IO_MUX_GPIO4_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO4_MCU_OE    (BIT(0))
#define IO_MUX_GPIO4_MCU_OE_M  (IO_MUX_GPIO4_MCU_OE_V << IO_MUX_GPIO4_MCU_OE_S)
#define IO_MUX_GPIO4_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_OE_S  0
/** IO_MUX_GPIO4_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO4_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO4_SLP_SEL_M  (IO_MUX_GPIO4_SLP_SEL_V << IO_MUX_GPIO4_SLP_SEL_S)
#define IO_MUX_GPIO4_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO4_SLP_SEL_S  1
/** IO_MUX_GPIO4_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO4_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO4_MCU_WPD_M  (IO_MUX_GPIO4_MCU_WPD_V << IO_MUX_GPIO4_MCU_WPD_S)
#define IO_MUX_GPIO4_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPD_S  2
/** IO_MUX_GPIO4_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO4_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO4_MCU_WPU_M  (IO_MUX_GPIO4_MCU_WPU_V << IO_MUX_GPIO4_MCU_WPU_S)
#define IO_MUX_GPIO4_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO4_MCU_WPU_S  3
/** IO_MUX_GPIO4_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO4_MCU_IE    (BIT(4))
#define IO_MUX_GPIO4_MCU_IE_M  (IO_MUX_GPIO4_MCU_IE_V << IO_MUX_GPIO4_MCU_IE_S)
#define IO_MUX_GPIO4_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO4_MCU_IE_S  4
/** IO_MUX_GPIO4_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO4_MCU_DRV    0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_M  (IO_MUX_GPIO4_MCU_DRV_V << IO_MUX_GPIO4_MCU_DRV_S)
#define IO_MUX_GPIO4_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO4_MCU_DRV_S  5
/** IO_MUX_GPIO4_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO4_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO4_FUN_WPD_M  (IO_MUX_GPIO4_FUN_WPD_V << IO_MUX_GPIO4_FUN_WPD_S)
#define IO_MUX_GPIO4_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPD_S  7
/** IO_MUX_GPIO4_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO4_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO4_FUN_WPU_M  (IO_MUX_GPIO4_FUN_WPU_V << IO_MUX_GPIO4_FUN_WPU_S)
#define IO_MUX_GPIO4_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO4_FUN_WPU_S  8
/** IO_MUX_GPIO4_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO4_FUN_IE    (BIT(9))
#define IO_MUX_GPIO4_FUN_IE_M  (IO_MUX_GPIO4_FUN_IE_V << IO_MUX_GPIO4_FUN_IE_S)
#define IO_MUX_GPIO4_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO4_FUN_IE_S  9
/** IO_MUX_GPIO4_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO4_FUN_DRV    0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_M  (IO_MUX_GPIO4_FUN_DRV_V << IO_MUX_GPIO4_FUN_DRV_S)
#define IO_MUX_GPIO4_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO4_FUN_DRV_S  10
/** IO_MUX_GPIO4_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO4_MCU_SEL    0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_M  (IO_MUX_GPIO4_MCU_SEL_V << IO_MUX_GPIO4_MCU_SEL_S)
#define IO_MUX_GPIO4_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO4_MCU_SEL_S  12
/** IO_MUX_GPIO4_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO4_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO4_FILTER_EN_M  (IO_MUX_GPIO4_FILTER_EN_V << IO_MUX_GPIO4_FILTER_EN_S)
#define IO_MUX_GPIO4_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO4_FILTER_EN_S  15
/** IO_MUX_GPIO4_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO4_HYS_EN    (BIT(16))
#define IO_MUX_GPIO4_HYS_EN_M  (IO_MUX_GPIO4_HYS_EN_V << IO_MUX_GPIO4_HYS_EN_S)
#define IO_MUX_GPIO4_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO4_HYS_EN_S  16
/** IO_MUX_GPIO4_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO4_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO4_HYS_SEL_M  (IO_MUX_GPIO4_HYS_SEL_V << IO_MUX_GPIO4_HYS_SEL_S)
#define IO_MUX_GPIO4_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO4_HYS_SEL_S  17

/** IO_MUX_GPIO5_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO5_REG (DR_REG_IO_MUX_BASE + 0x18)
/** IO_MUX_GPIO5_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO5_MCU_OE    (BIT(0))
#define IO_MUX_GPIO5_MCU_OE_M  (IO_MUX_GPIO5_MCU_OE_V << IO_MUX_GPIO5_MCU_OE_S)
#define IO_MUX_GPIO5_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_OE_S  0
/** IO_MUX_GPIO5_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO5_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO5_SLP_SEL_M  (IO_MUX_GPIO5_SLP_SEL_V << IO_MUX_GPIO5_SLP_SEL_S)
#define IO_MUX_GPIO5_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO5_SLP_SEL_S  1
/** IO_MUX_GPIO5_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO5_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO5_MCU_WPD_M  (IO_MUX_GPIO5_MCU_WPD_V << IO_MUX_GPIO5_MCU_WPD_S)
#define IO_MUX_GPIO5_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPD_S  2
/** IO_MUX_GPIO5_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO5_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO5_MCU_WPU_M  (IO_MUX_GPIO5_MCU_WPU_V << IO_MUX_GPIO5_MCU_WPU_S)
#define IO_MUX_GPIO5_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO5_MCU_WPU_S  3
/** IO_MUX_GPIO5_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO5_MCU_IE    (BIT(4))
#define IO_MUX_GPIO5_MCU_IE_M  (IO_MUX_GPIO5_MCU_IE_V << IO_MUX_GPIO5_MCU_IE_S)
#define IO_MUX_GPIO5_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO5_MCU_IE_S  4
/** IO_MUX_GPIO5_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO5_MCU_DRV    0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_M  (IO_MUX_GPIO5_MCU_DRV_V << IO_MUX_GPIO5_MCU_DRV_S)
#define IO_MUX_GPIO5_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO5_MCU_DRV_S  5
/** IO_MUX_GPIO5_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO5_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO5_FUN_WPD_M  (IO_MUX_GPIO5_FUN_WPD_V << IO_MUX_GPIO5_FUN_WPD_S)
#define IO_MUX_GPIO5_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPD_S  7
/** IO_MUX_GPIO5_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO5_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO5_FUN_WPU_M  (IO_MUX_GPIO5_FUN_WPU_V << IO_MUX_GPIO5_FUN_WPU_S)
#define IO_MUX_GPIO5_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO5_FUN_WPU_S  8
/** IO_MUX_GPIO5_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO5_FUN_IE    (BIT(9))
#define IO_MUX_GPIO5_FUN_IE_M  (IO_MUX_GPIO5_FUN_IE_V << IO_MUX_GPIO5_FUN_IE_S)
#define IO_MUX_GPIO5_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO5_FUN_IE_S  9
/** IO_MUX_GPIO5_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO5_FUN_DRV    0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_M  (IO_MUX_GPIO5_FUN_DRV_V << IO_MUX_GPIO5_FUN_DRV_S)
#define IO_MUX_GPIO5_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO5_FUN_DRV_S  10
/** IO_MUX_GPIO5_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO5_MCU_SEL    0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_M  (IO_MUX_GPIO5_MCU_SEL_V << IO_MUX_GPIO5_MCU_SEL_S)
#define IO_MUX_GPIO5_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO5_MCU_SEL_S  12
/** IO_MUX_GPIO5_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO5_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO5_FILTER_EN_M  (IO_MUX_GPIO5_FILTER_EN_V << IO_MUX_GPIO5_FILTER_EN_S)
#define IO_MUX_GPIO5_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO5_FILTER_EN_S  15
/** IO_MUX_GPIO5_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO5_HYS_EN    (BIT(16))
#define IO_MUX_GPIO5_HYS_EN_M  (IO_MUX_GPIO5_HYS_EN_V << IO_MUX_GPIO5_HYS_EN_S)
#define IO_MUX_GPIO5_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO5_HYS_EN_S  16
/** IO_MUX_GPIO5_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO5_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO5_HYS_SEL_M  (IO_MUX_GPIO5_HYS_SEL_V << IO_MUX_GPIO5_HYS_SEL_S)
#define IO_MUX_GPIO5_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO5_HYS_SEL_S  17

/** IO_MUX_GPIO6_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO6_REG (DR_REG_IO_MUX_BASE + 0x1c)
/** IO_MUX_GPIO6_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO6_MCU_OE    (BIT(0))
#define IO_MUX_GPIO6_MCU_OE_M  (IO_MUX_GPIO6_MCU_OE_V << IO_MUX_GPIO6_MCU_OE_S)
#define IO_MUX_GPIO6_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_OE_S  0
/** IO_MUX_GPIO6_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO6_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO6_SLP_SEL_M  (IO_MUX_GPIO6_SLP_SEL_V << IO_MUX_GPIO6_SLP_SEL_S)
#define IO_MUX_GPIO6_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO6_SLP_SEL_S  1
/** IO_MUX_GPIO6_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO6_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO6_MCU_WPD_M  (IO_MUX_GPIO6_MCU_WPD_V << IO_MUX_GPIO6_MCU_WPD_S)
#define IO_MUX_GPIO6_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPD_S  2
/** IO_MUX_GPIO6_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO6_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO6_MCU_WPU_M  (IO_MUX_GPIO6_MCU_WPU_V << IO_MUX_GPIO6_MCU_WPU_S)
#define IO_MUX_GPIO6_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO6_MCU_WPU_S  3
/** IO_MUX_GPIO6_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO6_MCU_IE    (BIT(4))
#define IO_MUX_GPIO6_MCU_IE_M  (IO_MUX_GPIO6_MCU_IE_V << IO_MUX_GPIO6_MCU_IE_S)
#define IO_MUX_GPIO6_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO6_MCU_IE_S  4
/** IO_MUX_GPIO6_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO6_MCU_DRV    0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_M  (IO_MUX_GPIO6_MCU_DRV_V << IO_MUX_GPIO6_MCU_DRV_S)
#define IO_MUX_GPIO6_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO6_MCU_DRV_S  5
/** IO_MUX_GPIO6_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO6_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO6_FUN_WPD_M  (IO_MUX_GPIO6_FUN_WPD_V << IO_MUX_GPIO6_FUN_WPD_S)
#define IO_MUX_GPIO6_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPD_S  7
/** IO_MUX_GPIO6_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO6_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO6_FUN_WPU_M  (IO_MUX_GPIO6_FUN_WPU_V << IO_MUX_GPIO6_FUN_WPU_S)
#define IO_MUX_GPIO6_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO6_FUN_WPU_S  8
/** IO_MUX_GPIO6_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO6_FUN_IE    (BIT(9))
#define IO_MUX_GPIO6_FUN_IE_M  (IO_MUX_GPIO6_FUN_IE_V << IO_MUX_GPIO6_FUN_IE_S)
#define IO_MUX_GPIO6_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO6_FUN_IE_S  9
/** IO_MUX_GPIO6_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO6_FUN_DRV    0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_M  (IO_MUX_GPIO6_FUN_DRV_V << IO_MUX_GPIO6_FUN_DRV_S)
#define IO_MUX_GPIO6_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO6_FUN_DRV_S  10
/** IO_MUX_GPIO6_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO6_MCU_SEL    0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_M  (IO_MUX_GPIO6_MCU_SEL_V << IO_MUX_GPIO6_MCU_SEL_S)
#define IO_MUX_GPIO6_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO6_MCU_SEL_S  12
/** IO_MUX_GPIO6_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO6_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO6_FILTER_EN_M  (IO_MUX_GPIO6_FILTER_EN_V << IO_MUX_GPIO6_FILTER_EN_S)
#define IO_MUX_GPIO6_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO6_FILTER_EN_S  15
/** IO_MUX_GPIO6_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO6_HYS_EN    (BIT(16))
#define IO_MUX_GPIO6_HYS_EN_M  (IO_MUX_GPIO6_HYS_EN_V << IO_MUX_GPIO6_HYS_EN_S)
#define IO_MUX_GPIO6_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO6_HYS_EN_S  16
/** IO_MUX_GPIO6_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO6_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO6_HYS_SEL_M  (IO_MUX_GPIO6_HYS_SEL_V << IO_MUX_GPIO6_HYS_SEL_S)
#define IO_MUX_GPIO6_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO6_HYS_SEL_S  17

/** IO_MUX_GPIO7_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO7_REG (DR_REG_IO_MUX_BASE + 0x20)
/** IO_MUX_GPIO7_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO7_MCU_OE    (BIT(0))
#define IO_MUX_GPIO7_MCU_OE_M  (IO_MUX_GPIO7_MCU_OE_V << IO_MUX_GPIO7_MCU_OE_S)
#define IO_MUX_GPIO7_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_OE_S  0
/** IO_MUX_GPIO7_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO7_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO7_SLP_SEL_M  (IO_MUX_GPIO7_SLP_SEL_V << IO_MUX_GPIO7_SLP_SEL_S)
#define IO_MUX_GPIO7_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO7_SLP_SEL_S  1
/** IO_MUX_GPIO7_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO7_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO7_MCU_WPD_M  (IO_MUX_GPIO7_MCU_WPD_V << IO_MUX_GPIO7_MCU_WPD_S)
#define IO_MUX_GPIO7_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPD_S  2
/** IO_MUX_GPIO7_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO7_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO7_MCU_WPU_M  (IO_MUX_GPIO7_MCU_WPU_V << IO_MUX_GPIO7_MCU_WPU_S)
#define IO_MUX_GPIO7_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO7_MCU_WPU_S  3
/** IO_MUX_GPIO7_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO7_MCU_IE    (BIT(4))
#define IO_MUX_GPIO7_MCU_IE_M  (IO_MUX_GPIO7_MCU_IE_V << IO_MUX_GPIO7_MCU_IE_S)
#define IO_MUX_GPIO7_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO7_MCU_IE_S  4
/** IO_MUX_GPIO7_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO7_MCU_DRV    0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_M  (IO_MUX_GPIO7_MCU_DRV_V << IO_MUX_GPIO7_MCU_DRV_S)
#define IO_MUX_GPIO7_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO7_MCU_DRV_S  5
/** IO_MUX_GPIO7_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO7_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO7_FUN_WPD_M  (IO_MUX_GPIO7_FUN_WPD_V << IO_MUX_GPIO7_FUN_WPD_S)
#define IO_MUX_GPIO7_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPD_S  7
/** IO_MUX_GPIO7_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO7_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO7_FUN_WPU_M  (IO_MUX_GPIO7_FUN_WPU_V << IO_MUX_GPIO7_FUN_WPU_S)
#define IO_MUX_GPIO7_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO7_FUN_WPU_S  8
/** IO_MUX_GPIO7_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO7_FUN_IE    (BIT(9))
#define IO_MUX_GPIO7_FUN_IE_M  (IO_MUX_GPIO7_FUN_IE_V << IO_MUX_GPIO7_FUN_IE_S)
#define IO_MUX_GPIO7_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO7_FUN_IE_S  9
/** IO_MUX_GPIO7_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO7_FUN_DRV    0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_M  (IO_MUX_GPIO7_FUN_DRV_V << IO_MUX_GPIO7_FUN_DRV_S)
#define IO_MUX_GPIO7_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO7_FUN_DRV_S  10
/** IO_MUX_GPIO7_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO7_MCU_SEL    0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_M  (IO_MUX_GPIO7_MCU_SEL_V << IO_MUX_GPIO7_MCU_SEL_S)
#define IO_MUX_GPIO7_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO7_MCU_SEL_S  12
/** IO_MUX_GPIO7_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO7_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO7_FILTER_EN_M  (IO_MUX_GPIO7_FILTER_EN_V << IO_MUX_GPIO7_FILTER_EN_S)
#define IO_MUX_GPIO7_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO7_FILTER_EN_S  15
/** IO_MUX_GPIO7_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO7_HYS_EN    (BIT(16))
#define IO_MUX_GPIO7_HYS_EN_M  (IO_MUX_GPIO7_HYS_EN_V << IO_MUX_GPIO7_HYS_EN_S)
#define IO_MUX_GPIO7_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO7_HYS_EN_S  16
/** IO_MUX_GPIO7_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO7_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO7_HYS_SEL_M  (IO_MUX_GPIO7_HYS_SEL_V << IO_MUX_GPIO7_HYS_SEL_S)
#define IO_MUX_GPIO7_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO7_HYS_SEL_S  17

/** IO_MUX_GPIO8_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO8_REG (DR_REG_IO_MUX_BASE + 0x24)
/** IO_MUX_GPIO8_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO8_MCU_OE    (BIT(0))
#define IO_MUX_GPIO8_MCU_OE_M  (IO_MUX_GPIO8_MCU_OE_V << IO_MUX_GPIO8_MCU_OE_S)
#define IO_MUX_GPIO8_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_OE_S  0
/** IO_MUX_GPIO8_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO8_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO8_SLP_SEL_M  (IO_MUX_GPIO8_SLP_SEL_V << IO_MUX_GPIO8_SLP_SEL_S)
#define IO_MUX_GPIO8_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO8_SLP_SEL_S  1
/** IO_MUX_GPIO8_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO8_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO8_MCU_WPD_M  (IO_MUX_GPIO8_MCU_WPD_V << IO_MUX_GPIO8_MCU_WPD_S)
#define IO_MUX_GPIO8_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPD_S  2
/** IO_MUX_GPIO8_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO8_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO8_MCU_WPU_M  (IO_MUX_GPIO8_MCU_WPU_V << IO_MUX_GPIO8_MCU_WPU_S)
#define IO_MUX_GPIO8_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO8_MCU_WPU_S  3
/** IO_MUX_GPIO8_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO8_MCU_IE    (BIT(4))
#define IO_MUX_GPIO8_MCU_IE_M  (IO_MUX_GPIO8_MCU_IE_V << IO_MUX_GPIO8_MCU_IE_S)
#define IO_MUX_GPIO8_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO8_MCU_IE_S  4
/** IO_MUX_GPIO8_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO8_MCU_DRV    0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_M  (IO_MUX_GPIO8_MCU_DRV_V << IO_MUX_GPIO8_MCU_DRV_S)
#define IO_MUX_GPIO8_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO8_MCU_DRV_S  5
/** IO_MUX_GPIO8_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO8_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO8_FUN_WPD_M  (IO_MUX_GPIO8_FUN_WPD_V << IO_MUX_GPIO8_FUN_WPD_S)
#define IO_MUX_GPIO8_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPD_S  7
/** IO_MUX_GPIO8_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO8_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO8_FUN_WPU_M  (IO_MUX_GPIO8_FUN_WPU_V << IO_MUX_GPIO8_FUN_WPU_S)
#define IO_MUX_GPIO8_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO8_FUN_WPU_S  8
/** IO_MUX_GPIO8_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO8_FUN_IE    (BIT(9))
#define IO_MUX_GPIO8_FUN_IE_M  (IO_MUX_GPIO8_FUN_IE_V << IO_MUX_GPIO8_FUN_IE_S)
#define IO_MUX_GPIO8_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO8_FUN_IE_S  9
/** IO_MUX_GPIO8_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO8_FUN_DRV    0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_M  (IO_MUX_GPIO8_FUN_DRV_V << IO_MUX_GPIO8_FUN_DRV_S)
#define IO_MUX_GPIO8_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO8_FUN_DRV_S  10
/** IO_MUX_GPIO8_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO8_MCU_SEL    0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_M  (IO_MUX_GPIO8_MCU_SEL_V << IO_MUX_GPIO8_MCU_SEL_S)
#define IO_MUX_GPIO8_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO8_MCU_SEL_S  12
/** IO_MUX_GPIO8_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO8_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO8_FILTER_EN_M  (IO_MUX_GPIO8_FILTER_EN_V << IO_MUX_GPIO8_FILTER_EN_S)
#define IO_MUX_GPIO8_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO8_FILTER_EN_S  15
/** IO_MUX_GPIO8_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO8_HYS_EN    (BIT(16))
#define IO_MUX_GPIO8_HYS_EN_M  (IO_MUX_GPIO8_HYS_EN_V << IO_MUX_GPIO8_HYS_EN_S)
#define IO_MUX_GPIO8_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO8_HYS_EN_S  16
/** IO_MUX_GPIO8_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO8_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO8_HYS_SEL_M  (IO_MUX_GPIO8_HYS_SEL_V << IO_MUX_GPIO8_HYS_SEL_S)
#define IO_MUX_GPIO8_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO8_HYS_SEL_S  17

/** IO_MUX_GPIO9_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO9_REG (DR_REG_IO_MUX_BASE + 0x28)
/** IO_MUX_GPIO9_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO9_MCU_OE    (BIT(0))
#define IO_MUX_GPIO9_MCU_OE_M  (IO_MUX_GPIO9_MCU_OE_V << IO_MUX_GPIO9_MCU_OE_S)
#define IO_MUX_GPIO9_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_OE_S  0
/** IO_MUX_GPIO9_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO9_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO9_SLP_SEL_M  (IO_MUX_GPIO9_SLP_SEL_V << IO_MUX_GPIO9_SLP_SEL_S)
#define IO_MUX_GPIO9_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO9_SLP_SEL_S  1
/** IO_MUX_GPIO9_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO9_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO9_MCU_WPD_M  (IO_MUX_GPIO9_MCU_WPD_V << IO_MUX_GPIO9_MCU_WPD_S)
#define IO_MUX_GPIO9_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPD_S  2
/** IO_MUX_GPIO9_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO9_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO9_MCU_WPU_M  (IO_MUX_GPIO9_MCU_WPU_V << IO_MUX_GPIO9_MCU_WPU_S)
#define IO_MUX_GPIO9_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO9_MCU_WPU_S  3
/** IO_MUX_GPIO9_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO9_MCU_IE    (BIT(4))
#define IO_MUX_GPIO9_MCU_IE_M  (IO_MUX_GPIO9_MCU_IE_V << IO_MUX_GPIO9_MCU_IE_S)
#define IO_MUX_GPIO9_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO9_MCU_IE_S  4
/** IO_MUX_GPIO9_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO9_MCU_DRV    0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_M  (IO_MUX_GPIO9_MCU_DRV_V << IO_MUX_GPIO9_MCU_DRV_S)
#define IO_MUX_GPIO9_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO9_MCU_DRV_S  5
/** IO_MUX_GPIO9_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO9_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO9_FUN_WPD_M  (IO_MUX_GPIO9_FUN_WPD_V << IO_MUX_GPIO9_FUN_WPD_S)
#define IO_MUX_GPIO9_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPD_S  7
/** IO_MUX_GPIO9_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO9_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO9_FUN_WPU_M  (IO_MUX_GPIO9_FUN_WPU_V << IO_MUX_GPIO9_FUN_WPU_S)
#define IO_MUX_GPIO9_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO9_FUN_WPU_S  8
/** IO_MUX_GPIO9_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO9_FUN_IE    (BIT(9))
#define IO_MUX_GPIO9_FUN_IE_M  (IO_MUX_GPIO9_FUN_IE_V << IO_MUX_GPIO9_FUN_IE_S)
#define IO_MUX_GPIO9_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO9_FUN_IE_S  9
/** IO_MUX_GPIO9_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO9_FUN_DRV    0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_M  (IO_MUX_GPIO9_FUN_DRV_V << IO_MUX_GPIO9_FUN_DRV_S)
#define IO_MUX_GPIO9_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO9_FUN_DRV_S  10
/** IO_MUX_GPIO9_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO9_MCU_SEL    0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_M  (IO_MUX_GPIO9_MCU_SEL_V << IO_MUX_GPIO9_MCU_SEL_S)
#define IO_MUX_GPIO9_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO9_MCU_SEL_S  12
/** IO_MUX_GPIO9_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO9_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO9_FILTER_EN_M  (IO_MUX_GPIO9_FILTER_EN_V << IO_MUX_GPIO9_FILTER_EN_S)
#define IO_MUX_GPIO9_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO9_FILTER_EN_S  15
/** IO_MUX_GPIO9_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO9_HYS_EN    (BIT(16))
#define IO_MUX_GPIO9_HYS_EN_M  (IO_MUX_GPIO9_HYS_EN_V << IO_MUX_GPIO9_HYS_EN_S)
#define IO_MUX_GPIO9_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO9_HYS_EN_S  16
/** IO_MUX_GPIO9_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO9_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO9_HYS_SEL_M  (IO_MUX_GPIO9_HYS_SEL_V << IO_MUX_GPIO9_HYS_SEL_S)
#define IO_MUX_GPIO9_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO9_HYS_SEL_S  17

/** IO_MUX_GPIO10_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO10_REG (DR_REG_IO_MUX_BASE + 0x2c)
/** IO_MUX_GPIO10_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO10_MCU_OE    (BIT(0))
#define IO_MUX_GPIO10_MCU_OE_M  (IO_MUX_GPIO10_MCU_OE_V << IO_MUX_GPIO10_MCU_OE_S)
#define IO_MUX_GPIO10_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_OE_S  0
/** IO_MUX_GPIO10_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO10_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO10_SLP_SEL_M  (IO_MUX_GPIO10_SLP_SEL_V << IO_MUX_GPIO10_SLP_SEL_S)
#define IO_MUX_GPIO10_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO10_SLP_SEL_S  1
/** IO_MUX_GPIO10_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO10_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO10_MCU_WPD_M  (IO_MUX_GPIO10_MCU_WPD_V << IO_MUX_GPIO10_MCU_WPD_S)
#define IO_MUX_GPIO10_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPD_S  2
/** IO_MUX_GPIO10_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO10_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO10_MCU_WPU_M  (IO_MUX_GPIO10_MCU_WPU_V << IO_MUX_GPIO10_MCU_WPU_S)
#define IO_MUX_GPIO10_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO10_MCU_WPU_S  3
/** IO_MUX_GPIO10_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO10_MCU_IE    (BIT(4))
#define IO_MUX_GPIO10_MCU_IE_M  (IO_MUX_GPIO10_MCU_IE_V << IO_MUX_GPIO10_MCU_IE_S)
#define IO_MUX_GPIO10_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO10_MCU_IE_S  4
/** IO_MUX_GPIO10_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO10_MCU_DRV    0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_M  (IO_MUX_GPIO10_MCU_DRV_V << IO_MUX_GPIO10_MCU_DRV_S)
#define IO_MUX_GPIO10_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO10_MCU_DRV_S  5
/** IO_MUX_GPIO10_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO10_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO10_FUN_WPD_M  (IO_MUX_GPIO10_FUN_WPD_V << IO_MUX_GPIO10_FUN_WPD_S)
#define IO_MUX_GPIO10_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPD_S  7
/** IO_MUX_GPIO10_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO10_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO10_FUN_WPU_M  (IO_MUX_GPIO10_FUN_WPU_V << IO_MUX_GPIO10_FUN_WPU_S)
#define IO_MUX_GPIO10_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO10_FUN_WPU_S  8
/** IO_MUX_GPIO10_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO10_FUN_IE    (BIT(9))
#define IO_MUX_GPIO10_FUN_IE_M  (IO_MUX_GPIO10_FUN_IE_V << IO_MUX_GPIO10_FUN_IE_S)
#define IO_MUX_GPIO10_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO10_FUN_IE_S  9
/** IO_MUX_GPIO10_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO10_FUN_DRV    0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_M  (IO_MUX_GPIO10_FUN_DRV_V << IO_MUX_GPIO10_FUN_DRV_S)
#define IO_MUX_GPIO10_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO10_FUN_DRV_S  10
/** IO_MUX_GPIO10_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO10_MCU_SEL    0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_M  (IO_MUX_GPIO10_MCU_SEL_V << IO_MUX_GPIO10_MCU_SEL_S)
#define IO_MUX_GPIO10_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO10_MCU_SEL_S  12
/** IO_MUX_GPIO10_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO10_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO10_FILTER_EN_M  (IO_MUX_GPIO10_FILTER_EN_V << IO_MUX_GPIO10_FILTER_EN_S)
#define IO_MUX_GPIO10_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO10_FILTER_EN_S  15
/** IO_MUX_GPIO10_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO10_HYS_EN    (BIT(16))
#define IO_MUX_GPIO10_HYS_EN_M  (IO_MUX_GPIO10_HYS_EN_V << IO_MUX_GPIO10_HYS_EN_S)
#define IO_MUX_GPIO10_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO10_HYS_EN_S  16
/** IO_MUX_GPIO10_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO10_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO10_HYS_SEL_M  (IO_MUX_GPIO10_HYS_SEL_V << IO_MUX_GPIO10_HYS_SEL_S)
#define IO_MUX_GPIO10_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO10_HYS_SEL_S  17

/** IO_MUX_GPIO11_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO11_REG (DR_REG_IO_MUX_BASE + 0x30)
/** IO_MUX_GPIO11_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO11_MCU_OE    (BIT(0))
#define IO_MUX_GPIO11_MCU_OE_M  (IO_MUX_GPIO11_MCU_OE_V << IO_MUX_GPIO11_MCU_OE_S)
#define IO_MUX_GPIO11_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_OE_S  0
/** IO_MUX_GPIO11_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO11_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO11_SLP_SEL_M  (IO_MUX_GPIO11_SLP_SEL_V << IO_MUX_GPIO11_SLP_SEL_S)
#define IO_MUX_GPIO11_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO11_SLP_SEL_S  1
/** IO_MUX_GPIO11_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO11_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO11_MCU_WPD_M  (IO_MUX_GPIO11_MCU_WPD_V << IO_MUX_GPIO11_MCU_WPD_S)
#define IO_MUX_GPIO11_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPD_S  2
/** IO_MUX_GPIO11_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO11_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO11_MCU_WPU_M  (IO_MUX_GPIO11_MCU_WPU_V << IO_MUX_GPIO11_MCU_WPU_S)
#define IO_MUX_GPIO11_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO11_MCU_WPU_S  3
/** IO_MUX_GPIO11_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO11_MCU_IE    (BIT(4))
#define IO_MUX_GPIO11_MCU_IE_M  (IO_MUX_GPIO11_MCU_IE_V << IO_MUX_GPIO11_MCU_IE_S)
#define IO_MUX_GPIO11_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO11_MCU_IE_S  4
/** IO_MUX_GPIO11_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO11_MCU_DRV    0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_M  (IO_MUX_GPIO11_MCU_DRV_V << IO_MUX_GPIO11_MCU_DRV_S)
#define IO_MUX_GPIO11_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO11_MCU_DRV_S  5
/** IO_MUX_GPIO11_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO11_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO11_FUN_WPD_M  (IO_MUX_GPIO11_FUN_WPD_V << IO_MUX_GPIO11_FUN_WPD_S)
#define IO_MUX_GPIO11_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPD_S  7
/** IO_MUX_GPIO11_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO11_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO11_FUN_WPU_M  (IO_MUX_GPIO11_FUN_WPU_V << IO_MUX_GPIO11_FUN_WPU_S)
#define IO_MUX_GPIO11_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO11_FUN_WPU_S  8
/** IO_MUX_GPIO11_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO11_FUN_IE    (BIT(9))
#define IO_MUX_GPIO11_FUN_IE_M  (IO_MUX_GPIO11_FUN_IE_V << IO_MUX_GPIO11_FUN_IE_S)
#define IO_MUX_GPIO11_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO11_FUN_IE_S  9
/** IO_MUX_GPIO11_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO11_FUN_DRV    0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_M  (IO_MUX_GPIO11_FUN_DRV_V << IO_MUX_GPIO11_FUN_DRV_S)
#define IO_MUX_GPIO11_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO11_FUN_DRV_S  10
/** IO_MUX_GPIO11_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO11_MCU_SEL    0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_M  (IO_MUX_GPIO11_MCU_SEL_V << IO_MUX_GPIO11_MCU_SEL_S)
#define IO_MUX_GPIO11_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO11_MCU_SEL_S  12
/** IO_MUX_GPIO11_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO11_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO11_FILTER_EN_M  (IO_MUX_GPIO11_FILTER_EN_V << IO_MUX_GPIO11_FILTER_EN_S)
#define IO_MUX_GPIO11_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO11_FILTER_EN_S  15
/** IO_MUX_GPIO11_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO11_HYS_EN    (BIT(16))
#define IO_MUX_GPIO11_HYS_EN_M  (IO_MUX_GPIO11_HYS_EN_V << IO_MUX_GPIO11_HYS_EN_S)
#define IO_MUX_GPIO11_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO11_HYS_EN_S  16
/** IO_MUX_GPIO11_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO11_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO11_HYS_SEL_M  (IO_MUX_GPIO11_HYS_SEL_V << IO_MUX_GPIO11_HYS_SEL_S)
#define IO_MUX_GPIO11_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO11_HYS_SEL_S  17

/** IO_MUX_GPIO12_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO12_REG (DR_REG_IO_MUX_BASE + 0x34)
/** IO_MUX_GPIO12_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO12_MCU_OE    (BIT(0))
#define IO_MUX_GPIO12_MCU_OE_M  (IO_MUX_GPIO12_MCU_OE_V << IO_MUX_GPIO12_MCU_OE_S)
#define IO_MUX_GPIO12_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_OE_S  0
/** IO_MUX_GPIO12_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO12_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO12_SLP_SEL_M  (IO_MUX_GPIO12_SLP_SEL_V << IO_MUX_GPIO12_SLP_SEL_S)
#define IO_MUX_GPIO12_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO12_SLP_SEL_S  1
/** IO_MUX_GPIO12_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO12_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO12_MCU_WPD_M  (IO_MUX_GPIO12_MCU_WPD_V << IO_MUX_GPIO12_MCU_WPD_S)
#define IO_MUX_GPIO12_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPD_S  2
/** IO_MUX_GPIO12_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO12_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO12_MCU_WPU_M  (IO_MUX_GPIO12_MCU_WPU_V << IO_MUX_GPIO12_MCU_WPU_S)
#define IO_MUX_GPIO12_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO12_MCU_WPU_S  3
/** IO_MUX_GPIO12_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO12_MCU_IE    (BIT(4))
#define IO_MUX_GPIO12_MCU_IE_M  (IO_MUX_GPIO12_MCU_IE_V << IO_MUX_GPIO12_MCU_IE_S)
#define IO_MUX_GPIO12_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO12_MCU_IE_S  4
/** IO_MUX_GPIO12_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO12_MCU_DRV    0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_M  (IO_MUX_GPIO12_MCU_DRV_V << IO_MUX_GPIO12_MCU_DRV_S)
#define IO_MUX_GPIO12_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO12_MCU_DRV_S  5
/** IO_MUX_GPIO12_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO12_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO12_FUN_WPD_M  (IO_MUX_GPIO12_FUN_WPD_V << IO_MUX_GPIO12_FUN_WPD_S)
#define IO_MUX_GPIO12_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPD_S  7
/** IO_MUX_GPIO12_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO12_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO12_FUN_WPU_M  (IO_MUX_GPIO12_FUN_WPU_V << IO_MUX_GPIO12_FUN_WPU_S)
#define IO_MUX_GPIO12_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO12_FUN_WPU_S  8
/** IO_MUX_GPIO12_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO12_FUN_IE    (BIT(9))
#define IO_MUX_GPIO12_FUN_IE_M  (IO_MUX_GPIO12_FUN_IE_V << IO_MUX_GPIO12_FUN_IE_S)
#define IO_MUX_GPIO12_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO12_FUN_IE_S  9
/** IO_MUX_GPIO12_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO12_FUN_DRV    0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_M  (IO_MUX_GPIO12_FUN_DRV_V << IO_MUX_GPIO12_FUN_DRV_S)
#define IO_MUX_GPIO12_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO12_FUN_DRV_S  10
/** IO_MUX_GPIO12_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO12_MCU_SEL    0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_M  (IO_MUX_GPIO12_MCU_SEL_V << IO_MUX_GPIO12_MCU_SEL_S)
#define IO_MUX_GPIO12_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO12_MCU_SEL_S  12
/** IO_MUX_GPIO12_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO12_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO12_FILTER_EN_M  (IO_MUX_GPIO12_FILTER_EN_V << IO_MUX_GPIO12_FILTER_EN_S)
#define IO_MUX_GPIO12_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO12_FILTER_EN_S  15
/** IO_MUX_GPIO12_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO12_HYS_EN    (BIT(16))
#define IO_MUX_GPIO12_HYS_EN_M  (IO_MUX_GPIO12_HYS_EN_V << IO_MUX_GPIO12_HYS_EN_S)
#define IO_MUX_GPIO12_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO12_HYS_EN_S  16
/** IO_MUX_GPIO12_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO12_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO12_HYS_SEL_M  (IO_MUX_GPIO12_HYS_SEL_V << IO_MUX_GPIO12_HYS_SEL_S)
#define IO_MUX_GPIO12_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO12_HYS_SEL_S  17

/** IO_MUX_GPIO13_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO13_REG (DR_REG_IO_MUX_BASE + 0x38)
/** IO_MUX_GPIO13_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO13_MCU_OE    (BIT(0))
#define IO_MUX_GPIO13_MCU_OE_M  (IO_MUX_GPIO13_MCU_OE_V << IO_MUX_GPIO13_MCU_OE_S)
#define IO_MUX_GPIO13_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_OE_S  0
/** IO_MUX_GPIO13_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO13_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO13_SLP_SEL_M  (IO_MUX_GPIO13_SLP_SEL_V << IO_MUX_GPIO13_SLP_SEL_S)
#define IO_MUX_GPIO13_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO13_SLP_SEL_S  1
/** IO_MUX_GPIO13_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO13_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO13_MCU_WPD_M  (IO_MUX_GPIO13_MCU_WPD_V << IO_MUX_GPIO13_MCU_WPD_S)
#define IO_MUX_GPIO13_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPD_S  2
/** IO_MUX_GPIO13_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO13_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO13_MCU_WPU_M  (IO_MUX_GPIO13_MCU_WPU_V << IO_MUX_GPIO13_MCU_WPU_S)
#define IO_MUX_GPIO13_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO13_MCU_WPU_S  3
/** IO_MUX_GPIO13_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO13_MCU_IE    (BIT(4))
#define IO_MUX_GPIO13_MCU_IE_M  (IO_MUX_GPIO13_MCU_IE_V << IO_MUX_GPIO13_MCU_IE_S)
#define IO_MUX_GPIO13_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO13_MCU_IE_S  4
/** IO_MUX_GPIO13_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO13_MCU_DRV    0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_M  (IO_MUX_GPIO13_MCU_DRV_V << IO_MUX_GPIO13_MCU_DRV_S)
#define IO_MUX_GPIO13_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO13_MCU_DRV_S  5
/** IO_MUX_GPIO13_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO13_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO13_FUN_WPD_M  (IO_MUX_GPIO13_FUN_WPD_V << IO_MUX_GPIO13_FUN_WPD_S)
#define IO_MUX_GPIO13_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPD_S  7
/** IO_MUX_GPIO13_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO13_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO13_FUN_WPU_M  (IO_MUX_GPIO13_FUN_WPU_V << IO_MUX_GPIO13_FUN_WPU_S)
#define IO_MUX_GPIO13_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO13_FUN_WPU_S  8
/** IO_MUX_GPIO13_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO13_FUN_IE    (BIT(9))
#define IO_MUX_GPIO13_FUN_IE_M  (IO_MUX_GPIO13_FUN_IE_V << IO_MUX_GPIO13_FUN_IE_S)
#define IO_MUX_GPIO13_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO13_FUN_IE_S  9
/** IO_MUX_GPIO13_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO13_FUN_DRV    0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_M  (IO_MUX_GPIO13_FUN_DRV_V << IO_MUX_GPIO13_FUN_DRV_S)
#define IO_MUX_GPIO13_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO13_FUN_DRV_S  10
/** IO_MUX_GPIO13_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO13_MCU_SEL    0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_M  (IO_MUX_GPIO13_MCU_SEL_V << IO_MUX_GPIO13_MCU_SEL_S)
#define IO_MUX_GPIO13_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO13_MCU_SEL_S  12
/** IO_MUX_GPIO13_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO13_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO13_FILTER_EN_M  (IO_MUX_GPIO13_FILTER_EN_V << IO_MUX_GPIO13_FILTER_EN_S)
#define IO_MUX_GPIO13_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO13_FILTER_EN_S  15
/** IO_MUX_GPIO13_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO13_HYS_EN    (BIT(16))
#define IO_MUX_GPIO13_HYS_EN_M  (IO_MUX_GPIO13_HYS_EN_V << IO_MUX_GPIO13_HYS_EN_S)
#define IO_MUX_GPIO13_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO13_HYS_EN_S  16
/** IO_MUX_GPIO13_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO13_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO13_HYS_SEL_M  (IO_MUX_GPIO13_HYS_SEL_V << IO_MUX_GPIO13_HYS_SEL_S)
#define IO_MUX_GPIO13_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO13_HYS_SEL_S  17

/** IO_MUX_GPIO14_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO14_REG (DR_REG_IO_MUX_BASE + 0x3c)
/** IO_MUX_GPIO14_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO14_MCU_OE    (BIT(0))
#define IO_MUX_GPIO14_MCU_OE_M  (IO_MUX_GPIO14_MCU_OE_V << IO_MUX_GPIO14_MCU_OE_S)
#define IO_MUX_GPIO14_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_OE_S  0
/** IO_MUX_GPIO14_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO14_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO14_SLP_SEL_M  (IO_MUX_GPIO14_SLP_SEL_V << IO_MUX_GPIO14_SLP_SEL_S)
#define IO_MUX_GPIO14_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO14_SLP_SEL_S  1
/** IO_MUX_GPIO14_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO14_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO14_MCU_WPD_M  (IO_MUX_GPIO14_MCU_WPD_V << IO_MUX_GPIO14_MCU_WPD_S)
#define IO_MUX_GPIO14_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPD_S  2
/** IO_MUX_GPIO14_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO14_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO14_MCU_WPU_M  (IO_MUX_GPIO14_MCU_WPU_V << IO_MUX_GPIO14_MCU_WPU_S)
#define IO_MUX_GPIO14_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO14_MCU_WPU_S  3
/** IO_MUX_GPIO14_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO14_MCU_IE    (BIT(4))
#define IO_MUX_GPIO14_MCU_IE_M  (IO_MUX_GPIO14_MCU_IE_V << IO_MUX_GPIO14_MCU_IE_S)
#define IO_MUX_GPIO14_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO14_MCU_IE_S  4
/** IO_MUX_GPIO14_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO14_MCU_DRV    0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_M  (IO_MUX_GPIO14_MCU_DRV_V << IO_MUX_GPIO14_MCU_DRV_S)
#define IO_MUX_GPIO14_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO14_MCU_DRV_S  5
/** IO_MUX_GPIO14_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO14_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO14_FUN_WPD_M  (IO_MUX_GPIO14_FUN_WPD_V << IO_MUX_GPIO14_FUN_WPD_S)
#define IO_MUX_GPIO14_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPD_S  7
/** IO_MUX_GPIO14_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO14_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO14_FUN_WPU_M  (IO_MUX_GPIO14_FUN_WPU_V << IO_MUX_GPIO14_FUN_WPU_S)
#define IO_MUX_GPIO14_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO14_FUN_WPU_S  8
/** IO_MUX_GPIO14_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO14_FUN_IE    (BIT(9))
#define IO_MUX_GPIO14_FUN_IE_M  (IO_MUX_GPIO14_FUN_IE_V << IO_MUX_GPIO14_FUN_IE_S)
#define IO_MUX_GPIO14_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO14_FUN_IE_S  9
/** IO_MUX_GPIO14_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO14_FUN_DRV    0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_M  (IO_MUX_GPIO14_FUN_DRV_V << IO_MUX_GPIO14_FUN_DRV_S)
#define IO_MUX_GPIO14_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO14_FUN_DRV_S  10
/** IO_MUX_GPIO14_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO14_MCU_SEL    0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_M  (IO_MUX_GPIO14_MCU_SEL_V << IO_MUX_GPIO14_MCU_SEL_S)
#define IO_MUX_GPIO14_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO14_MCU_SEL_S  12
/** IO_MUX_GPIO14_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO14_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO14_FILTER_EN_M  (IO_MUX_GPIO14_FILTER_EN_V << IO_MUX_GPIO14_FILTER_EN_S)
#define IO_MUX_GPIO14_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO14_FILTER_EN_S  15
/** IO_MUX_GPIO14_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO14_HYS_EN    (BIT(16))
#define IO_MUX_GPIO14_HYS_EN_M  (IO_MUX_GPIO14_HYS_EN_V << IO_MUX_GPIO14_HYS_EN_S)
#define IO_MUX_GPIO14_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO14_HYS_EN_S  16
/** IO_MUX_GPIO14_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO14_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO14_HYS_SEL_M  (IO_MUX_GPIO14_HYS_SEL_V << IO_MUX_GPIO14_HYS_SEL_S)
#define IO_MUX_GPIO14_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO14_HYS_SEL_S  17

/** IO_MUX_GPIO15_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO15_REG (DR_REG_IO_MUX_BASE + 0x40)
/** IO_MUX_GPIO15_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO15_MCU_OE    (BIT(0))
#define IO_MUX_GPIO15_MCU_OE_M  (IO_MUX_GPIO15_MCU_OE_V << IO_MUX_GPIO15_MCU_OE_S)
#define IO_MUX_GPIO15_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_OE_S  0
/** IO_MUX_GPIO15_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO15_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO15_SLP_SEL_M  (IO_MUX_GPIO15_SLP_SEL_V << IO_MUX_GPIO15_SLP_SEL_S)
#define IO_MUX_GPIO15_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO15_SLP_SEL_S  1
/** IO_MUX_GPIO15_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO15_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO15_MCU_WPD_M  (IO_MUX_GPIO15_MCU_WPD_V << IO_MUX_GPIO15_MCU_WPD_S)
#define IO_MUX_GPIO15_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPD_S  2
/** IO_MUX_GPIO15_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO15_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO15_MCU_WPU_M  (IO_MUX_GPIO15_MCU_WPU_V << IO_MUX_GPIO15_MCU_WPU_S)
#define IO_MUX_GPIO15_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO15_MCU_WPU_S  3
/** IO_MUX_GPIO15_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO15_MCU_IE    (BIT(4))
#define IO_MUX_GPIO15_MCU_IE_M  (IO_MUX_GPIO15_MCU_IE_V << IO_MUX_GPIO15_MCU_IE_S)
#define IO_MUX_GPIO15_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO15_MCU_IE_S  4
/** IO_MUX_GPIO15_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO15_MCU_DRV    0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_M  (IO_MUX_GPIO15_MCU_DRV_V << IO_MUX_GPIO15_MCU_DRV_S)
#define IO_MUX_GPIO15_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO15_MCU_DRV_S  5
/** IO_MUX_GPIO15_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO15_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO15_FUN_WPD_M  (IO_MUX_GPIO15_FUN_WPD_V << IO_MUX_GPIO15_FUN_WPD_S)
#define IO_MUX_GPIO15_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPD_S  7
/** IO_MUX_GPIO15_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO15_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO15_FUN_WPU_M  (IO_MUX_GPIO15_FUN_WPU_V << IO_MUX_GPIO15_FUN_WPU_S)
#define IO_MUX_GPIO15_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO15_FUN_WPU_S  8
/** IO_MUX_GPIO15_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO15_FUN_IE    (BIT(9))
#define IO_MUX_GPIO15_FUN_IE_M  (IO_MUX_GPIO15_FUN_IE_V << IO_MUX_GPIO15_FUN_IE_S)
#define IO_MUX_GPIO15_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO15_FUN_IE_S  9
/** IO_MUX_GPIO15_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO15_FUN_DRV    0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_M  (IO_MUX_GPIO15_FUN_DRV_V << IO_MUX_GPIO15_FUN_DRV_S)
#define IO_MUX_GPIO15_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO15_FUN_DRV_S  10
/** IO_MUX_GPIO15_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO15_MCU_SEL    0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_M  (IO_MUX_GPIO15_MCU_SEL_V << IO_MUX_GPIO15_MCU_SEL_S)
#define IO_MUX_GPIO15_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO15_MCU_SEL_S  12
/** IO_MUX_GPIO15_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO15_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO15_FILTER_EN_M  (IO_MUX_GPIO15_FILTER_EN_V << IO_MUX_GPIO15_FILTER_EN_S)
#define IO_MUX_GPIO15_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO15_FILTER_EN_S  15
/** IO_MUX_GPIO15_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO15_HYS_EN    (BIT(16))
#define IO_MUX_GPIO15_HYS_EN_M  (IO_MUX_GPIO15_HYS_EN_V << IO_MUX_GPIO15_HYS_EN_S)
#define IO_MUX_GPIO15_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO15_HYS_EN_S  16
/** IO_MUX_GPIO15_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO15_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO15_HYS_SEL_M  (IO_MUX_GPIO15_HYS_SEL_V << IO_MUX_GPIO15_HYS_SEL_S)
#define IO_MUX_GPIO15_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO15_HYS_SEL_S  17

/** IO_MUX_GPIO16_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO16_REG (DR_REG_IO_MUX_BASE + 0x44)
/** IO_MUX_GPIO16_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO16_MCU_OE    (BIT(0))
#define IO_MUX_GPIO16_MCU_OE_M  (IO_MUX_GPIO16_MCU_OE_V << IO_MUX_GPIO16_MCU_OE_S)
#define IO_MUX_GPIO16_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_OE_S  0
/** IO_MUX_GPIO16_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO16_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO16_SLP_SEL_M  (IO_MUX_GPIO16_SLP_SEL_V << IO_MUX_GPIO16_SLP_SEL_S)
#define IO_MUX_GPIO16_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO16_SLP_SEL_S  1
/** IO_MUX_GPIO16_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO16_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO16_MCU_WPD_M  (IO_MUX_GPIO16_MCU_WPD_V << IO_MUX_GPIO16_MCU_WPD_S)
#define IO_MUX_GPIO16_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPD_S  2
/** IO_MUX_GPIO16_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO16_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO16_MCU_WPU_M  (IO_MUX_GPIO16_MCU_WPU_V << IO_MUX_GPIO16_MCU_WPU_S)
#define IO_MUX_GPIO16_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO16_MCU_WPU_S  3
/** IO_MUX_GPIO16_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO16_MCU_IE    (BIT(4))
#define IO_MUX_GPIO16_MCU_IE_M  (IO_MUX_GPIO16_MCU_IE_V << IO_MUX_GPIO16_MCU_IE_S)
#define IO_MUX_GPIO16_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO16_MCU_IE_S  4
/** IO_MUX_GPIO16_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO16_MCU_DRV    0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_M  (IO_MUX_GPIO16_MCU_DRV_V << IO_MUX_GPIO16_MCU_DRV_S)
#define IO_MUX_GPIO16_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO16_MCU_DRV_S  5
/** IO_MUX_GPIO16_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO16_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO16_FUN_WPD_M  (IO_MUX_GPIO16_FUN_WPD_V << IO_MUX_GPIO16_FUN_WPD_S)
#define IO_MUX_GPIO16_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPD_S  7
/** IO_MUX_GPIO16_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO16_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO16_FUN_WPU_M  (IO_MUX_GPIO16_FUN_WPU_V << IO_MUX_GPIO16_FUN_WPU_S)
#define IO_MUX_GPIO16_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO16_FUN_WPU_S  8
/** IO_MUX_GPIO16_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO16_FUN_IE    (BIT(9))
#define IO_MUX_GPIO16_FUN_IE_M  (IO_MUX_GPIO16_FUN_IE_V << IO_MUX_GPIO16_FUN_IE_S)
#define IO_MUX_GPIO16_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO16_FUN_IE_S  9
/** IO_MUX_GPIO16_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO16_FUN_DRV    0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_M  (IO_MUX_GPIO16_FUN_DRV_V << IO_MUX_GPIO16_FUN_DRV_S)
#define IO_MUX_GPIO16_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO16_FUN_DRV_S  10
/** IO_MUX_GPIO16_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO16_MCU_SEL    0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_M  (IO_MUX_GPIO16_MCU_SEL_V << IO_MUX_GPIO16_MCU_SEL_S)
#define IO_MUX_GPIO16_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO16_MCU_SEL_S  12
/** IO_MUX_GPIO16_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO16_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO16_FILTER_EN_M  (IO_MUX_GPIO16_FILTER_EN_V << IO_MUX_GPIO16_FILTER_EN_S)
#define IO_MUX_GPIO16_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO16_FILTER_EN_S  15
/** IO_MUX_GPIO16_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO16_HYS_EN    (BIT(16))
#define IO_MUX_GPIO16_HYS_EN_M  (IO_MUX_GPIO16_HYS_EN_V << IO_MUX_GPIO16_HYS_EN_S)
#define IO_MUX_GPIO16_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO16_HYS_EN_S  16
/** IO_MUX_GPIO16_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO16_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO16_HYS_SEL_M  (IO_MUX_GPIO16_HYS_SEL_V << IO_MUX_GPIO16_HYS_SEL_S)
#define IO_MUX_GPIO16_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO16_HYS_SEL_S  17

/** IO_MUX_GPIO17_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO17_REG (DR_REG_IO_MUX_BASE + 0x48)
/** IO_MUX_GPIO17_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO17_MCU_OE    (BIT(0))
#define IO_MUX_GPIO17_MCU_OE_M  (IO_MUX_GPIO17_MCU_OE_V << IO_MUX_GPIO17_MCU_OE_S)
#define IO_MUX_GPIO17_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_OE_S  0
/** IO_MUX_GPIO17_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO17_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO17_SLP_SEL_M  (IO_MUX_GPIO17_SLP_SEL_V << IO_MUX_GPIO17_SLP_SEL_S)
#define IO_MUX_GPIO17_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO17_SLP_SEL_S  1
/** IO_MUX_GPIO17_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO17_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO17_MCU_WPD_M  (IO_MUX_GPIO17_MCU_WPD_V << IO_MUX_GPIO17_MCU_WPD_S)
#define IO_MUX_GPIO17_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPD_S  2
/** IO_MUX_GPIO17_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO17_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO17_MCU_WPU_M  (IO_MUX_GPIO17_MCU_WPU_V << IO_MUX_GPIO17_MCU_WPU_S)
#define IO_MUX_GPIO17_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO17_MCU_WPU_S  3
/** IO_MUX_GPIO17_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO17_MCU_IE    (BIT(4))
#define IO_MUX_GPIO17_MCU_IE_M  (IO_MUX_GPIO17_MCU_IE_V << IO_MUX_GPIO17_MCU_IE_S)
#define IO_MUX_GPIO17_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO17_MCU_IE_S  4
/** IO_MUX_GPIO17_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO17_MCU_DRV    0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_M  (IO_MUX_GPIO17_MCU_DRV_V << IO_MUX_GPIO17_MCU_DRV_S)
#define IO_MUX_GPIO17_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO17_MCU_DRV_S  5
/** IO_MUX_GPIO17_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO17_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO17_FUN_WPD_M  (IO_MUX_GPIO17_FUN_WPD_V << IO_MUX_GPIO17_FUN_WPD_S)
#define IO_MUX_GPIO17_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPD_S  7
/** IO_MUX_GPIO17_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO17_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO17_FUN_WPU_M  (IO_MUX_GPIO17_FUN_WPU_V << IO_MUX_GPIO17_FUN_WPU_S)
#define IO_MUX_GPIO17_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO17_FUN_WPU_S  8
/** IO_MUX_GPIO17_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO17_FUN_IE    (BIT(9))
#define IO_MUX_GPIO17_FUN_IE_M  (IO_MUX_GPIO17_FUN_IE_V << IO_MUX_GPIO17_FUN_IE_S)
#define IO_MUX_GPIO17_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO17_FUN_IE_S  9
/** IO_MUX_GPIO17_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO17_FUN_DRV    0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_M  (IO_MUX_GPIO17_FUN_DRV_V << IO_MUX_GPIO17_FUN_DRV_S)
#define IO_MUX_GPIO17_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO17_FUN_DRV_S  10
/** IO_MUX_GPIO17_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO17_MCU_SEL    0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_M  (IO_MUX_GPIO17_MCU_SEL_V << IO_MUX_GPIO17_MCU_SEL_S)
#define IO_MUX_GPIO17_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO17_MCU_SEL_S  12
/** IO_MUX_GPIO17_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO17_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO17_FILTER_EN_M  (IO_MUX_GPIO17_FILTER_EN_V << IO_MUX_GPIO17_FILTER_EN_S)
#define IO_MUX_GPIO17_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO17_FILTER_EN_S  15
/** IO_MUX_GPIO17_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO17_HYS_EN    (BIT(16))
#define IO_MUX_GPIO17_HYS_EN_M  (IO_MUX_GPIO17_HYS_EN_V << IO_MUX_GPIO17_HYS_EN_S)
#define IO_MUX_GPIO17_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO17_HYS_EN_S  16
/** IO_MUX_GPIO17_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO17_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO17_HYS_SEL_M  (IO_MUX_GPIO17_HYS_SEL_V << IO_MUX_GPIO17_HYS_SEL_S)
#define IO_MUX_GPIO17_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO17_HYS_SEL_S  17

/** IO_MUX_GPIO18_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO18_REG (DR_REG_IO_MUX_BASE + 0x4c)
/** IO_MUX_GPIO18_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO18_MCU_OE    (BIT(0))
#define IO_MUX_GPIO18_MCU_OE_M  (IO_MUX_GPIO18_MCU_OE_V << IO_MUX_GPIO18_MCU_OE_S)
#define IO_MUX_GPIO18_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_OE_S  0
/** IO_MUX_GPIO18_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO18_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO18_SLP_SEL_M  (IO_MUX_GPIO18_SLP_SEL_V << IO_MUX_GPIO18_SLP_SEL_S)
#define IO_MUX_GPIO18_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO18_SLP_SEL_S  1
/** IO_MUX_GPIO18_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO18_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO18_MCU_WPD_M  (IO_MUX_GPIO18_MCU_WPD_V << IO_MUX_GPIO18_MCU_WPD_S)
#define IO_MUX_GPIO18_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPD_S  2
/** IO_MUX_GPIO18_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO18_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO18_MCU_WPU_M  (IO_MUX_GPIO18_MCU_WPU_V << IO_MUX_GPIO18_MCU_WPU_S)
#define IO_MUX_GPIO18_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO18_MCU_WPU_S  3
/** IO_MUX_GPIO18_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO18_MCU_IE    (BIT(4))
#define IO_MUX_GPIO18_MCU_IE_M  (IO_MUX_GPIO18_MCU_IE_V << IO_MUX_GPIO18_MCU_IE_S)
#define IO_MUX_GPIO18_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO18_MCU_IE_S  4
/** IO_MUX_GPIO18_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO18_MCU_DRV    0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_M  (IO_MUX_GPIO18_MCU_DRV_V << IO_MUX_GPIO18_MCU_DRV_S)
#define IO_MUX_GPIO18_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO18_MCU_DRV_S  5
/** IO_MUX_GPIO18_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO18_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO18_FUN_WPD_M  (IO_MUX_GPIO18_FUN_WPD_V << IO_MUX_GPIO18_FUN_WPD_S)
#define IO_MUX_GPIO18_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPD_S  7
/** IO_MUX_GPIO18_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO18_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO18_FUN_WPU_M  (IO_MUX_GPIO18_FUN_WPU_V << IO_MUX_GPIO18_FUN_WPU_S)
#define IO_MUX_GPIO18_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO18_FUN_WPU_S  8
/** IO_MUX_GPIO18_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO18_FUN_IE    (BIT(9))
#define IO_MUX_GPIO18_FUN_IE_M  (IO_MUX_GPIO18_FUN_IE_V << IO_MUX_GPIO18_FUN_IE_S)
#define IO_MUX_GPIO18_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO18_FUN_IE_S  9
/** IO_MUX_GPIO18_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO18_FUN_DRV    0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_M  (IO_MUX_GPIO18_FUN_DRV_V << IO_MUX_GPIO18_FUN_DRV_S)
#define IO_MUX_GPIO18_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO18_FUN_DRV_S  10
/** IO_MUX_GPIO18_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO18_MCU_SEL    0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_M  (IO_MUX_GPIO18_MCU_SEL_V << IO_MUX_GPIO18_MCU_SEL_S)
#define IO_MUX_GPIO18_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO18_MCU_SEL_S  12
/** IO_MUX_GPIO18_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO18_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO18_FILTER_EN_M  (IO_MUX_GPIO18_FILTER_EN_V << IO_MUX_GPIO18_FILTER_EN_S)
#define IO_MUX_GPIO18_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO18_FILTER_EN_S  15
/** IO_MUX_GPIO18_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO18_HYS_EN    (BIT(16))
#define IO_MUX_GPIO18_HYS_EN_M  (IO_MUX_GPIO18_HYS_EN_V << IO_MUX_GPIO18_HYS_EN_S)
#define IO_MUX_GPIO18_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO18_HYS_EN_S  16
/** IO_MUX_GPIO18_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO18_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO18_HYS_SEL_M  (IO_MUX_GPIO18_HYS_SEL_V << IO_MUX_GPIO18_HYS_SEL_S)
#define IO_MUX_GPIO18_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO18_HYS_SEL_S  17

/** IO_MUX_GPIO19_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO19_REG (DR_REG_IO_MUX_BASE + 0x50)
/** IO_MUX_GPIO19_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO19_MCU_OE    (BIT(0))
#define IO_MUX_GPIO19_MCU_OE_M  (IO_MUX_GPIO19_MCU_OE_V << IO_MUX_GPIO19_MCU_OE_S)
#define IO_MUX_GPIO19_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_OE_S  0
/** IO_MUX_GPIO19_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO19_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO19_SLP_SEL_M  (IO_MUX_GPIO19_SLP_SEL_V << IO_MUX_GPIO19_SLP_SEL_S)
#define IO_MUX_GPIO19_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO19_SLP_SEL_S  1
/** IO_MUX_GPIO19_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO19_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO19_MCU_WPD_M  (IO_MUX_GPIO19_MCU_WPD_V << IO_MUX_GPIO19_MCU_WPD_S)
#define IO_MUX_GPIO19_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPD_S  2
/** IO_MUX_GPIO19_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO19_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO19_MCU_WPU_M  (IO_MUX_GPIO19_MCU_WPU_V << IO_MUX_GPIO19_MCU_WPU_S)
#define IO_MUX_GPIO19_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO19_MCU_WPU_S  3
/** IO_MUX_GPIO19_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO19_MCU_IE    (BIT(4))
#define IO_MUX_GPIO19_MCU_IE_M  (IO_MUX_GPIO19_MCU_IE_V << IO_MUX_GPIO19_MCU_IE_S)
#define IO_MUX_GPIO19_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO19_MCU_IE_S  4
/** IO_MUX_GPIO19_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO19_MCU_DRV    0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_M  (IO_MUX_GPIO19_MCU_DRV_V << IO_MUX_GPIO19_MCU_DRV_S)
#define IO_MUX_GPIO19_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO19_MCU_DRV_S  5
/** IO_MUX_GPIO19_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO19_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO19_FUN_WPD_M  (IO_MUX_GPIO19_FUN_WPD_V << IO_MUX_GPIO19_FUN_WPD_S)
#define IO_MUX_GPIO19_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPD_S  7
/** IO_MUX_GPIO19_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO19_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO19_FUN_WPU_M  (IO_MUX_GPIO19_FUN_WPU_V << IO_MUX_GPIO19_FUN_WPU_S)
#define IO_MUX_GPIO19_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO19_FUN_WPU_S  8
/** IO_MUX_GPIO19_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO19_FUN_IE    (BIT(9))
#define IO_MUX_GPIO19_FUN_IE_M  (IO_MUX_GPIO19_FUN_IE_V << IO_MUX_GPIO19_FUN_IE_S)
#define IO_MUX_GPIO19_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO19_FUN_IE_S  9
/** IO_MUX_GPIO19_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO19_FUN_DRV    0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_M  (IO_MUX_GPIO19_FUN_DRV_V << IO_MUX_GPIO19_FUN_DRV_S)
#define IO_MUX_GPIO19_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO19_FUN_DRV_S  10
/** IO_MUX_GPIO19_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO19_MCU_SEL    0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_M  (IO_MUX_GPIO19_MCU_SEL_V << IO_MUX_GPIO19_MCU_SEL_S)
#define IO_MUX_GPIO19_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO19_MCU_SEL_S  12
/** IO_MUX_GPIO19_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO19_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO19_FILTER_EN_M  (IO_MUX_GPIO19_FILTER_EN_V << IO_MUX_GPIO19_FILTER_EN_S)
#define IO_MUX_GPIO19_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO19_FILTER_EN_S  15
/** IO_MUX_GPIO19_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO19_HYS_EN    (BIT(16))
#define IO_MUX_GPIO19_HYS_EN_M  (IO_MUX_GPIO19_HYS_EN_V << IO_MUX_GPIO19_HYS_EN_S)
#define IO_MUX_GPIO19_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO19_HYS_EN_S  16
/** IO_MUX_GPIO19_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO19_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO19_HYS_SEL_M  (IO_MUX_GPIO19_HYS_SEL_V << IO_MUX_GPIO19_HYS_SEL_S)
#define IO_MUX_GPIO19_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO19_HYS_SEL_S  17

/** IO_MUX_GPIO20_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO20_REG (DR_REG_IO_MUX_BASE + 0x54)
/** IO_MUX_GPIO20_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO20_MCU_OE    (BIT(0))
#define IO_MUX_GPIO20_MCU_OE_M  (IO_MUX_GPIO20_MCU_OE_V << IO_MUX_GPIO20_MCU_OE_S)
#define IO_MUX_GPIO20_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_OE_S  0
/** IO_MUX_GPIO20_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO20_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO20_SLP_SEL_M  (IO_MUX_GPIO20_SLP_SEL_V << IO_MUX_GPIO20_SLP_SEL_S)
#define IO_MUX_GPIO20_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO20_SLP_SEL_S  1
/** IO_MUX_GPIO20_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO20_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO20_MCU_WPD_M  (IO_MUX_GPIO20_MCU_WPD_V << IO_MUX_GPIO20_MCU_WPD_S)
#define IO_MUX_GPIO20_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPD_S  2
/** IO_MUX_GPIO20_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO20_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO20_MCU_WPU_M  (IO_MUX_GPIO20_MCU_WPU_V << IO_MUX_GPIO20_MCU_WPU_S)
#define IO_MUX_GPIO20_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO20_MCU_WPU_S  3
/** IO_MUX_GPIO20_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO20_MCU_IE    (BIT(4))
#define IO_MUX_GPIO20_MCU_IE_M  (IO_MUX_GPIO20_MCU_IE_V << IO_MUX_GPIO20_MCU_IE_S)
#define IO_MUX_GPIO20_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO20_MCU_IE_S  4
/** IO_MUX_GPIO20_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO20_MCU_DRV    0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_M  (IO_MUX_GPIO20_MCU_DRV_V << IO_MUX_GPIO20_MCU_DRV_S)
#define IO_MUX_GPIO20_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO20_MCU_DRV_S  5
/** IO_MUX_GPIO20_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO20_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO20_FUN_WPD_M  (IO_MUX_GPIO20_FUN_WPD_V << IO_MUX_GPIO20_FUN_WPD_S)
#define IO_MUX_GPIO20_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPD_S  7
/** IO_MUX_GPIO20_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO20_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO20_FUN_WPU_M  (IO_MUX_GPIO20_FUN_WPU_V << IO_MUX_GPIO20_FUN_WPU_S)
#define IO_MUX_GPIO20_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO20_FUN_WPU_S  8
/** IO_MUX_GPIO20_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO20_FUN_IE    (BIT(9))
#define IO_MUX_GPIO20_FUN_IE_M  (IO_MUX_GPIO20_FUN_IE_V << IO_MUX_GPIO20_FUN_IE_S)
#define IO_MUX_GPIO20_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO20_FUN_IE_S  9
/** IO_MUX_GPIO20_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO20_FUN_DRV    0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_M  (IO_MUX_GPIO20_FUN_DRV_V << IO_MUX_GPIO20_FUN_DRV_S)
#define IO_MUX_GPIO20_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO20_FUN_DRV_S  10
/** IO_MUX_GPIO20_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO20_MCU_SEL    0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_M  (IO_MUX_GPIO20_MCU_SEL_V << IO_MUX_GPIO20_MCU_SEL_S)
#define IO_MUX_GPIO20_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO20_MCU_SEL_S  12
/** IO_MUX_GPIO20_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO20_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO20_FILTER_EN_M  (IO_MUX_GPIO20_FILTER_EN_V << IO_MUX_GPIO20_FILTER_EN_S)
#define IO_MUX_GPIO20_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO20_FILTER_EN_S  15
/** IO_MUX_GPIO20_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO20_HYS_EN    (BIT(16))
#define IO_MUX_GPIO20_HYS_EN_M  (IO_MUX_GPIO20_HYS_EN_V << IO_MUX_GPIO20_HYS_EN_S)
#define IO_MUX_GPIO20_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO20_HYS_EN_S  16
/** IO_MUX_GPIO20_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO20_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO20_HYS_SEL_M  (IO_MUX_GPIO20_HYS_SEL_V << IO_MUX_GPIO20_HYS_SEL_S)
#define IO_MUX_GPIO20_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO20_HYS_SEL_S  17

/** IO_MUX_GPIO21_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO21_REG (DR_REG_IO_MUX_BASE + 0x58)
/** IO_MUX_GPIO21_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO21_MCU_OE    (BIT(0))
#define IO_MUX_GPIO21_MCU_OE_M  (IO_MUX_GPIO21_MCU_OE_V << IO_MUX_GPIO21_MCU_OE_S)
#define IO_MUX_GPIO21_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_OE_S  0
/** IO_MUX_GPIO21_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO21_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO21_SLP_SEL_M  (IO_MUX_GPIO21_SLP_SEL_V << IO_MUX_GPIO21_SLP_SEL_S)
#define IO_MUX_GPIO21_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO21_SLP_SEL_S  1
/** IO_MUX_GPIO21_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO21_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO21_MCU_WPD_M  (IO_MUX_GPIO21_MCU_WPD_V << IO_MUX_GPIO21_MCU_WPD_S)
#define IO_MUX_GPIO21_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPD_S  2
/** IO_MUX_GPIO21_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO21_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO21_MCU_WPU_M  (IO_MUX_GPIO21_MCU_WPU_V << IO_MUX_GPIO21_MCU_WPU_S)
#define IO_MUX_GPIO21_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO21_MCU_WPU_S  3
/** IO_MUX_GPIO21_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO21_MCU_IE    (BIT(4))
#define IO_MUX_GPIO21_MCU_IE_M  (IO_MUX_GPIO21_MCU_IE_V << IO_MUX_GPIO21_MCU_IE_S)
#define IO_MUX_GPIO21_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO21_MCU_IE_S  4
/** IO_MUX_GPIO21_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO21_MCU_DRV    0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_M  (IO_MUX_GPIO21_MCU_DRV_V << IO_MUX_GPIO21_MCU_DRV_S)
#define IO_MUX_GPIO21_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO21_MCU_DRV_S  5
/** IO_MUX_GPIO21_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO21_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO21_FUN_WPD_M  (IO_MUX_GPIO21_FUN_WPD_V << IO_MUX_GPIO21_FUN_WPD_S)
#define IO_MUX_GPIO21_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPD_S  7
/** IO_MUX_GPIO21_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO21_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO21_FUN_WPU_M  (IO_MUX_GPIO21_FUN_WPU_V << IO_MUX_GPIO21_FUN_WPU_S)
#define IO_MUX_GPIO21_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO21_FUN_WPU_S  8
/** IO_MUX_GPIO21_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO21_FUN_IE    (BIT(9))
#define IO_MUX_GPIO21_FUN_IE_M  (IO_MUX_GPIO21_FUN_IE_V << IO_MUX_GPIO21_FUN_IE_S)
#define IO_MUX_GPIO21_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO21_FUN_IE_S  9
/** IO_MUX_GPIO21_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO21_FUN_DRV    0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_M  (IO_MUX_GPIO21_FUN_DRV_V << IO_MUX_GPIO21_FUN_DRV_S)
#define IO_MUX_GPIO21_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO21_FUN_DRV_S  10
/** IO_MUX_GPIO21_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO21_MCU_SEL    0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_M  (IO_MUX_GPIO21_MCU_SEL_V << IO_MUX_GPIO21_MCU_SEL_S)
#define IO_MUX_GPIO21_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO21_MCU_SEL_S  12
/** IO_MUX_GPIO21_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO21_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO21_FILTER_EN_M  (IO_MUX_GPIO21_FILTER_EN_V << IO_MUX_GPIO21_FILTER_EN_S)
#define IO_MUX_GPIO21_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO21_FILTER_EN_S  15
/** IO_MUX_GPIO21_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO21_HYS_EN    (BIT(16))
#define IO_MUX_GPIO21_HYS_EN_M  (IO_MUX_GPIO21_HYS_EN_V << IO_MUX_GPIO21_HYS_EN_S)
#define IO_MUX_GPIO21_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO21_HYS_EN_S  16
/** IO_MUX_GPIO21_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO21_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO21_HYS_SEL_M  (IO_MUX_GPIO21_HYS_SEL_V << IO_MUX_GPIO21_HYS_SEL_S)
#define IO_MUX_GPIO21_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO21_HYS_SEL_S  17

/** IO_MUX_GPIO22_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO22_REG (DR_REG_IO_MUX_BASE + 0x5c)
/** IO_MUX_GPIO22_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO22_MCU_OE    (BIT(0))
#define IO_MUX_GPIO22_MCU_OE_M  (IO_MUX_GPIO22_MCU_OE_V << IO_MUX_GPIO22_MCU_OE_S)
#define IO_MUX_GPIO22_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_OE_S  0
/** IO_MUX_GPIO22_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO22_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO22_SLP_SEL_M  (IO_MUX_GPIO22_SLP_SEL_V << IO_MUX_GPIO22_SLP_SEL_S)
#define IO_MUX_GPIO22_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO22_SLP_SEL_S  1
/** IO_MUX_GPIO22_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO22_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO22_MCU_WPD_M  (IO_MUX_GPIO22_MCU_WPD_V << IO_MUX_GPIO22_MCU_WPD_S)
#define IO_MUX_GPIO22_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPD_S  2
/** IO_MUX_GPIO22_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO22_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO22_MCU_WPU_M  (IO_MUX_GPIO22_MCU_WPU_V << IO_MUX_GPIO22_MCU_WPU_S)
#define IO_MUX_GPIO22_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO22_MCU_WPU_S  3
/** IO_MUX_GPIO22_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO22_MCU_IE    (BIT(4))
#define IO_MUX_GPIO22_MCU_IE_M  (IO_MUX_GPIO22_MCU_IE_V << IO_MUX_GPIO22_MCU_IE_S)
#define IO_MUX_GPIO22_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO22_MCU_IE_S  4
/** IO_MUX_GPIO22_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO22_MCU_DRV    0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_M  (IO_MUX_GPIO22_MCU_DRV_V << IO_MUX_GPIO22_MCU_DRV_S)
#define IO_MUX_GPIO22_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO22_MCU_DRV_S  5
/** IO_MUX_GPIO22_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO22_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO22_FUN_WPD_M  (IO_MUX_GPIO22_FUN_WPD_V << IO_MUX_GPIO22_FUN_WPD_S)
#define IO_MUX_GPIO22_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPD_S  7
/** IO_MUX_GPIO22_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO22_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO22_FUN_WPU_M  (IO_MUX_GPIO22_FUN_WPU_V << IO_MUX_GPIO22_FUN_WPU_S)
#define IO_MUX_GPIO22_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO22_FUN_WPU_S  8
/** IO_MUX_GPIO22_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO22_FUN_IE    (BIT(9))
#define IO_MUX_GPIO22_FUN_IE_M  (IO_MUX_GPIO22_FUN_IE_V << IO_MUX_GPIO22_FUN_IE_S)
#define IO_MUX_GPIO22_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO22_FUN_IE_S  9
/** IO_MUX_GPIO22_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO22_FUN_DRV    0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_M  (IO_MUX_GPIO22_FUN_DRV_V << IO_MUX_GPIO22_FUN_DRV_S)
#define IO_MUX_GPIO22_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO22_FUN_DRV_S  10
/** IO_MUX_GPIO22_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO22_MCU_SEL    0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_M  (IO_MUX_GPIO22_MCU_SEL_V << IO_MUX_GPIO22_MCU_SEL_S)
#define IO_MUX_GPIO22_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO22_MCU_SEL_S  12
/** IO_MUX_GPIO22_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO22_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO22_FILTER_EN_M  (IO_MUX_GPIO22_FILTER_EN_V << IO_MUX_GPIO22_FILTER_EN_S)
#define IO_MUX_GPIO22_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO22_FILTER_EN_S  15
/** IO_MUX_GPIO22_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO22_HYS_EN    (BIT(16))
#define IO_MUX_GPIO22_HYS_EN_M  (IO_MUX_GPIO22_HYS_EN_V << IO_MUX_GPIO22_HYS_EN_S)
#define IO_MUX_GPIO22_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO22_HYS_EN_S  16
/** IO_MUX_GPIO22_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO22_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO22_HYS_SEL_M  (IO_MUX_GPIO22_HYS_SEL_V << IO_MUX_GPIO22_HYS_SEL_S)
#define IO_MUX_GPIO22_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO22_HYS_SEL_S  17

/** IO_MUX_GPIO23_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO23_REG (DR_REG_IO_MUX_BASE + 0x60)
/** IO_MUX_GPIO23_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO23_MCU_OE    (BIT(0))
#define IO_MUX_GPIO23_MCU_OE_M  (IO_MUX_GPIO23_MCU_OE_V << IO_MUX_GPIO23_MCU_OE_S)
#define IO_MUX_GPIO23_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_OE_S  0
/** IO_MUX_GPIO23_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO23_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO23_SLP_SEL_M  (IO_MUX_GPIO23_SLP_SEL_V << IO_MUX_GPIO23_SLP_SEL_S)
#define IO_MUX_GPIO23_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO23_SLP_SEL_S  1
/** IO_MUX_GPIO23_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO23_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO23_MCU_WPD_M  (IO_MUX_GPIO23_MCU_WPD_V << IO_MUX_GPIO23_MCU_WPD_S)
#define IO_MUX_GPIO23_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPD_S  2
/** IO_MUX_GPIO23_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO23_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO23_MCU_WPU_M  (IO_MUX_GPIO23_MCU_WPU_V << IO_MUX_GPIO23_MCU_WPU_S)
#define IO_MUX_GPIO23_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO23_MCU_WPU_S  3
/** IO_MUX_GPIO23_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO23_MCU_IE    (BIT(4))
#define IO_MUX_GPIO23_MCU_IE_M  (IO_MUX_GPIO23_MCU_IE_V << IO_MUX_GPIO23_MCU_IE_S)
#define IO_MUX_GPIO23_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO23_MCU_IE_S  4
/** IO_MUX_GPIO23_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO23_MCU_DRV    0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_M  (IO_MUX_GPIO23_MCU_DRV_V << IO_MUX_GPIO23_MCU_DRV_S)
#define IO_MUX_GPIO23_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO23_MCU_DRV_S  5
/** IO_MUX_GPIO23_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO23_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO23_FUN_WPD_M  (IO_MUX_GPIO23_FUN_WPD_V << IO_MUX_GPIO23_FUN_WPD_S)
#define IO_MUX_GPIO23_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPD_S  7
/** IO_MUX_GPIO23_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO23_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO23_FUN_WPU_M  (IO_MUX_GPIO23_FUN_WPU_V << IO_MUX_GPIO23_FUN_WPU_S)
#define IO_MUX_GPIO23_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO23_FUN_WPU_S  8
/** IO_MUX_GPIO23_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO23_FUN_IE    (BIT(9))
#define IO_MUX_GPIO23_FUN_IE_M  (IO_MUX_GPIO23_FUN_IE_V << IO_MUX_GPIO23_FUN_IE_S)
#define IO_MUX_GPIO23_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO23_FUN_IE_S  9
/** IO_MUX_GPIO23_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO23_FUN_DRV    0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_M  (IO_MUX_GPIO23_FUN_DRV_V << IO_MUX_GPIO23_FUN_DRV_S)
#define IO_MUX_GPIO23_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO23_FUN_DRV_S  10
/** IO_MUX_GPIO23_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO23_MCU_SEL    0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_M  (IO_MUX_GPIO23_MCU_SEL_V << IO_MUX_GPIO23_MCU_SEL_S)
#define IO_MUX_GPIO23_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO23_MCU_SEL_S  12
/** IO_MUX_GPIO23_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO23_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO23_FILTER_EN_M  (IO_MUX_GPIO23_FILTER_EN_V << IO_MUX_GPIO23_FILTER_EN_S)
#define IO_MUX_GPIO23_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO23_FILTER_EN_S  15
/** IO_MUX_GPIO23_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO23_HYS_EN    (BIT(16))
#define IO_MUX_GPIO23_HYS_EN_M  (IO_MUX_GPIO23_HYS_EN_V << IO_MUX_GPIO23_HYS_EN_S)
#define IO_MUX_GPIO23_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO23_HYS_EN_S  16
/** IO_MUX_GPIO23_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO23_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO23_HYS_SEL_M  (IO_MUX_GPIO23_HYS_SEL_V << IO_MUX_GPIO23_HYS_SEL_S)
#define IO_MUX_GPIO23_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO23_HYS_SEL_S  17

/** IO_MUX_GPIO24_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO24_REG (DR_REG_IO_MUX_BASE + 0x64)
/** IO_MUX_GPIO24_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO24_MCU_OE    (BIT(0))
#define IO_MUX_GPIO24_MCU_OE_M  (IO_MUX_GPIO24_MCU_OE_V << IO_MUX_GPIO24_MCU_OE_S)
#define IO_MUX_GPIO24_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_OE_S  0
/** IO_MUX_GPIO24_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO24_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO24_SLP_SEL_M  (IO_MUX_GPIO24_SLP_SEL_V << IO_MUX_GPIO24_SLP_SEL_S)
#define IO_MUX_GPIO24_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO24_SLP_SEL_S  1
/** IO_MUX_GPIO24_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO24_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO24_MCU_WPD_M  (IO_MUX_GPIO24_MCU_WPD_V << IO_MUX_GPIO24_MCU_WPD_S)
#define IO_MUX_GPIO24_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPD_S  2
/** IO_MUX_GPIO24_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO24_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO24_MCU_WPU_M  (IO_MUX_GPIO24_MCU_WPU_V << IO_MUX_GPIO24_MCU_WPU_S)
#define IO_MUX_GPIO24_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO24_MCU_WPU_S  3
/** IO_MUX_GPIO24_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO24_MCU_IE    (BIT(4))
#define IO_MUX_GPIO24_MCU_IE_M  (IO_MUX_GPIO24_MCU_IE_V << IO_MUX_GPIO24_MCU_IE_S)
#define IO_MUX_GPIO24_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO24_MCU_IE_S  4
/** IO_MUX_GPIO24_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO24_MCU_DRV    0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_M  (IO_MUX_GPIO24_MCU_DRV_V << IO_MUX_GPIO24_MCU_DRV_S)
#define IO_MUX_GPIO24_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO24_MCU_DRV_S  5
/** IO_MUX_GPIO24_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO24_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO24_FUN_WPD_M  (IO_MUX_GPIO24_FUN_WPD_V << IO_MUX_GPIO24_FUN_WPD_S)
#define IO_MUX_GPIO24_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPD_S  7
/** IO_MUX_GPIO24_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO24_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO24_FUN_WPU_M  (IO_MUX_GPIO24_FUN_WPU_V << IO_MUX_GPIO24_FUN_WPU_S)
#define IO_MUX_GPIO24_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO24_FUN_WPU_S  8
/** IO_MUX_GPIO24_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO24_FUN_IE    (BIT(9))
#define IO_MUX_GPIO24_FUN_IE_M  (IO_MUX_GPIO24_FUN_IE_V << IO_MUX_GPIO24_FUN_IE_S)
#define IO_MUX_GPIO24_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO24_FUN_IE_S  9
/** IO_MUX_GPIO24_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO24_FUN_DRV    0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_M  (IO_MUX_GPIO24_FUN_DRV_V << IO_MUX_GPIO24_FUN_DRV_S)
#define IO_MUX_GPIO24_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO24_FUN_DRV_S  10
/** IO_MUX_GPIO24_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO24_MCU_SEL    0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_M  (IO_MUX_GPIO24_MCU_SEL_V << IO_MUX_GPIO24_MCU_SEL_S)
#define IO_MUX_GPIO24_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO24_MCU_SEL_S  12
/** IO_MUX_GPIO24_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO24_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO24_FILTER_EN_M  (IO_MUX_GPIO24_FILTER_EN_V << IO_MUX_GPIO24_FILTER_EN_S)
#define IO_MUX_GPIO24_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO24_FILTER_EN_S  15
/** IO_MUX_GPIO24_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO24_HYS_EN    (BIT(16))
#define IO_MUX_GPIO24_HYS_EN_M  (IO_MUX_GPIO24_HYS_EN_V << IO_MUX_GPIO24_HYS_EN_S)
#define IO_MUX_GPIO24_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO24_HYS_EN_S  16
/** IO_MUX_GPIO24_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO24_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO24_HYS_SEL_M  (IO_MUX_GPIO24_HYS_SEL_V << IO_MUX_GPIO24_HYS_SEL_S)
#define IO_MUX_GPIO24_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO24_HYS_SEL_S  17

/** IO_MUX_GPIO25_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO25_REG (DR_REG_IO_MUX_BASE + 0x68)
/** IO_MUX_GPIO25_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO25_MCU_OE    (BIT(0))
#define IO_MUX_GPIO25_MCU_OE_M  (IO_MUX_GPIO25_MCU_OE_V << IO_MUX_GPIO25_MCU_OE_S)
#define IO_MUX_GPIO25_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_OE_S  0
/** IO_MUX_GPIO25_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO25_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO25_SLP_SEL_M  (IO_MUX_GPIO25_SLP_SEL_V << IO_MUX_GPIO25_SLP_SEL_S)
#define IO_MUX_GPIO25_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO25_SLP_SEL_S  1
/** IO_MUX_GPIO25_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO25_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO25_MCU_WPD_M  (IO_MUX_GPIO25_MCU_WPD_V << IO_MUX_GPIO25_MCU_WPD_S)
#define IO_MUX_GPIO25_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPD_S  2
/** IO_MUX_GPIO25_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO25_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO25_MCU_WPU_M  (IO_MUX_GPIO25_MCU_WPU_V << IO_MUX_GPIO25_MCU_WPU_S)
#define IO_MUX_GPIO25_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO25_MCU_WPU_S  3
/** IO_MUX_GPIO25_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO25_MCU_IE    (BIT(4))
#define IO_MUX_GPIO25_MCU_IE_M  (IO_MUX_GPIO25_MCU_IE_V << IO_MUX_GPIO25_MCU_IE_S)
#define IO_MUX_GPIO25_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO25_MCU_IE_S  4
/** IO_MUX_GPIO25_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO25_MCU_DRV    0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_M  (IO_MUX_GPIO25_MCU_DRV_V << IO_MUX_GPIO25_MCU_DRV_S)
#define IO_MUX_GPIO25_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO25_MCU_DRV_S  5
/** IO_MUX_GPIO25_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO25_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO25_FUN_WPD_M  (IO_MUX_GPIO25_FUN_WPD_V << IO_MUX_GPIO25_FUN_WPD_S)
#define IO_MUX_GPIO25_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPD_S  7
/** IO_MUX_GPIO25_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO25_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO25_FUN_WPU_M  (IO_MUX_GPIO25_FUN_WPU_V << IO_MUX_GPIO25_FUN_WPU_S)
#define IO_MUX_GPIO25_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO25_FUN_WPU_S  8
/** IO_MUX_GPIO25_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO25_FUN_IE    (BIT(9))
#define IO_MUX_GPIO25_FUN_IE_M  (IO_MUX_GPIO25_FUN_IE_V << IO_MUX_GPIO25_FUN_IE_S)
#define IO_MUX_GPIO25_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO25_FUN_IE_S  9
/** IO_MUX_GPIO25_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO25_FUN_DRV    0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_M  (IO_MUX_GPIO25_FUN_DRV_V << IO_MUX_GPIO25_FUN_DRV_S)
#define IO_MUX_GPIO25_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO25_FUN_DRV_S  10
/** IO_MUX_GPIO25_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO25_MCU_SEL    0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_M  (IO_MUX_GPIO25_MCU_SEL_V << IO_MUX_GPIO25_MCU_SEL_S)
#define IO_MUX_GPIO25_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO25_MCU_SEL_S  12
/** IO_MUX_GPIO25_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO25_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO25_FILTER_EN_M  (IO_MUX_GPIO25_FILTER_EN_V << IO_MUX_GPIO25_FILTER_EN_S)
#define IO_MUX_GPIO25_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO25_FILTER_EN_S  15
/** IO_MUX_GPIO25_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO25_HYS_EN    (BIT(16))
#define IO_MUX_GPIO25_HYS_EN_M  (IO_MUX_GPIO25_HYS_EN_V << IO_MUX_GPIO25_HYS_EN_S)
#define IO_MUX_GPIO25_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO25_HYS_EN_S  16
/** IO_MUX_GPIO25_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO25_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO25_HYS_SEL_M  (IO_MUX_GPIO25_HYS_SEL_V << IO_MUX_GPIO25_HYS_SEL_S)
#define IO_MUX_GPIO25_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO25_HYS_SEL_S  17

/** IO_MUX_GPIO26_REG register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
#define IO_MUX_GPIO26_REG (DR_REG_IO_MUX_BASE + 0x6c)
/** IO_MUX_GPIO26_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
 */
#define IO_MUX_GPIO26_MCU_OE    (BIT(0))
#define IO_MUX_GPIO26_MCU_OE_M  (IO_MUX_GPIO26_MCU_OE_V << IO_MUX_GPIO26_MCU_OE_S)
#define IO_MUX_GPIO26_MCU_OE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_OE_S  0
/** IO_MUX_GPIO26_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
 */
#define IO_MUX_GPIO26_SLP_SEL    (BIT(1))
#define IO_MUX_GPIO26_SLP_SEL_M  (IO_MUX_GPIO26_SLP_SEL_V << IO_MUX_GPIO26_SLP_SEL_S)
#define IO_MUX_GPIO26_SLP_SEL_V  0x00000001U
#define IO_MUX_GPIO26_SLP_SEL_S  1
/** IO_MUX_GPIO26_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
 *  internal pull-down disabled.
 */
#define IO_MUX_GPIO26_MCU_WPD    (BIT(2))
#define IO_MUX_GPIO26_MCU_WPD_M  (IO_MUX_GPIO26_MCU_WPD_V << IO_MUX_GPIO26_MCU_WPD_S)
#define IO_MUX_GPIO26_MCU_WPD_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPD_S  2
/** IO_MUX_GPIO26_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
 *  internal pull-up disabled.
 */
#define IO_MUX_GPIO26_MCU_WPU    (BIT(3))
#define IO_MUX_GPIO26_MCU_WPU_M  (IO_MUX_GPIO26_MCU_WPU_V << IO_MUX_GPIO26_MCU_WPU_S)
#define IO_MUX_GPIO26_MCU_WPU_V  0x00000001U
#define IO_MUX_GPIO26_MCU_WPU_S  3
/** IO_MUX_GPIO26_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO26_MCU_IE    (BIT(4))
#define IO_MUX_GPIO26_MCU_IE_M  (IO_MUX_GPIO26_MCU_IE_V << IO_MUX_GPIO26_MCU_IE_S)
#define IO_MUX_GPIO26_MCU_IE_V  0x00000001U
#define IO_MUX_GPIO26_MCU_IE_S  4
/** IO_MUX_GPIO26_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
 *  ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO26_MCU_DRV    0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_M  (IO_MUX_GPIO26_MCU_DRV_V << IO_MUX_GPIO26_MCU_DRV_S)
#define IO_MUX_GPIO26_MCU_DRV_V  0x00000003U
#define IO_MUX_GPIO26_MCU_DRV_S  5
/** IO_MUX_GPIO26_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
 *  disabled.
 */
#define IO_MUX_GPIO26_FUN_WPD    (BIT(7))
#define IO_MUX_GPIO26_FUN_WPD_M  (IO_MUX_GPIO26_FUN_WPD_V << IO_MUX_GPIO26_FUN_WPD_S)
#define IO_MUX_GPIO26_FUN_WPD_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPD_S  7
/** IO_MUX_GPIO26_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
 *  disabled.
 */
#define IO_MUX_GPIO26_FUN_WPU    (BIT(8))
#define IO_MUX_GPIO26_FUN_WPU_M  (IO_MUX_GPIO26_FUN_WPU_V << IO_MUX_GPIO26_FUN_WPU_S)
#define IO_MUX_GPIO26_FUN_WPU_V  0x00000001U
#define IO_MUX_GPIO26_FUN_WPU_S  8
/** IO_MUX_GPIO26_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Input enable of the pad. 1: input enabled. 0: input disabled.
 */
#define IO_MUX_GPIO26_FUN_IE    (BIT(9))
#define IO_MUX_GPIO26_FUN_IE_M  (IO_MUX_GPIO26_FUN_IE_V << IO_MUX_GPIO26_FUN_IE_S)
#define IO_MUX_GPIO26_FUN_IE_V  0x00000001U
#define IO_MUX_GPIO26_FUN_IE_S  9
/** IO_MUX_GPIO26_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
 */
#define IO_MUX_GPIO26_FUN_DRV    0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_M  (IO_MUX_GPIO26_FUN_DRV_V << IO_MUX_GPIO26_FUN_DRV_S)
#define IO_MUX_GPIO26_FUN_DRV_V  0x00000003U
#define IO_MUX_GPIO26_FUN_DRV_S  10
/** IO_MUX_GPIO26_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
 *  etc.
 */
#define IO_MUX_GPIO26_MCU_SEL    0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_M  (IO_MUX_GPIO26_MCU_SEL_V << IO_MUX_GPIO26_MCU_SEL_S)
#define IO_MUX_GPIO26_MCU_SEL_V  0x00000007U
#define IO_MUX_GPIO26_MCU_SEL_S  12
/** IO_MUX_GPIO26_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
 */
#define IO_MUX_GPIO26_FILTER_EN    (BIT(15))
#define IO_MUX_GPIO26_FILTER_EN_M  (IO_MUX_GPIO26_FILTER_EN_V << IO_MUX_GPIO26_FILTER_EN_S)
#define IO_MUX_GPIO26_FILTER_EN_V  0x00000001U
#define IO_MUX_GPIO26_FILTER_EN_S  15
/** IO_MUX_GPIO26_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
 *  Hysteresis disabled.
 */
#define IO_MUX_GPIO26_HYS_EN    (BIT(16))
#define IO_MUX_GPIO26_HYS_EN_M  (IO_MUX_GPIO26_HYS_EN_V << IO_MUX_GPIO26_HYS_EN_S)
#define IO_MUX_GPIO26_HYS_EN_V  0x00000001U
#define IO_MUX_GPIO26_HYS_EN_S  16
/** IO_MUX_GPIO26_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Select enabling signals of the pad from software and efuse hardware. 1: Select
 *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
 */
#define IO_MUX_GPIO26_HYS_SEL    (BIT(17))
#define IO_MUX_GPIO26_HYS_SEL_M  (IO_MUX_GPIO26_HYS_SEL_V << IO_MUX_GPIO26_HYS_SEL_S)
#define IO_MUX_GPIO26_HYS_SEL_V  0x00000001U
#define IO_MUX_GPIO26_HYS_SEL_S  17

/** IO_MUX_DATE_REG register
 *  IO MUX Version Control Register
 */
#define IO_MUX_DATE_REG (DR_REG_IO_MUX_BASE + 0xfc)
/** IO_MUX_REG_DATE : R/W; bitpos: [27:0]; default: 36708704;
 *  Version control register
 */
#define IO_MUX_REG_DATE    0x0FFFFFFFU
#define IO_MUX_REG_DATE_M  (IO_MUX_REG_DATE_V << IO_MUX_REG_DATE_S)
#define IO_MUX_REG_DATE_V  0x0FFFFFFFU
#define IO_MUX_REG_DATE_S  0

#ifdef __cplusplus
}
#endif
