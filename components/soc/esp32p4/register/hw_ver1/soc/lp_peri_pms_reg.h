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

/** PMS_LP_PERI_PMS_DATE_REG register
 *  Version control register
 */
#define PMS_LP_PERI_PMS_DATE_REG (DR_REG_LP_PERI_PMS_BASE + 0x0)
/** PMS_LP_PERI_PMS_DATE : R/W; bitpos: [31:0]; default: 2294537;
 *  Version control register
 */
#define PMS_LP_PERI_PMS_DATE    0xFFFFFFFFU
#define PMS_LP_PERI_PMS_DATE_M  (PMS_LP_PERI_PMS_DATE_V << PMS_LP_PERI_PMS_DATE_S)
#define PMS_LP_PERI_PMS_DATE_V  0xFFFFFFFFU
#define PMS_LP_PERI_PMS_DATE_S  0

/** PMS_LP_PERI_PMS_CLK_EN_REG register
 *  Clock gating register
 */
#define PMS_LP_PERI_PMS_CLK_EN_REG (DR_REG_LP_PERI_PMS_BASE + 0x4)
/** PMS_LP_PERI_PMS_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: Enable automatic clock gating
 *  1: Keep the clock always on
 */
#define PMS_LP_PERI_PMS_CLK_EN    (BIT(0))
#define PMS_LP_PERI_PMS_CLK_EN_M  (PMS_LP_PERI_PMS_CLK_EN_V << PMS_LP_PERI_PMS_CLK_EN_S)
#define PMS_LP_PERI_PMS_CLK_EN_V  0x00000001U
#define PMS_LP_PERI_PMS_CLK_EN_S  0

/** PMS_LP_MM_LP_PERI_PMS_REG0_REG register
 *  Permission control register0 for LP CPU in machine mode
 */
#define PMS_LP_MM_LP_PERI_PMS_REG0_REG (DR_REG_LP_PERI_PMS_BASE + 0x8)
/** PMS_LP_MM_LP_SYSREG_ALLOW : R/W; bitpos: [0]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP system
 *  registers.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_SYSREG_ALLOW    (BIT(0))
#define PMS_LP_MM_LP_SYSREG_ALLOW_M  (PMS_LP_MM_LP_SYSREG_ALLOW_V << PMS_LP_MM_LP_SYSREG_ALLOW_S)
#define PMS_LP_MM_LP_SYSREG_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_SYSREG_ALLOW_S  0
/** PMS_LP_MM_LP_AONCLKRST_ALLOW : R/W; bitpos: [1]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP_AONCLKRST (LP
 *  always-on clock and reset).
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_AONCLKRST_ALLOW    (BIT(1))
#define PMS_LP_MM_LP_AONCLKRST_ALLOW_M  (PMS_LP_MM_LP_AONCLKRST_ALLOW_V << PMS_LP_MM_LP_AONCLKRST_ALLOW_S)
#define PMS_LP_MM_LP_AONCLKRST_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_AONCLKRST_ALLOW_S  1
/** PMS_LP_MM_LP_TIMER_ALLOW : R/W; bitpos: [2]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP timer.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_TIMER_ALLOW    (BIT(2))
#define PMS_LP_MM_LP_TIMER_ALLOW_M  (PMS_LP_MM_LP_TIMER_ALLOW_V << PMS_LP_MM_LP_TIMER_ALLOW_S)
#define PMS_LP_MM_LP_TIMER_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_TIMER_ALLOW_S  2
/** PMS_LP_MM_LP_ANAPERI_ALLOW : R/W; bitpos: [3]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP ANAPERI
 *  (analog peripherals).
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_ANAPERI_ALLOW    (BIT(3))
#define PMS_LP_MM_LP_ANAPERI_ALLOW_M  (PMS_LP_MM_LP_ANAPERI_ALLOW_V << PMS_LP_MM_LP_ANAPERI_ALLOW_S)
#define PMS_LP_MM_LP_ANAPERI_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_ANAPERI_ALLOW_S  3
/** PMS_LP_MM_LP_PMU_ALLOW : R/W; bitpos: [4]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP PMU (Power
 *  Management Unit).
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_PMU_ALLOW    (BIT(4))
#define PMS_LP_MM_LP_PMU_ALLOW_M  (PMS_LP_MM_LP_PMU_ALLOW_V << PMS_LP_MM_LP_PMU_ALLOW_S)
#define PMS_LP_MM_LP_PMU_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_PMU_ALLOW_S  4
/** PMS_LP_MM_LP_WDT_ALLOW : R/W; bitpos: [5]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP WDT (watchdog
 *  timer).
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_WDT_ALLOW    (BIT(5))
#define PMS_LP_MM_LP_WDT_ALLOW_M  (PMS_LP_MM_LP_WDT_ALLOW_V << PMS_LP_MM_LP_WDT_ALLOW_S)
#define PMS_LP_MM_LP_WDT_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_WDT_ALLOW_S  5
/** PMS_LP_MM_LP_MAILBOX_ALLOW : R/W; bitpos: [6]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP Mailbox
 *  Controller.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_MAILBOX_ALLOW    (BIT(6))
#define PMS_LP_MM_LP_MAILBOX_ALLOW_M  (PMS_LP_MM_LP_MAILBOX_ALLOW_V << PMS_LP_MM_LP_MAILBOX_ALLOW_S)
#define PMS_LP_MM_LP_MAILBOX_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_MAILBOX_ALLOW_S  6
/** PMS_LP_MM_LP_RTC_ALLOW : R/W; bitpos: [7]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP RTC.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_LP_MM_LP_RTC_ALLOW    (BIT(7))
#define PMS_LP_MM_LP_RTC_ALLOW_M  (PMS_LP_MM_LP_RTC_ALLOW_V << PMS_LP_MM_LP_RTC_ALLOW_S)
#define PMS_LP_MM_LP_RTC_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_RTC_ALLOW_S  7
/** PMS_LP_MM_LP_PERICLKRST_ALLOW : R/W; bitpos: [8]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access  LP PREICLKRST
 *  (peripheral clock and reset).
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_PERICLKRST_ALLOW    (BIT(8))
#define PMS_LP_MM_LP_PERICLKRST_ALLOW_M  (PMS_LP_MM_LP_PERICLKRST_ALLOW_V << PMS_LP_MM_LP_PERICLKRST_ALLOW_S)
#define PMS_LP_MM_LP_PERICLKRST_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_PERICLKRST_ALLOW_S  8
/** PMS_LP_MM_LP_UART_ALLOW : R/W; bitpos: [9]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access  LP UART.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_UART_ALLOW    (BIT(9))
#define PMS_LP_MM_LP_UART_ALLOW_M  (PMS_LP_MM_LP_UART_ALLOW_V << PMS_LP_MM_LP_UART_ALLOW_S)
#define PMS_LP_MM_LP_UART_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_UART_ALLOW_S  9
/** PMS_LP_MM_LP_I2C_ALLOW : R/W; bitpos: [10]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP I2S.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_I2C_ALLOW    (BIT(10))
#define PMS_LP_MM_LP_I2C_ALLOW_M  (PMS_LP_MM_LP_I2C_ALLOW_V << PMS_LP_MM_LP_I2C_ALLOW_S)
#define PMS_LP_MM_LP_I2C_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_I2C_ALLOW_S  10
/** PMS_LP_MM_LP_SPI_ALLOW : R/W; bitpos: [11]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP SPI.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_SPI_ALLOW    (BIT(11))
#define PMS_LP_MM_LP_SPI_ALLOW_M  (PMS_LP_MM_LP_SPI_ALLOW_V << PMS_LP_MM_LP_SPI_ALLOW_S)
#define PMS_LP_MM_LP_SPI_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_SPI_ALLOW_S  11
/** PMS_LP_MM_LP_I2CMST_ALLOW : R/W; bitpos: [12]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP I2C master.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_I2CMST_ALLOW    (BIT(12))
#define PMS_LP_MM_LP_I2CMST_ALLOW_M  (PMS_LP_MM_LP_I2CMST_ALLOW_V << PMS_LP_MM_LP_I2CMST_ALLOW_S)
#define PMS_LP_MM_LP_I2CMST_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_I2CMST_ALLOW_S  12
/** PMS_LP_MM_LP_I2S_ALLOW : R/W; bitpos: [13]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP I2S.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_I2S_ALLOW    (BIT(13))
#define PMS_LP_MM_LP_I2S_ALLOW_M  (PMS_LP_MM_LP_I2S_ALLOW_V << PMS_LP_MM_LP_I2S_ALLOW_S)
#define PMS_LP_MM_LP_I2S_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_I2S_ALLOW_S  13
/** PMS_LP_MM_LP_ADC_ALLOW : R/W; bitpos: [14]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP ADC.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_ADC_ALLOW    (BIT(14))
#define PMS_LP_MM_LP_ADC_ALLOW_M  (PMS_LP_MM_LP_ADC_ALLOW_V << PMS_LP_MM_LP_ADC_ALLOW_S)
#define PMS_LP_MM_LP_ADC_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_ADC_ALLOW_S  14
/** PMS_LP_MM_LP_TOUCH_ALLOW : R/W; bitpos: [15]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP touch sensor.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_TOUCH_ALLOW    (BIT(15))
#define PMS_LP_MM_LP_TOUCH_ALLOW_M  (PMS_LP_MM_LP_TOUCH_ALLOW_V << PMS_LP_MM_LP_TOUCH_ALLOW_S)
#define PMS_LP_MM_LP_TOUCH_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_TOUCH_ALLOW_S  15
/** PMS_LP_MM_LP_IOMUX_ALLOW : R/W; bitpos: [16]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP IO MUX.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_IOMUX_ALLOW    (BIT(16))
#define PMS_LP_MM_LP_IOMUX_ALLOW_M  (PMS_LP_MM_LP_IOMUX_ALLOW_V << PMS_LP_MM_LP_IOMUX_ALLOW_S)
#define PMS_LP_MM_LP_IOMUX_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_IOMUX_ALLOW_S  16
/** PMS_LP_MM_LP_INTR_ALLOW : R/W; bitpos: [17]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP INTR
 *  (interrupt).
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_INTR_ALLOW    (BIT(17))
#define PMS_LP_MM_LP_INTR_ALLOW_M  (PMS_LP_MM_LP_INTR_ALLOW_V << PMS_LP_MM_LP_INTR_ALLOW_S)
#define PMS_LP_MM_LP_INTR_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_INTR_ALLOW_S  17
/** PMS_LP_MM_LP_EFUSE_ALLOW : R/W; bitpos: [18]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP eFuse.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_EFUSE_ALLOW    (BIT(18))
#define PMS_LP_MM_LP_EFUSE_ALLOW_M  (PMS_LP_MM_LP_EFUSE_ALLOW_V << PMS_LP_MM_LP_EFUSE_ALLOW_S)
#define PMS_LP_MM_LP_EFUSE_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_EFUSE_ALLOW_S  18
/** PMS_LP_MM_LP_PMS_ALLOW : R/W; bitpos: [19]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP_PERI_PMS_REG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_PMS_ALLOW    (BIT(19))
#define PMS_LP_MM_LP_PMS_ALLOW_M  (PMS_LP_MM_LP_PMS_ALLOW_V << PMS_LP_MM_LP_PMS_ALLOW_S)
#define PMS_LP_MM_LP_PMS_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_PMS_ALLOW_S  19
/** PMS_LP_MM_HP2LP_PMS_ALLOW : R/W; bitpos: [20]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access
 *  HP2LP_PERI_PMS_REG.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_HP2LP_PMS_ALLOW    (BIT(20))
#define PMS_LP_MM_HP2LP_PMS_ALLOW_M  (PMS_LP_MM_HP2LP_PMS_ALLOW_V << PMS_LP_MM_HP2LP_PMS_ALLOW_S)
#define PMS_LP_MM_HP2LP_PMS_ALLOW_V  0x00000001U
#define PMS_LP_MM_HP2LP_PMS_ALLOW_S  20
/** PMS_LP_MM_LP_TSENS_ALLOW : R/W; bitpos: [21]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP temperature
 *  sensor.
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_TSENS_ALLOW    (BIT(21))
#define PMS_LP_MM_LP_TSENS_ALLOW_M  (PMS_LP_MM_LP_TSENS_ALLOW_V << PMS_LP_MM_LP_TSENS_ALLOW_S)
#define PMS_LP_MM_LP_TSENS_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_TSENS_ALLOW_S  21
/** PMS_LP_MM_LP_HUK_ALLOW : R/W; bitpos: [22]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP HUK (Hardware
 *  Unique Key).
 *  0: Not allowed
 *  1: Allowed
 */
#define PMS_LP_MM_LP_HUK_ALLOW    (BIT(22))
#define PMS_LP_MM_LP_HUK_ALLOW_M  (PMS_LP_MM_LP_HUK_ALLOW_V << PMS_LP_MM_LP_HUK_ALLOW_S)
#define PMS_LP_MM_LP_HUK_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_HUK_ALLOW_S  22
/** PMS_LP_MM_LP_SRAM_ALLOW : R/W; bitpos: [23]; default: 1;
 *  Configures whether LP CPU in machine mode has permission to access LP SRAM.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_LP_MM_LP_SRAM_ALLOW    (BIT(23))
#define PMS_LP_MM_LP_SRAM_ALLOW_M  (PMS_LP_MM_LP_SRAM_ALLOW_V << PMS_LP_MM_LP_SRAM_ALLOW_S)
#define PMS_LP_MM_LP_SRAM_ALLOW_V  0x00000001U
#define PMS_LP_MM_LP_SRAM_ALLOW_S  23

/** PMS_PERI_REGION0_LOW_REG register
 *  Region0 start address configuration register
 */
#define PMS_PERI_REGION0_LOW_REG (DR_REG_LP_PERI_PMS_BASE + 0xc)
/** PMS_PERI_REGION0_LOW : R/W; bitpos: [31:2]; default: 0;
 *  Configures the high 30 bits of the start address of peripheral register's region0.
 */
#define PMS_PERI_REGION0_LOW    0x3FFFFFFFU
#define PMS_PERI_REGION0_LOW_M  (PMS_PERI_REGION0_LOW_V << PMS_PERI_REGION0_LOW_S)
#define PMS_PERI_REGION0_LOW_V  0x3FFFFFFFU
#define PMS_PERI_REGION0_LOW_S  2

/** PMS_PERI_REGION0_HIGH_REG register
 *  Region0 end address configuration register
 */
#define PMS_PERI_REGION0_HIGH_REG (DR_REG_LP_PERI_PMS_BASE + 0x10)
/** PMS_PERI_REGION0_HIGH : R/W; bitpos: [31:2]; default: 1073741823;
 *  Configures the high 30 bits of the end address of peripheral register's region0.
 */
#define PMS_PERI_REGION0_HIGH    0x3FFFFFFFU
#define PMS_PERI_REGION0_HIGH_M  (PMS_PERI_REGION0_HIGH_V << PMS_PERI_REGION0_HIGH_S)
#define PMS_PERI_REGION0_HIGH_V  0x3FFFFFFFU
#define PMS_PERI_REGION0_HIGH_S  2

/** PMS_PERI_REGION1_LOW_REG register
 *  Region1 start address configuration register
 */
#define PMS_PERI_REGION1_LOW_REG (DR_REG_LP_PERI_PMS_BASE + 0x14)
/** PMS_PERI_REGION1_LOW : R/W; bitpos: [31:2]; default: 0;
 *  Configures the high 30 bits of the start address of peripheral register's region1.
 */
#define PMS_PERI_REGION1_LOW    0x3FFFFFFFU
#define PMS_PERI_REGION1_LOW_M  (PMS_PERI_REGION1_LOW_V << PMS_PERI_REGION1_LOW_S)
#define PMS_PERI_REGION1_LOW_V  0x3FFFFFFFU
#define PMS_PERI_REGION1_LOW_S  2

/** PMS_PERI_REGION1_HIGH_REG register
 *  Region1 end address configuration register
 */
#define PMS_PERI_REGION1_HIGH_REG (DR_REG_LP_PERI_PMS_BASE + 0x18)
/** PMS_PERI_REGION1_HIGH : R/W; bitpos: [31:2]; default: 1073741823;
 *  Configures the high 30 bits of the end address of peripheral register's region1.
 */
#define PMS_PERI_REGION1_HIGH    0x3FFFFFFFU
#define PMS_PERI_REGION1_HIGH_M  (PMS_PERI_REGION1_HIGH_V << PMS_PERI_REGION1_HIGH_S)
#define PMS_PERI_REGION1_HIGH_V  0x3FFFFFFFU
#define PMS_PERI_REGION1_HIGH_S  2

/** PMS_PERI_REGION_PMS_REG register
 *  Permission register of  region
 */
#define PMS_PERI_REGION_PMS_REG (DR_REG_LP_PERI_PMS_BASE + 0x1c)
/** PMS_LP_CORE_REGION_PMS : R/W; bitpos: [1:0]; default: 3;
 *  Configures whether LP core in machine mode has permission to access address region0
 *  and address region1. Bit0 corresponds to region0 and bit1 corresponds to region1.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_LP_CORE_REGION_PMS    0x00000003U
#define PMS_LP_CORE_REGION_PMS_M  (PMS_LP_CORE_REGION_PMS_V << PMS_LP_CORE_REGION_PMS_S)
#define PMS_LP_CORE_REGION_PMS_V  0x00000003U
#define PMS_LP_CORE_REGION_PMS_S  0
/** PMS_HP_CORE0_UM_REGION_PMS : R/W; bitpos: [3:2]; default: 3;
 *  Configures whether HP CPU0 in user mode has permission to access address region0
 *  and address region1. Bit2 corresponds to region0 and bit3 corresponds to region1.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_HP_CORE0_UM_REGION_PMS    0x00000003U
#define PMS_HP_CORE0_UM_REGION_PMS_M  (PMS_HP_CORE0_UM_REGION_PMS_V << PMS_HP_CORE0_UM_REGION_PMS_S)
#define PMS_HP_CORE0_UM_REGION_PMS_V  0x00000003U
#define PMS_HP_CORE0_UM_REGION_PMS_S  2
/** PMS_HP_CORE0_MM_REGION_PMS : R/W; bitpos: [5:4]; default: 3;
 *  Configures whether HP CPU0 in machine mode has permission to access address region0
 *  and address region1. Bit4 corresponds to region0 and bit5 corresponds to region1.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_HP_CORE0_MM_REGION_PMS    0x00000003U
#define PMS_HP_CORE0_MM_REGION_PMS_M  (PMS_HP_CORE0_MM_REGION_PMS_V << PMS_HP_CORE0_MM_REGION_PMS_S)
#define PMS_HP_CORE0_MM_REGION_PMS_V  0x00000003U
#define PMS_HP_CORE0_MM_REGION_PMS_S  4
/** PMS_HP_CORE1_UM_REGION_PMS : R/W; bitpos: [7:6]; default: 3;
 *  Configures whether HP CPU1 in user mode has permission to access address region0
 *  and address region1. Bit6 corresponds to region0 and bit7 corresponds to region1.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_HP_CORE1_UM_REGION_PMS    0x00000003U
#define PMS_HP_CORE1_UM_REGION_PMS_M  (PMS_HP_CORE1_UM_REGION_PMS_V << PMS_HP_CORE1_UM_REGION_PMS_S)
#define PMS_HP_CORE1_UM_REGION_PMS_V  0x00000003U
#define PMS_HP_CORE1_UM_REGION_PMS_S  6
/** PMS_HP_CORE1_MM_REGION_PMS : R/W; bitpos: [9:8]; default: 3;
 *  Configures whether HP CPU1 in machine mode has permission to access address region0
 *  and address region1. Bit8 corresponds to region0 and bit9 corresponds to region1.
 *  0: Not allowed
 *  1: Allow
 */
#define PMS_HP_CORE1_MM_REGION_PMS    0x00000003U
#define PMS_HP_CORE1_MM_REGION_PMS_M  (PMS_HP_CORE1_MM_REGION_PMS_V << PMS_HP_CORE1_MM_REGION_PMS_S)
#define PMS_HP_CORE1_MM_REGION_PMS_V  0x00000003U
#define PMS_HP_CORE1_MM_REGION_PMS_S  8

#ifdef __cplusplus
}
#endif
