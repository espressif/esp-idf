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

/** PMS_DMA_DATE_REG register
 *  Version control register
 */
#define PMS_DMA_DATE_REG (DR_REG_DMA_PMS_BASE + 0x0)
/** PMS_DMA_DATE : R/W; bitpos: [31:0]; default: 539165460;
 *  Version control register.
 */
#define PMS_DMA_DATE    0xFFFFFFFFU
#define PMS_DMA_DATE_M  (PMS_DMA_DATE_V << PMS_DMA_DATE_S)
#define PMS_DMA_DATE_V  0xFFFFFFFFU
#define PMS_DMA_DATE_S  0

/** PMS_DMA_CLK_EN_REG register
 *  Clock gating register
 */
#define PMS_DMA_CLK_EN_REG (DR_REG_DMA_PMS_BASE + 0x4)
/** PMS_DMA_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether to keep the clock always on.
 *  0: Enable automatic clock gating.
 *  1: Keep the clock always on.
 */
#define PMS_DMA_CLK_EN    (BIT(0))
#define PMS_DMA_CLK_EN_M  (PMS_DMA_CLK_EN_V << PMS_DMA_CLK_EN_S)
#define PMS_DMA_CLK_EN_V  0x00000001U
#define PMS_DMA_CLK_EN_S  0

/** PMS_DMA_REGION0_LOW_REG register
 *  Region0 start address configuration register
 */
#define PMS_DMA_REGION0_LOW_REG (DR_REG_DMA_PMS_BASE + 0x8)
/** PMS_DMA_REGION0_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region0.
 */
#define PMS_DMA_REGION0_LOW    0x000FFFFFU
#define PMS_DMA_REGION0_LOW_M  (PMS_DMA_REGION0_LOW_V << PMS_DMA_REGION0_LOW_S)
#define PMS_DMA_REGION0_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION0_LOW_S  12

/** PMS_DMA_REGION0_HIGH_REG register
 *  Region0 end address configuration register
 */
#define PMS_DMA_REGION0_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xc)
/** PMS_DMA_REGION0_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region0.
 */
#define PMS_DMA_REGION0_HIGH    0x000FFFFFU
#define PMS_DMA_REGION0_HIGH_M  (PMS_DMA_REGION0_HIGH_V << PMS_DMA_REGION0_HIGH_S)
#define PMS_DMA_REGION0_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION0_HIGH_S  12

/** PMS_DMA_REGION1_LOW_REG register
 *  Region1 start address configuration register
 */
#define PMS_DMA_REGION1_LOW_REG (DR_REG_DMA_PMS_BASE + 0x10)
/** PMS_DMA_REGION1_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region1.
 */
#define PMS_DMA_REGION1_LOW    0x000FFFFFU
#define PMS_DMA_REGION1_LOW_M  (PMS_DMA_REGION1_LOW_V << PMS_DMA_REGION1_LOW_S)
#define PMS_DMA_REGION1_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION1_LOW_S  12

/** PMS_DMA_REGION1_HIGH_REG register
 *  Region1 end address configuration register
 */
#define PMS_DMA_REGION1_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x14)
/** PMS_DMA_REGION1_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region1.
 */
#define PMS_DMA_REGION1_HIGH    0x000FFFFFU
#define PMS_DMA_REGION1_HIGH_M  (PMS_DMA_REGION1_HIGH_V << PMS_DMA_REGION1_HIGH_S)
#define PMS_DMA_REGION1_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION1_HIGH_S  12

/** PMS_DMA_REGION2_LOW_REG register
 *  Region2 start address configuration register
 */
#define PMS_DMA_REGION2_LOW_REG (DR_REG_DMA_PMS_BASE + 0x18)
/** PMS_DMA_REGION2_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region2.
 */
#define PMS_DMA_REGION2_LOW    0x000FFFFFU
#define PMS_DMA_REGION2_LOW_M  (PMS_DMA_REGION2_LOW_V << PMS_DMA_REGION2_LOW_S)
#define PMS_DMA_REGION2_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION2_LOW_S  12

/** PMS_DMA_REGION2_HIGH_REG register
 *  Region2 end address configuration register
 */
#define PMS_DMA_REGION2_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x1c)
/** PMS_DMA_REGION2_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region2.
 */
#define PMS_DMA_REGION2_HIGH    0x000FFFFFU
#define PMS_DMA_REGION2_HIGH_M  (PMS_DMA_REGION2_HIGH_V << PMS_DMA_REGION2_HIGH_S)
#define PMS_DMA_REGION2_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION2_HIGH_S  12

/** PMS_DMA_REGION3_LOW_REG register
 *  Region3 start address configuration register
 */
#define PMS_DMA_REGION3_LOW_REG (DR_REG_DMA_PMS_BASE + 0x20)
/** PMS_DMA_REGION3_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region3.
 */
#define PMS_DMA_REGION3_LOW    0x000FFFFFU
#define PMS_DMA_REGION3_LOW_M  (PMS_DMA_REGION3_LOW_V << PMS_DMA_REGION3_LOW_S)
#define PMS_DMA_REGION3_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION3_LOW_S  12

/** PMS_DMA_REGION3_HIGH_REG register
 *  Region3 end address configuration register
 */
#define PMS_DMA_REGION3_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x24)
/** PMS_DMA_REGION3_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region3.
 */
#define PMS_DMA_REGION3_HIGH    0x000FFFFFU
#define PMS_DMA_REGION3_HIGH_M  (PMS_DMA_REGION3_HIGH_V << PMS_DMA_REGION3_HIGH_S)
#define PMS_DMA_REGION3_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION3_HIGH_S  12

/** PMS_DMA_REGION4_LOW_REG register
 *  Region4 start address configuration register
 */
#define PMS_DMA_REGION4_LOW_REG (DR_REG_DMA_PMS_BASE + 0x28)
/** PMS_DMA_REGION4_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region4.
 */
#define PMS_DMA_REGION4_LOW    0x000FFFFFU
#define PMS_DMA_REGION4_LOW_M  (PMS_DMA_REGION4_LOW_V << PMS_DMA_REGION4_LOW_S)
#define PMS_DMA_REGION4_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION4_LOW_S  12

/** PMS_DMA_REGION4_HIGH_REG register
 *  Region4 end address configuration register
 */
#define PMS_DMA_REGION4_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x2c)
/** PMS_DMA_REGION4_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region4.
 */
#define PMS_DMA_REGION4_HIGH    0x000FFFFFU
#define PMS_DMA_REGION4_HIGH_M  (PMS_DMA_REGION4_HIGH_V << PMS_DMA_REGION4_HIGH_S)
#define PMS_DMA_REGION4_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION4_HIGH_S  12

/** PMS_DMA_REGION5_LOW_REG register
 *  Region5 start address configuration register
 */
#define PMS_DMA_REGION5_LOW_REG (DR_REG_DMA_PMS_BASE + 0x30)
/** PMS_DMA_REGION5_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region5.
 */
#define PMS_DMA_REGION5_LOW    0x000FFFFFU
#define PMS_DMA_REGION5_LOW_M  (PMS_DMA_REGION5_LOW_V << PMS_DMA_REGION5_LOW_S)
#define PMS_DMA_REGION5_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION5_LOW_S  12

/** PMS_DMA_REGION5_HIGH_REG register
 *  Region5 end address configuration register
 */
#define PMS_DMA_REGION5_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x34)
/** PMS_DMA_REGION5_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region5.
 */
#define PMS_DMA_REGION5_HIGH    0x000FFFFFU
#define PMS_DMA_REGION5_HIGH_M  (PMS_DMA_REGION5_HIGH_V << PMS_DMA_REGION5_HIGH_S)
#define PMS_DMA_REGION5_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION5_HIGH_S  12

/** PMS_DMA_REGION6_LOW_REG register
 *  Region6 start address configuration register
 */
#define PMS_DMA_REGION6_LOW_REG (DR_REG_DMA_PMS_BASE + 0x38)
/** PMS_DMA_REGION6_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region6.
 */
#define PMS_DMA_REGION6_LOW    0x000FFFFFU
#define PMS_DMA_REGION6_LOW_M  (PMS_DMA_REGION6_LOW_V << PMS_DMA_REGION6_LOW_S)
#define PMS_DMA_REGION6_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION6_LOW_S  12

/** PMS_DMA_REGION6_HIGH_REG register
 *  Region6 end address configuration register
 */
#define PMS_DMA_REGION6_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x3c)
/** PMS_DMA_REGION6_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region6.
 */
#define PMS_DMA_REGION6_HIGH    0x000FFFFFU
#define PMS_DMA_REGION6_HIGH_M  (PMS_DMA_REGION6_HIGH_V << PMS_DMA_REGION6_HIGH_S)
#define PMS_DMA_REGION6_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION6_HIGH_S  12

/** PMS_DMA_REGION7_LOW_REG register
 *  Region7 start address configuration register
 */
#define PMS_DMA_REGION7_LOW_REG (DR_REG_DMA_PMS_BASE + 0x40)
/** PMS_DMA_REGION7_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region7.
 */
#define PMS_DMA_REGION7_LOW    0x000FFFFFU
#define PMS_DMA_REGION7_LOW_M  (PMS_DMA_REGION7_LOW_V << PMS_DMA_REGION7_LOW_S)
#define PMS_DMA_REGION7_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION7_LOW_S  12

/** PMS_DMA_REGION7_HIGH_REG register
 *  Region7 end address configuration register
 */
#define PMS_DMA_REGION7_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x44)
/** PMS_DMA_REGION7_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region7.
 */
#define PMS_DMA_REGION7_HIGH    0x000FFFFFU
#define PMS_DMA_REGION7_HIGH_M  (PMS_DMA_REGION7_HIGH_V << PMS_DMA_REGION7_HIGH_S)
#define PMS_DMA_REGION7_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION7_HIGH_S  12

/** PMS_DMA_REGION8_LOW_REG register
 *  Region8 start address configuration register
 */
#define PMS_DMA_REGION8_LOW_REG (DR_REG_DMA_PMS_BASE + 0x48)
/** PMS_DMA_REGION8_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region8.
 */
#define PMS_DMA_REGION8_LOW    0x000FFFFFU
#define PMS_DMA_REGION8_LOW_M  (PMS_DMA_REGION8_LOW_V << PMS_DMA_REGION8_LOW_S)
#define PMS_DMA_REGION8_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION8_LOW_S  12

/** PMS_DMA_REGION8_HIGH_REG register
 *  Region8 end address configuration register
 */
#define PMS_DMA_REGION8_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x4c)
/** PMS_DMA_REGION8_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region8.
 */
#define PMS_DMA_REGION8_HIGH    0x000FFFFFU
#define PMS_DMA_REGION8_HIGH_M  (PMS_DMA_REGION8_HIGH_V << PMS_DMA_REGION8_HIGH_S)
#define PMS_DMA_REGION8_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION8_HIGH_S  12

/** PMS_DMA_REGION9_LOW_REG register
 *  Region9 start address configuration register
 */
#define PMS_DMA_REGION9_LOW_REG (DR_REG_DMA_PMS_BASE + 0x50)
/** PMS_DMA_REGION9_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region9.
 */
#define PMS_DMA_REGION9_LOW    0x000FFFFFU
#define PMS_DMA_REGION9_LOW_M  (PMS_DMA_REGION9_LOW_V << PMS_DMA_REGION9_LOW_S)
#define PMS_DMA_REGION9_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION9_LOW_S  12

/** PMS_DMA_REGION9_HIGH_REG register
 *  Region9 end address configuration register
 */
#define PMS_DMA_REGION9_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x54)
/** PMS_DMA_REGION9_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region9.
 */
#define PMS_DMA_REGION9_HIGH    0x000FFFFFU
#define PMS_DMA_REGION9_HIGH_M  (PMS_DMA_REGION9_HIGH_V << PMS_DMA_REGION9_HIGH_S)
#define PMS_DMA_REGION9_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION9_HIGH_S  12

/** PMS_DMA_REGION10_LOW_REG register
 *  Region10 start address configuration register
 */
#define PMS_DMA_REGION10_LOW_REG (DR_REG_DMA_PMS_BASE + 0x58)
/** PMS_DMA_REGION10_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region10.
 */
#define PMS_DMA_REGION10_LOW    0x000FFFFFU
#define PMS_DMA_REGION10_LOW_M  (PMS_DMA_REGION10_LOW_V << PMS_DMA_REGION10_LOW_S)
#define PMS_DMA_REGION10_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION10_LOW_S  12

/** PMS_DMA_REGION10_HIGH_REG register
 *  Region10 end address configuration register
 */
#define PMS_DMA_REGION10_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x5c)
/** PMS_DMA_REGION10_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region10.
 */
#define PMS_DMA_REGION10_HIGH    0x000FFFFFU
#define PMS_DMA_REGION10_HIGH_M  (PMS_DMA_REGION10_HIGH_V << PMS_DMA_REGION10_HIGH_S)
#define PMS_DMA_REGION10_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION10_HIGH_S  12

/** PMS_DMA_REGION11_LOW_REG register
 *  Region11 start address configuration register
 */
#define PMS_DMA_REGION11_LOW_REG (DR_REG_DMA_PMS_BASE + 0x60)
/** PMS_DMA_REGION11_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region11.
 */
#define PMS_DMA_REGION11_LOW    0x000FFFFFU
#define PMS_DMA_REGION11_LOW_M  (PMS_DMA_REGION11_LOW_V << PMS_DMA_REGION11_LOW_S)
#define PMS_DMA_REGION11_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION11_LOW_S  12

/** PMS_DMA_REGION11_HIGH_REG register
 *  Region11 end address configuration register
 */
#define PMS_DMA_REGION11_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x64)
/** PMS_DMA_REGION11_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region11.
 */
#define PMS_DMA_REGION11_HIGH    0x000FFFFFU
#define PMS_DMA_REGION11_HIGH_M  (PMS_DMA_REGION11_HIGH_V << PMS_DMA_REGION11_HIGH_S)
#define PMS_DMA_REGION11_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION11_HIGH_S  12

/** PMS_DMA_REGION12_LOW_REG register
 *  Region12 start address configuration register
 */
#define PMS_DMA_REGION12_LOW_REG (DR_REG_DMA_PMS_BASE + 0x68)
/** PMS_DMA_REGION12_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region12.
 */
#define PMS_DMA_REGION12_LOW    0x000FFFFFU
#define PMS_DMA_REGION12_LOW_M  (PMS_DMA_REGION12_LOW_V << PMS_DMA_REGION12_LOW_S)
#define PMS_DMA_REGION12_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION12_LOW_S  12

/** PMS_DMA_REGION12_HIGH_REG register
 *  Region12 end address configuration register
 */
#define PMS_DMA_REGION12_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x6c)
/** PMS_DMA_REGION12_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region12.
 */
#define PMS_DMA_REGION12_HIGH    0x000FFFFFU
#define PMS_DMA_REGION12_HIGH_M  (PMS_DMA_REGION12_HIGH_V << PMS_DMA_REGION12_HIGH_S)
#define PMS_DMA_REGION12_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION12_HIGH_S  12

/** PMS_DMA_REGION13_LOW_REG register
 *  Region13 start address configuration register
 */
#define PMS_DMA_REGION13_LOW_REG (DR_REG_DMA_PMS_BASE + 0x70)
/** PMS_DMA_REGION13_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region13.
 */
#define PMS_DMA_REGION13_LOW    0x000FFFFFU
#define PMS_DMA_REGION13_LOW_M  (PMS_DMA_REGION13_LOW_V << PMS_DMA_REGION13_LOW_S)
#define PMS_DMA_REGION13_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION13_LOW_S  12

/** PMS_DMA_REGION13_HIGH_REG register
 *  Region13 end address configuration register
 */
#define PMS_DMA_REGION13_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x74)
/** PMS_DMA_REGION13_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region13.
 */
#define PMS_DMA_REGION13_HIGH    0x000FFFFFU
#define PMS_DMA_REGION13_HIGH_M  (PMS_DMA_REGION13_HIGH_V << PMS_DMA_REGION13_HIGH_S)
#define PMS_DMA_REGION13_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION13_HIGH_S  12

/** PMS_DMA_REGION14_LOW_REG register
 *  Region14 start address configuration register
 */
#define PMS_DMA_REGION14_LOW_REG (DR_REG_DMA_PMS_BASE + 0x78)
/** PMS_DMA_REGION14_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region14.
 */
#define PMS_DMA_REGION14_LOW    0x000FFFFFU
#define PMS_DMA_REGION14_LOW_M  (PMS_DMA_REGION14_LOW_V << PMS_DMA_REGION14_LOW_S)
#define PMS_DMA_REGION14_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION14_LOW_S  12

/** PMS_DMA_REGION14_HIGH_REG register
 *  Region14 end address configuration register
 */
#define PMS_DMA_REGION14_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x7c)
/** PMS_DMA_REGION14_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region14.
 */
#define PMS_DMA_REGION14_HIGH    0x000FFFFFU
#define PMS_DMA_REGION14_HIGH_M  (PMS_DMA_REGION14_HIGH_V << PMS_DMA_REGION14_HIGH_S)
#define PMS_DMA_REGION14_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION14_HIGH_S  12

/** PMS_DMA_REGION15_LOW_REG register
 *  Region15 start address configuration register
 */
#define PMS_DMA_REGION15_LOW_REG (DR_REG_DMA_PMS_BASE + 0x80)
/** PMS_DMA_REGION15_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region15.
 */
#define PMS_DMA_REGION15_LOW    0x000FFFFFU
#define PMS_DMA_REGION15_LOW_M  (PMS_DMA_REGION15_LOW_V << PMS_DMA_REGION15_LOW_S)
#define PMS_DMA_REGION15_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION15_LOW_S  12

/** PMS_DMA_REGION15_HIGH_REG register
 *  Region15 end address configuration register
 */
#define PMS_DMA_REGION15_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x84)
/** PMS_DMA_REGION15_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region15.
 */
#define PMS_DMA_REGION15_HIGH    0x000FFFFFU
#define PMS_DMA_REGION15_HIGH_M  (PMS_DMA_REGION15_HIGH_V << PMS_DMA_REGION15_HIGH_S)
#define PMS_DMA_REGION15_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION15_HIGH_S  12

/** PMS_DMA_REGION16_LOW_REG register
 *  Region16 start address configuration register
 */
#define PMS_DMA_REGION16_LOW_REG (DR_REG_DMA_PMS_BASE + 0x88)
/** PMS_DMA_REGION16_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region16.
 */
#define PMS_DMA_REGION16_LOW    0x000FFFFFU
#define PMS_DMA_REGION16_LOW_M  (PMS_DMA_REGION16_LOW_V << PMS_DMA_REGION16_LOW_S)
#define PMS_DMA_REGION16_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION16_LOW_S  12

/** PMS_DMA_REGION16_HIGH_REG register
 *  Region16 end address configuration register
 */
#define PMS_DMA_REGION16_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x8c)
/** PMS_DMA_REGION16_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region16.
 */
#define PMS_DMA_REGION16_HIGH    0x000FFFFFU
#define PMS_DMA_REGION16_HIGH_M  (PMS_DMA_REGION16_HIGH_V << PMS_DMA_REGION16_HIGH_S)
#define PMS_DMA_REGION16_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION16_HIGH_S  12

/** PMS_DMA_REGION17_LOW_REG register
 *  Region17 start address configuration register
 */
#define PMS_DMA_REGION17_LOW_REG (DR_REG_DMA_PMS_BASE + 0x90)
/** PMS_DMA_REGION17_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region17.
 */
#define PMS_DMA_REGION17_LOW    0x000FFFFFU
#define PMS_DMA_REGION17_LOW_M  (PMS_DMA_REGION17_LOW_V << PMS_DMA_REGION17_LOW_S)
#define PMS_DMA_REGION17_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION17_LOW_S  12

/** PMS_DMA_REGION17_HIGH_REG register
 *  Region17 end address configuration register
 */
#define PMS_DMA_REGION17_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x94)
/** PMS_DMA_REGION17_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region17.
 */
#define PMS_DMA_REGION17_HIGH    0x000FFFFFU
#define PMS_DMA_REGION17_HIGH_M  (PMS_DMA_REGION17_HIGH_V << PMS_DMA_REGION17_HIGH_S)
#define PMS_DMA_REGION17_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION17_HIGH_S  12

/** PMS_DMA_REGION18_LOW_REG register
 *  Region18 start address configuration register
 */
#define PMS_DMA_REGION18_LOW_REG (DR_REG_DMA_PMS_BASE + 0x98)
/** PMS_DMA_REGION18_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region18.
 */
#define PMS_DMA_REGION18_LOW    0x000FFFFFU
#define PMS_DMA_REGION18_LOW_M  (PMS_DMA_REGION18_LOW_V << PMS_DMA_REGION18_LOW_S)
#define PMS_DMA_REGION18_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION18_LOW_S  12

/** PMS_DMA_REGION18_HIGH_REG register
 *  Region18 end address configuration register
 */
#define PMS_DMA_REGION18_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x9c)
/** PMS_DMA_REGION18_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region18.
 */
#define PMS_DMA_REGION18_HIGH    0x000FFFFFU
#define PMS_DMA_REGION18_HIGH_M  (PMS_DMA_REGION18_HIGH_V << PMS_DMA_REGION18_HIGH_S)
#define PMS_DMA_REGION18_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION18_HIGH_S  12

/** PMS_DMA_REGION19_LOW_REG register
 *  Region19 start address configuration register
 */
#define PMS_DMA_REGION19_LOW_REG (DR_REG_DMA_PMS_BASE + 0xa0)
/** PMS_DMA_REGION19_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region19.
 */
#define PMS_DMA_REGION19_LOW    0x000FFFFFU
#define PMS_DMA_REGION19_LOW_M  (PMS_DMA_REGION19_LOW_V << PMS_DMA_REGION19_LOW_S)
#define PMS_DMA_REGION19_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION19_LOW_S  12

/** PMS_DMA_REGION19_HIGH_REG register
 *  Region19 end address configuration register
 */
#define PMS_DMA_REGION19_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xa4)
/** PMS_DMA_REGION19_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region19.
 */
#define PMS_DMA_REGION19_HIGH    0x000FFFFFU
#define PMS_DMA_REGION19_HIGH_M  (PMS_DMA_REGION19_HIGH_V << PMS_DMA_REGION19_HIGH_S)
#define PMS_DMA_REGION19_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION19_HIGH_S  12

/** PMS_DMA_REGION20_LOW_REG register
 *  Region20 start address configuration register
 */
#define PMS_DMA_REGION20_LOW_REG (DR_REG_DMA_PMS_BASE + 0xa8)
/** PMS_DMA_REGION20_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region20.
 */
#define PMS_DMA_REGION20_LOW    0x000FFFFFU
#define PMS_DMA_REGION20_LOW_M  (PMS_DMA_REGION20_LOW_V << PMS_DMA_REGION20_LOW_S)
#define PMS_DMA_REGION20_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION20_LOW_S  12

/** PMS_DMA_REGION20_HIGH_REG register
 *  Region20 end address configuration register
 */
#define PMS_DMA_REGION20_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xac)
/** PMS_DMA_REGION20_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region20.
 */
#define PMS_DMA_REGION20_HIGH    0x000FFFFFU
#define PMS_DMA_REGION20_HIGH_M  (PMS_DMA_REGION20_HIGH_V << PMS_DMA_REGION20_HIGH_S)
#define PMS_DMA_REGION20_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION20_HIGH_S  12

/** PMS_DMA_REGION21_LOW_REG register
 *  Region21 start address configuration register
 */
#define PMS_DMA_REGION21_LOW_REG (DR_REG_DMA_PMS_BASE + 0xb0)
/** PMS_DMA_REGION21_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region21.
 */
#define PMS_DMA_REGION21_LOW    0x000FFFFFU
#define PMS_DMA_REGION21_LOW_M  (PMS_DMA_REGION21_LOW_V << PMS_DMA_REGION21_LOW_S)
#define PMS_DMA_REGION21_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION21_LOW_S  12

/** PMS_DMA_REGION21_HIGH_REG register
 *  Region21 end address configuration register
 */
#define PMS_DMA_REGION21_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xb4)
/** PMS_DMA_REGION21_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region21.
 */
#define PMS_DMA_REGION21_HIGH    0x000FFFFFU
#define PMS_DMA_REGION21_HIGH_M  (PMS_DMA_REGION21_HIGH_V << PMS_DMA_REGION21_HIGH_S)
#define PMS_DMA_REGION21_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION21_HIGH_S  12

/** PMS_DMA_REGION22_LOW_REG register
 *  Region22 start address configuration register
 */
#define PMS_DMA_REGION22_LOW_REG (DR_REG_DMA_PMS_BASE + 0xb8)
/** PMS_DMA_REGION22_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region22.
 */
#define PMS_DMA_REGION22_LOW    0x000FFFFFU
#define PMS_DMA_REGION22_LOW_M  (PMS_DMA_REGION22_LOW_V << PMS_DMA_REGION22_LOW_S)
#define PMS_DMA_REGION22_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION22_LOW_S  12

/** PMS_DMA_REGION22_HIGH_REG register
 *  Region22 end address configuration register
 */
#define PMS_DMA_REGION22_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xbc)
/** PMS_DMA_REGION22_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region22.
 */
#define PMS_DMA_REGION22_HIGH    0x000FFFFFU
#define PMS_DMA_REGION22_HIGH_M  (PMS_DMA_REGION22_HIGH_V << PMS_DMA_REGION22_HIGH_S)
#define PMS_DMA_REGION22_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION22_HIGH_S  12

/** PMS_DMA_REGION23_LOW_REG register
 *  Region23 start address configuration register
 */
#define PMS_DMA_REGION23_LOW_REG (DR_REG_DMA_PMS_BASE + 0xc0)
/** PMS_DMA_REGION23_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region23.
 */
#define PMS_DMA_REGION23_LOW    0x000FFFFFU
#define PMS_DMA_REGION23_LOW_M  (PMS_DMA_REGION23_LOW_V << PMS_DMA_REGION23_LOW_S)
#define PMS_DMA_REGION23_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION23_LOW_S  12

/** PMS_DMA_REGION23_HIGH_REG register
 *  Region23 end address configuration register
 */
#define PMS_DMA_REGION23_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xc4)
/** PMS_DMA_REGION23_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region23.
 */
#define PMS_DMA_REGION23_HIGH    0x000FFFFFU
#define PMS_DMA_REGION23_HIGH_M  (PMS_DMA_REGION23_HIGH_V << PMS_DMA_REGION23_HIGH_S)
#define PMS_DMA_REGION23_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION23_HIGH_S  12

/** PMS_DMA_REGION24_LOW_REG register
 *  Region24 start address configuration register
 */
#define PMS_DMA_REGION24_LOW_REG (DR_REG_DMA_PMS_BASE + 0xc8)
/** PMS_DMA_REGION24_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region24.
 */
#define PMS_DMA_REGION24_LOW    0x000FFFFFU
#define PMS_DMA_REGION24_LOW_M  (PMS_DMA_REGION24_LOW_V << PMS_DMA_REGION24_LOW_S)
#define PMS_DMA_REGION24_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION24_LOW_S  12

/** PMS_DMA_REGION24_HIGH_REG register
 *  Region24 end address configuration register
 */
#define PMS_DMA_REGION24_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xcc)
/** PMS_DMA_REGION24_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region24.
 */
#define PMS_DMA_REGION24_HIGH    0x000FFFFFU
#define PMS_DMA_REGION24_HIGH_M  (PMS_DMA_REGION24_HIGH_V << PMS_DMA_REGION24_HIGH_S)
#define PMS_DMA_REGION24_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION24_HIGH_S  12

/** PMS_DMA_REGION25_LOW_REG register
 *  Region25 start address configuration register
 */
#define PMS_DMA_REGION25_LOW_REG (DR_REG_DMA_PMS_BASE + 0xd0)
/** PMS_DMA_REGION25_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region25.
 */
#define PMS_DMA_REGION25_LOW    0x000FFFFFU
#define PMS_DMA_REGION25_LOW_M  (PMS_DMA_REGION25_LOW_V << PMS_DMA_REGION25_LOW_S)
#define PMS_DMA_REGION25_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION25_LOW_S  12

/** PMS_DMA_REGION25_HIGH_REG register
 *  Region25 end address configuration register
 */
#define PMS_DMA_REGION25_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xd4)
/** PMS_DMA_REGION25_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region25.
 */
#define PMS_DMA_REGION25_HIGH    0x000FFFFFU
#define PMS_DMA_REGION25_HIGH_M  (PMS_DMA_REGION25_HIGH_V << PMS_DMA_REGION25_HIGH_S)
#define PMS_DMA_REGION25_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION25_HIGH_S  12

/** PMS_DMA_REGION26_LOW_REG register
 *  Region26 start address configuration register
 */
#define PMS_DMA_REGION26_LOW_REG (DR_REG_DMA_PMS_BASE + 0xd8)
/** PMS_DMA_REGION26_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region26.
 */
#define PMS_DMA_REGION26_LOW    0x000FFFFFU
#define PMS_DMA_REGION26_LOW_M  (PMS_DMA_REGION26_LOW_V << PMS_DMA_REGION26_LOW_S)
#define PMS_DMA_REGION26_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION26_LOW_S  12

/** PMS_DMA_REGION26_HIGH_REG register
 *  Region26 end address configuration register
 */
#define PMS_DMA_REGION26_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xdc)
/** PMS_DMA_REGION26_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region26.
 */
#define PMS_DMA_REGION26_HIGH    0x000FFFFFU
#define PMS_DMA_REGION26_HIGH_M  (PMS_DMA_REGION26_HIGH_V << PMS_DMA_REGION26_HIGH_S)
#define PMS_DMA_REGION26_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION26_HIGH_S  12

/** PMS_DMA_REGION27_LOW_REG register
 *  Region27 start address configuration register
 */
#define PMS_DMA_REGION27_LOW_REG (DR_REG_DMA_PMS_BASE + 0xe0)
/** PMS_DMA_REGION27_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region27.
 */
#define PMS_DMA_REGION27_LOW    0x000FFFFFU
#define PMS_DMA_REGION27_LOW_M  (PMS_DMA_REGION27_LOW_V << PMS_DMA_REGION27_LOW_S)
#define PMS_DMA_REGION27_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION27_LOW_S  12

/** PMS_DMA_REGION27_HIGH_REG register
 *  Region27 end address configuration register
 */
#define PMS_DMA_REGION27_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xe4)
/** PMS_DMA_REGION27_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region27.
 */
#define PMS_DMA_REGION27_HIGH    0x000FFFFFU
#define PMS_DMA_REGION27_HIGH_M  (PMS_DMA_REGION27_HIGH_V << PMS_DMA_REGION27_HIGH_S)
#define PMS_DMA_REGION27_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION27_HIGH_S  12

/** PMS_DMA_REGION28_LOW_REG register
 *  Region28 start address configuration register
 */
#define PMS_DMA_REGION28_LOW_REG (DR_REG_DMA_PMS_BASE + 0xe8)
/** PMS_DMA_REGION28_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region28.
 */
#define PMS_DMA_REGION28_LOW    0x000FFFFFU
#define PMS_DMA_REGION28_LOW_M  (PMS_DMA_REGION28_LOW_V << PMS_DMA_REGION28_LOW_S)
#define PMS_DMA_REGION28_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION28_LOW_S  12

/** PMS_DMA_REGION28_HIGH_REG register
 *  Region28 end address configuration register
 */
#define PMS_DMA_REGION28_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xec)
/** PMS_DMA_REGION28_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region28.
 */
#define PMS_DMA_REGION28_HIGH    0x000FFFFFU
#define PMS_DMA_REGION28_HIGH_M  (PMS_DMA_REGION28_HIGH_V << PMS_DMA_REGION28_HIGH_S)
#define PMS_DMA_REGION28_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION28_HIGH_S  12

/** PMS_DMA_REGION29_LOW_REG register
 *  Region29 start address configuration register
 */
#define PMS_DMA_REGION29_LOW_REG (DR_REG_DMA_PMS_BASE + 0xf0)
/** PMS_DMA_REGION29_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region29.
 */
#define PMS_DMA_REGION29_LOW    0x000FFFFFU
#define PMS_DMA_REGION29_LOW_M  (PMS_DMA_REGION29_LOW_V << PMS_DMA_REGION29_LOW_S)
#define PMS_DMA_REGION29_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION29_LOW_S  12

/** PMS_DMA_REGION29_HIGH_REG register
 *  Region29 end address configuration register
 */
#define PMS_DMA_REGION29_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xf4)
/** PMS_DMA_REGION29_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region29.
 */
#define PMS_DMA_REGION29_HIGH    0x000FFFFFU
#define PMS_DMA_REGION29_HIGH_M  (PMS_DMA_REGION29_HIGH_V << PMS_DMA_REGION29_HIGH_S)
#define PMS_DMA_REGION29_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION29_HIGH_S  12

/** PMS_DMA_REGION30_LOW_REG register
 *  Region30 start address configuration register
 */
#define PMS_DMA_REGION30_LOW_REG (DR_REG_DMA_PMS_BASE + 0xf8)
/** PMS_DMA_REGION30_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region30.
 */
#define PMS_DMA_REGION30_LOW    0x000FFFFFU
#define PMS_DMA_REGION30_LOW_M  (PMS_DMA_REGION30_LOW_V << PMS_DMA_REGION30_LOW_S)
#define PMS_DMA_REGION30_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION30_LOW_S  12

/** PMS_DMA_REGION30_HIGH_REG register
 *  Region30 end address configuration register
 */
#define PMS_DMA_REGION30_HIGH_REG (DR_REG_DMA_PMS_BASE + 0xfc)
/** PMS_DMA_REGION30_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region30.
 */
#define PMS_DMA_REGION30_HIGH    0x000FFFFFU
#define PMS_DMA_REGION30_HIGH_M  (PMS_DMA_REGION30_HIGH_V << PMS_DMA_REGION30_HIGH_S)
#define PMS_DMA_REGION30_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION30_HIGH_S  12

/** PMS_DMA_REGION31_LOW_REG register
 *  Region31 start address configuration register
 */
#define PMS_DMA_REGION31_LOW_REG (DR_REG_DMA_PMS_BASE + 0x100)
/** PMS_DMA_REGION31_LOW : R/W; bitpos: [31:12]; default: 0;
 *  Configures the high 20 bits of the start address for region31.
 */
#define PMS_DMA_REGION31_LOW    0x000FFFFFU
#define PMS_DMA_REGION31_LOW_M  (PMS_DMA_REGION31_LOW_V << PMS_DMA_REGION31_LOW_S)
#define PMS_DMA_REGION31_LOW_V  0x000FFFFFU
#define PMS_DMA_REGION31_LOW_S  12

/** PMS_DMA_REGION31_HIGH_REG register
 *  Region31 end address configuration register
 */
#define PMS_DMA_REGION31_HIGH_REG (DR_REG_DMA_PMS_BASE + 0x104)
/** PMS_DMA_REGION31_HIGH : R/W; bitpos: [31:12]; default: 1048575;
 *  Configures the high 20 bits of the end address for region31.
 */
#define PMS_DMA_REGION31_HIGH    0x000FFFFFU
#define PMS_DMA_REGION31_HIGH_M  (PMS_DMA_REGION31_HIGH_V << PMS_DMA_REGION31_HIGH_S)
#define PMS_DMA_REGION31_HIGH_V  0x000FFFFFU
#define PMS_DMA_REGION31_HIGH_S  12

/** PMS_DMA_GDMA_CH0_R_PMS_REG register
 *  GDMA ch0 read permission control register
 */
#define PMS_DMA_GDMA_CH0_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x108)
/** PMS_DMA_GDMA_CH0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch0 to read 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_GDMA_CH0_R_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH0_R_PMS_M  (PMS_DMA_GDMA_CH0_R_PMS_V << PMS_DMA_GDMA_CH0_R_PMS_S)
#define PMS_DMA_GDMA_CH0_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH0_R_PMS_S  0

/** PMS_DMA_GDMA_CH0_W_PMS_REG register
 *  GDMA ch0 write permission control register
 */
#define PMS_DMA_GDMA_CH0_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x10c)
/** PMS_DMA_GDMA_CH0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch0 to write 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_GDMA_CH0_W_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH0_W_PMS_M  (PMS_DMA_GDMA_CH0_W_PMS_V << PMS_DMA_GDMA_CH0_W_PMS_S)
#define PMS_DMA_GDMA_CH0_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH0_W_PMS_S  0

/** PMS_DMA_GDMA_CH1_R_PMS_REG register
 *  GDMA ch1 read permission control register
 */
#define PMS_DMA_GDMA_CH1_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x110)
/** PMS_DMA_GDMA_CH1_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch1 to read 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_GDMA_CH1_R_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH1_R_PMS_M  (PMS_DMA_GDMA_CH1_R_PMS_V << PMS_DMA_GDMA_CH1_R_PMS_S)
#define PMS_DMA_GDMA_CH1_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH1_R_PMS_S  0

/** PMS_DMA_GDMA_CH1_W_PMS_REG register
 *  GDMA ch1 write permission control register
 */
#define PMS_DMA_GDMA_CH1_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x114)
/** PMS_DMA_GDMA_CH1_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch1 to write 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_GDMA_CH1_W_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH1_W_PMS_M  (PMS_DMA_GDMA_CH1_W_PMS_V << PMS_DMA_GDMA_CH1_W_PMS_S)
#define PMS_DMA_GDMA_CH1_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH1_W_PMS_S  0

/** PMS_DMA_GDMA_CH2_R_PMS_REG register
 *  GDMA ch2 read permission control register
 */
#define PMS_DMA_GDMA_CH2_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x118)
/** PMS_DMA_GDMA_CH2_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch2 to read 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_GDMA_CH2_R_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH2_R_PMS_M  (PMS_DMA_GDMA_CH2_R_PMS_V << PMS_DMA_GDMA_CH2_R_PMS_S)
#define PMS_DMA_GDMA_CH2_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH2_R_PMS_S  0

/** PMS_DMA_GDMA_CH2_W_PMS_REG register
 *  GDMA ch2 write permission control register
 */
#define PMS_DMA_GDMA_CH2_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x11c)
/** PMS_DMA_GDMA_CH2_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch2 to write 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_GDMA_CH2_W_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH2_W_PMS_M  (PMS_DMA_GDMA_CH2_W_PMS_V << PMS_DMA_GDMA_CH2_W_PMS_S)
#define PMS_DMA_GDMA_CH2_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH2_W_PMS_S  0

/** PMS_DMA_GDMA_CH3_R_PMS_REG register
 *  GDMA ch3 read permission control register
 */
#define PMS_DMA_GDMA_CH3_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x120)
/** PMS_DMA_GDMA_CH3_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch3 to read 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_GDMA_CH3_R_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH3_R_PMS_M  (PMS_DMA_GDMA_CH3_R_PMS_V << PMS_DMA_GDMA_CH3_R_PMS_S)
#define PMS_DMA_GDMA_CH3_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH3_R_PMS_S  0

/** PMS_DMA_GDMA_CH3_W_PMS_REG register
 *  GDMA ch3 write permission control register
 */
#define PMS_DMA_GDMA_CH3_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x124)
/** PMS_DMA_GDMA_CH3_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures the permission for GDMA ch3 to write 32 address regions. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_GDMA_CH3_W_PMS    0xFFFFFFFFU
#define PMS_DMA_GDMA_CH3_W_PMS_M  (PMS_DMA_GDMA_CH3_W_PMS_V << PMS_DMA_GDMA_CH3_W_PMS_S)
#define PMS_DMA_GDMA_CH3_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GDMA_CH3_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_ADC_R_PMS_REG register
 *  GDMA-AHB ADC read permission control register
 */
#define PMS_DMA_AHB_PDMA_ADC_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x128)
/** PMS_DMA_AHB_PDMA_ADC_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by ADC. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_ADC_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_ADC_R_PMS_M  (PMS_DMA_AHB_PDMA_ADC_R_PMS_V << PMS_DMA_AHB_PDMA_ADC_R_PMS_S)
#define PMS_DMA_AHB_PDMA_ADC_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_ADC_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_ADC_W_PMS_REG register
 *  GDMA-AHB ADC write permission control register
 */
#define PMS_DMA_AHB_PDMA_ADC_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x12c)
/** PMS_DMA_AHB_PDMA_ADC_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by ADC. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_ADC_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_ADC_W_PMS_M  (PMS_DMA_AHB_PDMA_ADC_W_PMS_V << PMS_DMA_AHB_PDMA_ADC_W_PMS_S)
#define PMS_DMA_AHB_PDMA_ADC_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_ADC_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_I2S0_R_PMS_REG register
 *  GDMA-AHB I2S0 read permission control register
 */
#define PMS_DMA_AHB_PDMA_I2S0_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x130)
/** PMS_DMA_AHB_PDMA_I2S0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by I2S0. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_I2S0_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S0_R_PMS_M  (PMS_DMA_AHB_PDMA_I2S0_R_PMS_V << PMS_DMA_AHB_PDMA_I2S0_R_PMS_S)
#define PMS_DMA_AHB_PDMA_I2S0_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S0_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_I2S0_W_PMS_REG register
 *  GDMA-AHB I2S0 write permission control register
 */
#define PMS_DMA_AHB_PDMA_I2S0_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x134)
/** PMS_DMA_AHB_PDMA_I2S0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by I2S0. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_I2S0_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S0_W_PMS_M  (PMS_DMA_AHB_PDMA_I2S0_W_PMS_V << PMS_DMA_AHB_PDMA_I2S0_W_PMS_S)
#define PMS_DMA_AHB_PDMA_I2S0_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S0_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_I2S1_R_PMS_REG register
 *  GDMA-AHB I2S1 read permission control register
 */
#define PMS_DMA_AHB_PDMA_I2S1_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x138)
/** PMS_DMA_AHB_PDMA_I2S1_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by I2S1. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_I2S1_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S1_R_PMS_M  (PMS_DMA_AHB_PDMA_I2S1_R_PMS_V << PMS_DMA_AHB_PDMA_I2S1_R_PMS_S)
#define PMS_DMA_AHB_PDMA_I2S1_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S1_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_I2S1_W_PMS_REG register
 *  GDMA-AHB I2S1 write permission control register
 */
#define PMS_DMA_AHB_PDMA_I2S1_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x13c)
/** PMS_DMA_AHB_PDMA_I2S1_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by I2S1. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_I2S1_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S1_W_PMS_M  (PMS_DMA_AHB_PDMA_I2S1_W_PMS_V << PMS_DMA_AHB_PDMA_I2S1_W_PMS_S)
#define PMS_DMA_AHB_PDMA_I2S1_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S1_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_I2S2_R_PMS_REG register
 *  GDMA-AHB I2S2 read permission control register
 */
#define PMS_DMA_AHB_PDMA_I2S2_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x140)
/** PMS_DMA_AHB_PDMA_I2S2_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by I2S2. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_I2S2_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S2_R_PMS_M  (PMS_DMA_AHB_PDMA_I2S2_R_PMS_V << PMS_DMA_AHB_PDMA_I2S2_R_PMS_S)
#define PMS_DMA_AHB_PDMA_I2S2_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S2_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_I2S2_W_PMS_REG register
 *  GDMA-AHB I2S2 write permission control register
 */
#define PMS_DMA_AHB_PDMA_I2S2_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x144)
/** PMS_DMA_AHB_PDMA_I2S2_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by I2S2. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_I2S2_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S2_W_PMS_M  (PMS_DMA_AHB_PDMA_I2S2_W_PMS_V << PMS_DMA_AHB_PDMA_I2S2_W_PMS_S)
#define PMS_DMA_AHB_PDMA_I2S2_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I2S2_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_I3C_MST_R_PMS_REG register
 *  GDMA-AHB I3C MST read permission control register
 */
#define PMS_DMA_AHB_PDMA_I3C_MST_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x148)
/** PMS_DMA_AHB_PDMA_I3C_MST_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by I3C  master.
 *  Bit 0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_I3C_MST_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I3C_MST_R_PMS_M  (PMS_DMA_AHB_PDMA_I3C_MST_R_PMS_V << PMS_DMA_AHB_PDMA_I3C_MST_R_PMS_S)
#define PMS_DMA_AHB_PDMA_I3C_MST_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I3C_MST_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_I3C_MST_W_PMS_REG register
 *  GDMA-AHB I3C MST write permission control register
 */
#define PMS_DMA_AHB_PDMA_I3C_MST_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x14c)
/** PMS_DMA_AHB_PDMA_I3C_MST_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by I3C  master.
 *  Bit 0 corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_I3C_MST_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I3C_MST_W_PMS_M  (PMS_DMA_AHB_PDMA_I3C_MST_W_PMS_V << PMS_DMA_AHB_PDMA_I3C_MST_W_PMS_S)
#define PMS_DMA_AHB_PDMA_I3C_MST_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_I3C_MST_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_UHCI0_R_PMS_REG register
 *  GDMA-AHB UHCI read permission control register
 */
#define PMS_DMA_AHB_PDMA_UHCI0_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x150)
/** PMS_DMA_AHB_PDMA_UHCI0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by UHCI. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_UHCI0_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_UHCI0_R_PMS_M  (PMS_DMA_AHB_PDMA_UHCI0_R_PMS_V << PMS_DMA_AHB_PDMA_UHCI0_R_PMS_S)
#define PMS_DMA_AHB_PDMA_UHCI0_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_UHCI0_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_UHCI0_W_PMS_REG register
 *  GDMA-AHB UHCI write permission control register
 */
#define PMS_DMA_AHB_PDMA_UHCI0_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x154)
/** PMS_DMA_AHB_PDMA_UHCI0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by UHCI. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_UHCI0_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_UHCI0_W_PMS_M  (PMS_DMA_AHB_PDMA_UHCI0_W_PMS_V << PMS_DMA_AHB_PDMA_UHCI0_W_PMS_S)
#define PMS_DMA_AHB_PDMA_UHCI0_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_UHCI0_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_RMT_R_PMS_REG register
 *  GDMA-AHB RMT read permission control register
 */
#define PMS_DMA_AHB_PDMA_RMT_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x158)
/** PMS_DMA_AHB_PDMA_RMT_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by RMT. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_RMT_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_RMT_R_PMS_M  (PMS_DMA_AHB_PDMA_RMT_R_PMS_V << PMS_DMA_AHB_PDMA_RMT_R_PMS_S)
#define PMS_DMA_AHB_PDMA_RMT_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_RMT_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_RMT_W_PMS_REG register
 *  GDMA-AHB RMT write permission control register
 */
#define PMS_DMA_AHB_PDMA_RMT_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x170)
/** PMS_DMA_AHB_PDMA_RMT_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by RMT. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_RMT_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_RMT_W_PMS_M  (PMS_DMA_AHB_PDMA_RMT_W_PMS_V << PMS_DMA_AHB_PDMA_RMT_W_PMS_S)
#define PMS_DMA_AHB_PDMA_RMT_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_RMT_W_PMS_S  0

/** PMS_DMA_AXI_PDMA_LCDCAM_R_PMS_REG register
 *  GDMA-AXI LCD_CAM read permission control register
 */
#define PMS_DMA_AXI_PDMA_LCDCAM_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x174)
/** PMS_DMA_AXI_PDMA_LCDCAM_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to read 32 address ranges requested by LCD_CAM. Bit
 *  0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AXI_PDMA_LCDCAM_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_LCDCAM_R_PMS_M  (PMS_DMA_AXI_PDMA_LCDCAM_R_PMS_V << PMS_DMA_AXI_PDMA_LCDCAM_R_PMS_S)
#define PMS_DMA_AXI_PDMA_LCDCAM_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_LCDCAM_R_PMS_S  0

/** PMS_DMA_AXI_PDMA_LCDCAM_W_PMS_REG register
 *  GDMA-AXI LCD_CAM write permission control register
 */
#define PMS_DMA_AXI_PDMA_LCDCAM_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x178)
/** PMS_DMA_AXI_PDMA_LCDCAM_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to write 32 address ranges requested by LCD_CAM. Bit
 *  0 corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AXI_PDMA_LCDCAM_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_LCDCAM_W_PMS_M  (PMS_DMA_AXI_PDMA_LCDCAM_W_PMS_V << PMS_DMA_AXI_PDMA_LCDCAM_W_PMS_S)
#define PMS_DMA_AXI_PDMA_LCDCAM_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_LCDCAM_W_PMS_S  0

/** PMS_DMA_AXI_PDMA_GPSPI2_R_PMS_REG register
 *  GDMA-AXI GPSPI2 read permission control register
 */
#define PMS_DMA_AXI_PDMA_GPSPI2_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x17c)
/** PMS_DMA_AXI_PDMA_GPSPI2_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to read 32 address ranges requested by GP-SPI2. Bit
 *  0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AXI_PDMA_GPSPI2_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI2_R_PMS_M  (PMS_DMA_AXI_PDMA_GPSPI2_R_PMS_V << PMS_DMA_AXI_PDMA_GPSPI2_R_PMS_S)
#define PMS_DMA_AXI_PDMA_GPSPI2_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI2_R_PMS_S  0

/** PMS_DMA_AXI_PDMA_GPSPI2_W_PMS_REG register
 *  GDMA-AXI GPSPI2 write permission control register
 */
#define PMS_DMA_AXI_PDMA_GPSPI2_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x180)
/** PMS_DMA_AXI_PDMA_GPSPI2_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to write 32 address ranges requested by GP-SPI2. Bit
 *  0 corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AXI_PDMA_GPSPI2_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI2_W_PMS_M  (PMS_DMA_AXI_PDMA_GPSPI2_W_PMS_V << PMS_DMA_AXI_PDMA_GPSPI2_W_PMS_S)
#define PMS_DMA_AXI_PDMA_GPSPI2_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI2_W_PMS_S  0

/** PMS_DMA_AXI_PDMA_GPSPI3_R_PMS_REG register
 *  GDMA-AXI GPSPI3 read permission control register
 */
#define PMS_DMA_AXI_PDMA_GPSPI3_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x184)
/** PMS_DMA_AXI_PDMA_GPSPI3_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to read 32 address ranges requested by GP-SPI3. Bit
 *  0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AXI_PDMA_GPSPI3_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI3_R_PMS_M  (PMS_DMA_AXI_PDMA_GPSPI3_R_PMS_V << PMS_DMA_AXI_PDMA_GPSPI3_R_PMS_S)
#define PMS_DMA_AXI_PDMA_GPSPI3_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI3_R_PMS_S  0

/** PMS_DMA_AXI_PDMA_GPSPI3_W_PMS_REG register
 *  AXI PDMA GPSPI3 write permission control register
 */
#define PMS_DMA_AXI_PDMA_GPSPI3_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x188)
/** PMS_DMA_AXI_PDMA_GPSPI3_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to write 32 address ranges requested by GP-SPI3. Bit
 *  0 corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AXI_PDMA_GPSPI3_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI3_W_PMS_M  (PMS_DMA_AXI_PDMA_GPSPI3_W_PMS_V << PMS_DMA_AXI_PDMA_GPSPI3_W_PMS_S)
#define PMS_DMA_AXI_PDMA_GPSPI3_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_GPSPI3_W_PMS_S  0

/** PMS_DMA_AXI_PDMA_PARLIO_R_PMS_REG register
 *  GDMA-AXI PARLIO read permission control register
 */
#define PMS_DMA_AXI_PDMA_PARLIO_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x18c)
/** PMS_DMA_AXI_PDMA_PARLIO_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to read 32 address ranges requested by PARLIO
 *  (Parallel IO Controller). Bit 0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AXI_PDMA_PARLIO_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_PARLIO_R_PMS_M  (PMS_DMA_AXI_PDMA_PARLIO_R_PMS_V << PMS_DMA_AXI_PDMA_PARLIO_R_PMS_S)
#define PMS_DMA_AXI_PDMA_PARLIO_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_PARLIO_R_PMS_S  0

/** PMS_DMA_AXI_PDMA_PARLIO_W_PMS_REG register
 *  GDMA-AXI PARLIO write permission control register
 */
#define PMS_DMA_AXI_PDMA_PARLIO_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x190)
/** PMS_DMA_AXI_PDMA_PARLIO_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to write 32 address ranges requested by PARLIO. Bit
 *  0 corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AXI_PDMA_PARLIO_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_PARLIO_W_PMS_M  (PMS_DMA_AXI_PDMA_PARLIO_W_PMS_V << PMS_DMA_AXI_PDMA_PARLIO_W_PMS_S)
#define PMS_DMA_AXI_PDMA_PARLIO_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_PARLIO_W_PMS_S  0

/** PMS_DMA_AXI_PDMA_AES_R_PMS_REG register
 *  GDMA-AXI AES read permission control register
 */
#define PMS_DMA_AXI_PDMA_AES_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x194)
/** PMS_DMA_AXI_PDMA_AES_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to read 32 address ranges requested by AES. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AXI_PDMA_AES_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_AES_R_PMS_M  (PMS_DMA_AXI_PDMA_AES_R_PMS_V << PMS_DMA_AXI_PDMA_AES_R_PMS_S)
#define PMS_DMA_AXI_PDMA_AES_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_AES_R_PMS_S  0

/** PMS_DMA_AXI_PDMA_AES_W_PMS_REG register
 *  GDMA-AXI AES write permission control register
 */
#define PMS_DMA_AXI_PDMA_AES_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x198)
/** PMS_DMA_AXI_PDMA_AES_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to write 32 address ranges requested by AES. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AXI_PDMA_AES_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_AES_W_PMS_M  (PMS_DMA_AXI_PDMA_AES_W_PMS_V << PMS_DMA_AXI_PDMA_AES_W_PMS_S)
#define PMS_DMA_AXI_PDMA_AES_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_AES_W_PMS_S  0

/** PMS_DMA_AXI_PDMA_SHA_R_PMS_REG register
 *  GDMA-AXI SHA read permission control register
 */
#define PMS_DMA_AXI_PDMA_SHA_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x19c)
/** PMS_DMA_AXI_PDMA_SHA_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to read 32 address ranges requested by SHA. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AXI_PDMA_SHA_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_SHA_R_PMS_M  (PMS_DMA_AXI_PDMA_SHA_R_PMS_V << PMS_DMA_AXI_PDMA_SHA_R_PMS_S)
#define PMS_DMA_AXI_PDMA_SHA_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_SHA_R_PMS_S  0

/** PMS_DMA_AXI_PDMA_SHA_W_PMS_REG register
 *  GDMA-AXI SHA write permission control register
 */
#define PMS_DMA_AXI_PDMA_SHA_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x1a0)
/** PMS_DMA_AXI_PDMA_SHA_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to write 32 address ranges requested by SHA. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AXI_PDMA_SHA_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_SHA_W_PMS_M  (PMS_DMA_AXI_PDMA_SHA_W_PMS_V << PMS_DMA_AXI_PDMA_SHA_W_PMS_S)
#define PMS_DMA_AXI_PDMA_SHA_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_SHA_W_PMS_S  0

/** PMS_DMA_DMA2D_JPEG_PMS_R_REG register
 *  2D-DMA JPEG read permission control register
 */
#define PMS_DMA_DMA2D_JPEG_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1a4)
/** PMS_DMA_DMA2D_JPEG_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures 2D-DMA permission to read 32 address ranges requested by JPEG. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_DMA2D_JPEG_R_PMS    0xFFFFFFFFU
#define PMS_DMA_DMA2D_JPEG_R_PMS_M  (PMS_DMA_DMA2D_JPEG_R_PMS_V << PMS_DMA_DMA2D_JPEG_R_PMS_S)
#define PMS_DMA_DMA2D_JPEG_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_DMA2D_JPEG_R_PMS_S  0

/** PMS_DMA_DMA2D_JPEG_PMS_W_REG register
 *  2D-DMA JPEG write permission control register
 */
#define PMS_DMA_DMA2D_JPEG_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1a8)
/** PMS_DMA_DMA2D_JPEG_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures 2D-DMA permission to write 32 address ranges requested by JPEG. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_DMA2D_JPEG_W_PMS    0xFFFFFFFFU
#define PMS_DMA_DMA2D_JPEG_W_PMS_M  (PMS_DMA_DMA2D_JPEG_W_PMS_V << PMS_DMA_DMA2D_JPEG_W_PMS_S)
#define PMS_DMA_DMA2D_JPEG_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_DMA2D_JPEG_W_PMS_S  0

/** PMS_DMA_USB_PMS_R_REG register
 *  High-speed USB 2.0 OTG read permission control register
 */
#define PMS_DMA_USB_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1ac)
/** PMS_DMA_USB_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for high-speed USB 2.0 OTG to access 32 address ranges.
 *  Bit 0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_USB_R_PMS    0xFFFFFFFFU
#define PMS_DMA_USB_R_PMS_M  (PMS_DMA_USB_R_PMS_V << PMS_DMA_USB_R_PMS_S)
#define PMS_DMA_USB_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_USB_R_PMS_S  0

/** PMS_DMA_USB_PMS_W_REG register
 *  High-speed USB 2.0 OTG write permission control register
 */
#define PMS_DMA_USB_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1b0)
/** PMS_DMA_USB_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for high-speed USB 2.0 OTG to access 32 address ranges.
 *  Bit 0 corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_USB_W_PMS    0xFFFFFFFFU
#define PMS_DMA_USB_W_PMS_M  (PMS_DMA_USB_W_PMS_V << PMS_DMA_USB_W_PMS_S)
#define PMS_DMA_USB_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_USB_W_PMS_S  0

/** PMS_DMA_GMAC_PMS_R_REG register
 *  EMAC read permission control register
 */
#define PMS_DMA_GMAC_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1b4)
/** PMS_DMA_GMAC_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for EMAC to access 32 address ranges. Bit 0 corresponds
 *  to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_GMAC_R_PMS    0xFFFFFFFFU
#define PMS_DMA_GMAC_R_PMS_M  (PMS_DMA_GMAC_R_PMS_V << PMS_DMA_GMAC_R_PMS_S)
#define PMS_DMA_GMAC_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GMAC_R_PMS_S  0

/** PMS_DMA_GMAC_PMS_W_REG register
 *  EMAC write permission control register
 */
#define PMS_DMA_GMAC_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1b8)
/** PMS_DMA_GMAC_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for EMAC to access 32 address ranges. Bit 0 corresponds
 *  to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_GMAC_W_PMS    0xFFFFFFFFU
#define PMS_DMA_GMAC_W_PMS_M  (PMS_DMA_GMAC_W_PMS_V << PMS_DMA_GMAC_W_PMS_S)
#define PMS_DMA_GMAC_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_GMAC_W_PMS_S  0

/** PMS_DMA_SDMMC_PMS_R_REG register
 *  SDMMC read permission control register
 */
#define PMS_DMA_SDMMC_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1bc)
/** PMS_DMA_SDMMC_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for SDMMC to access 32 address ranges. Bit 0 corresponds
 *  to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_SDMMC_R_PMS    0xFFFFFFFFU
#define PMS_DMA_SDMMC_R_PMS_M  (PMS_DMA_SDMMC_R_PMS_V << PMS_DMA_SDMMC_R_PMS_S)
#define PMS_DMA_SDMMC_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_SDMMC_R_PMS_S  0

/** PMS_DMA_SDMMC_PMS_W_REG register
 *  SDMMC write permission control register
 */
#define PMS_DMA_SDMMC_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1c0)
/** PMS_DMA_SDMMC_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for SDMMC to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_SDMMC_W_PMS    0xFFFFFFFFU
#define PMS_DMA_SDMMC_W_PMS_M  (PMS_DMA_SDMMC_W_PMS_V << PMS_DMA_SDMMC_W_PMS_S)
#define PMS_DMA_SDMMC_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_SDMMC_W_PMS_S  0

/** PMS_DMA_USBOTG11_PMS_R_REG register
 *  Full-speed USB 2.0 OTG full-speed read permission control register
 */
#define PMS_DMA_USBOTG11_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1c4)
/** PMS_DMA_USBOTG11_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for full-speed USB 2.0 OTG to access 32 address ranges.
 *  Bit 0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_USBOTG11_R_PMS    0xFFFFFFFFU
#define PMS_DMA_USBOTG11_R_PMS_M  (PMS_DMA_USBOTG11_R_PMS_V << PMS_DMA_USBOTG11_R_PMS_S)
#define PMS_DMA_USBOTG11_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_USBOTG11_R_PMS_S  0

/** PMS_DMA_USBOTG11_PMS_W_REG register
 *  Full-speed USB 2.0 OTG full-speed write permission control register
 */
#define PMS_DMA_USBOTG11_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1c8)
/** PMS_DMA_USBOTG11_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for full-speed USB 2.0 OTG to access 32 address ranges.
 *  Bit 0 corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_USBOTG11_W_PMS    0xFFFFFFFFU
#define PMS_DMA_USBOTG11_W_PMS_M  (PMS_DMA_USBOTG11_W_PMS_V << PMS_DMA_USBOTG11_W_PMS_S)
#define PMS_DMA_USBOTG11_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_USBOTG11_W_PMS_S  0

/** PMS_DMA_TRACE0_PMS_R_REG register
 *  TRACE0 read permission control register
 */
#define PMS_DMA_TRACE0_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1cc)
/** PMS_DMA_TRACE0_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for TRACE0 to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_TRACE0_R_PMS    0xFFFFFFFFU
#define PMS_DMA_TRACE0_R_PMS_M  (PMS_DMA_TRACE0_R_PMS_V << PMS_DMA_TRACE0_R_PMS_S)
#define PMS_DMA_TRACE0_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_TRACE0_R_PMS_S  0

/** PMS_DMA_TRACE0_PMS_W_REG register
 *  TRACE0 write permission control register
 */
#define PMS_DMA_TRACE0_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1d0)
/** PMS_DMA_TRACE0_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for TRACE0 to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_TRACE0_W_PMS    0xFFFFFFFFU
#define PMS_DMA_TRACE0_W_PMS_M  (PMS_DMA_TRACE0_W_PMS_V << PMS_DMA_TRACE0_W_PMS_S)
#define PMS_DMA_TRACE0_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_TRACE0_W_PMS_S  0

/** PMS_DMA_TRACE1_PMS_R_REG register
 *  TRACE1 read permission control register
 */
#define PMS_DMA_TRACE1_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1d4)
/** PMS_DMA_TRACE1_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for TRACE1 to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_TRACE1_R_PMS    0xFFFFFFFFU
#define PMS_DMA_TRACE1_R_PMS_M  (PMS_DMA_TRACE1_R_PMS_V << PMS_DMA_TRACE1_R_PMS_S)
#define PMS_DMA_TRACE1_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_TRACE1_R_PMS_S  0

/** PMS_DMA_TRACE1_PMS_W_REG register
 *  TRACE1 write permission control register
 */
#define PMS_DMA_TRACE1_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1d8)
/** PMS_DMA_TRACE1_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for TRACE1 to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_TRACE1_W_PMS    0xFFFFFFFFU
#define PMS_DMA_TRACE1_W_PMS_M  (PMS_DMA_TRACE1_W_PMS_V << PMS_DMA_TRACE1_W_PMS_S)
#define PMS_DMA_TRACE1_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_TRACE1_W_PMS_S  0

/** PMS_DMA_L2MEM_MON_PMS_R_REG register
 *  L2MEM Monitor read permission control register
 */
#define PMS_DMA_L2MEM_MON_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1dc)
/** PMS_DMA_L2MEM_MON_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for L2MEM MON. Each bit corresponds to a region. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_L2MEM_MON_R_PMS    0xFFFFFFFFU
#define PMS_DMA_L2MEM_MON_R_PMS_M  (PMS_DMA_L2MEM_MON_R_PMS_V << PMS_DMA_L2MEM_MON_R_PMS_S)
#define PMS_DMA_L2MEM_MON_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_L2MEM_MON_R_PMS_S  0

/** PMS_DMA_L2MEM_MON_PMS_W_REG register
 *  L2MEM Monitor write permission control register
 */
#define PMS_DMA_L2MEM_MON_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1e0)
/** PMS_DMA_L2MEM_MON_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for L2MEM monitor to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_L2MEM_MON_W_PMS    0xFFFFFFFFU
#define PMS_DMA_L2MEM_MON_W_PMS_M  (PMS_DMA_L2MEM_MON_W_PMS_V << PMS_DMA_L2MEM_MON_W_PMS_S)
#define PMS_DMA_L2MEM_MON_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_L2MEM_MON_W_PMS_S  0

/** PMS_DMA_TCM_MON_PMS_R_REG register
 *  TCM Monitor read permission control register
 */
#define PMS_DMA_TCM_MON_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1e4)
/** PMS_DMA_TCM_MON_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for TCM MON. Each bit corresponds to a region. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_TCM_MON_R_PMS    0xFFFFFFFFU
#define PMS_DMA_TCM_MON_R_PMS_M  (PMS_DMA_TCM_MON_R_PMS_V << PMS_DMA_TCM_MON_R_PMS_S)
#define PMS_DMA_TCM_MON_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_TCM_MON_R_PMS_S  0

/** PMS_DMA_TCM_MON_PMS_W_REG register
 *  TCM Monitor write permission control register
 */
#define PMS_DMA_TCM_MON_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1e8)
/** PMS_DMA_TCM_MON_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for TCM monitor to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_TCM_MON_W_PMS    0xFFFFFFFFU
#define PMS_DMA_TCM_MON_W_PMS_M  (PMS_DMA_TCM_MON_W_PMS_V << PMS_DMA_TCM_MON_W_PMS_S)
#define PMS_DMA_TCM_MON_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_TCM_MON_W_PMS_S  0

/** PMS_DMA_REGDMA_PMS_R_REG register
 *  REGDMA read permission control register
 */
#define PMS_DMA_REGDMA_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1ec)
/** PMS_DMA_REGDMA_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for REGDMA. Each bit corresponds to a region. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_REGDMA_R_PMS    0xFFFFFFFFU
#define PMS_DMA_REGDMA_R_PMS_M  (PMS_DMA_REGDMA_R_PMS_V << PMS_DMA_REGDMA_R_PMS_S)
#define PMS_DMA_REGDMA_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_REGDMA_R_PMS_S  0

/** PMS_DMA_REGDMA_PMS_W_REG register
 *  REGDMA write permission control register
 */
#define PMS_DMA_REGDMA_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x1f0)
/** PMS_DMA_REGDMA_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for REGDMA. Each bit corresponds to a region. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_REGDMA_W_PMS    0xFFFFFFFFU
#define PMS_DMA_REGDMA_W_PMS_M  (PMS_DMA_REGDMA_W_PMS_V << PMS_DMA_REGDMA_W_PMS_S)
#define PMS_DMA_REGDMA_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_REGDMA_W_PMS_S  0

/** PMS_DMA_H264_PMS_R_REG register
 *  H264 DMA read permission control register
 */
#define PMS_DMA_H264_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x1fc)
/** PMS_DMA_H264_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures read permission for H264 DMA to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_H264_R_PMS    0xFFFFFFFFU
#define PMS_DMA_H264_R_PMS_M  (PMS_DMA_H264_R_PMS_V << PMS_DMA_H264_R_PMS_S)
#define PMS_DMA_H264_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_H264_R_PMS_S  0

/** PMS_DMA_H264_PMS_W_REG register
 *  H264 DMA write permission control register
 */
#define PMS_DMA_H264_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x200)
/** PMS_DMA_H264_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures write permission for H264 DMA to access 32 address ranges. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_H264_W_PMS    0xFFFFFFFFU
#define PMS_DMA_H264_W_PMS_M  (PMS_DMA_H264_W_PMS_V << PMS_DMA_H264_W_PMS_S)
#define PMS_DMA_H264_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_H264_W_PMS_S  0

/** PMS_DMA_DMA2D_PPA_PMS_R_REG register
 *  2D-DMA PPA read permission control register
 */
#define PMS_DMA_DMA2D_PPA_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x204)
/** PMS_DMA_DMA2D_PPA_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures 2D-DMA permission to read 32 address ranges requested by PPA
 *  (Pixel-Processing Accelerator). Bit 0 corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_DMA2D_PPA_R_PMS    0xFFFFFFFFU
#define PMS_DMA_DMA2D_PPA_R_PMS_M  (PMS_DMA_DMA2D_PPA_R_PMS_V << PMS_DMA_DMA2D_PPA_R_PMS_S)
#define PMS_DMA_DMA2D_PPA_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_DMA2D_PPA_R_PMS_S  0

/** PMS_DMA_DMA2D_PPA_PMS_W_REG register
 *  2D-DMA PPA write permission control register
 */
#define PMS_DMA_DMA2D_PPA_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x208)
/** PMS_DMA_DMA2D_PPA_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures 2D-DMA permission to write 32 address ranges requested by PPA. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_DMA2D_PPA_W_PMS    0xFFFFFFFFU
#define PMS_DMA_DMA2D_PPA_W_PMS_M  (PMS_DMA_DMA2D_PPA_W_PMS_V << PMS_DMA_DMA2D_PPA_W_PMS_S)
#define PMS_DMA_DMA2D_PPA_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_DMA2D_PPA_W_PMS_S  0

/** PMS_DMA_DMA2D_DUMMY_PMS_R_REG register
 *  2D-DMA dummy read permission control register
 */
#define PMS_DMA_DMA2D_DUMMY_PMS_R_REG (DR_REG_DMA_PMS_BASE + 0x20c)
/** PMS_DMA_DMA2D_DUMMY_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures 2D-DMA permission to read 32 address ranges requested by Dummy. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_DMA2D_DUMMY_R_PMS    0xFFFFFFFFU
#define PMS_DMA_DMA2D_DUMMY_R_PMS_M  (PMS_DMA_DMA2D_DUMMY_R_PMS_V << PMS_DMA_DMA2D_DUMMY_R_PMS_S)
#define PMS_DMA_DMA2D_DUMMY_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_DMA2D_DUMMY_R_PMS_S  0

/** PMS_DMA_DMA2D_DUMMY_PMS_W_REG register
 *  2D-DMA dummy write permission control register
 */
#define PMS_DMA_DMA2D_DUMMY_PMS_W_REG (DR_REG_DMA_PMS_BASE + 0x210)
/** PMS_DMA_DMA2D_DUMMY_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures 2D-DMA permission to write 32 address ranges requested by Dummy. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_DMA2D_DUMMY_W_PMS    0xFFFFFFFFU
#define PMS_DMA_DMA2D_DUMMY_W_PMS_M  (PMS_DMA_DMA2D_DUMMY_W_PMS_V << PMS_DMA_DMA2D_DUMMY_W_PMS_S)
#define PMS_DMA_DMA2D_DUMMY_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_DMA2D_DUMMY_W_PMS_S  0

/** PMS_DMA_AHB_PDMA_DUMMY_R_PMS_REG register
 *  GDMA-AHB dummy read permission control register
 */
#define PMS_DMA_AHB_PDMA_DUMMY_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x214)
/** PMS_DMA_AHB_PDMA_DUMMY_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to read 32 address ranges requested by Dummy. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AHB_PDMA_DUMMY_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_DUMMY_R_PMS_M  (PMS_DMA_AHB_PDMA_DUMMY_R_PMS_V << PMS_DMA_AHB_PDMA_DUMMY_R_PMS_S)
#define PMS_DMA_AHB_PDMA_DUMMY_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_DUMMY_R_PMS_S  0

/** PMS_DMA_AHB_PDMA_DUMMY_W_PMS_REG register
 *  GDMA-AHB dummy write permission control register
 */
#define PMS_DMA_AHB_PDMA_DUMMY_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x218)
/** PMS_DMA_AHB_PDMA_DUMMY_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AHB permission to write 32 address ranges requested by Dummy. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AHB_PDMA_DUMMY_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_DUMMY_W_PMS_M  (PMS_DMA_AHB_PDMA_DUMMY_W_PMS_V << PMS_DMA_AHB_PDMA_DUMMY_W_PMS_S)
#define PMS_DMA_AHB_PDMA_DUMMY_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AHB_PDMA_DUMMY_W_PMS_S  0

/** PMS_DMA_AXI_PDMA_DUMMY_R_PMS_REG register
 *  GDMA-AXI dummy read permission control register
 */
#define PMS_DMA_AXI_PDMA_DUMMY_R_PMS_REG (DR_REG_DMA_PMS_BASE + 0x21c)
/** PMS_DMA_AXI_PDMA_DUMMY_R_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to read 32 address ranges requested by Dummy. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable read permission.
 *  1: Enable read permission.
 */
#define PMS_DMA_AXI_PDMA_DUMMY_R_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_DUMMY_R_PMS_M  (PMS_DMA_AXI_PDMA_DUMMY_R_PMS_V << PMS_DMA_AXI_PDMA_DUMMY_R_PMS_S)
#define PMS_DMA_AXI_PDMA_DUMMY_R_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_DUMMY_R_PMS_S  0

/** PMS_DMA_AXI_PDMA_DUMMY_W_PMS_REG register
 *  GDMA-AXI dummy write permission control register
 */
#define PMS_DMA_AXI_PDMA_DUMMY_W_PMS_REG (DR_REG_DMA_PMS_BASE + 0x220)
/** PMS_DMA_AXI_PDMA_DUMMY_W_PMS : R/W; bitpos: [31:0]; default: 4294967295;
 *  Configures GDMA-AXI permission to write 32 address ranges requested by Dummy. Bit 0
 *  corresponds to region0, and so on.
 *  0: Disable write permission.
 *  1: Enable write permission.
 */
#define PMS_DMA_AXI_PDMA_DUMMY_W_PMS    0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_DUMMY_W_PMS_M  (PMS_DMA_AXI_PDMA_DUMMY_W_PMS_V << PMS_DMA_AXI_PDMA_DUMMY_W_PMS_S)
#define PMS_DMA_AXI_PDMA_DUMMY_W_PMS_V  0xFFFFFFFFU
#define PMS_DMA_AXI_PDMA_DUMMY_W_PMS_S  0

#ifdef __cplusplus
}
#endif
