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

/** LP_I2S_VAD_CONF_REG register
 *  I2S VAD Configure register
 */
#define LP_I2S_VAD_CONF_REG (DR_REG_LP_I2S_BASE + 0x0)
/** LP_I2S_VAD_EN : R/W; bitpos: [0]; default: 0;
 *  VAD enable register
 */
#define LP_I2S_VAD_EN    (BIT(0))
#define LP_I2S_VAD_EN_M  (LP_I2S_VAD_EN_V << LP_I2S_VAD_EN_S)
#define LP_I2S_VAD_EN_V  0x00000001U
#define LP_I2S_VAD_EN_S  0
/** LP_I2S_VAD_RESET : WT; bitpos: [1]; default: 0;
 *  VAD reset register
 */
#define LP_I2S_VAD_RESET    (BIT(1))
#define LP_I2S_VAD_RESET_M  (LP_I2S_VAD_RESET_V << LP_I2S_VAD_RESET_S)
#define LP_I2S_VAD_RESET_V  0x00000001U
#define LP_I2S_VAD_RESET_S  1
/** LP_I2S_VAD_FORCE_START : WT; bitpos: [2]; default: 0;
 *  VAD force start register.
 */
#define LP_I2S_VAD_FORCE_START    (BIT(2))
#define LP_I2S_VAD_FORCE_START_M  (LP_I2S_VAD_FORCE_START_V << LP_I2S_VAD_FORCE_START_S)
#define LP_I2S_VAD_FORCE_START_V  0x00000001U
#define LP_I2S_VAD_FORCE_START_S  2

/** LP_I2S_VAD_RESULT_REG register
 *  I2S VAD Result register
 */
#define LP_I2S_VAD_RESULT_REG (DR_REG_LP_I2S_BASE + 0x4)
/** LP_I2S_VAD_FLAG : RO; bitpos: [0]; default: 0;
 *  Reg vad flag observe signal
 */
#define LP_I2S_VAD_FLAG    (BIT(0))
#define LP_I2S_VAD_FLAG_M  (LP_I2S_VAD_FLAG_V << LP_I2S_VAD_FLAG_S)
#define LP_I2S_VAD_FLAG_V  0x00000001U
#define LP_I2S_VAD_FLAG_S  0
/** LP_I2S_ENERGY_ENOUGH : RO; bitpos: [1]; default: 0;
 *  Reg energy enough observe signal
 */
#define LP_I2S_ENERGY_ENOUGH    (BIT(1))
#define LP_I2S_ENERGY_ENOUGH_M  (LP_I2S_ENERGY_ENOUGH_V << LP_I2S_ENERGY_ENOUGH_S)
#define LP_I2S_ENERGY_ENOUGH_V  0x00000001U
#define LP_I2S_ENERGY_ENOUGH_S  1

/** LP_I2S_RX_MEM_CONF_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_RX_MEM_CONF_REG (DR_REG_LP_I2S_BASE + 0x8)
/** LP_I2S_RX_MEM_FIFO_CNT : RO; bitpos: [8:0]; default: 0;
 *  The number of data in the rx mem
 */
#define LP_I2S_RX_MEM_FIFO_CNT    0x000001FFU
#define LP_I2S_RX_MEM_FIFO_CNT_M  (LP_I2S_RX_MEM_FIFO_CNT_V << LP_I2S_RX_MEM_FIFO_CNT_S)
#define LP_I2S_RX_MEM_FIFO_CNT_V  0x000001FFU
#define LP_I2S_RX_MEM_FIFO_CNT_S  0
/** LP_I2S_RX_MEM_THRESHOLD : R/W; bitpos: [16:9]; default: 63;
 *  I2S rx mem will trigger an interrupt when the data in the mem is over(not including
 *  equal) reg_rx_mem_threshold
 */
#define LP_I2S_RX_MEM_THRESHOLD    0x000000FFU
#define LP_I2S_RX_MEM_THRESHOLD_M  (LP_I2S_RX_MEM_THRESHOLD_V << LP_I2S_RX_MEM_THRESHOLD_S)
#define LP_I2S_RX_MEM_THRESHOLD_V  0x000000FFU
#define LP_I2S_RX_MEM_THRESHOLD_S  9

/** LP_I2S_INT_RAW_REG register
 *  I2S interrupt raw register, valid in level.
 */
#define LP_I2S_INT_RAW_REG (DR_REG_LP_I2S_BASE + 0xc)
/** LP_I2S_RX_DONE_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status bit  for the i2s_rx_done_int interrupt
 */
#define LP_I2S_RX_DONE_INT_RAW    (BIT(0))
#define LP_I2S_RX_DONE_INT_RAW_M  (LP_I2S_RX_DONE_INT_RAW_V << LP_I2S_RX_DONE_INT_RAW_S)
#define LP_I2S_RX_DONE_INT_RAW_V  0x00000001U
#define LP_I2S_RX_DONE_INT_RAW_S  0
/** LP_I2S_RX_HUNG_INT_RAW : RO/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status bit  for the i2s_rx_hung_int interrupt
 */
#define LP_I2S_RX_HUNG_INT_RAW    (BIT(1))
#define LP_I2S_RX_HUNG_INT_RAW_M  (LP_I2S_RX_HUNG_INT_RAW_V << LP_I2S_RX_HUNG_INT_RAW_S)
#define LP_I2S_RX_HUNG_INT_RAW_V  0x00000001U
#define LP_I2S_RX_HUNG_INT_RAW_S  1
/** LP_I2S_RX_FIFOMEM_UDF_INT_RAW : RO/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status bit  for the i2s_rx_fifomem_udf_int interrupt
 */
#define LP_I2S_RX_FIFOMEM_UDF_INT_RAW    (BIT(2))
#define LP_I2S_RX_FIFOMEM_UDF_INT_RAW_M  (LP_I2S_RX_FIFOMEM_UDF_INT_RAW_V << LP_I2S_RX_FIFOMEM_UDF_INT_RAW_S)
#define LP_I2S_RX_FIFOMEM_UDF_INT_RAW_V  0x00000001U
#define LP_I2S_RX_FIFOMEM_UDF_INT_RAW_S  2
/** LP_I2S_VAD_DONE_INT_RAW : RO/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status bit  for the vad_done_int interrupt
 */
#define LP_I2S_VAD_DONE_INT_RAW    (BIT(3))
#define LP_I2S_VAD_DONE_INT_RAW_M  (LP_I2S_VAD_DONE_INT_RAW_V << LP_I2S_VAD_DONE_INT_RAW_S)
#define LP_I2S_VAD_DONE_INT_RAW_V  0x00000001U
#define LP_I2S_VAD_DONE_INT_RAW_S  3
/** LP_I2S_VAD_RESET_DONE_INT_RAW : RO/WTC/SS; bitpos: [4]; default: 0;
 *  The raw interrupt status bit  for the vad_reset_done_int interrupt
 */
#define LP_I2S_VAD_RESET_DONE_INT_RAW    (BIT(4))
#define LP_I2S_VAD_RESET_DONE_INT_RAW_M  (LP_I2S_VAD_RESET_DONE_INT_RAW_V << LP_I2S_VAD_RESET_DONE_INT_RAW_S)
#define LP_I2S_VAD_RESET_DONE_INT_RAW_V  0x00000001U
#define LP_I2S_VAD_RESET_DONE_INT_RAW_S  4
/** LP_I2S_RX_MEM_THRESHOLD_INT_RAW : RO/WTC/SS; bitpos: [5]; default: 0;
 *  The raw interrupt status bit  for the rx_mem_threshold_int interrupt
 */
#define LP_I2S_RX_MEM_THRESHOLD_INT_RAW    (BIT(5))
#define LP_I2S_RX_MEM_THRESHOLD_INT_RAW_M  (LP_I2S_RX_MEM_THRESHOLD_INT_RAW_V << LP_I2S_RX_MEM_THRESHOLD_INT_RAW_S)
#define LP_I2S_RX_MEM_THRESHOLD_INT_RAW_V  0x00000001U
#define LP_I2S_RX_MEM_THRESHOLD_INT_RAW_S  5

/** LP_I2S_INT_ST_REG register
 *  I2S interrupt status register.
 */
#define LP_I2S_INT_ST_REG (DR_REG_LP_I2S_BASE + 0x10)
/** LP_I2S_RX_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit  for the i2s_rx_done_int interrupt
 */
#define LP_I2S_RX_DONE_INT_ST    (BIT(0))
#define LP_I2S_RX_DONE_INT_ST_M  (LP_I2S_RX_DONE_INT_ST_V << LP_I2S_RX_DONE_INT_ST_S)
#define LP_I2S_RX_DONE_INT_ST_V  0x00000001U
#define LP_I2S_RX_DONE_INT_ST_S  0
/** LP_I2S_RX_HUNG_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit  for the i2s_rx_hung_int interrupt
 */
#define LP_I2S_RX_HUNG_INT_ST    (BIT(1))
#define LP_I2S_RX_HUNG_INT_ST_M  (LP_I2S_RX_HUNG_INT_ST_V << LP_I2S_RX_HUNG_INT_ST_S)
#define LP_I2S_RX_HUNG_INT_ST_V  0x00000001U
#define LP_I2S_RX_HUNG_INT_ST_S  1
/** LP_I2S_RX_FIFOMEM_UDF_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit  for the i2s_rx_fifomem_udf_int interrupt
 */
#define LP_I2S_RX_FIFOMEM_UDF_INT_ST    (BIT(2))
#define LP_I2S_RX_FIFOMEM_UDF_INT_ST_M  (LP_I2S_RX_FIFOMEM_UDF_INT_ST_V << LP_I2S_RX_FIFOMEM_UDF_INT_ST_S)
#define LP_I2S_RX_FIFOMEM_UDF_INT_ST_V  0x00000001U
#define LP_I2S_RX_FIFOMEM_UDF_INT_ST_S  2
/** LP_I2S_VAD_DONE_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit  for the vad_done_int interrupt
 */
#define LP_I2S_VAD_DONE_INT_ST    (BIT(3))
#define LP_I2S_VAD_DONE_INT_ST_M  (LP_I2S_VAD_DONE_INT_ST_V << LP_I2S_VAD_DONE_INT_ST_S)
#define LP_I2S_VAD_DONE_INT_ST_V  0x00000001U
#define LP_I2S_VAD_DONE_INT_ST_S  3
/** LP_I2S_VAD_RESET_DONE_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status bit  for the vad_reset_done_int interrupt
 */
#define LP_I2S_VAD_RESET_DONE_INT_ST    (BIT(4))
#define LP_I2S_VAD_RESET_DONE_INT_ST_M  (LP_I2S_VAD_RESET_DONE_INT_ST_V << LP_I2S_VAD_RESET_DONE_INT_ST_S)
#define LP_I2S_VAD_RESET_DONE_INT_ST_V  0x00000001U
#define LP_I2S_VAD_RESET_DONE_INT_ST_S  4
/** LP_I2S_RX_MEM_THRESHOLD_INT_ST : RO; bitpos: [5]; default: 0;
 *  The masked interrupt status bit  for the rx_mem_threshold_int interrupt
 */
#define LP_I2S_RX_MEM_THRESHOLD_INT_ST    (BIT(5))
#define LP_I2S_RX_MEM_THRESHOLD_INT_ST_M  (LP_I2S_RX_MEM_THRESHOLD_INT_ST_V << LP_I2S_RX_MEM_THRESHOLD_INT_ST_S)
#define LP_I2S_RX_MEM_THRESHOLD_INT_ST_V  0x00000001U
#define LP_I2S_RX_MEM_THRESHOLD_INT_ST_S  5

/** LP_I2S_INT_ENA_REG register
 *  I2S interrupt enable register.
 */
#define LP_I2S_INT_ENA_REG (DR_REG_LP_I2S_BASE + 0x14)
/** LP_I2S_RX_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit  for the i2s_rx_done_int interrupt
 */
#define LP_I2S_RX_DONE_INT_ENA    (BIT(0))
#define LP_I2S_RX_DONE_INT_ENA_M  (LP_I2S_RX_DONE_INT_ENA_V << LP_I2S_RX_DONE_INT_ENA_S)
#define LP_I2S_RX_DONE_INT_ENA_V  0x00000001U
#define LP_I2S_RX_DONE_INT_ENA_S  0
/** LP_I2S_RX_HUNG_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit  for the i2s_rx_hung_int interrupt
 */
#define LP_I2S_RX_HUNG_INT_ENA    (BIT(1))
#define LP_I2S_RX_HUNG_INT_ENA_M  (LP_I2S_RX_HUNG_INT_ENA_V << LP_I2S_RX_HUNG_INT_ENA_S)
#define LP_I2S_RX_HUNG_INT_ENA_V  0x00000001U
#define LP_I2S_RX_HUNG_INT_ENA_S  1
/** LP_I2S_RX_FIFOMEM_UDF_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit  for the i2s_rx_fifomem_udf_int interrupt
 */
#define LP_I2S_RX_FIFOMEM_UDF_INT_ENA    (BIT(2))
#define LP_I2S_RX_FIFOMEM_UDF_INT_ENA_M  (LP_I2S_RX_FIFOMEM_UDF_INT_ENA_V << LP_I2S_RX_FIFOMEM_UDF_INT_ENA_S)
#define LP_I2S_RX_FIFOMEM_UDF_INT_ENA_V  0x00000001U
#define LP_I2S_RX_FIFOMEM_UDF_INT_ENA_S  2
/** LP_I2S_VAD_DONE_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit  for the vad_done_int interrupt
 */
#define LP_I2S_VAD_DONE_INT_ENA    (BIT(3))
#define LP_I2S_VAD_DONE_INT_ENA_M  (LP_I2S_VAD_DONE_INT_ENA_V << LP_I2S_VAD_DONE_INT_ENA_S)
#define LP_I2S_VAD_DONE_INT_ENA_V  0x00000001U
#define LP_I2S_VAD_DONE_INT_ENA_S  3
/** LP_I2S_VAD_RESET_DONE_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  The interrupt enable bit  for the vad_reset_done_int interrupt
 */
#define LP_I2S_VAD_RESET_DONE_INT_ENA    (BIT(4))
#define LP_I2S_VAD_RESET_DONE_INT_ENA_M  (LP_I2S_VAD_RESET_DONE_INT_ENA_V << LP_I2S_VAD_RESET_DONE_INT_ENA_S)
#define LP_I2S_VAD_RESET_DONE_INT_ENA_V  0x00000001U
#define LP_I2S_VAD_RESET_DONE_INT_ENA_S  4
/** LP_I2S_RX_MEM_THRESHOLD_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  The interrupt enable bit  for the rx_mem_threshold_int interrupt
 */
#define LP_I2S_RX_MEM_THRESHOLD_INT_ENA    (BIT(5))
#define LP_I2S_RX_MEM_THRESHOLD_INT_ENA_M  (LP_I2S_RX_MEM_THRESHOLD_INT_ENA_V << LP_I2S_RX_MEM_THRESHOLD_INT_ENA_S)
#define LP_I2S_RX_MEM_THRESHOLD_INT_ENA_V  0x00000001U
#define LP_I2S_RX_MEM_THRESHOLD_INT_ENA_S  5

/** LP_I2S_INT_CLR_REG register
 *  I2S interrupt clear register.
 */
#define LP_I2S_INT_CLR_REG (DR_REG_LP_I2S_BASE + 0x18)
/** LP_I2S_RX_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the i2s_rx_done_int interrupt
 */
#define LP_I2S_RX_DONE_INT_CLR    (BIT(0))
#define LP_I2S_RX_DONE_INT_CLR_M  (LP_I2S_RX_DONE_INT_CLR_V << LP_I2S_RX_DONE_INT_CLR_S)
#define LP_I2S_RX_DONE_INT_CLR_V  0x00000001U
#define LP_I2S_RX_DONE_INT_CLR_S  0
/** LP_I2S_RX_HUNG_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear the i2s_rx_hung_int interrupt
 */
#define LP_I2S_RX_HUNG_INT_CLR    (BIT(1))
#define LP_I2S_RX_HUNG_INT_CLR_M  (LP_I2S_RX_HUNG_INT_CLR_V << LP_I2S_RX_HUNG_INT_CLR_S)
#define LP_I2S_RX_HUNG_INT_CLR_V  0x00000001U
#define LP_I2S_RX_HUNG_INT_CLR_S  1
/** LP_I2S_RX_FIFOMEM_UDF_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Set this bit to clear the i2s_rx_fifomem_udf_int interrupt
 */
#define LP_I2S_RX_FIFOMEM_UDF_INT_CLR    (BIT(2))
#define LP_I2S_RX_FIFOMEM_UDF_INT_CLR_M  (LP_I2S_RX_FIFOMEM_UDF_INT_CLR_V << LP_I2S_RX_FIFOMEM_UDF_INT_CLR_S)
#define LP_I2S_RX_FIFOMEM_UDF_INT_CLR_V  0x00000001U
#define LP_I2S_RX_FIFOMEM_UDF_INT_CLR_S  2
/** LP_I2S_VAD_DONE_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear the vad_done_int interrupt
 */
#define LP_I2S_VAD_DONE_INT_CLR    (BIT(3))
#define LP_I2S_VAD_DONE_INT_CLR_M  (LP_I2S_VAD_DONE_INT_CLR_V << LP_I2S_VAD_DONE_INT_CLR_S)
#define LP_I2S_VAD_DONE_INT_CLR_V  0x00000001U
#define LP_I2S_VAD_DONE_INT_CLR_S  3
/** LP_I2S_VAD_RESET_DONE_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Set this bit to clear the vad_reset_done_int interrupt
 */
#define LP_I2S_VAD_RESET_DONE_INT_CLR    (BIT(4))
#define LP_I2S_VAD_RESET_DONE_INT_CLR_M  (LP_I2S_VAD_RESET_DONE_INT_CLR_V << LP_I2S_VAD_RESET_DONE_INT_CLR_S)
#define LP_I2S_VAD_RESET_DONE_INT_CLR_V  0x00000001U
#define LP_I2S_VAD_RESET_DONE_INT_CLR_S  4
/** LP_I2S_RX_MEM_THRESHOLD_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Set this bit to clear the rx_mem_threshold_int interrupt
 */
#define LP_I2S_RX_MEM_THRESHOLD_INT_CLR    (BIT(5))
#define LP_I2S_RX_MEM_THRESHOLD_INT_CLR_M  (LP_I2S_RX_MEM_THRESHOLD_INT_CLR_V << LP_I2S_RX_MEM_THRESHOLD_INT_CLR_S)
#define LP_I2S_RX_MEM_THRESHOLD_INT_CLR_V  0x00000001U
#define LP_I2S_RX_MEM_THRESHOLD_INT_CLR_S  5

/** LP_I2S_RX_CONF_REG register
 *  I2S RX configure register
 */
#define LP_I2S_RX_CONF_REG (DR_REG_LP_I2S_BASE + 0x20)
/** LP_I2S_RX_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset receiver
 */
#define LP_I2S_RX_RESET    (BIT(0))
#define LP_I2S_RX_RESET_M  (LP_I2S_RX_RESET_V << LP_I2S_RX_RESET_S)
#define LP_I2S_RX_RESET_V  0x00000001U
#define LP_I2S_RX_RESET_S  0
/** LP_I2S_RX_FIFO_RESET : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset Rx AFIFO
 */
#define LP_I2S_RX_FIFO_RESET    (BIT(1))
#define LP_I2S_RX_FIFO_RESET_M  (LP_I2S_RX_FIFO_RESET_V << LP_I2S_RX_FIFO_RESET_S)
#define LP_I2S_RX_FIFO_RESET_V  0x00000001U
#define LP_I2S_RX_FIFO_RESET_S  1
/** LP_I2S_RX_START : R/W; bitpos: [2]; default: 0;
 *  Set this bit to start receiving data
 */
#define LP_I2S_RX_START    (BIT(2))
#define LP_I2S_RX_START_M  (LP_I2S_RX_START_V << LP_I2S_RX_START_S)
#define LP_I2S_RX_START_V  0x00000001U
#define LP_I2S_RX_START_S  2
/** LP_I2S_RX_SLAVE_MOD : R/W; bitpos: [3]; default: 0;
 *  Set this bit to enable slave receiver mode
 */
#define LP_I2S_RX_SLAVE_MOD    (BIT(3))
#define LP_I2S_RX_SLAVE_MOD_M  (LP_I2S_RX_SLAVE_MOD_V << LP_I2S_RX_SLAVE_MOD_S)
#define LP_I2S_RX_SLAVE_MOD_V  0x00000001U
#define LP_I2S_RX_SLAVE_MOD_S  3
/** LP_I2S_RX_FIFOMEM_RESET : WT; bitpos: [4]; default: 0;
 *  Set this bit to reset Rx Syncfifomem
 */
#define LP_I2S_RX_FIFOMEM_RESET    (BIT(4))
#define LP_I2S_RX_FIFOMEM_RESET_M  (LP_I2S_RX_FIFOMEM_RESET_V << LP_I2S_RX_FIFOMEM_RESET_S)
#define LP_I2S_RX_FIFOMEM_RESET_V  0x00000001U
#define LP_I2S_RX_FIFOMEM_RESET_S  4
/** LP_I2S_RX_MONO : R/W; bitpos: [5]; default: 0;
 *  Set this bit to enable receiver  in mono mode
 */
#define LP_I2S_RX_MONO    (BIT(5))
#define LP_I2S_RX_MONO_M  (LP_I2S_RX_MONO_V << LP_I2S_RX_MONO_S)
#define LP_I2S_RX_MONO_V  0x00000001U
#define LP_I2S_RX_MONO_S  5
/** LP_I2S_RX_BIG_ENDIAN : R/W; bitpos: [7]; default: 0;
 *  I2S Rx byte endian, 1: low addr value to high addr. 0: low addr with low addr value.
 */
#define LP_I2S_RX_BIG_ENDIAN    (BIT(7))
#define LP_I2S_RX_BIG_ENDIAN_M  (LP_I2S_RX_BIG_ENDIAN_V << LP_I2S_RX_BIG_ENDIAN_S)
#define LP_I2S_RX_BIG_ENDIAN_V  0x00000001U
#define LP_I2S_RX_BIG_ENDIAN_S  7
/** LP_I2S_RX_UPDATE : R/W/SC; bitpos: [8]; default: 0;
 *  Set 1 to update I2S RX registers from APB clock domain to I2S RX clock domain. This
 *  bit will be cleared by hardware after update register done.
 */
#define LP_I2S_RX_UPDATE    (BIT(8))
#define LP_I2S_RX_UPDATE_M  (LP_I2S_RX_UPDATE_V << LP_I2S_RX_UPDATE_S)
#define LP_I2S_RX_UPDATE_V  0x00000001U
#define LP_I2S_RX_UPDATE_S  8
/** LP_I2S_RX_MONO_FST_VLD : R/W; bitpos: [9]; default: 1;
 *  1: The first channel data value is valid in I2S RX mono mode.   0: The second
 *  channel data value is valid in I2S RX mono mode.
 */
#define LP_I2S_RX_MONO_FST_VLD    (BIT(9))
#define LP_I2S_RX_MONO_FST_VLD_M  (LP_I2S_RX_MONO_FST_VLD_V << LP_I2S_RX_MONO_FST_VLD_S)
#define LP_I2S_RX_MONO_FST_VLD_V  0x00000001U
#define LP_I2S_RX_MONO_FST_VLD_S  9
/** LP_I2S_RX_PCM_CONF : R/W; bitpos: [11:10]; default: 1;
 *  I2S RX compress/decompress configuration bit. & 0 (atol): A-Law decompress, 1
 *  (ltoa) : A-Law compress, 2 (utol) : u-Law decompress, 3 (ltou) : u-Law compress. &
 */
#define LP_I2S_RX_PCM_CONF    0x00000003U
#define LP_I2S_RX_PCM_CONF_M  (LP_I2S_RX_PCM_CONF_V << LP_I2S_RX_PCM_CONF_S)
#define LP_I2S_RX_PCM_CONF_V  0x00000003U
#define LP_I2S_RX_PCM_CONF_S  10
/** LP_I2S_RX_PCM_BYPASS : R/W; bitpos: [12]; default: 1;
 *  Set this bit to bypass Compress/Decompress module for received data.
 */
#define LP_I2S_RX_PCM_BYPASS    (BIT(12))
#define LP_I2S_RX_PCM_BYPASS_M  (LP_I2S_RX_PCM_BYPASS_V << LP_I2S_RX_PCM_BYPASS_S)
#define LP_I2S_RX_PCM_BYPASS_V  0x00000001U
#define LP_I2S_RX_PCM_BYPASS_S  12
/** LP_I2S_RX_STOP_MODE : R/W; bitpos: [14:13]; default: 0;
 *  0  : I2S Rx only stop when reg_rx_start is cleared.   1: Stop when reg_rx_start is
 *  0 or in_suc_eof is 1.   2:  Stop I2S RX when reg_rx_start is 0 or RX FIFO is full.
 */
#define LP_I2S_RX_STOP_MODE    0x00000003U
#define LP_I2S_RX_STOP_MODE_M  (LP_I2S_RX_STOP_MODE_V << LP_I2S_RX_STOP_MODE_S)
#define LP_I2S_RX_STOP_MODE_V  0x00000003U
#define LP_I2S_RX_STOP_MODE_S  13
/** LP_I2S_RX_LEFT_ALIGN : R/W; bitpos: [15]; default: 1;
 *  1: I2S RX left alignment mode. 0: I2S RX right alignment mode.
 */
#define LP_I2S_RX_LEFT_ALIGN    (BIT(15))
#define LP_I2S_RX_LEFT_ALIGN_M  (LP_I2S_RX_LEFT_ALIGN_V << LP_I2S_RX_LEFT_ALIGN_S)
#define LP_I2S_RX_LEFT_ALIGN_V  0x00000001U
#define LP_I2S_RX_LEFT_ALIGN_S  15
/** LP_I2S_RX_24_FILL_EN : R/W; bitpos: [16]; default: 0;
 *  1: store 24 channel bits to 32 bits. 0:store 24 channel bits to 24 bits.
 */
#define LP_I2S_RX_24_FILL_EN    (BIT(16))
#define LP_I2S_RX_24_FILL_EN_M  (LP_I2S_RX_24_FILL_EN_V << LP_I2S_RX_24_FILL_EN_S)
#define LP_I2S_RX_24_FILL_EN_V  0x00000001U
#define LP_I2S_RX_24_FILL_EN_S  16
/** LP_I2S_RX_WS_IDLE_POL : R/W; bitpos: [17]; default: 0;
 *  0: WS should be 0 when receiving left channel data, and WS is 1in right channel.
 *  1: WS should be 1 when receiving left channel data, and WS is 0in right channel.
 */
#define LP_I2S_RX_WS_IDLE_POL    (BIT(17))
#define LP_I2S_RX_WS_IDLE_POL_M  (LP_I2S_RX_WS_IDLE_POL_V << LP_I2S_RX_WS_IDLE_POL_S)
#define LP_I2S_RX_WS_IDLE_POL_V  0x00000001U
#define LP_I2S_RX_WS_IDLE_POL_S  17
/** LP_I2S_RX_BIT_ORDER : R/W; bitpos: [18]; default: 0;
 *  I2S Rx bit endian. 1:small endian, the LSB is received first. 0:big endian, the MSB
 *  is received first.
 */
#define LP_I2S_RX_BIT_ORDER    (BIT(18))
#define LP_I2S_RX_BIT_ORDER_M  (LP_I2S_RX_BIT_ORDER_V << LP_I2S_RX_BIT_ORDER_S)
#define LP_I2S_RX_BIT_ORDER_V  0x00000001U
#define LP_I2S_RX_BIT_ORDER_S  18
/** LP_I2S_RX_TDM_EN : R/W; bitpos: [19]; default: 0;
 *  1: Enable I2S TDM Rx mode . 0: Disable.
 */
#define LP_I2S_RX_TDM_EN    (BIT(19))
#define LP_I2S_RX_TDM_EN_M  (LP_I2S_RX_TDM_EN_V << LP_I2S_RX_TDM_EN_S)
#define LP_I2S_RX_TDM_EN_V  0x00000001U
#define LP_I2S_RX_TDM_EN_S  19
/** LP_I2S_RX_PDM_EN : R/W; bitpos: [20]; default: 0;
 *  1: Enable I2S PDM Rx mode . 0: Disable.
 */
#define LP_I2S_RX_PDM_EN    (BIT(20))
#define LP_I2S_RX_PDM_EN_M  (LP_I2S_RX_PDM_EN_V << LP_I2S_RX_PDM_EN_S)
#define LP_I2S_RX_PDM_EN_V  0x00000001U
#define LP_I2S_RX_PDM_EN_S  20

/** LP_I2S_RX_CONF1_REG register
 *  I2S RX configure register 1
 */
#define LP_I2S_RX_CONF1_REG (DR_REG_LP_I2S_BASE + 0x28)
/** LP_I2S_RX_TDM_WS_WIDTH : R/W; bitpos: [6:0]; default: 0;
 *  The width of rx_ws_out in TDM mode is (I2S_RX_TDM_WS_WIDTH[6:0] +1) * T_bck
 */
#define LP_I2S_RX_TDM_WS_WIDTH    0x0000007FU
#define LP_I2S_RX_TDM_WS_WIDTH_M  (LP_I2S_RX_TDM_WS_WIDTH_V << LP_I2S_RX_TDM_WS_WIDTH_S)
#define LP_I2S_RX_TDM_WS_WIDTH_V  0x0000007FU
#define LP_I2S_RX_TDM_WS_WIDTH_S  0
/** LP_I2S_RX_BCK_DIV_NUM : R/W; bitpos: [12:7]; default: 6;
 *  Bit clock configuration bits in receiver mode.
 */
#define LP_I2S_RX_BCK_DIV_NUM    0x0000003FU
#define LP_I2S_RX_BCK_DIV_NUM_M  (LP_I2S_RX_BCK_DIV_NUM_V << LP_I2S_RX_BCK_DIV_NUM_S)
#define LP_I2S_RX_BCK_DIV_NUM_V  0x0000003FU
#define LP_I2S_RX_BCK_DIV_NUM_S  7
/** LP_I2S_RX_BITS_MOD : R/W; bitpos: [17:13]; default: 15;
 *  Set the bits to configure the valid data bit length of I2S receiver channel. 7: all
 *  the valid channel data is in 8-bit-mode. 15: all the valid channel data is in
 *  16-bit-mode. 23: all the valid channel data is in 24-bit-mode. 31:all the valid
 *  channel data is in 32-bit-mode.
 */
#define LP_I2S_RX_BITS_MOD    0x0000001FU
#define LP_I2S_RX_BITS_MOD_M  (LP_I2S_RX_BITS_MOD_V << LP_I2S_RX_BITS_MOD_S)
#define LP_I2S_RX_BITS_MOD_V  0x0000001FU
#define LP_I2S_RX_BITS_MOD_S  13
/** LP_I2S_RX_HALF_SAMPLE_BITS : R/W; bitpos: [23:18]; default: 15;
 *  I2S Rx half sample bits -1.
 */
#define LP_I2S_RX_HALF_SAMPLE_BITS    0x0000003FU
#define LP_I2S_RX_HALF_SAMPLE_BITS_M  (LP_I2S_RX_HALF_SAMPLE_BITS_V << LP_I2S_RX_HALF_SAMPLE_BITS_S)
#define LP_I2S_RX_HALF_SAMPLE_BITS_V  0x0000003FU
#define LP_I2S_RX_HALF_SAMPLE_BITS_S  18
/** LP_I2S_RX_TDM_CHAN_BITS : R/W; bitpos: [28:24]; default: 15;
 *  The Rx bit number for each channel minus 1in TDM mode.
 */
#define LP_I2S_RX_TDM_CHAN_BITS    0x0000001FU
#define LP_I2S_RX_TDM_CHAN_BITS_M  (LP_I2S_RX_TDM_CHAN_BITS_V << LP_I2S_RX_TDM_CHAN_BITS_S)
#define LP_I2S_RX_TDM_CHAN_BITS_V  0x0000001FU
#define LP_I2S_RX_TDM_CHAN_BITS_S  24
/** LP_I2S_RX_MSB_SHIFT : R/W; bitpos: [29]; default: 1;
 *  Set this bit to enable receiver in Phillips standard mode
 */
#define LP_I2S_RX_MSB_SHIFT    (BIT(29))
#define LP_I2S_RX_MSB_SHIFT_M  (LP_I2S_RX_MSB_SHIFT_V << LP_I2S_RX_MSB_SHIFT_S)
#define LP_I2S_RX_MSB_SHIFT_V  0x00000001U
#define LP_I2S_RX_MSB_SHIFT_S  29

/** LP_I2S_RX_TDM_CTRL_REG register
 *  I2S TX TDM mode control register
 */
#define LP_I2S_RX_TDM_CTRL_REG (DR_REG_LP_I2S_BASE + 0x50)
/** LP_I2S_RX_TDM_PDM_CHAN0_EN : R/W; bitpos: [0]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define LP_I2S_RX_TDM_PDM_CHAN0_EN    (BIT(0))
#define LP_I2S_RX_TDM_PDM_CHAN0_EN_M  (LP_I2S_RX_TDM_PDM_CHAN0_EN_V << LP_I2S_RX_TDM_PDM_CHAN0_EN_S)
#define LP_I2S_RX_TDM_PDM_CHAN0_EN_V  0x00000001U
#define LP_I2S_RX_TDM_PDM_CHAN0_EN_S  0
/** LP_I2S_RX_TDM_PDM_CHAN1_EN : R/W; bitpos: [1]; default: 1;
 *  1: Enable the valid data input of I2S RX TDM or PDM channel $n. 0:  Disable, just
 *  input 0 in this channel.
 */
#define LP_I2S_RX_TDM_PDM_CHAN1_EN    (BIT(1))
#define LP_I2S_RX_TDM_PDM_CHAN1_EN_M  (LP_I2S_RX_TDM_PDM_CHAN1_EN_V << LP_I2S_RX_TDM_PDM_CHAN1_EN_S)
#define LP_I2S_RX_TDM_PDM_CHAN1_EN_V  0x00000001U
#define LP_I2S_RX_TDM_PDM_CHAN1_EN_S  1
/** LP_I2S_RX_TDM_TOT_CHAN_NUM : R/W; bitpos: [19:16]; default: 0;
 *  The total channel number of I2S TX TDM mode.
 */
#define LP_I2S_RX_TDM_TOT_CHAN_NUM    0x0000000FU
#define LP_I2S_RX_TDM_TOT_CHAN_NUM_M  (LP_I2S_RX_TDM_TOT_CHAN_NUM_V << LP_I2S_RX_TDM_TOT_CHAN_NUM_S)
#define LP_I2S_RX_TDM_TOT_CHAN_NUM_V  0x0000000FU
#define LP_I2S_RX_TDM_TOT_CHAN_NUM_S  16

/** LP_I2S_RX_TIMING_REG register
 *  I2S RX timing control register
 */
#define LP_I2S_RX_TIMING_REG (DR_REG_LP_I2S_BASE + 0x58)
/** LP_I2S_RX_SD_IN_DM : R/W; bitpos: [1:0]; default: 0;
 *  The delay mode of I2S Rx SD input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define LP_I2S_RX_SD_IN_DM    0x00000003U
#define LP_I2S_RX_SD_IN_DM_M  (LP_I2S_RX_SD_IN_DM_V << LP_I2S_RX_SD_IN_DM_S)
#define LP_I2S_RX_SD_IN_DM_V  0x00000003U
#define LP_I2S_RX_SD_IN_DM_S  0
/** LP_I2S_RX_WS_OUT_DM : R/W; bitpos: [17:16]; default: 0;
 *  The delay mode of I2S Rx WS output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define LP_I2S_RX_WS_OUT_DM    0x00000003U
#define LP_I2S_RX_WS_OUT_DM_M  (LP_I2S_RX_WS_OUT_DM_V << LP_I2S_RX_WS_OUT_DM_S)
#define LP_I2S_RX_WS_OUT_DM_V  0x00000003U
#define LP_I2S_RX_WS_OUT_DM_S  16
/** LP_I2S_RX_BCK_OUT_DM : R/W; bitpos: [21:20]; default: 0;
 *  The delay mode of I2S Rx BCK output signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define LP_I2S_RX_BCK_OUT_DM    0x00000003U
#define LP_I2S_RX_BCK_OUT_DM_M  (LP_I2S_RX_BCK_OUT_DM_V << LP_I2S_RX_BCK_OUT_DM_S)
#define LP_I2S_RX_BCK_OUT_DM_V  0x00000003U
#define LP_I2S_RX_BCK_OUT_DM_S  20
/** LP_I2S_RX_WS_IN_DM : R/W; bitpos: [25:24]; default: 0;
 *  The delay mode of I2S Rx WS input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define LP_I2S_RX_WS_IN_DM    0x00000003U
#define LP_I2S_RX_WS_IN_DM_M  (LP_I2S_RX_WS_IN_DM_V << LP_I2S_RX_WS_IN_DM_S)
#define LP_I2S_RX_WS_IN_DM_V  0x00000003U
#define LP_I2S_RX_WS_IN_DM_S  24
/** LP_I2S_RX_BCK_IN_DM : R/W; bitpos: [29:28]; default: 0;
 *  The delay mode of I2S Rx BCK input signal. 0: bypass. 1: delay by pos edge.  2:
 *  delay by neg edge. 3: not used.
 */
#define LP_I2S_RX_BCK_IN_DM    0x00000003U
#define LP_I2S_RX_BCK_IN_DM_M  (LP_I2S_RX_BCK_IN_DM_V << LP_I2S_RX_BCK_IN_DM_S)
#define LP_I2S_RX_BCK_IN_DM_V  0x00000003U
#define LP_I2S_RX_BCK_IN_DM_S  28

/** LP_I2S_LC_HUNG_CONF_REG register
 *  I2S HUNG configure register.
 */
#define LP_I2S_LC_HUNG_CONF_REG (DR_REG_LP_I2S_BASE + 0x60)
/** LP_I2S_LC_FIFO_TIMEOUT : R/W; bitpos: [7:0]; default: 16;
 *  the i2s_tx_hung_int interrupt or the i2s_rx_hung_int interrupt will be triggered
 *  when fifo hung counter is equal to this value
 */
#define LP_I2S_LC_FIFO_TIMEOUT    0x000000FFU
#define LP_I2S_LC_FIFO_TIMEOUT_M  (LP_I2S_LC_FIFO_TIMEOUT_V << LP_I2S_LC_FIFO_TIMEOUT_S)
#define LP_I2S_LC_FIFO_TIMEOUT_V  0x000000FFU
#define LP_I2S_LC_FIFO_TIMEOUT_S  0
/** LP_I2S_LC_FIFO_TIMEOUT_SHIFT : R/W; bitpos: [10:8]; default: 0;
 *  The bits are used to scale tick counter threshold. The tick counter is reset when
 *  counter value >= 88000/2^i2s_lc_fifo_timeout_shift
 */
#define LP_I2S_LC_FIFO_TIMEOUT_SHIFT    0x00000007U
#define LP_I2S_LC_FIFO_TIMEOUT_SHIFT_M  (LP_I2S_LC_FIFO_TIMEOUT_SHIFT_V << LP_I2S_LC_FIFO_TIMEOUT_SHIFT_S)
#define LP_I2S_LC_FIFO_TIMEOUT_SHIFT_V  0x00000007U
#define LP_I2S_LC_FIFO_TIMEOUT_SHIFT_S  8
/** LP_I2S_LC_FIFO_TIMEOUT_ENA : R/W; bitpos: [11]; default: 1;
 *  The enable bit for FIFO timeout
 */
#define LP_I2S_LC_FIFO_TIMEOUT_ENA    (BIT(11))
#define LP_I2S_LC_FIFO_TIMEOUT_ENA_M  (LP_I2S_LC_FIFO_TIMEOUT_ENA_V << LP_I2S_LC_FIFO_TIMEOUT_ENA_S)
#define LP_I2S_LC_FIFO_TIMEOUT_ENA_V  0x00000001U
#define LP_I2S_LC_FIFO_TIMEOUT_ENA_S  11

/** LP_I2S_RXEOF_NUM_REG register
 *  I2S RX data number control register.
 */
#define LP_I2S_RXEOF_NUM_REG (DR_REG_LP_I2S_BASE + 0x64)
/** LP_I2S_RX_EOF_NUM : R/W; bitpos: [11:0]; default: 64;
 *  The receive data bit length is (I2S_RX_BITS_MOD[4:0] + 1) * (REG_RX_EOF_NUM[11:0] +
 *  1) . It will trigger in_suc_eof interrupt in the configured DMA RX channel.
 */
#define LP_I2S_RX_EOF_NUM    0x00000FFFU
#define LP_I2S_RX_EOF_NUM_M  (LP_I2S_RX_EOF_NUM_V << LP_I2S_RX_EOF_NUM_S)
#define LP_I2S_RX_EOF_NUM_V  0x00000FFFU
#define LP_I2S_RX_EOF_NUM_S  0

/** LP_I2S_CONF_SIGLE_DATA_REG register
 *  I2S signal data register
 */
#define LP_I2S_CONF_SIGLE_DATA_REG (DR_REG_LP_I2S_BASE + 0x68)
/** LP_I2S_SINGLE_DATA : R/W; bitpos: [31:0]; default: 0;
 *  The configured constant channel data to be sent out.
 */
#define LP_I2S_SINGLE_DATA    0xFFFFFFFFU
#define LP_I2S_SINGLE_DATA_M  (LP_I2S_SINGLE_DATA_V << LP_I2S_SINGLE_DATA_S)
#define LP_I2S_SINGLE_DATA_V  0xFFFFFFFFU
#define LP_I2S_SINGLE_DATA_S  0

/** LP_I2S_RX_PDM_CONF_REG register
 *  I2S RX configure register
 */
#define LP_I2S_RX_PDM_CONF_REG (DR_REG_LP_I2S_BASE + 0x70)
/** LP_I2S_RX_PDM2PCM_EN : R/W; bitpos: [19]; default: 0;
 *  1: Enable PDM2PCM RX mode. 0: DIsable.
 */
#define LP_I2S_RX_PDM2PCM_EN    (BIT(19))
#define LP_I2S_RX_PDM2PCM_EN_M  (LP_I2S_RX_PDM2PCM_EN_V << LP_I2S_RX_PDM2PCM_EN_S)
#define LP_I2S_RX_PDM2PCM_EN_V  0x00000001U
#define LP_I2S_RX_PDM2PCM_EN_S  19
/** LP_I2S_RX_PDM_SINC_DSR_16_EN : R/W; bitpos: [20]; default: 0;
 *  Configure the down sampling rate of PDM RX filter group1 module. 1: The  down
 *  sampling rate is 128. 0: down sampling rate is 64.
 */
#define LP_I2S_RX_PDM_SINC_DSR_16_EN    (BIT(20))
#define LP_I2S_RX_PDM_SINC_DSR_16_EN_M  (LP_I2S_RX_PDM_SINC_DSR_16_EN_V << LP_I2S_RX_PDM_SINC_DSR_16_EN_S)
#define LP_I2S_RX_PDM_SINC_DSR_16_EN_V  0x00000001U
#define LP_I2S_RX_PDM_SINC_DSR_16_EN_S  20
/** LP_I2S_RX_PDM2PCM_AMPLIFY_NUM : R/W; bitpos: [24:21]; default: 1;
 *  Configure PDM RX amplify number.
 */
#define LP_I2S_RX_PDM2PCM_AMPLIFY_NUM    0x0000000FU
#define LP_I2S_RX_PDM2PCM_AMPLIFY_NUM_M  (LP_I2S_RX_PDM2PCM_AMPLIFY_NUM_V << LP_I2S_RX_PDM2PCM_AMPLIFY_NUM_S)
#define LP_I2S_RX_PDM2PCM_AMPLIFY_NUM_V  0x0000000FU
#define LP_I2S_RX_PDM2PCM_AMPLIFY_NUM_S  21
/** LP_I2S_RX_PDM_HP_BYPASS : R/W; bitpos: [25]; default: 0;
 *  I2S PDM RX bypass hp filter or not.
 */
#define LP_I2S_RX_PDM_HP_BYPASS    (BIT(25))
#define LP_I2S_RX_PDM_HP_BYPASS_M  (LP_I2S_RX_PDM_HP_BYPASS_V << LP_I2S_RX_PDM_HP_BYPASS_S)
#define LP_I2S_RX_PDM_HP_BYPASS_V  0x00000001U
#define LP_I2S_RX_PDM_HP_BYPASS_S  25
/** LP_I2S_RX_IIR_HP_MULT12_5 : R/W; bitpos: [28:26]; default: 6;
 *  The fourth parameter of PDM RX IIR_HP filter stage 2 is (504 +
 *  LP_I2S_RX_IIR_HP_MULT12_5[2:0])
 */
#define LP_I2S_RX_IIR_HP_MULT12_5    0x00000007U
#define LP_I2S_RX_IIR_HP_MULT12_5_M  (LP_I2S_RX_IIR_HP_MULT12_5_V << LP_I2S_RX_IIR_HP_MULT12_5_S)
#define LP_I2S_RX_IIR_HP_MULT12_5_V  0x00000007U
#define LP_I2S_RX_IIR_HP_MULT12_5_S  26
/** LP_I2S_RX_IIR_HP_MULT12_0 : R/W; bitpos: [31:29]; default: 7;
 *  The fourth parameter of PDM RX IIR_HP filter stage 1 is (504 +
 *  LP_I2S_RX_IIR_HP_MULT12_0[2:0])
 */
#define LP_I2S_RX_IIR_HP_MULT12_0    0x00000007U
#define LP_I2S_RX_IIR_HP_MULT12_0_M  (LP_I2S_RX_IIR_HP_MULT12_0_V << LP_I2S_RX_IIR_HP_MULT12_0_S)
#define LP_I2S_RX_IIR_HP_MULT12_0_V  0x00000007U
#define LP_I2S_RX_IIR_HP_MULT12_0_S  29

/** LP_I2S_ECO_LOW_REG register
 *  I2S ECO register
 */
#define LP_I2S_ECO_LOW_REG (DR_REG_LP_I2S_BASE + 0x74)
/** LP_I2S_RDN_ECO_LOW : R/W; bitpos: [31:0]; default: 0;
 *  logic low eco registers
 */
#define LP_I2S_RDN_ECO_LOW    0xFFFFFFFFU
#define LP_I2S_RDN_ECO_LOW_M  (LP_I2S_RDN_ECO_LOW_V << LP_I2S_RDN_ECO_LOW_S)
#define LP_I2S_RDN_ECO_LOW_V  0xFFFFFFFFU
#define LP_I2S_RDN_ECO_LOW_S  0

/** LP_I2S_ECO_HIGH_REG register
 *  I2S ECO register
 */
#define LP_I2S_ECO_HIGH_REG (DR_REG_LP_I2S_BASE + 0x78)
/** LP_I2S_RDN_ECO_HIGH : R/W; bitpos: [31:0]; default: 4294967295;
 *  logic high eco registers
 */
#define LP_I2S_RDN_ECO_HIGH    0xFFFFFFFFU
#define LP_I2S_RDN_ECO_HIGH_M  (LP_I2S_RDN_ECO_HIGH_V << LP_I2S_RDN_ECO_HIGH_S)
#define LP_I2S_RDN_ECO_HIGH_V  0xFFFFFFFFU
#define LP_I2S_RDN_ECO_HIGH_S  0

/** LP_I2S_ECO_CONF_REG register
 *  I2S ECO register
 */
#define LP_I2S_ECO_CONF_REG (DR_REG_LP_I2S_BASE + 0x7c)
/** LP_I2S_RDN_ENA : R/W; bitpos: [0]; default: 0;
 *  enable rdn counter bit
 */
#define LP_I2S_RDN_ENA    (BIT(0))
#define LP_I2S_RDN_ENA_M  (LP_I2S_RDN_ENA_V << LP_I2S_RDN_ENA_S)
#define LP_I2S_RDN_ENA_V  0x00000001U
#define LP_I2S_RDN_ENA_S  0
/** LP_I2S_RDN_RESULT : RO; bitpos: [1]; default: 0;
 *  rdn result
 */
#define LP_I2S_RDN_RESULT    (BIT(1))
#define LP_I2S_RDN_RESULT_M  (LP_I2S_RDN_RESULT_V << LP_I2S_RDN_RESULT_S)
#define LP_I2S_RDN_RESULT_V  0x00000001U
#define LP_I2S_RDN_RESULT_S  1

/** LP_I2S_VAD_PARAM0_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM0_REG (DR_REG_LP_I2S_BASE + 0x80)
/** LP_I2S_PARAM_MIN_ENERGY : R/W; bitpos: [15:0]; default: 5000;
 *  VAD parameter
 */
#define LP_I2S_PARAM_MIN_ENERGY    0x0000FFFFU
#define LP_I2S_PARAM_MIN_ENERGY_M  (LP_I2S_PARAM_MIN_ENERGY_V << LP_I2S_PARAM_MIN_ENERGY_S)
#define LP_I2S_PARAM_MIN_ENERGY_V  0x0000FFFFU
#define LP_I2S_PARAM_MIN_ENERGY_S  0
/** LP_I2S_PARAM_INIT_FRAME_NUM : R/W; bitpos: [24:16]; default: 200;
 *  VAD parameter
 */
#define LP_I2S_PARAM_INIT_FRAME_NUM    0x000001FFU
#define LP_I2S_PARAM_INIT_FRAME_NUM_M  (LP_I2S_PARAM_INIT_FRAME_NUM_V << LP_I2S_PARAM_INIT_FRAME_NUM_S)
#define LP_I2S_PARAM_INIT_FRAME_NUM_V  0x000001FFU
#define LP_I2S_PARAM_INIT_FRAME_NUM_S  16

/** LP_I2S_VAD_PARAM1_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM1_REG (DR_REG_LP_I2S_BASE + 0x84)
/** LP_I2S_PARAM_MIN_SPEECH_COUNT : R/W; bitpos: [3:0]; default: 3;
 *  VAD parameter
 */
#define LP_I2S_PARAM_MIN_SPEECH_COUNT    0x0000000FU
#define LP_I2S_PARAM_MIN_SPEECH_COUNT_M  (LP_I2S_PARAM_MIN_SPEECH_COUNT_V << LP_I2S_PARAM_MIN_SPEECH_COUNT_S)
#define LP_I2S_PARAM_MIN_SPEECH_COUNT_V  0x0000000FU
#define LP_I2S_PARAM_MIN_SPEECH_COUNT_S  0
/** LP_I2S_PARAM_MAX_SPEECH_COUNT : R/W; bitpos: [10:4]; default: 100;
 *  VAD parameter
 */
#define LP_I2S_PARAM_MAX_SPEECH_COUNT    0x0000007FU
#define LP_I2S_PARAM_MAX_SPEECH_COUNT_M  (LP_I2S_PARAM_MAX_SPEECH_COUNT_V << LP_I2S_PARAM_MAX_SPEECH_COUNT_S)
#define LP_I2S_PARAM_MAX_SPEECH_COUNT_V  0x0000007FU
#define LP_I2S_PARAM_MAX_SPEECH_COUNT_S  4
/** LP_I2S_PARAM_HANGOVER_SPEECH : R/W; bitpos: [15:11]; default: 3;
 *  VAD parameter
 */
#define LP_I2S_PARAM_HANGOVER_SPEECH    0x0000001FU
#define LP_I2S_PARAM_HANGOVER_SPEECH_M  (LP_I2S_PARAM_HANGOVER_SPEECH_V << LP_I2S_PARAM_HANGOVER_SPEECH_S)
#define LP_I2S_PARAM_HANGOVER_SPEECH_V  0x0000001FU
#define LP_I2S_PARAM_HANGOVER_SPEECH_S  11
/** LP_I2S_PARAM_HANGOVER_SILENT : R/W; bitpos: [23:16]; default: 30;
 *  VAD parameter
 */
#define LP_I2S_PARAM_HANGOVER_SILENT    0x000000FFU
#define LP_I2S_PARAM_HANGOVER_SILENT_M  (LP_I2S_PARAM_HANGOVER_SILENT_V << LP_I2S_PARAM_HANGOVER_SILENT_S)
#define LP_I2S_PARAM_HANGOVER_SILENT_V  0x000000FFU
#define LP_I2S_PARAM_HANGOVER_SILENT_S  16
/** LP_I2S_PARAM_MAX_OFFSET : R/W; bitpos: [30:24]; default: 40;
 *  VAD parameter
 */
#define LP_I2S_PARAM_MAX_OFFSET    0x0000007FU
#define LP_I2S_PARAM_MAX_OFFSET_M  (LP_I2S_PARAM_MAX_OFFSET_V << LP_I2S_PARAM_MAX_OFFSET_S)
#define LP_I2S_PARAM_MAX_OFFSET_V  0x0000007FU
#define LP_I2S_PARAM_MAX_OFFSET_S  24
/** LP_I2S_PARAM_SKIP_BAND_ENERGY : R/W; bitpos: [31]; default: 0;
 *  Set 1 to skip band energy check.
 */
#define LP_I2S_PARAM_SKIP_BAND_ENERGY    (BIT(31))
#define LP_I2S_PARAM_SKIP_BAND_ENERGY_M  (LP_I2S_PARAM_SKIP_BAND_ENERGY_V << LP_I2S_PARAM_SKIP_BAND_ENERGY_S)
#define LP_I2S_PARAM_SKIP_BAND_ENERGY_V  0x00000001U
#define LP_I2S_PARAM_SKIP_BAND_ENERGY_S  31

/** LP_I2S_VAD_PARAM2_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM2_REG (DR_REG_LP_I2S_BASE + 0x88)
/** LP_I2S_PARAM_NOISE_AMP_DOWN : R/W; bitpos: [15:0]; default: 26214;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_AMP_DOWN    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_AMP_DOWN_M  (LP_I2S_PARAM_NOISE_AMP_DOWN_V << LP_I2S_PARAM_NOISE_AMP_DOWN_S)
#define LP_I2S_PARAM_NOISE_AMP_DOWN_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_AMP_DOWN_S  0
/** LP_I2S_PARAM_NOISE_AMP_UP : R/W; bitpos: [31:16]; default: 32440;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_AMP_UP    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_AMP_UP_M  (LP_I2S_PARAM_NOISE_AMP_UP_V << LP_I2S_PARAM_NOISE_AMP_UP_S)
#define LP_I2S_PARAM_NOISE_AMP_UP_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_AMP_UP_S  16

/** LP_I2S_VAD_PARAM3_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM3_REG (DR_REG_LP_I2S_BASE + 0x8c)
/** LP_I2S_PARAM_NOISE_SPE_UP0 : R/W; bitpos: [15:0]; default: 32735;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_SPE_UP0    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_SPE_UP0_M  (LP_I2S_PARAM_NOISE_SPE_UP0_V << LP_I2S_PARAM_NOISE_SPE_UP0_S)
#define LP_I2S_PARAM_NOISE_SPE_UP0_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_SPE_UP0_S  0
/** LP_I2S_PARAM_NOISE_SPE_UP1 : R/W; bitpos: [31:16]; default: 32113;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_SPE_UP1    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_SPE_UP1_M  (LP_I2S_PARAM_NOISE_SPE_UP1_V << LP_I2S_PARAM_NOISE_SPE_UP1_S)
#define LP_I2S_PARAM_NOISE_SPE_UP1_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_SPE_UP1_S  16

/** LP_I2S_VAD_PARAM4_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM4_REG (DR_REG_LP_I2S_BASE + 0x90)
/** LP_I2S_PARAM_NOISE_SPE_DOWN : R/W; bitpos: [15:0]; default: 26214;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_SPE_DOWN    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_SPE_DOWN_M  (LP_I2S_PARAM_NOISE_SPE_DOWN_V << LP_I2S_PARAM_NOISE_SPE_DOWN_S)
#define LP_I2S_PARAM_NOISE_SPE_DOWN_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_SPE_DOWN_S  0
/** LP_I2S_PARAM_NOISE_MEAN_DOWN : R/W; bitpos: [31:16]; default: 31130;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_MEAN_DOWN    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_MEAN_DOWN_M  (LP_I2S_PARAM_NOISE_MEAN_DOWN_V << LP_I2S_PARAM_NOISE_MEAN_DOWN_S)
#define LP_I2S_PARAM_NOISE_MEAN_DOWN_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_MEAN_DOWN_S  16

/** LP_I2S_VAD_PARAM5_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM5_REG (DR_REG_LP_I2S_BASE + 0x94)
/** LP_I2S_PARAM_NOISE_MEAN_UP0 : R/W; bitpos: [15:0]; default: 32113;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_MEAN_UP0    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_MEAN_UP0_M  (LP_I2S_PARAM_NOISE_MEAN_UP0_V << LP_I2S_PARAM_NOISE_MEAN_UP0_S)
#define LP_I2S_PARAM_NOISE_MEAN_UP0_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_MEAN_UP0_S  0
/** LP_I2S_PARAM_NOISE_MEAN_UP1 : R/W; bitpos: [31:16]; default: 31784;
 *  VAD parameter
 */
#define LP_I2S_PARAM_NOISE_MEAN_UP1    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_MEAN_UP1_M  (LP_I2S_PARAM_NOISE_MEAN_UP1_V << LP_I2S_PARAM_NOISE_MEAN_UP1_S)
#define LP_I2S_PARAM_NOISE_MEAN_UP1_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_MEAN_UP1_S  16

/** LP_I2S_VAD_PARAM6_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM6_REG (DR_REG_LP_I2S_BASE + 0x98)
/** LP_I2S_PARAM_NOISE_STD_FS_THSL : R/W; bitpos: [15:0]; default: 32000;
 *  Feature_sum threshold to determine noise_std max value when vad_tag=1, equal to
 *  ((noise_std_max)>>11)^2*5
 */
#define LP_I2S_PARAM_NOISE_STD_FS_THSL    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_STD_FS_THSL_M  (LP_I2S_PARAM_NOISE_STD_FS_THSL_V << LP_I2S_PARAM_NOISE_STD_FS_THSL_S)
#define LP_I2S_PARAM_NOISE_STD_FS_THSL_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_STD_FS_THSL_S  0
/** LP_I2S_PARAM_NOISE_STD_FS_THSH : R/W; bitpos: [31:16]; default: 46080;
 *  Feature_sum threshold to determine noise_std max value when vad_tag=0, equal to
 *  ((noise_std_max)>>11)^2*5
 */
#define LP_I2S_PARAM_NOISE_STD_FS_THSH    0x0000FFFFU
#define LP_I2S_PARAM_NOISE_STD_FS_THSH_M  (LP_I2S_PARAM_NOISE_STD_FS_THSH_V << LP_I2S_PARAM_NOISE_STD_FS_THSH_S)
#define LP_I2S_PARAM_NOISE_STD_FS_THSH_V  0x0000FFFFU
#define LP_I2S_PARAM_NOISE_STD_FS_THSH_S  16

/** LP_I2S_VAD_PARAM7_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM7_REG (DR_REG_LP_I2S_BASE + 0x9c)
/** LP_I2S_PARAM_THRES_UPD_BASE : R/W; bitpos: [15:0]; default: 32440;
 *  VAD parameter
 */
#define LP_I2S_PARAM_THRES_UPD_BASE    0x0000FFFFU
#define LP_I2S_PARAM_THRES_UPD_BASE_M  (LP_I2S_PARAM_THRES_UPD_BASE_V << LP_I2S_PARAM_THRES_UPD_BASE_S)
#define LP_I2S_PARAM_THRES_UPD_BASE_V  0x0000FFFFU
#define LP_I2S_PARAM_THRES_UPD_BASE_S  0
/** LP_I2S_PARAM_THRES_UPD_VARY : R/W; bitpos: [31:16]; default: 328;
 *  VAD parameter
 */
#define LP_I2S_PARAM_THRES_UPD_VARY    0x0000FFFFU
#define LP_I2S_PARAM_THRES_UPD_VARY_M  (LP_I2S_PARAM_THRES_UPD_VARY_V << LP_I2S_PARAM_THRES_UPD_VARY_S)
#define LP_I2S_PARAM_THRES_UPD_VARY_V  0x0000FFFFU
#define LP_I2S_PARAM_THRES_UPD_VARY_S  16

/** LP_I2S_VAD_PARAM8_REG register
 *  I2S VAD Parameter register
 */
#define LP_I2S_VAD_PARAM8_REG (DR_REG_LP_I2S_BASE + 0xa0)
/** LP_I2S_PARAM_THRES_UPD_BDL : R/W; bitpos: [7:0]; default: 64;
 *  Noise_std boundary low when updating threshold.
 */
#define LP_I2S_PARAM_THRES_UPD_BDL    0x000000FFU
#define LP_I2S_PARAM_THRES_UPD_BDL_M  (LP_I2S_PARAM_THRES_UPD_BDL_V << LP_I2S_PARAM_THRES_UPD_BDL_S)
#define LP_I2S_PARAM_THRES_UPD_BDL_V  0x000000FFU
#define LP_I2S_PARAM_THRES_UPD_BDL_S  0
/** LP_I2S_PARAM_THRES_UPD_BDH : R/W; bitpos: [15:8]; default: 80;
 *  Noise_std boundary high when updating threshold.
 */
#define LP_I2S_PARAM_THRES_UPD_BDH    0x000000FFU
#define LP_I2S_PARAM_THRES_UPD_BDH_M  (LP_I2S_PARAM_THRES_UPD_BDH_V << LP_I2S_PARAM_THRES_UPD_BDH_S)
#define LP_I2S_PARAM_THRES_UPD_BDH_V  0x000000FFU
#define LP_I2S_PARAM_THRES_UPD_BDH_S  8
/** LP_I2S_PARAM_FEATURE_BURST : R/W; bitpos: [31:16]; default: 8192;
 *  VAD parameter
 */
#define LP_I2S_PARAM_FEATURE_BURST    0x0000FFFFU
#define LP_I2S_PARAM_FEATURE_BURST_M  (LP_I2S_PARAM_FEATURE_BURST_V << LP_I2S_PARAM_FEATURE_BURST_S)
#define LP_I2S_PARAM_FEATURE_BURST_V  0x0000FFFFU
#define LP_I2S_PARAM_FEATURE_BURST_S  16

/** LP_I2S_VAD_OB0_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB0_REG (DR_REG_LP_I2S_BASE + 0xb0)
/** LP_I2S_SPEECH_COUNT_OB : RO; bitpos: [7:0]; default: 0;
 *  Reg silent count observe
 */
#define LP_I2S_SPEECH_COUNT_OB    0x000000FFU
#define LP_I2S_SPEECH_COUNT_OB_M  (LP_I2S_SPEECH_COUNT_OB_V << LP_I2S_SPEECH_COUNT_OB_S)
#define LP_I2S_SPEECH_COUNT_OB_V  0x000000FFU
#define LP_I2S_SPEECH_COUNT_OB_S  0
/** LP_I2S_SILENT_COUNT_OB : RO; bitpos: [15:8]; default: 0;
 *  Reg speech count observe
 */
#define LP_I2S_SILENT_COUNT_OB    0x000000FFU
#define LP_I2S_SILENT_COUNT_OB_M  (LP_I2S_SILENT_COUNT_OB_V << LP_I2S_SILENT_COUNT_OB_S)
#define LP_I2S_SILENT_COUNT_OB_V  0x000000FFU
#define LP_I2S_SILENT_COUNT_OB_S  8
/** LP_I2S_MAX_SIGNAL0_OB : RO; bitpos: [31:16]; default: 0;
 *  Reg max signal0 observe
 */
#define LP_I2S_MAX_SIGNAL0_OB    0x0000FFFFU
#define LP_I2S_MAX_SIGNAL0_OB_M  (LP_I2S_MAX_SIGNAL0_OB_V << LP_I2S_MAX_SIGNAL0_OB_S)
#define LP_I2S_MAX_SIGNAL0_OB_V  0x0000FFFFU
#define LP_I2S_MAX_SIGNAL0_OB_S  16

/** LP_I2S_VAD_OB1_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB1_REG (DR_REG_LP_I2S_BASE + 0xb4)
/** LP_I2S_MAX_SIGNAL1_OB : RO; bitpos: [15:0]; default: 0;
 *  Reg max signal1 observe
 */
#define LP_I2S_MAX_SIGNAL1_OB    0x0000FFFFU
#define LP_I2S_MAX_SIGNAL1_OB_M  (LP_I2S_MAX_SIGNAL1_OB_V << LP_I2S_MAX_SIGNAL1_OB_S)
#define LP_I2S_MAX_SIGNAL1_OB_V  0x0000FFFFU
#define LP_I2S_MAX_SIGNAL1_OB_S  0
/** LP_I2S_MAX_SIGNAL2_OB : RO; bitpos: [31:16]; default: 0;
 *  Reg max signal2 observe
 */
#define LP_I2S_MAX_SIGNAL2_OB    0x0000FFFFU
#define LP_I2S_MAX_SIGNAL2_OB_M  (LP_I2S_MAX_SIGNAL2_OB_V << LP_I2S_MAX_SIGNAL2_OB_S)
#define LP_I2S_MAX_SIGNAL2_OB_V  0x0000FFFFU
#define LP_I2S_MAX_SIGNAL2_OB_S  16

/** LP_I2S_VAD_OB2_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB2_REG (DR_REG_LP_I2S_BASE + 0xb8)
/** LP_I2S_NOISE_AMP_OB : RO; bitpos: [31:0]; default: 0;
 *  Reg noise_amp observe signal
 */
#define LP_I2S_NOISE_AMP_OB    0xFFFFFFFFU
#define LP_I2S_NOISE_AMP_OB_M  (LP_I2S_NOISE_AMP_OB_V << LP_I2S_NOISE_AMP_OB_S)
#define LP_I2S_NOISE_AMP_OB_V  0xFFFFFFFFU
#define LP_I2S_NOISE_AMP_OB_S  0

/** LP_I2S_VAD_OB3_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB3_REG (DR_REG_LP_I2S_BASE + 0xbc)
/** LP_I2S_NOISE_MEAN_OB : RO; bitpos: [31:0]; default: 0;
 *  Reg noise_mean observe signal
 */
#define LP_I2S_NOISE_MEAN_OB    0xFFFFFFFFU
#define LP_I2S_NOISE_MEAN_OB_M  (LP_I2S_NOISE_MEAN_OB_V << LP_I2S_NOISE_MEAN_OB_S)
#define LP_I2S_NOISE_MEAN_OB_V  0xFFFFFFFFU
#define LP_I2S_NOISE_MEAN_OB_S  0

/** LP_I2S_VAD_OB4_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB4_REG (DR_REG_LP_I2S_BASE + 0xc0)
/** LP_I2S_NOISE_STD_OB : RO; bitpos: [31:0]; default: 0;
 *  Reg noise_std observe signal
 */
#define LP_I2S_NOISE_STD_OB    0xFFFFFFFFU
#define LP_I2S_NOISE_STD_OB_M  (LP_I2S_NOISE_STD_OB_V << LP_I2S_NOISE_STD_OB_S)
#define LP_I2S_NOISE_STD_OB_V  0xFFFFFFFFU
#define LP_I2S_NOISE_STD_OB_S  0

/** LP_I2S_VAD_OB5_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB5_REG (DR_REG_LP_I2S_BASE + 0xc4)
/** LP_I2S_OFFSET_OB : RO; bitpos: [31:0]; default: 0;
 *  Reg offset observe signal
 */
#define LP_I2S_OFFSET_OB    0xFFFFFFFFU
#define LP_I2S_OFFSET_OB_M  (LP_I2S_OFFSET_OB_V << LP_I2S_OFFSET_OB_S)
#define LP_I2S_OFFSET_OB_V  0xFFFFFFFFU
#define LP_I2S_OFFSET_OB_S  0

/** LP_I2S_VAD_OB6_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB6_REG (DR_REG_LP_I2S_BASE + 0xc8)
/** LP_I2S_THRESHOLD_OB : RO; bitpos: [31:0]; default: 0;
 *  Reg threshold observe signal
 */
#define LP_I2S_THRESHOLD_OB    0xFFFFFFFFU
#define LP_I2S_THRESHOLD_OB_M  (LP_I2S_THRESHOLD_OB_V << LP_I2S_THRESHOLD_OB_S)
#define LP_I2S_THRESHOLD_OB_V  0xFFFFFFFFU
#define LP_I2S_THRESHOLD_OB_S  0

/** LP_I2S_VAD_OB7_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB7_REG (DR_REG_LP_I2S_BASE + 0xcc)
/** LP_I2S_ENERGY_LOW_OB : RO; bitpos: [31:0]; default: 0;
 *  Reg energy bit 31~0 observe signal
 */
#define LP_I2S_ENERGY_LOW_OB    0xFFFFFFFFU
#define LP_I2S_ENERGY_LOW_OB_M  (LP_I2S_ENERGY_LOW_OB_V << LP_I2S_ENERGY_LOW_OB_S)
#define LP_I2S_ENERGY_LOW_OB_V  0xFFFFFFFFU
#define LP_I2S_ENERGY_LOW_OB_S  0

/** LP_I2S_VAD_OB8_REG register
 *  I2S VAD Observe register
 */
#define LP_I2S_VAD_OB8_REG (DR_REG_LP_I2S_BASE + 0xd0)
/** LP_I2S_ENERGY_HIGH_OB : RO; bitpos: [31:0]; default: 0;
 *  Reg energy bit 63~32 observe signal
 */
#define LP_I2S_ENERGY_HIGH_OB    0xFFFFFFFFU
#define LP_I2S_ENERGY_HIGH_OB_M  (LP_I2S_ENERGY_HIGH_OB_V << LP_I2S_ENERGY_HIGH_OB_S)
#define LP_I2S_ENERGY_HIGH_OB_V  0xFFFFFFFFU
#define LP_I2S_ENERGY_HIGH_OB_S  0

/** LP_I2S_CLK_GATE_REG register
 *  Clock gate register
 */
#define LP_I2S_CLK_GATE_REG (DR_REG_LP_I2S_BASE + 0xf8)
/** LP_I2S_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  set this bit to enable clock gate
 */
#define LP_I2S_CLK_EN    (BIT(0))
#define LP_I2S_CLK_EN_M  (LP_I2S_CLK_EN_V << LP_I2S_CLK_EN_S)
#define LP_I2S_CLK_EN_V  0x00000001U
#define LP_I2S_CLK_EN_S  0
/** LP_I2S_VAD_CG_FORCE_ON : R/W; bitpos: [1]; default: 1;
 *  VAD clock gate force on register
 */
#define LP_I2S_VAD_CG_FORCE_ON    (BIT(1))
#define LP_I2S_VAD_CG_FORCE_ON_M  (LP_I2S_VAD_CG_FORCE_ON_V << LP_I2S_VAD_CG_FORCE_ON_S)
#define LP_I2S_VAD_CG_FORCE_ON_V  0x00000001U
#define LP_I2S_VAD_CG_FORCE_ON_S  1
/** LP_I2S_RX_MEM_CG_FORCE_ON : R/W; bitpos: [2]; default: 0;
 *  I2S rx mem clock gate force on register
 */
#define LP_I2S_RX_MEM_CG_FORCE_ON    (BIT(2))
#define LP_I2S_RX_MEM_CG_FORCE_ON_M  (LP_I2S_RX_MEM_CG_FORCE_ON_V << LP_I2S_RX_MEM_CG_FORCE_ON_S)
#define LP_I2S_RX_MEM_CG_FORCE_ON_V  0x00000001U
#define LP_I2S_RX_MEM_CG_FORCE_ON_S  2
/** LP_I2S_RX_REG_CG_FORCE_ON : R/W; bitpos: [3]; default: 1;
 *  I2S rx reg clock gate force on register
 */
#define LP_I2S_RX_REG_CG_FORCE_ON    (BIT(3))
#define LP_I2S_RX_REG_CG_FORCE_ON_M  (LP_I2S_RX_REG_CG_FORCE_ON_V << LP_I2S_RX_REG_CG_FORCE_ON_S)
#define LP_I2S_RX_REG_CG_FORCE_ON_V  0x00000001U
#define LP_I2S_RX_REG_CG_FORCE_ON_S  3

/** LP_I2S_DATE_REG register
 *  Version control register
 */
#define LP_I2S_DATE_REG (DR_REG_LP_I2S_BASE + 0xfc)
/** LP_I2S_DATE : R/W; bitpos: [27:0]; default: 36720704;
 *  I2S version control register
 */
#define LP_I2S_DATE    0x0FFFFFFFU
#define LP_I2S_DATE_M  (LP_I2S_DATE_V << LP_I2S_DATE_S)
#define LP_I2S_DATE_V  0x0FFFFFFFU
#define LP_I2S_DATE_S  0

#ifdef __cplusplus
}
#endif
