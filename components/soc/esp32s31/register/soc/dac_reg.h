/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** DAC_PAD_CFG_REG register
 *  configure dac pad register
 */
#define DAC_PAD_CFG_REG (DR_REG_DAC_BASE + 0x0)
/** DAC_HS_MODE_PAD_0 : R/W; bitpos: [0]; default: 0;
 *  1 to let DAC PAD 0 enter High speed mode
 */
#define DAC_HS_MODE_PAD_0    (BIT(0))
#define DAC_HS_MODE_PAD_0_M  (DAC_HS_MODE_PAD_0_V << DAC_HS_MODE_PAD_0_S)
#define DAC_HS_MODE_PAD_0_V  0x00000001U
#define DAC_HS_MODE_PAD_0_S  0
/** DAC_XPD_PAD_0 : R/W; bitpos: [1]; default: 0;
 *  1 to power on DAC PAD 0
 */
#define DAC_XPD_PAD_0    (BIT(1))
#define DAC_XPD_PAD_0_M  (DAC_XPD_PAD_0_V << DAC_XPD_PAD_0_S)
#define DAC_XPD_PAD_0_V  0x00000001U
#define DAC_XPD_PAD_0_S  1
/** DAC_EN_ADC_MUX_PAD_0 : R/W; bitpos: [2]; default: 0;
 *  1 to enable adc mux for DAC PAD 0
 */
#define DAC_EN_ADC_MUX_PAD_0    (BIT(2))
#define DAC_EN_ADC_MUX_PAD_0_M  (DAC_EN_ADC_MUX_PAD_0_V << DAC_EN_ADC_MUX_PAD_0_S)
#define DAC_EN_ADC_MUX_PAD_0_V  0x00000001U
#define DAC_EN_ADC_MUX_PAD_0_S  2
/** DAC_DSAMP_LS_PAD_0 : R/W; bitpos: [3]; default: 0;
 *  1 to start sample and hold mode for DAC PAD 0
 */
#define DAC_DSAMP_LS_PAD_0    (BIT(3))
#define DAC_DSAMP_LS_PAD_0_M  (DAC_DSAMP_LS_PAD_0_V << DAC_DSAMP_LS_PAD_0_S)
#define DAC_DSAMP_LS_PAD_0_V  0x00000001U
#define DAC_DSAMP_LS_PAD_0_S  3
/** DAC_DCAL_LS_PAD_0 : R/W; bitpos: [4]; default: 0;
 *  1 to Calibrate the buffer of DAC PAD 0 (DBUF of pad 1 must be 1 during this
 *  operation)
 */
#define DAC_DCAL_LS_PAD_0    (BIT(4))
#define DAC_DCAL_LS_PAD_0_M  (DAC_DCAL_LS_PAD_0_V << DAC_DCAL_LS_PAD_0_S)
#define DAC_DCAL_LS_PAD_0_V  0x00000001U
#define DAC_DCAL_LS_PAD_0_S  4
/** DAC_DBUF_LS_PAD_0 : R/W; bitpos: [5]; default: 0;
 *  1 to turn on the buffer of DAC PAD 0
 */
#define DAC_DBUF_LS_PAD_0    (BIT(5))
#define DAC_DBUF_LS_PAD_0_M  (DAC_DBUF_LS_PAD_0_V << DAC_DBUF_LS_PAD_0_S)
#define DAC_DBUF_LS_PAD_0_V  0x00000001U
#define DAC_DBUF_LS_PAD_0_S  5
/** DAC_ON_LS_PAD_0 : R/W; bitpos: [6]; default: 0;
 *  1 to turn on the DAC PAD 0 during sample and hold mode
 */
#define DAC_ON_LS_PAD_0    (BIT(6))
#define DAC_ON_LS_PAD_0_M  (DAC_ON_LS_PAD_0_V << DAC_ON_LS_PAD_0_S)
#define DAC_ON_LS_PAD_0_V  0x00000001U
#define DAC_ON_LS_PAD_0_S  6
/** DAC_HS_MODE_PAD_1 : R/W; bitpos: [7]; default: 0;
 *  1 to let DAC PAD 1 enter High speed mode
 */
#define DAC_HS_MODE_PAD_1    (BIT(7))
#define DAC_HS_MODE_PAD_1_M  (DAC_HS_MODE_PAD_1_V << DAC_HS_MODE_PAD_1_S)
#define DAC_HS_MODE_PAD_1_V  0x00000001U
#define DAC_HS_MODE_PAD_1_S  7
/** DAC_XPD_PAD_1 : R/W; bitpos: [8]; default: 0;
 *  1 to power on DAC PAD 1
 */
#define DAC_XPD_PAD_1    (BIT(8))
#define DAC_XPD_PAD_1_M  (DAC_XPD_PAD_1_V << DAC_XPD_PAD_1_S)
#define DAC_XPD_PAD_1_V  0x00000001U
#define DAC_XPD_PAD_1_S  8
/** DAC_EN_ADC_MUX_PAD_1 : R/W; bitpos: [9]; default: 0;
 *  1 to enable adc mux for DAC PAD 1
 */
#define DAC_EN_ADC_MUX_PAD_1    (BIT(9))
#define DAC_EN_ADC_MUX_PAD_1_M  (DAC_EN_ADC_MUX_PAD_1_V << DAC_EN_ADC_MUX_PAD_1_S)
#define DAC_EN_ADC_MUX_PAD_1_V  0x00000001U
#define DAC_EN_ADC_MUX_PAD_1_S  9
/** DAC_DSAMP_LS_PAD_1 : R/W; bitpos: [10]; default: 0;
 *  1 to start sample and hold mode for DAC PAD 1
 */
#define DAC_DSAMP_LS_PAD_1    (BIT(10))
#define DAC_DSAMP_LS_PAD_1_M  (DAC_DSAMP_LS_PAD_1_V << DAC_DSAMP_LS_PAD_1_S)
#define DAC_DSAMP_LS_PAD_1_V  0x00000001U
#define DAC_DSAMP_LS_PAD_1_S  10
/** DAC_DCAL_LS_PAD_1 : R/W; bitpos: [11]; default: 0;
 *  1 to Calibrate the buffer of DAC PAD 1 (DBUF of pad 1 must be 1 during this
 *  operation)
 */
#define DAC_DCAL_LS_PAD_1    (BIT(11))
#define DAC_DCAL_LS_PAD_1_M  (DAC_DCAL_LS_PAD_1_V << DAC_DCAL_LS_PAD_1_S)
#define DAC_DCAL_LS_PAD_1_V  0x00000001U
#define DAC_DCAL_LS_PAD_1_S  11
/** DAC_DBUF_LS_PAD_1 : R/W; bitpos: [12]; default: 0;
 *  1 to turn on the buffer of DAC PAD 1
 */
#define DAC_DBUF_LS_PAD_1    (BIT(12))
#define DAC_DBUF_LS_PAD_1_M  (DAC_DBUF_LS_PAD_1_V << DAC_DBUF_LS_PAD_1_S)
#define DAC_DBUF_LS_PAD_1_V  0x00000001U
#define DAC_DBUF_LS_PAD_1_S  12
/** DAC_ON_LS_PAD_1 : R/W; bitpos: [13]; default: 0;
 *  1 to turn on the DAC PAD 1 during sample and hold mode
 */
#define DAC_ON_LS_PAD_1    (BIT(13))
#define DAC_ON_LS_PAD_1_M  (DAC_ON_LS_PAD_1_V << DAC_ON_LS_PAD_1_S)
#define DAC_ON_LS_PAD_1_V  0x00000001U
#define DAC_ON_LS_PAD_1_S  13
/** DAC_DIV_NUM : R/W; bitpos: [29:14]; default: 1;
 *  divide number between clk ls and clk hs
 */
#define DAC_DIV_NUM    0x0000FFFFU
#define DAC_DIV_NUM_M  (DAC_DIV_NUM_V << DAC_DIV_NUM_S)
#define DAC_DIV_NUM_V  0x0000FFFFU
#define DAC_DIV_NUM_S  14
/** DAC_DBUF_RANGE_LS_PAD_0 : R/W; bitpos: [30]; default: 0;
 *  1:when buffer is activated, allow using full code of maximum bitwidth
 *  0:when buffer is not activated, should always be 0
 */
#define DAC_DBUF_RANGE_LS_PAD_0    (BIT(30))
#define DAC_DBUF_RANGE_LS_PAD_0_M  (DAC_DBUF_RANGE_LS_PAD_0_V << DAC_DBUF_RANGE_LS_PAD_0_S)
#define DAC_DBUF_RANGE_LS_PAD_0_V  0x00000001U
#define DAC_DBUF_RANGE_LS_PAD_0_S  30
/** DAC_DBUF_RANGE_LS_PAD_1 : R/W; bitpos: [31]; default: 0;
 *  1:when buffer is activated, allow using full code of maximum bitwidth
 *  0:when buffer is not activated, should always be 0
 */
#define DAC_DBUF_RANGE_LS_PAD_1    (BIT(31))
#define DAC_DBUF_RANGE_LS_PAD_1_M  (DAC_DBUF_RANGE_LS_PAD_1_V << DAC_DBUF_RANGE_LS_PAD_1_S)
#define DAC_DBUF_RANGE_LS_PAD_1_V  0x00000001U
#define DAC_DBUF_RANGE_LS_PAD_1_S  31

/** DAC_CALI_RESULT_REG register
 *  DAC CALI result register
 */
#define DAC_CALI_RESULT_REG (DR_REG_DAC_BASE + 0x4)
/** DAC_CALI_RESULT_PAD_0 : R/W; bitpos: [11:0]; default: 0;
 *  cali result register for pad 0 output
 */
#define DAC_CALI_RESULT_PAD_0    0x00000FFFU
#define DAC_CALI_RESULT_PAD_0_M  (DAC_CALI_RESULT_PAD_0_V << DAC_CALI_RESULT_PAD_0_S)
#define DAC_CALI_RESULT_PAD_0_V  0x00000FFFU
#define DAC_CALI_RESULT_PAD_0_S  0
/** DAC_CALI_RESULT_PAD_1 : R/W; bitpos: [23:12]; default: 0;
 *  cali result register for pad 1 output
 */
#define DAC_CALI_RESULT_PAD_1    0x00000FFFU
#define DAC_CALI_RESULT_PAD_1_M  (DAC_CALI_RESULT_PAD_1_V << DAC_CALI_RESULT_PAD_1_S)
#define DAC_CALI_RESULT_PAD_1_V  0x00000FFFU
#define DAC_CALI_RESULT_PAD_1_S  12

/** DAC_PDMA_CFG_REG register
 *  dac cfg register
 */
#define DAC_PDMA_CFG_REG (DR_REG_DAC_BASE + 0x8)
/** DAC_PDMA_ALTER_MODE : R/W; bitpos: [0]; default: 0;
 *  1:make rotation between two pads when outputting data of ppdma input path requires
 *  reg_dac_1/0_data_sel = 1
 *  0:no change
 */
#define DAC_PDMA_ALTER_MODE    (BIT(0))
#define DAC_PDMA_ALTER_MODE_M  (DAC_PDMA_ALTER_MODE_V << DAC_PDMA_ALTER_MODE_S)
#define DAC_PDMA_ALTER_MODE_V  0x00000001U
#define DAC_PDMA_ALTER_MODE_S  0
/** DAC_PDMA_RESET_FIFO : WT; bitpos: [1]; default: 0;
 *  1:reset dac fifo
 *  0:no change
 */
#define DAC_PDMA_RESET_FIFO    (BIT(1))
#define DAC_PDMA_RESET_FIFO_M  (DAC_PDMA_RESET_FIFO_V << DAC_PDMA_RESET_FIFO_S)
#define DAC_PDMA_RESET_FIFO_V  0x00000001U
#define DAC_PDMA_RESET_FIFO_S  1
/** DAC_PDMA_TRANS : R/W; bitpos: [2]; default: 0;
 *  1:start transaction for pdma path output
 *  0:stop transaction for pdma path output
 */
#define DAC_PDMA_TRANS    (BIT(2))
#define DAC_PDMA_TRANS_M  (DAC_PDMA_TRANS_V << DAC_PDMA_TRANS_S)
#define DAC_PDMA_TRANS_V  0x00000001U
#define DAC_PDMA_TRANS_S  2

/** DAC_PDMA_TIMER_CFG_REG register
 *  PDMA path timer register
 */
#define DAC_PDMA_TIMER_CFG_REG (DR_REG_DAC_BASE + 0xc)
/** DAC_PDMA_TIMER_EN : R/W; bitpos: [0]; default: 0;
 *  1:enables using dac timer to reduce output frequency
 *  0:no data output
 */
#define DAC_PDMA_TIMER_EN    (BIT(0))
#define DAC_PDMA_TIMER_EN_M  (DAC_PDMA_TIMER_EN_V << DAC_PDMA_TIMER_EN_S)
#define DAC_PDMA_TIMER_EN_V  0x00000001U
#define DAC_PDMA_TIMER_EN_S  0
/** DAC_PDMA_TIMER_TARGET : R/W; bitpos: [24:1]; default: 2;
 *  dac pdma timer wait target
 */
#define DAC_PDMA_TIMER_TARGET    0x00FFFFFFU
#define DAC_PDMA_TIMER_TARGET_M  (DAC_PDMA_TIMER_TARGET_V << DAC_PDMA_TIMER_TARGET_S)
#define DAC_PDMA_TIMER_TARGET_V  0x00FFFFFFU
#define DAC_PDMA_TIMER_TARGET_S  1

/** DAC_DATA_OUTPUT_CFG_REG register
 *  dac DATA OUTPUT cfg register
 */
#define DAC_DATA_OUTPUT_CFG_REG (DR_REG_DAC_BASE + 0x10)
/** DAC_PAD_0_DATA_SEL : R/W; bitpos: [0]; default: 0;
 *  1:output pdma data
 *  0:output sintx data
 */
#define DAC_PAD_0_DATA_SEL    (BIT(0))
#define DAC_PAD_0_DATA_SEL_M  (DAC_PAD_0_DATA_SEL_V << DAC_PAD_0_DATA_SEL_S)
#define DAC_PAD_0_DATA_SEL_V  0x00000001U
#define DAC_PAD_0_DATA_SEL_S  0
/** DAC_PAD_1_DATA_SEL : R/W; bitpos: [1]; default: 0;
 *  1:output pdma data
 *  0:output sintx data
 */
#define DAC_PAD_1_DATA_SEL    (BIT(1))
#define DAC_PAD_1_DATA_SEL_M  (DAC_PAD_1_DATA_SEL_V << DAC_PAD_1_DATA_SEL_S)
#define DAC_PAD_1_DATA_SEL_V  0x00000001U
#define DAC_PAD_1_DATA_SEL_S  1

/** DAC_SINTX_CFG_REG register
 *  dac rstn register
 */
#define DAC_SINTX_CFG_REG (DR_REG_DAC_BASE + 0x14)
/** DAC_SW_TONE : R/W; bitpos: [0]; default: 0;
 *  1:enable software adjusting current sintx output data
 *  0:disable software adjustment
 */
#define DAC_SW_TONE    (BIT(0))
#define DAC_SW_TONE_M  (DAC_SW_TONE_V << DAC_SW_TONE_S)
#define DAC_SW_TONE_V  0x00000001U
#define DAC_SW_TONE_S  0
/** DAC_SW_FSTEP : R/W; bitpos: [16:1]; default: 1;
 *  software adjust angle velocity foot step for sintx LUT input
 */
#define DAC_SW_FSTEP    0x0000FFFFU
#define DAC_SW_FSTEP_M  (DAC_SW_FSTEP_V << DAC_SW_FSTEP_S)
#define DAC_SW_FSTEP_V  0x0000FFFFU
#define DAC_SW_FSTEP_S  1
/** DAC_SCALE_1 : R/W; bitpos: [18:17]; default: 0;
 *  right shift scaling config for LUT output of sintx for pad 0
 */
#define DAC_SCALE_1    0x00000003U
#define DAC_SCALE_1_M  (DAC_SCALE_1_V << DAC_SCALE_1_S)
#define DAC_SCALE_1_V  0x00000003U
#define DAC_SCALE_1_S  17
/** DAC_SCALE_2 : R/W; bitpos: [20:19]; default: 0;
 *  software adjust angle velocity foot step for sintx LUT input
 */
#define DAC_SCALE_2    0x00000003U
#define DAC_SCALE_2_M  (DAC_SCALE_2_V << DAC_SCALE_2_S)
#define DAC_SCALE_2_V  0x00000003U
#define DAC_SCALE_2_S  19
/** DAC_INV_1 : R/W; bitpos: [22:21]; default: 0;
 *  bit 1: invert the highest bit of output for pad 0
 *  Bit 0: invert the entire output for pad 0
 */
#define DAC_INV_1    0x00000003U
#define DAC_INV_1_M  (DAC_INV_1_V << DAC_INV_1_S)
#define DAC_INV_1_V  0x00000003U
#define DAC_INV_1_S  21
/** DAC_INV_2 : R/W; bitpos: [24:23]; default: 0;
 *  bit 1: invert the highest bit of output for pad 1
 *  Bit 0: invert the entire output for pad 1
 */
#define DAC_INV_2    0x00000003U
#define DAC_INV_2_M  (DAC_INV_2_V << DAC_INV_2_S)
#define DAC_INV_2_V  0x00000003U
#define DAC_INV_2_S  23

/** DAC_SINTX_TIMER_CFG_REG register
 *  SINTX path timer register
 */
#define DAC_SINTX_TIMER_CFG_REG (DR_REG_DAC_BASE + 0x18)
/** DAC_SINTX_TIMER_EN : R/W; bitpos: [0]; default: 0;
 *  1:enables using dac timer to reduce output frequency
 *  0:no data output
 */
#define DAC_SINTX_TIMER_EN    (BIT(0))
#define DAC_SINTX_TIMER_EN_M  (DAC_SINTX_TIMER_EN_V << DAC_SINTX_TIMER_EN_S)
#define DAC_SINTX_TIMER_EN_V  0x00000001U
#define DAC_SINTX_TIMER_EN_S  0
/** DAC_SINTX_TIMER_TARGET : R/W; bitpos: [24:1]; default: 2;
 *  dac sintx timer wait target
 */
#define DAC_SINTX_TIMER_TARGET    0x00FFFFFFU
#define DAC_SINTX_TIMER_TARGET_M  (DAC_SINTX_TIMER_TARGET_V << DAC_SINTX_TIMER_TARGET_S)
#define DAC_SINTX_TIMER_TARGET_V  0x00FFFFFFU
#define DAC_SINTX_TIMER_TARGET_S  1

/** DAC_SINTX_DATA_REG register
 *  dac output register for sintx path
 */
#define DAC_SINTX_DATA_REG (DR_REG_DAC_BASE + 0x1c)
/** DAC_DC_1 : R/W; bitpos: [11:0]; default: 0;
 *  the data output for DAC PAD0 for sintx path
 */
#define DAC_DC_1    0x00000FFFU
#define DAC_DC_1_M  (DAC_DC_1_V << DAC_DC_1_S)
#define DAC_DC_1_V  0x00000FFFU
#define DAC_DC_1_S  0
/** DAC_DC_2 : R/W; bitpos: [23:12]; default: 0;
 *  the data output for DAC PAD1 for sintx path
 */
#define DAC_DC_2    0x00000FFFU
#define DAC_DC_2_M  (DAC_DC_2_V << DAC_DC_2_S)
#define DAC_DC_2_V  0x00000FFFU
#define DAC_DC_2_S  12

/** DAC_CALI_REG register
 *  cali algorithm register for DAC
 */
#define DAC_CALI_REG (DR_REG_DAC_BASE + 0x20)
/** DAC_CALI_OUT_PAD_0 : RO; bitpos: [0]; default: 0;
 *  cali out from DAC PAD 0
 */
#define DAC_CALI_OUT_PAD_0    (BIT(0))
#define DAC_CALI_OUT_PAD_0_M  (DAC_CALI_OUT_PAD_0_V << DAC_CALI_OUT_PAD_0_S)
#define DAC_CALI_OUT_PAD_0_V  0x00000001U
#define DAC_CALI_OUT_PAD_0_S  0
/** DAC_CALI_OUT_PAD_1 : RO; bitpos: [1]; default: 0;
 *  cali out from DAC PAD 1
 */
#define DAC_CALI_OUT_PAD_1    (BIT(1))
#define DAC_CALI_OUT_PAD_1_M  (DAC_CALI_OUT_PAD_1_V << DAC_CALI_OUT_PAD_1_S)
#define DAC_CALI_OUT_PAD_1_V  0x00000001U
#define DAC_CALI_OUT_PAD_1_S  1

/** DAC_SAMPLE_WAIT_CFG_REG register
 *  cali sample phase duration register
 */
#define DAC_SAMPLE_WAIT_CFG_REG (DR_REG_DAC_BASE + 0x24)
/** DAC_WAIT_TARGET_SAMPLE_PAD_0 : R/W; bitpos: [15:0]; default: 630;
 *  sample wait target for DAC PAD 1
 */
#define DAC_WAIT_TARGET_SAMPLE_PAD_0    0x0000FFFFU
#define DAC_WAIT_TARGET_SAMPLE_PAD_0_M  (DAC_WAIT_TARGET_SAMPLE_PAD_0_V << DAC_WAIT_TARGET_SAMPLE_PAD_0_S)
#define DAC_WAIT_TARGET_SAMPLE_PAD_0_V  0x0000FFFFU
#define DAC_WAIT_TARGET_SAMPLE_PAD_0_S  0
/** DAC_WAIT_TARGET_SAMPLE_PAD_1 : R/W; bitpos: [31:16]; default: 630;
 *  sample wait target for DAC PAD 0
 */
#define DAC_WAIT_TARGET_SAMPLE_PAD_1    0x0000FFFFU
#define DAC_WAIT_TARGET_SAMPLE_PAD_1_M  (DAC_WAIT_TARGET_SAMPLE_PAD_1_V << DAC_WAIT_TARGET_SAMPLE_PAD_1_S)
#define DAC_WAIT_TARGET_SAMPLE_PAD_1_V  0x0000FFFFU
#define DAC_WAIT_TARGET_SAMPLE_PAD_1_S  16

/** DAC_HOLD_WAIT_CFG_REG register
 *  cali hold phase duration register
 */
#define DAC_HOLD_WAIT_CFG_REG (DR_REG_DAC_BASE + 0x28)
/** DAC_WAIT_TARGET_HOLD_PAD_0 : R/W; bitpos: [15:0]; default: 61;
 *  hold wait target for DAC PAD 0
 */
#define DAC_WAIT_TARGET_HOLD_PAD_0    0x0000FFFFU
#define DAC_WAIT_TARGET_HOLD_PAD_0_M  (DAC_WAIT_TARGET_HOLD_PAD_0_V << DAC_WAIT_TARGET_HOLD_PAD_0_S)
#define DAC_WAIT_TARGET_HOLD_PAD_0_V  0x0000FFFFU
#define DAC_WAIT_TARGET_HOLD_PAD_0_S  0
/** DAC_WAIT_TARGET_HOLD_PAD_1 : R/W; bitpos: [31:16]; default: 61;
 *  hold wait target for DAC PAD 1
 */
#define DAC_WAIT_TARGET_HOLD_PAD_1    0x0000FFFFU
#define DAC_WAIT_TARGET_HOLD_PAD_1_M  (DAC_WAIT_TARGET_HOLD_PAD_1_V << DAC_WAIT_TARGET_HOLD_PAD_1_S)
#define DAC_WAIT_TARGET_HOLD_PAD_1_V  0x0000FFFFU
#define DAC_WAIT_TARGET_HOLD_PAD_1_S  16

/** DAC_REFRESH_WAIT_CFG_REG register
 *  cali refresh phase duration register
 */
#define DAC_REFRESH_WAIT_CFG_REG (DR_REG_DAC_BASE + 0x2c)
/** DAC_WAIT_TARGET_REFRESH_PAD_0 : R/W; bitpos: [15:0]; default: 8;
 *  refresh wait target for DAC PAD 0
 */
#define DAC_WAIT_TARGET_REFRESH_PAD_0    0x0000FFFFU
#define DAC_WAIT_TARGET_REFRESH_PAD_0_M  (DAC_WAIT_TARGET_REFRESH_PAD_0_V << DAC_WAIT_TARGET_REFRESH_PAD_0_S)
#define DAC_WAIT_TARGET_REFRESH_PAD_0_V  0x0000FFFFU
#define DAC_WAIT_TARGET_REFRESH_PAD_0_S  0
/** DAC_WAIT_TARGET_REFRESH_PAD_1 : R/W; bitpos: [31:16]; default: 8;
 *  refresh wait target for DAC PAD 1
 */
#define DAC_WAIT_TARGET_REFRESH_PAD_1    0x0000FFFFU
#define DAC_WAIT_TARGET_REFRESH_PAD_1_M  (DAC_WAIT_TARGET_REFRESH_PAD_1_V << DAC_WAIT_TARGET_REFRESH_PAD_1_S)
#define DAC_WAIT_TARGET_REFRESH_PAD_1_V  0x0000FFFFU
#define DAC_WAIT_TARGET_REFRESH_PAD_1_S  16

/** DAC_PDMA_INT_RAW_REG register
 *  need_des
 */
#define DAC_PDMA_INT_RAW_REG (DR_REG_DAC_BASE + 0x30)
/** DAC_PDMA_EMPTY_INT_RAW : R/WTC/SS; bitpos: [31]; default: 0;
 *  need_des
 */
#define DAC_PDMA_EMPTY_INT_RAW    (BIT(31))
#define DAC_PDMA_EMPTY_INT_RAW_M  (DAC_PDMA_EMPTY_INT_RAW_V << DAC_PDMA_EMPTY_INT_RAW_S)
#define DAC_PDMA_EMPTY_INT_RAW_V  0x00000001U
#define DAC_PDMA_EMPTY_INT_RAW_S  31

/** DAC_INT_ST_REG register
 *  need_des
 */
#define DAC_INT_ST_REG (DR_REG_DAC_BASE + 0x34)
/** DAC_PDMA_EMPTY_INT_ST : RO; bitpos: [31]; default: 0;
 *  need_des
 */
#define DAC_PDMA_EMPTY_INT_ST    (BIT(31))
#define DAC_PDMA_EMPTY_INT_ST_M  (DAC_PDMA_EMPTY_INT_ST_V << DAC_PDMA_EMPTY_INT_ST_S)
#define DAC_PDMA_EMPTY_INT_ST_V  0x00000001U
#define DAC_PDMA_EMPTY_INT_ST_S  31

/** DAC_INT_ENA_REG register
 *  need_des
 */
#define DAC_INT_ENA_REG (DR_REG_DAC_BASE + 0x38)
/** DAC_PDMA_EMPTY_INT_ENA : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define DAC_PDMA_EMPTY_INT_ENA    (BIT(31))
#define DAC_PDMA_EMPTY_INT_ENA_M  (DAC_PDMA_EMPTY_INT_ENA_V << DAC_PDMA_EMPTY_INT_ENA_S)
#define DAC_PDMA_EMPTY_INT_ENA_V  0x00000001U
#define DAC_PDMA_EMPTY_INT_ENA_S  31

/** DAC_INT_CLR_REG register
 *  need_des
 */
#define DAC_INT_CLR_REG (DR_REG_DAC_BASE + 0x3c)
/** DAC_PDMA_EMPTY_INT_CLR : WT; bitpos: [31]; default: 0;
 *  need_des
 */
#define DAC_PDMA_EMPTY_INT_CLR    (BIT(31))
#define DAC_PDMA_EMPTY_INT_CLR_M  (DAC_PDMA_EMPTY_INT_CLR_V << DAC_PDMA_EMPTY_INT_CLR_S)
#define DAC_PDMA_EMPTY_INT_CLR_V  0x00000001U
#define DAC_PDMA_EMPTY_INT_CLR_S  31

/** DAC_DATE_REG register
 *  need_des
 */
#define DAC_DATE_REG (DR_REG_DAC_BASE + 0x3fc)
/** DAC_DATE : R/W; bitpos: [30:0]; default: 2426393;
 *  need_des
 */
#define DAC_DATE    0x7FFFFFFFU
#define DAC_DATE_M  (DAC_DATE_V << DAC_DATE_S)
#define DAC_DATE_V  0x7FFFFFFFU
#define DAC_DATE_S  0
/** DAC_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define DAC_CLK_EN    (BIT(31))
#define DAC_CLK_EN_M  (DAC_CLK_EN_V << DAC_CLK_EN_S)
#define DAC_CLK_EN_V  0x00000001U
#define DAC_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
