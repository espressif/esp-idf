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

/** Group: configure_register */
/** Type of timer_tar0_low register
 *  need_des
 */
typedef union {
    struct {
        /** timer_main_timer_tar_low0 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_tar_low0:32;
    };
    uint32_t val;
} lp_timer_tar0_low_reg_t;

/** Type of timer_tar0_high register
 *  need_des
 */
typedef union {
    struct {
        /** timer_main_timer_tar_high0 : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_tar_high0:16;
        uint32_t reserved_16:15;
        /** timer_main_timer_tar_en0 : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_tar_en0:1;
    };
    uint32_t val;
} lp_timer_tar0_high_reg_t;

/** Type of timer_update register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** timer_main_timer_update : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_update:1;
        /** timer_main_timer_regdma_work : R/W; bitpos: [28]; default: 0;
         *  Selects the triggering condition for the RTC timer,triggered when regdma working
         */
        uint32_t timer_main_timer_regdma_work:1;
        /** timer_main_timer_xtal_off : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_xtal_off:1;
        /** timer_main_timer_sys_stall : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_sys_stall:1;
        /** timer_main_timer_sys_rst : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_sys_rst:1;
    };
    uint32_t val;
} lp_timer_update_reg_t;

/** Type of timer_main_buf0_low register
 *  need_des
 */
typedef union {
    struct {
        /** timer_main_timer_buf0_low : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_buf0_low:32;
    };
    uint32_t val;
} lp_timer_main_buf0_low_reg_t;

/** Type of timer_main_buf0_high register
 *  need_des
 */
typedef union {
    struct {
        /** timer_main_timer_buf0_high : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_buf0_high:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_timer_main_buf0_high_reg_t;

/** Type of timer_main_buf1_low register
 *  need_des
 */
typedef union {
    struct {
        /** timer_main_timer_buf1_low : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_buf1_low:32;
    };
    uint32_t val;
} lp_timer_main_buf1_low_reg_t;

/** Type of timer_main_buf1_high register
 *  need_des
 */
typedef union {
    struct {
        /** timer_main_timer_buf1_high : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_buf1_high:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_timer_main_buf1_high_reg_t;

/** Type of timer_main_overflow register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** timer_main_timer_alarm_load : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_main_timer_alarm_load:1;
    };
    uint32_t val;
} lp_timer_main_overflow_reg_t;

/** Type of timer_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** timer_overflow_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t timer_overflow_raw:1;
        /** timer_soc_wakeup_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_soc_wakeup_int_raw:1;
    };
    uint32_t val;
} lp_timer_int_raw_reg_t;

/** Type of timer_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** timer_overflow_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t timer_overflow_st:1;
        /** timer_soc_wakeup_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_soc_wakeup_int_st:1;
    };
    uint32_t val;
} lp_timer_int_st_reg_t;

/** Type of timer_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** timer_overflow_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t timer_overflow_ena:1;
        /** timer_soc_wakeup_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_soc_wakeup_int_ena:1;
    };
    uint32_t val;
} lp_timer_int_ena_reg_t;

/** Type of timer_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** timer_overflow_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t timer_overflow_clr:1;
        /** timer_soc_wakeup_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_soc_wakeup_int_clr:1;
    };
    uint32_t val;
} lp_timer_int_clr_reg_t;

/** Type of timer_date register
 *  need_des
 */
typedef union {
    struct {
        /** timer_date : R/W; bitpos: [30:0]; default: 37785904;
         *  need_des
         */
        uint32_t timer_date:31;
        /** timer_clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t timer_clk_en:1;
    };
    uint32_t val;
} lp_timer_date_reg_t;


typedef struct {
    volatile lp_timer_tar0_low_reg_t timer_tar0_low;
    volatile lp_timer_tar0_high_reg_t timer_tar0_high;
    uint32_t reserved_008[2];
    volatile lp_timer_update_reg_t timer_update;
    volatile lp_timer_main_buf0_low_reg_t timer_main_buf0_low;
    volatile lp_timer_main_buf0_high_reg_t timer_main_buf0_high;
    volatile lp_timer_main_buf1_low_reg_t timer_main_buf1_low;
    volatile lp_timer_main_buf1_high_reg_t timer_main_buf1_high;
    volatile lp_timer_main_overflow_reg_t timer_main_overflow;
    volatile lp_timer_int_raw_reg_t timer_int_raw;
    volatile lp_timer_int_st_reg_t timer_int_st;
    volatile lp_timer_int_ena_reg_t timer_int_ena;
    volatile lp_timer_int_clr_reg_t timer_int_clr;
    uint32_t reserved_038[241];
    volatile lp_timer_date_reg_t timer_date;
} lp_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_dev_t) == 0x400, "Invalid size of lp_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
