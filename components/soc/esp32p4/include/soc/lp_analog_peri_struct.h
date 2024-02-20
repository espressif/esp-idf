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
/** Type of bod_mode0_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** bod_mode0_close_flash_ena : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_close_flash_ena:1;
        /** bod_mode0_pd_rf_ena : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_pd_rf_ena:1;
        /** bod_mode0_intr_wait : R/W; bitpos: [17:8]; default: 1;
         *  need_des
         */
        uint32_t bod_mode0_intr_wait:10;
        /** bod_mode0_reset_wait : R/W; bitpos: [27:18]; default: 1023;
         *  need_des
         */
        uint32_t bod_mode0_reset_wait:10;
        /** bod_mode0_cnt_clr : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_cnt_clr:1;
        /** bod_mode0_intr_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_intr_ena:1;
        /** bod_mode0_reset_sel : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_reset_sel:1;
        /** bod_mode0_reset_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_reset_ena:1;
    };
    uint32_t val;
} lp_analog_peri_bod_mode0_cntl_reg_t;

/** Type of bod_mode1_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode1_reset_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode1_reset_ena:1;
    };
    uint32_t val;
} lp_analog_peri_bod_mode1_cntl_reg_t;

/** Type of vdd_source_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** detmode_sel : R/W; bitpos: [7:0]; default: 255;
         *  need_des
         */
        uint32_t detmode_sel:8;
        /** vgood_event_record : RO; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t vgood_event_record:8;
        /** vbat_event_record_clr : WT; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t vbat_event_record_clr:8;
        /** bod_source_ena : R/W; bitpos: [31:24]; default: 4;
         *  need_des
         */
        uint32_t bod_source_ena:8;
    };
    uint32_t val;
} lp_analog_peri_vdd_source_cntl_reg_t;

/** Type of vddbat_bod_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** vddbat_undervoltage_flag : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t vddbat_undervoltage_flag:1;
        uint32_t reserved_1:9;
        /** vddbat_charger : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charger:1;
        /** vddbat_cnt_clr : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t vddbat_cnt_clr:1;
        /** vddbat_upvoltage_target : R/W; bitpos: [21:12]; default: 0;
         *  need_des
         */
        uint32_t vddbat_upvoltage_target:10;
        /** vddbat_undervoltage_target : R/W; bitpos: [31:22]; default: 1023;
         *  need_des
         */
        uint32_t vddbat_undervoltage_target:10;
    };
    uint32_t val;
} lp_analog_peri_vddbat_bod_cntl_reg_t;

/** Type of vddbat_charge_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** vddbat_charge_undervoltage_flag : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_undervoltage_flag:1;
        uint32_t reserved_1:9;
        /** vddbat_charge_charger : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_charger:1;
        /** vddbat_charge_cnt_clr : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_cnt_clr:1;
        /** vddbat_charge_upvoltage_target : R/W; bitpos: [21:12]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_upvoltage_target:10;
        /** vddbat_charge_undervoltage_target : R/W; bitpos: [31:22]; default: 1023;
         *  need_des
         */
        uint32_t vddbat_charge_undervoltage_target:10;
    };
    uint32_t val;
} lp_analog_peri_vddbat_charge_cntl_reg_t;

/** Type of ck_glitch_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** ck_glitch_reset_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t ck_glitch_reset_ena:1;
    };
    uint32_t val;
} lp_analog_peri_ck_glitch_cntl_reg_t;

/** Type of pg_glitch_cntl register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** power_glitch_reset_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t power_glitch_reset_ena:1;
    };
    uint32_t val;
} lp_analog_peri_pg_glitch_cntl_reg_t;

/** Type of fib_enable register
 *  need_des
 */
typedef union {
    struct {
        /** ana_fib_ena : R/W; bitpos: [31:0]; default: 4294967295;
         *  need_des
         */
        uint32_t ana_fib_ena:32;
    };
    uint32_t val;
} lp_analog_peri_fib_enable_reg_t;

/** Type of int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** vddbat_charge_upvoltage_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_upvoltage_int_raw:1;
        /** vddbat_charge_undervoltage_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_undervoltage_int_raw:1;
        /** vddbat_upvoltage_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t vddbat_upvoltage_int_raw:1;
        /** vddbat_undervoltage_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t vddbat_undervoltage_int_raw:1;
        /** bod_mode0_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_int_raw:1;
    };
    uint32_t val;
} lp_analog_peri_int_raw_reg_t;

/** Type of int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** vddbat_charge_upvoltage_int_st : RO; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_upvoltage_int_st:1;
        /** vddbat_charge_undervoltage_int_st : RO; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_undervoltage_int_st:1;
        /** vddbat_upvoltage_int_st : RO; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t vddbat_upvoltage_int_st:1;
        /** vddbat_undervoltage_int_st : RO; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t vddbat_undervoltage_int_st:1;
        /** bod_mode0_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_int_st:1;
    };
    uint32_t val;
} lp_analog_peri_int_st_reg_t;

/** Type of int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** vddbat_charge_upvoltage_int_ena : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_upvoltage_int_ena:1;
        /** vddbat_charge_undervoltage_int_ena : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_undervoltage_int_ena:1;
        /** vddbat_upvoltage_int_ena : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t vddbat_upvoltage_int_ena:1;
        /** vddbat_undervoltage_int_ena : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t vddbat_undervoltage_int_ena:1;
        /** bod_mode0_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_int_ena:1;
    };
    uint32_t val;
} lp_analog_peri_int_ena_reg_t;

/** Type of int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:27;
        /** vddbat_charge_upvoltage_int_clr : WT; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_upvoltage_int_clr:1;
        /** vddbat_charge_undervoltage_int_clr : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t vddbat_charge_undervoltage_int_clr:1;
        /** vddbat_upvoltage_int_clr : WT; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t vddbat_upvoltage_int_clr:1;
        /** vddbat_undervoltage_int_clr : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t vddbat_undervoltage_int_clr:1;
        /** bod_mode0_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_int_clr:1;
    };
    uint32_t val;
} lp_analog_peri_int_clr_reg_t;

/** Type of lp_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_lp_int_raw:1;
    };
    uint32_t val;
} lp_analog_peri_lp_int_raw_reg_t;

/** Type of lp_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_lp_int_st:1;
    };
    uint32_t val;
} lp_analog_peri_lp_int_st_reg_t;

/** Type of lp_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_lp_int_ena:1;
    };
    uint32_t val;
} lp_analog_peri_lp_int_ena_reg_t;

/** Type of lp_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** bod_mode0_lp_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t bod_mode0_lp_int_clr:1;
    };
    uint32_t val;
} lp_analog_peri_lp_int_clr_reg_t;

/** Type of touch_approach_work_meas_num register
 *  need_des
 */
typedef union {
    struct {
        /** touch_approach_meas_num2 : R/W; bitpos: [9:0]; default: 100;
         *  need_des
         */
        uint32_t touch_approach_meas_num2:10;
        /** touch_approach_meas_num1 : R/W; bitpos: [19:10]; default: 100;
         *  need_des
         */
        uint32_t touch_approach_meas_num1:10;
        /** touch_approach_meas_num0 : R/W; bitpos: [29:20]; default: 100;
         *  need_des
         */
        uint32_t touch_approach_meas_num0:10;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} lp_analog_peri_touch_approach_work_meas_num_reg_t;

/** Type of touch_scan_ctrl1 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_shield_pad_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t touch_shield_pad_en:1;
        /** touch_inactive_connection : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t touch_inactive_connection:1;
        /** touch_scan_pad_map : R/W; bitpos: [16:2]; default: 0;
         *  need_des
         */
        uint32_t touch_scan_pad_map:15;
        /** touch_xpd_wait : R/W; bitpos: [31:17]; default: 4;
         *  need_des
         */
        uint32_t touch_xpd_wait:15;
    };
    uint32_t val;
} lp_analog_peri_touch_scan_ctrl1_reg_t;

/** Type of touch_scan_ctrl2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** touch_timeout_num : R/W; bitpos: [21:6]; default: 65535;
         *  need_des
         */
        uint32_t touch_timeout_num:16;
        /** touch_timeout_en : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t touch_timeout_en:1;
        /** touch_out_ring : R/W; bitpos: [26:23]; default: 15;
         *  need_des
         */
        uint32_t touch_out_ring:4;
        /** freq_scan_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t freq_scan_en:1;
        /** freq_scan_cnt_limit : R/W; bitpos: [29:28]; default: 3;
         *  need_des
         */
        uint32_t freq_scan_cnt_limit:2;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} lp_analog_peri_touch_scan_ctrl2_reg_t;

/** Type of touch_work register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** div_num2 : R/W; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t div_num2:3;
        /** div_num1 : R/W; bitpos: [21:19]; default: 0;
         *  need_des
         */
        uint32_t div_num1:3;
        /** div_num0 : R/W; bitpos: [24:22]; default: 0;
         *  need_des
         */
        uint32_t div_num0:3;
        /** touch_out_sel : R/W; bitpos: [25]; default: 0;
         *  0: Select the output of the touch as data
         *  1: Select the output of the touch as clock
         */
        uint32_t touch_out_sel:1;
        /** touch_out_reset : WT; bitpos: [26]; default: 0;
         *  need_des
         */
        uint32_t touch_out_reset:1;
        /** touch_out_gate : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t touch_out_gate:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_analog_peri_touch_work_reg_t;

/** Type of touch_work_meas_num register
 *  need_des
 */
typedef union {
    struct {
        /** touch_meas_num2 : R/W; bitpos: [9:0]; default: 100;
         *  need_des
         */
        uint32_t touch_meas_num2:10;
        /** touch_meas_num1 : R/W; bitpos: [19:10]; default: 100;
         *  need_des
         */
        uint32_t touch_meas_num1:10;
        /** touch_meas_num0 : R/W; bitpos: [29:20]; default: 100;
         *  need_des
         */
        uint32_t touch_meas_num0:10;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} lp_analog_peri_touch_work_meas_num_reg_t;

/** Type of touch_filter1 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_neg_noise_disupdate_baseline_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t touch_neg_noise_disupdate_baseline_en:1;
        /** touch_hysteresis : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t touch_hysteresis:2;
        /** touch_neg_noise_thres : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t touch_neg_noise_thres:2;
        /** touch_noise_thres : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t touch_noise_thres:2;
        /** touch_smooth_lvl : R/W; bitpos: [8:7]; default: 0;
         *  need_des
         */
        uint32_t touch_smooth_lvl:2;
        /** touch_jitter_step : R/W; bitpos: [12:9]; default: 1;
         *  need_des
         */
        uint32_t touch_jitter_step:4;
        /** touch_filter_mode : R/W; bitpos: [15:13]; default: 0;
         *  need_des
         */
        uint32_t touch_filter_mode:3;
        /** touch_filter_en : R/W; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t touch_filter_en:1;
        /** touch_neg_noise_limit : R/W; bitpos: [20:17]; default: 5;
         *  need_des
         */
        uint32_t touch_neg_noise_limit:4;
        /** touch_approach_limit : R/W; bitpos: [28:21]; default: 80;
         *  need_des
         */
        uint32_t touch_approach_limit:8;
        /** touch_debounce_limit : R/W; bitpos: [31:29]; default: 3;
         *  need_des
         */
        uint32_t touch_debounce_limit:3;
    };
    uint32_t val;
} lp_analog_peri_touch_filter1_reg_t;

/** Type of touch_filter2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:15;
        /** touch_outen : R/W; bitpos: [29:15]; default: 16383;
         *  need_des
         */
        uint32_t touch_outen:15;
        /** touch_bypass_noise_thres : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t touch_bypass_noise_thres:1;
        /** touch_bypass_neg_noise_thres : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t touch_bypass_neg_noise_thres:1;
    };
    uint32_t val;
} lp_analog_peri_touch_filter2_reg_t;

/** Type of touch_filter3 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_baseline_sw : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t touch_baseline_sw:16;
        /** touch_update_baseline_sw : WT; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t touch_update_baseline_sw:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} lp_analog_peri_touch_filter3_reg_t;

/** Type of touch_slp0 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_slp_th0 : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t touch_slp_th0:16;
        /** touch_slp_channel_clr : WT; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t touch_slp_channel_clr:1;
        /** touch_slp_pad : R/W; bitpos: [20:17]; default: 15;
         *  need_des
         */
        uint32_t touch_slp_pad:4;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} lp_analog_peri_touch_slp0_reg_t;

/** Type of touch_slp1 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_slp_th2 : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t touch_slp_th2:16;
        /** touch_slp_th1 : R/W; bitpos: [31:16]; default: 0;
         *  need_des
         */
        uint32_t touch_slp_th1:16;
    };
    uint32_t val;
} lp_analog_peri_touch_slp1_reg_t;

/** Type of touch_clr register
 *  need_des
 */
typedef union {
    struct {
        /** touch_channel_clr : WT; bitpos: [14:0]; default: 0;
         *  need_des
         */
        uint32_t touch_channel_clr:15;
        /** touch_status_clr : WT; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t touch_status_clr:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_analog_peri_touch_clr_reg_t;

/** Type of touch_approach register
 *  need_des
 */
typedef union {
    struct {
        /** touch_approach_pad0 : R/W; bitpos: [3:0]; default: 15;
         *  need_des
         */
        uint32_t touch_approach_pad0:4;
        /** touch_approach_pad1 : R/W; bitpos: [7:4]; default: 15;
         *  need_des
         */
        uint32_t touch_approach_pad1:4;
        /** touch_approach_pad2 : R/W; bitpos: [11:8]; default: 15;
         *  need_des
         */
        uint32_t touch_approach_pad2:4;
        /** touch_slp_approach_en : R/W; bitpos: [12]; default: 0;
         *  need_des
         */
        uint32_t touch_slp_approach_en:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} lp_analog_peri_touch_approach_reg_t;

/** Type of touch_freq_scan_para register
 *  need_des
 */
typedef union {
    struct {
        /** touch_freq_dcap_lpf : R/W; bitpos: [6:0]; default: 0;
         *  Capacity of the RC low pass filter
         *  0 ~ 2.54 pF, step 20fF
         */
        uint32_t touch_freq_dcap_lpf:7;
        /** touch_freq_dres_lpf : R/W; bitpos: [8:7]; default: 0;
         *  Resistance of the RC low pass filter
         *  0 ~ 4.5 K, step 1.5 K
         */
        uint32_t touch_freq_dres_lpf:2;
        /** touch_freq_drv_ls : R/W; bitpos: [12:9]; default: 0;
         *  Low speed touch driver, effective when high speed driver is disabled
         */
        uint32_t touch_freq_drv_ls:4;
        /** touch_freq_drv_hs : R/W; bitpos: [17:13]; default: 0;
         *  High speed touch driver
         */
        uint32_t touch_freq_drv_hs:5;
        /** touch_freq_dbias : R/W; bitpos: [22:18]; default: 0;
         *  Internal LDO voltage
         */
        uint32_t touch_freq_dbias:5;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} lp_analog_peri_touch_freq_scan_para_reg_t;

/** Type of touch_ana_para register
 *  need_des
 */
typedef union {
    struct {
        /** touch_touch_buf_drv : R/W; bitpos: [2:0]; default: 0;
         *  The driver of water proof touch buff
         */
        uint32_t touch_touch_buf_drv:3;
        /** touch_touch_en_cal : R/W; bitpos: [3]; default: 0;
         *  Enable internal loop. Need to turn off touch pad.
         *  Tuning 'dcap_cal' to change the frequency
         */
        uint32_t touch_touch_en_cal:1;
        /** touch_touch_dcap_cal : R/W; bitpos: [10:4]; default: 0;
         *  The internal capacitor connected to the touch pad. Effective when 'en_cal' enabled
         *  Normally set to 0
         */
        uint32_t touch_touch_dcap_cal:7;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_analog_peri_touch_ana_para_reg_t;

/** Type of touch_mux0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** touch_data_sel : R/W; bitpos: [9:8]; default: 0;
         *  The type of the output data for debugging
         *  0/1: raw data
         *  2: baseline
         *  3: smooth data
         */
        uint32_t touch_data_sel:2;
        /** touch_freq_sel : R/W; bitpos: [11:10]; default: 0;
         *  The frequency id of the output data for debugging
         *  0: invalid
         *  1: Frequency 1
         *  2: Frequency 2
         *  3: Frequency 3
         */
        uint32_t touch_freq_sel:2;
        /** touch_bufsel : R/W; bitpos: [26:12]; default: 0;
         *  The bitmap of the shield pad
         */
        uint32_t touch_bufsel:15;
        /** touch_done_en : R/W; bitpos: [27]; default: 0;
         *  Force to terminate the touch by software
         */
        uint32_t touch_done_en:1;
        /** touch_done_force : R/W; bitpos: [28]; default: 0;
         *  0: Select touch_meas_done as the touch timer input
         *  1: Select software termination as the touch timer input
         */
        uint32_t touch_done_force:1;
        /** touch_fsm_en : R/W; bitpos: [29]; default: 1;
         *  0: Select software configured parameters for ana
         *  1: Select hardware calculated parameters for ana
         */
        uint32_t touch_fsm_en:1;
        /** touch_start_en : R/W; bitpos: [30]; default: 0;
         *  Force to start the touch by software
         */
        uint32_t touch_start_en:1;
        /** touch_start_force : R/W; bitpos: [31]; default: 0;
         *  0: Select the touch timer to start the touch scanning
         *  1: Select the software to start the touch scanning
         */
        uint32_t touch_start_force:1;
    };
    uint32_t val;
} lp_analog_peri_touch_mux0_reg_t;

/** Type of touch_mux1 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_start : R/W; bitpos: [14:0]; default: 0;
         *  The bitmap of the start touch channels
         */
        uint32_t touch_start:15;
        /** touch_xpd : R/W; bitpos: [29:15]; default: 0;
         *  The bitmap of the power on touch channels
         */
        uint32_t touch_xpd:15;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} lp_analog_peri_touch_mux1_reg_t;

/** Type of touch_pad0_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad_th : R/W; bitpos: [31:16]; default: 0;
         *  The threshold to activate a touch channel
         */
        uint32_t threshold:16;
    };
    uint32_t val;
} lp_analog_peri_touch_pad_thn_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** lp_analog_peri_date : R/W; bitpos: [30:0]; default: 2294816;
         *  need_des
         */
        uint32_t lp_analog_peri_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_analog_peri_date_reg_t;


typedef struct {
    volatile lp_analog_peri_touch_pad_thn_reg_t thn[3];
} lp_analog_peri_touch_padx_thn_reg_t;

typedef struct {
    volatile lp_analog_peri_bod_mode0_cntl_reg_t bod_mode0_cntl;
    volatile lp_analog_peri_bod_mode1_cntl_reg_t bod_mode1_cntl;
    volatile lp_analog_peri_vdd_source_cntl_reg_t vdd_source_cntl;
    volatile lp_analog_peri_vddbat_bod_cntl_reg_t vddbat_bod_cntl;
    volatile lp_analog_peri_vddbat_charge_cntl_reg_t vddbat_charge_cntl;
    volatile lp_analog_peri_ck_glitch_cntl_reg_t ck_glitch_cntl;
    volatile lp_analog_peri_pg_glitch_cntl_reg_t pg_glitch_cntl;
    volatile lp_analog_peri_fib_enable_reg_t fib_enable;
    volatile lp_analog_peri_int_raw_reg_t int_raw;
    volatile lp_analog_peri_int_st_reg_t int_st;
    volatile lp_analog_peri_int_ena_reg_t int_ena;
    volatile lp_analog_peri_int_clr_reg_t int_clr;
    volatile lp_analog_peri_lp_int_raw_reg_t lp_int_raw;
    volatile lp_analog_peri_lp_int_st_reg_t lp_int_st;
    volatile lp_analog_peri_lp_int_ena_reg_t lp_int_ena;
    volatile lp_analog_peri_lp_int_clr_reg_t lp_int_clr;
    uint32_t reserved_040[47];
    volatile lp_analog_peri_touch_approach_work_meas_num_reg_t touch_approach_work_meas_num;
    volatile lp_analog_peri_touch_scan_ctrl1_reg_t touch_scan_ctrl1;
    volatile lp_analog_peri_touch_scan_ctrl2_reg_t touch_scan_ctrl2;
    volatile lp_analog_peri_touch_work_reg_t touch_work;
    volatile lp_analog_peri_touch_work_meas_num_reg_t touch_work_meas_num;
    volatile lp_analog_peri_touch_filter1_reg_t touch_filter1;
    volatile lp_analog_peri_touch_filter2_reg_t touch_filter2;
    volatile lp_analog_peri_touch_filter3_reg_t touch_filter3;
    volatile lp_analog_peri_touch_slp0_reg_t touch_slp0;
    volatile lp_analog_peri_touch_slp1_reg_t touch_slp1;
    volatile lp_analog_peri_touch_clr_reg_t touch_clr;
    volatile lp_analog_peri_touch_approach_reg_t touch_approach;
    volatile lp_analog_peri_touch_freq_scan_para_reg_t touch_freq_scan_para[3];
    volatile lp_analog_peri_touch_ana_para_reg_t touch_ana_para;
    volatile lp_analog_peri_touch_mux0_reg_t touch_mux0;
    volatile lp_analog_peri_touch_mux1_reg_t touch_mux1;
    volatile lp_analog_peri_touch_padx_thn_reg_t touch_padx_thn[15];
    uint32_t reserved_1f8[129];
    volatile lp_analog_peri_date_reg_t date;
} lp_analog_peri_dev_t;

extern lp_analog_peri_dev_t LP_ANA_PERI;

#ifndef __cplusplus
_Static_assert(sizeof(lp_analog_peri_dev_t) == 0x400, "Invalid size of lp_analog_peri_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
