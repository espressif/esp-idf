/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** TEE_DMA_DATE_REG register
 *  NA
 */
#define TEE_DMA_DATE_REG (DR_REG_TEE_DMA_BASE + 0x0)
/** TEE_DMA_TEE_DATE : R/W; bitpos: [31:0]; default: 539165460;
 *  NA
 */
#define TEE_DMA_TEE_DATE    0xFFFFFFFFU
#define TEE_DMA_TEE_DATE_M  (TEE_DMA_TEE_DATE_V << TEE_DMA_TEE_DATE_S)
#define TEE_DMA_TEE_DATE_V  0xFFFFFFFFU
#define TEE_DMA_TEE_DATE_S  0

/** TEE_DMA_CLK_EN_REG register
 *  NA
 */
#define TEE_DMA_CLK_EN_REG (DR_REG_TEE_DMA_BASE + 0x4)
/** TEE_DMA_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  NA
 */
#define TEE_DMA_CLK_EN    (BIT(0))
#define TEE_DMA_CLK_EN_M  (TEE_DMA_CLK_EN_V << TEE_DMA_CLK_EN_S)
#define TEE_DMA_CLK_EN_V  0x00000001U
#define TEE_DMA_CLK_EN_S  0

/** TEE_DMA_REGION0_LOW_REG register
 *  Region0 address low register.
 */
#define TEE_DMA_REGION0_LOW_REG (DR_REG_TEE_DMA_BASE + 0x8)
/** TEE_DMA_REGION0_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region0 address low.
 */
#define TEE_DMA_REGION0_LOW    0x000FFFFFU
#define TEE_DMA_REGION0_LOW_M  (TEE_DMA_REGION0_LOW_V << TEE_DMA_REGION0_LOW_S)
#define TEE_DMA_REGION0_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION0_LOW_S  12

/** TEE_DMA_REGION0_HIGH_REG register
 *  Region0 address high register.
 */
#define TEE_DMA_REGION0_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xc)
/** TEE_DMA_REGION0_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region0 address high.
 */
#define TEE_DMA_REGION0_HIGH    0x000FFFFFU
#define TEE_DMA_REGION0_HIGH_M  (TEE_DMA_REGION0_HIGH_V << TEE_DMA_REGION0_HIGH_S)
#define TEE_DMA_REGION0_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION0_HIGH_S  12

/** TEE_DMA_REGION1_LOW_REG register
 *  Region1 address low register.
 */
#define TEE_DMA_REGION1_LOW_REG (DR_REG_TEE_DMA_BASE + 0x10)
/** TEE_DMA_REGION1_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region1 address low.
 */
#define TEE_DMA_REGION1_LOW    0x000FFFFFU
#define TEE_DMA_REGION1_LOW_M  (TEE_DMA_REGION1_LOW_V << TEE_DMA_REGION1_LOW_S)
#define TEE_DMA_REGION1_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION1_LOW_S  12

/** TEE_DMA_REGION1_HIGH_REG register
 *  Region1 address high register.
 */
#define TEE_DMA_REGION1_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x14)
/** TEE_DMA_REGION1_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region1 address high.
 */
#define TEE_DMA_REGION1_HIGH    0x000FFFFFU
#define TEE_DMA_REGION1_HIGH_M  (TEE_DMA_REGION1_HIGH_V << TEE_DMA_REGION1_HIGH_S)
#define TEE_DMA_REGION1_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION1_HIGH_S  12

/** TEE_DMA_REGION2_LOW_REG register
 *  Region2 address low register.
 */
#define TEE_DMA_REGION2_LOW_REG (DR_REG_TEE_DMA_BASE + 0x18)
/** TEE_DMA_REGION2_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region2 address low.
 */
#define TEE_DMA_REGION2_LOW    0x000FFFFFU
#define TEE_DMA_REGION2_LOW_M  (TEE_DMA_REGION2_LOW_V << TEE_DMA_REGION2_LOW_S)
#define TEE_DMA_REGION2_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION2_LOW_S  12

/** TEE_DMA_REGION2_HIGH_REG register
 *  Region2 address high register.
 */
#define TEE_DMA_REGION2_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x1c)
/** TEE_DMA_REGION2_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region2 address high.
 */
#define TEE_DMA_REGION2_HIGH    0x000FFFFFU
#define TEE_DMA_REGION2_HIGH_M  (TEE_DMA_REGION2_HIGH_V << TEE_DMA_REGION2_HIGH_S)
#define TEE_DMA_REGION2_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION2_HIGH_S  12

/** TEE_DMA_REGION3_LOW_REG register
 *  Region3 address low register.
 */
#define TEE_DMA_REGION3_LOW_REG (DR_REG_TEE_DMA_BASE + 0x20)
/** TEE_DMA_REGION3_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region3 address low.
 */
#define TEE_DMA_REGION3_LOW    0x000FFFFFU
#define TEE_DMA_REGION3_LOW_M  (TEE_DMA_REGION3_LOW_V << TEE_DMA_REGION3_LOW_S)
#define TEE_DMA_REGION3_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION3_LOW_S  12

/** TEE_DMA_REGION3_HIGH_REG register
 *  Region3 address high register.
 */
#define TEE_DMA_REGION3_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x24)
/** TEE_DMA_REGION3_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region3 address high.
 */
#define TEE_DMA_REGION3_HIGH    0x000FFFFFU
#define TEE_DMA_REGION3_HIGH_M  (TEE_DMA_REGION3_HIGH_V << TEE_DMA_REGION3_HIGH_S)
#define TEE_DMA_REGION3_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION3_HIGH_S  12

/** TEE_DMA_REGION4_LOW_REG register
 *  Region4 address low register.
 */
#define TEE_DMA_REGION4_LOW_REG (DR_REG_TEE_DMA_BASE + 0x28)
/** TEE_DMA_REGION4_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region4 address low.
 */
#define TEE_DMA_REGION4_LOW    0x000FFFFFU
#define TEE_DMA_REGION4_LOW_M  (TEE_DMA_REGION4_LOW_V << TEE_DMA_REGION4_LOW_S)
#define TEE_DMA_REGION4_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION4_LOW_S  12

/** TEE_DMA_REGION4_HIGH_REG register
 *  Region4 address high register.
 */
#define TEE_DMA_REGION4_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x2c)
/** TEE_DMA_REGION4_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region4 address high.
 */
#define TEE_DMA_REGION4_HIGH    0x000FFFFFU
#define TEE_DMA_REGION4_HIGH_M  (TEE_DMA_REGION4_HIGH_V << TEE_DMA_REGION4_HIGH_S)
#define TEE_DMA_REGION4_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION4_HIGH_S  12

/** TEE_DMA_REGION5_LOW_REG register
 *  Region5 address low register.
 */
#define TEE_DMA_REGION5_LOW_REG (DR_REG_TEE_DMA_BASE + 0x30)
/** TEE_DMA_REGION5_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region5 address low.
 */
#define TEE_DMA_REGION5_LOW    0x000FFFFFU
#define TEE_DMA_REGION5_LOW_M  (TEE_DMA_REGION5_LOW_V << TEE_DMA_REGION5_LOW_S)
#define TEE_DMA_REGION5_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION5_LOW_S  12

/** TEE_DMA_REGION5_HIGH_REG register
 *  Region5 address high register.
 */
#define TEE_DMA_REGION5_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x34)
/** TEE_DMA_REGION5_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region5 address high.
 */
#define TEE_DMA_REGION5_HIGH    0x000FFFFFU
#define TEE_DMA_REGION5_HIGH_M  (TEE_DMA_REGION5_HIGH_V << TEE_DMA_REGION5_HIGH_S)
#define TEE_DMA_REGION5_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION5_HIGH_S  12

/** TEE_DMA_REGION6_LOW_REG register
 *  Region6 address low register.
 */
#define TEE_DMA_REGION6_LOW_REG (DR_REG_TEE_DMA_BASE + 0x38)
/** TEE_DMA_REGION6_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region6 address low.
 */
#define TEE_DMA_REGION6_LOW    0x000FFFFFU
#define TEE_DMA_REGION6_LOW_M  (TEE_DMA_REGION6_LOW_V << TEE_DMA_REGION6_LOW_S)
#define TEE_DMA_REGION6_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION6_LOW_S  12

/** TEE_DMA_REGION6_HIGH_REG register
 *  Region6 address high register.
 */
#define TEE_DMA_REGION6_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x3c)
/** TEE_DMA_REGION6_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region6 address high.
 */
#define TEE_DMA_REGION6_HIGH    0x000FFFFFU
#define TEE_DMA_REGION6_HIGH_M  (TEE_DMA_REGION6_HIGH_V << TEE_DMA_REGION6_HIGH_S)
#define TEE_DMA_REGION6_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION6_HIGH_S  12

/** TEE_DMA_REGION7_LOW_REG register
 *  Region7 address low register.
 */
#define TEE_DMA_REGION7_LOW_REG (DR_REG_TEE_DMA_BASE + 0x40)
/** TEE_DMA_REGION7_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region7 address low.
 */
#define TEE_DMA_REGION7_LOW    0x000FFFFFU
#define TEE_DMA_REGION7_LOW_M  (TEE_DMA_REGION7_LOW_V << TEE_DMA_REGION7_LOW_S)
#define TEE_DMA_REGION7_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION7_LOW_S  12

/** TEE_DMA_REGION7_HIGH_REG register
 *  Region7 address high register.
 */
#define TEE_DMA_REGION7_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x44)
/** TEE_DMA_REGION7_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region7 address high.
 */
#define TEE_DMA_REGION7_HIGH    0x000FFFFFU
#define TEE_DMA_REGION7_HIGH_M  (TEE_DMA_REGION7_HIGH_V << TEE_DMA_REGION7_HIGH_S)
#define TEE_DMA_REGION7_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION7_HIGH_S  12

/** TEE_DMA_REGION8_LOW_REG register
 *  Region8 address low register.
 */
#define TEE_DMA_REGION8_LOW_REG (DR_REG_TEE_DMA_BASE + 0x48)
/** TEE_DMA_REGION8_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region8 address low.
 */
#define TEE_DMA_REGION8_LOW    0x000FFFFFU
#define TEE_DMA_REGION8_LOW_M  (TEE_DMA_REGION8_LOW_V << TEE_DMA_REGION8_LOW_S)
#define TEE_DMA_REGION8_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION8_LOW_S  12

/** TEE_DMA_REGION8_HIGH_REG register
 *  Region8 address high register.
 */
#define TEE_DMA_REGION8_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x4c)
/** TEE_DMA_REGION8_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region8 address high.
 */
#define TEE_DMA_REGION8_HIGH    0x000FFFFFU
#define TEE_DMA_REGION8_HIGH_M  (TEE_DMA_REGION8_HIGH_V << TEE_DMA_REGION8_HIGH_S)
#define TEE_DMA_REGION8_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION8_HIGH_S  12

/** TEE_DMA_REGION9_LOW_REG register
 *  Region9 address low register.
 */
#define TEE_DMA_REGION9_LOW_REG (DR_REG_TEE_DMA_BASE + 0x50)
/** TEE_DMA_REGION9_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region9 address low.
 */
#define TEE_DMA_REGION9_LOW    0x000FFFFFU
#define TEE_DMA_REGION9_LOW_M  (TEE_DMA_REGION9_LOW_V << TEE_DMA_REGION9_LOW_S)
#define TEE_DMA_REGION9_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION9_LOW_S  12

/** TEE_DMA_REGION9_HIGH_REG register
 *  Region9 address high register.
 */
#define TEE_DMA_REGION9_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x54)
/** TEE_DMA_REGION9_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region9 address high.
 */
#define TEE_DMA_REGION9_HIGH    0x000FFFFFU
#define TEE_DMA_REGION9_HIGH_M  (TEE_DMA_REGION9_HIGH_V << TEE_DMA_REGION9_HIGH_S)
#define TEE_DMA_REGION9_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION9_HIGH_S  12

/** TEE_DMA_REGION10_LOW_REG register
 *  Region10 address low register.
 */
#define TEE_DMA_REGION10_LOW_REG (DR_REG_TEE_DMA_BASE + 0x58)
/** TEE_DMA_REGION10_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region10 address low.
 */
#define TEE_DMA_REGION10_LOW    0x000FFFFFU
#define TEE_DMA_REGION10_LOW_M  (TEE_DMA_REGION10_LOW_V << TEE_DMA_REGION10_LOW_S)
#define TEE_DMA_REGION10_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION10_LOW_S  12

/** TEE_DMA_REGION10_HIGH_REG register
 *  Region10 address high register.
 */
#define TEE_DMA_REGION10_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x5c)
/** TEE_DMA_REGION10_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region10 address high.
 */
#define TEE_DMA_REGION10_HIGH    0x000FFFFFU
#define TEE_DMA_REGION10_HIGH_M  (TEE_DMA_REGION10_HIGH_V << TEE_DMA_REGION10_HIGH_S)
#define TEE_DMA_REGION10_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION10_HIGH_S  12

/** TEE_DMA_REGION11_LOW_REG register
 *  Region11 address low register.
 */
#define TEE_DMA_REGION11_LOW_REG (DR_REG_TEE_DMA_BASE + 0x60)
/** TEE_DMA_REGION11_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region11 address low.
 */
#define TEE_DMA_REGION11_LOW    0x000FFFFFU
#define TEE_DMA_REGION11_LOW_M  (TEE_DMA_REGION11_LOW_V << TEE_DMA_REGION11_LOW_S)
#define TEE_DMA_REGION11_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION11_LOW_S  12

/** TEE_DMA_REGION11_HIGH_REG register
 *  Region11 address high register.
 */
#define TEE_DMA_REGION11_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x64)
/** TEE_DMA_REGION11_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region11 address high.
 */
#define TEE_DMA_REGION11_HIGH    0x000FFFFFU
#define TEE_DMA_REGION11_HIGH_M  (TEE_DMA_REGION11_HIGH_V << TEE_DMA_REGION11_HIGH_S)
#define TEE_DMA_REGION11_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION11_HIGH_S  12

/** TEE_DMA_REGION12_LOW_REG register
 *  Region12 address low register.
 */
#define TEE_DMA_REGION12_LOW_REG (DR_REG_TEE_DMA_BASE + 0x68)
/** TEE_DMA_REGION12_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region12 address low.
 */
#define TEE_DMA_REGION12_LOW    0x000FFFFFU
#define TEE_DMA_REGION12_LOW_M  (TEE_DMA_REGION12_LOW_V << TEE_DMA_REGION12_LOW_S)
#define TEE_DMA_REGION12_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION12_LOW_S  12

/** TEE_DMA_REGION12_HIGH_REG register
 *  Region12 address high register.
 */
#define TEE_DMA_REGION12_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x6c)
/** TEE_DMA_REGION12_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region12 address high.
 */
#define TEE_DMA_REGION12_HIGH    0x000FFFFFU
#define TEE_DMA_REGION12_HIGH_M  (TEE_DMA_REGION12_HIGH_V << TEE_DMA_REGION12_HIGH_S)
#define TEE_DMA_REGION12_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION12_HIGH_S  12

/** TEE_DMA_REGION13_LOW_REG register
 *  Region13 address low register.
 */
#define TEE_DMA_REGION13_LOW_REG (DR_REG_TEE_DMA_BASE + 0x70)
/** TEE_DMA_REGION13_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region13 address low.
 */
#define TEE_DMA_REGION13_LOW    0x000FFFFFU
#define TEE_DMA_REGION13_LOW_M  (TEE_DMA_REGION13_LOW_V << TEE_DMA_REGION13_LOW_S)
#define TEE_DMA_REGION13_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION13_LOW_S  12

/** TEE_DMA_REGION13_HIGH_REG register
 *  Region13 address high register.
 */
#define TEE_DMA_REGION13_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x74)
/** TEE_DMA_REGION13_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region13 address high.
 */
#define TEE_DMA_REGION13_HIGH    0x000FFFFFU
#define TEE_DMA_REGION13_HIGH_M  (TEE_DMA_REGION13_HIGH_V << TEE_DMA_REGION13_HIGH_S)
#define TEE_DMA_REGION13_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION13_HIGH_S  12

/** TEE_DMA_REGION14_LOW_REG register
 *  Region14 address low register.
 */
#define TEE_DMA_REGION14_LOW_REG (DR_REG_TEE_DMA_BASE + 0x78)
/** TEE_DMA_REGION14_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region14 address low.
 */
#define TEE_DMA_REGION14_LOW    0x000FFFFFU
#define TEE_DMA_REGION14_LOW_M  (TEE_DMA_REGION14_LOW_V << TEE_DMA_REGION14_LOW_S)
#define TEE_DMA_REGION14_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION14_LOW_S  12

/** TEE_DMA_REGION14_HIGH_REG register
 *  Region14 address high register.
 */
#define TEE_DMA_REGION14_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x7c)
/** TEE_DMA_REGION14_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region14 address high.
 */
#define TEE_DMA_REGION14_HIGH    0x000FFFFFU
#define TEE_DMA_REGION14_HIGH_M  (TEE_DMA_REGION14_HIGH_V << TEE_DMA_REGION14_HIGH_S)
#define TEE_DMA_REGION14_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION14_HIGH_S  12

/** TEE_DMA_REGION15_LOW_REG register
 *  Region15 address low register.
 */
#define TEE_DMA_REGION15_LOW_REG (DR_REG_TEE_DMA_BASE + 0x80)
/** TEE_DMA_REGION15_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region15 address low.
 */
#define TEE_DMA_REGION15_LOW    0x000FFFFFU
#define TEE_DMA_REGION15_LOW_M  (TEE_DMA_REGION15_LOW_V << TEE_DMA_REGION15_LOW_S)
#define TEE_DMA_REGION15_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION15_LOW_S  12

/** TEE_DMA_REGION15_HIGH_REG register
 *  Region15 address high register.
 */
#define TEE_DMA_REGION15_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x84)
/** TEE_DMA_REGION15_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region15 address high.
 */
#define TEE_DMA_REGION15_HIGH    0x000FFFFFU
#define TEE_DMA_REGION15_HIGH_M  (TEE_DMA_REGION15_HIGH_V << TEE_DMA_REGION15_HIGH_S)
#define TEE_DMA_REGION15_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION15_HIGH_S  12

/** TEE_DMA_REGION16_LOW_REG register
 *  Region16 address low register.
 */
#define TEE_DMA_REGION16_LOW_REG (DR_REG_TEE_DMA_BASE + 0x88)
/** TEE_DMA_REGION16_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region16 address low.
 */
#define TEE_DMA_REGION16_LOW    0x000FFFFFU
#define TEE_DMA_REGION16_LOW_M  (TEE_DMA_REGION16_LOW_V << TEE_DMA_REGION16_LOW_S)
#define TEE_DMA_REGION16_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION16_LOW_S  12

/** TEE_DMA_REGION16_HIGH_REG register
 *  Region16 address high register.
 */
#define TEE_DMA_REGION16_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x8c)
/** TEE_DMA_REGION16_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region16 address high.
 */
#define TEE_DMA_REGION16_HIGH    0x000FFFFFU
#define TEE_DMA_REGION16_HIGH_M  (TEE_DMA_REGION16_HIGH_V << TEE_DMA_REGION16_HIGH_S)
#define TEE_DMA_REGION16_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION16_HIGH_S  12

/** TEE_DMA_REGION17_LOW_REG register
 *  Region17 address low register.
 */
#define TEE_DMA_REGION17_LOW_REG (DR_REG_TEE_DMA_BASE + 0x90)
/** TEE_DMA_REGION17_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region17 address low.
 */
#define TEE_DMA_REGION17_LOW    0x000FFFFFU
#define TEE_DMA_REGION17_LOW_M  (TEE_DMA_REGION17_LOW_V << TEE_DMA_REGION17_LOW_S)
#define TEE_DMA_REGION17_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION17_LOW_S  12

/** TEE_DMA_REGION17_HIGH_REG register
 *  Region17 address high register.
 */
#define TEE_DMA_REGION17_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x94)
/** TEE_DMA_REGION17_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region17 address high.
 */
#define TEE_DMA_REGION17_HIGH    0x000FFFFFU
#define TEE_DMA_REGION17_HIGH_M  (TEE_DMA_REGION17_HIGH_V << TEE_DMA_REGION17_HIGH_S)
#define TEE_DMA_REGION17_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION17_HIGH_S  12

/** TEE_DMA_REGION18_LOW_REG register
 *  Region18 address low register.
 */
#define TEE_DMA_REGION18_LOW_REG (DR_REG_TEE_DMA_BASE + 0x98)
/** TEE_DMA_REGION18_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region18 address low.
 */
#define TEE_DMA_REGION18_LOW    0x000FFFFFU
#define TEE_DMA_REGION18_LOW_M  (TEE_DMA_REGION18_LOW_V << TEE_DMA_REGION18_LOW_S)
#define TEE_DMA_REGION18_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION18_LOW_S  12

/** TEE_DMA_REGION18_HIGH_REG register
 *  Region18 address high register.
 */
#define TEE_DMA_REGION18_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x9c)
/** TEE_DMA_REGION18_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region18 address high.
 */
#define TEE_DMA_REGION18_HIGH    0x000FFFFFU
#define TEE_DMA_REGION18_HIGH_M  (TEE_DMA_REGION18_HIGH_V << TEE_DMA_REGION18_HIGH_S)
#define TEE_DMA_REGION18_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION18_HIGH_S  12

/** TEE_DMA_REGION19_LOW_REG register
 *  Region19 address low register.
 */
#define TEE_DMA_REGION19_LOW_REG (DR_REG_TEE_DMA_BASE + 0xa0)
/** TEE_DMA_REGION19_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region19 address low.
 */
#define TEE_DMA_REGION19_LOW    0x000FFFFFU
#define TEE_DMA_REGION19_LOW_M  (TEE_DMA_REGION19_LOW_V << TEE_DMA_REGION19_LOW_S)
#define TEE_DMA_REGION19_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION19_LOW_S  12

/** TEE_DMA_REGION19_HIGH_REG register
 *  Region19 address high register.
 */
#define TEE_DMA_REGION19_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xa4)
/** TEE_DMA_REGION19_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region19 address high.
 */
#define TEE_DMA_REGION19_HIGH    0x000FFFFFU
#define TEE_DMA_REGION19_HIGH_M  (TEE_DMA_REGION19_HIGH_V << TEE_DMA_REGION19_HIGH_S)
#define TEE_DMA_REGION19_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION19_HIGH_S  12

/** TEE_DMA_REGION20_LOW_REG register
 *  Region20 address low register.
 */
#define TEE_DMA_REGION20_LOW_REG (DR_REG_TEE_DMA_BASE + 0xa8)
/** TEE_DMA_REGION20_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region20 address low.
 */
#define TEE_DMA_REGION20_LOW    0x000FFFFFU
#define TEE_DMA_REGION20_LOW_M  (TEE_DMA_REGION20_LOW_V << TEE_DMA_REGION20_LOW_S)
#define TEE_DMA_REGION20_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION20_LOW_S  12

/** TEE_DMA_REGION20_HIGH_REG register
 *  Region20 address high register.
 */
#define TEE_DMA_REGION20_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xac)
/** TEE_DMA_REGION20_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region20 address high.
 */
#define TEE_DMA_REGION20_HIGH    0x000FFFFFU
#define TEE_DMA_REGION20_HIGH_M  (TEE_DMA_REGION20_HIGH_V << TEE_DMA_REGION20_HIGH_S)
#define TEE_DMA_REGION20_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION20_HIGH_S  12

/** TEE_DMA_REGION21_LOW_REG register
 *  Region21 address low register.
 */
#define TEE_DMA_REGION21_LOW_REG (DR_REG_TEE_DMA_BASE + 0xb0)
/** TEE_DMA_REGION21_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region21 address low.
 */
#define TEE_DMA_REGION21_LOW    0x000FFFFFU
#define TEE_DMA_REGION21_LOW_M  (TEE_DMA_REGION21_LOW_V << TEE_DMA_REGION21_LOW_S)
#define TEE_DMA_REGION21_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION21_LOW_S  12

/** TEE_DMA_REGION21_HIGH_REG register
 *  Region21 address high register.
 */
#define TEE_DMA_REGION21_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xb4)
/** TEE_DMA_REGION21_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region21 address high.
 */
#define TEE_DMA_REGION21_HIGH    0x000FFFFFU
#define TEE_DMA_REGION21_HIGH_M  (TEE_DMA_REGION21_HIGH_V << TEE_DMA_REGION21_HIGH_S)
#define TEE_DMA_REGION21_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION21_HIGH_S  12

/** TEE_DMA_REGION22_LOW_REG register
 *  Region22 address low register.
 */
#define TEE_DMA_REGION22_LOW_REG (DR_REG_TEE_DMA_BASE + 0xb8)
/** TEE_DMA_REGION22_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region22 address low.
 */
#define TEE_DMA_REGION22_LOW    0x000FFFFFU
#define TEE_DMA_REGION22_LOW_M  (TEE_DMA_REGION22_LOW_V << TEE_DMA_REGION22_LOW_S)
#define TEE_DMA_REGION22_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION22_LOW_S  12

/** TEE_DMA_REGION22_HIGH_REG register
 *  Region22 address high register.
 */
#define TEE_DMA_REGION22_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xbc)
/** TEE_DMA_REGION22_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region22 address high.
 */
#define TEE_DMA_REGION22_HIGH    0x000FFFFFU
#define TEE_DMA_REGION22_HIGH_M  (TEE_DMA_REGION22_HIGH_V << TEE_DMA_REGION22_HIGH_S)
#define TEE_DMA_REGION22_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION22_HIGH_S  12

/** TEE_DMA_REGION23_LOW_REG register
 *  Region23 address low register.
 */
#define TEE_DMA_REGION23_LOW_REG (DR_REG_TEE_DMA_BASE + 0xc0)
/** TEE_DMA_REGION23_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region23 address low.
 */
#define TEE_DMA_REGION23_LOW    0x000FFFFFU
#define TEE_DMA_REGION23_LOW_M  (TEE_DMA_REGION23_LOW_V << TEE_DMA_REGION23_LOW_S)
#define TEE_DMA_REGION23_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION23_LOW_S  12

/** TEE_DMA_REGION23_HIGH_REG register
 *  Region23 address high register.
 */
#define TEE_DMA_REGION23_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xc4)
/** TEE_DMA_REGION23_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region23 address high.
 */
#define TEE_DMA_REGION23_HIGH    0x000FFFFFU
#define TEE_DMA_REGION23_HIGH_M  (TEE_DMA_REGION23_HIGH_V << TEE_DMA_REGION23_HIGH_S)
#define TEE_DMA_REGION23_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION23_HIGH_S  12

/** TEE_DMA_REGION24_LOW_REG register
 *  Region24 address low register.
 */
#define TEE_DMA_REGION24_LOW_REG (DR_REG_TEE_DMA_BASE + 0xc8)
/** TEE_DMA_REGION24_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region24 address low.
 */
#define TEE_DMA_REGION24_LOW    0x000FFFFFU
#define TEE_DMA_REGION24_LOW_M  (TEE_DMA_REGION24_LOW_V << TEE_DMA_REGION24_LOW_S)
#define TEE_DMA_REGION24_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION24_LOW_S  12

/** TEE_DMA_REGION24_HIGH_REG register
 *  Region24 address high register.
 */
#define TEE_DMA_REGION24_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xcc)
/** TEE_DMA_REGION24_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region24 address high.
 */
#define TEE_DMA_REGION24_HIGH    0x000FFFFFU
#define TEE_DMA_REGION24_HIGH_M  (TEE_DMA_REGION24_HIGH_V << TEE_DMA_REGION24_HIGH_S)
#define TEE_DMA_REGION24_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION24_HIGH_S  12

/** TEE_DMA_REGION25_LOW_REG register
 *  Region25 address low register.
 */
#define TEE_DMA_REGION25_LOW_REG (DR_REG_TEE_DMA_BASE + 0xd0)
/** TEE_DMA_REGION25_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region25 address low.
 */
#define TEE_DMA_REGION25_LOW    0x000FFFFFU
#define TEE_DMA_REGION25_LOW_M  (TEE_DMA_REGION25_LOW_V << TEE_DMA_REGION25_LOW_S)
#define TEE_DMA_REGION25_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION25_LOW_S  12

/** TEE_DMA_REGION25_HIGH_REG register
 *  Region25 address high register.
 */
#define TEE_DMA_REGION25_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xd4)
/** TEE_DMA_REGION25_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region25 address high.
 */
#define TEE_DMA_REGION25_HIGH    0x000FFFFFU
#define TEE_DMA_REGION25_HIGH_M  (TEE_DMA_REGION25_HIGH_V << TEE_DMA_REGION25_HIGH_S)
#define TEE_DMA_REGION25_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION25_HIGH_S  12

/** TEE_DMA_REGION26_LOW_REG register
 *  Region26 address low register.
 */
#define TEE_DMA_REGION26_LOW_REG (DR_REG_TEE_DMA_BASE + 0xd8)
/** TEE_DMA_REGION26_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region26 address low.
 */
#define TEE_DMA_REGION26_LOW    0x000FFFFFU
#define TEE_DMA_REGION26_LOW_M  (TEE_DMA_REGION26_LOW_V << TEE_DMA_REGION26_LOW_S)
#define TEE_DMA_REGION26_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION26_LOW_S  12

/** TEE_DMA_REGION26_HIGH_REG register
 *  Region26 address high register.
 */
#define TEE_DMA_REGION26_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xdc)
/** TEE_DMA_REGION26_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region26 address high.
 */
#define TEE_DMA_REGION26_HIGH    0x000FFFFFU
#define TEE_DMA_REGION26_HIGH_M  (TEE_DMA_REGION26_HIGH_V << TEE_DMA_REGION26_HIGH_S)
#define TEE_DMA_REGION26_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION26_HIGH_S  12

/** TEE_DMA_REGION27_LOW_REG register
 *  Region27 address low register.
 */
#define TEE_DMA_REGION27_LOW_REG (DR_REG_TEE_DMA_BASE + 0xe0)
/** TEE_DMA_REGION27_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region27 address low.
 */
#define TEE_DMA_REGION27_LOW    0x000FFFFFU
#define TEE_DMA_REGION27_LOW_M  (TEE_DMA_REGION27_LOW_V << TEE_DMA_REGION27_LOW_S)
#define TEE_DMA_REGION27_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION27_LOW_S  12

/** TEE_DMA_REGION27_HIGH_REG register
 *  Region27 address high register.
 */
#define TEE_DMA_REGION27_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xe4)
/** TEE_DMA_REGION27_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region27 address high.
 */
#define TEE_DMA_REGION27_HIGH    0x000FFFFFU
#define TEE_DMA_REGION27_HIGH_M  (TEE_DMA_REGION27_HIGH_V << TEE_DMA_REGION27_HIGH_S)
#define TEE_DMA_REGION27_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION27_HIGH_S  12

/** TEE_DMA_REGION28_LOW_REG register
 *  Region28 address low register.
 */
#define TEE_DMA_REGION28_LOW_REG (DR_REG_TEE_DMA_BASE + 0xe8)
/** TEE_DMA_REGION28_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region28 address low.
 */
#define TEE_DMA_REGION28_LOW    0x000FFFFFU
#define TEE_DMA_REGION28_LOW_M  (TEE_DMA_REGION28_LOW_V << TEE_DMA_REGION28_LOW_S)
#define TEE_DMA_REGION28_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION28_LOW_S  12

/** TEE_DMA_REGION28_HIGH_REG register
 *  Region28 address high register.
 */
#define TEE_DMA_REGION28_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xec)
/** TEE_DMA_REGION28_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region28 address high.
 */
#define TEE_DMA_REGION28_HIGH    0x000FFFFFU
#define TEE_DMA_REGION28_HIGH_M  (TEE_DMA_REGION28_HIGH_V << TEE_DMA_REGION28_HIGH_S)
#define TEE_DMA_REGION28_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION28_HIGH_S  12

/** TEE_DMA_REGION29_LOW_REG register
 *  Region29 address low register.
 */
#define TEE_DMA_REGION29_LOW_REG (DR_REG_TEE_DMA_BASE + 0xf0)
/** TEE_DMA_REGION29_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region29 address low.
 */
#define TEE_DMA_REGION29_LOW    0x000FFFFFU
#define TEE_DMA_REGION29_LOW_M  (TEE_DMA_REGION29_LOW_V << TEE_DMA_REGION29_LOW_S)
#define TEE_DMA_REGION29_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION29_LOW_S  12

/** TEE_DMA_REGION29_HIGH_REG register
 *  Region29 address high register.
 */
#define TEE_DMA_REGION29_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xf4)
/** TEE_DMA_REGION29_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region29 address high.
 */
#define TEE_DMA_REGION29_HIGH    0x000FFFFFU
#define TEE_DMA_REGION29_HIGH_M  (TEE_DMA_REGION29_HIGH_V << TEE_DMA_REGION29_HIGH_S)
#define TEE_DMA_REGION29_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION29_HIGH_S  12

/** TEE_DMA_REGION30_LOW_REG register
 *  Region30 address low register.
 */
#define TEE_DMA_REGION30_LOW_REG (DR_REG_TEE_DMA_BASE + 0xf8)
/** TEE_DMA_REGION30_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region30 address low.
 */
#define TEE_DMA_REGION30_LOW    0x000FFFFFU
#define TEE_DMA_REGION30_LOW_M  (TEE_DMA_REGION30_LOW_V << TEE_DMA_REGION30_LOW_S)
#define TEE_DMA_REGION30_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION30_LOW_S  12

/** TEE_DMA_REGION30_HIGH_REG register
 *  Region30 address high register.
 */
#define TEE_DMA_REGION30_HIGH_REG (DR_REG_TEE_DMA_BASE + 0xfc)
/** TEE_DMA_REGION30_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region30 address high.
 */
#define TEE_DMA_REGION30_HIGH    0x000FFFFFU
#define TEE_DMA_REGION30_HIGH_M  (TEE_DMA_REGION30_HIGH_V << TEE_DMA_REGION30_HIGH_S)
#define TEE_DMA_REGION30_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION30_HIGH_S  12

/** TEE_DMA_REGION31_LOW_REG register
 *  Region31 address low register.
 */
#define TEE_DMA_REGION31_LOW_REG (DR_REG_TEE_DMA_BASE + 0x100)
/** TEE_DMA_REGION31_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Region31 address low.
 */
#define TEE_DMA_REGION31_LOW    0x000FFFFFU
#define TEE_DMA_REGION31_LOW_M  (TEE_DMA_REGION31_LOW_V << TEE_DMA_REGION31_LOW_S)
#define TEE_DMA_REGION31_LOW_V  0x000FFFFFU
#define TEE_DMA_REGION31_LOW_S  12

/** TEE_DMA_REGION31_HIGH_REG register
 *  Region31 address high register.
 */
#define TEE_DMA_REGION31_HIGH_REG (DR_REG_TEE_DMA_BASE + 0x104)
/** TEE_DMA_REGION31_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Region31 address high.
 */
#define TEE_DMA_REGION31_HIGH    0x000FFFFFU
#define TEE_DMA_REGION31_HIGH_M  (TEE_DMA_REGION31_HIGH_V << TEE_DMA_REGION31_HIGH_S)
#define TEE_DMA_REGION31_HIGH_V  0x000FFFFFU
#define TEE_DMA_REGION31_HIGH_S  12

/** TEE_DMA_GMDA_CH0_R_PMS_REG register
 *  GDMA ch0 read permission control registers.
 */
#define TEE_DMA_GMDA_CH0_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x108)
/** TEE_DMA_GDMA_CH0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch0 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH0_R_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH0_R_PMS_M  (TEE_DMA_GDMA_CH0_R_PMS_V << TEE_DMA_GDMA_CH0_R_PMS_S)
#define TEE_DMA_GDMA_CH0_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH0_R_PMS_S  0

/** TEE_DMA_GMDA_CH0_W_PMS_REG register
 *  GDMA ch0 write permission control registers.
 */
#define TEE_DMA_GMDA_CH0_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x10c)
/** TEE_DMA_GDMA_CH0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch0 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH0_W_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH0_W_PMS_M  (TEE_DMA_GDMA_CH0_W_PMS_V << TEE_DMA_GDMA_CH0_W_PMS_S)
#define TEE_DMA_GDMA_CH0_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH0_W_PMS_S  0

/** TEE_DMA_GMDA_CH1_R_PMS_REG register
 *  GDMA ch1 read permission control registers.
 */
#define TEE_DMA_GMDA_CH1_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x110)
/** TEE_DMA_GDMA_CH1_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch1 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH1_R_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH1_R_PMS_M  (TEE_DMA_GDMA_CH1_R_PMS_V << TEE_DMA_GDMA_CH1_R_PMS_S)
#define TEE_DMA_GDMA_CH1_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH1_R_PMS_S  0

/** TEE_DMA_GMDA_CH1_W_PMS_REG register
 *  GDMA ch1 write permission control registers.
 */
#define TEE_DMA_GMDA_CH1_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x114)
/** TEE_DMA_GDMA_CH1_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch1 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH1_W_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH1_W_PMS_M  (TEE_DMA_GDMA_CH1_W_PMS_V << TEE_DMA_GDMA_CH1_W_PMS_S)
#define TEE_DMA_GDMA_CH1_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH1_W_PMS_S  0

/** TEE_DMA_GMDA_CH2_R_PMS_REG register
 *  GDMA ch2 read permission control registers.
 */
#define TEE_DMA_GMDA_CH2_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x118)
/** TEE_DMA_GDMA_CH2_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch2 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH2_R_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH2_R_PMS_M  (TEE_DMA_GDMA_CH2_R_PMS_V << TEE_DMA_GDMA_CH2_R_PMS_S)
#define TEE_DMA_GDMA_CH2_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH2_R_PMS_S  0

/** TEE_DMA_GMDA_CH2_W_PMS_REG register
 *  GDMA ch2 write permission control registers.
 */
#define TEE_DMA_GMDA_CH2_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x11c)
/** TEE_DMA_GDMA_CH2_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch2 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH2_W_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH2_W_PMS_M  (TEE_DMA_GDMA_CH2_W_PMS_V << TEE_DMA_GDMA_CH2_W_PMS_S)
#define TEE_DMA_GDMA_CH2_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH2_W_PMS_S  0

/** TEE_DMA_GMDA_CH3_R_PMS_REG register
 *  GDMA ch3 read permission control registers.
 */
#define TEE_DMA_GMDA_CH3_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x120)
/** TEE_DMA_GDMA_CH3_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch3 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH3_R_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH3_R_PMS_M  (TEE_DMA_GDMA_CH3_R_PMS_V << TEE_DMA_GDMA_CH3_R_PMS_S)
#define TEE_DMA_GDMA_CH3_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH3_R_PMS_S  0

/** TEE_DMA_GMDA_CH3_W_PMS_REG register
 *  GDMA ch3 write permission control registers.
 */
#define TEE_DMA_GMDA_CH3_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x124)
/** TEE_DMA_GDMA_CH3_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GDMA ch3 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GDMA_CH3_W_PMS    0xFFFFFFFFU
#define TEE_DMA_GDMA_CH3_W_PMS_M  (TEE_DMA_GDMA_CH3_W_PMS_V << TEE_DMA_GDMA_CH3_W_PMS_S)
#define TEE_DMA_GDMA_CH3_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GDMA_CH3_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_ADC_R_PMS_REG register
 *  AHB PDMA adc read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_ADC_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x128)
/** TEE_DMA_AHB_PDMA_ADC_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA adc read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_ADC_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_ADC_R_PMS_M  (TEE_DMA_AHB_PDMA_ADC_R_PMS_V << TEE_DMA_AHB_PDMA_ADC_R_PMS_S)
#define TEE_DMA_AHB_PDMA_ADC_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_ADC_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_ADC_W_PMS_REG register
 *  AHB PDMA adc write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_ADC_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x12c)
/** TEE_DMA_AHB_PDMA_ADC_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA adc write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_ADC_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_ADC_W_PMS_M  (TEE_DMA_AHB_PDMA_ADC_W_PMS_V << TEE_DMA_AHB_PDMA_ADC_W_PMS_S)
#define TEE_DMA_AHB_PDMA_ADC_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_ADC_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_I2S0_R_PMS_REG register
 *  AHB PDMA i2s0 read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I2S0_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x130)
/** TEE_DMA_AHB_PDMA_I2S0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i2s0 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I2S0_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S0_R_PMS_M  (TEE_DMA_AHB_PDMA_I2S0_R_PMS_V << TEE_DMA_AHB_PDMA_I2S0_R_PMS_S)
#define TEE_DMA_AHB_PDMA_I2S0_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S0_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_I2S0_W_PMS_REG register
 *  AHB PDMA i2s0 write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I2S0_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x134)
/** TEE_DMA_AHB_PDMA_I2S0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i2s0 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I2S0_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S0_W_PMS_M  (TEE_DMA_AHB_PDMA_I2S0_W_PMS_V << TEE_DMA_AHB_PDMA_I2S0_W_PMS_S)
#define TEE_DMA_AHB_PDMA_I2S0_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S0_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_I2S1_R_PMS_REG register
 *  AHB PDMA i2s1 read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I2S1_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x138)
/** TEE_DMA_AHB_PDMA_I2S1_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i2s1 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I2S1_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S1_R_PMS_M  (TEE_DMA_AHB_PDMA_I2S1_R_PMS_V << TEE_DMA_AHB_PDMA_I2S1_R_PMS_S)
#define TEE_DMA_AHB_PDMA_I2S1_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S1_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_I2S1_W_PMS_REG register
 *  AHB PDMA i2s1 write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I2S1_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x13c)
/** TEE_DMA_AHB_PDMA_I2S1_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i2s1 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I2S1_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S1_W_PMS_M  (TEE_DMA_AHB_PDMA_I2S1_W_PMS_V << TEE_DMA_AHB_PDMA_I2S1_W_PMS_S)
#define TEE_DMA_AHB_PDMA_I2S1_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S1_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_I2S2_R_PMS_REG register
 *  AHB PDMA i2s2 read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I2S2_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x140)
/** TEE_DMA_AHB_PDMA_I2S2_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i2s2 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I2S2_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S2_R_PMS_M  (TEE_DMA_AHB_PDMA_I2S2_R_PMS_V << TEE_DMA_AHB_PDMA_I2S2_R_PMS_S)
#define TEE_DMA_AHB_PDMA_I2S2_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S2_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_I2S2_W_PMS_REG register
 *  AHB PDMA i2s2 write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I2S2_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x144)
/** TEE_DMA_AHB_PDMA_I2S2_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i2s2 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I2S2_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S2_W_PMS_M  (TEE_DMA_AHB_PDMA_I2S2_W_PMS_V << TEE_DMA_AHB_PDMA_I2S2_W_PMS_S)
#define TEE_DMA_AHB_PDMA_I2S2_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I2S2_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_I3C_MST_R_PMS_REG register
 *  AHB PDMA i3s mst read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I3C_MST_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x148)
/** TEE_DMA_AHB_PDMA_I3C_MST_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i3c mst read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I3C_MST_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I3C_MST_R_PMS_M  (TEE_DMA_AHB_PDMA_I3C_MST_R_PMS_V << TEE_DMA_AHB_PDMA_I3C_MST_R_PMS_S)
#define TEE_DMA_AHB_PDMA_I3C_MST_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I3C_MST_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_I3C_MST_W_PMS_REG register
 *  AHB PDMA i3c mst  write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_I3C_MST_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x14c)
/** TEE_DMA_AHB_PDMA_I3C_MST_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA i3c mst  write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_I3C_MST_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I3C_MST_W_PMS_M  (TEE_DMA_AHB_PDMA_I3C_MST_W_PMS_V << TEE_DMA_AHB_PDMA_I3C_MST_W_PMS_S)
#define TEE_DMA_AHB_PDMA_I3C_MST_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_I3C_MST_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_UHCI0_R_PMS_REG register
 *  AHB PDMA uhci0 read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_UHCI0_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x150)
/** TEE_DMA_AHB_PDMA_UHCI0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA uhci0 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_UHCI0_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_UHCI0_R_PMS_M  (TEE_DMA_AHB_PDMA_UHCI0_R_PMS_V << TEE_DMA_AHB_PDMA_UHCI0_R_PMS_S)
#define TEE_DMA_AHB_PDMA_UHCI0_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_UHCI0_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_UHCI0_W_PMS_REG register
 *  AHB PDMA uhci0 write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_UHCI0_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x154)
/** TEE_DMA_AHB_PDMA_UHCI0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA uhci0 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_UHCI0_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_UHCI0_W_PMS_M  (TEE_DMA_AHB_PDMA_UHCI0_W_PMS_V << TEE_DMA_AHB_PDMA_UHCI0_W_PMS_S)
#define TEE_DMA_AHB_PDMA_UHCI0_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_UHCI0_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_RMT_R_PMS_REG register
 *  AHB PDMA rmt read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_RMT_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x158)
/** TEE_DMA_AHB_PDMA_RMT_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA rmt read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_RMT_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_RMT_R_PMS_M  (TEE_DMA_AHB_PDMA_RMT_R_PMS_V << TEE_DMA_AHB_PDMA_RMT_R_PMS_S)
#define TEE_DMA_AHB_PDMA_RMT_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_RMT_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_RMT_W_PMS_REG register
 *  AHB PDMA rmt write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_RMT_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x170)
/** TEE_DMA_AHB_PDMA_RMT_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA rmt write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_RMT_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_RMT_W_PMS_M  (TEE_DMA_AHB_PDMA_RMT_W_PMS_V << TEE_DMA_AHB_PDMA_RMT_W_PMS_S)
#define TEE_DMA_AHB_PDMA_RMT_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_RMT_W_PMS_S  0

/** TEE_DMA_AXI_PDMA_LCDCAM_R_PMS_REG register
 *  AXI PDMA lcdcam read permission control registers.
 */
#define TEE_DMA_AXI_PDMA_LCDCAM_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x174)
/** TEE_DMA_AXI_PDMA_LCDCAM_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA lcdcam read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_LCDCAM_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_LCDCAM_R_PMS_M  (TEE_DMA_AXI_PDMA_LCDCAM_R_PMS_V << TEE_DMA_AXI_PDMA_LCDCAM_R_PMS_S)
#define TEE_DMA_AXI_PDMA_LCDCAM_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_LCDCAM_R_PMS_S  0

/** TEE_DMA_AXI_PDMA_LCDCAM_W_PMS_REG register
 *  AXI PDMA lcdcam write permission control registers.
 */
#define TEE_DMA_AXI_PDMA_LCDCAM_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x178)
/** TEE_DMA_AXI_PDMA_LCDCAM_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA lcdcam write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_LCDCAM_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_LCDCAM_W_PMS_M  (TEE_DMA_AXI_PDMA_LCDCAM_W_PMS_V << TEE_DMA_AXI_PDMA_LCDCAM_W_PMS_S)
#define TEE_DMA_AXI_PDMA_LCDCAM_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_LCDCAM_W_PMS_S  0

/** TEE_DMA_AXI_PDMA_GPSPI2_R_PMS_REG register
 *  AXI PDMA gpspi2 read permission control registers.
 */
#define TEE_DMA_AXI_PDMA_GPSPI2_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x17c)
/** TEE_DMA_AXI_PDMA_GPSPI2_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA gpspi2 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_GPSPI2_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI2_R_PMS_M  (TEE_DMA_AXI_PDMA_GPSPI2_R_PMS_V << TEE_DMA_AXI_PDMA_GPSPI2_R_PMS_S)
#define TEE_DMA_AXI_PDMA_GPSPI2_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI2_R_PMS_S  0

/** TEE_DMA_AXI_PDMA_GPSPI2_W_PMS_REG register
 *  AXI PDMA gpspi2 write permission control registers.
 */
#define TEE_DMA_AXI_PDMA_GPSPI2_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x180)
/** TEE_DMA_AXI_PDMA_GPSPI2_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA gpspi2 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_GPSPI2_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI2_W_PMS_M  (TEE_DMA_AXI_PDMA_GPSPI2_W_PMS_V << TEE_DMA_AXI_PDMA_GPSPI2_W_PMS_S)
#define TEE_DMA_AXI_PDMA_GPSPI2_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI2_W_PMS_S  0

/** TEE_DMA_AXI_PDMA_GPSPI3_R_PMS_REG register
 *  AXI PDMA gpspi3 read permission control registers.
 */
#define TEE_DMA_AXI_PDMA_GPSPI3_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x184)
/** TEE_DMA_AXI_PDMA_GPSPI3_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA gpspi3 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_GPSPI3_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI3_R_PMS_M  (TEE_DMA_AXI_PDMA_GPSPI3_R_PMS_V << TEE_DMA_AXI_PDMA_GPSPI3_R_PMS_S)
#define TEE_DMA_AXI_PDMA_GPSPI3_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI3_R_PMS_S  0

/** TEE_DMA_AXI_PDMA_GPSPI3_W_PMS_REG register
 *  AXI PDMA gpspi3 write permission control registers.
 */
#define TEE_DMA_AXI_PDMA_GPSPI3_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x188)
/** TEE_DMA_AXI_PDMA_GPSPI3_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA gpspi3 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_GPSPI3_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI3_W_PMS_M  (TEE_DMA_AXI_PDMA_GPSPI3_W_PMS_V << TEE_DMA_AXI_PDMA_GPSPI3_W_PMS_S)
#define TEE_DMA_AXI_PDMA_GPSPI3_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_GPSPI3_W_PMS_S  0

/** TEE_DMA_AXI_PDMA_PARLIO_R_PMS_REG register
 *  AXI PDMA parl io read permission control registers.
 */
#define TEE_DMA_AXI_PDMA_PARLIO_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x18c)
/** TEE_DMA_AXI_PDMA_PARLIO_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA parl io read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_PARLIO_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_PARLIO_R_PMS_M  (TEE_DMA_AXI_PDMA_PARLIO_R_PMS_V << TEE_DMA_AXI_PDMA_PARLIO_R_PMS_S)
#define TEE_DMA_AXI_PDMA_PARLIO_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_PARLIO_R_PMS_S  0

/** TEE_DMA_AXI_PDMA_PARLIO_W_PMS_REG register
 *  AXI PDMA parl io write permission control registers.
 */
#define TEE_DMA_AXI_PDMA_PARLIO_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x190)
/** TEE_DMA_AXI_PDMA_PARLIO_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA parl io write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_PARLIO_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_PARLIO_W_PMS_M  (TEE_DMA_AXI_PDMA_PARLIO_W_PMS_V << TEE_DMA_AXI_PDMA_PARLIO_W_PMS_S)
#define TEE_DMA_AXI_PDMA_PARLIO_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_PARLIO_W_PMS_S  0

/** TEE_DMA_AXI_PDMA_AES_R_PMS_REG register
 *  AXI PDMA aes read permission control registers.
 */
#define TEE_DMA_AXI_PDMA_AES_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x194)
/** TEE_DMA_AXI_PDMA_AES_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA aes read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_AES_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_AES_R_PMS_M  (TEE_DMA_AXI_PDMA_AES_R_PMS_V << TEE_DMA_AXI_PDMA_AES_R_PMS_S)
#define TEE_DMA_AXI_PDMA_AES_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_AES_R_PMS_S  0

/** TEE_DMA_AXI_PDMA_AES_W_PMS_REG register
 *  AXI PDMA aes write permission control registers.
 */
#define TEE_DMA_AXI_PDMA_AES_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x198)
/** TEE_DMA_AXI_PDMA_AES_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA aes write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_AES_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_AES_W_PMS_M  (TEE_DMA_AXI_PDMA_AES_W_PMS_V << TEE_DMA_AXI_PDMA_AES_W_PMS_S)
#define TEE_DMA_AXI_PDMA_AES_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_AES_W_PMS_S  0

/** TEE_DMA_AXI_PDMA_SHA_R_PMS_REG register
 *  AXI PDMA sha read permission control registers.
 */
#define TEE_DMA_AXI_PDMA_SHA_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x19c)
/** TEE_DMA_AXI_PDMA_SHA_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA sha read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_SHA_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_SHA_R_PMS_M  (TEE_DMA_AXI_PDMA_SHA_R_PMS_V << TEE_DMA_AXI_PDMA_SHA_R_PMS_S)
#define TEE_DMA_AXI_PDMA_SHA_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_SHA_R_PMS_S  0

/** TEE_DMA_AXI_PDMA_SHA_W_PMS_REG register
 *  AXI PDMA sha write permission control registers.
 */
#define TEE_DMA_AXI_PDMA_SHA_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x1a0)
/** TEE_DMA_AXI_PDMA_SHA_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA sha write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_SHA_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_SHA_W_PMS_M  (TEE_DMA_AXI_PDMA_SHA_W_PMS_V << TEE_DMA_AXI_PDMA_SHA_W_PMS_S)
#define TEE_DMA_AXI_PDMA_SHA_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_SHA_W_PMS_S  0

/** TEE_DMA_DMA2D_JPEG_PMS_R_REG register
 *  DMA2D JPEG read permission control registers.
 */
#define TEE_DMA_DMA2D_JPEG_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1a4)
/** TEE_DMA_DMA2D_JPEG_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  DMA2D JPEG read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_DMA2D_JPEG_R_PMS    0xFFFFFFFFU
#define TEE_DMA_DMA2D_JPEG_R_PMS_M  (TEE_DMA_DMA2D_JPEG_R_PMS_V << TEE_DMA_DMA2D_JPEG_R_PMS_S)
#define TEE_DMA_DMA2D_JPEG_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_DMA2D_JPEG_R_PMS_S  0

/** TEE_DMA_DMA2D_JPEG_PMS_W_REG register
 *  DMA2D JPEG write permission control registers.
 */
#define TEE_DMA_DMA2D_JPEG_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1a8)
/** TEE_DMA_DMA2D_JPEG_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  DMA2D  JPEG write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_DMA2D_JPEG_W_PMS    0xFFFFFFFFU
#define TEE_DMA_DMA2D_JPEG_W_PMS_M  (TEE_DMA_DMA2D_JPEG_W_PMS_V << TEE_DMA_DMA2D_JPEG_W_PMS_S)
#define TEE_DMA_DMA2D_JPEG_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_DMA2D_JPEG_W_PMS_S  0

/** TEE_DMA_USB_PMS_R_REG register
 *  USB read permission control registers.
 */
#define TEE_DMA_USB_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1ac)
/** TEE_DMA_USB_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  USB read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_USB_R_PMS    0xFFFFFFFFU
#define TEE_DMA_USB_R_PMS_M  (TEE_DMA_USB_R_PMS_V << TEE_DMA_USB_R_PMS_S)
#define TEE_DMA_USB_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_USB_R_PMS_S  0

/** TEE_DMA_USB_PMS_W_REG register
 *  USB write permission control registers.
 */
#define TEE_DMA_USB_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1b0)
/** TEE_DMA_USB_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  USB write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_USB_W_PMS    0xFFFFFFFFU
#define TEE_DMA_USB_W_PMS_M  (TEE_DMA_USB_W_PMS_V << TEE_DMA_USB_W_PMS_S)
#define TEE_DMA_USB_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_USB_W_PMS_S  0

/** TEE_DMA_GMAC_PMS_R_REG register
 *  GMAC read permission control registers.
 */
#define TEE_DMA_GMAC_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1b4)
/** TEE_DMA_GMAC_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GMAC read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GMAC_R_PMS    0xFFFFFFFFU
#define TEE_DMA_GMAC_R_PMS_M  (TEE_DMA_GMAC_R_PMS_V << TEE_DMA_GMAC_R_PMS_S)
#define TEE_DMA_GMAC_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GMAC_R_PMS_S  0

/** TEE_DMA_GMAC_PMS_W_REG register
 *  GMAC write permission control registers.
 */
#define TEE_DMA_GMAC_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1b8)
/** TEE_DMA_GMAC_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  GMAC write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_GMAC_W_PMS    0xFFFFFFFFU
#define TEE_DMA_GMAC_W_PMS_M  (TEE_DMA_GMAC_W_PMS_V << TEE_DMA_GMAC_W_PMS_S)
#define TEE_DMA_GMAC_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_GMAC_W_PMS_S  0

/** TEE_DMA_SDMMC_PMS_R_REG register
 *  SDMMC read permission control registers.
 */
#define TEE_DMA_SDMMC_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1bc)
/** TEE_DMA_SDMMC_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  SDMMC read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_SDMMC_R_PMS    0xFFFFFFFFU
#define TEE_DMA_SDMMC_R_PMS_M  (TEE_DMA_SDMMC_R_PMS_V << TEE_DMA_SDMMC_R_PMS_S)
#define TEE_DMA_SDMMC_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_SDMMC_R_PMS_S  0

/** TEE_DMA_SDMMC_PMS_W_REG register
 *  SDMMC write permission control registers.
 */
#define TEE_DMA_SDMMC_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1c0)
/** TEE_DMA_SDMMC_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  SDMMC write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_SDMMC_W_PMS    0xFFFFFFFFU
#define TEE_DMA_SDMMC_W_PMS_M  (TEE_DMA_SDMMC_W_PMS_V << TEE_DMA_SDMMC_W_PMS_S)
#define TEE_DMA_SDMMC_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_SDMMC_W_PMS_S  0

/** TEE_DMA_USBOTG11_PMS_R_REG register
 *  USBOTG11 read permission control registers.
 */
#define TEE_DMA_USBOTG11_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1c4)
/** TEE_DMA_USBOTG11_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  USBOTG11 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_USBOTG11_R_PMS    0xFFFFFFFFU
#define TEE_DMA_USBOTG11_R_PMS_M  (TEE_DMA_USBOTG11_R_PMS_V << TEE_DMA_USBOTG11_R_PMS_S)
#define TEE_DMA_USBOTG11_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_USBOTG11_R_PMS_S  0

/** TEE_DMA_USBOTG11_PMS_W_REG register
 *  USBOTG11 write permission control registers.
 */
#define TEE_DMA_USBOTG11_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1c8)
/** TEE_DMA_USBOTG11_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  USBOTG11 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_USBOTG11_W_PMS    0xFFFFFFFFU
#define TEE_DMA_USBOTG11_W_PMS_M  (TEE_DMA_USBOTG11_W_PMS_V << TEE_DMA_USBOTG11_W_PMS_S)
#define TEE_DMA_USBOTG11_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_USBOTG11_W_PMS_S  0

/** TEE_DMA_TRACE0_PMS_R_REG register
 *  TRACE0 read permission control registers.
 */
#define TEE_DMA_TRACE0_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1cc)
/** TEE_DMA_TRACE0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  TRACE0 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_TRACE0_R_PMS    0xFFFFFFFFU
#define TEE_DMA_TRACE0_R_PMS_M  (TEE_DMA_TRACE0_R_PMS_V << TEE_DMA_TRACE0_R_PMS_S)
#define TEE_DMA_TRACE0_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_TRACE0_R_PMS_S  0

/** TEE_DMA_TRACE0_PMS_W_REG register
 *  TRACE0 write permission control registers.
 */
#define TEE_DMA_TRACE0_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1d0)
/** TEE_DMA_TRACE0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  TRACE0 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_TRACE0_W_PMS    0xFFFFFFFFU
#define TEE_DMA_TRACE0_W_PMS_M  (TEE_DMA_TRACE0_W_PMS_V << TEE_DMA_TRACE0_W_PMS_S)
#define TEE_DMA_TRACE0_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_TRACE0_W_PMS_S  0

/** TEE_DMA_TRACE1_PMS_R_REG register
 *  TRACE1 read permission control registers.
 */
#define TEE_DMA_TRACE1_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1d4)
/** TEE_DMA_TRACE1_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  TRACE1 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_TRACE1_R_PMS    0xFFFFFFFFU
#define TEE_DMA_TRACE1_R_PMS_M  (TEE_DMA_TRACE1_R_PMS_V << TEE_DMA_TRACE1_R_PMS_S)
#define TEE_DMA_TRACE1_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_TRACE1_R_PMS_S  0

/** TEE_DMA_TRACE1_PMS_W_REG register
 *  TRACE1 write permission control registers.
 */
#define TEE_DMA_TRACE1_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1d8)
/** TEE_DMA_TRACE1_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  TRACE1 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_TRACE1_W_PMS    0xFFFFFFFFU
#define TEE_DMA_TRACE1_W_PMS_M  (TEE_DMA_TRACE1_W_PMS_V << TEE_DMA_TRACE1_W_PMS_S)
#define TEE_DMA_TRACE1_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_TRACE1_W_PMS_S  0

/** TEE_DMA_L2MEM_MON_PMS_R_REG register
 *  L2MEM MON read permission control registers.
 */
#define TEE_DMA_L2MEM_MON_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1dc)
/** TEE_DMA_L2MEM_MON_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  L2MEM MON read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_L2MEM_MON_R_PMS    0xFFFFFFFFU
#define TEE_DMA_L2MEM_MON_R_PMS_M  (TEE_DMA_L2MEM_MON_R_PMS_V << TEE_DMA_L2MEM_MON_R_PMS_S)
#define TEE_DMA_L2MEM_MON_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_L2MEM_MON_R_PMS_S  0

/** TEE_DMA_L2MEM_MON_PMS_W_REG register
 *  L2MEM MON write permission control registers.
 */
#define TEE_DMA_L2MEM_MON_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1e0)
/** TEE_DMA_L2MEM_MON_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  L2MEM MON write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_L2MEM_MON_W_PMS    0xFFFFFFFFU
#define TEE_DMA_L2MEM_MON_W_PMS_M  (TEE_DMA_L2MEM_MON_W_PMS_V << TEE_DMA_L2MEM_MON_W_PMS_S)
#define TEE_DMA_L2MEM_MON_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_L2MEM_MON_W_PMS_S  0

/** TEE_DMA_TCM_MON_PMS_R_REG register
 *  TCM MON read permission control registers.
 */
#define TEE_DMA_TCM_MON_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1e4)
/** TEE_DMA_TCM_MON_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  TCM MON read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_TCM_MON_R_PMS    0xFFFFFFFFU
#define TEE_DMA_TCM_MON_R_PMS_M  (TEE_DMA_TCM_MON_R_PMS_V << TEE_DMA_TCM_MON_R_PMS_S)
#define TEE_DMA_TCM_MON_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_TCM_MON_R_PMS_S  0

/** TEE_DMA_TCM_MON_PMS_W_REG register
 *  TCM MON write permission control registers.
 */
#define TEE_DMA_TCM_MON_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1e8)
/** TEE_DMA_TCM_MON_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  TCM MON write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_TCM_MON_W_PMS    0xFFFFFFFFU
#define TEE_DMA_TCM_MON_W_PMS_M  (TEE_DMA_TCM_MON_W_PMS_V << TEE_DMA_TCM_MON_W_PMS_S)
#define TEE_DMA_TCM_MON_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_TCM_MON_W_PMS_S  0

/** TEE_DMA_REGDMA_PMS_R_REG register
 *  REGDMA read permission control registers.
 */
#define TEE_DMA_REGDMA_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1ec)
/** TEE_DMA_REGDMA_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  REGDMA read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_REGDMA_R_PMS    0xFFFFFFFFU
#define TEE_DMA_REGDMA_R_PMS_M  (TEE_DMA_REGDMA_R_PMS_V << TEE_DMA_REGDMA_R_PMS_S)
#define TEE_DMA_REGDMA_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_REGDMA_R_PMS_S  0

/** TEE_DMA_REGDMA_PMS_W_REG register
 *  REGDMA write permission control registers.
 */
#define TEE_DMA_REGDMA_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x1f0)
/** TEE_DMA_REGDMA_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  REGDMA write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_REGDMA_W_PMS    0xFFFFFFFFU
#define TEE_DMA_REGDMA_W_PMS_M  (TEE_DMA_REGDMA_W_PMS_V << TEE_DMA_REGDMA_W_PMS_S)
#define TEE_DMA_REGDMA_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_REGDMA_W_PMS_S  0

/** TEE_DMA_H264_PMS_R_REG register
 *  H264 read permission control registers.
 */
#define TEE_DMA_H264_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x1fc)
/** TEE_DMA_H264_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  H264 read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_H264_R_PMS    0xFFFFFFFFU
#define TEE_DMA_H264_R_PMS_M  (TEE_DMA_H264_R_PMS_V << TEE_DMA_H264_R_PMS_S)
#define TEE_DMA_H264_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_H264_R_PMS_S  0

/** TEE_DMA_H264_PMS_W_REG register
 *  H264 write permission control registers.
 */
#define TEE_DMA_H264_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x200)
/** TEE_DMA_H264_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  H264 write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_H264_W_PMS    0xFFFFFFFFU
#define TEE_DMA_H264_W_PMS_M  (TEE_DMA_H264_W_PMS_V << TEE_DMA_H264_W_PMS_S)
#define TEE_DMA_H264_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_H264_W_PMS_S  0

/** TEE_DMA_DMA2D_PPA_PMS_R_REG register
 *  DMA2D PPA read permission control registers.
 */
#define TEE_DMA_DMA2D_PPA_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x204)
/** TEE_DMA_DMA2D_PPA_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  DMA2D PPA  read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_DMA2D_PPA_R_PMS    0xFFFFFFFFU
#define TEE_DMA_DMA2D_PPA_R_PMS_M  (TEE_DMA_DMA2D_PPA_R_PMS_V << TEE_DMA_DMA2D_PPA_R_PMS_S)
#define TEE_DMA_DMA2D_PPA_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_DMA2D_PPA_R_PMS_S  0

/** TEE_DMA_DMA2D_PPA_PMS_W_REG register
 *  DMA2D PPA  write permission control registers.
 */
#define TEE_DMA_DMA2D_PPA_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x208)
/** TEE_DMA_DMA2D_PPA_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  DMA2D  PPA write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_DMA2D_PPA_W_PMS    0xFFFFFFFFU
#define TEE_DMA_DMA2D_PPA_W_PMS_M  (TEE_DMA_DMA2D_PPA_W_PMS_V << TEE_DMA_DMA2D_PPA_W_PMS_S)
#define TEE_DMA_DMA2D_PPA_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_DMA2D_PPA_W_PMS_S  0

/** TEE_DMA_DMA2D_DUMMY_PMS_R_REG register
 *  DMA2D dummy read permission control registers.
 */
#define TEE_DMA_DMA2D_DUMMY_PMS_R_REG (DR_REG_TEE_DMA_BASE + 0x20c)
/** TEE_DMA_DMA2D_DUMMY_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  DMA2D dummy  read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_DMA2D_DUMMY_R_PMS    0xFFFFFFFFU
#define TEE_DMA_DMA2D_DUMMY_R_PMS_M  (TEE_DMA_DMA2D_DUMMY_R_PMS_V << TEE_DMA_DMA2D_DUMMY_R_PMS_S)
#define TEE_DMA_DMA2D_DUMMY_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_DMA2D_DUMMY_R_PMS_S  0

/** TEE_DMA_DMA2D_DUMMY_PMS_W_REG register
 *  DMA2D dummy  write permission control registers.
 */
#define TEE_DMA_DMA2D_DUMMY_PMS_W_REG (DR_REG_TEE_DMA_BASE + 0x210)
/** TEE_DMA_DMA2D_DUMMY_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  DMA2D dummy  write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_DMA2D_DUMMY_W_PMS    0xFFFFFFFFU
#define TEE_DMA_DMA2D_DUMMY_W_PMS_M  (TEE_DMA_DMA2D_DUMMY_W_PMS_V << TEE_DMA_DMA2D_DUMMY_W_PMS_S)
#define TEE_DMA_DMA2D_DUMMY_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_DMA2D_DUMMY_W_PMS_S  0

/** TEE_DMA_AHB_PDMA_DUMMY_R_PMS_REG register
 *  AHB PDMA dummy read permission control registers.
 */
#define TEE_DMA_AHB_PDMA_DUMMY_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x214)
/** TEE_DMA_AHB_PDMA_DUMMY_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA dummy read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_DUMMY_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_DUMMY_R_PMS_M  (TEE_DMA_AHB_PDMA_DUMMY_R_PMS_V << TEE_DMA_AHB_PDMA_DUMMY_R_PMS_S)
#define TEE_DMA_AHB_PDMA_DUMMY_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_DUMMY_R_PMS_S  0

/** TEE_DMA_AHB_PDMA_DUMMY_W_PMS_REG register
 *  AHB PDMA dummy write permission control registers.
 */
#define TEE_DMA_AHB_PDMA_DUMMY_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x218)
/** TEE_DMA_AHB_PDMA_DUMMY_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AHB PDMA dummy write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AHB_PDMA_DUMMY_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_DUMMY_W_PMS_M  (TEE_DMA_AHB_PDMA_DUMMY_W_PMS_V << TEE_DMA_AHB_PDMA_DUMMY_W_PMS_S)
#define TEE_DMA_AHB_PDMA_DUMMY_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AHB_PDMA_DUMMY_W_PMS_S  0

/** TEE_DMA_AXI_PDMA_DUMMY_R_PMS_REG register
 *  AXI PDMA dummy read permission control registers.
 */
#define TEE_DMA_AXI_PDMA_DUMMY_R_PMS_REG (DR_REG_TEE_DMA_BASE + 0x21c)
/** TEE_DMA_AXI_PDMA_DUMMY_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA dummy read permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_DUMMY_R_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_DUMMY_R_PMS_M  (TEE_DMA_AXI_PDMA_DUMMY_R_PMS_V << TEE_DMA_AXI_PDMA_DUMMY_R_PMS_S)
#define TEE_DMA_AXI_PDMA_DUMMY_R_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_DUMMY_R_PMS_S  0

/** TEE_DMA_AXI_PDMA_DUMMY_W_PMS_REG register
 *  AXI PDMA dummy write permission control registers.
 */
#define TEE_DMA_AXI_PDMA_DUMMY_W_PMS_REG (DR_REG_TEE_DMA_BASE + 0x220)
/** TEE_DMA_AXI_PDMA_DUMMY_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI PDMA dummy write permission control, each bit corresponds to a region.
 */
#define TEE_DMA_AXI_PDMA_DUMMY_W_PMS    0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_DUMMY_W_PMS_M  (TEE_DMA_AXI_PDMA_DUMMY_W_PMS_V << TEE_DMA_AXI_PDMA_DUMMY_W_PMS_S)
#define TEE_DMA_AXI_PDMA_DUMMY_W_PMS_V  0xFFFFFFFFU
#define TEE_DMA_AXI_PDMA_DUMMY_W_PMS_S  0

#ifdef __cplusplus
}
#endif
