/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** SYSTEM TIMER REGISTER */
/** Type of conf register
 *  Configure system timer clock
 */
typedef union {
    struct {
        /** clk_fo : R/W; bitpos: [0]; default: 0;
         *  system timer force clock enable
         */
        uint32_t clk_fo: 1;
        uint32_t reserved_1: 30;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  register clock enable
         */
        uint32_t clk_en: 1;
    };
    uint32_t val;
} systimer_conf_reg_t;

/** Type of load register
 *  load value to system timer
 */
typedef union {
    struct {
        uint32_t reserved_0: 31;
        /** timer_load : WO; bitpos: [31]; default: 0;
         *  load value to system timer
         */
        uint32_t timer_load: 1;
    };
    uint32_t val;
} systimer_load_reg_t;

/** Type of load_hi register
 *  High 32-bit load to system timer
 */
typedef union {
    struct {
        /** timer_load_hi : R/W; bitpos: [31:0]; default: 0;
         *  High 32-bit load to system timer
         */
        uint32_t timer_load_hi: 32;
    };
    uint32_t val;
} systimer_load_hi_reg_t;

/** Type of load_lo register
 *  Low 32-bit load to system timer
 */
typedef union {
    struct {
        /** timer_load_lo : R/W; bitpos: [31:0]; default: 0;
         *  Low 32-bit load to system timer
         */
        uint32_t timer_load_lo: 32;
    };
    uint32_t val;
} systimer_load_lo_reg_t;

/** Type of step register
 *  system timer accumulation step
 */
typedef union {
    struct {
        /** timer_xtal_step : R/W; bitpos: [9:0]; default: 80;
         *  system timer accumulation step when using XTAL
         */
        uint32_t timer_xtal_step: 10;
        /** timer_pll_step : R/W; bitpos: [19:10]; default: 1;
         *  system timer accumulation step when using PLL
         */
        uint32_t timer_pll_step: 10;
        uint32_t reserved20: 12;
    };
    uint32_t val;
} systimer_step_reg_t;

/** Type of target_val register
 *  System timer target value
 */
typedef struct {
    union {
        struct {
            /** timer_target_hi : R/W; bitpos: [31:0]; default: 0;
             *  System timer target0 high 32-bit
             */
            uint32_t timer_target_hi: 32;
        };
        uint32_t val;
    } hi;
    union {
        struct {
            /** timer_target_lo : R/W; bitpos: [31:0]; default: 0;
             *  System timer target0 low 32-bit
             */
            uint32_t timer_target_lo: 32;
        };
        uint32_t val;
    } lo;
} systimer_target_val_reg_t;

/** Type of target_conf register
 *  Configure system timer target work mode
 */
typedef union {
    struct {
        /** target_period : R/W; bitpos: [29:0]; default: 0;
         *  System timer target alarm period
         */
        uint32_t target_period: 30;
        /** target_period_mode : R/W; bitpos: [30]; default: 0;
         *  Whether system timer target work in period mode
         */
        uint32_t target_period_mode: 1;
        /** target_work_en : R/W; bitpos: [31]; default: 0;
         *  system timer target work enable
         */
        uint32_t target_work_en: 1;
    };
    uint32_t val;
} systimer_target_conf_reg_t;

/** Type of update register
 *  Read out system timer value
 */
typedef union {
    struct {
        uint32_t reserved_0: 30;
        /** timer_value_valid : RO; bitpos: [30]; default: 0;
         *  If it is valid to read out timer value from register
         */
        uint32_t timer_value_valid: 1;
        /** timer_update : WO; bitpos: [31]; default: 0;
         *  Update system timer value to register
         */
        uint32_t timer_update: 1;
    };
    uint32_t val;
} systimer_update_reg_t;

/** Type of value_hi register
 *  system timer high 32-bit
 */
typedef union {
    struct {
        /** timer_value_hi : RO; bitpos: [31:0]; default: 0;
         *  system timer high 32-bit
         */
        uint32_t timer_value_hi: 32;
    };
    uint32_t val;
} systimer_value_hi_reg_t;

/** Type of value_lo register
 *  system timer low 32-bit
 */
typedef union {
    struct {
        /** timer_value_lo : RO; bitpos: [31:0]; default: 0;
         *  system timer low 32-bit
         */
        uint32_t timer_value_lo: 32;
    };
    uint32_t val;
} systimer_value_lo_reg_t;

/** Type of int_ena register
 *  system timer interrupt enable
 */
typedef union {
    struct {
        /** systimer_int0_ena : R/W; bitpos: [0]; default: 0;
         *  system timer target0 interrupt enable
         */
        uint32_t systimer_int0_ena: 1;
        /** systimer_int1_ena : R/W; bitpos: [1]; default: 0;
         *  system timer target1 interrupt enable
         */
        uint32_t systimer_int1_ena: 1;
        /** systimer_int2_ena : R/W; bitpos: [2]; default: 0;
         *  system timer target2 interrupt enable
         */
        uint32_t systimer_int2_ena: 1;
        uint32_t reserved3: 29;
    };
    uint32_t val;
} systimer_int_ena_reg_t;

/** Type of int_raw register
 *  system timer interrupt raw
 */
typedef union {
    struct {
        /** systimer_int0_raw : RO; bitpos: [0]; default: 0;
         *  system timer target0 interrupt raw
         */
        uint32_t systimer_int0_raw: 1;
        /** systimer_int1_raw : RO; bitpos: [1]; default: 0;
         *  system timer target1 interrupt raw
         */
        uint32_t systimer_int1_raw: 1;
        /** systimer_int2_raw : RO; bitpos: [2]; default: 0;
         *  system timer target2 interrupt raw
         */
        uint32_t systimer_int2_raw: 1;
        uint32_t reserved3: 29;
    };
    uint32_t val;
} systimer_int_raw_reg_t;

/** Type of int_clr register
 *  system timer interrupt clear
 */
typedef union {
    struct {
        /** systimer_int0_clr : WO; bitpos: [0]; default: 0;
         *  system timer target0 interrupt clear
         */
        uint32_t systimer_int0_clr: 1;
        /** systimer_int1_clr : WO; bitpos: [1]; default: 0;
         *  system timer target1 interrupt clear
         */
        uint32_t systimer_int1_clr: 1;
        /** systimer_int2_clr : WO; bitpos: [2]; default: 0;
         *  system timer target2 interrupt clear
         */
        uint32_t systimer_int2_clr: 1;
        uint32_t reserved3: 29;
    };
    uint32_t val;
} systimer_int_clr_reg_t;


/** DATE */
/** Type of date register
 *  system timer register version
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 25194848;
         *  system timer register version
         */
        uint32_t date: 32;
    };
    uint32_t val;
} systimer_date_reg_t;


typedef struct systimer_dev_t {
    volatile systimer_conf_reg_t conf;
    volatile systimer_load_reg_t load;
    volatile systimer_load_hi_reg_t load_hi;
    volatile systimer_load_lo_reg_t load_lo;
    volatile systimer_step_reg_t step;
    volatile systimer_target_val_reg_t target_val[3];
    volatile systimer_target_conf_reg_t target_conf[3];
    volatile systimer_update_reg_t update;
    volatile systimer_value_hi_reg_t value_hi;
    volatile systimer_value_lo_reg_t value_lo;
    volatile systimer_int_ena_reg_t int_ena;
    volatile systimer_int_raw_reg_t int_raw;
    volatile systimer_int_clr_reg_t int_clr;
    uint32_t reserved_050;
    uint32_t reserved_054;
    uint32_t reserved_058;
    uint32_t reserved_05c;
    uint32_t reserved_060;
    uint32_t reserved_064;
    uint32_t reserved_068;
    uint32_t reserved_06c;
    uint32_t reserved_070;
    uint32_t reserved_074;
    uint32_t reserved_078;
    uint32_t reserved_07c;
    uint32_t reserved_080;
    uint32_t reserved_084;
    uint32_t reserved_088;
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
