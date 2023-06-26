/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** H264_SYS_CTRL_REG register
 *  H264 system level control register.
 */
#define H264_SYS_CTRL_REG (DR_REG_H264_BASE + 0x0)
/** H264_FRAME_START : WT; bitpos: [0]; default: 0;
 *  Configures whether or not to start encoding one frame.\\0: Invalid. No effect\\1:
 *  Start encoding one frame
 */
#define H264_FRAME_START    (BIT(0))
#define H264_FRAME_START_M  (H264_FRAME_START_V << H264_FRAME_START_S)
#define H264_FRAME_START_V  0x00000001U
#define H264_FRAME_START_S  0
/** H264_DMA_MOVE_START : WT; bitpos: [1]; default: 0;
 *  Configures whether or not to start moving reference data from external mem.\\0:
 *  Invalid. No effect\\1: H264 start moving two MB lines of reference frame from
 *  external mem to internal mem
 */
#define H264_DMA_MOVE_START    (BIT(1))
#define H264_DMA_MOVE_START_M  (H264_DMA_MOVE_START_V << H264_DMA_MOVE_START_S)
#define H264_DMA_MOVE_START_V  0x00000001U
#define H264_DMA_MOVE_START_S  1
/** H264_FRAME_MODE : R/W; bitpos: [2]; default: 0;
 *  Configures H264 running mode. When field H264_DUAL_STREAM_MODE is set to 1, this
 *  field must be set to 1 too.\\0: GOP mode. Before every GOP first frame start, need
 *  reconfig reference frame DMA\\1: Frame mode. Before every frame start, need
 *  reconfig reference frame DMA
 */
#define H264_FRAME_MODE    (BIT(2))
#define H264_FRAME_MODE_M  (H264_FRAME_MODE_V << H264_FRAME_MODE_S)
#define H264_FRAME_MODE_V  0x00000001U
#define H264_FRAME_MODE_S  2
/** H264_SYS_RST_PULSE : WT; bitpos: [3]; default: 0;
 *  Configures whether or not to reset H264 ip.\\0: Invalid. No effect\\1: Reset H264 ip
 */
#define H264_SYS_RST_PULSE    (BIT(3))
#define H264_SYS_RST_PULSE_M  (H264_SYS_RST_PULSE_V << H264_SYS_RST_PULSE_S)
#define H264_SYS_RST_PULSE_V  0x00000001U
#define H264_SYS_RST_PULSE_S  3

/** H264_GOP_CONF_REG register
 *  GOP related configuration register.
 */
#define H264_GOP_CONF_REG (DR_REG_H264_BASE + 0x4)
/** H264_DUAL_STREAM_MODE : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable dual stream mode. When this field is set to 1,
 *  H264_FRAME_MODE field must be set to 1 too.\\0: Normal mode\\1: Dual stream mode
 */
#define H264_DUAL_STREAM_MODE    (BIT(0))
#define H264_DUAL_STREAM_MODE_M  (H264_DUAL_STREAM_MODE_V << H264_DUAL_STREAM_MODE_S)
#define H264_DUAL_STREAM_MODE_V  0x00000001U
#define H264_DUAL_STREAM_MODE_S  0
/** H264_GOP_NUM : R/W; bitpos: [8:1]; default: 0;
 *  Configures the frame number of one GOP.\\0: The frame number of one GOP is
 *  infinite\\Others: Actual frame number of one GOP
 */
#define H264_GOP_NUM    0x000000FFU
#define H264_GOP_NUM_M  (H264_GOP_NUM_V << H264_GOP_NUM_S)
#define H264_GOP_NUM_V  0x000000FFU
#define H264_GOP_NUM_S  1

/** H264_A_SYS_MB_RES_REG register
 *  Video A horizontal and vertical MB resolution register.
 */
#define H264_A_SYS_MB_RES_REG (DR_REG_H264_BASE + 0x8)
/** H264_A_SYS_TOTAL_MB_Y : R/W; bitpos: [6:0]; default: 0;
 *  Configures video A vertical MB resolution.
 */
#define H264_A_SYS_TOTAL_MB_Y    0x0000007FU
#define H264_A_SYS_TOTAL_MB_Y_M  (H264_A_SYS_TOTAL_MB_Y_V << H264_A_SYS_TOTAL_MB_Y_S)
#define H264_A_SYS_TOTAL_MB_Y_V  0x0000007FU
#define H264_A_SYS_TOTAL_MB_Y_S  0
/** H264_A_SYS_TOTAL_MB_X : R/W; bitpos: [13:7]; default: 0;
 *  Configures video A horizontal MB resolution.
 */
#define H264_A_SYS_TOTAL_MB_X    0x0000007FU
#define H264_A_SYS_TOTAL_MB_X_M  (H264_A_SYS_TOTAL_MB_X_V << H264_A_SYS_TOTAL_MB_X_S)
#define H264_A_SYS_TOTAL_MB_X_V  0x0000007FU
#define H264_A_SYS_TOTAL_MB_X_S  7

/** H264_A_SYS_CONF_REG register
 *  Video A system level configuration register.
 */
#define H264_A_SYS_CONF_REG (DR_REG_H264_BASE + 0xc)
/** H264_A_DB_TMP_READY_TRIGGER_MB_NUM : R/W; bitpos: [6:0]; default: 3;
 *  Configures when to trigger  video A H264_DB_TMP_READY_INT. When the (MB number of
 *  written db temp+1) is greater than this filed in first MB line, trigger
 *  H264_DB_TMP_READY_INT. Min is 3.
 */
#define H264_A_DB_TMP_READY_TRIGGER_MB_NUM    0x0000007FU
#define H264_A_DB_TMP_READY_TRIGGER_MB_NUM_M  (H264_A_DB_TMP_READY_TRIGGER_MB_NUM_V << H264_A_DB_TMP_READY_TRIGGER_MB_NUM_S)
#define H264_A_DB_TMP_READY_TRIGGER_MB_NUM_V  0x0000007FU
#define H264_A_DB_TMP_READY_TRIGGER_MB_NUM_S  0
/** H264_A_REC_READY_TRIGGER_MB_LINES : R/W; bitpos: [13:7]; default: 4;
 *  Configures when to trigger  video A H264_REC_READY_INT. When the MB line number of
 *  generated reconstruct pixel is greater than this filed, trigger H264_REC_READY_INT.
 *  Min is 4.
 */
#define H264_A_REC_READY_TRIGGER_MB_LINES    0x0000007FU
#define H264_A_REC_READY_TRIGGER_MB_LINES_M  (H264_A_REC_READY_TRIGGER_MB_LINES_V << H264_A_REC_READY_TRIGGER_MB_LINES_S)
#define H264_A_REC_READY_TRIGGER_MB_LINES_V  0x0000007FU
#define H264_A_REC_READY_TRIGGER_MB_LINES_S  7
/** H264_A_INTRA_COST_CMP_OFFSET : R/W; bitpos: [29:14]; default: 0;
 *  Configures video A intra cost offset when I MB compared with P MB.
 */
#define H264_A_INTRA_COST_CMP_OFFSET    0x0000FFFFU
#define H264_A_INTRA_COST_CMP_OFFSET_M  (H264_A_INTRA_COST_CMP_OFFSET_V << H264_A_INTRA_COST_CMP_OFFSET_S)
#define H264_A_INTRA_COST_CMP_OFFSET_V  0x0000FFFFU
#define H264_A_INTRA_COST_CMP_OFFSET_S  14

/** H264_A_DECI_SCORE_REG register
 *  Video A luma and chroma MB decimate score Register.
 */
#define H264_A_DECI_SCORE_REG (DR_REG_H264_BASE + 0x10)
/** H264_A_C_DECI_SCORE : R/W; bitpos: [9:0]; default: 0;
 *  Configures video A chroma MB decimate score. When chroma score is smaller than it,
 *  chroma decimate will be enable.
 */
#define H264_A_C_DECI_SCORE    0x000003FFU
#define H264_A_C_DECI_SCORE_M  (H264_A_C_DECI_SCORE_V << H264_A_C_DECI_SCORE_S)
#define H264_A_C_DECI_SCORE_V  0x000003FFU
#define H264_A_C_DECI_SCORE_S  0
/** H264_A_L_DECI_SCORE : R/W; bitpos: [19:10]; default: 0;
 *  Configures video A luma MB decimate score. When luma score is smaller than it, luma
 *  decimate will be enable.
 */
#define H264_A_L_DECI_SCORE    0x000003FFU
#define H264_A_L_DECI_SCORE_M  (H264_A_L_DECI_SCORE_V << H264_A_L_DECI_SCORE_S)
#define H264_A_L_DECI_SCORE_V  0x000003FFU
#define H264_A_L_DECI_SCORE_S  10

/** H264_A_DECI_SCORE_OFFSET_REG register
 *  Video A luma and chroma MB decimate score offset Register.
 */
#define H264_A_DECI_SCORE_OFFSET_REG (DR_REG_H264_BASE + 0x14)
/** H264_A_I16X16_DECI_SCORE_OFFSET : R/W; bitpos: [5:0]; default: 0;
 *  Configures video A i16x16 MB decimate score offset. This offset will be added to
 *  i16x16 MB score.
 */
#define H264_A_I16X16_DECI_SCORE_OFFSET    0x0000003FU
#define H264_A_I16X16_DECI_SCORE_OFFSET_M  (H264_A_I16X16_DECI_SCORE_OFFSET_V << H264_A_I16X16_DECI_SCORE_OFFSET_S)
#define H264_A_I16X16_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_A_I16X16_DECI_SCORE_OFFSET_S  0
/** H264_A_I_CHROMA_DECI_SCORE_OFFSET : R/W; bitpos: [11:6]; default: 0;
 *  Configures video A I chroma MB decimate score offset. This offset will be added to
 *  I chroma MB score.
 */
#define H264_A_I_CHROMA_DECI_SCORE_OFFSET    0x0000003FU
#define H264_A_I_CHROMA_DECI_SCORE_OFFSET_M  (H264_A_I_CHROMA_DECI_SCORE_OFFSET_V << H264_A_I_CHROMA_DECI_SCORE_OFFSET_S)
#define H264_A_I_CHROMA_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_A_I_CHROMA_DECI_SCORE_OFFSET_S  6
/** H264_A_P16X16_DECI_SCORE_OFFSET : R/W; bitpos: [17:12]; default: 0;
 *  Configures video A p16x16 MB decimate score offset. This offset will be added to
 *  p16x16 MB score.
 */
#define H264_A_P16X16_DECI_SCORE_OFFSET    0x0000003FU
#define H264_A_P16X16_DECI_SCORE_OFFSET_M  (H264_A_P16X16_DECI_SCORE_OFFSET_V << H264_A_P16X16_DECI_SCORE_OFFSET_S)
#define H264_A_P16X16_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_A_P16X16_DECI_SCORE_OFFSET_S  12
/** H264_A_P_CHROMA_DECI_SCORE_OFFSET : R/W; bitpos: [23:18]; default: 0;
 *  Configures video A p chroma MB decimate score offset. This offset will be added to
 *  p chroma MB score.
 */
#define H264_A_P_CHROMA_DECI_SCORE_OFFSET    0x0000003FU
#define H264_A_P_CHROMA_DECI_SCORE_OFFSET_M  (H264_A_P_CHROMA_DECI_SCORE_OFFSET_V << H264_A_P_CHROMA_DECI_SCORE_OFFSET_S)
#define H264_A_P_CHROMA_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_A_P_CHROMA_DECI_SCORE_OFFSET_S  18

/** H264_A_RC_CONF0_REG register
 *  Video A rate control configuration register0.
 */
#define H264_A_RC_CONF0_REG (DR_REG_H264_BASE + 0x18)
/** H264_A_QP : R/W; bitpos: [5:0]; default: 0;
 *  Configures video A frame level initial luma QP value.
 */
#define H264_A_QP    0x0000003FU
#define H264_A_QP_M  (H264_A_QP_V << H264_A_QP_S)
#define H264_A_QP_V  0x0000003FU
#define H264_A_QP_S  0
/** H264_A_RATE_CTRL_U : R/W; bitpos: [21:6]; default: 0;
 *  Configures video A parameter U value. U = int((float) u << 8).
 */
#define H264_A_RATE_CTRL_U    0x0000FFFFU
#define H264_A_RATE_CTRL_U_M  (H264_A_RATE_CTRL_U_V << H264_A_RATE_CTRL_U_S)
#define H264_A_RATE_CTRL_U_V  0x0000FFFFU
#define H264_A_RATE_CTRL_U_S  6
/** H264_A_MB_RATE_CTRL_EN : R/W; bitpos: [22]; default: 0;
 *  Configures video A whether or not to open macro block rate ctrl.\\1:Open the macro
 *  block rate ctrl\\1:Close the macro block rate ctrl.
 */
#define H264_A_MB_RATE_CTRL_EN    (BIT(22))
#define H264_A_MB_RATE_CTRL_EN_M  (H264_A_MB_RATE_CTRL_EN_V << H264_A_MB_RATE_CTRL_EN_S)
#define H264_A_MB_RATE_CTRL_EN_V  0x00000001U
#define H264_A_MB_RATE_CTRL_EN_S  22

/** H264_A_RC_CONF1_REG register
 *  Video A rate control configuration register1.
 */
#define H264_A_RC_CONF1_REG (DR_REG_H264_BASE + 0x1c)
/** H264_A_CHROMA_DC_QP_DELTA : R/W; bitpos: [2:0]; default: 0;
 *  Configures video A chroma DC QP offset based on Chroma QP. Chroma DC QP = Chroma
 *  QP(after map) + reg_chroma_dc_qp_delta.
 */
#define H264_A_CHROMA_DC_QP_DELTA    0x00000007U
#define H264_A_CHROMA_DC_QP_DELTA_M  (H264_A_CHROMA_DC_QP_DELTA_V << H264_A_CHROMA_DC_QP_DELTA_S)
#define H264_A_CHROMA_DC_QP_DELTA_V  0x00000007U
#define H264_A_CHROMA_DC_QP_DELTA_S  0
/** H264_A_CHROMA_QP_DELTA : R/W; bitpos: [6:3]; default: 0;
 *  Configures video A chroma QP offset based on luma QP. Chroma QP(before map) = Luma
 *  QP + reg_chroma_qp_delta.
 */
#define H264_A_CHROMA_QP_DELTA    0x0000000FU
#define H264_A_CHROMA_QP_DELTA_M  (H264_A_CHROMA_QP_DELTA_V << H264_A_CHROMA_QP_DELTA_S)
#define H264_A_CHROMA_QP_DELTA_V  0x0000000FU
#define H264_A_CHROMA_QP_DELTA_S  3
/** H264_A_QP_MIN : R/W; bitpos: [12:7]; default: 0;
 *  Configures video A allowed luma QP min value.
 */
#define H264_A_QP_MIN    0x0000003FU
#define H264_A_QP_MIN_M  (H264_A_QP_MIN_V << H264_A_QP_MIN_S)
#define H264_A_QP_MIN_V  0x0000003FU
#define H264_A_QP_MIN_S  7
/** H264_A_QP_MAX : R/W; bitpos: [18:13]; default: 0;
 *  Configures video A allowed luma QP max value.
 */
#define H264_A_QP_MAX    0x0000003FU
#define H264_A_QP_MAX_M  (H264_A_QP_MAX_V << H264_A_QP_MAX_S)
#define H264_A_QP_MAX_V  0x0000003FU
#define H264_A_QP_MAX_S  13
/** H264_A_MAD_FRAME_PRED : R/W; bitpos: [30:19]; default: 0;
 *  Configures vdieo A frame level predicted MB MAD value.
 */
#define H264_A_MAD_FRAME_PRED    0x00000FFFU
#define H264_A_MAD_FRAME_PRED_M  (H264_A_MAD_FRAME_PRED_V << H264_A_MAD_FRAME_PRED_S)
#define H264_A_MAD_FRAME_PRED_V  0x00000FFFU
#define H264_A_MAD_FRAME_PRED_S  19

/** H264_A_DB_BYPASS_REG register
 *  Video A Deblocking bypass register
 */
#define H264_A_DB_BYPASS_REG (DR_REG_H264_BASE + 0x20)
/** H264_A_BYPASS_DB_FILTER : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to bypass video A deblcoking filter. \\0: Open the
 *  deblock filter\\1: Close the deblock filter
 */
#define H264_A_BYPASS_DB_FILTER    (BIT(0))
#define H264_A_BYPASS_DB_FILTER_M  (H264_A_BYPASS_DB_FILTER_V << H264_A_BYPASS_DB_FILTER_S)
#define H264_A_BYPASS_DB_FILTER_V  0x00000001U
#define H264_A_BYPASS_DB_FILTER_S  0

/** H264_A_ROI_REGION0_REG register
 *  Video A H264 ROI region0 range configure register.
 */
#define H264_A_ROI_REGION0_REG (DR_REG_H264_BASE + 0x24)
/** H264_A_ROI_REGION0_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 0 in Video A.
 */
#define H264_A_ROI_REGION0_X    0x0000007FU
#define H264_A_ROI_REGION0_X_M  (H264_A_ROI_REGION0_X_V << H264_A_ROI_REGION0_X_S)
#define H264_A_ROI_REGION0_X_V  0x0000007FU
#define H264_A_ROI_REGION0_X_S  0
/** H264_A_ROI_REGION0_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 0 in Video A.
 */
#define H264_A_ROI_REGION0_Y    0x0000007FU
#define H264_A_ROI_REGION0_Y_M  (H264_A_ROI_REGION0_Y_V << H264_A_ROI_REGION0_Y_S)
#define H264_A_ROI_REGION0_Y_V  0x0000007FU
#define H264_A_ROI_REGION0_Y_S  7
/** H264_A_ROI_REGION0_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 0 in
 *  Video A.
 */
#define H264_A_ROI_REGION0_X_LEN    0x0000007FU
#define H264_A_ROI_REGION0_X_LEN_M  (H264_A_ROI_REGION0_X_LEN_V << H264_A_ROI_REGION0_X_LEN_S)
#define H264_A_ROI_REGION0_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION0_X_LEN_S  14
/** H264_A_ROI_REGION0_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 0 in
 *  Video A.
 */
#define H264_A_ROI_REGION0_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION0_Y_LEN_M  (H264_A_ROI_REGION0_Y_LEN_V << H264_A_ROI_REGION0_Y_LEN_S)
#define H264_A_ROI_REGION0_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION0_Y_LEN_S  21
/** H264_A_ROI_REGION0_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 0 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION0_EN    (BIT(28))
#define H264_A_ROI_REGION0_EN_M  (H264_A_ROI_REGION0_EN_V << H264_A_ROI_REGION0_EN_S)
#define H264_A_ROI_REGION0_EN_V  0x00000001U
#define H264_A_ROI_REGION0_EN_S  28

/** H264_A_ROI_REGION1_REG register
 *  Video A H264 ROI region1 range configure register.
 */
#define H264_A_ROI_REGION1_REG (DR_REG_H264_BASE + 0x28)
/** H264_A_ROI_REGION1_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 1 in Video A.
 */
#define H264_A_ROI_REGION1_X    0x0000007FU
#define H264_A_ROI_REGION1_X_M  (H264_A_ROI_REGION1_X_V << H264_A_ROI_REGION1_X_S)
#define H264_A_ROI_REGION1_X_V  0x0000007FU
#define H264_A_ROI_REGION1_X_S  0
/** H264_A_ROI_REGION1_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 1 in Video A.
 */
#define H264_A_ROI_REGION1_Y    0x0000007FU
#define H264_A_ROI_REGION1_Y_M  (H264_A_ROI_REGION1_Y_V << H264_A_ROI_REGION1_Y_S)
#define H264_A_ROI_REGION1_Y_V  0x0000007FU
#define H264_A_ROI_REGION1_Y_S  7
/** H264_A_ROI_REGION1_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 1 in
 *  Video A.
 */
#define H264_A_ROI_REGION1_X_LEN    0x0000007FU
#define H264_A_ROI_REGION1_X_LEN_M  (H264_A_ROI_REGION1_X_LEN_V << H264_A_ROI_REGION1_X_LEN_S)
#define H264_A_ROI_REGION1_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION1_X_LEN_S  14
/** H264_A_ROI_REGION1_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 1 in
 *  Video A.
 */
#define H264_A_ROI_REGION1_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION1_Y_LEN_M  (H264_A_ROI_REGION1_Y_LEN_V << H264_A_ROI_REGION1_Y_LEN_S)
#define H264_A_ROI_REGION1_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION1_Y_LEN_S  21
/** H264_A_ROI_REGION1_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 1 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION1_EN    (BIT(28))
#define H264_A_ROI_REGION1_EN_M  (H264_A_ROI_REGION1_EN_V << H264_A_ROI_REGION1_EN_S)
#define H264_A_ROI_REGION1_EN_V  0x00000001U
#define H264_A_ROI_REGION1_EN_S  28

/** H264_A_ROI_REGION2_REG register
 *  Video A H264 ROI region2 range configure register.
 */
#define H264_A_ROI_REGION2_REG (DR_REG_H264_BASE + 0x2c)
/** H264_A_ROI_REGION2_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 2 in Video A.
 */
#define H264_A_ROI_REGION2_X    0x0000007FU
#define H264_A_ROI_REGION2_X_M  (H264_A_ROI_REGION2_X_V << H264_A_ROI_REGION2_X_S)
#define H264_A_ROI_REGION2_X_V  0x0000007FU
#define H264_A_ROI_REGION2_X_S  0
/** H264_A_ROI_REGION2_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 2 in Video A.
 */
#define H264_A_ROI_REGION2_Y    0x0000007FU
#define H264_A_ROI_REGION2_Y_M  (H264_A_ROI_REGION2_Y_V << H264_A_ROI_REGION2_Y_S)
#define H264_A_ROI_REGION2_Y_V  0x0000007FU
#define H264_A_ROI_REGION2_Y_S  7
/** H264_A_ROI_REGION2_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 2 in
 *  Video A.
 */
#define H264_A_ROI_REGION2_X_LEN    0x0000007FU
#define H264_A_ROI_REGION2_X_LEN_M  (H264_A_ROI_REGION2_X_LEN_V << H264_A_ROI_REGION2_X_LEN_S)
#define H264_A_ROI_REGION2_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION2_X_LEN_S  14
/** H264_A_ROI_REGION2_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 2 in
 *  Video A.
 */
#define H264_A_ROI_REGION2_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION2_Y_LEN_M  (H264_A_ROI_REGION2_Y_LEN_V << H264_A_ROI_REGION2_Y_LEN_S)
#define H264_A_ROI_REGION2_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION2_Y_LEN_S  21
/** H264_A_ROI_REGION2_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 2 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION2_EN    (BIT(28))
#define H264_A_ROI_REGION2_EN_M  (H264_A_ROI_REGION2_EN_V << H264_A_ROI_REGION2_EN_S)
#define H264_A_ROI_REGION2_EN_V  0x00000001U
#define H264_A_ROI_REGION2_EN_S  28

/** H264_A_ROI_REGION3_REG register
 *  Video A H264 ROI region3 range configure register.
 */
#define H264_A_ROI_REGION3_REG (DR_REG_H264_BASE + 0x30)
/** H264_A_ROI_REGION3_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 3 in Video A.
 */
#define H264_A_ROI_REGION3_X    0x0000007FU
#define H264_A_ROI_REGION3_X_M  (H264_A_ROI_REGION3_X_V << H264_A_ROI_REGION3_X_S)
#define H264_A_ROI_REGION3_X_V  0x0000007FU
#define H264_A_ROI_REGION3_X_S  0
/** H264_A_ROI_REGION3_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 3 in Video A.
 */
#define H264_A_ROI_REGION3_Y    0x0000007FU
#define H264_A_ROI_REGION3_Y_M  (H264_A_ROI_REGION3_Y_V << H264_A_ROI_REGION3_Y_S)
#define H264_A_ROI_REGION3_Y_V  0x0000007FU
#define H264_A_ROI_REGION3_Y_S  7
/** H264_A_ROI_REGION3_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 3 in
 *  video A.
 */
#define H264_A_ROI_REGION3_X_LEN    0x0000007FU
#define H264_A_ROI_REGION3_X_LEN_M  (H264_A_ROI_REGION3_X_LEN_V << H264_A_ROI_REGION3_X_LEN_S)
#define H264_A_ROI_REGION3_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION3_X_LEN_S  14
/** H264_A_ROI_REGION3_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 3 in
 *  video A.
 */
#define H264_A_ROI_REGION3_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION3_Y_LEN_M  (H264_A_ROI_REGION3_Y_LEN_V << H264_A_ROI_REGION3_Y_LEN_S)
#define H264_A_ROI_REGION3_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION3_Y_LEN_S  21
/** H264_A_ROI_REGION3_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 3 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION3_EN    (BIT(28))
#define H264_A_ROI_REGION3_EN_M  (H264_A_ROI_REGION3_EN_V << H264_A_ROI_REGION3_EN_S)
#define H264_A_ROI_REGION3_EN_V  0x00000001U
#define H264_A_ROI_REGION3_EN_S  28

/** H264_A_ROI_REGION4_REG register
 *  Video A H264 ROI region4 range configure register.
 */
#define H264_A_ROI_REGION4_REG (DR_REG_H264_BASE + 0x34)
/** H264_A_ROI_REGION4_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 4 in Video A.
 */
#define H264_A_ROI_REGION4_X    0x0000007FU
#define H264_A_ROI_REGION4_X_M  (H264_A_ROI_REGION4_X_V << H264_A_ROI_REGION4_X_S)
#define H264_A_ROI_REGION4_X_V  0x0000007FU
#define H264_A_ROI_REGION4_X_S  0
/** H264_A_ROI_REGION4_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 4 in Video A.
 */
#define H264_A_ROI_REGION4_Y    0x0000007FU
#define H264_A_ROI_REGION4_Y_M  (H264_A_ROI_REGION4_Y_V << H264_A_ROI_REGION4_Y_S)
#define H264_A_ROI_REGION4_Y_V  0x0000007FU
#define H264_A_ROI_REGION4_Y_S  7
/** H264_A_ROI_REGION4_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 4 in
 *  video A.
 */
#define H264_A_ROI_REGION4_X_LEN    0x0000007FU
#define H264_A_ROI_REGION4_X_LEN_M  (H264_A_ROI_REGION4_X_LEN_V << H264_A_ROI_REGION4_X_LEN_S)
#define H264_A_ROI_REGION4_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION4_X_LEN_S  14
/** H264_A_ROI_REGION4_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 4 in
 *  video A.
 */
#define H264_A_ROI_REGION4_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION4_Y_LEN_M  (H264_A_ROI_REGION4_Y_LEN_V << H264_A_ROI_REGION4_Y_LEN_S)
#define H264_A_ROI_REGION4_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION4_Y_LEN_S  21
/** H264_A_ROI_REGION4_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 4 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION4_EN    (BIT(28))
#define H264_A_ROI_REGION4_EN_M  (H264_A_ROI_REGION4_EN_V << H264_A_ROI_REGION4_EN_S)
#define H264_A_ROI_REGION4_EN_V  0x00000001U
#define H264_A_ROI_REGION4_EN_S  28

/** H264_A_ROI_REGION5_REG register
 *  Video A H264 ROI region5 range configure register.
 */
#define H264_A_ROI_REGION5_REG (DR_REG_H264_BASE + 0x38)
/** H264_A_ROI_REGION5_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontial start macroblocks of region 5 video A.
 */
#define H264_A_ROI_REGION5_X    0x0000007FU
#define H264_A_ROI_REGION5_X_M  (H264_A_ROI_REGION5_X_V << H264_A_ROI_REGION5_X_S)
#define H264_A_ROI_REGION5_X_V  0x0000007FU
#define H264_A_ROI_REGION5_X_S  0
/** H264_A_ROI_REGION5_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 5 video A.
 */
#define H264_A_ROI_REGION5_Y    0x0000007FU
#define H264_A_ROI_REGION5_Y_M  (H264_A_ROI_REGION5_Y_V << H264_A_ROI_REGION5_Y_S)
#define H264_A_ROI_REGION5_Y_V  0x0000007FU
#define H264_A_ROI_REGION5_Y_S  7
/** H264_A_ROI_REGION5_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 5
 *  video A.
 */
#define H264_A_ROI_REGION5_X_LEN    0x0000007FU
#define H264_A_ROI_REGION5_X_LEN_M  (H264_A_ROI_REGION5_X_LEN_V << H264_A_ROI_REGION5_X_LEN_S)
#define H264_A_ROI_REGION5_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION5_X_LEN_S  14
/** H264_A_ROI_REGION5_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 5 in
 *  video A.
 */
#define H264_A_ROI_REGION5_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION5_Y_LEN_M  (H264_A_ROI_REGION5_Y_LEN_V << H264_A_ROI_REGION5_Y_LEN_S)
#define H264_A_ROI_REGION5_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION5_Y_LEN_S  21
/** H264_A_ROI_REGION5_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 5 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION5_EN    (BIT(28))
#define H264_A_ROI_REGION5_EN_M  (H264_A_ROI_REGION5_EN_V << H264_A_ROI_REGION5_EN_S)
#define H264_A_ROI_REGION5_EN_V  0x00000001U
#define H264_A_ROI_REGION5_EN_S  28

/** H264_A_ROI_REGION6_REG register
 *  Video A H264 ROI region6 range configure register.
 */
#define H264_A_ROI_REGION6_REG (DR_REG_H264_BASE + 0x3c)
/** H264_A_ROI_REGION6_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontial start macroblocks of region 6 video A.
 */
#define H264_A_ROI_REGION6_X    0x0000007FU
#define H264_A_ROI_REGION6_X_M  (H264_A_ROI_REGION6_X_V << H264_A_ROI_REGION6_X_S)
#define H264_A_ROI_REGION6_X_V  0x0000007FU
#define H264_A_ROI_REGION6_X_S  0
/** H264_A_ROI_REGION6_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 6 in video A.
 */
#define H264_A_ROI_REGION6_Y    0x0000007FU
#define H264_A_ROI_REGION6_Y_M  (H264_A_ROI_REGION6_Y_V << H264_A_ROI_REGION6_Y_S)
#define H264_A_ROI_REGION6_Y_V  0x0000007FU
#define H264_A_ROI_REGION6_Y_S  7
/** H264_A_ROI_REGION6_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 6 in
 *  video A.
 */
#define H264_A_ROI_REGION6_X_LEN    0x0000007FU
#define H264_A_ROI_REGION6_X_LEN_M  (H264_A_ROI_REGION6_X_LEN_V << H264_A_ROI_REGION6_X_LEN_S)
#define H264_A_ROI_REGION6_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION6_X_LEN_S  14
/** H264_A_ROI_REGION6_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 6 in
 *  video A.
 */
#define H264_A_ROI_REGION6_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION6_Y_LEN_M  (H264_A_ROI_REGION6_Y_LEN_V << H264_A_ROI_REGION6_Y_LEN_S)
#define H264_A_ROI_REGION6_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION6_Y_LEN_S  21
/** H264_A_ROI_REGION6_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 6 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION6_EN    (BIT(28))
#define H264_A_ROI_REGION6_EN_M  (H264_A_ROI_REGION6_EN_V << H264_A_ROI_REGION6_EN_S)
#define H264_A_ROI_REGION6_EN_V  0x00000001U
#define H264_A_ROI_REGION6_EN_S  28

/** H264_A_ROI_REGION7_REG register
 *  Video A H264 ROI region7 range configure register.
 */
#define H264_A_ROI_REGION7_REG (DR_REG_H264_BASE + 0x40)
/** H264_A_ROI_REGION7_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 7 in video A.
 */
#define H264_A_ROI_REGION7_X    0x0000007FU
#define H264_A_ROI_REGION7_X_M  (H264_A_ROI_REGION7_X_V << H264_A_ROI_REGION7_X_S)
#define H264_A_ROI_REGION7_X_V  0x0000007FU
#define H264_A_ROI_REGION7_X_S  0
/** H264_A_ROI_REGION7_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 7 in video A.
 */
#define H264_A_ROI_REGION7_Y    0x0000007FU
#define H264_A_ROI_REGION7_Y_M  (H264_A_ROI_REGION7_Y_V << H264_A_ROI_REGION7_Y_S)
#define H264_A_ROI_REGION7_Y_V  0x0000007FU
#define H264_A_ROI_REGION7_Y_S  7
/** H264_A_ROI_REGION7_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 7 in
 *  video A.
 */
#define H264_A_ROI_REGION7_X_LEN    0x0000007FU
#define H264_A_ROI_REGION7_X_LEN_M  (H264_A_ROI_REGION7_X_LEN_V << H264_A_ROI_REGION7_X_LEN_S)
#define H264_A_ROI_REGION7_X_LEN_V  0x0000007FU
#define H264_A_ROI_REGION7_X_LEN_S  14
/** H264_A_ROI_REGION7_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 7 in
 *  video A.
 */
#define H264_A_ROI_REGION7_Y_LEN    0x0000007FU
#define H264_A_ROI_REGION7_Y_LEN_M  (H264_A_ROI_REGION7_Y_LEN_V << H264_A_ROI_REGION7_Y_LEN_S)
#define H264_A_ROI_REGION7_Y_LEN_V  0x0000007FU
#define H264_A_ROI_REGION7_Y_LEN_S  21
/** H264_A_ROI_REGION7_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video A ROI of region 7 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_A_ROI_REGION7_EN    (BIT(28))
#define H264_A_ROI_REGION7_EN_M  (H264_A_ROI_REGION7_EN_V << H264_A_ROI_REGION7_EN_S)
#define H264_A_ROI_REGION7_EN_V  0x00000001U
#define H264_A_ROI_REGION7_EN_S  28

/** H264_A_ROI_REGION0_3_QP_REG register
 *  Video A H264 ROI region0, region1,region2,region3 QP register.
 */
#define H264_A_ROI_REGION0_3_QP_REG (DR_REG_H264_BASE + 0x44)
/** H264_A_ROI_REGION0_QP : R/W; bitpos: [6:0]; default: 0;
 *  Configure H264 ROI region0 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION0_QP    0x0000007FU
#define H264_A_ROI_REGION0_QP_M  (H264_A_ROI_REGION0_QP_V << H264_A_ROI_REGION0_QP_S)
#define H264_A_ROI_REGION0_QP_V  0x0000007FU
#define H264_A_ROI_REGION0_QP_S  0
/** H264_A_ROI_REGION1_QP : R/W; bitpos: [13:7]; default: 0;
 *  Configure H264 ROI region1 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION1_QP    0x0000007FU
#define H264_A_ROI_REGION1_QP_M  (H264_A_ROI_REGION1_QP_V << H264_A_ROI_REGION1_QP_S)
#define H264_A_ROI_REGION1_QP_V  0x0000007FU
#define H264_A_ROI_REGION1_QP_S  7
/** H264_A_ROI_REGION2_QP : R/W; bitpos: [20:14]; default: 0;
 *  Configure H264 ROI region2 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION2_QP    0x0000007FU
#define H264_A_ROI_REGION2_QP_M  (H264_A_ROI_REGION2_QP_V << H264_A_ROI_REGION2_QP_S)
#define H264_A_ROI_REGION2_QP_V  0x0000007FU
#define H264_A_ROI_REGION2_QP_S  14
/** H264_A_ROI_REGION3_QP : R/W; bitpos: [27:21]; default: 0;
 *  Configure H264 ROI region3 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION3_QP    0x0000007FU
#define H264_A_ROI_REGION3_QP_M  (H264_A_ROI_REGION3_QP_V << H264_A_ROI_REGION3_QP_S)
#define H264_A_ROI_REGION3_QP_V  0x0000007FU
#define H264_A_ROI_REGION3_QP_S  21

/** H264_A_ROI_REGION4_7_QP_REG register
 *  Video A H264 ROI region4, region5,region6,region7 QP register.
 */
#define H264_A_ROI_REGION4_7_QP_REG (DR_REG_H264_BASE + 0x48)
/** H264_A_ROI_REGION4_QP : R/W; bitpos: [6:0]; default: 0;
 *  Configure H264 ROI region4 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION4_QP    0x0000007FU
#define H264_A_ROI_REGION4_QP_M  (H264_A_ROI_REGION4_QP_V << H264_A_ROI_REGION4_QP_S)
#define H264_A_ROI_REGION4_QP_V  0x0000007FU
#define H264_A_ROI_REGION4_QP_S  0
/** H264_A_ROI_REGION5_QP : R/W; bitpos: [13:7]; default: 0;
 *  Configure H264 ROI region5 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION5_QP    0x0000007FU
#define H264_A_ROI_REGION5_QP_M  (H264_A_ROI_REGION5_QP_V << H264_A_ROI_REGION5_QP_S)
#define H264_A_ROI_REGION5_QP_V  0x0000007FU
#define H264_A_ROI_REGION5_QP_S  7
/** H264_A_ROI_REGION6_QP : R/W; bitpos: [20:14]; default: 0;
 *  Configure H264 ROI region6 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION6_QP    0x0000007FU
#define H264_A_ROI_REGION6_QP_M  (H264_A_ROI_REGION6_QP_V << H264_A_ROI_REGION6_QP_S)
#define H264_A_ROI_REGION6_QP_V  0x0000007FU
#define H264_A_ROI_REGION6_QP_S  14
/** H264_A_ROI_REGION7_QP : R/W; bitpos: [27:21]; default: 0;
 *  Configure H264 ROI region7 qp in video A,fixed qp or delta qp.
 */
#define H264_A_ROI_REGION7_QP    0x0000007FU
#define H264_A_ROI_REGION7_QP_M  (H264_A_ROI_REGION7_QP_V << H264_A_ROI_REGION7_QP_S)
#define H264_A_ROI_REGION7_QP_V  0x0000007FU
#define H264_A_ROI_REGION7_QP_S  21

/** H264_A_NO_ROI_REGION_QP_OFFSET_REG register
 *  Video A H264 no roi region QP register.
 */
#define H264_A_NO_ROI_REGION_QP_OFFSET_REG (DR_REG_H264_BASE + 0x4c)
/** H264_A_NO_ROI_REGION_QP : R/W; bitpos: [6:0]; default: 0;
 *  Configure H264 no region qp in video A, delta qp.
 */
#define H264_A_NO_ROI_REGION_QP    0x0000007FU
#define H264_A_NO_ROI_REGION_QP_M  (H264_A_NO_ROI_REGION_QP_V << H264_A_NO_ROI_REGION_QP_S)
#define H264_A_NO_ROI_REGION_QP_V  0x0000007FU
#define H264_A_NO_ROI_REGION_QP_S  0

/** H264_A_ROI_CONFIG_REG register
 *  Video A H264 ROI configure register.
 */
#define H264_A_ROI_CONFIG_REG (DR_REG_H264_BASE + 0x50)
/** H264_A_ROI_EN : R/W; bitpos: [0]; default: 0;
 *  Configure whether or not to enable ROI in video A.\\0:not enable ROI\\1:enable ROI.
 */
#define H264_A_ROI_EN    (BIT(0))
#define H264_A_ROI_EN_M  (H264_A_ROI_EN_V << H264_A_ROI_EN_S)
#define H264_A_ROI_EN_V  0x00000001U
#define H264_A_ROI_EN_S  0
/** H264_A_ROI_MODE : R/W; bitpos: [1]; default: 0;
 *  Configure the mode of ROI in video A.\\0:fixed qp\\1:delta qp.
 */
#define H264_A_ROI_MODE    (BIT(1))
#define H264_A_ROI_MODE_M  (H264_A_ROI_MODE_V << H264_A_ROI_MODE_S)
#define H264_A_ROI_MODE_V  0x00000001U
#define H264_A_ROI_MODE_S  1

/** H264_B_SYS_MB_RES_REG register
 *  Video B horizontal and vertical MB resolution register.
 */
#define H264_B_SYS_MB_RES_REG (DR_REG_H264_BASE + 0x54)
/** H264_B_SYS_TOTAL_MB_Y : R/W; bitpos: [6:0]; default: 0;
 *  Configures video B vertical MB resolution.
 */
#define H264_B_SYS_TOTAL_MB_Y    0x0000007FU
#define H264_B_SYS_TOTAL_MB_Y_M  (H264_B_SYS_TOTAL_MB_Y_V << H264_B_SYS_TOTAL_MB_Y_S)
#define H264_B_SYS_TOTAL_MB_Y_V  0x0000007FU
#define H264_B_SYS_TOTAL_MB_Y_S  0
/** H264_B_SYS_TOTAL_MB_X : R/W; bitpos: [13:7]; default: 0;
 *  Configures video B horizontal MB resolution.
 */
#define H264_B_SYS_TOTAL_MB_X    0x0000007FU
#define H264_B_SYS_TOTAL_MB_X_M  (H264_B_SYS_TOTAL_MB_X_V << H264_B_SYS_TOTAL_MB_X_S)
#define H264_B_SYS_TOTAL_MB_X_V  0x0000007FU
#define H264_B_SYS_TOTAL_MB_X_S  7

/** H264_B_SYS_CONF_REG register
 *  Video B system level configuration register.
 */
#define H264_B_SYS_CONF_REG (DR_REG_H264_BASE + 0x58)
/** H264_B_DB_TMP_READY_TRIGGER_MB_NUM : R/W; bitpos: [6:0]; default: 3;
 *  Configures when to trigger  video B H264_DB_TMP_READY_INT. When the (MB number of
 *  written db temp+1) is greater than this filed in first MB line, trigger
 *  H264_DB_TMP_READY_INT. Min is 3.
 */
#define H264_B_DB_TMP_READY_TRIGGER_MB_NUM    0x0000007FU
#define H264_B_DB_TMP_READY_TRIGGER_MB_NUM_M  (H264_B_DB_TMP_READY_TRIGGER_MB_NUM_V << H264_B_DB_TMP_READY_TRIGGER_MB_NUM_S)
#define H264_B_DB_TMP_READY_TRIGGER_MB_NUM_V  0x0000007FU
#define H264_B_DB_TMP_READY_TRIGGER_MB_NUM_S  0
/** H264_B_REC_READY_TRIGGER_MB_LINES : R/W; bitpos: [13:7]; default: 4;
 *  Configures when to trigger  video B H264_REC_READY_INT. When the MB line number of
 *  generated reconstruct pixel is greater than this filed, trigger H264_REC_READY_INT.
 *  Min is 4.
 */
#define H264_B_REC_READY_TRIGGER_MB_LINES    0x0000007FU
#define H264_B_REC_READY_TRIGGER_MB_LINES_M  (H264_B_REC_READY_TRIGGER_MB_LINES_V << H264_B_REC_READY_TRIGGER_MB_LINES_S)
#define H264_B_REC_READY_TRIGGER_MB_LINES_V  0x0000007FU
#define H264_B_REC_READY_TRIGGER_MB_LINES_S  7
/** H264_B_INTRA_COST_CMP_OFFSET : R/W; bitpos: [29:14]; default: 0;
 *  Configures video B intra cost offset when I MB compared with P MB.
 */
#define H264_B_INTRA_COST_CMP_OFFSET    0x0000FFFFU
#define H264_B_INTRA_COST_CMP_OFFSET_M  (H264_B_INTRA_COST_CMP_OFFSET_V << H264_B_INTRA_COST_CMP_OFFSET_S)
#define H264_B_INTRA_COST_CMP_OFFSET_V  0x0000FFFFU
#define H264_B_INTRA_COST_CMP_OFFSET_S  14

/** H264_B_DECI_SCORE_REG register
 *  Video B luma and chroma MB decimate score Register.
 */
#define H264_B_DECI_SCORE_REG (DR_REG_H264_BASE + 0x5c)
/** H264_B_C_DECI_SCORE : R/W; bitpos: [9:0]; default: 0;
 *  Configures video B chroma MB decimate score. When chroma score is smaller than it,
 *  chroma decimate will be enable.
 */
#define H264_B_C_DECI_SCORE    0x000003FFU
#define H264_B_C_DECI_SCORE_M  (H264_B_C_DECI_SCORE_V << H264_B_C_DECI_SCORE_S)
#define H264_B_C_DECI_SCORE_V  0x000003FFU
#define H264_B_C_DECI_SCORE_S  0
/** H264_B_L_DECI_SCORE : R/W; bitpos: [19:10]; default: 0;
 *  Configures video B luma MB decimate score. When luma score is smaller than it, luma
 *  decimate will be enable.
 */
#define H264_B_L_DECI_SCORE    0x000003FFU
#define H264_B_L_DECI_SCORE_M  (H264_B_L_DECI_SCORE_V << H264_B_L_DECI_SCORE_S)
#define H264_B_L_DECI_SCORE_V  0x000003FFU
#define H264_B_L_DECI_SCORE_S  10

/** H264_B_DECI_SCORE_OFFSET_REG register
 *  Video B luma and chroma MB decimate score offset Register.
 */
#define H264_B_DECI_SCORE_OFFSET_REG (DR_REG_H264_BASE + 0x60)
/** H264_B_I16X16_DECI_SCORE_OFFSET : R/W; bitpos: [5:0]; default: 0;
 *  Configures video B i16x16 MB decimate score offset. This offset will be added to
 *  i16x16 MB score.
 */
#define H264_B_I16X16_DECI_SCORE_OFFSET    0x0000003FU
#define H264_B_I16X16_DECI_SCORE_OFFSET_M  (H264_B_I16X16_DECI_SCORE_OFFSET_V << H264_B_I16X16_DECI_SCORE_OFFSET_S)
#define H264_B_I16X16_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_B_I16X16_DECI_SCORE_OFFSET_S  0
/** H264_B_I_CHROMA_DECI_SCORE_OFFSET : R/W; bitpos: [11:6]; default: 0;
 *  Configures video B I chroma MB decimate score offset. This offset will be added to
 *  I chroma MB score.
 */
#define H264_B_I_CHROMA_DECI_SCORE_OFFSET    0x0000003FU
#define H264_B_I_CHROMA_DECI_SCORE_OFFSET_M  (H264_B_I_CHROMA_DECI_SCORE_OFFSET_V << H264_B_I_CHROMA_DECI_SCORE_OFFSET_S)
#define H264_B_I_CHROMA_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_B_I_CHROMA_DECI_SCORE_OFFSET_S  6
/** H264_B_P16X16_DECI_SCORE_OFFSET : R/W; bitpos: [17:12]; default: 0;
 *  Configures video B p16x16 MB decimate score offset. This offset will be added to
 *  p16x16 MB score.
 */
#define H264_B_P16X16_DECI_SCORE_OFFSET    0x0000003FU
#define H264_B_P16X16_DECI_SCORE_OFFSET_M  (H264_B_P16X16_DECI_SCORE_OFFSET_V << H264_B_P16X16_DECI_SCORE_OFFSET_S)
#define H264_B_P16X16_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_B_P16X16_DECI_SCORE_OFFSET_S  12
/** H264_B_P_CHROMA_DECI_SCORE_OFFSET : R/W; bitpos: [23:18]; default: 0;
 *  Configures video B p chroma MB decimate score offset. This offset will be added to
 *  p chroma MB score.
 */
#define H264_B_P_CHROMA_DECI_SCORE_OFFSET    0x0000003FU
#define H264_B_P_CHROMA_DECI_SCORE_OFFSET_M  (H264_B_P_CHROMA_DECI_SCORE_OFFSET_V << H264_B_P_CHROMA_DECI_SCORE_OFFSET_S)
#define H264_B_P_CHROMA_DECI_SCORE_OFFSET_V  0x0000003FU
#define H264_B_P_CHROMA_DECI_SCORE_OFFSET_S  18

/** H264_B_RC_CONF0_REG register
 *  Video B rate control configuration register0.
 */
#define H264_B_RC_CONF0_REG (DR_REG_H264_BASE + 0x64)
/** H264_B_QP : R/W; bitpos: [5:0]; default: 0;
 *  Configures video B frame level initial luma QP value.
 */
#define H264_B_QP    0x0000003FU
#define H264_B_QP_M  (H264_B_QP_V << H264_B_QP_S)
#define H264_B_QP_V  0x0000003FU
#define H264_B_QP_S  0
/** H264_B_RATE_CTRL_U : R/W; bitpos: [21:6]; default: 0;
 *  Configures video B parameter U value. U = int((float) u << 8).
 */
#define H264_B_RATE_CTRL_U    0x0000FFFFU
#define H264_B_RATE_CTRL_U_M  (H264_B_RATE_CTRL_U_V << H264_B_RATE_CTRL_U_S)
#define H264_B_RATE_CTRL_U_V  0x0000FFFFU
#define H264_B_RATE_CTRL_U_S  6
/** H264_B_MB_RATE_CTRL_EN : R/W; bitpos: [22]; default: 0;
 *  Configures video A whether or not to open macro block rate ctrl.\\1:Open the macro
 *  block rate ctrl\\1:Close the macro block rate ctrl.
 */
#define H264_B_MB_RATE_CTRL_EN    (BIT(22))
#define H264_B_MB_RATE_CTRL_EN_M  (H264_B_MB_RATE_CTRL_EN_V << H264_B_MB_RATE_CTRL_EN_S)
#define H264_B_MB_RATE_CTRL_EN_V  0x00000001U
#define H264_B_MB_RATE_CTRL_EN_S  22

/** H264_B_RC_CONF1_REG register
 *  Video B rate control configuration register1.
 */
#define H264_B_RC_CONF1_REG (DR_REG_H264_BASE + 0x68)
/** H264_B_CHROMA_DC_QP_DELTA : R/W; bitpos: [2:0]; default: 0;
 *  Configures video B chroma DC QP offset based on Chroma QP. Chroma DC QP = Chroma
 *  QP(after map) + reg_chroma_dc_qp_delta.
 */
#define H264_B_CHROMA_DC_QP_DELTA    0x00000007U
#define H264_B_CHROMA_DC_QP_DELTA_M  (H264_B_CHROMA_DC_QP_DELTA_V << H264_B_CHROMA_DC_QP_DELTA_S)
#define H264_B_CHROMA_DC_QP_DELTA_V  0x00000007U
#define H264_B_CHROMA_DC_QP_DELTA_S  0
/** H264_B_CHROMA_QP_DELTA : R/W; bitpos: [6:3]; default: 0;
 *  Configures video B chroma QP offset based on luma QP. Chroma QP(before map) = Luma
 *  QP + reg_chroma_qp_delta.
 */
#define H264_B_CHROMA_QP_DELTA    0x0000000FU
#define H264_B_CHROMA_QP_DELTA_M  (H264_B_CHROMA_QP_DELTA_V << H264_B_CHROMA_QP_DELTA_S)
#define H264_B_CHROMA_QP_DELTA_V  0x0000000FU
#define H264_B_CHROMA_QP_DELTA_S  3
/** H264_B_QP_MIN : R/W; bitpos: [12:7]; default: 0;
 *  Configures video B allowed luma QP min value.
 */
#define H264_B_QP_MIN    0x0000003FU
#define H264_B_QP_MIN_M  (H264_B_QP_MIN_V << H264_B_QP_MIN_S)
#define H264_B_QP_MIN_V  0x0000003FU
#define H264_B_QP_MIN_S  7
/** H264_B_QP_MAX : R/W; bitpos: [18:13]; default: 0;
 *  Configures video B allowed luma QP max value.
 */
#define H264_B_QP_MAX    0x0000003FU
#define H264_B_QP_MAX_M  (H264_B_QP_MAX_V << H264_B_QP_MAX_S)
#define H264_B_QP_MAX_V  0x0000003FU
#define H264_B_QP_MAX_S  13
/** H264_B_MAD_FRAME_PRED : R/W; bitpos: [30:19]; default: 0;
 *  Configures vdieo B frame level predicted MB MAD value.
 */
#define H264_B_MAD_FRAME_PRED    0x00000FFFU
#define H264_B_MAD_FRAME_PRED_M  (H264_B_MAD_FRAME_PRED_V << H264_B_MAD_FRAME_PRED_S)
#define H264_B_MAD_FRAME_PRED_V  0x00000FFFU
#define H264_B_MAD_FRAME_PRED_S  19

/** H264_B_DB_BYPASS_REG register
 *  Video B Deblocking bypass register
 */
#define H264_B_DB_BYPASS_REG (DR_REG_H264_BASE + 0x6c)
/** H264_B_BYPASS_DB_FILTER : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to bypass video B deblcoking filter. \\0: Open the
 *  deblock filter\\1: Close the deblock filter
 */
#define H264_B_BYPASS_DB_FILTER    (BIT(0))
#define H264_B_BYPASS_DB_FILTER_M  (H264_B_BYPASS_DB_FILTER_V << H264_B_BYPASS_DB_FILTER_S)
#define H264_B_BYPASS_DB_FILTER_V  0x00000001U
#define H264_B_BYPASS_DB_FILTER_S  0

/** H264_B_ROI_REGION0_REG register
 *  Video B H264 ROI region0 range configure register.
 */
#define H264_B_ROI_REGION0_REG (DR_REG_H264_BASE + 0x70)
/** H264_B_ROI_REGION0_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 0 in Video B.
 */
#define H264_B_ROI_REGION0_X    0x0000007FU
#define H264_B_ROI_REGION0_X_M  (H264_B_ROI_REGION0_X_V << H264_B_ROI_REGION0_X_S)
#define H264_B_ROI_REGION0_X_V  0x0000007FU
#define H264_B_ROI_REGION0_X_S  0
/** H264_B_ROI_REGION0_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 0 in Video B.
 */
#define H264_B_ROI_REGION0_Y    0x0000007FU
#define H264_B_ROI_REGION0_Y_M  (H264_B_ROI_REGION0_Y_V << H264_B_ROI_REGION0_Y_S)
#define H264_B_ROI_REGION0_Y_V  0x0000007FU
#define H264_B_ROI_REGION0_Y_S  7
/** H264_B_ROI_REGION0_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 0 in
 *  Video B.
 */
#define H264_B_ROI_REGION0_X_LEN    0x0000007FU
#define H264_B_ROI_REGION0_X_LEN_M  (H264_B_ROI_REGION0_X_LEN_V << H264_B_ROI_REGION0_X_LEN_S)
#define H264_B_ROI_REGION0_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION0_X_LEN_S  14
/** H264_B_ROI_REGION0_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 0 in
 *  Video B.
 */
#define H264_B_ROI_REGION0_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION0_Y_LEN_M  (H264_B_ROI_REGION0_Y_LEN_V << H264_B_ROI_REGION0_Y_LEN_S)
#define H264_B_ROI_REGION0_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION0_Y_LEN_S  21
/** H264_B_ROI_REGION0_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 0 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION0_EN    (BIT(28))
#define H264_B_ROI_REGION0_EN_M  (H264_B_ROI_REGION0_EN_V << H264_B_ROI_REGION0_EN_S)
#define H264_B_ROI_REGION0_EN_V  0x00000001U
#define H264_B_ROI_REGION0_EN_S  28

/** H264_B_ROI_REGION1_REG register
 *  Video B H264 ROI region1 range configure register.
 */
#define H264_B_ROI_REGION1_REG (DR_REG_H264_BASE + 0x74)
/** H264_B_ROI_REGION1_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 1 in Video B.
 */
#define H264_B_ROI_REGION1_X    0x0000007FU
#define H264_B_ROI_REGION1_X_M  (H264_B_ROI_REGION1_X_V << H264_B_ROI_REGION1_X_S)
#define H264_B_ROI_REGION1_X_V  0x0000007FU
#define H264_B_ROI_REGION1_X_S  0
/** H264_B_ROI_REGION1_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 1 in Video B.
 */
#define H264_B_ROI_REGION1_Y    0x0000007FU
#define H264_B_ROI_REGION1_Y_M  (H264_B_ROI_REGION1_Y_V << H264_B_ROI_REGION1_Y_S)
#define H264_B_ROI_REGION1_Y_V  0x0000007FU
#define H264_B_ROI_REGION1_Y_S  7
/** H264_B_ROI_REGION1_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 1 in
 *  Video B.
 */
#define H264_B_ROI_REGION1_X_LEN    0x0000007FU
#define H264_B_ROI_REGION1_X_LEN_M  (H264_B_ROI_REGION1_X_LEN_V << H264_B_ROI_REGION1_X_LEN_S)
#define H264_B_ROI_REGION1_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION1_X_LEN_S  14
/** H264_B_ROI_REGION1_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 1 in
 *  Video B.
 */
#define H264_B_ROI_REGION1_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION1_Y_LEN_M  (H264_B_ROI_REGION1_Y_LEN_V << H264_B_ROI_REGION1_Y_LEN_S)
#define H264_B_ROI_REGION1_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION1_Y_LEN_S  21
/** H264_B_ROI_REGION1_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 1 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION1_EN    (BIT(28))
#define H264_B_ROI_REGION1_EN_M  (H264_B_ROI_REGION1_EN_V << H264_B_ROI_REGION1_EN_S)
#define H264_B_ROI_REGION1_EN_V  0x00000001U
#define H264_B_ROI_REGION1_EN_S  28

/** H264_B_ROI_REGION2_REG register
 *  Video B H264 ROI region2 range configure register.
 */
#define H264_B_ROI_REGION2_REG (DR_REG_H264_BASE + 0x78)
/** H264_B_ROI_REGION2_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 2 in Video B.
 */
#define H264_B_ROI_REGION2_X    0x0000007FU
#define H264_B_ROI_REGION2_X_M  (H264_B_ROI_REGION2_X_V << H264_B_ROI_REGION2_X_S)
#define H264_B_ROI_REGION2_X_V  0x0000007FU
#define H264_B_ROI_REGION2_X_S  0
/** H264_B_ROI_REGION2_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 2 in Video B.
 */
#define H264_B_ROI_REGION2_Y    0x0000007FU
#define H264_B_ROI_REGION2_Y_M  (H264_B_ROI_REGION2_Y_V << H264_B_ROI_REGION2_Y_S)
#define H264_B_ROI_REGION2_Y_V  0x0000007FU
#define H264_B_ROI_REGION2_Y_S  7
/** H264_B_ROI_REGION2_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 2 in
 *  Video B.
 */
#define H264_B_ROI_REGION2_X_LEN    0x0000007FU
#define H264_B_ROI_REGION2_X_LEN_M  (H264_B_ROI_REGION2_X_LEN_V << H264_B_ROI_REGION2_X_LEN_S)
#define H264_B_ROI_REGION2_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION2_X_LEN_S  14
/** H264_B_ROI_REGION2_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 2 in
 *  Video B.
 */
#define H264_B_ROI_REGION2_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION2_Y_LEN_M  (H264_B_ROI_REGION2_Y_LEN_V << H264_B_ROI_REGION2_Y_LEN_S)
#define H264_B_ROI_REGION2_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION2_Y_LEN_S  21
/** H264_B_ROI_REGION2_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 2 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION2_EN    (BIT(28))
#define H264_B_ROI_REGION2_EN_M  (H264_B_ROI_REGION2_EN_V << H264_B_ROI_REGION2_EN_S)
#define H264_B_ROI_REGION2_EN_V  0x00000001U
#define H264_B_ROI_REGION2_EN_S  28

/** H264_B_ROI_REGION3_REG register
 *  Video B H264 ROI region3 range configure register.
 */
#define H264_B_ROI_REGION3_REG (DR_REG_H264_BASE + 0x7c)
/** H264_B_ROI_REGION3_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 3 in Video B.
 */
#define H264_B_ROI_REGION3_X    0x0000007FU
#define H264_B_ROI_REGION3_X_M  (H264_B_ROI_REGION3_X_V << H264_B_ROI_REGION3_X_S)
#define H264_B_ROI_REGION3_X_V  0x0000007FU
#define H264_B_ROI_REGION3_X_S  0
/** H264_B_ROI_REGION3_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 3 in Video B.
 */
#define H264_B_ROI_REGION3_Y    0x0000007FU
#define H264_B_ROI_REGION3_Y_M  (H264_B_ROI_REGION3_Y_V << H264_B_ROI_REGION3_Y_S)
#define H264_B_ROI_REGION3_Y_V  0x0000007FU
#define H264_B_ROI_REGION3_Y_S  7
/** H264_B_ROI_REGION3_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 3 in
 *  video B.
 */
#define H264_B_ROI_REGION3_X_LEN    0x0000007FU
#define H264_B_ROI_REGION3_X_LEN_M  (H264_B_ROI_REGION3_X_LEN_V << H264_B_ROI_REGION3_X_LEN_S)
#define H264_B_ROI_REGION3_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION3_X_LEN_S  14
/** H264_B_ROI_REGION3_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 3 in
 *  video B.
 */
#define H264_B_ROI_REGION3_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION3_Y_LEN_M  (H264_B_ROI_REGION3_Y_LEN_V << H264_B_ROI_REGION3_Y_LEN_S)
#define H264_B_ROI_REGION3_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION3_Y_LEN_S  21
/** H264_B_ROI_REGION3_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 3 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION3_EN    (BIT(28))
#define H264_B_ROI_REGION3_EN_M  (H264_B_ROI_REGION3_EN_V << H264_B_ROI_REGION3_EN_S)
#define H264_B_ROI_REGION3_EN_V  0x00000001U
#define H264_B_ROI_REGION3_EN_S  28

/** H264_B_ROI_REGION4_REG register
 *  Video B H264 ROI region4 range configure register.
 */
#define H264_B_ROI_REGION4_REG (DR_REG_H264_BASE + 0x80)
/** H264_B_ROI_REGION4_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 4 in Video B.
 */
#define H264_B_ROI_REGION4_X    0x0000007FU
#define H264_B_ROI_REGION4_X_M  (H264_B_ROI_REGION4_X_V << H264_B_ROI_REGION4_X_S)
#define H264_B_ROI_REGION4_X_V  0x0000007FU
#define H264_B_ROI_REGION4_X_S  0
/** H264_B_ROI_REGION4_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 4 in Video B.
 */
#define H264_B_ROI_REGION4_Y    0x0000007FU
#define H264_B_ROI_REGION4_Y_M  (H264_B_ROI_REGION4_Y_V << H264_B_ROI_REGION4_Y_S)
#define H264_B_ROI_REGION4_Y_V  0x0000007FU
#define H264_B_ROI_REGION4_Y_S  7
/** H264_B_ROI_REGION4_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 4 in
 *  video B.
 */
#define H264_B_ROI_REGION4_X_LEN    0x0000007FU
#define H264_B_ROI_REGION4_X_LEN_M  (H264_B_ROI_REGION4_X_LEN_V << H264_B_ROI_REGION4_X_LEN_S)
#define H264_B_ROI_REGION4_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION4_X_LEN_S  14
/** H264_B_ROI_REGION4_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 4 in
 *  video B.
 */
#define H264_B_ROI_REGION4_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION4_Y_LEN_M  (H264_B_ROI_REGION4_Y_LEN_V << H264_B_ROI_REGION4_Y_LEN_S)
#define H264_B_ROI_REGION4_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION4_Y_LEN_S  21
/** H264_B_ROI_REGION4_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 4 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION4_EN    (BIT(28))
#define H264_B_ROI_REGION4_EN_M  (H264_B_ROI_REGION4_EN_V << H264_B_ROI_REGION4_EN_S)
#define H264_B_ROI_REGION4_EN_V  0x00000001U
#define H264_B_ROI_REGION4_EN_S  28

/** H264_B_ROI_REGION5_REG register
 *  Video B H264 ROI region5 range configure register.
 */
#define H264_B_ROI_REGION5_REG (DR_REG_H264_BASE + 0x84)
/** H264_B_ROI_REGION5_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontial start macroblocks of region 5 video B.
 */
#define H264_B_ROI_REGION5_X    0x0000007FU
#define H264_B_ROI_REGION5_X_M  (H264_B_ROI_REGION5_X_V << H264_B_ROI_REGION5_X_S)
#define H264_B_ROI_REGION5_X_V  0x0000007FU
#define H264_B_ROI_REGION5_X_S  0
/** H264_B_ROI_REGION5_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 5 video B.
 */
#define H264_B_ROI_REGION5_Y    0x0000007FU
#define H264_B_ROI_REGION5_Y_M  (H264_B_ROI_REGION5_Y_V << H264_B_ROI_REGION5_Y_S)
#define H264_B_ROI_REGION5_Y_V  0x0000007FU
#define H264_B_ROI_REGION5_Y_S  7
/** H264_B_ROI_REGION5_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 5
 *  video B.
 */
#define H264_B_ROI_REGION5_X_LEN    0x0000007FU
#define H264_B_ROI_REGION5_X_LEN_M  (H264_B_ROI_REGION5_X_LEN_V << H264_B_ROI_REGION5_X_LEN_S)
#define H264_B_ROI_REGION5_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION5_X_LEN_S  14
/** H264_B_ROI_REGION5_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 5 in
 *  video B.
 */
#define H264_B_ROI_REGION5_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION5_Y_LEN_M  (H264_B_ROI_REGION5_Y_LEN_V << H264_B_ROI_REGION5_Y_LEN_S)
#define H264_B_ROI_REGION5_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION5_Y_LEN_S  21
/** H264_B_ROI_REGION5_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 5 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION5_EN    (BIT(28))
#define H264_B_ROI_REGION5_EN_M  (H264_B_ROI_REGION5_EN_V << H264_B_ROI_REGION5_EN_S)
#define H264_B_ROI_REGION5_EN_V  0x00000001U
#define H264_B_ROI_REGION5_EN_S  28

/** H264_B_ROI_REGION6_REG register
 *  Video B H264 ROI region6 range configure register.
 */
#define H264_B_ROI_REGION6_REG (DR_REG_H264_BASE + 0x88)
/** H264_B_ROI_REGION6_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontial start macroblocks of region 6 video B.
 */
#define H264_B_ROI_REGION6_X    0x0000007FU
#define H264_B_ROI_REGION6_X_M  (H264_B_ROI_REGION6_X_V << H264_B_ROI_REGION6_X_S)
#define H264_B_ROI_REGION6_X_V  0x0000007FU
#define H264_B_ROI_REGION6_X_S  0
/** H264_B_ROI_REGION6_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 6 in video B.
 */
#define H264_B_ROI_REGION6_Y    0x0000007FU
#define H264_B_ROI_REGION6_Y_M  (H264_B_ROI_REGION6_Y_V << H264_B_ROI_REGION6_Y_S)
#define H264_B_ROI_REGION6_Y_V  0x0000007FU
#define H264_B_ROI_REGION6_Y_S  7
/** H264_B_ROI_REGION6_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 6 in
 *  video B.
 */
#define H264_B_ROI_REGION6_X_LEN    0x0000007FU
#define H264_B_ROI_REGION6_X_LEN_M  (H264_B_ROI_REGION6_X_LEN_V << H264_B_ROI_REGION6_X_LEN_S)
#define H264_B_ROI_REGION6_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION6_X_LEN_S  14
/** H264_B_ROI_REGION6_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 6 in
 *  video B.
 */
#define H264_B_ROI_REGION6_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION6_Y_LEN_M  (H264_B_ROI_REGION6_Y_LEN_V << H264_B_ROI_REGION6_Y_LEN_S)
#define H264_B_ROI_REGION6_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION6_Y_LEN_S  21
/** H264_B_ROI_REGION6_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 6 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION6_EN    (BIT(28))
#define H264_B_ROI_REGION6_EN_M  (H264_B_ROI_REGION6_EN_V << H264_B_ROI_REGION6_EN_S)
#define H264_B_ROI_REGION6_EN_V  0x00000001U
#define H264_B_ROI_REGION6_EN_S  28

/** H264_B_ROI_REGION7_REG register
 *  Video B H264 ROI region7 range configure register.
 */
#define H264_B_ROI_REGION7_REG (DR_REG_H264_BASE + 0x8c)
/** H264_B_ROI_REGION7_X : R/W; bitpos: [6:0]; default: 0;
 *  Configures the horizontal start macroblocks of region 7 in video B.
 */
#define H264_B_ROI_REGION7_X    0x0000007FU
#define H264_B_ROI_REGION7_X_M  (H264_B_ROI_REGION7_X_V << H264_B_ROI_REGION7_X_S)
#define H264_B_ROI_REGION7_X_V  0x0000007FU
#define H264_B_ROI_REGION7_X_S  0
/** H264_B_ROI_REGION7_Y : R/W; bitpos: [13:7]; default: 0;
 *  Configures the  vertical start macroblocks of region 7 in video B.
 */
#define H264_B_ROI_REGION7_Y    0x0000007FU
#define H264_B_ROI_REGION7_Y_M  (H264_B_ROI_REGION7_Y_V << H264_B_ROI_REGION7_Y_S)
#define H264_B_ROI_REGION7_Y_V  0x0000007FU
#define H264_B_ROI_REGION7_Y_S  7
/** H264_B_ROI_REGION7_X_LEN : R/W; bitpos: [20:14]; default: 0;
 *  Configures the number of  macroblocks in horizontal direction of  the region 7 in
 *  video B.
 */
#define H264_B_ROI_REGION7_X_LEN    0x0000007FU
#define H264_B_ROI_REGION7_X_LEN_M  (H264_B_ROI_REGION7_X_LEN_V << H264_B_ROI_REGION7_X_LEN_S)
#define H264_B_ROI_REGION7_X_LEN_V  0x0000007FU
#define H264_B_ROI_REGION7_X_LEN_S  14
/** H264_B_ROI_REGION7_Y_LEN : R/W; bitpos: [27:21]; default: 0;
 *  Configures the number of  macroblocks in vertical direction of  the region 7 in
 *  video B.
 */
#define H264_B_ROI_REGION7_Y_LEN    0x0000007FU
#define H264_B_ROI_REGION7_Y_LEN_M  (H264_B_ROI_REGION7_Y_LEN_V << H264_B_ROI_REGION7_Y_LEN_S)
#define H264_B_ROI_REGION7_Y_LEN_V  0x0000007FU
#define H264_B_ROI_REGION7_Y_LEN_S  21
/** H264_B_ROI_REGION7_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to open Video B ROI of region 7 .\\0:Close ROI\\1:Open
 *  ROI.
 */
#define H264_B_ROI_REGION7_EN    (BIT(28))
#define H264_B_ROI_REGION7_EN_M  (H264_B_ROI_REGION7_EN_V << H264_B_ROI_REGION7_EN_S)
#define H264_B_ROI_REGION7_EN_V  0x00000001U
#define H264_B_ROI_REGION7_EN_S  28

/** H264_B_ROI_REGION0_3_QP_REG register
 *  Video B H264 ROI region0, region1,region2,region3 QP register.
 */
#define H264_B_ROI_REGION0_3_QP_REG (DR_REG_H264_BASE + 0x90)
/** H264_B_ROI_REGION0_QP : R/W; bitpos: [6:0]; default: 0;
 *  Configure H264 ROI region0 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION0_QP    0x0000007FU
#define H264_B_ROI_REGION0_QP_M  (H264_B_ROI_REGION0_QP_V << H264_B_ROI_REGION0_QP_S)
#define H264_B_ROI_REGION0_QP_V  0x0000007FU
#define H264_B_ROI_REGION0_QP_S  0
/** H264_B_ROI_REGION1_QP : R/W; bitpos: [13:7]; default: 0;
 *  Configure H264 ROI region1 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION1_QP    0x0000007FU
#define H264_B_ROI_REGION1_QP_M  (H264_B_ROI_REGION1_QP_V << H264_B_ROI_REGION1_QP_S)
#define H264_B_ROI_REGION1_QP_V  0x0000007FU
#define H264_B_ROI_REGION1_QP_S  7
/** H264_B_ROI_REGION2_QP : R/W; bitpos: [20:14]; default: 0;
 *  Configure H264 ROI region2 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION2_QP    0x0000007FU
#define H264_B_ROI_REGION2_QP_M  (H264_B_ROI_REGION2_QP_V << H264_B_ROI_REGION2_QP_S)
#define H264_B_ROI_REGION2_QP_V  0x0000007FU
#define H264_B_ROI_REGION2_QP_S  14
/** H264_B_ROI_REGION3_QP : R/W; bitpos: [27:21]; default: 0;
 *  Configure H264 ROI region3 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION3_QP    0x0000007FU
#define H264_B_ROI_REGION3_QP_M  (H264_B_ROI_REGION3_QP_V << H264_B_ROI_REGION3_QP_S)
#define H264_B_ROI_REGION3_QP_V  0x0000007FU
#define H264_B_ROI_REGION3_QP_S  21

/** H264_B_ROI_REGION4_7_QP_REG register
 *  Video B H264 ROI region4, region5,region6,region7 QP register.
 */
#define H264_B_ROI_REGION4_7_QP_REG (DR_REG_H264_BASE + 0x94)
/** H264_B_ROI_REGION4_QP : R/W; bitpos: [6:0]; default: 0;
 *  Configure H264 ROI region4 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION4_QP    0x0000007FU
#define H264_B_ROI_REGION4_QP_M  (H264_B_ROI_REGION4_QP_V << H264_B_ROI_REGION4_QP_S)
#define H264_B_ROI_REGION4_QP_V  0x0000007FU
#define H264_B_ROI_REGION4_QP_S  0
/** H264_B_ROI_REGION5_QP : R/W; bitpos: [13:7]; default: 0;
 *  Configure H264 ROI region5 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION5_QP    0x0000007FU
#define H264_B_ROI_REGION5_QP_M  (H264_B_ROI_REGION5_QP_V << H264_B_ROI_REGION5_QP_S)
#define H264_B_ROI_REGION5_QP_V  0x0000007FU
#define H264_B_ROI_REGION5_QP_S  7
/** H264_B_ROI_REGION6_QP : R/W; bitpos: [20:14]; default: 0;
 *  Configure H264 ROI region6 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION6_QP    0x0000007FU
#define H264_B_ROI_REGION6_QP_M  (H264_B_ROI_REGION6_QP_V << H264_B_ROI_REGION6_QP_S)
#define H264_B_ROI_REGION6_QP_V  0x0000007FU
#define H264_B_ROI_REGION6_QP_S  14
/** H264_B_ROI_REGION7_QP : R/W; bitpos: [27:21]; default: 0;
 *  Configure H264 ROI region7 qp in video B,fixed qp or delta qp.
 */
#define H264_B_ROI_REGION7_QP    0x0000007FU
#define H264_B_ROI_REGION7_QP_M  (H264_B_ROI_REGION7_QP_V << H264_B_ROI_REGION7_QP_S)
#define H264_B_ROI_REGION7_QP_V  0x0000007FU
#define H264_B_ROI_REGION7_QP_S  21

/** H264_B_NO_ROI_REGION_QP_OFFSET_REG register
 *  Video B H264 no roi region QP register.
 */
#define H264_B_NO_ROI_REGION_QP_OFFSET_REG (DR_REG_H264_BASE + 0x98)
/** H264_B_NO_ROI_REGION_QP : R/W; bitpos: [6:0]; default: 0;
 *  Configure H264 no region qp in video B, delta qp.
 */
#define H264_B_NO_ROI_REGION_QP    0x0000007FU
#define H264_B_NO_ROI_REGION_QP_M  (H264_B_NO_ROI_REGION_QP_V << H264_B_NO_ROI_REGION_QP_S)
#define H264_B_NO_ROI_REGION_QP_V  0x0000007FU
#define H264_B_NO_ROI_REGION_QP_S  0

/** H264_B_ROI_CONFIG_REG register
 *  Video B H264 ROI configure register.
 */
#define H264_B_ROI_CONFIG_REG (DR_REG_H264_BASE + 0x9c)
/** H264_B_ROI_EN : R/W; bitpos: [0]; default: 0;
 *  Configure whether or not to enable ROI in video B.\\0:not enable ROI\\1:enable ROI.
 */
#define H264_B_ROI_EN    (BIT(0))
#define H264_B_ROI_EN_M  (H264_B_ROI_EN_V << H264_B_ROI_EN_S)
#define H264_B_ROI_EN_V  0x00000001U
#define H264_B_ROI_EN_S  0
/** H264_B_ROI_MODE : R/W; bitpos: [1]; default: 0;
 *  Configure the mode of ROI in video B.\\0:fixed qp\\1:delta qp.
 */
#define H264_B_ROI_MODE    (BIT(1))
#define H264_B_ROI_MODE_M  (H264_B_ROI_MODE_V << H264_B_ROI_MODE_S)
#define H264_B_ROI_MODE_V  0x00000001U
#define H264_B_ROI_MODE_S  1

/** H264_RC_STATUS0_REG register
 *  Rate control status register0.
 */
#define H264_RC_STATUS0_REG (DR_REG_H264_BASE + 0xa0)
/** H264_FRAME_MAD_SUM : RO; bitpos: [20:0]; default: 0;
 *  Represents all MB actual MAD sum value of one frame.
 */
#define H264_FRAME_MAD_SUM    0x001FFFFFU
#define H264_FRAME_MAD_SUM_M  (H264_FRAME_MAD_SUM_V << H264_FRAME_MAD_SUM_S)
#define H264_FRAME_MAD_SUM_V  0x001FFFFFU
#define H264_FRAME_MAD_SUM_S  0

/** H264_RC_STATUS1_REG register
 *  Rate control status register1.
 */
#define H264_RC_STATUS1_REG (DR_REG_H264_BASE + 0xa4)
/** H264_FRAME_ENC_BITS : RO; bitpos: [26:0]; default: 0;
 *  Represents all MB actual encoding bits sum value of one frame.
 */
#define H264_FRAME_ENC_BITS    0x07FFFFFFU
#define H264_FRAME_ENC_BITS_M  (H264_FRAME_ENC_BITS_V << H264_FRAME_ENC_BITS_S)
#define H264_FRAME_ENC_BITS_V  0x07FFFFFFU
#define H264_FRAME_ENC_BITS_S  0

/** H264_RC_STATUS2_REG register
 *  Rate control status register2.
 */
#define H264_RC_STATUS2_REG (DR_REG_H264_BASE + 0xa8)
/** H264_FRAME_QP_SUM : RO; bitpos: [18:0]; default: 0;
 *  Represents all MB actual luma QP sum value of one frame.
 */
#define H264_FRAME_QP_SUM    0x0007FFFFU
#define H264_FRAME_QP_SUM_M  (H264_FRAME_QP_SUM_V << H264_FRAME_QP_SUM_S)
#define H264_FRAME_QP_SUM_V  0x0007FFFFU
#define H264_FRAME_QP_SUM_S  0

/** H264_SLICE_HEADER_REMAIN_REG register
 *  Frame Slice Header remain bit register.
 */
#define H264_SLICE_HEADER_REMAIN_REG (DR_REG_H264_BASE + 0xac)
/** H264_SLICE_REMAIN_BITLENGTH : R/W; bitpos: [2:0]; default: 0;
 *  Configures Slice Header remain bit number
 */
#define H264_SLICE_REMAIN_BITLENGTH    0x00000007U
#define H264_SLICE_REMAIN_BITLENGTH_M  (H264_SLICE_REMAIN_BITLENGTH_V << H264_SLICE_REMAIN_BITLENGTH_S)
#define H264_SLICE_REMAIN_BITLENGTH_V  0x00000007U
#define H264_SLICE_REMAIN_BITLENGTH_S  0
/** H264_SLICE_REMAIN_BIT : R/W; bitpos: [10:3]; default: 0;
 *  Configures Slice Header remain bit
 */
#define H264_SLICE_REMAIN_BIT    0x000000FFU
#define H264_SLICE_REMAIN_BIT_M  (H264_SLICE_REMAIN_BIT_V << H264_SLICE_REMAIN_BIT_S)
#define H264_SLICE_REMAIN_BIT_V  0x000000FFU
#define H264_SLICE_REMAIN_BIT_S  3

/** H264_SLICE_HEADER_BYTE_LENGTH_REG register
 *  Frame Slice Header byte length register.
 */
#define H264_SLICE_HEADER_BYTE_LENGTH_REG (DR_REG_H264_BASE + 0xb0)
/** H264_SLICE_BYTE_LENGTH : R/W; bitpos: [3:0]; default: 0;
 *  Configures Slice Header byte number
 */
#define H264_SLICE_BYTE_LENGTH    0x0000000FU
#define H264_SLICE_BYTE_LENGTH_M  (H264_SLICE_BYTE_LENGTH_V << H264_SLICE_BYTE_LENGTH_S)
#define H264_SLICE_BYTE_LENGTH_V  0x0000000FU
#define H264_SLICE_BYTE_LENGTH_S  0

/** H264_BS_THRESHOLD_REG register
 *  Bitstream buffer overflow threshold register
 */
#define H264_BS_THRESHOLD_REG (DR_REG_H264_BASE + 0xb4)
/** H264_BS_BUFFER_THRESHOLD : R/W; bitpos: [6:0]; default: 48;
 *  Configures bitstream buffer overflow threshold. This value should be bigger than
 *  the encode bytes of one 4x4 submb.
 */
#define H264_BS_BUFFER_THRESHOLD    0x0000007FU
#define H264_BS_BUFFER_THRESHOLD_M  (H264_BS_BUFFER_THRESHOLD_V << H264_BS_BUFFER_THRESHOLD_S)
#define H264_BS_BUFFER_THRESHOLD_V  0x0000007FU
#define H264_BS_BUFFER_THRESHOLD_S  0

/** H264_SLICE_HEADER_BYTE0_REG register
 *  Frame Slice Header byte low 32 bit  register.
 */
#define H264_SLICE_HEADER_BYTE0_REG (DR_REG_H264_BASE + 0xb8)
/** H264_SLICE_BYTE_LSB : R/W; bitpos: [31:0]; default: 0;
 *  Configures Slice Header low 32 bit
 */
#define H264_SLICE_BYTE_LSB    0xFFFFFFFFU
#define H264_SLICE_BYTE_LSB_M  (H264_SLICE_BYTE_LSB_V << H264_SLICE_BYTE_LSB_S)
#define H264_SLICE_BYTE_LSB_V  0xFFFFFFFFU
#define H264_SLICE_BYTE_LSB_S  0

/** H264_SLICE_HEADER_BYTE1_REG register
 *  Frame Slice Header byte high 32 bit  register.
 */
#define H264_SLICE_HEADER_BYTE1_REG (DR_REG_H264_BASE + 0xbc)
/** H264_SLICE_BYTE_MSB : R/W; bitpos: [31:0]; default: 0;
 *  Configures Slice Header high 32 bit
 */
#define H264_SLICE_BYTE_MSB    0xFFFFFFFFU
#define H264_SLICE_BYTE_MSB_M  (H264_SLICE_BYTE_MSB_V << H264_SLICE_BYTE_MSB_S)
#define H264_SLICE_BYTE_MSB_V  0xFFFFFFFFU
#define H264_SLICE_BYTE_MSB_S  0

/** H264_INT_RAW_REG register
 *  Interrupt raw status register
 */
#define H264_INT_RAW_REG (DR_REG_H264_BASE + 0xc0)
/** H264_DB_TMP_READY_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  Raw status bit: The raw interrupt status of  H264_DB_TMP_READY_INT. Triggered when
 *  H264 written enough db tmp pixel.
 */
#define H264_DB_TMP_READY_INT_RAW    (BIT(0))
#define H264_DB_TMP_READY_INT_RAW_M  (H264_DB_TMP_READY_INT_RAW_V << H264_DB_TMP_READY_INT_RAW_S)
#define H264_DB_TMP_READY_INT_RAW_V  0x00000001U
#define H264_DB_TMP_READY_INT_RAW_S  0
/** H264_REC_READY_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  Raw status bit: The raw interrupt status of  H264_REC_READY_INT. Triggered when
 *  H264 encoding enough reconstruct pixel.
 */
#define H264_REC_READY_INT_RAW    (BIT(1))
#define H264_REC_READY_INT_RAW_M  (H264_REC_READY_INT_RAW_V << H264_REC_READY_INT_RAW_S)
#define H264_REC_READY_INT_RAW_V  0x00000001U
#define H264_REC_READY_INT_RAW_S  1
/** H264_FRAME_DONE_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  Raw status bit: The raw interrupt status of  H264_FRAME_DONE_INT. Triggered when
 *  H264 encoding one frame done.
 */
#define H264_FRAME_DONE_INT_RAW    (BIT(2))
#define H264_FRAME_DONE_INT_RAW_M  (H264_FRAME_DONE_INT_RAW_V << H264_FRAME_DONE_INT_RAW_S)
#define H264_FRAME_DONE_INT_RAW_V  0x00000001U
#define H264_FRAME_DONE_INT_RAW_S  2
/** H264_DMA_MOVE_2MB_LINE_DONE_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  Raw status bit: The raw interrupt status of H264_DMA_MOVE_2MB_LINE_DONE_INT.
 *  Triggered when H264 move two MB lines of reference frame from external mem to
 *  internal mem done.
 */
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_RAW    (BIT(3))
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_RAW_M  (H264_DMA_MOVE_2MB_LINE_DONE_INT_RAW_V << H264_DMA_MOVE_2MB_LINE_DONE_INT_RAW_S)
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_RAW_V  0x00000001U
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_RAW_S  3

/** H264_INT_ST_REG register
 *  Interrupt masked status register
 */
#define H264_INT_ST_REG (DR_REG_H264_BASE + 0xc4)
/** H264_DB_TMP_READY_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of H264_DB_TMP_READY_INT. Valid only when the
 *  H264_DB_TMP_READY_INT_ENA is set to 1.
 */
#define H264_DB_TMP_READY_INT_ST    (BIT(0))
#define H264_DB_TMP_READY_INT_ST_M  (H264_DB_TMP_READY_INT_ST_V << H264_DB_TMP_READY_INT_ST_S)
#define H264_DB_TMP_READY_INT_ST_V  0x00000001U
#define H264_DB_TMP_READY_INT_ST_S  0
/** H264_REC_READY_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of H264_REC_READY_INT. Valid only when the
 *  H264_REC_READY_INT_ENA is set to 1.
 */
#define H264_REC_READY_INT_ST    (BIT(1))
#define H264_REC_READY_INT_ST_M  (H264_REC_READY_INT_ST_V << H264_REC_READY_INT_ST_S)
#define H264_REC_READY_INT_ST_V  0x00000001U
#define H264_REC_READY_INT_ST_S  1
/** H264_FRAME_DONE_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of H264_FRAME_DONE_INT. Valid only when the
 *  H264_FRAME_DONE_INT_ENA is set to 1.
 */
#define H264_FRAME_DONE_INT_ST    (BIT(2))
#define H264_FRAME_DONE_INT_ST_M  (H264_FRAME_DONE_INT_ST_V << H264_FRAME_DONE_INT_ST_S)
#define H264_FRAME_DONE_INT_ST_V  0x00000001U
#define H264_FRAME_DONE_INT_ST_S  2
/** H264_DMA_MOVE_2MB_LINE_DONE_INT_ST : RO; bitpos: [3]; default: 0;
 *  Masked status bit: The masked interrupt status of H264_DMA_MOVE_2MB_LINE_DONE_INT.
 *  Valid only when the H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA is set to 1.
 */
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ST    (BIT(3))
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ST_M  (H264_DMA_MOVE_2MB_LINE_DONE_INT_ST_V << H264_DMA_MOVE_2MB_LINE_DONE_INT_ST_S)
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ST_V  0x00000001U
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ST_S  3

/** H264_INT_ENA_REG register
 *  Interrupt enable register
 */
#define H264_INT_ENA_REG (DR_REG_H264_BASE + 0xc8)
/** H264_DB_TMP_READY_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable H264_DB_TMP_READY_INT.
 */
#define H264_DB_TMP_READY_INT_ENA    (BIT(0))
#define H264_DB_TMP_READY_INT_ENA_M  (H264_DB_TMP_READY_INT_ENA_V << H264_DB_TMP_READY_INT_ENA_S)
#define H264_DB_TMP_READY_INT_ENA_V  0x00000001U
#define H264_DB_TMP_READY_INT_ENA_S  0
/** H264_REC_READY_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable H264_REC_READY_INT.
 */
#define H264_REC_READY_INT_ENA    (BIT(1))
#define H264_REC_READY_INT_ENA_M  (H264_REC_READY_INT_ENA_V << H264_REC_READY_INT_ENA_S)
#define H264_REC_READY_INT_ENA_V  0x00000001U
#define H264_REC_READY_INT_ENA_S  1
/** H264_FRAME_DONE_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable H264_FRAME_DONE_INT.
 */
#define H264_FRAME_DONE_INT_ENA    (BIT(2))
#define H264_FRAME_DONE_INT_ENA_M  (H264_FRAME_DONE_INT_ENA_V << H264_FRAME_DONE_INT_ENA_S)
#define H264_FRAME_DONE_INT_ENA_V  0x00000001U
#define H264_FRAME_DONE_INT_ENA_S  2
/** H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Enable bit: Write 1 to enable H264_DMA_MOVE_2MB_LINE_DONE_INT.
 */
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA    (BIT(3))
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA_M  (H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA_V << H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA_S)
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA_V  0x00000001U
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_ENA_S  3

/** H264_INT_CLR_REG register
 *  Interrupt clear register
 */
#define H264_INT_CLR_REG (DR_REG_H264_BASE + 0xcc)
/** H264_DB_TMP_READY_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear H264_DB_TMP_READY_INT.
 */
#define H264_DB_TMP_READY_INT_CLR    (BIT(0))
#define H264_DB_TMP_READY_INT_CLR_M  (H264_DB_TMP_READY_INT_CLR_V << H264_DB_TMP_READY_INT_CLR_S)
#define H264_DB_TMP_READY_INT_CLR_V  0x00000001U
#define H264_DB_TMP_READY_INT_CLR_S  0
/** H264_REC_READY_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear H264_REC_READY_INT.
 */
#define H264_REC_READY_INT_CLR    (BIT(1))
#define H264_REC_READY_INT_CLR_M  (H264_REC_READY_INT_CLR_V << H264_REC_READY_INT_CLR_S)
#define H264_REC_READY_INT_CLR_V  0x00000001U
#define H264_REC_READY_INT_CLR_S  1
/** H264_FRAME_DONE_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear H264_FRAME_DONE_INT.
 */
#define H264_FRAME_DONE_INT_CLR    (BIT(2))
#define H264_FRAME_DONE_INT_CLR_M  (H264_FRAME_DONE_INT_CLR_V << H264_FRAME_DONE_INT_CLR_S)
#define H264_FRAME_DONE_INT_CLR_V  0x00000001U
#define H264_FRAME_DONE_INT_CLR_S  2
/** H264_DMA_MOVE_2MB_LINE_DONE_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Clear bit: Write 1 to clear H264_DMA_MOVE_2MB_LINE_DONE_INT.
 */
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_CLR    (BIT(3))
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_CLR_M  (H264_DMA_MOVE_2MB_LINE_DONE_INT_CLR_V << H264_DMA_MOVE_2MB_LINE_DONE_INT_CLR_S)
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_CLR_V  0x00000001U
#define H264_DMA_MOVE_2MB_LINE_DONE_INT_CLR_S  3

/** H264_CONF_REG register
 *  General configuration register.
 */
#define H264_CONF_REG (DR_REG_H264_BASE + 0xd0)
/** H264_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to open register clock gate.\\0: Open the clock gate only
 *  when application writes registers\\1: Force open the clock gate for register
 */
#define H264_CLK_EN    (BIT(0))
#define H264_CLK_EN_M  (H264_CLK_EN_V << H264_CLK_EN_S)
#define H264_CLK_EN_V  0x00000001U
#define H264_CLK_EN_S  0
/** H264_REC_RAM_CLK_EN2 : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to open the clock gate for rec ram2.\\0: Open the clock
 *  gate only when application writes or reads rec ram2\\1: Force open the clock gate
 *  for rec ram2
 */
#define H264_REC_RAM_CLK_EN2    (BIT(1))
#define H264_REC_RAM_CLK_EN2_M  (H264_REC_RAM_CLK_EN2_V << H264_REC_RAM_CLK_EN2_S)
#define H264_REC_RAM_CLK_EN2_V  0x00000001U
#define H264_REC_RAM_CLK_EN2_S  1
/** H264_REC_RAM_CLK_EN1 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to open the clock gate for rec ram1.\\0: Open the clock
 *  gate only when application writes or reads rec ram1\\1: Force open the clock gate
 *  for rec ram1
 */
#define H264_REC_RAM_CLK_EN1    (BIT(2))
#define H264_REC_RAM_CLK_EN1_M  (H264_REC_RAM_CLK_EN1_V << H264_REC_RAM_CLK_EN1_S)
#define H264_REC_RAM_CLK_EN1_V  0x00000001U
#define H264_REC_RAM_CLK_EN1_S  2
/** H264_QUANT_RAM_CLK_EN2 : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to open the clock gate for quant ram2.\\0: Open the clock
 *  gate only when application writes or reads quant ram2\\1: Force open the clock gate
 *  for quant ram2
 */
#define H264_QUANT_RAM_CLK_EN2    (BIT(3))
#define H264_QUANT_RAM_CLK_EN2_M  (H264_QUANT_RAM_CLK_EN2_V << H264_QUANT_RAM_CLK_EN2_S)
#define H264_QUANT_RAM_CLK_EN2_V  0x00000001U
#define H264_QUANT_RAM_CLK_EN2_S  3
/** H264_QUANT_RAM_CLK_EN1 : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to open the clock gate for quant ram1.\\0: Open the clock
 *  gate only when application writes or reads quant ram1\\1: Force open the clock gate
 *  for quant ram1
 */
#define H264_QUANT_RAM_CLK_EN1    (BIT(4))
#define H264_QUANT_RAM_CLK_EN1_M  (H264_QUANT_RAM_CLK_EN1_V << H264_QUANT_RAM_CLK_EN1_S)
#define H264_QUANT_RAM_CLK_EN1_V  0x00000001U
#define H264_QUANT_RAM_CLK_EN1_S  4
/** H264_PRE_RAM_CLK_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to open the clock gate for pre ram.\\0: Open the clock
 *  gate only when application writes or reads pre ram\\1: Force open the clock gate
 *  for pre ram
 */
#define H264_PRE_RAM_CLK_EN    (BIT(5))
#define H264_PRE_RAM_CLK_EN_M  (H264_PRE_RAM_CLK_EN_V << H264_PRE_RAM_CLK_EN_S)
#define H264_PRE_RAM_CLK_EN_V  0x00000001U
#define H264_PRE_RAM_CLK_EN_S  5
/** H264_MVD_RAM_CLK_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to open the clock gate for mvd ram.\\0: Open the clock
 *  gate only when application writes or reads mvd ram\\1: Force open the clock gate
 *  for mvd ram
 */
#define H264_MVD_RAM_CLK_EN    (BIT(6))
#define H264_MVD_RAM_CLK_EN_M  (H264_MVD_RAM_CLK_EN_V << H264_MVD_RAM_CLK_EN_S)
#define H264_MVD_RAM_CLK_EN_V  0x00000001U
#define H264_MVD_RAM_CLK_EN_S  6
/** H264_MC_RAM_CLK_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to open the clock gate for mc ram.\\0: Open the clock
 *  gate only when application writes or reads mc ram\\1: Force open the clock gate for
 *  mc ram
 */
#define H264_MC_RAM_CLK_EN    (BIT(7))
#define H264_MC_RAM_CLK_EN_M  (H264_MC_RAM_CLK_EN_V << H264_MC_RAM_CLK_EN_S)
#define H264_MC_RAM_CLK_EN_V  0x00000001U
#define H264_MC_RAM_CLK_EN_S  7
/** H264_REF_RAM_CLK_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to open the clock gate for ref ram.\\0: Open the clock
 *  gate only when application writes or reads ref ram\\1: Force open the clock gate
 *  for ref ram
 */
#define H264_REF_RAM_CLK_EN    (BIT(8))
#define H264_REF_RAM_CLK_EN_M  (H264_REF_RAM_CLK_EN_V << H264_REF_RAM_CLK_EN_S)
#define H264_REF_RAM_CLK_EN_V  0x00000001U
#define H264_REF_RAM_CLK_EN_S  8
/** H264_I4X4_REF_RAM_CLK_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to open the clock gate for i4x4_mode ram.\\0: Open the
 *  clock gate only when application writes or reads i4x4_mode ram\\1: Force open the
 *  clock gate for i4x4_mode ram
 */
#define H264_I4X4_REF_RAM_CLK_EN    (BIT(9))
#define H264_I4X4_REF_RAM_CLK_EN_M  (H264_I4X4_REF_RAM_CLK_EN_V << H264_I4X4_REF_RAM_CLK_EN_S)
#define H264_I4X4_REF_RAM_CLK_EN_V  0x00000001U
#define H264_I4X4_REF_RAM_CLK_EN_S  9
/** H264_IME_RAM_CLK_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to open the clock gate for ime ram.\\0: Open the clock
 *  gate only when application writes or reads ime ram\\1: Force open the clock gate
 *  for ime ram
 */
#define H264_IME_RAM_CLK_EN    (BIT(10))
#define H264_IME_RAM_CLK_EN_M  (H264_IME_RAM_CLK_EN_V << H264_IME_RAM_CLK_EN_S)
#define H264_IME_RAM_CLK_EN_V  0x00000001U
#define H264_IME_RAM_CLK_EN_S  10
/** H264_FME_RAM_CLK_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to open the clock gate for fme ram.\\0: Open the clock
 *  gate only when application writes or readsfme ram\\1: Force open the clock gate for
 *  fme ram
 */
#define H264_FME_RAM_CLK_EN    (BIT(11))
#define H264_FME_RAM_CLK_EN_M  (H264_FME_RAM_CLK_EN_V << H264_FME_RAM_CLK_EN_S)
#define H264_FME_RAM_CLK_EN_V  0x00000001U
#define H264_FME_RAM_CLK_EN_S  11
/** H264_FETCH_RAM_CLK_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to open the clock gate for fetch ram.\\0: Open the clock
 *  gate only when application writes or reads fetch ram\\1: Force open the clock gate
 *  for fetch ram
 */
#define H264_FETCH_RAM_CLK_EN    (BIT(12))
#define H264_FETCH_RAM_CLK_EN_M  (H264_FETCH_RAM_CLK_EN_V << H264_FETCH_RAM_CLK_EN_S)
#define H264_FETCH_RAM_CLK_EN_V  0x00000001U
#define H264_FETCH_RAM_CLK_EN_S  12
/** H264_DB_RAM_CLK_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to open the clock gate for db ram.\\0: Open the clock
 *  gate only when application writes or reads db ram\\1: Force open the clock gate for
 *  db ram
 */
#define H264_DB_RAM_CLK_EN    (BIT(13))
#define H264_DB_RAM_CLK_EN_M  (H264_DB_RAM_CLK_EN_V << H264_DB_RAM_CLK_EN_S)
#define H264_DB_RAM_CLK_EN_V  0x00000001U
#define H264_DB_RAM_CLK_EN_S  13
/** H264_CUR_MB_RAM_CLK_EN : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to open the clock gate for cur_mb ram.\\0: Open the clock
 *  gate only when application writes or reads cur_mb ram\\1: Force open the clock gate
 *  for cur_mb ram
 */
#define H264_CUR_MB_RAM_CLK_EN    (BIT(14))
#define H264_CUR_MB_RAM_CLK_EN_M  (H264_CUR_MB_RAM_CLK_EN_V << H264_CUR_MB_RAM_CLK_EN_S)
#define H264_CUR_MB_RAM_CLK_EN_V  0x00000001U
#define H264_CUR_MB_RAM_CLK_EN_S  14
/** H264_CAVLC_RAM_CLK_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to open the clock gate for cavlc ram.\\0: Open the clock
 *  gate only when application writes or reads cavlc ram\\1: Force open the clock gate
 *  for cavlc ram
 */
#define H264_CAVLC_RAM_CLK_EN    (BIT(15))
#define H264_CAVLC_RAM_CLK_EN_M  (H264_CAVLC_RAM_CLK_EN_V << H264_CAVLC_RAM_CLK_EN_S)
#define H264_CAVLC_RAM_CLK_EN_V  0x00000001U
#define H264_CAVLC_RAM_CLK_EN_S  15
/** H264_IME_CLK_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to open the clock gate for ime.\\0: Open the clock gate
 *  only when ime work\\1: Force open the clock gate for ime
 */
#define H264_IME_CLK_EN    (BIT(16))
#define H264_IME_CLK_EN_M  (H264_IME_CLK_EN_V << H264_IME_CLK_EN_S)
#define H264_IME_CLK_EN_V  0x00000001U
#define H264_IME_CLK_EN_S  16
/** H264_FME_CLK_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to open the clock gate for fme.\\0: Open the clock gate
 *  only when fme work\\1: Force open the clock gate for fme
 */
#define H264_FME_CLK_EN    (BIT(17))
#define H264_FME_CLK_EN_M  (H264_FME_CLK_EN_V << H264_FME_CLK_EN_S)
#define H264_FME_CLK_EN_V  0x00000001U
#define H264_FME_CLK_EN_S  17
/** H264_MC_CLK_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to open the clock gate for mc.\\0: Open the clock gate
 *  only when mc work\\1: Force open the clock gate for mc
 */
#define H264_MC_CLK_EN    (BIT(18))
#define H264_MC_CLK_EN_M  (H264_MC_CLK_EN_V << H264_MC_CLK_EN_S)
#define H264_MC_CLK_EN_V  0x00000001U
#define H264_MC_CLK_EN_S  18
/** H264_INTERPOLATOR_CLK_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to open the clock gate for interpolator.\\0: Open the
 *  clock gate only when interpolator work\\1: Force open the clock gate for
 *  interpolator
 */
#define H264_INTERPOLATOR_CLK_EN    (BIT(19))
#define H264_INTERPOLATOR_CLK_EN_M  (H264_INTERPOLATOR_CLK_EN_V << H264_INTERPOLATOR_CLK_EN_S)
#define H264_INTERPOLATOR_CLK_EN_V  0x00000001U
#define H264_INTERPOLATOR_CLK_EN_S  19
/** H264_DB_CLK_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to open the clock gate for deblocking filter.\\0: Open
 *  the clock gate only when deblocking filter work\\1: Force open the clock gate for
 *  deblocking filter
 */
#define H264_DB_CLK_EN    (BIT(20))
#define H264_DB_CLK_EN_M  (H264_DB_CLK_EN_V << H264_DB_CLK_EN_S)
#define H264_DB_CLK_EN_V  0x00000001U
#define H264_DB_CLK_EN_S  20
/** H264_CLAVLC_CLK_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to open the clock gate for cavlc.\\0: Open the clock gate
 *  only when cavlc work\\1: Force open the clock gate for cavlc
 */
#define H264_CLAVLC_CLK_EN    (BIT(21))
#define H264_CLAVLC_CLK_EN_M  (H264_CLAVLC_CLK_EN_V << H264_CLAVLC_CLK_EN_S)
#define H264_CLAVLC_CLK_EN_V  0x00000001U
#define H264_CLAVLC_CLK_EN_S  21
/** H264_INTRA_CLK_EN : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to open the clock gate for intra.\\0: Open the clock gate
 *  only when intra work\\1: Force open the clock gate for intra
 */
#define H264_INTRA_CLK_EN    (BIT(22))
#define H264_INTRA_CLK_EN_M  (H264_INTRA_CLK_EN_V << H264_INTRA_CLK_EN_S)
#define H264_INTRA_CLK_EN_V  0x00000001U
#define H264_INTRA_CLK_EN_S  22
/** H264_DECI_CLK_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to open the clock gate for decimate.\\0: Open the clock
 *  gate only when decimate work\\1: Force open the clock gate for decimate
 */
#define H264_DECI_CLK_EN    (BIT(23))
#define H264_DECI_CLK_EN_M  (H264_DECI_CLK_EN_V << H264_DECI_CLK_EN_S)
#define H264_DECI_CLK_EN_V  0x00000001U
#define H264_DECI_CLK_EN_S  23
/** H264_BS_CLK_EN : R/W; bitpos: [24]; default: 0;
 *  Configures whether or not to open the clock gate for bs buffer.\\0: Open the clock
 *  gate only when bs buffer work\\1: Force open the clock gate for bs buffer
 */
#define H264_BS_CLK_EN    (BIT(24))
#define H264_BS_CLK_EN_M  (H264_BS_CLK_EN_V << H264_BS_CLK_EN_S)
#define H264_BS_CLK_EN_V  0x00000001U
#define H264_BS_CLK_EN_S  24
/** H264_MV_MERGE_CLK_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to open the clock gate for mv merge.\\0: Open the clock
 *  gate only when mv merge work\\1: Force open the clock gate for mv merge
 */
#define H264_MV_MERGE_CLK_EN    (BIT(25))
#define H264_MV_MERGE_CLK_EN_M  (H264_MV_MERGE_CLK_EN_V << H264_MV_MERGE_CLK_EN_S)
#define H264_MV_MERGE_CLK_EN_V  0x00000001U
#define H264_MV_MERGE_CLK_EN_S  25

/** H264_MV_MERGE_CONFIG_REG register
 *  Mv merge configuration register.
 */
#define H264_MV_MERGE_CONFIG_REG (DR_REG_H264_BASE + 0xd4)
/** H264_MV_MERGE_TYPE : R/W; bitpos: [1:0]; default: 0;
 *  Configure mv merge type.\\0: merge p16x16 mv\\1: merge min mv\\2: merge max mv\\3:
 *  not valid.
 */
#define H264_MV_MERGE_TYPE    0x00000003U
#define H264_MV_MERGE_TYPE_M  (H264_MV_MERGE_TYPE_V << H264_MV_MERGE_TYPE_S)
#define H264_MV_MERGE_TYPE_V  0x00000003U
#define H264_MV_MERGE_TYPE_S  0
/** H264_INT_MV_OUT_EN : R/W; bitpos: [2]; default: 0;
 *  Configure mv merge output integer part not zero mv or all part not zero mv.\\0:
 *  output all part not zero mv\\1: output integer part not zero mv.
 */
#define H264_INT_MV_OUT_EN    (BIT(2))
#define H264_INT_MV_OUT_EN_M  (H264_INT_MV_OUT_EN_V << H264_INT_MV_OUT_EN_S)
#define H264_INT_MV_OUT_EN_V  0x00000001U
#define H264_INT_MV_OUT_EN_S  2
/** H264_A_MV_MERGE_EN : R/W; bitpos: [3]; default: 0;
 *  Configure whether or not to enable video A mv merge.\\0: disable\\1: enable.
 */
#define H264_A_MV_MERGE_EN    (BIT(3))
#define H264_A_MV_MERGE_EN_M  (H264_A_MV_MERGE_EN_V << H264_A_MV_MERGE_EN_S)
#define H264_A_MV_MERGE_EN_V  0x00000001U
#define H264_A_MV_MERGE_EN_S  3
/** H264_B_MV_MERGE_EN : R/W; bitpos: [4]; default: 0;
 *  Configure whether or not to enable video B mv merge.\\0: disable\\1: enable.
 */
#define H264_B_MV_MERGE_EN    (BIT(4))
#define H264_B_MV_MERGE_EN_M  (H264_B_MV_MERGE_EN_V << H264_B_MV_MERGE_EN_S)
#define H264_B_MV_MERGE_EN_V  0x00000001U
#define H264_B_MV_MERGE_EN_S  4
/** H264_MB_VALID_NUM : RO; bitpos: [17:5]; default: 0;
 *  Represents the valid mb number of mv merge output.
 */
#define H264_MB_VALID_NUM    0x00001FFFU
#define H264_MB_VALID_NUM_M  (H264_MB_VALID_NUM_V << H264_MB_VALID_NUM_S)
#define H264_MB_VALID_NUM_V  0x00001FFFU
#define H264_MB_VALID_NUM_S  5

/** H264_DEBUG_DMA_SEL_REG register
 *  Debug H264 DMA select register
 */
#define H264_DEBUG_DMA_SEL_REG (DR_REG_H264_BASE + 0xd8)
/** H264_DBG_DMA_SEL : R/W; bitpos: [7:0]; default: 0;
 *  Every bit represents a dma in h264
 */
#define H264_DBG_DMA_SEL    0x000000FFU
#define H264_DBG_DMA_SEL_M  (H264_DBG_DMA_SEL_V << H264_DBG_DMA_SEL_S)
#define H264_DBG_DMA_SEL_V  0x000000FFU
#define H264_DBG_DMA_SEL_S  0

/** H264_SYS_STATUS_REG register
 *  System status register.
 */
#define H264_SYS_STATUS_REG (DR_REG_H264_BASE + 0xdc)
/** H264_FRAME_NUM : RO; bitpos: [8:0]; default: 0;
 *  Represents current frame number.
 */
#define H264_FRAME_NUM    0x000001FFU
#define H264_FRAME_NUM_M  (H264_FRAME_NUM_V << H264_FRAME_NUM_S)
#define H264_FRAME_NUM_V  0x000001FFU
#define H264_FRAME_NUM_S  0
/** H264_DUAL_STREAM_SEL : RO; bitpos: [9]; default: 0;
 *  Represents which register group is used for cur frame.\\0: Register group A is
 *  used\\1: Register group B is used.
 */
#define H264_DUAL_STREAM_SEL    (BIT(9))
#define H264_DUAL_STREAM_SEL_M  (H264_DUAL_STREAM_SEL_V << H264_DUAL_STREAM_SEL_S)
#define H264_DUAL_STREAM_SEL_V  0x00000001U
#define H264_DUAL_STREAM_SEL_S  9
/** H264_INTRA_FLAG : RO; bitpos: [10]; default: 0;
 *  Represents the type of current encoding frame.\\0: P frame\\1: I frame.
 */
#define H264_INTRA_FLAG    (BIT(10))
#define H264_INTRA_FLAG_M  (H264_INTRA_FLAG_V << H264_INTRA_FLAG_S)
#define H264_INTRA_FLAG_V  0x00000001U
#define H264_INTRA_FLAG_S  10

/** H264_FRAME_CODE_LENGTH_REG register
 *  Frame code byte length register.
 */
#define H264_FRAME_CODE_LENGTH_REG (DR_REG_H264_BASE + 0xe0)
/** H264_FRAME_CODE_LENGTH : RO; bitpos: [23:0]; default: 0;
 *  Represents current frame code byte length.
 */
#define H264_FRAME_CODE_LENGTH    0x00FFFFFFU
#define H264_FRAME_CODE_LENGTH_M  (H264_FRAME_CODE_LENGTH_V << H264_FRAME_CODE_LENGTH_S)
#define H264_FRAME_CODE_LENGTH_V  0x00FFFFFFU
#define H264_FRAME_CODE_LENGTH_S  0

/** H264_DEBUG_INFO0_REG register
 *  Debug information register0.
 */
#define H264_DEBUG_INFO0_REG (DR_REG_H264_BASE + 0xe4)
/** H264_TOP_CTRL_INTER_DEBUG_STATE : RO; bitpos: [3:0]; default: 0;
 *  Represents top_ctrl_inter module FSM info.
 */
#define H264_TOP_CTRL_INTER_DEBUG_STATE    0x0000000FU
#define H264_TOP_CTRL_INTER_DEBUG_STATE_M  (H264_TOP_CTRL_INTER_DEBUG_STATE_V << H264_TOP_CTRL_INTER_DEBUG_STATE_S)
#define H264_TOP_CTRL_INTER_DEBUG_STATE_V  0x0000000FU
#define H264_TOP_CTRL_INTER_DEBUG_STATE_S  0
/** H264_TOP_CTRL_INTRA_DEBUG_STATE : RO; bitpos: [6:4]; default: 0;
 *  Represents top_ctrl_intra module FSM info.
 */
#define H264_TOP_CTRL_INTRA_DEBUG_STATE    0x00000007U
#define H264_TOP_CTRL_INTRA_DEBUG_STATE_M  (H264_TOP_CTRL_INTRA_DEBUG_STATE_V << H264_TOP_CTRL_INTRA_DEBUG_STATE_S)
#define H264_TOP_CTRL_INTRA_DEBUG_STATE_V  0x00000007U
#define H264_TOP_CTRL_INTRA_DEBUG_STATE_S  4
/** H264_P_I_CMP_DEBUG_STATE : RO; bitpos: [9:7]; default: 0;
 *  Represents p_i_cmp module FSM info.
 */
#define H264_P_I_CMP_DEBUG_STATE    0x00000007U
#define H264_P_I_CMP_DEBUG_STATE_M  (H264_P_I_CMP_DEBUG_STATE_V << H264_P_I_CMP_DEBUG_STATE_S)
#define H264_P_I_CMP_DEBUG_STATE_V  0x00000007U
#define H264_P_I_CMP_DEBUG_STATE_S  7
/** H264_MVD_DEBUG_STATE : RO; bitpos: [12:10]; default: 0;
 *  Represents mvd module FSM info.
 */
#define H264_MVD_DEBUG_STATE    0x00000007U
#define H264_MVD_DEBUG_STATE_M  (H264_MVD_DEBUG_STATE_V << H264_MVD_DEBUG_STATE_S)
#define H264_MVD_DEBUG_STATE_V  0x00000007U
#define H264_MVD_DEBUG_STATE_S  10
/** H264_MC_CHROMA_IP_DEBUG_STATE : RO; bitpos: [13]; default: 0;
 *  Represents mc_chroma_ip module FSM info.
 */
#define H264_MC_CHROMA_IP_DEBUG_STATE    (BIT(13))
#define H264_MC_CHROMA_IP_DEBUG_STATE_M  (H264_MC_CHROMA_IP_DEBUG_STATE_V << H264_MC_CHROMA_IP_DEBUG_STATE_S)
#define H264_MC_CHROMA_IP_DEBUG_STATE_V  0x00000001U
#define H264_MC_CHROMA_IP_DEBUG_STATE_S  13
/** H264_INTRA_16X16_CHROMA_CTRL_DEBUG_STATE : RO; bitpos: [17:14]; default: 0;
 *  Represents intra_16x16_chroma_ctrl module FSM info.
 */
#define H264_INTRA_16X16_CHROMA_CTRL_DEBUG_STATE    0x0000000FU
#define H264_INTRA_16X16_CHROMA_CTRL_DEBUG_STATE_M  (H264_INTRA_16X16_CHROMA_CTRL_DEBUG_STATE_V << H264_INTRA_16X16_CHROMA_CTRL_DEBUG_STATE_S)
#define H264_INTRA_16X16_CHROMA_CTRL_DEBUG_STATE_V  0x0000000FU
#define H264_INTRA_16X16_CHROMA_CTRL_DEBUG_STATE_S  14
/** H264_INTRA_4X4_CTRL_DEBUG_STATE : RO; bitpos: [21:18]; default: 0;
 *  Represents intra_4x4_ctrl module FSM info.
 */
#define H264_INTRA_4X4_CTRL_DEBUG_STATE    0x0000000FU
#define H264_INTRA_4X4_CTRL_DEBUG_STATE_M  (H264_INTRA_4X4_CTRL_DEBUG_STATE_V << H264_INTRA_4X4_CTRL_DEBUG_STATE_S)
#define H264_INTRA_4X4_CTRL_DEBUG_STATE_V  0x0000000FU
#define H264_INTRA_4X4_CTRL_DEBUG_STATE_S  18
/** H264_INTRA_TOP_CTRL_DEBUG_STATE : RO; bitpos: [24:22]; default: 0;
 *  Represents intra_top_ctrl module FSM info.
 */
#define H264_INTRA_TOP_CTRL_DEBUG_STATE    0x00000007U
#define H264_INTRA_TOP_CTRL_DEBUG_STATE_M  (H264_INTRA_TOP_CTRL_DEBUG_STATE_V << H264_INTRA_TOP_CTRL_DEBUG_STATE_S)
#define H264_INTRA_TOP_CTRL_DEBUG_STATE_V  0x00000007U
#define H264_INTRA_TOP_CTRL_DEBUG_STATE_S  22
/** H264_IME_CTRL_DEBUG_STATE : RO; bitpos: [27:25]; default: 0;
 *  Represents ime_ctrl module FSM info.
 */
#define H264_IME_CTRL_DEBUG_STATE    0x00000007U
#define H264_IME_CTRL_DEBUG_STATE_M  (H264_IME_CTRL_DEBUG_STATE_V << H264_IME_CTRL_DEBUG_STATE_S)
#define H264_IME_CTRL_DEBUG_STATE_V  0x00000007U
#define H264_IME_CTRL_DEBUG_STATE_S  25

/** H264_DEBUG_INFO1_REG register
 *  Debug information register1.
 */
#define H264_DEBUG_INFO1_REG (DR_REG_H264_BASE + 0xe8)
/** H264_FME_CTRL_DEBUG_STATE : RO; bitpos: [2:0]; default: 0;
 *  Represents fme_ctrl module FSM info.
 */
#define H264_FME_CTRL_DEBUG_STATE    0x00000007U
#define H264_FME_CTRL_DEBUG_STATE_M  (H264_FME_CTRL_DEBUG_STATE_V << H264_FME_CTRL_DEBUG_STATE_S)
#define H264_FME_CTRL_DEBUG_STATE_V  0x00000007U
#define H264_FME_CTRL_DEBUG_STATE_S  0
/** H264_DECI_CALC_DEBUG_STATE : RO; bitpos: [4:3]; default: 0;
 *  Represents deci_calc module's FSM info. DEV use only.
 */
#define H264_DECI_CALC_DEBUG_STATE    0x00000003U
#define H264_DECI_CALC_DEBUG_STATE_M  (H264_DECI_CALC_DEBUG_STATE_V << H264_DECI_CALC_DEBUG_STATE_S)
#define H264_DECI_CALC_DEBUG_STATE_V  0x00000003U
#define H264_DECI_CALC_DEBUG_STATE_S  3
/** H264_DB_DEBUG_STATE : RO; bitpos: [7:5]; default: 0;
 *  Represents db module FSM info.
 */
#define H264_DB_DEBUG_STATE    0x00000007U
#define H264_DB_DEBUG_STATE_M  (H264_DB_DEBUG_STATE_V << H264_DB_DEBUG_STATE_S)
#define H264_DB_DEBUG_STATE_V  0x00000007U
#define H264_DB_DEBUG_STATE_S  5
/** H264_CAVLC_ENC_DEBUG_STATE : RO; bitpos: [11:8]; default: 0;
 *  Represents cavlc module enc FSM info.
 */
#define H264_CAVLC_ENC_DEBUG_STATE    0x0000000FU
#define H264_CAVLC_ENC_DEBUG_STATE_M  (H264_CAVLC_ENC_DEBUG_STATE_V << H264_CAVLC_ENC_DEBUG_STATE_S)
#define H264_CAVLC_ENC_DEBUG_STATE_V  0x0000000FU
#define H264_CAVLC_ENC_DEBUG_STATE_S  8
/** H264_CAVLC_SCAN_DEBUG_STATE : RO; bitpos: [15:12]; default: 0;
 *  Represents cavlc module scan FSM info.
 */
#define H264_CAVLC_SCAN_DEBUG_STATE    0x0000000FU
#define H264_CAVLC_SCAN_DEBUG_STATE_M  (H264_CAVLC_SCAN_DEBUG_STATE_V << H264_CAVLC_SCAN_DEBUG_STATE_S)
#define H264_CAVLC_SCAN_DEBUG_STATE_V  0x0000000FU
#define H264_CAVLC_SCAN_DEBUG_STATE_S  12
/** H264_CAVLC_CTRL_DEBUG_STATE : RO; bitpos: [17:16]; default: 0;
 *  Represents cavlc module ctrl FSM info.
 */
#define H264_CAVLC_CTRL_DEBUG_STATE    0x00000003U
#define H264_CAVLC_CTRL_DEBUG_STATE_M  (H264_CAVLC_CTRL_DEBUG_STATE_V << H264_CAVLC_CTRL_DEBUG_STATE_S)
#define H264_CAVLC_CTRL_DEBUG_STATE_V  0x00000003U
#define H264_CAVLC_CTRL_DEBUG_STATE_S  16
/** H264_BS_BUFFER_DEBUG_STATE : RO; bitpos: [18]; default: 0;
 *  Represents bs buffer overflow info.
 */
#define H264_BS_BUFFER_DEBUG_STATE    (BIT(18))
#define H264_BS_BUFFER_DEBUG_STATE_M  (H264_BS_BUFFER_DEBUG_STATE_V << H264_BS_BUFFER_DEBUG_STATE_S)
#define H264_BS_BUFFER_DEBUG_STATE_V  0x00000001U
#define H264_BS_BUFFER_DEBUG_STATE_S  18

/** H264_DEBUG_INFO2_REG register
 *  Debug information register2.
 */
#define H264_DEBUG_INFO2_REG (DR_REG_H264_BASE + 0xec)
/** H264_P_RC_DONE_DEBUG_FLAG : RO; bitpos: [0]; default: 0;
 *  Represents p rate ctrl done status.\\0: not done\\1: done.
 */
#define H264_P_RC_DONE_DEBUG_FLAG    (BIT(0))
#define H264_P_RC_DONE_DEBUG_FLAG_M  (H264_P_RC_DONE_DEBUG_FLAG_V << H264_P_RC_DONE_DEBUG_FLAG_S)
#define H264_P_RC_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_RC_DONE_DEBUG_FLAG_S  0
/** H264_P_P_I_CMP_DONE_DEBUG_FLAG : RO; bitpos: [1]; default: 0;
 *  Represents p p_i_cmp done status.\\0: not done\\1: done.
 */
#define H264_P_P_I_CMP_DONE_DEBUG_FLAG    (BIT(1))
#define H264_P_P_I_CMP_DONE_DEBUG_FLAG_M  (H264_P_P_I_CMP_DONE_DEBUG_FLAG_V << H264_P_P_I_CMP_DONE_DEBUG_FLAG_S)
#define H264_P_P_I_CMP_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_P_I_CMP_DONE_DEBUG_FLAG_S  1
/** H264_P_MV_MERGE_DONE_DEBUG_FLAG : RO; bitpos: [2]; default: 0;
 *  Represents p mv merge done status.\\0: not done\\1: done.
 */
#define H264_P_MV_MERGE_DONE_DEBUG_FLAG    (BIT(2))
#define H264_P_MV_MERGE_DONE_DEBUG_FLAG_M  (H264_P_MV_MERGE_DONE_DEBUG_FLAG_V << H264_P_MV_MERGE_DONE_DEBUG_FLAG_S)
#define H264_P_MV_MERGE_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_MV_MERGE_DONE_DEBUG_FLAG_S  2
/** H264_P_MOVE_ORI_DONE_DEBUG_FLAG : RO; bitpos: [3]; default: 0;
 *  Represents p move origin done status.\\0: not done\\1: done.
 */
#define H264_P_MOVE_ORI_DONE_DEBUG_FLAG    (BIT(3))
#define H264_P_MOVE_ORI_DONE_DEBUG_FLAG_M  (H264_P_MOVE_ORI_DONE_DEBUG_FLAG_V << H264_P_MOVE_ORI_DONE_DEBUG_FLAG_S)
#define H264_P_MOVE_ORI_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_MOVE_ORI_DONE_DEBUG_FLAG_S  3
/** H264_P_MC_DONE_DEBUG_FLAG : RO; bitpos: [4]; default: 0;
 *  Represents p mc done status.\\0: not done\\1: done.
 */
#define H264_P_MC_DONE_DEBUG_FLAG    (BIT(4))
#define H264_P_MC_DONE_DEBUG_FLAG_M  (H264_P_MC_DONE_DEBUG_FLAG_V << H264_P_MC_DONE_DEBUG_FLAG_S)
#define H264_P_MC_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_MC_DONE_DEBUG_FLAG_S  4
/** H264_P_IME_DONE_DEBUG_FLAG : RO; bitpos: [5]; default: 0;
 *  Represents p ime done status.\\0: not done\\1: done.
 */
#define H264_P_IME_DONE_DEBUG_FLAG    (BIT(5))
#define H264_P_IME_DONE_DEBUG_FLAG_M  (H264_P_IME_DONE_DEBUG_FLAG_V << H264_P_IME_DONE_DEBUG_FLAG_S)
#define H264_P_IME_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_IME_DONE_DEBUG_FLAG_S  5
/** H264_P_GET_ORI_DONE_DEBUG_FLAG : RO; bitpos: [6]; default: 0;
 *  Represents p get origin done status.\\0: not done\\1: done.
 */
#define H264_P_GET_ORI_DONE_DEBUG_FLAG    (BIT(6))
#define H264_P_GET_ORI_DONE_DEBUG_FLAG_M  (H264_P_GET_ORI_DONE_DEBUG_FLAG_V << H264_P_GET_ORI_DONE_DEBUG_FLAG_S)
#define H264_P_GET_ORI_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_GET_ORI_DONE_DEBUG_FLAG_S  6
/** H264_P_FME_DONE_DEBUG_FLAG : RO; bitpos: [7]; default: 0;
 *  Represents p fme done status.\\0: not done\\1: done.
 */
#define H264_P_FME_DONE_DEBUG_FLAG    (BIT(7))
#define H264_P_FME_DONE_DEBUG_FLAG_M  (H264_P_FME_DONE_DEBUG_FLAG_V << H264_P_FME_DONE_DEBUG_FLAG_S)
#define H264_P_FME_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_FME_DONE_DEBUG_FLAG_S  7
/** H264_P_FETCH_DONE_DEBUG_FLAG : RO; bitpos: [8]; default: 0;
 *  Represents p fetch done status.\\0: not done\\1: done.
 */
#define H264_P_FETCH_DONE_DEBUG_FLAG    (BIT(8))
#define H264_P_FETCH_DONE_DEBUG_FLAG_M  (H264_P_FETCH_DONE_DEBUG_FLAG_V << H264_P_FETCH_DONE_DEBUG_FLAG_S)
#define H264_P_FETCH_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_FETCH_DONE_DEBUG_FLAG_S  8
/** H264_P_DB_DONE_DEBUG_FLAG : RO; bitpos: [9]; default: 0;
 *  Represents p deblocking done status.\\0: not done\\1: done.
 */
#define H264_P_DB_DONE_DEBUG_FLAG    (BIT(9))
#define H264_P_DB_DONE_DEBUG_FLAG_M  (H264_P_DB_DONE_DEBUG_FLAG_V << H264_P_DB_DONE_DEBUG_FLAG_S)
#define H264_P_DB_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_DB_DONE_DEBUG_FLAG_S  9
/** H264_P_BS_BUF_DONE_DEBUG_FLAG : RO; bitpos: [10]; default: 0;
 *  Represents p bitstream buffer done status.\\0: not done\\1: done.
 */
#define H264_P_BS_BUF_DONE_DEBUG_FLAG    (BIT(10))
#define H264_P_BS_BUF_DONE_DEBUG_FLAG_M  (H264_P_BS_BUF_DONE_DEBUG_FLAG_V << H264_P_BS_BUF_DONE_DEBUG_FLAG_S)
#define H264_P_BS_BUF_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_P_BS_BUF_DONE_DEBUG_FLAG_S  10
/** H264_REF_MOVE_2MB_LINE_DONE_DEBUG_FLAG : RO; bitpos: [11]; default: 0;
 *  Represents dma move 2 ref mb line done status.\\0: not done\\1: done.
 */
#define H264_REF_MOVE_2MB_LINE_DONE_DEBUG_FLAG    (BIT(11))
#define H264_REF_MOVE_2MB_LINE_DONE_DEBUG_FLAG_M  (H264_REF_MOVE_2MB_LINE_DONE_DEBUG_FLAG_V << H264_REF_MOVE_2MB_LINE_DONE_DEBUG_FLAG_S)
#define H264_REF_MOVE_2MB_LINE_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_REF_MOVE_2MB_LINE_DONE_DEBUG_FLAG_S  11
/** H264_I_P_I_CMP_DONE_DEBUG_FLAG : RO; bitpos: [12]; default: 0;
 *  Represents I p_i_cmp done status.\\0: not done\\1: done.
 */
#define H264_I_P_I_CMP_DONE_DEBUG_FLAG    (BIT(12))
#define H264_I_P_I_CMP_DONE_DEBUG_FLAG_M  (H264_I_P_I_CMP_DONE_DEBUG_FLAG_V << H264_I_P_I_CMP_DONE_DEBUG_FLAG_S)
#define H264_I_P_I_CMP_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_I_P_I_CMP_DONE_DEBUG_FLAG_S  12
/** H264_I_MOVE_ORI_DONE_DEBUG_FLAG : RO; bitpos: [13]; default: 0;
 *  Represents I move origin done status.\\0: not done\\1: done.
 */
#define H264_I_MOVE_ORI_DONE_DEBUG_FLAG    (BIT(13))
#define H264_I_MOVE_ORI_DONE_DEBUG_FLAG_M  (H264_I_MOVE_ORI_DONE_DEBUG_FLAG_V << H264_I_MOVE_ORI_DONE_DEBUG_FLAG_S)
#define H264_I_MOVE_ORI_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_I_MOVE_ORI_DONE_DEBUG_FLAG_S  13
/** H264_I_GET_ORI_DONE_DEBUG_FLAG : RO; bitpos: [14]; default: 0;
 *  Represents I get origin done status.\\0: not done\\1: done.
 */
#define H264_I_GET_ORI_DONE_DEBUG_FLAG    (BIT(14))
#define H264_I_GET_ORI_DONE_DEBUG_FLAG_M  (H264_I_GET_ORI_DONE_DEBUG_FLAG_V << H264_I_GET_ORI_DONE_DEBUG_FLAG_S)
#define H264_I_GET_ORI_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_I_GET_ORI_DONE_DEBUG_FLAG_S  14
/** H264_I_EC_DONE_DEBUG_FLAG : RO; bitpos: [15]; default: 0;
 *  Represents I encoder done status.\\0: not done\\1: done.
 */
#define H264_I_EC_DONE_DEBUG_FLAG    (BIT(15))
#define H264_I_EC_DONE_DEBUG_FLAG_M  (H264_I_EC_DONE_DEBUG_FLAG_V << H264_I_EC_DONE_DEBUG_FLAG_S)
#define H264_I_EC_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_I_EC_DONE_DEBUG_FLAG_S  15
/** H264_I_DB_DONE_DEBUG_FLAG : RO; bitpos: [16]; default: 0;
 *  Represents I deblocking done status.\\0: not done\\1: done.
 */
#define H264_I_DB_DONE_DEBUG_FLAG    (BIT(16))
#define H264_I_DB_DONE_DEBUG_FLAG_M  (H264_I_DB_DONE_DEBUG_FLAG_V << H264_I_DB_DONE_DEBUG_FLAG_S)
#define H264_I_DB_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_I_DB_DONE_DEBUG_FLAG_S  16
/** H264_I_BS_BUF_DONE_DEBUG_FLAG : RO; bitpos: [17]; default: 0;
 *  Represents I bitstream buffer done status.\\0: not done\\1: done.
 */
#define H264_I_BS_BUF_DONE_DEBUG_FLAG    (BIT(17))
#define H264_I_BS_BUF_DONE_DEBUG_FLAG_M  (H264_I_BS_BUF_DONE_DEBUG_FLAG_V << H264_I_BS_BUF_DONE_DEBUG_FLAG_S)
#define H264_I_BS_BUF_DONE_DEBUG_FLAG_V  0x00000001U
#define H264_I_BS_BUF_DONE_DEBUG_FLAG_S  17

/** H264_DATE_REG register
 *  Version control register
 */
#define H264_DATE_REG (DR_REG_H264_BASE + 0xf0)
/** H264_LEDC_DATE : R/W; bitpos: [27:0]; default: 36717120;
 *  Configures the version.
 */
#define H264_LEDC_DATE    0x0FFFFFFFU
#define H264_LEDC_DATE_M  (H264_LEDC_DATE_V << H264_LEDC_DATE_S)
#define H264_LEDC_DATE_V  0x0FFFFFFFU
#define H264_LEDC_DATE_S  0

#ifdef __cplusplus
}
#endif
