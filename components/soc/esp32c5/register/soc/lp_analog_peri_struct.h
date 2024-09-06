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
/** Type of bod_mode0_cntl register
 *  Configure brownout mode0
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** bod_mode0_close_flash_ena : R/W; bitpos: [6]; default: 0;
         *  enable suspend spi when brownout interrupt or not
         *  1:enable
         *  0:disable
         */
        uint32_t bod_mode0_close_flash_ena:1;
        /** bod_mode0_pd_rf_ena : R/W; bitpos: [7]; default: 0;
         *  enable power down RF when brownout interrupt or not
         *  1:enable
         *  0:disable
         */
        uint32_t bod_mode0_pd_rf_ena:1;
        /** bod_mode0_intr_wait : R/W; bitpos: [17:8]; default: 1;
         *  set the undervoltage hold time for triggering brownout interrupt
         */
        uint32_t bod_mode0_intr_wait:10;
        /** bod_mode0_reset_wait : R/W; bitpos: [27:18]; default: 1023;
         *  set the undervoltage hold time for triggering brownout reset
         */
        uint32_t bod_mode0_reset_wait:10;
        /** bod_mode0_cnt_clr : R/W; bitpos: [28]; default: 0;
         *  clear brownout count or not
         *  1: clear
         *  0: no operation
         */
        uint32_t bod_mode0_cnt_clr:1;
        /** bod_mode0_intr_ena : R/W; bitpos: [29]; default: 0;
         *  enable brownout interrupt or not
         *  1: enable
         *  0: disable
         */
        uint32_t bod_mode0_intr_ena:1;
        /** bod_mode0_reset_sel : R/W; bitpos: [30]; default: 0;
         *  select brownout reset level
         *  1: system reset
         *  0: chip reset
         */
        uint32_t bod_mode0_reset_sel:1;
        /** bod_mode0_reset_ena : R/W; bitpos: [31]; default: 0;
         *  enable brownout reset or not
         *  1: enable
         *  0: disable
         */
        uint32_t bod_mode0_reset_ena:1;
    };
    uint32_t val;
} lp_ana_bod_mode0_cntl_reg_t;

/** Type of bod_mode1_cntl register
 *  Configure brownout mode1
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode1_reset_ena : R/W; bitpos: [31]; default: 0;
         *  enable brownout mode1 reset or not
         *  1: enable
         *  0: disable
         */
        uint32_t bod_mode1_reset_ena:1;
    };
    uint32_t val;
} lp_ana_bod_mode1_cntl_reg_t;

/** Type of ck_glitch_cntl register
 *  Configure power glitch
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** pwr_glitch_reset_ena : R/W; bitpos: [30:27]; default: 0;
         *  enable powerglitch or not
         */
        uint32_t pwr_glitch_reset_ena:4;
        /** ck_glitch_reset_ena : R/W; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t ck_glitch_reset_ena:1;
    };
    uint32_t val;
} lp_ana_ck_glitch_cntl_reg_t;

/** Type of fib_enable register
 *  configure FIB REG
 */
typedef union {
    struct {
        /** ana_fib_ena : R/W; bitpos: [31:0]; default: 4294967295;
         *  configure analog fib by software
         */
        uint32_t ana_fib_ena:32;
    };
    uint32_t val;
} lp_ana_fib_enable_reg_t;

/** Type of int_raw register
 *  interrpt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  brownout mode0 interrupt raw register
         */
        uint32_t bod_mode0_int_raw:1;
    };
    uint32_t val;
} lp_ana_int_raw_reg_t;

/** Type of int_st register
 *  interrpt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_st : RO; bitpos: [31]; default: 0;
         *  brownout mode0 interrupt status register
         */
        uint32_t bod_mode0_int_st:1;
    };
    uint32_t val;
} lp_ana_int_st_reg_t;

/** Type of int_ena register
 *  interrpt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_ena : R/W; bitpos: [31]; default: 0;
         *  brownout mode0 interrupt enable register
         */
        uint32_t bod_mode0_int_ena:1;
    };
    uint32_t val;
} lp_ana_int_ena_reg_t;

/** Type of int_clr register
 *  interrpt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_int_clr : WT; bitpos: [31]; default: 0;
         *  brownout mode0 interrupt clear register
         */
        uint32_t bod_mode0_int_clr:1;
    };
    uint32_t val;
} lp_ana_int_clr_reg_t;

/** Type of lp_int_raw register
 *  lp interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  brownout mode0 lp interrupt raw register
         */
        uint32_t bod_mode0_lp_int_raw:1;
    };
    uint32_t val;
} lp_ana_lp_int_raw_reg_t;

/** Type of lp_int_st register
 *  lp interrupt status register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_st : RO; bitpos: [31]; default: 0;
         *  brownout mode0 lp interrupt status register
         */
        uint32_t bod_mode0_lp_int_st:1;
    };
    uint32_t val;
} lp_ana_lp_int_st_reg_t;

/** Type of lp_int_ena register
 *  lp interrupt enable register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_ena : R/W; bitpos: [31]; default: 0;
         *  brownout mode0 lp interrupt enable register
         */
        uint32_t bod_mode0_lp_int_ena:1;
    };
    uint32_t val;
} lp_ana_lp_int_ena_reg_t;

/** Type of lp_int_clr register
 *  lp interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_clr : WT; bitpos: [31]; default: 0;
         *  brownout mode0 lp interrupt clear register
         */
        uint32_t bod_mode0_lp_int_clr:1;
    };
    uint32_t val;
} lp_ana_lp_int_clr_reg_t;

/** Type of date register
 *  version register
 */
typedef union {
    struct {
        /** lp_ana_date : R/W; bitpos: [30:0]; default: 36774528;
         *  version register
         */
        uint32_t lp_ana_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_ana_date_reg_t;


typedef struct {
    volatile lp_ana_bod_mode0_cntl_reg_t bod_mode0_cntl;
    volatile lp_ana_bod_mode1_cntl_reg_t bod_mode1_cntl;
    volatile lp_ana_ck_glitch_cntl_reg_t ck_glitch_cntl;
    volatile lp_ana_fib_enable_reg_t fib_enable;
    volatile lp_ana_int_raw_reg_t int_raw;
    volatile lp_ana_int_st_reg_t int_st;
    volatile lp_ana_int_ena_reg_t int_ena;
    volatile lp_ana_int_clr_reg_t int_clr;
    volatile lp_ana_lp_int_raw_reg_t lp_int_raw;
    volatile lp_ana_lp_int_st_reg_t lp_int_st;
    volatile lp_ana_lp_int_ena_reg_t lp_int_ena;
    volatile lp_ana_lp_int_clr_reg_t lp_int_clr;
    uint32_t reserved_030[243];
    volatile lp_ana_date_reg_t date;
} lp_ana_dev_t;

extern lp_ana_dev_t LP_ANA_PERI;

#ifndef __cplusplus
_Static_assert(sizeof(lp_ana_dev_t) == 0x400, "Invalid size of lp_ana_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
