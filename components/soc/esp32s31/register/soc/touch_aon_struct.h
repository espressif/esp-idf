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
/** Type of approach_work_meas_num register
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
} touch_aon_approach_work_meas_num_reg_t;

/** Type of scan_ctrl1 register
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
} touch_aon_scan_ctrl1_reg_t;

/** Type of scan_ctrl2 register
 *  need_des
 */
typedef union {
    struct {
        /** freq_scan_cnt_rise : R/W; bitpos: [1:0]; default: 1;
         *  need_des
         */
        uint32_t freq_scan_cnt_rise:2;
        uint32_t reserved_2:4;
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
} touch_aon_scan_ctrl2_reg_t;

/** Type of work register
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
         *  need_des
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
} touch_aon_work_reg_t;

/** Type of work_meas_num register
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
} touch_aon_work_meas_num_reg_t;

/** Type of filter1 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_nn_disupdate_benchmark_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t touch_nn_disupdate_benchmark_en:1;
        /** touch_hysteresis : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t touch_hysteresis:2;
        /** touch_nn_thres : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t touch_nn_thres:2;
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
        /** touch_nn_limit : R/W; bitpos: [20:17]; default: 5;
         *  need_des
         */
        uint32_t touch_nn_limit:4;
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
} touch_aon_filter1_reg_t;

/** Type of filter2 register
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
        /** touch_bypass_nn_thres : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t touch_bypass_nn_thres:1;
    };
    uint32_t val;
} touch_aon_filter2_reg_t;

/** Type of filter3 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_benchmark_sw : R/W; bitpos: [15:0]; default: 0;
         *  need_des
         */
        uint32_t touch_benchmark_sw:16;
        /** touch_update_benchmark_sw : WT; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t touch_update_benchmark_sw:1;
        /** touch_update_benchmark_freq_sel : R/W; bitpos: [18:17]; default: 3;
         *  Reserved
         */
        uint32_t touch_update_benchmark_freq_sel:2;
        /** touch_update_benchmark_pad_sel : R/W; bitpos: [22:19]; default: 15;
         *  Reserved
         */
        uint32_t touch_update_benchmark_pad_sel:4;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} touch_aon_filter3_reg_t;

/** Type of slp0 register
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
} touch_aon_slp0_reg_t;

/** Type of slp1 register
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
} touch_aon_slp1_reg_t;

/** Type of clr register
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
} touch_aon_clr_reg_t;

/** Type of approach register
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
} touch_aon_approach_reg_t;

/** Type of freq0_scan_para register
 *  need_des
 */
typedef union {
    struct {
        /** touch_freq0_dcap_lpf : R/W; bitpos: [6:0]; default: 0;
         *  need_des
         */
        uint32_t touch_freq0_dcap_lpf:7;
        /** touch_freq0_dres_lpf : R/W; bitpos: [8:7]; default: 0;
         *  need_des
         */
        uint32_t touch_freq0_dres_lpf:2;
        /** touch_freq0_drv_ls : R/W; bitpos: [12:9]; default: 0;
         *  need_des
         */
        uint32_t touch_freq0_drv_ls:4;
        /** touch_freq0_drv_hs : R/W; bitpos: [17:13]; default: 0;
         *  need_des
         */
        uint32_t touch_freq0_drv_hs:5;
        /** touch_freq0_dbias : R/W; bitpos: [22:18]; default: 0;
         *  need_des
         */
        uint32_t touch_freq0_dbias:5;
        /** touch_freq0_buf_sel_en : R/W; bitpos: [23]; default: 1;
         *  need_des
         */
        uint32_t touch_freq0_buf_sel_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} touch_aon_freq0_scan_para_reg_t;

/** Type of freq1_scan_para register
 *  need_des
 */
typedef union {
    struct {
        /** touch_freq1_dcap_lpf : R/W; bitpos: [6:0]; default: 0;
         *  need_des
         */
        uint32_t touch_freq1_dcap_lpf:7;
        /** touch_freq1_dres_lpf : R/W; bitpos: [8:7]; default: 0;
         *  need_des
         */
        uint32_t touch_freq1_dres_lpf:2;
        /** touch_freq1_drv_ls : R/W; bitpos: [12:9]; default: 0;
         *  need_des
         */
        uint32_t touch_freq1_drv_ls:4;
        /** touch_freq1_drv_hs : R/W; bitpos: [17:13]; default: 0;
         *  need_des
         */
        uint32_t touch_freq1_drv_hs:5;
        /** touch_freq1_dbias : R/W; bitpos: [22:18]; default: 0;
         *  need_des
         */
        uint32_t touch_freq1_dbias:5;
        /** touch_freq1_buf_sel_en : R/W; bitpos: [23]; default: 1;
         *  need_des
         */
        uint32_t touch_freq1_buf_sel_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} touch_aon_freq1_scan_para_reg_t;

/** Type of freq2_scan_para register
 *  need_des
 */
typedef union {
    struct {
        /** touch_freq2_dcap_lpf : R/W; bitpos: [6:0]; default: 0;
         *  need_des
         */
        uint32_t touch_freq2_dcap_lpf:7;
        /** touch_freq2_dres_lpf : R/W; bitpos: [8:7]; default: 0;
         *  need_des
         */
        uint32_t touch_freq2_dres_lpf:2;
        /** touch_freq2_drv_ls : R/W; bitpos: [12:9]; default: 0;
         *  need_des
         */
        uint32_t touch_freq2_drv_ls:4;
        /** touch_freq2_drv_hs : R/W; bitpos: [17:13]; default: 0;
         *  need_des
         */
        uint32_t touch_freq2_drv_hs:5;
        /** touch_freq2_dbias : R/W; bitpos: [22:18]; default: 0;
         *  need_des
         */
        uint32_t touch_freq2_dbias:5;
        /** touch_freq2_buf_sel_en : R/W; bitpos: [23]; default: 1;
         *  need_des
         */
        uint32_t touch_freq2_buf_sel_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} touch_aon_freq2_scan_para_reg_t;

/** Type of ana_para register
 *  need_des
 */
typedef union {
    struct {
        /** touch_touch_buf_drv : R/W; bitpos: [2:0]; default: 0;
         *  need_des
         */
        uint32_t touch_touch_buf_drv:3;
        /** touch_touch_en_cal : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t touch_touch_en_cal:1;
        /** touch_touch_dcap_cal : R/W; bitpos: [10:4]; default: 0;
         *  need_des
         */
        uint32_t touch_touch_dcap_cal:7;
        /** touch_drefh : R/W; bitpos: [12:11]; default: 3;
         *  need_des
         */
        uint32_t touch_drefh:2;
        /** touch_drefl : R/W; bitpos: [14:13]; default: 0;
         *  need_des
         */
        uint32_t touch_drefl:2;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} touch_aon_ana_para_reg_t;

/** Type of mux0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** touch_data_sel : R/W; bitpos: [9:8]; default: 0;
         *  need_des
         */
        uint32_t touch_data_sel:2;
        /** touch_freq_sel : R/W; bitpos: [11:10]; default: 0;
         *  need_des
         */
        uint32_t touch_freq_sel:2;
        /** touch_bufsel : R/W; bitpos: [26:12]; default: 0;
         *  need_des
         */
        uint32_t touch_bufsel:15;
        /** touch_done_en : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t touch_done_en:1;
        /** touch_done_force : R/W; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t touch_done_force:1;
        /** touch_fsm_en : R/W; bitpos: [29]; default: 1;
         *  need_des
         */
        uint32_t touch_fsm_en:1;
        /** touch_start_en : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t touch_start_en:1;
        /** touch_start_force : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t touch_start_force:1;
    };
    uint32_t val;
} touch_aon_mux0_reg_t;

/** Type of mux1 register
 *  need_des
 */
typedef union {
    struct {
        /** touch_start : R/W; bitpos: [14:0]; default: 0;
         *  need_des
         */
        uint32_t touch_start:15;
        /** touch_xpd : R/W; bitpos: [29:15]; default: 0;
         *  need_des
         */
        uint32_t touch_xpd:15;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} touch_aon_mux1_reg_t;

/** Type of pad0_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad0_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad0_th0:16;
    };
    uint32_t val;
} touch_aon_pad0_th0_reg_t;

/** Type of pad0_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad0_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad0_th1:16;
    };
    uint32_t val;
} touch_aon_pad0_th1_reg_t;

/** Type of pad0_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad0_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad0_th2:16;
    };
    uint32_t val;
} touch_aon_pad0_th2_reg_t;

/** Type of pad1_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad1_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad1_th0:16;
    };
    uint32_t val;
} touch_aon_pad1_th0_reg_t;

/** Type of pad1_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad1_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad1_th1:16;
    };
    uint32_t val;
} touch_aon_pad1_th1_reg_t;

/** Type of pad1_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad1_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad1_th2:16;
    };
    uint32_t val;
} touch_aon_pad1_th2_reg_t;

/** Type of pad2_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad2_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad2_th0:16;
    };
    uint32_t val;
} touch_aon_pad2_th0_reg_t;

/** Type of pad2_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad2_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad2_th1:16;
    };
    uint32_t val;
} touch_aon_pad2_th1_reg_t;

/** Type of pad2_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad2_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad2_th2:16;
    };
    uint32_t val;
} touch_aon_pad2_th2_reg_t;

/** Type of pad3_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad3_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad3_th0:16;
    };
    uint32_t val;
} touch_aon_pad3_th0_reg_t;

/** Type of pad3_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad3_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad3_th1:16;
    };
    uint32_t val;
} touch_aon_pad3_th1_reg_t;

/** Type of pad3_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad3_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad3_th2:16;
    };
    uint32_t val;
} touch_aon_pad3_th2_reg_t;

/** Type of pad4_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad4_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad4_th0:16;
    };
    uint32_t val;
} touch_aon_pad4_th0_reg_t;

/** Type of pad4_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad4_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad4_th1:16;
    };
    uint32_t val;
} touch_aon_pad4_th1_reg_t;

/** Type of pad4_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad4_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad4_th2:16;
    };
    uint32_t val;
} touch_aon_pad4_th2_reg_t;

/** Type of pad5_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad5_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad5_th0:16;
    };
    uint32_t val;
} touch_aon_pad5_th0_reg_t;

/** Type of pad5_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad5_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad5_th1:16;
    };
    uint32_t val;
} touch_aon_pad5_th1_reg_t;

/** Type of pad5_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad5_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad5_th2:16;
    };
    uint32_t val;
} touch_aon_pad5_th2_reg_t;

/** Type of pad6_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad6_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad6_th0:16;
    };
    uint32_t val;
} touch_aon_pad6_th0_reg_t;

/** Type of pad6_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad6_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad6_th1:16;
    };
    uint32_t val;
} touch_aon_pad6_th1_reg_t;

/** Type of pad6_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad6_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad6_th2:16;
    };
    uint32_t val;
} touch_aon_pad6_th2_reg_t;

/** Type of pad7_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad7_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad7_th0:16;
    };
    uint32_t val;
} touch_aon_pad7_th0_reg_t;

/** Type of pad7_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad7_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad7_th1:16;
    };
    uint32_t val;
} touch_aon_pad7_th1_reg_t;

/** Type of pad7_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad7_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad7_th2:16;
    };
    uint32_t val;
} touch_aon_pad7_th2_reg_t;

/** Type of pad8_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad8_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad8_th0:16;
    };
    uint32_t val;
} touch_aon_pad8_th0_reg_t;

/** Type of pad8_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad8_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad8_th1:16;
    };
    uint32_t val;
} touch_aon_pad8_th1_reg_t;

/** Type of pad8_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad8_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad8_th2:16;
    };
    uint32_t val;
} touch_aon_pad8_th2_reg_t;

/** Type of pad9_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad9_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad9_th0:16;
    };
    uint32_t val;
} touch_aon_pad9_th0_reg_t;

/** Type of pad9_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad9_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad9_th1:16;
    };
    uint32_t val;
} touch_aon_pad9_th1_reg_t;

/** Type of pad9_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad9_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad9_th2:16;
    };
    uint32_t val;
} touch_aon_pad9_th2_reg_t;

/** Type of pad10_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad10_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad10_th0:16;
    };
    uint32_t val;
} touch_aon_pad10_th0_reg_t;

/** Type of pad10_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad10_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad10_th1:16;
    };
    uint32_t val;
} touch_aon_pad10_th1_reg_t;

/** Type of pad10_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad10_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad10_th2:16;
    };
    uint32_t val;
} touch_aon_pad10_th2_reg_t;

/** Type of pad11_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad11_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad11_th0:16;
    };
    uint32_t val;
} touch_aon_pad11_th0_reg_t;

/** Type of pad11_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad11_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad11_th1:16;
    };
    uint32_t val;
} touch_aon_pad11_th1_reg_t;

/** Type of pad11_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad11_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad11_th2:16;
    };
    uint32_t val;
} touch_aon_pad11_th2_reg_t;

/** Type of pad12_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad12_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad12_th0:16;
    };
    uint32_t val;
} touch_aon_pad12_th0_reg_t;

/** Type of pad12_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad12_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad12_th1:16;
    };
    uint32_t val;
} touch_aon_pad12_th1_reg_t;

/** Type of pad12_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad12_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad12_th2:16;
    };
    uint32_t val;
} touch_aon_pad12_th2_reg_t;

/** Type of pad13_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad13_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad13_th0:16;
    };
    uint32_t val;
} touch_aon_pad13_th0_reg_t;

/** Type of pad13_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad13_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad13_th1:16;
    };
    uint32_t val;
} touch_aon_pad13_th1_reg_t;

/** Type of pad13_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad13_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad13_th2:16;
    };
    uint32_t val;
} touch_aon_pad13_th2_reg_t;

/** Type of pad14_th0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad14_th0 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad14_th0:16;
    };
    uint32_t val;
} touch_aon_pad14_th0_reg_t;

/** Type of pad14_th1 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad14_th1 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad14_th1:16;
    };
    uint32_t val;
} touch_aon_pad14_th1_reg_t;

/** Type of pad14_th2 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** touch_pad14_th2 : R/W; bitpos: [31:16]; default: 0;
         *  Reserved
         */
        uint32_t touch_pad14_th2:16;
    };
    uint32_t val;
} touch_aon_pad14_th2_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 38830400;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} touch_aon_date_reg_t;


typedef struct {
    volatile touch_aon_approach_work_meas_num_reg_t approach_work_meas_num;
    volatile touch_aon_scan_ctrl1_reg_t scan_ctrl1;
    volatile touch_aon_scan_ctrl2_reg_t scan_ctrl2;
    volatile touch_aon_work_reg_t work;
    volatile touch_aon_work_meas_num_reg_t work_meas_num;
    volatile touch_aon_filter1_reg_t filter1;
    volatile touch_aon_filter2_reg_t filter2;
    volatile touch_aon_filter3_reg_t filter3;
    volatile touch_aon_slp0_reg_t slp0;
    volatile touch_aon_slp1_reg_t slp1;
    volatile touch_aon_clr_reg_t clr;
    volatile touch_aon_approach_reg_t approach;
    volatile touch_aon_freq0_scan_para_reg_t freq0_scan_para;
    volatile touch_aon_freq1_scan_para_reg_t freq1_scan_para;
    volatile touch_aon_freq2_scan_para_reg_t freq2_scan_para;
    volatile touch_aon_ana_para_reg_t ana_para;
    volatile touch_aon_mux0_reg_t mux0;
    volatile touch_aon_mux1_reg_t mux1;
    volatile touch_aon_pad0_th0_reg_t pad0_th0;
    volatile touch_aon_pad0_th1_reg_t pad0_th1;
    volatile touch_aon_pad0_th2_reg_t pad0_th2;
    volatile touch_aon_pad1_th0_reg_t pad1_th0;
    volatile touch_aon_pad1_th1_reg_t pad1_th1;
    volatile touch_aon_pad1_th2_reg_t pad1_th2;
    volatile touch_aon_pad2_th0_reg_t pad2_th0;
    volatile touch_aon_pad2_th1_reg_t pad2_th1;
    volatile touch_aon_pad2_th2_reg_t pad2_th2;
    volatile touch_aon_pad3_th0_reg_t pad3_th0;
    volatile touch_aon_pad3_th1_reg_t pad3_th1;
    volatile touch_aon_pad3_th2_reg_t pad3_th2;
    volatile touch_aon_pad4_th0_reg_t pad4_th0;
    volatile touch_aon_pad4_th1_reg_t pad4_th1;
    volatile touch_aon_pad4_th2_reg_t pad4_th2;
    volatile touch_aon_pad5_th0_reg_t pad5_th0;
    volatile touch_aon_pad5_th1_reg_t pad5_th1;
    volatile touch_aon_pad5_th2_reg_t pad5_th2;
    volatile touch_aon_pad6_th0_reg_t pad6_th0;
    volatile touch_aon_pad6_th1_reg_t pad6_th1;
    volatile touch_aon_pad6_th2_reg_t pad6_th2;
    volatile touch_aon_pad7_th0_reg_t pad7_th0;
    volatile touch_aon_pad7_th1_reg_t pad7_th1;
    volatile touch_aon_pad7_th2_reg_t pad7_th2;
    volatile touch_aon_pad8_th0_reg_t pad8_th0;
    volatile touch_aon_pad8_th1_reg_t pad8_th1;
    volatile touch_aon_pad8_th2_reg_t pad8_th2;
    volatile touch_aon_pad9_th0_reg_t pad9_th0;
    volatile touch_aon_pad9_th1_reg_t pad9_th1;
    volatile touch_aon_pad9_th2_reg_t pad9_th2;
    volatile touch_aon_pad10_th0_reg_t pad10_th0;
    volatile touch_aon_pad10_th1_reg_t pad10_th1;
    volatile touch_aon_pad10_th2_reg_t pad10_th2;
    volatile touch_aon_pad11_th0_reg_t pad11_th0;
    volatile touch_aon_pad11_th1_reg_t pad11_th1;
    volatile touch_aon_pad11_th2_reg_t pad11_th2;
    volatile touch_aon_pad12_th0_reg_t pad12_th0;
    volatile touch_aon_pad12_th1_reg_t pad12_th1;
    volatile touch_aon_pad12_th2_reg_t pad12_th2;
    volatile touch_aon_pad13_th0_reg_t pad13_th0;
    volatile touch_aon_pad13_th1_reg_t pad13_th1;
    volatile touch_aon_pad13_th2_reg_t pad13_th2;
    volatile touch_aon_pad14_th0_reg_t pad14_th0;
    volatile touch_aon_pad14_th1_reg_t pad14_th1;
    volatile touch_aon_pad14_th2_reg_t pad14_th2;
    volatile touch_aon_date_reg_t date;
} touch_aon_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(touch_aon_dev_t) == 0x100, "Invalid size of touch_aon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
