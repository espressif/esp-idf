/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** LP_CLKRST_LP_CLK_CONF_REG register
 *  Configures the root clk of LP system
 */
#define LP_CLKRST_LP_CLK_CONF_REG (DR_REG_LP_BASE + 0x0)
/** LP_CLKRST_SLOW_CLK_SEL : R/W; bitpos: [1:0]; default: 0;
 *  Configures the source of LP_SLOW_CLK.
 *  0: RC_SLOW_CLK
 *  1: XTAL32K_CLK
 *  2: RC32K_CLK
 *  3:OSC_SLOW_CLK
 */
#define LP_CLKRST_SLOW_CLK_SEL    0x00000003U
#define LP_CLKRST_SLOW_CLK_SEL_M  (LP_CLKRST_SLOW_CLK_SEL_V << LP_CLKRST_SLOW_CLK_SEL_S)
#define LP_CLKRST_SLOW_CLK_SEL_V  0x00000003U
#define LP_CLKRST_SLOW_CLK_SEL_S  0
/** LP_CLKRST_FAST_CLK_SEL : R/W; bitpos: [2]; default: 1;
 *  configures the source of LP_FAST_CLK.
 *  0: RC_FAST_CLK
 *  1: XTAL_D2_CLK
 */
#define LP_CLKRST_FAST_CLK_SEL    (BIT(2))
#define LP_CLKRST_FAST_CLK_SEL_M  (LP_CLKRST_FAST_CLK_SEL_V << LP_CLKRST_FAST_CLK_SEL_S)
#define LP_CLKRST_FAST_CLK_SEL_V  0x00000001U
#define LP_CLKRST_FAST_CLK_SEL_S  2
/** LP_CLKRST_LP_PERI_DIV_NUM : R/W; bitpos: [10:3]; default: 0;
 *  reserved
 */
#define LP_CLKRST_LP_PERI_DIV_NUM    0x000000FFU
#define LP_CLKRST_LP_PERI_DIV_NUM_M  (LP_CLKRST_LP_PERI_DIV_NUM_V << LP_CLKRST_LP_PERI_DIV_NUM_S)
#define LP_CLKRST_LP_PERI_DIV_NUM_V  0x000000FFU
#define LP_CLKRST_LP_PERI_DIV_NUM_S  3

/** LP_CLKRST_LP_CLK_PO_EN_REG register
 *  Configures the clk gate to pad
 */
#define LP_CLKRST_LP_CLK_PO_EN_REG (DR_REG_LP_BASE + 0x4)
/** LP_CLKRST_AON_SLOW_OEN : R/W; bitpos: [0]; default: 1;
 *  Configures the clock gate to pad of the LP_DYN_SLOW_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_AON_SLOW_OEN    (BIT(0))
#define LP_CLKRST_AON_SLOW_OEN_M  (LP_CLKRST_AON_SLOW_OEN_V << LP_CLKRST_AON_SLOW_OEN_S)
#define LP_CLKRST_AON_SLOW_OEN_V  0x00000001U
#define LP_CLKRST_AON_SLOW_OEN_S  0
/** LP_CLKRST_AON_FAST_OEN : R/W; bitpos: [1]; default: 1;
 *  Configures the clock gate to pad of the LP_DYN_FAST_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_AON_FAST_OEN    (BIT(1))
#define LP_CLKRST_AON_FAST_OEN_M  (LP_CLKRST_AON_FAST_OEN_V << LP_CLKRST_AON_FAST_OEN_S)
#define LP_CLKRST_AON_FAST_OEN_V  0x00000001U
#define LP_CLKRST_AON_FAST_OEN_S  1
/** LP_CLKRST_SOSC_OEN : R/W; bitpos: [2]; default: 1;
 *  Configures the clock gate to pad of the OSC_SLOW_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_SOSC_OEN    (BIT(2))
#define LP_CLKRST_SOSC_OEN_M  (LP_CLKRST_SOSC_OEN_V << LP_CLKRST_SOSC_OEN_S)
#define LP_CLKRST_SOSC_OEN_V  0x00000001U
#define LP_CLKRST_SOSC_OEN_S  2
/** LP_CLKRST_FOSC_OEN : R/W; bitpos: [3]; default: 1;
 *  Configures the clock gate to pad of the RC_FAST_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_FOSC_OEN    (BIT(3))
#define LP_CLKRST_FOSC_OEN_M  (LP_CLKRST_FOSC_OEN_V << LP_CLKRST_FOSC_OEN_S)
#define LP_CLKRST_FOSC_OEN_V  0x00000001U
#define LP_CLKRST_FOSC_OEN_S  3
/** LP_CLKRST_OSC32K_OEN : R/W; bitpos: [4]; default: 1;
 *  Configures the clock gate to pad of the RC32K_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_OSC32K_OEN    (BIT(4))
#define LP_CLKRST_OSC32K_OEN_M  (LP_CLKRST_OSC32K_OEN_V << LP_CLKRST_OSC32K_OEN_S)
#define LP_CLKRST_OSC32K_OEN_V  0x00000001U
#define LP_CLKRST_OSC32K_OEN_S  4
/** LP_CLKRST_XTAL32K_OEN : R/W; bitpos: [5]; default: 1;
 *  Configures the clock gate to pad of the XTAL32K_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_XTAL32K_OEN    (BIT(5))
#define LP_CLKRST_XTAL32K_OEN_M  (LP_CLKRST_XTAL32K_OEN_V << LP_CLKRST_XTAL32K_OEN_S)
#define LP_CLKRST_XTAL32K_OEN_V  0x00000001U
#define LP_CLKRST_XTAL32K_OEN_S  5
/** LP_CLKRST_CORE_EFUSE_OEN : R/W; bitpos: [6]; default: 1;
 *  Configures the clock gate to pad of the EFUSE_CTRL clock.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_CORE_EFUSE_OEN    (BIT(6))
#define LP_CLKRST_CORE_EFUSE_OEN_M  (LP_CLKRST_CORE_EFUSE_OEN_V << LP_CLKRST_CORE_EFUSE_OEN_S)
#define LP_CLKRST_CORE_EFUSE_OEN_V  0x00000001U
#define LP_CLKRST_CORE_EFUSE_OEN_S  6
/** LP_CLKRST_SLOW_OEN : R/W; bitpos: [7]; default: 1;
 *  Configures the clock gate to pad of the LP_SLOW_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_SLOW_OEN    (BIT(7))
#define LP_CLKRST_SLOW_OEN_M  (LP_CLKRST_SLOW_OEN_V << LP_CLKRST_SLOW_OEN_S)
#define LP_CLKRST_SLOW_OEN_V  0x00000001U
#define LP_CLKRST_SLOW_OEN_S  7
/** LP_CLKRST_FAST_OEN : R/W; bitpos: [8]; default: 1;
 *  Configures the clock gate to pad of the LP_FAST_CLK.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_FAST_OEN    (BIT(8))
#define LP_CLKRST_FAST_OEN_M  (LP_CLKRST_FAST_OEN_V << LP_CLKRST_FAST_OEN_S)
#define LP_CLKRST_FAST_OEN_V  0x00000001U
#define LP_CLKRST_FAST_OEN_S  8
/** LP_CLKRST_RNG_OEN : R/W; bitpos: [9]; default: 1;
 *  Configures the clock gate to pad of the RNG clk.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_RNG_OEN    (BIT(9))
#define LP_CLKRST_RNG_OEN_M  (LP_CLKRST_RNG_OEN_V << LP_CLKRST_RNG_OEN_S)
#define LP_CLKRST_RNG_OEN_V  0x00000001U
#define LP_CLKRST_RNG_OEN_S  9
/** LP_CLKRST_LPBUS_OEN : R/W; bitpos: [10]; default: 1;
 *  Configures the clock gate to pad of the LP bus clk.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_LPBUS_OEN    (BIT(10))
#define LP_CLKRST_LPBUS_OEN_M  (LP_CLKRST_LPBUS_OEN_V << LP_CLKRST_LPBUS_OEN_S)
#define LP_CLKRST_LPBUS_OEN_V  0x00000001U
#define LP_CLKRST_LPBUS_OEN_S  10

/** LP_CLKRST_LP_CLK_EN_REG register
 *  Configure LP root clk source gate
 */
#define LP_CLKRST_LP_CLK_EN_REG (DR_REG_LP_BASE + 0x8)
/** LP_CLKRST_RTC_BLE_TIMER_APB_GATE : R/W; bitpos: [27]; default: 1;
 *  Configures the clock gate to RTC_BLE_TIMER_APB_CLK
 *  0: Invalid. The clock gate controlled
 *  1: Force the clk pass clock gate
 */
#define LP_CLKRST_RTC_BLE_TIMER_APB_GATE    (BIT(27))
#define LP_CLKRST_RTC_BLE_TIMER_APB_GATE_M  (LP_CLKRST_RTC_BLE_TIMER_APB_GATE_V << LP_CLKRST_RTC_BLE_TIMER_APB_GATE_S)
#define LP_CLKRST_RTC_BLE_TIMER_APB_GATE_V  0x00000001U
#define LP_CLKRST_RTC_BLE_TIMER_APB_GATE_S  27
/** LP_CLKRST_TOTAL_CORE_EFUSE_GATE : R/W; bitpos: [28]; default: 1;
 *  Configures the clock gate to TOTAL_EFUSE_AON_CLK
 *  0: Invalid. The clock gate controlled by hardware fsm
 *  1: Force the clk pass clock gate
 */
#define LP_CLKRST_TOTAL_CORE_EFUSE_GATE    (BIT(28))
#define LP_CLKRST_TOTAL_CORE_EFUSE_GATE_M  (LP_CLKRST_TOTAL_CORE_EFUSE_GATE_V << LP_CLKRST_TOTAL_CORE_EFUSE_GATE_S)
#define LP_CLKRST_TOTAL_CORE_EFUSE_GATE_V  0x00000001U
#define LP_CLKRST_TOTAL_CORE_EFUSE_GATE_S  28
/** LP_CLKRST_AON_CORE_EFUSE_GATE : R/W; bitpos: [29]; default: 1;
 *  Configures the clock gate to CORE_EFUSE_AON_CLK
 *  0: Invalid. The clock gate controlled by hardware fsm
 *  1: Force the clk pass clock gate
 */
#define LP_CLKRST_AON_CORE_EFUSE_GATE    (BIT(29))
#define LP_CLKRST_AON_CORE_EFUSE_GATE_M  (LP_CLKRST_AON_CORE_EFUSE_GATE_V << LP_CLKRST_AON_CORE_EFUSE_GATE_S)
#define LP_CLKRST_AON_CORE_EFUSE_GATE_V  0x00000001U
#define LP_CLKRST_AON_CORE_EFUSE_GATE_S  29
/** LP_CLKRST_AON_TOUCH_GATE : R/W; bitpos: [30]; default: 1;
 *  Configures the clock gate to TOUCH_AON_CLK
 *  0: Invalid. The clock gate controlled by hardware fsm
 *  1: Force the clk pass clock gate
 */
#define LP_CLKRST_AON_TOUCH_GATE    (BIT(30))
#define LP_CLKRST_AON_TOUCH_GATE_M  (LP_CLKRST_AON_TOUCH_GATE_V << LP_CLKRST_AON_TOUCH_GATE_S)
#define LP_CLKRST_AON_TOUCH_GATE_V  0x00000001U
#define LP_CLKRST_AON_TOUCH_GATE_S  30
/** LP_CLKRST_FAST_ORI_GATE : R/W; bitpos: [31]; default: 0;
 *  Configures the clock gate to LP_FAST_CLK
 *  0: Invalid. The clock gate controlled by hardware fsm
 *  1: Force the clk pass clock gate
 */
#define LP_CLKRST_FAST_ORI_GATE    (BIT(31))
#define LP_CLKRST_FAST_ORI_GATE_M  (LP_CLKRST_FAST_ORI_GATE_V << LP_CLKRST_FAST_ORI_GATE_S)
#define LP_CLKRST_FAST_ORI_GATE_V  0x00000001U
#define LP_CLKRST_FAST_ORI_GATE_S  31

/** LP_CLKRST_LP_RST_EN_REG register
 *  Configures the peri of LP system software reset
 */
#define LP_CLKRST_LP_RST_EN_REG (DR_REG_LP_BASE + 0xc)
/** LP_CLKRST_HUK_RESET_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to reset  HUK
 *  0: Invalid.No effect
 *  1: Reset
 */
#define LP_CLKRST_HUK_RESET_EN    (BIT(25))
#define LP_CLKRST_HUK_RESET_EN_M  (LP_CLKRST_HUK_RESET_EN_V << LP_CLKRST_HUK_RESET_EN_S)
#define LP_CLKRST_HUK_RESET_EN_V  0x00000001U
#define LP_CLKRST_HUK_RESET_EN_S  25
/** LP_CLKRST_AON_BLETIMER_RESET_EN : R/W; bitpos: [26]; default: 0;
 *  Configures whether or not to reset  bletimer  part
 *  0: Invalid.No effect
 *  1: Reset
 */
#define LP_CLKRST_AON_BLETIMER_RESET_EN    (BIT(26))
#define LP_CLKRST_AON_BLETIMER_RESET_EN_M  (LP_CLKRST_AON_BLETIMER_RESET_EN_V << LP_CLKRST_AON_BLETIMER_RESET_EN_S)
#define LP_CLKRST_AON_BLETIMER_RESET_EN_V  0x00000001U
#define LP_CLKRST_AON_BLETIMER_RESET_EN_S  26
/** LP_CLKRST_AON_TOUCH_RESET_EN : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to reset  TOUCH  part
 *  0: Invalid.No effect
 *  1: Reset
 */
#define LP_CLKRST_AON_TOUCH_RESET_EN    (BIT(27))
#define LP_CLKRST_AON_TOUCH_RESET_EN_M  (LP_CLKRST_AON_TOUCH_RESET_EN_V << LP_CLKRST_AON_TOUCH_RESET_EN_S)
#define LP_CLKRST_AON_TOUCH_RESET_EN_V  0x00000001U
#define LP_CLKRST_AON_TOUCH_RESET_EN_S  27
/** LP_CLKRST_AON_EFUSE_CORE_RESET_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to reset  EFUSE_CTRL always-on part
 *  0: Invalid.No effect
 *  1: Reset
 */
#define LP_CLKRST_AON_EFUSE_CORE_RESET_EN    (BIT(28))
#define LP_CLKRST_AON_EFUSE_CORE_RESET_EN_M  (LP_CLKRST_AON_EFUSE_CORE_RESET_EN_V << LP_CLKRST_AON_EFUSE_CORE_RESET_EN_S)
#define LP_CLKRST_AON_EFUSE_CORE_RESET_EN_V  0x00000001U
#define LP_CLKRST_AON_EFUSE_CORE_RESET_EN_S  28
/** LP_CLKRST_LP_TIMER_RESET_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to reset  LP_TIMER
 *  0: Invalid.No effect
 *  1: Reset
 */
#define LP_CLKRST_LP_TIMER_RESET_EN    (BIT(29))
#define LP_CLKRST_LP_TIMER_RESET_EN_M  (LP_CLKRST_LP_TIMER_RESET_EN_V << LP_CLKRST_LP_TIMER_RESET_EN_S)
#define LP_CLKRST_LP_TIMER_RESET_EN_V  0x00000001U
#define LP_CLKRST_LP_TIMER_RESET_EN_S  29
/** LP_CLKRST_WDT_RESET_EN : R/W; bitpos: [30]; default: 0;
 *  Configures whether or not to reset LP_WDT and super watch dog
 *  0: Invalid.No effect
 *  1: Reset
 */
#define LP_CLKRST_WDT_RESET_EN    (BIT(30))
#define LP_CLKRST_WDT_RESET_EN_M  (LP_CLKRST_WDT_RESET_EN_V << LP_CLKRST_WDT_RESET_EN_S)
#define LP_CLKRST_WDT_RESET_EN_V  0x00000001U
#define LP_CLKRST_WDT_RESET_EN_S  30
/** LP_CLKRST_ANA_PERI_RESET_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not to reset analog peri, include brownout controller
 *  0: Invalid.No effect
 *  1: Reset
 */
#define LP_CLKRST_ANA_PERI_RESET_EN    (BIT(31))
#define LP_CLKRST_ANA_PERI_RESET_EN_M  (LP_CLKRST_ANA_PERI_RESET_EN_V << LP_CLKRST_ANA_PERI_RESET_EN_S)
#define LP_CLKRST_ANA_PERI_RESET_EN_V  0x00000001U
#define LP_CLKRST_ANA_PERI_RESET_EN_S  31

/** LP_CLKRST_RESET_CORE0_CAUSE_REG register
 *  Represents the reset cause
 */
#define LP_CLKRST_RESET_CORE0_CAUSE_REG (DR_REG_LP_BASE + 0x10)
/** LP_CLKRST_CORE0_RESET_CAUSE : RO; bitpos: [4:0]; default: 0;
 *  Represents the reset cause
 */
#define LP_CLKRST_CORE0_RESET_CAUSE    0x0000001FU
#define LP_CLKRST_CORE0_RESET_CAUSE_M  (LP_CLKRST_CORE0_RESET_CAUSE_V << LP_CLKRST_CORE0_RESET_CAUSE_S)
#define LP_CLKRST_CORE0_RESET_CAUSE_V  0x0000001FU
#define LP_CLKRST_CORE0_RESET_CAUSE_S  0
/** LP_CLKRST_CORE0_RESET_FLAG : RO; bitpos: [5]; default: 1;
 *  Represents the reset flag
 */
#define LP_CLKRST_CORE0_RESET_FLAG    (BIT(5))
#define LP_CLKRST_CORE0_RESET_FLAG_M  (LP_CLKRST_CORE0_RESET_FLAG_V << LP_CLKRST_CORE0_RESET_FLAG_S)
#define LP_CLKRST_CORE0_RESET_FLAG_V  0x00000001U
#define LP_CLKRST_CORE0_RESET_FLAG_S  5
/** LP_CLKRST_CORE0_RESET_CAUSE_CLR : WT; bitpos: [29]; default: 0;
 *  0: no operation
 */
#define LP_CLKRST_CORE0_RESET_CAUSE_CLR    (BIT(29))
#define LP_CLKRST_CORE0_RESET_CAUSE_CLR_M  (LP_CLKRST_CORE0_RESET_CAUSE_CLR_V << LP_CLKRST_CORE0_RESET_CAUSE_CLR_S)
#define LP_CLKRST_CORE0_RESET_CAUSE_CLR_V  0x00000001U
#define LP_CLKRST_CORE0_RESET_CAUSE_CLR_S  29
/** LP_CLKRST_CORE0_RESET_FLAG_SET : WT; bitpos: [30]; default: 0;
 *  configure set reset flag
 */
#define LP_CLKRST_CORE0_RESET_FLAG_SET    (BIT(30))
#define LP_CLKRST_CORE0_RESET_FLAG_SET_M  (LP_CLKRST_CORE0_RESET_FLAG_SET_V << LP_CLKRST_CORE0_RESET_FLAG_SET_S)
#define LP_CLKRST_CORE0_RESET_FLAG_SET_V  0x00000001U
#define LP_CLKRST_CORE0_RESET_FLAG_SET_S  30
/** LP_CLKRST_CORE0_RESET_FLAG_CLR : WT; bitpos: [31]; default: 0;
 *  configure clear reset flag
 *  0: no operation
 *  1: clear flag to 0
 */
#define LP_CLKRST_CORE0_RESET_FLAG_CLR    (BIT(31))
#define LP_CLKRST_CORE0_RESET_FLAG_CLR_M  (LP_CLKRST_CORE0_RESET_FLAG_CLR_V << LP_CLKRST_CORE0_RESET_FLAG_CLR_S)
#define LP_CLKRST_CORE0_RESET_FLAG_CLR_V  0x00000001U
#define LP_CLKRST_CORE0_RESET_FLAG_CLR_S  31

/** LP_CLKRST_RESET_CORE1_CAUSE_REG register
 *  Represents the reset cause
 */
#define LP_CLKRST_RESET_CORE1_CAUSE_REG (DR_REG_LP_BASE + 0x14)
/** LP_CLKRST_CORE1_RESET_CAUSE : RO; bitpos: [4:0]; default: 0;
 *  Represents the reset cause
 */
#define LP_CLKRST_CORE1_RESET_CAUSE    0x0000001FU
#define LP_CLKRST_CORE1_RESET_CAUSE_M  (LP_CLKRST_CORE1_RESET_CAUSE_V << LP_CLKRST_CORE1_RESET_CAUSE_S)
#define LP_CLKRST_CORE1_RESET_CAUSE_V  0x0000001FU
#define LP_CLKRST_CORE1_RESET_CAUSE_S  0
/** LP_CLKRST_CORE1_RESET_FLAG : RO; bitpos: [5]; default: 1;
 *  Represents the reset flag
 */
#define LP_CLKRST_CORE1_RESET_FLAG    (BIT(5))
#define LP_CLKRST_CORE1_RESET_FLAG_M  (LP_CLKRST_CORE1_RESET_FLAG_V << LP_CLKRST_CORE1_RESET_FLAG_S)
#define LP_CLKRST_CORE1_RESET_FLAG_V  0x00000001U
#define LP_CLKRST_CORE1_RESET_FLAG_S  5
/** LP_CLKRST_CORE1_RESET_CAUSE_CLR : WT; bitpos: [29]; default: 0;
 *  0: no operation
 */
#define LP_CLKRST_CORE1_RESET_CAUSE_CLR    (BIT(29))
#define LP_CLKRST_CORE1_RESET_CAUSE_CLR_M  (LP_CLKRST_CORE1_RESET_CAUSE_CLR_V << LP_CLKRST_CORE1_RESET_CAUSE_CLR_S)
#define LP_CLKRST_CORE1_RESET_CAUSE_CLR_V  0x00000001U
#define LP_CLKRST_CORE1_RESET_CAUSE_CLR_S  29
/** LP_CLKRST_CORE1_RESET_FLAG_SET : WT; bitpos: [30]; default: 0;
 *  configure set reset flag
 */
#define LP_CLKRST_CORE1_RESET_FLAG_SET    (BIT(30))
#define LP_CLKRST_CORE1_RESET_FLAG_SET_M  (LP_CLKRST_CORE1_RESET_FLAG_SET_V << LP_CLKRST_CORE1_RESET_FLAG_SET_S)
#define LP_CLKRST_CORE1_RESET_FLAG_SET_V  0x00000001U
#define LP_CLKRST_CORE1_RESET_FLAG_SET_S  30
/** LP_CLKRST_CORE1_RESET_FLAG_CLR : WT; bitpos: [31]; default: 0;
 *  configure clear reset flag
 *  0: no operation
 *  1: clear flag to 0
 */
#define LP_CLKRST_CORE1_RESET_FLAG_CLR    (BIT(31))
#define LP_CLKRST_CORE1_RESET_FLAG_CLR_M  (LP_CLKRST_CORE1_RESET_FLAG_CLR_V << LP_CLKRST_CORE1_RESET_FLAG_CLR_S)
#define LP_CLKRST_CORE1_RESET_FLAG_CLR_V  0x00000001U
#define LP_CLKRST_CORE1_RESET_FLAG_CLR_S  31

/** LP_CLKRST_CPU_CORE0_RESET_REG register
 *  Configures CPU reset
 */
#define LP_CLKRST_CPU_CORE0_RESET_REG (DR_REG_LP_BASE + 0x18)
/** LP_CLKRST_HPCORE0_LOCKUP_RESET_EN : R/W; bitpos: [21]; default: 1;
 *  configure the hpcore0 luckup reset enable
 *  0: disable
 *  1:enable
 */
#define LP_CLKRST_HPCORE0_LOCKUP_RESET_EN    (BIT(21))
#define LP_CLKRST_HPCORE0_LOCKUP_RESET_EN_M  (LP_CLKRST_HPCORE0_LOCKUP_RESET_EN_V << LP_CLKRST_HPCORE0_LOCKUP_RESET_EN_S)
#define LP_CLKRST_HPCORE0_LOCKUP_RESET_EN_V  0x00000001U
#define LP_CLKRST_HPCORE0_LOCKUP_RESET_EN_S  21
/** LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_LENGTH : R/W; bitpos: [24:22]; default: 1;
 *  configures the reset length of LP_WDT reset CPU
 *  Measurement unit: LP_DYN_FAST_CLK
 */
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_LENGTH    0x00000007U
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_LENGTH_M  (LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_LENGTH_V << LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_LENGTH_S)
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_LENGTH_V  0x00000007U
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_LENGTH_S  22
/** LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not LP_WDT can reset CPU
 *  0: LP_WDT could not reset CPU when LP_WDT timeout
 *  1: LP_WDT could reset CPU when LP_WDT timeout
 */
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_EN    (BIT(25))
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_EN_M  (LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_EN_V << LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_EN_S)
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_EN_V  0x00000001U
#define LP_CLKRST_RTC_WDT_CPU_CORE0_RESET_EN_S  25
/** LP_CLKRST_CPU_CORE0_STALL_WAIT : R/W; bitpos: [30:26]; default: 1;
 *  configure the time between CPU stall and reset
 *  Measurement unit: LP_DYN_FAST_CLK
 */
#define LP_CLKRST_CPU_CORE0_STALL_WAIT    0x0000001FU
#define LP_CLKRST_CPU_CORE0_STALL_WAIT_M  (LP_CLKRST_CPU_CORE0_STALL_WAIT_V << LP_CLKRST_CPU_CORE0_STALL_WAIT_S)
#define LP_CLKRST_CPU_CORE0_STALL_WAIT_V  0x0000001FU
#define LP_CLKRST_CPU_CORE0_STALL_WAIT_S  26
/** LP_CLKRST_CPU_CORE0_STALL_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not CPU entry stall state before LP_WDT and software reset CPU
 *  0: CPU will not entry stall state before LP_WDT and software reset CPU
 *  1: CPU will entry stall state before LP_WDT and software reset CPU
 */
#define LP_CLKRST_CPU_CORE0_STALL_EN    (BIT(31))
#define LP_CLKRST_CPU_CORE0_STALL_EN_M  (LP_CLKRST_CPU_CORE0_STALL_EN_V << LP_CLKRST_CPU_CORE0_STALL_EN_S)
#define LP_CLKRST_CPU_CORE0_STALL_EN_V  0x00000001U
#define LP_CLKRST_CPU_CORE0_STALL_EN_S  31

/** LP_CLKRST_CPU_CORE1_RESET_REG register
 *  Configures CPU reset
 */
#define LP_CLKRST_CPU_CORE1_RESET_REG (DR_REG_LP_BASE + 0x1c)
/** LP_CLKRST_HPCORE1_LOCKUP_RESET_EN : R/W; bitpos: [21]; default: 1;
 *  configure the hpcore0 luckup reset enable
 *  0: disable
 *  1:enable
 */
#define LP_CLKRST_HPCORE1_LOCKUP_RESET_EN    (BIT(21))
#define LP_CLKRST_HPCORE1_LOCKUP_RESET_EN_M  (LP_CLKRST_HPCORE1_LOCKUP_RESET_EN_V << LP_CLKRST_HPCORE1_LOCKUP_RESET_EN_S)
#define LP_CLKRST_HPCORE1_LOCKUP_RESET_EN_V  0x00000001U
#define LP_CLKRST_HPCORE1_LOCKUP_RESET_EN_S  21
/** LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_LENGTH : R/W; bitpos: [24:22]; default: 1;
 *  configures the reset length of LP_WDT reset CPU
 *  Measurement unit: LP_DYN_FAST_CLK
 */
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_LENGTH    0x00000007U
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_LENGTH_M  (LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_LENGTH_V << LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_LENGTH_S)
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_LENGTH_V  0x00000007U
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_LENGTH_S  22
/** LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not LP_WDT can reset CPU
 *  0: LP_WDT could not reset CPU when LP_WDT timeout
 *  1: LP_WDT could reset CPU when LP_WDT timeout
 */
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_EN    (BIT(25))
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_EN_M  (LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_EN_V << LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_EN_S)
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_EN_V  0x00000001U
#define LP_CLKRST_RTC_WDT_CPU_CORE1_RESET_EN_S  25
/** LP_CLKRST_CPU_CORE1_STALL_WAIT : R/W; bitpos: [30:26]; default: 1;
 *  configure the time between CPU stall and reset
 *  Measurement unit: LP_DYN_FAST_CLK
 */
#define LP_CLKRST_CPU_CORE1_STALL_WAIT    0x0000001FU
#define LP_CLKRST_CPU_CORE1_STALL_WAIT_M  (LP_CLKRST_CPU_CORE1_STALL_WAIT_V << LP_CLKRST_CPU_CORE1_STALL_WAIT_S)
#define LP_CLKRST_CPU_CORE1_STALL_WAIT_V  0x0000001FU
#define LP_CLKRST_CPU_CORE1_STALL_WAIT_S  26
/** LP_CLKRST_CPU_CORE1_STALL_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not CPU entry stall state before LP_WDT and software reset CPU
 *  0: CPU will not entry stall state before LP_WDT and software reset CPU
 *  1: CPU will entry stall state before LP_WDT and software reset CPU
 */
#define LP_CLKRST_CPU_CORE1_STALL_EN    (BIT(31))
#define LP_CLKRST_CPU_CORE1_STALL_EN_M  (LP_CLKRST_CPU_CORE1_STALL_EN_V << LP_CLKRST_CPU_CORE1_STALL_EN_S)
#define LP_CLKRST_CPU_CORE1_STALL_EN_V  0x00000001U
#define LP_CLKRST_CPU_CORE1_STALL_EN_S  31

/** LP_CLKRST_FOSC_CNTL_REG register
 *  Configures the RC_FAST_CLK frequency
 */
#define LP_CLKRST_FOSC_CNTL_REG (DR_REG_LP_BASE + 0x20)
/** LP_CLKRST_FOSC_DFREQ : R/W; bitpos: [31:22]; default: 547;
 *  Configures the RC_FAST_CLK frequency,the clock frequency will increase with this
 *  field
 */
#define LP_CLKRST_FOSC_DFREQ    0x000003FFU
#define LP_CLKRST_FOSC_DFREQ_M  (LP_CLKRST_FOSC_DFREQ_V << LP_CLKRST_FOSC_DFREQ_S)
#define LP_CLKRST_FOSC_DFREQ_V  0x000003FFU
#define LP_CLKRST_FOSC_DFREQ_S  22

/** LP_CLKRST_SOSC_CNTL_REG register
 *  Configures the RC_SLOW_CLK frequency
 */
#define LP_CLKRST_SOSC_CNTL_REG (DR_REG_LP_BASE + 0x24)
/** LP_CLKRST_SLOW_DFREQ : R/W; bitpos: [31:26]; default: 10;
 *  Configures the RC_SLOW_CLK frequency,the clock frequency will increase with this
 *  field
 */
#define LP_CLKRST_SLOW_DFREQ    0x0000003FU
#define LP_CLKRST_SLOW_DFREQ_M  (LP_CLKRST_SLOW_DFREQ_V << LP_CLKRST_SLOW_DFREQ_S)
#define LP_CLKRST_SLOW_DFREQ_V  0x0000003FU
#define LP_CLKRST_SLOW_DFREQ_S  26

/** LP_CLKRST_RC32K_CNTL_REG register
 *  Configures the RC32K_CLK frequency
 */
#define LP_CLKRST_RC32K_CNTL_REG (DR_REG_LP_BASE + 0x28)
/** LP_CLKRST_RC32K_DFREQ : R/W; bitpos: [31:22]; default: 172;
 *  Configures the RC32K_CLK frequency, the clock frequency will increase with this
 *  field
 */
#define LP_CLKRST_RC32K_DFREQ    0x000003FFU
#define LP_CLKRST_RC32K_DFREQ_M  (LP_CLKRST_RC32K_DFREQ_V << LP_CLKRST_RC32K_DFREQ_S)
#define LP_CLKRST_RC32K_DFREQ_V  0x000003FFU
#define LP_CLKRST_RC32K_DFREQ_S  22

/** LP_CLKRST_CLK_TO_HP_REG register
 *  Configures the clk gate of LP clk to HP system
 */
#define LP_CLKRST_CLK_TO_HP_REG (DR_REG_LP_BASE + 0x2c)
/** LP_CLKRST_CLK_PWR_FOSC_EN : R/W; bitpos: [25]; default: 1;
 *  Configures the clock gate to modem of the fosc clk.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_CLK_PWR_FOSC_EN    (BIT(25))
#define LP_CLKRST_CLK_PWR_FOSC_EN_M  (LP_CLKRST_CLK_PWR_FOSC_EN_V << LP_CLKRST_CLK_PWR_FOSC_EN_S)
#define LP_CLKRST_CLK_PWR_FOSC_EN_V  0x00000001U
#define LP_CLKRST_CLK_PWR_FOSC_EN_S  25
/** LP_CLKRST_CLK_PWR_XTAL_EN : R/W; bitpos: [26]; default: 1;
 *  Configures the clock gate to modem of the xtal clk.
 *  0: Disable the clk pass clock gate
 *  1: Enable the clk pass clock gate
 */
#define LP_CLKRST_CLK_PWR_XTAL_EN    (BIT(26))
#define LP_CLKRST_CLK_PWR_XTAL_EN_M  (LP_CLKRST_CLK_PWR_XTAL_EN_V << LP_CLKRST_CLK_PWR_XTAL_EN_S)
#define LP_CLKRST_CLK_PWR_XTAL_EN_V  0x00000001U
#define LP_CLKRST_CLK_PWR_XTAL_EN_S  26
/** LP_CLKRST_ICG_HP_XTAL32K : R/W; bitpos: [28]; default: 1;
 *  Configures the clk gate of XTAL32K_CLK to HP system
 *  0: The clk could not pass to HP system
 *  1: The clk could pass to HP system
 */
#define LP_CLKRST_ICG_HP_XTAL32K    (BIT(28))
#define LP_CLKRST_ICG_HP_XTAL32K_M  (LP_CLKRST_ICG_HP_XTAL32K_V << LP_CLKRST_ICG_HP_XTAL32K_S)
#define LP_CLKRST_ICG_HP_XTAL32K_V  0x00000001U
#define LP_CLKRST_ICG_HP_XTAL32K_S  28
/** LP_CLKRST_ICG_HP_SOSC : R/W; bitpos: [29]; default: 1;
 *  Configures the clk gate of RC_SLOW_CLK to HP system
 *  0: The clk could not pass to HP system
 *  1: The clk could pass to HP system
 */
#define LP_CLKRST_ICG_HP_SOSC    (BIT(29))
#define LP_CLKRST_ICG_HP_SOSC_M  (LP_CLKRST_ICG_HP_SOSC_V << LP_CLKRST_ICG_HP_SOSC_S)
#define LP_CLKRST_ICG_HP_SOSC_V  0x00000001U
#define LP_CLKRST_ICG_HP_SOSC_S  29
/** LP_CLKRST_ICG_HP_OSC32K : R/W; bitpos: [30]; default: 1;
 *  Configures the clk gate of RC32K_CLK to HP system
 *  0: The clk could not pass to HP system
 *  1: The clk could pass to HP system
 */
#define LP_CLKRST_ICG_HP_OSC32K    (BIT(30))
#define LP_CLKRST_ICG_HP_OSC32K_M  (LP_CLKRST_ICG_HP_OSC32K_V << LP_CLKRST_ICG_HP_OSC32K_S)
#define LP_CLKRST_ICG_HP_OSC32K_V  0x00000001U
#define LP_CLKRST_ICG_HP_OSC32K_S  30
/** LP_CLKRST_ICG_HP_FOSC : R/W; bitpos: [31]; default: 1;
 *  Configures the clk gate of RC_FAST_CLK to HP system
 *  0: The clk could not pass to HP system
 *  1: The clk could pass to HP system
 */
#define LP_CLKRST_ICG_HP_FOSC    (BIT(31))
#define LP_CLKRST_ICG_HP_FOSC_M  (LP_CLKRST_ICG_HP_FOSC_V << LP_CLKRST_ICG_HP_FOSC_S)
#define LP_CLKRST_ICG_HP_FOSC_V  0x00000001U
#define LP_CLKRST_ICG_HP_FOSC_S  31

/** LP_CLKRST_LPMEM_FORCE_REG register
 *  Configures the LP_MEM clk gate force parameter
 */
#define LP_CLKRST_LPMEM_FORCE_REG (DR_REG_LP_BASE + 0x30)
/** LP_CLKRST_LPMEM_CLK_FORCE_ON : R/W; bitpos: [31]; default: 0;
 *  Configures whether ot not force open the clock gate of LP MEM
 *  0: Invalid. The clock gate controlled by hardware FSM
 *  1: Force open clock gate of LP MEM
 */
#define LP_CLKRST_LPMEM_CLK_FORCE_ON    (BIT(31))
#define LP_CLKRST_LPMEM_CLK_FORCE_ON_M  (LP_CLKRST_LPMEM_CLK_FORCE_ON_V << LP_CLKRST_LPMEM_CLK_FORCE_ON_S)
#define LP_CLKRST_LPMEM_CLK_FORCE_ON_V  0x00000001U
#define LP_CLKRST_LPMEM_CLK_FORCE_ON_S  31

/** LP_CLKRST_XTAL32K_REG register
 *  Configures the XTAL32K parameter
 */
#define LP_CLKRST_XTAL32K_REG (DR_REG_LP_BASE + 0x34)
/** LP_CLKRST_RTC_SEL_POWER_XTAL32K : R/W; bitpos: [21]; default: 0;
 *  need_des
 */
#define LP_CLKRST_RTC_SEL_POWER_XTAL32K    (BIT(21))
#define LP_CLKRST_RTC_SEL_POWER_XTAL32K_M  (LP_CLKRST_RTC_SEL_POWER_XTAL32K_V << LP_CLKRST_RTC_SEL_POWER_XTAL32K_S)
#define LP_CLKRST_RTC_SEL_POWER_XTAL32K_V  0x00000001U
#define LP_CLKRST_RTC_SEL_POWER_XTAL32K_S  21
/** LP_CLKRST_DRES_XTAL32K : R/W; bitpos: [24:22]; default: 3;
 *  Configures DRES
 */
#define LP_CLKRST_DRES_XTAL32K    0x00000007U
#define LP_CLKRST_DRES_XTAL32K_M  (LP_CLKRST_DRES_XTAL32K_V << LP_CLKRST_DRES_XTAL32K_S)
#define LP_CLKRST_DRES_XTAL32K_V  0x00000007U
#define LP_CLKRST_DRES_XTAL32K_S  22
/** LP_CLKRST_DGM_XTAL32K : R/W; bitpos: [27:25]; default: 3;
 *  Configures DGM
 */
#define LP_CLKRST_DGM_XTAL32K    0x00000007U
#define LP_CLKRST_DGM_XTAL32K_M  (LP_CLKRST_DGM_XTAL32K_V << LP_CLKRST_DGM_XTAL32K_S)
#define LP_CLKRST_DGM_XTAL32K_V  0x00000007U
#define LP_CLKRST_DGM_XTAL32K_S  25
/** LP_CLKRST_DBUF_XTAL32K : R/W; bitpos: [28]; default: 0;
 *  Configures DBUF
 */
#define LP_CLKRST_DBUF_XTAL32K    (BIT(28))
#define LP_CLKRST_DBUF_XTAL32K_M  (LP_CLKRST_DBUF_XTAL32K_V << LP_CLKRST_DBUF_XTAL32K_S)
#define LP_CLKRST_DBUF_XTAL32K_V  0x00000001U
#define LP_CLKRST_DBUF_XTAL32K_S  28
/** LP_CLKRST_DAC_XTAL32K : R/W; bitpos: [31:29]; default: 3;
 *  Configures DAC
 */
#define LP_CLKRST_DAC_XTAL32K    0x00000007U
#define LP_CLKRST_DAC_XTAL32K_M  (LP_CLKRST_DAC_XTAL32K_V << LP_CLKRST_DAC_XTAL32K_S)
#define LP_CLKRST_DAC_XTAL32K_V  0x00000007U
#define LP_CLKRST_DAC_XTAL32K_S  29

/** LP_CLKRST_CALI0_REG register
 *  need_des
 */
#define LP_CLKRST_CALI0_REG (DR_REG_LP_BASE + 0x38)
/** LP_CLKRST_LP_CALI_DIV_CYCLE : R/W; bitpos: [7:0]; default: 1;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_CYCLE    0x000000FFU
#define LP_CLKRST_LP_CALI_DIV_CYCLE_M  (LP_CLKRST_LP_CALI_DIV_CYCLE_V << LP_CLKRST_LP_CALI_DIV_CYCLE_S)
#define LP_CLKRST_LP_CALI_DIV_CYCLE_V  0x000000FFU
#define LP_CLKRST_LP_CALI_DIV_CYCLE_S  0
/** LP_CLKRST_LP_CALI_FULL_CNT_DONE : RO; bitpos: [8]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_FULL_CNT_DONE    (BIT(8))
#define LP_CLKRST_LP_CALI_FULL_CNT_DONE_M  (LP_CLKRST_LP_CALI_FULL_CNT_DONE_V << LP_CLKRST_LP_CALI_FULL_CNT_DONE_S)
#define LP_CLKRST_LP_CALI_FULL_CNT_DONE_V  0x00000001U
#define LP_CLKRST_LP_CALI_FULL_CNT_DONE_S  8
/** LP_CLKRST_LP_CALI_DIV_CALI_CNT : RO; bitpos: [24:9]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_CALI_CNT    0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_CALI_CNT_M  (LP_CLKRST_LP_CALI_DIV_CALI_CNT_V << LP_CLKRST_LP_CALI_DIV_CALI_CNT_S)
#define LP_CLKRST_LP_CALI_DIV_CALI_CNT_V  0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_CALI_CNT_S  9
/** LP_CLKRST_LP_CALI_DIV_NUMERATOR_TYPE : RO; bitpos: [25]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR_TYPE    (BIT(25))
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR_TYPE_M  (LP_CLKRST_LP_CALI_DIV_NUMERATOR_TYPE_V << LP_CLKRST_LP_CALI_DIV_NUMERATOR_TYPE_S)
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR_TYPE_V  0x00000001U
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR_TYPE_S  25
/** LP_CLKRST_LP_CALI_DIV_NUM : RO; bitpos: [31:26]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_NUM    0x0000003FU
#define LP_CLKRST_LP_CALI_DIV_NUM_M  (LP_CLKRST_LP_CALI_DIV_NUM_V << LP_CLKRST_LP_CALI_DIV_NUM_S)
#define LP_CLKRST_LP_CALI_DIV_NUM_V  0x0000003FU
#define LP_CLKRST_LP_CALI_DIV_NUM_S  26

/** LP_CLKRST_CALI1_REG register
 *  need_des
 */
#define LP_CLKRST_CALI1_REG (DR_REG_LP_BASE + 0x3c)
/** LP_CLKRST_LP_CALI_DIV_NUMERATOR : RO; bitpos: [15:0]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR    0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR_M  (LP_CLKRST_LP_CALI_DIV_NUMERATOR_V << LP_CLKRST_LP_CALI_DIV_NUMERATOR_S)
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR_V  0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_NUMERATOR_S  0
/** LP_CLKRST_LP_CALI_DIV_DENOMINATOR : RO; bitpos: [31:16]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_DENOMINATOR    0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_DENOMINATOR_M  (LP_CLKRST_LP_CALI_DIV_DENOMINATOR_V << LP_CLKRST_LP_CALI_DIV_DENOMINATOR_S)
#define LP_CLKRST_LP_CALI_DIV_DENOMINATOR_V  0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_DENOMINATOR_S  16

/** LP_CLKRST_CALI2_REG register
 *  need_des
 */
#define LP_CLKRST_CALI2_REG (DR_REG_LP_BASE + 0x40)
/** LP_CLKRST_LP_CALI_DIV_WAIT_PWR_GOOD : R/W; bitpos: [8:0]; default: 255;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_WAIT_PWR_GOOD    0x000001FFU
#define LP_CLKRST_LP_CALI_DIV_WAIT_PWR_GOOD_M  (LP_CLKRST_LP_CALI_DIV_WAIT_PWR_GOOD_V << LP_CLKRST_LP_CALI_DIV_WAIT_PWR_GOOD_S)
#define LP_CLKRST_LP_CALI_DIV_WAIT_PWR_GOOD_V  0x000001FFU
#define LP_CLKRST_LP_CALI_DIV_WAIT_PWR_GOOD_S  0
/** LP_CLKRST_LP_CALI_DIV_SLP_VAL : R/W; bitpos: [30:15]; default: 1;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_SLP_VAL    0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_SLP_VAL_M  (LP_CLKRST_LP_CALI_DIV_SLP_VAL_V << LP_CLKRST_LP_CALI_DIV_SLP_VAL_S)
#define LP_CLKRST_LP_CALI_DIV_SLP_VAL_V  0x0000FFFFU
#define LP_CLKRST_LP_CALI_DIV_SLP_VAL_S  15
/** LP_CLKRST_LP_CALI_DIV_TIMER_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_CALI_DIV_TIMER_EN    (BIT(31))
#define LP_CLKRST_LP_CALI_DIV_TIMER_EN_M  (LP_CLKRST_LP_CALI_DIV_TIMER_EN_V << LP_CLKRST_LP_CALI_DIV_TIMER_EN_S)
#define LP_CLKRST_LP_CALI_DIV_TIMER_EN_V  0x00000001U
#define LP_CLKRST_LP_CALI_DIV_TIMER_EN_S  31

/** LP_CLKRST_LPPERI_REG register
 *  need_des
 */
#define LP_CLKRST_LPPERI_REG (DR_REG_LP_BASE + 0x44)
/** LP_CLKRST_HUK_CLK_SEL : R/W; bitpos: [11]; default: 1;
 *  Configures the source clk of HUK
 *  0: 0: RC_FAST_CLK
 *  1: XTAL_D2_CLK
 */
#define LP_CLKRST_HUK_CLK_SEL    (BIT(11))
#define LP_CLKRST_HUK_CLK_SEL_M  (LP_CLKRST_HUK_CLK_SEL_V << LP_CLKRST_HUK_CLK_SEL_S)
#define LP_CLKRST_HUK_CLK_SEL_V  0x00000001U
#define LP_CLKRST_HUK_CLK_SEL_S  11
/** LP_CLKRST_LP_BLETIMER_DIV_NUM : R/W; bitpos: [23:12]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_BLETIMER_DIV_NUM    0x00000FFFU
#define LP_CLKRST_LP_BLETIMER_DIV_NUM_M  (LP_CLKRST_LP_BLETIMER_DIV_NUM_V << LP_CLKRST_LP_BLETIMER_DIV_NUM_S)
#define LP_CLKRST_LP_BLETIMER_DIV_NUM_V  0x00000FFFU
#define LP_CLKRST_LP_BLETIMER_DIV_NUM_S  12
/** LP_CLKRST_LP_BLETIMER_32K_SEL : R/W; bitpos: [25:24]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_BLETIMER_32K_SEL    0x00000003U
#define LP_CLKRST_LP_BLETIMER_32K_SEL_M  (LP_CLKRST_LP_BLETIMER_32K_SEL_V << LP_CLKRST_LP_BLETIMER_32K_SEL_S)
#define LP_CLKRST_LP_BLETIMER_32K_SEL_V  0x00000003U
#define LP_CLKRST_LP_BLETIMER_32K_SEL_S  24
/** LP_CLKRST_LP_SEL_OSC_SLOW : R/W; bitpos: [26]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_SEL_OSC_SLOW    (BIT(26))
#define LP_CLKRST_LP_SEL_OSC_SLOW_M  (LP_CLKRST_LP_SEL_OSC_SLOW_V << LP_CLKRST_LP_SEL_OSC_SLOW_S)
#define LP_CLKRST_LP_SEL_OSC_SLOW_V  0x00000001U
#define LP_CLKRST_LP_SEL_OSC_SLOW_S  26
/** LP_CLKRST_LP_SEL_OSC_FAST : R/W; bitpos: [27]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_SEL_OSC_FAST    (BIT(27))
#define LP_CLKRST_LP_SEL_OSC_FAST_M  (LP_CLKRST_LP_SEL_OSC_FAST_V << LP_CLKRST_LP_SEL_OSC_FAST_S)
#define LP_CLKRST_LP_SEL_OSC_FAST_V  0x00000001U
#define LP_CLKRST_LP_SEL_OSC_FAST_S  27
/** LP_CLKRST_LP_SEL_XTAL : R/W; bitpos: [28]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_SEL_XTAL    (BIT(28))
#define LP_CLKRST_LP_SEL_XTAL_M  (LP_CLKRST_LP_SEL_XTAL_V << LP_CLKRST_LP_SEL_XTAL_S)
#define LP_CLKRST_LP_SEL_XTAL_V  0x00000001U
#define LP_CLKRST_LP_SEL_XTAL_S  28
/** LP_CLKRST_LP_SEL_XTAL32K : R/W; bitpos: [29]; default: 0;
 *  need_des
 */
#define LP_CLKRST_LP_SEL_XTAL32K    (BIT(29))
#define LP_CLKRST_LP_SEL_XTAL32K_M  (LP_CLKRST_LP_SEL_XTAL32K_V << LP_CLKRST_LP_SEL_XTAL32K_S)
#define LP_CLKRST_LP_SEL_XTAL32K_V  0x00000001U
#define LP_CLKRST_LP_SEL_XTAL32K_S  29

/** LP_CLKRST_DATE_REG register
 *  Version control register
 */
#define LP_CLKRST_DATE_REG (DR_REG_LP_BASE + 0x3fc)
/** LP_CLKRST_CLKRST_DATE : R/W; bitpos: [30:0]; default: 37818640;
 *  Version control register
 */
#define LP_CLKRST_CLKRST_DATE    0x7FFFFFFFU
#define LP_CLKRST_CLKRST_DATE_M  (LP_CLKRST_CLKRST_DATE_V << LP_CLKRST_CLKRST_DATE_S)
#define LP_CLKRST_CLKRST_DATE_V  0x7FFFFFFFU
#define LP_CLKRST_CLKRST_DATE_S  0
/** LP_CLKRST_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  configure register clk bypass clk gate
 */
#define LP_CLKRST_CLK_EN    (BIT(31))
#define LP_CLKRST_CLK_EN_M  (LP_CLKRST_CLK_EN_V << LP_CLKRST_CLK_EN_S)
#define LP_CLKRST_CLK_EN_V  0x00000001U
#define LP_CLKRST_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
