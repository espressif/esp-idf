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

/** AXI_DMA_IN_INT_RAW_CH0_REG register
 *  Raw status interrupt of channel 0
 */
#define AXI_DMA_IN_INT_RAW_CH0_REG (DR_REG_AXI_DMA_BASE + 0x0)
/** AXI_DMA_IN_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0.
 */
#define AXI_DMA_IN_DONE_CH0_INT_RAW    (BIT(0))
#define AXI_DMA_IN_DONE_CH0_INT_RAW_M  (AXI_DMA_IN_DONE_CH0_INT_RAW_V << AXI_DMA_IN_DONE_CH0_INT_RAW_S)
#define AXI_DMA_IN_DONE_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DONE_CH0_INT_RAW_S  0
/** AXI_DMA_IN_SUC_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 0.
 */
#define AXI_DMA_IN_SUC_EOF_CH0_INT_RAW    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH0_INT_RAW_M  (AXI_DMA_IN_SUC_EOF_CH0_INT_RAW_V << AXI_DMA_IN_SUC_EOF_CH0_INT_RAW_S)
#define AXI_DMA_IN_SUC_EOF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH0_INT_RAW_S  1
/** AXI_DMA_IN_ERR_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 0. For other peripherals this raw
 *  interrupt is reserved.
 */
#define AXI_DMA_IN_ERR_EOF_CH0_INT_RAW    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH0_INT_RAW_M  (AXI_DMA_IN_ERR_EOF_CH0_INT_RAW_V << AXI_DMA_IN_ERR_EOF_CH0_INT_RAW_S)
#define AXI_DMA_IN_ERR_EOF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH0_INT_RAW_S  2
/** AXI_DMA_IN_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 0.
 */
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_RAW    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_RAW_M  (AXI_DMA_IN_DSCR_ERR_CH0_INT_RAW_V << AXI_DMA_IN_DSCR_ERR_CH0_INT_RAW_S)
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_RAW_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 0.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_RAW    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_M  (AXI_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_V << AXI_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_RAW    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_RAW_M  (AXI_DMA_INFIFO_L1_OVF_CH0_INT_RAW_V << AXI_DMA_INFIFO_L1_OVF_CH0_INT_RAW_S)
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_RAW_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_RAW    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_RAW_M  (AXI_DMA_INFIFO_L1_UDF_CH0_INT_RAW_V << AXI_DMA_INFIFO_L1_UDF_CH0_INT_RAW_S)
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_RAW_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_RAW    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_RAW_M  (AXI_DMA_INFIFO_L2_OVF_CH0_INT_RAW_V << AXI_DMA_INFIFO_L2_OVF_CH0_INT_RAW_S)
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_RAW_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_RAW    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_RAW_M  (AXI_DMA_INFIFO_L2_UDF_CH0_INT_RAW_V << AXI_DMA_INFIFO_L2_UDF_CH0_INT_RAW_S)
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_RAW_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_RAW    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_RAW_M  (AXI_DMA_INFIFO_L3_OVF_CH0_INT_RAW_V << AXI_DMA_INFIFO_L3_OVF_CH0_INT_RAW_S)
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_RAW_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_RAW    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_RAW_M  (AXI_DMA_INFIFO_L3_UDF_CH0_INT_RAW_V << AXI_DMA_INFIFO_L3_UDF_CH0_INT_RAW_S)
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_RAW_S  10

/** AXI_DMA_IN_INT_ST_CH0_REG register
 *  Masked interrupt of channel 0
 */
#define AXI_DMA_IN_INT_ST_CH0_REG (DR_REG_AXI_DMA_BASE + 0x4)
/** AXI_DMA_IN_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH0_INT_ST    (BIT(0))
#define AXI_DMA_IN_DONE_CH0_INT_ST_M  (AXI_DMA_IN_DONE_CH0_INT_ST_V << AXI_DMA_IN_DONE_CH0_INT_ST_S)
#define AXI_DMA_IN_DONE_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DONE_CH0_INT_ST_S  0
/** AXI_DMA_IN_SUC_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ST    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ST_M  (AXI_DMA_IN_SUC_EOF_CH0_INT_ST_V << AXI_DMA_IN_SUC_EOF_CH0_INT_ST_S)
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ST_S  1
/** AXI_DMA_IN_ERR_EOF_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ST    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ST_M  (AXI_DMA_IN_ERR_EOF_CH0_INT_ST_V << AXI_DMA_IN_ERR_EOF_CH0_INT_ST_S)
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ST_S  2
/** AXI_DMA_IN_DSCR_ERR_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ST    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ST_M  (AXI_DMA_IN_DSCR_ERR_CH0_INT_ST_V << AXI_DMA_IN_DSCR_ERR_CH0_INT_ST_S)
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ST_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ST    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ST_M  (AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ST_V << AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ST_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ST_S  4
/** AXI_DMA_INFIFO_OVF_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_OVF_CH0_INT_ST    (BIT(5))
#define AXI_DMA_INFIFO_OVF_CH0_INT_ST_M  (AXI_DMA_INFIFO_OVF_CH0_INT_ST_V << AXI_DMA_INFIFO_OVF_CH0_INT_ST_S)
#define AXI_DMA_INFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_OVF_CH0_INT_ST_S  5
/** AXI_DMA_INFIFO_UDF_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_UDF_CH0_INT_ST    (BIT(6))
#define AXI_DMA_INFIFO_UDF_CH0_INT_ST_M  (AXI_DMA_INFIFO_UDF_CH0_INT_ST_V << AXI_DMA_INFIFO_UDF_CH0_INT_ST_S)
#define AXI_DMA_INFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_UDF_CH0_INT_ST_S  6
/** AXI_DMA_INFIFO_L1_OVF_CH0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ST    (BIT(7))
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ST_M  (AXI_DMA_INFIFO_L1_OVF_CH0_INT_ST_V << AXI_DMA_INFIFO_L1_OVF_CH0_INT_ST_S)
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ST_S  7
/** AXI_DMA_INFIFO_L1_UDF_CH0_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ST    (BIT(8))
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ST_M  (AXI_DMA_INFIFO_L1_UDF_CH0_INT_ST_V << AXI_DMA_INFIFO_L1_UDF_CH0_INT_ST_S)
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ST_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH0_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ST    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ST_M  (AXI_DMA_INFIFO_L3_OVF_CH0_INT_ST_V << AXI_DMA_INFIFO_L3_OVF_CH0_INT_ST_S)
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ST_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH0_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ST    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ST_M  (AXI_DMA_INFIFO_L3_UDF_CH0_INT_ST_V << AXI_DMA_INFIFO_L3_UDF_CH0_INT_ST_S)
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ST_S  10

/** AXI_DMA_IN_INT_ENA_CH0_REG register
 *  Interrupt enable bits of channel 0
 */
#define AXI_DMA_IN_INT_ENA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x8)
/** AXI_DMA_IN_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH0_INT_ENA    (BIT(0))
#define AXI_DMA_IN_DONE_CH0_INT_ENA_M  (AXI_DMA_IN_DONE_CH0_INT_ENA_V << AXI_DMA_IN_DONE_CH0_INT_ENA_S)
#define AXI_DMA_IN_DONE_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DONE_CH0_INT_ENA_S  0
/** AXI_DMA_IN_SUC_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ENA    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ENA_M  (AXI_DMA_IN_SUC_EOF_CH0_INT_ENA_V << AXI_DMA_IN_SUC_EOF_CH0_INT_ENA_S)
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH0_INT_ENA_S  1
/** AXI_DMA_IN_ERR_EOF_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ENA    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ENA_M  (AXI_DMA_IN_ERR_EOF_CH0_INT_ENA_V << AXI_DMA_IN_ERR_EOF_CH0_INT_ENA_S)
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH0_INT_ENA_S  2
/** AXI_DMA_IN_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ENA    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ENA_M  (AXI_DMA_IN_DSCR_ERR_CH0_INT_ENA_V << AXI_DMA_IN_DSCR_ERR_CH0_INT_ENA_S)
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_ENA_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ENA    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_M  (AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_V << AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ENA    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ENA_M  (AXI_DMA_INFIFO_L1_OVF_CH0_INT_ENA_V << AXI_DMA_INFIFO_L1_OVF_CH0_INT_ENA_S)
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_ENA_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ENA    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ENA_M  (AXI_DMA_INFIFO_L1_UDF_CH0_INT_ENA_V << AXI_DMA_INFIFO_L1_UDF_CH0_INT_ENA_S)
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_ENA_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_ENA    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_ENA_M  (AXI_DMA_INFIFO_L2_OVF_CH0_INT_ENA_V << AXI_DMA_INFIFO_L2_OVF_CH0_INT_ENA_S)
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_ENA_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH0_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_ENA    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_ENA_M  (AXI_DMA_INFIFO_L2_UDF_CH0_INT_ENA_V << AXI_DMA_INFIFO_L2_UDF_CH0_INT_ENA_S)
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_ENA_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH0_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ENA    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ENA_M  (AXI_DMA_INFIFO_L3_OVF_CH0_INT_ENA_V << AXI_DMA_INFIFO_L3_OVF_CH0_INT_ENA_S)
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_ENA_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH0_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ENA    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ENA_M  (AXI_DMA_INFIFO_L3_UDF_CH0_INT_ENA_V << AXI_DMA_INFIFO_L3_UDF_CH0_INT_ENA_S)
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_ENA_S  10

/** AXI_DMA_IN_INT_CLR_CH0_REG register
 *  Interrupt clear bits of channel 0
 */
#define AXI_DMA_IN_INT_CLR_CH0_REG (DR_REG_AXI_DMA_BASE + 0xc)
/** AXI_DMA_IN_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH0_INT_CLR    (BIT(0))
#define AXI_DMA_IN_DONE_CH0_INT_CLR_M  (AXI_DMA_IN_DONE_CH0_INT_CLR_V << AXI_DMA_IN_DONE_CH0_INT_CLR_S)
#define AXI_DMA_IN_DONE_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DONE_CH0_INT_CLR_S  0
/** AXI_DMA_IN_SUC_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH0_INT_CLR    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH0_INT_CLR_M  (AXI_DMA_IN_SUC_EOF_CH0_INT_CLR_V << AXI_DMA_IN_SUC_EOF_CH0_INT_CLR_S)
#define AXI_DMA_IN_SUC_EOF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH0_INT_CLR_S  1
/** AXI_DMA_IN_ERR_EOF_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH0_INT_CLR    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH0_INT_CLR_M  (AXI_DMA_IN_ERR_EOF_CH0_INT_CLR_V << AXI_DMA_IN_ERR_EOF_CH0_INT_CLR_S)
#define AXI_DMA_IN_ERR_EOF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH0_INT_CLR_S  2
/** AXI_DMA_IN_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_CLR    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_CLR_M  (AXI_DMA_IN_DSCR_ERR_CH0_INT_CLR_V << AXI_DMA_IN_DSCR_ERR_CH0_INT_CLR_S)
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH0_INT_CLR_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_CLR    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_M  (AXI_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_V << AXI_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_CLR    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_CLR_M  (AXI_DMA_INFIFO_L1_OVF_CH0_INT_CLR_V << AXI_DMA_INFIFO_L1_OVF_CH0_INT_CLR_S)
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH0_INT_CLR_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_CLR    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_CLR_M  (AXI_DMA_INFIFO_L1_UDF_CH0_INT_CLR_V << AXI_DMA_INFIFO_L1_UDF_CH0_INT_CLR_S)
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH0_INT_CLR_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_CLR    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_CLR_M  (AXI_DMA_INFIFO_L2_OVF_CH0_INT_CLR_V << AXI_DMA_INFIFO_L2_OVF_CH0_INT_CLR_S)
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH0_INT_CLR_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH0_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_CLR    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_CLR_M  (AXI_DMA_INFIFO_L2_UDF_CH0_INT_CLR_V << AXI_DMA_INFIFO_L2_UDF_CH0_INT_CLR_S)
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH0_INT_CLR_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH0_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_CLR    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_CLR_M  (AXI_DMA_INFIFO_L3_OVF_CH0_INT_CLR_V << AXI_DMA_INFIFO_L3_OVF_CH0_INT_CLR_S)
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH0_INT_CLR_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH0_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_CLR    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_CLR_M  (AXI_DMA_INFIFO_L3_UDF_CH0_INT_CLR_V << AXI_DMA_INFIFO_L3_UDF_CH0_INT_CLR_S)
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH0_INT_CLR_S  10

/** AXI_DMA_IN_CONF0_CH0_REG register
 *  Configure 0 register of Rx channel 0
 */
#define AXI_DMA_IN_CONF0_CH0_REG (DR_REG_AXI_DMA_BASE + 0x10)
/** AXI_DMA_IN_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AXI_DMA channel 0 Rx FSM and Rx FIFO pointer.
 */
#define AXI_DMA_IN_RST_CH0    (BIT(0))
#define AXI_DMA_IN_RST_CH0_M  (AXI_DMA_IN_RST_CH0_V << AXI_DMA_IN_RST_CH0_S)
#define AXI_DMA_IN_RST_CH0_V  0x00000001U
#define AXI_DMA_IN_RST_CH0_S  0
/** AXI_DMA_IN_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_LOOP_TEST_CH0    (BIT(1))
#define AXI_DMA_IN_LOOP_TEST_CH0_M  (AXI_DMA_IN_LOOP_TEST_CH0_V << AXI_DMA_IN_LOOP_TEST_CH0_S)
#define AXI_DMA_IN_LOOP_TEST_CH0_V  0x00000001U
#define AXI_DMA_IN_LOOP_TEST_CH0_S  1
/** AXI_DMA_MEM_TRANS_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via
 *  AXI_DMA.
 */
#define AXI_DMA_MEM_TRANS_EN_CH0    (BIT(2))
#define AXI_DMA_MEM_TRANS_EN_CH0_M  (AXI_DMA_MEM_TRANS_EN_CH0_V << AXI_DMA_MEM_TRANS_EN_CH0_S)
#define AXI_DMA_MEM_TRANS_EN_CH0_V  0x00000001U
#define AXI_DMA_MEM_TRANS_EN_CH0_S  2
/** AXI_DMA_IN_ETM_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 0 is triggered by etm
 *  task.
 */
#define AXI_DMA_IN_ETM_EN_CH0    (BIT(3))
#define AXI_DMA_IN_ETM_EN_CH0_M  (AXI_DMA_IN_ETM_EN_CH0_V << AXI_DMA_IN_ETM_EN_CH0_S)
#define AXI_DMA_IN_ETM_EN_CH0_V  0x00000001U
#define AXI_DMA_IN_ETM_EN_CH0_S  3
/** AXI_DMA_IN_BURST_SIZE_SEL_CH0 : R/W; bitpos: [6:4]; default: 0;
 *  3'b000-3'b100:burst length 8byte~128byte
 */
#define AXI_DMA_IN_BURST_SIZE_SEL_CH0    0x00000007U
#define AXI_DMA_IN_BURST_SIZE_SEL_CH0_M  (AXI_DMA_IN_BURST_SIZE_SEL_CH0_V << AXI_DMA_IN_BURST_SIZE_SEL_CH0_S)
#define AXI_DMA_IN_BURST_SIZE_SEL_CH0_V  0x00000007U
#define AXI_DMA_IN_BURST_SIZE_SEL_CH0_S  4
/** AXI_DMA_IN_CMD_DISABLE_CH0 : R/W; bitpos: [7]; default: 0;
 *  1:mean disable cmd of this ch0
 */
#define AXI_DMA_IN_CMD_DISABLE_CH0    (BIT(7))
#define AXI_DMA_IN_CMD_DISABLE_CH0_M  (AXI_DMA_IN_CMD_DISABLE_CH0_V << AXI_DMA_IN_CMD_DISABLE_CH0_S)
#define AXI_DMA_IN_CMD_DISABLE_CH0_V  0x00000001U
#define AXI_DMA_IN_CMD_DISABLE_CH0_S  7
/** AXI_DMA_IN_ECC_AEC_EN_CH0 : R/W; bitpos: [8]; default: 0;
 *  1: mean access ecc or aes domain,0: mean not
 */
#define AXI_DMA_IN_ECC_AEC_EN_CH0    (BIT(8))
#define AXI_DMA_IN_ECC_AEC_EN_CH0_M  (AXI_DMA_IN_ECC_AEC_EN_CH0_V << AXI_DMA_IN_ECC_AEC_EN_CH0_S)
#define AXI_DMA_IN_ECC_AEC_EN_CH0_V  0x00000001U
#define AXI_DMA_IN_ECC_AEC_EN_CH0_S  8
/** AXI_DMA_INDSCR_BURST_EN_CH0 : R/W; bitpos: [9]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AXI_DMA_INDSCR_BURST_EN_CH0    (BIT(9))
#define AXI_DMA_INDSCR_BURST_EN_CH0_M  (AXI_DMA_INDSCR_BURST_EN_CH0_V << AXI_DMA_INDSCR_BURST_EN_CH0_S)
#define AXI_DMA_INDSCR_BURST_EN_CH0_V  0x00000001U
#define AXI_DMA_INDSCR_BURST_EN_CH0_S  9

/** AXI_DMA_IN_CONF1_CH0_REG register
 *  Configure 1 register of Rx channel 0
 */
#define AXI_DMA_IN_CONF1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x14)
/** AXI_DMA_IN_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AXI_DMA_IN_CHECK_OWNER_CH0    (BIT(12))
#define AXI_DMA_IN_CHECK_OWNER_CH0_M  (AXI_DMA_IN_CHECK_OWNER_CH0_V << AXI_DMA_IN_CHECK_OWNER_CH0_S)
#define AXI_DMA_IN_CHECK_OWNER_CH0_V  0x00000001U
#define AXI_DMA_IN_CHECK_OWNER_CH0_S  12

/** AXI_DMA_INFIFO_STATUS_CH0_REG register
 *  Receive FIFO status of Rx channel 0
 */
#define AXI_DMA_INFIFO_STATUS_CH0_REG (DR_REG_AXI_DMA_BASE + 0x18)
/** AXI_DMA_INFIFO_L3_FULL_CH0 : RO; bitpos: [0]; default: 1;
 *  L3 Rx FIFO full signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L3_FULL_CH0    (BIT(0))
#define AXI_DMA_INFIFO_L3_FULL_CH0_M  (AXI_DMA_INFIFO_L3_FULL_CH0_V << AXI_DMA_INFIFO_L3_FULL_CH0_S)
#define AXI_DMA_INFIFO_L3_FULL_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L3_FULL_CH0_S  0
/** AXI_DMA_INFIFO_L3_EMPTY_CH0 : RO; bitpos: [1]; default: 1;
 *  L3 Rx FIFO empty signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L3_EMPTY_CH0    (BIT(1))
#define AXI_DMA_INFIFO_L3_EMPTY_CH0_M  (AXI_DMA_INFIFO_L3_EMPTY_CH0_V << AXI_DMA_INFIFO_L3_EMPTY_CH0_S)
#define AXI_DMA_INFIFO_L3_EMPTY_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L3_EMPTY_CH0_S  1
/** AXI_DMA_INFIFO_L3_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L3 Rx FIFO for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L3_CNT_CH0    0x0000003FU
#define AXI_DMA_INFIFO_L3_CNT_CH0_M  (AXI_DMA_INFIFO_L3_CNT_CH0_V << AXI_DMA_INFIFO_L3_CNT_CH0_S)
#define AXI_DMA_INFIFO_L3_CNT_CH0_V  0x0000003FU
#define AXI_DMA_INFIFO_L3_CNT_CH0_S  2
/** AXI_DMA_INFIFO_L3_UDF_CH0 : RO; bitpos: [8]; default: 0;
 *  L3 Rx FIFO under flow signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH0    (BIT(8))
#define AXI_DMA_INFIFO_L3_UDF_CH0_M  (AXI_DMA_INFIFO_L3_UDF_CH0_V << AXI_DMA_INFIFO_L3_UDF_CH0_S)
#define AXI_DMA_INFIFO_L3_UDF_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH0_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH0 : RO; bitpos: [9]; default: 0;
 *  L3 Rx FIFO over flow signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH0    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH0_M  (AXI_DMA_INFIFO_L3_OVF_CH0_V << AXI_DMA_INFIFO_L3_OVF_CH0_S)
#define AXI_DMA_INFIFO_L3_OVF_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH0_S  9
/** AXI_DMA_INFIFO_L1_FULL_CH0 : RO; bitpos: [10]; default: 0;
 *  L1 Rx FIFO full signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L1_FULL_CH0    (BIT(10))
#define AXI_DMA_INFIFO_L1_FULL_CH0_M  (AXI_DMA_INFIFO_L1_FULL_CH0_V << AXI_DMA_INFIFO_L1_FULL_CH0_S)
#define AXI_DMA_INFIFO_L1_FULL_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L1_FULL_CH0_S  10
/** AXI_DMA_INFIFO_L1_EMPTY_CH0 : RO; bitpos: [11]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L1_EMPTY_CH0    (BIT(11))
#define AXI_DMA_INFIFO_L1_EMPTY_CH0_M  (AXI_DMA_INFIFO_L1_EMPTY_CH0_V << AXI_DMA_INFIFO_L1_EMPTY_CH0_S)
#define AXI_DMA_INFIFO_L1_EMPTY_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L1_EMPTY_CH0_S  11
/** AXI_DMA_INFIFO_L1_UDF_CH0 : RO; bitpos: [12]; default: 0;
 *  L1 Rx FIFO under flow signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH0    (BIT(12))
#define AXI_DMA_INFIFO_L1_UDF_CH0_M  (AXI_DMA_INFIFO_L1_UDF_CH0_V << AXI_DMA_INFIFO_L1_UDF_CH0_S)
#define AXI_DMA_INFIFO_L1_UDF_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH0_S  12
/** AXI_DMA_INFIFO_L1_OVF_CH0 : RO; bitpos: [13]; default: 0;
 *  L1 Rx FIFO over flow signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH0    (BIT(13))
#define AXI_DMA_INFIFO_L1_OVF_CH0_M  (AXI_DMA_INFIFO_L1_OVF_CH0_V << AXI_DMA_INFIFO_L1_OVF_CH0_S)
#define AXI_DMA_INFIFO_L1_OVF_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH0_S  13
/** AXI_DMA_INFIFO_L2_FULL_CH0 : RO; bitpos: [14]; default: 0;
 *  L2 Rx RAM full signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L2_FULL_CH0    (BIT(14))
#define AXI_DMA_INFIFO_L2_FULL_CH0_M  (AXI_DMA_INFIFO_L2_FULL_CH0_V << AXI_DMA_INFIFO_L2_FULL_CH0_S)
#define AXI_DMA_INFIFO_L2_FULL_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L2_FULL_CH0_S  14
/** AXI_DMA_INFIFO_L2_EMPTY_CH0 : RO; bitpos: [15]; default: 1;
 *  L2 Rx RAM empty signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L2_EMPTY_CH0    (BIT(15))
#define AXI_DMA_INFIFO_L2_EMPTY_CH0_M  (AXI_DMA_INFIFO_L2_EMPTY_CH0_V << AXI_DMA_INFIFO_L2_EMPTY_CH0_S)
#define AXI_DMA_INFIFO_L2_EMPTY_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L2_EMPTY_CH0_S  15
/** AXI_DMA_INFIFO_L2_UDF_CH0 : RO; bitpos: [16]; default: 0;
 *  L2 Rx FIFO under flow signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH0    (BIT(16))
#define AXI_DMA_INFIFO_L2_UDF_CH0_M  (AXI_DMA_INFIFO_L2_UDF_CH0_V << AXI_DMA_INFIFO_L2_UDF_CH0_S)
#define AXI_DMA_INFIFO_L2_UDF_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH0_S  16
/** AXI_DMA_INFIFO_L2_OVF_CH0 : RO; bitpos: [17]; default: 0;
 *  L2 Rx FIFO over flow signal for Rx channel 0.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH0    (BIT(17))
#define AXI_DMA_INFIFO_L2_OVF_CH0_M  (AXI_DMA_INFIFO_L2_OVF_CH0_V << AXI_DMA_INFIFO_L2_OVF_CH0_S)
#define AXI_DMA_INFIFO_L2_OVF_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH0_S  17
/** AXI_DMA_IN_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH0    (BIT(23))
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_1B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_1B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH0_S  23
/** AXI_DMA_IN_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH0    (BIT(24))
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_2B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_2B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH0_S  24
/** AXI_DMA_IN_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH0    (BIT(25))
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_3B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_3B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH0_S  25
/** AXI_DMA_IN_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH0    (BIT(26))
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_4B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_4B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH0_S  26
/** AXI_DMA_IN_REMAIN_UNDER_5B_CH0 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH0    (BIT(27))
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_5B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_5B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH0_S  27
/** AXI_DMA_IN_REMAIN_UNDER_6B_CH0 : RO; bitpos: [28]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH0    (BIT(28))
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_6B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_6B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH0_S  28
/** AXI_DMA_IN_REMAIN_UNDER_7B_CH0 : RO; bitpos: [29]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH0    (BIT(29))
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_7B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_7B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH0_S  29
/** AXI_DMA_IN_REMAIN_UNDER_8B_CH0 : RO; bitpos: [30]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH0    (BIT(30))
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH0_M  (AXI_DMA_IN_REMAIN_UNDER_8B_CH0_V << AXI_DMA_IN_REMAIN_UNDER_8B_CH0_S)
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH0_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH0_S  30
/** AXI_DMA_IN_BUF_HUNGRY_CH0 : RO; bitpos: [31]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_BUF_HUNGRY_CH0    (BIT(31))
#define AXI_DMA_IN_BUF_HUNGRY_CH0_M  (AXI_DMA_IN_BUF_HUNGRY_CH0_V << AXI_DMA_IN_BUF_HUNGRY_CH0_S)
#define AXI_DMA_IN_BUF_HUNGRY_CH0_V  0x00000001U
#define AXI_DMA_IN_BUF_HUNGRY_CH0_S  31

/** AXI_DMA_IN_POP_CH0_REG register
 *  Pop control register of Rx channel 0
 */
#define AXI_DMA_IN_POP_CH0_REG (DR_REG_AXI_DMA_BASE + 0x1c)
/** AXI_DMA_INFIFO_RDATA_CH0 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from AXI_DMA FIFO.
 */
#define AXI_DMA_INFIFO_RDATA_CH0    0x00000FFFU
#define AXI_DMA_INFIFO_RDATA_CH0_M  (AXI_DMA_INFIFO_RDATA_CH0_V << AXI_DMA_INFIFO_RDATA_CH0_S)
#define AXI_DMA_INFIFO_RDATA_CH0_V  0x00000FFFU
#define AXI_DMA_INFIFO_RDATA_CH0_S  0
/** AXI_DMA_INFIFO_POP_CH0 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from AXI_DMA FIFO.
 */
#define AXI_DMA_INFIFO_POP_CH0    (BIT(12))
#define AXI_DMA_INFIFO_POP_CH0_M  (AXI_DMA_INFIFO_POP_CH0_V << AXI_DMA_INFIFO_POP_CH0_S)
#define AXI_DMA_INFIFO_POP_CH0_V  0x00000001U
#define AXI_DMA_INFIFO_POP_CH0_S  12

/** AXI_DMA_IN_LINK1_CH0_REG register
 *  Link descriptor configure and control register of Rx channel 0
 */
#define AXI_DMA_IN_LINK1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x20)
/** AXI_DMA_INLINK_AUTO_RET_CH0 : R/W; bitpos: [0]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define AXI_DMA_INLINK_AUTO_RET_CH0    (BIT(0))
#define AXI_DMA_INLINK_AUTO_RET_CH0_M  (AXI_DMA_INLINK_AUTO_RET_CH0_V << AXI_DMA_INLINK_AUTO_RET_CH0_S)
#define AXI_DMA_INLINK_AUTO_RET_CH0_V  0x00000001U
#define AXI_DMA_INLINK_AUTO_RET_CH0_S  0
/** AXI_DMA_INLINK_STOP_CH0 : WT; bitpos: [1]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define AXI_DMA_INLINK_STOP_CH0    (BIT(1))
#define AXI_DMA_INLINK_STOP_CH0_M  (AXI_DMA_INLINK_STOP_CH0_V << AXI_DMA_INLINK_STOP_CH0_S)
#define AXI_DMA_INLINK_STOP_CH0_V  0x00000001U
#define AXI_DMA_INLINK_STOP_CH0_S  1
/** AXI_DMA_INLINK_START_CH0 : WT; bitpos: [2]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define AXI_DMA_INLINK_START_CH0    (BIT(2))
#define AXI_DMA_INLINK_START_CH0_M  (AXI_DMA_INLINK_START_CH0_V << AXI_DMA_INLINK_START_CH0_S)
#define AXI_DMA_INLINK_START_CH0_V  0x00000001U
#define AXI_DMA_INLINK_START_CH0_S  2
/** AXI_DMA_INLINK_RESTART_CH0 : WT; bitpos: [3]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define AXI_DMA_INLINK_RESTART_CH0    (BIT(3))
#define AXI_DMA_INLINK_RESTART_CH0_M  (AXI_DMA_INLINK_RESTART_CH0_V << AXI_DMA_INLINK_RESTART_CH0_S)
#define AXI_DMA_INLINK_RESTART_CH0_V  0x00000001U
#define AXI_DMA_INLINK_RESTART_CH0_S  3
/** AXI_DMA_INLINK_PARK_CH0 : RO; bitpos: [4]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define AXI_DMA_INLINK_PARK_CH0    (BIT(4))
#define AXI_DMA_INLINK_PARK_CH0_M  (AXI_DMA_INLINK_PARK_CH0_V << AXI_DMA_INLINK_PARK_CH0_S)
#define AXI_DMA_INLINK_PARK_CH0_V  0x00000001U
#define AXI_DMA_INLINK_PARK_CH0_S  4

/** AXI_DMA_IN_LINK2_CH0_REG register
 *  Link descriptor configure and control register of Rx channel 0
 */
#define AXI_DMA_IN_LINK2_CH0_REG (DR_REG_AXI_DMA_BASE + 0x24)
/** AXI_DMA_INLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 20 least significant bits of the first inlink descriptor's
 *  address.
 */
#define AXI_DMA_INLINK_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_INLINK_ADDR_CH0_M  (AXI_DMA_INLINK_ADDR_CH0_V << AXI_DMA_INLINK_ADDR_CH0_S)
#define AXI_DMA_INLINK_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_ADDR_CH0_S  0

/** AXI_DMA_IN_STATE_CH0_REG register
 *  Receive status of Rx channel 0
 */
#define AXI_DMA_IN_STATE_CH0_REG (DR_REG_AXI_DMA_BASE + 0x28)
/** AXI_DMA_INLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define AXI_DMA_INLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define AXI_DMA_INLINK_DSCR_ADDR_CH0_M  (AXI_DMA_INLINK_DSCR_ADDR_CH0_V << AXI_DMA_INLINK_DSCR_ADDR_CH0_S)
#define AXI_DMA_INLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define AXI_DMA_INLINK_DSCR_ADDR_CH0_S  0
/** AXI_DMA_IN_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_DSCR_STATE_CH0    0x00000003U
#define AXI_DMA_IN_DSCR_STATE_CH0_M  (AXI_DMA_IN_DSCR_STATE_CH0_V << AXI_DMA_IN_DSCR_STATE_CH0_S)
#define AXI_DMA_IN_DSCR_STATE_CH0_V  0x00000003U
#define AXI_DMA_IN_DSCR_STATE_CH0_S  18
/** AXI_DMA_IN_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_STATE_CH0    0x00000007U
#define AXI_DMA_IN_STATE_CH0_M  (AXI_DMA_IN_STATE_CH0_V << AXI_DMA_IN_STATE_CH0_S)
#define AXI_DMA_IN_STATE_CH0_V  0x00000007U
#define AXI_DMA_IN_STATE_CH0_S  20

/** AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 0
 */
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x2c)
/** AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0_M  (AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0_V << AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0_S)
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH0_S  0

/** AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when errors occur of Rx channel 0
 */
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x30)
/** AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0_M  (AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0_V << AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0_S)
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH0_S  0

/** AXI_DMA_IN_DSCR_CH0_REG register
 *  Current inlink descriptor address of Rx channel 0
 */
#define AXI_DMA_IN_DSCR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x34)
/** AXI_DMA_INLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define AXI_DMA_INLINK_DSCR_CH0    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_CH0_M  (AXI_DMA_INLINK_DSCR_CH0_V << AXI_DMA_INLINK_DSCR_CH0_S)
#define AXI_DMA_INLINK_DSCR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_CH0_S  0

/** AXI_DMA_IN_DSCR_BF0_CH0_REG register
 *  The last inlink descriptor address of Rx channel 0
 */
#define AXI_DMA_IN_DSCR_BF0_CH0_REG (DR_REG_AXI_DMA_BASE + 0x38)
/** AXI_DMA_INLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define AXI_DMA_INLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF0_CH0_M  (AXI_DMA_INLINK_DSCR_BF0_CH0_V << AXI_DMA_INLINK_DSCR_BF0_CH0_S)
#define AXI_DMA_INLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF0_CH0_S  0

/** AXI_DMA_IN_DSCR_BF1_CH0_REG register
 *  The second-to-last inlink descriptor address of Rx channel 0
 */
#define AXI_DMA_IN_DSCR_BF1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x3c)
/** AXI_DMA_INLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AXI_DMA_INLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF1_CH0_M  (AXI_DMA_INLINK_DSCR_BF1_CH0_V << AXI_DMA_INLINK_DSCR_BF1_CH0_S)
#define AXI_DMA_INLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF1_CH0_S  0

/** AXI_DMA_IN_PRI_CH0_REG register
 *  Priority register of Rx channel 0
 */
#define AXI_DMA_IN_PRI_CH0_REG (DR_REG_AXI_DMA_BASE + 0x40)
/** AXI_DMA_RX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 0. The larger of the value the higher of the priority.
 */
#define AXI_DMA_RX_PRI_CH0    0x0000000FU
#define AXI_DMA_RX_PRI_CH0_M  (AXI_DMA_RX_PRI_CH0_V << AXI_DMA_RX_PRI_CH0_S)
#define AXI_DMA_RX_PRI_CH0_V  0x0000000FU
#define AXI_DMA_RX_PRI_CH0_S  0
/** AXI_DMA_RX_CH_ARB_WEIGH_CH0 : R/W; bitpos: [7:4]; default: 0;
 *  The weight of Rx channel 0
 */
#define AXI_DMA_RX_CH_ARB_WEIGH_CH0    0x0000000FU
#define AXI_DMA_RX_CH_ARB_WEIGH_CH0_M  (AXI_DMA_RX_CH_ARB_WEIGH_CH0_V << AXI_DMA_RX_CH_ARB_WEIGH_CH0_S)
#define AXI_DMA_RX_CH_ARB_WEIGH_CH0_V  0x0000000FU
#define AXI_DMA_RX_CH_ARB_WEIGH_CH0_S  4
/** AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH0 : R/W; bitpos: [8]; default: 0;
 *  0: mean not optimization weight function ,1: mean optimization
 */
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH0    (BIT(8))
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_M  (AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_V << AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_S)
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_V  0x00000001U
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_S  8

/** AXI_DMA_IN_PERI_SEL_CH0_REG register
 *  Peripheral selection of Rx channel 0
 */
#define AXI_DMA_IN_PERI_SEL_CH0_REG (DR_REG_AXI_DMA_BASE + 0x44)
/** AXI_DMA_PERI_IN_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 0. 0:lcdcam. 1: gpspi_2.
 *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
 */
#define AXI_DMA_PERI_IN_SEL_CH0    0x0000003FU
#define AXI_DMA_PERI_IN_SEL_CH0_M  (AXI_DMA_PERI_IN_SEL_CH0_V << AXI_DMA_PERI_IN_SEL_CH0_S)
#define AXI_DMA_PERI_IN_SEL_CH0_V  0x0000003FU
#define AXI_DMA_PERI_IN_SEL_CH0_S  0

/** AXI_DMA_IN_CRC_INIT_DATA_CH0_REG register
 *  This register is used to config ch0 crc initial data(max 32 bit)
 */
#define AXI_DMA_IN_CRC_INIT_DATA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x48)
/** AXI_DMA_IN_CRC_INIT_DATA_CH0 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch0 of rx crc initial value
 */
#define AXI_DMA_IN_CRC_INIT_DATA_CH0    0xFFFFFFFFU
#define AXI_DMA_IN_CRC_INIT_DATA_CH0_M  (AXI_DMA_IN_CRC_INIT_DATA_CH0_V << AXI_DMA_IN_CRC_INIT_DATA_CH0_S)
#define AXI_DMA_IN_CRC_INIT_DATA_CH0_V  0xFFFFFFFFU
#define AXI_DMA_IN_CRC_INIT_DATA_CH0_S  0

/** AXI_DMA_RX_CRC_WIDTH_CH0_REG register
 *  This register is used to config rx ch0 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AXI_DMA_RX_CRC_WIDTH_CH0_REG (DR_REG_AXI_DMA_BASE + 0x4c)
/** AXI_DMA_RX_CRC_WIDTH_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_WIDTH_CH0    0x00000003U
#define AXI_DMA_RX_CRC_WIDTH_CH0_M  (AXI_DMA_RX_CRC_WIDTH_CH0_V << AXI_DMA_RX_CRC_WIDTH_CH0_S)
#define AXI_DMA_RX_CRC_WIDTH_CH0_V  0x00000003U
#define AXI_DMA_RX_CRC_WIDTH_CH0_S  0
/** AXI_DMA_RX_CRC_LATCH_FLAG_CH0 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH0    (BIT(2))
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH0_M  (AXI_DMA_RX_CRC_LATCH_FLAG_CH0_V << AXI_DMA_RX_CRC_LATCH_FLAG_CH0_S)
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH0_V  0x00000001U
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH0_S  2

/** AXI_DMA_IN_CRC_CLEAR_CH0_REG register
 *  This register is used to clear ch0 crc result
 */
#define AXI_DMA_IN_CRC_CLEAR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x50)
/** AXI_DMA_IN_CRC_CLEAR_CH0 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch0 of rx crc result
 */
#define AXI_DMA_IN_CRC_CLEAR_CH0    (BIT(0))
#define AXI_DMA_IN_CRC_CLEAR_CH0_M  (AXI_DMA_IN_CRC_CLEAR_CH0_V << AXI_DMA_IN_CRC_CLEAR_CH0_S)
#define AXI_DMA_IN_CRC_CLEAR_CH0_V  0x00000001U
#define AXI_DMA_IN_CRC_CLEAR_CH0_S  0

/** AXI_DMA_IN_CRC_FINAL_RESULT_CH0_REG register
 *  This register is used to store ch0 crc result
 */
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH0_REG (DR_REG_AXI_DMA_BASE + 0x54)
/** AXI_DMA_IN_CRC_FINAL_RESULT_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch0 of rx
 */
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH0    0xFFFFFFFFU
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH0_M  (AXI_DMA_IN_CRC_FINAL_RESULT_CH0_V << AXI_DMA_IN_CRC_FINAL_RESULT_CH0_S)
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH0_V  0xFFFFFFFFU
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH0_S  0

/** AXI_DMA_RX_CRC_EN_WR_DATA_CH0_REG register
 *  This resister is used to config ch0 crc en for every bit
 */
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x58)
/** AXI_DMA_RX_CRC_EN_WR_DATA_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable rx ch0 crc 32bit on/off
 */
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH0    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH0_M  (AXI_DMA_RX_CRC_EN_WR_DATA_CH0_V << AXI_DMA_RX_CRC_EN_WR_DATA_CH0_S)
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH0_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH0_S  0

/** AXI_DMA_RX_CRC_EN_ADDR_CH0_REG register
 *  This register is used to config ch0 crc en addr
 */
#define AXI_DMA_RX_CRC_EN_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x5c)
/** AXI_DMA_RX_CRC_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_EN_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_ADDR_CH0_M  (AXI_DMA_RX_CRC_EN_ADDR_CH0_V << AXI_DMA_RX_CRC_EN_ADDR_CH0_S)
#define AXI_DMA_RX_CRC_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_ADDR_CH0_S  0

/** AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_REG register
 *  This register is used to config crc data_8bit en
 */
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x60)
/** AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0 : R/W; bitpos: [15:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0    0x0000FFFFU
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_M  (AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_V << AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_S)
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_V  0x0000FFFFU
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_S  0

/** AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x64)
/** AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0_M  (AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0_V << AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0_S)
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH0_S  0

/** AXI_DMA_IN_INT_RAW_CH1_REG register
 *  Raw status interrupt of channel 1
 */
#define AXI_DMA_IN_INT_RAW_CH1_REG (DR_REG_AXI_DMA_BASE + 0x68)
/** AXI_DMA_IN_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 1.
 */
#define AXI_DMA_IN_DONE_CH1_INT_RAW    (BIT(0))
#define AXI_DMA_IN_DONE_CH1_INT_RAW_M  (AXI_DMA_IN_DONE_CH1_INT_RAW_V << AXI_DMA_IN_DONE_CH1_INT_RAW_S)
#define AXI_DMA_IN_DONE_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DONE_CH1_INT_RAW_S  0
/** AXI_DMA_IN_SUC_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 1. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 1.
 */
#define AXI_DMA_IN_SUC_EOF_CH1_INT_RAW    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH1_INT_RAW_M  (AXI_DMA_IN_SUC_EOF_CH1_INT_RAW_V << AXI_DMA_IN_SUC_EOF_CH1_INT_RAW_S)
#define AXI_DMA_IN_SUC_EOF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH1_INT_RAW_S  1
/** AXI_DMA_IN_ERR_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 1. For other peripherals this raw
 *  interrupt is reserved.
 */
#define AXI_DMA_IN_ERR_EOF_CH1_INT_RAW    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH1_INT_RAW_M  (AXI_DMA_IN_ERR_EOF_CH1_INT_RAW_V << AXI_DMA_IN_ERR_EOF_CH1_INT_RAW_S)
#define AXI_DMA_IN_ERR_EOF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH1_INT_RAW_S  2
/** AXI_DMA_IN_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 1.
 */
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_RAW    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_RAW_M  (AXI_DMA_IN_DSCR_ERR_CH1_INT_RAW_V << AXI_DMA_IN_DSCR_ERR_CH1_INT_RAW_S)
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_RAW_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 1.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_RAW    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_M  (AXI_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_V << AXI_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_RAW    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_RAW_M  (AXI_DMA_INFIFO_L1_OVF_CH1_INT_RAW_V << AXI_DMA_INFIFO_L1_OVF_CH1_INT_RAW_S)
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_RAW_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_RAW    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_RAW_M  (AXI_DMA_INFIFO_L1_UDF_CH1_INT_RAW_V << AXI_DMA_INFIFO_L1_UDF_CH1_INT_RAW_S)
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_RAW_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_RAW    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_RAW_M  (AXI_DMA_INFIFO_L2_OVF_CH1_INT_RAW_V << AXI_DMA_INFIFO_L2_OVF_CH1_INT_RAW_S)
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_RAW_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_RAW    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_RAW_M  (AXI_DMA_INFIFO_L2_UDF_CH1_INT_RAW_V << AXI_DMA_INFIFO_L2_UDF_CH1_INT_RAW_S)
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_RAW_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_RAW    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_RAW_M  (AXI_DMA_INFIFO_L3_OVF_CH1_INT_RAW_V << AXI_DMA_INFIFO_L3_OVF_CH1_INT_RAW_S)
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_RAW_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_RAW    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_RAW_M  (AXI_DMA_INFIFO_L3_UDF_CH1_INT_RAW_V << AXI_DMA_INFIFO_L3_UDF_CH1_INT_RAW_S)
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_RAW_S  10

/** AXI_DMA_IN_INT_ST_CH1_REG register
 *  Masked interrupt of channel 1
 */
#define AXI_DMA_IN_INT_ST_CH1_REG (DR_REG_AXI_DMA_BASE + 0x6c)
/** AXI_DMA_IN_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH1_INT_ST    (BIT(0))
#define AXI_DMA_IN_DONE_CH1_INT_ST_M  (AXI_DMA_IN_DONE_CH1_INT_ST_V << AXI_DMA_IN_DONE_CH1_INT_ST_S)
#define AXI_DMA_IN_DONE_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DONE_CH1_INT_ST_S  0
/** AXI_DMA_IN_SUC_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ST    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ST_M  (AXI_DMA_IN_SUC_EOF_CH1_INT_ST_V << AXI_DMA_IN_SUC_EOF_CH1_INT_ST_S)
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ST_S  1
/** AXI_DMA_IN_ERR_EOF_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ST    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ST_M  (AXI_DMA_IN_ERR_EOF_CH1_INT_ST_V << AXI_DMA_IN_ERR_EOF_CH1_INT_ST_S)
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ST_S  2
/** AXI_DMA_IN_DSCR_ERR_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ST    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ST_M  (AXI_DMA_IN_DSCR_ERR_CH1_INT_ST_V << AXI_DMA_IN_DSCR_ERR_CH1_INT_ST_S)
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ST_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ST    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ST_M  (AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ST_V << AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ST_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ST_S  4
/** AXI_DMA_INFIFO_OVF_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_OVF_CH1_INT_ST    (BIT(5))
#define AXI_DMA_INFIFO_OVF_CH1_INT_ST_M  (AXI_DMA_INFIFO_OVF_CH1_INT_ST_V << AXI_DMA_INFIFO_OVF_CH1_INT_ST_S)
#define AXI_DMA_INFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_OVF_CH1_INT_ST_S  5
/** AXI_DMA_INFIFO_UDF_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_UDF_CH1_INT_ST    (BIT(6))
#define AXI_DMA_INFIFO_UDF_CH1_INT_ST_M  (AXI_DMA_INFIFO_UDF_CH1_INT_ST_V << AXI_DMA_INFIFO_UDF_CH1_INT_ST_S)
#define AXI_DMA_INFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_UDF_CH1_INT_ST_S  6
/** AXI_DMA_INFIFO_L1_OVF_CH1_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ST    (BIT(7))
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ST_M  (AXI_DMA_INFIFO_L1_OVF_CH1_INT_ST_V << AXI_DMA_INFIFO_L1_OVF_CH1_INT_ST_S)
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ST_S  7
/** AXI_DMA_INFIFO_L1_UDF_CH1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ST    (BIT(8))
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ST_M  (AXI_DMA_INFIFO_L1_UDF_CH1_INT_ST_V << AXI_DMA_INFIFO_L1_UDF_CH1_INT_ST_S)
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ST_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH1_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ST    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ST_M  (AXI_DMA_INFIFO_L3_OVF_CH1_INT_ST_V << AXI_DMA_INFIFO_L3_OVF_CH1_INT_ST_S)
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ST_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH1_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ST    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ST_M  (AXI_DMA_INFIFO_L3_UDF_CH1_INT_ST_V << AXI_DMA_INFIFO_L3_UDF_CH1_INT_ST_S)
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ST_S  10

/** AXI_DMA_IN_INT_ENA_CH1_REG register
 *  Interrupt enable bits of channel 1
 */
#define AXI_DMA_IN_INT_ENA_CH1_REG (DR_REG_AXI_DMA_BASE + 0x70)
/** AXI_DMA_IN_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH1_INT_ENA    (BIT(0))
#define AXI_DMA_IN_DONE_CH1_INT_ENA_M  (AXI_DMA_IN_DONE_CH1_INT_ENA_V << AXI_DMA_IN_DONE_CH1_INT_ENA_S)
#define AXI_DMA_IN_DONE_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DONE_CH1_INT_ENA_S  0
/** AXI_DMA_IN_SUC_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ENA    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ENA_M  (AXI_DMA_IN_SUC_EOF_CH1_INT_ENA_V << AXI_DMA_IN_SUC_EOF_CH1_INT_ENA_S)
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH1_INT_ENA_S  1
/** AXI_DMA_IN_ERR_EOF_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ENA    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ENA_M  (AXI_DMA_IN_ERR_EOF_CH1_INT_ENA_V << AXI_DMA_IN_ERR_EOF_CH1_INT_ENA_S)
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH1_INT_ENA_S  2
/** AXI_DMA_IN_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ENA    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ENA_M  (AXI_DMA_IN_DSCR_ERR_CH1_INT_ENA_V << AXI_DMA_IN_DSCR_ERR_CH1_INT_ENA_S)
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_ENA_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ENA    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_M  (AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_V << AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ENA    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ENA_M  (AXI_DMA_INFIFO_L1_OVF_CH1_INT_ENA_V << AXI_DMA_INFIFO_L1_OVF_CH1_INT_ENA_S)
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_ENA_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ENA    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ENA_M  (AXI_DMA_INFIFO_L1_UDF_CH1_INT_ENA_V << AXI_DMA_INFIFO_L1_UDF_CH1_INT_ENA_S)
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_ENA_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH1_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_ENA    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_ENA_M  (AXI_DMA_INFIFO_L2_OVF_CH1_INT_ENA_V << AXI_DMA_INFIFO_L2_OVF_CH1_INT_ENA_S)
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_ENA_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_ENA    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_ENA_M  (AXI_DMA_INFIFO_L2_UDF_CH1_INT_ENA_V << AXI_DMA_INFIFO_L2_UDF_CH1_INT_ENA_S)
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_ENA_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH1_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ENA    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ENA_M  (AXI_DMA_INFIFO_L3_OVF_CH1_INT_ENA_V << AXI_DMA_INFIFO_L3_OVF_CH1_INT_ENA_S)
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_ENA_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH1_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ENA    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ENA_M  (AXI_DMA_INFIFO_L3_UDF_CH1_INT_ENA_V << AXI_DMA_INFIFO_L3_UDF_CH1_INT_ENA_S)
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_ENA_S  10

/** AXI_DMA_IN_INT_CLR_CH1_REG register
 *  Interrupt clear bits of channel 1
 */
#define AXI_DMA_IN_INT_CLR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x74)
/** AXI_DMA_IN_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH1_INT_CLR    (BIT(0))
#define AXI_DMA_IN_DONE_CH1_INT_CLR_M  (AXI_DMA_IN_DONE_CH1_INT_CLR_V << AXI_DMA_IN_DONE_CH1_INT_CLR_S)
#define AXI_DMA_IN_DONE_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DONE_CH1_INT_CLR_S  0
/** AXI_DMA_IN_SUC_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH1_INT_CLR    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH1_INT_CLR_M  (AXI_DMA_IN_SUC_EOF_CH1_INT_CLR_V << AXI_DMA_IN_SUC_EOF_CH1_INT_CLR_S)
#define AXI_DMA_IN_SUC_EOF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH1_INT_CLR_S  1
/** AXI_DMA_IN_ERR_EOF_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH1_INT_CLR    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH1_INT_CLR_M  (AXI_DMA_IN_ERR_EOF_CH1_INT_CLR_V << AXI_DMA_IN_ERR_EOF_CH1_INT_CLR_S)
#define AXI_DMA_IN_ERR_EOF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH1_INT_CLR_S  2
/** AXI_DMA_IN_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_CLR    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_CLR_M  (AXI_DMA_IN_DSCR_ERR_CH1_INT_CLR_V << AXI_DMA_IN_DSCR_ERR_CH1_INT_CLR_S)
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH1_INT_CLR_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_CLR    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_M  (AXI_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_V << AXI_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_CLR    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_CLR_M  (AXI_DMA_INFIFO_L1_OVF_CH1_INT_CLR_V << AXI_DMA_INFIFO_L1_OVF_CH1_INT_CLR_S)
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH1_INT_CLR_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_CLR    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_CLR_M  (AXI_DMA_INFIFO_L1_UDF_CH1_INT_CLR_V << AXI_DMA_INFIFO_L1_UDF_CH1_INT_CLR_S)
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH1_INT_CLR_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH1_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_CLR    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_CLR_M  (AXI_DMA_INFIFO_L2_OVF_CH1_INT_CLR_V << AXI_DMA_INFIFO_L2_OVF_CH1_INT_CLR_S)
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH1_INT_CLR_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_CLR    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_CLR_M  (AXI_DMA_INFIFO_L2_UDF_CH1_INT_CLR_V << AXI_DMA_INFIFO_L2_UDF_CH1_INT_CLR_S)
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH1_INT_CLR_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH1_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_CLR    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_CLR_M  (AXI_DMA_INFIFO_L3_OVF_CH1_INT_CLR_V << AXI_DMA_INFIFO_L3_OVF_CH1_INT_CLR_S)
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH1_INT_CLR_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH1_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_CLR    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_CLR_M  (AXI_DMA_INFIFO_L3_UDF_CH1_INT_CLR_V << AXI_DMA_INFIFO_L3_UDF_CH1_INT_CLR_S)
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH1_INT_CLR_S  10

/** AXI_DMA_IN_CONF0_CH1_REG register
 *  Configure 0 register of Rx channel 1
 */
#define AXI_DMA_IN_CONF0_CH1_REG (DR_REG_AXI_DMA_BASE + 0x78)
/** AXI_DMA_IN_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AXI_DMA channel 1 Rx FSM and Rx FIFO pointer.
 */
#define AXI_DMA_IN_RST_CH1    (BIT(0))
#define AXI_DMA_IN_RST_CH1_M  (AXI_DMA_IN_RST_CH1_V << AXI_DMA_IN_RST_CH1_S)
#define AXI_DMA_IN_RST_CH1_V  0x00000001U
#define AXI_DMA_IN_RST_CH1_S  0
/** AXI_DMA_IN_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_LOOP_TEST_CH1    (BIT(1))
#define AXI_DMA_IN_LOOP_TEST_CH1_M  (AXI_DMA_IN_LOOP_TEST_CH1_V << AXI_DMA_IN_LOOP_TEST_CH1_S)
#define AXI_DMA_IN_LOOP_TEST_CH1_V  0x00000001U
#define AXI_DMA_IN_LOOP_TEST_CH1_S  1
/** AXI_DMA_MEM_TRANS_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via
 *  AXI_DMA.
 */
#define AXI_DMA_MEM_TRANS_EN_CH1    (BIT(2))
#define AXI_DMA_MEM_TRANS_EN_CH1_M  (AXI_DMA_MEM_TRANS_EN_CH1_V << AXI_DMA_MEM_TRANS_EN_CH1_S)
#define AXI_DMA_MEM_TRANS_EN_CH1_V  0x00000001U
#define AXI_DMA_MEM_TRANS_EN_CH1_S  2
/** AXI_DMA_IN_ETM_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 1 is triggered by etm
 *  task.
 */
#define AXI_DMA_IN_ETM_EN_CH1    (BIT(3))
#define AXI_DMA_IN_ETM_EN_CH1_M  (AXI_DMA_IN_ETM_EN_CH1_V << AXI_DMA_IN_ETM_EN_CH1_S)
#define AXI_DMA_IN_ETM_EN_CH1_V  0x00000001U
#define AXI_DMA_IN_ETM_EN_CH1_S  3
/** AXI_DMA_IN_BURST_SIZE_SEL_CH1 : R/W; bitpos: [6:4]; default: 0;
 *  3'b000-3'b100:burst length 8byte~128byte
 */
#define AXI_DMA_IN_BURST_SIZE_SEL_CH1    0x00000007U
#define AXI_DMA_IN_BURST_SIZE_SEL_CH1_M  (AXI_DMA_IN_BURST_SIZE_SEL_CH1_V << AXI_DMA_IN_BURST_SIZE_SEL_CH1_S)
#define AXI_DMA_IN_BURST_SIZE_SEL_CH1_V  0x00000007U
#define AXI_DMA_IN_BURST_SIZE_SEL_CH1_S  4
/** AXI_DMA_IN_CMD_DISABLE_CH1 : R/W; bitpos: [7]; default: 0;
 *  1:mean disable cmd of this ch1
 */
#define AXI_DMA_IN_CMD_DISABLE_CH1    (BIT(7))
#define AXI_DMA_IN_CMD_DISABLE_CH1_M  (AXI_DMA_IN_CMD_DISABLE_CH1_V << AXI_DMA_IN_CMD_DISABLE_CH1_S)
#define AXI_DMA_IN_CMD_DISABLE_CH1_V  0x00000001U
#define AXI_DMA_IN_CMD_DISABLE_CH1_S  7
/** AXI_DMA_IN_ECC_AEC_EN_CH1 : R/W; bitpos: [8]; default: 0;
 *  1: mean access ecc or aes domain,0: mean not
 */
#define AXI_DMA_IN_ECC_AEC_EN_CH1    (BIT(8))
#define AXI_DMA_IN_ECC_AEC_EN_CH1_M  (AXI_DMA_IN_ECC_AEC_EN_CH1_V << AXI_DMA_IN_ECC_AEC_EN_CH1_S)
#define AXI_DMA_IN_ECC_AEC_EN_CH1_V  0x00000001U
#define AXI_DMA_IN_ECC_AEC_EN_CH1_S  8
/** AXI_DMA_INDSCR_BURST_EN_CH1 : R/W; bitpos: [9]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 1 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AXI_DMA_INDSCR_BURST_EN_CH1    (BIT(9))
#define AXI_DMA_INDSCR_BURST_EN_CH1_M  (AXI_DMA_INDSCR_BURST_EN_CH1_V << AXI_DMA_INDSCR_BURST_EN_CH1_S)
#define AXI_DMA_INDSCR_BURST_EN_CH1_V  0x00000001U
#define AXI_DMA_INDSCR_BURST_EN_CH1_S  9

/** AXI_DMA_IN_CONF1_CH1_REG register
 *  Configure 1 register of Rx channel 1
 */
#define AXI_DMA_IN_CONF1_CH1_REG (DR_REG_AXI_DMA_BASE + 0x7c)
/** AXI_DMA_IN_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AXI_DMA_IN_CHECK_OWNER_CH1    (BIT(12))
#define AXI_DMA_IN_CHECK_OWNER_CH1_M  (AXI_DMA_IN_CHECK_OWNER_CH1_V << AXI_DMA_IN_CHECK_OWNER_CH1_S)
#define AXI_DMA_IN_CHECK_OWNER_CH1_V  0x00000001U
#define AXI_DMA_IN_CHECK_OWNER_CH1_S  12

/** AXI_DMA_INFIFO_STATUS_CH1_REG register
 *  Receive FIFO status of Rx channel 1
 */
#define AXI_DMA_INFIFO_STATUS_CH1_REG (DR_REG_AXI_DMA_BASE + 0x80)
/** AXI_DMA_INFIFO_L3_FULL_CH1 : RO; bitpos: [0]; default: 1;
 *  L3 Rx FIFO full signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L3_FULL_CH1    (BIT(0))
#define AXI_DMA_INFIFO_L3_FULL_CH1_M  (AXI_DMA_INFIFO_L3_FULL_CH1_V << AXI_DMA_INFIFO_L3_FULL_CH1_S)
#define AXI_DMA_INFIFO_L3_FULL_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L3_FULL_CH1_S  0
/** AXI_DMA_INFIFO_L3_EMPTY_CH1 : RO; bitpos: [1]; default: 1;
 *  L3 Rx FIFO empty signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L3_EMPTY_CH1    (BIT(1))
#define AXI_DMA_INFIFO_L3_EMPTY_CH1_M  (AXI_DMA_INFIFO_L3_EMPTY_CH1_V << AXI_DMA_INFIFO_L3_EMPTY_CH1_S)
#define AXI_DMA_INFIFO_L3_EMPTY_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L3_EMPTY_CH1_S  1
/** AXI_DMA_INFIFO_L3_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L3 Rx FIFO for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L3_CNT_CH1    0x0000003FU
#define AXI_DMA_INFIFO_L3_CNT_CH1_M  (AXI_DMA_INFIFO_L3_CNT_CH1_V << AXI_DMA_INFIFO_L3_CNT_CH1_S)
#define AXI_DMA_INFIFO_L3_CNT_CH1_V  0x0000003FU
#define AXI_DMA_INFIFO_L3_CNT_CH1_S  2
/** AXI_DMA_INFIFO_L3_UDF_CH1 : RO; bitpos: [8]; default: 0;
 *  L3 Rx FIFO under flow signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH1    (BIT(8))
#define AXI_DMA_INFIFO_L3_UDF_CH1_M  (AXI_DMA_INFIFO_L3_UDF_CH1_V << AXI_DMA_INFIFO_L3_UDF_CH1_S)
#define AXI_DMA_INFIFO_L3_UDF_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH1_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH1 : RO; bitpos: [9]; default: 0;
 *  L3 Rx FIFO over flow signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH1    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH1_M  (AXI_DMA_INFIFO_L3_OVF_CH1_V << AXI_DMA_INFIFO_L3_OVF_CH1_S)
#define AXI_DMA_INFIFO_L3_OVF_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH1_S  9
/** AXI_DMA_INFIFO_L1_FULL_CH1 : RO; bitpos: [10]; default: 0;
 *  L1 Rx FIFO full signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L1_FULL_CH1    (BIT(10))
#define AXI_DMA_INFIFO_L1_FULL_CH1_M  (AXI_DMA_INFIFO_L1_FULL_CH1_V << AXI_DMA_INFIFO_L1_FULL_CH1_S)
#define AXI_DMA_INFIFO_L1_FULL_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L1_FULL_CH1_S  10
/** AXI_DMA_INFIFO_L1_EMPTY_CH1 : RO; bitpos: [11]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L1_EMPTY_CH1    (BIT(11))
#define AXI_DMA_INFIFO_L1_EMPTY_CH1_M  (AXI_DMA_INFIFO_L1_EMPTY_CH1_V << AXI_DMA_INFIFO_L1_EMPTY_CH1_S)
#define AXI_DMA_INFIFO_L1_EMPTY_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L1_EMPTY_CH1_S  11
/** AXI_DMA_INFIFO_L1_UDF_CH1 : RO; bitpos: [12]; default: 0;
 *  L1 Rx FIFO under flow signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH1    (BIT(12))
#define AXI_DMA_INFIFO_L1_UDF_CH1_M  (AXI_DMA_INFIFO_L1_UDF_CH1_V << AXI_DMA_INFIFO_L1_UDF_CH1_S)
#define AXI_DMA_INFIFO_L1_UDF_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH1_S  12
/** AXI_DMA_INFIFO_L1_OVF_CH1 : RO; bitpos: [13]; default: 0;
 *  L1 Rx FIFO over flow signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH1    (BIT(13))
#define AXI_DMA_INFIFO_L1_OVF_CH1_M  (AXI_DMA_INFIFO_L1_OVF_CH1_V << AXI_DMA_INFIFO_L1_OVF_CH1_S)
#define AXI_DMA_INFIFO_L1_OVF_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH1_S  13
/** AXI_DMA_INFIFO_L2_FULL_CH1 : RO; bitpos: [14]; default: 0;
 *  L2 Rx RAM full signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L2_FULL_CH1    (BIT(14))
#define AXI_DMA_INFIFO_L2_FULL_CH1_M  (AXI_DMA_INFIFO_L2_FULL_CH1_V << AXI_DMA_INFIFO_L2_FULL_CH1_S)
#define AXI_DMA_INFIFO_L2_FULL_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L2_FULL_CH1_S  14
/** AXI_DMA_INFIFO_L2_EMPTY_CH1 : RO; bitpos: [15]; default: 1;
 *  L2 Rx RAM empty signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L2_EMPTY_CH1    (BIT(15))
#define AXI_DMA_INFIFO_L2_EMPTY_CH1_M  (AXI_DMA_INFIFO_L2_EMPTY_CH1_V << AXI_DMA_INFIFO_L2_EMPTY_CH1_S)
#define AXI_DMA_INFIFO_L2_EMPTY_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L2_EMPTY_CH1_S  15
/** AXI_DMA_INFIFO_L2_UDF_CH1 : RO; bitpos: [16]; default: 0;
 *  L2 Rx FIFO under flow signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH1    (BIT(16))
#define AXI_DMA_INFIFO_L2_UDF_CH1_M  (AXI_DMA_INFIFO_L2_UDF_CH1_V << AXI_DMA_INFIFO_L2_UDF_CH1_S)
#define AXI_DMA_INFIFO_L2_UDF_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH1_S  16
/** AXI_DMA_INFIFO_L2_OVF_CH1 : RO; bitpos: [17]; default: 0;
 *  L2 Rx FIFO over flow signal for Rx channel 1.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH1    (BIT(17))
#define AXI_DMA_INFIFO_L2_OVF_CH1_M  (AXI_DMA_INFIFO_L2_OVF_CH1_V << AXI_DMA_INFIFO_L2_OVF_CH1_S)
#define AXI_DMA_INFIFO_L2_OVF_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH1_S  17
/** AXI_DMA_IN_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH1    (BIT(23))
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_1B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_1B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH1_S  23
/** AXI_DMA_IN_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH1    (BIT(24))
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_2B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_2B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH1_S  24
/** AXI_DMA_IN_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH1    (BIT(25))
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_3B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_3B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH1_S  25
/** AXI_DMA_IN_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH1    (BIT(26))
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_4B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_4B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH1_S  26
/** AXI_DMA_IN_REMAIN_UNDER_5B_CH1 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH1    (BIT(27))
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_5B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_5B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH1_S  27
/** AXI_DMA_IN_REMAIN_UNDER_6B_CH1 : RO; bitpos: [28]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH1    (BIT(28))
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_6B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_6B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH1_S  28
/** AXI_DMA_IN_REMAIN_UNDER_7B_CH1 : RO; bitpos: [29]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH1    (BIT(29))
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_7B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_7B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH1_S  29
/** AXI_DMA_IN_REMAIN_UNDER_8B_CH1 : RO; bitpos: [30]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH1    (BIT(30))
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH1_M  (AXI_DMA_IN_REMAIN_UNDER_8B_CH1_V << AXI_DMA_IN_REMAIN_UNDER_8B_CH1_S)
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH1_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH1_S  30
/** AXI_DMA_IN_BUF_HUNGRY_CH1 : RO; bitpos: [31]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_BUF_HUNGRY_CH1    (BIT(31))
#define AXI_DMA_IN_BUF_HUNGRY_CH1_M  (AXI_DMA_IN_BUF_HUNGRY_CH1_V << AXI_DMA_IN_BUF_HUNGRY_CH1_S)
#define AXI_DMA_IN_BUF_HUNGRY_CH1_V  0x00000001U
#define AXI_DMA_IN_BUF_HUNGRY_CH1_S  31

/** AXI_DMA_IN_POP_CH1_REG register
 *  Pop control register of Rx channel 1
 */
#define AXI_DMA_IN_POP_CH1_REG (DR_REG_AXI_DMA_BASE + 0x84)
/** AXI_DMA_INFIFO_RDATA_CH1 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from AXI_DMA FIFO.
 */
#define AXI_DMA_INFIFO_RDATA_CH1    0x00000FFFU
#define AXI_DMA_INFIFO_RDATA_CH1_M  (AXI_DMA_INFIFO_RDATA_CH1_V << AXI_DMA_INFIFO_RDATA_CH1_S)
#define AXI_DMA_INFIFO_RDATA_CH1_V  0x00000FFFU
#define AXI_DMA_INFIFO_RDATA_CH1_S  0
/** AXI_DMA_INFIFO_POP_CH1 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from AXI_DMA FIFO.
 */
#define AXI_DMA_INFIFO_POP_CH1    (BIT(12))
#define AXI_DMA_INFIFO_POP_CH1_M  (AXI_DMA_INFIFO_POP_CH1_V << AXI_DMA_INFIFO_POP_CH1_S)
#define AXI_DMA_INFIFO_POP_CH1_V  0x00000001U
#define AXI_DMA_INFIFO_POP_CH1_S  12

/** AXI_DMA_IN_LINK1_CH1_REG register
 *  Link descriptor configure and control register of Rx channel 1
 */
#define AXI_DMA_IN_LINK1_CH1_REG (DR_REG_AXI_DMA_BASE + 0x88)
/** AXI_DMA_INLINK_AUTO_RET_CH1 : R/W; bitpos: [0]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define AXI_DMA_INLINK_AUTO_RET_CH1    (BIT(0))
#define AXI_DMA_INLINK_AUTO_RET_CH1_M  (AXI_DMA_INLINK_AUTO_RET_CH1_V << AXI_DMA_INLINK_AUTO_RET_CH1_S)
#define AXI_DMA_INLINK_AUTO_RET_CH1_V  0x00000001U
#define AXI_DMA_INLINK_AUTO_RET_CH1_S  0
/** AXI_DMA_INLINK_STOP_CH1 : WT; bitpos: [1]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define AXI_DMA_INLINK_STOP_CH1    (BIT(1))
#define AXI_DMA_INLINK_STOP_CH1_M  (AXI_DMA_INLINK_STOP_CH1_V << AXI_DMA_INLINK_STOP_CH1_S)
#define AXI_DMA_INLINK_STOP_CH1_V  0x00000001U
#define AXI_DMA_INLINK_STOP_CH1_S  1
/** AXI_DMA_INLINK_START_CH1 : WT; bitpos: [2]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define AXI_DMA_INLINK_START_CH1    (BIT(2))
#define AXI_DMA_INLINK_START_CH1_M  (AXI_DMA_INLINK_START_CH1_V << AXI_DMA_INLINK_START_CH1_S)
#define AXI_DMA_INLINK_START_CH1_V  0x00000001U
#define AXI_DMA_INLINK_START_CH1_S  2
/** AXI_DMA_INLINK_RESTART_CH1 : WT; bitpos: [3]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define AXI_DMA_INLINK_RESTART_CH1    (BIT(3))
#define AXI_DMA_INLINK_RESTART_CH1_M  (AXI_DMA_INLINK_RESTART_CH1_V << AXI_DMA_INLINK_RESTART_CH1_S)
#define AXI_DMA_INLINK_RESTART_CH1_V  0x00000001U
#define AXI_DMA_INLINK_RESTART_CH1_S  3
/** AXI_DMA_INLINK_PARK_CH1 : RO; bitpos: [4]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define AXI_DMA_INLINK_PARK_CH1    (BIT(4))
#define AXI_DMA_INLINK_PARK_CH1_M  (AXI_DMA_INLINK_PARK_CH1_V << AXI_DMA_INLINK_PARK_CH1_S)
#define AXI_DMA_INLINK_PARK_CH1_V  0x00000001U
#define AXI_DMA_INLINK_PARK_CH1_S  4

/** AXI_DMA_IN_LINK2_CH1_REG register
 *  Link descriptor configure and control register of Rx channel 1
 */
#define AXI_DMA_IN_LINK2_CH1_REG (DR_REG_AXI_DMA_BASE + 0x8c)
/** AXI_DMA_INLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 20 least significant bits of the first inlink descriptor's
 *  address.
 */
#define AXI_DMA_INLINK_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_INLINK_ADDR_CH1_M  (AXI_DMA_INLINK_ADDR_CH1_V << AXI_DMA_INLINK_ADDR_CH1_S)
#define AXI_DMA_INLINK_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_ADDR_CH1_S  0

/** AXI_DMA_IN_STATE_CH1_REG register
 *  Receive status of Rx channel 1
 */
#define AXI_DMA_IN_STATE_CH1_REG (DR_REG_AXI_DMA_BASE + 0x90)
/** AXI_DMA_INLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define AXI_DMA_INLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define AXI_DMA_INLINK_DSCR_ADDR_CH1_M  (AXI_DMA_INLINK_DSCR_ADDR_CH1_V << AXI_DMA_INLINK_DSCR_ADDR_CH1_S)
#define AXI_DMA_INLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define AXI_DMA_INLINK_DSCR_ADDR_CH1_S  0
/** AXI_DMA_IN_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_DSCR_STATE_CH1    0x00000003U
#define AXI_DMA_IN_DSCR_STATE_CH1_M  (AXI_DMA_IN_DSCR_STATE_CH1_V << AXI_DMA_IN_DSCR_STATE_CH1_S)
#define AXI_DMA_IN_DSCR_STATE_CH1_V  0x00000003U
#define AXI_DMA_IN_DSCR_STATE_CH1_S  18
/** AXI_DMA_IN_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_STATE_CH1    0x00000007U
#define AXI_DMA_IN_STATE_CH1_M  (AXI_DMA_IN_STATE_CH1_V << AXI_DMA_IN_STATE_CH1_S)
#define AXI_DMA_IN_STATE_CH1_V  0x00000007U
#define AXI_DMA_IN_STATE_CH1_S  20

/** AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 1
 */
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x94)
/** AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1_M  (AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1_V << AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1_S)
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH1_S  0

/** AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when errors occur of Rx channel 1
 */
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x98)
/** AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1_M  (AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1_V << AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1_S)
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH1_S  0

/** AXI_DMA_IN_DSCR_CH1_REG register
 *  Current inlink descriptor address of Rx channel 1
 */
#define AXI_DMA_IN_DSCR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x9c)
/** AXI_DMA_INLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define AXI_DMA_INLINK_DSCR_CH1    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_CH1_M  (AXI_DMA_INLINK_DSCR_CH1_V << AXI_DMA_INLINK_DSCR_CH1_S)
#define AXI_DMA_INLINK_DSCR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_CH1_S  0

/** AXI_DMA_IN_DSCR_BF0_CH1_REG register
 *  The last inlink descriptor address of Rx channel 1
 */
#define AXI_DMA_IN_DSCR_BF0_CH1_REG (DR_REG_AXI_DMA_BASE + 0xa0)
/** AXI_DMA_INLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define AXI_DMA_INLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF0_CH1_M  (AXI_DMA_INLINK_DSCR_BF0_CH1_V << AXI_DMA_INLINK_DSCR_BF0_CH1_S)
#define AXI_DMA_INLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF0_CH1_S  0

/** AXI_DMA_IN_DSCR_BF1_CH1_REG register
 *  The second-to-last inlink descriptor address of Rx channel 1
 */
#define AXI_DMA_IN_DSCR_BF1_CH1_REG (DR_REG_AXI_DMA_BASE + 0xa4)
/** AXI_DMA_INLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AXI_DMA_INLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF1_CH1_M  (AXI_DMA_INLINK_DSCR_BF1_CH1_V << AXI_DMA_INLINK_DSCR_BF1_CH1_S)
#define AXI_DMA_INLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF1_CH1_S  0

/** AXI_DMA_IN_PRI_CH1_REG register
 *  Priority register of Rx channel 1
 */
#define AXI_DMA_IN_PRI_CH1_REG (DR_REG_AXI_DMA_BASE + 0xa8)
/** AXI_DMA_RX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 1. The larger of the value the higher of the priority.
 */
#define AXI_DMA_RX_PRI_CH1    0x0000000FU
#define AXI_DMA_RX_PRI_CH1_M  (AXI_DMA_RX_PRI_CH1_V << AXI_DMA_RX_PRI_CH1_S)
#define AXI_DMA_RX_PRI_CH1_V  0x0000000FU
#define AXI_DMA_RX_PRI_CH1_S  0
/** AXI_DMA_RX_CH_ARB_WEIGH_CH1 : R/W; bitpos: [7:4]; default: 0;
 *  The weight of Rx channel 1
 */
#define AXI_DMA_RX_CH_ARB_WEIGH_CH1    0x0000000FU
#define AXI_DMA_RX_CH_ARB_WEIGH_CH1_M  (AXI_DMA_RX_CH_ARB_WEIGH_CH1_V << AXI_DMA_RX_CH_ARB_WEIGH_CH1_S)
#define AXI_DMA_RX_CH_ARB_WEIGH_CH1_V  0x0000000FU
#define AXI_DMA_RX_CH_ARB_WEIGH_CH1_S  4
/** AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH1 : R/W; bitpos: [8]; default: 0;
 *  0: mean not optimization weight function ,1: mean optimization
 */
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH1    (BIT(8))
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_M  (AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_V << AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_S)
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_V  0x00000001U
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_S  8

/** AXI_DMA_IN_PERI_SEL_CH1_REG register
 *  Peripheral selection of Rx channel 1
 */
#define AXI_DMA_IN_PERI_SEL_CH1_REG (DR_REG_AXI_DMA_BASE + 0xac)
/** AXI_DMA_PERI_IN_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 1. 0:lcdcam. 1: gpspi_2.
 *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
 */
#define AXI_DMA_PERI_IN_SEL_CH1    0x0000003FU
#define AXI_DMA_PERI_IN_SEL_CH1_M  (AXI_DMA_PERI_IN_SEL_CH1_V << AXI_DMA_PERI_IN_SEL_CH1_S)
#define AXI_DMA_PERI_IN_SEL_CH1_V  0x0000003FU
#define AXI_DMA_PERI_IN_SEL_CH1_S  0

/** AXI_DMA_IN_CRC_INIT_DATA_CH1_REG register
 *  This register is used to config ch1 crc initial data(max 32 bit)
 */
#define AXI_DMA_IN_CRC_INIT_DATA_CH1_REG (DR_REG_AXI_DMA_BASE + 0xb0)
/** AXI_DMA_IN_CRC_INIT_DATA_CH1 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch1 of rx crc initial value
 */
#define AXI_DMA_IN_CRC_INIT_DATA_CH1    0xFFFFFFFFU
#define AXI_DMA_IN_CRC_INIT_DATA_CH1_M  (AXI_DMA_IN_CRC_INIT_DATA_CH1_V << AXI_DMA_IN_CRC_INIT_DATA_CH1_S)
#define AXI_DMA_IN_CRC_INIT_DATA_CH1_V  0xFFFFFFFFU
#define AXI_DMA_IN_CRC_INIT_DATA_CH1_S  0

/** AXI_DMA_RX_CRC_WIDTH_CH1_REG register
 *  This register is used to config rx ch1 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AXI_DMA_RX_CRC_WIDTH_CH1_REG (DR_REG_AXI_DMA_BASE + 0xb4)
/** AXI_DMA_RX_CRC_WIDTH_CH1 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_WIDTH_CH1    0x00000003U
#define AXI_DMA_RX_CRC_WIDTH_CH1_M  (AXI_DMA_RX_CRC_WIDTH_CH1_V << AXI_DMA_RX_CRC_WIDTH_CH1_S)
#define AXI_DMA_RX_CRC_WIDTH_CH1_V  0x00000003U
#define AXI_DMA_RX_CRC_WIDTH_CH1_S  0
/** AXI_DMA_RX_CRC_LATCH_FLAG_CH1 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH1    (BIT(2))
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH1_M  (AXI_DMA_RX_CRC_LATCH_FLAG_CH1_V << AXI_DMA_RX_CRC_LATCH_FLAG_CH1_S)
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH1_V  0x00000001U
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH1_S  2

/** AXI_DMA_IN_CRC_CLEAR_CH1_REG register
 *  This register is used to clear ch1 crc result
 */
#define AXI_DMA_IN_CRC_CLEAR_CH1_REG (DR_REG_AXI_DMA_BASE + 0xb8)
/** AXI_DMA_IN_CRC_CLEAR_CH1 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch1 of rx crc result
 */
#define AXI_DMA_IN_CRC_CLEAR_CH1    (BIT(0))
#define AXI_DMA_IN_CRC_CLEAR_CH1_M  (AXI_DMA_IN_CRC_CLEAR_CH1_V << AXI_DMA_IN_CRC_CLEAR_CH1_S)
#define AXI_DMA_IN_CRC_CLEAR_CH1_V  0x00000001U
#define AXI_DMA_IN_CRC_CLEAR_CH1_S  0

/** AXI_DMA_IN_CRC_FINAL_RESULT_CH1_REG register
 *  This register is used to store ch1 crc result
 */
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH1_REG (DR_REG_AXI_DMA_BASE + 0xbc)
/** AXI_DMA_IN_CRC_FINAL_RESULT_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch1 of rx
 */
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH1    0xFFFFFFFFU
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH1_M  (AXI_DMA_IN_CRC_FINAL_RESULT_CH1_V << AXI_DMA_IN_CRC_FINAL_RESULT_CH1_S)
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH1_V  0xFFFFFFFFU
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH1_S  0

/** AXI_DMA_RX_CRC_EN_WR_DATA_CH1_REG register
 *  This resister is used to config ch1 crc en for every bit
 */
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH1_REG (DR_REG_AXI_DMA_BASE + 0xc0)
/** AXI_DMA_RX_CRC_EN_WR_DATA_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable rx ch1 crc 32bit on/off
 */
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH1    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH1_M  (AXI_DMA_RX_CRC_EN_WR_DATA_CH1_V << AXI_DMA_RX_CRC_EN_WR_DATA_CH1_S)
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH1_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH1_S  0

/** AXI_DMA_RX_CRC_EN_ADDR_CH1_REG register
 *  This register is used to config ch1 crc en addr
 */
#define AXI_DMA_RX_CRC_EN_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0xc4)
/** AXI_DMA_RX_CRC_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_EN_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_ADDR_CH1_M  (AXI_DMA_RX_CRC_EN_ADDR_CH1_V << AXI_DMA_RX_CRC_EN_ADDR_CH1_S)
#define AXI_DMA_RX_CRC_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_ADDR_CH1_S  0

/** AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_REG register
 *  This register is used to config crc data_8bit en
 */
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_REG (DR_REG_AXI_DMA_BASE + 0xc8)
/** AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1 : R/W; bitpos: [15:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1    0x0000FFFFU
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_M  (AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_V << AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_S)
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_V  0x0000FFFFU
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_S  0

/** AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0xcc)
/** AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1_M  (AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1_V << AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1_S)
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH1_S  0

/** AXI_DMA_IN_INT_RAW_CH2_REG register
 *  Raw status interrupt of channel 2
 */
#define AXI_DMA_IN_INT_RAW_CH2_REG (DR_REG_AXI_DMA_BASE + 0xd0)
/** AXI_DMA_IN_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 2.
 */
#define AXI_DMA_IN_DONE_CH2_INT_RAW    (BIT(0))
#define AXI_DMA_IN_DONE_CH2_INT_RAW_M  (AXI_DMA_IN_DONE_CH2_INT_RAW_V << AXI_DMA_IN_DONE_CH2_INT_RAW_S)
#define AXI_DMA_IN_DONE_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DONE_CH2_INT_RAW_S  0
/** AXI_DMA_IN_SUC_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 2. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 2.
 */
#define AXI_DMA_IN_SUC_EOF_CH2_INT_RAW    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH2_INT_RAW_M  (AXI_DMA_IN_SUC_EOF_CH2_INT_RAW_V << AXI_DMA_IN_SUC_EOF_CH2_INT_RAW_S)
#define AXI_DMA_IN_SUC_EOF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH2_INT_RAW_S  1
/** AXI_DMA_IN_ERR_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 2. For other peripherals this raw
 *  interrupt is reserved.
 */
#define AXI_DMA_IN_ERR_EOF_CH2_INT_RAW    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH2_INT_RAW_M  (AXI_DMA_IN_ERR_EOF_CH2_INT_RAW_V << AXI_DMA_IN_ERR_EOF_CH2_INT_RAW_S)
#define AXI_DMA_IN_ERR_EOF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH2_INT_RAW_S  2
/** AXI_DMA_IN_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 2.
 */
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_RAW    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_RAW_M  (AXI_DMA_IN_DSCR_ERR_CH2_INT_RAW_V << AXI_DMA_IN_DSCR_ERR_CH2_INT_RAW_S)
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_RAW_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 2.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_RAW    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_M  (AXI_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_V << AXI_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_RAW    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_RAW_M  (AXI_DMA_INFIFO_L1_OVF_CH2_INT_RAW_V << AXI_DMA_INFIFO_L1_OVF_CH2_INT_RAW_S)
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_RAW_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_RAW    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_RAW_M  (AXI_DMA_INFIFO_L1_UDF_CH2_INT_RAW_V << AXI_DMA_INFIFO_L1_UDF_CH2_INT_RAW_S)
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_RAW_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_RAW    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_RAW_M  (AXI_DMA_INFIFO_L2_OVF_CH2_INT_RAW_V << AXI_DMA_INFIFO_L2_OVF_CH2_INT_RAW_S)
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_RAW_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_RAW    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_RAW_M  (AXI_DMA_INFIFO_L2_UDF_CH2_INT_RAW_V << AXI_DMA_INFIFO_L2_UDF_CH2_INT_RAW_S)
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_RAW_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  overflow.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_RAW    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_RAW_M  (AXI_DMA_INFIFO_L3_OVF_CH2_INT_RAW_V << AXI_DMA_INFIFO_L3_OVF_CH2_INT_RAW_S)
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_RAW_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  underflow.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_RAW    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_RAW_M  (AXI_DMA_INFIFO_L3_UDF_CH2_INT_RAW_V << AXI_DMA_INFIFO_L3_UDF_CH2_INT_RAW_S)
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_RAW_S  10

/** AXI_DMA_IN_INT_ST_CH2_REG register
 *  Masked interrupt of channel 2
 */
#define AXI_DMA_IN_INT_ST_CH2_REG (DR_REG_AXI_DMA_BASE + 0xd4)
/** AXI_DMA_IN_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH2_INT_ST    (BIT(0))
#define AXI_DMA_IN_DONE_CH2_INT_ST_M  (AXI_DMA_IN_DONE_CH2_INT_ST_V << AXI_DMA_IN_DONE_CH2_INT_ST_S)
#define AXI_DMA_IN_DONE_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DONE_CH2_INT_ST_S  0
/** AXI_DMA_IN_SUC_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ST    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ST_M  (AXI_DMA_IN_SUC_EOF_CH2_INT_ST_V << AXI_DMA_IN_SUC_EOF_CH2_INT_ST_S)
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ST_S  1
/** AXI_DMA_IN_ERR_EOF_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ST    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ST_M  (AXI_DMA_IN_ERR_EOF_CH2_INT_ST_V << AXI_DMA_IN_ERR_EOF_CH2_INT_ST_S)
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ST_S  2
/** AXI_DMA_IN_DSCR_ERR_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ST    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ST_M  (AXI_DMA_IN_DSCR_ERR_CH2_INT_ST_V << AXI_DMA_IN_DSCR_ERR_CH2_INT_ST_S)
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ST_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ST    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ST_M  (AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ST_V << AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ST_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ST_S  4
/** AXI_DMA_INFIFO_OVF_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_OVF_CH2_INT_ST    (BIT(5))
#define AXI_DMA_INFIFO_OVF_CH2_INT_ST_M  (AXI_DMA_INFIFO_OVF_CH2_INT_ST_V << AXI_DMA_INFIFO_OVF_CH2_INT_ST_S)
#define AXI_DMA_INFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_OVF_CH2_INT_ST_S  5
/** AXI_DMA_INFIFO_UDF_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_UDF_CH2_INT_ST    (BIT(6))
#define AXI_DMA_INFIFO_UDF_CH2_INT_ST_M  (AXI_DMA_INFIFO_UDF_CH2_INT_ST_V << AXI_DMA_INFIFO_UDF_CH2_INT_ST_S)
#define AXI_DMA_INFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_UDF_CH2_INT_ST_S  6
/** AXI_DMA_INFIFO_L1_OVF_CH2_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ST    (BIT(7))
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ST_M  (AXI_DMA_INFIFO_L1_OVF_CH2_INT_ST_V << AXI_DMA_INFIFO_L1_OVF_CH2_INT_ST_S)
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ST_S  7
/** AXI_DMA_INFIFO_L1_UDF_CH2_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ST    (BIT(8))
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ST_M  (AXI_DMA_INFIFO_L1_UDF_CH2_INT_ST_V << AXI_DMA_INFIFO_L1_UDF_CH2_INT_ST_S)
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ST_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH2_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ST    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ST_M  (AXI_DMA_INFIFO_L3_OVF_CH2_INT_ST_V << AXI_DMA_INFIFO_L3_OVF_CH2_INT_ST_S)
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ST_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH2_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ST    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ST_M  (AXI_DMA_INFIFO_L3_UDF_CH2_INT_ST_V << AXI_DMA_INFIFO_L3_UDF_CH2_INT_ST_S)
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ST_S  10

/** AXI_DMA_IN_INT_ENA_CH2_REG register
 *  Interrupt enable bits of channel 2
 */
#define AXI_DMA_IN_INT_ENA_CH2_REG (DR_REG_AXI_DMA_BASE + 0xd8)
/** AXI_DMA_IN_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH2_INT_ENA    (BIT(0))
#define AXI_DMA_IN_DONE_CH2_INT_ENA_M  (AXI_DMA_IN_DONE_CH2_INT_ENA_V << AXI_DMA_IN_DONE_CH2_INT_ENA_S)
#define AXI_DMA_IN_DONE_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DONE_CH2_INT_ENA_S  0
/** AXI_DMA_IN_SUC_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ENA    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ENA_M  (AXI_DMA_IN_SUC_EOF_CH2_INT_ENA_V << AXI_DMA_IN_SUC_EOF_CH2_INT_ENA_S)
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH2_INT_ENA_S  1
/** AXI_DMA_IN_ERR_EOF_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ENA    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ENA_M  (AXI_DMA_IN_ERR_EOF_CH2_INT_ENA_V << AXI_DMA_IN_ERR_EOF_CH2_INT_ENA_S)
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH2_INT_ENA_S  2
/** AXI_DMA_IN_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ENA    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ENA_M  (AXI_DMA_IN_DSCR_ERR_CH2_INT_ENA_V << AXI_DMA_IN_DSCR_ERR_CH2_INT_ENA_S)
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_ENA_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ENA    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_M  (AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_V << AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ENA    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ENA_M  (AXI_DMA_INFIFO_L1_OVF_CH2_INT_ENA_V << AXI_DMA_INFIFO_L1_OVF_CH2_INT_ENA_S)
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_ENA_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ENA    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ENA_M  (AXI_DMA_INFIFO_L1_UDF_CH2_INT_ENA_V << AXI_DMA_INFIFO_L1_UDF_CH2_INT_ENA_S)
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_ENA_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH2_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_ENA    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_ENA_M  (AXI_DMA_INFIFO_L2_OVF_CH2_INT_ENA_V << AXI_DMA_INFIFO_L2_OVF_CH2_INT_ENA_S)
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_ENA_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH2_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_ENA    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_ENA_M  (AXI_DMA_INFIFO_L2_UDF_CH2_INT_ENA_V << AXI_DMA_INFIFO_L2_UDF_CH2_INT_ENA_S)
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_ENA_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH2_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ENA    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ENA_M  (AXI_DMA_INFIFO_L3_OVF_CH2_INT_ENA_V << AXI_DMA_INFIFO_L3_OVF_CH2_INT_ENA_S)
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_ENA_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH2_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ENA    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ENA_M  (AXI_DMA_INFIFO_L3_UDF_CH2_INT_ENA_V << AXI_DMA_INFIFO_L3_UDF_CH2_INT_ENA_S)
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_ENA_S  10

/** AXI_DMA_IN_INT_CLR_CH2_REG register
 *  Interrupt clear bits of channel 2
 */
#define AXI_DMA_IN_INT_CLR_CH2_REG (DR_REG_AXI_DMA_BASE + 0xdc)
/** AXI_DMA_IN_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define AXI_DMA_IN_DONE_CH2_INT_CLR    (BIT(0))
#define AXI_DMA_IN_DONE_CH2_INT_CLR_M  (AXI_DMA_IN_DONE_CH2_INT_CLR_V << AXI_DMA_IN_DONE_CH2_INT_CLR_S)
#define AXI_DMA_IN_DONE_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DONE_CH2_INT_CLR_S  0
/** AXI_DMA_IN_SUC_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_SUC_EOF_CH2_INT_CLR    (BIT(1))
#define AXI_DMA_IN_SUC_EOF_CH2_INT_CLR_M  (AXI_DMA_IN_SUC_EOF_CH2_INT_CLR_V << AXI_DMA_IN_SUC_EOF_CH2_INT_CLR_S)
#define AXI_DMA_IN_SUC_EOF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_SUC_EOF_CH2_INT_CLR_S  1
/** AXI_DMA_IN_ERR_EOF_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define AXI_DMA_IN_ERR_EOF_CH2_INT_CLR    (BIT(2))
#define AXI_DMA_IN_ERR_EOF_CH2_INT_CLR_M  (AXI_DMA_IN_ERR_EOF_CH2_INT_CLR_V << AXI_DMA_IN_ERR_EOF_CH2_INT_CLR_S)
#define AXI_DMA_IN_ERR_EOF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_ERR_EOF_CH2_INT_CLR_S  2
/** AXI_DMA_IN_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_CLR    (BIT(3))
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_CLR_M  (AXI_DMA_IN_DSCR_ERR_CH2_INT_CLR_V << AXI_DMA_IN_DSCR_ERR_CH2_INT_CLR_S)
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DSCR_ERR_CH2_INT_CLR_S  3
/** AXI_DMA_IN_DSCR_EMPTY_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_CLR    (BIT(4))
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_M  (AXI_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_V << AXI_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_S)
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_S  4
/** AXI_DMA_INFIFO_L1_OVF_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_CLR    (BIT(5))
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_CLR_M  (AXI_DMA_INFIFO_L1_OVF_CH2_INT_CLR_V << AXI_DMA_INFIFO_L1_OVF_CH2_INT_CLR_S)
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH2_INT_CLR_S  5
/** AXI_DMA_INFIFO_L1_UDF_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_CLR    (BIT(6))
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_CLR_M  (AXI_DMA_INFIFO_L1_UDF_CH2_INT_CLR_V << AXI_DMA_INFIFO_L1_UDF_CH2_INT_CLR_S)
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH2_INT_CLR_S  6
/** AXI_DMA_INFIFO_L2_OVF_CH2_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_CLR    (BIT(7))
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_CLR_M  (AXI_DMA_INFIFO_L2_OVF_CH2_INT_CLR_V << AXI_DMA_INFIFO_L2_OVF_CH2_INT_CLR_S)
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH2_INT_CLR_S  7
/** AXI_DMA_INFIFO_L2_UDF_CH2_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_CLR    (BIT(8))
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_CLR_M  (AXI_DMA_INFIFO_L2_UDF_CH2_INT_CLR_V << AXI_DMA_INFIFO_L2_UDF_CH2_INT_CLR_S)
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH2_INT_CLR_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH2_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_CLR    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_CLR_M  (AXI_DMA_INFIFO_L3_OVF_CH2_INT_CLR_V << AXI_DMA_INFIFO_L3_OVF_CH2_INT_CLR_S)
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH2_INT_CLR_S  9
/** AXI_DMA_INFIFO_L3_UDF_CH2_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_CLR    (BIT(10))
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_CLR_M  (AXI_DMA_INFIFO_L3_UDF_CH2_INT_CLR_V << AXI_DMA_INFIFO_L3_UDF_CH2_INT_CLR_S)
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH2_INT_CLR_S  10

/** AXI_DMA_IN_CONF0_CH2_REG register
 *  Configure 0 register of Rx channel 2
 */
#define AXI_DMA_IN_CONF0_CH2_REG (DR_REG_AXI_DMA_BASE + 0xe0)
/** AXI_DMA_IN_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AXI_DMA channel 2 Rx FSM and Rx FIFO pointer.
 */
#define AXI_DMA_IN_RST_CH2    (BIT(0))
#define AXI_DMA_IN_RST_CH2_M  (AXI_DMA_IN_RST_CH2_V << AXI_DMA_IN_RST_CH2_S)
#define AXI_DMA_IN_RST_CH2_V  0x00000001U
#define AXI_DMA_IN_RST_CH2_S  0
/** AXI_DMA_IN_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_LOOP_TEST_CH2    (BIT(1))
#define AXI_DMA_IN_LOOP_TEST_CH2_M  (AXI_DMA_IN_LOOP_TEST_CH2_V << AXI_DMA_IN_LOOP_TEST_CH2_S)
#define AXI_DMA_IN_LOOP_TEST_CH2_V  0x00000001U
#define AXI_DMA_IN_LOOP_TEST_CH2_S  1
/** AXI_DMA_MEM_TRANS_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via
 *  AXI_DMA.
 */
#define AXI_DMA_MEM_TRANS_EN_CH2    (BIT(2))
#define AXI_DMA_MEM_TRANS_EN_CH2_M  (AXI_DMA_MEM_TRANS_EN_CH2_V << AXI_DMA_MEM_TRANS_EN_CH2_S)
#define AXI_DMA_MEM_TRANS_EN_CH2_V  0x00000001U
#define AXI_DMA_MEM_TRANS_EN_CH2_S  2
/** AXI_DMA_IN_ETM_EN_CH2 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 2 is triggered by etm
 *  task.
 */
#define AXI_DMA_IN_ETM_EN_CH2    (BIT(3))
#define AXI_DMA_IN_ETM_EN_CH2_M  (AXI_DMA_IN_ETM_EN_CH2_V << AXI_DMA_IN_ETM_EN_CH2_S)
#define AXI_DMA_IN_ETM_EN_CH2_V  0x00000001U
#define AXI_DMA_IN_ETM_EN_CH2_S  3
/** AXI_DMA_IN_BURST_SIZE_SEL_CH2 : R/W; bitpos: [6:4]; default: 0;
 *  3'b000-3'b100:burst length 8byte~128byte
 */
#define AXI_DMA_IN_BURST_SIZE_SEL_CH2    0x00000007U
#define AXI_DMA_IN_BURST_SIZE_SEL_CH2_M  (AXI_DMA_IN_BURST_SIZE_SEL_CH2_V << AXI_DMA_IN_BURST_SIZE_SEL_CH2_S)
#define AXI_DMA_IN_BURST_SIZE_SEL_CH2_V  0x00000007U
#define AXI_DMA_IN_BURST_SIZE_SEL_CH2_S  4
/** AXI_DMA_IN_CMD_DISABLE_CH2 : R/W; bitpos: [7]; default: 0;
 *  1:mean disable cmd of this ch2
 */
#define AXI_DMA_IN_CMD_DISABLE_CH2    (BIT(7))
#define AXI_DMA_IN_CMD_DISABLE_CH2_M  (AXI_DMA_IN_CMD_DISABLE_CH2_V << AXI_DMA_IN_CMD_DISABLE_CH2_S)
#define AXI_DMA_IN_CMD_DISABLE_CH2_V  0x00000001U
#define AXI_DMA_IN_CMD_DISABLE_CH2_S  7
/** AXI_DMA_IN_ECC_AEC_EN_CH2 : R/W; bitpos: [8]; default: 0;
 *  1: mean access ecc or aes domain,0: mean not
 */
#define AXI_DMA_IN_ECC_AEC_EN_CH2    (BIT(8))
#define AXI_DMA_IN_ECC_AEC_EN_CH2_M  (AXI_DMA_IN_ECC_AEC_EN_CH2_V << AXI_DMA_IN_ECC_AEC_EN_CH2_S)
#define AXI_DMA_IN_ECC_AEC_EN_CH2_V  0x00000001U
#define AXI_DMA_IN_ECC_AEC_EN_CH2_S  8
/** AXI_DMA_INDSCR_BURST_EN_CH2 : R/W; bitpos: [9]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 2 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AXI_DMA_INDSCR_BURST_EN_CH2    (BIT(9))
#define AXI_DMA_INDSCR_BURST_EN_CH2_M  (AXI_DMA_INDSCR_BURST_EN_CH2_V << AXI_DMA_INDSCR_BURST_EN_CH2_S)
#define AXI_DMA_INDSCR_BURST_EN_CH2_V  0x00000001U
#define AXI_DMA_INDSCR_BURST_EN_CH2_S  9

/** AXI_DMA_IN_CONF1_CH2_REG register
 *  Configure 1 register of Rx channel 2
 */
#define AXI_DMA_IN_CONF1_CH2_REG (DR_REG_AXI_DMA_BASE + 0xe4)
/** AXI_DMA_IN_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AXI_DMA_IN_CHECK_OWNER_CH2    (BIT(12))
#define AXI_DMA_IN_CHECK_OWNER_CH2_M  (AXI_DMA_IN_CHECK_OWNER_CH2_V << AXI_DMA_IN_CHECK_OWNER_CH2_S)
#define AXI_DMA_IN_CHECK_OWNER_CH2_V  0x00000001U
#define AXI_DMA_IN_CHECK_OWNER_CH2_S  12

/** AXI_DMA_INFIFO_STATUS_CH2_REG register
 *  Receive FIFO status of Rx channel 2
 */
#define AXI_DMA_INFIFO_STATUS_CH2_REG (DR_REG_AXI_DMA_BASE + 0xe8)
/** AXI_DMA_INFIFO_L3_FULL_CH2 : RO; bitpos: [0]; default: 1;
 *  L3 Rx FIFO full signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L3_FULL_CH2    (BIT(0))
#define AXI_DMA_INFIFO_L3_FULL_CH2_M  (AXI_DMA_INFIFO_L3_FULL_CH2_V << AXI_DMA_INFIFO_L3_FULL_CH2_S)
#define AXI_DMA_INFIFO_L3_FULL_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L3_FULL_CH2_S  0
/** AXI_DMA_INFIFO_L3_EMPTY_CH2 : RO; bitpos: [1]; default: 1;
 *  L3 Rx FIFO empty signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L3_EMPTY_CH2    (BIT(1))
#define AXI_DMA_INFIFO_L3_EMPTY_CH2_M  (AXI_DMA_INFIFO_L3_EMPTY_CH2_V << AXI_DMA_INFIFO_L3_EMPTY_CH2_S)
#define AXI_DMA_INFIFO_L3_EMPTY_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L3_EMPTY_CH2_S  1
/** AXI_DMA_INFIFO_L3_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L3 Rx FIFO for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L3_CNT_CH2    0x0000003FU
#define AXI_DMA_INFIFO_L3_CNT_CH2_M  (AXI_DMA_INFIFO_L3_CNT_CH2_V << AXI_DMA_INFIFO_L3_CNT_CH2_S)
#define AXI_DMA_INFIFO_L3_CNT_CH2_V  0x0000003FU
#define AXI_DMA_INFIFO_L3_CNT_CH2_S  2
/** AXI_DMA_INFIFO_L3_UDF_CH2 : RO; bitpos: [8]; default: 0;
 *  L3 Rx FIFO under flow signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L3_UDF_CH2    (BIT(8))
#define AXI_DMA_INFIFO_L3_UDF_CH2_M  (AXI_DMA_INFIFO_L3_UDF_CH2_V << AXI_DMA_INFIFO_L3_UDF_CH2_S)
#define AXI_DMA_INFIFO_L3_UDF_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L3_UDF_CH2_S  8
/** AXI_DMA_INFIFO_L3_OVF_CH2 : RO; bitpos: [9]; default: 0;
 *  L3 Rx FIFO over flow signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L3_OVF_CH2    (BIT(9))
#define AXI_DMA_INFIFO_L3_OVF_CH2_M  (AXI_DMA_INFIFO_L3_OVF_CH2_V << AXI_DMA_INFIFO_L3_OVF_CH2_S)
#define AXI_DMA_INFIFO_L3_OVF_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L3_OVF_CH2_S  9
/** AXI_DMA_INFIFO_L1_FULL_CH2 : RO; bitpos: [10]; default: 0;
 *  L1 Rx FIFO full signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L1_FULL_CH2    (BIT(10))
#define AXI_DMA_INFIFO_L1_FULL_CH2_M  (AXI_DMA_INFIFO_L1_FULL_CH2_V << AXI_DMA_INFIFO_L1_FULL_CH2_S)
#define AXI_DMA_INFIFO_L1_FULL_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L1_FULL_CH2_S  10
/** AXI_DMA_INFIFO_L1_EMPTY_CH2 : RO; bitpos: [11]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L1_EMPTY_CH2    (BIT(11))
#define AXI_DMA_INFIFO_L1_EMPTY_CH2_M  (AXI_DMA_INFIFO_L1_EMPTY_CH2_V << AXI_DMA_INFIFO_L1_EMPTY_CH2_S)
#define AXI_DMA_INFIFO_L1_EMPTY_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L1_EMPTY_CH2_S  11
/** AXI_DMA_INFIFO_L1_UDF_CH2 : RO; bitpos: [12]; default: 0;
 *  L1 Rx FIFO under flow signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L1_UDF_CH2    (BIT(12))
#define AXI_DMA_INFIFO_L1_UDF_CH2_M  (AXI_DMA_INFIFO_L1_UDF_CH2_V << AXI_DMA_INFIFO_L1_UDF_CH2_S)
#define AXI_DMA_INFIFO_L1_UDF_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L1_UDF_CH2_S  12
/** AXI_DMA_INFIFO_L1_OVF_CH2 : RO; bitpos: [13]; default: 0;
 *  L1 Rx FIFO over flow signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L1_OVF_CH2    (BIT(13))
#define AXI_DMA_INFIFO_L1_OVF_CH2_M  (AXI_DMA_INFIFO_L1_OVF_CH2_V << AXI_DMA_INFIFO_L1_OVF_CH2_S)
#define AXI_DMA_INFIFO_L1_OVF_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L1_OVF_CH2_S  13
/** AXI_DMA_INFIFO_L2_FULL_CH2 : RO; bitpos: [14]; default: 0;
 *  L2 Rx RAM full signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L2_FULL_CH2    (BIT(14))
#define AXI_DMA_INFIFO_L2_FULL_CH2_M  (AXI_DMA_INFIFO_L2_FULL_CH2_V << AXI_DMA_INFIFO_L2_FULL_CH2_S)
#define AXI_DMA_INFIFO_L2_FULL_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L2_FULL_CH2_S  14
/** AXI_DMA_INFIFO_L2_EMPTY_CH2 : RO; bitpos: [15]; default: 1;
 *  L2 Rx RAM empty signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L2_EMPTY_CH2    (BIT(15))
#define AXI_DMA_INFIFO_L2_EMPTY_CH2_M  (AXI_DMA_INFIFO_L2_EMPTY_CH2_V << AXI_DMA_INFIFO_L2_EMPTY_CH2_S)
#define AXI_DMA_INFIFO_L2_EMPTY_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L2_EMPTY_CH2_S  15
/** AXI_DMA_INFIFO_L2_UDF_CH2 : RO; bitpos: [16]; default: 0;
 *  L2 Rx FIFO under flow signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L2_UDF_CH2    (BIT(16))
#define AXI_DMA_INFIFO_L2_UDF_CH2_M  (AXI_DMA_INFIFO_L2_UDF_CH2_V << AXI_DMA_INFIFO_L2_UDF_CH2_S)
#define AXI_DMA_INFIFO_L2_UDF_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L2_UDF_CH2_S  16
/** AXI_DMA_INFIFO_L2_OVF_CH2 : RO; bitpos: [17]; default: 0;
 *  L2 Rx FIFO over flow signal for Rx channel 2.
 */
#define AXI_DMA_INFIFO_L2_OVF_CH2    (BIT(17))
#define AXI_DMA_INFIFO_L2_OVF_CH2_M  (AXI_DMA_INFIFO_L2_OVF_CH2_V << AXI_DMA_INFIFO_L2_OVF_CH2_S)
#define AXI_DMA_INFIFO_L2_OVF_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_L2_OVF_CH2_S  17
/** AXI_DMA_IN_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH2    (BIT(23))
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_1B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_1B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_1B_CH2_S  23
/** AXI_DMA_IN_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH2    (BIT(24))
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_2B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_2B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_2B_CH2_S  24
/** AXI_DMA_IN_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH2    (BIT(25))
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_3B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_3B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_3B_CH2_S  25
/** AXI_DMA_IN_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH2    (BIT(26))
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_4B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_4B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_4B_CH2_S  26
/** AXI_DMA_IN_REMAIN_UNDER_5B_CH2 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH2    (BIT(27))
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_5B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_5B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_5B_CH2_S  27
/** AXI_DMA_IN_REMAIN_UNDER_6B_CH2 : RO; bitpos: [28]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH2    (BIT(28))
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_6B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_6B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_6B_CH2_S  28
/** AXI_DMA_IN_REMAIN_UNDER_7B_CH2 : RO; bitpos: [29]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH2    (BIT(29))
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_7B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_7B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_7B_CH2_S  29
/** AXI_DMA_IN_REMAIN_UNDER_8B_CH2 : RO; bitpos: [30]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH2    (BIT(30))
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH2_M  (AXI_DMA_IN_REMAIN_UNDER_8B_CH2_V << AXI_DMA_IN_REMAIN_UNDER_8B_CH2_S)
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH2_V  0x00000001U
#define AXI_DMA_IN_REMAIN_UNDER_8B_CH2_S  30
/** AXI_DMA_IN_BUF_HUNGRY_CH2 : RO; bitpos: [31]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_BUF_HUNGRY_CH2    (BIT(31))
#define AXI_DMA_IN_BUF_HUNGRY_CH2_M  (AXI_DMA_IN_BUF_HUNGRY_CH2_V << AXI_DMA_IN_BUF_HUNGRY_CH2_S)
#define AXI_DMA_IN_BUF_HUNGRY_CH2_V  0x00000001U
#define AXI_DMA_IN_BUF_HUNGRY_CH2_S  31

/** AXI_DMA_IN_POP_CH2_REG register
 *  Pop control register of Rx channel 2
 */
#define AXI_DMA_IN_POP_CH2_REG (DR_REG_AXI_DMA_BASE + 0xec)
/** AXI_DMA_INFIFO_RDATA_CH2 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from AXI_DMA FIFO.
 */
#define AXI_DMA_INFIFO_RDATA_CH2    0x00000FFFU
#define AXI_DMA_INFIFO_RDATA_CH2_M  (AXI_DMA_INFIFO_RDATA_CH2_V << AXI_DMA_INFIFO_RDATA_CH2_S)
#define AXI_DMA_INFIFO_RDATA_CH2_V  0x00000FFFU
#define AXI_DMA_INFIFO_RDATA_CH2_S  0
/** AXI_DMA_INFIFO_POP_CH2 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from AXI_DMA FIFO.
 */
#define AXI_DMA_INFIFO_POP_CH2    (BIT(12))
#define AXI_DMA_INFIFO_POP_CH2_M  (AXI_DMA_INFIFO_POP_CH2_V << AXI_DMA_INFIFO_POP_CH2_S)
#define AXI_DMA_INFIFO_POP_CH2_V  0x00000001U
#define AXI_DMA_INFIFO_POP_CH2_S  12

/** AXI_DMA_IN_LINK1_CH2_REG register
 *  Link descriptor configure and control register of Rx channel 2
 */
#define AXI_DMA_IN_LINK1_CH2_REG (DR_REG_AXI_DMA_BASE + 0xf0)
/** AXI_DMA_INLINK_AUTO_RET_CH2 : R/W; bitpos: [0]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define AXI_DMA_INLINK_AUTO_RET_CH2    (BIT(0))
#define AXI_DMA_INLINK_AUTO_RET_CH2_M  (AXI_DMA_INLINK_AUTO_RET_CH2_V << AXI_DMA_INLINK_AUTO_RET_CH2_S)
#define AXI_DMA_INLINK_AUTO_RET_CH2_V  0x00000001U
#define AXI_DMA_INLINK_AUTO_RET_CH2_S  0
/** AXI_DMA_INLINK_STOP_CH2 : WT; bitpos: [1]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define AXI_DMA_INLINK_STOP_CH2    (BIT(1))
#define AXI_DMA_INLINK_STOP_CH2_M  (AXI_DMA_INLINK_STOP_CH2_V << AXI_DMA_INLINK_STOP_CH2_S)
#define AXI_DMA_INLINK_STOP_CH2_V  0x00000001U
#define AXI_DMA_INLINK_STOP_CH2_S  1
/** AXI_DMA_INLINK_START_CH2 : WT; bitpos: [2]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define AXI_DMA_INLINK_START_CH2    (BIT(2))
#define AXI_DMA_INLINK_START_CH2_M  (AXI_DMA_INLINK_START_CH2_V << AXI_DMA_INLINK_START_CH2_S)
#define AXI_DMA_INLINK_START_CH2_V  0x00000001U
#define AXI_DMA_INLINK_START_CH2_S  2
/** AXI_DMA_INLINK_RESTART_CH2 : WT; bitpos: [3]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define AXI_DMA_INLINK_RESTART_CH2    (BIT(3))
#define AXI_DMA_INLINK_RESTART_CH2_M  (AXI_DMA_INLINK_RESTART_CH2_V << AXI_DMA_INLINK_RESTART_CH2_S)
#define AXI_DMA_INLINK_RESTART_CH2_V  0x00000001U
#define AXI_DMA_INLINK_RESTART_CH2_S  3
/** AXI_DMA_INLINK_PARK_CH2 : RO; bitpos: [4]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define AXI_DMA_INLINK_PARK_CH2    (BIT(4))
#define AXI_DMA_INLINK_PARK_CH2_M  (AXI_DMA_INLINK_PARK_CH2_V << AXI_DMA_INLINK_PARK_CH2_S)
#define AXI_DMA_INLINK_PARK_CH2_V  0x00000001U
#define AXI_DMA_INLINK_PARK_CH2_S  4

/** AXI_DMA_IN_LINK2_CH2_REG register
 *  Link descriptor configure and control register of Rx channel 2
 */
#define AXI_DMA_IN_LINK2_CH2_REG (DR_REG_AXI_DMA_BASE + 0xf4)
/** AXI_DMA_INLINK_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 20 least significant bits of the first inlink descriptor's
 *  address.
 */
#define AXI_DMA_INLINK_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_INLINK_ADDR_CH2_M  (AXI_DMA_INLINK_ADDR_CH2_V << AXI_DMA_INLINK_ADDR_CH2_S)
#define AXI_DMA_INLINK_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_ADDR_CH2_S  0

/** AXI_DMA_IN_STATE_CH2_REG register
 *  Receive status of Rx channel 2
 */
#define AXI_DMA_IN_STATE_CH2_REG (DR_REG_AXI_DMA_BASE + 0xf8)
/** AXI_DMA_INLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define AXI_DMA_INLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define AXI_DMA_INLINK_DSCR_ADDR_CH2_M  (AXI_DMA_INLINK_DSCR_ADDR_CH2_V << AXI_DMA_INLINK_DSCR_ADDR_CH2_S)
#define AXI_DMA_INLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define AXI_DMA_INLINK_DSCR_ADDR_CH2_S  0
/** AXI_DMA_IN_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_DSCR_STATE_CH2    0x00000003U
#define AXI_DMA_IN_DSCR_STATE_CH2_M  (AXI_DMA_IN_DSCR_STATE_CH2_V << AXI_DMA_IN_DSCR_STATE_CH2_S)
#define AXI_DMA_IN_DSCR_STATE_CH2_V  0x00000003U
#define AXI_DMA_IN_DSCR_STATE_CH2_S  18
/** AXI_DMA_IN_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AXI_DMA_IN_STATE_CH2    0x00000007U
#define AXI_DMA_IN_STATE_CH2_M  (AXI_DMA_IN_STATE_CH2_V << AXI_DMA_IN_STATE_CH2_S)
#define AXI_DMA_IN_STATE_CH2_V  0x00000007U
#define AXI_DMA_IN_STATE_CH2_S  20

/** AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 2
 */
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0xfc)
/** AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2_M  (AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2_V << AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2_S)
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_IN_SUC_EOF_DES_ADDR_CH2_S  0

/** AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when errors occur of Rx channel 2
 */
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x100)
/** AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2_M  (AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2_V << AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2_S)
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_IN_ERR_EOF_DES_ADDR_CH2_S  0

/** AXI_DMA_IN_DSCR_CH2_REG register
 *  Current inlink descriptor address of Rx channel 2
 */
#define AXI_DMA_IN_DSCR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x104)
/** AXI_DMA_INLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define AXI_DMA_INLINK_DSCR_CH2    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_CH2_M  (AXI_DMA_INLINK_DSCR_CH2_V << AXI_DMA_INLINK_DSCR_CH2_S)
#define AXI_DMA_INLINK_DSCR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_CH2_S  0

/** AXI_DMA_IN_DSCR_BF0_CH2_REG register
 *  The last inlink descriptor address of Rx channel 2
 */
#define AXI_DMA_IN_DSCR_BF0_CH2_REG (DR_REG_AXI_DMA_BASE + 0x108)
/** AXI_DMA_INLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define AXI_DMA_INLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF0_CH2_M  (AXI_DMA_INLINK_DSCR_BF0_CH2_V << AXI_DMA_INLINK_DSCR_BF0_CH2_S)
#define AXI_DMA_INLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF0_CH2_S  0

/** AXI_DMA_IN_DSCR_BF1_CH2_REG register
 *  The second-to-last inlink descriptor address of Rx channel 2
 */
#define AXI_DMA_IN_DSCR_BF1_CH2_REG (DR_REG_AXI_DMA_BASE + 0x10c)
/** AXI_DMA_INLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AXI_DMA_INLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF1_CH2_M  (AXI_DMA_INLINK_DSCR_BF1_CH2_V << AXI_DMA_INLINK_DSCR_BF1_CH2_S)
#define AXI_DMA_INLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define AXI_DMA_INLINK_DSCR_BF1_CH2_S  0

/** AXI_DMA_IN_PRI_CH2_REG register
 *  Priority register of Rx channel 2
 */
#define AXI_DMA_IN_PRI_CH2_REG (DR_REG_AXI_DMA_BASE + 0x110)
/** AXI_DMA_RX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 2. The larger of the value the higher of the priority.
 */
#define AXI_DMA_RX_PRI_CH2    0x0000000FU
#define AXI_DMA_RX_PRI_CH2_M  (AXI_DMA_RX_PRI_CH2_V << AXI_DMA_RX_PRI_CH2_S)
#define AXI_DMA_RX_PRI_CH2_V  0x0000000FU
#define AXI_DMA_RX_PRI_CH2_S  0
/** AXI_DMA_RX_CH_ARB_WEIGH_CH2 : R/W; bitpos: [7:4]; default: 0;
 *  The weight of Rx channel 2
 */
#define AXI_DMA_RX_CH_ARB_WEIGH_CH2    0x0000000FU
#define AXI_DMA_RX_CH_ARB_WEIGH_CH2_M  (AXI_DMA_RX_CH_ARB_WEIGH_CH2_V << AXI_DMA_RX_CH_ARB_WEIGH_CH2_S)
#define AXI_DMA_RX_CH_ARB_WEIGH_CH2_V  0x0000000FU
#define AXI_DMA_RX_CH_ARB_WEIGH_CH2_S  4
/** AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH2 : R/W; bitpos: [8]; default: 0;
 *  0: mean not optimization weight function ,1: mean optimization
 */
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH2    (BIT(8))
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_M  (AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_V << AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_S)
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_V  0x00000001U
#define AXI_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_S  8

/** AXI_DMA_IN_PERI_SEL_CH2_REG register
 *  Peripheral selection of Rx channel 2
 */
#define AXI_DMA_IN_PERI_SEL_CH2_REG (DR_REG_AXI_DMA_BASE + 0x114)
/** AXI_DMA_PERI_IN_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 2. 0:lcdcam. 1: gpspi_2.
 *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
 */
#define AXI_DMA_PERI_IN_SEL_CH2    0x0000003FU
#define AXI_DMA_PERI_IN_SEL_CH2_M  (AXI_DMA_PERI_IN_SEL_CH2_V << AXI_DMA_PERI_IN_SEL_CH2_S)
#define AXI_DMA_PERI_IN_SEL_CH2_V  0x0000003FU
#define AXI_DMA_PERI_IN_SEL_CH2_S  0

/** AXI_DMA_IN_CRC_INIT_DATA_CH2_REG register
 *  This register is used to config ch2 crc initial data(max 32 bit)
 */
#define AXI_DMA_IN_CRC_INIT_DATA_CH2_REG (DR_REG_AXI_DMA_BASE + 0x118)
/** AXI_DMA_IN_CRC_INIT_DATA_CH2 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch2 of rx crc initial value
 */
#define AXI_DMA_IN_CRC_INIT_DATA_CH2    0xFFFFFFFFU
#define AXI_DMA_IN_CRC_INIT_DATA_CH2_M  (AXI_DMA_IN_CRC_INIT_DATA_CH2_V << AXI_DMA_IN_CRC_INIT_DATA_CH2_S)
#define AXI_DMA_IN_CRC_INIT_DATA_CH2_V  0xFFFFFFFFU
#define AXI_DMA_IN_CRC_INIT_DATA_CH2_S  0

/** AXI_DMA_RX_CRC_WIDTH_CH2_REG register
 *  This register is used to config rx ch2 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AXI_DMA_RX_CRC_WIDTH_CH2_REG (DR_REG_AXI_DMA_BASE + 0x11c)
/** AXI_DMA_RX_CRC_WIDTH_CH2 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_WIDTH_CH2    0x00000003U
#define AXI_DMA_RX_CRC_WIDTH_CH2_M  (AXI_DMA_RX_CRC_WIDTH_CH2_V << AXI_DMA_RX_CRC_WIDTH_CH2_S)
#define AXI_DMA_RX_CRC_WIDTH_CH2_V  0x00000003U
#define AXI_DMA_RX_CRC_WIDTH_CH2_S  0
/** AXI_DMA_RX_CRC_LATCH_FLAG_CH2 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH2    (BIT(2))
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH2_M  (AXI_DMA_RX_CRC_LATCH_FLAG_CH2_V << AXI_DMA_RX_CRC_LATCH_FLAG_CH2_S)
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH2_V  0x00000001U
#define AXI_DMA_RX_CRC_LATCH_FLAG_CH2_S  2

/** AXI_DMA_IN_CRC_CLEAR_CH2_REG register
 *  This register is used to clear ch2 crc result
 */
#define AXI_DMA_IN_CRC_CLEAR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x120)
/** AXI_DMA_IN_CRC_CLEAR_CH2 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch2 of rx crc result
 */
#define AXI_DMA_IN_CRC_CLEAR_CH2    (BIT(0))
#define AXI_DMA_IN_CRC_CLEAR_CH2_M  (AXI_DMA_IN_CRC_CLEAR_CH2_V << AXI_DMA_IN_CRC_CLEAR_CH2_S)
#define AXI_DMA_IN_CRC_CLEAR_CH2_V  0x00000001U
#define AXI_DMA_IN_CRC_CLEAR_CH2_S  0

/** AXI_DMA_IN_CRC_FINAL_RESULT_CH2_REG register
 *  This register is used to store ch2 crc result
 */
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH2_REG (DR_REG_AXI_DMA_BASE + 0x124)
/** AXI_DMA_IN_CRC_FINAL_RESULT_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch2 of rx
 */
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH2    0xFFFFFFFFU
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH2_M  (AXI_DMA_IN_CRC_FINAL_RESULT_CH2_V << AXI_DMA_IN_CRC_FINAL_RESULT_CH2_S)
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH2_V  0xFFFFFFFFU
#define AXI_DMA_IN_CRC_FINAL_RESULT_CH2_S  0

/** AXI_DMA_RX_CRC_EN_WR_DATA_CH2_REG register
 *  This resister is used to config ch2 crc en for every bit
 */
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH2_REG (DR_REG_AXI_DMA_BASE + 0x128)
/** AXI_DMA_RX_CRC_EN_WR_DATA_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable rx ch2 crc 32bit on/off
 */
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH2    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH2_M  (AXI_DMA_RX_CRC_EN_WR_DATA_CH2_V << AXI_DMA_RX_CRC_EN_WR_DATA_CH2_S)
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH2_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_WR_DATA_CH2_S  0

/** AXI_DMA_RX_CRC_EN_ADDR_CH2_REG register
 *  This register is used to config ch2 crc en addr
 */
#define AXI_DMA_RX_CRC_EN_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x12c)
/** AXI_DMA_RX_CRC_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_EN_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_ADDR_CH2_M  (AXI_DMA_RX_CRC_EN_ADDR_CH2_V << AXI_DMA_RX_CRC_EN_ADDR_CH2_S)
#define AXI_DMA_RX_CRC_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_EN_ADDR_CH2_S  0

/** AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_REG register
 *  This register is used to config crc data_8bit en
 */
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_REG (DR_REG_AXI_DMA_BASE + 0x130)
/** AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2 : R/W; bitpos: [15:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2    0x0000FFFFU
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_M  (AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_V << AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_S)
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_V  0x0000FFFFU
#define AXI_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_S  0

/** AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x134)
/** AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2_M  (AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2_V << AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2_S)
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_RX_CRC_DATA_EN_ADDR_CH2_S  0

/** AXI_DMA_OUT_INT_RAW_CH0_REG register
 *  Raw status interrupt of channel0
 */
#define AXI_DMA_OUT_INT_RAW_CH0_REG (DR_REG_AXI_DMA_BASE + 0x138)
/** AXI_DMA_OUT_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel0.
 */
#define AXI_DMA_OUT_DONE_CH0_INT_RAW    (BIT(0))
#define AXI_DMA_OUT_DONE_CH0_INT_RAW_M  (AXI_DMA_OUT_DONE_CH0_INT_RAW_V << AXI_DMA_OUT_DONE_CH0_INT_RAW_S)
#define AXI_DMA_OUT_DONE_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH0_INT_RAW_S  0
/** AXI_DMA_OUT_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel0.
 */
#define AXI_DMA_OUT_EOF_CH0_INT_RAW    (BIT(1))
#define AXI_DMA_OUT_EOF_CH0_INT_RAW_M  (AXI_DMA_OUT_EOF_CH0_INT_RAW_V << AXI_DMA_OUT_EOF_CH0_INT_RAW_S)
#define AXI_DMA_OUT_EOF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH0_INT_RAW_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel0.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_RAW    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_RAW_M  (AXI_DMA_OUT_DSCR_ERR_CH0_INT_RAW_V << AXI_DMA_OUT_DSCR_ERR_CH0_INT_RAW_S)
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_RAW_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel0.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_RAW    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_M  (AXI_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_V << AXI_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_RAW    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_RAW_M  (AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_RAW_V << AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_RAW_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_RAW    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_RAW_M  (AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_RAW_V << AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_RAW_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_RAW    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_RAW_M  (AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_RAW_V << AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_RAW_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_RAW    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_RAW_M  (AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_RAW_V << AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_RAW_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_RAW    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_RAW_M  (AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_RAW_V << AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_RAW_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_RAW    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_RAW_M  (AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_RAW_V << AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_RAW_S  9

/** AXI_DMA_OUT_INT_ST_CH0_REG register
 *  Masked interrupt of channel0
 */
#define AXI_DMA_OUT_INT_ST_CH0_REG (DR_REG_AXI_DMA_BASE + 0x13c)
/** AXI_DMA_OUT_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH0_INT_ST    (BIT(0))
#define AXI_DMA_OUT_DONE_CH0_INT_ST_M  (AXI_DMA_OUT_DONE_CH0_INT_ST_V << AXI_DMA_OUT_DONE_CH0_INT_ST_S)
#define AXI_DMA_OUT_DONE_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH0_INT_ST_S  0
/** AXI_DMA_OUT_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH0_INT_ST    (BIT(1))
#define AXI_DMA_OUT_EOF_CH0_INT_ST_M  (AXI_DMA_OUT_EOF_CH0_INT_ST_V << AXI_DMA_OUT_EOF_CH0_INT_ST_S)
#define AXI_DMA_OUT_EOF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH0_INT_ST_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ST    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ST_M  (AXI_DMA_OUT_DSCR_ERR_CH0_INT_ST_V << AXI_DMA_OUT_DSCR_ERR_CH0_INT_ST_S)
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ST_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ST    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ST_M  (AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ST_V << AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ST_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ST_S  3
/** AXI_DMA_OUTFIFO_OVF_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_OVF_CH0_INT_ST    (BIT(4))
#define AXI_DMA_OUTFIFO_OVF_CH0_INT_ST_M  (AXI_DMA_OUTFIFO_OVF_CH0_INT_ST_V << AXI_DMA_OUTFIFO_OVF_CH0_INT_ST_S)
#define AXI_DMA_OUTFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_OVF_CH0_INT_ST_S  4
/** AXI_DMA_OUTFIFO_UDF_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_UDF_CH0_INT_ST    (BIT(5))
#define AXI_DMA_OUTFIFO_UDF_CH0_INT_ST_M  (AXI_DMA_OUTFIFO_UDF_CH0_INT_ST_V << AXI_DMA_OUTFIFO_UDF_CH0_INT_ST_S)
#define AXI_DMA_OUTFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_UDF_CH0_INT_ST_S  5
/** AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ST    (BIT(6))
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ST_M  (AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ST_V << AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ST_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ST_S  6
/** AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ST    (BIT(7))
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ST_M  (AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ST_V << AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ST_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ST_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ST    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ST_M  (AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ST_V << AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ST_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ST_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ST    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ST_M  (AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ST_V << AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ST_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ST_S  9

/** AXI_DMA_OUT_INT_ENA_CH0_REG register
 *  Interrupt enable bits of channel0
 */
#define AXI_DMA_OUT_INT_ENA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x140)
/** AXI_DMA_OUT_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH0_INT_ENA    (BIT(0))
#define AXI_DMA_OUT_DONE_CH0_INT_ENA_M  (AXI_DMA_OUT_DONE_CH0_INT_ENA_V << AXI_DMA_OUT_DONE_CH0_INT_ENA_S)
#define AXI_DMA_OUT_DONE_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH0_INT_ENA_S  0
/** AXI_DMA_OUT_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH0_INT_ENA    (BIT(1))
#define AXI_DMA_OUT_EOF_CH0_INT_ENA_M  (AXI_DMA_OUT_EOF_CH0_INT_ENA_V << AXI_DMA_OUT_EOF_CH0_INT_ENA_S)
#define AXI_DMA_OUT_EOF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH0_INT_ENA_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ENA    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ENA_M  (AXI_DMA_OUT_DSCR_ERR_CH0_INT_ENA_V << AXI_DMA_OUT_DSCR_ERR_CH0_INT_ENA_S)
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_ENA_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ENA    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_M  (AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_V << AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ENA    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ENA_M  (AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ENA_V << AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_ENA_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ENA    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ENA_M  (AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ENA_V << AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_ENA_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_ENA    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_ENA_M  (AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_ENA_V << AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_ENA_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_ENA    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_ENA_M  (AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_ENA_V << AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_ENA_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ENA    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ENA_M  (AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ENA_V << AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_ENA_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ENA    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ENA_M  (AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ENA_V << AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_ENA_S  9

/** AXI_DMA_OUT_INT_CLR_CH0_REG register
 *  Interrupt clear bits of channel0
 */
#define AXI_DMA_OUT_INT_CLR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x144)
/** AXI_DMA_OUT_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH0_INT_CLR    (BIT(0))
#define AXI_DMA_OUT_DONE_CH0_INT_CLR_M  (AXI_DMA_OUT_DONE_CH0_INT_CLR_V << AXI_DMA_OUT_DONE_CH0_INT_CLR_S)
#define AXI_DMA_OUT_DONE_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH0_INT_CLR_S  0
/** AXI_DMA_OUT_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH0_INT_CLR    (BIT(1))
#define AXI_DMA_OUT_EOF_CH0_INT_CLR_M  (AXI_DMA_OUT_EOF_CH0_INT_CLR_V << AXI_DMA_OUT_EOF_CH0_INT_CLR_S)
#define AXI_DMA_OUT_EOF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH0_INT_CLR_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_CLR    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_CLR_M  (AXI_DMA_OUT_DSCR_ERR_CH0_INT_CLR_V << AXI_DMA_OUT_DSCR_ERR_CH0_INT_CLR_S)
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH0_INT_CLR_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_CLR    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_M  (AXI_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_V << AXI_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_CLR    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_CLR_M  (AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_CLR_V << AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_INT_CLR_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_CLR    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_CLR_M  (AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_CLR_V << AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_INT_CLR_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_CLR    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_CLR_M  (AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_CLR_V << AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_INT_CLR_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_CLR    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_CLR_M  (AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_CLR_V << AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_INT_CLR_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_CLR    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_CLR_M  (AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_CLR_V << AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_INT_CLR_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_CLR    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_CLR_M  (AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_CLR_V << AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_INT_CLR_S  9

/** AXI_DMA_OUT_CONF0_CH0_REG register
 *  Configure 0 register of Tx channel0
 */
#define AXI_DMA_OUT_CONF0_CH0_REG (DR_REG_AXI_DMA_BASE + 0x148)
/** AXI_DMA_OUT_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AXI_DMA channel0 Tx FSM and Tx FIFO pointer.
 */
#define AXI_DMA_OUT_RST_CH0    (BIT(0))
#define AXI_DMA_OUT_RST_CH0_M  (AXI_DMA_OUT_RST_CH0_V << AXI_DMA_OUT_RST_CH0_S)
#define AXI_DMA_OUT_RST_CH0_V  0x00000001U
#define AXI_DMA_OUT_RST_CH0_S  0
/** AXI_DMA_OUT_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_LOOP_TEST_CH0    (BIT(1))
#define AXI_DMA_OUT_LOOP_TEST_CH0_M  (AXI_DMA_OUT_LOOP_TEST_CH0_V << AXI_DMA_OUT_LOOP_TEST_CH0_S)
#define AXI_DMA_OUT_LOOP_TEST_CH0_V  0x00000001U
#define AXI_DMA_OUT_LOOP_TEST_CH0_S  1
/** AXI_DMA_OUT_AUTO_WRBACK_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define AXI_DMA_OUT_AUTO_WRBACK_CH0    (BIT(2))
#define AXI_DMA_OUT_AUTO_WRBACK_CH0_M  (AXI_DMA_OUT_AUTO_WRBACK_CH0_V << AXI_DMA_OUT_AUTO_WRBACK_CH0_S)
#define AXI_DMA_OUT_AUTO_WRBACK_CH0_V  0x00000001U
#define AXI_DMA_OUT_AUTO_WRBACK_CH0_S  2
/** AXI_DMA_OUT_EOF_MODE_CH0 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel0 is
 *  generated when data need to transmit has been popped from FIFO in AXI_DMA
 */
#define AXI_DMA_OUT_EOF_MODE_CH0    (BIT(3))
#define AXI_DMA_OUT_EOF_MODE_CH0_M  (AXI_DMA_OUT_EOF_MODE_CH0_V << AXI_DMA_OUT_EOF_MODE_CH0_S)
#define AXI_DMA_OUT_EOF_MODE_CH0_V  0x00000001U
#define AXI_DMA_OUT_EOF_MODE_CH0_S  3
/** AXI_DMA_OUT_ETM_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel0 is triggered by etm
 *  task.
 */
#define AXI_DMA_OUT_ETM_EN_CH0    (BIT(4))
#define AXI_DMA_OUT_ETM_EN_CH0_M  (AXI_DMA_OUT_ETM_EN_CH0_V << AXI_DMA_OUT_ETM_EN_CH0_S)
#define AXI_DMA_OUT_ETM_EN_CH0_V  0x00000001U
#define AXI_DMA_OUT_ETM_EN_CH0_S  4
/** AXI_DMA_OUT_BURST_SIZE_SEL_CH0 : R/W; bitpos: [7:5]; default: 0;
 *  3'b000-3'b100:burst length 8byte~128byte
 */
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH0    0x00000007U
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH0_M  (AXI_DMA_OUT_BURST_SIZE_SEL_CH0_V << AXI_DMA_OUT_BURST_SIZE_SEL_CH0_S)
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH0_V  0x00000007U
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH0_S  5
/** AXI_DMA_OUT_CMD_DISABLE_CH0 : R/W; bitpos: [8]; default: 0;
 *  1:mean disable cmd of this ch0
 */
#define AXI_DMA_OUT_CMD_DISABLE_CH0    (BIT(8))
#define AXI_DMA_OUT_CMD_DISABLE_CH0_M  (AXI_DMA_OUT_CMD_DISABLE_CH0_V << AXI_DMA_OUT_CMD_DISABLE_CH0_S)
#define AXI_DMA_OUT_CMD_DISABLE_CH0_V  0x00000001U
#define AXI_DMA_OUT_CMD_DISABLE_CH0_S  8
/** AXI_DMA_OUT_ECC_AEC_EN_CH0 : R/W; bitpos: [9]; default: 0;
 *  1: mean access ecc or aes domain,0: mean not
 */
#define AXI_DMA_OUT_ECC_AEC_EN_CH0    (BIT(9))
#define AXI_DMA_OUT_ECC_AEC_EN_CH0_M  (AXI_DMA_OUT_ECC_AEC_EN_CH0_V << AXI_DMA_OUT_ECC_AEC_EN_CH0_S)
#define AXI_DMA_OUT_ECC_AEC_EN_CH0_V  0x00000001U
#define AXI_DMA_OUT_ECC_AEC_EN_CH0_S  9
/** AXI_DMA_OUTDSCR_BURST_EN_CH0 : R/W; bitpos: [10]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AXI_DMA_OUTDSCR_BURST_EN_CH0    (BIT(10))
#define AXI_DMA_OUTDSCR_BURST_EN_CH0_M  (AXI_DMA_OUTDSCR_BURST_EN_CH0_V << AXI_DMA_OUTDSCR_BURST_EN_CH0_S)
#define AXI_DMA_OUTDSCR_BURST_EN_CH0_V  0x00000001U
#define AXI_DMA_OUTDSCR_BURST_EN_CH0_S  10

/** AXI_DMA_OUT_CONF1_CH0_REG register
 *  Configure 1 register of Tx channel0
 */
#define AXI_DMA_OUT_CONF1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x14c)
/** AXI_DMA_OUT_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AXI_DMA_OUT_CHECK_OWNER_CH0    (BIT(12))
#define AXI_DMA_OUT_CHECK_OWNER_CH0_M  (AXI_DMA_OUT_CHECK_OWNER_CH0_V << AXI_DMA_OUT_CHECK_OWNER_CH0_S)
#define AXI_DMA_OUT_CHECK_OWNER_CH0_V  0x00000001U
#define AXI_DMA_OUT_CHECK_OWNER_CH0_S  12

/** AXI_DMA_OUTFIFO_STATUS_CH0_REG register
 *  Transmit FIFO status of Tx channel0
 */
#define AXI_DMA_OUTFIFO_STATUS_CH0_REG (DR_REG_AXI_DMA_BASE + 0x150)
/** AXI_DMA_OUTFIFO_L3_FULL_CH0 : RO; bitpos: [0]; default: 0;
 *  L3 Tx FIFO full signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L3_FULL_CH0    (BIT(0))
#define AXI_DMA_OUTFIFO_L3_FULL_CH0_M  (AXI_DMA_OUTFIFO_L3_FULL_CH0_V << AXI_DMA_OUTFIFO_L3_FULL_CH0_S)
#define AXI_DMA_OUTFIFO_L3_FULL_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_FULL_CH0_S  0
/** AXI_DMA_OUTFIFO_L3_EMPTY_CH0 : RO; bitpos: [1]; default: 1;
 *  L3 Tx FIFO empty signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH0    (BIT(1))
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH0_M  (AXI_DMA_OUTFIFO_L3_EMPTY_CH0_V << AXI_DMA_OUTFIFO_L3_EMPTY_CH0_S)
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH0_S  1
/** AXI_DMA_OUTFIFO_L3_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L3 Tx FIFO for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L3_CNT_CH0    0x0000003FU
#define AXI_DMA_OUTFIFO_L3_CNT_CH0_M  (AXI_DMA_OUTFIFO_L3_CNT_CH0_V << AXI_DMA_OUTFIFO_L3_CNT_CH0_S)
#define AXI_DMA_OUTFIFO_L3_CNT_CH0_V  0x0000003FU
#define AXI_DMA_OUTFIFO_L3_CNT_CH0_S  2
/** AXI_DMA_OUTFIFO_L3_UDF_CH0 : RO; bitpos: [8]; default: 0;
 *  L3 Tx FIFO under flow signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH0    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_M  (AXI_DMA_OUTFIFO_L3_UDF_CH0_V << AXI_DMA_OUTFIFO_L3_UDF_CH0_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH0_S  8
/** AXI_DMA_OUTFIFO_L3_OVF_CH0 : RO; bitpos: [9]; default: 0;
 *  L3 Tx FIFO over flow signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH0    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_M  (AXI_DMA_OUTFIFO_L3_OVF_CH0_V << AXI_DMA_OUTFIFO_L3_OVF_CH0_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH0_S  9
/** AXI_DMA_OUTFIFO_L1_FULL_CH0 : RO; bitpos: [10]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L1_FULL_CH0    (BIT(10))
#define AXI_DMA_OUTFIFO_L1_FULL_CH0_M  (AXI_DMA_OUTFIFO_L1_FULL_CH0_V << AXI_DMA_OUTFIFO_L1_FULL_CH0_S)
#define AXI_DMA_OUTFIFO_L1_FULL_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_FULL_CH0_S  10
/** AXI_DMA_OUTFIFO_L1_EMPTY_CH0 : RO; bitpos: [11]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH0    (BIT(11))
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH0_M  (AXI_DMA_OUTFIFO_L1_EMPTY_CH0_V << AXI_DMA_OUTFIFO_L1_EMPTY_CH0_S)
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH0_S  11
/** AXI_DMA_OUTFIFO_L1_UDF_CH0 : RO; bitpos: [12]; default: 0;
 *  L1 Tx FIFO under flow signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH0    (BIT(12))
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_M  (AXI_DMA_OUTFIFO_L1_UDF_CH0_V << AXI_DMA_OUTFIFO_L1_UDF_CH0_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH0_S  12
/** AXI_DMA_OUTFIFO_L1_OVF_CH0 : RO; bitpos: [13]; default: 0;
 *  L1 Tx FIFO over flow signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH0    (BIT(13))
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_M  (AXI_DMA_OUTFIFO_L1_OVF_CH0_V << AXI_DMA_OUTFIFO_L1_OVF_CH0_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH0_S  13
/** AXI_DMA_OUTFIFO_L2_FULL_CH0 : RO; bitpos: [14]; default: 0;
 *  L2 Tx RAM full signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L2_FULL_CH0    (BIT(14))
#define AXI_DMA_OUTFIFO_L2_FULL_CH0_M  (AXI_DMA_OUTFIFO_L2_FULL_CH0_V << AXI_DMA_OUTFIFO_L2_FULL_CH0_S)
#define AXI_DMA_OUTFIFO_L2_FULL_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_FULL_CH0_S  14
/** AXI_DMA_OUTFIFO_L2_EMPTY_CH0 : RO; bitpos: [15]; default: 1;
 *  L2 Tx RAM empty signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH0    (BIT(15))
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH0_M  (AXI_DMA_OUTFIFO_L2_EMPTY_CH0_V << AXI_DMA_OUTFIFO_L2_EMPTY_CH0_S)
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH0_S  15
/** AXI_DMA_OUTFIFO_L2_UDF_CH0 : RO; bitpos: [16]; default: 0;
 *  L2 Tx FIFO under flow signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH0    (BIT(16))
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_M  (AXI_DMA_OUTFIFO_L2_UDF_CH0_V << AXI_DMA_OUTFIFO_L2_UDF_CH0_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH0_S  16
/** AXI_DMA_OUTFIFO_L2_OVF_CH0 : RO; bitpos: [17]; default: 0;
 *  L2 Tx FIFO over flow signal for Tx channel0.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH0    (BIT(17))
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_M  (AXI_DMA_OUTFIFO_L2_OVF_CH0_V << AXI_DMA_OUTFIFO_L2_OVF_CH0_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH0_S  17
/** AXI_DMA_OUT_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH0    (BIT(23))
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_1B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_1B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH0_S  23
/** AXI_DMA_OUT_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH0    (BIT(24))
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_2B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_2B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH0_S  24
/** AXI_DMA_OUT_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH0    (BIT(25))
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_3B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_3B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH0_S  25
/** AXI_DMA_OUT_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH0    (BIT(26))
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_4B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_4B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH0_S  26
/** AXI_DMA_OUT_REMAIN_UNDER_5B_CH0 : RO; bitpos: [27]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH0    (BIT(27))
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_5B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_5B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH0_S  27
/** AXI_DMA_OUT_REMAIN_UNDER_6B_CH0 : RO; bitpos: [28]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH0    (BIT(28))
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_6B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_6B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH0_S  28
/** AXI_DMA_OUT_REMAIN_UNDER_7B_CH0 : RO; bitpos: [29]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH0    (BIT(29))
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_7B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_7B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH0_S  29
/** AXI_DMA_OUT_REMAIN_UNDER_8B_CH0 : RO; bitpos: [30]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH0    (BIT(30))
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH0_M  (AXI_DMA_OUT_REMAIN_UNDER_8B_CH0_V << AXI_DMA_OUT_REMAIN_UNDER_8B_CH0_S)
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH0_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH0_S  30

/** AXI_DMA_OUT_PUSH_CH0_REG register
 *  Push control register of Tx channel0
 */
#define AXI_DMA_OUT_PUSH_CH0_REG (DR_REG_AXI_DMA_BASE + 0x154)
/** AXI_DMA_OUTFIFO_WDATA_CH0 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into AXI_DMA FIFO.
 */
#define AXI_DMA_OUTFIFO_WDATA_CH0    0x000001FFU
#define AXI_DMA_OUTFIFO_WDATA_CH0_M  (AXI_DMA_OUTFIFO_WDATA_CH0_V << AXI_DMA_OUTFIFO_WDATA_CH0_S)
#define AXI_DMA_OUTFIFO_WDATA_CH0_V  0x000001FFU
#define AXI_DMA_OUTFIFO_WDATA_CH0_S  0
/** AXI_DMA_OUTFIFO_PUSH_CH0 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into AXI_DMA FIFO.
 */
#define AXI_DMA_OUTFIFO_PUSH_CH0    (BIT(9))
#define AXI_DMA_OUTFIFO_PUSH_CH0_M  (AXI_DMA_OUTFIFO_PUSH_CH0_V << AXI_DMA_OUTFIFO_PUSH_CH0_S)
#define AXI_DMA_OUTFIFO_PUSH_CH0_V  0x00000001U
#define AXI_DMA_OUTFIFO_PUSH_CH0_S  9

/** AXI_DMA_OUT_LINK1_CH0_REG register
 *  Link descriptor configure and control register of Tx channel0
 */
#define AXI_DMA_OUT_LINK1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x158)
/** AXI_DMA_OUTLINK_STOP_CH0 : WT; bitpos: [0]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define AXI_DMA_OUTLINK_STOP_CH0    (BIT(0))
#define AXI_DMA_OUTLINK_STOP_CH0_M  (AXI_DMA_OUTLINK_STOP_CH0_V << AXI_DMA_OUTLINK_STOP_CH0_S)
#define AXI_DMA_OUTLINK_STOP_CH0_V  0x00000001U
#define AXI_DMA_OUTLINK_STOP_CH0_S  0
/** AXI_DMA_OUTLINK_START_CH0 : WT; bitpos: [1]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define AXI_DMA_OUTLINK_START_CH0    (BIT(1))
#define AXI_DMA_OUTLINK_START_CH0_M  (AXI_DMA_OUTLINK_START_CH0_V << AXI_DMA_OUTLINK_START_CH0_S)
#define AXI_DMA_OUTLINK_START_CH0_V  0x00000001U
#define AXI_DMA_OUTLINK_START_CH0_S  1
/** AXI_DMA_OUTLINK_RESTART_CH0 : WT; bitpos: [2]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define AXI_DMA_OUTLINK_RESTART_CH0    (BIT(2))
#define AXI_DMA_OUTLINK_RESTART_CH0_M  (AXI_DMA_OUTLINK_RESTART_CH0_V << AXI_DMA_OUTLINK_RESTART_CH0_S)
#define AXI_DMA_OUTLINK_RESTART_CH0_V  0x00000001U
#define AXI_DMA_OUTLINK_RESTART_CH0_S  2
/** AXI_DMA_OUTLINK_PARK_CH0 : RO; bitpos: [3]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define AXI_DMA_OUTLINK_PARK_CH0    (BIT(3))
#define AXI_DMA_OUTLINK_PARK_CH0_M  (AXI_DMA_OUTLINK_PARK_CH0_V << AXI_DMA_OUTLINK_PARK_CH0_S)
#define AXI_DMA_OUTLINK_PARK_CH0_V  0x00000001U
#define AXI_DMA_OUTLINK_PARK_CH0_S  3

/** AXI_DMA_OUT_LINK2_CH0_REG register
 *  Link descriptor configure and control register of Tx channel0
 */
#define AXI_DMA_OUT_LINK2_CH0_REG (DR_REG_AXI_DMA_BASE + 0x15c)
/** AXI_DMA_OUTLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first outlink
 *  descriptor's address.
 */
#define AXI_DMA_OUTLINK_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_ADDR_CH0_M  (AXI_DMA_OUTLINK_ADDR_CH0_V << AXI_DMA_OUTLINK_ADDR_CH0_S)
#define AXI_DMA_OUTLINK_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_ADDR_CH0_S  0

/** AXI_DMA_OUT_STATE_CH0_REG register
 *  Transmit status of Tx channel0
 */
#define AXI_DMA_OUT_STATE_CH0_REG (DR_REG_AXI_DMA_BASE + 0x160)
/** AXI_DMA_OUTLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH0_M  (AXI_DMA_OUTLINK_DSCR_ADDR_CH0_V << AXI_DMA_OUTLINK_DSCR_ADDR_CH0_S)
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH0_S  0
/** AXI_DMA_OUT_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_DSCR_STATE_CH0    0x00000003U
#define AXI_DMA_OUT_DSCR_STATE_CH0_M  (AXI_DMA_OUT_DSCR_STATE_CH0_V << AXI_DMA_OUT_DSCR_STATE_CH0_S)
#define AXI_DMA_OUT_DSCR_STATE_CH0_V  0x00000003U
#define AXI_DMA_OUT_DSCR_STATE_CH0_S  18
/** AXI_DMA_OUT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_STATE_CH0    0x00000007U
#define AXI_DMA_OUT_STATE_CH0_M  (AXI_DMA_OUT_STATE_CH0_V << AXI_DMA_OUT_STATE_CH0_S)
#define AXI_DMA_OUT_STATE_CH0_V  0x00000007U
#define AXI_DMA_OUT_STATE_CH0_S  20

/** AXI_DMA_OUT_EOF_DES_ADDR_CH0_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel0
 */
#define AXI_DMA_OUT_EOF_DES_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x164)
/** AXI_DMA_OUT_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AXI_DMA_OUT_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_DES_ADDR_CH0_M  (AXI_DMA_OUT_EOF_DES_ADDR_CH0_V << AXI_DMA_OUT_EOF_DES_ADDR_CH0_S)
#define AXI_DMA_OUT_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_DES_ADDR_CH0_S  0

/** AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel0
 */
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x168)
/** AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0_M  (AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0_V << AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0_S)
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH0_S  0

/** AXI_DMA_OUT_DSCR_CH0_REG register
 *  Current outlink descriptor address of Tx channel0
 */
#define AXI_DMA_OUT_DSCR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x16c)
/** AXI_DMA_OUTLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define AXI_DMA_OUTLINK_DSCR_CH0    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_CH0_M  (AXI_DMA_OUTLINK_DSCR_CH0_V << AXI_DMA_OUTLINK_DSCR_CH0_S)
#define AXI_DMA_OUTLINK_DSCR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_CH0_S  0

/** AXI_DMA_OUT_DSCR_BF0_CH0_REG register
 *  The last outlink descriptor address of Tx channel0
 */
#define AXI_DMA_OUT_DSCR_BF0_CH0_REG (DR_REG_AXI_DMA_BASE + 0x170)
/** AXI_DMA_OUTLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define AXI_DMA_OUTLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF0_CH0_M  (AXI_DMA_OUTLINK_DSCR_BF0_CH0_V << AXI_DMA_OUTLINK_DSCR_BF0_CH0_S)
#define AXI_DMA_OUTLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF0_CH0_S  0

/** AXI_DMA_OUT_DSCR_BF1_CH0_REG register
 *  The second-to-last outlink descriptor address of Tx channel0
 */
#define AXI_DMA_OUT_DSCR_BF1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x174)
/** AXI_DMA_OUTLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor x-2.
 */
#define AXI_DMA_OUTLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF1_CH0_M  (AXI_DMA_OUTLINK_DSCR_BF1_CH0_V << AXI_DMA_OUTLINK_DSCR_BF1_CH0_S)
#define AXI_DMA_OUTLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF1_CH0_S  0

/** AXI_DMA_OUT_PRI_CH0_REG register
 *  Priority register of Tx channel0.
 */
#define AXI_DMA_OUT_PRI_CH0_REG (DR_REG_AXI_DMA_BASE + 0x178)
/** AXI_DMA_TX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel0. The larger of the value the higher of the priority.
 */
#define AXI_DMA_TX_PRI_CH0    0x0000000FU
#define AXI_DMA_TX_PRI_CH0_M  (AXI_DMA_TX_PRI_CH0_V << AXI_DMA_TX_PRI_CH0_S)
#define AXI_DMA_TX_PRI_CH0_V  0x0000000FU
#define AXI_DMA_TX_PRI_CH0_S  0
/** AXI_DMA_TX_CH_ARB_WEIGH_CH0 : R/W; bitpos: [7:4]; default: 0;
 *  The weight of Tx channel0
 */
#define AXI_DMA_TX_CH_ARB_WEIGH_CH0    0x0000000FU
#define AXI_DMA_TX_CH_ARB_WEIGH_CH0_M  (AXI_DMA_TX_CH_ARB_WEIGH_CH0_V << AXI_DMA_TX_CH_ARB_WEIGH_CH0_S)
#define AXI_DMA_TX_CH_ARB_WEIGH_CH0_V  0x0000000FU
#define AXI_DMA_TX_CH_ARB_WEIGH_CH0_S  4
/** AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH0 : R/W; bitpos: [8]; default: 0;
 *  0: mean not optimization weight function ,1: mean optimization
 */
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH0    (BIT(8))
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_M  (AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_V << AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_S)
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_V  0x00000001U
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_S  8

/** AXI_DMA_OUT_PERI_SEL_CH0_REG register
 *  Peripheral selection of Tx channel0
 */
#define AXI_DMA_OUT_PERI_SEL_CH0_REG (DR_REG_AXI_DMA_BASE + 0x17c)
/** AXI_DMA_PERI_OUT_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel0. 0:lcdcam. 1: gpspi_2.
 *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
 */
#define AXI_DMA_PERI_OUT_SEL_CH0    0x0000003FU
#define AXI_DMA_PERI_OUT_SEL_CH0_M  (AXI_DMA_PERI_OUT_SEL_CH0_V << AXI_DMA_PERI_OUT_SEL_CH0_S)
#define AXI_DMA_PERI_OUT_SEL_CH0_V  0x0000003FU
#define AXI_DMA_PERI_OUT_SEL_CH0_S  0

/** AXI_DMA_OUT_CRC_INIT_DATA_CH0_REG register
 *  This register is used to config ch0 crc initial data(max 32 bit)
 */
#define AXI_DMA_OUT_CRC_INIT_DATA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x180)
/** AXI_DMA_OUT_CRC_INIT_DATA_CH0 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch0 of tx crc initial value
 */
#define AXI_DMA_OUT_CRC_INIT_DATA_CH0    0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_INIT_DATA_CH0_M  (AXI_DMA_OUT_CRC_INIT_DATA_CH0_V << AXI_DMA_OUT_CRC_INIT_DATA_CH0_S)
#define AXI_DMA_OUT_CRC_INIT_DATA_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_INIT_DATA_CH0_S  0

/** AXI_DMA_TX_CRC_WIDTH_CH0_REG register
 *  This register is used to config tx ch0 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AXI_DMA_TX_CRC_WIDTH_CH0_REG (DR_REG_AXI_DMA_BASE + 0x184)
/** AXI_DMA_TX_CRC_WIDTH_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_WIDTH_CH0    0x00000003U
#define AXI_DMA_TX_CRC_WIDTH_CH0_M  (AXI_DMA_TX_CRC_WIDTH_CH0_V << AXI_DMA_TX_CRC_WIDTH_CH0_S)
#define AXI_DMA_TX_CRC_WIDTH_CH0_V  0x00000003U
#define AXI_DMA_TX_CRC_WIDTH_CH0_S  0
/** AXI_DMA_TX_CRC_LATCH_FLAG_CH0 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH0    (BIT(2))
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH0_M  (AXI_DMA_TX_CRC_LATCH_FLAG_CH0_V << AXI_DMA_TX_CRC_LATCH_FLAG_CH0_S)
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH0_V  0x00000001U
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH0_S  2

/** AXI_DMA_OUT_CRC_CLEAR_CH0_REG register
 *  This register is used to clear ch0 crc result
 */
#define AXI_DMA_OUT_CRC_CLEAR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x188)
/** AXI_DMA_OUT_CRC_CLEAR_CH0 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch0 of tx crc result
 */
#define AXI_DMA_OUT_CRC_CLEAR_CH0    (BIT(0))
#define AXI_DMA_OUT_CRC_CLEAR_CH0_M  (AXI_DMA_OUT_CRC_CLEAR_CH0_V << AXI_DMA_OUT_CRC_CLEAR_CH0_S)
#define AXI_DMA_OUT_CRC_CLEAR_CH0_V  0x00000001U
#define AXI_DMA_OUT_CRC_CLEAR_CH0_S  0

/** AXI_DMA_OUT_CRC_FINAL_RESULT_CH0_REG register
 *  This register is used to store ch0 crc result
 */
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH0_REG (DR_REG_AXI_DMA_BASE + 0x18c)
/** AXI_DMA_OUT_CRC_FINAL_RESULT_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch0 of tx
 */
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH0    0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH0_M  (AXI_DMA_OUT_CRC_FINAL_RESULT_CH0_V << AXI_DMA_OUT_CRC_FINAL_RESULT_CH0_S)
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH0_V  0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH0_S  0

/** AXI_DMA_TX_CRC_EN_WR_DATA_CH0_REG register
 *  This resister is used to config ch0 crc en for every bit
 */
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x190)
/** AXI_DMA_TX_CRC_EN_WR_DATA_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable tx ch0 crc 32bit on/off
 */
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH0    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH0_M  (AXI_DMA_TX_CRC_EN_WR_DATA_CH0_V << AXI_DMA_TX_CRC_EN_WR_DATA_CH0_S)
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH0_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH0_S  0

/** AXI_DMA_TX_CRC_EN_ADDR_CH0_REG register
 *  This register is used to config ch0 crc en addr
 */
#define AXI_DMA_TX_CRC_EN_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x194)
/** AXI_DMA_TX_CRC_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_EN_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_ADDR_CH0_M  (AXI_DMA_TX_CRC_EN_ADDR_CH0_V << AXI_DMA_TX_CRC_EN_ADDR_CH0_S)
#define AXI_DMA_TX_CRC_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_ADDR_CH0_S  0

/** AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_REG register
 *  This register is used to config crc data_8bit en
 */
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_REG (DR_REG_AXI_DMA_BASE + 0x198)
/** AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0 : R/W; bitpos: [15:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0    0x0000FFFFU
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_M  (AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_V << AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_S)
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_V  0x0000FFFFU
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_S  0

/** AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0_REG (DR_REG_AXI_DMA_BASE + 0x19c)
/** AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0_M  (AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0_V << AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0_S)
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH0_S  0

/** AXI_DMA_OUT_INT_RAW_CH1_REG register
 *  Raw status interrupt of channel1
 */
#define AXI_DMA_OUT_INT_RAW_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1a0)
/** AXI_DMA_OUT_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel1.
 */
#define AXI_DMA_OUT_DONE_CH1_INT_RAW    (BIT(0))
#define AXI_DMA_OUT_DONE_CH1_INT_RAW_M  (AXI_DMA_OUT_DONE_CH1_INT_RAW_V << AXI_DMA_OUT_DONE_CH1_INT_RAW_S)
#define AXI_DMA_OUT_DONE_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH1_INT_RAW_S  0
/** AXI_DMA_OUT_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel1.
 */
#define AXI_DMA_OUT_EOF_CH1_INT_RAW    (BIT(1))
#define AXI_DMA_OUT_EOF_CH1_INT_RAW_M  (AXI_DMA_OUT_EOF_CH1_INT_RAW_V << AXI_DMA_OUT_EOF_CH1_INT_RAW_S)
#define AXI_DMA_OUT_EOF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH1_INT_RAW_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel1.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_RAW    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_RAW_M  (AXI_DMA_OUT_DSCR_ERR_CH1_INT_RAW_V << AXI_DMA_OUT_DSCR_ERR_CH1_INT_RAW_S)
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_RAW_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel1.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_RAW    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_M  (AXI_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_V << AXI_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel1 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_RAW    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_RAW_M  (AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_RAW_V << AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_RAW_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel1 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_RAW    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_RAW_M  (AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_RAW_V << AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_RAW_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel1 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_RAW    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_RAW_M  (AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_RAW_V << AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_RAW_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel1 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_RAW    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_RAW_M  (AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_RAW_V << AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_RAW_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel1 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_RAW    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_RAW_M  (AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_RAW_V << AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_RAW_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel1 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_RAW    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_RAW_M  (AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_RAW_V << AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_RAW_S  9

/** AXI_DMA_OUT_INT_ST_CH1_REG register
 *  Masked interrupt of channel1
 */
#define AXI_DMA_OUT_INT_ST_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1a4)
/** AXI_DMA_OUT_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH1_INT_ST    (BIT(0))
#define AXI_DMA_OUT_DONE_CH1_INT_ST_M  (AXI_DMA_OUT_DONE_CH1_INT_ST_V << AXI_DMA_OUT_DONE_CH1_INT_ST_S)
#define AXI_DMA_OUT_DONE_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH1_INT_ST_S  0
/** AXI_DMA_OUT_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH1_INT_ST    (BIT(1))
#define AXI_DMA_OUT_EOF_CH1_INT_ST_M  (AXI_DMA_OUT_EOF_CH1_INT_ST_V << AXI_DMA_OUT_EOF_CH1_INT_ST_S)
#define AXI_DMA_OUT_EOF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH1_INT_ST_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ST    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ST_M  (AXI_DMA_OUT_DSCR_ERR_CH1_INT_ST_V << AXI_DMA_OUT_DSCR_ERR_CH1_INT_ST_S)
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ST_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ST    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ST_M  (AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ST_V << AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ST_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ST_S  3
/** AXI_DMA_OUTFIFO_OVF_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_OVF_CH1_INT_ST    (BIT(4))
#define AXI_DMA_OUTFIFO_OVF_CH1_INT_ST_M  (AXI_DMA_OUTFIFO_OVF_CH1_INT_ST_V << AXI_DMA_OUTFIFO_OVF_CH1_INT_ST_S)
#define AXI_DMA_OUTFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_OVF_CH1_INT_ST_S  4
/** AXI_DMA_OUTFIFO_UDF_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_UDF_CH1_INT_ST    (BIT(5))
#define AXI_DMA_OUTFIFO_UDF_CH1_INT_ST_M  (AXI_DMA_OUTFIFO_UDF_CH1_INT_ST_V << AXI_DMA_OUTFIFO_UDF_CH1_INT_ST_S)
#define AXI_DMA_OUTFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_UDF_CH1_INT_ST_S  5
/** AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ST    (BIT(6))
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ST_M  (AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ST_V << AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ST_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ST_S  6
/** AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ST    (BIT(7))
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ST_M  (AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ST_V << AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ST_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ST_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ST    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ST_M  (AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ST_V << AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ST_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ST_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ST    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ST_M  (AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ST_V << AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ST_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ST_S  9

/** AXI_DMA_OUT_INT_ENA_CH1_REG register
 *  Interrupt enable bits of channel1
 */
#define AXI_DMA_OUT_INT_ENA_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1a8)
/** AXI_DMA_OUT_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH1_INT_ENA    (BIT(0))
#define AXI_DMA_OUT_DONE_CH1_INT_ENA_M  (AXI_DMA_OUT_DONE_CH1_INT_ENA_V << AXI_DMA_OUT_DONE_CH1_INT_ENA_S)
#define AXI_DMA_OUT_DONE_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH1_INT_ENA_S  0
/** AXI_DMA_OUT_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH1_INT_ENA    (BIT(1))
#define AXI_DMA_OUT_EOF_CH1_INT_ENA_M  (AXI_DMA_OUT_EOF_CH1_INT_ENA_V << AXI_DMA_OUT_EOF_CH1_INT_ENA_S)
#define AXI_DMA_OUT_EOF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH1_INT_ENA_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ENA    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ENA_M  (AXI_DMA_OUT_DSCR_ERR_CH1_INT_ENA_V << AXI_DMA_OUT_DSCR_ERR_CH1_INT_ENA_S)
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_ENA_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ENA    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_M  (AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_V << AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ENA    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ENA_M  (AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ENA_V << AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_ENA_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ENA    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ENA_M  (AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ENA_V << AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_ENA_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_ENA    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_ENA_M  (AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_ENA_V << AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_ENA_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_ENA    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_ENA_M  (AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_ENA_V << AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_ENA_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ENA    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ENA_M  (AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ENA_V << AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_ENA_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ENA    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ENA_M  (AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ENA_V << AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_ENA_S  9

/** AXI_DMA_OUT_INT_CLR_CH1_REG register
 *  Interrupt clear bits of channel1
 */
#define AXI_DMA_OUT_INT_CLR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1ac)
/** AXI_DMA_OUT_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH1_INT_CLR    (BIT(0))
#define AXI_DMA_OUT_DONE_CH1_INT_CLR_M  (AXI_DMA_OUT_DONE_CH1_INT_CLR_V << AXI_DMA_OUT_DONE_CH1_INT_CLR_S)
#define AXI_DMA_OUT_DONE_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH1_INT_CLR_S  0
/** AXI_DMA_OUT_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH1_INT_CLR    (BIT(1))
#define AXI_DMA_OUT_EOF_CH1_INT_CLR_M  (AXI_DMA_OUT_EOF_CH1_INT_CLR_V << AXI_DMA_OUT_EOF_CH1_INT_CLR_S)
#define AXI_DMA_OUT_EOF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH1_INT_CLR_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_CLR    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_CLR_M  (AXI_DMA_OUT_DSCR_ERR_CH1_INT_CLR_V << AXI_DMA_OUT_DSCR_ERR_CH1_INT_CLR_S)
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH1_INT_CLR_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_CLR    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_M  (AXI_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_V << AXI_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_CLR    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_CLR_M  (AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_CLR_V << AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_INT_CLR_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_CLR    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_CLR_M  (AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_CLR_V << AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_INT_CLR_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_CLR    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_CLR_M  (AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_CLR_V << AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_INT_CLR_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_CLR    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_CLR_M  (AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_CLR_V << AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_INT_CLR_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_CLR    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_CLR_M  (AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_CLR_V << AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_INT_CLR_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_CLR    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_CLR_M  (AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_CLR_V << AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_INT_CLR_S  9

/** AXI_DMA_OUT_CONF0_CH1_REG register
 *  Configure 0 register of Tx channel1
 */
#define AXI_DMA_OUT_CONF0_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1b0)
/** AXI_DMA_OUT_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AXI_DMA channel1 Tx FSM and Tx FIFO pointer.
 */
#define AXI_DMA_OUT_RST_CH1    (BIT(0))
#define AXI_DMA_OUT_RST_CH1_M  (AXI_DMA_OUT_RST_CH1_V << AXI_DMA_OUT_RST_CH1_S)
#define AXI_DMA_OUT_RST_CH1_V  0x00000001U
#define AXI_DMA_OUT_RST_CH1_S  0
/** AXI_DMA_OUT_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_LOOP_TEST_CH1    (BIT(1))
#define AXI_DMA_OUT_LOOP_TEST_CH1_M  (AXI_DMA_OUT_LOOP_TEST_CH1_V << AXI_DMA_OUT_LOOP_TEST_CH1_S)
#define AXI_DMA_OUT_LOOP_TEST_CH1_V  0x00000001U
#define AXI_DMA_OUT_LOOP_TEST_CH1_S  1
/** AXI_DMA_OUT_AUTO_WRBACK_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define AXI_DMA_OUT_AUTO_WRBACK_CH1    (BIT(2))
#define AXI_DMA_OUT_AUTO_WRBACK_CH1_M  (AXI_DMA_OUT_AUTO_WRBACK_CH1_V << AXI_DMA_OUT_AUTO_WRBACK_CH1_S)
#define AXI_DMA_OUT_AUTO_WRBACK_CH1_V  0x00000001U
#define AXI_DMA_OUT_AUTO_WRBACK_CH1_S  2
/** AXI_DMA_OUT_EOF_MODE_CH1 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel1 is
 *  generated when data need to transmit has been popped from FIFO in AXI_DMA
 */
#define AXI_DMA_OUT_EOF_MODE_CH1    (BIT(3))
#define AXI_DMA_OUT_EOF_MODE_CH1_M  (AXI_DMA_OUT_EOF_MODE_CH1_V << AXI_DMA_OUT_EOF_MODE_CH1_S)
#define AXI_DMA_OUT_EOF_MODE_CH1_V  0x00000001U
#define AXI_DMA_OUT_EOF_MODE_CH1_S  3
/** AXI_DMA_OUT_ETM_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel1 is triggered by etm
 *  task.
 */
#define AXI_DMA_OUT_ETM_EN_CH1    (BIT(4))
#define AXI_DMA_OUT_ETM_EN_CH1_M  (AXI_DMA_OUT_ETM_EN_CH1_V << AXI_DMA_OUT_ETM_EN_CH1_S)
#define AXI_DMA_OUT_ETM_EN_CH1_V  0x00000001U
#define AXI_DMA_OUT_ETM_EN_CH1_S  4
/** AXI_DMA_OUT_BURST_SIZE_SEL_CH1 : R/W; bitpos: [7:5]; default: 0;
 *  3'b000-3'b100:burst length 8byte~128byte
 */
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH1    0x00000007U
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH1_M  (AXI_DMA_OUT_BURST_SIZE_SEL_CH1_V << AXI_DMA_OUT_BURST_SIZE_SEL_CH1_S)
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH1_V  0x00000007U
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH1_S  5
/** AXI_DMA_OUT_CMD_DISABLE_CH1 : R/W; bitpos: [8]; default: 0;
 *  1:mean disable cmd of this ch1
 */
#define AXI_DMA_OUT_CMD_DISABLE_CH1    (BIT(8))
#define AXI_DMA_OUT_CMD_DISABLE_CH1_M  (AXI_DMA_OUT_CMD_DISABLE_CH1_V << AXI_DMA_OUT_CMD_DISABLE_CH1_S)
#define AXI_DMA_OUT_CMD_DISABLE_CH1_V  0x00000001U
#define AXI_DMA_OUT_CMD_DISABLE_CH1_S  8
/** AXI_DMA_OUT_ECC_AEC_EN_CH1 : R/W; bitpos: [9]; default: 0;
 *  1: mean access ecc or aes domain,0: mean not
 */
#define AXI_DMA_OUT_ECC_AEC_EN_CH1    (BIT(9))
#define AXI_DMA_OUT_ECC_AEC_EN_CH1_M  (AXI_DMA_OUT_ECC_AEC_EN_CH1_V << AXI_DMA_OUT_ECC_AEC_EN_CH1_S)
#define AXI_DMA_OUT_ECC_AEC_EN_CH1_V  0x00000001U
#define AXI_DMA_OUT_ECC_AEC_EN_CH1_S  9
/** AXI_DMA_OUTDSCR_BURST_EN_CH1 : R/W; bitpos: [10]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel1 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AXI_DMA_OUTDSCR_BURST_EN_CH1    (BIT(10))
#define AXI_DMA_OUTDSCR_BURST_EN_CH1_M  (AXI_DMA_OUTDSCR_BURST_EN_CH1_V << AXI_DMA_OUTDSCR_BURST_EN_CH1_S)
#define AXI_DMA_OUTDSCR_BURST_EN_CH1_V  0x00000001U
#define AXI_DMA_OUTDSCR_BURST_EN_CH1_S  10

/** AXI_DMA_OUT_CONF1_CH1_REG register
 *  Configure 1 register of Tx channel1
 */
#define AXI_DMA_OUT_CONF1_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1b4)
/** AXI_DMA_OUT_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AXI_DMA_OUT_CHECK_OWNER_CH1    (BIT(12))
#define AXI_DMA_OUT_CHECK_OWNER_CH1_M  (AXI_DMA_OUT_CHECK_OWNER_CH1_V << AXI_DMA_OUT_CHECK_OWNER_CH1_S)
#define AXI_DMA_OUT_CHECK_OWNER_CH1_V  0x00000001U
#define AXI_DMA_OUT_CHECK_OWNER_CH1_S  12

/** AXI_DMA_OUTFIFO_STATUS_CH1_REG register
 *  Transmit FIFO status of Tx channel1
 */
#define AXI_DMA_OUTFIFO_STATUS_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1b8)
/** AXI_DMA_OUTFIFO_L3_FULL_CH1 : RO; bitpos: [0]; default: 0;
 *  L3 Tx FIFO full signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L3_FULL_CH1    (BIT(0))
#define AXI_DMA_OUTFIFO_L3_FULL_CH1_M  (AXI_DMA_OUTFIFO_L3_FULL_CH1_V << AXI_DMA_OUTFIFO_L3_FULL_CH1_S)
#define AXI_DMA_OUTFIFO_L3_FULL_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_FULL_CH1_S  0
/** AXI_DMA_OUTFIFO_L3_EMPTY_CH1 : RO; bitpos: [1]; default: 1;
 *  L3 Tx FIFO empty signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH1    (BIT(1))
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH1_M  (AXI_DMA_OUTFIFO_L3_EMPTY_CH1_V << AXI_DMA_OUTFIFO_L3_EMPTY_CH1_S)
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH1_S  1
/** AXI_DMA_OUTFIFO_L3_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L3 Tx FIFO for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L3_CNT_CH1    0x0000003FU
#define AXI_DMA_OUTFIFO_L3_CNT_CH1_M  (AXI_DMA_OUTFIFO_L3_CNT_CH1_V << AXI_DMA_OUTFIFO_L3_CNT_CH1_S)
#define AXI_DMA_OUTFIFO_L3_CNT_CH1_V  0x0000003FU
#define AXI_DMA_OUTFIFO_L3_CNT_CH1_S  2
/** AXI_DMA_OUTFIFO_L3_UDF_CH1 : RO; bitpos: [8]; default: 0;
 *  L3 Tx FIFO under flow signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH1    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_M  (AXI_DMA_OUTFIFO_L3_UDF_CH1_V << AXI_DMA_OUTFIFO_L3_UDF_CH1_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH1_S  8
/** AXI_DMA_OUTFIFO_L3_OVF_CH1 : RO; bitpos: [9]; default: 0;
 *  L3 Tx FIFO over flow signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH1    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_M  (AXI_DMA_OUTFIFO_L3_OVF_CH1_V << AXI_DMA_OUTFIFO_L3_OVF_CH1_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH1_S  9
/** AXI_DMA_OUTFIFO_L1_FULL_CH1 : RO; bitpos: [10]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L1_FULL_CH1    (BIT(10))
#define AXI_DMA_OUTFIFO_L1_FULL_CH1_M  (AXI_DMA_OUTFIFO_L1_FULL_CH1_V << AXI_DMA_OUTFIFO_L1_FULL_CH1_S)
#define AXI_DMA_OUTFIFO_L1_FULL_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_FULL_CH1_S  10
/** AXI_DMA_OUTFIFO_L1_EMPTY_CH1 : RO; bitpos: [11]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH1    (BIT(11))
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH1_M  (AXI_DMA_OUTFIFO_L1_EMPTY_CH1_V << AXI_DMA_OUTFIFO_L1_EMPTY_CH1_S)
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH1_S  11
/** AXI_DMA_OUTFIFO_L1_UDF_CH1 : RO; bitpos: [12]; default: 0;
 *  L1 Tx FIFO under flow signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH1    (BIT(12))
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_M  (AXI_DMA_OUTFIFO_L1_UDF_CH1_V << AXI_DMA_OUTFIFO_L1_UDF_CH1_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH1_S  12
/** AXI_DMA_OUTFIFO_L1_OVF_CH1 : RO; bitpos: [13]; default: 0;
 *  L1 Tx FIFO over flow signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH1    (BIT(13))
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_M  (AXI_DMA_OUTFIFO_L1_OVF_CH1_V << AXI_DMA_OUTFIFO_L1_OVF_CH1_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH1_S  13
/** AXI_DMA_OUTFIFO_L2_FULL_CH1 : RO; bitpos: [14]; default: 0;
 *  L2 Tx RAM full signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L2_FULL_CH1    (BIT(14))
#define AXI_DMA_OUTFIFO_L2_FULL_CH1_M  (AXI_DMA_OUTFIFO_L2_FULL_CH1_V << AXI_DMA_OUTFIFO_L2_FULL_CH1_S)
#define AXI_DMA_OUTFIFO_L2_FULL_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_FULL_CH1_S  14
/** AXI_DMA_OUTFIFO_L2_EMPTY_CH1 : RO; bitpos: [15]; default: 1;
 *  L2 Tx RAM empty signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH1    (BIT(15))
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH1_M  (AXI_DMA_OUTFIFO_L2_EMPTY_CH1_V << AXI_DMA_OUTFIFO_L2_EMPTY_CH1_S)
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH1_S  15
/** AXI_DMA_OUTFIFO_L2_UDF_CH1 : RO; bitpos: [16]; default: 0;
 *  L2 Tx FIFO under flow signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH1    (BIT(16))
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_M  (AXI_DMA_OUTFIFO_L2_UDF_CH1_V << AXI_DMA_OUTFIFO_L2_UDF_CH1_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH1_S  16
/** AXI_DMA_OUTFIFO_L2_OVF_CH1 : RO; bitpos: [17]; default: 0;
 *  L2 Tx FIFO over flow signal for Tx channel1.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH1    (BIT(17))
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_M  (AXI_DMA_OUTFIFO_L2_OVF_CH1_V << AXI_DMA_OUTFIFO_L2_OVF_CH1_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH1_S  17
/** AXI_DMA_OUT_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH1    (BIT(23))
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_1B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_1B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH1_S  23
/** AXI_DMA_OUT_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH1    (BIT(24))
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_2B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_2B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH1_S  24
/** AXI_DMA_OUT_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH1    (BIT(25))
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_3B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_3B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH1_S  25
/** AXI_DMA_OUT_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH1    (BIT(26))
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_4B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_4B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH1_S  26
/** AXI_DMA_OUT_REMAIN_UNDER_5B_CH1 : RO; bitpos: [27]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH1    (BIT(27))
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_5B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_5B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH1_S  27
/** AXI_DMA_OUT_REMAIN_UNDER_6B_CH1 : RO; bitpos: [28]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH1    (BIT(28))
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_6B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_6B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH1_S  28
/** AXI_DMA_OUT_REMAIN_UNDER_7B_CH1 : RO; bitpos: [29]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH1    (BIT(29))
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_7B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_7B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH1_S  29
/** AXI_DMA_OUT_REMAIN_UNDER_8B_CH1 : RO; bitpos: [30]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH1    (BIT(30))
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH1_M  (AXI_DMA_OUT_REMAIN_UNDER_8B_CH1_V << AXI_DMA_OUT_REMAIN_UNDER_8B_CH1_S)
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH1_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH1_S  30

/** AXI_DMA_OUT_PUSH_CH1_REG register
 *  Push control register of Tx channel1
 */
#define AXI_DMA_OUT_PUSH_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1bc)
/** AXI_DMA_OUTFIFO_WDATA_CH1 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into AXI_DMA FIFO.
 */
#define AXI_DMA_OUTFIFO_WDATA_CH1    0x000001FFU
#define AXI_DMA_OUTFIFO_WDATA_CH1_M  (AXI_DMA_OUTFIFO_WDATA_CH1_V << AXI_DMA_OUTFIFO_WDATA_CH1_S)
#define AXI_DMA_OUTFIFO_WDATA_CH1_V  0x000001FFU
#define AXI_DMA_OUTFIFO_WDATA_CH1_S  0
/** AXI_DMA_OUTFIFO_PUSH_CH1 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into AXI_DMA FIFO.
 */
#define AXI_DMA_OUTFIFO_PUSH_CH1    (BIT(9))
#define AXI_DMA_OUTFIFO_PUSH_CH1_M  (AXI_DMA_OUTFIFO_PUSH_CH1_V << AXI_DMA_OUTFIFO_PUSH_CH1_S)
#define AXI_DMA_OUTFIFO_PUSH_CH1_V  0x00000001U
#define AXI_DMA_OUTFIFO_PUSH_CH1_S  9

/** AXI_DMA_OUT_LINK1_CH1_REG register
 *  Link descriptor configure and control register of Tx channel1
 */
#define AXI_DMA_OUT_LINK1_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1c0)
/** AXI_DMA_OUTLINK_STOP_CH1 : WT; bitpos: [0]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define AXI_DMA_OUTLINK_STOP_CH1    (BIT(0))
#define AXI_DMA_OUTLINK_STOP_CH1_M  (AXI_DMA_OUTLINK_STOP_CH1_V << AXI_DMA_OUTLINK_STOP_CH1_S)
#define AXI_DMA_OUTLINK_STOP_CH1_V  0x00000001U
#define AXI_DMA_OUTLINK_STOP_CH1_S  0
/** AXI_DMA_OUTLINK_START_CH1 : WT; bitpos: [1]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define AXI_DMA_OUTLINK_START_CH1    (BIT(1))
#define AXI_DMA_OUTLINK_START_CH1_M  (AXI_DMA_OUTLINK_START_CH1_V << AXI_DMA_OUTLINK_START_CH1_S)
#define AXI_DMA_OUTLINK_START_CH1_V  0x00000001U
#define AXI_DMA_OUTLINK_START_CH1_S  1
/** AXI_DMA_OUTLINK_RESTART_CH1 : WT; bitpos: [2]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define AXI_DMA_OUTLINK_RESTART_CH1    (BIT(2))
#define AXI_DMA_OUTLINK_RESTART_CH1_M  (AXI_DMA_OUTLINK_RESTART_CH1_V << AXI_DMA_OUTLINK_RESTART_CH1_S)
#define AXI_DMA_OUTLINK_RESTART_CH1_V  0x00000001U
#define AXI_DMA_OUTLINK_RESTART_CH1_S  2
/** AXI_DMA_OUTLINK_PARK_CH1 : RO; bitpos: [3]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define AXI_DMA_OUTLINK_PARK_CH1    (BIT(3))
#define AXI_DMA_OUTLINK_PARK_CH1_M  (AXI_DMA_OUTLINK_PARK_CH1_V << AXI_DMA_OUTLINK_PARK_CH1_S)
#define AXI_DMA_OUTLINK_PARK_CH1_V  0x00000001U
#define AXI_DMA_OUTLINK_PARK_CH1_S  3

/** AXI_DMA_OUT_LINK2_CH1_REG register
 *  Link descriptor configure and control register of Tx channel1
 */
#define AXI_DMA_OUT_LINK2_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1c4)
/** AXI_DMA_OUTLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first outlink
 *  descriptor's address.
 */
#define AXI_DMA_OUTLINK_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_ADDR_CH1_M  (AXI_DMA_OUTLINK_ADDR_CH1_V << AXI_DMA_OUTLINK_ADDR_CH1_S)
#define AXI_DMA_OUTLINK_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_ADDR_CH1_S  0

/** AXI_DMA_OUT_STATE_CH1_REG register
 *  Transmit status of Tx channel1
 */
#define AXI_DMA_OUT_STATE_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1c8)
/** AXI_DMA_OUTLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH1_M  (AXI_DMA_OUTLINK_DSCR_ADDR_CH1_V << AXI_DMA_OUTLINK_DSCR_ADDR_CH1_S)
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH1_S  0
/** AXI_DMA_OUT_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_DSCR_STATE_CH1    0x00000003U
#define AXI_DMA_OUT_DSCR_STATE_CH1_M  (AXI_DMA_OUT_DSCR_STATE_CH1_V << AXI_DMA_OUT_DSCR_STATE_CH1_S)
#define AXI_DMA_OUT_DSCR_STATE_CH1_V  0x00000003U
#define AXI_DMA_OUT_DSCR_STATE_CH1_S  18
/** AXI_DMA_OUT_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_STATE_CH1    0x00000007U
#define AXI_DMA_OUT_STATE_CH1_M  (AXI_DMA_OUT_STATE_CH1_V << AXI_DMA_OUT_STATE_CH1_S)
#define AXI_DMA_OUT_STATE_CH1_V  0x00000007U
#define AXI_DMA_OUT_STATE_CH1_S  20

/** AXI_DMA_OUT_EOF_DES_ADDR_CH1_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel1
 */
#define AXI_DMA_OUT_EOF_DES_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1cc)
/** AXI_DMA_OUT_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AXI_DMA_OUT_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_DES_ADDR_CH1_M  (AXI_DMA_OUT_EOF_DES_ADDR_CH1_V << AXI_DMA_OUT_EOF_DES_ADDR_CH1_S)
#define AXI_DMA_OUT_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_DES_ADDR_CH1_S  0

/** AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel1
 */
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1d0)
/** AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1_M  (AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1_V << AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1_S)
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH1_S  0

/** AXI_DMA_OUT_DSCR_CH1_REG register
 *  Current outlink descriptor address of Tx channel1
 */
#define AXI_DMA_OUT_DSCR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1d4)
/** AXI_DMA_OUTLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define AXI_DMA_OUTLINK_DSCR_CH1    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_CH1_M  (AXI_DMA_OUTLINK_DSCR_CH1_V << AXI_DMA_OUTLINK_DSCR_CH1_S)
#define AXI_DMA_OUTLINK_DSCR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_CH1_S  0

/** AXI_DMA_OUT_DSCR_BF0_CH1_REG register
 *  The last outlink descriptor address of Tx channel1
 */
#define AXI_DMA_OUT_DSCR_BF0_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1d8)
/** AXI_DMA_OUTLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define AXI_DMA_OUTLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF0_CH1_M  (AXI_DMA_OUTLINK_DSCR_BF0_CH1_V << AXI_DMA_OUTLINK_DSCR_BF0_CH1_S)
#define AXI_DMA_OUTLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF0_CH1_S  0

/** AXI_DMA_OUT_DSCR_BF1_CH1_REG register
 *  The second-to-last outlink descriptor address of Tx channel1
 */
#define AXI_DMA_OUT_DSCR_BF1_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1dc)
/** AXI_DMA_OUTLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor x-2.
 */
#define AXI_DMA_OUTLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF1_CH1_M  (AXI_DMA_OUTLINK_DSCR_BF1_CH1_V << AXI_DMA_OUTLINK_DSCR_BF1_CH1_S)
#define AXI_DMA_OUTLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF1_CH1_S  0

/** AXI_DMA_OUT_PRI_CH1_REG register
 *  Priority register of Tx channel1.
 */
#define AXI_DMA_OUT_PRI_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1e0)
/** AXI_DMA_TX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel1. The larger of the value the higher of the priority.
 */
#define AXI_DMA_TX_PRI_CH1    0x0000000FU
#define AXI_DMA_TX_PRI_CH1_M  (AXI_DMA_TX_PRI_CH1_V << AXI_DMA_TX_PRI_CH1_S)
#define AXI_DMA_TX_PRI_CH1_V  0x0000000FU
#define AXI_DMA_TX_PRI_CH1_S  0
/** AXI_DMA_TX_CH_ARB_WEIGH_CH1 : R/W; bitpos: [7:4]; default: 0;
 *  The weight of Tx channel1
 */
#define AXI_DMA_TX_CH_ARB_WEIGH_CH1    0x0000000FU
#define AXI_DMA_TX_CH_ARB_WEIGH_CH1_M  (AXI_DMA_TX_CH_ARB_WEIGH_CH1_V << AXI_DMA_TX_CH_ARB_WEIGH_CH1_S)
#define AXI_DMA_TX_CH_ARB_WEIGH_CH1_V  0x0000000FU
#define AXI_DMA_TX_CH_ARB_WEIGH_CH1_S  4
/** AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH1 : R/W; bitpos: [8]; default: 0;
 *  0: mean not optimization weight function ,1: mean optimization
 */
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH1    (BIT(8))
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_M  (AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_V << AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_S)
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_V  0x00000001U
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_S  8

/** AXI_DMA_OUT_PERI_SEL_CH1_REG register
 *  Peripheral selection of Tx channel1
 */
#define AXI_DMA_OUT_PERI_SEL_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1e4)
/** AXI_DMA_PERI_OUT_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel1. 0:lcdcam. 1: gpspi_2.
 *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
 */
#define AXI_DMA_PERI_OUT_SEL_CH1    0x0000003FU
#define AXI_DMA_PERI_OUT_SEL_CH1_M  (AXI_DMA_PERI_OUT_SEL_CH1_V << AXI_DMA_PERI_OUT_SEL_CH1_S)
#define AXI_DMA_PERI_OUT_SEL_CH1_V  0x0000003FU
#define AXI_DMA_PERI_OUT_SEL_CH1_S  0

/** AXI_DMA_OUT_CRC_INIT_DATA_CH1_REG register
 *  This register is used to config ch1 crc initial data(max 32 bit)
 */
#define AXI_DMA_OUT_CRC_INIT_DATA_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1e8)
/** AXI_DMA_OUT_CRC_INIT_DATA_CH1 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch1 of tx crc initial value
 */
#define AXI_DMA_OUT_CRC_INIT_DATA_CH1    0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_INIT_DATA_CH1_M  (AXI_DMA_OUT_CRC_INIT_DATA_CH1_V << AXI_DMA_OUT_CRC_INIT_DATA_CH1_S)
#define AXI_DMA_OUT_CRC_INIT_DATA_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_INIT_DATA_CH1_S  0

/** AXI_DMA_TX_CRC_WIDTH_CH1_REG register
 *  This register is used to config tx ch1 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AXI_DMA_TX_CRC_WIDTH_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1ec)
/** AXI_DMA_TX_CRC_WIDTH_CH1 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_WIDTH_CH1    0x00000003U
#define AXI_DMA_TX_CRC_WIDTH_CH1_M  (AXI_DMA_TX_CRC_WIDTH_CH1_V << AXI_DMA_TX_CRC_WIDTH_CH1_S)
#define AXI_DMA_TX_CRC_WIDTH_CH1_V  0x00000003U
#define AXI_DMA_TX_CRC_WIDTH_CH1_S  0
/** AXI_DMA_TX_CRC_LATCH_FLAG_CH1 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH1    (BIT(2))
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH1_M  (AXI_DMA_TX_CRC_LATCH_FLAG_CH1_V << AXI_DMA_TX_CRC_LATCH_FLAG_CH1_S)
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH1_V  0x00000001U
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH1_S  2

/** AXI_DMA_OUT_CRC_CLEAR_CH1_REG register
 *  This register is used to clear ch1 crc result
 */
#define AXI_DMA_OUT_CRC_CLEAR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1f0)
/** AXI_DMA_OUT_CRC_CLEAR_CH1 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch1 of tx crc result
 */
#define AXI_DMA_OUT_CRC_CLEAR_CH1    (BIT(0))
#define AXI_DMA_OUT_CRC_CLEAR_CH1_M  (AXI_DMA_OUT_CRC_CLEAR_CH1_V << AXI_DMA_OUT_CRC_CLEAR_CH1_S)
#define AXI_DMA_OUT_CRC_CLEAR_CH1_V  0x00000001U
#define AXI_DMA_OUT_CRC_CLEAR_CH1_S  0

/** AXI_DMA_OUT_CRC_FINAL_RESULT_CH1_REG register
 *  This register is used to store ch1 crc result
 */
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1f4)
/** AXI_DMA_OUT_CRC_FINAL_RESULT_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch1 of tx
 */
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH1    0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH1_M  (AXI_DMA_OUT_CRC_FINAL_RESULT_CH1_V << AXI_DMA_OUT_CRC_FINAL_RESULT_CH1_S)
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH1_V  0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH1_S  0

/** AXI_DMA_TX_CRC_EN_WR_DATA_CH1_REG register
 *  This resister is used to config ch1 crc en for every bit
 */
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1f8)
/** AXI_DMA_TX_CRC_EN_WR_DATA_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable tx ch1 crc 32bit on/off
 */
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH1    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH1_M  (AXI_DMA_TX_CRC_EN_WR_DATA_CH1_V << AXI_DMA_TX_CRC_EN_WR_DATA_CH1_S)
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH1_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH1_S  0

/** AXI_DMA_TX_CRC_EN_ADDR_CH1_REG register
 *  This register is used to config ch1 crc en addr
 */
#define AXI_DMA_TX_CRC_EN_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x1fc)
/** AXI_DMA_TX_CRC_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_EN_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_ADDR_CH1_M  (AXI_DMA_TX_CRC_EN_ADDR_CH1_V << AXI_DMA_TX_CRC_EN_ADDR_CH1_S)
#define AXI_DMA_TX_CRC_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_ADDR_CH1_S  0

/** AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_REG register
 *  This register is used to config crc data_8bit en
 */
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_REG (DR_REG_AXI_DMA_BASE + 0x200)
/** AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1 : R/W; bitpos: [15:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1    0x0000FFFFU
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_M  (AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_V << AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_S)
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_V  0x0000FFFFU
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_S  0

/** AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1_REG (DR_REG_AXI_DMA_BASE + 0x204)
/** AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1_M  (AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1_V << AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1_S)
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH1_S  0

/** AXI_DMA_OUT_INT_RAW_CH2_REG register
 *  Raw status interrupt of channel2
 */
#define AXI_DMA_OUT_INT_RAW_CH2_REG (DR_REG_AXI_DMA_BASE + 0x208)
/** AXI_DMA_OUT_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel2.
 */
#define AXI_DMA_OUT_DONE_CH2_INT_RAW    (BIT(0))
#define AXI_DMA_OUT_DONE_CH2_INT_RAW_M  (AXI_DMA_OUT_DONE_CH2_INT_RAW_V << AXI_DMA_OUT_DONE_CH2_INT_RAW_S)
#define AXI_DMA_OUT_DONE_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH2_INT_RAW_S  0
/** AXI_DMA_OUT_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel2.
 */
#define AXI_DMA_OUT_EOF_CH2_INT_RAW    (BIT(1))
#define AXI_DMA_OUT_EOF_CH2_INT_RAW_M  (AXI_DMA_OUT_EOF_CH2_INT_RAW_V << AXI_DMA_OUT_EOF_CH2_INT_RAW_S)
#define AXI_DMA_OUT_EOF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH2_INT_RAW_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel2.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_RAW    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_RAW_M  (AXI_DMA_OUT_DSCR_ERR_CH2_INT_RAW_V << AXI_DMA_OUT_DSCR_ERR_CH2_INT_RAW_S)
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_RAW_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel2.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_RAW    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_M  (AXI_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_V << AXI_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel2 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_RAW    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_RAW_M  (AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_RAW_V << AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_RAW_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel2 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_RAW    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_RAW_M  (AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_RAW_V << AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_RAW_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel2 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_RAW    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_RAW_M  (AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_RAW_V << AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_RAW_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel2 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_RAW    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_RAW_M  (AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_RAW_V << AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_RAW_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel2 is
 *  overflow.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_RAW    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_RAW_M  (AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_RAW_V << AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_RAW_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel2 is
 *  underflow.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_RAW    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_RAW_M  (AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_RAW_V << AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_RAW_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_RAW_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_RAW_S  9

/** AXI_DMA_OUT_INT_ST_CH2_REG register
 *  Masked interrupt of channel2
 */
#define AXI_DMA_OUT_INT_ST_CH2_REG (DR_REG_AXI_DMA_BASE + 0x20c)
/** AXI_DMA_OUT_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH2_INT_ST    (BIT(0))
#define AXI_DMA_OUT_DONE_CH2_INT_ST_M  (AXI_DMA_OUT_DONE_CH2_INT_ST_V << AXI_DMA_OUT_DONE_CH2_INT_ST_S)
#define AXI_DMA_OUT_DONE_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH2_INT_ST_S  0
/** AXI_DMA_OUT_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH2_INT_ST    (BIT(1))
#define AXI_DMA_OUT_EOF_CH2_INT_ST_M  (AXI_DMA_OUT_EOF_CH2_INT_ST_V << AXI_DMA_OUT_EOF_CH2_INT_ST_S)
#define AXI_DMA_OUT_EOF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH2_INT_ST_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ST    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ST_M  (AXI_DMA_OUT_DSCR_ERR_CH2_INT_ST_V << AXI_DMA_OUT_DSCR_ERR_CH2_INT_ST_S)
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ST_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ST    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ST_M  (AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ST_V << AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ST_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ST_S  3
/** AXI_DMA_OUTFIFO_OVF_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_OVF_CH2_INT_ST    (BIT(4))
#define AXI_DMA_OUTFIFO_OVF_CH2_INT_ST_M  (AXI_DMA_OUTFIFO_OVF_CH2_INT_ST_V << AXI_DMA_OUTFIFO_OVF_CH2_INT_ST_S)
#define AXI_DMA_OUTFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_OVF_CH2_INT_ST_S  4
/** AXI_DMA_OUTFIFO_UDF_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_UDF_CH2_INT_ST    (BIT(5))
#define AXI_DMA_OUTFIFO_UDF_CH2_INT_ST_M  (AXI_DMA_OUTFIFO_UDF_CH2_INT_ST_V << AXI_DMA_OUTFIFO_UDF_CH2_INT_ST_S)
#define AXI_DMA_OUTFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_UDF_CH2_INT_ST_S  5
/** AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ST    (BIT(6))
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ST_M  (AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ST_V << AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ST_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ST_S  6
/** AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ST    (BIT(7))
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ST_M  (AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ST_V << AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ST_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ST_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ST    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ST_M  (AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ST_V << AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ST_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ST_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ST    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ST_M  (AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ST_V << AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ST_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ST_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ST_S  9

/** AXI_DMA_OUT_INT_ENA_CH2_REG register
 *  Interrupt enable bits of channel2
 */
#define AXI_DMA_OUT_INT_ENA_CH2_REG (DR_REG_AXI_DMA_BASE + 0x210)
/** AXI_DMA_OUT_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH2_INT_ENA    (BIT(0))
#define AXI_DMA_OUT_DONE_CH2_INT_ENA_M  (AXI_DMA_OUT_DONE_CH2_INT_ENA_V << AXI_DMA_OUT_DONE_CH2_INT_ENA_S)
#define AXI_DMA_OUT_DONE_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH2_INT_ENA_S  0
/** AXI_DMA_OUT_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH2_INT_ENA    (BIT(1))
#define AXI_DMA_OUT_EOF_CH2_INT_ENA_M  (AXI_DMA_OUT_EOF_CH2_INT_ENA_V << AXI_DMA_OUT_EOF_CH2_INT_ENA_S)
#define AXI_DMA_OUT_EOF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH2_INT_ENA_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ENA    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ENA_M  (AXI_DMA_OUT_DSCR_ERR_CH2_INT_ENA_V << AXI_DMA_OUT_DSCR_ERR_CH2_INT_ENA_S)
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_ENA_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ENA    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_M  (AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_V << AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ENA    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ENA_M  (AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ENA_V << AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_ENA_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ENA    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ENA_M  (AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ENA_V << AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_ENA_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_ENA    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_ENA_M  (AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_ENA_V << AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_ENA_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_ENA    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_ENA_M  (AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_ENA_V << AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_ENA_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ENA    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ENA_M  (AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ENA_V << AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_ENA_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ENA    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ENA_M  (AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ENA_V << AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ENA_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ENA_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_ENA_S  9

/** AXI_DMA_OUT_INT_CLR_CH2_REG register
 *  Interrupt clear bits of channel2
 */
#define AXI_DMA_OUT_INT_CLR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x214)
/** AXI_DMA_OUT_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DONE_CH2_INT_CLR    (BIT(0))
#define AXI_DMA_OUT_DONE_CH2_INT_CLR_M  (AXI_DMA_OUT_DONE_CH2_INT_CLR_V << AXI_DMA_OUT_DONE_CH2_INT_CLR_S)
#define AXI_DMA_OUT_DONE_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_DONE_CH2_INT_CLR_S  0
/** AXI_DMA_OUT_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_EOF_CH2_INT_CLR    (BIT(1))
#define AXI_DMA_OUT_EOF_CH2_INT_CLR_M  (AXI_DMA_OUT_EOF_CH2_INT_CLR_V << AXI_DMA_OUT_EOF_CH2_INT_CLR_S)
#define AXI_DMA_OUT_EOF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_EOF_CH2_INT_CLR_S  1
/** AXI_DMA_OUT_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_CLR    (BIT(2))
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_CLR_M  (AXI_DMA_OUT_DSCR_ERR_CH2_INT_CLR_V << AXI_DMA_OUT_DSCR_ERR_CH2_INT_CLR_S)
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_DSCR_ERR_CH2_INT_CLR_S  2
/** AXI_DMA_OUT_TOTAL_EOF_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_CLR    (BIT(3))
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_M  (AXI_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_V << AXI_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_S)
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_S  3
/** AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_CLR    (BIT(4))
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_CLR_M  (AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_CLR_V << AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_INT_CLR_S  4
/** AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_CLR    (BIT(5))
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_CLR_M  (AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_CLR_V << AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_INT_CLR_S  5
/** AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_CLR    (BIT(6))
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_CLR_M  (AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_CLR_V << AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_INT_CLR_S  6
/** AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_CLR    (BIT(7))
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_CLR_M  (AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_CLR_V << AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_INT_CLR_S  7
/** AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_CLR    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_CLR_M  (AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_CLR_V << AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_INT_CLR_S  8
/** AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_CLR    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_CLR_M  (AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_CLR_V << AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_CLR_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_CLR_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_INT_CLR_S  9

/** AXI_DMA_OUT_CONF0_CH2_REG register
 *  Configure 0 register of Tx channel2
 */
#define AXI_DMA_OUT_CONF0_CH2_REG (DR_REG_AXI_DMA_BASE + 0x218)
/** AXI_DMA_OUT_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AXI_DMA channel2 Tx FSM and Tx FIFO pointer.
 */
#define AXI_DMA_OUT_RST_CH2    (BIT(0))
#define AXI_DMA_OUT_RST_CH2_M  (AXI_DMA_OUT_RST_CH2_V << AXI_DMA_OUT_RST_CH2_S)
#define AXI_DMA_OUT_RST_CH2_V  0x00000001U
#define AXI_DMA_OUT_RST_CH2_S  0
/** AXI_DMA_OUT_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_LOOP_TEST_CH2    (BIT(1))
#define AXI_DMA_OUT_LOOP_TEST_CH2_M  (AXI_DMA_OUT_LOOP_TEST_CH2_V << AXI_DMA_OUT_LOOP_TEST_CH2_S)
#define AXI_DMA_OUT_LOOP_TEST_CH2_V  0x00000001U
#define AXI_DMA_OUT_LOOP_TEST_CH2_S  1
/** AXI_DMA_OUT_AUTO_WRBACK_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define AXI_DMA_OUT_AUTO_WRBACK_CH2    (BIT(2))
#define AXI_DMA_OUT_AUTO_WRBACK_CH2_M  (AXI_DMA_OUT_AUTO_WRBACK_CH2_V << AXI_DMA_OUT_AUTO_WRBACK_CH2_S)
#define AXI_DMA_OUT_AUTO_WRBACK_CH2_V  0x00000001U
#define AXI_DMA_OUT_AUTO_WRBACK_CH2_S  2
/** AXI_DMA_OUT_EOF_MODE_CH2 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel2 is
 *  generated when data need to transmit has been popped from FIFO in AXI_DMA
 */
#define AXI_DMA_OUT_EOF_MODE_CH2    (BIT(3))
#define AXI_DMA_OUT_EOF_MODE_CH2_M  (AXI_DMA_OUT_EOF_MODE_CH2_V << AXI_DMA_OUT_EOF_MODE_CH2_S)
#define AXI_DMA_OUT_EOF_MODE_CH2_V  0x00000001U
#define AXI_DMA_OUT_EOF_MODE_CH2_S  3
/** AXI_DMA_OUT_ETM_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel2 is triggered by etm
 *  task.
 */
#define AXI_DMA_OUT_ETM_EN_CH2    (BIT(4))
#define AXI_DMA_OUT_ETM_EN_CH2_M  (AXI_DMA_OUT_ETM_EN_CH2_V << AXI_DMA_OUT_ETM_EN_CH2_S)
#define AXI_DMA_OUT_ETM_EN_CH2_V  0x00000001U
#define AXI_DMA_OUT_ETM_EN_CH2_S  4
/** AXI_DMA_OUT_BURST_SIZE_SEL_CH2 : R/W; bitpos: [7:5]; default: 0;
 *  3'b000-3'b100:burst length 8byte~128byte
 */
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH2    0x00000007U
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH2_M  (AXI_DMA_OUT_BURST_SIZE_SEL_CH2_V << AXI_DMA_OUT_BURST_SIZE_SEL_CH2_S)
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH2_V  0x00000007U
#define AXI_DMA_OUT_BURST_SIZE_SEL_CH2_S  5
/** AXI_DMA_OUT_CMD_DISABLE_CH2 : R/W; bitpos: [8]; default: 0;
 *  1:mean disable cmd of this ch2
 */
#define AXI_DMA_OUT_CMD_DISABLE_CH2    (BIT(8))
#define AXI_DMA_OUT_CMD_DISABLE_CH2_M  (AXI_DMA_OUT_CMD_DISABLE_CH2_V << AXI_DMA_OUT_CMD_DISABLE_CH2_S)
#define AXI_DMA_OUT_CMD_DISABLE_CH2_V  0x00000001U
#define AXI_DMA_OUT_CMD_DISABLE_CH2_S  8
/** AXI_DMA_OUT_ECC_AEC_EN_CH2 : R/W; bitpos: [9]; default: 0;
 *  1: mean access ecc or aes domain,0: mean not
 */
#define AXI_DMA_OUT_ECC_AEC_EN_CH2    (BIT(9))
#define AXI_DMA_OUT_ECC_AEC_EN_CH2_M  (AXI_DMA_OUT_ECC_AEC_EN_CH2_V << AXI_DMA_OUT_ECC_AEC_EN_CH2_S)
#define AXI_DMA_OUT_ECC_AEC_EN_CH2_V  0x00000001U
#define AXI_DMA_OUT_ECC_AEC_EN_CH2_S  9
/** AXI_DMA_OUTDSCR_BURST_EN_CH2 : R/W; bitpos: [10]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel2 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AXI_DMA_OUTDSCR_BURST_EN_CH2    (BIT(10))
#define AXI_DMA_OUTDSCR_BURST_EN_CH2_M  (AXI_DMA_OUTDSCR_BURST_EN_CH2_V << AXI_DMA_OUTDSCR_BURST_EN_CH2_S)
#define AXI_DMA_OUTDSCR_BURST_EN_CH2_V  0x00000001U
#define AXI_DMA_OUTDSCR_BURST_EN_CH2_S  10

/** AXI_DMA_OUT_CONF1_CH2_REG register
 *  Configure 1 register of Tx channel2
 */
#define AXI_DMA_OUT_CONF1_CH2_REG (DR_REG_AXI_DMA_BASE + 0x21c)
/** AXI_DMA_OUT_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AXI_DMA_OUT_CHECK_OWNER_CH2    (BIT(12))
#define AXI_DMA_OUT_CHECK_OWNER_CH2_M  (AXI_DMA_OUT_CHECK_OWNER_CH2_V << AXI_DMA_OUT_CHECK_OWNER_CH2_S)
#define AXI_DMA_OUT_CHECK_OWNER_CH2_V  0x00000001U
#define AXI_DMA_OUT_CHECK_OWNER_CH2_S  12

/** AXI_DMA_OUTFIFO_STATUS_CH2_REG register
 *  Transmit FIFO status of Tx channel2
 */
#define AXI_DMA_OUTFIFO_STATUS_CH2_REG (DR_REG_AXI_DMA_BASE + 0x220)
/** AXI_DMA_OUTFIFO_L3_FULL_CH2 : RO; bitpos: [0]; default: 0;
 *  L3 Tx FIFO full signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L3_FULL_CH2    (BIT(0))
#define AXI_DMA_OUTFIFO_L3_FULL_CH2_M  (AXI_DMA_OUTFIFO_L3_FULL_CH2_V << AXI_DMA_OUTFIFO_L3_FULL_CH2_S)
#define AXI_DMA_OUTFIFO_L3_FULL_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_FULL_CH2_S  0
/** AXI_DMA_OUTFIFO_L3_EMPTY_CH2 : RO; bitpos: [1]; default: 1;
 *  L3 Tx FIFO empty signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH2    (BIT(1))
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH2_M  (AXI_DMA_OUTFIFO_L3_EMPTY_CH2_V << AXI_DMA_OUTFIFO_L3_EMPTY_CH2_S)
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_EMPTY_CH2_S  1
/** AXI_DMA_OUTFIFO_L3_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L3 Tx FIFO for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L3_CNT_CH2    0x0000003FU
#define AXI_DMA_OUTFIFO_L3_CNT_CH2_M  (AXI_DMA_OUTFIFO_L3_CNT_CH2_V << AXI_DMA_OUTFIFO_L3_CNT_CH2_S)
#define AXI_DMA_OUTFIFO_L3_CNT_CH2_V  0x0000003FU
#define AXI_DMA_OUTFIFO_L3_CNT_CH2_S  2
/** AXI_DMA_OUTFIFO_L3_UDF_CH2 : RO; bitpos: [8]; default: 0;
 *  L3 Tx FIFO under flow signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L3_UDF_CH2    (BIT(8))
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_M  (AXI_DMA_OUTFIFO_L3_UDF_CH2_V << AXI_DMA_OUTFIFO_L3_UDF_CH2_S)
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_UDF_CH2_S  8
/** AXI_DMA_OUTFIFO_L3_OVF_CH2 : RO; bitpos: [9]; default: 0;
 *  L3 Tx FIFO over flow signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L3_OVF_CH2    (BIT(9))
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_M  (AXI_DMA_OUTFIFO_L3_OVF_CH2_V << AXI_DMA_OUTFIFO_L3_OVF_CH2_S)
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L3_OVF_CH2_S  9
/** AXI_DMA_OUTFIFO_L1_FULL_CH2 : RO; bitpos: [10]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L1_FULL_CH2    (BIT(10))
#define AXI_DMA_OUTFIFO_L1_FULL_CH2_M  (AXI_DMA_OUTFIFO_L1_FULL_CH2_V << AXI_DMA_OUTFIFO_L1_FULL_CH2_S)
#define AXI_DMA_OUTFIFO_L1_FULL_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_FULL_CH2_S  10
/** AXI_DMA_OUTFIFO_L1_EMPTY_CH2 : RO; bitpos: [11]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH2    (BIT(11))
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH2_M  (AXI_DMA_OUTFIFO_L1_EMPTY_CH2_V << AXI_DMA_OUTFIFO_L1_EMPTY_CH2_S)
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_EMPTY_CH2_S  11
/** AXI_DMA_OUTFIFO_L1_UDF_CH2 : RO; bitpos: [12]; default: 0;
 *  L1 Tx FIFO under flow signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L1_UDF_CH2    (BIT(12))
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_M  (AXI_DMA_OUTFIFO_L1_UDF_CH2_V << AXI_DMA_OUTFIFO_L1_UDF_CH2_S)
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_UDF_CH2_S  12
/** AXI_DMA_OUTFIFO_L1_OVF_CH2 : RO; bitpos: [13]; default: 0;
 *  L1 Tx FIFO over flow signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L1_OVF_CH2    (BIT(13))
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_M  (AXI_DMA_OUTFIFO_L1_OVF_CH2_V << AXI_DMA_OUTFIFO_L1_OVF_CH2_S)
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L1_OVF_CH2_S  13
/** AXI_DMA_OUTFIFO_L2_FULL_CH2 : RO; bitpos: [14]; default: 0;
 *  L2 Tx RAM full signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L2_FULL_CH2    (BIT(14))
#define AXI_DMA_OUTFIFO_L2_FULL_CH2_M  (AXI_DMA_OUTFIFO_L2_FULL_CH2_V << AXI_DMA_OUTFIFO_L2_FULL_CH2_S)
#define AXI_DMA_OUTFIFO_L2_FULL_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_FULL_CH2_S  14
/** AXI_DMA_OUTFIFO_L2_EMPTY_CH2 : RO; bitpos: [15]; default: 1;
 *  L2 Tx RAM empty signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH2    (BIT(15))
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH2_M  (AXI_DMA_OUTFIFO_L2_EMPTY_CH2_V << AXI_DMA_OUTFIFO_L2_EMPTY_CH2_S)
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_EMPTY_CH2_S  15
/** AXI_DMA_OUTFIFO_L2_UDF_CH2 : RO; bitpos: [16]; default: 0;
 *  L2 Tx FIFO under flow signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L2_UDF_CH2    (BIT(16))
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_M  (AXI_DMA_OUTFIFO_L2_UDF_CH2_V << AXI_DMA_OUTFIFO_L2_UDF_CH2_S)
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_UDF_CH2_S  16
/** AXI_DMA_OUTFIFO_L2_OVF_CH2 : RO; bitpos: [17]; default: 0;
 *  L2 Tx FIFO over flow signal for Tx channel2.
 */
#define AXI_DMA_OUTFIFO_L2_OVF_CH2    (BIT(17))
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_M  (AXI_DMA_OUTFIFO_L2_OVF_CH2_V << AXI_DMA_OUTFIFO_L2_OVF_CH2_S)
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_L2_OVF_CH2_S  17
/** AXI_DMA_OUT_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH2    (BIT(23))
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_1B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_1B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_1B_CH2_S  23
/** AXI_DMA_OUT_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH2    (BIT(24))
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_2B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_2B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_2B_CH2_S  24
/** AXI_DMA_OUT_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH2    (BIT(25))
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_3B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_3B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_3B_CH2_S  25
/** AXI_DMA_OUT_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH2    (BIT(26))
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_4B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_4B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_4B_CH2_S  26
/** AXI_DMA_OUT_REMAIN_UNDER_5B_CH2 : RO; bitpos: [27]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH2    (BIT(27))
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_5B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_5B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_5B_CH2_S  27
/** AXI_DMA_OUT_REMAIN_UNDER_6B_CH2 : RO; bitpos: [28]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH2    (BIT(28))
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_6B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_6B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_6B_CH2_S  28
/** AXI_DMA_OUT_REMAIN_UNDER_7B_CH2 : RO; bitpos: [29]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH2    (BIT(29))
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_7B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_7B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_7B_CH2_S  29
/** AXI_DMA_OUT_REMAIN_UNDER_8B_CH2 : RO; bitpos: [30]; default: 1;
 *  reserved
 */
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH2    (BIT(30))
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH2_M  (AXI_DMA_OUT_REMAIN_UNDER_8B_CH2_V << AXI_DMA_OUT_REMAIN_UNDER_8B_CH2_S)
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH2_V  0x00000001U
#define AXI_DMA_OUT_REMAIN_UNDER_8B_CH2_S  30

/** AXI_DMA_OUT_PUSH_CH2_REG register
 *  Push control register of Tx channel2
 */
#define AXI_DMA_OUT_PUSH_CH2_REG (DR_REG_AXI_DMA_BASE + 0x224)
/** AXI_DMA_OUTFIFO_WDATA_CH2 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into AXI_DMA FIFO.
 */
#define AXI_DMA_OUTFIFO_WDATA_CH2    0x000001FFU
#define AXI_DMA_OUTFIFO_WDATA_CH2_M  (AXI_DMA_OUTFIFO_WDATA_CH2_V << AXI_DMA_OUTFIFO_WDATA_CH2_S)
#define AXI_DMA_OUTFIFO_WDATA_CH2_V  0x000001FFU
#define AXI_DMA_OUTFIFO_WDATA_CH2_S  0
/** AXI_DMA_OUTFIFO_PUSH_CH2 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into AXI_DMA FIFO.
 */
#define AXI_DMA_OUTFIFO_PUSH_CH2    (BIT(9))
#define AXI_DMA_OUTFIFO_PUSH_CH2_M  (AXI_DMA_OUTFIFO_PUSH_CH2_V << AXI_DMA_OUTFIFO_PUSH_CH2_S)
#define AXI_DMA_OUTFIFO_PUSH_CH2_V  0x00000001U
#define AXI_DMA_OUTFIFO_PUSH_CH2_S  9

/** AXI_DMA_OUT_LINK1_CH2_REG register
 *  Link descriptor configure and control register of Tx channel2
 */
#define AXI_DMA_OUT_LINK1_CH2_REG (DR_REG_AXI_DMA_BASE + 0x228)
/** AXI_DMA_OUTLINK_STOP_CH2 : WT; bitpos: [0]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define AXI_DMA_OUTLINK_STOP_CH2    (BIT(0))
#define AXI_DMA_OUTLINK_STOP_CH2_M  (AXI_DMA_OUTLINK_STOP_CH2_V << AXI_DMA_OUTLINK_STOP_CH2_S)
#define AXI_DMA_OUTLINK_STOP_CH2_V  0x00000001U
#define AXI_DMA_OUTLINK_STOP_CH2_S  0
/** AXI_DMA_OUTLINK_START_CH2 : WT; bitpos: [1]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define AXI_DMA_OUTLINK_START_CH2    (BIT(1))
#define AXI_DMA_OUTLINK_START_CH2_M  (AXI_DMA_OUTLINK_START_CH2_V << AXI_DMA_OUTLINK_START_CH2_S)
#define AXI_DMA_OUTLINK_START_CH2_V  0x00000001U
#define AXI_DMA_OUTLINK_START_CH2_S  1
/** AXI_DMA_OUTLINK_RESTART_CH2 : WT; bitpos: [2]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define AXI_DMA_OUTLINK_RESTART_CH2    (BIT(2))
#define AXI_DMA_OUTLINK_RESTART_CH2_M  (AXI_DMA_OUTLINK_RESTART_CH2_V << AXI_DMA_OUTLINK_RESTART_CH2_S)
#define AXI_DMA_OUTLINK_RESTART_CH2_V  0x00000001U
#define AXI_DMA_OUTLINK_RESTART_CH2_S  2
/** AXI_DMA_OUTLINK_PARK_CH2 : RO; bitpos: [3]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define AXI_DMA_OUTLINK_PARK_CH2    (BIT(3))
#define AXI_DMA_OUTLINK_PARK_CH2_M  (AXI_DMA_OUTLINK_PARK_CH2_V << AXI_DMA_OUTLINK_PARK_CH2_S)
#define AXI_DMA_OUTLINK_PARK_CH2_V  0x00000001U
#define AXI_DMA_OUTLINK_PARK_CH2_S  3

/** AXI_DMA_OUT_LINK2_CH2_REG register
 *  Link descriptor configure and control register of Tx channel2
 */
#define AXI_DMA_OUT_LINK2_CH2_REG (DR_REG_AXI_DMA_BASE + 0x22c)
/** AXI_DMA_OUTLINK_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first outlink
 *  descriptor's address.
 */
#define AXI_DMA_OUTLINK_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_ADDR_CH2_M  (AXI_DMA_OUTLINK_ADDR_CH2_V << AXI_DMA_OUTLINK_ADDR_CH2_S)
#define AXI_DMA_OUTLINK_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_ADDR_CH2_S  0

/** AXI_DMA_OUT_STATE_CH2_REG register
 *  Transmit status of Tx channel2
 */
#define AXI_DMA_OUT_STATE_CH2_REG (DR_REG_AXI_DMA_BASE + 0x230)
/** AXI_DMA_OUTLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH2_M  (AXI_DMA_OUTLINK_DSCR_ADDR_CH2_V << AXI_DMA_OUTLINK_DSCR_ADDR_CH2_S)
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define AXI_DMA_OUTLINK_DSCR_ADDR_CH2_S  0
/** AXI_DMA_OUT_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_DSCR_STATE_CH2    0x00000003U
#define AXI_DMA_OUT_DSCR_STATE_CH2_M  (AXI_DMA_OUT_DSCR_STATE_CH2_V << AXI_DMA_OUT_DSCR_STATE_CH2_S)
#define AXI_DMA_OUT_DSCR_STATE_CH2_V  0x00000003U
#define AXI_DMA_OUT_DSCR_STATE_CH2_S  18
/** AXI_DMA_OUT_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AXI_DMA_OUT_STATE_CH2    0x00000007U
#define AXI_DMA_OUT_STATE_CH2_M  (AXI_DMA_OUT_STATE_CH2_V << AXI_DMA_OUT_STATE_CH2_S)
#define AXI_DMA_OUT_STATE_CH2_V  0x00000007U
#define AXI_DMA_OUT_STATE_CH2_S  20

/** AXI_DMA_OUT_EOF_DES_ADDR_CH2_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel2
 */
#define AXI_DMA_OUT_EOF_DES_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x234)
/** AXI_DMA_OUT_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AXI_DMA_OUT_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_DES_ADDR_CH2_M  (AXI_DMA_OUT_EOF_DES_ADDR_CH2_V << AXI_DMA_OUT_EOF_DES_ADDR_CH2_S)
#define AXI_DMA_OUT_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_DES_ADDR_CH2_S  0

/** AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel2
 */
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x238)
/** AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2_M  (AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2_V << AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2_S)
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUT_EOF_BFR_DES_ADDR_CH2_S  0

/** AXI_DMA_OUT_DSCR_CH2_REG register
 *  Current outlink descriptor address of Tx channel2
 */
#define AXI_DMA_OUT_DSCR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x23c)
/** AXI_DMA_OUTLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define AXI_DMA_OUTLINK_DSCR_CH2    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_CH2_M  (AXI_DMA_OUTLINK_DSCR_CH2_V << AXI_DMA_OUTLINK_DSCR_CH2_S)
#define AXI_DMA_OUTLINK_DSCR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_CH2_S  0

/** AXI_DMA_OUT_DSCR_BF0_CH2_REG register
 *  The last outlink descriptor address of Tx channel2
 */
#define AXI_DMA_OUT_DSCR_BF0_CH2_REG (DR_REG_AXI_DMA_BASE + 0x240)
/** AXI_DMA_OUTLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define AXI_DMA_OUTLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF0_CH2_M  (AXI_DMA_OUTLINK_DSCR_BF0_CH2_V << AXI_DMA_OUTLINK_DSCR_BF0_CH2_S)
#define AXI_DMA_OUTLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF0_CH2_S  0

/** AXI_DMA_OUT_DSCR_BF1_CH2_REG register
 *  The second-to-last outlink descriptor address of Tx channel2
 */
#define AXI_DMA_OUT_DSCR_BF1_CH2_REG (DR_REG_AXI_DMA_BASE + 0x244)
/** AXI_DMA_OUTLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last outlink descriptor x-2.
 */
#define AXI_DMA_OUTLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF1_CH2_M  (AXI_DMA_OUTLINK_DSCR_BF1_CH2_V << AXI_DMA_OUTLINK_DSCR_BF1_CH2_S)
#define AXI_DMA_OUTLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUTLINK_DSCR_BF1_CH2_S  0

/** AXI_DMA_OUT_PRI_CH2_REG register
 *  Priority register of Tx channel2.
 */
#define AXI_DMA_OUT_PRI_CH2_REG (DR_REG_AXI_DMA_BASE + 0x248)
/** AXI_DMA_TX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel2. The larger of the value the higher of the priority.
 */
#define AXI_DMA_TX_PRI_CH2    0x0000000FU
#define AXI_DMA_TX_PRI_CH2_M  (AXI_DMA_TX_PRI_CH2_V << AXI_DMA_TX_PRI_CH2_S)
#define AXI_DMA_TX_PRI_CH2_V  0x0000000FU
#define AXI_DMA_TX_PRI_CH2_S  0
/** AXI_DMA_TX_CH_ARB_WEIGH_CH2 : R/W; bitpos: [7:4]; default: 0;
 *  The weight of Tx channel2
 */
#define AXI_DMA_TX_CH_ARB_WEIGH_CH2    0x0000000FU
#define AXI_DMA_TX_CH_ARB_WEIGH_CH2_M  (AXI_DMA_TX_CH_ARB_WEIGH_CH2_V << AXI_DMA_TX_CH_ARB_WEIGH_CH2_S)
#define AXI_DMA_TX_CH_ARB_WEIGH_CH2_V  0x0000000FU
#define AXI_DMA_TX_CH_ARB_WEIGH_CH2_S  4
/** AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH2 : R/W; bitpos: [8]; default: 0;
 *  0: mean not optimization weight function ,1: mean optimization
 */
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH2    (BIT(8))
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_M  (AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_V << AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_S)
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_V  0x00000001U
#define AXI_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_S  8

/** AXI_DMA_OUT_PERI_SEL_CH2_REG register
 *  Peripheral selection of Tx channel2
 */
#define AXI_DMA_OUT_PERI_SEL_CH2_REG (DR_REG_AXI_DMA_BASE + 0x24c)
/** AXI_DMA_PERI_OUT_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel2. 0:lcdcam. 1: gpspi_2.
 *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
 */
#define AXI_DMA_PERI_OUT_SEL_CH2    0x0000003FU
#define AXI_DMA_PERI_OUT_SEL_CH2_M  (AXI_DMA_PERI_OUT_SEL_CH2_V << AXI_DMA_PERI_OUT_SEL_CH2_S)
#define AXI_DMA_PERI_OUT_SEL_CH2_V  0x0000003FU
#define AXI_DMA_PERI_OUT_SEL_CH2_S  0

/** AXI_DMA_OUT_CRC_INIT_DATA_CH2_REG register
 *  This register is used to config ch2 crc initial data(max 32 bit)
 */
#define AXI_DMA_OUT_CRC_INIT_DATA_CH2_REG (DR_REG_AXI_DMA_BASE + 0x250)
/** AXI_DMA_OUT_CRC_INIT_DATA_CH2 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch2 of tx crc initial value
 */
#define AXI_DMA_OUT_CRC_INIT_DATA_CH2    0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_INIT_DATA_CH2_M  (AXI_DMA_OUT_CRC_INIT_DATA_CH2_V << AXI_DMA_OUT_CRC_INIT_DATA_CH2_S)
#define AXI_DMA_OUT_CRC_INIT_DATA_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_INIT_DATA_CH2_S  0

/** AXI_DMA_TX_CRC_WIDTH_CH2_REG register
 *  This register is used to config tx ch2 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AXI_DMA_TX_CRC_WIDTH_CH2_REG (DR_REG_AXI_DMA_BASE + 0x254)
/** AXI_DMA_TX_CRC_WIDTH_CH2 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_WIDTH_CH2    0x00000003U
#define AXI_DMA_TX_CRC_WIDTH_CH2_M  (AXI_DMA_TX_CRC_WIDTH_CH2_V << AXI_DMA_TX_CRC_WIDTH_CH2_S)
#define AXI_DMA_TX_CRC_WIDTH_CH2_V  0x00000003U
#define AXI_DMA_TX_CRC_WIDTH_CH2_S  0
/** AXI_DMA_TX_CRC_LATCH_FLAG_CH2 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH2    (BIT(2))
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH2_M  (AXI_DMA_TX_CRC_LATCH_FLAG_CH2_V << AXI_DMA_TX_CRC_LATCH_FLAG_CH2_S)
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH2_V  0x00000001U
#define AXI_DMA_TX_CRC_LATCH_FLAG_CH2_S  2

/** AXI_DMA_OUT_CRC_CLEAR_CH2_REG register
 *  This register is used to clear ch2 crc result
 */
#define AXI_DMA_OUT_CRC_CLEAR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x258)
/** AXI_DMA_OUT_CRC_CLEAR_CH2 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch2 of tx crc result
 */
#define AXI_DMA_OUT_CRC_CLEAR_CH2    (BIT(0))
#define AXI_DMA_OUT_CRC_CLEAR_CH2_M  (AXI_DMA_OUT_CRC_CLEAR_CH2_V << AXI_DMA_OUT_CRC_CLEAR_CH2_S)
#define AXI_DMA_OUT_CRC_CLEAR_CH2_V  0x00000001U
#define AXI_DMA_OUT_CRC_CLEAR_CH2_S  0

/** AXI_DMA_OUT_CRC_FINAL_RESULT_CH2_REG register
 *  This register is used to store ch2 crc result
 */
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH2_REG (DR_REG_AXI_DMA_BASE + 0x25c)
/** AXI_DMA_OUT_CRC_FINAL_RESULT_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch2 of tx
 */
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH2    0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH2_M  (AXI_DMA_OUT_CRC_FINAL_RESULT_CH2_V << AXI_DMA_OUT_CRC_FINAL_RESULT_CH2_S)
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH2_V  0xFFFFFFFFU
#define AXI_DMA_OUT_CRC_FINAL_RESULT_CH2_S  0

/** AXI_DMA_TX_CRC_EN_WR_DATA_CH2_REG register
 *  This resister is used to config ch2 crc en for every bit
 */
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH2_REG (DR_REG_AXI_DMA_BASE + 0x260)
/** AXI_DMA_TX_CRC_EN_WR_DATA_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable tx ch2 crc 32bit on/off
 */
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH2    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH2_M  (AXI_DMA_TX_CRC_EN_WR_DATA_CH2_V << AXI_DMA_TX_CRC_EN_WR_DATA_CH2_S)
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH2_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_WR_DATA_CH2_S  0

/** AXI_DMA_TX_CRC_EN_ADDR_CH2_REG register
 *  This register is used to config ch2 crc en addr
 */
#define AXI_DMA_TX_CRC_EN_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x264)
/** AXI_DMA_TX_CRC_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_EN_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_ADDR_CH2_M  (AXI_DMA_TX_CRC_EN_ADDR_CH2_V << AXI_DMA_TX_CRC_EN_ADDR_CH2_S)
#define AXI_DMA_TX_CRC_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_EN_ADDR_CH2_S  0

/** AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_REG register
 *  This register is used to config crc data_8bit en
 */
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_REG (DR_REG_AXI_DMA_BASE + 0x268)
/** AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2 : R/W; bitpos: [15:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2    0x0000FFFFU
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_M  (AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_V << AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_S)
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_V  0x0000FFFFU
#define AXI_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_S  0

/** AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2_REG (DR_REG_AXI_DMA_BASE + 0x26c)
/** AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2    0xFFFFFFFFU
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2_M  (AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2_V << AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2_S)
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AXI_DMA_TX_CRC_DATA_EN_ADDR_CH2_S  0

/** AXI_DMA_ARB_TIMEOUT_REG register
 *  This retister is used to config arbiter time slice
 */
#define AXI_DMA_ARB_TIMEOUT_REG (DR_REG_AXI_DMA_BASE + 0x270)
/** AXI_DMA_ARB_TIMEOUT_TX : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to config tx arbiter time out value
 */
#define AXI_DMA_ARB_TIMEOUT_TX    0x0000FFFFU
#define AXI_DMA_ARB_TIMEOUT_TX_M  (AXI_DMA_ARB_TIMEOUT_TX_V << AXI_DMA_ARB_TIMEOUT_TX_S)
#define AXI_DMA_ARB_TIMEOUT_TX_V  0x0000FFFFU
#define AXI_DMA_ARB_TIMEOUT_TX_S  0
/** AXI_DMA_ARB_TIMEOUT_RX : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to config rx arbiter time out value
 */
#define AXI_DMA_ARB_TIMEOUT_RX    0x0000FFFFU
#define AXI_DMA_ARB_TIMEOUT_RX_M  (AXI_DMA_ARB_TIMEOUT_RX_V << AXI_DMA_ARB_TIMEOUT_RX_S)
#define AXI_DMA_ARB_TIMEOUT_RX_V  0x0000FFFFU
#define AXI_DMA_ARB_TIMEOUT_RX_S  16

/** AXI_DMA_WEIGHT_EN_REG register
 *  This register is used to config arbiter weight function to on or off
 */
#define AXI_DMA_WEIGHT_EN_REG (DR_REG_AXI_DMA_BASE + 0x274)
/** AXI_DMA_WEIGHT_EN_TX : R/W; bitpos: [0]; default: 0;
 *  This register is used to config tx arbiter weight function  off/on
 */
#define AXI_DMA_WEIGHT_EN_TX    (BIT(0))
#define AXI_DMA_WEIGHT_EN_TX_M  (AXI_DMA_WEIGHT_EN_TX_V << AXI_DMA_WEIGHT_EN_TX_S)
#define AXI_DMA_WEIGHT_EN_TX_V  0x00000001U
#define AXI_DMA_WEIGHT_EN_TX_S  0
/** AXI_DMA_WEIGHT_EN_RX : R/W; bitpos: [1]; default: 0;
 *  This register is used to config rx arbiter weight function  off/on
 */
#define AXI_DMA_WEIGHT_EN_RX    (BIT(1))
#define AXI_DMA_WEIGHT_EN_RX_M  (AXI_DMA_WEIGHT_EN_RX_V << AXI_DMA_WEIGHT_EN_RX_S)
#define AXI_DMA_WEIGHT_EN_RX_V  0x00000001U
#define AXI_DMA_WEIGHT_EN_RX_S  1

/** AXI_DMA_IN_MEM_CONF_REG register
 *  Mem power configure register of Rx channel
 */
#define AXI_DMA_IN_MEM_CONF_REG (DR_REG_AXI_DMA_BASE + 0x278)
/** AXI_DMA_IN_MEM_CLK_FORCE_EN : R/W; bitpos: [0]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in
 *  AXI_DMA. 0: A gate-clock will be used when accessing the RAM in AXI_DMA.
 */
#define AXI_DMA_IN_MEM_CLK_FORCE_EN    (BIT(0))
#define AXI_DMA_IN_MEM_CLK_FORCE_EN_M  (AXI_DMA_IN_MEM_CLK_FORCE_EN_V << AXI_DMA_IN_MEM_CLK_FORCE_EN_S)
#define AXI_DMA_IN_MEM_CLK_FORCE_EN_V  0x00000001U
#define AXI_DMA_IN_MEM_CLK_FORCE_EN_S  0
/** AXI_DMA_IN_MEM_FORCE_PU : R/W; bitpos: [1]; default: 0;
 *  Force power up ram
 */
#define AXI_DMA_IN_MEM_FORCE_PU    (BIT(1))
#define AXI_DMA_IN_MEM_FORCE_PU_M  (AXI_DMA_IN_MEM_FORCE_PU_V << AXI_DMA_IN_MEM_FORCE_PU_S)
#define AXI_DMA_IN_MEM_FORCE_PU_V  0x00000001U
#define AXI_DMA_IN_MEM_FORCE_PU_S  1
/** AXI_DMA_IN_MEM_FORCE_PD : R/W; bitpos: [2]; default: 0;
 *  Force power down ram
 */
#define AXI_DMA_IN_MEM_FORCE_PD    (BIT(2))
#define AXI_DMA_IN_MEM_FORCE_PD_M  (AXI_DMA_IN_MEM_FORCE_PD_V << AXI_DMA_IN_MEM_FORCE_PD_S)
#define AXI_DMA_IN_MEM_FORCE_PD_V  0x00000001U
#define AXI_DMA_IN_MEM_FORCE_PD_S  2
/** AXI_DMA_OUT_MEM_CLK_FORCE_EN : R/W; bitpos: [3]; default: 0;
 *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in
 *  AXI_DMA. 0: A gate-clock will be used when accessing the RAM in AXI_DMA.
 */
#define AXI_DMA_OUT_MEM_CLK_FORCE_EN    (BIT(3))
#define AXI_DMA_OUT_MEM_CLK_FORCE_EN_M  (AXI_DMA_OUT_MEM_CLK_FORCE_EN_V << AXI_DMA_OUT_MEM_CLK_FORCE_EN_S)
#define AXI_DMA_OUT_MEM_CLK_FORCE_EN_V  0x00000001U
#define AXI_DMA_OUT_MEM_CLK_FORCE_EN_S  3
/** AXI_DMA_OUT_MEM_FORCE_PU : R/W; bitpos: [4]; default: 0;
 *  Force power up ram
 */
#define AXI_DMA_OUT_MEM_FORCE_PU    (BIT(4))
#define AXI_DMA_OUT_MEM_FORCE_PU_M  (AXI_DMA_OUT_MEM_FORCE_PU_V << AXI_DMA_OUT_MEM_FORCE_PU_S)
#define AXI_DMA_OUT_MEM_FORCE_PU_V  0x00000001U
#define AXI_DMA_OUT_MEM_FORCE_PU_S  4
/** AXI_DMA_OUT_MEM_FORCE_PD : R/W; bitpos: [5]; default: 0;
 *  Force power down ram
 */
#define AXI_DMA_OUT_MEM_FORCE_PD    (BIT(5))
#define AXI_DMA_OUT_MEM_FORCE_PD_M  (AXI_DMA_OUT_MEM_FORCE_PD_V << AXI_DMA_OUT_MEM_FORCE_PD_S)
#define AXI_DMA_OUT_MEM_FORCE_PD_V  0x00000001U
#define AXI_DMA_OUT_MEM_FORCE_PD_S  5

/** AXI_DMA_INTR_MEM_START_ADDR_REG register
 *  The start address of accessible address space.
 */
#define AXI_DMA_INTR_MEM_START_ADDR_REG (DR_REG_AXI_DMA_BASE + 0x27c)
/** AXI_DMA_ACCESS_INTR_MEM_START_ADDR : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define AXI_DMA_ACCESS_INTR_MEM_START_ADDR    0xFFFFFFFFU
#define AXI_DMA_ACCESS_INTR_MEM_START_ADDR_M  (AXI_DMA_ACCESS_INTR_MEM_START_ADDR_V << AXI_DMA_ACCESS_INTR_MEM_START_ADDR_S)
#define AXI_DMA_ACCESS_INTR_MEM_START_ADDR_V  0xFFFFFFFFU
#define AXI_DMA_ACCESS_INTR_MEM_START_ADDR_S  0

/** AXI_DMA_INTR_MEM_END_ADDR_REG register
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define AXI_DMA_INTR_MEM_END_ADDR_REG (DR_REG_AXI_DMA_BASE + 0x280)
/** AXI_DMA_ACCESS_INTR_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define AXI_DMA_ACCESS_INTR_MEM_END_ADDR    0xFFFFFFFFU
#define AXI_DMA_ACCESS_INTR_MEM_END_ADDR_M  (AXI_DMA_ACCESS_INTR_MEM_END_ADDR_V << AXI_DMA_ACCESS_INTR_MEM_END_ADDR_S)
#define AXI_DMA_ACCESS_INTR_MEM_END_ADDR_V  0xFFFFFFFFU
#define AXI_DMA_ACCESS_INTR_MEM_END_ADDR_S  0

/** AXI_DMA_EXTR_MEM_START_ADDR_REG register
 *  The start address of accessible address space.
 */
#define AXI_DMA_EXTR_MEM_START_ADDR_REG (DR_REG_AXI_DMA_BASE + 0x284)
/** AXI_DMA_ACCESS_EXTR_MEM_START_ADDR : R/W; bitpos: [31:0]; default: 806354944;
 *  The start address of accessible address space.
 */
#define AXI_DMA_ACCESS_EXTR_MEM_START_ADDR    0xFFFFFFFFU
#define AXI_DMA_ACCESS_EXTR_MEM_START_ADDR_M  (AXI_DMA_ACCESS_EXTR_MEM_START_ADDR_V << AXI_DMA_ACCESS_EXTR_MEM_START_ADDR_S)
#define AXI_DMA_ACCESS_EXTR_MEM_START_ADDR_V  0xFFFFFFFFU
#define AXI_DMA_ACCESS_EXTR_MEM_START_ADDR_S  0

/** AXI_DMA_EXTR_MEM_END_ADDR_REG register
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define AXI_DMA_EXTR_MEM_END_ADDR_REG (DR_REG_AXI_DMA_BASE + 0x288)
/** AXI_DMA_ACCESS_EXTR_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 2415919103;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define AXI_DMA_ACCESS_EXTR_MEM_END_ADDR    0xFFFFFFFFU
#define AXI_DMA_ACCESS_EXTR_MEM_END_ADDR_M  (AXI_DMA_ACCESS_EXTR_MEM_END_ADDR_V << AXI_DMA_ACCESS_EXTR_MEM_END_ADDR_S)
#define AXI_DMA_ACCESS_EXTR_MEM_END_ADDR_V  0xFFFFFFFFU
#define AXI_DMA_ACCESS_EXTR_MEM_END_ADDR_S  0

/** AXI_DMA_IN_RESET_AVAIL_CH0_REG register
 *  The rx channel 0 reset valid_flag register.
 */
#define AXI_DMA_IN_RESET_AVAIL_CH0_REG (DR_REG_AXI_DMA_BASE + 0x28c)
/** AXI_DMA_IN_RESET_AVAIL_CH0 : RO; bitpos: [0]; default: 1;
 *  rx chan0 reset valid reg.
 */
#define AXI_DMA_IN_RESET_AVAIL_CH0    (BIT(0))
#define AXI_DMA_IN_RESET_AVAIL_CH0_M  (AXI_DMA_IN_RESET_AVAIL_CH0_V << AXI_DMA_IN_RESET_AVAIL_CH0_S)
#define AXI_DMA_IN_RESET_AVAIL_CH0_V  0x00000001U
#define AXI_DMA_IN_RESET_AVAIL_CH0_S  0

/** AXI_DMA_IN_RESET_AVAIL_CH1_REG register
 *  The rx channel 1 reset valid_flag register.
 */
#define AXI_DMA_IN_RESET_AVAIL_CH1_REG (DR_REG_AXI_DMA_BASE + 0x290)
/** AXI_DMA_IN_RESET_AVAIL_CH1 : RO; bitpos: [0]; default: 1;
 *  rx chan1 reset valid reg.
 */
#define AXI_DMA_IN_RESET_AVAIL_CH1    (BIT(0))
#define AXI_DMA_IN_RESET_AVAIL_CH1_M  (AXI_DMA_IN_RESET_AVAIL_CH1_V << AXI_DMA_IN_RESET_AVAIL_CH1_S)
#define AXI_DMA_IN_RESET_AVAIL_CH1_V  0x00000001U
#define AXI_DMA_IN_RESET_AVAIL_CH1_S  0

/** AXI_DMA_IN_RESET_AVAIL_CH2_REG register
 *  The rx channel 2 reset valid_flag register.
 */
#define AXI_DMA_IN_RESET_AVAIL_CH2_REG (DR_REG_AXI_DMA_BASE + 0x294)
/** AXI_DMA_IN_RESET_AVAIL_CH2 : RO; bitpos: [0]; default: 1;
 *  rx chan2 reset valid reg.
 */
#define AXI_DMA_IN_RESET_AVAIL_CH2    (BIT(0))
#define AXI_DMA_IN_RESET_AVAIL_CH2_M  (AXI_DMA_IN_RESET_AVAIL_CH2_V << AXI_DMA_IN_RESET_AVAIL_CH2_S)
#define AXI_DMA_IN_RESET_AVAIL_CH2_V  0x00000001U
#define AXI_DMA_IN_RESET_AVAIL_CH2_S  0

/** AXI_DMA_OUT_RESET_AVAIL_CH0_REG register
 *  The tx channel 0 reset valid_flag register.
 */
#define AXI_DMA_OUT_RESET_AVAIL_CH0_REG (DR_REG_AXI_DMA_BASE + 0x298)
/** AXI_DMA_OUT_RESET_AVAIL_CH0 : RO; bitpos: [0]; default: 1;
 *  tx chan0 reset valid reg.
 */
#define AXI_DMA_OUT_RESET_AVAIL_CH0    (BIT(0))
#define AXI_DMA_OUT_RESET_AVAIL_CH0_M  (AXI_DMA_OUT_RESET_AVAIL_CH0_V << AXI_DMA_OUT_RESET_AVAIL_CH0_S)
#define AXI_DMA_OUT_RESET_AVAIL_CH0_V  0x00000001U
#define AXI_DMA_OUT_RESET_AVAIL_CH0_S  0

/** AXI_DMA_OUT_RESET_AVAIL_CH1_REG register
 *  The tx channel 1 reset valid_flag register.
 */
#define AXI_DMA_OUT_RESET_AVAIL_CH1_REG (DR_REG_AXI_DMA_BASE + 0x29c)
/** AXI_DMA_OUT_RESET_AVAIL_CH1 : RO; bitpos: [0]; default: 1;
 *  tx chan1 reset valid reg.
 */
#define AXI_DMA_OUT_RESET_AVAIL_CH1    (BIT(0))
#define AXI_DMA_OUT_RESET_AVAIL_CH1_M  (AXI_DMA_OUT_RESET_AVAIL_CH1_V << AXI_DMA_OUT_RESET_AVAIL_CH1_S)
#define AXI_DMA_OUT_RESET_AVAIL_CH1_V  0x00000001U
#define AXI_DMA_OUT_RESET_AVAIL_CH1_S  0

/** AXI_DMA_OUT_RESET_AVAIL_CH2_REG register
 *  The tx channel 2 reset valid_flag register.
 */
#define AXI_DMA_OUT_RESET_AVAIL_CH2_REG (DR_REG_AXI_DMA_BASE + 0x2a0)
/** AXI_DMA_OUT_RESET_AVAIL_CH2 : RO; bitpos: [0]; default: 1;
 *  tx chan2 reset valid reg.
 */
#define AXI_DMA_OUT_RESET_AVAIL_CH2    (BIT(0))
#define AXI_DMA_OUT_RESET_AVAIL_CH2_M  (AXI_DMA_OUT_RESET_AVAIL_CH2_V << AXI_DMA_OUT_RESET_AVAIL_CH2_S)
#define AXI_DMA_OUT_RESET_AVAIL_CH2_V  0x00000001U
#define AXI_DMA_OUT_RESET_AVAIL_CH2_S  0

/** AXI_DMA_MISC_CONF_REG register
 *  MISC register
 */
#define AXI_DMA_MISC_CONF_REG (DR_REG_AXI_DMA_BASE + 0x2a8)
/** AXI_DMA_AXIM_RST_WR_INTER : R/W; bitpos: [0]; default: 0;
 *  Set this bit then clear this bit to reset the internal axi_wr FSM.
 */
#define AXI_DMA_AXIM_RST_WR_INTER    (BIT(0))
#define AXI_DMA_AXIM_RST_WR_INTER_M  (AXI_DMA_AXIM_RST_WR_INTER_V << AXI_DMA_AXIM_RST_WR_INTER_S)
#define AXI_DMA_AXIM_RST_WR_INTER_V  0x00000001U
#define AXI_DMA_AXIM_RST_WR_INTER_S  0
/** AXI_DMA_AXIM_RST_RD_INTER : R/W; bitpos: [1]; default: 0;
 *  Set this bit then clear this bit to reset the internal axi_rd FSM.
 */
#define AXI_DMA_AXIM_RST_RD_INTER    (BIT(1))
#define AXI_DMA_AXIM_RST_RD_INTER_M  (AXI_DMA_AXIM_RST_RD_INTER_V << AXI_DMA_AXIM_RST_RD_INTER_S)
#define AXI_DMA_AXIM_RST_RD_INTER_V  0x00000001U
#define AXI_DMA_AXIM_RST_RD_INTER_S  1
/** AXI_DMA_ARB_PRI_DIS : R/W; bitpos: [3]; default: 0;
 *  Set this bit to disable priority arbitration function.
 */
#define AXI_DMA_ARB_PRI_DIS    (BIT(3))
#define AXI_DMA_ARB_PRI_DIS_M  (AXI_DMA_ARB_PRI_DIS_V << AXI_DMA_ARB_PRI_DIS_S)
#define AXI_DMA_ARB_PRI_DIS_V  0x00000001U
#define AXI_DMA_ARB_PRI_DIS_S  3
/** AXI_DMA_CLK_EN : R/W; bitpos: [4]; default: 0;
 *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
 *  registers.
 */
#define AXI_DMA_CLK_EN    (BIT(4))
#define AXI_DMA_CLK_EN_M  (AXI_DMA_CLK_EN_V << AXI_DMA_CLK_EN_S)
#define AXI_DMA_CLK_EN_V  0x00000001U
#define AXI_DMA_CLK_EN_S  4

/** AXI_DMA_RDN_RESULT_REG register
 *  reserved
 */
#define AXI_DMA_RDN_RESULT_REG (DR_REG_AXI_DMA_BASE + 0x2ac)
/** AXI_DMA_RDN_ENA : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define AXI_DMA_RDN_ENA    (BIT(0))
#define AXI_DMA_RDN_ENA_M  (AXI_DMA_RDN_ENA_V << AXI_DMA_RDN_ENA_S)
#define AXI_DMA_RDN_ENA_V  0x00000001U
#define AXI_DMA_RDN_ENA_S  0
/** AXI_DMA_RDN_RESULT : RO; bitpos: [1]; default: 0;
 *  reserved
 */
#define AXI_DMA_RDN_RESULT    (BIT(1))
#define AXI_DMA_RDN_RESULT_M  (AXI_DMA_RDN_RESULT_V << AXI_DMA_RDN_RESULT_S)
#define AXI_DMA_RDN_RESULT_V  0x00000001U
#define AXI_DMA_RDN_RESULT_S  1

/** AXI_DMA_RDN_ECO_HIGH_REG register
 *  reserved
 */
#define AXI_DMA_RDN_ECO_HIGH_REG (DR_REG_AXI_DMA_BASE + 0x2b0)
/** AXI_DMA_RDN_ECO_HIGH : R/W; bitpos: [31:0]; default: 4294967295;
 *  The start address of accessible address space.
 */
#define AXI_DMA_RDN_ECO_HIGH    0xFFFFFFFFU
#define AXI_DMA_RDN_ECO_HIGH_M  (AXI_DMA_RDN_ECO_HIGH_V << AXI_DMA_RDN_ECO_HIGH_S)
#define AXI_DMA_RDN_ECO_HIGH_V  0xFFFFFFFFU
#define AXI_DMA_RDN_ECO_HIGH_S  0

/** AXI_DMA_RDN_ECO_LOW_REG register
 *  reserved
 */
#define AXI_DMA_RDN_ECO_LOW_REG (DR_REG_AXI_DMA_BASE + 0x2b4)
/** AXI_DMA_RDN_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  The start address of accessible address space.
 */
#define AXI_DMA_RDN_ECO_LOW    0xFFFFFFFFU
#define AXI_DMA_RDN_ECO_LOW_M  (AXI_DMA_RDN_ECO_LOW_V << AXI_DMA_RDN_ECO_LOW_S)
#define AXI_DMA_RDN_ECO_LOW_V  0xFFFFFFFFU
#define AXI_DMA_RDN_ECO_LOW_S  0

/** AXI_DMA_WRESP_CNT_REG register
 *  AXI wr response cnt register.
 */
#define AXI_DMA_WRESP_CNT_REG (DR_REG_AXI_DMA_BASE + 0x2b8)
/** AXI_DMA_WRESP_CNT : RO; bitpos: [3:0]; default: 0;
 *  axi wr response cnt reg.
 */
#define AXI_DMA_WRESP_CNT    0x0000000FU
#define AXI_DMA_WRESP_CNT_M  (AXI_DMA_WRESP_CNT_V << AXI_DMA_WRESP_CNT_S)
#define AXI_DMA_WRESP_CNT_V  0x0000000FU
#define AXI_DMA_WRESP_CNT_S  0

/** AXI_DMA_RRESP_CNT_REG register
 *  AXI wr response cnt register.
 */
#define AXI_DMA_RRESP_CNT_REG (DR_REG_AXI_DMA_BASE + 0x2bc)
/** AXI_DMA_RRESP_CNT : RO; bitpos: [3:0]; default: 0;
 *  axi rd response cnt reg.
 */
#define AXI_DMA_RRESP_CNT    0x0000000FU
#define AXI_DMA_RRESP_CNT_M  (AXI_DMA_RRESP_CNT_V << AXI_DMA_RRESP_CNT_S)
#define AXI_DMA_RRESP_CNT_V  0x0000000FU
#define AXI_DMA_RRESP_CNT_S  0

/** AXI_DMA_INFIFO_STATUS1_CH0_REG register
 *  Receive FIFO status of Rx channel 0
 */
#define AXI_DMA_INFIFO_STATUS1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x2c0)
/** AXI_DMA_L1INFIFO_CNT_CH0 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.
 */
#define AXI_DMA_L1INFIFO_CNT_CH0    0x0000003FU
#define AXI_DMA_L1INFIFO_CNT_CH0_M  (AXI_DMA_L1INFIFO_CNT_CH0_V << AXI_DMA_L1INFIFO_CNT_CH0_S)
#define AXI_DMA_L1INFIFO_CNT_CH0_V  0x0000003FU
#define AXI_DMA_L1INFIFO_CNT_CH0_S  0
/** AXI_DMA_L2INFIFO_CNT_CH0 : RO; bitpos: [9:6]; default: 0;
 *  The register stores the byte number of the data in L2 Rx FIFO for Rx channel 0.
 */
#define AXI_DMA_L2INFIFO_CNT_CH0    0x0000000FU
#define AXI_DMA_L2INFIFO_CNT_CH0_M  (AXI_DMA_L2INFIFO_CNT_CH0_V << AXI_DMA_L2INFIFO_CNT_CH0_S)
#define AXI_DMA_L2INFIFO_CNT_CH0_V  0x0000000FU
#define AXI_DMA_L2INFIFO_CNT_CH0_S  6

/** AXI_DMA_INFIFO_STATUS1_CH1_REG register
 *  Receive FIFO status of Rx channel 1
 */
#define AXI_DMA_INFIFO_STATUS1_CH1_REG (DR_REG_AXI_DMA_BASE + 0x2c4)
/** AXI_DMA_L1INFIFO_CNT_CH1 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 1.
 */
#define AXI_DMA_L1INFIFO_CNT_CH1    0x0000003FU
#define AXI_DMA_L1INFIFO_CNT_CH1_M  (AXI_DMA_L1INFIFO_CNT_CH1_V << AXI_DMA_L1INFIFO_CNT_CH1_S)
#define AXI_DMA_L1INFIFO_CNT_CH1_V  0x0000003FU
#define AXI_DMA_L1INFIFO_CNT_CH1_S  0
/** AXI_DMA_L2INFIFO_CNT_CH1 : RO; bitpos: [9:6]; default: 0;
 *  The register stores the byte number of the data in L2 Rx FIFO for Rx channel 1.
 */
#define AXI_DMA_L2INFIFO_CNT_CH1    0x0000000FU
#define AXI_DMA_L2INFIFO_CNT_CH1_M  (AXI_DMA_L2INFIFO_CNT_CH1_V << AXI_DMA_L2INFIFO_CNT_CH1_S)
#define AXI_DMA_L2INFIFO_CNT_CH1_V  0x0000000FU
#define AXI_DMA_L2INFIFO_CNT_CH1_S  6

/** AXI_DMA_INFIFO_STATUS1_CH2_REG register
 *  Receive FIFO status of Rx channel 1
 */
#define AXI_DMA_INFIFO_STATUS1_CH2_REG (DR_REG_AXI_DMA_BASE + 0x2c8)
/** AXI_DMA_L1INFIFO_CNT_CH2 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 2.
 */
#define AXI_DMA_L1INFIFO_CNT_CH2    0x0000003FU
#define AXI_DMA_L1INFIFO_CNT_CH2_M  (AXI_DMA_L1INFIFO_CNT_CH2_V << AXI_DMA_L1INFIFO_CNT_CH2_S)
#define AXI_DMA_L1INFIFO_CNT_CH2_V  0x0000003FU
#define AXI_DMA_L1INFIFO_CNT_CH2_S  0
/** AXI_DMA_L2INFIFO_CNT_CH2 : RO; bitpos: [9:6]; default: 0;
 *  The register stores the byte number of the data in L2 Rx FIFO for Rx channel 2.
 */
#define AXI_DMA_L2INFIFO_CNT_CH2    0x0000000FU
#define AXI_DMA_L2INFIFO_CNT_CH2_M  (AXI_DMA_L2INFIFO_CNT_CH2_V << AXI_DMA_L2INFIFO_CNT_CH2_S)
#define AXI_DMA_L2INFIFO_CNT_CH2_V  0x0000000FU
#define AXI_DMA_L2INFIFO_CNT_CH2_S  6

/** AXI_DMA_OUTFIFO_STATUS1_CH0_REG register
 *  Receive FIFO status of Tx channel 0
 */
#define AXI_DMA_OUTFIFO_STATUS1_CH0_REG (DR_REG_AXI_DMA_BASE + 0x2cc)
/** AXI_DMA_L1OUTFIFO_CNT_CH0 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.
 */
#define AXI_DMA_L1OUTFIFO_CNT_CH0    0x0000003FU
#define AXI_DMA_L1OUTFIFO_CNT_CH0_M  (AXI_DMA_L1OUTFIFO_CNT_CH0_V << AXI_DMA_L1OUTFIFO_CNT_CH0_S)
#define AXI_DMA_L1OUTFIFO_CNT_CH0_V  0x0000003FU
#define AXI_DMA_L1OUTFIFO_CNT_CH0_S  0
/** AXI_DMA_L2OUTFIFO_CNT_CH0 : RO; bitpos: [9:6]; default: 0;
 *  The register stores the byte number of the data in L2 Tx FIFO for Tx channel 0.
 */
#define AXI_DMA_L2OUTFIFO_CNT_CH0    0x0000000FU
#define AXI_DMA_L2OUTFIFO_CNT_CH0_M  (AXI_DMA_L2OUTFIFO_CNT_CH0_V << AXI_DMA_L2OUTFIFO_CNT_CH0_S)
#define AXI_DMA_L2OUTFIFO_CNT_CH0_V  0x0000000FU
#define AXI_DMA_L2OUTFIFO_CNT_CH0_S  6

/** AXI_DMA_OUTFIFO_STATUS1_CH1_REG register
 *  Receive FIFO status of Tx channel 1
 */
#define AXI_DMA_OUTFIFO_STATUS1_CH1_REG (DR_REG_AXI_DMA_BASE + 0x2d0)
/** AXI_DMA_L1OUTFIFO_CNT_CH1 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 1.
 */
#define AXI_DMA_L1OUTFIFO_CNT_CH1    0x0000003FU
#define AXI_DMA_L1OUTFIFO_CNT_CH1_M  (AXI_DMA_L1OUTFIFO_CNT_CH1_V << AXI_DMA_L1OUTFIFO_CNT_CH1_S)
#define AXI_DMA_L1OUTFIFO_CNT_CH1_V  0x0000003FU
#define AXI_DMA_L1OUTFIFO_CNT_CH1_S  0
/** AXI_DMA_L2OUTFIFO_CNT_CH1 : RO; bitpos: [9:6]; default: 0;
 *  The register stores the byte number of the data in L2 Tx FIFO for Tx channel 1.
 */
#define AXI_DMA_L2OUTFIFO_CNT_CH1    0x0000000FU
#define AXI_DMA_L2OUTFIFO_CNT_CH1_M  (AXI_DMA_L2OUTFIFO_CNT_CH1_V << AXI_DMA_L2OUTFIFO_CNT_CH1_S)
#define AXI_DMA_L2OUTFIFO_CNT_CH1_V  0x0000000FU
#define AXI_DMA_L2OUTFIFO_CNT_CH1_S  6

/** AXI_DMA_OUTFIFO_STATUS1_CH2_REG register
 *  Receive FIFO status of Tx channel 1
 */
#define AXI_DMA_OUTFIFO_STATUS1_CH2_REG (DR_REG_AXI_DMA_BASE + 0x2d4)
/** AXI_DMA_L1OUTFIFO_CNT_CH2 : RO; bitpos: [5:0]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 2.
 */
#define AXI_DMA_L1OUTFIFO_CNT_CH2    0x0000003FU
#define AXI_DMA_L1OUTFIFO_CNT_CH2_M  (AXI_DMA_L1OUTFIFO_CNT_CH2_V << AXI_DMA_L1OUTFIFO_CNT_CH2_S)
#define AXI_DMA_L1OUTFIFO_CNT_CH2_V  0x0000003FU
#define AXI_DMA_L1OUTFIFO_CNT_CH2_S  0
/** AXI_DMA_L2OUTFIFO_CNT_CH2 : RO; bitpos: [9:6]; default: 0;
 *  The register stores the byte number of the data in L2 Tx FIFO for Tx channel 2.
 */
#define AXI_DMA_L2OUTFIFO_CNT_CH2    0x0000000FU
#define AXI_DMA_L2OUTFIFO_CNT_CH2_M  (AXI_DMA_L2OUTFIFO_CNT_CH2_V << AXI_DMA_L2OUTFIFO_CNT_CH2_S)
#define AXI_DMA_L2OUTFIFO_CNT_CH2_V  0x0000000FU
#define AXI_DMA_L2OUTFIFO_CNT_CH2_S  6

/** AXI_DMA_DATE_REG register
 *  Version control register
 */
#define AXI_DMA_DATE_REG (DR_REG_AXI_DMA_BASE + 0x2d8)
/** AXI_DMA_DATE : R/W; bitpos: [31:0]; default: 36712768;
 *  register version.
 */
#define AXI_DMA_DATE    0xFFFFFFFFU
#define AXI_DMA_DATE_M  (AXI_DMA_DATE_V << AXI_DMA_DATE_S)
#define AXI_DMA_DATE_V  0xFFFFFFFFU
#define AXI_DMA_DATE_S  0

#ifdef __cplusplus
}
#endif
