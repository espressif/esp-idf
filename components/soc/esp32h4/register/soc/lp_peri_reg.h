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

/** LPPERI_CLK_EN_REG register
 *  configure peri in lp system clk enable
 */
#define LPPERI_CLK_EN_REG (DR_REG_LPPERI_BASE + 0x0)
/** LPPERI_RNG_APB_CK_EN : R/W; bitpos: [23]; default: 1;
 *  lp rng apb clk enable
 *  1: enable clock
 *  0: disable clock
 */
#define LPPERI_RNG_APB_CK_EN    (BIT(23))
#define LPPERI_RNG_APB_CK_EN_M  (LPPERI_RNG_APB_CK_EN_V << LPPERI_RNG_APB_CK_EN_S)
#define LPPERI_RNG_APB_CK_EN_V  0x00000001U
#define LPPERI_RNG_APB_CK_EN_S  23
/** LPPERI_RNG_CK_EN : R/W; bitpos: [24]; default: 1;
 *  lp rng clk enable
 *  1: enable clock
 *  0: disable clock
 */
#define LPPERI_RNG_CK_EN    (BIT(24))
#define LPPERI_RNG_CK_EN_M  (LPPERI_RNG_CK_EN_V << LPPERI_RNG_CK_EN_S)
#define LPPERI_RNG_CK_EN_V  0x00000001U
#define LPPERI_RNG_CK_EN_S  24
/** LPPERI_OTP_DBG_CK_EN : R/W; bitpos: [25]; default: 1;
 *  lp optdebug clk enable
 *  1: enable clock
 *  0: disable clock
 */
#define LPPERI_OTP_DBG_CK_EN    (BIT(25))
#define LPPERI_OTP_DBG_CK_EN_M  (LPPERI_OTP_DBG_CK_EN_V << LPPERI_OTP_DBG_CK_EN_S)
#define LPPERI_OTP_DBG_CK_EN_V  0x00000001U
#define LPPERI_OTP_DBG_CK_EN_S  25
/** LPPERI_LP_TOUCH_CK_EN : R/W; bitpos: [26]; default: 1;
 *  lp touch clk enable
 *  1: enable clock
 *  0: disable clock
 */
#define LPPERI_LP_TOUCH_CK_EN    (BIT(26))
#define LPPERI_LP_TOUCH_CK_EN_M  (LPPERI_LP_TOUCH_CK_EN_V << LPPERI_LP_TOUCH_CK_EN_S)
#define LPPERI_LP_TOUCH_CK_EN_V  0x00000001U
#define LPPERI_LP_TOUCH_CK_EN_S  26
/** LPPERI_LP_IO_CK_EN : R/W; bitpos: [27]; default: 1;
 *  lp io clk enable
 *  1: enable clock
 *  0: disable clock
 */
#define LPPERI_LP_IO_CK_EN    (BIT(27))
#define LPPERI_LP_IO_CK_EN_M  (LPPERI_LP_IO_CK_EN_V << LPPERI_LP_IO_CK_EN_S)
#define LPPERI_LP_IO_CK_EN_V  0x00000001U
#define LPPERI_LP_IO_CK_EN_S  27
/** LPPERI_EFUSE_CK_EN : R/W; bitpos: [30]; default: 1;
 *  efuse core clk enable
 *  1: enable clock
 *  0: disable clock
 */
#define LPPERI_EFUSE_CK_EN    (BIT(30))
#define LPPERI_EFUSE_CK_EN_M  (LPPERI_EFUSE_CK_EN_V << LPPERI_EFUSE_CK_EN_S)
#define LPPERI_EFUSE_CK_EN_V  0x00000001U
#define LPPERI_EFUSE_CK_EN_S  30

/** LPPERI_RESET_EN_REG register
 *  configure peri in lp system reset enable
 */
#define LPPERI_RESET_EN_REG (DR_REG_LPPERI_BASE + 0x4)
/** LPPERI_BUS_RESET_EN : WT; bitpos: [23]; default: 0;
 *  lp bus reset enable
 *  1: enable reset
 *  0: disable reset
 */
#define LPPERI_BUS_RESET_EN    (BIT(23))
#define LPPERI_BUS_RESET_EN_M  (LPPERI_BUS_RESET_EN_V << LPPERI_BUS_RESET_EN_S)
#define LPPERI_BUS_RESET_EN_V  0x00000001U
#define LPPERI_BUS_RESET_EN_S  23
/** LPPERI_LP_RNG_APB_RESET_EN : R/W; bitpos: [24]; default: 0;
 *  lp rng apb reset enable
 *  1: enable reset
 *  0: disable reset
 */
#define LPPERI_LP_RNG_APB_RESET_EN    (BIT(24))
#define LPPERI_LP_RNG_APB_RESET_EN_M  (LPPERI_LP_RNG_APB_RESET_EN_V << LPPERI_LP_RNG_APB_RESET_EN_S)
#define LPPERI_LP_RNG_APB_RESET_EN_V  0x00000001U
#define LPPERI_LP_RNG_APB_RESET_EN_S  24
/** LPPERI_OTP_DBG_RESET_EN : R/W; bitpos: [25]; default: 0;
 *  lp optdebug reset enable
 *  1: enable reset
 *  0: disable reset
 */
#define LPPERI_OTP_DBG_RESET_EN    (BIT(25))
#define LPPERI_OTP_DBG_RESET_EN_M  (LPPERI_OTP_DBG_RESET_EN_V << LPPERI_OTP_DBG_RESET_EN_S)
#define LPPERI_OTP_DBG_RESET_EN_V  0x00000001U
#define LPPERI_OTP_DBG_RESET_EN_S  25
/** LPPERI_LP_TOUCH_RESET_EN : R/W; bitpos: [26]; default: 0;
 *  lp touch reset enable
 *  1: enable reset
 *  0: disable reset
 */
#define LPPERI_LP_TOUCH_RESET_EN    (BIT(26))
#define LPPERI_LP_TOUCH_RESET_EN_M  (LPPERI_LP_TOUCH_RESET_EN_V << LPPERI_LP_TOUCH_RESET_EN_S)
#define LPPERI_LP_TOUCH_RESET_EN_V  0x00000001U
#define LPPERI_LP_TOUCH_RESET_EN_S  26
/** LPPERI_LP_IO_RESET_EN : R/W; bitpos: [27]; default: 0;
 *  lp io reset enable
 *  1: enable reset
 *  0: disable reset
 */
#define LPPERI_LP_IO_RESET_EN    (BIT(27))
#define LPPERI_LP_IO_RESET_EN_M  (LPPERI_LP_IO_RESET_EN_V << LPPERI_LP_IO_RESET_EN_S)
#define LPPERI_LP_IO_RESET_EN_V  0x00000001U
#define LPPERI_LP_IO_RESET_EN_S  27
/** LPPERI_EFUSE_RESET_EN : R/W; bitpos: [30]; default: 0;
 *  efuse core reset enable
 *  1: enable reset
 *  0: disable reset
 */
#define LPPERI_EFUSE_RESET_EN    (BIT(30))
#define LPPERI_EFUSE_RESET_EN_M  (LPPERI_EFUSE_RESET_EN_V << LPPERI_EFUSE_RESET_EN_S)
#define LPPERI_EFUSE_RESET_EN_V  0x00000001U
#define LPPERI_EFUSE_RESET_EN_S  30

/** LPPERI_LP_PERI_PMS_CONF_REG register
 *  LP Peripherals PMS configuration register
 */
#define LPPERI_LP_PERI_PMS_CONF_REG (DR_REG_LPPERI_BASE + 0x10)
/** LPPERI_LP_PERI_PMS_EXCEPTION_CLR : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to clear lp peri_pms_record_reg.
 *  0: No clear
 *  1: Clear peri_pms_record_reg
 */
#define LPPERI_LP_PERI_PMS_EXCEPTION_CLR    (BIT(0))
#define LPPERI_LP_PERI_PMS_EXCEPTION_CLR_M  (LPPERI_LP_PERI_PMS_EXCEPTION_CLR_V << LPPERI_LP_PERI_PMS_EXCEPTION_CLR_S)
#define LPPERI_LP_PERI_PMS_EXCEPTION_CLR_V  0x00000001U
#define LPPERI_LP_PERI_PMS_EXCEPTION_CLR_S  0

/** LPPERI_LP_PERI_PMS_EXCEPTION_INFO_REG register
 *  LP Peripherals PMS exception info record register
 */
#define LPPERI_LP_PERI_PMS_EXCEPTION_INFO_REG (DR_REG_LPPERI_BASE + 0x14)
/** LPPERI_LP_PERI_PMS_EXCEPTION_DET : RO; bitpos: [0]; default: 0;
 *  Represents whether the lp peripheral pms has been triggered.
 *  0: No triggered
 *  1: Has been triggered
 */
#define LPPERI_LP_PERI_PMS_EXCEPTION_DET    (BIT(0))
#define LPPERI_LP_PERI_PMS_EXCEPTION_DET_M  (LPPERI_LP_PERI_PMS_EXCEPTION_DET_V << LPPERI_LP_PERI_PMS_EXCEPTION_DET_S)
#define LPPERI_LP_PERI_PMS_EXCEPTION_DET_V  0x00000001U
#define LPPERI_LP_PERI_PMS_EXCEPTION_DET_S  0
/** LPPERI_LP_PERI_PMS_EXCEPTION_ID : RO; bitpos: [5:1]; default: 0;
 *  Represents the master id when lp peripheral pms has been triggered.
 */
#define LPPERI_LP_PERI_PMS_EXCEPTION_ID    0x0000001FU
#define LPPERI_LP_PERI_PMS_EXCEPTION_ID_M  (LPPERI_LP_PERI_PMS_EXCEPTION_ID_V << LPPERI_LP_PERI_PMS_EXCEPTION_ID_S)
#define LPPERI_LP_PERI_PMS_EXCEPTION_ID_V  0x0000001FU
#define LPPERI_LP_PERI_PMS_EXCEPTION_ID_S  1
/** LPPERI_LP_PERI_PMS_EXCEPTION_MODE : RO; bitpos: [7:6]; default: 0;
 *  Represents the security mode when lp peripheral pms has been triggered.
 */
#define LPPERI_LP_PERI_PMS_EXCEPTION_MODE    0x00000003U
#define LPPERI_LP_PERI_PMS_EXCEPTION_MODE_M  (LPPERI_LP_PERI_PMS_EXCEPTION_MODE_V << LPPERI_LP_PERI_PMS_EXCEPTION_MODE_S)
#define LPPERI_LP_PERI_PMS_EXCEPTION_MODE_V  0x00000003U
#define LPPERI_LP_PERI_PMS_EXCEPTION_MODE_S  6
/** LPPERI_LP_PERI_PMS_EXCEPTION_ADDR : RO; bitpos: [31:8]; default: 0;
 *  Represents the access address (bit23~bit0) when lp peripheral pms has been
 *  triggered.
 */
#define LPPERI_LP_PERI_PMS_EXCEPTION_ADDR    0x00FFFFFFU
#define LPPERI_LP_PERI_PMS_EXCEPTION_ADDR_M  (LPPERI_LP_PERI_PMS_EXCEPTION_ADDR_V << LPPERI_LP_PERI_PMS_EXCEPTION_ADDR_S)
#define LPPERI_LP_PERI_PMS_EXCEPTION_ADDR_V  0x00FFFFFFU
#define LPPERI_LP_PERI_PMS_EXCEPTION_ADDR_S  8

/** LPPERI_INTERRUPT_SOURCE_REG register
 *  record the lp cpu interrupt
 */
#define LPPERI_INTERRUPT_SOURCE_REG (DR_REG_LPPERI_BASE + 0x20)
/** LPPERI_LP_INTERRUPT_SOURCE : RO; bitpos: [4:0]; default: 0;
 *  BIT4~BIT0: pmu_lp_int, modem_lp_int, lp_timer_lp_int, lp_analog_peri_int, lp_io_int
 */
#define LPPERI_LP_INTERRUPT_SOURCE    0x0000001FU
#define LPPERI_LP_INTERRUPT_SOURCE_M  (LPPERI_LP_INTERRUPT_SOURCE_V << LPPERI_LP_INTERRUPT_SOURCE_S)
#define LPPERI_LP_INTERRUPT_SOURCE_V  0x0000001FU
#define LPPERI_LP_INTERRUPT_SOURCE_S  0

/** LPPERI_DATE_REG register
 *  version register
 */
#define LPPERI_DATE_REG (DR_REG_LPPERI_BASE + 0x3fc)
/** LPPERI_LPPERI_DATE : R/W; bitpos: [30:0]; default: 37819136;
 *  version register
 */
#define LPPERI_LPPERI_DATE    0x7FFFFFFFU
#define LPPERI_LPPERI_DATE_M  (LPPERI_LPPERI_DATE_V << LPPERI_LPPERI_DATE_S)
#define LPPERI_LPPERI_DATE_V  0x7FFFFFFFU
#define LPPERI_LPPERI_DATE_S  0
/** LPPERI_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  force on reg clk
 */
#define LPPERI_CLK_EN    (BIT(31))
#define LPPERI_CLK_EN_M  (LPPERI_CLK_EN_V << LPPERI_CLK_EN_S)
#define LPPERI_CLK_EN_V  0x00000001U
#define LPPERI_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
