/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of cfg0 register
 *  need_des
 */
typedef union {
    struct {
        /** div_cycle : R/W; bitpos: [7:0]; default: 1;
         *  need_des
         */
        uint32_t div_cycle:8;
        /** full_cnt_done : RO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t full_cnt_done:1;
        /** div_cali_cnt : RO; bitpos: [24:9]; default: 0;
         *  need_des
         */
        uint32_t div_cali_cnt:16;
        /** div_numerator_type : RO; bitpos: [25]; default: 0;
         *  need_des
         */
        uint32_t div_numerator_type:1;
        /** div_num : RO; bitpos: [31:26]; default: 0;
         *  need_des
         */
        uint32_t div_num:6;
    };
    uint32_t val;
} rtclockcali_cfg0_reg_t;

/** Type of cfg1 register
 *  need_des
 */
typedef union {
    struct {
        /** div_numerator : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t div_numerator:16;
        /** div_denominator : RO; bitpos: [31:16]; default: 0;
         *  need_des
         */
        uint32_t div_denominator:16;
    };
    uint32_t val;
} rtclockcali_cfg1_reg_t;

/** Type of cfg2 register
 *  need_des
 */
typedef union {
    struct {
        /** div_wait_pwr_good : R/W; bitpos: [8:0]; default: 255;
         *  need_des
         */
        uint32_t div_wait_pwr_good:9;
        uint32_t reserved_9:6;
        /** div_slp_val : R/W; bitpos: [30:15]; default: 1;
         *  need_des
         */
        uint32_t div_slp_val:16;
        /** div_timer_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t div_timer_en:1;
    };
    uint32_t val;
} rtclockcali_cfg2_reg_t;

/** Type of date register
 *  Configure register.
 */
typedef union {
    struct {
        /** rtclockcali_date : R/W; bitpos: [30:0]; default: 38805584;
         *  need_des
         */
        uint32_t rtclockcali_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} rtclockcali_date_reg_t;


typedef struct {
    uint32_t reserved_000[14];
    volatile rtclockcali_cfg0_reg_t cfg0;
    volatile rtclockcali_cfg1_reg_t cfg1;
    volatile rtclockcali_cfg2_reg_t cfg2;
    uint32_t reserved_044[238];
    volatile rtclockcali_date_reg_t date;
} rtclockcali_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(rtclockcali_dev_t) == 0x400, "Invalid size of rtclockcali_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
