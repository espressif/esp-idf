/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

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
 *  CPU_INTR_FROM_CPU_1 mapping register
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
 *  CPU_INTR_FROM_CPU_2 mapping register
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
 *  CPU_INTR_FROM_CPU_3 mapping register
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

#ifdef __cplusplus
}
#endif
