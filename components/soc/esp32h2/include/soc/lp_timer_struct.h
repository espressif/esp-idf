/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    union {
        struct {
            uint32_t target_lo: 32;
        };
        uint32_t val;
    } lo;
    union {
        struct {
            uint32_t target_hi: 16;
            uint32_t reserved0: 15;
            uint32_t enable   : 1;
        };
        uint32_t val;
    } hi;
} lp_timer_target_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 28;
        uint32_t update   : 1;
        uint32_t xtal_off : 1;
        uint32_t sys_stall: 1;
        uint32_t sys_rst  : 1;
    };
    uint32_t val;
} lp_timer_update_reg_t;

typedef struct {
    union {
        struct {
            uint32_t counter_lo: 32;
        };
        uint32_t val;
    } lo;
    union {
        struct {
            uint32_t counter_hi: 16;
            uint32_t reserved0 : 16;
        };
        uint32_t val;
    } hi;
} lp_timer_counter_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 31;
        uint32_t trigger  : 1;
    };
    uint32_t val;
} lp_timer_overflow_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 30;
        uint32_t overflow : 1;
        uint32_t alarm    : 1;
    };
    uint32_t val;
} lp_timer_intr_reg_t;

typedef union {
    struct {
        uint32_t reserved0: 30;
        uint32_t overflow : 1;
        uint32_t alarm    : 1;
    };
    uint32_t val;
} lp_timer_lp_intr_reg_t;

typedef union {
    struct {
        uint32_t date  : 31;
        uint32_t clk_en: 1;
    };
    uint32_t val;
} lp_timer_date_clken_reg_t;

typedef struct lp_timer_dev_t{
    volatile lp_timer_target_reg_t          target[2];
    volatile lp_timer_update_reg_t          update;
    volatile lp_timer_counter_reg_t         counter[2];
    volatile lp_timer_overflow_reg_t        overflow;
    volatile lp_timer_intr_reg_t            int_raw;
    volatile lp_timer_intr_reg_t            int_st;
    volatile lp_timer_intr_reg_t            int_en;
    volatile lp_timer_intr_reg_t            int_clr;
    volatile lp_timer_lp_intr_reg_t         lp_int_raw;
    volatile lp_timer_lp_intr_reg_t         lp_int_st;
    volatile lp_timer_lp_intr_reg_t         lp_int_en;
    volatile lp_timer_lp_intr_reg_t         lp_int_clr;
    uint32_t reserved[237];
    volatile lp_timer_date_clken_reg_t      date_clken;
} lp_timer_dev_t;

extern lp_timer_dev_t LP_TIMER;

#ifndef __cplusplus
_Static_assert(sizeof(lp_timer_dev_t) == 0x400, "Invalid size of lp_timer_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
