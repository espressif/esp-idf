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

/** Group: configure register */
/** Type of pmup_bitmap_high0 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_high0 : R/W; bitpos: [31:0]; default: 0;
         *  select valid high channel0
         */
        uint32_t pump_bitmap_high0:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_high0_reg_t;

/** Type of pmup_bitmap_high1 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_high1 : R/W; bitpos: [31:0]; default: 0;
         *  select valid high channel1
         */
        uint32_t pump_bitmap_high1:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_high1_reg_t;

/** Type of pmup_bitmap_high2 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_high2 : R/W; bitpos: [31:0]; default: 0;
         *  select valid high channel2
         */
        uint32_t pump_bitmap_high2:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_high2_reg_t;

/** Type of pmup_bitmap_high3 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_high3 : R/W; bitpos: [31:0]; default: 0;
         *  select valid high channel3
         */
        uint32_t pump_bitmap_high3:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_high3_reg_t;

/** Type of pmup_bitmap_high4 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_high4 : R/W; bitpos: [31:0]; default: 0;
         *  select valid high channel4
         */
        uint32_t pump_bitmap_high4:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_high4_reg_t;

/** Type of pmup_bitmap_low0 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_low0 : R/W; bitpos: [31:0]; default: 0;
         *  select valid low channel0
         */
        uint32_t pump_bitmap_low0:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_low0_reg_t;

/** Type of pmup_bitmap_low1 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_low1 : R/W; bitpos: [31:0]; default: 0;
         *  select valid low channel1
         */
        uint32_t pump_bitmap_low1:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_low1_reg_t;

/** Type of pmup_bitmap_low2 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_low2 : R/W; bitpos: [31:0]; default: 0;
         *  select valid low channel2
         */
        uint32_t pump_bitmap_low2:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_low2_reg_t;

/** Type of pmup_bitmap_low3 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_low3 : R/W; bitpos: [31:0]; default: 0;
         *  select valid low channel3
         */
        uint32_t pump_bitmap_low3:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_low3_reg_t;

/** Type of pmup_bitmap_low4 register
 *  select valid pvt channel
 */
typedef union {
    struct {
        /** pump_bitmap_low4 : R/W; bitpos: [31:0]; default: 0;
         *  select valid low channel4
         */
        uint32_t pump_bitmap_low4:32;
    };
    uint32_t val;
} pvt_pmup_bitmap_low4_reg_t;

/** Type of pmup_drv_cfg register
 *  configure pump drv
 */
typedef union {
    struct {
        uint32_t reserved_0:9;
        /** pump_en : R/W; bitpos: [9]; default: 0;
         *  configure pvt charge xpd
         */
        uint32_t pump_en:1;
        /** clk_en : R/W; bitpos: [10]; default: 0;
         *  force register clken
         */
        uint32_t clk_en:1;
        /** pump_drv4 : R/W; bitpos: [14:11]; default: 0;
         *  configure cmd4 drv
         */
        uint32_t pump_drv4:4;
        /** pump_drv3 : R/W; bitpos: [18:15]; default: 0;
         *  configure cmd3 drv
         */
        uint32_t pump_drv3:4;
        /** pump_drv2 : R/W; bitpos: [22:19]; default: 0;
         *  configure cmd2 drv
         */
        uint32_t pump_drv2:4;
        /** pump_drv1 : R/W; bitpos: [26:23]; default: 0;
         *  configure cmd1 drv
         */
        uint32_t pump_drv1:4;
        /** pump_drv0 : R/W; bitpos: [30:27]; default: 0;
         *  configure cmd0 drv
         */
        uint32_t pump_drv0:4;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} pvt_pmup_drv_cfg_reg_t;

/** Type of pmup_channel_cfg register
 *  configure the code of valid pump channel code
 */
typedef union {
    struct {
        uint32_t reserved_0:7;
        /** pump_channel_code4 : R/W; bitpos: [11:7]; default: 0;
         *  configure cmd4 code
         */
        uint32_t pump_channel_code4:5;
        /** pump_channel_code3 : R/W; bitpos: [16:12]; default: 0;
         *  configure cmd3 code
         */
        uint32_t pump_channel_code3:5;
        /** pump_channel_code2 : R/W; bitpos: [21:17]; default: 0;
         *  configure cmd2 code
         */
        uint32_t pump_channel_code2:5;
        /** pump_channel_code1 : R/W; bitpos: [26:22]; default: 0;
         *  configure cmd1 code
         */
        uint32_t pump_channel_code1:5;
        /** pump_channel_code0 : R/W; bitpos: [31:27]; default: 0;
         *  configure cmd0 code
         */
        uint32_t pump_channel_code0:5;
    };
    uint32_t val;
} pvt_pmup_channel_cfg_reg_t;

/** Type of clk_cfg register
 *  configure pvt clk
 */
typedef union {
    struct {
        /** pump_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  needs field desc
         */
        uint32_t pump_clk_div_num:8;
        /** monitor_clk_pvt_en : R/W; bitpos: [8]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_clk_pvt_en:1;
        uint32_t reserved_9:22;
        /** clk_sel : R/W; bitpos: [31]; default: 0;
         *  select pvt clk
         */
        uint32_t clk_sel:1;
    };
    uint32_t val;
} pvt_clk_cfg_reg_t;

/** Type of dbias_channel_sel0 register
 *  needs desc
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** dbias_channel3_sel : R/W; bitpos: [10:4]; default: 64;
         *  needs field desc
         */
        uint32_t dbias_channel3_sel:7;
        /** dbias_channel2_sel : R/W; bitpos: [17:11]; default: 64;
         *  needs field desc
         */
        uint32_t dbias_channel2_sel:7;
        /** dbias_channel1_sel : R/W; bitpos: [24:18]; default: 64;
         *  needs field desc
         */
        uint32_t dbias_channel1_sel:7;
        /** dbias_channel0_sel : R/W; bitpos: [31:25]; default: 64;
         *  needs field desc
         */
        uint32_t dbias_channel0_sel:7;
    };
    uint32_t val;
} pvt_dbias_channel_sel0_reg_t;

/** Type of dbias_channel_sel1 register
 *  needs desc
 */
typedef union {
    struct {
        uint32_t reserved_0:25;
        /** dbias_channel4_sel : R/W; bitpos: [31:25]; default: 64;
         *  needs field desc
         */
        uint32_t dbias_channel4_sel:7;
    };
    uint32_t val;
} pvt_dbias_channel_sel1_reg_t;

/** Type of dbias_channel0_sel register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_channel0_cfg : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_channel0_cfg:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_channel0_sel_reg_t;

/** Type of dbias_channel1_sel register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_channel1_cfg : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_channel1_cfg:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_channel1_sel_reg_t;

/** Type of dbias_channel2_sel register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_channel2_cfg : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_channel2_cfg:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_channel2_sel_reg_t;

/** Type of dbias_channel3_sel register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_channel3_cfg : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_channel3_cfg:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_channel3_sel_reg_t;

/** Type of dbias_channel4_sel register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_channel4_cfg : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_channel4_cfg:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_channel4_sel_reg_t;

/** Type of dbias_cmd0 register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_cmd0 : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_cmd0:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_cmd0_reg_t;

/** Type of dbias_cmd1 register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_cmd1 : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_cmd1:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_cmd1_reg_t;

/** Type of dbias_cmd2 register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_cmd2 : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_cmd2:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_cmd2_reg_t;

/** Type of dbias_cmd3 register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_cmd3 : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_cmd3:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_cmd3_reg_t;

/** Type of dbias_cmd4 register
 *  needs desc
 */
typedef union {
    struct {
        /** dbias_cmd4 : R/W; bitpos: [16:0]; default: 0;
         *  needs field desc
         */
        uint32_t dbias_cmd4:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} pvt_dbias_cmd4_reg_t;

/** Type of dbias_timer register
 *  needs desc
 */
typedef union {
    struct {
        uint32_t reserved_0:15;
        /** timer_target : R/W; bitpos: [30:15]; default: 65535;
         *  needs field desc
         */
        uint32_t timer_target:16;
        /** timer_en : R/W; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timer_en:1;
    };
    uint32_t val;
} pvt_dbias_timer_reg_t;

/** Type of comb_pd_site0_unit0_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site0_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site0_unit0:1;
        /** timing_err_cnt_clr_vt0_pd_site0_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site0_unit0:1;
        /** delay_limit_vt0_pd_site0_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site0_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site0_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site0_unit0:8;
        /** timing_err_vt0_pd_site0_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site0_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit0_vt0_conf1_reg_t;

/** Type of comb_pd_site0_unit1_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site0_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site0_unit1:1;
        /** timing_err_cnt_clr_vt0_pd_site0_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site0_unit1:1;
        /** delay_limit_vt0_pd_site0_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site0_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site0_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site0_unit1:8;
        /** timing_err_vt0_pd_site0_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site0_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit1_vt0_conf1_reg_t;

/** Type of comb_pd_site0_unit2_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site0_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site0_unit2:1;
        /** timing_err_cnt_clr_vt0_pd_site0_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site0_unit2:1;
        /** delay_limit_vt0_pd_site0_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site0_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site0_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site0_unit2:8;
        /** timing_err_vt0_pd_site0_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site0_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit2_vt0_conf1_reg_t;

/** Type of comb_pd_site0_unit3_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site0_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site0_unit3:1;
        /** timing_err_cnt_clr_vt0_pd_site0_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site0_unit3:1;
        /** delay_limit_vt0_pd_site0_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site0_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site0_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site0_unit3:8;
        /** timing_err_vt0_pd_site0_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site0_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit3_vt0_conf1_reg_t;

/** Type of comb_pd_site0_unit0_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site0_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site0_unit0:1;
        /** timing_err_cnt_clr_vt1_pd_site0_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site0_unit0:1;
        /** delay_limit_vt1_pd_site0_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site0_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site0_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site0_unit0:8;
        /** timing_err_vt1_pd_site0_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site0_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit0_vt1_conf1_reg_t;

/** Type of comb_pd_site0_unit1_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site0_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site0_unit1:1;
        /** timing_err_cnt_clr_vt1_pd_site0_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site0_unit1:1;
        /** delay_limit_vt1_pd_site0_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site0_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site0_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site0_unit1:8;
        /** timing_err_vt1_pd_site0_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site0_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit1_vt1_conf1_reg_t;

/** Type of comb_pd_site0_unit2_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site0_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site0_unit2:1;
        /** timing_err_cnt_clr_vt1_pd_site0_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site0_unit2:1;
        /** delay_limit_vt1_pd_site0_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site0_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site0_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site0_unit2:8;
        /** timing_err_vt1_pd_site0_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site0_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit2_vt1_conf1_reg_t;

/** Type of comb_pd_site0_unit3_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site0_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site0_unit3:1;
        /** timing_err_cnt_clr_vt1_pd_site0_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site0_unit3:1;
        /** delay_limit_vt1_pd_site0_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site0_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site0_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site0_unit3:8;
        /** timing_err_vt1_pd_site0_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site0_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit3_vt1_conf1_reg_t;

/** Type of comb_pd_site0_unit0_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site0_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site0_unit0:1;
        /** timing_err_cnt_clr_vt2_pd_site0_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site0_unit0:1;
        /** delay_limit_vt2_pd_site0_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site0_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site0_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site0_unit0:8;
        /** timing_err_vt2_pd_site0_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site0_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit0_vt2_conf1_reg_t;

/** Type of comb_pd_site0_unit1_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site0_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site0_unit1:1;
        /** timing_err_cnt_clr_vt2_pd_site0_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site0_unit1:1;
        /** delay_limit_vt2_pd_site0_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site0_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site0_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site0_unit1:8;
        /** timing_err_vt2_pd_site0_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site0_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit1_vt2_conf1_reg_t;

/** Type of comb_pd_site0_unit2_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site0_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site0_unit2:1;
        /** timing_err_cnt_clr_vt2_pd_site0_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site0_unit2:1;
        /** delay_limit_vt2_pd_site0_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site0_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site0_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site0_unit2:8;
        /** timing_err_vt2_pd_site0_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site0_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit2_vt2_conf1_reg_t;

/** Type of comb_pd_site0_unit3_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site0_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site0_unit3:1;
        /** timing_err_cnt_clr_vt2_pd_site0_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site0_unit3:1;
        /** delay_limit_vt2_pd_site0_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site0_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site0_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site0_unit3:8;
        /** timing_err_vt2_pd_site0_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site0_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit3_vt2_conf1_reg_t;

/** Type of comb_pd_site1_unit0_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site1_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site1_unit0:1;
        /** timing_err_cnt_clr_vt0_pd_site1_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site1_unit0:1;
        /** delay_limit_vt0_pd_site1_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site1_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site1_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site1_unit0:8;
        /** timing_err_vt0_pd_site1_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site1_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit0_vt0_conf1_reg_t;

/** Type of comb_pd_site1_unit1_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site1_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site1_unit1:1;
        /** timing_err_cnt_clr_vt0_pd_site1_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site1_unit1:1;
        /** delay_limit_vt0_pd_site1_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site1_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site1_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site1_unit1:8;
        /** timing_err_vt0_pd_site1_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site1_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit1_vt0_conf1_reg_t;

/** Type of comb_pd_site1_unit2_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site1_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site1_unit2:1;
        /** timing_err_cnt_clr_vt0_pd_site1_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site1_unit2:1;
        /** delay_limit_vt0_pd_site1_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site1_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site1_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site1_unit2:8;
        /** timing_err_vt0_pd_site1_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site1_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit2_vt0_conf1_reg_t;

/** Type of comb_pd_site1_unit3_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site1_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site1_unit3:1;
        /** timing_err_cnt_clr_vt0_pd_site1_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site1_unit3:1;
        /** delay_limit_vt0_pd_site1_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site1_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site1_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site1_unit3:8;
        /** timing_err_vt0_pd_site1_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site1_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit3_vt0_conf1_reg_t;

/** Type of comb_pd_site1_unit0_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site1_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site1_unit0:1;
        /** timing_err_cnt_clr_vt1_pd_site1_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site1_unit0:1;
        /** delay_limit_vt1_pd_site1_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site1_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site1_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site1_unit0:8;
        /** timing_err_vt1_pd_site1_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site1_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit0_vt1_conf1_reg_t;

/** Type of comb_pd_site1_unit1_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site1_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site1_unit1:1;
        /** timing_err_cnt_clr_vt1_pd_site1_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site1_unit1:1;
        /** delay_limit_vt1_pd_site1_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site1_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site1_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site1_unit1:8;
        /** timing_err_vt1_pd_site1_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site1_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit1_vt1_conf1_reg_t;

/** Type of comb_pd_site1_unit2_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site1_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site1_unit2:1;
        /** timing_err_cnt_clr_vt1_pd_site1_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site1_unit2:1;
        /** delay_limit_vt1_pd_site1_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site1_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site1_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site1_unit2:8;
        /** timing_err_vt1_pd_site1_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site1_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit2_vt1_conf1_reg_t;

/** Type of comb_pd_site1_unit3_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site1_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site1_unit3:1;
        /** timing_err_cnt_clr_vt1_pd_site1_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site1_unit3:1;
        /** delay_limit_vt1_pd_site1_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site1_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site1_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site1_unit3:8;
        /** timing_err_vt1_pd_site1_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site1_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit3_vt1_conf1_reg_t;

/** Type of comb_pd_site1_unit0_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site1_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site1_unit0:1;
        /** timing_err_cnt_clr_vt2_pd_site1_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site1_unit0:1;
        /** delay_limit_vt2_pd_site1_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site1_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site1_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site1_unit0:8;
        /** timing_err_vt2_pd_site1_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site1_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit0_vt2_conf1_reg_t;

/** Type of comb_pd_site1_unit1_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site1_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site1_unit1:1;
        /** timing_err_cnt_clr_vt2_pd_site1_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site1_unit1:1;
        /** delay_limit_vt2_pd_site1_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site1_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site1_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site1_unit1:8;
        /** timing_err_vt2_pd_site1_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site1_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit1_vt2_conf1_reg_t;

/** Type of comb_pd_site1_unit2_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site1_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site1_unit2:1;
        /** timing_err_cnt_clr_vt2_pd_site1_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site1_unit2:1;
        /** delay_limit_vt2_pd_site1_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site1_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site1_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site1_unit2:8;
        /** timing_err_vt2_pd_site1_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site1_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit2_vt2_conf1_reg_t;

/** Type of comb_pd_site1_unit3_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site1_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site1_unit3:1;
        /** timing_err_cnt_clr_vt2_pd_site1_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site1_unit3:1;
        /** delay_limit_vt2_pd_site1_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site1_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site1_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site1_unit3:8;
        /** timing_err_vt2_pd_site1_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site1_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit3_vt2_conf1_reg_t;

/** Type of comb_pd_site2_unit0_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site2_unit0 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site2_unit0:1;
        /** timing_err_cnt_clr_vt0_pd_site2_unit0 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site2_unit0:1;
        /** delay_limit_vt0_pd_site2_unit0 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site2_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site2_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site2_unit0:8;
        /** timing_err_vt0_pd_site2_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site2_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit0_vt0_conf1_reg_t;

/** Type of comb_pd_site2_unit1_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site2_unit1 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site2_unit1:1;
        /** timing_err_cnt_clr_vt0_pd_site2_unit1 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site2_unit1:1;
        /** delay_limit_vt0_pd_site2_unit1 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site2_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site2_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site2_unit1:8;
        /** timing_err_vt0_pd_site2_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site2_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit1_vt0_conf1_reg_t;

/** Type of comb_pd_site2_unit2_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site2_unit2 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site2_unit2:1;
        /** timing_err_cnt_clr_vt0_pd_site2_unit2 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site2_unit2:1;
        /** delay_limit_vt0_pd_site2_unit2 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site2_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site2_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site2_unit2:8;
        /** timing_err_vt0_pd_site2_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site2_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit2_vt0_conf1_reg_t;

/** Type of comb_pd_site2_unit3_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site2_unit3 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site2_unit3:1;
        /** timing_err_cnt_clr_vt0_pd_site2_unit3 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site2_unit3:1;
        /** delay_limit_vt0_pd_site2_unit3 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site2_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site2_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site2_unit3:8;
        /** timing_err_vt0_pd_site2_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site2_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit3_vt0_conf1_reg_t;

/** Type of comb_pd_site2_unit0_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site2_unit0 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site2_unit0:1;
        /** timing_err_cnt_clr_vt1_pd_site2_unit0 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site2_unit0:1;
        /** delay_limit_vt1_pd_site2_unit0 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site2_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site2_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site2_unit0:8;
        /** timing_err_vt1_pd_site2_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site2_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit0_vt1_conf1_reg_t;

/** Type of comb_pd_site2_unit1_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site2_unit1 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site2_unit1:1;
        /** timing_err_cnt_clr_vt1_pd_site2_unit1 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site2_unit1:1;
        /** delay_limit_vt1_pd_site2_unit1 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site2_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site2_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site2_unit1:8;
        /** timing_err_vt1_pd_site2_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site2_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit1_vt1_conf1_reg_t;

/** Type of comb_pd_site2_unit2_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site2_unit2 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site2_unit2:1;
        /** timing_err_cnt_clr_vt1_pd_site2_unit2 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site2_unit2:1;
        /** delay_limit_vt1_pd_site2_unit2 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site2_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site2_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site2_unit2:8;
        /** timing_err_vt1_pd_site2_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site2_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit2_vt1_conf1_reg_t;

/** Type of comb_pd_site2_unit3_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site2_unit3 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site2_unit3:1;
        /** timing_err_cnt_clr_vt1_pd_site2_unit3 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site2_unit3:1;
        /** delay_limit_vt1_pd_site2_unit3 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site2_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site2_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site2_unit3:8;
        /** timing_err_vt1_pd_site2_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site2_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit3_vt1_conf1_reg_t;

/** Type of comb_pd_site2_unit0_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site2_unit0 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site2_unit0:1;
        /** timing_err_cnt_clr_vt2_pd_site2_unit0 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site2_unit0:1;
        /** delay_limit_vt2_pd_site2_unit0 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site2_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site2_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site2_unit0:8;
        /** timing_err_vt2_pd_site2_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site2_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit0_vt2_conf1_reg_t;

/** Type of comb_pd_site2_unit1_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site2_unit1 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site2_unit1:1;
        /** timing_err_cnt_clr_vt2_pd_site2_unit1 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site2_unit1:1;
        /** delay_limit_vt2_pd_site2_unit1 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site2_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site2_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site2_unit1:8;
        /** timing_err_vt2_pd_site2_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site2_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit1_vt2_conf1_reg_t;

/** Type of comb_pd_site2_unit2_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site2_unit2 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site2_unit2:1;
        /** timing_err_cnt_clr_vt2_pd_site2_unit2 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site2_unit2:1;
        /** delay_limit_vt2_pd_site2_unit2 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site2_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site2_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site2_unit2:8;
        /** timing_err_vt2_pd_site2_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site2_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit2_vt2_conf1_reg_t;

/** Type of comb_pd_site2_unit3_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site2_unit3 : R/W; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site2_unit3:1;
        /** timing_err_cnt_clr_vt2_pd_site2_unit3 : WT; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site2_unit3:1;
        /** delay_limit_vt2_pd_site2_unit3 : R/W; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site2_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site2_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site2_unit3:8;
        /** timing_err_vt2_pd_site2_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site2_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit3_vt2_conf1_reg_t;

/** Type of comb_pd_site3_unit0_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site3_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site3_unit0:1;
        /** timing_err_cnt_clr_vt0_pd_site3_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site3_unit0:1;
        /** delay_limit_vt0_pd_site3_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site3_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site3_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site3_unit0:8;
        /** timing_err_vt0_pd_site3_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site3_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit0_vt0_conf1_reg_t;

/** Type of comb_pd_site3_unit1_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site3_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site3_unit1:1;
        /** timing_err_cnt_clr_vt0_pd_site3_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site3_unit1:1;
        /** delay_limit_vt0_pd_site3_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site3_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site3_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site3_unit1:8;
        /** timing_err_vt0_pd_site3_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site3_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit1_vt0_conf1_reg_t;

/** Type of comb_pd_site3_unit2_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site3_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site3_unit2:1;
        /** timing_err_cnt_clr_vt0_pd_site3_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site3_unit2:1;
        /** delay_limit_vt0_pd_site3_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site3_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site3_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site3_unit2:8;
        /** timing_err_vt0_pd_site3_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site3_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit2_vt0_conf1_reg_t;

/** Type of comb_pd_site3_unit3_vt0_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt0_pd_site3_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt0_pd_site3_unit3:1;
        /** timing_err_cnt_clr_vt0_pd_site3_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt0_pd_site3_unit3:1;
        /** delay_limit_vt0_pd_site3_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt0_pd_site3_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt0_pd_site3_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt0_pd_site3_unit3:8;
        /** timing_err_vt0_pd_site3_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt0_pd_site3_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit3_vt0_conf1_reg_t;

/** Type of comb_pd_site3_unit0_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site3_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site3_unit0:1;
        /** timing_err_cnt_clr_vt1_pd_site3_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site3_unit0:1;
        /** delay_limit_vt1_pd_site3_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site3_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site3_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site3_unit0:8;
        /** timing_err_vt1_pd_site3_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site3_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit0_vt1_conf1_reg_t;

/** Type of comb_pd_site3_unit1_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site3_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site3_unit1:1;
        /** timing_err_cnt_clr_vt1_pd_site3_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site3_unit1:1;
        /** delay_limit_vt1_pd_site3_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site3_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site3_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site3_unit1:8;
        /** timing_err_vt1_pd_site3_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site3_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit1_vt1_conf1_reg_t;

/** Type of comb_pd_site3_unit2_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site3_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site3_unit2:1;
        /** timing_err_cnt_clr_vt1_pd_site3_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site3_unit2:1;
        /** delay_limit_vt1_pd_site3_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site3_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site3_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site3_unit2:8;
        /** timing_err_vt1_pd_site3_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site3_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit2_vt1_conf1_reg_t;

/** Type of comb_pd_site3_unit3_vt1_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt1_pd_site3_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt1_pd_site3_unit3:1;
        /** timing_err_cnt_clr_vt1_pd_site3_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt1_pd_site3_unit3:1;
        /** delay_limit_vt1_pd_site3_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt1_pd_site3_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt1_pd_site3_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt1_pd_site3_unit3:8;
        /** timing_err_vt1_pd_site3_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt1_pd_site3_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit3_vt1_conf1_reg_t;

/** Type of comb_pd_site3_unit0_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site3_unit0 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site3_unit0:1;
        /** timing_err_cnt_clr_vt2_pd_site3_unit0 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site3_unit0:1;
        /** delay_limit_vt2_pd_site3_unit0 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site3_unit0:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site3_unit0 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site3_unit0:8;
        /** timing_err_vt2_pd_site3_unit0 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site3_unit0:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit0_vt2_conf1_reg_t;

/** Type of comb_pd_site3_unit1_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site3_unit1 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site3_unit1:1;
        /** timing_err_cnt_clr_vt2_pd_site3_unit1 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site3_unit1:1;
        /** delay_limit_vt2_pd_site3_unit1 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site3_unit1:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site3_unit1 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site3_unit1:8;
        /** timing_err_vt2_pd_site3_unit1 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site3_unit1:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit1_vt2_conf1_reg_t;

/** Type of comb_pd_site3_unit2_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site3_unit2 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site3_unit2:1;
        /** timing_err_cnt_clr_vt2_pd_site3_unit2 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site3_unit2:1;
        /** delay_limit_vt2_pd_site3_unit2 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site3_unit2:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site3_unit2 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site3_unit2:8;
        /** timing_err_vt2_pd_site3_unit2 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site3_unit2:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit2_vt2_conf1_reg_t;

/** Type of comb_pd_site3_unit3_vt2_conf1 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_en_vt2_pd_site3_unit3 : HRO; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_en_vt2_pd_site3_unit3:1;
        /** timing_err_cnt_clr_vt2_pd_site3_unit3 : HRO; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_clr_vt2_pd_site3_unit3:1;
        /** delay_limit_vt2_pd_site3_unit3 : HRO; bitpos: [9:2]; default: 20;
         *  needs field desc
         */
        uint32_t delay_limit_vt2_pd_site3_unit3:8;
        uint32_t reserved_10:13;
        /** delay_num_o_vt2_pd_site3_unit3 : RO; bitpos: [30:23]; default: 0;
         *  needs field desc
         */
        uint32_t delay_num_o_vt2_pd_site3_unit3:8;
        /** timing_err_vt2_pd_site3_unit3 : RO; bitpos: [31]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_vt2_pd_site3_unit3:1;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit3_vt2_conf1_reg_t;

/** Type of comb_pd_site0_unit0_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site0_unit0 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site0_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site0_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site0_unit0:1;
        /** timing_err_cnt_o_vt0_pd_site0_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site0_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit0_vt0_conf2_reg_t;

/** Type of comb_pd_site0_unit1_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site0_unit1 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site0_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site0_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site0_unit1:1;
        /** timing_err_cnt_o_vt0_pd_site0_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site0_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit1_vt0_conf2_reg_t;

/** Type of comb_pd_site0_unit2_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site0_unit2 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site0_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site0_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site0_unit2:1;
        /** timing_err_cnt_o_vt0_pd_site0_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site0_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit2_vt0_conf2_reg_t;

/** Type of comb_pd_site0_unit3_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site0_unit3 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site0_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site0_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site0_unit3:1;
        /** timing_err_cnt_o_vt0_pd_site0_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site0_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit3_vt0_conf2_reg_t;

/** Type of comb_pd_site0_unit0_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site0_unit0 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site0_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site0_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site0_unit0:1;
        /** timing_err_cnt_o_vt1_pd_site0_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site0_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit0_vt1_conf2_reg_t;

/** Type of comb_pd_site0_unit1_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site0_unit1 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site0_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site0_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site0_unit1:1;
        /** timing_err_cnt_o_vt1_pd_site0_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site0_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit1_vt1_conf2_reg_t;

/** Type of comb_pd_site0_unit2_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site0_unit2 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site0_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site0_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site0_unit2:1;
        /** timing_err_cnt_o_vt1_pd_site0_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site0_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit2_vt1_conf2_reg_t;

/** Type of comb_pd_site0_unit3_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site0_unit3 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site0_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site0_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site0_unit3:1;
        /** timing_err_cnt_o_vt1_pd_site0_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site0_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit3_vt1_conf2_reg_t;

/** Type of comb_pd_site0_unit0_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site0_unit0 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site0_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site0_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site0_unit0:1;
        /** timing_err_cnt_o_vt2_pd_site0_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site0_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit0_vt2_conf2_reg_t;

/** Type of comb_pd_site0_unit1_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site0_unit1 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site0_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site0_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site0_unit1:1;
        /** timing_err_cnt_o_vt2_pd_site0_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site0_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit1_vt2_conf2_reg_t;

/** Type of comb_pd_site0_unit2_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site0_unit2 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site0_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site0_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site0_unit2:1;
        /** timing_err_cnt_o_vt2_pd_site0_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site0_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit2_vt2_conf2_reg_t;

/** Type of comb_pd_site0_unit3_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site0_unit3 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site0_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site0_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site0_unit3:1;
        /** timing_err_cnt_o_vt2_pd_site0_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site0_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site0_unit3_vt2_conf2_reg_t;

/** Type of comb_pd_site1_unit0_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site1_unit0 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site1_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site1_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site1_unit0:1;
        /** timing_err_cnt_o_vt0_pd_site1_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site1_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit0_vt0_conf2_reg_t;

/** Type of comb_pd_site1_unit1_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site1_unit1 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site1_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site1_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site1_unit1:1;
        /** timing_err_cnt_o_vt0_pd_site1_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site1_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit1_vt0_conf2_reg_t;

/** Type of comb_pd_site1_unit2_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site1_unit2 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site1_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site1_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site1_unit2:1;
        /** timing_err_cnt_o_vt0_pd_site1_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site1_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit2_vt0_conf2_reg_t;

/** Type of comb_pd_site1_unit3_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site1_unit3 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site1_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site1_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site1_unit3:1;
        /** timing_err_cnt_o_vt0_pd_site1_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site1_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit3_vt0_conf2_reg_t;

/** Type of comb_pd_site1_unit0_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site1_unit0 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site1_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site1_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site1_unit0:1;
        /** timing_err_cnt_o_vt1_pd_site1_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site1_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit0_vt1_conf2_reg_t;

/** Type of comb_pd_site1_unit1_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site1_unit1 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site1_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site1_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site1_unit1:1;
        /** timing_err_cnt_o_vt1_pd_site1_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site1_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit1_vt1_conf2_reg_t;

/** Type of comb_pd_site1_unit2_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site1_unit2 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site1_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site1_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site1_unit2:1;
        /** timing_err_cnt_o_vt1_pd_site1_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site1_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit2_vt1_conf2_reg_t;

/** Type of comb_pd_site1_unit3_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site1_unit3 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site1_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site1_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site1_unit3:1;
        /** timing_err_cnt_o_vt1_pd_site1_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site1_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit3_vt1_conf2_reg_t;

/** Type of comb_pd_site1_unit0_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site1_unit0 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site1_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site1_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site1_unit0:1;
        /** timing_err_cnt_o_vt2_pd_site1_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site1_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit0_vt2_conf2_reg_t;

/** Type of comb_pd_site1_unit1_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site1_unit1 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site1_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site1_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site1_unit1:1;
        /** timing_err_cnt_o_vt2_pd_site1_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site1_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit1_vt2_conf2_reg_t;

/** Type of comb_pd_site1_unit2_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site1_unit2 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site1_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site1_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site1_unit2:1;
        /** timing_err_cnt_o_vt2_pd_site1_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site1_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit2_vt2_conf2_reg_t;

/** Type of comb_pd_site1_unit3_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site1_unit3 : HRO; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site1_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site1_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site1_unit3:1;
        /** timing_err_cnt_o_vt2_pd_site1_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site1_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site1_unit3_vt2_conf2_reg_t;

/** Type of comb_pd_site2_unit0_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site2_unit0 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site2_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site2_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site2_unit0:1;
        /** timing_err_cnt_o_vt0_pd_site2_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site2_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit0_vt0_conf2_reg_t;

/** Type of comb_pd_site2_unit1_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site2_unit1 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site2_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site2_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site2_unit1:1;
        /** timing_err_cnt_o_vt0_pd_site2_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site2_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit1_vt0_conf2_reg_t;

/** Type of comb_pd_site2_unit2_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site2_unit2 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site2_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site2_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site2_unit2:1;
        /** timing_err_cnt_o_vt0_pd_site2_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site2_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit2_vt0_conf2_reg_t;

/** Type of comb_pd_site2_unit3_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site2_unit3 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site2_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site2_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site2_unit3:1;
        /** timing_err_cnt_o_vt0_pd_site2_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site2_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit3_vt0_conf2_reg_t;

/** Type of comb_pd_site2_unit0_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site2_unit0 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site2_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site2_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site2_unit0:1;
        /** timing_err_cnt_o_vt1_pd_site2_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site2_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit0_vt1_conf2_reg_t;

/** Type of comb_pd_site2_unit1_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site2_unit1 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site2_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site2_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site2_unit1:1;
        /** timing_err_cnt_o_vt1_pd_site2_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site2_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit1_vt1_conf2_reg_t;

/** Type of comb_pd_site2_unit2_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site2_unit2 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site2_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site2_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site2_unit2:1;
        /** timing_err_cnt_o_vt1_pd_site2_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site2_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit2_vt1_conf2_reg_t;

/** Type of comb_pd_site2_unit3_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site2_unit3 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site2_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site2_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site2_unit3:1;
        /** timing_err_cnt_o_vt1_pd_site2_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site2_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit3_vt1_conf2_reg_t;

/** Type of comb_pd_site2_unit0_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site2_unit0 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site2_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site2_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site2_unit0:1;
        /** timing_err_cnt_o_vt2_pd_site2_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site2_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit0_vt2_conf2_reg_t;

/** Type of comb_pd_site2_unit1_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site2_unit1 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site2_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site2_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site2_unit1:1;
        /** timing_err_cnt_o_vt2_pd_site2_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site2_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit1_vt2_conf2_reg_t;

/** Type of comb_pd_site2_unit2_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site2_unit2 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site2_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site2_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site2_unit2:1;
        /** timing_err_cnt_o_vt2_pd_site2_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site2_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit2_vt2_conf2_reg_t;

/** Type of comb_pd_site2_unit3_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site2_unit3 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site2_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site2_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site2_unit3:1;
        /** timing_err_cnt_o_vt2_pd_site2_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site2_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site2_unit3_vt2_conf2_reg_t;

/** Type of comb_pd_site3_unit0_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site3_unit0 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site3_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site3_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site3_unit0:1;
        /** timing_err_cnt_o_vt0_pd_site3_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site3_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit0_vt0_conf2_reg_t;

/** Type of comb_pd_site3_unit1_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site3_unit1 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site3_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site3_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site3_unit1:1;
        /** timing_err_cnt_o_vt0_pd_site3_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site3_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit1_vt0_conf2_reg_t;

/** Type of comb_pd_site3_unit2_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site3_unit2 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site3_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site3_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site3_unit2:1;
        /** timing_err_cnt_o_vt0_pd_site3_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site3_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit2_vt0_conf2_reg_t;

/** Type of comb_pd_site3_unit3_vt0_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt0_pd_site3_unit3 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt0_pd_site3_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt0_pd_site3_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt0_pd_site3_unit3:1;
        /** timing_err_cnt_o_vt0_pd_site3_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt0_pd_site3_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit3_vt0_conf2_reg_t;

/** Type of comb_pd_site3_unit0_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site3_unit0 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site3_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site3_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site3_unit0:1;
        /** timing_err_cnt_o_vt1_pd_site3_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site3_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit0_vt1_conf2_reg_t;

/** Type of comb_pd_site3_unit1_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site3_unit1 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site3_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site3_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site3_unit1:1;
        /** timing_err_cnt_o_vt1_pd_site3_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site3_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit1_vt1_conf2_reg_t;

/** Type of comb_pd_site3_unit2_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site3_unit2 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site3_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site3_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site3_unit2:1;
        /** timing_err_cnt_o_vt1_pd_site3_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site3_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit2_vt1_conf2_reg_t;

/** Type of comb_pd_site3_unit3_vt1_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt1_pd_site3_unit3 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt1_pd_site3_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt1_pd_site3_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt1_pd_site3_unit3:1;
        /** timing_err_cnt_o_vt1_pd_site3_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt1_pd_site3_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit3_vt1_conf2_reg_t;

/** Type of comb_pd_site3_unit0_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site3_unit0 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site3_unit0:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site3_unit0 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site3_unit0:1;
        /** timing_err_cnt_o_vt2_pd_site3_unit0 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site3_unit0:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit0_vt2_conf2_reg_t;

/** Type of comb_pd_site3_unit1_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site3_unit1 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site3_unit1:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site3_unit1 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site3_unit1:1;
        /** timing_err_cnt_o_vt2_pd_site3_unit1 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site3_unit1:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit1_vt2_conf2_reg_t;

/** Type of comb_pd_site3_unit2_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site3_unit2 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site3_unit2:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site3_unit2 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site3_unit2:1;
        /** timing_err_cnt_o_vt2_pd_site3_unit2 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site3_unit2:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit2_vt2_conf2_reg_t;

/** Type of comb_pd_site3_unit3_vt2_conf2 register
 *  needs desc
 */
typedef union {
    struct {
        /** monitor_edg_mod_vt2_pd_site3_unit3 : R/W; bitpos: [1:0]; default: 0;
         *  needs field desc
         */
        uint32_t monitor_edg_mod_vt2_pd_site3_unit3:2;
        uint32_t reserved_2:13;
        /** delay_ovf_vt2_pd_site3_unit3 : RO; bitpos: [15]; default: 0;
         *  needs field desc
         */
        uint32_t delay_ovf_vt2_pd_site3_unit3:1;
        /** timing_err_cnt_o_vt2_pd_site3_unit3 : RO; bitpos: [31:16]; default: 0;
         *  needs field desc
         */
        uint32_t timing_err_cnt_o_vt2_pd_site3_unit3:16;
    };
    uint32_t val;
} pvt_comb_pd_site3_unit3_vt2_conf2_reg_t;

/** Type of value_update register
 *  needs field desc
 */
typedef union {
    struct {
        /** value_update : WT; bitpos: [0]; default: 0;
         *  needs field desc
         */
        uint32_t value_update:1;
        /** value_update_bypass : R/W; bitpos: [1]; default: 0;
         *  needs field desc
         */
        uint32_t value_update_bypass:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} pvt_value_update_reg_t;

/** Type of bypass_chain register
 *  needs field desc
 */
typedef union {
    struct {
        /** clk_chain_en : R/W; bitpos: [31:0]; default: 4294967295;
         *  needs field desc
         */
        uint32_t clk_chain_en:32;
    };
    uint32_t val;
} pvt_bypass_chain_reg_t;


/** Group: version register */
/** Type of date register
 *  version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 34677040;
         *  version register
         */
        uint32_t date:32;
    };
    uint32_t val;
} pvt_date_reg_t;


typedef struct {
    volatile pvt_pmup_bitmap_high0_reg_t pmup_bitmap_high0;
    volatile pvt_pmup_bitmap_high1_reg_t pmup_bitmap_high1;
    volatile pvt_pmup_bitmap_high2_reg_t pmup_bitmap_high2;
    volatile pvt_pmup_bitmap_high3_reg_t pmup_bitmap_high3;
    volatile pvt_pmup_bitmap_high4_reg_t pmup_bitmap_high4;
    volatile pvt_pmup_bitmap_low0_reg_t pmup_bitmap_low0;
    volatile pvt_pmup_bitmap_low1_reg_t pmup_bitmap_low1;
    volatile pvt_pmup_bitmap_low2_reg_t pmup_bitmap_low2;
    volatile pvt_pmup_bitmap_low3_reg_t pmup_bitmap_low3;
    volatile pvt_pmup_bitmap_low4_reg_t pmup_bitmap_low4;
    volatile pvt_pmup_drv_cfg_reg_t pmup_drv_cfg;
    volatile pvt_pmup_channel_cfg_reg_t pmup_channel_cfg;
    volatile pvt_clk_cfg_reg_t clk_cfg;
    volatile pvt_dbias_channel_sel0_reg_t dbias_channel_sel0;
    volatile pvt_dbias_channel_sel1_reg_t dbias_channel_sel1;
    volatile pvt_dbias_channel0_sel_reg_t dbias_channel0_sel;
    volatile pvt_dbias_channel1_sel_reg_t dbias_channel1_sel;
    volatile pvt_dbias_channel2_sel_reg_t dbias_channel2_sel;
    volatile pvt_dbias_channel3_sel_reg_t dbias_channel3_sel;
    volatile pvt_dbias_channel4_sel_reg_t dbias_channel4_sel;
    volatile pvt_dbias_cmd0_reg_t dbias_cmd0;
    volatile pvt_dbias_cmd1_reg_t dbias_cmd1;
    volatile pvt_dbias_cmd2_reg_t dbias_cmd2;
    volatile pvt_dbias_cmd3_reg_t dbias_cmd3;
    volatile pvt_dbias_cmd4_reg_t dbias_cmd4;
    volatile pvt_dbias_timer_reg_t dbias_timer;
    volatile pvt_comb_pd_site0_unit0_vt0_conf1_reg_t comb_pd_site0_unit0_vt0_conf1;
    volatile pvt_comb_pd_site0_unit1_vt0_conf1_reg_t comb_pd_site0_unit1_vt0_conf1;
    volatile pvt_comb_pd_site0_unit2_vt0_conf1_reg_t comb_pd_site0_unit2_vt0_conf1;
    volatile pvt_comb_pd_site0_unit3_vt0_conf1_reg_t comb_pd_site0_unit3_vt0_conf1;
    volatile pvt_comb_pd_site0_unit0_vt1_conf1_reg_t comb_pd_site0_unit0_vt1_conf1;
    volatile pvt_comb_pd_site0_unit1_vt1_conf1_reg_t comb_pd_site0_unit1_vt1_conf1;
    volatile pvt_comb_pd_site0_unit2_vt1_conf1_reg_t comb_pd_site0_unit2_vt1_conf1;
    volatile pvt_comb_pd_site0_unit3_vt1_conf1_reg_t comb_pd_site0_unit3_vt1_conf1;
    volatile pvt_comb_pd_site0_unit0_vt2_conf1_reg_t comb_pd_site0_unit0_vt2_conf1;
    volatile pvt_comb_pd_site0_unit1_vt2_conf1_reg_t comb_pd_site0_unit1_vt2_conf1;
    volatile pvt_comb_pd_site0_unit2_vt2_conf1_reg_t comb_pd_site0_unit2_vt2_conf1;
    volatile pvt_comb_pd_site0_unit3_vt2_conf1_reg_t comb_pd_site0_unit3_vt2_conf1;
    volatile pvt_comb_pd_site1_unit0_vt0_conf1_reg_t comb_pd_site1_unit0_vt0_conf1;
    volatile pvt_comb_pd_site1_unit1_vt0_conf1_reg_t comb_pd_site1_unit1_vt0_conf1;
    volatile pvt_comb_pd_site1_unit2_vt0_conf1_reg_t comb_pd_site1_unit2_vt0_conf1;
    volatile pvt_comb_pd_site1_unit3_vt0_conf1_reg_t comb_pd_site1_unit3_vt0_conf1;
    volatile pvt_comb_pd_site1_unit0_vt1_conf1_reg_t comb_pd_site1_unit0_vt1_conf1;
    volatile pvt_comb_pd_site1_unit1_vt1_conf1_reg_t comb_pd_site1_unit1_vt1_conf1;
    volatile pvt_comb_pd_site1_unit2_vt1_conf1_reg_t comb_pd_site1_unit2_vt1_conf1;
    volatile pvt_comb_pd_site1_unit3_vt1_conf1_reg_t comb_pd_site1_unit3_vt1_conf1;
    volatile pvt_comb_pd_site1_unit0_vt2_conf1_reg_t comb_pd_site1_unit0_vt2_conf1;
    volatile pvt_comb_pd_site1_unit1_vt2_conf1_reg_t comb_pd_site1_unit1_vt2_conf1;
    volatile pvt_comb_pd_site1_unit2_vt2_conf1_reg_t comb_pd_site1_unit2_vt2_conf1;
    volatile pvt_comb_pd_site1_unit3_vt2_conf1_reg_t comb_pd_site1_unit3_vt2_conf1;
    volatile pvt_comb_pd_site2_unit0_vt0_conf1_reg_t comb_pd_site2_unit0_vt0_conf1;
    volatile pvt_comb_pd_site2_unit1_vt0_conf1_reg_t comb_pd_site2_unit1_vt0_conf1;
    volatile pvt_comb_pd_site2_unit2_vt0_conf1_reg_t comb_pd_site2_unit2_vt0_conf1;
    volatile pvt_comb_pd_site2_unit3_vt0_conf1_reg_t comb_pd_site2_unit3_vt0_conf1;
    volatile pvt_comb_pd_site2_unit0_vt1_conf1_reg_t comb_pd_site2_unit0_vt1_conf1;
    volatile pvt_comb_pd_site2_unit1_vt1_conf1_reg_t comb_pd_site2_unit1_vt1_conf1;
    volatile pvt_comb_pd_site2_unit2_vt1_conf1_reg_t comb_pd_site2_unit2_vt1_conf1;
    volatile pvt_comb_pd_site2_unit3_vt1_conf1_reg_t comb_pd_site2_unit3_vt1_conf1;
    volatile pvt_comb_pd_site2_unit0_vt2_conf1_reg_t comb_pd_site2_unit0_vt2_conf1;
    volatile pvt_comb_pd_site2_unit1_vt2_conf1_reg_t comb_pd_site2_unit1_vt2_conf1;
    volatile pvt_comb_pd_site2_unit2_vt2_conf1_reg_t comb_pd_site2_unit2_vt2_conf1;
    volatile pvt_comb_pd_site2_unit3_vt2_conf1_reg_t comb_pd_site2_unit3_vt2_conf1;
    volatile pvt_comb_pd_site3_unit0_vt0_conf1_reg_t comb_pd_site3_unit0_vt0_conf1;
    volatile pvt_comb_pd_site3_unit1_vt0_conf1_reg_t comb_pd_site3_unit1_vt0_conf1;
    volatile pvt_comb_pd_site3_unit2_vt0_conf1_reg_t comb_pd_site3_unit2_vt0_conf1;
    volatile pvt_comb_pd_site3_unit3_vt0_conf1_reg_t comb_pd_site3_unit3_vt0_conf1;
    volatile pvt_comb_pd_site3_unit0_vt1_conf1_reg_t comb_pd_site3_unit0_vt1_conf1;
    volatile pvt_comb_pd_site3_unit1_vt1_conf1_reg_t comb_pd_site3_unit1_vt1_conf1;
    volatile pvt_comb_pd_site3_unit2_vt1_conf1_reg_t comb_pd_site3_unit2_vt1_conf1;
    volatile pvt_comb_pd_site3_unit3_vt1_conf1_reg_t comb_pd_site3_unit3_vt1_conf1;
    volatile pvt_comb_pd_site3_unit0_vt2_conf1_reg_t comb_pd_site3_unit0_vt2_conf1;
    volatile pvt_comb_pd_site3_unit1_vt2_conf1_reg_t comb_pd_site3_unit1_vt2_conf1;
    volatile pvt_comb_pd_site3_unit2_vt2_conf1_reg_t comb_pd_site3_unit2_vt2_conf1;
    volatile pvt_comb_pd_site3_unit3_vt2_conf1_reg_t comb_pd_site3_unit3_vt2_conf1;
    volatile pvt_comb_pd_site0_unit0_vt0_conf2_reg_t comb_pd_site0_unit0_vt0_conf2;
    volatile pvt_comb_pd_site0_unit1_vt0_conf2_reg_t comb_pd_site0_unit1_vt0_conf2;
    volatile pvt_comb_pd_site0_unit2_vt0_conf2_reg_t comb_pd_site0_unit2_vt0_conf2;
    volatile pvt_comb_pd_site0_unit3_vt0_conf2_reg_t comb_pd_site0_unit3_vt0_conf2;
    volatile pvt_comb_pd_site0_unit0_vt1_conf2_reg_t comb_pd_site0_unit0_vt1_conf2;
    volatile pvt_comb_pd_site0_unit1_vt1_conf2_reg_t comb_pd_site0_unit1_vt1_conf2;
    volatile pvt_comb_pd_site0_unit2_vt1_conf2_reg_t comb_pd_site0_unit2_vt1_conf2;
    volatile pvt_comb_pd_site0_unit3_vt1_conf2_reg_t comb_pd_site0_unit3_vt1_conf2;
    volatile pvt_comb_pd_site0_unit0_vt2_conf2_reg_t comb_pd_site0_unit0_vt2_conf2;
    volatile pvt_comb_pd_site0_unit1_vt2_conf2_reg_t comb_pd_site0_unit1_vt2_conf2;
    volatile pvt_comb_pd_site0_unit2_vt2_conf2_reg_t comb_pd_site0_unit2_vt2_conf2;
    volatile pvt_comb_pd_site0_unit3_vt2_conf2_reg_t comb_pd_site0_unit3_vt2_conf2;
    volatile pvt_comb_pd_site1_unit0_vt0_conf2_reg_t comb_pd_site1_unit0_vt0_conf2;
    volatile pvt_comb_pd_site1_unit1_vt0_conf2_reg_t comb_pd_site1_unit1_vt0_conf2;
    volatile pvt_comb_pd_site1_unit2_vt0_conf2_reg_t comb_pd_site1_unit2_vt0_conf2;
    volatile pvt_comb_pd_site1_unit3_vt0_conf2_reg_t comb_pd_site1_unit3_vt0_conf2;
    volatile pvt_comb_pd_site1_unit0_vt1_conf2_reg_t comb_pd_site1_unit0_vt1_conf2;
    volatile pvt_comb_pd_site1_unit1_vt1_conf2_reg_t comb_pd_site1_unit1_vt1_conf2;
    volatile pvt_comb_pd_site1_unit2_vt1_conf2_reg_t comb_pd_site1_unit2_vt1_conf2;
    volatile pvt_comb_pd_site1_unit3_vt1_conf2_reg_t comb_pd_site1_unit3_vt1_conf2;
    volatile pvt_comb_pd_site1_unit0_vt2_conf2_reg_t comb_pd_site1_unit0_vt2_conf2;
    volatile pvt_comb_pd_site1_unit1_vt2_conf2_reg_t comb_pd_site1_unit1_vt2_conf2;
    volatile pvt_comb_pd_site1_unit2_vt2_conf2_reg_t comb_pd_site1_unit2_vt2_conf2;
    volatile pvt_comb_pd_site1_unit3_vt2_conf2_reg_t comb_pd_site1_unit3_vt2_conf2;
    volatile pvt_comb_pd_site2_unit0_vt0_conf2_reg_t comb_pd_site2_unit0_vt0_conf2;
    volatile pvt_comb_pd_site2_unit1_vt0_conf2_reg_t comb_pd_site2_unit1_vt0_conf2;
    volatile pvt_comb_pd_site2_unit2_vt0_conf2_reg_t comb_pd_site2_unit2_vt0_conf2;
    volatile pvt_comb_pd_site2_unit3_vt0_conf2_reg_t comb_pd_site2_unit3_vt0_conf2;
    volatile pvt_comb_pd_site2_unit0_vt1_conf2_reg_t comb_pd_site2_unit0_vt1_conf2;
    volatile pvt_comb_pd_site2_unit1_vt1_conf2_reg_t comb_pd_site2_unit1_vt1_conf2;
    volatile pvt_comb_pd_site2_unit2_vt1_conf2_reg_t comb_pd_site2_unit2_vt1_conf2;
    volatile pvt_comb_pd_site2_unit3_vt1_conf2_reg_t comb_pd_site2_unit3_vt1_conf2;
    volatile pvt_comb_pd_site2_unit0_vt2_conf2_reg_t comb_pd_site2_unit0_vt2_conf2;
    volatile pvt_comb_pd_site2_unit1_vt2_conf2_reg_t comb_pd_site2_unit1_vt2_conf2;
    volatile pvt_comb_pd_site2_unit2_vt2_conf2_reg_t comb_pd_site2_unit2_vt2_conf2;
    volatile pvt_comb_pd_site2_unit3_vt2_conf2_reg_t comb_pd_site2_unit3_vt2_conf2;
    volatile pvt_comb_pd_site3_unit0_vt0_conf2_reg_t comb_pd_site3_unit0_vt0_conf2;
    volatile pvt_comb_pd_site3_unit1_vt0_conf2_reg_t comb_pd_site3_unit1_vt0_conf2;
    volatile pvt_comb_pd_site3_unit2_vt0_conf2_reg_t comb_pd_site3_unit2_vt0_conf2;
    volatile pvt_comb_pd_site3_unit3_vt0_conf2_reg_t comb_pd_site3_unit3_vt0_conf2;
    volatile pvt_comb_pd_site3_unit0_vt1_conf2_reg_t comb_pd_site3_unit0_vt1_conf2;
    volatile pvt_comb_pd_site3_unit1_vt1_conf2_reg_t comb_pd_site3_unit1_vt1_conf2;
    volatile pvt_comb_pd_site3_unit2_vt1_conf2_reg_t comb_pd_site3_unit2_vt1_conf2;
    volatile pvt_comb_pd_site3_unit3_vt1_conf2_reg_t comb_pd_site3_unit3_vt1_conf2;
    volatile pvt_comb_pd_site3_unit0_vt2_conf2_reg_t comb_pd_site3_unit0_vt2_conf2;
    volatile pvt_comb_pd_site3_unit1_vt2_conf2_reg_t comb_pd_site3_unit1_vt2_conf2;
    volatile pvt_comb_pd_site3_unit2_vt2_conf2_reg_t comb_pd_site3_unit2_vt2_conf2;
    volatile pvt_comb_pd_site3_unit3_vt2_conf2_reg_t comb_pd_site3_unit3_vt2_conf2;
    volatile pvt_value_update_reg_t value_update;
    volatile pvt_bypass_chain_reg_t bypass_chain;
    uint32_t reserved_1f0[899];
    volatile pvt_date_reg_t date;
} pvt_dev_t;

extern pvt_dev_t PVT_MONITOR;

#ifndef __cplusplus
_Static_assert(sizeof(pvt_dev_t) == 0x1000, "Invalid size of pvt_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
