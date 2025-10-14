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

/** Group: SYSTEM TIMER CLK CONTROL REGISTER */
/** Type of conf register
 *  Configure system timer clock
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** etm_en : R/W; bitpos: [1]; default: 0;
         *  enable systimer's etm task and event
         */
        uint32_t etm_en:1;
        uint32_t reserved_2:20;
        /** target2_work_en : R/W; bitpos: [22]; default: 0;
         *  target2 work enable
         */
        uint32_t target2_work_en:1;
        /** target1_work_en : R/W; bitpos: [23]; default: 0;
         *  target1 work enable
         */
        uint32_t target1_work_en:1;
        /** target0_work_en : R/W; bitpos: [24]; default: 0;
         *  target0 work enable
         */
        uint32_t target0_work_en:1;
        /** timer_unit1_core1_stall_en : R/W; bitpos: [25]; default: 1;
         *  If timer unit1 is stalled when core1 stalled
         */
        uint32_t timer_unit1_core1_stall_en:1;
        /** timer_unit1_core0_stall_en : R/W; bitpos: [26]; default: 1;
         *  If timer unit1 is stalled when core0 stalled
         */
        uint32_t timer_unit1_core0_stall_en:1;
        /** timer_unit0_core1_stall_en : R/W; bitpos: [27]; default: 0;
         *  If timer unit0 is stalled when core1 stalled
         */
        uint32_t timer_unit0_core1_stall_en:1;
        /** timer_unit0_core0_stall_en : R/W; bitpos: [28]; default: 0;
         *  If timer unit0 is stalled when core0 stalled
         */
        uint32_t timer_unit0_core0_stall_en:1;
        /** timer_unit1_work_en : R/W; bitpos: [29]; default: 0;
         *  timer unit1 work enable
         */
        uint32_t timer_unit1_work_en:1;
        /** timer_unit0_work_en : R/W; bitpos: [30]; default: 1;
         *  timer unit0 work enable
         */
        uint32_t timer_unit0_work_en:1;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  register file clk gating
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} systimer_conf_reg_t;


/** Group: SYSTEM TIMER UNIT0 CONTROL AND CONFIGURATION REGISTER */
/** Type of unit_op register
 *  system timer unit value update register
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** timer_unit_value_valid : R/SS/WTC; bitpos: [29]; default: 0;
         *  timer value is sync and valid
         */
        uint32_t timer_unit_value_valid:1;
        /** timer_unit_update : WT; bitpos: [30]; default: 0;
         *  update timer_unit0
         */
        uint32_t timer_unit_update:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} systimer_unit_op_reg_t;

/** Type of unit0_load_hi register
 *  system timer unit0 value high load register
 */
typedef union {
    struct {
        /** timer_unit0_load_hi : R/W; bitpos: [19:0]; default: 0;
         *  timer unit0 load high 20 bits
         */
        uint32_t timer_unit_load_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit0_load_hi_reg_t;

/** Type of unit0_load_lo register
 *  system timer unit0 value low load register
 */
typedef union {
    struct {
        /** timer_unit_load_lo : R/W; bitpos: [31:0]; default: 0;
         *  timer unit load low 32 bits
         */
        uint32_t timer_unit_load_lo:32;
    };
    uint32_t val;
} systimer_unit0_load_lo_reg_t;

/** Type of unit0_value_hi register
 *  system timer unit0 value high register
 */
typedef union {
    struct {
        /** timer_unit_value_hi : RO; bitpos: [19:0]; default: 0;
         *  timer read value high 20bits
         */
        uint32_t timer_unit_value_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit0_value_hi_reg_t;

/** Type of unit0_value_lo register
 *  system timer unit0 value low register
 */
typedef union {
    struct {
        /** timer_unit_value_lo : RO; bitpos: [31:0]; default: 0;
         *  timer read value low 32bits
         */
        uint32_t timer_unit_value_lo:32;
    };
    uint32_t val;
} systimer_unit0_value_lo_reg_t;

/** Type of unit_load register
 *  system timer unit conf sync register
 */
typedef union {
    struct {
        /** timer_unit_load : WT; bitpos: [0]; default: 0;
         *  timer unit sync enable signal
         */
        uint32_t timer_unit0_load:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} systimer_unit_load_reg_t;

/** Type of unit1_load_hi register
 *  system timer unit1 value high load register
 */
typedef union {
    struct {
        /** timer_unit1_load_hi : R/W; bitpos: [19:0]; default: 0;
         *  timer unit1 load high 20 bits
         */
        uint32_t timer_unit1_load_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit1_load_hi_reg_t;

/** Type of unit1_load_lo register
 *  system timer unit1 value low load register
 */
typedef union {
    struct {
        /** timer_unit1_load_lo : R/W; bitpos: [31:0]; default: 0;
         *  timer unit1 load low 32 bits
         */
        uint32_t timer_unit1_load_lo:32;
    };
    uint32_t val;
} systimer_unit1_load_lo_reg_t;

/** Type of unit1_value_hi register
 *  system timer unit1 value high register
 */
typedef union {
    struct {
        /** timer_unit1_value_hi : RO; bitpos: [19:0]; default: 0;
         *  timer read value high 20bits
         */
        uint32_t timer_unit1_value_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit1_value_hi_reg_t;

/** Type of unit1_value_lo register
 *  system timer unit1 value low register
 */
typedef union {
    struct {
        /** timer_unit1_value_lo : RO; bitpos: [31:0]; default: 0;
         *  timer read value low 32bits
         */
        uint32_t timer_unit1_value_lo:32;
    };
    uint32_t val;
} systimer_unit1_value_lo_reg_t;

/** Group: SYSTEM TIMER COMP0 CONTROL AND CONFIGURATION REGISTER */
/** Type of target0_hi register
 *  system timer comp0 value high register
 */
typedef union {
    struct {
        /** timer_target_hi : R/W; bitpos: [19:0]; default: 0;
         *  timer taget0 high 20 bits
         */
        uint32_t timer_target_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_target0_hi_reg_t;

/** Type of target0_lo register
 *  system timer comp0 value low register
 */
typedef union {
    struct {
        /** timer_target_lo : R/W; bitpos: [31:0]; default: 0;
         *  timer target low 32 bits
         */
        uint32_t timer_target_lo:32;
    };
    uint32_t val;
} systimer_target0_lo_reg_t;

/** Type of target_conf register
 *  system timer comp0 target mode register
 */
typedef union {
    struct {
        /** target_period : R/W; bitpos: [25:0]; default: 0;
         *  target period
         */
        uint32_t target_period:26;
        uint32_t reserved_26:4;
        /** target_period_mode : R/W; bitpos: [30]; default: 0;
         *  Set target to period mode
         */
        uint32_t target_period_mode:1;
        /** target0_timer_unit_sel : R/W; bitpos: [31]; default: 0;
         *  select which unit to compare
         */
        uint32_t target_timer_unit_sel:1;
    };
    uint32_t val;
} systimer_target_conf_reg_t;

/** Type of comp_load register
 *  system timer comp conf sync register
 */
typedef union {
    struct {
        /** timer_comp_load : WT; bitpos: [0]; default: 0;
         *  timer comp sync enable signal
         */
        uint32_t timer_comp0_load:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} systimer_comp_load_reg_t;


/** Group: SYSTEM TIMER COMP1 CONTROL AND CONFIGURATION REGISTER */
/** Type of target1_hi register
 *  system timer comp1 value high register
 */
typedef union {
    struct {
        /** timer_target1_hi : R/W; bitpos: [19:0]; default: 0;
         *  timer taget1 high 20 bits
         */
        uint32_t timer_target1_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_target1_hi_reg_t;

/** Type of target1_lo register
 *  system timer comp1 value low register
 */
typedef union {
    struct {
        /** timer_target1_lo : R/W; bitpos: [31:0]; default: 0;
         *  timer taget1 low 32 bits
         */
        uint32_t timer_target1_lo:32;
    };
    uint32_t val;
} systimer_target1_lo_reg_t;

/** Type of target1_conf register
 *  system timer comp1 target mode register
 */
typedef union {
    struct {
        /** target1_period : R/W; bitpos: [25:0]; default: 0;
         *  target1 period
         */
        uint32_t target1_period:26;
        uint32_t reserved_26:4;
        /** target1_period_mode : R/W; bitpos: [30]; default: 0;
         *  Set target1 to period mode
         */
        uint32_t target1_period_mode:1;
        /** target1_timer_unit_sel : R/W; bitpos: [31]; default: 0;
         *  select which unit to compare
         */
        uint32_t target1_timer_unit_sel:1;
    };
    uint32_t val;
} systimer_target1_conf_reg_t;

/** Group: SYSTEM TIMER COMP2 CONTROL AND CONFIGURATION REGISTER */
/** Type of target2_hi register
 *  system timer comp2 value high register
 */
typedef union {
    struct {
        /** timer_target2_hi : R/W; bitpos: [19:0]; default: 0;
         *  timer taget2 high 20 bits
         */
        uint32_t timer_target2_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_target2_hi_reg_t;

/** Type of target2_lo register
 *  system timer comp2 value low register
 */
typedef union {
    struct {
        /** timer_target2_lo : R/W; bitpos: [31:0]; default: 0;
         *  timer taget2 low 32 bits
         */
        uint32_t timer_target2_lo:32;
    };
    uint32_t val;
} systimer_target2_lo_reg_t;

/** Group: SYSTEM TIMER INTERRUPT REGISTER */
/** Type of int_ena register
 *  systimer interrupt enable register
 */
typedef union {
    struct {
        /** target0_int_ena : R/W; bitpos: [0]; default: 0;
         *  interupt0 enable
         */
        uint32_t target0_int_ena:1;
        /** target1_int_ena : R/W; bitpos: [1]; default: 0;
         *  interupt1 enable
         */
        uint32_t target1_int_ena:1;
        /** target2_int_ena : R/W; bitpos: [2]; default: 0;
         *  interupt2 enable
         */
        uint32_t target2_int_ena:1;
        uint32_t reserved_3:29;
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
        uint32_t target0_int_raw:1;
        /** target1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  interupt1 raw
         */
        uint32_t target1_int_raw:1;
        /** target2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  interupt2 raw
         */
        uint32_t target2_int_raw:1;
        uint32_t reserved_3:29;
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
        uint32_t target0_int_clr:1;
        /** target1_int_clr : WT; bitpos: [1]; default: 0;
         *  interupt1 clear
         */
        uint32_t target1_int_clr:1;
        /** target2_int_clr : WT; bitpos: [2]; default: 0;
         *  interupt2 clear
         */
        uint32_t target2_int_clr:1;
        uint32_t reserved_3:29;
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
        uint32_t target0_int_st:1;
        /** target1_int_st : RO; bitpos: [1]; default: 0;
         *  interupt1 status
         */
        uint32_t target1_int_st:1;
        /** target2_int_st : RO; bitpos: [2]; default: 0;
         *  interupt2 status
         */
        uint32_t target2_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} systimer_int_st_reg_t;


/** Group: SYSTEM TIMER COMP0 STATUS REGISTER */
/** Type of real_target0_lo register
 *  system timer comp0 actual target value low register
 */
typedef union {
    struct {
        /** target0_lo_ro : RO; bitpos: [31:0]; default: 0;
         *  actual target value value low 32bits
         */
        uint32_t target0_lo_ro:32;
    };
    uint32_t val;
} systimer_real_target0_lo_reg_t;

/** Type of real_target0_hi register
 *  system timer comp0 actual target value high register
 */
typedef union {
    struct {
        /** target0_hi_ro : RO; bitpos: [19:0]; default: 0;
         *  actual target value value high 20bits
         */
        uint32_t target0_hi_ro:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_real_target0_hi_reg_t;


/** Group: SYSTEM TIMER COMP1 STATUS REGISTER */
/** Type of real_target1_lo register
 *  system timer comp1 actual target value low register
 */
typedef union {
    struct {
        /** target1_lo_ro : RO; bitpos: [31:0]; default: 0;
         *  actual target value value low 32bits
         */
        uint32_t target1_lo_ro:32;
    };
    uint32_t val;
} systimer_real_target1_lo_reg_t;

/** Type of real_target1_hi register
 *  system timer comp1 actual target value high register
 */
typedef union {
    struct {
        /** target1_hi_ro : RO; bitpos: [19:0]; default: 0;
         *  actual target value value high 20bits
         */
        uint32_t target1_hi_ro:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_real_target1_hi_reg_t;


/** Group: SYSTEM TIMER COMP2 STATUS REGISTER */
/** Type of real_target2_lo register
 *  system timer comp2 actual target value low register
 */
typedef union {
    struct {
        /** target2_lo_ro : RO; bitpos: [31:0]; default: 0;
         *  actual target value value low 32bits
         */
        uint32_t target2_lo_ro:32;
    };
    uint32_t val;
} systimer_real_target2_lo_reg_t;

/** Type of real_target2_hi register
 *  system timer comp2 actual target value high register
 */
typedef union {
    struct {
        /** target2_hi_ro : RO; bitpos: [19:0]; default: 0;
         *  actual target value value high 20bits
         */
        uint32_t target2_hi_ro:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_real_target2_hi_reg_t;


/** Group: VERSION REGISTER */
/** Type of date register
 *  system timer version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 35655795;
         *  systimer register version
         */
        uint32_t date:32;
    };
    uint32_t val;
} systimer_date_reg_t;

typedef struct systimer_unit_load_val_reg
{
    systimer_unit0_load_hi_reg_t hi;
    systimer_unit0_load_lo_reg_t lo;
} systimer_unit_load_val_reg_t;

typedef struct systimer_target_val_reg
{
    systimer_target0_hi_reg_t hi;
    systimer_target0_lo_reg_t lo;
} systimer_target_val_reg_t;

typedef struct systimer_unit_value_reg
{
    systimer_unit0_value_hi_reg_t hi;
    systimer_unit0_value_lo_reg_t lo;
} systimer_unit_value_reg_t;

typedef struct systimer_real_target_reg
{
    systimer_real_target0_lo_reg_t lo;
    systimer_real_target0_hi_reg_t hi;
} systimer_real_target_reg_t;

typedef struct systimer_dev_t{
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
    volatile systimer_real_target_reg_t real_target[3];
    uint32_t reserved_08c[28];
    volatile systimer_date_reg_t date;
} systimer_dev_t;

extern systimer_dev_t SYSTIMER;

#ifndef __cplusplus
_Static_assert(sizeof(systimer_dev_t) == 0x100, "Invalid size of systimer_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
