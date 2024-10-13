/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** LP_IO_MUX_GPIO0_REG register
 *  LP IO MUX configuration register for LP_GPIO0
 */
#define LP_IO_MUX_GPIO0_REG (DR_REG_LP_IO_MUX_BASE + 0x0)
/** LP_IO_MUX_GPIO0_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO0 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_MCU_OE    (BIT(0))
#define LP_IO_MUX_GPIO0_MCU_OE_M  (LP_IO_MUX_GPIO0_MCU_OE_V << LP_IO_MUX_GPIO0_MCU_OE_S)
#define LP_IO_MUX_GPIO0_MCU_OE_V  0x00000001U
#define LP_IO_MUX_GPIO0_MCU_OE_S  0
/** LP_IO_MUX_GPIO0_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for LP_GPIO0.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define LP_IO_MUX_GPIO0_SLP_SEL    (BIT(1))
#define LP_IO_MUX_GPIO0_SLP_SEL_M  (LP_IO_MUX_GPIO0_SLP_SEL_V << LP_IO_MUX_GPIO0_SLP_SEL_S)
#define LP_IO_MUX_GPIO0_SLP_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO0_SLP_SEL_S  1
/** LP_IO_MUX_GPIO0_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of LP_GPIO0 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_MCU_WPD    (BIT(2))
#define LP_IO_MUX_GPIO0_MCU_WPD_M  (LP_IO_MUX_GPIO0_MCU_WPD_V << LP_IO_MUX_GPIO0_MCU_WPD_S)
#define LP_IO_MUX_GPIO0_MCU_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO0_MCU_WPD_S  2
/** LP_IO_MUX_GPIO0_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of LP_GPIO0 during sleep mode.
 *  \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_MCU_WPU    (BIT(3))
#define LP_IO_MUX_GPIO0_MCU_WPU_M  (LP_IO_MUX_GPIO0_MCU_WPU_V << LP_IO_MUX_GPIO0_MCU_WPU_S)
#define LP_IO_MUX_GPIO0_MCU_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO0_MCU_WPU_S  3
/** LP_IO_MUX_GPIO0_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of LP_GPIO0 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_MCU_IE    (BIT(4))
#define LP_IO_MUX_GPIO0_MCU_IE_M  (LP_IO_MUX_GPIO0_MCU_IE_V << LP_IO_MUX_GPIO0_MCU_IE_S)
#define LP_IO_MUX_GPIO0_MCU_IE_V  0x00000001U
#define LP_IO_MUX_GPIO0_MCU_IE_S  4
/** LP_IO_MUX_GPIO0_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of LP_GPIO0 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO0_MCU_DRV    0x00000003U
#define LP_IO_MUX_GPIO0_MCU_DRV_M  (LP_IO_MUX_GPIO0_MCU_DRV_V << LP_IO_MUX_GPIO0_MCU_DRV_S)
#define LP_IO_MUX_GPIO0_MCU_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO0_MCU_DRV_S  5
/** LP_IO_MUX_GPIO0_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of LP_GPIO0.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_FUN_WPD    (BIT(7))
#define LP_IO_MUX_GPIO0_FUN_WPD_M  (LP_IO_MUX_GPIO0_FUN_WPD_V << LP_IO_MUX_GPIO0_FUN_WPD_S)
#define LP_IO_MUX_GPIO0_FUN_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO0_FUN_WPD_S  7
/** LP_IO_MUX_GPIO0_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of LP_GPIO0.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_FUN_WPU    (BIT(8))
#define LP_IO_MUX_GPIO0_FUN_WPU_M  (LP_IO_MUX_GPIO0_FUN_WPU_V << LP_IO_MUX_GPIO0_FUN_WPU_S)
#define LP_IO_MUX_GPIO0_FUN_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO0_FUN_WPU_S  8
/** LP_IO_MUX_GPIO0_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of LP_GPIO0.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_FUN_IE    (BIT(9))
#define LP_IO_MUX_GPIO0_FUN_IE_M  (LP_IO_MUX_GPIO0_FUN_IE_V << LP_IO_MUX_GPIO0_FUN_IE_S)
#define LP_IO_MUX_GPIO0_FUN_IE_V  0x00000001U
#define LP_IO_MUX_GPIO0_FUN_IE_S  9
/** LP_IO_MUX_GPIO0_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of LP_GPIO0. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO0_FUN_DRV    0x00000003U
#define LP_IO_MUX_GPIO0_FUN_DRV_M  (LP_IO_MUX_GPIO0_FUN_DRV_V << LP_IO_MUX_GPIO0_FUN_DRV_S)
#define LP_IO_MUX_GPIO0_FUN_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO0_FUN_DRV_S  10
/** LP_IO_MUX_GPIO0_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select LP IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define LP_IO_MUX_GPIO0_MCU_SEL    0x00000007U
#define LP_IO_MUX_GPIO0_MCU_SEL_M  (LP_IO_MUX_GPIO0_MCU_SEL_V << LP_IO_MUX_GPIO0_MCU_SEL_S)
#define LP_IO_MUX_GPIO0_MCU_SEL_V  0x00000007U
#define LP_IO_MUX_GPIO0_MCU_SEL_S  12
/** LP_IO_MUX_GPIO0_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_FILTER_EN    (BIT(15))
#define LP_IO_MUX_GPIO0_FILTER_EN_M  (LP_IO_MUX_GPIO0_FILTER_EN_V << LP_IO_MUX_GPIO0_FILTER_EN_S)
#define LP_IO_MUX_GPIO0_FILTER_EN_V  0x00000001U
#define LP_IO_MUX_GPIO0_FILTER_EN_S  15
/** LP_IO_MUX_GPIO0_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  LP_IO_MUX_GPIO0_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO0_HYS_EN    (BIT(16))
#define LP_IO_MUX_GPIO0_HYS_EN_M  (LP_IO_MUX_GPIO0_HYS_EN_V << LP_IO_MUX_GPIO0_HYS_EN_S)
#define LP_IO_MUX_GPIO0_HYS_EN_V  0x00000001U
#define LP_IO_MUX_GPIO0_HYS_EN_S  16
/** LP_IO_MUX_GPIO0_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for LP_GPIO0.
 *  \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of LP_IO_MUX_GPIO0_HYS_EN\\
 */
#define LP_IO_MUX_GPIO0_HYS_SEL    (BIT(17))
#define LP_IO_MUX_GPIO0_HYS_SEL_M  (LP_IO_MUX_GPIO0_HYS_SEL_V << LP_IO_MUX_GPIO0_HYS_SEL_S)
#define LP_IO_MUX_GPIO0_HYS_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO0_HYS_SEL_S  17

/** LP_IO_MUX_GPIO1_REG register
 *  LP IO MUX configuration register for LP_GPIO1
 */
#define LP_IO_MUX_GPIO1_REG (DR_REG_LP_IO_MUX_BASE + 0x4)
/** LP_IO_MUX_GPIO1_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO1 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_MCU_OE    (BIT(0))
#define LP_IO_MUX_GPIO1_MCU_OE_M  (LP_IO_MUX_GPIO1_MCU_OE_V << LP_IO_MUX_GPIO1_MCU_OE_S)
#define LP_IO_MUX_GPIO1_MCU_OE_V  0x00000001U
#define LP_IO_MUX_GPIO1_MCU_OE_S  0
/** LP_IO_MUX_GPIO1_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for LP_GPIO1.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define LP_IO_MUX_GPIO1_SLP_SEL    (BIT(1))
#define LP_IO_MUX_GPIO1_SLP_SEL_M  (LP_IO_MUX_GPIO1_SLP_SEL_V << LP_IO_MUX_GPIO1_SLP_SEL_S)
#define LP_IO_MUX_GPIO1_SLP_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO1_SLP_SEL_S  1
/** LP_IO_MUX_GPIO1_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of LP_GPIO1 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_MCU_WPD    (BIT(2))
#define LP_IO_MUX_GPIO1_MCU_WPD_M  (LP_IO_MUX_GPIO1_MCU_WPD_V << LP_IO_MUX_GPIO1_MCU_WPD_S)
#define LP_IO_MUX_GPIO1_MCU_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO1_MCU_WPD_S  2
/** LP_IO_MUX_GPIO1_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of LP_GPIO1 during sleep mode.
 *  \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_MCU_WPU    (BIT(3))
#define LP_IO_MUX_GPIO1_MCU_WPU_M  (LP_IO_MUX_GPIO1_MCU_WPU_V << LP_IO_MUX_GPIO1_MCU_WPU_S)
#define LP_IO_MUX_GPIO1_MCU_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO1_MCU_WPU_S  3
/** LP_IO_MUX_GPIO1_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of LP_GPIO1 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_MCU_IE    (BIT(4))
#define LP_IO_MUX_GPIO1_MCU_IE_M  (LP_IO_MUX_GPIO1_MCU_IE_V << LP_IO_MUX_GPIO1_MCU_IE_S)
#define LP_IO_MUX_GPIO1_MCU_IE_V  0x00000001U
#define LP_IO_MUX_GPIO1_MCU_IE_S  4
/** LP_IO_MUX_GPIO1_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of LP_GPIO1 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO1_MCU_DRV    0x00000003U
#define LP_IO_MUX_GPIO1_MCU_DRV_M  (LP_IO_MUX_GPIO1_MCU_DRV_V << LP_IO_MUX_GPIO1_MCU_DRV_S)
#define LP_IO_MUX_GPIO1_MCU_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO1_MCU_DRV_S  5
/** LP_IO_MUX_GPIO1_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of LP_GPIO1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_FUN_WPD    (BIT(7))
#define LP_IO_MUX_GPIO1_FUN_WPD_M  (LP_IO_MUX_GPIO1_FUN_WPD_V << LP_IO_MUX_GPIO1_FUN_WPD_S)
#define LP_IO_MUX_GPIO1_FUN_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO1_FUN_WPD_S  7
/** LP_IO_MUX_GPIO1_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of LP_GPIO1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_FUN_WPU    (BIT(8))
#define LP_IO_MUX_GPIO1_FUN_WPU_M  (LP_IO_MUX_GPIO1_FUN_WPU_V << LP_IO_MUX_GPIO1_FUN_WPU_S)
#define LP_IO_MUX_GPIO1_FUN_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO1_FUN_WPU_S  8
/** LP_IO_MUX_GPIO1_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of LP_GPIO1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_FUN_IE    (BIT(9))
#define LP_IO_MUX_GPIO1_FUN_IE_M  (LP_IO_MUX_GPIO1_FUN_IE_V << LP_IO_MUX_GPIO1_FUN_IE_S)
#define LP_IO_MUX_GPIO1_FUN_IE_V  0x00000001U
#define LP_IO_MUX_GPIO1_FUN_IE_S  9
/** LP_IO_MUX_GPIO1_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of LP_GPIO1. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO1_FUN_DRV    0x00000003U
#define LP_IO_MUX_GPIO1_FUN_DRV_M  (LP_IO_MUX_GPIO1_FUN_DRV_V << LP_IO_MUX_GPIO1_FUN_DRV_S)
#define LP_IO_MUX_GPIO1_FUN_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO1_FUN_DRV_S  10
/** LP_IO_MUX_GPIO1_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select LP IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define LP_IO_MUX_GPIO1_MCU_SEL    0x00000007U
#define LP_IO_MUX_GPIO1_MCU_SEL_M  (LP_IO_MUX_GPIO1_MCU_SEL_V << LP_IO_MUX_GPIO1_MCU_SEL_S)
#define LP_IO_MUX_GPIO1_MCU_SEL_V  0x00000007U
#define LP_IO_MUX_GPIO1_MCU_SEL_S  12
/** LP_IO_MUX_GPIO1_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_FILTER_EN    (BIT(15))
#define LP_IO_MUX_GPIO1_FILTER_EN_M  (LP_IO_MUX_GPIO1_FILTER_EN_V << LP_IO_MUX_GPIO1_FILTER_EN_S)
#define LP_IO_MUX_GPIO1_FILTER_EN_V  0x00000001U
#define LP_IO_MUX_GPIO1_FILTER_EN_S  15
/** LP_IO_MUX_GPIO1_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  LP_IO_MUX_GPIO1_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO1_HYS_EN    (BIT(16))
#define LP_IO_MUX_GPIO1_HYS_EN_M  (LP_IO_MUX_GPIO1_HYS_EN_V << LP_IO_MUX_GPIO1_HYS_EN_S)
#define LP_IO_MUX_GPIO1_HYS_EN_V  0x00000001U
#define LP_IO_MUX_GPIO1_HYS_EN_S  16
/** LP_IO_MUX_GPIO1_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for LP_GPIO1.
 *  \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of LP_IO_MUX_GPIO1_HYS_EN\\
 */
#define LP_IO_MUX_GPIO1_HYS_SEL    (BIT(17))
#define LP_IO_MUX_GPIO1_HYS_SEL_M  (LP_IO_MUX_GPIO1_HYS_SEL_V << LP_IO_MUX_GPIO1_HYS_SEL_S)
#define LP_IO_MUX_GPIO1_HYS_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO1_HYS_SEL_S  17

/** LP_IO_MUX_GPIO2_REG register
 *  LP IO MUX configuration register for LP_GPIO2
 */
#define LP_IO_MUX_GPIO2_REG (DR_REG_LP_IO_MUX_BASE + 0x8)
/** LP_IO_MUX_GPIO2_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO2 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_MCU_OE    (BIT(0))
#define LP_IO_MUX_GPIO2_MCU_OE_M  (LP_IO_MUX_GPIO2_MCU_OE_V << LP_IO_MUX_GPIO2_MCU_OE_S)
#define LP_IO_MUX_GPIO2_MCU_OE_V  0x00000001U
#define LP_IO_MUX_GPIO2_MCU_OE_S  0
/** LP_IO_MUX_GPIO2_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for LP_GPIO2.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define LP_IO_MUX_GPIO2_SLP_SEL    (BIT(1))
#define LP_IO_MUX_GPIO2_SLP_SEL_M  (LP_IO_MUX_GPIO2_SLP_SEL_V << LP_IO_MUX_GPIO2_SLP_SEL_S)
#define LP_IO_MUX_GPIO2_SLP_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO2_SLP_SEL_S  1
/** LP_IO_MUX_GPIO2_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of LP_GPIO2 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_MCU_WPD    (BIT(2))
#define LP_IO_MUX_GPIO2_MCU_WPD_M  (LP_IO_MUX_GPIO2_MCU_WPD_V << LP_IO_MUX_GPIO2_MCU_WPD_S)
#define LP_IO_MUX_GPIO2_MCU_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO2_MCU_WPD_S  2
/** LP_IO_MUX_GPIO2_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of LP_GPIO2 during sleep mode.
 *  \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_MCU_WPU    (BIT(3))
#define LP_IO_MUX_GPIO2_MCU_WPU_M  (LP_IO_MUX_GPIO2_MCU_WPU_V << LP_IO_MUX_GPIO2_MCU_WPU_S)
#define LP_IO_MUX_GPIO2_MCU_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO2_MCU_WPU_S  3
/** LP_IO_MUX_GPIO2_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of LP_GPIO2 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_MCU_IE    (BIT(4))
#define LP_IO_MUX_GPIO2_MCU_IE_M  (LP_IO_MUX_GPIO2_MCU_IE_V << LP_IO_MUX_GPIO2_MCU_IE_S)
#define LP_IO_MUX_GPIO2_MCU_IE_V  0x00000001U
#define LP_IO_MUX_GPIO2_MCU_IE_S  4
/** LP_IO_MUX_GPIO2_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of LP_GPIO2 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO2_MCU_DRV    0x00000003U
#define LP_IO_MUX_GPIO2_MCU_DRV_M  (LP_IO_MUX_GPIO2_MCU_DRV_V << LP_IO_MUX_GPIO2_MCU_DRV_S)
#define LP_IO_MUX_GPIO2_MCU_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO2_MCU_DRV_S  5
/** LP_IO_MUX_GPIO2_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of LP_GPIO2.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_FUN_WPD    (BIT(7))
#define LP_IO_MUX_GPIO2_FUN_WPD_M  (LP_IO_MUX_GPIO2_FUN_WPD_V << LP_IO_MUX_GPIO2_FUN_WPD_S)
#define LP_IO_MUX_GPIO2_FUN_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO2_FUN_WPD_S  7
/** LP_IO_MUX_GPIO2_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of LP_GPIO2.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_FUN_WPU    (BIT(8))
#define LP_IO_MUX_GPIO2_FUN_WPU_M  (LP_IO_MUX_GPIO2_FUN_WPU_V << LP_IO_MUX_GPIO2_FUN_WPU_S)
#define LP_IO_MUX_GPIO2_FUN_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO2_FUN_WPU_S  8
/** LP_IO_MUX_GPIO2_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of LP_GPIO2.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_FUN_IE    (BIT(9))
#define LP_IO_MUX_GPIO2_FUN_IE_M  (LP_IO_MUX_GPIO2_FUN_IE_V << LP_IO_MUX_GPIO2_FUN_IE_S)
#define LP_IO_MUX_GPIO2_FUN_IE_V  0x00000001U
#define LP_IO_MUX_GPIO2_FUN_IE_S  9
/** LP_IO_MUX_GPIO2_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of LP_GPIO2. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO2_FUN_DRV    0x00000003U
#define LP_IO_MUX_GPIO2_FUN_DRV_M  (LP_IO_MUX_GPIO2_FUN_DRV_V << LP_IO_MUX_GPIO2_FUN_DRV_S)
#define LP_IO_MUX_GPIO2_FUN_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO2_FUN_DRV_S  10
/** LP_IO_MUX_GPIO2_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select LP IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define LP_IO_MUX_GPIO2_MCU_SEL    0x00000007U
#define LP_IO_MUX_GPIO2_MCU_SEL_M  (LP_IO_MUX_GPIO2_MCU_SEL_V << LP_IO_MUX_GPIO2_MCU_SEL_S)
#define LP_IO_MUX_GPIO2_MCU_SEL_V  0x00000007U
#define LP_IO_MUX_GPIO2_MCU_SEL_S  12
/** LP_IO_MUX_GPIO2_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_FILTER_EN    (BIT(15))
#define LP_IO_MUX_GPIO2_FILTER_EN_M  (LP_IO_MUX_GPIO2_FILTER_EN_V << LP_IO_MUX_GPIO2_FILTER_EN_S)
#define LP_IO_MUX_GPIO2_FILTER_EN_V  0x00000001U
#define LP_IO_MUX_GPIO2_FILTER_EN_S  15
/** LP_IO_MUX_GPIO2_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  LP_IO_MUX_GPIO2_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO2_HYS_EN    (BIT(16))
#define LP_IO_MUX_GPIO2_HYS_EN_M  (LP_IO_MUX_GPIO2_HYS_EN_V << LP_IO_MUX_GPIO2_HYS_EN_S)
#define LP_IO_MUX_GPIO2_HYS_EN_V  0x00000001U
#define LP_IO_MUX_GPIO2_HYS_EN_S  16
/** LP_IO_MUX_GPIO2_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for LP_GPIO2.
 *  \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of LP_IO_MUX_GPIO2_HYS_EN\\
 */
#define LP_IO_MUX_GPIO2_HYS_SEL    (BIT(17))
#define LP_IO_MUX_GPIO2_HYS_SEL_M  (LP_IO_MUX_GPIO2_HYS_SEL_V << LP_IO_MUX_GPIO2_HYS_SEL_S)
#define LP_IO_MUX_GPIO2_HYS_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO2_HYS_SEL_S  17

/** LP_IO_MUX_GPIO3_REG register
 *  LP IO MUX configuration register for LP_GPIO3
 */
#define LP_IO_MUX_GPIO3_REG (DR_REG_LP_IO_MUX_BASE + 0xc)
/** LP_IO_MUX_GPIO3_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO3 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_MCU_OE    (BIT(0))
#define LP_IO_MUX_GPIO3_MCU_OE_M  (LP_IO_MUX_GPIO3_MCU_OE_V << LP_IO_MUX_GPIO3_MCU_OE_S)
#define LP_IO_MUX_GPIO3_MCU_OE_V  0x00000001U
#define LP_IO_MUX_GPIO3_MCU_OE_S  0
/** LP_IO_MUX_GPIO3_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for LP_GPIO3.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define LP_IO_MUX_GPIO3_SLP_SEL    (BIT(1))
#define LP_IO_MUX_GPIO3_SLP_SEL_M  (LP_IO_MUX_GPIO3_SLP_SEL_V << LP_IO_MUX_GPIO3_SLP_SEL_S)
#define LP_IO_MUX_GPIO3_SLP_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO3_SLP_SEL_S  1
/** LP_IO_MUX_GPIO3_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of LP_GPIO3 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_MCU_WPD    (BIT(2))
#define LP_IO_MUX_GPIO3_MCU_WPD_M  (LP_IO_MUX_GPIO3_MCU_WPD_V << LP_IO_MUX_GPIO3_MCU_WPD_S)
#define LP_IO_MUX_GPIO3_MCU_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO3_MCU_WPD_S  2
/** LP_IO_MUX_GPIO3_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of LP_GPIO3 during sleep mode.
 *  \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_MCU_WPU    (BIT(3))
#define LP_IO_MUX_GPIO3_MCU_WPU_M  (LP_IO_MUX_GPIO3_MCU_WPU_V << LP_IO_MUX_GPIO3_MCU_WPU_S)
#define LP_IO_MUX_GPIO3_MCU_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO3_MCU_WPU_S  3
/** LP_IO_MUX_GPIO3_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of LP_GPIO3 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_MCU_IE    (BIT(4))
#define LP_IO_MUX_GPIO3_MCU_IE_M  (LP_IO_MUX_GPIO3_MCU_IE_V << LP_IO_MUX_GPIO3_MCU_IE_S)
#define LP_IO_MUX_GPIO3_MCU_IE_V  0x00000001U
#define LP_IO_MUX_GPIO3_MCU_IE_S  4
/** LP_IO_MUX_GPIO3_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of LP_GPIO3 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO3_MCU_DRV    0x00000003U
#define LP_IO_MUX_GPIO3_MCU_DRV_M  (LP_IO_MUX_GPIO3_MCU_DRV_V << LP_IO_MUX_GPIO3_MCU_DRV_S)
#define LP_IO_MUX_GPIO3_MCU_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO3_MCU_DRV_S  5
/** LP_IO_MUX_GPIO3_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of LP_GPIO3.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_FUN_WPD    (BIT(7))
#define LP_IO_MUX_GPIO3_FUN_WPD_M  (LP_IO_MUX_GPIO3_FUN_WPD_V << LP_IO_MUX_GPIO3_FUN_WPD_S)
#define LP_IO_MUX_GPIO3_FUN_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO3_FUN_WPD_S  7
/** LP_IO_MUX_GPIO3_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of LP_GPIO3.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_FUN_WPU    (BIT(8))
#define LP_IO_MUX_GPIO3_FUN_WPU_M  (LP_IO_MUX_GPIO3_FUN_WPU_V << LP_IO_MUX_GPIO3_FUN_WPU_S)
#define LP_IO_MUX_GPIO3_FUN_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO3_FUN_WPU_S  8
/** LP_IO_MUX_GPIO3_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of LP_GPIO3.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_FUN_IE    (BIT(9))
#define LP_IO_MUX_GPIO3_FUN_IE_M  (LP_IO_MUX_GPIO3_FUN_IE_V << LP_IO_MUX_GPIO3_FUN_IE_S)
#define LP_IO_MUX_GPIO3_FUN_IE_V  0x00000001U
#define LP_IO_MUX_GPIO3_FUN_IE_S  9
/** LP_IO_MUX_GPIO3_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of LP_GPIO3. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO3_FUN_DRV    0x00000003U
#define LP_IO_MUX_GPIO3_FUN_DRV_M  (LP_IO_MUX_GPIO3_FUN_DRV_V << LP_IO_MUX_GPIO3_FUN_DRV_S)
#define LP_IO_MUX_GPIO3_FUN_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO3_FUN_DRV_S  10
/** LP_IO_MUX_GPIO3_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select LP IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define LP_IO_MUX_GPIO3_MCU_SEL    0x00000007U
#define LP_IO_MUX_GPIO3_MCU_SEL_M  (LP_IO_MUX_GPIO3_MCU_SEL_V << LP_IO_MUX_GPIO3_MCU_SEL_S)
#define LP_IO_MUX_GPIO3_MCU_SEL_V  0x00000007U
#define LP_IO_MUX_GPIO3_MCU_SEL_S  12
/** LP_IO_MUX_GPIO3_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_FILTER_EN    (BIT(15))
#define LP_IO_MUX_GPIO3_FILTER_EN_M  (LP_IO_MUX_GPIO3_FILTER_EN_V << LP_IO_MUX_GPIO3_FILTER_EN_S)
#define LP_IO_MUX_GPIO3_FILTER_EN_V  0x00000001U
#define LP_IO_MUX_GPIO3_FILTER_EN_S  15
/** LP_IO_MUX_GPIO3_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  LP_IO_MUX_GPIO3_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO3_HYS_EN    (BIT(16))
#define LP_IO_MUX_GPIO3_HYS_EN_M  (LP_IO_MUX_GPIO3_HYS_EN_V << LP_IO_MUX_GPIO3_HYS_EN_S)
#define LP_IO_MUX_GPIO3_HYS_EN_V  0x00000001U
#define LP_IO_MUX_GPIO3_HYS_EN_S  16
/** LP_IO_MUX_GPIO3_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for LP_GPIO3.
 *  \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of LP_IO_MUX_GPIO3_HYS_EN\\
 */
#define LP_IO_MUX_GPIO3_HYS_SEL    (BIT(17))
#define LP_IO_MUX_GPIO3_HYS_SEL_M  (LP_IO_MUX_GPIO3_HYS_SEL_V << LP_IO_MUX_GPIO3_HYS_SEL_S)
#define LP_IO_MUX_GPIO3_HYS_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO3_HYS_SEL_S  17

/** LP_IO_MUX_GPIO4_REG register
 *  LP IO MUX configuration register for LP_GPIO4
 */
#define LP_IO_MUX_GPIO4_REG (DR_REG_LP_IO_MUX_BASE + 0x10)
/** LP_IO_MUX_GPIO4_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO4 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_MCU_OE    (BIT(0))
#define LP_IO_MUX_GPIO4_MCU_OE_M  (LP_IO_MUX_GPIO4_MCU_OE_V << LP_IO_MUX_GPIO4_MCU_OE_S)
#define LP_IO_MUX_GPIO4_MCU_OE_V  0x00000001U
#define LP_IO_MUX_GPIO4_MCU_OE_S  0
/** LP_IO_MUX_GPIO4_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for LP_GPIO4.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define LP_IO_MUX_GPIO4_SLP_SEL    (BIT(1))
#define LP_IO_MUX_GPIO4_SLP_SEL_M  (LP_IO_MUX_GPIO4_SLP_SEL_V << LP_IO_MUX_GPIO4_SLP_SEL_S)
#define LP_IO_MUX_GPIO4_SLP_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO4_SLP_SEL_S  1
/** LP_IO_MUX_GPIO4_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of LP_GPIO4 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_MCU_WPD    (BIT(2))
#define LP_IO_MUX_GPIO4_MCU_WPD_M  (LP_IO_MUX_GPIO4_MCU_WPD_V << LP_IO_MUX_GPIO4_MCU_WPD_S)
#define LP_IO_MUX_GPIO4_MCU_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO4_MCU_WPD_S  2
/** LP_IO_MUX_GPIO4_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of LP_GPIO4 during sleep mode.
 *  \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_MCU_WPU    (BIT(3))
#define LP_IO_MUX_GPIO4_MCU_WPU_M  (LP_IO_MUX_GPIO4_MCU_WPU_V << LP_IO_MUX_GPIO4_MCU_WPU_S)
#define LP_IO_MUX_GPIO4_MCU_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO4_MCU_WPU_S  3
/** LP_IO_MUX_GPIO4_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of LP_GPIO4 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_MCU_IE    (BIT(4))
#define LP_IO_MUX_GPIO4_MCU_IE_M  (LP_IO_MUX_GPIO4_MCU_IE_V << LP_IO_MUX_GPIO4_MCU_IE_S)
#define LP_IO_MUX_GPIO4_MCU_IE_V  0x00000001U
#define LP_IO_MUX_GPIO4_MCU_IE_S  4
/** LP_IO_MUX_GPIO4_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of LP_GPIO4 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO4_MCU_DRV    0x00000003U
#define LP_IO_MUX_GPIO4_MCU_DRV_M  (LP_IO_MUX_GPIO4_MCU_DRV_V << LP_IO_MUX_GPIO4_MCU_DRV_S)
#define LP_IO_MUX_GPIO4_MCU_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO4_MCU_DRV_S  5
/** LP_IO_MUX_GPIO4_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of LP_GPIO4.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_FUN_WPD    (BIT(7))
#define LP_IO_MUX_GPIO4_FUN_WPD_M  (LP_IO_MUX_GPIO4_FUN_WPD_V << LP_IO_MUX_GPIO4_FUN_WPD_S)
#define LP_IO_MUX_GPIO4_FUN_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO4_FUN_WPD_S  7
/** LP_IO_MUX_GPIO4_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of LP_GPIO4.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_FUN_WPU    (BIT(8))
#define LP_IO_MUX_GPIO4_FUN_WPU_M  (LP_IO_MUX_GPIO4_FUN_WPU_V << LP_IO_MUX_GPIO4_FUN_WPU_S)
#define LP_IO_MUX_GPIO4_FUN_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO4_FUN_WPU_S  8
/** LP_IO_MUX_GPIO4_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of LP_GPIO4.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_FUN_IE    (BIT(9))
#define LP_IO_MUX_GPIO4_FUN_IE_M  (LP_IO_MUX_GPIO4_FUN_IE_V << LP_IO_MUX_GPIO4_FUN_IE_S)
#define LP_IO_MUX_GPIO4_FUN_IE_V  0x00000001U
#define LP_IO_MUX_GPIO4_FUN_IE_S  9
/** LP_IO_MUX_GPIO4_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of LP_GPIO4. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO4_FUN_DRV    0x00000003U
#define LP_IO_MUX_GPIO4_FUN_DRV_M  (LP_IO_MUX_GPIO4_FUN_DRV_V << LP_IO_MUX_GPIO4_FUN_DRV_S)
#define LP_IO_MUX_GPIO4_FUN_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO4_FUN_DRV_S  10
/** LP_IO_MUX_GPIO4_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select LP IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define LP_IO_MUX_GPIO4_MCU_SEL    0x00000007U
#define LP_IO_MUX_GPIO4_MCU_SEL_M  (LP_IO_MUX_GPIO4_MCU_SEL_V << LP_IO_MUX_GPIO4_MCU_SEL_S)
#define LP_IO_MUX_GPIO4_MCU_SEL_V  0x00000007U
#define LP_IO_MUX_GPIO4_MCU_SEL_S  12
/** LP_IO_MUX_GPIO4_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_FILTER_EN    (BIT(15))
#define LP_IO_MUX_GPIO4_FILTER_EN_M  (LP_IO_MUX_GPIO4_FILTER_EN_V << LP_IO_MUX_GPIO4_FILTER_EN_S)
#define LP_IO_MUX_GPIO4_FILTER_EN_V  0x00000001U
#define LP_IO_MUX_GPIO4_FILTER_EN_S  15
/** LP_IO_MUX_GPIO4_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  LP_IO_MUX_GPIO4_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO4_HYS_EN    (BIT(16))
#define LP_IO_MUX_GPIO4_HYS_EN_M  (LP_IO_MUX_GPIO4_HYS_EN_V << LP_IO_MUX_GPIO4_HYS_EN_S)
#define LP_IO_MUX_GPIO4_HYS_EN_V  0x00000001U
#define LP_IO_MUX_GPIO4_HYS_EN_S  16
/** LP_IO_MUX_GPIO4_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for LP_GPIO4.
 *  \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of LP_IO_MUX_GPIO4_HYS_EN\\
 */
#define LP_IO_MUX_GPIO4_HYS_SEL    (BIT(17))
#define LP_IO_MUX_GPIO4_HYS_SEL_M  (LP_IO_MUX_GPIO4_HYS_SEL_V << LP_IO_MUX_GPIO4_HYS_SEL_S)
#define LP_IO_MUX_GPIO4_HYS_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO4_HYS_SEL_S  17

/** LP_IO_MUX_GPIO5_REG register
 *  LP IO MUX configuration register for LP_GPIO5
 */
#define LP_IO_MUX_GPIO5_REG (DR_REG_LP_IO_MUX_BASE + 0x14)
/** LP_IO_MUX_GPIO5_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO5 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_MCU_OE    (BIT(0))
#define LP_IO_MUX_GPIO5_MCU_OE_M  (LP_IO_MUX_GPIO5_MCU_OE_V << LP_IO_MUX_GPIO5_MCU_OE_S)
#define LP_IO_MUX_GPIO5_MCU_OE_V  0x00000001U
#define LP_IO_MUX_GPIO5_MCU_OE_S  0
/** LP_IO_MUX_GPIO5_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for LP_GPIO5.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define LP_IO_MUX_GPIO5_SLP_SEL    (BIT(1))
#define LP_IO_MUX_GPIO5_SLP_SEL_M  (LP_IO_MUX_GPIO5_SLP_SEL_V << LP_IO_MUX_GPIO5_SLP_SEL_S)
#define LP_IO_MUX_GPIO5_SLP_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO5_SLP_SEL_S  1
/** LP_IO_MUX_GPIO5_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of LP_GPIO5 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_MCU_WPD    (BIT(2))
#define LP_IO_MUX_GPIO5_MCU_WPD_M  (LP_IO_MUX_GPIO5_MCU_WPD_V << LP_IO_MUX_GPIO5_MCU_WPD_S)
#define LP_IO_MUX_GPIO5_MCU_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO5_MCU_WPD_S  2
/** LP_IO_MUX_GPIO5_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of LP_GPIO5 during sleep mode.
 *  \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_MCU_WPU    (BIT(3))
#define LP_IO_MUX_GPIO5_MCU_WPU_M  (LP_IO_MUX_GPIO5_MCU_WPU_V << LP_IO_MUX_GPIO5_MCU_WPU_S)
#define LP_IO_MUX_GPIO5_MCU_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO5_MCU_WPU_S  3
/** LP_IO_MUX_GPIO5_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of LP_GPIO5 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_MCU_IE    (BIT(4))
#define LP_IO_MUX_GPIO5_MCU_IE_M  (LP_IO_MUX_GPIO5_MCU_IE_V << LP_IO_MUX_GPIO5_MCU_IE_S)
#define LP_IO_MUX_GPIO5_MCU_IE_V  0x00000001U
#define LP_IO_MUX_GPIO5_MCU_IE_S  4
/** LP_IO_MUX_GPIO5_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of LP_GPIO5 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO5_MCU_DRV    0x00000003U
#define LP_IO_MUX_GPIO5_MCU_DRV_M  (LP_IO_MUX_GPIO5_MCU_DRV_V << LP_IO_MUX_GPIO5_MCU_DRV_S)
#define LP_IO_MUX_GPIO5_MCU_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO5_MCU_DRV_S  5
/** LP_IO_MUX_GPIO5_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of LP_GPIO5.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_FUN_WPD    (BIT(7))
#define LP_IO_MUX_GPIO5_FUN_WPD_M  (LP_IO_MUX_GPIO5_FUN_WPD_V << LP_IO_MUX_GPIO5_FUN_WPD_S)
#define LP_IO_MUX_GPIO5_FUN_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO5_FUN_WPD_S  7
/** LP_IO_MUX_GPIO5_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of LP_GPIO5.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_FUN_WPU    (BIT(8))
#define LP_IO_MUX_GPIO5_FUN_WPU_M  (LP_IO_MUX_GPIO5_FUN_WPU_V << LP_IO_MUX_GPIO5_FUN_WPU_S)
#define LP_IO_MUX_GPIO5_FUN_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO5_FUN_WPU_S  8
/** LP_IO_MUX_GPIO5_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of LP_GPIO5.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_FUN_IE    (BIT(9))
#define LP_IO_MUX_GPIO5_FUN_IE_M  (LP_IO_MUX_GPIO5_FUN_IE_V << LP_IO_MUX_GPIO5_FUN_IE_S)
#define LP_IO_MUX_GPIO5_FUN_IE_V  0x00000001U
#define LP_IO_MUX_GPIO5_FUN_IE_S  9
/** LP_IO_MUX_GPIO5_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of LP_GPIO5. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO5_FUN_DRV    0x00000003U
#define LP_IO_MUX_GPIO5_FUN_DRV_M  (LP_IO_MUX_GPIO5_FUN_DRV_V << LP_IO_MUX_GPIO5_FUN_DRV_S)
#define LP_IO_MUX_GPIO5_FUN_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO5_FUN_DRV_S  10
/** LP_IO_MUX_GPIO5_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select LP IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define LP_IO_MUX_GPIO5_MCU_SEL    0x00000007U
#define LP_IO_MUX_GPIO5_MCU_SEL_M  (LP_IO_MUX_GPIO5_MCU_SEL_V << LP_IO_MUX_GPIO5_MCU_SEL_S)
#define LP_IO_MUX_GPIO5_MCU_SEL_V  0x00000007U
#define LP_IO_MUX_GPIO5_MCU_SEL_S  12
/** LP_IO_MUX_GPIO5_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_FILTER_EN    (BIT(15))
#define LP_IO_MUX_GPIO5_FILTER_EN_M  (LP_IO_MUX_GPIO5_FILTER_EN_V << LP_IO_MUX_GPIO5_FILTER_EN_S)
#define LP_IO_MUX_GPIO5_FILTER_EN_V  0x00000001U
#define LP_IO_MUX_GPIO5_FILTER_EN_S  15
/** LP_IO_MUX_GPIO5_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  LP_IO_MUX_GPIO5_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO5_HYS_EN    (BIT(16))
#define LP_IO_MUX_GPIO5_HYS_EN_M  (LP_IO_MUX_GPIO5_HYS_EN_V << LP_IO_MUX_GPIO5_HYS_EN_S)
#define LP_IO_MUX_GPIO5_HYS_EN_V  0x00000001U
#define LP_IO_MUX_GPIO5_HYS_EN_S  16
/** LP_IO_MUX_GPIO5_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for LP_GPIO5.
 *  \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of LP_IO_MUX_GPIO5_HYS_EN\\
 */
#define LP_IO_MUX_GPIO5_HYS_SEL    (BIT(17))
#define LP_IO_MUX_GPIO5_HYS_SEL_M  (LP_IO_MUX_GPIO5_HYS_SEL_V << LP_IO_MUX_GPIO5_HYS_SEL_S)
#define LP_IO_MUX_GPIO5_HYS_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO5_HYS_SEL_S  17

/** LP_IO_MUX_GPIO6_REG register
 *  LP IO MUX configuration register for LP_GPIO6
 */
#define LP_IO_MUX_GPIO6_REG (DR_REG_LP_IO_MUX_BASE + 0x18)
/** LP_IO_MUX_GPIO6_MCU_OE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the output of LP_GPIO6 in sleep mode.
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_MCU_OE    (BIT(0))
#define LP_IO_MUX_GPIO6_MCU_OE_M  (LP_IO_MUX_GPIO6_MCU_OE_V << LP_IO_MUX_GPIO6_MCU_OE_S)
#define LP_IO_MUX_GPIO6_MCU_OE_V  0x00000001U
#define LP_IO_MUX_GPIO6_MCU_OE_S  0
/** LP_IO_MUX_GPIO6_SLP_SEL : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enter sleep mode for LP_GPIO6.\\
 *  0: Not enter\\
 *  1: Enter\\
 */
#define LP_IO_MUX_GPIO6_SLP_SEL    (BIT(1))
#define LP_IO_MUX_GPIO6_SLP_SEL_M  (LP_IO_MUX_GPIO6_SLP_SEL_V << LP_IO_MUX_GPIO6_SLP_SEL_S)
#define LP_IO_MUX_GPIO6_SLP_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO6_SLP_SEL_S  1
/** LP_IO_MUX_GPIO6_MCU_WPD : R/W; bitpos: [2]; default: 0;
 *  Configure whether or not to enable pull-down resistor of LP_GPIO6 in sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_MCU_WPD    (BIT(2))
#define LP_IO_MUX_GPIO6_MCU_WPD_M  (LP_IO_MUX_GPIO6_MCU_WPD_V << LP_IO_MUX_GPIO6_MCU_WPD_S)
#define LP_IO_MUX_GPIO6_MCU_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO6_MCU_WPD_S  2
/** LP_IO_MUX_GPIO6_MCU_WPU : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable pull-up resistor of LP_GPIO6 during sleep mode.
 *  \\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_MCU_WPU    (BIT(3))
#define LP_IO_MUX_GPIO6_MCU_WPU_M  (LP_IO_MUX_GPIO6_MCU_WPU_V << LP_IO_MUX_GPIO6_MCU_WPU_S)
#define LP_IO_MUX_GPIO6_MCU_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO6_MCU_WPU_S  3
/** LP_IO_MUX_GPIO6_MCU_IE : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the input of LP_GPIO6 during sleep mode.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_MCU_IE    (BIT(4))
#define LP_IO_MUX_GPIO6_MCU_IE_M  (LP_IO_MUX_GPIO6_MCU_IE_V << LP_IO_MUX_GPIO6_MCU_IE_S)
#define LP_IO_MUX_GPIO6_MCU_IE_V  0x00000001U
#define LP_IO_MUX_GPIO6_MCU_IE_S  4
/** LP_IO_MUX_GPIO6_MCU_DRV : R/W; bitpos: [6:5]; default: 0;
 *  Configures the drive strength of LP_GPIO6 during sleep mode. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO6_MCU_DRV    0x00000003U
#define LP_IO_MUX_GPIO6_MCU_DRV_M  (LP_IO_MUX_GPIO6_MCU_DRV_V << LP_IO_MUX_GPIO6_MCU_DRV_S)
#define LP_IO_MUX_GPIO6_MCU_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO6_MCU_DRV_S  5
/** LP_IO_MUX_GPIO6_FUN_WPD : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable pull-down resistor of LP_GPIO6.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_FUN_WPD    (BIT(7))
#define LP_IO_MUX_GPIO6_FUN_WPD_M  (LP_IO_MUX_GPIO6_FUN_WPD_V << LP_IO_MUX_GPIO6_FUN_WPD_S)
#define LP_IO_MUX_GPIO6_FUN_WPD_V  0x00000001U
#define LP_IO_MUX_GPIO6_FUN_WPD_S  7
/** LP_IO_MUX_GPIO6_FUN_WPU : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not enable pull-up resistor of LP_GPIO6.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_FUN_WPU    (BIT(8))
#define LP_IO_MUX_GPIO6_FUN_WPU_M  (LP_IO_MUX_GPIO6_FUN_WPU_V << LP_IO_MUX_GPIO6_FUN_WPU_S)
#define LP_IO_MUX_GPIO6_FUN_WPU_V  0x00000001U
#define LP_IO_MUX_GPIO6_FUN_WPU_S  8
/** LP_IO_MUX_GPIO6_FUN_IE : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable input of LP_GPIO6.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_FUN_IE    (BIT(9))
#define LP_IO_MUX_GPIO6_FUN_IE_M  (LP_IO_MUX_GPIO6_FUN_IE_V << LP_IO_MUX_GPIO6_FUN_IE_S)
#define LP_IO_MUX_GPIO6_FUN_IE_V  0x00000001U
#define LP_IO_MUX_GPIO6_FUN_IE_S  9
/** LP_IO_MUX_GPIO6_FUN_DRV : R/W; bitpos: [11:10]; default: 2;
 *  Configures the drive strength of LP_GPIO6. \\
 *  0: ~5 mA\\
 *  1: ~10 mA\\
 *  2: ~20 mA\\
 *  3: ~40 mA\\
 */
#define LP_IO_MUX_GPIO6_FUN_DRV    0x00000003U
#define LP_IO_MUX_GPIO6_FUN_DRV_M  (LP_IO_MUX_GPIO6_FUN_DRV_V << LP_IO_MUX_GPIO6_FUN_DRV_S)
#define LP_IO_MUX_GPIO6_FUN_DRV_V  0x00000003U
#define LP_IO_MUX_GPIO6_FUN_DRV_S  10
/** LP_IO_MUX_GPIO6_MCU_SEL : R/W; bitpos: [14:12]; default: 0;
 *  Configures to select LP IO MUX function for this signal. \\
 *  0: Select Function 0\\
 *  1: Select Function 1\\
 *  ......\\
 */
#define LP_IO_MUX_GPIO6_MCU_SEL    0x00000007U
#define LP_IO_MUX_GPIO6_MCU_SEL_M  (LP_IO_MUX_GPIO6_MCU_SEL_V << LP_IO_MUX_GPIO6_MCU_SEL_S)
#define LP_IO_MUX_GPIO6_MCU_SEL_V  0x00000007U
#define LP_IO_MUX_GPIO6_MCU_SEL_S  12
/** LP_IO_MUX_GPIO6_FILTER_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable filter for pin input signals.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_FILTER_EN    (BIT(15))
#define LP_IO_MUX_GPIO6_FILTER_EN_M  (LP_IO_MUX_GPIO6_FILTER_EN_V << LP_IO_MUX_GPIO6_FILTER_EN_S)
#define LP_IO_MUX_GPIO6_FILTER_EN_V  0x00000001U
#define LP_IO_MUX_GPIO6_FILTER_EN_S  15
/** LP_IO_MUX_GPIO6_HYS_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the hysteresis function of the pin when
 *  LP_IO_MUX_GPIO6_HYS_SEL is set to 1.\\
 *  0: Disable\\
 *  1: Enable\\
 */
#define LP_IO_MUX_GPIO6_HYS_EN    (BIT(16))
#define LP_IO_MUX_GPIO6_HYS_EN_M  (LP_IO_MUX_GPIO6_HYS_EN_V << LP_IO_MUX_GPIO6_HYS_EN_S)
#define LP_IO_MUX_GPIO6_HYS_EN_V  0x00000001U
#define LP_IO_MUX_GPIO6_HYS_EN_S  16
/** LP_IO_MUX_GPIO6_HYS_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures to choose the signal for enabling the hysteresis function for LP_GPIO6.
 *  \\
 *  0: Choose the output enable signal of eFuse\\
 *  1: Choose the output enable signal of LP_IO_MUX_GPIO6_HYS_EN\\
 */
#define LP_IO_MUX_GPIO6_HYS_SEL    (BIT(17))
#define LP_IO_MUX_GPIO6_HYS_SEL_M  (LP_IO_MUX_GPIO6_HYS_SEL_V << LP_IO_MUX_GPIO6_HYS_SEL_S)
#define LP_IO_MUX_GPIO6_HYS_SEL_V  0x00000001U
#define LP_IO_MUX_GPIO6_HYS_SEL_S  17

/** LP_IO_MUX_DATE_REG register
 *  Version control register
 */
#define LP_IO_MUX_DATE_REG (DR_REG_LP_IO_MUX_BASE + 0x1fc)
/** LP_IO_MUX_REG_DATE : R/W; bitpos: [27:0]; default: 36729408;
 *  Version control register
 */
#define LP_IO_MUX_REG_DATE    0x0FFFFFFFU
#define LP_IO_MUX_REG_DATE_M  (LP_IO_MUX_REG_DATE_V << LP_IO_MUX_REG_DATE_S)
#define LP_IO_MUX_REG_DATE_V  0x0FFFFFFFU
#define LP_IO_MUX_REG_DATE_S  0

#ifdef __cplusplus
}
#endif
