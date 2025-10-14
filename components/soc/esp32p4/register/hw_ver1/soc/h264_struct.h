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

/** Group: Configuration Register */
/** Type of sys_ctrl register
 *  H264 system level control register.
 */
typedef union {
    struct {
        /** frame_start : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to start encoding one frame.\\0: Invalid. No effect\\1:
         *  Start encoding one frame
         */
        uint32_t frame_start:1;
        /** dma_move_start : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to start moving reference data from external mem.\\0:
         *  Invalid. No effect\\1: H264 start moving two MB lines of reference frame from
         *  external mem to internal mem
         */
        uint32_t dma_move_start:1;
        /** frame_mode : R/W; bitpos: [2]; default: 0;
         *  Configures H264 running mode. When field H264_DUAL_STREAM_MODE is set to 1, this
         *  field must be set to 1 too.\\0: GOP mode. Before every GOP first frame start, need
         *  reconfig reference frame DMA\\1: Frame mode. Before every frame start, need
         *  reconfig reference frame DMA
         */
        uint32_t frame_mode:1;
        /** sys_rst_pulse : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to reset H264 ip.\\0: Invalid. No effect\\1: Reset H264 ip
         */
        uint32_t sys_rst_pulse:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_sys_ctrl_reg_t;

/** Type of gop_conf register
 *  GOP related configuration register.
 */
typedef union {
    struct {
        /** dual_stream_mode : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable dual stream mode. When this field is set to 1,
         *  H264_FRAME_MODE field must be set to 1 too.\\0: Normal mode\\1: Dual stream mode
         */
        uint32_t dual_stream_mode:1;
        /** gop_num : R/W; bitpos: [8:1]; default: 0;
         *  Configures the frame number of one GOP.\\0: The frame number of one GOP is
         *  infinite\\Others: Actual frame number of one GOP
         */
        uint32_t gop_num:8;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} h264_gop_conf_reg_t;

/** Type of sys_mb_res register
 *  Video A horizontal and vertical MB resolution register.
 */
typedef union {
    struct {
        /** sys_total_mb_y : R/W; bitpos: [6:0]; default: 0;
         *  Configures video A vertical MB resolution.
         */
        uint32_t sys_total_mb_y:7;
        /** sys_total_mb_x : R/W; bitpos: [13:7]; default: 0;
         *  Configures video A horizontal MB resolution.
         */
        uint32_t sys_total_mb_x:7;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} h264_sys_mb_res_reg_t;

/** Type of sys_conf register
 *  Video A system level configuration register.
 */
typedef union {
    struct {
        /** db_tmp_ready_trigger_mb_num : R/W; bitpos: [6:0]; default: 3;
         *  Configures when to trigger  video A H264_DB_TMP_READY_INT. When the (MB number of
         *  written db temp+1) is greater than this filed in first MB line, trigger
         *  H264_DB_TMP_READY_INT. Min is 3.
         */
        uint32_t db_tmp_ready_trigger_mb_num:7;
        /** rec_ready_trigger_mb_lines : R/W; bitpos: [13:7]; default: 4;
         *  Configures when to trigger  video A H264_REC_READY_INT. When the MB line number of
         *  generated reconstruct pixel is greater than this filed, trigger H264_REC_READY_INT.
         *  Min is 4.
         */
        uint32_t rec_ready_trigger_mb_lines:7;
        /** intra_cost_cmp_offset : R/W; bitpos: [29:14]; default: 0;
         *  Configures video A intra cost offset when I MB compared with P MB.
         */
        uint32_t intra_cost_cmp_offset:16;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} h264_sys_conf_reg_t;

/** Type of deci_score register
 *  Video A luma and chroma MB decimate score Register.
 */
typedef union {
    struct {
        /** c_deci_score : R/W; bitpos: [9:0]; default: 0;
         *  Configures video A chroma MB decimate score. When chroma score is smaller than it,
         *  chroma decimate will be enable.
         */
        uint32_t c_deci_score:10;
        /** l_deci_score : R/W; bitpos: [19:10]; default: 0;
         *  Configures video A luma MB decimate score. When luma score is smaller than it, luma
         *  decimate will be enable.
         */
        uint32_t l_deci_score:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} h264_deci_score_reg_t;

/** Type of deci_score_offset register
 *  Video A luma and chroma MB decimate score offset Register.
 */
typedef union {
    struct {
        /** i16x16_deci_score_offset : R/W; bitpos: [5:0]; default: 0;
         *  Configures video A i16x16 MB decimate score offset. This offset will be added to
         *  i16x16 MB score.
         */
        uint32_t i16x16_deci_score_offset:6;
        /** i_chroma_deci_score_offset : R/W; bitpos: [11:6]; default: 0;
         *  Configures video A I chroma MB decimate score offset. This offset will be added to
         *  I chroma MB score.
         */
        uint32_t i_chroma_deci_score_offset:6;
        /** p16x16_deci_score_offset : R/W; bitpos: [17:12]; default: 0;
         *  Configures video A p16x16 MB decimate score offset. This offset will be added to
         *  p16x16 MB score.
         */
        uint32_t p16x16_deci_score_offset:6;
        /** p_chroma_deci_score_offset : R/W; bitpos: [23:18]; default: 0;
         *  Configures video A p chroma MB decimate score offset. This offset will be added to
         *  p chroma MB score.
         */
        uint32_t p_chroma_deci_score_offset:6;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_deci_score_offset_reg_t;

/** Type of rc_conf0 register
 *  Video A rate control configuration register0.
 */
typedef union {
    struct {
        /** qp : R/W; bitpos: [5:0]; default: 0;
         *  Configures video A frame level initial luma QP value.
         */
        uint32_t qp:6;
        /** rate_ctrl_u : R/W; bitpos: [21:6]; default: 0;
         *  Configures video A parameter U value. U = int((float) u << 8).
         */
        uint32_t rate_ctrl_u:16;
        /** mb_rate_ctrl_en : R/W; bitpos: [22]; default: 0;
         *  Configures video A whether or not to open macro block rate ctrl.\\1:Open the macro
         *  block rate ctrl\\1:Close the macro block rate ctrl.
         */
        uint32_t mb_rate_ctrl_en:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} h264_rc_conf0_reg_t;

/** Type of rc_conf1 register
 *  Video A rate control configuration register1.
 */
typedef union {
    struct {
        /** chroma_dc_qp_delta : R/W; bitpos: [2:0]; default: 0;
         *  Configures video A chroma DC QP offset based on Chroma QP. Chroma DC QP = Chroma
         *  QP(after map) + reg_chroma_dc_qp_delta.
         */
        uint32_t chroma_dc_qp_delta:3;
        /** chroma_qp_delta : R/W; bitpos: [6:3]; default: 0;
         *  Configures video A chroma QP offset based on luma QP. Chroma QP(before map) = Luma
         *  QP + reg_chroma_qp_delta.
         */
        uint32_t chroma_qp_delta:4;
        /** qp_min : R/W; bitpos: [12:7]; default: 0;
         *  Configures video A allowed luma QP min value.
         */
        uint32_t qp_min:6;
        /** qp_max : R/W; bitpos: [18:13]; default: 0;
         *  Configures video A allowed luma QP max value.
         */
        uint32_t qp_max:6;
        /** mad_frame_pred : R/W; bitpos: [30:19]; default: 0;
         *  Configures vdieo A frame level predicted MB MAD value.
         */
        uint32_t mad_frame_pred:12;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} h264_rc_conf1_reg_t;

/** Type of db_bypass register
 *  Video A Deblocking bypass register
 */
typedef union {
    struct {
        /** bypass_db_filter : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to bypass video A deblcoking filter. \\0: Open the
         *  deblock filter\\1: Close the deblock filter
         */
        uint32_t bypass_db_filter:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} h264_db_bypass_reg_t;

/** Type of roi_region register
 *  Video A H264 ROI region range configure register.
 */
typedef union {
    struct {
        /** roi_region_x : R/W; bitpos: [6:0]; default: 0;
         *  Configures the horizontal start macroblocks of region 0 in Video A.
         */
        uint32_t roi_region_x:7;
        /** roi_region_y : R/W; bitpos: [13:7]; default: 0;
         *  Configures the  vertical start macroblocks of region 0 in Video A.
         */
        uint32_t roi_region_y:7;
        /** roi_region_x_len : R/W; bitpos: [20:14]; default: 0;
         *  Configures the number of  macroblocks in horizontal direction of  the region 0 in
         *  Video A.
         */
        uint32_t roi_region_x_len:7;
        /** roi_region_y_len : R/W; bitpos: [27:21]; default: 0;
         *  Configures the number of  macroblocks in vertical direction of  the region 0 in
         *  Video A.
         */
        uint32_t roi_region_y_len:7;
        /** roi_region_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to open Video A ROI of region 0 .\\0:Close ROI\\1:Open
         *  ROI.
         */
        uint32_t roi_region_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} h264_roi_region_reg_t;

/** Type of roi_region0_3_qp register
 *  Video A H264 ROI region0, region1,region2,region3 QP register.
 */
typedef union {
    struct {
        /** roi_region0_qp : R/W; bitpos: [6:0]; default: 0;
         *  Configure H264 ROI region0 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region0_qp:7;
        /** roi_region1_qp : R/W; bitpos: [13:7]; default: 0;
         *  Configure H264 ROI region1 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region1_qp:7;
        /** roi_region2_qp : R/W; bitpos: [20:14]; default: 0;
         *  Configure H264 ROI region2 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region2_qp:7;
        /** roi_region3_qp : R/W; bitpos: [27:21]; default: 0;
         *  Configure H264 ROI region3 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region3_qp:7;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} h264_roi_region0_3_qp_reg_t;

/** Type of roi_region4_7_qp register
 *  Video A H264 ROI region4, region5,region6,region7 QP register.
 */
typedef union {
    struct {
        /** roi_region4_qp : R/W; bitpos: [6:0]; default: 0;
         *  Configure H264 ROI region4 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region4_qp:7;
        /** roi_region5_qp : R/W; bitpos: [13:7]; default: 0;
         *  Configure H264 ROI region5 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region5_qp:7;
        /** roi_region6_qp : R/W; bitpos: [20:14]; default: 0;
         *  Configure H264 ROI region6 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region6_qp:7;
        /** roi_region7_qp : R/W; bitpos: [27:21]; default: 0;
         *  Configure H264 ROI region7 qp in video A,fixed qp or delta qp.
         */
        uint32_t roi_region7_qp:7;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} h264_roi_region4_7_qp_reg_t;

/** Type of no_roi_region_qp_offset register
 *  Video A H264 no roi region QP register.
 */
typedef union {
    struct {
        /** no_roi_region_qp : R/W; bitpos: [6:0]; default: 0;
         *  Configure H264 no region qp in video A, delta qp.
         */
        uint32_t no_roi_region_qp:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} h264_no_roi_region_qp_offset_reg_t;

/** Type of roi_config register
 *  Video A H264 ROI configure register.
 */
typedef union {
    struct {
        /** roi_en : R/W; bitpos: [0]; default: 0;
         *  Configure whether or not to enable ROI in video A.\\0:not enable ROI\\1:enable ROI.
         */
        uint32_t roi_en:1;
        /** roi_mode : R/W; bitpos: [1]; default: 0;
         *  Configure the mode of ROI in video A.\\0:fixed qp\\1:delta qp.
         */
        uint32_t roi_mode:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} h264_roi_config_reg_t;

/** Type of slice_header_remain register
 *  Frame Slice Header remain bit register.
 */
typedef union {
    struct {
        /** slice_remain_bitlength : R/W; bitpos: [2:0]; default: 0;
         *  Configures Slice Header remain bit number
         */
        uint32_t slice_remain_bitlength:3;
        /** slice_remain_bit : R/W; bitpos: [10:3]; default: 0;
         *  Configures Slice Header remain bit
         */
        uint32_t slice_remain_bit:8;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_slice_header_remain_reg_t;

/** Type of slice_header_byte_length register
 *  Frame Slice Header byte length register.
 */
typedef union {
    struct {
        /** slice_byte_length : R/W; bitpos: [3:0]; default: 0;
         *  Configures Slice Header byte number
         */
        uint32_t slice_byte_length:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_slice_header_byte_length_reg_t;

/** Type of bs_threshold register
 *  Bitstream buffer overflow threshold register
 */
typedef union {
    struct {
        /** bs_buffer_threshold : R/W; bitpos: [6:0]; default: 48;
         *  Configures bitstream buffer overflow threshold. This value should be bigger than
         *  the encode bytes of one 4x4 submb.
         */
        uint32_t bs_buffer_threshold:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} h264_bs_threshold_reg_t;

/** Type of slice_header_byte register
 *  Frame Slice Header byte 32 bit register.
 */
typedef union {
    struct {
        /** byte : R/W; bitpos: [31:0]; default: 0;
         *  Configures Slice Header 32 bit
         */
        uint32_t byte:32;
    };
    uint32_t val;
} h264_slice_header_reg_t;

/** Type of conf register
 *  General configuration register.
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to open register clock gate.\\0: Open the clock gate only
         *  when application writes registers\\1: Force open the clock gate for register
         */
        uint32_t clk_en:1;
        /** rec_ram_clk_en2 : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to open the clock gate for rec ram2.\\0: Open the clock
         *  gate only when application writes or reads rec ram2\\1: Force open the clock gate
         *  for rec ram2
         */
        uint32_t rec_ram_clk_en2:1;
        /** rec_ram_clk_en1 : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to open the clock gate for rec ram1.\\0: Open the clock
         *  gate only when application writes or reads rec ram1\\1: Force open the clock gate
         *  for rec ram1
         */
        uint32_t rec_ram_clk_en1:1;
        /** quant_ram_clk_en2 : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to open the clock gate for quant ram2.\\0: Open the clock
         *  gate only when application writes or reads quant ram2\\1: Force open the clock gate
         *  for quant ram2
         */
        uint32_t quant_ram_clk_en2:1;
        /** quant_ram_clk_en1 : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to open the clock gate for quant ram1.\\0: Open the clock
         *  gate only when application writes or reads quant ram1\\1: Force open the clock gate
         *  for quant ram1
         */
        uint32_t quant_ram_clk_en1:1;
        /** pre_ram_clk_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to open the clock gate for pre ram.\\0: Open the clock
         *  gate only when application writes or reads pre ram\\1: Force open the clock gate
         *  for pre ram
         */
        uint32_t pre_ram_clk_en:1;
        /** mvd_ram_clk_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to open the clock gate for mvd ram.\\0: Open the clock
         *  gate only when application writes or reads mvd ram\\1: Force open the clock gate
         *  for mvd ram
         */
        uint32_t mvd_ram_clk_en:1;
        /** mc_ram_clk_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to open the clock gate for mc ram.\\0: Open the clock
         *  gate only when application writes or reads mc ram\\1: Force open the clock gate for
         *  mc ram
         */
        uint32_t mc_ram_clk_en:1;
        /** ref_ram_clk_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to open the clock gate for ref ram.\\0: Open the clock
         *  gate only when application writes or reads ref ram\\1: Force open the clock gate
         *  for ref ram
         */
        uint32_t ref_ram_clk_en:1;
        /** i4x4_ref_ram_clk_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to open the clock gate for i4x4_mode ram.\\0: Open the
         *  clock gate only when application writes or reads i4x4_mode ram\\1: Force open the
         *  clock gate for i4x4_mode ram
         */
        uint32_t i4x4_ref_ram_clk_en:1;
        /** ime_ram_clk_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to open the clock gate for ime ram.\\0: Open the clock
         *  gate only when application writes or reads ime ram\\1: Force open the clock gate
         *  for ime ram
         */
        uint32_t ime_ram_clk_en:1;
        /** fme_ram_clk_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to open the clock gate for fme ram.\\0: Open the clock
         *  gate only when application writes or readsfme ram\\1: Force open the clock gate for
         *  fme ram
         */
        uint32_t fme_ram_clk_en:1;
        /** fetch_ram_clk_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to open the clock gate for fetch ram.\\0: Open the clock
         *  gate only when application writes or reads fetch ram\\1: Force open the clock gate
         *  for fetch ram
         */
        uint32_t fetch_ram_clk_en:1;
        /** db_ram_clk_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to open the clock gate for db ram.\\0: Open the clock
         *  gate only when application writes or reads db ram\\1: Force open the clock gate for
         *  db ram
         */
        uint32_t db_ram_clk_en:1;
        /** cur_mb_ram_clk_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to open the clock gate for cur_mb ram.\\0: Open the clock
         *  gate only when application writes or reads cur_mb ram\\1: Force open the clock gate
         *  for cur_mb ram
         */
        uint32_t cur_mb_ram_clk_en:1;
        /** cavlc_ram_clk_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to open the clock gate for cavlc ram.\\0: Open the clock
         *  gate only when application writes or reads cavlc ram\\1: Force open the clock gate
         *  for cavlc ram
         */
        uint32_t cavlc_ram_clk_en:1;
        /** ime_clk_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to open the clock gate for ime.\\0: Open the clock gate
         *  only when ime work\\1: Force open the clock gate for ime
         */
        uint32_t ime_clk_en:1;
        /** fme_clk_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to open the clock gate for fme.\\0: Open the clock gate
         *  only when fme work\\1: Force open the clock gate for fme
         */
        uint32_t fme_clk_en:1;
        /** mc_clk_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to open the clock gate for mc.\\0: Open the clock gate
         *  only when mc work\\1: Force open the clock gate for mc
         */
        uint32_t mc_clk_en:1;
        /** interpolator_clk_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to open the clock gate for interpolator.\\0: Open the
         *  clock gate only when interpolator work\\1: Force open the clock gate for
         *  interpolator
         */
        uint32_t interpolator_clk_en:1;
        /** db_clk_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to open the clock gate for deblocking filter.\\0: Open
         *  the clock gate only when deblocking filter work\\1: Force open the clock gate for
         *  deblocking filter
         */
        uint32_t db_clk_en:1;
        /** clavlc_clk_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to open the clock gate for cavlc.\\0: Open the clock gate
         *  only when cavlc work\\1: Force open the clock gate for cavlc
         */
        uint32_t clavlc_clk_en:1;
        /** intra_clk_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to open the clock gate for intra.\\0: Open the clock gate
         *  only when intra work\\1: Force open the clock gate for intra
         */
        uint32_t intra_clk_en:1;
        /** deci_clk_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to open the clock gate for decimate.\\0: Open the clock
         *  gate only when decimate work\\1: Force open the clock gate for decimate
         */
        uint32_t deci_clk_en:1;
        /** bs_clk_en : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to open the clock gate for bs buffer.\\0: Open the clock
         *  gate only when bs buffer work\\1: Force open the clock gate for bs buffer
         */
        uint32_t bs_clk_en:1;
        /** mv_merge_clk_en : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not to open the clock gate for mv merge.\\0: Open the clock
         *  gate only when mv merge work\\1: Force open the clock gate for mv merge
         */
        uint32_t mv_merge_clk_en:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} h264_conf_reg_t;

/** Type of mv_merge_config register
 *  Mv merge configuration register.
 */
typedef union {
    struct {
        /** mv_merge_type : R/W; bitpos: [1:0]; default: 0;
         *  Configure mv merge type.\\0: merge p16x16 mv\\1: merge min mv\\2: merge max mv\\3:
         *  not valid.
         */
        uint32_t mv_merge_type:2;
        /** int_mv_out_en : R/W; bitpos: [2]; default: 0;
         *  Configure mv merge output integer part not zero mv or all part not zero mv.\\0:
         *  output all part not zero mv\\1: output integer part not zero mv.
         */
        uint32_t int_mv_out_en:1;
        /** a_mv_merge_en : R/W; bitpos: [3]; default: 0;
         *  Configure whether or not to enable video A mv merge.\\0: disable\\1: enable.
         */
        uint32_t a_mv_merge_en:1;
        /** b_mv_merge_en : R/W; bitpos: [4]; default: 0;
         *  Configure whether or not to enable video B mv merge.\\0: disable\\1: enable.
         */
        uint32_t b_mv_merge_en:1;
        /** mb_valid_num : RO; bitpos: [17:5]; default: 0;
         *  Represents the valid mb number of mv merge output.
         */
        uint32_t mb_valid_num:13;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} h264_mv_merge_config_reg_t;

/** Type of debug_dma_sel register
 *  Debug H264 DMA select register
 */
typedef union {
    struct {
        /** dbg_dma_sel : R/W; bitpos: [7:0]; default: 0;
         *  Every bit represents a dma in h264
         */
        uint32_t dbg_dma_sel:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} h264_debug_dma_sel_reg_t;


/** Group: Status Register */
/** Type of rc_status0 register
 *  Rate control status register0.
 */
typedef union {
    struct {
        /** frame_mad_sum : RO; bitpos: [20:0]; default: 0;
         *  Represents all MB actual MAD sum value of one frame.
         */
        uint32_t frame_mad_sum:21;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} h264_rc_status0_reg_t;

/** Type of rc_status1 register
 *  Rate control status register1.
 */
typedef union {
    struct {
        /** frame_enc_bits : RO; bitpos: [26:0]; default: 0;
         *  Represents all MB actual encoding bits sum value of one frame.
         */
        uint32_t frame_enc_bits:27;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} h264_rc_status1_reg_t;

/** Type of rc_status2 register
 *  Rate control status register2.
 */
typedef union {
    struct {
        /** frame_qp_sum : RO; bitpos: [18:0]; default: 0;
         *  Represents all MB actual luma QP sum value of one frame.
         */
        uint32_t frame_qp_sum:19;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} h264_rc_status2_reg_t;

/** Type of sys_status register
 *  System status register.
 */
typedef union {
    struct {
        /** frame_num : RO; bitpos: [8:0]; default: 0;
         *  Represents current frame number.
         */
        uint32_t frame_num:9;
        /** dual_stream_sel : RO; bitpos: [9]; default: 0;
         *  Represents which register group is used for cur frame.\\0: Register group A is
         *  used\\1: Register group B is used.
         */
        uint32_t dual_stream_sel:1;
        /** intra_flag : RO; bitpos: [10]; default: 0;
         *  Represents the type of current encoding frame.\\0: P frame\\1: I frame.
         */
        uint32_t intra_flag:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} h264_sys_status_reg_t;

/** Type of frame_code_length register
 *  Frame code byte length register.
 */
typedef union {
    struct {
        /** frame_code_length : RO; bitpos: [23:0]; default: 0;
         *  Represents current frame code byte length.
         */
        uint32_t frame_code_length:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} h264_frame_code_length_reg_t;

/** Type of debug_info0 register
 *  Debug information register0.
 */
typedef union {
    struct {
        /** top_ctrl_inter_debug_state : RO; bitpos: [3:0]; default: 0;
         *  Represents top_ctrl_inter module FSM info.
         */
        uint32_t top_ctrl_inter_debug_state:4;
        /** top_ctrl_intra_debug_state : RO; bitpos: [6:4]; default: 0;
         *  Represents top_ctrl_intra module FSM info.
         */
        uint32_t top_ctrl_intra_debug_state:3;
        /** p_i_cmp_debug_state : RO; bitpos: [9:7]; default: 0;
         *  Represents p_i_cmp module FSM info.
         */
        uint32_t p_i_cmp_debug_state:3;
        /** mvd_debug_state : RO; bitpos: [12:10]; default: 0;
         *  Represents mvd module FSM info.
         */
        uint32_t mvd_debug_state:3;
        /** mc_chroma_ip_debug_state : RO; bitpos: [13]; default: 0;
         *  Represents mc_chroma_ip module FSM info.
         */
        uint32_t mc_chroma_ip_debug_state:1;
        /** intra_16x16_chroma_ctrl_debug_state : RO; bitpos: [17:14]; default: 0;
         *  Represents intra_16x16_chroma_ctrl module FSM info.
         */
        uint32_t intra_16x16_chroma_ctrl_debug_state:4;
        /** intra_4x4_ctrl_debug_state : RO; bitpos: [21:18]; default: 0;
         *  Represents intra_4x4_ctrl module FSM info.
         */
        uint32_t intra_4x4_ctrl_debug_state:4;
        /** intra_top_ctrl_debug_state : RO; bitpos: [24:22]; default: 0;
         *  Represents intra_top_ctrl module FSM info.
         */
        uint32_t intra_top_ctrl_debug_state:3;
        /** ime_ctrl_debug_state : RO; bitpos: [27:25]; default: 0;
         *  Represents ime_ctrl module FSM info.
         */
        uint32_t ime_ctrl_debug_state:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} h264_debug_info0_reg_t;

/** Type of debug_info1 register
 *  Debug information register1.
 */
typedef union {
    struct {
        /** fme_ctrl_debug_state : RO; bitpos: [2:0]; default: 0;
         *  Represents fme_ctrl module FSM info.
         */
        uint32_t fme_ctrl_debug_state:3;
        /** deci_calc_debug_state : RO; bitpos: [4:3]; default: 0;
         *  Represents deci_calc module's FSM info. DEV use only.
         */
        uint32_t deci_calc_debug_state:2;
        /** db_debug_state : RO; bitpos: [7:5]; default: 0;
         *  Represents db module FSM info.
         */
        uint32_t db_debug_state:3;
        /** cavlc_enc_debug_state : RO; bitpos: [11:8]; default: 0;
         *  Represents cavlc module enc FSM info.
         */
        uint32_t cavlc_enc_debug_state:4;
        /** cavlc_scan_debug_state : RO; bitpos: [15:12]; default: 0;
         *  Represents cavlc module scan FSM info.
         */
        uint32_t cavlc_scan_debug_state:4;
        /** cavlc_ctrl_debug_state : RO; bitpos: [17:16]; default: 0;
         *  Represents cavlc module ctrl FSM info.
         */
        uint32_t cavlc_ctrl_debug_state:2;
        /** bs_buffer_debug_state : RO; bitpos: [18]; default: 0;
         *  Represents bs buffer overflow info.
         */
        uint32_t bs_buffer_debug_state:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} h264_debug_info1_reg_t;

/** Type of debug_info2 register
 *  Debug information register2.
 */
typedef union {
    struct {
        /** p_rc_done_debug_flag : RO; bitpos: [0]; default: 0;
         *  Represents p rate ctrl done status.\\0: not done\\1: done.
         */
        uint32_t p_rc_done_debug_flag:1;
        /** p_p_i_cmp_done_debug_flag : RO; bitpos: [1]; default: 0;
         *  Represents p p_i_cmp done status.\\0: not done\\1: done.
         */
        uint32_t p_p_i_cmp_done_debug_flag:1;
        /** p_mv_merge_done_debug_flag : RO; bitpos: [2]; default: 0;
         *  Represents p mv merge done status.\\0: not done\\1: done.
         */
        uint32_t p_mv_merge_done_debug_flag:1;
        /** p_move_ori_done_debug_flag : RO; bitpos: [3]; default: 0;
         *  Represents p move origin done status.\\0: not done\\1: done.
         */
        uint32_t p_move_ori_done_debug_flag:1;
        /** p_mc_done_debug_flag : RO; bitpos: [4]; default: 0;
         *  Represents p mc done status.\\0: not done\\1: done.
         */
        uint32_t p_mc_done_debug_flag:1;
        /** p_ime_done_debug_flag : RO; bitpos: [5]; default: 0;
         *  Represents p ime done status.\\0: not done\\1: done.
         */
        uint32_t p_ime_done_debug_flag:1;
        /** p_get_ori_done_debug_flag : RO; bitpos: [6]; default: 0;
         *  Represents p get origin done status.\\0: not done\\1: done.
         */
        uint32_t p_get_ori_done_debug_flag:1;
        /** p_fme_done_debug_flag : RO; bitpos: [7]; default: 0;
         *  Represents p fme done status.\\0: not done\\1: done.
         */
        uint32_t p_fme_done_debug_flag:1;
        /** p_fetch_done_debug_flag : RO; bitpos: [8]; default: 0;
         *  Represents p fetch done status.\\0: not done\\1: done.
         */
        uint32_t p_fetch_done_debug_flag:1;
        /** p_db_done_debug_flag : RO; bitpos: [9]; default: 0;
         *  Represents p deblocking done status.\\0: not done\\1: done.
         */
        uint32_t p_db_done_debug_flag:1;
        /** p_bs_buf_done_debug_flag : RO; bitpos: [10]; default: 0;
         *  Represents p bitstream buffer done status.\\0: not done\\1: done.
         */
        uint32_t p_bs_buf_done_debug_flag:1;
        /** ref_move_2mb_line_done_debug_flag : RO; bitpos: [11]; default: 0;
         *  Represents dma move 2 ref mb line done status.\\0: not done\\1: done.
         */
        uint32_t ref_move_2mb_line_done_debug_flag:1;
        /** i_p_i_cmp_done_debug_flag : RO; bitpos: [12]; default: 0;
         *  Represents I p_i_cmp done status.\\0: not done\\1: done.
         */
        uint32_t i_p_i_cmp_done_debug_flag:1;
        /** i_move_ori_done_debug_flag : RO; bitpos: [13]; default: 0;
         *  Represents I move origin done status.\\0: not done\\1: done.
         */
        uint32_t i_move_ori_done_debug_flag:1;
        /** i_get_ori_done_debug_flag : RO; bitpos: [14]; default: 0;
         *  Represents I get origin done status.\\0: not done\\1: done.
         */
        uint32_t i_get_ori_done_debug_flag:1;
        /** i_ec_done_debug_flag : RO; bitpos: [15]; default: 0;
         *  Represents I encoder done status.\\0: not done\\1: done.
         */
        uint32_t i_ec_done_debug_flag:1;
        /** i_db_done_debug_flag : RO; bitpos: [16]; default: 0;
         *  Represents I deblocking done status.\\0: not done\\1: done.
         */
        uint32_t i_db_done_debug_flag:1;
        /** i_bs_buf_done_debug_flag : RO; bitpos: [17]; default: 0;
         *  Represents I bitstream buffer done status.\\0: not done\\1: done.
         */
        uint32_t i_bs_buf_done_debug_flag:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} h264_debug_info2_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  Interrupt raw status register
 */
typedef union {
    struct {
        /** db_tmp_ready_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  Raw status bit: The raw interrupt status of  H264_DB_TMP_READY_INT. Triggered when
         *  H264 written enough db tmp pixel.
         */
        uint32_t db_tmp_ready_int_raw:1;
        /** rec_ready_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  Raw status bit: The raw interrupt status of  H264_REC_READY_INT. Triggered when
         *  H264 encoding enough reconstruct pixel.
         */
        uint32_t rec_ready_int_raw:1;
        /** frame_done_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  Raw status bit: The raw interrupt status of  H264_FRAME_DONE_INT. Triggered when
         *  H264 encoding one frame done.
         */
        uint32_t frame_done_int_raw:1;
        /** dma_move_2mb_line_done_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  Raw status bit: The raw interrupt status of H264_DMA_MOVE_2MB_LINE_DONE_INT.
         *  Triggered when H264 move two MB lines of reference frame from external mem to
         *  internal mem done.
         */
        uint32_t dma_move_2mb_line_done_int_raw:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_int_raw_reg_t;

/** Type of int_st register
 *  Interrupt masked status register
 */
typedef union {
    struct {
        /** db_tmp_ready_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of H264_DB_TMP_READY_INT. Valid only when the
         *  H264_DB_TMP_READY_INT_ENA is set to 1.
         */
        uint32_t db_tmp_ready_int_st:1;
        /** rec_ready_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of H264_REC_READY_INT. Valid only when the
         *  H264_REC_READY_INT_ENA is set to 1.
         */
        uint32_t rec_ready_int_st:1;
        /** frame_done_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of H264_FRAME_DONE_INT. Valid only when the
         *  H264_FRAME_DONE_INT_ENA is set to 1.
         */
        uint32_t frame_done_int_st:1;
        /** dma_move_2mb_line_done_int_st : RO; bitpos: [3]; default: 0;
         *  Masked status bit: The masked interrupt status of H264_DMA_MOVE_2MB_LINE_DONE_INT.
         *  Valid only when the H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA is set to 1.
         */
        uint32_t dma_move_2mb_line_done_int_st:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable register
 */
typedef union {
    struct {
        /** db_tmp_ready_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable H264_DB_TMP_READY_INT.
         */
        uint32_t db_tmp_ready_int_ena:1;
        /** rec_ready_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable H264_REC_READY_INT.
         */
        uint32_t rec_ready_int_ena:1;
        /** frame_done_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable H264_FRAME_DONE_INT.
         */
        uint32_t frame_done_int_ena:1;
        /** dma_move_2mb_line_done_int_ena : R/W; bitpos: [3]; default: 0;
         *  Enable bit: Write 1 to enable H264_DMA_MOVE_2MB_LINE_DONE_INT.
         */
        uint32_t dma_move_2mb_line_done_int_ena:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear register
 */
typedef union {
    struct {
        /** db_tmp_ready_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear H264_DB_TMP_READY_INT.
         */
        uint32_t db_tmp_ready_int_clr:1;
        /** rec_ready_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear H264_REC_READY_INT.
         */
        uint32_t rec_ready_int_clr:1;
        /** frame_done_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear H264_FRAME_DONE_INT.
         */
        uint32_t frame_done_int_clr:1;
        /** dma_move_2mb_line_done_int_clr : WT; bitpos: [3]; default: 0;
         *  Clear bit: Write 1 to clear H264_DMA_MOVE_2MB_LINE_DONE_INT.
         */
        uint32_t dma_move_2mb_line_done_int_clr:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} h264_int_clr_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** ledc_date : R/W; bitpos: [27:0]; default: 36717120;
         *  Configures the version.
         */
        uint32_t ledc_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} h264_date_reg_t;


typedef struct {
    volatile h264_sys_mb_res_reg_t sys_mb_res;
    volatile h264_sys_conf_reg_t sys_conf;
    volatile h264_deci_score_reg_t deci_score;
    volatile h264_deci_score_offset_reg_t deci_score_offset;
    volatile h264_rc_conf0_reg_t rc_conf0;
    volatile h264_rc_conf1_reg_t rc_conf1;
    volatile h264_db_bypass_reg_t db_bypass;
    volatile h264_roi_region_reg_t roi_region[8];
    volatile h264_roi_region0_3_qp_reg_t roi_region0_3_qp;
    volatile h264_roi_region4_7_qp_reg_t roi_region4_7_qp;
    volatile h264_no_roi_region_qp_offset_reg_t no_roi_region_qp_offset;
    volatile h264_roi_config_reg_t roi_config;
} h264_ctrl_regs_t;

typedef struct {
    volatile h264_sys_ctrl_reg_t sys_ctrl;
    volatile h264_gop_conf_reg_t gop_conf;
    volatile h264_ctrl_regs_t ctrl[2];
    volatile h264_rc_status0_reg_t rc_status0;
    volatile h264_rc_status1_reg_t rc_status1;
    volatile h264_rc_status2_reg_t rc_status2;
    volatile h264_slice_header_remain_reg_t slice_header_remain;
    volatile h264_slice_header_byte_length_reg_t slice_header_byte_length;
    volatile h264_bs_threshold_reg_t bs_threshold;
    volatile h264_slice_header_reg_t slice_header[2];
    volatile h264_int_raw_reg_t int_raw;
    volatile h264_int_st_reg_t int_st;
    volatile h264_int_ena_reg_t int_ena;
    volatile h264_int_clr_reg_t int_clr;
    volatile h264_conf_reg_t conf;
    volatile h264_mv_merge_config_reg_t mv_merge_config;
    volatile h264_debug_dma_sel_reg_t debug_dma_sel;
    volatile h264_sys_status_reg_t sys_status;
    volatile h264_frame_code_length_reg_t frame_code_length;
    volatile h264_debug_info0_reg_t debug_info0;
    volatile h264_debug_info1_reg_t debug_info1;
    volatile h264_debug_info2_reg_t debug_info2;
    volatile h264_date_reg_t date;
} h264_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(h264_dev_t) == 0xf4, "Invalid size of h264_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
