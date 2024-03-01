/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt Registers */
/** Type of cpu_intr_from_cpu_n register
 *  CPU_INTR_FROM_CPU_0 mapping register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_n : R/W; bitpos: [0]; default: 0;
         *  CPU_INTR_FROM_CPU_n mapping register.
         */
        uint32_t cpu_intr_from_cpu_n:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} intpri_cpu_intr_from_cpu_n_reg_t;


/** Group: Version Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36712784;
         *  Version control register.
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


/** Group: Redcy ECO Registers */
/** Type of rnd_eco register
 *  redcy eco register.
 */
typedef union {
    struct {
        /** redcy_ena : W/R; bitpos: [0]; default: 0;
         *  Only reserved for ECO.
         */
        uint32_t redcy_ena:1;
        /** redcy_result : RO; bitpos: [1]; default: 0;
         *  Only reserved for ECO.
         */
        uint32_t redcy_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} intpri_rnd_eco_reg_t;

/** Type of rnd_eco_low register
 *  redcy eco low register.
 */
typedef union {
    struct {
        /** redcy_low : W/R; bitpos: [31:0]; default: 0;
         *  Only reserved for ECO.
         */
        uint32_t redcy_low:32;
    };
    uint32_t val;
} intpri_rnd_eco_low_reg_t;

/** Type of rnd_eco_high register
 *  redcy eco high register.
 */
typedef union {
    struct {
        /** redcy_high : W/R; bitpos: [31:0]; default: 4294967295;
         *  Only reserved for ECO.
         */
        uint32_t redcy_high:32;
    };
    uint32_t val;
} intpri_rnd_eco_high_reg_t;


typedef struct {
    uint32_t reserved_000[36];
    volatile intpri_cpu_intr_from_cpu_n_reg_t cpu_intr_from_cpu_n[4];
    volatile intpri_date_reg_t date;
    volatile intpri_clock_gate_reg_t clock_gate;
    uint32_t reserved_0a8;
    volatile intpri_rnd_eco_reg_t rnd_eco;
    volatile intpri_rnd_eco_low_reg_t rnd_eco_low;
    uint32_t reserved_0b4[210];
    volatile intpri_rnd_eco_high_reg_t rnd_eco_high;
} intpri_dev_t;

extern intpri_dev_t INTPRI;

#ifndef __cplusplus
_Static_assert(sizeof(intpri_dev_t) == 0x400, "Invalid size of intpri_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
