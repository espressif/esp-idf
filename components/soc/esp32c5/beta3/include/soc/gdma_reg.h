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

/** GDMA_IN_INT_RAW_CH0_REG register
 *  Raw status interrupt of channel 0
 */
#define GDMA_IN_INT_RAW_CH0_REG (DR_REG_GDMA_BASE + 0x0)
/** GDMA_IN_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0.
 */
#define GDMA_IN_DONE_CH0_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_RAW_M  (GDMA_IN_DONE_CH0_INT_RAW_V << GDMA_IN_DONE_CH0_INT_RAW_S)
#define GDMA_IN_DONE_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_RAW_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 0.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_M  (GDMA_IN_SUC_EOF_CH0_INT_RAW_V << GDMA_IN_SUC_EOF_CH0_INT_RAW_S)
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_RAW_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 0. For other peripherals this raw
 *  interrupt is reserved.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_M  (GDMA_IN_ERR_EOF_CH0_INT_RAW_V << GDMA_IN_ERR_EOF_CH0_INT_RAW_S)
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_RAW_S  2
/** GDMA_IN_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 0.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_M  (GDMA_IN_DSCR_ERR_CH0_INT_RAW_V << GDMA_IN_DSCR_ERR_CH0_INT_RAW_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_RAW_S  3
/** GDMA_IN_DSCR_EMPTY_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 0.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_V << GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_RAW_S  4
/** GDMA_INFIFO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  overflow.
 */
#define GDMA_INFIFO_OVF_CH0_INT_RAW    (BIT(5))
#define GDMA_INFIFO_OVF_CH0_INT_RAW_M  (GDMA_INFIFO_OVF_CH0_INT_RAW_V << GDMA_INFIFO_OVF_CH0_INT_RAW_S)
#define GDMA_INFIFO_OVF_CH0_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_RAW_S  5
/** GDMA_INFIFO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  underflow.
 */
#define GDMA_INFIFO_UDF_CH0_INT_RAW    (BIT(6))
#define GDMA_INFIFO_UDF_CH0_INT_RAW_M  (GDMA_INFIFO_UDF_CH0_INT_RAW_V << GDMA_INFIFO_UDF_CH0_INT_RAW_S)
#define GDMA_INFIFO_UDF_CH0_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_RAW_S  6

/** GDMA_IN_INT_ST_CH0_REG register
 *  Masked interrupt of channel 0
 */
#define GDMA_IN_INT_ST_CH0_REG (DR_REG_GDMA_BASE + 0x4)
/** GDMA_IN_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH0_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ST_M  (GDMA_IN_DONE_CH0_INT_ST_V << GDMA_IN_DONE_CH0_INT_ST_S)
#define GDMA_IN_DONE_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_ST_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ST_M  (GDMA_IN_SUC_EOF_CH0_INT_ST_V << GDMA_IN_SUC_EOF_CH0_INT_ST_S)
#define GDMA_IN_SUC_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_ST_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ST_M  (GDMA_IN_ERR_EOF_CH0_INT_ST_V << GDMA_IN_ERR_EOF_CH0_INT_ST_S)
#define GDMA_IN_ERR_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_ST_S  2
/** GDMA_IN_DSCR_ERR_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_M  (GDMA_IN_DSCR_ERR_CH0_INT_ST_V << GDMA_IN_DSCR_ERR_CH0_INT_ST_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_ST_S  3
/** GDMA_IN_DSCR_EMPTY_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_ST_V << GDMA_IN_DSCR_EMPTY_CH0_INT_ST_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ST_S  4
/** GDMA_INFIFO_OVF_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH0_INT_ST    (BIT(5))
#define GDMA_INFIFO_OVF_CH0_INT_ST_M  (GDMA_INFIFO_OVF_CH0_INT_ST_V << GDMA_INFIFO_OVF_CH0_INT_ST_S)
#define GDMA_INFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_ST_S  5
/** GDMA_INFIFO_UDF_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH0_INT_ST    (BIT(6))
#define GDMA_INFIFO_UDF_CH0_INT_ST_M  (GDMA_INFIFO_UDF_CH0_INT_ST_V << GDMA_INFIFO_UDF_CH0_INT_ST_S)
#define GDMA_INFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_ST_S  6

/** GDMA_IN_INT_ENA_CH0_REG register
 *  Interrupt enable bits of channel 0
 */
#define GDMA_IN_INT_ENA_CH0_REG (DR_REG_GDMA_BASE + 0x8)
/** GDMA_IN_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH0_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_ENA_M  (GDMA_IN_DONE_CH0_INT_ENA_V << GDMA_IN_DONE_CH0_INT_ENA_S)
#define GDMA_IN_DONE_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_ENA_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_M  (GDMA_IN_SUC_EOF_CH0_INT_ENA_V << GDMA_IN_SUC_EOF_CH0_INT_ENA_S)
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_ENA_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_M  (GDMA_IN_ERR_EOF_CH0_INT_ENA_V << GDMA_IN_ERR_EOF_CH0_INT_ENA_S)
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_ENA_S  2
/** GDMA_IN_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_M  (GDMA_IN_DSCR_ERR_CH0_INT_ENA_V << GDMA_IN_DSCR_ERR_CH0_INT_ENA_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_ENA_S  3
/** GDMA_IN_DSCR_EMPTY_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_V << GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_ENA_S  4
/** GDMA_INFIFO_OVF_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH0_INT_ENA    (BIT(5))
#define GDMA_INFIFO_OVF_CH0_INT_ENA_M  (GDMA_INFIFO_OVF_CH0_INT_ENA_V << GDMA_INFIFO_OVF_CH0_INT_ENA_S)
#define GDMA_INFIFO_OVF_CH0_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_ENA_S  5
/** GDMA_INFIFO_UDF_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH0_INT_ENA    (BIT(6))
#define GDMA_INFIFO_UDF_CH0_INT_ENA_M  (GDMA_INFIFO_UDF_CH0_INT_ENA_V << GDMA_INFIFO_UDF_CH0_INT_ENA_S)
#define GDMA_INFIFO_UDF_CH0_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_ENA_S  6

/** GDMA_IN_INT_CLR_CH0_REG register
 *  Interrupt clear bits of channel 0
 */
#define GDMA_IN_INT_CLR_CH0_REG (DR_REG_GDMA_BASE + 0xc)
/** GDMA_IN_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH0_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH0_INT_CLR_M  (GDMA_IN_DONE_CH0_INT_CLR_V << GDMA_IN_DONE_CH0_INT_CLR_S)
#define GDMA_IN_DONE_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_DONE_CH0_INT_CLR_S  0
/** GDMA_IN_SUC_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH0_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_M  (GDMA_IN_SUC_EOF_CH0_INT_CLR_V << GDMA_IN_SUC_EOF_CH0_INT_CLR_S)
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH0_INT_CLR_S  1
/** GDMA_IN_ERR_EOF_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH0_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_M  (GDMA_IN_ERR_EOF_CH0_INT_CLR_V << GDMA_IN_ERR_EOF_CH0_INT_CLR_S)
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH0_INT_CLR_S  2
/** GDMA_IN_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_M  (GDMA_IN_DSCR_ERR_CH0_INT_CLR_V << GDMA_IN_DSCR_ERR_CH0_INT_CLR_S)
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH0_INT_CLR_S  3
/** GDMA_IN_DSCR_EMPTY_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_M  (GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_V << GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_S)
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH0_INT_CLR_S  4
/** GDMA_INFIFO_OVF_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH0_INT_CLR    (BIT(5))
#define GDMA_INFIFO_OVF_CH0_INT_CLR_M  (GDMA_INFIFO_OVF_CH0_INT_CLR_V << GDMA_INFIFO_OVF_CH0_INT_CLR_S)
#define GDMA_INFIFO_OVF_CH0_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_OVF_CH0_INT_CLR_S  5
/** GDMA_INFIFO_UDF_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH0_INT_CLR    (BIT(6))
#define GDMA_INFIFO_UDF_CH0_INT_CLR_M  (GDMA_INFIFO_UDF_CH0_INT_CLR_V << GDMA_INFIFO_UDF_CH0_INT_CLR_S)
#define GDMA_INFIFO_UDF_CH0_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_UDF_CH0_INT_CLR_S  6

/** GDMA_IN_INT_RAW_CH1_REG register
 *  Raw status interrupt of channel 0
 */
#define GDMA_IN_INT_RAW_CH1_REG (DR_REG_GDMA_BASE + 0x10)
/** GDMA_IN_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0.
 */
#define GDMA_IN_DONE_CH1_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_RAW_M  (GDMA_IN_DONE_CH1_INT_RAW_V << GDMA_IN_DONE_CH1_INT_RAW_S)
#define GDMA_IN_DONE_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_RAW_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 0.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_M  (GDMA_IN_SUC_EOF_CH1_INT_RAW_V << GDMA_IN_SUC_EOF_CH1_INT_RAW_S)
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_RAW_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 0. For other peripherals this raw
 *  interrupt is reserved.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_M  (GDMA_IN_ERR_EOF_CH1_INT_RAW_V << GDMA_IN_ERR_EOF_CH1_INT_RAW_S)
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_RAW_S  2
/** GDMA_IN_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 0.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_M  (GDMA_IN_DSCR_ERR_CH1_INT_RAW_V << GDMA_IN_DSCR_ERR_CH1_INT_RAW_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_RAW_S  3
/** GDMA_IN_DSCR_EMPTY_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 0.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_V << GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_RAW_S  4
/** GDMA_INFIFO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  overflow.
 */
#define GDMA_INFIFO_OVF_CH1_INT_RAW    (BIT(5))
#define GDMA_INFIFO_OVF_CH1_INT_RAW_M  (GDMA_INFIFO_OVF_CH1_INT_RAW_V << GDMA_INFIFO_OVF_CH1_INT_RAW_S)
#define GDMA_INFIFO_OVF_CH1_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_RAW_S  5
/** GDMA_INFIFO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  underflow.
 */
#define GDMA_INFIFO_UDF_CH1_INT_RAW    (BIT(6))
#define GDMA_INFIFO_UDF_CH1_INT_RAW_M  (GDMA_INFIFO_UDF_CH1_INT_RAW_V << GDMA_INFIFO_UDF_CH1_INT_RAW_S)
#define GDMA_INFIFO_UDF_CH1_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_RAW_S  6

/** GDMA_IN_INT_ST_CH1_REG register
 *  Masked interrupt of channel 0
 */
#define GDMA_IN_INT_ST_CH1_REG (DR_REG_GDMA_BASE + 0x14)
/** GDMA_IN_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH1_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ST_M  (GDMA_IN_DONE_CH1_INT_ST_V << GDMA_IN_DONE_CH1_INT_ST_S)
#define GDMA_IN_DONE_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_ST_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ST_M  (GDMA_IN_SUC_EOF_CH1_INT_ST_V << GDMA_IN_SUC_EOF_CH1_INT_ST_S)
#define GDMA_IN_SUC_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_ST_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ST_M  (GDMA_IN_ERR_EOF_CH1_INT_ST_V << GDMA_IN_ERR_EOF_CH1_INT_ST_S)
#define GDMA_IN_ERR_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_ST_S  2
/** GDMA_IN_DSCR_ERR_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_M  (GDMA_IN_DSCR_ERR_CH1_INT_ST_V << GDMA_IN_DSCR_ERR_CH1_INT_ST_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_ST_S  3
/** GDMA_IN_DSCR_EMPTY_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_ST_V << GDMA_IN_DSCR_EMPTY_CH1_INT_ST_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ST_S  4
/** GDMA_INFIFO_OVF_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH1_INT_ST    (BIT(5))
#define GDMA_INFIFO_OVF_CH1_INT_ST_M  (GDMA_INFIFO_OVF_CH1_INT_ST_V << GDMA_INFIFO_OVF_CH1_INT_ST_S)
#define GDMA_INFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_ST_S  5
/** GDMA_INFIFO_UDF_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH1_INT_ST    (BIT(6))
#define GDMA_INFIFO_UDF_CH1_INT_ST_M  (GDMA_INFIFO_UDF_CH1_INT_ST_V << GDMA_INFIFO_UDF_CH1_INT_ST_S)
#define GDMA_INFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_ST_S  6

/** GDMA_IN_INT_ENA_CH1_REG register
 *  Interrupt enable bits of channel 0
 */
#define GDMA_IN_INT_ENA_CH1_REG (DR_REG_GDMA_BASE + 0x18)
/** GDMA_IN_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH1_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_ENA_M  (GDMA_IN_DONE_CH1_INT_ENA_V << GDMA_IN_DONE_CH1_INT_ENA_S)
#define GDMA_IN_DONE_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_ENA_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_M  (GDMA_IN_SUC_EOF_CH1_INT_ENA_V << GDMA_IN_SUC_EOF_CH1_INT_ENA_S)
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_ENA_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_M  (GDMA_IN_ERR_EOF_CH1_INT_ENA_V << GDMA_IN_ERR_EOF_CH1_INT_ENA_S)
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_ENA_S  2
/** GDMA_IN_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_M  (GDMA_IN_DSCR_ERR_CH1_INT_ENA_V << GDMA_IN_DSCR_ERR_CH1_INT_ENA_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_ENA_S  3
/** GDMA_IN_DSCR_EMPTY_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_V << GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_ENA_S  4
/** GDMA_INFIFO_OVF_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH1_INT_ENA    (BIT(5))
#define GDMA_INFIFO_OVF_CH1_INT_ENA_M  (GDMA_INFIFO_OVF_CH1_INT_ENA_V << GDMA_INFIFO_OVF_CH1_INT_ENA_S)
#define GDMA_INFIFO_OVF_CH1_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_ENA_S  5
/** GDMA_INFIFO_UDF_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH1_INT_ENA    (BIT(6))
#define GDMA_INFIFO_UDF_CH1_INT_ENA_M  (GDMA_INFIFO_UDF_CH1_INT_ENA_V << GDMA_INFIFO_UDF_CH1_INT_ENA_S)
#define GDMA_INFIFO_UDF_CH1_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_ENA_S  6

/** GDMA_IN_INT_CLR_CH1_REG register
 *  Interrupt clear bits of channel 0
 */
#define GDMA_IN_INT_CLR_CH1_REG (DR_REG_GDMA_BASE + 0x1c)
/** GDMA_IN_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH1_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH1_INT_CLR_M  (GDMA_IN_DONE_CH1_INT_CLR_V << GDMA_IN_DONE_CH1_INT_CLR_S)
#define GDMA_IN_DONE_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_DONE_CH1_INT_CLR_S  0
/** GDMA_IN_SUC_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH1_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_M  (GDMA_IN_SUC_EOF_CH1_INT_CLR_V << GDMA_IN_SUC_EOF_CH1_INT_CLR_S)
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH1_INT_CLR_S  1
/** GDMA_IN_ERR_EOF_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH1_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_M  (GDMA_IN_ERR_EOF_CH1_INT_CLR_V << GDMA_IN_ERR_EOF_CH1_INT_CLR_S)
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH1_INT_CLR_S  2
/** GDMA_IN_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_M  (GDMA_IN_DSCR_ERR_CH1_INT_CLR_V << GDMA_IN_DSCR_ERR_CH1_INT_CLR_S)
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH1_INT_CLR_S  3
/** GDMA_IN_DSCR_EMPTY_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_M  (GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_V << GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_S)
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH1_INT_CLR_S  4
/** GDMA_INFIFO_OVF_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH1_INT_CLR    (BIT(5))
#define GDMA_INFIFO_OVF_CH1_INT_CLR_M  (GDMA_INFIFO_OVF_CH1_INT_CLR_V << GDMA_INFIFO_OVF_CH1_INT_CLR_S)
#define GDMA_INFIFO_OVF_CH1_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_OVF_CH1_INT_CLR_S  5
/** GDMA_INFIFO_UDF_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH1_INT_CLR    (BIT(6))
#define GDMA_INFIFO_UDF_CH1_INT_CLR_M  (GDMA_INFIFO_UDF_CH1_INT_CLR_V << GDMA_INFIFO_UDF_CH1_INT_CLR_S)
#define GDMA_INFIFO_UDF_CH1_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_UDF_CH1_INT_CLR_S  6

/** GDMA_IN_INT_RAW_CH2_REG register
 *  Raw status interrupt of channel 0
 */
#define GDMA_IN_INT_RAW_CH2_REG (DR_REG_GDMA_BASE + 0x20)
/** GDMA_IN_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0.
 */
#define GDMA_IN_DONE_CH2_INT_RAW    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_RAW_M  (GDMA_IN_DONE_CH2_INT_RAW_V << GDMA_IN_DONE_CH2_INT_RAW_S)
#define GDMA_IN_DONE_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_RAW_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 0.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_RAW    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_M  (GDMA_IN_SUC_EOF_CH2_INT_RAW_V << GDMA_IN_SUC_EOF_CH2_INT_RAW_S)
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_RAW_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 0. For other peripherals this raw
 *  interrupt is reserved.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_RAW    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_M  (GDMA_IN_ERR_EOF_CH2_INT_RAW_V << GDMA_IN_ERR_EOF_CH2_INT_RAW_S)
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_RAW_S  2
/** GDMA_IN_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 0.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_M  (GDMA_IN_DSCR_ERR_CH2_INT_RAW_V << GDMA_IN_DSCR_ERR_CH2_INT_RAW_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_RAW_S  3
/** GDMA_IN_DSCR_EMPTY_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 0.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_V << GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_RAW_S  4
/** GDMA_INFIFO_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  overflow.
 */
#define GDMA_INFIFO_OVF_CH2_INT_RAW    (BIT(5))
#define GDMA_INFIFO_OVF_CH2_INT_RAW_M  (GDMA_INFIFO_OVF_CH2_INT_RAW_V << GDMA_INFIFO_OVF_CH2_INT_RAW_S)
#define GDMA_INFIFO_OVF_CH2_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_RAW_S  5
/** GDMA_INFIFO_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  underflow.
 */
#define GDMA_INFIFO_UDF_CH2_INT_RAW    (BIT(6))
#define GDMA_INFIFO_UDF_CH2_INT_RAW_M  (GDMA_INFIFO_UDF_CH2_INT_RAW_V << GDMA_INFIFO_UDF_CH2_INT_RAW_S)
#define GDMA_INFIFO_UDF_CH2_INT_RAW_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_RAW_S  6

/** GDMA_IN_INT_ST_CH2_REG register
 *  Masked interrupt of channel 0
 */
#define GDMA_IN_INT_ST_CH2_REG (DR_REG_GDMA_BASE + 0x24)
/** GDMA_IN_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH2_INT_ST    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ST_M  (GDMA_IN_DONE_CH2_INT_ST_V << GDMA_IN_DONE_CH2_INT_ST_S)
#define GDMA_IN_DONE_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_ST_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_ST    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ST_M  (GDMA_IN_SUC_EOF_CH2_INT_ST_V << GDMA_IN_SUC_EOF_CH2_INT_ST_S)
#define GDMA_IN_SUC_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_ST_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_ST    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ST_M  (GDMA_IN_ERR_EOF_CH2_INT_ST_V << GDMA_IN_ERR_EOF_CH2_INT_ST_S)
#define GDMA_IN_ERR_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_ST_S  2
/** GDMA_IN_DSCR_ERR_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_ST    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_M  (GDMA_IN_DSCR_ERR_CH2_INT_ST_V << GDMA_IN_DSCR_ERR_CH2_INT_ST_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_ST_S  3
/** GDMA_IN_DSCR_EMPTY_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_ST_V << GDMA_IN_DSCR_EMPTY_CH2_INT_ST_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ST_S  4
/** GDMA_INFIFO_OVF_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH2_INT_ST    (BIT(5))
#define GDMA_INFIFO_OVF_CH2_INT_ST_M  (GDMA_INFIFO_OVF_CH2_INT_ST_V << GDMA_INFIFO_OVF_CH2_INT_ST_S)
#define GDMA_INFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_ST_S  5
/** GDMA_INFIFO_UDF_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH2_INT_ST    (BIT(6))
#define GDMA_INFIFO_UDF_CH2_INT_ST_M  (GDMA_INFIFO_UDF_CH2_INT_ST_V << GDMA_INFIFO_UDF_CH2_INT_ST_S)
#define GDMA_INFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_ST_S  6

/** GDMA_IN_INT_ENA_CH2_REG register
 *  Interrupt enable bits of channel 0
 */
#define GDMA_IN_INT_ENA_CH2_REG (DR_REG_GDMA_BASE + 0x28)
/** GDMA_IN_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH2_INT_ENA    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_ENA_M  (GDMA_IN_DONE_CH2_INT_ENA_V << GDMA_IN_DONE_CH2_INT_ENA_S)
#define GDMA_IN_DONE_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_ENA_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_ENA    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_M  (GDMA_IN_SUC_EOF_CH2_INT_ENA_V << GDMA_IN_SUC_EOF_CH2_INT_ENA_S)
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_ENA_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_ENA    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_M  (GDMA_IN_ERR_EOF_CH2_INT_ENA_V << GDMA_IN_ERR_EOF_CH2_INT_ENA_S)
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_ENA_S  2
/** GDMA_IN_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_M  (GDMA_IN_DSCR_ERR_CH2_INT_ENA_V << GDMA_IN_DSCR_ERR_CH2_INT_ENA_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_ENA_S  3
/** GDMA_IN_DSCR_EMPTY_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_V << GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_ENA_S  4
/** GDMA_INFIFO_OVF_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH2_INT_ENA    (BIT(5))
#define GDMA_INFIFO_OVF_CH2_INT_ENA_M  (GDMA_INFIFO_OVF_CH2_INT_ENA_V << GDMA_INFIFO_OVF_CH2_INT_ENA_S)
#define GDMA_INFIFO_OVF_CH2_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_ENA_S  5
/** GDMA_INFIFO_UDF_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH2_INT_ENA    (BIT(6))
#define GDMA_INFIFO_UDF_CH2_INT_ENA_M  (GDMA_INFIFO_UDF_CH2_INT_ENA_V << GDMA_INFIFO_UDF_CH2_INT_ENA_S)
#define GDMA_INFIFO_UDF_CH2_INT_ENA_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_ENA_S  6

/** GDMA_IN_INT_CLR_CH2_REG register
 *  Interrupt clear bits of channel 0
 */
#define GDMA_IN_INT_CLR_CH2_REG (DR_REG_GDMA_BASE + 0x2c)
/** GDMA_IN_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define GDMA_IN_DONE_CH2_INT_CLR    (BIT(0))
#define GDMA_IN_DONE_CH2_INT_CLR_M  (GDMA_IN_DONE_CH2_INT_CLR_V << GDMA_IN_DONE_CH2_INT_CLR_S)
#define GDMA_IN_DONE_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_DONE_CH2_INT_CLR_S  0
/** GDMA_IN_SUC_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define GDMA_IN_SUC_EOF_CH2_INT_CLR    (BIT(1))
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_M  (GDMA_IN_SUC_EOF_CH2_INT_CLR_V << GDMA_IN_SUC_EOF_CH2_INT_CLR_S)
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_SUC_EOF_CH2_INT_CLR_S  1
/** GDMA_IN_ERR_EOF_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define GDMA_IN_ERR_EOF_CH2_INT_CLR    (BIT(2))
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_M  (GDMA_IN_ERR_EOF_CH2_INT_CLR_V << GDMA_IN_ERR_EOF_CH2_INT_CLR_S)
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_ERR_EOF_CH2_INT_CLR_S  2
/** GDMA_IN_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR    (BIT(3))
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_M  (GDMA_IN_DSCR_ERR_CH2_INT_CLR_V << GDMA_IN_DSCR_ERR_CH2_INT_CLR_S)
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_ERR_CH2_INT_CLR_S  3
/** GDMA_IN_DSCR_EMPTY_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR    (BIT(4))
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_M  (GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_V << GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_S)
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_V  0x00000001U
#define GDMA_IN_DSCR_EMPTY_CH2_INT_CLR_S  4
/** GDMA_INFIFO_OVF_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_OVF_CH2_INT_CLR    (BIT(5))
#define GDMA_INFIFO_OVF_CH2_INT_CLR_M  (GDMA_INFIFO_OVF_CH2_INT_CLR_V << GDMA_INFIFO_OVF_CH2_INT_CLR_S)
#define GDMA_INFIFO_OVF_CH2_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_OVF_CH2_INT_CLR_S  5
/** GDMA_INFIFO_UDF_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_INFIFO_UDF_CH2_INT_CLR    (BIT(6))
#define GDMA_INFIFO_UDF_CH2_INT_CLR_M  (GDMA_INFIFO_UDF_CH2_INT_CLR_V << GDMA_INFIFO_UDF_CH2_INT_CLR_S)
#define GDMA_INFIFO_UDF_CH2_INT_CLR_V  0x00000001U
#define GDMA_INFIFO_UDF_CH2_INT_CLR_S  6

/** GDMA_OUT_INT_RAW_CH0_REG register
 *  Raw status interrupt of channel 0
 */
#define GDMA_OUT_INT_RAW_CH0_REG (DR_REG_GDMA_BASE + 0x30)
/** GDMA_OUT_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define GDMA_OUT_DONE_CH0_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_RAW_M  (GDMA_OUT_DONE_CH0_INT_RAW_V << GDMA_OUT_DONE_CH0_INT_RAW_S)
#define GDMA_OUT_DONE_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_RAW_S  0
/** GDMA_OUT_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define GDMA_OUT_EOF_CH0_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_RAW_M  (GDMA_OUT_EOF_CH0_INT_RAW_V << GDMA_OUT_EOF_CH0_INT_RAW_S)
#define GDMA_OUT_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_RAW_S  1
/** GDMA_OUT_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel 0.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_M  (GDMA_OUT_DSCR_ERR_CH0_INT_RAW_V << GDMA_OUT_DSCR_ERR_CH0_INT_RAW_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_RAW_S  2
/** GDMA_OUT_TOTAL_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_V << GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_RAW_S  3
/** GDMA_OUTFIFO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  overflow.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW_M  (GDMA_OUTFIFO_OVF_CH0_INT_RAW_V << GDMA_OUTFIFO_OVF_CH0_INT_RAW_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_RAW_S  4
/** GDMA_OUTFIFO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  underflow.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW_M  (GDMA_OUTFIFO_UDF_CH0_INT_RAW_V << GDMA_OUTFIFO_UDF_CH0_INT_RAW_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_RAW_S  5

/** GDMA_OUT_INT_ST_CH0_REG register
 *  Masked interrupt of channel 0
 */
#define GDMA_OUT_INT_ST_CH0_REG (DR_REG_GDMA_BASE + 0x34)
/** GDMA_OUT_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH0_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_ST_M  (GDMA_OUT_DONE_CH0_INT_ST_V << GDMA_OUT_DONE_CH0_INT_ST_S)
#define GDMA_OUT_DONE_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_ST_S  0
/** GDMA_OUT_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH0_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_ST_M  (GDMA_OUT_EOF_CH0_INT_ST_V << GDMA_OUT_EOF_CH0_INT_ST_S)
#define GDMA_OUT_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_ST_S  1
/** GDMA_OUT_DSCR_ERR_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_M  (GDMA_OUT_DSCR_ERR_CH0_INT_ST_V << GDMA_OUT_DSCR_ERR_CH0_INT_ST_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_ST_S  2
/** GDMA_OUT_TOTAL_EOF_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_ST_V << GDMA_OUT_TOTAL_EOF_CH0_INT_ST_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ST_S  3
/** GDMA_OUTFIFO_OVF_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH0_INT_ST_M  (GDMA_OUTFIFO_OVF_CH0_INT_ST_V << GDMA_OUTFIFO_OVF_CH0_INT_ST_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_ST_S  4
/** GDMA_OUTFIFO_UDF_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH0_INT_ST_M  (GDMA_OUTFIFO_UDF_CH0_INT_ST_V << GDMA_OUTFIFO_UDF_CH0_INT_ST_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_ST_S  5

/** GDMA_OUT_INT_ENA_CH0_REG register
 *  Interrupt enable bits of channel 0
 */
#define GDMA_OUT_INT_ENA_CH0_REG (DR_REG_GDMA_BASE + 0x38)
/** GDMA_OUT_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH0_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_ENA_M  (GDMA_OUT_DONE_CH0_INT_ENA_V << GDMA_OUT_DONE_CH0_INT_ENA_S)
#define GDMA_OUT_DONE_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_ENA_S  0
/** GDMA_OUT_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH0_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_ENA_M  (GDMA_OUT_EOF_CH0_INT_ENA_V << GDMA_OUT_EOF_CH0_INT_ENA_S)
#define GDMA_OUT_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_ENA_S  1
/** GDMA_OUT_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_M  (GDMA_OUT_DSCR_ERR_CH0_INT_ENA_V << GDMA_OUT_DSCR_ERR_CH0_INT_ENA_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_ENA_S  2
/** GDMA_OUT_TOTAL_EOF_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_V << GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_ENA_S  3
/** GDMA_OUTFIFO_OVF_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA_M  (GDMA_OUTFIFO_OVF_CH0_INT_ENA_V << GDMA_OUTFIFO_OVF_CH0_INT_ENA_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_ENA_S  4
/** GDMA_OUTFIFO_UDF_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA_M  (GDMA_OUTFIFO_UDF_CH0_INT_ENA_V << GDMA_OUTFIFO_UDF_CH0_INT_ENA_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_ENA_S  5

/** GDMA_OUT_INT_CLR_CH0_REG register
 *  Interrupt clear bits of channel 0
 */
#define GDMA_OUT_INT_CLR_CH0_REG (DR_REG_GDMA_BASE + 0x3c)
/** GDMA_OUT_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH0_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH0_INT_CLR_M  (GDMA_OUT_DONE_CH0_INT_CLR_V << GDMA_OUT_DONE_CH0_INT_CLR_S)
#define GDMA_OUT_DONE_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_DONE_CH0_INT_CLR_S  0
/** GDMA_OUT_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH0_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH0_INT_CLR_M  (GDMA_OUT_EOF_CH0_INT_CLR_V << GDMA_OUT_EOF_CH0_INT_CLR_S)
#define GDMA_OUT_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_EOF_CH0_INT_CLR_S  1
/** GDMA_OUT_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_M  (GDMA_OUT_DSCR_ERR_CH0_INT_CLR_V << GDMA_OUT_DSCR_ERR_CH0_INT_CLR_S)
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH0_INT_CLR_S  2
/** GDMA_OUT_TOTAL_EOF_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_M  (GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_V << GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_S)
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH0_INT_CLR_S  3
/** GDMA_OUTFIFO_OVF_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR_M  (GDMA_OUTFIFO_OVF_CH0_INT_CLR_V << GDMA_OUTFIFO_OVF_CH0_INT_CLR_S)
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH0_INT_CLR_S  4
/** GDMA_OUTFIFO_UDF_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR_M  (GDMA_OUTFIFO_UDF_CH0_INT_CLR_V << GDMA_OUTFIFO_UDF_CH0_INT_CLR_S)
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH0_INT_CLR_S  5

/** GDMA_OUT_INT_RAW_CH1_REG register
 *  Raw status interrupt of channel 0
 */
#define GDMA_OUT_INT_RAW_CH1_REG (DR_REG_GDMA_BASE + 0x40)
/** GDMA_OUT_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define GDMA_OUT_DONE_CH1_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_RAW_M  (GDMA_OUT_DONE_CH1_INT_RAW_V << GDMA_OUT_DONE_CH1_INT_RAW_S)
#define GDMA_OUT_DONE_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_RAW_S  0
/** GDMA_OUT_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define GDMA_OUT_EOF_CH1_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_RAW_M  (GDMA_OUT_EOF_CH1_INT_RAW_V << GDMA_OUT_EOF_CH1_INT_RAW_S)
#define GDMA_OUT_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_RAW_S  1
/** GDMA_OUT_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel 0.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_M  (GDMA_OUT_DSCR_ERR_CH1_INT_RAW_V << GDMA_OUT_DSCR_ERR_CH1_INT_RAW_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_RAW_S  2
/** GDMA_OUT_TOTAL_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_V << GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_RAW_S  3
/** GDMA_OUTFIFO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  overflow.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW_M  (GDMA_OUTFIFO_OVF_CH1_INT_RAW_V << GDMA_OUTFIFO_OVF_CH1_INT_RAW_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_RAW_S  4
/** GDMA_OUTFIFO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  underflow.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW_M  (GDMA_OUTFIFO_UDF_CH1_INT_RAW_V << GDMA_OUTFIFO_UDF_CH1_INT_RAW_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_RAW_S  5

/** GDMA_OUT_INT_ST_CH1_REG register
 *  Masked interrupt of channel 0
 */
#define GDMA_OUT_INT_ST_CH1_REG (DR_REG_GDMA_BASE + 0x44)
/** GDMA_OUT_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH1_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_ST_M  (GDMA_OUT_DONE_CH1_INT_ST_V << GDMA_OUT_DONE_CH1_INT_ST_S)
#define GDMA_OUT_DONE_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_ST_S  0
/** GDMA_OUT_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH1_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_ST_M  (GDMA_OUT_EOF_CH1_INT_ST_V << GDMA_OUT_EOF_CH1_INT_ST_S)
#define GDMA_OUT_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_ST_S  1
/** GDMA_OUT_DSCR_ERR_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_M  (GDMA_OUT_DSCR_ERR_CH1_INT_ST_V << GDMA_OUT_DSCR_ERR_CH1_INT_ST_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_ST_S  2
/** GDMA_OUT_TOTAL_EOF_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_ST_V << GDMA_OUT_TOTAL_EOF_CH1_INT_ST_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ST_S  3
/** GDMA_OUTFIFO_OVF_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH1_INT_ST_M  (GDMA_OUTFIFO_OVF_CH1_INT_ST_V << GDMA_OUTFIFO_OVF_CH1_INT_ST_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_ST_S  4
/** GDMA_OUTFIFO_UDF_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH1_INT_ST_M  (GDMA_OUTFIFO_UDF_CH1_INT_ST_V << GDMA_OUTFIFO_UDF_CH1_INT_ST_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_ST_S  5

/** GDMA_OUT_INT_ENA_CH1_REG register
 *  Interrupt enable bits of channel 0
 */
#define GDMA_OUT_INT_ENA_CH1_REG (DR_REG_GDMA_BASE + 0x48)
/** GDMA_OUT_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH1_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_ENA_M  (GDMA_OUT_DONE_CH1_INT_ENA_V << GDMA_OUT_DONE_CH1_INT_ENA_S)
#define GDMA_OUT_DONE_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_ENA_S  0
/** GDMA_OUT_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH1_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_ENA_M  (GDMA_OUT_EOF_CH1_INT_ENA_V << GDMA_OUT_EOF_CH1_INT_ENA_S)
#define GDMA_OUT_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_ENA_S  1
/** GDMA_OUT_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_M  (GDMA_OUT_DSCR_ERR_CH1_INT_ENA_V << GDMA_OUT_DSCR_ERR_CH1_INT_ENA_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_ENA_S  2
/** GDMA_OUT_TOTAL_EOF_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_V << GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_ENA_S  3
/** GDMA_OUTFIFO_OVF_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA_M  (GDMA_OUTFIFO_OVF_CH1_INT_ENA_V << GDMA_OUTFIFO_OVF_CH1_INT_ENA_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_ENA_S  4
/** GDMA_OUTFIFO_UDF_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA_M  (GDMA_OUTFIFO_UDF_CH1_INT_ENA_V << GDMA_OUTFIFO_UDF_CH1_INT_ENA_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_ENA_S  5

/** GDMA_OUT_INT_CLR_CH1_REG register
 *  Interrupt clear bits of channel 0
 */
#define GDMA_OUT_INT_CLR_CH1_REG (DR_REG_GDMA_BASE + 0x4c)
/** GDMA_OUT_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH1_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH1_INT_CLR_M  (GDMA_OUT_DONE_CH1_INT_CLR_V << GDMA_OUT_DONE_CH1_INT_CLR_S)
#define GDMA_OUT_DONE_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_DONE_CH1_INT_CLR_S  0
/** GDMA_OUT_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH1_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH1_INT_CLR_M  (GDMA_OUT_EOF_CH1_INT_CLR_V << GDMA_OUT_EOF_CH1_INT_CLR_S)
#define GDMA_OUT_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_EOF_CH1_INT_CLR_S  1
/** GDMA_OUT_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_M  (GDMA_OUT_DSCR_ERR_CH1_INT_CLR_V << GDMA_OUT_DSCR_ERR_CH1_INT_CLR_S)
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH1_INT_CLR_S  2
/** GDMA_OUT_TOTAL_EOF_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_M  (GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_V << GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_S)
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH1_INT_CLR_S  3
/** GDMA_OUTFIFO_OVF_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR_M  (GDMA_OUTFIFO_OVF_CH1_INT_CLR_V << GDMA_OUTFIFO_OVF_CH1_INT_CLR_S)
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH1_INT_CLR_S  4
/** GDMA_OUTFIFO_UDF_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR_M  (GDMA_OUTFIFO_UDF_CH1_INT_CLR_V << GDMA_OUTFIFO_UDF_CH1_INT_CLR_S)
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH1_INT_CLR_S  5

/** GDMA_OUT_INT_RAW_CH2_REG register
 *  Raw status interrupt of channel 0
 */
#define GDMA_OUT_INT_RAW_CH2_REG (DR_REG_GDMA_BASE + 0x50)
/** GDMA_OUT_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define GDMA_OUT_DONE_CH2_INT_RAW    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_RAW_M  (GDMA_OUT_DONE_CH2_INT_RAW_V << GDMA_OUT_DONE_CH2_INT_RAW_S)
#define GDMA_OUT_DONE_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_RAW_S  0
/** GDMA_OUT_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define GDMA_OUT_EOF_CH2_INT_RAW    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_RAW_M  (GDMA_OUT_EOF_CH2_INT_RAW_V << GDMA_OUT_EOF_CH2_INT_RAW_S)
#define GDMA_OUT_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_RAW_S  1
/** GDMA_OUT_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel 0.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_M  (GDMA_OUT_DSCR_ERR_CH2_INT_RAW_V << GDMA_OUT_DSCR_ERR_CH2_INT_RAW_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_RAW_S  2
/** GDMA_OUT_TOTAL_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_V << GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_RAW_S  3
/** GDMA_OUTFIFO_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  overflow.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW_M  (GDMA_OUTFIFO_OVF_CH2_INT_RAW_V << GDMA_OUTFIFO_OVF_CH2_INT_RAW_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_RAW_S  4
/** GDMA_OUTFIFO_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  underflow.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW_M  (GDMA_OUTFIFO_UDF_CH2_INT_RAW_V << GDMA_OUTFIFO_UDF_CH2_INT_RAW_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_RAW_S  5

/** GDMA_OUT_INT_ST_CH2_REG register
 *  Masked interrupt of channel 0
 */
#define GDMA_OUT_INT_ST_CH2_REG (DR_REG_GDMA_BASE + 0x54)
/** GDMA_OUT_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH2_INT_ST    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_ST_M  (GDMA_OUT_DONE_CH2_INT_ST_V << GDMA_OUT_DONE_CH2_INT_ST_S)
#define GDMA_OUT_DONE_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_ST_S  0
/** GDMA_OUT_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH2_INT_ST    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_ST_M  (GDMA_OUT_EOF_CH2_INT_ST_V << GDMA_OUT_EOF_CH2_INT_ST_S)
#define GDMA_OUT_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_ST_S  1
/** GDMA_OUT_DSCR_ERR_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_M  (GDMA_OUT_DSCR_ERR_CH2_INT_ST_V << GDMA_OUT_DSCR_ERR_CH2_INT_ST_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_ST_S  2
/** GDMA_OUT_TOTAL_EOF_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_ST_V << GDMA_OUT_TOTAL_EOF_CH2_INT_ST_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ST_S  3
/** GDMA_OUTFIFO_OVF_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_ST    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH2_INT_ST_M  (GDMA_OUTFIFO_OVF_CH2_INT_ST_V << GDMA_OUTFIFO_OVF_CH2_INT_ST_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_ST_S  4
/** GDMA_OUTFIFO_UDF_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_ST    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH2_INT_ST_M  (GDMA_OUTFIFO_UDF_CH2_INT_ST_V << GDMA_OUTFIFO_UDF_CH2_INT_ST_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_ST_S  5

/** GDMA_OUT_INT_ENA_CH2_REG register
 *  Interrupt enable bits of channel 0
 */
#define GDMA_OUT_INT_ENA_CH2_REG (DR_REG_GDMA_BASE + 0x58)
/** GDMA_OUT_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH2_INT_ENA    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_ENA_M  (GDMA_OUT_DONE_CH2_INT_ENA_V << GDMA_OUT_DONE_CH2_INT_ENA_S)
#define GDMA_OUT_DONE_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_ENA_S  0
/** GDMA_OUT_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH2_INT_ENA    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_ENA_M  (GDMA_OUT_EOF_CH2_INT_ENA_V << GDMA_OUT_EOF_CH2_INT_ENA_S)
#define GDMA_OUT_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_ENA_S  1
/** GDMA_OUT_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_M  (GDMA_OUT_DSCR_ERR_CH2_INT_ENA_V << GDMA_OUT_DSCR_ERR_CH2_INT_ENA_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_ENA_S  2
/** GDMA_OUT_TOTAL_EOF_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_V << GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_ENA_S  3
/** GDMA_OUTFIFO_OVF_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA_M  (GDMA_OUTFIFO_OVF_CH2_INT_ENA_V << GDMA_OUTFIFO_OVF_CH2_INT_ENA_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_ENA_S  4
/** GDMA_OUTFIFO_UDF_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA_M  (GDMA_OUTFIFO_UDF_CH2_INT_ENA_V << GDMA_OUTFIFO_UDF_CH2_INT_ENA_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_ENA_S  5

/** GDMA_OUT_INT_CLR_CH2_REG register
 *  Interrupt clear bits of channel 0
 */
#define GDMA_OUT_INT_CLR_CH2_REG (DR_REG_GDMA_BASE + 0x5c)
/** GDMA_OUT_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define GDMA_OUT_DONE_CH2_INT_CLR    (BIT(0))
#define GDMA_OUT_DONE_CH2_INT_CLR_M  (GDMA_OUT_DONE_CH2_INT_CLR_V << GDMA_OUT_DONE_CH2_INT_CLR_S)
#define GDMA_OUT_DONE_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_DONE_CH2_INT_CLR_S  0
/** GDMA_OUT_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_EOF_CH2_INT_CLR    (BIT(1))
#define GDMA_OUT_EOF_CH2_INT_CLR_M  (GDMA_OUT_EOF_CH2_INT_CLR_V << GDMA_OUT_EOF_CH2_INT_CLR_S)
#define GDMA_OUT_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_EOF_CH2_INT_CLR_S  1
/** GDMA_OUT_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR    (BIT(2))
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_M  (GDMA_OUT_DSCR_ERR_CH2_INT_CLR_V << GDMA_OUT_DSCR_ERR_CH2_INT_CLR_S)
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_DSCR_ERR_CH2_INT_CLR_S  2
/** GDMA_OUT_TOTAL_EOF_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR    (BIT(3))
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_M  (GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_V << GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_S)
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUT_TOTAL_EOF_CH2_INT_CLR_S  3
/** GDMA_OUTFIFO_OVF_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR    (BIT(4))
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR_M  (GDMA_OUTFIFO_OVF_CH2_INT_CLR_V << GDMA_OUTFIFO_OVF_CH2_INT_CLR_S)
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_OVF_CH2_INT_CLR_S  4
/** GDMA_OUTFIFO_UDF_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR    (BIT(5))
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR_M  (GDMA_OUTFIFO_UDF_CH2_INT_CLR_V << GDMA_OUTFIFO_UDF_CH2_INT_CLR_S)
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR_V  0x00000001U
#define GDMA_OUTFIFO_UDF_CH2_INT_CLR_S  5

/** GDMA_AHB_TEST_REG register
 *  reserved
 */
#define GDMA_AHB_TEST_REG (DR_REG_GDMA_BASE + 0x60)
/** GDMA_AHB_TESTMODE : R/W; bitpos: [2:0]; default: 0;
 *  reserved
 */
#define GDMA_AHB_TESTMODE    0x00000007U
#define GDMA_AHB_TESTMODE_M  (GDMA_AHB_TESTMODE_V << GDMA_AHB_TESTMODE_S)
#define GDMA_AHB_TESTMODE_V  0x00000007U
#define GDMA_AHB_TESTMODE_S  0
/** GDMA_AHB_TESTADDR : R/W; bitpos: [5:4]; default: 0;
 *  reserved
 */
#define GDMA_AHB_TESTADDR    0x00000003U
#define GDMA_AHB_TESTADDR_M  (GDMA_AHB_TESTADDR_V << GDMA_AHB_TESTADDR_S)
#define GDMA_AHB_TESTADDR_V  0x00000003U
#define GDMA_AHB_TESTADDR_S  4

/** GDMA_MISC_CONF_REG register
 *  MISC register
 */
#define GDMA_MISC_CONF_REG (DR_REG_GDMA_BASE + 0x64)
/** GDMA_AHBM_RST_INTER : R/W; bitpos: [0]; default: 0;
 *  Set this bit then clear this bit to reset the internal ahb FSM.
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
 *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
 *  registers.
 */
#define GDMA_CLK_EN    (BIT(3))
#define GDMA_CLK_EN_M  (GDMA_CLK_EN_V << GDMA_CLK_EN_S)
#define GDMA_CLK_EN_V  0x00000001U
#define GDMA_CLK_EN_S  3

/** GDMA_DATE_REG register
 *  Version control register
 */
#define GDMA_DATE_REG (DR_REG_GDMA_BASE + 0x68)
/** GDMA_DATE : R/W; bitpos: [31:0]; default: 36720912;
 *  register version.
 */
#define GDMA_DATE    0xFFFFFFFFU
#define GDMA_DATE_M  (GDMA_DATE_V << GDMA_DATE_S)
#define GDMA_DATE_V  0xFFFFFFFFU
#define GDMA_DATE_S  0

/** GDMA_IN_CONF0_CH0_REG register
 *  Configure 0 register of Rx channel 0
 */
#define GDMA_IN_CONF0_CH0_REG (DR_REG_GDMA_BASE + 0x70)
/** GDMA_IN_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset DMA channel 0 Rx FSM and Rx FIFO pointer.
 */
#define GDMA_IN_RST_CH0    (BIT(0))
#define GDMA_IN_RST_CH0_M  (GDMA_IN_RST_CH0_V << GDMA_IN_RST_CH0_S)
#define GDMA_IN_RST_CH0_V  0x00000001U
#define GDMA_IN_RST_CH0_S  0
/** GDMA_IN_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define GDMA_IN_LOOP_TEST_CH0    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH0_M  (GDMA_IN_LOOP_TEST_CH0_V << GDMA_IN_LOOP_TEST_CH0_S)
#define GDMA_IN_LOOP_TEST_CH0_V  0x00000001U
#define GDMA_IN_LOOP_TEST_CH0_S  1
/** GDMA_INDSCR_BURST_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define GDMA_INDSCR_BURST_EN_CH0    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH0_M  (GDMA_INDSCR_BURST_EN_CH0_V << GDMA_INDSCR_BURST_EN_CH0_S)
#define GDMA_INDSCR_BURST_EN_CH0_V  0x00000001U
#define GDMA_INDSCR_BURST_EN_CH0_S  2
/** GDMA_IN_DATA_BURST_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 receiving data
 *  when accessing internal SRAM.
 */
#define GDMA_IN_DATA_BURST_EN_CH0    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH0_M  (GDMA_IN_DATA_BURST_EN_CH0_V << GDMA_IN_DATA_BURST_EN_CH0_S)
#define GDMA_IN_DATA_BURST_EN_CH0_V  0x00000001U
#define GDMA_IN_DATA_BURST_EN_CH0_S  3
/** GDMA_MEM_TRANS_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.
 */
#define GDMA_MEM_TRANS_EN_CH0    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH0_M  (GDMA_MEM_TRANS_EN_CH0_V << GDMA_MEM_TRANS_EN_CH0_S)
#define GDMA_MEM_TRANS_EN_CH0_V  0x00000001U
#define GDMA_MEM_TRANS_EN_CH0_S  4
/** GDMA_IN_ETM_EN_CH0 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 0 is triggered by etm
 *  task.
 */
#define GDMA_IN_ETM_EN_CH0    (BIT(5))
#define GDMA_IN_ETM_EN_CH0_M  (GDMA_IN_ETM_EN_CH0_V << GDMA_IN_ETM_EN_CH0_S)
#define GDMA_IN_ETM_EN_CH0_V  0x00000001U
#define GDMA_IN_ETM_EN_CH0_S  5

/** GDMA_IN_CONF1_CH0_REG register
 *  Configure 1 register of Rx channel 0
 */
#define GDMA_IN_CONF1_CH0_REG (DR_REG_GDMA_BASE + 0x74)
/** GDMA_IN_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define GDMA_IN_CHECK_OWNER_CH0    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH0_M  (GDMA_IN_CHECK_OWNER_CH0_V << GDMA_IN_CHECK_OWNER_CH0_S)
#define GDMA_IN_CHECK_OWNER_CH0_V  0x00000001U
#define GDMA_IN_CHECK_OWNER_CH0_S  12

/** GDMA_INFIFO_STATUS_CH0_REG register
 *  Receive FIFO status of Rx channel 0
 */
#define GDMA_INFIFO_STATUS_CH0_REG (DR_REG_GDMA_BASE + 0x78)
/** GDMA_INFIFO_FULL_CH0 : RO; bitpos: [0]; default: 1;
 *  L1 Rx FIFO full signal for Rx channel 0.
 */
#define GDMA_INFIFO_FULL_CH0    (BIT(0))
#define GDMA_INFIFO_FULL_CH0_M  (GDMA_INFIFO_FULL_CH0_V << GDMA_INFIFO_FULL_CH0_S)
#define GDMA_INFIFO_FULL_CH0_V  0x00000001U
#define GDMA_INFIFO_FULL_CH0_S  0
/** GDMA_INFIFO_EMPTY_CH0 : RO; bitpos: [1]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 0.
 */
#define GDMA_INFIFO_EMPTY_CH0    (BIT(1))
#define GDMA_INFIFO_EMPTY_CH0_M  (GDMA_INFIFO_EMPTY_CH0_V << GDMA_INFIFO_EMPTY_CH0_S)
#define GDMA_INFIFO_EMPTY_CH0_V  0x00000001U
#define GDMA_INFIFO_EMPTY_CH0_S  1
/** GDMA_INFIFO_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.
 */
#define GDMA_INFIFO_CNT_CH0    0x0000003FU
#define GDMA_INFIFO_CNT_CH0_M  (GDMA_INFIFO_CNT_CH0_V << GDMA_INFIFO_CNT_CH0_S)
#define GDMA_INFIFO_CNT_CH0_V  0x0000003FU
#define GDMA_INFIFO_CNT_CH0_S  2
/** GDMA_IN_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_1B_CH0    (BIT(23))
#define GDMA_IN_REMAIN_UNDER_1B_CH0_M  (GDMA_IN_REMAIN_UNDER_1B_CH0_V << GDMA_IN_REMAIN_UNDER_1B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_1B_CH0_S  23
/** GDMA_IN_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_2B_CH0    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_2B_CH0_M  (GDMA_IN_REMAIN_UNDER_2B_CH0_V << GDMA_IN_REMAIN_UNDER_2B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_2B_CH0_S  24
/** GDMA_IN_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_3B_CH0    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_3B_CH0_M  (GDMA_IN_REMAIN_UNDER_3B_CH0_V << GDMA_IN_REMAIN_UNDER_3B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_3B_CH0_S  25
/** GDMA_IN_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_4B_CH0    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_4B_CH0_M  (GDMA_IN_REMAIN_UNDER_4B_CH0_V << GDMA_IN_REMAIN_UNDER_4B_CH0_S)
#define GDMA_IN_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_4B_CH0_S  26
/** GDMA_IN_BUF_HUNGRY_CH0 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define GDMA_IN_BUF_HUNGRY_CH0    (BIT(27))
#define GDMA_IN_BUF_HUNGRY_CH0_M  (GDMA_IN_BUF_HUNGRY_CH0_V << GDMA_IN_BUF_HUNGRY_CH0_S)
#define GDMA_IN_BUF_HUNGRY_CH0_V  0x00000001U
#define GDMA_IN_BUF_HUNGRY_CH0_S  27

/** GDMA_IN_POP_CH0_REG register
 *  Pop control register of Rx channel 0
 */
#define GDMA_IN_POP_CH0_REG (DR_REG_GDMA_BASE + 0x7c)
/** GDMA_INFIFO_RDATA_CH0 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from DMA FIFO.
 */
#define GDMA_INFIFO_RDATA_CH0    0x00000FFFU
#define GDMA_INFIFO_RDATA_CH0_M  (GDMA_INFIFO_RDATA_CH0_V << GDMA_INFIFO_RDATA_CH0_S)
#define GDMA_INFIFO_RDATA_CH0_V  0x00000FFFU
#define GDMA_INFIFO_RDATA_CH0_S  0
/** GDMA_INFIFO_POP_CH0 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from DMA FIFO.
 */
#define GDMA_INFIFO_POP_CH0    (BIT(12))
#define GDMA_INFIFO_POP_CH0_M  (GDMA_INFIFO_POP_CH0_V << GDMA_INFIFO_POP_CH0_S)
#define GDMA_INFIFO_POP_CH0_V  0x00000001U
#define GDMA_INFIFO_POP_CH0_S  12

/** GDMA_IN_LINK_CH0_REG register
 *  Link descriptor configure and control register of Rx channel 0
 */
#define GDMA_IN_LINK_CH0_REG (DR_REG_GDMA_BASE + 0x80)
/** GDMA_INLINK_ADDR_CH0 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first inlink descriptor's
 *  address.
 */
#define GDMA_INLINK_ADDR_CH0    0x000FFFFFU
#define GDMA_INLINK_ADDR_CH0_M  (GDMA_INLINK_ADDR_CH0_V << GDMA_INLINK_ADDR_CH0_S)
#define GDMA_INLINK_ADDR_CH0_V  0x000FFFFFU
#define GDMA_INLINK_ADDR_CH0_S  0
/** GDMA_INLINK_AUTO_RET_CH0 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define GDMA_INLINK_AUTO_RET_CH0    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH0_M  (GDMA_INLINK_AUTO_RET_CH0_V << GDMA_INLINK_AUTO_RET_CH0_S)
#define GDMA_INLINK_AUTO_RET_CH0_V  0x00000001U
#define GDMA_INLINK_AUTO_RET_CH0_S  20
/** GDMA_INLINK_STOP_CH0 : WT; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define GDMA_INLINK_STOP_CH0    (BIT(21))
#define GDMA_INLINK_STOP_CH0_M  (GDMA_INLINK_STOP_CH0_V << GDMA_INLINK_STOP_CH0_S)
#define GDMA_INLINK_STOP_CH0_V  0x00000001U
#define GDMA_INLINK_STOP_CH0_S  21
/** GDMA_INLINK_START_CH0 : WT; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define GDMA_INLINK_START_CH0    (BIT(22))
#define GDMA_INLINK_START_CH0_M  (GDMA_INLINK_START_CH0_V << GDMA_INLINK_START_CH0_S)
#define GDMA_INLINK_START_CH0_V  0x00000001U
#define GDMA_INLINK_START_CH0_S  22
/** GDMA_INLINK_RESTART_CH0 : WT; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define GDMA_INLINK_RESTART_CH0    (BIT(23))
#define GDMA_INLINK_RESTART_CH0_M  (GDMA_INLINK_RESTART_CH0_V << GDMA_INLINK_RESTART_CH0_S)
#define GDMA_INLINK_RESTART_CH0_V  0x00000001U
#define GDMA_INLINK_RESTART_CH0_S  23
/** GDMA_INLINK_PARK_CH0 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define GDMA_INLINK_PARK_CH0    (BIT(24))
#define GDMA_INLINK_PARK_CH0_M  (GDMA_INLINK_PARK_CH0_V << GDMA_INLINK_PARK_CH0_S)
#define GDMA_INLINK_PARK_CH0_V  0x00000001U
#define GDMA_INLINK_PARK_CH0_S  24

/** GDMA_IN_STATE_CH0_REG register
 *  Receive status of Rx channel 0
 */
#define GDMA_IN_STATE_CH0_REG (DR_REG_GDMA_BASE + 0x84)
/** GDMA_INLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define GDMA_INLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH0_M  (GDMA_INLINK_DSCR_ADDR_CH0_V << GDMA_INLINK_DSCR_ADDR_CH0_S)
#define GDMA_INLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH0_S  0
/** GDMA_IN_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define GDMA_IN_DSCR_STATE_CH0    0x00000003U
#define GDMA_IN_DSCR_STATE_CH0_M  (GDMA_IN_DSCR_STATE_CH0_V << GDMA_IN_DSCR_STATE_CH0_S)
#define GDMA_IN_DSCR_STATE_CH0_V  0x00000003U
#define GDMA_IN_DSCR_STATE_CH0_S  18
/** GDMA_IN_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define GDMA_IN_STATE_CH0    0x00000007U
#define GDMA_IN_STATE_CH0_M  (GDMA_IN_STATE_CH0_V << GDMA_IN_STATE_CH0_S)
#define GDMA_IN_STATE_CH0_V  0x00000007U
#define GDMA_IN_STATE_CH0_S  20

/** GDMA_IN_SUC_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 0
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0x88)
/** GDMA_IN_SUC_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_M  (GDMA_IN_SUC_EOF_DES_ADDR_CH0_V << GDMA_IN_SUC_EOF_DES_ADDR_CH0_S)
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH0_S  0

/** GDMA_IN_ERR_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when errors occur of Rx channel 0
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0x8c)
/** GDMA_IN_ERR_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_M  (GDMA_IN_ERR_EOF_DES_ADDR_CH0_V << GDMA_IN_ERR_EOF_DES_ADDR_CH0_S)
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH0_S  0

/** GDMA_IN_DSCR_CH0_REG register
 *  Current inlink descriptor address of Rx channel 0
 */
#define GDMA_IN_DSCR_CH0_REG (DR_REG_GDMA_BASE + 0x90)
/** GDMA_INLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define GDMA_INLINK_DSCR_CH0    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH0_M  (GDMA_INLINK_DSCR_CH0_V << GDMA_INLINK_DSCR_CH0_S)
#define GDMA_INLINK_DSCR_CH0_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH0_S  0

/** GDMA_IN_DSCR_BF0_CH0_REG register
 *  The last inlink descriptor address of Rx channel 0
 */
#define GDMA_IN_DSCR_BF0_CH0_REG (DR_REG_GDMA_BASE + 0x94)
/** GDMA_INLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define GDMA_INLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH0_M  (GDMA_INLINK_DSCR_BF0_CH0_V << GDMA_INLINK_DSCR_BF0_CH0_S)
#define GDMA_INLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH0_S  0

/** GDMA_IN_DSCR_BF1_CH0_REG register
 *  The second-to-last inlink descriptor address of Rx channel 0
 */
#define GDMA_IN_DSCR_BF1_CH0_REG (DR_REG_GDMA_BASE + 0x98)
/** GDMA_INLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define GDMA_INLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH0_M  (GDMA_INLINK_DSCR_BF1_CH0_V << GDMA_INLINK_DSCR_BF1_CH0_S)
#define GDMA_INLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH0_S  0

/** GDMA_IN_PRI_CH0_REG register
 *  Priority register of Rx channel 0
 */
#define GDMA_IN_PRI_CH0_REG (DR_REG_GDMA_BASE + 0x9c)
/** GDMA_RX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 0. The larger of the value the higher of the priority.
 */
#define GDMA_RX_PRI_CH0    0x0000000FU
#define GDMA_RX_PRI_CH0_M  (GDMA_RX_PRI_CH0_V << GDMA_RX_PRI_CH0_S)
#define GDMA_RX_PRI_CH0_V  0x0000000FU
#define GDMA_RX_PRI_CH0_S  0

/** GDMA_IN_PERI_SEL_CH0_REG register
 *  Peripheral selection of Rx channel 0
 */
#define GDMA_IN_PERI_SEL_CH0_REG (DR_REG_GDMA_BASE + 0xa0)
/** GDMA_PERI_IN_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 0. 0:SPI2. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: Dummy. 5: Dummy. 6: AES. 7: SHA. 8: ADC_DAC. 9: Parallel_IO.
 *  10~15: Dummy
 */
#define GDMA_PERI_IN_SEL_CH0    0x0000003FU
#define GDMA_PERI_IN_SEL_CH0_M  (GDMA_PERI_IN_SEL_CH0_V << GDMA_PERI_IN_SEL_CH0_S)
#define GDMA_PERI_IN_SEL_CH0_V  0x0000003FU
#define GDMA_PERI_IN_SEL_CH0_S  0

/** GDMA_OUT_CONF0_CH0_REG register
 *  Configure 0 register of Tx channel 0
 */
#define GDMA_OUT_CONF0_CH0_REG (DR_REG_GDMA_BASE + 0xd0)
/** GDMA_OUT_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset DMA channel 0 Tx FSM and Tx FIFO pointer.
 */
#define GDMA_OUT_RST_CH0    (BIT(0))
#define GDMA_OUT_RST_CH0_M  (GDMA_OUT_RST_CH0_V << GDMA_OUT_RST_CH0_S)
#define GDMA_OUT_RST_CH0_V  0x00000001U
#define GDMA_OUT_RST_CH0_S  0
/** GDMA_OUT_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define GDMA_OUT_LOOP_TEST_CH0    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH0_M  (GDMA_OUT_LOOP_TEST_CH0_V << GDMA_OUT_LOOP_TEST_CH0_S)
#define GDMA_OUT_LOOP_TEST_CH0_V  0x00000001U
#define GDMA_OUT_LOOP_TEST_CH0_S  1
/** GDMA_OUT_AUTO_WRBACK_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define GDMA_OUT_AUTO_WRBACK_CH0    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH0_M  (GDMA_OUT_AUTO_WRBACK_CH0_V << GDMA_OUT_AUTO_WRBACK_CH0_S)
#define GDMA_OUT_AUTO_WRBACK_CH0_V  0x00000001U
#define GDMA_OUT_AUTO_WRBACK_CH0_S  2
/** GDMA_OUT_EOF_MODE_CH0 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to transmit has been popped from FIFO in DMA
 */
#define GDMA_OUT_EOF_MODE_CH0    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH0_M  (GDMA_OUT_EOF_MODE_CH0_V << GDMA_OUT_EOF_MODE_CH0_S)
#define GDMA_OUT_EOF_MODE_CH0_V  0x00000001U
#define GDMA_OUT_EOF_MODE_CH0_S  3
/** GDMA_OUTDSCR_BURST_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define GDMA_OUTDSCR_BURST_EN_CH0    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH0_M  (GDMA_OUTDSCR_BURST_EN_CH0_V << GDMA_OUTDSCR_BURST_EN_CH0_S)
#define GDMA_OUTDSCR_BURST_EN_CH0_V  0x00000001U
#define GDMA_OUTDSCR_BURST_EN_CH0_S  4
/** GDMA_OUT_DATA_BURST_EN_CH0 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 transmitting data
 *  when accessing internal SRAM.
 */
#define GDMA_OUT_DATA_BURST_EN_CH0    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH0_M  (GDMA_OUT_DATA_BURST_EN_CH0_V << GDMA_OUT_DATA_BURST_EN_CH0_S)
#define GDMA_OUT_DATA_BURST_EN_CH0_V  0x00000001U
#define GDMA_OUT_DATA_BURST_EN_CH0_S  5
/** GDMA_OUT_ETM_EN_CH0 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel 0 is triggered by etm
 *  task.
 */
#define GDMA_OUT_ETM_EN_CH0    (BIT(6))
#define GDMA_OUT_ETM_EN_CH0_M  (GDMA_OUT_ETM_EN_CH0_V << GDMA_OUT_ETM_EN_CH0_S)
#define GDMA_OUT_ETM_EN_CH0_V  0x00000001U
#define GDMA_OUT_ETM_EN_CH0_S  6

/** GDMA_OUT_CONF1_CH0_REG register
 *  Configure 1 register of Tx channel 0
 */
#define GDMA_OUT_CONF1_CH0_REG (DR_REG_GDMA_BASE + 0xd4)
/** GDMA_OUT_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define GDMA_OUT_CHECK_OWNER_CH0    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH0_M  (GDMA_OUT_CHECK_OWNER_CH0_V << GDMA_OUT_CHECK_OWNER_CH0_S)
#define GDMA_OUT_CHECK_OWNER_CH0_V  0x00000001U
#define GDMA_OUT_CHECK_OWNER_CH0_S  12

/** GDMA_OUTFIFO_STATUS_CH0_REG register
 *  Transmit FIFO status of Tx channel 0
 */
#define GDMA_OUTFIFO_STATUS_CH0_REG (DR_REG_GDMA_BASE + 0xd8)
/** GDMA_OUTFIFO_FULL_CH0 : RO; bitpos: [0]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel 0.
 */
#define GDMA_OUTFIFO_FULL_CH0    (BIT(0))
#define GDMA_OUTFIFO_FULL_CH0_M  (GDMA_OUTFIFO_FULL_CH0_V << GDMA_OUTFIFO_FULL_CH0_S)
#define GDMA_OUTFIFO_FULL_CH0_V  0x00000001U
#define GDMA_OUTFIFO_FULL_CH0_S  0
/** GDMA_OUTFIFO_EMPTY_CH0 : RO; bitpos: [1]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel 0.
 */
#define GDMA_OUTFIFO_EMPTY_CH0    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_CH0_M  (GDMA_OUTFIFO_EMPTY_CH0_V << GDMA_OUTFIFO_EMPTY_CH0_S)
#define GDMA_OUTFIFO_EMPTY_CH0_V  0x00000001U
#define GDMA_OUTFIFO_EMPTY_CH0_S  1
/** GDMA_OUTFIFO_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.
 */
#define GDMA_OUTFIFO_CNT_CH0    0x0000003FU
#define GDMA_OUTFIFO_CNT_CH0_M  (GDMA_OUTFIFO_CNT_CH0_V << GDMA_OUTFIFO_CNT_CH0_S)
#define GDMA_OUTFIFO_CNT_CH0_V  0x0000003FU
#define GDMA_OUTFIFO_CNT_CH0_S  2
/** GDMA_OUT_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_1B_CH0    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_CH0_M  (GDMA_OUT_REMAIN_UNDER_1B_CH0_V << GDMA_OUT_REMAIN_UNDER_1B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_1B_CH0_S  23
/** GDMA_OUT_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_2B_CH0    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_CH0_M  (GDMA_OUT_REMAIN_UNDER_2B_CH0_V << GDMA_OUT_REMAIN_UNDER_2B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_2B_CH0_S  24
/** GDMA_OUT_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_3B_CH0    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_CH0_M  (GDMA_OUT_REMAIN_UNDER_3B_CH0_V << GDMA_OUT_REMAIN_UNDER_3B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_3B_CH0_S  25
/** GDMA_OUT_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_4B_CH0    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_CH0_M  (GDMA_OUT_REMAIN_UNDER_4B_CH0_V << GDMA_OUT_REMAIN_UNDER_4B_CH0_S)
#define GDMA_OUT_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_4B_CH0_S  26

/** GDMA_OUT_PUSH_CH0_REG register
 *  Push control register of Rx channel 0
 */
#define GDMA_OUT_PUSH_CH0_REG (DR_REG_GDMA_BASE + 0xdc)
/** GDMA_OUTFIFO_WDATA_CH0 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA FIFO.
 */
#define GDMA_OUTFIFO_WDATA_CH0    0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH0_M  (GDMA_OUTFIFO_WDATA_CH0_V << GDMA_OUTFIFO_WDATA_CH0_S)
#define GDMA_OUTFIFO_WDATA_CH0_V  0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH0_S  0
/** GDMA_OUTFIFO_PUSH_CH0 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into DMA FIFO.
 */
#define GDMA_OUTFIFO_PUSH_CH0    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH0_M  (GDMA_OUTFIFO_PUSH_CH0_V << GDMA_OUTFIFO_PUSH_CH0_S)
#define GDMA_OUTFIFO_PUSH_CH0_V  0x00000001U
#define GDMA_OUTFIFO_PUSH_CH0_S  9

/** GDMA_OUT_LINK_CH0_REG register
 *  Link descriptor configure and control register of Tx channel 0
 */
#define GDMA_OUT_LINK_CH0_REG (DR_REG_GDMA_BASE + 0xe0)
/** GDMA_OUTLINK_ADDR_CH0 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first outlink
 *  descriptor's address.
 */
#define GDMA_OUTLINK_ADDR_CH0    0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH0_M  (GDMA_OUTLINK_ADDR_CH0_V << GDMA_OUTLINK_ADDR_CH0_S)
#define GDMA_OUTLINK_ADDR_CH0_V  0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH0_S  0
/** GDMA_OUTLINK_STOP_CH0 : WT; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define GDMA_OUTLINK_STOP_CH0    (BIT(20))
#define GDMA_OUTLINK_STOP_CH0_M  (GDMA_OUTLINK_STOP_CH0_V << GDMA_OUTLINK_STOP_CH0_S)
#define GDMA_OUTLINK_STOP_CH0_V  0x00000001U
#define GDMA_OUTLINK_STOP_CH0_S  20
/** GDMA_OUTLINK_START_CH0 : WT; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define GDMA_OUTLINK_START_CH0    (BIT(21))
#define GDMA_OUTLINK_START_CH0_M  (GDMA_OUTLINK_START_CH0_V << GDMA_OUTLINK_START_CH0_S)
#define GDMA_OUTLINK_START_CH0_V  0x00000001U
#define GDMA_OUTLINK_START_CH0_S  21
/** GDMA_OUTLINK_RESTART_CH0 : WT; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define GDMA_OUTLINK_RESTART_CH0    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH0_M  (GDMA_OUTLINK_RESTART_CH0_V << GDMA_OUTLINK_RESTART_CH0_S)
#define GDMA_OUTLINK_RESTART_CH0_V  0x00000001U
#define GDMA_OUTLINK_RESTART_CH0_S  22
/** GDMA_OUTLINK_PARK_CH0 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define GDMA_OUTLINK_PARK_CH0    (BIT(23))
#define GDMA_OUTLINK_PARK_CH0_M  (GDMA_OUTLINK_PARK_CH0_V << GDMA_OUTLINK_PARK_CH0_S)
#define GDMA_OUTLINK_PARK_CH0_V  0x00000001U
#define GDMA_OUTLINK_PARK_CH0_S  23

/** GDMA_OUT_STATE_CH0_REG register
 *  Transmit status of Tx channel 0
 */
#define GDMA_OUT_STATE_CH0_REG (DR_REG_GDMA_BASE + 0xe4)
/** GDMA_OUTLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define GDMA_OUTLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH0_M  (GDMA_OUTLINK_DSCR_ADDR_CH0_V << GDMA_OUTLINK_DSCR_ADDR_CH0_S)
#define GDMA_OUTLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH0_S  0
/** GDMA_OUT_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define GDMA_OUT_DSCR_STATE_CH0    0x00000003U
#define GDMA_OUT_DSCR_STATE_CH0_M  (GDMA_OUT_DSCR_STATE_CH0_V << GDMA_OUT_DSCR_STATE_CH0_S)
#define GDMA_OUT_DSCR_STATE_CH0_V  0x00000003U
#define GDMA_OUT_DSCR_STATE_CH0_S  18
/** GDMA_OUT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define GDMA_OUT_STATE_CH0    0x00000007U
#define GDMA_OUT_STATE_CH0_M  (GDMA_OUT_STATE_CH0_V << GDMA_OUT_STATE_CH0_S)
#define GDMA_OUT_STATE_CH0_V  0x00000007U
#define GDMA_OUT_STATE_CH0_S  20

/** GDMA_OUT_EOF_DES_ADDR_CH0_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel 0
 */
#define GDMA_OUT_EOF_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0xe8)
/** GDMA_OUT_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_OUT_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH0_M  (GDMA_OUT_EOF_DES_ADDR_CH0_V << GDMA_OUT_EOF_DES_ADDR_CH0_S)
#define GDMA_OUT_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH0_S  0

/** GDMA_OUT_EOF_BFR_DES_ADDR_CH0_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel 0
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_REG (DR_REG_GDMA_BASE + 0xec)
/** GDMA_OUT_EOF_BFR_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0    0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_M  (GDMA_OUT_EOF_BFR_DES_ADDR_CH0_V << GDMA_OUT_EOF_BFR_DES_ADDR_CH0_S)
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH0_S  0

/** GDMA_OUT_DSCR_CH0_REG register
 *  Current inlink descriptor address of Tx channel 0
 */
#define GDMA_OUT_DSCR_CH0_REG (DR_REG_GDMA_BASE + 0xf0)
/** GDMA_OUTLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define GDMA_OUTLINK_DSCR_CH0    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH0_M  (GDMA_OUTLINK_DSCR_CH0_V << GDMA_OUTLINK_DSCR_CH0_S)
#define GDMA_OUTLINK_DSCR_CH0_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH0_S  0

/** GDMA_OUT_DSCR_BF0_CH0_REG register
 *  The last inlink descriptor address of Tx channel 0
 */
#define GDMA_OUT_DSCR_BF0_CH0_REG (DR_REG_GDMA_BASE + 0xf4)
/** GDMA_OUTLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define GDMA_OUTLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH0_M  (GDMA_OUTLINK_DSCR_BF0_CH0_V << GDMA_OUTLINK_DSCR_BF0_CH0_S)
#define GDMA_OUTLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH0_S  0

/** GDMA_OUT_DSCR_BF1_CH0_REG register
 *  The second-to-last inlink descriptor address of Tx channel 0
 */
#define GDMA_OUT_DSCR_BF1_CH0_REG (DR_REG_GDMA_BASE + 0xf8)
/** GDMA_OUTLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define GDMA_OUTLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH0_M  (GDMA_OUTLINK_DSCR_BF1_CH0_V << GDMA_OUTLINK_DSCR_BF1_CH0_S)
#define GDMA_OUTLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH0_S  0

/** GDMA_OUT_PRI_CH0_REG register
 *  Priority register of Tx channel 0.
 */
#define GDMA_OUT_PRI_CH0_REG (DR_REG_GDMA_BASE + 0xfc)
/** GDMA_TX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel 0. The larger of the value the higher of the priority.
 */
#define GDMA_TX_PRI_CH0    0x0000000FU
#define GDMA_TX_PRI_CH0_M  (GDMA_TX_PRI_CH0_V << GDMA_TX_PRI_CH0_S)
#define GDMA_TX_PRI_CH0_V  0x0000000FU
#define GDMA_TX_PRI_CH0_S  0

/** GDMA_OUT_PERI_SEL_CH0_REG register
 *  Peripheral selection of Tx channel 0
 */
#define GDMA_OUT_PERI_SEL_CH0_REG (DR_REG_GDMA_BASE + 0x100)
/** GDMA_PERI_OUT_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel 0. 0:SPI2. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: Dummy. 5: Dummy. 6: AES. 7: SHA. 8: ADC_DAC. 9: Parallel_IO.
 *  10~15: Dummy
 */
#define GDMA_PERI_OUT_SEL_CH0    0x0000003FU
#define GDMA_PERI_OUT_SEL_CH0_M  (GDMA_PERI_OUT_SEL_CH0_V << GDMA_PERI_OUT_SEL_CH0_S)
#define GDMA_PERI_OUT_SEL_CH0_V  0x0000003FU
#define GDMA_PERI_OUT_SEL_CH0_S  0

/** GDMA_IN_CONF0_CH1_REG register
 *  Configure 0 register of Rx channel 1
 */
#define GDMA_IN_CONF0_CH1_REG (DR_REG_GDMA_BASE + 0x130)
/** GDMA_IN_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset DMA channel 1 Rx FSM and Rx FIFO pointer.
 */
#define GDMA_IN_RST_CH1    (BIT(0))
#define GDMA_IN_RST_CH1_M  (GDMA_IN_RST_CH1_V << GDMA_IN_RST_CH1_S)
#define GDMA_IN_RST_CH1_V  0x00000001U
#define GDMA_IN_RST_CH1_S  0
/** GDMA_IN_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define GDMA_IN_LOOP_TEST_CH1    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH1_M  (GDMA_IN_LOOP_TEST_CH1_V << GDMA_IN_LOOP_TEST_CH1_S)
#define GDMA_IN_LOOP_TEST_CH1_V  0x00000001U
#define GDMA_IN_LOOP_TEST_CH1_S  1
/** GDMA_INDSCR_BURST_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 1 reading link
 *  descriptor when accessing internal SRAM.
 */
#define GDMA_INDSCR_BURST_EN_CH1    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH1_M  (GDMA_INDSCR_BURST_EN_CH1_V << GDMA_INDSCR_BURST_EN_CH1_S)
#define GDMA_INDSCR_BURST_EN_CH1_V  0x00000001U
#define GDMA_INDSCR_BURST_EN_CH1_S  2
/** GDMA_IN_DATA_BURST_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 1 receiving data
 *  when accessing internal SRAM.
 */
#define GDMA_IN_DATA_BURST_EN_CH1    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH1_M  (GDMA_IN_DATA_BURST_EN_CH1_V << GDMA_IN_DATA_BURST_EN_CH1_S)
#define GDMA_IN_DATA_BURST_EN_CH1_V  0x00000001U
#define GDMA_IN_DATA_BURST_EN_CH1_S  3
/** GDMA_MEM_TRANS_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.
 */
#define GDMA_MEM_TRANS_EN_CH1    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH1_M  (GDMA_MEM_TRANS_EN_CH1_V << GDMA_MEM_TRANS_EN_CH1_S)
#define GDMA_MEM_TRANS_EN_CH1_V  0x00000001U
#define GDMA_MEM_TRANS_EN_CH1_S  4
/** GDMA_IN_ETM_EN_CH1 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 1 is triggered by etm
 *  task.
 */
#define GDMA_IN_ETM_EN_CH1    (BIT(5))
#define GDMA_IN_ETM_EN_CH1_M  (GDMA_IN_ETM_EN_CH1_V << GDMA_IN_ETM_EN_CH1_S)
#define GDMA_IN_ETM_EN_CH1_V  0x00000001U
#define GDMA_IN_ETM_EN_CH1_S  5

/** GDMA_IN_CONF1_CH1_REG register
 *  Configure 1 register of Rx channel 1
 */
#define GDMA_IN_CONF1_CH1_REG (DR_REG_GDMA_BASE + 0x134)
/** GDMA_IN_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define GDMA_IN_CHECK_OWNER_CH1    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH1_M  (GDMA_IN_CHECK_OWNER_CH1_V << GDMA_IN_CHECK_OWNER_CH1_S)
#define GDMA_IN_CHECK_OWNER_CH1_V  0x00000001U
#define GDMA_IN_CHECK_OWNER_CH1_S  12

/** GDMA_INFIFO_STATUS_CH1_REG register
 *  Receive FIFO status of Rx channel 1
 */
#define GDMA_INFIFO_STATUS_CH1_REG (DR_REG_GDMA_BASE + 0x138)
/** GDMA_INFIFO_FULL_CH1 : RO; bitpos: [0]; default: 1;
 *  L1 Rx FIFO full signal for Rx channel 1.
 */
#define GDMA_INFIFO_FULL_CH1    (BIT(0))
#define GDMA_INFIFO_FULL_CH1_M  (GDMA_INFIFO_FULL_CH1_V << GDMA_INFIFO_FULL_CH1_S)
#define GDMA_INFIFO_FULL_CH1_V  0x00000001U
#define GDMA_INFIFO_FULL_CH1_S  0
/** GDMA_INFIFO_EMPTY_CH1 : RO; bitpos: [1]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 1.
 */
#define GDMA_INFIFO_EMPTY_CH1    (BIT(1))
#define GDMA_INFIFO_EMPTY_CH1_M  (GDMA_INFIFO_EMPTY_CH1_V << GDMA_INFIFO_EMPTY_CH1_S)
#define GDMA_INFIFO_EMPTY_CH1_V  0x00000001U
#define GDMA_INFIFO_EMPTY_CH1_S  1
/** GDMA_INFIFO_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 1.
 */
#define GDMA_INFIFO_CNT_CH1    0x0000003FU
#define GDMA_INFIFO_CNT_CH1_M  (GDMA_INFIFO_CNT_CH1_V << GDMA_INFIFO_CNT_CH1_S)
#define GDMA_INFIFO_CNT_CH1_V  0x0000003FU
#define GDMA_INFIFO_CNT_CH1_S  2
/** GDMA_IN_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_1B_CH1    (BIT(23))
#define GDMA_IN_REMAIN_UNDER_1B_CH1_M  (GDMA_IN_REMAIN_UNDER_1B_CH1_V << GDMA_IN_REMAIN_UNDER_1B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_1B_CH1_S  23
/** GDMA_IN_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_2B_CH1    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_2B_CH1_M  (GDMA_IN_REMAIN_UNDER_2B_CH1_V << GDMA_IN_REMAIN_UNDER_2B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_2B_CH1_S  24
/** GDMA_IN_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_3B_CH1    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_3B_CH1_M  (GDMA_IN_REMAIN_UNDER_3B_CH1_V << GDMA_IN_REMAIN_UNDER_3B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_3B_CH1_S  25
/** GDMA_IN_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_4B_CH1    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_4B_CH1_M  (GDMA_IN_REMAIN_UNDER_4B_CH1_V << GDMA_IN_REMAIN_UNDER_4B_CH1_S)
#define GDMA_IN_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_4B_CH1_S  26
/** GDMA_IN_BUF_HUNGRY_CH1 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define GDMA_IN_BUF_HUNGRY_CH1    (BIT(27))
#define GDMA_IN_BUF_HUNGRY_CH1_M  (GDMA_IN_BUF_HUNGRY_CH1_V << GDMA_IN_BUF_HUNGRY_CH1_S)
#define GDMA_IN_BUF_HUNGRY_CH1_V  0x00000001U
#define GDMA_IN_BUF_HUNGRY_CH1_S  27

/** GDMA_IN_POP_CH1_REG register
 *  Pop control register of Rx channel 1
 */
#define GDMA_IN_POP_CH1_REG (DR_REG_GDMA_BASE + 0x13c)
/** GDMA_INFIFO_RDATA_CH1 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from DMA FIFO.
 */
#define GDMA_INFIFO_RDATA_CH1    0x00000FFFU
#define GDMA_INFIFO_RDATA_CH1_M  (GDMA_INFIFO_RDATA_CH1_V << GDMA_INFIFO_RDATA_CH1_S)
#define GDMA_INFIFO_RDATA_CH1_V  0x00000FFFU
#define GDMA_INFIFO_RDATA_CH1_S  0
/** GDMA_INFIFO_POP_CH1 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from DMA FIFO.
 */
#define GDMA_INFIFO_POP_CH1    (BIT(12))
#define GDMA_INFIFO_POP_CH1_M  (GDMA_INFIFO_POP_CH1_V << GDMA_INFIFO_POP_CH1_S)
#define GDMA_INFIFO_POP_CH1_V  0x00000001U
#define GDMA_INFIFO_POP_CH1_S  12

/** GDMA_IN_LINK_CH1_REG register
 *  Link descriptor configure and control register of Rx channel 1
 */
#define GDMA_IN_LINK_CH1_REG (DR_REG_GDMA_BASE + 0x140)
/** GDMA_INLINK_ADDR_CH1 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first inlink descriptor's
 *  address.
 */
#define GDMA_INLINK_ADDR_CH1    0x000FFFFFU
#define GDMA_INLINK_ADDR_CH1_M  (GDMA_INLINK_ADDR_CH1_V << GDMA_INLINK_ADDR_CH1_S)
#define GDMA_INLINK_ADDR_CH1_V  0x000FFFFFU
#define GDMA_INLINK_ADDR_CH1_S  0
/** GDMA_INLINK_AUTO_RET_CH1 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define GDMA_INLINK_AUTO_RET_CH1    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH1_M  (GDMA_INLINK_AUTO_RET_CH1_V << GDMA_INLINK_AUTO_RET_CH1_S)
#define GDMA_INLINK_AUTO_RET_CH1_V  0x00000001U
#define GDMA_INLINK_AUTO_RET_CH1_S  20
/** GDMA_INLINK_STOP_CH1 : WT; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define GDMA_INLINK_STOP_CH1    (BIT(21))
#define GDMA_INLINK_STOP_CH1_M  (GDMA_INLINK_STOP_CH1_V << GDMA_INLINK_STOP_CH1_S)
#define GDMA_INLINK_STOP_CH1_V  0x00000001U
#define GDMA_INLINK_STOP_CH1_S  21
/** GDMA_INLINK_START_CH1 : WT; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define GDMA_INLINK_START_CH1    (BIT(22))
#define GDMA_INLINK_START_CH1_M  (GDMA_INLINK_START_CH1_V << GDMA_INLINK_START_CH1_S)
#define GDMA_INLINK_START_CH1_V  0x00000001U
#define GDMA_INLINK_START_CH1_S  22
/** GDMA_INLINK_RESTART_CH1 : WT; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define GDMA_INLINK_RESTART_CH1    (BIT(23))
#define GDMA_INLINK_RESTART_CH1_M  (GDMA_INLINK_RESTART_CH1_V << GDMA_INLINK_RESTART_CH1_S)
#define GDMA_INLINK_RESTART_CH1_V  0x00000001U
#define GDMA_INLINK_RESTART_CH1_S  23
/** GDMA_INLINK_PARK_CH1 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define GDMA_INLINK_PARK_CH1    (BIT(24))
#define GDMA_INLINK_PARK_CH1_M  (GDMA_INLINK_PARK_CH1_V << GDMA_INLINK_PARK_CH1_S)
#define GDMA_INLINK_PARK_CH1_V  0x00000001U
#define GDMA_INLINK_PARK_CH1_S  24

/** GDMA_IN_STATE_CH1_REG register
 *  Receive status of Rx channel 1
 */
#define GDMA_IN_STATE_CH1_REG (DR_REG_GDMA_BASE + 0x144)
/** GDMA_INLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define GDMA_INLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH1_M  (GDMA_INLINK_DSCR_ADDR_CH1_V << GDMA_INLINK_DSCR_ADDR_CH1_S)
#define GDMA_INLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH1_S  0
/** GDMA_IN_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define GDMA_IN_DSCR_STATE_CH1    0x00000003U
#define GDMA_IN_DSCR_STATE_CH1_M  (GDMA_IN_DSCR_STATE_CH1_V << GDMA_IN_DSCR_STATE_CH1_S)
#define GDMA_IN_DSCR_STATE_CH1_V  0x00000003U
#define GDMA_IN_DSCR_STATE_CH1_S  18
/** GDMA_IN_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define GDMA_IN_STATE_CH1    0x00000007U
#define GDMA_IN_STATE_CH1_M  (GDMA_IN_STATE_CH1_V << GDMA_IN_STATE_CH1_S)
#define GDMA_IN_STATE_CH1_V  0x00000007U
#define GDMA_IN_STATE_CH1_S  20

/** GDMA_IN_SUC_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 1
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x148)
/** GDMA_IN_SUC_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_M  (GDMA_IN_SUC_EOF_DES_ADDR_CH1_V << GDMA_IN_SUC_EOF_DES_ADDR_CH1_S)
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH1_S  0

/** GDMA_IN_ERR_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when errors occur of Rx channel 1
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x14c)
/** GDMA_IN_ERR_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_M  (GDMA_IN_ERR_EOF_DES_ADDR_CH1_V << GDMA_IN_ERR_EOF_DES_ADDR_CH1_S)
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH1_S  0

/** GDMA_IN_DSCR_CH1_REG register
 *  Current inlink descriptor address of Rx channel 1
 */
#define GDMA_IN_DSCR_CH1_REG (DR_REG_GDMA_BASE + 0x150)
/** GDMA_INLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define GDMA_INLINK_DSCR_CH1    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH1_M  (GDMA_INLINK_DSCR_CH1_V << GDMA_INLINK_DSCR_CH1_S)
#define GDMA_INLINK_DSCR_CH1_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH1_S  0

/** GDMA_IN_DSCR_BF0_CH1_REG register
 *  The last inlink descriptor address of Rx channel 1
 */
#define GDMA_IN_DSCR_BF0_CH1_REG (DR_REG_GDMA_BASE + 0x154)
/** GDMA_INLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define GDMA_INLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH1_M  (GDMA_INLINK_DSCR_BF0_CH1_V << GDMA_INLINK_DSCR_BF0_CH1_S)
#define GDMA_INLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH1_S  0

/** GDMA_IN_DSCR_BF1_CH1_REG register
 *  The second-to-last inlink descriptor address of Rx channel 1
 */
#define GDMA_IN_DSCR_BF1_CH1_REG (DR_REG_GDMA_BASE + 0x158)
/** GDMA_INLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define GDMA_INLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH1_M  (GDMA_INLINK_DSCR_BF1_CH1_V << GDMA_INLINK_DSCR_BF1_CH1_S)
#define GDMA_INLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH1_S  0

/** GDMA_IN_PRI_CH1_REG register
 *  Priority register of Rx channel 1
 */
#define GDMA_IN_PRI_CH1_REG (DR_REG_GDMA_BASE + 0x15c)
/** GDMA_RX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 1. The larger of the value the higher of the priority.
 */
#define GDMA_RX_PRI_CH1    0x0000000FU
#define GDMA_RX_PRI_CH1_M  (GDMA_RX_PRI_CH1_V << GDMA_RX_PRI_CH1_S)
#define GDMA_RX_PRI_CH1_V  0x0000000FU
#define GDMA_RX_PRI_CH1_S  0

/** GDMA_IN_PERI_SEL_CH1_REG register
 *  Peripheral selection of Rx channel 1
 */
#define GDMA_IN_PERI_SEL_CH1_REG (DR_REG_GDMA_BASE + 0x160)
/** GDMA_PERI_IN_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 0. 0:SPI2. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: Dummy. 5: Dummy. 6: AES. 7: SHA. 8: ADC_DAC. 9: Parallel_IO.
 *  10~15: Dummy
 */
#define GDMA_PERI_IN_SEL_CH1    0x0000003FU
#define GDMA_PERI_IN_SEL_CH1_M  (GDMA_PERI_IN_SEL_CH1_V << GDMA_PERI_IN_SEL_CH1_S)
#define GDMA_PERI_IN_SEL_CH1_V  0x0000003FU
#define GDMA_PERI_IN_SEL_CH1_S  0

/** GDMA_OUT_CONF0_CH1_REG register
 *  Configure 0 register of Tx channel 1
 */
#define GDMA_OUT_CONF0_CH1_REG (DR_REG_GDMA_BASE + 0x190)
/** GDMA_OUT_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset DMA channel 1 Tx FSM and Tx FIFO pointer.
 */
#define GDMA_OUT_RST_CH1    (BIT(0))
#define GDMA_OUT_RST_CH1_M  (GDMA_OUT_RST_CH1_V << GDMA_OUT_RST_CH1_S)
#define GDMA_OUT_RST_CH1_V  0x00000001U
#define GDMA_OUT_RST_CH1_S  0
/** GDMA_OUT_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define GDMA_OUT_LOOP_TEST_CH1    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH1_M  (GDMA_OUT_LOOP_TEST_CH1_V << GDMA_OUT_LOOP_TEST_CH1_S)
#define GDMA_OUT_LOOP_TEST_CH1_V  0x00000001U
#define GDMA_OUT_LOOP_TEST_CH1_S  1
/** GDMA_OUT_AUTO_WRBACK_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define GDMA_OUT_AUTO_WRBACK_CH1    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH1_M  (GDMA_OUT_AUTO_WRBACK_CH1_V << GDMA_OUT_AUTO_WRBACK_CH1_S)
#define GDMA_OUT_AUTO_WRBACK_CH1_V  0x00000001U
#define GDMA_OUT_AUTO_WRBACK_CH1_S  2
/** GDMA_OUT_EOF_MODE_CH1 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 1 is
 *  generated when data need to transmit has been popped from FIFO in DMA
 */
#define GDMA_OUT_EOF_MODE_CH1    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH1_M  (GDMA_OUT_EOF_MODE_CH1_V << GDMA_OUT_EOF_MODE_CH1_S)
#define GDMA_OUT_EOF_MODE_CH1_V  0x00000001U
#define GDMA_OUT_EOF_MODE_CH1_S  3
/** GDMA_OUTDSCR_BURST_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 1 reading link
 *  descriptor when accessing internal SRAM.
 */
#define GDMA_OUTDSCR_BURST_EN_CH1    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH1_M  (GDMA_OUTDSCR_BURST_EN_CH1_V << GDMA_OUTDSCR_BURST_EN_CH1_S)
#define GDMA_OUTDSCR_BURST_EN_CH1_V  0x00000001U
#define GDMA_OUTDSCR_BURST_EN_CH1_S  4
/** GDMA_OUT_DATA_BURST_EN_CH1 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 1 transmitting data
 *  when accessing internal SRAM.
 */
#define GDMA_OUT_DATA_BURST_EN_CH1    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH1_M  (GDMA_OUT_DATA_BURST_EN_CH1_V << GDMA_OUT_DATA_BURST_EN_CH1_S)
#define GDMA_OUT_DATA_BURST_EN_CH1_V  0x00000001U
#define GDMA_OUT_DATA_BURST_EN_CH1_S  5
/** GDMA_OUT_ETM_EN_CH1 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel 1 is triggered by etm
 *  task.
 */
#define GDMA_OUT_ETM_EN_CH1    (BIT(6))
#define GDMA_OUT_ETM_EN_CH1_M  (GDMA_OUT_ETM_EN_CH1_V << GDMA_OUT_ETM_EN_CH1_S)
#define GDMA_OUT_ETM_EN_CH1_V  0x00000001U
#define GDMA_OUT_ETM_EN_CH1_S  6

/** GDMA_OUT_CONF1_CH1_REG register
 *  Configure 1 register of Tx channel 1
 */
#define GDMA_OUT_CONF1_CH1_REG (DR_REG_GDMA_BASE + 0x194)
/** GDMA_OUT_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define GDMA_OUT_CHECK_OWNER_CH1    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH1_M  (GDMA_OUT_CHECK_OWNER_CH1_V << GDMA_OUT_CHECK_OWNER_CH1_S)
#define GDMA_OUT_CHECK_OWNER_CH1_V  0x00000001U
#define GDMA_OUT_CHECK_OWNER_CH1_S  12

/** GDMA_OUTFIFO_STATUS_CH1_REG register
 *  Transmit FIFO status of Tx channel 1
 */
#define GDMA_OUTFIFO_STATUS_CH1_REG (DR_REG_GDMA_BASE + 0x198)
/** GDMA_OUTFIFO_FULL_CH1 : RO; bitpos: [0]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel 1.
 */
#define GDMA_OUTFIFO_FULL_CH1    (BIT(0))
#define GDMA_OUTFIFO_FULL_CH1_M  (GDMA_OUTFIFO_FULL_CH1_V << GDMA_OUTFIFO_FULL_CH1_S)
#define GDMA_OUTFIFO_FULL_CH1_V  0x00000001U
#define GDMA_OUTFIFO_FULL_CH1_S  0
/** GDMA_OUTFIFO_EMPTY_CH1 : RO; bitpos: [1]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel 1.
 */
#define GDMA_OUTFIFO_EMPTY_CH1    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_CH1_M  (GDMA_OUTFIFO_EMPTY_CH1_V << GDMA_OUTFIFO_EMPTY_CH1_S)
#define GDMA_OUTFIFO_EMPTY_CH1_V  0x00000001U
#define GDMA_OUTFIFO_EMPTY_CH1_S  1
/** GDMA_OUTFIFO_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 1.
 */
#define GDMA_OUTFIFO_CNT_CH1    0x0000003FU
#define GDMA_OUTFIFO_CNT_CH1_M  (GDMA_OUTFIFO_CNT_CH1_V << GDMA_OUTFIFO_CNT_CH1_S)
#define GDMA_OUTFIFO_CNT_CH1_V  0x0000003FU
#define GDMA_OUTFIFO_CNT_CH1_S  2
/** GDMA_OUT_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_1B_CH1    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_CH1_M  (GDMA_OUT_REMAIN_UNDER_1B_CH1_V << GDMA_OUT_REMAIN_UNDER_1B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_1B_CH1_S  23
/** GDMA_OUT_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_2B_CH1    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_CH1_M  (GDMA_OUT_REMAIN_UNDER_2B_CH1_V << GDMA_OUT_REMAIN_UNDER_2B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_2B_CH1_S  24
/** GDMA_OUT_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_3B_CH1    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_CH1_M  (GDMA_OUT_REMAIN_UNDER_3B_CH1_V << GDMA_OUT_REMAIN_UNDER_3B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_3B_CH1_S  25
/** GDMA_OUT_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_4B_CH1    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_CH1_M  (GDMA_OUT_REMAIN_UNDER_4B_CH1_V << GDMA_OUT_REMAIN_UNDER_4B_CH1_S)
#define GDMA_OUT_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_4B_CH1_S  26

/** GDMA_OUT_PUSH_CH1_REG register
 *  Push control register of Rx channel 1
 */
#define GDMA_OUT_PUSH_CH1_REG (DR_REG_GDMA_BASE + 0x19c)
/** GDMA_OUTFIFO_WDATA_CH1 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA FIFO.
 */
#define GDMA_OUTFIFO_WDATA_CH1    0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH1_M  (GDMA_OUTFIFO_WDATA_CH1_V << GDMA_OUTFIFO_WDATA_CH1_S)
#define GDMA_OUTFIFO_WDATA_CH1_V  0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH1_S  0
/** GDMA_OUTFIFO_PUSH_CH1 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into DMA FIFO.
 */
#define GDMA_OUTFIFO_PUSH_CH1    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH1_M  (GDMA_OUTFIFO_PUSH_CH1_V << GDMA_OUTFIFO_PUSH_CH1_S)
#define GDMA_OUTFIFO_PUSH_CH1_V  0x00000001U
#define GDMA_OUTFIFO_PUSH_CH1_S  9

/** GDMA_OUT_LINK_CH1_REG register
 *  Link descriptor configure and control register of Tx channel 1
 */
#define GDMA_OUT_LINK_CH1_REG (DR_REG_GDMA_BASE + 0x1a0)
/** GDMA_OUTLINK_ADDR_CH1 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first outlink
 *  descriptor's address.
 */
#define GDMA_OUTLINK_ADDR_CH1    0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH1_M  (GDMA_OUTLINK_ADDR_CH1_V << GDMA_OUTLINK_ADDR_CH1_S)
#define GDMA_OUTLINK_ADDR_CH1_V  0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH1_S  0
/** GDMA_OUTLINK_STOP_CH1 : WT; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define GDMA_OUTLINK_STOP_CH1    (BIT(20))
#define GDMA_OUTLINK_STOP_CH1_M  (GDMA_OUTLINK_STOP_CH1_V << GDMA_OUTLINK_STOP_CH1_S)
#define GDMA_OUTLINK_STOP_CH1_V  0x00000001U
#define GDMA_OUTLINK_STOP_CH1_S  20
/** GDMA_OUTLINK_START_CH1 : WT; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define GDMA_OUTLINK_START_CH1    (BIT(21))
#define GDMA_OUTLINK_START_CH1_M  (GDMA_OUTLINK_START_CH1_V << GDMA_OUTLINK_START_CH1_S)
#define GDMA_OUTLINK_START_CH1_V  0x00000001U
#define GDMA_OUTLINK_START_CH1_S  21
/** GDMA_OUTLINK_RESTART_CH1 : WT; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define GDMA_OUTLINK_RESTART_CH1    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH1_M  (GDMA_OUTLINK_RESTART_CH1_V << GDMA_OUTLINK_RESTART_CH1_S)
#define GDMA_OUTLINK_RESTART_CH1_V  0x00000001U
#define GDMA_OUTLINK_RESTART_CH1_S  22
/** GDMA_OUTLINK_PARK_CH1 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define GDMA_OUTLINK_PARK_CH1    (BIT(23))
#define GDMA_OUTLINK_PARK_CH1_M  (GDMA_OUTLINK_PARK_CH1_V << GDMA_OUTLINK_PARK_CH1_S)
#define GDMA_OUTLINK_PARK_CH1_V  0x00000001U
#define GDMA_OUTLINK_PARK_CH1_S  23

/** GDMA_OUT_STATE_CH1_REG register
 *  Transmit status of Tx channel 1
 */
#define GDMA_OUT_STATE_CH1_REG (DR_REG_GDMA_BASE + 0x1a4)
/** GDMA_OUTLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define GDMA_OUTLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH1_M  (GDMA_OUTLINK_DSCR_ADDR_CH1_V << GDMA_OUTLINK_DSCR_ADDR_CH1_S)
#define GDMA_OUTLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH1_S  0
/** GDMA_OUT_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define GDMA_OUT_DSCR_STATE_CH1    0x00000003U
#define GDMA_OUT_DSCR_STATE_CH1_M  (GDMA_OUT_DSCR_STATE_CH1_V << GDMA_OUT_DSCR_STATE_CH1_S)
#define GDMA_OUT_DSCR_STATE_CH1_V  0x00000003U
#define GDMA_OUT_DSCR_STATE_CH1_S  18
/** GDMA_OUT_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define GDMA_OUT_STATE_CH1    0x00000007U
#define GDMA_OUT_STATE_CH1_M  (GDMA_OUT_STATE_CH1_V << GDMA_OUT_STATE_CH1_S)
#define GDMA_OUT_STATE_CH1_V  0x00000007U
#define GDMA_OUT_STATE_CH1_S  20

/** GDMA_OUT_EOF_DES_ADDR_CH1_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel 1
 */
#define GDMA_OUT_EOF_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x1a8)
/** GDMA_OUT_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_OUT_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH1_M  (GDMA_OUT_EOF_DES_ADDR_CH1_V << GDMA_OUT_EOF_DES_ADDR_CH1_S)
#define GDMA_OUT_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH1_S  0

/** GDMA_OUT_EOF_BFR_DES_ADDR_CH1_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel 1
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_REG (DR_REG_GDMA_BASE + 0x1ac)
/** GDMA_OUT_EOF_BFR_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1    0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_M  (GDMA_OUT_EOF_BFR_DES_ADDR_CH1_V << GDMA_OUT_EOF_BFR_DES_ADDR_CH1_S)
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH1_S  0

/** GDMA_OUT_DSCR_CH1_REG register
 *  Current inlink descriptor address of Tx channel 1
 */
#define GDMA_OUT_DSCR_CH1_REG (DR_REG_GDMA_BASE + 0x1b0)
/** GDMA_OUTLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define GDMA_OUTLINK_DSCR_CH1    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH1_M  (GDMA_OUTLINK_DSCR_CH1_V << GDMA_OUTLINK_DSCR_CH1_S)
#define GDMA_OUTLINK_DSCR_CH1_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH1_S  0

/** GDMA_OUT_DSCR_BF0_CH1_REG register
 *  The last inlink descriptor address of Tx channel 1
 */
#define GDMA_OUT_DSCR_BF0_CH1_REG (DR_REG_GDMA_BASE + 0x1b4)
/** GDMA_OUTLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define GDMA_OUTLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH1_M  (GDMA_OUTLINK_DSCR_BF0_CH1_V << GDMA_OUTLINK_DSCR_BF0_CH1_S)
#define GDMA_OUTLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH1_S  0

/** GDMA_OUT_DSCR_BF1_CH1_REG register
 *  The second-to-last inlink descriptor address of Tx channel 1
 */
#define GDMA_OUT_DSCR_BF1_CH1_REG (DR_REG_GDMA_BASE + 0x1b8)
/** GDMA_OUTLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define GDMA_OUTLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH1_M  (GDMA_OUTLINK_DSCR_BF1_CH1_V << GDMA_OUTLINK_DSCR_BF1_CH1_S)
#define GDMA_OUTLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH1_S  0

/** GDMA_OUT_PRI_CH1_REG register
 *  Priority register of Tx channel 1
 */
#define GDMA_OUT_PRI_CH1_REG (DR_REG_GDMA_BASE + 0x1bc)
/** GDMA_TX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel 1. The larger of the value the higher of the priority.
 */
#define GDMA_TX_PRI_CH1    0x0000000FU
#define GDMA_TX_PRI_CH1_M  (GDMA_TX_PRI_CH1_V << GDMA_TX_PRI_CH1_S)
#define GDMA_TX_PRI_CH1_V  0x0000000FU
#define GDMA_TX_PRI_CH1_S  0

/** GDMA_OUT_PERI_SEL_CH1_REG register
 *  Peripheral selection of Tx channel 1
 */
#define GDMA_OUT_PERI_SEL_CH1_REG (DR_REG_GDMA_BASE + 0x1c0)
/** GDMA_PERI_OUT_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel 0. 0:SPI2. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: Dummy. 5: Dummy. 6: AES. 7: SHA. 8: ADC_DAC. 9: Parallel_IO.
 *  10~15: Dummy
 */
#define GDMA_PERI_OUT_SEL_CH1    0x0000003FU
#define GDMA_PERI_OUT_SEL_CH1_M  (GDMA_PERI_OUT_SEL_CH1_V << GDMA_PERI_OUT_SEL_CH1_S)
#define GDMA_PERI_OUT_SEL_CH1_V  0x0000003FU
#define GDMA_PERI_OUT_SEL_CH1_S  0

/** GDMA_IN_CONF0_CH2_REG register
 *  Configure 0 register of Rx channel 2
 */
#define GDMA_IN_CONF0_CH2_REG (DR_REG_GDMA_BASE + 0x1f0)
/** GDMA_IN_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset DMA channel 2 Rx FSM and Rx FIFO pointer.
 */
#define GDMA_IN_RST_CH2    (BIT(0))
#define GDMA_IN_RST_CH2_M  (GDMA_IN_RST_CH2_V << GDMA_IN_RST_CH2_S)
#define GDMA_IN_RST_CH2_V  0x00000001U
#define GDMA_IN_RST_CH2_S  0
/** GDMA_IN_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define GDMA_IN_LOOP_TEST_CH2    (BIT(1))
#define GDMA_IN_LOOP_TEST_CH2_M  (GDMA_IN_LOOP_TEST_CH2_V << GDMA_IN_LOOP_TEST_CH2_S)
#define GDMA_IN_LOOP_TEST_CH2_V  0x00000001U
#define GDMA_IN_LOOP_TEST_CH2_S  1
/** GDMA_INDSCR_BURST_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 2 reading link
 *  descriptor when accessing internal SRAM.
 */
#define GDMA_INDSCR_BURST_EN_CH2    (BIT(2))
#define GDMA_INDSCR_BURST_EN_CH2_M  (GDMA_INDSCR_BURST_EN_CH2_V << GDMA_INDSCR_BURST_EN_CH2_S)
#define GDMA_INDSCR_BURST_EN_CH2_V  0x00000001U
#define GDMA_INDSCR_BURST_EN_CH2_S  2
/** GDMA_IN_DATA_BURST_EN_CH2 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 2 receiving data
 *  when accessing internal SRAM.
 */
#define GDMA_IN_DATA_BURST_EN_CH2    (BIT(3))
#define GDMA_IN_DATA_BURST_EN_CH2_M  (GDMA_IN_DATA_BURST_EN_CH2_V << GDMA_IN_DATA_BURST_EN_CH2_S)
#define GDMA_IN_DATA_BURST_EN_CH2_V  0x00000001U
#define GDMA_IN_DATA_BURST_EN_CH2_S  3
/** GDMA_MEM_TRANS_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.
 */
#define GDMA_MEM_TRANS_EN_CH2    (BIT(4))
#define GDMA_MEM_TRANS_EN_CH2_M  (GDMA_MEM_TRANS_EN_CH2_V << GDMA_MEM_TRANS_EN_CH2_S)
#define GDMA_MEM_TRANS_EN_CH2_V  0x00000001U
#define GDMA_MEM_TRANS_EN_CH2_S  4
/** GDMA_IN_ETM_EN_CH2 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 2 is triggered by etm
 *  task.
 */
#define GDMA_IN_ETM_EN_CH2    (BIT(5))
#define GDMA_IN_ETM_EN_CH2_M  (GDMA_IN_ETM_EN_CH2_V << GDMA_IN_ETM_EN_CH2_S)
#define GDMA_IN_ETM_EN_CH2_V  0x00000001U
#define GDMA_IN_ETM_EN_CH2_S  5

/** GDMA_IN_CONF1_CH2_REG register
 *  Configure 1 register of Rx channel 2
 */
#define GDMA_IN_CONF1_CH2_REG (DR_REG_GDMA_BASE + 0x1f4)
/** GDMA_IN_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define GDMA_IN_CHECK_OWNER_CH2    (BIT(12))
#define GDMA_IN_CHECK_OWNER_CH2_M  (GDMA_IN_CHECK_OWNER_CH2_V << GDMA_IN_CHECK_OWNER_CH2_S)
#define GDMA_IN_CHECK_OWNER_CH2_V  0x00000001U
#define GDMA_IN_CHECK_OWNER_CH2_S  12

/** GDMA_INFIFO_STATUS_CH2_REG register
 *  Receive FIFO status of Rx channel 2
 */
#define GDMA_INFIFO_STATUS_CH2_REG (DR_REG_GDMA_BASE + 0x1f8)
/** GDMA_INFIFO_FULL_CH2 : RO; bitpos: [0]; default: 1;
 *  L1 Rx FIFO full signal for Rx channel 2.
 */
#define GDMA_INFIFO_FULL_CH2    (BIT(0))
#define GDMA_INFIFO_FULL_CH2_M  (GDMA_INFIFO_FULL_CH2_V << GDMA_INFIFO_FULL_CH2_S)
#define GDMA_INFIFO_FULL_CH2_V  0x00000001U
#define GDMA_INFIFO_FULL_CH2_S  0
/** GDMA_INFIFO_EMPTY_CH2 : RO; bitpos: [1]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 2.
 */
#define GDMA_INFIFO_EMPTY_CH2    (BIT(1))
#define GDMA_INFIFO_EMPTY_CH2_M  (GDMA_INFIFO_EMPTY_CH2_V << GDMA_INFIFO_EMPTY_CH2_S)
#define GDMA_INFIFO_EMPTY_CH2_V  0x00000001U
#define GDMA_INFIFO_EMPTY_CH2_S  1
/** GDMA_INFIFO_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 2.
 */
#define GDMA_INFIFO_CNT_CH2    0x0000003FU
#define GDMA_INFIFO_CNT_CH2_M  (GDMA_INFIFO_CNT_CH2_V << GDMA_INFIFO_CNT_CH2_S)
#define GDMA_INFIFO_CNT_CH2_V  0x0000003FU
#define GDMA_INFIFO_CNT_CH2_S  2
/** GDMA_IN_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_1B_CH2    (BIT(23))
#define GDMA_IN_REMAIN_UNDER_1B_CH2_M  (GDMA_IN_REMAIN_UNDER_1B_CH2_V << GDMA_IN_REMAIN_UNDER_1B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_1B_CH2_S  23
/** GDMA_IN_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_2B_CH2    (BIT(24))
#define GDMA_IN_REMAIN_UNDER_2B_CH2_M  (GDMA_IN_REMAIN_UNDER_2B_CH2_V << GDMA_IN_REMAIN_UNDER_2B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_2B_CH2_S  24
/** GDMA_IN_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_3B_CH2    (BIT(25))
#define GDMA_IN_REMAIN_UNDER_3B_CH2_M  (GDMA_IN_REMAIN_UNDER_3B_CH2_V << GDMA_IN_REMAIN_UNDER_3B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_3B_CH2_S  25
/** GDMA_IN_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define GDMA_IN_REMAIN_UNDER_4B_CH2    (BIT(26))
#define GDMA_IN_REMAIN_UNDER_4B_CH2_M  (GDMA_IN_REMAIN_UNDER_4B_CH2_V << GDMA_IN_REMAIN_UNDER_4B_CH2_S)
#define GDMA_IN_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define GDMA_IN_REMAIN_UNDER_4B_CH2_S  26
/** GDMA_IN_BUF_HUNGRY_CH2 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define GDMA_IN_BUF_HUNGRY_CH2    (BIT(27))
#define GDMA_IN_BUF_HUNGRY_CH2_M  (GDMA_IN_BUF_HUNGRY_CH2_V << GDMA_IN_BUF_HUNGRY_CH2_S)
#define GDMA_IN_BUF_HUNGRY_CH2_V  0x00000001U
#define GDMA_IN_BUF_HUNGRY_CH2_S  27

/** GDMA_IN_POP_CH2_REG register
 *  Pop control register of Rx channel 2
 */
#define GDMA_IN_POP_CH2_REG (DR_REG_GDMA_BASE + 0x1fc)
/** GDMA_INFIFO_RDATA_CH2 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from DMA FIFO.
 */
#define GDMA_INFIFO_RDATA_CH2    0x00000FFFU
#define GDMA_INFIFO_RDATA_CH2_M  (GDMA_INFIFO_RDATA_CH2_V << GDMA_INFIFO_RDATA_CH2_S)
#define GDMA_INFIFO_RDATA_CH2_V  0x00000FFFU
#define GDMA_INFIFO_RDATA_CH2_S  0
/** GDMA_INFIFO_POP_CH2 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from DMA FIFO.
 */
#define GDMA_INFIFO_POP_CH2    (BIT(12))
#define GDMA_INFIFO_POP_CH2_M  (GDMA_INFIFO_POP_CH2_V << GDMA_INFIFO_POP_CH2_S)
#define GDMA_INFIFO_POP_CH2_V  0x00000001U
#define GDMA_INFIFO_POP_CH2_S  12

/** GDMA_IN_LINK_CH2_REG register
 *  Link descriptor configure and control register of Rx channel 2
 */
#define GDMA_IN_LINK_CH2_REG (DR_REG_GDMA_BASE + 0x200)
/** GDMA_INLINK_ADDR_CH2 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first inlink descriptor's
 *  address.
 */
#define GDMA_INLINK_ADDR_CH2    0x000FFFFFU
#define GDMA_INLINK_ADDR_CH2_M  (GDMA_INLINK_ADDR_CH2_V << GDMA_INLINK_ADDR_CH2_S)
#define GDMA_INLINK_ADDR_CH2_V  0x000FFFFFU
#define GDMA_INLINK_ADDR_CH2_S  0
/** GDMA_INLINK_AUTO_RET_CH2 : R/W; bitpos: [20]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define GDMA_INLINK_AUTO_RET_CH2    (BIT(20))
#define GDMA_INLINK_AUTO_RET_CH2_M  (GDMA_INLINK_AUTO_RET_CH2_V << GDMA_INLINK_AUTO_RET_CH2_S)
#define GDMA_INLINK_AUTO_RET_CH2_V  0x00000001U
#define GDMA_INLINK_AUTO_RET_CH2_S  20
/** GDMA_INLINK_STOP_CH2 : WT; bitpos: [21]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define GDMA_INLINK_STOP_CH2    (BIT(21))
#define GDMA_INLINK_STOP_CH2_M  (GDMA_INLINK_STOP_CH2_V << GDMA_INLINK_STOP_CH2_S)
#define GDMA_INLINK_STOP_CH2_V  0x00000001U
#define GDMA_INLINK_STOP_CH2_S  21
/** GDMA_INLINK_START_CH2 : WT; bitpos: [22]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define GDMA_INLINK_START_CH2    (BIT(22))
#define GDMA_INLINK_START_CH2_M  (GDMA_INLINK_START_CH2_V << GDMA_INLINK_START_CH2_S)
#define GDMA_INLINK_START_CH2_V  0x00000001U
#define GDMA_INLINK_START_CH2_S  22
/** GDMA_INLINK_RESTART_CH2 : WT; bitpos: [23]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define GDMA_INLINK_RESTART_CH2    (BIT(23))
#define GDMA_INLINK_RESTART_CH2_M  (GDMA_INLINK_RESTART_CH2_V << GDMA_INLINK_RESTART_CH2_S)
#define GDMA_INLINK_RESTART_CH2_V  0x00000001U
#define GDMA_INLINK_RESTART_CH2_S  23
/** GDMA_INLINK_PARK_CH2 : RO; bitpos: [24]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define GDMA_INLINK_PARK_CH2    (BIT(24))
#define GDMA_INLINK_PARK_CH2_M  (GDMA_INLINK_PARK_CH2_V << GDMA_INLINK_PARK_CH2_S)
#define GDMA_INLINK_PARK_CH2_V  0x00000001U
#define GDMA_INLINK_PARK_CH2_S  24

/** GDMA_IN_STATE_CH2_REG register
 *  Receive status of Rx channel 2
 */
#define GDMA_IN_STATE_CH2_REG (DR_REG_GDMA_BASE + 0x204)
/** GDMA_INLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define GDMA_INLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH2_M  (GDMA_INLINK_DSCR_ADDR_CH2_V << GDMA_INLINK_DSCR_ADDR_CH2_S)
#define GDMA_INLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define GDMA_INLINK_DSCR_ADDR_CH2_S  0
/** GDMA_IN_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define GDMA_IN_DSCR_STATE_CH2    0x00000003U
#define GDMA_IN_DSCR_STATE_CH2_M  (GDMA_IN_DSCR_STATE_CH2_V << GDMA_IN_DSCR_STATE_CH2_S)
#define GDMA_IN_DSCR_STATE_CH2_V  0x00000003U
#define GDMA_IN_DSCR_STATE_CH2_S  18
/** GDMA_IN_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define GDMA_IN_STATE_CH2    0x00000007U
#define GDMA_IN_STATE_CH2_M  (GDMA_IN_STATE_CH2_V << GDMA_IN_STATE_CH2_S)
#define GDMA_IN_STATE_CH2_V  0x00000007U
#define GDMA_IN_STATE_CH2_S  20

/** GDMA_IN_SUC_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 2
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x208)
/** GDMA_IN_SUC_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_M  (GDMA_IN_SUC_EOF_DES_ADDR_CH2_V << GDMA_IN_SUC_EOF_DES_ADDR_CH2_S)
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_IN_SUC_EOF_DES_ADDR_CH2_S  0

/** GDMA_IN_ERR_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when errors occur of Rx channel 2
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x20c)
/** GDMA_IN_ERR_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_M  (GDMA_IN_ERR_EOF_DES_ADDR_CH2_V << GDMA_IN_ERR_EOF_DES_ADDR_CH2_S)
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_IN_ERR_EOF_DES_ADDR_CH2_S  0

/** GDMA_IN_DSCR_CH2_REG register
 *  Current inlink descriptor address of Rx channel 2
 */
#define GDMA_IN_DSCR_CH2_REG (DR_REG_GDMA_BASE + 0x210)
/** GDMA_INLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define GDMA_INLINK_DSCR_CH2    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH2_M  (GDMA_INLINK_DSCR_CH2_V << GDMA_INLINK_DSCR_CH2_S)
#define GDMA_INLINK_DSCR_CH2_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_CH2_S  0

/** GDMA_IN_DSCR_BF0_CH2_REG register
 *  The last inlink descriptor address of Rx channel 2
 */
#define GDMA_IN_DSCR_BF0_CH2_REG (DR_REG_GDMA_BASE + 0x214)
/** GDMA_INLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define GDMA_INLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH2_M  (GDMA_INLINK_DSCR_BF0_CH2_V << GDMA_INLINK_DSCR_BF0_CH2_S)
#define GDMA_INLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF0_CH2_S  0

/** GDMA_IN_DSCR_BF1_CH2_REG register
 *  The second-to-last inlink descriptor address of Rx channel 2
 */
#define GDMA_IN_DSCR_BF1_CH2_REG (DR_REG_GDMA_BASE + 0x218)
/** GDMA_INLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define GDMA_INLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH2_M  (GDMA_INLINK_DSCR_BF1_CH2_V << GDMA_INLINK_DSCR_BF1_CH2_S)
#define GDMA_INLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define GDMA_INLINK_DSCR_BF1_CH2_S  0

/** GDMA_IN_PRI_CH2_REG register
 *  Priority register of Rx channel 2
 */
#define GDMA_IN_PRI_CH2_REG (DR_REG_GDMA_BASE + 0x21c)
/** GDMA_RX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 2. The larger of the value the higher of the priority.
 */
#define GDMA_RX_PRI_CH2    0x0000000FU
#define GDMA_RX_PRI_CH2_M  (GDMA_RX_PRI_CH2_V << GDMA_RX_PRI_CH2_S)
#define GDMA_RX_PRI_CH2_V  0x0000000FU
#define GDMA_RX_PRI_CH2_S  0

/** GDMA_IN_PERI_SEL_CH2_REG register
 *  Peripheral selection of Rx channel 2
 */
#define GDMA_IN_PERI_SEL_CH2_REG (DR_REG_GDMA_BASE + 0x220)
/** GDMA_PERI_IN_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 0. 0:SPI2. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: Dummy. 5: Dummy. 6: AES. 7: SHA. 8: ADC_DAC. 9: Parallel_IO.
 *  10~15: Dummy
 */
#define GDMA_PERI_IN_SEL_CH2    0x0000003FU
#define GDMA_PERI_IN_SEL_CH2_M  (GDMA_PERI_IN_SEL_CH2_V << GDMA_PERI_IN_SEL_CH2_S)
#define GDMA_PERI_IN_SEL_CH2_V  0x0000003FU
#define GDMA_PERI_IN_SEL_CH2_S  0

/** GDMA_OUT_CONF0_CH2_REG register
 *  Configure 0 register of Tx channel 2
 */
#define GDMA_OUT_CONF0_CH2_REG (DR_REG_GDMA_BASE + 0x250)
/** GDMA_OUT_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset DMA channel 2 Tx FSM and Tx FIFO pointer.
 */
#define GDMA_OUT_RST_CH2    (BIT(0))
#define GDMA_OUT_RST_CH2_M  (GDMA_OUT_RST_CH2_V << GDMA_OUT_RST_CH2_S)
#define GDMA_OUT_RST_CH2_V  0x00000001U
#define GDMA_OUT_RST_CH2_S  0
/** GDMA_OUT_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define GDMA_OUT_LOOP_TEST_CH2    (BIT(1))
#define GDMA_OUT_LOOP_TEST_CH2_M  (GDMA_OUT_LOOP_TEST_CH2_V << GDMA_OUT_LOOP_TEST_CH2_S)
#define GDMA_OUT_LOOP_TEST_CH2_V  0x00000001U
#define GDMA_OUT_LOOP_TEST_CH2_S  1
/** GDMA_OUT_AUTO_WRBACK_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define GDMA_OUT_AUTO_WRBACK_CH2    (BIT(2))
#define GDMA_OUT_AUTO_WRBACK_CH2_M  (GDMA_OUT_AUTO_WRBACK_CH2_V << GDMA_OUT_AUTO_WRBACK_CH2_S)
#define GDMA_OUT_AUTO_WRBACK_CH2_V  0x00000001U
#define GDMA_OUT_AUTO_WRBACK_CH2_S  2
/** GDMA_OUT_EOF_MODE_CH2 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 2 is
 *  generated when data need to transmit has been popped from FIFO in DMA
 */
#define GDMA_OUT_EOF_MODE_CH2    (BIT(3))
#define GDMA_OUT_EOF_MODE_CH2_M  (GDMA_OUT_EOF_MODE_CH2_V << GDMA_OUT_EOF_MODE_CH2_S)
#define GDMA_OUT_EOF_MODE_CH2_V  0x00000001U
#define GDMA_OUT_EOF_MODE_CH2_S  3
/** GDMA_OUTDSCR_BURST_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 2 reading link
 *  descriptor when accessing internal SRAM.
 */
#define GDMA_OUTDSCR_BURST_EN_CH2    (BIT(4))
#define GDMA_OUTDSCR_BURST_EN_CH2_M  (GDMA_OUTDSCR_BURST_EN_CH2_V << GDMA_OUTDSCR_BURST_EN_CH2_S)
#define GDMA_OUTDSCR_BURST_EN_CH2_V  0x00000001U
#define GDMA_OUTDSCR_BURST_EN_CH2_S  4
/** GDMA_OUT_DATA_BURST_EN_CH2 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 2 transmitting data
 *  when accessing internal SRAM.
 */
#define GDMA_OUT_DATA_BURST_EN_CH2    (BIT(5))
#define GDMA_OUT_DATA_BURST_EN_CH2_M  (GDMA_OUT_DATA_BURST_EN_CH2_V << GDMA_OUT_DATA_BURST_EN_CH2_S)
#define GDMA_OUT_DATA_BURST_EN_CH2_V  0x00000001U
#define GDMA_OUT_DATA_BURST_EN_CH2_S  5
/** GDMA_OUT_ETM_EN_CH2 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel 2 is triggered by etm
 *  task.
 */
#define GDMA_OUT_ETM_EN_CH2    (BIT(6))
#define GDMA_OUT_ETM_EN_CH2_M  (GDMA_OUT_ETM_EN_CH2_V << GDMA_OUT_ETM_EN_CH2_S)
#define GDMA_OUT_ETM_EN_CH2_V  0x00000001U
#define GDMA_OUT_ETM_EN_CH2_S  6

/** GDMA_OUT_CONF1_CH2_REG register
 *  Configure 1 register of Tx channel 2
 */
#define GDMA_OUT_CONF1_CH2_REG (DR_REG_GDMA_BASE + 0x254)
/** GDMA_OUT_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define GDMA_OUT_CHECK_OWNER_CH2    (BIT(12))
#define GDMA_OUT_CHECK_OWNER_CH2_M  (GDMA_OUT_CHECK_OWNER_CH2_V << GDMA_OUT_CHECK_OWNER_CH2_S)
#define GDMA_OUT_CHECK_OWNER_CH2_V  0x00000001U
#define GDMA_OUT_CHECK_OWNER_CH2_S  12

/** GDMA_OUTFIFO_STATUS_CH2_REG register
 *  Transmit FIFO status of Tx channel 2
 */
#define GDMA_OUTFIFO_STATUS_CH2_REG (DR_REG_GDMA_BASE + 0x258)
/** GDMA_OUTFIFO_FULL_CH2 : RO; bitpos: [0]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel 2.
 */
#define GDMA_OUTFIFO_FULL_CH2    (BIT(0))
#define GDMA_OUTFIFO_FULL_CH2_M  (GDMA_OUTFIFO_FULL_CH2_V << GDMA_OUTFIFO_FULL_CH2_S)
#define GDMA_OUTFIFO_FULL_CH2_V  0x00000001U
#define GDMA_OUTFIFO_FULL_CH2_S  0
/** GDMA_OUTFIFO_EMPTY_CH2 : RO; bitpos: [1]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel 2.
 */
#define GDMA_OUTFIFO_EMPTY_CH2    (BIT(1))
#define GDMA_OUTFIFO_EMPTY_CH2_M  (GDMA_OUTFIFO_EMPTY_CH2_V << GDMA_OUTFIFO_EMPTY_CH2_S)
#define GDMA_OUTFIFO_EMPTY_CH2_V  0x00000001U
#define GDMA_OUTFIFO_EMPTY_CH2_S  1
/** GDMA_OUTFIFO_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 2.
 */
#define GDMA_OUTFIFO_CNT_CH2    0x0000003FU
#define GDMA_OUTFIFO_CNT_CH2_M  (GDMA_OUTFIFO_CNT_CH2_V << GDMA_OUTFIFO_CNT_CH2_S)
#define GDMA_OUTFIFO_CNT_CH2_V  0x0000003FU
#define GDMA_OUTFIFO_CNT_CH2_S  2
/** GDMA_OUT_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_1B_CH2    (BIT(23))
#define GDMA_OUT_REMAIN_UNDER_1B_CH2_M  (GDMA_OUT_REMAIN_UNDER_1B_CH2_V << GDMA_OUT_REMAIN_UNDER_1B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_1B_CH2_S  23
/** GDMA_OUT_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_2B_CH2    (BIT(24))
#define GDMA_OUT_REMAIN_UNDER_2B_CH2_M  (GDMA_OUT_REMAIN_UNDER_2B_CH2_V << GDMA_OUT_REMAIN_UNDER_2B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_2B_CH2_S  24
/** GDMA_OUT_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_3B_CH2    (BIT(25))
#define GDMA_OUT_REMAIN_UNDER_3B_CH2_M  (GDMA_OUT_REMAIN_UNDER_3B_CH2_V << GDMA_OUT_REMAIN_UNDER_3B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_3B_CH2_S  25
/** GDMA_OUT_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define GDMA_OUT_REMAIN_UNDER_4B_CH2    (BIT(26))
#define GDMA_OUT_REMAIN_UNDER_4B_CH2_M  (GDMA_OUT_REMAIN_UNDER_4B_CH2_V << GDMA_OUT_REMAIN_UNDER_4B_CH2_S)
#define GDMA_OUT_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define GDMA_OUT_REMAIN_UNDER_4B_CH2_S  26

/** GDMA_OUT_PUSH_CH2_REG register
 *  Push control register of Rx channel 2
 */
#define GDMA_OUT_PUSH_CH2_REG (DR_REG_GDMA_BASE + 0x25c)
/** GDMA_OUTFIFO_WDATA_CH2 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into DMA FIFO.
 */
#define GDMA_OUTFIFO_WDATA_CH2    0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH2_M  (GDMA_OUTFIFO_WDATA_CH2_V << GDMA_OUTFIFO_WDATA_CH2_S)
#define GDMA_OUTFIFO_WDATA_CH2_V  0x000001FFU
#define GDMA_OUTFIFO_WDATA_CH2_S  0
/** GDMA_OUTFIFO_PUSH_CH2 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into DMA FIFO.
 */
#define GDMA_OUTFIFO_PUSH_CH2    (BIT(9))
#define GDMA_OUTFIFO_PUSH_CH2_M  (GDMA_OUTFIFO_PUSH_CH2_V << GDMA_OUTFIFO_PUSH_CH2_S)
#define GDMA_OUTFIFO_PUSH_CH2_V  0x00000001U
#define GDMA_OUTFIFO_PUSH_CH2_S  9

/** GDMA_OUT_LINK_CH2_REG register
 *  Link descriptor configure and control register of Tx channel 2
 */
#define GDMA_OUT_LINK_CH2_REG (DR_REG_GDMA_BASE + 0x260)
/** GDMA_OUTLINK_ADDR_CH2 : R/W; bitpos: [19:0]; default: 0;
 *  This register stores the 20 least significant bits of the first outlink
 *  descriptor's address.
 */
#define GDMA_OUTLINK_ADDR_CH2    0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH2_M  (GDMA_OUTLINK_ADDR_CH2_V << GDMA_OUTLINK_ADDR_CH2_S)
#define GDMA_OUTLINK_ADDR_CH2_V  0x000FFFFFU
#define GDMA_OUTLINK_ADDR_CH2_S  0
/** GDMA_OUTLINK_STOP_CH2 : WT; bitpos: [20]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define GDMA_OUTLINK_STOP_CH2    (BIT(20))
#define GDMA_OUTLINK_STOP_CH2_M  (GDMA_OUTLINK_STOP_CH2_V << GDMA_OUTLINK_STOP_CH2_S)
#define GDMA_OUTLINK_STOP_CH2_V  0x00000001U
#define GDMA_OUTLINK_STOP_CH2_S  20
/** GDMA_OUTLINK_START_CH2 : WT; bitpos: [21]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define GDMA_OUTLINK_START_CH2    (BIT(21))
#define GDMA_OUTLINK_START_CH2_M  (GDMA_OUTLINK_START_CH2_V << GDMA_OUTLINK_START_CH2_S)
#define GDMA_OUTLINK_START_CH2_V  0x00000001U
#define GDMA_OUTLINK_START_CH2_S  21
/** GDMA_OUTLINK_RESTART_CH2 : WT; bitpos: [22]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define GDMA_OUTLINK_RESTART_CH2    (BIT(22))
#define GDMA_OUTLINK_RESTART_CH2_M  (GDMA_OUTLINK_RESTART_CH2_V << GDMA_OUTLINK_RESTART_CH2_S)
#define GDMA_OUTLINK_RESTART_CH2_V  0x00000001U
#define GDMA_OUTLINK_RESTART_CH2_S  22
/** GDMA_OUTLINK_PARK_CH2 : RO; bitpos: [23]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define GDMA_OUTLINK_PARK_CH2    (BIT(23))
#define GDMA_OUTLINK_PARK_CH2_M  (GDMA_OUTLINK_PARK_CH2_V << GDMA_OUTLINK_PARK_CH2_S)
#define GDMA_OUTLINK_PARK_CH2_V  0x00000001U
#define GDMA_OUTLINK_PARK_CH2_S  23

/** GDMA_OUT_STATE_CH2_REG register
 *  Transmit status of Tx channel 2
 */
#define GDMA_OUT_STATE_CH2_REG (DR_REG_GDMA_BASE + 0x264)
/** GDMA_OUTLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define GDMA_OUTLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH2_M  (GDMA_OUTLINK_DSCR_ADDR_CH2_V << GDMA_OUTLINK_DSCR_ADDR_CH2_S)
#define GDMA_OUTLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define GDMA_OUTLINK_DSCR_ADDR_CH2_S  0
/** GDMA_OUT_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define GDMA_OUT_DSCR_STATE_CH2    0x00000003U
#define GDMA_OUT_DSCR_STATE_CH2_M  (GDMA_OUT_DSCR_STATE_CH2_V << GDMA_OUT_DSCR_STATE_CH2_S)
#define GDMA_OUT_DSCR_STATE_CH2_V  0x00000003U
#define GDMA_OUT_DSCR_STATE_CH2_S  18
/** GDMA_OUT_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define GDMA_OUT_STATE_CH2    0x00000007U
#define GDMA_OUT_STATE_CH2_M  (GDMA_OUT_STATE_CH2_V << GDMA_OUT_STATE_CH2_S)
#define GDMA_OUT_STATE_CH2_V  0x00000007U
#define GDMA_OUT_STATE_CH2_S  20

/** GDMA_OUT_EOF_DES_ADDR_CH2_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel 2
 */
#define GDMA_OUT_EOF_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x268)
/** GDMA_OUT_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define GDMA_OUT_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH2_M  (GDMA_OUT_EOF_DES_ADDR_CH2_V << GDMA_OUT_EOF_DES_ADDR_CH2_S)
#define GDMA_OUT_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_DES_ADDR_CH2_S  0

/** GDMA_OUT_EOF_BFR_DES_ADDR_CH2_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel 2
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_REG (DR_REG_GDMA_BASE + 0x26c)
/** GDMA_OUT_EOF_BFR_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2    0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_M  (GDMA_OUT_EOF_BFR_DES_ADDR_CH2_V << GDMA_OUT_EOF_BFR_DES_ADDR_CH2_S)
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_V  0xFFFFFFFFU
#define GDMA_OUT_EOF_BFR_DES_ADDR_CH2_S  0

/** GDMA_OUT_DSCR_CH2_REG register
 *  Current inlink descriptor address of Tx channel 2
 */
#define GDMA_OUT_DSCR_CH2_REG (DR_REG_GDMA_BASE + 0x270)
/** GDMA_OUTLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define GDMA_OUTLINK_DSCR_CH2    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH2_M  (GDMA_OUTLINK_DSCR_CH2_V << GDMA_OUTLINK_DSCR_CH2_S)
#define GDMA_OUTLINK_DSCR_CH2_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_CH2_S  0

/** GDMA_OUT_DSCR_BF0_CH2_REG register
 *  The last inlink descriptor address of Tx channel 2
 */
#define GDMA_OUT_DSCR_BF0_CH2_REG (DR_REG_GDMA_BASE + 0x274)
/** GDMA_OUTLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define GDMA_OUTLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH2_M  (GDMA_OUTLINK_DSCR_BF0_CH2_V << GDMA_OUTLINK_DSCR_BF0_CH2_S)
#define GDMA_OUTLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF0_CH2_S  0

/** GDMA_OUT_DSCR_BF1_CH2_REG register
 *  The second-to-last inlink descriptor address of Tx channel 2
 */
#define GDMA_OUT_DSCR_BF1_CH2_REG (DR_REG_GDMA_BASE + 0x278)
/** GDMA_OUTLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define GDMA_OUTLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH2_M  (GDMA_OUTLINK_DSCR_BF1_CH2_V << GDMA_OUTLINK_DSCR_BF1_CH2_S)
#define GDMA_OUTLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define GDMA_OUTLINK_DSCR_BF1_CH2_S  0

/** GDMA_OUT_PRI_CH2_REG register
 *  Priority register of Tx channel 2
 */
#define GDMA_OUT_PRI_CH2_REG (DR_REG_GDMA_BASE + 0x27c)
/** GDMA_TX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel 2. The larger of the value the higher of the priority.
 */
#define GDMA_TX_PRI_CH2    0x0000000FU
#define GDMA_TX_PRI_CH2_M  (GDMA_TX_PRI_CH2_V << GDMA_TX_PRI_CH2_S)
#define GDMA_TX_PRI_CH2_V  0x0000000FU
#define GDMA_TX_PRI_CH2_S  0

/** GDMA_OUT_PERI_SEL_CH2_REG register
 *  Peripheral selection of Tx channel 2
 */
#define GDMA_OUT_PERI_SEL_CH2_REG (DR_REG_GDMA_BASE + 0x280)
/** GDMA_PERI_OUT_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel 0. 0:SPI2. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: Dummy. 5: Dummy. 6: AES. 7: SHA. 8: ADC_DAC. 9: Parallel_IO.
 *  10~15: Dummy
 */
#define GDMA_PERI_OUT_SEL_CH2    0x0000003FU
#define GDMA_PERI_OUT_SEL_CH2_M  (GDMA_PERI_OUT_SEL_CH2_V << GDMA_PERI_OUT_SEL_CH2_S)
#define GDMA_PERI_OUT_SEL_CH2_V  0x0000003FU
#define GDMA_PERI_OUT_SEL_CH2_S  0

/** GDMA_BT_TX_SEL_REG register
 *  Bit scrambler selection
 */
#define GDMA_BT_TX_SEL_REG (DR_REG_GDMA_BASE + 0x284)
/** GDMA_BT_TX_SEL_CH0 : R/W; bitpos: [0]; default: 0;
 *  This register is used to select wiitch tx channel 0 across bit scrambler module
 */
#define GDMA_BT_TX_SEL_CH0    (BIT(0))
#define GDMA_BT_TX_SEL_CH0_M  (GDMA_BT_TX_SEL_CH0_V << GDMA_BT_TX_SEL_CH0_S)
#define GDMA_BT_TX_SEL_CH0_V  0x00000001U
#define GDMA_BT_TX_SEL_CH0_S  0
/** GDMA_BT_TX_SEL_CH1 : R/W; bitpos: [1]; default: 0;
 *  This register is used to select wiitch tx channel 1 across bit scrambler module
 */
#define GDMA_BT_TX_SEL_CH1    (BIT(1))
#define GDMA_BT_TX_SEL_CH1_M  (GDMA_BT_TX_SEL_CH1_V << GDMA_BT_TX_SEL_CH1_S)
#define GDMA_BT_TX_SEL_CH1_V  0x00000001U
#define GDMA_BT_TX_SEL_CH1_S  1
/** GDMA_BT_TX_SEL_CH2 : R/W; bitpos: [2]; default: 0;
 *  This register is used to select wiitch tx channel 2 across bit scrambler module
 */
#define GDMA_BT_TX_SEL_CH2    (BIT(2))
#define GDMA_BT_TX_SEL_CH2_M  (GDMA_BT_TX_SEL_CH2_V << GDMA_BT_TX_SEL_CH2_S)
#define GDMA_BT_TX_SEL_CH2_V  0x00000001U
#define GDMA_BT_TX_SEL_CH2_S  2

/** GDMA_BT_RX_SEL_REG register
 *  Bit scrambler selection
 */
#define GDMA_BT_RX_SEL_REG (DR_REG_GDMA_BASE + 0x288)
/** GDMA_BT_RX_SEL_CH0 : R/W; bitpos: [0]; default: 0;
 *  This register is used to select wiitch rx channel 0 across bit scrambler module
 */
#define GDMA_BT_RX_SEL_CH0    (BIT(0))
#define GDMA_BT_RX_SEL_CH0_M  (GDMA_BT_RX_SEL_CH0_V << GDMA_BT_RX_SEL_CH0_S)
#define GDMA_BT_RX_SEL_CH0_V  0x00000001U
#define GDMA_BT_RX_SEL_CH0_S  0
/** GDMA_BT_RX_SEL_CH1 : R/W; bitpos: [1]; default: 0;
 *  This register is used to select wiitch rx channel 1 across bit scrambler module
 */
#define GDMA_BT_RX_SEL_CH1    (BIT(1))
#define GDMA_BT_RX_SEL_CH1_M  (GDMA_BT_RX_SEL_CH1_V << GDMA_BT_RX_SEL_CH1_S)
#define GDMA_BT_RX_SEL_CH1_V  0x00000001U
#define GDMA_BT_RX_SEL_CH1_S  1
/** GDMA_BT_RX_SEL_CH2 : R/W; bitpos: [2]; default: 0;
 *  This register is used to select wiitch rx channel 2 across bit scrambler module
 */
#define GDMA_BT_RX_SEL_CH2    (BIT(2))
#define GDMA_BT_RX_SEL_CH2_M  (GDMA_BT_RX_SEL_CH2_V << GDMA_BT_RX_SEL_CH2_S)
#define GDMA_BT_RX_SEL_CH2_V  0x00000001U
#define GDMA_BT_RX_SEL_CH2_S  2

#ifdef __cplusplus
}
#endif
