/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Version Register */
/** Type of ver_date register
 *  version control register
 */
typedef union {
    struct {
        /** ver_data : R/W; bitpos: [31:0]; default: 539035144;
         *  csv version
         */
        uint32_t ver_data:32;
    };
    uint32_t val;
} isp_ver_date_reg_t;


/** Group: Configuration Registers */
/** Type of clk_en register
 *  isp clk control register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  this bit configures the clk force on of isp reg. 0: disable, 1: enable
         */
        uint32_t clk_en:1;
        /** clk_blc_force_on : R/W; bitpos: [1]; default: 0;
         *  this bit configures the clk force on of blc. 0: disable, 1: enable
         */
        uint32_t clk_blc_force_on:1;
        /** clk_dpc_force_on : R/W; bitpos: [2]; default: 0;
         *  this bit configures the clk force on of dpc. 0: disable, 1: enable
         */
        uint32_t clk_dpc_force_on:1;
        /** clk_bf_force_on : R/W; bitpos: [3]; default: 0;
         *  this bit configures the clk force on of bf. 0: disable, 1: enable
         */
        uint32_t clk_bf_force_on:1;
        /** clk_lsc_force_on : R/W; bitpos: [4]; default: 0;
         *  this bit configures the clk force on of lsc. 0: disable, 1: enable
         */
        uint32_t clk_lsc_force_on:1;
        /** clk_demosaic_force_on : R/W; bitpos: [5]; default: 0;
         *  this bit configures the clk force on of demosaic. 0: disable, 1: enable
         */
        uint32_t clk_demosaic_force_on:1;
        /** clk_median_force_on : R/W; bitpos: [6]; default: 0;
         *  this bit configures the clk force on of median. 0: disable, 1: enable
         */
        uint32_t clk_median_force_on:1;
        /** clk_ccm_force_on : R/W; bitpos: [7]; default: 0;
         *  this bit configures the clk force on of ccm. 0: disable, 1: enable
         */
        uint32_t clk_ccm_force_on:1;
        /** clk_gamma_force_on : R/W; bitpos: [8]; default: 0;
         *  this bit configures the clk force on of gamma. 0: disable, 1: enable
         */
        uint32_t clk_gamma_force_on:1;
        /** clk_rgb2yuv_force_on : R/W; bitpos: [9]; default: 0;
         *  this bit configures the clk force on of rgb2yuv. 0: disable, 1: enable
         */
        uint32_t clk_rgb2yuv_force_on:1;
        /** clk_sharp_force_on : R/W; bitpos: [10]; default: 0;
         *  this bit configures the clk force on of sharp. 0: disable, 1: enable
         */
        uint32_t clk_sharp_force_on:1;
        /** clk_color_force_on : R/W; bitpos: [11]; default: 0;
         *  this bit configures the clk force on of color. 0: disable, 1: enable
         */
        uint32_t clk_color_force_on:1;
        /** clk_yuv2rgb_force_on : R/W; bitpos: [12]; default: 0;
         *  this bit configures the clk force on of yuv2rgb. 0: disable, 1: enable
         */
        uint32_t clk_yuv2rgb_force_on:1;
        /** clk_ae_force_on : R/W; bitpos: [13]; default: 0;
         *  this bit configures the clk force on of ae. 0: disable, 1: enable
         */
        uint32_t clk_ae_force_on:1;
        /** clk_af_force_on : R/W; bitpos: [14]; default: 0;
         *  this bit configures the clk force on of af. 0: disable, 1: enable
         */
        uint32_t clk_af_force_on:1;
        /** clk_awb_force_on : R/W; bitpos: [15]; default: 0;
         *  this bit configures the clk force on of awb. 0: disable, 1: enable
         */
        uint32_t clk_awb_force_on:1;
        /** clk_hist_force_on : R/W; bitpos: [16]; default: 0;
         *  this bit configures the clk force on of hist. 0: disable, 1: enable
         */
        uint32_t clk_hist_force_on:1;
        /** clk_mipi_idi_force_on : R/W; bitpos: [17]; default: 0;
         *  this bit configures the clk force on of mipi idi input. 0: disable, 1: enable
         */
        uint32_t clk_mipi_idi_force_on:1;
        /** isp_mem_clk_force_on : R/W; bitpos: [18]; default: 0;
         *  this bit configures the clk force on of all isp memory. 0: disable, 1: enable
         */
        uint32_t isp_mem_clk_force_on:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} isp_clk_en_reg_t;

/** Type of cntl register
 *  isp module enable control register
 */
typedef union {
    struct {
        /** mipi_data_en : R/W; bitpos: [0]; default: 0;
         *  this bit configures mipi input data enable. 0: disable, 1: enable
         */
        uint32_t mipi_data_en:1;
        /** isp_en : R/W; bitpos: [1]; default: 1;
         *  this bit configures isp global enable. 0: disable, 1: enable
         */
        uint32_t isp_en:1;
        /** blc_en : R/W; bitpos: [2]; default: 0;
         *  this bit configures blc enable. 0: disable, 1: enable
         */
        uint32_t blc_en:1;
        /** dpc_en : R/W; bitpos: [3]; default: 0;
         *  this bit configures dpc enable. 0: disable, 1: enable
         */
        uint32_t dpc_en:1;
        /** bf_en : R/W; bitpos: [4]; default: 0;
         *  this bit configures bf enable. 0: disable, 1: enable
         */
        uint32_t bf_en:1;
        /** lsc_en : R/W; bitpos: [5]; default: 0;
         *  this bit configures lsc enable. 0: disable, 1: enable
         */
        uint32_t lsc_en:1;
        /** demosaic_en : R/W; bitpos: [6]; default: 1;
         *  this bit configures demosaic enable. 0: disable, 1: enable
         */
        uint32_t demosaic_en:1;
        /** median_en : R/W; bitpos: [7]; default: 0;
         *  this bit configures median enable. 0: disable, 1: enable
         */
        uint32_t median_en:1;
        /** ccm_en : R/W; bitpos: [8]; default: 0;
         *  this bit configures ccm enable. 0: disable, 1: enable
         */
        uint32_t ccm_en:1;
        /** gamma_en : R/W; bitpos: [9]; default: 0;
         *  this bit configures gamma enable. 0: disable, 1: enable
         */
        uint32_t gamma_en:1;
        /** rgb2yuv_en : R/W; bitpos: [10]; default: 1;
         *  this bit configures rgb2yuv enable. 0: disable, 1: enable
         */
        uint32_t rgb2yuv_en:1;
        /** sharp_en : R/W; bitpos: [11]; default: 0;
         *  this bit configures sharp enable. 0: disable, 1: enable
         */
        uint32_t sharp_en:1;
        /** color_en : R/W; bitpos: [12]; default: 0;
         *  this bit configures color enable. 0: disable, 1: enable
         */
        uint32_t color_en:1;
        /** yuv2rgb_en : R/W; bitpos: [13]; default: 1;
         *  this bit configures yuv2rgb enable. 0: disable, 1: enable
         */
        uint32_t yuv2rgb_en:1;
        /** ae_en : R/W; bitpos: [14]; default: 0;
         *  this bit configures ae enable. 0: disable, 1: enable
         */
        uint32_t ae_en:1;
        /** af_en : R/W; bitpos: [15]; default: 0;
         *  this bit configures af enable. 0: disable, 1: enable
         */
        uint32_t af_en:1;
        /** awb_en : R/W; bitpos: [16]; default: 0;
         *  this bit configures awb enable. 0: disable, 1: enable
         */
        uint32_t awb_en:1;
        /** hist_en : R/W; bitpos: [17]; default: 0;
         *  this bit configures hist enable. 0: disable, 1: enable
         */
        uint32_t hist_en:1;
        uint32_t reserved_18:6;
        /** byte_endian_order : R/W; bitpos: [24]; default: 0;
         *  select input idi data byte_endian_order when isp is bypass, 0: csi_data[31:0], 1:
         *  {[7:0], [15:8], [23:16], [31:24]}
         */
        uint32_t byte_endian_order:1;
        /** isp_data_type : R/W; bitpos: [26:25]; default: 0;
         *  this field configures input data type, 0:RAW8 1:RAW10 2:RAW12
         */
        uint32_t isp_data_type:2;
        /** isp_in_src : R/W; bitpos: [28:27]; default: 0;
         *  this field configures input data source, 0:CSI HOST 1:CAM 2:DMA
         */
        uint32_t isp_in_src:2;
        /** isp_out_type : R/W; bitpos: [31:29]; default: 2;
         *  this field configures pixel output type, 0: RAW8 1: YUV422 2: RGB888 3: YUV420 4:
         *  RGB565
         */
        uint32_t isp_out_type:3;
    };
    uint32_t val;
} isp_cntl_reg_t;

/** Type of hsync_cnt register
 *  header hsync interval control register
 */
typedef union {
    struct {
        /** hsync_cnt : R/W; bitpos: [7:0]; default: 7;
         *  this field configures the number of clock before hsync and after vsync and line_end
         *  when decodes pix data from idi to isp
         */
        uint32_t hsync_cnt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} isp_hsync_cnt_reg_t;

/** Type of frame_cfg register
 *  frame control parameter register
 */
typedef union {
    struct {
        /** vadr_num : R/W; bitpos: [11:0]; default: 480;
         *  this field configures input image size in y-direction, image row number - 1
         */
        uint32_t vadr_num:12;
        /** hadr_num : R/W; bitpos: [23:12]; default: 480;
         *  this field configures input image size in x-direction, image line number - 1
         */
        uint32_t hadr_num:12;
        uint32_t reserved_24:3;
        /** bayer_mode : R/W; bitpos: [28:27]; default: 0;
         *  this field configures the bayer mode of input pixel. 00 : BG/GR    01 : GB/RG   10
         *  : GR/BG  11 : RG/GB
         */
        uint32_t bayer_mode:2;
        /** hsync_start_exist : R/W; bitpos: [29]; default: 1;
         *  this bit configures the line end start exist or not. 0: not exist, 1: exist
         */
        uint32_t hsync_start_exist:1;
        /** hsync_end_exist : R/W; bitpos: [30]; default: 1;
         *  this bit configures the line end packet exist or not. 0: not exist, 1: exist
         */
        uint32_t hsync_end_exist:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} isp_frame_cfg_reg_t;

/** Type of ccm_coef0 register
 *  ccm coef register 0
 */
typedef union {
    struct {
        /** ccm_rr : R/W; bitpos: [12:0]; default: 1856;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_rr:13;
        /** ccm_rg : R/W; bitpos: [25:13]; default: 4736;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_rg:13;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} isp_ccm_coef0_reg_t;

/** Type of ccm_coef1 register
 *  ccm coef register 1
 */
typedef union {
    struct {
        /** ccm_rb : R/W; bitpos: [12:0]; default: 4288;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_rb:13;
        /** ccm_gr : R/W; bitpos: [25:13]; default: 4416;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_gr:13;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} isp_ccm_coef1_reg_t;

/** Type of ccm_coef3 register
 *  ccm coef register 3
 */
typedef union {
    struct {
        /** ccm_gg : R/W; bitpos: [12:0]; default: 1664;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_gg:13;
        /** ccm_gb : R/W; bitpos: [25:13]; default: 4352;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_gb:13;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} isp_ccm_coef3_reg_t;

/** Type of ccm_coef4 register
 *  ccm coef register 4
 */
typedef union {
    struct {
        /** ccm_br : R/W; bitpos: [12:0]; default: 4160;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_br:13;
        /** ccm_bg : R/W; bitpos: [25:13]; default: 4800;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_bg:13;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} isp_ccm_coef4_reg_t;

/** Type of ccm_coef5 register
 *  ccm coef register 5
 */
typedef union {
    struct {
        /** ccm_bb : R/W; bitpos: [12:0]; default: 1856;
         *  this field configures the color correction matrix coefficient
         */
        uint32_t ccm_bb:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} isp_ccm_coef5_reg_t;

/** Type of bf_matrix_ctrl register
 *  bf pix2matrix ctrl
 */
typedef union {
    struct {
        /** bf_tail_pixen_pulse_tl : R/W; bitpos: [7:0]; default: 0;
         *  matrix tail pixen low level threshold, should not to large to prevent expanding to
         *  next frame, only reg_bf_tail_pixen_pulse_th!=0 and reg_bf_tail_pixen_pulse_tl!=0
         *  and reg_bf_tail_pixen_pulse_th < reg_bf_tail_pixen_pulse_tl will enable tail pulse
         *  function
         */
        uint32_t bf_tail_pixen_pulse_tl:8;
        /** bf_tail_pixen_pulse_th : R/W; bitpos: [15:8]; default: 0;
         *  matrix tail pixen high level threshold, must < hnum-1, only
         *  reg_bf_tail_pixen_pulse_th!=0 and reg_bf_tail_pixen_pulse_tl!=0 and
         *  reg_bf_tail_pixen_pulse_th < reg_bf_tail_pixen_pulse_tl will enable tail pulse
         *  function
         */
        uint32_t bf_tail_pixen_pulse_th:8;
        /** bf_padding_data : R/W; bitpos: [23:16]; default: 0;
         *  this field configures bf matrix padding data
         */
        uint32_t bf_padding_data:8;
        /** bf_padding_mode : R/W; bitpos: [24]; default: 0;
         *  this bit configures the padding mode of bf matrix.  0: use pixel in image to do
         *  padding   1: use reg_padding_data to do padding
         */
        uint32_t bf_padding_mode:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} isp_bf_matrix_ctrl_reg_t;

/** Type of bf_sigma register
 *  bf denoising level control register
 */
typedef union {
    struct {
        /** sigma : R/W; bitpos: [5:0]; default: 2;
         *  this field configures the bayer denoising level, valid data from 2 to 20
         */
        uint32_t sigma:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} isp_bf_sigma_reg_t;

/** Type of bf_gau0 register
 *  bf gau template register 0
 */
typedef union {
    struct {
        /** gau_template21 : R/W; bitpos: [3:0]; default: 15;
         *  this field configures index 21 of gaussian template
         */
        uint32_t gau_template21:4;
        /** gau_template20 : R/W; bitpos: [7:4]; default: 15;
         *  this field configures index 20 of gaussian template
         */
        uint32_t gau_template20:4;
        /** gau_template12 : R/W; bitpos: [11:8]; default: 15;
         *  this field configures index 12 of gaussian template
         */
        uint32_t gau_template12:4;
        /** gau_template11 : R/W; bitpos: [15:12]; default: 15;
         *  this field configures index 11 of gaussian template
         */
        uint32_t gau_template11:4;
        /** gau_template10 : R/W; bitpos: [19:16]; default: 15;
         *  this field configures index 10 of gaussian template
         */
        uint32_t gau_template10:4;
        /** gau_template02 : R/W; bitpos: [23:20]; default: 15;
         *  this field configures index 02 of gaussian template
         */
        uint32_t gau_template02:4;
        /** gau_template01 : R/W; bitpos: [27:24]; default: 15;
         *  this field configures index 01 of gaussian template
         */
        uint32_t gau_template01:4;
        /** gau_template00 : R/W; bitpos: [31:28]; default: 15;
         *  this field configures index 00 of gaussian template
         */
        uint32_t gau_template00:4;
    };
    uint32_t val;
} isp_bf_gau0_reg_t;

/** Type of bf_gau1 register
 *  bf gau template register 1
 */
typedef union {
    struct {
        /** gau_template22 : R/W; bitpos: [3:0]; default: 15;
         *  this field configures index 22 of gaussian template
         */
        uint32_t gau_template22:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} isp_bf_gau1_reg_t;

/** Type of dpc_ctrl register
 *  DPC mode control register
 */
typedef union {
    struct {
        /** dpc_check_en : R/W; bitpos: [0]; default: 0;
         *  this bit configures the check mode enable. 0: disable, 1: enable
         */
        uint32_t dpc_check_en:1;
        /** sta_en : R/W; bitpos: [1]; default: 0;
         *  this bit configures the sta dpc enable. 0: disable, 1: enable
         */
        uint32_t sta_en:1;
        /** dyn_en : R/W; bitpos: [2]; default: 1;
         *  this bit configures the dyn dpc enable. 0: disable, 1: enable
         */
        uint32_t dyn_en:1;
        /** dpc_black_en : R/W; bitpos: [3]; default: 0;
         *  this bit configures input image type select when in check mode, 0: white img, 1:
         *  black img
         */
        uint32_t dpc_black_en:1;
        /** dpc_method_sel : R/W; bitpos: [4]; default: 0;
         *  this bit configures dyn dpc method select. 0: simple method, 1: hard method
         */
        uint32_t dpc_method_sel:1;
        /** dpc_check_od_en : R/W; bitpos: [5]; default: 0;
         *  this bit configures output pixel data when in check mode or not. 0: no data output,
         *  1: data output
         */
        uint32_t dpc_check_od_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} isp_dpc_ctrl_reg_t;

/** Type of dpc_conf register
 *  DPC parameter config register
 */
typedef union {
    struct {
        /** dpc_threshold_l : R/W; bitpos: [7:0]; default: 48;
         *  this bit configures the threshold to detect black img in check mode, or the low
         *  threshold(use 8 bit 0~255) in dyn method 0, or the low threshold factor (use 5 bit
         *  10000-> 16/16, 00001->1/16, 0/16~16/16) in dyn method 1
         */
        uint32_t dpc_threshold_l:8;
        /** dpc_threshold_h : R/W; bitpos: [15:8]; default: 48;
         *  this bit configures the threshold to detect white img in check mode, or the high
         *  threshold(use 8 bit 0~255) in dyn method 0, or the high threshold factor (use 5 bit
         *  10000-> 16/16, 00001->1/16, 0/16~16/16) in dyn method 1
         */
        uint32_t dpc_threshold_h:8;
        /** dpc_factor_dark : R/W; bitpos: [21:16]; default: 16;
         *  this field configures the dynamic correction method 1 dark   factor
         */
        uint32_t dpc_factor_dark:6;
        /** dpc_factor_brig : R/W; bitpos: [27:22]; default: 16;
         *  this field configures the dynamic correction method 1 bright factor
         */
        uint32_t dpc_factor_brig:6;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_dpc_conf_reg_t;

/** Type of dpc_matrix_ctrl register
 *  dpc pix2matrix ctrl
 */
typedef union {
    struct {
        /** dpc_tail_pixen_pulse_tl : R/W; bitpos: [7:0]; default: 0;
         *  matrix tail pixen low level threshold,  should not to large to prevent expanding to
         *  next frame, only reg_dpc_tail_pixen_pulse_th!=0 and reg_dpc_tail_pixen_pulse_tl!=0
         *  and reg_dpc_tail_pixen_pulse_th < reg_dpc_tail_pixen_pulse_tl will enable tail
         *  pulse function
         */
        uint32_t dpc_tail_pixen_pulse_tl:8;
        /** dpc_tail_pixen_pulse_th : R/W; bitpos: [15:8]; default: 0;
         *  matrix tail pixen high level threshold, must < hnum-1, only
         *  reg_dpc_tail_pixen_pulse_th!=0 and reg_dpc_tail_pixen_pulse_tl!=0 and
         *  reg_dpc_tail_pixen_pulse_th < reg_dpc_tail_pixen_pulse_tl will enable tail pulse
         *  function
         */
        uint32_t dpc_tail_pixen_pulse_th:8;
        /** dpc_padding_data : R/W; bitpos: [23:16]; default: 0;
         *  this field configures dpc matrix padding data
         */
        uint32_t dpc_padding_data:8;
        /** dpc_padding_mode : R/W; bitpos: [24]; default: 0;
         *  this bit configures the padding mode of dpc matrix.  0: use pixel in image to do
         *  padding   1: use reg_padding_data to do padding
         */
        uint32_t dpc_padding_mode:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} isp_dpc_matrix_ctrl_reg_t;

/** Type of lut_cmd register
 *  LUT command register
 */
typedef union {
    struct {
        /** lut_addr : WT; bitpos: [11:0]; default: 0;
         *  this field configures the lut access addr, when select lsc lut, [11:10]:00 sel gb_b
         *  lut, 01 sel r_gr lut
         */
        uint32_t lut_addr:12;
        /** lut_num : WT; bitpos: [15:12]; default: 0;
         *  this field configures the lut selection. 0000:LSC LUT 0001:DPC LUT
         */
        uint32_t lut_num:4;
        /** lut_cmd : WT; bitpos: [16]; default: 0;
         *  this bit configures the access event of lut. 0:rd 1: wr
         */
        uint32_t lut_cmd:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} isp_lut_cmd_reg_t;

/** Type of lut_wdata register
 *  LUT write data register
 */
typedef union {
    struct {
        /** lut_wdata : R/W; bitpos: [31:0]; default: 0;
         *  this field configures the write data of lut. please initial ISP_LUT_WDATA before
         *  write ISP_LUT_CMD register
         */
        uint32_t lut_wdata:32;
    };
    uint32_t val;
} isp_lut_wdata_reg_t;

/** Type of lsc_tablesize register
 *  LSC point in x-direction
 */
typedef union {
    struct {
        /** lsc_xtablesize : R/W; bitpos: [4:0]; default: 31;
         *  this field configures lsc table size in x-direction
         */
        uint32_t lsc_xtablesize:5;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} isp_lsc_tablesize_reg_t;

/** Type of demosaic_matrix_ctrl register
 *  demosaic pix2matrix ctrl
 */
typedef union {
    struct {
        /** demosaic_tail_pixen_pulse_tl : R/W; bitpos: [7:0]; default: 0;
         *  matrix tail pixen low level threshold,  should not to large to prevent expanding to
         *  next frame, only reg_demosaic_tail_pixen_pulse_th!=0 and
         *  reg_demosaic_tail_pixen_pulse_tl!=0 and reg_demosaic_tail_pixen_pulse_th <
         *  reg_demosaic_tail_pixen_pulse_tl will enable tail pulse function
         */
        uint32_t demosaic_tail_pixen_pulse_tl:8;
        /** demosaic_tail_pixen_pulse_th : R/W; bitpos: [15:8]; default: 0;
         *  matrix tail pixen high level threshold, must < hnum-1, only
         *  reg_demosaic_tail_pixen_pulse_th!=0 and reg_demosaic_tail_pixen_pulse_tl!=0 and
         *  reg_demosaic_tail_pixen_pulse_th < reg_demosaic_tail_pixen_pulse_tl will enable
         *  tail pulse function
         */
        uint32_t demosaic_tail_pixen_pulse_th:8;
        /** demosaic_padding_data : R/W; bitpos: [23:16]; default: 0;
         *  this field configures demosaic matrix padding data
         */
        uint32_t demosaic_padding_data:8;
        /** demosaic_padding_mode : R/W; bitpos: [24]; default: 0;
         *  this bit configures the padding mode of demosaic matrix.  0: use pixel in image to
         *  do padding   1: use reg_padding_data to do padding
         */
        uint32_t demosaic_padding_mode:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} isp_demosaic_matrix_ctrl_reg_t;

/** Type of demosaic_grad_ratio register
 *  demosaic gradient select ratio
 */
typedef union {
    struct {
        /** demosaic_grad_ratio : R/W; bitpos: [5:0]; default: 16;
         *  this field configures demosaic gradient select ratio
         */
        uint32_t demosaic_grad_ratio:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} isp_demosaic_grad_ratio_reg_t;

/** Type of median_matrix_ctrl register
 *  median pix2matrix ctrl
 */
typedef union {
    struct {
        /** median_padding_data : R/W; bitpos: [7:0]; default: 0;
         *  this field configures median matrix padding data
         */
        uint32_t median_padding_data:8;
        /** median_padding_mode : R/W; bitpos: [8]; default: 0;
         *  this bit configures the padding mode of median matrix.  0: use pixel in image to do
         *  padding   1: use reg_padding_data to do padding
         */
        uint32_t median_padding_mode:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} isp_median_matrix_ctrl_reg_t;

/** Type of gamma_ctrl register
 *  gamma control register
 */
typedef union {
    struct {
        /** gamma_update : R/W; bitpos: [0]; default: 0;
         *  Indicates that gamma register configuration is complete
         */
        uint32_t gamma_update:1;
        /** gamma_b_last_correct : R/W; bitpos: [1]; default: 1;
         *  this bit configures enable of last b segment correcction. 0: disable, 1: enable
         */
        uint32_t gamma_b_last_correct:1;
        /** gamma_g_last_correct : R/W; bitpos: [2]; default: 1;
         *  this bit configures enable of last g segment correcction. 0: disable, 1: enable
         */
        uint32_t gamma_g_last_correct:1;
        /** gamma_r_last_correct : R/W; bitpos: [3]; default: 1;
         *  this bit configures enable of last r segment correcction. 0: disable, 1: enable
         */
        uint32_t gamma_r_last_correct:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} isp_gamma_ctrl_reg_t;

/** Type of gamma_y1 register
 *  point of Y-axis of r/g/b channel gamma curve register 1
 */
typedef union {
    struct {
        /** gamma_y03 : R/W; bitpos: [7:0]; default: 64;
         *  this field configures the point 3 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y03:8;
        /** gamma_y02 : R/W; bitpos: [15:8]; default: 48;
         *  this field configures the point 2 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y02:8;
        /** gamma_y01 : R/W; bitpos: [23:16]; default: 32;
         *  this field configures the point 1 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y01:8;
        /** gamma_y00 : R/W; bitpos: [31:24]; default: 16;
         *  this field configures the point 0 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y00:8;
    };
    uint32_t val;
} isp_gamma_y1_reg_t;

/** Type of gamma_y2 register
 *  point of Y-axis of r/g/b channel gamma curve register 2
 */
typedef union {
    struct {
        /** gamma_y07 : R/W; bitpos: [7:0]; default: 128;
         *  this field configures the point 7 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y07:8;
        /** gamma_y06 : R/W; bitpos: [15:8]; default: 112;
         *  this field configures the point 6 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y06:8;
        /** gamma_y05 : R/W; bitpos: [23:16]; default: 96;
         *  this field configures the point 5 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y05:8;
        /** gamma_y04 : R/W; bitpos: [31:24]; default: 80;
         *  this field configures the point 4 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y04:8;
    };
    uint32_t val;
} isp_gamma_y2_reg_t;

/** Type of gamma_y3 register
 *  point of Y-axis of r/g/b channel gamma curve register 3
 */
typedef union {
    struct {
        /** gamma_y0b : R/W; bitpos: [7:0]; default: 192;
         *  this field configures the point 11 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y0b:8;
        /** gamma_y0a : R/W; bitpos: [15:8]; default: 176;
         *  this field configures the point 10 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y0a:8;
        /** gamma_y09 : R/W; bitpos: [23:16]; default: 160;
         *  this field configures the point 9 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y09:8;
        /** gamma_y08 : R/W; bitpos: [31:24]; default: 144;
         *  this field configures the point 8 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y08:8;
    };
    uint32_t val;
} isp_gamma_y3_reg_t;

/** Type of gamma_y4 register
 *  point of Y-axis of r/g/b channel gamma curve register 4
 */
typedef union {
    struct {
        /** gamma_y0f : R/W; bitpos: [7:0]; default: 255;
         *  this field configures the point 15 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y0f:8;
        /** gamma_y0e : R/W; bitpos: [15:8]; default: 240;
         *  this field configures the point 14 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y0e:8;
        /** gamma_y0d : R/W; bitpos: [23:16]; default: 224;
         *  this field configures the point 13 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y0d:8;
        /** gamma_y0c : R/W; bitpos: [31:24]; default: 208;
         *  this field configures the point 12 of Y-axis of r/g/b channel gamma curve
         */
        uint32_t gamma_y0c:8;
    };
    uint32_t val;
} isp_gamma_y4_reg_t;

/** Type of gamma_x1 register
 *  point of X-axis of r/g/b channel gamma curve register 1
 */
typedef union {
    struct {
        /** gamma_r_x07 : R/W; bitpos: [2:0]; default: 4;
         *  this field configures the point 7 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x07:3;
        /** gamma_r_x06 : R/W; bitpos: [5:3]; default: 4;
         *  this field configures the point 6 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x06:3;
        /** gamma_x05 : R/W; bitpos: [8:6]; default: 4;
         *  this field configures the point 5 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x05:3;
        /** gamma_x04 : R/W; bitpos: [11:9]; default: 4;
         *  this field configures the point 4 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x04:3;
        /** gamma_x03 : R/W; bitpos: [14:12]; default: 4;
         *  this field configures the point 3 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x03:3;
        /** gamma_x02 : R/W; bitpos: [17:15]; default: 4;
         *  this field configures the point 2 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x02:3;
        /** gamma_x01 : R/W; bitpos: [20:18]; default: 4;
         *  this field configures the point 1 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x01:3;
        /** gamma_x00 : R/W; bitpos: [23:21]; default: 4;
         *  this field configures the point 0 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x00:3;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} isp_gamma_x1_reg_t;

/** Type of gamma_x2 register
 *  point of X-axis of r/g/b channel gamma curve register 2
 */
typedef union {
    struct {
        /** gamma_x0f : R/W; bitpos: [2:0]; default: 4;
         *  this field configures the point 15 of X-axis of r/g/b channel gamma curve, it
         *  represents the power of the distance from the previous point
         */
        uint32_t gamma_x0f:3;
        /** gamma_x0e : R/W; bitpos: [5:3]; default: 4;
         *  this field configures the point 14 of X-axis of r/g/b channel gamma curve, it
         *  represents the power of the distance from the previous point
         */
        uint32_t gamma_x0e:3;
        /** gamma_x0d : R/W; bitpos: [8:6]; default: 4;
         *  this field configures the point 13 of X-axis of r/g/b channel gamma curve, it
         *  represents the power of the distance from the previous point
         */
        uint32_t gamma_x0d:3;
        /** gamma_x0c : R/W; bitpos: [11:9]; default: 4;
         *  this field configures the point 12 of X-axis of r/g/b channel gamma curve, it
         *  represents the power of the distance from the previous point
         */
        uint32_t gamma_x0c:3;
        /** gamma_x0b : R/W; bitpos: [14:12]; default: 4;
         *  this field configures the point 11 of X-axis of r/g/b channel gamma curve, it
         *  represents the power of the distance from the previous point
         */
        uint32_t gamma_x0b:3;
        /** gamma_x0a : R/W; bitpos: [17:15]; default: 4;
         *  this field configures the point 10 of X-axis of r/g/b channel gamma curve, it
         *  represents the power of the distance from the previous point
         */
        uint32_t gamma_x0a:3;
        /** gamma_x09 : R/W; bitpos: [20:18]; default: 4;
         *  this field configures the point 9 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x09:3;
        /** gamma_x08 : R/W; bitpos: [23:21]; default: 4;
         *  this field configures the point 8 of X-axis of r/g/b channel gamma curve, it represents
         *  the power of the distance from the previous point
         */
        uint32_t gamma_x08:3;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} isp_gamma_x2_reg_t;

/** Type of ae_ctrl register
 *  ae control register
 */
typedef union {
    struct {
        /** ae_update : WT; bitpos: [0]; default: 0;
         *  write 1 to this bit triggers one statistic event
         */
        uint32_t ae_update:1;
        /** ae_select : R/W; bitpos: [1]; default: 0;
         *  this field configures ae input data source, 0: data from median, 1: data from gama
         */
        uint32_t ae_select:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} isp_ae_ctrl_reg_t;

/** Type of ae_monitor register
 *  ae monitor control register
 */
typedef union {
    struct {
        /** ae_monitor_tl : R/W; bitpos: [7:0]; default: 0;
         *  this field configures the lower lum threshold of ae monitor
         */
        uint32_t ae_monitor_tl:8;
        /** ae_monitor_th : R/W; bitpos: [15:8]; default: 0;
         *  this field configures the higher lum threshold of ae monitor
         */
        uint32_t ae_monitor_th:8;
        /** ae_monitor_period : R/W; bitpos: [21:16]; default: 0;
         *  this field configures ae monitor frame period
         */
        uint32_t ae_monitor_period:6;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} isp_ae_monitor_reg_t;

/** Type of ae_bx register
 *  ae window register in x-direction
 */
typedef union {
    struct {
        /** ae_x_bsize : R/W; bitpos: [10:0]; default: 384;
         *  this field configures every block x size
         */
        uint32_t ae_x_bsize:11;
        /** ae_x_start : R/W; bitpos: [21:11]; default: 0;
         *  this field configures first block start x address
         */
        uint32_t ae_x_start:11;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} isp_ae_bx_reg_t;

/** Type of ae_by register
 *  ae window register in y-direction
 */
typedef union {
    struct {
        /** ae_y_bsize : R/W; bitpos: [10:0]; default: 216;
         *  this field configures every block y size
         */
        uint32_t ae_y_bsize:11;
        /** ae_y_start : R/W; bitpos: [21:11]; default: 0;
         *  this field configures first block start y address
         */
        uint32_t ae_y_start:11;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} isp_ae_by_reg_t;

/** Type of ae_winpixnum register
 *  ae sub-window pix num register
 */
typedef union {
    struct {
        /** ae_subwin_pixnum : R/W; bitpos: [16:0]; default: 82944;
         *  this field configures the pixel number of each sub win
         */
        uint32_t ae_subwin_pixnum:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} isp_ae_winpixnum_reg_t;

/** Type of ae_win_reciprocal register
 *  reciprocal of ae sub-window pixel number
 */
typedef union {
    struct {
        /** ae_subwin_recip : R/W; bitpos: [19:0]; default: 0;
         *  this field configures the reciprocal of each subwin_pixnum, 20bit fraction
         */
        uint32_t ae_subwin_recip:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} isp_ae_win_reciprocal_reg_t;

/** Type of sharp_ctrl0 register
 *  sharp control register 0
 */
typedef union {
    struct {
        /** sharp_threshold_low : R/W; bitpos: [7:0]; default: 0;
         *  this field configures sharpen threshold for detail
         */
        uint32_t sharp_threshold_low:8;
        /** sharp_threshold_high : R/W; bitpos: [15:8]; default: 0;
         *  this field configures sharpen threshold for edge
         */
        uint32_t sharp_threshold_high:8;
        /** sharp_amount_low : R/W; bitpos: [23:16]; default: 0;
         *  this field configures sharpen amount for detail
         */
        uint32_t sharp_amount_low:8;
        /** sharp_amount_high : R/W; bitpos: [31:24]; default: 0;
         *  this field configures sharpen amount for edge
         */
        uint32_t sharp_amount_high:8;
    };
    uint32_t val;
} isp_sharp_ctrl0_reg_t;

/** Type of sharp_filter0 register
 *  sharp usm config register 0
 */
typedef union {
    struct {
        /** sharp_filter_coe00 : R/W; bitpos: [4:0]; default: 1;
         *  this field configures unsharp masking(usm) filter coefficient
         */
        uint32_t sharp_filter_coe00:5;
        /** sharp_filter_coe01 : R/W; bitpos: [9:5]; default: 2;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe01:5;
        /** sharp_filter_coe02 : R/W; bitpos: [14:10]; default: 1;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe02:5;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} isp_sharp_filter0_reg_t;

/** Type of sharp_filter1 register
 *  sharp usm config register 1
 */
typedef union {
    struct {
        /** sharp_filter_coe10 : R/W; bitpos: [4:0]; default: 2;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe10:5;
        /** sharp_filter_coe11 : R/W; bitpos: [9:5]; default: 4;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe11:5;
        /** sharp_filter_coe12 : R/W; bitpos: [14:10]; default: 2;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe12:5;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} isp_sharp_filter1_reg_t;

/** Type of sharp_filter2 register
 *  sharp usm config register 2
 */
typedef union {
    struct {
        /** sharp_filter_coe20 : R/W; bitpos: [4:0]; default: 1;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe20:5;
        /** sharp_filter_coe21 : R/W; bitpos: [9:5]; default: 2;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe21:5;
        /** sharp_filter_coe22 : R/W; bitpos: [14:10]; default: 1;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe22:5;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} isp_sharp_filter2_reg_t;

typedef union {
    struct {
        /** sharp_filter_coe0 : R/W; bitpos: [4:0]; default: 1;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe0:5;
        /** sharp_filter_coe1 : R/W; bitpos: [9:5]; default: 2;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe1:5;
        /** sharp_filter_coe2 : R/W; bitpos: [14:10]; default: 1;
         *  this field configures usm filter coefficient
         */
        uint32_t sharp_filter_coe2:5;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} isp_sharp_filter_reg_t;

/** Type of sharp_matrix_ctrl register
 *  sharp pix2matrix ctrl
 */
typedef union {
    struct {
        /** sharp_tail_pixen_pulse_tl : R/W; bitpos: [7:0]; default: 0;
         *  matrix tail pixen low level threshold,  should not to large to prevent expanding to
         *  next frame, only reg_demosaic_tail_pixen_pulse_th!=0 and
         *  reg_demosaic_tail_pixen_pulse_tl!=0 and reg_demosaic_tail_pixen_pulse_th <
         *  reg_demosaic_tail_pixen_pulse_tl will enable tail pulse function
         */
        uint32_t sharp_tail_pixen_pulse_tl:8;
        /** sharp_tail_pixen_pulse_th : R/W; bitpos: [15:8]; default: 0;
         *  matrix tail pixen high level threshold, must < hnum-1, only
         *  reg_sharp_tail_pixen_pulse_th!=0 and reg_sharp_tail_pixen_pulse_tl!=0 and
         *  reg_sharp_tail_pixen_pulse_th < reg_sharp_tail_pixen_pulse_tl will enable tail
         *  pulse function
         */
        uint32_t sharp_tail_pixen_pulse_th:8;
        /** sharp_padding_data : R/W; bitpos: [23:16]; default: 0;
         *  this field configures sharp padding data
         */
        uint32_t sharp_padding_data:8;
        /** sharp_padding_mode : R/W; bitpos: [24]; default: 0;
         *  this field configures sharp padding mode
         */
        uint32_t sharp_padding_mode:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} isp_sharp_matrix_ctrl_reg_t;

/** Type of sharp_ctrl1 register
 *  sharp control register 1
 */
typedef union {
    struct {
        /** sharp_gradient_max : RO; bitpos: [7:0]; default: 0;
         *  this field configures sharp max gradient, refresh at the end of each frame end
         */
        uint32_t sharp_gradient_max:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} isp_sharp_ctrl1_reg_t;

/** Type of dma_cntl register
 *  isp dma source trans control register
 */
typedef union {
    struct {
        /** dma_en : WT; bitpos: [0]; default: 0;
         *  write 1 to trigger dma to get 1 frame
         */
        uint32_t dma_en:1;
        /** dma_update_reg : R/W; bitpos: [1]; default: 0;
         *  write 1 to update reg_dma_burst_len & reg_dma_data_type
         */
        uint32_t dma_update_reg:1;
        /** dma_data_type : R/W; bitpos: [7:2]; default: 42;
         *  this field configures the idi data type for image data
         */
        uint32_t dma_data_type:6;
        /** dma_burst_len : R/W; bitpos: [19:8]; default: 128;
         *  this field configures dma burst len when data source is dma. set according to
         *  dma_msize, it is the number of 64bits in a dma transfer
         */
        uint32_t dma_burst_len:12;
        /** dma_interval : R/W; bitpos: [31:20]; default: 1;
         *  this field configures dma req interval, 12'b1: 1 cycle, 12'b11 2 cycle ...
         */
        uint32_t dma_interval:12;
    };
    uint32_t val;
} isp_dma_cntl_reg_t;

/** Type of dma_raw_data register
 *  isp dma source total raw number set register
 */
typedef union {
    struct {
        /** dma_raw_num_total : R/W; bitpos: [21:0]; default: 0;
         *  this field configures the the number of 64bits in a frame
         */
        uint32_t dma_raw_num_total:22;
        uint32_t reserved_22:9;
        /** dma_raw_num_total_set : WT; bitpos: [31]; default: 0;
         *  write 1 to update reg_dma_raw_num_total
         */
        uint32_t dma_raw_num_total_set:1;
    };
    uint32_t val;
} isp_dma_raw_data_reg_t;

/** Type of cam_cntl register
 *  isp cam source control register
 */
typedef union {
    struct {
        /** cam_en : R/W; bitpos: [0]; default: 0;
         *  write 1 to start receive camera data, write 0 to disable
         */
        uint32_t cam_en:1;
        /** cam_update_reg : R/W; bitpos: [1]; default: 0;
         *  write 1 to update ISP_CAM_CONF
         */
        uint32_t cam_update_reg:1;
        /** cam_reset : R/W; bitpos: [2]; default: 1;
         *  this bit configures cam clk domain reset, 1: reset cam input logic, 0: release reset
         */
        uint32_t cam_reset:1;
        /** cam_clk_inv : R/W; bitpos: [3]; default: 0;
         *  this bit configures the inversion of cam clk from pad. 0: not invert cam clk, 1:
         *  invert cam clk
         */
        uint32_t cam_clk_inv:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} isp_cam_cntl_reg_t;

/** Type of cam_conf register
 *  isp cam source config register
 */
typedef union {
    struct {
        /** cam_data_order : R/W; bitpos: [0]; default: 0;
         *  this field configures data order of cam port, 0: cam_data_in, 1:{cam_data_in[7:0],
         *  cam_data_in[15:8]}
         */
        uint32_t cam_data_order:1;
        /** cam_2byte_mode : R/W; bitpos: [1]; default: 0;
         *  this field configures enable of cam 2 byte mode(input 2 bytes each clock). 0:
         *  disable, 1: enable
         */
        uint32_t cam_2byte_mode:1;
        /** cam_data_type : R/W; bitpos: [7:2]; default: 42;
         *  this field configures idi data type for image data, 0x2a: RAW8, 0x2b: RAW10, 0x2c:
         *  RAW12
         */
        uint32_t cam_data_type:6;
        /** cam_de_inv : R/W; bitpos: [8]; default: 0;
         *  this bit configures cam data enable invert. 0: not invert, 1: invert
         */
        uint32_t cam_de_inv:1;
        /** cam_hsync_inv : R/W; bitpos: [9]; default: 0;
         *  this bit configures cam hsync invert. 0: not invert, 1: invert
         */
        uint32_t cam_hsync_inv:1;
        /** cam_vsync_inv : R/W; bitpos: [10]; default: 0;
         *  this bit configures cam vsync invert. 0: not invert, 1: invert
         */
        uint32_t cam_vsync_inv:1;
        /** cam_vsync_filter_thres : R/W; bitpos: [13:11]; default: 0;
         *  this bit configures the number of clock of vsync filter length
         */
        uint32_t cam_vsync_filter_thres:3;
        /** cam_vsync_filter_en : R/W; bitpos: [14]; default: 0;
         *  this bit configures vsync filter en
         */
        uint32_t cam_vsync_filter_en:1;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} isp_cam_conf_reg_t;

/** Type of af_ctrl0 register
 *  af control register 0
 */
typedef union {
    struct {
        /** af_auto_update : R/W; bitpos: [0]; default: 0;
         *  this bit configures auto_update enable. when set to 1, will update sum and lum each
         *  frame
         */
        uint32_t af_auto_update:1;
        uint32_t reserved_1:3;
        /** af_manual_update : WT; bitpos: [4]; default: 0;
         *  write 1 to this bit will update the sum and lum once
         */
        uint32_t af_manual_update:1;
        uint32_t reserved_5:3;
        /** af_env_threshold : R/W; bitpos: [11:8]; default: 0;
         *  this field configures env threshold. when both sum and lum changes larger than this
         *  value, consider environment changes and need to trigger a new autofocus. 4Bit
         *  fractional
         */
        uint32_t af_env_threshold:4;
        uint32_t reserved_12:4;
        /** af_env_period : R/W; bitpos: [23:16]; default: 0;
         *  this field configures environment changes detection period (frame). When set to 0,
         *  disable this function
         */
        uint32_t af_env_period:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} isp_af_ctrl0_reg_t;

/** Type of af_ctrl1 register
 *  af control register 1
 */
typedef union {
    struct {
        /** af_thpixnum : R/W; bitpos: [21:0]; default: 0;
         *  this field configures pixnum used when calculating the autofocus threshold. Set to
         *  0 to disable threshold calculation
         */
        uint32_t af_thpixnum:22;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} isp_af_ctrl1_reg_t;

/** Type of af_gen_th_ctrl register
 *  af gen threshold control register
 */
typedef union {
    struct {
        /** af_gen_threshold_min : R/W; bitpos: [15:0]; default: 128;
         *  this field configures min threshold when use auto_threshold
         */
        uint32_t af_gen_threshold_min:16;
        /** af_gen_threshold_max : R/W; bitpos: [31:16]; default: 1088;
         *  this field configures max threshold when use auto_threshold
         */
        uint32_t af_gen_threshold_max:16;
    };
    uint32_t val;
} isp_af_gen_th_ctrl_reg_t;

/** Type of af_env_user_th_sum register
 *  af monitor user sum threshold register
 */
typedef union {
    struct {
        /** af_env_user_threshold_sum : R/W; bitpos: [31:0]; default: 0;
         *  this field configures user setup env detect sum threshold
         */
        uint32_t af_env_user_threshold_sum:32;
    };
    uint32_t val;
} isp_af_env_user_th_sum_reg_t;

/** Type of af_env_user_th_lum register
 *  af monitor user lum threshold register
 */
typedef union {
    struct {
        /** af_env_user_threshold_lum : R/W; bitpos: [29:0]; default: 0;
         *  this field configures user setup env detect lum threshold
         */
        uint32_t af_env_user_threshold_lum:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_af_env_user_th_lum_reg_t;

/** Type of af_threshold register
 *  af threshold register
 */
typedef union {
    struct {
        /** af_threshold : R/W; bitpos: [15:0]; default: 256;
         *  this field configures user threshold. When set to non-zero, autofocus will use this
         *  threshold
         */
        uint32_t af_threshold:16;
        /** af_gen_threshold : RO; bitpos: [31:16]; default: 0;
         *  this field represents the last calculated threshold
         */
        uint32_t af_gen_threshold:16;
    };
    uint32_t val;
} isp_af_threshold_reg_t;

/** Type of af_hscale_a register
 *  h-scale of af window a register
 */
typedef union {
    struct {
        /** af_rpoint_a : R/W; bitpos: [11:0]; default: 128;
         *  this field configures left coordinate of focus window a, must >= 2
         */
        uint32_t af_rpoint_a:12;
        uint32_t reserved_12:4;
        /** af_lpoint_a : R/W; bitpos: [27:16]; default: 1;
         *  this field configures top coordinate of focus window a, must >= 2
         */
        uint32_t af_lpoint_a:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_hscale_a_reg_t;

/** Type of af_vscale_a register
 *  v-scale of af window a register
 */
typedef union {
    struct {
        /** af_bpoint_a : R/W; bitpos: [11:0]; default: 128;
         *  this field configures right coordinate of focus window a, must <= hnum-2
         */
        uint32_t af_bpoint_a:12;
        uint32_t reserved_12:4;
        /** af_tpoint_a : R/W; bitpos: [27:16]; default: 1;
         *  this field configures bottom coordinate of focus window a, must <= hnum-2
         */
        uint32_t af_tpoint_a:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_vscale_a_reg_t;

/** Type of af_hscale_b register
 *  h-scale of af window b register
 */
typedef union {
    struct {
        /** af_rpoint_b : R/W; bitpos: [11:0]; default: 128;
         *  this field configures left coordinate of focus window b, must >= 2
         */
        uint32_t af_rpoint_b:12;
        uint32_t reserved_12:4;
        /** af_lpoint_b : R/W; bitpos: [27:16]; default: 1;
         *  this field configures top coordinate of focus window b, must >= 2
         */
        uint32_t af_lpoint_b:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_hscale_b_reg_t;

/** Type of af_vscale_b register
 *  v-scale of af window b register
 */
typedef union {
    struct {
        /** af_bpoint_b : R/W; bitpos: [11:0]; default: 128;
         *  this field configures right coordinate of focus window b, must <= hnum-2
         */
        uint32_t af_bpoint_b:12;
        uint32_t reserved_12:4;
        /** af_tpoint_b : R/W; bitpos: [27:16]; default: 1;
         *  this field configures bottom coordinate of focus window b, must <= hnum-2
         */
        uint32_t af_tpoint_b:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_vscale_b_reg_t;

/** Type of af_hscale_c register
 *  v-scale of af window c register
 */
typedef union {
    struct {
        /** af_rpoint_c : R/W; bitpos: [11:0]; default: 128;
         *  this field configures left coordinate of focus window c, must >= 2
         */
        uint32_t af_rpoint_c:12;
        uint32_t reserved_12:4;
        /** af_lpoint_c : R/W; bitpos: [27:16]; default: 1;
         *  this field configures top coordinate of focus window c, must >= 2
         */
        uint32_t af_lpoint_c:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_hscale_c_reg_t;

/** Type of af_vscale_c register
 *  v-scale of af window c register
 */
typedef union {
    struct {
        /** af_bpoint_c : R/W; bitpos: [11:0]; default: 128;
         *  this field configures right coordinate of focus window c, must <= hnum-2
         */
        uint32_t af_bpoint_c:12;
        uint32_t reserved_12:4;
        /** af_tpoint_c : R/W; bitpos: [27:16]; default: 1;
         *  this field configures bottom coordinate of focus window c, must <= hnum-2
         */
        uint32_t af_tpoint_c:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_vscale_c_reg_t;

/** Type of awb_mode register
 *  awb mode control register
 */
typedef union {
    struct {
        /** awb_mode : R/W; bitpos: [1:0]; default: 3;
         *  this field configures awb algo sel. 00: none selected. 01: sel algo0. 10: sel
         *  algo1. 11: sel both algo0 and algo1
         */
        uint32_t awb_mode:2;
        uint32_t reserved_2:2;
        /** awb_sample : R/W; bitpos: [4]; default: 0;
         *  this bit configures awb sample location, 0:before ccm, 1:after ccm
         */
        uint32_t awb_sample:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} isp_awb_mode_reg_t;

/** Type of awb_hscale register
 *  h-scale of awb window
 */
typedef union {
    struct {
        /** awb_rpoint : R/W; bitpos: [11:0]; default: 1919;
         *  this field configures awb window right coordinate
         */
        uint32_t awb_rpoint:12;
        uint32_t reserved_12:4;
        /** awb_lpoint : R/W; bitpos: [27:16]; default: 0;
         *  this field configures awb window left coordinate
         */
        uint32_t awb_lpoint:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_awb_hscale_reg_t;

/** Type of awb_vscale register
 *  v-scale of awb window
 */
typedef union {
    struct {
        /** awb_bpoint : R/W; bitpos: [11:0]; default: 1079;
         *  this field configures awb window bottom coordinate
         */
        uint32_t awb_bpoint:12;
        uint32_t reserved_12:4;
        /** awb_tpoint : R/W; bitpos: [27:16]; default: 0;
         *  this field configures awb window top coordinate
         */
        uint32_t awb_tpoint:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_awb_vscale_reg_t;

/** Type of awb_th_lum register
 *  awb lum threshold register
 */
typedef union {
    struct {
        /** awb_min_lum : R/W; bitpos: [9:0]; default: 0;
         *  this field configures lower threshold of r+g+b
         */
        uint32_t awb_min_lum:10;
        uint32_t reserved_10:6;
        /** awb_max_lum : R/W; bitpos: [25:16]; default: 765;
         *  this field configures upper threshold of r+g+b
         */
        uint32_t awb_max_lum:10;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} isp_awb_th_lum_reg_t;

/** Type of awb_th_rg register
 *  awb r/g threshold register
 */
typedef union {
    struct {
        /** awb_min_rg : R/W; bitpos: [9:0]; default: 0;
         *  this field configures lower threshold of r/g, 2bit integer and 8bit fraction
         */
        uint32_t awb_min_rg:10;
        uint32_t reserved_10:6;
        /** awb_max_rg : R/W; bitpos: [25:16]; default: 1023;
         *  this field configures upper threshold of r/g, 2bit integer and 8bit fraction
         */
        uint32_t awb_max_rg:10;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} isp_awb_th_rg_reg_t;

/** Type of awb_th_bg register
 *  awb b/g threshold register
 */
typedef union {
    struct {
        /** awb_min_bg : R/W; bitpos: [9:0]; default: 0;
         *  this field configures lower threshold of b/g, 2bit integer and 8bit fraction
         */
        uint32_t awb_min_bg:10;
        uint32_t reserved_10:6;
        /** awb_max_bg : R/W; bitpos: [25:16]; default: 1023;
         *  this field configures upper threshold of b/g, 2bit integer and 8bit fraction
         */
        uint32_t awb_max_bg:10;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} isp_awb_th_bg_reg_t;

/** Type of color_ctrl register
 *  color control register
 */
typedef union {
    struct {
        /** color_saturation : R/W; bitpos: [7:0]; default: 128;
         *  this field configures the color saturation value
         */
        uint32_t color_saturation:8;
        /** color_hue : R/W; bitpos: [15:8]; default: 0;
         *  this field configures the color hue angle
         */
        uint32_t color_hue:8;
        /** color_contrast : R/W; bitpos: [23:16]; default: 128;
         *  this field configures the color contrast value
         */
        uint32_t color_contrast:8;
        /** color_brightness : R/W; bitpos: [31:24]; default: 0;
         *  this field configures the color brightness value, signed 2's complement
         */
        uint32_t color_brightness:8;
    };
    uint32_t val;
} isp_color_ctrl_reg_t;

/** Type of blc_value register
 *  blc black level register
 */
typedef union {
    struct {
        /** blc_r3_value : R/W; bitpos: [7:0]; default: 0;
         *  this field configures the black level of bottom right channel of bayer img
         */
        uint32_t blc_r3_value:8;
        /** blc_r2_value : R/W; bitpos: [15:8]; default: 0;
         *  this field configures the black level of bottom left channel of bayer img
         */
        uint32_t blc_r2_value:8;
        /** blc_r1_value : R/W; bitpos: [23:16]; default: 0;
         *  this field configures the black level of top right channel of bayer img
         */
        uint32_t blc_r1_value:8;
        /** blc_r0_value : R/W; bitpos: [31:24]; default: 0;
         *  this field configures the black level of top left channel of bayer img
         */
        uint32_t blc_r0_value:8;
    };
    uint32_t val;
} isp_blc_value_reg_t;

/** Type of blc_ctrl0 register
 *  blc stretch control register
 */
typedef union {
    struct {
        /** blc_r3_stretch : R/W; bitpos: [0]; default: 0;
         *  this bit configures the stretch feature of bottom right channel. 0: stretch
         *  disable, 1: stretch enable
         */
        uint32_t blc_r3_stretch:1;
        /** blc_r2_stretch : R/W; bitpos: [1]; default: 0;
         *  this bit configures the stretch feature of bottom left channel. 0: stretch disable,
         *  1: stretch enable
         */
        uint32_t blc_r2_stretch:1;
        /** blc_r1_stretch : R/W; bitpos: [2]; default: 0;
         *  this bit configures the stretch feature of top right channel. 0: stretch disable,
         *  1: stretch enable
         */
        uint32_t blc_r1_stretch:1;
        /** blc_r0_stretch : R/W; bitpos: [3]; default: 0;
         *  this bit configures the stretch feature of top left channel. 0: stretch disable, 1:
         *  stretch enable
         */
        uint32_t blc_r0_stretch:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} isp_blc_ctrl0_reg_t;

/** Type of blc_ctrl1 register
 *  blc window control register
 */
typedef union {
    struct {
        /** blc_window_top : R/W; bitpos: [10:0]; default: 0;
         *  this field configures blc average calculation window top
         */
        uint32_t blc_window_top:11;
        /** blc_window_left : R/W; bitpos: [21:11]; default: 0;
         *  this field configures blc average calculation window left
         */
        uint32_t blc_window_left:11;
        /** blc_window_vnum : R/W; bitpos: [25:22]; default: 0;
         *  this field configures blc average calculation window vnum
         */
        uint32_t blc_window_vnum:4;
        /** blc_window_hnum : R/W; bitpos: [29:26]; default: 0;
         *  this field configures blc average calculation window hnum
         */
        uint32_t blc_window_hnum:4;
        /** blc_filter_en : R/W; bitpos: [30]; default: 0;
         *  this bit configures enable blc average input filter. 0: disable, 1: enable
         */
        uint32_t blc_filter_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} isp_blc_ctrl1_reg_t;

/** Type of blc_ctrl2 register
 *  blc black threshold control register
 */
typedef union {
    struct {
        /** blc_r3_th : R/W; bitpos: [7:0]; default: 0;
         *  this field configures black threshold when get blc average of bottom right channel
         */
        uint32_t blc_r3_th:8;
        /** blc_r2_th : R/W; bitpos: [15:8]; default: 0;
         *  this field configures black threshold when get blc average of bottom left channel
         */
        uint32_t blc_r2_th:8;
        /** blc_r1_th : R/W; bitpos: [23:16]; default: 0;
         *  this field configures black threshold when get blc average of top right channel
         */
        uint32_t blc_r1_th:8;
        /** blc_r0_th : R/W; bitpos: [31:24]; default: 0;
         *  this field configures black threshold when get blc average of top left channel
         */
        uint32_t blc_r0_th:8;
    };
    uint32_t val;
} isp_blc_ctrl2_reg_t;

/** Type of hist_mode register
 *  histogram mode control register
 */
typedef union {
    struct {
        /** hist_mode : R/W; bitpos: [2:0]; default: 4;
         *  this field configures statistic mode. 0: RAW_B, 1: RAW_GB, 2: RAW_GR 3: RAW_R, 4:
         *  RGB, 5:YUV_Y, 6:YUV_U, 7:YUV_V
         */
        uint32_t hist_mode:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} isp_hist_mode_reg_t;

/** Type of hist_coeff register
 *  histogram rgb to gray coefficients register
 */
typedef union {
    struct {
        /** hist_coeff_b : R/W; bitpos: [7:0]; default: 85;
         *  this field configures coefficient of B when set hist_mode to RGB, sum of coeff_r
         *  and coeff_g and coeff_b should be 256
         */
        uint32_t hist_coeff_b:8;
        /** hist_coeff_g : R/W; bitpos: [15:8]; default: 85;
         *  this field configures coefficient of G when set hist_mode to RGB, sum of coeff_r
         *  and coeff_g and coeff_b should be 256
         */
        uint32_t hist_coeff_g:8;
        /** hist_coeff_r : R/W; bitpos: [23:16]; default: 85;
         *  this field configures coefficient of R when set hist_mode to RGB, sum of coeff_r
         *  and coeff_g and coeff_b should be 256
         */
        uint32_t hist_coeff_r:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} isp_hist_coeff_reg_t;

/** Type of hist_offs register
 *  histogram window offsets register
 */
typedef union {
    struct {
        /** hist_y_offs : R/W; bitpos: [11:0]; default: 0;
         *  this field configures y coordinate of first window
         */
        uint32_t hist_y_offs:12;
        uint32_t reserved_12:4;
        /** hist_x_offs : R/W; bitpos: [27:16]; default: 0;
         *  this field configures x coordinate of first window
         */
        uint32_t hist_x_offs:12;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_hist_offs_reg_t;

/** Type of hist_size register
 *  histogram sub-window size register
 */
typedef union {
    struct {
        /** hist_y_size : R/W; bitpos: [8:0]; default: 32;
         *  this field configures y direction size of subwindow
         */
        uint32_t hist_y_size:9;
        uint32_t reserved_9:7;
        /** hist_x_size : R/W; bitpos: [24:16]; default: 18;
         *  this field configures x direction size of subwindow
         */
        uint32_t hist_x_size:9;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} isp_hist_size_reg_t;

/** Type of hist_seg register
 *  histogram bin control register
 */
typedef union {
    struct {
        /** hist_seg: R/W;
         * default:
         * 16, 32, 48, 64,
         * 80, 96, 112, 128,
         * 144, 160, 176, 192,
         * 208, 224, 240
         *  this field configures threshold of histogram
         */
        uint8_t hist_seg_b[4];
    };
    uint32_t val;
} isp_hist_seg_reg_t;


/** Type of hist_seg0 register
 *  histogram bin control register 0
 */
typedef union {
    struct {
        /** hist_seg_3_4 : R/W; bitpos: [7:0]; default: 64;
         *  this field configures threshold of histogram bin 3 and bin 4
         */
        uint32_t hist_seg_3_4:8;
        /** hist_seg_2_3 : R/W; bitpos: [15:8]; default: 48;
         *  this field configures threshold of histogram bin 2 and bin 3
         */
        uint32_t hist_seg_2_3:8;
        /** hist_seg_1_2 : R/W; bitpos: [23:16]; default: 32;
         *  this field configures threshold of histogram bin 1 and bin 2
         */
        uint32_t hist_seg_1_2:8;
        /** hist_seg_0_1 : R/W; bitpos: [31:24]; default: 16;
         *  this field configures threshold of histogram bin 0 and bin 1
         */
        uint32_t hist_seg_0_1:8;
    };
    uint32_t val;
} isp_hist_seg0_reg_t;

/** Type of hist_seg1 register
 *  histogram bin control register 1
 */
typedef union {
    struct {
        /** hist_seg_7_8 : R/W; bitpos: [7:0]; default: 128;
         *  this field configures threshold of histogram bin 7 and bin 8
         */
        uint32_t hist_seg_7_8:8;
        /** hist_seg_6_7 : R/W; bitpos: [15:8]; default: 112;
         *  this field configures threshold of histogram bin 6 and bin 7
         */
        uint32_t hist_seg_6_7:8;
        /** hist_seg_5_6 : R/W; bitpos: [23:16]; default: 96;
         *  this field configures threshold of histogram bin 5 and bin 6
         */
        uint32_t hist_seg_5_6:8;
        /** hist_seg_4_5 : R/W; bitpos: [31:24]; default: 80;
         *  this field configures threshold of histogram bin 4 and bin 5
         */
        uint32_t hist_seg_4_5:8;
    };
    uint32_t val;
} isp_hist_seg1_reg_t;

/** Type of hist_seg2 register
 *  histogram bin control register 2
 */
typedef union {
    struct {
        /** hist_seg_11_12 : R/W; bitpos: [7:0]; default: 192;
         *  this field configures threshold of histogram bin 11 and bin 12
         */
        uint32_t hist_seg_11_12:8;
        /** hist_seg_10_11 : R/W; bitpos: [15:8]; default: 176;
         *  this field configures threshold of histogram bin 10 and bin 11
         */
        uint32_t hist_seg_10_11:8;
        /** hist_seg_9_10 : R/W; bitpos: [23:16]; default: 160;
         *  this field configures threshold of histogram bin 9 and bin 10
         */
        uint32_t hist_seg_9_10:8;
        /** hist_seg_8_9 : R/W; bitpos: [31:24]; default: 144;
         *  this field configures threshold of histogram bin 8 and bin 9
         */
        uint32_t hist_seg_8_9:8;
    };
    uint32_t val;
} isp_hist_seg2_reg_t;

/** Type of hist_seg3 register
 *  histogram bin control register 3
 */
typedef union {
    struct {
        /** hist_seg_14_15 : R/W; bitpos: [7:0]; default: 240;
         *  this field configures threshold of histogram bin 14 and bin 15
         */
        uint32_t hist_seg_14_15:8;
        /** hist_seg_13_14 : R/W; bitpos: [15:8]; default: 224;
         *  this field configures threshold of histogram bin 13 and bin 14
         */
        uint32_t hist_seg_13_14:8;
        /** hist_seg_12_13 : R/W; bitpos: [23:16]; default: 208;
         *  this field configures threshold of histogram bin 12 and bin 13
         */
        uint32_t hist_seg_12_13:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} isp_hist_seg3_reg_t;

/** Type of hist_weight register
 *  histogram sub-window weight register 0
 */
typedef union {
    struct {
        /** histogram weight : RO; bitpos: [31:0];
         *  weight[12] default 232, others default 1
         *  this field represents the weight of histogram subwindow, sum of all weight should be 256
         */
        uint8_t hist_weight_b[4];
    };
    uint32_t val;
} isp_hist_weight_reg_t;

/** Type of hist_weight0 register
 *  histogram sub-window weight register 0
 */
typedef union {
    struct {
        /** hist_weight_03 : R/W; bitpos: [7:0]; default: 1;
         *  this field configures weight of subwindow 03
         */
        uint32_t hist_weight_03:8;
        /** hist_weight_02 : R/W; bitpos: [15:8]; default: 1;
         *  this field configures weight of subwindow 02
         */
        uint32_t hist_weight_02:8;
        /** hist_weight_01 : R/W; bitpos: [23:16]; default: 1;
         *  this field configures weight of subwindow 01
         */
        uint32_t hist_weight_01:8;
        /** hist_weight_00 : R/W; bitpos: [31:24]; default: 1;
         *  this field configures weight of subwindow 00 and sum of all weight should be 256
         */
        uint32_t hist_weight_00:8;
    };
    uint32_t val;
} isp_hist_weight0_reg_t;

/** Type of hist_weight1 register
 *  histogram sub-window weight register 1
 */
typedef union {
    struct {
        /** hist_weight_12 : R/W; bitpos: [7:0]; default: 1;
         *  this field configures weight of subwindow 12
         */
        uint32_t hist_weight_12:8;
        /** hist_weight_11 : R/W; bitpos: [15:8]; default: 1;
         *  this field configures weight of subwindow 11
         */
        uint32_t hist_weight_11:8;
        /** hist_weight_10 : R/W; bitpos: [23:16]; default: 1;
         *  this field configures weight of subwindow 10
         */
        uint32_t hist_weight_10:8;
        /** hist_weight_04 : R/W; bitpos: [31:24]; default: 1;
         *  this field configures weight of subwindow 04
         */
        uint32_t hist_weight_04:8;
    };
    uint32_t val;
} isp_hist_weight1_reg_t;

/** Type of hist_weight2 register
 *  histogram sub-window weight register 2
 */
typedef union {
    struct {
        /** hist_weight_21 : R/W; bitpos: [7:0]; default: 1;
         *  this field configures weight of subwindow 21
         */
        uint32_t hist_weight_21:8;
        /** hist_weight_20 : R/W; bitpos: [15:8]; default: 1;
         *  this field configures weight of subwindow 20
         */
        uint32_t hist_weight_20:8;
        /** hist_weight_14 : R/W; bitpos: [23:16]; default: 1;
         *  this field configures weight of subwindow 04
         */
        uint32_t hist_weight_14:8;
        /** hist_weight_13 : R/W; bitpos: [31:24]; default: 1;
         *  this field configures weight of subwindow 13
         */
        uint32_t hist_weight_13:8;
    };
    uint32_t val;
} isp_hist_weight2_reg_t;

/** Type of hist_weight3 register
 *  histogram sub-window weight register 3
 */
typedef union {
    struct {
        /** hist_weight_30 : R/W; bitpos: [7:0]; default: 1;
         *  this field configures weight of subwindow 30
         */
        uint32_t hist_weight_30:8;
        /** hist_weight_24 : R/W; bitpos: [15:8]; default: 1;
         *  this field configures weight of subwindow 24
         */
        uint32_t hist_weight_24:8;
        /** hist_weight_23 : R/W; bitpos: [23:16]; default: 1;
         *  this field configures weight of subwindow 23
         */
        uint32_t hist_weight_23:8;
        /** hist_weight_22 : R/W; bitpos: [31:24]; default: 232;
         *  this field configures weight of subwindow 22
         */
        uint32_t hist_weight_22:8;
    };
    uint32_t val;
} isp_hist_weight3_reg_t;

/** Type of hist_weight4 register
 *  histogram sub-window weight register 4
 */
typedef union {
    struct {
        /** hist_weight_34 : R/W; bitpos: [7:0]; default: 1;
         *  this field configures weight of subwindow 34
         */
        uint32_t hist_weight_34:8;
        /** hist_weight_33 : R/W; bitpos: [15:8]; default: 1;
         *  this field configures weight of subwindow 33
         */
        uint32_t hist_weight_33:8;
        /** hist_weight_32 : R/W; bitpos: [23:16]; default: 1;
         *  this field configures weight of subwindow 32
         */
        uint32_t hist_weight_32:8;
        /** hist_weight_31 : R/W; bitpos: [31:24]; default: 1;
         *  this field configures weight of subwindow 31
         */
        uint32_t hist_weight_31:8;
    };
    uint32_t val;
} isp_hist_weight4_reg_t;

/** Type of hist_weight5 register
 *  histogram sub-window weight register 5
 */
typedef union {
    struct {
        /** hist_weight_43 : R/W; bitpos: [7:0]; default: 1;
         *  this field configures weight of subwindow 43
         */
        uint32_t hist_weight_43:8;
        /** hist_weight_42 : R/W; bitpos: [15:8]; default: 1;
         *  this field configures weight of subwindow 42
         */
        uint32_t hist_weight_42:8;
        /** hist_weight_41 : R/W; bitpos: [23:16]; default: 1;
         *  this field configures weight of subwindow 41
         */
        uint32_t hist_weight_41:8;
        /** hist_weight_40 : R/W; bitpos: [31:24]; default: 1;
         *  this field configures weight of subwindow 40
         */
        uint32_t hist_weight_40:8;
    };
    uint32_t val;
} isp_hist_weight5_reg_t;

/** Type of hist_weight6 register
 *  histogram sub-window weight register 6
 */
typedef union {
    struct {
        /** hist_weight_44 : R/W; bitpos: [7:0]; default: 1;
         *  this field configures weight of subwindow 44
         */
        uint32_t hist_weight_44:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} isp_hist_weight6_reg_t;

/** Type of mem_aux_ctrl_0 register
 *  mem aux control register 0
 */
typedef union {
    struct {
        /** header_mem_aux_ctrl : R/W; bitpos: [13:0]; default: 4896;
         *  this field configures the mem_aux of isp input buffer memory
         */
        uint32_t header_mem_aux_ctrl:14;
        uint32_t reserved_14:2;
        /** dpc_lut_mem_aux_ctrl : R/W; bitpos: [29:16]; default: 4896;
         *  this field represents this field configures the mem_aux of dpc lut memory
         */
        uint32_t dpc_lut_mem_aux_ctrl:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_mem_aux_ctrl_0_reg_t;

/** Type of mem_aux_ctrl_1 register
 *  mem aux control register 1
 */
typedef union {
    struct {
        /** lsc_lut_r_gr_mem_aux_ctrl : R/W; bitpos: [13:0]; default: 4896;
         *  this field configures the mem_aux of lsc r gr lut memory
         */
        uint32_t lsc_lut_r_gr_mem_aux_ctrl:14;
        uint32_t reserved_14:2;
        /** lsc_lut_gb_b_mem_aux_ctrl : R/W; bitpos: [29:16]; default: 4896;
         *  this field configures the mem_aux of lsc gb b lut memory
         */
        uint32_t lsc_lut_gb_b_mem_aux_ctrl:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_mem_aux_ctrl_1_reg_t;

/** Type of mem_aux_ctrl_2 register
 *  mem aux control register 2
 */
typedef union {
    struct {
        /** bf_matrix_mem_aux_ctrl : R/W; bitpos: [13:0]; default: 4896;
         *  this field configures the mem_aux of bf line buffer memory
         */
        uint32_t bf_matrix_mem_aux_ctrl:14;
        uint32_t reserved_14:2;
        /** dpc_matrix_mem_aux_ctrl : R/W; bitpos: [29:16]; default: 4896;
         *  this field configures the mem_aux of dpc line buffer memory
         */
        uint32_t dpc_matrix_mem_aux_ctrl:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_mem_aux_ctrl_2_reg_t;

/** Type of mem_aux_ctrl_3 register
 *  mem aux control register 3
 */
typedef union {
    struct {
        /** sharp_matrix_y_mem_aux_ctrl : R/W; bitpos: [13:0]; default: 4896;
         *  this field configures the mem_aux of sharp y line buffer memory
         */
        uint32_t sharp_matrix_y_mem_aux_ctrl:14;
        uint32_t reserved_14:2;
        /** demosaic_matrix_mem_aux_ctrl : R/W; bitpos: [29:16]; default: 4896;
         *  this field configures the mem_aux of demosaic line buffer memory
         */
        uint32_t demosaic_matrix_mem_aux_ctrl:14;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_mem_aux_ctrl_3_reg_t;

/** Type of mem_aux_ctrl_4 register
 *  mem aux control register 4
 */
typedef union {
    struct {
        /** sharp_matrix_uv_mem_aux_ctrl : R/W; bitpos: [13:0]; default: 4896;
         *  this field configures the mem_aux of sharp uv line buffer memory
         */
        uint32_t sharp_matrix_uv_mem_aux_ctrl:14;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} isp_mem_aux_ctrl_4_reg_t;

/** Type of yuv_format register
 *  yuv format control register
 */
typedef union {
    struct {
        /** yuv_mode : R/W; bitpos: [0]; default: 0;
         *  this bit configures the yuv mode. 0: ITU-R BT.601, 1: ITU-R BT.709
         */
        uint32_t yuv_mode:1;
        /** yuv_range : R/W; bitpos: [1]; default: 0;
         *  this bit configures the yuv range. 0: full range, 1: limit range
         */
        uint32_t yuv_range:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} isp_yuv_format_reg_t;

/** Type of rdn_eco_low register
 *  rdn eco all low register
 */
typedef union {
    struct {
        /** rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  rdn_eco_low
         */
        uint32_t rdn_eco_low:32;
    };
    uint32_t val;
} isp_rdn_eco_low_reg_t;

/** Type of rdn_eco_high register
 *  rdn eco all high register
 */
typedef union {
    struct {
        /** rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  rdn_eco_high
         */
        uint32_t rdn_eco_high:32;
    };
    uint32_t val;
} isp_rdn_eco_high_reg_t;


/** Group: Status Registers */
/** Type of dpc_deadpix_cnt register
 *  DPC dead-pix number register
 */
typedef union {
    struct {
        /** dpc_deadpix_cnt : RO; bitpos: [9:0]; default: 0;
         *  this field represents the dead pixel count
         */
        uint32_t dpc_deadpix_cnt:10;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} isp_dpc_deadpix_cnt_reg_t;

/** Type of lut_rdata register
 *  LUT read data register
 */
typedef union {
    struct {
        /** lut_rdata : RO; bitpos: [31:0]; default: 0;
         *  this field represents the read data of lut. read ISP_LUT_RDATA after write
         *  ISP_LUT_CMD register
         */
        uint32_t lut_rdata:32;
    };
    uint32_t val;
} isp_lut_rdata_reg_t;

/** Type of ae_block_mean register
 *  ae statistic result
 */
typedef union {
    struct {
        /** ae_lum : RO; bitpos: [31:0]; default: 0;
         *  this field represents the result of AE block
         */
        uint8_t ae_b_mean[4];
    };
    uint32_t val;
} isp_ae_block_mean_reg_t;

/** Type of ae_block_mean_0 register
 *  ae statistic result register 0
 */
typedef union {
    struct {
        /** ae_b03_mean : RO; bitpos: [7:0]; default: 0;
         *  this field configures block03 Y mean data
         */
        uint32_t ae_b03_mean:8;
        /** ae_b02_mean : RO; bitpos: [15:8]; default: 0;
         *  this field configures block02 Y mean data
         */
        uint32_t ae_b02_mean:8;
        /** ae_b01_mean : RO; bitpos: [23:16]; default: 0;
         *  this field configures block01 Y mean data
         */
        uint32_t ae_b01_mean:8;
        /** ae_b00_mean : RO; bitpos: [31:24]; default: 0;
         *  this field configures block00 Y mean data
         */
        uint32_t ae_b00_mean:8;
    };
    uint32_t val;
} isp_ae_block_mean_0_reg_t;

/** Type of ae_block_mean_1 register
 *  ae statistic result register 1
 */
typedef union {
    struct {
        /** ae_b12_mean : RO; bitpos: [7:0]; default: 0;
         *  this field configures block12 Y mean data
         */
        uint32_t ae_b12_mean:8;
        /** ae_b11_mean : RO; bitpos: [15:8]; default: 0;
         *  this field configures block11 Y mean data
         */
        uint32_t ae_b11_mean:8;
        /** ae_b10_mean : RO; bitpos: [23:16]; default: 0;
         *  this field configures block10 Y mean data
         */
        uint32_t ae_b10_mean:8;
        /** ae_b04_mean : RO; bitpos: [31:24]; default: 0;
         *  this field configures block04 Y mean data
         */
        uint32_t ae_b04_mean:8;
    };
    uint32_t val;
} isp_ae_block_mean_1_reg_t;

/** Type of ae_block_mean_2 register
 *  ae statistic result register 2
 */
typedef union {
    struct {
        /** ae_b21_mean : RO; bitpos: [7:0]; default: 0;
         *  this field configures block21 Y mean data
         */
        uint32_t ae_b21_mean:8;
        /** ae_b20_mean : RO; bitpos: [15:8]; default: 0;
         *  this field configures block20 Y mean data
         */
        uint32_t ae_b20_mean:8;
        /** ae_b14_mean : RO; bitpos: [23:16]; default: 0;
         *  this field configures block14 Y mean data
         */
        uint32_t ae_b14_mean:8;
        /** ae_b13_mean : RO; bitpos: [31:24]; default: 0;
         *  this field configures block13 Y mean data
         */
        uint32_t ae_b13_mean:8;
    };
    uint32_t val;
} isp_ae_block_mean_2_reg_t;

/** Type of ae_block_mean_3 register
 *  ae statistic result register 3
 */
typedef union {
    struct {
        /** ae_b30_mean : RO; bitpos: [7:0]; default: 0;
         *  this field configures block30 Y mean data
         */
        uint32_t ae_b30_mean:8;
        /** ae_b24_mean : RO; bitpos: [15:8]; default: 0;
         *  this field configures block24 Y mean data
         */
        uint32_t ae_b24_mean:8;
        /** ae_b23_mean : RO; bitpos: [23:16]; default: 0;
         *  this field configures block23 Y mean data
         */
        uint32_t ae_b23_mean:8;
        /** ae_b22_mean : RO; bitpos: [31:24]; default: 0;
         *  this field configures block22 Y mean data
         */
        uint32_t ae_b22_mean:8;
    };
    uint32_t val;
} isp_ae_block_mean_3_reg_t;

/** Type of ae_block_mean_4 register
 *  ae statistic result register 4
 */
typedef union {
    struct {
        /** ae_b34_mean : RO; bitpos: [7:0]; default: 0;
         *  this field configures block34 Y mean data
         */
        uint32_t ae_b34_mean:8;
        /** ae_b33_mean : RO; bitpos: [15:8]; default: 0;
         *  this field configures block33 Y mean data
         */
        uint32_t ae_b33_mean:8;
        /** ae_b32_mean : RO; bitpos: [23:16]; default: 0;
         *  this field configures block32 Y mean data
         */
        uint32_t ae_b32_mean:8;
        /** ae_b31_mean : RO; bitpos: [31:24]; default: 0;
         *  this field configures block31 Y mean data
         */
        uint32_t ae_b31_mean:8;
    };
    uint32_t val;
} isp_ae_block_mean_4_reg_t;

/** Type of ae_block_mean_5 register
 *  ae statistic result register 5
 */
typedef union {
    struct {
        /** ae_b43_mean : RO; bitpos: [7:0]; default: 0;
         *  this field configures block43 Y mean data
         */
        uint32_t ae_b43_mean:8;
        /** ae_b42_mean : RO; bitpos: [15:8]; default: 0;
         *  this field configures block42 Y mean data
         */
        uint32_t ae_b42_mean:8;
        /** ae_b41_mean : RO; bitpos: [23:16]; default: 0;
         *  this field configures block41 Y mean data
         */
        uint32_t ae_b41_mean:8;
        /** ae_b40_mean : RO; bitpos: [31:24]; default: 0;
         *  this field configures block40 Y mean data
         */
        uint32_t ae_b40_mean:8;
    };
    uint32_t val;
} isp_ae_block_mean_5_reg_t;

/** Type of ae_block_mean_6 register
 *  ae statistic result register 6
 */
typedef union {
    struct {
        uint32_t reserved_0:24;
        /** ae_b44_mean : RO; bitpos: [31:24]; default: 0;
         *  this field configures block44 Y mean data
         */
        uint32_t ae_b44_mean:8;
    };
    uint32_t val;
} isp_ae_block_mean_6_reg_t;
/** Type of af_sum_a register
 *  result of sum of af window a
 */
typedef union {
    struct {
        /** af_suma : RO; bitpos: [29:0]; default: 0;
         *  this field represents the result of accumulation of pix grad of focus window a
         */
        uint32_t af_suma:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_af_sum_a_reg_t;

/** Type of af_sum_b register
 *  result of sum of af window b
 */
typedef union {
    struct {
        /** af_sumb : RO; bitpos: [29:0]; default: 0;
         *  this field represents the result of accumulation of pix grad of focus window b
         */
        uint32_t af_sumb:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_af_sum_b_reg_t;

/** Type of af_sum_c register
 *  result of sum of af window c
 */
typedef union {
    struct {
        /** af_sumc : RO; bitpos: [29:0]; default: 0;
         *  this field represents the result of accumulation of pix grad of focus window c
         */
        uint32_t af_sumc:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} isp_af_sum_c_reg_t;

/** Type of af_lum_a register
 *  result of lum of af window a
 */
typedef union {
    struct {
        /** af_luma : RO; bitpos: [27:0]; default: 0;
         *  this field represents the result of accumulation of pix light of focus window a
         */
        uint32_t af_luma:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_lum_a_reg_t;

/** Type of af_lum_b register
 *  result of lum of af window b
 */
typedef union {
    struct {
        /** af_lumb : RO; bitpos: [27:0]; default: 0;
         *  this field represents the result of accumulation of pix light of focus window b
         */
        uint32_t af_lumb:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_lum_b_reg_t;

/** Type of af_lum_c register
 *  result of lum of af window c
 */
typedef union {
    struct {
        /** af_lumc : RO; bitpos: [27:0]; default: 0;
         *  this field represents the result of accumulation of pix light of focus window c
         */
        uint32_t af_lumc:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} isp_af_lum_c_reg_t;

/** Type of awb0_white_cnt register
 *  result of awb white point number
 */
typedef union {
    struct {
        /** awb0_white_cnt : RO; bitpos: [23:0]; default: 0;
         *  this field configures number of white point detected of algo0
         */
        uint32_t awb0_white_cnt:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} isp_awb0_white_cnt_reg_t;

/** Type of awb0_acc_r register
 *  result of accumulate of r channel of all white points
 */
typedef union {
    struct {
        /** awb0_acc_r : RO; bitpos: [31:0]; default: 0;
         *  this field represents accumulate of channel r of all white point of algo0
         */
        uint32_t awb0_acc_r:32;
    };
    uint32_t val;
} isp_awb0_acc_r_reg_t;

/** Type of awb0_acc_g register
 *  result of accumulate of g channel of all white points
 */
typedef union {
    struct {
        /** awb0_acc_g : RO; bitpos: [31:0]; default: 0;
         *  this field represents accumulate of channel g of all white point of algo0
         */
        uint32_t awb0_acc_g:32;
    };
    uint32_t val;
} isp_awb0_acc_g_reg_t;

/** Type of awb0_acc_b register
 *  result of accumulate of b channel of all white points
 */
typedef union {
    struct {
        /** awb0_acc_b : RO; bitpos: [31:0]; default: 0;
         *  this field represents accumulate of channel b of all white point of algo0
         */
        uint32_t awb0_acc_b:32;
    };
    uint32_t val;
} isp_awb0_acc_b_reg_t;

/** Type of blc_mean register
 *  results of the average of black window
 */
typedef union {
    struct {
        /** blc_r3_mean : RO; bitpos: [7:0]; default: 0;
         *  this field represents the average black value of bottom right channel
         */
        uint32_t blc_r3_mean:8;
        /** blc_r2_mean : RO; bitpos: [15:8]; default: 0;
         *  this field represents the average black value of bottom left channel
         */
        uint32_t blc_r2_mean:8;
        /** blc_r1_mean : RO; bitpos: [23:16]; default: 0;
         *  this field represents the average black value of top right channel
         */
        uint32_t blc_r1_mean:8;
        /** blc_r0_mean : RO; bitpos: [31:24]; default: 0;
         *  this field represents the average black value of top left channel
         */
        uint32_t blc_r0_mean:8;
    };
    uint32_t val;
} isp_blc_mean_reg_t;

/** Type of hist_bin register
 *  result of histogram bin n
 */
typedef union {
    struct {
        /** hist_bin_n : RO; bitpos: [16:0]; default: 0;
         *  this field represents result of histogram bin n
         */
        uint32_t hist_bin_n:17;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} isp_hist_binn_reg_t;

/** Type of rdn_eco_cs register
 *  rdn eco cs register
 */
typedef union {
    struct {
        /** rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  rdn_eco_en
         */
        uint32_t rdn_eco_en:1;
        /** rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  rdn_eco_result
         */
        uint32_t rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} isp_rdn_eco_cs_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  raw interrupt register
 */
typedef union {
    struct {
        /** isp_data_type_err_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  the raw interrupt status of input data type error. isp only support RGB bayer data
         *  type, other type will report type_err_int
         */
        uint32_t isp_data_type_err_int_raw:1;
        /** isp_async_fifo_ovf_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
         *  the raw interrupt status of isp input fifo overflow
         */
        uint32_t isp_async_fifo_ovf_int_raw:1;
        /** isp_buf_full_int_raw : R/SS/WTC; bitpos: [2]; default: 0;
         *  the raw interrupt status of isp input buffer full
         */
        uint32_t isp_buf_full_int_raw:1;
        /** isp_hvnum_setting_err_int_raw : R/SS/WTC; bitpos: [3]; default: 0;
         *  the raw interrupt status of hnum and vnum setting format error
         */
        uint32_t isp_hvnum_setting_err_int_raw:1;
        /** isp_data_type_setting_err_int_raw : R/SS/WTC; bitpos: [4]; default: 0;
         *  the raw interrupt status of setting invalid reg_data_type
         */
        uint32_t isp_data_type_setting_err_int_raw:1;
        /** isp_mipi_hnum_unmatch_int_raw : R/SS/WTC; bitpos: [5]; default: 0;
         *  the raw interrupt status of hnum setting unmatch with mipi input
         */
        uint32_t isp_mipi_hnum_unmatch_int_raw:1;
        /** dpc_check_done_int_raw : R/SS/WTC; bitpos: [6]; default: 0;
         *  the raw interrupt status of dpc check done
         */
        uint32_t dpc_check_done_int_raw:1;
        /** gamma_xcoord_err_int_raw : R/SS/WTC; bitpos: [7]; default: 0;
         *  the raw interrupt status of gamma setting error. it report the sum of the lengths
         *  represented by reg_gamma_x00~x0F isn't equal to 256
         */
        uint32_t gamma_xcoord_err_int_raw:1;
        /** ae_monitor_int_raw : R/SS/WTC; bitpos: [8]; default: 0;
         *  the raw interrupt status of ae monitor
         */
        uint32_t ae_monitor_int_raw:1;
        /** ae_frame_done_int_raw : R/SS/WTC; bitpos: [9]; default: 0;
         *  the raw interrupt status of ae.
         */
        uint32_t ae_frame_done_int_raw:1;
        /** af_fdone_int_raw : R/SS/WTC; bitpos: [10]; default: 0;
         *  the raw interrupt status of af statistic. when auto_update enable, each frame done
         *  will send one int pulse when manual_update, each time when write 1 to
         *  reg_manual_update will send a int pulse when next frame done
         */
        uint32_t af_fdone_int_raw:1;
        /** af_env_int_raw : R/SS/WTC; bitpos: [11]; default: 0;
         *  the raw interrupt status of af monitor. send a int pulse when env_det function
         *  enabled and environment changes detected
         */
        uint32_t af_env_int_raw:1;
        /** awb_fdone_int_raw : R/SS/WTC; bitpos: [12]; default: 0;
         *  the raw interrupt status of awb. send a int pulse when statistic of one awb frame
         *  done
         */
        uint32_t awb_fdone_int_raw:1;
        /** hist_fdone_int_raw : R/SS/WTC; bitpos: [13]; default: 0;
         *  the raw interrupt status of histogram. send a int pulse when statistic of one frame
         *  histogram done
         */
        uint32_t hist_fdone_int_raw:1;
        /** frame_int_raw : R/SS/WTC; bitpos: [14]; default: 0;
         *  the raw interrupt status of isp frame end
         */
        uint32_t frame_int_raw:1;
        /** blc_frame_int_raw : R/SS/WTC; bitpos: [15]; default: 0;
         *  the raw interrupt status of blc frame done
         */
        uint32_t blc_frame_int_raw:1;
        /** lsc_frame_int_raw : R/SS/WTC; bitpos: [16]; default: 0;
         *  the raw interrupt status of lsc frame done
         */
        uint32_t lsc_frame_int_raw:1;
        /** dpc_frame_int_raw : R/SS/WTC; bitpos: [17]; default: 0;
         *  the raw interrupt status of dpc frame done
         */
        uint32_t dpc_frame_int_raw:1;
        /** bf_frame_int_raw : R/SS/WTC; bitpos: [18]; default: 0;
         *  the raw interrupt status of bf frame done
         */
        uint32_t bf_frame_int_raw:1;
        /** demosaic_frame_int_raw : R/SS/WTC; bitpos: [19]; default: 0;
         *  the raw interrupt status of demosaic frame done
         */
        uint32_t demosaic_frame_int_raw:1;
        /** median_frame_int_raw : R/SS/WTC; bitpos: [20]; default: 0;
         *  the raw interrupt status of median frame done
         */
        uint32_t median_frame_int_raw:1;
        /** ccm_frame_int_raw : R/SS/WTC; bitpos: [21]; default: 0;
         *  the raw interrupt status of ccm frame done
         */
        uint32_t ccm_frame_int_raw:1;
        /** gamma_frame_int_raw : R/SS/WTC; bitpos: [22]; default: 0;
         *  the raw interrupt status of gamma frame done
         */
        uint32_t gamma_frame_int_raw:1;
        /** rgb2yuv_frame_int_raw : R/SS/WTC; bitpos: [23]; default: 0;
         *  the raw interrupt status of rgb2yuv frame done
         */
        uint32_t rgb2yuv_frame_int_raw:1;
        /** sharp_frame_int_raw : R/SS/WTC; bitpos: [24]; default: 0;
         *  the raw interrupt status of sharp frame done
         */
        uint32_t sharp_frame_int_raw:1;
        /** color_frame_int_raw : R/SS/WTC; bitpos: [25]; default: 0;
         *  the raw interrupt status of color frame done
         */
        uint32_t color_frame_int_raw:1;
        /** yuv2rgb_frame_int_raw : R/SS/WTC; bitpos: [26]; default: 0;
         *  the raw interrupt status of yuv2rgb frame done
         */
        uint32_t yuv2rgb_frame_int_raw:1;
        /** tail_idi_frame_int_raw : R/SS/WTC; bitpos: [27]; default: 0;
         *  the raw interrupt status of isp_tail idi frame_end
         */
        uint32_t tail_idi_frame_int_raw:1;
        /** header_idi_frame_int_raw : R/SS/WTC; bitpos: [28]; default: 0;
         *  the raw interrupt status of real input frame end of isp_input
         */
        uint32_t header_idi_frame_int_raw:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} isp_int_raw_reg_t;

/** Type of int_st register
 *  masked interrupt register
 */
typedef union {
    struct {
        /** isp_data_type_err_int_st : RO; bitpos: [0]; default: 0;
         *  the masked interrupt status of input data type error
         */
        uint32_t isp_data_type_err_int_st:1;
        /** isp_async_fifo_ovf_int_st : RO; bitpos: [1]; default: 0;
         *  the masked interrupt status of isp input fifo overflow
         */
        uint32_t isp_async_fifo_ovf_int_st:1;
        /** isp_buf_full_int_st : RO; bitpos: [2]; default: 0;
         *  the masked interrupt status of isp input buffer full
         */
        uint32_t isp_buf_full_int_st:1;
        /** isp_hvnum_setting_err_int_st : RO; bitpos: [3]; default: 0;
         *  the masked interrupt status of hnum and vnum setting format error
         */
        uint32_t isp_hvnum_setting_err_int_st:1;
        /** isp_data_type_setting_err_int_st : RO; bitpos: [4]; default: 0;
         *  the masked interrupt status of setting invalid reg_data_type
         */
        uint32_t isp_data_type_setting_err_int_st:1;
        /** isp_mipi_hnum_unmatch_int_st : RO; bitpos: [5]; default: 0;
         *  the masked interrupt status of hnum setting unmatch with mipi input
         */
        uint32_t isp_mipi_hnum_unmatch_int_st:1;
        /** dpc_check_done_int_st : RO; bitpos: [6]; default: 0;
         *  the masked interrupt status of dpc check done
         */
        uint32_t dpc_check_done_int_st:1;
        /** gamma_xcoord_err_int_st : RO; bitpos: [7]; default: 0;
         *  the masked interrupt status of gamma setting error
         */
        uint32_t gamma_xcoord_err_int_st:1;
        /** ae_monitor_int_st : RO; bitpos: [8]; default: 0;
         *  the masked interrupt status of ae monitor
         */
        uint32_t ae_monitor_int_st:1;
        /** ae_frame_done_int_st : RO; bitpos: [9]; default: 0;
         *  the masked interrupt status of ae
         */
        uint32_t ae_frame_done_int_st:1;
        /** af_fdone_int_st : RO; bitpos: [10]; default: 0;
         *  the masked interrupt status of af statistic
         */
        uint32_t af_fdone_int_st:1;
        /** af_env_int_st : RO; bitpos: [11]; default: 0;
         *  the masked interrupt status of af monitor
         */
        uint32_t af_env_int_st:1;
        /** awb_fdone_int_st : RO; bitpos: [12]; default: 0;
         *  the masked interrupt status of awb
         */
        uint32_t awb_fdone_int_st:1;
        /** hist_fdone_int_st : RO; bitpos: [13]; default: 0;
         *  the masked interrupt status of histogram
         */
        uint32_t hist_fdone_int_st:1;
        /** frame_int_st : RO; bitpos: [14]; default: 0;
         *  the masked interrupt status of isp frame end
         */
        uint32_t frame_int_st:1;
        /** blc_frame_int_st : RO; bitpos: [15]; default: 0;
         *  the masked interrupt status of blc frame done
         */
        uint32_t blc_frame_int_st:1;
        /** lsc_frame_int_st : RO; bitpos: [16]; default: 0;
         *  the masked interrupt status of lsc frame done
         */
        uint32_t lsc_frame_int_st:1;
        /** dpc_frame_int_st : RO; bitpos: [17]; default: 0;
         *  the masked interrupt status of dpc frame done
         */
        uint32_t dpc_frame_int_st:1;
        /** bf_frame_int_st : RO; bitpos: [18]; default: 0;
         *  the masked interrupt status of bf frame done
         */
        uint32_t bf_frame_int_st:1;
        /** demosaic_frame_int_st : RO; bitpos: [19]; default: 0;
         *  the masked interrupt status of demosaic frame done
         */
        uint32_t demosaic_frame_int_st:1;
        /** median_frame_int_st : RO; bitpos: [20]; default: 0;
         *  the masked interrupt status of median frame done
         */
        uint32_t median_frame_int_st:1;
        /** ccm_frame_int_st : RO; bitpos: [21]; default: 0;
         *  the masked interrupt status of ccm frame done
         */
        uint32_t ccm_frame_int_st:1;
        /** gamma_frame_int_st : RO; bitpos: [22]; default: 0;
         *  the masked interrupt status of gamma frame done
         */
        uint32_t gamma_frame_int_st:1;
        /** rgb2yuv_frame_int_st : RO; bitpos: [23]; default: 0;
         *  the masked interrupt status of rgb2yuv frame done
         */
        uint32_t rgb2yuv_frame_int_st:1;
        /** sharp_frame_int_st : RO; bitpos: [24]; default: 0;
         *  the masked interrupt status of sharp frame done
         */
        uint32_t sharp_frame_int_st:1;
        /** color_frame_int_st : RO; bitpos: [25]; default: 0;
         *  the masked interrupt status of color frame done
         */
        uint32_t color_frame_int_st:1;
        /** yuv2rgb_frame_int_st : RO; bitpos: [26]; default: 0;
         *  the masked interrupt status of yuv2rgb frame done
         */
        uint32_t yuv2rgb_frame_int_st:1;
        /** tail_idi_frame_int_st : RO; bitpos: [27]; default: 0;
         *  the masked interrupt status of isp_tail idi frame_end
         */
        uint32_t tail_idi_frame_int_st:1;
        /** header_idi_frame_int_st : RO; bitpos: [28]; default: 0;
         *  the masked interrupt status of real input frame end of isp_input
         */
        uint32_t header_idi_frame_int_st:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} isp_int_st_reg_t;

/** Type of int_ena register
 *  interrupt enable register
 */
typedef union {
    struct {
        /** isp_data_type_err_int_ena : R/W; bitpos: [0]; default: 1;
         *  write 1 to enable input data type error
         */
        uint32_t isp_data_type_err_int_ena:1;
        /** isp_async_fifo_ovf_int_ena : R/W; bitpos: [1]; default: 1;
         *  write 1 to enable isp input fifo overflow
         */
        uint32_t isp_async_fifo_ovf_int_ena:1;
        /** isp_buf_full_int_ena : R/W; bitpos: [2]; default: 0;
         *  write 1 to enable isp input buffer full
         */
        uint32_t isp_buf_full_int_ena:1;
        /** isp_hvnum_setting_err_int_ena : R/W; bitpos: [3]; default: 0;
         *  write 1 to enable hnum and vnum setting format error
         */
        uint32_t isp_hvnum_setting_err_int_ena:1;
        /** isp_data_type_setting_err_int_ena : R/W; bitpos: [4]; default: 0;
         *  write 1 to enable setting invalid reg_data_type
         */
        uint32_t isp_data_type_setting_err_int_ena:1;
        /** isp_mipi_hnum_unmatch_int_ena : R/W; bitpos: [5]; default: 0;
         *  write 1 to enable hnum setting unmatch with mipi input
         */
        uint32_t isp_mipi_hnum_unmatch_int_ena:1;
        /** dpc_check_done_int_ena : R/W; bitpos: [6]; default: 1;
         *  write 1 to enable dpc check done
         */
        uint32_t dpc_check_done_int_ena:1;
        /** gamma_xcoord_err_int_ena : R/W; bitpos: [7]; default: 1;
         *  write 1 to enable gamma setting error
         */
        uint32_t gamma_xcoord_err_int_ena:1;
        /** ae_monitor_int_ena : R/W; bitpos: [8]; default: 0;
         *  write 1 to enable ae monitor
         */
        uint32_t ae_monitor_int_ena:1;
        /** ae_frame_done_int_ena : R/W; bitpos: [9]; default: 0;
         *  write 1 to enable ae
         */
        uint32_t ae_frame_done_int_ena:1;
        /** af_fdone_int_ena : R/W; bitpos: [10]; default: 0;
         *  write 1 to enable af statistic
         */
        uint32_t af_fdone_int_ena:1;
        /** af_env_int_ena : R/W; bitpos: [11]; default: 0;
         *  write 1 to enable af monitor
         */
        uint32_t af_env_int_ena:1;
        /** awb_fdone_int_ena : R/W; bitpos: [12]; default: 0;
         *  write 1 to enable awb
         */
        uint32_t awb_fdone_int_ena:1;
        /** hist_fdone_int_ena : R/W; bitpos: [13]; default: 0;
         *  write 1 to enable histogram
         */
        uint32_t hist_fdone_int_ena:1;
        /** frame_int_ena : R/W; bitpos: [14]; default: 0;
         *  write 1 to enable isp frame end
         */
        uint32_t frame_int_ena:1;
        /** blc_frame_int_ena : R/W; bitpos: [15]; default: 0;
         *  write 1 to enable blc frame done
         */
        uint32_t blc_frame_int_ena:1;
        /** lsc_frame_int_ena : R/W; bitpos: [16]; default: 0;
         *  write 1 to enable lsc frame done
         */
        uint32_t lsc_frame_int_ena:1;
        /** dpc_frame_int_ena : R/W; bitpos: [17]; default: 0;
         *  write 1 to enable dpc frame done
         */
        uint32_t dpc_frame_int_ena:1;
        /** bf_frame_int_ena : R/W; bitpos: [18]; default: 0;
         *  write 1 to enable bf frame done
         */
        uint32_t bf_frame_int_ena:1;
        /** demosaic_frame_int_ena : R/W; bitpos: [19]; default: 0;
         *  write 1 to enable demosaic frame done
         */
        uint32_t demosaic_frame_int_ena:1;
        /** median_frame_int_ena : R/W; bitpos: [20]; default: 0;
         *  write 1 to enable median frame done
         */
        uint32_t median_frame_int_ena:1;
        /** ccm_frame_int_ena : R/W; bitpos: [21]; default: 0;
         *  write 1 to enable ccm frame done
         */
        uint32_t ccm_frame_int_ena:1;
        /** gamma_frame_int_ena : R/W; bitpos: [22]; default: 0;
         *  write 1 to enable gamma frame done
         */
        uint32_t gamma_frame_int_ena:1;
        /** rgb2yuv_frame_int_ena : R/W; bitpos: [23]; default: 0;
         *  write 1 to enable rgb2yuv frame done
         */
        uint32_t rgb2yuv_frame_int_ena:1;
        /** sharp_frame_int_ena : R/W; bitpos: [24]; default: 0;
         *  write 1 to enable sharp frame done
         */
        uint32_t sharp_frame_int_ena:1;
        /** color_frame_int_ena : R/W; bitpos: [25]; default: 0;
         *  write 1 to enable color frame done
         */
        uint32_t color_frame_int_ena:1;
        /** yuv2rgb_frame_int_ena : R/W; bitpos: [26]; default: 0;
         *  write 1 to enable yuv2rgb frame done
         */
        uint32_t yuv2rgb_frame_int_ena:1;
        /** tail_idi_frame_int_ena : R/W; bitpos: [27]; default: 0;
         *  write 1 to enable isp_tail idi frame_end
         */
        uint32_t tail_idi_frame_int_ena:1;
        /** header_idi_frame_int_ena : R/W; bitpos: [28]; default: 0;
         *  write 1 to enable real input frame end of isp_input
         */
        uint32_t header_idi_frame_int_ena:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} isp_int_ena_reg_t;

/** Type of int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        /** isp_data_type_err_int_clr : WT; bitpos: [0]; default: 0;
         *  write 1 to clear input data type error
         */
        uint32_t isp_data_type_err_int_clr:1;
        /** isp_async_fifo_ovf_int_clr : WT; bitpos: [1]; default: 0;
         *  write 1 to clear isp input fifo overflow
         */
        uint32_t isp_async_fifo_ovf_int_clr:1;
        /** isp_buf_full_int_clr : WT; bitpos: [2]; default: 0;
         *  write 1 to clear isp input buffer full
         */
        uint32_t isp_buf_full_int_clr:1;
        /** isp_hvnum_setting_err_int_clr : WT; bitpos: [3]; default: 0;
         *  write 1 to clear hnum and vnum setting format error
         */
        uint32_t isp_hvnum_setting_err_int_clr:1;
        /** isp_data_type_setting_err_int_clr : WT; bitpos: [4]; default: 0;
         *  write 1 to clear setting invalid reg_data_type
         */
        uint32_t isp_data_type_setting_err_int_clr:1;
        /** isp_mipi_hnum_unmatch_int_clr : WT; bitpos: [5]; default: 0;
         *  write 1 to clear hnum setting unmatch with mipi input
         */
        uint32_t isp_mipi_hnum_unmatch_int_clr:1;
        /** dpc_check_done_int_clr : WT; bitpos: [6]; default: 0;
         *  write 1 to clear dpc check done
         */
        uint32_t dpc_check_done_int_clr:1;
        /** gamma_xcoord_err_int_clr : WT; bitpos: [7]; default: 0;
         *  write 1 to clear gamma setting error
         */
        uint32_t gamma_xcoord_err_int_clr:1;
        /** ae_monitor_int_clr : WT; bitpos: [8]; default: 0;
         *  write 1 to clear ae monitor
         */
        uint32_t ae_monitor_int_clr:1;
        /** ae_frame_done_int_clr : WT; bitpos: [9]; default: 0;
         *  write 1 to clear ae
         */
        uint32_t ae_frame_done_int_clr:1;
        /** af_fdone_int_clr : WT; bitpos: [10]; default: 0;
         *  write 1 to clear af statistic
         */
        uint32_t af_fdone_int_clr:1;
        /** af_env_int_clr : WT; bitpos: [11]; default: 0;
         *  write 1 to clear af monitor
         */
        uint32_t af_env_int_clr:1;
        /** awb_fdone_int_clr : WT; bitpos: [12]; default: 0;
         *  write 1 to clear awb
         */
        uint32_t awb_fdone_int_clr:1;
        /** hist_fdone_int_clr : WT; bitpos: [13]; default: 0;
         *  write 1 to clear histogram
         */
        uint32_t hist_fdone_int_clr:1;
        /** frame_int_clr : WT; bitpos: [14]; default: 0;
         *  write 1 to clear isp frame end
         */
        uint32_t frame_int_clr:1;
        /** blc_frame_int_clr : WT; bitpos: [15]; default: 0;
         *  write 1 to clear blc frame done
         */
        uint32_t blc_frame_int_clr:1;
        /** lsc_frame_int_clr : WT; bitpos: [16]; default: 0;
         *  write 1 to clear lsc frame done
         */
        uint32_t lsc_frame_int_clr:1;
        /** dpc_frame_int_clr : WT; bitpos: [17]; default: 0;
         *  write 1 to clear dpc frame done
         */
        uint32_t dpc_frame_int_clr:1;
        /** bf_frame_int_clr : WT; bitpos: [18]; default: 0;
         *  write 1 to clear bf frame done
         */
        uint32_t bf_frame_int_clr:1;
        /** demosaic_frame_int_clr : WT; bitpos: [19]; default: 0;
         *  write 1 to clear demosaic frame done
         */
        uint32_t demosaic_frame_int_clr:1;
        /** median_frame_int_clr : WT; bitpos: [20]; default: 0;
         *  write 1 to clear median frame done
         */
        uint32_t median_frame_int_clr:1;
        /** ccm_frame_int_clr : WT; bitpos: [21]; default: 0;
         *  write 1 to clear ccm frame done
         */
        uint32_t ccm_frame_int_clr:1;
        /** gamma_frame_int_clr : WT; bitpos: [22]; default: 0;
         *  write 1 to clear gamma frame done
         */
        uint32_t gamma_frame_int_clr:1;
        /** rgb2yuv_frame_int_clr : WT; bitpos: [23]; default: 0;
         *  write 1 to clear rgb2yuv frame done
         */
        uint32_t rgb2yuv_frame_int_clr:1;
        /** sharp_frame_int_clr : WT; bitpos: [24]; default: 0;
         *  write 1 to clear sharp frame done
         */
        uint32_t sharp_frame_int_clr:1;
        /** color_frame_int_clr : WT; bitpos: [25]; default: 0;
         *  write 1 to clear color frame done
         */
        uint32_t color_frame_int_clr:1;
        /** yuv2rgb_frame_int_clr : WT; bitpos: [26]; default: 0;
         *  write 1 to clear yuv2rgb frame done
         */
        uint32_t yuv2rgb_frame_int_clr:1;
        /** tail_idi_frame_int_clr : WT; bitpos: [27]; default: 0;
         *  write 1 to clear isp_tail idi frame_end
         */
        uint32_t tail_idi_frame_int_clr:1;
        /** header_idi_frame_int_clr : WT; bitpos: [28]; default: 0;
         *  write 1 to clear real input frame end of isp_input
         */
        uint32_t header_idi_frame_int_clr:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} isp_int_clr_reg_t;

typedef struct {
    volatile isp_gamma_y1_reg_t gamma_y1;
    volatile isp_gamma_y2_reg_t gamma_y2;
    volatile isp_gamma_y3_reg_t gamma_y3;
    volatile isp_gamma_y4_reg_t gamma_y4;
} isp_gamma_y_reg_t;

typedef struct {
    volatile isp_gamma_x1_reg_t gamma_x1;
    volatile isp_gamma_x2_reg_t gamma_x2;
} isp_gamma_x_reg_t;

typedef struct {
    volatile isp_ver_date_reg_t ver_date;
    volatile isp_clk_en_reg_t clk_en;
    volatile isp_cntl_reg_t cntl;
    volatile isp_hsync_cnt_reg_t hsync_cnt;
    volatile isp_frame_cfg_reg_t frame_cfg;
    volatile isp_ccm_coef0_reg_t ccm_coef0;
    volatile isp_ccm_coef1_reg_t ccm_coef1;
    volatile isp_ccm_coef3_reg_t ccm_coef3;
    volatile isp_ccm_coef4_reg_t ccm_coef4;
    volatile isp_ccm_coef5_reg_t ccm_coef5;
    volatile isp_bf_matrix_ctrl_reg_t bf_matrix_ctrl;
    volatile isp_bf_sigma_reg_t bf_sigma;
    volatile isp_bf_gau0_reg_t bf_gau0;
    volatile isp_bf_gau1_reg_t bf_gau1;
    volatile isp_dpc_ctrl_reg_t dpc_ctrl;
    volatile isp_dpc_conf_reg_t dpc_conf;
    volatile isp_dpc_matrix_ctrl_reg_t dpc_matrix_ctrl;
    volatile isp_dpc_deadpix_cnt_reg_t dpc_deadpix_cnt;
    volatile isp_lut_cmd_reg_t lut_cmd;
    volatile isp_lut_wdata_reg_t lut_wdata;
    volatile isp_lut_rdata_reg_t lut_rdata;
    volatile isp_lsc_tablesize_reg_t lsc_tablesize;
    volatile isp_demosaic_matrix_ctrl_reg_t demosaic_matrix_ctrl;
    volatile isp_demosaic_grad_ratio_reg_t demosaic_grad_ratio;
    volatile isp_median_matrix_ctrl_reg_t median_matrix_ctrl;
    volatile isp_int_raw_reg_t int_raw;
    volatile isp_int_st_reg_t int_st;
    volatile isp_int_ena_reg_t int_ena;
    volatile isp_int_clr_reg_t int_clr;
    volatile isp_gamma_ctrl_reg_t gamma_ctrl;
    volatile isp_gamma_y_reg_t gamma_rgb_y[3]; // r: gamma_rgb_y[0], g: gamma_rgb_y[1], b: gamma_rgb_y[2]
    volatile isp_gamma_x_reg_t gamma_rgb_x[3]; // r: gamma_rgb_x[0], g: gamma_rgb_x[1], b: gamma_rgb_x[2]
    volatile isp_ae_ctrl_reg_t ae_ctrl;
    volatile isp_ae_monitor_reg_t ae_monitor;
    volatile isp_ae_bx_reg_t ae_bx;
    volatile isp_ae_by_reg_t ae_by;
    volatile isp_ae_winpixnum_reg_t ae_winpixnum;
    volatile isp_ae_win_reciprocal_reg_t ae_win_reciprocal;
    volatile isp_ae_block_mean_reg_t ae_block_mean[7];
    volatile isp_sharp_ctrl0_reg_t sharp_ctrl0;
    volatile isp_sharp_filter_reg_t sharp_filter[3];
    volatile isp_sharp_matrix_ctrl_reg_t sharp_matrix_ctrl;
    volatile isp_sharp_ctrl1_reg_t sharp_ctrl1;
    volatile isp_dma_cntl_reg_t dma_cntl;
    volatile isp_dma_raw_data_reg_t dma_raw_data;
    volatile isp_cam_cntl_reg_t cam_cntl;
    volatile isp_cam_conf_reg_t cam_conf;
    volatile isp_af_ctrl0_reg_t af_ctrl0;
    volatile isp_af_ctrl1_reg_t af_ctrl1;
    volatile isp_af_gen_th_ctrl_reg_t af_gen_th_ctrl;
    volatile isp_af_env_user_th_sum_reg_t af_env_user_th_sum;
    volatile isp_af_env_user_th_lum_reg_t af_env_user_th_lum;
    volatile isp_af_threshold_reg_t af_threshold;
    volatile isp_af_hscale_a_reg_t af_hscale_a;
    volatile isp_af_vscale_a_reg_t af_vscale_a;
    volatile isp_af_hscale_b_reg_t af_hscale_b;
    volatile isp_af_vscale_b_reg_t af_vscale_b;
    volatile isp_af_hscale_c_reg_t af_hscale_c;
    volatile isp_af_vscale_c_reg_t af_vscale_c;
    volatile isp_af_sum_a_reg_t af_sum_a;
    volatile isp_af_sum_b_reg_t af_sum_b;
    volatile isp_af_sum_c_reg_t af_sum_c;
    volatile isp_af_lum_a_reg_t af_lum_a;
    volatile isp_af_lum_b_reg_t af_lum_b;
    volatile isp_af_lum_c_reg_t af_lum_c;
    volatile isp_awb_mode_reg_t awb_mode;
    volatile isp_awb_hscale_reg_t awb_hscale;
    volatile isp_awb_vscale_reg_t awb_vscale;
    volatile isp_awb_th_lum_reg_t awb_th_lum;
    volatile isp_awb_th_rg_reg_t awb_th_rg;
    volatile isp_awb_th_bg_reg_t awb_th_bg;
    volatile isp_awb0_white_cnt_reg_t awb0_white_cnt;
    volatile isp_awb0_acc_r_reg_t awb0_acc_r;
    volatile isp_awb0_acc_g_reg_t awb0_acc_g;
    volatile isp_awb0_acc_b_reg_t awb0_acc_b;
    volatile isp_color_ctrl_reg_t color_ctrl;
    volatile isp_blc_value_reg_t blc_value;
    volatile isp_blc_ctrl0_reg_t blc_ctrl0;
    volatile isp_blc_ctrl1_reg_t blc_ctrl1;
    volatile isp_blc_ctrl2_reg_t blc_ctrl2;
    volatile isp_blc_mean_reg_t blc_mean;
    volatile isp_hist_mode_reg_t hist_mode;
    volatile isp_hist_coeff_reg_t hist_coeff;
    volatile isp_hist_offs_reg_t hist_offs;
    volatile isp_hist_size_reg_t hist_size;
    volatile isp_hist_seg_reg_t hist_seg[4];
    volatile isp_hist_weight_reg_t hist_weight[7];
    volatile isp_hist_binn_reg_t hist_binn[16];
    volatile isp_mem_aux_ctrl_0_reg_t mem_aux_ctrl_0;
    volatile isp_mem_aux_ctrl_1_reg_t mem_aux_ctrl_1;
    volatile isp_mem_aux_ctrl_2_reg_t mem_aux_ctrl_2;
    volatile isp_mem_aux_ctrl_3_reg_t mem_aux_ctrl_3;
    volatile isp_mem_aux_ctrl_4_reg_t mem_aux_ctrl_4;
    volatile isp_yuv_format_reg_t yuv_format;
    volatile isp_rdn_eco_cs_reg_t rdn_eco_cs;
    volatile isp_rdn_eco_low_reg_t rdn_eco_low;
    volatile isp_rdn_eco_high_reg_t rdn_eco_high;
} isp_dev_t;

extern isp_dev_t ISP;

#ifndef __cplusplus
_Static_assert(sizeof(isp_dev_t) == 0x244, "Invalid size of isp_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
