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

/** CORDIC_CSR_CFG_REG register
 *  Cordic model config register
 */
#define CORDIC_CSR_CFG_REG (DR_REG_CORDIC_BASE + 0x0)
/** CORDIC_FUNC : R/W; bitpos: [3:0]; default: 0;
 *  Used to inicate the type of operation currently being performed
 */
#define CORDIC_FUNC    0x0000000FU
#define CORDIC_FUNC_M  (CORDIC_FUNC_V << CORDIC_FUNC_S)
#define CORDIC_FUNC_V  0x0000000FU
#define CORDIC_FUNC_S  0
/** CORDIC_PRES : R/W; bitpos: [7:4]; default: 0;
 *  Instruct the cordic calculate cycle number
 */
#define CORDIC_PRES    0x0000000FU
#define CORDIC_PRES_M  (CORDIC_PRES_V << CORDIC_PRES_S)
#define CORDIC_PRES_V  0x0000000FU
#define CORDIC_PRES_S  4
/** CORDIC_SCALE : R/W; bitpos: [11:8]; default: 0;
 *  The scaling of Input argument
 */
#define CORDIC_SCALE    0x0000000FU
#define CORDIC_SCALE_M  (CORDIC_SCALE_V << CORDIC_SCALE_S)
#define CORDIC_SCALE_V  0x0000000FU
#define CORDIC_SCALE_S  8
/** CORDIC_WORK_MODE : R/W; bitpos: [18:17]; default: 0;
 *  Select Cordic Module data transfer Mode,2'd0: reg_in,reg_out, no zero mode, 2'd1:
 *  dma_in, reg_out, no zero mode, 2'd2: dma_loop, 2'd3: reg_in, reg_out, zero mode
 */
#define CORDIC_WORK_MODE    0x00000003U
#define CORDIC_WORK_MODE_M  (CORDIC_WORK_MODE_V << CORDIC_WORK_MODE_S)
#define CORDIC_WORK_MODE_V  0x00000003U
#define CORDIC_WORK_MODE_S  17
/** CORDIC_RES_NUM : R/W; bitpos: [19]; default: 0;
 *  Result number
 */
#define CORDIC_RES_NUM    (BIT(19))
#define CORDIC_RES_NUM_M  (CORDIC_RES_NUM_V << CORDIC_RES_NUM_S)
#define CORDIC_RES_NUM_V  0x00000001U
#define CORDIC_RES_NUM_S  19
/** CORDIC_ARG_NUM : R/W; bitpos: [20]; default: 0;
 *  Argument number
 */
#define CORDIC_ARG_NUM    (BIT(20))
#define CORDIC_ARG_NUM_M  (CORDIC_ARG_NUM_V << CORDIC_ARG_NUM_S)
#define CORDIC_ARG_NUM_V  0x00000001U
#define CORDIC_ARG_NUM_S  20
/** CORDIC_RES_SIZE : R/W; bitpos: [21]; default: 0;
 *  Result Size, 0: q1.15, 1: q1.31
 */
#define CORDIC_RES_SIZE    (BIT(21))
#define CORDIC_RES_SIZE_M  (CORDIC_RES_SIZE_V << CORDIC_RES_SIZE_S)
#define CORDIC_RES_SIZE_V  0x00000001U
#define CORDIC_RES_SIZE_S  21
/** CORDIC_ARG_SIZE : R/W; bitpos: [22]; default: 0;
 *  Argument Size, 0: q1.15, 1: q1.31
 */
#define CORDIC_ARG_SIZE    (BIT(22))
#define CORDIC_ARG_SIZE_M  (CORDIC_ARG_SIZE_V << CORDIC_ARG_SIZE_S)
#define CORDIC_ARG_SIZE_V  0x00000001U
#define CORDIC_ARG_SIZE_S  22
/** CORDIC_UPDATE_FLAG : R/W/SC; bitpos: [29]; default: 0;
 *  Used to synchronize register data to the computing core and start calculate
 */
#define CORDIC_UPDATE_FLAG    (BIT(29))
#define CORDIC_UPDATE_FLAG_M  (CORDIC_UPDATE_FLAG_V << CORDIC_UPDATE_FLAG_S)
#define CORDIC_UPDATE_FLAG_V  0x00000001U
#define CORDIC_UPDATE_FLAG_S  29
/** CORDIC_RES_ALL_RDY_FLAG : RO/SS/SC; bitpos: [30]; default: 0;
 *  Used to indicate whether the operation all result has been stored in the result
 *  register
 */
#define CORDIC_RES_ALL_RDY_FLAG    (BIT(30))
#define CORDIC_RES_ALL_RDY_FLAG_M  (CORDIC_RES_ALL_RDY_FLAG_V << CORDIC_RES_ALL_RDY_FLAG_S)
#define CORDIC_RES_ALL_RDY_FLAG_V  0x00000001U
#define CORDIC_RES_ALL_RDY_FLAG_S  30
/** CORDIC_RES_RDY_FLAG : RO/SS/SC; bitpos: [31]; default: 0;
 *  Used to indicate whether the operation result has been stored in the result register
 */
#define CORDIC_RES_RDY_FLAG    (BIT(31))
#define CORDIC_RES_RDY_FLAG_M  (CORDIC_RES_RDY_FLAG_V << CORDIC_RES_RDY_FLAG_S)
#define CORDIC_RES_RDY_FLAG_V  0x00000001U
#define CORDIC_RES_RDY_FLAG_S  31

/** CORDIC_ARG1_REG register
 *  Cordic argument 1 config register
 */
#define CORDIC_ARG1_REG (DR_REG_CORDIC_BASE + 0x4)
/** CORDIC_ARG1_DATA : R/W; bitpos: [31:0]; default: 0;
 *  First argument data for calculate
 */
#define CORDIC_ARG1_DATA    0xFFFFFFFFU
#define CORDIC_ARG1_DATA_M  (CORDIC_ARG1_DATA_V << CORDIC_ARG1_DATA_S)
#define CORDIC_ARG1_DATA_V  0xFFFFFFFFU
#define CORDIC_ARG1_DATA_S  0

/** CORDIC_ARG2_REG register
 *  Cordic argument 2 config register
 */
#define CORDIC_ARG2_REG (DR_REG_CORDIC_BASE + 0x8)
/** CORDIC_ARG2_DATA : R/W; bitpos: [31:0]; default: 0;
 *  Second argument data for calculate
 */
#define CORDIC_ARG2_DATA    0xFFFFFFFFU
#define CORDIC_ARG2_DATA_M  (CORDIC_ARG2_DATA_V << CORDIC_ARG2_DATA_S)
#define CORDIC_ARG2_DATA_V  0xFFFFFFFFU
#define CORDIC_ARG2_DATA_S  0

/** CORDIC_RES1_REG register
 *  Cordic result 1 receive register
 */
#define CORDIC_RES1_REG (DR_REG_CORDIC_BASE + 0xc)
/** CORDIC_RES1_DATA : RO; bitpos: [31:0]; default: 0;
 *  First result data from calculate
 */
#define CORDIC_RES1_DATA    0xFFFFFFFFU
#define CORDIC_RES1_DATA_M  (CORDIC_RES1_DATA_V << CORDIC_RES1_DATA_S)
#define CORDIC_RES1_DATA_V  0xFFFFFFFFU
#define CORDIC_RES1_DATA_S  0

/** CORDIC_RES2_REG register
 *  Cordic result 2 receive register
 */
#define CORDIC_RES2_REG (DR_REG_CORDIC_BASE + 0x10)
/** CORDIC_RES2_DATA : RO; bitpos: [31:0]; default: 0;
 *  Second result data from calculate
 */
#define CORDIC_RES2_DATA    0xFFFFFFFFU
#define CORDIC_RES2_DATA_M  (CORDIC_RES2_DATA_V << CORDIC_RES2_DATA_S)
#define CORDIC_RES2_DATA_V  0xFFFFFFFFU
#define CORDIC_RES2_DATA_S  0

/** CORDIC_IO_INT_ENA_REG register
 *  Cordic interrupt enable signal configuration register.
 */
#define CORDIC_IO_INT_ENA_REG (DR_REG_CORDIC_BASE + 0x28)
/** CORDIC_CAL_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable CAL_DONE_INT.
 */
#define CORDIC_CAL_DONE_INT_ENA    (BIT(0))
#define CORDIC_CAL_DONE_INT_ENA_M  (CORDIC_CAL_DONE_INT_ENA_V << CORDIC_CAL_DONE_INT_ENA_S)
#define CORDIC_CAL_DONE_INT_ENA_V  0x00000001U
#define CORDIC_CAL_DONE_INT_ENA_S  0
/** CORDIC_DMA_ALL_CAL_DONE_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Write 1 to enable DMA_ALL_CAL_DONE_INT.
 */
#define CORDIC_DMA_ALL_CAL_DONE_INT_ENA    (BIT(1))
#define CORDIC_DMA_ALL_CAL_DONE_INT_ENA_M  (CORDIC_DMA_ALL_CAL_DONE_INT_ENA_V << CORDIC_DMA_ALL_CAL_DONE_INT_ENA_S)
#define CORDIC_DMA_ALL_CAL_DONE_INT_ENA_V  0x00000001U
#define CORDIC_DMA_ALL_CAL_DONE_INT_ENA_S  1
/** CORDIC_OUT_AFIFO_WOVF_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Write 1 to enable OUT_AFIFO_WOVF_INT.
 */
#define CORDIC_OUT_AFIFO_WOVF_INT_ENA    (BIT(2))
#define CORDIC_OUT_AFIFO_WOVF_INT_ENA_M  (CORDIC_OUT_AFIFO_WOVF_INT_ENA_V << CORDIC_OUT_AFIFO_WOVF_INT_ENA_S)
#define CORDIC_OUT_AFIFO_WOVF_INT_ENA_V  0x00000001U
#define CORDIC_OUT_AFIFO_WOVF_INT_ENA_S  2
/** CORDIC_IN_AFIFO_RUDF_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Write 1 to enable IN_AFIFO_RUDF_INT.
 */
#define CORDIC_IN_AFIFO_RUDF_INT_ENA    (BIT(3))
#define CORDIC_IN_AFIFO_RUDF_INT_ENA_M  (CORDIC_IN_AFIFO_RUDF_INT_ENA_V << CORDIC_IN_AFIFO_RUDF_INT_ENA_S)
#define CORDIC_IN_AFIFO_RUDF_INT_ENA_V  0x00000001U
#define CORDIC_IN_AFIFO_RUDF_INT_ENA_S  3
/** CORDIC_ARG_NUM_ERR_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Write 1 to enable ARG_NUM_ERR_INT.
 */
#define CORDIC_ARG_NUM_ERR_INT_ENA    (BIT(4))
#define CORDIC_ARG_NUM_ERR_INT_ENA_M  (CORDIC_ARG_NUM_ERR_INT_ENA_V << CORDIC_ARG_NUM_ERR_INT_ENA_S)
#define CORDIC_ARG_NUM_ERR_INT_ENA_V  0x00000001U
#define CORDIC_ARG_NUM_ERR_INT_ENA_S  4

/** CORDIC_IO_INT_RAW_REG register
 *  Cordic interrupt raw signal status register.
 */
#define CORDIC_IO_INT_RAW_REG (DR_REG_CORDIC_BASE + 0x2c)
/** CORDIC_CAL_DONE_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  The raw interrupt status of CAL_DONE_INT.
 */
#define CORDIC_CAL_DONE_INT_RAW    (BIT(0))
#define CORDIC_CAL_DONE_INT_RAW_M  (CORDIC_CAL_DONE_INT_RAW_V << CORDIC_CAL_DONE_INT_RAW_S)
#define CORDIC_CAL_DONE_INT_RAW_V  0x00000001U
#define CORDIC_CAL_DONE_INT_RAW_S  0
/** CORDIC_DMA_ALL_CAL_DONE_INT_RAW : R/SS/WTC; bitpos: [1]; default: 0;
 *  The raw interrupt status of DMA_ALL_CAL_DONE_INT.
 */
#define CORDIC_DMA_ALL_CAL_DONE_INT_RAW    (BIT(1))
#define CORDIC_DMA_ALL_CAL_DONE_INT_RAW_M  (CORDIC_DMA_ALL_CAL_DONE_INT_RAW_V << CORDIC_DMA_ALL_CAL_DONE_INT_RAW_S)
#define CORDIC_DMA_ALL_CAL_DONE_INT_RAW_V  0x00000001U
#define CORDIC_DMA_ALL_CAL_DONE_INT_RAW_S  1
/** CORDIC_OUT_AFIFO_WOVF_INT_RAW : R/SS/WTC; bitpos: [2]; default: 0;
 *  The raw interrupt status of OUT_AFIFO_WOVF_INT.
 */
#define CORDIC_OUT_AFIFO_WOVF_INT_RAW    (BIT(2))
#define CORDIC_OUT_AFIFO_WOVF_INT_RAW_M  (CORDIC_OUT_AFIFO_WOVF_INT_RAW_V << CORDIC_OUT_AFIFO_WOVF_INT_RAW_S)
#define CORDIC_OUT_AFIFO_WOVF_INT_RAW_V  0x00000001U
#define CORDIC_OUT_AFIFO_WOVF_INT_RAW_S  2
/** CORDIC_IN_AFIFO_RUDF_INT_RAW : R/SS/WTC; bitpos: [3]; default: 0;
 *  The raw interrupt status of IN_AFIFO_RUDF_INT.
 */
#define CORDIC_IN_AFIFO_RUDF_INT_RAW    (BIT(3))
#define CORDIC_IN_AFIFO_RUDF_INT_RAW_M  (CORDIC_IN_AFIFO_RUDF_INT_RAW_V << CORDIC_IN_AFIFO_RUDF_INT_RAW_S)
#define CORDIC_IN_AFIFO_RUDF_INT_RAW_V  0x00000001U
#define CORDIC_IN_AFIFO_RUDF_INT_RAW_S  3
/** CORDIC_ARG_NUM_ERR_INT_RAW : R/SS/WTC; bitpos: [4]; default: 0;
 *  The raw interrupt status of ARG_NUM_ERR_INT.
 */
#define CORDIC_ARG_NUM_ERR_INT_RAW    (BIT(4))
#define CORDIC_ARG_NUM_ERR_INT_RAW_M  (CORDIC_ARG_NUM_ERR_INT_RAW_V << CORDIC_ARG_NUM_ERR_INT_RAW_S)
#define CORDIC_ARG_NUM_ERR_INT_RAW_V  0x00000001U
#define CORDIC_ARG_NUM_ERR_INT_RAW_S  4

/** CORDIC_IO_INT_ST_REG register
 *  Cordic interrupt signal status register.
 */
#define CORDIC_IO_INT_ST_REG (DR_REG_CORDIC_BASE + 0x30)
/** CORDIC_CAL_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of CAL_DONE_INT.
 */
#define CORDIC_CAL_DONE_INT_ST    (BIT(0))
#define CORDIC_CAL_DONE_INT_ST_M  (CORDIC_CAL_DONE_INT_ST_V << CORDIC_CAL_DONE_INT_ST_S)
#define CORDIC_CAL_DONE_INT_ST_V  0x00000001U
#define CORDIC_CAL_DONE_INT_ST_S  0
/** CORDIC_DMA_ALL_CAL_DONE_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status of DMA_ALL_CAL_DONE_INT.
 */
#define CORDIC_DMA_ALL_CAL_DONE_INT_ST    (BIT(1))
#define CORDIC_DMA_ALL_CAL_DONE_INT_ST_M  (CORDIC_DMA_ALL_CAL_DONE_INT_ST_V << CORDIC_DMA_ALL_CAL_DONE_INT_ST_S)
#define CORDIC_DMA_ALL_CAL_DONE_INT_ST_V  0x00000001U
#define CORDIC_DMA_ALL_CAL_DONE_INT_ST_S  1
/** CORDIC_OUT_AFIFO_WOVF_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status of OUT_AFIFO_WOVF_INT.
 */
#define CORDIC_OUT_AFIFO_WOVF_INT_ST    (BIT(2))
#define CORDIC_OUT_AFIFO_WOVF_INT_ST_M  (CORDIC_OUT_AFIFO_WOVF_INT_ST_V << CORDIC_OUT_AFIFO_WOVF_INT_ST_S)
#define CORDIC_OUT_AFIFO_WOVF_INT_ST_V  0x00000001U
#define CORDIC_OUT_AFIFO_WOVF_INT_ST_S  2
/** CORDIC_IN_AFIFO_RUDF_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status of IN_AFIFO_RUDF_INT.
 */
#define CORDIC_IN_AFIFO_RUDF_INT_ST    (BIT(3))
#define CORDIC_IN_AFIFO_RUDF_INT_ST_M  (CORDIC_IN_AFIFO_RUDF_INT_ST_V << CORDIC_IN_AFIFO_RUDF_INT_ST_S)
#define CORDIC_IN_AFIFO_RUDF_INT_ST_V  0x00000001U
#define CORDIC_IN_AFIFO_RUDF_INT_ST_S  3
/** CORDIC_ARG_NUM_ERR_INT_ST : RO; bitpos: [4]; default: 0;
 *  The masked interrupt status of ARG_NUM_ERR_INT.
 */
#define CORDIC_ARG_NUM_ERR_INT_ST    (BIT(4))
#define CORDIC_ARG_NUM_ERR_INT_ST_M  (CORDIC_ARG_NUM_ERR_INT_ST_V << CORDIC_ARG_NUM_ERR_INT_ST_S)
#define CORDIC_ARG_NUM_ERR_INT_ST_V  0x00000001U
#define CORDIC_ARG_NUM_ERR_INT_ST_S  4

/** CORDIC_IO_INT_CLR_REG register
 *  Cordic interrupt  clear signal configuration register.
 */
#define CORDIC_IO_INT_CLR_REG (DR_REG_CORDIC_BASE + 0x34)
/** CORDIC_CAL_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear CAL_DONE_INT.
 */
#define CORDIC_CAL_DONE_INT_CLR    (BIT(0))
#define CORDIC_CAL_DONE_INT_CLR_M  (CORDIC_CAL_DONE_INT_CLR_V << CORDIC_CAL_DONE_INT_CLR_S)
#define CORDIC_CAL_DONE_INT_CLR_V  0x00000001U
#define CORDIC_CAL_DONE_INT_CLR_S  0
/** CORDIC_DMA_ALL_CAL_DONE_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Write 1 to clear DMA_ALL_CAL_DONE_INT.
 */
#define CORDIC_DMA_ALL_CAL_DONE_INT_CLR    (BIT(1))
#define CORDIC_DMA_ALL_CAL_DONE_INT_CLR_M  (CORDIC_DMA_ALL_CAL_DONE_INT_CLR_V << CORDIC_DMA_ALL_CAL_DONE_INT_CLR_S)
#define CORDIC_DMA_ALL_CAL_DONE_INT_CLR_V  0x00000001U
#define CORDIC_DMA_ALL_CAL_DONE_INT_CLR_S  1
/** CORDIC_OUT_AFIFO_WOVF_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Write 1 to clear OUT_AFIFO_WOVF_INT.
 */
#define CORDIC_OUT_AFIFO_WOVF_INT_CLR    (BIT(2))
#define CORDIC_OUT_AFIFO_WOVF_INT_CLR_M  (CORDIC_OUT_AFIFO_WOVF_INT_CLR_V << CORDIC_OUT_AFIFO_WOVF_INT_CLR_S)
#define CORDIC_OUT_AFIFO_WOVF_INT_CLR_V  0x00000001U
#define CORDIC_OUT_AFIFO_WOVF_INT_CLR_S  2
/** CORDIC_IN_AFIFO_RUDF_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Write 1 to clear IN_AFIFO_RUDF_INT.
 */
#define CORDIC_IN_AFIFO_RUDF_INT_CLR    (BIT(3))
#define CORDIC_IN_AFIFO_RUDF_INT_CLR_M  (CORDIC_IN_AFIFO_RUDF_INT_CLR_V << CORDIC_IN_AFIFO_RUDF_INT_CLR_S)
#define CORDIC_IN_AFIFO_RUDF_INT_CLR_V  0x00000001U
#define CORDIC_IN_AFIFO_RUDF_INT_CLR_S  3
/** CORDIC_ARG_NUM_ERR_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Write 1 to clear ARG_NUM_ERR_INT.
 */
#define CORDIC_ARG_NUM_ERR_INT_CLR    (BIT(4))
#define CORDIC_ARG_NUM_ERR_INT_CLR_M  (CORDIC_ARG_NUM_ERR_INT_CLR_V << CORDIC_ARG_NUM_ERR_INT_CLR_S)
#define CORDIC_ARG_NUM_ERR_INT_CLR_V  0x00000001U
#define CORDIC_ARG_NUM_ERR_INT_CLR_S  4

/** CORDIC_FIFO_CFG_REG register
 *  Cordic FIFO configuration register.
 */
#define CORDIC_FIFO_CFG_REG (DR_REG_CORDIC_BASE + 0x38)
/** CORDIC_INFIFO_RESET : WT; bitpos: [0]; default: 0;
 *  infifo_reset_register
 */
#define CORDIC_INFIFO_RESET    (BIT(0))
#define CORDIC_INFIFO_RESET_M  (CORDIC_INFIFO_RESET_V << CORDIC_INFIFO_RESET_S)
#define CORDIC_INFIFO_RESET_V  0x00000001U
#define CORDIC_INFIFO_RESET_S  0
/** CORDIC_OUTFIFO_RESET : WT; bitpos: [1]; default: 0;
 *  outfifo_reset_register
 */
#define CORDIC_OUTFIFO_RESET    (BIT(1))
#define CORDIC_OUTFIFO_RESET_M  (CORDIC_OUTFIFO_RESET_V << CORDIC_OUTFIFO_RESET_S)
#define CORDIC_OUTFIFO_RESET_V  0x00000001U
#define CORDIC_OUTFIFO_RESET_S  1

/** CORDIC_CLK_REG register
 *  Cordic clk configuration register.
 */
#define CORDIC_CLK_REG (DR_REG_CORDIC_BASE + 0x3c)
/** CORDIC_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Force clock on for this register file
 */
#define CORDIC_CLK_EN    (BIT(31))
#define CORDIC_CLK_EN_M  (CORDIC_CLK_EN_V << CORDIC_CLK_EN_S)
#define CORDIC_CLK_EN_V  0x00000001U
#define CORDIC_CLK_EN_S  31

/** CORDIC_VERSION_REG register
 *  Version register.
 */
#define CORDIC_VERSION_REG (DR_REG_CORDIC_BASE + 0x40)
/** CORDIC_DATE : R/W; bitpos: [27:0]; default: 37818960;
 *  Version of this register file
 */
#define CORDIC_DATE    0x0FFFFFFFU
#define CORDIC_DATE_M  (CORDIC_DATE_V << CORDIC_DATE_S)
#define CORDIC_DATE_V  0x0FFFFFFFU
#define CORDIC_DATE_S  0

#ifdef __cplusplus
}
#endif
