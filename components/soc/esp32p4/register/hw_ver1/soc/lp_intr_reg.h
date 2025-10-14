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

/** LP_INTR_SW_INT_RAW_REG register
 *  need_des
 */
#define LP_INTR_SW_INT_RAW_REG (DR_REG_LP_INTR_BASE + 0x0)
/** LP_INTR_LP_SW_INT_RAW : R/W/WTC; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_SW_INT_RAW    (BIT(31))
#define LP_INTR_LP_SW_INT_RAW_M  (LP_INTR_LP_SW_INT_RAW_V << LP_INTR_LP_SW_INT_RAW_S)
#define LP_INTR_LP_SW_INT_RAW_V  0x00000001U
#define LP_INTR_LP_SW_INT_RAW_S  31

/** LP_INTR_SW_INT_ST_REG register
 *  need_des
 */
#define LP_INTR_SW_INT_ST_REG (DR_REG_LP_INTR_BASE + 0x4)
/** LP_INTR_LP_SW_INT_ST : RO; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_SW_INT_ST    (BIT(31))
#define LP_INTR_LP_SW_INT_ST_M  (LP_INTR_LP_SW_INT_ST_V << LP_INTR_LP_SW_INT_ST_S)
#define LP_INTR_LP_SW_INT_ST_V  0x00000001U
#define LP_INTR_LP_SW_INT_ST_S  31

/** LP_INTR_SW_INT_ENA_REG register
 *  need_des
 */
#define LP_INTR_SW_INT_ENA_REG (DR_REG_LP_INTR_BASE + 0x8)
/** LP_INTR_LP_SW_INT_ENA : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_SW_INT_ENA    (BIT(31))
#define LP_INTR_LP_SW_INT_ENA_M  (LP_INTR_LP_SW_INT_ENA_V << LP_INTR_LP_SW_INT_ENA_S)
#define LP_INTR_LP_SW_INT_ENA_V  0x00000001U
#define LP_INTR_LP_SW_INT_ENA_S  31

/** LP_INTR_SW_INT_CLR_REG register
 *  need_des
 */
#define LP_INTR_SW_INT_CLR_REG (DR_REG_LP_INTR_BASE + 0xc)
/** LP_INTR_LP_SW_INT_CLR : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_SW_INT_CLR    (BIT(31))
#define LP_INTR_LP_SW_INT_CLR_M  (LP_INTR_LP_SW_INT_CLR_V << LP_INTR_LP_SW_INT_CLR_S)
#define LP_INTR_LP_SW_INT_CLR_V  0x00000001U
#define LP_INTR_LP_SW_INT_CLR_S  31

/** LP_INTR_STATUS_REG register
 *  need_des
 */
#define LP_INTR_STATUS_REG (DR_REG_LP_INTR_BASE + 0x10)
/** LP_INTR_LP_HUK_INTR_ST : RO; bitpos: [10]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_HUK_INTR_ST    (BIT(10))
#define LP_INTR_LP_HUK_INTR_ST_M  (LP_INTR_LP_HUK_INTR_ST_V << LP_INTR_LP_HUK_INTR_ST_S)
#define LP_INTR_LP_HUK_INTR_ST_V  0x00000001U
#define LP_INTR_LP_HUK_INTR_ST_S  10
/** LP_INTR_SYSREG_INTR_ST : RO; bitpos: [11]; default: 0;
 *  need_des
 */
#define LP_INTR_SYSREG_INTR_ST    (BIT(11))
#define LP_INTR_SYSREG_INTR_ST_M  (LP_INTR_SYSREG_INTR_ST_V << LP_INTR_SYSREG_INTR_ST_S)
#define LP_INTR_SYSREG_INTR_ST_V  0x00000001U
#define LP_INTR_SYSREG_INTR_ST_S  11
/** LP_INTR_LP_SW_INTR_ST : RO; bitpos: [12]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_SW_INTR_ST    (BIT(12))
#define LP_INTR_LP_SW_INTR_ST_M  (LP_INTR_LP_SW_INTR_ST_V << LP_INTR_LP_SW_INTR_ST_S)
#define LP_INTR_LP_SW_INTR_ST_V  0x00000001U
#define LP_INTR_LP_SW_INTR_ST_S  12
/** LP_INTR_LP_EFUSE_INTR_ST : RO; bitpos: [13]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_EFUSE_INTR_ST    (BIT(13))
#define LP_INTR_LP_EFUSE_INTR_ST_M  (LP_INTR_LP_EFUSE_INTR_ST_V << LP_INTR_LP_EFUSE_INTR_ST_S)
#define LP_INTR_LP_EFUSE_INTR_ST_V  0x00000001U
#define LP_INTR_LP_EFUSE_INTR_ST_S  13
/** LP_INTR_LP_UART_INTR_ST : RO; bitpos: [14]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_UART_INTR_ST    (BIT(14))
#define LP_INTR_LP_UART_INTR_ST_M  (LP_INTR_LP_UART_INTR_ST_V << LP_INTR_LP_UART_INTR_ST_S)
#define LP_INTR_LP_UART_INTR_ST_V  0x00000001U
#define LP_INTR_LP_UART_INTR_ST_S  14
/** LP_INTR_LP_TSENS_INTR_ST : RO; bitpos: [15]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_TSENS_INTR_ST    (BIT(15))
#define LP_INTR_LP_TSENS_INTR_ST_M  (LP_INTR_LP_TSENS_INTR_ST_V << LP_INTR_LP_TSENS_INTR_ST_S)
#define LP_INTR_LP_TSENS_INTR_ST_V  0x00000001U
#define LP_INTR_LP_TSENS_INTR_ST_S  15
/** LP_INTR_LP_TOUCH_INTR_ST : RO; bitpos: [16]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_TOUCH_INTR_ST    (BIT(16))
#define LP_INTR_LP_TOUCH_INTR_ST_M  (LP_INTR_LP_TOUCH_INTR_ST_V << LP_INTR_LP_TOUCH_INTR_ST_S)
#define LP_INTR_LP_TOUCH_INTR_ST_V  0x00000001U
#define LP_INTR_LP_TOUCH_INTR_ST_S  16
/** LP_INTR_LP_SPI_INTR_ST : RO; bitpos: [17]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_SPI_INTR_ST    (BIT(17))
#define LP_INTR_LP_SPI_INTR_ST_M  (LP_INTR_LP_SPI_INTR_ST_V << LP_INTR_LP_SPI_INTR_ST_S)
#define LP_INTR_LP_SPI_INTR_ST_V  0x00000001U
#define LP_INTR_LP_SPI_INTR_ST_S  17
/** LP_INTR_LP_I2S_INTR_ST : RO; bitpos: [18]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_I2S_INTR_ST    (BIT(18))
#define LP_INTR_LP_I2S_INTR_ST_M  (LP_INTR_LP_I2S_INTR_ST_V << LP_INTR_LP_I2S_INTR_ST_S)
#define LP_INTR_LP_I2S_INTR_ST_V  0x00000001U
#define LP_INTR_LP_I2S_INTR_ST_S  18
/** LP_INTR_LP_I2C_INTR_ST : RO; bitpos: [19]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_I2C_INTR_ST    (BIT(19))
#define LP_INTR_LP_I2C_INTR_ST_M  (LP_INTR_LP_I2C_INTR_ST_V << LP_INTR_LP_I2C_INTR_ST_S)
#define LP_INTR_LP_I2C_INTR_ST_V  0x00000001U
#define LP_INTR_LP_I2C_INTR_ST_S  19
/** LP_INTR_LP_GPIO_INTR_ST : RO; bitpos: [20]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_GPIO_INTR_ST    (BIT(20))
#define LP_INTR_LP_GPIO_INTR_ST_M  (LP_INTR_LP_GPIO_INTR_ST_V << LP_INTR_LP_GPIO_INTR_ST_S)
#define LP_INTR_LP_GPIO_INTR_ST_V  0x00000001U
#define LP_INTR_LP_GPIO_INTR_ST_S  20
/** LP_INTR_LP_ADC_INTR_ST : RO; bitpos: [21]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_ADC_INTR_ST    (BIT(21))
#define LP_INTR_LP_ADC_INTR_ST_M  (LP_INTR_LP_ADC_INTR_ST_V << LP_INTR_LP_ADC_INTR_ST_S)
#define LP_INTR_LP_ADC_INTR_ST_V  0x00000001U
#define LP_INTR_LP_ADC_INTR_ST_S  21
/** LP_INTR_ANAPERI_INTR_ST : RO; bitpos: [22]; default: 0;
 *  need_des
 */
#define LP_INTR_ANAPERI_INTR_ST    (BIT(22))
#define LP_INTR_ANAPERI_INTR_ST_M  (LP_INTR_ANAPERI_INTR_ST_V << LP_INTR_ANAPERI_INTR_ST_S)
#define LP_INTR_ANAPERI_INTR_ST_V  0x00000001U
#define LP_INTR_ANAPERI_INTR_ST_S  22
/** LP_INTR_PMU_REG_1_INTR_ST : RO; bitpos: [23]; default: 0;
 *  need_des
 */
#define LP_INTR_PMU_REG_1_INTR_ST    (BIT(23))
#define LP_INTR_PMU_REG_1_INTR_ST_M  (LP_INTR_PMU_REG_1_INTR_ST_V << LP_INTR_PMU_REG_1_INTR_ST_S)
#define LP_INTR_PMU_REG_1_INTR_ST_V  0x00000001U
#define LP_INTR_PMU_REG_1_INTR_ST_S  23
/** LP_INTR_PMU_REG_0_INTR_ST : RO; bitpos: [24]; default: 0;
 *  need_des
 */
#define LP_INTR_PMU_REG_0_INTR_ST    (BIT(24))
#define LP_INTR_PMU_REG_0_INTR_ST_M  (LP_INTR_PMU_REG_0_INTR_ST_V << LP_INTR_PMU_REG_0_INTR_ST_S)
#define LP_INTR_PMU_REG_0_INTR_ST_V  0x00000001U
#define LP_INTR_PMU_REG_0_INTR_ST_S  24
/** LP_INTR_MB_LP_INTR_ST : RO; bitpos: [25]; default: 0;
 *  need_des
 */
#define LP_INTR_MB_LP_INTR_ST    (BIT(25))
#define LP_INTR_MB_LP_INTR_ST_M  (LP_INTR_MB_LP_INTR_ST_V << LP_INTR_MB_LP_INTR_ST_S)
#define LP_INTR_MB_LP_INTR_ST_V  0x00000001U
#define LP_INTR_MB_LP_INTR_ST_S  25
/** LP_INTR_MB_HP_INTR_ST : RO; bitpos: [26]; default: 0;
 *  need_des
 */
#define LP_INTR_MB_HP_INTR_ST    (BIT(26))
#define LP_INTR_MB_HP_INTR_ST_M  (LP_INTR_MB_HP_INTR_ST_V << LP_INTR_MB_HP_INTR_ST_S)
#define LP_INTR_MB_HP_INTR_ST_V  0x00000001U
#define LP_INTR_MB_HP_INTR_ST_S  26
/** LP_INTR_LP_TIMER_REG_1_INTR_ST : RO; bitpos: [27]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_TIMER_REG_1_INTR_ST    (BIT(27))
#define LP_INTR_LP_TIMER_REG_1_INTR_ST_M  (LP_INTR_LP_TIMER_REG_1_INTR_ST_V << LP_INTR_LP_TIMER_REG_1_INTR_ST_S)
#define LP_INTR_LP_TIMER_REG_1_INTR_ST_V  0x00000001U
#define LP_INTR_LP_TIMER_REG_1_INTR_ST_S  27
/** LP_INTR_LP_TIMER_REG_0_INTR_ST : RO; bitpos: [28]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_TIMER_REG_0_INTR_ST    (BIT(28))
#define LP_INTR_LP_TIMER_REG_0_INTR_ST_M  (LP_INTR_LP_TIMER_REG_0_INTR_ST_V << LP_INTR_LP_TIMER_REG_0_INTR_ST_S)
#define LP_INTR_LP_TIMER_REG_0_INTR_ST_V  0x00000001U
#define LP_INTR_LP_TIMER_REG_0_INTR_ST_S  28
/** LP_INTR_LP_WDT_INTR_ST : RO; bitpos: [29]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_WDT_INTR_ST    (BIT(29))
#define LP_INTR_LP_WDT_INTR_ST_M  (LP_INTR_LP_WDT_INTR_ST_V << LP_INTR_LP_WDT_INTR_ST_S)
#define LP_INTR_LP_WDT_INTR_ST_V  0x00000001U
#define LP_INTR_LP_WDT_INTR_ST_S  29
/** LP_INTR_LP_RTC_INTR_ST : RO; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_INTR_LP_RTC_INTR_ST    (BIT(30))
#define LP_INTR_LP_RTC_INTR_ST_M  (LP_INTR_LP_RTC_INTR_ST_V << LP_INTR_LP_RTC_INTR_ST_S)
#define LP_INTR_LP_RTC_INTR_ST_V  0x00000001U
#define LP_INTR_LP_RTC_INTR_ST_S  30
/** LP_INTR_HP_INTR_ST : RO; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_INTR_HP_INTR_ST    (BIT(31))
#define LP_INTR_HP_INTR_ST_M  (LP_INTR_HP_INTR_ST_V << LP_INTR_HP_INTR_ST_S)
#define LP_INTR_HP_INTR_ST_V  0x00000001U
#define LP_INTR_HP_INTR_ST_S  31

/** LP_INTR_DATE_REG register
 *  need_des
 */
#define LP_INTR_DATE_REG (DR_REG_LP_INTR_BASE + 0x3fc)
/** LP_INTR_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_INTR_CLK_EN    (BIT(31))
#define LP_INTR_CLK_EN_M  (LP_INTR_CLK_EN_V << LP_INTR_CLK_EN_S)
#define LP_INTR_CLK_EN_V  0x00000001U
#define LP_INTR_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
