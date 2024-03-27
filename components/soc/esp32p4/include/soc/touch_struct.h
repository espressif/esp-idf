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
/** Type of int_raw register
 *  need_des
 */
typedef union {
    struct {
        /** scan_done_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t scan_done_int_raw:1;
        /** done_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t done_int_raw:1;
        /** active_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t active_int_raw:1;
        /** inactive_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t inactive_int_raw:1;
        /** timeout_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t timeout_int_raw:1;
        /** approach_loop_done_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t approach_loop_done_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtc_touch_int_raw_reg_t;

/** Type of int_st register
 *  need_des
 */
typedef union {
    struct {
        /** scan_done_int_st : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t scan_done_int_st:1;
        /** done_int_st : RO; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t done_int_st:1;
        /** active_int_st : RO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t active_int_st:1;
        /** inactive_int_st : RO; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t inactive_int_st:1;
        /** timeout_int_st : RO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t timeout_int_st:1;
        /** approach_loop_done_int_st : RO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t approach_loop_done_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtc_touch_int_st_reg_t;

/** Type of int_ena register
 *  need_des
 */
typedef union {
    struct {
        /** scan_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t scan_done_int_ena:1;
        /** done_int_ena : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t done_int_ena:1;
        /** active_int_ena : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t active_int_ena:1;
        /** inactive_int_ena : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t inactive_int_ena:1;
        /** timeout_int_ena : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t timeout_int_ena:1;
        /** approach_loop_done_int_ena : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t approach_loop_done_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtc_touch_int_ena_reg_t;

/** Type of int_clr register
 *  need_des
 */
typedef union {
    struct {
        /** scan_done_int_clr : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t scan_done_int_clr:1;
        /** done_int_clr : WT; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t done_int_clr:1;
        /** active_int_clr : WT; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t active_int_clr:1;
        /** inactive_int_clr : WT; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t inactive_int_clr:1;
        /** timeout_int_clr : WT; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t timeout_int_clr:1;
        /** approach_loop_done_int_clr : WT; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t approach_loop_done_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} rtc_touch_int_clr_reg_t;

/** Type of chn_status register
 *  Latched channel status
 */
typedef union {
    struct {
        /** pad_active : RO; bitpos: [14:0]; default: 0;
         *  need_des
         */
        uint32_t pad_active:15;
        /** meas_done : RO; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t meas_done:1;
        /** scan_curr : RO; bitpos: [19:16]; default: 0;
         *  need_des
         */
        uint32_t scan_curr:4;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} rtc_touch_chn_status_reg_t;

/** Type of chn_data register
 *  need_des
 */
typedef union {
    struct {
        /** pad_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad_data:16;
        /** pad_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad_debounce_cnt:3;
        /** pad_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_chn_data_reg_t;

/** Type of slp_ch_data register
 *  need_des
 */
typedef union {
    struct {
        /** slp_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t slp_data:16;
        /** slp_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t slp_debounce_cnt:3;
        /** slp_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t slp_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_slp_ch_data_reg_t;

/** Type of aprch_ch_data register
 *  need_des
 */
typedef union {
    struct {
        /** approach_pad2_cnt : RO; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t approach_pad2_cnt:8;
        /** approach_pad1_cnt : RO; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t approach_pad1_cnt:8;
        /** approach_pad0_cnt : RO; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t approach_pad0_cnt:8;
        /** slp_approach_cnt : RO; bitpos: [31:24]; default: 0;
         *  need_des
         */
        uint32_t slp_approach_cnt:8;
    };
    uint32_t val;
} rtc_touch_aprch_ch_data_reg_t;

/** Type of config register
 *  need_des
 */
typedef union {
    struct {
        /** dcap_lpf : RO; bitpos: [6:0]; default: 0;
         *  Reserved
         */
        uint32_t dcap_lpf:7;
        /** dres_lpf : RO; bitpos: [8:7]; default: 0;
         *  need_des
         */
        uint32_t dres_lpf:2;
        /** drv_ls : RO; bitpos: [12:9]; default: 0;
         *  need_des
         */
        uint32_t drv_ls:4;
        /** drv_hs : RO; bitpos: [17:13]; default: 0;
         *  need_des
         */
        uint32_t drv_hs:5;
        /** dbias : RO; bitpos: [22:18]; default: 0;
         *  need_des
         */
        uint32_t dbias:5;
        /** freq_scan_cnt : RO; bitpos: [24:23]; default: 0;
         *  need_des
         */
        uint32_t freq_scan_cnt:2;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} rtc_touch_sampler_status_reg_t;

/** Type of chn_tmp_status register
 *  Realtime channel status
 */
typedef union {
    struct {
        /** pad_inactive_status : RO; bitpos: [14:0]; default: 0;
         *  need_des
         */
        uint32_t pad_inactive_status:15;
        /** pad_active_status : RO; bitpos: [29:15]; default: 0;
         *  need_des
         */
        uint32_t pad_active_status:15;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} rtc_touch_chn_tmp_status_reg_t;


/** Group: Version */
/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 2294548;
         *  need_des
         */
        uint32_t date:28;
        uint32_t reserved_28:3;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} rtc_touch_date_reg_t;


typedef struct {
    volatile rtc_touch_int_raw_reg_t int_raw;
    volatile rtc_touch_int_st_reg_t int_st;
    volatile rtc_touch_int_ena_reg_t int_ena;
    volatile rtc_touch_int_clr_reg_t int_clr;
    volatile rtc_touch_chn_status_reg_t chn_status;
    volatile rtc_touch_chn_data_reg_t chn_data[15];
    volatile rtc_touch_slp_ch_data_reg_t slp_ch_data;
    volatile rtc_touch_aprch_ch_data_reg_t aprch_ch_data;
    volatile rtc_touch_sampler_status_reg_t sampler_status;
    volatile rtc_touch_chn_tmp_status_reg_t chn_tmp_status;
    uint32_t reserved_060[40];
    volatile rtc_touch_date_reg_t date;
} rtc_touch_dev_t;

extern rtc_touch_dev_t LP_TOUCH;

#ifndef __cplusplus
_Static_assert(sizeof(rtc_touch_dev_t) == 0x104, "Invalid size of rtc_touch_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
