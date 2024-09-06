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

/** ISP_VER_DATE_REG register
 *  version control register
 */
#define ISP_VER_DATE_REG (DR_REG_ISP_BASE + 0x0)
/** ISP_VER_DATA : R/W; bitpos: [31:0]; default: 539035144;
 *  csv version
 */
#define ISP_VER_DATA    0xFFFFFFFFU
#define ISP_VER_DATA_M  (ISP_VER_DATA_V << ISP_VER_DATA_S)
#define ISP_VER_DATA_V  0xFFFFFFFFU
#define ISP_VER_DATA_S  0

/** ISP_CLK_EN_REG register
 *  isp clk control register
 */
#define ISP_CLK_EN_REG (DR_REG_ISP_BASE + 0x4)
/** ISP_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  this bit configures the clk force on of isp reg. 0: disable, 1: enable
 */
#define ISP_CLK_EN    (BIT(0))
#define ISP_CLK_EN_M  (ISP_CLK_EN_V << ISP_CLK_EN_S)
#define ISP_CLK_EN_V  0x00000001U
#define ISP_CLK_EN_S  0
/** ISP_CLK_BLC_FORCE_ON : R/W; bitpos: [1]; default: 0;
 *  this bit configures the clk force on of blc. 0: disable, 1: enable
 */
#define ISP_CLK_BLC_FORCE_ON    (BIT(1))
#define ISP_CLK_BLC_FORCE_ON_M  (ISP_CLK_BLC_FORCE_ON_V << ISP_CLK_BLC_FORCE_ON_S)
#define ISP_CLK_BLC_FORCE_ON_V  0x00000001U
#define ISP_CLK_BLC_FORCE_ON_S  1
/** ISP_CLK_DPC_FORCE_ON : R/W; bitpos: [2]; default: 0;
 *  this bit configures the clk force on of dpc. 0: disable, 1: enable
 */
#define ISP_CLK_DPC_FORCE_ON    (BIT(2))
#define ISP_CLK_DPC_FORCE_ON_M  (ISP_CLK_DPC_FORCE_ON_V << ISP_CLK_DPC_FORCE_ON_S)
#define ISP_CLK_DPC_FORCE_ON_V  0x00000001U
#define ISP_CLK_DPC_FORCE_ON_S  2
/** ISP_CLK_BF_FORCE_ON : R/W; bitpos: [3]; default: 0;
 *  this bit configures the clk force on of bf. 0: disable, 1: enable
 */
#define ISP_CLK_BF_FORCE_ON    (BIT(3))
#define ISP_CLK_BF_FORCE_ON_M  (ISP_CLK_BF_FORCE_ON_V << ISP_CLK_BF_FORCE_ON_S)
#define ISP_CLK_BF_FORCE_ON_V  0x00000001U
#define ISP_CLK_BF_FORCE_ON_S  3
/** ISP_CLK_LSC_FORCE_ON : R/W; bitpos: [4]; default: 0;
 *  this bit configures the clk force on of lsc. 0: disable, 1: enable
 */
#define ISP_CLK_LSC_FORCE_ON    (BIT(4))
#define ISP_CLK_LSC_FORCE_ON_M  (ISP_CLK_LSC_FORCE_ON_V << ISP_CLK_LSC_FORCE_ON_S)
#define ISP_CLK_LSC_FORCE_ON_V  0x00000001U
#define ISP_CLK_LSC_FORCE_ON_S  4
/** ISP_CLK_DEMOSAIC_FORCE_ON : R/W; bitpos: [5]; default: 0;
 *  this bit configures the clk force on of demosaic. 0: disable, 1: enable
 */
#define ISP_CLK_DEMOSAIC_FORCE_ON    (BIT(5))
#define ISP_CLK_DEMOSAIC_FORCE_ON_M  (ISP_CLK_DEMOSAIC_FORCE_ON_V << ISP_CLK_DEMOSAIC_FORCE_ON_S)
#define ISP_CLK_DEMOSAIC_FORCE_ON_V  0x00000001U
#define ISP_CLK_DEMOSAIC_FORCE_ON_S  5
/** ISP_CLK_MEDIAN_FORCE_ON : R/W; bitpos: [6]; default: 0;
 *  this bit configures the clk force on of median. 0: disable, 1: enable
 */
#define ISP_CLK_MEDIAN_FORCE_ON    (BIT(6))
#define ISP_CLK_MEDIAN_FORCE_ON_M  (ISP_CLK_MEDIAN_FORCE_ON_V << ISP_CLK_MEDIAN_FORCE_ON_S)
#define ISP_CLK_MEDIAN_FORCE_ON_V  0x00000001U
#define ISP_CLK_MEDIAN_FORCE_ON_S  6
/** ISP_CLK_CCM_FORCE_ON : R/W; bitpos: [7]; default: 0;
 *  this bit configures the clk force on of ccm. 0: disable, 1: enable
 */
#define ISP_CLK_CCM_FORCE_ON    (BIT(7))
#define ISP_CLK_CCM_FORCE_ON_M  (ISP_CLK_CCM_FORCE_ON_V << ISP_CLK_CCM_FORCE_ON_S)
#define ISP_CLK_CCM_FORCE_ON_V  0x00000001U
#define ISP_CLK_CCM_FORCE_ON_S  7
/** ISP_CLK_GAMMA_FORCE_ON : R/W; bitpos: [8]; default: 0;
 *  this bit configures the clk force on of gamma. 0: disable, 1: enable
 */
#define ISP_CLK_GAMMA_FORCE_ON    (BIT(8))
#define ISP_CLK_GAMMA_FORCE_ON_M  (ISP_CLK_GAMMA_FORCE_ON_V << ISP_CLK_GAMMA_FORCE_ON_S)
#define ISP_CLK_GAMMA_FORCE_ON_V  0x00000001U
#define ISP_CLK_GAMMA_FORCE_ON_S  8
/** ISP_CLK_RGB2YUV_FORCE_ON : R/W; bitpos: [9]; default: 0;
 *  this bit configures the clk force on of rgb2yuv. 0: disable, 1: enable
 */
#define ISP_CLK_RGB2YUV_FORCE_ON    (BIT(9))
#define ISP_CLK_RGB2YUV_FORCE_ON_M  (ISP_CLK_RGB2YUV_FORCE_ON_V << ISP_CLK_RGB2YUV_FORCE_ON_S)
#define ISP_CLK_RGB2YUV_FORCE_ON_V  0x00000001U
#define ISP_CLK_RGB2YUV_FORCE_ON_S  9
/** ISP_CLK_SHARP_FORCE_ON : R/W; bitpos: [10]; default: 0;
 *  this bit configures the clk force on of sharp. 0: disable, 1: enable
 */
#define ISP_CLK_SHARP_FORCE_ON    (BIT(10))
#define ISP_CLK_SHARP_FORCE_ON_M  (ISP_CLK_SHARP_FORCE_ON_V << ISP_CLK_SHARP_FORCE_ON_S)
#define ISP_CLK_SHARP_FORCE_ON_V  0x00000001U
#define ISP_CLK_SHARP_FORCE_ON_S  10
/** ISP_CLK_COLOR_FORCE_ON : R/W; bitpos: [11]; default: 0;
 *  this bit configures the clk force on of color. 0: disable, 1: enable
 */
#define ISP_CLK_COLOR_FORCE_ON    (BIT(11))
#define ISP_CLK_COLOR_FORCE_ON_M  (ISP_CLK_COLOR_FORCE_ON_V << ISP_CLK_COLOR_FORCE_ON_S)
#define ISP_CLK_COLOR_FORCE_ON_V  0x00000001U
#define ISP_CLK_COLOR_FORCE_ON_S  11
/** ISP_CLK_YUV2RGB_FORCE_ON : R/W; bitpos: [12]; default: 0;
 *  this bit configures the clk force on of yuv2rgb. 0: disable, 1: enable
 */
#define ISP_CLK_YUV2RGB_FORCE_ON    (BIT(12))
#define ISP_CLK_YUV2RGB_FORCE_ON_M  (ISP_CLK_YUV2RGB_FORCE_ON_V << ISP_CLK_YUV2RGB_FORCE_ON_S)
#define ISP_CLK_YUV2RGB_FORCE_ON_V  0x00000001U
#define ISP_CLK_YUV2RGB_FORCE_ON_S  12
/** ISP_CLK_AE_FORCE_ON : R/W; bitpos: [13]; default: 0;
 *  this bit configures the clk force on of ae. 0: disable, 1: enable
 */
#define ISP_CLK_AE_FORCE_ON    (BIT(13))
#define ISP_CLK_AE_FORCE_ON_M  (ISP_CLK_AE_FORCE_ON_V << ISP_CLK_AE_FORCE_ON_S)
#define ISP_CLK_AE_FORCE_ON_V  0x00000001U
#define ISP_CLK_AE_FORCE_ON_S  13
/** ISP_CLK_AF_FORCE_ON : R/W; bitpos: [14]; default: 0;
 *  this bit configures the clk force on of af. 0: disable, 1: enable
 */
#define ISP_CLK_AF_FORCE_ON    (BIT(14))
#define ISP_CLK_AF_FORCE_ON_M  (ISP_CLK_AF_FORCE_ON_V << ISP_CLK_AF_FORCE_ON_S)
#define ISP_CLK_AF_FORCE_ON_V  0x00000001U
#define ISP_CLK_AF_FORCE_ON_S  14
/** ISP_CLK_AWB_FORCE_ON : R/W; bitpos: [15]; default: 0;
 *  this bit configures the clk force on of awb. 0: disable, 1: enable
 */
#define ISP_CLK_AWB_FORCE_ON    (BIT(15))
#define ISP_CLK_AWB_FORCE_ON_M  (ISP_CLK_AWB_FORCE_ON_V << ISP_CLK_AWB_FORCE_ON_S)
#define ISP_CLK_AWB_FORCE_ON_V  0x00000001U
#define ISP_CLK_AWB_FORCE_ON_S  15
/** ISP_CLK_HIST_FORCE_ON : R/W; bitpos: [16]; default: 0;
 *  this bit configures the clk force on of hist. 0: disable, 1: enable
 */
#define ISP_CLK_HIST_FORCE_ON    (BIT(16))
#define ISP_CLK_HIST_FORCE_ON_M  (ISP_CLK_HIST_FORCE_ON_V << ISP_CLK_HIST_FORCE_ON_S)
#define ISP_CLK_HIST_FORCE_ON_V  0x00000001U
#define ISP_CLK_HIST_FORCE_ON_S  16
/** ISP_CLK_MIPI_IDI_FORCE_ON : R/W; bitpos: [17]; default: 0;
 *  this bit configures the clk force on of mipi idi input. 0: disable, 1: enable
 */
#define ISP_CLK_MIPI_IDI_FORCE_ON    (BIT(17))
#define ISP_CLK_MIPI_IDI_FORCE_ON_M  (ISP_CLK_MIPI_IDI_FORCE_ON_V << ISP_CLK_MIPI_IDI_FORCE_ON_S)
#define ISP_CLK_MIPI_IDI_FORCE_ON_V  0x00000001U
#define ISP_CLK_MIPI_IDI_FORCE_ON_S  17
/** ISP_ISP_MEM_CLK_FORCE_ON : R/W; bitpos: [18]; default: 0;
 *  this bit configures the clk force on of all isp memory. 0: disable, 1: enable
 */
#define ISP_ISP_MEM_CLK_FORCE_ON    (BIT(18))
#define ISP_ISP_MEM_CLK_FORCE_ON_M  (ISP_ISP_MEM_CLK_FORCE_ON_V << ISP_ISP_MEM_CLK_FORCE_ON_S)
#define ISP_ISP_MEM_CLK_FORCE_ON_V  0x00000001U
#define ISP_ISP_MEM_CLK_FORCE_ON_S  18

/** ISP_CNTL_REG register
 *  isp module enable control register
 */
#define ISP_CNTL_REG (DR_REG_ISP_BASE + 0x8)
/** ISP_MIPI_DATA_EN : R/W; bitpos: [0]; default: 0;
 *  this bit configures mipi input data enable. 0: disable, 1: enable
 */
#define ISP_MIPI_DATA_EN    (BIT(0))
#define ISP_MIPI_DATA_EN_M  (ISP_MIPI_DATA_EN_V << ISP_MIPI_DATA_EN_S)
#define ISP_MIPI_DATA_EN_V  0x00000001U
#define ISP_MIPI_DATA_EN_S  0
/** ISP_ISP_EN : R/W; bitpos: [1]; default: 1;
 *  this bit configures isp global enable. 0: disable, 1: enable
 */
#define ISP_ISP_EN    (BIT(1))
#define ISP_ISP_EN_M  (ISP_ISP_EN_V << ISP_ISP_EN_S)
#define ISP_ISP_EN_V  0x00000001U
#define ISP_ISP_EN_S  1
/** ISP_BLC_EN : R/W; bitpos: [2]; default: 0;
 *  this bit configures blc enable. 0: disable, 1: enable
 */
#define ISP_BLC_EN    (BIT(2))
#define ISP_BLC_EN_M  (ISP_BLC_EN_V << ISP_BLC_EN_S)
#define ISP_BLC_EN_V  0x00000001U
#define ISP_BLC_EN_S  2
/** ISP_DPC_EN : R/W; bitpos: [3]; default: 0;
 *  this bit configures dpc enable. 0: disable, 1: enable
 */
#define ISP_DPC_EN    (BIT(3))
#define ISP_DPC_EN_M  (ISP_DPC_EN_V << ISP_DPC_EN_S)
#define ISP_DPC_EN_V  0x00000001U
#define ISP_DPC_EN_S  3
/** ISP_BF_EN : R/W; bitpos: [4]; default: 0;
 *  this bit configures bf enable. 0: disable, 1: enable
 */
#define ISP_BF_EN    (BIT(4))
#define ISP_BF_EN_M  (ISP_BF_EN_V << ISP_BF_EN_S)
#define ISP_BF_EN_V  0x00000001U
#define ISP_BF_EN_S  4
/** ISP_LSC_EN : R/W; bitpos: [5]; default: 0;
 *  this bit configures lsc enable. 0: disable, 1: enable
 */
#define ISP_LSC_EN    (BIT(5))
#define ISP_LSC_EN_M  (ISP_LSC_EN_V << ISP_LSC_EN_S)
#define ISP_LSC_EN_V  0x00000001U
#define ISP_LSC_EN_S  5
/** ISP_DEMOSAIC_EN : R/W; bitpos: [6]; default: 1;
 *  this bit configures demosaic enable. 0: disable, 1: enable
 */
#define ISP_DEMOSAIC_EN    (BIT(6))
#define ISP_DEMOSAIC_EN_M  (ISP_DEMOSAIC_EN_V << ISP_DEMOSAIC_EN_S)
#define ISP_DEMOSAIC_EN_V  0x00000001U
#define ISP_DEMOSAIC_EN_S  6
/** ISP_MEDIAN_EN : R/W; bitpos: [7]; default: 0;
 *  this bit configures median enable. 0: disable, 1: enable
 */
#define ISP_MEDIAN_EN    (BIT(7))
#define ISP_MEDIAN_EN_M  (ISP_MEDIAN_EN_V << ISP_MEDIAN_EN_S)
#define ISP_MEDIAN_EN_V  0x00000001U
#define ISP_MEDIAN_EN_S  7
/** ISP_CCM_EN : R/W; bitpos: [8]; default: 0;
 *  this bit configures ccm enable. 0: disable, 1: enable
 */
#define ISP_CCM_EN    (BIT(8))
#define ISP_CCM_EN_M  (ISP_CCM_EN_V << ISP_CCM_EN_S)
#define ISP_CCM_EN_V  0x00000001U
#define ISP_CCM_EN_S  8
/** ISP_GAMMA_EN : R/W; bitpos: [9]; default: 0;
 *  this bit configures gamma enable. 0: disable, 1: enable
 */
#define ISP_GAMMA_EN    (BIT(9))
#define ISP_GAMMA_EN_M  (ISP_GAMMA_EN_V << ISP_GAMMA_EN_S)
#define ISP_GAMMA_EN_V  0x00000001U
#define ISP_GAMMA_EN_S  9
/** ISP_RGB2YUV_EN : R/W; bitpos: [10]; default: 1;
 *  this bit configures rgb2yuv enable. 0: disable, 1: enable
 */
#define ISP_RGB2YUV_EN    (BIT(10))
#define ISP_RGB2YUV_EN_M  (ISP_RGB2YUV_EN_V << ISP_RGB2YUV_EN_S)
#define ISP_RGB2YUV_EN_V  0x00000001U
#define ISP_RGB2YUV_EN_S  10
/** ISP_SHARP_EN : R/W; bitpos: [11]; default: 0;
 *  this bit configures sharp enable. 0: disable, 1: enable
 */
#define ISP_SHARP_EN    (BIT(11))
#define ISP_SHARP_EN_M  (ISP_SHARP_EN_V << ISP_SHARP_EN_S)
#define ISP_SHARP_EN_V  0x00000001U
#define ISP_SHARP_EN_S  11
/** ISP_COLOR_EN : R/W; bitpos: [12]; default: 0;
 *  this bit configures color enable. 0: disable, 1: enable
 */
#define ISP_COLOR_EN    (BIT(12))
#define ISP_COLOR_EN_M  (ISP_COLOR_EN_V << ISP_COLOR_EN_S)
#define ISP_COLOR_EN_V  0x00000001U
#define ISP_COLOR_EN_S  12
/** ISP_YUV2RGB_EN : R/W; bitpos: [13]; default: 1;
 *  this bit configures yuv2rgb enable. 0: disable, 1: enable
 */
#define ISP_YUV2RGB_EN    (BIT(13))
#define ISP_YUV2RGB_EN_M  (ISP_YUV2RGB_EN_V << ISP_YUV2RGB_EN_S)
#define ISP_YUV2RGB_EN_V  0x00000001U
#define ISP_YUV2RGB_EN_S  13
/** ISP_AE_EN : R/W; bitpos: [14]; default: 0;
 *  this bit configures ae enable. 0: disable, 1: enable
 */
#define ISP_AE_EN    (BIT(14))
#define ISP_AE_EN_M  (ISP_AE_EN_V << ISP_AE_EN_S)
#define ISP_AE_EN_V  0x00000001U
#define ISP_AE_EN_S  14
/** ISP_AF_EN : R/W; bitpos: [15]; default: 0;
 *  this bit configures af enable. 0: disable, 1: enable
 */
#define ISP_AF_EN    (BIT(15))
#define ISP_AF_EN_M  (ISP_AF_EN_V << ISP_AF_EN_S)
#define ISP_AF_EN_V  0x00000001U
#define ISP_AF_EN_S  15
/** ISP_AWB_EN : R/W; bitpos: [16]; default: 0;
 *  this bit configures awb enable. 0: disable, 1: enable
 */
#define ISP_AWB_EN    (BIT(16))
#define ISP_AWB_EN_M  (ISP_AWB_EN_V << ISP_AWB_EN_S)
#define ISP_AWB_EN_V  0x00000001U
#define ISP_AWB_EN_S  16
/** ISP_HIST_EN : R/W; bitpos: [17]; default: 0;
 *  this bit configures hist enable. 0: disable, 1: enable
 */
#define ISP_HIST_EN    (BIT(17))
#define ISP_HIST_EN_M  (ISP_HIST_EN_V << ISP_HIST_EN_S)
#define ISP_HIST_EN_V  0x00000001U
#define ISP_HIST_EN_S  17
/** ISP_BYTE_ENDIAN_ORDER : R/W; bitpos: [24]; default: 0;
 *  select input idi data byte_endian_order when isp is bypass, 0: csi_data[31:0], 1:
 *  {[7:0], [15:8], [23:16], [31:24]}
 */
#define ISP_BYTE_ENDIAN_ORDER    (BIT(24))
#define ISP_BYTE_ENDIAN_ORDER_M  (ISP_BYTE_ENDIAN_ORDER_V << ISP_BYTE_ENDIAN_ORDER_S)
#define ISP_BYTE_ENDIAN_ORDER_V  0x00000001U
#define ISP_BYTE_ENDIAN_ORDER_S  24
/** ISP_ISP_DATA_TYPE : R/W; bitpos: [26:25]; default: 0;
 *  this field configures input data type, 0:RAW8 1:RAW10 2:RAW12
 */
#define ISP_ISP_DATA_TYPE    0x00000003U
#define ISP_ISP_DATA_TYPE_M  (ISP_ISP_DATA_TYPE_V << ISP_ISP_DATA_TYPE_S)
#define ISP_ISP_DATA_TYPE_V  0x00000003U
#define ISP_ISP_DATA_TYPE_S  25
/** ISP_ISP_IN_SRC : R/W; bitpos: [28:27]; default: 0;
 *  this field configures input data source, 0:CSI HOST 1:CAM 2:DMA
 */
#define ISP_ISP_IN_SRC    0x00000003U
#define ISP_ISP_IN_SRC_M  (ISP_ISP_IN_SRC_V << ISP_ISP_IN_SRC_S)
#define ISP_ISP_IN_SRC_V  0x00000003U
#define ISP_ISP_IN_SRC_S  27
/** ISP_ISP_OUT_TYPE : R/W; bitpos: [31:29]; default: 2;
 *  this field configures pixel output type, 0: RAW8 1: YUV422 2: RGB888 3: YUV420 4:
 *  RGB565
 */
#define ISP_ISP_OUT_TYPE    0x00000007U
#define ISP_ISP_OUT_TYPE_M  (ISP_ISP_OUT_TYPE_V << ISP_ISP_OUT_TYPE_S)
#define ISP_ISP_OUT_TYPE_V  0x00000007U
#define ISP_ISP_OUT_TYPE_S  29

/** ISP_HSYNC_CNT_REG register
 *  header hsync interval control register
 */
#define ISP_HSYNC_CNT_REG (DR_REG_ISP_BASE + 0xc)
/** ISP_HSYNC_CNT : R/W; bitpos: [7:0]; default: 7;
 *  this field configures the number of clock before hsync and after vsync and line_end
 *  when decodes pix data from idi to isp
 */
#define ISP_HSYNC_CNT    0x000000FFU
#define ISP_HSYNC_CNT_M  (ISP_HSYNC_CNT_V << ISP_HSYNC_CNT_S)
#define ISP_HSYNC_CNT_V  0x000000FFU
#define ISP_HSYNC_CNT_S  0

/** ISP_FRAME_CFG_REG register
 *  frame control parameter register
 */
#define ISP_FRAME_CFG_REG (DR_REG_ISP_BASE + 0x10)
/** ISP_VADR_NUM : R/W; bitpos: [11:0]; default: 480;
 *  this field configures input image size in y-direction, image row number - 1
 */
#define ISP_VADR_NUM    0x00000FFFU
#define ISP_VADR_NUM_M  (ISP_VADR_NUM_V << ISP_VADR_NUM_S)
#define ISP_VADR_NUM_V  0x00000FFFU
#define ISP_VADR_NUM_S  0
/** ISP_HADR_NUM : R/W; bitpos: [23:12]; default: 480;
 *  this field configures input image size in x-direction, image line number - 1
 */
#define ISP_HADR_NUM    0x00000FFFU
#define ISP_HADR_NUM_M  (ISP_HADR_NUM_V << ISP_HADR_NUM_S)
#define ISP_HADR_NUM_V  0x00000FFFU
#define ISP_HADR_NUM_S  12
/** ISP_BAYER_MODE : R/W; bitpos: [28:27]; default: 0;
 *  this field configures the bayer mode of input pixel. 00 : BG/GR    01 : GB/RG   10
 *  : GR/BG  11 : RG/GB
 */
#define ISP_BAYER_MODE    0x00000003U
#define ISP_BAYER_MODE_M  (ISP_BAYER_MODE_V << ISP_BAYER_MODE_S)
#define ISP_BAYER_MODE_V  0x00000003U
#define ISP_BAYER_MODE_S  27
/** ISP_HSYNC_START_EXIST : R/W; bitpos: [29]; default: 1;
 *  this bit configures the line end packet exist or not. 0: not exist, 1: exist
 */
#define ISP_HSYNC_START_EXIST    (BIT(29))
#define ISP_HSYNC_START_EXIST_M  (ISP_HSYNC_START_EXIST_V << ISP_HSYNC_START_EXIST_S)
#define ISP_HSYNC_START_EXIST_V  0x00000001U
#define ISP_HSYNC_START_EXIST_S  29
/** ISP_HSYNC_END_EXIST : R/W; bitpos: [30]; default: 1;
 *  this bit configures the line start packet exist or not. 0: not exist, 1: exist
 */
#define ISP_HSYNC_END_EXIST    (BIT(30))
#define ISP_HSYNC_END_EXIST_M  (ISP_HSYNC_END_EXIST_V << ISP_HSYNC_END_EXIST_S)
#define ISP_HSYNC_END_EXIST_V  0x00000001U
#define ISP_HSYNC_END_EXIST_S  30

/** ISP_CCM_COEF0_REG register
 *  ccm coef register 0
 */
#define ISP_CCM_COEF0_REG (DR_REG_ISP_BASE + 0x14)
/** ISP_CCM_RR : R/W; bitpos: [12:0]; default: 1856;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_RR    0x00001FFFU
#define ISP_CCM_RR_M  (ISP_CCM_RR_V << ISP_CCM_RR_S)
#define ISP_CCM_RR_V  0x00001FFFU
#define ISP_CCM_RR_S  0
/** ISP_CCM_RG : R/W; bitpos: [25:13]; default: 4736;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_RG    0x00001FFFU
#define ISP_CCM_RG_M  (ISP_CCM_RG_V << ISP_CCM_RG_S)
#define ISP_CCM_RG_V  0x00001FFFU
#define ISP_CCM_RG_S  13

/** ISP_CCM_COEF1_REG register
 *  ccm coef register 1
 */
#define ISP_CCM_COEF1_REG (DR_REG_ISP_BASE + 0x18)
/** ISP_CCM_RB : R/W; bitpos: [12:0]; default: 4288;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_RB    0x00001FFFU
#define ISP_CCM_RB_M  (ISP_CCM_RB_V << ISP_CCM_RB_S)
#define ISP_CCM_RB_V  0x00001FFFU
#define ISP_CCM_RB_S  0
/** ISP_CCM_GR : R/W; bitpos: [25:13]; default: 4416;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_GR    0x00001FFFU
#define ISP_CCM_GR_M  (ISP_CCM_GR_V << ISP_CCM_GR_S)
#define ISP_CCM_GR_V  0x00001FFFU
#define ISP_CCM_GR_S  13

/** ISP_CCM_COEF3_REG register
 *  ccm coef register 3
 */
#define ISP_CCM_COEF3_REG (DR_REG_ISP_BASE + 0x1c)
/** ISP_CCM_GG : R/W; bitpos: [12:0]; default: 1664;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_GG    0x00001FFFU
#define ISP_CCM_GG_M  (ISP_CCM_GG_V << ISP_CCM_GG_S)
#define ISP_CCM_GG_V  0x00001FFFU
#define ISP_CCM_GG_S  0
/** ISP_CCM_GB : R/W; bitpos: [25:13]; default: 4352;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_GB    0x00001FFFU
#define ISP_CCM_GB_M  (ISP_CCM_GB_V << ISP_CCM_GB_S)
#define ISP_CCM_GB_V  0x00001FFFU
#define ISP_CCM_GB_S  13

/** ISP_CCM_COEF4_REG register
 *  ccm coef register 4
 */
#define ISP_CCM_COEF4_REG (DR_REG_ISP_BASE + 0x20)
/** ISP_CCM_BR : R/W; bitpos: [12:0]; default: 4160;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_BR    0x00001FFFU
#define ISP_CCM_BR_M  (ISP_CCM_BR_V << ISP_CCM_BR_S)
#define ISP_CCM_BR_V  0x00001FFFU
#define ISP_CCM_BR_S  0
/** ISP_CCM_BG : R/W; bitpos: [25:13]; default: 4800;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_BG    0x00001FFFU
#define ISP_CCM_BG_M  (ISP_CCM_BG_V << ISP_CCM_BG_S)
#define ISP_CCM_BG_V  0x00001FFFU
#define ISP_CCM_BG_S  13

/** ISP_CCM_COEF5_REG register
 *  ccm coef register 5
 */
#define ISP_CCM_COEF5_REG (DR_REG_ISP_BASE + 0x24)
/** ISP_CCM_BB : R/W; bitpos: [12:0]; default: 1856;
 *  this field configures the color correction matrix coefficient
 */
#define ISP_CCM_BB    0x00001FFFU
#define ISP_CCM_BB_M  (ISP_CCM_BB_V << ISP_CCM_BB_S)
#define ISP_CCM_BB_V  0x00001FFFU
#define ISP_CCM_BB_S  0

/** ISP_BF_MATRIX_CTRL_REG register
 *  bf pix2matrix ctrl
 */
#define ISP_BF_MATRIX_CTRL_REG (DR_REG_ISP_BASE + 0x28)
/** ISP_BF_TAIL_PIXEN_PULSE_TL : R/W; bitpos: [7:0]; default: 0;
 *  matrix tail pixen low level threshold, should not to large to prevent expanding to
 *  next frame, only reg_bf_tail_pixen_pulse_th!=0 and reg_bf_tail_pixen_pulse_tl!=0
 *  and reg_bf_tail_pixen_pulse_th < reg_bf_tail_pixen_pulse_tl will enable tail pulse
 *  function
 */
#define ISP_BF_TAIL_PIXEN_PULSE_TL    0x000000FFU
#define ISP_BF_TAIL_PIXEN_PULSE_TL_M  (ISP_BF_TAIL_PIXEN_PULSE_TL_V << ISP_BF_TAIL_PIXEN_PULSE_TL_S)
#define ISP_BF_TAIL_PIXEN_PULSE_TL_V  0x000000FFU
#define ISP_BF_TAIL_PIXEN_PULSE_TL_S  0
/** ISP_BF_TAIL_PIXEN_PULSE_TH : R/W; bitpos: [15:8]; default: 0;
 *  matrix tail pixen high level threshold, must < hnum-1, only
 *  reg_bf_tail_pixen_pulse_th!=0 and reg_bf_tail_pixen_pulse_tl!=0 and
 *  reg_bf_tail_pixen_pulse_th < reg_bf_tail_pixen_pulse_tl will enable tail pulse
 *  function
 */
#define ISP_BF_TAIL_PIXEN_PULSE_TH    0x000000FFU
#define ISP_BF_TAIL_PIXEN_PULSE_TH_M  (ISP_BF_TAIL_PIXEN_PULSE_TH_V << ISP_BF_TAIL_PIXEN_PULSE_TH_S)
#define ISP_BF_TAIL_PIXEN_PULSE_TH_V  0x000000FFU
#define ISP_BF_TAIL_PIXEN_PULSE_TH_S  8
/** ISP_BF_PADDING_DATA : R/W; bitpos: [23:16]; default: 0;
 *  this field configures bf matrix padding data
 */
#define ISP_BF_PADDING_DATA    0x000000FFU
#define ISP_BF_PADDING_DATA_M  (ISP_BF_PADDING_DATA_V << ISP_BF_PADDING_DATA_S)
#define ISP_BF_PADDING_DATA_V  0x000000FFU
#define ISP_BF_PADDING_DATA_S  16
/** ISP_BF_PADDING_MODE : R/W; bitpos: [24]; default: 0;
 *  this bit configures the padding mode of bf matrix.  0: use pixel in image to do
 *  padding   1: use reg_padding_data to do padding
 */
#define ISP_BF_PADDING_MODE    (BIT(24))
#define ISP_BF_PADDING_MODE_M  (ISP_BF_PADDING_MODE_V << ISP_BF_PADDING_MODE_S)
#define ISP_BF_PADDING_MODE_V  0x00000001U
#define ISP_BF_PADDING_MODE_S  24

/** ISP_BF_SIGMA_REG register
 *  bf denoising level control register
 */
#define ISP_BF_SIGMA_REG (DR_REG_ISP_BASE + 0x2c)
/** ISP_SIGMA : R/W; bitpos: [5:0]; default: 2;
 *  this field configures the bayer denoising level, valid data from 2 to 20
 */
#define ISP_SIGMA    0x0000003FU
#define ISP_SIGMA_M  (ISP_SIGMA_V << ISP_SIGMA_S)
#define ISP_SIGMA_V  0x0000003FU
#define ISP_SIGMA_S  0

/** ISP_BF_GAU0_REG register
 *  bf gau template register 0
 */
#define ISP_BF_GAU0_REG (DR_REG_ISP_BASE + 0x30)
/** ISP_GAU_TEMPLATE21 : R/W; bitpos: [3:0]; default: 15;
 *  this field configures index 21 of gaussian template
 */
#define ISP_GAU_TEMPLATE21    0x0000000FU
#define ISP_GAU_TEMPLATE21_M  (ISP_GAU_TEMPLATE21_V << ISP_GAU_TEMPLATE21_S)
#define ISP_GAU_TEMPLATE21_V  0x0000000FU
#define ISP_GAU_TEMPLATE21_S  0
/** ISP_GAU_TEMPLATE20 : R/W; bitpos: [7:4]; default: 15;
 *  this field configures index 20 of gaussian template
 */
#define ISP_GAU_TEMPLATE20    0x0000000FU
#define ISP_GAU_TEMPLATE20_M  (ISP_GAU_TEMPLATE20_V << ISP_GAU_TEMPLATE20_S)
#define ISP_GAU_TEMPLATE20_V  0x0000000FU
#define ISP_GAU_TEMPLATE20_S  4
/** ISP_GAU_TEMPLATE12 : R/W; bitpos: [11:8]; default: 15;
 *  this field configures index 12 of gaussian template
 */
#define ISP_GAU_TEMPLATE12    0x0000000FU
#define ISP_GAU_TEMPLATE12_M  (ISP_GAU_TEMPLATE12_V << ISP_GAU_TEMPLATE12_S)
#define ISP_GAU_TEMPLATE12_V  0x0000000FU
#define ISP_GAU_TEMPLATE12_S  8
/** ISP_GAU_TEMPLATE11 : R/W; bitpos: [15:12]; default: 15;
 *  this field configures index 11 of gaussian template
 */
#define ISP_GAU_TEMPLATE11    0x0000000FU
#define ISP_GAU_TEMPLATE11_M  (ISP_GAU_TEMPLATE11_V << ISP_GAU_TEMPLATE11_S)
#define ISP_GAU_TEMPLATE11_V  0x0000000FU
#define ISP_GAU_TEMPLATE11_S  12
/** ISP_GAU_TEMPLATE10 : R/W; bitpos: [19:16]; default: 15;
 *  this field configures index 10 of gaussian template
 */
#define ISP_GAU_TEMPLATE10    0x0000000FU
#define ISP_GAU_TEMPLATE10_M  (ISP_GAU_TEMPLATE10_V << ISP_GAU_TEMPLATE10_S)
#define ISP_GAU_TEMPLATE10_V  0x0000000FU
#define ISP_GAU_TEMPLATE10_S  16
/** ISP_GAU_TEMPLATE02 : R/W; bitpos: [23:20]; default: 15;
 *  this field configures index 02 of gaussian template
 */
#define ISP_GAU_TEMPLATE02    0x0000000FU
#define ISP_GAU_TEMPLATE02_M  (ISP_GAU_TEMPLATE02_V << ISP_GAU_TEMPLATE02_S)
#define ISP_GAU_TEMPLATE02_V  0x0000000FU
#define ISP_GAU_TEMPLATE02_S  20
/** ISP_GAU_TEMPLATE01 : R/W; bitpos: [27:24]; default: 15;
 *  this field configures index 01 of gaussian template
 */
#define ISP_GAU_TEMPLATE01    0x0000000FU
#define ISP_GAU_TEMPLATE01_M  (ISP_GAU_TEMPLATE01_V << ISP_GAU_TEMPLATE01_S)
#define ISP_GAU_TEMPLATE01_V  0x0000000FU
#define ISP_GAU_TEMPLATE01_S  24
/** ISP_GAU_TEMPLATE00 : R/W; bitpos: [31:28]; default: 15;
 *  this field configures index 00 of gaussian template
 */
#define ISP_GAU_TEMPLATE00    0x0000000FU
#define ISP_GAU_TEMPLATE00_M  (ISP_GAU_TEMPLATE00_V << ISP_GAU_TEMPLATE00_S)
#define ISP_GAU_TEMPLATE00_V  0x0000000FU
#define ISP_GAU_TEMPLATE00_S  28

/** ISP_BF_GAU1_REG register
 *  bf gau template register 1
 */
#define ISP_BF_GAU1_REG (DR_REG_ISP_BASE + 0x34)
/** ISP_GAU_TEMPLATE22 : R/W; bitpos: [3:0]; default: 15;
 *  this field configures index 22 of gaussian template
 */
#define ISP_GAU_TEMPLATE22    0x0000000FU
#define ISP_GAU_TEMPLATE22_M  (ISP_GAU_TEMPLATE22_V << ISP_GAU_TEMPLATE22_S)
#define ISP_GAU_TEMPLATE22_V  0x0000000FU
#define ISP_GAU_TEMPLATE22_S  0

/** ISP_DPC_CTRL_REG register
 *  DPC mode control register
 */
#define ISP_DPC_CTRL_REG (DR_REG_ISP_BASE + 0x38)
/** ISP_DPC_CHECK_EN : R/W; bitpos: [0]; default: 0;
 *  this bit configures the check mode enable. 0: disable, 1: enable
 */
#define ISP_DPC_CHECK_EN    (BIT(0))
#define ISP_DPC_CHECK_EN_M  (ISP_DPC_CHECK_EN_V << ISP_DPC_CHECK_EN_S)
#define ISP_DPC_CHECK_EN_V  0x00000001U
#define ISP_DPC_CHECK_EN_S  0
/** ISP_STA_EN : R/W; bitpos: [1]; default: 0;
 *  this bit configures the sta dpc enable. 0: disable, 1: enable
 */
#define ISP_STA_EN    (BIT(1))
#define ISP_STA_EN_M  (ISP_STA_EN_V << ISP_STA_EN_S)
#define ISP_STA_EN_V  0x00000001U
#define ISP_STA_EN_S  1
/** ISP_DYN_EN : R/W; bitpos: [2]; default: 1;
 *  this bit configures the dyn dpc enable. 0: disable, 1: enable
 */
#define ISP_DYN_EN    (BIT(2))
#define ISP_DYN_EN_M  (ISP_DYN_EN_V << ISP_DYN_EN_S)
#define ISP_DYN_EN_V  0x00000001U
#define ISP_DYN_EN_S  2
/** ISP_DPC_BLACK_EN : R/W; bitpos: [3]; default: 0;
 *  this bit configures input image type select when in check mode, 0: white img, 1:
 *  black img
 */
#define ISP_DPC_BLACK_EN    (BIT(3))
#define ISP_DPC_BLACK_EN_M  (ISP_DPC_BLACK_EN_V << ISP_DPC_BLACK_EN_S)
#define ISP_DPC_BLACK_EN_V  0x00000001U
#define ISP_DPC_BLACK_EN_S  3
/** ISP_DPC_METHOD_SEL : R/W; bitpos: [4]; default: 0;
 *  this bit configures dyn dpc method select. 0: simple method, 1: hard method
 */
#define ISP_DPC_METHOD_SEL    (BIT(4))
#define ISP_DPC_METHOD_SEL_M  (ISP_DPC_METHOD_SEL_V << ISP_DPC_METHOD_SEL_S)
#define ISP_DPC_METHOD_SEL_V  0x00000001U
#define ISP_DPC_METHOD_SEL_S  4
/** ISP_DPC_CHECK_OD_EN : R/W; bitpos: [5]; default: 0;
 *  this bit configures output pixel data when in check mode or not. 0: no data output,
 *  1: data output
 */
#define ISP_DPC_CHECK_OD_EN    (BIT(5))
#define ISP_DPC_CHECK_OD_EN_M  (ISP_DPC_CHECK_OD_EN_V << ISP_DPC_CHECK_OD_EN_S)
#define ISP_DPC_CHECK_OD_EN_V  0x00000001U
#define ISP_DPC_CHECK_OD_EN_S  5

/** ISP_DPC_CONF_REG register
 *  DPC parameter config register
 */
#define ISP_DPC_CONF_REG (DR_REG_ISP_BASE + 0x3c)
/** ISP_DPC_THRESHOLD_L : R/W; bitpos: [7:0]; default: 48;
 *  this bit configures the threshold to detect black img in check mode, or the low
 *  threshold(use 8 bit 0~255) in dyn method 0, or the low threshold factor (use 5 bit
 *  10000-> 16/16, 00001->1/16, 0/16~16/16) in dyn method 1
 */
#define ISP_DPC_THRESHOLD_L    0x000000FFU
#define ISP_DPC_THRESHOLD_L_M  (ISP_DPC_THRESHOLD_L_V << ISP_DPC_THRESHOLD_L_S)
#define ISP_DPC_THRESHOLD_L_V  0x000000FFU
#define ISP_DPC_THRESHOLD_L_S  0
/** ISP_DPC_THRESHOLD_H : R/W; bitpos: [15:8]; default: 48;
 *  this bit configures the threshold to detect white img in check mode, or the high
 *  threshold(use 8 bit 0~255) in dyn method 0, or the high threshold factor (use 5 bit
 *  10000-> 16/16, 00001->1/16, 0/16~16/16) in dyn method 1
 */
#define ISP_DPC_THRESHOLD_H    0x000000FFU
#define ISP_DPC_THRESHOLD_H_M  (ISP_DPC_THRESHOLD_H_V << ISP_DPC_THRESHOLD_H_S)
#define ISP_DPC_THRESHOLD_H_V  0x000000FFU
#define ISP_DPC_THRESHOLD_H_S  8
/** ISP_DPC_FACTOR_DARK : R/W; bitpos: [21:16]; default: 16;
 *  this field configures the dynamic correction method 1 dark   factor
 */
#define ISP_DPC_FACTOR_DARK    0x0000003FU
#define ISP_DPC_FACTOR_DARK_M  (ISP_DPC_FACTOR_DARK_V << ISP_DPC_FACTOR_DARK_S)
#define ISP_DPC_FACTOR_DARK_V  0x0000003FU
#define ISP_DPC_FACTOR_DARK_S  16
/** ISP_DPC_FACTOR_BRIG : R/W; bitpos: [27:22]; default: 16;
 *  this field configures the dynamic correction method 1 bright factor
 */
#define ISP_DPC_FACTOR_BRIG    0x0000003FU
#define ISP_DPC_FACTOR_BRIG_M  (ISP_DPC_FACTOR_BRIG_V << ISP_DPC_FACTOR_BRIG_S)
#define ISP_DPC_FACTOR_BRIG_V  0x0000003FU
#define ISP_DPC_FACTOR_BRIG_S  22

/** ISP_DPC_MATRIX_CTRL_REG register
 *  dpc pix2matrix ctrl
 */
#define ISP_DPC_MATRIX_CTRL_REG (DR_REG_ISP_BASE + 0x40)
/** ISP_DPC_TAIL_PIXEN_PULSE_TL : R/W; bitpos: [7:0]; default: 0;
 *  matrix tail pixen low level threshold,  should not to large to prevent expanding to
 *  next frame, only reg_dpc_tail_pixen_pulse_th!=0 and reg_dpc_tail_pixen_pulse_tl!=0
 *  and reg_dpc_tail_pixen_pulse_th < reg_dpc_tail_pixen_pulse_tl will enable tail
 *  pulse function
 */
#define ISP_DPC_TAIL_PIXEN_PULSE_TL    0x000000FFU
#define ISP_DPC_TAIL_PIXEN_PULSE_TL_M  (ISP_DPC_TAIL_PIXEN_PULSE_TL_V << ISP_DPC_TAIL_PIXEN_PULSE_TL_S)
#define ISP_DPC_TAIL_PIXEN_PULSE_TL_V  0x000000FFU
#define ISP_DPC_TAIL_PIXEN_PULSE_TL_S  0
/** ISP_DPC_TAIL_PIXEN_PULSE_TH : R/W; bitpos: [15:8]; default: 0;
 *  matrix tail pixen high level threshold, must < hnum-1, only
 *  reg_dpc_tail_pixen_pulse_th!=0 and reg_dpc_tail_pixen_pulse_tl!=0 and
 *  reg_dpc_tail_pixen_pulse_th < reg_dpc_tail_pixen_pulse_tl will enable tail pulse
 *  function
 */
#define ISP_DPC_TAIL_PIXEN_PULSE_TH    0x000000FFU
#define ISP_DPC_TAIL_PIXEN_PULSE_TH_M  (ISP_DPC_TAIL_PIXEN_PULSE_TH_V << ISP_DPC_TAIL_PIXEN_PULSE_TH_S)
#define ISP_DPC_TAIL_PIXEN_PULSE_TH_V  0x000000FFU
#define ISP_DPC_TAIL_PIXEN_PULSE_TH_S  8
/** ISP_DPC_PADDING_DATA : R/W; bitpos: [23:16]; default: 0;
 *  this field configures dpc matrix padding data
 */
#define ISP_DPC_PADDING_DATA    0x000000FFU
#define ISP_DPC_PADDING_DATA_M  (ISP_DPC_PADDING_DATA_V << ISP_DPC_PADDING_DATA_S)
#define ISP_DPC_PADDING_DATA_V  0x000000FFU
#define ISP_DPC_PADDING_DATA_S  16
/** ISP_DPC_PADDING_MODE : R/W; bitpos: [24]; default: 0;
 *  this bit configures the padding mode of dpc matrix.  0: use pixel in image to do
 *  padding   1: use reg_padding_data to do padding
 */
#define ISP_DPC_PADDING_MODE    (BIT(24))
#define ISP_DPC_PADDING_MODE_M  (ISP_DPC_PADDING_MODE_V << ISP_DPC_PADDING_MODE_S)
#define ISP_DPC_PADDING_MODE_V  0x00000001U
#define ISP_DPC_PADDING_MODE_S  24

/** ISP_DPC_DEADPIX_CNT_REG register
 *  DPC dead-pix number register
 */
#define ISP_DPC_DEADPIX_CNT_REG (DR_REG_ISP_BASE + 0x44)
/** ISP_DPC_DEADPIX_CNT : RO; bitpos: [9:0]; default: 0;
 *  this field represents the dead pixel count
 */
#define ISP_DPC_DEADPIX_CNT    0x000003FFU
#define ISP_DPC_DEADPIX_CNT_M  (ISP_DPC_DEADPIX_CNT_V << ISP_DPC_DEADPIX_CNT_S)
#define ISP_DPC_DEADPIX_CNT_V  0x000003FFU
#define ISP_DPC_DEADPIX_CNT_S  0

/** ISP_LUT_CMD_REG register
 *  LUT command register
 */
#define ISP_LUT_CMD_REG (DR_REG_ISP_BASE + 0x48)
/** ISP_LUT_ADDR : WT; bitpos: [11:0]; default: 0;
 *  this field configures the lut access addr, when select lsc lut, [11:10]:00 sel gb_b
 *  lut, 01 sel r_gr lut
 */
#define ISP_LUT_ADDR    0x00000FFFU
#define ISP_LUT_ADDR_M  (ISP_LUT_ADDR_V << ISP_LUT_ADDR_S)
#define ISP_LUT_ADDR_V  0x00000FFFU
#define ISP_LUT_ADDR_S  0
/** ISP_LUT_NUM : WT; bitpos: [15:12]; default: 0;
 *  this field configures the lut selection. 0000:LSC LUT 0001:DPC LUT
 */
#define ISP_LUT_NUM    0x0000000FU
#define ISP_LUT_NUM_M  (ISP_LUT_NUM_V << ISP_LUT_NUM_S)
#define ISP_LUT_NUM_V  0x0000000FU
#define ISP_LUT_NUM_S  12
/** ISP_LUT_CMD : WT; bitpos: [16]; default: 0;
 *  this bit configures the access event of lut. 0:rd 1: wr
 */
#define ISP_LUT_CMD    (BIT(16))
#define ISP_LUT_CMD_M  (ISP_LUT_CMD_V << ISP_LUT_CMD_S)
#define ISP_LUT_CMD_V  0x00000001U
#define ISP_LUT_CMD_S  16

/** ISP_LUT_WDATA_REG register
 *  LUT write data register
 */
#define ISP_LUT_WDATA_REG (DR_REG_ISP_BASE + 0x4c)
/** ISP_LUT_WDATA : R/W; bitpos: [31:0]; default: 0;
 *  this field configures the write data of lut. please initial ISP_LUT_WDATA before
 *  write ISP_LUT_CMD register
 */
#define ISP_LUT_WDATA    0xFFFFFFFFU
#define ISP_LUT_WDATA_M  (ISP_LUT_WDATA_V << ISP_LUT_WDATA_S)
#define ISP_LUT_WDATA_V  0xFFFFFFFFU
#define ISP_LUT_WDATA_S  0

/** ISP_LUT_RDATA_REG register
 *  LUT read data register
 */
#define ISP_LUT_RDATA_REG (DR_REG_ISP_BASE + 0x50)
/** ISP_LUT_RDATA : RO; bitpos: [31:0]; default: 0;
 *  this field represents the read data of lut. read ISP_LUT_RDATA after write
 *  ISP_LUT_CMD register
 */
#define ISP_LUT_RDATA    0xFFFFFFFFU
#define ISP_LUT_RDATA_M  (ISP_LUT_RDATA_V << ISP_LUT_RDATA_S)
#define ISP_LUT_RDATA_V  0xFFFFFFFFU
#define ISP_LUT_RDATA_S  0

/** ISP_LSC_TABLESIZE_REG register
 *  LSC point in x-direction
 */
#define ISP_LSC_TABLESIZE_REG (DR_REG_ISP_BASE + 0x54)
/** ISP_LSC_XTABLESIZE : R/W; bitpos: [4:0]; default: 31;
 *  this field configures lsc table size in x-direction
 */
#define ISP_LSC_XTABLESIZE    0x0000001FU
#define ISP_LSC_XTABLESIZE_M  (ISP_LSC_XTABLESIZE_V << ISP_LSC_XTABLESIZE_S)
#define ISP_LSC_XTABLESIZE_V  0x0000001FU
#define ISP_LSC_XTABLESIZE_S  0

/** ISP_DEMOSAIC_MATRIX_CTRL_REG register
 *  demosaic pix2matrix ctrl
 */
#define ISP_DEMOSAIC_MATRIX_CTRL_REG (DR_REG_ISP_BASE + 0x58)
/** ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TL : R/W; bitpos: [7:0]; default: 0;
 *  matrix tail pixen low level threshold,  should not to large to prevent expanding to
 *  next frame, only reg_demosaic_tail_pixen_pulse_th!=0 and
 *  reg_demosaic_tail_pixen_pulse_tl!=0 and reg_demosaic_tail_pixen_pulse_th <
 *  reg_demosaic_tail_pixen_pulse_tl will enable tail pulse function
 */
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TL    0x000000FFU
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TL_M  (ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TL_V << ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TL_S)
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TL_V  0x000000FFU
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TL_S  0
/** ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TH : R/W; bitpos: [15:8]; default: 0;
 *  matrix tail pixen high level threshold, must < hnum-1, only
 *  reg_demosaic_tail_pixen_pulse_th!=0 and reg_demosaic_tail_pixen_pulse_tl!=0 and
 *  reg_demosaic_tail_pixen_pulse_th < reg_demosaic_tail_pixen_pulse_tl will enable
 *  tail pulse function
 */
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TH    0x000000FFU
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TH_M  (ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TH_V << ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TH_S)
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TH_V  0x000000FFU
#define ISP_DEMOSAIC_TAIL_PIXEN_PULSE_TH_S  8
/** ISP_DEMOSAIC_PADDING_DATA : R/W; bitpos: [23:16]; default: 0;
 *  this field configures demosaic matrix padding data
 */
#define ISP_DEMOSAIC_PADDING_DATA    0x000000FFU
#define ISP_DEMOSAIC_PADDING_DATA_M  (ISP_DEMOSAIC_PADDING_DATA_V << ISP_DEMOSAIC_PADDING_DATA_S)
#define ISP_DEMOSAIC_PADDING_DATA_V  0x000000FFU
#define ISP_DEMOSAIC_PADDING_DATA_S  16
/** ISP_DEMOSAIC_PADDING_MODE : R/W; bitpos: [24]; default: 0;
 *  this bit configures the padding mode of demosaic matrix.  0: use pixel in image to
 *  do padding   1: use reg_padding_data to do padding
 */
#define ISP_DEMOSAIC_PADDING_MODE    (BIT(24))
#define ISP_DEMOSAIC_PADDING_MODE_M  (ISP_DEMOSAIC_PADDING_MODE_V << ISP_DEMOSAIC_PADDING_MODE_S)
#define ISP_DEMOSAIC_PADDING_MODE_V  0x00000001U
#define ISP_DEMOSAIC_PADDING_MODE_S  24

/** ISP_DEMOSAIC_GRAD_RATIO_REG register
 *  demosaic gradient select ratio
 */
#define ISP_DEMOSAIC_GRAD_RATIO_REG (DR_REG_ISP_BASE + 0x5c)
/** ISP_DEMOSAIC_GRAD_RATIO : R/W; bitpos: [5:0]; default: 16;
 *  this field configures demosaic gradient select ratio
 */
#define ISP_DEMOSAIC_GRAD_RATIO    0x0000003FU
#define ISP_DEMOSAIC_GRAD_RATIO_M  (ISP_DEMOSAIC_GRAD_RATIO_V << ISP_DEMOSAIC_GRAD_RATIO_S)
#define ISP_DEMOSAIC_GRAD_RATIO_V  0x0000003FU
#define ISP_DEMOSAIC_GRAD_RATIO_S  0

/** ISP_MEDIAN_MATRIX_CTRL_REG register
 *  median pix2matrix ctrl
 */
#define ISP_MEDIAN_MATRIX_CTRL_REG (DR_REG_ISP_BASE + 0x60)
/** ISP_MEDIAN_PADDING_DATA : R/W; bitpos: [7:0]; default: 0;
 *  this field configures median matrix padding data
 */
#define ISP_MEDIAN_PADDING_DATA    0x000000FFU
#define ISP_MEDIAN_PADDING_DATA_M  (ISP_MEDIAN_PADDING_DATA_V << ISP_MEDIAN_PADDING_DATA_S)
#define ISP_MEDIAN_PADDING_DATA_V  0x000000FFU
#define ISP_MEDIAN_PADDING_DATA_S  0
/** ISP_MEDIAN_PADDING_MODE : R/W; bitpos: [8]; default: 0;
 *  this bit configures the padding mode of median matrix.  0: use pixel in image to do
 *  padding   1: use reg_padding_data to do padding
 */
#define ISP_MEDIAN_PADDING_MODE    (BIT(8))
#define ISP_MEDIAN_PADDING_MODE_M  (ISP_MEDIAN_PADDING_MODE_V << ISP_MEDIAN_PADDING_MODE_S)
#define ISP_MEDIAN_PADDING_MODE_V  0x00000001U
#define ISP_MEDIAN_PADDING_MODE_S  8

/** ISP_INT_RAW_REG register
 *  raw interrupt register
 */
#define ISP_INT_RAW_REG (DR_REG_ISP_BASE + 0x64)
/** ISP_ISP_DATA_TYPE_ERR_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  the raw interrupt status of input data type error. isp only support RGB bayer data
 *  type, other type will report type_err_int
 */
#define ISP_ISP_DATA_TYPE_ERR_INT_RAW    (BIT(0))
#define ISP_ISP_DATA_TYPE_ERR_INT_RAW_M  (ISP_ISP_DATA_TYPE_ERR_INT_RAW_V << ISP_ISP_DATA_TYPE_ERR_INT_RAW_S)
#define ISP_ISP_DATA_TYPE_ERR_INT_RAW_V  0x00000001U
#define ISP_ISP_DATA_TYPE_ERR_INT_RAW_S  0
/** ISP_ISP_ASYNC_FIFO_OVF_INT_RAW : R/SS/WTC; bitpos: [1]; default: 0;
 *  the raw interrupt status of isp input fifo overflow
 */
#define ISP_ISP_ASYNC_FIFO_OVF_INT_RAW    (BIT(1))
#define ISP_ISP_ASYNC_FIFO_OVF_INT_RAW_M  (ISP_ISP_ASYNC_FIFO_OVF_INT_RAW_V << ISP_ISP_ASYNC_FIFO_OVF_INT_RAW_S)
#define ISP_ISP_ASYNC_FIFO_OVF_INT_RAW_V  0x00000001U
#define ISP_ISP_ASYNC_FIFO_OVF_INT_RAW_S  1
/** ISP_ISP_BUF_FULL_INT_RAW : R/SS/WTC; bitpos: [2]; default: 0;
 *  the raw interrupt status of isp input buffer full
 */
#define ISP_ISP_BUF_FULL_INT_RAW    (BIT(2))
#define ISP_ISP_BUF_FULL_INT_RAW_M  (ISP_ISP_BUF_FULL_INT_RAW_V << ISP_ISP_BUF_FULL_INT_RAW_S)
#define ISP_ISP_BUF_FULL_INT_RAW_V  0x00000001U
#define ISP_ISP_BUF_FULL_INT_RAW_S  2
/** ISP_ISP_HVNUM_SETTING_ERR_INT_RAW : R/SS/WTC; bitpos: [3]; default: 0;
 *  the raw interrupt status of hnum and vnum setting format error
 */
#define ISP_ISP_HVNUM_SETTING_ERR_INT_RAW    (BIT(3))
#define ISP_ISP_HVNUM_SETTING_ERR_INT_RAW_M  (ISP_ISP_HVNUM_SETTING_ERR_INT_RAW_V << ISP_ISP_HVNUM_SETTING_ERR_INT_RAW_S)
#define ISP_ISP_HVNUM_SETTING_ERR_INT_RAW_V  0x00000001U
#define ISP_ISP_HVNUM_SETTING_ERR_INT_RAW_S  3
/** ISP_ISP_DATA_TYPE_SETTING_ERR_INT_RAW : R/SS/WTC; bitpos: [4]; default: 0;
 *  the raw interrupt status of setting invalid reg_data_type
 */
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_RAW    (BIT(4))
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_RAW_M  (ISP_ISP_DATA_TYPE_SETTING_ERR_INT_RAW_V << ISP_ISP_DATA_TYPE_SETTING_ERR_INT_RAW_S)
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_RAW_V  0x00000001U
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_RAW_S  4
/** ISP_ISP_MIPI_HNUM_UNMATCH_INT_RAW : R/SS/WTC; bitpos: [5]; default: 0;
 *  the raw interrupt status of hnum setting unmatch with mipi input
 */
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_RAW    (BIT(5))
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_RAW_M  (ISP_ISP_MIPI_HNUM_UNMATCH_INT_RAW_V << ISP_ISP_MIPI_HNUM_UNMATCH_INT_RAW_S)
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_RAW_V  0x00000001U
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_RAW_S  5
/** ISP_DPC_CHECK_DONE_INT_RAW : R/SS/WTC; bitpos: [6]; default: 0;
 *  the raw interrupt status of dpc check done
 */
#define ISP_DPC_CHECK_DONE_INT_RAW    (BIT(6))
#define ISP_DPC_CHECK_DONE_INT_RAW_M  (ISP_DPC_CHECK_DONE_INT_RAW_V << ISP_DPC_CHECK_DONE_INT_RAW_S)
#define ISP_DPC_CHECK_DONE_INT_RAW_V  0x00000001U
#define ISP_DPC_CHECK_DONE_INT_RAW_S  6
/** ISP_GAMMA_XCOORD_ERR_INT_RAW : R/SS/WTC; bitpos: [7]; default: 0;
 *  the raw interrupt status of gamma setting error. it report the sum of the lengths
 *  represented by reg_gamma_x00~x0F isn't equal to 256
 */
#define ISP_GAMMA_XCOORD_ERR_INT_RAW    (BIT(7))
#define ISP_GAMMA_XCOORD_ERR_INT_RAW_M  (ISP_GAMMA_XCOORD_ERR_INT_RAW_V << ISP_GAMMA_XCOORD_ERR_INT_RAW_S)
#define ISP_GAMMA_XCOORD_ERR_INT_RAW_V  0x00000001U
#define ISP_GAMMA_XCOORD_ERR_INT_RAW_S  7
/** ISP_AE_MONITOR_INT_RAW : R/SS/WTC; bitpos: [8]; default: 0;
 *  the raw interrupt status of ae monitor
 */
#define ISP_AE_MONITOR_INT_RAW    (BIT(8))
#define ISP_AE_MONITOR_INT_RAW_M  (ISP_AE_MONITOR_INT_RAW_V << ISP_AE_MONITOR_INT_RAW_S)
#define ISP_AE_MONITOR_INT_RAW_V  0x00000001U
#define ISP_AE_MONITOR_INT_RAW_S  8
/** ISP_AE_FRAME_DONE_INT_RAW : R/SS/WTC; bitpos: [9]; default: 0;
 *  the raw interrupt status of ae.
 */
#define ISP_AE_FRAME_DONE_INT_RAW    (BIT(9))
#define ISP_AE_FRAME_DONE_INT_RAW_M  (ISP_AE_FRAME_DONE_INT_RAW_V << ISP_AE_FRAME_DONE_INT_RAW_S)
#define ISP_AE_FRAME_DONE_INT_RAW_V  0x00000001U
#define ISP_AE_FRAME_DONE_INT_RAW_S  9
/** ISP_AF_FDONE_INT_RAW : R/SS/WTC; bitpos: [10]; default: 0;
 *  the raw interrupt status of af statistic. when auto_update enable, each frame done
 *  will send one int pulse when manual_update, each time when write 1 to
 *  reg_manual_update will send a int pulse when next frame done
 */
#define ISP_AF_FDONE_INT_RAW    (BIT(10))
#define ISP_AF_FDONE_INT_RAW_M  (ISP_AF_FDONE_INT_RAW_V << ISP_AF_FDONE_INT_RAW_S)
#define ISP_AF_FDONE_INT_RAW_V  0x00000001U
#define ISP_AF_FDONE_INT_RAW_S  10
/** ISP_AF_ENV_INT_RAW : R/SS/WTC; bitpos: [11]; default: 0;
 *  the raw interrupt status of af monitor. send a int pulse when env_det function
 *  enabled and environment changes detected
 */
#define ISP_AF_ENV_INT_RAW    (BIT(11))
#define ISP_AF_ENV_INT_RAW_M  (ISP_AF_ENV_INT_RAW_V << ISP_AF_ENV_INT_RAW_S)
#define ISP_AF_ENV_INT_RAW_V  0x00000001U
#define ISP_AF_ENV_INT_RAW_S  11
/** ISP_AWB_FDONE_INT_RAW : R/SS/WTC; bitpos: [12]; default: 0;
 *  the raw interrupt status of awb. send a int pulse when statistic of one awb frame
 *  done
 */
#define ISP_AWB_FDONE_INT_RAW    (BIT(12))
#define ISP_AWB_FDONE_INT_RAW_M  (ISP_AWB_FDONE_INT_RAW_V << ISP_AWB_FDONE_INT_RAW_S)
#define ISP_AWB_FDONE_INT_RAW_V  0x00000001U
#define ISP_AWB_FDONE_INT_RAW_S  12
/** ISP_HIST_FDONE_INT_RAW : R/SS/WTC; bitpos: [13]; default: 0;
 *  the raw interrupt status of histogram. send a int pulse when statistic of one frame
 *  histogram done
 */
#define ISP_HIST_FDONE_INT_RAW    (BIT(13))
#define ISP_HIST_FDONE_INT_RAW_M  (ISP_HIST_FDONE_INT_RAW_V << ISP_HIST_FDONE_INT_RAW_S)
#define ISP_HIST_FDONE_INT_RAW_V  0x00000001U
#define ISP_HIST_FDONE_INT_RAW_S  13
/** ISP_FRAME_INT_RAW : R/SS/WTC; bitpos: [14]; default: 0;
 *  the raw interrupt status of isp frame end
 */
#define ISP_FRAME_INT_RAW    (BIT(14))
#define ISP_FRAME_INT_RAW_M  (ISP_FRAME_INT_RAW_V << ISP_FRAME_INT_RAW_S)
#define ISP_FRAME_INT_RAW_V  0x00000001U
#define ISP_FRAME_INT_RAW_S  14
/** ISP_BLC_FRAME_INT_RAW : R/SS/WTC; bitpos: [15]; default: 0;
 *  the raw interrupt status of blc frame done
 */
#define ISP_BLC_FRAME_INT_RAW    (BIT(15))
#define ISP_BLC_FRAME_INT_RAW_M  (ISP_BLC_FRAME_INT_RAW_V << ISP_BLC_FRAME_INT_RAW_S)
#define ISP_BLC_FRAME_INT_RAW_V  0x00000001U
#define ISP_BLC_FRAME_INT_RAW_S  15
/** ISP_LSC_FRAME_INT_RAW : R/SS/WTC; bitpos: [16]; default: 0;
 *  the raw interrupt status of lsc frame done
 */
#define ISP_LSC_FRAME_INT_RAW    (BIT(16))
#define ISP_LSC_FRAME_INT_RAW_M  (ISP_LSC_FRAME_INT_RAW_V << ISP_LSC_FRAME_INT_RAW_S)
#define ISP_LSC_FRAME_INT_RAW_V  0x00000001U
#define ISP_LSC_FRAME_INT_RAW_S  16
/** ISP_DPC_FRAME_INT_RAW : R/SS/WTC; bitpos: [17]; default: 0;
 *  the raw interrupt status of dpc frame done
 */
#define ISP_DPC_FRAME_INT_RAW    (BIT(17))
#define ISP_DPC_FRAME_INT_RAW_M  (ISP_DPC_FRAME_INT_RAW_V << ISP_DPC_FRAME_INT_RAW_S)
#define ISP_DPC_FRAME_INT_RAW_V  0x00000001U
#define ISP_DPC_FRAME_INT_RAW_S  17
/** ISP_BF_FRAME_INT_RAW : R/SS/WTC; bitpos: [18]; default: 0;
 *  the raw interrupt status of bf frame done
 */
#define ISP_BF_FRAME_INT_RAW    (BIT(18))
#define ISP_BF_FRAME_INT_RAW_M  (ISP_BF_FRAME_INT_RAW_V << ISP_BF_FRAME_INT_RAW_S)
#define ISP_BF_FRAME_INT_RAW_V  0x00000001U
#define ISP_BF_FRAME_INT_RAW_S  18
/** ISP_DEMOSAIC_FRAME_INT_RAW : R/SS/WTC; bitpos: [19]; default: 0;
 *  the raw interrupt status of demosaic frame done
 */
#define ISP_DEMOSAIC_FRAME_INT_RAW    (BIT(19))
#define ISP_DEMOSAIC_FRAME_INT_RAW_M  (ISP_DEMOSAIC_FRAME_INT_RAW_V << ISP_DEMOSAIC_FRAME_INT_RAW_S)
#define ISP_DEMOSAIC_FRAME_INT_RAW_V  0x00000001U
#define ISP_DEMOSAIC_FRAME_INT_RAW_S  19
/** ISP_MEDIAN_FRAME_INT_RAW : R/SS/WTC; bitpos: [20]; default: 0;
 *  the raw interrupt status of median frame done
 */
#define ISP_MEDIAN_FRAME_INT_RAW    (BIT(20))
#define ISP_MEDIAN_FRAME_INT_RAW_M  (ISP_MEDIAN_FRAME_INT_RAW_V << ISP_MEDIAN_FRAME_INT_RAW_S)
#define ISP_MEDIAN_FRAME_INT_RAW_V  0x00000001U
#define ISP_MEDIAN_FRAME_INT_RAW_S  20
/** ISP_CCM_FRAME_INT_RAW : R/SS/WTC; bitpos: [21]; default: 0;
 *  the raw interrupt status of ccm frame done
 */
#define ISP_CCM_FRAME_INT_RAW    (BIT(21))
#define ISP_CCM_FRAME_INT_RAW_M  (ISP_CCM_FRAME_INT_RAW_V << ISP_CCM_FRAME_INT_RAW_S)
#define ISP_CCM_FRAME_INT_RAW_V  0x00000001U
#define ISP_CCM_FRAME_INT_RAW_S  21
/** ISP_GAMMA_FRAME_INT_RAW : R/SS/WTC; bitpos: [22]; default: 0;
 *  the raw interrupt status of gamma frame done
 */
#define ISP_GAMMA_FRAME_INT_RAW    (BIT(22))
#define ISP_GAMMA_FRAME_INT_RAW_M  (ISP_GAMMA_FRAME_INT_RAW_V << ISP_GAMMA_FRAME_INT_RAW_S)
#define ISP_GAMMA_FRAME_INT_RAW_V  0x00000001U
#define ISP_GAMMA_FRAME_INT_RAW_S  22
/** ISP_RGB2YUV_FRAME_INT_RAW : R/SS/WTC; bitpos: [23]; default: 0;
 *  the raw interrupt status of rgb2yuv frame done
 */
#define ISP_RGB2YUV_FRAME_INT_RAW    (BIT(23))
#define ISP_RGB2YUV_FRAME_INT_RAW_M  (ISP_RGB2YUV_FRAME_INT_RAW_V << ISP_RGB2YUV_FRAME_INT_RAW_S)
#define ISP_RGB2YUV_FRAME_INT_RAW_V  0x00000001U
#define ISP_RGB2YUV_FRAME_INT_RAW_S  23
/** ISP_SHARP_FRAME_INT_RAW : R/SS/WTC; bitpos: [24]; default: 0;
 *  the raw interrupt status of sharp frame done
 */
#define ISP_SHARP_FRAME_INT_RAW    (BIT(24))
#define ISP_SHARP_FRAME_INT_RAW_M  (ISP_SHARP_FRAME_INT_RAW_V << ISP_SHARP_FRAME_INT_RAW_S)
#define ISP_SHARP_FRAME_INT_RAW_V  0x00000001U
#define ISP_SHARP_FRAME_INT_RAW_S  24
/** ISP_COLOR_FRAME_INT_RAW : R/SS/WTC; bitpos: [25]; default: 0;
 *  the raw interrupt status of color frame done
 */
#define ISP_COLOR_FRAME_INT_RAW    (BIT(25))
#define ISP_COLOR_FRAME_INT_RAW_M  (ISP_COLOR_FRAME_INT_RAW_V << ISP_COLOR_FRAME_INT_RAW_S)
#define ISP_COLOR_FRAME_INT_RAW_V  0x00000001U
#define ISP_COLOR_FRAME_INT_RAW_S  25
/** ISP_YUV2RGB_FRAME_INT_RAW : R/SS/WTC; bitpos: [26]; default: 0;
 *  the raw interrupt status of yuv2rgb frame done
 */
#define ISP_YUV2RGB_FRAME_INT_RAW    (BIT(26))
#define ISP_YUV2RGB_FRAME_INT_RAW_M  (ISP_YUV2RGB_FRAME_INT_RAW_V << ISP_YUV2RGB_FRAME_INT_RAW_S)
#define ISP_YUV2RGB_FRAME_INT_RAW_V  0x00000001U
#define ISP_YUV2RGB_FRAME_INT_RAW_S  26
/** ISP_TAIL_IDI_FRAME_INT_RAW : R/SS/WTC; bitpos: [27]; default: 0;
 *  the raw interrupt status of isp_tail idi frame_end
 */
#define ISP_TAIL_IDI_FRAME_INT_RAW    (BIT(27))
#define ISP_TAIL_IDI_FRAME_INT_RAW_M  (ISP_TAIL_IDI_FRAME_INT_RAW_V << ISP_TAIL_IDI_FRAME_INT_RAW_S)
#define ISP_TAIL_IDI_FRAME_INT_RAW_V  0x00000001U
#define ISP_TAIL_IDI_FRAME_INT_RAW_S  27
/** ISP_HEADER_IDI_FRAME_INT_RAW : R/SS/WTC; bitpos: [28]; default: 0;
 *  the raw interrupt status of real input frame end of isp_input
 */
#define ISP_HEADER_IDI_FRAME_INT_RAW    (BIT(28))
#define ISP_HEADER_IDI_FRAME_INT_RAW_M  (ISP_HEADER_IDI_FRAME_INT_RAW_V << ISP_HEADER_IDI_FRAME_INT_RAW_S)
#define ISP_HEADER_IDI_FRAME_INT_RAW_V  0x00000001U
#define ISP_HEADER_IDI_FRAME_INT_RAW_S  28

/** ISP_INT_ST_REG register
 *  masked interrupt register
 */
#define ISP_INT_ST_REG (DR_REG_ISP_BASE + 0x68)
/** ISP_ISP_DATA_TYPE_ERR_INT_ST : RO; bitpos: [0]; default: 0;
 *  the masked interrupt status of input data type error
 */
#define ISP_ISP_DATA_TYPE_ERR_INT_ST    (BIT(0))
#define ISP_ISP_DATA_TYPE_ERR_INT_ST_M  (ISP_ISP_DATA_TYPE_ERR_INT_ST_V << ISP_ISP_DATA_TYPE_ERR_INT_ST_S)
#define ISP_ISP_DATA_TYPE_ERR_INT_ST_V  0x00000001U
#define ISP_ISP_DATA_TYPE_ERR_INT_ST_S  0
/** ISP_ISP_ASYNC_FIFO_OVF_INT_ST : RO; bitpos: [1]; default: 0;
 *  the masked interrupt status of isp input fifo overflow
 */
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ST    (BIT(1))
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ST_M  (ISP_ISP_ASYNC_FIFO_OVF_INT_ST_V << ISP_ISP_ASYNC_FIFO_OVF_INT_ST_S)
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ST_V  0x00000001U
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ST_S  1
/** ISP_ISP_BUF_FULL_INT_ST : RO; bitpos: [2]; default: 0;
 *  the masked interrupt status of isp input buffer full
 */
#define ISP_ISP_BUF_FULL_INT_ST    (BIT(2))
#define ISP_ISP_BUF_FULL_INT_ST_M  (ISP_ISP_BUF_FULL_INT_ST_V << ISP_ISP_BUF_FULL_INT_ST_S)
#define ISP_ISP_BUF_FULL_INT_ST_V  0x00000001U
#define ISP_ISP_BUF_FULL_INT_ST_S  2
/** ISP_ISP_HVNUM_SETTING_ERR_INT_ST : RO; bitpos: [3]; default: 0;
 *  the masked interrupt status of hnum and vnum setting format error
 */
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ST    (BIT(3))
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ST_M  (ISP_ISP_HVNUM_SETTING_ERR_INT_ST_V << ISP_ISP_HVNUM_SETTING_ERR_INT_ST_S)
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ST_V  0x00000001U
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ST_S  3
/** ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ST : RO; bitpos: [4]; default: 0;
 *  the masked interrupt status of setting invalid reg_data_type
 */
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ST    (BIT(4))
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ST_M  (ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ST_V << ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ST_S)
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ST_V  0x00000001U
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ST_S  4
/** ISP_ISP_MIPI_HNUM_UNMATCH_INT_ST : RO; bitpos: [5]; default: 0;
 *  the masked interrupt status of hnum setting unmatch with mipi input
 */
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ST    (BIT(5))
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ST_M  (ISP_ISP_MIPI_HNUM_UNMATCH_INT_ST_V << ISP_ISP_MIPI_HNUM_UNMATCH_INT_ST_S)
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ST_V  0x00000001U
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ST_S  5
/** ISP_DPC_CHECK_DONE_INT_ST : RO; bitpos: [6]; default: 0;
 *  the masked interrupt status of dpc check done
 */
#define ISP_DPC_CHECK_DONE_INT_ST    (BIT(6))
#define ISP_DPC_CHECK_DONE_INT_ST_M  (ISP_DPC_CHECK_DONE_INT_ST_V << ISP_DPC_CHECK_DONE_INT_ST_S)
#define ISP_DPC_CHECK_DONE_INT_ST_V  0x00000001U
#define ISP_DPC_CHECK_DONE_INT_ST_S  6
/** ISP_GAMMA_XCOORD_ERR_INT_ST : RO; bitpos: [7]; default: 0;
 *  the masked interrupt status of gamma setting error
 */
#define ISP_GAMMA_XCOORD_ERR_INT_ST    (BIT(7))
#define ISP_GAMMA_XCOORD_ERR_INT_ST_M  (ISP_GAMMA_XCOORD_ERR_INT_ST_V << ISP_GAMMA_XCOORD_ERR_INT_ST_S)
#define ISP_GAMMA_XCOORD_ERR_INT_ST_V  0x00000001U
#define ISP_GAMMA_XCOORD_ERR_INT_ST_S  7
/** ISP_AE_MONITOR_INT_ST : RO; bitpos: [8]; default: 0;
 *  the masked interrupt status of ae monitor
 */
#define ISP_AE_MONITOR_INT_ST    (BIT(8))
#define ISP_AE_MONITOR_INT_ST_M  (ISP_AE_MONITOR_INT_ST_V << ISP_AE_MONITOR_INT_ST_S)
#define ISP_AE_MONITOR_INT_ST_V  0x00000001U
#define ISP_AE_MONITOR_INT_ST_S  8
/** ISP_AE_FRAME_DONE_INT_ST : RO; bitpos: [9]; default: 0;
 *  the masked interrupt status of ae
 */
#define ISP_AE_FRAME_DONE_INT_ST    (BIT(9))
#define ISP_AE_FRAME_DONE_INT_ST_M  (ISP_AE_FRAME_DONE_INT_ST_V << ISP_AE_FRAME_DONE_INT_ST_S)
#define ISP_AE_FRAME_DONE_INT_ST_V  0x00000001U
#define ISP_AE_FRAME_DONE_INT_ST_S  9
/** ISP_AF_FDONE_INT_ST : RO; bitpos: [10]; default: 0;
 *  the masked interrupt status of af statistic
 */
#define ISP_AF_FDONE_INT_ST    (BIT(10))
#define ISP_AF_FDONE_INT_ST_M  (ISP_AF_FDONE_INT_ST_V << ISP_AF_FDONE_INT_ST_S)
#define ISP_AF_FDONE_INT_ST_V  0x00000001U
#define ISP_AF_FDONE_INT_ST_S  10
/** ISP_AF_ENV_INT_ST : RO; bitpos: [11]; default: 0;
 *  the masked interrupt status of af monitor
 */
#define ISP_AF_ENV_INT_ST    (BIT(11))
#define ISP_AF_ENV_INT_ST_M  (ISP_AF_ENV_INT_ST_V << ISP_AF_ENV_INT_ST_S)
#define ISP_AF_ENV_INT_ST_V  0x00000001U
#define ISP_AF_ENV_INT_ST_S  11
/** ISP_AWB_FDONE_INT_ST : RO; bitpos: [12]; default: 0;
 *  the masked interrupt status of awb
 */
#define ISP_AWB_FDONE_INT_ST    (BIT(12))
#define ISP_AWB_FDONE_INT_ST_M  (ISP_AWB_FDONE_INT_ST_V << ISP_AWB_FDONE_INT_ST_S)
#define ISP_AWB_FDONE_INT_ST_V  0x00000001U
#define ISP_AWB_FDONE_INT_ST_S  12
/** ISP_HIST_FDONE_INT_ST : RO; bitpos: [13]; default: 0;
 *  the masked interrupt status of histogram
 */
#define ISP_HIST_FDONE_INT_ST    (BIT(13))
#define ISP_HIST_FDONE_INT_ST_M  (ISP_HIST_FDONE_INT_ST_V << ISP_HIST_FDONE_INT_ST_S)
#define ISP_HIST_FDONE_INT_ST_V  0x00000001U
#define ISP_HIST_FDONE_INT_ST_S  13
/** ISP_FRAME_INT_ST : RO; bitpos: [14]; default: 0;
 *  the masked interrupt status of isp frame end
 */
#define ISP_FRAME_INT_ST    (BIT(14))
#define ISP_FRAME_INT_ST_M  (ISP_FRAME_INT_ST_V << ISP_FRAME_INT_ST_S)
#define ISP_FRAME_INT_ST_V  0x00000001U
#define ISP_FRAME_INT_ST_S  14
/** ISP_BLC_FRAME_INT_ST : RO; bitpos: [15]; default: 0;
 *  the masked interrupt status of blc frame done
 */
#define ISP_BLC_FRAME_INT_ST    (BIT(15))
#define ISP_BLC_FRAME_INT_ST_M  (ISP_BLC_FRAME_INT_ST_V << ISP_BLC_FRAME_INT_ST_S)
#define ISP_BLC_FRAME_INT_ST_V  0x00000001U
#define ISP_BLC_FRAME_INT_ST_S  15
/** ISP_LSC_FRAME_INT_ST : RO; bitpos: [16]; default: 0;
 *  the masked interrupt status of lsc frame done
 */
#define ISP_LSC_FRAME_INT_ST    (BIT(16))
#define ISP_LSC_FRAME_INT_ST_M  (ISP_LSC_FRAME_INT_ST_V << ISP_LSC_FRAME_INT_ST_S)
#define ISP_LSC_FRAME_INT_ST_V  0x00000001U
#define ISP_LSC_FRAME_INT_ST_S  16
/** ISP_DPC_FRAME_INT_ST : RO; bitpos: [17]; default: 0;
 *  the masked interrupt status of dpc frame done
 */
#define ISP_DPC_FRAME_INT_ST    (BIT(17))
#define ISP_DPC_FRAME_INT_ST_M  (ISP_DPC_FRAME_INT_ST_V << ISP_DPC_FRAME_INT_ST_S)
#define ISP_DPC_FRAME_INT_ST_V  0x00000001U
#define ISP_DPC_FRAME_INT_ST_S  17
/** ISP_BF_FRAME_INT_ST : RO; bitpos: [18]; default: 0;
 *  the masked interrupt status of bf frame done
 */
#define ISP_BF_FRAME_INT_ST    (BIT(18))
#define ISP_BF_FRAME_INT_ST_M  (ISP_BF_FRAME_INT_ST_V << ISP_BF_FRAME_INT_ST_S)
#define ISP_BF_FRAME_INT_ST_V  0x00000001U
#define ISP_BF_FRAME_INT_ST_S  18
/** ISP_DEMOSAIC_FRAME_INT_ST : RO; bitpos: [19]; default: 0;
 *  the masked interrupt status of demosaic frame done
 */
#define ISP_DEMOSAIC_FRAME_INT_ST    (BIT(19))
#define ISP_DEMOSAIC_FRAME_INT_ST_M  (ISP_DEMOSAIC_FRAME_INT_ST_V << ISP_DEMOSAIC_FRAME_INT_ST_S)
#define ISP_DEMOSAIC_FRAME_INT_ST_V  0x00000001U
#define ISP_DEMOSAIC_FRAME_INT_ST_S  19
/** ISP_MEDIAN_FRAME_INT_ST : RO; bitpos: [20]; default: 0;
 *  the masked interrupt status of median frame done
 */
#define ISP_MEDIAN_FRAME_INT_ST    (BIT(20))
#define ISP_MEDIAN_FRAME_INT_ST_M  (ISP_MEDIAN_FRAME_INT_ST_V << ISP_MEDIAN_FRAME_INT_ST_S)
#define ISP_MEDIAN_FRAME_INT_ST_V  0x00000001U
#define ISP_MEDIAN_FRAME_INT_ST_S  20
/** ISP_CCM_FRAME_INT_ST : RO; bitpos: [21]; default: 0;
 *  the masked interrupt status of ccm frame done
 */
#define ISP_CCM_FRAME_INT_ST    (BIT(21))
#define ISP_CCM_FRAME_INT_ST_M  (ISP_CCM_FRAME_INT_ST_V << ISP_CCM_FRAME_INT_ST_S)
#define ISP_CCM_FRAME_INT_ST_V  0x00000001U
#define ISP_CCM_FRAME_INT_ST_S  21
/** ISP_GAMMA_FRAME_INT_ST : RO; bitpos: [22]; default: 0;
 *  the masked interrupt status of gamma frame done
 */
#define ISP_GAMMA_FRAME_INT_ST    (BIT(22))
#define ISP_GAMMA_FRAME_INT_ST_M  (ISP_GAMMA_FRAME_INT_ST_V << ISP_GAMMA_FRAME_INT_ST_S)
#define ISP_GAMMA_FRAME_INT_ST_V  0x00000001U
#define ISP_GAMMA_FRAME_INT_ST_S  22
/** ISP_RGB2YUV_FRAME_INT_ST : RO; bitpos: [23]; default: 0;
 *  the masked interrupt status of rgb2yuv frame done
 */
#define ISP_RGB2YUV_FRAME_INT_ST    (BIT(23))
#define ISP_RGB2YUV_FRAME_INT_ST_M  (ISP_RGB2YUV_FRAME_INT_ST_V << ISP_RGB2YUV_FRAME_INT_ST_S)
#define ISP_RGB2YUV_FRAME_INT_ST_V  0x00000001U
#define ISP_RGB2YUV_FRAME_INT_ST_S  23
/** ISP_SHARP_FRAME_INT_ST : RO; bitpos: [24]; default: 0;
 *  the masked interrupt status of sharp frame done
 */
#define ISP_SHARP_FRAME_INT_ST    (BIT(24))
#define ISP_SHARP_FRAME_INT_ST_M  (ISP_SHARP_FRAME_INT_ST_V << ISP_SHARP_FRAME_INT_ST_S)
#define ISP_SHARP_FRAME_INT_ST_V  0x00000001U
#define ISP_SHARP_FRAME_INT_ST_S  24
/** ISP_COLOR_FRAME_INT_ST : RO; bitpos: [25]; default: 0;
 *  the masked interrupt status of color frame done
 */
#define ISP_COLOR_FRAME_INT_ST    (BIT(25))
#define ISP_COLOR_FRAME_INT_ST_M  (ISP_COLOR_FRAME_INT_ST_V << ISP_COLOR_FRAME_INT_ST_S)
#define ISP_COLOR_FRAME_INT_ST_V  0x00000001U
#define ISP_COLOR_FRAME_INT_ST_S  25
/** ISP_YUV2RGB_FRAME_INT_ST : RO; bitpos: [26]; default: 0;
 *  the masked interrupt status of yuv2rgb frame done
 */
#define ISP_YUV2RGB_FRAME_INT_ST    (BIT(26))
#define ISP_YUV2RGB_FRAME_INT_ST_M  (ISP_YUV2RGB_FRAME_INT_ST_V << ISP_YUV2RGB_FRAME_INT_ST_S)
#define ISP_YUV2RGB_FRAME_INT_ST_V  0x00000001U
#define ISP_YUV2RGB_FRAME_INT_ST_S  26
/** ISP_TAIL_IDI_FRAME_INT_ST : RO; bitpos: [27]; default: 0;
 *  the masked interrupt status of isp_tail idi frame_end
 */
#define ISP_TAIL_IDI_FRAME_INT_ST    (BIT(27))
#define ISP_TAIL_IDI_FRAME_INT_ST_M  (ISP_TAIL_IDI_FRAME_INT_ST_V << ISP_TAIL_IDI_FRAME_INT_ST_S)
#define ISP_TAIL_IDI_FRAME_INT_ST_V  0x00000001U
#define ISP_TAIL_IDI_FRAME_INT_ST_S  27
/** ISP_HEADER_IDI_FRAME_INT_ST : RO; bitpos: [28]; default: 0;
 *  the masked interrupt status of real input frame end of isp_input
 */
#define ISP_HEADER_IDI_FRAME_INT_ST    (BIT(28))
#define ISP_HEADER_IDI_FRAME_INT_ST_M  (ISP_HEADER_IDI_FRAME_INT_ST_V << ISP_HEADER_IDI_FRAME_INT_ST_S)
#define ISP_HEADER_IDI_FRAME_INT_ST_V  0x00000001U
#define ISP_HEADER_IDI_FRAME_INT_ST_S  28

/** ISP_INT_ENA_REG register
 *  interrupt enable register
 */
#define ISP_INT_ENA_REG (DR_REG_ISP_BASE + 0x6c)
/** ISP_ISP_DATA_TYPE_ERR_INT_ENA : R/W; bitpos: [0]; default: 1;
 *  write 1 to enable input data type error
 */
#define ISP_ISP_DATA_TYPE_ERR_INT_ENA    (BIT(0))
#define ISP_ISP_DATA_TYPE_ERR_INT_ENA_M  (ISP_ISP_DATA_TYPE_ERR_INT_ENA_V << ISP_ISP_DATA_TYPE_ERR_INT_ENA_S)
#define ISP_ISP_DATA_TYPE_ERR_INT_ENA_V  0x00000001U
#define ISP_ISP_DATA_TYPE_ERR_INT_ENA_S  0
/** ISP_ISP_ASYNC_FIFO_OVF_INT_ENA : R/W; bitpos: [1]; default: 1;
 *  write 1 to enable isp input fifo overflow
 */
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ENA    (BIT(1))
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ENA_M  (ISP_ISP_ASYNC_FIFO_OVF_INT_ENA_V << ISP_ISP_ASYNC_FIFO_OVF_INT_ENA_S)
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ENA_V  0x00000001U
#define ISP_ISP_ASYNC_FIFO_OVF_INT_ENA_S  1
/** ISP_ISP_BUF_FULL_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  write 1 to enable isp input buffer full
 */
#define ISP_ISP_BUF_FULL_INT_ENA    (BIT(2))
#define ISP_ISP_BUF_FULL_INT_ENA_M  (ISP_ISP_BUF_FULL_INT_ENA_V << ISP_ISP_BUF_FULL_INT_ENA_S)
#define ISP_ISP_BUF_FULL_INT_ENA_V  0x00000001U
#define ISP_ISP_BUF_FULL_INT_ENA_S  2
/** ISP_ISP_HVNUM_SETTING_ERR_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  write 1 to enable hnum and vnum setting format error
 */
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ENA    (BIT(3))
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ENA_M  (ISP_ISP_HVNUM_SETTING_ERR_INT_ENA_V << ISP_ISP_HVNUM_SETTING_ERR_INT_ENA_S)
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ENA_V  0x00000001U
#define ISP_ISP_HVNUM_SETTING_ERR_INT_ENA_S  3
/** ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  write 1 to enable setting invalid reg_data_type
 */
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ENA    (BIT(4))
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ENA_M  (ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ENA_V << ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ENA_S)
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ENA_V  0x00000001U
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_ENA_S  4
/** ISP_ISP_MIPI_HNUM_UNMATCH_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  write 1 to enable hnum setting unmatch with mipi input
 */
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ENA    (BIT(5))
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ENA_M  (ISP_ISP_MIPI_HNUM_UNMATCH_INT_ENA_V << ISP_ISP_MIPI_HNUM_UNMATCH_INT_ENA_S)
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ENA_V  0x00000001U
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_ENA_S  5
/** ISP_DPC_CHECK_DONE_INT_ENA : R/W; bitpos: [6]; default: 1;
 *  write 1 to enable dpc check done
 */
#define ISP_DPC_CHECK_DONE_INT_ENA    (BIT(6))
#define ISP_DPC_CHECK_DONE_INT_ENA_M  (ISP_DPC_CHECK_DONE_INT_ENA_V << ISP_DPC_CHECK_DONE_INT_ENA_S)
#define ISP_DPC_CHECK_DONE_INT_ENA_V  0x00000001U
#define ISP_DPC_CHECK_DONE_INT_ENA_S  6
/** ISP_GAMMA_XCOORD_ERR_INT_ENA : R/W; bitpos: [7]; default: 1;
 *  write 1 to enable gamma setting error
 */
#define ISP_GAMMA_XCOORD_ERR_INT_ENA    (BIT(7))
#define ISP_GAMMA_XCOORD_ERR_INT_ENA_M  (ISP_GAMMA_XCOORD_ERR_INT_ENA_V << ISP_GAMMA_XCOORD_ERR_INT_ENA_S)
#define ISP_GAMMA_XCOORD_ERR_INT_ENA_V  0x00000001U
#define ISP_GAMMA_XCOORD_ERR_INT_ENA_S  7
/** ISP_AE_MONITOR_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  write 1 to enable ae monitor
 */
#define ISP_AE_MONITOR_INT_ENA    (BIT(8))
#define ISP_AE_MONITOR_INT_ENA_M  (ISP_AE_MONITOR_INT_ENA_V << ISP_AE_MONITOR_INT_ENA_S)
#define ISP_AE_MONITOR_INT_ENA_V  0x00000001U
#define ISP_AE_MONITOR_INT_ENA_S  8
/** ISP_AE_FRAME_DONE_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  write 1 to enable ae
 */
#define ISP_AE_FRAME_DONE_INT_ENA    (BIT(9))
#define ISP_AE_FRAME_DONE_INT_ENA_M  (ISP_AE_FRAME_DONE_INT_ENA_V << ISP_AE_FRAME_DONE_INT_ENA_S)
#define ISP_AE_FRAME_DONE_INT_ENA_V  0x00000001U
#define ISP_AE_FRAME_DONE_INT_ENA_S  9
/** ISP_AF_FDONE_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  write 1 to enable af statistic
 */
#define ISP_AF_FDONE_INT_ENA    (BIT(10))
#define ISP_AF_FDONE_INT_ENA_M  (ISP_AF_FDONE_INT_ENA_V << ISP_AF_FDONE_INT_ENA_S)
#define ISP_AF_FDONE_INT_ENA_V  0x00000001U
#define ISP_AF_FDONE_INT_ENA_S  10
/** ISP_AF_ENV_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  write 1 to enable af monitor
 */
#define ISP_AF_ENV_INT_ENA    (BIT(11))
#define ISP_AF_ENV_INT_ENA_M  (ISP_AF_ENV_INT_ENA_V << ISP_AF_ENV_INT_ENA_S)
#define ISP_AF_ENV_INT_ENA_V  0x00000001U
#define ISP_AF_ENV_INT_ENA_S  11
/** ISP_AWB_FDONE_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  write 1 to enable awb
 */
#define ISP_AWB_FDONE_INT_ENA    (BIT(12))
#define ISP_AWB_FDONE_INT_ENA_M  (ISP_AWB_FDONE_INT_ENA_V << ISP_AWB_FDONE_INT_ENA_S)
#define ISP_AWB_FDONE_INT_ENA_V  0x00000001U
#define ISP_AWB_FDONE_INT_ENA_S  12
/** ISP_HIST_FDONE_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  write 1 to enable histogram
 */
#define ISP_HIST_FDONE_INT_ENA    (BIT(13))
#define ISP_HIST_FDONE_INT_ENA_M  (ISP_HIST_FDONE_INT_ENA_V << ISP_HIST_FDONE_INT_ENA_S)
#define ISP_HIST_FDONE_INT_ENA_V  0x00000001U
#define ISP_HIST_FDONE_INT_ENA_S  13
/** ISP_FRAME_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  write 1 to enable isp frame end
 */
#define ISP_FRAME_INT_ENA    (BIT(14))
#define ISP_FRAME_INT_ENA_M  (ISP_FRAME_INT_ENA_V << ISP_FRAME_INT_ENA_S)
#define ISP_FRAME_INT_ENA_V  0x00000001U
#define ISP_FRAME_INT_ENA_S  14
/** ISP_BLC_FRAME_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  write 1 to enable blc frame done
 */
#define ISP_BLC_FRAME_INT_ENA    (BIT(15))
#define ISP_BLC_FRAME_INT_ENA_M  (ISP_BLC_FRAME_INT_ENA_V << ISP_BLC_FRAME_INT_ENA_S)
#define ISP_BLC_FRAME_INT_ENA_V  0x00000001U
#define ISP_BLC_FRAME_INT_ENA_S  15
/** ISP_LSC_FRAME_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  write 1 to enable lsc frame done
 */
#define ISP_LSC_FRAME_INT_ENA    (BIT(16))
#define ISP_LSC_FRAME_INT_ENA_M  (ISP_LSC_FRAME_INT_ENA_V << ISP_LSC_FRAME_INT_ENA_S)
#define ISP_LSC_FRAME_INT_ENA_V  0x00000001U
#define ISP_LSC_FRAME_INT_ENA_S  16
/** ISP_DPC_FRAME_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  write 1 to enable dpc frame done
 */
#define ISP_DPC_FRAME_INT_ENA    (BIT(17))
#define ISP_DPC_FRAME_INT_ENA_M  (ISP_DPC_FRAME_INT_ENA_V << ISP_DPC_FRAME_INT_ENA_S)
#define ISP_DPC_FRAME_INT_ENA_V  0x00000001U
#define ISP_DPC_FRAME_INT_ENA_S  17
/** ISP_BF_FRAME_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  write 1 to enable bf frame done
 */
#define ISP_BF_FRAME_INT_ENA    (BIT(18))
#define ISP_BF_FRAME_INT_ENA_M  (ISP_BF_FRAME_INT_ENA_V << ISP_BF_FRAME_INT_ENA_S)
#define ISP_BF_FRAME_INT_ENA_V  0x00000001U
#define ISP_BF_FRAME_INT_ENA_S  18
/** ISP_DEMOSAIC_FRAME_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  write 1 to enable demosaic frame done
 */
#define ISP_DEMOSAIC_FRAME_INT_ENA    (BIT(19))
#define ISP_DEMOSAIC_FRAME_INT_ENA_M  (ISP_DEMOSAIC_FRAME_INT_ENA_V << ISP_DEMOSAIC_FRAME_INT_ENA_S)
#define ISP_DEMOSAIC_FRAME_INT_ENA_V  0x00000001U
#define ISP_DEMOSAIC_FRAME_INT_ENA_S  19
/** ISP_MEDIAN_FRAME_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  write 1 to enable median frame done
 */
#define ISP_MEDIAN_FRAME_INT_ENA    (BIT(20))
#define ISP_MEDIAN_FRAME_INT_ENA_M  (ISP_MEDIAN_FRAME_INT_ENA_V << ISP_MEDIAN_FRAME_INT_ENA_S)
#define ISP_MEDIAN_FRAME_INT_ENA_V  0x00000001U
#define ISP_MEDIAN_FRAME_INT_ENA_S  20
/** ISP_CCM_FRAME_INT_ENA : R/W; bitpos: [21]; default: 0;
 *  write 1 to enable ccm frame done
 */
#define ISP_CCM_FRAME_INT_ENA    (BIT(21))
#define ISP_CCM_FRAME_INT_ENA_M  (ISP_CCM_FRAME_INT_ENA_V << ISP_CCM_FRAME_INT_ENA_S)
#define ISP_CCM_FRAME_INT_ENA_V  0x00000001U
#define ISP_CCM_FRAME_INT_ENA_S  21
/** ISP_GAMMA_FRAME_INT_ENA : R/W; bitpos: [22]; default: 0;
 *  write 1 to enable gamma frame done
 */
#define ISP_GAMMA_FRAME_INT_ENA    (BIT(22))
#define ISP_GAMMA_FRAME_INT_ENA_M  (ISP_GAMMA_FRAME_INT_ENA_V << ISP_GAMMA_FRAME_INT_ENA_S)
#define ISP_GAMMA_FRAME_INT_ENA_V  0x00000001U
#define ISP_GAMMA_FRAME_INT_ENA_S  22
/** ISP_RGB2YUV_FRAME_INT_ENA : R/W; bitpos: [23]; default: 0;
 *  write 1 to enable rgb2yuv frame done
 */
#define ISP_RGB2YUV_FRAME_INT_ENA    (BIT(23))
#define ISP_RGB2YUV_FRAME_INT_ENA_M  (ISP_RGB2YUV_FRAME_INT_ENA_V << ISP_RGB2YUV_FRAME_INT_ENA_S)
#define ISP_RGB2YUV_FRAME_INT_ENA_V  0x00000001U
#define ISP_RGB2YUV_FRAME_INT_ENA_S  23
/** ISP_SHARP_FRAME_INT_ENA : R/W; bitpos: [24]; default: 0;
 *  write 1 to enable sharp frame done
 */
#define ISP_SHARP_FRAME_INT_ENA    (BIT(24))
#define ISP_SHARP_FRAME_INT_ENA_M  (ISP_SHARP_FRAME_INT_ENA_V << ISP_SHARP_FRAME_INT_ENA_S)
#define ISP_SHARP_FRAME_INT_ENA_V  0x00000001U
#define ISP_SHARP_FRAME_INT_ENA_S  24
/** ISP_COLOR_FRAME_INT_ENA : R/W; bitpos: [25]; default: 0;
 *  write 1 to enable color frame done
 */
#define ISP_COLOR_FRAME_INT_ENA    (BIT(25))
#define ISP_COLOR_FRAME_INT_ENA_M  (ISP_COLOR_FRAME_INT_ENA_V << ISP_COLOR_FRAME_INT_ENA_S)
#define ISP_COLOR_FRAME_INT_ENA_V  0x00000001U
#define ISP_COLOR_FRAME_INT_ENA_S  25
/** ISP_YUV2RGB_FRAME_INT_ENA : R/W; bitpos: [26]; default: 0;
 *  write 1 to enable yuv2rgb frame done
 */
#define ISP_YUV2RGB_FRAME_INT_ENA    (BIT(26))
#define ISP_YUV2RGB_FRAME_INT_ENA_M  (ISP_YUV2RGB_FRAME_INT_ENA_V << ISP_YUV2RGB_FRAME_INT_ENA_S)
#define ISP_YUV2RGB_FRAME_INT_ENA_V  0x00000001U
#define ISP_YUV2RGB_FRAME_INT_ENA_S  26
/** ISP_TAIL_IDI_FRAME_INT_ENA : R/W; bitpos: [27]; default: 0;
 *  write 1 to enable isp_tail idi frame_end
 */
#define ISP_TAIL_IDI_FRAME_INT_ENA    (BIT(27))
#define ISP_TAIL_IDI_FRAME_INT_ENA_M  (ISP_TAIL_IDI_FRAME_INT_ENA_V << ISP_TAIL_IDI_FRAME_INT_ENA_S)
#define ISP_TAIL_IDI_FRAME_INT_ENA_V  0x00000001U
#define ISP_TAIL_IDI_FRAME_INT_ENA_S  27
/** ISP_HEADER_IDI_FRAME_INT_ENA : R/W; bitpos: [28]; default: 0;
 *  write 1 to enable real input frame end of isp_input
 */
#define ISP_HEADER_IDI_FRAME_INT_ENA    (BIT(28))
#define ISP_HEADER_IDI_FRAME_INT_ENA_M  (ISP_HEADER_IDI_FRAME_INT_ENA_V << ISP_HEADER_IDI_FRAME_INT_ENA_S)
#define ISP_HEADER_IDI_FRAME_INT_ENA_V  0x00000001U
#define ISP_HEADER_IDI_FRAME_INT_ENA_S  28

/** ISP_INT_CLR_REG register
 *  interrupt clear register
 */
#define ISP_INT_CLR_REG (DR_REG_ISP_BASE + 0x70)
/** ISP_ISP_DATA_TYPE_ERR_INT_CLR : WT; bitpos: [0]; default: 0;
 *  write 1 to clear input data type error
 */
#define ISP_ISP_DATA_TYPE_ERR_INT_CLR    (BIT(0))
#define ISP_ISP_DATA_TYPE_ERR_INT_CLR_M  (ISP_ISP_DATA_TYPE_ERR_INT_CLR_V << ISP_ISP_DATA_TYPE_ERR_INT_CLR_S)
#define ISP_ISP_DATA_TYPE_ERR_INT_CLR_V  0x00000001U
#define ISP_ISP_DATA_TYPE_ERR_INT_CLR_S  0
/** ISP_ISP_ASYNC_FIFO_OVF_INT_CLR : WT; bitpos: [1]; default: 0;
 *  write 1 to clear isp input fifo overflow
 */
#define ISP_ISP_ASYNC_FIFO_OVF_INT_CLR    (BIT(1))
#define ISP_ISP_ASYNC_FIFO_OVF_INT_CLR_M  (ISP_ISP_ASYNC_FIFO_OVF_INT_CLR_V << ISP_ISP_ASYNC_FIFO_OVF_INT_CLR_S)
#define ISP_ISP_ASYNC_FIFO_OVF_INT_CLR_V  0x00000001U
#define ISP_ISP_ASYNC_FIFO_OVF_INT_CLR_S  1
/** ISP_ISP_BUF_FULL_INT_CLR : WT; bitpos: [2]; default: 0;
 *  write 1 to clear isp input buffer full
 */
#define ISP_ISP_BUF_FULL_INT_CLR    (BIT(2))
#define ISP_ISP_BUF_FULL_INT_CLR_M  (ISP_ISP_BUF_FULL_INT_CLR_V << ISP_ISP_BUF_FULL_INT_CLR_S)
#define ISP_ISP_BUF_FULL_INT_CLR_V  0x00000001U
#define ISP_ISP_BUF_FULL_INT_CLR_S  2
/** ISP_ISP_HVNUM_SETTING_ERR_INT_CLR : WT; bitpos: [3]; default: 0;
 *  write 1 to clear hnum and vnum setting format error
 */
#define ISP_ISP_HVNUM_SETTING_ERR_INT_CLR    (BIT(3))
#define ISP_ISP_HVNUM_SETTING_ERR_INT_CLR_M  (ISP_ISP_HVNUM_SETTING_ERR_INT_CLR_V << ISP_ISP_HVNUM_SETTING_ERR_INT_CLR_S)
#define ISP_ISP_HVNUM_SETTING_ERR_INT_CLR_V  0x00000001U
#define ISP_ISP_HVNUM_SETTING_ERR_INT_CLR_S  3
/** ISP_ISP_DATA_TYPE_SETTING_ERR_INT_CLR : WT; bitpos: [4]; default: 0;
 *  write 1 to clear setting invalid reg_data_type
 */
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_CLR    (BIT(4))
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_CLR_M  (ISP_ISP_DATA_TYPE_SETTING_ERR_INT_CLR_V << ISP_ISP_DATA_TYPE_SETTING_ERR_INT_CLR_S)
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_CLR_V  0x00000001U
#define ISP_ISP_DATA_TYPE_SETTING_ERR_INT_CLR_S  4
/** ISP_ISP_MIPI_HNUM_UNMATCH_INT_CLR : WT; bitpos: [5]; default: 0;
 *  write 1 to clear hnum setting unmatch with mipi input
 */
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_CLR    (BIT(5))
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_CLR_M  (ISP_ISP_MIPI_HNUM_UNMATCH_INT_CLR_V << ISP_ISP_MIPI_HNUM_UNMATCH_INT_CLR_S)
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_CLR_V  0x00000001U
#define ISP_ISP_MIPI_HNUM_UNMATCH_INT_CLR_S  5
/** ISP_DPC_CHECK_DONE_INT_CLR : WT; bitpos: [6]; default: 0;
 *  write 1 to clear dpc check done
 */
#define ISP_DPC_CHECK_DONE_INT_CLR    (BIT(6))
#define ISP_DPC_CHECK_DONE_INT_CLR_M  (ISP_DPC_CHECK_DONE_INT_CLR_V << ISP_DPC_CHECK_DONE_INT_CLR_S)
#define ISP_DPC_CHECK_DONE_INT_CLR_V  0x00000001U
#define ISP_DPC_CHECK_DONE_INT_CLR_S  6
/** ISP_GAMMA_XCOORD_ERR_INT_CLR : WT; bitpos: [7]; default: 0;
 *  write 1 to clear gamma setting error
 */
#define ISP_GAMMA_XCOORD_ERR_INT_CLR    (BIT(7))
#define ISP_GAMMA_XCOORD_ERR_INT_CLR_M  (ISP_GAMMA_XCOORD_ERR_INT_CLR_V << ISP_GAMMA_XCOORD_ERR_INT_CLR_S)
#define ISP_GAMMA_XCOORD_ERR_INT_CLR_V  0x00000001U
#define ISP_GAMMA_XCOORD_ERR_INT_CLR_S  7
/** ISP_AE_MONITOR_INT_CLR : WT; bitpos: [8]; default: 0;
 *  write 1 to clear ae monitor
 */
#define ISP_AE_MONITOR_INT_CLR    (BIT(8))
#define ISP_AE_MONITOR_INT_CLR_M  (ISP_AE_MONITOR_INT_CLR_V << ISP_AE_MONITOR_INT_CLR_S)
#define ISP_AE_MONITOR_INT_CLR_V  0x00000001U
#define ISP_AE_MONITOR_INT_CLR_S  8
/** ISP_AE_FRAME_DONE_INT_CLR : WT; bitpos: [9]; default: 0;
 *  write 1 to clear ae
 */
#define ISP_AE_FRAME_DONE_INT_CLR    (BIT(9))
#define ISP_AE_FRAME_DONE_INT_CLR_M  (ISP_AE_FRAME_DONE_INT_CLR_V << ISP_AE_FRAME_DONE_INT_CLR_S)
#define ISP_AE_FRAME_DONE_INT_CLR_V  0x00000001U
#define ISP_AE_FRAME_DONE_INT_CLR_S  9
/** ISP_AF_FDONE_INT_CLR : WT; bitpos: [10]; default: 0;
 *  write 1 to clear af statistic
 */
#define ISP_AF_FDONE_INT_CLR    (BIT(10))
#define ISP_AF_FDONE_INT_CLR_M  (ISP_AF_FDONE_INT_CLR_V << ISP_AF_FDONE_INT_CLR_S)
#define ISP_AF_FDONE_INT_CLR_V  0x00000001U
#define ISP_AF_FDONE_INT_CLR_S  10
/** ISP_AF_ENV_INT_CLR : WT; bitpos: [11]; default: 0;
 *  write 1 to clear af monitor
 */
#define ISP_AF_ENV_INT_CLR    (BIT(11))
#define ISP_AF_ENV_INT_CLR_M  (ISP_AF_ENV_INT_CLR_V << ISP_AF_ENV_INT_CLR_S)
#define ISP_AF_ENV_INT_CLR_V  0x00000001U
#define ISP_AF_ENV_INT_CLR_S  11
/** ISP_AWB_FDONE_INT_CLR : WT; bitpos: [12]; default: 0;
 *  write 1 to clear awb
 */
#define ISP_AWB_FDONE_INT_CLR    (BIT(12))
#define ISP_AWB_FDONE_INT_CLR_M  (ISP_AWB_FDONE_INT_CLR_V << ISP_AWB_FDONE_INT_CLR_S)
#define ISP_AWB_FDONE_INT_CLR_V  0x00000001U
#define ISP_AWB_FDONE_INT_CLR_S  12
/** ISP_HIST_FDONE_INT_CLR : WT; bitpos: [13]; default: 0;
 *  write 1 to clear histogram
 */
#define ISP_HIST_FDONE_INT_CLR    (BIT(13))
#define ISP_HIST_FDONE_INT_CLR_M  (ISP_HIST_FDONE_INT_CLR_V << ISP_HIST_FDONE_INT_CLR_S)
#define ISP_HIST_FDONE_INT_CLR_V  0x00000001U
#define ISP_HIST_FDONE_INT_CLR_S  13
/** ISP_FRAME_INT_CLR : WT; bitpos: [14]; default: 0;
 *  write 1 to clear isp frame end
 */
#define ISP_FRAME_INT_CLR    (BIT(14))
#define ISP_FRAME_INT_CLR_M  (ISP_FRAME_INT_CLR_V << ISP_FRAME_INT_CLR_S)
#define ISP_FRAME_INT_CLR_V  0x00000001U
#define ISP_FRAME_INT_CLR_S  14
/** ISP_BLC_FRAME_INT_CLR : WT; bitpos: [15]; default: 0;
 *  write 1 to clear blc frame done
 */
#define ISP_BLC_FRAME_INT_CLR    (BIT(15))
#define ISP_BLC_FRAME_INT_CLR_M  (ISP_BLC_FRAME_INT_CLR_V << ISP_BLC_FRAME_INT_CLR_S)
#define ISP_BLC_FRAME_INT_CLR_V  0x00000001U
#define ISP_BLC_FRAME_INT_CLR_S  15
/** ISP_LSC_FRAME_INT_CLR : WT; bitpos: [16]; default: 0;
 *  write 1 to clear lsc frame done
 */
#define ISP_LSC_FRAME_INT_CLR    (BIT(16))
#define ISP_LSC_FRAME_INT_CLR_M  (ISP_LSC_FRAME_INT_CLR_V << ISP_LSC_FRAME_INT_CLR_S)
#define ISP_LSC_FRAME_INT_CLR_V  0x00000001U
#define ISP_LSC_FRAME_INT_CLR_S  16
/** ISP_DPC_FRAME_INT_CLR : WT; bitpos: [17]; default: 0;
 *  write 1 to clear dpc frame done
 */
#define ISP_DPC_FRAME_INT_CLR    (BIT(17))
#define ISP_DPC_FRAME_INT_CLR_M  (ISP_DPC_FRAME_INT_CLR_V << ISP_DPC_FRAME_INT_CLR_S)
#define ISP_DPC_FRAME_INT_CLR_V  0x00000001U
#define ISP_DPC_FRAME_INT_CLR_S  17
/** ISP_BF_FRAME_INT_CLR : WT; bitpos: [18]; default: 0;
 *  write 1 to clear bf frame done
 */
#define ISP_BF_FRAME_INT_CLR    (BIT(18))
#define ISP_BF_FRAME_INT_CLR_M  (ISP_BF_FRAME_INT_CLR_V << ISP_BF_FRAME_INT_CLR_S)
#define ISP_BF_FRAME_INT_CLR_V  0x00000001U
#define ISP_BF_FRAME_INT_CLR_S  18
/** ISP_DEMOSAIC_FRAME_INT_CLR : WT; bitpos: [19]; default: 0;
 *  write 1 to clear demosaic frame done
 */
#define ISP_DEMOSAIC_FRAME_INT_CLR    (BIT(19))
#define ISP_DEMOSAIC_FRAME_INT_CLR_M  (ISP_DEMOSAIC_FRAME_INT_CLR_V << ISP_DEMOSAIC_FRAME_INT_CLR_S)
#define ISP_DEMOSAIC_FRAME_INT_CLR_V  0x00000001U
#define ISP_DEMOSAIC_FRAME_INT_CLR_S  19
/** ISP_MEDIAN_FRAME_INT_CLR : WT; bitpos: [20]; default: 0;
 *  write 1 to clear median frame done
 */
#define ISP_MEDIAN_FRAME_INT_CLR    (BIT(20))
#define ISP_MEDIAN_FRAME_INT_CLR_M  (ISP_MEDIAN_FRAME_INT_CLR_V << ISP_MEDIAN_FRAME_INT_CLR_S)
#define ISP_MEDIAN_FRAME_INT_CLR_V  0x00000001U
#define ISP_MEDIAN_FRAME_INT_CLR_S  20
/** ISP_CCM_FRAME_INT_CLR : WT; bitpos: [21]; default: 0;
 *  write 1 to clear ccm frame done
 */
#define ISP_CCM_FRAME_INT_CLR    (BIT(21))
#define ISP_CCM_FRAME_INT_CLR_M  (ISP_CCM_FRAME_INT_CLR_V << ISP_CCM_FRAME_INT_CLR_S)
#define ISP_CCM_FRAME_INT_CLR_V  0x00000001U
#define ISP_CCM_FRAME_INT_CLR_S  21
/** ISP_GAMMA_FRAME_INT_CLR : WT; bitpos: [22]; default: 0;
 *  write 1 to clear gamma frame done
 */
#define ISP_GAMMA_FRAME_INT_CLR    (BIT(22))
#define ISP_GAMMA_FRAME_INT_CLR_M  (ISP_GAMMA_FRAME_INT_CLR_V << ISP_GAMMA_FRAME_INT_CLR_S)
#define ISP_GAMMA_FRAME_INT_CLR_V  0x00000001U
#define ISP_GAMMA_FRAME_INT_CLR_S  22
/** ISP_RGB2YUV_FRAME_INT_CLR : WT; bitpos: [23]; default: 0;
 *  write 1 to clear rgb2yuv frame done
 */
#define ISP_RGB2YUV_FRAME_INT_CLR    (BIT(23))
#define ISP_RGB2YUV_FRAME_INT_CLR_M  (ISP_RGB2YUV_FRAME_INT_CLR_V << ISP_RGB2YUV_FRAME_INT_CLR_S)
#define ISP_RGB2YUV_FRAME_INT_CLR_V  0x00000001U
#define ISP_RGB2YUV_FRAME_INT_CLR_S  23
/** ISP_SHARP_FRAME_INT_CLR : WT; bitpos: [24]; default: 0;
 *  write 1 to clear sharp frame done
 */
#define ISP_SHARP_FRAME_INT_CLR    (BIT(24))
#define ISP_SHARP_FRAME_INT_CLR_M  (ISP_SHARP_FRAME_INT_CLR_V << ISP_SHARP_FRAME_INT_CLR_S)
#define ISP_SHARP_FRAME_INT_CLR_V  0x00000001U
#define ISP_SHARP_FRAME_INT_CLR_S  24
/** ISP_COLOR_FRAME_INT_CLR : WT; bitpos: [25]; default: 0;
 *  write 1 to clear color frame done
 */
#define ISP_COLOR_FRAME_INT_CLR    (BIT(25))
#define ISP_COLOR_FRAME_INT_CLR_M  (ISP_COLOR_FRAME_INT_CLR_V << ISP_COLOR_FRAME_INT_CLR_S)
#define ISP_COLOR_FRAME_INT_CLR_V  0x00000001U
#define ISP_COLOR_FRAME_INT_CLR_S  25
/** ISP_YUV2RGB_FRAME_INT_CLR : WT; bitpos: [26]; default: 0;
 *  write 1 to clear yuv2rgb frame done
 */
#define ISP_YUV2RGB_FRAME_INT_CLR    (BIT(26))
#define ISP_YUV2RGB_FRAME_INT_CLR_M  (ISP_YUV2RGB_FRAME_INT_CLR_V << ISP_YUV2RGB_FRAME_INT_CLR_S)
#define ISP_YUV2RGB_FRAME_INT_CLR_V  0x00000001U
#define ISP_YUV2RGB_FRAME_INT_CLR_S  26
/** ISP_TAIL_IDI_FRAME_INT_CLR : WT; bitpos: [27]; default: 0;
 *  write 1 to clear isp_tail idi frame_end
 */
#define ISP_TAIL_IDI_FRAME_INT_CLR    (BIT(27))
#define ISP_TAIL_IDI_FRAME_INT_CLR_M  (ISP_TAIL_IDI_FRAME_INT_CLR_V << ISP_TAIL_IDI_FRAME_INT_CLR_S)
#define ISP_TAIL_IDI_FRAME_INT_CLR_V  0x00000001U
#define ISP_TAIL_IDI_FRAME_INT_CLR_S  27
/** ISP_HEADER_IDI_FRAME_INT_CLR : WT; bitpos: [28]; default: 0;
 *  write 1 to clear real input frame end of isp_input
 */
#define ISP_HEADER_IDI_FRAME_INT_CLR    (BIT(28))
#define ISP_HEADER_IDI_FRAME_INT_CLR_M  (ISP_HEADER_IDI_FRAME_INT_CLR_V << ISP_HEADER_IDI_FRAME_INT_CLR_S)
#define ISP_HEADER_IDI_FRAME_INT_CLR_V  0x00000001U
#define ISP_HEADER_IDI_FRAME_INT_CLR_S  28

/** ISP_GAMMA_CTRL_REG register
 *  gamma control register
 */
#define ISP_GAMMA_CTRL_REG (DR_REG_ISP_BASE + 0x74)
/** ISP_GAMMA_UPDATE : R/W; bitpos: [0]; default: 0;
 *  Indicates that gamma register configuration is complete
 */
#define ISP_GAMMA_UPDATE    (BIT(0))
#define ISP_GAMMA_UPDATE_M  (ISP_GAMMA_UPDATE_V << ISP_GAMMA_UPDATE_S)
#define ISP_GAMMA_UPDATE_V  0x00000001U
#define ISP_GAMMA_UPDATE_S  0
/** ISP_GAMMA_B_LAST_CORRECT : R/W; bitpos: [1]; default: 1;
 *  this bit configures enable of last b segment correcction. 0: disable, 1: enable
 */
#define ISP_GAMMA_B_LAST_CORRECT    (BIT(1))
#define ISP_GAMMA_B_LAST_CORRECT_M  (ISP_GAMMA_B_LAST_CORRECT_V << ISP_GAMMA_B_LAST_CORRECT_S)
#define ISP_GAMMA_B_LAST_CORRECT_V  0x00000001U
#define ISP_GAMMA_B_LAST_CORRECT_S  1
/** ISP_GAMMA_G_LAST_CORRECT : R/W; bitpos: [2]; default: 1;
 *  this bit configures enable of last g segment correcction. 0: disable, 1: enable
 */
#define ISP_GAMMA_G_LAST_CORRECT    (BIT(2))
#define ISP_GAMMA_G_LAST_CORRECT_M  (ISP_GAMMA_G_LAST_CORRECT_V << ISP_GAMMA_G_LAST_CORRECT_S)
#define ISP_GAMMA_G_LAST_CORRECT_V  0x00000001U
#define ISP_GAMMA_G_LAST_CORRECT_S  2
/** ISP_GAMMA_R_LAST_CORRECT : R/W; bitpos: [3]; default: 1;
 *  this bit configures enable of last r segment correcction. 0: disable, 1: enable
 */
#define ISP_GAMMA_R_LAST_CORRECT    (BIT(3))
#define ISP_GAMMA_R_LAST_CORRECT_M  (ISP_GAMMA_R_LAST_CORRECT_V << ISP_GAMMA_R_LAST_CORRECT_S)
#define ISP_GAMMA_R_LAST_CORRECT_V  0x00000001U
#define ISP_GAMMA_R_LAST_CORRECT_S  3

/** ISP_GAMMA_RY1_REG register
 *  point of Y-axis of r channel gamma curve register 1
 */
#define ISP_GAMMA_RY1_REG (DR_REG_ISP_BASE + 0x78)
/** ISP_GAMMA_R_Y03 : R/W; bitpos: [7:0]; default: 64;
 *  this field configures the point 3 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y03    0x000000FFU
#define ISP_GAMMA_R_Y03_M  (ISP_GAMMA_R_Y03_V << ISP_GAMMA_R_Y03_S)
#define ISP_GAMMA_R_Y03_V  0x000000FFU
#define ISP_GAMMA_R_Y03_S  0
/** ISP_GAMMA_R_Y02 : R/W; bitpos: [15:8]; default: 48;
 *  this field configures the point 2 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y02    0x000000FFU
#define ISP_GAMMA_R_Y02_M  (ISP_GAMMA_R_Y02_V << ISP_GAMMA_R_Y02_S)
#define ISP_GAMMA_R_Y02_V  0x000000FFU
#define ISP_GAMMA_R_Y02_S  8
/** ISP_GAMMA_R_Y01 : R/W; bitpos: [23:16]; default: 32;
 *  this field configures the point 1 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y01    0x000000FFU
#define ISP_GAMMA_R_Y01_M  (ISP_GAMMA_R_Y01_V << ISP_GAMMA_R_Y01_S)
#define ISP_GAMMA_R_Y01_V  0x000000FFU
#define ISP_GAMMA_R_Y01_S  16
/** ISP_GAMMA_R_Y00 : R/W; bitpos: [31:24]; default: 16;
 *  this field configures the point 0 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y00    0x000000FFU
#define ISP_GAMMA_R_Y00_M  (ISP_GAMMA_R_Y00_V << ISP_GAMMA_R_Y00_S)
#define ISP_GAMMA_R_Y00_V  0x000000FFU
#define ISP_GAMMA_R_Y00_S  24

/** ISP_GAMMA_RY2_REG register
 *  point of Y-axis of r channel gamma curve register 2
 */
#define ISP_GAMMA_RY2_REG (DR_REG_ISP_BASE + 0x7c)
/** ISP_GAMMA_R_Y07 : R/W; bitpos: [7:0]; default: 128;
 *  this field configures the point 7 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y07    0x000000FFU
#define ISP_GAMMA_R_Y07_M  (ISP_GAMMA_R_Y07_V << ISP_GAMMA_R_Y07_S)
#define ISP_GAMMA_R_Y07_V  0x000000FFU
#define ISP_GAMMA_R_Y07_S  0
/** ISP_GAMMA_R_Y06 : R/W; bitpos: [15:8]; default: 112;
 *  this field configures the point 6 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y06    0x000000FFU
#define ISP_GAMMA_R_Y06_M  (ISP_GAMMA_R_Y06_V << ISP_GAMMA_R_Y06_S)
#define ISP_GAMMA_R_Y06_V  0x000000FFU
#define ISP_GAMMA_R_Y06_S  8
/** ISP_GAMMA_R_Y05 : R/W; bitpos: [23:16]; default: 96;
 *  this field configures the point 5 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y05    0x000000FFU
#define ISP_GAMMA_R_Y05_M  (ISP_GAMMA_R_Y05_V << ISP_GAMMA_R_Y05_S)
#define ISP_GAMMA_R_Y05_V  0x000000FFU
#define ISP_GAMMA_R_Y05_S  16
/** ISP_GAMMA_R_Y04 : R/W; bitpos: [31:24]; default: 80;
 *  this field configures the point 4 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y04    0x000000FFU
#define ISP_GAMMA_R_Y04_M  (ISP_GAMMA_R_Y04_V << ISP_GAMMA_R_Y04_S)
#define ISP_GAMMA_R_Y04_V  0x000000FFU
#define ISP_GAMMA_R_Y04_S  24

/** ISP_GAMMA_RY3_REG register
 *  point of Y-axis of r channel gamma curve register 3
 */
#define ISP_GAMMA_RY3_REG (DR_REG_ISP_BASE + 0x80)
/** ISP_GAMMA_R_Y0B : R/W; bitpos: [7:0]; default: 192;
 *  this field configures the point 11 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y0B    0x000000FFU
#define ISP_GAMMA_R_Y0B_M  (ISP_GAMMA_R_Y0B_V << ISP_GAMMA_R_Y0B_S)
#define ISP_GAMMA_R_Y0B_V  0x000000FFU
#define ISP_GAMMA_R_Y0B_S  0
/** ISP_GAMMA_R_Y0A : R/W; bitpos: [15:8]; default: 176;
 *  this field configures the point 10 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y0A    0x000000FFU
#define ISP_GAMMA_R_Y0A_M  (ISP_GAMMA_R_Y0A_V << ISP_GAMMA_R_Y0A_S)
#define ISP_GAMMA_R_Y0A_V  0x000000FFU
#define ISP_GAMMA_R_Y0A_S  8
/** ISP_GAMMA_R_Y09 : R/W; bitpos: [23:16]; default: 160;
 *  this field configures the point 9 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y09    0x000000FFU
#define ISP_GAMMA_R_Y09_M  (ISP_GAMMA_R_Y09_V << ISP_GAMMA_R_Y09_S)
#define ISP_GAMMA_R_Y09_V  0x000000FFU
#define ISP_GAMMA_R_Y09_S  16
/** ISP_GAMMA_R_Y08 : R/W; bitpos: [31:24]; default: 144;
 *  this field configures the point 8 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y08    0x000000FFU
#define ISP_GAMMA_R_Y08_M  (ISP_GAMMA_R_Y08_V << ISP_GAMMA_R_Y08_S)
#define ISP_GAMMA_R_Y08_V  0x000000FFU
#define ISP_GAMMA_R_Y08_S  24

/** ISP_GAMMA_RY4_REG register
 *  point of Y-axis of r channel gamma curve register 4
 */
#define ISP_GAMMA_RY4_REG (DR_REG_ISP_BASE + 0x84)
/** ISP_GAMMA_R_Y0F : R/W; bitpos: [7:0]; default: 255;
 *  this field configures the point 15 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y0F    0x000000FFU
#define ISP_GAMMA_R_Y0F_M  (ISP_GAMMA_R_Y0F_V << ISP_GAMMA_R_Y0F_S)
#define ISP_GAMMA_R_Y0F_V  0x000000FFU
#define ISP_GAMMA_R_Y0F_S  0
/** ISP_GAMMA_R_Y0E : R/W; bitpos: [15:8]; default: 240;
 *  this field configures the point 14 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y0E    0x000000FFU
#define ISP_GAMMA_R_Y0E_M  (ISP_GAMMA_R_Y0E_V << ISP_GAMMA_R_Y0E_S)
#define ISP_GAMMA_R_Y0E_V  0x000000FFU
#define ISP_GAMMA_R_Y0E_S  8
/** ISP_GAMMA_R_Y0D : R/W; bitpos: [23:16]; default: 224;
 *  this field configures the point 13 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y0D    0x000000FFU
#define ISP_GAMMA_R_Y0D_M  (ISP_GAMMA_R_Y0D_V << ISP_GAMMA_R_Y0D_S)
#define ISP_GAMMA_R_Y0D_V  0x000000FFU
#define ISP_GAMMA_R_Y0D_S  16
/** ISP_GAMMA_R_Y0C : R/W; bitpos: [31:24]; default: 208;
 *  this field configures the point 12 of Y-axis of r channel gamma curve
 */
#define ISP_GAMMA_R_Y0C    0x000000FFU
#define ISP_GAMMA_R_Y0C_M  (ISP_GAMMA_R_Y0C_V << ISP_GAMMA_R_Y0C_S)
#define ISP_GAMMA_R_Y0C_V  0x000000FFU
#define ISP_GAMMA_R_Y0C_S  24

/** ISP_GAMMA_GY1_REG register
 *  point of Y-axis of g channel gamma curve register 1
 */
#define ISP_GAMMA_GY1_REG (DR_REG_ISP_BASE + 0x88)
/** ISP_GAMMA_G_Y03 : R/W; bitpos: [7:0]; default: 64;
 *  this field configures the point 3 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y03    0x000000FFU
#define ISP_GAMMA_G_Y03_M  (ISP_GAMMA_G_Y03_V << ISP_GAMMA_G_Y03_S)
#define ISP_GAMMA_G_Y03_V  0x000000FFU
#define ISP_GAMMA_G_Y03_S  0
/** ISP_GAMMA_G_Y02 : R/W; bitpos: [15:8]; default: 48;
 *  this field configures the point 2 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y02    0x000000FFU
#define ISP_GAMMA_G_Y02_M  (ISP_GAMMA_G_Y02_V << ISP_GAMMA_G_Y02_S)
#define ISP_GAMMA_G_Y02_V  0x000000FFU
#define ISP_GAMMA_G_Y02_S  8
/** ISP_GAMMA_G_Y01 : R/W; bitpos: [23:16]; default: 32;
 *  this field configures the point 1 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y01    0x000000FFU
#define ISP_GAMMA_G_Y01_M  (ISP_GAMMA_G_Y01_V << ISP_GAMMA_G_Y01_S)
#define ISP_GAMMA_G_Y01_V  0x000000FFU
#define ISP_GAMMA_G_Y01_S  16
/** ISP_GAMMA_G_Y00 : R/W; bitpos: [31:24]; default: 16;
 *  this field configures the point 0 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y00    0x000000FFU
#define ISP_GAMMA_G_Y00_M  (ISP_GAMMA_G_Y00_V << ISP_GAMMA_G_Y00_S)
#define ISP_GAMMA_G_Y00_V  0x000000FFU
#define ISP_GAMMA_G_Y00_S  24

/** ISP_GAMMA_GY2_REG register
 *  point of Y-axis of g channel gamma curve register 2
 */
#define ISP_GAMMA_GY2_REG (DR_REG_ISP_BASE + 0x8c)
/** ISP_GAMMA_G_Y07 : R/W; bitpos: [7:0]; default: 128;
 *  this field configures the point 7 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y07    0x000000FFU
#define ISP_GAMMA_G_Y07_M  (ISP_GAMMA_G_Y07_V << ISP_GAMMA_G_Y07_S)
#define ISP_GAMMA_G_Y07_V  0x000000FFU
#define ISP_GAMMA_G_Y07_S  0
/** ISP_GAMMA_G_Y06 : R/W; bitpos: [15:8]; default: 112;
 *  this field configures the point 6 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y06    0x000000FFU
#define ISP_GAMMA_G_Y06_M  (ISP_GAMMA_G_Y06_V << ISP_GAMMA_G_Y06_S)
#define ISP_GAMMA_G_Y06_V  0x000000FFU
#define ISP_GAMMA_G_Y06_S  8
/** ISP_GAMMA_G_Y05 : R/W; bitpos: [23:16]; default: 96;
 *  this field configures the point 5 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y05    0x000000FFU
#define ISP_GAMMA_G_Y05_M  (ISP_GAMMA_G_Y05_V << ISP_GAMMA_G_Y05_S)
#define ISP_GAMMA_G_Y05_V  0x000000FFU
#define ISP_GAMMA_G_Y05_S  16
/** ISP_GAMMA_G_Y04 : R/W; bitpos: [31:24]; default: 80;
 *  this field configures the point 4 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y04    0x000000FFU
#define ISP_GAMMA_G_Y04_M  (ISP_GAMMA_G_Y04_V << ISP_GAMMA_G_Y04_S)
#define ISP_GAMMA_G_Y04_V  0x000000FFU
#define ISP_GAMMA_G_Y04_S  24

/** ISP_GAMMA_GY3_REG register
 *  point of Y-axis of g channel gamma curve register 3
 */
#define ISP_GAMMA_GY3_REG (DR_REG_ISP_BASE + 0x90)
/** ISP_GAMMA_G_Y0B : R/W; bitpos: [7:0]; default: 192;
 *  this field configures the point 11 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y0B    0x000000FFU
#define ISP_GAMMA_G_Y0B_M  (ISP_GAMMA_G_Y0B_V << ISP_GAMMA_G_Y0B_S)
#define ISP_GAMMA_G_Y0B_V  0x000000FFU
#define ISP_GAMMA_G_Y0B_S  0
/** ISP_GAMMA_G_Y0A : R/W; bitpos: [15:8]; default: 176;
 *  this field configures the point 10 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y0A    0x000000FFU
#define ISP_GAMMA_G_Y0A_M  (ISP_GAMMA_G_Y0A_V << ISP_GAMMA_G_Y0A_S)
#define ISP_GAMMA_G_Y0A_V  0x000000FFU
#define ISP_GAMMA_G_Y0A_S  8
/** ISP_GAMMA_G_Y09 : R/W; bitpos: [23:16]; default: 160;
 *  this field configures the point 9 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y09    0x000000FFU
#define ISP_GAMMA_G_Y09_M  (ISP_GAMMA_G_Y09_V << ISP_GAMMA_G_Y09_S)
#define ISP_GAMMA_G_Y09_V  0x000000FFU
#define ISP_GAMMA_G_Y09_S  16
/** ISP_GAMMA_G_Y08 : R/W; bitpos: [31:24]; default: 144;
 *  this field configures the point 8 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y08    0x000000FFU
#define ISP_GAMMA_G_Y08_M  (ISP_GAMMA_G_Y08_V << ISP_GAMMA_G_Y08_S)
#define ISP_GAMMA_G_Y08_V  0x000000FFU
#define ISP_GAMMA_G_Y08_S  24

/** ISP_GAMMA_GY4_REG register
 *  point of Y-axis of g channel gamma curve register 4
 */
#define ISP_GAMMA_GY4_REG (DR_REG_ISP_BASE + 0x94)
/** ISP_GAMMA_G_Y0F : R/W; bitpos: [7:0]; default: 255;
 *  this field configures the point 15 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y0F    0x000000FFU
#define ISP_GAMMA_G_Y0F_M  (ISP_GAMMA_G_Y0F_V << ISP_GAMMA_G_Y0F_S)
#define ISP_GAMMA_G_Y0F_V  0x000000FFU
#define ISP_GAMMA_G_Y0F_S  0
/** ISP_GAMMA_G_Y0E : R/W; bitpos: [15:8]; default: 240;
 *  this field configures the point 14 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y0E    0x000000FFU
#define ISP_GAMMA_G_Y0E_M  (ISP_GAMMA_G_Y0E_V << ISP_GAMMA_G_Y0E_S)
#define ISP_GAMMA_G_Y0E_V  0x000000FFU
#define ISP_GAMMA_G_Y0E_S  8
/** ISP_GAMMA_G_Y0D : R/W; bitpos: [23:16]; default: 224;
 *  this field configures the point 13 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y0D    0x000000FFU
#define ISP_GAMMA_G_Y0D_M  (ISP_GAMMA_G_Y0D_V << ISP_GAMMA_G_Y0D_S)
#define ISP_GAMMA_G_Y0D_V  0x000000FFU
#define ISP_GAMMA_G_Y0D_S  16
/** ISP_GAMMA_G_Y0C : R/W; bitpos: [31:24]; default: 208;
 *  this field configures the point 12 of Y-axis of g channel gamma curve
 */
#define ISP_GAMMA_G_Y0C    0x000000FFU
#define ISP_GAMMA_G_Y0C_M  (ISP_GAMMA_G_Y0C_V << ISP_GAMMA_G_Y0C_S)
#define ISP_GAMMA_G_Y0C_V  0x000000FFU
#define ISP_GAMMA_G_Y0C_S  24

/** ISP_GAMMA_BY1_REG register
 *  point of Y-axis of b channel gamma curve register 1
 */
#define ISP_GAMMA_BY1_REG (DR_REG_ISP_BASE + 0x98)
/** ISP_GAMMA_B_Y03 : R/W; bitpos: [7:0]; default: 64;
 *  this field configures the point 3 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y03    0x000000FFU
#define ISP_GAMMA_B_Y03_M  (ISP_GAMMA_B_Y03_V << ISP_GAMMA_B_Y03_S)
#define ISP_GAMMA_B_Y03_V  0x000000FFU
#define ISP_GAMMA_B_Y03_S  0
/** ISP_GAMMA_B_Y02 : R/W; bitpos: [15:8]; default: 48;
 *  this field configures the point 2 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y02    0x000000FFU
#define ISP_GAMMA_B_Y02_M  (ISP_GAMMA_B_Y02_V << ISP_GAMMA_B_Y02_S)
#define ISP_GAMMA_B_Y02_V  0x000000FFU
#define ISP_GAMMA_B_Y02_S  8
/** ISP_GAMMA_B_Y01 : R/W; bitpos: [23:16]; default: 32;
 *  this field configures the point 1 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y01    0x000000FFU
#define ISP_GAMMA_B_Y01_M  (ISP_GAMMA_B_Y01_V << ISP_GAMMA_B_Y01_S)
#define ISP_GAMMA_B_Y01_V  0x000000FFU
#define ISP_GAMMA_B_Y01_S  16
/** ISP_GAMMA_B_Y00 : R/W; bitpos: [31:24]; default: 16;
 *  this field configures the point 0 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y00    0x000000FFU
#define ISP_GAMMA_B_Y00_M  (ISP_GAMMA_B_Y00_V << ISP_GAMMA_B_Y00_S)
#define ISP_GAMMA_B_Y00_V  0x000000FFU
#define ISP_GAMMA_B_Y00_S  24

/** ISP_GAMMA_BY2_REG register
 *  point of Y-axis of b channel gamma curve register 2
 */
#define ISP_GAMMA_BY2_REG (DR_REG_ISP_BASE + 0x9c)
/** ISP_GAMMA_B_Y07 : R/W; bitpos: [7:0]; default: 128;
 *  this field configures the point 7 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y07    0x000000FFU
#define ISP_GAMMA_B_Y07_M  (ISP_GAMMA_B_Y07_V << ISP_GAMMA_B_Y07_S)
#define ISP_GAMMA_B_Y07_V  0x000000FFU
#define ISP_GAMMA_B_Y07_S  0
/** ISP_GAMMA_B_Y06 : R/W; bitpos: [15:8]; default: 112;
 *  this field configures the point 6 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y06    0x000000FFU
#define ISP_GAMMA_B_Y06_M  (ISP_GAMMA_B_Y06_V << ISP_GAMMA_B_Y06_S)
#define ISP_GAMMA_B_Y06_V  0x000000FFU
#define ISP_GAMMA_B_Y06_S  8
/** ISP_GAMMA_B_Y05 : R/W; bitpos: [23:16]; default: 96;
 *  this field configures the point 5 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y05    0x000000FFU
#define ISP_GAMMA_B_Y05_M  (ISP_GAMMA_B_Y05_V << ISP_GAMMA_B_Y05_S)
#define ISP_GAMMA_B_Y05_V  0x000000FFU
#define ISP_GAMMA_B_Y05_S  16
/** ISP_GAMMA_B_Y04 : R/W; bitpos: [31:24]; default: 80;
 *  this field configures the point 4 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y04    0x000000FFU
#define ISP_GAMMA_B_Y04_M  (ISP_GAMMA_B_Y04_V << ISP_GAMMA_B_Y04_S)
#define ISP_GAMMA_B_Y04_V  0x000000FFU
#define ISP_GAMMA_B_Y04_S  24

/** ISP_GAMMA_BY3_REG register
 *  point of Y-axis of b channel gamma curve register 3
 */
#define ISP_GAMMA_BY3_REG (DR_REG_ISP_BASE + 0xa0)
/** ISP_GAMMA_B_Y0B : R/W; bitpos: [7:0]; default: 192;
 *  this field configures the point 11 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y0B    0x000000FFU
#define ISP_GAMMA_B_Y0B_M  (ISP_GAMMA_B_Y0B_V << ISP_GAMMA_B_Y0B_S)
#define ISP_GAMMA_B_Y0B_V  0x000000FFU
#define ISP_GAMMA_B_Y0B_S  0
/** ISP_GAMMA_B_Y0A : R/W; bitpos: [15:8]; default: 176;
 *  this field configures the point 10 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y0A    0x000000FFU
#define ISP_GAMMA_B_Y0A_M  (ISP_GAMMA_B_Y0A_V << ISP_GAMMA_B_Y0A_S)
#define ISP_GAMMA_B_Y0A_V  0x000000FFU
#define ISP_GAMMA_B_Y0A_S  8
/** ISP_GAMMA_B_Y09 : R/W; bitpos: [23:16]; default: 160;
 *  this field configures the point 9 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y09    0x000000FFU
#define ISP_GAMMA_B_Y09_M  (ISP_GAMMA_B_Y09_V << ISP_GAMMA_B_Y09_S)
#define ISP_GAMMA_B_Y09_V  0x000000FFU
#define ISP_GAMMA_B_Y09_S  16
/** ISP_GAMMA_B_Y08 : R/W; bitpos: [31:24]; default: 144;
 *  this field configures the point 8 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y08    0x000000FFU
#define ISP_GAMMA_B_Y08_M  (ISP_GAMMA_B_Y08_V << ISP_GAMMA_B_Y08_S)
#define ISP_GAMMA_B_Y08_V  0x000000FFU
#define ISP_GAMMA_B_Y08_S  24

/** ISP_GAMMA_BY4_REG register
 *  point of Y-axis of b channel gamma curve register 4
 */
#define ISP_GAMMA_BY4_REG (DR_REG_ISP_BASE + 0xa4)
/** ISP_GAMMA_B_Y0F : R/W; bitpos: [7:0]; default: 255;
 *  this field configures the point 15 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y0F    0x000000FFU
#define ISP_GAMMA_B_Y0F_M  (ISP_GAMMA_B_Y0F_V << ISP_GAMMA_B_Y0F_S)
#define ISP_GAMMA_B_Y0F_V  0x000000FFU
#define ISP_GAMMA_B_Y0F_S  0
/** ISP_GAMMA_B_Y0E : R/W; bitpos: [15:8]; default: 240;
 *  this field configures the point 14 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y0E    0x000000FFU
#define ISP_GAMMA_B_Y0E_M  (ISP_GAMMA_B_Y0E_V << ISP_GAMMA_B_Y0E_S)
#define ISP_GAMMA_B_Y0E_V  0x000000FFU
#define ISP_GAMMA_B_Y0E_S  8
/** ISP_GAMMA_B_Y0D : R/W; bitpos: [23:16]; default: 224;
 *  this field configures the point 13 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y0D    0x000000FFU
#define ISP_GAMMA_B_Y0D_M  (ISP_GAMMA_B_Y0D_V << ISP_GAMMA_B_Y0D_S)
#define ISP_GAMMA_B_Y0D_V  0x000000FFU
#define ISP_GAMMA_B_Y0D_S  16
/** ISP_GAMMA_B_Y0C : R/W; bitpos: [31:24]; default: 208;
 *  this field configures the point 12 of Y-axis of b channel gamma curve
 */
#define ISP_GAMMA_B_Y0C    0x000000FFU
#define ISP_GAMMA_B_Y0C_M  (ISP_GAMMA_B_Y0C_V << ISP_GAMMA_B_Y0C_S)
#define ISP_GAMMA_B_Y0C_V  0x000000FFU
#define ISP_GAMMA_B_Y0C_S  24

/** ISP_GAMMA_RX1_REG register
 *  point of X-axis of r channel gamma curve register 1
 */
#define ISP_GAMMA_RX1_REG (DR_REG_ISP_BASE + 0xa8)
/** ISP_GAMMA_R_X07 : R/W; bitpos: [2:0]; default: 4;
 *  this field configures the point 7 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X07    0x00000007U
#define ISP_GAMMA_R_X07_M  (ISP_GAMMA_R_X07_V << ISP_GAMMA_R_X07_S)
#define ISP_GAMMA_R_X07_V  0x00000007U
#define ISP_GAMMA_R_X07_S  0
/** ISP_GAMMA_R_X06 : R/W; bitpos: [5:3]; default: 4;
 *  this field configures the point 6 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X06    0x00000007U
#define ISP_GAMMA_R_X06_M  (ISP_GAMMA_R_X06_V << ISP_GAMMA_R_X06_S)
#define ISP_GAMMA_R_X06_V  0x00000007U
#define ISP_GAMMA_R_X06_S  3
/** ISP_GAMMA_R_X05 : R/W; bitpos: [8:6]; default: 4;
 *  this field configures the point 5 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X05    0x00000007U
#define ISP_GAMMA_R_X05_M  (ISP_GAMMA_R_X05_V << ISP_GAMMA_R_X05_S)
#define ISP_GAMMA_R_X05_V  0x00000007U
#define ISP_GAMMA_R_X05_S  6
/** ISP_GAMMA_R_X04 : R/W; bitpos: [11:9]; default: 4;
 *  this field configures the point 4 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X04    0x00000007U
#define ISP_GAMMA_R_X04_M  (ISP_GAMMA_R_X04_V << ISP_GAMMA_R_X04_S)
#define ISP_GAMMA_R_X04_V  0x00000007U
#define ISP_GAMMA_R_X04_S  9
/** ISP_GAMMA_R_X03 : R/W; bitpos: [14:12]; default: 4;
 *  this field configures the point 3 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X03    0x00000007U
#define ISP_GAMMA_R_X03_M  (ISP_GAMMA_R_X03_V << ISP_GAMMA_R_X03_S)
#define ISP_GAMMA_R_X03_V  0x00000007U
#define ISP_GAMMA_R_X03_S  12
/** ISP_GAMMA_R_X02 : R/W; bitpos: [17:15]; default: 4;
 *  this field configures the point 2 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X02    0x00000007U
#define ISP_GAMMA_R_X02_M  (ISP_GAMMA_R_X02_V << ISP_GAMMA_R_X02_S)
#define ISP_GAMMA_R_X02_V  0x00000007U
#define ISP_GAMMA_R_X02_S  15
/** ISP_GAMMA_R_X01 : R/W; bitpos: [20:18]; default: 4;
 *  this field configures the point 1 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X01    0x00000007U
#define ISP_GAMMA_R_X01_M  (ISP_GAMMA_R_X01_V << ISP_GAMMA_R_X01_S)
#define ISP_GAMMA_R_X01_V  0x00000007U
#define ISP_GAMMA_R_X01_S  18
/** ISP_GAMMA_R_X00 : R/W; bitpos: [23:21]; default: 4;
 *  this field configures the point 0 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X00    0x00000007U
#define ISP_GAMMA_R_X00_M  (ISP_GAMMA_R_X00_V << ISP_GAMMA_R_X00_S)
#define ISP_GAMMA_R_X00_V  0x00000007U
#define ISP_GAMMA_R_X00_S  21

/** ISP_GAMMA_RX2_REG register
 *  point of X-axis of r channel gamma curve register 2
 */
#define ISP_GAMMA_RX2_REG (DR_REG_ISP_BASE + 0xac)
/** ISP_GAMMA_R_X0F : R/W; bitpos: [2:0]; default: 4;
 *  this field configures the point 15 of X-axis of r channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X0F    0x00000007U
#define ISP_GAMMA_R_X0F_M  (ISP_GAMMA_R_X0F_V << ISP_GAMMA_R_X0F_S)
#define ISP_GAMMA_R_X0F_V  0x00000007U
#define ISP_GAMMA_R_X0F_S  0
/** ISP_GAMMA_R_X0E : R/W; bitpos: [5:3]; default: 4;
 *  this field configures the point 14 of X-axis of r channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X0E    0x00000007U
#define ISP_GAMMA_R_X0E_M  (ISP_GAMMA_R_X0E_V << ISP_GAMMA_R_X0E_S)
#define ISP_GAMMA_R_X0E_V  0x00000007U
#define ISP_GAMMA_R_X0E_S  3
/** ISP_GAMMA_R_X0D : R/W; bitpos: [8:6]; default: 4;
 *  this field configures the point 13 of X-axis of r channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X0D    0x00000007U
#define ISP_GAMMA_R_X0D_M  (ISP_GAMMA_R_X0D_V << ISP_GAMMA_R_X0D_S)
#define ISP_GAMMA_R_X0D_V  0x00000007U
#define ISP_GAMMA_R_X0D_S  6
/** ISP_GAMMA_R_X0C : R/W; bitpos: [11:9]; default: 4;
 *  this field configures the point 12 of X-axis of r channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X0C    0x00000007U
#define ISP_GAMMA_R_X0C_M  (ISP_GAMMA_R_X0C_V << ISP_GAMMA_R_X0C_S)
#define ISP_GAMMA_R_X0C_V  0x00000007U
#define ISP_GAMMA_R_X0C_S  9
/** ISP_GAMMA_R_X0B : R/W; bitpos: [14:12]; default: 4;
 *  this field configures the point 11 of X-axis of r channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X0B    0x00000007U
#define ISP_GAMMA_R_X0B_M  (ISP_GAMMA_R_X0B_V << ISP_GAMMA_R_X0B_S)
#define ISP_GAMMA_R_X0B_V  0x00000007U
#define ISP_GAMMA_R_X0B_S  12
/** ISP_GAMMA_R_X0A : R/W; bitpos: [17:15]; default: 4;
 *  this field configures the point 10 of X-axis of r channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X0A    0x00000007U
#define ISP_GAMMA_R_X0A_M  (ISP_GAMMA_R_X0A_V << ISP_GAMMA_R_X0A_S)
#define ISP_GAMMA_R_X0A_V  0x00000007U
#define ISP_GAMMA_R_X0A_S  15
/** ISP_GAMMA_R_X09 : R/W; bitpos: [20:18]; default: 4;
 *  this field configures the point 9 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X09    0x00000007U
#define ISP_GAMMA_R_X09_M  (ISP_GAMMA_R_X09_V << ISP_GAMMA_R_X09_S)
#define ISP_GAMMA_R_X09_V  0x00000007U
#define ISP_GAMMA_R_X09_S  18
/** ISP_GAMMA_R_X08 : R/W; bitpos: [23:21]; default: 4;
 *  this field configures the point 8 of X-axis of r channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_R_X08    0x00000007U
#define ISP_GAMMA_R_X08_M  (ISP_GAMMA_R_X08_V << ISP_GAMMA_R_X08_S)
#define ISP_GAMMA_R_X08_V  0x00000007U
#define ISP_GAMMA_R_X08_S  21

/** ISP_GAMMA_GX1_REG register
 *  point of X-axis of g channel gamma curve register 1
 */
#define ISP_GAMMA_GX1_REG (DR_REG_ISP_BASE + 0xb0)
/** ISP_GAMMA_G_X07 : R/W; bitpos: [2:0]; default: 4;
 *  this field configures the point 7 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X07    0x00000007U
#define ISP_GAMMA_G_X07_M  (ISP_GAMMA_G_X07_V << ISP_GAMMA_G_X07_S)
#define ISP_GAMMA_G_X07_V  0x00000007U
#define ISP_GAMMA_G_X07_S  0
/** ISP_GAMMA_G_X06 : R/W; bitpos: [5:3]; default: 4;
 *  this field configures the point 6 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X06    0x00000007U
#define ISP_GAMMA_G_X06_M  (ISP_GAMMA_G_X06_V << ISP_GAMMA_G_X06_S)
#define ISP_GAMMA_G_X06_V  0x00000007U
#define ISP_GAMMA_G_X06_S  3
/** ISP_GAMMA_G_X05 : R/W; bitpos: [8:6]; default: 4;
 *  this field configures the point 5 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X05    0x00000007U
#define ISP_GAMMA_G_X05_M  (ISP_GAMMA_G_X05_V << ISP_GAMMA_G_X05_S)
#define ISP_GAMMA_G_X05_V  0x00000007U
#define ISP_GAMMA_G_X05_S  6
/** ISP_GAMMA_G_X04 : R/W; bitpos: [11:9]; default: 4;
 *  this field configures the point 4 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X04    0x00000007U
#define ISP_GAMMA_G_X04_M  (ISP_GAMMA_G_X04_V << ISP_GAMMA_G_X04_S)
#define ISP_GAMMA_G_X04_V  0x00000007U
#define ISP_GAMMA_G_X04_S  9
/** ISP_GAMMA_G_X03 : R/W; bitpos: [14:12]; default: 4;
 *  this field configures the point 3 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X03    0x00000007U
#define ISP_GAMMA_G_X03_M  (ISP_GAMMA_G_X03_V << ISP_GAMMA_G_X03_S)
#define ISP_GAMMA_G_X03_V  0x00000007U
#define ISP_GAMMA_G_X03_S  12
/** ISP_GAMMA_G_X02 : R/W; bitpos: [17:15]; default: 4;
 *  this field configures the point 2 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X02    0x00000007U
#define ISP_GAMMA_G_X02_M  (ISP_GAMMA_G_X02_V << ISP_GAMMA_G_X02_S)
#define ISP_GAMMA_G_X02_V  0x00000007U
#define ISP_GAMMA_G_X02_S  15
/** ISP_GAMMA_G_X01 : R/W; bitpos: [20:18]; default: 4;
 *  this field configures the point 1 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X01    0x00000007U
#define ISP_GAMMA_G_X01_M  (ISP_GAMMA_G_X01_V << ISP_GAMMA_G_X01_S)
#define ISP_GAMMA_G_X01_V  0x00000007U
#define ISP_GAMMA_G_X01_S  18
/** ISP_GAMMA_G_X00 : R/W; bitpos: [23:21]; default: 4;
 *  this field configures the point 0 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X00    0x00000007U
#define ISP_GAMMA_G_X00_M  (ISP_GAMMA_G_X00_V << ISP_GAMMA_G_X00_S)
#define ISP_GAMMA_G_X00_V  0x00000007U
#define ISP_GAMMA_G_X00_S  21

/** ISP_GAMMA_GX2_REG register
 *  point of X-axis of g channel gamma curve register 2
 */
#define ISP_GAMMA_GX2_REG (DR_REG_ISP_BASE + 0xb4)
/** ISP_GAMMA_G_X0F : R/W; bitpos: [2:0]; default: 4;
 *  this field configures the point 15 of X-axis of g channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X0F    0x00000007U
#define ISP_GAMMA_G_X0F_M  (ISP_GAMMA_G_X0F_V << ISP_GAMMA_G_X0F_S)
#define ISP_GAMMA_G_X0F_V  0x00000007U
#define ISP_GAMMA_G_X0F_S  0
/** ISP_GAMMA_G_X0E : R/W; bitpos: [5:3]; default: 4;
 *  this field configures the point 14 of X-axis of g channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X0E    0x00000007U
#define ISP_GAMMA_G_X0E_M  (ISP_GAMMA_G_X0E_V << ISP_GAMMA_G_X0E_S)
#define ISP_GAMMA_G_X0E_V  0x00000007U
#define ISP_GAMMA_G_X0E_S  3
/** ISP_GAMMA_G_X0D : R/W; bitpos: [8:6]; default: 4;
 *  this field configures the point 13 of X-axis of g channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X0D    0x00000007U
#define ISP_GAMMA_G_X0D_M  (ISP_GAMMA_G_X0D_V << ISP_GAMMA_G_X0D_S)
#define ISP_GAMMA_G_X0D_V  0x00000007U
#define ISP_GAMMA_G_X0D_S  6
/** ISP_GAMMA_G_X0C : R/W; bitpos: [11:9]; default: 4;
 *  this field configures the point 12 of X-axis of g channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X0C    0x00000007U
#define ISP_GAMMA_G_X0C_M  (ISP_GAMMA_G_X0C_V << ISP_GAMMA_G_X0C_S)
#define ISP_GAMMA_G_X0C_V  0x00000007U
#define ISP_GAMMA_G_X0C_S  9
/** ISP_GAMMA_G_X0B : R/W; bitpos: [14:12]; default: 4;
 *  this field configures the point 11 of X-axis of g channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X0B    0x00000007U
#define ISP_GAMMA_G_X0B_M  (ISP_GAMMA_G_X0B_V << ISP_GAMMA_G_X0B_S)
#define ISP_GAMMA_G_X0B_V  0x00000007U
#define ISP_GAMMA_G_X0B_S  12
/** ISP_GAMMA_G_X0A : R/W; bitpos: [17:15]; default: 4;
 *  this field configures the point 10 of X-axis of g channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X0A    0x00000007U
#define ISP_GAMMA_G_X0A_M  (ISP_GAMMA_G_X0A_V << ISP_GAMMA_G_X0A_S)
#define ISP_GAMMA_G_X0A_V  0x00000007U
#define ISP_GAMMA_G_X0A_S  15
/** ISP_GAMMA_G_X09 : R/W; bitpos: [20:18]; default: 4;
 *  this field configures the point 9 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X09    0x00000007U
#define ISP_GAMMA_G_X09_M  (ISP_GAMMA_G_X09_V << ISP_GAMMA_G_X09_S)
#define ISP_GAMMA_G_X09_V  0x00000007U
#define ISP_GAMMA_G_X09_S  18
/** ISP_GAMMA_G_X08 : R/W; bitpos: [23:21]; default: 4;
 *  this field configures the point 8 of X-axis of g channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_G_X08    0x00000007U
#define ISP_GAMMA_G_X08_M  (ISP_GAMMA_G_X08_V << ISP_GAMMA_G_X08_S)
#define ISP_GAMMA_G_X08_V  0x00000007U
#define ISP_GAMMA_G_X08_S  21

/** ISP_GAMMA_BX1_REG register
 *  point of X-axis of b channel gamma curve register 1
 */
#define ISP_GAMMA_BX1_REG (DR_REG_ISP_BASE + 0xb8)
/** ISP_GAMMA_B_X07 : R/W; bitpos: [2:0]; default: 4;
 *  this field configures the point 7 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X07    0x00000007U
#define ISP_GAMMA_B_X07_M  (ISP_GAMMA_B_X07_V << ISP_GAMMA_B_X07_S)
#define ISP_GAMMA_B_X07_V  0x00000007U
#define ISP_GAMMA_B_X07_S  0
/** ISP_GAMMA_B_X06 : R/W; bitpos: [5:3]; default: 4;
 *  this field configures the point 6 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X06    0x00000007U
#define ISP_GAMMA_B_X06_M  (ISP_GAMMA_B_X06_V << ISP_GAMMA_B_X06_S)
#define ISP_GAMMA_B_X06_V  0x00000007U
#define ISP_GAMMA_B_X06_S  3
/** ISP_GAMMA_B_X05 : R/W; bitpos: [8:6]; default: 4;
 *  this field configures the point 5 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X05    0x00000007U
#define ISP_GAMMA_B_X05_M  (ISP_GAMMA_B_X05_V << ISP_GAMMA_B_X05_S)
#define ISP_GAMMA_B_X05_V  0x00000007U
#define ISP_GAMMA_B_X05_S  6
/** ISP_GAMMA_B_X04 : R/W; bitpos: [11:9]; default: 4;
 *  this field configures the point 4 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X04    0x00000007U
#define ISP_GAMMA_B_X04_M  (ISP_GAMMA_B_X04_V << ISP_GAMMA_B_X04_S)
#define ISP_GAMMA_B_X04_V  0x00000007U
#define ISP_GAMMA_B_X04_S  9
/** ISP_GAMMA_B_X03 : R/W; bitpos: [14:12]; default: 4;
 *  this field configures the point 3 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X03    0x00000007U
#define ISP_GAMMA_B_X03_M  (ISP_GAMMA_B_X03_V << ISP_GAMMA_B_X03_S)
#define ISP_GAMMA_B_X03_V  0x00000007U
#define ISP_GAMMA_B_X03_S  12
/** ISP_GAMMA_B_X02 : R/W; bitpos: [17:15]; default: 4;
 *  this field configures the point 2 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X02    0x00000007U
#define ISP_GAMMA_B_X02_M  (ISP_GAMMA_B_X02_V << ISP_GAMMA_B_X02_S)
#define ISP_GAMMA_B_X02_V  0x00000007U
#define ISP_GAMMA_B_X02_S  15
/** ISP_GAMMA_B_X01 : R/W; bitpos: [20:18]; default: 4;
 *  this field configures the point 1 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X01    0x00000007U
#define ISP_GAMMA_B_X01_M  (ISP_GAMMA_B_X01_V << ISP_GAMMA_B_X01_S)
#define ISP_GAMMA_B_X01_V  0x00000007U
#define ISP_GAMMA_B_X01_S  18
/** ISP_GAMMA_B_X00 : R/W; bitpos: [23:21]; default: 4;
 *  this field configures the point 0 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X00    0x00000007U
#define ISP_GAMMA_B_X00_M  (ISP_GAMMA_B_X00_V << ISP_GAMMA_B_X00_S)
#define ISP_GAMMA_B_X00_V  0x00000007U
#define ISP_GAMMA_B_X00_S  21

/** ISP_GAMMA_BX2_REG register
 *  point of X-axis of b channel gamma curve register 2
 */
#define ISP_GAMMA_BX2_REG (DR_REG_ISP_BASE + 0xbc)
/** ISP_GAMMA_B_X0F : R/W; bitpos: [2:0]; default: 4;
 *  this field configures the point 15 of X-axis of b channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X0F    0x00000007U
#define ISP_GAMMA_B_X0F_M  (ISP_GAMMA_B_X0F_V << ISP_GAMMA_B_X0F_S)
#define ISP_GAMMA_B_X0F_V  0x00000007U
#define ISP_GAMMA_B_X0F_S  0
/** ISP_GAMMA_B_X0E : R/W; bitpos: [5:3]; default: 4;
 *  this field configures the point 14 of X-axis of b channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X0E    0x00000007U
#define ISP_GAMMA_B_X0E_M  (ISP_GAMMA_B_X0E_V << ISP_GAMMA_B_X0E_S)
#define ISP_GAMMA_B_X0E_V  0x00000007U
#define ISP_GAMMA_B_X0E_S  3
/** ISP_GAMMA_B_X0D : R/W; bitpos: [8:6]; default: 4;
 *  this field configures the point 13 of X-axis of b channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X0D    0x00000007U
#define ISP_GAMMA_B_X0D_M  (ISP_GAMMA_B_X0D_V << ISP_GAMMA_B_X0D_S)
#define ISP_GAMMA_B_X0D_V  0x00000007U
#define ISP_GAMMA_B_X0D_S  6
/** ISP_GAMMA_B_X0C : R/W; bitpos: [11:9]; default: 4;
 *  this field configures the point 12 of X-axis of b channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X0C    0x00000007U
#define ISP_GAMMA_B_X0C_M  (ISP_GAMMA_B_X0C_V << ISP_GAMMA_B_X0C_S)
#define ISP_GAMMA_B_X0C_V  0x00000007U
#define ISP_GAMMA_B_X0C_S  9
/** ISP_GAMMA_B_X0B : R/W; bitpos: [14:12]; default: 4;
 *  this field configures the point 11 of X-axis of b channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X0B    0x00000007U
#define ISP_GAMMA_B_X0B_M  (ISP_GAMMA_B_X0B_V << ISP_GAMMA_B_X0B_S)
#define ISP_GAMMA_B_X0B_V  0x00000007U
#define ISP_GAMMA_B_X0B_S  12
/** ISP_GAMMA_B_X0A : R/W; bitpos: [17:15]; default: 4;
 *  this field configures the point 10 of X-axis of b channel gamma curve, it
 *  represents the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X0A    0x00000007U
#define ISP_GAMMA_B_X0A_M  (ISP_GAMMA_B_X0A_V << ISP_GAMMA_B_X0A_S)
#define ISP_GAMMA_B_X0A_V  0x00000007U
#define ISP_GAMMA_B_X0A_S  15
/** ISP_GAMMA_B_X09 : R/W; bitpos: [20:18]; default: 4;
 *  this field configures the point 9 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X09    0x00000007U
#define ISP_GAMMA_B_X09_M  (ISP_GAMMA_B_X09_V << ISP_GAMMA_B_X09_S)
#define ISP_GAMMA_B_X09_V  0x00000007U
#define ISP_GAMMA_B_X09_S  18
/** ISP_GAMMA_B_X08 : R/W; bitpos: [23:21]; default: 4;
 *  this field configures the point 8 of X-axis of b channel gamma curve, it represents
 *  the power of the distance from the previous point
 */
#define ISP_GAMMA_B_X08    0x00000007U
#define ISP_GAMMA_B_X08_M  (ISP_GAMMA_B_X08_V << ISP_GAMMA_B_X08_S)
#define ISP_GAMMA_B_X08_V  0x00000007U
#define ISP_GAMMA_B_X08_S  21

/** ISP_AE_CTRL_REG register
 *  ae control register
 */
#define ISP_AE_CTRL_REG (DR_REG_ISP_BASE + 0xc0)
/** ISP_AE_UPDATE : WT; bitpos: [0]; default: 0;
 *  write 1 to this bit triggers one statistic event
 */
#define ISP_AE_UPDATE    (BIT(0))
#define ISP_AE_UPDATE_M  (ISP_AE_UPDATE_V << ISP_AE_UPDATE_S)
#define ISP_AE_UPDATE_V  0x00000001U
#define ISP_AE_UPDATE_S  0
/** ISP_AE_SELECT : R/W; bitpos: [1]; default: 0;
 *  this field configures ae input data source, 0: data from median, 1: data from gama
 */
#define ISP_AE_SELECT    (BIT(1))
#define ISP_AE_SELECT_M  (ISP_AE_SELECT_V << ISP_AE_SELECT_S)
#define ISP_AE_SELECT_V  0x00000001U
#define ISP_AE_SELECT_S  1

/** ISP_AE_MONITOR_REG register
 *  ae monitor control register
 */
#define ISP_AE_MONITOR_REG (DR_REG_ISP_BASE + 0xc4)
/** ISP_AE_MONITOR_TL : R/W; bitpos: [7:0]; default: 0;
 *  this field configures the lower lum threshold of ae monitor
 */
#define ISP_AE_MONITOR_TL    0x000000FFU
#define ISP_AE_MONITOR_TL_M  (ISP_AE_MONITOR_TL_V << ISP_AE_MONITOR_TL_S)
#define ISP_AE_MONITOR_TL_V  0x000000FFU
#define ISP_AE_MONITOR_TL_S  0
/** ISP_AE_MONITOR_TH : R/W; bitpos: [15:8]; default: 0;
 *  this field configures the higher lum threshold of ae monitor
 */
#define ISP_AE_MONITOR_TH    0x000000FFU
#define ISP_AE_MONITOR_TH_M  (ISP_AE_MONITOR_TH_V << ISP_AE_MONITOR_TH_S)
#define ISP_AE_MONITOR_TH_V  0x000000FFU
#define ISP_AE_MONITOR_TH_S  8
/** ISP_AE_MONITOR_PERIOD : R/W; bitpos: [21:16]; default: 0;
 *  this field configures ae monitor frame period
 */
#define ISP_AE_MONITOR_PERIOD    0x0000003FU
#define ISP_AE_MONITOR_PERIOD_M  (ISP_AE_MONITOR_PERIOD_V << ISP_AE_MONITOR_PERIOD_S)
#define ISP_AE_MONITOR_PERIOD_V  0x0000003FU
#define ISP_AE_MONITOR_PERIOD_S  16

/** ISP_AE_BX_REG register
 *  ae window register in x-direction
 */
#define ISP_AE_BX_REG (DR_REG_ISP_BASE + 0xc8)
/** ISP_AE_X_BSIZE : R/W; bitpos: [10:0]; default: 384;
 *  this field configures every block x size
 */
#define ISP_AE_X_BSIZE    0x000007FFU
#define ISP_AE_X_BSIZE_M  (ISP_AE_X_BSIZE_V << ISP_AE_X_BSIZE_S)
#define ISP_AE_X_BSIZE_V  0x000007FFU
#define ISP_AE_X_BSIZE_S  0
/** ISP_AE_X_START : R/W; bitpos: [21:11]; default: 0;
 *  this field configures first block start x address
 */
#define ISP_AE_X_START    0x000007FFU
#define ISP_AE_X_START_M  (ISP_AE_X_START_V << ISP_AE_X_START_S)
#define ISP_AE_X_START_V  0x000007FFU
#define ISP_AE_X_START_S  11

/** ISP_AE_BY_REG register
 *  ae window register in y-direction
 */
#define ISP_AE_BY_REG (DR_REG_ISP_BASE + 0xcc)
/** ISP_AE_Y_BSIZE : R/W; bitpos: [10:0]; default: 216;
 *  this field configures every block y size
 */
#define ISP_AE_Y_BSIZE    0x000007FFU
#define ISP_AE_Y_BSIZE_M  (ISP_AE_Y_BSIZE_V << ISP_AE_Y_BSIZE_S)
#define ISP_AE_Y_BSIZE_V  0x000007FFU
#define ISP_AE_Y_BSIZE_S  0
/** ISP_AE_Y_START : R/W; bitpos: [21:11]; default: 0;
 *  this field configures first block start y address
 */
#define ISP_AE_Y_START    0x000007FFU
#define ISP_AE_Y_START_M  (ISP_AE_Y_START_V << ISP_AE_Y_START_S)
#define ISP_AE_Y_START_V  0x000007FFU
#define ISP_AE_Y_START_S  11

/** ISP_AE_WINPIXNUM_REG register
 *  ae sub-window pix num register
 */
#define ISP_AE_WINPIXNUM_REG (DR_REG_ISP_BASE + 0xd0)
/** ISP_AE_SUBWIN_PIXNUM : R/W; bitpos: [16:0]; default: 82944;
 *  this field configures the pixel number of each sub win
 */
#define ISP_AE_SUBWIN_PIXNUM    0x0001FFFFU
#define ISP_AE_SUBWIN_PIXNUM_M  (ISP_AE_SUBWIN_PIXNUM_V << ISP_AE_SUBWIN_PIXNUM_S)
#define ISP_AE_SUBWIN_PIXNUM_V  0x0001FFFFU
#define ISP_AE_SUBWIN_PIXNUM_S  0

/** ISP_AE_WIN_RECIPROCAL_REG register
 *  reciprocal of ae sub-window pixel number
 */
#define ISP_AE_WIN_RECIPROCAL_REG (DR_REG_ISP_BASE + 0xd4)
/** ISP_AE_SUBWIN_RECIP : R/W; bitpos: [19:0]; default: 0;
 *  this field configures the reciprocal of each subwin_pixnum, 20bit fraction
 */
#define ISP_AE_SUBWIN_RECIP    0x000FFFFFU
#define ISP_AE_SUBWIN_RECIP_M  (ISP_AE_SUBWIN_RECIP_V << ISP_AE_SUBWIN_RECIP_S)
#define ISP_AE_SUBWIN_RECIP_V  0x000FFFFFU
#define ISP_AE_SUBWIN_RECIP_S  0

/** ISP_AE_BLOCK_MEAN_0_REG register
 *  ae statistic result register 0
 */
#define ISP_AE_BLOCK_MEAN_0_REG (DR_REG_ISP_BASE + 0xd8)
/** ISP_AE_B03_MEAN : RO; bitpos: [7:0]; default: 0;
 *  this field configures block03 Y mean data
 */
#define ISP_AE_B03_MEAN    0x000000FFU
#define ISP_AE_B03_MEAN_M  (ISP_AE_B03_MEAN_V << ISP_AE_B03_MEAN_S)
#define ISP_AE_B03_MEAN_V  0x000000FFU
#define ISP_AE_B03_MEAN_S  0
/** ISP_AE_B02_MEAN : RO; bitpos: [15:8]; default: 0;
 *  this field configures block02 Y mean data
 */
#define ISP_AE_B02_MEAN    0x000000FFU
#define ISP_AE_B02_MEAN_M  (ISP_AE_B02_MEAN_V << ISP_AE_B02_MEAN_S)
#define ISP_AE_B02_MEAN_V  0x000000FFU
#define ISP_AE_B02_MEAN_S  8
/** ISP_AE_B01_MEAN : RO; bitpos: [23:16]; default: 0;
 *  this field configures block01 Y mean data
 */
#define ISP_AE_B01_MEAN    0x000000FFU
#define ISP_AE_B01_MEAN_M  (ISP_AE_B01_MEAN_V << ISP_AE_B01_MEAN_S)
#define ISP_AE_B01_MEAN_V  0x000000FFU
#define ISP_AE_B01_MEAN_S  16
/** ISP_AE_B00_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field configures block00 Y mean data
 */
#define ISP_AE_B00_MEAN    0x000000FFU
#define ISP_AE_B00_MEAN_M  (ISP_AE_B00_MEAN_V << ISP_AE_B00_MEAN_S)
#define ISP_AE_B00_MEAN_V  0x000000FFU
#define ISP_AE_B00_MEAN_S  24

/** ISP_AE_BLOCK_MEAN_1_REG register
 *  ae statistic result register 1
 */
#define ISP_AE_BLOCK_MEAN_1_REG (DR_REG_ISP_BASE + 0xdc)
/** ISP_AE_B12_MEAN : RO; bitpos: [7:0]; default: 0;
 *  this field configures block12 Y mean data
 */
#define ISP_AE_B12_MEAN    0x000000FFU
#define ISP_AE_B12_MEAN_M  (ISP_AE_B12_MEAN_V << ISP_AE_B12_MEAN_S)
#define ISP_AE_B12_MEAN_V  0x000000FFU
#define ISP_AE_B12_MEAN_S  0
/** ISP_AE_B11_MEAN : RO; bitpos: [15:8]; default: 0;
 *  this field configures block11 Y mean data
 */
#define ISP_AE_B11_MEAN    0x000000FFU
#define ISP_AE_B11_MEAN_M  (ISP_AE_B11_MEAN_V << ISP_AE_B11_MEAN_S)
#define ISP_AE_B11_MEAN_V  0x000000FFU
#define ISP_AE_B11_MEAN_S  8
/** ISP_AE_B10_MEAN : RO; bitpos: [23:16]; default: 0;
 *  this field configures block10 Y mean data
 */
#define ISP_AE_B10_MEAN    0x000000FFU
#define ISP_AE_B10_MEAN_M  (ISP_AE_B10_MEAN_V << ISP_AE_B10_MEAN_S)
#define ISP_AE_B10_MEAN_V  0x000000FFU
#define ISP_AE_B10_MEAN_S  16
/** ISP_AE_B04_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field configures block04 Y mean data
 */
#define ISP_AE_B04_MEAN    0x000000FFU
#define ISP_AE_B04_MEAN_M  (ISP_AE_B04_MEAN_V << ISP_AE_B04_MEAN_S)
#define ISP_AE_B04_MEAN_V  0x000000FFU
#define ISP_AE_B04_MEAN_S  24

/** ISP_AE_BLOCK_MEAN_2_REG register
 *  ae statistic result register 2
 */
#define ISP_AE_BLOCK_MEAN_2_REG (DR_REG_ISP_BASE + 0xe0)
/** ISP_AE_B21_MEAN : RO; bitpos: [7:0]; default: 0;
 *  this field configures block21 Y mean data
 */
#define ISP_AE_B21_MEAN    0x000000FFU
#define ISP_AE_B21_MEAN_M  (ISP_AE_B21_MEAN_V << ISP_AE_B21_MEAN_S)
#define ISP_AE_B21_MEAN_V  0x000000FFU
#define ISP_AE_B21_MEAN_S  0
/** ISP_AE_B20_MEAN : RO; bitpos: [15:8]; default: 0;
 *  this field configures block20 Y mean data
 */
#define ISP_AE_B20_MEAN    0x000000FFU
#define ISP_AE_B20_MEAN_M  (ISP_AE_B20_MEAN_V << ISP_AE_B20_MEAN_S)
#define ISP_AE_B20_MEAN_V  0x000000FFU
#define ISP_AE_B20_MEAN_S  8
/** ISP_AE_B14_MEAN : RO; bitpos: [23:16]; default: 0;
 *  this field configures block14 Y mean data
 */
#define ISP_AE_B14_MEAN    0x000000FFU
#define ISP_AE_B14_MEAN_M  (ISP_AE_B14_MEAN_V << ISP_AE_B14_MEAN_S)
#define ISP_AE_B14_MEAN_V  0x000000FFU
#define ISP_AE_B14_MEAN_S  16
/** ISP_AE_B13_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field configures block13 Y mean data
 */
#define ISP_AE_B13_MEAN    0x000000FFU
#define ISP_AE_B13_MEAN_M  (ISP_AE_B13_MEAN_V << ISP_AE_B13_MEAN_S)
#define ISP_AE_B13_MEAN_V  0x000000FFU
#define ISP_AE_B13_MEAN_S  24

/** ISP_AE_BLOCK_MEAN_3_REG register
 *  ae statistic result register 3
 */
#define ISP_AE_BLOCK_MEAN_3_REG (DR_REG_ISP_BASE + 0xe4)
/** ISP_AE_B30_MEAN : RO; bitpos: [7:0]; default: 0;
 *  this field configures block30 Y mean data
 */
#define ISP_AE_B30_MEAN    0x000000FFU
#define ISP_AE_B30_MEAN_M  (ISP_AE_B30_MEAN_V << ISP_AE_B30_MEAN_S)
#define ISP_AE_B30_MEAN_V  0x000000FFU
#define ISP_AE_B30_MEAN_S  0
/** ISP_AE_B24_MEAN : RO; bitpos: [15:8]; default: 0;
 *  this field configures block24 Y mean data
 */
#define ISP_AE_B24_MEAN    0x000000FFU
#define ISP_AE_B24_MEAN_M  (ISP_AE_B24_MEAN_V << ISP_AE_B24_MEAN_S)
#define ISP_AE_B24_MEAN_V  0x000000FFU
#define ISP_AE_B24_MEAN_S  8
/** ISP_AE_B23_MEAN : RO; bitpos: [23:16]; default: 0;
 *  this field configures block23 Y mean data
 */
#define ISP_AE_B23_MEAN    0x000000FFU
#define ISP_AE_B23_MEAN_M  (ISP_AE_B23_MEAN_V << ISP_AE_B23_MEAN_S)
#define ISP_AE_B23_MEAN_V  0x000000FFU
#define ISP_AE_B23_MEAN_S  16
/** ISP_AE_B22_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field configures block22 Y mean data
 */
#define ISP_AE_B22_MEAN    0x000000FFU
#define ISP_AE_B22_MEAN_M  (ISP_AE_B22_MEAN_V << ISP_AE_B22_MEAN_S)
#define ISP_AE_B22_MEAN_V  0x000000FFU
#define ISP_AE_B22_MEAN_S  24

/** ISP_AE_BLOCK_MEAN_4_REG register
 *  ae statistic result register 4
 */
#define ISP_AE_BLOCK_MEAN_4_REG (DR_REG_ISP_BASE + 0xe8)
/** ISP_AE_B34_MEAN : RO; bitpos: [7:0]; default: 0;
 *  this field configures block34 Y mean data
 */
#define ISP_AE_B34_MEAN    0x000000FFU
#define ISP_AE_B34_MEAN_M  (ISP_AE_B34_MEAN_V << ISP_AE_B34_MEAN_S)
#define ISP_AE_B34_MEAN_V  0x000000FFU
#define ISP_AE_B34_MEAN_S  0
/** ISP_AE_B33_MEAN : RO; bitpos: [15:8]; default: 0;
 *  this field configures block33 Y mean data
 */
#define ISP_AE_B33_MEAN    0x000000FFU
#define ISP_AE_B33_MEAN_M  (ISP_AE_B33_MEAN_V << ISP_AE_B33_MEAN_S)
#define ISP_AE_B33_MEAN_V  0x000000FFU
#define ISP_AE_B33_MEAN_S  8
/** ISP_AE_B32_MEAN : RO; bitpos: [23:16]; default: 0;
 *  this field configures block32 Y mean data
 */
#define ISP_AE_B32_MEAN    0x000000FFU
#define ISP_AE_B32_MEAN_M  (ISP_AE_B32_MEAN_V << ISP_AE_B32_MEAN_S)
#define ISP_AE_B32_MEAN_V  0x000000FFU
#define ISP_AE_B32_MEAN_S  16
/** ISP_AE_B31_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field configures block31 Y mean data
 */
#define ISP_AE_B31_MEAN    0x000000FFU
#define ISP_AE_B31_MEAN_M  (ISP_AE_B31_MEAN_V << ISP_AE_B31_MEAN_S)
#define ISP_AE_B31_MEAN_V  0x000000FFU
#define ISP_AE_B31_MEAN_S  24

/** ISP_AE_BLOCK_MEAN_5_REG register
 *  ae statistic result register 5
 */
#define ISP_AE_BLOCK_MEAN_5_REG (DR_REG_ISP_BASE + 0xec)
/** ISP_AE_B43_MEAN : RO; bitpos: [7:0]; default: 0;
 *  this field configures block43 Y mean data
 */
#define ISP_AE_B43_MEAN    0x000000FFU
#define ISP_AE_B43_MEAN_M  (ISP_AE_B43_MEAN_V << ISP_AE_B43_MEAN_S)
#define ISP_AE_B43_MEAN_V  0x000000FFU
#define ISP_AE_B43_MEAN_S  0
/** ISP_AE_B42_MEAN : RO; bitpos: [15:8]; default: 0;
 *  this field configures block42 Y mean data
 */
#define ISP_AE_B42_MEAN    0x000000FFU
#define ISP_AE_B42_MEAN_M  (ISP_AE_B42_MEAN_V << ISP_AE_B42_MEAN_S)
#define ISP_AE_B42_MEAN_V  0x000000FFU
#define ISP_AE_B42_MEAN_S  8
/** ISP_AE_B41_MEAN : RO; bitpos: [23:16]; default: 0;
 *  this field configures block41 Y mean data
 */
#define ISP_AE_B41_MEAN    0x000000FFU
#define ISP_AE_B41_MEAN_M  (ISP_AE_B41_MEAN_V << ISP_AE_B41_MEAN_S)
#define ISP_AE_B41_MEAN_V  0x000000FFU
#define ISP_AE_B41_MEAN_S  16
/** ISP_AE_B40_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field configures block40 Y mean data
 */
#define ISP_AE_B40_MEAN    0x000000FFU
#define ISP_AE_B40_MEAN_M  (ISP_AE_B40_MEAN_V << ISP_AE_B40_MEAN_S)
#define ISP_AE_B40_MEAN_V  0x000000FFU
#define ISP_AE_B40_MEAN_S  24

/** ISP_AE_BLOCK_MEAN_6_REG register
 *  ae statistic result register 6
 */
#define ISP_AE_BLOCK_MEAN_6_REG (DR_REG_ISP_BASE + 0xf0)
/** ISP_AE_B44_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field configures block44 Y mean data
 */
#define ISP_AE_B44_MEAN    0x000000FFU
#define ISP_AE_B44_MEAN_M  (ISP_AE_B44_MEAN_V << ISP_AE_B44_MEAN_S)
#define ISP_AE_B44_MEAN_V  0x000000FFU
#define ISP_AE_B44_MEAN_S  24

/** ISP_SHARP_CTRL0_REG register
 *  sharp control register 0
 */
#define ISP_SHARP_CTRL0_REG (DR_REG_ISP_BASE + 0xf4)
/** ISP_SHARP_THRESHOLD_LOW : R/W; bitpos: [7:0]; default: 0;
 *  this field configures sharpen threshold for detail
 */
#define ISP_SHARP_THRESHOLD_LOW    0x000000FFU
#define ISP_SHARP_THRESHOLD_LOW_M  (ISP_SHARP_THRESHOLD_LOW_V << ISP_SHARP_THRESHOLD_LOW_S)
#define ISP_SHARP_THRESHOLD_LOW_V  0x000000FFU
#define ISP_SHARP_THRESHOLD_LOW_S  0
/** ISP_SHARP_THRESHOLD_HIGH : R/W; bitpos: [15:8]; default: 0;
 *  this field configures sharpen threshold for edge
 */
#define ISP_SHARP_THRESHOLD_HIGH    0x000000FFU
#define ISP_SHARP_THRESHOLD_HIGH_M  (ISP_SHARP_THRESHOLD_HIGH_V << ISP_SHARP_THRESHOLD_HIGH_S)
#define ISP_SHARP_THRESHOLD_HIGH_V  0x000000FFU
#define ISP_SHARP_THRESHOLD_HIGH_S  8
/** ISP_SHARP_AMOUNT_LOW : R/W; bitpos: [23:16]; default: 0;
 *  this field configures sharpen amount for detail
 */
#define ISP_SHARP_AMOUNT_LOW    0x000000FFU
#define ISP_SHARP_AMOUNT_LOW_M  (ISP_SHARP_AMOUNT_LOW_V << ISP_SHARP_AMOUNT_LOW_S)
#define ISP_SHARP_AMOUNT_LOW_V  0x000000FFU
#define ISP_SHARP_AMOUNT_LOW_S  16
/** ISP_SHARP_AMOUNT_HIGH : R/W; bitpos: [31:24]; default: 0;
 *  this field configures sharpen amount for edge
 */
#define ISP_SHARP_AMOUNT_HIGH    0x000000FFU
#define ISP_SHARP_AMOUNT_HIGH_M  (ISP_SHARP_AMOUNT_HIGH_V << ISP_SHARP_AMOUNT_HIGH_S)
#define ISP_SHARP_AMOUNT_HIGH_V  0x000000FFU
#define ISP_SHARP_AMOUNT_HIGH_S  24

/** ISP_SHARP_FILTER0_REG register
 *  sharp usm config register 0
 */
#define ISP_SHARP_FILTER0_REG (DR_REG_ISP_BASE + 0xf8)
/** ISP_SHARP_FILTER_COE00 : R/W; bitpos: [4:0]; default: 1;
 *  this field configures unsharp masking(usm) filter coefficient
 */
#define ISP_SHARP_FILTER_COE00    0x0000001FU
#define ISP_SHARP_FILTER_COE00_M  (ISP_SHARP_FILTER_COE00_V << ISP_SHARP_FILTER_COE00_S)
#define ISP_SHARP_FILTER_COE00_V  0x0000001FU
#define ISP_SHARP_FILTER_COE00_S  0
/** ISP_SHARP_FILTER_COE01 : R/W; bitpos: [9:5]; default: 2;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE01    0x0000001FU
#define ISP_SHARP_FILTER_COE01_M  (ISP_SHARP_FILTER_COE01_V << ISP_SHARP_FILTER_COE01_S)
#define ISP_SHARP_FILTER_COE01_V  0x0000001FU
#define ISP_SHARP_FILTER_COE01_S  5
/** ISP_SHARP_FILTER_COE02 : R/W; bitpos: [14:10]; default: 1;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE02    0x0000001FU
#define ISP_SHARP_FILTER_COE02_M  (ISP_SHARP_FILTER_COE02_V << ISP_SHARP_FILTER_COE02_S)
#define ISP_SHARP_FILTER_COE02_V  0x0000001FU
#define ISP_SHARP_FILTER_COE02_S  10

/** ISP_SHARP_FILTER1_REG register
 *  sharp usm config register 1
 */
#define ISP_SHARP_FILTER1_REG (DR_REG_ISP_BASE + 0xfc)
/** ISP_SHARP_FILTER_COE10 : R/W; bitpos: [4:0]; default: 2;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE10    0x0000001FU
#define ISP_SHARP_FILTER_COE10_M  (ISP_SHARP_FILTER_COE10_V << ISP_SHARP_FILTER_COE10_S)
#define ISP_SHARP_FILTER_COE10_V  0x0000001FU
#define ISP_SHARP_FILTER_COE10_S  0
/** ISP_SHARP_FILTER_COE11 : R/W; bitpos: [9:5]; default: 4;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE11    0x0000001FU
#define ISP_SHARP_FILTER_COE11_M  (ISP_SHARP_FILTER_COE11_V << ISP_SHARP_FILTER_COE11_S)
#define ISP_SHARP_FILTER_COE11_V  0x0000001FU
#define ISP_SHARP_FILTER_COE11_S  5
/** ISP_SHARP_FILTER_COE12 : R/W; bitpos: [14:10]; default: 2;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE12    0x0000001FU
#define ISP_SHARP_FILTER_COE12_M  (ISP_SHARP_FILTER_COE12_V << ISP_SHARP_FILTER_COE12_S)
#define ISP_SHARP_FILTER_COE12_V  0x0000001FU
#define ISP_SHARP_FILTER_COE12_S  10

/** ISP_SHARP_FILTER2_REG register
 *  sharp usm config register 2
 */
#define ISP_SHARP_FILTER2_REG (DR_REG_ISP_BASE + 0x100)
/** ISP_SHARP_FILTER_COE20 : R/W; bitpos: [4:0]; default: 1;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE20    0x0000001FU
#define ISP_SHARP_FILTER_COE20_M  (ISP_SHARP_FILTER_COE20_V << ISP_SHARP_FILTER_COE20_S)
#define ISP_SHARP_FILTER_COE20_V  0x0000001FU
#define ISP_SHARP_FILTER_COE20_S  0
/** ISP_SHARP_FILTER_COE21 : R/W; bitpos: [9:5]; default: 2;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE21    0x0000001FU
#define ISP_SHARP_FILTER_COE21_M  (ISP_SHARP_FILTER_COE21_V << ISP_SHARP_FILTER_COE21_S)
#define ISP_SHARP_FILTER_COE21_V  0x0000001FU
#define ISP_SHARP_FILTER_COE21_S  5
/** ISP_SHARP_FILTER_COE22 : R/W; bitpos: [14:10]; default: 1;
 *  this field configures usm filter coefficient
 */
#define ISP_SHARP_FILTER_COE22    0x0000001FU
#define ISP_SHARP_FILTER_COE22_M  (ISP_SHARP_FILTER_COE22_V << ISP_SHARP_FILTER_COE22_S)
#define ISP_SHARP_FILTER_COE22_V  0x0000001FU
#define ISP_SHARP_FILTER_COE22_S  10

/** ISP_SHARP_MATRIX_CTRL_REG register
 *  sharp pix2matrix ctrl
 */
#define ISP_SHARP_MATRIX_CTRL_REG (DR_REG_ISP_BASE + 0x104)
/** ISP_SHARP_TAIL_PIXEN_PULSE_TL : R/W; bitpos: [7:0]; default: 0;
 *  matrix tail pixen low level threshold,  should not to large to prevent expanding to
 *  next frame, only reg_demosaic_tail_pixen_pulse_th!=0 and
 *  reg_demosaic_tail_pixen_pulse_tl!=0 and reg_demosaic_tail_pixen_pulse_th <
 *  reg_demosaic_tail_pixen_pulse_tl will enable tail pulse function
 */
#define ISP_SHARP_TAIL_PIXEN_PULSE_TL    0x000000FFU
#define ISP_SHARP_TAIL_PIXEN_PULSE_TL_M  (ISP_SHARP_TAIL_PIXEN_PULSE_TL_V << ISP_SHARP_TAIL_PIXEN_PULSE_TL_S)
#define ISP_SHARP_TAIL_PIXEN_PULSE_TL_V  0x000000FFU
#define ISP_SHARP_TAIL_PIXEN_PULSE_TL_S  0
/** ISP_SHARP_TAIL_PIXEN_PULSE_TH : R/W; bitpos: [15:8]; default: 0;
 *  matrix tail pixen high level threshold, must < hnum-1, only
 *  reg_sharp_tail_pixen_pulse_th!=0 and reg_sharp_tail_pixen_pulse_tl!=0 and
 *  reg_sharp_tail_pixen_pulse_th < reg_sharp_tail_pixen_pulse_tl will enable tail
 *  pulse function
 */
#define ISP_SHARP_TAIL_PIXEN_PULSE_TH    0x000000FFU
#define ISP_SHARP_TAIL_PIXEN_PULSE_TH_M  (ISP_SHARP_TAIL_PIXEN_PULSE_TH_V << ISP_SHARP_TAIL_PIXEN_PULSE_TH_S)
#define ISP_SHARP_TAIL_PIXEN_PULSE_TH_V  0x000000FFU
#define ISP_SHARP_TAIL_PIXEN_PULSE_TH_S  8
/** ISP_SHARP_PADDING_DATA : R/W; bitpos: [23:16]; default: 0;
 *  this field configures sharp padding data
 */
#define ISP_SHARP_PADDING_DATA    0x000000FFU
#define ISP_SHARP_PADDING_DATA_M  (ISP_SHARP_PADDING_DATA_V << ISP_SHARP_PADDING_DATA_S)
#define ISP_SHARP_PADDING_DATA_V  0x000000FFU
#define ISP_SHARP_PADDING_DATA_S  16
/** ISP_SHARP_PADDING_MODE : R/W; bitpos: [24]; default: 0;
 *  this field configures sharp padding mode
 */
#define ISP_SHARP_PADDING_MODE    (BIT(24))
#define ISP_SHARP_PADDING_MODE_M  (ISP_SHARP_PADDING_MODE_V << ISP_SHARP_PADDING_MODE_S)
#define ISP_SHARP_PADDING_MODE_V  0x00000001U
#define ISP_SHARP_PADDING_MODE_S  24

/** ISP_SHARP_CTRL1_REG register
 *  sharp control register 1
 */
#define ISP_SHARP_CTRL1_REG (DR_REG_ISP_BASE + 0x108)
/** ISP_SHARP_GRADIENT_MAX : RO; bitpos: [7:0]; default: 0;
 *  this field configures sharp max gradient, refresh at the end of each frame end
 */
#define ISP_SHARP_GRADIENT_MAX    0x000000FFU
#define ISP_SHARP_GRADIENT_MAX_M  (ISP_SHARP_GRADIENT_MAX_V << ISP_SHARP_GRADIENT_MAX_S)
#define ISP_SHARP_GRADIENT_MAX_V  0x000000FFU
#define ISP_SHARP_GRADIENT_MAX_S  0

/** ISP_DMA_CNTL_REG register
 *  isp dma source trans control register
 */
#define ISP_DMA_CNTL_REG (DR_REG_ISP_BASE + 0x10c)
/** ISP_DMA_EN : WT; bitpos: [0]; default: 0;
 *  write 1 to trigger dma to get 1 frame
 */
#define ISP_DMA_EN    (BIT(0))
#define ISP_DMA_EN_M  (ISP_DMA_EN_V << ISP_DMA_EN_S)
#define ISP_DMA_EN_V  0x00000001U
#define ISP_DMA_EN_S  0
/** ISP_DMA_UPDATE_REG : R/W; bitpos: [1]; default: 0;
 *  write 1 to update reg_dma_burst_len & reg_dma_data_type
 */
#define ISP_DMA_UPDATE_REG    (BIT(1))
#define ISP_DMA_UPDATE_REG_M  (ISP_DMA_UPDATE_REG_V << ISP_DMA_UPDATE_REG_S)
#define ISP_DMA_UPDATE_REG_V  0x00000001U
#define ISP_DMA_UPDATE_REG_S  1
/** ISP_DMA_DATA_TYPE : R/W; bitpos: [7:2]; default: 42;
 *  this field configures the idi data type for image data
 */
#define ISP_DMA_DATA_TYPE    0x0000003FU
#define ISP_DMA_DATA_TYPE_M  (ISP_DMA_DATA_TYPE_V << ISP_DMA_DATA_TYPE_S)
#define ISP_DMA_DATA_TYPE_V  0x0000003FU
#define ISP_DMA_DATA_TYPE_S  2
/** ISP_DMA_BURST_LEN : R/W; bitpos: [19:8]; default: 128;
 *  this field configures dma burst len when data source is dma. set according to
 *  dma_msize, it is the number of 64bits in a dma transfer
 */
#define ISP_DMA_BURST_LEN    0x00000FFFU
#define ISP_DMA_BURST_LEN_M  (ISP_DMA_BURST_LEN_V << ISP_DMA_BURST_LEN_S)
#define ISP_DMA_BURST_LEN_V  0x00000FFFU
#define ISP_DMA_BURST_LEN_S  8
/** ISP_DMA_INTERVAL : R/W; bitpos: [31:20]; default: 1;
 *  this field configures dma req interval, 12'b1: 1 cycle, 12'b11 2 cycle ...
 */
#define ISP_DMA_INTERVAL    0x00000FFFU
#define ISP_DMA_INTERVAL_M  (ISP_DMA_INTERVAL_V << ISP_DMA_INTERVAL_S)
#define ISP_DMA_INTERVAL_V  0x00000FFFU
#define ISP_DMA_INTERVAL_S  20

/** ISP_DMA_RAW_DATA_REG register
 *  isp dma source total raw number set register
 */
#define ISP_DMA_RAW_DATA_REG (DR_REG_ISP_BASE + 0x110)
/** ISP_DMA_RAW_NUM_TOTAL : R/W; bitpos: [21:0]; default: 0;
 *  this field configures the the number of 64bits in a frame
 */
#define ISP_DMA_RAW_NUM_TOTAL    0x003FFFFFU
#define ISP_DMA_RAW_NUM_TOTAL_M  (ISP_DMA_RAW_NUM_TOTAL_V << ISP_DMA_RAW_NUM_TOTAL_S)
#define ISP_DMA_RAW_NUM_TOTAL_V  0x003FFFFFU
#define ISP_DMA_RAW_NUM_TOTAL_S  0
/** ISP_DMA_RAW_NUM_TOTAL_SET : WT; bitpos: [31]; default: 0;
 *  write 1 to update reg_dma_raw_num_total
 */
#define ISP_DMA_RAW_NUM_TOTAL_SET    (BIT(31))
#define ISP_DMA_RAW_NUM_TOTAL_SET_M  (ISP_DMA_RAW_NUM_TOTAL_SET_V << ISP_DMA_RAW_NUM_TOTAL_SET_S)
#define ISP_DMA_RAW_NUM_TOTAL_SET_V  0x00000001U
#define ISP_DMA_RAW_NUM_TOTAL_SET_S  31

/** ISP_CAM_CNTL_REG register
 *  isp cam source control register
 */
#define ISP_CAM_CNTL_REG (DR_REG_ISP_BASE + 0x114)
/** ISP_CAM_EN : R/W; bitpos: [0]; default: 0;
 *  write 1 to start receive camera data, write 0 to disable
 */
#define ISP_CAM_EN    (BIT(0))
#define ISP_CAM_EN_M  (ISP_CAM_EN_V << ISP_CAM_EN_S)
#define ISP_CAM_EN_V  0x00000001U
#define ISP_CAM_EN_S  0
/** ISP_CAM_UPDATE_REG : R/W; bitpos: [1]; default: 0;
 *  write 1 to update ISP_CAM_CONF
 */
#define ISP_CAM_UPDATE_REG    (BIT(1))
#define ISP_CAM_UPDATE_REG_M  (ISP_CAM_UPDATE_REG_V << ISP_CAM_UPDATE_REG_S)
#define ISP_CAM_UPDATE_REG_V  0x00000001U
#define ISP_CAM_UPDATE_REG_S  1
/** ISP_CAM_RESET : R/W; bitpos: [2]; default: 1;
 *  this bit configures cam clk domain reset, 1: reset cam input logic, 0: release reset
 */
#define ISP_CAM_RESET    (BIT(2))
#define ISP_CAM_RESET_M  (ISP_CAM_RESET_V << ISP_CAM_RESET_S)
#define ISP_CAM_RESET_V  0x00000001U
#define ISP_CAM_RESET_S  2
/** ISP_CAM_CLK_INV : R/W; bitpos: [3]; default: 0;
 *  this bit configures the inversion of cam clk from pad. 0: not invert cam clk, 1:
 *  invert cam clk
 */
#define ISP_CAM_CLK_INV    (BIT(3))
#define ISP_CAM_CLK_INV_M  (ISP_CAM_CLK_INV_V << ISP_CAM_CLK_INV_S)
#define ISP_CAM_CLK_INV_V  0x00000001U
#define ISP_CAM_CLK_INV_S  3

/** ISP_CAM_CONF_REG register
 *  isp cam source config register
 */
#define ISP_CAM_CONF_REG (DR_REG_ISP_BASE + 0x118)
/** ISP_CAM_DATA_ORDER : R/W; bitpos: [0]; default: 0;
 *  this field configures data order of cam port, 0: cam_data_in, 1:{cam_data_in[7:0],
 *  cam_data_in[15:8]}
 */
#define ISP_CAM_DATA_ORDER    (BIT(0))
#define ISP_CAM_DATA_ORDER_M  (ISP_CAM_DATA_ORDER_V << ISP_CAM_DATA_ORDER_S)
#define ISP_CAM_DATA_ORDER_V  0x00000001U
#define ISP_CAM_DATA_ORDER_S  0
/** ISP_CAM_2BYTE_MODE : R/W; bitpos: [1]; default: 0;
 *  this field configures enable of cam 2 byte mode(input 2 bytes each clock). 0:
 *  disable, 1: enable
 */
#define ISP_CAM_2BYTE_MODE    (BIT(1))
#define ISP_CAM_2BYTE_MODE_M  (ISP_CAM_2BYTE_MODE_V << ISP_CAM_2BYTE_MODE_S)
#define ISP_CAM_2BYTE_MODE_V  0x00000001U
#define ISP_CAM_2BYTE_MODE_S  1
/** ISP_CAM_DATA_TYPE : R/W; bitpos: [7:2]; default: 42;
 *  this field configures idi data type for image data, 0x2a: RAW8, 0x2b: RAW10, 0x2c:
 *  RAW12
 */
#define ISP_CAM_DATA_TYPE    0x0000003FU
#define ISP_CAM_DATA_TYPE_M  (ISP_CAM_DATA_TYPE_V << ISP_CAM_DATA_TYPE_S)
#define ISP_CAM_DATA_TYPE_V  0x0000003FU
#define ISP_CAM_DATA_TYPE_S  2
/** ISP_CAM_DE_INV : R/W; bitpos: [8]; default: 0;
 *  this bit configures cam data enable invert. 0: not invert, 1: invert
 */
#define ISP_CAM_DE_INV    (BIT(8))
#define ISP_CAM_DE_INV_M  (ISP_CAM_DE_INV_V << ISP_CAM_DE_INV_S)
#define ISP_CAM_DE_INV_V  0x00000001U
#define ISP_CAM_DE_INV_S  8
/** ISP_CAM_HSYNC_INV : R/W; bitpos: [9]; default: 0;
 *  this bit configures cam hsync invert. 0: not invert, 1: invert
 */
#define ISP_CAM_HSYNC_INV    (BIT(9))
#define ISP_CAM_HSYNC_INV_M  (ISP_CAM_HSYNC_INV_V << ISP_CAM_HSYNC_INV_S)
#define ISP_CAM_HSYNC_INV_V  0x00000001U
#define ISP_CAM_HSYNC_INV_S  9
/** ISP_CAM_VSYNC_INV : R/W; bitpos: [10]; default: 0;
 *  this bit configures cam vsync invert. 0: not invert, 1: invert
 */
#define ISP_CAM_VSYNC_INV    (BIT(10))
#define ISP_CAM_VSYNC_INV_M  (ISP_CAM_VSYNC_INV_V << ISP_CAM_VSYNC_INV_S)
#define ISP_CAM_VSYNC_INV_V  0x00000001U
#define ISP_CAM_VSYNC_INV_S  10
/** ISP_CAM_VSYNC_FILTER_THRES : R/W; bitpos: [13:11]; default: 0;
 *  this bit configures the number of clock of vsync filter length
 */
#define ISP_CAM_VSYNC_FILTER_THRES    0x00000007U
#define ISP_CAM_VSYNC_FILTER_THRES_M  (ISP_CAM_VSYNC_FILTER_THRES_V << ISP_CAM_VSYNC_FILTER_THRES_S)
#define ISP_CAM_VSYNC_FILTER_THRES_V  0x00000007U
#define ISP_CAM_VSYNC_FILTER_THRES_S  11
/** ISP_CAM_VSYNC_FILTER_EN : R/W; bitpos: [14]; default: 0;
 *  this bit configures vsync filter en
 */
#define ISP_CAM_VSYNC_FILTER_EN    (BIT(14))
#define ISP_CAM_VSYNC_FILTER_EN_M  (ISP_CAM_VSYNC_FILTER_EN_V << ISP_CAM_VSYNC_FILTER_EN_S)
#define ISP_CAM_VSYNC_FILTER_EN_V  0x00000001U
#define ISP_CAM_VSYNC_FILTER_EN_S  14

/** ISP_AF_CTRL0_REG register
 *  af control register 0
 */
#define ISP_AF_CTRL0_REG (DR_REG_ISP_BASE + 0x11c)
/** ISP_AF_AUTO_UPDATE : R/W; bitpos: [0]; default: 0;
 *  this bit configures auto_update enable. when set to 1, will update sum and lum each
 *  frame
 */
#define ISP_AF_AUTO_UPDATE    (BIT(0))
#define ISP_AF_AUTO_UPDATE_M  (ISP_AF_AUTO_UPDATE_V << ISP_AF_AUTO_UPDATE_S)
#define ISP_AF_AUTO_UPDATE_V  0x00000001U
#define ISP_AF_AUTO_UPDATE_S  0
/** ISP_AF_MANUAL_UPDATE : WT; bitpos: [4]; default: 0;
 *  write 1 to this bit will update the sum and lum once
 */
#define ISP_AF_MANUAL_UPDATE    (BIT(4))
#define ISP_AF_MANUAL_UPDATE_M  (ISP_AF_MANUAL_UPDATE_V << ISP_AF_MANUAL_UPDATE_S)
#define ISP_AF_MANUAL_UPDATE_V  0x00000001U
#define ISP_AF_MANUAL_UPDATE_S  4
/** ISP_AF_ENV_THRESHOLD : R/W; bitpos: [11:8]; default: 0;
 *  this field configures env threshold. when both sum and lum changes larger than this
 *  value, consider environment changes and need to trigger a new autofocus. 4Bit
 *  fractional
 */
#define ISP_AF_ENV_THRESHOLD    0x0000000FU
#define ISP_AF_ENV_THRESHOLD_M  (ISP_AF_ENV_THRESHOLD_V << ISP_AF_ENV_THRESHOLD_S)
#define ISP_AF_ENV_THRESHOLD_V  0x0000000FU
#define ISP_AF_ENV_THRESHOLD_S  8
/** ISP_AF_ENV_PERIOD : R/W; bitpos: [23:16]; default: 0;
 *  this field configures environment changes detection period (frame). When set to 0,
 *  disable this function
 */
#define ISP_AF_ENV_PERIOD    0x000000FFU
#define ISP_AF_ENV_PERIOD_M  (ISP_AF_ENV_PERIOD_V << ISP_AF_ENV_PERIOD_S)
#define ISP_AF_ENV_PERIOD_V  0x000000FFU
#define ISP_AF_ENV_PERIOD_S  16

/** ISP_AF_CTRL1_REG register
 *  af control register 1
 */
#define ISP_AF_CTRL1_REG (DR_REG_ISP_BASE + 0x120)
/** ISP_AF_THPIXNUM : R/W; bitpos: [21:0]; default: 0;
 *  this field configures pixnum used when calculating the autofocus threshold. Set to
 *  0 to disable threshold calculation
 */
#define ISP_AF_THPIXNUM    0x003FFFFFU
#define ISP_AF_THPIXNUM_M  (ISP_AF_THPIXNUM_V << ISP_AF_THPIXNUM_S)
#define ISP_AF_THPIXNUM_V  0x003FFFFFU
#define ISP_AF_THPIXNUM_S  0

/** ISP_AF_GEN_TH_CTRL_REG register
 *  af gen threshold control register
 */
#define ISP_AF_GEN_TH_CTRL_REG (DR_REG_ISP_BASE + 0x124)
/** ISP_AF_GEN_THRESHOLD_MIN : R/W; bitpos: [15:0]; default: 128;
 *  this field configures min threshold when use auto_threshold
 */
#define ISP_AF_GEN_THRESHOLD_MIN    0x0000FFFFU
#define ISP_AF_GEN_THRESHOLD_MIN_M  (ISP_AF_GEN_THRESHOLD_MIN_V << ISP_AF_GEN_THRESHOLD_MIN_S)
#define ISP_AF_GEN_THRESHOLD_MIN_V  0x0000FFFFU
#define ISP_AF_GEN_THRESHOLD_MIN_S  0
/** ISP_AF_GEN_THRESHOLD_MAX : R/W; bitpos: [31:16]; default: 1088;
 *  this field configures max threshold when use auto_threshold
 */
#define ISP_AF_GEN_THRESHOLD_MAX    0x0000FFFFU
#define ISP_AF_GEN_THRESHOLD_MAX_M  (ISP_AF_GEN_THRESHOLD_MAX_V << ISP_AF_GEN_THRESHOLD_MAX_S)
#define ISP_AF_GEN_THRESHOLD_MAX_V  0x0000FFFFU
#define ISP_AF_GEN_THRESHOLD_MAX_S  16

/** ISP_AF_ENV_USER_TH_SUM_REG register
 *  af monitor user sum threshold register
 */
#define ISP_AF_ENV_USER_TH_SUM_REG (DR_REG_ISP_BASE + 0x128)
/** ISP_AF_ENV_USER_THRESHOLD_SUM : R/W; bitpos: [31:0]; default: 0;
 *  this field configures user setup env detect sum threshold
 */
#define ISP_AF_ENV_USER_THRESHOLD_SUM    0xFFFFFFFFU
#define ISP_AF_ENV_USER_THRESHOLD_SUM_M  (ISP_AF_ENV_USER_THRESHOLD_SUM_V << ISP_AF_ENV_USER_THRESHOLD_SUM_S)
#define ISP_AF_ENV_USER_THRESHOLD_SUM_V  0xFFFFFFFFU
#define ISP_AF_ENV_USER_THRESHOLD_SUM_S  0

/** ISP_AF_ENV_USER_TH_LUM_REG register
 *  af monitor user lum threshold register
 */
#define ISP_AF_ENV_USER_TH_LUM_REG (DR_REG_ISP_BASE + 0x12c)
/** ISP_AF_ENV_USER_THRESHOLD_LUM : R/W; bitpos: [29:0]; default: 0;
 *  this field configures user setup env detect lum threshold
 */
#define ISP_AF_ENV_USER_THRESHOLD_LUM    0x3FFFFFFFU
#define ISP_AF_ENV_USER_THRESHOLD_LUM_M  (ISP_AF_ENV_USER_THRESHOLD_LUM_V << ISP_AF_ENV_USER_THRESHOLD_LUM_S)
#define ISP_AF_ENV_USER_THRESHOLD_LUM_V  0x3FFFFFFFU
#define ISP_AF_ENV_USER_THRESHOLD_LUM_S  0

/** ISP_AF_THRESHOLD_REG register
 *  af threshold register
 */
#define ISP_AF_THRESHOLD_REG (DR_REG_ISP_BASE + 0x130)
/** ISP_AF_THRESHOLD : R/W; bitpos: [15:0]; default: 256;
 *  this field configures user threshold. When set to non-zero, autofocus will use this
 *  threshold
 */
#define ISP_AF_THRESHOLD    0x0000FFFFU
#define ISP_AF_THRESHOLD_M  (ISP_AF_THRESHOLD_V << ISP_AF_THRESHOLD_S)
#define ISP_AF_THRESHOLD_V  0x0000FFFFU
#define ISP_AF_THRESHOLD_S  0
/** ISP_AF_GEN_THRESHOLD : RO; bitpos: [31:16]; default: 0;
 *  this field represents the last calculated threshold
 */
#define ISP_AF_GEN_THRESHOLD    0x0000FFFFU
#define ISP_AF_GEN_THRESHOLD_M  (ISP_AF_GEN_THRESHOLD_V << ISP_AF_GEN_THRESHOLD_S)
#define ISP_AF_GEN_THRESHOLD_V  0x0000FFFFU
#define ISP_AF_GEN_THRESHOLD_S  16

/** ISP_AF_HSCALE_A_REG register
 *  h-scale of af window a register
 */
#define ISP_AF_HSCALE_A_REG (DR_REG_ISP_BASE + 0x134)
/** ISP_AF_RPOINT_A : R/W; bitpos: [11:0]; default: 128;
 *  this field configures left coordinate of focus window a, must >= 2
 */
#define ISP_AF_RPOINT_A    0x00000FFFU
#define ISP_AF_RPOINT_A_M  (ISP_AF_RPOINT_A_V << ISP_AF_RPOINT_A_S)
#define ISP_AF_RPOINT_A_V  0x00000FFFU
#define ISP_AF_RPOINT_A_S  0
/** ISP_AF_LPOINT_A : R/W; bitpos: [27:16]; default: 1;
 *  this field configures top coordinate of focus window a, must >= 2
 */
#define ISP_AF_LPOINT_A    0x00000FFFU
#define ISP_AF_LPOINT_A_M  (ISP_AF_LPOINT_A_V << ISP_AF_LPOINT_A_S)
#define ISP_AF_LPOINT_A_V  0x00000FFFU
#define ISP_AF_LPOINT_A_S  16

/** ISP_AF_VSCALE_A_REG register
 *  v-scale of af window a register
 */
#define ISP_AF_VSCALE_A_REG (DR_REG_ISP_BASE + 0x138)
/** ISP_AF_BPOINT_A : R/W; bitpos: [11:0]; default: 128;
 *  this field configures right coordinate of focus window a, must <= hnum-2
 */
#define ISP_AF_BPOINT_A    0x00000FFFU
#define ISP_AF_BPOINT_A_M  (ISP_AF_BPOINT_A_V << ISP_AF_BPOINT_A_S)
#define ISP_AF_BPOINT_A_V  0x00000FFFU
#define ISP_AF_BPOINT_A_S  0
/** ISP_AF_TPOINT_A : R/W; bitpos: [27:16]; default: 1;
 *  this field configures bottom coordinate of focus window a, must <= hnum-2
 */
#define ISP_AF_TPOINT_A    0x00000FFFU
#define ISP_AF_TPOINT_A_M  (ISP_AF_TPOINT_A_V << ISP_AF_TPOINT_A_S)
#define ISP_AF_TPOINT_A_V  0x00000FFFU
#define ISP_AF_TPOINT_A_S  16

/** ISP_AF_HSCALE_B_REG register
 *  h-scale of af window b register
 */
#define ISP_AF_HSCALE_B_REG (DR_REG_ISP_BASE + 0x13c)
/** ISP_AF_RPOINT_B : R/W; bitpos: [11:0]; default: 128;
 *  this field configures left coordinate of focus window b, must >= 2
 */
#define ISP_AF_RPOINT_B    0x00000FFFU
#define ISP_AF_RPOINT_B_M  (ISP_AF_RPOINT_B_V << ISP_AF_RPOINT_B_S)
#define ISP_AF_RPOINT_B_V  0x00000FFFU
#define ISP_AF_RPOINT_B_S  0
/** ISP_AF_LPOINT_B : R/W; bitpos: [27:16]; default: 1;
 *  this field configures top coordinate of focus window b, must >= 2
 */
#define ISP_AF_LPOINT_B    0x00000FFFU
#define ISP_AF_LPOINT_B_M  (ISP_AF_LPOINT_B_V << ISP_AF_LPOINT_B_S)
#define ISP_AF_LPOINT_B_V  0x00000FFFU
#define ISP_AF_LPOINT_B_S  16

/** ISP_AF_VSCALE_B_REG register
 *  v-scale of af window b register
 */
#define ISP_AF_VSCALE_B_REG (DR_REG_ISP_BASE + 0x140)
/** ISP_AF_BPOINT_B : R/W; bitpos: [11:0]; default: 128;
 *  this field configures right coordinate of focus window b, must <= hnum-2
 */
#define ISP_AF_BPOINT_B    0x00000FFFU
#define ISP_AF_BPOINT_B_M  (ISP_AF_BPOINT_B_V << ISP_AF_BPOINT_B_S)
#define ISP_AF_BPOINT_B_V  0x00000FFFU
#define ISP_AF_BPOINT_B_S  0
/** ISP_AF_TPOINT_B : R/W; bitpos: [27:16]; default: 1;
 *  this field configures bottom coordinate of focus window b, must <= hnum-2
 */
#define ISP_AF_TPOINT_B    0x00000FFFU
#define ISP_AF_TPOINT_B_M  (ISP_AF_TPOINT_B_V << ISP_AF_TPOINT_B_S)
#define ISP_AF_TPOINT_B_V  0x00000FFFU
#define ISP_AF_TPOINT_B_S  16

/** ISP_AF_HSCALE_C_REG register
 *  v-scale of af window c register
 */
#define ISP_AF_HSCALE_C_REG (DR_REG_ISP_BASE + 0x144)
/** ISP_AF_RPOINT_C : R/W; bitpos: [11:0]; default: 128;
 *  this field configures left coordinate of focus window c, must >= 2
 */
#define ISP_AF_RPOINT_C    0x00000FFFU
#define ISP_AF_RPOINT_C_M  (ISP_AF_RPOINT_C_V << ISP_AF_RPOINT_C_S)
#define ISP_AF_RPOINT_C_V  0x00000FFFU
#define ISP_AF_RPOINT_C_S  0
/** ISP_AF_LPOINT_C : R/W; bitpos: [27:16]; default: 1;
 *  this field configures top coordinate of focus window c, must >= 2
 */
#define ISP_AF_LPOINT_C    0x00000FFFU
#define ISP_AF_LPOINT_C_M  (ISP_AF_LPOINT_C_V << ISP_AF_LPOINT_C_S)
#define ISP_AF_LPOINT_C_V  0x00000FFFU
#define ISP_AF_LPOINT_C_S  16

/** ISP_AF_VSCALE_C_REG register
 *  v-scale of af window c register
 */
#define ISP_AF_VSCALE_C_REG (DR_REG_ISP_BASE + 0x148)
/** ISP_AF_BPOINT_C : R/W; bitpos: [11:0]; default: 128;
 *  this field configures right coordinate of focus window c, must <= hnum-2
 */
#define ISP_AF_BPOINT_C    0x00000FFFU
#define ISP_AF_BPOINT_C_M  (ISP_AF_BPOINT_C_V << ISP_AF_BPOINT_C_S)
#define ISP_AF_BPOINT_C_V  0x00000FFFU
#define ISP_AF_BPOINT_C_S  0
/** ISP_AF_TPOINT_C : R/W; bitpos: [27:16]; default: 1;
 *  this field configures bottom coordinate of focus window c, must <= hnum-2
 */
#define ISP_AF_TPOINT_C    0x00000FFFU
#define ISP_AF_TPOINT_C_M  (ISP_AF_TPOINT_C_V << ISP_AF_TPOINT_C_S)
#define ISP_AF_TPOINT_C_V  0x00000FFFU
#define ISP_AF_TPOINT_C_S  16

/** ISP_AF_SUM_A_REG register
 *  result of sum of af window a
 */
#define ISP_AF_SUM_A_REG (DR_REG_ISP_BASE + 0x14c)
/** ISP_AF_SUMA : RO; bitpos: [29:0]; default: 0;
 *  this field represents the result of accumulation of pix grad of focus window a
 */
#define ISP_AF_SUMA    0x3FFFFFFFU
#define ISP_AF_SUMA_M  (ISP_AF_SUMA_V << ISP_AF_SUMA_S)
#define ISP_AF_SUMA_V  0x3FFFFFFFU
#define ISP_AF_SUMA_S  0

/** ISP_AF_SUM_B_REG register
 *  result of sum of af window b
 */
#define ISP_AF_SUM_B_REG (DR_REG_ISP_BASE + 0x150)
/** ISP_AF_SUMB : RO; bitpos: [29:0]; default: 0;
 *  this field represents the result of accumulation of pix grad of focus window b
 */
#define ISP_AF_SUMB    0x3FFFFFFFU
#define ISP_AF_SUMB_M  (ISP_AF_SUMB_V << ISP_AF_SUMB_S)
#define ISP_AF_SUMB_V  0x3FFFFFFFU
#define ISP_AF_SUMB_S  0

/** ISP_AF_SUM_C_REG register
 *  result of sum of af window c
 */
#define ISP_AF_SUM_C_REG (DR_REG_ISP_BASE + 0x154)
/** ISP_AF_SUMC : RO; bitpos: [29:0]; default: 0;
 *  this field represents the result of accumulation of pix grad of focus window c
 */
#define ISP_AF_SUMC    0x3FFFFFFFU
#define ISP_AF_SUMC_M  (ISP_AF_SUMC_V << ISP_AF_SUMC_S)
#define ISP_AF_SUMC_V  0x3FFFFFFFU
#define ISP_AF_SUMC_S  0

/** ISP_AF_LUM_A_REG register
 *  result of lum of af window a
 */
#define ISP_AF_LUM_A_REG (DR_REG_ISP_BASE + 0x158)
/** ISP_AF_LUMA : RO; bitpos: [27:0]; default: 0;
 *  this field represents the result of accumulation of pix light of focus window a
 */
#define ISP_AF_LUMA    0x0FFFFFFFU
#define ISP_AF_LUMA_M  (ISP_AF_LUMA_V << ISP_AF_LUMA_S)
#define ISP_AF_LUMA_V  0x0FFFFFFFU
#define ISP_AF_LUMA_S  0

/** ISP_AF_LUM_B_REG register
 *  result of lum of af window b
 */
#define ISP_AF_LUM_B_REG (DR_REG_ISP_BASE + 0x15c)
/** ISP_AF_LUMB : RO; bitpos: [27:0]; default: 0;
 *  this field represents the result of accumulation of pix light of focus window b
 */
#define ISP_AF_LUMB    0x0FFFFFFFU
#define ISP_AF_LUMB_M  (ISP_AF_LUMB_V << ISP_AF_LUMB_S)
#define ISP_AF_LUMB_V  0x0FFFFFFFU
#define ISP_AF_LUMB_S  0

/** ISP_AF_LUM_C_REG register
 *  result of lum of af window c
 */
#define ISP_AF_LUM_C_REG (DR_REG_ISP_BASE + 0x160)
/** ISP_AF_LUMC : RO; bitpos: [27:0]; default: 0;
 *  this field represents the result of accumulation of pix light of focus window c
 */
#define ISP_AF_LUMC    0x0FFFFFFFU
#define ISP_AF_LUMC_M  (ISP_AF_LUMC_V << ISP_AF_LUMC_S)
#define ISP_AF_LUMC_V  0x0FFFFFFFU
#define ISP_AF_LUMC_S  0

/** ISP_AWB_MODE_REG register
 *  awb mode control register
 */
#define ISP_AWB_MODE_REG (DR_REG_ISP_BASE + 0x164)
/** ISP_AWB_MODE : R/W; bitpos: [1:0]; default: 3;
 *  this field configures awb algo sel. 00: none selected. 01: sel algo0. 10: sel
 *  algo1. 11: sel both algo0 and algo1
 */
#define ISP_AWB_MODE    0x00000003U
#define ISP_AWB_MODE_M  (ISP_AWB_MODE_V << ISP_AWB_MODE_S)
#define ISP_AWB_MODE_V  0x00000003U
#define ISP_AWB_MODE_S  0
/** ISP_AWB_SAMPLE : R/W; bitpos: [4]; default: 0;
 *  this bit configures awb sample location, 0:before ccm, 1:after ccm
 */
#define ISP_AWB_SAMPLE    (BIT(4))
#define ISP_AWB_SAMPLE_M  (ISP_AWB_SAMPLE_V << ISP_AWB_SAMPLE_S)
#define ISP_AWB_SAMPLE_V  0x00000001U
#define ISP_AWB_SAMPLE_S  4

/** ISP_AWB_HSCALE_REG register
 *  h-scale of awb window
 */
#define ISP_AWB_HSCALE_REG (DR_REG_ISP_BASE + 0x168)
/** ISP_AWB_RPOINT : R/W; bitpos: [11:0]; default: 1919;
 *  this field configures awb window right coordinate
 */
#define ISP_AWB_RPOINT    0x00000FFFU
#define ISP_AWB_RPOINT_M  (ISP_AWB_RPOINT_V << ISP_AWB_RPOINT_S)
#define ISP_AWB_RPOINT_V  0x00000FFFU
#define ISP_AWB_RPOINT_S  0
/** ISP_AWB_LPOINT : R/W; bitpos: [27:16]; default: 0;
 *  this field configures awb window left coordinate
 */
#define ISP_AWB_LPOINT    0x00000FFFU
#define ISP_AWB_LPOINT_M  (ISP_AWB_LPOINT_V << ISP_AWB_LPOINT_S)
#define ISP_AWB_LPOINT_V  0x00000FFFU
#define ISP_AWB_LPOINT_S  16

/** ISP_AWB_VSCALE_REG register
 *  v-scale of awb window
 */
#define ISP_AWB_VSCALE_REG (DR_REG_ISP_BASE + 0x16c)
/** ISP_AWB_BPOINT : R/W; bitpos: [11:0]; default: 1079;
 *  this field configures awb window bottom coordinate
 */
#define ISP_AWB_BPOINT    0x00000FFFU
#define ISP_AWB_BPOINT_M  (ISP_AWB_BPOINT_V << ISP_AWB_BPOINT_S)
#define ISP_AWB_BPOINT_V  0x00000FFFU
#define ISP_AWB_BPOINT_S  0
/** ISP_AWB_TPOINT : R/W; bitpos: [27:16]; default: 0;
 *  this field configures awb window top coordinate
 */
#define ISP_AWB_TPOINT    0x00000FFFU
#define ISP_AWB_TPOINT_M  (ISP_AWB_TPOINT_V << ISP_AWB_TPOINT_S)
#define ISP_AWB_TPOINT_V  0x00000FFFU
#define ISP_AWB_TPOINT_S  16

/** ISP_AWB_TH_LUM_REG register
 *  awb lum threshold register
 */
#define ISP_AWB_TH_LUM_REG (DR_REG_ISP_BASE + 0x170)
/** ISP_AWB_MIN_LUM : R/W; bitpos: [9:0]; default: 0;
 *  this field configures lower threshold of r+g+b
 */
#define ISP_AWB_MIN_LUM    0x000003FFU
#define ISP_AWB_MIN_LUM_M  (ISP_AWB_MIN_LUM_V << ISP_AWB_MIN_LUM_S)
#define ISP_AWB_MIN_LUM_V  0x000003FFU
#define ISP_AWB_MIN_LUM_S  0
/** ISP_AWB_MAX_LUM : R/W; bitpos: [25:16]; default: 765;
 *  this field configures upper threshold of r+g+b
 */
#define ISP_AWB_MAX_LUM    0x000003FFU
#define ISP_AWB_MAX_LUM_M  (ISP_AWB_MAX_LUM_V << ISP_AWB_MAX_LUM_S)
#define ISP_AWB_MAX_LUM_V  0x000003FFU
#define ISP_AWB_MAX_LUM_S  16

/** ISP_AWB_TH_RG_REG register
 *  awb r/g threshold register
 */
#define ISP_AWB_TH_RG_REG (DR_REG_ISP_BASE + 0x174)
/** ISP_AWB_MIN_RG : R/W; bitpos: [9:0]; default: 0;
 *  this field configures lower threshold of r/g, 2bit integer and 8bit fraction
 */
#define ISP_AWB_MIN_RG    0x000003FFU
#define ISP_AWB_MIN_RG_M  (ISP_AWB_MIN_RG_V << ISP_AWB_MIN_RG_S)
#define ISP_AWB_MIN_RG_V  0x000003FFU
#define ISP_AWB_MIN_RG_S  0
/** ISP_AWB_MAX_RG : R/W; bitpos: [25:16]; default: 1023;
 *  this field configures upper threshold of r/g, 2bit integer and 8bit fraction
 */
#define ISP_AWB_MAX_RG    0x000003FFU
#define ISP_AWB_MAX_RG_M  (ISP_AWB_MAX_RG_V << ISP_AWB_MAX_RG_S)
#define ISP_AWB_MAX_RG_V  0x000003FFU
#define ISP_AWB_MAX_RG_S  16

/** ISP_AWB_TH_BG_REG register
 *  awb b/g threshold register
 */
#define ISP_AWB_TH_BG_REG (DR_REG_ISP_BASE + 0x178)
/** ISP_AWB_MIN_BG : R/W; bitpos: [9:0]; default: 0;
 *  this field configures lower threshold of b/g, 2bit integer and 8bit fraction
 */
#define ISP_AWB_MIN_BG    0x000003FFU
#define ISP_AWB_MIN_BG_M  (ISP_AWB_MIN_BG_V << ISP_AWB_MIN_BG_S)
#define ISP_AWB_MIN_BG_V  0x000003FFU
#define ISP_AWB_MIN_BG_S  0
/** ISP_AWB_MAX_BG : R/W; bitpos: [25:16]; default: 1023;
 *  this field configures upper threshold of b/g, 2bit integer and 8bit fraction
 */
#define ISP_AWB_MAX_BG    0x000003FFU
#define ISP_AWB_MAX_BG_M  (ISP_AWB_MAX_BG_V << ISP_AWB_MAX_BG_S)
#define ISP_AWB_MAX_BG_V  0x000003FFU
#define ISP_AWB_MAX_BG_S  16

/** ISP_AWB0_WHITE_CNT_REG register
 *  result of awb white point number
 */
#define ISP_AWB0_WHITE_CNT_REG (DR_REG_ISP_BASE + 0x17c)
/** ISP_AWB0_WHITE_CNT : RO; bitpos: [23:0]; default: 0;
 *  this field configures number of white point detected of algo0
 */
#define ISP_AWB0_WHITE_CNT    0x00FFFFFFU
#define ISP_AWB0_WHITE_CNT_M  (ISP_AWB0_WHITE_CNT_V << ISP_AWB0_WHITE_CNT_S)
#define ISP_AWB0_WHITE_CNT_V  0x00FFFFFFU
#define ISP_AWB0_WHITE_CNT_S  0

/** ISP_AWB0_ACC_R_REG register
 *  result of accumulate of r channel of all white points
 */
#define ISP_AWB0_ACC_R_REG (DR_REG_ISP_BASE + 0x180)
/** ISP_AWB0_ACC_R : RO; bitpos: [31:0]; default: 0;
 *  this field represents accumulate of channel r of all white point of algo0
 */
#define ISP_AWB0_ACC_R    0xFFFFFFFFU
#define ISP_AWB0_ACC_R_M  (ISP_AWB0_ACC_R_V << ISP_AWB0_ACC_R_S)
#define ISP_AWB0_ACC_R_V  0xFFFFFFFFU
#define ISP_AWB0_ACC_R_S  0

/** ISP_AWB0_ACC_G_REG register
 *  result of accumulate of g channel of all white points
 */
#define ISP_AWB0_ACC_G_REG (DR_REG_ISP_BASE + 0x184)
/** ISP_AWB0_ACC_G : RO; bitpos: [31:0]; default: 0;
 *  this field represents accumulate of channel g of all white point of algo0
 */
#define ISP_AWB0_ACC_G    0xFFFFFFFFU
#define ISP_AWB0_ACC_G_M  (ISP_AWB0_ACC_G_V << ISP_AWB0_ACC_G_S)
#define ISP_AWB0_ACC_G_V  0xFFFFFFFFU
#define ISP_AWB0_ACC_G_S  0

/** ISP_AWB0_ACC_B_REG register
 *  result of accumulate of b channel of all white points
 */
#define ISP_AWB0_ACC_B_REG (DR_REG_ISP_BASE + 0x188)
/** ISP_AWB0_ACC_B : RO; bitpos: [31:0]; default: 0;
 *  this field represents accumulate of channel b of all white point of algo0
 */
#define ISP_AWB0_ACC_B    0xFFFFFFFFU
#define ISP_AWB0_ACC_B_M  (ISP_AWB0_ACC_B_V << ISP_AWB0_ACC_B_S)
#define ISP_AWB0_ACC_B_V  0xFFFFFFFFU
#define ISP_AWB0_ACC_B_S  0

/** ISP_COLOR_CTRL_REG register
 *  color control register
 */
#define ISP_COLOR_CTRL_REG (DR_REG_ISP_BASE + 0x18c)
/** ISP_COLOR_SATURATION : R/W; bitpos: [7:0]; default: 128;
 *  this field configures the color saturation value
 */
#define ISP_COLOR_SATURATION    0x000000FFU
#define ISP_COLOR_SATURATION_M  (ISP_COLOR_SATURATION_V << ISP_COLOR_SATURATION_S)
#define ISP_COLOR_SATURATION_V  0x000000FFU
#define ISP_COLOR_SATURATION_S  0
/** ISP_COLOR_HUE : R/W; bitpos: [15:8]; default: 0;
 *  this field configures the color hue angle
 */
#define ISP_COLOR_HUE    0x000000FFU
#define ISP_COLOR_HUE_M  (ISP_COLOR_HUE_V << ISP_COLOR_HUE_S)
#define ISP_COLOR_HUE_V  0x000000FFU
#define ISP_COLOR_HUE_S  8
/** ISP_COLOR_CONTRAST : R/W; bitpos: [23:16]; default: 128;
 *  this field configures the color contrast value
 */
#define ISP_COLOR_CONTRAST    0x000000FFU
#define ISP_COLOR_CONTRAST_M  (ISP_COLOR_CONTRAST_V << ISP_COLOR_CONTRAST_S)
#define ISP_COLOR_CONTRAST_V  0x000000FFU
#define ISP_COLOR_CONTRAST_S  16
/** ISP_COLOR_BRIGHTNESS : R/W; bitpos: [31:24]; default: 0;
 *  this field configures the color brightness value, signed 2's complement
 */
#define ISP_COLOR_BRIGHTNESS    0x000000FFU
#define ISP_COLOR_BRIGHTNESS_M  (ISP_COLOR_BRIGHTNESS_V << ISP_COLOR_BRIGHTNESS_S)
#define ISP_COLOR_BRIGHTNESS_V  0x000000FFU
#define ISP_COLOR_BRIGHTNESS_S  24

/** ISP_BLC_VALUE_REG register
 *  blc black level register
 */
#define ISP_BLC_VALUE_REG (DR_REG_ISP_BASE + 0x190)
/** ISP_BLC_R3_VALUE : R/W; bitpos: [7:0]; default: 0;
 *  this field configures the black level of bottom right channel of bayer img
 */
#define ISP_BLC_R3_VALUE    0x000000FFU
#define ISP_BLC_R3_VALUE_M  (ISP_BLC_R3_VALUE_V << ISP_BLC_R3_VALUE_S)
#define ISP_BLC_R3_VALUE_V  0x000000FFU
#define ISP_BLC_R3_VALUE_S  0
/** ISP_BLC_R2_VALUE : R/W; bitpos: [15:8]; default: 0;
 *  this field configures the black level of bottom left channel of bayer img
 */
#define ISP_BLC_R2_VALUE    0x000000FFU
#define ISP_BLC_R2_VALUE_M  (ISP_BLC_R2_VALUE_V << ISP_BLC_R2_VALUE_S)
#define ISP_BLC_R2_VALUE_V  0x000000FFU
#define ISP_BLC_R2_VALUE_S  8
/** ISP_BLC_R1_VALUE : R/W; bitpos: [23:16]; default: 0;
 *  this field configures the black level of top right channel of bayer img
 */
#define ISP_BLC_R1_VALUE    0x000000FFU
#define ISP_BLC_R1_VALUE_M  (ISP_BLC_R1_VALUE_V << ISP_BLC_R1_VALUE_S)
#define ISP_BLC_R1_VALUE_V  0x000000FFU
#define ISP_BLC_R1_VALUE_S  16
/** ISP_BLC_R0_VALUE : R/W; bitpos: [31:24]; default: 0;
 *  this field configures the black level of top left channel of bayer img
 */
#define ISP_BLC_R0_VALUE    0x000000FFU
#define ISP_BLC_R0_VALUE_M  (ISP_BLC_R0_VALUE_V << ISP_BLC_R0_VALUE_S)
#define ISP_BLC_R0_VALUE_V  0x000000FFU
#define ISP_BLC_R0_VALUE_S  24

/** ISP_BLC_CTRL0_REG register
 *  blc stretch control register
 */
#define ISP_BLC_CTRL0_REG (DR_REG_ISP_BASE + 0x194)
/** ISP_BLC_R3_STRETCH : R/W; bitpos: [0]; default: 0;
 *  this bit configures the stretch feature of bottom right channel. 0: stretch
 *  disable, 1: stretch enable
 */
#define ISP_BLC_R3_STRETCH    (BIT(0))
#define ISP_BLC_R3_STRETCH_M  (ISP_BLC_R3_STRETCH_V << ISP_BLC_R3_STRETCH_S)
#define ISP_BLC_R3_STRETCH_V  0x00000001U
#define ISP_BLC_R3_STRETCH_S  0
/** ISP_BLC_R2_STRETCH : R/W; bitpos: [1]; default: 0;
 *  this bit configures the stretch feature of bottom left channel. 0: stretch disable,
 *  1: stretch enable
 */
#define ISP_BLC_R2_STRETCH    (BIT(1))
#define ISP_BLC_R2_STRETCH_M  (ISP_BLC_R2_STRETCH_V << ISP_BLC_R2_STRETCH_S)
#define ISP_BLC_R2_STRETCH_V  0x00000001U
#define ISP_BLC_R2_STRETCH_S  1
/** ISP_BLC_R1_STRETCH : R/W; bitpos: [2]; default: 0;
 *  this bit configures the stretch feature of top right channel. 0: stretch disable,
 *  1: stretch enable
 */
#define ISP_BLC_R1_STRETCH    (BIT(2))
#define ISP_BLC_R1_STRETCH_M  (ISP_BLC_R1_STRETCH_V << ISP_BLC_R1_STRETCH_S)
#define ISP_BLC_R1_STRETCH_V  0x00000001U
#define ISP_BLC_R1_STRETCH_S  2
/** ISP_BLC_R0_STRETCH : R/W; bitpos: [3]; default: 0;
 *  this bit configures the stretch feature of top left channel. 0: stretch disable, 1:
 *  stretch enable
 */
#define ISP_BLC_R0_STRETCH    (BIT(3))
#define ISP_BLC_R0_STRETCH_M  (ISP_BLC_R0_STRETCH_V << ISP_BLC_R0_STRETCH_S)
#define ISP_BLC_R0_STRETCH_V  0x00000001U
#define ISP_BLC_R0_STRETCH_S  3

/** ISP_BLC_CTRL1_REG register
 *  blc window control register
 */
#define ISP_BLC_CTRL1_REG (DR_REG_ISP_BASE + 0x198)
/** ISP_BLC_WINDOW_TOP : R/W; bitpos: [10:0]; default: 0;
 *  this field configures blc average calculation window top
 */
#define ISP_BLC_WINDOW_TOP    0x000007FFU
#define ISP_BLC_WINDOW_TOP_M  (ISP_BLC_WINDOW_TOP_V << ISP_BLC_WINDOW_TOP_S)
#define ISP_BLC_WINDOW_TOP_V  0x000007FFU
#define ISP_BLC_WINDOW_TOP_S  0
/** ISP_BLC_WINDOW_LEFT : R/W; bitpos: [21:11]; default: 0;
 *  this field configures blc average calculation window left
 */
#define ISP_BLC_WINDOW_LEFT    0x000007FFU
#define ISP_BLC_WINDOW_LEFT_M  (ISP_BLC_WINDOW_LEFT_V << ISP_BLC_WINDOW_LEFT_S)
#define ISP_BLC_WINDOW_LEFT_V  0x000007FFU
#define ISP_BLC_WINDOW_LEFT_S  11
/** ISP_BLC_WINDOW_VNUM : R/W; bitpos: [25:22]; default: 0;
 *  this field configures blc average calculation window vnum
 */
#define ISP_BLC_WINDOW_VNUM    0x0000000FU
#define ISP_BLC_WINDOW_VNUM_M  (ISP_BLC_WINDOW_VNUM_V << ISP_BLC_WINDOW_VNUM_S)
#define ISP_BLC_WINDOW_VNUM_V  0x0000000FU
#define ISP_BLC_WINDOW_VNUM_S  22
/** ISP_BLC_WINDOW_HNUM : R/W; bitpos: [29:26]; default: 0;
 *  this field configures blc average calculation window hnum
 */
#define ISP_BLC_WINDOW_HNUM    0x0000000FU
#define ISP_BLC_WINDOW_HNUM_M  (ISP_BLC_WINDOW_HNUM_V << ISP_BLC_WINDOW_HNUM_S)
#define ISP_BLC_WINDOW_HNUM_V  0x0000000FU
#define ISP_BLC_WINDOW_HNUM_S  26
/** ISP_BLC_FILTER_EN : R/W; bitpos: [30]; default: 0;
 *  this bit configures enable blc average input filter. 0: disable, 1: enable
 */
#define ISP_BLC_FILTER_EN    (BIT(30))
#define ISP_BLC_FILTER_EN_M  (ISP_BLC_FILTER_EN_V << ISP_BLC_FILTER_EN_S)
#define ISP_BLC_FILTER_EN_V  0x00000001U
#define ISP_BLC_FILTER_EN_S  30

/** ISP_BLC_CTRL2_REG register
 *  blc black threshold control register
 */
#define ISP_BLC_CTRL2_REG (DR_REG_ISP_BASE + 0x19c)
/** ISP_BLC_R3_TH : R/W; bitpos: [7:0]; default: 0;
 *  this field configures black threshold when get blc average of bottom right channel
 */
#define ISP_BLC_R3_TH    0x000000FFU
#define ISP_BLC_R3_TH_M  (ISP_BLC_R3_TH_V << ISP_BLC_R3_TH_S)
#define ISP_BLC_R3_TH_V  0x000000FFU
#define ISP_BLC_R3_TH_S  0
/** ISP_BLC_R2_TH : R/W; bitpos: [15:8]; default: 0;
 *  this field configures black threshold when get blc average of bottom left channel
 */
#define ISP_BLC_R2_TH    0x000000FFU
#define ISP_BLC_R2_TH_M  (ISP_BLC_R2_TH_V << ISP_BLC_R2_TH_S)
#define ISP_BLC_R2_TH_V  0x000000FFU
#define ISP_BLC_R2_TH_S  8
/** ISP_BLC_R1_TH : R/W; bitpos: [23:16]; default: 0;
 *  this field configures black threshold when get blc average of top right channel
 */
#define ISP_BLC_R1_TH    0x000000FFU
#define ISP_BLC_R1_TH_M  (ISP_BLC_R1_TH_V << ISP_BLC_R1_TH_S)
#define ISP_BLC_R1_TH_V  0x000000FFU
#define ISP_BLC_R1_TH_S  16
/** ISP_BLC_R0_TH : R/W; bitpos: [31:24]; default: 0;
 *  this field configures black threshold when get blc average of top left channel
 */
#define ISP_BLC_R0_TH    0x000000FFU
#define ISP_BLC_R0_TH_M  (ISP_BLC_R0_TH_V << ISP_BLC_R0_TH_S)
#define ISP_BLC_R0_TH_V  0x000000FFU
#define ISP_BLC_R0_TH_S  24

/** ISP_BLC_MEAN_REG register
 *  results of the average of black window
 */
#define ISP_BLC_MEAN_REG (DR_REG_ISP_BASE + 0x1a0)
/** ISP_BLC_R3_MEAN : RO; bitpos: [7:0]; default: 0;
 *  this field represents the average black value of bottom right channel
 */
#define ISP_BLC_R3_MEAN    0x000000FFU
#define ISP_BLC_R3_MEAN_M  (ISP_BLC_R3_MEAN_V << ISP_BLC_R3_MEAN_S)
#define ISP_BLC_R3_MEAN_V  0x000000FFU
#define ISP_BLC_R3_MEAN_S  0
/** ISP_BLC_R2_MEAN : RO; bitpos: [15:8]; default: 0;
 *  this field represents the average black value of bottom left channel
 */
#define ISP_BLC_R2_MEAN    0x000000FFU
#define ISP_BLC_R2_MEAN_M  (ISP_BLC_R2_MEAN_V << ISP_BLC_R2_MEAN_S)
#define ISP_BLC_R2_MEAN_V  0x000000FFU
#define ISP_BLC_R2_MEAN_S  8
/** ISP_BLC_R1_MEAN : RO; bitpos: [23:16]; default: 0;
 *  this field represents the average black value of top right channel
 */
#define ISP_BLC_R1_MEAN    0x000000FFU
#define ISP_BLC_R1_MEAN_M  (ISP_BLC_R1_MEAN_V << ISP_BLC_R1_MEAN_S)
#define ISP_BLC_R1_MEAN_V  0x000000FFU
#define ISP_BLC_R1_MEAN_S  16
/** ISP_BLC_R0_MEAN : RO; bitpos: [31:24]; default: 0;
 *  this field represents the average black value of top left channel
 */
#define ISP_BLC_R0_MEAN    0x000000FFU
#define ISP_BLC_R0_MEAN_M  (ISP_BLC_R0_MEAN_V << ISP_BLC_R0_MEAN_S)
#define ISP_BLC_R0_MEAN_V  0x000000FFU
#define ISP_BLC_R0_MEAN_S  24

/** ISP_HIST_MODE_REG register
 *  histogram mode control register
 */
#define ISP_HIST_MODE_REG (DR_REG_ISP_BASE + 0x1a4)
/** ISP_HIST_MODE : R/W; bitpos: [2:0]; default: 4;
 *  this field configures statistic mode. 0: RAW_B, 1: RAW_GB, 2: RAW_GR 3: RAW_R, 4:
 *  RGB, 5:YUV_Y, 6:YUV_U, 7:YUV_V
 */
#define ISP_HIST_MODE    0x00000007U
#define ISP_HIST_MODE_M  (ISP_HIST_MODE_V << ISP_HIST_MODE_S)
#define ISP_HIST_MODE_V  0x00000007U
#define ISP_HIST_MODE_S  0

/** ISP_HIST_COEFF_REG register
 *  histogram rgb to gray coefficients register
 */
#define ISP_HIST_COEFF_REG (DR_REG_ISP_BASE + 0x1a8)
/** ISP_HIST_COEFF_B : R/W; bitpos: [7:0]; default: 85;
 *  this field configures coefficient of B when set hist_mode to RGB, sum of coeff_r
 *  and coeff_g and coeff_b should be 256
 */
#define ISP_HIST_COEFF_B    0x000000FFU
#define ISP_HIST_COEFF_B_M  (ISP_HIST_COEFF_B_V << ISP_HIST_COEFF_B_S)
#define ISP_HIST_COEFF_B_V  0x000000FFU
#define ISP_HIST_COEFF_B_S  0
/** ISP_HIST_COEFF_G : R/W; bitpos: [15:8]; default: 85;
 *  this field configures coefficient of G when set hist_mode to RGB, sum of coeff_r
 *  and coeff_g and coeff_b should be 256
 */
#define ISP_HIST_COEFF_G    0x000000FFU
#define ISP_HIST_COEFF_G_M  (ISP_HIST_COEFF_G_V << ISP_HIST_COEFF_G_S)
#define ISP_HIST_COEFF_G_V  0x000000FFU
#define ISP_HIST_COEFF_G_S  8
/** ISP_HIST_COEFF_R : R/W; bitpos: [23:16]; default: 85;
 *  this field configures coefficient of R when set hist_mode to RGB, sum of coeff_r
 *  and coeff_g and coeff_b should be 256
 */
#define ISP_HIST_COEFF_R    0x000000FFU
#define ISP_HIST_COEFF_R_M  (ISP_HIST_COEFF_R_V << ISP_HIST_COEFF_R_S)
#define ISP_HIST_COEFF_R_V  0x000000FFU
#define ISP_HIST_COEFF_R_S  16

/** ISP_HIST_OFFS_REG register
 *  histogram window offsets register
 */
#define ISP_HIST_OFFS_REG (DR_REG_ISP_BASE + 0x1ac)
/** ISP_HIST_Y_OFFS : R/W; bitpos: [11:0]; default: 0;
 *  this field configures y coordinate of first window
 */
#define ISP_HIST_Y_OFFS    0x00000FFFU
#define ISP_HIST_Y_OFFS_M  (ISP_HIST_Y_OFFS_V << ISP_HIST_Y_OFFS_S)
#define ISP_HIST_Y_OFFS_V  0x00000FFFU
#define ISP_HIST_Y_OFFS_S  0
/** ISP_HIST_X_OFFS : R/W; bitpos: [27:16]; default: 0;
 *  this field configures x coordinate of first window
 */
#define ISP_HIST_X_OFFS    0x00000FFFU
#define ISP_HIST_X_OFFS_M  (ISP_HIST_X_OFFS_V << ISP_HIST_X_OFFS_S)
#define ISP_HIST_X_OFFS_V  0x00000FFFU
#define ISP_HIST_X_OFFS_S  16

/** ISP_HIST_SIZE_REG register
 *  histogram sub-window size register
 */
#define ISP_HIST_SIZE_REG (DR_REG_ISP_BASE + 0x1b0)
/** ISP_HIST_Y_SIZE : R/W; bitpos: [8:0]; default: 32;
 *  this field configures y direction size of subwindow
 */
#define ISP_HIST_Y_SIZE    0x000001FFU
#define ISP_HIST_Y_SIZE_M  (ISP_HIST_Y_SIZE_V << ISP_HIST_Y_SIZE_S)
#define ISP_HIST_Y_SIZE_V  0x000001FFU
#define ISP_HIST_Y_SIZE_S  0
/** ISP_HIST_X_SIZE : R/W; bitpos: [24:16]; default: 18;
 *  this field configures x direction size of subwindow
 */
#define ISP_HIST_X_SIZE    0x000001FFU
#define ISP_HIST_X_SIZE_M  (ISP_HIST_X_SIZE_V << ISP_HIST_X_SIZE_S)
#define ISP_HIST_X_SIZE_V  0x000001FFU
#define ISP_HIST_X_SIZE_S  16

/** ISP_HIST_SEG0_REG register
 *  histogram bin control register 0
 */
#define ISP_HIST_SEG0_REG (DR_REG_ISP_BASE + 0x1b4)
/** ISP_HIST_SEG_3_4 : R/W; bitpos: [7:0]; default: 64;
 *  this field configures threshold of histogram bin 3 and bin 4
 */
#define ISP_HIST_SEG_3_4    0x000000FFU
#define ISP_HIST_SEG_3_4_M  (ISP_HIST_SEG_3_4_V << ISP_HIST_SEG_3_4_S)
#define ISP_HIST_SEG_3_4_V  0x000000FFU
#define ISP_HIST_SEG_3_4_S  0
/** ISP_HIST_SEG_2_3 : R/W; bitpos: [15:8]; default: 48;
 *  this field configures threshold of histogram bin 2 and bin 3
 */
#define ISP_HIST_SEG_2_3    0x000000FFU
#define ISP_HIST_SEG_2_3_M  (ISP_HIST_SEG_2_3_V << ISP_HIST_SEG_2_3_S)
#define ISP_HIST_SEG_2_3_V  0x000000FFU
#define ISP_HIST_SEG_2_3_S  8
/** ISP_HIST_SEG_1_2 : R/W; bitpos: [23:16]; default: 32;
 *  this field configures threshold of histogram bin 1 and bin 2
 */
#define ISP_HIST_SEG_1_2    0x000000FFU
#define ISP_HIST_SEG_1_2_M  (ISP_HIST_SEG_1_2_V << ISP_HIST_SEG_1_2_S)
#define ISP_HIST_SEG_1_2_V  0x000000FFU
#define ISP_HIST_SEG_1_2_S  16
/** ISP_HIST_SEG_0_1 : R/W; bitpos: [31:24]; default: 16;
 *  this field configures threshold of histogram bin 0 and bin 1
 */
#define ISP_HIST_SEG_0_1    0x000000FFU
#define ISP_HIST_SEG_0_1_M  (ISP_HIST_SEG_0_1_V << ISP_HIST_SEG_0_1_S)
#define ISP_HIST_SEG_0_1_V  0x000000FFU
#define ISP_HIST_SEG_0_1_S  24

/** ISP_HIST_SEG1_REG register
 *  histogram bin control register 1
 */
#define ISP_HIST_SEG1_REG (DR_REG_ISP_BASE + 0x1b8)
/** ISP_HIST_SEG_7_8 : R/W; bitpos: [7:0]; default: 128;
 *  this field configures threshold of histogram bin 7 and bin 8
 */
#define ISP_HIST_SEG_7_8    0x000000FFU
#define ISP_HIST_SEG_7_8_M  (ISP_HIST_SEG_7_8_V << ISP_HIST_SEG_7_8_S)
#define ISP_HIST_SEG_7_8_V  0x000000FFU
#define ISP_HIST_SEG_7_8_S  0
/** ISP_HIST_SEG_6_7 : R/W; bitpos: [15:8]; default: 112;
 *  this field configures threshold of histogram bin 6 and bin 7
 */
#define ISP_HIST_SEG_6_7    0x000000FFU
#define ISP_HIST_SEG_6_7_M  (ISP_HIST_SEG_6_7_V << ISP_HIST_SEG_6_7_S)
#define ISP_HIST_SEG_6_7_V  0x000000FFU
#define ISP_HIST_SEG_6_7_S  8
/** ISP_HIST_SEG_5_6 : R/W; bitpos: [23:16]; default: 96;
 *  this field configures threshold of histogram bin 5 and bin 6
 */
#define ISP_HIST_SEG_5_6    0x000000FFU
#define ISP_HIST_SEG_5_6_M  (ISP_HIST_SEG_5_6_V << ISP_HIST_SEG_5_6_S)
#define ISP_HIST_SEG_5_6_V  0x000000FFU
#define ISP_HIST_SEG_5_6_S  16
/** ISP_HIST_SEG_4_5 : R/W; bitpos: [31:24]; default: 80;
 *  this field configures threshold of histogram bin 4 and bin 5
 */
#define ISP_HIST_SEG_4_5    0x000000FFU
#define ISP_HIST_SEG_4_5_M  (ISP_HIST_SEG_4_5_V << ISP_HIST_SEG_4_5_S)
#define ISP_HIST_SEG_4_5_V  0x000000FFU
#define ISP_HIST_SEG_4_5_S  24

/** ISP_HIST_SEG2_REG register
 *  histogram bin control register 2
 */
#define ISP_HIST_SEG2_REG (DR_REG_ISP_BASE + 0x1bc)
/** ISP_HIST_SEG_11_12 : R/W; bitpos: [7:0]; default: 192;
 *  this field configures threshold of histogram bin 11 and bin 12
 */
#define ISP_HIST_SEG_11_12    0x000000FFU
#define ISP_HIST_SEG_11_12_M  (ISP_HIST_SEG_11_12_V << ISP_HIST_SEG_11_12_S)
#define ISP_HIST_SEG_11_12_V  0x000000FFU
#define ISP_HIST_SEG_11_12_S  0
/** ISP_HIST_SEG_10_11 : R/W; bitpos: [15:8]; default: 176;
 *  this field configures threshold of histogram bin 10 and bin 11
 */
#define ISP_HIST_SEG_10_11    0x000000FFU
#define ISP_HIST_SEG_10_11_M  (ISP_HIST_SEG_10_11_V << ISP_HIST_SEG_10_11_S)
#define ISP_HIST_SEG_10_11_V  0x000000FFU
#define ISP_HIST_SEG_10_11_S  8
/** ISP_HIST_SEG_9_10 : R/W; bitpos: [23:16]; default: 160;
 *  this field configures threshold of histogram bin 9 and bin 10
 */
#define ISP_HIST_SEG_9_10    0x000000FFU
#define ISP_HIST_SEG_9_10_M  (ISP_HIST_SEG_9_10_V << ISP_HIST_SEG_9_10_S)
#define ISP_HIST_SEG_9_10_V  0x000000FFU
#define ISP_HIST_SEG_9_10_S  16
/** ISP_HIST_SEG_8_9 : R/W; bitpos: [31:24]; default: 144;
 *  this field configures threshold of histogram bin 8 and bin 9
 */
#define ISP_HIST_SEG_8_9    0x000000FFU
#define ISP_HIST_SEG_8_9_M  (ISP_HIST_SEG_8_9_V << ISP_HIST_SEG_8_9_S)
#define ISP_HIST_SEG_8_9_V  0x000000FFU
#define ISP_HIST_SEG_8_9_S  24

/** ISP_HIST_SEG3_REG register
 *  histogram bin control register 3
 */
#define ISP_HIST_SEG3_REG (DR_REG_ISP_BASE + 0x1c0)
/** ISP_HIST_SEG_14_15 : R/W; bitpos: [7:0]; default: 240;
 *  this field configures threshold of histogram bin 14 and bin 15
 */
#define ISP_HIST_SEG_14_15    0x000000FFU
#define ISP_HIST_SEG_14_15_M  (ISP_HIST_SEG_14_15_V << ISP_HIST_SEG_14_15_S)
#define ISP_HIST_SEG_14_15_V  0x000000FFU
#define ISP_HIST_SEG_14_15_S  0
/** ISP_HIST_SEG_13_14 : R/W; bitpos: [15:8]; default: 224;
 *  this field configures threshold of histogram bin 13 and bin 14
 */
#define ISP_HIST_SEG_13_14    0x000000FFU
#define ISP_HIST_SEG_13_14_M  (ISP_HIST_SEG_13_14_V << ISP_HIST_SEG_13_14_S)
#define ISP_HIST_SEG_13_14_V  0x000000FFU
#define ISP_HIST_SEG_13_14_S  8
/** ISP_HIST_SEG_12_13 : R/W; bitpos: [23:16]; default: 208;
 *  this field configures threshold of histogram bin 12 and bin 13
 */
#define ISP_HIST_SEG_12_13    0x000000FFU
#define ISP_HIST_SEG_12_13_M  (ISP_HIST_SEG_12_13_V << ISP_HIST_SEG_12_13_S)
#define ISP_HIST_SEG_12_13_V  0x000000FFU
#define ISP_HIST_SEG_12_13_S  16

/** ISP_HIST_WEIGHT0_REG register
 *  histogram sub-window weight register 0
 */
#define ISP_HIST_WEIGHT0_REG (DR_REG_ISP_BASE + 0x1c4)
/** ISP_HIST_WEIGHT_03 : R/W; bitpos: [7:0]; default: 1;
 *  this field configures weight of subwindow 03
 */
#define ISP_HIST_WEIGHT_03    0x000000FFU
#define ISP_HIST_WEIGHT_03_M  (ISP_HIST_WEIGHT_03_V << ISP_HIST_WEIGHT_03_S)
#define ISP_HIST_WEIGHT_03_V  0x000000FFU
#define ISP_HIST_WEIGHT_03_S  0
/** ISP_HIST_WEIGHT_02 : R/W; bitpos: [15:8]; default: 1;
 *  this field configures weight of subwindow 02
 */
#define ISP_HIST_WEIGHT_02    0x000000FFU
#define ISP_HIST_WEIGHT_02_M  (ISP_HIST_WEIGHT_02_V << ISP_HIST_WEIGHT_02_S)
#define ISP_HIST_WEIGHT_02_V  0x000000FFU
#define ISP_HIST_WEIGHT_02_S  8
/** ISP_HIST_WEIGHT_01 : R/W; bitpos: [23:16]; default: 1;
 *  this field configures weight of subwindow 01
 */
#define ISP_HIST_WEIGHT_01    0x000000FFU
#define ISP_HIST_WEIGHT_01_M  (ISP_HIST_WEIGHT_01_V << ISP_HIST_WEIGHT_01_S)
#define ISP_HIST_WEIGHT_01_V  0x000000FFU
#define ISP_HIST_WEIGHT_01_S  16
/** ISP_HIST_WEIGHT_00 : R/W; bitpos: [31:24]; default: 1;
 *  this field configures weight of subwindow 00 and sum of all weight should be 256
 */
#define ISP_HIST_WEIGHT_00    0x000000FFU
#define ISP_HIST_WEIGHT_00_M  (ISP_HIST_WEIGHT_00_V << ISP_HIST_WEIGHT_00_S)
#define ISP_HIST_WEIGHT_00_V  0x000000FFU
#define ISP_HIST_WEIGHT_00_S  24

/** ISP_HIST_WEIGHT1_REG register
 *  histogram sub-window weight register 1
 */
#define ISP_HIST_WEIGHT1_REG (DR_REG_ISP_BASE + 0x1c8)
/** ISP_HIST_WEIGHT_12 : R/W; bitpos: [7:0]; default: 1;
 *  this field configures weight of subwindow 12
 */
#define ISP_HIST_WEIGHT_12    0x000000FFU
#define ISP_HIST_WEIGHT_12_M  (ISP_HIST_WEIGHT_12_V << ISP_HIST_WEIGHT_12_S)
#define ISP_HIST_WEIGHT_12_V  0x000000FFU
#define ISP_HIST_WEIGHT_12_S  0
/** ISP_HIST_WEIGHT_11 : R/W; bitpos: [15:8]; default: 1;
 *  this field configures weight of subwindow 11
 */
#define ISP_HIST_WEIGHT_11    0x000000FFU
#define ISP_HIST_WEIGHT_11_M  (ISP_HIST_WEIGHT_11_V << ISP_HIST_WEIGHT_11_S)
#define ISP_HIST_WEIGHT_11_V  0x000000FFU
#define ISP_HIST_WEIGHT_11_S  8
/** ISP_HIST_WEIGHT_10 : R/W; bitpos: [23:16]; default: 1;
 *  this field configures weight of subwindow 10
 */
#define ISP_HIST_WEIGHT_10    0x000000FFU
#define ISP_HIST_WEIGHT_10_M  (ISP_HIST_WEIGHT_10_V << ISP_HIST_WEIGHT_10_S)
#define ISP_HIST_WEIGHT_10_V  0x000000FFU
#define ISP_HIST_WEIGHT_10_S  16
/** ISP_HIST_WEIGHT_04 : R/W; bitpos: [31:24]; default: 1;
 *  this field configures weight of subwindow 04
 */
#define ISP_HIST_WEIGHT_04    0x000000FFU
#define ISP_HIST_WEIGHT_04_M  (ISP_HIST_WEIGHT_04_V << ISP_HIST_WEIGHT_04_S)
#define ISP_HIST_WEIGHT_04_V  0x000000FFU
#define ISP_HIST_WEIGHT_04_S  24

/** ISP_HIST_WEIGHT2_REG register
 *  histogram sub-window weight register 2
 */
#define ISP_HIST_WEIGHT2_REG (DR_REG_ISP_BASE + 0x1cc)
/** ISP_HIST_WEIGHT_21 : R/W; bitpos: [7:0]; default: 1;
 *  this field configures weight of subwindow 21
 */
#define ISP_HIST_WEIGHT_21    0x000000FFU
#define ISP_HIST_WEIGHT_21_M  (ISP_HIST_WEIGHT_21_V << ISP_HIST_WEIGHT_21_S)
#define ISP_HIST_WEIGHT_21_V  0x000000FFU
#define ISP_HIST_WEIGHT_21_S  0
/** ISP_HIST_WEIGHT_20 : R/W; bitpos: [15:8]; default: 1;
 *  this field configures weight of subwindow 20
 */
#define ISP_HIST_WEIGHT_20    0x000000FFU
#define ISP_HIST_WEIGHT_20_M  (ISP_HIST_WEIGHT_20_V << ISP_HIST_WEIGHT_20_S)
#define ISP_HIST_WEIGHT_20_V  0x000000FFU
#define ISP_HIST_WEIGHT_20_S  8
/** ISP_HIST_WEIGHT_14 : R/W; bitpos: [23:16]; default: 1;
 *  this field configures weight of subwindow 04
 */
#define ISP_HIST_WEIGHT_14    0x000000FFU
#define ISP_HIST_WEIGHT_14_M  (ISP_HIST_WEIGHT_14_V << ISP_HIST_WEIGHT_14_S)
#define ISP_HIST_WEIGHT_14_V  0x000000FFU
#define ISP_HIST_WEIGHT_14_S  16
/** ISP_HIST_WEIGHT_13 : R/W; bitpos: [31:24]; default: 1;
 *  this field configures weight of subwindow 13
 */
#define ISP_HIST_WEIGHT_13    0x000000FFU
#define ISP_HIST_WEIGHT_13_M  (ISP_HIST_WEIGHT_13_V << ISP_HIST_WEIGHT_13_S)
#define ISP_HIST_WEIGHT_13_V  0x000000FFU
#define ISP_HIST_WEIGHT_13_S  24

/** ISP_HIST_WEIGHT3_REG register
 *  histogram sub-window weight register 3
 */
#define ISP_HIST_WEIGHT3_REG (DR_REG_ISP_BASE + 0x1d0)
/** ISP_HIST_WEIGHT_30 : R/W; bitpos: [7:0]; default: 1;
 *  this field configures weight of subwindow 30
 */
#define ISP_HIST_WEIGHT_30    0x000000FFU
#define ISP_HIST_WEIGHT_30_M  (ISP_HIST_WEIGHT_30_V << ISP_HIST_WEIGHT_30_S)
#define ISP_HIST_WEIGHT_30_V  0x000000FFU
#define ISP_HIST_WEIGHT_30_S  0
/** ISP_HIST_WEIGHT_24 : R/W; bitpos: [15:8]; default: 1;
 *  this field configures weight of subwindow 24
 */
#define ISP_HIST_WEIGHT_24    0x000000FFU
#define ISP_HIST_WEIGHT_24_M  (ISP_HIST_WEIGHT_24_V << ISP_HIST_WEIGHT_24_S)
#define ISP_HIST_WEIGHT_24_V  0x000000FFU
#define ISP_HIST_WEIGHT_24_S  8
/** ISP_HIST_WEIGHT_23 : R/W; bitpos: [23:16]; default: 1;
 *  this field configures weight of subwindow 23
 */
#define ISP_HIST_WEIGHT_23    0x000000FFU
#define ISP_HIST_WEIGHT_23_M  (ISP_HIST_WEIGHT_23_V << ISP_HIST_WEIGHT_23_S)
#define ISP_HIST_WEIGHT_23_V  0x000000FFU
#define ISP_HIST_WEIGHT_23_S  16
/** ISP_HIST_WEIGHT_22 : R/W; bitpos: [31:24]; default: 232;
 *  this field configures weight of subwindow 22
 */
#define ISP_HIST_WEIGHT_22    0x000000FFU
#define ISP_HIST_WEIGHT_22_M  (ISP_HIST_WEIGHT_22_V << ISP_HIST_WEIGHT_22_S)
#define ISP_HIST_WEIGHT_22_V  0x000000FFU
#define ISP_HIST_WEIGHT_22_S  24

/** ISP_HIST_WEIGHT4_REG register
 *  histogram sub-window weight register 4
 */
#define ISP_HIST_WEIGHT4_REG (DR_REG_ISP_BASE + 0x1d4)
/** ISP_HIST_WEIGHT_34 : R/W; bitpos: [7:0]; default: 1;
 *  this field configures weight of subwindow 34
 */
#define ISP_HIST_WEIGHT_34    0x000000FFU
#define ISP_HIST_WEIGHT_34_M  (ISP_HIST_WEIGHT_34_V << ISP_HIST_WEIGHT_34_S)
#define ISP_HIST_WEIGHT_34_V  0x000000FFU
#define ISP_HIST_WEIGHT_34_S  0
/** ISP_HIST_WEIGHT_33 : R/W; bitpos: [15:8]; default: 1;
 *  this field configures weight of subwindow 33
 */
#define ISP_HIST_WEIGHT_33    0x000000FFU
#define ISP_HIST_WEIGHT_33_M  (ISP_HIST_WEIGHT_33_V << ISP_HIST_WEIGHT_33_S)
#define ISP_HIST_WEIGHT_33_V  0x000000FFU
#define ISP_HIST_WEIGHT_33_S  8
/** ISP_HIST_WEIGHT_32 : R/W; bitpos: [23:16]; default: 1;
 *  this field configures weight of subwindow 32
 */
#define ISP_HIST_WEIGHT_32    0x000000FFU
#define ISP_HIST_WEIGHT_32_M  (ISP_HIST_WEIGHT_32_V << ISP_HIST_WEIGHT_32_S)
#define ISP_HIST_WEIGHT_32_V  0x000000FFU
#define ISP_HIST_WEIGHT_32_S  16
/** ISP_HIST_WEIGHT_31 : R/W; bitpos: [31:24]; default: 1;
 *  this field configures weight of subwindow 31
 */
#define ISP_HIST_WEIGHT_31    0x000000FFU
#define ISP_HIST_WEIGHT_31_M  (ISP_HIST_WEIGHT_31_V << ISP_HIST_WEIGHT_31_S)
#define ISP_HIST_WEIGHT_31_V  0x000000FFU
#define ISP_HIST_WEIGHT_31_S  24

/** ISP_HIST_WEIGHT5_REG register
 *  histogram sub-window weight register 5
 */
#define ISP_HIST_WEIGHT5_REG (DR_REG_ISP_BASE + 0x1d8)
/** ISP_HIST_WEIGHT_43 : R/W; bitpos: [7:0]; default: 1;
 *  this field configures weight of subwindow 43
 */
#define ISP_HIST_WEIGHT_43    0x000000FFU
#define ISP_HIST_WEIGHT_43_M  (ISP_HIST_WEIGHT_43_V << ISP_HIST_WEIGHT_43_S)
#define ISP_HIST_WEIGHT_43_V  0x000000FFU
#define ISP_HIST_WEIGHT_43_S  0
/** ISP_HIST_WEIGHT_42 : R/W; bitpos: [15:8]; default: 1;
 *  this field configures weight of subwindow 42
 */
#define ISP_HIST_WEIGHT_42    0x000000FFU
#define ISP_HIST_WEIGHT_42_M  (ISP_HIST_WEIGHT_42_V << ISP_HIST_WEIGHT_42_S)
#define ISP_HIST_WEIGHT_42_V  0x000000FFU
#define ISP_HIST_WEIGHT_42_S  8
/** ISP_HIST_WEIGHT_41 : R/W; bitpos: [23:16]; default: 1;
 *  this field configures weight of subwindow 41
 */
#define ISP_HIST_WEIGHT_41    0x000000FFU
#define ISP_HIST_WEIGHT_41_M  (ISP_HIST_WEIGHT_41_V << ISP_HIST_WEIGHT_41_S)
#define ISP_HIST_WEIGHT_41_V  0x000000FFU
#define ISP_HIST_WEIGHT_41_S  16
/** ISP_HIST_WEIGHT_40 : R/W; bitpos: [31:24]; default: 1;
 *  this field configures weight of subwindow 40
 */
#define ISP_HIST_WEIGHT_40    0x000000FFU
#define ISP_HIST_WEIGHT_40_M  (ISP_HIST_WEIGHT_40_V << ISP_HIST_WEIGHT_40_S)
#define ISP_HIST_WEIGHT_40_V  0x000000FFU
#define ISP_HIST_WEIGHT_40_S  24

/** ISP_HIST_WEIGHT6_REG register
 *  histogram sub-window weight register 6
 */
#define ISP_HIST_WEIGHT6_REG (DR_REG_ISP_BASE + 0x1dc)
/** ISP_HIST_WEIGHT_44 : R/W; bitpos: [7:0]; default: 1;
 *  this field configures weight of subwindow 44
 */
#define ISP_HIST_WEIGHT_44    0x000000FFU
#define ISP_HIST_WEIGHT_44_M  (ISP_HIST_WEIGHT_44_V << ISP_HIST_WEIGHT_44_S)
#define ISP_HIST_WEIGHT_44_V  0x000000FFU
#define ISP_HIST_WEIGHT_44_S  0

/** ISP_HIST_BIN0_REG register
 *  result of histogram bin 0
 */
#define ISP_HIST_BIN0_REG (DR_REG_ISP_BASE + 0x1e0)
/** ISP_HIST_BIN_0 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 0
 */
#define ISP_HIST_BIN_0    0x0001FFFFU
#define ISP_HIST_BIN_0_M  (ISP_HIST_BIN_0_V << ISP_HIST_BIN_0_S)
#define ISP_HIST_BIN_0_V  0x0001FFFFU
#define ISP_HIST_BIN_0_S  0

/** ISP_HIST_BIN1_REG register
 *  result of histogram bin 1
 */
#define ISP_HIST_BIN1_REG (DR_REG_ISP_BASE + 0x1e4)
/** ISP_HIST_BIN_1 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 1
 */
#define ISP_HIST_BIN_1    0x0001FFFFU
#define ISP_HIST_BIN_1_M  (ISP_HIST_BIN_1_V << ISP_HIST_BIN_1_S)
#define ISP_HIST_BIN_1_V  0x0001FFFFU
#define ISP_HIST_BIN_1_S  0

/** ISP_HIST_BIN2_REG register
 *  result of histogram bin 2
 */
#define ISP_HIST_BIN2_REG (DR_REG_ISP_BASE + 0x1e8)
/** ISP_HIST_BIN_2 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 2
 */
#define ISP_HIST_BIN_2    0x0001FFFFU
#define ISP_HIST_BIN_2_M  (ISP_HIST_BIN_2_V << ISP_HIST_BIN_2_S)
#define ISP_HIST_BIN_2_V  0x0001FFFFU
#define ISP_HIST_BIN_2_S  0

/** ISP_HIST_BIN3_REG register
 *  result of histogram bin 3
 */
#define ISP_HIST_BIN3_REG (DR_REG_ISP_BASE + 0x1ec)
/** ISP_HIST_BIN_3 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 3
 */
#define ISP_HIST_BIN_3    0x0001FFFFU
#define ISP_HIST_BIN_3_M  (ISP_HIST_BIN_3_V << ISP_HIST_BIN_3_S)
#define ISP_HIST_BIN_3_V  0x0001FFFFU
#define ISP_HIST_BIN_3_S  0

/** ISP_HIST_BIN4_REG register
 *  result of histogram bin 4
 */
#define ISP_HIST_BIN4_REG (DR_REG_ISP_BASE + 0x1f0)
/** ISP_HIST_BIN_4 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 4
 */
#define ISP_HIST_BIN_4    0x0001FFFFU
#define ISP_HIST_BIN_4_M  (ISP_HIST_BIN_4_V << ISP_HIST_BIN_4_S)
#define ISP_HIST_BIN_4_V  0x0001FFFFU
#define ISP_HIST_BIN_4_S  0

/** ISP_HIST_BIN5_REG register
 *  result of histogram bin 5
 */
#define ISP_HIST_BIN5_REG (DR_REG_ISP_BASE + 0x1f4)
/** ISP_HIST_BIN_5 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 5
 */
#define ISP_HIST_BIN_5    0x0001FFFFU
#define ISP_HIST_BIN_5_M  (ISP_HIST_BIN_5_V << ISP_HIST_BIN_5_S)
#define ISP_HIST_BIN_5_V  0x0001FFFFU
#define ISP_HIST_BIN_5_S  0

/** ISP_HIST_BIN6_REG register
 *  result of histogram bin 6
 */
#define ISP_HIST_BIN6_REG (DR_REG_ISP_BASE + 0x1f8)
/** ISP_HIST_BIN_6 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 6
 */
#define ISP_HIST_BIN_6    0x0001FFFFU
#define ISP_HIST_BIN_6_M  (ISP_HIST_BIN_6_V << ISP_HIST_BIN_6_S)
#define ISP_HIST_BIN_6_V  0x0001FFFFU
#define ISP_HIST_BIN_6_S  0

/** ISP_HIST_BIN7_REG register
 *  result of histogram bin 7
 */
#define ISP_HIST_BIN7_REG (DR_REG_ISP_BASE + 0x1fc)
/** ISP_HIST_BIN_7 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 7
 */
#define ISP_HIST_BIN_7    0x0001FFFFU
#define ISP_HIST_BIN_7_M  (ISP_HIST_BIN_7_V << ISP_HIST_BIN_7_S)
#define ISP_HIST_BIN_7_V  0x0001FFFFU
#define ISP_HIST_BIN_7_S  0

/** ISP_HIST_BIN8_REG register
 *  result of histogram bin 8
 */
#define ISP_HIST_BIN8_REG (DR_REG_ISP_BASE + 0x200)
/** ISP_HIST_BIN_8 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 8
 */
#define ISP_HIST_BIN_8    0x0001FFFFU
#define ISP_HIST_BIN_8_M  (ISP_HIST_BIN_8_V << ISP_HIST_BIN_8_S)
#define ISP_HIST_BIN_8_V  0x0001FFFFU
#define ISP_HIST_BIN_8_S  0

/** ISP_HIST_BIN9_REG register
 *  result of histogram bin 9
 */
#define ISP_HIST_BIN9_REG (DR_REG_ISP_BASE + 0x204)
/** ISP_HIST_BIN_9 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 9
 */
#define ISP_HIST_BIN_9    0x0001FFFFU
#define ISP_HIST_BIN_9_M  (ISP_HIST_BIN_9_V << ISP_HIST_BIN_9_S)
#define ISP_HIST_BIN_9_V  0x0001FFFFU
#define ISP_HIST_BIN_9_S  0

/** ISP_HIST_BIN10_REG register
 *  result of histogram bin 10
 */
#define ISP_HIST_BIN10_REG (DR_REG_ISP_BASE + 0x208)
/** ISP_HIST_BIN_10 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 10
 */
#define ISP_HIST_BIN_10    0x0001FFFFU
#define ISP_HIST_BIN_10_M  (ISP_HIST_BIN_10_V << ISP_HIST_BIN_10_S)
#define ISP_HIST_BIN_10_V  0x0001FFFFU
#define ISP_HIST_BIN_10_S  0

/** ISP_HIST_BIN11_REG register
 *  result of histogram bin 11
 */
#define ISP_HIST_BIN11_REG (DR_REG_ISP_BASE + 0x20c)
/** ISP_HIST_BIN_11 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 11
 */
#define ISP_HIST_BIN_11    0x0001FFFFU
#define ISP_HIST_BIN_11_M  (ISP_HIST_BIN_11_V << ISP_HIST_BIN_11_S)
#define ISP_HIST_BIN_11_V  0x0001FFFFU
#define ISP_HIST_BIN_11_S  0

/** ISP_HIST_BIN12_REG register
 *  result of histogram bin 12
 */
#define ISP_HIST_BIN12_REG (DR_REG_ISP_BASE + 0x210)
/** ISP_HIST_BIN_12 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 12
 */
#define ISP_HIST_BIN_12    0x0001FFFFU
#define ISP_HIST_BIN_12_M  (ISP_HIST_BIN_12_V << ISP_HIST_BIN_12_S)
#define ISP_HIST_BIN_12_V  0x0001FFFFU
#define ISP_HIST_BIN_12_S  0

/** ISP_HIST_BIN13_REG register
 *  result of histogram bin 13
 */
#define ISP_HIST_BIN13_REG (DR_REG_ISP_BASE + 0x214)
/** ISP_HIST_BIN_13 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 13
 */
#define ISP_HIST_BIN_13    0x0001FFFFU
#define ISP_HIST_BIN_13_M  (ISP_HIST_BIN_13_V << ISP_HIST_BIN_13_S)
#define ISP_HIST_BIN_13_V  0x0001FFFFU
#define ISP_HIST_BIN_13_S  0

/** ISP_HIST_BIN14_REG register
 *  result of histogram bin 14
 */
#define ISP_HIST_BIN14_REG (DR_REG_ISP_BASE + 0x218)
/** ISP_HIST_BIN_14 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 14
 */
#define ISP_HIST_BIN_14    0x0001FFFFU
#define ISP_HIST_BIN_14_M  (ISP_HIST_BIN_14_V << ISP_HIST_BIN_14_S)
#define ISP_HIST_BIN_14_V  0x0001FFFFU
#define ISP_HIST_BIN_14_S  0

/** ISP_HIST_BIN15_REG register
 *  result of histogram bin 15
 */
#define ISP_HIST_BIN15_REG (DR_REG_ISP_BASE + 0x21c)
/** ISP_HIST_BIN_15 : RO; bitpos: [16:0]; default: 0;
 *  this field represents result of histogram bin 15
 */
#define ISP_HIST_BIN_15    0x0001FFFFU
#define ISP_HIST_BIN_15_M  (ISP_HIST_BIN_15_V << ISP_HIST_BIN_15_S)
#define ISP_HIST_BIN_15_V  0x0001FFFFU
#define ISP_HIST_BIN_15_S  0

/** ISP_MEM_AUX_CTRL_0_REG register
 *  mem aux control register 0
 */
#define ISP_MEM_AUX_CTRL_0_REG (DR_REG_ISP_BASE + 0x220)
/** ISP_HEADER_MEM_AUX_CTRL : R/W; bitpos: [13:0]; default: 4896;
 *  this field configures the mem_aux of isp input buffer memory
 */
#define ISP_HEADER_MEM_AUX_CTRL    0x00003FFFU
#define ISP_HEADER_MEM_AUX_CTRL_M  (ISP_HEADER_MEM_AUX_CTRL_V << ISP_HEADER_MEM_AUX_CTRL_S)
#define ISP_HEADER_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_HEADER_MEM_AUX_CTRL_S  0
/** ISP_DPC_LUT_MEM_AUX_CTRL : R/W; bitpos: [29:16]; default: 4896;
 *  this field represents this field configures the mem_aux of dpc lut memory
 */
#define ISP_DPC_LUT_MEM_AUX_CTRL    0x00003FFFU
#define ISP_DPC_LUT_MEM_AUX_CTRL_M  (ISP_DPC_LUT_MEM_AUX_CTRL_V << ISP_DPC_LUT_MEM_AUX_CTRL_S)
#define ISP_DPC_LUT_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_DPC_LUT_MEM_AUX_CTRL_S  16

/** ISP_MEM_AUX_CTRL_1_REG register
 *  mem aux control register 1
 */
#define ISP_MEM_AUX_CTRL_1_REG (DR_REG_ISP_BASE + 0x224)
/** ISP_LSC_LUT_R_GR_MEM_AUX_CTRL : R/W; bitpos: [13:0]; default: 4896;
 *  this field configures the mem_aux of lsc r gr lut memory
 */
#define ISP_LSC_LUT_R_GR_MEM_AUX_CTRL    0x00003FFFU
#define ISP_LSC_LUT_R_GR_MEM_AUX_CTRL_M  (ISP_LSC_LUT_R_GR_MEM_AUX_CTRL_V << ISP_LSC_LUT_R_GR_MEM_AUX_CTRL_S)
#define ISP_LSC_LUT_R_GR_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_LSC_LUT_R_GR_MEM_AUX_CTRL_S  0
/** ISP_LSC_LUT_GB_B_MEM_AUX_CTRL : R/W; bitpos: [29:16]; default: 4896;
 *  this field configures the mem_aux of lsc gb b lut memory
 */
#define ISP_LSC_LUT_GB_B_MEM_AUX_CTRL    0x00003FFFU
#define ISP_LSC_LUT_GB_B_MEM_AUX_CTRL_M  (ISP_LSC_LUT_GB_B_MEM_AUX_CTRL_V << ISP_LSC_LUT_GB_B_MEM_AUX_CTRL_S)
#define ISP_LSC_LUT_GB_B_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_LSC_LUT_GB_B_MEM_AUX_CTRL_S  16

/** ISP_MEM_AUX_CTRL_2_REG register
 *  mem aux control register 2
 */
#define ISP_MEM_AUX_CTRL_2_REG (DR_REG_ISP_BASE + 0x228)
/** ISP_BF_MATRIX_MEM_AUX_CTRL : R/W; bitpos: [13:0]; default: 4896;
 *  this field configures the mem_aux of bf line buffer memory
 */
#define ISP_BF_MATRIX_MEM_AUX_CTRL    0x00003FFFU
#define ISP_BF_MATRIX_MEM_AUX_CTRL_M  (ISP_BF_MATRIX_MEM_AUX_CTRL_V << ISP_BF_MATRIX_MEM_AUX_CTRL_S)
#define ISP_BF_MATRIX_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_BF_MATRIX_MEM_AUX_CTRL_S  0
/** ISP_DPC_MATRIX_MEM_AUX_CTRL : R/W; bitpos: [29:16]; default: 4896;
 *  this field configures the mem_aux of dpc line buffer memory
 */
#define ISP_DPC_MATRIX_MEM_AUX_CTRL    0x00003FFFU
#define ISP_DPC_MATRIX_MEM_AUX_CTRL_M  (ISP_DPC_MATRIX_MEM_AUX_CTRL_V << ISP_DPC_MATRIX_MEM_AUX_CTRL_S)
#define ISP_DPC_MATRIX_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_DPC_MATRIX_MEM_AUX_CTRL_S  16

/** ISP_MEM_AUX_CTRL_3_REG register
 *  mem aux control register 3
 */
#define ISP_MEM_AUX_CTRL_3_REG (DR_REG_ISP_BASE + 0x22c)
/** ISP_SHARP_MATRIX_Y_MEM_AUX_CTRL : R/W; bitpos: [13:0]; default: 4896;
 *  this field configures the mem_aux of sharp y line buffer memory
 */
#define ISP_SHARP_MATRIX_Y_MEM_AUX_CTRL    0x00003FFFU
#define ISP_SHARP_MATRIX_Y_MEM_AUX_CTRL_M  (ISP_SHARP_MATRIX_Y_MEM_AUX_CTRL_V << ISP_SHARP_MATRIX_Y_MEM_AUX_CTRL_S)
#define ISP_SHARP_MATRIX_Y_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_SHARP_MATRIX_Y_MEM_AUX_CTRL_S  0
/** ISP_DEMOSAIC_MATRIX_MEM_AUX_CTRL : R/W; bitpos: [29:16]; default: 4896;
 *  this field configures the mem_aux of demosaic line buffer memory
 */
#define ISP_DEMOSAIC_MATRIX_MEM_AUX_CTRL    0x00003FFFU
#define ISP_DEMOSAIC_MATRIX_MEM_AUX_CTRL_M  (ISP_DEMOSAIC_MATRIX_MEM_AUX_CTRL_V << ISP_DEMOSAIC_MATRIX_MEM_AUX_CTRL_S)
#define ISP_DEMOSAIC_MATRIX_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_DEMOSAIC_MATRIX_MEM_AUX_CTRL_S  16

/** ISP_MEM_AUX_CTRL_4_REG register
 *  mem aux control register 4
 */
#define ISP_MEM_AUX_CTRL_4_REG (DR_REG_ISP_BASE + 0x230)
/** ISP_SHARP_MATRIX_UV_MEM_AUX_CTRL : R/W; bitpos: [13:0]; default: 4896;
 *  this field configures the mem_aux of sharp uv line buffer memory
 */
#define ISP_SHARP_MATRIX_UV_MEM_AUX_CTRL    0x00003FFFU
#define ISP_SHARP_MATRIX_UV_MEM_AUX_CTRL_M  (ISP_SHARP_MATRIX_UV_MEM_AUX_CTRL_V << ISP_SHARP_MATRIX_UV_MEM_AUX_CTRL_S)
#define ISP_SHARP_MATRIX_UV_MEM_AUX_CTRL_V  0x00003FFFU
#define ISP_SHARP_MATRIX_UV_MEM_AUX_CTRL_S  0

/** ISP_YUV_FORMAT_REG register
 *  yuv format control register
 */
#define ISP_YUV_FORMAT_REG (DR_REG_ISP_BASE + 0x234)
/** ISP_YUV_MODE : R/W; bitpos: [0]; default: 0;
 *  this bit configures the yuv mode. 0: ITU-R BT.601, 1: ITU-R BT.709
 */
#define ISP_YUV_MODE    (BIT(0))
#define ISP_YUV_MODE_M  (ISP_YUV_MODE_V << ISP_YUV_MODE_S)
#define ISP_YUV_MODE_V  0x00000001U
#define ISP_YUV_MODE_S  0
/** ISP_YUV_RANGE : R/W; bitpos: [1]; default: 0;
 *  this bit configures the yuv range. 0: full range, 1: limit range
 */
#define ISP_YUV_RANGE    (BIT(1))
#define ISP_YUV_RANGE_M  (ISP_YUV_RANGE_V << ISP_YUV_RANGE_S)
#define ISP_YUV_RANGE_V  0x00000001U
#define ISP_YUV_RANGE_S  1

/** ISP_RDN_ECO_CS_REG register
 *  rdn eco cs register
 */
#define ISP_RDN_ECO_CS_REG (DR_REG_ISP_BASE + 0x238)
/** ISP_RDN_ECO_EN : R/W; bitpos: [0]; default: 0;
 *  rdn_eco_en
 */
#define ISP_RDN_ECO_EN    (BIT(0))
#define ISP_RDN_ECO_EN_M  (ISP_RDN_ECO_EN_V << ISP_RDN_ECO_EN_S)
#define ISP_RDN_ECO_EN_V  0x00000001U
#define ISP_RDN_ECO_EN_S  0
/** ISP_RDN_ECO_RESULT : RO; bitpos: [1]; default: 0;
 *  rdn_eco_result
 */
#define ISP_RDN_ECO_RESULT    (BIT(1))
#define ISP_RDN_ECO_RESULT_M  (ISP_RDN_ECO_RESULT_V << ISP_RDN_ECO_RESULT_S)
#define ISP_RDN_ECO_RESULT_V  0x00000001U
#define ISP_RDN_ECO_RESULT_S  1

/** ISP_RDN_ECO_LOW_REG register
 *  rdn eco all low register
 */
#define ISP_RDN_ECO_LOW_REG (DR_REG_ISP_BASE + 0x23c)
/** ISP_RDN_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  rdn_eco_low
 */
#define ISP_RDN_ECO_LOW    0xFFFFFFFFU
#define ISP_RDN_ECO_LOW_M  (ISP_RDN_ECO_LOW_V << ISP_RDN_ECO_LOW_S)
#define ISP_RDN_ECO_LOW_V  0xFFFFFFFFU
#define ISP_RDN_ECO_LOW_S  0

/** ISP_RDN_ECO_HIGH_REG register
 *  rdn eco all high register
 */
#define ISP_RDN_ECO_HIGH_REG (DR_REG_ISP_BASE + 0x240)
/** ISP_RDN_ECO_HIGH : R/W; bitpos: [31:0]; default: 4294967295;
 *  rdn_eco_high
 */
#define ISP_RDN_ECO_HIGH    0xFFFFFFFFU
#define ISP_RDN_ECO_HIGH_M  (ISP_RDN_ECO_HIGH_V << ISP_RDN_ECO_HIGH_S)
#define ISP_RDN_ECO_HIGH_V  0xFFFFFFFFU
#define ISP_RDN_ECO_HIGH_S  0

#ifdef __cplusplus
}
#endif
