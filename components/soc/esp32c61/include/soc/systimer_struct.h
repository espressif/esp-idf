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
        /** systimer_clk_fo : R/W; bitpos: [0]; default: 0;
         *  systimer clock force on
         */
        uint32_t systimer_clk_fo:1;
        /** etm_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable generation of ETM events.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t etm_en:1;
        uint32_t reserved_2:20;
        /** target2_work_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable COMP2.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t target2_work_en:1;
        /** target1_work_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable COMP1. See details in SYSTIMER_TARGET2_WORK_EN.
         */
        uint32_t target1_work_en:1;
        /** target0_work_en : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to enable COMP0. See details in SYSTIMER_TARGET2_WORK_EN.
         */
        uint32_t target0_work_en:1;
        /** timer_unit1_core1_stall_en : R/W; bitpos: [25]; default: 1;
         *  Configures whether or not UNIT1 is stalled when CORE1 is stalled. \\
         *  0: UNIT1 is not stalled. \\
         *  1: UNIT1 is stalled.\\
         */
        uint32_t timer_unit1_core1_stall_en:1;
        /** timer_unit1_core0_stall_en : R/W; bitpos: [26]; default: 1;
         *  Configures whether or not UNIT1 is stalled when CORE0 is stalled. See details in
         *  SYSTIMER_TIMER_UNIT1_CORE1_STALL_EN.
         */
        uint32_t timer_unit1_core0_stall_en:1;
        /** timer_unit0_core1_stall_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not UNIT0 is stalled when CORE1 is stalled. See details in
         *  SYSTIMER_TIMER_UNIT1_CORE1_STALL_EN.
         */
        uint32_t timer_unit0_core1_stall_en:1;
        /** timer_unit0_core0_stall_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not UNIT0 is stalled when CORE0 is stalled. See details in
         *  SYSTIMER_TIMER_UNIT1_CORE1_STALL_EN.
         */
        uint32_t timer_unit0_core0_stall_en:1;
        /** timer_unit1_work_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable UNIT1. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t timer_unit1_work_en:1;
        /** timer_unit0_work_en : R/W; bitpos: [30]; default: 1;
         *  Configures whether or not to enable UNIT0. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t timer_unit0_work_en:1;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Configures register clock gating. \\
         *  0: Only enable needed clock for register read or write operations. \\
         *  1: Register clock is always enabled for read and write operations. \\
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} systimer_conf_reg_t;


/** Group: SYSTEM TIMER UNIT0 CONTROL AND CONFIGURATION REGISTER */
/** Type of unit0_op register
 *  Read UNIT0 value to registers
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** timer_unit0_value_valid : R/SS/WTC; bitpos: [29]; default: 0;
         *  Represents UNIT0 value is synchronized and valid.
         */
        uint32_t timer_unit_value_valid:1;
        /** timer_unit0_update : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to update timer UNIT0, i.e., reads the UNIT0 count value
         *  to SYSTIMER_TIMER_UNIT0_VALUE_HI and SYSTIMER_TIMER_UNIT0_VALUE_LO. \\
         *  0: No effect\\
         *  1: Update timer UNIT0 \\
         */
        uint32_t timer_unit_update:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} systimer_unit_op_reg_t;

/** Type of unit0_load_hi register
 *  High 20 bits to be loaded to UNIT0
 */
typedef union {
    struct {
        /** timer_unit0_load_hi : R/W; bitpos: [19:0]; default: 0;
         *  Configures the value to be loaded to UNIT0, high 20 bits.
         */
        uint32_t timer_unit_load_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit0_load_hi_reg_t;

/** Type of unit0_load_lo register
 *  Low 32 bits to be loaded to UNIT0
 */
typedef union {
    struct {
        /** timer_unit0_load_lo : R/W; bitpos: [31:0]; default: 0;
         *  Configures the value to be loaded to UNIT0, low 32 bits.
         */
        uint32_t timer_unit_load_lo:32;
    };
    uint32_t val;
} systimer_unit0_load_lo_reg_t;

/** Type of unit0_value_hi register
 *  UNIT0 value, high 20 bits
 */
typedef union {
    struct {
        /** timer_unit0_value_hi : RO; bitpos: [19:0]; default: 0;
         *  Represents UNIT0 read value, high 20 bits.
         */
        uint32_t timer_unit_value_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit0_value_hi_reg_t;

/** Type of unit0_value_lo register
 *  UNIT0 value, low 32 bits
 */
typedef union {
    struct {
        /** timer_unit0_value_lo : RO; bitpos: [31:0]; default: 0;
         *  Represents UNIT0 read value, low 32 bits.
         */
        uint32_t timer_unit_value_lo:32;
    };
    uint32_t val;
} systimer_unit0_value_lo_reg_t;

/** Type of unit0_load register
 *  UNIT0 synchronization register
 */
typedef union {
    struct {
        /** timer_unit0_load : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to reload the value of UNIT0, i.e., reloads the values of
         *  SYSTIMER_TIMER_UNIT0_VALUE_HI and SYSTIMER_TIMER_UNIT0_VALUE_LO to UNIT0. \\
         *  0: No effect \\
         *  1: Reload the value of UNIT0\\
         */
        uint32_t timer_unit0_load:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} systimer_unit_load_reg_t;


/** Group: SYSTEM TIMER UNIT1 CONTROL AND CONFIGURATION REGISTER */
/** Type of unit1_op register
 *  Read UNIT1 value to registers
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** timer_unit1_value_valid : R/SS/WTC; bitpos: [29]; default: 0;
         *  Represents UNIT1 value is synchronized and valid.
         */
        uint32_t timer_unit1_value_valid:1;
        /** timer_unit1_update : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to update timer UNIT1, i.e., reads the UNIT1 count value
         *  to SYSTIMER_TIMER_UNIT1_VALUE_HI and SYSTIMER_TIMER_UNIT1_VALUE_LO. \\
         *  0: No effect \\
         *  1: Update timer UNIT1\\
         */
        uint32_t timer_unit1_update:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} systimer_unit1_op_reg_t;

/** Type of unit1_load_hi register
 *  High 20 bits to be loaded to UNIT1systimer_unit1_load_reg_t
 */
typedef union {
    struct {
        /** timer_unit1_load_hi : R/W; bitpos: [19:0]; default: 0;
         *  Configures the value to be loaded to UNIT1, high 20 bits.
         */
        uint32_t timer_unit1_load_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit1_load_hi_reg_t;

/** Type of unit1_load_lo register
 *  Low 32 bits to be loaded to UNIT1
 */
typedef union {
    struct {
        /** timer_unit1_load_lo : R/W; bitpos: [31:0]; default: 0;
         *  Configures the value to be loaded to UNIT1, low 32 bits.
         */
        uint32_t timer_unit1_load_lo:32;
    };
    uint32_t val;
} systimer_unit1_load_lo_reg_t;

/** Type of unit1_value_hi register
 *  UNIT1 value, high 20 bits
 */
typedef union {
    struct {
        /** timer_unit1_value_hi : RO; bitpos: [19:0]; default: 0;
         *  Represents UNIT1 read value, high 20 bits.
         */
        uint32_t timer_unit1_value_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_unit1_value_hi_reg_t;

/** Type of unit1_value_lo register
 *  UNIT1 value, low 32 bits
 */
typedef union {
    struct {
        /** timer_unit1_value_lo : RO; bitpos: [31:0]; default: 0;
         *  Represents UNIT1 read value, low 32 bits.
         */
        uint32_t timer_unit1_value_lo:32;
    };
    uint32_t val;
} systimer_unit1_value_lo_reg_t;

/** Type of unit1_load register
 *  UNIT1 synchronization register
 */
typedef union {
    struct {
        /** timer_unit1_load : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to reload the value of UNIT1, i.e., reload the values of
         *  SYSTIMER_TIMER_UNIT1_VALUE_HI and SYSTIMER_TIMER_UNIT1_VALUE_LO to UNIT1. \\
         *  0: No effect \\
         *  1: Reload the value of UNIT1\\
         */
        uint32_t timer_unit1_load:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} systimer_unit1_load_reg_t;


/** Group: SYSTEM TIMER COMP0 CONTROL AND CONFIGURATION REGISTER */
/** Type of target0_hi register
 *  Alarm value to be loaded to COMP0, high 20 bits
 */
typedef union {
    struct {
        /** timer_target0_hi : R/W; bitpos: [19:0]; default: 0;
         *  Configures the alarm value to be loaded to COMP0, high 20 bits.
         */
        uint32_t timer_target_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_target0_hi_reg_t;

/** Type of target0_lo register
 *  Alarm value to be loaded to COMP0, low 32 bits
 */
typedef union {
    struct {
        /** timer_target0_lo : R/W; bitpos: [31:0]; default: 0;
         *  Configures the alarm value to be loaded to COMP0, low 32 bits.
         */
        uint32_t timer_target_lo:32;
    };
    uint32_t val;
} systimer_target0_lo_reg_t;

/** Type of target0_conf register
 *  Configure COMP0 alarm mode
 */
typedef union {
    struct {
        /** target_period : R/W; bitpos: [25:0]; default: 0;
         *  Configures COMP0 alarm period.
         */
        uint32_t target_period:26;
        uint32_t reserved_26:4;
        /** target_period_mode : R/W; bitpos: [30]; default: 0;
         *  Selects the two alarm modes for COMP0. \\
         *  0: Target mode\\
         *  1: Period mode\\
         */
        uint32_t target_period_mode:1;
        /** target_timer_unit_sel : R/W; bitpos: [31]; default: 0;
         *  Chooses the counter value for comparison with COMP0.\\
         *  0: Use the count value from UNIT$0\\
         *  1: Use the count value from UNIT$1\\
         */
        uint32_t target_timer_unit_sel:1;
    };
    uint32_t val;
} systimer_target_conf_reg_t;

/** Type of comp0_load register
 *  COMP0 synchronization register
 */
typedef union {
    struct {
        /** timer_comp0_load : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable COMP0 synchronization, i.e., reload the alarm
         *  value/period to COMP0.\\
         *  0: No effect \\
         *  1: Enable COMP0 synchronization\\
         */
        uint32_t timer_comp0_load:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} systimer_comp_load_reg_t;


/** Group: SYSTEM TIMER COMP1 CONTROL AND CONFIGURATION REGISTER */
/** Type of target1_hi register
 *  Alarm value to be loaded to COMP1, high 20 bits
 */
typedef union {
    struct {
        /** timer_target1_hi : R/W; bitpos: [19:0]; default: 0;
         *  Configures the alarm value to be loaded to COMP1, high 20 bits.
         */
        uint32_t timer_target1_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_target1_hi_reg_t;

/** Type of target1_lo register
 *  Alarm value to be loaded to COMP1, low 32 bits
 */
typedef union {
    struct {
        /** timer_target1_lo : R/W; bitpos: [31:0]; default: 0;
         *  Configures the alarm value to be loaded to COMP1, low 32 bits.
         */
        uint32_t timer_target1_lo:32;
    };
    uint32_t val;
} systimer_target1_lo_reg_t;

/** Type of target1_conf register
 *  Configure COMP1 alarm mode
 */
typedef union {
    struct {
        /** target1_period : R/W; bitpos: [25:0]; default: 0;
         *  Configures COMP1 alarm period.
         */
        uint32_t target1_period:26;
        uint32_t reserved_26:4;
        /** target1_period_mode : R/W; bitpos: [30]; default: 0;
         *  Selects the two alarm modes for COMP1. See details in SYSTIMER_TARGET0_PERIOD_MODE.
         */
        uint32_t target1_period_mode:1;
        /** target1_timer_unit_sel : R/W; bitpos: [31]; default: 0;
         *  Chooses the counter value for comparison with COMP1. See details in
         *  SYSTIMER_TARGET0_TIMER_UNIT_SEL.
         */
        uint32_t target1_timer_unit_sel:1;
    };
    uint32_t val;
} systimer_target1_conf_reg_t;

/** Type of comp1_load register
 *  COMP1 synchronization register
 */
typedef union {
    struct {
        /** timer_comp1_load : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable COMP1 synchronization, i.e., reload the alarm
         *  value/period to COMP1. \\
         *  0: No effect \\
         *  1: Enable COMP1 synchronization\\
         */
        uint32_t timer_comp1_load:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} systimer_comp1_load_reg_t;


/** Group: SYSTEM TIMER COMP2 CONTROL AND CONFIGURATION REGISTER */
/** Type of target2_hi register
 *  Alarm value to be loaded to COMP2, high 20 bits
 */
typedef union {
    struct {
        /** timer_target2_hi : R/W; bitpos: [19:0]; default: 0;
         *  Configures the alarm value to be loaded to COMP2, high 20 bits.
         */
        uint32_t timer_target2_hi:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_target2_hi_reg_t;

/** Type of target2_lo register
 *  Alarm value to be loaded to COMP2, low 32 bits
 */
typedef union {
    struct {
        /** timer_target2_lo : R/W; bitpos: [31:0]; default: 0;
         *  Configures the alarm value to be loaded to COMP2, low 32 bits.
         */
        uint32_t timer_target2_lo:32;
    };
    uint32_t val;
} systimer_target2_lo_reg_t;

/** Type of target2_conf register
 *  Configure COMP2 alarm mode
 */
typedef union {
    struct {
        /** target2_period : R/W; bitpos: [25:0]; default: 0;
         *  Configures COMP2 alarm period.
         */
        uint32_t target2_period:26;
        uint32_t reserved_26:4;
        /** target2_period_mode : R/W; bitpos: [30]; default: 0;
         *  Configures Configures the two alarm modes for COMP2. See details in
         *  SYSTIMER_TARGET0_PERIOD_MODE.
         */
        uint32_t target2_period_mode:1;
        /** target2_timer_unit_sel : R/W; bitpos: [31]; default: 0;
         *  Chooses the counter value for comparison with COMP2. See details in
         *  SYSTIMER_TARGET0_TIMER_UNIT_SEL.
         */
        uint32_t target2_timer_unit_sel:1;
    };
    uint32_t val;
} systimer_target2_conf_reg_t;

/** Type of comp2_load register
 *  COMP2 synchronization register
 */
typedef union {
    struct {
        /** timer_comp2_load : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable COMP2 synchronization, i.e., reload the alarm
         *  value/period to COMP2.\\
         *  0: No effect \\
         *  1: Enable COMP2 synchronization\\
         */
        uint32_t timer_comp2_load:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} systimer_comp2_load_reg_t;


/** Group: SYSTEM TIMER INTERRUPT REGISTER */
/** Type of int_ena register
 *  Interrupt enable register of system timer
 */
typedef union {
    struct {
        /** target0_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable SYSTIMER_TARGET0_INT.
         */
        uint32_t target0_int_ena:1;
        /** target1_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable SYSTIMER_TARGET1_INT.
         */
        uint32_t target1_int_ena:1;
        /** target2_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable SYSTIMER_TARGET2_INT.
         */
        uint32_t target2_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} systimer_int_ena_reg_t;

/** Type of int_raw register
 *  Interrupt raw register of system timer
 */
typedef union {
    struct {
        /** target0_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of SYSTIMER_TARGET0_INT.
         */
        uint32_t target0_int_raw:1;
        /** target1_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of SYSTIMER_TARGET1_INT.
         */
        uint32_t target1_int_raw:1;
        /** target2_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of SYSTIMER_TARGET2_INT.
         */
        uint32_t target2_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} systimer_int_raw_reg_t;

/** Type of int_clr register
 *  Interrupt clear register of system timer
 */
typedef union {
    struct {
        /** target0_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear SYSTIMER_TARGET0_INT.
         */
        uint32_t target0_int_clr:1;
        /** target1_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear SYSTIMER_TARGET1_INT.
         */
        uint32_t target1_int_clr:1;
        /** target2_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear SYSTIMER_TARGET2_INT.
         */
        uint32_t target2_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} systimer_int_clr_reg_t;

/** Type of int_st register
 *  Interrupt status register of system timer
 */
typedef union {
    struct {
        /** target0_int_st : RO; bitpos: [0]; default: 0;
         *  The interrupt status of SYSTIMER_TARGET0_INT.
         */
        uint32_t target0_int_st:1;
        /** target1_int_st : RO; bitpos: [1]; default: 0;
         *  The interrupt status of SYSTIMER_TARGET1_INT.
         */
        uint32_t target1_int_st:1;
        /** target2_int_st : RO; bitpos: [2]; default: 0;
         *  The interrupt status of SYSTIMER_TARGET2_INT.
         */
        uint32_t target2_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} systimer_int_st_reg_t;


/** Group: SYSTEM TIMER COMP0 STATUS REGISTER */
/** Type of real_target0_lo register
 *  Actual target value of COMP0, low 32 bits
 */
typedef union {
    struct {
        /** target0_lo_ro : RO; bitpos: [31:0]; default: 0;
         *  Represents the actual target value of COMP0, low 32 bits.
         */
        uint32_t target_lo_ro:32;
    };
    uint32_t val;
} systimer_real_target0_lo_reg_t;

/** Type of real_target0_hi register
 *  Actual target value of COMP0, high 20 bits
 */
typedef union {
    struct {
        /** target0_hi_ro : RO; bitpos: [19:0]; default: 0;
         *  Represents the actual target value of COMP0, high 20 bits.
         */
        uint32_t target_hi_ro:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_real_target0_hi_reg_t;


/** Group: SYSTEM TIMER COMP1 STATUS REGISTER */
/** Type of real_target1_lo register
 *  Actual target value of COMP1, low 32 bits
 */
typedef union {
    struct {
        /** target1_lo_ro : RO; bitpos: [31:0]; default: 0;
         *  Represents the actual target value of COMP1, low 32 bits.
         */
        uint32_t target1_lo_ro:32;
    };
    uint32_t val;
} systimer_real_target1_lo_reg_t;

/** Type of real_target1_hi register
 *  Actual target value of COMP1, high 20 bits
 */
typedef union {
    struct {
        /** target1_hi_ro : RO; bitpos: [19:0]; default: 0;
         *  Represents the actual target value of COMP1, high 20 bits.
         */
        uint32_t target1_hi_ro:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_real_target1_hi_reg_t;


/** Group: SYSTEM TIMER COMP2 STATUS REGISTER */
/** Type of real_target2_lo register
 *  Actual target value of COMP2, low 32 bits
 */
typedef union {
    struct {
        /** target2_lo_ro : RO; bitpos: [31:0]; default: 0;
         *  Represents the actual target value of COMP2, low 32 bits.
         */
        uint32_t target2_lo_ro:32;
    };
    uint32_t val;
} systimer_real_target2_lo_reg_t;

/** Type of real_target2_hi register
 *  Actual target value of COMP2, high 20 bits
 */
typedef union {
    struct {
        /** target2_hi_ro : RO; bitpos: [19:0]; default: 0;
         *  Represents the actual target value of COMP2, high 20 bits.
         */
        uint32_t target2_hi_ro:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} systimer_real_target2_hi_reg_t;


/** Group: VERSION REGISTER */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36774432;
         *  Version control register.
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
