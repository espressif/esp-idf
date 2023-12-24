/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt Registers */
/** Type of cpu_intr_from_cpu_0 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_0 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_0:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} intpri_cpu_intr_from_cpu_0_reg_t;

/** Type of cpu_intr_from_cpu_1 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_1 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} intpri_cpu_intr_from_cpu_1_reg_t;

/** Type of cpu_intr_from_cpu_2 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_2 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_2:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} intpri_cpu_intr_from_cpu_2_reg_t;

/** Type of cpu_intr_from_cpu_3 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_3 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} intpri_cpu_intr_from_cpu_3_reg_t;


/** Group: Version Registers */
/** Type of date register
 *  register description
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36712784;
         *  Need add description
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} intpri_date_reg_t;


/** Group: Configuration Registers */
/** Type of clock_gate register
 *  register description
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} intpri_clock_gate_reg_t;


typedef struct intpri_dev_t {
    uint32_t reserved_000[36];
    volatile intpri_cpu_intr_from_cpu_0_reg_t cpu_intr_from_cpu_0;
    volatile intpri_cpu_intr_from_cpu_1_reg_t cpu_intr_from_cpu_1;
    volatile intpri_cpu_intr_from_cpu_2_reg_t cpu_intr_from_cpu_2;
    volatile intpri_cpu_intr_from_cpu_3_reg_t cpu_intr_from_cpu_3;
    volatile intpri_date_reg_t date;
    volatile intpri_clock_gate_reg_t clock_gate;
} intpri_dev_t;

extern intpri_dev_t INTPRI;

#ifndef __cplusplus
_Static_assert(sizeof(intpri_dev_t) == 0xa8, "Invalid size of intpri_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
