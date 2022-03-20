/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ECC_MULT_INT_RAW_REG          (DR_REG_ECC_MULT_BASE + 0xC)
/* ECC_MULT_CALC_DONE_INT_RAW : RO/WTC/SS ;bitpos:[0] ;default: 1'b0 ; */
/*description: The raw interrupt status bit  for the i2s_tx_hung_int interrupt.*/
#define ECC_MULT_CALC_DONE_INT_RAW    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_RAW_M  (BIT(0))
#define ECC_MULT_CALC_DONE_INT_RAW_V  0x1
#define ECC_MULT_CALC_DONE_INT_RAW_S  0

#define ECC_MULT_INT_ST_REG          (DR_REG_ECC_MULT_BASE + 0x10)
/* ECC_MULT_CALC_DONE_INT_ST : RO ;bitpos:[0] ;default: 1'b0 ; */
/*description: The masked interrupt status bit  for the i2s_rx_done_int interrupt.*/
#define ECC_MULT_CALC_DONE_INT_ST    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ST_M  (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ST_V  0x1
#define ECC_MULT_CALC_DONE_INT_ST_S  0

#define ECC_MULT_INT_ENA_REG          (DR_REG_ECC_MULT_BASE + 0x14)
/* ECC_MULT_CALC_DONE_INT_ENA : R/W ;bitpos:[0] ;default: 1'b0 ; */
/*description: The interrupt enable bit  for the i2s_rx_done_int interrupt.*/
#define ECC_MULT_CALC_DONE_INT_ENA    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ENA_M  (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ENA_V  0x1
#define ECC_MULT_CALC_DONE_INT_ENA_S  0

#define ECC_MULT_INT_CLR_REG          (DR_REG_ECC_MULT_BASE + 0x18)
/* ECC_MULT_CALC_DONE_INT_CLR : WT ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to clear the i2s_rx_done_int interrupt.*/
#define ECC_MULT_CALC_DONE_INT_CLR    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_CLR_M  (BIT(0))
#define ECC_MULT_CALC_DONE_INT_CLR_V  0x1
#define ECC_MULT_CALC_DONE_INT_CLR_S  0

#define ECC_MULT_CONF_REG          (DR_REG_ECC_MULT_BASE + 0x1C)
/* ECC_MULT_VERIFICATION_RESULT : RO/SS ;bitpos:[8] ;default: 1'b0 ; */
/*description: Reserve.*/
#define ECC_MULT_VERIFICATION_RESULT    (BIT(8))
#define ECC_MULT_VERIFICATION_RESULT_M  (BIT(8))
#define ECC_MULT_VERIFICATION_RESULT_V  0x1
#define ECC_MULT_VERIFICATION_RESULT_S  8
/* ECC_MULT_WORK_MODE : R/W ;bitpos:[7:5] ;default: 3'b0 ; */
/*description:  Reserved.*/
#define ECC_MULT_WORK_MODE    0x00000007
#define ECC_MULT_WORK_MODE_M  ((ECC_MULT_WORK_MODE_V)<<(ECC_MULT_WORK_MODE_S))
#define ECC_MULT_WORK_MODE_V  0x7
#define ECC_MULT_WORK_MODE_S  5
/* ECC_MULT_CLK_EN : R/W ;bitpos:[4] ;default: 1'b0 ; */
/*description: clk gate.*/
#define ECC_MULT_CLK_EN    (BIT(4))
#define ECC_MULT_CLK_EN_M  (BIT(4))
#define ECC_MULT_CLK_EN_V  0x1
#define ECC_MULT_CLK_EN_S  4
/* ECC_MULT_SECURITY_MODE : R/W ;bitpos:[3] ;default: 1'b0 ; */
/*description: Set this bit to enable slave receiver mode.*/
#define ECC_MULT_SECURITY_MODE    (BIT(3))
#define ECC_MULT_SECURITY_MODE_M  (BIT(3))
#define ECC_MULT_SECURITY_MODE_V  0x1
#define ECC_MULT_SECURITY_MODE_S  3
/* ECC_MULT_KEY_LENGTH : R/W ;bitpos:[2] ;default: 1'b0 ; */
/*description: Set this bit to start receiving data.*/
#define ECC_MULT_KEY_LENGTH    (BIT(2))
#define ECC_MULT_KEY_LENGTH_M  (BIT(2))
#define ECC_MULT_KEY_LENGTH_V  0x1
#define ECC_MULT_KEY_LENGTH_S  2
/* ECC_MULT_RESET : WT ;bitpos:[1] ;default: 1'b0 ; */
/*description: Set this bit to reset Rx AFIFO.*/
#define ECC_MULT_RESET    (BIT(1))
#define ECC_MULT_RESET_M  (BIT(1))
#define ECC_MULT_RESET_V  0x1
#define ECC_MULT_RESET_S  1
/* ECC_MULT_START : R/W/SC ;bitpos:[0] ;default: 1'b0 ; */
/*description: Set this bit to reset receiver.*/
#define ECC_MULT_START    (BIT(0))
#define ECC_MULT_START_M  (BIT(0))
#define ECC_MULT_START_V  0x1
#define ECC_MULT_START_S  0

#define ECC_MULT_DATE_REG          (DR_REG_ECC_MULT_BASE + 0xFC)
/* ECC_MULT_DATE : R/W ;bitpos:[27:0] ;default: 28'h2012230 ; */
/*description: ECC mult version control register.*/
#define ECC_MULT_DATE    0x0FFFFFFF
#define ECC_MULT_DATE_M  ((ECC_MULT_DATE_V)<<(ECC_MULT_DATE_S))
#define ECC_MULT_DATE_V  0xFFFFFFF
#define ECC_MULT_DATE_S  0

#define ECC_MULT_K_1_REG          (DR_REG_ECC_MULT_BASE + 0x0100)
/* ECC_MULT_MEM_K_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_1  0xFFFFFFFF
#define ECC_MULT_MEM_K_1_M  ((ECC_MULT_MEM_K_1_V)<<(ECC_MULT_MEM_K_1_S))
#define ECC_MULT_MEM_K_1_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_1_S  0

#define ECC_MULT_K_2_REG          (DR_REG_ECC_MULT_BASE + 0x0104)
/* ECC_MULT_MEM_K_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_2  0xFFFFFFFF
#define ECC_MULT_MEM_K_2_M  ((ECC_MULT_MEM_K_2_V)<<(ECC_MULT_MEM_K_2_S))
#define ECC_MULT_MEM_K_2_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_2_S  0

#define ECC_MULT_K_3_REG          (DR_REG_ECC_MULT_BASE + 0x0108)
/* ECC_MULT_MEM_K_3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_3  0xFFFFFFFF
#define ECC_MULT_MEM_K_3_M  ((ECC_MULT_MEM_K_3_V)<<(ECC_MULT_MEM_K_3_S))
#define ECC_MULT_MEM_K_3_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_3_S  0

#define ECC_MULT_K_4_REG          (DR_REG_ECC_MULT_BASE + 0x010c)
/* ECC_MULT_MEM_K_4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_4  0xFFFFFFFF
#define ECC_MULT_MEM_K_4_M  ((ECC_MULT_MEM_K_4_V)<<(ECC_MULT_MEM_K_4_S))
#define ECC_MULT_MEM_K_4_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_4_S  0

#define ECC_MULT_K_5_REG          (DR_REG_ECC_MULT_BASE + 0x0110)
/* ECC_MULT_MEM_K_5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_5  0xFFFFFFFF
#define ECC_MULT_MEM_K_5_M  ((ECC_MULT_MEM_K_5_V)<<(ECC_MULT_MEM_K_5_S))
#define ECC_MULT_MEM_K_5_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_5_S  0

#define ECC_MULT_K_6_REG          (DR_REG_ECC_MULT_BASE + 0x0114)
/* ECC_MULT_MEM_K_6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_6  0xFFFFFFFF
#define ECC_MULT_MEM_K_6_M  ((ECC_MULT_MEM_K_6_V)<<(ECC_MULT_MEM_K_6_S))
#define ECC_MULT_MEM_K_6_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_6_S  0

#define ECC_MULT_K_7_REG          (DR_REG_ECC_MULT_BASE + 0x0118)
/* ECC_MULT_MEM_K_7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_7  0xFFFFFFFF
#define ECC_MULT_MEM_K_7_M  ((ECC_MULT_MEM_K_7_V)<<(ECC_MULT_MEM_K_7_S))
#define ECC_MULT_MEM_K_7_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_7_S  0

#define ECC_MULT_K_8_REG          (DR_REG_ECC_MULT_BASE + 0x011c)
/* ECC_MULT_MEM_K_8 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter k.*/
#define ECC_MULT_MEM_K_8  0xFFFFFFFF
#define ECC_MULT_MEM_K_8_M  ((ECC_MULT_MEM_K_8_V)<<(ECC_MULT_MEM_K_8_S))
#define ECC_MULT_MEM_K_8_V  0xFFFFFFFF
#define ECC_MULT_MEM_K_8_S  0

#define ECC_MULT_PX_1_REG          (DR_REG_ECC_MULT_BASE + 0x0120)
/* ECC_MULT_MEM_PX_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_1  0xFFFFFFFF
#define ECC_MULT_MEM_PX_1_M  ((ECC_MULT_MEM_PX_1_V)<<(ECC_MULT_MEM_PX_1_S))
#define ECC_MULT_MEM_PX_1_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_1_S  0

#define ECC_MULT_PX_2_REG          (DR_REG_ECC_MULT_BASE + 0x0124)
/* ECC_MULT_MEM_PX_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_2  0xFFFFFFFF
#define ECC_MULT_MEM_PX_2_M  ((ECC_MULT_MEM_PX_2_V)<<(ECC_MULT_MEM_PX_2_S))
#define ECC_MULT_MEM_PX_2_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_2_S  0

#define ECC_MULT_PX_3_REG          (DR_REG_ECC_MULT_BASE + 0x0128)
/* ECC_MULT_MEM_PX_3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_3  0xFFFFFFFF
#define ECC_MULT_MEM_PX_3_M  ((ECC_MULT_MEM_PX_3_V)<<(ECC_MULT_MEM_PX_3_S))
#define ECC_MULT_MEM_PX_3_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_3_S  0

#define ECC_MULT_PX_4_REG          (DR_REG_ECC_MULT_BASE + 0x012c)
/* ECC_MULT_MEM_PX_4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_4  0xFFFFFFFF
#define ECC_MULT_MEM_PX_4_M  ((ECC_MULT_MEM_PX_4_V)<<(ECC_MULT_MEM_PX_4_S))
#define ECC_MULT_MEM_PX_4_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_4_S  0

#define ECC_MULT_PX_5_REG          (DR_REG_ECC_MULT_BASE + 0x0130)
/* ECC_MULT_MEM_PX_5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_5  0xFFFFFFFF
#define ECC_MULT_MEM_PX_5_M  ((ECC_MULT_MEM_PX_5_V)<<(ECC_MULT_MEM_PX_5_S))
#define ECC_MULT_MEM_PX_5_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_5_S  0

#define ECC_MULT_PX_6_REG          (DR_REG_ECC_MULT_BASE + 0x0134)
/* ECC_MULT_MEM_PX_6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_6  0xFFFFFFFF
#define ECC_MULT_MEM_PX_6_M  ((ECC_MULT_MEM_PX_6_V)<<(ECC_MULT_MEM_PX_6_S))
#define ECC_MULT_MEM_PX_6_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_6_S  0

#define ECC_MULT_PX_7_REG          (DR_REG_ECC_MULT_BASE + 0x0138)
/* ECC_MULT_MEM_PX_7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_7  0xFFFFFFFF
#define ECC_MULT_MEM_PX_7_M  ((ECC_MULT_MEM_PX_7_V)<<(ECC_MULT_MEM_PX_7_S))
#define ECC_MULT_MEM_PX_7_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_7_S  0

#define ECC_MULT_PX_8_REG          (DR_REG_ECC_MULT_BASE + 0x013c)
/* ECC_MULT_MEM_PX_8 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Px.*/
#define ECC_MULT_MEM_PX_8  0xFFFFFFFF
#define ECC_MULT_MEM_PX_8_M  ((ECC_MULT_MEM_PX_8_V)<<(ECC_MULT_MEM_PX_8_S))
#define ECC_MULT_MEM_PX_8_V  0xFFFFFFFF
#define ECC_MULT_MEM_PX_8_S  0

#define ECC_MULT_PY_1_REG          (DR_REG_ECC_MULT_BASE + 0x0140)
/* ECC_MULT_MEM_PY_1 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_1  0xFFFFFFFF
#define ECC_MULT_MEM_PY_1_M  ((ECC_MULT_MEM_PY_1_V)<<(ECC_MULT_MEM_PY_1_S))
#define ECC_MULT_MEM_PY_1_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_1_S  0

#define ECC_MULT_PY_2_REG          (DR_REG_ECC_MULT_BASE + 0x0144)
/* ECC_MULT_MEM_PY_2 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_2  0xFFFFFFFF
#define ECC_MULT_MEM_PY_2_M  ((ECC_MULT_MEM_PY_2_V)<<(ECC_MULT_MEM_PY_2_S))
#define ECC_MULT_MEM_PY_2_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_2_S  0

#define ECC_MULT_PY_3_REG          (DR_REG_ECC_MULT_BASE + 0x0148)
/* ECC_MULT_MEM_PY_3 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_3  0xFFFFFFFF
#define ECC_MULT_MEM_PY_3_M  ((ECC_MULT_MEM_PY_3_V)<<(ECC_MULT_MEM_PY_3_S))
#define ECC_MULT_MEM_PY_3_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_3_S  0

#define ECC_MULT_PY_4_REG          (DR_REG_ECC_MULT_BASE + 0x014c)
/* ECC_MULT_MEM_PY_4 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_4  0xFFFFFFFF
#define ECC_MULT_MEM_PY_4_M  ((ECC_MULT_MEM_PY_4_V)<<(ECC_MULT_MEM_PY_4_S))
#define ECC_MULT_MEM_PY_4_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_4_S  0

#define ECC_MULT_PY_5_REG          (DR_REG_ECC_MULT_BASE + 0x0150)
/* ECC_MULT_MEM_PY_5 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_5  0xFFFFFFFF
#define ECC_MULT_MEM_PY_5_M  ((ECC_MULT_MEM_PY_5_V)<<(ECC_MULT_MEM_PY_5_S))
#define ECC_MULT_MEM_PY_5_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_5_S  0

#define ECC_MULT_PY_6_REG          (DR_REG_ECC_MULT_BASE + 0x0154)
/* ECC_MULT_MEM_PY_6 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_6  0xFFFFFFFF
#define ECC_MULT_MEM_PY_6_M  ((ECC_MULT_MEM_PY_6_V)<<(ECC_MULT_MEM_PY_6_S))
#define ECC_MULT_MEM_PY_6_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_6_S  0

#define ECC_MULT_PY_7_REG          (DR_REG_ECC_MULT_BASE + 0x0158)
/* ECC_MULT_MEM_PY_7 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_7  0xFFFFFFFF
#define ECC_MULT_MEM_PY_7_M  ((ECC_MULT_MEM_PY_7_V)<<(ECC_MULT_MEM_PY_7_S))
#define ECC_MULT_MEM_PY_7_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_7_S  0

#define ECC_MULT_PY_8_REG          (DR_REG_ECC_MULT_BASE + 0x015c)
/* ECC_MULT_MEM_PY_8 : R/W ;bitpos:[31:0] ;default: 32'h0 ; */
/*description: ECC Mem Parameter Py.*/
#define ECC_MULT_MEM_PY_8  0xFFFFFFFF
#define ECC_MULT_MEM_PY_8_M  ((ECC_MULT_MEM_PY_8_V)<<(ECC_MULT_MEM_PY_8_S))
#define ECC_MULT_MEM_PY_8_V  0xFFFFFFFF
#define ECC_MULT_MEM_PY_8_S  0

#ifdef __cplusplus
}
#endif
