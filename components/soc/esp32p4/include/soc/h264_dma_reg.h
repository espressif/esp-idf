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

/** H264_DMA_OUT_CONF0_CH0_REG register
 *  TX CH0 config0 register
 */
#define H264_DMA_OUT_CONF0_CH0_REG (DR_REG_H264_DMA_BASE + 0x0)
/** H264_DMA_OUT_AUTO_WRBACK_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define H264_DMA_OUT_AUTO_WRBACK_CH0    (BIT(0))
#define H264_DMA_OUT_AUTO_WRBACK_CH0_M  (H264_DMA_OUT_AUTO_WRBACK_CH0_V << H264_DMA_OUT_AUTO_WRBACK_CH0_S)
#define H264_DMA_OUT_AUTO_WRBACK_CH0_V  0x00000001U
#define H264_DMA_OUT_AUTO_WRBACK_CH0_S  0
/** H264_DMA_OUT_EOF_MODE_CH0 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define H264_DMA_OUT_EOF_MODE_CH0    (BIT(1))
#define H264_DMA_OUT_EOF_MODE_CH0_M  (H264_DMA_OUT_EOF_MODE_CH0_V << H264_DMA_OUT_EOF_MODE_CH0_S)
#define H264_DMA_OUT_EOF_MODE_CH0_V  0x00000001U
#define H264_DMA_OUT_EOF_MODE_CH0_S  1
/** H264_DMA_OUTDSCR_BURST_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define H264_DMA_OUTDSCR_BURST_EN_CH0    (BIT(2))
#define H264_DMA_OUTDSCR_BURST_EN_CH0_M  (H264_DMA_OUTDSCR_BURST_EN_CH0_V << H264_DMA_OUTDSCR_BURST_EN_CH0_S)
#define H264_DMA_OUTDSCR_BURST_EN_CH0_V  0x00000001U
#define H264_DMA_OUTDSCR_BURST_EN_CH0_S  2
/** H264_DMA_OUT_ECC_AES_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_OUT_ECC_AES_EN_CH0    (BIT(3))
#define H264_DMA_OUT_ECC_AES_EN_CH0_M  (H264_DMA_OUT_ECC_AES_EN_CH0_V << H264_DMA_OUT_ECC_AES_EN_CH0_S)
#define H264_DMA_OUT_ECC_AES_EN_CH0_V  0x00000001U
#define H264_DMA_OUT_ECC_AES_EN_CH0_S  3
/** H264_DMA_OUT_CHECK_OWNER_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_OUT_CHECK_OWNER_CH0    (BIT(4))
#define H264_DMA_OUT_CHECK_OWNER_CH0_M  (H264_DMA_OUT_CHECK_OWNER_CH0_V << H264_DMA_OUT_CHECK_OWNER_CH0_S)
#define H264_DMA_OUT_CHECK_OWNER_CH0_V  0x00000001U
#define H264_DMA_OUT_CHECK_OWNER_CH0_S  4
/** H264_DMA_OUT_MEM_BURST_LENGTH_CH0 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 0. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH0    0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH0_M  (H264_DMA_OUT_MEM_BURST_LENGTH_CH0_V << H264_DMA_OUT_MEM_BURST_LENGTH_CH0_S)
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH0_V  0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH0_S  6
/** H264_DMA_OUT_PAGE_BOUND_EN_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define H264_DMA_OUT_PAGE_BOUND_EN_CH0    (BIT(12))
#define H264_DMA_OUT_PAGE_BOUND_EN_CH0_M  (H264_DMA_OUT_PAGE_BOUND_EN_CH0_V << H264_DMA_OUT_PAGE_BOUND_EN_CH0_S)
#define H264_DMA_OUT_PAGE_BOUND_EN_CH0_V  0x00000001U
#define H264_DMA_OUT_PAGE_BOUND_EN_CH0_S  12
/** H264_DMA_OUT_REORDER_EN_CH0 : R/W; bitpos: [16]; default: 0;
 *  Enable TX channel 0 macro block reorder when set to 1, only channel0 have this
 *  selection
 */
#define H264_DMA_OUT_REORDER_EN_CH0    (BIT(16))
#define H264_DMA_OUT_REORDER_EN_CH0_M  (H264_DMA_OUT_REORDER_EN_CH0_V << H264_DMA_OUT_REORDER_EN_CH0_S)
#define H264_DMA_OUT_REORDER_EN_CH0_V  0x00000001U
#define H264_DMA_OUT_REORDER_EN_CH0_S  16
/** H264_DMA_OUT_RST_CH0 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset TX channel
 */
#define H264_DMA_OUT_RST_CH0    (BIT(24))
#define H264_DMA_OUT_RST_CH0_M  (H264_DMA_OUT_RST_CH0_V << H264_DMA_OUT_RST_CH0_S)
#define H264_DMA_OUT_RST_CH0_V  0x00000001U
#define H264_DMA_OUT_RST_CH0_S  24
/** H264_DMA_OUT_CMD_DISABLE_CH0 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_OUT_CMD_DISABLE_CH0    (BIT(25))
#define H264_DMA_OUT_CMD_DISABLE_CH0_M  (H264_DMA_OUT_CMD_DISABLE_CH0_V << H264_DMA_OUT_CMD_DISABLE_CH0_S)
#define H264_DMA_OUT_CMD_DISABLE_CH0_V  0x00000001U
#define H264_DMA_OUT_CMD_DISABLE_CH0_S  25
/** H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH0 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH0    (BIT(26))
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH0_M  (H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH0_V << H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH0_S)
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH0_V  0x00000001U
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH0_S  26

/** H264_DMA_OUT_INT_RAW_CH0_REG register
 *  TX CH0  interrupt raw register
 */
#define H264_DMA_OUT_INT_RAW_CH0_REG (DR_REG_H264_DMA_BASE + 0x4)
/** H264_DMA_OUT_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define H264_DMA_OUT_DONE_CH0_INT_RAW    (BIT(0))
#define H264_DMA_OUT_DONE_CH0_INT_RAW_M  (H264_DMA_OUT_DONE_CH0_INT_RAW_V << H264_DMA_OUT_DONE_CH0_INT_RAW_S)
#define H264_DMA_OUT_DONE_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DONE_CH0_INT_RAW_S  0
/** H264_DMA_OUT_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define H264_DMA_OUT_EOF_CH0_INT_RAW    (BIT(1))
#define H264_DMA_OUT_EOF_CH0_INT_RAW_M  (H264_DMA_OUT_EOF_CH0_INT_RAW_V << H264_DMA_OUT_EOF_CH0_INT_RAW_S)
#define H264_DMA_OUT_EOF_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_EOF_CH0_INT_RAW_S  1
/** H264_DMA_OUT_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_RAW    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_RAW_M  (H264_DMA_OUT_DSCR_ERR_CH0_INT_RAW_V << H264_DMA_OUT_DSCR_ERR_CH0_INT_RAW_S)
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_RAW_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_RAW    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_M  (H264_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_V << H264_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_S)
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_RAW    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_RAW_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_RAW    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_RAW_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_RAW    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L2_CH0_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L2_CH0_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_RAW_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_RAW    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L2_CH0_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L2_CH0_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_RAW_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_RAW    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_RAW_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_RAW_V << H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_RAW_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_RAW_S  8

/** H264_DMA_OUT_INT_ENA_CH0_REG register
 *  TX CH0  interrupt ena register
 */
#define H264_DMA_OUT_INT_ENA_CH0_REG (DR_REG_H264_DMA_BASE + 0x8)
/** H264_DMA_OUT_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH0_INT_ENA    (BIT(0))
#define H264_DMA_OUT_DONE_CH0_INT_ENA_M  (H264_DMA_OUT_DONE_CH0_INT_ENA_V << H264_DMA_OUT_DONE_CH0_INT_ENA_S)
#define H264_DMA_OUT_DONE_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DONE_CH0_INT_ENA_S  0
/** H264_DMA_OUT_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH0_INT_ENA    (BIT(1))
#define H264_DMA_OUT_EOF_CH0_INT_ENA_M  (H264_DMA_OUT_EOF_CH0_INT_ENA_V << H264_DMA_OUT_EOF_CH0_INT_ENA_S)
#define H264_DMA_OUT_EOF_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_EOF_CH0_INT_ENA_S  1
/** H264_DMA_OUT_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ENA    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ENA_M  (H264_DMA_OUT_DSCR_ERR_CH0_INT_ENA_V << H264_DMA_OUT_DSCR_ERR_CH0_INT_ENA_S)
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ENA_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ENA    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_M  (H264_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_V << H264_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_S)
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ENA    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ENA_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ENA    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ENA_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ENA    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ENA_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ENA    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ENA_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ENA    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ENA_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ENA_V << H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ENA_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ENA_S  8

/** H264_DMA_OUT_INT_ST_CH0_REG register
 *  TX CH0  interrupt st register
 */
#define H264_DMA_OUT_INT_ST_CH0_REG (DR_REG_H264_DMA_BASE + 0xc)
/** H264_DMA_OUT_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH0_INT_ST    (BIT(0))
#define H264_DMA_OUT_DONE_CH0_INT_ST_M  (H264_DMA_OUT_DONE_CH0_INT_ST_V << H264_DMA_OUT_DONE_CH0_INT_ST_S)
#define H264_DMA_OUT_DONE_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DONE_CH0_INT_ST_S  0
/** H264_DMA_OUT_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH0_INT_ST    (BIT(1))
#define H264_DMA_OUT_EOF_CH0_INT_ST_M  (H264_DMA_OUT_EOF_CH0_INT_ST_V << H264_DMA_OUT_EOF_CH0_INT_ST_S)
#define H264_DMA_OUT_EOF_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUT_EOF_CH0_INT_ST_S  1
/** H264_DMA_OUT_DSCR_ERR_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ST    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ST_M  (H264_DMA_OUT_DSCR_ERR_CH0_INT_ST_V << H264_DMA_OUT_DSCR_ERR_CH0_INT_ST_S)
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_ST_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ST    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ST_M  (H264_DMA_OUT_TOTAL_EOF_CH0_INT_ST_V << H264_DMA_OUT_TOTAL_EOF_CH0_INT_ST_S)
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_ST_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ST    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ST_V << H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_ST_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ST    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ST_V << H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_ST_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ST    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ST_V << H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_ST_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ST    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ST_V << H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_ST_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ST    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ST_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ST_V << H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ST_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_ST_S  8

/** H264_DMA_OUT_INT_CLR_CH0_REG register
 *  TX CH0  interrupt clr register
 */
#define H264_DMA_OUT_INT_CLR_CH0_REG (DR_REG_H264_DMA_BASE + 0x10)
/** H264_DMA_OUT_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH0_INT_CLR    (BIT(0))
#define H264_DMA_OUT_DONE_CH0_INT_CLR_M  (H264_DMA_OUT_DONE_CH0_INT_CLR_V << H264_DMA_OUT_DONE_CH0_INT_CLR_S)
#define H264_DMA_OUT_DONE_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DONE_CH0_INT_CLR_S  0
/** H264_DMA_OUT_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH0_INT_CLR    (BIT(1))
#define H264_DMA_OUT_EOF_CH0_INT_CLR_M  (H264_DMA_OUT_EOF_CH0_INT_CLR_V << H264_DMA_OUT_EOF_CH0_INT_CLR_S)
#define H264_DMA_OUT_EOF_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_EOF_CH0_INT_CLR_S  1
/** H264_DMA_OUT_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_CLR    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_CLR_M  (H264_DMA_OUT_DSCR_ERR_CH0_INT_CLR_V << H264_DMA_OUT_DSCR_ERR_CH0_INT_CLR_S)
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH0_INT_CLR_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_CLR    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_M  (H264_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_V << H264_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_S)
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_CLR    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH0_INT_CLR_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_CLR    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH0_INT_CLR_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_CLR    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L2_CH0_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L2_CH0_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH0_INT_CLR_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_CLR    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L2_CH0_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L2_CH0_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH0_INT_CLR_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_CLR    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_CLR_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_CLR_V << H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_CLR_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH0_INT_CLR_S  8

/** H264_DMA_OUTFIFO_STATUS_CH0_REG register
 *  TX CH0 outfifo status register
 */
#define H264_DMA_OUTFIFO_STATUS_CH0_REG (DR_REG_H264_DMA_BASE + 0x14)
/** H264_DMA_OUTFIFO_FULL_L2_CH0 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_FULL_L2_CH0    (BIT(0))
#define H264_DMA_OUTFIFO_FULL_L2_CH0_M  (H264_DMA_OUTFIFO_FULL_L2_CH0_V << H264_DMA_OUTFIFO_FULL_L2_CH0_S)
#define H264_DMA_OUTFIFO_FULL_L2_CH0_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L2_CH0_S  0
/** H264_DMA_OUTFIFO_EMPTY_L2_CH0 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_EMPTY_L2_CH0    (BIT(1))
#define H264_DMA_OUTFIFO_EMPTY_L2_CH0_M  (H264_DMA_OUTFIFO_EMPTY_L2_CH0_V << H264_DMA_OUTFIFO_EMPTY_L2_CH0_S)
#define H264_DMA_OUTFIFO_EMPTY_L2_CH0_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L2_CH0_S  1
/** H264_DMA_OUTFIFO_CNT_L2_CH0 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_CNT_L2_CH0    0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH0_M  (H264_DMA_OUTFIFO_CNT_L2_CH0_V << H264_DMA_OUTFIFO_CNT_L2_CH0_S)
#define H264_DMA_OUTFIFO_CNT_L2_CH0_V  0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH0_S  2
/** H264_DMA_OUTFIFO_FULL_L1_CH0 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_FULL_L1_CH0    (BIT(6))
#define H264_DMA_OUTFIFO_FULL_L1_CH0_M  (H264_DMA_OUTFIFO_FULL_L1_CH0_V << H264_DMA_OUTFIFO_FULL_L1_CH0_S)
#define H264_DMA_OUTFIFO_FULL_L1_CH0_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L1_CH0_S  6
/** H264_DMA_OUTFIFO_EMPTY_L1_CH0 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_EMPTY_L1_CH0    (BIT(7))
#define H264_DMA_OUTFIFO_EMPTY_L1_CH0_M  (H264_DMA_OUTFIFO_EMPTY_L1_CH0_V << H264_DMA_OUTFIFO_EMPTY_L1_CH0_S)
#define H264_DMA_OUTFIFO_EMPTY_L1_CH0_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L1_CH0_S  7
/** H264_DMA_OUTFIFO_CNT_L1_CH0 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_CNT_L1_CH0    0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH0_M  (H264_DMA_OUTFIFO_CNT_L1_CH0_V << H264_DMA_OUTFIFO_CNT_L1_CH0_S)
#define H264_DMA_OUTFIFO_CNT_L1_CH0_V  0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH0_S  8
/** H264_DMA_OUTFIFO_FULL_L3_CH0 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_FULL_L3_CH0    (BIT(16))
#define H264_DMA_OUTFIFO_FULL_L3_CH0_M  (H264_DMA_OUTFIFO_FULL_L3_CH0_V << H264_DMA_OUTFIFO_FULL_L3_CH0_S)
#define H264_DMA_OUTFIFO_FULL_L3_CH0_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L3_CH0_S  16
/** H264_DMA_OUTFIFO_EMPTY_L3_CH0 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_EMPTY_L3_CH0    (BIT(17))
#define H264_DMA_OUTFIFO_EMPTY_L3_CH0_M  (H264_DMA_OUTFIFO_EMPTY_L3_CH0_V << H264_DMA_OUTFIFO_EMPTY_L3_CH0_S)
#define H264_DMA_OUTFIFO_EMPTY_L3_CH0_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L3_CH0_S  17
/** H264_DMA_OUTFIFO_CNT_L3_CH0 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the 8byte number of the data in Tx FIFO for Tx channel 0.
 */
#define H264_DMA_OUTFIFO_CNT_L3_CH0    0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH0_M  (H264_DMA_OUTFIFO_CNT_L3_CH0_V << H264_DMA_OUTFIFO_CNT_L3_CH0_S)
#define H264_DMA_OUTFIFO_CNT_L3_CH0_V  0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH0_S  18

/** H264_DMA_OUT_PUSH_CH0_REG register
 *  TX CH0 outfifo push register
 */
#define H264_DMA_OUT_PUSH_CH0_REG (DR_REG_H264_DMA_BASE + 0x18)
/** H264_DMA_OUTFIFO_WDATA_CH0 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_WDATA_CH0    0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH0_M  (H264_DMA_OUTFIFO_WDATA_CH0_V << H264_DMA_OUTFIFO_WDATA_CH0_S)
#define H264_DMA_OUTFIFO_WDATA_CH0_V  0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH0_S  0
/** H264_DMA_OUTFIFO_PUSH_CH0 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_PUSH_CH0    (BIT(10))
#define H264_DMA_OUTFIFO_PUSH_CH0_M  (H264_DMA_OUTFIFO_PUSH_CH0_V << H264_DMA_OUTFIFO_PUSH_CH0_S)
#define H264_DMA_OUTFIFO_PUSH_CH0_V  0x00000001U
#define H264_DMA_OUTFIFO_PUSH_CH0_S  10

/** H264_DMA_OUT_LINK_CONF_CH0_REG register
 *  TX CH0 out_link dscr ctrl register
 */
#define H264_DMA_OUT_LINK_CONF_CH0_REG (DR_REG_H264_DMA_BASE + 0x1c)
/** H264_DMA_OUTLINK_STOP_CH0 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_STOP_CH0    (BIT(20))
#define H264_DMA_OUTLINK_STOP_CH0_M  (H264_DMA_OUTLINK_STOP_CH0_V << H264_DMA_OUTLINK_STOP_CH0_S)
#define H264_DMA_OUTLINK_STOP_CH0_V  0x00000001U
#define H264_DMA_OUTLINK_STOP_CH0_S  20
/** H264_DMA_OUTLINK_START_CH0 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_START_CH0    (BIT(21))
#define H264_DMA_OUTLINK_START_CH0_M  (H264_DMA_OUTLINK_START_CH0_V << H264_DMA_OUTLINK_START_CH0_S)
#define H264_DMA_OUTLINK_START_CH0_V  0x00000001U
#define H264_DMA_OUTLINK_START_CH0_S  21
/** H264_DMA_OUTLINK_RESTART_CH0 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define H264_DMA_OUTLINK_RESTART_CH0    (BIT(22))
#define H264_DMA_OUTLINK_RESTART_CH0_M  (H264_DMA_OUTLINK_RESTART_CH0_V << H264_DMA_OUTLINK_RESTART_CH0_S)
#define H264_DMA_OUTLINK_RESTART_CH0_V  0x00000001U
#define H264_DMA_OUTLINK_RESTART_CH0_S  22
/** H264_DMA_OUTLINK_PARK_CH0 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define H264_DMA_OUTLINK_PARK_CH0    (BIT(23))
#define H264_DMA_OUTLINK_PARK_CH0_M  (H264_DMA_OUTLINK_PARK_CH0_V << H264_DMA_OUTLINK_PARK_CH0_S)
#define H264_DMA_OUTLINK_PARK_CH0_V  0x00000001U
#define H264_DMA_OUTLINK_PARK_CH0_S  23

/** H264_DMA_OUT_LINK_ADDR_CH0_REG register
 *  TX CH0 out_link dscr addr register
 */
#define H264_DMA_OUT_LINK_ADDR_CH0_REG (DR_REG_H264_DMA_BASE + 0x20)
/** H264_DMA_OUTLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_ADDR_CH0    0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH0_M  (H264_DMA_OUTLINK_ADDR_CH0_V << H264_DMA_OUTLINK_ADDR_CH0_S)
#define H264_DMA_OUTLINK_ADDR_CH0_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH0_S  0

/** H264_DMA_OUT_STATE_CH0_REG register
 *  TX CH0 state register
 */
#define H264_DMA_OUT_STATE_CH0_REG (DR_REG_H264_DMA_BASE + 0x24)
/** H264_DMA_OUTLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH0_M  (H264_DMA_OUTLINK_DSCR_ADDR_CH0_V << H264_DMA_OUTLINK_DSCR_ADDR_CH0_S)
#define H264_DMA_OUTLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH0_S  0
/** H264_DMA_OUT_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_OUT_DSCR_STATE_CH0    0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH0_M  (H264_DMA_OUT_DSCR_STATE_CH0_V << H264_DMA_OUT_DSCR_STATE_CH0_S)
#define H264_DMA_OUT_DSCR_STATE_CH0_V  0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH0_S  18
/** H264_DMA_OUT_STATE_CH0 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_OUT_STATE_CH0    0x0000000FU
#define H264_DMA_OUT_STATE_CH0_M  (H264_DMA_OUT_STATE_CH0_V << H264_DMA_OUT_STATE_CH0_S)
#define H264_DMA_OUT_STATE_CH0_V  0x0000000FU
#define H264_DMA_OUT_STATE_CH0_S  20
/** H264_DMA_OUT_RESET_AVAIL_CH0 : RO; bitpos: [24]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_OUT_RESET_AVAIL_CH0    (BIT(24))
#define H264_DMA_OUT_RESET_AVAIL_CH0_M  (H264_DMA_OUT_RESET_AVAIL_CH0_V << H264_DMA_OUT_RESET_AVAIL_CH0_S)
#define H264_DMA_OUT_RESET_AVAIL_CH0_V  0x00000001U
#define H264_DMA_OUT_RESET_AVAIL_CH0_S  24

/** H264_DMA_OUT_EOF_DES_ADDR_CH0_REG register
 *  TX CH0 eof des addr register
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH0_REG (DR_REG_H264_DMA_BASE + 0x28)
/** H264_DMA_OUT_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH0_M  (H264_DMA_OUT_EOF_DES_ADDR_CH0_V << H264_DMA_OUT_EOF_DES_ADDR_CH0_S)
#define H264_DMA_OUT_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH0_S  0

/** H264_DMA_OUT_DSCR_CH0_REG register
 *  TX CH0 next dscr addr register
 */
#define H264_DMA_OUT_DSCR_CH0_REG (DR_REG_H264_DMA_BASE + 0x2c)
/** H264_DMA_OUTLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define H264_DMA_OUTLINK_DSCR_CH0    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH0_M  (H264_DMA_OUTLINK_DSCR_CH0_V << H264_DMA_OUTLINK_DSCR_CH0_S)
#define H264_DMA_OUTLINK_DSCR_CH0_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH0_S  0

/** H264_DMA_OUT_DSCR_BF0_CH0_REG register
 *  TX CH0 last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF0_CH0_REG (DR_REG_H264_DMA_BASE + 0x30)
/** H264_DMA_OUTLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define H264_DMA_OUTLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH0_M  (H264_DMA_OUTLINK_DSCR_BF0_CH0_V << H264_DMA_OUTLINK_DSCR_BF0_CH0_S)
#define H264_DMA_OUTLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH0_S  0

/** H264_DMA_OUT_DSCR_BF1_CH0_REG register
 *  TX CH0 second-to-last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF1_CH0_REG (DR_REG_H264_DMA_BASE + 0x34)
/** H264_DMA_OUTLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define H264_DMA_OUTLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH0_M  (H264_DMA_OUTLINK_DSCR_BF1_CH0_V << H264_DMA_OUTLINK_DSCR_BF1_CH0_S)
#define H264_DMA_OUTLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH0_S  0

/** H264_DMA_OUT_ARB_CH0_REG register
 *  TX CH0 arb register
 */
#define H264_DMA_OUT_ARB_CH0_REG (DR_REG_H264_DMA_BASE + 0x3c)
/** H264_DMA_OUT_ARB_TOKEN_NUM_CH0 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH0    0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH0_M  (H264_DMA_OUT_ARB_TOKEN_NUM_CH0_V << H264_DMA_OUT_ARB_TOKEN_NUM_CH0_S)
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH0_V  0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH0_S  0
/** H264_DMA_EXTER_OUT_ARB_PRIORITY_CH0 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH0    0x00000003U
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH0_M  (H264_DMA_EXTER_OUT_ARB_PRIORITY_CH0_V << H264_DMA_EXTER_OUT_ARB_PRIORITY_CH0_S)
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH0_V  0x00000003U
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH0_S  4

/** H264_DMA_OUT_RO_STATUS_CH0_REG register
 *  TX CH0 reorder status register
 */
#define H264_DMA_OUT_RO_STATUS_CH0_REG (DR_REG_H264_DMA_BASE + 0x40)
/** H264_DMA_OUTFIFO_RO_CNT_CH0 : RO; bitpos: [1:0]; default: 0;
 *  The register stores the 8byte number of the data in reorder Tx FIFO for channel 0.
 */
#define H264_DMA_OUTFIFO_RO_CNT_CH0    0x00000003U
#define H264_DMA_OUTFIFO_RO_CNT_CH0_M  (H264_DMA_OUTFIFO_RO_CNT_CH0_V << H264_DMA_OUTFIFO_RO_CNT_CH0_S)
#define H264_DMA_OUTFIFO_RO_CNT_CH0_V  0x00000003U
#define H264_DMA_OUTFIFO_RO_CNT_CH0_S  0
/** H264_DMA_OUT_RO_WR_STATE_CH0 : RO; bitpos: [7:6]; default: 0;
 *  The register stores the state of read ram of reorder
 */
#define H264_DMA_OUT_RO_WR_STATE_CH0    0x00000003U
#define H264_DMA_OUT_RO_WR_STATE_CH0_M  (H264_DMA_OUT_RO_WR_STATE_CH0_V << H264_DMA_OUT_RO_WR_STATE_CH0_S)
#define H264_DMA_OUT_RO_WR_STATE_CH0_V  0x00000003U
#define H264_DMA_OUT_RO_WR_STATE_CH0_S  6
/** H264_DMA_OUT_RO_RD_STATE_CH0 : RO; bitpos: [9:8]; default: 0;
 *  The register stores the state of write ram of reorder
 */
#define H264_DMA_OUT_RO_RD_STATE_CH0    0x00000003U
#define H264_DMA_OUT_RO_RD_STATE_CH0_M  (H264_DMA_OUT_RO_RD_STATE_CH0_V << H264_DMA_OUT_RO_RD_STATE_CH0_S)
#define H264_DMA_OUT_RO_RD_STATE_CH0_V  0x00000003U
#define H264_DMA_OUT_RO_RD_STATE_CH0_S  8
/** H264_DMA_OUT_PIXEL_BYTE_CH0 : RO; bitpos: [13:10]; default: 2;
 *  the number of bytes contained in a pixel at TX channel     0: 1byte     1: 1.5bytes
 *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
 */
#define H264_DMA_OUT_PIXEL_BYTE_CH0    0x0000000FU
#define H264_DMA_OUT_PIXEL_BYTE_CH0_M  (H264_DMA_OUT_PIXEL_BYTE_CH0_V << H264_DMA_OUT_PIXEL_BYTE_CH0_S)
#define H264_DMA_OUT_PIXEL_BYTE_CH0_V  0x0000000FU
#define H264_DMA_OUT_PIXEL_BYTE_CH0_S  10
/** H264_DMA_OUT_BURST_BLOCK_NUM_CH0 : RO; bitpos: [17:14]; default: 0;
 *  the number of macro blocks contained in a burst of data at TX channel
 */
#define H264_DMA_OUT_BURST_BLOCK_NUM_CH0    0x0000000FU
#define H264_DMA_OUT_BURST_BLOCK_NUM_CH0_M  (H264_DMA_OUT_BURST_BLOCK_NUM_CH0_V << H264_DMA_OUT_BURST_BLOCK_NUM_CH0_S)
#define H264_DMA_OUT_BURST_BLOCK_NUM_CH0_V  0x0000000FU
#define H264_DMA_OUT_BURST_BLOCK_NUM_CH0_S  14

/** H264_DMA_OUT_RO_PD_CONF_CH0_REG register
 *  TX CH0 reorder power config register
 */
#define H264_DMA_OUT_RO_PD_CONF_CH0_REG (DR_REG_H264_DMA_BASE + 0x44)
/** H264_DMA_OUT_RO_RAM_FORCE_PD_CH0 : R/W; bitpos: [4]; default: 0;
 *  dma reorder ram power down
 */
#define H264_DMA_OUT_RO_RAM_FORCE_PD_CH0    (BIT(4))
#define H264_DMA_OUT_RO_RAM_FORCE_PD_CH0_M  (H264_DMA_OUT_RO_RAM_FORCE_PD_CH0_V << H264_DMA_OUT_RO_RAM_FORCE_PD_CH0_S)
#define H264_DMA_OUT_RO_RAM_FORCE_PD_CH0_V  0x00000001U
#define H264_DMA_OUT_RO_RAM_FORCE_PD_CH0_S  4
/** H264_DMA_OUT_RO_RAM_FORCE_PU_CH0 : R/W; bitpos: [5]; default: 1;
 *  dma reorder ram power up
 */
#define H264_DMA_OUT_RO_RAM_FORCE_PU_CH0    (BIT(5))
#define H264_DMA_OUT_RO_RAM_FORCE_PU_CH0_M  (H264_DMA_OUT_RO_RAM_FORCE_PU_CH0_V << H264_DMA_OUT_RO_RAM_FORCE_PU_CH0_S)
#define H264_DMA_OUT_RO_RAM_FORCE_PU_CH0_V  0x00000001U
#define H264_DMA_OUT_RO_RAM_FORCE_PU_CH0_S  5
/** H264_DMA_OUT_RO_RAM_CLK_FO_CH0 : R/W; bitpos: [6]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
 *  0: A gate-clock will be used when accessing the RAM in DMA.
 */
#define H264_DMA_OUT_RO_RAM_CLK_FO_CH0    (BIT(6))
#define H264_DMA_OUT_RO_RAM_CLK_FO_CH0_M  (H264_DMA_OUT_RO_RAM_CLK_FO_CH0_V << H264_DMA_OUT_RO_RAM_CLK_FO_CH0_S)
#define H264_DMA_OUT_RO_RAM_CLK_FO_CH0_V  0x00000001U
#define H264_DMA_OUT_RO_RAM_CLK_FO_CH0_S  6

/** H264_DMA_OUT_MODE_ENABLE_CH0_REG register
 *  tx CH0 mode enable register
 */
#define H264_DMA_OUT_MODE_ENABLE_CH0_REG (DR_REG_H264_DMA_BASE + 0x50)
/** H264_DMA_OUT_TEST_MODE_ENABLE_CH0 : R/W; bitpos: [0]; default: 0;
 *  tx CH0 test mode enable.0 : H264_DMA work in normal mode.1 : H264_DMA work in test
 *  mode
 */
#define H264_DMA_OUT_TEST_MODE_ENABLE_CH0    (BIT(0))
#define H264_DMA_OUT_TEST_MODE_ENABLE_CH0_M  (H264_DMA_OUT_TEST_MODE_ENABLE_CH0_V << H264_DMA_OUT_TEST_MODE_ENABLE_CH0_S)
#define H264_DMA_OUT_TEST_MODE_ENABLE_CH0_V  0x00000001U
#define H264_DMA_OUT_TEST_MODE_ENABLE_CH0_S  0

/** H264_DMA_OUT_MODE_YUV_CH0_REG register
 *  tx CH0 test mode yuv value register
 */
#define H264_DMA_OUT_MODE_YUV_CH0_REG (DR_REG_H264_DMA_BASE + 0x54)
/** H264_DMA_OUT_TEST_Y_VALUE_CH0 : R/W; bitpos: [7:0]; default: 0;
 *  tx CH0 test mode y value
 */
#define H264_DMA_OUT_TEST_Y_VALUE_CH0    0x000000FFU
#define H264_DMA_OUT_TEST_Y_VALUE_CH0_M  (H264_DMA_OUT_TEST_Y_VALUE_CH0_V << H264_DMA_OUT_TEST_Y_VALUE_CH0_S)
#define H264_DMA_OUT_TEST_Y_VALUE_CH0_V  0x000000FFU
#define H264_DMA_OUT_TEST_Y_VALUE_CH0_S  0
/** H264_DMA_OUT_TEST_U_VALUE_CH0 : R/W; bitpos: [15:8]; default: 0;
 *  tx CH0 test mode u value
 */
#define H264_DMA_OUT_TEST_U_VALUE_CH0    0x000000FFU
#define H264_DMA_OUT_TEST_U_VALUE_CH0_M  (H264_DMA_OUT_TEST_U_VALUE_CH0_V << H264_DMA_OUT_TEST_U_VALUE_CH0_S)
#define H264_DMA_OUT_TEST_U_VALUE_CH0_V  0x000000FFU
#define H264_DMA_OUT_TEST_U_VALUE_CH0_S  8
/** H264_DMA_OUT_TEST_V_VALUE_CH0 : R/W; bitpos: [23:16]; default: 0;
 *  tx CH0 test mode v value
 */
#define H264_DMA_OUT_TEST_V_VALUE_CH0    0x000000FFU
#define H264_DMA_OUT_TEST_V_VALUE_CH0_M  (H264_DMA_OUT_TEST_V_VALUE_CH0_V << H264_DMA_OUT_TEST_V_VALUE_CH0_S)
#define H264_DMA_OUT_TEST_V_VALUE_CH0_V  0x000000FFU
#define H264_DMA_OUT_TEST_V_VALUE_CH0_S  16

/** H264_DMA_OUT_ETM_CONF_CH0_REG register
 *  TX CH0 ETM config register
 */
#define H264_DMA_OUT_ETM_CONF_CH0_REG (DR_REG_H264_DMA_BASE + 0x68)
/** H264_DMA_OUT_ETM_EN_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_OUT_ETM_EN_CH0    (BIT(0))
#define H264_DMA_OUT_ETM_EN_CH0_M  (H264_DMA_OUT_ETM_EN_CH0_V << H264_DMA_OUT_ETM_EN_CH0_S)
#define H264_DMA_OUT_ETM_EN_CH0_V  0x00000001U
#define H264_DMA_OUT_ETM_EN_CH0_S  0
/** H264_DMA_OUT_ETM_LOOP_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_OUT_ETM_LOOP_EN_CH0    (BIT(1))
#define H264_DMA_OUT_ETM_LOOP_EN_CH0_M  (H264_DMA_OUT_ETM_LOOP_EN_CH0_V << H264_DMA_OUT_ETM_LOOP_EN_CH0_S)
#define H264_DMA_OUT_ETM_LOOP_EN_CH0_V  0x00000001U
#define H264_DMA_OUT_ETM_LOOP_EN_CH0_S  1
/** H264_DMA_OUT_DSCR_TASK_MAK_CH0 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_OUT_DSCR_TASK_MAK_CH0    0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH0_M  (H264_DMA_OUT_DSCR_TASK_MAK_CH0_V << H264_DMA_OUT_DSCR_TASK_MAK_CH0_S)
#define H264_DMA_OUT_DSCR_TASK_MAK_CH0_V  0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH0_S  2

/** H264_DMA_OUT_BUF_LEN_CH0_REG register
 *  tx CH0 buf len register
 */
#define H264_DMA_OUT_BUF_LEN_CH0_REG (DR_REG_H264_DMA_BASE + 0x70)
/** H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH0 : RO; bitpos: [12:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH0    0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH0_M  (H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH0_V << H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH0_S)
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH0_V  0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH0_S  0

/** H264_DMA_OUT_FIFO_BCNT_CH0_REG register
 *  tx CH0 fifo byte cnt register
 */
#define H264_DMA_OUT_FIFO_BCNT_CH0_REG (DR_REG_H264_DMA_BASE + 0x74)
/** H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH0 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH0    0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH0_M  (H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH0_V << H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH0_S)
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH0_V  0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH0_S  0

/** H264_DMA_OUT_PUSH_BYTECNT_CH0_REG register
 *  tx CH0 push byte cnt register
 */
#define H264_DMA_OUT_PUSH_BYTECNT_CH0_REG (DR_REG_H264_DMA_BASE + 0x78)
/** H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH0 : RO; bitpos: [7:0]; default: 255;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH0    0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH0_M  (H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH0_V << H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH0_S)
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH0_V  0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH0_S  0

/** H264_DMA_OUT_XADDR_CH0_REG register
 *  tx CH0 xaddr register
 */
#define H264_DMA_OUT_XADDR_CH0_REG (DR_REG_H264_DMA_BASE + 0x7c)
/** H264_DMA_OUT_CMDFIFO_XADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_XADDR_CH0    0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH0_M  (H264_DMA_OUT_CMDFIFO_XADDR_CH0_V << H264_DMA_OUT_CMDFIFO_XADDR_CH0_S)
#define H264_DMA_OUT_CMDFIFO_XADDR_CH0_V  0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH0_S  0

/** H264_DMA_OUT_CONF0_CH1_REG register
 *  TX CH1 config0 register
 */
#define H264_DMA_OUT_CONF0_CH1_REG (DR_REG_H264_DMA_BASE + 0x100)
/** H264_DMA_OUT_AUTO_WRBACK_CH1 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define H264_DMA_OUT_AUTO_WRBACK_CH1    (BIT(0))
#define H264_DMA_OUT_AUTO_WRBACK_CH1_M  (H264_DMA_OUT_AUTO_WRBACK_CH1_V << H264_DMA_OUT_AUTO_WRBACK_CH1_S)
#define H264_DMA_OUT_AUTO_WRBACK_CH1_V  0x00000001U
#define H264_DMA_OUT_AUTO_WRBACK_CH1_S  0
/** H264_DMA_OUT_EOF_MODE_CH1 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define H264_DMA_OUT_EOF_MODE_CH1    (BIT(1))
#define H264_DMA_OUT_EOF_MODE_CH1_M  (H264_DMA_OUT_EOF_MODE_CH1_V << H264_DMA_OUT_EOF_MODE_CH1_S)
#define H264_DMA_OUT_EOF_MODE_CH1_V  0x00000001U
#define H264_DMA_OUT_EOF_MODE_CH1_S  1
/** H264_DMA_OUTDSCR_BURST_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define H264_DMA_OUTDSCR_BURST_EN_CH1    (BIT(2))
#define H264_DMA_OUTDSCR_BURST_EN_CH1_M  (H264_DMA_OUTDSCR_BURST_EN_CH1_V << H264_DMA_OUTDSCR_BURST_EN_CH1_S)
#define H264_DMA_OUTDSCR_BURST_EN_CH1_V  0x00000001U
#define H264_DMA_OUTDSCR_BURST_EN_CH1_S  2
/** H264_DMA_OUT_ECC_AES_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_OUT_ECC_AES_EN_CH1    (BIT(3))
#define H264_DMA_OUT_ECC_AES_EN_CH1_M  (H264_DMA_OUT_ECC_AES_EN_CH1_V << H264_DMA_OUT_ECC_AES_EN_CH1_S)
#define H264_DMA_OUT_ECC_AES_EN_CH1_V  0x00000001U
#define H264_DMA_OUT_ECC_AES_EN_CH1_S  3
/** H264_DMA_OUT_CHECK_OWNER_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_OUT_CHECK_OWNER_CH1    (BIT(4))
#define H264_DMA_OUT_CHECK_OWNER_CH1_M  (H264_DMA_OUT_CHECK_OWNER_CH1_V << H264_DMA_OUT_CHECK_OWNER_CH1_S)
#define H264_DMA_OUT_CHECK_OWNER_CH1_V  0x00000001U
#define H264_DMA_OUT_CHECK_OWNER_CH1_S  4
/** H264_DMA_OUT_MEM_BURST_LENGTH_CH1 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 64 bytes
 */
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH1    0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH1_M  (H264_DMA_OUT_MEM_BURST_LENGTH_CH1_V << H264_DMA_OUT_MEM_BURST_LENGTH_CH1_S)
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH1_V  0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH1_S  6
/** H264_DMA_OUT_PAGE_BOUND_EN_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define H264_DMA_OUT_PAGE_BOUND_EN_CH1    (BIT(12))
#define H264_DMA_OUT_PAGE_BOUND_EN_CH1_M  (H264_DMA_OUT_PAGE_BOUND_EN_CH1_V << H264_DMA_OUT_PAGE_BOUND_EN_CH1_S)
#define H264_DMA_OUT_PAGE_BOUND_EN_CH1_V  0x00000001U
#define H264_DMA_OUT_PAGE_BOUND_EN_CH1_S  12
/** H264_DMA_OUT_RST_CH1 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset TX channel
 */
#define H264_DMA_OUT_RST_CH1    (BIT(24))
#define H264_DMA_OUT_RST_CH1_M  (H264_DMA_OUT_RST_CH1_V << H264_DMA_OUT_RST_CH1_S)
#define H264_DMA_OUT_RST_CH1_V  0x00000001U
#define H264_DMA_OUT_RST_CH1_S  24
/** H264_DMA_OUT_CMD_DISABLE_CH1 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_OUT_CMD_DISABLE_CH1    (BIT(25))
#define H264_DMA_OUT_CMD_DISABLE_CH1_M  (H264_DMA_OUT_CMD_DISABLE_CH1_V << H264_DMA_OUT_CMD_DISABLE_CH1_S)
#define H264_DMA_OUT_CMD_DISABLE_CH1_V  0x00000001U
#define H264_DMA_OUT_CMD_DISABLE_CH1_S  25
/** H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH1 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH1    (BIT(26))
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH1_M  (H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH1_V << H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH1_S)
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH1_V  0x00000001U
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH1_S  26

/** H264_DMA_OUT_INT_RAW_CH1_REG register
 *  TX CH1  interrupt raw register
 */
#define H264_DMA_OUT_INT_RAW_CH1_REG (DR_REG_H264_DMA_BASE + 0x104)
/** H264_DMA_OUT_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define H264_DMA_OUT_DONE_CH1_INT_RAW    (BIT(0))
#define H264_DMA_OUT_DONE_CH1_INT_RAW_M  (H264_DMA_OUT_DONE_CH1_INT_RAW_V << H264_DMA_OUT_DONE_CH1_INT_RAW_S)
#define H264_DMA_OUT_DONE_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DONE_CH1_INT_RAW_S  0
/** H264_DMA_OUT_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define H264_DMA_OUT_EOF_CH1_INT_RAW    (BIT(1))
#define H264_DMA_OUT_EOF_CH1_INT_RAW_M  (H264_DMA_OUT_EOF_CH1_INT_RAW_V << H264_DMA_OUT_EOF_CH1_INT_RAW_S)
#define H264_DMA_OUT_EOF_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_EOF_CH1_INT_RAW_S  1
/** H264_DMA_OUT_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_RAW    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_RAW_M  (H264_DMA_OUT_DSCR_ERR_CH1_INT_RAW_V << H264_DMA_OUT_DSCR_ERR_CH1_INT_RAW_S)
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_RAW_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_RAW    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_M  (H264_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_V << H264_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_S)
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_RAW    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_RAW_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_RAW    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_RAW_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_RAW    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L2_CH1_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L2_CH1_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_RAW_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_RAW    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L2_CH1_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L2_CH1_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_RAW_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_RAW    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_RAW_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_RAW_V << H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_RAW_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_RAW_S  8

/** H264_DMA_OUT_INT_ENA_CH1_REG register
 *  TX CH1  interrupt ena register
 */
#define H264_DMA_OUT_INT_ENA_CH1_REG (DR_REG_H264_DMA_BASE + 0x108)
/** H264_DMA_OUT_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH1_INT_ENA    (BIT(0))
#define H264_DMA_OUT_DONE_CH1_INT_ENA_M  (H264_DMA_OUT_DONE_CH1_INT_ENA_V << H264_DMA_OUT_DONE_CH1_INT_ENA_S)
#define H264_DMA_OUT_DONE_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DONE_CH1_INT_ENA_S  0
/** H264_DMA_OUT_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH1_INT_ENA    (BIT(1))
#define H264_DMA_OUT_EOF_CH1_INT_ENA_M  (H264_DMA_OUT_EOF_CH1_INT_ENA_V << H264_DMA_OUT_EOF_CH1_INT_ENA_S)
#define H264_DMA_OUT_EOF_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_EOF_CH1_INT_ENA_S  1
/** H264_DMA_OUT_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ENA    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ENA_M  (H264_DMA_OUT_DSCR_ERR_CH1_INT_ENA_V << H264_DMA_OUT_DSCR_ERR_CH1_INT_ENA_S)
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ENA_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ENA    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_M  (H264_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_V << H264_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_S)
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ENA    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ENA_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ENA    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ENA_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ENA    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ENA_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ENA    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ENA_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ENA    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ENA_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ENA_V << H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ENA_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ENA_S  8

/** H264_DMA_OUT_INT_ST_CH1_REG register
 *  TX CH1  interrupt st register
 */
#define H264_DMA_OUT_INT_ST_CH1_REG (DR_REG_H264_DMA_BASE + 0x10c)
/** H264_DMA_OUT_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH1_INT_ST    (BIT(0))
#define H264_DMA_OUT_DONE_CH1_INT_ST_M  (H264_DMA_OUT_DONE_CH1_INT_ST_V << H264_DMA_OUT_DONE_CH1_INT_ST_S)
#define H264_DMA_OUT_DONE_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DONE_CH1_INT_ST_S  0
/** H264_DMA_OUT_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH1_INT_ST    (BIT(1))
#define H264_DMA_OUT_EOF_CH1_INT_ST_M  (H264_DMA_OUT_EOF_CH1_INT_ST_V << H264_DMA_OUT_EOF_CH1_INT_ST_S)
#define H264_DMA_OUT_EOF_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUT_EOF_CH1_INT_ST_S  1
/** H264_DMA_OUT_DSCR_ERR_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ST    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ST_M  (H264_DMA_OUT_DSCR_ERR_CH1_INT_ST_V << H264_DMA_OUT_DSCR_ERR_CH1_INT_ST_S)
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_ST_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ST    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ST_M  (H264_DMA_OUT_TOTAL_EOF_CH1_INT_ST_V << H264_DMA_OUT_TOTAL_EOF_CH1_INT_ST_S)
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_ST_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ST    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ST_V << H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_ST_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ST    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ST_V << H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_ST_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ST    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ST_V << H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_ST_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ST    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ST_V << H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_ST_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ST    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ST_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ST_V << H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ST_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_ST_S  8

/** H264_DMA_OUT_INT_CLR_CH1_REG register
 *  TX CH1  interrupt clr register
 */
#define H264_DMA_OUT_INT_CLR_CH1_REG (DR_REG_H264_DMA_BASE + 0x110)
/** H264_DMA_OUT_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH1_INT_CLR    (BIT(0))
#define H264_DMA_OUT_DONE_CH1_INT_CLR_M  (H264_DMA_OUT_DONE_CH1_INT_CLR_V << H264_DMA_OUT_DONE_CH1_INT_CLR_S)
#define H264_DMA_OUT_DONE_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DONE_CH1_INT_CLR_S  0
/** H264_DMA_OUT_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH1_INT_CLR    (BIT(1))
#define H264_DMA_OUT_EOF_CH1_INT_CLR_M  (H264_DMA_OUT_EOF_CH1_INT_CLR_V << H264_DMA_OUT_EOF_CH1_INT_CLR_S)
#define H264_DMA_OUT_EOF_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_EOF_CH1_INT_CLR_S  1
/** H264_DMA_OUT_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_CLR    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_CLR_M  (H264_DMA_OUT_DSCR_ERR_CH1_INT_CLR_V << H264_DMA_OUT_DSCR_ERR_CH1_INT_CLR_S)
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH1_INT_CLR_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_CLR    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_M  (H264_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_V << H264_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_S)
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_CLR    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH1_INT_CLR_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_CLR    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH1_INT_CLR_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_CLR    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L2_CH1_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L2_CH1_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH1_INT_CLR_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH1_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_CLR    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L2_CH1_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L2_CH1_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH1_INT_CLR_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_CLR    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_CLR_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_CLR_V << H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_CLR_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH1_INT_CLR_S  8

/** H264_DMA_OUTFIFO_STATUS_CH1_REG register
 *  TX CH1 outfifo status register
 */
#define H264_DMA_OUTFIFO_STATUS_CH1_REG (DR_REG_H264_DMA_BASE + 0x114)
/** H264_DMA_OUTFIFO_FULL_L2_CH1 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_FULL_L2_CH1    (BIT(0))
#define H264_DMA_OUTFIFO_FULL_L2_CH1_M  (H264_DMA_OUTFIFO_FULL_L2_CH1_V << H264_DMA_OUTFIFO_FULL_L2_CH1_S)
#define H264_DMA_OUTFIFO_FULL_L2_CH1_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L2_CH1_S  0
/** H264_DMA_OUTFIFO_EMPTY_L2_CH1 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_EMPTY_L2_CH1    (BIT(1))
#define H264_DMA_OUTFIFO_EMPTY_L2_CH1_M  (H264_DMA_OUTFIFO_EMPTY_L2_CH1_V << H264_DMA_OUTFIFO_EMPTY_L2_CH1_S)
#define H264_DMA_OUTFIFO_EMPTY_L2_CH1_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L2_CH1_S  1
/** H264_DMA_OUTFIFO_CNT_L2_CH1 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_CNT_L2_CH1    0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH1_M  (H264_DMA_OUTFIFO_CNT_L2_CH1_V << H264_DMA_OUTFIFO_CNT_L2_CH1_S)
#define H264_DMA_OUTFIFO_CNT_L2_CH1_V  0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH1_S  2
/** H264_DMA_OUTFIFO_FULL_L1_CH1 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_FULL_L1_CH1    (BIT(6))
#define H264_DMA_OUTFIFO_FULL_L1_CH1_M  (H264_DMA_OUTFIFO_FULL_L1_CH1_V << H264_DMA_OUTFIFO_FULL_L1_CH1_S)
#define H264_DMA_OUTFIFO_FULL_L1_CH1_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L1_CH1_S  6
/** H264_DMA_OUTFIFO_EMPTY_L1_CH1 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_EMPTY_L1_CH1    (BIT(7))
#define H264_DMA_OUTFIFO_EMPTY_L1_CH1_M  (H264_DMA_OUTFIFO_EMPTY_L1_CH1_V << H264_DMA_OUTFIFO_EMPTY_L1_CH1_S)
#define H264_DMA_OUTFIFO_EMPTY_L1_CH1_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L1_CH1_S  7
/** H264_DMA_OUTFIFO_CNT_L1_CH1 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_CNT_L1_CH1    0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH1_M  (H264_DMA_OUTFIFO_CNT_L1_CH1_V << H264_DMA_OUTFIFO_CNT_L1_CH1_S)
#define H264_DMA_OUTFIFO_CNT_L1_CH1_V  0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH1_S  8
/** H264_DMA_OUTFIFO_FULL_L3_CH1 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_FULL_L3_CH1    (BIT(16))
#define H264_DMA_OUTFIFO_FULL_L3_CH1_M  (H264_DMA_OUTFIFO_FULL_L3_CH1_V << H264_DMA_OUTFIFO_FULL_L3_CH1_S)
#define H264_DMA_OUTFIFO_FULL_L3_CH1_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L3_CH1_S  16
/** H264_DMA_OUTFIFO_EMPTY_L3_CH1 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_EMPTY_L3_CH1    (BIT(17))
#define H264_DMA_OUTFIFO_EMPTY_L3_CH1_M  (H264_DMA_OUTFIFO_EMPTY_L3_CH1_V << H264_DMA_OUTFIFO_EMPTY_L3_CH1_S)
#define H264_DMA_OUTFIFO_EMPTY_L3_CH1_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L3_CH1_S  17
/** H264_DMA_OUTFIFO_CNT_L3_CH1 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_OUTFIFO_CNT_L3_CH1    0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH1_M  (H264_DMA_OUTFIFO_CNT_L3_CH1_V << H264_DMA_OUTFIFO_CNT_L3_CH1_S)
#define H264_DMA_OUTFIFO_CNT_L3_CH1_V  0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH1_S  18

/** H264_DMA_OUT_PUSH_CH1_REG register
 *  TX CH1 outfifo push register
 */
#define H264_DMA_OUT_PUSH_CH1_REG (DR_REG_H264_DMA_BASE + 0x118)
/** H264_DMA_OUTFIFO_WDATA_CH1 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_WDATA_CH1    0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH1_M  (H264_DMA_OUTFIFO_WDATA_CH1_V << H264_DMA_OUTFIFO_WDATA_CH1_S)
#define H264_DMA_OUTFIFO_WDATA_CH1_V  0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH1_S  0
/** H264_DMA_OUTFIFO_PUSH_CH1 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_PUSH_CH1    (BIT(10))
#define H264_DMA_OUTFIFO_PUSH_CH1_M  (H264_DMA_OUTFIFO_PUSH_CH1_V << H264_DMA_OUTFIFO_PUSH_CH1_S)
#define H264_DMA_OUTFIFO_PUSH_CH1_V  0x00000001U
#define H264_DMA_OUTFIFO_PUSH_CH1_S  10

/** H264_DMA_OUT_LINK_CONF_CH1_REG register
 *  TX CH1 out_link dscr ctrl register
 */
#define H264_DMA_OUT_LINK_CONF_CH1_REG (DR_REG_H264_DMA_BASE + 0x11c)
/** H264_DMA_OUTLINK_STOP_CH1 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_STOP_CH1    (BIT(20))
#define H264_DMA_OUTLINK_STOP_CH1_M  (H264_DMA_OUTLINK_STOP_CH1_V << H264_DMA_OUTLINK_STOP_CH1_S)
#define H264_DMA_OUTLINK_STOP_CH1_V  0x00000001U
#define H264_DMA_OUTLINK_STOP_CH1_S  20
/** H264_DMA_OUTLINK_START_CH1 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_START_CH1    (BIT(21))
#define H264_DMA_OUTLINK_START_CH1_M  (H264_DMA_OUTLINK_START_CH1_V << H264_DMA_OUTLINK_START_CH1_S)
#define H264_DMA_OUTLINK_START_CH1_V  0x00000001U
#define H264_DMA_OUTLINK_START_CH1_S  21
/** H264_DMA_OUTLINK_RESTART_CH1 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define H264_DMA_OUTLINK_RESTART_CH1    (BIT(22))
#define H264_DMA_OUTLINK_RESTART_CH1_M  (H264_DMA_OUTLINK_RESTART_CH1_V << H264_DMA_OUTLINK_RESTART_CH1_S)
#define H264_DMA_OUTLINK_RESTART_CH1_V  0x00000001U
#define H264_DMA_OUTLINK_RESTART_CH1_S  22
/** H264_DMA_OUTLINK_PARK_CH1 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define H264_DMA_OUTLINK_PARK_CH1    (BIT(23))
#define H264_DMA_OUTLINK_PARK_CH1_M  (H264_DMA_OUTLINK_PARK_CH1_V << H264_DMA_OUTLINK_PARK_CH1_S)
#define H264_DMA_OUTLINK_PARK_CH1_V  0x00000001U
#define H264_DMA_OUTLINK_PARK_CH1_S  23

/** H264_DMA_OUT_LINK_ADDR_CH1_REG register
 *  TX CH1 out_link dscr addr register
 */
#define H264_DMA_OUT_LINK_ADDR_CH1_REG (DR_REG_H264_DMA_BASE + 0x120)
/** H264_DMA_OUTLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_ADDR_CH1    0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH1_M  (H264_DMA_OUTLINK_ADDR_CH1_V << H264_DMA_OUTLINK_ADDR_CH1_S)
#define H264_DMA_OUTLINK_ADDR_CH1_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH1_S  0

/** H264_DMA_OUT_STATE_CH1_REG register
 *  TX CH1 state register
 */
#define H264_DMA_OUT_STATE_CH1_REG (DR_REG_H264_DMA_BASE + 0x124)
/** H264_DMA_OUTLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH1_M  (H264_DMA_OUTLINK_DSCR_ADDR_CH1_V << H264_DMA_OUTLINK_DSCR_ADDR_CH1_S)
#define H264_DMA_OUTLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH1_S  0
/** H264_DMA_OUT_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_OUT_DSCR_STATE_CH1    0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH1_M  (H264_DMA_OUT_DSCR_STATE_CH1_V << H264_DMA_OUT_DSCR_STATE_CH1_S)
#define H264_DMA_OUT_DSCR_STATE_CH1_V  0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH1_S  18
/** H264_DMA_OUT_STATE_CH1 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_OUT_STATE_CH1    0x0000000FU
#define H264_DMA_OUT_STATE_CH1_M  (H264_DMA_OUT_STATE_CH1_V << H264_DMA_OUT_STATE_CH1_S)
#define H264_DMA_OUT_STATE_CH1_V  0x0000000FU
#define H264_DMA_OUT_STATE_CH1_S  20
/** H264_DMA_OUT_RESET_AVAIL_CH1 : RO; bitpos: [24]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_OUT_RESET_AVAIL_CH1    (BIT(24))
#define H264_DMA_OUT_RESET_AVAIL_CH1_M  (H264_DMA_OUT_RESET_AVAIL_CH1_V << H264_DMA_OUT_RESET_AVAIL_CH1_S)
#define H264_DMA_OUT_RESET_AVAIL_CH1_V  0x00000001U
#define H264_DMA_OUT_RESET_AVAIL_CH1_S  24

/** H264_DMA_OUT_EOF_DES_ADDR_CH1_REG register
 *  TX CH1 eof des addr register
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH1_REG (DR_REG_H264_DMA_BASE + 0x128)
/** H264_DMA_OUT_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH1_M  (H264_DMA_OUT_EOF_DES_ADDR_CH1_V << H264_DMA_OUT_EOF_DES_ADDR_CH1_S)
#define H264_DMA_OUT_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH1_S  0

/** H264_DMA_OUT_DSCR_CH1_REG register
 *  TX CH1 next dscr addr register
 */
#define H264_DMA_OUT_DSCR_CH1_REG (DR_REG_H264_DMA_BASE + 0x12c)
/** H264_DMA_OUTLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define H264_DMA_OUTLINK_DSCR_CH1    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH1_M  (H264_DMA_OUTLINK_DSCR_CH1_V << H264_DMA_OUTLINK_DSCR_CH1_S)
#define H264_DMA_OUTLINK_DSCR_CH1_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH1_S  0

/** H264_DMA_OUT_DSCR_BF0_CH1_REG register
 *  TX CH1 last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF0_CH1_REG (DR_REG_H264_DMA_BASE + 0x130)
/** H264_DMA_OUTLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define H264_DMA_OUTLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH1_M  (H264_DMA_OUTLINK_DSCR_BF0_CH1_V << H264_DMA_OUTLINK_DSCR_BF0_CH1_S)
#define H264_DMA_OUTLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH1_S  0

/** H264_DMA_OUT_DSCR_BF1_CH1_REG register
 *  TX CH1 second-to-last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF1_CH1_REG (DR_REG_H264_DMA_BASE + 0x134)
/** H264_DMA_OUTLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define H264_DMA_OUTLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH1_M  (H264_DMA_OUTLINK_DSCR_BF1_CH1_V << H264_DMA_OUTLINK_DSCR_BF1_CH1_S)
#define H264_DMA_OUTLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH1_S  0

/** H264_DMA_OUT_ARB_CH1_REG register
 *  TX CH1 arb register
 */
#define H264_DMA_OUT_ARB_CH1_REG (DR_REG_H264_DMA_BASE + 0x13c)
/** H264_DMA_OUT_ARB_TOKEN_NUM_CH1 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH1    0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH1_M  (H264_DMA_OUT_ARB_TOKEN_NUM_CH1_V << H264_DMA_OUT_ARB_TOKEN_NUM_CH1_S)
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH1_V  0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH1_S  0
/** H264_DMA_INTER_OUT_ARB_PRIORITY_CH1 : R/W; bitpos: [6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH1    (BIT(6))
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH1_M  (H264_DMA_INTER_OUT_ARB_PRIORITY_CH1_V << H264_DMA_INTER_OUT_ARB_PRIORITY_CH1_S)
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH1_V  0x00000001U
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH1_S  6

/** H264_DMA_OUT_ETM_CONF_CH1_REG register
 *  TX CH1 ETM config register
 */
#define H264_DMA_OUT_ETM_CONF_CH1_REG (DR_REG_H264_DMA_BASE + 0x168)
/** H264_DMA_OUT_ETM_EN_CH1 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_OUT_ETM_EN_CH1    (BIT(0))
#define H264_DMA_OUT_ETM_EN_CH1_M  (H264_DMA_OUT_ETM_EN_CH1_V << H264_DMA_OUT_ETM_EN_CH1_S)
#define H264_DMA_OUT_ETM_EN_CH1_V  0x00000001U
#define H264_DMA_OUT_ETM_EN_CH1_S  0
/** H264_DMA_OUT_ETM_LOOP_EN_CH1 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_OUT_ETM_LOOP_EN_CH1    (BIT(1))
#define H264_DMA_OUT_ETM_LOOP_EN_CH1_M  (H264_DMA_OUT_ETM_LOOP_EN_CH1_V << H264_DMA_OUT_ETM_LOOP_EN_CH1_S)
#define H264_DMA_OUT_ETM_LOOP_EN_CH1_V  0x00000001U
#define H264_DMA_OUT_ETM_LOOP_EN_CH1_S  1
/** H264_DMA_OUT_DSCR_TASK_MAK_CH1 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_OUT_DSCR_TASK_MAK_CH1    0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH1_M  (H264_DMA_OUT_DSCR_TASK_MAK_CH1_V << H264_DMA_OUT_DSCR_TASK_MAK_CH1_S)
#define H264_DMA_OUT_DSCR_TASK_MAK_CH1_V  0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH1_S  2

/** H264_DMA_OUT_BUF_LEN_CH1_REG register
 *  tx CH1 buf len register
 */
#define H264_DMA_OUT_BUF_LEN_CH1_REG (DR_REG_H264_DMA_BASE + 0x170)
/** H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH1 : RO; bitpos: [12:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH1    0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH1_M  (H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH1_V << H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH1_S)
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH1_V  0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH1_S  0

/** H264_DMA_OUT_FIFO_BCNT_CH1_REG register
 *  tx CH1 fifo byte cnt register
 */
#define H264_DMA_OUT_FIFO_BCNT_CH1_REG (DR_REG_H264_DMA_BASE + 0x174)
/** H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH1 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH1    0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH1_M  (H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH1_V << H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH1_S)
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH1_V  0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH1_S  0

/** H264_DMA_OUT_PUSH_BYTECNT_CH1_REG register
 *  tx CH1 push byte cnt register
 */
#define H264_DMA_OUT_PUSH_BYTECNT_CH1_REG (DR_REG_H264_DMA_BASE + 0x178)
/** H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH1 : RO; bitpos: [7:0]; default: 255;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH1    0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH1_M  (H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH1_V << H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH1_S)
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH1_V  0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH1_S  0

/** H264_DMA_OUT_XADDR_CH1_REG register
 *  tx CH1 xaddr register
 */
#define H264_DMA_OUT_XADDR_CH1_REG (DR_REG_H264_DMA_BASE + 0x17c)
/** H264_DMA_OUT_CMDFIFO_XADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_XADDR_CH1    0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH1_M  (H264_DMA_OUT_CMDFIFO_XADDR_CH1_V << H264_DMA_OUT_CMDFIFO_XADDR_CH1_S)
#define H264_DMA_OUT_CMDFIFO_XADDR_CH1_V  0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH1_S  0

/** H264_DMA_OUT_CONF0_CH2_REG register
 *  TX CH2 config0 register
 */
#define H264_DMA_OUT_CONF0_CH2_REG (DR_REG_H264_DMA_BASE + 0x200)
/** H264_DMA_OUT_AUTO_WRBACK_CH2 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define H264_DMA_OUT_AUTO_WRBACK_CH2    (BIT(0))
#define H264_DMA_OUT_AUTO_WRBACK_CH2_M  (H264_DMA_OUT_AUTO_WRBACK_CH2_V << H264_DMA_OUT_AUTO_WRBACK_CH2_S)
#define H264_DMA_OUT_AUTO_WRBACK_CH2_V  0x00000001U
#define H264_DMA_OUT_AUTO_WRBACK_CH2_S  0
/** H264_DMA_OUT_EOF_MODE_CH2 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define H264_DMA_OUT_EOF_MODE_CH2    (BIT(1))
#define H264_DMA_OUT_EOF_MODE_CH2_M  (H264_DMA_OUT_EOF_MODE_CH2_V << H264_DMA_OUT_EOF_MODE_CH2_S)
#define H264_DMA_OUT_EOF_MODE_CH2_V  0x00000001U
#define H264_DMA_OUT_EOF_MODE_CH2_S  1
/** H264_DMA_OUTDSCR_BURST_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define H264_DMA_OUTDSCR_BURST_EN_CH2    (BIT(2))
#define H264_DMA_OUTDSCR_BURST_EN_CH2_M  (H264_DMA_OUTDSCR_BURST_EN_CH2_V << H264_DMA_OUTDSCR_BURST_EN_CH2_S)
#define H264_DMA_OUTDSCR_BURST_EN_CH2_V  0x00000001U
#define H264_DMA_OUTDSCR_BURST_EN_CH2_S  2
/** H264_DMA_OUT_ECC_AES_EN_CH2 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_OUT_ECC_AES_EN_CH2    (BIT(3))
#define H264_DMA_OUT_ECC_AES_EN_CH2_M  (H264_DMA_OUT_ECC_AES_EN_CH2_V << H264_DMA_OUT_ECC_AES_EN_CH2_S)
#define H264_DMA_OUT_ECC_AES_EN_CH2_V  0x00000001U
#define H264_DMA_OUT_ECC_AES_EN_CH2_S  3
/** H264_DMA_OUT_CHECK_OWNER_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_OUT_CHECK_OWNER_CH2    (BIT(4))
#define H264_DMA_OUT_CHECK_OWNER_CH2_M  (H264_DMA_OUT_CHECK_OWNER_CH2_V << H264_DMA_OUT_CHECK_OWNER_CH2_S)
#define H264_DMA_OUT_CHECK_OWNER_CH2_V  0x00000001U
#define H264_DMA_OUT_CHECK_OWNER_CH2_S  4
/** H264_DMA_OUT_MEM_BURST_LENGTH_CH2 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 2. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH2    0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH2_M  (H264_DMA_OUT_MEM_BURST_LENGTH_CH2_V << H264_DMA_OUT_MEM_BURST_LENGTH_CH2_S)
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH2_V  0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH2_S  6
/** H264_DMA_OUT_PAGE_BOUND_EN_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define H264_DMA_OUT_PAGE_BOUND_EN_CH2    (BIT(12))
#define H264_DMA_OUT_PAGE_BOUND_EN_CH2_M  (H264_DMA_OUT_PAGE_BOUND_EN_CH2_V << H264_DMA_OUT_PAGE_BOUND_EN_CH2_S)
#define H264_DMA_OUT_PAGE_BOUND_EN_CH2_V  0x00000001U
#define H264_DMA_OUT_PAGE_BOUND_EN_CH2_S  12
/** H264_DMA_OUT_RST_CH2 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset TX channel
 */
#define H264_DMA_OUT_RST_CH2    (BIT(24))
#define H264_DMA_OUT_RST_CH2_M  (H264_DMA_OUT_RST_CH2_V << H264_DMA_OUT_RST_CH2_S)
#define H264_DMA_OUT_RST_CH2_V  0x00000001U
#define H264_DMA_OUT_RST_CH2_S  24
/** H264_DMA_OUT_CMD_DISABLE_CH2 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_OUT_CMD_DISABLE_CH2    (BIT(25))
#define H264_DMA_OUT_CMD_DISABLE_CH2_M  (H264_DMA_OUT_CMD_DISABLE_CH2_V << H264_DMA_OUT_CMD_DISABLE_CH2_S)
#define H264_DMA_OUT_CMD_DISABLE_CH2_V  0x00000001U
#define H264_DMA_OUT_CMD_DISABLE_CH2_S  25
/** H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH2 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH2    (BIT(26))
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH2_M  (H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH2_V << H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH2_S)
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH2_V  0x00000001U
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH2_S  26

/** H264_DMA_OUT_INT_RAW_CH2_REG register
 *  TX CH2  interrupt raw register
 */
#define H264_DMA_OUT_INT_RAW_CH2_REG (DR_REG_H264_DMA_BASE + 0x204)
/** H264_DMA_OUT_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define H264_DMA_OUT_DONE_CH2_INT_RAW    (BIT(0))
#define H264_DMA_OUT_DONE_CH2_INT_RAW_M  (H264_DMA_OUT_DONE_CH2_INT_RAW_V << H264_DMA_OUT_DONE_CH2_INT_RAW_S)
#define H264_DMA_OUT_DONE_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DONE_CH2_INT_RAW_S  0
/** H264_DMA_OUT_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define H264_DMA_OUT_EOF_CH2_INT_RAW    (BIT(1))
#define H264_DMA_OUT_EOF_CH2_INT_RAW_M  (H264_DMA_OUT_EOF_CH2_INT_RAW_V << H264_DMA_OUT_EOF_CH2_INT_RAW_S)
#define H264_DMA_OUT_EOF_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_EOF_CH2_INT_RAW_S  1
/** H264_DMA_OUT_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_RAW    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_RAW_M  (H264_DMA_OUT_DSCR_ERR_CH2_INT_RAW_V << H264_DMA_OUT_DSCR_ERR_CH2_INT_RAW_S)
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_RAW_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_RAW    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_M  (H264_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_V << H264_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_S)
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_RAW    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_RAW_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_RAW    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_RAW_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_RAW    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L2_CH2_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L2_CH2_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_RAW_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH2_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_RAW    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L2_CH2_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L2_CH2_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_RAW_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_RAW    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_RAW_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_RAW_V << H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_RAW_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_RAW_S  8

/** H264_DMA_OUT_INT_ENA_CH2_REG register
 *  TX CH2  interrupt ena register
 */
#define H264_DMA_OUT_INT_ENA_CH2_REG (DR_REG_H264_DMA_BASE + 0x208)
/** H264_DMA_OUT_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH2_INT_ENA    (BIT(0))
#define H264_DMA_OUT_DONE_CH2_INT_ENA_M  (H264_DMA_OUT_DONE_CH2_INT_ENA_V << H264_DMA_OUT_DONE_CH2_INT_ENA_S)
#define H264_DMA_OUT_DONE_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DONE_CH2_INT_ENA_S  0
/** H264_DMA_OUT_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH2_INT_ENA    (BIT(1))
#define H264_DMA_OUT_EOF_CH2_INT_ENA_M  (H264_DMA_OUT_EOF_CH2_INT_ENA_V << H264_DMA_OUT_EOF_CH2_INT_ENA_S)
#define H264_DMA_OUT_EOF_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_EOF_CH2_INT_ENA_S  1
/** H264_DMA_OUT_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ENA    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ENA_M  (H264_DMA_OUT_DSCR_ERR_CH2_INT_ENA_V << H264_DMA_OUT_DSCR_ERR_CH2_INT_ENA_S)
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ENA_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ENA    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_M  (H264_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_V << H264_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_S)
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ENA    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ENA_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ENA    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ENA_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ENA    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ENA_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ENA    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ENA_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ENA    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ENA_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ENA_V << H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ENA_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ENA_S  8

/** H264_DMA_OUT_INT_ST_CH2_REG register
 *  TX CH2  interrupt st register
 */
#define H264_DMA_OUT_INT_ST_CH2_REG (DR_REG_H264_DMA_BASE + 0x20c)
/** H264_DMA_OUT_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH2_INT_ST    (BIT(0))
#define H264_DMA_OUT_DONE_CH2_INT_ST_M  (H264_DMA_OUT_DONE_CH2_INT_ST_V << H264_DMA_OUT_DONE_CH2_INT_ST_S)
#define H264_DMA_OUT_DONE_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DONE_CH2_INT_ST_S  0
/** H264_DMA_OUT_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH2_INT_ST    (BIT(1))
#define H264_DMA_OUT_EOF_CH2_INT_ST_M  (H264_DMA_OUT_EOF_CH2_INT_ST_V << H264_DMA_OUT_EOF_CH2_INT_ST_S)
#define H264_DMA_OUT_EOF_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUT_EOF_CH2_INT_ST_S  1
/** H264_DMA_OUT_DSCR_ERR_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ST    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ST_M  (H264_DMA_OUT_DSCR_ERR_CH2_INT_ST_V << H264_DMA_OUT_DSCR_ERR_CH2_INT_ST_S)
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_ST_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ST    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ST_M  (H264_DMA_OUT_TOTAL_EOF_CH2_INT_ST_V << H264_DMA_OUT_TOTAL_EOF_CH2_INT_ST_S)
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_ST_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ST    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ST_V << H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_ST_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ST    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ST_V << H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_ST_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ST    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ST_V << H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_ST_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ST    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ST_V << H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_ST_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ST    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ST_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ST_V << H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ST_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_ST_S  8

/** H264_DMA_OUT_INT_CLR_CH2_REG register
 *  TX CH2  interrupt clr register
 */
#define H264_DMA_OUT_INT_CLR_CH2_REG (DR_REG_H264_DMA_BASE + 0x210)
/** H264_DMA_OUT_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH2_INT_CLR    (BIT(0))
#define H264_DMA_OUT_DONE_CH2_INT_CLR_M  (H264_DMA_OUT_DONE_CH2_INT_CLR_V << H264_DMA_OUT_DONE_CH2_INT_CLR_S)
#define H264_DMA_OUT_DONE_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DONE_CH2_INT_CLR_S  0
/** H264_DMA_OUT_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH2_INT_CLR    (BIT(1))
#define H264_DMA_OUT_EOF_CH2_INT_CLR_M  (H264_DMA_OUT_EOF_CH2_INT_CLR_V << H264_DMA_OUT_EOF_CH2_INT_CLR_S)
#define H264_DMA_OUT_EOF_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_EOF_CH2_INT_CLR_S  1
/** H264_DMA_OUT_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_CLR    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_CLR_M  (H264_DMA_OUT_DSCR_ERR_CH2_INT_CLR_V << H264_DMA_OUT_DSCR_ERR_CH2_INT_CLR_S)
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH2_INT_CLR_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_CLR    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_M  (H264_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_V << H264_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_S)
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_CLR    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH2_INT_CLR_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_CLR    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH2_INT_CLR_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_CLR    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L2_CH2_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L2_CH2_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH2_INT_CLR_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH2_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_CLR    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L2_CH2_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L2_CH2_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH2_INT_CLR_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_CLR    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_CLR_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_CLR_V << H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_CLR_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH2_INT_CLR_S  8

/** H264_DMA_OUTFIFO_STATUS_CH2_REG register
 *  TX CH2 outfifo status register
 */
#define H264_DMA_OUTFIFO_STATUS_CH2_REG (DR_REG_H264_DMA_BASE + 0x214)
/** H264_DMA_OUTFIFO_FULL_L2_CH2 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L2_CH2    (BIT(0))
#define H264_DMA_OUTFIFO_FULL_L2_CH2_M  (H264_DMA_OUTFIFO_FULL_L2_CH2_V << H264_DMA_OUTFIFO_FULL_L2_CH2_S)
#define H264_DMA_OUTFIFO_FULL_L2_CH2_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L2_CH2_S  0
/** H264_DMA_OUTFIFO_EMPTY_L2_CH2 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L2_CH2    (BIT(1))
#define H264_DMA_OUTFIFO_EMPTY_L2_CH2_M  (H264_DMA_OUTFIFO_EMPTY_L2_CH2_V << H264_DMA_OUTFIFO_EMPTY_L2_CH2_S)
#define H264_DMA_OUTFIFO_EMPTY_L2_CH2_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L2_CH2_S  1
/** H264_DMA_OUTFIFO_CNT_L2_CH2 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L2_CH2    0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH2_M  (H264_DMA_OUTFIFO_CNT_L2_CH2_V << H264_DMA_OUTFIFO_CNT_L2_CH2_S)
#define H264_DMA_OUTFIFO_CNT_L2_CH2_V  0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH2_S  2
/** H264_DMA_OUTFIFO_FULL_L1_CH2 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L1_CH2    (BIT(6))
#define H264_DMA_OUTFIFO_FULL_L1_CH2_M  (H264_DMA_OUTFIFO_FULL_L1_CH2_V << H264_DMA_OUTFIFO_FULL_L1_CH2_S)
#define H264_DMA_OUTFIFO_FULL_L1_CH2_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L1_CH2_S  6
/** H264_DMA_OUTFIFO_EMPTY_L1_CH2 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L1_CH2    (BIT(7))
#define H264_DMA_OUTFIFO_EMPTY_L1_CH2_M  (H264_DMA_OUTFIFO_EMPTY_L1_CH2_V << H264_DMA_OUTFIFO_EMPTY_L1_CH2_S)
#define H264_DMA_OUTFIFO_EMPTY_L1_CH2_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L1_CH2_S  7
/** H264_DMA_OUTFIFO_CNT_L1_CH2 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L1_CH2    0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH2_M  (H264_DMA_OUTFIFO_CNT_L1_CH2_V << H264_DMA_OUTFIFO_CNT_L1_CH2_S)
#define H264_DMA_OUTFIFO_CNT_L1_CH2_V  0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH2_S  8
/** H264_DMA_OUTFIFO_FULL_L3_CH2 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L3_CH2    (BIT(16))
#define H264_DMA_OUTFIFO_FULL_L3_CH2_M  (H264_DMA_OUTFIFO_FULL_L3_CH2_V << H264_DMA_OUTFIFO_FULL_L3_CH2_S)
#define H264_DMA_OUTFIFO_FULL_L3_CH2_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L3_CH2_S  16
/** H264_DMA_OUTFIFO_EMPTY_L3_CH2 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L3_CH2    (BIT(17))
#define H264_DMA_OUTFIFO_EMPTY_L3_CH2_M  (H264_DMA_OUTFIFO_EMPTY_L3_CH2_V << H264_DMA_OUTFIFO_EMPTY_L3_CH2_S)
#define H264_DMA_OUTFIFO_EMPTY_L3_CH2_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L3_CH2_S  17
/** H264_DMA_OUTFIFO_CNT_L3_CH2 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L3_CH2    0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH2_M  (H264_DMA_OUTFIFO_CNT_L3_CH2_V << H264_DMA_OUTFIFO_CNT_L3_CH2_S)
#define H264_DMA_OUTFIFO_CNT_L3_CH2_V  0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH2_S  18

/** H264_DMA_OUT_PUSH_CH2_REG register
 *  TX CH2 outfifo push register
 */
#define H264_DMA_OUT_PUSH_CH2_REG (DR_REG_H264_DMA_BASE + 0x218)
/** H264_DMA_OUTFIFO_WDATA_CH2 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_WDATA_CH2    0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH2_M  (H264_DMA_OUTFIFO_WDATA_CH2_V << H264_DMA_OUTFIFO_WDATA_CH2_S)
#define H264_DMA_OUTFIFO_WDATA_CH2_V  0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH2_S  0
/** H264_DMA_OUTFIFO_PUSH_CH2 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_PUSH_CH2    (BIT(10))
#define H264_DMA_OUTFIFO_PUSH_CH2_M  (H264_DMA_OUTFIFO_PUSH_CH2_V << H264_DMA_OUTFIFO_PUSH_CH2_S)
#define H264_DMA_OUTFIFO_PUSH_CH2_V  0x00000001U
#define H264_DMA_OUTFIFO_PUSH_CH2_S  10

/** H264_DMA_OUT_LINK_CONF_CH2_REG register
 *  TX CH2 out_link dscr ctrl register
 */
#define H264_DMA_OUT_LINK_CONF_CH2_REG (DR_REG_H264_DMA_BASE + 0x21c)
/** H264_DMA_OUTLINK_STOP_CH2 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_STOP_CH2    (BIT(20))
#define H264_DMA_OUTLINK_STOP_CH2_M  (H264_DMA_OUTLINK_STOP_CH2_V << H264_DMA_OUTLINK_STOP_CH2_S)
#define H264_DMA_OUTLINK_STOP_CH2_V  0x00000001U
#define H264_DMA_OUTLINK_STOP_CH2_S  20
/** H264_DMA_OUTLINK_START_CH2 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_START_CH2    (BIT(21))
#define H264_DMA_OUTLINK_START_CH2_M  (H264_DMA_OUTLINK_START_CH2_V << H264_DMA_OUTLINK_START_CH2_S)
#define H264_DMA_OUTLINK_START_CH2_V  0x00000001U
#define H264_DMA_OUTLINK_START_CH2_S  21
/** H264_DMA_OUTLINK_RESTART_CH2 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define H264_DMA_OUTLINK_RESTART_CH2    (BIT(22))
#define H264_DMA_OUTLINK_RESTART_CH2_M  (H264_DMA_OUTLINK_RESTART_CH2_V << H264_DMA_OUTLINK_RESTART_CH2_S)
#define H264_DMA_OUTLINK_RESTART_CH2_V  0x00000001U
#define H264_DMA_OUTLINK_RESTART_CH2_S  22
/** H264_DMA_OUTLINK_PARK_CH2 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define H264_DMA_OUTLINK_PARK_CH2    (BIT(23))
#define H264_DMA_OUTLINK_PARK_CH2_M  (H264_DMA_OUTLINK_PARK_CH2_V << H264_DMA_OUTLINK_PARK_CH2_S)
#define H264_DMA_OUTLINK_PARK_CH2_V  0x00000001U
#define H264_DMA_OUTLINK_PARK_CH2_S  23

/** H264_DMA_OUT_LINK_ADDR_CH2_REG register
 *  TX CH2 out_link dscr addr register
 */
#define H264_DMA_OUT_LINK_ADDR_CH2_REG (DR_REG_H264_DMA_BASE + 0x220)
/** H264_DMA_OUTLINK_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_ADDR_CH2    0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH2_M  (H264_DMA_OUTLINK_ADDR_CH2_V << H264_DMA_OUTLINK_ADDR_CH2_S)
#define H264_DMA_OUTLINK_ADDR_CH2_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH2_S  0

/** H264_DMA_OUT_STATE_CH2_REG register
 *  TX CH2 state register
 */
#define H264_DMA_OUT_STATE_CH2_REG (DR_REG_H264_DMA_BASE + 0x224)
/** H264_DMA_OUTLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH2_M  (H264_DMA_OUTLINK_DSCR_ADDR_CH2_V << H264_DMA_OUTLINK_DSCR_ADDR_CH2_S)
#define H264_DMA_OUTLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH2_S  0
/** H264_DMA_OUT_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_OUT_DSCR_STATE_CH2    0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH2_M  (H264_DMA_OUT_DSCR_STATE_CH2_V << H264_DMA_OUT_DSCR_STATE_CH2_S)
#define H264_DMA_OUT_DSCR_STATE_CH2_V  0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH2_S  18
/** H264_DMA_OUT_STATE_CH2 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_OUT_STATE_CH2    0x0000000FU
#define H264_DMA_OUT_STATE_CH2_M  (H264_DMA_OUT_STATE_CH2_V << H264_DMA_OUT_STATE_CH2_S)
#define H264_DMA_OUT_STATE_CH2_V  0x0000000FU
#define H264_DMA_OUT_STATE_CH2_S  20
/** H264_DMA_OUT_RESET_AVAIL_CH2 : RO; bitpos: [24]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_OUT_RESET_AVAIL_CH2    (BIT(24))
#define H264_DMA_OUT_RESET_AVAIL_CH2_M  (H264_DMA_OUT_RESET_AVAIL_CH2_V << H264_DMA_OUT_RESET_AVAIL_CH2_S)
#define H264_DMA_OUT_RESET_AVAIL_CH2_V  0x00000001U
#define H264_DMA_OUT_RESET_AVAIL_CH2_S  24

/** H264_DMA_OUT_EOF_DES_ADDR_CH2_REG register
 *  TX CH2 eof des addr register
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH2_REG (DR_REG_H264_DMA_BASE + 0x228)
/** H264_DMA_OUT_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH2_M  (H264_DMA_OUT_EOF_DES_ADDR_CH2_V << H264_DMA_OUT_EOF_DES_ADDR_CH2_S)
#define H264_DMA_OUT_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH2_S  0

/** H264_DMA_OUT_DSCR_CH2_REG register
 *  TX CH2 next dscr addr register
 */
#define H264_DMA_OUT_DSCR_CH2_REG (DR_REG_H264_DMA_BASE + 0x22c)
/** H264_DMA_OUTLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define H264_DMA_OUTLINK_DSCR_CH2    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH2_M  (H264_DMA_OUTLINK_DSCR_CH2_V << H264_DMA_OUTLINK_DSCR_CH2_S)
#define H264_DMA_OUTLINK_DSCR_CH2_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH2_S  0

/** H264_DMA_OUT_DSCR_BF0_CH2_REG register
 *  TX CH2 last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF0_CH2_REG (DR_REG_H264_DMA_BASE + 0x230)
/** H264_DMA_OUTLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define H264_DMA_OUTLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH2_M  (H264_DMA_OUTLINK_DSCR_BF0_CH2_V << H264_DMA_OUTLINK_DSCR_BF0_CH2_S)
#define H264_DMA_OUTLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH2_S  0

/** H264_DMA_OUT_DSCR_BF1_CH2_REG register
 *  TX CH2 second-to-last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF1_CH2_REG (DR_REG_H264_DMA_BASE + 0x234)
/** H264_DMA_OUTLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define H264_DMA_OUTLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH2_M  (H264_DMA_OUTLINK_DSCR_BF1_CH2_V << H264_DMA_OUTLINK_DSCR_BF1_CH2_S)
#define H264_DMA_OUTLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH2_S  0

/** H264_DMA_OUT_ARB_CH2_REG register
 *  TX CH2 arb register
 */
#define H264_DMA_OUT_ARB_CH2_REG (DR_REG_H264_DMA_BASE + 0x23c)
/** H264_DMA_OUT_ARB_TOKEN_NUM_CH2 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH2    0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH2_M  (H264_DMA_OUT_ARB_TOKEN_NUM_CH2_V << H264_DMA_OUT_ARB_TOKEN_NUM_CH2_S)
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH2_V  0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH2_S  0
/** H264_DMA_INTER_OUT_ARB_PRIORITY_CH2 : R/W; bitpos: [6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH2    (BIT(6))
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH2_M  (H264_DMA_INTER_OUT_ARB_PRIORITY_CH2_V << H264_DMA_INTER_OUT_ARB_PRIORITY_CH2_S)
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH2_V  0x00000001U
#define H264_DMA_INTER_OUT_ARB_PRIORITY_CH2_S  6

/** H264_DMA_OUT_ETM_CONF_CH2_REG register
 *  TX CH2 ETM config register
 */
#define H264_DMA_OUT_ETM_CONF_CH2_REG (DR_REG_H264_DMA_BASE + 0x268)
/** H264_DMA_OUT_ETM_EN_CH2 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_OUT_ETM_EN_CH2    (BIT(0))
#define H264_DMA_OUT_ETM_EN_CH2_M  (H264_DMA_OUT_ETM_EN_CH2_V << H264_DMA_OUT_ETM_EN_CH2_S)
#define H264_DMA_OUT_ETM_EN_CH2_V  0x00000001U
#define H264_DMA_OUT_ETM_EN_CH2_S  0
/** H264_DMA_OUT_ETM_LOOP_EN_CH2 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_OUT_ETM_LOOP_EN_CH2    (BIT(1))
#define H264_DMA_OUT_ETM_LOOP_EN_CH2_M  (H264_DMA_OUT_ETM_LOOP_EN_CH2_V << H264_DMA_OUT_ETM_LOOP_EN_CH2_S)
#define H264_DMA_OUT_ETM_LOOP_EN_CH2_V  0x00000001U
#define H264_DMA_OUT_ETM_LOOP_EN_CH2_S  1
/** H264_DMA_OUT_DSCR_TASK_MAK_CH2 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_OUT_DSCR_TASK_MAK_CH2    0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH2_M  (H264_DMA_OUT_DSCR_TASK_MAK_CH2_V << H264_DMA_OUT_DSCR_TASK_MAK_CH2_S)
#define H264_DMA_OUT_DSCR_TASK_MAK_CH2_V  0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH2_S  2

/** H264_DMA_OUT_BUF_LEN_CH2_REG register
 *  tx CH2 buf len register
 */
#define H264_DMA_OUT_BUF_LEN_CH2_REG (DR_REG_H264_DMA_BASE + 0x270)
/** H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH2 : RO; bitpos: [12:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH2    0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH2_M  (H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH2_V << H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH2_S)
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH2_V  0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH2_S  0

/** H264_DMA_OUT_FIFO_BCNT_CH2_REG register
 *  tx CH2 fifo byte cnt register
 */
#define H264_DMA_OUT_FIFO_BCNT_CH2_REG (DR_REG_H264_DMA_BASE + 0x274)
/** H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH2 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH2    0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH2_M  (H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH2_V << H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH2_S)
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH2_V  0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH2_S  0

/** H264_DMA_OUT_PUSH_BYTECNT_CH2_REG register
 *  tx CH2 push byte cnt register
 */
#define H264_DMA_OUT_PUSH_BYTECNT_CH2_REG (DR_REG_H264_DMA_BASE + 0x278)
/** H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH2 : RO; bitpos: [7:0]; default: 255;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH2    0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH2_M  (H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH2_V << H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH2_S)
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH2_V  0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH2_S  0

/** H264_DMA_OUT_XADDR_CH2_REG register
 *  tx CH2 xaddr register
 */
#define H264_DMA_OUT_XADDR_CH2_REG (DR_REG_H264_DMA_BASE + 0x27c)
/** H264_DMA_OUT_CMDFIFO_XADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_XADDR_CH2    0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH2_M  (H264_DMA_OUT_CMDFIFO_XADDR_CH2_V << H264_DMA_OUT_CMDFIFO_XADDR_CH2_S)
#define H264_DMA_OUT_CMDFIFO_XADDR_CH2_V  0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH2_S  0

/** H264_DMA_OUT_CONF0_CH3_REG register
 *  TX CH3 config0 register
 */
#define H264_DMA_OUT_CONF0_CH3_REG (DR_REG_H264_DMA_BASE + 0x300)
/** H264_DMA_OUT_AUTO_WRBACK_CH3 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define H264_DMA_OUT_AUTO_WRBACK_CH3    (BIT(0))
#define H264_DMA_OUT_AUTO_WRBACK_CH3_M  (H264_DMA_OUT_AUTO_WRBACK_CH3_V << H264_DMA_OUT_AUTO_WRBACK_CH3_S)
#define H264_DMA_OUT_AUTO_WRBACK_CH3_V  0x00000001U
#define H264_DMA_OUT_AUTO_WRBACK_CH3_S  0
/** H264_DMA_OUT_EOF_MODE_CH3 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define H264_DMA_OUT_EOF_MODE_CH3    (BIT(1))
#define H264_DMA_OUT_EOF_MODE_CH3_M  (H264_DMA_OUT_EOF_MODE_CH3_V << H264_DMA_OUT_EOF_MODE_CH3_S)
#define H264_DMA_OUT_EOF_MODE_CH3_V  0x00000001U
#define H264_DMA_OUT_EOF_MODE_CH3_S  1
/** H264_DMA_OUTDSCR_BURST_EN_CH3 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define H264_DMA_OUTDSCR_BURST_EN_CH3    (BIT(2))
#define H264_DMA_OUTDSCR_BURST_EN_CH3_M  (H264_DMA_OUTDSCR_BURST_EN_CH3_V << H264_DMA_OUTDSCR_BURST_EN_CH3_S)
#define H264_DMA_OUTDSCR_BURST_EN_CH3_V  0x00000001U
#define H264_DMA_OUTDSCR_BURST_EN_CH3_S  2
/** H264_DMA_OUT_ECC_AES_EN_CH3 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_OUT_ECC_AES_EN_CH3    (BIT(3))
#define H264_DMA_OUT_ECC_AES_EN_CH3_M  (H264_DMA_OUT_ECC_AES_EN_CH3_V << H264_DMA_OUT_ECC_AES_EN_CH3_S)
#define H264_DMA_OUT_ECC_AES_EN_CH3_V  0x00000001U
#define H264_DMA_OUT_ECC_AES_EN_CH3_S  3
/** H264_DMA_OUT_CHECK_OWNER_CH3 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_OUT_CHECK_OWNER_CH3    (BIT(4))
#define H264_DMA_OUT_CHECK_OWNER_CH3_M  (H264_DMA_OUT_CHECK_OWNER_CH3_V << H264_DMA_OUT_CHECK_OWNER_CH3_S)
#define H264_DMA_OUT_CHECK_OWNER_CH3_V  0x00000001U
#define H264_DMA_OUT_CHECK_OWNER_CH3_S  4
/** H264_DMA_OUT_MEM_BURST_LENGTH_CH3 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 3. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH3    0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH3_M  (H264_DMA_OUT_MEM_BURST_LENGTH_CH3_V << H264_DMA_OUT_MEM_BURST_LENGTH_CH3_S)
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH3_V  0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH3_S  6
/** H264_DMA_OUT_PAGE_BOUND_EN_CH3 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define H264_DMA_OUT_PAGE_BOUND_EN_CH3    (BIT(12))
#define H264_DMA_OUT_PAGE_BOUND_EN_CH3_M  (H264_DMA_OUT_PAGE_BOUND_EN_CH3_V << H264_DMA_OUT_PAGE_BOUND_EN_CH3_S)
#define H264_DMA_OUT_PAGE_BOUND_EN_CH3_V  0x00000001U
#define H264_DMA_OUT_PAGE_BOUND_EN_CH3_S  12
/** H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH3 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH3    (BIT(26))
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH3_M  (H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH3_V << H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH3_S)
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH3_V  0x00000001U
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH3_S  26

/** H264_DMA_OUT_INT_RAW_CH3_REG register
 *  TX CH3  interrupt raw register
 */
#define H264_DMA_OUT_INT_RAW_CH3_REG (DR_REG_H264_DMA_BASE + 0x304)
/** H264_DMA_OUT_DONE_CH3_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define H264_DMA_OUT_DONE_CH3_INT_RAW    (BIT(0))
#define H264_DMA_OUT_DONE_CH3_INT_RAW_M  (H264_DMA_OUT_DONE_CH3_INT_RAW_V << H264_DMA_OUT_DONE_CH3_INT_RAW_S)
#define H264_DMA_OUT_DONE_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DONE_CH3_INT_RAW_S  0
/** H264_DMA_OUT_EOF_CH3_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define H264_DMA_OUT_EOF_CH3_INT_RAW    (BIT(1))
#define H264_DMA_OUT_EOF_CH3_INT_RAW_M  (H264_DMA_OUT_EOF_CH3_INT_RAW_V << H264_DMA_OUT_EOF_CH3_INT_RAW_S)
#define H264_DMA_OUT_EOF_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_EOF_CH3_INT_RAW_S  1
/** H264_DMA_OUT_DSCR_ERR_CH3_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_RAW    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_RAW_M  (H264_DMA_OUT_DSCR_ERR_CH3_INT_RAW_V << H264_DMA_OUT_DSCR_ERR_CH3_INT_RAW_S)
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_RAW_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH3_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_RAW    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_RAW_M  (H264_DMA_OUT_TOTAL_EOF_CH3_INT_RAW_V << H264_DMA_OUT_TOTAL_EOF_CH3_INT_RAW_S)
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_RAW_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH3_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_RAW    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_RAW_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH3_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_RAW    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_RAW_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH3_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_RAW    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L2_CH3_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L2_CH3_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_RAW_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH3_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_RAW    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L2_CH3_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L2_CH3_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_RAW_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_RAW    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_RAW_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_RAW_V << H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_RAW_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_RAW_S  8

/** H264_DMA_OUT_INT_ENA_CH3_REG register
 *  TX CH3  interrupt ena register
 */
#define H264_DMA_OUT_INT_ENA_CH3_REG (DR_REG_H264_DMA_BASE + 0x308)
/** H264_DMA_OUT_DONE_CH3_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH3_INT_ENA    (BIT(0))
#define H264_DMA_OUT_DONE_CH3_INT_ENA_M  (H264_DMA_OUT_DONE_CH3_INT_ENA_V << H264_DMA_OUT_DONE_CH3_INT_ENA_S)
#define H264_DMA_OUT_DONE_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DONE_CH3_INT_ENA_S  0
/** H264_DMA_OUT_EOF_CH3_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH3_INT_ENA    (BIT(1))
#define H264_DMA_OUT_EOF_CH3_INT_ENA_M  (H264_DMA_OUT_EOF_CH3_INT_ENA_V << H264_DMA_OUT_EOF_CH3_INT_ENA_S)
#define H264_DMA_OUT_EOF_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_EOF_CH3_INT_ENA_S  1
/** H264_DMA_OUT_DSCR_ERR_CH3_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ENA    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ENA_M  (H264_DMA_OUT_DSCR_ERR_CH3_INT_ENA_V << H264_DMA_OUT_DSCR_ERR_CH3_INT_ENA_S)
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ENA_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH3_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ENA    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ENA_M  (H264_DMA_OUT_TOTAL_EOF_CH3_INT_ENA_V << H264_DMA_OUT_TOTAL_EOF_CH3_INT_ENA_S)
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ENA_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ENA    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ENA_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ENA    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ENA_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ENA    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ENA_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ENA    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ENA_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ENA    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ENA_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ENA_V << H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ENA_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ENA_S  8

/** H264_DMA_OUT_INT_ST_CH3_REG register
 *  TX CH3  interrupt st register
 */
#define H264_DMA_OUT_INT_ST_CH3_REG (DR_REG_H264_DMA_BASE + 0x30c)
/** H264_DMA_OUT_DONE_CH3_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH3_INT_ST    (BIT(0))
#define H264_DMA_OUT_DONE_CH3_INT_ST_M  (H264_DMA_OUT_DONE_CH3_INT_ST_V << H264_DMA_OUT_DONE_CH3_INT_ST_S)
#define H264_DMA_OUT_DONE_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DONE_CH3_INT_ST_S  0
/** H264_DMA_OUT_EOF_CH3_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH3_INT_ST    (BIT(1))
#define H264_DMA_OUT_EOF_CH3_INT_ST_M  (H264_DMA_OUT_EOF_CH3_INT_ST_V << H264_DMA_OUT_EOF_CH3_INT_ST_S)
#define H264_DMA_OUT_EOF_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUT_EOF_CH3_INT_ST_S  1
/** H264_DMA_OUT_DSCR_ERR_CH3_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ST    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ST_M  (H264_DMA_OUT_DSCR_ERR_CH3_INT_ST_V << H264_DMA_OUT_DSCR_ERR_CH3_INT_ST_S)
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_ST_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH3_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ST    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ST_M  (H264_DMA_OUT_TOTAL_EOF_CH3_INT_ST_V << H264_DMA_OUT_TOTAL_EOF_CH3_INT_ST_S)
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_ST_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ST    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ST_V << H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_ST_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ST    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ST_V << H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_ST_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ST    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ST_V << H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_ST_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ST    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ST_V << H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_ST_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ST    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ST_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ST_V << H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ST_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_ST_S  8

/** H264_DMA_OUT_INT_CLR_CH3_REG register
 *  TX CH3  interrupt clr register
 */
#define H264_DMA_OUT_INT_CLR_CH3_REG (DR_REG_H264_DMA_BASE + 0x310)
/** H264_DMA_OUT_DONE_CH3_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH3_INT_CLR    (BIT(0))
#define H264_DMA_OUT_DONE_CH3_INT_CLR_M  (H264_DMA_OUT_DONE_CH3_INT_CLR_V << H264_DMA_OUT_DONE_CH3_INT_CLR_S)
#define H264_DMA_OUT_DONE_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DONE_CH3_INT_CLR_S  0
/** H264_DMA_OUT_EOF_CH3_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH3_INT_CLR    (BIT(1))
#define H264_DMA_OUT_EOF_CH3_INT_CLR_M  (H264_DMA_OUT_EOF_CH3_INT_CLR_V << H264_DMA_OUT_EOF_CH3_INT_CLR_S)
#define H264_DMA_OUT_EOF_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_EOF_CH3_INT_CLR_S  1
/** H264_DMA_OUT_DSCR_ERR_CH3_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_CLR    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_CLR_M  (H264_DMA_OUT_DSCR_ERR_CH3_INT_CLR_V << H264_DMA_OUT_DSCR_ERR_CH3_INT_CLR_S)
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH3_INT_CLR_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH3_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_CLR    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_CLR_M  (H264_DMA_OUT_TOTAL_EOF_CH3_INT_CLR_V << H264_DMA_OUT_TOTAL_EOF_CH3_INT_CLR_S)
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH3_INT_CLR_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH3_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_CLR    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH3_INT_CLR_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH3_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_CLR    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH3_INT_CLR_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH3_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_CLR    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L2_CH3_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L2_CH3_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH3_INT_CLR_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH3_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_CLR    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L2_CH3_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L2_CH3_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH3_INT_CLR_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_CLR    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_CLR_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_CLR_V << H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_CLR_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH3_INT_CLR_S  8

/** H264_DMA_OUTFIFO_STATUS_CH3_REG register
 *  TX CH3 outfifo status register
 */
#define H264_DMA_OUTFIFO_STATUS_CH3_REG (DR_REG_H264_DMA_BASE + 0x314)
/** H264_DMA_OUTFIFO_FULL_L2_CH3 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L2_CH3    (BIT(0))
#define H264_DMA_OUTFIFO_FULL_L2_CH3_M  (H264_DMA_OUTFIFO_FULL_L2_CH3_V << H264_DMA_OUTFIFO_FULL_L2_CH3_S)
#define H264_DMA_OUTFIFO_FULL_L2_CH3_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L2_CH3_S  0
/** H264_DMA_OUTFIFO_EMPTY_L2_CH3 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L2_CH3    (BIT(1))
#define H264_DMA_OUTFIFO_EMPTY_L2_CH3_M  (H264_DMA_OUTFIFO_EMPTY_L2_CH3_V << H264_DMA_OUTFIFO_EMPTY_L2_CH3_S)
#define H264_DMA_OUTFIFO_EMPTY_L2_CH3_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L2_CH3_S  1
/** H264_DMA_OUTFIFO_CNT_L2_CH3 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L2_CH3    0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH3_M  (H264_DMA_OUTFIFO_CNT_L2_CH3_V << H264_DMA_OUTFIFO_CNT_L2_CH3_S)
#define H264_DMA_OUTFIFO_CNT_L2_CH3_V  0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH3_S  2
/** H264_DMA_OUTFIFO_FULL_L1_CH3 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L1_CH3    (BIT(6))
#define H264_DMA_OUTFIFO_FULL_L1_CH3_M  (H264_DMA_OUTFIFO_FULL_L1_CH3_V << H264_DMA_OUTFIFO_FULL_L1_CH3_S)
#define H264_DMA_OUTFIFO_FULL_L1_CH3_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L1_CH3_S  6
/** H264_DMA_OUTFIFO_EMPTY_L1_CH3 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L1_CH3    (BIT(7))
#define H264_DMA_OUTFIFO_EMPTY_L1_CH3_M  (H264_DMA_OUTFIFO_EMPTY_L1_CH3_V << H264_DMA_OUTFIFO_EMPTY_L1_CH3_S)
#define H264_DMA_OUTFIFO_EMPTY_L1_CH3_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L1_CH3_S  7
/** H264_DMA_OUTFIFO_CNT_L1_CH3 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L1_CH3    0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH3_M  (H264_DMA_OUTFIFO_CNT_L1_CH3_V << H264_DMA_OUTFIFO_CNT_L1_CH3_S)
#define H264_DMA_OUTFIFO_CNT_L1_CH3_V  0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH3_S  8
/** H264_DMA_OUTFIFO_FULL_L3_CH3 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L3_CH3    (BIT(16))
#define H264_DMA_OUTFIFO_FULL_L3_CH3_M  (H264_DMA_OUTFIFO_FULL_L3_CH3_V << H264_DMA_OUTFIFO_FULL_L3_CH3_S)
#define H264_DMA_OUTFIFO_FULL_L3_CH3_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L3_CH3_S  16
/** H264_DMA_OUTFIFO_EMPTY_L3_CH3 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L3_CH3    (BIT(17))
#define H264_DMA_OUTFIFO_EMPTY_L3_CH3_M  (H264_DMA_OUTFIFO_EMPTY_L3_CH3_V << H264_DMA_OUTFIFO_EMPTY_L3_CH3_S)
#define H264_DMA_OUTFIFO_EMPTY_L3_CH3_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L3_CH3_S  17
/** H264_DMA_OUTFIFO_CNT_L3_CH3 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L3_CH3    0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH3_M  (H264_DMA_OUTFIFO_CNT_L3_CH3_V << H264_DMA_OUTFIFO_CNT_L3_CH3_S)
#define H264_DMA_OUTFIFO_CNT_L3_CH3_V  0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH3_S  18

/** H264_DMA_OUT_PUSH_CH3_REG register
 *  TX CH3 outfifo push register
 */
#define H264_DMA_OUT_PUSH_CH3_REG (DR_REG_H264_DMA_BASE + 0x318)
/** H264_DMA_OUTFIFO_WDATA_CH3 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_WDATA_CH3    0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH3_M  (H264_DMA_OUTFIFO_WDATA_CH3_V << H264_DMA_OUTFIFO_WDATA_CH3_S)
#define H264_DMA_OUTFIFO_WDATA_CH3_V  0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH3_S  0
/** H264_DMA_OUTFIFO_PUSH_CH3 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_PUSH_CH3    (BIT(10))
#define H264_DMA_OUTFIFO_PUSH_CH3_M  (H264_DMA_OUTFIFO_PUSH_CH3_V << H264_DMA_OUTFIFO_PUSH_CH3_S)
#define H264_DMA_OUTFIFO_PUSH_CH3_V  0x00000001U
#define H264_DMA_OUTFIFO_PUSH_CH3_S  10

/** H264_DMA_OUT_LINK_CONF_CH3_REG register
 *  TX CH3 out_link dscr ctrl register
 */
#define H264_DMA_OUT_LINK_CONF_CH3_REG (DR_REG_H264_DMA_BASE + 0x31c)
/** H264_DMA_OUTLINK_STOP_CH3 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_STOP_CH3    (BIT(20))
#define H264_DMA_OUTLINK_STOP_CH3_M  (H264_DMA_OUTLINK_STOP_CH3_V << H264_DMA_OUTLINK_STOP_CH3_S)
#define H264_DMA_OUTLINK_STOP_CH3_V  0x00000001U
#define H264_DMA_OUTLINK_STOP_CH3_S  20
/** H264_DMA_OUTLINK_START_CH3 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_START_CH3    (BIT(21))
#define H264_DMA_OUTLINK_START_CH3_M  (H264_DMA_OUTLINK_START_CH3_V << H264_DMA_OUTLINK_START_CH3_S)
#define H264_DMA_OUTLINK_START_CH3_V  0x00000001U
#define H264_DMA_OUTLINK_START_CH3_S  21
/** H264_DMA_OUTLINK_RESTART_CH3 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define H264_DMA_OUTLINK_RESTART_CH3    (BIT(22))
#define H264_DMA_OUTLINK_RESTART_CH3_M  (H264_DMA_OUTLINK_RESTART_CH3_V << H264_DMA_OUTLINK_RESTART_CH3_S)
#define H264_DMA_OUTLINK_RESTART_CH3_V  0x00000001U
#define H264_DMA_OUTLINK_RESTART_CH3_S  22
/** H264_DMA_OUTLINK_PARK_CH3 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define H264_DMA_OUTLINK_PARK_CH3    (BIT(23))
#define H264_DMA_OUTLINK_PARK_CH3_M  (H264_DMA_OUTLINK_PARK_CH3_V << H264_DMA_OUTLINK_PARK_CH3_S)
#define H264_DMA_OUTLINK_PARK_CH3_V  0x00000001U
#define H264_DMA_OUTLINK_PARK_CH3_S  23

/** H264_DMA_OUT_LINK_ADDR_CH3_REG register
 *  TX CH3 out_link dscr addr register
 */
#define H264_DMA_OUT_LINK_ADDR_CH3_REG (DR_REG_H264_DMA_BASE + 0x320)
/** H264_DMA_OUTLINK_ADDR_CH3 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_ADDR_CH3    0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH3_M  (H264_DMA_OUTLINK_ADDR_CH3_V << H264_DMA_OUTLINK_ADDR_CH3_S)
#define H264_DMA_OUTLINK_ADDR_CH3_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH3_S  0

/** H264_DMA_OUT_STATE_CH3_REG register
 *  TX CH3 state register
 */
#define H264_DMA_OUT_STATE_CH3_REG (DR_REG_H264_DMA_BASE + 0x324)
/** H264_DMA_OUTLINK_DSCR_ADDR_CH3 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_DSCR_ADDR_CH3    0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH3_M  (H264_DMA_OUTLINK_DSCR_ADDR_CH3_V << H264_DMA_OUTLINK_DSCR_ADDR_CH3_S)
#define H264_DMA_OUTLINK_DSCR_ADDR_CH3_V  0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH3_S  0
/** H264_DMA_OUT_DSCR_STATE_CH3 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_OUT_DSCR_STATE_CH3    0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH3_M  (H264_DMA_OUT_DSCR_STATE_CH3_V << H264_DMA_OUT_DSCR_STATE_CH3_S)
#define H264_DMA_OUT_DSCR_STATE_CH3_V  0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH3_S  18
/** H264_DMA_OUT_STATE_CH3 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_OUT_STATE_CH3    0x0000000FU
#define H264_DMA_OUT_STATE_CH3_M  (H264_DMA_OUT_STATE_CH3_V << H264_DMA_OUT_STATE_CH3_S)
#define H264_DMA_OUT_STATE_CH3_V  0x0000000FU
#define H264_DMA_OUT_STATE_CH3_S  20

/** H264_DMA_OUT_EOF_DES_ADDR_CH3_REG register
 *  TX CH3 eof des addr register
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH3_REG (DR_REG_H264_DMA_BASE + 0x328)
/** H264_DMA_OUT_EOF_DES_ADDR_CH3 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH3    0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH3_M  (H264_DMA_OUT_EOF_DES_ADDR_CH3_V << H264_DMA_OUT_EOF_DES_ADDR_CH3_S)
#define H264_DMA_OUT_EOF_DES_ADDR_CH3_V  0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH3_S  0

/** H264_DMA_OUT_DSCR_CH3_REG register
 *  TX CH3 next dscr addr register
 */
#define H264_DMA_OUT_DSCR_CH3_REG (DR_REG_H264_DMA_BASE + 0x32c)
/** H264_DMA_OUTLINK_DSCR_CH3 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define H264_DMA_OUTLINK_DSCR_CH3    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH3_M  (H264_DMA_OUTLINK_DSCR_CH3_V << H264_DMA_OUTLINK_DSCR_CH3_S)
#define H264_DMA_OUTLINK_DSCR_CH3_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH3_S  0

/** H264_DMA_OUT_DSCR_BF0_CH3_REG register
 *  TX CH3 last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF0_CH3_REG (DR_REG_H264_DMA_BASE + 0x330)
/** H264_DMA_OUTLINK_DSCR_BF0_CH3 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define H264_DMA_OUTLINK_DSCR_BF0_CH3    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH3_M  (H264_DMA_OUTLINK_DSCR_BF0_CH3_V << H264_DMA_OUTLINK_DSCR_BF0_CH3_S)
#define H264_DMA_OUTLINK_DSCR_BF0_CH3_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH3_S  0

/** H264_DMA_OUT_DSCR_BF1_CH3_REG register
 *  TX CH3 second-to-last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF1_CH3_REG (DR_REG_H264_DMA_BASE + 0x334)
/** H264_DMA_OUTLINK_DSCR_BF1_CH3 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define H264_DMA_OUTLINK_DSCR_BF1_CH3    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH3_M  (H264_DMA_OUTLINK_DSCR_BF1_CH3_V << H264_DMA_OUTLINK_DSCR_BF1_CH3_S)
#define H264_DMA_OUTLINK_DSCR_BF1_CH3_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH3_S  0

/** H264_DMA_OUT_ARB_CH3_REG register
 *  TX CH3 arb register
 */
#define H264_DMA_OUT_ARB_CH3_REG (DR_REG_H264_DMA_BASE + 0x33c)
/** H264_DMA_OUT_ARB_TOKEN_NUM_CH3 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH3    0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH3_M  (H264_DMA_OUT_ARB_TOKEN_NUM_CH3_V << H264_DMA_OUT_ARB_TOKEN_NUM_CH3_S)
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH3_V  0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH3_S  0
/** H264_DMA_EXTER_OUT_ARB_PRIORITY_CH3 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH3    0x00000003U
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH3_M  (H264_DMA_EXTER_OUT_ARB_PRIORITY_CH3_V << H264_DMA_EXTER_OUT_ARB_PRIORITY_CH3_S)
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH3_V  0x00000003U
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH3_S  4

/** H264_DMA_OUT_ETM_CONF_CH3_REG register
 *  TX CH3 ETM config register
 */
#define H264_DMA_OUT_ETM_CONF_CH3_REG (DR_REG_H264_DMA_BASE + 0x368)
/** H264_DMA_OUT_ETM_EN_CH3 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_OUT_ETM_EN_CH3    (BIT(0))
#define H264_DMA_OUT_ETM_EN_CH3_M  (H264_DMA_OUT_ETM_EN_CH3_V << H264_DMA_OUT_ETM_EN_CH3_S)
#define H264_DMA_OUT_ETM_EN_CH3_V  0x00000001U
#define H264_DMA_OUT_ETM_EN_CH3_S  0
/** H264_DMA_OUT_ETM_LOOP_EN_CH3 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_OUT_ETM_LOOP_EN_CH3    (BIT(1))
#define H264_DMA_OUT_ETM_LOOP_EN_CH3_M  (H264_DMA_OUT_ETM_LOOP_EN_CH3_V << H264_DMA_OUT_ETM_LOOP_EN_CH3_S)
#define H264_DMA_OUT_ETM_LOOP_EN_CH3_V  0x00000001U
#define H264_DMA_OUT_ETM_LOOP_EN_CH3_S  1
/** H264_DMA_OUT_DSCR_TASK_MAK_CH3 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_OUT_DSCR_TASK_MAK_CH3    0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH3_M  (H264_DMA_OUT_DSCR_TASK_MAK_CH3_V << H264_DMA_OUT_DSCR_TASK_MAK_CH3_S)
#define H264_DMA_OUT_DSCR_TASK_MAK_CH3_V  0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH3_S  2

/** H264_DMA_OUT_BUF_LEN_CH3_REG register
 *  tx CH3 buf len register
 */
#define H264_DMA_OUT_BUF_LEN_CH3_REG (DR_REG_H264_DMA_BASE + 0x370)
/** H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH3 : RO; bitpos: [12:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH3    0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH3_M  (H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH3_V << H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH3_S)
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH3_V  0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH3_S  0

/** H264_DMA_OUT_FIFO_BCNT_CH3_REG register
 *  tx CH3 fifo byte cnt register
 */
#define H264_DMA_OUT_FIFO_BCNT_CH3_REG (DR_REG_H264_DMA_BASE + 0x374)
/** H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH3 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH3    0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH3_M  (H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH3_V << H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH3_S)
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH3_V  0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH3_S  0

/** H264_DMA_OUT_PUSH_BYTECNT_CH3_REG register
 *  tx CH3 push byte cnt register
 */
#define H264_DMA_OUT_PUSH_BYTECNT_CH3_REG (DR_REG_H264_DMA_BASE + 0x378)
/** H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH3 : RO; bitpos: [7:0]; default: 63;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH3    0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH3_M  (H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH3_V << H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH3_S)
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH3_V  0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH3_S  0

/** H264_DMA_OUT_XADDR_CH3_REG register
 *  tx CH3 xaddr register
 */
#define H264_DMA_OUT_XADDR_CH3_REG (DR_REG_H264_DMA_BASE + 0x37c)
/** H264_DMA_OUT_CMDFIFO_XADDR_CH3 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_XADDR_CH3    0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH3_M  (H264_DMA_OUT_CMDFIFO_XADDR_CH3_V << H264_DMA_OUT_CMDFIFO_XADDR_CH3_S)
#define H264_DMA_OUT_CMDFIFO_XADDR_CH3_V  0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH3_S  0

/** H264_DMA_OUT_BLOCK_BUF_LEN_CH3_REG register
 *  tx CH3 block buf len register
 */
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH3_REG (DR_REG_H264_DMA_BASE + 0x380)
/** H264_DMA_OUT_BLOCK_BUF_LEN_CH3 : RO; bitpos: [27:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH3    0x0FFFFFFFU
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH3_M  (H264_DMA_OUT_BLOCK_BUF_LEN_CH3_V << H264_DMA_OUT_BLOCK_BUF_LEN_CH3_S)
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH3_V  0x0FFFFFFFU
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH3_S  0

/** H264_DMA_OUT_CONF0_CH4_REG register
 *  TX CH4 config0 register
 */
#define H264_DMA_OUT_CONF0_CH4_REG (DR_REG_H264_DMA_BASE + 0x400)
/** H264_DMA_OUT_AUTO_WRBACK_CH4 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define H264_DMA_OUT_AUTO_WRBACK_CH4    (BIT(0))
#define H264_DMA_OUT_AUTO_WRBACK_CH4_M  (H264_DMA_OUT_AUTO_WRBACK_CH4_V << H264_DMA_OUT_AUTO_WRBACK_CH4_S)
#define H264_DMA_OUT_AUTO_WRBACK_CH4_V  0x00000001U
#define H264_DMA_OUT_AUTO_WRBACK_CH4_S  0
/** H264_DMA_OUT_EOF_MODE_CH4 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define H264_DMA_OUT_EOF_MODE_CH4    (BIT(1))
#define H264_DMA_OUT_EOF_MODE_CH4_M  (H264_DMA_OUT_EOF_MODE_CH4_V << H264_DMA_OUT_EOF_MODE_CH4_S)
#define H264_DMA_OUT_EOF_MODE_CH4_V  0x00000001U
#define H264_DMA_OUT_EOF_MODE_CH4_S  1
/** H264_DMA_OUTDSCR_BURST_EN_CH4 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define H264_DMA_OUTDSCR_BURST_EN_CH4    (BIT(2))
#define H264_DMA_OUTDSCR_BURST_EN_CH4_M  (H264_DMA_OUTDSCR_BURST_EN_CH4_V << H264_DMA_OUTDSCR_BURST_EN_CH4_S)
#define H264_DMA_OUTDSCR_BURST_EN_CH4_V  0x00000001U
#define H264_DMA_OUTDSCR_BURST_EN_CH4_S  2
/** H264_DMA_OUT_ECC_AES_EN_CH4 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_OUT_ECC_AES_EN_CH4    (BIT(3))
#define H264_DMA_OUT_ECC_AES_EN_CH4_M  (H264_DMA_OUT_ECC_AES_EN_CH4_V << H264_DMA_OUT_ECC_AES_EN_CH4_S)
#define H264_DMA_OUT_ECC_AES_EN_CH4_V  0x00000001U
#define H264_DMA_OUT_ECC_AES_EN_CH4_S  3
/** H264_DMA_OUT_CHECK_OWNER_CH4 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_OUT_CHECK_OWNER_CH4    (BIT(4))
#define H264_DMA_OUT_CHECK_OWNER_CH4_M  (H264_DMA_OUT_CHECK_OWNER_CH4_V << H264_DMA_OUT_CHECK_OWNER_CH4_S)
#define H264_DMA_OUT_CHECK_OWNER_CH4_V  0x00000001U
#define H264_DMA_OUT_CHECK_OWNER_CH4_S  4
/** H264_DMA_OUT_MEM_BURST_LENGTH_CH4 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 4. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH4    0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH4_M  (H264_DMA_OUT_MEM_BURST_LENGTH_CH4_V << H264_DMA_OUT_MEM_BURST_LENGTH_CH4_S)
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH4_V  0x00000007U
#define H264_DMA_OUT_MEM_BURST_LENGTH_CH4_S  6
/** H264_DMA_OUT_PAGE_BOUND_EN_CH4 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define H264_DMA_OUT_PAGE_BOUND_EN_CH4    (BIT(12))
#define H264_DMA_OUT_PAGE_BOUND_EN_CH4_M  (H264_DMA_OUT_PAGE_BOUND_EN_CH4_V << H264_DMA_OUT_PAGE_BOUND_EN_CH4_S)
#define H264_DMA_OUT_PAGE_BOUND_EN_CH4_V  0x00000001U
#define H264_DMA_OUT_PAGE_BOUND_EN_CH4_S  12
/** H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH4 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH4    (BIT(26))
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH4_M  (H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH4_V << H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH4_S)
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH4_V  0x00000001U
#define H264_DMA_OUT_ARB_WEIGHT_OPT_DIS_CH4_S  26

/** H264_DMA_OUT_INT_RAW_CH4_REG register
 *  TX CH4  interrupt raw register
 */
#define H264_DMA_OUT_INT_RAW_CH4_REG (DR_REG_H264_DMA_BASE + 0x404)
/** H264_DMA_OUT_DONE_CH4_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define H264_DMA_OUT_DONE_CH4_INT_RAW    (BIT(0))
#define H264_DMA_OUT_DONE_CH4_INT_RAW_M  (H264_DMA_OUT_DONE_CH4_INT_RAW_V << H264_DMA_OUT_DONE_CH4_INT_RAW_S)
#define H264_DMA_OUT_DONE_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DONE_CH4_INT_RAW_S  0
/** H264_DMA_OUT_EOF_CH4_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define H264_DMA_OUT_EOF_CH4_INT_RAW    (BIT(1))
#define H264_DMA_OUT_EOF_CH4_INT_RAW_M  (H264_DMA_OUT_EOF_CH4_INT_RAW_V << H264_DMA_OUT_EOF_CH4_INT_RAW_S)
#define H264_DMA_OUT_EOF_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_EOF_CH4_INT_RAW_S  1
/** H264_DMA_OUT_DSCR_ERR_CH4_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_RAW    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_RAW_M  (H264_DMA_OUT_DSCR_ERR_CH4_INT_RAW_V << H264_DMA_OUT_DSCR_ERR_CH4_INT_RAW_S)
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_RAW_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH4_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_RAW    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_RAW_M  (H264_DMA_OUT_TOTAL_EOF_CH4_INT_RAW_V << H264_DMA_OUT_TOTAL_EOF_CH4_INT_RAW_S)
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_RAW_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH4_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_RAW    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_RAW_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH4_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_RAW    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_RAW_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH4_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_RAW    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_RAW_M  (H264_DMA_OUTFIFO_OVF_L2_CH4_INT_RAW_V << H264_DMA_OUTFIFO_OVF_L2_CH4_INT_RAW_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_RAW_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH4_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_RAW    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_RAW_M  (H264_DMA_OUTFIFO_UDF_L2_CH4_INT_RAW_V << H264_DMA_OUTFIFO_UDF_L2_CH4_INT_RAW_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_RAW_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_RAW    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_RAW_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_RAW_V << H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_RAW_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_RAW_S  8

/** H264_DMA_OUT_INT_ENA_CH4_REG register
 *  TX CH4  interrupt ena register
 */
#define H264_DMA_OUT_INT_ENA_CH4_REG (DR_REG_H264_DMA_BASE + 0x408)
/** H264_DMA_OUT_DONE_CH4_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH4_INT_ENA    (BIT(0))
#define H264_DMA_OUT_DONE_CH4_INT_ENA_M  (H264_DMA_OUT_DONE_CH4_INT_ENA_V << H264_DMA_OUT_DONE_CH4_INT_ENA_S)
#define H264_DMA_OUT_DONE_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DONE_CH4_INT_ENA_S  0
/** H264_DMA_OUT_EOF_CH4_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH4_INT_ENA    (BIT(1))
#define H264_DMA_OUT_EOF_CH4_INT_ENA_M  (H264_DMA_OUT_EOF_CH4_INT_ENA_V << H264_DMA_OUT_EOF_CH4_INT_ENA_S)
#define H264_DMA_OUT_EOF_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_EOF_CH4_INT_ENA_S  1
/** H264_DMA_OUT_DSCR_ERR_CH4_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ENA    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ENA_M  (H264_DMA_OUT_DSCR_ERR_CH4_INT_ENA_V << H264_DMA_OUT_DSCR_ERR_CH4_INT_ENA_S)
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ENA_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH4_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ENA    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ENA_M  (H264_DMA_OUT_TOTAL_EOF_CH4_INT_ENA_V << H264_DMA_OUT_TOTAL_EOF_CH4_INT_ENA_S)
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ENA_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ENA    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ENA_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ENA    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ENA_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ENA    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ENA_M  (H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ENA_V << H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ENA_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ENA_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ENA    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ENA_M  (H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ENA_V << H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ENA_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ENA_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ENA    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ENA_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ENA_V << H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ENA_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ENA_S  8

/** H264_DMA_OUT_INT_ST_CH4_REG register
 *  TX CH4  interrupt st register
 */
#define H264_DMA_OUT_INT_ST_CH4_REG (DR_REG_H264_DMA_BASE + 0x40c)
/** H264_DMA_OUT_DONE_CH4_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH4_INT_ST    (BIT(0))
#define H264_DMA_OUT_DONE_CH4_INT_ST_M  (H264_DMA_OUT_DONE_CH4_INT_ST_V << H264_DMA_OUT_DONE_CH4_INT_ST_S)
#define H264_DMA_OUT_DONE_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DONE_CH4_INT_ST_S  0
/** H264_DMA_OUT_EOF_CH4_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH4_INT_ST    (BIT(1))
#define H264_DMA_OUT_EOF_CH4_INT_ST_M  (H264_DMA_OUT_EOF_CH4_INT_ST_V << H264_DMA_OUT_EOF_CH4_INT_ST_S)
#define H264_DMA_OUT_EOF_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUT_EOF_CH4_INT_ST_S  1
/** H264_DMA_OUT_DSCR_ERR_CH4_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ST    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ST_M  (H264_DMA_OUT_DSCR_ERR_CH4_INT_ST_V << H264_DMA_OUT_DSCR_ERR_CH4_INT_ST_S)
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_ST_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH4_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ST    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ST_M  (H264_DMA_OUT_TOTAL_EOF_CH4_INT_ST_V << H264_DMA_OUT_TOTAL_EOF_CH4_INT_ST_S)
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_ST_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ST    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ST_V << H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_ST_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ST    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ST_V << H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_ST_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ST    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ST_M  (H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ST_V << H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ST_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_ST_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ST    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ST_M  (H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ST_V << H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ST_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_ST_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ST    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ST_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ST_V << H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ST_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ST_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_ST_S  8

/** H264_DMA_OUT_INT_CLR_CH4_REG register
 *  TX CH4  interrupt clr register
 */
#define H264_DMA_OUT_INT_CLR_CH4_REG (DR_REG_H264_DMA_BASE + 0x410)
/** H264_DMA_OUT_DONE_CH4_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define H264_DMA_OUT_DONE_CH4_INT_CLR    (BIT(0))
#define H264_DMA_OUT_DONE_CH4_INT_CLR_M  (H264_DMA_OUT_DONE_CH4_INT_CLR_V << H264_DMA_OUT_DONE_CH4_INT_CLR_S)
#define H264_DMA_OUT_DONE_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DONE_CH4_INT_CLR_S  0
/** H264_DMA_OUT_EOF_CH4_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_EOF_CH4_INT_CLR    (BIT(1))
#define H264_DMA_OUT_EOF_CH4_INT_CLR_M  (H264_DMA_OUT_EOF_CH4_INT_CLR_V << H264_DMA_OUT_EOF_CH4_INT_CLR_S)
#define H264_DMA_OUT_EOF_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_EOF_CH4_INT_CLR_S  1
/** H264_DMA_OUT_DSCR_ERR_CH4_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_CLR    (BIT(2))
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_CLR_M  (H264_DMA_OUT_DSCR_ERR_CH4_INT_CLR_V << H264_DMA_OUT_DSCR_ERR_CH4_INT_CLR_S)
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_ERR_CH4_INT_CLR_S  2
/** H264_DMA_OUT_TOTAL_EOF_CH4_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_CLR    (BIT(3))
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_CLR_M  (H264_DMA_OUT_TOTAL_EOF_CH4_INT_CLR_V << H264_DMA_OUT_TOTAL_EOF_CH4_INT_CLR_S)
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_TOTAL_EOF_CH4_INT_CLR_S  3
/** H264_DMA_OUTFIFO_OVF_L1_CH4_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_CLR    (BIT(4))
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L1_CH4_INT_CLR_S  4
/** H264_DMA_OUTFIFO_UDF_L1_CH4_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_CLR    (BIT(5))
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L1_CH4_INT_CLR_S  5
/** H264_DMA_OUTFIFO_OVF_L2_CH4_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_CLR    (BIT(6))
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_CLR_M  (H264_DMA_OUTFIFO_OVF_L2_CH4_INT_CLR_V << H264_DMA_OUTFIFO_OVF_L2_CH4_INT_CLR_S)
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_OVF_L2_CH4_INT_CLR_S  6
/** H264_DMA_OUTFIFO_UDF_L2_CH4_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_CLR    (BIT(7))
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_CLR_M  (H264_DMA_OUTFIFO_UDF_L2_CH4_INT_CLR_V << H264_DMA_OUTFIFO_UDF_L2_CH4_INT_CLR_S)
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUTFIFO_UDF_L2_CH4_INT_CLR_S  7
/** H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_CLR    (BIT(8))
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_CLR_M  (H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_CLR_V << H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_CLR_S)
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_OUT_DSCR_TASK_OVF_CH4_INT_CLR_S  8

/** H264_DMA_OUTFIFO_STATUS_CH4_REG register
 *  TX CH4 outfifo status register
 */
#define H264_DMA_OUTFIFO_STATUS_CH4_REG (DR_REG_H264_DMA_BASE + 0x414)
/** H264_DMA_OUTFIFO_FULL_L2_CH4 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L2_CH4    (BIT(0))
#define H264_DMA_OUTFIFO_FULL_L2_CH4_M  (H264_DMA_OUTFIFO_FULL_L2_CH4_V << H264_DMA_OUTFIFO_FULL_L2_CH4_S)
#define H264_DMA_OUTFIFO_FULL_L2_CH4_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L2_CH4_S  0
/** H264_DMA_OUTFIFO_EMPTY_L2_CH4 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L2_CH4    (BIT(1))
#define H264_DMA_OUTFIFO_EMPTY_L2_CH4_M  (H264_DMA_OUTFIFO_EMPTY_L2_CH4_V << H264_DMA_OUTFIFO_EMPTY_L2_CH4_S)
#define H264_DMA_OUTFIFO_EMPTY_L2_CH4_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L2_CH4_S  1
/** H264_DMA_OUTFIFO_CNT_L2_CH4 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L2_CH4    0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH4_M  (H264_DMA_OUTFIFO_CNT_L2_CH4_V << H264_DMA_OUTFIFO_CNT_L2_CH4_S)
#define H264_DMA_OUTFIFO_CNT_L2_CH4_V  0x0000000FU
#define H264_DMA_OUTFIFO_CNT_L2_CH4_S  2
/** H264_DMA_OUTFIFO_FULL_L1_CH4 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L1_CH4    (BIT(6))
#define H264_DMA_OUTFIFO_FULL_L1_CH4_M  (H264_DMA_OUTFIFO_FULL_L1_CH4_V << H264_DMA_OUTFIFO_FULL_L1_CH4_S)
#define H264_DMA_OUTFIFO_FULL_L1_CH4_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L1_CH4_S  6
/** H264_DMA_OUTFIFO_EMPTY_L1_CH4 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L1_CH4    (BIT(7))
#define H264_DMA_OUTFIFO_EMPTY_L1_CH4_M  (H264_DMA_OUTFIFO_EMPTY_L1_CH4_V << H264_DMA_OUTFIFO_EMPTY_L1_CH4_S)
#define H264_DMA_OUTFIFO_EMPTY_L1_CH4_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L1_CH4_S  7
/** H264_DMA_OUTFIFO_CNT_L1_CH4 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L1_CH4    0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH4_M  (H264_DMA_OUTFIFO_CNT_L1_CH4_V << H264_DMA_OUTFIFO_CNT_L1_CH4_S)
#define H264_DMA_OUTFIFO_CNT_L1_CH4_V  0x0000001FU
#define H264_DMA_OUTFIFO_CNT_L1_CH4_S  8
/** H264_DMA_OUTFIFO_FULL_L3_CH4 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_FULL_L3_CH4    (BIT(16))
#define H264_DMA_OUTFIFO_FULL_L3_CH4_M  (H264_DMA_OUTFIFO_FULL_L3_CH4_V << H264_DMA_OUTFIFO_FULL_L3_CH4_S)
#define H264_DMA_OUTFIFO_FULL_L3_CH4_V  0x00000001U
#define H264_DMA_OUTFIFO_FULL_L3_CH4_S  16
/** H264_DMA_OUTFIFO_EMPTY_L3_CH4 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_EMPTY_L3_CH4    (BIT(17))
#define H264_DMA_OUTFIFO_EMPTY_L3_CH4_M  (H264_DMA_OUTFIFO_EMPTY_L3_CH4_V << H264_DMA_OUTFIFO_EMPTY_L3_CH4_S)
#define H264_DMA_OUTFIFO_EMPTY_L3_CH4_V  0x00000001U
#define H264_DMA_OUTFIFO_EMPTY_L3_CH4_S  17
/** H264_DMA_OUTFIFO_CNT_L3_CH4 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 2.
 */
#define H264_DMA_OUTFIFO_CNT_L3_CH4    0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH4_M  (H264_DMA_OUTFIFO_CNT_L3_CH4_V << H264_DMA_OUTFIFO_CNT_L3_CH4_S)
#define H264_DMA_OUTFIFO_CNT_L3_CH4_V  0x00000003U
#define H264_DMA_OUTFIFO_CNT_L3_CH4_S  18

/** H264_DMA_OUT_PUSH_CH4_REG register
 *  TX CH4 outfifo push register
 */
#define H264_DMA_OUT_PUSH_CH4_REG (DR_REG_H264_DMA_BASE + 0x418)
/** H264_DMA_OUTFIFO_WDATA_CH4 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_WDATA_CH4    0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH4_M  (H264_DMA_OUTFIFO_WDATA_CH4_V << H264_DMA_OUTFIFO_WDATA_CH4_S)
#define H264_DMA_OUTFIFO_WDATA_CH4_V  0x000003FFU
#define H264_DMA_OUTFIFO_WDATA_CH4_S  0
/** H264_DMA_OUTFIFO_PUSH_CH4 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define H264_DMA_OUTFIFO_PUSH_CH4    (BIT(10))
#define H264_DMA_OUTFIFO_PUSH_CH4_M  (H264_DMA_OUTFIFO_PUSH_CH4_V << H264_DMA_OUTFIFO_PUSH_CH4_S)
#define H264_DMA_OUTFIFO_PUSH_CH4_V  0x00000001U
#define H264_DMA_OUTFIFO_PUSH_CH4_S  10

/** H264_DMA_OUT_LINK_CONF_CH4_REG register
 *  TX CH4 out_link dscr ctrl register
 */
#define H264_DMA_OUT_LINK_CONF_CH4_REG (DR_REG_H264_DMA_BASE + 0x41c)
/** H264_DMA_OUTLINK_STOP_CH4 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_STOP_CH4    (BIT(20))
#define H264_DMA_OUTLINK_STOP_CH4_M  (H264_DMA_OUTLINK_STOP_CH4_V << H264_DMA_OUTLINK_STOP_CH4_S)
#define H264_DMA_OUTLINK_STOP_CH4_V  0x00000001U
#define H264_DMA_OUTLINK_STOP_CH4_S  20
/** H264_DMA_OUTLINK_START_CH4 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define H264_DMA_OUTLINK_START_CH4    (BIT(21))
#define H264_DMA_OUTLINK_START_CH4_M  (H264_DMA_OUTLINK_START_CH4_V << H264_DMA_OUTLINK_START_CH4_S)
#define H264_DMA_OUTLINK_START_CH4_V  0x00000001U
#define H264_DMA_OUTLINK_START_CH4_S  21
/** H264_DMA_OUTLINK_RESTART_CH4 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define H264_DMA_OUTLINK_RESTART_CH4    (BIT(22))
#define H264_DMA_OUTLINK_RESTART_CH4_M  (H264_DMA_OUTLINK_RESTART_CH4_V << H264_DMA_OUTLINK_RESTART_CH4_S)
#define H264_DMA_OUTLINK_RESTART_CH4_V  0x00000001U
#define H264_DMA_OUTLINK_RESTART_CH4_S  22
/** H264_DMA_OUTLINK_PARK_CH4 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define H264_DMA_OUTLINK_PARK_CH4    (BIT(23))
#define H264_DMA_OUTLINK_PARK_CH4_M  (H264_DMA_OUTLINK_PARK_CH4_V << H264_DMA_OUTLINK_PARK_CH4_S)
#define H264_DMA_OUTLINK_PARK_CH4_V  0x00000001U
#define H264_DMA_OUTLINK_PARK_CH4_S  23

/** H264_DMA_OUT_LINK_ADDR_CH4_REG register
 *  TX CH4 out_link dscr addr register
 */
#define H264_DMA_OUT_LINK_ADDR_CH4_REG (DR_REG_H264_DMA_BASE + 0x420)
/** H264_DMA_OUTLINK_ADDR_CH4 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_ADDR_CH4    0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH4_M  (H264_DMA_OUTLINK_ADDR_CH4_V << H264_DMA_OUTLINK_ADDR_CH4_S)
#define H264_DMA_OUTLINK_ADDR_CH4_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_ADDR_CH4_S  0

/** H264_DMA_OUT_STATE_CH4_REG register
 *  TX CH4 state register
 */
#define H264_DMA_OUT_STATE_CH4_REG (DR_REG_H264_DMA_BASE + 0x424)
/** H264_DMA_OUTLINK_DSCR_ADDR_CH4 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define H264_DMA_OUTLINK_DSCR_ADDR_CH4    0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH4_M  (H264_DMA_OUTLINK_DSCR_ADDR_CH4_V << H264_DMA_OUTLINK_DSCR_ADDR_CH4_S)
#define H264_DMA_OUTLINK_DSCR_ADDR_CH4_V  0x0003FFFFU
#define H264_DMA_OUTLINK_DSCR_ADDR_CH4_S  0
/** H264_DMA_OUT_DSCR_STATE_CH4 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_OUT_DSCR_STATE_CH4    0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH4_M  (H264_DMA_OUT_DSCR_STATE_CH4_V << H264_DMA_OUT_DSCR_STATE_CH4_S)
#define H264_DMA_OUT_DSCR_STATE_CH4_V  0x00000003U
#define H264_DMA_OUT_DSCR_STATE_CH4_S  18
/** H264_DMA_OUT_STATE_CH4 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_OUT_STATE_CH4    0x0000000FU
#define H264_DMA_OUT_STATE_CH4_M  (H264_DMA_OUT_STATE_CH4_V << H264_DMA_OUT_STATE_CH4_S)
#define H264_DMA_OUT_STATE_CH4_V  0x0000000FU
#define H264_DMA_OUT_STATE_CH4_S  20

/** H264_DMA_OUT_EOF_DES_ADDR_CH4_REG register
 *  TX CH4 eof des addr register
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH4_REG (DR_REG_H264_DMA_BASE + 0x428)
/** H264_DMA_OUT_EOF_DES_ADDR_CH4 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_OUT_EOF_DES_ADDR_CH4    0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH4_M  (H264_DMA_OUT_EOF_DES_ADDR_CH4_V << H264_DMA_OUT_EOF_DES_ADDR_CH4_S)
#define H264_DMA_OUT_EOF_DES_ADDR_CH4_V  0xFFFFFFFFU
#define H264_DMA_OUT_EOF_DES_ADDR_CH4_S  0

/** H264_DMA_OUT_DSCR_CH4_REG register
 *  TX CH4 next dscr addr register
 */
#define H264_DMA_OUT_DSCR_CH4_REG (DR_REG_H264_DMA_BASE + 0x42c)
/** H264_DMA_OUTLINK_DSCR_CH4 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define H264_DMA_OUTLINK_DSCR_CH4    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH4_M  (H264_DMA_OUTLINK_DSCR_CH4_V << H264_DMA_OUTLINK_DSCR_CH4_S)
#define H264_DMA_OUTLINK_DSCR_CH4_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_CH4_S  0

/** H264_DMA_OUT_DSCR_BF0_CH4_REG register
 *  TX CH4 last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF0_CH4_REG (DR_REG_H264_DMA_BASE + 0x430)
/** H264_DMA_OUTLINK_DSCR_BF0_CH4 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define H264_DMA_OUTLINK_DSCR_BF0_CH4    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH4_M  (H264_DMA_OUTLINK_DSCR_BF0_CH4_V << H264_DMA_OUTLINK_DSCR_BF0_CH4_S)
#define H264_DMA_OUTLINK_DSCR_BF0_CH4_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF0_CH4_S  0

/** H264_DMA_OUT_DSCR_BF1_CH4_REG register
 *  TX CH4 second-to-last dscr addr register
 */
#define H264_DMA_OUT_DSCR_BF1_CH4_REG (DR_REG_H264_DMA_BASE + 0x434)
/** H264_DMA_OUTLINK_DSCR_BF1_CH4 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define H264_DMA_OUTLINK_DSCR_BF1_CH4    0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH4_M  (H264_DMA_OUTLINK_DSCR_BF1_CH4_V << H264_DMA_OUTLINK_DSCR_BF1_CH4_S)
#define H264_DMA_OUTLINK_DSCR_BF1_CH4_V  0xFFFFFFFFU
#define H264_DMA_OUTLINK_DSCR_BF1_CH4_S  0

/** H264_DMA_OUT_ARB_CH4_REG register
 *  TX CH4 arb register
 */
#define H264_DMA_OUT_ARB_CH4_REG (DR_REG_H264_DMA_BASE + 0x43c)
/** H264_DMA_OUT_ARB_TOKEN_NUM_CH4 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH4    0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH4_M  (H264_DMA_OUT_ARB_TOKEN_NUM_CH4_V << H264_DMA_OUT_ARB_TOKEN_NUM_CH4_S)
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH4_V  0x0000000FU
#define H264_DMA_OUT_ARB_TOKEN_NUM_CH4_S  0
/** H264_DMA_EXTER_OUT_ARB_PRIORITY_CH4 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH4    0x00000003U
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH4_M  (H264_DMA_EXTER_OUT_ARB_PRIORITY_CH4_V << H264_DMA_EXTER_OUT_ARB_PRIORITY_CH4_S)
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH4_V  0x00000003U
#define H264_DMA_EXTER_OUT_ARB_PRIORITY_CH4_S  4

/** H264_DMA_OUT_ETM_CONF_CH4_REG register
 *  TX CH4 ETM config register
 */
#define H264_DMA_OUT_ETM_CONF_CH4_REG (DR_REG_H264_DMA_BASE + 0x468)
/** H264_DMA_OUT_ETM_EN_CH4 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_OUT_ETM_EN_CH4    (BIT(0))
#define H264_DMA_OUT_ETM_EN_CH4_M  (H264_DMA_OUT_ETM_EN_CH4_V << H264_DMA_OUT_ETM_EN_CH4_S)
#define H264_DMA_OUT_ETM_EN_CH4_V  0x00000001U
#define H264_DMA_OUT_ETM_EN_CH4_S  0
/** H264_DMA_OUT_ETM_LOOP_EN_CH4 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_OUT_ETM_LOOP_EN_CH4    (BIT(1))
#define H264_DMA_OUT_ETM_LOOP_EN_CH4_M  (H264_DMA_OUT_ETM_LOOP_EN_CH4_V << H264_DMA_OUT_ETM_LOOP_EN_CH4_S)
#define H264_DMA_OUT_ETM_LOOP_EN_CH4_V  0x00000001U
#define H264_DMA_OUT_ETM_LOOP_EN_CH4_S  1
/** H264_DMA_OUT_DSCR_TASK_MAK_CH4 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_OUT_DSCR_TASK_MAK_CH4    0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH4_M  (H264_DMA_OUT_DSCR_TASK_MAK_CH4_V << H264_DMA_OUT_DSCR_TASK_MAK_CH4_S)
#define H264_DMA_OUT_DSCR_TASK_MAK_CH4_V  0x00000003U
#define H264_DMA_OUT_DSCR_TASK_MAK_CH4_S  2

/** H264_DMA_OUT_BUF_LEN_CH4_REG register
 *  tx CH4 buf len register
 */
#define H264_DMA_OUT_BUF_LEN_CH4_REG (DR_REG_H264_DMA_BASE + 0x470)
/** H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH4 : RO; bitpos: [12:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH4    0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH4_M  (H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH4_V << H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH4_S)
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH4_V  0x00001FFFU
#define H264_DMA_OUT_CMDFIFO_BUF_LEN_HB_CH4_S  0

/** H264_DMA_OUT_FIFO_BCNT_CH4_REG register
 *  tx CH4 fifo byte cnt register
 */
#define H264_DMA_OUT_FIFO_BCNT_CH4_REG (DR_REG_H264_DMA_BASE + 0x474)
/** H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH4 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH4    0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH4_M  (H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH4_V << H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH4_S)
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH4_V  0x000003FFU
#define H264_DMA_OUT_CMDFIFO_OUTFIFO_BCNT_CH4_S  0

/** H264_DMA_OUT_PUSH_BYTECNT_CH4_REG register
 *  tx CH4 push byte cnt register
 */
#define H264_DMA_OUT_PUSH_BYTECNT_CH4_REG (DR_REG_H264_DMA_BASE + 0x478)
/** H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH4 : RO; bitpos: [7:0]; default: 63;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH4    0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH4_M  (H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH4_V << H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH4_S)
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH4_V  0x000000FFU
#define H264_DMA_OUT_CMDFIFO_PUSH_BYTECNT_CH4_S  0

/** H264_DMA_OUT_XADDR_CH4_REG register
 *  tx CH4 xaddr register
 */
#define H264_DMA_OUT_XADDR_CH4_REG (DR_REG_H264_DMA_BASE + 0x47c)
/** H264_DMA_OUT_CMDFIFO_XADDR_CH4 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_CMDFIFO_XADDR_CH4    0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH4_M  (H264_DMA_OUT_CMDFIFO_XADDR_CH4_V << H264_DMA_OUT_CMDFIFO_XADDR_CH4_S)
#define H264_DMA_OUT_CMDFIFO_XADDR_CH4_V  0xFFFFFFFFU
#define H264_DMA_OUT_CMDFIFO_XADDR_CH4_S  0

/** H264_DMA_OUT_BLOCK_BUF_LEN_CH4_REG register
 *  tx CH4 block buf len register
 */
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH4_REG (DR_REG_H264_DMA_BASE + 0x480)
/** H264_DMA_OUT_BLOCK_BUF_LEN_CH4 : RO; bitpos: [27:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH4    0x0FFFFFFFU
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH4_M  (H264_DMA_OUT_BLOCK_BUF_LEN_CH4_V << H264_DMA_OUT_BLOCK_BUF_LEN_CH4_S)
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH4_V  0x0FFFFFFFU
#define H264_DMA_OUT_BLOCK_BUF_LEN_CH4_S  0

/** H264_DMA_IN_CONF0_CH0_REG register
 *  RX CH0 config0 register
 */
#define H264_DMA_IN_CONF0_CH0_REG (DR_REG_H264_DMA_BASE + 0x500)
/** H264_DMA_INDSCR_BURST_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
 *  when accessing SRAM.
 */
#define H264_DMA_INDSCR_BURST_EN_CH0    (BIT(2))
#define H264_DMA_INDSCR_BURST_EN_CH0_M  (H264_DMA_INDSCR_BURST_EN_CH0_V << H264_DMA_INDSCR_BURST_EN_CH0_S)
#define H264_DMA_INDSCR_BURST_EN_CH0_V  0x00000001U
#define H264_DMA_INDSCR_BURST_EN_CH0_S  2
/** H264_DMA_IN_ECC_AES_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_IN_ECC_AES_EN_CH0    (BIT(3))
#define H264_DMA_IN_ECC_AES_EN_CH0_M  (H264_DMA_IN_ECC_AES_EN_CH0_V << H264_DMA_IN_ECC_AES_EN_CH0_S)
#define H264_DMA_IN_ECC_AES_EN_CH0_V  0x00000001U
#define H264_DMA_IN_ECC_AES_EN_CH0_S  3
/** H264_DMA_IN_CHECK_OWNER_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_IN_CHECK_OWNER_CH0    (BIT(4))
#define H264_DMA_IN_CHECK_OWNER_CH0_M  (H264_DMA_IN_CHECK_OWNER_CH0_V << H264_DMA_IN_CHECK_OWNER_CH0_S)
#define H264_DMA_IN_CHECK_OWNER_CH0_V  0x00000001U
#define H264_DMA_IN_CHECK_OWNER_CH0_S  4
/** H264_DMA_IN_MEM_BURST_LENGTH_CH0 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 0. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_IN_MEM_BURST_LENGTH_CH0    0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH0_M  (H264_DMA_IN_MEM_BURST_LENGTH_CH0_V << H264_DMA_IN_MEM_BURST_LENGTH_CH0_S)
#define H264_DMA_IN_MEM_BURST_LENGTH_CH0_V  0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH0_S  6
/** H264_DMA_IN_PAGE_BOUND_EN_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define H264_DMA_IN_PAGE_BOUND_EN_CH0    (BIT(12))
#define H264_DMA_IN_PAGE_BOUND_EN_CH0_M  (H264_DMA_IN_PAGE_BOUND_EN_CH0_V << H264_DMA_IN_PAGE_BOUND_EN_CH0_S)
#define H264_DMA_IN_PAGE_BOUND_EN_CH0_V  0x00000001U
#define H264_DMA_IN_PAGE_BOUND_EN_CH0_S  12
/** H264_DMA_IN_RST_CH0 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define H264_DMA_IN_RST_CH0    (BIT(24))
#define H264_DMA_IN_RST_CH0_M  (H264_DMA_IN_RST_CH0_V << H264_DMA_IN_RST_CH0_S)
#define H264_DMA_IN_RST_CH0_V  0x00000001U
#define H264_DMA_IN_RST_CH0_S  24
/** H264_DMA_IN_CMD_DISABLE_CH0 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_IN_CMD_DISABLE_CH0    (BIT(25))
#define H264_DMA_IN_CMD_DISABLE_CH0_M  (H264_DMA_IN_CMD_DISABLE_CH0_V << H264_DMA_IN_CMD_DISABLE_CH0_S)
#define H264_DMA_IN_CMD_DISABLE_CH0_V  0x00000001U
#define H264_DMA_IN_CMD_DISABLE_CH0_S  25
/** H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH0 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH0    (BIT(26))
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH0_M  (H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH0_V << H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH0_S)
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH0_V  0x00000001U
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH0_S  26

/** H264_DMA_IN_INT_RAW_CH0_REG register
 *  RX CH0  interrupt raw register
 */
#define H264_DMA_IN_INT_RAW_CH0_REG (DR_REG_H264_DMA_BASE + 0x504)
/** H264_DMA_IN_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 0.
 */
#define H264_DMA_IN_DONE_CH0_INT_RAW    (BIT(0))
#define H264_DMA_IN_DONE_CH0_INT_RAW_M  (H264_DMA_IN_DONE_CH0_INT_RAW_V << H264_DMA_IN_DONE_CH0_INT_RAW_S)
#define H264_DMA_IN_DONE_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DONE_CH0_INT_RAW_S  0
/** H264_DMA_IN_SUC_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 0.
 */
#define H264_DMA_IN_SUC_EOF_CH0_INT_RAW    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH0_INT_RAW_M  (H264_DMA_IN_SUC_EOF_CH0_INT_RAW_V << H264_DMA_IN_SUC_EOF_CH0_INT_RAW_S)
#define H264_DMA_IN_SUC_EOF_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH0_INT_RAW_S  1
/** H264_DMA_IN_ERR_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and data error is detected
 */
#define H264_DMA_IN_ERR_EOF_CH0_INT_RAW    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH0_INT_RAW_M  (H264_DMA_IN_ERR_EOF_CH0_INT_RAW_V << H264_DMA_IN_ERR_EOF_CH0_INT_RAW_S)
#define H264_DMA_IN_ERR_EOF_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH0_INT_RAW_S  2
/** H264_DMA_IN_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor for Rx
 *  channel 0.
 */
#define H264_DMA_IN_DSCR_ERR_CH0_INT_RAW    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH0_INT_RAW_M  (H264_DMA_IN_DSCR_ERR_CH0_INT_RAW_V << H264_DMA_IN_DSCR_ERR_CH0_INT_RAW_S)
#define H264_DMA_IN_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH0_INT_RAW_S  3
/** H264_DMA_INFIFO_OVF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_RAW    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_RAW_M  (H264_DMA_INFIFO_OVF_L1_CH0_INT_RAW_V << H264_DMA_INFIFO_OVF_L1_CH0_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_RAW_S  4
/** H264_DMA_INFIFO_UDF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_RAW    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_RAW_M  (H264_DMA_INFIFO_UDF_L1_CH0_INT_RAW_V << H264_DMA_INFIFO_UDF_L1_CH0_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_RAW_S  5
/** H264_DMA_INFIFO_OVF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_RAW    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_RAW_M  (H264_DMA_INFIFO_OVF_L2_CH0_INT_RAW_V << H264_DMA_INFIFO_OVF_L2_CH0_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_RAW_S  6
/** H264_DMA_INFIFO_UDF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_RAW    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_RAW_M  (H264_DMA_INFIFO_UDF_L2_CH0_INT_RAW_V << H264_DMA_INFIFO_UDF_L2_CH0_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_RAW_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH0_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
 *  also remain data.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_RAW    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_M  (H264_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_V << H264_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_S)
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_RAW    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_RAW_M  (H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_RAW_V << H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_RAW_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_RAW_S  9

/** H264_DMA_IN_INT_ENA_CH0_REG register
 *  RX CH0  interrupt ena register
 */
#define H264_DMA_IN_INT_ENA_CH0_REG (DR_REG_H264_DMA_BASE + 0x508)
/** H264_DMA_IN_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH0_INT_ENA    (BIT(0))
#define H264_DMA_IN_DONE_CH0_INT_ENA_M  (H264_DMA_IN_DONE_CH0_INT_ENA_V << H264_DMA_IN_DONE_CH0_INT_ENA_S)
#define H264_DMA_IN_DONE_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DONE_CH0_INT_ENA_S  0
/** H264_DMA_IN_SUC_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH0_INT_ENA    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH0_INT_ENA_M  (H264_DMA_IN_SUC_EOF_CH0_INT_ENA_V << H264_DMA_IN_SUC_EOF_CH0_INT_ENA_S)
#define H264_DMA_IN_SUC_EOF_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH0_INT_ENA_S  1
/** H264_DMA_IN_ERR_EOF_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH0_INT_ENA    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH0_INT_ENA_M  (H264_DMA_IN_ERR_EOF_CH0_INT_ENA_V << H264_DMA_IN_ERR_EOF_CH0_INT_ENA_S)
#define H264_DMA_IN_ERR_EOF_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH0_INT_ENA_S  2
/** H264_DMA_IN_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ENA    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ENA_M  (H264_DMA_IN_DSCR_ERR_CH0_INT_ENA_V << H264_DMA_IN_DSCR_ERR_CH0_INT_ENA_S)
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ENA_S  3
/** H264_DMA_INFIFO_OVF_L1_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ENA    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ENA_M  (H264_DMA_INFIFO_OVF_L1_CH0_INT_ENA_V << H264_DMA_INFIFO_OVF_L1_CH0_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ENA_S  4
/** H264_DMA_INFIFO_UDF_L1_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ENA    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ENA_M  (H264_DMA_INFIFO_UDF_L1_CH0_INT_ENA_V << H264_DMA_INFIFO_UDF_L1_CH0_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ENA_S  5
/** H264_DMA_INFIFO_OVF_L2_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ENA    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ENA_M  (H264_DMA_INFIFO_OVF_L2_CH0_INT_ENA_V << H264_DMA_INFIFO_OVF_L2_CH0_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ENA_S  6
/** H264_DMA_INFIFO_UDF_L2_CH0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ENA    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ENA_M  (H264_DMA_INFIFO_UDF_L2_CH0_INT_ENA_V << H264_DMA_INFIFO_UDF_L2_CH0_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ENA_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH0_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ENA    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_M  (H264_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_V << H264_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_S)
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ENA    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ENA_M  (H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ENA_V << H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ENA_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ENA_S  9

/** H264_DMA_IN_INT_ST_CH0_REG register
 *  RX CH0  interrupt st register
 */
#define H264_DMA_IN_INT_ST_CH0_REG (DR_REG_H264_DMA_BASE + 0x50c)
/** H264_DMA_IN_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH0_INT_ST    (BIT(0))
#define H264_DMA_IN_DONE_CH0_INT_ST_M  (H264_DMA_IN_DONE_CH0_INT_ST_V << H264_DMA_IN_DONE_CH0_INT_ST_S)
#define H264_DMA_IN_DONE_CH0_INT_ST_V  0x00000001U
#define H264_DMA_IN_DONE_CH0_INT_ST_S  0
/** H264_DMA_IN_SUC_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH0_INT_ST    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH0_INT_ST_M  (H264_DMA_IN_SUC_EOF_CH0_INT_ST_V << H264_DMA_IN_SUC_EOF_CH0_INT_ST_S)
#define H264_DMA_IN_SUC_EOF_CH0_INT_ST_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH0_INT_ST_S  1
/** H264_DMA_IN_ERR_EOF_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH0_INT_ST    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH0_INT_ST_M  (H264_DMA_IN_ERR_EOF_CH0_INT_ST_V << H264_DMA_IN_ERR_EOF_CH0_INT_ST_S)
#define H264_DMA_IN_ERR_EOF_CH0_INT_ST_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH0_INT_ST_S  2
/** H264_DMA_IN_DSCR_ERR_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ST    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ST_M  (H264_DMA_IN_DSCR_ERR_CH0_INT_ST_V << H264_DMA_IN_DSCR_ERR_CH0_INT_ST_S)
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH0_INT_ST_S  3
/** H264_DMA_INFIFO_OVF_L1_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ST    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ST_M  (H264_DMA_INFIFO_OVF_L1_CH0_INT_ST_V << H264_DMA_INFIFO_OVF_L1_CH0_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_ST_S  4
/** H264_DMA_INFIFO_UDF_L1_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ST    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ST_M  (H264_DMA_INFIFO_UDF_L1_CH0_INT_ST_V << H264_DMA_INFIFO_UDF_L1_CH0_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_ST_S  5
/** H264_DMA_INFIFO_OVF_L2_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ST    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ST_M  (H264_DMA_INFIFO_OVF_L2_CH0_INT_ST_V << H264_DMA_INFIFO_OVF_L2_CH0_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_ST_S  6
/** H264_DMA_INFIFO_UDF_L2_CH0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ST    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ST_M  (H264_DMA_INFIFO_UDF_L2_CH0_INT_ST_V << H264_DMA_INFIFO_UDF_L2_CH0_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_ST_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH0_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ST    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ST_M  (H264_DMA_IN_DSCR_EMPTY_CH0_INT_ST_V << H264_DMA_IN_DSCR_EMPTY_CH0_INT_ST_S)
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_ST_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ST    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ST_M  (H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ST_V << H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ST_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_ST_S  9

/** H264_DMA_IN_INT_CLR_CH0_REG register
 *  RX CH0  interrupt clr register
 */
#define H264_DMA_IN_INT_CLR_CH0_REG (DR_REG_H264_DMA_BASE + 0x510)
/** H264_DMA_IN_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH0_INT_CLR    (BIT(0))
#define H264_DMA_IN_DONE_CH0_INT_CLR_M  (H264_DMA_IN_DONE_CH0_INT_CLR_V << H264_DMA_IN_DONE_CH0_INT_CLR_S)
#define H264_DMA_IN_DONE_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DONE_CH0_INT_CLR_S  0
/** H264_DMA_IN_SUC_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH0_INT_CLR    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH0_INT_CLR_M  (H264_DMA_IN_SUC_EOF_CH0_INT_CLR_V << H264_DMA_IN_SUC_EOF_CH0_INT_CLR_S)
#define H264_DMA_IN_SUC_EOF_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH0_INT_CLR_S  1
/** H264_DMA_IN_ERR_EOF_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH0_INT_CLR    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH0_INT_CLR_M  (H264_DMA_IN_ERR_EOF_CH0_INT_CLR_V << H264_DMA_IN_ERR_EOF_CH0_INT_CLR_S)
#define H264_DMA_IN_ERR_EOF_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH0_INT_CLR_S  2
/** H264_DMA_IN_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH0_INT_CLR    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH0_INT_CLR_M  (H264_DMA_IN_DSCR_ERR_CH0_INT_CLR_V << H264_DMA_IN_DSCR_ERR_CH0_INT_CLR_S)
#define H264_DMA_IN_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH0_INT_CLR_S  3
/** H264_DMA_INFIFO_OVF_L1_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_CLR    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_CLR_M  (H264_DMA_INFIFO_OVF_L1_CH0_INT_CLR_V << H264_DMA_INFIFO_OVF_L1_CH0_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH0_INT_CLR_S  4
/** H264_DMA_INFIFO_UDF_L1_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_CLR    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_CLR_M  (H264_DMA_INFIFO_UDF_L1_CH0_INT_CLR_V << H264_DMA_INFIFO_UDF_L1_CH0_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH0_INT_CLR_S  5
/** H264_DMA_INFIFO_OVF_L2_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_CLR    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_CLR_M  (H264_DMA_INFIFO_OVF_L2_CH0_INT_CLR_V << H264_DMA_INFIFO_OVF_L2_CH0_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH0_INT_CLR_S  6
/** H264_DMA_INFIFO_UDF_L2_CH0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_CLR    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_CLR_M  (H264_DMA_INFIFO_UDF_L2_CH0_INT_CLR_V << H264_DMA_INFIFO_UDF_L2_CH0_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH0_INT_CLR_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH0_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_CLR    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_M  (H264_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_V << H264_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_S)
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_CLR    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_CLR_M  (H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_CLR_V << H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_CLR_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH0_INT_CLR_S  9

/** H264_DMA_INFIFO_STATUS_CH0_REG register
 *  RX CH0 INFIFO status register
 */
#define H264_DMA_INFIFO_STATUS_CH0_REG (DR_REG_H264_DMA_BASE + 0x514)
/** H264_DMA_INFIFO_FULL_L2_CH0 : RO; bitpos: [0]; default: 0;
 *  Rx FIFO full signal for Rx channel.
 */
#define H264_DMA_INFIFO_FULL_L2_CH0    (BIT(0))
#define H264_DMA_INFIFO_FULL_L2_CH0_M  (H264_DMA_INFIFO_FULL_L2_CH0_V << H264_DMA_INFIFO_FULL_L2_CH0_S)
#define H264_DMA_INFIFO_FULL_L2_CH0_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L2_CH0_S  0
/** H264_DMA_INFIFO_EMPTY_L2_CH0 : RO; bitpos: [1]; default: 1;
 *  Rx FIFO empty signal for Rx channel.
 */
#define H264_DMA_INFIFO_EMPTY_L2_CH0    (BIT(1))
#define H264_DMA_INFIFO_EMPTY_L2_CH0_M  (H264_DMA_INFIFO_EMPTY_L2_CH0_V << H264_DMA_INFIFO_EMPTY_L2_CH0_S)
#define H264_DMA_INFIFO_EMPTY_L2_CH0_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L2_CH0_S  1
/** H264_DMA_INFIFO_CNT_L2_CH0 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Rx FIFO for Rx channel.
 */
#define H264_DMA_INFIFO_CNT_L2_CH0    0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH0_M  (H264_DMA_INFIFO_CNT_L2_CH0_V << H264_DMA_INFIFO_CNT_L2_CH0_S)
#define H264_DMA_INFIFO_CNT_L2_CH0_V  0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH0_S  2
/** H264_DMA_INFIFO_FULL_L1_CH0 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define H264_DMA_INFIFO_FULL_L1_CH0    (BIT(6))
#define H264_DMA_INFIFO_FULL_L1_CH0_M  (H264_DMA_INFIFO_FULL_L1_CH0_V << H264_DMA_INFIFO_FULL_L1_CH0_S)
#define H264_DMA_INFIFO_FULL_L1_CH0_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L1_CH0_S  6
/** H264_DMA_INFIFO_EMPTY_L1_CH0 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define H264_DMA_INFIFO_EMPTY_L1_CH0    (BIT(7))
#define H264_DMA_INFIFO_EMPTY_L1_CH0_M  (H264_DMA_INFIFO_EMPTY_L1_CH0_V << H264_DMA_INFIFO_EMPTY_L1_CH0_S)
#define H264_DMA_INFIFO_EMPTY_L1_CH0_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L1_CH0_S  7
/** H264_DMA_INFIFO_CNT_L1_CH0 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define H264_DMA_INFIFO_CNT_L1_CH0    0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH0_M  (H264_DMA_INFIFO_CNT_L1_CH0_V << H264_DMA_INFIFO_CNT_L1_CH0_S)
#define H264_DMA_INFIFO_CNT_L1_CH0_V  0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH0_S  8
/** H264_DMA_INFIFO_FULL_L3_CH0 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define H264_DMA_INFIFO_FULL_L3_CH0    (BIT(16))
#define H264_DMA_INFIFO_FULL_L3_CH0_M  (H264_DMA_INFIFO_FULL_L3_CH0_V << H264_DMA_INFIFO_FULL_L3_CH0_S)
#define H264_DMA_INFIFO_FULL_L3_CH0_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L3_CH0_S  16
/** H264_DMA_INFIFO_EMPTY_L3_CH0 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define H264_DMA_INFIFO_EMPTY_L3_CH0    (BIT(17))
#define H264_DMA_INFIFO_EMPTY_L3_CH0_M  (H264_DMA_INFIFO_EMPTY_L3_CH0_V << H264_DMA_INFIFO_EMPTY_L3_CH0_S)
#define H264_DMA_INFIFO_EMPTY_L3_CH0_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L3_CH0_S  17
/** H264_DMA_INFIFO_CNT_L3_CH0 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the 8byte number of the data in Tx FIFO for Tx channel 0.
 */
#define H264_DMA_INFIFO_CNT_L3_CH0    0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH0_M  (H264_DMA_INFIFO_CNT_L3_CH0_V << H264_DMA_INFIFO_CNT_L3_CH0_S)
#define H264_DMA_INFIFO_CNT_L3_CH0_V  0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH0_S  18

/** H264_DMA_IN_POP_CH0_REG register
 *  RX CH0 INFIFO pop register
 */
#define H264_DMA_IN_POP_CH0_REG (DR_REG_H264_DMA_BASE + 0x518)
/** H264_DMA_INFIFO_RDATA_CH0 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_RDATA_CH0    0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH0_M  (H264_DMA_INFIFO_RDATA_CH0_V << H264_DMA_INFIFO_RDATA_CH0_S)
#define H264_DMA_INFIFO_RDATA_CH0_V  0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH0_S  0
/** H264_DMA_INFIFO_POP_CH0 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_POP_CH0    (BIT(11))
#define H264_DMA_INFIFO_POP_CH0_M  (H264_DMA_INFIFO_POP_CH0_V << H264_DMA_INFIFO_POP_CH0_S)
#define H264_DMA_INFIFO_POP_CH0_V  0x00000001U
#define H264_DMA_INFIFO_POP_CH0_S  11

/** H264_DMA_IN_LINK_CONF_CH0_REG register
 *  RX CH0 in_link dscr ctrl register
 */
#define H264_DMA_IN_LINK_CONF_CH0_REG (DR_REG_H264_DMA_BASE + 0x51c)
/** H264_DMA_INLINK_AUTO_RET_CH0 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_INLINK_AUTO_RET_CH0    (BIT(20))
#define H264_DMA_INLINK_AUTO_RET_CH0_M  (H264_DMA_INLINK_AUTO_RET_CH0_V << H264_DMA_INLINK_AUTO_RET_CH0_S)
#define H264_DMA_INLINK_AUTO_RET_CH0_V  0x00000001U
#define H264_DMA_INLINK_AUTO_RET_CH0_S  20
/** H264_DMA_INLINK_STOP_CH0 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_STOP_CH0    (BIT(21))
#define H264_DMA_INLINK_STOP_CH0_M  (H264_DMA_INLINK_STOP_CH0_V << H264_DMA_INLINK_STOP_CH0_S)
#define H264_DMA_INLINK_STOP_CH0_V  0x00000001U
#define H264_DMA_INLINK_STOP_CH0_S  21
/** H264_DMA_INLINK_START_CH0 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_START_CH0    (BIT(22))
#define H264_DMA_INLINK_START_CH0_M  (H264_DMA_INLINK_START_CH0_V << H264_DMA_INLINK_START_CH0_S)
#define H264_DMA_INLINK_START_CH0_V  0x00000001U
#define H264_DMA_INLINK_START_CH0_S  22
/** H264_DMA_INLINK_RESTART_CH0 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define H264_DMA_INLINK_RESTART_CH0    (BIT(23))
#define H264_DMA_INLINK_RESTART_CH0_M  (H264_DMA_INLINK_RESTART_CH0_V << H264_DMA_INLINK_RESTART_CH0_S)
#define H264_DMA_INLINK_RESTART_CH0_V  0x00000001U
#define H264_DMA_INLINK_RESTART_CH0_S  23
/** H264_DMA_INLINK_PARK_CH0 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define H264_DMA_INLINK_PARK_CH0    (BIT(24))
#define H264_DMA_INLINK_PARK_CH0_M  (H264_DMA_INLINK_PARK_CH0_V << H264_DMA_INLINK_PARK_CH0_S)
#define H264_DMA_INLINK_PARK_CH0_V  0x00000001U
#define H264_DMA_INLINK_PARK_CH0_S  24

/** H264_DMA_IN_LINK_ADDR_CH0_REG register
 *  RX CH0 in_link dscr addr register
 */
#define H264_DMA_IN_LINK_ADDR_CH0_REG (DR_REG_H264_DMA_BASE + 0x520)
/** H264_DMA_INLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first inlink descriptor's address.
 */
#define H264_DMA_INLINK_ADDR_CH0    0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH0_M  (H264_DMA_INLINK_ADDR_CH0_V << H264_DMA_INLINK_ADDR_CH0_S)
#define H264_DMA_INLINK_ADDR_CH0_V  0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH0_S  0

/** H264_DMA_IN_STATE_CH0_REG register
 *  RX CH0 state register
 */
#define H264_DMA_IN_STATE_CH0_REG (DR_REG_H264_DMA_BASE + 0x524)
/** H264_DMA_INLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define H264_DMA_INLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH0_M  (H264_DMA_INLINK_DSCR_ADDR_CH0_V << H264_DMA_INLINK_DSCR_ADDR_CH0_S)
#define H264_DMA_INLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH0_S  0
/** H264_DMA_IN_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_IN_DSCR_STATE_CH0    0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH0_M  (H264_DMA_IN_DSCR_STATE_CH0_V << H264_DMA_IN_DSCR_STATE_CH0_S)
#define H264_DMA_IN_DSCR_STATE_CH0_V  0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH0_S  18
/** H264_DMA_IN_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_IN_STATE_CH0    0x00000007U
#define H264_DMA_IN_STATE_CH0_M  (H264_DMA_IN_STATE_CH0_V << H264_DMA_IN_STATE_CH0_S)
#define H264_DMA_IN_STATE_CH0_V  0x00000007U
#define H264_DMA_IN_STATE_CH0_S  20
/** H264_DMA_IN_RESET_AVAIL_CH0 : RO; bitpos: [23]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_IN_RESET_AVAIL_CH0    (BIT(23))
#define H264_DMA_IN_RESET_AVAIL_CH0_M  (H264_DMA_IN_RESET_AVAIL_CH0_V << H264_DMA_IN_RESET_AVAIL_CH0_S)
#define H264_DMA_IN_RESET_AVAIL_CH0_V  0x00000001U
#define H264_DMA_IN_RESET_AVAIL_CH0_S  23

/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH0_REG register
 *  RX CH0 eof des addr register
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH0_REG (DR_REG_H264_DMA_BASE + 0x528)
/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH0_M  (H264_DMA_IN_SUC_EOF_DES_ADDR_CH0_V << H264_DMA_IN_SUC_EOF_DES_ADDR_CH0_S)
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH0_S  0

/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH0_REG register
 *  RX CH0 err eof des addr register
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH0_REG (DR_REG_H264_DMA_BASE + 0x52c)
/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH0_M  (H264_DMA_IN_ERR_EOF_DES_ADDR_CH0_V << H264_DMA_IN_ERR_EOF_DES_ADDR_CH0_S)
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH0_S  0

/** H264_DMA_IN_DSCR_CH0_REG register
 *  RX CH0 next dscr addr register
 */
#define H264_DMA_IN_DSCR_CH0_REG (DR_REG_H264_DMA_BASE + 0x530)
/** H264_DMA_INLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next inlink descriptor address x.
 */
#define H264_DMA_INLINK_DSCR_CH0    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH0_M  (H264_DMA_INLINK_DSCR_CH0_V << H264_DMA_INLINK_DSCR_CH0_S)
#define H264_DMA_INLINK_DSCR_CH0_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH0_S  0

/** H264_DMA_IN_DSCR_BF0_CH0_REG register
 *  RX CH0 last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF0_CH0_REG (DR_REG_H264_DMA_BASE + 0x534)
/** H264_DMA_INLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor's next address x-1.
 */
#define H264_DMA_INLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH0_M  (H264_DMA_INLINK_DSCR_BF0_CH0_V << H264_DMA_INLINK_DSCR_BF0_CH0_S)
#define H264_DMA_INLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH0_S  0

/** H264_DMA_IN_DSCR_BF1_CH0_REG register
 *  RX CH0 second-to-last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF1_CH0_REG (DR_REG_H264_DMA_BASE + 0x538)
/** H264_DMA_INLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor's next address x-2.
 */
#define H264_DMA_INLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH0_M  (H264_DMA_INLINK_DSCR_BF1_CH0_V << H264_DMA_INLINK_DSCR_BF1_CH0_S)
#define H264_DMA_INLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH0_S  0

/** H264_DMA_IN_ARB_CH0_REG register
 *  RX CH0 arb register
 */
#define H264_DMA_IN_ARB_CH0_REG (DR_REG_H264_DMA_BASE + 0x540)
/** H264_DMA_IN_ARB_TOKEN_NUM_CH0 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_IN_ARB_TOKEN_NUM_CH0    0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH0_M  (H264_DMA_IN_ARB_TOKEN_NUM_CH0_V << H264_DMA_IN_ARB_TOKEN_NUM_CH0_S)
#define H264_DMA_IN_ARB_TOKEN_NUM_CH0_V  0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH0_S  0
/** H264_DMA_EXTER_IN_ARB_PRIORITY_CH0 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH0    0x00000003U
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH0_M  (H264_DMA_EXTER_IN_ARB_PRIORITY_CH0_V << H264_DMA_EXTER_IN_ARB_PRIORITY_CH0_S)
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH0_V  0x00000003U
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH0_S  4
/** H264_DMA_INTER_IN_ARB_PRIORITY_CH0 : R/W; bitpos: [8:6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH0    0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH0_M  (H264_DMA_INTER_IN_ARB_PRIORITY_CH0_V << H264_DMA_INTER_IN_ARB_PRIORITY_CH0_S)
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH0_V  0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH0_S  6

/** H264_DMA_IN_RO_PD_CONF_CH0_REG register
 *  RX CH0 reorder power config register
 */
#define H264_DMA_IN_RO_PD_CONF_CH0_REG (DR_REG_H264_DMA_BASE + 0x548)
/** H264_DMA_IN_RO_RAM_CLK_FO_CH0 : R/W; bitpos: [6]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
 *  0: A gate-clock will be used when accessing the RAM in DMA.
 */
#define H264_DMA_IN_RO_RAM_CLK_FO_CH0    (BIT(6))
#define H264_DMA_IN_RO_RAM_CLK_FO_CH0_M  (H264_DMA_IN_RO_RAM_CLK_FO_CH0_V << H264_DMA_IN_RO_RAM_CLK_FO_CH0_S)
#define H264_DMA_IN_RO_RAM_CLK_FO_CH0_V  0x00000001U
#define H264_DMA_IN_RO_RAM_CLK_FO_CH0_S  6

/** H264_DMA_IN_ETM_CONF_CH0_REG register
 *  RX CH0 ETM config register
 */
#define H264_DMA_IN_ETM_CONF_CH0_REG (DR_REG_H264_DMA_BASE + 0x56c)
/** H264_DMA_IN_ETM_EN_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_IN_ETM_EN_CH0    (BIT(0))
#define H264_DMA_IN_ETM_EN_CH0_M  (H264_DMA_IN_ETM_EN_CH0_V << H264_DMA_IN_ETM_EN_CH0_S)
#define H264_DMA_IN_ETM_EN_CH0_V  0x00000001U
#define H264_DMA_IN_ETM_EN_CH0_S  0
/** H264_DMA_IN_ETM_LOOP_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_IN_ETM_LOOP_EN_CH0    (BIT(1))
#define H264_DMA_IN_ETM_LOOP_EN_CH0_M  (H264_DMA_IN_ETM_LOOP_EN_CH0_V << H264_DMA_IN_ETM_LOOP_EN_CH0_S)
#define H264_DMA_IN_ETM_LOOP_EN_CH0_V  0x00000001U
#define H264_DMA_IN_ETM_LOOP_EN_CH0_S  1
/** H264_DMA_IN_DSCR_TASK_MAK_CH0 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_IN_DSCR_TASK_MAK_CH0    0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH0_M  (H264_DMA_IN_DSCR_TASK_MAK_CH0_V << H264_DMA_IN_DSCR_TASK_MAK_CH0_S)
#define H264_DMA_IN_DSCR_TASK_MAK_CH0_V  0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH0_S  2

/** H264_DMA_IN_FIFO_CNT_CH0_REG register
 *  rx CH0 fifo cnt register
 */
#define H264_DMA_IN_FIFO_CNT_CH0_REG (DR_REG_H264_DMA_BASE + 0x580)
/** H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH0 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH0    0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH0_M  (H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH0_V << H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH0_S)
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH0_V  0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH0_S  0

/** H264_DMA_IN_POP_DATA_CNT_CH0_REG register
 *  rx CH0 pop data cnt register
 */
#define H264_DMA_IN_POP_DATA_CNT_CH0_REG (DR_REG_H264_DMA_BASE + 0x584)
/** H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH0 : RO; bitpos: [7:0]; default: 7;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH0    0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH0_M  (H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH0_V << H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH0_S)
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH0_V  0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH0_S  0

/** H264_DMA_IN_XADDR_CH0_REG register
 *  rx CH0 xaddr register
 */
#define H264_DMA_IN_XADDR_CH0_REG (DR_REG_H264_DMA_BASE + 0x588)
/** H264_DMA_IN_CMDFIFO_XADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_XADDR_CH0    0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH0_M  (H264_DMA_IN_CMDFIFO_XADDR_CH0_V << H264_DMA_IN_CMDFIFO_XADDR_CH0_S)
#define H264_DMA_IN_CMDFIFO_XADDR_CH0_V  0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH0_S  0

/** H264_DMA_IN_BUF_HB_RCV_CH0_REG register
 *  rx CH0 buf len hb rcv register
 */
#define H264_DMA_IN_BUF_HB_RCV_CH0_REG (DR_REG_H264_DMA_BASE + 0x58c)
/** H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH0 : RO; bitpos: [28:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH0    0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH0_M  (H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH0_V << H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH0_S)
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH0_V  0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH0_S  0

/** H264_DMA_IN_CONF0_CH1_REG register
 *  RX CH1 config0 register
 */
#define H264_DMA_IN_CONF0_CH1_REG (DR_REG_H264_DMA_BASE + 0x600)
/** H264_DMA_INDSCR_BURST_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
 *  when accessing SRAM.
 */
#define H264_DMA_INDSCR_BURST_EN_CH1    (BIT(2))
#define H264_DMA_INDSCR_BURST_EN_CH1_M  (H264_DMA_INDSCR_BURST_EN_CH1_V << H264_DMA_INDSCR_BURST_EN_CH1_S)
#define H264_DMA_INDSCR_BURST_EN_CH1_V  0x00000001U
#define H264_DMA_INDSCR_BURST_EN_CH1_S  2
/** H264_DMA_IN_ECC_AES_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_IN_ECC_AES_EN_CH1    (BIT(3))
#define H264_DMA_IN_ECC_AES_EN_CH1_M  (H264_DMA_IN_ECC_AES_EN_CH1_V << H264_DMA_IN_ECC_AES_EN_CH1_S)
#define H264_DMA_IN_ECC_AES_EN_CH1_V  0x00000001U
#define H264_DMA_IN_ECC_AES_EN_CH1_S  3
/** H264_DMA_IN_CHECK_OWNER_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_IN_CHECK_OWNER_CH1    (BIT(4))
#define H264_DMA_IN_CHECK_OWNER_CH1_M  (H264_DMA_IN_CHECK_OWNER_CH1_V << H264_DMA_IN_CHECK_OWNER_CH1_S)
#define H264_DMA_IN_CHECK_OWNER_CH1_V  0x00000001U
#define H264_DMA_IN_CHECK_OWNER_CH1_S  4
/** H264_DMA_IN_MEM_BURST_LENGTH_CH1 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_IN_MEM_BURST_LENGTH_CH1    0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH1_M  (H264_DMA_IN_MEM_BURST_LENGTH_CH1_V << H264_DMA_IN_MEM_BURST_LENGTH_CH1_S)
#define H264_DMA_IN_MEM_BURST_LENGTH_CH1_V  0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH1_S  6
/** H264_DMA_IN_PAGE_BOUND_EN_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define H264_DMA_IN_PAGE_BOUND_EN_CH1    (BIT(12))
#define H264_DMA_IN_PAGE_BOUND_EN_CH1_M  (H264_DMA_IN_PAGE_BOUND_EN_CH1_V << H264_DMA_IN_PAGE_BOUND_EN_CH1_S)
#define H264_DMA_IN_PAGE_BOUND_EN_CH1_V  0x00000001U
#define H264_DMA_IN_PAGE_BOUND_EN_CH1_S  12
/** H264_DMA_IN_RST_CH1 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define H264_DMA_IN_RST_CH1    (BIT(24))
#define H264_DMA_IN_RST_CH1_M  (H264_DMA_IN_RST_CH1_V << H264_DMA_IN_RST_CH1_S)
#define H264_DMA_IN_RST_CH1_V  0x00000001U
#define H264_DMA_IN_RST_CH1_S  24
/** H264_DMA_IN_CMD_DISABLE_CH1 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_IN_CMD_DISABLE_CH1    (BIT(25))
#define H264_DMA_IN_CMD_DISABLE_CH1_M  (H264_DMA_IN_CMD_DISABLE_CH1_V << H264_DMA_IN_CMD_DISABLE_CH1_S)
#define H264_DMA_IN_CMD_DISABLE_CH1_V  0x00000001U
#define H264_DMA_IN_CMD_DISABLE_CH1_S  25
/** H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH1 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH1    (BIT(26))
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH1_M  (H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH1_V << H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH1_S)
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH1_V  0x00000001U
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH1_S  26

/** H264_DMA_IN_INT_RAW_CH1_REG register
 *  RX CH1  interrupt raw register
 */
#define H264_DMA_IN_INT_RAW_CH1_REG (DR_REG_H264_DMA_BASE + 0x604)
/** H264_DMA_IN_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 1.
 */
#define H264_DMA_IN_DONE_CH1_INT_RAW    (BIT(0))
#define H264_DMA_IN_DONE_CH1_INT_RAW_M  (H264_DMA_IN_DONE_CH1_INT_RAW_V << H264_DMA_IN_DONE_CH1_INT_RAW_S)
#define H264_DMA_IN_DONE_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DONE_CH1_INT_RAW_S  0
/** H264_DMA_IN_SUC_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 1.
 */
#define H264_DMA_IN_SUC_EOF_CH1_INT_RAW    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH1_INT_RAW_M  (H264_DMA_IN_SUC_EOF_CH1_INT_RAW_V << H264_DMA_IN_SUC_EOF_CH1_INT_RAW_S)
#define H264_DMA_IN_SUC_EOF_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH1_INT_RAW_S  1
/** H264_DMA_IN_ERR_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and data error is detected
 */
#define H264_DMA_IN_ERR_EOF_CH1_INT_RAW    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH1_INT_RAW_M  (H264_DMA_IN_ERR_EOF_CH1_INT_RAW_V << H264_DMA_IN_ERR_EOF_CH1_INT_RAW_S)
#define H264_DMA_IN_ERR_EOF_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH1_INT_RAW_S  2
/** H264_DMA_IN_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor for Rx
 *  channel 1.
 */
#define H264_DMA_IN_DSCR_ERR_CH1_INT_RAW    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH1_INT_RAW_M  (H264_DMA_IN_DSCR_ERR_CH1_INT_RAW_V << H264_DMA_IN_DSCR_ERR_CH1_INT_RAW_S)
#define H264_DMA_IN_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH1_INT_RAW_S  3
/** H264_DMA_INFIFO_OVF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_RAW    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_RAW_M  (H264_DMA_INFIFO_OVF_L1_CH1_INT_RAW_V << H264_DMA_INFIFO_OVF_L1_CH1_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_RAW_S  4
/** H264_DMA_INFIFO_UDF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_RAW    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_RAW_M  (H264_DMA_INFIFO_UDF_L1_CH1_INT_RAW_V << H264_DMA_INFIFO_UDF_L1_CH1_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_RAW_S  5
/** H264_DMA_INFIFO_OVF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_RAW    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_RAW_M  (H264_DMA_INFIFO_OVF_L2_CH1_INT_RAW_V << H264_DMA_INFIFO_OVF_L2_CH1_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_RAW_S  6
/** H264_DMA_INFIFO_UDF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_RAW    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_RAW_M  (H264_DMA_INFIFO_UDF_L2_CH1_INT_RAW_V << H264_DMA_INFIFO_UDF_L2_CH1_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_RAW_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH1_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
 *  also remain data.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_RAW    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_M  (H264_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_V << H264_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_S)
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_RAW    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_RAW_M  (H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_RAW_V << H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_RAW_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_RAW_S  9

/** H264_DMA_IN_INT_ENA_CH1_REG register
 *  RX CH1  interrupt ena register
 */
#define H264_DMA_IN_INT_ENA_CH1_REG (DR_REG_H264_DMA_BASE + 0x608)
/** H264_DMA_IN_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH1_INT_ENA    (BIT(0))
#define H264_DMA_IN_DONE_CH1_INT_ENA_M  (H264_DMA_IN_DONE_CH1_INT_ENA_V << H264_DMA_IN_DONE_CH1_INT_ENA_S)
#define H264_DMA_IN_DONE_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DONE_CH1_INT_ENA_S  0
/** H264_DMA_IN_SUC_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH1_INT_ENA    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH1_INT_ENA_M  (H264_DMA_IN_SUC_EOF_CH1_INT_ENA_V << H264_DMA_IN_SUC_EOF_CH1_INT_ENA_S)
#define H264_DMA_IN_SUC_EOF_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH1_INT_ENA_S  1
/** H264_DMA_IN_ERR_EOF_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH1_INT_ENA    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH1_INT_ENA_M  (H264_DMA_IN_ERR_EOF_CH1_INT_ENA_V << H264_DMA_IN_ERR_EOF_CH1_INT_ENA_S)
#define H264_DMA_IN_ERR_EOF_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH1_INT_ENA_S  2
/** H264_DMA_IN_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ENA    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ENA_M  (H264_DMA_IN_DSCR_ERR_CH1_INT_ENA_V << H264_DMA_IN_DSCR_ERR_CH1_INT_ENA_S)
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ENA_S  3
/** H264_DMA_INFIFO_OVF_L1_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ENA    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ENA_M  (H264_DMA_INFIFO_OVF_L1_CH1_INT_ENA_V << H264_DMA_INFIFO_OVF_L1_CH1_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ENA_S  4
/** H264_DMA_INFIFO_UDF_L1_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ENA    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ENA_M  (H264_DMA_INFIFO_UDF_L1_CH1_INT_ENA_V << H264_DMA_INFIFO_UDF_L1_CH1_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ENA_S  5
/** H264_DMA_INFIFO_OVF_L2_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ENA    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ENA_M  (H264_DMA_INFIFO_OVF_L2_CH1_INT_ENA_V << H264_DMA_INFIFO_OVF_L2_CH1_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ENA_S  6
/** H264_DMA_INFIFO_UDF_L2_CH1_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ENA    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ENA_M  (H264_DMA_INFIFO_UDF_L2_CH1_INT_ENA_V << H264_DMA_INFIFO_UDF_L2_CH1_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ENA_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ENA    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_M  (H264_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_V << H264_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_S)
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ENA    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ENA_M  (H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ENA_V << H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ENA_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ENA_S  9

/** H264_DMA_IN_INT_ST_CH1_REG register
 *  RX CH1  interrupt st register
 */
#define H264_DMA_IN_INT_ST_CH1_REG (DR_REG_H264_DMA_BASE + 0x60c)
/** H264_DMA_IN_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH1_INT_ST    (BIT(0))
#define H264_DMA_IN_DONE_CH1_INT_ST_M  (H264_DMA_IN_DONE_CH1_INT_ST_V << H264_DMA_IN_DONE_CH1_INT_ST_S)
#define H264_DMA_IN_DONE_CH1_INT_ST_V  0x00000001U
#define H264_DMA_IN_DONE_CH1_INT_ST_S  0
/** H264_DMA_IN_SUC_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH1_INT_ST    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH1_INT_ST_M  (H264_DMA_IN_SUC_EOF_CH1_INT_ST_V << H264_DMA_IN_SUC_EOF_CH1_INT_ST_S)
#define H264_DMA_IN_SUC_EOF_CH1_INT_ST_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH1_INT_ST_S  1
/** H264_DMA_IN_ERR_EOF_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH1_INT_ST    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH1_INT_ST_M  (H264_DMA_IN_ERR_EOF_CH1_INT_ST_V << H264_DMA_IN_ERR_EOF_CH1_INT_ST_S)
#define H264_DMA_IN_ERR_EOF_CH1_INT_ST_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH1_INT_ST_S  2
/** H264_DMA_IN_DSCR_ERR_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ST    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ST_M  (H264_DMA_IN_DSCR_ERR_CH1_INT_ST_V << H264_DMA_IN_DSCR_ERR_CH1_INT_ST_S)
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH1_INT_ST_S  3
/** H264_DMA_INFIFO_OVF_L1_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ST    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ST_M  (H264_DMA_INFIFO_OVF_L1_CH1_INT_ST_V << H264_DMA_INFIFO_OVF_L1_CH1_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_ST_S  4
/** H264_DMA_INFIFO_UDF_L1_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ST    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ST_M  (H264_DMA_INFIFO_UDF_L1_CH1_INT_ST_V << H264_DMA_INFIFO_UDF_L1_CH1_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_ST_S  5
/** H264_DMA_INFIFO_OVF_L2_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ST    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ST_M  (H264_DMA_INFIFO_OVF_L2_CH1_INT_ST_V << H264_DMA_INFIFO_OVF_L2_CH1_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_ST_S  6
/** H264_DMA_INFIFO_UDF_L2_CH1_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ST    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ST_M  (H264_DMA_INFIFO_UDF_L2_CH1_INT_ST_V << H264_DMA_INFIFO_UDF_L2_CH1_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_ST_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ST    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ST_M  (H264_DMA_IN_DSCR_EMPTY_CH1_INT_ST_V << H264_DMA_IN_DSCR_EMPTY_CH1_INT_ST_S)
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_ST_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ST    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ST_M  (H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ST_V << H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ST_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_ST_S  9

/** H264_DMA_IN_INT_CLR_CH1_REG register
 *  RX CH1  interrupt clr register
 */
#define H264_DMA_IN_INT_CLR_CH1_REG (DR_REG_H264_DMA_BASE + 0x610)
/** H264_DMA_IN_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH1_INT_CLR    (BIT(0))
#define H264_DMA_IN_DONE_CH1_INT_CLR_M  (H264_DMA_IN_DONE_CH1_INT_CLR_V << H264_DMA_IN_DONE_CH1_INT_CLR_S)
#define H264_DMA_IN_DONE_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DONE_CH1_INT_CLR_S  0
/** H264_DMA_IN_SUC_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH1_INT_CLR    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH1_INT_CLR_M  (H264_DMA_IN_SUC_EOF_CH1_INT_CLR_V << H264_DMA_IN_SUC_EOF_CH1_INT_CLR_S)
#define H264_DMA_IN_SUC_EOF_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH1_INT_CLR_S  1
/** H264_DMA_IN_ERR_EOF_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH1_INT_CLR    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH1_INT_CLR_M  (H264_DMA_IN_ERR_EOF_CH1_INT_CLR_V << H264_DMA_IN_ERR_EOF_CH1_INT_CLR_S)
#define H264_DMA_IN_ERR_EOF_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH1_INT_CLR_S  2
/** H264_DMA_IN_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH1_INT_CLR    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH1_INT_CLR_M  (H264_DMA_IN_DSCR_ERR_CH1_INT_CLR_V << H264_DMA_IN_DSCR_ERR_CH1_INT_CLR_S)
#define H264_DMA_IN_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH1_INT_CLR_S  3
/** H264_DMA_INFIFO_OVF_L1_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_CLR    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_CLR_M  (H264_DMA_INFIFO_OVF_L1_CH1_INT_CLR_V << H264_DMA_INFIFO_OVF_L1_CH1_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH1_INT_CLR_S  4
/** H264_DMA_INFIFO_UDF_L1_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_CLR    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_CLR_M  (H264_DMA_INFIFO_UDF_L1_CH1_INT_CLR_V << H264_DMA_INFIFO_UDF_L1_CH1_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH1_INT_CLR_S  5
/** H264_DMA_INFIFO_OVF_L2_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_CLR    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_CLR_M  (H264_DMA_INFIFO_OVF_L2_CH1_INT_CLR_V << H264_DMA_INFIFO_OVF_L2_CH1_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH1_INT_CLR_S  6
/** H264_DMA_INFIFO_UDF_L2_CH1_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_CLR    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_CLR_M  (H264_DMA_INFIFO_UDF_L2_CH1_INT_CLR_V << H264_DMA_INFIFO_UDF_L2_CH1_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH1_INT_CLR_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_CLR    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_M  (H264_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_V << H264_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_S)
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_CLR    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_CLR_M  (H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_CLR_V << H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_CLR_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH1_INT_CLR_S  9

/** H264_DMA_INFIFO_STATUS_CH1_REG register
 *  RX CH1 INFIFO status register
 */
#define H264_DMA_INFIFO_STATUS_CH1_REG (DR_REG_H264_DMA_BASE + 0x614)
/** H264_DMA_INFIFO_FULL_L2_CH1 : RO; bitpos: [0]; default: 0;
 *  Rx FIFO full signal for Rx channel.
 */
#define H264_DMA_INFIFO_FULL_L2_CH1    (BIT(0))
#define H264_DMA_INFIFO_FULL_L2_CH1_M  (H264_DMA_INFIFO_FULL_L2_CH1_V << H264_DMA_INFIFO_FULL_L2_CH1_S)
#define H264_DMA_INFIFO_FULL_L2_CH1_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L2_CH1_S  0
/** H264_DMA_INFIFO_EMPTY_L2_CH1 : RO; bitpos: [1]; default: 1;
 *  Rx FIFO empty signal for Rx channel.
 */
#define H264_DMA_INFIFO_EMPTY_L2_CH1    (BIT(1))
#define H264_DMA_INFIFO_EMPTY_L2_CH1_M  (H264_DMA_INFIFO_EMPTY_L2_CH1_V << H264_DMA_INFIFO_EMPTY_L2_CH1_S)
#define H264_DMA_INFIFO_EMPTY_L2_CH1_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L2_CH1_S  1
/** H264_DMA_INFIFO_CNT_L2_CH1 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Rx FIFO for Rx channel.
 */
#define H264_DMA_INFIFO_CNT_L2_CH1    0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH1_M  (H264_DMA_INFIFO_CNT_L2_CH1_V << H264_DMA_INFIFO_CNT_L2_CH1_S)
#define H264_DMA_INFIFO_CNT_L2_CH1_V  0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH1_S  2
/** H264_DMA_INFIFO_FULL_L1_CH1 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L1_CH1    (BIT(6))
#define H264_DMA_INFIFO_FULL_L1_CH1_M  (H264_DMA_INFIFO_FULL_L1_CH1_V << H264_DMA_INFIFO_FULL_L1_CH1_S)
#define H264_DMA_INFIFO_FULL_L1_CH1_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L1_CH1_S  6
/** H264_DMA_INFIFO_EMPTY_L1_CH1 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L1_CH1    (BIT(7))
#define H264_DMA_INFIFO_EMPTY_L1_CH1_M  (H264_DMA_INFIFO_EMPTY_L1_CH1_V << H264_DMA_INFIFO_EMPTY_L1_CH1_S)
#define H264_DMA_INFIFO_EMPTY_L1_CH1_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L1_CH1_S  7
/** H264_DMA_INFIFO_CNT_L1_CH1 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L1_CH1    0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH1_M  (H264_DMA_INFIFO_CNT_L1_CH1_V << H264_DMA_INFIFO_CNT_L1_CH1_S)
#define H264_DMA_INFIFO_CNT_L1_CH1_V  0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH1_S  8
/** H264_DMA_INFIFO_FULL_L3_CH1 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L3_CH1    (BIT(16))
#define H264_DMA_INFIFO_FULL_L3_CH1_M  (H264_DMA_INFIFO_FULL_L3_CH1_V << H264_DMA_INFIFO_FULL_L3_CH1_S)
#define H264_DMA_INFIFO_FULL_L3_CH1_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L3_CH1_S  16
/** H264_DMA_INFIFO_EMPTY_L3_CH1 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L3_CH1    (BIT(17))
#define H264_DMA_INFIFO_EMPTY_L3_CH1_M  (H264_DMA_INFIFO_EMPTY_L3_CH1_V << H264_DMA_INFIFO_EMPTY_L3_CH1_S)
#define H264_DMA_INFIFO_EMPTY_L3_CH1_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L3_CH1_S  17
/** H264_DMA_INFIFO_CNT_L3_CH1 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L3_CH1    0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH1_M  (H264_DMA_INFIFO_CNT_L3_CH1_V << H264_DMA_INFIFO_CNT_L3_CH1_S)
#define H264_DMA_INFIFO_CNT_L3_CH1_V  0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH1_S  18

/** H264_DMA_IN_POP_CH1_REG register
 *  RX CH1 INFIFO pop register
 */
#define H264_DMA_IN_POP_CH1_REG (DR_REG_H264_DMA_BASE + 0x618)
/** H264_DMA_INFIFO_RDATA_CH1 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_RDATA_CH1    0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH1_M  (H264_DMA_INFIFO_RDATA_CH1_V << H264_DMA_INFIFO_RDATA_CH1_S)
#define H264_DMA_INFIFO_RDATA_CH1_V  0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH1_S  0
/** H264_DMA_INFIFO_POP_CH1 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_POP_CH1    (BIT(11))
#define H264_DMA_INFIFO_POP_CH1_M  (H264_DMA_INFIFO_POP_CH1_V << H264_DMA_INFIFO_POP_CH1_S)
#define H264_DMA_INFIFO_POP_CH1_V  0x00000001U
#define H264_DMA_INFIFO_POP_CH1_S  11

/** H264_DMA_IN_LINK_CONF_CH1_REG register
 *  RX CH1 in_link dscr ctrl register
 */
#define H264_DMA_IN_LINK_CONF_CH1_REG (DR_REG_H264_DMA_BASE + 0x61c)
/** H264_DMA_INLINK_AUTO_RET_CH1 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_INLINK_AUTO_RET_CH1    (BIT(20))
#define H264_DMA_INLINK_AUTO_RET_CH1_M  (H264_DMA_INLINK_AUTO_RET_CH1_V << H264_DMA_INLINK_AUTO_RET_CH1_S)
#define H264_DMA_INLINK_AUTO_RET_CH1_V  0x00000001U
#define H264_DMA_INLINK_AUTO_RET_CH1_S  20
/** H264_DMA_INLINK_STOP_CH1 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_STOP_CH1    (BIT(21))
#define H264_DMA_INLINK_STOP_CH1_M  (H264_DMA_INLINK_STOP_CH1_V << H264_DMA_INLINK_STOP_CH1_S)
#define H264_DMA_INLINK_STOP_CH1_V  0x00000001U
#define H264_DMA_INLINK_STOP_CH1_S  21
/** H264_DMA_INLINK_START_CH1 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_START_CH1    (BIT(22))
#define H264_DMA_INLINK_START_CH1_M  (H264_DMA_INLINK_START_CH1_V << H264_DMA_INLINK_START_CH1_S)
#define H264_DMA_INLINK_START_CH1_V  0x00000001U
#define H264_DMA_INLINK_START_CH1_S  22
/** H264_DMA_INLINK_RESTART_CH1 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define H264_DMA_INLINK_RESTART_CH1    (BIT(23))
#define H264_DMA_INLINK_RESTART_CH1_M  (H264_DMA_INLINK_RESTART_CH1_V << H264_DMA_INLINK_RESTART_CH1_S)
#define H264_DMA_INLINK_RESTART_CH1_V  0x00000001U
#define H264_DMA_INLINK_RESTART_CH1_S  23
/** H264_DMA_INLINK_PARK_CH1 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define H264_DMA_INLINK_PARK_CH1    (BIT(24))
#define H264_DMA_INLINK_PARK_CH1_M  (H264_DMA_INLINK_PARK_CH1_V << H264_DMA_INLINK_PARK_CH1_S)
#define H264_DMA_INLINK_PARK_CH1_V  0x00000001U
#define H264_DMA_INLINK_PARK_CH1_S  24

/** H264_DMA_IN_LINK_ADDR_CH1_REG register
 *  RX CH1 in_link dscr addr register
 */
#define H264_DMA_IN_LINK_ADDR_CH1_REG (DR_REG_H264_DMA_BASE + 0x620)
/** H264_DMA_INLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first inlink descriptor's address.
 */
#define H264_DMA_INLINK_ADDR_CH1    0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH1_M  (H264_DMA_INLINK_ADDR_CH1_V << H264_DMA_INLINK_ADDR_CH1_S)
#define H264_DMA_INLINK_ADDR_CH1_V  0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH1_S  0

/** H264_DMA_IN_STATE_CH1_REG register
 *  RX CH1 state register
 */
#define H264_DMA_IN_STATE_CH1_REG (DR_REG_H264_DMA_BASE + 0x624)
/** H264_DMA_INLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define H264_DMA_INLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH1_M  (H264_DMA_INLINK_DSCR_ADDR_CH1_V << H264_DMA_INLINK_DSCR_ADDR_CH1_S)
#define H264_DMA_INLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH1_S  0
/** H264_DMA_IN_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_IN_DSCR_STATE_CH1    0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH1_M  (H264_DMA_IN_DSCR_STATE_CH1_V << H264_DMA_IN_DSCR_STATE_CH1_S)
#define H264_DMA_IN_DSCR_STATE_CH1_V  0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH1_S  18
/** H264_DMA_IN_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_IN_STATE_CH1    0x00000007U
#define H264_DMA_IN_STATE_CH1_M  (H264_DMA_IN_STATE_CH1_V << H264_DMA_IN_STATE_CH1_S)
#define H264_DMA_IN_STATE_CH1_V  0x00000007U
#define H264_DMA_IN_STATE_CH1_S  20
/** H264_DMA_IN_RESET_AVAIL_CH1 : RO; bitpos: [23]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_IN_RESET_AVAIL_CH1    (BIT(23))
#define H264_DMA_IN_RESET_AVAIL_CH1_M  (H264_DMA_IN_RESET_AVAIL_CH1_V << H264_DMA_IN_RESET_AVAIL_CH1_S)
#define H264_DMA_IN_RESET_AVAIL_CH1_V  0x00000001U
#define H264_DMA_IN_RESET_AVAIL_CH1_S  23

/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH1_REG register
 *  RX CH1 eof des addr register
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH1_REG (DR_REG_H264_DMA_BASE + 0x628)
/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH1_M  (H264_DMA_IN_SUC_EOF_DES_ADDR_CH1_V << H264_DMA_IN_SUC_EOF_DES_ADDR_CH1_S)
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH1_S  0

/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH1_REG register
 *  RX CH1 err eof des addr register
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH1_REG (DR_REG_H264_DMA_BASE + 0x62c)
/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH1_M  (H264_DMA_IN_ERR_EOF_DES_ADDR_CH1_V << H264_DMA_IN_ERR_EOF_DES_ADDR_CH1_S)
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH1_S  0

/** H264_DMA_IN_DSCR_CH1_REG register
 *  RX CH1 next dscr addr register
 */
#define H264_DMA_IN_DSCR_CH1_REG (DR_REG_H264_DMA_BASE + 0x630)
/** H264_DMA_INLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next inlink descriptor address x.
 */
#define H264_DMA_INLINK_DSCR_CH1    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH1_M  (H264_DMA_INLINK_DSCR_CH1_V << H264_DMA_INLINK_DSCR_CH1_S)
#define H264_DMA_INLINK_DSCR_CH1_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH1_S  0

/** H264_DMA_IN_DSCR_BF0_CH1_REG register
 *  RX CH1 last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF0_CH1_REG (DR_REG_H264_DMA_BASE + 0x634)
/** H264_DMA_INLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor's next address x-1.
 */
#define H264_DMA_INLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH1_M  (H264_DMA_INLINK_DSCR_BF0_CH1_V << H264_DMA_INLINK_DSCR_BF0_CH1_S)
#define H264_DMA_INLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH1_S  0

/** H264_DMA_IN_DSCR_BF1_CH1_REG register
 *  RX CH1 second-to-last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF1_CH1_REG (DR_REG_H264_DMA_BASE + 0x638)
/** H264_DMA_INLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor's next address x-2.
 */
#define H264_DMA_INLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH1_M  (H264_DMA_INLINK_DSCR_BF1_CH1_V << H264_DMA_INLINK_DSCR_BF1_CH1_S)
#define H264_DMA_INLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH1_S  0

/** H264_DMA_IN_ARB_CH1_REG register
 *  RX CH1 arb register
 */
#define H264_DMA_IN_ARB_CH1_REG (DR_REG_H264_DMA_BASE + 0x640)
/** H264_DMA_IN_ARB_TOKEN_NUM_CH1 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_IN_ARB_TOKEN_NUM_CH1    0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH1_M  (H264_DMA_IN_ARB_TOKEN_NUM_CH1_V << H264_DMA_IN_ARB_TOKEN_NUM_CH1_S)
#define H264_DMA_IN_ARB_TOKEN_NUM_CH1_V  0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH1_S  0
/** H264_DMA_EXTER_IN_ARB_PRIORITY_CH1 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH1    0x00000003U
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH1_M  (H264_DMA_EXTER_IN_ARB_PRIORITY_CH1_V << H264_DMA_EXTER_IN_ARB_PRIORITY_CH1_S)
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH1_V  0x00000003U
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH1_S  4
/** H264_DMA_INTER_IN_ARB_PRIORITY_CH1 : R/W; bitpos: [8:6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH1    0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH1_M  (H264_DMA_INTER_IN_ARB_PRIORITY_CH1_V << H264_DMA_INTER_IN_ARB_PRIORITY_CH1_S)
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH1_V  0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH1_S  6

/** H264_DMA_IN_ETM_CONF_CH1_REG register
 *  RX CH1 ETM config register
 */
#define H264_DMA_IN_ETM_CONF_CH1_REG (DR_REG_H264_DMA_BASE + 0x648)
/** H264_DMA_IN_ETM_EN_CH1 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_IN_ETM_EN_CH1    (BIT(0))
#define H264_DMA_IN_ETM_EN_CH1_M  (H264_DMA_IN_ETM_EN_CH1_V << H264_DMA_IN_ETM_EN_CH1_S)
#define H264_DMA_IN_ETM_EN_CH1_V  0x00000001U
#define H264_DMA_IN_ETM_EN_CH1_S  0
/** H264_DMA_IN_ETM_LOOP_EN_CH1 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_IN_ETM_LOOP_EN_CH1    (BIT(1))
#define H264_DMA_IN_ETM_LOOP_EN_CH1_M  (H264_DMA_IN_ETM_LOOP_EN_CH1_V << H264_DMA_IN_ETM_LOOP_EN_CH1_S)
#define H264_DMA_IN_ETM_LOOP_EN_CH1_V  0x00000001U
#define H264_DMA_IN_ETM_LOOP_EN_CH1_S  1
/** H264_DMA_IN_DSCR_TASK_MAK_CH1 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_IN_DSCR_TASK_MAK_CH1    0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH1_M  (H264_DMA_IN_DSCR_TASK_MAK_CH1_V << H264_DMA_IN_DSCR_TASK_MAK_CH1_S)
#define H264_DMA_IN_DSCR_TASK_MAK_CH1_V  0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH1_S  2

/** H264_DMA_IN_FIFO_CNT_CH1_REG register
 *  rx CH1 fifo cnt register
 */
#define H264_DMA_IN_FIFO_CNT_CH1_REG (DR_REG_H264_DMA_BASE + 0x680)
/** H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH1 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH1    0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH1_M  (H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH1_V << H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH1_S)
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH1_V  0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH1_S  0

/** H264_DMA_IN_POP_DATA_CNT_CH1_REG register
 *  rx CH1 pop data cnt register
 */
#define H264_DMA_IN_POP_DATA_CNT_CH1_REG (DR_REG_H264_DMA_BASE + 0x684)
/** H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH1 : RO; bitpos: [7:0]; default: 7;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH1    0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH1_M  (H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH1_V << H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH1_S)
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH1_V  0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH1_S  0

/** H264_DMA_IN_XADDR_CH1_REG register
 *  rx CH1 xaddr register
 */
#define H264_DMA_IN_XADDR_CH1_REG (DR_REG_H264_DMA_BASE + 0x688)
/** H264_DMA_IN_CMDFIFO_XADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_XADDR_CH1    0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH1_M  (H264_DMA_IN_CMDFIFO_XADDR_CH1_V << H264_DMA_IN_CMDFIFO_XADDR_CH1_S)
#define H264_DMA_IN_CMDFIFO_XADDR_CH1_V  0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH1_S  0

/** H264_DMA_IN_BUF_HB_RCV_CH1_REG register
 *  rx CH1 buf len hb rcv register
 */
#define H264_DMA_IN_BUF_HB_RCV_CH1_REG (DR_REG_H264_DMA_BASE + 0x68c)
/** H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH1 : RO; bitpos: [28:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH1    0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH1_M  (H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH1_V << H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH1_S)
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH1_V  0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH1_S  0

/** H264_DMA_IN_CONF0_CH2_REG register
 *  RX CH2 config0 register
 */
#define H264_DMA_IN_CONF0_CH2_REG (DR_REG_H264_DMA_BASE + 0x700)
/** H264_DMA_INDSCR_BURST_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
 *  when accessing SRAM.
 */
#define H264_DMA_INDSCR_BURST_EN_CH2    (BIT(2))
#define H264_DMA_INDSCR_BURST_EN_CH2_M  (H264_DMA_INDSCR_BURST_EN_CH2_V << H264_DMA_INDSCR_BURST_EN_CH2_S)
#define H264_DMA_INDSCR_BURST_EN_CH2_V  0x00000001U
#define H264_DMA_INDSCR_BURST_EN_CH2_S  2
/** H264_DMA_IN_ECC_AES_EN_CH2 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_IN_ECC_AES_EN_CH2    (BIT(3))
#define H264_DMA_IN_ECC_AES_EN_CH2_M  (H264_DMA_IN_ECC_AES_EN_CH2_V << H264_DMA_IN_ECC_AES_EN_CH2_S)
#define H264_DMA_IN_ECC_AES_EN_CH2_V  0x00000001U
#define H264_DMA_IN_ECC_AES_EN_CH2_S  3
/** H264_DMA_IN_CHECK_OWNER_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_IN_CHECK_OWNER_CH2    (BIT(4))
#define H264_DMA_IN_CHECK_OWNER_CH2_M  (H264_DMA_IN_CHECK_OWNER_CH2_V << H264_DMA_IN_CHECK_OWNER_CH2_S)
#define H264_DMA_IN_CHECK_OWNER_CH2_V  0x00000001U
#define H264_DMA_IN_CHECK_OWNER_CH2_S  4
/** H264_DMA_IN_MEM_BURST_LENGTH_CH2 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 2. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_IN_MEM_BURST_LENGTH_CH2    0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH2_M  (H264_DMA_IN_MEM_BURST_LENGTH_CH2_V << H264_DMA_IN_MEM_BURST_LENGTH_CH2_S)
#define H264_DMA_IN_MEM_BURST_LENGTH_CH2_V  0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH2_S  6
/** H264_DMA_IN_PAGE_BOUND_EN_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define H264_DMA_IN_PAGE_BOUND_EN_CH2    (BIT(12))
#define H264_DMA_IN_PAGE_BOUND_EN_CH2_M  (H264_DMA_IN_PAGE_BOUND_EN_CH2_V << H264_DMA_IN_PAGE_BOUND_EN_CH2_S)
#define H264_DMA_IN_PAGE_BOUND_EN_CH2_V  0x00000001U
#define H264_DMA_IN_PAGE_BOUND_EN_CH2_S  12
/** H264_DMA_IN_RST_CH2 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define H264_DMA_IN_RST_CH2    (BIT(24))
#define H264_DMA_IN_RST_CH2_M  (H264_DMA_IN_RST_CH2_V << H264_DMA_IN_RST_CH2_S)
#define H264_DMA_IN_RST_CH2_V  0x00000001U
#define H264_DMA_IN_RST_CH2_S  24
/** H264_DMA_IN_CMD_DISABLE_CH2 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_IN_CMD_DISABLE_CH2    (BIT(25))
#define H264_DMA_IN_CMD_DISABLE_CH2_M  (H264_DMA_IN_CMD_DISABLE_CH2_V << H264_DMA_IN_CMD_DISABLE_CH2_S)
#define H264_DMA_IN_CMD_DISABLE_CH2_V  0x00000001U
#define H264_DMA_IN_CMD_DISABLE_CH2_S  25
/** H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH2 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH2    (BIT(26))
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH2_M  (H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH2_V << H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH2_S)
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH2_V  0x00000001U
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH2_S  26

/** H264_DMA_IN_INT_RAW_CH2_REG register
 *  RX CH2  interrupt raw register
 */
#define H264_DMA_IN_INT_RAW_CH2_REG (DR_REG_H264_DMA_BASE + 0x704)
/** H264_DMA_IN_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 1.
 */
#define H264_DMA_IN_DONE_CH2_INT_RAW    (BIT(0))
#define H264_DMA_IN_DONE_CH2_INT_RAW_M  (H264_DMA_IN_DONE_CH2_INT_RAW_V << H264_DMA_IN_DONE_CH2_INT_RAW_S)
#define H264_DMA_IN_DONE_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DONE_CH2_INT_RAW_S  0
/** H264_DMA_IN_SUC_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 1.
 */
#define H264_DMA_IN_SUC_EOF_CH2_INT_RAW    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH2_INT_RAW_M  (H264_DMA_IN_SUC_EOF_CH2_INT_RAW_V << H264_DMA_IN_SUC_EOF_CH2_INT_RAW_S)
#define H264_DMA_IN_SUC_EOF_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH2_INT_RAW_S  1
/** H264_DMA_IN_ERR_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and data error is detected
 */
#define H264_DMA_IN_ERR_EOF_CH2_INT_RAW    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH2_INT_RAW_M  (H264_DMA_IN_ERR_EOF_CH2_INT_RAW_V << H264_DMA_IN_ERR_EOF_CH2_INT_RAW_S)
#define H264_DMA_IN_ERR_EOF_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH2_INT_RAW_S  2
/** H264_DMA_IN_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor for Rx
 *  channel 1.
 */
#define H264_DMA_IN_DSCR_ERR_CH2_INT_RAW    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH2_INT_RAW_M  (H264_DMA_IN_DSCR_ERR_CH2_INT_RAW_V << H264_DMA_IN_DSCR_ERR_CH2_INT_RAW_S)
#define H264_DMA_IN_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH2_INT_RAW_S  3
/** H264_DMA_INFIFO_OVF_L1_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_RAW    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_RAW_M  (H264_DMA_INFIFO_OVF_L1_CH2_INT_RAW_V << H264_DMA_INFIFO_OVF_L1_CH2_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_RAW_S  4
/** H264_DMA_INFIFO_UDF_L1_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_RAW    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_RAW_M  (H264_DMA_INFIFO_UDF_L1_CH2_INT_RAW_V << H264_DMA_INFIFO_UDF_L1_CH2_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_RAW_S  5
/** H264_DMA_INFIFO_OVF_L2_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_RAW    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_RAW_M  (H264_DMA_INFIFO_OVF_L2_CH2_INT_RAW_V << H264_DMA_INFIFO_OVF_L2_CH2_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_RAW_S  6
/** H264_DMA_INFIFO_UDF_L2_CH2_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_RAW    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_RAW_M  (H264_DMA_INFIFO_UDF_L2_CH2_INT_RAW_V << H264_DMA_INFIFO_UDF_L2_CH2_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_RAW_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH2_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
 *  also remain data.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_RAW    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_M  (H264_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_V << H264_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_S)
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_RAW    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_RAW_M  (H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_RAW_V << H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_RAW_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_RAW_S  9

/** H264_DMA_IN_INT_ENA_CH2_REG register
 *  RX CH2  interrupt ena register
 */
#define H264_DMA_IN_INT_ENA_CH2_REG (DR_REG_H264_DMA_BASE + 0x708)
/** H264_DMA_IN_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH2_INT_ENA    (BIT(0))
#define H264_DMA_IN_DONE_CH2_INT_ENA_M  (H264_DMA_IN_DONE_CH2_INT_ENA_V << H264_DMA_IN_DONE_CH2_INT_ENA_S)
#define H264_DMA_IN_DONE_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DONE_CH2_INT_ENA_S  0
/** H264_DMA_IN_SUC_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH2_INT_ENA    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH2_INT_ENA_M  (H264_DMA_IN_SUC_EOF_CH2_INT_ENA_V << H264_DMA_IN_SUC_EOF_CH2_INT_ENA_S)
#define H264_DMA_IN_SUC_EOF_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH2_INT_ENA_S  1
/** H264_DMA_IN_ERR_EOF_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH2_INT_ENA    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH2_INT_ENA_M  (H264_DMA_IN_ERR_EOF_CH2_INT_ENA_V << H264_DMA_IN_ERR_EOF_CH2_INT_ENA_S)
#define H264_DMA_IN_ERR_EOF_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH2_INT_ENA_S  2
/** H264_DMA_IN_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ENA    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ENA_M  (H264_DMA_IN_DSCR_ERR_CH2_INT_ENA_V << H264_DMA_IN_DSCR_ERR_CH2_INT_ENA_S)
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ENA_S  3
/** H264_DMA_INFIFO_OVF_L1_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ENA    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ENA_M  (H264_DMA_INFIFO_OVF_L1_CH2_INT_ENA_V << H264_DMA_INFIFO_OVF_L1_CH2_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ENA_S  4
/** H264_DMA_INFIFO_UDF_L1_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ENA    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ENA_M  (H264_DMA_INFIFO_UDF_L1_CH2_INT_ENA_V << H264_DMA_INFIFO_UDF_L1_CH2_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ENA_S  5
/** H264_DMA_INFIFO_OVF_L2_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ENA    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ENA_M  (H264_DMA_INFIFO_OVF_L2_CH2_INT_ENA_V << H264_DMA_INFIFO_OVF_L2_CH2_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ENA_S  6
/** H264_DMA_INFIFO_UDF_L2_CH2_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ENA    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ENA_M  (H264_DMA_INFIFO_UDF_L2_CH2_INT_ENA_V << H264_DMA_INFIFO_UDF_L2_CH2_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ENA_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH2_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ENA    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_M  (H264_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_V << H264_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_S)
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ENA    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ENA_M  (H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ENA_V << H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ENA_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ENA_S  9

/** H264_DMA_IN_INT_ST_CH2_REG register
 *  RX CH2  interrupt st register
 */
#define H264_DMA_IN_INT_ST_CH2_REG (DR_REG_H264_DMA_BASE + 0x70c)
/** H264_DMA_IN_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH2_INT_ST    (BIT(0))
#define H264_DMA_IN_DONE_CH2_INT_ST_M  (H264_DMA_IN_DONE_CH2_INT_ST_V << H264_DMA_IN_DONE_CH2_INT_ST_S)
#define H264_DMA_IN_DONE_CH2_INT_ST_V  0x00000001U
#define H264_DMA_IN_DONE_CH2_INT_ST_S  0
/** H264_DMA_IN_SUC_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH2_INT_ST    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH2_INT_ST_M  (H264_DMA_IN_SUC_EOF_CH2_INT_ST_V << H264_DMA_IN_SUC_EOF_CH2_INT_ST_S)
#define H264_DMA_IN_SUC_EOF_CH2_INT_ST_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH2_INT_ST_S  1
/** H264_DMA_IN_ERR_EOF_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH2_INT_ST    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH2_INT_ST_M  (H264_DMA_IN_ERR_EOF_CH2_INT_ST_V << H264_DMA_IN_ERR_EOF_CH2_INT_ST_S)
#define H264_DMA_IN_ERR_EOF_CH2_INT_ST_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH2_INT_ST_S  2
/** H264_DMA_IN_DSCR_ERR_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ST    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ST_M  (H264_DMA_IN_DSCR_ERR_CH2_INT_ST_V << H264_DMA_IN_DSCR_ERR_CH2_INT_ST_S)
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH2_INT_ST_S  3
/** H264_DMA_INFIFO_OVF_L1_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ST    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ST_M  (H264_DMA_INFIFO_OVF_L1_CH2_INT_ST_V << H264_DMA_INFIFO_OVF_L1_CH2_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_ST_S  4
/** H264_DMA_INFIFO_UDF_L1_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ST    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ST_M  (H264_DMA_INFIFO_UDF_L1_CH2_INT_ST_V << H264_DMA_INFIFO_UDF_L1_CH2_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_ST_S  5
/** H264_DMA_INFIFO_OVF_L2_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ST    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ST_M  (H264_DMA_INFIFO_OVF_L2_CH2_INT_ST_V << H264_DMA_INFIFO_OVF_L2_CH2_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_ST_S  6
/** H264_DMA_INFIFO_UDF_L2_CH2_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ST    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ST_M  (H264_DMA_INFIFO_UDF_L2_CH2_INT_ST_V << H264_DMA_INFIFO_UDF_L2_CH2_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_ST_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH2_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ST    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ST_M  (H264_DMA_IN_DSCR_EMPTY_CH2_INT_ST_V << H264_DMA_IN_DSCR_EMPTY_CH2_INT_ST_S)
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_ST_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ST    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ST_M  (H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ST_V << H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ST_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_ST_S  9

/** H264_DMA_IN_INT_CLR_CH2_REG register
 *  RX CH2  interrupt clr register
 */
#define H264_DMA_IN_INT_CLR_CH2_REG (DR_REG_H264_DMA_BASE + 0x710)
/** H264_DMA_IN_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH2_INT_CLR    (BIT(0))
#define H264_DMA_IN_DONE_CH2_INT_CLR_M  (H264_DMA_IN_DONE_CH2_INT_CLR_V << H264_DMA_IN_DONE_CH2_INT_CLR_S)
#define H264_DMA_IN_DONE_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DONE_CH2_INT_CLR_S  0
/** H264_DMA_IN_SUC_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH2_INT_CLR    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH2_INT_CLR_M  (H264_DMA_IN_SUC_EOF_CH2_INT_CLR_V << H264_DMA_IN_SUC_EOF_CH2_INT_CLR_S)
#define H264_DMA_IN_SUC_EOF_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH2_INT_CLR_S  1
/** H264_DMA_IN_ERR_EOF_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH2_INT_CLR    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH2_INT_CLR_M  (H264_DMA_IN_ERR_EOF_CH2_INT_CLR_V << H264_DMA_IN_ERR_EOF_CH2_INT_CLR_S)
#define H264_DMA_IN_ERR_EOF_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH2_INT_CLR_S  2
/** H264_DMA_IN_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH2_INT_CLR    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH2_INT_CLR_M  (H264_DMA_IN_DSCR_ERR_CH2_INT_CLR_V << H264_DMA_IN_DSCR_ERR_CH2_INT_CLR_S)
#define H264_DMA_IN_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH2_INT_CLR_S  3
/** H264_DMA_INFIFO_OVF_L1_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_CLR    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_CLR_M  (H264_DMA_INFIFO_OVF_L1_CH2_INT_CLR_V << H264_DMA_INFIFO_OVF_L1_CH2_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH2_INT_CLR_S  4
/** H264_DMA_INFIFO_UDF_L1_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_CLR    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_CLR_M  (H264_DMA_INFIFO_UDF_L1_CH2_INT_CLR_V << H264_DMA_INFIFO_UDF_L1_CH2_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH2_INT_CLR_S  5
/** H264_DMA_INFIFO_OVF_L2_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_CLR    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_CLR_M  (H264_DMA_INFIFO_OVF_L2_CH2_INT_CLR_V << H264_DMA_INFIFO_OVF_L2_CH2_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH2_INT_CLR_S  6
/** H264_DMA_INFIFO_UDF_L2_CH2_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_CLR    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_CLR_M  (H264_DMA_INFIFO_UDF_L2_CH2_INT_CLR_V << H264_DMA_INFIFO_UDF_L2_CH2_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH2_INT_CLR_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH2_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_CLR    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_M  (H264_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_V << H264_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_S)
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_CLR    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_CLR_M  (H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_CLR_V << H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_CLR_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH2_INT_CLR_S  9

/** H264_DMA_INFIFO_STATUS_CH2_REG register
 *  RX CH2 INFIFO status register
 */
#define H264_DMA_INFIFO_STATUS_CH2_REG (DR_REG_H264_DMA_BASE + 0x714)
/** H264_DMA_INFIFO_FULL_L2_CH2 : RO; bitpos: [0]; default: 0;
 *  Rx FIFO full signal for Rx channel.
 */
#define H264_DMA_INFIFO_FULL_L2_CH2    (BIT(0))
#define H264_DMA_INFIFO_FULL_L2_CH2_M  (H264_DMA_INFIFO_FULL_L2_CH2_V << H264_DMA_INFIFO_FULL_L2_CH2_S)
#define H264_DMA_INFIFO_FULL_L2_CH2_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L2_CH2_S  0
/** H264_DMA_INFIFO_EMPTY_L2_CH2 : RO; bitpos: [1]; default: 1;
 *  Rx FIFO empty signal for Rx channel.
 */
#define H264_DMA_INFIFO_EMPTY_L2_CH2    (BIT(1))
#define H264_DMA_INFIFO_EMPTY_L2_CH2_M  (H264_DMA_INFIFO_EMPTY_L2_CH2_V << H264_DMA_INFIFO_EMPTY_L2_CH2_S)
#define H264_DMA_INFIFO_EMPTY_L2_CH2_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L2_CH2_S  1
/** H264_DMA_INFIFO_CNT_L2_CH2 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Rx FIFO for Rx channel.
 */
#define H264_DMA_INFIFO_CNT_L2_CH2    0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH2_M  (H264_DMA_INFIFO_CNT_L2_CH2_V << H264_DMA_INFIFO_CNT_L2_CH2_S)
#define H264_DMA_INFIFO_CNT_L2_CH2_V  0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH2_S  2
/** H264_DMA_INFIFO_FULL_L1_CH2 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L1_CH2    (BIT(6))
#define H264_DMA_INFIFO_FULL_L1_CH2_M  (H264_DMA_INFIFO_FULL_L1_CH2_V << H264_DMA_INFIFO_FULL_L1_CH2_S)
#define H264_DMA_INFIFO_FULL_L1_CH2_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L1_CH2_S  6
/** H264_DMA_INFIFO_EMPTY_L1_CH2 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L1_CH2    (BIT(7))
#define H264_DMA_INFIFO_EMPTY_L1_CH2_M  (H264_DMA_INFIFO_EMPTY_L1_CH2_V << H264_DMA_INFIFO_EMPTY_L1_CH2_S)
#define H264_DMA_INFIFO_EMPTY_L1_CH2_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L1_CH2_S  7
/** H264_DMA_INFIFO_CNT_L1_CH2 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L1_CH2    0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH2_M  (H264_DMA_INFIFO_CNT_L1_CH2_V << H264_DMA_INFIFO_CNT_L1_CH2_S)
#define H264_DMA_INFIFO_CNT_L1_CH2_V  0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH2_S  8
/** H264_DMA_INFIFO_FULL_L3_CH2 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L3_CH2    (BIT(16))
#define H264_DMA_INFIFO_FULL_L3_CH2_M  (H264_DMA_INFIFO_FULL_L3_CH2_V << H264_DMA_INFIFO_FULL_L3_CH2_S)
#define H264_DMA_INFIFO_FULL_L3_CH2_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L3_CH2_S  16
/** H264_DMA_INFIFO_EMPTY_L3_CH2 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L3_CH2    (BIT(17))
#define H264_DMA_INFIFO_EMPTY_L3_CH2_M  (H264_DMA_INFIFO_EMPTY_L3_CH2_V << H264_DMA_INFIFO_EMPTY_L3_CH2_S)
#define H264_DMA_INFIFO_EMPTY_L3_CH2_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L3_CH2_S  17
/** H264_DMA_INFIFO_CNT_L3_CH2 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L3_CH2    0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH2_M  (H264_DMA_INFIFO_CNT_L3_CH2_V << H264_DMA_INFIFO_CNT_L3_CH2_S)
#define H264_DMA_INFIFO_CNT_L3_CH2_V  0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH2_S  18

/** H264_DMA_IN_POP_CH2_REG register
 *  RX CH2 INFIFO pop register
 */
#define H264_DMA_IN_POP_CH2_REG (DR_REG_H264_DMA_BASE + 0x718)
/** H264_DMA_INFIFO_RDATA_CH2 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_RDATA_CH2    0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH2_M  (H264_DMA_INFIFO_RDATA_CH2_V << H264_DMA_INFIFO_RDATA_CH2_S)
#define H264_DMA_INFIFO_RDATA_CH2_V  0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH2_S  0
/** H264_DMA_INFIFO_POP_CH2 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_POP_CH2    (BIT(11))
#define H264_DMA_INFIFO_POP_CH2_M  (H264_DMA_INFIFO_POP_CH2_V << H264_DMA_INFIFO_POP_CH2_S)
#define H264_DMA_INFIFO_POP_CH2_V  0x00000001U
#define H264_DMA_INFIFO_POP_CH2_S  11

/** H264_DMA_IN_LINK_CONF_CH2_REG register
 *  RX CH2 in_link dscr ctrl register
 */
#define H264_DMA_IN_LINK_CONF_CH2_REG (DR_REG_H264_DMA_BASE + 0x71c)
/** H264_DMA_INLINK_AUTO_RET_CH2 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_INLINK_AUTO_RET_CH2    (BIT(20))
#define H264_DMA_INLINK_AUTO_RET_CH2_M  (H264_DMA_INLINK_AUTO_RET_CH2_V << H264_DMA_INLINK_AUTO_RET_CH2_S)
#define H264_DMA_INLINK_AUTO_RET_CH2_V  0x00000001U
#define H264_DMA_INLINK_AUTO_RET_CH2_S  20
/** H264_DMA_INLINK_STOP_CH2 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_STOP_CH2    (BIT(21))
#define H264_DMA_INLINK_STOP_CH2_M  (H264_DMA_INLINK_STOP_CH2_V << H264_DMA_INLINK_STOP_CH2_S)
#define H264_DMA_INLINK_STOP_CH2_V  0x00000001U
#define H264_DMA_INLINK_STOP_CH2_S  21
/** H264_DMA_INLINK_START_CH2 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_START_CH2    (BIT(22))
#define H264_DMA_INLINK_START_CH2_M  (H264_DMA_INLINK_START_CH2_V << H264_DMA_INLINK_START_CH2_S)
#define H264_DMA_INLINK_START_CH2_V  0x00000001U
#define H264_DMA_INLINK_START_CH2_S  22
/** H264_DMA_INLINK_RESTART_CH2 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define H264_DMA_INLINK_RESTART_CH2    (BIT(23))
#define H264_DMA_INLINK_RESTART_CH2_M  (H264_DMA_INLINK_RESTART_CH2_V << H264_DMA_INLINK_RESTART_CH2_S)
#define H264_DMA_INLINK_RESTART_CH2_V  0x00000001U
#define H264_DMA_INLINK_RESTART_CH2_S  23
/** H264_DMA_INLINK_PARK_CH2 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define H264_DMA_INLINK_PARK_CH2    (BIT(24))
#define H264_DMA_INLINK_PARK_CH2_M  (H264_DMA_INLINK_PARK_CH2_V << H264_DMA_INLINK_PARK_CH2_S)
#define H264_DMA_INLINK_PARK_CH2_V  0x00000001U
#define H264_DMA_INLINK_PARK_CH2_S  24

/** H264_DMA_IN_LINK_ADDR_CH2_REG register
 *  RX CH2 in_link dscr addr register
 */
#define H264_DMA_IN_LINK_ADDR_CH2_REG (DR_REG_H264_DMA_BASE + 0x720)
/** H264_DMA_INLINK_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first inlink descriptor's address.
 */
#define H264_DMA_INLINK_ADDR_CH2    0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH2_M  (H264_DMA_INLINK_ADDR_CH2_V << H264_DMA_INLINK_ADDR_CH2_S)
#define H264_DMA_INLINK_ADDR_CH2_V  0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH2_S  0

/** H264_DMA_IN_STATE_CH2_REG register
 *  RX CH2 state register
 */
#define H264_DMA_IN_STATE_CH2_REG (DR_REG_H264_DMA_BASE + 0x724)
/** H264_DMA_INLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define H264_DMA_INLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH2_M  (H264_DMA_INLINK_DSCR_ADDR_CH2_V << H264_DMA_INLINK_DSCR_ADDR_CH2_S)
#define H264_DMA_INLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH2_S  0
/** H264_DMA_IN_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_IN_DSCR_STATE_CH2    0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH2_M  (H264_DMA_IN_DSCR_STATE_CH2_V << H264_DMA_IN_DSCR_STATE_CH2_S)
#define H264_DMA_IN_DSCR_STATE_CH2_V  0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH2_S  18
/** H264_DMA_IN_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_IN_STATE_CH2    0x00000007U
#define H264_DMA_IN_STATE_CH2_M  (H264_DMA_IN_STATE_CH2_V << H264_DMA_IN_STATE_CH2_S)
#define H264_DMA_IN_STATE_CH2_V  0x00000007U
#define H264_DMA_IN_STATE_CH2_S  20
/** H264_DMA_IN_RESET_AVAIL_CH2 : RO; bitpos: [23]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_IN_RESET_AVAIL_CH2    (BIT(23))
#define H264_DMA_IN_RESET_AVAIL_CH2_M  (H264_DMA_IN_RESET_AVAIL_CH2_V << H264_DMA_IN_RESET_AVAIL_CH2_S)
#define H264_DMA_IN_RESET_AVAIL_CH2_V  0x00000001U
#define H264_DMA_IN_RESET_AVAIL_CH2_S  23

/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH2_REG register
 *  RX CH2 eof des addr register
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH2_REG (DR_REG_H264_DMA_BASE + 0x728)
/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH2_M  (H264_DMA_IN_SUC_EOF_DES_ADDR_CH2_V << H264_DMA_IN_SUC_EOF_DES_ADDR_CH2_S)
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH2_S  0

/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH2_REG register
 *  RX CH2 err eof des addr register
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH2_REG (DR_REG_H264_DMA_BASE + 0x72c)
/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH2_M  (H264_DMA_IN_ERR_EOF_DES_ADDR_CH2_V << H264_DMA_IN_ERR_EOF_DES_ADDR_CH2_S)
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH2_S  0

/** H264_DMA_IN_DSCR_CH2_REG register
 *  RX CH2 next dscr addr register
 */
#define H264_DMA_IN_DSCR_CH2_REG (DR_REG_H264_DMA_BASE + 0x730)
/** H264_DMA_INLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next inlink descriptor address x.
 */
#define H264_DMA_INLINK_DSCR_CH2    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH2_M  (H264_DMA_INLINK_DSCR_CH2_V << H264_DMA_INLINK_DSCR_CH2_S)
#define H264_DMA_INLINK_DSCR_CH2_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH2_S  0

/** H264_DMA_IN_DSCR_BF0_CH2_REG register
 *  RX CH2 last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF0_CH2_REG (DR_REG_H264_DMA_BASE + 0x734)
/** H264_DMA_INLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor's next address x-1.
 */
#define H264_DMA_INLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH2_M  (H264_DMA_INLINK_DSCR_BF0_CH2_V << H264_DMA_INLINK_DSCR_BF0_CH2_S)
#define H264_DMA_INLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH2_S  0

/** H264_DMA_IN_DSCR_BF1_CH2_REG register
 *  RX CH2 second-to-last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF1_CH2_REG (DR_REG_H264_DMA_BASE + 0x738)
/** H264_DMA_INLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor's next address x-2.
 */
#define H264_DMA_INLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH2_M  (H264_DMA_INLINK_DSCR_BF1_CH2_V << H264_DMA_INLINK_DSCR_BF1_CH2_S)
#define H264_DMA_INLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH2_S  0

/** H264_DMA_IN_ARB_CH2_REG register
 *  RX CH2 arb register
 */
#define H264_DMA_IN_ARB_CH2_REG (DR_REG_H264_DMA_BASE + 0x740)
/** H264_DMA_IN_ARB_TOKEN_NUM_CH2 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_IN_ARB_TOKEN_NUM_CH2    0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH2_M  (H264_DMA_IN_ARB_TOKEN_NUM_CH2_V << H264_DMA_IN_ARB_TOKEN_NUM_CH2_S)
#define H264_DMA_IN_ARB_TOKEN_NUM_CH2_V  0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH2_S  0
/** H264_DMA_INTER_IN_ARB_PRIORITY_CH2 : R/W; bitpos: [8:6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH2    0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH2_M  (H264_DMA_INTER_IN_ARB_PRIORITY_CH2_V << H264_DMA_INTER_IN_ARB_PRIORITY_CH2_S)
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH2_V  0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH2_S  6

/** H264_DMA_IN_ETM_CONF_CH2_REG register
 *  RX CH2 ETM config register
 */
#define H264_DMA_IN_ETM_CONF_CH2_REG (DR_REG_H264_DMA_BASE + 0x748)
/** H264_DMA_IN_ETM_EN_CH2 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_IN_ETM_EN_CH2    (BIT(0))
#define H264_DMA_IN_ETM_EN_CH2_M  (H264_DMA_IN_ETM_EN_CH2_V << H264_DMA_IN_ETM_EN_CH2_S)
#define H264_DMA_IN_ETM_EN_CH2_V  0x00000001U
#define H264_DMA_IN_ETM_EN_CH2_S  0
/** H264_DMA_IN_ETM_LOOP_EN_CH2 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_IN_ETM_LOOP_EN_CH2    (BIT(1))
#define H264_DMA_IN_ETM_LOOP_EN_CH2_M  (H264_DMA_IN_ETM_LOOP_EN_CH2_V << H264_DMA_IN_ETM_LOOP_EN_CH2_S)
#define H264_DMA_IN_ETM_LOOP_EN_CH2_V  0x00000001U
#define H264_DMA_IN_ETM_LOOP_EN_CH2_S  1
/** H264_DMA_IN_DSCR_TASK_MAK_CH2 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_IN_DSCR_TASK_MAK_CH2    0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH2_M  (H264_DMA_IN_DSCR_TASK_MAK_CH2_V << H264_DMA_IN_DSCR_TASK_MAK_CH2_S)
#define H264_DMA_IN_DSCR_TASK_MAK_CH2_V  0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH2_S  2

/** H264_DMA_IN_FIFO_CNT_CH2_REG register
 *  rx CH2 fifo cnt register
 */
#define H264_DMA_IN_FIFO_CNT_CH2_REG (DR_REG_H264_DMA_BASE + 0x780)
/** H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH2 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH2    0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH2_M  (H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH2_V << H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH2_S)
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH2_V  0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH2_S  0

/** H264_DMA_IN_POP_DATA_CNT_CH2_REG register
 *  rx CH2 pop data cnt register
 */
#define H264_DMA_IN_POP_DATA_CNT_CH2_REG (DR_REG_H264_DMA_BASE + 0x784)
/** H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH2 : RO; bitpos: [7:0]; default: 7;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH2    0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH2_M  (H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH2_V << H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH2_S)
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH2_V  0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH2_S  0

/** H264_DMA_IN_XADDR_CH2_REG register
 *  rx CH2 xaddr register
 */
#define H264_DMA_IN_XADDR_CH2_REG (DR_REG_H264_DMA_BASE + 0x788)
/** H264_DMA_IN_CMDFIFO_XADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_XADDR_CH2    0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH2_M  (H264_DMA_IN_CMDFIFO_XADDR_CH2_V << H264_DMA_IN_CMDFIFO_XADDR_CH2_S)
#define H264_DMA_IN_CMDFIFO_XADDR_CH2_V  0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH2_S  0

/** H264_DMA_IN_BUF_HB_RCV_CH2_REG register
 *  rx CH2 buf len hb rcv register
 */
#define H264_DMA_IN_BUF_HB_RCV_CH2_REG (DR_REG_H264_DMA_BASE + 0x78c)
/** H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH2 : RO; bitpos: [28:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH2    0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH2_M  (H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH2_V << H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH2_S)
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH2_V  0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH2_S  0

/** H264_DMA_IN_CONF0_CH3_REG register
 *  RX CH3 config0 register
 */
#define H264_DMA_IN_CONF0_CH3_REG (DR_REG_H264_DMA_BASE + 0x800)
/** H264_DMA_INDSCR_BURST_EN_CH3 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
 *  when accessing SRAM.
 */
#define H264_DMA_INDSCR_BURST_EN_CH3    (BIT(2))
#define H264_DMA_INDSCR_BURST_EN_CH3_M  (H264_DMA_INDSCR_BURST_EN_CH3_V << H264_DMA_INDSCR_BURST_EN_CH3_S)
#define H264_DMA_INDSCR_BURST_EN_CH3_V  0x00000001U
#define H264_DMA_INDSCR_BURST_EN_CH3_S  2
/** H264_DMA_IN_ECC_AES_EN_CH3 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_IN_ECC_AES_EN_CH3    (BIT(3))
#define H264_DMA_IN_ECC_AES_EN_CH3_M  (H264_DMA_IN_ECC_AES_EN_CH3_V << H264_DMA_IN_ECC_AES_EN_CH3_S)
#define H264_DMA_IN_ECC_AES_EN_CH3_V  0x00000001U
#define H264_DMA_IN_ECC_AES_EN_CH3_S  3
/** H264_DMA_IN_CHECK_OWNER_CH3 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_IN_CHECK_OWNER_CH3    (BIT(4))
#define H264_DMA_IN_CHECK_OWNER_CH3_M  (H264_DMA_IN_CHECK_OWNER_CH3_V << H264_DMA_IN_CHECK_OWNER_CH3_S)
#define H264_DMA_IN_CHECK_OWNER_CH3_V  0x00000001U
#define H264_DMA_IN_CHECK_OWNER_CH3_S  4
/** H264_DMA_IN_MEM_BURST_LENGTH_CH3 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_IN_MEM_BURST_LENGTH_CH3    0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH3_M  (H264_DMA_IN_MEM_BURST_LENGTH_CH3_V << H264_DMA_IN_MEM_BURST_LENGTH_CH3_S)
#define H264_DMA_IN_MEM_BURST_LENGTH_CH3_V  0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH3_S  6
/** H264_DMA_IN_PAGE_BOUND_EN_CH3 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define H264_DMA_IN_PAGE_BOUND_EN_CH3    (BIT(12))
#define H264_DMA_IN_PAGE_BOUND_EN_CH3_M  (H264_DMA_IN_PAGE_BOUND_EN_CH3_V << H264_DMA_IN_PAGE_BOUND_EN_CH3_S)
#define H264_DMA_IN_PAGE_BOUND_EN_CH3_V  0x00000001U
#define H264_DMA_IN_PAGE_BOUND_EN_CH3_S  12
/** H264_DMA_IN_RST_CH3 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define H264_DMA_IN_RST_CH3    (BIT(24))
#define H264_DMA_IN_RST_CH3_M  (H264_DMA_IN_RST_CH3_V << H264_DMA_IN_RST_CH3_S)
#define H264_DMA_IN_RST_CH3_V  0x00000001U
#define H264_DMA_IN_RST_CH3_S  24
/** H264_DMA_IN_CMD_DISABLE_CH3 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_IN_CMD_DISABLE_CH3    (BIT(25))
#define H264_DMA_IN_CMD_DISABLE_CH3_M  (H264_DMA_IN_CMD_DISABLE_CH3_V << H264_DMA_IN_CMD_DISABLE_CH3_S)
#define H264_DMA_IN_CMD_DISABLE_CH3_V  0x00000001U
#define H264_DMA_IN_CMD_DISABLE_CH3_S  25
/** H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH3 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH3    (BIT(26))
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH3_M  (H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH3_V << H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH3_S)
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH3_V  0x00000001U
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH3_S  26

/** H264_DMA_IN_INT_RAW_CH3_REG register
 *  RX CH3  interrupt raw register
 */
#define H264_DMA_IN_INT_RAW_CH3_REG (DR_REG_H264_DMA_BASE + 0x804)
/** H264_DMA_IN_DONE_CH3_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 1.
 */
#define H264_DMA_IN_DONE_CH3_INT_RAW    (BIT(0))
#define H264_DMA_IN_DONE_CH3_INT_RAW_M  (H264_DMA_IN_DONE_CH3_INT_RAW_V << H264_DMA_IN_DONE_CH3_INT_RAW_S)
#define H264_DMA_IN_DONE_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DONE_CH3_INT_RAW_S  0
/** H264_DMA_IN_SUC_EOF_CH3_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 1.
 */
#define H264_DMA_IN_SUC_EOF_CH3_INT_RAW    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH3_INT_RAW_M  (H264_DMA_IN_SUC_EOF_CH3_INT_RAW_V << H264_DMA_IN_SUC_EOF_CH3_INT_RAW_S)
#define H264_DMA_IN_SUC_EOF_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH3_INT_RAW_S  1
/** H264_DMA_IN_ERR_EOF_CH3_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and data error is detected
 */
#define H264_DMA_IN_ERR_EOF_CH3_INT_RAW    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH3_INT_RAW_M  (H264_DMA_IN_ERR_EOF_CH3_INT_RAW_V << H264_DMA_IN_ERR_EOF_CH3_INT_RAW_S)
#define H264_DMA_IN_ERR_EOF_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH3_INT_RAW_S  2
/** H264_DMA_IN_DSCR_ERR_CH3_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor for Rx
 *  channel 1.
 */
#define H264_DMA_IN_DSCR_ERR_CH3_INT_RAW    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH3_INT_RAW_M  (H264_DMA_IN_DSCR_ERR_CH3_INT_RAW_V << H264_DMA_IN_DSCR_ERR_CH3_INT_RAW_S)
#define H264_DMA_IN_DSCR_ERR_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH3_INT_RAW_S  3
/** H264_DMA_INFIFO_OVF_L1_CH3_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_RAW    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_RAW_M  (H264_DMA_INFIFO_OVF_L1_CH3_INT_RAW_V << H264_DMA_INFIFO_OVF_L1_CH3_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_RAW_S  4
/** H264_DMA_INFIFO_UDF_L1_CH3_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_RAW    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_RAW_M  (H264_DMA_INFIFO_UDF_L1_CH3_INT_RAW_V << H264_DMA_INFIFO_UDF_L1_CH3_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_RAW_S  5
/** H264_DMA_INFIFO_OVF_L2_CH3_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_RAW    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_RAW_M  (H264_DMA_INFIFO_OVF_L2_CH3_INT_RAW_V << H264_DMA_INFIFO_OVF_L2_CH3_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_RAW_S  6
/** H264_DMA_INFIFO_UDF_L2_CH3_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_RAW    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_RAW_M  (H264_DMA_INFIFO_UDF_L2_CH3_INT_RAW_V << H264_DMA_INFIFO_UDF_L2_CH3_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_RAW_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH3_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
 *  also remain data.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_RAW    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_RAW_M  (H264_DMA_IN_DSCR_EMPTY_CH3_INT_RAW_V << H264_DMA_IN_DSCR_EMPTY_CH3_INT_RAW_S)
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_RAW_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_RAW    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_RAW_M  (H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_RAW_V << H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_RAW_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_RAW_S  9

/** H264_DMA_IN_INT_ENA_CH3_REG register
 *  RX CH3  interrupt ena register
 */
#define H264_DMA_IN_INT_ENA_CH3_REG (DR_REG_H264_DMA_BASE + 0x808)
/** H264_DMA_IN_DONE_CH3_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH3_INT_ENA    (BIT(0))
#define H264_DMA_IN_DONE_CH3_INT_ENA_M  (H264_DMA_IN_DONE_CH3_INT_ENA_V << H264_DMA_IN_DONE_CH3_INT_ENA_S)
#define H264_DMA_IN_DONE_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DONE_CH3_INT_ENA_S  0
/** H264_DMA_IN_SUC_EOF_CH3_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH3_INT_ENA    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH3_INT_ENA_M  (H264_DMA_IN_SUC_EOF_CH3_INT_ENA_V << H264_DMA_IN_SUC_EOF_CH3_INT_ENA_S)
#define H264_DMA_IN_SUC_EOF_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH3_INT_ENA_S  1
/** H264_DMA_IN_ERR_EOF_CH3_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH3_INT_ENA    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH3_INT_ENA_M  (H264_DMA_IN_ERR_EOF_CH3_INT_ENA_V << H264_DMA_IN_ERR_EOF_CH3_INT_ENA_S)
#define H264_DMA_IN_ERR_EOF_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH3_INT_ENA_S  2
/** H264_DMA_IN_DSCR_ERR_CH3_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ENA    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ENA_M  (H264_DMA_IN_DSCR_ERR_CH3_INT_ENA_V << H264_DMA_IN_DSCR_ERR_CH3_INT_ENA_S)
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ENA_S  3
/** H264_DMA_INFIFO_OVF_L1_CH3_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ENA    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ENA_M  (H264_DMA_INFIFO_OVF_L1_CH3_INT_ENA_V << H264_DMA_INFIFO_OVF_L1_CH3_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ENA_S  4
/** H264_DMA_INFIFO_UDF_L1_CH3_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ENA    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ENA_M  (H264_DMA_INFIFO_UDF_L1_CH3_INT_ENA_V << H264_DMA_INFIFO_UDF_L1_CH3_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ENA_S  5
/** H264_DMA_INFIFO_OVF_L2_CH3_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ENA    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ENA_M  (H264_DMA_INFIFO_OVF_L2_CH3_INT_ENA_V << H264_DMA_INFIFO_OVF_L2_CH3_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ENA_S  6
/** H264_DMA_INFIFO_UDF_L2_CH3_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ENA    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ENA_M  (H264_DMA_INFIFO_UDF_L2_CH3_INT_ENA_V << H264_DMA_INFIFO_UDF_L2_CH3_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ENA_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH3_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ENA    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ENA_M  (H264_DMA_IN_DSCR_EMPTY_CH3_INT_ENA_V << H264_DMA_IN_DSCR_EMPTY_CH3_INT_ENA_S)
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ENA_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ENA    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ENA_M  (H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ENA_V << H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ENA_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ENA_S  9

/** H264_DMA_IN_INT_ST_CH3_REG register
 *  RX CH3  interrupt st register
 */
#define H264_DMA_IN_INT_ST_CH3_REG (DR_REG_H264_DMA_BASE + 0x80c)
/** H264_DMA_IN_DONE_CH3_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH3_INT_ST    (BIT(0))
#define H264_DMA_IN_DONE_CH3_INT_ST_M  (H264_DMA_IN_DONE_CH3_INT_ST_V << H264_DMA_IN_DONE_CH3_INT_ST_S)
#define H264_DMA_IN_DONE_CH3_INT_ST_V  0x00000001U
#define H264_DMA_IN_DONE_CH3_INT_ST_S  0
/** H264_DMA_IN_SUC_EOF_CH3_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH3_INT_ST    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH3_INT_ST_M  (H264_DMA_IN_SUC_EOF_CH3_INT_ST_V << H264_DMA_IN_SUC_EOF_CH3_INT_ST_S)
#define H264_DMA_IN_SUC_EOF_CH3_INT_ST_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH3_INT_ST_S  1
/** H264_DMA_IN_ERR_EOF_CH3_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH3_INT_ST    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH3_INT_ST_M  (H264_DMA_IN_ERR_EOF_CH3_INT_ST_V << H264_DMA_IN_ERR_EOF_CH3_INT_ST_S)
#define H264_DMA_IN_ERR_EOF_CH3_INT_ST_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH3_INT_ST_S  2
/** H264_DMA_IN_DSCR_ERR_CH3_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ST    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ST_M  (H264_DMA_IN_DSCR_ERR_CH3_INT_ST_V << H264_DMA_IN_DSCR_ERR_CH3_INT_ST_S)
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH3_INT_ST_S  3
/** H264_DMA_INFIFO_OVF_L1_CH3_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ST    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ST_M  (H264_DMA_INFIFO_OVF_L1_CH3_INT_ST_V << H264_DMA_INFIFO_OVF_L1_CH3_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_ST_S  4
/** H264_DMA_INFIFO_UDF_L1_CH3_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ST    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ST_M  (H264_DMA_INFIFO_UDF_L1_CH3_INT_ST_V << H264_DMA_INFIFO_UDF_L1_CH3_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_ST_S  5
/** H264_DMA_INFIFO_OVF_L2_CH3_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ST    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ST_M  (H264_DMA_INFIFO_OVF_L2_CH3_INT_ST_V << H264_DMA_INFIFO_OVF_L2_CH3_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_ST_S  6
/** H264_DMA_INFIFO_UDF_L2_CH3_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ST    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ST_M  (H264_DMA_INFIFO_UDF_L2_CH3_INT_ST_V << H264_DMA_INFIFO_UDF_L2_CH3_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_ST_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH3_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ST    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ST_M  (H264_DMA_IN_DSCR_EMPTY_CH3_INT_ST_V << H264_DMA_IN_DSCR_EMPTY_CH3_INT_ST_S)
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_ST_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ST    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ST_M  (H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ST_V << H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ST_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_ST_S  9

/** H264_DMA_IN_INT_CLR_CH3_REG register
 *  RX CH3  interrupt clr register
 */
#define H264_DMA_IN_INT_CLR_CH3_REG (DR_REG_H264_DMA_BASE + 0x810)
/** H264_DMA_IN_DONE_CH3_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH3_INT_CLR    (BIT(0))
#define H264_DMA_IN_DONE_CH3_INT_CLR_M  (H264_DMA_IN_DONE_CH3_INT_CLR_V << H264_DMA_IN_DONE_CH3_INT_CLR_S)
#define H264_DMA_IN_DONE_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DONE_CH3_INT_CLR_S  0
/** H264_DMA_IN_SUC_EOF_CH3_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH3_INT_CLR    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH3_INT_CLR_M  (H264_DMA_IN_SUC_EOF_CH3_INT_CLR_V << H264_DMA_IN_SUC_EOF_CH3_INT_CLR_S)
#define H264_DMA_IN_SUC_EOF_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH3_INT_CLR_S  1
/** H264_DMA_IN_ERR_EOF_CH3_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH3_INT_CLR    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH3_INT_CLR_M  (H264_DMA_IN_ERR_EOF_CH3_INT_CLR_V << H264_DMA_IN_ERR_EOF_CH3_INT_CLR_S)
#define H264_DMA_IN_ERR_EOF_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH3_INT_CLR_S  2
/** H264_DMA_IN_DSCR_ERR_CH3_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH3_INT_CLR    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH3_INT_CLR_M  (H264_DMA_IN_DSCR_ERR_CH3_INT_CLR_V << H264_DMA_IN_DSCR_ERR_CH3_INT_CLR_S)
#define H264_DMA_IN_DSCR_ERR_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH3_INT_CLR_S  3
/** H264_DMA_INFIFO_OVF_L1_CH3_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_CLR    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_CLR_M  (H264_DMA_INFIFO_OVF_L1_CH3_INT_CLR_V << H264_DMA_INFIFO_OVF_L1_CH3_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH3_INT_CLR_S  4
/** H264_DMA_INFIFO_UDF_L1_CH3_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_CLR    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_CLR_M  (H264_DMA_INFIFO_UDF_L1_CH3_INT_CLR_V << H264_DMA_INFIFO_UDF_L1_CH3_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH3_INT_CLR_S  5
/** H264_DMA_INFIFO_OVF_L2_CH3_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_CLR    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_CLR_M  (H264_DMA_INFIFO_OVF_L2_CH3_INT_CLR_V << H264_DMA_INFIFO_OVF_L2_CH3_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH3_INT_CLR_S  6
/** H264_DMA_INFIFO_UDF_L2_CH3_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_CLR    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_CLR_M  (H264_DMA_INFIFO_UDF_L2_CH3_INT_CLR_V << H264_DMA_INFIFO_UDF_L2_CH3_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH3_INT_CLR_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH3_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_CLR    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_CLR_M  (H264_DMA_IN_DSCR_EMPTY_CH3_INT_CLR_V << H264_DMA_IN_DSCR_EMPTY_CH3_INT_CLR_S)
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH3_INT_CLR_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_CLR    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_CLR_M  (H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_CLR_V << H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_CLR_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH3_INT_CLR_S  9

/** H264_DMA_INFIFO_STATUS_CH3_REG register
 *  RX CH3 INFIFO status register
 */
#define H264_DMA_INFIFO_STATUS_CH3_REG (DR_REG_H264_DMA_BASE + 0x814)
/** H264_DMA_INFIFO_FULL_L2_CH3 : RO; bitpos: [0]; default: 0;
 *  Rx FIFO full signal for Rx channel.
 */
#define H264_DMA_INFIFO_FULL_L2_CH3    (BIT(0))
#define H264_DMA_INFIFO_FULL_L2_CH3_M  (H264_DMA_INFIFO_FULL_L2_CH3_V << H264_DMA_INFIFO_FULL_L2_CH3_S)
#define H264_DMA_INFIFO_FULL_L2_CH3_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L2_CH3_S  0
/** H264_DMA_INFIFO_EMPTY_L2_CH3 : RO; bitpos: [1]; default: 1;
 *  Rx FIFO empty signal for Rx channel.
 */
#define H264_DMA_INFIFO_EMPTY_L2_CH3    (BIT(1))
#define H264_DMA_INFIFO_EMPTY_L2_CH3_M  (H264_DMA_INFIFO_EMPTY_L2_CH3_V << H264_DMA_INFIFO_EMPTY_L2_CH3_S)
#define H264_DMA_INFIFO_EMPTY_L2_CH3_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L2_CH3_S  1
/** H264_DMA_INFIFO_CNT_L2_CH3 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Rx FIFO for Rx channel.
 */
#define H264_DMA_INFIFO_CNT_L2_CH3    0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH3_M  (H264_DMA_INFIFO_CNT_L2_CH3_V << H264_DMA_INFIFO_CNT_L2_CH3_S)
#define H264_DMA_INFIFO_CNT_L2_CH3_V  0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH3_S  2
/** H264_DMA_INFIFO_FULL_L1_CH3 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L1_CH3    (BIT(6))
#define H264_DMA_INFIFO_FULL_L1_CH3_M  (H264_DMA_INFIFO_FULL_L1_CH3_V << H264_DMA_INFIFO_FULL_L1_CH3_S)
#define H264_DMA_INFIFO_FULL_L1_CH3_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L1_CH3_S  6
/** H264_DMA_INFIFO_EMPTY_L1_CH3 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L1_CH3    (BIT(7))
#define H264_DMA_INFIFO_EMPTY_L1_CH3_M  (H264_DMA_INFIFO_EMPTY_L1_CH3_V << H264_DMA_INFIFO_EMPTY_L1_CH3_S)
#define H264_DMA_INFIFO_EMPTY_L1_CH3_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L1_CH3_S  7
/** H264_DMA_INFIFO_CNT_L1_CH3 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L1_CH3    0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH3_M  (H264_DMA_INFIFO_CNT_L1_CH3_V << H264_DMA_INFIFO_CNT_L1_CH3_S)
#define H264_DMA_INFIFO_CNT_L1_CH3_V  0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH3_S  8
/** H264_DMA_INFIFO_FULL_L3_CH3 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L3_CH3    (BIT(16))
#define H264_DMA_INFIFO_FULL_L3_CH3_M  (H264_DMA_INFIFO_FULL_L3_CH3_V << H264_DMA_INFIFO_FULL_L3_CH3_S)
#define H264_DMA_INFIFO_FULL_L3_CH3_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L3_CH3_S  16
/** H264_DMA_INFIFO_EMPTY_L3_CH3 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L3_CH3    (BIT(17))
#define H264_DMA_INFIFO_EMPTY_L3_CH3_M  (H264_DMA_INFIFO_EMPTY_L3_CH3_V << H264_DMA_INFIFO_EMPTY_L3_CH3_S)
#define H264_DMA_INFIFO_EMPTY_L3_CH3_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L3_CH3_S  17
/** H264_DMA_INFIFO_CNT_L3_CH3 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L3_CH3    0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH3_M  (H264_DMA_INFIFO_CNT_L3_CH3_V << H264_DMA_INFIFO_CNT_L3_CH3_S)
#define H264_DMA_INFIFO_CNT_L3_CH3_V  0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH3_S  18

/** H264_DMA_IN_POP_CH3_REG register
 *  RX CH3 INFIFO pop register
 */
#define H264_DMA_IN_POP_CH3_REG (DR_REG_H264_DMA_BASE + 0x818)
/** H264_DMA_INFIFO_RDATA_CH3 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_RDATA_CH3    0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH3_M  (H264_DMA_INFIFO_RDATA_CH3_V << H264_DMA_INFIFO_RDATA_CH3_S)
#define H264_DMA_INFIFO_RDATA_CH3_V  0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH3_S  0
/** H264_DMA_INFIFO_POP_CH3 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_POP_CH3    (BIT(11))
#define H264_DMA_INFIFO_POP_CH3_M  (H264_DMA_INFIFO_POP_CH3_V << H264_DMA_INFIFO_POP_CH3_S)
#define H264_DMA_INFIFO_POP_CH3_V  0x00000001U
#define H264_DMA_INFIFO_POP_CH3_S  11

/** H264_DMA_IN_LINK_CONF_CH3_REG register
 *  RX CH3 in_link dscr ctrl register
 */
#define H264_DMA_IN_LINK_CONF_CH3_REG (DR_REG_H264_DMA_BASE + 0x81c)
/** H264_DMA_INLINK_AUTO_RET_CH3 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_INLINK_AUTO_RET_CH3    (BIT(20))
#define H264_DMA_INLINK_AUTO_RET_CH3_M  (H264_DMA_INLINK_AUTO_RET_CH3_V << H264_DMA_INLINK_AUTO_RET_CH3_S)
#define H264_DMA_INLINK_AUTO_RET_CH3_V  0x00000001U
#define H264_DMA_INLINK_AUTO_RET_CH3_S  20
/** H264_DMA_INLINK_STOP_CH3 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_STOP_CH3    (BIT(21))
#define H264_DMA_INLINK_STOP_CH3_M  (H264_DMA_INLINK_STOP_CH3_V << H264_DMA_INLINK_STOP_CH3_S)
#define H264_DMA_INLINK_STOP_CH3_V  0x00000001U
#define H264_DMA_INLINK_STOP_CH3_S  21
/** H264_DMA_INLINK_START_CH3 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_START_CH3    (BIT(22))
#define H264_DMA_INLINK_START_CH3_M  (H264_DMA_INLINK_START_CH3_V << H264_DMA_INLINK_START_CH3_S)
#define H264_DMA_INLINK_START_CH3_V  0x00000001U
#define H264_DMA_INLINK_START_CH3_S  22
/** H264_DMA_INLINK_RESTART_CH3 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define H264_DMA_INLINK_RESTART_CH3    (BIT(23))
#define H264_DMA_INLINK_RESTART_CH3_M  (H264_DMA_INLINK_RESTART_CH3_V << H264_DMA_INLINK_RESTART_CH3_S)
#define H264_DMA_INLINK_RESTART_CH3_V  0x00000001U
#define H264_DMA_INLINK_RESTART_CH3_S  23
/** H264_DMA_INLINK_PARK_CH3 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define H264_DMA_INLINK_PARK_CH3    (BIT(24))
#define H264_DMA_INLINK_PARK_CH3_M  (H264_DMA_INLINK_PARK_CH3_V << H264_DMA_INLINK_PARK_CH3_S)
#define H264_DMA_INLINK_PARK_CH3_V  0x00000001U
#define H264_DMA_INLINK_PARK_CH3_S  24

/** H264_DMA_IN_LINK_ADDR_CH3_REG register
 *  RX CH3 in_link dscr addr register
 */
#define H264_DMA_IN_LINK_ADDR_CH3_REG (DR_REG_H264_DMA_BASE + 0x820)
/** H264_DMA_INLINK_ADDR_CH3 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first inlink descriptor's address.
 */
#define H264_DMA_INLINK_ADDR_CH3    0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH3_M  (H264_DMA_INLINK_ADDR_CH3_V << H264_DMA_INLINK_ADDR_CH3_S)
#define H264_DMA_INLINK_ADDR_CH3_V  0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH3_S  0

/** H264_DMA_IN_STATE_CH3_REG register
 *  RX CH3 state register
 */
#define H264_DMA_IN_STATE_CH3_REG (DR_REG_H264_DMA_BASE + 0x824)
/** H264_DMA_INLINK_DSCR_ADDR_CH3 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define H264_DMA_INLINK_DSCR_ADDR_CH3    0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH3_M  (H264_DMA_INLINK_DSCR_ADDR_CH3_V << H264_DMA_INLINK_DSCR_ADDR_CH3_S)
#define H264_DMA_INLINK_DSCR_ADDR_CH3_V  0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH3_S  0
/** H264_DMA_IN_DSCR_STATE_CH3 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_IN_DSCR_STATE_CH3    0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH3_M  (H264_DMA_IN_DSCR_STATE_CH3_V << H264_DMA_IN_DSCR_STATE_CH3_S)
#define H264_DMA_IN_DSCR_STATE_CH3_V  0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH3_S  18
/** H264_DMA_IN_STATE_CH3 : RO; bitpos: [22:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_IN_STATE_CH3    0x00000007U
#define H264_DMA_IN_STATE_CH3_M  (H264_DMA_IN_STATE_CH3_V << H264_DMA_IN_STATE_CH3_S)
#define H264_DMA_IN_STATE_CH3_V  0x00000007U
#define H264_DMA_IN_STATE_CH3_S  20
/** H264_DMA_IN_RESET_AVAIL_CH3 : RO; bitpos: [23]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_IN_RESET_AVAIL_CH3    (BIT(23))
#define H264_DMA_IN_RESET_AVAIL_CH3_M  (H264_DMA_IN_RESET_AVAIL_CH3_V << H264_DMA_IN_RESET_AVAIL_CH3_S)
#define H264_DMA_IN_RESET_AVAIL_CH3_V  0x00000001U
#define H264_DMA_IN_RESET_AVAIL_CH3_S  23

/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH3_REG register
 *  RX CH3 eof des addr register
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH3_REG (DR_REG_H264_DMA_BASE + 0x828)
/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH3 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH3    0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH3_M  (H264_DMA_IN_SUC_EOF_DES_ADDR_CH3_V << H264_DMA_IN_SUC_EOF_DES_ADDR_CH3_S)
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH3_V  0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH3_S  0

/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH3_REG register
 *  RX CH3 err eof des addr register
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH3_REG (DR_REG_H264_DMA_BASE + 0x82c)
/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH3 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH3    0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH3_M  (H264_DMA_IN_ERR_EOF_DES_ADDR_CH3_V << H264_DMA_IN_ERR_EOF_DES_ADDR_CH3_S)
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH3_V  0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH3_S  0

/** H264_DMA_IN_DSCR_CH3_REG register
 *  RX CH3 next dscr addr register
 */
#define H264_DMA_IN_DSCR_CH3_REG (DR_REG_H264_DMA_BASE + 0x830)
/** H264_DMA_INLINK_DSCR_CH3 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next inlink descriptor address x.
 */
#define H264_DMA_INLINK_DSCR_CH3    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH3_M  (H264_DMA_INLINK_DSCR_CH3_V << H264_DMA_INLINK_DSCR_CH3_S)
#define H264_DMA_INLINK_DSCR_CH3_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH3_S  0

/** H264_DMA_IN_DSCR_BF0_CH3_REG register
 *  RX CH3 last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF0_CH3_REG (DR_REG_H264_DMA_BASE + 0x834)
/** H264_DMA_INLINK_DSCR_BF0_CH3 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor's next address x-1.
 */
#define H264_DMA_INLINK_DSCR_BF0_CH3    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH3_M  (H264_DMA_INLINK_DSCR_BF0_CH3_V << H264_DMA_INLINK_DSCR_BF0_CH3_S)
#define H264_DMA_INLINK_DSCR_BF0_CH3_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH3_S  0

/** H264_DMA_IN_DSCR_BF1_CH3_REG register
 *  RX CH3 second-to-last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF1_CH3_REG (DR_REG_H264_DMA_BASE + 0x838)
/** H264_DMA_INLINK_DSCR_BF1_CH3 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor's next address x-2.
 */
#define H264_DMA_INLINK_DSCR_BF1_CH3    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH3_M  (H264_DMA_INLINK_DSCR_BF1_CH3_V << H264_DMA_INLINK_DSCR_BF1_CH3_S)
#define H264_DMA_INLINK_DSCR_BF1_CH3_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH3_S  0

/** H264_DMA_IN_ARB_CH3_REG register
 *  RX CH3 arb register
 */
#define H264_DMA_IN_ARB_CH3_REG (DR_REG_H264_DMA_BASE + 0x840)
/** H264_DMA_IN_ARB_TOKEN_NUM_CH3 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_IN_ARB_TOKEN_NUM_CH3    0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH3_M  (H264_DMA_IN_ARB_TOKEN_NUM_CH3_V << H264_DMA_IN_ARB_TOKEN_NUM_CH3_S)
#define H264_DMA_IN_ARB_TOKEN_NUM_CH3_V  0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH3_S  0
/** H264_DMA_INTER_IN_ARB_PRIORITY_CH3 : R/W; bitpos: [8:6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH3    0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH3_M  (H264_DMA_INTER_IN_ARB_PRIORITY_CH3_V << H264_DMA_INTER_IN_ARB_PRIORITY_CH3_S)
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH3_V  0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH3_S  6

/** H264_DMA_IN_ETM_CONF_CH3_REG register
 *  RX CH3 ETM config register
 */
#define H264_DMA_IN_ETM_CONF_CH3_REG (DR_REG_H264_DMA_BASE + 0x848)
/** H264_DMA_IN_ETM_EN_CH3 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_IN_ETM_EN_CH3    (BIT(0))
#define H264_DMA_IN_ETM_EN_CH3_M  (H264_DMA_IN_ETM_EN_CH3_V << H264_DMA_IN_ETM_EN_CH3_S)
#define H264_DMA_IN_ETM_EN_CH3_V  0x00000001U
#define H264_DMA_IN_ETM_EN_CH3_S  0
/** H264_DMA_IN_ETM_LOOP_EN_CH3 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_IN_ETM_LOOP_EN_CH3    (BIT(1))
#define H264_DMA_IN_ETM_LOOP_EN_CH3_M  (H264_DMA_IN_ETM_LOOP_EN_CH3_V << H264_DMA_IN_ETM_LOOP_EN_CH3_S)
#define H264_DMA_IN_ETM_LOOP_EN_CH3_V  0x00000001U
#define H264_DMA_IN_ETM_LOOP_EN_CH3_S  1
/** H264_DMA_IN_DSCR_TASK_MAK_CH3 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_IN_DSCR_TASK_MAK_CH3    0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH3_M  (H264_DMA_IN_DSCR_TASK_MAK_CH3_V << H264_DMA_IN_DSCR_TASK_MAK_CH3_S)
#define H264_DMA_IN_DSCR_TASK_MAK_CH3_V  0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH3_S  2

/** H264_DMA_IN_FIFO_CNT_CH3_REG register
 *  rx CH3 fifo cnt register
 */
#define H264_DMA_IN_FIFO_CNT_CH3_REG (DR_REG_H264_DMA_BASE + 0x880)
/** H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH3 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH3    0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH3_M  (H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH3_V << H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH3_S)
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH3_V  0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH3_S  0

/** H264_DMA_IN_POP_DATA_CNT_CH3_REG register
 *  rx CH3 pop data cnt register
 */
#define H264_DMA_IN_POP_DATA_CNT_CH3_REG (DR_REG_H264_DMA_BASE + 0x884)
/** H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH3 : RO; bitpos: [7:0]; default: 7;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH3    0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH3_M  (H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH3_V << H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH3_S)
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH3_V  0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH3_S  0

/** H264_DMA_IN_XADDR_CH3_REG register
 *  rx CH3 xaddr register
 */
#define H264_DMA_IN_XADDR_CH3_REG (DR_REG_H264_DMA_BASE + 0x888)
/** H264_DMA_IN_CMDFIFO_XADDR_CH3 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_XADDR_CH3    0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH3_M  (H264_DMA_IN_CMDFIFO_XADDR_CH3_V << H264_DMA_IN_CMDFIFO_XADDR_CH3_S)
#define H264_DMA_IN_CMDFIFO_XADDR_CH3_V  0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH3_S  0

/** H264_DMA_IN_BUF_HB_RCV_CH3_REG register
 *  rx CH3 buf len hb rcv register
 */
#define H264_DMA_IN_BUF_HB_RCV_CH3_REG (DR_REG_H264_DMA_BASE + 0x88c)
/** H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH3 : RO; bitpos: [28:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH3    0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH3_M  (H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH3_V << H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH3_S)
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH3_V  0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH3_S  0

/** H264_DMA_IN_CONF0_CH4_REG register
 *  RX CH4 config0 register
 */
#define H264_DMA_IN_CONF0_CH4_REG (DR_REG_H264_DMA_BASE + 0x900)
/** H264_DMA_INDSCR_BURST_EN_CH4 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
 *  when accessing SRAM.
 */
#define H264_DMA_INDSCR_BURST_EN_CH4    (BIT(2))
#define H264_DMA_INDSCR_BURST_EN_CH4_M  (H264_DMA_INDSCR_BURST_EN_CH4_V << H264_DMA_INDSCR_BURST_EN_CH4_S)
#define H264_DMA_INDSCR_BURST_EN_CH4_V  0x00000001U
#define H264_DMA_INDSCR_BURST_EN_CH4_S  2
/** H264_DMA_IN_ECC_AES_EN_CH4 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_IN_ECC_AES_EN_CH4    (BIT(3))
#define H264_DMA_IN_ECC_AES_EN_CH4_M  (H264_DMA_IN_ECC_AES_EN_CH4_V << H264_DMA_IN_ECC_AES_EN_CH4_S)
#define H264_DMA_IN_ECC_AES_EN_CH4_V  0x00000001U
#define H264_DMA_IN_ECC_AES_EN_CH4_S  3
/** H264_DMA_IN_CHECK_OWNER_CH4 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define H264_DMA_IN_CHECK_OWNER_CH4    (BIT(4))
#define H264_DMA_IN_CHECK_OWNER_CH4_M  (H264_DMA_IN_CHECK_OWNER_CH4_V << H264_DMA_IN_CHECK_OWNER_CH4_S)
#define H264_DMA_IN_CHECK_OWNER_CH4_V  0x00000001U
#define H264_DMA_IN_CHECK_OWNER_CH4_S  4
/** H264_DMA_IN_MEM_BURST_LENGTH_CH4 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_IN_MEM_BURST_LENGTH_CH4    0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH4_M  (H264_DMA_IN_MEM_BURST_LENGTH_CH4_V << H264_DMA_IN_MEM_BURST_LENGTH_CH4_S)
#define H264_DMA_IN_MEM_BURST_LENGTH_CH4_V  0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH4_S  6
/** H264_DMA_IN_PAGE_BOUND_EN_CH4 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define H264_DMA_IN_PAGE_BOUND_EN_CH4    (BIT(12))
#define H264_DMA_IN_PAGE_BOUND_EN_CH4_M  (H264_DMA_IN_PAGE_BOUND_EN_CH4_V << H264_DMA_IN_PAGE_BOUND_EN_CH4_S)
#define H264_DMA_IN_PAGE_BOUND_EN_CH4_V  0x00000001U
#define H264_DMA_IN_PAGE_BOUND_EN_CH4_S  12
/** H264_DMA_IN_RST_CH4 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define H264_DMA_IN_RST_CH4    (BIT(24))
#define H264_DMA_IN_RST_CH4_M  (H264_DMA_IN_RST_CH4_V << H264_DMA_IN_RST_CH4_S)
#define H264_DMA_IN_RST_CH4_V  0x00000001U
#define H264_DMA_IN_RST_CH4_S  24
/** H264_DMA_IN_CMD_DISABLE_CH4 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_IN_CMD_DISABLE_CH4    (BIT(25))
#define H264_DMA_IN_CMD_DISABLE_CH4_M  (H264_DMA_IN_CMD_DISABLE_CH4_V << H264_DMA_IN_CMD_DISABLE_CH4_S)
#define H264_DMA_IN_CMD_DISABLE_CH4_V  0x00000001U
#define H264_DMA_IN_CMD_DISABLE_CH4_S  25
/** H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH4 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH4    (BIT(26))
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH4_M  (H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH4_V << H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH4_S)
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH4_V  0x00000001U
#define H264_DMA_IN_ARB_WEIGHT_OPT_DIS_CH4_S  26

/** H264_DMA_IN_INT_RAW_CH4_REG register
 *  RX CH4  interrupt raw register
 */
#define H264_DMA_IN_INT_RAW_CH4_REG (DR_REG_H264_DMA_BASE + 0x904)
/** H264_DMA_IN_DONE_CH4_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 1.
 */
#define H264_DMA_IN_DONE_CH4_INT_RAW    (BIT(0))
#define H264_DMA_IN_DONE_CH4_INT_RAW_M  (H264_DMA_IN_DONE_CH4_INT_RAW_V << H264_DMA_IN_DONE_CH4_INT_RAW_S)
#define H264_DMA_IN_DONE_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DONE_CH4_INT_RAW_S  0
/** H264_DMA_IN_SUC_EOF_CH4_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 1.
 */
#define H264_DMA_IN_SUC_EOF_CH4_INT_RAW    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH4_INT_RAW_M  (H264_DMA_IN_SUC_EOF_CH4_INT_RAW_V << H264_DMA_IN_SUC_EOF_CH4_INT_RAW_S)
#define H264_DMA_IN_SUC_EOF_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH4_INT_RAW_S  1
/** H264_DMA_IN_ERR_EOF_CH4_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and data error is detected
 */
#define H264_DMA_IN_ERR_EOF_CH4_INT_RAW    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH4_INT_RAW_M  (H264_DMA_IN_ERR_EOF_CH4_INT_RAW_V << H264_DMA_IN_ERR_EOF_CH4_INT_RAW_S)
#define H264_DMA_IN_ERR_EOF_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH4_INT_RAW_S  2
/** H264_DMA_IN_DSCR_ERR_CH4_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor for Rx
 *  channel 1.
 */
#define H264_DMA_IN_DSCR_ERR_CH4_INT_RAW    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH4_INT_RAW_M  (H264_DMA_IN_DSCR_ERR_CH4_INT_RAW_V << H264_DMA_IN_DSCR_ERR_CH4_INT_RAW_S)
#define H264_DMA_IN_DSCR_ERR_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH4_INT_RAW_S  3
/** H264_DMA_INFIFO_OVF_L1_CH4_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_RAW    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_RAW_M  (H264_DMA_INFIFO_OVF_L1_CH4_INT_RAW_V << H264_DMA_INFIFO_OVF_L1_CH4_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_RAW_S  4
/** H264_DMA_INFIFO_UDF_L1_CH4_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_RAW    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_RAW_M  (H264_DMA_INFIFO_UDF_L1_CH4_INT_RAW_V << H264_DMA_INFIFO_UDF_L1_CH4_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_RAW_S  5
/** H264_DMA_INFIFO_OVF_L2_CH4_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_RAW    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_RAW_M  (H264_DMA_INFIFO_OVF_L2_CH4_INT_RAW_V << H264_DMA_INFIFO_OVF_L2_CH4_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_RAW_S  6
/** H264_DMA_INFIFO_UDF_L2_CH4_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_RAW    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_RAW_M  (H264_DMA_INFIFO_UDF_L2_CH4_INT_RAW_V << H264_DMA_INFIFO_UDF_L2_CH4_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_RAW_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH4_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
 *  also remain data.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_RAW    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_RAW_M  (H264_DMA_IN_DSCR_EMPTY_CH4_INT_RAW_V << H264_DMA_IN_DSCR_EMPTY_CH4_INT_RAW_S)
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_RAW_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_RAW    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_RAW_M  (H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_RAW_V << H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_RAW_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_RAW_S  9

/** H264_DMA_IN_INT_ENA_CH4_REG register
 *  RX CH4  interrupt ena register
 */
#define H264_DMA_IN_INT_ENA_CH4_REG (DR_REG_H264_DMA_BASE + 0x908)
/** H264_DMA_IN_DONE_CH4_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH4_INT_ENA    (BIT(0))
#define H264_DMA_IN_DONE_CH4_INT_ENA_M  (H264_DMA_IN_DONE_CH4_INT_ENA_V << H264_DMA_IN_DONE_CH4_INT_ENA_S)
#define H264_DMA_IN_DONE_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DONE_CH4_INT_ENA_S  0
/** H264_DMA_IN_SUC_EOF_CH4_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH4_INT_ENA    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH4_INT_ENA_M  (H264_DMA_IN_SUC_EOF_CH4_INT_ENA_V << H264_DMA_IN_SUC_EOF_CH4_INT_ENA_S)
#define H264_DMA_IN_SUC_EOF_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH4_INT_ENA_S  1
/** H264_DMA_IN_ERR_EOF_CH4_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH4_INT_ENA    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH4_INT_ENA_M  (H264_DMA_IN_ERR_EOF_CH4_INT_ENA_V << H264_DMA_IN_ERR_EOF_CH4_INT_ENA_S)
#define H264_DMA_IN_ERR_EOF_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH4_INT_ENA_S  2
/** H264_DMA_IN_DSCR_ERR_CH4_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ENA    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ENA_M  (H264_DMA_IN_DSCR_ERR_CH4_INT_ENA_V << H264_DMA_IN_DSCR_ERR_CH4_INT_ENA_S)
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ENA_S  3
/** H264_DMA_INFIFO_OVF_L1_CH4_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ENA    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ENA_M  (H264_DMA_INFIFO_OVF_L1_CH4_INT_ENA_V << H264_DMA_INFIFO_OVF_L1_CH4_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ENA_S  4
/** H264_DMA_INFIFO_UDF_L1_CH4_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ENA    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ENA_M  (H264_DMA_INFIFO_UDF_L1_CH4_INT_ENA_V << H264_DMA_INFIFO_UDF_L1_CH4_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ENA_S  5
/** H264_DMA_INFIFO_OVF_L2_CH4_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ENA    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ENA_M  (H264_DMA_INFIFO_OVF_L2_CH4_INT_ENA_V << H264_DMA_INFIFO_OVF_L2_CH4_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ENA_S  6
/** H264_DMA_INFIFO_UDF_L2_CH4_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ENA    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ENA_M  (H264_DMA_INFIFO_UDF_L2_CH4_INT_ENA_V << H264_DMA_INFIFO_UDF_L2_CH4_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ENA_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH4_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ENA    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ENA_M  (H264_DMA_IN_DSCR_EMPTY_CH4_INT_ENA_V << H264_DMA_IN_DSCR_EMPTY_CH4_INT_ENA_S)
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ENA_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ENA    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ENA_M  (H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ENA_V << H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ENA_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ENA_S  9

/** H264_DMA_IN_INT_ST_CH4_REG register
 *  RX CH4  interrupt st register
 */
#define H264_DMA_IN_INT_ST_CH4_REG (DR_REG_H264_DMA_BASE + 0x90c)
/** H264_DMA_IN_DONE_CH4_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH4_INT_ST    (BIT(0))
#define H264_DMA_IN_DONE_CH4_INT_ST_M  (H264_DMA_IN_DONE_CH4_INT_ST_V << H264_DMA_IN_DONE_CH4_INT_ST_S)
#define H264_DMA_IN_DONE_CH4_INT_ST_V  0x00000001U
#define H264_DMA_IN_DONE_CH4_INT_ST_S  0
/** H264_DMA_IN_SUC_EOF_CH4_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH4_INT_ST    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH4_INT_ST_M  (H264_DMA_IN_SUC_EOF_CH4_INT_ST_V << H264_DMA_IN_SUC_EOF_CH4_INT_ST_S)
#define H264_DMA_IN_SUC_EOF_CH4_INT_ST_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH4_INT_ST_S  1
/** H264_DMA_IN_ERR_EOF_CH4_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH4_INT_ST    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH4_INT_ST_M  (H264_DMA_IN_ERR_EOF_CH4_INT_ST_V << H264_DMA_IN_ERR_EOF_CH4_INT_ST_S)
#define H264_DMA_IN_ERR_EOF_CH4_INT_ST_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH4_INT_ST_S  2
/** H264_DMA_IN_DSCR_ERR_CH4_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ST    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ST_M  (H264_DMA_IN_DSCR_ERR_CH4_INT_ST_V << H264_DMA_IN_DSCR_ERR_CH4_INT_ST_S)
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH4_INT_ST_S  3
/** H264_DMA_INFIFO_OVF_L1_CH4_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ST    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ST_M  (H264_DMA_INFIFO_OVF_L1_CH4_INT_ST_V << H264_DMA_INFIFO_OVF_L1_CH4_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_ST_S  4
/** H264_DMA_INFIFO_UDF_L1_CH4_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ST    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ST_M  (H264_DMA_INFIFO_UDF_L1_CH4_INT_ST_V << H264_DMA_INFIFO_UDF_L1_CH4_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_ST_S  5
/** H264_DMA_INFIFO_OVF_L2_CH4_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ST    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ST_M  (H264_DMA_INFIFO_OVF_L2_CH4_INT_ST_V << H264_DMA_INFIFO_OVF_L2_CH4_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_ST_S  6
/** H264_DMA_INFIFO_UDF_L2_CH4_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ST    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ST_M  (H264_DMA_INFIFO_UDF_L2_CH4_INT_ST_V << H264_DMA_INFIFO_UDF_L2_CH4_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_ST_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH4_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ST    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ST_M  (H264_DMA_IN_DSCR_EMPTY_CH4_INT_ST_V << H264_DMA_IN_DSCR_EMPTY_CH4_INT_ST_S)
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_ST_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ST    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ST_M  (H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ST_V << H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ST_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ST_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_ST_S  9

/** H264_DMA_IN_INT_CLR_CH4_REG register
 *  RX CH4  interrupt clr register
 */
#define H264_DMA_IN_INT_CLR_CH4_REG (DR_REG_H264_DMA_BASE + 0x910)
/** H264_DMA_IN_DONE_CH4_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH4_INT_CLR    (BIT(0))
#define H264_DMA_IN_DONE_CH4_INT_CLR_M  (H264_DMA_IN_DONE_CH4_INT_CLR_V << H264_DMA_IN_DONE_CH4_INT_CLR_S)
#define H264_DMA_IN_DONE_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DONE_CH4_INT_CLR_S  0
/** H264_DMA_IN_SUC_EOF_CH4_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH4_INT_CLR    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH4_INT_CLR_M  (H264_DMA_IN_SUC_EOF_CH4_INT_CLR_V << H264_DMA_IN_SUC_EOF_CH4_INT_CLR_S)
#define H264_DMA_IN_SUC_EOF_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH4_INT_CLR_S  1
/** H264_DMA_IN_ERR_EOF_CH4_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_ERR_EOF_CH4_INT_CLR    (BIT(2))
#define H264_DMA_IN_ERR_EOF_CH4_INT_CLR_M  (H264_DMA_IN_ERR_EOF_CH4_INT_CLR_V << H264_DMA_IN_ERR_EOF_CH4_INT_CLR_S)
#define H264_DMA_IN_ERR_EOF_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_IN_ERR_EOF_CH4_INT_CLR_S  2
/** H264_DMA_IN_DSCR_ERR_CH4_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_ERR_CH4_INT_CLR    (BIT(3))
#define H264_DMA_IN_DSCR_ERR_CH4_INT_CLR_M  (H264_DMA_IN_DSCR_ERR_CH4_INT_CLR_V << H264_DMA_IN_DSCR_ERR_CH4_INT_CLR_S)
#define H264_DMA_IN_DSCR_ERR_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_ERR_CH4_INT_CLR_S  3
/** H264_DMA_INFIFO_OVF_L1_CH4_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_CLR    (BIT(4))
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_CLR_M  (H264_DMA_INFIFO_OVF_L1_CH4_INT_CLR_V << H264_DMA_INFIFO_OVF_L1_CH4_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH4_INT_CLR_S  4
/** H264_DMA_INFIFO_UDF_L1_CH4_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_CLR    (BIT(5))
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_CLR_M  (H264_DMA_INFIFO_UDF_L1_CH4_INT_CLR_V << H264_DMA_INFIFO_UDF_L1_CH4_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH4_INT_CLR_S  5
/** H264_DMA_INFIFO_OVF_L2_CH4_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_CLR    (BIT(6))
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_CLR_M  (H264_DMA_INFIFO_OVF_L2_CH4_INT_CLR_V << H264_DMA_INFIFO_OVF_L2_CH4_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L2_CH4_INT_CLR_S  6
/** H264_DMA_INFIFO_UDF_L2_CH4_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_CLR    (BIT(7))
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_CLR_M  (H264_DMA_INFIFO_UDF_L2_CH4_INT_CLR_V << H264_DMA_INFIFO_UDF_L2_CH4_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L2_CH4_INT_CLR_S  7
/** H264_DMA_IN_DSCR_EMPTY_CH4_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_CLR    (BIT(8))
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_CLR_M  (H264_DMA_IN_DSCR_EMPTY_CH4_INT_CLR_V << H264_DMA_IN_DSCR_EMPTY_CH4_INT_CLR_S)
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_EMPTY_CH4_INT_CLR_S  8
/** H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_CLR    (BIT(9))
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_CLR_M  (H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_CLR_V << H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_CLR_S)
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DSCR_TASK_OVF_CH4_INT_CLR_S  9

/** H264_DMA_INFIFO_STATUS_CH4_REG register
 *  RX CH4 INFIFO status register
 */
#define H264_DMA_INFIFO_STATUS_CH4_REG (DR_REG_H264_DMA_BASE + 0x914)
/** H264_DMA_INFIFO_FULL_L2_CH4 : RO; bitpos: [0]; default: 0;
 *  Rx FIFO full signal for Rx channel.
 */
#define H264_DMA_INFIFO_FULL_L2_CH4    (BIT(0))
#define H264_DMA_INFIFO_FULL_L2_CH4_M  (H264_DMA_INFIFO_FULL_L2_CH4_V << H264_DMA_INFIFO_FULL_L2_CH4_S)
#define H264_DMA_INFIFO_FULL_L2_CH4_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L2_CH4_S  0
/** H264_DMA_INFIFO_EMPTY_L2_CH4 : RO; bitpos: [1]; default: 1;
 *  Rx FIFO empty signal for Rx channel.
 */
#define H264_DMA_INFIFO_EMPTY_L2_CH4    (BIT(1))
#define H264_DMA_INFIFO_EMPTY_L2_CH4_M  (H264_DMA_INFIFO_EMPTY_L2_CH4_V << H264_DMA_INFIFO_EMPTY_L2_CH4_S)
#define H264_DMA_INFIFO_EMPTY_L2_CH4_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L2_CH4_S  1
/** H264_DMA_INFIFO_CNT_L2_CH4 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Rx FIFO for Rx channel.
 */
#define H264_DMA_INFIFO_CNT_L2_CH4    0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH4_M  (H264_DMA_INFIFO_CNT_L2_CH4_V << H264_DMA_INFIFO_CNT_L2_CH4_S)
#define H264_DMA_INFIFO_CNT_L2_CH4_V  0x0000000FU
#define H264_DMA_INFIFO_CNT_L2_CH4_S  2
/** H264_DMA_INFIFO_FULL_L1_CH4 : RO; bitpos: [6]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L1_CH4    (BIT(6))
#define H264_DMA_INFIFO_FULL_L1_CH4_M  (H264_DMA_INFIFO_FULL_L1_CH4_V << H264_DMA_INFIFO_FULL_L1_CH4_S)
#define H264_DMA_INFIFO_FULL_L1_CH4_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L1_CH4_S  6
/** H264_DMA_INFIFO_EMPTY_L1_CH4 : RO; bitpos: [7]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L1_CH4    (BIT(7))
#define H264_DMA_INFIFO_EMPTY_L1_CH4_M  (H264_DMA_INFIFO_EMPTY_L1_CH4_V << H264_DMA_INFIFO_EMPTY_L1_CH4_S)
#define H264_DMA_INFIFO_EMPTY_L1_CH4_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L1_CH4_S  7
/** H264_DMA_INFIFO_CNT_L1_CH4 : RO; bitpos: [12:8]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L1_CH4    0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH4_M  (H264_DMA_INFIFO_CNT_L1_CH4_V << H264_DMA_INFIFO_CNT_L1_CH4_S)
#define H264_DMA_INFIFO_CNT_L1_CH4_V  0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH4_S  8
/** H264_DMA_INFIFO_FULL_L3_CH4 : RO; bitpos: [16]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L3_CH4    (BIT(16))
#define H264_DMA_INFIFO_FULL_L3_CH4_M  (H264_DMA_INFIFO_FULL_L3_CH4_V << H264_DMA_INFIFO_FULL_L3_CH4_S)
#define H264_DMA_INFIFO_FULL_L3_CH4_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L3_CH4_S  16
/** H264_DMA_INFIFO_EMPTY_L3_CH4 : RO; bitpos: [17]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L3_CH4    (BIT(17))
#define H264_DMA_INFIFO_EMPTY_L3_CH4_M  (H264_DMA_INFIFO_EMPTY_L3_CH4_V << H264_DMA_INFIFO_EMPTY_L3_CH4_S)
#define H264_DMA_INFIFO_EMPTY_L3_CH4_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L3_CH4_S  17
/** H264_DMA_INFIFO_CNT_L3_CH4 : RO; bitpos: [19:18]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L3_CH4    0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH4_M  (H264_DMA_INFIFO_CNT_L3_CH4_V << H264_DMA_INFIFO_CNT_L3_CH4_S)
#define H264_DMA_INFIFO_CNT_L3_CH4_V  0x00000003U
#define H264_DMA_INFIFO_CNT_L3_CH4_S  18

/** H264_DMA_IN_POP_CH4_REG register
 *  RX CH4 INFIFO pop register
 */
#define H264_DMA_IN_POP_CH4_REG (DR_REG_H264_DMA_BASE + 0x918)
/** H264_DMA_INFIFO_RDATA_CH4 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_RDATA_CH4    0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH4_M  (H264_DMA_INFIFO_RDATA_CH4_V << H264_DMA_INFIFO_RDATA_CH4_S)
#define H264_DMA_INFIFO_RDATA_CH4_V  0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH4_S  0
/** H264_DMA_INFIFO_POP_CH4 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_POP_CH4    (BIT(11))
#define H264_DMA_INFIFO_POP_CH4_M  (H264_DMA_INFIFO_POP_CH4_V << H264_DMA_INFIFO_POP_CH4_S)
#define H264_DMA_INFIFO_POP_CH4_V  0x00000001U
#define H264_DMA_INFIFO_POP_CH4_S  11

/** H264_DMA_IN_LINK_CONF_CH4_REG register
 *  RX CH4 in_link dscr ctrl register
 */
#define H264_DMA_IN_LINK_CONF_CH4_REG (DR_REG_H264_DMA_BASE + 0x91c)
/** H264_DMA_INLINK_AUTO_RET_CH4 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_INLINK_AUTO_RET_CH4    (BIT(20))
#define H264_DMA_INLINK_AUTO_RET_CH4_M  (H264_DMA_INLINK_AUTO_RET_CH4_V << H264_DMA_INLINK_AUTO_RET_CH4_S)
#define H264_DMA_INLINK_AUTO_RET_CH4_V  0x00000001U
#define H264_DMA_INLINK_AUTO_RET_CH4_S  20
/** H264_DMA_INLINK_STOP_CH4 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_STOP_CH4    (BIT(21))
#define H264_DMA_INLINK_STOP_CH4_M  (H264_DMA_INLINK_STOP_CH4_V << H264_DMA_INLINK_STOP_CH4_S)
#define H264_DMA_INLINK_STOP_CH4_V  0x00000001U
#define H264_DMA_INLINK_STOP_CH4_S  21
/** H264_DMA_INLINK_START_CH4 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define H264_DMA_INLINK_START_CH4    (BIT(22))
#define H264_DMA_INLINK_START_CH4_M  (H264_DMA_INLINK_START_CH4_V << H264_DMA_INLINK_START_CH4_S)
#define H264_DMA_INLINK_START_CH4_V  0x00000001U
#define H264_DMA_INLINK_START_CH4_S  22
/** H264_DMA_INLINK_RESTART_CH4 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define H264_DMA_INLINK_RESTART_CH4    (BIT(23))
#define H264_DMA_INLINK_RESTART_CH4_M  (H264_DMA_INLINK_RESTART_CH4_V << H264_DMA_INLINK_RESTART_CH4_S)
#define H264_DMA_INLINK_RESTART_CH4_V  0x00000001U
#define H264_DMA_INLINK_RESTART_CH4_S  23
/** H264_DMA_INLINK_PARK_CH4 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define H264_DMA_INLINK_PARK_CH4    (BIT(24))
#define H264_DMA_INLINK_PARK_CH4_M  (H264_DMA_INLINK_PARK_CH4_V << H264_DMA_INLINK_PARK_CH4_S)
#define H264_DMA_INLINK_PARK_CH4_V  0x00000001U
#define H264_DMA_INLINK_PARK_CH4_S  24

/** H264_DMA_IN_LINK_ADDR_CH4_REG register
 *  RX CH4 in_link dscr addr register
 */
#define H264_DMA_IN_LINK_ADDR_CH4_REG (DR_REG_H264_DMA_BASE + 0x920)
/** H264_DMA_INLINK_ADDR_CH4 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first inlink descriptor's address.
 */
#define H264_DMA_INLINK_ADDR_CH4    0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH4_M  (H264_DMA_INLINK_ADDR_CH4_V << H264_DMA_INLINK_ADDR_CH4_S)
#define H264_DMA_INLINK_ADDR_CH4_V  0xFFFFFFFFU
#define H264_DMA_INLINK_ADDR_CH4_S  0

/** H264_DMA_IN_STATE_CH4_REG register
 *  RX CH4 state register
 */
#define H264_DMA_IN_STATE_CH4_REG (DR_REG_H264_DMA_BASE + 0x924)
/** H264_DMA_INLINK_DSCR_ADDR_CH4 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define H264_DMA_INLINK_DSCR_ADDR_CH4    0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH4_M  (H264_DMA_INLINK_DSCR_ADDR_CH4_V << H264_DMA_INLINK_DSCR_ADDR_CH4_S)
#define H264_DMA_INLINK_DSCR_ADDR_CH4_V  0x0003FFFFU
#define H264_DMA_INLINK_DSCR_ADDR_CH4_S  0
/** H264_DMA_IN_DSCR_STATE_CH4 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define H264_DMA_IN_DSCR_STATE_CH4    0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH4_M  (H264_DMA_IN_DSCR_STATE_CH4_V << H264_DMA_IN_DSCR_STATE_CH4_S)
#define H264_DMA_IN_DSCR_STATE_CH4_V  0x00000003U
#define H264_DMA_IN_DSCR_STATE_CH4_S  18
/** H264_DMA_IN_STATE_CH4 : RO; bitpos: [22:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_IN_STATE_CH4    0x00000007U
#define H264_DMA_IN_STATE_CH4_M  (H264_DMA_IN_STATE_CH4_V << H264_DMA_IN_STATE_CH4_S)
#define H264_DMA_IN_STATE_CH4_V  0x00000007U
#define H264_DMA_IN_STATE_CH4_S  20
/** H264_DMA_IN_RESET_AVAIL_CH4 : RO; bitpos: [23]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_IN_RESET_AVAIL_CH4    (BIT(23))
#define H264_DMA_IN_RESET_AVAIL_CH4_M  (H264_DMA_IN_RESET_AVAIL_CH4_V << H264_DMA_IN_RESET_AVAIL_CH4_S)
#define H264_DMA_IN_RESET_AVAIL_CH4_V  0x00000001U
#define H264_DMA_IN_RESET_AVAIL_CH4_S  23

/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH4_REG register
 *  RX CH4 eof des addr register
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH4_REG (DR_REG_H264_DMA_BASE + 0x928)
/** H264_DMA_IN_SUC_EOF_DES_ADDR_CH4 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH4    0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH4_M  (H264_DMA_IN_SUC_EOF_DES_ADDR_CH4_V << H264_DMA_IN_SUC_EOF_DES_ADDR_CH4_S)
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH4_V  0xFFFFFFFFU
#define H264_DMA_IN_SUC_EOF_DES_ADDR_CH4_S  0

/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH4_REG register
 *  RX CH4 err eof des addr register
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH4_REG (DR_REG_H264_DMA_BASE + 0x92c)
/** H264_DMA_IN_ERR_EOF_DES_ADDR_CH4 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data.
 */
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH4    0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH4_M  (H264_DMA_IN_ERR_EOF_DES_ADDR_CH4_V << H264_DMA_IN_ERR_EOF_DES_ADDR_CH4_S)
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH4_V  0xFFFFFFFFU
#define H264_DMA_IN_ERR_EOF_DES_ADDR_CH4_S  0

/** H264_DMA_IN_DSCR_CH4_REG register
 *  RX CH4 next dscr addr register
 */
#define H264_DMA_IN_DSCR_CH4_REG (DR_REG_H264_DMA_BASE + 0x930)
/** H264_DMA_INLINK_DSCR_CH4 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next inlink descriptor address x.
 */
#define H264_DMA_INLINK_DSCR_CH4    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH4_M  (H264_DMA_INLINK_DSCR_CH4_V << H264_DMA_INLINK_DSCR_CH4_S)
#define H264_DMA_INLINK_DSCR_CH4_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_CH4_S  0

/** H264_DMA_IN_DSCR_BF0_CH4_REG register
 *  RX CH4 last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF0_CH4_REG (DR_REG_H264_DMA_BASE + 0x934)
/** H264_DMA_INLINK_DSCR_BF0_CH4 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor's next address x-1.
 */
#define H264_DMA_INLINK_DSCR_BF0_CH4    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH4_M  (H264_DMA_INLINK_DSCR_BF0_CH4_V << H264_DMA_INLINK_DSCR_BF0_CH4_S)
#define H264_DMA_INLINK_DSCR_BF0_CH4_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF0_CH4_S  0

/** H264_DMA_IN_DSCR_BF1_CH4_REG register
 *  RX CH4 second-to-last dscr addr register
 */
#define H264_DMA_IN_DSCR_BF1_CH4_REG (DR_REG_H264_DMA_BASE + 0x938)
/** H264_DMA_INLINK_DSCR_BF1_CH4 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor's next address x-2.
 */
#define H264_DMA_INLINK_DSCR_BF1_CH4    0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH4_M  (H264_DMA_INLINK_DSCR_BF1_CH4_V << H264_DMA_INLINK_DSCR_BF1_CH4_S)
#define H264_DMA_INLINK_DSCR_BF1_CH4_V  0xFFFFFFFFU
#define H264_DMA_INLINK_DSCR_BF1_CH4_S  0

/** H264_DMA_IN_ARB_CH4_REG register
 *  RX CH4 arb register
 */
#define H264_DMA_IN_ARB_CH4_REG (DR_REG_H264_DMA_BASE + 0x940)
/** H264_DMA_IN_ARB_TOKEN_NUM_CH4 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_IN_ARB_TOKEN_NUM_CH4    0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH4_M  (H264_DMA_IN_ARB_TOKEN_NUM_CH4_V << H264_DMA_IN_ARB_TOKEN_NUM_CH4_S)
#define H264_DMA_IN_ARB_TOKEN_NUM_CH4_V  0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH4_S  0
/** H264_DMA_EXTER_IN_ARB_PRIORITY_CH4 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH4    0x00000003U
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH4_M  (H264_DMA_EXTER_IN_ARB_PRIORITY_CH4_V << H264_DMA_EXTER_IN_ARB_PRIORITY_CH4_S)
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH4_V  0x00000003U
#define H264_DMA_EXTER_IN_ARB_PRIORITY_CH4_S  4
/** H264_DMA_INTER_IN_ARB_PRIORITY_CH4 : R/W; bitpos: [8:6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH4    0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH4_M  (H264_DMA_INTER_IN_ARB_PRIORITY_CH4_V << H264_DMA_INTER_IN_ARB_PRIORITY_CH4_S)
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH4_V  0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH4_S  6

/** H264_DMA_IN_ETM_CONF_CH4_REG register
 *  RX CH4 ETM config register
 */
#define H264_DMA_IN_ETM_CONF_CH4_REG (DR_REG_H264_DMA_BASE + 0x948)
/** H264_DMA_IN_ETM_EN_CH4 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to 1 to enable ETM task function
 */
#define H264_DMA_IN_ETM_EN_CH4    (BIT(0))
#define H264_DMA_IN_ETM_EN_CH4_M  (H264_DMA_IN_ETM_EN_CH4_V << H264_DMA_IN_ETM_EN_CH4_S)
#define H264_DMA_IN_ETM_EN_CH4_V  0x00000001U
#define H264_DMA_IN_ETM_EN_CH4_S  0
/** H264_DMA_IN_ETM_LOOP_EN_CH4 : R/W; bitpos: [1]; default: 0;
 *  when this bit is 1, dscr can be processed after receiving a task
 */
#define H264_DMA_IN_ETM_LOOP_EN_CH4    (BIT(1))
#define H264_DMA_IN_ETM_LOOP_EN_CH4_M  (H264_DMA_IN_ETM_LOOP_EN_CH4_V << H264_DMA_IN_ETM_LOOP_EN_CH4_S)
#define H264_DMA_IN_ETM_LOOP_EN_CH4_V  0x00000001U
#define H264_DMA_IN_ETM_LOOP_EN_CH4_S  1
/** H264_DMA_IN_DSCR_TASK_MAK_CH4 : R/W; bitpos: [3:2]; default: 1;
 *  ETM dscr_ready maximum cache numbers
 */
#define H264_DMA_IN_DSCR_TASK_MAK_CH4    0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH4_M  (H264_DMA_IN_DSCR_TASK_MAK_CH4_V << H264_DMA_IN_DSCR_TASK_MAK_CH4_S)
#define H264_DMA_IN_DSCR_TASK_MAK_CH4_V  0x00000003U
#define H264_DMA_IN_DSCR_TASK_MAK_CH4_S  2

/** H264_DMA_IN_FIFO_CNT_CH4_REG register
 *  rx CH4 fifo cnt register
 */
#define H264_DMA_IN_FIFO_CNT_CH4_REG (DR_REG_H264_DMA_BASE + 0x980)
/** H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH4 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH4    0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH4_M  (H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH4_V << H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH4_S)
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH4_V  0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH4_S  0

/** H264_DMA_IN_POP_DATA_CNT_CH4_REG register
 *  rx CH4 pop data cnt register
 */
#define H264_DMA_IN_POP_DATA_CNT_CH4_REG (DR_REG_H264_DMA_BASE + 0x984)
/** H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH4 : RO; bitpos: [7:0]; default: 7;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH4    0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH4_M  (H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH4_V << H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH4_S)
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH4_V  0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH4_S  0

/** H264_DMA_IN_XADDR_CH4_REG register
 *  rx CH4 xaddr register
 */
#define H264_DMA_IN_XADDR_CH4_REG (DR_REG_H264_DMA_BASE + 0x988)
/** H264_DMA_IN_CMDFIFO_XADDR_CH4 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_XADDR_CH4    0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH4_M  (H264_DMA_IN_CMDFIFO_XADDR_CH4_V << H264_DMA_IN_CMDFIFO_XADDR_CH4_S)
#define H264_DMA_IN_CMDFIFO_XADDR_CH4_V  0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH4_S  0

/** H264_DMA_IN_BUF_HB_RCV_CH4_REG register
 *  rx CH4 buf len hb rcv register
 */
#define H264_DMA_IN_BUF_HB_RCV_CH4_REG (DR_REG_H264_DMA_BASE + 0x98c)
/** H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH4 : RO; bitpos: [28:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH4    0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH4_M  (H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH4_V << H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH4_S)
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH4_V  0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH4_S  0

/** H264_DMA_IN_CONF0_CH5_REG register
 *  RX CH5 config0 register
 */
#define H264_DMA_IN_CONF0_CH5_REG (DR_REG_H264_DMA_BASE + 0xa00)
/** H264_DMA_IN_ECC_AES_EN_CH5 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define H264_DMA_IN_ECC_AES_EN_CH5    (BIT(3))
#define H264_DMA_IN_ECC_AES_EN_CH5_M  (H264_DMA_IN_ECC_AES_EN_CH5_V << H264_DMA_IN_ECC_AES_EN_CH5_S)
#define H264_DMA_IN_ECC_AES_EN_CH5_V  0x00000001U
#define H264_DMA_IN_ECC_AES_EN_CH5_S  3
/** H264_DMA_IN_MEM_BURST_LENGTH_CH5 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 1. 0: single      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define H264_DMA_IN_MEM_BURST_LENGTH_CH5    0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH5_M  (H264_DMA_IN_MEM_BURST_LENGTH_CH5_V << H264_DMA_IN_MEM_BURST_LENGTH_CH5_S)
#define H264_DMA_IN_MEM_BURST_LENGTH_CH5_V  0x00000007U
#define H264_DMA_IN_MEM_BURST_LENGTH_CH5_S  6
/** H264_DMA_IN_PAGE_BOUND_EN_CH5 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define H264_DMA_IN_PAGE_BOUND_EN_CH5    (BIT(12))
#define H264_DMA_IN_PAGE_BOUND_EN_CH5_M  (H264_DMA_IN_PAGE_BOUND_EN_CH5_V << H264_DMA_IN_PAGE_BOUND_EN_CH5_S)
#define H264_DMA_IN_PAGE_BOUND_EN_CH5_V  0x00000001U
#define H264_DMA_IN_PAGE_BOUND_EN_CH5_S  12
/** H264_DMA_IN_RST_CH5 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define H264_DMA_IN_RST_CH5    (BIT(24))
#define H264_DMA_IN_RST_CH5_M  (H264_DMA_IN_RST_CH5_V << H264_DMA_IN_RST_CH5_S)
#define H264_DMA_IN_RST_CH5_V  0x00000001U
#define H264_DMA_IN_RST_CH5_S  24
/** H264_DMA_IN_CMD_DISABLE_CH5 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define H264_DMA_IN_CMD_DISABLE_CH5    (BIT(25))
#define H264_DMA_IN_CMD_DISABLE_CH5_M  (H264_DMA_IN_CMD_DISABLE_CH5_V << H264_DMA_IN_CMD_DISABLE_CH5_S)
#define H264_DMA_IN_CMD_DISABLE_CH5_V  0x00000001U
#define H264_DMA_IN_CMD_DISABLE_CH5_S  25

/** H264_DMA_IN_CONF1_CH5_REG register
 *  RX CH5 config1 register
 */
#define H264_DMA_IN_CONF1_CH5_REG (DR_REG_H264_DMA_BASE + 0xa04)
/** H264_DMA_BLOCK_START_ADDR_CH5 : R/W; bitpos: [31:0]; default: 0;
 *  RX Channel 5 destination start address
 */
#define H264_DMA_BLOCK_START_ADDR_CH5    0xFFFFFFFFU
#define H264_DMA_BLOCK_START_ADDR_CH5_M  (H264_DMA_BLOCK_START_ADDR_CH5_V << H264_DMA_BLOCK_START_ADDR_CH5_S)
#define H264_DMA_BLOCK_START_ADDR_CH5_V  0xFFFFFFFFU
#define H264_DMA_BLOCK_START_ADDR_CH5_S  0

/** H264_DMA_IN_CONF2_CH5_REG register
 *  RX CH5 config2 register
 */
#define H264_DMA_IN_CONF2_CH5_REG (DR_REG_H264_DMA_BASE + 0xa08)
/** H264_DMA_BLOCK_ROW_LENGTH_12LINE_CH5 : R/W; bitpos: [15:0]; default: 30720;
 *  The number of bytes contained in a row block 12line in RX channel 5
 */
#define H264_DMA_BLOCK_ROW_LENGTH_12LINE_CH5    0x0000FFFFU
#define H264_DMA_BLOCK_ROW_LENGTH_12LINE_CH5_M  (H264_DMA_BLOCK_ROW_LENGTH_12LINE_CH5_V << H264_DMA_BLOCK_ROW_LENGTH_12LINE_CH5_S)
#define H264_DMA_BLOCK_ROW_LENGTH_12LINE_CH5_V  0x0000FFFFU
#define H264_DMA_BLOCK_ROW_LENGTH_12LINE_CH5_S  0
/** H264_DMA_BLOCK_ROW_LENGTH_4LINE_CH5 : R/W; bitpos: [31:16]; default: 15360;
 *  The number of bytes contained in a row block 4line in RX channel 5
 */
#define H264_DMA_BLOCK_ROW_LENGTH_4LINE_CH5    0x0000FFFFU
#define H264_DMA_BLOCK_ROW_LENGTH_4LINE_CH5_M  (H264_DMA_BLOCK_ROW_LENGTH_4LINE_CH5_V << H264_DMA_BLOCK_ROW_LENGTH_4LINE_CH5_S)
#define H264_DMA_BLOCK_ROW_LENGTH_4LINE_CH5_V  0x0000FFFFU
#define H264_DMA_BLOCK_ROW_LENGTH_4LINE_CH5_S  16

/** H264_DMA_IN_CONF3_CH5_REG register
 *  RX CH5 config3 register
 */
#define H264_DMA_IN_CONF3_CH5_REG (DR_REG_H264_DMA_BASE + 0xa0c)
/** H264_DMA_BLOCK_LENGTH_12LINE_CH5 : R/W; bitpos: [13:0]; default: 256;
 *  The number of bytes contained in a block 12line
 */
#define H264_DMA_BLOCK_LENGTH_12LINE_CH5    0x00003FFFU
#define H264_DMA_BLOCK_LENGTH_12LINE_CH5_M  (H264_DMA_BLOCK_LENGTH_12LINE_CH5_V << H264_DMA_BLOCK_LENGTH_12LINE_CH5_S)
#define H264_DMA_BLOCK_LENGTH_12LINE_CH5_V  0x00003FFFU
#define H264_DMA_BLOCK_LENGTH_12LINE_CH5_S  0
/** H264_DMA_BLOCK_LENGTH_4LINE_CH5 : R/W; bitpos: [27:14]; default: 128;
 *  The number of bytes contained in a block 4line
 */
#define H264_DMA_BLOCK_LENGTH_4LINE_CH5    0x00003FFFU
#define H264_DMA_BLOCK_LENGTH_4LINE_CH5_M  (H264_DMA_BLOCK_LENGTH_4LINE_CH5_V << H264_DMA_BLOCK_LENGTH_4LINE_CH5_S)
#define H264_DMA_BLOCK_LENGTH_4LINE_CH5_V  0x00003FFFU
#define H264_DMA_BLOCK_LENGTH_4LINE_CH5_S  14

/** H264_DMA_IN_INT_RAW_CH5_REG register
 *  RX CH5  interrupt raw register
 */
#define H264_DMA_IN_INT_RAW_CH5_REG (DR_REG_H264_DMA_BASE + 0xa10)
/** H264_DMA_IN_DONE_CH5_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 1.
 */
#define H264_DMA_IN_DONE_CH5_INT_RAW    (BIT(0))
#define H264_DMA_IN_DONE_CH5_INT_RAW_M  (H264_DMA_IN_DONE_CH5_INT_RAW_V << H264_DMA_IN_DONE_CH5_INT_RAW_S)
#define H264_DMA_IN_DONE_CH5_INT_RAW_V  0x00000001U
#define H264_DMA_IN_DONE_CH5_INT_RAW_S  0
/** H264_DMA_IN_SUC_EOF_CH5_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 1.
 */
#define H264_DMA_IN_SUC_EOF_CH5_INT_RAW    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH5_INT_RAW_M  (H264_DMA_IN_SUC_EOF_CH5_INT_RAW_V << H264_DMA_IN_SUC_EOF_CH5_INT_RAW_S)
#define H264_DMA_IN_SUC_EOF_CH5_INT_RAW_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH5_INT_RAW_S  1
/** H264_DMA_INFIFO_OVF_L1_CH5_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_RAW    (BIT(2))
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_RAW_M  (H264_DMA_INFIFO_OVF_L1_CH5_INT_RAW_V << H264_DMA_INFIFO_OVF_L1_CH5_INT_RAW_S)
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_RAW_S  2
/** H264_DMA_INFIFO_UDF_L1_CH5_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_RAW    (BIT(3))
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_RAW_M  (H264_DMA_INFIFO_UDF_L1_CH5_INT_RAW_V << H264_DMA_INFIFO_UDF_L1_CH5_INT_RAW_S)
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_RAW_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_RAW_S  3
/** H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_RAW    (BIT(4))
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_RAW_M  (H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_RAW_V << H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_RAW_S)
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_RAW_V  0x00000001U
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_RAW_S  4

/** H264_DMA_IN_INT_ENA_CH5_REG register
 *  RX CH5  interrupt ena register
 */
#define H264_DMA_IN_INT_ENA_CH5_REG (DR_REG_H264_DMA_BASE + 0xa14)
/** H264_DMA_IN_DONE_CH5_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH5_INT_ENA    (BIT(0))
#define H264_DMA_IN_DONE_CH5_INT_ENA_M  (H264_DMA_IN_DONE_CH5_INT_ENA_V << H264_DMA_IN_DONE_CH5_INT_ENA_S)
#define H264_DMA_IN_DONE_CH5_INT_ENA_V  0x00000001U
#define H264_DMA_IN_DONE_CH5_INT_ENA_S  0
/** H264_DMA_IN_SUC_EOF_CH5_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH5_INT_ENA    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH5_INT_ENA_M  (H264_DMA_IN_SUC_EOF_CH5_INT_ENA_V << H264_DMA_IN_SUC_EOF_CH5_INT_ENA_S)
#define H264_DMA_IN_SUC_EOF_CH5_INT_ENA_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH5_INT_ENA_S  1
/** H264_DMA_INFIFO_OVF_L1_CH5_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ENA    (BIT(2))
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ENA_M  (H264_DMA_INFIFO_OVF_L1_CH5_INT_ENA_V << H264_DMA_INFIFO_OVF_L1_CH5_INT_ENA_S)
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ENA_S  2
/** H264_DMA_INFIFO_UDF_L1_CH5_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ENA    (BIT(3))
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ENA_M  (H264_DMA_INFIFO_UDF_L1_CH5_INT_ENA_V << H264_DMA_INFIFO_UDF_L1_CH5_INT_ENA_S)
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ENA_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ENA_S  3
/** H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ENA    (BIT(4))
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ENA_M  (H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ENA_V << H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ENA_S)
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ENA_V  0x00000001U
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ENA_S  4

/** H264_DMA_IN_INT_ST_CH5_REG register
 *  RX CH5  interrupt st register
 */
#define H264_DMA_IN_INT_ST_CH5_REG (DR_REG_H264_DMA_BASE + 0xa18)
/** H264_DMA_IN_DONE_CH5_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH5_INT_ST    (BIT(0))
#define H264_DMA_IN_DONE_CH5_INT_ST_M  (H264_DMA_IN_DONE_CH5_INT_ST_V << H264_DMA_IN_DONE_CH5_INT_ST_S)
#define H264_DMA_IN_DONE_CH5_INT_ST_V  0x00000001U
#define H264_DMA_IN_DONE_CH5_INT_ST_S  0
/** H264_DMA_IN_SUC_EOF_CH5_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH5_INT_ST    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH5_INT_ST_M  (H264_DMA_IN_SUC_EOF_CH5_INT_ST_V << H264_DMA_IN_SUC_EOF_CH5_INT_ST_S)
#define H264_DMA_IN_SUC_EOF_CH5_INT_ST_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH5_INT_ST_S  1
/** H264_DMA_INFIFO_OVF_L1_CH5_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ST    (BIT(2))
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ST_M  (H264_DMA_INFIFO_OVF_L1_CH5_INT_ST_V << H264_DMA_INFIFO_OVF_L1_CH5_INT_ST_S)
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_ST_S  2
/** H264_DMA_INFIFO_UDF_L1_CH5_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ST    (BIT(3))
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ST_M  (H264_DMA_INFIFO_UDF_L1_CH5_INT_ST_V << H264_DMA_INFIFO_UDF_L1_CH5_INT_ST_S)
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ST_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_ST_S  3
/** H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ST    (BIT(4))
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ST_M  (H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ST_V << H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ST_S)
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ST_V  0x00000001U
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_ST_S  4

/** H264_DMA_IN_INT_CLR_CH5_REG register
 *  RX CH5  interrupt clr register
 */
#define H264_DMA_IN_INT_CLR_CH5_REG (DR_REG_H264_DMA_BASE + 0xa1c)
/** H264_DMA_IN_DONE_CH5_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define H264_DMA_IN_DONE_CH5_INT_CLR    (BIT(0))
#define H264_DMA_IN_DONE_CH5_INT_CLR_M  (H264_DMA_IN_DONE_CH5_INT_CLR_V << H264_DMA_IN_DONE_CH5_INT_CLR_S)
#define H264_DMA_IN_DONE_CH5_INT_CLR_V  0x00000001U
#define H264_DMA_IN_DONE_CH5_INT_CLR_S  0
/** H264_DMA_IN_SUC_EOF_CH5_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define H264_DMA_IN_SUC_EOF_CH5_INT_CLR    (BIT(1))
#define H264_DMA_IN_SUC_EOF_CH5_INT_CLR_M  (H264_DMA_IN_SUC_EOF_CH5_INT_CLR_V << H264_DMA_IN_SUC_EOF_CH5_INT_CLR_S)
#define H264_DMA_IN_SUC_EOF_CH5_INT_CLR_V  0x00000001U
#define H264_DMA_IN_SUC_EOF_CH5_INT_CLR_S  1
/** H264_DMA_INFIFO_OVF_L1_CH5_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_CLR    (BIT(2))
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_CLR_M  (H264_DMA_INFIFO_OVF_L1_CH5_INT_CLR_V << H264_DMA_INFIFO_OVF_L1_CH5_INT_CLR_S)
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_OVF_L1_CH5_INT_CLR_S  2
/** H264_DMA_INFIFO_UDF_L1_CH5_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_CLR    (BIT(3))
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_CLR_M  (H264_DMA_INFIFO_UDF_L1_CH5_INT_CLR_V << H264_DMA_INFIFO_UDF_L1_CH5_INT_CLR_S)
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_CLR_V  0x00000001U
#define H264_DMA_INFIFO_UDF_L1_CH5_INT_CLR_S  3
/** H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_CLR    (BIT(4))
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_CLR_M  (H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_CLR_V << H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_CLR_S)
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_CLR_V  0x00000001U
#define H264_DMA_FETCH_MB_COL_CNT_OVF_CH5_INT_CLR_S  4

/** H264_DMA_INFIFO_STATUS_CH5_REG register
 *  RX CH5 INFIFO status register
 */
#define H264_DMA_INFIFO_STATUS_CH5_REG (DR_REG_H264_DMA_BASE + 0xa20)
/** H264_DMA_INFIFO_FULL_L1_CH5 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_FULL_L1_CH5    (BIT(0))
#define H264_DMA_INFIFO_FULL_L1_CH5_M  (H264_DMA_INFIFO_FULL_L1_CH5_V << H264_DMA_INFIFO_FULL_L1_CH5_S)
#define H264_DMA_INFIFO_FULL_L1_CH5_V  0x00000001U
#define H264_DMA_INFIFO_FULL_L1_CH5_S  0
/** H264_DMA_INFIFO_EMPTY_L1_CH5 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 1.
 */
#define H264_DMA_INFIFO_EMPTY_L1_CH5    (BIT(1))
#define H264_DMA_INFIFO_EMPTY_L1_CH5_M  (H264_DMA_INFIFO_EMPTY_L1_CH5_V << H264_DMA_INFIFO_EMPTY_L1_CH5_S)
#define H264_DMA_INFIFO_EMPTY_L1_CH5_V  0x00000001U
#define H264_DMA_INFIFO_EMPTY_L1_CH5_S  1
/** H264_DMA_INFIFO_CNT_L1_CH5 : RO; bitpos: [6:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 1.
 */
#define H264_DMA_INFIFO_CNT_L1_CH5    0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH5_M  (H264_DMA_INFIFO_CNT_L1_CH5_V << H264_DMA_INFIFO_CNT_L1_CH5_S)
#define H264_DMA_INFIFO_CNT_L1_CH5_V  0x0000001FU
#define H264_DMA_INFIFO_CNT_L1_CH5_S  2

/** H264_DMA_IN_POP_CH5_REG register
 *  RX CH5 INFIFO pop register
 */
#define H264_DMA_IN_POP_CH5_REG (DR_REG_H264_DMA_BASE + 0xa24)
/** H264_DMA_INFIFO_RDATA_CH5 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_RDATA_CH5    0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH5_M  (H264_DMA_INFIFO_RDATA_CH5_V << H264_DMA_INFIFO_RDATA_CH5_S)
#define H264_DMA_INFIFO_RDATA_CH5_V  0x000007FFU
#define H264_DMA_INFIFO_RDATA_CH5_S  0
/** H264_DMA_INFIFO_POP_CH5 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define H264_DMA_INFIFO_POP_CH5    (BIT(11))
#define H264_DMA_INFIFO_POP_CH5_M  (H264_DMA_INFIFO_POP_CH5_V << H264_DMA_INFIFO_POP_CH5_S)
#define H264_DMA_INFIFO_POP_CH5_V  0x00000001U
#define H264_DMA_INFIFO_POP_CH5_S  11

/** H264_DMA_IN_STATE_CH5_REG register
 *  RX CH5 state register
 */
#define H264_DMA_IN_STATE_CH5_REG (DR_REG_H264_DMA_BASE + 0xa28)
/** H264_DMA_IN_STATE_CH5 : RO; bitpos: [2:0]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define H264_DMA_IN_STATE_CH5    0x00000007U
#define H264_DMA_IN_STATE_CH5_M  (H264_DMA_IN_STATE_CH5_V << H264_DMA_IN_STATE_CH5_S)
#define H264_DMA_IN_STATE_CH5_V  0x00000007U
#define H264_DMA_IN_STATE_CH5_S  0
/** H264_DMA_IN_RESET_AVAIL_CH5 : RO; bitpos: [3]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define H264_DMA_IN_RESET_AVAIL_CH5    (BIT(3))
#define H264_DMA_IN_RESET_AVAIL_CH5_M  (H264_DMA_IN_RESET_AVAIL_CH5_V << H264_DMA_IN_RESET_AVAIL_CH5_S)
#define H264_DMA_IN_RESET_AVAIL_CH5_V  0x00000001U
#define H264_DMA_IN_RESET_AVAIL_CH5_S  3

/** H264_DMA_IN_ARB_CH5_REG register
 *  RX CH5 arb register
 */
#define H264_DMA_IN_ARB_CH5_REG (DR_REG_H264_DMA_BASE + 0xa40)
/** H264_DMA_IN_ARB_TOKEN_NUM_CH5 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define H264_DMA_IN_ARB_TOKEN_NUM_CH5    0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH5_M  (H264_DMA_IN_ARB_TOKEN_NUM_CH5_V << H264_DMA_IN_ARB_TOKEN_NUM_CH5_S)
#define H264_DMA_IN_ARB_TOKEN_NUM_CH5_V  0x0000000FU
#define H264_DMA_IN_ARB_TOKEN_NUM_CH5_S  0
/** H264_DMA_INTER_IN_ARB_PRIORITY_CH5 : R/W; bitpos: [8:6]; default: 1;
 *  Set the priority of channel
 */
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH5    0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH5_M  (H264_DMA_INTER_IN_ARB_PRIORITY_CH5_V << H264_DMA_INTER_IN_ARB_PRIORITY_CH5_S)
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH5_V  0x00000007U
#define H264_DMA_INTER_IN_ARB_PRIORITY_CH5_S  6

/** H264_DMA_IN_FIFO_CNT_CH5_REG register
 *  rx CH5 fifo cnt register
 */
#define H264_DMA_IN_FIFO_CNT_CH5_REG (DR_REG_H264_DMA_BASE + 0xa80)
/** H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH5 : RO; bitpos: [9:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH5    0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH5_M  (H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH5_V << H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH5_S)
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH5_V  0x000003FFU
#define H264_DMA_IN_CMDFIFO_INFIFO_CNT_CH5_S  0

/** H264_DMA_IN_POP_DATA_CNT_CH5_REG register
 *  rx CH5 pop data cnt register
 */
#define H264_DMA_IN_POP_DATA_CNT_CH5_REG (DR_REG_H264_DMA_BASE + 0xa84)
/** H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH5 : RO; bitpos: [7:0]; default: 255;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH5    0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH5_M  (H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH5_V << H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH5_S)
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH5_V  0x000000FFU
#define H264_DMA_IN_CMDFIFO_POP_DATA_CNT_CH5_S  0

/** H264_DMA_IN_XADDR_CH5_REG register
 *  rx CH5 xaddr register
 */
#define H264_DMA_IN_XADDR_CH5_REG (DR_REG_H264_DMA_BASE + 0xa88)
/** H264_DMA_IN_CMDFIFO_XADDR_CH5 : RO; bitpos: [31:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_XADDR_CH5    0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH5_M  (H264_DMA_IN_CMDFIFO_XADDR_CH5_V << H264_DMA_IN_CMDFIFO_XADDR_CH5_S)
#define H264_DMA_IN_CMDFIFO_XADDR_CH5_V  0xFFFFFFFFU
#define H264_DMA_IN_CMDFIFO_XADDR_CH5_S  0

/** H264_DMA_IN_BUF_HB_RCV_CH5_REG register
 *  rx CH5 buf len hb rcv register
 */
#define H264_DMA_IN_BUF_HB_RCV_CH5_REG (DR_REG_H264_DMA_BASE + 0xa8c)
/** H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH5 : RO; bitpos: [28:0]; default: 0;
 *  only for debug
 */
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH5    0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH5_M  (H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH5_V << H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH5_S)
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH5_V  0x1FFFFFFFU
#define H264_DMA_IN_CMDFIFO_BUF_HB_RCV_CH5_S  0

/** H264_DMA_INTER_AXI_ERR_REG register
 *  inter memory axi err register
 */
#define H264_DMA_INTER_AXI_ERR_REG (DR_REG_H264_DMA_BASE + 0xb00)
/** H264_DMA_INTER_RID_ERR_CNT : RO; bitpos: [3:0]; default: 0;
 *  AXI read id err cnt
 */
#define H264_DMA_INTER_RID_ERR_CNT    0x0000000FU
#define H264_DMA_INTER_RID_ERR_CNT_M  (H264_DMA_INTER_RID_ERR_CNT_V << H264_DMA_INTER_RID_ERR_CNT_S)
#define H264_DMA_INTER_RID_ERR_CNT_V  0x0000000FU
#define H264_DMA_INTER_RID_ERR_CNT_S  0
/** H264_DMA_INTER_RRESP_ERR_CNT : RO; bitpos: [7:4]; default: 0;
 *  AXI read resp err cnt
 */
#define H264_DMA_INTER_RRESP_ERR_CNT    0x0000000FU
#define H264_DMA_INTER_RRESP_ERR_CNT_M  (H264_DMA_INTER_RRESP_ERR_CNT_V << H264_DMA_INTER_RRESP_ERR_CNT_S)
#define H264_DMA_INTER_RRESP_ERR_CNT_V  0x0000000FU
#define H264_DMA_INTER_RRESP_ERR_CNT_S  4
/** H264_DMA_INTER_WRESP_ERR_CNT : RO; bitpos: [11:8]; default: 0;
 *  AXI write resp err cnt
 */
#define H264_DMA_INTER_WRESP_ERR_CNT    0x0000000FU
#define H264_DMA_INTER_WRESP_ERR_CNT_M  (H264_DMA_INTER_WRESP_ERR_CNT_V << H264_DMA_INTER_WRESP_ERR_CNT_S)
#define H264_DMA_INTER_WRESP_ERR_CNT_V  0x0000000FU
#define H264_DMA_INTER_WRESP_ERR_CNT_S  8
/** H264_DMA_INTER_RD_FIFO_CNT : RO; bitpos: [14:12]; default: 0;
 *  AXI read cmd fifo remain cmd count
 */
#define H264_DMA_INTER_RD_FIFO_CNT    0x00000007U
#define H264_DMA_INTER_RD_FIFO_CNT_M  (H264_DMA_INTER_RD_FIFO_CNT_V << H264_DMA_INTER_RD_FIFO_CNT_S)
#define H264_DMA_INTER_RD_FIFO_CNT_V  0x00000007U
#define H264_DMA_INTER_RD_FIFO_CNT_S  12
/** H264_DMA_INTER_RD_BAK_FIFO_CNT : RO; bitpos: [18:15]; default: 0;
 *  AXI read backup cmd fifo remain cmd count
 */
#define H264_DMA_INTER_RD_BAK_FIFO_CNT    0x0000000FU
#define H264_DMA_INTER_RD_BAK_FIFO_CNT_M  (H264_DMA_INTER_RD_BAK_FIFO_CNT_V << H264_DMA_INTER_RD_BAK_FIFO_CNT_S)
#define H264_DMA_INTER_RD_BAK_FIFO_CNT_V  0x0000000FU
#define H264_DMA_INTER_RD_BAK_FIFO_CNT_S  15
/** H264_DMA_INTER_WR_FIFO_CNT : RO; bitpos: [21:19]; default: 0;
 *  AXI write cmd fifo remain cmd count
 */
#define H264_DMA_INTER_WR_FIFO_CNT    0x00000007U
#define H264_DMA_INTER_WR_FIFO_CNT_M  (H264_DMA_INTER_WR_FIFO_CNT_V << H264_DMA_INTER_WR_FIFO_CNT_S)
#define H264_DMA_INTER_WR_FIFO_CNT_V  0x00000007U
#define H264_DMA_INTER_WR_FIFO_CNT_S  19
/** H264_DMA_INTER_WR_BAK_FIFO_CNT : RO; bitpos: [25:22]; default: 0;
 *  AXI write backup cmd fifo remain cmd count
 */
#define H264_DMA_INTER_WR_BAK_FIFO_CNT    0x0000000FU
#define H264_DMA_INTER_WR_BAK_FIFO_CNT_M  (H264_DMA_INTER_WR_BAK_FIFO_CNT_V << H264_DMA_INTER_WR_BAK_FIFO_CNT_S)
#define H264_DMA_INTER_WR_BAK_FIFO_CNT_V  0x0000000FU
#define H264_DMA_INTER_WR_BAK_FIFO_CNT_S  22

/** H264_DMA_EXTER_AXI_ERR_REG register
 *  exter memory axi err register
 */
#define H264_DMA_EXTER_AXI_ERR_REG (DR_REG_H264_DMA_BASE + 0xb04)
/** H264_DMA_EXTER_RID_ERR_CNT : RO; bitpos: [3:0]; default: 0;
 *  AXI read id err cnt
 */
#define H264_DMA_EXTER_RID_ERR_CNT    0x0000000FU
#define H264_DMA_EXTER_RID_ERR_CNT_M  (H264_DMA_EXTER_RID_ERR_CNT_V << H264_DMA_EXTER_RID_ERR_CNT_S)
#define H264_DMA_EXTER_RID_ERR_CNT_V  0x0000000FU
#define H264_DMA_EXTER_RID_ERR_CNT_S  0
/** H264_DMA_EXTER_RRESP_ERR_CNT : RO; bitpos: [7:4]; default: 0;
 *  AXI read resp err cnt
 */
#define H264_DMA_EXTER_RRESP_ERR_CNT    0x0000000FU
#define H264_DMA_EXTER_RRESP_ERR_CNT_M  (H264_DMA_EXTER_RRESP_ERR_CNT_V << H264_DMA_EXTER_RRESP_ERR_CNT_S)
#define H264_DMA_EXTER_RRESP_ERR_CNT_V  0x0000000FU
#define H264_DMA_EXTER_RRESP_ERR_CNT_S  4
/** H264_DMA_EXTER_WRESP_ERR_CNT : RO; bitpos: [11:8]; default: 0;
 *  AXI write resp err cnt
 */
#define H264_DMA_EXTER_WRESP_ERR_CNT    0x0000000FU
#define H264_DMA_EXTER_WRESP_ERR_CNT_M  (H264_DMA_EXTER_WRESP_ERR_CNT_V << H264_DMA_EXTER_WRESP_ERR_CNT_S)
#define H264_DMA_EXTER_WRESP_ERR_CNT_V  0x0000000FU
#define H264_DMA_EXTER_WRESP_ERR_CNT_S  8
/** H264_DMA_EXTER_RD_FIFO_CNT : RO; bitpos: [14:12]; default: 0;
 *  AXI read cmd fifo remain cmd count
 */
#define H264_DMA_EXTER_RD_FIFO_CNT    0x00000007U
#define H264_DMA_EXTER_RD_FIFO_CNT_M  (H264_DMA_EXTER_RD_FIFO_CNT_V << H264_DMA_EXTER_RD_FIFO_CNT_S)
#define H264_DMA_EXTER_RD_FIFO_CNT_V  0x00000007U
#define H264_DMA_EXTER_RD_FIFO_CNT_S  12
/** H264_DMA_EXTER_RD_BAK_FIFO_CNT : RO; bitpos: [18:15]; default: 0;
 *  AXI read backup cmd fifo remain cmd count
 */
#define H264_DMA_EXTER_RD_BAK_FIFO_CNT    0x0000000FU
#define H264_DMA_EXTER_RD_BAK_FIFO_CNT_M  (H264_DMA_EXTER_RD_BAK_FIFO_CNT_V << H264_DMA_EXTER_RD_BAK_FIFO_CNT_S)
#define H264_DMA_EXTER_RD_BAK_FIFO_CNT_V  0x0000000FU
#define H264_DMA_EXTER_RD_BAK_FIFO_CNT_S  15
/** H264_DMA_EXTER_WR_FIFO_CNT : RO; bitpos: [21:19]; default: 0;
 *  AXI write cmd fifo remain cmd count
 */
#define H264_DMA_EXTER_WR_FIFO_CNT    0x00000007U
#define H264_DMA_EXTER_WR_FIFO_CNT_M  (H264_DMA_EXTER_WR_FIFO_CNT_V << H264_DMA_EXTER_WR_FIFO_CNT_S)
#define H264_DMA_EXTER_WR_FIFO_CNT_V  0x00000007U
#define H264_DMA_EXTER_WR_FIFO_CNT_S  19
/** H264_DMA_EXTER_WR_BAK_FIFO_CNT : RO; bitpos: [25:22]; default: 0;
 *  AXI write backup cmd fifo remain cmd count
 */
#define H264_DMA_EXTER_WR_BAK_FIFO_CNT    0x0000000FU
#define H264_DMA_EXTER_WR_BAK_FIFO_CNT_M  (H264_DMA_EXTER_WR_BAK_FIFO_CNT_V << H264_DMA_EXTER_WR_BAK_FIFO_CNT_S)
#define H264_DMA_EXTER_WR_BAK_FIFO_CNT_V  0x0000000FU
#define H264_DMA_EXTER_WR_BAK_FIFO_CNT_S  22

/** H264_DMA_RST_CONF_REG register
 *  axi reset config register
 */
#define H264_DMA_RST_CONF_REG (DR_REG_H264_DMA_BASE + 0xb08)
/** H264_DMA_INTER_AXIM_RD_RST : R/W; bitpos: [0]; default: 0;
 *  Write 1 then write 0  to this bit to reset axi master read data FIFO.
 */
#define H264_DMA_INTER_AXIM_RD_RST    (BIT(0))
#define H264_DMA_INTER_AXIM_RD_RST_M  (H264_DMA_INTER_AXIM_RD_RST_V << H264_DMA_INTER_AXIM_RD_RST_S)
#define H264_DMA_INTER_AXIM_RD_RST_V  0x00000001U
#define H264_DMA_INTER_AXIM_RD_RST_S  0
/** H264_DMA_INTER_AXIM_WR_RST : R/W; bitpos: [1]; default: 0;
 *  Write 1 then write 0  to this bit to reset axi master write data FIFO.
 */
#define H264_DMA_INTER_AXIM_WR_RST    (BIT(1))
#define H264_DMA_INTER_AXIM_WR_RST_M  (H264_DMA_INTER_AXIM_WR_RST_V << H264_DMA_INTER_AXIM_WR_RST_S)
#define H264_DMA_INTER_AXIM_WR_RST_V  0x00000001U
#define H264_DMA_INTER_AXIM_WR_RST_S  1
/** H264_DMA_EXTER_AXIM_RD_RST : R/W; bitpos: [2]; default: 0;
 *  Write 1 then write 0  to this bit to reset axi master read data FIFO.
 */
#define H264_DMA_EXTER_AXIM_RD_RST    (BIT(2))
#define H264_DMA_EXTER_AXIM_RD_RST_M  (H264_DMA_EXTER_AXIM_RD_RST_V << H264_DMA_EXTER_AXIM_RD_RST_S)
#define H264_DMA_EXTER_AXIM_RD_RST_V  0x00000001U
#define H264_DMA_EXTER_AXIM_RD_RST_S  2
/** H264_DMA_EXTER_AXIM_WR_RST : R/W; bitpos: [3]; default: 0;
 *  Write 1 then write 0  to this bit to reset axi master write data FIFO.
 */
#define H264_DMA_EXTER_AXIM_WR_RST    (BIT(3))
#define H264_DMA_EXTER_AXIM_WR_RST_M  (H264_DMA_EXTER_AXIM_WR_RST_V << H264_DMA_EXTER_AXIM_WR_RST_S)
#define H264_DMA_EXTER_AXIM_WR_RST_V  0x00000001U
#define H264_DMA_EXTER_AXIM_WR_RST_S  3
/** H264_DMA_CLK_EN : R/W; bitpos: [4]; default: 0;
 *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
 *  registers.
 */
#define H264_DMA_CLK_EN    (BIT(4))
#define H264_DMA_CLK_EN_M  (H264_DMA_CLK_EN_V << H264_DMA_CLK_EN_S)
#define H264_DMA_CLK_EN_V  0x00000001U
#define H264_DMA_CLK_EN_S  4

/** H264_DMA_INTER_MEM_START_ADDR0_REG register
 *  Start address of inter memory range0 register
 */
#define H264_DMA_INTER_MEM_START_ADDR0_REG (DR_REG_H264_DMA_BASE + 0xb0c)
/** H264_DMA_ACCESS_INTER_MEM_START_ADDR0 : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR0    0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR0_M  (H264_DMA_ACCESS_INTER_MEM_START_ADDR0_V << H264_DMA_ACCESS_INTER_MEM_START_ADDR0_S)
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR0_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR0_S  0

/** H264_DMA_INTER_MEM_END_ADDR0_REG register
 *  end address of inter memory range0 register
 */
#define H264_DMA_INTER_MEM_END_ADDR0_REG (DR_REG_H264_DMA_BASE + 0xb10)
/** H264_DMA_ACCESS_INTER_MEM_END_ADDR0 : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR0    0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR0_M  (H264_DMA_ACCESS_INTER_MEM_END_ADDR0_V << H264_DMA_ACCESS_INTER_MEM_END_ADDR0_S)
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR0_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR0_S  0

/** H264_DMA_INTER_MEM_START_ADDR1_REG register
 *  Start address of inter memory range1 register
 */
#define H264_DMA_INTER_MEM_START_ADDR1_REG (DR_REG_H264_DMA_BASE + 0xb14)
/** H264_DMA_ACCESS_INTER_MEM_START_ADDR1 : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR1    0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR1_M  (H264_DMA_ACCESS_INTER_MEM_START_ADDR1_V << H264_DMA_ACCESS_INTER_MEM_START_ADDR1_S)
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR1_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_START_ADDR1_S  0

/** H264_DMA_INTER_MEM_END_ADDR1_REG register
 *  end address of inter memory range1 register
 */
#define H264_DMA_INTER_MEM_END_ADDR1_REG (DR_REG_H264_DMA_BASE + 0xb18)
/** H264_DMA_ACCESS_INTER_MEM_END_ADDR1 : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR1    0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR1_M  (H264_DMA_ACCESS_INTER_MEM_END_ADDR1_V << H264_DMA_ACCESS_INTER_MEM_END_ADDR1_S)
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR1_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_INTER_MEM_END_ADDR1_S  0

/** H264_DMA_EXTER_MEM_START_ADDR0_REG register
 *  Start address of exter memory range0 register
 */
#define H264_DMA_EXTER_MEM_START_ADDR0_REG (DR_REG_H264_DMA_BASE + 0xb20)
/** H264_DMA_ACCESS_EXTER_MEM_START_ADDR0 : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR0    0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR0_M  (H264_DMA_ACCESS_EXTER_MEM_START_ADDR0_V << H264_DMA_ACCESS_EXTER_MEM_START_ADDR0_S)
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR0_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR0_S  0

/** H264_DMA_EXTER_MEM_END_ADDR0_REG register
 *  end address of exter memory range0 register
 */
#define H264_DMA_EXTER_MEM_END_ADDR0_REG (DR_REG_H264_DMA_BASE + 0xb24)
/** H264_DMA_ACCESS_EXTER_MEM_END_ADDR0 : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR0    0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR0_M  (H264_DMA_ACCESS_EXTER_MEM_END_ADDR0_V << H264_DMA_ACCESS_EXTER_MEM_END_ADDR0_S)
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR0_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR0_S  0

/** H264_DMA_EXTER_MEM_START_ADDR1_REG register
 *  Start address of exter memory range1 register
 */
#define H264_DMA_EXTER_MEM_START_ADDR1_REG (DR_REG_H264_DMA_BASE + 0xb28)
/** H264_DMA_ACCESS_EXTER_MEM_START_ADDR1 : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR1    0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR1_M  (H264_DMA_ACCESS_EXTER_MEM_START_ADDR1_V << H264_DMA_ACCESS_EXTER_MEM_START_ADDR1_S)
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR1_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_START_ADDR1_S  0

/** H264_DMA_EXTER_MEM_END_ADDR1_REG register
 *  end address of exter memory range1 register
 */
#define H264_DMA_EXTER_MEM_END_ADDR1_REG (DR_REG_H264_DMA_BASE + 0xb2c)
/** H264_DMA_ACCESS_EXTER_MEM_END_ADDR1 : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR1    0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR1_M  (H264_DMA_ACCESS_EXTER_MEM_END_ADDR1_V << H264_DMA_ACCESS_EXTER_MEM_END_ADDR1_S)
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR1_V  0xFFFFFFFFU
#define H264_DMA_ACCESS_EXTER_MEM_END_ADDR1_S  0

/** H264_DMA_OUT_ARB_CONFIG_REG register
 *  reserved
 */
#define H264_DMA_OUT_ARB_CONFIG_REG (DR_REG_H264_DMA_BASE + 0xb30)
/** H264_DMA_OUT_ARB_TIMEOUT_NUM : R/W; bitpos: [15:0]; default: 0;
 *  Set the max number of timeout count of arbiter
 */
#define H264_DMA_OUT_ARB_TIMEOUT_NUM    0x0000FFFFU
#define H264_DMA_OUT_ARB_TIMEOUT_NUM_M  (H264_DMA_OUT_ARB_TIMEOUT_NUM_V << H264_DMA_OUT_ARB_TIMEOUT_NUM_S)
#define H264_DMA_OUT_ARB_TIMEOUT_NUM_V  0x0000FFFFU
#define H264_DMA_OUT_ARB_TIMEOUT_NUM_S  0
/** H264_DMA_OUT_WEIGHT_EN : R/W; bitpos: [16]; default: 0;
 *  reserved
 */
#define H264_DMA_OUT_WEIGHT_EN    (BIT(16))
#define H264_DMA_OUT_WEIGHT_EN_M  (H264_DMA_OUT_WEIGHT_EN_V << H264_DMA_OUT_WEIGHT_EN_S)
#define H264_DMA_OUT_WEIGHT_EN_V  0x00000001U
#define H264_DMA_OUT_WEIGHT_EN_S  16

/** H264_DMA_IN_ARB_CONFIG_REG register
 *  reserved
 */
#define H264_DMA_IN_ARB_CONFIG_REG (DR_REG_H264_DMA_BASE + 0xb34)
/** H264_DMA_IN_ARB_TIMEOUT_NUM : R/W; bitpos: [15:0]; default: 0;
 *  Set the max number of timeout count of arbiter
 */
#define H264_DMA_IN_ARB_TIMEOUT_NUM    0x0000FFFFU
#define H264_DMA_IN_ARB_TIMEOUT_NUM_M  (H264_DMA_IN_ARB_TIMEOUT_NUM_V << H264_DMA_IN_ARB_TIMEOUT_NUM_S)
#define H264_DMA_IN_ARB_TIMEOUT_NUM_V  0x0000FFFFU
#define H264_DMA_IN_ARB_TIMEOUT_NUM_S  0
/** H264_DMA_IN_WEIGHT_EN : R/W; bitpos: [16]; default: 0;
 *  reserved
 */
#define H264_DMA_IN_WEIGHT_EN    (BIT(16))
#define H264_DMA_IN_WEIGHT_EN_M  (H264_DMA_IN_WEIGHT_EN_V << H264_DMA_IN_WEIGHT_EN_S)
#define H264_DMA_IN_WEIGHT_EN_V  0x00000001U
#define H264_DMA_IN_WEIGHT_EN_S  16

/** H264_DMA_DATE_REG register
 *  reserved
 */
#define H264_DMA_DATE_REG (DR_REG_H264_DMA_BASE + 0xb3c)
/** H264_DMA_DATE : R/W; bitpos: [31:0]; default: 539165699;
 *  register version.
 */
#define H264_DMA_DATE    0xFFFFFFFFU
#define H264_DMA_DATE_M  (H264_DMA_DATE_V << H264_DMA_DATE_S)
#define H264_DMA_DATE_V  0xFFFFFFFFU
#define H264_DMA_DATE_S  0

/** H264_DMA_COUNTER_RST_REG register
 *  counter reset register
 */
#define H264_DMA_COUNTER_RST_REG (DR_REG_H264_DMA_BASE + 0xb50)
/** H264_DMA_RX_CH0_EXTER_COUNTER_RST : R/W; bitpos: [0]; default: 0;
 *  Write 1 then write 0 to this bit to reset rx ch0 counter.
 */
#define H264_DMA_RX_CH0_EXTER_COUNTER_RST    (BIT(0))
#define H264_DMA_RX_CH0_EXTER_COUNTER_RST_M  (H264_DMA_RX_CH0_EXTER_COUNTER_RST_V << H264_DMA_RX_CH0_EXTER_COUNTER_RST_S)
#define H264_DMA_RX_CH0_EXTER_COUNTER_RST_V  0x00000001U
#define H264_DMA_RX_CH0_EXTER_COUNTER_RST_S  0
/** H264_DMA_RX_CH1_EXTER_COUNTER_RST : R/W; bitpos: [1]; default: 0;
 *  Write 1 then write 0 to this bit to reset rx ch1 counter.
 */
#define H264_DMA_RX_CH1_EXTER_COUNTER_RST    (BIT(1))
#define H264_DMA_RX_CH1_EXTER_COUNTER_RST_M  (H264_DMA_RX_CH1_EXTER_COUNTER_RST_V << H264_DMA_RX_CH1_EXTER_COUNTER_RST_S)
#define H264_DMA_RX_CH1_EXTER_COUNTER_RST_V  0x00000001U
#define H264_DMA_RX_CH1_EXTER_COUNTER_RST_S  1
/** H264_DMA_RX_CH2_INTER_COUNTER_RST : R/W; bitpos: [2]; default: 0;
 *  Write 1 then write 0 to this bit to reset rx ch2 counter.
 */
#define H264_DMA_RX_CH2_INTER_COUNTER_RST    (BIT(2))
#define H264_DMA_RX_CH2_INTER_COUNTER_RST_M  (H264_DMA_RX_CH2_INTER_COUNTER_RST_V << H264_DMA_RX_CH2_INTER_COUNTER_RST_S)
#define H264_DMA_RX_CH2_INTER_COUNTER_RST_V  0x00000001U
#define H264_DMA_RX_CH2_INTER_COUNTER_RST_S  2
/** H264_DMA_RX_CH5_INTER_COUNTER_RST : R/W; bitpos: [3]; default: 0;
 *  Write 1 then write 0 to this bit to reset rx ch5 counter.
 */
#define H264_DMA_RX_CH5_INTER_COUNTER_RST    (BIT(3))
#define H264_DMA_RX_CH5_INTER_COUNTER_RST_M  (H264_DMA_RX_CH5_INTER_COUNTER_RST_V << H264_DMA_RX_CH5_INTER_COUNTER_RST_S)
#define H264_DMA_RX_CH5_INTER_COUNTER_RST_V  0x00000001U
#define H264_DMA_RX_CH5_INTER_COUNTER_RST_S  3

/** H264_DMA_RX_CH0_COUNTER_REG register
 *  rx ch0 counter register
 */
#define H264_DMA_RX_CH0_COUNTER_REG (DR_REG_H264_DMA_BASE + 0xb54)
/** H264_DMA_RX_CH0_CNT : RO; bitpos: [22:0]; default: 0;
 *  rx ch0 counter register
 */
#define H264_DMA_RX_CH0_CNT    0x007FFFFFU
#define H264_DMA_RX_CH0_CNT_M  (H264_DMA_RX_CH0_CNT_V << H264_DMA_RX_CH0_CNT_S)
#define H264_DMA_RX_CH0_CNT_V  0x007FFFFFU
#define H264_DMA_RX_CH0_CNT_S  0

/** H264_DMA_RX_CH1_COUNTER_REG register
 *  rx ch1 counter register
 */
#define H264_DMA_RX_CH1_COUNTER_REG (DR_REG_H264_DMA_BASE + 0xb58)
/** H264_DMA_RX_CH1_CNT : RO; bitpos: [20:0]; default: 0;
 *  rx ch1 counter register
 */
#define H264_DMA_RX_CH1_CNT    0x001FFFFFU
#define H264_DMA_RX_CH1_CNT_M  (H264_DMA_RX_CH1_CNT_V << H264_DMA_RX_CH1_CNT_S)
#define H264_DMA_RX_CH1_CNT_V  0x001FFFFFU
#define H264_DMA_RX_CH1_CNT_S  0

/** H264_DMA_RX_CH2_COUNTER_REG register
 *  rx ch2 counter register
 */
#define H264_DMA_RX_CH2_COUNTER_REG (DR_REG_H264_DMA_BASE + 0xb5c)
/** H264_DMA_RX_CH2_CNT : RO; bitpos: [10:0]; default: 0;
 *  rx ch2 counter register
 */
#define H264_DMA_RX_CH2_CNT    0x000007FFU
#define H264_DMA_RX_CH2_CNT_M  (H264_DMA_RX_CH2_CNT_V << H264_DMA_RX_CH2_CNT_S)
#define H264_DMA_RX_CH2_CNT_V  0x000007FFU
#define H264_DMA_RX_CH2_CNT_S  0

/** H264_DMA_RX_CH5_COUNTER_REG register
 *  rx ch5 counter register
 */
#define H264_DMA_RX_CH5_COUNTER_REG (DR_REG_H264_DMA_BASE + 0xb60)
/** H264_DMA_RX_CH5_CNT : RO; bitpos: [16:0]; default: 0;
 *  rx ch5 counter register
 */
#define H264_DMA_RX_CH5_CNT    0x0001FFFFU
#define H264_DMA_RX_CH5_CNT_M  (H264_DMA_RX_CH5_CNT_V << H264_DMA_RX_CH5_CNT_S)
#define H264_DMA_RX_CH5_CNT_V  0x0001FFFFU
#define H264_DMA_RX_CH5_CNT_S  0

#ifdef __cplusplus
}
#endif
