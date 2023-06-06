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

/** GDMA_INT_RAW_CH0_REG register
 *  Raw status interrupt of RX channel 0
 */
#define GDMA_INT_RAW_CH0_REG (DR_REG_GDMA_BASE + 0x0)
/** GDMA_IN_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one receive
 *  descriptor has been received for RX channel 0.
 */
#define GDMA_IN_DONE_CH0_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_RAW_M  (GDMA_IN_DONE_CH0_INT_RAW_V << GDMA_IN_DONE_CH0_INT_RAW_S)
#define GDMA_IN_DONE_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_RAW_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level for RX channel 0 when the last data
 *  pointed by one receive descriptor has been received and the suc_eof bit in this
 *  descriptor is 1. For UHCI0, the raw interrupt bit turns to high level when the last
 *  data pointed by one receive descriptor has been received and no data error is
 *  detected for RX channel 0.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_M  (GDMA_IN_SUC_EOF_CH0_INT_RAW_V << GDMA_IN_SUC_EOF_CH0_INT_RAW_S)
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for RX channel 0. For other peripherals, this raw
 *  interrupt is reserved.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_M  (GDMA_IN_ERR_EOF_CH0_INT_RAW_V << GDMA_IN_ERR_EOF_CH0_INT_RAW_S)
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_S  2
/** GDMA_OUT_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one
 *  transmit descriptor has been transmitted to peripherals for TX channel 0.
 */
#define GDMA_OUT_DONE_CH0_INT_RAW    (BIT(3))
#define GDMA_OUT_DONE_CH0_INT_RAW_M  (GDMA_OUT_DONE_CH0_INT_RAW_V << GDMA_OUT_DONE_CH0_INT_RAW_S)
#define GDMA_OUT_DONE_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_RAW_S  3
/** GDMA_OUT_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one
 *  transmit descriptor has been read from memory for TX channel 0.
 */
#define GDMA_OUT_EOF_CH0_INT_RAW    (BIT(4))
#define GDMA_OUT_EOF_CH0_INT_RAW_M  (GDMA_OUT_EOF_CH0_INT_RAW_V << GDMA_OUT_EOF_CH0_INT_RAW_S)
#define GDMA_OUT_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_RAW_S  4
/** GDMA_IN_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when detecting receive descriptor error,
 *  including owner error, the second and third word error of receive descriptor for RX
 *  channel 0.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_M  (GDMA_IN_DSCR_ERR_CH0_INT_RAW_V << GDMA_IN_DSCR_ERR_CH0_INT_RAW_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_S  5
/** GDMA_OUT_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when detecting transmit descriptor error,
 *  including owner error, the second and third word error of transmit descriptor for
 *  TX channel 0.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_M  (GDMA_OUT_DSCR_ERR_CH0_INT_RAW_V << GDMA_OUT_DSCR_ERR_CH0_INT_RAW_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_S  6
/** GDMA_IN_DSCR_EMPTY_CH0_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when RX buffer pointed by inlink is full
 *  and receiving data is not completed, but there is no more inlink for RX channel 0.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_V << GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_S  7
/** GDMA_OUT_TOTAL_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one descriptor or few descriptors) is transmitted out for TX channel 0.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_V << GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_S  8
/** GDMA_INFIFO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of RX channel 0 is
 *  overflow.
 */
#define GDMA_INFIFO_OVF_CH0_INT_RAW    (BIT(9))
#define GDMA_INFIFO_OVF_CH0_INT_RAW_M  (GDMA_INFIFO_OVF_CH0_INT_RAW_V << GDMA_INFIFO_OVF_CH0_INT_RAW_S)
#define GDMA_INFIFO_OVF_CH0_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_RAW_S  9
/** GDMA_INFIFO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of RX channel 0 is
 *  underflow.
 */
#define GDMA_INFIFO_UDF_CH0_INT_RAW    (BIT(10))
#define GDMA_INFIFO_UDF_CH0_INT_RAW_M  (GDMA_INFIFO_UDF_CH0_INT_RAW_V << GDMA_INFIFO_UDF_CH0_INT_RAW_S)
#define GDMA_INFIFO_UDF_CH0_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_RAW_S  10
/** GDMA_OUTFIFO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of TX channel 0 is
 *  overflow.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW_M  (GDMA_OUTFIFO_OVF_CH0_INT_RAW_V << GDMA_OUTFIFO_OVF_CH0_INT_RAW_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW_S  11
/** GDMA_OUTFIFO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of TX channel 0 is
 *  underflow.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW_M  (GDMA_OUTFIFO_UDF_CH0_INT_RAW_V << GDMA_OUTFIFO_UDF_CH0_INT_RAW_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW_S  12

/** GDMA_INT_ST_CH0_REG register
 *  Masked interrupt of RX channel 0
 */
#define GDMA_INT_ST_CH0_REG (DR_REG_GDMA_BASE + 0x4)
/** GDMA_IN_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH0_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ST_M  (GDMA_IN_DONE_CH0_INT_ST_V << GDMA_IN_DONE_CH0_INT_ST_S)
#define GDMA_IN_DONE_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_ST_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ST_M  (GDMA_IN_SUC_EOF_CH0_INT_ST_V << GDMA_IN_SUC_EOF_CH0_INT_ST_S)
#define GDMA_IN_SUC_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_ST_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ST_M  (GDMA_IN_ERR_EOF_CH0_INT_ST_V << GDMA_IN_ERR_EOF_CH0_INT_ST_S)
#define GDMA_IN_ERR_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_ST_S  2
/** GDMA_OUT_DONE_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH0_INT_ST    (BIT(3))
#define GDMA_OUT_DONE_CH0_INT_ST_M  (GDMA_OUT_DONE_CH0_INT_ST_V << GDMA_OUT_DONE_CH0_INT_ST_S)
#define GDMA_OUT_DONE_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_ST_S  3
/** GDMA_OUT_EOF_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH0_INT_ST    (BIT(4))
#define GDMA_OUT_EOF_CH0_INT_ST_M  (GDMA_OUT_EOF_CH0_INT_ST_V << GDMA_OUT_EOF_CH0_INT_ST_S)
#define GDMA_OUT_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_ST_S  4
/** GDMA_IN_DSCR_ERR_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_ST    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_M  (GDMA_IN_DSCR_ERR_CH0_INT_ST_V << GDMA_IN_DSCR_ERR_CH0_INT_ST_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_S  5
/** GDMA_OUT_DSCR_ERR_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_M  (GDMA_OUT_DSCR_ERR_CH0_INT_ST_V << GDMA_OUT_DSCR_ERR_CH0_INT_ST_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_S  6
/** GDMA_IN_DSCR_EMPTY_CH0_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_ST_V << GDMA_IN_DSCR_EMPTY_CH0_INT_ST_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_S  7
/** GDMA_OUT_TOTAL_EOF_CH0_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_ST_V << GDMA_OUT_TOTAL_EOF_CH0_INT_ST_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_S  8
/** GDMA_INFIFO_OVF_CH0_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH0_INT_ST    (BIT(9))
#define GDMA_INFIFO_OVF_CH0_INT_ST_M  (GDMA_INFIFO_OVF_CH0_INT_ST_V << GDMA_INFIFO_OVF_CH0_INT_ST_S)
#define GDMA_INFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_ST_S  9
/** GDMA_INFIFO_UDF_CH0_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH0_INT_ST    (BIT(10))
#define GDMA_INFIFO_UDF_CH0_INT_ST_M  (GDMA_INFIFO_UDF_CH0_INT_ST_V << GDMA_INFIFO_UDF_CH0_INT_ST_S)
#define GDMA_INFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_ST_S  10
/** GDMA_OUTFIFO_OVF_CH0_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_ST    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH0_INT_ST_M  (GDMA_OUTFIFO_OVF_CH0_INT_ST_V << GDMA_OUTFIFO_OVF_CH0_INT_ST_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_ST_S  11
/** GDMA_OUTFIFO_UDF_CH0_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_ST    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH0_INT_ST_M  (GDMA_OUTFIFO_UDF_CH0_INT_ST_V << GDMA_OUTFIFO_UDF_CH0_INT_ST_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_ST_S  12

/** GDMA_INT_ENA_CH0_REG register
 *  Interrupt enable bits of RX channel 0
 */
#define GDMA_INT_ENA_CH0_REG (DR_REG_GDMA_BASE + 0x8)
/** GDMA_IN_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH0_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ENA_M  (GDMA_IN_DONE_CH0_INT_ENA_V << GDMA_IN_DONE_CH0_INT_ENA_S)
#define GDMA_IN_DONE_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_ENA_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_M  (GDMA_IN_SUC_EOF_CH0_INT_ENA_V << GDMA_IN_SUC_EOF_CH0_INT_ENA_S)
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_M  (GDMA_IN_ERR_EOF_CH0_INT_ENA_V << GDMA_IN_ERR_EOF_CH0_INT_ENA_S)
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_S  2
/** GDMA_OUT_DONE_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH0_INT_ENA    (BIT(3))
#define GDMA_OUT_DONE_CH0_INT_ENA_M  (GDMA_OUT_DONE_CH0_INT_ENA_V << GDMA_OUT_DONE_CH0_INT_ENA_S)
#define GDMA_OUT_DONE_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_ENA_S  3
/** GDMA_OUT_EOF_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH0_INT_ENA    (BIT(4))
#define GDMA_OUT_EOF_CH0_INT_ENA_M  (GDMA_OUT_EOF_CH0_INT_ENA_V << GDMA_OUT_EOF_CH0_INT_ENA_S)
#define GDMA_OUT_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_ENA_S  4
/** GDMA_IN_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_M  (GDMA_IN_DSCR_ERR_CH0_INT_ENA_V << GDMA_IN_DSCR_ERR_CH0_INT_ENA_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_S  5
/** GDMA_OUT_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_M  (GDMA_OUT_DSCR_ERR_CH0_INT_ENA_V << GDMA_OUT_DSCR_ERR_CH0_INT_ENA_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_S  6
/** GDMA_IN_DSCR_EMPTY_CH0_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_V << GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_S  7
/** GDMA_OUT_TOTAL_EOF_CH0_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_V << GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_S  8
/** GDMA_INFIFO_OVF_CH0_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH0_INT_ENA    (BIT(9))
#define GDMA_INFIFO_OVF_CH0_INT_ENA_M  (GDMA_INFIFO_OVF_CH0_INT_ENA_V << GDMA_INFIFO_OVF_CH0_INT_ENA_S)
#define GDMA_INFIFO_OVF_CH0_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_ENA_S  9
/** GDMA_INFIFO_UDF_CH0_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH0_INT_ENA    (BIT(10))
#define GDMA_INFIFO_UDF_CH0_INT_ENA_M  (GDMA_INFIFO_UDF_CH0_INT_ENA_V << GDMA_INFIFO_UDF_CH0_INT_ENA_S)
#define GDMA_INFIFO_UDF_CH0_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_ENA_S  10
/** GDMA_OUTFIFO_OVF_CH0_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA_M  (GDMA_OUTFIFO_OVF_CH0_INT_ENA_V << GDMA_OUTFIFO_OVF_CH0_INT_ENA_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA_S  11
/** GDMA_OUTFIFO_UDF_CH0_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA_M  (GDMA_OUTFIFO_UDF_CH0_INT_ENA_V << GDMA_OUTFIFO_UDF_CH0_INT_ENA_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA_S  12

/** GDMA_INT_CLR_CH0_REG register
 *  Interrupt clear bits of RX channel 0
 */
#define GDMA_INT_CLR_CH0_REG (DR_REG_GDMA_BASE + 0xc)
/** GDMA_IN_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH0_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_CLR_M  (GDMA_IN_DONE_CH0_INT_CLR_V << GDMA_IN_DONE_CH0_INT_CLR_S)
#define GDMA_IN_DONE_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_CLR_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_M  (GDMA_IN_SUC_EOF_CH0_INT_CLR_V << GDMA_IN_SUC_EOF_CH0_INT_CLR_S)
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_M  (GDMA_IN_ERR_EOF_CH0_INT_CLR_V << GDMA_IN_ERR_EOF_CH0_INT_CLR_S)
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_S  2
/** GDMA_OUT_DONE_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH0_INT_CLR    (BIT(3))
#define GDMA_OUT_DONE_CH0_INT_CLR_M  (GDMA_OUT_DONE_CH0_INT_CLR_V << GDMA_OUT_DONE_CH0_INT_CLR_S)
#define GDMA_OUT_DONE_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_CLR_S  3
/** GDMA_OUT_EOF_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH0_INT_CLR    (BIT(4))
#define GDMA_OUT_EOF_CH0_INT_CLR_M  (GDMA_OUT_EOF_CH0_INT_CLR_V << GDMA_OUT_EOF_CH0_INT_CLR_S)
#define GDMA_OUT_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_CLR_S  4
/** GDMA_IN_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_M  (GDMA_IN_DSCR_ERR_CH0_INT_CLR_V << GDMA_IN_DSCR_ERR_CH0_INT_CLR_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_S  5
/** GDMA_OUT_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_M  (GDMA_OUT_DSCR_ERR_CH0_INT_CLR_V << GDMA_OUT_DSCR_ERR_CH0_INT_CLR_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_S  6
/** GDMA_IN_DSCR_EMPTY_CH0_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_V << GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_S  7
/** GDMA_OUT_TOTAL_EOF_CH0_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_V << GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_S  8
/** GDMA_INFIFO_OVF_CH0_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH0_INT_CLR    (BIT(9))
#define GDMA_INFIFO_OVF_CH0_INT_CLR_M  (GDMA_INFIFO_OVF_CH0_INT_CLR_V << GDMA_INFIFO_OVF_CH0_INT_CLR_S)
#define GDMA_INFIFO_OVF_CH0_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_CLR_S  9
/** GDMA_INFIFO_UDF_CH0_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH0_INT_CLR    (BIT(10))
#define GDMA_INFIFO_UDF_CH0_INT_CLR_M  (GDMA_INFIFO_UDF_CH0_INT_CLR_V << GDMA_INFIFO_UDF_CH0_INT_CLR_S)
#define GDMA_INFIFO_UDF_CH0_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_CLR_S  10
/** GDMA_OUTFIFO_OVF_CH0_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR_M  (GDMA_OUTFIFO_OVF_CH0_INT_CLR_V << GDMA_OUTFIFO_OVF_CH0_INT_CLR_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR_S  11
/** GDMA_OUTFIFO_UDF_CH0_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR_M  (GDMA_OUTFIFO_UDF_CH0_INT_CLR_V << GDMA_OUTFIFO_UDF_CH0_INT_CLR_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR_S  12

/** GDMA_INT_RAW_CH1_REG register
 *  Raw status interrupt of RX channel 1
 */
#define GDMA_INT_RAW_CH1_REG (DR_REG_GDMA_BASE + 0x10)
/** GDMA_IN_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one receive
 *  descriptor has been received for RX channel 0.
 */
#define GDMA_IN_DONE_CH1_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_RAW_M  (GDMA_IN_DONE_CH1_INT_RAW_V << GDMA_IN_DONE_CH1_INT_RAW_S)
#define GDMA_IN_DONE_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_RAW_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level for RX channel 0 when the last data
 *  pointed by one receive descriptor has been received and the suc_eof bit in this
 *  descriptor is 1. For UHCI0, the raw interrupt bit turns to high level when the last
 *  data pointed by one receive descriptor has been received and no data error is
 *  detected for RX channel 0.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_M  (GDMA_IN_SUC_EOF_CH1_INT_RAW_V << GDMA_IN_SUC_EOF_CH1_INT_RAW_S)
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for RX channel 0. For other peripherals, this raw
 *  interrupt is reserved.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_M  (GDMA_IN_ERR_EOF_CH1_INT_RAW_V << GDMA_IN_ERR_EOF_CH1_INT_RAW_S)
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_S  2
/** GDMA_OUT_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one
 *  transmit descriptor has been transmitted to peripherals for TX channel 0.
 */
#define GDMA_OUT_DONE_CH1_INT_RAW    (BIT(3))
#define GDMA_OUT_DONE_CH1_INT_RAW_M  (GDMA_OUT_DONE_CH1_INT_RAW_V << GDMA_OUT_DONE_CH1_INT_RAW_S)
#define GDMA_OUT_DONE_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_RAW_S  3
/** GDMA_OUT_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one
 *  transmit descriptor has been read from memory for TX channel 0.
 */
#define GDMA_OUT_EOF_CH1_INT_RAW    (BIT(4))
#define GDMA_OUT_EOF_CH1_INT_RAW_M  (GDMA_OUT_EOF_CH1_INT_RAW_V << GDMA_OUT_EOF_CH1_INT_RAW_S)
#define GDMA_OUT_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_RAW_S  4
/** GDMA_IN_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when detecting receive descriptor error,
 *  including owner error, the second and third word error of receive descriptor for RX
 *  channel 0.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_M  (GDMA_IN_DSCR_ERR_CH1_INT_RAW_V << GDMA_IN_DSCR_ERR_CH1_INT_RAW_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_S  5
/** GDMA_OUT_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when detecting transmit descriptor error,
 *  including owner error, the second and third word error of transmit descriptor for
 *  TX channel 0.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_M  (GDMA_OUT_DSCR_ERR_CH1_INT_RAW_V << GDMA_OUT_DSCR_ERR_CH1_INT_RAW_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_S  6
/** GDMA_IN_DSCR_EMPTY_CH1_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when RX buffer pointed by inlink is full
 *  and receiving data is not completed, but there is no more inlink for RX channel 0.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_V << GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_S  7
/** GDMA_OUT_TOTAL_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one descriptor or few descriptors) is transmitted out for TX channel 0.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_V << GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_S  8
/** GDMA_INFIFO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of RX channel 0 is
 *  overflow.
 */
#define GDMA_INFIFO_OVF_CH1_INT_RAW    (BIT(9))
#define GDMA_INFIFO_OVF_CH1_INT_RAW_M  (GDMA_INFIFO_OVF_CH1_INT_RAW_V << GDMA_INFIFO_OVF_CH1_INT_RAW_S)
#define GDMA_INFIFO_OVF_CH1_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_RAW_S  9
/** GDMA_INFIFO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of RX channel 0 is
 *  underflow.
 */
#define GDMA_INFIFO_UDF_CH1_INT_RAW    (BIT(10))
#define GDMA_INFIFO_UDF_CH1_INT_RAW_M  (GDMA_INFIFO_UDF_CH1_INT_RAW_V << GDMA_INFIFO_UDF_CH1_INT_RAW_S)
#define GDMA_INFIFO_UDF_CH1_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_RAW_S  10
/** GDMA_OUTFIFO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of TX channel 0 is
 *  overflow.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW_M  (GDMA_OUTFIFO_OVF_CH1_INT_RAW_V << GDMA_OUTFIFO_OVF_CH1_INT_RAW_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW_S  11
/** GDMA_OUTFIFO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of TX channel 0 is
 *  underflow.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW_M  (GDMA_OUTFIFO_UDF_CH1_INT_RAW_V << GDMA_OUTFIFO_UDF_CH1_INT_RAW_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW_S  12

/** GDMA_INT_ST_CH1_REG register
 *  Masked interrupt of RX channel 1
 */
#define GDMA_INT_ST_CH1_REG (DR_REG_GDMA_BASE + 0x14)
/** GDMA_IN_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH1_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ST_M  (GDMA_IN_DONE_CH1_INT_ST_V << GDMA_IN_DONE_CH1_INT_ST_S)
#define GDMA_IN_DONE_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_ST_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ST_M  (GDMA_IN_SUC_EOF_CH1_INT_ST_V << GDMA_IN_SUC_EOF_CH1_INT_ST_S)
#define GDMA_IN_SUC_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_ST_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ST_M  (GDMA_IN_ERR_EOF_CH1_INT_ST_V << GDMA_IN_ERR_EOF_CH1_INT_ST_S)
#define GDMA_IN_ERR_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_ST_S  2
/** GDMA_OUT_DONE_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH1_INT_ST    (BIT(3))
#define GDMA_OUT_DONE_CH1_INT_ST_M  (GDMA_OUT_DONE_CH1_INT_ST_V << GDMA_OUT_DONE_CH1_INT_ST_S)
#define GDMA_OUT_DONE_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_ST_S  3
/** GDMA_OUT_EOF_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH1_INT_ST    (BIT(4))
#define GDMA_OUT_EOF_CH1_INT_ST_M  (GDMA_OUT_EOF_CH1_INT_ST_V << GDMA_OUT_EOF_CH1_INT_ST_S)
#define GDMA_OUT_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_ST_S  4
/** GDMA_IN_DSCR_ERR_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_ST    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_M  (GDMA_IN_DSCR_ERR_CH1_INT_ST_V << GDMA_IN_DSCR_ERR_CH1_INT_ST_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_S  5
/** GDMA_OUT_DSCR_ERR_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_M  (GDMA_OUT_DSCR_ERR_CH1_INT_ST_V << GDMA_OUT_DSCR_ERR_CH1_INT_ST_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_S  6
/** GDMA_IN_DSCR_EMPTY_CH1_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_ST_V << GDMA_IN_DSCR_EMPTY_CH1_INT_ST_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_S  7
/** GDMA_OUT_TOTAL_EOF_CH1_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_ST_V << GDMA_OUT_TOTAL_EOF_CH1_INT_ST_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_S  8
/** GDMA_INFIFO_OVF_CH1_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH1_INT_ST    (BIT(9))
#define GDMA_INFIFO_OVF_CH1_INT_ST_M  (GDMA_INFIFO_OVF_CH1_INT_ST_V << GDMA_INFIFO_OVF_CH1_INT_ST_S)
#define GDMA_INFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_ST_S  9
/** GDMA_INFIFO_UDF_CH1_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH1_INT_ST    (BIT(10))
#define GDMA_INFIFO_UDF_CH1_INT_ST_M  (GDMA_INFIFO_UDF_CH1_INT_ST_V << GDMA_INFIFO_UDF_CH1_INT_ST_S)
#define GDMA_INFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_ST_S  10
/** GDMA_OUTFIFO_OVF_CH1_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_ST    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH1_INT_ST_M  (GDMA_OUTFIFO_OVF_CH1_INT_ST_V << GDMA_OUTFIFO_OVF_CH1_INT_ST_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_ST_S  11
/** GDMA_OUTFIFO_UDF_CH1_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_ST    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH1_INT_ST_M  (GDMA_OUTFIFO_UDF_CH1_INT_ST_V << GDMA_OUTFIFO_UDF_CH1_INT_ST_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_ST_S  12

/** GDMA_INT_ENA_CH1_REG register
 *  Interrupt enable bits of RX channel 1
 */
#define GDMA_INT_ENA_CH1_REG (DR_REG_GDMA_BASE + 0x18)
/** GDMA_IN_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH1_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ENA_M  (GDMA_IN_DONE_CH1_INT_ENA_V << GDMA_IN_DONE_CH1_INT_ENA_S)
#define GDMA_IN_DONE_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_ENA_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_M  (GDMA_IN_SUC_EOF_CH1_INT_ENA_V << GDMA_IN_SUC_EOF_CH1_INT_ENA_S)
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_M  (GDMA_IN_ERR_EOF_CH1_INT_ENA_V << GDMA_IN_ERR_EOF_CH1_INT_ENA_S)
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_S  2
/** GDMA_OUT_DONE_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH1_INT_ENA    (BIT(3))
#define GDMA_OUT_DONE_CH1_INT_ENA_M  (GDMA_OUT_DONE_CH1_INT_ENA_V << GDMA_OUT_DONE_CH1_INT_ENA_S)
#define GDMA_OUT_DONE_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_ENA_S  3
/** GDMA_OUT_EOF_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH1_INT_ENA    (BIT(4))
#define GDMA_OUT_EOF_CH1_INT_ENA_M  (GDMA_OUT_EOF_CH1_INT_ENA_V << GDMA_OUT_EOF_CH1_INT_ENA_S)
#define GDMA_OUT_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_ENA_S  4
/** GDMA_IN_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_M  (GDMA_IN_DSCR_ERR_CH1_INT_ENA_V << GDMA_IN_DSCR_ERR_CH1_INT_ENA_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_S  5
/** GDMA_OUT_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_M  (GDMA_OUT_DSCR_ERR_CH1_INT_ENA_V << GDMA_OUT_DSCR_ERR_CH1_INT_ENA_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_S  6
/** GDMA_IN_DSCR_EMPTY_CH1_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_V << GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_S  7
/** GDMA_OUT_TOTAL_EOF_CH1_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_V << GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_S  8
/** GDMA_INFIFO_OVF_CH1_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH1_INT_ENA    (BIT(9))
#define GDMA_INFIFO_OVF_CH1_INT_ENA_M  (GDMA_INFIFO_OVF_CH1_INT_ENA_V << GDMA_INFIFO_OVF_CH1_INT_ENA_S)
#define GDMA_INFIFO_OVF_CH1_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_ENA_S  9
/** GDMA_INFIFO_UDF_CH1_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH1_INT_ENA    (BIT(10))
#define GDMA_INFIFO_UDF_CH1_INT_ENA_M  (GDMA_INFIFO_UDF_CH1_INT_ENA_V << GDMA_INFIFO_UDF_CH1_INT_ENA_S)
#define GDMA_INFIFO_UDF_CH1_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_ENA_S  10
/** GDMA_OUTFIFO_OVF_CH1_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA_M  (GDMA_OUTFIFO_OVF_CH1_INT_ENA_V << GDMA_OUTFIFO_OVF_CH1_INT_ENA_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA_S  11
/** GDMA_OUTFIFO_UDF_CH1_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA_M  (GDMA_OUTFIFO_UDF_CH1_INT_ENA_V << GDMA_OUTFIFO_UDF_CH1_INT_ENA_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA_S  12

/** GDMA_INT_CLR_CH1_REG register
 *  Interrupt clear bits of RX channel 1
 */
#define GDMA_INT_CLR_CH1_REG (DR_REG_GDMA_BASE + 0x1c)
/** GDMA_IN_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH1_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_CLR_M  (GDMA_IN_DONE_CH1_INT_CLR_V << GDMA_IN_DONE_CH1_INT_CLR_S)
#define GDMA_IN_DONE_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_CLR_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_M  (GDMA_IN_SUC_EOF_CH1_INT_CLR_V << GDMA_IN_SUC_EOF_CH1_INT_CLR_S)
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_M  (GDMA_IN_ERR_EOF_CH1_INT_CLR_V << GDMA_IN_ERR_EOF_CH1_INT_CLR_S)
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_S  2
/** GDMA_OUT_DONE_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH1_INT_CLR    (BIT(3))
#define GDMA_OUT_DONE_CH1_INT_CLR_M  (GDMA_OUT_DONE_CH1_INT_CLR_V << GDMA_OUT_DONE_CH1_INT_CLR_S)
#define GDMA_OUT_DONE_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_CLR_S  3
/** GDMA_OUT_EOF_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH1_INT_CLR    (BIT(4))
#define GDMA_OUT_EOF_CH1_INT_CLR_M  (GDMA_OUT_EOF_CH1_INT_CLR_V << GDMA_OUT_EOF_CH1_INT_CLR_S)
#define GDMA_OUT_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_CLR_S  4
/** GDMA_IN_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_M  (GDMA_IN_DSCR_ERR_CH1_INT_CLR_V << GDMA_IN_DSCR_ERR_CH1_INT_CLR_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_S  5
/** GDMA_OUT_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_M  (GDMA_OUT_DSCR_ERR_CH1_INT_CLR_V << GDMA_OUT_DSCR_ERR_CH1_INT_CLR_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_S  6
/** GDMA_IN_DSCR_EMPTY_CH1_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_V << GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_S  7
/** GDMA_OUT_TOTAL_EOF_CH1_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_V << GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_S  8
/** GDMA_INFIFO_OVF_CH1_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH1_INT_CLR    (BIT(9))
#define GDMA_INFIFO_OVF_CH1_INT_CLR_M  (GDMA_INFIFO_OVF_CH1_INT_CLR_V << GDMA_INFIFO_OVF_CH1_INT_CLR_S)
#define GDMA_INFIFO_OVF_CH1_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_CLR_S  9
/** GDMA_INFIFO_UDF_CH1_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH1_INT_CLR    (BIT(10))
#define GDMA_INFIFO_UDF_CH1_INT_CLR_M  (GDMA_INFIFO_UDF_CH1_INT_CLR_V << GDMA_INFIFO_UDF_CH1_INT_CLR_S)
#define GDMA_INFIFO_UDF_CH1_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_CLR_S  10
/** GDMA_OUTFIFO_OVF_CH1_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR_M  (GDMA_OUTFIFO_OVF_CH1_INT_CLR_V << GDMA_OUTFIFO_OVF_CH1_INT_CLR_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR_S  11
/** GDMA_OUTFIFO_UDF_CH1_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR_M  (GDMA_OUTFIFO_UDF_CH1_INT_CLR_V << GDMA_OUTFIFO_UDF_CH1_INT_CLR_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR_S  12

/** GDMA_INT_RAW_CH2_REG register
 *  Raw status interrupt of RX channel 2
 */
#define GDMA_INT_RAW_CH2_REG (DR_REG_GDMA_BASE + 0x20)
/** GDMA_IN_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one receive
 *  descriptor has been received for RX channel 0.
 */
#define GDMA_IN_DONE_CH2_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_RAW_M  (GDMA_IN_DONE_CH2_INT_RAW_V << GDMA_IN_DONE_CH2_INT_RAW_S)
#define GDMA_IN_DONE_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_RAW_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level for RX channel 0 when the last data
 *  pointed by one receive descriptor has been received and the suc_eof bit in this
 *  descriptor is 1. For UHCI0, the raw interrupt bit turns to high level when the last
 *  data pointed by one receive descriptor has been received and no data error is
 *  detected for RX channel 0.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_M  (GDMA_IN_SUC_EOF_CH2_INT_RAW_V << GDMA_IN_SUC_EOF_CH2_INT_RAW_S)
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for RX channel 0. For other peripherals, this raw
 *  interrupt is reserved.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_M  (GDMA_IN_ERR_EOF_CH2_INT_RAW_V << GDMA_IN_ERR_EOF_CH2_INT_RAW_S)
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_S  2
/** GDMA_OUT_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one
 *  transmit descriptor has been transmitted to peripherals for TX channel 0.
 */
#define GDMA_OUT_DONE_CH2_INT_RAW    (BIT(3))
#define GDMA_OUT_DONE_CH2_INT_RAW_M  (GDMA_OUT_DONE_CH2_INT_RAW_V << GDMA_OUT_DONE_CH2_INT_RAW_S)
#define GDMA_OUT_DONE_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_RAW_S  3
/** GDMA_OUT_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one
 *  transmit descriptor has been read from memory for TX channel 0.
 */
#define GDMA_OUT_EOF_CH2_INT_RAW    (BIT(4))
#define GDMA_OUT_EOF_CH2_INT_RAW_M  (GDMA_OUT_EOF_CH2_INT_RAW_V << GDMA_OUT_EOF_CH2_INT_RAW_S)
#define GDMA_OUT_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_RAW_S  4
/** GDMA_IN_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt bit turns to high level when detecting receive descriptor error,
 *  including owner error, the second and third word error of receive descriptor for RX
 *  channel 0.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_M  (GDMA_IN_DSCR_ERR_CH2_INT_RAW_V << GDMA_IN_DSCR_ERR_CH2_INT_RAW_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_S  5
/** GDMA_OUT_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  The raw interrupt bit turns to high level when detecting transmit descriptor error,
 *  including owner error, the second and third word error of transmit descriptor for
 *  TX channel 0.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_M  (GDMA_OUT_DSCR_ERR_CH2_INT_RAW_V << GDMA_OUT_DSCR_ERR_CH2_INT_RAW_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_S  6
/** GDMA_IN_DSCR_EMPTY_CH2_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  The raw interrupt bit turns to high level when RX buffer pointed by inlink is full
 *  and receiving data is not completed, but there is no more inlink for RX channel 0.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_V << GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_S  7
/** GDMA_OUT_TOTAL_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one descriptor or few descriptors) is transmitted out for TX channel 0.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_V << GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_S  8
/** GDMA_INFIFO_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of RX channel 0 is
 *  overflow.
 */
#define GDMA_INFIFO_OVF_CH2_INT_RAW    (BIT(9))
#define GDMA_INFIFO_OVF_CH2_INT_RAW_M  (GDMA_INFIFO_OVF_CH2_INT_RAW_V << GDMA_INFIFO_OVF_CH2_INT_RAW_S)
#define GDMA_INFIFO_OVF_CH2_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_RAW_S  9
/** GDMA_INFIFO_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of RX channel 0 is
 *  underflow.
 */
#define GDMA_INFIFO_UDF_CH2_INT_RAW    (BIT(10))
#define GDMA_INFIFO_UDF_CH2_INT_RAW_M  (GDMA_INFIFO_UDF_CH2_INT_RAW_V << GDMA_INFIFO_UDF_CH2_INT_RAW_S)
#define GDMA_INFIFO_UDF_CH2_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_RAW_S  10
/** GDMA_OUTFIFO_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of TX channel 0 is
 *  overflow.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW_M  (GDMA_OUTFIFO_OVF_CH2_INT_RAW_V << GDMA_OUTFIFO_OVF_CH2_INT_RAW_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW_S  11
/** GDMA_OUTFIFO_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 FIFO of TX channel 0 is
 *  underflow.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW_M  (GDMA_OUTFIFO_UDF_CH2_INT_RAW_V << GDMA_OUTFIFO_UDF_CH2_INT_RAW_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW_S  12

/** GDMA_INT_ST_CH2_REG register
 *  Masked interrupt of RX channel 2
 */
#define GDMA_INT_ST_CH2_REG (DR_REG_GDMA_BASE + 0x24)
/** GDMA_IN_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH2_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ST_M  (GDMA_IN_DONE_CH2_INT_ST_V << GDMA_IN_DONE_CH2_INT_ST_S)
#define GDMA_IN_DONE_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_ST_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ST_M  (GDMA_IN_SUC_EOF_CH2_INT_ST_V << GDMA_IN_SUC_EOF_CH2_INT_ST_S)
#define GDMA_IN_SUC_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_ST_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ST_M  (GDMA_IN_ERR_EOF_CH2_INT_ST_V << GDMA_IN_ERR_EOF_CH2_INT_ST_S)
#define GDMA_IN_ERR_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_ST_S  2
/** GDMA_OUT_DONE_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH2_INT_ST    (BIT(3))
#define GDMA_OUT_DONE_CH2_INT_ST_M  (GDMA_OUT_DONE_CH2_INT_ST_V << GDMA_OUT_DONE_CH2_INT_ST_S)
#define GDMA_OUT_DONE_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_ST_S  3
/** GDMA_OUT_EOF_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH2_INT_ST    (BIT(4))
#define GDMA_OUT_EOF_CH2_INT_ST_M  (GDMA_OUT_EOF_CH2_INT_ST_V << GDMA_OUT_EOF_CH2_INT_ST_S)
#define GDMA_OUT_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_ST_S  4
/** GDMA_IN_DSCR_ERR_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_ST    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_M  (GDMA_IN_DSCR_ERR_CH2_INT_ST_V << GDMA_IN_DSCR_ERR_CH2_INT_ST_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_S  5
/** GDMA_OUT_DSCR_ERR_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_M  (GDMA_OUT_DSCR_ERR_CH2_INT_ST_V << GDMA_OUT_DSCR_ERR_CH2_INT_ST_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_S  6
/** GDMA_IN_DSCR_EMPTY_CH2_INT_ST : RO; bitpos: [7]; default: 0;
 *  The raw interrupt status bit for the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_ST_V << GDMA_IN_DSCR_EMPTY_CH2_INT_ST_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_S  7
/** GDMA_OUT_TOTAL_EOF_CH2_INT_ST : RO; bitpos: [8]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_ST_V << GDMA_OUT_TOTAL_EOF_CH2_INT_ST_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_S  8
/** GDMA_INFIFO_OVF_CH2_INT_ST : RO; bitpos: [9]; default: 0;
 *  The raw interrupt status bit for the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH2_INT_ST    (BIT(9))
#define GDMA_INFIFO_OVF_CH2_INT_ST_M  (GDMA_INFIFO_OVF_CH2_INT_ST_V << GDMA_INFIFO_OVF_CH2_INT_ST_S)
#define GDMA_INFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_ST_S  9
/** GDMA_INFIFO_UDF_CH2_INT_ST : RO; bitpos: [10]; default: 0;
 *  The raw interrupt status bit for the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH2_INT_ST    (BIT(10))
#define GDMA_INFIFO_UDF_CH2_INT_ST_M  (GDMA_INFIFO_UDF_CH2_INT_ST_V << GDMA_INFIFO_UDF_CH2_INT_ST_S)
#define GDMA_INFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_ST_S  10
/** GDMA_OUTFIFO_OVF_CH2_INT_ST : RO; bitpos: [11]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_ST    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH2_INT_ST_M  (GDMA_OUTFIFO_OVF_CH2_INT_ST_V << GDMA_OUTFIFO_OVF_CH2_INT_ST_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_ST_S  11
/** GDMA_OUTFIFO_UDF_CH2_INT_ST : RO; bitpos: [12]; default: 0;
 *  The raw interrupt status bit for the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_ST    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH2_INT_ST_M  (GDMA_OUTFIFO_UDF_CH2_INT_ST_V << GDMA_OUTFIFO_UDF_CH2_INT_ST_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_ST_S  12

/** GDMA_INT_ENA_CH2_REG register
 *  Interrupt enable bits of RX channel 2
 */
#define GDMA_INT_ENA_CH2_REG (DR_REG_GDMA_BASE + 0x28)
/** GDMA_IN_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH2_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ENA_M  (GDMA_IN_DONE_CH2_INT_ENA_V << GDMA_IN_DONE_CH2_INT_ENA_S)
#define GDMA_IN_DONE_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_ENA_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_M  (GDMA_IN_SUC_EOF_CH2_INT_ENA_V << GDMA_IN_SUC_EOF_CH2_INT_ENA_S)
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_M  (GDMA_IN_ERR_EOF_CH2_INT_ENA_V << GDMA_IN_ERR_EOF_CH2_INT_ENA_S)
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_S  2
/** GDMA_OUT_DONE_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH2_INT_ENA    (BIT(3))
#define GDMA_OUT_DONE_CH2_INT_ENA_M  (GDMA_OUT_DONE_CH2_INT_ENA_V << GDMA_OUT_DONE_CH2_INT_ENA_S)
#define GDMA_OUT_DONE_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_ENA_S  3
/** GDMA_OUT_EOF_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH2_INT_ENA    (BIT(4))
#define GDMA_OUT_EOF_CH2_INT_ENA_M  (GDMA_OUT_EOF_CH2_INT_ENA_V << GDMA_OUT_EOF_CH2_INT_ENA_S)
#define GDMA_OUT_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_ENA_S  4
/** GDMA_IN_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_M  (GDMA_IN_DSCR_ERR_CH2_INT_ENA_V << GDMA_IN_DSCR_ERR_CH2_INT_ENA_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_S  5
/** GDMA_OUT_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_M  (GDMA_OUT_DSCR_ERR_CH2_INT_ENA_V << GDMA_OUT_DSCR_ERR_CH2_INT_ENA_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_S  6
/** GDMA_IN_DSCR_EMPTY_CH2_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  The interrupt enable bit for the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_V << GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_S  7
/** GDMA_OUT_TOTAL_EOF_CH2_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  The interrupt enable bit for the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_V << GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_S  8
/** GDMA_INFIFO_OVF_CH2_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  The interrupt enable bit for the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH2_INT_ENA    (BIT(9))
#define GDMA_INFIFO_OVF_CH2_INT_ENA_M  (GDMA_INFIFO_OVF_CH2_INT_ENA_V << GDMA_INFIFO_OVF_CH2_INT_ENA_S)
#define GDMA_INFIFO_OVF_CH2_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_ENA_S  9
/** GDMA_INFIFO_UDF_CH2_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  The interrupt enable bit for the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH2_INT_ENA    (BIT(10))
#define GDMA_INFIFO_UDF_CH2_INT_ENA_M  (GDMA_INFIFO_UDF_CH2_INT_ENA_V << GDMA_INFIFO_UDF_CH2_INT_ENA_S)
#define GDMA_INFIFO_UDF_CH2_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_ENA_S  10
/** GDMA_OUTFIFO_OVF_CH2_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  The interrupt enable bit for the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA_M  (GDMA_OUTFIFO_OVF_CH2_INT_ENA_V << GDMA_OUTFIFO_OVF_CH2_INT_ENA_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA_S  11
/** GDMA_OUTFIFO_UDF_CH2_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  The interrupt enable bit for the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA_M  (GDMA_OUTFIFO_UDF_CH2_INT_ENA_V << GDMA_OUTFIFO_UDF_CH2_INT_ENA_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA_S  12

/** GDMA_INT_CLR_CH2_REG register
 *  Interrupt clear bits of RX channel 2
 */
#define GDMA_INT_CLR_CH2_REG (DR_REG_GDMA_BASE + 0x2c)
/** GDMA_IN_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the GDMA_IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH2_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_CLR_M  (GDMA_IN_DONE_CH2_INT_CLR_V << GDMA_IN_DONE_CH2_INT_CLR_S)
#define GDMA_IN_DONE_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_CLR_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the GDMA_IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_M  (GDMA_IN_SUC_EOF_CH2_INT_CLR_V << GDMA_IN_SUC_EOF_CH2_INT_CLR_S)
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the GDMA_IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_M  (GDMA_IN_ERR_EOF_CH2_INT_CLR_V << GDMA_IN_ERR_EOF_CH2_INT_CLR_S)
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_S  2
/** GDMA_OUT_DONE_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the GDMA_OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH2_INT_CLR    (BIT(3))
#define GDMA_OUT_DONE_CH2_INT_CLR_M  (GDMA_OUT_DONE_CH2_INT_CLR_V << GDMA_OUT_DONE_CH2_INT_CLR_S)
#define GDMA_OUT_DONE_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_CLR_S  3
/** GDMA_OUT_EOF_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the GDMA_OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH2_INT_CLR    (BIT(4))
#define GDMA_OUT_EOF_CH2_INT_CLR_M  (GDMA_OUT_EOF_CH2_INT_CLR_V << GDMA_OUT_EOF_CH2_INT_CLR_S)
#define GDMA_OUT_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_CLR_S  4
/** GDMA_IN_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the GDMA_IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR    (BIT(5))
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_M  (GDMA_IN_DSCR_ERR_CH2_INT_CLR_V << GDMA_IN_DSCR_ERR_CH2_INT_CLR_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_S  5
/** GDMA_OUT_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the GDMA_OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR    (BIT(6))
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_M  (GDMA_OUT_DSCR_ERR_CH2_INT_CLR_V << GDMA_OUT_DSCR_ERR_CH2_INT_CLR_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_S  6
/** GDMA_IN_DSCR_EMPTY_CH2_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Set this bit to clear the GDMA_IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR    (BIT(7))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_V << GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_S  7
/** GDMA_OUT_TOTAL_EOF_CH2_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Set this bit to clear the GDMA_OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR    (BIT(8))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_V << GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_S  8
/** GDMA_INFIFO_OVF_CH2_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Set this bit to clear the GDMA_INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH2_INT_CLR    (BIT(9))
#define GDMA_INFIFO_OVF_CH2_INT_CLR_M  (GDMA_INFIFO_OVF_CH2_INT_CLR_V << GDMA_INFIFO_OVF_CH2_INT_CLR_S)
#define GDMA_INFIFO_OVF_CH2_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_CLR_S  9
/** GDMA_INFIFO_UDF_CH2_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Set this bit to clear the GDMA_INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH2_INT_CLR    (BIT(10))
#define GDMA_INFIFO_UDF_CH2_INT_CLR_M  (GDMA_INFIFO_UDF_CH2_INT_CLR_V << GDMA_INFIFO_UDF_CH2_INT_CLR_S)
#define GDMA_INFIFO_UDF_CH2_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_CLR_S  10
/** GDMA_OUTFIFO_OVF_CH2_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Set this bit to clear the GDMA_OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR    (BIT(11))
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR_M  (GDMA_OUTFIFO_OVF_CH2_INT_CLR_V << GDMA_OUTFIFO_OVF_CH2_INT_CLR_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR_S  11
/** GDMA_OUTFIFO_UDF_CH2_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Set this bit to clear the GDMA_OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR    (BIT(12))
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR_M  (GDMA_OUTFIFO_UDF_CH2_INT_CLR_V << GDMA_OUTFIFO_UDF_CH2_INT_CLR_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR_S  12

/** GDMA_MISC_CONF_REG register
 *  Miscellaneous register
 */
#define GDMA_MISC_CONF_REG (DR_REG_GDMA_BASE + 0x44)
/** GDMA_AHBM_RST_INTER : R/W; bitpos: [0]; default: 0;
 *  Set this bit, then clear this bit to reset the internal ahb FSM.
 */
#define GDMA_AHBM_RST_INTER    (BIT(0))
#define GDMA_AHBM_RST_INTER_M  (GDMA_AHBM_RST_INTER_V << GDMA_AHBM_RST_INTER_S)
#define GDMA_AHBM_RST_INTER_V  0x00000001U
#define GDMA_AHBM_RST_INTER_S  0
/** GDMA_ARB_PRI_DIS : R/W; bitpos: [2]; default: 0;
 *  Set this bit to disable priority arbitration function.
 */
#define GDMA_ARB_PRI_DIS    (BIT(2))
#define GDMA_ARB_PRI_DIS_M  (GDMA_ARB_PRI_DIS_V << GDMA_ARB_PRI_DIS_S)
#define GDMA_ARB_PRI_DIS_V  0x00000001U
#define GDMA_ARB_PRI_DIS_S  2
/** GDMA_CLK_EN : R/W; bitpos: [3]; default: 0;
 *  0: Enable the clock only when application writes registers. 1: Force the clock on
 *  for registers.
 */
#define GDMA_CLK_EN    (BIT(3))
#define GDMA_CLK_EN_M  (GDMA_CLK_EN_V << GDMA_CLK_EN_S)
#define GDMA_CLK_EN_V  0x00000001U
#define GDMA_CLK_EN_S  3

/** GDMA_DATE_REG register
 *  Version control register
 */
#define GDMA_DATE_REG (DR_REG_GDMA_BASE + 0x48)
/** GDMA_DATE : R/W; bitpos: [31:0]; default: 33587792;
 *  This is the version control register.
 */
#define GDMA_DATE    0xFFFFFFFFU
#define GDMA_DATE_M  (GDMA_DATE_V << GDMA_DATE_S)
#define GDMA_DATE_V  0xFFFFFFFFU
#define GDMA_DATE_S  0

/** GDMA_IN_CONF0_CH0_REG register
 *  Configuration register 0 of RX channel 0
 */
#define GDMA_IN_CONF0_CH0_REG (DR_REG_GDMA_BASE + 0x70)
/** GDMA_IN_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset GDMA channel 0 RX FSM and RX FIFO pointer.
 */
#define GDMA_IN_RST_CH0    (BIT(0))
#define GDMA_IN_RST_CH0_M  (GDMA_IN_RST_CH0_V << GDMA_IN_RST_CH0_S)
#define GDMA_IN_RST_CH0_V  0x00000001U
#define GDMA_IN_RST_CH0_S  0
/** GDMA_IN_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  This bit is used to fill the owner bit of receive descriptor by hardware of receive
 *  descriptor.
 */
#define GDMA_IN_LOOP_TEST_CH0    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH0_M  (GDMA_IN_LOOP_TEST_CH0_V << GDMA_IN_LOOP_TEST_CH0_S)
#define GDMA_IN_LOOP_TEST_CH0_V  0x00000001U
#define GDMA_IN_LOOP_TEST_CH0_S  1
/** GDMA_INDSCR_BURST_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for RX channel 0 reading descriptor
 *  when accessing internal RAM.
 */
#define GDMA_INDSCR_BURST_EN_CH0    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH0_M  (GDMA_INDSCR_BURST_EN_CH0_V << GDMA_INDSCR_BURST_EN_CH0_S)
#define GDMA_INDSCR_BURST_EN_CH0_V  0x00000001U
#define GDMA_INDSCR_BURST_EN_CH0_S  2
/** GDMA_IN_DATA_BURST_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for RX channel 0 receiving data
 *  when accessing internal RAM.
 */
#define GDMA_IN_DATA_BURST_EN_CH0    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH0_M  (GDMA_IN_DATA_BURST_EN_CH0_V << GDMA_IN_DATA_BURST_EN_CH0_S)
#define GDMA_IN_DATA_BURST_EN_CH0_V  0x00000001U
#define GDMA_IN_DATA_BURST_EN_CH0_S  3
/** GDMA_MEM_TRANS_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via GDMA.
 */
#define GDMA_MEM_TRANS_EN_CH0    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH0_M  (GDMA_MEM_TRANS_EN_CH0_V << GDMA_MEM_TRANS_EN_CH0_S)
#define GDMA_MEM_TRANS_EN_CH0_V  0x00000001U
#define GDMA_MEM_TRANS_EN_CH0_S  4

/** GDMA_IN_CONF1_CH0_REG register
 *  Configuration register 1 of RX channel 0
 */
#define GDMA_IN_CONF1_CH0_REG (DR_REG_GDMA_BASE + 0x74)
/** GDMA_IN_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *
 *  Set this bit to enable checking the owner attribute of the descriptor.
 */
#define GDMA_IN_CHECK_OWNER_CH0    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH0_M  (GDMA_IN_CHECK_OWNER_CH0_V << GDMA_IN_CHECK_OWNER_CH0_S)
#define GDMA_IN_CHECK_OWNER_CH0_V  0x00000001U
#define GDMA_IN_CHECK_OWNER_CH0_S  12

/** GDMA_INFIFO_STATUS_CH0_REG register
 *  RX FIFO status of RX channel 0
 */
#define GDMA_INFIFO_STATUS_CH0_REG (DR_REG_GDMA_BASE + 0x78)
/** GDMA_INFIFO_FULL_CH0 : RO; bitpos: [0]; default: 0;
 *  L1 RX FIFO full signal for RX channel 0.
 */
#define GDMA_INFIFO_FULL_CH0    (BIT(0))
#define GDMA_INFIFO_FULL_CH0_M  (GDMA_INFIFO_FULL_CH0_V << GDMA_INFIFO_FULL_CH0_S)
#define GDMA_INFIFO_FULL_CH0_V  0x00000001U
#define GDMA_INFIFO_FULL_CH0_S  0
/** GDMA_INFIFO_EMPTY_CH0 : RO; bitpos: [1]; default: 0;
 *  L1 RX FIFO empty signal for RX channel 0.
 */
#define GDMA_INFIFO_EMPTY_CH0    (BIT(1))
#define GDMA_INFIFO_EMPTY_CH0_M  (GDMA_INFIFO_EMPTY_CH0_V << GDMA_INFIFO_EMPTY_CH0_S)
#define GDMA_INFIFO_EMPTY_CH0_V  0x00000001U
#define GDMA_INFIFO_EMPTY_CH0_S  1
/** GDMA_INFIFO_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 RX FIFO for RX channel 0.
 */
#define GDMA_INFIFO_CNT_CH0    0x0000003FU
#define GDMA_INFIFO_CNT_CH0_M  (GDMA_INFIFO_CNT_CH0_V << GDMA_INFIFO_CNT_CH0_S)
#define GDMA_INFIFO_CNT_CH0_V  0x0000003FU
#define GDMA_INFIFO_CNT_CH0_S  2
/** GDMA_IN_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_1B_CH0    (BIT(23))
#define GDMA_IN_REMAIN_UNDER_1B_CH0_M  (GDMA_IN_REMAIN_UNDER_1B_CH0_V << GDMA_IN_REMAIN_UNDER_1B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_1B_CH0_S  23
/** GDMA_IN_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_2B_CH0    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_2B_CH0_M  (GDMA_IN_REMAIN_UNDER_2B_CH0_V << GDMA_IN_REMAIN_UNDER_2B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_2B_CH0_S  24
/** GDMA_IN_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_3B_CH0    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_3B_CH0_M  (GDMA_IN_REMAIN_UNDER_3B_CH0_V << GDMA_IN_REMAIN_UNDER_3B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_3B_CH0_S  25
/** GDMA_IN_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_4B_CH0    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_4B_CH0_M  (GDMA_IN_REMAIN_UNDER_4B_CH0_V << GDMA_IN_REMAIN_UNDER_4B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_4B_CH0_S  26
/** GDMA_IN_BUF_HUNGRY_CH0 : RO; bitpos: [27]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_BUF_HUNGRY_CH0    (BIT(27))
#define GDMA_IN_BUF_HUNGRY_CH0_M  (GDMA_IN_BUF_HUNGRY_CH0_V << GDMA_IN_BUF_HUNGRY_CH0_S)
#define GDMA_IN_BUF_HUNGRY_CH0_V  0x00000001U
#define GDMA_IN_BUF_HUNGRY_CH0_S  27

/** GDMA_IN_POP_CH0_REG register
 *  Pop control register of RX channel 0
 */
#define GDMA_IN_POP_CH0_REG (DR_REG_GDMA_BASE + 0x7c)
/** GDMA_INFIFO_RDATA_CH0 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from GDMA FIFO (intended for debugging).
 */
#define GDMA_INFIFO_RDATA_CH0    0x00000FFFU
#define GDMA_INFIFO_RDATA_CH0_M  (GDMA_INFIFO_RDATA_CH0_V << GDMA_INFIFO_RDATA_CH0_S)
#define GDMA_INFIFO_RDATA_CH0_V  0x00000FFFU
#define GDMA_INFIFO_RDATA_CH0_S  0
/** GDMA_INFIFO_POP_CH0 : R/W/SC; bitpos: [12]; default: 0;
 *  Set this bit to pop data from GDMA FIFO (intended for debugging).
 */
#define GDMA_INFIFO_POP_CH0    (BIT(12))
#define GDMA_INFIFO_POP_CH0_M  (GDMA_INFIFO_POP_CH0_V << GDMA_INFIFO_POP_CH0_S)
#define GDMA_INFIFO_POP_CH0_V  0x00000001U
#define GDMA_INFIFO_POP_CH0_S  12

/** GDMA_IN_LINK_CH0_REG register
 *  Link descriptor configuration and control register of RX channel 0
 */
#define GDMA_IN_LINK_CH0_REG (DR_REG_GDMA_BASE + 0x80)
/** GDMA_INLINK_ADDR_CH0 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first receive
 *  descriptor's address.
 */
#define GDMA_INLINK_ADDR_CH0    0x000FFFFFU
#define GDMA_INLINK_ADDR_CH0_M  (GDMA_INLINK_ADDR_CH0_V << GDMA_INLINK_ADDR_CH0_S)
#define GDMA_INLINK_ADDR_CH0_V  0x000FFFFFU
#define GDMA_INLINK_ADDR_CH0_S  0
/** GDMA_INLINK_AUTO_RET_CH0 : R/W; bitpos: [20]; default: 0;
 *  Set this bit to return to current receive descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define GDMA_INLINK_AUTO_RET_CH0    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH0_M  (GDMA_INLINK_AUTO_RET_CH0_V << GDMA_INLINK_AUTO_RET_CH0_S)
#define GDMA_INLINK_AUTO_RET_CH0_V  0x00000001U
#define GDMA_INLINK_AUTO_RET_CH0_S  20
/** GDMA_INLINK_STOP_CH0 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop GDMA's receive channel from receiving data.
 */
#define GDMA_INLINK_STOP_CH0    (BIT(21))
#define GDMA_INLINK_STOP_CH0_M  (GDMA_INLINK_STOP_CH0_V << GDMA_INLINK_STOP_CH0_S)
#define GDMA_INLINK_STOP_CH0_V  0x00000001U
#define GDMA_INLINK_STOP_CH0_S  21
/** GDMA_INLINK_START_CH0 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to enable GDMA's receive channel from receiving data.
 */
#define GDMA_INLINK_START_CH0    (BIT(22))
#define GDMA_INLINK_START_CH0_M  (GDMA_INLINK_START_CH0_V << GDMA_INLINK_START_CH0_S)
#define GDMA_INLINK_START_CH0_V  0x00000001U
#define GDMA_INLINK_START_CH0_S  22
/** GDMA_INLINK_RESTART_CH0 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new receive descriptor.
 */
#define GDMA_INLINK_RESTART_CH0    (BIT(23))
#define GDMA_INLINK_RESTART_CH0_M  (GDMA_INLINK_RESTART_CH0_V << GDMA_INLINK_RESTART_CH0_S)
#define GDMA_INLINK_RESTART_CH0_V  0x00000001U
#define GDMA_INLINK_RESTART_CH0_S  23
/** GDMA_INLINK_PARK_CH0 : RO; bitpos: [24]; default: 0;
 *  1: the receive descriptor's FSM is in idle state. 0: the receive descriptor's FSM
 *  is working.
 */
#define GDMA_INLINK_PARK_CH0    (BIT(24))
#define GDMA_INLINK_PARK_CH0_M  (GDMA_INLINK_PARK_CH0_V << GDMA_INLINK_PARK_CH0_S)
#define GDMA_INLINK_PARK_CH0_V  0x00000001U
#define GDMA_INLINK_PARK_CH0_S  24

/** GDMA_IN_STATE_CH0_REG register
 *  Receive status of RX channel 0
 */
#define GDMA_IN_STATE_CH0_REG (DR_REG_GDMA_BASE + 0x84)
/** GDMA_INLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the lower 18 bits of the  next receive descriptor address that
 *  is pre-read (but not processed yet). If the current receive descriptor is the last
 *  descriptor, then this field represents the address of the current receive
 *  descriptor.
 */
#define GDMA_INLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH0_M  (GDMA_INLINK_DSCR_ADDR_CH0_V << GDMA_INLINK_DSCR_ADDR_CH0_S)
#define GDMA_INLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH0_S  0
/** GDMA_IN_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_DSCR_STATE_CH0    0x00000003U
#define GDMA_IN_DSCR_STATE_CH0_M  (GDMA_IN_DSCR_STATE_CH0_V << GDMA_IN_DSCR_STATE_CH0_S)
#define GDMA_IN_DSCR_STATE_CH0_V  0x00000003U
#define GDMA_IN_DSCR_STATE_CH0_S  18
/** GDMA_IN_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_STATE_CH0    0x00000007U
#define GDMA_IN_STATE_CH0_M  (GDMA_IN_STATE_CH0_V << GDMA_IN_STATE_CH0_S)
#define GDMA_IN_STATE_CH0_V  0x00000007U
#define GDMA_IN_STATE_CH0_S  20

/** GDMA_IN_SUC_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when EOF occurs of RX channel 0
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0x88)
/** GDMA_IN_SUC_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the receive descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_M  (GDMA_IN_SUC_EOF_DES_ADDR_CH0_V << GDMA_IN_SUC_EOF_DES_ADDR_CH0_S)
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_S  0

/** GDMA_IN_ERR_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when errors occur of RX channel 0
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0x8c)
/** GDMA_IN_ERR_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the receive descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_M  (GDMA_IN_ERR_EOF_DES_ADDR_CH0_V << GDMA_IN_ERR_EOF_DES_ADDR_CH0_S)
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_S  0

/** GDMA_IN_DSCR_CH0_REG register
 *  Address of the next receive descriptor pointed by the current pre-read receive
 *  descriptor on RX channel 0
 */
#define GDMA_IN_DSCR_CH0_REG (DR_REG_GDMA_BASE + 0x90)
/** GDMA_INLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the next receive descriptor x+1 pointed by the current
 *  receive descriptor that is pre-read.
 */
#define GDMA_INLINK_DSCR_CH0    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH0_M  (GDMA_INLINK_DSCR_CH0_V << GDMA_INLINK_DSCR_CH0_S)
#define GDMA_INLINK_DSCR_CH0_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH0_S  0

/** GDMA_IN_DSCR_BF0_CH0_REG register
 *  Address of the current pre-read receive descriptor on RX channel 0
 */
#define GDMA_IN_DSCR_BF0_CH0_REG (DR_REG_GDMA_BASE + 0x94)
/** GDMA_INLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the current receive descriptor x that is pre-read.
 */
#define GDMA_INLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH0_M  (GDMA_INLINK_DSCR_BF0_CH0_V << GDMA_INLINK_DSCR_BF0_CH0_S)
#define GDMA_INLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH0_S  0

/** GDMA_IN_DSCR_BF1_CH0_REG register
 *  Address of the previous pre-read receive descriptor on RX channel 0
 */
#define GDMA_IN_DSCR_BF1_CH0_REG (DR_REG_GDMA_BASE + 0x98)
/** GDMA_INLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the previous receive descriptor x-1 that is pre-read.
 */
#define GDMA_INLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH0_M  (GDMA_INLINK_DSCR_BF1_CH0_V << GDMA_INLINK_DSCR_BF1_CH0_S)
#define GDMA_INLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH0_S  0

/** GDMA_IN_PRI_CH0_REG register
 *  Priority register of RX channel 0
 */
#define GDMA_IN_PRI_CH0_REG (DR_REG_GDMA_BASE + 0x9c)
/** GDMA_RX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of RX channel 0. The larger the value, the higher the priority.
 */
#define GDMA_RX_PRI_CH0    0x0000000FU
#define GDMA_RX_PRI_CH0_M  (GDMA_RX_PRI_CH0_V << GDMA_RX_PRI_CH0_S)
#define GDMA_RX_PRI_CH0_V  0x0000000FU
#define GDMA_RX_PRI_CH0_S  0

/** GDMA_IN_PERI_SEL_CH0_REG register
 *  Peripheral selection of RX channel 0
 */
#define GDMA_IN_PERI_SEL_CH0_REG (DR_REG_GDMA_BASE + 0xa0)
/** GDMA_PERI_IN_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for RX channel 0. 0: SPI2. 1: reserved.
 *  2: UHCI0. 3: I2S. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC. 9 ~ 63: Invalid.
 */
#define GDMA_PERI_IN_SEL_CH0    0x0000003FU
#define GDMA_PERI_IN_SEL_CH0_M  (GDMA_PERI_IN_SEL_CH0_V << GDMA_PERI_IN_SEL_CH0_S)
#define GDMA_PERI_IN_SEL_CH0_V  0x0000003FU
#define GDMA_PERI_IN_SEL_CH0_S  0

/** GDMA_OUT_CONF0_CH0_REG register
 *  Configuration register 0 of TX channel 0
 */
#define GDMA_OUT_CONF0_CH0_REG (DR_REG_GDMA_BASE + 0xd0)
/** GDMA_OUT_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset GDMA channel 0 TX FSM and TX FIFO pointer.
 */
#define GDMA_OUT_RST_CH0    (BIT(0))
#define GDMA_OUT_RST_CH0_M  (GDMA_OUT_RST_CH0_V << GDMA_OUT_RST_CH0_S)
#define GDMA_OUT_RST_CH0_V  0x00000001U
#define GDMA_OUT_RST_CH0_S  0
/** GDMA_OUT_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_LOOP_TEST_CH0    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH0_M  (GDMA_OUT_LOOP_TEST_CH0_V << GDMA_OUT_LOOP_TEST_CH0_S)
#define GDMA_OUT_LOOP_TEST_CH0_V  0x00000001U
#define GDMA_OUT_LOOP_TEST_CH0_S  1
/** GDMA_OUT_AUTO_WRBACK_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in TX buffer
 *  has been transmitted.
 */
#define GDMA_OUT_AUTO_WRBACK_CH0    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH0_M  (GDMA_OUT_AUTO_WRBACK_CH0_V << GDMA_OUT_AUTO_WRBACK_CH0_S)
#define GDMA_OUT_AUTO_WRBACK_CH0_V  0x00000001U
#define GDMA_OUT_AUTO_WRBACK_CH0_S  2
/** GDMA_OUT_EOF_MODE_CH0 : R/W; bitpos: [3]; default: 0;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for TX channel 0 is
 *  generated when data need to transmit has been popped from FIFO in GDMA.
 */
#define GDMA_OUT_EOF_MODE_CH0    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH0_M  (GDMA_OUT_EOF_MODE_CH0_V << GDMA_OUT_EOF_MODE_CH0_S)
#define GDMA_OUT_EOF_MODE_CH0_V  0x00000001U
#define GDMA_OUT_EOF_MODE_CH0_S  3
/** GDMA_OUTDSCR_BURST_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for TX channel 0 reading descriptor
 *  when accessing internal RAM.
 */
#define GDMA_OUTDSCR_BURST_EN_CH0    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH0_M  (GDMA_OUTDSCR_BURST_EN_CH0_V << GDMA_OUTDSCR_BURST_EN_CH0_S)
#define GDMA_OUTDSCR_BURST_EN_CH0_V  0x00000001U
#define GDMA_OUTDSCR_BURST_EN_CH0_S  4
/** GDMA_OUT_DATA_BURST_EN_CH0 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for TX channel 0 transmitting data
 *  when accessing internal RAM.
 */
#define GDMA_OUT_DATA_BURST_EN_CH0    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH0_M  (GDMA_OUT_DATA_BURST_EN_CH0_V << GDMA_OUT_DATA_BURST_EN_CH0_S)
#define GDMA_OUT_DATA_BURST_EN_CH0_V  0x00000001U
#define GDMA_OUT_DATA_BURST_EN_CH0_S  5

/** GDMA_OUT_CONF1_CH0_REG register
 *  Configuration register 1 of TX channel 0
 */
#define GDMA_OUT_CONF1_CH0_REG (DR_REG_GDMA_BASE + 0xd4)
/** GDMA_OUT_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the descriptor.
 */
#define GDMA_OUT_CHECK_OWNER_CH0    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH0_M  (GDMA_OUT_CHECK_OWNER_CH0_V << GDMA_OUT_CHECK_OWNER_CH0_S)
#define GDMA_OUT_CHECK_OWNER_CH0_V  0x00000001U
#define GDMA_OUT_CHECK_OWNER_CH0_S  12

/** GDMA_OUTFIFO_STATUS_CH0_REG register
 *  TX FIFO status of TX channel 0
 */
#define GDMA_OUTFIFO_STATUS_CH0_REG (DR_REG_GDMA_BASE + 0xd8)
/** GDMA_OUTFIFO_FULL_CH0 : RO; bitpos: [0]; default: 0;
 *  L1 TX FIFO full signal for TX channel 0.
 */
#define GDMA_OUTFIFO_FULL_CH0    (BIT(0))
#define GDMA_OUTFIFO_FULL_CH0_M  (GDMA_OUTFIFO_FULL_CH0_V << GDMA_OUTFIFO_FULL_CH0_S)
#define GDMA_OUTFIFO_FULL_CH0_V  0x00000001U
#define GDMA_OUTFIFO_FULL_CH0_S  0
/** GDMA_OUTFIFO_EMPTY_CH0 : RO; bitpos: [1]; default: 0;
 *  L1 TX FIFO empty signal for TX channel 0.
 */
#define GDMA_OUTFIFO_EMPTY_CH0    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_CH0_M  (GDMA_OUTFIFO_EMPTY_CH0_V << GDMA_OUTFIFO_EMPTY_CH0_S)
#define GDMA_OUTFIFO_EMPTY_CH0_V  0x00000001U
#define GDMA_OUTFIFO_EMPTY_CH0_S  1
/** GDMA_OUTFIFO_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 TX FIFO for TX channel 0.
 */
#define GDMA_OUTFIFO_CNT_CH0    0x0000003FU
#define GDMA_OUTFIFO_CNT_CH0_M  (GDMA_OUTFIFO_CNT_CH0_V << GDMA_OUTFIFO_CNT_CH0_S)
#define GDMA_OUTFIFO_CNT_CH0_V  0x0000003FU
#define GDMA_OUTFIFO_CNT_CH0_S  2
/** GDMA_OUT_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_1B_CH0    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_CH0_M  (GDMA_OUT_REMAIN_UNDER_1B_CH0_V << GDMA_OUT_REMAIN_UNDER_1B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_1B_CH0_S  23
/** GDMA_OUT_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_2B_CH0    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_CH0_M  (GDMA_OUT_REMAIN_UNDER_2B_CH0_V << GDMA_OUT_REMAIN_UNDER_2B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_2B_CH0_S  24
/** GDMA_OUT_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_3B_CH0    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_CH0_M  (GDMA_OUT_REMAIN_UNDER_3B_CH0_V << GDMA_OUT_REMAIN_UNDER_3B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_3B_CH0_S  25
/** GDMA_OUT_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_4B_CH0    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_CH0_M  (GDMA_OUT_REMAIN_UNDER_4B_CH0_V << GDMA_OUT_REMAIN_UNDER_4B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_4B_CH0_S  26

/** GDMA_OUT_PUSH_CH0_REG register
 *  Push control register of TX channel 0
 */
#define GDMA_OUT_PUSH_CH0_REG (DR_REG_GDMA_BASE + 0xdc)
/** GDMA_OUTFIFO_WDATA_CH0 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into GDMA FIFO.
 */
#define GDMA_OUTFIFO_WDATA_CH0    0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH0_M  (GDMA_OUTFIFO_WDATA_CH0_V << GDMA_OUTFIFO_WDATA_CH0_S)
#define GDMA_OUTFIFO_WDATA_CH0_V  0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH0_S  0
/** GDMA_OUTFIFO_PUSH_CH0 : R/W/SC; bitpos: [9]; default: 0;
 *  Set this bit to push data into GDMA FIFO.
 */
#define GDMA_OUTFIFO_PUSH_CH0    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH0_M  (GDMA_OUTFIFO_PUSH_CH0_V << GDMA_OUTFIFO_PUSH_CH0_S)
#define GDMA_OUTFIFO_PUSH_CH0_V  0x00000001U
#define GDMA_OUTFIFO_PUSH_CH0_S  9

/** GDMA_OUT_LINK_CH0_REG register
 *  Link descriptor configuration and control register of TX channel 0
 */
#define GDMA_OUT_LINK_CH0_REG (DR_REG_GDMA_BASE + 0xe0)
/** GDMA_OUTLINK_ADDR_CH0 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first transmit
 *  descriptor's address.
 */
#define GDMA_OUTLINK_ADDR_CH0    0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH0_M  (GDMA_OUTLINK_ADDR_CH0_V << GDMA_OUTLINK_ADDR_CH0_S)
#define GDMA_OUTLINK_ADDR_CH0_V  0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH0_S  0
/** GDMA_OUTLINK_STOP_CH0 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop GDMA's transmit channel from transferring data.
 */
#define GDMA_OUTLINK_STOP_CH0    (BIT(20))
#define GDMA_OUTLINK_STOP_CH0_M  (GDMA_OUTLINK_STOP_CH0_V << GDMA_OUTLINK_STOP_CH0_S)
#define GDMA_OUTLINK_STOP_CH0_V  0x00000001U
#define GDMA_OUTLINK_STOP_CH0_S  20
/** GDMA_OUTLINK_START_CH0 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to enable GDMA's transmit channel for data transfer.
 */
#define GDMA_OUTLINK_START_CH0    (BIT(21))
#define GDMA_OUTLINK_START_CH0_M  (GDMA_OUTLINK_START_CH0_V << GDMA_OUTLINK_START_CH0_S)
#define GDMA_OUTLINK_START_CH0_V  0x00000001U
#define GDMA_OUTLINK_START_CH0_S  21
/** GDMA_OUTLINK_RESTART_CH0 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define GDMA_OUTLINK_RESTART_CH0    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH0_M  (GDMA_OUTLINK_RESTART_CH0_V << GDMA_OUTLINK_RESTART_CH0_S)
#define GDMA_OUTLINK_RESTART_CH0_V  0x00000001U
#define GDMA_OUTLINK_RESTART_CH0_S  22
/** GDMA_OUTLINK_PARK_CH0 : RO; bitpos: [23]; default: 0;
 *  1: the transmit descriptor's FSM is in idle state. 0: the transmit descriptor's FSM
 *  is working.
 */
#define GDMA_OUTLINK_PARK_CH0    (BIT(23))
#define GDMA_OUTLINK_PARK_CH0_M  (GDMA_OUTLINK_PARK_CH0_V << GDMA_OUTLINK_PARK_CH0_S)
#define GDMA_OUTLINK_PARK_CH0_V  0x00000001U
#define GDMA_OUTLINK_PARK_CH0_S  23

/** GDMA_OUT_STATE_CH0_REG register
 *  Transmit status of TX channel 0
 */
#define GDMA_OUT_STATE_CH0_REG (DR_REG_GDMA_BASE + 0xe4)
/** GDMA_OUTLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the lower 18 bits of the next receive descriptor address that
 *  is pre-read (but not processed yet). If the current receive descriptor is the last
 *  descriptor, then this field represents the address of the current receive
 *  descriptor.
 */
#define GDMA_OUTLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH0_M  (GDMA_OUTLINK_DSCR_ADDR_CH0_V << GDMA_OUTLINK_DSCR_ADDR_CH0_S)
#define GDMA_OUTLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH0_S  0
/** GDMA_OUT_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_DSCR_STATE_CH0    0x00000003U
#define GDMA_OUT_DSCR_STATE_CH0_M  (GDMA_OUT_DSCR_STATE_CH0_V << GDMA_OUT_DSCR_STATE_CH0_S)
#define GDMA_OUT_DSCR_STATE_CH0_V  0x00000003U
#define GDMA_OUT_DSCR_STATE_CH0_S  18
/** GDMA_OUT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_STATE_CH0    0x00000007U
#define GDMA_OUT_STATE_CH0_M  (GDMA_OUT_STATE_CH0_V << GDMA_OUT_STATE_CH0_S)
#define GDMA_OUT_STATE_CH0_V  0x00000007U
#define GDMA_OUT_STATE_CH0_S  20

/** GDMA_OUT_EOF_DES_ADDR_CH0_REG register
 *  Outlink descriptor address when EOF occurs of TX channel 0
 */
#define GDMA_OUT_EOF_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0xe8)
/** GDMA_OUT_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the transmit descriptor when the EOF bit in
 *  this descriptor is 1.
 */
#define GDMA_OUT_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH0_M  (GDMA_OUT_EOF_DES_ADDR_CH0_V << GDMA_OUT_EOF_DES_ADDR_CH0_S)
#define GDMA_OUT_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH0_S  0

/** GDMA_OUT_EOF_BFR_DES_ADDR_CH0_REG register
 *  The last outlink descriptor address when EOF occurs of TX channel 0
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0xec)
/** GDMA_OUT_EOF_BFR_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the transmit descriptor before the last
 *  transmit descriptor.
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_M  (GDMA_OUT_EOF_BFR_DES_ADDR_CH0_V << GDMA_OUT_EOF_BFR_DES_ADDR_CH0_S)
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_S  0

/** GDMA_OUT_DSCR_CH0_REG register
 *  Address of the next transmit descriptor pointed by the current pre-read transmit
 *  descriptor on TX channel 0
 */
#define GDMA_OUT_DSCR_CH0_REG (DR_REG_GDMA_BASE + 0xf0)
/** GDMA_OUTLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the next transmit descriptor y+1 pointed by the current
 *  transmit descriptor that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_CH0    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH0_M  (GDMA_OUTLINK_DSCR_CH0_V << GDMA_OUTLINK_DSCR_CH0_S)
#define GDMA_OUTLINK_DSCR_CH0_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH0_S  0

/** GDMA_OUT_DSCR_BF0_CH0_REG register
 *  Address of the current pre-read transmit descriptor on TX channel 0
 */
#define GDMA_OUT_DSCR_BF0_CH0_REG (DR_REG_GDMA_BASE + 0xf4)
/** GDMA_OUTLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the current transmit descriptor y that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH0_M  (GDMA_OUTLINK_DSCR_BF0_CH0_V << GDMA_OUTLINK_DSCR_BF0_CH0_S)
#define GDMA_OUTLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH0_S  0

/** GDMA_OUT_DSCR_BF1_CH0_REG register
 *  Address of the previous pre-read transmit descriptor on TX channel 0
 */
#define GDMA_OUT_DSCR_BF1_CH0_REG (DR_REG_GDMA_BASE + 0xf8)
/** GDMA_OUTLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the previous transmit descriptor y-1 that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH0_M  (GDMA_OUTLINK_DSCR_BF1_CH0_V << GDMA_OUTLINK_DSCR_BF1_CH0_S)
#define GDMA_OUTLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH0_S  0

/** GDMA_OUT_PRI_CH0_REG register
 *  Priority register of TX channel 0
 */
#define GDMA_OUT_PRI_CH0_REG (DR_REG_GDMA_BASE + 0xfc)
/** GDMA_TX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of TX channel 0. The larger the value, the higher the priority.
 */
#define GDMA_TX_PRI_CH0    0x0000000FU
#define GDMA_TX_PRI_CH0_M  (GDMA_TX_PRI_CH0_V << GDMA_TX_PRI_CH0_S)
#define GDMA_TX_PRI_CH0_V  0x0000000FU
#define GDMA_TX_PRI_CH0_S  0

/** GDMA_OUT_PERI_SEL_CH0_REG register
 *  Peripheral selection of TX channel 0
 */
#define GDMA_OUT_PERI_SEL_CH0_REG (DR_REG_GDMA_BASE + 0x100)
/** GDMA_PERI_OUT_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for TX channel 0. 0: SPI2. 1: reserved.
 *  2: UHCI0. 3: I2S. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC. 9 ~ 63: Invalid.
 */
#define GDMA_PERI_OUT_SEL_CH0    0x0000003FU
#define GDMA_PERI_OUT_SEL_CH0_M  (GDMA_PERI_OUT_SEL_CH0_V << GDMA_PERI_OUT_SEL_CH0_S)
#define GDMA_PERI_OUT_SEL_CH0_V  0x0000003FU
#define GDMA_PERI_OUT_SEL_CH0_S  0

/** GDMA_IN_CONF0_CH1_REG register
 *  Configuration register 0 of RX channel 1
 */
#define GDMA_IN_CONF0_CH1_REG (DR_REG_GDMA_BASE + 0x130)
/** GDMA_IN_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset GDMA channel 0 RX FSM and RX FIFO pointer.
 */
#define GDMA_IN_RST_CH1    (BIT(0))
#define GDMA_IN_RST_CH1_M  (GDMA_IN_RST_CH1_V << GDMA_IN_RST_CH1_S)
#define GDMA_IN_RST_CH1_V  0x00000001U
#define GDMA_IN_RST_CH1_S  0
/** GDMA_IN_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  This bit is used to fill the owner bit of receive descriptor by hardware of receive
 *  descriptor.
 */
#define GDMA_IN_LOOP_TEST_CH1    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH1_M  (GDMA_IN_LOOP_TEST_CH1_V << GDMA_IN_LOOP_TEST_CH1_S)
#define GDMA_IN_LOOP_TEST_CH1_V  0x00000001U
#define GDMA_IN_LOOP_TEST_CH1_S  1
/** GDMA_INDSCR_BURST_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for RX channel 0 reading descriptor
 *  when accessing internal RAM.
 */
#define GDMA_INDSCR_BURST_EN_CH1    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH1_M  (GDMA_INDSCR_BURST_EN_CH1_V << GDMA_INDSCR_BURST_EN_CH1_S)
#define GDMA_INDSCR_BURST_EN_CH1_V  0x00000001U
#define GDMA_INDSCR_BURST_EN_CH1_S  2
/** GDMA_IN_DATA_BURST_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for RX channel 0 receiving data
 *  when accessing internal RAM.
 */
#define GDMA_IN_DATA_BURST_EN_CH1    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH1_M  (GDMA_IN_DATA_BURST_EN_CH1_V << GDMA_IN_DATA_BURST_EN_CH1_S)
#define GDMA_IN_DATA_BURST_EN_CH1_V  0x00000001U
#define GDMA_IN_DATA_BURST_EN_CH1_S  3
/** GDMA_MEM_TRANS_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via GDMA.
 */
#define GDMA_MEM_TRANS_EN_CH1    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH1_M  (GDMA_MEM_TRANS_EN_CH1_V << GDMA_MEM_TRANS_EN_CH1_S)
#define GDMA_MEM_TRANS_EN_CH1_V  0x00000001U
#define GDMA_MEM_TRANS_EN_CH1_S  4

/** GDMA_IN_CONF1_CH1_REG register
 *  Configuration register 1 of RX channel 1
 */
#define GDMA_IN_CONF1_CH1_REG (DR_REG_GDMA_BASE + 0x134)
/** GDMA_IN_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *
 *  Set this bit to enable checking the owner attribute of the descriptor.
 */
#define GDMA_IN_CHECK_OWNER_CH1    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH1_M  (GDMA_IN_CHECK_OWNER_CH1_V << GDMA_IN_CHECK_OWNER_CH1_S)
#define GDMA_IN_CHECK_OWNER_CH1_V  0x00000001U
#define GDMA_IN_CHECK_OWNER_CH1_S  12

/** GDMA_INFIFO_STATUS_CH1_REG register
 *  RX FIFO status of RX channel 1
 */
#define GDMA_INFIFO_STATUS_CH1_REG (DR_REG_GDMA_BASE + 0x138)
/** GDMA_INFIFO_FULL_CH1 : RO; bitpos: [0]; default: 0;
 *  L1 RX FIFO full signal for RX channel 0.
 */
#define GDMA_INFIFO_FULL_CH1    (BIT(0))
#define GDMA_INFIFO_FULL_CH1_M  (GDMA_INFIFO_FULL_CH1_V << GDMA_INFIFO_FULL_CH1_S)
#define GDMA_INFIFO_FULL_CH1_V  0x00000001U
#define GDMA_INFIFO_FULL_CH1_S  0
/** GDMA_INFIFO_EMPTY_CH1 : RO; bitpos: [1]; default: 0;
 *  L1 RX FIFO empty signal for RX channel 0.
 */
#define GDMA_INFIFO_EMPTY_CH1    (BIT(1))
#define GDMA_INFIFO_EMPTY_CH1_M  (GDMA_INFIFO_EMPTY_CH1_V << GDMA_INFIFO_EMPTY_CH1_S)
#define GDMA_INFIFO_EMPTY_CH1_V  0x00000001U
#define GDMA_INFIFO_EMPTY_CH1_S  1
/** GDMA_INFIFO_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 RX FIFO for RX channel 0.
 */
#define GDMA_INFIFO_CNT_CH1    0x0000003FU
#define GDMA_INFIFO_CNT_CH1_M  (GDMA_INFIFO_CNT_CH1_V << GDMA_INFIFO_CNT_CH1_S)
#define GDMA_INFIFO_CNT_CH1_V  0x0000003FU
#define GDMA_INFIFO_CNT_CH1_S  2
/** GDMA_IN_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_1B_CH1    (BIT(23))
#define GDMA_IN_REMAIN_UNDER_1B_CH1_M  (GDMA_IN_REMAIN_UNDER_1B_CH1_V << GDMA_IN_REMAIN_UNDER_1B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_1B_CH1_S  23
/** GDMA_IN_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_2B_CH1    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_2B_CH1_M  (GDMA_IN_REMAIN_UNDER_2B_CH1_V << GDMA_IN_REMAIN_UNDER_2B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_2B_CH1_S  24
/** GDMA_IN_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_3B_CH1    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_3B_CH1_M  (GDMA_IN_REMAIN_UNDER_3B_CH1_V << GDMA_IN_REMAIN_UNDER_3B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_3B_CH1_S  25
/** GDMA_IN_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_4B_CH1    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_4B_CH1_M  (GDMA_IN_REMAIN_UNDER_4B_CH1_V << GDMA_IN_REMAIN_UNDER_4B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_4B_CH1_S  26
/** GDMA_IN_BUF_HUNGRY_CH1 : RO; bitpos: [27]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_BUF_HUNGRY_CH1    (BIT(27))
#define GDMA_IN_BUF_HUNGRY_CH1_M  (GDMA_IN_BUF_HUNGRY_CH1_V << GDMA_IN_BUF_HUNGRY_CH1_S)
#define GDMA_IN_BUF_HUNGRY_CH1_V  0x00000001U
#define GDMA_IN_BUF_HUNGRY_CH1_S  27

/** GDMA_IN_POP_CH1_REG register
 *  Pop control register of RX channel 1
 */
#define GDMA_IN_POP_CH1_REG (DR_REG_GDMA_BASE + 0x13c)
/** GDMA_INFIFO_RDATA_CH1 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from GDMA FIFO (intended for debugging).
 */
#define GDMA_INFIFO_RDATA_CH1    0x00000FFFU
#define GDMA_INFIFO_RDATA_CH1_M  (GDMA_INFIFO_RDATA_CH1_V << GDMA_INFIFO_RDATA_CH1_S)
#define GDMA_INFIFO_RDATA_CH1_V  0x00000FFFU
#define GDMA_INFIFO_RDATA_CH1_S  0
/** GDMA_INFIFO_POP_CH1 : R/W/SC; bitpos: [12]; default: 0;
 *  Set this bit to pop data from GDMA FIFO (intended for debugging).
 */
#define GDMA_INFIFO_POP_CH1    (BIT(12))
#define GDMA_INFIFO_POP_CH1_M  (GDMA_INFIFO_POP_CH1_V << GDMA_INFIFO_POP_CH1_S)
#define GDMA_INFIFO_POP_CH1_V  0x00000001U
#define GDMA_INFIFO_POP_CH1_S  12

/** GDMA_IN_LINK_CH1_REG register
 *  Link descriptor configuration and control register of RX channel 1
 */
#define GDMA_IN_LINK_CH1_REG (DR_REG_GDMA_BASE + 0x140)
/** GDMA_INLINK_ADDR_CH1 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first receive
 *  descriptor's address.
 */
#define GDMA_INLINK_ADDR_CH1    0x000FFFFFU
#define GDMA_INLINK_ADDR_CH1_M  (GDMA_INLINK_ADDR_CH1_V << GDMA_INLINK_ADDR_CH1_S)
#define GDMA_INLINK_ADDR_CH1_V  0x000FFFFFU
#define GDMA_INLINK_ADDR_CH1_S  0
/** GDMA_INLINK_AUTO_RET_CH1 : R/W; bitpos: [20]; default: 0;
 *  Set this bit to return to current receive descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define GDMA_INLINK_AUTO_RET_CH1    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH1_M  (GDMA_INLINK_AUTO_RET_CH1_V << GDMA_INLINK_AUTO_RET_CH1_S)
#define GDMA_INLINK_AUTO_RET_CH1_V  0x00000001U
#define GDMA_INLINK_AUTO_RET_CH1_S  20
/** GDMA_INLINK_STOP_CH1 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop GDMA's receive channel from receiving data.
 */
#define GDMA_INLINK_STOP_CH1    (BIT(21))
#define GDMA_INLINK_STOP_CH1_M  (GDMA_INLINK_STOP_CH1_V << GDMA_INLINK_STOP_CH1_S)
#define GDMA_INLINK_STOP_CH1_V  0x00000001U
#define GDMA_INLINK_STOP_CH1_S  21
/** GDMA_INLINK_START_CH1 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to enable GDMA's receive channel from receiving data.
 */
#define GDMA_INLINK_START_CH1    (BIT(22))
#define GDMA_INLINK_START_CH1_M  (GDMA_INLINK_START_CH1_V << GDMA_INLINK_START_CH1_S)
#define GDMA_INLINK_START_CH1_V  0x00000001U
#define GDMA_INLINK_START_CH1_S  22
/** GDMA_INLINK_RESTART_CH1 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new receive descriptor.
 */
#define GDMA_INLINK_RESTART_CH1    (BIT(23))
#define GDMA_INLINK_RESTART_CH1_M  (GDMA_INLINK_RESTART_CH1_V << GDMA_INLINK_RESTART_CH1_S)
#define GDMA_INLINK_RESTART_CH1_V  0x00000001U
#define GDMA_INLINK_RESTART_CH1_S  23
/** GDMA_INLINK_PARK_CH1 : RO; bitpos: [24]; default: 0;
 *  1: the receive descriptor's FSM is in idle state. 0: the receive descriptor's FSM
 *  is working.
 */
#define GDMA_INLINK_PARK_CH1    (BIT(24))
#define GDMA_INLINK_PARK_CH1_M  (GDMA_INLINK_PARK_CH1_V << GDMA_INLINK_PARK_CH1_S)
#define GDMA_INLINK_PARK_CH1_V  0x00000001U
#define GDMA_INLINK_PARK_CH1_S  24

/** GDMA_IN_STATE_CH1_REG register
 *  Receive status of RX channel 1
 */
#define GDMA_IN_STATE_CH1_REG (DR_REG_GDMA_BASE + 0x144)
/** GDMA_INLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the lower 18 bits of the  next receive descriptor address that
 *  is pre-read (but not processed yet). If the current receive descriptor is the last
 *  descriptor, then this field represents the address of the current receive
 *  descriptor.
 */
#define GDMA_INLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH1_M  (GDMA_INLINK_DSCR_ADDR_CH1_V << GDMA_INLINK_DSCR_ADDR_CH1_S)
#define GDMA_INLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH1_S  0
/** GDMA_IN_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_DSCR_STATE_CH1    0x00000003U
#define GDMA_IN_DSCR_STATE_CH1_M  (GDMA_IN_DSCR_STATE_CH1_V << GDMA_IN_DSCR_STATE_CH1_S)
#define GDMA_IN_DSCR_STATE_CH1_V  0x00000003U
#define GDMA_IN_DSCR_STATE_CH1_S  18
/** GDMA_IN_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_STATE_CH1    0x00000007U
#define GDMA_IN_STATE_CH1_M  (GDMA_IN_STATE_CH1_V << GDMA_IN_STATE_CH1_S)
#define GDMA_IN_STATE_CH1_V  0x00000007U
#define GDMA_IN_STATE_CH1_S  20

/** GDMA_IN_SUC_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when EOF occurs of RX channel 1
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x148)
/** GDMA_IN_SUC_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the receive descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_M  (GDMA_IN_SUC_EOF_DES_ADDR_CH1_V << GDMA_IN_SUC_EOF_DES_ADDR_CH1_S)
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_S  0

/** GDMA_IN_ERR_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when errors occur of RX channel 1
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x14c)
/** GDMA_IN_ERR_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the receive descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_M  (GDMA_IN_ERR_EOF_DES_ADDR_CH1_V << GDMA_IN_ERR_EOF_DES_ADDR_CH1_S)
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_S  0

/** GDMA_IN_DSCR_CH1_REG register
 *  Address of the next receive descriptor pointed by the current pre-read receive
 *  descriptor on RX channel 1
 */
#define GDMA_IN_DSCR_CH1_REG (DR_REG_GDMA_BASE + 0x150)
/** GDMA_INLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the next receive descriptor x+1 pointed by the current
 *  receive descriptor that is pre-read.
 */
#define GDMA_INLINK_DSCR_CH1    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH1_M  (GDMA_INLINK_DSCR_CH1_V << GDMA_INLINK_DSCR_CH1_S)
#define GDMA_INLINK_DSCR_CH1_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH1_S  0

/** GDMA_IN_DSCR_BF0_CH1_REG register
 *  Address of the current pre-read receive descriptor on RX channel 1
 */
#define GDMA_IN_DSCR_BF0_CH1_REG (DR_REG_GDMA_BASE + 0x154)
/** GDMA_INLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the current receive descriptor x that is pre-read.
 */
#define GDMA_INLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH1_M  (GDMA_INLINK_DSCR_BF0_CH1_V << GDMA_INLINK_DSCR_BF0_CH1_S)
#define GDMA_INLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH1_S  0

/** GDMA_IN_DSCR_BF1_CH1_REG register
 *  Address of the previous pre-read receive descriptor on RX channel 1
 */
#define GDMA_IN_DSCR_BF1_CH1_REG (DR_REG_GDMA_BASE + 0x158)
/** GDMA_INLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the previous receive descriptor x-1 that is pre-read.
 */
#define GDMA_INLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH1_M  (GDMA_INLINK_DSCR_BF1_CH1_V << GDMA_INLINK_DSCR_BF1_CH1_S)
#define GDMA_INLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH1_S  0

/** GDMA_IN_PRI_CH1_REG register
 *  Priority register of RX channel 1
 */
#define GDMA_IN_PRI_CH1_REG (DR_REG_GDMA_BASE + 0x15c)
/** GDMA_RX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of RX channel 0. The larger the value, the higher the priority.
 */
#define GDMA_RX_PRI_CH1    0x0000000FU
#define GDMA_RX_PRI_CH1_M  (GDMA_RX_PRI_CH1_V << GDMA_RX_PRI_CH1_S)
#define GDMA_RX_PRI_CH1_V  0x0000000FU
#define GDMA_RX_PRI_CH1_S  0

/** GDMA_IN_PERI_SEL_CH1_REG register
 *  Peripheral selection of RX channel 1
 */
#define GDMA_IN_PERI_SEL_CH1_REG (DR_REG_GDMA_BASE + 0x160)
/** GDMA_PERI_IN_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for RX channel 0. 0: SPI2. 1: reserved.
 *  2: UHCI0. 3: I2S. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC. 9 ~ 63: Invalid.
 */
#define GDMA_PERI_IN_SEL_CH1    0x0000003FU
#define GDMA_PERI_IN_SEL_CH1_M  (GDMA_PERI_IN_SEL_CH1_V << GDMA_PERI_IN_SEL_CH1_S)
#define GDMA_PERI_IN_SEL_CH1_V  0x0000003FU
#define GDMA_PERI_IN_SEL_CH1_S  0

/** GDMA_OUT_CONF0_CH1_REG register
 *  Configuration register 0 of TX channel 1
 */
#define GDMA_OUT_CONF0_CH1_REG (DR_REG_GDMA_BASE + 0x190)
/** GDMA_OUT_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset GDMA channel 0 TX FSM and TX FIFO pointer.
 */
#define GDMA_OUT_RST_CH1    (BIT(0))
#define GDMA_OUT_RST_CH1_M  (GDMA_OUT_RST_CH1_V << GDMA_OUT_RST_CH1_S)
#define GDMA_OUT_RST_CH1_V  0x00000001U
#define GDMA_OUT_RST_CH1_S  0
/** GDMA_OUT_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_LOOP_TEST_CH1    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH1_M  (GDMA_OUT_LOOP_TEST_CH1_V << GDMA_OUT_LOOP_TEST_CH1_S)
#define GDMA_OUT_LOOP_TEST_CH1_V  0x00000001U
#define GDMA_OUT_LOOP_TEST_CH1_S  1
/** GDMA_OUT_AUTO_WRBACK_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in TX buffer
 *  has been transmitted.
 */
#define GDMA_OUT_AUTO_WRBACK_CH1    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH1_M  (GDMA_OUT_AUTO_WRBACK_CH1_V << GDMA_OUT_AUTO_WRBACK_CH1_S)
#define GDMA_OUT_AUTO_WRBACK_CH1_V  0x00000001U
#define GDMA_OUT_AUTO_WRBACK_CH1_S  2
/** GDMA_OUT_EOF_MODE_CH1 : R/W; bitpos: [3]; default: 0;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for TX channel 0 is
 *  generated when data need to transmit has been popped from FIFO in GDMA.
 */
#define GDMA_OUT_EOF_MODE_CH1    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH1_M  (GDMA_OUT_EOF_MODE_CH1_V << GDMA_OUT_EOF_MODE_CH1_S)
#define GDMA_OUT_EOF_MODE_CH1_V  0x00000001U
#define GDMA_OUT_EOF_MODE_CH1_S  3
/** GDMA_OUTDSCR_BURST_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for TX channel 0 reading descriptor
 *  when accessing internal RAM.
 */
#define GDMA_OUTDSCR_BURST_EN_CH1    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH1_M  (GDMA_OUTDSCR_BURST_EN_CH1_V << GDMA_OUTDSCR_BURST_EN_CH1_S)
#define GDMA_OUTDSCR_BURST_EN_CH1_V  0x00000001U
#define GDMA_OUTDSCR_BURST_EN_CH1_S  4
/** GDMA_OUT_DATA_BURST_EN_CH1 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for TX channel 0 transmitting data
 *  when accessing internal RAM.
 */
#define GDMA_OUT_DATA_BURST_EN_CH1    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH1_M  (GDMA_OUT_DATA_BURST_EN_CH1_V << GDMA_OUT_DATA_BURST_EN_CH1_S)
#define GDMA_OUT_DATA_BURST_EN_CH1_V  0x00000001U
#define GDMA_OUT_DATA_BURST_EN_CH1_S  5

/** GDMA_OUT_CONF1_CH1_REG register
 *  Configuration register 1 of TX channel 1
 */
#define GDMA_OUT_CONF1_CH1_REG (DR_REG_GDMA_BASE + 0x194)
/** GDMA_OUT_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the descriptor.
 */
#define GDMA_OUT_CHECK_OWNER_CH1    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH1_M  (GDMA_OUT_CHECK_OWNER_CH1_V << GDMA_OUT_CHECK_OWNER_CH1_S)
#define GDMA_OUT_CHECK_OWNER_CH1_V  0x00000001U
#define GDMA_OUT_CHECK_OWNER_CH1_S  12

/** GDMA_OUTFIFO_STATUS_CH1_REG register
 *  TX FIFO status of TX channel 1
 */
#define GDMA_OUTFIFO_STATUS_CH1_REG (DR_REG_GDMA_BASE + 0x198)
/** GDMA_OUTFIFO_FULL_CH1 : RO; bitpos: [0]; default: 0;
 *  L1 TX FIFO full signal for TX channel 0.
 */
#define GDMA_OUTFIFO_FULL_CH1    (BIT(0))
#define GDMA_OUTFIFO_FULL_CH1_M  (GDMA_OUTFIFO_FULL_CH1_V << GDMA_OUTFIFO_FULL_CH1_S)
#define GDMA_OUTFIFO_FULL_CH1_V  0x00000001U
#define GDMA_OUTFIFO_FULL_CH1_S  0
/** GDMA_OUTFIFO_EMPTY_CH1 : RO; bitpos: [1]; default: 0;
 *  L1 TX FIFO empty signal for TX channel 0.
 */
#define GDMA_OUTFIFO_EMPTY_CH1    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_CH1_M  (GDMA_OUTFIFO_EMPTY_CH1_V << GDMA_OUTFIFO_EMPTY_CH1_S)
#define GDMA_OUTFIFO_EMPTY_CH1_V  0x00000001U
#define GDMA_OUTFIFO_EMPTY_CH1_S  1
/** GDMA_OUTFIFO_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 TX FIFO for TX channel 0.
 */
#define GDMA_OUTFIFO_CNT_CH1    0x0000003FU
#define GDMA_OUTFIFO_CNT_CH1_M  (GDMA_OUTFIFO_CNT_CH1_V << GDMA_OUTFIFO_CNT_CH1_S)
#define GDMA_OUTFIFO_CNT_CH1_V  0x0000003FU
#define GDMA_OUTFIFO_CNT_CH1_S  2
/** GDMA_OUT_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_1B_CH1    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_CH1_M  (GDMA_OUT_REMAIN_UNDER_1B_CH1_V << GDMA_OUT_REMAIN_UNDER_1B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_1B_CH1_S  23
/** GDMA_OUT_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_2B_CH1    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_CH1_M  (GDMA_OUT_REMAIN_UNDER_2B_CH1_V << GDMA_OUT_REMAIN_UNDER_2B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_2B_CH1_S  24
/** GDMA_OUT_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_3B_CH1    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_CH1_M  (GDMA_OUT_REMAIN_UNDER_3B_CH1_V << GDMA_OUT_REMAIN_UNDER_3B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_3B_CH1_S  25
/** GDMA_OUT_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_4B_CH1    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_CH1_M  (GDMA_OUT_REMAIN_UNDER_4B_CH1_V << GDMA_OUT_REMAIN_UNDER_4B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_4B_CH1_S  26

/** GDMA_OUT_PUSH_CH1_REG register
 *  Push control register of TX channel 1
 */
#define GDMA_OUT_PUSH_CH1_REG (DR_REG_GDMA_BASE + 0x19c)
/** GDMA_OUTFIFO_WDATA_CH1 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into GDMA FIFO.
 */
#define GDMA_OUTFIFO_WDATA_CH1    0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH1_M  (GDMA_OUTFIFO_WDATA_CH1_V << GDMA_OUTFIFO_WDATA_CH1_S)
#define GDMA_OUTFIFO_WDATA_CH1_V  0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH1_S  0
/** GDMA_OUTFIFO_PUSH_CH1 : R/W/SC; bitpos: [9]; default: 0;
 *  Set this bit to push data into GDMA FIFO.
 */
#define GDMA_OUTFIFO_PUSH_CH1    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH1_M  (GDMA_OUTFIFO_PUSH_CH1_V << GDMA_OUTFIFO_PUSH_CH1_S)
#define GDMA_OUTFIFO_PUSH_CH1_V  0x00000001U
#define GDMA_OUTFIFO_PUSH_CH1_S  9

/** GDMA_OUT_LINK_CH1_REG register
 *  Link descriptor configuration and control register of TX channel 1
 */
#define GDMA_OUT_LINK_CH1_REG (DR_REG_GDMA_BASE + 0x1a0)
/** GDMA_OUTLINK_ADDR_CH1 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first transmit
 *  descriptor's address.
 */
#define GDMA_OUTLINK_ADDR_CH1    0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH1_M  (GDMA_OUTLINK_ADDR_CH1_V << GDMA_OUTLINK_ADDR_CH1_S)
#define GDMA_OUTLINK_ADDR_CH1_V  0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH1_S  0
/** GDMA_OUTLINK_STOP_CH1 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop GDMA's transmit channel from transferring data.
 */
#define GDMA_OUTLINK_STOP_CH1    (BIT(20))
#define GDMA_OUTLINK_STOP_CH1_M  (GDMA_OUTLINK_STOP_CH1_V << GDMA_OUTLINK_STOP_CH1_S)
#define GDMA_OUTLINK_STOP_CH1_V  0x00000001U
#define GDMA_OUTLINK_STOP_CH1_S  20
/** GDMA_OUTLINK_START_CH1 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to enable GDMA's transmit channel for data transfer.
 */
#define GDMA_OUTLINK_START_CH1    (BIT(21))
#define GDMA_OUTLINK_START_CH1_M  (GDMA_OUTLINK_START_CH1_V << GDMA_OUTLINK_START_CH1_S)
#define GDMA_OUTLINK_START_CH1_V  0x00000001U
#define GDMA_OUTLINK_START_CH1_S  21
/** GDMA_OUTLINK_RESTART_CH1 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define GDMA_OUTLINK_RESTART_CH1    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH1_M  (GDMA_OUTLINK_RESTART_CH1_V << GDMA_OUTLINK_RESTART_CH1_S)
#define GDMA_OUTLINK_RESTART_CH1_V  0x00000001U
#define GDMA_OUTLINK_RESTART_CH1_S  22
/** GDMA_OUTLINK_PARK_CH1 : RO; bitpos: [23]; default: 0;
 *  1: the transmit descriptor's FSM is in idle state. 0: the transmit descriptor's FSM
 *  is working.
 */
#define GDMA_OUTLINK_PARK_CH1    (BIT(23))
#define GDMA_OUTLINK_PARK_CH1_M  (GDMA_OUTLINK_PARK_CH1_V << GDMA_OUTLINK_PARK_CH1_S)
#define GDMA_OUTLINK_PARK_CH1_V  0x00000001U
#define GDMA_OUTLINK_PARK_CH1_S  23

/** GDMA_OUT_STATE_CH1_REG register
 *  Transmit status of TX channel 1
 */
#define GDMA_OUT_STATE_CH1_REG (DR_REG_GDMA_BASE + 0x1a4)
/** GDMA_OUTLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the lower 18 bits of the next receive descriptor address that
 *  is pre-read (but not processed yet). If the current receive descriptor is the last
 *  descriptor, then this field represents the address of the current receive
 *  descriptor.
 */
#define GDMA_OUTLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH1_M  (GDMA_OUTLINK_DSCR_ADDR_CH1_V << GDMA_OUTLINK_DSCR_ADDR_CH1_S)
#define GDMA_OUTLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH1_S  0
/** GDMA_OUT_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_DSCR_STATE_CH1    0x00000003U
#define GDMA_OUT_DSCR_STATE_CH1_M  (GDMA_OUT_DSCR_STATE_CH1_V << GDMA_OUT_DSCR_STATE_CH1_S)
#define GDMA_OUT_DSCR_STATE_CH1_V  0x00000003U
#define GDMA_OUT_DSCR_STATE_CH1_S  18
/** GDMA_OUT_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_STATE_CH1    0x00000007U
#define GDMA_OUT_STATE_CH1_M  (GDMA_OUT_STATE_CH1_V << GDMA_OUT_STATE_CH1_S)
#define GDMA_OUT_STATE_CH1_V  0x00000007U
#define GDMA_OUT_STATE_CH1_S  20

/** GDMA_OUT_EOF_DES_ADDR_CH1_REG register
 *  Outlink descriptor address when EOF occurs of TX channel 1
 */
#define GDMA_OUT_EOF_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x1a8)
/** GDMA_OUT_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the transmit descriptor when the EOF bit in
 *  this descriptor is 1.
 */
#define GDMA_OUT_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH1_M  (GDMA_OUT_EOF_DES_ADDR_CH1_V << GDMA_OUT_EOF_DES_ADDR_CH1_S)
#define GDMA_OUT_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH1_S  0

/** GDMA_OUT_EOF_BFR_DES_ADDR_CH1_REG register
 *  The last outlink descriptor address when EOF occurs of TX channel 1
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x1ac)
/** GDMA_OUT_EOF_BFR_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the transmit descriptor before the last
 *  transmit descriptor.
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_M  (GDMA_OUT_EOF_BFR_DES_ADDR_CH1_V << GDMA_OUT_EOF_BFR_DES_ADDR_CH1_S)
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_S  0

/** GDMA_OUT_DSCR_CH1_REG register
 *  Address of the next transmit descriptor pointed by the current pre-read transmit
 *  descriptor on TX channel 1
 */
#define GDMA_OUT_DSCR_CH1_REG (DR_REG_GDMA_BASE + 0x1b0)
/** GDMA_OUTLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the next transmit descriptor y+1 pointed by the current
 *  transmit descriptor that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_CH1    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH1_M  (GDMA_OUTLINK_DSCR_CH1_V << GDMA_OUTLINK_DSCR_CH1_S)
#define GDMA_OUTLINK_DSCR_CH1_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH1_S  0

/** GDMA_OUT_DSCR_BF0_CH1_REG register
 *  Address of the current pre-read transmit descriptor on TX channel 1
 */
#define GDMA_OUT_DSCR_BF0_CH1_REG (DR_REG_GDMA_BASE + 0x1b4)
/** GDMA_OUTLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the current transmit descriptor y that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH1_M  (GDMA_OUTLINK_DSCR_BF0_CH1_V << GDMA_OUTLINK_DSCR_BF0_CH1_S)
#define GDMA_OUTLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH1_S  0

/** GDMA_OUT_DSCR_BF1_CH1_REG register
 *  Address of the previous pre-read transmit descriptor on TX channel 1
 */
#define GDMA_OUT_DSCR_BF1_CH1_REG (DR_REG_GDMA_BASE + 0x1b8)
/** GDMA_OUTLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the previous transmit descriptor y-1 that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH1_M  (GDMA_OUTLINK_DSCR_BF1_CH1_V << GDMA_OUTLINK_DSCR_BF1_CH1_S)
#define GDMA_OUTLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH1_S  0

/** GDMA_OUT_PRI_CH1_REG register
 *  Priority register of TX channel 1
 */
#define GDMA_OUT_PRI_CH1_REG (DR_REG_GDMA_BASE + 0x1bc)
/** GDMA_TX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of TX channel 0. The larger the value, the higher the priority.
 */
#define GDMA_TX_PRI_CH1    0x0000000FU
#define GDMA_TX_PRI_CH1_M  (GDMA_TX_PRI_CH1_V << GDMA_TX_PRI_CH1_S)
#define GDMA_TX_PRI_CH1_V  0x0000000FU
#define GDMA_TX_PRI_CH1_S  0

/** GDMA_OUT_PERI_SEL_CH1_REG register
 *  Peripheral selection of TX channel 1
 */
#define GDMA_OUT_PERI_SEL_CH1_REG (DR_REG_GDMA_BASE + 0x1c0)
/** GDMA_PERI_OUT_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for TX channel 0. 0: SPI2. 1: reserved.
 *  2: UHCI0. 3: I2S. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC. 9 ~ 63: Invalid.
 */
#define GDMA_PERI_OUT_SEL_CH1    0x0000003FU
#define GDMA_PERI_OUT_SEL_CH1_M  (GDMA_PERI_OUT_SEL_CH1_V << GDMA_PERI_OUT_SEL_CH1_S)
#define GDMA_PERI_OUT_SEL_CH1_V  0x0000003FU
#define GDMA_PERI_OUT_SEL_CH1_S  0

/** GDMA_IN_CONF0_CH2_REG register
 *  Configuration register 0 of RX channel 2
 */
#define GDMA_IN_CONF0_CH2_REG (DR_REG_GDMA_BASE + 0x1f0)
/** GDMA_IN_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset GDMA channel 0 RX FSM and RX FIFO pointer.
 */
#define GDMA_IN_RST_CH2    (BIT(0))
#define GDMA_IN_RST_CH2_M  (GDMA_IN_RST_CH2_V << GDMA_IN_RST_CH2_S)
#define GDMA_IN_RST_CH2_V  0x00000001U
#define GDMA_IN_RST_CH2_S  0
/** GDMA_IN_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  This bit is used to fill the owner bit of receive descriptor by hardware of receive
 *  descriptor.
 */
#define GDMA_IN_LOOP_TEST_CH2    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH2_M  (GDMA_IN_LOOP_TEST_CH2_V << GDMA_IN_LOOP_TEST_CH2_S)
#define GDMA_IN_LOOP_TEST_CH2_V  0x00000001U
#define GDMA_IN_LOOP_TEST_CH2_S  1
/** GDMA_INDSCR_BURST_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for RX channel 0 reading descriptor
 *  when accessing internal RAM.
 */
#define GDMA_INDSCR_BURST_EN_CH2    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH2_M  (GDMA_INDSCR_BURST_EN_CH2_V << GDMA_INDSCR_BURST_EN_CH2_S)
#define GDMA_INDSCR_BURST_EN_CH2_V  0x00000001U
#define GDMA_INDSCR_BURST_EN_CH2_S  2
/** GDMA_IN_DATA_BURST_EN_CH2 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for RX channel 0 receiving data
 *  when accessing internal RAM.
 */
#define GDMA_IN_DATA_BURST_EN_CH2    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH2_M  (GDMA_IN_DATA_BURST_EN_CH2_V << GDMA_IN_DATA_BURST_EN_CH2_S)
#define GDMA_IN_DATA_BURST_EN_CH2_V  0x00000001U
#define GDMA_IN_DATA_BURST_EN_CH2_S  3
/** GDMA_MEM_TRANS_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via GDMA.
 */
#define GDMA_MEM_TRANS_EN_CH2    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH2_M  (GDMA_MEM_TRANS_EN_CH2_V << GDMA_MEM_TRANS_EN_CH2_S)
#define GDMA_MEM_TRANS_EN_CH2_V  0x00000001U
#define GDMA_MEM_TRANS_EN_CH2_S  4

/** GDMA_IN_CONF1_CH2_REG register
 *  Configuration register 1 of RX channel 2
 */
#define GDMA_IN_CONF1_CH2_REG (DR_REG_GDMA_BASE + 0x1f4)
/** GDMA_IN_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *
 *  Set this bit to enable checking the owner attribute of the descriptor.
 */
#define GDMA_IN_CHECK_OWNER_CH2    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH2_M  (GDMA_IN_CHECK_OWNER_CH2_V << GDMA_IN_CHECK_OWNER_CH2_S)
#define GDMA_IN_CHECK_OWNER_CH2_V  0x00000001U
#define GDMA_IN_CHECK_OWNER_CH2_S  12

/** GDMA_INFIFO_STATUS_CH2_REG register
 *  RX FIFO status of RX channel 2
 */
#define GDMA_INFIFO_STATUS_CH2_REG (DR_REG_GDMA_BASE + 0x1f8)
/** GDMA_INFIFO_FULL_CH2 : RO; bitpos: [0]; default: 0;
 *  L1 RX FIFO full signal for RX channel 0.
 */
#define GDMA_INFIFO_FULL_CH2    (BIT(0))
#define GDMA_INFIFO_FULL_CH2_M  (GDMA_INFIFO_FULL_CH2_V << GDMA_INFIFO_FULL_CH2_S)
#define GDMA_INFIFO_FULL_CH2_V  0x00000001U
#define GDMA_INFIFO_FULL_CH2_S  0
/** GDMA_INFIFO_EMPTY_CH2 : RO; bitpos: [1]; default: 0;
 *  L1 RX FIFO empty signal for RX channel 0.
 */
#define GDMA_INFIFO_EMPTY_CH2    (BIT(1))
#define GDMA_INFIFO_EMPTY_CH2_M  (GDMA_INFIFO_EMPTY_CH2_V << GDMA_INFIFO_EMPTY_CH2_S)
#define GDMA_INFIFO_EMPTY_CH2_V  0x00000001U
#define GDMA_INFIFO_EMPTY_CH2_S  1
/** GDMA_INFIFO_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 RX FIFO for RX channel 0.
 */
#define GDMA_INFIFO_CNT_CH2    0x0000003FU
#define GDMA_INFIFO_CNT_CH2_M  (GDMA_INFIFO_CNT_CH2_V << GDMA_INFIFO_CNT_CH2_S)
#define GDMA_INFIFO_CNT_CH2_V  0x0000003FU
#define GDMA_INFIFO_CNT_CH2_S  2
/** GDMA_IN_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_1B_CH2    (BIT(23))
#define GDMA_IN_REMAIN_UNDER_1B_CH2_M  (GDMA_IN_REMAIN_UNDER_1B_CH2_V << GDMA_IN_REMAIN_UNDER_1B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_1B_CH2_S  23
/** GDMA_IN_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_2B_CH2    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_2B_CH2_M  (GDMA_IN_REMAIN_UNDER_2B_CH2_V << GDMA_IN_REMAIN_UNDER_2B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_2B_CH2_S  24
/** GDMA_IN_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_3B_CH2    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_3B_CH2_M  (GDMA_IN_REMAIN_UNDER_3B_CH2_V << GDMA_IN_REMAIN_UNDER_3B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_3B_CH2_S  25
/** GDMA_IN_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_REMAIN_UNDER_4B_CH2    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_4B_CH2_M  (GDMA_IN_REMAIN_UNDER_4B_CH2_V << GDMA_IN_REMAIN_UNDER_4B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_4B_CH2_S  26
/** GDMA_IN_BUF_HUNGRY_CH2 : RO; bitpos: [27]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_BUF_HUNGRY_CH2    (BIT(27))
#define GDMA_IN_BUF_HUNGRY_CH2_M  (GDMA_IN_BUF_HUNGRY_CH2_V << GDMA_IN_BUF_HUNGRY_CH2_S)
#define GDMA_IN_BUF_HUNGRY_CH2_V  0x00000001U
#define GDMA_IN_BUF_HUNGRY_CH2_S  27

/** GDMA_IN_POP_CH2_REG register
 *  Pop control register of RX channel 2
 */
#define GDMA_IN_POP_CH2_REG (DR_REG_GDMA_BASE + 0x1fc)
/** GDMA_INFIFO_RDATA_CH2 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from GDMA FIFO (intended for debugging).
 */
#define GDMA_INFIFO_RDATA_CH2    0x00000FFFU
#define GDMA_INFIFO_RDATA_CH2_M  (GDMA_INFIFO_RDATA_CH2_V << GDMA_INFIFO_RDATA_CH2_S)
#define GDMA_INFIFO_RDATA_CH2_V  0x00000FFFU
#define GDMA_INFIFO_RDATA_CH2_S  0
/** GDMA_INFIFO_POP_CH2 : R/W/SC; bitpos: [12]; default: 0;
 *  Set this bit to pop data from GDMA FIFO (intended for debugging).
 */
#define GDMA_INFIFO_POP_CH2    (BIT(12))
#define GDMA_INFIFO_POP_CH2_M  (GDMA_INFIFO_POP_CH2_V << GDMA_INFIFO_POP_CH2_S)
#define GDMA_INFIFO_POP_CH2_V  0x00000001U
#define GDMA_INFIFO_POP_CH2_S  12

/** GDMA_IN_LINK_CH2_REG register
 *  Link descriptor configuration and control register of RX channel 2
 */
#define GDMA_IN_LINK_CH2_REG (DR_REG_GDMA_BASE + 0x200)
/** GDMA_INLINK_ADDR_CH2 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first receive
 *  descriptor's address.
 */
#define GDMA_INLINK_ADDR_CH2    0x000FFFFFU
#define GDMA_INLINK_ADDR_CH2_M  (GDMA_INLINK_ADDR_CH2_V << GDMA_INLINK_ADDR_CH2_S)
#define GDMA_INLINK_ADDR_CH2_V  0x000FFFFFU
#define GDMA_INLINK_ADDR_CH2_S  0
/** GDMA_INLINK_AUTO_RET_CH2 : R/W; bitpos: [20]; default: 0;
 *  Set this bit to return to current receive descriptor's address, when there are some
 *  errors in current receiving data.
 */
#define GDMA_INLINK_AUTO_RET_CH2    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH2_M  (GDMA_INLINK_AUTO_RET_CH2_V << GDMA_INLINK_AUTO_RET_CH2_S)
#define GDMA_INLINK_AUTO_RET_CH2_V  0x00000001U
#define GDMA_INLINK_AUTO_RET_CH2_S  20
/** GDMA_INLINK_STOP_CH2 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to stop GDMA's receive channel from receiving data.
 */
#define GDMA_INLINK_STOP_CH2    (BIT(21))
#define GDMA_INLINK_STOP_CH2_M  (GDMA_INLINK_STOP_CH2_V << GDMA_INLINK_STOP_CH2_S)
#define GDMA_INLINK_STOP_CH2_V  0x00000001U
#define GDMA_INLINK_STOP_CH2_S  21
/** GDMA_INLINK_START_CH2 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to enable GDMA's receive channel from receiving data.
 */
#define GDMA_INLINK_START_CH2    (BIT(22))
#define GDMA_INLINK_START_CH2_M  (GDMA_INLINK_START_CH2_V << GDMA_INLINK_START_CH2_S)
#define GDMA_INLINK_START_CH2_V  0x00000001U
#define GDMA_INLINK_START_CH2_S  22
/** GDMA_INLINK_RESTART_CH2 : R/W/SC; bitpos: [23]; default: 0;
 *  Set this bit to mount a new receive descriptor.
 */
#define GDMA_INLINK_RESTART_CH2    (BIT(23))
#define GDMA_INLINK_RESTART_CH2_M  (GDMA_INLINK_RESTART_CH2_V << GDMA_INLINK_RESTART_CH2_S)
#define GDMA_INLINK_RESTART_CH2_V  0x00000001U
#define GDMA_INLINK_RESTART_CH2_S  23
/** GDMA_INLINK_PARK_CH2 : RO; bitpos: [24]; default: 0;
 *  1: the receive descriptor's FSM is in idle state. 0: the receive descriptor's FSM
 *  is working.
 */
#define GDMA_INLINK_PARK_CH2    (BIT(24))
#define GDMA_INLINK_PARK_CH2_M  (GDMA_INLINK_PARK_CH2_V << GDMA_INLINK_PARK_CH2_S)
#define GDMA_INLINK_PARK_CH2_V  0x00000001U
#define GDMA_INLINK_PARK_CH2_S  24

/** GDMA_IN_STATE_CH2_REG register
 *  Receive status of RX channel 2
 */
#define GDMA_IN_STATE_CH2_REG (DR_REG_GDMA_BASE + 0x204)
/** GDMA_INLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the lower 18 bits of the  next receive descriptor address that
 *  is pre-read (but not processed yet). If the current receive descriptor is the last
 *  descriptor, then this field represents the address of the current receive
 *  descriptor.
 */
#define GDMA_INLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH2_M  (GDMA_INLINK_DSCR_ADDR_CH2_V << GDMA_INLINK_DSCR_ADDR_CH2_S)
#define GDMA_INLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH2_S  0
/** GDMA_IN_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_DSCR_STATE_CH2    0x00000003U
#define GDMA_IN_DSCR_STATE_CH2_M  (GDMA_IN_DSCR_STATE_CH2_V << GDMA_IN_DSCR_STATE_CH2_S)
#define GDMA_IN_DSCR_STATE_CH2_V  0x00000003U
#define GDMA_IN_DSCR_STATE_CH2_S  18
/** GDMA_IN_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define GDMA_IN_STATE_CH2    0x00000007U
#define GDMA_IN_STATE_CH2_M  (GDMA_IN_STATE_CH2_V << GDMA_IN_STATE_CH2_S)
#define GDMA_IN_STATE_CH2_V  0x00000007U
#define GDMA_IN_STATE_CH2_S  20

/** GDMA_IN_SUC_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when EOF occurs of RX channel 2
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x208)
/** GDMA_IN_SUC_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the receive descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_M  (GDMA_IN_SUC_EOF_DES_ADDR_CH2_V << GDMA_IN_SUC_EOF_DES_ADDR_CH2_S)
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_S  0

/** GDMA_IN_ERR_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when errors occur of RX channel 2
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x20c)
/** GDMA_IN_ERR_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the receive descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_M  (GDMA_IN_ERR_EOF_DES_ADDR_CH2_V << GDMA_IN_ERR_EOF_DES_ADDR_CH2_S)
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_S  0

/** GDMA_IN_DSCR_CH2_REG register
 *  Address of the next receive descriptor pointed by the current pre-read receive
 *  descriptor on RX channel 2
 */
#define GDMA_IN_DSCR_CH2_REG (DR_REG_GDMA_BASE + 0x210)
/** GDMA_INLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the next receive descriptor x+1 pointed by the current
 *  receive descriptor that is pre-read.
 */
#define GDMA_INLINK_DSCR_CH2    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH2_M  (GDMA_INLINK_DSCR_CH2_V << GDMA_INLINK_DSCR_CH2_S)
#define GDMA_INLINK_DSCR_CH2_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH2_S  0

/** GDMA_IN_DSCR_BF0_CH2_REG register
 *  Address of the current pre-read receive descriptor on RX channel 2
 */
#define GDMA_IN_DSCR_BF0_CH2_REG (DR_REG_GDMA_BASE + 0x214)
/** GDMA_INLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the current receive descriptor x that is pre-read.
 */
#define GDMA_INLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH2_M  (GDMA_INLINK_DSCR_BF0_CH2_V << GDMA_INLINK_DSCR_BF0_CH2_S)
#define GDMA_INLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH2_S  0

/** GDMA_IN_DSCR_BF1_CH2_REG register
 *  Address of the previous pre-read receive descriptor on RX channel 2
 */
#define GDMA_IN_DSCR_BF1_CH2_REG (DR_REG_GDMA_BASE + 0x218)
/** GDMA_INLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the previous receive descriptor x-1 that is pre-read.
 */
#define GDMA_INLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH2_M  (GDMA_INLINK_DSCR_BF1_CH2_V << GDMA_INLINK_DSCR_BF1_CH2_S)
#define GDMA_INLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH2_S  0

/** GDMA_IN_PRI_CH2_REG register
 *  Priority register of RX channel 2
 */
#define GDMA_IN_PRI_CH2_REG (DR_REG_GDMA_BASE + 0x21c)
/** GDMA_RX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of RX channel 0. The larger the value, the higher the priority.
 */
#define GDMA_RX_PRI_CH2    0x0000000FU
#define GDMA_RX_PRI_CH2_M  (GDMA_RX_PRI_CH2_V << GDMA_RX_PRI_CH2_S)
#define GDMA_RX_PRI_CH2_V  0x0000000FU
#define GDMA_RX_PRI_CH2_S  0

/** GDMA_IN_PERI_SEL_CH2_REG register
 *  Peripheral selection of RX channel 2
 */
#define GDMA_IN_PERI_SEL_CH2_REG (DR_REG_GDMA_BASE + 0x220)
/** GDMA_PERI_IN_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for RX channel 0. 0: SPI2. 1: reserved.
 *  2: UHCI0. 3: I2S. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC. 9 ~ 63: Invalid.
 */
#define GDMA_PERI_IN_SEL_CH2    0x0000003FU
#define GDMA_PERI_IN_SEL_CH2_M  (GDMA_PERI_IN_SEL_CH2_V << GDMA_PERI_IN_SEL_CH2_S)
#define GDMA_PERI_IN_SEL_CH2_V  0x0000003FU
#define GDMA_PERI_IN_SEL_CH2_S  0

/** GDMA_OUT_CONF0_CH2_REG register
 *  Configuration register 0 of TX channel 2
 */
#define GDMA_OUT_CONF0_CH2_REG (DR_REG_GDMA_BASE + 0x250)
/** GDMA_OUT_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset GDMA channel 0 TX FSM and TX FIFO pointer.
 */
#define GDMA_OUT_RST_CH2    (BIT(0))
#define GDMA_OUT_RST_CH2_M  (GDMA_OUT_RST_CH2_V << GDMA_OUT_RST_CH2_S)
#define GDMA_OUT_RST_CH2_V  0x00000001U
#define GDMA_OUT_RST_CH2_S  0
/** GDMA_OUT_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_LOOP_TEST_CH2    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH2_M  (GDMA_OUT_LOOP_TEST_CH2_V << GDMA_OUT_LOOP_TEST_CH2_S)
#define GDMA_OUT_LOOP_TEST_CH2_V  0x00000001U
#define GDMA_OUT_LOOP_TEST_CH2_S  1
/** GDMA_OUT_AUTO_WRBACK_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in TX buffer
 *  has been transmitted.
 */
#define GDMA_OUT_AUTO_WRBACK_CH2    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH2_M  (GDMA_OUT_AUTO_WRBACK_CH2_V << GDMA_OUT_AUTO_WRBACK_CH2_S)
#define GDMA_OUT_AUTO_WRBACK_CH2_V  0x00000001U
#define GDMA_OUT_AUTO_WRBACK_CH2_S  2
/** GDMA_OUT_EOF_MODE_CH2 : R/W; bitpos: [3]; default: 0;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for TX channel 0 is
 *  generated when data need to transmit has been popped from FIFO in GDMA.
 */
#define GDMA_OUT_EOF_MODE_CH2    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH2_M  (GDMA_OUT_EOF_MODE_CH2_V << GDMA_OUT_EOF_MODE_CH2_S)
#define GDMA_OUT_EOF_MODE_CH2_V  0x00000001U
#define GDMA_OUT_EOF_MODE_CH2_S  3
/** GDMA_OUTDSCR_BURST_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for TX channel 0 reading descriptor
 *  when accessing internal RAM.
 */
#define GDMA_OUTDSCR_BURST_EN_CH2    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH2_M  (GDMA_OUTDSCR_BURST_EN_CH2_V << GDMA_OUTDSCR_BURST_EN_CH2_S)
#define GDMA_OUTDSCR_BURST_EN_CH2_V  0x00000001U
#define GDMA_OUTDSCR_BURST_EN_CH2_S  4
/** GDMA_OUT_DATA_BURST_EN_CH2 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for TX channel 0 transmitting data
 *  when accessing internal RAM.
 */
#define GDMA_OUT_DATA_BURST_EN_CH2    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH2_M  (GDMA_OUT_DATA_BURST_EN_CH2_V << GDMA_OUT_DATA_BURST_EN_CH2_S)
#define GDMA_OUT_DATA_BURST_EN_CH2_V  0x00000001U
#define GDMA_OUT_DATA_BURST_EN_CH2_S  5

/** GDMA_OUT_CONF1_CH2_REG register
 *  Configuration register 1 of TX channel 2
 */
#define GDMA_OUT_CONF1_CH2_REG (DR_REG_GDMA_BASE + 0x254)
/** GDMA_OUT_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the descriptor.
 */
#define GDMA_OUT_CHECK_OWNER_CH2    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH2_M  (GDMA_OUT_CHECK_OWNER_CH2_V << GDMA_OUT_CHECK_OWNER_CH2_S)
#define GDMA_OUT_CHECK_OWNER_CH2_V  0x00000001U
#define GDMA_OUT_CHECK_OWNER_CH2_S  12

/** GDMA_OUTFIFO_STATUS_CH2_REG register
 *  TX FIFO status of TX channel 2
 */
#define GDMA_OUTFIFO_STATUS_CH2_REG (DR_REG_GDMA_BASE + 0x258)
/** GDMA_OUTFIFO_FULL_CH2 : RO; bitpos: [0]; default: 0;
 *  L1 TX FIFO full signal for TX channel 0.
 */
#define GDMA_OUTFIFO_FULL_CH2    (BIT(0))
#define GDMA_OUTFIFO_FULL_CH2_M  (GDMA_OUTFIFO_FULL_CH2_V << GDMA_OUTFIFO_FULL_CH2_S)
#define GDMA_OUTFIFO_FULL_CH2_V  0x00000001U
#define GDMA_OUTFIFO_FULL_CH2_S  0
/** GDMA_OUTFIFO_EMPTY_CH2 : RO; bitpos: [1]; default: 0;
 *  L1 TX FIFO empty signal for TX channel 0.
 */
#define GDMA_OUTFIFO_EMPTY_CH2    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_CH2_M  (GDMA_OUTFIFO_EMPTY_CH2_V << GDMA_OUTFIFO_EMPTY_CH2_S)
#define GDMA_OUTFIFO_EMPTY_CH2_V  0x00000001U
#define GDMA_OUTFIFO_EMPTY_CH2_S  1
/** GDMA_OUTFIFO_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 TX FIFO for TX channel 0.
 */
#define GDMA_OUTFIFO_CNT_CH2    0x0000003FU
#define GDMA_OUTFIFO_CNT_CH2_M  (GDMA_OUTFIFO_CNT_CH2_V << GDMA_OUTFIFO_CNT_CH2_S)
#define GDMA_OUTFIFO_CNT_CH2_V  0x0000003FU
#define GDMA_OUTFIFO_CNT_CH2_S  2
/** GDMA_OUT_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_1B_CH2    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_CH2_M  (GDMA_OUT_REMAIN_UNDER_1B_CH2_V << GDMA_OUT_REMAIN_UNDER_1B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_1B_CH2_S  23
/** GDMA_OUT_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_2B_CH2    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_CH2_M  (GDMA_OUT_REMAIN_UNDER_2B_CH2_V << GDMA_OUT_REMAIN_UNDER_2B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_2B_CH2_S  24
/** GDMA_OUT_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_3B_CH2    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_CH2_M  (GDMA_OUT_REMAIN_UNDER_3B_CH2_V << GDMA_OUT_REMAIN_UNDER_3B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_3B_CH2_S  25
/** GDMA_OUT_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_REMAIN_UNDER_4B_CH2    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_CH2_M  (GDMA_OUT_REMAIN_UNDER_4B_CH2_V << GDMA_OUT_REMAIN_UNDER_4B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_4B_CH2_S  26

/** GDMA_OUT_PUSH_CH2_REG register
 *  Push control register of TX channel 2
 */
#define GDMA_OUT_PUSH_CH2_REG (DR_REG_GDMA_BASE + 0x25c)
/** GDMA_OUTFIFO_WDATA_CH2 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into GDMA FIFO.
 */
#define GDMA_OUTFIFO_WDATA_CH2    0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH2_M  (GDMA_OUTFIFO_WDATA_CH2_V << GDMA_OUTFIFO_WDATA_CH2_S)
#define GDMA_OUTFIFO_WDATA_CH2_V  0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH2_S  0
/** GDMA_OUTFIFO_PUSH_CH2 : R/W/SC; bitpos: [9]; default: 0;
 *  Set this bit to push data into GDMA FIFO.
 */
#define GDMA_OUTFIFO_PUSH_CH2    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH2_M  (GDMA_OUTFIFO_PUSH_CH2_V << GDMA_OUTFIFO_PUSH_CH2_S)
#define GDMA_OUTFIFO_PUSH_CH2_V  0x00000001U
#define GDMA_OUTFIFO_PUSH_CH2_S  9

/** GDMA_OUT_LINK_CH2_REG register
 *  Link descriptor configuration and control register of TX channel 2
 */
#define GDMA_OUT_LINK_CH2_REG (DR_REG_GDMA_BASE + 0x260)
/** GDMA_OUTLINK_ADDR_CH2 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first transmit
 *  descriptor's address.
 */
#define GDMA_OUTLINK_ADDR_CH2    0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH2_M  (GDMA_OUTLINK_ADDR_CH2_V << GDMA_OUTLINK_ADDR_CH2_S)
#define GDMA_OUTLINK_ADDR_CH2_V  0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH2_S  0
/** GDMA_OUTLINK_STOP_CH2 : R/W/SC; bitpos: [20]; default: 0;
 *  Set this bit to stop GDMA's transmit channel from transferring data.
 */
#define GDMA_OUTLINK_STOP_CH2    (BIT(20))
#define GDMA_OUTLINK_STOP_CH2_M  (GDMA_OUTLINK_STOP_CH2_V << GDMA_OUTLINK_STOP_CH2_S)
#define GDMA_OUTLINK_STOP_CH2_V  0x00000001U
#define GDMA_OUTLINK_STOP_CH2_S  20
/** GDMA_OUTLINK_START_CH2 : R/W/SC; bitpos: [21]; default: 0;
 *  Set this bit to enable GDMA's transmit channel for data transfer.
 */
#define GDMA_OUTLINK_START_CH2    (BIT(21))
#define GDMA_OUTLINK_START_CH2_M  (GDMA_OUTLINK_START_CH2_V << GDMA_OUTLINK_START_CH2_S)
#define GDMA_OUTLINK_START_CH2_V  0x00000001U
#define GDMA_OUTLINK_START_CH2_S  21
/** GDMA_OUTLINK_RESTART_CH2 : R/W/SC; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define GDMA_OUTLINK_RESTART_CH2    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH2_M  (GDMA_OUTLINK_RESTART_CH2_V << GDMA_OUTLINK_RESTART_CH2_S)
#define GDMA_OUTLINK_RESTART_CH2_V  0x00000001U
#define GDMA_OUTLINK_RESTART_CH2_S  22
/** GDMA_OUTLINK_PARK_CH2 : RO; bitpos: [23]; default: 0;
 *  1: the transmit descriptor's FSM is in idle state. 0: the transmit descriptor's FSM
 *  is working.
 */
#define GDMA_OUTLINK_PARK_CH2    (BIT(23))
#define GDMA_OUTLINK_PARK_CH2_M  (GDMA_OUTLINK_PARK_CH2_V << GDMA_OUTLINK_PARK_CH2_S)
#define GDMA_OUTLINK_PARK_CH2_V  0x00000001U
#define GDMA_OUTLINK_PARK_CH2_S  23

/** GDMA_OUT_STATE_CH2_REG register
 *  Transmit status of TX channel 2
 */
#define GDMA_OUT_STATE_CH2_REG (DR_REG_GDMA_BASE + 0x264)
/** GDMA_OUTLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the lower 18 bits of the next receive descriptor address that
 *  is pre-read (but not processed yet). If the current receive descriptor is the last
 *  descriptor, then this field represents the address of the current receive
 *  descriptor.
 */
#define GDMA_OUTLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH2_M  (GDMA_OUTLINK_DSCR_ADDR_CH2_V << GDMA_OUTLINK_DSCR_ADDR_CH2_S)
#define GDMA_OUTLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH2_S  0
/** GDMA_OUT_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_DSCR_STATE_CH2    0x00000003U
#define GDMA_OUT_DSCR_STATE_CH2_M  (GDMA_OUT_DSCR_STATE_CH2_V << GDMA_OUT_DSCR_STATE_CH2_S)
#define GDMA_OUT_DSCR_STATE_CH2_V  0x00000003U
#define GDMA_OUT_DSCR_STATE_CH2_S  18
/** GDMA_OUT_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  Reserved.
 */
#define GDMA_OUT_STATE_CH2    0x00000007U
#define GDMA_OUT_STATE_CH2_M  (GDMA_OUT_STATE_CH2_V << GDMA_OUT_STATE_CH2_S)
#define GDMA_OUT_STATE_CH2_V  0x00000007U
#define GDMA_OUT_STATE_CH2_S  20

/** GDMA_OUT_EOF_DES_ADDR_CH2_REG register
 *  Outlink descriptor address when EOF occurs of TX channel 2
 */
#define GDMA_OUT_EOF_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x268)
/** GDMA_OUT_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the transmit descriptor when the EOF bit in
 *  this descriptor is 1.
 */
#define GDMA_OUT_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH2_M  (GDMA_OUT_EOF_DES_ADDR_CH2_V << GDMA_OUT_EOF_DES_ADDR_CH2_S)
#define GDMA_OUT_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH2_S  0

/** GDMA_OUT_EOF_BFR_DES_ADDR_CH2_REG register
 *  The last outlink descriptor address when EOF occurs of TX channel 2
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x26c)
/** GDMA_OUT_EOF_BFR_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the transmit descriptor before the last
 *  transmit descriptor.
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_M  (GDMA_OUT_EOF_BFR_DES_ADDR_CH2_V << GDMA_OUT_EOF_BFR_DES_ADDR_CH2_S)
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_S  0

/** GDMA_OUT_DSCR_CH2_REG register
 *  Address of the next transmit descriptor pointed by the current pre-read transmit
 *  descriptor on TX channel 2
 */
#define GDMA_OUT_DSCR_CH2_REG (DR_REG_GDMA_BASE + 0x270)
/** GDMA_OUTLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the next transmit descriptor y+1 pointed by the current
 *  transmit descriptor that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_CH2    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH2_M  (GDMA_OUTLINK_DSCR_CH2_V << GDMA_OUTLINK_DSCR_CH2_S)
#define GDMA_OUTLINK_DSCR_CH2_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH2_S  0

/** GDMA_OUT_DSCR_BF0_CH2_REG register
 *  Address of the current pre-read transmit descriptor on TX channel 2
 */
#define GDMA_OUT_DSCR_BF0_CH2_REG (DR_REG_GDMA_BASE + 0x274)
/** GDMA_OUTLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the current transmit descriptor y that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH2_M  (GDMA_OUTLINK_DSCR_BF0_CH2_V << GDMA_OUTLINK_DSCR_BF0_CH2_S)
#define GDMA_OUTLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH2_S  0

/** GDMA_OUT_DSCR_BF1_CH2_REG register
 *  Address of the previous pre-read transmit descriptor on TX channel 2
 */
#define GDMA_OUT_DSCR_BF1_CH2_REG (DR_REG_GDMA_BASE + 0x278)
/** GDMA_OUTLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  Represents the address of the previous transmit descriptor y-1 that is pre-read.
 */
#define GDMA_OUTLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH2_M  (GDMA_OUTLINK_DSCR_BF1_CH2_V << GDMA_OUTLINK_DSCR_BF1_CH2_S)
#define GDMA_OUTLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH2_S  0

/** GDMA_OUT_PRI_CH2_REG register
 *  Priority register of TX channel 2
 */
#define GDMA_OUT_PRI_CH2_REG (DR_REG_GDMA_BASE + 0x27c)
/** GDMA_TX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of TX channel 0. The larger the value, the higher the priority.
 */
#define GDMA_TX_PRI_CH2    0x0000000FU
#define GDMA_TX_PRI_CH2_M  (GDMA_TX_PRI_CH2_V << GDMA_TX_PRI_CH2_S)
#define GDMA_TX_PRI_CH2_V  0x0000000FU
#define GDMA_TX_PRI_CH2_S  0

/** GDMA_OUT_PERI_SEL_CH2_REG register
 *  Peripheral selection of TX channel 2
 */
#define GDMA_OUT_PERI_SEL_CH2_REG (DR_REG_GDMA_BASE + 0x280)
/** GDMA_PERI_OUT_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for TX channel 0. 0: SPI2. 1: reserved.
 *  2: UHCI0. 3: I2S. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC. 9 ~ 63: Invalid.
 */
#define GDMA_PERI_OUT_SEL_CH2    0x0000003FU
#define GDMA_PERI_OUT_SEL_CH2_M  (GDMA_PERI_OUT_SEL_CH2_V << GDMA_PERI_OUT_SEL_CH2_S)
#define GDMA_PERI_OUT_SEL_CH2_V  0x0000003FU
#define GDMA_PERI_OUT_SEL_CH2_S  0

#ifdef __cplusplus
}
#endif
