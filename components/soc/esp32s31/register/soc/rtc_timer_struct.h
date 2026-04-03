/**
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
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
} rtc_timer_target_reg_t;


/** Type of update register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:26;
        /** tickupdate : R/W; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t tickupdate:1;
        /** main_timer_update : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t main_timer_update:1;
        /** main_timer_regdma_work : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t main_timer_regdma_work:1;
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
} rtc_timer_update_reg_t;

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
} rtc_timer_counter_reg_t;

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
} rtc_timer_main_overflow_reg_t;

/** Type of int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_int_raw:1;
        /** updtae_done_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_raw:1;
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
} rtc_timer_int_raw_reg_t;

/** Type of int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_int_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_int_st:1;
        /** updtae_done_int_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_int_st:1;
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
} rtc_timer_int_st_reg_t;

/** Type of int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_int_ena : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_int_ena:1;
        /** updtae_done_int_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_int_ena:1;
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
} rtc_timer_int_ena_reg_t;

/** Type of int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_int_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_int_clr:1;
        /** updtae_done_int_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_int_clr:1;
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
} rtc_timer_int_clr_reg_t;

/** Type of lp_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_lp_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_lp_int_raw:1;
        /** updtae_done_lp_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_lp_int_raw:1;
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
} rtc_timer_lp_int_raw_reg_t;

/** Type of lp_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_lp_int_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_lp_int_st:1;
        /** updtae_done_int_lp_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_int_lp_st:1;
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
} rtc_timer_lp_int_st_reg_t;

/** Type of lp_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_lp_int_ena : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_lp_int_ena:1;
        /** updtae_done_lp_int_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_lp_int_ena:1;
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
} rtc_timer_lp_int_ena_reg_t;

/** Type of lp_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:28;
        /** tick_lp_int_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t tick_lp_int_clr:1;
        /** updtae_done_lp_int_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t updtae_done_lp_int_clr:1;
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
} rtc_timer_lp_int_clr_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 38830160;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} rtc_timer_date_reg_t;


typedef struct {
    volatile rtc_timer_target_reg_t target[2];
    volatile rtc_timer_update_reg_t update;
    volatile rtc_timer_counter_reg_t counter[2];
    volatile rtc_timer_main_overflow_reg_t main_overflow;
    volatile rtc_timer_int_raw_reg_t int_raw;
    volatile rtc_timer_int_st_reg_t int_st;
    volatile rtc_timer_int_ena_reg_t int_ena;
    volatile rtc_timer_int_clr_reg_t int_clr;
    volatile rtc_timer_lp_int_raw_reg_t lp_int_raw;
    volatile rtc_timer_lp_int_st_reg_t lp_int_st;
    volatile rtc_timer_lp_int_ena_reg_t lp_int_ena;
    volatile rtc_timer_lp_int_clr_reg_t lp_int_clr;
    uint32_t reserved_048[237];
    volatile rtc_timer_date_reg_t date;
} rtc_timer_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(rtc_timer_dev_t) == 0x400, "Invalid size of rtc_timer_dev_t structure");
#endif

extern rtc_timer_dev_t RTC_TIMER;

#ifdef __cplusplus
}
#endif
