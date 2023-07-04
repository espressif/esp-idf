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

/** Group: configure_register */
/** Type of tar0_low register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_tar_low0 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_tar_low0:32;
    };
    uint32_t val;
} lp_timer_tar0_low_reg_t;

/** Type of tar0_high register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_tar_high0 : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_tar_high0:16;
        uint32_t reserved_16:15;
        /** main_timer_tar_en0 : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_tar_en0:1;
    };
    uint32_t val;
} lp_timer_tar0_high_reg_t;

/** Type of tar1_low register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_tar_low1 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_tar_low1:32;
    };
    uint32_t val;
} lp_timer_tar1_low_reg_t;

/** Type of tar1_high register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_tar_high1 : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_tar_high1:16;
        uint32_t reserved_16:15;
        /** main_timer_tar_en1 : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_tar_en1:1;
    };
    uint32_t val;
} lp_timer_tar1_high_reg_t;

/** Type of update register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** main_timer_update : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t main_timer_update:1;
        /** main_timer_xtal_off : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t main_timer_xtal_off:1;
        /** main_timer_sys_stall : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t main_timer_sys_stall:1;
        /** main_timer_sys_rst : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_sys_rst:1;
    };
    uint32_t val;
} lp_timer_update_reg_t;

/** Type of main_buf0_low register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_buf0_low : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_buf0_low:32;
    };
    uint32_t val;
} lp_timer_main_buf0_low_reg_t;

/** Type of main_buf0_high register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_buf0_high : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_buf0_high:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_timer_main_buf0_high_reg_t;

/** Type of main_buf1_low register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_buf1_low : RO; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_buf1_low:32;
    };
    uint32_t val;
} lp_timer_main_buf1_low_reg_t;

/** Type of main_buf1_high register
 *  need_des
 */
typedef union {
    struct {
        /** main_timer_buf1_high : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t main_timer_buf1_high:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_timer_main_buf1_high_reg_t;

/** Type of main_overflow register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** main_timer_alarm_load : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_alarm_load:1;
    };
    uint32_t val;
} lp_timer_main_overflow_reg_t;

/** Type of int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** overflow_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t overflow_raw:1;
        /** soc_wakeup_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_raw:1;
    };
    uint32_t val;
} lp_timer_int_raw_reg_t;

/** Type of int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** overflow_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t overflow_st:1;
        /** soc_wakeup_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_st:1;
    };
    uint32_t val;
} lp_timer_int_st_reg_t;

/** Type of int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** overflow_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t overflow_ena:1;
        /** soc_wakeup_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_ena:1;
    };
    uint32_t val;
} lp_timer_int_ena_reg_t;

/** Type of int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** overflow_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t overflow_clr:1;
        /** soc_wakeup_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t soc_wakeup_int_clr:1;
    };
    uint32_t val;
} lp_timer_int_clr_reg_t;

/** Type of lp_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** main_timer_overflow_lp_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t main_timer_overflow_lp_int_raw:1;
        /** main_timer_lp_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_lp_int_raw:1;
    };
    uint32_t val;
} lp_timer_lp_int_raw_reg_t;

/** Type of lp_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** main_timer_overflow_lp_int_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t main_timer_overflow_lp_int_st:1;
        /** main_timer_lp_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_lp_int_st:1;
    };
    uint32_t val;
} lp_timer_lp_int_st_reg_t;

/** Type of lp_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** main_timer_overflow_lp_int_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t main_timer_overflow_lp_int_ena:1;
        /** main_timer_lp_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_lp_int_ena:1;
    };
    uint32_t val;
} lp_timer_lp_int_ena_reg_t;

/** Type of lp_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** main_timer_overflow_lp_int_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t main_timer_overflow_lp_int_clr:1;
        /** main_timer_lp_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t main_timer_lp_int_clr:1;
    };
    uint32_t val;
} lp_timer_lp_int_clr_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 34672976;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_timer_date_reg_t;


typedef struct {
    volatile lp_timer_tar0_low_reg_t tar0_low;
    volatile lp_timer_tar0_high_reg_t tar0_high;
    volatile lp_timer_tar1_low_reg_t tar1_low;
    volatile lp_timer_tar1_high_reg_t tar1_high;
    volatile lp_timer_update_reg_t update;
    volatile lp_timer_main_buf0_low_reg_t main_buf0_low;
    volatile lp_timer_main_buf0_high_reg_t main_buf0_high;
    volatile lp_timer_main_buf1_low_reg_t main_buf1_low;
    volatile lp_timer_main_buf1_high_reg_t main_buf1_high;
    volatile lp_timer_main_overflow_reg_t main_overflow;
    volatile lp_timer_int_raw_reg_t int_raw;
    volatile lp_timer_int_st_reg_t int_st;
    volatile lp_timer_int_ena_reg_t int_ena;
    volatile lp_timer_int_clr_reg_t int_clr;
    volatile lp_timer_lp_int_raw_reg_t lp_int_raw;
    volatile lp_timer_lp_int_st_reg_t lp_int_st;
    volatile lp_timer_lp_int_ena_reg_t lp_int_ena;
    volatile lp_timer_lp_int_clr_reg_t lp_int_clr;
    uint32_t reserved_048[237];
    volatile lp_timer_date_reg_t date;
} lp_timer_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_timer_dev_t) == 0x400, "Invalid size of lp_timer_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
