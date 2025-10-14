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

/** DSI_BRG_CLK_EN_REG register
 *  dsi bridge clk control register
 */
#define DSI_BRG_CLK_EN_REG (DR_REG_DSI_BRG_BASE + 0x0)
/** DSI_BRG_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  this bit configures force_on of dsi_bridge register clock gate
 */
#define DSI_BRG_CLK_EN    (BIT(0))
#define DSI_BRG_CLK_EN_M  (DSI_BRG_CLK_EN_V << DSI_BRG_CLK_EN_S)
#define DSI_BRG_CLK_EN_V  0x00000001U
#define DSI_BRG_CLK_EN_S  0

/** DSI_BRG_EN_REG register
 *  dsi bridge en register
 */
#define DSI_BRG_EN_REG (DR_REG_DSI_BRG_BASE + 0x4)
/** DSI_BRG_DSI_EN : R/W; bitpos: [0]; default: 0;
 *  this bit configures module enable of dsi_bridge. 0: disable, 1: enable
 */
#define DSI_BRG_DSI_EN    (BIT(0))
#define DSI_BRG_DSI_EN_M  (DSI_BRG_DSI_EN_V << DSI_BRG_DSI_EN_S)
#define DSI_BRG_DSI_EN_V  0x00000001U
#define DSI_BRG_DSI_EN_S  0

/** DSI_BRG_DMA_REQ_CFG_REG register
 *  dsi bridge dma burst len register
 */
#define DSI_BRG_DMA_REQ_CFG_REG (DR_REG_DSI_BRG_BASE + 0x8)
/** DSI_BRG_DMA_BURST_LEN : R/W; bitpos: [11:0]; default: 128;
 *  this field configures the num of 64-bit in one dma burst transfer, valid only when
 *  dsi_bridge as flow controller
 */
#define DSI_BRG_DMA_BURST_LEN    0x00000FFFU
#define DSI_BRG_DMA_BURST_LEN_M  (DSI_BRG_DMA_BURST_LEN_V << DSI_BRG_DMA_BURST_LEN_S)
#define DSI_BRG_DMA_BURST_LEN_V  0x00000FFFU
#define DSI_BRG_DMA_BURST_LEN_S  0

/** DSI_BRG_RAW_NUM_CFG_REG register
 *  dsi bridge raw number control register
 */
#define DSI_BRG_RAW_NUM_CFG_REG (DR_REG_DSI_BRG_BASE + 0xc)
/** DSI_BRG_RAW_NUM_TOTAL : R/W; bitpos: [21:0]; default: 230400;
 *  this field configures number of total pix bits/64
 */
#define DSI_BRG_RAW_NUM_TOTAL    0x003FFFFFU
#define DSI_BRG_RAW_NUM_TOTAL_M  (DSI_BRG_RAW_NUM_TOTAL_V << DSI_BRG_RAW_NUM_TOTAL_S)
#define DSI_BRG_RAW_NUM_TOTAL_V  0x003FFFFFU
#define DSI_BRG_RAW_NUM_TOTAL_S  0
/** DSI_BRG_UNALIGN_64BIT_EN : R/W; bitpos: [22]; default: 0;
 *  this field configures whether the total pix bits is a multiple of 64bits. 0: align
 *  to 64-bit, 1: unalign to 64-bit
 */
#define DSI_BRG_UNALIGN_64BIT_EN    (BIT(22))
#define DSI_BRG_UNALIGN_64BIT_EN_M  (DSI_BRG_UNALIGN_64BIT_EN_V << DSI_BRG_UNALIGN_64BIT_EN_S)
#define DSI_BRG_UNALIGN_64BIT_EN_V  0x00000001U
#define DSI_BRG_UNALIGN_64BIT_EN_S  22
/** DSI_BRG_RAW_NUM_TOTAL_SET : WT; bitpos: [31]; default: 0;
 *  this bit configures enable of reload reg_raw_num_total to internal cnt. 0: disable,
 *  1: enable. valid only when dsi_bridge as flow controller
 */
#define DSI_BRG_RAW_NUM_TOTAL_SET    (BIT(31))
#define DSI_BRG_RAW_NUM_TOTAL_SET_M  (DSI_BRG_RAW_NUM_TOTAL_SET_V << DSI_BRG_RAW_NUM_TOTAL_SET_S)
#define DSI_BRG_RAW_NUM_TOTAL_SET_V  0x00000001U
#define DSI_BRG_RAW_NUM_TOTAL_SET_S  31

/** DSI_BRG_RAW_BUF_CREDIT_CTL_REG register
 *  dsi bridge credit register
 */
#define DSI_BRG_RAW_BUF_CREDIT_CTL_REG (DR_REG_DSI_BRG_BASE + 0x10)
/** DSI_BRG_CREDIT_THRD : R/W; bitpos: [14:0]; default: 1024;
 *  this field configures the threshold whether dsi_bridge fifo can receive one more
 *  64-bit, valid only when dsi_bridge as flow controller
 */
#define DSI_BRG_CREDIT_THRD    0x00007FFFU
#define DSI_BRG_CREDIT_THRD_M  (DSI_BRG_CREDIT_THRD_V << DSI_BRG_CREDIT_THRD_S)
#define DSI_BRG_CREDIT_THRD_V  0x00007FFFU
#define DSI_BRG_CREDIT_THRD_S  0
/** DSI_BRG_CREDIT_BURST_THRD : R/W; bitpos: [30:16]; default: 800;
 *  this field configures the threshold whether dsi_bridge fifo can receive one more
 *  dma burst, valid only when dsi_bridge as flow controller
 */
#define DSI_BRG_CREDIT_BURST_THRD    0x00007FFFU
#define DSI_BRG_CREDIT_BURST_THRD_M  (DSI_BRG_CREDIT_BURST_THRD_V << DSI_BRG_CREDIT_BURST_THRD_S)
#define DSI_BRG_CREDIT_BURST_THRD_V  0x00007FFFU
#define DSI_BRG_CREDIT_BURST_THRD_S  16
/** DSI_BRG_CREDIT_RESET : R/W; bitpos: [31]; default: 0;
 *  this bit configures internal credit cnt clear, 0: non, 1: reset. valid only when
 *  dsi_bridge as flow controller
 */
#define DSI_BRG_CREDIT_RESET    (BIT(31))
#define DSI_BRG_CREDIT_RESET_M  (DSI_BRG_CREDIT_RESET_V << DSI_BRG_CREDIT_RESET_S)
#define DSI_BRG_CREDIT_RESET_V  0x00000001U
#define DSI_BRG_CREDIT_RESET_S  31

/** DSI_BRG_FIFO_FLOW_STATUS_REG register
 *  dsi bridge raw buffer depth register
 */
#define DSI_BRG_FIFO_FLOW_STATUS_REG (DR_REG_DSI_BRG_BASE + 0x14)
/** DSI_BRG_RAW_BUF_DEPTH : RO; bitpos: [13:0]; default: 0;
 *  this field configures the depth of dsi_bridge fifo depth
 */
#define DSI_BRG_RAW_BUF_DEPTH    0x00003FFFU
#define DSI_BRG_RAW_BUF_DEPTH_M  (DSI_BRG_RAW_BUF_DEPTH_V << DSI_BRG_RAW_BUF_DEPTH_S)
#define DSI_BRG_RAW_BUF_DEPTH_V  0x00003FFFU
#define DSI_BRG_RAW_BUF_DEPTH_S  0

/** DSI_BRG_PIXEL_TYPE_REG register
 *  dsi bridge dpi type control register
 */
#define DSI_BRG_PIXEL_TYPE_REG (DR_REG_DSI_BRG_BASE + 0x18)
/** DSI_BRG_RAW_TYPE : R/W; bitpos: [3:0]; default: 0;
 *  this field configures the pixel type. 0: rgb888, 1:rgb666, 2:rgb565
 */
#define DSI_BRG_RAW_TYPE    0x0000000FU
#define DSI_BRG_RAW_TYPE_M  (DSI_BRG_RAW_TYPE_V << DSI_BRG_RAW_TYPE_S)
#define DSI_BRG_RAW_TYPE_V  0x0000000FU
#define DSI_BRG_RAW_TYPE_S  0
/** DSI_BRG_DPI_CONFIG : R/W; bitpos: [5:4]; default: 0;
 *  this field configures the pixel arrange type of dpi interface
 */
#define DSI_BRG_DPI_CONFIG    0x00000003U
#define DSI_BRG_DPI_CONFIG_M  (DSI_BRG_DPI_CONFIG_V << DSI_BRG_DPI_CONFIG_S)
#define DSI_BRG_DPI_CONFIG_V  0x00000003U
#define DSI_BRG_DPI_CONFIG_S  4
/** DSI_BRG_DATA_IN_TYPE : R/W; bitpos: [6]; default: 0;
 *  input data type, 0: rgb, 1: yuv
 */
#define DSI_BRG_DATA_IN_TYPE    (BIT(6))
#define DSI_BRG_DATA_IN_TYPE_M  (DSI_BRG_DATA_IN_TYPE_V << DSI_BRG_DATA_IN_TYPE_S)
#define DSI_BRG_DATA_IN_TYPE_V  0x00000001U
#define DSI_BRG_DATA_IN_TYPE_S  6

/** DSI_BRG_DMA_BLOCK_INTERVAL_REG register
 *  dsi bridge dma block interval control register
 */
#define DSI_BRG_DMA_BLOCK_INTERVAL_REG (DR_REG_DSI_BRG_BASE + 0x1c)
/** DSI_BRG_DMA_BLOCK_SLOT : R/W; bitpos: [9:0]; default: 9;
 *  this field configures the max block_slot_cnt
 */
#define DSI_BRG_DMA_BLOCK_SLOT    0x000003FFU
#define DSI_BRG_DMA_BLOCK_SLOT_M  (DSI_BRG_DMA_BLOCK_SLOT_V << DSI_BRG_DMA_BLOCK_SLOT_S)
#define DSI_BRG_DMA_BLOCK_SLOT_V  0x000003FFU
#define DSI_BRG_DMA_BLOCK_SLOT_S  0
/** DSI_BRG_DMA_BLOCK_INTERVAL : R/W; bitpos: [27:10]; default: 9;
 *  this field configures the max block_interval_cnt, block_interval_cnt increased by 1
 *  when block_slot_cnt if full
 */
#define DSI_BRG_DMA_BLOCK_INTERVAL    0x0003FFFFU
#define DSI_BRG_DMA_BLOCK_INTERVAL_M  (DSI_BRG_DMA_BLOCK_INTERVAL_V << DSI_BRG_DMA_BLOCK_INTERVAL_S)
#define DSI_BRG_DMA_BLOCK_INTERVAL_V  0x0003FFFFU
#define DSI_BRG_DMA_BLOCK_INTERVAL_S  10
/** DSI_BRG_RAW_NUM_TOTAL_AUTO_RELOAD : R/W; bitpos: [28]; default: 1;
 *  this bit configures enable of auto reload reg_raw_num_total, 0: disable, 1: enable
 */
#define DSI_BRG_RAW_NUM_TOTAL_AUTO_RELOAD    (BIT(28))
#define DSI_BRG_RAW_NUM_TOTAL_AUTO_RELOAD_M  (DSI_BRG_RAW_NUM_TOTAL_AUTO_RELOAD_V << DSI_BRG_RAW_NUM_TOTAL_AUTO_RELOAD_S)
#define DSI_BRG_RAW_NUM_TOTAL_AUTO_RELOAD_V  0x00000001U
#define DSI_BRG_RAW_NUM_TOTAL_AUTO_RELOAD_S  28
/** DSI_BRG_DMA_BLOCK_INTERVAL_EN : R/W; bitpos: [29]; default: 1;
 *  this bit configures enable of interval between dma block transfer, 0: disable, 1:
 *  enable
 */
#define DSI_BRG_DMA_BLOCK_INTERVAL_EN    (BIT(29))
#define DSI_BRG_DMA_BLOCK_INTERVAL_EN_M  (DSI_BRG_DMA_BLOCK_INTERVAL_EN_V << DSI_BRG_DMA_BLOCK_INTERVAL_EN_S)
#define DSI_BRG_DMA_BLOCK_INTERVAL_EN_V  0x00000001U
#define DSI_BRG_DMA_BLOCK_INTERVAL_EN_S  29

/** DSI_BRG_DMA_REQ_INTERVAL_REG register
 *  dsi bridge dma req interval control register
 */
#define DSI_BRG_DMA_REQ_INTERVAL_REG (DR_REG_DSI_BRG_BASE + 0x20)
/** DSI_BRG_DMA_REQ_INTERVAL : R/W; bitpos: [15:0]; default: 1;
 *  this field configures the interval between dma req events
 */
#define DSI_BRG_DMA_REQ_INTERVAL    0x0000FFFFU
#define DSI_BRG_DMA_REQ_INTERVAL_M  (DSI_BRG_DMA_REQ_INTERVAL_V << DSI_BRG_DMA_REQ_INTERVAL_S)
#define DSI_BRG_DMA_REQ_INTERVAL_V  0x0000FFFFU
#define DSI_BRG_DMA_REQ_INTERVAL_S  0

/** DSI_BRG_DPI_LCD_CTL_REG register
 *  dsi bridge dpi signal control register
 */
#define DSI_BRG_DPI_LCD_CTL_REG (DR_REG_DSI_BRG_BASE + 0x24)
/** DSI_BRG_DPISHUTDN : R/W; bitpos: [0]; default: 0;
 *  this bit configures dpishutdn signal in dpi interface
 */
#define DSI_BRG_DPISHUTDN    (BIT(0))
#define DSI_BRG_DPISHUTDN_M  (DSI_BRG_DPISHUTDN_V << DSI_BRG_DPISHUTDN_S)
#define DSI_BRG_DPISHUTDN_V  0x00000001U
#define DSI_BRG_DPISHUTDN_S  0
/** DSI_BRG_DPICOLORM : R/W; bitpos: [1]; default: 0;
 *  this bit configures dpicolorm signal in dpi interface
 */
#define DSI_BRG_DPICOLORM    (BIT(1))
#define DSI_BRG_DPICOLORM_M  (DSI_BRG_DPICOLORM_V << DSI_BRG_DPICOLORM_S)
#define DSI_BRG_DPICOLORM_V  0x00000001U
#define DSI_BRG_DPICOLORM_S  1
/** DSI_BRG_DPIUPDATECFG : R/W; bitpos: [2]; default: 0;
 *  this bit configures dpiupdatecfg signal in dpi interface
 */
#define DSI_BRG_DPIUPDATECFG    (BIT(2))
#define DSI_BRG_DPIUPDATECFG_M  (DSI_BRG_DPIUPDATECFG_V << DSI_BRG_DPIUPDATECFG_S)
#define DSI_BRG_DPIUPDATECFG_V  0x00000001U
#define DSI_BRG_DPIUPDATECFG_S  2

/** DSI_BRG_DPI_RSV_DPI_DATA_REG register
 *  dsi bridge dpi reserved data register
 */
#define DSI_BRG_DPI_RSV_DPI_DATA_REG (DR_REG_DSI_BRG_BASE + 0x28)
/** DSI_BRG_DPI_RSV_DATA : R/W; bitpos: [29:0]; default: 16383;
 *  this field controls the pixel data sent to dsi_host when dsi_bridge fifo underflow
 */
#define DSI_BRG_DPI_RSV_DATA    0x3FFFFFFFU
#define DSI_BRG_DPI_RSV_DATA_M  (DSI_BRG_DPI_RSV_DATA_V << DSI_BRG_DPI_RSV_DATA_S)
#define DSI_BRG_DPI_RSV_DATA_V  0x3FFFFFFFU
#define DSI_BRG_DPI_RSV_DATA_S  0

/** DSI_BRG_DPI_V_CFG0_REG register
 *  dsi bridge dpi v config register 0
 */
#define DSI_BRG_DPI_V_CFG0_REG (DR_REG_DSI_BRG_BASE + 0x30)
/** DSI_BRG_VTOTAL : R/W; bitpos: [11:0]; default: 525;
 *  this field configures the total length of one frame (by line) for dpi output, must
 *  meet: reg_vtotal > reg_vdisp+reg_vsync+reg_vbank
 */
#define DSI_BRG_VTOTAL    0x00000FFFU
#define DSI_BRG_VTOTAL_M  (DSI_BRG_VTOTAL_V << DSI_BRG_VTOTAL_S)
#define DSI_BRG_VTOTAL_V  0x00000FFFU
#define DSI_BRG_VTOTAL_S  0
/** DSI_BRG_VDISP : R/W; bitpos: [27:16]; default: 480;
 *  this field configures the length of valid line (by line) for dpi output
 */
#define DSI_BRG_VDISP    0x00000FFFU
#define DSI_BRG_VDISP_M  (DSI_BRG_VDISP_V << DSI_BRG_VDISP_S)
#define DSI_BRG_VDISP_V  0x00000FFFU
#define DSI_BRG_VDISP_S  16

/** DSI_BRG_DPI_V_CFG1_REG register
 *  dsi bridge dpi v config register 1
 */
#define DSI_BRG_DPI_V_CFG1_REG (DR_REG_DSI_BRG_BASE + 0x34)
/** DSI_BRG_VBANK : R/W; bitpos: [11:0]; default: 33;
 *  this field configures the length between vsync and valid line (by line) for dpi
 *  output
 */
#define DSI_BRG_VBANK    0x00000FFFU
#define DSI_BRG_VBANK_M  (DSI_BRG_VBANK_V << DSI_BRG_VBANK_S)
#define DSI_BRG_VBANK_V  0x00000FFFU
#define DSI_BRG_VBANK_S  0
/** DSI_BRG_VSYNC : R/W; bitpos: [27:16]; default: 2;
 *  this field configures the length of vsync (by line) for dpi output
 */
#define DSI_BRG_VSYNC    0x00000FFFU
#define DSI_BRG_VSYNC_M  (DSI_BRG_VSYNC_V << DSI_BRG_VSYNC_S)
#define DSI_BRG_VSYNC_V  0x00000FFFU
#define DSI_BRG_VSYNC_S  16

/** DSI_BRG_DPI_H_CFG0_REG register
 *  dsi bridge dpi h config register 0
 */
#define DSI_BRG_DPI_H_CFG0_REG (DR_REG_DSI_BRG_BASE + 0x38)
/** DSI_BRG_HTOTAL : R/W; bitpos: [11:0]; default: 800;
 *  this field configures the total length of one line (by pixel num) for dpi output,
 *  must meet: reg_htotal > reg_hdisp+reg_hsync+reg_hbank
 */
#define DSI_BRG_HTOTAL    0x00000FFFU
#define DSI_BRG_HTOTAL_M  (DSI_BRG_HTOTAL_V << DSI_BRG_HTOTAL_S)
#define DSI_BRG_HTOTAL_V  0x00000FFFU
#define DSI_BRG_HTOTAL_S  0
/** DSI_BRG_HDISP : R/W; bitpos: [27:16]; default: 640;
 *  this field configures the length of valid pixel data (by pixel num) for dpi output
 */
#define DSI_BRG_HDISP    0x00000FFFU
#define DSI_BRG_HDISP_M  (DSI_BRG_HDISP_V << DSI_BRG_HDISP_S)
#define DSI_BRG_HDISP_V  0x00000FFFU
#define DSI_BRG_HDISP_S  16

/** DSI_BRG_DPI_H_CFG1_REG register
 *  dsi bridge dpi h config register 1
 */
#define DSI_BRG_DPI_H_CFG1_REG (DR_REG_DSI_BRG_BASE + 0x3c)
/** DSI_BRG_HBANK : R/W; bitpos: [11:0]; default: 48;
 *  this field configures the length between hsync and pixel data valid (by pixel num)
 *  for dpi output
 */
#define DSI_BRG_HBANK    0x00000FFFU
#define DSI_BRG_HBANK_M  (DSI_BRG_HBANK_V << DSI_BRG_HBANK_S)
#define DSI_BRG_HBANK_V  0x00000FFFU
#define DSI_BRG_HBANK_S  0
/** DSI_BRG_HSYNC : R/W; bitpos: [27:16]; default: 96;
 *  this field configures the length of hsync (by pixel num) for dpi output
 */
#define DSI_BRG_HSYNC    0x00000FFFU
#define DSI_BRG_HSYNC_M  (DSI_BRG_HSYNC_V << DSI_BRG_HSYNC_S)
#define DSI_BRG_HSYNC_V  0x00000FFFU
#define DSI_BRG_HSYNC_S  16

/** DSI_BRG_DPI_MISC_CONFIG_REG register
 *  dsi_bridge dpi misc config register
 */
#define DSI_BRG_DPI_MISC_CONFIG_REG (DR_REG_DSI_BRG_BASE + 0x40)
/** DSI_BRG_DPI_EN : R/W; bitpos: [0]; default: 0;
 *  this bit configures enable of dpi output, 0: disable, 1: enable
 */
#define DSI_BRG_DPI_EN    (BIT(0))
#define DSI_BRG_DPI_EN_M  (DSI_BRG_DPI_EN_V << DSI_BRG_DPI_EN_S)
#define DSI_BRG_DPI_EN_V  0x00000001U
#define DSI_BRG_DPI_EN_S  0
/** DSI_BRG_FIFO_UNDERRUN_DISCARD_VCNT : R/W; bitpos: [15:4]; default: 413;
 *  this field configures the underrun interrupt musk, when underrun occurs and line
 *  cnt is less then this field
 */
#define DSI_BRG_FIFO_UNDERRUN_DISCARD_VCNT    0x00000FFFU
#define DSI_BRG_FIFO_UNDERRUN_DISCARD_VCNT_M  (DSI_BRG_FIFO_UNDERRUN_DISCARD_VCNT_V << DSI_BRG_FIFO_UNDERRUN_DISCARD_VCNT_S)
#define DSI_BRG_FIFO_UNDERRUN_DISCARD_VCNT_V  0x00000FFFU
#define DSI_BRG_FIFO_UNDERRUN_DISCARD_VCNT_S  4

/** DSI_BRG_DPI_CONFIG_UPDATE_REG register
 *  dsi_bridge dpi config update register
 */
#define DSI_BRG_DPI_CONFIG_UPDATE_REG (DR_REG_DSI_BRG_BASE + 0x44)
/** DSI_BRG_DPI_CONFIG_UPDATE : WT; bitpos: [0]; default: 0;
 *  write 1 to this bit to update dpi config register MIPI_DSI_BRG_DPI_*
 */
#define DSI_BRG_DPI_CONFIG_UPDATE    (BIT(0))
#define DSI_BRG_DPI_CONFIG_UPDATE_M  (DSI_BRG_DPI_CONFIG_UPDATE_V << DSI_BRG_DPI_CONFIG_UPDATE_S)
#define DSI_BRG_DPI_CONFIG_UPDATE_V  0x00000001U
#define DSI_BRG_DPI_CONFIG_UPDATE_S  0

/** DSI_BRG_INT_ENA_REG register
 *  dsi_bridge interrupt enable register
 */
#define DSI_BRG_INT_ENA_REG (DR_REG_DSI_BRG_BASE + 0x50)
/** DSI_BRG_UNDERRUN_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  write 1 to enables dpi_underrun_int_st field of MIPI_DSI_BRG_INT_ST_REG controlled
 *  by dpi_underrun  interrupt signal
 */
#define DSI_BRG_UNDERRUN_INT_ENA    (BIT(0))
#define DSI_BRG_UNDERRUN_INT_ENA_M  (DSI_BRG_UNDERRUN_INT_ENA_V << DSI_BRG_UNDERRUN_INT_ENA_S)
#define DSI_BRG_UNDERRUN_INT_ENA_V  0x00000001U
#define DSI_BRG_UNDERRUN_INT_ENA_S  0

/** DSI_BRG_INT_CLR_REG register
 *  dsi_bridge interrupt clear register
 */
#define DSI_BRG_INT_CLR_REG (DR_REG_DSI_BRG_BASE + 0x54)
/** DSI_BRG_UNDERRUN_INT_CLR : WT; bitpos: [0]; default: 0;
 *  write 1 to this bit to clear dpi_underrun_int_raw field of MIPI_DSI_BRG_INT_RAW_REG
 */
#define DSI_BRG_UNDERRUN_INT_CLR    (BIT(0))
#define DSI_BRG_UNDERRUN_INT_CLR_M  (DSI_BRG_UNDERRUN_INT_CLR_V << DSI_BRG_UNDERRUN_INT_CLR_S)
#define DSI_BRG_UNDERRUN_INT_CLR_V  0x00000001U
#define DSI_BRG_UNDERRUN_INT_CLR_S  0

/** DSI_BRG_INT_RAW_REG register
 *  dsi_bridge raw interrupt register
 */
#define DSI_BRG_INT_RAW_REG (DR_REG_DSI_BRG_BASE + 0x58)
/** DSI_BRG_UNDERRUN_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  the raw interrupt status of dpi_underrun
 */
#define DSI_BRG_UNDERRUN_INT_RAW    (BIT(0))
#define DSI_BRG_UNDERRUN_INT_RAW_M  (DSI_BRG_UNDERRUN_INT_RAW_V << DSI_BRG_UNDERRUN_INT_RAW_S)
#define DSI_BRG_UNDERRUN_INT_RAW_V  0x00000001U
#define DSI_BRG_UNDERRUN_INT_RAW_S  0

/** DSI_BRG_INT_ST_REG register
 *  dsi_bridge masked interrupt register
 */
#define DSI_BRG_INT_ST_REG (DR_REG_DSI_BRG_BASE + 0x5c)
/** DSI_BRG_UNDERRUN_INT_ST : RO; bitpos: [0]; default: 0;
 *  the masked interrupt status of dpi_underrun
 */
#define DSI_BRG_UNDERRUN_INT_ST    (BIT(0))
#define DSI_BRG_UNDERRUN_INT_ST_M  (DSI_BRG_UNDERRUN_INT_ST_V << DSI_BRG_UNDERRUN_INT_ST_S)
#define DSI_BRG_UNDERRUN_INT_ST_V  0x00000001U
#define DSI_BRG_UNDERRUN_INT_ST_S  0

/** DSI_BRG_HOST_BIST_CTL_REG register
 *  dsi_bridge host bist control register
 */
#define DSI_BRG_HOST_BIST_CTL_REG (DR_REG_DSI_BRG_BASE + 0x60)
/** DSI_BRG_BISTOK : RO; bitpos: [0]; default: 0;
 *  bistok
 */
#define DSI_BRG_BISTOK    (BIT(0))
#define DSI_BRG_BISTOK_M  (DSI_BRG_BISTOK_V << DSI_BRG_BISTOK_S)
#define DSI_BRG_BISTOK_V  0x00000001U
#define DSI_BRG_BISTOK_S  0
/** DSI_BRG_BISTON : R/W; bitpos: [1]; default: 0;
 *  biston
 */
#define DSI_BRG_BISTON    (BIT(1))
#define DSI_BRG_BISTON_M  (DSI_BRG_BISTON_V << DSI_BRG_BISTON_S)
#define DSI_BRG_BISTON_V  0x00000001U
#define DSI_BRG_BISTON_S  1

/** DSI_BRG_HOST_TRIGGER_REV_REG register
 *  dsi_bridge host trigger reverse control register
 */
#define DSI_BRG_HOST_TRIGGER_REV_REG (DR_REG_DSI_BRG_BASE + 0x64)
/** DSI_BRG_TX_TRIGGER_REV_EN : R/W; bitpos: [0]; default: 0;
 *  tx_trigger reverse. 0: disable, 1: enable
 */
#define DSI_BRG_TX_TRIGGER_REV_EN    (BIT(0))
#define DSI_BRG_TX_TRIGGER_REV_EN_M  (DSI_BRG_TX_TRIGGER_REV_EN_V << DSI_BRG_TX_TRIGGER_REV_EN_S)
#define DSI_BRG_TX_TRIGGER_REV_EN_V  0x00000001U
#define DSI_BRG_TX_TRIGGER_REV_EN_S  0
/** DSI_BRG_RX_TRIGGER_REV_EN : R/W; bitpos: [1]; default: 0;
 *  rx_trigger reverse. 0: disable, 1: enable
 */
#define DSI_BRG_RX_TRIGGER_REV_EN    (BIT(1))
#define DSI_BRG_RX_TRIGGER_REV_EN_M  (DSI_BRG_RX_TRIGGER_REV_EN_V << DSI_BRG_RX_TRIGGER_REV_EN_S)
#define DSI_BRG_RX_TRIGGER_REV_EN_V  0x00000001U
#define DSI_BRG_RX_TRIGGER_REV_EN_S  1

/** DSI_BRG_BLK_RAW_NUM_CFG_REG register
 *  dsi_bridge block raw number control register
 */
#define DSI_BRG_BLK_RAW_NUM_CFG_REG (DR_REG_DSI_BRG_BASE + 0x68)
/** DSI_BRG_BLK_RAW_NUM_TOTAL : R/W; bitpos: [21:0]; default: 230400;
 *  this field configures number of total block pix bits/64
 */
#define DSI_BRG_BLK_RAW_NUM_TOTAL    0x003FFFFFU
#define DSI_BRG_BLK_RAW_NUM_TOTAL_M  (DSI_BRG_BLK_RAW_NUM_TOTAL_V << DSI_BRG_BLK_RAW_NUM_TOTAL_S)
#define DSI_BRG_BLK_RAW_NUM_TOTAL_V  0x003FFFFFU
#define DSI_BRG_BLK_RAW_NUM_TOTAL_S  0
/** DSI_BRG_BLK_RAW_NUM_TOTAL_SET : WT; bitpos: [31]; default: 0;
 *  write 1 to reload reg_blk_raw_num_total to internal cnt
 */
#define DSI_BRG_BLK_RAW_NUM_TOTAL_SET    (BIT(31))
#define DSI_BRG_BLK_RAW_NUM_TOTAL_SET_M  (DSI_BRG_BLK_RAW_NUM_TOTAL_SET_V << DSI_BRG_BLK_RAW_NUM_TOTAL_SET_S)
#define DSI_BRG_BLK_RAW_NUM_TOTAL_SET_V  0x00000001U
#define DSI_BRG_BLK_RAW_NUM_TOTAL_SET_S  31

/** DSI_BRG_DMA_FRAME_INTERVAL_REG register
 *  dsi_bridge dam frame interval control register
 */
#define DSI_BRG_DMA_FRAME_INTERVAL_REG (DR_REG_DSI_BRG_BASE + 0x6c)
/** DSI_BRG_DMA_FRAME_SLOT : R/W; bitpos: [9:0]; default: 9;
 *  this field configures the max frame_slot_cnt
 */
#define DSI_BRG_DMA_FRAME_SLOT    0x000003FFU
#define DSI_BRG_DMA_FRAME_SLOT_M  (DSI_BRG_DMA_FRAME_SLOT_V << DSI_BRG_DMA_FRAME_SLOT_S)
#define DSI_BRG_DMA_FRAME_SLOT_V  0x000003FFU
#define DSI_BRG_DMA_FRAME_SLOT_S  0
/** DSI_BRG_DMA_FRAME_INTERVAL : R/W; bitpos: [27:10]; default: 9;
 *  this field configures the max frame_interval_cnt, frame_interval_cnt increased by 1
 *  when frame_slot_cnt if full
 */
#define DSI_BRG_DMA_FRAME_INTERVAL    0x0003FFFFU
#define DSI_BRG_DMA_FRAME_INTERVAL_M  (DSI_BRG_DMA_FRAME_INTERVAL_V << DSI_BRG_DMA_FRAME_INTERVAL_S)
#define DSI_BRG_DMA_FRAME_INTERVAL_V  0x0003FFFFU
#define DSI_BRG_DMA_FRAME_INTERVAL_S  10
/** DSI_BRG_DMA_MULTIBLK_EN : R/W; bitpos: [28]; default: 0;
 *  this bit configures enable multi-blk transfer, 0: disable, 1: enable
 */
#define DSI_BRG_DMA_MULTIBLK_EN    (BIT(28))
#define DSI_BRG_DMA_MULTIBLK_EN_M  (DSI_BRG_DMA_MULTIBLK_EN_V << DSI_BRG_DMA_MULTIBLK_EN_S)
#define DSI_BRG_DMA_MULTIBLK_EN_V  0x00000001U
#define DSI_BRG_DMA_MULTIBLK_EN_S  28
/** DSI_BRG_DMA_FRAME_INTERVAL_EN : R/W; bitpos: [29]; default: 1;
 *  this bit configures enable interval between frame transfer, 0: disable, 1: enable
 */
#define DSI_BRG_DMA_FRAME_INTERVAL_EN    (BIT(29))
#define DSI_BRG_DMA_FRAME_INTERVAL_EN_M  (DSI_BRG_DMA_FRAME_INTERVAL_EN_V << DSI_BRG_DMA_FRAME_INTERVAL_EN_S)
#define DSI_BRG_DMA_FRAME_INTERVAL_EN_V  0x00000001U
#define DSI_BRG_DMA_FRAME_INTERVAL_EN_S  29

/** DSI_BRG_MEM_AUX_CTRL_REG register
 *  dsi_bridge mem aux control register
 */
#define DSI_BRG_MEM_AUX_CTRL_REG (DR_REG_DSI_BRG_BASE + 0x70)
/** DSI_BRG_DSI_MEM_AUX_CTRL : R/W; bitpos: [13:0]; default: 4896;
 *  this field configures dsi_bridge fifo memory aux ctrl
 */
#define DSI_BRG_DSI_MEM_AUX_CTRL    0x00003FFFU
#define DSI_BRG_DSI_MEM_AUX_CTRL_M  (DSI_BRG_DSI_MEM_AUX_CTRL_V << DSI_BRG_DSI_MEM_AUX_CTRL_S)
#define DSI_BRG_DSI_MEM_AUX_CTRL_V  0x00003FFFU
#define DSI_BRG_DSI_MEM_AUX_CTRL_S  0

/** DSI_BRG_RDN_ECO_CS_REG register
 *  dsi_bridge rdn eco cs register
 */
#define DSI_BRG_RDN_ECO_CS_REG (DR_REG_DSI_BRG_BASE + 0x74)
/** DSI_BRG_RDN_ECO_EN : R/W; bitpos: [0]; default: 0;
 *  rdn_eco_en
 */
#define DSI_BRG_RDN_ECO_EN    (BIT(0))
#define DSI_BRG_RDN_ECO_EN_M  (DSI_BRG_RDN_ECO_EN_V << DSI_BRG_RDN_ECO_EN_S)
#define DSI_BRG_RDN_ECO_EN_V  0x00000001U
#define DSI_BRG_RDN_ECO_EN_S  0
/** DSI_BRG_RDN_ECO_RESULT : RO; bitpos: [1]; default: 0;
 *  rdn_eco_result
 */
#define DSI_BRG_RDN_ECO_RESULT    (BIT(1))
#define DSI_BRG_RDN_ECO_RESULT_M  (DSI_BRG_RDN_ECO_RESULT_V << DSI_BRG_RDN_ECO_RESULT_S)
#define DSI_BRG_RDN_ECO_RESULT_V  0x00000001U
#define DSI_BRG_RDN_ECO_RESULT_S  1

/** DSI_BRG_RDN_ECO_LOW_REG register
 *  dsi_bridge rdn eco all low register
 */
#define DSI_BRG_RDN_ECO_LOW_REG (DR_REG_DSI_BRG_BASE + 0x78)
/** DSI_BRG_RDN_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  rdn_eco_low
 */
#define DSI_BRG_RDN_ECO_LOW    0xFFFFFFFFU
#define DSI_BRG_RDN_ECO_LOW_M  (DSI_BRG_RDN_ECO_LOW_V << DSI_BRG_RDN_ECO_LOW_S)
#define DSI_BRG_RDN_ECO_LOW_V  0xFFFFFFFFU
#define DSI_BRG_RDN_ECO_LOW_S  0

/** DSI_BRG_RDN_ECO_HIGH_REG register
 *  dsi_bridge rdn eco all high register
 */
#define DSI_BRG_RDN_ECO_HIGH_REG (DR_REG_DSI_BRG_BASE + 0x7c)
/** DSI_BRG_RDN_ECO_HIGH : R/W; bitpos: [31:0]; default: 4294967295;
 *  rdn_eco_high
 */
#define DSI_BRG_RDN_ECO_HIGH    0xFFFFFFFFU
#define DSI_BRG_RDN_ECO_HIGH_M  (DSI_BRG_RDN_ECO_HIGH_V << DSI_BRG_RDN_ECO_HIGH_S)
#define DSI_BRG_RDN_ECO_HIGH_V  0xFFFFFFFFU
#define DSI_BRG_RDN_ECO_HIGH_S  0

/** DSI_BRG_HOST_CTRL_REG register
 *  dsi_bridge host control register
 */
#define DSI_BRG_HOST_CTRL_REG (DR_REG_DSI_BRG_BASE + 0x80)
/** DSI_BRG_DSI_CFG_REF_CLK_EN : R/W; bitpos: [0]; default: 1;
 *  this bit configures the clk enable refclk and cfg_clk of dsi_host. 0: disable, 1:
 *  enable
 */
#define DSI_BRG_DSI_CFG_REF_CLK_EN    (BIT(0))
#define DSI_BRG_DSI_CFG_REF_CLK_EN_M  (DSI_BRG_DSI_CFG_REF_CLK_EN_V << DSI_BRG_DSI_CFG_REF_CLK_EN_S)
#define DSI_BRG_DSI_CFG_REF_CLK_EN_V  0x00000001U
#define DSI_BRG_DSI_CFG_REF_CLK_EN_S  0

/** DSI_BRG_MEM_CLK_CTRL_REG register
 *  dsi_bridge mem force on control register
 */
#define DSI_BRG_MEM_CLK_CTRL_REG (DR_REG_DSI_BRG_BASE + 0x84)
/** DSI_BRG_DSI_BRIDGE_MEM_CLK_FORCE_ON : R/W; bitpos: [0]; default: 0;
 *  this bit configures the clock force on of dsi_bridge fifo memory. 0: disable, 1:
 *  force on
 */
#define DSI_BRG_DSI_BRIDGE_MEM_CLK_FORCE_ON    (BIT(0))
#define DSI_BRG_DSI_BRIDGE_MEM_CLK_FORCE_ON_M  (DSI_BRG_DSI_BRIDGE_MEM_CLK_FORCE_ON_V << DSI_BRG_DSI_BRIDGE_MEM_CLK_FORCE_ON_S)
#define DSI_BRG_DSI_BRIDGE_MEM_CLK_FORCE_ON_V  0x00000001U
#define DSI_BRG_DSI_BRIDGE_MEM_CLK_FORCE_ON_S  0
/** DSI_BRG_DSI_MEM_CLK_FORCE_ON : R/W; bitpos: [1]; default: 0;
 *  this bit configures the clock force on of dpi fifo memory. 0: disable, 1: force on
 */
#define DSI_BRG_DSI_MEM_CLK_FORCE_ON    (BIT(1))
#define DSI_BRG_DSI_MEM_CLK_FORCE_ON_M  (DSI_BRG_DSI_MEM_CLK_FORCE_ON_V << DSI_BRG_DSI_MEM_CLK_FORCE_ON_S)
#define DSI_BRG_DSI_MEM_CLK_FORCE_ON_V  0x00000001U
#define DSI_BRG_DSI_MEM_CLK_FORCE_ON_S  1

/** DSI_BRG_DMA_FLOW_CTRL_REG register
 *  dsi_bridge dma flow controller register
 */
#define DSI_BRG_DMA_FLOW_CTRL_REG (DR_REG_DSI_BRG_BASE + 0x88)
/** DSI_BRG_DSI_DMA_FLOW_CONTROLLER : R/W; bitpos: [0]; default: 1;
 *  this bit configures the flow controller, 0: dmac as flow controller, 1:dsi_bridge
 *  as flow controller
 */
#define DSI_BRG_DSI_DMA_FLOW_CONTROLLER    (BIT(0))
#define DSI_BRG_DSI_DMA_FLOW_CONTROLLER_M  (DSI_BRG_DSI_DMA_FLOW_CONTROLLER_V << DSI_BRG_DSI_DMA_FLOW_CONTROLLER_S)
#define DSI_BRG_DSI_DMA_FLOW_CONTROLLER_V  0x00000001U
#define DSI_BRG_DSI_DMA_FLOW_CONTROLLER_S  0
/** DSI_BRG_DMA_FLOW_MULTIBLK_NUM : R/W; bitpos: [7:4]; default: 1;
 *  this field configures the num of blocks when multi-blk is enable and dmac as flow
 *  controller
 */
#define DSI_BRG_DMA_FLOW_MULTIBLK_NUM    0x0000000FU
#define DSI_BRG_DMA_FLOW_MULTIBLK_NUM_M  (DSI_BRG_DMA_FLOW_MULTIBLK_NUM_V << DSI_BRG_DMA_FLOW_MULTIBLK_NUM_S)
#define DSI_BRG_DMA_FLOW_MULTIBLK_NUM_V  0x0000000FU
#define DSI_BRG_DMA_FLOW_MULTIBLK_NUM_S  4

/** DSI_BRG_RAW_BUF_ALMOST_EMPTY_THRD_REG register
 *  dsi_bridge buffer empty threshold register
 */
#define DSI_BRG_RAW_BUF_ALMOST_EMPTY_THRD_REG (DR_REG_DSI_BRG_BASE + 0x8c)
/** DSI_BRG_DSI_RAW_BUF_ALMOST_EMPTY_THRD : R/W; bitpos: [10:0]; default: 512;
 *  this field configures the fifo almost empty threshold, is valid only when dmac as
 *  flow controller
 */
#define DSI_BRG_DSI_RAW_BUF_ALMOST_EMPTY_THRD    0x000007FFU
#define DSI_BRG_DSI_RAW_BUF_ALMOST_EMPTY_THRD_M  (DSI_BRG_DSI_RAW_BUF_ALMOST_EMPTY_THRD_V << DSI_BRG_DSI_RAW_BUF_ALMOST_EMPTY_THRD_S)
#define DSI_BRG_DSI_RAW_BUF_ALMOST_EMPTY_THRD_V  0x000007FFU
#define DSI_BRG_DSI_RAW_BUF_ALMOST_EMPTY_THRD_S  0

/** DSI_BRG_YUV_CFG_REG register
 *  dsi_bridge yuv format config register
 */
#define DSI_BRG_YUV_CFG_REG (DR_REG_DSI_BRG_BASE + 0x90)
/** DSI_BRG_PROTOCAL : R/W; bitpos: [0]; default: 0;
 *  this bit configures yuv protoocl, 0: bt.601, 1: bt.709
 */
#define DSI_BRG_PROTOCAL    (BIT(0))
#define DSI_BRG_PROTOCAL_M  (DSI_BRG_PROTOCAL_V << DSI_BRG_PROTOCAL_S)
#define DSI_BRG_PROTOCAL_V  0x00000001U
#define DSI_BRG_PROTOCAL_S  0
/** DSI_BRG_YUV_PIX_ENDIAN : R/W; bitpos: [1]; default: 0;
 *  this bit configures yuv pixel endian, 0: y0u0y1v1y2u2y3v3, 1: y3u3y2v2y1u1y0v0
 */
#define DSI_BRG_YUV_PIX_ENDIAN    (BIT(1))
#define DSI_BRG_YUV_PIX_ENDIAN_M  (DSI_BRG_YUV_PIX_ENDIAN_V << DSI_BRG_YUV_PIX_ENDIAN_S)
#define DSI_BRG_YUV_PIX_ENDIAN_V  0x00000001U
#define DSI_BRG_YUV_PIX_ENDIAN_S  1
/** DSI_BRG_YUV422_FORMAT : R/W; bitpos: [3:2]; default: 0;
 *  this field configures yuv422 store format, 0: yuyv, 1: yvyu, 2: uyvy, 3: vyuy
 */
#define DSI_BRG_YUV422_FORMAT    0x00000003U
#define DSI_BRG_YUV422_FORMAT_M  (DSI_BRG_YUV422_FORMAT_V << DSI_BRG_YUV422_FORMAT_S)
#define DSI_BRG_YUV422_FORMAT_V  0x00000003U
#define DSI_BRG_YUV422_FORMAT_S  2

/** DSI_BRG_PHY_LP_LOOPBACK_CTRL_REG register
 *  dsi phy lp_loopback test ctrl
 */
#define DSI_BRG_PHY_LP_LOOPBACK_CTRL_REG (DR_REG_DSI_BRG_BASE + 0x94)
/** DSI_BRG_PHY_LP_TXDATAESC_1 : R/W; bitpos: [7:0]; default: 0;
 *  txdataesc_1 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXDATAESC_1    0x000000FFU
#define DSI_BRG_PHY_LP_TXDATAESC_1_M  (DSI_BRG_PHY_LP_TXDATAESC_1_V << DSI_BRG_PHY_LP_TXDATAESC_1_S)
#define DSI_BRG_PHY_LP_TXDATAESC_1_V  0x000000FFU
#define DSI_BRG_PHY_LP_TXDATAESC_1_S  0
/** DSI_BRG_PHY_LP_TXREQUESTESC_1 : R/W; bitpos: [8]; default: 0;
 *  txrequestesc_1 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXREQUESTESC_1    (BIT(8))
#define DSI_BRG_PHY_LP_TXREQUESTESC_1_M  (DSI_BRG_PHY_LP_TXREQUESTESC_1_V << DSI_BRG_PHY_LP_TXREQUESTESC_1_S)
#define DSI_BRG_PHY_LP_TXREQUESTESC_1_V  0x00000001U
#define DSI_BRG_PHY_LP_TXREQUESTESC_1_S  8
/** DSI_BRG_PHY_LP_TXVALIDESC_1 : R/W; bitpos: [9]; default: 0;
 *  txvalidesc_1 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXVALIDESC_1    (BIT(9))
#define DSI_BRG_PHY_LP_TXVALIDESC_1_M  (DSI_BRG_PHY_LP_TXVALIDESC_1_V << DSI_BRG_PHY_LP_TXVALIDESC_1_S)
#define DSI_BRG_PHY_LP_TXVALIDESC_1_V  0x00000001U
#define DSI_BRG_PHY_LP_TXVALIDESC_1_S  9
/** DSI_BRG_PHY_LP_TXLPDTESC_1 : R/W; bitpos: [10]; default: 0;
 *  txlpdtesc_1 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXLPDTESC_1    (BIT(10))
#define DSI_BRG_PHY_LP_TXLPDTESC_1_M  (DSI_BRG_PHY_LP_TXLPDTESC_1_V << DSI_BRG_PHY_LP_TXLPDTESC_1_S)
#define DSI_BRG_PHY_LP_TXLPDTESC_1_V  0x00000001U
#define DSI_BRG_PHY_LP_TXLPDTESC_1_S  10
/** DSI_BRG_PHY_LP_BASEDIR_1 : R/W; bitpos: [11]; default: 0;
 *  basedir_1 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_BASEDIR_1    (BIT(11))
#define DSI_BRG_PHY_LP_BASEDIR_1_M  (DSI_BRG_PHY_LP_BASEDIR_1_V << DSI_BRG_PHY_LP_BASEDIR_1_S)
#define DSI_BRG_PHY_LP_BASEDIR_1_V  0x00000001U
#define DSI_BRG_PHY_LP_BASEDIR_1_S  11
/** DSI_BRG_PHY_LP_TXDATAESC_0 : R/W; bitpos: [23:16]; default: 0;
 *  txdataesc_0 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXDATAESC_0    0x000000FFU
#define DSI_BRG_PHY_LP_TXDATAESC_0_M  (DSI_BRG_PHY_LP_TXDATAESC_0_V << DSI_BRG_PHY_LP_TXDATAESC_0_S)
#define DSI_BRG_PHY_LP_TXDATAESC_0_V  0x000000FFU
#define DSI_BRG_PHY_LP_TXDATAESC_0_S  16
/** DSI_BRG_PHY_LP_TXREQUESTESC_0 : R/W; bitpos: [24]; default: 0;
 *  txrequestesc_0 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXREQUESTESC_0    (BIT(24))
#define DSI_BRG_PHY_LP_TXREQUESTESC_0_M  (DSI_BRG_PHY_LP_TXREQUESTESC_0_V << DSI_BRG_PHY_LP_TXREQUESTESC_0_S)
#define DSI_BRG_PHY_LP_TXREQUESTESC_0_V  0x00000001U
#define DSI_BRG_PHY_LP_TXREQUESTESC_0_S  24
/** DSI_BRG_PHY_LP_TXVALIDESC_0 : R/W; bitpos: [25]; default: 0;
 *  txvalidesc_0 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXVALIDESC_0    (BIT(25))
#define DSI_BRG_PHY_LP_TXVALIDESC_0_M  (DSI_BRG_PHY_LP_TXVALIDESC_0_V << DSI_BRG_PHY_LP_TXVALIDESC_0_S)
#define DSI_BRG_PHY_LP_TXVALIDESC_0_V  0x00000001U
#define DSI_BRG_PHY_LP_TXVALIDESC_0_S  25
/** DSI_BRG_PHY_LP_TXLPDTESC_0 : R/W; bitpos: [26]; default: 0;
 *  txlpdtesc_0 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_TXLPDTESC_0    (BIT(26))
#define DSI_BRG_PHY_LP_TXLPDTESC_0_M  (DSI_BRG_PHY_LP_TXLPDTESC_0_V << DSI_BRG_PHY_LP_TXLPDTESC_0_S)
#define DSI_BRG_PHY_LP_TXLPDTESC_0_V  0x00000001U
#define DSI_BRG_PHY_LP_TXLPDTESC_0_S  26
/** DSI_BRG_PHY_LP_BASEDIR_0 : R/W; bitpos: [27]; default: 0;
 *  basedir_0 ctrl when enable dsi phy lp_loopback_test
 */
#define DSI_BRG_PHY_LP_BASEDIR_0    (BIT(27))
#define DSI_BRG_PHY_LP_BASEDIR_0_M  (DSI_BRG_PHY_LP_BASEDIR_0_V << DSI_BRG_PHY_LP_BASEDIR_0_S)
#define DSI_BRG_PHY_LP_BASEDIR_0_V  0x00000001U
#define DSI_BRG_PHY_LP_BASEDIR_0_S  27
/** DSI_BRG_PHY_LP_LOOPBACK_CHECK : WT; bitpos: [28]; default: 0;
 *  dsi phy lp_loopback test start check
 */
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK    (BIT(28))
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK_M  (DSI_BRG_PHY_LP_LOOPBACK_CHECK_V << DSI_BRG_PHY_LP_LOOPBACK_CHECK_S)
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK_V  0x00000001U
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK_S  28
/** DSI_BRG_PHY_LP_LOOPBACK_CHECK_DONE : RO; bitpos: [29]; default: 0;
 *  dsi phy lp_loopback test check done
 */
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK_DONE    (BIT(29))
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK_DONE_M  (DSI_BRG_PHY_LP_LOOPBACK_CHECK_DONE_V << DSI_BRG_PHY_LP_LOOPBACK_CHECK_DONE_S)
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK_DONE_V  0x00000001U
#define DSI_BRG_PHY_LP_LOOPBACK_CHECK_DONE_S  29
/** DSI_BRG_PHY_LP_LOOPBACK_EN : R/W; bitpos: [30]; default: 0;
 *  dsi phy lp_loopback ctrl en
 */
#define DSI_BRG_PHY_LP_LOOPBACK_EN    (BIT(30))
#define DSI_BRG_PHY_LP_LOOPBACK_EN_M  (DSI_BRG_PHY_LP_LOOPBACK_EN_V << DSI_BRG_PHY_LP_LOOPBACK_EN_S)
#define DSI_BRG_PHY_LP_LOOPBACK_EN_V  0x00000001U
#define DSI_BRG_PHY_LP_LOOPBACK_EN_S  30
/** DSI_BRG_PHY_LP_LOOPBACK_OK : RO; bitpos: [31]; default: 0;
 *  result of dsi phy lp_loopback test
 */
#define DSI_BRG_PHY_LP_LOOPBACK_OK    (BIT(31))
#define DSI_BRG_PHY_LP_LOOPBACK_OK_M  (DSI_BRG_PHY_LP_LOOPBACK_OK_V << DSI_BRG_PHY_LP_LOOPBACK_OK_S)
#define DSI_BRG_PHY_LP_LOOPBACK_OK_V  0x00000001U
#define DSI_BRG_PHY_LP_LOOPBACK_OK_S  31

/** DSI_BRG_PHY_HS_LOOPBACK_CTRL_REG register
 *  dsi phy hp_loopback test ctrl
 */
#define DSI_BRG_PHY_HS_LOOPBACK_CTRL_REG (DR_REG_DSI_BRG_BASE + 0x98)
/** DSI_BRG_PHY_HS_TXDATAHS_1 : R/W; bitpos: [7:0]; default: 0;
 *  txdatahs_1 ctrl when enable dsi phy hs_loopback_test
 */
#define DSI_BRG_PHY_HS_TXDATAHS_1    0x000000FFU
#define DSI_BRG_PHY_HS_TXDATAHS_1_M  (DSI_BRG_PHY_HS_TXDATAHS_1_V << DSI_BRG_PHY_HS_TXDATAHS_1_S)
#define DSI_BRG_PHY_HS_TXDATAHS_1_V  0x000000FFU
#define DSI_BRG_PHY_HS_TXDATAHS_1_S  0
/** DSI_BRG_PHY_HS_TXREQUESTDATAHS_1 : R/W; bitpos: [8]; default: 0;
 *  txrequestdatahs_1 ctrl when enable dsi phy hs_loopback_test
 */
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_1    (BIT(8))
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_1_M  (DSI_BRG_PHY_HS_TXREQUESTDATAHS_1_V << DSI_BRG_PHY_HS_TXREQUESTDATAHS_1_S)
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_1_V  0x00000001U
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_1_S  8
/** DSI_BRG_PHY_HS_BASEDIR_1 : R/W; bitpos: [9]; default: 1;
 *  basedir_1 ctrl when enable dsi phy hs_loopback_test
 */
#define DSI_BRG_PHY_HS_BASEDIR_1    (BIT(9))
#define DSI_BRG_PHY_HS_BASEDIR_1_M  (DSI_BRG_PHY_HS_BASEDIR_1_V << DSI_BRG_PHY_HS_BASEDIR_1_S)
#define DSI_BRG_PHY_HS_BASEDIR_1_V  0x00000001U
#define DSI_BRG_PHY_HS_BASEDIR_1_S  9
/** DSI_BRG_PHY_HS_TXDATAHS_0 : R/W; bitpos: [23:16]; default: 0;
 *  txdatahs_0 ctrl when enable dsi phy hs_loopback_test
 */
#define DSI_BRG_PHY_HS_TXDATAHS_0    0x000000FFU
#define DSI_BRG_PHY_HS_TXDATAHS_0_M  (DSI_BRG_PHY_HS_TXDATAHS_0_V << DSI_BRG_PHY_HS_TXDATAHS_0_S)
#define DSI_BRG_PHY_HS_TXDATAHS_0_V  0x000000FFU
#define DSI_BRG_PHY_HS_TXDATAHS_0_S  16
/** DSI_BRG_PHY_HS_TXREQUESTDATAHS_0 : R/W; bitpos: [24]; default: 0;
 *  txrequestdatahs_0 ctrl when enable dsi phy hs_loopback_test
 */
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_0    (BIT(24))
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_0_M  (DSI_BRG_PHY_HS_TXREQUESTDATAHS_0_V << DSI_BRG_PHY_HS_TXREQUESTDATAHS_0_S)
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_0_V  0x00000001U
#define DSI_BRG_PHY_HS_TXREQUESTDATAHS_0_S  24
/** DSI_BRG_PHY_HS_BASEDIR_0 : R/W; bitpos: [25]; default: 0;
 *  basedir_0 ctrl when enable dsi phy hs_loopback_test
 */
#define DSI_BRG_PHY_HS_BASEDIR_0    (BIT(25))
#define DSI_BRG_PHY_HS_BASEDIR_0_M  (DSI_BRG_PHY_HS_BASEDIR_0_V << DSI_BRG_PHY_HS_BASEDIR_0_S)
#define DSI_BRG_PHY_HS_BASEDIR_0_V  0x00000001U
#define DSI_BRG_PHY_HS_BASEDIR_0_S  25
/** DSI_BRG_PHY_HS_TXREQUESTHSCLK : R/W; bitpos: [27]; default: 0;
 *  txrequesthsclk when  enable dsi phy hs_loopback_test
 */
#define DSI_BRG_PHY_HS_TXREQUESTHSCLK    (BIT(27))
#define DSI_BRG_PHY_HS_TXREQUESTHSCLK_M  (DSI_BRG_PHY_HS_TXREQUESTHSCLK_V << DSI_BRG_PHY_HS_TXREQUESTHSCLK_S)
#define DSI_BRG_PHY_HS_TXREQUESTHSCLK_V  0x00000001U
#define DSI_BRG_PHY_HS_TXREQUESTHSCLK_S  27
/** DSI_BRG_PHY_HS_LOOPBACK_CHECK : WT; bitpos: [28]; default: 0;
 *  dsi phy hs_loopback test start check
 */
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK    (BIT(28))
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK_M  (DSI_BRG_PHY_HS_LOOPBACK_CHECK_V << DSI_BRG_PHY_HS_LOOPBACK_CHECK_S)
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK_V  0x00000001U
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK_S  28
/** DSI_BRG_PHY_HS_LOOPBACK_CHECK_DONE : RO; bitpos: [29]; default: 0;
 *  dsi phy hs_loopback test check done
 */
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK_DONE    (BIT(29))
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK_DONE_M  (DSI_BRG_PHY_HS_LOOPBACK_CHECK_DONE_V << DSI_BRG_PHY_HS_LOOPBACK_CHECK_DONE_S)
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK_DONE_V  0x00000001U
#define DSI_BRG_PHY_HS_LOOPBACK_CHECK_DONE_S  29
/** DSI_BRG_PHY_HS_LOOPBACK_EN : R/W; bitpos: [30]; default: 0;
 *  dsi phy hs_loopback ctrl en
 */
#define DSI_BRG_PHY_HS_LOOPBACK_EN    (BIT(30))
#define DSI_BRG_PHY_HS_LOOPBACK_EN_M  (DSI_BRG_PHY_HS_LOOPBACK_EN_V << DSI_BRG_PHY_HS_LOOPBACK_EN_S)
#define DSI_BRG_PHY_HS_LOOPBACK_EN_V  0x00000001U
#define DSI_BRG_PHY_HS_LOOPBACK_EN_S  30
/** DSI_BRG_PHY_HS_LOOPBACK_OK : RO; bitpos: [31]; default: 0;
 *  result of dsi phy hs_loopback test
 */
#define DSI_BRG_PHY_HS_LOOPBACK_OK    (BIT(31))
#define DSI_BRG_PHY_HS_LOOPBACK_OK_M  (DSI_BRG_PHY_HS_LOOPBACK_OK_V << DSI_BRG_PHY_HS_LOOPBACK_OK_S)
#define DSI_BRG_PHY_HS_LOOPBACK_OK_V  0x00000001U
#define DSI_BRG_PHY_HS_LOOPBACK_OK_S  31

/** DSI_BRG_PHY_LOOPBACK_CNT_REG register
 *  loopback test cnt
 */
#define DSI_BRG_PHY_LOOPBACK_CNT_REG (DR_REG_DSI_BRG_BASE + 0x9c)
/** DSI_BRG_PHY_HS_CHECK_CNT_TH : R/W; bitpos: [7:0]; default: 64;
 *  hs_loopback test check cnt
 */
#define DSI_BRG_PHY_HS_CHECK_CNT_TH    0x000000FFU
#define DSI_BRG_PHY_HS_CHECK_CNT_TH_M  (DSI_BRG_PHY_HS_CHECK_CNT_TH_V << DSI_BRG_PHY_HS_CHECK_CNT_TH_S)
#define DSI_BRG_PHY_HS_CHECK_CNT_TH_V  0x000000FFU
#define DSI_BRG_PHY_HS_CHECK_CNT_TH_S  0
/** DSI_BRG_PHY_LP_CHECK_CNT_TH : R/W; bitpos: [23:16]; default: 64;
 *  lp_loopback test check cnt
 */
#define DSI_BRG_PHY_LP_CHECK_CNT_TH    0x000000FFU
#define DSI_BRG_PHY_LP_CHECK_CNT_TH_M  (DSI_BRG_PHY_LP_CHECK_CNT_TH_V << DSI_BRG_PHY_LP_CHECK_CNT_TH_S)
#define DSI_BRG_PHY_LP_CHECK_CNT_TH_V  0x000000FFU
#define DSI_BRG_PHY_LP_CHECK_CNT_TH_S  16

#ifdef __cplusplus
}
#endif
