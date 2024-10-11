/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** DMA2D_OUT_CONF0_CH0_REG register
 *  Configures the tx direction of channel 0
 */
#define DMA2D_OUT_CONF0_CH0_REG (DR_REG_DMA2D_BASE + 0x0)
/** DMA2D_OUT_AUTO_WRBACK_CH0 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define DMA2D_OUT_AUTO_WRBACK_CH0    (BIT(0))
#define DMA2D_OUT_AUTO_WRBACK_CH0_M  (DMA2D_OUT_AUTO_WRBACK_CH0_V << DMA2D_OUT_AUTO_WRBACK_CH0_S)
#define DMA2D_OUT_AUTO_WRBACK_CH0_V  0x00000001U
#define DMA2D_OUT_AUTO_WRBACK_CH0_S  0
/** DMA2D_OUT_EOF_MODE_CH0 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define DMA2D_OUT_EOF_MODE_CH0    (BIT(1))
#define DMA2D_OUT_EOF_MODE_CH0_M  (DMA2D_OUT_EOF_MODE_CH0_V << DMA2D_OUT_EOF_MODE_CH0_S)
#define DMA2D_OUT_EOF_MODE_CH0_V  0x00000001U
#define DMA2D_OUT_EOF_MODE_CH0_S  1
/** DMA2D_OUTDSCR_BURST_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define DMA2D_OUTDSCR_BURST_EN_CH0    (BIT(2))
#define DMA2D_OUTDSCR_BURST_EN_CH0_M  (DMA2D_OUTDSCR_BURST_EN_CH0_V << DMA2D_OUTDSCR_BURST_EN_CH0_S)
#define DMA2D_OUTDSCR_BURST_EN_CH0_V  0x00000001U
#define DMA2D_OUTDSCR_BURST_EN_CH0_S  2
/** DMA2D_OUT_ECC_AES_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define DMA2D_OUT_ECC_AES_EN_CH0    (BIT(3))
#define DMA2D_OUT_ECC_AES_EN_CH0_M  (DMA2D_OUT_ECC_AES_EN_CH0_V << DMA2D_OUT_ECC_AES_EN_CH0_S)
#define DMA2D_OUT_ECC_AES_EN_CH0_V  0x00000001U
#define DMA2D_OUT_ECC_AES_EN_CH0_S  3
/** DMA2D_OUT_CHECK_OWNER_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define DMA2D_OUT_CHECK_OWNER_CH0    (BIT(4))
#define DMA2D_OUT_CHECK_OWNER_CH0_M  (DMA2D_OUT_CHECK_OWNER_CH0_V << DMA2D_OUT_CHECK_OWNER_CH0_S)
#define DMA2D_OUT_CHECK_OWNER_CH0_V  0x00000001U
#define DMA2D_OUT_CHECK_OWNER_CH0_S  4
/** DMA2D_OUT_LOOP_TEST_CH0 : R/W; bitpos: [5]; default: 0;
 *  reserved
 */
#define DMA2D_OUT_LOOP_TEST_CH0    (BIT(5))
#define DMA2D_OUT_LOOP_TEST_CH0_M  (DMA2D_OUT_LOOP_TEST_CH0_V << DMA2D_OUT_LOOP_TEST_CH0_S)
#define DMA2D_OUT_LOOP_TEST_CH0_V  0x00000001U
#define DMA2D_OUT_LOOP_TEST_CH0_S  5
/** DMA2D_OUT_MEM_BURST_LENGTH_CH0 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 0. 0: 8 bytes      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define DMA2D_OUT_MEM_BURST_LENGTH_CH0    0x00000007U
#define DMA2D_OUT_MEM_BURST_LENGTH_CH0_M  (DMA2D_OUT_MEM_BURST_LENGTH_CH0_V << DMA2D_OUT_MEM_BURST_LENGTH_CH0_S)
#define DMA2D_OUT_MEM_BURST_LENGTH_CH0_V  0x00000007U
#define DMA2D_OUT_MEM_BURST_LENGTH_CH0_S  6
/** DMA2D_OUT_MACRO_BLOCK_SIZE_CH0 : R/W; bitpos: [10:9]; default: 0;
 *  Sel TX macro-block size 0: 8pixel*8pixel     1: 8pixel*16pixel     2:
 *  16pixel*16pixel     3: no macro-block     , only useful in mode 1 of the link
 *  descriptor
 */
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH0    0x00000003U
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH0_M  (DMA2D_OUT_MACRO_BLOCK_SIZE_CH0_V << DMA2D_OUT_MACRO_BLOCK_SIZE_CH0_S)
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH0_V  0x00000003U
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH0_S  9
/** DMA2D_OUT_DSCR_PORT_EN_CH0 : R/W; bitpos: [11]; default: 0;
 *  Set this bit to 1 to obtain descriptor from IP port
 */
#define DMA2D_OUT_DSCR_PORT_EN_CH0    (BIT(11))
#define DMA2D_OUT_DSCR_PORT_EN_CH0_M  (DMA2D_OUT_DSCR_PORT_EN_CH0_V << DMA2D_OUT_DSCR_PORT_EN_CH0_S)
#define DMA2D_OUT_DSCR_PORT_EN_CH0_V  0x00000001U
#define DMA2D_OUT_DSCR_PORT_EN_CH0_S  11
/** DMA2D_OUT_PAGE_BOUND_EN_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define DMA2D_OUT_PAGE_BOUND_EN_CH0    (BIT(12))
#define DMA2D_OUT_PAGE_BOUND_EN_CH0_M  (DMA2D_OUT_PAGE_BOUND_EN_CH0_V << DMA2D_OUT_PAGE_BOUND_EN_CH0_S)
#define DMA2D_OUT_PAGE_BOUND_EN_CH0_V  0x00000001U
#define DMA2D_OUT_PAGE_BOUND_EN_CH0_S  12
/** DMA2D_OUT_REORDER_EN_CH0 : R/W; bitpos: [16]; default: 0;
 *  Enable TX channel 0 macro block reorder when set to 1, only channel0 have this
 *  selection
 */
#define DMA2D_OUT_REORDER_EN_CH0    (BIT(16))
#define DMA2D_OUT_REORDER_EN_CH0_M  (DMA2D_OUT_REORDER_EN_CH0_V << DMA2D_OUT_REORDER_EN_CH0_S)
#define DMA2D_OUT_REORDER_EN_CH0_V  0x00000001U
#define DMA2D_OUT_REORDER_EN_CH0_S  16
/** DMA2D_OUT_RST_CH0 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset TX channel
 */
#define DMA2D_OUT_RST_CH0    (BIT(24))
#define DMA2D_OUT_RST_CH0_M  (DMA2D_OUT_RST_CH0_V << DMA2D_OUT_RST_CH0_S)
#define DMA2D_OUT_RST_CH0_V  0x00000001U
#define DMA2D_OUT_RST_CH0_S  24
/** DMA2D_OUT_CMD_DISABLE_CH0 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define DMA2D_OUT_CMD_DISABLE_CH0    (BIT(25))
#define DMA2D_OUT_CMD_DISABLE_CH0_M  (DMA2D_OUT_CMD_DISABLE_CH0_V << DMA2D_OUT_CMD_DISABLE_CH0_S)
#define DMA2D_OUT_CMD_DISABLE_CH0_V  0x00000001U
#define DMA2D_OUT_CMD_DISABLE_CH0_S  25
/** DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH0 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH0    (BIT(26))
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH0_M  (DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH0_V << DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH0_S)
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH0_V  0x00000001U
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH0_S  26

/** DMA2D_OUT_INT_RAW_CH0_REG register
 *  Raw interrupt status of TX channel 0
 */
#define DMA2D_OUT_INT_RAW_CH0_REG (DR_REG_DMA2D_BASE + 0x4)
/** DMA2D_OUT_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define DMA2D_OUT_DONE_CH0_INT_RAW    (BIT(0))
#define DMA2D_OUT_DONE_CH0_INT_RAW_M  (DMA2D_OUT_DONE_CH0_INT_RAW_V << DMA2D_OUT_DONE_CH0_INT_RAW_S)
#define DMA2D_OUT_DONE_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DONE_CH0_INT_RAW_S  0
/** DMA2D_OUT_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define DMA2D_OUT_EOF_CH0_INT_RAW    (BIT(1))
#define DMA2D_OUT_EOF_CH0_INT_RAW_M  (DMA2D_OUT_EOF_CH0_INT_RAW_V << DMA2D_OUT_EOF_CH0_INT_RAW_S)
#define DMA2D_OUT_EOF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUT_EOF_CH0_INT_RAW_S  1
/** DMA2D_OUT_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define DMA2D_OUT_DSCR_ERR_CH0_INT_RAW    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH0_INT_RAW_M  (DMA2D_OUT_DSCR_ERR_CH0_INT_RAW_V << DMA2D_OUT_DSCR_ERR_CH0_INT_RAW_S)
#define DMA2D_OUT_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH0_INT_RAW_S  2
/** DMA2D_OUT_TOTAL_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_RAW    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_RAW_M  (DMA2D_OUT_TOTAL_EOF_CH0_INT_RAW_V << DMA2D_OUT_TOTAL_EOF_CH0_INT_RAW_S)
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_RAW_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_RAW    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L1_CH0_INT_RAW_V << DMA2D_OUTFIFO_OVF_L1_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_RAW_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_RAW    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L1_CH0_INT_RAW_V << DMA2D_OUTFIFO_UDF_L1_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_RAW_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_RAW    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L2_CH0_INT_RAW_V << DMA2D_OUTFIFO_OVF_L2_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_RAW_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_RAW    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L2_CH0_INT_RAW_V << DMA2D_OUTFIFO_UDF_L2_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_RAW_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH0_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_RAW    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L3_CH0_INT_RAW_V << DMA2D_OUTFIFO_OVF_L3_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_RAW_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH0_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_RAW    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L3_CH0_INT_RAW_V << DMA2D_OUTFIFO_UDF_L3_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_RAW_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is overflow.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_RAW    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_RAW_M  (DMA2D_OUTFIFO_RO_OVF_CH0_INT_RAW_V << DMA2D_OUTFIFO_RO_OVF_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_RAW_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is underflow.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_RAW    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_RAW_M  (DMA2D_OUTFIFO_RO_UDF_CH0_INT_RAW_V << DMA2D_OUTFIFO_RO_UDF_CH0_INT_RAW_S)
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_RAW_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_RAW    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_RAW_M  (DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_RAW_V << DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_RAW_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_RAW_S  12

/** DMA2D_OUT_INT_ENA_CH0_REG register
 *  Interrupt enable bits of TX channel 0
 */
#define DMA2D_OUT_INT_ENA_CH0_REG (DR_REG_DMA2D_BASE + 0x8)
/** DMA2D_OUT_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH0_INT_ENA    (BIT(0))
#define DMA2D_OUT_DONE_CH0_INT_ENA_M  (DMA2D_OUT_DONE_CH0_INT_ENA_V << DMA2D_OUT_DONE_CH0_INT_ENA_S)
#define DMA2D_OUT_DONE_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DONE_CH0_INT_ENA_S  0
/** DMA2D_OUT_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH0_INT_ENA    (BIT(1))
#define DMA2D_OUT_EOF_CH0_INT_ENA_M  (DMA2D_OUT_EOF_CH0_INT_ENA_V << DMA2D_OUT_EOF_CH0_INT_ENA_S)
#define DMA2D_OUT_EOF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUT_EOF_CH0_INT_ENA_S  1
/** DMA2D_OUT_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ENA    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ENA_M  (DMA2D_OUT_DSCR_ERR_CH0_INT_ENA_V << DMA2D_OUT_DSCR_ERR_CH0_INT_ENA_S)
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ENA_S  2
/** DMA2D_OUT_TOTAL_EOF_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ENA    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ENA_M  (DMA2D_OUT_TOTAL_EOF_CH0_INT_ENA_V << DMA2D_OUT_TOTAL_EOF_CH0_INT_ENA_S)
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ENA_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ENA    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L1_CH0_INT_ENA_V << DMA2D_OUTFIFO_OVF_L1_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ENA_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ENA    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L1_CH0_INT_ENA_V << DMA2D_OUTFIFO_UDF_L1_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ENA_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ENA    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L2_CH0_INT_ENA_V << DMA2D_OUTFIFO_OVF_L2_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ENA_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ENA    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L2_CH0_INT_ENA_V << DMA2D_OUTFIFO_UDF_L2_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ENA_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH0_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ENA    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L3_CH0_INT_ENA_V << DMA2D_OUTFIFO_OVF_L3_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ENA_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH0_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ENA    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L3_CH0_INT_ENA_V << DMA2D_OUTFIFO_UDF_L3_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ENA_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH0_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ENA    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ENA_M  (DMA2D_OUTFIFO_RO_OVF_CH0_INT_ENA_V << DMA2D_OUTFIFO_RO_OVF_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ENA_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH0_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ENA    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ENA_M  (DMA2D_OUTFIFO_RO_UDF_CH0_INT_ENA_V << DMA2D_OUTFIFO_RO_UDF_CH0_INT_ENA_S)
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ENA_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ENA    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ENA_M  (DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ENA_V << DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ENA_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ENA_S  12

/** DMA2D_OUT_INT_ST_CH0_REG register
 *  Masked interrupt status of TX channel 0
 */
#define DMA2D_OUT_INT_ST_CH0_REG (DR_REG_DMA2D_BASE + 0xc)
/** DMA2D_OUT_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH0_INT_ST    (BIT(0))
#define DMA2D_OUT_DONE_CH0_INT_ST_M  (DMA2D_OUT_DONE_CH0_INT_ST_V << DMA2D_OUT_DONE_CH0_INT_ST_S)
#define DMA2D_OUT_DONE_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUT_DONE_CH0_INT_ST_S  0
/** DMA2D_OUT_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH0_INT_ST    (BIT(1))
#define DMA2D_OUT_EOF_CH0_INT_ST_M  (DMA2D_OUT_EOF_CH0_INT_ST_V << DMA2D_OUT_EOF_CH0_INT_ST_S)
#define DMA2D_OUT_EOF_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUT_EOF_CH0_INT_ST_S  1
/** DMA2D_OUT_DSCR_ERR_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ST    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ST_M  (DMA2D_OUT_DSCR_ERR_CH0_INT_ST_V << DMA2D_OUT_DSCR_ERR_CH0_INT_ST_S)
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH0_INT_ST_S  2
/** DMA2D_OUT_TOTAL_EOF_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ST    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ST_M  (DMA2D_OUT_TOTAL_EOF_CH0_INT_ST_V << DMA2D_OUT_TOTAL_EOF_CH0_INT_ST_S)
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_ST_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ST    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ST_M  (DMA2D_OUTFIFO_OVF_L1_CH0_INT_ST_V << DMA2D_OUTFIFO_OVF_L1_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_ST_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ST    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ST_M  (DMA2D_OUTFIFO_UDF_L1_CH0_INT_ST_V << DMA2D_OUTFIFO_UDF_L1_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_ST_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ST    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ST_M  (DMA2D_OUTFIFO_OVF_L2_CH0_INT_ST_V << DMA2D_OUTFIFO_OVF_L2_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_ST_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ST    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ST_M  (DMA2D_OUTFIFO_UDF_L2_CH0_INT_ST_V << DMA2D_OUTFIFO_UDF_L2_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_ST_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH0_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ST    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ST_M  (DMA2D_OUTFIFO_OVF_L3_CH0_INT_ST_V << DMA2D_OUTFIFO_OVF_L3_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_ST_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH0_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ST    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ST_M  (DMA2D_OUTFIFO_UDF_L3_CH0_INT_ST_V << DMA2D_OUTFIFO_UDF_L3_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_ST_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH0_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ST    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ST_M  (DMA2D_OUTFIFO_RO_OVF_CH0_INT_ST_V << DMA2D_OUTFIFO_RO_OVF_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_ST_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH0_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ST    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ST_M  (DMA2D_OUTFIFO_RO_UDF_CH0_INT_ST_V << DMA2D_OUTFIFO_RO_UDF_CH0_INT_ST_S)
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_ST_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ST    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ST_M  (DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ST_V << DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ST_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ST_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_ST_S  12

/** DMA2D_OUT_INT_CLR_CH0_REG register
 *  Interrupt clear bits of TX channel 0
 */
#define DMA2D_OUT_INT_CLR_CH0_REG (DR_REG_DMA2D_BASE + 0x10)
/** DMA2D_OUT_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH0_INT_CLR    (BIT(0))
#define DMA2D_OUT_DONE_CH0_INT_CLR_M  (DMA2D_OUT_DONE_CH0_INT_CLR_V << DMA2D_OUT_DONE_CH0_INT_CLR_S)
#define DMA2D_OUT_DONE_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DONE_CH0_INT_CLR_S  0
/** DMA2D_OUT_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH0_INT_CLR    (BIT(1))
#define DMA2D_OUT_EOF_CH0_INT_CLR_M  (DMA2D_OUT_EOF_CH0_INT_CLR_V << DMA2D_OUT_EOF_CH0_INT_CLR_S)
#define DMA2D_OUT_EOF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUT_EOF_CH0_INT_CLR_S  1
/** DMA2D_OUT_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH0_INT_CLR    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH0_INT_CLR_M  (DMA2D_OUT_DSCR_ERR_CH0_INT_CLR_V << DMA2D_OUT_DSCR_ERR_CH0_INT_CLR_S)
#define DMA2D_OUT_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH0_INT_CLR_S  2
/** DMA2D_OUT_TOTAL_EOF_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_CLR    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_CLR_M  (DMA2D_OUT_TOTAL_EOF_CH0_INT_CLR_V << DMA2D_OUT_TOTAL_EOF_CH0_INT_CLR_S)
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH0_INT_CLR_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_CLR    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L1_CH0_INT_CLR_V << DMA2D_OUTFIFO_OVF_L1_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH0_INT_CLR_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_CLR    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L1_CH0_INT_CLR_V << DMA2D_OUTFIFO_UDF_L1_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH0_INT_CLR_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_CLR    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L2_CH0_INT_CLR_V << DMA2D_OUTFIFO_OVF_L2_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH0_INT_CLR_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_CLR    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L2_CH0_INT_CLR_V << DMA2D_OUTFIFO_UDF_L2_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH0_INT_CLR_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH0_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_CLR    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L3_CH0_INT_CLR_V << DMA2D_OUTFIFO_OVF_L3_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH0_INT_CLR_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH0_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_CLR    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L3_CH0_INT_CLR_V << DMA2D_OUTFIFO_UDF_L3_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH0_INT_CLR_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH0_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_CLR    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_CLR_M  (DMA2D_OUTFIFO_RO_OVF_CH0_INT_CLR_V << DMA2D_OUTFIFO_RO_OVF_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH0_INT_CLR_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH0_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_CLR    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_CLR_M  (DMA2D_OUTFIFO_RO_UDF_CH0_INT_CLR_V << DMA2D_OUTFIFO_RO_UDF_CH0_INT_CLR_S)
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH0_INT_CLR_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_CLR    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_CLR_M  (DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_CLR_V << DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_CLR_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH0_INT_CLR_S  12

/** DMA2D_OUTFIFO_STATUS_CH0_REG register
 *  Represents the status of the tx fifo of channel 0
 */
#define DMA2D_OUTFIFO_STATUS_CH0_REG (DR_REG_DMA2D_BASE + 0x14)
/** DMA2D_OUTFIFO_FULL_L2_CH0 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L2_CH0    (BIT(0))
#define DMA2D_OUTFIFO_FULL_L2_CH0_M  (DMA2D_OUTFIFO_FULL_L2_CH0_V << DMA2D_OUTFIFO_FULL_L2_CH0_S)
#define DMA2D_OUTFIFO_FULL_L2_CH0_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L2_CH0_S  0
/** DMA2D_OUTFIFO_EMPTY_L2_CH0 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L2_CH0    (BIT(1))
#define DMA2D_OUTFIFO_EMPTY_L2_CH0_M  (DMA2D_OUTFIFO_EMPTY_L2_CH0_V << DMA2D_OUTFIFO_EMPTY_L2_CH0_S)
#define DMA2D_OUTFIFO_EMPTY_L2_CH0_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L2_CH0_S  1
/** DMA2D_OUTFIFO_CNT_L2_CH0 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L2_CH0    0x0000000FU
#define DMA2D_OUTFIFO_CNT_L2_CH0_M  (DMA2D_OUTFIFO_CNT_L2_CH0_V << DMA2D_OUTFIFO_CNT_L2_CH0_S)
#define DMA2D_OUTFIFO_CNT_L2_CH0_V  0x0000000FU
#define DMA2D_OUTFIFO_CNT_L2_CH0_S  2
/** DMA2D_OUT_REMAIN_UNDER_1B_CH0 : RO; bitpos: [7]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_1B_CH0    (BIT(7))
#define DMA2D_OUT_REMAIN_UNDER_1B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_1B_CH0_V << DMA2D_OUT_REMAIN_UNDER_1B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_1B_CH0_S  7
/** DMA2D_OUT_REMAIN_UNDER_2B_CH0 : RO; bitpos: [8]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_2B_CH0    (BIT(8))
#define DMA2D_OUT_REMAIN_UNDER_2B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_2B_CH0_V << DMA2D_OUT_REMAIN_UNDER_2B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_2B_CH0_S  8
/** DMA2D_OUT_REMAIN_UNDER_3B_CH0 : RO; bitpos: [9]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_3B_CH0    (BIT(9))
#define DMA2D_OUT_REMAIN_UNDER_3B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_3B_CH0_V << DMA2D_OUT_REMAIN_UNDER_3B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_3B_CH0_S  9
/** DMA2D_OUT_REMAIN_UNDER_4B_CH0 : RO; bitpos: [10]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_4B_CH0    (BIT(10))
#define DMA2D_OUT_REMAIN_UNDER_4B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_4B_CH0_V << DMA2D_OUT_REMAIN_UNDER_4B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_4B_CH0_S  10
/** DMA2D_OUT_REMAIN_UNDER_5B_CH0 : RO; bitpos: [11]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_5B_CH0    (BIT(11))
#define DMA2D_OUT_REMAIN_UNDER_5B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_5B_CH0_V << DMA2D_OUT_REMAIN_UNDER_5B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_5B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_5B_CH0_S  11
/** DMA2D_OUT_REMAIN_UNDER_6B_CH0 : RO; bitpos: [12]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_6B_CH0    (BIT(12))
#define DMA2D_OUT_REMAIN_UNDER_6B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_6B_CH0_V << DMA2D_OUT_REMAIN_UNDER_6B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_6B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_6B_CH0_S  12
/** DMA2D_OUT_REMAIN_UNDER_7B_CH0 : RO; bitpos: [13]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_7B_CH0    (BIT(13))
#define DMA2D_OUT_REMAIN_UNDER_7B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_7B_CH0_V << DMA2D_OUT_REMAIN_UNDER_7B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_7B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_7B_CH0_S  13
/** DMA2D_OUT_REMAIN_UNDER_8B_CH0 : RO; bitpos: [14]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_8B_CH0    (BIT(14))
#define DMA2D_OUT_REMAIN_UNDER_8B_CH0_M  (DMA2D_OUT_REMAIN_UNDER_8B_CH0_V << DMA2D_OUT_REMAIN_UNDER_8B_CH0_S)
#define DMA2D_OUT_REMAIN_UNDER_8B_CH0_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_8B_CH0_S  14
/** DMA2D_OUTFIFO_FULL_L1_CH0 : RO; bitpos: [15]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L1_CH0    (BIT(15))
#define DMA2D_OUTFIFO_FULL_L1_CH0_M  (DMA2D_OUTFIFO_FULL_L1_CH0_V << DMA2D_OUTFIFO_FULL_L1_CH0_S)
#define DMA2D_OUTFIFO_FULL_L1_CH0_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L1_CH0_S  15
/** DMA2D_OUTFIFO_EMPTY_L1_CH0 : RO; bitpos: [16]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L1_CH0    (BIT(16))
#define DMA2D_OUTFIFO_EMPTY_L1_CH0_M  (DMA2D_OUTFIFO_EMPTY_L1_CH0_V << DMA2D_OUTFIFO_EMPTY_L1_CH0_S)
#define DMA2D_OUTFIFO_EMPTY_L1_CH0_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L1_CH0_S  16
/** DMA2D_OUTFIFO_CNT_L1_CH0 : RO; bitpos: [21:17]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L1_CH0    0x0000001FU
#define DMA2D_OUTFIFO_CNT_L1_CH0_M  (DMA2D_OUTFIFO_CNT_L1_CH0_V << DMA2D_OUTFIFO_CNT_L1_CH0_S)
#define DMA2D_OUTFIFO_CNT_L1_CH0_V  0x0000001FU
#define DMA2D_OUTFIFO_CNT_L1_CH0_S  17
/** DMA2D_OUTFIFO_FULL_L3_CH0 : RO; bitpos: [22]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L3_CH0    (BIT(22))
#define DMA2D_OUTFIFO_FULL_L3_CH0_M  (DMA2D_OUTFIFO_FULL_L3_CH0_V << DMA2D_OUTFIFO_FULL_L3_CH0_S)
#define DMA2D_OUTFIFO_FULL_L3_CH0_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L3_CH0_S  22
/** DMA2D_OUTFIFO_EMPTY_L3_CH0 : RO; bitpos: [23]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L3_CH0    (BIT(23))
#define DMA2D_OUTFIFO_EMPTY_L3_CH0_M  (DMA2D_OUTFIFO_EMPTY_L3_CH0_V << DMA2D_OUTFIFO_EMPTY_L3_CH0_S)
#define DMA2D_OUTFIFO_EMPTY_L3_CH0_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L3_CH0_S  23
/** DMA2D_OUTFIFO_CNT_L3_CH0 : RO; bitpos: [28:24]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L3_CH0    0x0000001FU
#define DMA2D_OUTFIFO_CNT_L3_CH0_M  (DMA2D_OUTFIFO_CNT_L3_CH0_V << DMA2D_OUTFIFO_CNT_L3_CH0_S)
#define DMA2D_OUTFIFO_CNT_L3_CH0_V  0x0000001FU
#define DMA2D_OUTFIFO_CNT_L3_CH0_S  24

/** DMA2D_OUT_PUSH_CH0_REG register
 *  Configures the tx fifo of channel 0
 */
#define DMA2D_OUT_PUSH_CH0_REG (DR_REG_DMA2D_BASE + 0x18)
/** DMA2D_OUTFIFO_WDATA_CH0 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define DMA2D_OUTFIFO_WDATA_CH0    0x000003FFU
#define DMA2D_OUTFIFO_WDATA_CH0_M  (DMA2D_OUTFIFO_WDATA_CH0_V << DMA2D_OUTFIFO_WDATA_CH0_S)
#define DMA2D_OUTFIFO_WDATA_CH0_V  0x000003FFU
#define DMA2D_OUTFIFO_WDATA_CH0_S  0
/** DMA2D_OUTFIFO_PUSH_CH0 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define DMA2D_OUTFIFO_PUSH_CH0    (BIT(10))
#define DMA2D_OUTFIFO_PUSH_CH0_M  (DMA2D_OUTFIFO_PUSH_CH0_V << DMA2D_OUTFIFO_PUSH_CH0_S)
#define DMA2D_OUTFIFO_PUSH_CH0_V  0x00000001U
#define DMA2D_OUTFIFO_PUSH_CH0_S  10

/** DMA2D_OUT_LINK_CONF_CH0_REG register
 *  Configures the tx descriptor operations of channel 0
 */
#define DMA2D_OUT_LINK_CONF_CH0_REG (DR_REG_DMA2D_BASE + 0x1c)
/** DMA2D_OUTLINK_STOP_CH0 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define DMA2D_OUTLINK_STOP_CH0    (BIT(20))
#define DMA2D_OUTLINK_STOP_CH0_M  (DMA2D_OUTLINK_STOP_CH0_V << DMA2D_OUTLINK_STOP_CH0_S)
#define DMA2D_OUTLINK_STOP_CH0_V  0x00000001U
#define DMA2D_OUTLINK_STOP_CH0_S  20
/** DMA2D_OUTLINK_START_CH0 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define DMA2D_OUTLINK_START_CH0    (BIT(21))
#define DMA2D_OUTLINK_START_CH0_M  (DMA2D_OUTLINK_START_CH0_V << DMA2D_OUTLINK_START_CH0_S)
#define DMA2D_OUTLINK_START_CH0_V  0x00000001U
#define DMA2D_OUTLINK_START_CH0_S  21
/** DMA2D_OUTLINK_RESTART_CH0 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define DMA2D_OUTLINK_RESTART_CH0    (BIT(22))
#define DMA2D_OUTLINK_RESTART_CH0_M  (DMA2D_OUTLINK_RESTART_CH0_V << DMA2D_OUTLINK_RESTART_CH0_S)
#define DMA2D_OUTLINK_RESTART_CH0_V  0x00000001U
#define DMA2D_OUTLINK_RESTART_CH0_S  22
/** DMA2D_OUTLINK_PARK_CH0 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define DMA2D_OUTLINK_PARK_CH0    (BIT(23))
#define DMA2D_OUTLINK_PARK_CH0_M  (DMA2D_OUTLINK_PARK_CH0_V << DMA2D_OUTLINK_PARK_CH0_S)
#define DMA2D_OUTLINK_PARK_CH0_V  0x00000001U
#define DMA2D_OUTLINK_PARK_CH0_S  23

/** DMA2D_OUT_LINK_ADDR_CH0_REG register
 *  Configures the tx descriptor address of channel 0
 */
#define DMA2D_OUT_LINK_ADDR_CH0_REG (DR_REG_DMA2D_BASE + 0x20)
/** DMA2D_OUTLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define DMA2D_OUTLINK_ADDR_CH0    0xFFFFFFFFU
#define DMA2D_OUTLINK_ADDR_CH0_M  (DMA2D_OUTLINK_ADDR_CH0_V << DMA2D_OUTLINK_ADDR_CH0_S)
#define DMA2D_OUTLINK_ADDR_CH0_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_ADDR_CH0_S  0

/** DMA2D_OUT_STATE_CH0_REG register
 *  Represents the working status of the tx descriptor of channel 0
 */
#define DMA2D_OUT_STATE_CH0_REG (DR_REG_DMA2D_BASE + 0x24)
/** DMA2D_OUTLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define DMA2D_OUTLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define DMA2D_OUTLINK_DSCR_ADDR_CH0_M  (DMA2D_OUTLINK_DSCR_ADDR_CH0_V << DMA2D_OUTLINK_DSCR_ADDR_CH0_S)
#define DMA2D_OUTLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define DMA2D_OUTLINK_DSCR_ADDR_CH0_S  0
/** DMA2D_OUT_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define DMA2D_OUT_DSCR_STATE_CH0    0x00000003U
#define DMA2D_OUT_DSCR_STATE_CH0_M  (DMA2D_OUT_DSCR_STATE_CH0_V << DMA2D_OUT_DSCR_STATE_CH0_S)
#define DMA2D_OUT_DSCR_STATE_CH0_V  0x00000003U
#define DMA2D_OUT_DSCR_STATE_CH0_S  18
/** DMA2D_OUT_STATE_CH0 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define DMA2D_OUT_STATE_CH0    0x0000000FU
#define DMA2D_OUT_STATE_CH0_M  (DMA2D_OUT_STATE_CH0_V << DMA2D_OUT_STATE_CH0_S)
#define DMA2D_OUT_STATE_CH0_V  0x0000000FU
#define DMA2D_OUT_STATE_CH0_S  20
/** DMA2D_OUT_RESET_AVAIL_CH0 : RO; bitpos: [24]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define DMA2D_OUT_RESET_AVAIL_CH0    (BIT(24))
#define DMA2D_OUT_RESET_AVAIL_CH0_M  (DMA2D_OUT_RESET_AVAIL_CH0_V << DMA2D_OUT_RESET_AVAIL_CH0_S)
#define DMA2D_OUT_RESET_AVAIL_CH0_V  0x00000001U
#define DMA2D_OUT_RESET_AVAIL_CH0_S  24

/** DMA2D_OUT_EOF_DES_ADDR_CH0_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_EOF_DES_ADDR_CH0_REG (DR_REG_DMA2D_BASE + 0x28)
/** DMA2D_OUT_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define DMA2D_OUT_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define DMA2D_OUT_EOF_DES_ADDR_CH0_M  (DMA2D_OUT_EOF_DES_ADDR_CH0_V << DMA2D_OUT_EOF_DES_ADDR_CH0_S)
#define DMA2D_OUT_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define DMA2D_OUT_EOF_DES_ADDR_CH0_S  0

/** DMA2D_OUT_DSCR_CH0_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_CH0_REG (DR_REG_DMA2D_BASE + 0x2c)
/** DMA2D_OUTLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define DMA2D_OUTLINK_DSCR_CH0    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_CH0_M  (DMA2D_OUTLINK_DSCR_CH0_V << DMA2D_OUTLINK_DSCR_CH0_S)
#define DMA2D_OUTLINK_DSCR_CH0_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_CH0_S  0

/** DMA2D_OUT_DSCR_BF0_CH0_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_BF0_CH0_REG (DR_REG_DMA2D_BASE + 0x30)
/** DMA2D_OUTLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define DMA2D_OUTLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF0_CH0_M  (DMA2D_OUTLINK_DSCR_BF0_CH0_V << DMA2D_OUTLINK_DSCR_BF0_CH0_S)
#define DMA2D_OUTLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF0_CH0_S  0

/** DMA2D_OUT_DSCR_BF1_CH0_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_BF1_CH0_REG (DR_REG_DMA2D_BASE + 0x34)
/** DMA2D_OUTLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define DMA2D_OUTLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF1_CH0_M  (DMA2D_OUTLINK_DSCR_BF1_CH0_V << DMA2D_OUTLINK_DSCR_BF1_CH0_S)
#define DMA2D_OUTLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF1_CH0_S  0

/** DMA2D_OUT_PERI_SEL_CH0_REG register
 *  Configures the tx peripheral of channel 0
 */
#define DMA2D_OUT_PERI_SEL_CH0_REG (DR_REG_DMA2D_BASE + 0x38)
/** DMA2D_OUT_PERI_SEL_CH0 : R/W; bitpos: [2:0]; default: 7;
 *  This register is used to select peripheral for Tx channel   0:  jpeg     1:
 *  display-1     2: display-2     3: display-3      7: no choose
 */
#define DMA2D_OUT_PERI_SEL_CH0    0x00000007U
#define DMA2D_OUT_PERI_SEL_CH0_M  (DMA2D_OUT_PERI_SEL_CH0_V << DMA2D_OUT_PERI_SEL_CH0_S)
#define DMA2D_OUT_PERI_SEL_CH0_V  0x00000007U
#define DMA2D_OUT_PERI_SEL_CH0_S  0

/** DMA2D_OUT_ARB_CH0_REG register
 *  Configures the tx arbiter of channel 0
 */
#define DMA2D_OUT_ARB_CH0_REG (DR_REG_DMA2D_BASE + 0x3c)
/** DMA2D_OUT_ARB_TOKEN_NUM_CH0 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define DMA2D_OUT_ARB_TOKEN_NUM_CH0    0x0000000FU
#define DMA2D_OUT_ARB_TOKEN_NUM_CH0_M  (DMA2D_OUT_ARB_TOKEN_NUM_CH0_V << DMA2D_OUT_ARB_TOKEN_NUM_CH0_S)
#define DMA2D_OUT_ARB_TOKEN_NUM_CH0_V  0x0000000FU
#define DMA2D_OUT_ARB_TOKEN_NUM_CH0_S  0
/** DMA2D_OUT_ARB_PRIORITY_CH0 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define DMA2D_OUT_ARB_PRIORITY_CH0    0x00000003U
#define DMA2D_OUT_ARB_PRIORITY_CH0_M  (DMA2D_OUT_ARB_PRIORITY_CH0_V << DMA2D_OUT_ARB_PRIORITY_CH0_S)
#define DMA2D_OUT_ARB_PRIORITY_CH0_V  0x00000003U
#define DMA2D_OUT_ARB_PRIORITY_CH0_S  4

/** DMA2D_OUT_RO_STATUS_CH0_REG register
 *  Represents the status of the tx reorder module of channel 0
 */
#define DMA2D_OUT_RO_STATUS_CH0_REG (DR_REG_DMA2D_BASE + 0x40)
/** DMA2D_OUTFIFO_RO_CNT_CH0 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in color convert Tx FIFO for
 *  channel 0.
 */
#define DMA2D_OUTFIFO_RO_CNT_CH0    0x0000003FU
#define DMA2D_OUTFIFO_RO_CNT_CH0_M  (DMA2D_OUTFIFO_RO_CNT_CH0_V << DMA2D_OUTFIFO_RO_CNT_CH0_S)
#define DMA2D_OUTFIFO_RO_CNT_CH0_V  0x0000003FU
#define DMA2D_OUTFIFO_RO_CNT_CH0_S  0
/** DMA2D_OUT_RO_WR_STATE_CH0 : RO; bitpos: [7:6]; default: 0;
 *  The register stores the state of read ram of reorder
 */
#define DMA2D_OUT_RO_WR_STATE_CH0    0x00000003U
#define DMA2D_OUT_RO_WR_STATE_CH0_M  (DMA2D_OUT_RO_WR_STATE_CH0_V << DMA2D_OUT_RO_WR_STATE_CH0_S)
#define DMA2D_OUT_RO_WR_STATE_CH0_V  0x00000003U
#define DMA2D_OUT_RO_WR_STATE_CH0_S  6
/** DMA2D_OUT_RO_RD_STATE_CH0 : RO; bitpos: [9:8]; default: 0;
 *  The register stores the state of write ram of reorder
 */
#define DMA2D_OUT_RO_RD_STATE_CH0    0x00000003U
#define DMA2D_OUT_RO_RD_STATE_CH0_M  (DMA2D_OUT_RO_RD_STATE_CH0_V << DMA2D_OUT_RO_RD_STATE_CH0_S)
#define DMA2D_OUT_RO_RD_STATE_CH0_V  0x00000003U
#define DMA2D_OUT_RO_RD_STATE_CH0_S  8
/** DMA2D_OUT_PIXEL_BYTE_CH0 : RO; bitpos: [13:10]; default: 0;
 *  the number of bytes contained in a pixel at TX channel     0: 1byte     1: 1.5bytes
 *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
 */
#define DMA2D_OUT_PIXEL_BYTE_CH0    0x0000000FU
#define DMA2D_OUT_PIXEL_BYTE_CH0_M  (DMA2D_OUT_PIXEL_BYTE_CH0_V << DMA2D_OUT_PIXEL_BYTE_CH0_S)
#define DMA2D_OUT_PIXEL_BYTE_CH0_V  0x0000000FU
#define DMA2D_OUT_PIXEL_BYTE_CH0_S  10
/** DMA2D_OUT_BURST_BLOCK_NUM_CH0 : RO; bitpos: [17:14]; default: 0;
 *  the number of macro blocks contained in a burst of data at TX channel
 */
#define DMA2D_OUT_BURST_BLOCK_NUM_CH0    0x0000000FU
#define DMA2D_OUT_BURST_BLOCK_NUM_CH0_M  (DMA2D_OUT_BURST_BLOCK_NUM_CH0_V << DMA2D_OUT_BURST_BLOCK_NUM_CH0_S)
#define DMA2D_OUT_BURST_BLOCK_NUM_CH0_V  0x0000000FU
#define DMA2D_OUT_BURST_BLOCK_NUM_CH0_S  14

/** DMA2D_OUT_RO_PD_CONF_CH0_REG register
 *  Configures the tx reorder memory of channel 0
 */
#define DMA2D_OUT_RO_PD_CONF_CH0_REG (DR_REG_DMA2D_BASE + 0x44)
/** DMA2D_OUT_RO_RAM_FORCE_PD_CH0 : R/W; bitpos: [4]; default: 0;
 *  dma reorder ram power down
 */
#define DMA2D_OUT_RO_RAM_FORCE_PD_CH0    (BIT(4))
#define DMA2D_OUT_RO_RAM_FORCE_PD_CH0_M  (DMA2D_OUT_RO_RAM_FORCE_PD_CH0_V << DMA2D_OUT_RO_RAM_FORCE_PD_CH0_S)
#define DMA2D_OUT_RO_RAM_FORCE_PD_CH0_V  0x00000001U
#define DMA2D_OUT_RO_RAM_FORCE_PD_CH0_S  4
/** DMA2D_OUT_RO_RAM_FORCE_PU_CH0 : R/W; bitpos: [5]; default: 1;
 *  dma reorder ram power up
 */
#define DMA2D_OUT_RO_RAM_FORCE_PU_CH0    (BIT(5))
#define DMA2D_OUT_RO_RAM_FORCE_PU_CH0_M  (DMA2D_OUT_RO_RAM_FORCE_PU_CH0_V << DMA2D_OUT_RO_RAM_FORCE_PU_CH0_S)
#define DMA2D_OUT_RO_RAM_FORCE_PU_CH0_V  0x00000001U
#define DMA2D_OUT_RO_RAM_FORCE_PU_CH0_S  5
/** DMA2D_OUT_RO_RAM_CLK_FO_CH0 : R/W; bitpos: [6]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
 *  0: A gate-clock will be used when accessing the RAM in DMA.
 */
#define DMA2D_OUT_RO_RAM_CLK_FO_CH0    (BIT(6))
#define DMA2D_OUT_RO_RAM_CLK_FO_CH0_M  (DMA2D_OUT_RO_RAM_CLK_FO_CH0_V << DMA2D_OUT_RO_RAM_CLK_FO_CH0_S)
#define DMA2D_OUT_RO_RAM_CLK_FO_CH0_V  0x00000001U
#define DMA2D_OUT_RO_RAM_CLK_FO_CH0_S  6

/** DMA2D_OUT_COLOR_CONVERT_CH0_REG register
 *  Configures the tx color convert of channel 0
 */
#define DMA2D_OUT_COLOR_CONVERT_CH0_REG (DR_REG_DMA2D_BASE + 0x48)
/** DMA2D_OUT_COLOR_OUTPUT_SEL_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  Set final color convert process and output type    0: RGB888 to RGB565     1:
 *  YUV444 to YUV422     2: output directly
 */
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH0    0x00000003U
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH0_M  (DMA2D_OUT_COLOR_OUTPUT_SEL_CH0_V << DMA2D_OUT_COLOR_OUTPUT_SEL_CH0_S)
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH0_V  0x00000003U
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH0_S  0
/** DMA2D_OUT_COLOR_3B_PROC_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Enable generic color convert module between color input & color output, need to
 *  configure parameter.
 */
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH0    (BIT(2))
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH0_M  (DMA2D_OUT_COLOR_3B_PROC_EN_CH0_V << DMA2D_OUT_COLOR_3B_PROC_EN_CH0_S)
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH0_V  0x00000001U
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH0_S  2
/** DMA2D_OUT_COLOR_INPUT_SEL_CH0 : R/W; bitpos: [5:3]; default: 7;
 *  Set first color convert process and input color type    0: RGB565 to RGB888     1:
 *  YUV422 to YUV444      2: Other 2byte/pixel type    3: Other 3byte/pixel type    7:
 *  disable color space convert
 */
#define DMA2D_OUT_COLOR_INPUT_SEL_CH0    0x00000007U
#define DMA2D_OUT_COLOR_INPUT_SEL_CH0_M  (DMA2D_OUT_COLOR_INPUT_SEL_CH0_V << DMA2D_OUT_COLOR_INPUT_SEL_CH0_S)
#define DMA2D_OUT_COLOR_INPUT_SEL_CH0_V  0x00000007U
#define DMA2D_OUT_COLOR_INPUT_SEL_CH0_S  3

/** DMA2D_OUT_SCRAMBLE_CH0_REG register
 *  Configures the tx scramble of channel 0
 */
#define DMA2D_OUT_SCRAMBLE_CH0_REG (DR_REG_DMA2D_BASE + 0x4c)
/** DMA2D_OUT_SCRAMBLE_SEL_PRE_CH0 : R/W; bitpos: [2:0]; default: 0;
 *  Scramble data before color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 :
 *  BYTE1-0-2    3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
 */
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH0    0x00000007U
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH0_M  (DMA2D_OUT_SCRAMBLE_SEL_PRE_CH0_V << DMA2D_OUT_SCRAMBLE_SEL_PRE_CH0_S)
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH0_V  0x00000007U
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH0_S  0

/** DMA2D_OUT_COLOR_PARAM0_CH0_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM0_CH0_REG (DR_REG_DMA2D_BASE + 0x50)
/** DMA2D_OUT_COLOR_PARAM_H0_CH0 : R/W; bitpos: [20:0]; default: 298;
 *  Set first 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_H0_CH0    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_H0_CH0_M  (DMA2D_OUT_COLOR_PARAM_H0_CH0_V << DMA2D_OUT_COLOR_PARAM_H0_CH0_S)
#define DMA2D_OUT_COLOR_PARAM_H0_CH0_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_H0_CH0_S  0

/** DMA2D_OUT_COLOR_PARAM1_CH0_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM1_CH0_REG (DR_REG_DMA2D_BASE + 0x54)
/** DMA2D_OUT_COLOR_PARAM_H1_CH0 : R/W; bitpos: [27:0]; default: 210164121;
 *  Set last 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_H1_CH0    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_H1_CH0_M  (DMA2D_OUT_COLOR_PARAM_H1_CH0_V << DMA2D_OUT_COLOR_PARAM_H1_CH0_S)
#define DMA2D_OUT_COLOR_PARAM_H1_CH0_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_H1_CH0_S  0

/** DMA2D_OUT_COLOR_PARAM2_CH0_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM2_CH0_REG (DR_REG_DMA2D_BASE + 0x58)
/** DMA2D_OUT_COLOR_PARAM_M0_CH0 : R/W; bitpos: [20:0]; default: 1995050;
 *  Set first 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_M0_CH0    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_M0_CH0_M  (DMA2D_OUT_COLOR_PARAM_M0_CH0_V << DMA2D_OUT_COLOR_PARAM_M0_CH0_S)
#define DMA2D_OUT_COLOR_PARAM_M0_CH0_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_M0_CH0_S  0

/** DMA2D_OUT_COLOR_PARAM3_CH0_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM3_CH0_REG (DR_REG_DMA2D_BASE + 0x5c)
/** DMA2D_OUT_COLOR_PARAM_M1_CH0 : R/W; bitpos: [27:0]; default: 35540784;
 *  Set last 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_M1_CH0    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_M1_CH0_M  (DMA2D_OUT_COLOR_PARAM_M1_CH0_V << DMA2D_OUT_COLOR_PARAM_M1_CH0_S)
#define DMA2D_OUT_COLOR_PARAM_M1_CH0_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_M1_CH0_S  0

/** DMA2D_OUT_COLOR_PARAM4_CH0_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM4_CH0_REG (DR_REG_DMA2D_BASE + 0x60)
/** DMA2D_OUT_COLOR_PARAM_L0_CH0 : R/W; bitpos: [20:0]; default: 528682;
 *  Set first 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_L0_CH0    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_L0_CH0_M  (DMA2D_OUT_COLOR_PARAM_L0_CH0_V << DMA2D_OUT_COLOR_PARAM_L0_CH0_S)
#define DMA2D_OUT_COLOR_PARAM_L0_CH0_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_L0_CH0_S  0

/** DMA2D_OUT_COLOR_PARAM5_CH0_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM5_CH0_REG (DR_REG_DMA2D_BASE + 0x64)
/** DMA2D_OUT_COLOR_PARAM_L1_CH0 : R/W; bitpos: [27:0]; default: 195899392;
 *  Set last 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_L1_CH0    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_L1_CH0_M  (DMA2D_OUT_COLOR_PARAM_L1_CH0_V << DMA2D_OUT_COLOR_PARAM_L1_CH0_S)
#define DMA2D_OUT_COLOR_PARAM_L1_CH0_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_L1_CH0_S  0

/** DMA2D_OUT_ETM_CONF_CH0_REG register
 *  Configures the tx etm of channel 0
 */
#define DMA2D_OUT_ETM_CONF_CH0_REG (DR_REG_DMA2D_BASE + 0x68)
/** DMA2D_OUT_ETM_EN_CH0 : R/W; bitpos: [0]; default: 0;
 *  Configures the enable of the etm function, 1 is enable.
 */
#define DMA2D_OUT_ETM_EN_CH0    (BIT(0))
#define DMA2D_OUT_ETM_EN_CH0_M  (DMA2D_OUT_ETM_EN_CH0_V << DMA2D_OUT_ETM_EN_CH0_S)
#define DMA2D_OUT_ETM_EN_CH0_V  0x00000001U
#define DMA2D_OUT_ETM_EN_CH0_S  0
/** DMA2D_OUT_ETM_LOOP_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  Configures the enable of the descriptors loop etm function, 1 is enable.
 */
#define DMA2D_OUT_ETM_LOOP_EN_CH0    (BIT(1))
#define DMA2D_OUT_ETM_LOOP_EN_CH0_M  (DMA2D_OUT_ETM_LOOP_EN_CH0_V << DMA2D_OUT_ETM_LOOP_EN_CH0_S)
#define DMA2D_OUT_ETM_LOOP_EN_CH0_V  0x00000001U
#define DMA2D_OUT_ETM_LOOP_EN_CH0_S  1
/** DMA2D_OUT_DSCR_TASK_MAK_CH0 : R/W; bitpos: [3:2]; default: 1;
 *  Configures the maximum number of cacheable descriptors.
 */
#define DMA2D_OUT_DSCR_TASK_MAK_CH0    0x00000003U
#define DMA2D_OUT_DSCR_TASK_MAK_CH0_M  (DMA2D_OUT_DSCR_TASK_MAK_CH0_V << DMA2D_OUT_DSCR_TASK_MAK_CH0_S)
#define DMA2D_OUT_DSCR_TASK_MAK_CH0_V  0x00000003U
#define DMA2D_OUT_DSCR_TASK_MAK_CH0_S  2

/** DMA2D_OUT_DSCR_PORT_BLK_CH0_REG register
 *  Configures the tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_CH0_REG (DR_REG_DMA2D_BASE + 0x6c)
/** DMA2D_OUT_DSCR_PORT_BLK_H_CH0 : R/W; bitpos: [13:0]; default: 18;
 *  Set the horizontal width of tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH0    0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH0_M  (DMA2D_OUT_DSCR_PORT_BLK_H_CH0_V << DMA2D_OUT_DSCR_PORT_BLK_H_CH0_S)
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH0_V  0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH0_S  0
/** DMA2D_OUT_DSCR_PORT_BLK_V_CH0 : R/W; bitpos: [27:14]; default: 18;
 *  Set the vertical height of tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH0    0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH0_M  (DMA2D_OUT_DSCR_PORT_BLK_V_CH0_V << DMA2D_OUT_DSCR_PORT_BLK_V_CH0_S)
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH0_V  0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH0_S  14

/** DMA2D_OUT_CONF0_CH1_REG register
 *  Configures the tx direction of channel 0
 */
#define DMA2D_OUT_CONF0_CH1_REG (DR_REG_DMA2D_BASE + 0x100)
/** DMA2D_OUT_AUTO_WRBACK_CH1 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define DMA2D_OUT_AUTO_WRBACK_CH1    (BIT(0))
#define DMA2D_OUT_AUTO_WRBACK_CH1_M  (DMA2D_OUT_AUTO_WRBACK_CH1_V << DMA2D_OUT_AUTO_WRBACK_CH1_S)
#define DMA2D_OUT_AUTO_WRBACK_CH1_V  0x00000001U
#define DMA2D_OUT_AUTO_WRBACK_CH1_S  0
/** DMA2D_OUT_EOF_MODE_CH1 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define DMA2D_OUT_EOF_MODE_CH1    (BIT(1))
#define DMA2D_OUT_EOF_MODE_CH1_M  (DMA2D_OUT_EOF_MODE_CH1_V << DMA2D_OUT_EOF_MODE_CH1_S)
#define DMA2D_OUT_EOF_MODE_CH1_V  0x00000001U
#define DMA2D_OUT_EOF_MODE_CH1_S  1
/** DMA2D_OUTDSCR_BURST_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define DMA2D_OUTDSCR_BURST_EN_CH1    (BIT(2))
#define DMA2D_OUTDSCR_BURST_EN_CH1_M  (DMA2D_OUTDSCR_BURST_EN_CH1_V << DMA2D_OUTDSCR_BURST_EN_CH1_S)
#define DMA2D_OUTDSCR_BURST_EN_CH1_V  0x00000001U
#define DMA2D_OUTDSCR_BURST_EN_CH1_S  2
/** DMA2D_OUT_ECC_AES_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define DMA2D_OUT_ECC_AES_EN_CH1    (BIT(3))
#define DMA2D_OUT_ECC_AES_EN_CH1_M  (DMA2D_OUT_ECC_AES_EN_CH1_V << DMA2D_OUT_ECC_AES_EN_CH1_S)
#define DMA2D_OUT_ECC_AES_EN_CH1_V  0x00000001U
#define DMA2D_OUT_ECC_AES_EN_CH1_S  3
/** DMA2D_OUT_CHECK_OWNER_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define DMA2D_OUT_CHECK_OWNER_CH1    (BIT(4))
#define DMA2D_OUT_CHECK_OWNER_CH1_M  (DMA2D_OUT_CHECK_OWNER_CH1_V << DMA2D_OUT_CHECK_OWNER_CH1_S)
#define DMA2D_OUT_CHECK_OWNER_CH1_V  0x00000001U
#define DMA2D_OUT_CHECK_OWNER_CH1_S  4
/** DMA2D_OUT_LOOP_TEST_CH1 : R/W; bitpos: [5]; default: 0;
 *  reserved
 */
#define DMA2D_OUT_LOOP_TEST_CH1    (BIT(5))
#define DMA2D_OUT_LOOP_TEST_CH1_M  (DMA2D_OUT_LOOP_TEST_CH1_V << DMA2D_OUT_LOOP_TEST_CH1_S)
#define DMA2D_OUT_LOOP_TEST_CH1_V  0x00000001U
#define DMA2D_OUT_LOOP_TEST_CH1_S  5
/** DMA2D_OUT_MEM_BURST_LENGTH_CH1 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 0. 0: 8 bytes      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define DMA2D_OUT_MEM_BURST_LENGTH_CH1    0x00000007U
#define DMA2D_OUT_MEM_BURST_LENGTH_CH1_M  (DMA2D_OUT_MEM_BURST_LENGTH_CH1_V << DMA2D_OUT_MEM_BURST_LENGTH_CH1_S)
#define DMA2D_OUT_MEM_BURST_LENGTH_CH1_V  0x00000007U
#define DMA2D_OUT_MEM_BURST_LENGTH_CH1_S  6
/** DMA2D_OUT_MACRO_BLOCK_SIZE_CH1 : R/W; bitpos: [10:9]; default: 0;
 *  Sel TX macro-block size 0: 8pixel*8pixel     1: 8pixel*16pixel     2:
 *  16pixel*16pixel     3: no macro-block     , only useful in mode 1 of the link
 *  descriptor
 */
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH1    0x00000003U
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH1_M  (DMA2D_OUT_MACRO_BLOCK_SIZE_CH1_V << DMA2D_OUT_MACRO_BLOCK_SIZE_CH1_S)
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH1_V  0x00000003U
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH1_S  9
/** DMA2D_OUT_DSCR_PORT_EN_CH1 : R/W; bitpos: [11]; default: 0;
 *  Set this bit to 1 to obtain descriptor from IP port
 */
#define DMA2D_OUT_DSCR_PORT_EN_CH1    (BIT(11))
#define DMA2D_OUT_DSCR_PORT_EN_CH1_M  (DMA2D_OUT_DSCR_PORT_EN_CH1_V << DMA2D_OUT_DSCR_PORT_EN_CH1_S)
#define DMA2D_OUT_DSCR_PORT_EN_CH1_V  0x00000001U
#define DMA2D_OUT_DSCR_PORT_EN_CH1_S  11
/** DMA2D_OUT_PAGE_BOUND_EN_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define DMA2D_OUT_PAGE_BOUND_EN_CH1    (BIT(12))
#define DMA2D_OUT_PAGE_BOUND_EN_CH1_M  (DMA2D_OUT_PAGE_BOUND_EN_CH1_V << DMA2D_OUT_PAGE_BOUND_EN_CH1_S)
#define DMA2D_OUT_PAGE_BOUND_EN_CH1_V  0x00000001U
#define DMA2D_OUT_PAGE_BOUND_EN_CH1_S  12
/** DMA2D_OUT_REORDER_EN_CH1 : R/W; bitpos: [16]; default: 0;
 *  Enable TX channel 0 macro block reorder when set to 1, only channel0 have this
 *  selection
 */
#define DMA2D_OUT_REORDER_EN_CH1    (BIT(16))
#define DMA2D_OUT_REORDER_EN_CH1_M  (DMA2D_OUT_REORDER_EN_CH1_V << DMA2D_OUT_REORDER_EN_CH1_S)
#define DMA2D_OUT_REORDER_EN_CH1_V  0x00000001U
#define DMA2D_OUT_REORDER_EN_CH1_S  16
/** DMA2D_OUT_RST_CH1 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset TX channel
 */
#define DMA2D_OUT_RST_CH1    (BIT(24))
#define DMA2D_OUT_RST_CH1_M  (DMA2D_OUT_RST_CH1_V << DMA2D_OUT_RST_CH1_S)
#define DMA2D_OUT_RST_CH1_V  0x00000001U
#define DMA2D_OUT_RST_CH1_S  24
/** DMA2D_OUT_CMD_DISABLE_CH1 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define DMA2D_OUT_CMD_DISABLE_CH1    (BIT(25))
#define DMA2D_OUT_CMD_DISABLE_CH1_M  (DMA2D_OUT_CMD_DISABLE_CH1_V << DMA2D_OUT_CMD_DISABLE_CH1_S)
#define DMA2D_OUT_CMD_DISABLE_CH1_V  0x00000001U
#define DMA2D_OUT_CMD_DISABLE_CH1_S  25
/** DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH1 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH1    (BIT(26))
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH1_M  (DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH1_V << DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH1_S)
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH1_V  0x00000001U
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH1_S  26

/** DMA2D_OUT_INT_RAW_CH1_REG register
 *  Raw interrupt status of TX channel 0
 */
#define DMA2D_OUT_INT_RAW_CH1_REG (DR_REG_DMA2D_BASE + 0x104)
/** DMA2D_OUT_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define DMA2D_OUT_DONE_CH1_INT_RAW    (BIT(0))
#define DMA2D_OUT_DONE_CH1_INT_RAW_M  (DMA2D_OUT_DONE_CH1_INT_RAW_V << DMA2D_OUT_DONE_CH1_INT_RAW_S)
#define DMA2D_OUT_DONE_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DONE_CH1_INT_RAW_S  0
/** DMA2D_OUT_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define DMA2D_OUT_EOF_CH1_INT_RAW    (BIT(1))
#define DMA2D_OUT_EOF_CH1_INT_RAW_M  (DMA2D_OUT_EOF_CH1_INT_RAW_V << DMA2D_OUT_EOF_CH1_INT_RAW_S)
#define DMA2D_OUT_EOF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUT_EOF_CH1_INT_RAW_S  1
/** DMA2D_OUT_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define DMA2D_OUT_DSCR_ERR_CH1_INT_RAW    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH1_INT_RAW_M  (DMA2D_OUT_DSCR_ERR_CH1_INT_RAW_V << DMA2D_OUT_DSCR_ERR_CH1_INT_RAW_S)
#define DMA2D_OUT_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH1_INT_RAW_S  2
/** DMA2D_OUT_TOTAL_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_RAW    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_RAW_M  (DMA2D_OUT_TOTAL_EOF_CH1_INT_RAW_V << DMA2D_OUT_TOTAL_EOF_CH1_INT_RAW_S)
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_RAW_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_RAW    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L1_CH1_INT_RAW_V << DMA2D_OUTFIFO_OVF_L1_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_RAW_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_RAW    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L1_CH1_INT_RAW_V << DMA2D_OUTFIFO_UDF_L1_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_RAW_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_RAW    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L2_CH1_INT_RAW_V << DMA2D_OUTFIFO_OVF_L2_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_RAW_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_RAW    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L2_CH1_INT_RAW_V << DMA2D_OUTFIFO_UDF_L2_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_RAW_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH1_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_RAW    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L3_CH1_INT_RAW_V << DMA2D_OUTFIFO_OVF_L3_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_RAW_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH1_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_RAW    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L3_CH1_INT_RAW_V << DMA2D_OUTFIFO_UDF_L3_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_RAW_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is overflow.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_RAW    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_RAW_M  (DMA2D_OUTFIFO_RO_OVF_CH1_INT_RAW_V << DMA2D_OUTFIFO_RO_OVF_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_RAW_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is underflow.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_RAW    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_RAW_M  (DMA2D_OUTFIFO_RO_UDF_CH1_INT_RAW_V << DMA2D_OUTFIFO_RO_UDF_CH1_INT_RAW_S)
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_RAW_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_RAW    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_RAW_M  (DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_RAW_V << DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_RAW_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_RAW_S  12

/** DMA2D_OUT_INT_ENA_CH1_REG register
 *  Interrupt enable bits of TX channel 0
 */
#define DMA2D_OUT_INT_ENA_CH1_REG (DR_REG_DMA2D_BASE + 0x108)
/** DMA2D_OUT_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH1_INT_ENA    (BIT(0))
#define DMA2D_OUT_DONE_CH1_INT_ENA_M  (DMA2D_OUT_DONE_CH1_INT_ENA_V << DMA2D_OUT_DONE_CH1_INT_ENA_S)
#define DMA2D_OUT_DONE_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DONE_CH1_INT_ENA_S  0
/** DMA2D_OUT_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH1_INT_ENA    (BIT(1))
#define DMA2D_OUT_EOF_CH1_INT_ENA_M  (DMA2D_OUT_EOF_CH1_INT_ENA_V << DMA2D_OUT_EOF_CH1_INT_ENA_S)
#define DMA2D_OUT_EOF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUT_EOF_CH1_INT_ENA_S  1
/** DMA2D_OUT_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ENA    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ENA_M  (DMA2D_OUT_DSCR_ERR_CH1_INT_ENA_V << DMA2D_OUT_DSCR_ERR_CH1_INT_ENA_S)
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ENA_S  2
/** DMA2D_OUT_TOTAL_EOF_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ENA    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ENA_M  (DMA2D_OUT_TOTAL_EOF_CH1_INT_ENA_V << DMA2D_OUT_TOTAL_EOF_CH1_INT_ENA_S)
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ENA_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ENA    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L1_CH1_INT_ENA_V << DMA2D_OUTFIFO_OVF_L1_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ENA_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ENA    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L1_CH1_INT_ENA_V << DMA2D_OUTFIFO_UDF_L1_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ENA_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ENA    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L2_CH1_INT_ENA_V << DMA2D_OUTFIFO_OVF_L2_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ENA_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH1_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ENA    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L2_CH1_INT_ENA_V << DMA2D_OUTFIFO_UDF_L2_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ENA_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ENA    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L3_CH1_INT_ENA_V << DMA2D_OUTFIFO_OVF_L3_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ENA_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH1_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ENA    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L3_CH1_INT_ENA_V << DMA2D_OUTFIFO_UDF_L3_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ENA_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH1_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ENA    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ENA_M  (DMA2D_OUTFIFO_RO_OVF_CH1_INT_ENA_V << DMA2D_OUTFIFO_RO_OVF_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ENA_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH1_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ENA    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ENA_M  (DMA2D_OUTFIFO_RO_UDF_CH1_INT_ENA_V << DMA2D_OUTFIFO_RO_UDF_CH1_INT_ENA_S)
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ENA_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ENA    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ENA_M  (DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ENA_V << DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ENA_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ENA_S  12

/** DMA2D_OUT_INT_ST_CH1_REG register
 *  Masked interrupt status of TX channel 0
 */
#define DMA2D_OUT_INT_ST_CH1_REG (DR_REG_DMA2D_BASE + 0x10c)
/** DMA2D_OUT_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH1_INT_ST    (BIT(0))
#define DMA2D_OUT_DONE_CH1_INT_ST_M  (DMA2D_OUT_DONE_CH1_INT_ST_V << DMA2D_OUT_DONE_CH1_INT_ST_S)
#define DMA2D_OUT_DONE_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUT_DONE_CH1_INT_ST_S  0
/** DMA2D_OUT_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH1_INT_ST    (BIT(1))
#define DMA2D_OUT_EOF_CH1_INT_ST_M  (DMA2D_OUT_EOF_CH1_INT_ST_V << DMA2D_OUT_EOF_CH1_INT_ST_S)
#define DMA2D_OUT_EOF_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUT_EOF_CH1_INT_ST_S  1
/** DMA2D_OUT_DSCR_ERR_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ST    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ST_M  (DMA2D_OUT_DSCR_ERR_CH1_INT_ST_V << DMA2D_OUT_DSCR_ERR_CH1_INT_ST_S)
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH1_INT_ST_S  2
/** DMA2D_OUT_TOTAL_EOF_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ST    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ST_M  (DMA2D_OUT_TOTAL_EOF_CH1_INT_ST_V << DMA2D_OUT_TOTAL_EOF_CH1_INT_ST_S)
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_ST_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ST    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ST_M  (DMA2D_OUTFIFO_OVF_L1_CH1_INT_ST_V << DMA2D_OUTFIFO_OVF_L1_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_ST_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ST    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ST_M  (DMA2D_OUTFIFO_UDF_L1_CH1_INT_ST_V << DMA2D_OUTFIFO_UDF_L1_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_ST_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ST    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ST_M  (DMA2D_OUTFIFO_OVF_L2_CH1_INT_ST_V << DMA2D_OUTFIFO_OVF_L2_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_ST_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH1_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ST    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ST_M  (DMA2D_OUTFIFO_UDF_L2_CH1_INT_ST_V << DMA2D_OUTFIFO_UDF_L2_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_ST_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ST    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ST_M  (DMA2D_OUTFIFO_OVF_L3_CH1_INT_ST_V << DMA2D_OUTFIFO_OVF_L3_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_ST_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH1_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ST    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ST_M  (DMA2D_OUTFIFO_UDF_L3_CH1_INT_ST_V << DMA2D_OUTFIFO_UDF_L3_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_ST_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH1_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ST    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ST_M  (DMA2D_OUTFIFO_RO_OVF_CH1_INT_ST_V << DMA2D_OUTFIFO_RO_OVF_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_ST_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH1_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ST    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ST_M  (DMA2D_OUTFIFO_RO_UDF_CH1_INT_ST_V << DMA2D_OUTFIFO_RO_UDF_CH1_INT_ST_S)
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_ST_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ST    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ST_M  (DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ST_V << DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ST_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ST_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_ST_S  12

/** DMA2D_OUT_INT_CLR_CH1_REG register
 *  Interrupt clear bits of TX channel 0
 */
#define DMA2D_OUT_INT_CLR_CH1_REG (DR_REG_DMA2D_BASE + 0x110)
/** DMA2D_OUT_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH1_INT_CLR    (BIT(0))
#define DMA2D_OUT_DONE_CH1_INT_CLR_M  (DMA2D_OUT_DONE_CH1_INT_CLR_V << DMA2D_OUT_DONE_CH1_INT_CLR_S)
#define DMA2D_OUT_DONE_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DONE_CH1_INT_CLR_S  0
/** DMA2D_OUT_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH1_INT_CLR    (BIT(1))
#define DMA2D_OUT_EOF_CH1_INT_CLR_M  (DMA2D_OUT_EOF_CH1_INT_CLR_V << DMA2D_OUT_EOF_CH1_INT_CLR_S)
#define DMA2D_OUT_EOF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUT_EOF_CH1_INT_CLR_S  1
/** DMA2D_OUT_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH1_INT_CLR    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH1_INT_CLR_M  (DMA2D_OUT_DSCR_ERR_CH1_INT_CLR_V << DMA2D_OUT_DSCR_ERR_CH1_INT_CLR_S)
#define DMA2D_OUT_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH1_INT_CLR_S  2
/** DMA2D_OUT_TOTAL_EOF_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_CLR    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_CLR_M  (DMA2D_OUT_TOTAL_EOF_CH1_INT_CLR_V << DMA2D_OUT_TOTAL_EOF_CH1_INT_CLR_S)
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH1_INT_CLR_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_CLR    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L1_CH1_INT_CLR_V << DMA2D_OUTFIFO_OVF_L1_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH1_INT_CLR_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_CLR    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L1_CH1_INT_CLR_V << DMA2D_OUTFIFO_UDF_L1_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH1_INT_CLR_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_CLR    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L2_CH1_INT_CLR_V << DMA2D_OUTFIFO_OVF_L2_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH1_INT_CLR_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH1_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_CLR    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L2_CH1_INT_CLR_V << DMA2D_OUTFIFO_UDF_L2_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH1_INT_CLR_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_CLR    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L3_CH1_INT_CLR_V << DMA2D_OUTFIFO_OVF_L3_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH1_INT_CLR_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH1_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_CLR    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L3_CH1_INT_CLR_V << DMA2D_OUTFIFO_UDF_L3_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH1_INT_CLR_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH1_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_CLR    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_CLR_M  (DMA2D_OUTFIFO_RO_OVF_CH1_INT_CLR_V << DMA2D_OUTFIFO_RO_OVF_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH1_INT_CLR_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH1_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_CLR    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_CLR_M  (DMA2D_OUTFIFO_RO_UDF_CH1_INT_CLR_V << DMA2D_OUTFIFO_RO_UDF_CH1_INT_CLR_S)
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH1_INT_CLR_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_CLR    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_CLR_M  (DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_CLR_V << DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_CLR_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH1_INT_CLR_S  12

/** DMA2D_OUTFIFO_STATUS_CH1_REG register
 *  Represents the status of the tx fifo of channel 0
 */
#define DMA2D_OUTFIFO_STATUS_CH1_REG (DR_REG_DMA2D_BASE + 0x114)
/** DMA2D_OUTFIFO_FULL_L2_CH1 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L2_CH1    (BIT(0))
#define DMA2D_OUTFIFO_FULL_L2_CH1_M  (DMA2D_OUTFIFO_FULL_L2_CH1_V << DMA2D_OUTFIFO_FULL_L2_CH1_S)
#define DMA2D_OUTFIFO_FULL_L2_CH1_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L2_CH1_S  0
/** DMA2D_OUTFIFO_EMPTY_L2_CH1 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L2_CH1    (BIT(1))
#define DMA2D_OUTFIFO_EMPTY_L2_CH1_M  (DMA2D_OUTFIFO_EMPTY_L2_CH1_V << DMA2D_OUTFIFO_EMPTY_L2_CH1_S)
#define DMA2D_OUTFIFO_EMPTY_L2_CH1_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L2_CH1_S  1
/** DMA2D_OUTFIFO_CNT_L2_CH1 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L2_CH1    0x0000000FU
#define DMA2D_OUTFIFO_CNT_L2_CH1_M  (DMA2D_OUTFIFO_CNT_L2_CH1_V << DMA2D_OUTFIFO_CNT_L2_CH1_S)
#define DMA2D_OUTFIFO_CNT_L2_CH1_V  0x0000000FU
#define DMA2D_OUTFIFO_CNT_L2_CH1_S  2
/** DMA2D_OUT_REMAIN_UNDER_1B_CH1 : RO; bitpos: [7]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_1B_CH1    (BIT(7))
#define DMA2D_OUT_REMAIN_UNDER_1B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_1B_CH1_V << DMA2D_OUT_REMAIN_UNDER_1B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_1B_CH1_S  7
/** DMA2D_OUT_REMAIN_UNDER_2B_CH1 : RO; bitpos: [8]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_2B_CH1    (BIT(8))
#define DMA2D_OUT_REMAIN_UNDER_2B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_2B_CH1_V << DMA2D_OUT_REMAIN_UNDER_2B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_2B_CH1_S  8
/** DMA2D_OUT_REMAIN_UNDER_3B_CH1 : RO; bitpos: [9]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_3B_CH1    (BIT(9))
#define DMA2D_OUT_REMAIN_UNDER_3B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_3B_CH1_V << DMA2D_OUT_REMAIN_UNDER_3B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_3B_CH1_S  9
/** DMA2D_OUT_REMAIN_UNDER_4B_CH1 : RO; bitpos: [10]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_4B_CH1    (BIT(10))
#define DMA2D_OUT_REMAIN_UNDER_4B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_4B_CH1_V << DMA2D_OUT_REMAIN_UNDER_4B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_4B_CH1_S  10
/** DMA2D_OUT_REMAIN_UNDER_5B_CH1 : RO; bitpos: [11]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_5B_CH1    (BIT(11))
#define DMA2D_OUT_REMAIN_UNDER_5B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_5B_CH1_V << DMA2D_OUT_REMAIN_UNDER_5B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_5B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_5B_CH1_S  11
/** DMA2D_OUT_REMAIN_UNDER_6B_CH1 : RO; bitpos: [12]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_6B_CH1    (BIT(12))
#define DMA2D_OUT_REMAIN_UNDER_6B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_6B_CH1_V << DMA2D_OUT_REMAIN_UNDER_6B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_6B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_6B_CH1_S  12
/** DMA2D_OUT_REMAIN_UNDER_7B_CH1 : RO; bitpos: [13]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_7B_CH1    (BIT(13))
#define DMA2D_OUT_REMAIN_UNDER_7B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_7B_CH1_V << DMA2D_OUT_REMAIN_UNDER_7B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_7B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_7B_CH1_S  13
/** DMA2D_OUT_REMAIN_UNDER_8B_CH1 : RO; bitpos: [14]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_8B_CH1    (BIT(14))
#define DMA2D_OUT_REMAIN_UNDER_8B_CH1_M  (DMA2D_OUT_REMAIN_UNDER_8B_CH1_V << DMA2D_OUT_REMAIN_UNDER_8B_CH1_S)
#define DMA2D_OUT_REMAIN_UNDER_8B_CH1_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_8B_CH1_S  14
/** DMA2D_OUTFIFO_FULL_L1_CH1 : RO; bitpos: [15]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L1_CH1    (BIT(15))
#define DMA2D_OUTFIFO_FULL_L1_CH1_M  (DMA2D_OUTFIFO_FULL_L1_CH1_V << DMA2D_OUTFIFO_FULL_L1_CH1_S)
#define DMA2D_OUTFIFO_FULL_L1_CH1_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L1_CH1_S  15
/** DMA2D_OUTFIFO_EMPTY_L1_CH1 : RO; bitpos: [16]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L1_CH1    (BIT(16))
#define DMA2D_OUTFIFO_EMPTY_L1_CH1_M  (DMA2D_OUTFIFO_EMPTY_L1_CH1_V << DMA2D_OUTFIFO_EMPTY_L1_CH1_S)
#define DMA2D_OUTFIFO_EMPTY_L1_CH1_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L1_CH1_S  16
/** DMA2D_OUTFIFO_CNT_L1_CH1 : RO; bitpos: [21:17]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L1_CH1    0x0000001FU
#define DMA2D_OUTFIFO_CNT_L1_CH1_M  (DMA2D_OUTFIFO_CNT_L1_CH1_V << DMA2D_OUTFIFO_CNT_L1_CH1_S)
#define DMA2D_OUTFIFO_CNT_L1_CH1_V  0x0000001FU
#define DMA2D_OUTFIFO_CNT_L1_CH1_S  17
/** DMA2D_OUTFIFO_FULL_L3_CH1 : RO; bitpos: [22]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L3_CH1    (BIT(22))
#define DMA2D_OUTFIFO_FULL_L3_CH1_M  (DMA2D_OUTFIFO_FULL_L3_CH1_V << DMA2D_OUTFIFO_FULL_L3_CH1_S)
#define DMA2D_OUTFIFO_FULL_L3_CH1_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L3_CH1_S  22
/** DMA2D_OUTFIFO_EMPTY_L3_CH1 : RO; bitpos: [23]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L3_CH1    (BIT(23))
#define DMA2D_OUTFIFO_EMPTY_L3_CH1_M  (DMA2D_OUTFIFO_EMPTY_L3_CH1_V << DMA2D_OUTFIFO_EMPTY_L3_CH1_S)
#define DMA2D_OUTFIFO_EMPTY_L3_CH1_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L3_CH1_S  23
/** DMA2D_OUTFIFO_CNT_L3_CH1 : RO; bitpos: [28:24]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L3_CH1    0x0000001FU
#define DMA2D_OUTFIFO_CNT_L3_CH1_M  (DMA2D_OUTFIFO_CNT_L3_CH1_V << DMA2D_OUTFIFO_CNT_L3_CH1_S)
#define DMA2D_OUTFIFO_CNT_L3_CH1_V  0x0000001FU
#define DMA2D_OUTFIFO_CNT_L3_CH1_S  24

/** DMA2D_OUT_PUSH_CH1_REG register
 *  Configures the tx fifo of channel 0
 */
#define DMA2D_OUT_PUSH_CH1_REG (DR_REG_DMA2D_BASE + 0x118)
/** DMA2D_OUTFIFO_WDATA_CH1 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define DMA2D_OUTFIFO_WDATA_CH1    0x000003FFU
#define DMA2D_OUTFIFO_WDATA_CH1_M  (DMA2D_OUTFIFO_WDATA_CH1_V << DMA2D_OUTFIFO_WDATA_CH1_S)
#define DMA2D_OUTFIFO_WDATA_CH1_V  0x000003FFU
#define DMA2D_OUTFIFO_WDATA_CH1_S  0
/** DMA2D_OUTFIFO_PUSH_CH1 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define DMA2D_OUTFIFO_PUSH_CH1    (BIT(10))
#define DMA2D_OUTFIFO_PUSH_CH1_M  (DMA2D_OUTFIFO_PUSH_CH1_V << DMA2D_OUTFIFO_PUSH_CH1_S)
#define DMA2D_OUTFIFO_PUSH_CH1_V  0x00000001U
#define DMA2D_OUTFIFO_PUSH_CH1_S  10

/** DMA2D_OUT_LINK_CONF_CH1_REG register
 *  Configures the tx descriptor operations of channel 0
 */
#define DMA2D_OUT_LINK_CONF_CH1_REG (DR_REG_DMA2D_BASE + 0x11c)
/** DMA2D_OUTLINK_STOP_CH1 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define DMA2D_OUTLINK_STOP_CH1    (BIT(20))
#define DMA2D_OUTLINK_STOP_CH1_M  (DMA2D_OUTLINK_STOP_CH1_V << DMA2D_OUTLINK_STOP_CH1_S)
#define DMA2D_OUTLINK_STOP_CH1_V  0x00000001U
#define DMA2D_OUTLINK_STOP_CH1_S  20
/** DMA2D_OUTLINK_START_CH1 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define DMA2D_OUTLINK_START_CH1    (BIT(21))
#define DMA2D_OUTLINK_START_CH1_M  (DMA2D_OUTLINK_START_CH1_V << DMA2D_OUTLINK_START_CH1_S)
#define DMA2D_OUTLINK_START_CH1_V  0x00000001U
#define DMA2D_OUTLINK_START_CH1_S  21
/** DMA2D_OUTLINK_RESTART_CH1 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define DMA2D_OUTLINK_RESTART_CH1    (BIT(22))
#define DMA2D_OUTLINK_RESTART_CH1_M  (DMA2D_OUTLINK_RESTART_CH1_V << DMA2D_OUTLINK_RESTART_CH1_S)
#define DMA2D_OUTLINK_RESTART_CH1_V  0x00000001U
#define DMA2D_OUTLINK_RESTART_CH1_S  22
/** DMA2D_OUTLINK_PARK_CH1 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define DMA2D_OUTLINK_PARK_CH1    (BIT(23))
#define DMA2D_OUTLINK_PARK_CH1_M  (DMA2D_OUTLINK_PARK_CH1_V << DMA2D_OUTLINK_PARK_CH1_S)
#define DMA2D_OUTLINK_PARK_CH1_V  0x00000001U
#define DMA2D_OUTLINK_PARK_CH1_S  23

/** DMA2D_OUT_LINK_ADDR_CH1_REG register
 *  Configures the tx descriptor address of channel 0
 */
#define DMA2D_OUT_LINK_ADDR_CH1_REG (DR_REG_DMA2D_BASE + 0x120)
/** DMA2D_OUTLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define DMA2D_OUTLINK_ADDR_CH1    0xFFFFFFFFU
#define DMA2D_OUTLINK_ADDR_CH1_M  (DMA2D_OUTLINK_ADDR_CH1_V << DMA2D_OUTLINK_ADDR_CH1_S)
#define DMA2D_OUTLINK_ADDR_CH1_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_ADDR_CH1_S  0

/** DMA2D_OUT_STATE_CH1_REG register
 *  Represents the working status of the tx descriptor of channel 0
 */
#define DMA2D_OUT_STATE_CH1_REG (DR_REG_DMA2D_BASE + 0x124)
/** DMA2D_OUTLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define DMA2D_OUTLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define DMA2D_OUTLINK_DSCR_ADDR_CH1_M  (DMA2D_OUTLINK_DSCR_ADDR_CH1_V << DMA2D_OUTLINK_DSCR_ADDR_CH1_S)
#define DMA2D_OUTLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define DMA2D_OUTLINK_DSCR_ADDR_CH1_S  0
/** DMA2D_OUT_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define DMA2D_OUT_DSCR_STATE_CH1    0x00000003U
#define DMA2D_OUT_DSCR_STATE_CH1_M  (DMA2D_OUT_DSCR_STATE_CH1_V << DMA2D_OUT_DSCR_STATE_CH1_S)
#define DMA2D_OUT_DSCR_STATE_CH1_V  0x00000003U
#define DMA2D_OUT_DSCR_STATE_CH1_S  18
/** DMA2D_OUT_STATE_CH1 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define DMA2D_OUT_STATE_CH1    0x0000000FU
#define DMA2D_OUT_STATE_CH1_M  (DMA2D_OUT_STATE_CH1_V << DMA2D_OUT_STATE_CH1_S)
#define DMA2D_OUT_STATE_CH1_V  0x0000000FU
#define DMA2D_OUT_STATE_CH1_S  20
/** DMA2D_OUT_RESET_AVAIL_CH1 : RO; bitpos: [24]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define DMA2D_OUT_RESET_AVAIL_CH1    (BIT(24))
#define DMA2D_OUT_RESET_AVAIL_CH1_M  (DMA2D_OUT_RESET_AVAIL_CH1_V << DMA2D_OUT_RESET_AVAIL_CH1_S)
#define DMA2D_OUT_RESET_AVAIL_CH1_V  0x00000001U
#define DMA2D_OUT_RESET_AVAIL_CH1_S  24

/** DMA2D_OUT_EOF_DES_ADDR_CH1_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_EOF_DES_ADDR_CH1_REG (DR_REG_DMA2D_BASE + 0x128)
/** DMA2D_OUT_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define DMA2D_OUT_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define DMA2D_OUT_EOF_DES_ADDR_CH1_M  (DMA2D_OUT_EOF_DES_ADDR_CH1_V << DMA2D_OUT_EOF_DES_ADDR_CH1_S)
#define DMA2D_OUT_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define DMA2D_OUT_EOF_DES_ADDR_CH1_S  0

/** DMA2D_OUT_DSCR_CH1_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_CH1_REG (DR_REG_DMA2D_BASE + 0x12c)
/** DMA2D_OUTLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define DMA2D_OUTLINK_DSCR_CH1    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_CH1_M  (DMA2D_OUTLINK_DSCR_CH1_V << DMA2D_OUTLINK_DSCR_CH1_S)
#define DMA2D_OUTLINK_DSCR_CH1_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_CH1_S  0

/** DMA2D_OUT_DSCR_BF0_CH1_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_BF0_CH1_REG (DR_REG_DMA2D_BASE + 0x130)
/** DMA2D_OUTLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define DMA2D_OUTLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF0_CH1_M  (DMA2D_OUTLINK_DSCR_BF0_CH1_V << DMA2D_OUTLINK_DSCR_BF0_CH1_S)
#define DMA2D_OUTLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF0_CH1_S  0

/** DMA2D_OUT_DSCR_BF1_CH1_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_BF1_CH1_REG (DR_REG_DMA2D_BASE + 0x134)
/** DMA2D_OUTLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define DMA2D_OUTLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF1_CH1_M  (DMA2D_OUTLINK_DSCR_BF1_CH1_V << DMA2D_OUTLINK_DSCR_BF1_CH1_S)
#define DMA2D_OUTLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF1_CH1_S  0

/** DMA2D_OUT_PERI_SEL_CH1_REG register
 *  Configures the tx peripheral of channel 0
 */
#define DMA2D_OUT_PERI_SEL_CH1_REG (DR_REG_DMA2D_BASE + 0x138)
/** DMA2D_OUT_PERI_SEL_CH1 : R/W; bitpos: [2:0]; default: 7;
 *  This register is used to select peripheral for Tx channel   0:  jpeg     1:
 *  display-1     2: display-2     3: display-3      7: no choose
 */
#define DMA2D_OUT_PERI_SEL_CH1    0x00000007U
#define DMA2D_OUT_PERI_SEL_CH1_M  (DMA2D_OUT_PERI_SEL_CH1_V << DMA2D_OUT_PERI_SEL_CH1_S)
#define DMA2D_OUT_PERI_SEL_CH1_V  0x00000007U
#define DMA2D_OUT_PERI_SEL_CH1_S  0

/** DMA2D_OUT_ARB_CH1_REG register
 *  Configures the tx arbiter of channel 0
 */
#define DMA2D_OUT_ARB_CH1_REG (DR_REG_DMA2D_BASE + 0x13c)
/** DMA2D_OUT_ARB_TOKEN_NUM_CH1 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define DMA2D_OUT_ARB_TOKEN_NUM_CH1    0x0000000FU
#define DMA2D_OUT_ARB_TOKEN_NUM_CH1_M  (DMA2D_OUT_ARB_TOKEN_NUM_CH1_V << DMA2D_OUT_ARB_TOKEN_NUM_CH1_S)
#define DMA2D_OUT_ARB_TOKEN_NUM_CH1_V  0x0000000FU
#define DMA2D_OUT_ARB_TOKEN_NUM_CH1_S  0
/** DMA2D_OUT_ARB_PRIORITY_CH1 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define DMA2D_OUT_ARB_PRIORITY_CH1    0x00000003U
#define DMA2D_OUT_ARB_PRIORITY_CH1_M  (DMA2D_OUT_ARB_PRIORITY_CH1_V << DMA2D_OUT_ARB_PRIORITY_CH1_S)
#define DMA2D_OUT_ARB_PRIORITY_CH1_V  0x00000003U
#define DMA2D_OUT_ARB_PRIORITY_CH1_S  4

/** DMA2D_OUT_RO_STATUS_CH1_REG register
 *  Represents the status of the tx reorder module of channel 0
 */
#define DMA2D_OUT_RO_STATUS_CH1_REG (DR_REG_DMA2D_BASE + 0x140)
/** DMA2D_OUTFIFO_RO_CNT_CH1 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in color convert Tx FIFO for
 *  channel 0.
 */
#define DMA2D_OUTFIFO_RO_CNT_CH1    0x0000003FU
#define DMA2D_OUTFIFO_RO_CNT_CH1_M  (DMA2D_OUTFIFO_RO_CNT_CH1_V << DMA2D_OUTFIFO_RO_CNT_CH1_S)
#define DMA2D_OUTFIFO_RO_CNT_CH1_V  0x0000003FU
#define DMA2D_OUTFIFO_RO_CNT_CH1_S  0
/** DMA2D_OUT_RO_WR_STATE_CH1 : RO; bitpos: [7:6]; default: 0;
 *  The register stores the state of read ram of reorder
 */
#define DMA2D_OUT_RO_WR_STATE_CH1    0x00000003U
#define DMA2D_OUT_RO_WR_STATE_CH1_M  (DMA2D_OUT_RO_WR_STATE_CH1_V << DMA2D_OUT_RO_WR_STATE_CH1_S)
#define DMA2D_OUT_RO_WR_STATE_CH1_V  0x00000003U
#define DMA2D_OUT_RO_WR_STATE_CH1_S  6
/** DMA2D_OUT_RO_RD_STATE_CH1 : RO; bitpos: [9:8]; default: 0;
 *  The register stores the state of write ram of reorder
 */
#define DMA2D_OUT_RO_RD_STATE_CH1    0x00000003U
#define DMA2D_OUT_RO_RD_STATE_CH1_M  (DMA2D_OUT_RO_RD_STATE_CH1_V << DMA2D_OUT_RO_RD_STATE_CH1_S)
#define DMA2D_OUT_RO_RD_STATE_CH1_V  0x00000003U
#define DMA2D_OUT_RO_RD_STATE_CH1_S  8
/** DMA2D_OUT_PIXEL_BYTE_CH1 : RO; bitpos: [13:10]; default: 0;
 *  the number of bytes contained in a pixel at TX channel     0: 1byte     1: 1.5bytes
 *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
 */
#define DMA2D_OUT_PIXEL_BYTE_CH1    0x0000000FU
#define DMA2D_OUT_PIXEL_BYTE_CH1_M  (DMA2D_OUT_PIXEL_BYTE_CH1_V << DMA2D_OUT_PIXEL_BYTE_CH1_S)
#define DMA2D_OUT_PIXEL_BYTE_CH1_V  0x0000000FU
#define DMA2D_OUT_PIXEL_BYTE_CH1_S  10
/** DMA2D_OUT_BURST_BLOCK_NUM_CH1 : RO; bitpos: [17:14]; default: 0;
 *  the number of macro blocks contained in a burst of data at TX channel
 */
#define DMA2D_OUT_BURST_BLOCK_NUM_CH1    0x0000000FU
#define DMA2D_OUT_BURST_BLOCK_NUM_CH1_M  (DMA2D_OUT_BURST_BLOCK_NUM_CH1_V << DMA2D_OUT_BURST_BLOCK_NUM_CH1_S)
#define DMA2D_OUT_BURST_BLOCK_NUM_CH1_V  0x0000000FU
#define DMA2D_OUT_BURST_BLOCK_NUM_CH1_S  14

/** DMA2D_OUT_COLOR_CONVERT_CH1_REG register
 *  Configures the tx color convert of channel 0
 */
#define DMA2D_OUT_COLOR_CONVERT_CH1_REG (DR_REG_DMA2D_BASE + 0x148)
/** DMA2D_OUT_COLOR_OUTPUT_SEL_CH1 : R/W; bitpos: [1:0]; default: 0;
 *  Set final color convert process and output type    0: RGB888 to RGB565     1:
 *  YUV444 to YUV422     2: output directly
 */
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH1    0x00000003U
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH1_M  (DMA2D_OUT_COLOR_OUTPUT_SEL_CH1_V << DMA2D_OUT_COLOR_OUTPUT_SEL_CH1_S)
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH1_V  0x00000003U
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH1_S  0
/** DMA2D_OUT_COLOR_3B_PROC_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Enable generic color convert module between color input & color output, need to
 *  configure parameter.
 */
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH1    (BIT(2))
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH1_M  (DMA2D_OUT_COLOR_3B_PROC_EN_CH1_V << DMA2D_OUT_COLOR_3B_PROC_EN_CH1_S)
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH1_V  0x00000001U
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH1_S  2
/** DMA2D_OUT_COLOR_INPUT_SEL_CH1 : R/W; bitpos: [5:3]; default: 7;
 *  Set first color convert process and input color type    0: RGB565 to RGB888     1:
 *  YUV422 to YUV444      2: Other 2byte/pixel type    3: Other 3byte/pixel type    7:
 *  disable color space convert
 */
#define DMA2D_OUT_COLOR_INPUT_SEL_CH1    0x00000007U
#define DMA2D_OUT_COLOR_INPUT_SEL_CH1_M  (DMA2D_OUT_COLOR_INPUT_SEL_CH1_V << DMA2D_OUT_COLOR_INPUT_SEL_CH1_S)
#define DMA2D_OUT_COLOR_INPUT_SEL_CH1_V  0x00000007U
#define DMA2D_OUT_COLOR_INPUT_SEL_CH1_S  3

/** DMA2D_OUT_SCRAMBLE_CH1_REG register
 *  Configures the tx scramble of channel 0
 */
#define DMA2D_OUT_SCRAMBLE_CH1_REG (DR_REG_DMA2D_BASE + 0x14c)
/** DMA2D_OUT_SCRAMBLE_SEL_PRE_CH1 : R/W; bitpos: [2:0]; default: 0;
 *  Scramble data before color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 :
 *  BYTE1-0-2    3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
 */
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH1    0x00000007U
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH1_M  (DMA2D_OUT_SCRAMBLE_SEL_PRE_CH1_V << DMA2D_OUT_SCRAMBLE_SEL_PRE_CH1_S)
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH1_V  0x00000007U
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH1_S  0

/** DMA2D_OUT_COLOR_PARAM0_CH1_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM0_CH1_REG (DR_REG_DMA2D_BASE + 0x150)
/** DMA2D_OUT_COLOR_PARAM_H0_CH1 : R/W; bitpos: [20:0]; default: 298;
 *  Set first 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_H0_CH1    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_H0_CH1_M  (DMA2D_OUT_COLOR_PARAM_H0_CH1_V << DMA2D_OUT_COLOR_PARAM_H0_CH1_S)
#define DMA2D_OUT_COLOR_PARAM_H0_CH1_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_H0_CH1_S  0

/** DMA2D_OUT_COLOR_PARAM1_CH1_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM1_CH1_REG (DR_REG_DMA2D_BASE + 0x154)
/** DMA2D_OUT_COLOR_PARAM_H1_CH1 : R/W; bitpos: [27:0]; default: 210164121;
 *  Set last 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_H1_CH1    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_H1_CH1_M  (DMA2D_OUT_COLOR_PARAM_H1_CH1_V << DMA2D_OUT_COLOR_PARAM_H1_CH1_S)
#define DMA2D_OUT_COLOR_PARAM_H1_CH1_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_H1_CH1_S  0

/** DMA2D_OUT_COLOR_PARAM2_CH1_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM2_CH1_REG (DR_REG_DMA2D_BASE + 0x158)
/** DMA2D_OUT_COLOR_PARAM_M0_CH1 : R/W; bitpos: [20:0]; default: 1995050;
 *  Set first 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_M0_CH1    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_M0_CH1_M  (DMA2D_OUT_COLOR_PARAM_M0_CH1_V << DMA2D_OUT_COLOR_PARAM_M0_CH1_S)
#define DMA2D_OUT_COLOR_PARAM_M0_CH1_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_M0_CH1_S  0

/** DMA2D_OUT_COLOR_PARAM3_CH1_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM3_CH1_REG (DR_REG_DMA2D_BASE + 0x15c)
/** DMA2D_OUT_COLOR_PARAM_M1_CH1 : R/W; bitpos: [27:0]; default: 35540784;
 *  Set last 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_M1_CH1    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_M1_CH1_M  (DMA2D_OUT_COLOR_PARAM_M1_CH1_V << DMA2D_OUT_COLOR_PARAM_M1_CH1_S)
#define DMA2D_OUT_COLOR_PARAM_M1_CH1_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_M1_CH1_S  0

/** DMA2D_OUT_COLOR_PARAM4_CH1_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM4_CH1_REG (DR_REG_DMA2D_BASE + 0x160)
/** DMA2D_OUT_COLOR_PARAM_L0_CH1 : R/W; bitpos: [20:0]; default: 528682;
 *  Set first 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_L0_CH1    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_L0_CH1_M  (DMA2D_OUT_COLOR_PARAM_L0_CH1_V << DMA2D_OUT_COLOR_PARAM_L0_CH1_S)
#define DMA2D_OUT_COLOR_PARAM_L0_CH1_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_L0_CH1_S  0

/** DMA2D_OUT_COLOR_PARAM5_CH1_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM5_CH1_REG (DR_REG_DMA2D_BASE + 0x164)
/** DMA2D_OUT_COLOR_PARAM_L1_CH1 : R/W; bitpos: [27:0]; default: 195899392;
 *  Set last 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_L1_CH1    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_L1_CH1_M  (DMA2D_OUT_COLOR_PARAM_L1_CH1_V << DMA2D_OUT_COLOR_PARAM_L1_CH1_S)
#define DMA2D_OUT_COLOR_PARAM_L1_CH1_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_L1_CH1_S  0

/** DMA2D_OUT_ETM_CONF_CH1_REG register
 *  Configures the tx etm of channel 0
 */
#define DMA2D_OUT_ETM_CONF_CH1_REG (DR_REG_DMA2D_BASE + 0x168)
/** DMA2D_OUT_ETM_EN_CH1 : R/W; bitpos: [0]; default: 0;
 *  Configures the enable of the etm function, 1 is enable.
 */
#define DMA2D_OUT_ETM_EN_CH1    (BIT(0))
#define DMA2D_OUT_ETM_EN_CH1_M  (DMA2D_OUT_ETM_EN_CH1_V << DMA2D_OUT_ETM_EN_CH1_S)
#define DMA2D_OUT_ETM_EN_CH1_V  0x00000001U
#define DMA2D_OUT_ETM_EN_CH1_S  0
/** DMA2D_OUT_ETM_LOOP_EN_CH1 : R/W; bitpos: [1]; default: 0;
 *  Configures the enable of the descriptors loop etm function, 1 is enable.
 */
#define DMA2D_OUT_ETM_LOOP_EN_CH1    (BIT(1))
#define DMA2D_OUT_ETM_LOOP_EN_CH1_M  (DMA2D_OUT_ETM_LOOP_EN_CH1_V << DMA2D_OUT_ETM_LOOP_EN_CH1_S)
#define DMA2D_OUT_ETM_LOOP_EN_CH1_V  0x00000001U
#define DMA2D_OUT_ETM_LOOP_EN_CH1_S  1
/** DMA2D_OUT_DSCR_TASK_MAK_CH1 : R/W; bitpos: [3:2]; default: 1;
 *  Configures the maximum number of cacheable descriptors.
 */
#define DMA2D_OUT_DSCR_TASK_MAK_CH1    0x00000003U
#define DMA2D_OUT_DSCR_TASK_MAK_CH1_M  (DMA2D_OUT_DSCR_TASK_MAK_CH1_V << DMA2D_OUT_DSCR_TASK_MAK_CH1_S)
#define DMA2D_OUT_DSCR_TASK_MAK_CH1_V  0x00000003U
#define DMA2D_OUT_DSCR_TASK_MAK_CH1_S  2

/** DMA2D_OUT_DSCR_PORT_BLK_CH1_REG register
 *  Configures the tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_CH1_REG (DR_REG_DMA2D_BASE + 0x16c)
/** DMA2D_OUT_DSCR_PORT_BLK_H_CH1 : R/W; bitpos: [13:0]; default: 18;
 *  Set the horizontal width of tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH1    0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH1_M  (DMA2D_OUT_DSCR_PORT_BLK_H_CH1_V << DMA2D_OUT_DSCR_PORT_BLK_H_CH1_S)
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH1_V  0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH1_S  0
/** DMA2D_OUT_DSCR_PORT_BLK_V_CH1 : R/W; bitpos: [27:14]; default: 18;
 *  Set the vertical height of tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH1    0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH1_M  (DMA2D_OUT_DSCR_PORT_BLK_V_CH1_V << DMA2D_OUT_DSCR_PORT_BLK_V_CH1_S)
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH1_V  0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH1_S  14

/** DMA2D_OUT_CONF0_CH2_REG register
 *  Configures the tx direction of channel 0
 */
#define DMA2D_OUT_CONF0_CH2_REG (DR_REG_DMA2D_BASE + 0x200)
/** DMA2D_OUT_AUTO_WRBACK_CH2 : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data pointed by
 *  outlink descriptor has been received.
 */
#define DMA2D_OUT_AUTO_WRBACK_CH2    (BIT(0))
#define DMA2D_OUT_AUTO_WRBACK_CH2_M  (DMA2D_OUT_AUTO_WRBACK_CH2_V << DMA2D_OUT_AUTO_WRBACK_CH2_S)
#define DMA2D_OUT_AUTO_WRBACK_CH2_V  0x00000001U
#define DMA2D_OUT_AUTO_WRBACK_CH2_S  0
/** DMA2D_OUT_EOF_MODE_CH2 : R/W; bitpos: [1]; default: 1;
 *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to read has been popped from FIFO in DMA
 */
#define DMA2D_OUT_EOF_MODE_CH2    (BIT(1))
#define DMA2D_OUT_EOF_MODE_CH2_M  (DMA2D_OUT_EOF_MODE_CH2_V << DMA2D_OUT_EOF_MODE_CH2_S)
#define DMA2D_OUT_EOF_MODE_CH2_V  0x00000001U
#define DMA2D_OUT_EOF_MODE_CH2_S  1
/** DMA2D_OUTDSCR_BURST_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define DMA2D_OUTDSCR_BURST_EN_CH2    (BIT(2))
#define DMA2D_OUTDSCR_BURST_EN_CH2_M  (DMA2D_OUTDSCR_BURST_EN_CH2_V << DMA2D_OUTDSCR_BURST_EN_CH2_S)
#define DMA2D_OUTDSCR_BURST_EN_CH2_V  0x00000001U
#define DMA2D_OUTDSCR_BURST_EN_CH2_S  2
/** DMA2D_OUT_ECC_AES_EN_CH2 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define DMA2D_OUT_ECC_AES_EN_CH2    (BIT(3))
#define DMA2D_OUT_ECC_AES_EN_CH2_M  (DMA2D_OUT_ECC_AES_EN_CH2_V << DMA2D_OUT_ECC_AES_EN_CH2_S)
#define DMA2D_OUT_ECC_AES_EN_CH2_V  0x00000001U
#define DMA2D_OUT_ECC_AES_EN_CH2_S  3
/** DMA2D_OUT_CHECK_OWNER_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define DMA2D_OUT_CHECK_OWNER_CH2    (BIT(4))
#define DMA2D_OUT_CHECK_OWNER_CH2_M  (DMA2D_OUT_CHECK_OWNER_CH2_V << DMA2D_OUT_CHECK_OWNER_CH2_S)
#define DMA2D_OUT_CHECK_OWNER_CH2_V  0x00000001U
#define DMA2D_OUT_CHECK_OWNER_CH2_S  4
/** DMA2D_OUT_LOOP_TEST_CH2 : R/W; bitpos: [5]; default: 0;
 *  reserved
 */
#define DMA2D_OUT_LOOP_TEST_CH2    (BIT(5))
#define DMA2D_OUT_LOOP_TEST_CH2_M  (DMA2D_OUT_LOOP_TEST_CH2_V << DMA2D_OUT_LOOP_TEST_CH2_S)
#define DMA2D_OUT_LOOP_TEST_CH2_V  0x00000001U
#define DMA2D_OUT_LOOP_TEST_CH2_S  5
/** DMA2D_OUT_MEM_BURST_LENGTH_CH2 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Tx channel 0. 0: 8 bytes      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define DMA2D_OUT_MEM_BURST_LENGTH_CH2    0x00000007U
#define DMA2D_OUT_MEM_BURST_LENGTH_CH2_M  (DMA2D_OUT_MEM_BURST_LENGTH_CH2_V << DMA2D_OUT_MEM_BURST_LENGTH_CH2_S)
#define DMA2D_OUT_MEM_BURST_LENGTH_CH2_V  0x00000007U
#define DMA2D_OUT_MEM_BURST_LENGTH_CH2_S  6
/** DMA2D_OUT_MACRO_BLOCK_SIZE_CH2 : R/W; bitpos: [10:9]; default: 0;
 *  Sel TX macro-block size 0: 8pixel*8pixel     1: 8pixel*16pixel     2:
 *  16pixel*16pixel     3: no macro-block     , only useful in mode 1 of the link
 *  descriptor
 */
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH2    0x00000003U
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH2_M  (DMA2D_OUT_MACRO_BLOCK_SIZE_CH2_V << DMA2D_OUT_MACRO_BLOCK_SIZE_CH2_S)
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH2_V  0x00000003U
#define DMA2D_OUT_MACRO_BLOCK_SIZE_CH2_S  9
/** DMA2D_OUT_DSCR_PORT_EN_CH2 : R/W; bitpos: [11]; default: 0;
 *  Set this bit to 1 to obtain descriptor from IP port
 */
#define DMA2D_OUT_DSCR_PORT_EN_CH2    (BIT(11))
#define DMA2D_OUT_DSCR_PORT_EN_CH2_M  (DMA2D_OUT_DSCR_PORT_EN_CH2_V << DMA2D_OUT_DSCR_PORT_EN_CH2_S)
#define DMA2D_OUT_DSCR_PORT_EN_CH2_V  0x00000001U
#define DMA2D_OUT_DSCR_PORT_EN_CH2_S  11
/** DMA2D_OUT_PAGE_BOUND_EN_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
 *  define by mem_burst_length
 */
#define DMA2D_OUT_PAGE_BOUND_EN_CH2    (BIT(12))
#define DMA2D_OUT_PAGE_BOUND_EN_CH2_M  (DMA2D_OUT_PAGE_BOUND_EN_CH2_V << DMA2D_OUT_PAGE_BOUND_EN_CH2_S)
#define DMA2D_OUT_PAGE_BOUND_EN_CH2_V  0x00000001U
#define DMA2D_OUT_PAGE_BOUND_EN_CH2_S  12
/** DMA2D_OUT_REORDER_EN_CH2 : R/W; bitpos: [16]; default: 0;
 *  Enable TX channel 0 macro block reorder when set to 1, only channel0 have this
 *  selection
 */
#define DMA2D_OUT_REORDER_EN_CH2    (BIT(16))
#define DMA2D_OUT_REORDER_EN_CH2_M  (DMA2D_OUT_REORDER_EN_CH2_V << DMA2D_OUT_REORDER_EN_CH2_S)
#define DMA2D_OUT_REORDER_EN_CH2_V  0x00000001U
#define DMA2D_OUT_REORDER_EN_CH2_S  16
/** DMA2D_OUT_RST_CH2 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset TX channel
 */
#define DMA2D_OUT_RST_CH2    (BIT(24))
#define DMA2D_OUT_RST_CH2_M  (DMA2D_OUT_RST_CH2_V << DMA2D_OUT_RST_CH2_S)
#define DMA2D_OUT_RST_CH2_V  0x00000001U
#define DMA2D_OUT_RST_CH2_S  24
/** DMA2D_OUT_CMD_DISABLE_CH2 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define DMA2D_OUT_CMD_DISABLE_CH2    (BIT(25))
#define DMA2D_OUT_CMD_DISABLE_CH2_M  (DMA2D_OUT_CMD_DISABLE_CH2_V << DMA2D_OUT_CMD_DISABLE_CH2_S)
#define DMA2D_OUT_CMD_DISABLE_CH2_V  0x00000001U
#define DMA2D_OUT_CMD_DISABLE_CH2_S  25
/** DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH2 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH2    (BIT(26))
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH2_M  (DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH2_V << DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH2_S)
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH2_V  0x00000001U
#define DMA2D_OUT_ARB_WEIGHT_OPT_DIS_CH2_S  26

/** DMA2D_OUT_INT_RAW_CH2_REG register
 *  Raw interrupt status of TX channel 0
 */
#define DMA2D_OUT_INT_RAW_CH2_REG (DR_REG_DMA2D_BASE + 0x204)
/** DMA2D_OUT_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define DMA2D_OUT_DONE_CH2_INT_RAW    (BIT(0))
#define DMA2D_OUT_DONE_CH2_INT_RAW_M  (DMA2D_OUT_DONE_CH2_INT_RAW_V << DMA2D_OUT_DONE_CH2_INT_RAW_S)
#define DMA2D_OUT_DONE_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DONE_CH2_INT_RAW_S  0
/** DMA2D_OUT_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define DMA2D_OUT_EOF_CH2_INT_RAW    (BIT(1))
#define DMA2D_OUT_EOF_CH2_INT_RAW_M  (DMA2D_OUT_EOF_CH2_INT_RAW_V << DMA2D_OUT_EOF_CH2_INT_RAW_S)
#define DMA2D_OUT_EOF_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUT_EOF_CH2_INT_RAW_S  1
/** DMA2D_OUT_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
 *  including owner error, the second and third word error of outlink descriptor for Tx
 *  channel 0.
 */
#define DMA2D_OUT_DSCR_ERR_CH2_INT_RAW    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH2_INT_RAW_M  (DMA2D_OUT_DSCR_ERR_CH2_INT_RAW_V << DMA2D_OUT_DSCR_ERR_CH2_INT_RAW_S)
#define DMA2D_OUT_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH2_INT_RAW_S  2
/** DMA2D_OUT_TOTAL_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_RAW    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_RAW_M  (DMA2D_OUT_TOTAL_EOF_CH2_INT_RAW_V << DMA2D_OUT_TOTAL_EOF_CH2_INT_RAW_S)
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_RAW_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_RAW    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L1_CH2_INT_RAW_V << DMA2D_OUTFIFO_OVF_L1_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_RAW_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_RAW    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L1_CH2_INT_RAW_V << DMA2D_OUTFIFO_UDF_L1_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_RAW_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_RAW    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L2_CH2_INT_RAW_V << DMA2D_OUTFIFO_OVF_L2_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_RAW_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH2_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_RAW    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L2_CH2_INT_RAW_V << DMA2D_OUTFIFO_UDF_L2_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_RAW_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH2_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is overflow.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_RAW    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_RAW_M  (DMA2D_OUTFIFO_OVF_L3_CH2_INT_RAW_V << DMA2D_OUTFIFO_OVF_L3_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_RAW_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH2_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when fifo is underflow.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_RAW    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_RAW_M  (DMA2D_OUTFIFO_UDF_L3_CH2_INT_RAW_V << DMA2D_OUTFIFO_UDF_L3_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_RAW_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is overflow.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_RAW    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_RAW_M  (DMA2D_OUTFIFO_RO_OVF_CH2_INT_RAW_V << DMA2D_OUTFIFO_RO_OVF_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_RAW_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is underflow.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_RAW    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_RAW_M  (DMA2D_OUTFIFO_RO_UDF_CH2_INT_RAW_V << DMA2D_OUTFIFO_RO_UDF_CH2_INT_RAW_S)
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_RAW_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_RAW    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_RAW_M  (DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_RAW_V << DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_RAW_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_RAW_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_RAW_S  12

/** DMA2D_OUT_INT_ENA_CH2_REG register
 *  Interrupt enable bits of TX channel 0
 */
#define DMA2D_OUT_INT_ENA_CH2_REG (DR_REG_DMA2D_BASE + 0x208)
/** DMA2D_OUT_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH2_INT_ENA    (BIT(0))
#define DMA2D_OUT_DONE_CH2_INT_ENA_M  (DMA2D_OUT_DONE_CH2_INT_ENA_V << DMA2D_OUT_DONE_CH2_INT_ENA_S)
#define DMA2D_OUT_DONE_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DONE_CH2_INT_ENA_S  0
/** DMA2D_OUT_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH2_INT_ENA    (BIT(1))
#define DMA2D_OUT_EOF_CH2_INT_ENA_M  (DMA2D_OUT_EOF_CH2_INT_ENA_V << DMA2D_OUT_EOF_CH2_INT_ENA_S)
#define DMA2D_OUT_EOF_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUT_EOF_CH2_INT_ENA_S  1
/** DMA2D_OUT_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ENA    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ENA_M  (DMA2D_OUT_DSCR_ERR_CH2_INT_ENA_V << DMA2D_OUT_DSCR_ERR_CH2_INT_ENA_S)
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ENA_S  2
/** DMA2D_OUT_TOTAL_EOF_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ENA    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ENA_M  (DMA2D_OUT_TOTAL_EOF_CH2_INT_ENA_V << DMA2D_OUT_TOTAL_EOF_CH2_INT_ENA_S)
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ENA_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ENA    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L1_CH2_INT_ENA_V << DMA2D_OUTFIFO_OVF_L1_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ENA_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ENA    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L1_CH2_INT_ENA_V << DMA2D_OUTFIFO_UDF_L1_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ENA_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ENA    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L2_CH2_INT_ENA_V << DMA2D_OUTFIFO_OVF_L2_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ENA_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH2_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ENA    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L2_CH2_INT_ENA_V << DMA2D_OUTFIFO_UDF_L2_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ENA_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH2_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ENA    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ENA_M  (DMA2D_OUTFIFO_OVF_L3_CH2_INT_ENA_V << DMA2D_OUTFIFO_OVF_L3_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ENA_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH2_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ENA    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ENA_M  (DMA2D_OUTFIFO_UDF_L3_CH2_INT_ENA_V << DMA2D_OUTFIFO_UDF_L3_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ENA_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH2_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ENA    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ENA_M  (DMA2D_OUTFIFO_RO_OVF_CH2_INT_ENA_V << DMA2D_OUTFIFO_RO_OVF_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ENA_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH2_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ENA    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ENA_M  (DMA2D_OUTFIFO_RO_UDF_CH2_INT_ENA_V << DMA2D_OUTFIFO_RO_UDF_CH2_INT_ENA_S)
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ENA_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ENA    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ENA_M  (DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ENA_V << DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ENA_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ENA_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ENA_S  12

/** DMA2D_OUT_INT_ST_CH2_REG register
 *  Masked interrupt status of TX channel 0
 */
#define DMA2D_OUT_INT_ST_CH2_REG (DR_REG_DMA2D_BASE + 0x20c)
/** DMA2D_OUT_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH2_INT_ST    (BIT(0))
#define DMA2D_OUT_DONE_CH2_INT_ST_M  (DMA2D_OUT_DONE_CH2_INT_ST_V << DMA2D_OUT_DONE_CH2_INT_ST_S)
#define DMA2D_OUT_DONE_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUT_DONE_CH2_INT_ST_S  0
/** DMA2D_OUT_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH2_INT_ST    (BIT(1))
#define DMA2D_OUT_EOF_CH2_INT_ST_M  (DMA2D_OUT_EOF_CH2_INT_ST_V << DMA2D_OUT_EOF_CH2_INT_ST_S)
#define DMA2D_OUT_EOF_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUT_EOF_CH2_INT_ST_S  1
/** DMA2D_OUT_DSCR_ERR_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ST    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ST_M  (DMA2D_OUT_DSCR_ERR_CH2_INT_ST_V << DMA2D_OUT_DSCR_ERR_CH2_INT_ST_S)
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH2_INT_ST_S  2
/** DMA2D_OUT_TOTAL_EOF_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ST    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ST_M  (DMA2D_OUT_TOTAL_EOF_CH2_INT_ST_V << DMA2D_OUT_TOTAL_EOF_CH2_INT_ST_S)
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_ST_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ST    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ST_M  (DMA2D_OUTFIFO_OVF_L1_CH2_INT_ST_V << DMA2D_OUTFIFO_OVF_L1_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_ST_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ST    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ST_M  (DMA2D_OUTFIFO_UDF_L1_CH2_INT_ST_V << DMA2D_OUTFIFO_UDF_L1_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_ST_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ST    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ST_M  (DMA2D_OUTFIFO_OVF_L2_CH2_INT_ST_V << DMA2D_OUTFIFO_OVF_L2_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_ST_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH2_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ST    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ST_M  (DMA2D_OUTFIFO_UDF_L2_CH2_INT_ST_V << DMA2D_OUTFIFO_UDF_L2_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_ST_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH2_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ST    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ST_M  (DMA2D_OUTFIFO_OVF_L3_CH2_INT_ST_V << DMA2D_OUTFIFO_OVF_L3_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_ST_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH2_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ST    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ST_M  (DMA2D_OUTFIFO_UDF_L3_CH2_INT_ST_V << DMA2D_OUTFIFO_UDF_L3_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_ST_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH2_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ST    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ST_M  (DMA2D_OUTFIFO_RO_OVF_CH2_INT_ST_V << DMA2D_OUTFIFO_RO_OVF_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_ST_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH2_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ST    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ST_M  (DMA2D_OUTFIFO_RO_UDF_CH2_INT_ST_V << DMA2D_OUTFIFO_RO_UDF_CH2_INT_ST_S)
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_ST_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ST    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ST_M  (DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ST_V << DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ST_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ST_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_ST_S  12

/** DMA2D_OUT_INT_CLR_CH2_REG register
 *  Interrupt clear bits of TX channel 0
 */
#define DMA2D_OUT_INT_CLR_CH2_REG (DR_REG_DMA2D_BASE + 0x210)
/** DMA2D_OUT_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define DMA2D_OUT_DONE_CH2_INT_CLR    (BIT(0))
#define DMA2D_OUT_DONE_CH2_INT_CLR_M  (DMA2D_OUT_DONE_CH2_INT_CLR_V << DMA2D_OUT_DONE_CH2_INT_CLR_S)
#define DMA2D_OUT_DONE_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DONE_CH2_INT_CLR_S  0
/** DMA2D_OUT_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_EOF_CH2_INT_CLR    (BIT(1))
#define DMA2D_OUT_EOF_CH2_INT_CLR_M  (DMA2D_OUT_EOF_CH2_INT_CLR_V << DMA2D_OUT_EOF_CH2_INT_CLR_S)
#define DMA2D_OUT_EOF_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUT_EOF_CH2_INT_CLR_S  1
/** DMA2D_OUT_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_ERR_CH2_INT_CLR    (BIT(2))
#define DMA2D_OUT_DSCR_ERR_CH2_INT_CLR_M  (DMA2D_OUT_DSCR_ERR_CH2_INT_CLR_V << DMA2D_OUT_DSCR_ERR_CH2_INT_CLR_S)
#define DMA2D_OUT_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DSCR_ERR_CH2_INT_CLR_S  2
/** DMA2D_OUT_TOTAL_EOF_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_CLR    (BIT(3))
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_CLR_M  (DMA2D_OUT_TOTAL_EOF_CH2_INT_CLR_V << DMA2D_OUT_TOTAL_EOF_CH2_INT_CLR_S)
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUT_TOTAL_EOF_CH2_INT_CLR_S  3
/** DMA2D_OUTFIFO_OVF_L1_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_CLR    (BIT(4))
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L1_CH2_INT_CLR_V << DMA2D_OUTFIFO_OVF_L1_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L1_CH2_INT_CLR_S  4
/** DMA2D_OUTFIFO_UDF_L1_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_CLR    (BIT(5))
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L1_CH2_INT_CLR_V << DMA2D_OUTFIFO_UDF_L1_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L1_CH2_INT_CLR_S  5
/** DMA2D_OUTFIFO_OVF_L2_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_CLR    (BIT(6))
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L2_CH2_INT_CLR_V << DMA2D_OUTFIFO_OVF_L2_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L2_CH2_INT_CLR_S  6
/** DMA2D_OUTFIFO_UDF_L2_CH2_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_CLR    (BIT(7))
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L2_CH2_INT_CLR_V << DMA2D_OUTFIFO_UDF_L2_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L2_CH2_INT_CLR_S  7
/** DMA2D_OUTFIFO_OVF_L3_CH2_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_CLR    (BIT(8))
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_CLR_M  (DMA2D_OUTFIFO_OVF_L3_CH2_INT_CLR_V << DMA2D_OUTFIFO_OVF_L3_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_OVF_L3_CH2_INT_CLR_S  8
/** DMA2D_OUTFIFO_UDF_L3_CH2_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_CLR    (BIT(9))
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_CLR_M  (DMA2D_OUTFIFO_UDF_L3_CH2_INT_CLR_V << DMA2D_OUTFIFO_UDF_L3_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_UDF_L3_CH2_INT_CLR_S  9
/** DMA2D_OUTFIFO_RO_OVF_CH2_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the OUTFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_CLR    (BIT(10))
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_CLR_M  (DMA2D_OUTFIFO_RO_OVF_CH2_INT_CLR_V << DMA2D_OUTFIFO_RO_OVF_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_RO_OVF_CH2_INT_CLR_S  10
/** DMA2D_OUTFIFO_RO_UDF_CH2_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the OUTFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_CLR    (BIT(11))
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_CLR_M  (DMA2D_OUTFIFO_RO_UDF_CH2_INT_CLR_V << DMA2D_OUTFIFO_RO_UDF_CH2_INT_CLR_S)
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUTFIFO_RO_UDF_CH2_INT_CLR_S  11
/** DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_CLR    (BIT(12))
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_CLR_M  (DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_CLR_V << DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_CLR_S)
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_CLR_V  0x00000001U
#define DMA2D_OUT_DSCR_TASK_OVF_CH2_INT_CLR_S  12

/** DMA2D_OUTFIFO_STATUS_CH2_REG register
 *  Represents the status of the tx fifo of channel 0
 */
#define DMA2D_OUTFIFO_STATUS_CH2_REG (DR_REG_DMA2D_BASE + 0x214)
/** DMA2D_OUTFIFO_FULL_L2_CH2 : RO; bitpos: [0]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L2_CH2    (BIT(0))
#define DMA2D_OUTFIFO_FULL_L2_CH2_M  (DMA2D_OUTFIFO_FULL_L2_CH2_V << DMA2D_OUTFIFO_FULL_L2_CH2_S)
#define DMA2D_OUTFIFO_FULL_L2_CH2_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L2_CH2_S  0
/** DMA2D_OUTFIFO_EMPTY_L2_CH2 : RO; bitpos: [1]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L2_CH2    (BIT(1))
#define DMA2D_OUTFIFO_EMPTY_L2_CH2_M  (DMA2D_OUTFIFO_EMPTY_L2_CH2_V << DMA2D_OUTFIFO_EMPTY_L2_CH2_S)
#define DMA2D_OUTFIFO_EMPTY_L2_CH2_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L2_CH2_S  1
/** DMA2D_OUTFIFO_CNT_L2_CH2 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L2_CH2    0x0000000FU
#define DMA2D_OUTFIFO_CNT_L2_CH2_M  (DMA2D_OUTFIFO_CNT_L2_CH2_V << DMA2D_OUTFIFO_CNT_L2_CH2_S)
#define DMA2D_OUTFIFO_CNT_L2_CH2_V  0x0000000FU
#define DMA2D_OUTFIFO_CNT_L2_CH2_S  2
/** DMA2D_OUT_REMAIN_UNDER_1B_CH2 : RO; bitpos: [7]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_1B_CH2    (BIT(7))
#define DMA2D_OUT_REMAIN_UNDER_1B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_1B_CH2_V << DMA2D_OUT_REMAIN_UNDER_1B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_1B_CH2_S  7
/** DMA2D_OUT_REMAIN_UNDER_2B_CH2 : RO; bitpos: [8]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_2B_CH2    (BIT(8))
#define DMA2D_OUT_REMAIN_UNDER_2B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_2B_CH2_V << DMA2D_OUT_REMAIN_UNDER_2B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_2B_CH2_S  8
/** DMA2D_OUT_REMAIN_UNDER_3B_CH2 : RO; bitpos: [9]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_3B_CH2    (BIT(9))
#define DMA2D_OUT_REMAIN_UNDER_3B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_3B_CH2_V << DMA2D_OUT_REMAIN_UNDER_3B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_3B_CH2_S  9
/** DMA2D_OUT_REMAIN_UNDER_4B_CH2 : RO; bitpos: [10]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_4B_CH2    (BIT(10))
#define DMA2D_OUT_REMAIN_UNDER_4B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_4B_CH2_V << DMA2D_OUT_REMAIN_UNDER_4B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_4B_CH2_S  10
/** DMA2D_OUT_REMAIN_UNDER_5B_CH2 : RO; bitpos: [11]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_5B_CH2    (BIT(11))
#define DMA2D_OUT_REMAIN_UNDER_5B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_5B_CH2_V << DMA2D_OUT_REMAIN_UNDER_5B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_5B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_5B_CH2_S  11
/** DMA2D_OUT_REMAIN_UNDER_6B_CH2 : RO; bitpos: [12]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_6B_CH2    (BIT(12))
#define DMA2D_OUT_REMAIN_UNDER_6B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_6B_CH2_V << DMA2D_OUT_REMAIN_UNDER_6B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_6B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_6B_CH2_S  12
/** DMA2D_OUT_REMAIN_UNDER_7B_CH2 : RO; bitpos: [13]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_7B_CH2    (BIT(13))
#define DMA2D_OUT_REMAIN_UNDER_7B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_7B_CH2_V << DMA2D_OUT_REMAIN_UNDER_7B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_7B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_7B_CH2_S  13
/** DMA2D_OUT_REMAIN_UNDER_8B_CH2 : RO; bitpos: [14]; default: 1;
 *  reserved
 */
#define DMA2D_OUT_REMAIN_UNDER_8B_CH2    (BIT(14))
#define DMA2D_OUT_REMAIN_UNDER_8B_CH2_M  (DMA2D_OUT_REMAIN_UNDER_8B_CH2_V << DMA2D_OUT_REMAIN_UNDER_8B_CH2_S)
#define DMA2D_OUT_REMAIN_UNDER_8B_CH2_V  0x00000001U
#define DMA2D_OUT_REMAIN_UNDER_8B_CH2_S  14
/** DMA2D_OUTFIFO_FULL_L1_CH2 : RO; bitpos: [15]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L1_CH2    (BIT(15))
#define DMA2D_OUTFIFO_FULL_L1_CH2_M  (DMA2D_OUTFIFO_FULL_L1_CH2_V << DMA2D_OUTFIFO_FULL_L1_CH2_S)
#define DMA2D_OUTFIFO_FULL_L1_CH2_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L1_CH2_S  15
/** DMA2D_OUTFIFO_EMPTY_L1_CH2 : RO; bitpos: [16]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L1_CH2    (BIT(16))
#define DMA2D_OUTFIFO_EMPTY_L1_CH2_M  (DMA2D_OUTFIFO_EMPTY_L1_CH2_V << DMA2D_OUTFIFO_EMPTY_L1_CH2_S)
#define DMA2D_OUTFIFO_EMPTY_L1_CH2_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L1_CH2_S  16
/** DMA2D_OUTFIFO_CNT_L1_CH2 : RO; bitpos: [21:17]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L1_CH2    0x0000001FU
#define DMA2D_OUTFIFO_CNT_L1_CH2_M  (DMA2D_OUTFIFO_CNT_L1_CH2_V << DMA2D_OUTFIFO_CNT_L1_CH2_S)
#define DMA2D_OUTFIFO_CNT_L1_CH2_V  0x0000001FU
#define DMA2D_OUTFIFO_CNT_L1_CH2_S  17
/** DMA2D_OUTFIFO_FULL_L3_CH2 : RO; bitpos: [22]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_FULL_L3_CH2    (BIT(22))
#define DMA2D_OUTFIFO_FULL_L3_CH2_M  (DMA2D_OUTFIFO_FULL_L3_CH2_V << DMA2D_OUTFIFO_FULL_L3_CH2_S)
#define DMA2D_OUTFIFO_FULL_L3_CH2_V  0x00000001U
#define DMA2D_OUTFIFO_FULL_L3_CH2_S  22
/** DMA2D_OUTFIFO_EMPTY_L3_CH2 : RO; bitpos: [23]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_OUTFIFO_EMPTY_L3_CH2    (BIT(23))
#define DMA2D_OUTFIFO_EMPTY_L3_CH2_M  (DMA2D_OUTFIFO_EMPTY_L3_CH2_V << DMA2D_OUTFIFO_EMPTY_L3_CH2_S)
#define DMA2D_OUTFIFO_EMPTY_L3_CH2_V  0x00000001U
#define DMA2D_OUTFIFO_EMPTY_L3_CH2_S  23
/** DMA2D_OUTFIFO_CNT_L3_CH2 : RO; bitpos: [28:24]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_OUTFIFO_CNT_L3_CH2    0x0000001FU
#define DMA2D_OUTFIFO_CNT_L3_CH2_M  (DMA2D_OUTFIFO_CNT_L3_CH2_V << DMA2D_OUTFIFO_CNT_L3_CH2_S)
#define DMA2D_OUTFIFO_CNT_L3_CH2_V  0x0000001FU
#define DMA2D_OUTFIFO_CNT_L3_CH2_S  24

/** DMA2D_OUT_PUSH_CH2_REG register
 *  Configures the tx fifo of channel 0
 */
#define DMA2D_OUT_PUSH_CH2_REG (DR_REG_DMA2D_BASE + 0x218)
/** DMA2D_OUTFIFO_WDATA_CH2 : R/W; bitpos: [9:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA Tx FIFO.
 */
#define DMA2D_OUTFIFO_WDATA_CH2    0x000003FFU
#define DMA2D_OUTFIFO_WDATA_CH2_M  (DMA2D_OUTFIFO_WDATA_CH2_V << DMA2D_OUTFIFO_WDATA_CH2_S)
#define DMA2D_OUTFIFO_WDATA_CH2_V  0x000003FFU
#define DMA2D_OUTFIFO_WDATA_CH2_S  0
/** DMA2D_OUTFIFO_PUSH_CH2 : R/W/SC; bitpos: [10]; default: 0;
 *  Set this bit to push data into DMA Tx FIFO.
 */
#define DMA2D_OUTFIFO_PUSH_CH2    (BIT(10))
#define DMA2D_OUTFIFO_PUSH_CH2_M  (DMA2D_OUTFIFO_PUSH_CH2_V << DMA2D_OUTFIFO_PUSH_CH2_S)
#define DMA2D_OUTFIFO_PUSH_CH2_V  0x00000001U
#define DMA2D_OUTFIFO_PUSH_CH2_S  10

/** DMA2D_OUT_LINK_CONF_CH2_REG register
 *  Configures the tx descriptor operations of channel 0
 */
#define DMA2D_OUT_LINK_CONF_CH2_REG (DR_REG_DMA2D_BASE + 0x21c)
/** DMA2D_OUTLINK_STOP_CH2 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define DMA2D_OUTLINK_STOP_CH2    (BIT(20))
#define DMA2D_OUTLINK_STOP_CH2_M  (DMA2D_OUTLINK_STOP_CH2_V << DMA2D_OUTLINK_STOP_CH2_S)
#define DMA2D_OUTLINK_STOP_CH2_V  0x00000001U
#define DMA2D_OUTLINK_STOP_CH2_S  20
/** DMA2D_OUTLINK_START_CH2 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define DMA2D_OUTLINK_START_CH2    (BIT(21))
#define DMA2D_OUTLINK_START_CH2_M  (DMA2D_OUTLINK_START_CH2_V << DMA2D_OUTLINK_START_CH2_S)
#define DMA2D_OUTLINK_START_CH2_V  0x00000001U
#define DMA2D_OUTLINK_START_CH2_S  21
/** DMA2D_OUTLINK_RESTART_CH2 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define DMA2D_OUTLINK_RESTART_CH2    (BIT(22))
#define DMA2D_OUTLINK_RESTART_CH2_M  (DMA2D_OUTLINK_RESTART_CH2_V << DMA2D_OUTLINK_RESTART_CH2_S)
#define DMA2D_OUTLINK_RESTART_CH2_V  0x00000001U
#define DMA2D_OUTLINK_RESTART_CH2_S  22
/** DMA2D_OUTLINK_PARK_CH2 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define DMA2D_OUTLINK_PARK_CH2    (BIT(23))
#define DMA2D_OUTLINK_PARK_CH2_M  (DMA2D_OUTLINK_PARK_CH2_V << DMA2D_OUTLINK_PARK_CH2_S)
#define DMA2D_OUTLINK_PARK_CH2_V  0x00000001U
#define DMA2D_OUTLINK_PARK_CH2_S  23

/** DMA2D_OUT_LINK_ADDR_CH2_REG register
 *  Configures the tx descriptor address of channel 0
 */
#define DMA2D_OUT_LINK_ADDR_CH2_REG (DR_REG_DMA2D_BASE + 0x220)
/** DMA2D_OUTLINK_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first outlink descriptor's address.
 */
#define DMA2D_OUTLINK_ADDR_CH2    0xFFFFFFFFU
#define DMA2D_OUTLINK_ADDR_CH2_M  (DMA2D_OUTLINK_ADDR_CH2_V << DMA2D_OUTLINK_ADDR_CH2_S)
#define DMA2D_OUTLINK_ADDR_CH2_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_ADDR_CH2_S  0

/** DMA2D_OUT_STATE_CH2_REG register
 *  Represents the working status of the tx descriptor of channel 0
 */
#define DMA2D_OUT_STATE_CH2_REG (DR_REG_DMA2D_BASE + 0x224)
/** DMA2D_OUTLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define DMA2D_OUTLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define DMA2D_OUTLINK_DSCR_ADDR_CH2_M  (DMA2D_OUTLINK_DSCR_ADDR_CH2_V << DMA2D_OUTLINK_DSCR_ADDR_CH2_S)
#define DMA2D_OUTLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define DMA2D_OUTLINK_DSCR_ADDR_CH2_S  0
/** DMA2D_OUT_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  This register stores the current descriptor state machine state.
 */
#define DMA2D_OUT_DSCR_STATE_CH2    0x00000003U
#define DMA2D_OUT_DSCR_STATE_CH2_M  (DMA2D_OUT_DSCR_STATE_CH2_V << DMA2D_OUT_DSCR_STATE_CH2_S)
#define DMA2D_OUT_DSCR_STATE_CH2_V  0x00000003U
#define DMA2D_OUT_DSCR_STATE_CH2_S  18
/** DMA2D_OUT_STATE_CH2 : RO; bitpos: [23:20]; default: 0;
 *  This register stores the current control module state machine state.
 */
#define DMA2D_OUT_STATE_CH2    0x0000000FU
#define DMA2D_OUT_STATE_CH2_M  (DMA2D_OUT_STATE_CH2_V << DMA2D_OUT_STATE_CH2_S)
#define DMA2D_OUT_STATE_CH2_V  0x0000000FU
#define DMA2D_OUT_STATE_CH2_S  20
/** DMA2D_OUT_RESET_AVAIL_CH2 : RO; bitpos: [24]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define DMA2D_OUT_RESET_AVAIL_CH2    (BIT(24))
#define DMA2D_OUT_RESET_AVAIL_CH2_M  (DMA2D_OUT_RESET_AVAIL_CH2_V << DMA2D_OUT_RESET_AVAIL_CH2_S)
#define DMA2D_OUT_RESET_AVAIL_CH2_V  0x00000001U
#define DMA2D_OUT_RESET_AVAIL_CH2_S  24

/** DMA2D_OUT_EOF_DES_ADDR_CH2_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_EOF_DES_ADDR_CH2_REG (DR_REG_DMA2D_BASE + 0x228)
/** DMA2D_OUT_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define DMA2D_OUT_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define DMA2D_OUT_EOF_DES_ADDR_CH2_M  (DMA2D_OUT_EOF_DES_ADDR_CH2_V << DMA2D_OUT_EOF_DES_ADDR_CH2_S)
#define DMA2D_OUT_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define DMA2D_OUT_EOF_DES_ADDR_CH2_S  0

/** DMA2D_OUT_DSCR_CH2_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_CH2_REG (DR_REG_DMA2D_BASE + 0x22c)
/** DMA2D_OUTLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next outlink descriptor address y.
 */
#define DMA2D_OUTLINK_DSCR_CH2    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_CH2_M  (DMA2D_OUTLINK_DSCR_CH2_V << DMA2D_OUTLINK_DSCR_CH2_S)
#define DMA2D_OUTLINK_DSCR_CH2_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_CH2_S  0

/** DMA2D_OUT_DSCR_BF0_CH2_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_BF0_CH2_REG (DR_REG_DMA2D_BASE + 0x230)
/** DMA2D_OUTLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor's next address y-1.
 */
#define DMA2D_OUTLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF0_CH2_M  (DMA2D_OUTLINK_DSCR_BF0_CH2_V << DMA2D_OUTLINK_DSCR_BF0_CH2_S)
#define DMA2D_OUTLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF0_CH2_S  0

/** DMA2D_OUT_DSCR_BF1_CH2_REG register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
#define DMA2D_OUT_DSCR_BF1_CH2_REG (DR_REG_DMA2D_BASE + 0x234)
/** DMA2D_OUTLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor's next address y-2.
 */
#define DMA2D_OUTLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF1_CH2_M  (DMA2D_OUTLINK_DSCR_BF1_CH2_V << DMA2D_OUTLINK_DSCR_BF1_CH2_S)
#define DMA2D_OUTLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define DMA2D_OUTLINK_DSCR_BF1_CH2_S  0

/** DMA2D_OUT_PERI_SEL_CH2_REG register
 *  Configures the tx peripheral of channel 0
 */
#define DMA2D_OUT_PERI_SEL_CH2_REG (DR_REG_DMA2D_BASE + 0x238)
/** DMA2D_OUT_PERI_SEL_CH2 : R/W; bitpos: [2:0]; default: 7;
 *  This register is used to select peripheral for Tx channel   0:  jpeg     1:
 *  display-1     2: display-2     3: display-3      7: no choose
 */
#define DMA2D_OUT_PERI_SEL_CH2    0x00000007U
#define DMA2D_OUT_PERI_SEL_CH2_M  (DMA2D_OUT_PERI_SEL_CH2_V << DMA2D_OUT_PERI_SEL_CH2_S)
#define DMA2D_OUT_PERI_SEL_CH2_V  0x00000007U
#define DMA2D_OUT_PERI_SEL_CH2_S  0

/** DMA2D_OUT_ARB_CH2_REG register
 *  Configures the tx arbiter of channel 0
 */
#define DMA2D_OUT_ARB_CH2_REG (DR_REG_DMA2D_BASE + 0x23c)
/** DMA2D_OUT_ARB_TOKEN_NUM_CH2 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define DMA2D_OUT_ARB_TOKEN_NUM_CH2    0x0000000FU
#define DMA2D_OUT_ARB_TOKEN_NUM_CH2_M  (DMA2D_OUT_ARB_TOKEN_NUM_CH2_V << DMA2D_OUT_ARB_TOKEN_NUM_CH2_S)
#define DMA2D_OUT_ARB_TOKEN_NUM_CH2_V  0x0000000FU
#define DMA2D_OUT_ARB_TOKEN_NUM_CH2_S  0
/** DMA2D_OUT_ARB_PRIORITY_CH2 : R/W; bitpos: [5:4]; default: 1;
 *  Set the priority of channel
 */
#define DMA2D_OUT_ARB_PRIORITY_CH2    0x00000003U
#define DMA2D_OUT_ARB_PRIORITY_CH2_M  (DMA2D_OUT_ARB_PRIORITY_CH2_V << DMA2D_OUT_ARB_PRIORITY_CH2_S)
#define DMA2D_OUT_ARB_PRIORITY_CH2_V  0x00000003U
#define DMA2D_OUT_ARB_PRIORITY_CH2_S  4

/** DMA2D_OUT_RO_STATUS_CH2_REG register
 *  Represents the status of the tx reorder module of channel 0
 */
#define DMA2D_OUT_RO_STATUS_CH2_REG (DR_REG_DMA2D_BASE + 0x240)
/** DMA2D_OUTFIFO_RO_CNT_CH2 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in color convert Tx FIFO for
 *  channel 0.
 */
#define DMA2D_OUTFIFO_RO_CNT_CH2    0x0000003FU
#define DMA2D_OUTFIFO_RO_CNT_CH2_M  (DMA2D_OUTFIFO_RO_CNT_CH2_V << DMA2D_OUTFIFO_RO_CNT_CH2_S)
#define DMA2D_OUTFIFO_RO_CNT_CH2_V  0x0000003FU
#define DMA2D_OUTFIFO_RO_CNT_CH2_S  0
/** DMA2D_OUT_RO_WR_STATE_CH2 : RO; bitpos: [7:6]; default: 0;
 *  The register stores the state of read ram of reorder
 */
#define DMA2D_OUT_RO_WR_STATE_CH2    0x00000003U
#define DMA2D_OUT_RO_WR_STATE_CH2_M  (DMA2D_OUT_RO_WR_STATE_CH2_V << DMA2D_OUT_RO_WR_STATE_CH2_S)
#define DMA2D_OUT_RO_WR_STATE_CH2_V  0x00000003U
#define DMA2D_OUT_RO_WR_STATE_CH2_S  6
/** DMA2D_OUT_RO_RD_STATE_CH2 : RO; bitpos: [9:8]; default: 0;
 *  The register stores the state of write ram of reorder
 */
#define DMA2D_OUT_RO_RD_STATE_CH2    0x00000003U
#define DMA2D_OUT_RO_RD_STATE_CH2_M  (DMA2D_OUT_RO_RD_STATE_CH2_V << DMA2D_OUT_RO_RD_STATE_CH2_S)
#define DMA2D_OUT_RO_RD_STATE_CH2_V  0x00000003U
#define DMA2D_OUT_RO_RD_STATE_CH2_S  8
/** DMA2D_OUT_PIXEL_BYTE_CH2 : RO; bitpos: [13:10]; default: 0;
 *  the number of bytes contained in a pixel at TX channel     0: 1byte     1: 1.5bytes
 *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
 */
#define DMA2D_OUT_PIXEL_BYTE_CH2    0x0000000FU
#define DMA2D_OUT_PIXEL_BYTE_CH2_M  (DMA2D_OUT_PIXEL_BYTE_CH2_V << DMA2D_OUT_PIXEL_BYTE_CH2_S)
#define DMA2D_OUT_PIXEL_BYTE_CH2_V  0x0000000FU
#define DMA2D_OUT_PIXEL_BYTE_CH2_S  10
/** DMA2D_OUT_BURST_BLOCK_NUM_CH2 : RO; bitpos: [17:14]; default: 0;
 *  the number of macro blocks contained in a burst of data at TX channel
 */
#define DMA2D_OUT_BURST_BLOCK_NUM_CH2    0x0000000FU
#define DMA2D_OUT_BURST_BLOCK_NUM_CH2_M  (DMA2D_OUT_BURST_BLOCK_NUM_CH2_V << DMA2D_OUT_BURST_BLOCK_NUM_CH2_S)
#define DMA2D_OUT_BURST_BLOCK_NUM_CH2_V  0x0000000FU
#define DMA2D_OUT_BURST_BLOCK_NUM_CH2_S  14

/** DMA2D_OUT_COLOR_CONVERT_CH2_REG register
 *  Configures the tx color convert of channel 0
 */
#define DMA2D_OUT_COLOR_CONVERT_CH2_REG (DR_REG_DMA2D_BASE + 0x248)
/** DMA2D_OUT_COLOR_OUTPUT_SEL_CH2 : R/W; bitpos: [1:0]; default: 0;
 *  Set final color convert process and output type    0: RGB888 to RGB565     1:
 *  YUV444 to YUV422     2: output directly
 */
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH2    0x00000003U
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH2_M  (DMA2D_OUT_COLOR_OUTPUT_SEL_CH2_V << DMA2D_OUT_COLOR_OUTPUT_SEL_CH2_S)
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH2_V  0x00000003U
#define DMA2D_OUT_COLOR_OUTPUT_SEL_CH2_S  0
/** DMA2D_OUT_COLOR_3B_PROC_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Enable generic color convert module between color input & color output, need to
 *  configure parameter.
 */
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH2    (BIT(2))
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH2_M  (DMA2D_OUT_COLOR_3B_PROC_EN_CH2_V << DMA2D_OUT_COLOR_3B_PROC_EN_CH2_S)
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH2_V  0x00000001U
#define DMA2D_OUT_COLOR_3B_PROC_EN_CH2_S  2
/** DMA2D_OUT_COLOR_INPUT_SEL_CH2 : R/W; bitpos: [5:3]; default: 7;
 *  Set first color convert process and input color type    0: RGB565 to RGB888     1:
 *  YUV422 to YUV444      2: Other 2byte/pixel type    3: Other 3byte/pixel type    7:
 *  disable color space convert
 */
#define DMA2D_OUT_COLOR_INPUT_SEL_CH2    0x00000007U
#define DMA2D_OUT_COLOR_INPUT_SEL_CH2_M  (DMA2D_OUT_COLOR_INPUT_SEL_CH2_V << DMA2D_OUT_COLOR_INPUT_SEL_CH2_S)
#define DMA2D_OUT_COLOR_INPUT_SEL_CH2_V  0x00000007U
#define DMA2D_OUT_COLOR_INPUT_SEL_CH2_S  3

/** DMA2D_OUT_SCRAMBLE_CH2_REG register
 *  Configures the tx scramble of channel 0
 */
#define DMA2D_OUT_SCRAMBLE_CH2_REG (DR_REG_DMA2D_BASE + 0x24c)
/** DMA2D_OUT_SCRAMBLE_SEL_PRE_CH2 : R/W; bitpos: [2:0]; default: 0;
 *  Scramble data before color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 :
 *  BYTE1-0-2    3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
 */
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH2    0x00000007U
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH2_M  (DMA2D_OUT_SCRAMBLE_SEL_PRE_CH2_V << DMA2D_OUT_SCRAMBLE_SEL_PRE_CH2_S)
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH2_V  0x00000007U
#define DMA2D_OUT_SCRAMBLE_SEL_PRE_CH2_S  0

/** DMA2D_OUT_COLOR_PARAM0_CH2_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM0_CH2_REG (DR_REG_DMA2D_BASE + 0x250)
/** DMA2D_OUT_COLOR_PARAM_H0_CH2 : R/W; bitpos: [20:0]; default: 298;
 *  Set first 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_H0_CH2    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_H0_CH2_M  (DMA2D_OUT_COLOR_PARAM_H0_CH2_V << DMA2D_OUT_COLOR_PARAM_H0_CH2_S)
#define DMA2D_OUT_COLOR_PARAM_H0_CH2_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_H0_CH2_S  0

/** DMA2D_OUT_COLOR_PARAM1_CH2_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM1_CH2_REG (DR_REG_DMA2D_BASE + 0x254)
/** DMA2D_OUT_COLOR_PARAM_H1_CH2 : R/W; bitpos: [27:0]; default: 210164121;
 *  Set last 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_H1_CH2    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_H1_CH2_M  (DMA2D_OUT_COLOR_PARAM_H1_CH2_V << DMA2D_OUT_COLOR_PARAM_H1_CH2_S)
#define DMA2D_OUT_COLOR_PARAM_H1_CH2_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_H1_CH2_S  0

/** DMA2D_OUT_COLOR_PARAM2_CH2_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM2_CH2_REG (DR_REG_DMA2D_BASE + 0x258)
/** DMA2D_OUT_COLOR_PARAM_M0_CH2 : R/W; bitpos: [20:0]; default: 1995050;
 *  Set first 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_M0_CH2    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_M0_CH2_M  (DMA2D_OUT_COLOR_PARAM_M0_CH2_V << DMA2D_OUT_COLOR_PARAM_M0_CH2_S)
#define DMA2D_OUT_COLOR_PARAM_M0_CH2_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_M0_CH2_S  0

/** DMA2D_OUT_COLOR_PARAM3_CH2_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM3_CH2_REG (DR_REG_DMA2D_BASE + 0x25c)
/** DMA2D_OUT_COLOR_PARAM_M1_CH2 : R/W; bitpos: [27:0]; default: 35540784;
 *  Set last 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_M1_CH2    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_M1_CH2_M  (DMA2D_OUT_COLOR_PARAM_M1_CH2_V << DMA2D_OUT_COLOR_PARAM_M1_CH2_S)
#define DMA2D_OUT_COLOR_PARAM_M1_CH2_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_M1_CH2_S  0

/** DMA2D_OUT_COLOR_PARAM4_CH2_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM4_CH2_REG (DR_REG_DMA2D_BASE + 0x260)
/** DMA2D_OUT_COLOR_PARAM_L0_CH2 : R/W; bitpos: [20:0]; default: 528682;
 *  Set first 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_L0_CH2    0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_L0_CH2_M  (DMA2D_OUT_COLOR_PARAM_L0_CH2_V << DMA2D_OUT_COLOR_PARAM_L0_CH2_S)
#define DMA2D_OUT_COLOR_PARAM_L0_CH2_V  0x001FFFFFU
#define DMA2D_OUT_COLOR_PARAM_L0_CH2_S  0

/** DMA2D_OUT_COLOR_PARAM5_CH2_REG register
 *  Configures the tx color convert parameter of channel 0
 */
#define DMA2D_OUT_COLOR_PARAM5_CH2_REG (DR_REG_DMA2D_BASE + 0x264)
/** DMA2D_OUT_COLOR_PARAM_L1_CH2 : R/W; bitpos: [27:0]; default: 195899392;
 *  Set last 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_OUT_COLOR_PARAM_L1_CH2    0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_L1_CH2_M  (DMA2D_OUT_COLOR_PARAM_L1_CH2_V << DMA2D_OUT_COLOR_PARAM_L1_CH2_S)
#define DMA2D_OUT_COLOR_PARAM_L1_CH2_V  0x0FFFFFFFU
#define DMA2D_OUT_COLOR_PARAM_L1_CH2_S  0

/** DMA2D_OUT_ETM_CONF_CH2_REG register
 *  Configures the tx etm of channel 0
 */
#define DMA2D_OUT_ETM_CONF_CH2_REG (DR_REG_DMA2D_BASE + 0x268)
/** DMA2D_OUT_ETM_EN_CH2 : R/W; bitpos: [0]; default: 0;
 *  Configures the enable of the etm function, 1 is enable.
 */
#define DMA2D_OUT_ETM_EN_CH2    (BIT(0))
#define DMA2D_OUT_ETM_EN_CH2_M  (DMA2D_OUT_ETM_EN_CH2_V << DMA2D_OUT_ETM_EN_CH2_S)
#define DMA2D_OUT_ETM_EN_CH2_V  0x00000001U
#define DMA2D_OUT_ETM_EN_CH2_S  0
/** DMA2D_OUT_ETM_LOOP_EN_CH2 : R/W; bitpos: [1]; default: 0;
 *  Configures the enable of the descriptors loop etm function, 1 is enable.
 */
#define DMA2D_OUT_ETM_LOOP_EN_CH2    (BIT(1))
#define DMA2D_OUT_ETM_LOOP_EN_CH2_M  (DMA2D_OUT_ETM_LOOP_EN_CH2_V << DMA2D_OUT_ETM_LOOP_EN_CH2_S)
#define DMA2D_OUT_ETM_LOOP_EN_CH2_V  0x00000001U
#define DMA2D_OUT_ETM_LOOP_EN_CH2_S  1
/** DMA2D_OUT_DSCR_TASK_MAK_CH2 : R/W; bitpos: [3:2]; default: 1;
 *  Configures the maximum number of cacheable descriptors.
 */
#define DMA2D_OUT_DSCR_TASK_MAK_CH2    0x00000003U
#define DMA2D_OUT_DSCR_TASK_MAK_CH2_M  (DMA2D_OUT_DSCR_TASK_MAK_CH2_V << DMA2D_OUT_DSCR_TASK_MAK_CH2_S)
#define DMA2D_OUT_DSCR_TASK_MAK_CH2_V  0x00000003U
#define DMA2D_OUT_DSCR_TASK_MAK_CH2_S  2

/** DMA2D_OUT_DSCR_PORT_BLK_CH2_REG register
 *  Configures the tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_CH2_REG (DR_REG_DMA2D_BASE + 0x26c)
/** DMA2D_OUT_DSCR_PORT_BLK_H_CH2 : R/W; bitpos: [13:0]; default: 18;
 *  Set the horizontal width of tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH2    0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH2_M  (DMA2D_OUT_DSCR_PORT_BLK_H_CH2_V << DMA2D_OUT_DSCR_PORT_BLK_H_CH2_S)
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH2_V  0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_H_CH2_S  0
/** DMA2D_OUT_DSCR_PORT_BLK_V_CH2 : R/W; bitpos: [27:14]; default: 18;
 *  Set the vertical height of tx block size in dscr port mode
 */
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH2    0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH2_M  (DMA2D_OUT_DSCR_PORT_BLK_V_CH2_V << DMA2D_OUT_DSCR_PORT_BLK_V_CH2_S)
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH2_V  0x00003FFFU
#define DMA2D_OUT_DSCR_PORT_BLK_V_CH2_S  14

/** DMA2D_IN_CONF0_CH0_REG register
 *  Configures the rx direction of channel 0
 */
#define DMA2D_IN_CONF0_CH0_REG (DR_REG_DMA2D_BASE + 0x500)
/** DMA2D_IN_MEM_TRANS_EN_CH0 : R/W; bitpos: [0]; default: 0;
 *  enable memory trans of the same channel
 */
#define DMA2D_IN_MEM_TRANS_EN_CH0    (BIT(0))
#define DMA2D_IN_MEM_TRANS_EN_CH0_M  (DMA2D_IN_MEM_TRANS_EN_CH0_V << DMA2D_IN_MEM_TRANS_EN_CH0_S)
#define DMA2D_IN_MEM_TRANS_EN_CH0_V  0x00000001U
#define DMA2D_IN_MEM_TRANS_EN_CH0_S  0
/** DMA2D_INDSCR_BURST_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
 *  when accessing SRAM.
 */
#define DMA2D_INDSCR_BURST_EN_CH0    (BIT(2))
#define DMA2D_INDSCR_BURST_EN_CH0_M  (DMA2D_INDSCR_BURST_EN_CH0_V << DMA2D_INDSCR_BURST_EN_CH0_S)
#define DMA2D_INDSCR_BURST_EN_CH0_V  0x00000001U
#define DMA2D_INDSCR_BURST_EN_CH0_S  2
/** DMA2D_IN_ECC_AES_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define DMA2D_IN_ECC_AES_EN_CH0    (BIT(3))
#define DMA2D_IN_ECC_AES_EN_CH0_M  (DMA2D_IN_ECC_AES_EN_CH0_V << DMA2D_IN_ECC_AES_EN_CH0_S)
#define DMA2D_IN_ECC_AES_EN_CH0_V  0x00000001U
#define DMA2D_IN_ECC_AES_EN_CH0_S  3
/** DMA2D_IN_CHECK_OWNER_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define DMA2D_IN_CHECK_OWNER_CH0    (BIT(4))
#define DMA2D_IN_CHECK_OWNER_CH0_M  (DMA2D_IN_CHECK_OWNER_CH0_V << DMA2D_IN_CHECK_OWNER_CH0_S)
#define DMA2D_IN_CHECK_OWNER_CH0_V  0x00000001U
#define DMA2D_IN_CHECK_OWNER_CH0_S  4
/** DMA2D_IN_LOOP_TEST_CH0 : R/W; bitpos: [5]; default: 0;
 *  reserved
 */
#define DMA2D_IN_LOOP_TEST_CH0    (BIT(5))
#define DMA2D_IN_LOOP_TEST_CH0_M  (DMA2D_IN_LOOP_TEST_CH0_V << DMA2D_IN_LOOP_TEST_CH0_S)
#define DMA2D_IN_LOOP_TEST_CH0_V  0x00000001U
#define DMA2D_IN_LOOP_TEST_CH0_S  5
/** DMA2D_IN_MEM_BURST_LENGTH_CH0 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 0. 0: 8 bytes      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define DMA2D_IN_MEM_BURST_LENGTH_CH0    0x00000007U
#define DMA2D_IN_MEM_BURST_LENGTH_CH0_M  (DMA2D_IN_MEM_BURST_LENGTH_CH0_V << DMA2D_IN_MEM_BURST_LENGTH_CH0_S)
#define DMA2D_IN_MEM_BURST_LENGTH_CH0_V  0x00000007U
#define DMA2D_IN_MEM_BURST_LENGTH_CH0_S  6
/** DMA2D_IN_MACRO_BLOCK_SIZE_CH0 : R/W; bitpos: [10:9]; default: 0;
 *  Sel RX macro-block size 0: 8pixel*8pixel     1: 8pixel*16pixel     2:
 *  16pixel*16pixel     3: no macro-block     , only useful in mode 1 of the link
 *  descriptor
 */
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH0    0x00000003U
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH0_M  (DMA2D_IN_MACRO_BLOCK_SIZE_CH0_V << DMA2D_IN_MACRO_BLOCK_SIZE_CH0_S)
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH0_V  0x00000003U
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH0_S  9
/** DMA2D_IN_DSCR_PORT_EN_CH0 : R/W; bitpos: [11]; default: 0;
 *  Set this bit to 1 to obtain descriptor from IP port
 */
#define DMA2D_IN_DSCR_PORT_EN_CH0    (BIT(11))
#define DMA2D_IN_DSCR_PORT_EN_CH0_M  (DMA2D_IN_DSCR_PORT_EN_CH0_V << DMA2D_IN_DSCR_PORT_EN_CH0_S)
#define DMA2D_IN_DSCR_PORT_EN_CH0_V  0x00000001U
#define DMA2D_IN_DSCR_PORT_EN_CH0_S  11
/** DMA2D_IN_PAGE_BOUND_EN_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define DMA2D_IN_PAGE_BOUND_EN_CH0    (BIT(12))
#define DMA2D_IN_PAGE_BOUND_EN_CH0_M  (DMA2D_IN_PAGE_BOUND_EN_CH0_V << DMA2D_IN_PAGE_BOUND_EN_CH0_S)
#define DMA2D_IN_PAGE_BOUND_EN_CH0_V  0x00000001U
#define DMA2D_IN_PAGE_BOUND_EN_CH0_S  12
/** DMA2D_IN_REORDER_EN_CH0 : R/W; bitpos: [16]; default: 0;
 *  Enable RX channel 0 macro block reorder when set to 1, only channel0 have this
 *  selection
 */
#define DMA2D_IN_REORDER_EN_CH0    (BIT(16))
#define DMA2D_IN_REORDER_EN_CH0_M  (DMA2D_IN_REORDER_EN_CH0_V << DMA2D_IN_REORDER_EN_CH0_S)
#define DMA2D_IN_REORDER_EN_CH0_V  0x00000001U
#define DMA2D_IN_REORDER_EN_CH0_S  16
/** DMA2D_IN_RST_CH0 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define DMA2D_IN_RST_CH0    (BIT(24))
#define DMA2D_IN_RST_CH0_M  (DMA2D_IN_RST_CH0_V << DMA2D_IN_RST_CH0_S)
#define DMA2D_IN_RST_CH0_V  0x00000001U
#define DMA2D_IN_RST_CH0_S  24
/** DMA2D_IN_CMD_DISABLE_CH0 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define DMA2D_IN_CMD_DISABLE_CH0    (BIT(25))
#define DMA2D_IN_CMD_DISABLE_CH0_M  (DMA2D_IN_CMD_DISABLE_CH0_V << DMA2D_IN_CMD_DISABLE_CH0_S)
#define DMA2D_IN_CMD_DISABLE_CH0_V  0x00000001U
#define DMA2D_IN_CMD_DISABLE_CH0_S  25
/** DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH0 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH0    (BIT(26))
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH0_M  (DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH0_V << DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH0_S)
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH0_V  0x00000001U
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH0_S  26

/** DMA2D_IN_INT_RAW_CH0_REG register
 *  Raw interrupt status of RX channel 0
 */
#define DMA2D_IN_INT_RAW_CH0_REG (DR_REG_DMA2D_BASE + 0x504)
/** DMA2D_IN_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 0.
 */
#define DMA2D_IN_DONE_CH0_INT_RAW    (BIT(0))
#define DMA2D_IN_DONE_CH0_INT_RAW_M  (DMA2D_IN_DONE_CH0_INT_RAW_V << DMA2D_IN_DONE_CH0_INT_RAW_S)
#define DMA2D_IN_DONE_CH0_INT_RAW_V  0x00000001U
#define DMA2D_IN_DONE_CH0_INT_RAW_S  0
/** DMA2D_IN_SUC_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 0.
 */
#define DMA2D_IN_SUC_EOF_CH0_INT_RAW    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH0_INT_RAW_M  (DMA2D_IN_SUC_EOF_CH0_INT_RAW_V << DMA2D_IN_SUC_EOF_CH0_INT_RAW_S)
#define DMA2D_IN_SUC_EOF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH0_INT_RAW_S  1
/** DMA2D_IN_ERR_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and data error is detected
 */
#define DMA2D_IN_ERR_EOF_CH0_INT_RAW    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH0_INT_RAW_M  (DMA2D_IN_ERR_EOF_CH0_INT_RAW_V << DMA2D_IN_ERR_EOF_CH0_INT_RAW_S)
#define DMA2D_IN_ERR_EOF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH0_INT_RAW_S  2
/** DMA2D_IN_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor for Rx
 *  channel 0.
 */
#define DMA2D_IN_DSCR_ERR_CH0_INT_RAW    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH0_INT_RAW_M  (DMA2D_IN_DSCR_ERR_CH0_INT_RAW_V << DMA2D_IN_DSCR_ERR_CH0_INT_RAW_S)
#define DMA2D_IN_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH0_INT_RAW_S  3
/** DMA2D_INFIFO_OVF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define DMA2D_INFIFO_OVF_L1_CH0_INT_RAW    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH0_INT_RAW_M  (DMA2D_INFIFO_OVF_L1_CH0_INT_RAW_V << DMA2D_INFIFO_OVF_L1_CH0_INT_RAW_S)
#define DMA2D_INFIFO_OVF_L1_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH0_INT_RAW_S  4
/** DMA2D_INFIFO_UDF_L1_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define DMA2D_INFIFO_UDF_L1_CH0_INT_RAW    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH0_INT_RAW_M  (DMA2D_INFIFO_UDF_L1_CH0_INT_RAW_V << DMA2D_INFIFO_UDF_L1_CH0_INT_RAW_S)
#define DMA2D_INFIFO_UDF_L1_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH0_INT_RAW_S  5
/** DMA2D_INFIFO_OVF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define DMA2D_INFIFO_OVF_L2_CH0_INT_RAW    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH0_INT_RAW_M  (DMA2D_INFIFO_OVF_L2_CH0_INT_RAW_V << DMA2D_INFIFO_OVF_L2_CH0_INT_RAW_S)
#define DMA2D_INFIFO_OVF_L2_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH0_INT_RAW_S  6
/** DMA2D_INFIFO_UDF_L2_CH0_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define DMA2D_INFIFO_UDF_L2_CH0_INT_RAW    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH0_INT_RAW_M  (DMA2D_INFIFO_UDF_L2_CH0_INT_RAW_V << DMA2D_INFIFO_UDF_L2_CH0_INT_RAW_S)
#define DMA2D_INFIFO_UDF_L2_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH0_INT_RAW_S  7
/** DMA2D_INFIFO_OVF_L3_CH0_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define DMA2D_INFIFO_OVF_L3_CH0_INT_RAW    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH0_INT_RAW_M  (DMA2D_INFIFO_OVF_L3_CH0_INT_RAW_V << DMA2D_INFIFO_OVF_L3_CH0_INT_RAW_S)
#define DMA2D_INFIFO_OVF_L3_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH0_INT_RAW_S  8
/** DMA2D_INFIFO_UDF_L3_CH0_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define DMA2D_INFIFO_UDF_L3_CH0_INT_RAW    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH0_INT_RAW_M  (DMA2D_INFIFO_UDF_L3_CH0_INT_RAW_V << DMA2D_INFIFO_UDF_L3_CH0_INT_RAW_S)
#define DMA2D_INFIFO_UDF_L3_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH0_INT_RAW_S  9
/** DMA2D_IN_DSCR_EMPTY_CH0_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
 *  also remain data.
 */
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_RAW    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_RAW_M  (DMA2D_IN_DSCR_EMPTY_CH0_INT_RAW_V << DMA2D_IN_DSCR_EMPTY_CH0_INT_RAW_S)
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_RAW_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_RAW_S  10
/** DMA2D_INFIFO_RO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is overflow.
 */
#define DMA2D_INFIFO_RO_OVF_CH0_INT_RAW    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH0_INT_RAW_M  (DMA2D_INFIFO_RO_OVF_CH0_INT_RAW_V << DMA2D_INFIFO_RO_OVF_CH0_INT_RAW_S)
#define DMA2D_INFIFO_RO_OVF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH0_INT_RAW_S  11
/** DMA2D_INFIFO_RO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is underflow.
 */
#define DMA2D_INFIFO_RO_UDF_CH0_INT_RAW    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH0_INT_RAW_M  (DMA2D_INFIFO_RO_UDF_CH0_INT_RAW_V << DMA2D_INFIFO_RO_UDF_CH0_INT_RAW_S)
#define DMA2D_INFIFO_RO_UDF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH0_INT_RAW_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_RAW    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_RAW_M  (DMA2D_IN_DSCR_TASK_OVF_CH0_INT_RAW_V << DMA2D_IN_DSCR_TASK_OVF_CH0_INT_RAW_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_RAW_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_RAW_S  13

/** DMA2D_IN_INT_ENA_CH0_REG register
 *  Interrupt enable bits of RX channel 0
 */
#define DMA2D_IN_INT_ENA_CH0_REG (DR_REG_DMA2D_BASE + 0x508)
/** DMA2D_IN_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define DMA2D_IN_DONE_CH0_INT_ENA    (BIT(0))
#define DMA2D_IN_DONE_CH0_INT_ENA_M  (DMA2D_IN_DONE_CH0_INT_ENA_V << DMA2D_IN_DONE_CH0_INT_ENA_S)
#define DMA2D_IN_DONE_CH0_INT_ENA_V  0x00000001U
#define DMA2D_IN_DONE_CH0_INT_ENA_S  0
/** DMA2D_IN_SUC_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_SUC_EOF_CH0_INT_ENA    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH0_INT_ENA_M  (DMA2D_IN_SUC_EOF_CH0_INT_ENA_V << DMA2D_IN_SUC_EOF_CH0_INT_ENA_S)
#define DMA2D_IN_SUC_EOF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH0_INT_ENA_S  1
/** DMA2D_IN_ERR_EOF_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_ERR_EOF_CH0_INT_ENA    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH0_INT_ENA_M  (DMA2D_IN_ERR_EOF_CH0_INT_ENA_V << DMA2D_IN_ERR_EOF_CH0_INT_ENA_S)
#define DMA2D_IN_ERR_EOF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH0_INT_ENA_S  2
/** DMA2D_IN_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_ERR_CH0_INT_ENA    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH0_INT_ENA_M  (DMA2D_IN_DSCR_ERR_CH0_INT_ENA_V << DMA2D_IN_DSCR_ERR_CH0_INT_ENA_S)
#define DMA2D_IN_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH0_INT_ENA_S  3
/** DMA2D_INFIFO_OVF_L1_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ENA    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ENA_M  (DMA2D_INFIFO_OVF_L1_CH0_INT_ENA_V << DMA2D_INFIFO_OVF_L1_CH0_INT_ENA_S)
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ENA_S  4
/** DMA2D_INFIFO_UDF_L1_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ENA    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ENA_M  (DMA2D_INFIFO_UDF_L1_CH0_INT_ENA_V << DMA2D_INFIFO_UDF_L1_CH0_INT_ENA_S)
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ENA_S  5
/** DMA2D_INFIFO_OVF_L2_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ENA    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ENA_M  (DMA2D_INFIFO_OVF_L2_CH0_INT_ENA_V << DMA2D_INFIFO_OVF_L2_CH0_INT_ENA_S)
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ENA_S  6
/** DMA2D_INFIFO_UDF_L2_CH0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ENA    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ENA_M  (DMA2D_INFIFO_UDF_L2_CH0_INT_ENA_V << DMA2D_INFIFO_UDF_L2_CH0_INT_ENA_S)
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ENA_S  7
/** DMA2D_INFIFO_OVF_L3_CH0_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ENA    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ENA_M  (DMA2D_INFIFO_OVF_L3_CH0_INT_ENA_V << DMA2D_INFIFO_OVF_L3_CH0_INT_ENA_S)
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ENA_S  8
/** DMA2D_INFIFO_UDF_L3_CH0_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ENA    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ENA_M  (DMA2D_INFIFO_UDF_L3_CH0_INT_ENA_V << DMA2D_INFIFO_UDF_L3_CH0_INT_ENA_S)
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ENA_S  9
/** DMA2D_IN_DSCR_EMPTY_CH0_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ENA    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ENA_M  (DMA2D_IN_DSCR_EMPTY_CH0_INT_ENA_V << DMA2D_IN_DSCR_EMPTY_CH0_INT_ENA_S)
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ENA_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ENA_S  10
/** DMA2D_INFIFO_RO_OVF_CH0_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the INFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ENA    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ENA_M  (DMA2D_INFIFO_RO_OVF_CH0_INT_ENA_V << DMA2D_INFIFO_RO_OVF_CH0_INT_ENA_S)
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ENA_S  11
/** DMA2D_INFIFO_RO_UDF_CH0_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the INFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ENA    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ENA_M  (DMA2D_INFIFO_RO_UDF_CH0_INT_ENA_V << DMA2D_INFIFO_RO_UDF_CH0_INT_ENA_S)
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ENA_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ENA    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ENA_M  (DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ENA_V << DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ENA_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ENA_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ENA_S  13

/** DMA2D_IN_INT_ST_CH0_REG register
 *  Masked interrupt status of RX channel 0
 */
#define DMA2D_IN_INT_ST_CH0_REG (DR_REG_DMA2D_BASE + 0x50c)
/** DMA2D_IN_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define DMA2D_IN_DONE_CH0_INT_ST    (BIT(0))
#define DMA2D_IN_DONE_CH0_INT_ST_M  (DMA2D_IN_DONE_CH0_INT_ST_V << DMA2D_IN_DONE_CH0_INT_ST_S)
#define DMA2D_IN_DONE_CH0_INT_ST_V  0x00000001U
#define DMA2D_IN_DONE_CH0_INT_ST_S  0
/** DMA2D_IN_SUC_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_SUC_EOF_CH0_INT_ST    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH0_INT_ST_M  (DMA2D_IN_SUC_EOF_CH0_INT_ST_V << DMA2D_IN_SUC_EOF_CH0_INT_ST_S)
#define DMA2D_IN_SUC_EOF_CH0_INT_ST_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH0_INT_ST_S  1
/** DMA2D_IN_ERR_EOF_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_ERR_EOF_CH0_INT_ST    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH0_INT_ST_M  (DMA2D_IN_ERR_EOF_CH0_INT_ST_V << DMA2D_IN_ERR_EOF_CH0_INT_ST_S)
#define DMA2D_IN_ERR_EOF_CH0_INT_ST_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH0_INT_ST_S  2
/** DMA2D_IN_DSCR_ERR_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_ERR_CH0_INT_ST    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH0_INT_ST_M  (DMA2D_IN_DSCR_ERR_CH0_INT_ST_V << DMA2D_IN_DSCR_ERR_CH0_INT_ST_S)
#define DMA2D_IN_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH0_INT_ST_S  3
/** DMA2D_INFIFO_OVF_L1_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ST    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ST_M  (DMA2D_INFIFO_OVF_L1_CH0_INT_ST_V << DMA2D_INFIFO_OVF_L1_CH0_INT_ST_S)
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH0_INT_ST_S  4
/** DMA2D_INFIFO_UDF_L1_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ST    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ST_M  (DMA2D_INFIFO_UDF_L1_CH0_INT_ST_V << DMA2D_INFIFO_UDF_L1_CH0_INT_ST_S)
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH0_INT_ST_S  5
/** DMA2D_INFIFO_OVF_L2_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ST    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ST_M  (DMA2D_INFIFO_OVF_L2_CH0_INT_ST_V << DMA2D_INFIFO_OVF_L2_CH0_INT_ST_S)
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH0_INT_ST_S  6
/** DMA2D_INFIFO_UDF_L2_CH0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ST    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ST_M  (DMA2D_INFIFO_UDF_L2_CH0_INT_ST_V << DMA2D_INFIFO_UDF_L2_CH0_INT_ST_S)
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH0_INT_ST_S  7
/** DMA2D_INFIFO_OVF_L3_CH0_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ST    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ST_M  (DMA2D_INFIFO_OVF_L3_CH0_INT_ST_V << DMA2D_INFIFO_OVF_L3_CH0_INT_ST_S)
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH0_INT_ST_S  8
/** DMA2D_INFIFO_UDF_L3_CH0_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ST    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ST_M  (DMA2D_INFIFO_UDF_L3_CH0_INT_ST_V << DMA2D_INFIFO_UDF_L3_CH0_INT_ST_S)
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH0_INT_ST_S  9
/** DMA2D_IN_DSCR_EMPTY_CH0_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ST    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ST_M  (DMA2D_IN_DSCR_EMPTY_CH0_INT_ST_V << DMA2D_IN_DSCR_EMPTY_CH0_INT_ST_S)
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ST_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_ST_S  10
/** DMA2D_INFIFO_RO_OVF_CH0_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the INFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ST    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ST_M  (DMA2D_INFIFO_RO_OVF_CH0_INT_ST_V << DMA2D_INFIFO_RO_OVF_CH0_INT_ST_S)
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH0_INT_ST_S  11
/** DMA2D_INFIFO_RO_UDF_CH0_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the INFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ST    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ST_M  (DMA2D_INFIFO_RO_UDF_CH0_INT_ST_V << DMA2D_INFIFO_RO_UDF_CH0_INT_ST_S)
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH0_INT_ST_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ST : RO; bitpos: [13]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ST    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ST_M  (DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ST_V << DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ST_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ST_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_ST_S  13

/** DMA2D_IN_INT_CLR_CH0_REG register
 *  Interrupt clear bits of RX channel 0
 */
#define DMA2D_IN_INT_CLR_CH0_REG (DR_REG_DMA2D_BASE + 0x510)
/** DMA2D_IN_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define DMA2D_IN_DONE_CH0_INT_CLR    (BIT(0))
#define DMA2D_IN_DONE_CH0_INT_CLR_M  (DMA2D_IN_DONE_CH0_INT_CLR_V << DMA2D_IN_DONE_CH0_INT_CLR_S)
#define DMA2D_IN_DONE_CH0_INT_CLR_V  0x00000001U
#define DMA2D_IN_DONE_CH0_INT_CLR_S  0
/** DMA2D_IN_SUC_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_SUC_EOF_CH0_INT_CLR    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH0_INT_CLR_M  (DMA2D_IN_SUC_EOF_CH0_INT_CLR_V << DMA2D_IN_SUC_EOF_CH0_INT_CLR_S)
#define DMA2D_IN_SUC_EOF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH0_INT_CLR_S  1
/** DMA2D_IN_ERR_EOF_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_ERR_EOF_CH0_INT_CLR    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH0_INT_CLR_M  (DMA2D_IN_ERR_EOF_CH0_INT_CLR_V << DMA2D_IN_ERR_EOF_CH0_INT_CLR_S)
#define DMA2D_IN_ERR_EOF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH0_INT_CLR_S  2
/** DMA2D_IN_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_ERR_CH0_INT_CLR    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH0_INT_CLR_M  (DMA2D_IN_DSCR_ERR_CH0_INT_CLR_V << DMA2D_IN_DSCR_ERR_CH0_INT_CLR_S)
#define DMA2D_IN_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH0_INT_CLR_S  3
/** DMA2D_INFIFO_OVF_L1_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L1_CH0_INT_CLR    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH0_INT_CLR_M  (DMA2D_INFIFO_OVF_L1_CH0_INT_CLR_V << DMA2D_INFIFO_OVF_L1_CH0_INT_CLR_S)
#define DMA2D_INFIFO_OVF_L1_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH0_INT_CLR_S  4
/** DMA2D_INFIFO_UDF_L1_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L1_CH0_INT_CLR    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH0_INT_CLR_M  (DMA2D_INFIFO_UDF_L1_CH0_INT_CLR_V << DMA2D_INFIFO_UDF_L1_CH0_INT_CLR_S)
#define DMA2D_INFIFO_UDF_L1_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH0_INT_CLR_S  5
/** DMA2D_INFIFO_OVF_L2_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L2_CH0_INT_CLR    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH0_INT_CLR_M  (DMA2D_INFIFO_OVF_L2_CH0_INT_CLR_V << DMA2D_INFIFO_OVF_L2_CH0_INT_CLR_S)
#define DMA2D_INFIFO_OVF_L2_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH0_INT_CLR_S  6
/** DMA2D_INFIFO_UDF_L2_CH0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L2_CH0_INT_CLR    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH0_INT_CLR_M  (DMA2D_INFIFO_UDF_L2_CH0_INT_CLR_V << DMA2D_INFIFO_UDF_L2_CH0_INT_CLR_S)
#define DMA2D_INFIFO_UDF_L2_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH0_INT_CLR_S  7
/** DMA2D_INFIFO_OVF_L3_CH0_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L3_CH0_INT_CLR    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH0_INT_CLR_M  (DMA2D_INFIFO_OVF_L3_CH0_INT_CLR_V << DMA2D_INFIFO_OVF_L3_CH0_INT_CLR_S)
#define DMA2D_INFIFO_OVF_L3_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH0_INT_CLR_S  8
/** DMA2D_INFIFO_UDF_L3_CH0_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L3_CH0_INT_CLR    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH0_INT_CLR_M  (DMA2D_INFIFO_UDF_L3_CH0_INT_CLR_V << DMA2D_INFIFO_UDF_L3_CH0_INT_CLR_S)
#define DMA2D_INFIFO_UDF_L3_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH0_INT_CLR_S  9
/** DMA2D_IN_DSCR_EMPTY_CH0_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_CLR    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_CLR_M  (DMA2D_IN_DSCR_EMPTY_CH0_INT_CLR_V << DMA2D_IN_DSCR_EMPTY_CH0_INT_CLR_S)
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_CLR_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH0_INT_CLR_S  10
/** DMA2D_INFIFO_RO_OVF_CH0_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the INFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_OVF_CH0_INT_CLR    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH0_INT_CLR_M  (DMA2D_INFIFO_RO_OVF_CH0_INT_CLR_V << DMA2D_INFIFO_RO_OVF_CH0_INT_CLR_S)
#define DMA2D_INFIFO_RO_OVF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH0_INT_CLR_S  11
/** DMA2D_INFIFO_RO_UDF_CH0_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the INFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_UDF_CH0_INT_CLR    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH0_INT_CLR_M  (DMA2D_INFIFO_RO_UDF_CH0_INT_CLR_V << DMA2D_INFIFO_RO_UDF_CH0_INT_CLR_S)
#define DMA2D_INFIFO_RO_UDF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH0_INT_CLR_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH0_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_CLR    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_CLR_M  (DMA2D_IN_DSCR_TASK_OVF_CH0_INT_CLR_V << DMA2D_IN_DSCR_TASK_OVF_CH0_INT_CLR_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_CLR_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH0_INT_CLR_S  13

/** DMA2D_INFIFO_STATUS_CH0_REG register
 *  Represents the status of the rx fifo of channel 0
 */
#define DMA2D_INFIFO_STATUS_CH0_REG (DR_REG_DMA2D_BASE + 0x514)
/** DMA2D_INFIFO_FULL_L2_CH0 : RO; bitpos: [0]; default: 0;
 *  Rx FIFO full signal for Rx channel.
 */
#define DMA2D_INFIFO_FULL_L2_CH0    (BIT(0))
#define DMA2D_INFIFO_FULL_L2_CH0_M  (DMA2D_INFIFO_FULL_L2_CH0_V << DMA2D_INFIFO_FULL_L2_CH0_S)
#define DMA2D_INFIFO_FULL_L2_CH0_V  0x00000001U
#define DMA2D_INFIFO_FULL_L2_CH0_S  0
/** DMA2D_INFIFO_EMPTY_L2_CH0 : RO; bitpos: [1]; default: 1;
 *  Rx FIFO empty signal for Rx channel.
 */
#define DMA2D_INFIFO_EMPTY_L2_CH0    (BIT(1))
#define DMA2D_INFIFO_EMPTY_L2_CH0_M  (DMA2D_INFIFO_EMPTY_L2_CH0_V << DMA2D_INFIFO_EMPTY_L2_CH0_S)
#define DMA2D_INFIFO_EMPTY_L2_CH0_V  0x00000001U
#define DMA2D_INFIFO_EMPTY_L2_CH0_S  1
/** DMA2D_INFIFO_CNT_L2_CH0 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Rx FIFO for Rx channel.
 */
#define DMA2D_INFIFO_CNT_L2_CH0    0x0000000FU
#define DMA2D_INFIFO_CNT_L2_CH0_M  (DMA2D_INFIFO_CNT_L2_CH0_V << DMA2D_INFIFO_CNT_L2_CH0_S)
#define DMA2D_INFIFO_CNT_L2_CH0_V  0x0000000FU
#define DMA2D_INFIFO_CNT_L2_CH0_S  2
/** DMA2D_IN_REMAIN_UNDER_1B_CH0 : RO; bitpos: [7]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_1B_CH0    (BIT(7))
#define DMA2D_IN_REMAIN_UNDER_1B_CH0_M  (DMA2D_IN_REMAIN_UNDER_1B_CH0_V << DMA2D_IN_REMAIN_UNDER_1B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_1B_CH0_S  7
/** DMA2D_IN_REMAIN_UNDER_2B_CH0 : RO; bitpos: [8]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_2B_CH0    (BIT(8))
#define DMA2D_IN_REMAIN_UNDER_2B_CH0_M  (DMA2D_IN_REMAIN_UNDER_2B_CH0_V << DMA2D_IN_REMAIN_UNDER_2B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_2B_CH0_S  8
/** DMA2D_IN_REMAIN_UNDER_3B_CH0 : RO; bitpos: [9]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_3B_CH0    (BIT(9))
#define DMA2D_IN_REMAIN_UNDER_3B_CH0_M  (DMA2D_IN_REMAIN_UNDER_3B_CH0_V << DMA2D_IN_REMAIN_UNDER_3B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_3B_CH0_S  9
/** DMA2D_IN_REMAIN_UNDER_4B_CH0 : RO; bitpos: [10]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_4B_CH0    (BIT(10))
#define DMA2D_IN_REMAIN_UNDER_4B_CH0_M  (DMA2D_IN_REMAIN_UNDER_4B_CH0_V << DMA2D_IN_REMAIN_UNDER_4B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_4B_CH0_S  10
/** DMA2D_IN_REMAIN_UNDER_5B_CH0 : RO; bitpos: [11]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_5B_CH0    (BIT(11))
#define DMA2D_IN_REMAIN_UNDER_5B_CH0_M  (DMA2D_IN_REMAIN_UNDER_5B_CH0_V << DMA2D_IN_REMAIN_UNDER_5B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_5B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_5B_CH0_S  11
/** DMA2D_IN_REMAIN_UNDER_6B_CH0 : RO; bitpos: [12]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_6B_CH0    (BIT(12))
#define DMA2D_IN_REMAIN_UNDER_6B_CH0_M  (DMA2D_IN_REMAIN_UNDER_6B_CH0_V << DMA2D_IN_REMAIN_UNDER_6B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_6B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_6B_CH0_S  12
/** DMA2D_IN_REMAIN_UNDER_7B_CH0 : RO; bitpos: [13]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_7B_CH0    (BIT(13))
#define DMA2D_IN_REMAIN_UNDER_7B_CH0_M  (DMA2D_IN_REMAIN_UNDER_7B_CH0_V << DMA2D_IN_REMAIN_UNDER_7B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_7B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_7B_CH0_S  13
/** DMA2D_IN_REMAIN_UNDER_8B_CH0 : RO; bitpos: [14]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_8B_CH0    (BIT(14))
#define DMA2D_IN_REMAIN_UNDER_8B_CH0_M  (DMA2D_IN_REMAIN_UNDER_8B_CH0_V << DMA2D_IN_REMAIN_UNDER_8B_CH0_S)
#define DMA2D_IN_REMAIN_UNDER_8B_CH0_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_8B_CH0_S  14
/** DMA2D_INFIFO_FULL_L1_CH0 : RO; bitpos: [15]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_INFIFO_FULL_L1_CH0    (BIT(15))
#define DMA2D_INFIFO_FULL_L1_CH0_M  (DMA2D_INFIFO_FULL_L1_CH0_V << DMA2D_INFIFO_FULL_L1_CH0_S)
#define DMA2D_INFIFO_FULL_L1_CH0_V  0x00000001U
#define DMA2D_INFIFO_FULL_L1_CH0_S  15
/** DMA2D_INFIFO_EMPTY_L1_CH0 : RO; bitpos: [16]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_INFIFO_EMPTY_L1_CH0    (BIT(16))
#define DMA2D_INFIFO_EMPTY_L1_CH0_M  (DMA2D_INFIFO_EMPTY_L1_CH0_V << DMA2D_INFIFO_EMPTY_L1_CH0_S)
#define DMA2D_INFIFO_EMPTY_L1_CH0_V  0x00000001U
#define DMA2D_INFIFO_EMPTY_L1_CH0_S  16
/** DMA2D_INFIFO_CNT_L1_CH0 : RO; bitpos: [21:17]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_INFIFO_CNT_L1_CH0    0x0000001FU
#define DMA2D_INFIFO_CNT_L1_CH0_M  (DMA2D_INFIFO_CNT_L1_CH0_V << DMA2D_INFIFO_CNT_L1_CH0_S)
#define DMA2D_INFIFO_CNT_L1_CH0_V  0x0000001FU
#define DMA2D_INFIFO_CNT_L1_CH0_S  17
/** DMA2D_INFIFO_FULL_L3_CH0 : RO; bitpos: [22]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_INFIFO_FULL_L3_CH0    (BIT(22))
#define DMA2D_INFIFO_FULL_L3_CH0_M  (DMA2D_INFIFO_FULL_L3_CH0_V << DMA2D_INFIFO_FULL_L3_CH0_S)
#define DMA2D_INFIFO_FULL_L3_CH0_V  0x00000001U
#define DMA2D_INFIFO_FULL_L3_CH0_S  22
/** DMA2D_INFIFO_EMPTY_L3_CH0 : RO; bitpos: [23]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_INFIFO_EMPTY_L3_CH0    (BIT(23))
#define DMA2D_INFIFO_EMPTY_L3_CH0_M  (DMA2D_INFIFO_EMPTY_L3_CH0_V << DMA2D_INFIFO_EMPTY_L3_CH0_S)
#define DMA2D_INFIFO_EMPTY_L3_CH0_V  0x00000001U
#define DMA2D_INFIFO_EMPTY_L3_CH0_S  23
/** DMA2D_INFIFO_CNT_L3_CH0 : RO; bitpos: [28:24]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_INFIFO_CNT_L3_CH0    0x0000001FU
#define DMA2D_INFIFO_CNT_L3_CH0_M  (DMA2D_INFIFO_CNT_L3_CH0_V << DMA2D_INFIFO_CNT_L3_CH0_S)
#define DMA2D_INFIFO_CNT_L3_CH0_V  0x0000001FU
#define DMA2D_INFIFO_CNT_L3_CH0_S  24

/** DMA2D_IN_POP_CH0_REG register
 *  Configures the rx fifo of channel 0
 */
#define DMA2D_IN_POP_CH0_REG (DR_REG_DMA2D_BASE + 0x518)
/** DMA2D_INFIFO_RDATA_CH0 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define DMA2D_INFIFO_RDATA_CH0    0x000007FFU
#define DMA2D_INFIFO_RDATA_CH0_M  (DMA2D_INFIFO_RDATA_CH0_V << DMA2D_INFIFO_RDATA_CH0_S)
#define DMA2D_INFIFO_RDATA_CH0_V  0x000007FFU
#define DMA2D_INFIFO_RDATA_CH0_S  0
/** DMA2D_INFIFO_POP_CH0 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define DMA2D_INFIFO_POP_CH0    (BIT(11))
#define DMA2D_INFIFO_POP_CH0_M  (DMA2D_INFIFO_POP_CH0_V << DMA2D_INFIFO_POP_CH0_S)
#define DMA2D_INFIFO_POP_CH0_V  0x00000001U
#define DMA2D_INFIFO_POP_CH0_S  11

/** DMA2D_IN_LINK_CONF_CH0_REG register
 *  Configures the rx descriptor operations of channel 0
 */
#define DMA2D_IN_LINK_CONF_CH0_REG (DR_REG_DMA2D_BASE + 0x51c)
/** DMA2D_INLINK_AUTO_RET_CH0 : R/W; bitpos: [20]; default: 1;
 *  Configure the value of the owner field written back to the inlink descriptor.
 *  1: Write back 1. 0: Write back 0.
 */
#define DMA2D_INLINK_AUTO_RET_CH0    (BIT(20))
#define DMA2D_INLINK_AUTO_RET_CH0_M  (DMA2D_INLINK_AUTO_RET_CH0_V << DMA2D_INLINK_AUTO_RET_CH0_S)
#define DMA2D_INLINK_AUTO_RET_CH0_V  0x00000001U
#define DMA2D_INLINK_AUTO_RET_CH0_S  20
/** DMA2D_INLINK_STOP_CH0 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define DMA2D_INLINK_STOP_CH0    (BIT(21))
#define DMA2D_INLINK_STOP_CH0_M  (DMA2D_INLINK_STOP_CH0_V << DMA2D_INLINK_STOP_CH0_S)
#define DMA2D_INLINK_STOP_CH0_V  0x00000001U
#define DMA2D_INLINK_STOP_CH0_S  21
/** DMA2D_INLINK_START_CH0 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define DMA2D_INLINK_START_CH0    (BIT(22))
#define DMA2D_INLINK_START_CH0_M  (DMA2D_INLINK_START_CH0_V << DMA2D_INLINK_START_CH0_S)
#define DMA2D_INLINK_START_CH0_V  0x00000001U
#define DMA2D_INLINK_START_CH0_S  22
/** DMA2D_INLINK_RESTART_CH0 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define DMA2D_INLINK_RESTART_CH0    (BIT(23))
#define DMA2D_INLINK_RESTART_CH0_M  (DMA2D_INLINK_RESTART_CH0_V << DMA2D_INLINK_RESTART_CH0_S)
#define DMA2D_INLINK_RESTART_CH0_V  0x00000001U
#define DMA2D_INLINK_RESTART_CH0_S  23
/** DMA2D_INLINK_PARK_CH0 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define DMA2D_INLINK_PARK_CH0    (BIT(24))
#define DMA2D_INLINK_PARK_CH0_M  (DMA2D_INLINK_PARK_CH0_V << DMA2D_INLINK_PARK_CH0_S)
#define DMA2D_INLINK_PARK_CH0_V  0x00000001U
#define DMA2D_INLINK_PARK_CH0_S  24

/** DMA2D_IN_LINK_ADDR_CH0_REG register
 *  Configures the rx descriptor address of channel 0
 */
#define DMA2D_IN_LINK_ADDR_CH0_REG (DR_REG_DMA2D_BASE + 0x520)
/** DMA2D_INLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first inlink descriptor's address.
 */
#define DMA2D_INLINK_ADDR_CH0    0xFFFFFFFFU
#define DMA2D_INLINK_ADDR_CH0_M  (DMA2D_INLINK_ADDR_CH0_V << DMA2D_INLINK_ADDR_CH0_S)
#define DMA2D_INLINK_ADDR_CH0_V  0xFFFFFFFFU
#define DMA2D_INLINK_ADDR_CH0_S  0

/** DMA2D_IN_STATE_CH0_REG register
 *  Represents the working status of the rx descriptor of channel 0
 */
#define DMA2D_IN_STATE_CH0_REG (DR_REG_DMA2D_BASE + 0x524)
/** DMA2D_INLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define DMA2D_INLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define DMA2D_INLINK_DSCR_ADDR_CH0_M  (DMA2D_INLINK_DSCR_ADDR_CH0_V << DMA2D_INLINK_DSCR_ADDR_CH0_S)
#define DMA2D_INLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define DMA2D_INLINK_DSCR_ADDR_CH0_S  0
/** DMA2D_IN_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define DMA2D_IN_DSCR_STATE_CH0    0x00000003U
#define DMA2D_IN_DSCR_STATE_CH0_M  (DMA2D_IN_DSCR_STATE_CH0_V << DMA2D_IN_DSCR_STATE_CH0_S)
#define DMA2D_IN_DSCR_STATE_CH0_V  0x00000003U
#define DMA2D_IN_DSCR_STATE_CH0_S  18
/** DMA2D_IN_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define DMA2D_IN_STATE_CH0    0x00000007U
#define DMA2D_IN_STATE_CH0_M  (DMA2D_IN_STATE_CH0_V << DMA2D_IN_STATE_CH0_S)
#define DMA2D_IN_STATE_CH0_V  0x00000007U
#define DMA2D_IN_STATE_CH0_S  20
/** DMA2D_IN_RESET_AVAIL_CH0 : RO; bitpos: [23]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define DMA2D_IN_RESET_AVAIL_CH0    (BIT(23))
#define DMA2D_IN_RESET_AVAIL_CH0_M  (DMA2D_IN_RESET_AVAIL_CH0_V << DMA2D_IN_RESET_AVAIL_CH0_S)
#define DMA2D_IN_RESET_AVAIL_CH0_V  0x00000001U
#define DMA2D_IN_RESET_AVAIL_CH0_S  23

/** DMA2D_IN_SUC_EOF_DES_ADDR_CH0_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH0_REG (DR_REG_DMA2D_BASE + 0x528)
/** DMA2D_IN_SUC_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH0_M  (DMA2D_IN_SUC_EOF_DES_ADDR_CH0_V << DMA2D_IN_SUC_EOF_DES_ADDR_CH0_S)
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH0_S  0

/** DMA2D_IN_ERR_EOF_DES_ADDR_CH0_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH0_REG (DR_REG_DMA2D_BASE + 0x52c)
/** DMA2D_IN_ERR_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data.
 */
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH0_M  (DMA2D_IN_ERR_EOF_DES_ADDR_CH0_V << DMA2D_IN_ERR_EOF_DES_ADDR_CH0_S)
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH0_S  0

/** DMA2D_IN_DSCR_CH0_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_DSCR_CH0_REG (DR_REG_DMA2D_BASE + 0x530)
/** DMA2D_INLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next inlink descriptor address x.
 */
#define DMA2D_INLINK_DSCR_CH0    0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_CH0_M  (DMA2D_INLINK_DSCR_CH0_V << DMA2D_INLINK_DSCR_CH0_S)
#define DMA2D_INLINK_DSCR_CH0_V  0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_CH0_S  0

/** DMA2D_IN_DSCR_BF0_CH0_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_DSCR_BF0_CH0_REG (DR_REG_DMA2D_BASE + 0x534)
/** DMA2D_INLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor's next address x-1.
 */
#define DMA2D_INLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF0_CH0_M  (DMA2D_INLINK_DSCR_BF0_CH0_V << DMA2D_INLINK_DSCR_BF0_CH0_S)
#define DMA2D_INLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF0_CH0_S  0

/** DMA2D_IN_DSCR_BF1_CH0_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_DSCR_BF1_CH0_REG (DR_REG_DMA2D_BASE + 0x538)
/** DMA2D_INLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor's next address x-2.
 */
#define DMA2D_INLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF1_CH0_M  (DMA2D_INLINK_DSCR_BF1_CH0_V << DMA2D_INLINK_DSCR_BF1_CH0_S)
#define DMA2D_INLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF1_CH0_S  0

/** DMA2D_IN_PERI_SEL_CH0_REG register
 *  Configures the rx peripheral of channel 0
 */
#define DMA2D_IN_PERI_SEL_CH0_REG (DR_REG_DMA2D_BASE + 0x53c)
/** DMA2D_IN_PERI_SEL_CH0 : R/W; bitpos: [2:0]; default: 7;
 *  This register is used to select peripheral for Rx channel   0:  jpeg     1:
 *  display-1     2: display-2        7: no choose
 */
#define DMA2D_IN_PERI_SEL_CH0    0x00000007U
#define DMA2D_IN_PERI_SEL_CH0_M  (DMA2D_IN_PERI_SEL_CH0_V << DMA2D_IN_PERI_SEL_CH0_S)
#define DMA2D_IN_PERI_SEL_CH0_V  0x00000007U
#define DMA2D_IN_PERI_SEL_CH0_S  0

/** DMA2D_IN_ARB_CH0_REG register
 *  Configures the rx arbiter of channel 0
 */
#define DMA2D_IN_ARB_CH0_REG (DR_REG_DMA2D_BASE + 0x540)
/** DMA2D_IN_ARB_TOKEN_NUM_CH0 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define DMA2D_IN_ARB_TOKEN_NUM_CH0    0x0000000FU
#define DMA2D_IN_ARB_TOKEN_NUM_CH0_M  (DMA2D_IN_ARB_TOKEN_NUM_CH0_V << DMA2D_IN_ARB_TOKEN_NUM_CH0_S)
#define DMA2D_IN_ARB_TOKEN_NUM_CH0_V  0x0000000FU
#define DMA2D_IN_ARB_TOKEN_NUM_CH0_S  0
/** DMA2D_IN_ARB_PRIORITY_CH0 : R/W; bitpos: [4]; default: 1;
 *  Set the priority of channel
 */
#define DMA2D_IN_ARB_PRIORITY_CH0    (BIT(4))
#define DMA2D_IN_ARB_PRIORITY_CH0_M  (DMA2D_IN_ARB_PRIORITY_CH0_V << DMA2D_IN_ARB_PRIORITY_CH0_S)
#define DMA2D_IN_ARB_PRIORITY_CH0_V  0x00000001U
#define DMA2D_IN_ARB_PRIORITY_CH0_S  4

/** DMA2D_IN_RO_STATUS_CH0_REG register
 *  Represents the status of the rx reorder module of channel 0
 */
#define DMA2D_IN_RO_STATUS_CH0_REG (DR_REG_DMA2D_BASE + 0x544)
/** DMA2D_INFIFO_RO_CNT_CH0 : RO; bitpos: [4:0]; default: 0;
 *  The register stores the byte number of the data in color convert Rx FIFO for
 *  channel 0.
 */
#define DMA2D_INFIFO_RO_CNT_CH0    0x0000001FU
#define DMA2D_INFIFO_RO_CNT_CH0_M  (DMA2D_INFIFO_RO_CNT_CH0_V << DMA2D_INFIFO_RO_CNT_CH0_S)
#define DMA2D_INFIFO_RO_CNT_CH0_V  0x0000001FU
#define DMA2D_INFIFO_RO_CNT_CH0_S  0
/** DMA2D_IN_RO_WR_STATE_CH0 : RO; bitpos: [6:5]; default: 0;
 *  The register stores the state of read ram of reorder
 */
#define DMA2D_IN_RO_WR_STATE_CH0    0x00000003U
#define DMA2D_IN_RO_WR_STATE_CH0_M  (DMA2D_IN_RO_WR_STATE_CH0_V << DMA2D_IN_RO_WR_STATE_CH0_S)
#define DMA2D_IN_RO_WR_STATE_CH0_V  0x00000003U
#define DMA2D_IN_RO_WR_STATE_CH0_S  5
/** DMA2D_IN_RO_RD_STATE_CH0 : RO; bitpos: [8:7]; default: 0;
 *  The register stores the state of write ram of reorder
 */
#define DMA2D_IN_RO_RD_STATE_CH0    0x00000003U
#define DMA2D_IN_RO_RD_STATE_CH0_M  (DMA2D_IN_RO_RD_STATE_CH0_V << DMA2D_IN_RO_RD_STATE_CH0_S)
#define DMA2D_IN_RO_RD_STATE_CH0_V  0x00000003U
#define DMA2D_IN_RO_RD_STATE_CH0_S  7
/** DMA2D_IN_PIXEL_BYTE_CH0 : RO; bitpos: [12:9]; default: 0;
 *  the number of bytes contained in a pixel at RX channel     0: 1byte     1: 1.5bytes
 *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
 */
#define DMA2D_IN_PIXEL_BYTE_CH0    0x0000000FU
#define DMA2D_IN_PIXEL_BYTE_CH0_M  (DMA2D_IN_PIXEL_BYTE_CH0_V << DMA2D_IN_PIXEL_BYTE_CH0_S)
#define DMA2D_IN_PIXEL_BYTE_CH0_V  0x0000000FU
#define DMA2D_IN_PIXEL_BYTE_CH0_S  9
/** DMA2D_IN_BURST_BLOCK_NUM_CH0 : RO; bitpos: [16:13]; default: 0;
 *  the number of macro blocks contained in a burst of data at RX channel
 */
#define DMA2D_IN_BURST_BLOCK_NUM_CH0    0x0000000FU
#define DMA2D_IN_BURST_BLOCK_NUM_CH0_M  (DMA2D_IN_BURST_BLOCK_NUM_CH0_V << DMA2D_IN_BURST_BLOCK_NUM_CH0_S)
#define DMA2D_IN_BURST_BLOCK_NUM_CH0_V  0x0000000FU
#define DMA2D_IN_BURST_BLOCK_NUM_CH0_S  13

/** DMA2D_IN_RO_PD_CONF_CH0_REG register
 *  Configures the rx reorder memory of channel 0
 */
#define DMA2D_IN_RO_PD_CONF_CH0_REG (DR_REG_DMA2D_BASE + 0x548)
/** DMA2D_IN_RO_RAM_FORCE_PD_CH0 : R/W; bitpos: [4]; default: 0;
 *  dma reorder ram power down
 */
#define DMA2D_IN_RO_RAM_FORCE_PD_CH0    (BIT(4))
#define DMA2D_IN_RO_RAM_FORCE_PD_CH0_M  (DMA2D_IN_RO_RAM_FORCE_PD_CH0_V << DMA2D_IN_RO_RAM_FORCE_PD_CH0_S)
#define DMA2D_IN_RO_RAM_FORCE_PD_CH0_V  0x00000001U
#define DMA2D_IN_RO_RAM_FORCE_PD_CH0_S  4
/** DMA2D_IN_RO_RAM_FORCE_PU_CH0 : R/W; bitpos: [5]; default: 1;
 *  dma reorder ram power up
 */
#define DMA2D_IN_RO_RAM_FORCE_PU_CH0    (BIT(5))
#define DMA2D_IN_RO_RAM_FORCE_PU_CH0_M  (DMA2D_IN_RO_RAM_FORCE_PU_CH0_V << DMA2D_IN_RO_RAM_FORCE_PU_CH0_S)
#define DMA2D_IN_RO_RAM_FORCE_PU_CH0_V  0x00000001U
#define DMA2D_IN_RO_RAM_FORCE_PU_CH0_S  5
/** DMA2D_IN_RO_RAM_CLK_FO_CH0 : R/W; bitpos: [6]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
 *  0: A gate-clock will be used when accessing the RAM in DMA.
 */
#define DMA2D_IN_RO_RAM_CLK_FO_CH0    (BIT(6))
#define DMA2D_IN_RO_RAM_CLK_FO_CH0_M  (DMA2D_IN_RO_RAM_CLK_FO_CH0_V << DMA2D_IN_RO_RAM_CLK_FO_CH0_S)
#define DMA2D_IN_RO_RAM_CLK_FO_CH0_V  0x00000001U
#define DMA2D_IN_RO_RAM_CLK_FO_CH0_S  6

/** DMA2D_IN_COLOR_CONVERT_CH0_REG register
 *  Configures the tx color convert of channel 0
 */
#define DMA2D_IN_COLOR_CONVERT_CH0_REG (DR_REG_DMA2D_BASE + 0x54c)
/** DMA2D_IN_COLOR_OUTPUT_SEL_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  Set final color convert process and output type    0: RGB888 to RGB565     1:
 *  output directly
 */
#define DMA2D_IN_COLOR_OUTPUT_SEL_CH0    0x00000003U
#define DMA2D_IN_COLOR_OUTPUT_SEL_CH0_M  (DMA2D_IN_COLOR_OUTPUT_SEL_CH0_V << DMA2D_IN_COLOR_OUTPUT_SEL_CH0_S)
#define DMA2D_IN_COLOR_OUTPUT_SEL_CH0_V  0x00000003U
#define DMA2D_IN_COLOR_OUTPUT_SEL_CH0_S  0
/** DMA2D_IN_COLOR_3B_PROC_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Enable generic color convert module between color input & color output, need to
 *  configure parameter.
 */
#define DMA2D_IN_COLOR_3B_PROC_EN_CH0    (BIT(2))
#define DMA2D_IN_COLOR_3B_PROC_EN_CH0_M  (DMA2D_IN_COLOR_3B_PROC_EN_CH0_V << DMA2D_IN_COLOR_3B_PROC_EN_CH0_S)
#define DMA2D_IN_COLOR_3B_PROC_EN_CH0_V  0x00000001U
#define DMA2D_IN_COLOR_3B_PROC_EN_CH0_S  2
/** DMA2D_IN_COLOR_INPUT_SEL_CH0 : R/W; bitpos: [5:3]; default: 7;
 *  Set first color convert process and input color type    0: YUV422/420 to YUV444
 *  1: YUV422      2: YUV444/420      7: disable color space convert
 */
#define DMA2D_IN_COLOR_INPUT_SEL_CH0    0x00000007U
#define DMA2D_IN_COLOR_INPUT_SEL_CH0_M  (DMA2D_IN_COLOR_INPUT_SEL_CH0_V << DMA2D_IN_COLOR_INPUT_SEL_CH0_S)
#define DMA2D_IN_COLOR_INPUT_SEL_CH0_V  0x00000007U
#define DMA2D_IN_COLOR_INPUT_SEL_CH0_S  3

/** DMA2D_IN_SCRAMBLE_CH0_REG register
 *  Configures the rx scramble of channel 0
 */
#define DMA2D_IN_SCRAMBLE_CH0_REG (DR_REG_DMA2D_BASE + 0x550)
/** DMA2D_IN_SCRAMBLE_SEL_PRE_CH0 : R/W; bitpos: [2:0]; default: 0;
 *  Scramble data before color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 :
 *  BYTE1-0-2    3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
 */
#define DMA2D_IN_SCRAMBLE_SEL_PRE_CH0    0x00000007U
#define DMA2D_IN_SCRAMBLE_SEL_PRE_CH0_M  (DMA2D_IN_SCRAMBLE_SEL_PRE_CH0_V << DMA2D_IN_SCRAMBLE_SEL_PRE_CH0_S)
#define DMA2D_IN_SCRAMBLE_SEL_PRE_CH0_V  0x00000007U
#define DMA2D_IN_SCRAMBLE_SEL_PRE_CH0_S  0
/** DMA2D_IN_SCRAMBLE_SEL_POST_CH0 : R/W; bitpos: [5:3]; default: 0;
 *  Scramble data after color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 : BYTE1-0-2
 *  3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
 */
#define DMA2D_IN_SCRAMBLE_SEL_POST_CH0    0x00000007U
#define DMA2D_IN_SCRAMBLE_SEL_POST_CH0_M  (DMA2D_IN_SCRAMBLE_SEL_POST_CH0_V << DMA2D_IN_SCRAMBLE_SEL_POST_CH0_S)
#define DMA2D_IN_SCRAMBLE_SEL_POST_CH0_V  0x00000007U
#define DMA2D_IN_SCRAMBLE_SEL_POST_CH0_S  3

/** DMA2D_IN_COLOR_PARAM0_CH0_REG register
 *  Configures the rx color convert parameter of channel 0
 */
#define DMA2D_IN_COLOR_PARAM0_CH0_REG (DR_REG_DMA2D_BASE + 0x554)
/** DMA2D_IN_COLOR_PARAM_H0_CH0 : R/W; bitpos: [20:0]; default: 298;
 *  Set first 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_IN_COLOR_PARAM_H0_CH0    0x001FFFFFU
#define DMA2D_IN_COLOR_PARAM_H0_CH0_M  (DMA2D_IN_COLOR_PARAM_H0_CH0_V << DMA2D_IN_COLOR_PARAM_H0_CH0_S)
#define DMA2D_IN_COLOR_PARAM_H0_CH0_V  0x001FFFFFU
#define DMA2D_IN_COLOR_PARAM_H0_CH0_S  0

/** DMA2D_IN_COLOR_PARAM1_CH0_REG register
 *  Configures the rx color convert parameter of channel 0
 */
#define DMA2D_IN_COLOR_PARAM1_CH0_REG (DR_REG_DMA2D_BASE + 0x558)
/** DMA2D_IN_COLOR_PARAM_H1_CH0 : R/W; bitpos: [27:0]; default: 210164121;
 *  Set last 2 parameter of most significant byte of pending 3 bytes
 */
#define DMA2D_IN_COLOR_PARAM_H1_CH0    0x0FFFFFFFU
#define DMA2D_IN_COLOR_PARAM_H1_CH0_M  (DMA2D_IN_COLOR_PARAM_H1_CH0_V << DMA2D_IN_COLOR_PARAM_H1_CH0_S)
#define DMA2D_IN_COLOR_PARAM_H1_CH0_V  0x0FFFFFFFU
#define DMA2D_IN_COLOR_PARAM_H1_CH0_S  0

/** DMA2D_IN_COLOR_PARAM2_CH0_REG register
 *  Configures the rx color convert parameter of channel 0
 */
#define DMA2D_IN_COLOR_PARAM2_CH0_REG (DR_REG_DMA2D_BASE + 0x55c)
/** DMA2D_IN_COLOR_PARAM_M0_CH0 : R/W; bitpos: [20:0]; default: 1995050;
 *  Set first 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_IN_COLOR_PARAM_M0_CH0    0x001FFFFFU
#define DMA2D_IN_COLOR_PARAM_M0_CH0_M  (DMA2D_IN_COLOR_PARAM_M0_CH0_V << DMA2D_IN_COLOR_PARAM_M0_CH0_S)
#define DMA2D_IN_COLOR_PARAM_M0_CH0_V  0x001FFFFFU
#define DMA2D_IN_COLOR_PARAM_M0_CH0_S  0

/** DMA2D_IN_COLOR_PARAM3_CH0_REG register
 *  Configures the rx color convert parameter of channel 0
 */
#define DMA2D_IN_COLOR_PARAM3_CH0_REG (DR_REG_DMA2D_BASE + 0x560)
/** DMA2D_IN_COLOR_PARAM_M1_CH0 : R/W; bitpos: [27:0]; default: 35540784;
 *  Set last 2 parameter of midium significant byte of pending 3 bytes
 */
#define DMA2D_IN_COLOR_PARAM_M1_CH0    0x0FFFFFFFU
#define DMA2D_IN_COLOR_PARAM_M1_CH0_M  (DMA2D_IN_COLOR_PARAM_M1_CH0_V << DMA2D_IN_COLOR_PARAM_M1_CH0_S)
#define DMA2D_IN_COLOR_PARAM_M1_CH0_V  0x0FFFFFFFU
#define DMA2D_IN_COLOR_PARAM_M1_CH0_S  0

/** DMA2D_IN_COLOR_PARAM4_CH0_REG register
 *  Configures the rx color convert parameter of channel 0
 */
#define DMA2D_IN_COLOR_PARAM4_CH0_REG (DR_REG_DMA2D_BASE + 0x564)
/** DMA2D_IN_COLOR_PARAM_L0_CH0 : R/W; bitpos: [20:0]; default: 528682;
 *  Set first 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_IN_COLOR_PARAM_L0_CH0    0x001FFFFFU
#define DMA2D_IN_COLOR_PARAM_L0_CH0_M  (DMA2D_IN_COLOR_PARAM_L0_CH0_V << DMA2D_IN_COLOR_PARAM_L0_CH0_S)
#define DMA2D_IN_COLOR_PARAM_L0_CH0_V  0x001FFFFFU
#define DMA2D_IN_COLOR_PARAM_L0_CH0_S  0

/** DMA2D_IN_COLOR_PARAM5_CH0_REG register
 *  Configures the rx color convert parameter of channel 0
 */
#define DMA2D_IN_COLOR_PARAM5_CH0_REG (DR_REG_DMA2D_BASE + 0x568)
/** DMA2D_IN_COLOR_PARAM_L1_CH0 : R/W; bitpos: [27:0]; default: 195899392;
 *  Set last 2 parameter of least significant byte of pending 3 bytes
 */
#define DMA2D_IN_COLOR_PARAM_L1_CH0    0x0FFFFFFFU
#define DMA2D_IN_COLOR_PARAM_L1_CH0_M  (DMA2D_IN_COLOR_PARAM_L1_CH0_V << DMA2D_IN_COLOR_PARAM_L1_CH0_S)
#define DMA2D_IN_COLOR_PARAM_L1_CH0_V  0x0FFFFFFFU
#define DMA2D_IN_COLOR_PARAM_L1_CH0_S  0

/** DMA2D_IN_ETM_CONF_CH0_REG register
 *  Configures the rx etm of channel 0
 */
#define DMA2D_IN_ETM_CONF_CH0_REG (DR_REG_DMA2D_BASE + 0x56c)
/** DMA2D_IN_ETM_EN_CH0 : R/W; bitpos: [0]; default: 0;
 *  Configures the enable of the etm function, 1 is enable.
 */
#define DMA2D_IN_ETM_EN_CH0    (BIT(0))
#define DMA2D_IN_ETM_EN_CH0_M  (DMA2D_IN_ETM_EN_CH0_V << DMA2D_IN_ETM_EN_CH0_S)
#define DMA2D_IN_ETM_EN_CH0_V  0x00000001U
#define DMA2D_IN_ETM_EN_CH0_S  0
/** DMA2D_IN_ETM_LOOP_EN_CH0 : R/W; bitpos: [1]; default: 0;
 *  Configures the enable of the descriptors loop etm function, 1 is enable.
 */
#define DMA2D_IN_ETM_LOOP_EN_CH0    (BIT(1))
#define DMA2D_IN_ETM_LOOP_EN_CH0_M  (DMA2D_IN_ETM_LOOP_EN_CH0_V << DMA2D_IN_ETM_LOOP_EN_CH0_S)
#define DMA2D_IN_ETM_LOOP_EN_CH0_V  0x00000001U
#define DMA2D_IN_ETM_LOOP_EN_CH0_S  1
/** DMA2D_IN_DSCR_TASK_MAK_CH0 : R/W; bitpos: [3:2]; default: 1;
 *  Configures the maximum number of cacheable descriptors.
 */
#define DMA2D_IN_DSCR_TASK_MAK_CH0    0x00000003U
#define DMA2D_IN_DSCR_TASK_MAK_CH0_M  (DMA2D_IN_DSCR_TASK_MAK_CH0_V << DMA2D_IN_DSCR_TASK_MAK_CH0_S)
#define DMA2D_IN_DSCR_TASK_MAK_CH0_V  0x00000003U
#define DMA2D_IN_DSCR_TASK_MAK_CH0_S  2

/** DMA2D_IN_CONF0_CH1_REG register
 *  Configures the rx direction of channel 0
 */
#define DMA2D_IN_CONF0_CH1_REG (DR_REG_DMA2D_BASE + 0x600)
/** DMA2D_IN_MEM_TRANS_EN_CH1 : R/W; bitpos: [0]; default: 0;
 *  enable memory trans of the same channel
 */
#define DMA2D_IN_MEM_TRANS_EN_CH1    (BIT(0))
#define DMA2D_IN_MEM_TRANS_EN_CH1_M  (DMA2D_IN_MEM_TRANS_EN_CH1_V << DMA2D_IN_MEM_TRANS_EN_CH1_S)
#define DMA2D_IN_MEM_TRANS_EN_CH1_V  0x00000001U
#define DMA2D_IN_MEM_TRANS_EN_CH1_S  0
/** DMA2D_INDSCR_BURST_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
 *  when accessing SRAM.
 */
#define DMA2D_INDSCR_BURST_EN_CH1    (BIT(2))
#define DMA2D_INDSCR_BURST_EN_CH1_M  (DMA2D_INDSCR_BURST_EN_CH1_V << DMA2D_INDSCR_BURST_EN_CH1_S)
#define DMA2D_INDSCR_BURST_EN_CH1_V  0x00000001U
#define DMA2D_INDSCR_BURST_EN_CH1_S  2
/** DMA2D_IN_ECC_AES_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  When access address space is ecc/aes area, this bit should be set to 1. In this
 *  case, the start address of square should be 16-bit aligned. The width of square
 *  multiply byte number of one pixel should be 16-bit aligned.
 */
#define DMA2D_IN_ECC_AES_EN_CH1    (BIT(3))
#define DMA2D_IN_ECC_AES_EN_CH1_M  (DMA2D_IN_ECC_AES_EN_CH1_V << DMA2D_IN_ECC_AES_EN_CH1_S)
#define DMA2D_IN_ECC_AES_EN_CH1_V  0x00000001U
#define DMA2D_IN_ECC_AES_EN_CH1_S  3
/** DMA2D_IN_CHECK_OWNER_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define DMA2D_IN_CHECK_OWNER_CH1    (BIT(4))
#define DMA2D_IN_CHECK_OWNER_CH1_M  (DMA2D_IN_CHECK_OWNER_CH1_V << DMA2D_IN_CHECK_OWNER_CH1_S)
#define DMA2D_IN_CHECK_OWNER_CH1_V  0x00000001U
#define DMA2D_IN_CHECK_OWNER_CH1_S  4
/** DMA2D_IN_LOOP_TEST_CH1 : R/W; bitpos: [5]; default: 0;
 *  reserved
 */
#define DMA2D_IN_LOOP_TEST_CH1    (BIT(5))
#define DMA2D_IN_LOOP_TEST_CH1_M  (DMA2D_IN_LOOP_TEST_CH1_V << DMA2D_IN_LOOP_TEST_CH1_S)
#define DMA2D_IN_LOOP_TEST_CH1_V  0x00000001U
#define DMA2D_IN_LOOP_TEST_CH1_S  5
/** DMA2D_IN_MEM_BURST_LENGTH_CH1 : R/W; bitpos: [8:6]; default: 0;
 *  Block size of Rx channel 0. 0: 8 bytes      1: 16 bytes      2: 32 bytes    3: 64
 *  bytes    4: 128 bytes
 */
#define DMA2D_IN_MEM_BURST_LENGTH_CH1    0x00000007U
#define DMA2D_IN_MEM_BURST_LENGTH_CH1_M  (DMA2D_IN_MEM_BURST_LENGTH_CH1_V << DMA2D_IN_MEM_BURST_LENGTH_CH1_S)
#define DMA2D_IN_MEM_BURST_LENGTH_CH1_V  0x00000007U
#define DMA2D_IN_MEM_BURST_LENGTH_CH1_S  6
/** DMA2D_IN_MACRO_BLOCK_SIZE_CH1 : R/W; bitpos: [10:9]; default: 0;
 *  Sel RX macro-block size 0: 8pixel*8pixel     1: 8pixel*16pixel     2:
 *  16pixel*16pixel     3: no macro-block     , only useful in mode 1 of the link
 *  descriptor
 */
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH1    0x00000003U
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH1_M  (DMA2D_IN_MACRO_BLOCK_SIZE_CH1_V << DMA2D_IN_MACRO_BLOCK_SIZE_CH1_S)
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH1_V  0x00000003U
#define DMA2D_IN_MACRO_BLOCK_SIZE_CH1_S  9
/** DMA2D_IN_DSCR_PORT_EN_CH1 : R/W; bitpos: [11]; default: 0;
 *  Set this bit to 1 to obtain descriptor from IP port
 */
#define DMA2D_IN_DSCR_PORT_EN_CH1    (BIT(11))
#define DMA2D_IN_DSCR_PORT_EN_CH1_M  (DMA2D_IN_DSCR_PORT_EN_CH1_V << DMA2D_IN_DSCR_PORT_EN_CH1_S)
#define DMA2D_IN_DSCR_PORT_EN_CH1_V  0x00000001U
#define DMA2D_IN_DSCR_PORT_EN_CH1_S  11
/** DMA2D_IN_PAGE_BOUND_EN_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
 *  which define by mem_burst_length
 */
#define DMA2D_IN_PAGE_BOUND_EN_CH1    (BIT(12))
#define DMA2D_IN_PAGE_BOUND_EN_CH1_M  (DMA2D_IN_PAGE_BOUND_EN_CH1_V << DMA2D_IN_PAGE_BOUND_EN_CH1_S)
#define DMA2D_IN_PAGE_BOUND_EN_CH1_V  0x00000001U
#define DMA2D_IN_PAGE_BOUND_EN_CH1_S  12
/** DMA2D_IN_REORDER_EN_CH1 : R/W; bitpos: [16]; default: 0;
 *  Enable RX channel 0 macro block reorder when set to 1, only channel0 have this
 *  selection
 */
#define DMA2D_IN_REORDER_EN_CH1    (BIT(16))
#define DMA2D_IN_REORDER_EN_CH1_M  (DMA2D_IN_REORDER_EN_CH1_V << DMA2D_IN_REORDER_EN_CH1_S)
#define DMA2D_IN_REORDER_EN_CH1_V  0x00000001U
#define DMA2D_IN_REORDER_EN_CH1_S  16
/** DMA2D_IN_RST_CH1 : R/W; bitpos: [24]; default: 0;
 *  Write 1 then write 0 to this bit to reset Rx channel
 */
#define DMA2D_IN_RST_CH1    (BIT(24))
#define DMA2D_IN_RST_CH1_M  (DMA2D_IN_RST_CH1_V << DMA2D_IN_RST_CH1_S)
#define DMA2D_IN_RST_CH1_V  0x00000001U
#define DMA2D_IN_RST_CH1_S  24
/** DMA2D_IN_CMD_DISABLE_CH1 : R/W; bitpos: [25]; default: 0;
 *  Write 1 before reset and write 0 after reset
 */
#define DMA2D_IN_CMD_DISABLE_CH1    (BIT(25))
#define DMA2D_IN_CMD_DISABLE_CH1_M  (DMA2D_IN_CMD_DISABLE_CH1_V << DMA2D_IN_CMD_DISABLE_CH1_S)
#define DMA2D_IN_CMD_DISABLE_CH1_V  0x00000001U
#define DMA2D_IN_CMD_DISABLE_CH1_S  25
/** DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH1 : R/W; bitpos: [26]; default: 0;
 *  Set this bit to 1 to disable arbiter optimum weight function.
 */
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH1    (BIT(26))
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH1_M  (DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH1_V << DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH1_S)
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH1_V  0x00000001U
#define DMA2D_IN_ARB_WEIGHT_OPT_DIS_CH1_S  26

/** DMA2D_IN_INT_RAW_CH1_REG register
 *  Raw interrupt status of RX channel 0
 */
#define DMA2D_IN_INT_RAW_CH1_REG (DR_REG_DMA2D_BASE + 0x604)
/** DMA2D_IN_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been transmitted to peripherals for Rx channel 0.
 */
#define DMA2D_IN_DONE_CH1_INT_RAW    (BIT(0))
#define DMA2D_IN_DONE_CH1_INT_RAW_M  (DMA2D_IN_DONE_CH1_INT_RAW_V << DMA2D_IN_DONE_CH1_INT_RAW_S)
#define DMA2D_IN_DONE_CH1_INT_RAW_V  0x00000001U
#define DMA2D_IN_DONE_CH1_INT_RAW_S  0
/** DMA2D_IN_SUC_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and no data error is detected for Rx channel 0.
 */
#define DMA2D_IN_SUC_EOF_CH1_INT_RAW    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH1_INT_RAW_M  (DMA2D_IN_SUC_EOF_CH1_INT_RAW_V << DMA2D_IN_SUC_EOF_CH1_INT_RAW_S)
#define DMA2D_IN_SUC_EOF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH1_INT_RAW_S  1
/** DMA2D_IN_ERR_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received and data error is detected
 */
#define DMA2D_IN_ERR_EOF_CH1_INT_RAW    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH1_INT_RAW_M  (DMA2D_IN_ERR_EOF_CH1_INT_RAW_V << DMA2D_IN_ERR_EOF_CH1_INT_RAW_S)
#define DMA2D_IN_ERR_EOF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH1_INT_RAW_S  2
/** DMA2D_IN_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
 *  including owner error, the second and third word error of inlink descriptor for Rx
 *  channel 0.
 */
#define DMA2D_IN_DSCR_ERR_CH1_INT_RAW    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH1_INT_RAW_M  (DMA2D_IN_DSCR_ERR_CH1_INT_RAW_V << DMA2D_IN_DSCR_ERR_CH1_INT_RAW_S)
#define DMA2D_IN_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH1_INT_RAW_S  3
/** DMA2D_INFIFO_OVF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define DMA2D_INFIFO_OVF_L1_CH1_INT_RAW    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH1_INT_RAW_M  (DMA2D_INFIFO_OVF_L1_CH1_INT_RAW_V << DMA2D_INFIFO_OVF_L1_CH1_INT_RAW_S)
#define DMA2D_INFIFO_OVF_L1_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH1_INT_RAW_S  4
/** DMA2D_INFIFO_UDF_L1_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define DMA2D_INFIFO_UDF_L1_CH1_INT_RAW    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH1_INT_RAW_M  (DMA2D_INFIFO_UDF_L1_CH1_INT_RAW_V << DMA2D_INFIFO_UDF_L1_CH1_INT_RAW_S)
#define DMA2D_INFIFO_UDF_L1_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH1_INT_RAW_S  5
/** DMA2D_INFIFO_OVF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define DMA2D_INFIFO_OVF_L2_CH1_INT_RAW    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH1_INT_RAW_M  (DMA2D_INFIFO_OVF_L2_CH1_INT_RAW_V << DMA2D_INFIFO_OVF_L2_CH1_INT_RAW_S)
#define DMA2D_INFIFO_OVF_L2_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH1_INT_RAW_S  6
/** DMA2D_INFIFO_UDF_L2_CH1_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define DMA2D_INFIFO_UDF_L2_CH1_INT_RAW    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH1_INT_RAW_M  (DMA2D_INFIFO_UDF_L2_CH1_INT_RAW_V << DMA2D_INFIFO_UDF_L2_CH1_INT_RAW_S)
#define DMA2D_INFIFO_UDF_L2_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH1_INT_RAW_S  7
/** DMA2D_INFIFO_OVF_L3_CH1_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
 */
#define DMA2D_INFIFO_OVF_L3_CH1_INT_RAW    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH1_INT_RAW_M  (DMA2D_INFIFO_OVF_L3_CH1_INT_RAW_V << DMA2D_INFIFO_OVF_L3_CH1_INT_RAW_S)
#define DMA2D_INFIFO_OVF_L3_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH1_INT_RAW_S  8
/** DMA2D_INFIFO_UDF_L3_CH1_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
 */
#define DMA2D_INFIFO_UDF_L3_CH1_INT_RAW    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH1_INT_RAW_M  (DMA2D_INFIFO_UDF_L3_CH1_INT_RAW_V << DMA2D_INFIFO_UDF_L3_CH1_INT_RAW_S)
#define DMA2D_INFIFO_UDF_L3_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH1_INT_RAW_S  9
/** DMA2D_IN_DSCR_EMPTY_CH1_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
 *  also remain data.
 */
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_RAW    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_RAW_M  (DMA2D_IN_DSCR_EMPTY_CH1_INT_RAW_V << DMA2D_IN_DSCR_EMPTY_CH1_INT_RAW_S)
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_RAW_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_RAW_S  10
/** DMA2D_INFIFO_RO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is overflow.
 */
#define DMA2D_INFIFO_RO_OVF_CH1_INT_RAW    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH1_INT_RAW_M  (DMA2D_INFIFO_RO_OVF_CH1_INT_RAW_V << DMA2D_INFIFO_RO_OVF_CH1_INT_RAW_S)
#define DMA2D_INFIFO_RO_OVF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH1_INT_RAW_S  11
/** DMA2D_INFIFO_RO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  The raw interrupt bit turns to high level when reorder fifo is underflow.
 */
#define DMA2D_INFIFO_RO_UDF_CH1_INT_RAW    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH1_INT_RAW_M  (DMA2D_INFIFO_RO_UDF_CH1_INT_RAW_V << DMA2D_INFIFO_RO_UDF_CH1_INT_RAW_S)
#define DMA2D_INFIFO_RO_UDF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH1_INT_RAW_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_RAW    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_RAW_M  (DMA2D_IN_DSCR_TASK_OVF_CH1_INT_RAW_V << DMA2D_IN_DSCR_TASK_OVF_CH1_INT_RAW_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_RAW_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_RAW_S  13

/** DMA2D_IN_INT_ENA_CH1_REG register
 *  Interrupt enable bits of RX channel 0
 */
#define DMA2D_IN_INT_ENA_CH1_REG (DR_REG_DMA2D_BASE + 0x608)
/** DMA2D_IN_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define DMA2D_IN_DONE_CH1_INT_ENA    (BIT(0))
#define DMA2D_IN_DONE_CH1_INT_ENA_M  (DMA2D_IN_DONE_CH1_INT_ENA_V << DMA2D_IN_DONE_CH1_INT_ENA_S)
#define DMA2D_IN_DONE_CH1_INT_ENA_V  0x00000001U
#define DMA2D_IN_DONE_CH1_INT_ENA_S  0
/** DMA2D_IN_SUC_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_SUC_EOF_CH1_INT_ENA    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH1_INT_ENA_M  (DMA2D_IN_SUC_EOF_CH1_INT_ENA_V << DMA2D_IN_SUC_EOF_CH1_INT_ENA_S)
#define DMA2D_IN_SUC_EOF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH1_INT_ENA_S  1
/** DMA2D_IN_ERR_EOF_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_ERR_EOF_CH1_INT_ENA    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH1_INT_ENA_M  (DMA2D_IN_ERR_EOF_CH1_INT_ENA_V << DMA2D_IN_ERR_EOF_CH1_INT_ENA_S)
#define DMA2D_IN_ERR_EOF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH1_INT_ENA_S  2
/** DMA2D_IN_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_ERR_CH1_INT_ENA    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH1_INT_ENA_M  (DMA2D_IN_DSCR_ERR_CH1_INT_ENA_V << DMA2D_IN_DSCR_ERR_CH1_INT_ENA_S)
#define DMA2D_IN_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH1_INT_ENA_S  3
/** DMA2D_INFIFO_OVF_L1_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ENA    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ENA_M  (DMA2D_INFIFO_OVF_L1_CH1_INT_ENA_V << DMA2D_INFIFO_OVF_L1_CH1_INT_ENA_S)
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ENA_S  4
/** DMA2D_INFIFO_UDF_L1_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ENA    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ENA_M  (DMA2D_INFIFO_UDF_L1_CH1_INT_ENA_V << DMA2D_INFIFO_UDF_L1_CH1_INT_ENA_S)
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ENA_S  5
/** DMA2D_INFIFO_OVF_L2_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ENA    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ENA_M  (DMA2D_INFIFO_OVF_L2_CH1_INT_ENA_V << DMA2D_INFIFO_OVF_L2_CH1_INT_ENA_S)
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ENA_S  6
/** DMA2D_INFIFO_UDF_L2_CH1_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ENA    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ENA_M  (DMA2D_INFIFO_UDF_L2_CH1_INT_ENA_V << DMA2D_INFIFO_UDF_L2_CH1_INT_ENA_S)
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ENA_S  7
/** DMA2D_INFIFO_OVF_L3_CH1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ENA    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ENA_M  (DMA2D_INFIFO_OVF_L3_CH1_INT_ENA_V << DMA2D_INFIFO_OVF_L3_CH1_INT_ENA_S)
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ENA_S  8
/** DMA2D_INFIFO_UDF_L3_CH1_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ENA    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ENA_M  (DMA2D_INFIFO_UDF_L3_CH1_INT_ENA_V << DMA2D_INFIFO_UDF_L3_CH1_INT_ENA_S)
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ENA_S  9
/** DMA2D_IN_DSCR_EMPTY_CH1_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ENA    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ENA_M  (DMA2D_IN_DSCR_EMPTY_CH1_INT_ENA_V << DMA2D_IN_DSCR_EMPTY_CH1_INT_ENA_S)
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ENA_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ENA_S  10
/** DMA2D_INFIFO_RO_OVF_CH1_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the INFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ENA    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ENA_M  (DMA2D_INFIFO_RO_OVF_CH1_INT_ENA_V << DMA2D_INFIFO_RO_OVF_CH1_INT_ENA_S)
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ENA_S  11
/** DMA2D_INFIFO_RO_UDF_CH1_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the INFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ENA    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ENA_M  (DMA2D_INFIFO_RO_UDF_CH1_INT_ENA_V << DMA2D_INFIFO_RO_UDF_CH1_INT_ENA_S)
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ENA_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ENA    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ENA_M  (DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ENA_V << DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ENA_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ENA_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ENA_S  13

/** DMA2D_IN_INT_ST_CH1_REG register
 *  Masked interrupt status of RX channel 0
 */
#define DMA2D_IN_INT_ST_CH1_REG (DR_REG_DMA2D_BASE + 0x60c)
/** DMA2D_IN_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define DMA2D_IN_DONE_CH1_INT_ST    (BIT(0))
#define DMA2D_IN_DONE_CH1_INT_ST_M  (DMA2D_IN_DONE_CH1_INT_ST_V << DMA2D_IN_DONE_CH1_INT_ST_S)
#define DMA2D_IN_DONE_CH1_INT_ST_V  0x00000001U
#define DMA2D_IN_DONE_CH1_INT_ST_S  0
/** DMA2D_IN_SUC_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_SUC_EOF_CH1_INT_ST    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH1_INT_ST_M  (DMA2D_IN_SUC_EOF_CH1_INT_ST_V << DMA2D_IN_SUC_EOF_CH1_INT_ST_S)
#define DMA2D_IN_SUC_EOF_CH1_INT_ST_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH1_INT_ST_S  1
/** DMA2D_IN_ERR_EOF_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_ERR_EOF_CH1_INT_ST    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH1_INT_ST_M  (DMA2D_IN_ERR_EOF_CH1_INT_ST_V << DMA2D_IN_ERR_EOF_CH1_INT_ST_S)
#define DMA2D_IN_ERR_EOF_CH1_INT_ST_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH1_INT_ST_S  2
/** DMA2D_IN_DSCR_ERR_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_ERR_CH1_INT_ST    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH1_INT_ST_M  (DMA2D_IN_DSCR_ERR_CH1_INT_ST_V << DMA2D_IN_DSCR_ERR_CH1_INT_ST_S)
#define DMA2D_IN_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH1_INT_ST_S  3
/** DMA2D_INFIFO_OVF_L1_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ST    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ST_M  (DMA2D_INFIFO_OVF_L1_CH1_INT_ST_V << DMA2D_INFIFO_OVF_L1_CH1_INT_ST_S)
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH1_INT_ST_S  4
/** DMA2D_INFIFO_UDF_L1_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ST    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ST_M  (DMA2D_INFIFO_UDF_L1_CH1_INT_ST_V << DMA2D_INFIFO_UDF_L1_CH1_INT_ST_S)
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH1_INT_ST_S  5
/** DMA2D_INFIFO_OVF_L2_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ST    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ST_M  (DMA2D_INFIFO_OVF_L2_CH1_INT_ST_V << DMA2D_INFIFO_OVF_L2_CH1_INT_ST_S)
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH1_INT_ST_S  6
/** DMA2D_INFIFO_UDF_L2_CH1_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ST    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ST_M  (DMA2D_INFIFO_UDF_L2_CH1_INT_ST_V << DMA2D_INFIFO_UDF_L2_CH1_INT_ST_S)
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH1_INT_ST_S  7
/** DMA2D_INFIFO_OVF_L3_CH1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ST    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ST_M  (DMA2D_INFIFO_OVF_L3_CH1_INT_ST_V << DMA2D_INFIFO_OVF_L3_CH1_INT_ST_S)
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH1_INT_ST_S  8
/** DMA2D_INFIFO_UDF_L3_CH1_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ST    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ST_M  (DMA2D_INFIFO_UDF_L3_CH1_INT_ST_V << DMA2D_INFIFO_UDF_L3_CH1_INT_ST_S)
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH1_INT_ST_S  9
/** DMA2D_IN_DSCR_EMPTY_CH1_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ST    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ST_M  (DMA2D_IN_DSCR_EMPTY_CH1_INT_ST_V << DMA2D_IN_DSCR_EMPTY_CH1_INT_ST_S)
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ST_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_ST_S  10
/** DMA2D_INFIFO_RO_OVF_CH1_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the INFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ST    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ST_M  (DMA2D_INFIFO_RO_OVF_CH1_INT_ST_V << DMA2D_INFIFO_RO_OVF_CH1_INT_ST_S)
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH1_INT_ST_S  11
/** DMA2D_INFIFO_RO_UDF_CH1_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the INFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ST    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ST_M  (DMA2D_INFIFO_RO_UDF_CH1_INT_ST_V << DMA2D_INFIFO_RO_UDF_CH1_INT_ST_S)
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ST_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH1_INT_ST_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ST : RO; bitpos: [13]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ST    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ST_M  (DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ST_V << DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ST_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ST_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_ST_S  13

/** DMA2D_IN_INT_CLR_CH1_REG register
 *  Interrupt clear bits of RX channel 0
 */
#define DMA2D_IN_INT_CLR_CH1_REG (DR_REG_DMA2D_BASE + 0x610)
/** DMA2D_IN_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define DMA2D_IN_DONE_CH1_INT_CLR    (BIT(0))
#define DMA2D_IN_DONE_CH1_INT_CLR_M  (DMA2D_IN_DONE_CH1_INT_CLR_V << DMA2D_IN_DONE_CH1_INT_CLR_S)
#define DMA2D_IN_DONE_CH1_INT_CLR_V  0x00000001U
#define DMA2D_IN_DONE_CH1_INT_CLR_S  0
/** DMA2D_IN_SUC_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_SUC_EOF_CH1_INT_CLR    (BIT(1))
#define DMA2D_IN_SUC_EOF_CH1_INT_CLR_M  (DMA2D_IN_SUC_EOF_CH1_INT_CLR_V << DMA2D_IN_SUC_EOF_CH1_INT_CLR_S)
#define DMA2D_IN_SUC_EOF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_IN_SUC_EOF_CH1_INT_CLR_S  1
/** DMA2D_IN_ERR_EOF_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define DMA2D_IN_ERR_EOF_CH1_INT_CLR    (BIT(2))
#define DMA2D_IN_ERR_EOF_CH1_INT_CLR_M  (DMA2D_IN_ERR_EOF_CH1_INT_CLR_V << DMA2D_IN_ERR_EOF_CH1_INT_CLR_S)
#define DMA2D_IN_ERR_EOF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_IN_ERR_EOF_CH1_INT_CLR_S  2
/** DMA2D_IN_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_ERR_CH1_INT_CLR    (BIT(3))
#define DMA2D_IN_DSCR_ERR_CH1_INT_CLR_M  (DMA2D_IN_DSCR_ERR_CH1_INT_CLR_V << DMA2D_IN_DSCR_ERR_CH1_INT_CLR_S)
#define DMA2D_IN_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define DMA2D_IN_DSCR_ERR_CH1_INT_CLR_S  3
/** DMA2D_INFIFO_OVF_L1_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L1_CH1_INT_CLR    (BIT(4))
#define DMA2D_INFIFO_OVF_L1_CH1_INT_CLR_M  (DMA2D_INFIFO_OVF_L1_CH1_INT_CLR_V << DMA2D_INFIFO_OVF_L1_CH1_INT_CLR_S)
#define DMA2D_INFIFO_OVF_L1_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_OVF_L1_CH1_INT_CLR_S  4
/** DMA2D_INFIFO_UDF_L1_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L1_CH1_INT_CLR    (BIT(5))
#define DMA2D_INFIFO_UDF_L1_CH1_INT_CLR_M  (DMA2D_INFIFO_UDF_L1_CH1_INT_CLR_V << DMA2D_INFIFO_UDF_L1_CH1_INT_CLR_S)
#define DMA2D_INFIFO_UDF_L1_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_UDF_L1_CH1_INT_CLR_S  5
/** DMA2D_INFIFO_OVF_L2_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L2_CH1_INT_CLR    (BIT(6))
#define DMA2D_INFIFO_OVF_L2_CH1_INT_CLR_M  (DMA2D_INFIFO_OVF_L2_CH1_INT_CLR_V << DMA2D_INFIFO_OVF_L2_CH1_INT_CLR_S)
#define DMA2D_INFIFO_OVF_L2_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_OVF_L2_CH1_INT_CLR_S  6
/** DMA2D_INFIFO_UDF_L2_CH1_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L2_CH1_INT_CLR    (BIT(7))
#define DMA2D_INFIFO_UDF_L2_CH1_INT_CLR_M  (DMA2D_INFIFO_UDF_L2_CH1_INT_CLR_V << DMA2D_INFIFO_UDF_L2_CH1_INT_CLR_S)
#define DMA2D_INFIFO_UDF_L2_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_UDF_L2_CH1_INT_CLR_S  7
/** DMA2D_INFIFO_OVF_L3_CH1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_OVF_L3_CH1_INT_CLR    (BIT(8))
#define DMA2D_INFIFO_OVF_L3_CH1_INT_CLR_M  (DMA2D_INFIFO_OVF_L3_CH1_INT_CLR_V << DMA2D_INFIFO_OVF_L3_CH1_INT_CLR_S)
#define DMA2D_INFIFO_OVF_L3_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_OVF_L3_CH1_INT_CLR_S  8
/** DMA2D_INFIFO_UDF_L3_CH1_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define DMA2D_INFIFO_UDF_L3_CH1_INT_CLR    (BIT(9))
#define DMA2D_INFIFO_UDF_L3_CH1_INT_CLR_M  (DMA2D_INFIFO_UDF_L3_CH1_INT_CLR_V << DMA2D_INFIFO_UDF_L3_CH1_INT_CLR_S)
#define DMA2D_INFIFO_UDF_L3_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_UDF_L3_CH1_INT_CLR_S  9
/** DMA2D_IN_DSCR_EMPTY_CH1_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_CLR    (BIT(10))
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_CLR_M  (DMA2D_IN_DSCR_EMPTY_CH1_INT_CLR_V << DMA2D_IN_DSCR_EMPTY_CH1_INT_CLR_S)
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_CLR_V  0x00000001U
#define DMA2D_IN_DSCR_EMPTY_CH1_INT_CLR_S  10
/** DMA2D_INFIFO_RO_OVF_CH1_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the INFIFO_RO_OVF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_OVF_CH1_INT_CLR    (BIT(11))
#define DMA2D_INFIFO_RO_OVF_CH1_INT_CLR_M  (DMA2D_INFIFO_RO_OVF_CH1_INT_CLR_V << DMA2D_INFIFO_RO_OVF_CH1_INT_CLR_S)
#define DMA2D_INFIFO_RO_OVF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_RO_OVF_CH1_INT_CLR_S  11
/** DMA2D_INFIFO_RO_UDF_CH1_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the INFIFO_RO_UDF_CH_INT interrupt.
 */
#define DMA2D_INFIFO_RO_UDF_CH1_INT_CLR    (BIT(12))
#define DMA2D_INFIFO_RO_UDF_CH1_INT_CLR_M  (DMA2D_INFIFO_RO_UDF_CH1_INT_CLR_V << DMA2D_INFIFO_RO_UDF_CH1_INT_CLR_S)
#define DMA2D_INFIFO_RO_UDF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_INFIFO_RO_UDF_CH1_INT_CLR_S  12
/** DMA2D_IN_DSCR_TASK_OVF_CH1_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
 */
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_CLR    (BIT(13))
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_CLR_M  (DMA2D_IN_DSCR_TASK_OVF_CH1_INT_CLR_V << DMA2D_IN_DSCR_TASK_OVF_CH1_INT_CLR_S)
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_CLR_V  0x00000001U
#define DMA2D_IN_DSCR_TASK_OVF_CH1_INT_CLR_S  13

/** DMA2D_INFIFO_STATUS_CH1_REG register
 *  Represents the status of the rx fifo of channel 0
 */
#define DMA2D_INFIFO_STATUS_CH1_REG (DR_REG_DMA2D_BASE + 0x614)
/** DMA2D_INFIFO_FULL_L2_CH1 : RO; bitpos: [0]; default: 0;
 *  Rx FIFO full signal for Rx channel.
 */
#define DMA2D_INFIFO_FULL_L2_CH1    (BIT(0))
#define DMA2D_INFIFO_FULL_L2_CH1_M  (DMA2D_INFIFO_FULL_L2_CH1_V << DMA2D_INFIFO_FULL_L2_CH1_S)
#define DMA2D_INFIFO_FULL_L2_CH1_V  0x00000001U
#define DMA2D_INFIFO_FULL_L2_CH1_S  0
/** DMA2D_INFIFO_EMPTY_L2_CH1 : RO; bitpos: [1]; default: 1;
 *  Rx FIFO empty signal for Rx channel.
 */
#define DMA2D_INFIFO_EMPTY_L2_CH1    (BIT(1))
#define DMA2D_INFIFO_EMPTY_L2_CH1_M  (DMA2D_INFIFO_EMPTY_L2_CH1_V << DMA2D_INFIFO_EMPTY_L2_CH1_S)
#define DMA2D_INFIFO_EMPTY_L2_CH1_V  0x00000001U
#define DMA2D_INFIFO_EMPTY_L2_CH1_S  1
/** DMA2D_INFIFO_CNT_L2_CH1 : RO; bitpos: [5:2]; default: 0;
 *  The register stores the byte number of the data in Rx FIFO for Rx channel.
 */
#define DMA2D_INFIFO_CNT_L2_CH1    0x0000000FU
#define DMA2D_INFIFO_CNT_L2_CH1_M  (DMA2D_INFIFO_CNT_L2_CH1_V << DMA2D_INFIFO_CNT_L2_CH1_S)
#define DMA2D_INFIFO_CNT_L2_CH1_V  0x0000000FU
#define DMA2D_INFIFO_CNT_L2_CH1_S  2
/** DMA2D_IN_REMAIN_UNDER_1B_CH1 : RO; bitpos: [7]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_1B_CH1    (BIT(7))
#define DMA2D_IN_REMAIN_UNDER_1B_CH1_M  (DMA2D_IN_REMAIN_UNDER_1B_CH1_V << DMA2D_IN_REMAIN_UNDER_1B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_1B_CH1_S  7
/** DMA2D_IN_REMAIN_UNDER_2B_CH1 : RO; bitpos: [8]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_2B_CH1    (BIT(8))
#define DMA2D_IN_REMAIN_UNDER_2B_CH1_M  (DMA2D_IN_REMAIN_UNDER_2B_CH1_V << DMA2D_IN_REMAIN_UNDER_2B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_2B_CH1_S  8
/** DMA2D_IN_REMAIN_UNDER_3B_CH1 : RO; bitpos: [9]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_3B_CH1    (BIT(9))
#define DMA2D_IN_REMAIN_UNDER_3B_CH1_M  (DMA2D_IN_REMAIN_UNDER_3B_CH1_V << DMA2D_IN_REMAIN_UNDER_3B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_3B_CH1_S  9
/** DMA2D_IN_REMAIN_UNDER_4B_CH1 : RO; bitpos: [10]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_4B_CH1    (BIT(10))
#define DMA2D_IN_REMAIN_UNDER_4B_CH1_M  (DMA2D_IN_REMAIN_UNDER_4B_CH1_V << DMA2D_IN_REMAIN_UNDER_4B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_4B_CH1_S  10
/** DMA2D_IN_REMAIN_UNDER_5B_CH1 : RO; bitpos: [11]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_5B_CH1    (BIT(11))
#define DMA2D_IN_REMAIN_UNDER_5B_CH1_M  (DMA2D_IN_REMAIN_UNDER_5B_CH1_V << DMA2D_IN_REMAIN_UNDER_5B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_5B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_5B_CH1_S  11
/** DMA2D_IN_REMAIN_UNDER_6B_CH1 : RO; bitpos: [12]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_6B_CH1    (BIT(12))
#define DMA2D_IN_REMAIN_UNDER_6B_CH1_M  (DMA2D_IN_REMAIN_UNDER_6B_CH1_V << DMA2D_IN_REMAIN_UNDER_6B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_6B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_6B_CH1_S  12
/** DMA2D_IN_REMAIN_UNDER_7B_CH1 : RO; bitpos: [13]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_7B_CH1    (BIT(13))
#define DMA2D_IN_REMAIN_UNDER_7B_CH1_M  (DMA2D_IN_REMAIN_UNDER_7B_CH1_V << DMA2D_IN_REMAIN_UNDER_7B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_7B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_7B_CH1_S  13
/** DMA2D_IN_REMAIN_UNDER_8B_CH1 : RO; bitpos: [14]; default: 0;
 *  reserved
 */
#define DMA2D_IN_REMAIN_UNDER_8B_CH1    (BIT(14))
#define DMA2D_IN_REMAIN_UNDER_8B_CH1_M  (DMA2D_IN_REMAIN_UNDER_8B_CH1_V << DMA2D_IN_REMAIN_UNDER_8B_CH1_S)
#define DMA2D_IN_REMAIN_UNDER_8B_CH1_V  0x00000001U
#define DMA2D_IN_REMAIN_UNDER_8B_CH1_S  14
/** DMA2D_INFIFO_FULL_L1_CH1 : RO; bitpos: [15]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_INFIFO_FULL_L1_CH1    (BIT(15))
#define DMA2D_INFIFO_FULL_L1_CH1_M  (DMA2D_INFIFO_FULL_L1_CH1_V << DMA2D_INFIFO_FULL_L1_CH1_S)
#define DMA2D_INFIFO_FULL_L1_CH1_V  0x00000001U
#define DMA2D_INFIFO_FULL_L1_CH1_S  15
/** DMA2D_INFIFO_EMPTY_L1_CH1 : RO; bitpos: [16]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_INFIFO_EMPTY_L1_CH1    (BIT(16))
#define DMA2D_INFIFO_EMPTY_L1_CH1_M  (DMA2D_INFIFO_EMPTY_L1_CH1_V << DMA2D_INFIFO_EMPTY_L1_CH1_S)
#define DMA2D_INFIFO_EMPTY_L1_CH1_V  0x00000001U
#define DMA2D_INFIFO_EMPTY_L1_CH1_S  16
/** DMA2D_INFIFO_CNT_L1_CH1 : RO; bitpos: [21:17]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_INFIFO_CNT_L1_CH1    0x0000001FU
#define DMA2D_INFIFO_CNT_L1_CH1_M  (DMA2D_INFIFO_CNT_L1_CH1_V << DMA2D_INFIFO_CNT_L1_CH1_S)
#define DMA2D_INFIFO_CNT_L1_CH1_V  0x0000001FU
#define DMA2D_INFIFO_CNT_L1_CH1_S  17
/** DMA2D_INFIFO_FULL_L3_CH1 : RO; bitpos: [22]; default: 0;
 *  Tx FIFO full signal for Tx channel 0.
 */
#define DMA2D_INFIFO_FULL_L3_CH1    (BIT(22))
#define DMA2D_INFIFO_FULL_L3_CH1_M  (DMA2D_INFIFO_FULL_L3_CH1_V << DMA2D_INFIFO_FULL_L3_CH1_S)
#define DMA2D_INFIFO_FULL_L3_CH1_V  0x00000001U
#define DMA2D_INFIFO_FULL_L3_CH1_S  22
/** DMA2D_INFIFO_EMPTY_L3_CH1 : RO; bitpos: [23]; default: 1;
 *  Tx FIFO empty signal for Tx channel 0.
 */
#define DMA2D_INFIFO_EMPTY_L3_CH1    (BIT(23))
#define DMA2D_INFIFO_EMPTY_L3_CH1_M  (DMA2D_INFIFO_EMPTY_L3_CH1_V << DMA2D_INFIFO_EMPTY_L3_CH1_S)
#define DMA2D_INFIFO_EMPTY_L3_CH1_V  0x00000001U
#define DMA2D_INFIFO_EMPTY_L3_CH1_S  23
/** DMA2D_INFIFO_CNT_L3_CH1 : RO; bitpos: [28:24]; default: 0;
 *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
 */
#define DMA2D_INFIFO_CNT_L3_CH1    0x0000001FU
#define DMA2D_INFIFO_CNT_L3_CH1_M  (DMA2D_INFIFO_CNT_L3_CH1_V << DMA2D_INFIFO_CNT_L3_CH1_S)
#define DMA2D_INFIFO_CNT_L3_CH1_V  0x0000001FU
#define DMA2D_INFIFO_CNT_L3_CH1_S  24

/** DMA2D_IN_POP_CH1_REG register
 *  Configures the rx fifo of channel 0
 */
#define DMA2D_IN_POP_CH1_REG (DR_REG_DMA2D_BASE + 0x618)
/** DMA2D_INFIFO_RDATA_CH1 : RO; bitpos: [10:0]; default: 1024;
 *  This register stores the data popping from DMA Rx FIFO.
 */
#define DMA2D_INFIFO_RDATA_CH1    0x000007FFU
#define DMA2D_INFIFO_RDATA_CH1_M  (DMA2D_INFIFO_RDATA_CH1_V << DMA2D_INFIFO_RDATA_CH1_S)
#define DMA2D_INFIFO_RDATA_CH1_V  0x000007FFU
#define DMA2D_INFIFO_RDATA_CH1_S  0
/** DMA2D_INFIFO_POP_CH1 : R/W/SC; bitpos: [11]; default: 0;
 *  Set this bit to pop data from DMA Rx FIFO.
 */
#define DMA2D_INFIFO_POP_CH1    (BIT(11))
#define DMA2D_INFIFO_POP_CH1_M  (DMA2D_INFIFO_POP_CH1_V << DMA2D_INFIFO_POP_CH1_S)
#define DMA2D_INFIFO_POP_CH1_V  0x00000001U
#define DMA2D_INFIFO_POP_CH1_S  11

/** DMA2D_IN_LINK_CONF_CH1_REG register
 *  Configures the rx descriptor operations of channel 0
 */
#define DMA2D_IN_LINK_CONF_CH1_REG (DR_REG_DMA2D_BASE + 0x61c)
/** DMA2D_INLINK_AUTO_RET_CH1 : R/W; bitpos: [20]; default: 1;
 *  Configure the value of the owner field written back to the inlink descriptor.
 *  1: Write back 1. 0: Write back 0.
 */
#define DMA2D_INLINK_AUTO_RET_CH1    (BIT(20))
#define DMA2D_INLINK_AUTO_RET_CH1_M  (DMA2D_INLINK_AUTO_RET_CH1_V << DMA2D_INLINK_AUTO_RET_CH1_S)
#define DMA2D_INLINK_AUTO_RET_CH1_V  0x00000001U
#define DMA2D_INLINK_AUTO_RET_CH1_S  20
/** DMA2D_INLINK_STOP_CH1 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define DMA2D_INLINK_STOP_CH1    (BIT(21))
#define DMA2D_INLINK_STOP_CH1_M  (DMA2D_INLINK_STOP_CH1_V << DMA2D_INLINK_STOP_CH1_S)
#define DMA2D_INLINK_STOP_CH1_V  0x00000001U
#define DMA2D_INLINK_STOP_CH1_S  21
/** DMA2D_INLINK_START_CH1 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define DMA2D_INLINK_START_CH1    (BIT(22))
#define DMA2D_INLINK_START_CH1_M  (DMA2D_INLINK_START_CH1_V << DMA2D_INLINK_START_CH1_S)
#define DMA2D_INLINK_START_CH1_V  0x00000001U
#define DMA2D_INLINK_START_CH1_S  22
/** DMA2D_INLINK_RESTART_CH1 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define DMA2D_INLINK_RESTART_CH1    (BIT(23))
#define DMA2D_INLINK_RESTART_CH1_M  (DMA2D_INLINK_RESTART_CH1_V << DMA2D_INLINK_RESTART_CH1_S)
#define DMA2D_INLINK_RESTART_CH1_V  0x00000001U
#define DMA2D_INLINK_RESTART_CH1_S  23
/** DMA2D_INLINK_PARK_CH1 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define DMA2D_INLINK_PARK_CH1    (BIT(24))
#define DMA2D_INLINK_PARK_CH1_M  (DMA2D_INLINK_PARK_CH1_V << DMA2D_INLINK_PARK_CH1_S)
#define DMA2D_INLINK_PARK_CH1_V  0x00000001U
#define DMA2D_INLINK_PARK_CH1_S  24

/** DMA2D_IN_LINK_ADDR_CH1_REG register
 *  Configures the rx descriptor address of channel 0
 */
#define DMA2D_IN_LINK_ADDR_CH1_REG (DR_REG_DMA2D_BASE + 0x620)
/** DMA2D_INLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the first inlink descriptor's address.
 */
#define DMA2D_INLINK_ADDR_CH1    0xFFFFFFFFU
#define DMA2D_INLINK_ADDR_CH1_M  (DMA2D_INLINK_ADDR_CH1_V << DMA2D_INLINK_ADDR_CH1_S)
#define DMA2D_INLINK_ADDR_CH1_V  0xFFFFFFFFU
#define DMA2D_INLINK_ADDR_CH1_S  0

/** DMA2D_IN_STATE_CH1_REG register
 *  Represents the working status of the rx descriptor of channel 0
 */
#define DMA2D_IN_STATE_CH1_REG (DR_REG_DMA2D_BASE + 0x624)
/** DMA2D_INLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define DMA2D_INLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define DMA2D_INLINK_DSCR_ADDR_CH1_M  (DMA2D_INLINK_DSCR_ADDR_CH1_V << DMA2D_INLINK_DSCR_ADDR_CH1_S)
#define DMA2D_INLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define DMA2D_INLINK_DSCR_ADDR_CH1_S  0
/** DMA2D_IN_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define DMA2D_IN_DSCR_STATE_CH1    0x00000003U
#define DMA2D_IN_DSCR_STATE_CH1_M  (DMA2D_IN_DSCR_STATE_CH1_V << DMA2D_IN_DSCR_STATE_CH1_S)
#define DMA2D_IN_DSCR_STATE_CH1_V  0x00000003U
#define DMA2D_IN_DSCR_STATE_CH1_S  18
/** DMA2D_IN_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define DMA2D_IN_STATE_CH1    0x00000007U
#define DMA2D_IN_STATE_CH1_M  (DMA2D_IN_STATE_CH1_V << DMA2D_IN_STATE_CH1_S)
#define DMA2D_IN_STATE_CH1_V  0x00000007U
#define DMA2D_IN_STATE_CH1_S  20
/** DMA2D_IN_RESET_AVAIL_CH1 : RO; bitpos: [23]; default: 1;
 *  This register indicate that if the channel reset is safety.
 */
#define DMA2D_IN_RESET_AVAIL_CH1    (BIT(23))
#define DMA2D_IN_RESET_AVAIL_CH1_M  (DMA2D_IN_RESET_AVAIL_CH1_V << DMA2D_IN_RESET_AVAIL_CH1_S)
#define DMA2D_IN_RESET_AVAIL_CH1_V  0x00000001U
#define DMA2D_IN_RESET_AVAIL_CH1_S  23

/** DMA2D_IN_SUC_EOF_DES_ADDR_CH1_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH1_REG (DR_REG_DMA2D_BASE + 0x628)
/** DMA2D_IN_SUC_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH1_M  (DMA2D_IN_SUC_EOF_DES_ADDR_CH1_V << DMA2D_IN_SUC_EOF_DES_ADDR_CH1_S)
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define DMA2D_IN_SUC_EOF_DES_ADDR_CH1_S  0

/** DMA2D_IN_ERR_EOF_DES_ADDR_CH1_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH1_REG (DR_REG_DMA2D_BASE + 0x62c)
/** DMA2D_IN_ERR_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data.
 */
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH1_M  (DMA2D_IN_ERR_EOF_DES_ADDR_CH1_V << DMA2D_IN_ERR_EOF_DES_ADDR_CH1_S)
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define DMA2D_IN_ERR_EOF_DES_ADDR_CH1_S  0

/** DMA2D_IN_DSCR_CH1_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_DSCR_CH1_REG (DR_REG_DMA2D_BASE + 0x630)
/** DMA2D_INLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the next inlink descriptor address x.
 */
#define DMA2D_INLINK_DSCR_CH1    0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_CH1_M  (DMA2D_INLINK_DSCR_CH1_V << DMA2D_INLINK_DSCR_CH1_S)
#define DMA2D_INLINK_DSCR_CH1_V  0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_CH1_S  0

/** DMA2D_IN_DSCR_BF0_CH1_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_DSCR_BF0_CH1_REG (DR_REG_DMA2D_BASE + 0x634)
/** DMA2D_INLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor's next address x-1.
 */
#define DMA2D_INLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF0_CH1_M  (DMA2D_INLINK_DSCR_BF0_CH1_V << DMA2D_INLINK_DSCR_BF0_CH1_S)
#define DMA2D_INLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF0_CH1_S  0

/** DMA2D_IN_DSCR_BF1_CH1_REG register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
#define DMA2D_IN_DSCR_BF1_CH1_REG (DR_REG_DMA2D_BASE + 0x638)
/** DMA2D_INLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor's next address x-2.
 */
#define DMA2D_INLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF1_CH1_M  (DMA2D_INLINK_DSCR_BF1_CH1_V << DMA2D_INLINK_DSCR_BF1_CH1_S)
#define DMA2D_INLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define DMA2D_INLINK_DSCR_BF1_CH1_S  0

/** DMA2D_IN_PERI_SEL_CH1_REG register
 *  Configures the rx peripheral of channel 0
 */
#define DMA2D_IN_PERI_SEL_CH1_REG (DR_REG_DMA2D_BASE + 0x63c)
/** DMA2D_IN_PERI_SEL_CH1 : R/W; bitpos: [2:0]; default: 7;
 *  This register is used to select peripheral for Rx channel   0:  jpeg     1:
 *  display-1     2: display-2        7: no choose
 */
#define DMA2D_IN_PERI_SEL_CH1    0x00000007U
#define DMA2D_IN_PERI_SEL_CH1_M  (DMA2D_IN_PERI_SEL_CH1_V << DMA2D_IN_PERI_SEL_CH1_S)
#define DMA2D_IN_PERI_SEL_CH1_V  0x00000007U
#define DMA2D_IN_PERI_SEL_CH1_S  0

/** DMA2D_IN_ARB_CH1_REG register
 *  Configures the rx arbiter of channel 0
 */
#define DMA2D_IN_ARB_CH1_REG (DR_REG_DMA2D_BASE + 0x640)
/** DMA2D_IN_ARB_TOKEN_NUM_CH1 : R/W; bitpos: [3:0]; default: 1;
 *  Set the max number of token count of arbiter
 */
#define DMA2D_IN_ARB_TOKEN_NUM_CH1    0x0000000FU
#define DMA2D_IN_ARB_TOKEN_NUM_CH1_M  (DMA2D_IN_ARB_TOKEN_NUM_CH1_V << DMA2D_IN_ARB_TOKEN_NUM_CH1_S)
#define DMA2D_IN_ARB_TOKEN_NUM_CH1_V  0x0000000FU
#define DMA2D_IN_ARB_TOKEN_NUM_CH1_S  0
/** DMA2D_IN_ARB_PRIORITY_CH1 : R/W; bitpos: [4]; default: 1;
 *  Set the priority of channel
 */
#define DMA2D_IN_ARB_PRIORITY_CH1    (BIT(4))
#define DMA2D_IN_ARB_PRIORITY_CH1_M  (DMA2D_IN_ARB_PRIORITY_CH1_V << DMA2D_IN_ARB_PRIORITY_CH1_S)
#define DMA2D_IN_ARB_PRIORITY_CH1_V  0x00000001U
#define DMA2D_IN_ARB_PRIORITY_CH1_S  4

/** DMA2D_IN_RO_STATUS_CH1_REG register
 *  Represents the status of the rx reorder module of channel 0
 */
#define DMA2D_IN_RO_STATUS_CH1_REG (DR_REG_DMA2D_BASE + 0x644)
/** DMA2D_INFIFO_RO_CNT_CH1 : RO; bitpos: [4:0]; default: 0;
 *  The register stores the byte number of the data in color convert Rx FIFO for
 *  channel 0.
 */
#define DMA2D_INFIFO_RO_CNT_CH1    0x0000001FU
#define DMA2D_INFIFO_RO_CNT_CH1_M  (DMA2D_INFIFO_RO_CNT_CH1_V << DMA2D_INFIFO_RO_CNT_CH1_S)
#define DMA2D_INFIFO_RO_CNT_CH1_V  0x0000001FU
#define DMA2D_INFIFO_RO_CNT_CH1_S  0
/** DMA2D_IN_RO_WR_STATE_CH1 : RO; bitpos: [6:5]; default: 0;
 *  The register stores the state of read ram of reorder
 */
#define DMA2D_IN_RO_WR_STATE_CH1    0x00000003U
#define DMA2D_IN_RO_WR_STATE_CH1_M  (DMA2D_IN_RO_WR_STATE_CH1_V << DMA2D_IN_RO_WR_STATE_CH1_S)
#define DMA2D_IN_RO_WR_STATE_CH1_V  0x00000003U
#define DMA2D_IN_RO_WR_STATE_CH1_S  5
/** DMA2D_IN_RO_RD_STATE_CH1 : RO; bitpos: [8:7]; default: 0;
 *  The register stores the state of write ram of reorder
 */
#define DMA2D_IN_RO_RD_STATE_CH1    0x00000003U
#define DMA2D_IN_RO_RD_STATE_CH1_M  (DMA2D_IN_RO_RD_STATE_CH1_V << DMA2D_IN_RO_RD_STATE_CH1_S)
#define DMA2D_IN_RO_RD_STATE_CH1_V  0x00000003U
#define DMA2D_IN_RO_RD_STATE_CH1_S  7
/** DMA2D_IN_PIXEL_BYTE_CH1 : RO; bitpos: [12:9]; default: 0;
 *  the number of bytes contained in a pixel at RX channel     0: 1byte     1: 1.5bytes
 *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
 */
#define DMA2D_IN_PIXEL_BYTE_CH1    0x0000000FU
#define DMA2D_IN_PIXEL_BYTE_CH1_M  (DMA2D_IN_PIXEL_BYTE_CH1_V << DMA2D_IN_PIXEL_BYTE_CH1_S)
#define DMA2D_IN_PIXEL_BYTE_CH1_V  0x0000000FU
#define DMA2D_IN_PIXEL_BYTE_CH1_S  9
/** DMA2D_IN_BURST_BLOCK_NUM_CH1 : RO; bitpos: [16:13]; default: 0;
 *  the number of macro blocks contained in a burst of data at RX channel
 */
#define DMA2D_IN_BURST_BLOCK_NUM_CH1    0x0000000FU
#define DMA2D_IN_BURST_BLOCK_NUM_CH1_M  (DMA2D_IN_BURST_BLOCK_NUM_CH1_V << DMA2D_IN_BURST_BLOCK_NUM_CH1_S)
#define DMA2D_IN_BURST_BLOCK_NUM_CH1_V  0x0000000FU
#define DMA2D_IN_BURST_BLOCK_NUM_CH1_S  13

/** DMA2D_IN_ETM_CONF_CH1_REG register
 *  Configures the rx etm of channel 0
 */
#define DMA2D_IN_ETM_CONF_CH1_REG (DR_REG_DMA2D_BASE + 0x648)
/** DMA2D_IN_ETM_EN_CH1 : R/W; bitpos: [0]; default: 0;
 *  Configures the enable of the etm function, 1 is enable.
 */
#define DMA2D_IN_ETM_EN_CH1    (BIT(0))
#define DMA2D_IN_ETM_EN_CH1_M  (DMA2D_IN_ETM_EN_CH1_V << DMA2D_IN_ETM_EN_CH1_S)
#define DMA2D_IN_ETM_EN_CH1_V  0x00000001U
#define DMA2D_IN_ETM_EN_CH1_S  0
/** DMA2D_IN_ETM_LOOP_EN_CH1 : R/W; bitpos: [1]; default: 0;
 *  Configures the enable of the descriptors loop etm function, 1 is enable.
 */
#define DMA2D_IN_ETM_LOOP_EN_CH1    (BIT(1))
#define DMA2D_IN_ETM_LOOP_EN_CH1_M  (DMA2D_IN_ETM_LOOP_EN_CH1_V << DMA2D_IN_ETM_LOOP_EN_CH1_S)
#define DMA2D_IN_ETM_LOOP_EN_CH1_V  0x00000001U
#define DMA2D_IN_ETM_LOOP_EN_CH1_S  1
/** DMA2D_IN_DSCR_TASK_MAK_CH1 : R/W; bitpos: [3:2]; default: 1;
 *  Configures the maximum number of cacheable descriptors.
 */
#define DMA2D_IN_DSCR_TASK_MAK_CH1    0x00000003U
#define DMA2D_IN_DSCR_TASK_MAK_CH1_M  (DMA2D_IN_DSCR_TASK_MAK_CH1_V << DMA2D_IN_DSCR_TASK_MAK_CH1_S)
#define DMA2D_IN_DSCR_TASK_MAK_CH1_V  0x00000003U
#define DMA2D_IN_DSCR_TASK_MAK_CH1_S  2

/** DMA2D_AXI_ERR_REG register
 *  Represents the status of th axi bus
 */
#define DMA2D_AXI_ERR_REG (DR_REG_DMA2D_BASE + 0xa00)
/** DMA2D_RID_ERR_CNT : RO; bitpos: [3:0]; default: 0;
 *  AXI read id err cnt
 */
#define DMA2D_RID_ERR_CNT    0x0000000FU
#define DMA2D_RID_ERR_CNT_M  (DMA2D_RID_ERR_CNT_V << DMA2D_RID_ERR_CNT_S)
#define DMA2D_RID_ERR_CNT_V  0x0000000FU
#define DMA2D_RID_ERR_CNT_S  0
/** DMA2D_RRESP_ERR_CNT : RO; bitpos: [7:4]; default: 0;
 *  AXI read resp err cnt
 */
#define DMA2D_RRESP_ERR_CNT    0x0000000FU
#define DMA2D_RRESP_ERR_CNT_M  (DMA2D_RRESP_ERR_CNT_V << DMA2D_RRESP_ERR_CNT_S)
#define DMA2D_RRESP_ERR_CNT_V  0x0000000FU
#define DMA2D_RRESP_ERR_CNT_S  4
/** DMA2D_WRESP_ERR_CNT : RO; bitpos: [11:8]; default: 0;
 *  AXI write resp err cnt
 */
#define DMA2D_WRESP_ERR_CNT    0x0000000FU
#define DMA2D_WRESP_ERR_CNT_M  (DMA2D_WRESP_ERR_CNT_V << DMA2D_WRESP_ERR_CNT_S)
#define DMA2D_WRESP_ERR_CNT_V  0x0000000FU
#define DMA2D_WRESP_ERR_CNT_S  8
/** DMA2D_RD_FIFO_CNT : RO; bitpos: [14:12]; default: 0;
 *  AXI read cmd fifo remain cmd count
 */
#define DMA2D_RD_FIFO_CNT    0x00000007U
#define DMA2D_RD_FIFO_CNT_M  (DMA2D_RD_FIFO_CNT_V << DMA2D_RD_FIFO_CNT_S)
#define DMA2D_RD_FIFO_CNT_V  0x00000007U
#define DMA2D_RD_FIFO_CNT_S  12
/** DMA2D_RD_BAK_FIFO_CNT : RO; bitpos: [18:15]; default: 0;
 *  AXI read backup cmd fifo remain cmd count
 */
#define DMA2D_RD_BAK_FIFO_CNT    0x0000000FU
#define DMA2D_RD_BAK_FIFO_CNT_M  (DMA2D_RD_BAK_FIFO_CNT_V << DMA2D_RD_BAK_FIFO_CNT_S)
#define DMA2D_RD_BAK_FIFO_CNT_V  0x0000000FU
#define DMA2D_RD_BAK_FIFO_CNT_S  15
/** DMA2D_WR_FIFO_CNT : RO; bitpos: [21:19]; default: 0;
 *  AXI write cmd fifo remain cmd count
 */
#define DMA2D_WR_FIFO_CNT    0x00000007U
#define DMA2D_WR_FIFO_CNT_M  (DMA2D_WR_FIFO_CNT_V << DMA2D_WR_FIFO_CNT_S)
#define DMA2D_WR_FIFO_CNT_V  0x00000007U
#define DMA2D_WR_FIFO_CNT_S  19
/** DMA2D_WR_BAK_FIFO_CNT : RO; bitpos: [25:22]; default: 0;
 *  AXI write backup cmd fifo remain cmd count
 */
#define DMA2D_WR_BAK_FIFO_CNT    0x0000000FU
#define DMA2D_WR_BAK_FIFO_CNT_M  (DMA2D_WR_BAK_FIFO_CNT_V << DMA2D_WR_BAK_FIFO_CNT_S)
#define DMA2D_WR_BAK_FIFO_CNT_V  0x0000000FU
#define DMA2D_WR_BAK_FIFO_CNT_S  22

/** DMA2D_RST_CONF_REG register
 *  Configures the reset of axi
 */
#define DMA2D_RST_CONF_REG (DR_REG_DMA2D_BASE + 0xa04)
/** DMA2D_AXIM_RD_RST : R/W; bitpos: [0]; default: 0;
 *  Write 1 then write 0  to this bit to reset axi master read data FIFO.
 */
#define DMA2D_AXIM_RD_RST    (BIT(0))
#define DMA2D_AXIM_RD_RST_M  (DMA2D_AXIM_RD_RST_V << DMA2D_AXIM_RD_RST_S)
#define DMA2D_AXIM_RD_RST_V  0x00000001U
#define DMA2D_AXIM_RD_RST_S  0
/** DMA2D_AXIM_WR_RST : R/W; bitpos: [1]; default: 0;
 *  Write 1 then write 0  to this bit to reset axi master write data FIFO.
 */
#define DMA2D_AXIM_WR_RST    (BIT(1))
#define DMA2D_AXIM_WR_RST_M  (DMA2D_AXIM_WR_RST_V << DMA2D_AXIM_WR_RST_S)
#define DMA2D_AXIM_WR_RST_V  0x00000001U
#define DMA2D_AXIM_WR_RST_S  1
/** DMA2D_CLK_EN : R/W; bitpos: [2]; default: 0;
 *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
 *  registers.
 */
#define DMA2D_CLK_EN    (BIT(2))
#define DMA2D_CLK_EN_M  (DMA2D_CLK_EN_V << DMA2D_CLK_EN_S)
#define DMA2D_CLK_EN_V  0x00000001U
#define DMA2D_CLK_EN_S  2

/** DMA2D_INTR_MEM_START_ADDR_REG register
 *  The start address of accessible address space.
 */
#define DMA2D_INTR_MEM_START_ADDR_REG (DR_REG_DMA2D_BASE + 0xa08)
/** DMA2D_ACCESS_INTR_MEM_START_ADDR : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define DMA2D_ACCESS_INTR_MEM_START_ADDR    0xFFFFFFFFU
#define DMA2D_ACCESS_INTR_MEM_START_ADDR_M  (DMA2D_ACCESS_INTR_MEM_START_ADDR_V << DMA2D_ACCESS_INTR_MEM_START_ADDR_S)
#define DMA2D_ACCESS_INTR_MEM_START_ADDR_V  0xFFFFFFFFU
#define DMA2D_ACCESS_INTR_MEM_START_ADDR_S  0

/** DMA2D_INTR_MEM_END_ADDR_REG register
 *  The end address of accessible address space.
 */
#define DMA2D_INTR_MEM_END_ADDR_REG (DR_REG_DMA2D_BASE + 0xa0c)
/** DMA2D_ACCESS_INTR_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define DMA2D_ACCESS_INTR_MEM_END_ADDR    0xFFFFFFFFU
#define DMA2D_ACCESS_INTR_MEM_END_ADDR_M  (DMA2D_ACCESS_INTR_MEM_END_ADDR_V << DMA2D_ACCESS_INTR_MEM_END_ADDR_S)
#define DMA2D_ACCESS_INTR_MEM_END_ADDR_V  0xFFFFFFFFU
#define DMA2D_ACCESS_INTR_MEM_END_ADDR_S  0

/** DMA2D_EXTR_MEM_START_ADDR_REG register
 *  The start address of accessible address space.
 */
#define DMA2D_EXTR_MEM_START_ADDR_REG (DR_REG_DMA2D_BASE + 0xa10)
/** DMA2D_ACCESS_EXTR_MEM_START_ADDR : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define DMA2D_ACCESS_EXTR_MEM_START_ADDR    0xFFFFFFFFU
#define DMA2D_ACCESS_EXTR_MEM_START_ADDR_M  (DMA2D_ACCESS_EXTR_MEM_START_ADDR_V << DMA2D_ACCESS_EXTR_MEM_START_ADDR_S)
#define DMA2D_ACCESS_EXTR_MEM_START_ADDR_V  0xFFFFFFFFU
#define DMA2D_ACCESS_EXTR_MEM_START_ADDR_S  0

/** DMA2D_EXTR_MEM_END_ADDR_REG register
 *  The end address of accessible address space.
 */
#define DMA2D_EXTR_MEM_END_ADDR_REG (DR_REG_DMA2D_BASE + 0xa14)
/** DMA2D_ACCESS_EXTR_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define DMA2D_ACCESS_EXTR_MEM_END_ADDR    0xFFFFFFFFU
#define DMA2D_ACCESS_EXTR_MEM_END_ADDR_M  (DMA2D_ACCESS_EXTR_MEM_END_ADDR_V << DMA2D_ACCESS_EXTR_MEM_END_ADDR_S)
#define DMA2D_ACCESS_EXTR_MEM_END_ADDR_V  0xFFFFFFFFU
#define DMA2D_ACCESS_EXTR_MEM_END_ADDR_S  0

/** DMA2D_OUT_ARB_CONFIG_REG register
 *  Configures the tx arbiter
 */
#define DMA2D_OUT_ARB_CONFIG_REG (DR_REG_DMA2D_BASE + 0xa18)
/** DMA2D_OUT_ARB_TIMEOUT_NUM : R/W; bitpos: [15:0]; default: 0;
 *  Set the max number of timeout count of arbiter
 */
#define DMA2D_OUT_ARB_TIMEOUT_NUM    0x0000FFFFU
#define DMA2D_OUT_ARB_TIMEOUT_NUM_M  (DMA2D_OUT_ARB_TIMEOUT_NUM_V << DMA2D_OUT_ARB_TIMEOUT_NUM_S)
#define DMA2D_OUT_ARB_TIMEOUT_NUM_V  0x0000FFFFU
#define DMA2D_OUT_ARB_TIMEOUT_NUM_S  0
/** DMA2D_OUT_WEIGHT_EN : R/W; bitpos: [16]; default: 0;
 *  reserved
 */
#define DMA2D_OUT_WEIGHT_EN    (BIT(16))
#define DMA2D_OUT_WEIGHT_EN_M  (DMA2D_OUT_WEIGHT_EN_V << DMA2D_OUT_WEIGHT_EN_S)
#define DMA2D_OUT_WEIGHT_EN_V  0x00000001U
#define DMA2D_OUT_WEIGHT_EN_S  16

/** DMA2D_IN_ARB_CONFIG_REG register
 *  Configures the rx arbiter
 */
#define DMA2D_IN_ARB_CONFIG_REG (DR_REG_DMA2D_BASE + 0xa1c)
/** DMA2D_IN_ARB_TIMEOUT_NUM : R/W; bitpos: [15:0]; default: 0;
 *  Set the max number of timeout count of arbiter
 */
#define DMA2D_IN_ARB_TIMEOUT_NUM    0x0000FFFFU
#define DMA2D_IN_ARB_TIMEOUT_NUM_M  (DMA2D_IN_ARB_TIMEOUT_NUM_V << DMA2D_IN_ARB_TIMEOUT_NUM_S)
#define DMA2D_IN_ARB_TIMEOUT_NUM_V  0x0000FFFFU
#define DMA2D_IN_ARB_TIMEOUT_NUM_S  0
/** DMA2D_IN_WEIGHT_EN : R/W; bitpos: [16]; default: 0;
 *  reserved
 */
#define DMA2D_IN_WEIGHT_EN    (BIT(16))
#define DMA2D_IN_WEIGHT_EN_M  (DMA2D_IN_WEIGHT_EN_V << DMA2D_IN_WEIGHT_EN_S)
#define DMA2D_IN_WEIGHT_EN_V  0x00000001U
#define DMA2D_IN_WEIGHT_EN_S  16

/** DMA2D_RDN_RESULT_REG register
 *  reserved
 */
#define DMA2D_RDN_RESULT_REG (DR_REG_DMA2D_BASE + 0xa20)
/** DMA2D_RDN_ENA : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define DMA2D_RDN_ENA    (BIT(0))
#define DMA2D_RDN_ENA_M  (DMA2D_RDN_ENA_V << DMA2D_RDN_ENA_S)
#define DMA2D_RDN_ENA_V  0x00000001U
#define DMA2D_RDN_ENA_S  0
/** DMA2D_RDN_RESULT : RO; bitpos: [1]; default: 0;
 *  reserved
 */
#define DMA2D_RDN_RESULT    (BIT(1))
#define DMA2D_RDN_RESULT_M  (DMA2D_RDN_RESULT_V << DMA2D_RDN_RESULT_S)
#define DMA2D_RDN_RESULT_V  0x00000001U
#define DMA2D_RDN_RESULT_S  1

/** DMA2D_RDN_ECO_HIGH_REG register
 *  reserved
 */
#define DMA2D_RDN_ECO_HIGH_REG (DR_REG_DMA2D_BASE + 0xa24)
/** DMA2D_RDN_ECO_HIGH : R/W; bitpos: [31:0]; default: 4294967295;
 *  The start address of accessible address space.
 */
#define DMA2D_RDN_ECO_HIGH    0xFFFFFFFFU
#define DMA2D_RDN_ECO_HIGH_M  (DMA2D_RDN_ECO_HIGH_V << DMA2D_RDN_ECO_HIGH_S)
#define DMA2D_RDN_ECO_HIGH_V  0xFFFFFFFFU
#define DMA2D_RDN_ECO_HIGH_S  0

/** DMA2D_RDN_ECO_LOW_REG register
 *  reserved
 */
#define DMA2D_RDN_ECO_LOW_REG (DR_REG_DMA2D_BASE + 0xa28)
/** DMA2D_RDN_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  The start address of accessible address space.
 */
#define DMA2D_RDN_ECO_LOW    0xFFFFFFFFU
#define DMA2D_RDN_ECO_LOW_M  (DMA2D_RDN_ECO_LOW_V << DMA2D_RDN_ECO_LOW_S)
#define DMA2D_RDN_ECO_LOW_V  0xFFFFFFFFU
#define DMA2D_RDN_ECO_LOW_S  0

/** DMA2D_DATE_REG register
 *  register version.
 */
#define DMA2D_DATE_REG (DR_REG_DMA2D_BASE + 0xa2c)
/** DMA2D_DATE : R/W; bitpos: [31:0]; default: 36716816;
 *  register version.
 */
#define DMA2D_DATE    0xFFFFFFFFU
#define DMA2D_DATE_M  (DMA2D_DATE_V << DMA2D_DATE_S)
#define DMA2D_DATE_V  0xFFFFFFFFU
#define DMA2D_DATE_S  0

#ifdef __cplusplus
}
#endif
