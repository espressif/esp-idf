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

/** INTPRI_CPU_INTR_FROM_CPU_0_REG register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
#define INTPRI_CPU_INTR_FROM_CPU_0_REG (DR_REG_INTPRI_BASE + 0x90)
/** INTPRI_CPU_INTR_FROM_CPU_0 : R/W; bitpos: [0]; default: 0;
 *  CPU_INTR_FROM_CPU_0 mapping register.
 */
#define INTPRI_CPU_INTR_FROM_CPU_0    (BIT(0))
#define INTPRI_CPU_INTR_FROM_CPU_0_M  (INTPRI_CPU_INTR_FROM_CPU_0_V << INTPRI_CPU_INTR_FROM_CPU_0_S)
#define INTPRI_CPU_INTR_FROM_CPU_0_V  0x00000001U
#define INTPRI_CPU_INTR_FROM_CPU_0_S  0

/** INTPRI_CPU_INTR_FROM_CPU_1_REG register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
#define INTPRI_CPU_INTR_FROM_CPU_1_REG (DR_REG_INTPRI_BASE + 0x94)
/** INTPRI_CPU_INTR_FROM_CPU_1 : R/W; bitpos: [0]; default: 0;
 *  CPU_INTR_FROM_CPU_1 mapping register.
 */
#define INTPRI_CPU_INTR_FROM_CPU_1    (BIT(0))
#define INTPRI_CPU_INTR_FROM_CPU_1_M  (INTPRI_CPU_INTR_FROM_CPU_1_V << INTPRI_CPU_INTR_FROM_CPU_1_S)
#define INTPRI_CPU_INTR_FROM_CPU_1_V  0x00000001U
#define INTPRI_CPU_INTR_FROM_CPU_1_S  0

/** INTPRI_CPU_INTR_FROM_CPU_2_REG register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
#define INTPRI_CPU_INTR_FROM_CPU_2_REG (DR_REG_INTPRI_BASE + 0x98)
/** INTPRI_CPU_INTR_FROM_CPU_2 : R/W; bitpos: [0]; default: 0;
 *  CPU_INTR_FROM_CPU_2 mapping register.
 */
#define INTPRI_CPU_INTR_FROM_CPU_2    (BIT(0))
#define INTPRI_CPU_INTR_FROM_CPU_2_M  (INTPRI_CPU_INTR_FROM_CPU_2_V << INTPRI_CPU_INTR_FROM_CPU_2_S)
#define INTPRI_CPU_INTR_FROM_CPU_2_V  0x00000001U
#define INTPRI_CPU_INTR_FROM_CPU_2_S  0

/** INTPRI_CPU_INTR_FROM_CPU_3_REG register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
#define INTPRI_CPU_INTR_FROM_CPU_3_REG (DR_REG_INTPRI_BASE + 0x9c)
/** INTPRI_CPU_INTR_FROM_CPU_3 : R/W; bitpos: [0]; default: 0;
 *  CPU_INTR_FROM_CPU_3 mapping register.
 */
#define INTPRI_CPU_INTR_FROM_CPU_3    (BIT(0))
#define INTPRI_CPU_INTR_FROM_CPU_3_M  (INTPRI_CPU_INTR_FROM_CPU_3_V << INTPRI_CPU_INTR_FROM_CPU_3_S)
#define INTPRI_CPU_INTR_FROM_CPU_3_V  0x00000001U
#define INTPRI_CPU_INTR_FROM_CPU_3_S  0

/** INTPRI_DATE_REG register
 *  Version control register
 */
#define INTPRI_DATE_REG (DR_REG_INTPRI_BASE + 0xa0)
/** INTPRI_DATE : R/W; bitpos: [27:0]; default: 36712784;
 *  Version control register.
 */
#define INTPRI_DATE    0x0FFFFFFFU
#define INTPRI_DATE_M  (INTPRI_DATE_V << INTPRI_DATE_S)
#define INTPRI_DATE_V  0x0FFFFFFFU
#define INTPRI_DATE_S  0

/** INTPRI_CLOCK_GATE_REG register
 *  register description
 */
#define INTPRI_CLOCK_GATE_REG (DR_REG_INTPRI_BASE + 0xa4)
/** INTPRI_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  Need add description
 */
#define INTPRI_CLK_EN    (BIT(0))
#define INTPRI_CLK_EN_M  (INTPRI_CLK_EN_V << INTPRI_CLK_EN_S)
#define INTPRI_CLK_EN_V  0x00000001U
#define INTPRI_CLK_EN_S  0

/** INTPRI_RND_ECO_REG register
 *  redcy eco register.
 */
#define INTPRI_RND_ECO_REG (DR_REG_INTPRI_BASE + 0xac)
/** INTPRI_REDCY_ENA : W/R; bitpos: [0]; default: 0;
 *  Only reserved for ECO.
 */
#define INTPRI_REDCY_ENA    (BIT(0))
#define INTPRI_REDCY_ENA_M  (INTPRI_REDCY_ENA_V << INTPRI_REDCY_ENA_S)
#define INTPRI_REDCY_ENA_V  0x00000001U
#define INTPRI_REDCY_ENA_S  0
/** INTPRI_REDCY_RESULT : RO; bitpos: [1]; default: 0;
 *  Only reserved for ECO.
 */
#define INTPRI_REDCY_RESULT    (BIT(1))
#define INTPRI_REDCY_RESULT_M  (INTPRI_REDCY_RESULT_V << INTPRI_REDCY_RESULT_S)
#define INTPRI_REDCY_RESULT_V  0x00000001U
#define INTPRI_REDCY_RESULT_S  1

/** INTPRI_RND_ECO_LOW_REG register
 *  redcy eco low register.
 */
#define INTPRI_RND_ECO_LOW_REG (DR_REG_INTPRI_BASE + 0xb0)
/** INTPRI_REDCY_LOW : W/R; bitpos: [31:0]; default: 0;
 *  Only reserved for ECO.
 */
#define INTPRI_REDCY_LOW    0xFFFFFFFFU
#define INTPRI_REDCY_LOW_M  (INTPRI_REDCY_LOW_V << INTPRI_REDCY_LOW_S)
#define INTPRI_REDCY_LOW_V  0xFFFFFFFFU
#define INTPRI_REDCY_LOW_S  0

/** INTPRI_RND_ECO_HIGH_REG register
 *  redcy eco high register.
 */
#define INTPRI_RND_ECO_HIGH_REG (DR_REG_INTPRI_BASE + 0x3fc)
/** INTPRI_REDCY_HIGH : W/R; bitpos: [31:0]; default: 4294967295;
 *  Only reserved for ECO.
 */
#define INTPRI_REDCY_HIGH    0xFFFFFFFFU
#define INTPRI_REDCY_HIGH_M  (INTPRI_REDCY_HIGH_V << INTPRI_REDCY_HIGH_S)
#define INTPRI_REDCY_HIGH_V  0xFFFFFFFFU
#define INTPRI_REDCY_HIGH_S  0

#ifdef __cplusplus
}
#endif
