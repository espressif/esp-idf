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
 *  need_des
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

/** Type of status_0 register
 *  need_des
 */
typedef union {
    struct {
        /** pad0_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad0_data:16;
        /** pad0_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad0_debounce_cnt:3;
        /** pad0_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad0_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_0_reg_t;

/** Type of status_1 register
 *  need_des
 */
typedef union {
    struct {
        /** pad1_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad1_data:16;
        /** pad1_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad1_debounce_cnt:3;
        /** pad1_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad1_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_1_reg_t;

/** Type of status_2 register
 *  need_des
 */
typedef union {
    struct {
        /** pad2_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad2_data:16;
        /** pad2_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad2_debounce_cnt:3;
        /** pad2_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad2_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_2_reg_t;

/** Type of status_3 register
 *  need_des
 */
typedef union {
    struct {
        /** pad3_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad3_data:16;
        /** pad3_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad3_debounce_cnt:3;
        /** pad3_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad3_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_3_reg_t;

/** Type of status_4 register
 *  need_des
 */
typedef union {
    struct {
        /** pad4_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad4_data:16;
        /** pad4_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad4_debounce_cnt:3;
        /** pad4_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad4_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_4_reg_t;

/** Type of status_5 register
 *  need_des
 */
typedef union {
    struct {
        /** pad5_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad5_data:16;
        /** pad5_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad5_debounce_cnt:3;
        /** pad5_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad5_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_5_reg_t;

/** Type of status_6 register
 *  need_des
 */
typedef union {
    struct {
        /** pad6_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad6_data:16;
        /** pad6_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad6_debounce_cnt:3;
        /** pad6_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad6_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_6_reg_t;

/** Type of status_7 register
 *  need_des
 */
typedef union {
    struct {
        /** pad7_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad7_data:16;
        /** pad7_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad7_debounce_cnt:3;
        /** pad7_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad7_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_7_reg_t;

/** Type of status_8 register
 *  need_des
 */
typedef union {
    struct {
        /** pad8_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad8_data:16;
        /** pad8_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad8_debounce_cnt:3;
        /** pad8_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad8_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_8_reg_t;

/** Type of status_9 register
 *  need_des
 */
typedef union {
    struct {
        /** pad9_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad9_data:16;
        /** pad9_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad9_debounce_cnt:3;
        /** pad9_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad9_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_9_reg_t;

/** Type of status_10 register
 *  need_des
 */
typedef union {
    struct {
        /** pad10_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad10_data:16;
        /** pad10_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad10_debounce_cnt:3;
        /** pad10_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad10_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_10_reg_t;

/** Type of status_11 register
 *  need_des
 */
typedef union {
    struct {
        /** pad11_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad11_data:16;
        /** pad11_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad11_debounce_cnt:3;
        /** pad11_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad11_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_11_reg_t;

/** Type of status_12 register
 *  need_des
 */
typedef union {
    struct {
        /** pad12_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad12_data:16;
        /** pad12_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad12_debounce_cnt:3;
        /** pad12_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad12_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_12_reg_t;

/** Type of status_13 register
 *  need_des
 */
typedef union {
    struct {
        /** pad13_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad13_data:16;
        /** pad13_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad13_debounce_cnt:3;
        /** pad13_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad13_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_13_reg_t;

/** Type of status_14 register
 *  need_des
 */
typedef union {
    struct {
        /** pad14_data : RO; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t pad14_data:16;
        /** pad14_debounce_cnt : RO; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t pad14_debounce_cnt:3;
        /** pad14_neg_noise_cnt : RO; bitpos: [22:19]; default: 0;
         *  need_des
         */
        uint32_t pad14_neg_noise_cnt:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} rtc_touch_status_14_reg_t;

/** Type of status_15 register
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
} rtc_touch_status_15_reg_t;

/** Type of status_16 register
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
} rtc_touch_status_16_reg_t;

/** Type of status_17 register
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
} rtc_touch_status_17_reg_t;

/** Type of chn_tmp_status register
 *  need_des
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
    volatile rtc_touch_status_0_reg_t status_0;
    volatile rtc_touch_status_1_reg_t status_1;
    volatile rtc_touch_status_2_reg_t status_2;
    volatile rtc_touch_status_3_reg_t status_3;
    volatile rtc_touch_status_4_reg_t status_4;
    volatile rtc_touch_status_5_reg_t status_5;
    volatile rtc_touch_status_6_reg_t status_6;
    volatile rtc_touch_status_7_reg_t status_7;
    volatile rtc_touch_status_8_reg_t status_8;
    volatile rtc_touch_status_9_reg_t status_9;
    volatile rtc_touch_status_10_reg_t status_10;
    volatile rtc_touch_status_11_reg_t status_11;
    volatile rtc_touch_status_12_reg_t status_12;
    volatile rtc_touch_status_13_reg_t status_13;
    volatile rtc_touch_status_14_reg_t status_14;
    volatile rtc_touch_status_15_reg_t status_15;
    volatile rtc_touch_status_16_reg_t status_16;
    volatile rtc_touch_status_17_reg_t status_17;
    volatile rtc_touch_chn_tmp_status_reg_t chn_tmp_status;
    uint32_t reserved_060[40];
    volatile rtc_touch_date_reg_t date;
} rtc_touch_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(rtc_touch_dev_t) == 0x104, "Invalid size of rtc_touch_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
