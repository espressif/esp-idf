/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** SYSTEM TIMER CLK CONTROL REGISTER */
/** Type of conf register
 *  Configure system timer clock
 */
typedef union {
    struct {
        /** systimer_clk_fo : R/W; bitpos: [0]; default: 0;
         *  systimer clock force on
         */
        uint32_t systimer_clk_fo: 1;
        uint32_t reserved_1: 21;
        /** target2_work_en : R/W; bitpos: [22]; default: 0;
         *  target2 work enable
         */
        uint32_t target2_work_en: 1;
        /** target1_work_en : R/W; bitpos: [23]; default: 0;
         *  target1 work enable
         */
        uint32_t target1_work_en: 1;
        /** target0_work_en : R/W; bitpos: [24]; default: 0;
         *  target0 work enable
         */
        uint32_t target0_work_en: 1;
        /** timer_unit1_core1_stall_en : R/W; bitpos: [25]; default: 1;
         *  If timer unit1 is stalled when core1 stalled
         */
        uint32_t timer_unit1_core1_stall_en: 1;
        /** timer_unit1_core0_stall_en : R/W; bitpos: [26]; default: 1;
         *  If timer unit1 is stalled when core0 stalled
         */
        uint32_t timer_unit1_core0_stall_en: 1;
        /** timer_unit0_core1_stall_en : R/W; bitpos: [27]; default: 0;
         *  If timer unit0 is stalled when core1 stalled
         */
        uint32_t timer_unit0_core1_stall_en: 1;
        /** timer_unit0_core0_stall_en : R/W; bitpos: [28]; default: 0;
         *  If timer unit0 is stalled when core0 stalled
         */
        uint32_t timer_unit0_core0_stall_en: 1;
        /** timer_unit1_work_en : R/W; bitpos: [29]; default: 0;
         *  timer unit1 work enable
         */
        uint32_t timer_unit1_work_en: 1;
        /** timer_unit0_work_en : R/W; bitpos: [30]; default: 1;
         *  timer unit0 work enable
         */
        uint32_t timer_unit0_work_en: 1;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  register file clk gating
         */
        uint32_t clk_en: 1;
    };
    uint32_t val;
} systimer_conf_reg_t;


/** Type of unit_op register
 *  SYSTIMER_UNIT_OP.
 */
typedef union {
    struct {
        uint32_t reserved_0: 29;
        /** timer_unit_value_valid : R/SS/WTC; bitpos: [29]; default: 0;
         *  reg_timer_unit0_value_valid
         */
        uint32_t timer_unit_value_valid: 1;
        /** timer_unit_update : WT; bitpos: [30]; default: 0;
         *  update timer_unit0
         */
        uint32_t timer_unit_update: 1;
        uint32_t reserved_32: 1;
    };
    uint32_t val;
} systimer_unit_op_reg_t;

/** Type of unit_load register
 *  SYSTIMER_UNIT_LOAD
 */
typedef struct {
    union {
        struct {
            /** timer_unit_load_hi : R/W; bitpos: [19:0]; default: 0;
             *  timer unit load high 32 bit
             */
            uint32_t timer_unit_load_hi: 20;
            uint32_t reserved_20: 12;
        };
        uint32_t val;
    } hi;
    union {
        struct {
            /** timer_unit_load_lo : R/W; bitpos: [31:0]; default: 0;
             *  timer unit load low 32 bit
             */
            uint32_t timer_unit_load_lo: 32;
        };
        uint32_t val;
    } lo;
} systimer_unit_load_val_reg_t;


/** Type of target register
 *  SYSTIMER_TARGET.
 */
typedef struct {
    union {
        struct {
            /** timer_target_hi : R/W; bitpos: [19:0]; default: 0;
             *  timer target high 32 bit
             */
            uint32_t timer_target_hi: 20;
            uint32_t reserved_20: 12;
        };
        uint32_t val;
    } hi;
    union {
        struct {
            /** timer_target_lo : R/W; bitpos: [31:0]; default: 0;
             *  timer target low 32 bit
             */
            uint32_t timer_target_lo: 32;
        };
        uint32_t val;
    } lo;
} systimer_target_val_reg_t;


/** Type of target_conf register
 *  SYSTIMER_TARGET_CONF.
 */
typedef union {
    struct {
        /** target_period : R/W; bitpos: [25:0]; default: 0;
         *  target period
         */
        uint32_t target_period: 26;
        uint32_t reserved_26: 4;
        /** target_period_mode : R/W; bitpos: [30]; default: 0;
         *  Set target to period mode
         */
        uint32_t target_period_mode: 1;
        /** target_timer_unit_sel : R/W; bitpos: [31]; default: 0;
         *  select which unit to compare
         */
        uint32_t target_timer_unit_sel: 1;
    };
    uint32_t val;
} systimer_target_conf_reg_t;


/** Type of unit_value_hi register
 *  SYSTIMER_UNIT_VALUE_HI.
 */
typedef struct {
    union {
        struct {
            /** timer_unit_value_hi : RO; bitpos: [19:0]; default: 0;
             *  timer read value high 20bit
             */
            uint32_t timer_unit_value_hi: 20;
            uint32_t reserved_20: 12;
        };
        uint32_t val;
    } hi;
    union {
        struct {
            /** timer_unit_value_lo : RO; bitpos: [31:0]; default: 0;
             *  timer read value low 32bit
             */
            uint32_t timer_unit_value_lo: 32;
        };
        uint32_t val;
    } lo;
} systimer_unit_value_reg_t;


/** Type of comp_load register
 *  SYSTIMER_COMP_LOAD.
 */
typedef union {
    struct {
        /** timer_comp_load : WT; bitpos: [0]; default: 0;
         *  timer comp load value
         */
        uint32_t timer_comp_load: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} systimer_comp_load_reg_t;


/** Type of unit_load register
 *  SYSTIMER_UNIT_LOAD.
 */
typedef union {
    struct {
        /** timer_unit_load : WT; bitpos: [0]; default: 0;
         *  timer unit load value
         */
        uint32_t timer_unit_load: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} systimer_unit_load_reg_t;


/** SYSTEM TIMER INTERRUPT REGISTER */
/** Type of int_ena register
 *  systimer interrupt enable register
 */
typedef union {
    struct {
        /** target0_int_ena : R/W; bitpos: [0]; default: 0;
         *  interupt0 enable
         */
        uint32_t target0_int_ena: 1;
        /** target1_int_ena : R/W; bitpos: [1]; default: 0;
         *  interupt1 enable
         */
        uint32_t target1_int_ena: 1;
        /** target2_int_ena : R/W; bitpos: [2]; default: 0;
         *  interupt2 enable
         */
        uint32_t target2_int_ena: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} systimer_int_ena_reg_t;

/** Type of int_raw register
 *  systimer interrupt raw register
 */
typedef union {
    struct {
        /** target0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  interupt0 raw
         */
        uint32_t target0_int_raw: 1;
        /** target1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  interupt1 raw
         */
        uint32_t target1_int_raw: 1;
        /** target2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  interupt2 raw
         */
        uint32_t target2_int_raw: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} systimer_int_raw_reg_t;

/** Type of int_clr register
 *  systimer interrupt clear register
 */
typedef union {
    struct {
        /** target0_int_clr : WT; bitpos: [0]; default: 0;
         *  interupt0 clear
         */
        uint32_t target0_int_clr: 1;
        /** target1_int_clr : WT; bitpos: [1]; default: 0;
         *  interupt1 clear
         */
        uint32_t target1_int_clr: 1;
        /** target2_int_clr : WT; bitpos: [2]; default: 0;
         *  interupt2 clear
         */
        uint32_t target2_int_clr: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} systimer_int_clr_reg_t;

/** Type of int_st register
 *  systimer interrupt status register
 */
typedef union {
    struct {
        /** target0_int_st : RO; bitpos: [0]; default: 0;
         *  interupt0 status
         */
        uint32_t target0_int_st: 1;
        /** target1_int_st : RO; bitpos: [1]; default: 0;
         *  interupt1 status
         */
        uint32_t target1_int_st: 1;
        /** target2_int_st : RO; bitpos: [2]; default: 0;
         *  interupt2 status
         */
        uint32_t target2_int_st: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} systimer_int_st_reg_t;


/** SYSTEM TIMER COMP STATUS REGISTER
 *  systimer comp actual target value low register
 */
typedef struct {
    union {
        struct {
            /** target_lo_ro : RO; bitpos: [31:0]; default: 0;
             *  actual target value value low 32bits
             */
            uint32_t target_lo_ro: 32;
        };
        uint32_t val;
    } lo;
    union {
        struct {
            /** target_hi_ro : RO; bitpos: [19:0]; default: 0;
             *  actual target value value high 20bits
             */
            uint32_t target_hi_ro: 20;
            uint32_t reserved_20: 12;
        };
        uint32_t val;
    } hi;
} systimer_real_target_val_reg_t;


/** VERSION REGISTER */
/** Type of date register
 *  system timer version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 33628753;
         *  systimer register version
         */
        uint32_t date: 32;
    };
    uint32_t val;
} systimer_date_reg_t;


typedef struct systimer_dev_t {
    volatile systimer_conf_reg_t conf;
    volatile systimer_unit_op_reg_t unit_op[2];
    volatile systimer_unit_load_val_reg_t unit_load_val[2];
    volatile systimer_target_val_reg_t target_val[3];
    volatile systimer_target_conf_reg_t target_conf[3];
    volatile systimer_unit_value_reg_t unit_val[2];
    volatile systimer_comp_load_reg_t comp_load[3];
    volatile systimer_unit_load_reg_t unit_load[2];
    volatile systimer_int_ena_reg_t int_ena;
    volatile systimer_int_raw_reg_t int_raw;
    volatile systimer_int_clr_reg_t int_clr;
    volatile systimer_int_st_reg_t int_st;
    volatile systimer_real_target_val_reg_t real_target_val[3];
    uint32_t reserved_08c;
    uint32_t reserved_090;
    uint32_t reserved_094;
    uint32_t reserved_098;
    uint32_t reserved_09c;
    uint32_t reserved_0a0;
    uint32_t reserved_0a4;
    uint32_t reserved_0a8;
    uint32_t reserved_0ac;
    uint32_t reserved_0b0;
    uint32_t reserved_0b4;
    uint32_t reserved_0b8;
    uint32_t reserved_0bc;
    uint32_t reserved_0c0;
    uint32_t reserved_0c4;
    uint32_t reserved_0c8;
    uint32_t reserved_0cc;
    uint32_t reserved_0d0;
    uint32_t reserved_0d4;
    uint32_t reserved_0d8;
    uint32_t reserved_0dc;
    uint32_t reserved_0e0;
    uint32_t reserved_0e4;
    uint32_t reserved_0e8;
    uint32_t reserved_0ec;
    uint32_t reserved_0f0;
    uint32_t reserved_0f4;
    uint32_t reserved_0f8;
    volatile systimer_date_reg_t date;
} systimer_dev_t;

extern systimer_dev_t SYSTIMER;

#ifdef __cplusplus
}
#endif
