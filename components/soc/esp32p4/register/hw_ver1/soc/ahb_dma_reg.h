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

/** AHB_DMA_IN_INT_RAW_CH0_REG register
 *  Raw status interrupt of channel 0
 */
#define AHB_DMA_IN_INT_RAW_CH0_REG (DR_REG_AHB_DMA_BASE + 0x0)
/** AHB_DMA_IN_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0.
 */
#define AHB_DMA_IN_DONE_CH0_INT_RAW    (BIT(0))
#define AHB_DMA_IN_DONE_CH0_INT_RAW_M  (AHB_DMA_IN_DONE_CH0_INT_RAW_V << AHB_DMA_IN_DONE_CH0_INT_RAW_S)
#define AHB_DMA_IN_DONE_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DONE_CH0_INT_RAW_S  0
/** AHB_DMA_IN_SUC_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 0. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 0.
 */
#define AHB_DMA_IN_SUC_EOF_CH0_INT_RAW    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH0_INT_RAW_M  (AHB_DMA_IN_SUC_EOF_CH0_INT_RAW_V << AHB_DMA_IN_SUC_EOF_CH0_INT_RAW_S)
#define AHB_DMA_IN_SUC_EOF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH0_INT_RAW_S  1
/** AHB_DMA_IN_ERR_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 0. For other peripherals this raw
 *  interrupt is reserved.
 */
#define AHB_DMA_IN_ERR_EOF_CH0_INT_RAW    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH0_INT_RAW_M  (AHB_DMA_IN_ERR_EOF_CH0_INT_RAW_V << AHB_DMA_IN_ERR_EOF_CH0_INT_RAW_S)
#define AHB_DMA_IN_ERR_EOF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH0_INT_RAW_S  2
/** AHB_DMA_IN_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 0.
 */
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_RAW    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_RAW_M  (AHB_DMA_IN_DSCR_ERR_CH0_INT_RAW_V << AHB_DMA_IN_DSCR_ERR_CH0_INT_RAW_S)
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_RAW_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 0.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_RAW    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_M  (AHB_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_V << AHB_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_RAW_S  4
/** AHB_DMA_INFIFO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  overflow.
 */
#define AHB_DMA_INFIFO_OVF_CH0_INT_RAW    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH0_INT_RAW_M  (AHB_DMA_INFIFO_OVF_CH0_INT_RAW_V << AHB_DMA_INFIFO_OVF_CH0_INT_RAW_S)
#define AHB_DMA_INFIFO_OVF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH0_INT_RAW_S  5
/** AHB_DMA_INFIFO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
 *  underflow.
 */
#define AHB_DMA_INFIFO_UDF_CH0_INT_RAW    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH0_INT_RAW_M  (AHB_DMA_INFIFO_UDF_CH0_INT_RAW_V << AHB_DMA_INFIFO_UDF_CH0_INT_RAW_S)
#define AHB_DMA_INFIFO_UDF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH0_INT_RAW_S  6

/** AHB_DMA_IN_INT_ST_CH0_REG register
 *  Masked interrupt of channel 0
 */
#define AHB_DMA_IN_INT_ST_CH0_REG (DR_REG_AHB_DMA_BASE + 0x4)
/** AHB_DMA_IN_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH0_INT_ST    (BIT(0))
#define AHB_DMA_IN_DONE_CH0_INT_ST_M  (AHB_DMA_IN_DONE_CH0_INT_ST_V << AHB_DMA_IN_DONE_CH0_INT_ST_S)
#define AHB_DMA_IN_DONE_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DONE_CH0_INT_ST_S  0
/** AHB_DMA_IN_SUC_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ST    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ST_M  (AHB_DMA_IN_SUC_EOF_CH0_INT_ST_V << AHB_DMA_IN_SUC_EOF_CH0_INT_ST_S)
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ST_S  1
/** AHB_DMA_IN_ERR_EOF_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ST    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ST_M  (AHB_DMA_IN_ERR_EOF_CH0_INT_ST_V << AHB_DMA_IN_ERR_EOF_CH0_INT_ST_S)
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ST_S  2
/** AHB_DMA_IN_DSCR_ERR_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ST    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ST_M  (AHB_DMA_IN_DSCR_ERR_CH0_INT_ST_V << AHB_DMA_IN_DSCR_ERR_CH0_INT_ST_S)
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ST_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ST    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ST_M  (AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ST_V << AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ST_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ST_S  4
/** AHB_DMA_INFIFO_OVF_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH0_INT_ST    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH0_INT_ST_M  (AHB_DMA_INFIFO_OVF_CH0_INT_ST_V << AHB_DMA_INFIFO_OVF_CH0_INT_ST_S)
#define AHB_DMA_INFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH0_INT_ST_S  5
/** AHB_DMA_INFIFO_UDF_CH0_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH0_INT_ST    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH0_INT_ST_M  (AHB_DMA_INFIFO_UDF_CH0_INT_ST_V << AHB_DMA_INFIFO_UDF_CH0_INT_ST_S)
#define AHB_DMA_INFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH0_INT_ST_S  6

/** AHB_DMA_IN_INT_ENA_CH0_REG register
 *  Interrupt enable bits of channel 0
 */
#define AHB_DMA_IN_INT_ENA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x8)
/** AHB_DMA_IN_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH0_INT_ENA    (BIT(0))
#define AHB_DMA_IN_DONE_CH0_INT_ENA_M  (AHB_DMA_IN_DONE_CH0_INT_ENA_V << AHB_DMA_IN_DONE_CH0_INT_ENA_S)
#define AHB_DMA_IN_DONE_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DONE_CH0_INT_ENA_S  0
/** AHB_DMA_IN_SUC_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ENA    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ENA_M  (AHB_DMA_IN_SUC_EOF_CH0_INT_ENA_V << AHB_DMA_IN_SUC_EOF_CH0_INT_ENA_S)
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH0_INT_ENA_S  1
/** AHB_DMA_IN_ERR_EOF_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ENA    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ENA_M  (AHB_DMA_IN_ERR_EOF_CH0_INT_ENA_V << AHB_DMA_IN_ERR_EOF_CH0_INT_ENA_S)
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH0_INT_ENA_S  2
/** AHB_DMA_IN_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ENA    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ENA_M  (AHB_DMA_IN_DSCR_ERR_CH0_INT_ENA_V << AHB_DMA_IN_DSCR_ERR_CH0_INT_ENA_S)
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_ENA_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ENA    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_M  (AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_V << AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_ENA_S  4
/** AHB_DMA_INFIFO_OVF_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH0_INT_ENA    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH0_INT_ENA_M  (AHB_DMA_INFIFO_OVF_CH0_INT_ENA_V << AHB_DMA_INFIFO_OVF_CH0_INT_ENA_S)
#define AHB_DMA_INFIFO_OVF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH0_INT_ENA_S  5
/** AHB_DMA_INFIFO_UDF_CH0_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH0_INT_ENA    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH0_INT_ENA_M  (AHB_DMA_INFIFO_UDF_CH0_INT_ENA_V << AHB_DMA_INFIFO_UDF_CH0_INT_ENA_S)
#define AHB_DMA_INFIFO_UDF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH0_INT_ENA_S  6

/** AHB_DMA_IN_INT_CLR_CH0_REG register
 *  Interrupt clear bits of channel 0
 */
#define AHB_DMA_IN_INT_CLR_CH0_REG (DR_REG_AHB_DMA_BASE + 0xc)
/** AHB_DMA_IN_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH0_INT_CLR    (BIT(0))
#define AHB_DMA_IN_DONE_CH0_INT_CLR_M  (AHB_DMA_IN_DONE_CH0_INT_CLR_V << AHB_DMA_IN_DONE_CH0_INT_CLR_S)
#define AHB_DMA_IN_DONE_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DONE_CH0_INT_CLR_S  0
/** AHB_DMA_IN_SUC_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH0_INT_CLR    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH0_INT_CLR_M  (AHB_DMA_IN_SUC_EOF_CH0_INT_CLR_V << AHB_DMA_IN_SUC_EOF_CH0_INT_CLR_S)
#define AHB_DMA_IN_SUC_EOF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH0_INT_CLR_S  1
/** AHB_DMA_IN_ERR_EOF_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH0_INT_CLR    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH0_INT_CLR_M  (AHB_DMA_IN_ERR_EOF_CH0_INT_CLR_V << AHB_DMA_IN_ERR_EOF_CH0_INT_CLR_S)
#define AHB_DMA_IN_ERR_EOF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH0_INT_CLR_S  2
/** AHB_DMA_IN_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_CLR    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_CLR_M  (AHB_DMA_IN_DSCR_ERR_CH0_INT_CLR_V << AHB_DMA_IN_DSCR_ERR_CH0_INT_CLR_S)
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH0_INT_CLR_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_CLR    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_M  (AHB_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_V << AHB_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH0_INT_CLR_S  4
/** AHB_DMA_INFIFO_OVF_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH0_INT_CLR    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH0_INT_CLR_M  (AHB_DMA_INFIFO_OVF_CH0_INT_CLR_V << AHB_DMA_INFIFO_OVF_CH0_INT_CLR_S)
#define AHB_DMA_INFIFO_OVF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH0_INT_CLR_S  5
/** AHB_DMA_INFIFO_UDF_CH0_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH0_INT_CLR    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH0_INT_CLR_M  (AHB_DMA_INFIFO_UDF_CH0_INT_CLR_V << AHB_DMA_INFIFO_UDF_CH0_INT_CLR_S)
#define AHB_DMA_INFIFO_UDF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH0_INT_CLR_S  6

/** AHB_DMA_IN_INT_RAW_CH1_REG register
 *  Raw status interrupt of channel 1
 */
#define AHB_DMA_IN_INT_RAW_CH1_REG (DR_REG_AHB_DMA_BASE + 0x10)
/** AHB_DMA_IN_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 1.
 */
#define AHB_DMA_IN_DONE_CH1_INT_RAW    (BIT(0))
#define AHB_DMA_IN_DONE_CH1_INT_RAW_M  (AHB_DMA_IN_DONE_CH1_INT_RAW_V << AHB_DMA_IN_DONE_CH1_INT_RAW_S)
#define AHB_DMA_IN_DONE_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DONE_CH1_INT_RAW_S  0
/** AHB_DMA_IN_SUC_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 1. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 1.
 */
#define AHB_DMA_IN_SUC_EOF_CH1_INT_RAW    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH1_INT_RAW_M  (AHB_DMA_IN_SUC_EOF_CH1_INT_RAW_V << AHB_DMA_IN_SUC_EOF_CH1_INT_RAW_S)
#define AHB_DMA_IN_SUC_EOF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH1_INT_RAW_S  1
/** AHB_DMA_IN_ERR_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 1. For other peripherals this raw
 *  interrupt is reserved.
 */
#define AHB_DMA_IN_ERR_EOF_CH1_INT_RAW    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH1_INT_RAW_M  (AHB_DMA_IN_ERR_EOF_CH1_INT_RAW_V << AHB_DMA_IN_ERR_EOF_CH1_INT_RAW_S)
#define AHB_DMA_IN_ERR_EOF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH1_INT_RAW_S  2
/** AHB_DMA_IN_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 1.
 */
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_RAW    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_RAW_M  (AHB_DMA_IN_DSCR_ERR_CH1_INT_RAW_V << AHB_DMA_IN_DSCR_ERR_CH1_INT_RAW_S)
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_RAW_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 1.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_RAW    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_M  (AHB_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_V << AHB_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_RAW_S  4
/** AHB_DMA_INFIFO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  overflow.
 */
#define AHB_DMA_INFIFO_OVF_CH1_INT_RAW    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH1_INT_RAW_M  (AHB_DMA_INFIFO_OVF_CH1_INT_RAW_V << AHB_DMA_INFIFO_OVF_CH1_INT_RAW_S)
#define AHB_DMA_INFIFO_OVF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH1_INT_RAW_S  5
/** AHB_DMA_INFIFO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 1 is
 *  underflow.
 */
#define AHB_DMA_INFIFO_UDF_CH1_INT_RAW    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH1_INT_RAW_M  (AHB_DMA_INFIFO_UDF_CH1_INT_RAW_V << AHB_DMA_INFIFO_UDF_CH1_INT_RAW_S)
#define AHB_DMA_INFIFO_UDF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH1_INT_RAW_S  6

/** AHB_DMA_IN_INT_ST_CH1_REG register
 *  Masked interrupt of channel 1
 */
#define AHB_DMA_IN_INT_ST_CH1_REG (DR_REG_AHB_DMA_BASE + 0x14)
/** AHB_DMA_IN_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH1_INT_ST    (BIT(0))
#define AHB_DMA_IN_DONE_CH1_INT_ST_M  (AHB_DMA_IN_DONE_CH1_INT_ST_V << AHB_DMA_IN_DONE_CH1_INT_ST_S)
#define AHB_DMA_IN_DONE_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DONE_CH1_INT_ST_S  0
/** AHB_DMA_IN_SUC_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ST    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ST_M  (AHB_DMA_IN_SUC_EOF_CH1_INT_ST_V << AHB_DMA_IN_SUC_EOF_CH1_INT_ST_S)
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ST_S  1
/** AHB_DMA_IN_ERR_EOF_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ST    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ST_M  (AHB_DMA_IN_ERR_EOF_CH1_INT_ST_V << AHB_DMA_IN_ERR_EOF_CH1_INT_ST_S)
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ST_S  2
/** AHB_DMA_IN_DSCR_ERR_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ST    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ST_M  (AHB_DMA_IN_DSCR_ERR_CH1_INT_ST_V << AHB_DMA_IN_DSCR_ERR_CH1_INT_ST_S)
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ST_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ST    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ST_M  (AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ST_V << AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ST_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ST_S  4
/** AHB_DMA_INFIFO_OVF_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH1_INT_ST    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH1_INT_ST_M  (AHB_DMA_INFIFO_OVF_CH1_INT_ST_V << AHB_DMA_INFIFO_OVF_CH1_INT_ST_S)
#define AHB_DMA_INFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH1_INT_ST_S  5
/** AHB_DMA_INFIFO_UDF_CH1_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH1_INT_ST    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH1_INT_ST_M  (AHB_DMA_INFIFO_UDF_CH1_INT_ST_V << AHB_DMA_INFIFO_UDF_CH1_INT_ST_S)
#define AHB_DMA_INFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH1_INT_ST_S  6

/** AHB_DMA_IN_INT_ENA_CH1_REG register
 *  Interrupt enable bits of channel 1
 */
#define AHB_DMA_IN_INT_ENA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x18)
/** AHB_DMA_IN_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH1_INT_ENA    (BIT(0))
#define AHB_DMA_IN_DONE_CH1_INT_ENA_M  (AHB_DMA_IN_DONE_CH1_INT_ENA_V << AHB_DMA_IN_DONE_CH1_INT_ENA_S)
#define AHB_DMA_IN_DONE_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DONE_CH1_INT_ENA_S  0
/** AHB_DMA_IN_SUC_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ENA    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ENA_M  (AHB_DMA_IN_SUC_EOF_CH1_INT_ENA_V << AHB_DMA_IN_SUC_EOF_CH1_INT_ENA_S)
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH1_INT_ENA_S  1
/** AHB_DMA_IN_ERR_EOF_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ENA    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ENA_M  (AHB_DMA_IN_ERR_EOF_CH1_INT_ENA_V << AHB_DMA_IN_ERR_EOF_CH1_INT_ENA_S)
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH1_INT_ENA_S  2
/** AHB_DMA_IN_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ENA    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ENA_M  (AHB_DMA_IN_DSCR_ERR_CH1_INT_ENA_V << AHB_DMA_IN_DSCR_ERR_CH1_INT_ENA_S)
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_ENA_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ENA    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_M  (AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_V << AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_ENA_S  4
/** AHB_DMA_INFIFO_OVF_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH1_INT_ENA    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH1_INT_ENA_M  (AHB_DMA_INFIFO_OVF_CH1_INT_ENA_V << AHB_DMA_INFIFO_OVF_CH1_INT_ENA_S)
#define AHB_DMA_INFIFO_OVF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH1_INT_ENA_S  5
/** AHB_DMA_INFIFO_UDF_CH1_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH1_INT_ENA    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH1_INT_ENA_M  (AHB_DMA_INFIFO_UDF_CH1_INT_ENA_V << AHB_DMA_INFIFO_UDF_CH1_INT_ENA_S)
#define AHB_DMA_INFIFO_UDF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH1_INT_ENA_S  6

/** AHB_DMA_IN_INT_CLR_CH1_REG register
 *  Interrupt clear bits of channel 1
 */
#define AHB_DMA_IN_INT_CLR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1c)
/** AHB_DMA_IN_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH1_INT_CLR    (BIT(0))
#define AHB_DMA_IN_DONE_CH1_INT_CLR_M  (AHB_DMA_IN_DONE_CH1_INT_CLR_V << AHB_DMA_IN_DONE_CH1_INT_CLR_S)
#define AHB_DMA_IN_DONE_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DONE_CH1_INT_CLR_S  0
/** AHB_DMA_IN_SUC_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH1_INT_CLR    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH1_INT_CLR_M  (AHB_DMA_IN_SUC_EOF_CH1_INT_CLR_V << AHB_DMA_IN_SUC_EOF_CH1_INT_CLR_S)
#define AHB_DMA_IN_SUC_EOF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH1_INT_CLR_S  1
/** AHB_DMA_IN_ERR_EOF_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH1_INT_CLR    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH1_INT_CLR_M  (AHB_DMA_IN_ERR_EOF_CH1_INT_CLR_V << AHB_DMA_IN_ERR_EOF_CH1_INT_CLR_S)
#define AHB_DMA_IN_ERR_EOF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH1_INT_CLR_S  2
/** AHB_DMA_IN_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_CLR    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_CLR_M  (AHB_DMA_IN_DSCR_ERR_CH1_INT_CLR_V << AHB_DMA_IN_DSCR_ERR_CH1_INT_CLR_S)
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH1_INT_CLR_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_CLR    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_M  (AHB_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_V << AHB_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH1_INT_CLR_S  4
/** AHB_DMA_INFIFO_OVF_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH1_INT_CLR    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH1_INT_CLR_M  (AHB_DMA_INFIFO_OVF_CH1_INT_CLR_V << AHB_DMA_INFIFO_OVF_CH1_INT_CLR_S)
#define AHB_DMA_INFIFO_OVF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH1_INT_CLR_S  5
/** AHB_DMA_INFIFO_UDF_CH1_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH1_INT_CLR    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH1_INT_CLR_M  (AHB_DMA_INFIFO_UDF_CH1_INT_CLR_V << AHB_DMA_INFIFO_UDF_CH1_INT_CLR_S)
#define AHB_DMA_INFIFO_UDF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH1_INT_CLR_S  6

/** AHB_DMA_IN_INT_RAW_CH2_REG register
 *  Raw status interrupt of channel 2
 */
#define AHB_DMA_IN_INT_RAW_CH2_REG (DR_REG_AHB_DMA_BASE + 0x20)
/** AHB_DMA_IN_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 2.
 */
#define AHB_DMA_IN_DONE_CH2_INT_RAW    (BIT(0))
#define AHB_DMA_IN_DONE_CH2_INT_RAW_M  (AHB_DMA_IN_DONE_CH2_INT_RAW_V << AHB_DMA_IN_DONE_CH2_INT_RAW_S)
#define AHB_DMA_IN_DONE_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DONE_CH2_INT_RAW_S  0
/** AHB_DMA_IN_SUC_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one inlink
 *  descriptor has been received for Rx channel 2. For UHCI0 the raw interrupt bit
 *  turns to high level when the last data pointed by one inlink descriptor has been
 *  received and no data error is detected for Rx channel 2.
 */
#define AHB_DMA_IN_SUC_EOF_CH2_INT_RAW    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH2_INT_RAW_M  (AHB_DMA_IN_SUC_EOF_CH2_INT_RAW_V << AHB_DMA_IN_SUC_EOF_CH2_INT_RAW_S)
#define AHB_DMA_IN_SUC_EOF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH2_INT_RAW_S  1
/** AHB_DMA_IN_ERR_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when data error is detected only in the
 *  case that the peripheral is UHCI0 for Rx channel 2. For other peripherals this raw
 *  interrupt is reserved.
 */
#define AHB_DMA_IN_ERR_EOF_CH2_INT_RAW    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH2_INT_RAW_M  (AHB_DMA_IN_ERR_EOF_CH2_INT_RAW_V << AHB_DMA_IN_ERR_EOF_CH2_INT_RAW_S)
#define AHB_DMA_IN_ERR_EOF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH2_INT_RAW_S  2
/** AHB_DMA_IN_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when detecting inlink descriptor error
 *  including owner error and the second and third word error of inlink descriptor for
 *  Rx channel 2.
 */
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_RAW    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_RAW_M  (AHB_DMA_IN_DSCR_ERR_CH2_INT_RAW_V << AHB_DMA_IN_DSCR_ERR_CH2_INT_RAW_S)
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_RAW_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
 *  and receiving data is not completed but there is no more inlink for Rx channel 2.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_RAW    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_M  (AHB_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_V << AHB_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_RAW_S  4
/** AHB_DMA_INFIFO_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  overflow.
 */
#define AHB_DMA_INFIFO_OVF_CH2_INT_RAW    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH2_INT_RAW_M  (AHB_DMA_INFIFO_OVF_CH2_INT_RAW_V << AHB_DMA_INFIFO_OVF_CH2_INT_RAW_S)
#define AHB_DMA_INFIFO_OVF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH2_INT_RAW_S  5
/** AHB_DMA_INFIFO_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 2 is
 *  underflow.
 */
#define AHB_DMA_INFIFO_UDF_CH2_INT_RAW    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH2_INT_RAW_M  (AHB_DMA_INFIFO_UDF_CH2_INT_RAW_V << AHB_DMA_INFIFO_UDF_CH2_INT_RAW_S)
#define AHB_DMA_INFIFO_UDF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH2_INT_RAW_S  6

/** AHB_DMA_IN_INT_ST_CH2_REG register
 *  Masked interrupt of channel 2
 */
#define AHB_DMA_IN_INT_ST_CH2_REG (DR_REG_AHB_DMA_BASE + 0x24)
/** AHB_DMA_IN_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH2_INT_ST    (BIT(0))
#define AHB_DMA_IN_DONE_CH2_INT_ST_M  (AHB_DMA_IN_DONE_CH2_INT_ST_V << AHB_DMA_IN_DONE_CH2_INT_ST_S)
#define AHB_DMA_IN_DONE_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DONE_CH2_INT_ST_S  0
/** AHB_DMA_IN_SUC_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ST    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ST_M  (AHB_DMA_IN_SUC_EOF_CH2_INT_ST_V << AHB_DMA_IN_SUC_EOF_CH2_INT_ST_S)
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ST_S  1
/** AHB_DMA_IN_ERR_EOF_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ST    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ST_M  (AHB_DMA_IN_ERR_EOF_CH2_INT_ST_V << AHB_DMA_IN_ERR_EOF_CH2_INT_ST_S)
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ST_S  2
/** AHB_DMA_IN_DSCR_ERR_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ST    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ST_M  (AHB_DMA_IN_DSCR_ERR_CH2_INT_ST_V << AHB_DMA_IN_DSCR_ERR_CH2_INT_ST_S)
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ST_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ST    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ST_M  (AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ST_V << AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ST_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ST_S  4
/** AHB_DMA_INFIFO_OVF_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH2_INT_ST    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH2_INT_ST_M  (AHB_DMA_INFIFO_OVF_CH2_INT_ST_V << AHB_DMA_INFIFO_OVF_CH2_INT_ST_S)
#define AHB_DMA_INFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH2_INT_ST_S  5
/** AHB_DMA_INFIFO_UDF_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH2_INT_ST    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH2_INT_ST_M  (AHB_DMA_INFIFO_UDF_CH2_INT_ST_V << AHB_DMA_INFIFO_UDF_CH2_INT_ST_S)
#define AHB_DMA_INFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH2_INT_ST_S  6

/** AHB_DMA_IN_INT_ENA_CH2_REG register
 *  Interrupt enable bits of channel 2
 */
#define AHB_DMA_IN_INT_ENA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x28)
/** AHB_DMA_IN_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH2_INT_ENA    (BIT(0))
#define AHB_DMA_IN_DONE_CH2_INT_ENA_M  (AHB_DMA_IN_DONE_CH2_INT_ENA_V << AHB_DMA_IN_DONE_CH2_INT_ENA_S)
#define AHB_DMA_IN_DONE_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DONE_CH2_INT_ENA_S  0
/** AHB_DMA_IN_SUC_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ENA    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ENA_M  (AHB_DMA_IN_SUC_EOF_CH2_INT_ENA_V << AHB_DMA_IN_SUC_EOF_CH2_INT_ENA_S)
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH2_INT_ENA_S  1
/** AHB_DMA_IN_ERR_EOF_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ENA    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ENA_M  (AHB_DMA_IN_ERR_EOF_CH2_INT_ENA_V << AHB_DMA_IN_ERR_EOF_CH2_INT_ENA_S)
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH2_INT_ENA_S  2
/** AHB_DMA_IN_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ENA    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ENA_M  (AHB_DMA_IN_DSCR_ERR_CH2_INT_ENA_V << AHB_DMA_IN_DSCR_ERR_CH2_INT_ENA_S)
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_ENA_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ENA    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_M  (AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_V << AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_ENA_S  4
/** AHB_DMA_INFIFO_OVF_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH2_INT_ENA    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH2_INT_ENA_M  (AHB_DMA_INFIFO_OVF_CH2_INT_ENA_V << AHB_DMA_INFIFO_OVF_CH2_INT_ENA_S)
#define AHB_DMA_INFIFO_OVF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH2_INT_ENA_S  5
/** AHB_DMA_INFIFO_UDF_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH2_INT_ENA    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH2_INT_ENA_M  (AHB_DMA_INFIFO_UDF_CH2_INT_ENA_V << AHB_DMA_INFIFO_UDF_CH2_INT_ENA_S)
#define AHB_DMA_INFIFO_UDF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH2_INT_ENA_S  6

/** AHB_DMA_IN_INT_CLR_CH2_REG register
 *  Interrupt clear bits of channel 2
 */
#define AHB_DMA_IN_INT_CLR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x2c)
/** AHB_DMA_IN_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the IN_DONE_CH_INT interrupt.
 */
#define AHB_DMA_IN_DONE_CH2_INT_CLR    (BIT(0))
#define AHB_DMA_IN_DONE_CH2_INT_CLR_M  (AHB_DMA_IN_DONE_CH2_INT_CLR_V << AHB_DMA_IN_DONE_CH2_INT_CLR_S)
#define AHB_DMA_IN_DONE_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DONE_CH2_INT_CLR_S  0
/** AHB_DMA_IN_SUC_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_SUC_EOF_CH2_INT_CLR    (BIT(1))
#define AHB_DMA_IN_SUC_EOF_CH2_INT_CLR_M  (AHB_DMA_IN_SUC_EOF_CH2_INT_CLR_V << AHB_DMA_IN_SUC_EOF_CH2_INT_CLR_S)
#define AHB_DMA_IN_SUC_EOF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_SUC_EOF_CH2_INT_CLR_S  1
/** AHB_DMA_IN_ERR_EOF_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
 */
#define AHB_DMA_IN_ERR_EOF_CH2_INT_CLR    (BIT(2))
#define AHB_DMA_IN_ERR_EOF_CH2_INT_CLR_M  (AHB_DMA_IN_ERR_EOF_CH2_INT_CLR_V << AHB_DMA_IN_ERR_EOF_CH2_INT_CLR_S)
#define AHB_DMA_IN_ERR_EOF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_ERR_EOF_CH2_INT_CLR_S  2
/** AHB_DMA_IN_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_CLR    (BIT(3))
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_CLR_M  (AHB_DMA_IN_DSCR_ERR_CH2_INT_CLR_V << AHB_DMA_IN_DSCR_ERR_CH2_INT_CLR_S)
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DSCR_ERR_CH2_INT_CLR_S  3
/** AHB_DMA_IN_DSCR_EMPTY_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
 */
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_CLR    (BIT(4))
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_M  (AHB_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_V << AHB_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_S)
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_IN_DSCR_EMPTY_CH2_INT_CLR_S  4
/** AHB_DMA_INFIFO_OVF_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_OVF_CH2_INT_CLR    (BIT(5))
#define AHB_DMA_INFIFO_OVF_CH2_INT_CLR_M  (AHB_DMA_INFIFO_OVF_CH2_INT_CLR_V << AHB_DMA_INFIFO_OVF_CH2_INT_CLR_S)
#define AHB_DMA_INFIFO_OVF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_INFIFO_OVF_CH2_INT_CLR_S  5
/** AHB_DMA_INFIFO_UDF_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_INFIFO_UDF_CH2_INT_CLR    (BIT(6))
#define AHB_DMA_INFIFO_UDF_CH2_INT_CLR_M  (AHB_DMA_INFIFO_UDF_CH2_INT_CLR_V << AHB_DMA_INFIFO_UDF_CH2_INT_CLR_S)
#define AHB_DMA_INFIFO_UDF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_INFIFO_UDF_CH2_INT_CLR_S  6

/** AHB_DMA_OUT_INT_RAW_CH0_REG register
 *  Raw status interrupt of channel 0
 */
#define AHB_DMA_OUT_INT_RAW_CH0_REG (DR_REG_AHB_DMA_BASE + 0x30)
/** AHB_DMA_OUT_DONE_CH0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 0.
 */
#define AHB_DMA_OUT_DONE_CH0_INT_RAW    (BIT(0))
#define AHB_DMA_OUT_DONE_CH0_INT_RAW_M  (AHB_DMA_OUT_DONE_CH0_INT_RAW_V << AHB_DMA_OUT_DONE_CH0_INT_RAW_S)
#define AHB_DMA_OUT_DONE_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH0_INT_RAW_S  0
/** AHB_DMA_OUT_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 0.
 */
#define AHB_DMA_OUT_EOF_CH0_INT_RAW    (BIT(1))
#define AHB_DMA_OUT_EOF_CH0_INT_RAW_M  (AHB_DMA_OUT_EOF_CH0_INT_RAW_V << AHB_DMA_OUT_EOF_CH0_INT_RAW_S)
#define AHB_DMA_OUT_EOF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH0_INT_RAW_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH0_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel 0.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_RAW    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_RAW_M  (AHB_DMA_OUT_DSCR_ERR_CH0_INT_RAW_V << AHB_DMA_OUT_DSCR_ERR_CH0_INT_RAW_S)
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_RAW_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH0_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 0.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_RAW    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_M  (AHB_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_V << AHB_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_RAW_S  3
/** AHB_DMA_OUTFIFO_OVF_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  overflow.
 */
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_RAW    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_RAW_M  (AHB_DMA_OUTFIFO_OVF_CH0_INT_RAW_V << AHB_DMA_OUTFIFO_OVF_CH0_INT_RAW_S)
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_RAW_S  4
/** AHB_DMA_OUTFIFO_UDF_CH0_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
 *  underflow.
 */
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_RAW    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_RAW_M  (AHB_DMA_OUTFIFO_UDF_CH0_INT_RAW_V << AHB_DMA_OUTFIFO_UDF_CH0_INT_RAW_S)
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_RAW_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_RAW_S  5

/** AHB_DMA_OUT_INT_ST_CH0_REG register
 *  Masked interrupt of channel 0
 */
#define AHB_DMA_OUT_INT_ST_CH0_REG (DR_REG_AHB_DMA_BASE + 0x34)
/** AHB_DMA_OUT_DONE_CH0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH0_INT_ST    (BIT(0))
#define AHB_DMA_OUT_DONE_CH0_INT_ST_M  (AHB_DMA_OUT_DONE_CH0_INT_ST_V << AHB_DMA_OUT_DONE_CH0_INT_ST_S)
#define AHB_DMA_OUT_DONE_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH0_INT_ST_S  0
/** AHB_DMA_OUT_EOF_CH0_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH0_INT_ST    (BIT(1))
#define AHB_DMA_OUT_EOF_CH0_INT_ST_M  (AHB_DMA_OUT_EOF_CH0_INT_ST_V << AHB_DMA_OUT_EOF_CH0_INT_ST_S)
#define AHB_DMA_OUT_EOF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH0_INT_ST_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH0_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ST    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ST_M  (AHB_DMA_OUT_DSCR_ERR_CH0_INT_ST_V << AHB_DMA_OUT_DSCR_ERR_CH0_INT_ST_S)
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ST_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ST    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ST_M  (AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ST_V << AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ST_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ST_S  3
/** AHB_DMA_OUTFIFO_OVF_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ST    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ST_M  (AHB_DMA_OUTFIFO_OVF_CH0_INT_ST_V << AHB_DMA_OUTFIFO_OVF_CH0_INT_ST_S)
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ST_S  4
/** AHB_DMA_OUTFIFO_UDF_CH0_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ST    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ST_M  (AHB_DMA_OUTFIFO_UDF_CH0_INT_ST_V << AHB_DMA_OUTFIFO_UDF_CH0_INT_ST_S)
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ST_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ST_S  5

/** AHB_DMA_OUT_INT_ENA_CH0_REG register
 *  Interrupt enable bits of channel 0
 */
#define AHB_DMA_OUT_INT_ENA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x38)
/** AHB_DMA_OUT_DONE_CH0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH0_INT_ENA    (BIT(0))
#define AHB_DMA_OUT_DONE_CH0_INT_ENA_M  (AHB_DMA_OUT_DONE_CH0_INT_ENA_V << AHB_DMA_OUT_DONE_CH0_INT_ENA_S)
#define AHB_DMA_OUT_DONE_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH0_INT_ENA_S  0
/** AHB_DMA_OUT_EOF_CH0_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH0_INT_ENA    (BIT(1))
#define AHB_DMA_OUT_EOF_CH0_INT_ENA_M  (AHB_DMA_OUT_EOF_CH0_INT_ENA_V << AHB_DMA_OUT_EOF_CH0_INT_ENA_S)
#define AHB_DMA_OUT_EOF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH0_INT_ENA_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH0_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ENA    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ENA_M  (AHB_DMA_OUT_DSCR_ERR_CH0_INT_ENA_V << AHB_DMA_OUT_DSCR_ERR_CH0_INT_ENA_S)
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_ENA_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ENA    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_M  (AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_V << AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_ENA_S  3
/** AHB_DMA_OUTFIFO_OVF_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ENA    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ENA_M  (AHB_DMA_OUTFIFO_OVF_CH0_INT_ENA_V << AHB_DMA_OUTFIFO_OVF_CH0_INT_ENA_S)
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_ENA_S  4
/** AHB_DMA_OUTFIFO_UDF_CH0_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ENA    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ENA_M  (AHB_DMA_OUTFIFO_UDF_CH0_INT_ENA_V << AHB_DMA_OUTFIFO_UDF_CH0_INT_ENA_S)
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ENA_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_ENA_S  5

/** AHB_DMA_OUT_INT_CLR_CH0_REG register
 *  Interrupt clear bits of channel 0
 */
#define AHB_DMA_OUT_INT_CLR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x3c)
/** AHB_DMA_OUT_DONE_CH0_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH0_INT_CLR    (BIT(0))
#define AHB_DMA_OUT_DONE_CH0_INT_CLR_M  (AHB_DMA_OUT_DONE_CH0_INT_CLR_V << AHB_DMA_OUT_DONE_CH0_INT_CLR_S)
#define AHB_DMA_OUT_DONE_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH0_INT_CLR_S  0
/** AHB_DMA_OUT_EOF_CH0_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH0_INT_CLR    (BIT(1))
#define AHB_DMA_OUT_EOF_CH0_INT_CLR_M  (AHB_DMA_OUT_EOF_CH0_INT_CLR_V << AHB_DMA_OUT_EOF_CH0_INT_CLR_S)
#define AHB_DMA_OUT_EOF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH0_INT_CLR_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH0_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_CLR    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_CLR_M  (AHB_DMA_OUT_DSCR_ERR_CH0_INT_CLR_V << AHB_DMA_OUT_DSCR_ERR_CH0_INT_CLR_S)
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH0_INT_CLR_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH0_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_CLR    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_M  (AHB_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_V << AHB_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH0_INT_CLR_S  3
/** AHB_DMA_OUTFIFO_OVF_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_CLR    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_CLR_M  (AHB_DMA_OUTFIFO_OVF_CH0_INT_CLR_V << AHB_DMA_OUTFIFO_OVF_CH0_INT_CLR_S)
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH0_INT_CLR_S  4
/** AHB_DMA_OUTFIFO_UDF_CH0_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_CLR    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_CLR_M  (AHB_DMA_OUTFIFO_UDF_CH0_INT_CLR_V << AHB_DMA_OUTFIFO_UDF_CH0_INT_CLR_S)
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_CLR_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH0_INT_CLR_S  5

/** AHB_DMA_OUT_INT_RAW_CH1_REG register
 *  Raw status interrupt of channel 1
 */
#define AHB_DMA_OUT_INT_RAW_CH1_REG (DR_REG_AHB_DMA_BASE + 0x40)
/** AHB_DMA_OUT_DONE_CH1_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 1.
 */
#define AHB_DMA_OUT_DONE_CH1_INT_RAW    (BIT(0))
#define AHB_DMA_OUT_DONE_CH1_INT_RAW_M  (AHB_DMA_OUT_DONE_CH1_INT_RAW_V << AHB_DMA_OUT_DONE_CH1_INT_RAW_S)
#define AHB_DMA_OUT_DONE_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH1_INT_RAW_S  0
/** AHB_DMA_OUT_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 1.
 */
#define AHB_DMA_OUT_EOF_CH1_INT_RAW    (BIT(1))
#define AHB_DMA_OUT_EOF_CH1_INT_RAW_M  (AHB_DMA_OUT_EOF_CH1_INT_RAW_V << AHB_DMA_OUT_EOF_CH1_INT_RAW_S)
#define AHB_DMA_OUT_EOF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH1_INT_RAW_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH1_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel 1.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_RAW    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_RAW_M  (AHB_DMA_OUT_DSCR_ERR_CH1_INT_RAW_V << AHB_DMA_OUT_DSCR_ERR_CH1_INT_RAW_S)
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_RAW_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH1_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 1.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_RAW    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_M  (AHB_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_V << AHB_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_RAW_S  3
/** AHB_DMA_OUTFIFO_OVF_CH1_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 1 is
 *  overflow.
 */
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_RAW    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_RAW_M  (AHB_DMA_OUTFIFO_OVF_CH1_INT_RAW_V << AHB_DMA_OUTFIFO_OVF_CH1_INT_RAW_S)
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_RAW_S  4
/** AHB_DMA_OUTFIFO_UDF_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 1 is
 *  underflow.
 */
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_RAW    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_RAW_M  (AHB_DMA_OUTFIFO_UDF_CH1_INT_RAW_V << AHB_DMA_OUTFIFO_UDF_CH1_INT_RAW_S)
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_RAW_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_RAW_S  5

/** AHB_DMA_OUT_INT_ST_CH1_REG register
 *  Masked interrupt of channel 1
 */
#define AHB_DMA_OUT_INT_ST_CH1_REG (DR_REG_AHB_DMA_BASE + 0x44)
/** AHB_DMA_OUT_DONE_CH1_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH1_INT_ST    (BIT(0))
#define AHB_DMA_OUT_DONE_CH1_INT_ST_M  (AHB_DMA_OUT_DONE_CH1_INT_ST_V << AHB_DMA_OUT_DONE_CH1_INT_ST_S)
#define AHB_DMA_OUT_DONE_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH1_INT_ST_S  0
/** AHB_DMA_OUT_EOF_CH1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH1_INT_ST    (BIT(1))
#define AHB_DMA_OUT_EOF_CH1_INT_ST_M  (AHB_DMA_OUT_EOF_CH1_INT_ST_V << AHB_DMA_OUT_EOF_CH1_INT_ST_S)
#define AHB_DMA_OUT_EOF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH1_INT_ST_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH1_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ST    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ST_M  (AHB_DMA_OUT_DSCR_ERR_CH1_INT_ST_V << AHB_DMA_OUT_DSCR_ERR_CH1_INT_ST_S)
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ST_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ST    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ST_M  (AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ST_V << AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ST_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ST_S  3
/** AHB_DMA_OUTFIFO_OVF_CH1_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ST    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ST_M  (AHB_DMA_OUTFIFO_OVF_CH1_INT_ST_V << AHB_DMA_OUTFIFO_OVF_CH1_INT_ST_S)
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ST_S  4
/** AHB_DMA_OUTFIFO_UDF_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ST    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ST_M  (AHB_DMA_OUTFIFO_UDF_CH1_INT_ST_V << AHB_DMA_OUTFIFO_UDF_CH1_INT_ST_S)
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ST_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ST_S  5

/** AHB_DMA_OUT_INT_ENA_CH1_REG register
 *  Interrupt enable bits of channel 1
 */
#define AHB_DMA_OUT_INT_ENA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x48)
/** AHB_DMA_OUT_DONE_CH1_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH1_INT_ENA    (BIT(0))
#define AHB_DMA_OUT_DONE_CH1_INT_ENA_M  (AHB_DMA_OUT_DONE_CH1_INT_ENA_V << AHB_DMA_OUT_DONE_CH1_INT_ENA_S)
#define AHB_DMA_OUT_DONE_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH1_INT_ENA_S  0
/** AHB_DMA_OUT_EOF_CH1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH1_INT_ENA    (BIT(1))
#define AHB_DMA_OUT_EOF_CH1_INT_ENA_M  (AHB_DMA_OUT_EOF_CH1_INT_ENA_V << AHB_DMA_OUT_EOF_CH1_INT_ENA_S)
#define AHB_DMA_OUT_EOF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH1_INT_ENA_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH1_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ENA    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ENA_M  (AHB_DMA_OUT_DSCR_ERR_CH1_INT_ENA_V << AHB_DMA_OUT_DSCR_ERR_CH1_INT_ENA_S)
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_ENA_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ENA    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_M  (AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_V << AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_ENA_S  3
/** AHB_DMA_OUTFIFO_OVF_CH1_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ENA    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ENA_M  (AHB_DMA_OUTFIFO_OVF_CH1_INT_ENA_V << AHB_DMA_OUTFIFO_OVF_CH1_INT_ENA_S)
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_ENA_S  4
/** AHB_DMA_OUTFIFO_UDF_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ENA    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ENA_M  (AHB_DMA_OUTFIFO_UDF_CH1_INT_ENA_V << AHB_DMA_OUTFIFO_UDF_CH1_INT_ENA_S)
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ENA_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_ENA_S  5

/** AHB_DMA_OUT_INT_CLR_CH1_REG register
 *  Interrupt clear bits of channel 1
 */
#define AHB_DMA_OUT_INT_CLR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x4c)
/** AHB_DMA_OUT_DONE_CH1_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH1_INT_CLR    (BIT(0))
#define AHB_DMA_OUT_DONE_CH1_INT_CLR_M  (AHB_DMA_OUT_DONE_CH1_INT_CLR_V << AHB_DMA_OUT_DONE_CH1_INT_CLR_S)
#define AHB_DMA_OUT_DONE_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH1_INT_CLR_S  0
/** AHB_DMA_OUT_EOF_CH1_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH1_INT_CLR    (BIT(1))
#define AHB_DMA_OUT_EOF_CH1_INT_CLR_M  (AHB_DMA_OUT_EOF_CH1_INT_CLR_V << AHB_DMA_OUT_EOF_CH1_INT_CLR_S)
#define AHB_DMA_OUT_EOF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH1_INT_CLR_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH1_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_CLR    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_CLR_M  (AHB_DMA_OUT_DSCR_ERR_CH1_INT_CLR_V << AHB_DMA_OUT_DSCR_ERR_CH1_INT_CLR_S)
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH1_INT_CLR_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH1_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_CLR    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_M  (AHB_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_V << AHB_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH1_INT_CLR_S  3
/** AHB_DMA_OUTFIFO_OVF_CH1_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_CLR    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_CLR_M  (AHB_DMA_OUTFIFO_OVF_CH1_INT_CLR_V << AHB_DMA_OUTFIFO_OVF_CH1_INT_CLR_S)
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH1_INT_CLR_S  4
/** AHB_DMA_OUTFIFO_UDF_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_CLR    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_CLR_M  (AHB_DMA_OUTFIFO_UDF_CH1_INT_CLR_V << AHB_DMA_OUTFIFO_UDF_CH1_INT_CLR_S)
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_CLR_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH1_INT_CLR_S  5

/** AHB_DMA_OUT_INT_RAW_CH2_REG register
 *  Raw status interrupt of channel 2
 */
#define AHB_DMA_OUT_INT_RAW_CH2_REG (DR_REG_AHB_DMA_BASE + 0x50)
/** AHB_DMA_OUT_DONE_CH2_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been transmitted to peripherals for Tx channel 2.
 */
#define AHB_DMA_OUT_DONE_CH2_INT_RAW    (BIT(0))
#define AHB_DMA_OUT_DONE_CH2_INT_RAW_M  (AHB_DMA_OUT_DONE_CH2_INT_RAW_V << AHB_DMA_OUT_DONE_CH2_INT_RAW_S)
#define AHB_DMA_OUT_DONE_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH2_INT_RAW_S  0
/** AHB_DMA_OUT_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt bit turns to high level when the last data pointed by one outlink
 *  descriptor has been read from memory for Tx channel 2.
 */
#define AHB_DMA_OUT_EOF_CH2_INT_RAW    (BIT(1))
#define AHB_DMA_OUT_EOF_CH2_INT_RAW_M  (AHB_DMA_OUT_EOF_CH2_INT_RAW_V << AHB_DMA_OUT_EOF_CH2_INT_RAW_S)
#define AHB_DMA_OUT_EOF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH2_INT_RAW_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt bit turns to high level when detecting outlink descriptor error
 *  including owner error and the second and third word error of outlink descriptor for
 *  Tx channel 2.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_RAW    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_RAW_M  (AHB_DMA_OUT_DSCR_ERR_CH2_INT_RAW_V << AHB_DMA_OUT_DSCR_ERR_CH2_INT_RAW_S)
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_RAW_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH2_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt bit turns to high level when data corresponding a outlink
 *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
 *  channel 2.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_RAW    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_M  (AHB_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_V << AHB_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_RAW_S  3
/** AHB_DMA_OUTFIFO_OVF_CH2_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 2 is
 *  overflow.
 */
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_RAW    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_RAW_M  (AHB_DMA_OUTFIFO_OVF_CH2_INT_RAW_V << AHB_DMA_OUTFIFO_OVF_CH2_INT_RAW_S)
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_RAW_S  4
/** AHB_DMA_OUTFIFO_UDF_CH2_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 2 is
 *  underflow.
 */
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_RAW    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_RAW_M  (AHB_DMA_OUTFIFO_UDF_CH2_INT_RAW_V << AHB_DMA_OUTFIFO_UDF_CH2_INT_RAW_S)
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_RAW_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_RAW_S  5

/** AHB_DMA_OUT_INT_ST_CH2_REG register
 *  Masked interrupt of channel 2
 */
#define AHB_DMA_OUT_INT_ST_CH2_REG (DR_REG_AHB_DMA_BASE + 0x54)
/** AHB_DMA_OUT_DONE_CH2_INT_ST : RO; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH2_INT_ST    (BIT(0))
#define AHB_DMA_OUT_DONE_CH2_INT_ST_M  (AHB_DMA_OUT_DONE_CH2_INT_ST_V << AHB_DMA_OUT_DONE_CH2_INT_ST_S)
#define AHB_DMA_OUT_DONE_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH2_INT_ST_S  0
/** AHB_DMA_OUT_EOF_CH2_INT_ST : RO; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH2_INT_ST    (BIT(1))
#define AHB_DMA_OUT_EOF_CH2_INT_ST_M  (AHB_DMA_OUT_EOF_CH2_INT_ST_V << AHB_DMA_OUT_EOF_CH2_INT_ST_S)
#define AHB_DMA_OUT_EOF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH2_INT_ST_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ST    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ST_M  (AHB_DMA_OUT_DSCR_ERR_CH2_INT_ST_V << AHB_DMA_OUT_DSCR_ERR_CH2_INT_ST_S)
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ST_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ST : RO; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ST    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ST_M  (AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ST_V << AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ST_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ST_S  3
/** AHB_DMA_OUTFIFO_OVF_CH2_INT_ST : RO; bitpos: [4]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ST    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ST_M  (AHB_DMA_OUTFIFO_OVF_CH2_INT_ST_V << AHB_DMA_OUTFIFO_OVF_CH2_INT_ST_S)
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ST_S  4
/** AHB_DMA_OUTFIFO_UDF_CH2_INT_ST : RO; bitpos: [5]; default: 0;
 *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ST    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ST_M  (AHB_DMA_OUTFIFO_UDF_CH2_INT_ST_V << AHB_DMA_OUTFIFO_UDF_CH2_INT_ST_S)
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ST_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ST_S  5

/** AHB_DMA_OUT_INT_ENA_CH2_REG register
 *  Interrupt enable bits of channel 2
 */
#define AHB_DMA_OUT_INT_ENA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x58)
/** AHB_DMA_OUT_DONE_CH2_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH2_INT_ENA    (BIT(0))
#define AHB_DMA_OUT_DONE_CH2_INT_ENA_M  (AHB_DMA_OUT_DONE_CH2_INT_ENA_V << AHB_DMA_OUT_DONE_CH2_INT_ENA_S)
#define AHB_DMA_OUT_DONE_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH2_INT_ENA_S  0
/** AHB_DMA_OUT_EOF_CH2_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH2_INT_ENA    (BIT(1))
#define AHB_DMA_OUT_EOF_CH2_INT_ENA_M  (AHB_DMA_OUT_EOF_CH2_INT_ENA_V << AHB_DMA_OUT_EOF_CH2_INT_ENA_S)
#define AHB_DMA_OUT_EOF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH2_INT_ENA_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ENA    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ENA_M  (AHB_DMA_OUT_DSCR_ERR_CH2_INT_ENA_V << AHB_DMA_OUT_DSCR_ERR_CH2_INT_ENA_S)
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_ENA_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ENA    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_M  (AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_V << AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_ENA_S  3
/** AHB_DMA_OUTFIFO_OVF_CH2_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ENA    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ENA_M  (AHB_DMA_OUTFIFO_OVF_CH2_INT_ENA_V << AHB_DMA_OUTFIFO_OVF_CH2_INT_ENA_S)
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_ENA_S  4
/** AHB_DMA_OUTFIFO_UDF_CH2_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ENA    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ENA_M  (AHB_DMA_OUTFIFO_UDF_CH2_INT_ENA_V << AHB_DMA_OUTFIFO_UDF_CH2_INT_ENA_S)
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ENA_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_ENA_S  5

/** AHB_DMA_OUT_INT_CLR_CH2_REG register
 *  Interrupt clear bits of channel 2
 */
#define AHB_DMA_OUT_INT_CLR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x5c)
/** AHB_DMA_OUT_DONE_CH2_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DONE_CH2_INT_CLR    (BIT(0))
#define AHB_DMA_OUT_DONE_CH2_INT_CLR_M  (AHB_DMA_OUT_DONE_CH2_INT_CLR_V << AHB_DMA_OUT_DONE_CH2_INT_CLR_S)
#define AHB_DMA_OUT_DONE_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_DONE_CH2_INT_CLR_S  0
/** AHB_DMA_OUT_EOF_CH2_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_EOF_CH2_INT_CLR    (BIT(1))
#define AHB_DMA_OUT_EOF_CH2_INT_CLR_M  (AHB_DMA_OUT_EOF_CH2_INT_CLR_V << AHB_DMA_OUT_EOF_CH2_INT_CLR_S)
#define AHB_DMA_OUT_EOF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_EOF_CH2_INT_CLR_S  1
/** AHB_DMA_OUT_DSCR_ERR_CH2_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
 */
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_CLR    (BIT(2))
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_CLR_M  (AHB_DMA_OUT_DSCR_ERR_CH2_INT_CLR_V << AHB_DMA_OUT_DSCR_ERR_CH2_INT_CLR_S)
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_DSCR_ERR_CH2_INT_CLR_S  2
/** AHB_DMA_OUT_TOTAL_EOF_CH2_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
 */
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_CLR    (BIT(3))
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_M  (AHB_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_V << AHB_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_S)
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_OUT_TOTAL_EOF_CH2_INT_CLR_S  3
/** AHB_DMA_OUTFIFO_OVF_CH2_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_CLR    (BIT(4))
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_CLR_M  (AHB_DMA_OUTFIFO_OVF_CH2_INT_CLR_V << AHB_DMA_OUTFIFO_OVF_CH2_INT_CLR_S)
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_OUTFIFO_OVF_CH2_INT_CLR_S  4
/** AHB_DMA_OUTFIFO_UDF_CH2_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
 */
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_CLR    (BIT(5))
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_CLR_M  (AHB_DMA_OUTFIFO_UDF_CH2_INT_CLR_V << AHB_DMA_OUTFIFO_UDF_CH2_INT_CLR_S)
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_CLR_V  0x00000001U
#define AHB_DMA_OUTFIFO_UDF_CH2_INT_CLR_S  5

/** AHB_DMA_AHB_TEST_REG register
 *  reserved
 */
#define AHB_DMA_AHB_TEST_REG (DR_REG_AHB_DMA_BASE + 0x60)
/** AHB_DMA_AHB_TESTMODE : R/W; bitpos: [2:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_AHB_TESTMODE    0x00000007U
#define AHB_DMA_AHB_TESTMODE_M  (AHB_DMA_AHB_TESTMODE_V << AHB_DMA_AHB_TESTMODE_S)
#define AHB_DMA_AHB_TESTMODE_V  0x00000007U
#define AHB_DMA_AHB_TESTMODE_S  0
/** AHB_DMA_AHB_TESTADDR : R/W; bitpos: [5:4]; default: 0;
 *  reserved
 */
#define AHB_DMA_AHB_TESTADDR    0x00000003U
#define AHB_DMA_AHB_TESTADDR_M  (AHB_DMA_AHB_TESTADDR_V << AHB_DMA_AHB_TESTADDR_S)
#define AHB_DMA_AHB_TESTADDR_V  0x00000003U
#define AHB_DMA_AHB_TESTADDR_S  4

/** AHB_DMA_MISC_CONF_REG register
 *  MISC register
 */
#define AHB_DMA_MISC_CONF_REG (DR_REG_AHB_DMA_BASE + 0x64)
/** AHB_DMA_AHBM_RST_INTER : R/W; bitpos: [0]; default: 0;
 *  Set this bit then clear this bit to reset the internal ahb FSM.
 */
#define AHB_DMA_AHBM_RST_INTER    (BIT(0))
#define AHB_DMA_AHBM_RST_INTER_M  (AHB_DMA_AHBM_RST_INTER_V << AHB_DMA_AHBM_RST_INTER_S)
#define AHB_DMA_AHBM_RST_INTER_V  0x00000001U
#define AHB_DMA_AHBM_RST_INTER_S  0
/** AHB_DMA_ARB_PRI_DIS : R/W; bitpos: [2]; default: 0;
 *  Set this bit to disable priority arbitration function.
 */
#define AHB_DMA_ARB_PRI_DIS    (BIT(2))
#define AHB_DMA_ARB_PRI_DIS_M  (AHB_DMA_ARB_PRI_DIS_V << AHB_DMA_ARB_PRI_DIS_S)
#define AHB_DMA_ARB_PRI_DIS_V  0x00000001U
#define AHB_DMA_ARB_PRI_DIS_S  2
/** AHB_DMA_CLK_EN : R/W; bitpos: [3]; default: 0;
 *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
 *  registers.
 */
#define AHB_DMA_CLK_EN    (BIT(3))
#define AHB_DMA_CLK_EN_M  (AHB_DMA_CLK_EN_V << AHB_DMA_CLK_EN_S)
#define AHB_DMA_CLK_EN_V  0x00000001U
#define AHB_DMA_CLK_EN_S  3

/** AHB_DMA_DATE_REG register
 *  Version control register
 */
#define AHB_DMA_DATE_REG (DR_REG_AHB_DMA_BASE + 0x68)
/** AHB_DMA_DATE : R/W; bitpos: [31:0]; default: 36712768;
 *  register version.
 */
#define AHB_DMA_DATE    0xFFFFFFFFU
#define AHB_DMA_DATE_M  (AHB_DMA_DATE_V << AHB_DMA_DATE_S)
#define AHB_DMA_DATE_V  0xFFFFFFFFU
#define AHB_DMA_DATE_S  0

/** AHB_DMA_IN_CONF0_CH0_REG register
 *  Configure 0 register of Rx channel 0
 */
#define AHB_DMA_IN_CONF0_CH0_REG (DR_REG_AHB_DMA_BASE + 0x70)
/** AHB_DMA_IN_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AHB_DMA channel 0 Rx FSM and Rx FIFO pointer.
 */
#define AHB_DMA_IN_RST_CH0    (BIT(0))
#define AHB_DMA_IN_RST_CH0_M  (AHB_DMA_IN_RST_CH0_V << AHB_DMA_IN_RST_CH0_S)
#define AHB_DMA_IN_RST_CH0_V  0x00000001U
#define AHB_DMA_IN_RST_CH0_S  0
/** AHB_DMA_IN_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_LOOP_TEST_CH0    (BIT(1))
#define AHB_DMA_IN_LOOP_TEST_CH0_M  (AHB_DMA_IN_LOOP_TEST_CH0_V << AHB_DMA_IN_LOOP_TEST_CH0_S)
#define AHB_DMA_IN_LOOP_TEST_CH0_V  0x00000001U
#define AHB_DMA_IN_LOOP_TEST_CH0_S  1
/** AHB_DMA_INDSCR_BURST_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AHB_DMA_INDSCR_BURST_EN_CH0    (BIT(2))
#define AHB_DMA_INDSCR_BURST_EN_CH0_M  (AHB_DMA_INDSCR_BURST_EN_CH0_V << AHB_DMA_INDSCR_BURST_EN_CH0_S)
#define AHB_DMA_INDSCR_BURST_EN_CH0_V  0x00000001U
#define AHB_DMA_INDSCR_BURST_EN_CH0_S  2
/** AHB_DMA_IN_DATA_BURST_EN_CH0 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 receiving data
 *  when accessing internal SRAM.
 */
#define AHB_DMA_IN_DATA_BURST_EN_CH0    (BIT(3))
#define AHB_DMA_IN_DATA_BURST_EN_CH0_M  (AHB_DMA_IN_DATA_BURST_EN_CH0_V << AHB_DMA_IN_DATA_BURST_EN_CH0_S)
#define AHB_DMA_IN_DATA_BURST_EN_CH0_V  0x00000001U
#define AHB_DMA_IN_DATA_BURST_EN_CH0_S  3
/** AHB_DMA_MEM_TRANS_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via
 *  AHB_DMA.
 */
#define AHB_DMA_MEM_TRANS_EN_CH0    (BIT(4))
#define AHB_DMA_MEM_TRANS_EN_CH0_M  (AHB_DMA_MEM_TRANS_EN_CH0_V << AHB_DMA_MEM_TRANS_EN_CH0_S)
#define AHB_DMA_MEM_TRANS_EN_CH0_V  0x00000001U
#define AHB_DMA_MEM_TRANS_EN_CH0_S  4
/** AHB_DMA_IN_ETM_EN_CH0 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 0 is triggered by etm
 *  task.
 */
#define AHB_DMA_IN_ETM_EN_CH0    (BIT(5))
#define AHB_DMA_IN_ETM_EN_CH0_M  (AHB_DMA_IN_ETM_EN_CH0_V << AHB_DMA_IN_ETM_EN_CH0_S)
#define AHB_DMA_IN_ETM_EN_CH0_V  0x00000001U
#define AHB_DMA_IN_ETM_EN_CH0_S  5

/** AHB_DMA_IN_CONF1_CH0_REG register
 *  Configure 1 register of Rx channel 0
 */
#define AHB_DMA_IN_CONF1_CH0_REG (DR_REG_AHB_DMA_BASE + 0x74)
/** AHB_DMA_IN_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AHB_DMA_IN_CHECK_OWNER_CH0    (BIT(12))
#define AHB_DMA_IN_CHECK_OWNER_CH0_M  (AHB_DMA_IN_CHECK_OWNER_CH0_V << AHB_DMA_IN_CHECK_OWNER_CH0_S)
#define AHB_DMA_IN_CHECK_OWNER_CH0_V  0x00000001U
#define AHB_DMA_IN_CHECK_OWNER_CH0_S  12

/** AHB_DMA_INFIFO_STATUS_CH0_REG register
 *  Receive FIFO status of Rx channel 0
 */
#define AHB_DMA_INFIFO_STATUS_CH0_REG (DR_REG_AHB_DMA_BASE + 0x78)
/** AHB_DMA_INFIFO_FULL_CH0 : RO; bitpos: [0]; default: 1;
 *  L1 Rx FIFO full signal for Rx channel 0.
 */
#define AHB_DMA_INFIFO_FULL_CH0    (BIT(0))
#define AHB_DMA_INFIFO_FULL_CH0_M  (AHB_DMA_INFIFO_FULL_CH0_V << AHB_DMA_INFIFO_FULL_CH0_S)
#define AHB_DMA_INFIFO_FULL_CH0_V  0x00000001U
#define AHB_DMA_INFIFO_FULL_CH0_S  0
/** AHB_DMA_INFIFO_EMPTY_CH0 : RO; bitpos: [1]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 0.
 */
#define AHB_DMA_INFIFO_EMPTY_CH0    (BIT(1))
#define AHB_DMA_INFIFO_EMPTY_CH0_M  (AHB_DMA_INFIFO_EMPTY_CH0_V << AHB_DMA_INFIFO_EMPTY_CH0_S)
#define AHB_DMA_INFIFO_EMPTY_CH0_V  0x00000001U
#define AHB_DMA_INFIFO_EMPTY_CH0_S  1
/** AHB_DMA_INFIFO_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.
 */
#define AHB_DMA_INFIFO_CNT_CH0    0x0000003FU
#define AHB_DMA_INFIFO_CNT_CH0_M  (AHB_DMA_INFIFO_CNT_CH0_V << AHB_DMA_INFIFO_CNT_CH0_S)
#define AHB_DMA_INFIFO_CNT_CH0_V  0x0000003FU
#define AHB_DMA_INFIFO_CNT_CH0_S  2
/** AHB_DMA_IN_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH0    (BIT(23))
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH0_M  (AHB_DMA_IN_REMAIN_UNDER_1B_CH0_V << AHB_DMA_IN_REMAIN_UNDER_1B_CH0_S)
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH0_S  23
/** AHB_DMA_IN_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH0    (BIT(24))
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH0_M  (AHB_DMA_IN_REMAIN_UNDER_2B_CH0_V << AHB_DMA_IN_REMAIN_UNDER_2B_CH0_S)
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH0_S  24
/** AHB_DMA_IN_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH0    (BIT(25))
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH0_M  (AHB_DMA_IN_REMAIN_UNDER_3B_CH0_V << AHB_DMA_IN_REMAIN_UNDER_3B_CH0_S)
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH0_S  25
/** AHB_DMA_IN_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH0    (BIT(26))
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH0_M  (AHB_DMA_IN_REMAIN_UNDER_4B_CH0_V << AHB_DMA_IN_REMAIN_UNDER_4B_CH0_S)
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH0_S  26
/** AHB_DMA_IN_BUF_HUNGRY_CH0 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_BUF_HUNGRY_CH0    (BIT(27))
#define AHB_DMA_IN_BUF_HUNGRY_CH0_M  (AHB_DMA_IN_BUF_HUNGRY_CH0_V << AHB_DMA_IN_BUF_HUNGRY_CH0_S)
#define AHB_DMA_IN_BUF_HUNGRY_CH0_V  0x00000001U
#define AHB_DMA_IN_BUF_HUNGRY_CH0_S  27

/** AHB_DMA_IN_POP_CH0_REG register
 *  Pop control register of Rx channel 0
 */
#define AHB_DMA_IN_POP_CH0_REG (DR_REG_AHB_DMA_BASE + 0x7c)
/** AHB_DMA_INFIFO_RDATA_CH0 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from AHB_DMA FIFO.
 */
#define AHB_DMA_INFIFO_RDATA_CH0    0x00000FFFU
#define AHB_DMA_INFIFO_RDATA_CH0_M  (AHB_DMA_INFIFO_RDATA_CH0_V << AHB_DMA_INFIFO_RDATA_CH0_S)
#define AHB_DMA_INFIFO_RDATA_CH0_V  0x00000FFFU
#define AHB_DMA_INFIFO_RDATA_CH0_S  0
/** AHB_DMA_INFIFO_POP_CH0 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from AHB_DMA FIFO.
 */
#define AHB_DMA_INFIFO_POP_CH0    (BIT(12))
#define AHB_DMA_INFIFO_POP_CH0_M  (AHB_DMA_INFIFO_POP_CH0_V << AHB_DMA_INFIFO_POP_CH0_S)
#define AHB_DMA_INFIFO_POP_CH0_V  0x00000001U
#define AHB_DMA_INFIFO_POP_CH0_S  12

/** AHB_DMA_IN_LINK_CH0_REG register
 *  Link descriptor configure and control register of Rx channel 0
 */
#define AHB_DMA_IN_LINK_CH0_REG (DR_REG_AHB_DMA_BASE + 0x80)
/** AHB_DMA_INLINK_AUTO_RET_CH0 : R/W; bitpos: [0]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define AHB_DMA_INLINK_AUTO_RET_CH0    (BIT(0))
#define AHB_DMA_INLINK_AUTO_RET_CH0_M  (AHB_DMA_INLINK_AUTO_RET_CH0_V << AHB_DMA_INLINK_AUTO_RET_CH0_S)
#define AHB_DMA_INLINK_AUTO_RET_CH0_V  0x00000001U
#define AHB_DMA_INLINK_AUTO_RET_CH0_S  0
/** AHB_DMA_INLINK_STOP_CH0 : WT; bitpos: [1]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define AHB_DMA_INLINK_STOP_CH0    (BIT(1))
#define AHB_DMA_INLINK_STOP_CH0_M  (AHB_DMA_INLINK_STOP_CH0_V << AHB_DMA_INLINK_STOP_CH0_S)
#define AHB_DMA_INLINK_STOP_CH0_V  0x00000001U
#define AHB_DMA_INLINK_STOP_CH0_S  1
/** AHB_DMA_INLINK_START_CH0 : WT; bitpos: [2]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define AHB_DMA_INLINK_START_CH0    (BIT(2))
#define AHB_DMA_INLINK_START_CH0_M  (AHB_DMA_INLINK_START_CH0_V << AHB_DMA_INLINK_START_CH0_S)
#define AHB_DMA_INLINK_START_CH0_V  0x00000001U
#define AHB_DMA_INLINK_START_CH0_S  2
/** AHB_DMA_INLINK_RESTART_CH0 : WT; bitpos: [3]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define AHB_DMA_INLINK_RESTART_CH0    (BIT(3))
#define AHB_DMA_INLINK_RESTART_CH0_M  (AHB_DMA_INLINK_RESTART_CH0_V << AHB_DMA_INLINK_RESTART_CH0_S)
#define AHB_DMA_INLINK_RESTART_CH0_V  0x00000001U
#define AHB_DMA_INLINK_RESTART_CH0_S  3
/** AHB_DMA_INLINK_PARK_CH0 : RO; bitpos: [4]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define AHB_DMA_INLINK_PARK_CH0    (BIT(4))
#define AHB_DMA_INLINK_PARK_CH0_M  (AHB_DMA_INLINK_PARK_CH0_V << AHB_DMA_INLINK_PARK_CH0_S)
#define AHB_DMA_INLINK_PARK_CH0_V  0x00000001U
#define AHB_DMA_INLINK_PARK_CH0_S  4

/** AHB_DMA_IN_STATE_CH0_REG register
 *  Receive status of Rx channel 0
 */
#define AHB_DMA_IN_STATE_CH0_REG (DR_REG_AHB_DMA_BASE + 0x84)
/** AHB_DMA_INLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define AHB_DMA_INLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define AHB_DMA_INLINK_DSCR_ADDR_CH0_M  (AHB_DMA_INLINK_DSCR_ADDR_CH0_V << AHB_DMA_INLINK_DSCR_ADDR_CH0_S)
#define AHB_DMA_INLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define AHB_DMA_INLINK_DSCR_ADDR_CH0_S  0
/** AHB_DMA_IN_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_DSCR_STATE_CH0    0x00000003U
#define AHB_DMA_IN_DSCR_STATE_CH0_M  (AHB_DMA_IN_DSCR_STATE_CH0_V << AHB_DMA_IN_DSCR_STATE_CH0_S)
#define AHB_DMA_IN_DSCR_STATE_CH0_V  0x00000003U
#define AHB_DMA_IN_DSCR_STATE_CH0_S  18
/** AHB_DMA_IN_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_STATE_CH0    0x00000007U
#define AHB_DMA_IN_STATE_CH0_M  (AHB_DMA_IN_STATE_CH0_V << AHB_DMA_IN_STATE_CH0_S)
#define AHB_DMA_IN_STATE_CH0_V  0x00000007U
#define AHB_DMA_IN_STATE_CH0_S  20

/** AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 0
 */
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x88)
/** AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0_M  (AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0_V << AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0_S)
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH0_S  0

/** AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0_REG register
 *  Inlink descriptor address when errors occur of Rx channel 0
 */
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x8c)
/** AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0_M  (AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0_V << AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0_S)
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH0_S  0

/** AHB_DMA_IN_DSCR_CH0_REG register
 *  Current inlink descriptor address of Rx channel 0
 */
#define AHB_DMA_IN_DSCR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x90)
/** AHB_DMA_INLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define AHB_DMA_INLINK_DSCR_CH0    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_CH0_M  (AHB_DMA_INLINK_DSCR_CH0_V << AHB_DMA_INLINK_DSCR_CH0_S)
#define AHB_DMA_INLINK_DSCR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_CH0_S  0

/** AHB_DMA_IN_DSCR_BF0_CH0_REG register
 *  The last inlink descriptor address of Rx channel 0
 */
#define AHB_DMA_IN_DSCR_BF0_CH0_REG (DR_REG_AHB_DMA_BASE + 0x94)
/** AHB_DMA_INLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define AHB_DMA_INLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF0_CH0_M  (AHB_DMA_INLINK_DSCR_BF0_CH0_V << AHB_DMA_INLINK_DSCR_BF0_CH0_S)
#define AHB_DMA_INLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF0_CH0_S  0

/** AHB_DMA_IN_DSCR_BF1_CH0_REG register
 *  The second-to-last inlink descriptor address of Rx channel 0
 */
#define AHB_DMA_IN_DSCR_BF1_CH0_REG (DR_REG_AHB_DMA_BASE + 0x98)
/** AHB_DMA_INLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AHB_DMA_INLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF1_CH0_M  (AHB_DMA_INLINK_DSCR_BF1_CH0_V << AHB_DMA_INLINK_DSCR_BF1_CH0_S)
#define AHB_DMA_INLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF1_CH0_S  0

/** AHB_DMA_IN_PRI_CH0_REG register
 *  Priority register of Rx channel 0
 */
#define AHB_DMA_IN_PRI_CH0_REG (DR_REG_AHB_DMA_BASE + 0x9c)
/** AHB_DMA_RX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 0. The larger of the value the higher of the priority.
 */
#define AHB_DMA_RX_PRI_CH0    0x0000000FU
#define AHB_DMA_RX_PRI_CH0_M  (AHB_DMA_RX_PRI_CH0_V << AHB_DMA_RX_PRI_CH0_S)
#define AHB_DMA_RX_PRI_CH0_V  0x0000000FU
#define AHB_DMA_RX_PRI_CH0_S  0

/** AHB_DMA_IN_PERI_SEL_CH0_REG register
 *  Peripheral selection of Rx channel 0
 */
#define AHB_DMA_IN_PERI_SEL_CH0_REG (DR_REG_AHB_DMA_BASE + 0xa0)
/** AHB_DMA_PERI_IN_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 0. I3C. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: I2S1. 5: I2S2. 6: Dummy. 7: Dummy. 8: ADC_DAC. 9: Dummy. 10:
 *  RMT,11~15: Dummy
 */
#define AHB_DMA_PERI_IN_SEL_CH0    0x0000003FU
#define AHB_DMA_PERI_IN_SEL_CH0_M  (AHB_DMA_PERI_IN_SEL_CH0_V << AHB_DMA_PERI_IN_SEL_CH0_S)
#define AHB_DMA_PERI_IN_SEL_CH0_V  0x0000003FU
#define AHB_DMA_PERI_IN_SEL_CH0_S  0

/** AHB_DMA_OUT_CONF0_CH0_REG register
 *  Configure 0 register of Tx channel 0
 */
#define AHB_DMA_OUT_CONF0_CH0_REG (DR_REG_AHB_DMA_BASE + 0xd0)
/** AHB_DMA_OUT_RST_CH0 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AHB_DMA channel 0 Tx FSM and Tx FIFO pointer.
 */
#define AHB_DMA_OUT_RST_CH0    (BIT(0))
#define AHB_DMA_OUT_RST_CH0_M  (AHB_DMA_OUT_RST_CH0_V << AHB_DMA_OUT_RST_CH0_S)
#define AHB_DMA_OUT_RST_CH0_V  0x00000001U
#define AHB_DMA_OUT_RST_CH0_S  0
/** AHB_DMA_OUT_LOOP_TEST_CH0 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_LOOP_TEST_CH0    (BIT(1))
#define AHB_DMA_OUT_LOOP_TEST_CH0_M  (AHB_DMA_OUT_LOOP_TEST_CH0_V << AHB_DMA_OUT_LOOP_TEST_CH0_S)
#define AHB_DMA_OUT_LOOP_TEST_CH0_V  0x00000001U
#define AHB_DMA_OUT_LOOP_TEST_CH0_S  1
/** AHB_DMA_OUT_AUTO_WRBACK_CH0 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define AHB_DMA_OUT_AUTO_WRBACK_CH0    (BIT(2))
#define AHB_DMA_OUT_AUTO_WRBACK_CH0_M  (AHB_DMA_OUT_AUTO_WRBACK_CH0_V << AHB_DMA_OUT_AUTO_WRBACK_CH0_S)
#define AHB_DMA_OUT_AUTO_WRBACK_CH0_V  0x00000001U
#define AHB_DMA_OUT_AUTO_WRBACK_CH0_S  2
/** AHB_DMA_OUT_EOF_MODE_CH0 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is
 *  generated when data need to transmit has been popped from FIFO in AHB_DMA
 */
#define AHB_DMA_OUT_EOF_MODE_CH0    (BIT(3))
#define AHB_DMA_OUT_EOF_MODE_CH0_M  (AHB_DMA_OUT_EOF_MODE_CH0_V << AHB_DMA_OUT_EOF_MODE_CH0_S)
#define AHB_DMA_OUT_EOF_MODE_CH0_V  0x00000001U
#define AHB_DMA_OUT_EOF_MODE_CH0_S  3
/** AHB_DMA_OUTDSCR_BURST_EN_CH0 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AHB_DMA_OUTDSCR_BURST_EN_CH0    (BIT(4))
#define AHB_DMA_OUTDSCR_BURST_EN_CH0_M  (AHB_DMA_OUTDSCR_BURST_EN_CH0_V << AHB_DMA_OUTDSCR_BURST_EN_CH0_S)
#define AHB_DMA_OUTDSCR_BURST_EN_CH0_V  0x00000001U
#define AHB_DMA_OUTDSCR_BURST_EN_CH0_S  4
/** AHB_DMA_OUT_DATA_BURST_EN_CH0 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 transmitting data
 *  when accessing internal SRAM.
 */
#define AHB_DMA_OUT_DATA_BURST_EN_CH0    (BIT(5))
#define AHB_DMA_OUT_DATA_BURST_EN_CH0_M  (AHB_DMA_OUT_DATA_BURST_EN_CH0_V << AHB_DMA_OUT_DATA_BURST_EN_CH0_S)
#define AHB_DMA_OUT_DATA_BURST_EN_CH0_V  0x00000001U
#define AHB_DMA_OUT_DATA_BURST_EN_CH0_S  5
/** AHB_DMA_OUT_ETM_EN_CH0 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel 0 is triggered by etm
 *  task.
 */
#define AHB_DMA_OUT_ETM_EN_CH0    (BIT(6))
#define AHB_DMA_OUT_ETM_EN_CH0_M  (AHB_DMA_OUT_ETM_EN_CH0_V << AHB_DMA_OUT_ETM_EN_CH0_S)
#define AHB_DMA_OUT_ETM_EN_CH0_V  0x00000001U
#define AHB_DMA_OUT_ETM_EN_CH0_S  6

/** AHB_DMA_OUT_CONF1_CH0_REG register
 *  Configure 1 register of Tx channel 0
 */
#define AHB_DMA_OUT_CONF1_CH0_REG (DR_REG_AHB_DMA_BASE + 0xd4)
/** AHB_DMA_OUT_CHECK_OWNER_CH0 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AHB_DMA_OUT_CHECK_OWNER_CH0    (BIT(12))
#define AHB_DMA_OUT_CHECK_OWNER_CH0_M  (AHB_DMA_OUT_CHECK_OWNER_CH0_V << AHB_DMA_OUT_CHECK_OWNER_CH0_S)
#define AHB_DMA_OUT_CHECK_OWNER_CH0_V  0x00000001U
#define AHB_DMA_OUT_CHECK_OWNER_CH0_S  12

/** AHB_DMA_OUTFIFO_STATUS_CH0_REG register
 *  Transmit FIFO status of Tx channel 0
 */
#define AHB_DMA_OUTFIFO_STATUS_CH0_REG (DR_REG_AHB_DMA_BASE + 0xd8)
/** AHB_DMA_OUTFIFO_FULL_CH0 : RO; bitpos: [0]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel 0.
 */
#define AHB_DMA_OUTFIFO_FULL_CH0    (BIT(0))
#define AHB_DMA_OUTFIFO_FULL_CH0_M  (AHB_DMA_OUTFIFO_FULL_CH0_V << AHB_DMA_OUTFIFO_FULL_CH0_S)
#define AHB_DMA_OUTFIFO_FULL_CH0_V  0x00000001U
#define AHB_DMA_OUTFIFO_FULL_CH0_S  0
/** AHB_DMA_OUTFIFO_EMPTY_CH0 : RO; bitpos: [1]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel 0.
 */
#define AHB_DMA_OUTFIFO_EMPTY_CH0    (BIT(1))
#define AHB_DMA_OUTFIFO_EMPTY_CH0_M  (AHB_DMA_OUTFIFO_EMPTY_CH0_V << AHB_DMA_OUTFIFO_EMPTY_CH0_S)
#define AHB_DMA_OUTFIFO_EMPTY_CH0_V  0x00000001U
#define AHB_DMA_OUTFIFO_EMPTY_CH0_S  1
/** AHB_DMA_OUTFIFO_CNT_CH0 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.
 */
#define AHB_DMA_OUTFIFO_CNT_CH0    0x0000003FU
#define AHB_DMA_OUTFIFO_CNT_CH0_M  (AHB_DMA_OUTFIFO_CNT_CH0_V << AHB_DMA_OUTFIFO_CNT_CH0_S)
#define AHB_DMA_OUTFIFO_CNT_CH0_V  0x0000003FU
#define AHB_DMA_OUTFIFO_CNT_CH0_S  2
/** AHB_DMA_OUT_REMAIN_UNDER_1B_CH0 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH0    (BIT(23))
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH0_M  (AHB_DMA_OUT_REMAIN_UNDER_1B_CH0_V << AHB_DMA_OUT_REMAIN_UNDER_1B_CH0_S)
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH0_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH0_S  23
/** AHB_DMA_OUT_REMAIN_UNDER_2B_CH0 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH0    (BIT(24))
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH0_M  (AHB_DMA_OUT_REMAIN_UNDER_2B_CH0_V << AHB_DMA_OUT_REMAIN_UNDER_2B_CH0_S)
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH0_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH0_S  24
/** AHB_DMA_OUT_REMAIN_UNDER_3B_CH0 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH0    (BIT(25))
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH0_M  (AHB_DMA_OUT_REMAIN_UNDER_3B_CH0_V << AHB_DMA_OUT_REMAIN_UNDER_3B_CH0_S)
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH0_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH0_S  25
/** AHB_DMA_OUT_REMAIN_UNDER_4B_CH0 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH0    (BIT(26))
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH0_M  (AHB_DMA_OUT_REMAIN_UNDER_4B_CH0_V << AHB_DMA_OUT_REMAIN_UNDER_4B_CH0_S)
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH0_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH0_S  26

/** AHB_DMA_OUT_PUSH_CH0_REG register
 *  Push control register of Rx channel 0
 */
#define AHB_DMA_OUT_PUSH_CH0_REG (DR_REG_AHB_DMA_BASE + 0xdc)
/** AHB_DMA_OUTFIFO_WDATA_CH0 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into AHB_DMA FIFO.
 */
#define AHB_DMA_OUTFIFO_WDATA_CH0    0x000001FFU
#define AHB_DMA_OUTFIFO_WDATA_CH0_M  (AHB_DMA_OUTFIFO_WDATA_CH0_V << AHB_DMA_OUTFIFO_WDATA_CH0_S)
#define AHB_DMA_OUTFIFO_WDATA_CH0_V  0x000001FFU
#define AHB_DMA_OUTFIFO_WDATA_CH0_S  0
/** AHB_DMA_OUTFIFO_PUSH_CH0 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into AHB_DMA FIFO.
 */
#define AHB_DMA_OUTFIFO_PUSH_CH0    (BIT(9))
#define AHB_DMA_OUTFIFO_PUSH_CH0_M  (AHB_DMA_OUTFIFO_PUSH_CH0_V << AHB_DMA_OUTFIFO_PUSH_CH0_S)
#define AHB_DMA_OUTFIFO_PUSH_CH0_V  0x00000001U
#define AHB_DMA_OUTFIFO_PUSH_CH0_S  9

/** AHB_DMA_OUT_LINK_CH0_REG register
 *  Link descriptor configure and control register of Tx channel 0
 */
#define AHB_DMA_OUT_LINK_CH0_REG (DR_REG_AHB_DMA_BASE + 0xe0)
/** AHB_DMA_OUTLINK_STOP_CH0 : WT; bitpos: [0]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define AHB_DMA_OUTLINK_STOP_CH0    (BIT(0))
#define AHB_DMA_OUTLINK_STOP_CH0_M  (AHB_DMA_OUTLINK_STOP_CH0_V << AHB_DMA_OUTLINK_STOP_CH0_S)
#define AHB_DMA_OUTLINK_STOP_CH0_V  0x00000001U
#define AHB_DMA_OUTLINK_STOP_CH0_S  0
/** AHB_DMA_OUTLINK_START_CH0 : WT; bitpos: [1]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define AHB_DMA_OUTLINK_START_CH0    (BIT(1))
#define AHB_DMA_OUTLINK_START_CH0_M  (AHB_DMA_OUTLINK_START_CH0_V << AHB_DMA_OUTLINK_START_CH0_S)
#define AHB_DMA_OUTLINK_START_CH0_V  0x00000001U
#define AHB_DMA_OUTLINK_START_CH0_S  1
/** AHB_DMA_OUTLINK_RESTART_CH0 : WT; bitpos: [2]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define AHB_DMA_OUTLINK_RESTART_CH0    (BIT(2))
#define AHB_DMA_OUTLINK_RESTART_CH0_M  (AHB_DMA_OUTLINK_RESTART_CH0_V << AHB_DMA_OUTLINK_RESTART_CH0_S)
#define AHB_DMA_OUTLINK_RESTART_CH0_V  0x00000001U
#define AHB_DMA_OUTLINK_RESTART_CH0_S  2
/** AHB_DMA_OUTLINK_PARK_CH0 : RO; bitpos: [3]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define AHB_DMA_OUTLINK_PARK_CH0    (BIT(3))
#define AHB_DMA_OUTLINK_PARK_CH0_M  (AHB_DMA_OUTLINK_PARK_CH0_V << AHB_DMA_OUTLINK_PARK_CH0_S)
#define AHB_DMA_OUTLINK_PARK_CH0_V  0x00000001U
#define AHB_DMA_OUTLINK_PARK_CH0_S  3

/** AHB_DMA_OUT_STATE_CH0_REG register
 *  Transmit status of Tx channel 0
 */
#define AHB_DMA_OUT_STATE_CH0_REG (DR_REG_AHB_DMA_BASE + 0xe4)
/** AHB_DMA_OUTLINK_DSCR_ADDR_CH0 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH0    0x0003FFFFU
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH0_M  (AHB_DMA_OUTLINK_DSCR_ADDR_CH0_V << AHB_DMA_OUTLINK_DSCR_ADDR_CH0_S)
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH0_V  0x0003FFFFU
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH0_S  0
/** AHB_DMA_OUT_DSCR_STATE_CH0 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_DSCR_STATE_CH0    0x00000003U
#define AHB_DMA_OUT_DSCR_STATE_CH0_M  (AHB_DMA_OUT_DSCR_STATE_CH0_V << AHB_DMA_OUT_DSCR_STATE_CH0_S)
#define AHB_DMA_OUT_DSCR_STATE_CH0_V  0x00000003U
#define AHB_DMA_OUT_DSCR_STATE_CH0_S  18
/** AHB_DMA_OUT_STATE_CH0 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_STATE_CH0    0x00000007U
#define AHB_DMA_OUT_STATE_CH0_M  (AHB_DMA_OUT_STATE_CH0_V << AHB_DMA_OUT_STATE_CH0_S)
#define AHB_DMA_OUT_STATE_CH0_V  0x00000007U
#define AHB_DMA_OUT_STATE_CH0_S  20

/** AHB_DMA_OUT_EOF_DES_ADDR_CH0_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel 0
 */
#define AHB_DMA_OUT_EOF_DES_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0xe8)
/** AHB_DMA_OUT_EOF_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AHB_DMA_OUT_EOF_DES_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_DES_ADDR_CH0_M  (AHB_DMA_OUT_EOF_DES_ADDR_CH0_V << AHB_DMA_OUT_EOF_DES_ADDR_CH0_S)
#define AHB_DMA_OUT_EOF_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_DES_ADDR_CH0_S  0

/** AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel 0
 */
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0xec)
/** AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0_M  (AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0_V << AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0_S)
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH0_S  0

/** AHB_DMA_OUT_DSCR_CH0_REG register
 *  Current inlink descriptor address of Tx channel 0
 */
#define AHB_DMA_OUT_DSCR_CH0_REG (DR_REG_AHB_DMA_BASE + 0xf0)
/** AHB_DMA_OUTLINK_DSCR_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define AHB_DMA_OUTLINK_DSCR_CH0    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_CH0_M  (AHB_DMA_OUTLINK_DSCR_CH0_V << AHB_DMA_OUTLINK_DSCR_CH0_S)
#define AHB_DMA_OUTLINK_DSCR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_CH0_S  0

/** AHB_DMA_OUT_DSCR_BF0_CH0_REG register
 *  The last inlink descriptor address of Tx channel 0
 */
#define AHB_DMA_OUT_DSCR_BF0_CH0_REG (DR_REG_AHB_DMA_BASE + 0xf4)
/** AHB_DMA_OUTLINK_DSCR_BF0_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define AHB_DMA_OUTLINK_DSCR_BF0_CH0    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF0_CH0_M  (AHB_DMA_OUTLINK_DSCR_BF0_CH0_V << AHB_DMA_OUTLINK_DSCR_BF0_CH0_S)
#define AHB_DMA_OUTLINK_DSCR_BF0_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF0_CH0_S  0

/** AHB_DMA_OUT_DSCR_BF1_CH0_REG register
 *  The second-to-last inlink descriptor address of Tx channel 0
 */
#define AHB_DMA_OUT_DSCR_BF1_CH0_REG (DR_REG_AHB_DMA_BASE + 0xf8)
/** AHB_DMA_OUTLINK_DSCR_BF1_CH0 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AHB_DMA_OUTLINK_DSCR_BF1_CH0    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF1_CH0_M  (AHB_DMA_OUTLINK_DSCR_BF1_CH0_V << AHB_DMA_OUTLINK_DSCR_BF1_CH0_S)
#define AHB_DMA_OUTLINK_DSCR_BF1_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF1_CH0_S  0

/** AHB_DMA_OUT_PRI_CH0_REG register
 *  Priority register of Tx channel 0.
 */
#define AHB_DMA_OUT_PRI_CH0_REG (DR_REG_AHB_DMA_BASE + 0xfc)
/** AHB_DMA_TX_PRI_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel 0. The larger of the value the higher of the priority.
 */
#define AHB_DMA_TX_PRI_CH0    0x0000000FU
#define AHB_DMA_TX_PRI_CH0_M  (AHB_DMA_TX_PRI_CH0_V << AHB_DMA_TX_PRI_CH0_S)
#define AHB_DMA_TX_PRI_CH0_V  0x0000000FU
#define AHB_DMA_TX_PRI_CH0_S  0

/** AHB_DMA_OUT_PERI_SEL_CH0_REG register
 *  Peripheral selection of Tx channel 0
 */
#define AHB_DMA_OUT_PERI_SEL_CH0_REG (DR_REG_AHB_DMA_BASE + 0x100)
/** AHB_DMA_PERI_OUT_SEL_CH0 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel 0. I3C. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: I2S1. 5: I2S2. 6: Dummy. 7: Dummy. 8: ADC_DAC. 9: Dummy. 10:
 *  RMT,11~15: Dummy
 */
#define AHB_DMA_PERI_OUT_SEL_CH0    0x0000003FU
#define AHB_DMA_PERI_OUT_SEL_CH0_M  (AHB_DMA_PERI_OUT_SEL_CH0_V << AHB_DMA_PERI_OUT_SEL_CH0_S)
#define AHB_DMA_PERI_OUT_SEL_CH0_V  0x0000003FU
#define AHB_DMA_PERI_OUT_SEL_CH0_S  0

/** AHB_DMA_IN_CONF0_CH1_REG register
 *  Configure 0 register of Rx channel 1
 */
#define AHB_DMA_IN_CONF0_CH1_REG (DR_REG_AHB_DMA_BASE + 0x130)
/** AHB_DMA_IN_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AHB_DMA channel 1 Rx FSM and Rx FIFO pointer.
 */
#define AHB_DMA_IN_RST_CH1    (BIT(0))
#define AHB_DMA_IN_RST_CH1_M  (AHB_DMA_IN_RST_CH1_V << AHB_DMA_IN_RST_CH1_S)
#define AHB_DMA_IN_RST_CH1_V  0x00000001U
#define AHB_DMA_IN_RST_CH1_S  0
/** AHB_DMA_IN_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_LOOP_TEST_CH1    (BIT(1))
#define AHB_DMA_IN_LOOP_TEST_CH1_M  (AHB_DMA_IN_LOOP_TEST_CH1_V << AHB_DMA_IN_LOOP_TEST_CH1_S)
#define AHB_DMA_IN_LOOP_TEST_CH1_V  0x00000001U
#define AHB_DMA_IN_LOOP_TEST_CH1_S  1
/** AHB_DMA_INDSCR_BURST_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 1 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AHB_DMA_INDSCR_BURST_EN_CH1    (BIT(2))
#define AHB_DMA_INDSCR_BURST_EN_CH1_M  (AHB_DMA_INDSCR_BURST_EN_CH1_V << AHB_DMA_INDSCR_BURST_EN_CH1_S)
#define AHB_DMA_INDSCR_BURST_EN_CH1_V  0x00000001U
#define AHB_DMA_INDSCR_BURST_EN_CH1_S  2
/** AHB_DMA_IN_DATA_BURST_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 1 receiving data
 *  when accessing internal SRAM.
 */
#define AHB_DMA_IN_DATA_BURST_EN_CH1    (BIT(3))
#define AHB_DMA_IN_DATA_BURST_EN_CH1_M  (AHB_DMA_IN_DATA_BURST_EN_CH1_V << AHB_DMA_IN_DATA_BURST_EN_CH1_S)
#define AHB_DMA_IN_DATA_BURST_EN_CH1_V  0x00000001U
#define AHB_DMA_IN_DATA_BURST_EN_CH1_S  3
/** AHB_DMA_MEM_TRANS_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via
 *  AHB_DMA.
 */
#define AHB_DMA_MEM_TRANS_EN_CH1    (BIT(4))
#define AHB_DMA_MEM_TRANS_EN_CH1_M  (AHB_DMA_MEM_TRANS_EN_CH1_V << AHB_DMA_MEM_TRANS_EN_CH1_S)
#define AHB_DMA_MEM_TRANS_EN_CH1_V  0x00000001U
#define AHB_DMA_MEM_TRANS_EN_CH1_S  4
/** AHB_DMA_IN_ETM_EN_CH1 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 1 is triggered by etm
 *  task.
 */
#define AHB_DMA_IN_ETM_EN_CH1    (BIT(5))
#define AHB_DMA_IN_ETM_EN_CH1_M  (AHB_DMA_IN_ETM_EN_CH1_V << AHB_DMA_IN_ETM_EN_CH1_S)
#define AHB_DMA_IN_ETM_EN_CH1_V  0x00000001U
#define AHB_DMA_IN_ETM_EN_CH1_S  5

/** AHB_DMA_IN_CONF1_CH1_REG register
 *  Configure 1 register of Rx channel 1
 */
#define AHB_DMA_IN_CONF1_CH1_REG (DR_REG_AHB_DMA_BASE + 0x134)
/** AHB_DMA_IN_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AHB_DMA_IN_CHECK_OWNER_CH1    (BIT(12))
#define AHB_DMA_IN_CHECK_OWNER_CH1_M  (AHB_DMA_IN_CHECK_OWNER_CH1_V << AHB_DMA_IN_CHECK_OWNER_CH1_S)
#define AHB_DMA_IN_CHECK_OWNER_CH1_V  0x00000001U
#define AHB_DMA_IN_CHECK_OWNER_CH1_S  12

/** AHB_DMA_INFIFO_STATUS_CH1_REG register
 *  Receive FIFO status of Rx channel 1
 */
#define AHB_DMA_INFIFO_STATUS_CH1_REG (DR_REG_AHB_DMA_BASE + 0x138)
/** AHB_DMA_INFIFO_FULL_CH1 : RO; bitpos: [0]; default: 1;
 *  L1 Rx FIFO full signal for Rx channel 1.
 */
#define AHB_DMA_INFIFO_FULL_CH1    (BIT(0))
#define AHB_DMA_INFIFO_FULL_CH1_M  (AHB_DMA_INFIFO_FULL_CH1_V << AHB_DMA_INFIFO_FULL_CH1_S)
#define AHB_DMA_INFIFO_FULL_CH1_V  0x00000001U
#define AHB_DMA_INFIFO_FULL_CH1_S  0
/** AHB_DMA_INFIFO_EMPTY_CH1 : RO; bitpos: [1]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 1.
 */
#define AHB_DMA_INFIFO_EMPTY_CH1    (BIT(1))
#define AHB_DMA_INFIFO_EMPTY_CH1_M  (AHB_DMA_INFIFO_EMPTY_CH1_V << AHB_DMA_INFIFO_EMPTY_CH1_S)
#define AHB_DMA_INFIFO_EMPTY_CH1_V  0x00000001U
#define AHB_DMA_INFIFO_EMPTY_CH1_S  1
/** AHB_DMA_INFIFO_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 1.
 */
#define AHB_DMA_INFIFO_CNT_CH1    0x0000003FU
#define AHB_DMA_INFIFO_CNT_CH1_M  (AHB_DMA_INFIFO_CNT_CH1_V << AHB_DMA_INFIFO_CNT_CH1_S)
#define AHB_DMA_INFIFO_CNT_CH1_V  0x0000003FU
#define AHB_DMA_INFIFO_CNT_CH1_S  2
/** AHB_DMA_IN_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH1    (BIT(23))
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH1_M  (AHB_DMA_IN_REMAIN_UNDER_1B_CH1_V << AHB_DMA_IN_REMAIN_UNDER_1B_CH1_S)
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH1_S  23
/** AHB_DMA_IN_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH1    (BIT(24))
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH1_M  (AHB_DMA_IN_REMAIN_UNDER_2B_CH1_V << AHB_DMA_IN_REMAIN_UNDER_2B_CH1_S)
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH1_S  24
/** AHB_DMA_IN_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH1    (BIT(25))
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH1_M  (AHB_DMA_IN_REMAIN_UNDER_3B_CH1_V << AHB_DMA_IN_REMAIN_UNDER_3B_CH1_S)
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH1_S  25
/** AHB_DMA_IN_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH1    (BIT(26))
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH1_M  (AHB_DMA_IN_REMAIN_UNDER_4B_CH1_V << AHB_DMA_IN_REMAIN_UNDER_4B_CH1_S)
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH1_S  26
/** AHB_DMA_IN_BUF_HUNGRY_CH1 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_BUF_HUNGRY_CH1    (BIT(27))
#define AHB_DMA_IN_BUF_HUNGRY_CH1_M  (AHB_DMA_IN_BUF_HUNGRY_CH1_V << AHB_DMA_IN_BUF_HUNGRY_CH1_S)
#define AHB_DMA_IN_BUF_HUNGRY_CH1_V  0x00000001U
#define AHB_DMA_IN_BUF_HUNGRY_CH1_S  27

/** AHB_DMA_IN_POP_CH1_REG register
 *  Pop control register of Rx channel 1
 */
#define AHB_DMA_IN_POP_CH1_REG (DR_REG_AHB_DMA_BASE + 0x13c)
/** AHB_DMA_INFIFO_RDATA_CH1 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from AHB_DMA FIFO.
 */
#define AHB_DMA_INFIFO_RDATA_CH1    0x00000FFFU
#define AHB_DMA_INFIFO_RDATA_CH1_M  (AHB_DMA_INFIFO_RDATA_CH1_V << AHB_DMA_INFIFO_RDATA_CH1_S)
#define AHB_DMA_INFIFO_RDATA_CH1_V  0x00000FFFU
#define AHB_DMA_INFIFO_RDATA_CH1_S  0
/** AHB_DMA_INFIFO_POP_CH1 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from AHB_DMA FIFO.
 */
#define AHB_DMA_INFIFO_POP_CH1    (BIT(12))
#define AHB_DMA_INFIFO_POP_CH1_M  (AHB_DMA_INFIFO_POP_CH1_V << AHB_DMA_INFIFO_POP_CH1_S)
#define AHB_DMA_INFIFO_POP_CH1_V  0x00000001U
#define AHB_DMA_INFIFO_POP_CH1_S  12

/** AHB_DMA_IN_LINK_CH1_REG register
 *  Link descriptor configure and control register of Rx channel 1
 */
#define AHB_DMA_IN_LINK_CH1_REG (DR_REG_AHB_DMA_BASE + 0x140)
/** AHB_DMA_INLINK_AUTO_RET_CH1 : R/W; bitpos: [0]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define AHB_DMA_INLINK_AUTO_RET_CH1    (BIT(0))
#define AHB_DMA_INLINK_AUTO_RET_CH1_M  (AHB_DMA_INLINK_AUTO_RET_CH1_V << AHB_DMA_INLINK_AUTO_RET_CH1_S)
#define AHB_DMA_INLINK_AUTO_RET_CH1_V  0x00000001U
#define AHB_DMA_INLINK_AUTO_RET_CH1_S  0
/** AHB_DMA_INLINK_STOP_CH1 : WT; bitpos: [1]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define AHB_DMA_INLINK_STOP_CH1    (BIT(1))
#define AHB_DMA_INLINK_STOP_CH1_M  (AHB_DMA_INLINK_STOP_CH1_V << AHB_DMA_INLINK_STOP_CH1_S)
#define AHB_DMA_INLINK_STOP_CH1_V  0x00000001U
#define AHB_DMA_INLINK_STOP_CH1_S  1
/** AHB_DMA_INLINK_START_CH1 : WT; bitpos: [2]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define AHB_DMA_INLINK_START_CH1    (BIT(2))
#define AHB_DMA_INLINK_START_CH1_M  (AHB_DMA_INLINK_START_CH1_V << AHB_DMA_INLINK_START_CH1_S)
#define AHB_DMA_INLINK_START_CH1_V  0x00000001U
#define AHB_DMA_INLINK_START_CH1_S  2
/** AHB_DMA_INLINK_RESTART_CH1 : WT; bitpos: [3]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define AHB_DMA_INLINK_RESTART_CH1    (BIT(3))
#define AHB_DMA_INLINK_RESTART_CH1_M  (AHB_DMA_INLINK_RESTART_CH1_V << AHB_DMA_INLINK_RESTART_CH1_S)
#define AHB_DMA_INLINK_RESTART_CH1_V  0x00000001U
#define AHB_DMA_INLINK_RESTART_CH1_S  3
/** AHB_DMA_INLINK_PARK_CH1 : RO; bitpos: [4]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define AHB_DMA_INLINK_PARK_CH1    (BIT(4))
#define AHB_DMA_INLINK_PARK_CH1_M  (AHB_DMA_INLINK_PARK_CH1_V << AHB_DMA_INLINK_PARK_CH1_S)
#define AHB_DMA_INLINK_PARK_CH1_V  0x00000001U
#define AHB_DMA_INLINK_PARK_CH1_S  4

/** AHB_DMA_IN_STATE_CH1_REG register
 *  Receive status of Rx channel 1
 */
#define AHB_DMA_IN_STATE_CH1_REG (DR_REG_AHB_DMA_BASE + 0x144)
/** AHB_DMA_INLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define AHB_DMA_INLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define AHB_DMA_INLINK_DSCR_ADDR_CH1_M  (AHB_DMA_INLINK_DSCR_ADDR_CH1_V << AHB_DMA_INLINK_DSCR_ADDR_CH1_S)
#define AHB_DMA_INLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define AHB_DMA_INLINK_DSCR_ADDR_CH1_S  0
/** AHB_DMA_IN_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_DSCR_STATE_CH1    0x00000003U
#define AHB_DMA_IN_DSCR_STATE_CH1_M  (AHB_DMA_IN_DSCR_STATE_CH1_V << AHB_DMA_IN_DSCR_STATE_CH1_S)
#define AHB_DMA_IN_DSCR_STATE_CH1_V  0x00000003U
#define AHB_DMA_IN_DSCR_STATE_CH1_S  18
/** AHB_DMA_IN_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_STATE_CH1    0x00000007U
#define AHB_DMA_IN_STATE_CH1_M  (AHB_DMA_IN_STATE_CH1_V << AHB_DMA_IN_STATE_CH1_S)
#define AHB_DMA_IN_STATE_CH1_V  0x00000007U
#define AHB_DMA_IN_STATE_CH1_S  20

/** AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 1
 */
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x148)
/** AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1_M  (AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1_V << AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1_S)
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH1_S  0

/** AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1_REG register
 *  Inlink descriptor address when errors occur of Rx channel 1
 */
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x14c)
/** AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1_M  (AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1_V << AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1_S)
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH1_S  0

/** AHB_DMA_IN_DSCR_CH1_REG register
 *  Current inlink descriptor address of Rx channel 1
 */
#define AHB_DMA_IN_DSCR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x150)
/** AHB_DMA_INLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define AHB_DMA_INLINK_DSCR_CH1    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_CH1_M  (AHB_DMA_INLINK_DSCR_CH1_V << AHB_DMA_INLINK_DSCR_CH1_S)
#define AHB_DMA_INLINK_DSCR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_CH1_S  0

/** AHB_DMA_IN_DSCR_BF0_CH1_REG register
 *  The last inlink descriptor address of Rx channel 1
 */
#define AHB_DMA_IN_DSCR_BF0_CH1_REG (DR_REG_AHB_DMA_BASE + 0x154)
/** AHB_DMA_INLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define AHB_DMA_INLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF0_CH1_M  (AHB_DMA_INLINK_DSCR_BF0_CH1_V << AHB_DMA_INLINK_DSCR_BF0_CH1_S)
#define AHB_DMA_INLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF0_CH1_S  0

/** AHB_DMA_IN_DSCR_BF1_CH1_REG register
 *  The second-to-last inlink descriptor address of Rx channel 1
 */
#define AHB_DMA_IN_DSCR_BF1_CH1_REG (DR_REG_AHB_DMA_BASE + 0x158)
/** AHB_DMA_INLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AHB_DMA_INLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF1_CH1_M  (AHB_DMA_INLINK_DSCR_BF1_CH1_V << AHB_DMA_INLINK_DSCR_BF1_CH1_S)
#define AHB_DMA_INLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF1_CH1_S  0

/** AHB_DMA_IN_PRI_CH1_REG register
 *  Priority register of Rx channel 1
 */
#define AHB_DMA_IN_PRI_CH1_REG (DR_REG_AHB_DMA_BASE + 0x15c)
/** AHB_DMA_RX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 1. The larger of the value the higher of the priority.
 */
#define AHB_DMA_RX_PRI_CH1    0x0000000FU
#define AHB_DMA_RX_PRI_CH1_M  (AHB_DMA_RX_PRI_CH1_V << AHB_DMA_RX_PRI_CH1_S)
#define AHB_DMA_RX_PRI_CH1_V  0x0000000FU
#define AHB_DMA_RX_PRI_CH1_S  0

/** AHB_DMA_IN_PERI_SEL_CH1_REG register
 *  Peripheral selection of Rx channel 1
 */
#define AHB_DMA_IN_PERI_SEL_CH1_REG (DR_REG_AHB_DMA_BASE + 0x160)
/** AHB_DMA_PERI_IN_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 0. I3C. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: I2S1. 5: I2S2. 6: Dummy. 7: Dummy. 8: ADC_DAC. 9: Dummy. 10:
 *  RMT,11~15: Dummy
 */
#define AHB_DMA_PERI_IN_SEL_CH1    0x0000003FU
#define AHB_DMA_PERI_IN_SEL_CH1_M  (AHB_DMA_PERI_IN_SEL_CH1_V << AHB_DMA_PERI_IN_SEL_CH1_S)
#define AHB_DMA_PERI_IN_SEL_CH1_V  0x0000003FU
#define AHB_DMA_PERI_IN_SEL_CH1_S  0

/** AHB_DMA_OUT_CONF0_CH1_REG register
 *  Configure 0 register of Tx channel 1
 */
#define AHB_DMA_OUT_CONF0_CH1_REG (DR_REG_AHB_DMA_BASE + 0x190)
/** AHB_DMA_OUT_RST_CH1 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AHB_DMA channel 1 Tx FSM and Tx FIFO pointer.
 */
#define AHB_DMA_OUT_RST_CH1    (BIT(0))
#define AHB_DMA_OUT_RST_CH1_M  (AHB_DMA_OUT_RST_CH1_V << AHB_DMA_OUT_RST_CH1_S)
#define AHB_DMA_OUT_RST_CH1_V  0x00000001U
#define AHB_DMA_OUT_RST_CH1_S  0
/** AHB_DMA_OUT_LOOP_TEST_CH1 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_LOOP_TEST_CH1    (BIT(1))
#define AHB_DMA_OUT_LOOP_TEST_CH1_M  (AHB_DMA_OUT_LOOP_TEST_CH1_V << AHB_DMA_OUT_LOOP_TEST_CH1_S)
#define AHB_DMA_OUT_LOOP_TEST_CH1_V  0x00000001U
#define AHB_DMA_OUT_LOOP_TEST_CH1_S  1
/** AHB_DMA_OUT_AUTO_WRBACK_CH1 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define AHB_DMA_OUT_AUTO_WRBACK_CH1    (BIT(2))
#define AHB_DMA_OUT_AUTO_WRBACK_CH1_M  (AHB_DMA_OUT_AUTO_WRBACK_CH1_V << AHB_DMA_OUT_AUTO_WRBACK_CH1_S)
#define AHB_DMA_OUT_AUTO_WRBACK_CH1_V  0x00000001U
#define AHB_DMA_OUT_AUTO_WRBACK_CH1_S  2
/** AHB_DMA_OUT_EOF_MODE_CH1 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 1 is
 *  generated when data need to transmit has been popped from FIFO in AHB_DMA
 */
#define AHB_DMA_OUT_EOF_MODE_CH1    (BIT(3))
#define AHB_DMA_OUT_EOF_MODE_CH1_M  (AHB_DMA_OUT_EOF_MODE_CH1_V << AHB_DMA_OUT_EOF_MODE_CH1_S)
#define AHB_DMA_OUT_EOF_MODE_CH1_V  0x00000001U
#define AHB_DMA_OUT_EOF_MODE_CH1_S  3
/** AHB_DMA_OUTDSCR_BURST_EN_CH1 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 1 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AHB_DMA_OUTDSCR_BURST_EN_CH1    (BIT(4))
#define AHB_DMA_OUTDSCR_BURST_EN_CH1_M  (AHB_DMA_OUTDSCR_BURST_EN_CH1_V << AHB_DMA_OUTDSCR_BURST_EN_CH1_S)
#define AHB_DMA_OUTDSCR_BURST_EN_CH1_V  0x00000001U
#define AHB_DMA_OUTDSCR_BURST_EN_CH1_S  4
/** AHB_DMA_OUT_DATA_BURST_EN_CH1 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 1 transmitting data
 *  when accessing internal SRAM.
 */
#define AHB_DMA_OUT_DATA_BURST_EN_CH1    (BIT(5))
#define AHB_DMA_OUT_DATA_BURST_EN_CH1_M  (AHB_DMA_OUT_DATA_BURST_EN_CH1_V << AHB_DMA_OUT_DATA_BURST_EN_CH1_S)
#define AHB_DMA_OUT_DATA_BURST_EN_CH1_V  0x00000001U
#define AHB_DMA_OUT_DATA_BURST_EN_CH1_S  5
/** AHB_DMA_OUT_ETM_EN_CH1 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel 1 is triggered by etm
 *  task.
 */
#define AHB_DMA_OUT_ETM_EN_CH1    (BIT(6))
#define AHB_DMA_OUT_ETM_EN_CH1_M  (AHB_DMA_OUT_ETM_EN_CH1_V << AHB_DMA_OUT_ETM_EN_CH1_S)
#define AHB_DMA_OUT_ETM_EN_CH1_V  0x00000001U
#define AHB_DMA_OUT_ETM_EN_CH1_S  6

/** AHB_DMA_OUT_CONF1_CH1_REG register
 *  Configure 1 register of Tx channel 1
 */
#define AHB_DMA_OUT_CONF1_CH1_REG (DR_REG_AHB_DMA_BASE + 0x194)
/** AHB_DMA_OUT_CHECK_OWNER_CH1 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AHB_DMA_OUT_CHECK_OWNER_CH1    (BIT(12))
#define AHB_DMA_OUT_CHECK_OWNER_CH1_M  (AHB_DMA_OUT_CHECK_OWNER_CH1_V << AHB_DMA_OUT_CHECK_OWNER_CH1_S)
#define AHB_DMA_OUT_CHECK_OWNER_CH1_V  0x00000001U
#define AHB_DMA_OUT_CHECK_OWNER_CH1_S  12

/** AHB_DMA_OUTFIFO_STATUS_CH1_REG register
 *  Transmit FIFO status of Tx channel 1
 */
#define AHB_DMA_OUTFIFO_STATUS_CH1_REG (DR_REG_AHB_DMA_BASE + 0x198)
/** AHB_DMA_OUTFIFO_FULL_CH1 : RO; bitpos: [0]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel 1.
 */
#define AHB_DMA_OUTFIFO_FULL_CH1    (BIT(0))
#define AHB_DMA_OUTFIFO_FULL_CH1_M  (AHB_DMA_OUTFIFO_FULL_CH1_V << AHB_DMA_OUTFIFO_FULL_CH1_S)
#define AHB_DMA_OUTFIFO_FULL_CH1_V  0x00000001U
#define AHB_DMA_OUTFIFO_FULL_CH1_S  0
/** AHB_DMA_OUTFIFO_EMPTY_CH1 : RO; bitpos: [1]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel 1.
 */
#define AHB_DMA_OUTFIFO_EMPTY_CH1    (BIT(1))
#define AHB_DMA_OUTFIFO_EMPTY_CH1_M  (AHB_DMA_OUTFIFO_EMPTY_CH1_V << AHB_DMA_OUTFIFO_EMPTY_CH1_S)
#define AHB_DMA_OUTFIFO_EMPTY_CH1_V  0x00000001U
#define AHB_DMA_OUTFIFO_EMPTY_CH1_S  1
/** AHB_DMA_OUTFIFO_CNT_CH1 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 1.
 */
#define AHB_DMA_OUTFIFO_CNT_CH1    0x0000003FU
#define AHB_DMA_OUTFIFO_CNT_CH1_M  (AHB_DMA_OUTFIFO_CNT_CH1_V << AHB_DMA_OUTFIFO_CNT_CH1_S)
#define AHB_DMA_OUTFIFO_CNT_CH1_V  0x0000003FU
#define AHB_DMA_OUTFIFO_CNT_CH1_S  2
/** AHB_DMA_OUT_REMAIN_UNDER_1B_CH1 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH1    (BIT(23))
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH1_M  (AHB_DMA_OUT_REMAIN_UNDER_1B_CH1_V << AHB_DMA_OUT_REMAIN_UNDER_1B_CH1_S)
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH1_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH1_S  23
/** AHB_DMA_OUT_REMAIN_UNDER_2B_CH1 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH1    (BIT(24))
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH1_M  (AHB_DMA_OUT_REMAIN_UNDER_2B_CH1_V << AHB_DMA_OUT_REMAIN_UNDER_2B_CH1_S)
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH1_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH1_S  24
/** AHB_DMA_OUT_REMAIN_UNDER_3B_CH1 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH1    (BIT(25))
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH1_M  (AHB_DMA_OUT_REMAIN_UNDER_3B_CH1_V << AHB_DMA_OUT_REMAIN_UNDER_3B_CH1_S)
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH1_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH1_S  25
/** AHB_DMA_OUT_REMAIN_UNDER_4B_CH1 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH1    (BIT(26))
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH1_M  (AHB_DMA_OUT_REMAIN_UNDER_4B_CH1_V << AHB_DMA_OUT_REMAIN_UNDER_4B_CH1_S)
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH1_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH1_S  26

/** AHB_DMA_OUT_PUSH_CH1_REG register
 *  Push control register of Rx channel 1
 */
#define AHB_DMA_OUT_PUSH_CH1_REG (DR_REG_AHB_DMA_BASE + 0x19c)
/** AHB_DMA_OUTFIFO_WDATA_CH1 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into AHB_DMA FIFO.
 */
#define AHB_DMA_OUTFIFO_WDATA_CH1    0x000001FFU
#define AHB_DMA_OUTFIFO_WDATA_CH1_M  (AHB_DMA_OUTFIFO_WDATA_CH1_V << AHB_DMA_OUTFIFO_WDATA_CH1_S)
#define AHB_DMA_OUTFIFO_WDATA_CH1_V  0x000001FFU
#define AHB_DMA_OUTFIFO_WDATA_CH1_S  0
/** AHB_DMA_OUTFIFO_PUSH_CH1 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into AHB_DMA FIFO.
 */
#define AHB_DMA_OUTFIFO_PUSH_CH1    (BIT(9))
#define AHB_DMA_OUTFIFO_PUSH_CH1_M  (AHB_DMA_OUTFIFO_PUSH_CH1_V << AHB_DMA_OUTFIFO_PUSH_CH1_S)
#define AHB_DMA_OUTFIFO_PUSH_CH1_V  0x00000001U
#define AHB_DMA_OUTFIFO_PUSH_CH1_S  9

/** AHB_DMA_OUT_LINK_CH1_REG register
 *  Link descriptor configure and control register of Tx channel 1
 */
#define AHB_DMA_OUT_LINK_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1a0)
/** AHB_DMA_OUTLINK_STOP_CH1 : WT; bitpos: [0]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define AHB_DMA_OUTLINK_STOP_CH1    (BIT(0))
#define AHB_DMA_OUTLINK_STOP_CH1_M  (AHB_DMA_OUTLINK_STOP_CH1_V << AHB_DMA_OUTLINK_STOP_CH1_S)
#define AHB_DMA_OUTLINK_STOP_CH1_V  0x00000001U
#define AHB_DMA_OUTLINK_STOP_CH1_S  0
/** AHB_DMA_OUTLINK_START_CH1 : WT; bitpos: [1]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define AHB_DMA_OUTLINK_START_CH1    (BIT(1))
#define AHB_DMA_OUTLINK_START_CH1_M  (AHB_DMA_OUTLINK_START_CH1_V << AHB_DMA_OUTLINK_START_CH1_S)
#define AHB_DMA_OUTLINK_START_CH1_V  0x00000001U
#define AHB_DMA_OUTLINK_START_CH1_S  1
/** AHB_DMA_OUTLINK_RESTART_CH1 : WT; bitpos: [2]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define AHB_DMA_OUTLINK_RESTART_CH1    (BIT(2))
#define AHB_DMA_OUTLINK_RESTART_CH1_M  (AHB_DMA_OUTLINK_RESTART_CH1_V << AHB_DMA_OUTLINK_RESTART_CH1_S)
#define AHB_DMA_OUTLINK_RESTART_CH1_V  0x00000001U
#define AHB_DMA_OUTLINK_RESTART_CH1_S  2
/** AHB_DMA_OUTLINK_PARK_CH1 : RO; bitpos: [3]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define AHB_DMA_OUTLINK_PARK_CH1    (BIT(3))
#define AHB_DMA_OUTLINK_PARK_CH1_M  (AHB_DMA_OUTLINK_PARK_CH1_V << AHB_DMA_OUTLINK_PARK_CH1_S)
#define AHB_DMA_OUTLINK_PARK_CH1_V  0x00000001U
#define AHB_DMA_OUTLINK_PARK_CH1_S  3

/** AHB_DMA_OUT_STATE_CH1_REG register
 *  Transmit status of Tx channel 1
 */
#define AHB_DMA_OUT_STATE_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1a4)
/** AHB_DMA_OUTLINK_DSCR_ADDR_CH1 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH1    0x0003FFFFU
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH1_M  (AHB_DMA_OUTLINK_DSCR_ADDR_CH1_V << AHB_DMA_OUTLINK_DSCR_ADDR_CH1_S)
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH1_V  0x0003FFFFU
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH1_S  0
/** AHB_DMA_OUT_DSCR_STATE_CH1 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_DSCR_STATE_CH1    0x00000003U
#define AHB_DMA_OUT_DSCR_STATE_CH1_M  (AHB_DMA_OUT_DSCR_STATE_CH1_V << AHB_DMA_OUT_DSCR_STATE_CH1_S)
#define AHB_DMA_OUT_DSCR_STATE_CH1_V  0x00000003U
#define AHB_DMA_OUT_DSCR_STATE_CH1_S  18
/** AHB_DMA_OUT_STATE_CH1 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_STATE_CH1    0x00000007U
#define AHB_DMA_OUT_STATE_CH1_M  (AHB_DMA_OUT_STATE_CH1_V << AHB_DMA_OUT_STATE_CH1_S)
#define AHB_DMA_OUT_STATE_CH1_V  0x00000007U
#define AHB_DMA_OUT_STATE_CH1_S  20

/** AHB_DMA_OUT_EOF_DES_ADDR_CH1_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel 1
 */
#define AHB_DMA_OUT_EOF_DES_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1a8)
/** AHB_DMA_OUT_EOF_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AHB_DMA_OUT_EOF_DES_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_DES_ADDR_CH1_M  (AHB_DMA_OUT_EOF_DES_ADDR_CH1_V << AHB_DMA_OUT_EOF_DES_ADDR_CH1_S)
#define AHB_DMA_OUT_EOF_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_DES_ADDR_CH1_S  0

/** AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel 1
 */
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1ac)
/** AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1_M  (AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1_V << AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1_S)
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH1_S  0

/** AHB_DMA_OUT_DSCR_CH1_REG register
 *  Current inlink descriptor address of Tx channel 1
 */
#define AHB_DMA_OUT_DSCR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1b0)
/** AHB_DMA_OUTLINK_DSCR_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define AHB_DMA_OUTLINK_DSCR_CH1    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_CH1_M  (AHB_DMA_OUTLINK_DSCR_CH1_V << AHB_DMA_OUTLINK_DSCR_CH1_S)
#define AHB_DMA_OUTLINK_DSCR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_CH1_S  0

/** AHB_DMA_OUT_DSCR_BF0_CH1_REG register
 *  The last inlink descriptor address of Tx channel 1
 */
#define AHB_DMA_OUT_DSCR_BF0_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1b4)
/** AHB_DMA_OUTLINK_DSCR_BF0_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define AHB_DMA_OUTLINK_DSCR_BF0_CH1    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF0_CH1_M  (AHB_DMA_OUTLINK_DSCR_BF0_CH1_V << AHB_DMA_OUTLINK_DSCR_BF0_CH1_S)
#define AHB_DMA_OUTLINK_DSCR_BF0_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF0_CH1_S  0

/** AHB_DMA_OUT_DSCR_BF1_CH1_REG register
 *  The second-to-last inlink descriptor address of Tx channel 1
 */
#define AHB_DMA_OUT_DSCR_BF1_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1b8)
/** AHB_DMA_OUTLINK_DSCR_BF1_CH1 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AHB_DMA_OUTLINK_DSCR_BF1_CH1    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF1_CH1_M  (AHB_DMA_OUTLINK_DSCR_BF1_CH1_V << AHB_DMA_OUTLINK_DSCR_BF1_CH1_S)
#define AHB_DMA_OUTLINK_DSCR_BF1_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF1_CH1_S  0

/** AHB_DMA_OUT_PRI_CH1_REG register
 *  Priority register of Tx channel 1
 */
#define AHB_DMA_OUT_PRI_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1bc)
/** AHB_DMA_TX_PRI_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel 1. The larger of the value the higher of the priority.
 */
#define AHB_DMA_TX_PRI_CH1    0x0000000FU
#define AHB_DMA_TX_PRI_CH1_M  (AHB_DMA_TX_PRI_CH1_V << AHB_DMA_TX_PRI_CH1_S)
#define AHB_DMA_TX_PRI_CH1_V  0x0000000FU
#define AHB_DMA_TX_PRI_CH1_S  0

/** AHB_DMA_OUT_PERI_SEL_CH1_REG register
 *  Peripheral selection of Tx channel 1
 */
#define AHB_DMA_OUT_PERI_SEL_CH1_REG (DR_REG_AHB_DMA_BASE + 0x1c0)
/** AHB_DMA_PERI_OUT_SEL_CH1 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel 0. I3C. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: I2S1. 5: I2S2. 6: Dummy. 7: Dummy. 8: ADC_DAC. 9: Dummy. 10:
 *  RMT,11~15: Dummy
 */
#define AHB_DMA_PERI_OUT_SEL_CH1    0x0000003FU
#define AHB_DMA_PERI_OUT_SEL_CH1_M  (AHB_DMA_PERI_OUT_SEL_CH1_V << AHB_DMA_PERI_OUT_SEL_CH1_S)
#define AHB_DMA_PERI_OUT_SEL_CH1_V  0x0000003FU
#define AHB_DMA_PERI_OUT_SEL_CH1_S  0

/** AHB_DMA_IN_CONF0_CH2_REG register
 *  Configure 0 register of Rx channel 2
 */
#define AHB_DMA_IN_CONF0_CH2_REG (DR_REG_AHB_DMA_BASE + 0x1f0)
/** AHB_DMA_IN_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AHB_DMA channel 2 Rx FSM and Rx FIFO pointer.
 */
#define AHB_DMA_IN_RST_CH2    (BIT(0))
#define AHB_DMA_IN_RST_CH2_M  (AHB_DMA_IN_RST_CH2_V << AHB_DMA_IN_RST_CH2_S)
#define AHB_DMA_IN_RST_CH2_V  0x00000001U
#define AHB_DMA_IN_RST_CH2_S  0
/** AHB_DMA_IN_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_LOOP_TEST_CH2    (BIT(1))
#define AHB_DMA_IN_LOOP_TEST_CH2_M  (AHB_DMA_IN_LOOP_TEST_CH2_V << AHB_DMA_IN_LOOP_TEST_CH2_S)
#define AHB_DMA_IN_LOOP_TEST_CH2_V  0x00000001U
#define AHB_DMA_IN_LOOP_TEST_CH2_S  1
/** AHB_DMA_INDSCR_BURST_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 2 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AHB_DMA_INDSCR_BURST_EN_CH2    (BIT(2))
#define AHB_DMA_INDSCR_BURST_EN_CH2_M  (AHB_DMA_INDSCR_BURST_EN_CH2_V << AHB_DMA_INDSCR_BURST_EN_CH2_S)
#define AHB_DMA_INDSCR_BURST_EN_CH2_V  0x00000001U
#define AHB_DMA_INDSCR_BURST_EN_CH2_S  2
/** AHB_DMA_IN_DATA_BURST_EN_CH2 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Rx channel 2 receiving data
 *  when accessing internal SRAM.
 */
#define AHB_DMA_IN_DATA_BURST_EN_CH2    (BIT(3))
#define AHB_DMA_IN_DATA_BURST_EN_CH2_M  (AHB_DMA_IN_DATA_BURST_EN_CH2_V << AHB_DMA_IN_DATA_BURST_EN_CH2_S)
#define AHB_DMA_IN_DATA_BURST_EN_CH2_V  0x00000001U
#define AHB_DMA_IN_DATA_BURST_EN_CH2_S  3
/** AHB_DMA_MEM_TRANS_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit 1 to enable automatic transmitting data from memory to memory via
 *  AHB_DMA.
 */
#define AHB_DMA_MEM_TRANS_EN_CH2    (BIT(4))
#define AHB_DMA_MEM_TRANS_EN_CH2_M  (AHB_DMA_MEM_TRANS_EN_CH2_V << AHB_DMA_MEM_TRANS_EN_CH2_S)
#define AHB_DMA_MEM_TRANS_EN_CH2_V  0x00000001U
#define AHB_DMA_MEM_TRANS_EN_CH2_S  4
/** AHB_DMA_IN_ETM_EN_CH2 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Rx channel 2 is triggered by etm
 *  task.
 */
#define AHB_DMA_IN_ETM_EN_CH2    (BIT(5))
#define AHB_DMA_IN_ETM_EN_CH2_M  (AHB_DMA_IN_ETM_EN_CH2_V << AHB_DMA_IN_ETM_EN_CH2_S)
#define AHB_DMA_IN_ETM_EN_CH2_V  0x00000001U
#define AHB_DMA_IN_ETM_EN_CH2_S  5

/** AHB_DMA_IN_CONF1_CH2_REG register
 *  Configure 1 register of Rx channel 2
 */
#define AHB_DMA_IN_CONF1_CH2_REG (DR_REG_AHB_DMA_BASE + 0x1f4)
/** AHB_DMA_IN_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AHB_DMA_IN_CHECK_OWNER_CH2    (BIT(12))
#define AHB_DMA_IN_CHECK_OWNER_CH2_M  (AHB_DMA_IN_CHECK_OWNER_CH2_V << AHB_DMA_IN_CHECK_OWNER_CH2_S)
#define AHB_DMA_IN_CHECK_OWNER_CH2_V  0x00000001U
#define AHB_DMA_IN_CHECK_OWNER_CH2_S  12

/** AHB_DMA_INFIFO_STATUS_CH2_REG register
 *  Receive FIFO status of Rx channel 2
 */
#define AHB_DMA_INFIFO_STATUS_CH2_REG (DR_REG_AHB_DMA_BASE + 0x1f8)
/** AHB_DMA_INFIFO_FULL_CH2 : RO; bitpos: [0]; default: 1;
 *  L1 Rx FIFO full signal for Rx channel 2.
 */
#define AHB_DMA_INFIFO_FULL_CH2    (BIT(0))
#define AHB_DMA_INFIFO_FULL_CH2_M  (AHB_DMA_INFIFO_FULL_CH2_V << AHB_DMA_INFIFO_FULL_CH2_S)
#define AHB_DMA_INFIFO_FULL_CH2_V  0x00000001U
#define AHB_DMA_INFIFO_FULL_CH2_S  0
/** AHB_DMA_INFIFO_EMPTY_CH2 : RO; bitpos: [1]; default: 1;
 *  L1 Rx FIFO empty signal for Rx channel 2.
 */
#define AHB_DMA_INFIFO_EMPTY_CH2    (BIT(1))
#define AHB_DMA_INFIFO_EMPTY_CH2_M  (AHB_DMA_INFIFO_EMPTY_CH2_V << AHB_DMA_INFIFO_EMPTY_CH2_S)
#define AHB_DMA_INFIFO_EMPTY_CH2_V  0x00000001U
#define AHB_DMA_INFIFO_EMPTY_CH2_S  1
/** AHB_DMA_INFIFO_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 2.
 */
#define AHB_DMA_INFIFO_CNT_CH2    0x0000003FU
#define AHB_DMA_INFIFO_CNT_CH2_M  (AHB_DMA_INFIFO_CNT_CH2_V << AHB_DMA_INFIFO_CNT_CH2_S)
#define AHB_DMA_INFIFO_CNT_CH2_V  0x0000003FU
#define AHB_DMA_INFIFO_CNT_CH2_S  2
/** AHB_DMA_IN_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH2    (BIT(23))
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH2_M  (AHB_DMA_IN_REMAIN_UNDER_1B_CH2_V << AHB_DMA_IN_REMAIN_UNDER_1B_CH2_S)
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_1B_CH2_S  23
/** AHB_DMA_IN_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH2    (BIT(24))
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH2_M  (AHB_DMA_IN_REMAIN_UNDER_2B_CH2_V << AHB_DMA_IN_REMAIN_UNDER_2B_CH2_S)
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_2B_CH2_S  24
/** AHB_DMA_IN_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH2    (BIT(25))
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH2_M  (AHB_DMA_IN_REMAIN_UNDER_3B_CH2_V << AHB_DMA_IN_REMAIN_UNDER_3B_CH2_S)
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_3B_CH2_S  25
/** AHB_DMA_IN_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH2    (BIT(26))
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH2_M  (AHB_DMA_IN_REMAIN_UNDER_4B_CH2_V << AHB_DMA_IN_REMAIN_UNDER_4B_CH2_S)
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define AHB_DMA_IN_REMAIN_UNDER_4B_CH2_S  26
/** AHB_DMA_IN_BUF_HUNGRY_CH2 : RO; bitpos: [27]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_BUF_HUNGRY_CH2    (BIT(27))
#define AHB_DMA_IN_BUF_HUNGRY_CH2_M  (AHB_DMA_IN_BUF_HUNGRY_CH2_V << AHB_DMA_IN_BUF_HUNGRY_CH2_S)
#define AHB_DMA_IN_BUF_HUNGRY_CH2_V  0x00000001U
#define AHB_DMA_IN_BUF_HUNGRY_CH2_S  27

/** AHB_DMA_IN_POP_CH2_REG register
 *  Pop control register of Rx channel 2
 */
#define AHB_DMA_IN_POP_CH2_REG (DR_REG_AHB_DMA_BASE + 0x1fc)
/** AHB_DMA_INFIFO_RDATA_CH2 : RO; bitpos: [11:0]; default: 2048;
 *  This register stores the data popping from AHB_DMA FIFO.
 */
#define AHB_DMA_INFIFO_RDATA_CH2    0x00000FFFU
#define AHB_DMA_INFIFO_RDATA_CH2_M  (AHB_DMA_INFIFO_RDATA_CH2_V << AHB_DMA_INFIFO_RDATA_CH2_S)
#define AHB_DMA_INFIFO_RDATA_CH2_V  0x00000FFFU
#define AHB_DMA_INFIFO_RDATA_CH2_S  0
/** AHB_DMA_INFIFO_POP_CH2 : WT; bitpos: [12]; default: 0;
 *  Set this bit to pop data from AHB_DMA FIFO.
 */
#define AHB_DMA_INFIFO_POP_CH2    (BIT(12))
#define AHB_DMA_INFIFO_POP_CH2_M  (AHB_DMA_INFIFO_POP_CH2_V << AHB_DMA_INFIFO_POP_CH2_S)
#define AHB_DMA_INFIFO_POP_CH2_V  0x00000001U
#define AHB_DMA_INFIFO_POP_CH2_S  12

/** AHB_DMA_IN_LINK_CH2_REG register
 *  Link descriptor configure and control register of Rx channel 2
 */
#define AHB_DMA_IN_LINK_CH2_REG (DR_REG_AHB_DMA_BASE + 0x200)
/** AHB_DMA_INLINK_AUTO_RET_CH2 : R/W; bitpos: [0]; default: 1;
 *  Set this bit to return to current inlink descriptor's address when there are some
 *  errors in current receiving data.
 */
#define AHB_DMA_INLINK_AUTO_RET_CH2    (BIT(0))
#define AHB_DMA_INLINK_AUTO_RET_CH2_M  (AHB_DMA_INLINK_AUTO_RET_CH2_V << AHB_DMA_INLINK_AUTO_RET_CH2_S)
#define AHB_DMA_INLINK_AUTO_RET_CH2_V  0x00000001U
#define AHB_DMA_INLINK_AUTO_RET_CH2_S  0
/** AHB_DMA_INLINK_STOP_CH2 : WT; bitpos: [1]; default: 0;
 *  Set this bit to stop dealing with the inlink descriptors.
 */
#define AHB_DMA_INLINK_STOP_CH2    (BIT(1))
#define AHB_DMA_INLINK_STOP_CH2_M  (AHB_DMA_INLINK_STOP_CH2_V << AHB_DMA_INLINK_STOP_CH2_S)
#define AHB_DMA_INLINK_STOP_CH2_V  0x00000001U
#define AHB_DMA_INLINK_STOP_CH2_S  1
/** AHB_DMA_INLINK_START_CH2 : WT; bitpos: [2]; default: 0;
 *  Set this bit to start dealing with the inlink descriptors.
 */
#define AHB_DMA_INLINK_START_CH2    (BIT(2))
#define AHB_DMA_INLINK_START_CH2_M  (AHB_DMA_INLINK_START_CH2_V << AHB_DMA_INLINK_START_CH2_S)
#define AHB_DMA_INLINK_START_CH2_V  0x00000001U
#define AHB_DMA_INLINK_START_CH2_S  2
/** AHB_DMA_INLINK_RESTART_CH2 : WT; bitpos: [3]; default: 0;
 *  Set this bit to mount a new inlink descriptor.
 */
#define AHB_DMA_INLINK_RESTART_CH2    (BIT(3))
#define AHB_DMA_INLINK_RESTART_CH2_M  (AHB_DMA_INLINK_RESTART_CH2_V << AHB_DMA_INLINK_RESTART_CH2_S)
#define AHB_DMA_INLINK_RESTART_CH2_V  0x00000001U
#define AHB_DMA_INLINK_RESTART_CH2_S  3
/** AHB_DMA_INLINK_PARK_CH2 : RO; bitpos: [4]; default: 1;
 *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
 *  working.
 */
#define AHB_DMA_INLINK_PARK_CH2    (BIT(4))
#define AHB_DMA_INLINK_PARK_CH2_M  (AHB_DMA_INLINK_PARK_CH2_V << AHB_DMA_INLINK_PARK_CH2_S)
#define AHB_DMA_INLINK_PARK_CH2_V  0x00000001U
#define AHB_DMA_INLINK_PARK_CH2_S  4

/** AHB_DMA_IN_STATE_CH2_REG register
 *  Receive status of Rx channel 2
 */
#define AHB_DMA_IN_STATE_CH2_REG (DR_REG_AHB_DMA_BASE + 0x204)
/** AHB_DMA_INLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current inlink descriptor's address.
 */
#define AHB_DMA_INLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define AHB_DMA_INLINK_DSCR_ADDR_CH2_M  (AHB_DMA_INLINK_DSCR_ADDR_CH2_V << AHB_DMA_INLINK_DSCR_ADDR_CH2_S)
#define AHB_DMA_INLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define AHB_DMA_INLINK_DSCR_ADDR_CH2_S  0
/** AHB_DMA_IN_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_DSCR_STATE_CH2    0x00000003U
#define AHB_DMA_IN_DSCR_STATE_CH2_M  (AHB_DMA_IN_DSCR_STATE_CH2_V << AHB_DMA_IN_DSCR_STATE_CH2_S)
#define AHB_DMA_IN_DSCR_STATE_CH2_V  0x00000003U
#define AHB_DMA_IN_DSCR_STATE_CH2_S  18
/** AHB_DMA_IN_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AHB_DMA_IN_STATE_CH2    0x00000007U
#define AHB_DMA_IN_STATE_CH2_M  (AHB_DMA_IN_STATE_CH2_V << AHB_DMA_IN_STATE_CH2_S)
#define AHB_DMA_IN_STATE_CH2_V  0x00000007U
#define AHB_DMA_IN_STATE_CH2_S  20

/** AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when EOF occurs of Rx channel 2
 */
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x208)
/** AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2_M  (AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2_V << AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2_S)
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_IN_SUC_EOF_DES_ADDR_CH2_S  0

/** AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2_REG register
 *  Inlink descriptor address when errors occur of Rx channel 2
 */
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x20c)
/** AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the inlink descriptor when there are some
 *  errors in current receiving data. Only used when peripheral is UHCI0.
 */
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2_M  (AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2_V << AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2_S)
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_IN_ERR_EOF_DES_ADDR_CH2_S  0

/** AHB_DMA_IN_DSCR_CH2_REG register
 *  Current inlink descriptor address of Rx channel 2
 */
#define AHB_DMA_IN_DSCR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x210)
/** AHB_DMA_INLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current inlink descriptor x.
 */
#define AHB_DMA_INLINK_DSCR_CH2    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_CH2_M  (AHB_DMA_INLINK_DSCR_CH2_V << AHB_DMA_INLINK_DSCR_CH2_S)
#define AHB_DMA_INLINK_DSCR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_CH2_S  0

/** AHB_DMA_IN_DSCR_BF0_CH2_REG register
 *  The last inlink descriptor address of Rx channel 2
 */
#define AHB_DMA_IN_DSCR_BF0_CH2_REG (DR_REG_AHB_DMA_BASE + 0x214)
/** AHB_DMA_INLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last inlink descriptor x-1.
 */
#define AHB_DMA_INLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF0_CH2_M  (AHB_DMA_INLINK_DSCR_BF0_CH2_V << AHB_DMA_INLINK_DSCR_BF0_CH2_S)
#define AHB_DMA_INLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF0_CH2_S  0

/** AHB_DMA_IN_DSCR_BF1_CH2_REG register
 *  The second-to-last inlink descriptor address of Rx channel 2
 */
#define AHB_DMA_IN_DSCR_BF1_CH2_REG (DR_REG_AHB_DMA_BASE + 0x218)
/** AHB_DMA_INLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AHB_DMA_INLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF1_CH2_M  (AHB_DMA_INLINK_DSCR_BF1_CH2_V << AHB_DMA_INLINK_DSCR_BF1_CH2_S)
#define AHB_DMA_INLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_DSCR_BF1_CH2_S  0

/** AHB_DMA_IN_PRI_CH2_REG register
 *  Priority register of Rx channel 2
 */
#define AHB_DMA_IN_PRI_CH2_REG (DR_REG_AHB_DMA_BASE + 0x21c)
/** AHB_DMA_RX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Rx channel 2. The larger of the value the higher of the priority.
 */
#define AHB_DMA_RX_PRI_CH2    0x0000000FU
#define AHB_DMA_RX_PRI_CH2_M  (AHB_DMA_RX_PRI_CH2_V << AHB_DMA_RX_PRI_CH2_S)
#define AHB_DMA_RX_PRI_CH2_V  0x0000000FU
#define AHB_DMA_RX_PRI_CH2_S  0

/** AHB_DMA_IN_PERI_SEL_CH2_REG register
 *  Peripheral selection of Rx channel 2
 */
#define AHB_DMA_IN_PERI_SEL_CH2_REG (DR_REG_AHB_DMA_BASE + 0x220)
/** AHB_DMA_PERI_IN_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Rx channel 0. I3C. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: I2S1. 5: I2S2. 6: Dummy. 7: Dummy. 8: ADC_DAC. 9: Dummy. 10:
 *  RMT,11~15: Dummy
 */
#define AHB_DMA_PERI_IN_SEL_CH2    0x0000003FU
#define AHB_DMA_PERI_IN_SEL_CH2_M  (AHB_DMA_PERI_IN_SEL_CH2_V << AHB_DMA_PERI_IN_SEL_CH2_S)
#define AHB_DMA_PERI_IN_SEL_CH2_V  0x0000003FU
#define AHB_DMA_PERI_IN_SEL_CH2_S  0

/** AHB_DMA_OUT_CONF0_CH2_REG register
 *  Configure 0 register of Tx channel 2
 */
#define AHB_DMA_OUT_CONF0_CH2_REG (DR_REG_AHB_DMA_BASE + 0x250)
/** AHB_DMA_OUT_RST_CH2 : R/W; bitpos: [0]; default: 0;
 *  This bit is used to reset AHB_DMA channel 2 Tx FSM and Tx FIFO pointer.
 */
#define AHB_DMA_OUT_RST_CH2    (BIT(0))
#define AHB_DMA_OUT_RST_CH2_M  (AHB_DMA_OUT_RST_CH2_V << AHB_DMA_OUT_RST_CH2_S)
#define AHB_DMA_OUT_RST_CH2_V  0x00000001U
#define AHB_DMA_OUT_RST_CH2_S  0
/** AHB_DMA_OUT_LOOP_TEST_CH2 : R/W; bitpos: [1]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_LOOP_TEST_CH2    (BIT(1))
#define AHB_DMA_OUT_LOOP_TEST_CH2_M  (AHB_DMA_OUT_LOOP_TEST_CH2_V << AHB_DMA_OUT_LOOP_TEST_CH2_S)
#define AHB_DMA_OUT_LOOP_TEST_CH2_V  0x00000001U
#define AHB_DMA_OUT_LOOP_TEST_CH2_S  1
/** AHB_DMA_OUT_AUTO_WRBACK_CH2 : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
 *  has been transmitted.
 */
#define AHB_DMA_OUT_AUTO_WRBACK_CH2    (BIT(2))
#define AHB_DMA_OUT_AUTO_WRBACK_CH2_M  (AHB_DMA_OUT_AUTO_WRBACK_CH2_V << AHB_DMA_OUT_AUTO_WRBACK_CH2_S)
#define AHB_DMA_OUT_AUTO_WRBACK_CH2_V  0x00000001U
#define AHB_DMA_OUT_AUTO_WRBACK_CH2_S  2
/** AHB_DMA_OUT_EOF_MODE_CH2 : R/W; bitpos: [3]; default: 1;
 *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 2 is
 *  generated when data need to transmit has been popped from FIFO in AHB_DMA
 */
#define AHB_DMA_OUT_EOF_MODE_CH2    (BIT(3))
#define AHB_DMA_OUT_EOF_MODE_CH2_M  (AHB_DMA_OUT_EOF_MODE_CH2_V << AHB_DMA_OUT_EOF_MODE_CH2_S)
#define AHB_DMA_OUT_EOF_MODE_CH2_V  0x00000001U
#define AHB_DMA_OUT_EOF_MODE_CH2_S  3
/** AHB_DMA_OUTDSCR_BURST_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 2 reading link
 *  descriptor when accessing internal SRAM.
 */
#define AHB_DMA_OUTDSCR_BURST_EN_CH2    (BIT(4))
#define AHB_DMA_OUTDSCR_BURST_EN_CH2_M  (AHB_DMA_OUTDSCR_BURST_EN_CH2_V << AHB_DMA_OUTDSCR_BURST_EN_CH2_S)
#define AHB_DMA_OUTDSCR_BURST_EN_CH2_V  0x00000001U
#define AHB_DMA_OUTDSCR_BURST_EN_CH2_S  4
/** AHB_DMA_OUT_DATA_BURST_EN_CH2 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to 1 to enable INCR burst transfer for Tx channel 2 transmitting data
 *  when accessing internal SRAM.
 */
#define AHB_DMA_OUT_DATA_BURST_EN_CH2    (BIT(5))
#define AHB_DMA_OUT_DATA_BURST_EN_CH2_M  (AHB_DMA_OUT_DATA_BURST_EN_CH2_V << AHB_DMA_OUT_DATA_BURST_EN_CH2_S)
#define AHB_DMA_OUT_DATA_BURST_EN_CH2_V  0x00000001U
#define AHB_DMA_OUT_DATA_BURST_EN_CH2_S  5
/** AHB_DMA_OUT_ETM_EN_CH2 : R/W; bitpos: [6]; default: 0;
 *  Set this bit to 1 to enable etm control mode, dma Tx channel 2 is triggered by etm
 *  task.
 */
#define AHB_DMA_OUT_ETM_EN_CH2    (BIT(6))
#define AHB_DMA_OUT_ETM_EN_CH2_M  (AHB_DMA_OUT_ETM_EN_CH2_V << AHB_DMA_OUT_ETM_EN_CH2_S)
#define AHB_DMA_OUT_ETM_EN_CH2_V  0x00000001U
#define AHB_DMA_OUT_ETM_EN_CH2_S  6

/** AHB_DMA_OUT_CONF1_CH2_REG register
 *  Configure 1 register of Tx channel 2
 */
#define AHB_DMA_OUT_CONF1_CH2_REG (DR_REG_AHB_DMA_BASE + 0x254)
/** AHB_DMA_OUT_CHECK_OWNER_CH2 : R/W; bitpos: [12]; default: 0;
 *  Set this bit to enable checking the owner attribute of the link descriptor.
 */
#define AHB_DMA_OUT_CHECK_OWNER_CH2    (BIT(12))
#define AHB_DMA_OUT_CHECK_OWNER_CH2_M  (AHB_DMA_OUT_CHECK_OWNER_CH2_V << AHB_DMA_OUT_CHECK_OWNER_CH2_S)
#define AHB_DMA_OUT_CHECK_OWNER_CH2_V  0x00000001U
#define AHB_DMA_OUT_CHECK_OWNER_CH2_S  12

/** AHB_DMA_OUTFIFO_STATUS_CH2_REG register
 *  Transmit FIFO status of Tx channel 2
 */
#define AHB_DMA_OUTFIFO_STATUS_CH2_REG (DR_REG_AHB_DMA_BASE + 0x258)
/** AHB_DMA_OUTFIFO_FULL_CH2 : RO; bitpos: [0]; default: 0;
 *  L1 Tx FIFO full signal for Tx channel 2.
 */
#define AHB_DMA_OUTFIFO_FULL_CH2    (BIT(0))
#define AHB_DMA_OUTFIFO_FULL_CH2_M  (AHB_DMA_OUTFIFO_FULL_CH2_V << AHB_DMA_OUTFIFO_FULL_CH2_S)
#define AHB_DMA_OUTFIFO_FULL_CH2_V  0x00000001U
#define AHB_DMA_OUTFIFO_FULL_CH2_S  0
/** AHB_DMA_OUTFIFO_EMPTY_CH2 : RO; bitpos: [1]; default: 1;
 *  L1 Tx FIFO empty signal for Tx channel 2.
 */
#define AHB_DMA_OUTFIFO_EMPTY_CH2    (BIT(1))
#define AHB_DMA_OUTFIFO_EMPTY_CH2_M  (AHB_DMA_OUTFIFO_EMPTY_CH2_V << AHB_DMA_OUTFIFO_EMPTY_CH2_S)
#define AHB_DMA_OUTFIFO_EMPTY_CH2_V  0x00000001U
#define AHB_DMA_OUTFIFO_EMPTY_CH2_S  1
/** AHB_DMA_OUTFIFO_CNT_CH2 : RO; bitpos: [7:2]; default: 0;
 *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 2.
 */
#define AHB_DMA_OUTFIFO_CNT_CH2    0x0000003FU
#define AHB_DMA_OUTFIFO_CNT_CH2_M  (AHB_DMA_OUTFIFO_CNT_CH2_V << AHB_DMA_OUTFIFO_CNT_CH2_S)
#define AHB_DMA_OUTFIFO_CNT_CH2_V  0x0000003FU
#define AHB_DMA_OUTFIFO_CNT_CH2_S  2
/** AHB_DMA_OUT_REMAIN_UNDER_1B_CH2 : RO; bitpos: [23]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH2    (BIT(23))
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH2_M  (AHB_DMA_OUT_REMAIN_UNDER_1B_CH2_V << AHB_DMA_OUT_REMAIN_UNDER_1B_CH2_S)
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH2_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_1B_CH2_S  23
/** AHB_DMA_OUT_REMAIN_UNDER_2B_CH2 : RO; bitpos: [24]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH2    (BIT(24))
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH2_M  (AHB_DMA_OUT_REMAIN_UNDER_2B_CH2_V << AHB_DMA_OUT_REMAIN_UNDER_2B_CH2_S)
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH2_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_2B_CH2_S  24
/** AHB_DMA_OUT_REMAIN_UNDER_3B_CH2 : RO; bitpos: [25]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH2    (BIT(25))
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH2_M  (AHB_DMA_OUT_REMAIN_UNDER_3B_CH2_V << AHB_DMA_OUT_REMAIN_UNDER_3B_CH2_S)
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH2_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_3B_CH2_S  25
/** AHB_DMA_OUT_REMAIN_UNDER_4B_CH2 : RO; bitpos: [26]; default: 1;
 *  reserved
 */
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH2    (BIT(26))
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH2_M  (AHB_DMA_OUT_REMAIN_UNDER_4B_CH2_V << AHB_DMA_OUT_REMAIN_UNDER_4B_CH2_S)
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH2_V  0x00000001U
#define AHB_DMA_OUT_REMAIN_UNDER_4B_CH2_S  26

/** AHB_DMA_OUT_PUSH_CH2_REG register
 *  Push control register of Rx channel 2
 */
#define AHB_DMA_OUT_PUSH_CH2_REG (DR_REG_AHB_DMA_BASE + 0x25c)
/** AHB_DMA_OUTFIFO_WDATA_CH2 : R/W; bitpos: [8:0]; default: 0;
 *  This register stores the data that need to be pushed into AHB_DMA FIFO.
 */
#define AHB_DMA_OUTFIFO_WDATA_CH2    0x000001FFU
#define AHB_DMA_OUTFIFO_WDATA_CH2_M  (AHB_DMA_OUTFIFO_WDATA_CH2_V << AHB_DMA_OUTFIFO_WDATA_CH2_S)
#define AHB_DMA_OUTFIFO_WDATA_CH2_V  0x000001FFU
#define AHB_DMA_OUTFIFO_WDATA_CH2_S  0
/** AHB_DMA_OUTFIFO_PUSH_CH2 : WT; bitpos: [9]; default: 0;
 *  Set this bit to push data into AHB_DMA FIFO.
 */
#define AHB_DMA_OUTFIFO_PUSH_CH2    (BIT(9))
#define AHB_DMA_OUTFIFO_PUSH_CH2_M  (AHB_DMA_OUTFIFO_PUSH_CH2_V << AHB_DMA_OUTFIFO_PUSH_CH2_S)
#define AHB_DMA_OUTFIFO_PUSH_CH2_V  0x00000001U
#define AHB_DMA_OUTFIFO_PUSH_CH2_S  9

/** AHB_DMA_OUT_LINK_CH2_REG register
 *  Link descriptor configure and control register of Tx channel 2
 */
#define AHB_DMA_OUT_LINK_CH2_REG (DR_REG_AHB_DMA_BASE + 0x260)
/** AHB_DMA_OUTLINK_STOP_CH2 : WT; bitpos: [0]; default: 0;
 *  Set this bit to stop dealing with the outlink descriptors.
 */
#define AHB_DMA_OUTLINK_STOP_CH2    (BIT(0))
#define AHB_DMA_OUTLINK_STOP_CH2_M  (AHB_DMA_OUTLINK_STOP_CH2_V << AHB_DMA_OUTLINK_STOP_CH2_S)
#define AHB_DMA_OUTLINK_STOP_CH2_V  0x00000001U
#define AHB_DMA_OUTLINK_STOP_CH2_S  0
/** AHB_DMA_OUTLINK_START_CH2 : WT; bitpos: [1]; default: 0;
 *  Set this bit to start dealing with the outlink descriptors.
 */
#define AHB_DMA_OUTLINK_START_CH2    (BIT(1))
#define AHB_DMA_OUTLINK_START_CH2_M  (AHB_DMA_OUTLINK_START_CH2_V << AHB_DMA_OUTLINK_START_CH2_S)
#define AHB_DMA_OUTLINK_START_CH2_V  0x00000001U
#define AHB_DMA_OUTLINK_START_CH2_S  1
/** AHB_DMA_OUTLINK_RESTART_CH2 : WT; bitpos: [2]; default: 0;
 *  Set this bit to restart a new outlink from the last address.
 */
#define AHB_DMA_OUTLINK_RESTART_CH2    (BIT(2))
#define AHB_DMA_OUTLINK_RESTART_CH2_M  (AHB_DMA_OUTLINK_RESTART_CH2_V << AHB_DMA_OUTLINK_RESTART_CH2_S)
#define AHB_DMA_OUTLINK_RESTART_CH2_V  0x00000001U
#define AHB_DMA_OUTLINK_RESTART_CH2_S  2
/** AHB_DMA_OUTLINK_PARK_CH2 : RO; bitpos: [3]; default: 1;
 *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
 *  is working.
 */
#define AHB_DMA_OUTLINK_PARK_CH2    (BIT(3))
#define AHB_DMA_OUTLINK_PARK_CH2_M  (AHB_DMA_OUTLINK_PARK_CH2_V << AHB_DMA_OUTLINK_PARK_CH2_S)
#define AHB_DMA_OUTLINK_PARK_CH2_V  0x00000001U
#define AHB_DMA_OUTLINK_PARK_CH2_S  3

/** AHB_DMA_OUT_STATE_CH2_REG register
 *  Transmit status of Tx channel 2
 */
#define AHB_DMA_OUT_STATE_CH2_REG (DR_REG_AHB_DMA_BASE + 0x264)
/** AHB_DMA_OUTLINK_DSCR_ADDR_CH2 : RO; bitpos: [17:0]; default: 0;
 *  This register stores the current outlink descriptor's address.
 */
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH2    0x0003FFFFU
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH2_M  (AHB_DMA_OUTLINK_DSCR_ADDR_CH2_V << AHB_DMA_OUTLINK_DSCR_ADDR_CH2_S)
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH2_V  0x0003FFFFU
#define AHB_DMA_OUTLINK_DSCR_ADDR_CH2_S  0
/** AHB_DMA_OUT_DSCR_STATE_CH2 : RO; bitpos: [19:18]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_DSCR_STATE_CH2    0x00000003U
#define AHB_DMA_OUT_DSCR_STATE_CH2_M  (AHB_DMA_OUT_DSCR_STATE_CH2_V << AHB_DMA_OUT_DSCR_STATE_CH2_S)
#define AHB_DMA_OUT_DSCR_STATE_CH2_V  0x00000003U
#define AHB_DMA_OUT_DSCR_STATE_CH2_S  18
/** AHB_DMA_OUT_STATE_CH2 : RO; bitpos: [22:20]; default: 0;
 *  reserved
 */
#define AHB_DMA_OUT_STATE_CH2    0x00000007U
#define AHB_DMA_OUT_STATE_CH2_M  (AHB_DMA_OUT_STATE_CH2_V << AHB_DMA_OUT_STATE_CH2_S)
#define AHB_DMA_OUT_STATE_CH2_V  0x00000007U
#define AHB_DMA_OUT_STATE_CH2_S  20

/** AHB_DMA_OUT_EOF_DES_ADDR_CH2_REG register
 *  Outlink descriptor address when EOF occurs of Tx channel 2
 */
#define AHB_DMA_OUT_EOF_DES_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x268)
/** AHB_DMA_OUT_EOF_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor when the EOF bit in this
 *  descriptor is 1.
 */
#define AHB_DMA_OUT_EOF_DES_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_DES_ADDR_CH2_M  (AHB_DMA_OUT_EOF_DES_ADDR_CH2_V << AHB_DMA_OUT_EOF_DES_ADDR_CH2_S)
#define AHB_DMA_OUT_EOF_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_DES_ADDR_CH2_S  0

/** AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2_REG register
 *  The last outlink descriptor address when EOF occurs of Tx channel 2
 */
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x26c)
/** AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register stores the address of the outlink descriptor before the last outlink
 *  descriptor.
 */
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2_M  (AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2_V << AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2_S)
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUT_EOF_BFR_DES_ADDR_CH2_S  0

/** AHB_DMA_OUT_DSCR_CH2_REG register
 *  Current inlink descriptor address of Tx channel 2
 */
#define AHB_DMA_OUT_DSCR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x270)
/** AHB_DMA_OUTLINK_DSCR_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the current outlink descriptor y.
 */
#define AHB_DMA_OUTLINK_DSCR_CH2    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_CH2_M  (AHB_DMA_OUTLINK_DSCR_CH2_V << AHB_DMA_OUTLINK_DSCR_CH2_S)
#define AHB_DMA_OUTLINK_DSCR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_CH2_S  0

/** AHB_DMA_OUT_DSCR_BF0_CH2_REG register
 *  The last inlink descriptor address of Tx channel 2
 */
#define AHB_DMA_OUT_DSCR_BF0_CH2_REG (DR_REG_AHB_DMA_BASE + 0x274)
/** AHB_DMA_OUTLINK_DSCR_BF0_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the last outlink descriptor y-1.
 */
#define AHB_DMA_OUTLINK_DSCR_BF0_CH2    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF0_CH2_M  (AHB_DMA_OUTLINK_DSCR_BF0_CH2_V << AHB_DMA_OUTLINK_DSCR_BF0_CH2_S)
#define AHB_DMA_OUTLINK_DSCR_BF0_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF0_CH2_S  0

/** AHB_DMA_OUT_DSCR_BF1_CH2_REG register
 *  The second-to-last inlink descriptor address of Tx channel 2
 */
#define AHB_DMA_OUT_DSCR_BF1_CH2_REG (DR_REG_AHB_DMA_BASE + 0x278)
/** AHB_DMA_OUTLINK_DSCR_BF1_CH2 : RO; bitpos: [31:0]; default: 0;
 *  The address of the second-to-last inlink descriptor x-2.
 */
#define AHB_DMA_OUTLINK_DSCR_BF1_CH2    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF1_CH2_M  (AHB_DMA_OUTLINK_DSCR_BF1_CH2_V << AHB_DMA_OUTLINK_DSCR_BF1_CH2_S)
#define AHB_DMA_OUTLINK_DSCR_BF1_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_DSCR_BF1_CH2_S  0

/** AHB_DMA_OUT_PRI_CH2_REG register
 *  Priority register of Tx channel 2
 */
#define AHB_DMA_OUT_PRI_CH2_REG (DR_REG_AHB_DMA_BASE + 0x27c)
/** AHB_DMA_TX_PRI_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  The priority of Tx channel 2. The larger of the value the higher of the priority.
 */
#define AHB_DMA_TX_PRI_CH2    0x0000000FU
#define AHB_DMA_TX_PRI_CH2_M  (AHB_DMA_TX_PRI_CH2_V << AHB_DMA_TX_PRI_CH2_S)
#define AHB_DMA_TX_PRI_CH2_V  0x0000000FU
#define AHB_DMA_TX_PRI_CH2_S  0

/** AHB_DMA_OUT_PERI_SEL_CH2_REG register
 *  Peripheral selection of Tx channel 2
 */
#define AHB_DMA_OUT_PERI_SEL_CH2_REG (DR_REG_AHB_DMA_BASE + 0x280)
/** AHB_DMA_PERI_OUT_SEL_CH2 : R/W; bitpos: [5:0]; default: 63;
 *  This register is used to select peripheral for Tx channel 0. I3C. 1: Dummy. 2:
 *  UHCI0. 3: I2S0. 4: I2S1. 5: I2S2. 6: Dummy. 7: Dummy. 8: ADC_DAC. 9: Dummy. 10:
 *  RMT,11~15: Dummy
 */
#define AHB_DMA_PERI_OUT_SEL_CH2    0x0000003FU
#define AHB_DMA_PERI_OUT_SEL_CH2_M  (AHB_DMA_PERI_OUT_SEL_CH2_V << AHB_DMA_PERI_OUT_SEL_CH2_S)
#define AHB_DMA_PERI_OUT_SEL_CH2_V  0x0000003FU
#define AHB_DMA_PERI_OUT_SEL_CH2_S  0

/** AHB_DMA_OUT_CRC_INIT_DATA_CH0_REG register
 *  This register is used to config ch0 crc initial data(max 32 bit)
 */
#define AHB_DMA_OUT_CRC_INIT_DATA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2bc)
/** AHB_DMA_OUT_CRC_INIT_DATA_CH0 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch0 of tx crc initial value
 */
#define AHB_DMA_OUT_CRC_INIT_DATA_CH0    0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_INIT_DATA_CH0_M  (AHB_DMA_OUT_CRC_INIT_DATA_CH0_V << AHB_DMA_OUT_CRC_INIT_DATA_CH0_S)
#define AHB_DMA_OUT_CRC_INIT_DATA_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_INIT_DATA_CH0_S  0

/** AHB_DMA_TX_CRC_WIDTH_CH0_REG register
 *  This register is used to config tx ch0 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AHB_DMA_TX_CRC_WIDTH_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2c0)
/** AHB_DMA_TX_CRC_WIDTH_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_WIDTH_CH0    0x00000003U
#define AHB_DMA_TX_CRC_WIDTH_CH0_M  (AHB_DMA_TX_CRC_WIDTH_CH0_V << AHB_DMA_TX_CRC_WIDTH_CH0_S)
#define AHB_DMA_TX_CRC_WIDTH_CH0_V  0x00000003U
#define AHB_DMA_TX_CRC_WIDTH_CH0_S  0
/** AHB_DMA_TX_CRC_LATCH_FLAG_CH0 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH0    (BIT(2))
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH0_M  (AHB_DMA_TX_CRC_LATCH_FLAG_CH0_V << AHB_DMA_TX_CRC_LATCH_FLAG_CH0_S)
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH0_V  0x00000001U
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH0_S  2

/** AHB_DMA_OUT_CRC_CLEAR_CH0_REG register
 *  This register is used to clear ch0 crc result
 */
#define AHB_DMA_OUT_CRC_CLEAR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2c4)
/** AHB_DMA_OUT_CRC_CLEAR_CH0 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch0 of tx crc result
 */
#define AHB_DMA_OUT_CRC_CLEAR_CH0    (BIT(0))
#define AHB_DMA_OUT_CRC_CLEAR_CH0_M  (AHB_DMA_OUT_CRC_CLEAR_CH0_V << AHB_DMA_OUT_CRC_CLEAR_CH0_S)
#define AHB_DMA_OUT_CRC_CLEAR_CH0_V  0x00000001U
#define AHB_DMA_OUT_CRC_CLEAR_CH0_S  0

/** AHB_DMA_OUT_CRC_FINAL_RESULT_CH0_REG register
 *  This register is used to store ch0 crc result
 */
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2c8)
/** AHB_DMA_OUT_CRC_FINAL_RESULT_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch0 of tx
 */
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH0    0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH0_M  (AHB_DMA_OUT_CRC_FINAL_RESULT_CH0_V << AHB_DMA_OUT_CRC_FINAL_RESULT_CH0_S)
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH0_S  0

/** AHB_DMA_TX_CRC_EN_WR_DATA_CH0_REG register
 *  This resister is used to config ch0 crc en for every bit
 */
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2cc)
/** AHB_DMA_TX_CRC_EN_WR_DATA_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable tx ch0 crc 32bit on/off
 */
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH0    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH0_M  (AHB_DMA_TX_CRC_EN_WR_DATA_CH0_V << AHB_DMA_TX_CRC_EN_WR_DATA_CH0_S)
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH0_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH0_S  0

/** AHB_DMA_TX_CRC_EN_ADDR_CH0_REG register
 *  This register is used to config ch0 crc en addr
 */
#define AHB_DMA_TX_CRC_EN_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2d0)
/** AHB_DMA_TX_CRC_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_EN_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_ADDR_CH0_M  (AHB_DMA_TX_CRC_EN_ADDR_CH0_V << AHB_DMA_TX_CRC_EN_ADDR_CH0_S)
#define AHB_DMA_TX_CRC_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_ADDR_CH0_S  0

/** AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_REG register
 *  This register is used to config crc data_8bit en
 */
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2d4)
/** AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0 : R/W; bitpos: [7:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0    0x000000FFU
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_M  (AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_V << AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_S)
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_V  0x000000FFU
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH0_S  0

/** AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2d8)
/** AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0_M  (AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0_V << AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0_S)
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH0_S  0

/** AHB_DMA_TX_CH_ARB_WEIGH_CH0_REG register
 *  This register is used to config ch0 arbiter weigh
 */
#define AHB_DMA_TX_CH_ARB_WEIGH_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2dc)
/** AHB_DMA_TX_CH_ARB_WEIGH_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CH_ARB_WEIGH_CH0    0x0000000FU
#define AHB_DMA_TX_CH_ARB_WEIGH_CH0_M  (AHB_DMA_TX_CH_ARB_WEIGH_CH0_V << AHB_DMA_TX_CH_ARB_WEIGH_CH0_S)
#define AHB_DMA_TX_CH_ARB_WEIGH_CH0_V  0x0000000FU
#define AHB_DMA_TX_CH_ARB_WEIGH_CH0_S  0

/** AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_REG register
 *  This register is used to config off or on weigh optimization
 */
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x2e0)
/** AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0 : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0    (BIT(0))
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_M  (AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_V << AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_S)
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_V  0x00000001U
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_S  0

/** AHB_DMA_OUT_CRC_INIT_DATA_CH1_REG register
 *  This register is used to config ch1 crc initial data(max 32 bit)
 */
#define AHB_DMA_OUT_CRC_INIT_DATA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x2e4)
/** AHB_DMA_OUT_CRC_INIT_DATA_CH1 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch1 of tx crc initial value
 */
#define AHB_DMA_OUT_CRC_INIT_DATA_CH1    0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_INIT_DATA_CH1_M  (AHB_DMA_OUT_CRC_INIT_DATA_CH1_V << AHB_DMA_OUT_CRC_INIT_DATA_CH1_S)
#define AHB_DMA_OUT_CRC_INIT_DATA_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_INIT_DATA_CH1_S  0

/** AHB_DMA_TX_CRC_WIDTH_CH1_REG register
 *  This register is used to config tx ch1 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AHB_DMA_TX_CRC_WIDTH_CH1_REG (DR_REG_AHB_DMA_BASE + 0x2e8)
/** AHB_DMA_TX_CRC_WIDTH_CH1 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_WIDTH_CH1    0x00000003U
#define AHB_DMA_TX_CRC_WIDTH_CH1_M  (AHB_DMA_TX_CRC_WIDTH_CH1_V << AHB_DMA_TX_CRC_WIDTH_CH1_S)
#define AHB_DMA_TX_CRC_WIDTH_CH1_V  0x00000003U
#define AHB_DMA_TX_CRC_WIDTH_CH1_S  0
/** AHB_DMA_TX_CRC_LATCH_FLAG_CH1 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH1    (BIT(2))
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH1_M  (AHB_DMA_TX_CRC_LATCH_FLAG_CH1_V << AHB_DMA_TX_CRC_LATCH_FLAG_CH1_S)
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH1_V  0x00000001U
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH1_S  2

/** AHB_DMA_OUT_CRC_CLEAR_CH1_REG register
 *  This register is used to clear ch1 crc result
 */
#define AHB_DMA_OUT_CRC_CLEAR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x2ec)
/** AHB_DMA_OUT_CRC_CLEAR_CH1 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch1 of tx crc result
 */
#define AHB_DMA_OUT_CRC_CLEAR_CH1    (BIT(0))
#define AHB_DMA_OUT_CRC_CLEAR_CH1_M  (AHB_DMA_OUT_CRC_CLEAR_CH1_V << AHB_DMA_OUT_CRC_CLEAR_CH1_S)
#define AHB_DMA_OUT_CRC_CLEAR_CH1_V  0x00000001U
#define AHB_DMA_OUT_CRC_CLEAR_CH1_S  0

/** AHB_DMA_OUT_CRC_FINAL_RESULT_CH1_REG register
 *  This register is used to store ch1 crc result
 */
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH1_REG (DR_REG_AHB_DMA_BASE + 0x2f0)
/** AHB_DMA_OUT_CRC_FINAL_RESULT_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch1 of tx
 */
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH1    0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH1_M  (AHB_DMA_OUT_CRC_FINAL_RESULT_CH1_V << AHB_DMA_OUT_CRC_FINAL_RESULT_CH1_S)
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH1_S  0

/** AHB_DMA_TX_CRC_EN_WR_DATA_CH1_REG register
 *  This resister is used to config ch1 crc en for every bit
 */
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x2f4)
/** AHB_DMA_TX_CRC_EN_WR_DATA_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable tx ch1 crc 32bit on/off
 */
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH1    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH1_M  (AHB_DMA_TX_CRC_EN_WR_DATA_CH1_V << AHB_DMA_TX_CRC_EN_WR_DATA_CH1_S)
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH1_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH1_S  0

/** AHB_DMA_TX_CRC_EN_ADDR_CH1_REG register
 *  This register is used to config ch1 crc en addr
 */
#define AHB_DMA_TX_CRC_EN_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x2f8)
/** AHB_DMA_TX_CRC_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_EN_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_ADDR_CH1_M  (AHB_DMA_TX_CRC_EN_ADDR_CH1_V << AHB_DMA_TX_CRC_EN_ADDR_CH1_S)
#define AHB_DMA_TX_CRC_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_ADDR_CH1_S  0

/** AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_REG register
 *  This register is used to config crc data_8bit en
 */
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x2fc)
/** AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1 : R/W; bitpos: [7:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1    0x000000FFU
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_M  (AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_V << AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_S)
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_V  0x000000FFU
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH1_S  0

/** AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x300)
/** AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1_M  (AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1_V << AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1_S)
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH1_S  0

/** AHB_DMA_TX_CH_ARB_WEIGH_CH1_REG register
 *  This register is used to config ch1 arbiter weigh
 */
#define AHB_DMA_TX_CH_ARB_WEIGH_CH1_REG (DR_REG_AHB_DMA_BASE + 0x304)
/** AHB_DMA_TX_CH_ARB_WEIGH_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CH_ARB_WEIGH_CH1    0x0000000FU
#define AHB_DMA_TX_CH_ARB_WEIGH_CH1_M  (AHB_DMA_TX_CH_ARB_WEIGH_CH1_V << AHB_DMA_TX_CH_ARB_WEIGH_CH1_S)
#define AHB_DMA_TX_CH_ARB_WEIGH_CH1_V  0x0000000FU
#define AHB_DMA_TX_CH_ARB_WEIGH_CH1_S  0

/** AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_REG register
 *  This register is used to config off or on weigh optimization
 */
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x308)
/** AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1 : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1    (BIT(0))
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_M  (AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_V << AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_S)
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_V  0x00000001U
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_S  0

/** AHB_DMA_OUT_CRC_INIT_DATA_CH2_REG register
 *  This register is used to config ch2 crc initial data(max 32 bit)
 */
#define AHB_DMA_OUT_CRC_INIT_DATA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x30c)
/** AHB_DMA_OUT_CRC_INIT_DATA_CH2 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch2 of tx crc initial value
 */
#define AHB_DMA_OUT_CRC_INIT_DATA_CH2    0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_INIT_DATA_CH2_M  (AHB_DMA_OUT_CRC_INIT_DATA_CH2_V << AHB_DMA_OUT_CRC_INIT_DATA_CH2_S)
#define AHB_DMA_OUT_CRC_INIT_DATA_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_INIT_DATA_CH2_S  0

/** AHB_DMA_TX_CRC_WIDTH_CH2_REG register
 *  This register is used to config tx ch2 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AHB_DMA_TX_CRC_WIDTH_CH2_REG (DR_REG_AHB_DMA_BASE + 0x310)
/** AHB_DMA_TX_CRC_WIDTH_CH2 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_WIDTH_CH2    0x00000003U
#define AHB_DMA_TX_CRC_WIDTH_CH2_M  (AHB_DMA_TX_CRC_WIDTH_CH2_V << AHB_DMA_TX_CRC_WIDTH_CH2_S)
#define AHB_DMA_TX_CRC_WIDTH_CH2_V  0x00000003U
#define AHB_DMA_TX_CRC_WIDTH_CH2_S  0
/** AHB_DMA_TX_CRC_LATCH_FLAG_CH2 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH2    (BIT(2))
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH2_M  (AHB_DMA_TX_CRC_LATCH_FLAG_CH2_V << AHB_DMA_TX_CRC_LATCH_FLAG_CH2_S)
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH2_V  0x00000001U
#define AHB_DMA_TX_CRC_LATCH_FLAG_CH2_S  2

/** AHB_DMA_OUT_CRC_CLEAR_CH2_REG register
 *  This register is used to clear ch2 crc result
 */
#define AHB_DMA_OUT_CRC_CLEAR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x314)
/** AHB_DMA_OUT_CRC_CLEAR_CH2 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch2 of tx crc result
 */
#define AHB_DMA_OUT_CRC_CLEAR_CH2    (BIT(0))
#define AHB_DMA_OUT_CRC_CLEAR_CH2_M  (AHB_DMA_OUT_CRC_CLEAR_CH2_V << AHB_DMA_OUT_CRC_CLEAR_CH2_S)
#define AHB_DMA_OUT_CRC_CLEAR_CH2_V  0x00000001U
#define AHB_DMA_OUT_CRC_CLEAR_CH2_S  0

/** AHB_DMA_OUT_CRC_FINAL_RESULT_CH2_REG register
 *  This register is used to store ch2 crc result
 */
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH2_REG (DR_REG_AHB_DMA_BASE + 0x318)
/** AHB_DMA_OUT_CRC_FINAL_RESULT_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch2 of tx
 */
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH2    0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH2_M  (AHB_DMA_OUT_CRC_FINAL_RESULT_CH2_V << AHB_DMA_OUT_CRC_FINAL_RESULT_CH2_S)
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUT_CRC_FINAL_RESULT_CH2_S  0

/** AHB_DMA_TX_CRC_EN_WR_DATA_CH2_REG register
 *  This resister is used to config ch2 crc en for every bit
 */
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x31c)
/** AHB_DMA_TX_CRC_EN_WR_DATA_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable tx ch2 crc 32bit on/off
 */
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH2    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH2_M  (AHB_DMA_TX_CRC_EN_WR_DATA_CH2_V << AHB_DMA_TX_CRC_EN_WR_DATA_CH2_S)
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH2_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_WR_DATA_CH2_S  0

/** AHB_DMA_TX_CRC_EN_ADDR_CH2_REG register
 *  This register is used to config ch2 crc en addr
 */
#define AHB_DMA_TX_CRC_EN_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x320)
/** AHB_DMA_TX_CRC_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_EN_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_ADDR_CH2_M  (AHB_DMA_TX_CRC_EN_ADDR_CH2_V << AHB_DMA_TX_CRC_EN_ADDR_CH2_S)
#define AHB_DMA_TX_CRC_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_EN_ADDR_CH2_S  0

/** AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_REG register
 *  This register is used to config crc data_8bit en
 */
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x324)
/** AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2 : R/W; bitpos: [7:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2    0x000000FFU
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_M  (AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_V << AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_S)
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_V  0x000000FFU
#define AHB_DMA_TX_CRC_DATA_EN_WR_DATA_CH2_S  0

/** AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x328)
/** AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2_M  (AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2_V << AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2_S)
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_TX_CRC_DATA_EN_ADDR_CH2_S  0

/** AHB_DMA_TX_CH_ARB_WEIGH_CH2_REG register
 *  This register is used to config ch2 arbiter weigh
 */
#define AHB_DMA_TX_CH_ARB_WEIGH_CH2_REG (DR_REG_AHB_DMA_BASE + 0x32c)
/** AHB_DMA_TX_CH_ARB_WEIGH_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_CH_ARB_WEIGH_CH2    0x0000000FU
#define AHB_DMA_TX_CH_ARB_WEIGH_CH2_M  (AHB_DMA_TX_CH_ARB_WEIGH_CH2_V << AHB_DMA_TX_CH_ARB_WEIGH_CH2_S)
#define AHB_DMA_TX_CH_ARB_WEIGH_CH2_V  0x0000000FU
#define AHB_DMA_TX_CH_ARB_WEIGH_CH2_S  0

/** AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_REG register
 *  This register is used to config off or on weigh optimization
 */
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x330)
/** AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2 : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2    (BIT(0))
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_M  (AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_V << AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_S)
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_V  0x00000001U
#define AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_S  0

/** AHB_DMA_IN_CRC_INIT_DATA_CH0_REG register
 *  This register is used to config ch0 crc initial data(max 32 bit)
 */
#define AHB_DMA_IN_CRC_INIT_DATA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x334)
/** AHB_DMA_IN_CRC_INIT_DATA_CH0 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch0 of rx crc initial value
 */
#define AHB_DMA_IN_CRC_INIT_DATA_CH0    0xFFFFFFFFU
#define AHB_DMA_IN_CRC_INIT_DATA_CH0_M  (AHB_DMA_IN_CRC_INIT_DATA_CH0_V << AHB_DMA_IN_CRC_INIT_DATA_CH0_S)
#define AHB_DMA_IN_CRC_INIT_DATA_CH0_V  0xFFFFFFFFU
#define AHB_DMA_IN_CRC_INIT_DATA_CH0_S  0

/** AHB_DMA_RX_CRC_WIDTH_CH0_REG register
 *  This register is used to config rx ch0 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AHB_DMA_RX_CRC_WIDTH_CH0_REG (DR_REG_AHB_DMA_BASE + 0x338)
/** AHB_DMA_RX_CRC_WIDTH_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_WIDTH_CH0    0x00000003U
#define AHB_DMA_RX_CRC_WIDTH_CH0_M  (AHB_DMA_RX_CRC_WIDTH_CH0_V << AHB_DMA_RX_CRC_WIDTH_CH0_S)
#define AHB_DMA_RX_CRC_WIDTH_CH0_V  0x00000003U
#define AHB_DMA_RX_CRC_WIDTH_CH0_S  0
/** AHB_DMA_RX_CRC_LATCH_FLAG_CH0 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH0    (BIT(2))
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH0_M  (AHB_DMA_RX_CRC_LATCH_FLAG_CH0_V << AHB_DMA_RX_CRC_LATCH_FLAG_CH0_S)
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH0_V  0x00000001U
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH0_S  2

/** AHB_DMA_IN_CRC_CLEAR_CH0_REG register
 *  This register is used to clear ch0 crc result
 */
#define AHB_DMA_IN_CRC_CLEAR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x33c)
/** AHB_DMA_IN_CRC_CLEAR_CH0 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch0 of rx crc result
 */
#define AHB_DMA_IN_CRC_CLEAR_CH0    (BIT(0))
#define AHB_DMA_IN_CRC_CLEAR_CH0_M  (AHB_DMA_IN_CRC_CLEAR_CH0_V << AHB_DMA_IN_CRC_CLEAR_CH0_S)
#define AHB_DMA_IN_CRC_CLEAR_CH0_V  0x00000001U
#define AHB_DMA_IN_CRC_CLEAR_CH0_S  0

/** AHB_DMA_IN_CRC_FINAL_RESULT_CH0_REG register
 *  This register is used to store ch0 crc result
 */
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH0_REG (DR_REG_AHB_DMA_BASE + 0x340)
/** AHB_DMA_IN_CRC_FINAL_RESULT_CH0 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch0 of rx
 */
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH0    0xFFFFFFFFU
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH0_M  (AHB_DMA_IN_CRC_FINAL_RESULT_CH0_V << AHB_DMA_IN_CRC_FINAL_RESULT_CH0_S)
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH0_V  0xFFFFFFFFU
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH0_S  0

/** AHB_DMA_RX_CRC_EN_WR_DATA_CH0_REG register
 *  This resister is used to config ch0 crc en for every bit
 */
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x344)
/** AHB_DMA_RX_CRC_EN_WR_DATA_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable rx ch0 crc 32bit on/off
 */
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH0    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH0_M  (AHB_DMA_RX_CRC_EN_WR_DATA_CH0_V << AHB_DMA_RX_CRC_EN_WR_DATA_CH0_S)
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH0_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH0_S  0

/** AHB_DMA_RX_CRC_EN_ADDR_CH0_REG register
 *  This register is used to config ch0 crc en addr
 */
#define AHB_DMA_RX_CRC_EN_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x348)
/** AHB_DMA_RX_CRC_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_EN_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_ADDR_CH0_M  (AHB_DMA_RX_CRC_EN_ADDR_CH0_V << AHB_DMA_RX_CRC_EN_ADDR_CH0_S)
#define AHB_DMA_RX_CRC_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_ADDR_CH0_S  0

/** AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_REG register
 *  This register is used to config crc data_8bit en
 */
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_REG (DR_REG_AHB_DMA_BASE + 0x34c)
/** AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0 : R/W; bitpos: [7:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0    0x000000FFU
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_M  (AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_V << AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_S)
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_V  0x000000FFU
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH0_S  0

/** AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x350)
/** AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0_M  (AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0_V << AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0_S)
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH0_S  0

/** AHB_DMA_RX_CH_ARB_WEIGH_CH0_REG register
 *  This register is used to config ch0 arbiter weigh
 */
#define AHB_DMA_RX_CH_ARB_WEIGH_CH0_REG (DR_REG_AHB_DMA_BASE + 0x354)
/** AHB_DMA_RX_CH_ARB_WEIGH_CH0 : R/W; bitpos: [3:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CH_ARB_WEIGH_CH0    0x0000000FU
#define AHB_DMA_RX_CH_ARB_WEIGH_CH0_M  (AHB_DMA_RX_CH_ARB_WEIGH_CH0_V << AHB_DMA_RX_CH_ARB_WEIGH_CH0_S)
#define AHB_DMA_RX_CH_ARB_WEIGH_CH0_V  0x0000000FU
#define AHB_DMA_RX_CH_ARB_WEIGH_CH0_S  0

/** AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_REG register
 *  This register is used to config off or on weigh optimization
 */
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x358)
/** AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0 : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0    (BIT(0))
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_M  (AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_V << AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_S)
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_V  0x00000001U
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_S  0

/** AHB_DMA_IN_CRC_INIT_DATA_CH1_REG register
 *  This register is used to config ch1 crc initial data(max 32 bit)
 */
#define AHB_DMA_IN_CRC_INIT_DATA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x35c)
/** AHB_DMA_IN_CRC_INIT_DATA_CH1 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch1 of rx crc initial value
 */
#define AHB_DMA_IN_CRC_INIT_DATA_CH1    0xFFFFFFFFU
#define AHB_DMA_IN_CRC_INIT_DATA_CH1_M  (AHB_DMA_IN_CRC_INIT_DATA_CH1_V << AHB_DMA_IN_CRC_INIT_DATA_CH1_S)
#define AHB_DMA_IN_CRC_INIT_DATA_CH1_V  0xFFFFFFFFU
#define AHB_DMA_IN_CRC_INIT_DATA_CH1_S  0

/** AHB_DMA_RX_CRC_WIDTH_CH1_REG register
 *  This register is used to config rx ch1 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AHB_DMA_RX_CRC_WIDTH_CH1_REG (DR_REG_AHB_DMA_BASE + 0x360)
/** AHB_DMA_RX_CRC_WIDTH_CH1 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_WIDTH_CH1    0x00000003U
#define AHB_DMA_RX_CRC_WIDTH_CH1_M  (AHB_DMA_RX_CRC_WIDTH_CH1_V << AHB_DMA_RX_CRC_WIDTH_CH1_S)
#define AHB_DMA_RX_CRC_WIDTH_CH1_V  0x00000003U
#define AHB_DMA_RX_CRC_WIDTH_CH1_S  0
/** AHB_DMA_RX_CRC_LATCH_FLAG_CH1 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH1    (BIT(2))
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH1_M  (AHB_DMA_RX_CRC_LATCH_FLAG_CH1_V << AHB_DMA_RX_CRC_LATCH_FLAG_CH1_S)
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH1_V  0x00000001U
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH1_S  2

/** AHB_DMA_IN_CRC_CLEAR_CH1_REG register
 *  This register is used to clear ch1 crc result
 */
#define AHB_DMA_IN_CRC_CLEAR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x364)
/** AHB_DMA_IN_CRC_CLEAR_CH1 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch1 of rx crc result
 */
#define AHB_DMA_IN_CRC_CLEAR_CH1    (BIT(0))
#define AHB_DMA_IN_CRC_CLEAR_CH1_M  (AHB_DMA_IN_CRC_CLEAR_CH1_V << AHB_DMA_IN_CRC_CLEAR_CH1_S)
#define AHB_DMA_IN_CRC_CLEAR_CH1_V  0x00000001U
#define AHB_DMA_IN_CRC_CLEAR_CH1_S  0

/** AHB_DMA_IN_CRC_FINAL_RESULT_CH1_REG register
 *  This register is used to store ch1 crc result
 */
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH1_REG (DR_REG_AHB_DMA_BASE + 0x368)
/** AHB_DMA_IN_CRC_FINAL_RESULT_CH1 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch1 of rx
 */
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH1    0xFFFFFFFFU
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH1_M  (AHB_DMA_IN_CRC_FINAL_RESULT_CH1_V << AHB_DMA_IN_CRC_FINAL_RESULT_CH1_S)
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH1_V  0xFFFFFFFFU
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH1_S  0

/** AHB_DMA_RX_CRC_EN_WR_DATA_CH1_REG register
 *  This resister is used to config ch1 crc en for every bit
 */
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x36c)
/** AHB_DMA_RX_CRC_EN_WR_DATA_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable rx ch1 crc 32bit on/off
 */
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH1    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH1_M  (AHB_DMA_RX_CRC_EN_WR_DATA_CH1_V << AHB_DMA_RX_CRC_EN_WR_DATA_CH1_S)
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH1_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH1_S  0

/** AHB_DMA_RX_CRC_EN_ADDR_CH1_REG register
 *  This register is used to config ch1 crc en addr
 */
#define AHB_DMA_RX_CRC_EN_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x370)
/** AHB_DMA_RX_CRC_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_EN_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_ADDR_CH1_M  (AHB_DMA_RX_CRC_EN_ADDR_CH1_V << AHB_DMA_RX_CRC_EN_ADDR_CH1_S)
#define AHB_DMA_RX_CRC_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_ADDR_CH1_S  0

/** AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_REG register
 *  This register is used to config crc data_8bit en
 */
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_REG (DR_REG_AHB_DMA_BASE + 0x374)
/** AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1 : R/W; bitpos: [7:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1    0x000000FFU
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_M  (AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_V << AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_S)
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_V  0x000000FFU
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH1_S  0

/** AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x378)
/** AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1_M  (AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1_V << AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1_S)
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH1_S  0

/** AHB_DMA_RX_CH_ARB_WEIGH_CH1_REG register
 *  This register is used to config ch1 arbiter weigh
 */
#define AHB_DMA_RX_CH_ARB_WEIGH_CH1_REG (DR_REG_AHB_DMA_BASE + 0x37c)
/** AHB_DMA_RX_CH_ARB_WEIGH_CH1 : R/W; bitpos: [3:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CH_ARB_WEIGH_CH1    0x0000000FU
#define AHB_DMA_RX_CH_ARB_WEIGH_CH1_M  (AHB_DMA_RX_CH_ARB_WEIGH_CH1_V << AHB_DMA_RX_CH_ARB_WEIGH_CH1_S)
#define AHB_DMA_RX_CH_ARB_WEIGH_CH1_V  0x0000000FU
#define AHB_DMA_RX_CH_ARB_WEIGH_CH1_S  0

/** AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_REG register
 *  This register is used to config off or on weigh optimization
 */
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x380)
/** AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1 : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1    (BIT(0))
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_M  (AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_V << AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_S)
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_V  0x00000001U
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_S  0

/** AHB_DMA_IN_CRC_INIT_DATA_CH2_REG register
 *  This register is used to config ch2 crc initial data(max 32 bit)
 */
#define AHB_DMA_IN_CRC_INIT_DATA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x384)
/** AHB_DMA_IN_CRC_INIT_DATA_CH2 : R/W; bitpos: [31:0]; default: 4294967295;
 *  This register is used to config ch2 of rx crc initial value
 */
#define AHB_DMA_IN_CRC_INIT_DATA_CH2    0xFFFFFFFFU
#define AHB_DMA_IN_CRC_INIT_DATA_CH2_M  (AHB_DMA_IN_CRC_INIT_DATA_CH2_V << AHB_DMA_IN_CRC_INIT_DATA_CH2_S)
#define AHB_DMA_IN_CRC_INIT_DATA_CH2_V  0xFFFFFFFFU
#define AHB_DMA_IN_CRC_INIT_DATA_CH2_S  0

/** AHB_DMA_RX_CRC_WIDTH_CH2_REG register
 *  This register is used to config rx ch2 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
#define AHB_DMA_RX_CRC_WIDTH_CH2_REG (DR_REG_AHB_DMA_BASE + 0x388)
/** AHB_DMA_RX_CRC_WIDTH_CH2 : R/W; bitpos: [1:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_WIDTH_CH2    0x00000003U
#define AHB_DMA_RX_CRC_WIDTH_CH2_M  (AHB_DMA_RX_CRC_WIDTH_CH2_V << AHB_DMA_RX_CRC_WIDTH_CH2_S)
#define AHB_DMA_RX_CRC_WIDTH_CH2_V  0x00000003U
#define AHB_DMA_RX_CRC_WIDTH_CH2_S  0
/** AHB_DMA_RX_CRC_LATCH_FLAG_CH2 : R/W; bitpos: [2]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH2    (BIT(2))
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH2_M  (AHB_DMA_RX_CRC_LATCH_FLAG_CH2_V << AHB_DMA_RX_CRC_LATCH_FLAG_CH2_S)
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH2_V  0x00000001U
#define AHB_DMA_RX_CRC_LATCH_FLAG_CH2_S  2

/** AHB_DMA_IN_CRC_CLEAR_CH2_REG register
 *  This register is used to clear ch2 crc result
 */
#define AHB_DMA_IN_CRC_CLEAR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x38c)
/** AHB_DMA_IN_CRC_CLEAR_CH2 : R/W; bitpos: [0]; default: 0;
 *  This register is used to clear ch2 of rx crc result
 */
#define AHB_DMA_IN_CRC_CLEAR_CH2    (BIT(0))
#define AHB_DMA_IN_CRC_CLEAR_CH2_M  (AHB_DMA_IN_CRC_CLEAR_CH2_V << AHB_DMA_IN_CRC_CLEAR_CH2_S)
#define AHB_DMA_IN_CRC_CLEAR_CH2_V  0x00000001U
#define AHB_DMA_IN_CRC_CLEAR_CH2_S  0

/** AHB_DMA_IN_CRC_FINAL_RESULT_CH2_REG register
 *  This register is used to store ch2 crc result
 */
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH2_REG (DR_REG_AHB_DMA_BASE + 0x390)
/** AHB_DMA_IN_CRC_FINAL_RESULT_CH2 : RO; bitpos: [31:0]; default: 0;
 *  This register is used to store result ch2 of rx
 */
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH2    0xFFFFFFFFU
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH2_M  (AHB_DMA_IN_CRC_FINAL_RESULT_CH2_V << AHB_DMA_IN_CRC_FINAL_RESULT_CH2_S)
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH2_V  0xFFFFFFFFU
#define AHB_DMA_IN_CRC_FINAL_RESULT_CH2_S  0

/** AHB_DMA_RX_CRC_EN_WR_DATA_CH2_REG register
 *  This resister is used to config ch2 crc en for every bit
 */
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x394)
/** AHB_DMA_RX_CRC_EN_WR_DATA_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register is used to enable rx ch2 crc 32bit on/off
 */
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH2    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH2_M  (AHB_DMA_RX_CRC_EN_WR_DATA_CH2_V << AHB_DMA_RX_CRC_EN_WR_DATA_CH2_S)
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH2_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_WR_DATA_CH2_S  0

/** AHB_DMA_RX_CRC_EN_ADDR_CH2_REG register
 *  This register is used to config ch2 crc en addr
 */
#define AHB_DMA_RX_CRC_EN_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x398)
/** AHB_DMA_RX_CRC_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_EN_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_ADDR_CH2_M  (AHB_DMA_RX_CRC_EN_ADDR_CH2_V << AHB_DMA_RX_CRC_EN_ADDR_CH2_S)
#define AHB_DMA_RX_CRC_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_EN_ADDR_CH2_S  0

/** AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_REG register
 *  This register is used to config crc data_8bit en
 */
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_REG (DR_REG_AHB_DMA_BASE + 0x39c)
/** AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2 : R/W; bitpos: [7:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2    0x000000FFU
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_M  (AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_V << AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_S)
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_V  0x000000FFU
#define AHB_DMA_RX_CRC_DATA_EN_WR_DATA_CH2_S  0

/** AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2_REG register
 *  This register is used to config addr of crc data_8bit en
 */
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x3a0)
/** AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2_M  (AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2_V << AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2_S)
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_RX_CRC_DATA_EN_ADDR_CH2_S  0

/** AHB_DMA_RX_CH_ARB_WEIGH_CH2_REG register
 *  This register is used to config ch2 arbiter weigh
 */
#define AHB_DMA_RX_CH_ARB_WEIGH_CH2_REG (DR_REG_AHB_DMA_BASE + 0x3a4)
/** AHB_DMA_RX_CH_ARB_WEIGH_CH2 : R/W; bitpos: [3:0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_CH_ARB_WEIGH_CH2    0x0000000FU
#define AHB_DMA_RX_CH_ARB_WEIGH_CH2_M  (AHB_DMA_RX_CH_ARB_WEIGH_CH2_V << AHB_DMA_RX_CH_ARB_WEIGH_CH2_S)
#define AHB_DMA_RX_CH_ARB_WEIGH_CH2_V  0x0000000FU
#define AHB_DMA_RX_CH_ARB_WEIGH_CH2_S  0

/** AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_REG register
 *  This register is used to config off or on weigh optimization
 */
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x3a8)
/** AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2 : R/W; bitpos: [0]; default: 0;
 *  reserved
 */
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2    (BIT(0))
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_M  (AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_V << AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_S)
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_V  0x00000001U
#define AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_S  0

/** AHB_DMA_IN_LINK_ADDR_CH0_REG register
 *  Link descriptor configure of Rx channel 0
 */
#define AHB_DMA_IN_LINK_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x3ac)
/** AHB_DMA_INLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first inlink descriptor's
 *  address.
 */
#define AHB_DMA_INLINK_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_INLINK_ADDR_CH0_M  (AHB_DMA_INLINK_ADDR_CH0_V << AHB_DMA_INLINK_ADDR_CH0_S)
#define AHB_DMA_INLINK_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_ADDR_CH0_S  0

/** AHB_DMA_IN_LINK_ADDR_CH1_REG register
 *  Link descriptor configure of Rx channel 1
 */
#define AHB_DMA_IN_LINK_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x3b0)
/** AHB_DMA_INLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first inlink descriptor's
 *  address.
 */
#define AHB_DMA_INLINK_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_INLINK_ADDR_CH1_M  (AHB_DMA_INLINK_ADDR_CH1_V << AHB_DMA_INLINK_ADDR_CH1_S)
#define AHB_DMA_INLINK_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_ADDR_CH1_S  0

/** AHB_DMA_IN_LINK_ADDR_CH2_REG register
 *  Link descriptor configure of Rx channel 2
 */
#define AHB_DMA_IN_LINK_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x3b4)
/** AHB_DMA_INLINK_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first inlink descriptor's
 *  address.
 */
#define AHB_DMA_INLINK_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_INLINK_ADDR_CH2_M  (AHB_DMA_INLINK_ADDR_CH2_V << AHB_DMA_INLINK_ADDR_CH2_S)
#define AHB_DMA_INLINK_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_INLINK_ADDR_CH2_S  0

/** AHB_DMA_OUT_LINK_ADDR_CH0_REG register
 *  Link descriptor configure of Tx channel 0
 */
#define AHB_DMA_OUT_LINK_ADDR_CH0_REG (DR_REG_AHB_DMA_BASE + 0x3b8)
/** AHB_DMA_OUTLINK_ADDR_CH0 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first outlink
 *  descriptor's address.
 */
#define AHB_DMA_OUTLINK_ADDR_CH0    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_ADDR_CH0_M  (AHB_DMA_OUTLINK_ADDR_CH0_V << AHB_DMA_OUTLINK_ADDR_CH0_S)
#define AHB_DMA_OUTLINK_ADDR_CH0_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_ADDR_CH0_S  0

/** AHB_DMA_OUT_LINK_ADDR_CH1_REG register
 *  Link descriptor configure of Tx channel 1
 */
#define AHB_DMA_OUT_LINK_ADDR_CH1_REG (DR_REG_AHB_DMA_BASE + 0x3bc)
/** AHB_DMA_OUTLINK_ADDR_CH1 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first outlink
 *  descriptor's address.
 */
#define AHB_DMA_OUTLINK_ADDR_CH1    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_ADDR_CH1_M  (AHB_DMA_OUTLINK_ADDR_CH1_V << AHB_DMA_OUTLINK_ADDR_CH1_S)
#define AHB_DMA_OUTLINK_ADDR_CH1_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_ADDR_CH1_S  0

/** AHB_DMA_OUT_LINK_ADDR_CH2_REG register
 *  Link descriptor configure of Tx channel 2
 */
#define AHB_DMA_OUT_LINK_ADDR_CH2_REG (DR_REG_AHB_DMA_BASE + 0x3c0)
/** AHB_DMA_OUTLINK_ADDR_CH2 : R/W; bitpos: [31:0]; default: 0;
 *  This register stores the 32 least significant bits of the first outlink
 *  descriptor's address.
 */
#define AHB_DMA_OUTLINK_ADDR_CH2    0xFFFFFFFFU
#define AHB_DMA_OUTLINK_ADDR_CH2_M  (AHB_DMA_OUTLINK_ADDR_CH2_V << AHB_DMA_OUTLINK_ADDR_CH2_S)
#define AHB_DMA_OUTLINK_ADDR_CH2_V  0xFFFFFFFFU
#define AHB_DMA_OUTLINK_ADDR_CH2_S  0

/** AHB_DMA_INTR_MEM_START_ADDR_REG register
 *  The start address of accessible address space.
 */
#define AHB_DMA_INTR_MEM_START_ADDR_REG (DR_REG_AHB_DMA_BASE + 0x3c4)
/** AHB_DMA_ACCESS_INTR_MEM_START_ADDR : R/W; bitpos: [31:0]; default: 0;
 *  The start address of accessible address space.
 */
#define AHB_DMA_ACCESS_INTR_MEM_START_ADDR    0xFFFFFFFFU
#define AHB_DMA_ACCESS_INTR_MEM_START_ADDR_M  (AHB_DMA_ACCESS_INTR_MEM_START_ADDR_V << AHB_DMA_ACCESS_INTR_MEM_START_ADDR_S)
#define AHB_DMA_ACCESS_INTR_MEM_START_ADDR_V  0xFFFFFFFFU
#define AHB_DMA_ACCESS_INTR_MEM_START_ADDR_S  0

/** AHB_DMA_INTR_MEM_END_ADDR_REG register
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define AHB_DMA_INTR_MEM_END_ADDR_REG (DR_REG_AHB_DMA_BASE + 0x3c8)
/** AHB_DMA_ACCESS_INTR_MEM_END_ADDR : R/W; bitpos: [31:0]; default: 4294967295;
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
#define AHB_DMA_ACCESS_INTR_MEM_END_ADDR    0xFFFFFFFFU
#define AHB_DMA_ACCESS_INTR_MEM_END_ADDR_M  (AHB_DMA_ACCESS_INTR_MEM_END_ADDR_V << AHB_DMA_ACCESS_INTR_MEM_END_ADDR_S)
#define AHB_DMA_ACCESS_INTR_MEM_END_ADDR_V  0xFFFFFFFFU
#define AHB_DMA_ACCESS_INTR_MEM_END_ADDR_S  0

/** AHB_DMA_ARB_TIMEOUT_TX_REG register
 *  This retister is used to config arbiter time slice for tx dir
 */
#define AHB_DMA_ARB_TIMEOUT_TX_REG (DR_REG_AHB_DMA_BASE + 0x3cc)
/** AHB_DMA_ARB_TIMEOUT_TX : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to config arbiter time out value
 */
#define AHB_DMA_ARB_TIMEOUT_TX    0x0000FFFFU
#define AHB_DMA_ARB_TIMEOUT_TX_M  (AHB_DMA_ARB_TIMEOUT_TX_V << AHB_DMA_ARB_TIMEOUT_TX_S)
#define AHB_DMA_ARB_TIMEOUT_TX_V  0x0000FFFFU
#define AHB_DMA_ARB_TIMEOUT_TX_S  0

/** AHB_DMA_ARB_TIMEOUT_RX_REG register
 *  This retister is used to config arbiter time slice for rx dir
 */
#define AHB_DMA_ARB_TIMEOUT_RX_REG (DR_REG_AHB_DMA_BASE + 0x3d0)
/** AHB_DMA_ARB_TIMEOUT_RX : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to config arbiter time out value
 */
#define AHB_DMA_ARB_TIMEOUT_RX    0x0000FFFFU
#define AHB_DMA_ARB_TIMEOUT_RX_M  (AHB_DMA_ARB_TIMEOUT_RX_V << AHB_DMA_ARB_TIMEOUT_RX_S)
#define AHB_DMA_ARB_TIMEOUT_RX_V  0x0000FFFFU
#define AHB_DMA_ARB_TIMEOUT_RX_S  0

/** AHB_DMA_WEIGHT_EN_TX_REG register
 *  This register is used to config arbiter weigh function to on or off for tx dir
 */
#define AHB_DMA_WEIGHT_EN_TX_REG (DR_REG_AHB_DMA_BASE + 0x3d4)
/** AHB_DMA_WEIGHT_EN_TX : R/W; bitpos: [0]; default: 0;
 *  This register is used to config arbiter weight function  off/on
 */
#define AHB_DMA_WEIGHT_EN_TX    (BIT(0))
#define AHB_DMA_WEIGHT_EN_TX_M  (AHB_DMA_WEIGHT_EN_TX_V << AHB_DMA_WEIGHT_EN_TX_S)
#define AHB_DMA_WEIGHT_EN_TX_V  0x00000001U
#define AHB_DMA_WEIGHT_EN_TX_S  0

/** AHB_DMA_WEIGHT_EN_RX_REG register
 *  This register is used to config arbiter weigh function to on or off for rx dir
 */
#define AHB_DMA_WEIGHT_EN_RX_REG (DR_REG_AHB_DMA_BASE + 0x3d8)
/** AHB_DMA_WEIGHT_EN_RX : R/W; bitpos: [0]; default: 0;
 *  This register is used to config arbiter weight function  off/on
 */
#define AHB_DMA_WEIGHT_EN_RX    (BIT(0))
#define AHB_DMA_WEIGHT_EN_RX_M  (AHB_DMA_WEIGHT_EN_RX_V << AHB_DMA_WEIGHT_EN_RX_S)
#define AHB_DMA_WEIGHT_EN_RX_V  0x00000001U
#define AHB_DMA_WEIGHT_EN_RX_S  0

#ifdef __cplusplus
}
#endif
