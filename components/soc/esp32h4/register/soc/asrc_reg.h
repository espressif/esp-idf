/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** ASRC_CHNL0_CFG0_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_CFG0_REG (DR_REG_ASRC_BASE + 0x0)
/** ASRC_CHNL0_RS2_STG1_BYPASS : R/W; bitpos: [0]; default: 1;
 *  Set this bit to bypass stage 1 re-sampler in channel0.
 */
#define ASRC_CHNL0_RS2_STG1_BYPASS    (BIT(0))
#define ASRC_CHNL0_RS2_STG1_BYPASS_M  (ASRC_CHNL0_RS2_STG1_BYPASS_V << ASRC_CHNL0_RS2_STG1_BYPASS_S)
#define ASRC_CHNL0_RS2_STG1_BYPASS_V  0x00000001U
#define ASRC_CHNL0_RS2_STG1_BYPASS_S  0
/** ASRC_CHNL0_RS2_STG0_BYPASS : R/W; bitpos: [1]; default: 1;
 *  Set this bit to bypass stage 0 re-sampler in channel0.
 */
#define ASRC_CHNL0_RS2_STG0_BYPASS    (BIT(1))
#define ASRC_CHNL0_RS2_STG0_BYPASS_M  (ASRC_CHNL0_RS2_STG0_BYPASS_V << ASRC_CHNL0_RS2_STG0_BYPASS_S)
#define ASRC_CHNL0_RS2_STG0_BYPASS_V  0x00000001U
#define ASRC_CHNL0_RS2_STG0_BYPASS_S  1
/** ASRC_CHNL0_FRAC_BYPASS : R/W; bitpos: [2]; default: 1;
 *  Set this bit to bypass fractional re-sampler in channel0.
 */
#define ASRC_CHNL0_FRAC_BYPASS    (BIT(2))
#define ASRC_CHNL0_FRAC_BYPASS_M  (ASRC_CHNL0_FRAC_BYPASS_V << ASRC_CHNL0_FRAC_BYPASS_S)
#define ASRC_CHNL0_FRAC_BYPASS_V  0x00000001U
#define ASRC_CHNL0_FRAC_BYPASS_S  2
/** ASRC_CHNL0_RS2_STG1_MODE : R/W; bitpos: [3]; default: 0;
 *  Write this bit to configure stage 1 re-sampler mode in channel0, 0: interpolation
 *  by  factor of 2, 1: decimation by  factor of 2.
 */
#define ASRC_CHNL0_RS2_STG1_MODE    (BIT(3))
#define ASRC_CHNL0_RS2_STG1_MODE_M  (ASRC_CHNL0_RS2_STG1_MODE_V << ASRC_CHNL0_RS2_STG1_MODE_S)
#define ASRC_CHNL0_RS2_STG1_MODE_V  0x00000001U
#define ASRC_CHNL0_RS2_STG1_MODE_S  3
/** ASRC_CHNL0_RS2_STG0_MODE : R/W; bitpos: [4]; default: 0;
 *  Write this bit to configure stage 0 re-sampler mode in channel0, 0: interpolation
 *  by  factor of 2, 1: decimation by  factor of 2.
 */
#define ASRC_CHNL0_RS2_STG0_MODE    (BIT(4))
#define ASRC_CHNL0_RS2_STG0_MODE_M  (ASRC_CHNL0_RS2_STG0_MODE_V << ASRC_CHNL0_RS2_STG0_MODE_S)
#define ASRC_CHNL0_RS2_STG0_MODE_V  0x00000001U
#define ASRC_CHNL0_RS2_STG0_MODE_S  4
/** ASRC_CHNL0_FRAC_AHEAD : R/W; bitpos: [5]; default: 0;
 *  Set this bit to move fraction re-sampler to the first stage in channel0, it should
 *  be 1 when input frequency is higher the output.
 */
#define ASRC_CHNL0_FRAC_AHEAD    (BIT(5))
#define ASRC_CHNL0_FRAC_AHEAD_M  (ASRC_CHNL0_FRAC_AHEAD_V << ASRC_CHNL0_FRAC_AHEAD_S)
#define ASRC_CHNL0_FRAC_AHEAD_V  0x00000001U
#define ASRC_CHNL0_FRAC_AHEAD_S  5
/** ASRC_CHNL0_MODE : R/W; bitpos: [8:7]; default: 0;
 *  Write the bit to configure the channel mode,0: in and out are both mono, 1: in and
 *  out is both dual, 2: in is mono, out is dual, 3, in is dual, out is mono.
 */
#define ASRC_CHNL0_MODE    0x00000003U
#define ASRC_CHNL0_MODE_M  (ASRC_CHNL0_MODE_V << ASRC_CHNL0_MODE_S)
#define ASRC_CHNL0_MODE_V  0x00000003U
#define ASRC_CHNL0_MODE_S  7
/** ASRC_CHNL0_SEL : R/W; bitpos: [9]; default: 0;
 *  Write the bit to configure which 16bits data will be processing.
 */
#define ASRC_CHNL0_SEL    (BIT(9))
#define ASRC_CHNL0_SEL_M  (ASRC_CHNL0_SEL_V << ASRC_CHNL0_SEL_S)
#define ASRC_CHNL0_SEL_V  0x00000001U
#define ASRC_CHNL0_SEL_S  9

/** ASRC_CHNL0_CFG1_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_CFG1_REG (DR_REG_ASRC_BASE + 0x4)
/** ASRC_CHNL0_FRAC_M : R/W; bitpos: [9:0]; default: 0;
 *  Write the bits to specify the denominator of factor of fraction re-sampler in
 *  channel0, reg_chnl0_frac_m and reg_chnl0_frac_l are relatively prime.
 */
#define ASRC_CHNL0_FRAC_M    0x000003FFU
#define ASRC_CHNL0_FRAC_M_M  (ASRC_CHNL0_FRAC_M_V << ASRC_CHNL0_FRAC_M_S)
#define ASRC_CHNL0_FRAC_M_V  0x000003FFU
#define ASRC_CHNL0_FRAC_M_S  0
/** ASRC_CHNL0_FRAC_L : R/W; bitpos: [19:10]; default: 0;
 *  Write the bits to specify the nominator of factor of fraction re-sampler in
 *  channel0, reg_chnl0_frac_l and reg_chnl0_frac_m are relatively prime.
 */
#define ASRC_CHNL0_FRAC_L    0x000003FFU
#define ASRC_CHNL0_FRAC_L_M  (ASRC_CHNL0_FRAC_L_V << ASRC_CHNL0_FRAC_L_S)
#define ASRC_CHNL0_FRAC_L_V  0x000003FFU
#define ASRC_CHNL0_FRAC_L_S  10

/** ASRC_CHNL0_CFG2_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_CFG2_REG (DR_REG_ASRC_BASE + 0x8)
/** ASRC_CHNL0_FRAC_RECIPL : R/W; bitpos: [19:0]; default: 0;
 *  Write the bits with ((2^19+L)/(2L)) round down in channel0.
 */
#define ASRC_CHNL0_FRAC_RECIPL    0x000FFFFFU
#define ASRC_CHNL0_FRAC_RECIPL_M  (ASRC_CHNL0_FRAC_RECIPL_V << ASRC_CHNL0_FRAC_RECIPL_S)
#define ASRC_CHNL0_FRAC_RECIPL_V  0x000FFFFFU
#define ASRC_CHNL0_FRAC_RECIPL_S  0

/** ASRC_CHNL0_CFG3_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_CFG3_REG (DR_REG_ASRC_BASE + 0xc)
/** ASRC_CHNL0_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset channel0.
 */
#define ASRC_CHNL0_RESET    (BIT(0))
#define ASRC_CHNL0_RESET_M  (ASRC_CHNL0_RESET_V << ASRC_CHNL0_RESET_S)
#define ASRC_CHNL0_RESET_V  0x00000001U
#define ASRC_CHNL0_RESET_S  0

/** ASRC_CHNL0_CFG4_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_CFG4_REG (DR_REG_ASRC_BASE + 0x10)
/** ASRC_CHNL0_START : R/W; bitpos: [0]; default: 0;
 *  Set this bit to start channel0.
 */
#define ASRC_CHNL0_START    (BIT(0))
#define ASRC_CHNL0_START_M  (ASRC_CHNL0_START_V << ASRC_CHNL0_START_S)
#define ASRC_CHNL0_START_V  0x00000001U
#define ASRC_CHNL0_START_S  0

/** ASRC_CHNL0_CFG5_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_CFG5_REG (DR_REG_ASRC_BASE + 0x14)
/** ASRC_CHNL0_IN_CNT_ENA : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable in data byte counter.
 */
#define ASRC_CHNL0_IN_CNT_ENA    (BIT(0))
#define ASRC_CHNL0_IN_CNT_ENA_M  (ASRC_CHNL0_IN_CNT_ENA_V << ASRC_CHNL0_IN_CNT_ENA_S)
#define ASRC_CHNL0_IN_CNT_ENA_V  0x00000001U
#define ASRC_CHNL0_IN_CNT_ENA_S  0
/** ASRC_CHNL0_IN_CNT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear in data byte counter.
 */
#define ASRC_CHNL0_IN_CNT_CLR    (BIT(1))
#define ASRC_CHNL0_IN_CNT_CLR_M  (ASRC_CHNL0_IN_CNT_CLR_V << ASRC_CHNL0_IN_CNT_CLR_S)
#define ASRC_CHNL0_IN_CNT_CLR_V  0x00000001U
#define ASRC_CHNL0_IN_CNT_CLR_S  1
/** ASRC_CHNL0_IN_LEN : R/W; bitpos: [31:8]; default: 0;
 *  Write the bits to specify the data byte number of data from the DMA
 */
#define ASRC_CHNL0_IN_LEN    0x00FFFFFFU
#define ASRC_CHNL0_IN_LEN_M  (ASRC_CHNL0_IN_LEN_V << ASRC_CHNL0_IN_LEN_S)
#define ASRC_CHNL0_IN_LEN_V  0x00FFFFFFU
#define ASRC_CHNL0_IN_LEN_S  8

/** ASRC_CHNL0_CFG6_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_CFG6_REG (DR_REG_ASRC_BASE + 0x18)
/** ASRC_CHNL0_OUT_EOF_GEN_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Write the bits to specify the which eof will be written to DMA. 0:  counter eof, 1:
 *  DMA ineof, 2: both counter eof and DMA ineof, 3 none.
 */
#define ASRC_CHNL0_OUT_EOF_GEN_MODE    0x00000003U
#define ASRC_CHNL0_OUT_EOF_GEN_MODE_M  (ASRC_CHNL0_OUT_EOF_GEN_MODE_V << ASRC_CHNL0_OUT_EOF_GEN_MODE_S)
#define ASRC_CHNL0_OUT_EOF_GEN_MODE_V  0x00000003U
#define ASRC_CHNL0_OUT_EOF_GEN_MODE_S  0
/** ASRC_CHNL0_OUT_CNT_ENA : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable out data byte counter.
 */
#define ASRC_CHNL0_OUT_CNT_ENA    (BIT(2))
#define ASRC_CHNL0_OUT_CNT_ENA_M  (ASRC_CHNL0_OUT_CNT_ENA_V << ASRC_CHNL0_OUT_CNT_ENA_S)
#define ASRC_CHNL0_OUT_CNT_ENA_V  0x00000001U
#define ASRC_CHNL0_OUT_CNT_ENA_S  2
/** ASRC_CHNL0_OUT_CNT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear out data byte counter.
 */
#define ASRC_CHNL0_OUT_CNT_CLR    (BIT(3))
#define ASRC_CHNL0_OUT_CNT_CLR_M  (ASRC_CHNL0_OUT_CNT_CLR_V << ASRC_CHNL0_OUT_CNT_CLR_S)
#define ASRC_CHNL0_OUT_CNT_CLR_V  0x00000001U
#define ASRC_CHNL0_OUT_CNT_CLR_S  3
/** ASRC_CHNL0_OUT_LEN_COMP : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable out data byte counter compensation when using fractional
 *  re-sampler and decimation by factor of 2 which results in reg_chnl0_out_cnt >=
 *  reg_chnl0_out_len
 */
#define ASRC_CHNL0_OUT_LEN_COMP    (BIT(4))
#define ASRC_CHNL0_OUT_LEN_COMP_M  (ASRC_CHNL0_OUT_LEN_COMP_V << ASRC_CHNL0_OUT_LEN_COMP_S)
#define ASRC_CHNL0_OUT_LEN_COMP_V  0x00000001U
#define ASRC_CHNL0_OUT_LEN_COMP_S  4
/** ASRC_CHNL0_OUT_LEN : R/W; bitpos: [31:8]; default: 0;
 *  Write the bits to specify the data byte number of data to the DMA, the counter eof
 *  will be set when the counter approaches.
 */
#define ASRC_CHNL0_OUT_LEN    0x00FFFFFFU
#define ASRC_CHNL0_OUT_LEN_M  (ASRC_CHNL0_OUT_LEN_V << ASRC_CHNL0_OUT_LEN_S)
#define ASRC_CHNL0_OUT_LEN_V  0x00FFFFFFU
#define ASRC_CHNL0_OUT_LEN_S  8

/** ASRC_CHNL0_FIFO_CTRL_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL0_FIFO_CTRL_REG (DR_REG_ASRC_BASE + 0x1c)
/** ASRC_CHNL0_INFIFO_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset outfifo.
 */
#define ASRC_CHNL0_INFIFO_RESET    (BIT(0))
#define ASRC_CHNL0_INFIFO_RESET_M  (ASRC_CHNL0_INFIFO_RESET_V << ASRC_CHNL0_INFIFO_RESET_S)
#define ASRC_CHNL0_INFIFO_RESET_V  0x00000001U
#define ASRC_CHNL0_INFIFO_RESET_S  0
/** ASRC_CHNL0_OUTFIFO_RESET : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset infifo.
 */
#define ASRC_CHNL0_OUTFIFO_RESET    (BIT(1))
#define ASRC_CHNL0_OUTFIFO_RESET_M  (ASRC_CHNL0_OUTFIFO_RESET_V << ASRC_CHNL0_OUTFIFO_RESET_S)
#define ASRC_CHNL0_OUTFIFO_RESET_V  0x00000001U
#define ASRC_CHNL0_OUTFIFO_RESET_S  1

/** ASRC_CHNL0_INT_RAW_REG register
 *  Raw interrupt status
 */
#define ASRC_CHNL0_INT_RAW_REG (DR_REG_ASRC_BASE + 0x20)
/** ASRC_CHNL0_OUTCNT_EOF_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  This interrupt raw bit turns to high level when the counter approach to reg_out_len.
 */
#define ASRC_CHNL0_OUTCNT_EOF_INT_RAW    (BIT(0))
#define ASRC_CHNL0_OUTCNT_EOF_INT_RAW_M  (ASRC_CHNL0_OUTCNT_EOF_INT_RAW_V << ASRC_CHNL0_OUTCNT_EOF_INT_RAW_S)
#define ASRC_CHNL0_OUTCNT_EOF_INT_RAW_V  0x00000001U
#define ASRC_CHNL0_OUTCNT_EOF_INT_RAW_S  0

/** ASRC_CHNL0_INT_ST_REG register
 *  Masked interrupt status
 */
#define ASRC_CHNL0_INT_ST_REG (DR_REG_ASRC_BASE + 0x24)
/** ASRC_CHNL0_OUTCNT_EOF_INT_ST : RO; bitpos: [0]; default: 0;
 *  This is the status bit for reg_out_cnt_eof_int_raw when reg_out_cnt_eof_int_ena is
 *  set to 1.
 */
#define ASRC_CHNL0_OUTCNT_EOF_INT_ST    (BIT(0))
#define ASRC_CHNL0_OUTCNT_EOF_INT_ST_M  (ASRC_CHNL0_OUTCNT_EOF_INT_ST_V << ASRC_CHNL0_OUTCNT_EOF_INT_ST_S)
#define ASRC_CHNL0_OUTCNT_EOF_INT_ST_V  0x00000001U
#define ASRC_CHNL0_OUTCNT_EOF_INT_ST_S  0

/** ASRC_CHNL0_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define ASRC_CHNL0_INT_ENA_REG (DR_REG_ASRC_BASE + 0x28)
/** ASRC_CHNL0_OUTCNT_EOF_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  This is the enable bit for reg_out_cnt_eof_int_st register.
 */
#define ASRC_CHNL0_OUTCNT_EOF_INT_ENA    (BIT(0))
#define ASRC_CHNL0_OUTCNT_EOF_INT_ENA_M  (ASRC_CHNL0_OUTCNT_EOF_INT_ENA_V << ASRC_CHNL0_OUTCNT_EOF_INT_ENA_S)
#define ASRC_CHNL0_OUTCNT_EOF_INT_ENA_V  0x00000001U
#define ASRC_CHNL0_OUTCNT_EOF_INT_ENA_S  0

/** ASRC_CHNL0_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define ASRC_CHNL0_INT_CLR_REG (DR_REG_ASRC_BASE + 0x2c)
/** ASRC_CHNL0_OUTCNT_EOF_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the reg_out_cnt_eof_int_raw interrupt.
 */
#define ASRC_CHNL0_OUTCNT_EOF_INT_CLR    (BIT(0))
#define ASRC_CHNL0_OUTCNT_EOF_INT_CLR_M  (ASRC_CHNL0_OUTCNT_EOF_INT_CLR_V << ASRC_CHNL0_OUTCNT_EOF_INT_CLR_S)
#define ASRC_CHNL0_OUTCNT_EOF_INT_CLR_V  0x00000001U
#define ASRC_CHNL0_OUTCNT_EOF_INT_CLR_S  0

/** ASRC_CHNL0_OUT_CNT_REG register
 *  Status Registers
 */
#define ASRC_CHNL0_OUT_CNT_REG (DR_REG_ASRC_BASE + 0x30)
/** ASRC_CHNL0_OUT_CNT : RO; bitpos: [23:0]; default: 0;
 *  Represents the bytes numbers send to the DMA when EOF occurs.
 */
#define ASRC_CHNL0_OUT_CNT    0x00FFFFFFU
#define ASRC_CHNL0_OUT_CNT_M  (ASRC_CHNL0_OUT_CNT_V << ASRC_CHNL0_OUT_CNT_S)
#define ASRC_CHNL0_OUT_CNT_V  0x00FFFFFFU
#define ASRC_CHNL0_OUT_CNT_S  0

/** ASRC_CHNL0_TRACE1_REG register
 *  Debug Register1
 */
#define ASRC_CHNL0_TRACE1_REG (DR_REG_ASRC_BASE + 0x38)
/** ASRC_CHNL0_OUT_INC : RO; bitpos: [23:0]; default: 0;
 *  Represents the samples numbers send to the DMA
 */
#define ASRC_CHNL0_OUT_INC    0x00FFFFFFU
#define ASRC_CHNL0_OUT_INC_M  (ASRC_CHNL0_OUT_INC_V << ASRC_CHNL0_OUT_INC_S)
#define ASRC_CHNL0_OUT_INC_V  0x00FFFFFFU
#define ASRC_CHNL0_OUT_INC_S  0

/** ASRC_CHNL1_CFG0_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_CFG0_REG (DR_REG_ASRC_BASE + 0x3c)
/** ASRC_CHNL1_RS2_STG1_BYPASS : R/W; bitpos: [0]; default: 1;
 *  Set this bit to bypass stage 1 re-sampler in channel1.
 */
#define ASRC_CHNL1_RS2_STG1_BYPASS    (BIT(0))
#define ASRC_CHNL1_RS2_STG1_BYPASS_M  (ASRC_CHNL1_RS2_STG1_BYPASS_V << ASRC_CHNL1_RS2_STG1_BYPASS_S)
#define ASRC_CHNL1_RS2_STG1_BYPASS_V  0x00000001U
#define ASRC_CHNL1_RS2_STG1_BYPASS_S  0
/** ASRC_CHNL1_RS2_STG0_BYPASS : R/W; bitpos: [1]; default: 1;
 *  Set this bit to bypass stage 0 re-sampler in channel1.
 */
#define ASRC_CHNL1_RS2_STG0_BYPASS    (BIT(1))
#define ASRC_CHNL1_RS2_STG0_BYPASS_M  (ASRC_CHNL1_RS2_STG0_BYPASS_V << ASRC_CHNL1_RS2_STG0_BYPASS_S)
#define ASRC_CHNL1_RS2_STG0_BYPASS_V  0x00000001U
#define ASRC_CHNL1_RS2_STG0_BYPASS_S  1
/** ASRC_CHNL1_FRAC_BYPASS : R/W; bitpos: [2]; default: 1;
 *  Set this bit to bypass fractional re-sampler in channel1.
 */
#define ASRC_CHNL1_FRAC_BYPASS    (BIT(2))
#define ASRC_CHNL1_FRAC_BYPASS_M  (ASRC_CHNL1_FRAC_BYPASS_V << ASRC_CHNL1_FRAC_BYPASS_S)
#define ASRC_CHNL1_FRAC_BYPASS_V  0x00000001U
#define ASRC_CHNL1_FRAC_BYPASS_S  2
/** ASRC_CHNL1_RS2_STG1_MODE : R/W; bitpos: [3]; default: 0;
 *  Write this bit to configure stage 1 re-sampler mode in channel1, 0: interpolation
 *  by  factor of 2, 1: decimation by  factor of 2.
 */
#define ASRC_CHNL1_RS2_STG1_MODE    (BIT(3))
#define ASRC_CHNL1_RS2_STG1_MODE_M  (ASRC_CHNL1_RS2_STG1_MODE_V << ASRC_CHNL1_RS2_STG1_MODE_S)
#define ASRC_CHNL1_RS2_STG1_MODE_V  0x00000001U
#define ASRC_CHNL1_RS2_STG1_MODE_S  3
/** ASRC_CHNL1_RS2_STG0_MODE : R/W; bitpos: [4]; default: 0;
 *  Write this bit to configure stage 0 re-sampler mode in channel1, 0: interpolation
 *  by  factor of 2, 1: decimation by  factor of 2.
 */
#define ASRC_CHNL1_RS2_STG0_MODE    (BIT(4))
#define ASRC_CHNL1_RS2_STG0_MODE_M  (ASRC_CHNL1_RS2_STG0_MODE_V << ASRC_CHNL1_RS2_STG0_MODE_S)
#define ASRC_CHNL1_RS2_STG0_MODE_V  0x00000001U
#define ASRC_CHNL1_RS2_STG0_MODE_S  4
/** ASRC_CHNL1_FRAC_AHEAD : R/W; bitpos: [5]; default: 0;
 *  Set this bit to move fraction re-sampler to the first stage in channel1, it should
 *  be 1 when input frequency is higher the output.
 */
#define ASRC_CHNL1_FRAC_AHEAD    (BIT(5))
#define ASRC_CHNL1_FRAC_AHEAD_M  (ASRC_CHNL1_FRAC_AHEAD_V << ASRC_CHNL1_FRAC_AHEAD_S)
#define ASRC_CHNL1_FRAC_AHEAD_V  0x00000001U
#define ASRC_CHNL1_FRAC_AHEAD_S  5
/** ASRC_CHNL1_MODE : R/W; bitpos: [8:7]; default: 0;
 *  Write the bit to configure the channel mode,0: in and out are both mono, 1: in and
 *  out is both dual, 2: in is mono, out is dual, 3, in is dual, out is mono.
 */
#define ASRC_CHNL1_MODE    0x00000003U
#define ASRC_CHNL1_MODE_M  (ASRC_CHNL1_MODE_V << ASRC_CHNL1_MODE_S)
#define ASRC_CHNL1_MODE_V  0x00000003U
#define ASRC_CHNL1_MODE_S  7
/** ASRC_CHNL1_SEL : R/W; bitpos: [9]; default: 0;
 *  Write the bit to configure which 16bits data will be processing.
 */
#define ASRC_CHNL1_SEL    (BIT(9))
#define ASRC_CHNL1_SEL_M  (ASRC_CHNL1_SEL_V << ASRC_CHNL1_SEL_S)
#define ASRC_CHNL1_SEL_V  0x00000001U
#define ASRC_CHNL1_SEL_S  9

/** ASRC_CHNL1_CFG1_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_CFG1_REG (DR_REG_ASRC_BASE + 0x40)
/** ASRC_CHNL1_FRAC_M : R/W; bitpos: [9:0]; default: 0;
 *  Write the bits to specify the denominator of factor of fraction re-sampler in
 *  channel1, reg_chnl0_frac_m and reg_chnl0_frac_l are relatively prime.
 */
#define ASRC_CHNL1_FRAC_M    0x000003FFU
#define ASRC_CHNL1_FRAC_M_M  (ASRC_CHNL1_FRAC_M_V << ASRC_CHNL1_FRAC_M_S)
#define ASRC_CHNL1_FRAC_M_V  0x000003FFU
#define ASRC_CHNL1_FRAC_M_S  0
/** ASRC_CHNL1_FRAC_L : R/W; bitpos: [19:10]; default: 0;
 *  Write the bits to specify the nominator of factor of fraction re-sampler in
 *  channel1, reg_chnl0_frac_l and reg_chnl0_frac_m are relatively prime.
 */
#define ASRC_CHNL1_FRAC_L    0x000003FFU
#define ASRC_CHNL1_FRAC_L_M  (ASRC_CHNL1_FRAC_L_V << ASRC_CHNL1_FRAC_L_S)
#define ASRC_CHNL1_FRAC_L_V  0x000003FFU
#define ASRC_CHNL1_FRAC_L_S  10

/** ASRC_CHNL1_CFG2_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_CFG2_REG (DR_REG_ASRC_BASE + 0x44)
/** ASRC_CHNL1_FRAC_RECIPL : R/W; bitpos: [19:0]; default: 0;
 *  Write the bits with ((2^19+L)/(2L)) round down in channel1.
 */
#define ASRC_CHNL1_FRAC_RECIPL    0x000FFFFFU
#define ASRC_CHNL1_FRAC_RECIPL_M  (ASRC_CHNL1_FRAC_RECIPL_V << ASRC_CHNL1_FRAC_RECIPL_S)
#define ASRC_CHNL1_FRAC_RECIPL_V  0x000FFFFFU
#define ASRC_CHNL1_FRAC_RECIPL_S  0

/** ASRC_CHNL1_CFG3_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_CFG3_REG (DR_REG_ASRC_BASE + 0x48)
/** ASRC_CHNL1_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset channel1.
 */
#define ASRC_CHNL1_RESET    (BIT(0))
#define ASRC_CHNL1_RESET_M  (ASRC_CHNL1_RESET_V << ASRC_CHNL1_RESET_S)
#define ASRC_CHNL1_RESET_V  0x00000001U
#define ASRC_CHNL1_RESET_S  0

/** ASRC_CHNL1_CFG4_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_CFG4_REG (DR_REG_ASRC_BASE + 0x4c)
/** ASRC_CHNL1_START : R/W; bitpos: [0]; default: 0;
 *  Set this bit to start channel1.
 */
#define ASRC_CHNL1_START    (BIT(0))
#define ASRC_CHNL1_START_M  (ASRC_CHNL1_START_V << ASRC_CHNL1_START_S)
#define ASRC_CHNL1_START_V  0x00000001U
#define ASRC_CHNL1_START_S  0

/** ASRC_CHNL1_CFG5_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_CFG5_REG (DR_REG_ASRC_BASE + 0x50)
/** ASRC_CHNL1_IN_CNT_ENA : R/W; bitpos: [0]; default: 0;
 *  Set this bit to enable in data byte counter.
 */
#define ASRC_CHNL1_IN_CNT_ENA    (BIT(0))
#define ASRC_CHNL1_IN_CNT_ENA_M  (ASRC_CHNL1_IN_CNT_ENA_V << ASRC_CHNL1_IN_CNT_ENA_S)
#define ASRC_CHNL1_IN_CNT_ENA_V  0x00000001U
#define ASRC_CHNL1_IN_CNT_ENA_S  0
/** ASRC_CHNL1_IN_CNT_CLR : WT; bitpos: [1]; default: 0;
 *  Set this bit to clear in data byte counter.
 */
#define ASRC_CHNL1_IN_CNT_CLR    (BIT(1))
#define ASRC_CHNL1_IN_CNT_CLR_M  (ASRC_CHNL1_IN_CNT_CLR_V << ASRC_CHNL1_IN_CNT_CLR_S)
#define ASRC_CHNL1_IN_CNT_CLR_V  0x00000001U
#define ASRC_CHNL1_IN_CNT_CLR_S  1
/** ASRC_CHNL1_IN_LEN : R/W; bitpos: [31:8]; default: 0;
 *  Write the bits to specify the data byte numbers of data from the DMA
 */
#define ASRC_CHNL1_IN_LEN    0x00FFFFFFU
#define ASRC_CHNL1_IN_LEN_M  (ASRC_CHNL1_IN_LEN_V << ASRC_CHNL1_IN_LEN_S)
#define ASRC_CHNL1_IN_LEN_V  0x00FFFFFFU
#define ASRC_CHNL1_IN_LEN_S  8

/** ASRC_CHNL1_CFG6_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_CFG6_REG (DR_REG_ASRC_BASE + 0x54)
/** ASRC_CHNL1_OUT_EOF_GEN_MODE : R/W; bitpos: [1:0]; default: 0;
 *  Write the bits to specify the which eof will be written to DMA. 0:  counter eof, 1:
 *  DMA ineof, 2: both counter eof and DMA ineof, 3 none.
 */
#define ASRC_CHNL1_OUT_EOF_GEN_MODE    0x00000003U
#define ASRC_CHNL1_OUT_EOF_GEN_MODE_M  (ASRC_CHNL1_OUT_EOF_GEN_MODE_V << ASRC_CHNL1_OUT_EOF_GEN_MODE_S)
#define ASRC_CHNL1_OUT_EOF_GEN_MODE_V  0x00000003U
#define ASRC_CHNL1_OUT_EOF_GEN_MODE_S  0
/** ASRC_CHNL1_OUT_CNT_ENA : R/W; bitpos: [2]; default: 0;
 *  Set this bit to enable out data byte counter.
 */
#define ASRC_CHNL1_OUT_CNT_ENA    (BIT(2))
#define ASRC_CHNL1_OUT_CNT_ENA_M  (ASRC_CHNL1_OUT_CNT_ENA_V << ASRC_CHNL1_OUT_CNT_ENA_S)
#define ASRC_CHNL1_OUT_CNT_ENA_V  0x00000001U
#define ASRC_CHNL1_OUT_CNT_ENA_S  2
/** ASRC_CHNL1_OUT_CNT_CLR : WT; bitpos: [3]; default: 0;
 *  Set this bit to clear out data byte counter.
 */
#define ASRC_CHNL1_OUT_CNT_CLR    (BIT(3))
#define ASRC_CHNL1_OUT_CNT_CLR_M  (ASRC_CHNL1_OUT_CNT_CLR_V << ASRC_CHNL1_OUT_CNT_CLR_S)
#define ASRC_CHNL1_OUT_CNT_CLR_V  0x00000001U
#define ASRC_CHNL1_OUT_CNT_CLR_S  3
/** ASRC_CHNL1_OUT_SAMPLES_COMP : R/W; bitpos: [4]; default: 0;
 *  Set this bit to enable out data byte counter compensation when using fractional
 *  re-sampler and decimation by factor of 2 which results in reg_chnl1_out_cnt >=
 *  reg_chnl1_out_len
 */
#define ASRC_CHNL1_OUT_SAMPLES_COMP    (BIT(4))
#define ASRC_CHNL1_OUT_SAMPLES_COMP_M  (ASRC_CHNL1_OUT_SAMPLES_COMP_V << ASRC_CHNL1_OUT_SAMPLES_COMP_S)
#define ASRC_CHNL1_OUT_SAMPLES_COMP_V  0x00000001U
#define ASRC_CHNL1_OUT_SAMPLES_COMP_S  4
/** ASRC_CHNL1_OUT_LEN : R/W; bitpos: [31:8]; default: 0;
 *  Write the bits to specify the data byte number of data to the DMA, the counter eof
 *  will be set when the counter approaches.
 */
#define ASRC_CHNL1_OUT_LEN    0x00FFFFFFU
#define ASRC_CHNL1_OUT_LEN_M  (ASRC_CHNL1_OUT_LEN_V << ASRC_CHNL1_OUT_LEN_S)
#define ASRC_CHNL1_OUT_LEN_V  0x00FFFFFFU
#define ASRC_CHNL1_OUT_LEN_S  8

/** ASRC_CHNL1_FIFO_CTRL_REG register
 *  Control and configuration registers
 */
#define ASRC_CHNL1_FIFO_CTRL_REG (DR_REG_ASRC_BASE + 0x58)
/** ASRC_CHNL1_INFIFO_RESET : WT; bitpos: [0]; default: 0;
 *  Set this bit to reset outfifo.
 */
#define ASRC_CHNL1_INFIFO_RESET    (BIT(0))
#define ASRC_CHNL1_INFIFO_RESET_M  (ASRC_CHNL1_INFIFO_RESET_V << ASRC_CHNL1_INFIFO_RESET_S)
#define ASRC_CHNL1_INFIFO_RESET_V  0x00000001U
#define ASRC_CHNL1_INFIFO_RESET_S  0
/** ASRC_CHNL1_OUTFIFO_RESET : WT; bitpos: [1]; default: 0;
 *  Set this bit to reset infifo.
 */
#define ASRC_CHNL1_OUTFIFO_RESET    (BIT(1))
#define ASRC_CHNL1_OUTFIFO_RESET_M  (ASRC_CHNL1_OUTFIFO_RESET_V << ASRC_CHNL1_OUTFIFO_RESET_S)
#define ASRC_CHNL1_OUTFIFO_RESET_V  0x00000001U
#define ASRC_CHNL1_OUTFIFO_RESET_S  1

/** ASRC_CHNL1_INT_RAW_REG register
 *  Raw interrupt status
 */
#define ASRC_CHNL1_INT_RAW_REG (DR_REG_ASRC_BASE + 0x5c)
/** ASRC_CHNL1_OUTCNT_EOF_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  This interrupt raw bit turns to high level when the counter approach to reg_out_len.
 */
#define ASRC_CHNL1_OUTCNT_EOF_INT_RAW    (BIT(0))
#define ASRC_CHNL1_OUTCNT_EOF_INT_RAW_M  (ASRC_CHNL1_OUTCNT_EOF_INT_RAW_V << ASRC_CHNL1_OUTCNT_EOF_INT_RAW_S)
#define ASRC_CHNL1_OUTCNT_EOF_INT_RAW_V  0x00000001U
#define ASRC_CHNL1_OUTCNT_EOF_INT_RAW_S  0

/** ASRC_CHNL1_INT_ST_REG register
 *  Masked interrupt status
 */
#define ASRC_CHNL1_INT_ST_REG (DR_REG_ASRC_BASE + 0x60)
/** ASRC_CHNL1_OUTCNT_EOF_INT_ST : RO; bitpos: [0]; default: 0;
 *  This is the status bit for reg_out_cnt_eof_int_raw when reg_out_cnt_eof_int_ena is
 *  set to 1.
 */
#define ASRC_CHNL1_OUTCNT_EOF_INT_ST    (BIT(0))
#define ASRC_CHNL1_OUTCNT_EOF_INT_ST_M  (ASRC_CHNL1_OUTCNT_EOF_INT_ST_V << ASRC_CHNL1_OUTCNT_EOF_INT_ST_S)
#define ASRC_CHNL1_OUTCNT_EOF_INT_ST_V  0x00000001U
#define ASRC_CHNL1_OUTCNT_EOF_INT_ST_S  0

/** ASRC_CHNL1_INT_ENA_REG register
 *  Interrupt enable bits
 */
#define ASRC_CHNL1_INT_ENA_REG (DR_REG_ASRC_BASE + 0x64)
/** ASRC_CHNL1_OUTCNT_EOF_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  This is the enable bit for reg_out_cnt_eof_int_st register.
 */
#define ASRC_CHNL1_OUTCNT_EOF_INT_ENA    (BIT(0))
#define ASRC_CHNL1_OUTCNT_EOF_INT_ENA_M  (ASRC_CHNL1_OUTCNT_EOF_INT_ENA_V << ASRC_CHNL1_OUTCNT_EOF_INT_ENA_S)
#define ASRC_CHNL1_OUTCNT_EOF_INT_ENA_V  0x00000001U
#define ASRC_CHNL1_OUTCNT_EOF_INT_ENA_S  0

/** ASRC_CHNL1_INT_CLR_REG register
 *  Interrupt clear bits
 */
#define ASRC_CHNL1_INT_CLR_REG (DR_REG_ASRC_BASE + 0x68)
/** ASRC_CHNL1_OUTCNT_EOF_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Set this bit to clear the reg_out_cnt_eof_int_raw interrupt.
 */
#define ASRC_CHNL1_OUTCNT_EOF_INT_CLR    (BIT(0))
#define ASRC_CHNL1_OUTCNT_EOF_INT_CLR_M  (ASRC_CHNL1_OUTCNT_EOF_INT_CLR_V << ASRC_CHNL1_OUTCNT_EOF_INT_CLR_S)
#define ASRC_CHNL1_OUTCNT_EOF_INT_CLR_V  0x00000001U
#define ASRC_CHNL1_OUTCNT_EOF_INT_CLR_S  0

/** ASRC_CHNL1_OUT_CNT_REG register
 *  Status Registers
 */
#define ASRC_CHNL1_OUT_CNT_REG (DR_REG_ASRC_BASE + 0x6c)
/** ASRC_CHNL1_OUT_CNT : RO; bitpos: [23:0]; default: 0;
 *  Represents the data byte numbers send to the DMA when EOF occurs.
 */
#define ASRC_CHNL1_OUT_CNT    0x00FFFFFFU
#define ASRC_CHNL1_OUT_CNT_M  (ASRC_CHNL1_OUT_CNT_V << ASRC_CHNL1_OUT_CNT_S)
#define ASRC_CHNL1_OUT_CNT_V  0x00FFFFFFU
#define ASRC_CHNL1_OUT_CNT_S  0

/** ASRC_CHNL1_TRACE1_REG register
 *  Debug Register1
 */
#define ASRC_CHNL1_TRACE1_REG (DR_REG_ASRC_BASE + 0x74)
/** ASRC_CHNL1_OUT_INC : RO; bitpos: [23:0]; default: 0;
 *  Represents the data byte numbers send to the DMA
 */
#define ASRC_CHNL1_OUT_INC    0x00FFFFFFU
#define ASRC_CHNL1_OUT_INC_M  (ASRC_CHNL1_OUT_INC_V << ASRC_CHNL1_OUT_INC_S)
#define ASRC_CHNL1_OUT_INC_V  0x00FFFFFFU
#define ASRC_CHNL1_OUT_INC_S  0

/** ASRC_SYS_REG register
 *  Control and configuration
 */
#define ASRC_SYS_REG (DR_REG_ASRC_BASE + 0xf8)
/** ASRC_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Reserved
 */
#define ASRC_CLK_EN    (BIT(0))
#define ASRC_CLK_EN_M  (ASRC_CLK_EN_V << ASRC_CLK_EN_S)
#define ASRC_CLK_EN_V  0x00000001U
#define ASRC_CLK_EN_S  0
/** ASRC_CHNL0_CLK_FO : R/W; bitpos: [1]; default: 0;
 *  Set this bit to make channel0 clock free run.
 */
#define ASRC_CHNL0_CLK_FO    (BIT(1))
#define ASRC_CHNL0_CLK_FO_M  (ASRC_CHNL0_CLK_FO_V << ASRC_CHNL0_CLK_FO_S)
#define ASRC_CHNL0_CLK_FO_V  0x00000001U
#define ASRC_CHNL0_CLK_FO_S  1
/** ASRC_CHNL1_CLK_FO : R/W; bitpos: [2]; default: 0;
 *  Set this bit to make channel1 clock free run.
 */
#define ASRC_CHNL1_CLK_FO    (BIT(2))
#define ASRC_CHNL1_CLK_FO_M  (ASRC_CHNL1_CLK_FO_V << ASRC_CHNL1_CLK_FO_S)
#define ASRC_CHNL1_CLK_FO_V  0x00000001U
#define ASRC_CHNL1_CLK_FO_S  2
/** ASRC_CHNL0_OUTFIFO_CLK_FO : R/W; bitpos: [3]; default: 0;
 *  Set this bit to make channel0 outfifo clock free run.
 */
#define ASRC_CHNL0_OUTFIFO_CLK_FO    (BIT(3))
#define ASRC_CHNL0_OUTFIFO_CLK_FO_M  (ASRC_CHNL0_OUTFIFO_CLK_FO_V << ASRC_CHNL0_OUTFIFO_CLK_FO_S)
#define ASRC_CHNL0_OUTFIFO_CLK_FO_V  0x00000001U
#define ASRC_CHNL0_OUTFIFO_CLK_FO_S  3
/** ASRC_CHNL0_INFIFO_CLK_FO : R/W; bitpos: [4]; default: 0;
 *  Set this bit to make channel0 infifo clock free run.
 */
#define ASRC_CHNL0_INFIFO_CLK_FO    (BIT(4))
#define ASRC_CHNL0_INFIFO_CLK_FO_M  (ASRC_CHNL0_INFIFO_CLK_FO_V << ASRC_CHNL0_INFIFO_CLK_FO_S)
#define ASRC_CHNL0_INFIFO_CLK_FO_V  0x00000001U
#define ASRC_CHNL0_INFIFO_CLK_FO_S  4
/** ASRC_CHNL1_OUTFIFO_CLK_FO : R/W; bitpos: [5]; default: 0;
 *  Set this bit to make channel1 outfifo clock free run.
 */
#define ASRC_CHNL1_OUTFIFO_CLK_FO    (BIT(5))
#define ASRC_CHNL1_OUTFIFO_CLK_FO_M  (ASRC_CHNL1_OUTFIFO_CLK_FO_V << ASRC_CHNL1_OUTFIFO_CLK_FO_S)
#define ASRC_CHNL1_OUTFIFO_CLK_FO_V  0x00000001U
#define ASRC_CHNL1_OUTFIFO_CLK_FO_S  5
/** ASRC_CHNL1_INFIFO_CLK_FO : R/W; bitpos: [6]; default: 0;
 *  Set this bit to make channel1 infifo clock free run.
 */
#define ASRC_CHNL1_INFIFO_CLK_FO    (BIT(6))
#define ASRC_CHNL1_INFIFO_CLK_FO_M  (ASRC_CHNL1_INFIFO_CLK_FO_V << ASRC_CHNL1_INFIFO_CLK_FO_S)
#define ASRC_CHNL1_INFIFO_CLK_FO_V  0x00000001U
#define ASRC_CHNL1_INFIFO_CLK_FO_S  6

/** ASRC_DATE_REG register
 *  Control and configuration registers
 */
#define ASRC_DATE_REG (DR_REG_ASRC_BASE + 0xfc)
/** ASRC_DATE : R/W; bitpos: [27:0]; default: 37777984;
 *  Reserved
 */
#define ASRC_DATE    0x0FFFFFFFU
#define ASRC_DATE_M  (ASRC_DATE_V << ASRC_DATE_S)
#define ASRC_DATE_V  0x0FFFFFFFU
#define ASRC_DATE_S  0

#ifdef __cplusplus
}
#endif
