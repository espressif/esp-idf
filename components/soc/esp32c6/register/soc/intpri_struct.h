/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Registers */
/** Type of core0_cpu_int_enable register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_int_enable : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_int_enable:32;
    };
    uint32_t val;
} intpri_core0_cpu_int_enable_reg_t;

/** Type of core0_cpu_int_type register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_int_type : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_int_type:32;
    };
    uint32_t val;
} intpri_core0_cpu_int_type_reg_t;

/** Type of core0_cpu_int_eip_status register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_int_eip_status : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_int_eip_status:32;
    };
    uint32_t val;
} intpri_core0_cpu_int_eip_status_reg_t;

/** Type of core0_cpu_int_pri_n register
 *  register description
 */
typedef union {
    struct {
        /** map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_n_reg_t;

/** Type of core0_cpu_int_thresh register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_int_thresh : R/W; bitpos: [7:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_int_thresh:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} intpri_core0_cpu_int_thresh_reg_t;

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

/** Type of core0_cpu_int_clear register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_int_clear : R/W; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_int_clear:32;
    };
    uint32_t val;
} intpri_core0_cpu_int_clear_reg_t;


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
        /** date : R/W; bitpos: [27:0]; default: 35655824;
         *  Need add description
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} intpri_date_reg_t;


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


typedef struct intpri_dev_t {
    volatile intpri_core0_cpu_int_enable_reg_t core0_cpu_int_enable;
    volatile intpri_core0_cpu_int_type_reg_t core0_cpu_int_type;
    volatile intpri_core0_cpu_int_eip_status_reg_t core0_cpu_int_eip_status;
    volatile intpri_core0_cpu_int_pri_n_reg_t core0_cpu_int_pri[32];
    volatile intpri_core0_cpu_int_thresh_reg_t core0_cpu_int_thresh;
    volatile intpri_cpu_intr_from_cpu_0_reg_t cpu_intr_from_cpu_0;
    volatile intpri_cpu_intr_from_cpu_1_reg_t cpu_intr_from_cpu_1;
    volatile intpri_cpu_intr_from_cpu_2_reg_t cpu_intr_from_cpu_2;
    volatile intpri_cpu_intr_from_cpu_3_reg_t cpu_intr_from_cpu_3;
    volatile intpri_date_reg_t date;
    volatile intpri_clock_gate_reg_t clock_gate;
    volatile intpri_core0_cpu_int_clear_reg_t core0_cpu_int_clear;
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
