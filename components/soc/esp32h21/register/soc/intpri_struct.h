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

/** Type of core0_cpu_int_pri_0 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_0_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_0_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_0_reg_t;

/** Type of core0_cpu_int_pri_1 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_1_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_1_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_1_reg_t;

/** Type of core0_cpu_int_pri_2 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_2_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_2_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_2_reg_t;

/** Type of core0_cpu_int_pri_3 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_3_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_3_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_3_reg_t;

/** Type of core0_cpu_int_pri_4 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_4_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_4_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_4_reg_t;

/** Type of core0_cpu_int_pri_5 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_5_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_5_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_5_reg_t;

/** Type of core0_cpu_int_pri_6 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_6_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_6_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_6_reg_t;

/** Type of core0_cpu_int_pri_7 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_7_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_7_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_7_reg_t;

/** Type of core0_cpu_int_pri_8 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_8_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_8_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_8_reg_t;

/** Type of core0_cpu_int_pri_9 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_9_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_9_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_9_reg_t;

/** Type of core0_cpu_int_pri_10 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_10_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_10_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_10_reg_t;

/** Type of core0_cpu_int_pri_11 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_11_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_11_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_11_reg_t;

/** Type of core0_cpu_int_pri_12 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_12_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_12_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_12_reg_t;

/** Type of core0_cpu_int_pri_13 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_13_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_13_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_13_reg_t;

/** Type of core0_cpu_int_pri_14 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_14_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_14_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_14_reg_t;

/** Type of core0_cpu_int_pri_15 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_15_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_15_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_15_reg_t;

/** Type of core0_cpu_int_pri_16 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_16_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_16_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_16_reg_t;

/** Type of core0_cpu_int_pri_17 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_17_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_17_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_17_reg_t;

/** Type of core0_cpu_int_pri_18 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_18_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_18_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_18_reg_t;

/** Type of core0_cpu_int_pri_19 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_19_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_19_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_19_reg_t;

/** Type of core0_cpu_int_pri_20 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_20_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_20_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_20_reg_t;

/** Type of core0_cpu_int_pri_21 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_21_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_21_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_21_reg_t;

/** Type of core0_cpu_int_pri_22 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_22_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_22_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_22_reg_t;

/** Type of core0_cpu_int_pri_23 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_23_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_23_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_23_reg_t;

/** Type of core0_cpu_int_pri_24 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_24_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_24_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_24_reg_t;

/** Type of core0_cpu_int_pri_25 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_25_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_25_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_25_reg_t;

/** Type of core0_cpu_int_pri_26 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_26_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_26_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_26_reg_t;

/** Type of core0_cpu_int_pri_27 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_27_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_27_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_27_reg_t;

/** Type of core0_cpu_int_pri_28 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_28_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_28_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_28_reg_t;

/** Type of core0_cpu_int_pri_29 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_29_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_29_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_29_reg_t;

/** Type of core0_cpu_int_pri_30 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_30_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_30_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_30_reg_t;

/** Type of core0_cpu_int_pri_31 register
 *  register description
 */
typedef union {
    struct {
        /** core0_cpu_pri_31_map : R/W; bitpos: [3:0]; default: 0;
         *  Need add description
         */
        uint32_t core0_cpu_pri_31_map:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} intpri_core0_cpu_int_pri_31_reg_t;

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
        /** date : R/W; bitpos: [27:0]; default: 35660416;
         *  Need add description
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} intpri_date_reg_t;


typedef struct {
    volatile intpri_core0_cpu_int_enable_reg_t core0_cpu_int_enable;
    volatile intpri_core0_cpu_int_type_reg_t core0_cpu_int_type;
    volatile intpri_core0_cpu_int_eip_status_reg_t core0_cpu_int_eip_status;
    volatile intpri_core0_cpu_int_pri_0_reg_t core0_cpu_int_pri_0;
    volatile intpri_core0_cpu_int_pri_1_reg_t core0_cpu_int_pri_1;
    volatile intpri_core0_cpu_int_pri_2_reg_t core0_cpu_int_pri_2;
    volatile intpri_core0_cpu_int_pri_3_reg_t core0_cpu_int_pri_3;
    volatile intpri_core0_cpu_int_pri_4_reg_t core0_cpu_int_pri_4;
    volatile intpri_core0_cpu_int_pri_5_reg_t core0_cpu_int_pri_5;
    volatile intpri_core0_cpu_int_pri_6_reg_t core0_cpu_int_pri_6;
    volatile intpri_core0_cpu_int_pri_7_reg_t core0_cpu_int_pri_7;
    volatile intpri_core0_cpu_int_pri_8_reg_t core0_cpu_int_pri_8;
    volatile intpri_core0_cpu_int_pri_9_reg_t core0_cpu_int_pri_9;
    volatile intpri_core0_cpu_int_pri_10_reg_t core0_cpu_int_pri_10;
    volatile intpri_core0_cpu_int_pri_11_reg_t core0_cpu_int_pri_11;
    volatile intpri_core0_cpu_int_pri_12_reg_t core0_cpu_int_pri_12;
    volatile intpri_core0_cpu_int_pri_13_reg_t core0_cpu_int_pri_13;
    volatile intpri_core0_cpu_int_pri_14_reg_t core0_cpu_int_pri_14;
    volatile intpri_core0_cpu_int_pri_15_reg_t core0_cpu_int_pri_15;
    volatile intpri_core0_cpu_int_pri_16_reg_t core0_cpu_int_pri_16;
    volatile intpri_core0_cpu_int_pri_17_reg_t core0_cpu_int_pri_17;
    volatile intpri_core0_cpu_int_pri_18_reg_t core0_cpu_int_pri_18;
    volatile intpri_core0_cpu_int_pri_19_reg_t core0_cpu_int_pri_19;
    volatile intpri_core0_cpu_int_pri_20_reg_t core0_cpu_int_pri_20;
    volatile intpri_core0_cpu_int_pri_21_reg_t core0_cpu_int_pri_21;
    volatile intpri_core0_cpu_int_pri_22_reg_t core0_cpu_int_pri_22;
    volatile intpri_core0_cpu_int_pri_23_reg_t core0_cpu_int_pri_23;
    volatile intpri_core0_cpu_int_pri_24_reg_t core0_cpu_int_pri_24;
    volatile intpri_core0_cpu_int_pri_25_reg_t core0_cpu_int_pri_25;
    volatile intpri_core0_cpu_int_pri_26_reg_t core0_cpu_int_pri_26;
    volatile intpri_core0_cpu_int_pri_27_reg_t core0_cpu_int_pri_27;
    volatile intpri_core0_cpu_int_pri_28_reg_t core0_cpu_int_pri_28;
    volatile intpri_core0_cpu_int_pri_29_reg_t core0_cpu_int_pri_29;
    volatile intpri_core0_cpu_int_pri_30_reg_t core0_cpu_int_pri_30;
    volatile intpri_core0_cpu_int_pri_31_reg_t core0_cpu_int_pri_31;
    volatile intpri_core0_cpu_int_thresh_reg_t core0_cpu_int_thresh;
    volatile intpri_cpu_intr_from_cpu_0_reg_t cpu_intr_from_cpu_0;
    volatile intpri_cpu_intr_from_cpu_1_reg_t cpu_intr_from_cpu_1;
    volatile intpri_cpu_intr_from_cpu_2_reg_t cpu_intr_from_cpu_2;
    volatile intpri_cpu_intr_from_cpu_3_reg_t cpu_intr_from_cpu_3;
    volatile intpri_date_reg_t date;
    volatile intpri_clock_gate_reg_t clock_gate;
    volatile intpri_core0_cpu_int_clear_reg_t core0_cpu_int_clear;
} intpri_dev_t;

extern intpri_dev_t INTPRI;

#ifndef __cplusplus
_Static_assert(sizeof(intpri_dev_t) == 0xac, "Invalid size of intpri_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
