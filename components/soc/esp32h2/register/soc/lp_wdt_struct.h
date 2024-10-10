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

/** Group: configure_register */
/** Type of config0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:9;
        /** wdt_pause_in_slp : R/W; bitpos: [9]; default: 1;
         *  need_des
         */
        uint32_t wdt_pause_in_slp:1;
        /** wdt_appcpu_reset_en : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t wdt_appcpu_reset_en:1;
        /** wdt_procpu_reset_en : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t wdt_procpu_reset_en:1;
        /** wdt_flashboot_mod_en : R/W; bitpos: [12]; default: 1;
         *  need_des
         */
        uint32_t wdt_flashboot_mod_en:1;
        /** wdt_sys_reset_length : R/W; bitpos: [15:13]; default: 1;
         *  need_des
         */
        uint32_t wdt_sys_reset_length:3;
        /** wdt_cpu_reset_length : R/W; bitpos: [18:16]; default: 1;
         *  need_des
         */
        uint32_t wdt_cpu_reset_length:3;
        /** wdt_stg3 : R/W; bitpos: [21:19]; default: 0;
         *  need_des
         */
        uint32_t wdt_stg3:3;
        /** wdt_stg2 : R/W; bitpos: [24:22]; default: 0;
         *  need_des
         */
        uint32_t wdt_stg2:3;
        /** wdt_stg1 : R/W; bitpos: [27:25]; default: 0;
         *  need_des
         */
        uint32_t wdt_stg1:3;
        /** wdt_stg0 : R/W; bitpos: [30:28]; default: 0;
         *  need_des
         */
        uint32_t wdt_stg0:3;
        /** wdt_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t wdt_en:1;
    };
    uint32_t val;
} lp_wdt_config0_reg_t;

/** Type of config1 register
 *  need_des
 */
typedef union {
    struct {
        /** wdt_stg0_hold : R/W; bitpos: [31:0]; default: 200000;
         *  need_des
         */
        uint32_t wdt_stg0_hold:32;
    };
    uint32_t val;
} lp_wdt_config1_reg_t;

/** Type of config2 register
 *  need_des
 */
typedef union {
    struct {
        /** wdt_stg1_hold : R/W; bitpos: [31:0]; default: 80000;
         *  need_des
         */
        uint32_t wdt_stg1_hold:32;
    };
    uint32_t val;
} lp_wdt_config2_reg_t;

/** Type of config3 register
 *  need_des
 */
typedef union {
    struct {
        /** wdt_stg2_hold : R/W; bitpos: [31:0]; default: 4095;
         *  need_des
         */
        uint32_t wdt_stg2_hold:32;
    };
    uint32_t val;
} lp_wdt_config3_reg_t;

/** Type of config4 register
 *  need_des
 */
typedef union {
    struct {
        /** wdt_stg3_hold : R/W; bitpos: [31:0]; default: 4095;
         *  need_des
         */
        uint32_t wdt_stg3_hold:32;
    };
    uint32_t val;
} lp_wdt_config4_reg_t;

/** Type of config5 register
 *  need_des
 */
typedef union {
    struct {
        /** chip_reset_target : R/W; bitpos: [7:0]; default: 255;
         *  need_des
         */
        uint32_t chip_reset_target:8;
        /** chip_reset_en : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t chip_reset_en:1;
        /** chip_reset_key : R/W; bitpos: [16:9]; default: 0;
         *  need_des
         */
        uint32_t chip_reset_key:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_wdt_config5_reg_t;

/** Type of feed register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** rtc_wdt_feed : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t rtc_wdt_feed:1;
    };
    uint32_t val;
} lp_wdt_feed_reg_t;

/** Type of wprotect register
 *  need_des
 */
typedef union {
    struct {
        /** wdt_wkey : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t wdt_wkey:32;
    };
    uint32_t val;
} lp_wdt_wprotect_reg_t;

/** Type of swd_config register
 *  need_des
 */
typedef union {
    struct {
        /** swd_reset_flag : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t swd_reset_flag:1;
        uint32_t reserved_1:17;
        /** swd_auto_feed_en : R/W; bitpos: [18]; default: 0;
         *  need_des
         */
        uint32_t swd_auto_feed_en:1;
        /** swd_rst_flag_clr : WT; bitpos: [19]; default: 0;
         *  need_des
         */
        uint32_t swd_rst_flag_clr:1;
        /** swd_signal_width : R/W; bitpos: [29:20]; default: 300;
         *  need_des
         */
        uint32_t swd_signal_width:10;
        /** swd_disable : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t swd_disable:1;
        /** swd_feed : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t swd_feed:1;
    };
    uint32_t val;
} lp_wdt_swd_config_reg_t;

/** Type of swd_wprotect register
 *  need_des
 */
typedef union {
    struct {
        /** swd_wkey : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t swd_wkey:32;
    };
    uint32_t val;
} lp_wdt_swd_wprotect_reg_t;

/** Type of int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** super_wdt_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t super_wdt_int_raw:1;
        /** lp_wdt_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_wdt_int_raw:1;
    };
    uint32_t val;
} lp_wdt_int_raw_reg_t;

/** Type of int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** super_wdt_int_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t super_wdt_int_st:1;
        /** lp_wdt_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_wdt_int_st:1;
    };
    uint32_t val;
} lp_wdt_int_st_reg_t;

/** Type of int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** super_wdt_int_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t super_wdt_int_ena:1;
        /** lp_wdt_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_wdt_int_ena:1;
    };
    uint32_t val;
} lp_wdt_int_ena_reg_t;

/** Type of int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** super_wdt_int_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t super_wdt_int_clr:1;
        /** lp_wdt_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lp_wdt_int_clr:1;
    };
    uint32_t val;
} lp_wdt_int_clr_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** lp_wdt_date : R/W; bitpos: [30:0]; default: 34676864;
         *  need_des
         */
        uint32_t lp_wdt_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_wdt_date_reg_t;


typedef struct {
    volatile lp_wdt_config0_reg_t config0;
    volatile lp_wdt_config1_reg_t config1;
    volatile lp_wdt_config2_reg_t config2;
    volatile lp_wdt_config3_reg_t config3;
    volatile lp_wdt_config4_reg_t config4;
    volatile lp_wdt_config5_reg_t config5;
    volatile lp_wdt_feed_reg_t feed;
    volatile lp_wdt_wprotect_reg_t wprotect;
    volatile lp_wdt_swd_config_reg_t swd_config;
    volatile lp_wdt_swd_wprotect_reg_t swd_wprotect;
    volatile lp_wdt_int_raw_reg_t int_raw;
    volatile lp_wdt_int_st_reg_t int_st;
    volatile lp_wdt_int_ena_reg_t int_ena;
    volatile lp_wdt_int_clr_reg_t int_clr;
    uint32_t reserved_038[241];
    volatile lp_wdt_date_reg_t date;
} lp_wdt_dev_t;

extern lp_wdt_dev_t LP_WDT;

#ifndef __cplusplus
_Static_assert(sizeof(lp_wdt_dev_t) == 0x400, "Invalid size of lp_wdt_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
