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


/** LP_WDT_CONFIG0_REG register
 *  need_des
 */
#define LP_WDT_CONFIG0_REG (DR_REG_LP_WDT_BASE + 0x0)
/** LP_WDT_WDT_PAUSE_IN_SLP : R/W; bitpos: [9]; default: 1;
 *  need_des
 */
#define LP_WDT_WDT_PAUSE_IN_SLP    (BIT(9))
#define LP_WDT_WDT_PAUSE_IN_SLP_M  (LP_WDT_WDT_PAUSE_IN_SLP_V << LP_WDT_WDT_PAUSE_IN_SLP_S)
#define LP_WDT_WDT_PAUSE_IN_SLP_V  0x00000001U
#define LP_WDT_WDT_PAUSE_IN_SLP_S  9
/** LP_WDT_WDT_APPCPU_RESET_EN : R/W; bitpos: [10]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_APPCPU_RESET_EN    (BIT(10))
#define LP_WDT_WDT_APPCPU_RESET_EN_M  (LP_WDT_WDT_APPCPU_RESET_EN_V << LP_WDT_WDT_APPCPU_RESET_EN_S)
#define LP_WDT_WDT_APPCPU_RESET_EN_V  0x00000001U
#define LP_WDT_WDT_APPCPU_RESET_EN_S  10
/** LP_WDT_WDT_PROCPU_RESET_EN : R/W; bitpos: [11]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_PROCPU_RESET_EN    (BIT(11))
#define LP_WDT_WDT_PROCPU_RESET_EN_M  (LP_WDT_WDT_PROCPU_RESET_EN_V << LP_WDT_WDT_PROCPU_RESET_EN_S)
#define LP_WDT_WDT_PROCPU_RESET_EN_V  0x00000001U
#define LP_WDT_WDT_PROCPU_RESET_EN_S  11
/** LP_WDT_WDT_FLASHBOOT_MOD_EN : R/W; bitpos: [12]; default: 1;
 *  need_des
 */
#define LP_WDT_WDT_FLASHBOOT_MOD_EN    (BIT(12))
#define LP_WDT_WDT_FLASHBOOT_MOD_EN_M  (LP_WDT_WDT_FLASHBOOT_MOD_EN_V << LP_WDT_WDT_FLASHBOOT_MOD_EN_S)
#define LP_WDT_WDT_FLASHBOOT_MOD_EN_V  0x00000001U
#define LP_WDT_WDT_FLASHBOOT_MOD_EN_S  12
/** LP_WDT_WDT_SYS_RESET_LENGTH : R/W; bitpos: [15:13]; default: 1;
 *  need_des
 */
#define LP_WDT_WDT_SYS_RESET_LENGTH    0x00000007U
#define LP_WDT_WDT_SYS_RESET_LENGTH_M  (LP_WDT_WDT_SYS_RESET_LENGTH_V << LP_WDT_WDT_SYS_RESET_LENGTH_S)
#define LP_WDT_WDT_SYS_RESET_LENGTH_V  0x00000007U
#define LP_WDT_WDT_SYS_RESET_LENGTH_S  13
/** LP_WDT_WDT_CPU_RESET_LENGTH : R/W; bitpos: [18:16]; default: 1;
 *  need_des
 */
#define LP_WDT_WDT_CPU_RESET_LENGTH    0x00000007U
#define LP_WDT_WDT_CPU_RESET_LENGTH_M  (LP_WDT_WDT_CPU_RESET_LENGTH_V << LP_WDT_WDT_CPU_RESET_LENGTH_S)
#define LP_WDT_WDT_CPU_RESET_LENGTH_V  0x00000007U
#define LP_WDT_WDT_CPU_RESET_LENGTH_S  16
/** LP_WDT_WDT_STG3 : R/W; bitpos: [21:19]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_STG3    0x00000007U
#define LP_WDT_WDT_STG3_M  (LP_WDT_WDT_STG3_V << LP_WDT_WDT_STG3_S)
#define LP_WDT_WDT_STG3_V  0x00000007U
#define LP_WDT_WDT_STG3_S  19
/** LP_WDT_WDT_STG2 : R/W; bitpos: [24:22]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_STG2    0x00000007U
#define LP_WDT_WDT_STG2_M  (LP_WDT_WDT_STG2_V << LP_WDT_WDT_STG2_S)
#define LP_WDT_WDT_STG2_V  0x00000007U
#define LP_WDT_WDT_STG2_S  22
/** LP_WDT_WDT_STG1 : R/W; bitpos: [27:25]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_STG1    0x00000007U
#define LP_WDT_WDT_STG1_M  (LP_WDT_WDT_STG1_V << LP_WDT_WDT_STG1_S)
#define LP_WDT_WDT_STG1_V  0x00000007U
#define LP_WDT_WDT_STG1_S  25
/** LP_WDT_WDT_STG0 : R/W; bitpos: [30:28]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_STG0    0x00000007U
#define LP_WDT_WDT_STG0_M  (LP_WDT_WDT_STG0_V << LP_WDT_WDT_STG0_S)
#define LP_WDT_WDT_STG0_V  0x00000007U
#define LP_WDT_WDT_STG0_S  28
/** LP_WDT_WDT_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_EN    (BIT(31))
#define LP_WDT_WDT_EN_M  (LP_WDT_WDT_EN_V << LP_WDT_WDT_EN_S)
#define LP_WDT_WDT_EN_V  0x00000001U
#define LP_WDT_WDT_EN_S  31

/** LP_WDT_CONFIG1_REG register
 *  need_des
 */
#define LP_WDT_CONFIG1_REG (DR_REG_LP_WDT_BASE + 0x4)
/** LP_WDT_WDT_STG0_HOLD : R/W; bitpos: [31:0]; default: 200000;
 *  need_des
 */
#define LP_WDT_WDT_STG0_HOLD    0xFFFFFFFFU
#define LP_WDT_WDT_STG0_HOLD_M  (LP_WDT_WDT_STG0_HOLD_V << LP_WDT_WDT_STG0_HOLD_S)
#define LP_WDT_WDT_STG0_HOLD_V  0xFFFFFFFFU
#define LP_WDT_WDT_STG0_HOLD_S  0

/** LP_WDT_CONFIG2_REG register
 *  need_des
 */
#define LP_WDT_CONFIG2_REG (DR_REG_LP_WDT_BASE + 0x8)
/** LP_WDT_WDT_STG1_HOLD : R/W; bitpos: [31:0]; default: 80000;
 *  need_des
 */
#define LP_WDT_WDT_STG1_HOLD    0xFFFFFFFFU
#define LP_WDT_WDT_STG1_HOLD_M  (LP_WDT_WDT_STG1_HOLD_V << LP_WDT_WDT_STG1_HOLD_S)
#define LP_WDT_WDT_STG1_HOLD_V  0xFFFFFFFFU
#define LP_WDT_WDT_STG1_HOLD_S  0

/** LP_WDT_CONFIG3_REG register
 *  need_des
 */
#define LP_WDT_CONFIG3_REG (DR_REG_LP_WDT_BASE + 0xc)
/** LP_WDT_WDT_STG2_HOLD : R/W; bitpos: [31:0]; default: 4095;
 *  need_des
 */
#define LP_WDT_WDT_STG2_HOLD    0xFFFFFFFFU
#define LP_WDT_WDT_STG2_HOLD_M  (LP_WDT_WDT_STG2_HOLD_V << LP_WDT_WDT_STG2_HOLD_S)
#define LP_WDT_WDT_STG2_HOLD_V  0xFFFFFFFFU
#define LP_WDT_WDT_STG2_HOLD_S  0

/** LP_WDT_CONFIG4_REG register
 *  need_des
 */
#define LP_WDT_CONFIG4_REG (DR_REG_LP_WDT_BASE + 0x10)
/** LP_WDT_WDT_STG3_HOLD : R/W; bitpos: [31:0]; default: 4095;
 *  need_des
 */
#define LP_WDT_WDT_STG3_HOLD    0xFFFFFFFFU
#define LP_WDT_WDT_STG3_HOLD_M  (LP_WDT_WDT_STG3_HOLD_V << LP_WDT_WDT_STG3_HOLD_S)
#define LP_WDT_WDT_STG3_HOLD_V  0xFFFFFFFFU
#define LP_WDT_WDT_STG3_HOLD_S  0

/** LP_WDT_FEED_REG register
 *  need_des
 */
#define LP_WDT_FEED_REG (DR_REG_LP_WDT_BASE + 0x14)
/** LP_WDT_FEED : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_FEED    (BIT(31))
#define LP_WDT_FEED_M  (LP_WDT_FEED_V << LP_WDT_FEED_S)
#define LP_WDT_FEED_V  0x00000001U
#define LP_WDT_FEED_S  31

/** LP_WDT_WPROTECT_REG register
 *  need_des
 */
#define LP_WDT_WPROTECT_REG (DR_REG_LP_WDT_BASE + 0x18)
/** LP_WDT_WDT_WKEY : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_WDT_WDT_WKEY    0xFFFFFFFFU
#define LP_WDT_WDT_WKEY_M  (LP_WDT_WDT_WKEY_V << LP_WDT_WDT_WKEY_S)
#define LP_WDT_WDT_WKEY_V  0xFFFFFFFFU
#define LP_WDT_WDT_WKEY_S  0

/** LP_WDT_SWD_CONFIG_REG register
 *  need_des
 */
#define LP_WDT_SWD_CONFIG_REG (DR_REG_LP_WDT_BASE + 0x1c)
/** LP_WDT_SWD_RESET_FLAG : RO; bitpos: [0]; default: 0;
 *  need_des
 */
#define LP_WDT_SWD_RESET_FLAG    (BIT(0))
#define LP_WDT_SWD_RESET_FLAG_M  (LP_WDT_SWD_RESET_FLAG_V << LP_WDT_SWD_RESET_FLAG_S)
#define LP_WDT_SWD_RESET_FLAG_V  0x00000001U
#define LP_WDT_SWD_RESET_FLAG_S  0
/** LP_WDT_SWD_AUTO_FEED_EN : R/W; bitpos: [18]; default: 0;
 *  need_des
 */
#define LP_WDT_SWD_AUTO_FEED_EN    (BIT(18))
#define LP_WDT_SWD_AUTO_FEED_EN_M  (LP_WDT_SWD_AUTO_FEED_EN_V << LP_WDT_SWD_AUTO_FEED_EN_S)
#define LP_WDT_SWD_AUTO_FEED_EN_V  0x00000001U
#define LP_WDT_SWD_AUTO_FEED_EN_S  18
/** LP_WDT_SWD_RST_FLAG_CLR : WT; bitpos: [19]; default: 0;
 *  need_des
 */
#define LP_WDT_SWD_RST_FLAG_CLR    (BIT(19))
#define LP_WDT_SWD_RST_FLAG_CLR_M  (LP_WDT_SWD_RST_FLAG_CLR_V << LP_WDT_SWD_RST_FLAG_CLR_S)
#define LP_WDT_SWD_RST_FLAG_CLR_V  0x00000001U
#define LP_WDT_SWD_RST_FLAG_CLR_S  19
/** LP_WDT_SWD_SIGNAL_WIDTH : R/W; bitpos: [29:20]; default: 300;
 *  need_des
 */
#define LP_WDT_SWD_SIGNAL_WIDTH    0x000003FFU
#define LP_WDT_SWD_SIGNAL_WIDTH_M  (LP_WDT_SWD_SIGNAL_WIDTH_V << LP_WDT_SWD_SIGNAL_WIDTH_S)
#define LP_WDT_SWD_SIGNAL_WIDTH_V  0x000003FFU
#define LP_WDT_SWD_SIGNAL_WIDTH_S  20
/** LP_WDT_SWD_DISABLE : R/W; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_WDT_SWD_DISABLE    (BIT(30))
#define LP_WDT_SWD_DISABLE_M  (LP_WDT_SWD_DISABLE_V << LP_WDT_SWD_DISABLE_S)
#define LP_WDT_SWD_DISABLE_V  0x00000001U
#define LP_WDT_SWD_DISABLE_S  30
/** LP_WDT_SWD_FEED : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_SWD_FEED    (BIT(31))
#define LP_WDT_SWD_FEED_M  (LP_WDT_SWD_FEED_V << LP_WDT_SWD_FEED_S)
#define LP_WDT_SWD_FEED_V  0x00000001U
#define LP_WDT_SWD_FEED_S  31

/** LP_WDT_SWD_WPROTECT_REG register
 *  need_des
 */
#define LP_WDT_SWD_WPROTECT_REG (DR_REG_LP_WDT_BASE + 0x20)
/** LP_WDT_SWD_WKEY : R/W; bitpos: [31:0]; default: 0;
 *  need_des
 */
#define LP_WDT_SWD_WKEY    0xFFFFFFFFU
#define LP_WDT_SWD_WKEY_M  (LP_WDT_SWD_WKEY_V << LP_WDT_SWD_WKEY_S)
#define LP_WDT_SWD_WKEY_V  0xFFFFFFFFU
#define LP_WDT_SWD_WKEY_S  0

/** LP_WDT_INT_RAW_REG register
 *  need_des
 */
#define LP_WDT_INT_RAW_REG (DR_REG_LP_WDT_BASE + 0x24)
/** LP_WDT_SUPER_WDT_INT_RAW : R/WTC/SS; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_WDT_SUPER_WDT_INT_RAW    (BIT(30))
#define LP_WDT_SUPER_WDT_INT_RAW_M  (LP_WDT_SUPER_WDT_INT_RAW_V << LP_WDT_SUPER_WDT_INT_RAW_S)
#define LP_WDT_SUPER_WDT_INT_RAW_V  0x00000001U
#define LP_WDT_SUPER_WDT_INT_RAW_S  30
/** LP_WDT_LP_WDT_INT_RAW : R/WTC/SS; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_LP_WDT_INT_RAW    (BIT(31))
#define LP_WDT_LP_WDT_INT_RAW_M  (LP_WDT_LP_WDT_INT_RAW_V << LP_WDT_LP_WDT_INT_RAW_S)
#define LP_WDT_LP_WDT_INT_RAW_V  0x00000001U
#define LP_WDT_LP_WDT_INT_RAW_S  31

/** LP_WDT_INT_ST_REG register
 *  need_des
 */
#define LP_WDT_INT_ST_REG (DR_REG_LP_WDT_BASE + 0x28)
/** LP_WDT_SUPER_WDT_INT_ST : RO; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_WDT_SUPER_WDT_INT_ST    (BIT(30))
#define LP_WDT_SUPER_WDT_INT_ST_M  (LP_WDT_SUPER_WDT_INT_ST_V << LP_WDT_SUPER_WDT_INT_ST_S)
#define LP_WDT_SUPER_WDT_INT_ST_V  0x00000001U
#define LP_WDT_SUPER_WDT_INT_ST_S  30
/** LP_WDT_LP_WDT_INT_ST : RO; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_LP_WDT_INT_ST    (BIT(31))
#define LP_WDT_LP_WDT_INT_ST_M  (LP_WDT_LP_WDT_INT_ST_V << LP_WDT_LP_WDT_INT_ST_S)
#define LP_WDT_LP_WDT_INT_ST_V  0x00000001U
#define LP_WDT_LP_WDT_INT_ST_S  31

/** LP_WDT_INT_ENA_REG register
 *  need_des
 */
#define LP_WDT_INT_ENA_REG (DR_REG_LP_WDT_BASE + 0x2c)
/** LP_WDT_SUPER_WDT_INT_ENA : R/W; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_WDT_SUPER_WDT_INT_ENA    (BIT(30))
#define LP_WDT_SUPER_WDT_INT_ENA_M  (LP_WDT_SUPER_WDT_INT_ENA_V << LP_WDT_SUPER_WDT_INT_ENA_S)
#define LP_WDT_SUPER_WDT_INT_ENA_V  0x00000001U
#define LP_WDT_SUPER_WDT_INT_ENA_S  30
/** LP_WDT_LP_WDT_INT_ENA : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_LP_WDT_INT_ENA    (BIT(31))
#define LP_WDT_LP_WDT_INT_ENA_M  (LP_WDT_LP_WDT_INT_ENA_V << LP_WDT_LP_WDT_INT_ENA_S)
#define LP_WDT_LP_WDT_INT_ENA_V  0x00000001U
#define LP_WDT_LP_WDT_INT_ENA_S  31

/** LP_WDT_INT_CLR_REG register
 *  need_des
 */
#define LP_WDT_INT_CLR_REG (DR_REG_LP_WDT_BASE + 0x30)
/** LP_WDT_SUPER_WDT_INT_CLR : WT; bitpos: [30]; default: 0;
 *  need_des
 */
#define LP_WDT_SUPER_WDT_INT_CLR    (BIT(30))
#define LP_WDT_SUPER_WDT_INT_CLR_M  (LP_WDT_SUPER_WDT_INT_CLR_V << LP_WDT_SUPER_WDT_INT_CLR_S)
#define LP_WDT_SUPER_WDT_INT_CLR_V  0x00000001U
#define LP_WDT_SUPER_WDT_INT_CLR_S  30
/** LP_WDT_LP_WDT_INT_CLR : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_LP_WDT_INT_CLR    (BIT(31))
#define LP_WDT_LP_WDT_INT_CLR_M  (LP_WDT_LP_WDT_INT_CLR_V << LP_WDT_LP_WDT_INT_CLR_S)
#define LP_WDT_LP_WDT_INT_CLR_V  0x00000001U
#define LP_WDT_LP_WDT_INT_CLR_S  31

/** LP_WDT_DATE_REG register
 *  need_des
 */
#define LP_WDT_DATE_REG (DR_REG_LP_WDT_BASE + 0x3fc)
/** LP_WDT_LP_WDT_DATE : R/W; bitpos: [30:0]; default: 34676864;
 *  need_des
 */
#define LP_WDT_LP_WDT_DATE    0x7FFFFFFFU
#define LP_WDT_LP_WDT_DATE_M  (LP_WDT_LP_WDT_DATE_V << LP_WDT_LP_WDT_DATE_S)
#define LP_WDT_LP_WDT_DATE_V  0x7FFFFFFFU
#define LP_WDT_LP_WDT_DATE_S  0
/** LP_WDT_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_WDT_CLK_EN    (BIT(31))
#define LP_WDT_CLK_EN_M  (LP_WDT_CLK_EN_V << LP_WDT_CLK_EN_S)
#define LP_WDT_CLK_EN_V  0x00000001U
#define LP_WDT_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
