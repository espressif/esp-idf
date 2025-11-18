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

/** AXI_IC_DATE_REG register
 *  Version Control Register
 */
#define AXI_IC_DATE_REG (DR_REG_AXI_IC_BASE + 0x0)
/** AXI_IC_REG_DATE : R/W; bitpos: [31:0]; default: 23050900;
 *  Version Control Register
 */
#define AXI_IC_REG_DATE    0xFFFFFFFFU
#define AXI_IC_REG_DATE_M  (AXI_IC_REG_DATE_V << AXI_IC_REG_DATE_S)
#define AXI_IC_REG_DATE_V  0xFFFFFFFFU
#define AXI_IC_REG_DATE_S  0

/** AXI_IC_CLK_EN_REG register
 *  AXI IC reg clk enable config register
 */
#define AXI_IC_CLK_EN_REG (DR_REG_AXI_IC_BASE + 0x4)
/** AXI_IC_REG_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  value 1 enable clk, value 0 disable clk, default 1
 */
#define AXI_IC_REG_CLK_EN    (BIT(0))
#define AXI_IC_REG_CLK_EN_M  (AXI_IC_REG_CLK_EN_V << AXI_IC_REG_CLK_EN_S)
#define AXI_IC_REG_CLK_EN_V  0x00000001U
#define AXI_IC_REG_CLK_EN_S  0

/** AXI_IC_INTERRUPT_RAW_REG register
 *  Interrupt raw of AXI IC  Interrupt register.
 */
#define AXI_IC_INTERRUPT_RAW_REG (DR_REG_AXI_IC_BASE + 0x8)
/** AXI_IC_REG_DEC_FAILURE_INT_RAW : R/SS/WTC; bitpos: [0]; default: 0;
 *  Interrupt raw of reg_dec_failure_int_raw
 */
#define AXI_IC_REG_DEC_FAILURE_INT_RAW    (BIT(0))
#define AXI_IC_REG_DEC_FAILURE_INT_RAW_M  (AXI_IC_REG_DEC_FAILURE_INT_RAW_V << AXI_IC_REG_DEC_FAILURE_INT_RAW_S)
#define AXI_IC_REG_DEC_FAILURE_INT_RAW_V  0x00000001U
#define AXI_IC_REG_DEC_FAILURE_INT_RAW_S  0
/** AXI_IC_REG_TIMEOUT_INT_RAW : R/SS/WTC; bitpos: [1]; default: 0;
 *  Interrupt raw of reg_timeout_int_raw
 */
#define AXI_IC_REG_TIMEOUT_INT_RAW    (BIT(1))
#define AXI_IC_REG_TIMEOUT_INT_RAW_M  (AXI_IC_REG_TIMEOUT_INT_RAW_V << AXI_IC_REG_TIMEOUT_INT_RAW_S)
#define AXI_IC_REG_TIMEOUT_INT_RAW_V  0x00000001U
#define AXI_IC_REG_TIMEOUT_INT_RAW_S  1

/** AXI_IC_INTERRUPT_CLR_REG register
 *  Interrupt clear of AXI IC  Interrupt register.
 */
#define AXI_IC_INTERRUPT_CLR_REG (DR_REG_AXI_IC_BASE + 0xc)
/** AXI_IC_REG_DEC_FAILURE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Interrupt clr of reg_dec_failure_int_clr
 */
#define AXI_IC_REG_DEC_FAILURE_INT_CLR    (BIT(0))
#define AXI_IC_REG_DEC_FAILURE_INT_CLR_M  (AXI_IC_REG_DEC_FAILURE_INT_CLR_V << AXI_IC_REG_DEC_FAILURE_INT_CLR_S)
#define AXI_IC_REG_DEC_FAILURE_INT_CLR_V  0x00000001U
#define AXI_IC_REG_DEC_FAILURE_INT_CLR_S  0
/** AXI_IC_REG_TIMEOUT_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Interrupt clr of reg_timeout_int_clr
 */
#define AXI_IC_REG_TIMEOUT_INT_CLR    (BIT(1))
#define AXI_IC_REG_TIMEOUT_INT_CLR_M  (AXI_IC_REG_TIMEOUT_INT_CLR_V << AXI_IC_REG_TIMEOUT_INT_CLR_S)
#define AXI_IC_REG_TIMEOUT_INT_CLR_V  0x00000001U
#define AXI_IC_REG_TIMEOUT_INT_CLR_S  1

/** AXI_IC_INTERRUPT_ENA_REG register
 *  Interrupt enable of AXI IC  Interrupt register.
 */
#define AXI_IC_INTERRUPT_ENA_REG (DR_REG_AXI_IC_BASE + 0x10)
/** AXI_IC_REG_DEC_FAILURE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Interrupt ena of reg_dec_failure_int_ena
 */
#define AXI_IC_REG_DEC_FAILURE_INT_ENA    (BIT(0))
#define AXI_IC_REG_DEC_FAILURE_INT_ENA_M  (AXI_IC_REG_DEC_FAILURE_INT_ENA_V << AXI_IC_REG_DEC_FAILURE_INT_ENA_S)
#define AXI_IC_REG_DEC_FAILURE_INT_ENA_V  0x00000001U
#define AXI_IC_REG_DEC_FAILURE_INT_ENA_S  0
/** AXI_IC_REG_TIMEOUT_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Interrupt ena of reg_timeout_int_ena
 */
#define AXI_IC_REG_TIMEOUT_INT_ENA    (BIT(1))
#define AXI_IC_REG_TIMEOUT_INT_ENA_M  (AXI_IC_REG_TIMEOUT_INT_ENA_V << AXI_IC_REG_TIMEOUT_INT_ENA_S)
#define AXI_IC_REG_TIMEOUT_INT_ENA_V  0x00000001U
#define AXI_IC_REG_TIMEOUT_INT_ENA_S  1

/** AXI_IC_INTERRUPT_ST_REG register
 *  AXI IC  Interrupt register.
 */
#define AXI_IC_INTERRUPT_ST_REG (DR_REG_AXI_IC_BASE + 0x14)
/** AXI_IC_REG_DEC_FAILURE_INT_ST : RO; bitpos: [0]; default: 0;
 *  Interrupt st of reg_dec_failure_int_st
 */
#define AXI_IC_REG_DEC_FAILURE_INT_ST    (BIT(0))
#define AXI_IC_REG_DEC_FAILURE_INT_ST_M  (AXI_IC_REG_DEC_FAILURE_INT_ST_V << AXI_IC_REG_DEC_FAILURE_INT_ST_S)
#define AXI_IC_REG_DEC_FAILURE_INT_ST_V  0x00000001U
#define AXI_IC_REG_DEC_FAILURE_INT_ST_S  0
/** AXI_IC_REG_TIMEOUT_INT_ST : RO; bitpos: [1]; default: 0;
 *  Interrupt st of reg_timeout_int_st
 */
#define AXI_IC_REG_TIMEOUT_INT_ST    (BIT(1))
#define AXI_IC_REG_TIMEOUT_INT_ST_M  (AXI_IC_REG_TIMEOUT_INT_ST_V << AXI_IC_REG_TIMEOUT_INT_ST_S)
#define AXI_IC_REG_TIMEOUT_INT_ST_V  0x00000001U
#define AXI_IC_REG_TIMEOUT_INT_ST_S  1

/** AXI_IC_TIMEOUT_MAX_CNT_REG register
 *  AXI IC core AW or AR channel timeout counter maximum value.
 */
#define AXI_IC_TIMEOUT_MAX_CNT_REG (DR_REG_AXI_IC_BASE + 0x18)
/** AXI_IC_REG_TIMEOUT_MAX_CNT : R/W; bitpos: [31:0]; default: 4294967295;
 *  AXI IC core AW or AR channel timeout counter maximum value.
 */
#define AXI_IC_REG_TIMEOUT_MAX_CNT    0xFFFFFFFFU
#define AXI_IC_REG_TIMEOUT_MAX_CNT_M  (AXI_IC_REG_TIMEOUT_MAX_CNT_V << AXI_IC_REG_TIMEOUT_MAX_CNT_S)
#define AXI_IC_REG_TIMEOUT_MAX_CNT_V  0xFFFFFFFFU
#define AXI_IC_REG_TIMEOUT_MAX_CNT_S  0

/** AXI_IC_TIMEOUT_INFO_REG register
 *  AXI IC core AW or AR channel timeout information.
 */
#define AXI_IC_TIMEOUT_INFO_REG (DR_REG_AXI_IC_BASE + 0x1c)
/** AXI_IC_REG_TIMEOUT_SLAVE : RO; bitpos: [2:0]; default: 0;
 *  bit[i] is 1 means slave[i] occurs timeout.
 */
#define AXI_IC_REG_TIMEOUT_SLAVE    0x00000007U
#define AXI_IC_REG_TIMEOUT_SLAVE_M  (AXI_IC_REG_TIMEOUT_SLAVE_V << AXI_IC_REG_TIMEOUT_SLAVE_S)
#define AXI_IC_REG_TIMEOUT_SLAVE_V  0x00000007U
#define AXI_IC_REG_TIMEOUT_SLAVE_S  0
/** AXI_IC_REG_TIMEOUT_ID : RO; bitpos: [9:3]; default: 0;
 *  (master id, awid)
 */
#define AXI_IC_REG_TIMEOUT_ID    0x0000007FU
#define AXI_IC_REG_TIMEOUT_ID_M  (AXI_IC_REG_TIMEOUT_ID_V << AXI_IC_REG_TIMEOUT_ID_S)
#define AXI_IC_REG_TIMEOUT_ID_V  0x0000007FU
#define AXI_IC_REG_TIMEOUT_ID_S  3
/** AXI_IC_REG_TIMEOUT_WR : RO; bitpos: [10]; default: 0;
 *  1 indicates write operation, 0 indicates read operation.
 */
#define AXI_IC_REG_TIMEOUT_WR    (BIT(10))
#define AXI_IC_REG_TIMEOUT_WR_M  (AXI_IC_REG_TIMEOUT_WR_V << AXI_IC_REG_TIMEOUT_WR_S)
#define AXI_IC_REG_TIMEOUT_WR_V  0x00000001U
#define AXI_IC_REG_TIMEOUT_WR_S  10

/** AXI_IC_DEC_FAILURE_INFO_REG register
 *  Default slave information get control and status.
 */
#define AXI_IC_DEC_FAILURE_INFO_REG (DR_REG_AXI_IC_BASE + 0x20)
/** AXI_IC_REG_DEC_FAILURE_READ_FIRST : R/W; bitpos: [0]; default: 0;
 *  Decoder failure recorded control. 1 means record read failure first, else write
 *  failure first.
 */
#define AXI_IC_REG_DEC_FAILURE_READ_FIRST    (BIT(0))
#define AXI_IC_REG_DEC_FAILURE_READ_FIRST_M  (AXI_IC_REG_DEC_FAILURE_READ_FIRST_V << AXI_IC_REG_DEC_FAILURE_READ_FIRST_S)
#define AXI_IC_REG_DEC_FAILURE_READ_FIRST_V  0x00000001U
#define AXI_IC_REG_DEC_FAILURE_READ_FIRST_S  0
/** AXI_IC_REG_DEC_FAILURE_VALID : RO; bitpos: [1]; default: 0;
 *  Decoder failure valid control.
 */
#define AXI_IC_REG_DEC_FAILURE_VALID    (BIT(1))
#define AXI_IC_REG_DEC_FAILURE_VALID_M  (AXI_IC_REG_DEC_FAILURE_VALID_V << AXI_IC_REG_DEC_FAILURE_VALID_S)
#define AXI_IC_REG_DEC_FAILURE_VALID_V  0x00000001U
#define AXI_IC_REG_DEC_FAILURE_VALID_S  1
/** AXI_IC_REG_DEC_FAILURE_TYPE : RO; bitpos: [2]; default: 0;
 *  Decoder failure type control. 0 means ar failure, 1 means aw failure.
 */
#define AXI_IC_REG_DEC_FAILURE_TYPE    (BIT(2))
#define AXI_IC_REG_DEC_FAILURE_TYPE_M  (AXI_IC_REG_DEC_FAILURE_TYPE_V << AXI_IC_REG_DEC_FAILURE_TYPE_S)
#define AXI_IC_REG_DEC_FAILURE_TYPE_V  0x00000001U
#define AXI_IC_REG_DEC_FAILURE_TYPE_S  2
/** AXI_IC_REG_DEC_FAILURE_CAUSE : RO; bitpos: [5:3]; default: 0;
 *  bit[0] is 0 means to access the mapped slave address, and is 1 means to access the
 *  unmapped slave address. bit[1] is 0 means to access the visible slave, and is 1
 *  means to access the invisible slave. bit[2] is 0 means to access the secure slave,
 *  and is 1 means to access the non-secure slave.
 */
#define AXI_IC_REG_DEC_FAILURE_CAUSE    0x00000007U
#define AXI_IC_REG_DEC_FAILURE_CAUSE_M  (AXI_IC_REG_DEC_FAILURE_CAUSE_V << AXI_IC_REG_DEC_FAILURE_CAUSE_S)
#define AXI_IC_REG_DEC_FAILURE_CAUSE_V  0x00000007U
#define AXI_IC_REG_DEC_FAILURE_CAUSE_S  3
/** AXI_IC_REG_DEC_FAILURE_MST_ID : RO; bitpos: [12:6]; default: 0;
 *  (master id, awid/arid)
 */
#define AXI_IC_REG_DEC_FAILURE_MST_ID    0x0000007FU
#define AXI_IC_REG_DEC_FAILURE_MST_ID_M  (AXI_IC_REG_DEC_FAILURE_MST_ID_V << AXI_IC_REG_DEC_FAILURE_MST_ID_S)
#define AXI_IC_REG_DEC_FAILURE_MST_ID_V  0x0000007FU
#define AXI_IC_REG_DEC_FAILURE_MST_ID_S  6

/** AXI_IC_QOS_DEBUG_MST_SEL_REG register
 *  Config the select for debug master.
 */
#define AXI_IC_QOS_DEBUG_MST_SEL_REG (DR_REG_AXI_IC_BASE + 0x24)
/** AXI_IC_REG_QOS_DEBUG_MST_SEL : R/W; bitpos: [3:0]; default: 0;
 *  select which master will be debug.
 */
#define AXI_IC_REG_QOS_DEBUG_MST_SEL    0x0000000FU
#define AXI_IC_REG_QOS_DEBUG_MST_SEL_M  (AXI_IC_REG_QOS_DEBUG_MST_SEL_V << AXI_IC_REG_QOS_DEBUG_MST_SEL_S)
#define AXI_IC_REG_QOS_DEBUG_MST_SEL_V  0x0000000FU
#define AXI_IC_REG_QOS_DEBUG_MST_SEL_S  0

/** AXI_IC_DEC_FAILURE_ADDR_REG register
 *  Record the address for AXI Master access Default slave.
 */
#define AXI_IC_DEC_FAILURE_ADDR_REG (DR_REG_AXI_IC_BASE + 0x80)
/** AXI_IC_REG_DEC_FAILURE_ADDR : RO; bitpos: [31:0]; default: 0;
 *  AXI IC core AW or AR channel decoder failure address.
 */
#define AXI_IC_REG_DEC_FAILURE_ADDR    0xFFFFFFFFU
#define AXI_IC_REG_DEC_FAILURE_ADDR_M  (AXI_IC_REG_DEC_FAILURE_ADDR_V << AXI_IC_REG_DEC_FAILURE_ADDR_S)
#define AXI_IC_REG_DEC_FAILURE_ADDR_V  0xFFFFFFFFU
#define AXI_IC_REG_DEC_FAILURE_ADDR_S  0

/** AXI_IC_REG_SLAVE_IS_SECURE_REG register
 *  AXI IC core slave security control.
 */
#define AXI_IC_REG_SLAVE_IS_SECURE_REG (DR_REG_AXI_IC_BASE + 0x84)
/** AXI_IC_REG_SLAVE_IS_SECURE_0 : R/W; bitpos: [0]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_SLAVE_IS_SECURE_0    (BIT(0))
#define AXI_IC_REG_SLAVE_IS_SECURE_0_M  (AXI_IC_REG_SLAVE_IS_SECURE_0_V << AXI_IC_REG_SLAVE_IS_SECURE_0_S)
#define AXI_IC_REG_SLAVE_IS_SECURE_0_V  0x00000001U
#define AXI_IC_REG_SLAVE_IS_SECURE_0_S  0
/** AXI_IC_REG_SLAVE_IS_SECURE_1 : R/W; bitpos: [1]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_SLAVE_IS_SECURE_1    (BIT(1))
#define AXI_IC_REG_SLAVE_IS_SECURE_1_M  (AXI_IC_REG_SLAVE_IS_SECURE_1_V << AXI_IC_REG_SLAVE_IS_SECURE_1_S)
#define AXI_IC_REG_SLAVE_IS_SECURE_1_V  0x00000001U
#define AXI_IC_REG_SLAVE_IS_SECURE_1_S  1
/** AXI_IC_REG_SLAVE_IS_SECURE_2 : R/W; bitpos: [2]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_SLAVE_IS_SECURE_2    (BIT(2))
#define AXI_IC_REG_SLAVE_IS_SECURE_2_M  (AXI_IC_REG_SLAVE_IS_SECURE_2_V << AXI_IC_REG_SLAVE_IS_SECURE_2_S)
#define AXI_IC_REG_SLAVE_IS_SECURE_2_V  0x00000001U
#define AXI_IC_REG_SLAVE_IS_SECURE_2_S  2

/** AXI_IC_SLAVE_SECURITY_OVERRIDE_REG register
 *  The control signal whether slave security is overridden by software.
 */
#define AXI_IC_SLAVE_SECURITY_OVERRIDE_REG (DR_REG_AXI_IC_BASE + 0x88)
/** AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_0 : R/W; bitpos: [0]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_0    (BIT(0))
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_0_M  (AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_0_V << AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_0_S)
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_0_V  0x00000001U
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_0_S  0
/** AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_1 : R/W; bitpos: [1]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_1    (BIT(1))
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_1_M  (AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_1_V << AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_1_S)
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_1_V  0x00000001U
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_1_S  1
/** AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_2 : R/W; bitpos: [2]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_2    (BIT(2))
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_2_M  (AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_2_V << AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_2_S)
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_2_V  0x00000001U
#define AXI_IC_REG_SLAVE_SECURITY_OVERRIDE_2_S  2

/** AXI_IC_MASTER_IS_SECURE_REG register
 *  AXI IC core master security control.
 */
#define AXI_IC_MASTER_IS_SECURE_REG (DR_REG_AXI_IC_BASE + 0x8c)
/** AXI_IC_REG_MASTER_IS_SECURE_0 : R/W; bitpos: [0]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_MASTER_IS_SECURE_0    (BIT(0))
#define AXI_IC_REG_MASTER_IS_SECURE_0_M  (AXI_IC_REG_MASTER_IS_SECURE_0_V << AXI_IC_REG_MASTER_IS_SECURE_0_S)
#define AXI_IC_REG_MASTER_IS_SECURE_0_V  0x00000001U
#define AXI_IC_REG_MASTER_IS_SECURE_0_S  0
/** AXI_IC_REG_MASTER_IS_SECURE_1 : R/W; bitpos: [1]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_MASTER_IS_SECURE_1    (BIT(1))
#define AXI_IC_REG_MASTER_IS_SECURE_1_M  (AXI_IC_REG_MASTER_IS_SECURE_1_V << AXI_IC_REG_MASTER_IS_SECURE_1_S)
#define AXI_IC_REG_MASTER_IS_SECURE_1_V  0x00000001U
#define AXI_IC_REG_MASTER_IS_SECURE_1_S  1
/** AXI_IC_REG_MASTER_IS_SECURE_2 : R/W; bitpos: [2]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_MASTER_IS_SECURE_2    (BIT(2))
#define AXI_IC_REG_MASTER_IS_SECURE_2_M  (AXI_IC_REG_MASTER_IS_SECURE_2_V << AXI_IC_REG_MASTER_IS_SECURE_2_S)
#define AXI_IC_REG_MASTER_IS_SECURE_2_V  0x00000001U
#define AXI_IC_REG_MASTER_IS_SECURE_2_S  2
/** AXI_IC_REG_MASTER_IS_SECURE_3 : R/W; bitpos: [3]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_MASTER_IS_SECURE_3    (BIT(3))
#define AXI_IC_REG_MASTER_IS_SECURE_3_M  (AXI_IC_REG_MASTER_IS_SECURE_3_V << AXI_IC_REG_MASTER_IS_SECURE_3_S)
#define AXI_IC_REG_MASTER_IS_SECURE_3_V  0x00000001U
#define AXI_IC_REG_MASTER_IS_SECURE_3_S  3
/** AXI_IC_REG_MASTER_IS_SECURE_4 : R/W; bitpos: [4]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_MASTER_IS_SECURE_4    (BIT(4))
#define AXI_IC_REG_MASTER_IS_SECURE_4_M  (AXI_IC_REG_MASTER_IS_SECURE_4_V << AXI_IC_REG_MASTER_IS_SECURE_4_S)
#define AXI_IC_REG_MASTER_IS_SECURE_4_V  0x00000001U
#define AXI_IC_REG_MASTER_IS_SECURE_4_S  4
/** AXI_IC_REG_MASTER_IS_SECURE_5 : R/W; bitpos: [5]; default: 0;
 *  1 means safe and 0 means not safe.
 */
#define AXI_IC_REG_MASTER_IS_SECURE_5    (BIT(5))
#define AXI_IC_REG_MASTER_IS_SECURE_5_M  (AXI_IC_REG_MASTER_IS_SECURE_5_V << AXI_IC_REG_MASTER_IS_SECURE_5_S)
#define AXI_IC_REG_MASTER_IS_SECURE_5_V  0x00000001U
#define AXI_IC_REG_MASTER_IS_SECURE_5_S  5

/** AXI_IC_MASTER_SECURITY_OVERRIDE_REG register
 *  The control signal whether master security is overridden by software.
 */
#define AXI_IC_MASTER_SECURITY_OVERRIDE_REG (DR_REG_AXI_IC_BASE + 0x90)
/** AXI_IC_REG_MASTER_SECURITY_OVERRIDE_0 : R/W; bitpos: [0]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_0    (BIT(0))
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_0_M  (AXI_IC_REG_MASTER_SECURITY_OVERRIDE_0_V << AXI_IC_REG_MASTER_SECURITY_OVERRIDE_0_S)
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_0_V  0x00000001U
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_0_S  0
/** AXI_IC_REG_MASTER_SECURITY_OVERRIDE_1 : R/W; bitpos: [1]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_1    (BIT(1))
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_1_M  (AXI_IC_REG_MASTER_SECURITY_OVERRIDE_1_V << AXI_IC_REG_MASTER_SECURITY_OVERRIDE_1_S)
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_1_V  0x00000001U
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_1_S  1
/** AXI_IC_REG_MASTER_SECURITY_OVERRIDE_2 : R/W; bitpos: [2]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_2    (BIT(2))
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_2_M  (AXI_IC_REG_MASTER_SECURITY_OVERRIDE_2_V << AXI_IC_REG_MASTER_SECURITY_OVERRIDE_2_S)
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_2_V  0x00000001U
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_2_S  2
/** AXI_IC_REG_MASTER_SECURITY_OVERRIDE_3 : R/W; bitpos: [3]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_3    (BIT(3))
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_3_M  (AXI_IC_REG_MASTER_SECURITY_OVERRIDE_3_V << AXI_IC_REG_MASTER_SECURITY_OVERRIDE_3_S)
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_3_V  0x00000001U
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_3_S  3
/** AXI_IC_REG_MASTER_SECURITY_OVERRIDE_4 : R/W; bitpos: [4]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_4    (BIT(4))
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_4_M  (AXI_IC_REG_MASTER_SECURITY_OVERRIDE_4_V << AXI_IC_REG_MASTER_SECURITY_OVERRIDE_4_S)
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_4_V  0x00000001U
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_4_S  4
/** AXI_IC_REG_MASTER_SECURITY_OVERRIDE_5 : R/W; bitpos: [5]; default: 0;
 *  0 means not overridden, and 1 means overridden.
 */
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_5    (BIT(5))
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_5_M  (AXI_IC_REG_MASTER_SECURITY_OVERRIDE_5_V << AXI_IC_REG_MASTER_SECURITY_OVERRIDE_5_S)
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_5_V  0x00000001U
#define AXI_IC_REG_MASTER_SECURITY_OVERRIDE_5_S  5

/** AXI_IC_AW_QOS_VALUE_0_REG register
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_AW_QOS_VALUE_0_REG (DR_REG_AXI_IC_BASE + 0x94)
/** AXI_IC_REG_AW_QOS_VALUE_0 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_REG_AW_QOS_VALUE_0    0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_0_M  (AXI_IC_REG_AW_QOS_VALUE_0_V << AXI_IC_REG_AW_QOS_VALUE_0_S)
#define AXI_IC_REG_AW_QOS_VALUE_0_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_0_S  0

/** AXI_IC_AW_QOS_VALUE_1_REG register
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_AW_QOS_VALUE_1_REG (DR_REG_AXI_IC_BASE + 0x98)
/** AXI_IC_REG_AW_QOS_VALUE_1 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_REG_AW_QOS_VALUE_1    0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_1_M  (AXI_IC_REG_AW_QOS_VALUE_1_V << AXI_IC_REG_AW_QOS_VALUE_1_S)
#define AXI_IC_REG_AW_QOS_VALUE_1_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_1_S  0

/** AXI_IC_AW_QOS_VALUE_2_REG register
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_AW_QOS_VALUE_2_REG (DR_REG_AXI_IC_BASE + 0x9c)
/** AXI_IC_REG_AW_QOS_VALUE_2 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_REG_AW_QOS_VALUE_2    0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_2_M  (AXI_IC_REG_AW_QOS_VALUE_2_V << AXI_IC_REG_AW_QOS_VALUE_2_S)
#define AXI_IC_REG_AW_QOS_VALUE_2_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_2_S  0

/** AXI_IC_AW_QOS_VALUE_3_REG register
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_AW_QOS_VALUE_3_REG (DR_REG_AXI_IC_BASE + 0xa0)
/** AXI_IC_REG_AW_QOS_VALUE_3 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_REG_AW_QOS_VALUE_3    0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_3_M  (AXI_IC_REG_AW_QOS_VALUE_3_V << AXI_IC_REG_AW_QOS_VALUE_3_S)
#define AXI_IC_REG_AW_QOS_VALUE_3_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_3_S  0

/** AXI_IC_AW_QOS_VALUE_4_REG register
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_AW_QOS_VALUE_4_REG (DR_REG_AXI_IC_BASE + 0xa4)
/** AXI_IC_REG_AW_QOS_VALUE_4 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_REG_AW_QOS_VALUE_4    0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_4_M  (AXI_IC_REG_AW_QOS_VALUE_4_V << AXI_IC_REG_AW_QOS_VALUE_4_S)
#define AXI_IC_REG_AW_QOS_VALUE_4_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_4_S  0

/** AXI_IC_AW_QOS_VALUE_5_REG register
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_AW_QOS_VALUE_5_REG (DR_REG_AXI_IC_BASE + 0xa8)
/** AXI_IC_REG_AW_QOS_VALUE_5 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AW QOS value specified by software.
 */
#define AXI_IC_REG_AW_QOS_VALUE_5    0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_5_M  (AXI_IC_REG_AW_QOS_VALUE_5_V << AXI_IC_REG_AW_QOS_VALUE_5_S)
#define AXI_IC_REG_AW_QOS_VALUE_5_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_VALUE_5_S  0

/** AXI_IC_AR_QOS_VALUE_0_REG register
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_AR_QOS_VALUE_0_REG (DR_REG_AXI_IC_BASE + 0xac)
/** AXI_IC_REG_AR_QOS_VALUE_0 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_REG_AR_QOS_VALUE_0    0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_0_M  (AXI_IC_REG_AR_QOS_VALUE_0_V << AXI_IC_REG_AR_QOS_VALUE_0_S)
#define AXI_IC_REG_AR_QOS_VALUE_0_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_0_S  0

/** AXI_IC_AR_QOS_VALUE_1_REG register
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_AR_QOS_VALUE_1_REG (DR_REG_AXI_IC_BASE + 0xb0)
/** AXI_IC_REG_AR_QOS_VALUE_1 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_REG_AR_QOS_VALUE_1    0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_1_M  (AXI_IC_REG_AR_QOS_VALUE_1_V << AXI_IC_REG_AR_QOS_VALUE_1_S)
#define AXI_IC_REG_AR_QOS_VALUE_1_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_1_S  0

/** AXI_IC_AR_QOS_VALUE_2_REG register
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_AR_QOS_VALUE_2_REG (DR_REG_AXI_IC_BASE + 0xb4)
/** AXI_IC_REG_AR_QOS_VALUE_2 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_REG_AR_QOS_VALUE_2    0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_2_M  (AXI_IC_REG_AR_QOS_VALUE_2_V << AXI_IC_REG_AR_QOS_VALUE_2_S)
#define AXI_IC_REG_AR_QOS_VALUE_2_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_2_S  0

/** AXI_IC_AR_QOS_VALUE_3_REG register
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_AR_QOS_VALUE_3_REG (DR_REG_AXI_IC_BASE + 0xb8)
/** AXI_IC_REG_AR_QOS_VALUE_3 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_REG_AR_QOS_VALUE_3    0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_3_M  (AXI_IC_REG_AR_QOS_VALUE_3_V << AXI_IC_REG_AR_QOS_VALUE_3_S)
#define AXI_IC_REG_AR_QOS_VALUE_3_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_3_S  0

/** AXI_IC_AR_QOS_VALUE_4_REG register
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_AR_QOS_VALUE_4_REG (DR_REG_AXI_IC_BASE + 0xbc)
/** AXI_IC_REG_AR_QOS_VALUE_4 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_REG_AR_QOS_VALUE_4    0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_4_M  (AXI_IC_REG_AR_QOS_VALUE_4_V << AXI_IC_REG_AR_QOS_VALUE_4_S)
#define AXI_IC_REG_AR_QOS_VALUE_4_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_4_S  0

/** AXI_IC_AR_QOS_VALUE_5_REG register
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_AR_QOS_VALUE_5_REG (DR_REG_AXI_IC_BASE + 0xc0)
/** AXI_IC_REG_AR_QOS_VALUE_5 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core AR QOS value specified by software.
 */
#define AXI_IC_REG_AR_QOS_VALUE_5    0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_5_M  (AXI_IC_REG_AR_QOS_VALUE_5_V << AXI_IC_REG_AR_QOS_VALUE_5_S)
#define AXI_IC_REG_AR_QOS_VALUE_5_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_VALUE_5_S  0

/** AXI_IC_B_PRIOR_VALUE_0_REG register
 *  AXI IC core B priority value specified by software.
 */
#define AXI_IC_B_PRIOR_VALUE_0_REG (DR_REG_AXI_IC_BASE + 0xc4)
/** AXI_IC_REG_B_PRIOR_VALUE_0 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core B priority value specified by software.
 */
#define AXI_IC_REG_B_PRIOR_VALUE_0    0x0000000FU
#define AXI_IC_REG_B_PRIOR_VALUE_0_M  (AXI_IC_REG_B_PRIOR_VALUE_0_V << AXI_IC_REG_B_PRIOR_VALUE_0_S)
#define AXI_IC_REG_B_PRIOR_VALUE_0_V  0x0000000FU
#define AXI_IC_REG_B_PRIOR_VALUE_0_S  0

/** AXI_IC_B_PRIOR_VALUE_1_REG register
 *  AXI IC core B priority value specified by software.
 */
#define AXI_IC_B_PRIOR_VALUE_1_REG (DR_REG_AXI_IC_BASE + 0xc8)
/** AXI_IC_REG_B_PRIOR_VALUE_1 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core B priority value specified by software.
 */
#define AXI_IC_REG_B_PRIOR_VALUE_1    0x0000000FU
#define AXI_IC_REG_B_PRIOR_VALUE_1_M  (AXI_IC_REG_B_PRIOR_VALUE_1_V << AXI_IC_REG_B_PRIOR_VALUE_1_S)
#define AXI_IC_REG_B_PRIOR_VALUE_1_V  0x0000000FU
#define AXI_IC_REG_B_PRIOR_VALUE_1_S  0

/** AXI_IC_B_PRIOR_VALUE_2_REG register
 *  AXI IC core B priority value specified by software.
 */
#define AXI_IC_B_PRIOR_VALUE_2_REG (DR_REG_AXI_IC_BASE + 0xcc)
/** AXI_IC_REG_B_PRIOR_VALUE_2 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core B priority value specified by software.
 */
#define AXI_IC_REG_B_PRIOR_VALUE_2    0x0000000FU
#define AXI_IC_REG_B_PRIOR_VALUE_2_M  (AXI_IC_REG_B_PRIOR_VALUE_2_V << AXI_IC_REG_B_PRIOR_VALUE_2_S)
#define AXI_IC_REG_B_PRIOR_VALUE_2_V  0x0000000FU
#define AXI_IC_REG_B_PRIOR_VALUE_2_S  0

/** AXI_IC_R_PRIOR_VALUE_0_REG register
 *  AXI IC core R priority value specified by software.
 */
#define AXI_IC_R_PRIOR_VALUE_0_REG (DR_REG_AXI_IC_BASE + 0xd0)
/** AXI_IC_REG_R_PRIOR_VALUE_0 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core R priority value specified by software.
 */
#define AXI_IC_REG_R_PRIOR_VALUE_0    0x0000000FU
#define AXI_IC_REG_R_PRIOR_VALUE_0_M  (AXI_IC_REG_R_PRIOR_VALUE_0_V << AXI_IC_REG_R_PRIOR_VALUE_0_S)
#define AXI_IC_REG_R_PRIOR_VALUE_0_V  0x0000000FU
#define AXI_IC_REG_R_PRIOR_VALUE_0_S  0

/** AXI_IC_R_PRIOR_VALUE_1_REG register
 *  AXI IC core R priority value specified by software.
 */
#define AXI_IC_R_PRIOR_VALUE_1_REG (DR_REG_AXI_IC_BASE + 0xd4)
/** AXI_IC_REG_R_PRIOR_VALUE_1 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core R priority value specified by software.
 */
#define AXI_IC_REG_R_PRIOR_VALUE_1    0x0000000FU
#define AXI_IC_REG_R_PRIOR_VALUE_1_M  (AXI_IC_REG_R_PRIOR_VALUE_1_V << AXI_IC_REG_R_PRIOR_VALUE_1_S)
#define AXI_IC_REG_R_PRIOR_VALUE_1_V  0x0000000FU
#define AXI_IC_REG_R_PRIOR_VALUE_1_S  0

/** AXI_IC_R_PRIOR_VALUE_2_REG register
 *  AXI IC core R priority value specified by software.
 */
#define AXI_IC_R_PRIOR_VALUE_2_REG (DR_REG_AXI_IC_BASE + 0xd8)
/** AXI_IC_REG_R_PRIOR_VALUE_2 : R/W; bitpos: [3:0]; default: 0;
 *  AXI IC core R priority value specified by software.
 */
#define AXI_IC_REG_R_PRIOR_VALUE_2    0x0000000FU
#define AXI_IC_REG_R_PRIOR_VALUE_2_M  (AXI_IC_REG_R_PRIOR_VALUE_2_V << AXI_IC_REG_R_PRIOR_VALUE_2_S)
#define AXI_IC_REG_R_PRIOR_VALUE_2_V  0x0000000FU
#define AXI_IC_REG_R_PRIOR_VALUE_2_S  0

/** AXI_IC_AW_QOS_SEL_0_REG register
 *  AXI IC core AW QOS select signal specified by software.
 */
#define AXI_IC_AW_QOS_SEL_0_REG (DR_REG_AXI_IC_BASE + 0xdc)
/** AXI_IC_REG_AW_QOS_SEL_0 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AW_QOS_SEL_0    0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_0_M  (AXI_IC_REG_AW_QOS_SEL_0_V << AXI_IC_REG_AW_QOS_SEL_0_S)
#define AXI_IC_REG_AW_QOS_SEL_0_V  0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_0_S  0

/** AXI_IC_AW_QOS_SEL_1_REG register
 *  AXI IC core AW QOS select signal specified by software.
 */
#define AXI_IC_AW_QOS_SEL_1_REG (DR_REG_AXI_IC_BASE + 0xe0)
/** AXI_IC_REG_AW_QOS_SEL_1 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AW_QOS_SEL_1    0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_1_M  (AXI_IC_REG_AW_QOS_SEL_1_V << AXI_IC_REG_AW_QOS_SEL_1_S)
#define AXI_IC_REG_AW_QOS_SEL_1_V  0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_1_S  0

/** AXI_IC_AW_QOS_SEL_2_REG register
 *  AXI IC core AW QOS select signal specified by software.
 */
#define AXI_IC_AW_QOS_SEL_2_REG (DR_REG_AXI_IC_BASE + 0xe4)
/** AXI_IC_REG_AW_QOS_SEL_2 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AW_QOS_SEL_2    0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_2_M  (AXI_IC_REG_AW_QOS_SEL_2_V << AXI_IC_REG_AW_QOS_SEL_2_S)
#define AXI_IC_REG_AW_QOS_SEL_2_V  0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_2_S  0

/** AXI_IC_AW_QOS_SEL_3_REG register
 *  AXI IC core AW QOS select signal specified by software.
 */
#define AXI_IC_AW_QOS_SEL_3_REG (DR_REG_AXI_IC_BASE + 0xe8)
/** AXI_IC_REG_AW_QOS_SEL_3 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AW_QOS_SEL_3    0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_3_M  (AXI_IC_REG_AW_QOS_SEL_3_V << AXI_IC_REG_AW_QOS_SEL_3_S)
#define AXI_IC_REG_AW_QOS_SEL_3_V  0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_3_S  0

/** AXI_IC_AW_QOS_SEL_4_REG register
 *  AXI IC core AW QOS select signal specified by software.
 */
#define AXI_IC_AW_QOS_SEL_4_REG (DR_REG_AXI_IC_BASE + 0xec)
/** AXI_IC_REG_AW_QOS_SEL_4 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AW_QOS_SEL_4    0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_4_M  (AXI_IC_REG_AW_QOS_SEL_4_V << AXI_IC_REG_AW_QOS_SEL_4_S)
#define AXI_IC_REG_AW_QOS_SEL_4_V  0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_4_S  0

/** AXI_IC_AW_QOS_SEL_5_REG register
 *  AXI IC core AW QOS select signal specified by software.
 */
#define AXI_IC_AW_QOS_SEL_5_REG (DR_REG_AXI_IC_BASE + 0xf0)
/** AXI_IC_REG_AW_QOS_SEL_5 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AW_QOS_SEL_5    0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_5_M  (AXI_IC_REG_AW_QOS_SEL_5_V << AXI_IC_REG_AW_QOS_SEL_5_S)
#define AXI_IC_REG_AW_QOS_SEL_5_V  0x00000003U
#define AXI_IC_REG_AW_QOS_SEL_5_S  0

/** AXI_IC_AR_QOS_SEL_0_REG register
 *  AXI IC core AR QOS select signal specified by software.
 */
#define AXI_IC_AR_QOS_SEL_0_REG (DR_REG_AXI_IC_BASE + 0xf4)
/** AXI_IC_REG_AR_QOS_SEL_0 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AR_QOS_SEL_0    0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_0_M  (AXI_IC_REG_AR_QOS_SEL_0_V << AXI_IC_REG_AR_QOS_SEL_0_S)
#define AXI_IC_REG_AR_QOS_SEL_0_V  0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_0_S  0

/** AXI_IC_AR_QOS_SEL_1_REG register
 *  AXI IC core AR QOS select signal specified by software.
 */
#define AXI_IC_AR_QOS_SEL_1_REG (DR_REG_AXI_IC_BASE + 0xf8)
/** AXI_IC_REG_AR_QOS_SEL_1 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AR_QOS_SEL_1    0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_1_M  (AXI_IC_REG_AR_QOS_SEL_1_V << AXI_IC_REG_AR_QOS_SEL_1_S)
#define AXI_IC_REG_AR_QOS_SEL_1_V  0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_1_S  0

/** AXI_IC_AR_QOS_SEL_2_REG register
 *  AXI IC core AR QOS select signal specified by software.
 */
#define AXI_IC_AR_QOS_SEL_2_REG (DR_REG_AXI_IC_BASE + 0xfc)
/** AXI_IC_REG_AR_QOS_SEL_2 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AR_QOS_SEL_2    0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_2_M  (AXI_IC_REG_AR_QOS_SEL_2_V << AXI_IC_REG_AR_QOS_SEL_2_S)
#define AXI_IC_REG_AR_QOS_SEL_2_V  0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_2_S  0

/** AXI_IC_AR_QOS_SEL_3_REG register
 *  AXI IC core AR QOS select signal specified by software.
 */
#define AXI_IC_AR_QOS_SEL_3_REG (DR_REG_AXI_IC_BASE + 0x100)
/** AXI_IC_REG_AR_QOS_SEL_3 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AR_QOS_SEL_3    0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_3_M  (AXI_IC_REG_AR_QOS_SEL_3_V << AXI_IC_REG_AR_QOS_SEL_3_S)
#define AXI_IC_REG_AR_QOS_SEL_3_V  0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_3_S  0

/** AXI_IC_AR_QOS_SEL_4_REG register
 *  AXI IC core AR QOS select signal specified by software.
 */
#define AXI_IC_AR_QOS_SEL_4_REG (DR_REG_AXI_IC_BASE + 0x104)
/** AXI_IC_REG_AR_QOS_SEL_4 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AR_QOS_SEL_4    0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_4_M  (AXI_IC_REG_AR_QOS_SEL_4_V << AXI_IC_REG_AR_QOS_SEL_4_S)
#define AXI_IC_REG_AR_QOS_SEL_4_V  0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_4_S  0

/** AXI_IC_AR_QOS_SEL_5_REG register
 *  AXI IC core AR QOS select signal specified by software.
 */
#define AXI_IC_AR_QOS_SEL_5_REG (DR_REG_AXI_IC_BASE + 0x108)
/** AXI_IC_REG_AR_QOS_SEL_5 : R/W; bitpos: [1:0]; default: 0;
 *  2'b00 selects the original QOS value, 2'b01 selects the QOS value set by the
 *  software, 2'b10 selects the QOS value set by the QOS regulator.
 */
#define AXI_IC_REG_AR_QOS_SEL_5    0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_5_M  (AXI_IC_REG_AR_QOS_SEL_5_V << AXI_IC_REG_AR_QOS_SEL_5_S)
#define AXI_IC_REG_AR_QOS_SEL_5_V  0x00000003U
#define AXI_IC_REG_AR_QOS_SEL_5_S  0

/** AXI_IC_AW_QOS_RECOVERY_REG register
 *  AXI IC core AW QOS recovery signal specified by software.
 */
#define AXI_IC_AW_QOS_RECOVERY_REG (DR_REG_AXI_IC_BASE + 0x10c)
/** AXI_IC_REG_AW_QOS_RECOVERY_0 : R/W; bitpos: [0]; default: 0;
 *  Use QOS_COPY in SI_IF AW payload when it is asserted, otherwise use QOS_NEW (from
 *  QOS regulator or software).
 */
#define AXI_IC_REG_AW_QOS_RECOVERY_0    (BIT(0))
#define AXI_IC_REG_AW_QOS_RECOVERY_0_M  (AXI_IC_REG_AW_QOS_RECOVERY_0_V << AXI_IC_REG_AW_QOS_RECOVERY_0_S)
#define AXI_IC_REG_AW_QOS_RECOVERY_0_V  0x00000001U
#define AXI_IC_REG_AW_QOS_RECOVERY_0_S  0
/** AXI_IC_REG_AW_QOS_RECOVERY_1 : R/W; bitpos: [1]; default: 0;
 *  Use QOS_COPY in SI_IF AW payload when it is asserted, otherwise use QOS_NEW (from
 *  QOS regulator or software).
 */
#define AXI_IC_REG_AW_QOS_RECOVERY_1    (BIT(1))
#define AXI_IC_REG_AW_QOS_RECOVERY_1_M  (AXI_IC_REG_AW_QOS_RECOVERY_1_V << AXI_IC_REG_AW_QOS_RECOVERY_1_S)
#define AXI_IC_REG_AW_QOS_RECOVERY_1_V  0x00000001U
#define AXI_IC_REG_AW_QOS_RECOVERY_1_S  1
/** AXI_IC_REG_AW_QOS_RECOVERY_2 : R/W; bitpos: [2]; default: 0;
 *  Use QOS_COPY in SI_IF AW payload when it is asserted, otherwise use QOS_NEW (from
 *  QOS regulator or software).
 */
#define AXI_IC_REG_AW_QOS_RECOVERY_2    (BIT(2))
#define AXI_IC_REG_AW_QOS_RECOVERY_2_M  (AXI_IC_REG_AW_QOS_RECOVERY_2_V << AXI_IC_REG_AW_QOS_RECOVERY_2_S)
#define AXI_IC_REG_AW_QOS_RECOVERY_2_V  0x00000001U
#define AXI_IC_REG_AW_QOS_RECOVERY_2_S  2

/** AXI_IC_AR_QOS_RECOVERY_REG register
 *  AXI IC core AR QOS recovery signal specified by software.
 */
#define AXI_IC_AR_QOS_RECOVERY_REG (DR_REG_AXI_IC_BASE + 0x110)
/** AXI_IC_REG_AR_QOS_RECOVERY_0 : R/W; bitpos: [0]; default: 0;
 *  Use QOS_COPY in SI_IF AR payload when it is asserted, otherwise use QOS_NEW (from
 *  QOS regulator or software).
 */
#define AXI_IC_REG_AR_QOS_RECOVERY_0    (BIT(0))
#define AXI_IC_REG_AR_QOS_RECOVERY_0_M  (AXI_IC_REG_AR_QOS_RECOVERY_0_V << AXI_IC_REG_AR_QOS_RECOVERY_0_S)
#define AXI_IC_REG_AR_QOS_RECOVERY_0_V  0x00000001U
#define AXI_IC_REG_AR_QOS_RECOVERY_0_S  0
/** AXI_IC_REG_AR_QOS_RECOVERY_1 : R/W; bitpos: [1]; default: 0;
 *  Use QOS_COPY in SI_IF AR payload when it is asserted, otherwise use QOS_NEW (from
 *  QOS regulator or software).
 */
#define AXI_IC_REG_AR_QOS_RECOVERY_1    (BIT(1))
#define AXI_IC_REG_AR_QOS_RECOVERY_1_M  (AXI_IC_REG_AR_QOS_RECOVERY_1_V << AXI_IC_REG_AR_QOS_RECOVERY_1_S)
#define AXI_IC_REG_AR_QOS_RECOVERY_1_V  0x00000001U
#define AXI_IC_REG_AR_QOS_RECOVERY_1_S  1
/** AXI_IC_REG_AR_QOS_RECOVERY_2 : R/W; bitpos: [2]; default: 0;
 *  Use QOS_COPY in SI_IF AR payload when it is asserted, otherwise use QOS_NEW (from
 *  QOS regulator or software).
 */
#define AXI_IC_REG_AR_QOS_RECOVERY_2    (BIT(2))
#define AXI_IC_REG_AR_QOS_RECOVERY_2_M  (AXI_IC_REG_AR_QOS_RECOVERY_2_V << AXI_IC_REG_AR_QOS_RECOVERY_2_S)
#define AXI_IC_REG_AR_QOS_RECOVERY_2_V  0x00000001U
#define AXI_IC_REG_AR_QOS_RECOVERY_2_S  2

/** AXI_IC_AW_QOS_ID_MASK_0_REG register
 *  Config the aw channel id mask for id filter function.
 */
#define AXI_IC_AW_QOS_ID_MASK_0_REG (DR_REG_AXI_IC_BASE + 0x114)
/** AXI_IC_REG_AW_QOS_ID_MASK_0 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_MASK_0    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_0_M  (AXI_IC_REG_AW_QOS_ID_MASK_0_V << AXI_IC_REG_AW_QOS_ID_MASK_0_S)
#define AXI_IC_REG_AW_QOS_ID_MASK_0_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_0_S  0

/** AXI_IC_AW_QOS_ID_MASK_1_REG register
 *  Config the aw channel id mask for id filter function.
 */
#define AXI_IC_AW_QOS_ID_MASK_1_REG (DR_REG_AXI_IC_BASE + 0x118)
/** AXI_IC_REG_AW_QOS_ID_MASK_1 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_MASK_1    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_1_M  (AXI_IC_REG_AW_QOS_ID_MASK_1_V << AXI_IC_REG_AW_QOS_ID_MASK_1_S)
#define AXI_IC_REG_AW_QOS_ID_MASK_1_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_1_S  0

/** AXI_IC_AW_QOS_ID_MASK_2_REG register
 *  Config the aw channel id mask for id filter function.
 */
#define AXI_IC_AW_QOS_ID_MASK_2_REG (DR_REG_AXI_IC_BASE + 0x11c)
/** AXI_IC_REG_AW_QOS_ID_MASK_2 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_MASK_2    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_2_M  (AXI_IC_REG_AW_QOS_ID_MASK_2_V << AXI_IC_REG_AW_QOS_ID_MASK_2_S)
#define AXI_IC_REG_AW_QOS_ID_MASK_2_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_2_S  0

/** AXI_IC_AW_QOS_ID_MASK_3_REG register
 *  Config the aw channel id mask for id filter function.
 */
#define AXI_IC_AW_QOS_ID_MASK_3_REG (DR_REG_AXI_IC_BASE + 0x120)
/** AXI_IC_REG_AW_QOS_ID_MASK_3 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_MASK_3    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_3_M  (AXI_IC_REG_AW_QOS_ID_MASK_3_V << AXI_IC_REG_AW_QOS_ID_MASK_3_S)
#define AXI_IC_REG_AW_QOS_ID_MASK_3_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_3_S  0

/** AXI_IC_AW_QOS_ID_MASK_4_REG register
 *  Config the aw channel id mask for id filter function.
 */
#define AXI_IC_AW_QOS_ID_MASK_4_REG (DR_REG_AXI_IC_BASE + 0x124)
/** AXI_IC_REG_AW_QOS_ID_MASK_4 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_MASK_4    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_4_M  (AXI_IC_REG_AW_QOS_ID_MASK_4_V << AXI_IC_REG_AW_QOS_ID_MASK_4_S)
#define AXI_IC_REG_AW_QOS_ID_MASK_4_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_4_S  0

/** AXI_IC_AW_QOS_ID_MASK_5_REG register
 *  Config the aw channel id mask for id filter function.
 */
#define AXI_IC_AW_QOS_ID_MASK_5_REG (DR_REG_AXI_IC_BASE + 0x128)
/** AXI_IC_REG_AW_QOS_ID_MASK_5 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_MASK_5    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_5_M  (AXI_IC_REG_AW_QOS_ID_MASK_5_V << AXI_IC_REG_AW_QOS_ID_MASK_5_S)
#define AXI_IC_REG_AW_QOS_ID_MASK_5_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_MASK_5_S  0

/** AXI_IC_AR_QOS_ID_MASK_0_REG register
 *  Config the ar channel id mask for id filter function.
 */
#define AXI_IC_AR_QOS_ID_MASK_0_REG (DR_REG_AXI_IC_BASE + 0x12c)
/** AXI_IC_REG_AR_QOS_ID_MASK_0 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_MASK_0    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_0_M  (AXI_IC_REG_AR_QOS_ID_MASK_0_V << AXI_IC_REG_AR_QOS_ID_MASK_0_S)
#define AXI_IC_REG_AR_QOS_ID_MASK_0_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_0_S  0

/** AXI_IC_AR_QOS_ID_MASK_1_REG register
 *  Config the ar channel id mask for id filter function.
 */
#define AXI_IC_AR_QOS_ID_MASK_1_REG (DR_REG_AXI_IC_BASE + 0x130)
/** AXI_IC_REG_AR_QOS_ID_MASK_1 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_MASK_1    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_1_M  (AXI_IC_REG_AR_QOS_ID_MASK_1_V << AXI_IC_REG_AR_QOS_ID_MASK_1_S)
#define AXI_IC_REG_AR_QOS_ID_MASK_1_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_1_S  0

/** AXI_IC_AR_QOS_ID_MASK_2_REG register
 *  Config the ar channel id mask for id filter function.
 */
#define AXI_IC_AR_QOS_ID_MASK_2_REG (DR_REG_AXI_IC_BASE + 0x134)
/** AXI_IC_REG_AR_QOS_ID_MASK_2 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_MASK_2    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_2_M  (AXI_IC_REG_AR_QOS_ID_MASK_2_V << AXI_IC_REG_AR_QOS_ID_MASK_2_S)
#define AXI_IC_REG_AR_QOS_ID_MASK_2_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_2_S  0

/** AXI_IC_AR_QOS_ID_MASK_3_REG register
 *  Config the ar channel id mask for id filter function.
 */
#define AXI_IC_AR_QOS_ID_MASK_3_REG (DR_REG_AXI_IC_BASE + 0x138)
/** AXI_IC_REG_AR_QOS_ID_MASK_3 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_MASK_3    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_3_M  (AXI_IC_REG_AR_QOS_ID_MASK_3_V << AXI_IC_REG_AR_QOS_ID_MASK_3_S)
#define AXI_IC_REG_AR_QOS_ID_MASK_3_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_3_S  0

/** AXI_IC_AR_QOS_ID_MASK_4_REG register
 *  Config the ar channel id mask for id filter function.
 */
#define AXI_IC_AR_QOS_ID_MASK_4_REG (DR_REG_AXI_IC_BASE + 0x13c)
/** AXI_IC_REG_AR_QOS_ID_MASK_4 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_MASK_4    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_4_M  (AXI_IC_REG_AR_QOS_ID_MASK_4_V << AXI_IC_REG_AR_QOS_ID_MASK_4_S)
#define AXI_IC_REG_AR_QOS_ID_MASK_4_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_4_S  0

/** AXI_IC_AR_QOS_ID_MASK_5_REG register
 *  Config the ar channel id mask for id filter function.
 */
#define AXI_IC_AR_QOS_ID_MASK_5_REG (DR_REG_AXI_IC_BASE + 0x140)
/** AXI_IC_REG_AR_QOS_ID_MASK_5 : R/W; bitpos: [3:0]; default: 15;
 *  mask for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_MASK_5    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_5_M  (AXI_IC_REG_AR_QOS_ID_MASK_5_V << AXI_IC_REG_AR_QOS_ID_MASK_5_S)
#define AXI_IC_REG_AR_QOS_ID_MASK_5_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_MASK_5_S  0

/** AXI_IC_AW_QOS_ID_FILTER_0_REG register
 *  Config the aw channel id filter for id filter function.
 */
#define AXI_IC_AW_QOS_ID_FILTER_0_REG (DR_REG_AXI_IC_BASE + 0x144)
/** AXI_IC_REG_AW_QOS_ID_FILTER_0 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_FILTER_0    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_0_M  (AXI_IC_REG_AW_QOS_ID_FILTER_0_V << AXI_IC_REG_AW_QOS_ID_FILTER_0_S)
#define AXI_IC_REG_AW_QOS_ID_FILTER_0_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_0_S  0

/** AXI_IC_AW_QOS_ID_FILTER_1_REG register
 *  Config the aw channel id filter for id filter function.
 */
#define AXI_IC_AW_QOS_ID_FILTER_1_REG (DR_REG_AXI_IC_BASE + 0x148)
/** AXI_IC_REG_AW_QOS_ID_FILTER_1 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_FILTER_1    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_1_M  (AXI_IC_REG_AW_QOS_ID_FILTER_1_V << AXI_IC_REG_AW_QOS_ID_FILTER_1_S)
#define AXI_IC_REG_AW_QOS_ID_FILTER_1_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_1_S  0

/** AXI_IC_AW_QOS_ID_FILTER_2_REG register
 *  Config the aw channel id filter for id filter function.
 */
#define AXI_IC_AW_QOS_ID_FILTER_2_REG (DR_REG_AXI_IC_BASE + 0x14c)
/** AXI_IC_REG_AW_QOS_ID_FILTER_2 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_FILTER_2    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_2_M  (AXI_IC_REG_AW_QOS_ID_FILTER_2_V << AXI_IC_REG_AW_QOS_ID_FILTER_2_S)
#define AXI_IC_REG_AW_QOS_ID_FILTER_2_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_2_S  0

/** AXI_IC_AW_QOS_ID_FILTER_3_REG register
 *  Config the aw channel id filter for id filter function.
 */
#define AXI_IC_AW_QOS_ID_FILTER_3_REG (DR_REG_AXI_IC_BASE + 0x150)
/** AXI_IC_REG_AW_QOS_ID_FILTER_3 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_FILTER_3    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_3_M  (AXI_IC_REG_AW_QOS_ID_FILTER_3_V << AXI_IC_REG_AW_QOS_ID_FILTER_3_S)
#define AXI_IC_REG_AW_QOS_ID_FILTER_3_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_3_S  0

/** AXI_IC_AW_QOS_ID_FILTER_4_REG register
 *  Config the aw channel id filter for id filter function.
 */
#define AXI_IC_AW_QOS_ID_FILTER_4_REG (DR_REG_AXI_IC_BASE + 0x154)
/** AXI_IC_REG_AW_QOS_ID_FILTER_4 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_FILTER_4    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_4_M  (AXI_IC_REG_AW_QOS_ID_FILTER_4_V << AXI_IC_REG_AW_QOS_ID_FILTER_4_S)
#define AXI_IC_REG_AW_QOS_ID_FILTER_4_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_4_S  0

/** AXI_IC_AW_QOS_ID_FILTER_5_REG register
 *  Config the aw channel id filter for id filter function.
 */
#define AXI_IC_AW_QOS_ID_FILTER_5_REG (DR_REG_AXI_IC_BASE + 0x158)
/** AXI_IC_REG_AW_QOS_ID_FILTER_5 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of aw channel id.
 */
#define AXI_IC_REG_AW_QOS_ID_FILTER_5    0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_5_M  (AXI_IC_REG_AW_QOS_ID_FILTER_5_V << AXI_IC_REG_AW_QOS_ID_FILTER_5_S)
#define AXI_IC_REG_AW_QOS_ID_FILTER_5_V  0x0000000FU
#define AXI_IC_REG_AW_QOS_ID_FILTER_5_S  0

/** AXI_IC_AR_QOS_ID_FILTER_0_REG register
 *  Config the ar channel id filter for id filter function.
 */
#define AXI_IC_AR_QOS_ID_FILTER_0_REG (DR_REG_AXI_IC_BASE + 0x15c)
/** AXI_IC_REG_AR_QOS_ID_FILTER_0 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_FILTER_0    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_0_M  (AXI_IC_REG_AR_QOS_ID_FILTER_0_V << AXI_IC_REG_AR_QOS_ID_FILTER_0_S)
#define AXI_IC_REG_AR_QOS_ID_FILTER_0_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_0_S  0

/** AXI_IC_AR_QOS_ID_FILTER_1_REG register
 *  Config the ar channel id filter for id filter function.
 */
#define AXI_IC_AR_QOS_ID_FILTER_1_REG (DR_REG_AXI_IC_BASE + 0x160)
/** AXI_IC_REG_AR_QOS_ID_FILTER_1 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_FILTER_1    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_1_M  (AXI_IC_REG_AR_QOS_ID_FILTER_1_V << AXI_IC_REG_AR_QOS_ID_FILTER_1_S)
#define AXI_IC_REG_AR_QOS_ID_FILTER_1_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_1_S  0

/** AXI_IC_AR_QOS_ID_FILTER_2_REG register
 *  Config the ar channel id filter for id filter function.
 */
#define AXI_IC_AR_QOS_ID_FILTER_2_REG (DR_REG_AXI_IC_BASE + 0x164)
/** AXI_IC_REG_AR_QOS_ID_FILTER_2 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_FILTER_2    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_2_M  (AXI_IC_REG_AR_QOS_ID_FILTER_2_V << AXI_IC_REG_AR_QOS_ID_FILTER_2_S)
#define AXI_IC_REG_AR_QOS_ID_FILTER_2_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_2_S  0

/** AXI_IC_AR_QOS_ID_FILTER_3_REG register
 *  Config the ar channel id filter for id filter function.
 */
#define AXI_IC_AR_QOS_ID_FILTER_3_REG (DR_REG_AXI_IC_BASE + 0x168)
/** AXI_IC_REG_AR_QOS_ID_FILTER_3 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_FILTER_3    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_3_M  (AXI_IC_REG_AR_QOS_ID_FILTER_3_V << AXI_IC_REG_AR_QOS_ID_FILTER_3_S)
#define AXI_IC_REG_AR_QOS_ID_FILTER_3_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_3_S  0

/** AXI_IC_AR_QOS_ID_FILTER_4_REG register
 *  Config the ar channel id filter for id filter function.
 */
#define AXI_IC_AR_QOS_ID_FILTER_4_REG (DR_REG_AXI_IC_BASE + 0x16c)
/** AXI_IC_REG_AR_QOS_ID_FILTER_4 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_FILTER_4    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_4_M  (AXI_IC_REG_AR_QOS_ID_FILTER_4_V << AXI_IC_REG_AR_QOS_ID_FILTER_4_S)
#define AXI_IC_REG_AR_QOS_ID_FILTER_4_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_4_S  0

/** AXI_IC_AR_QOS_ID_FILTER_5_REG register
 *  Config the ar channel id filter for id filter function.
 */
#define AXI_IC_AR_QOS_ID_FILTER_5_REG (DR_REG_AXI_IC_BASE + 0x170)
/** AXI_IC_REG_AR_QOS_ID_FILTER_5 : R/W; bitpos: [3:0]; default: 15;
 *  filter for enable qos of ar channel id.
 */
#define AXI_IC_REG_AR_QOS_ID_FILTER_5    0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_5_M  (AXI_IC_REG_AR_QOS_ID_FILTER_5_V << AXI_IC_REG_AR_QOS_ID_FILTER_5_S)
#define AXI_IC_REG_AR_QOS_ID_FILTER_5_V  0x0000000FU
#define AXI_IC_REG_AR_QOS_ID_FILTER_5_S  0

/** AXI_IC_AW_QOS_CTRL_CFG_0_REG register
 *  Config the QOS Regulator aw module control.
 */
#define AXI_IC_AW_QOS_CTRL_CFG_0_REG (DR_REG_AXI_IC_BASE + 0x174)
/** AXI_IC_REG_AW_QOS_CTRL_CFG_0 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AW_QOS_CTRL_CFG_0    0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_0_M  (AXI_IC_REG_AW_QOS_CTRL_CFG_0_V << AXI_IC_REG_AW_QOS_CTRL_CFG_0_S)
#define AXI_IC_REG_AW_QOS_CTRL_CFG_0_V  0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_0_S  0

/** AXI_IC_AW_QOS_CTRL_CFG_1_REG register
 *  Config the QOS Regulator aw module control.
 */
#define AXI_IC_AW_QOS_CTRL_CFG_1_REG (DR_REG_AXI_IC_BASE + 0x178)
/** AXI_IC_REG_AW_QOS_CTRL_CFG_1 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AW_QOS_CTRL_CFG_1    0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_1_M  (AXI_IC_REG_AW_QOS_CTRL_CFG_1_V << AXI_IC_REG_AW_QOS_CTRL_CFG_1_S)
#define AXI_IC_REG_AW_QOS_CTRL_CFG_1_V  0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_1_S  0

/** AXI_IC_AW_QOS_CTRL_CFG_2_REG register
 *  Config the QOS Regulator aw module control.
 */
#define AXI_IC_AW_QOS_CTRL_CFG_2_REG (DR_REG_AXI_IC_BASE + 0x17c)
/** AXI_IC_REG_AW_QOS_CTRL_CFG_2 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AW_QOS_CTRL_CFG_2    0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_2_M  (AXI_IC_REG_AW_QOS_CTRL_CFG_2_V << AXI_IC_REG_AW_QOS_CTRL_CFG_2_S)
#define AXI_IC_REG_AW_QOS_CTRL_CFG_2_V  0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_2_S  0

/** AXI_IC_AW_QOS_CTRL_CFG_3_REG register
 *  Config the QOS Regulator aw module control.
 */
#define AXI_IC_AW_QOS_CTRL_CFG_3_REG (DR_REG_AXI_IC_BASE + 0x180)
/** AXI_IC_REG_AW_QOS_CTRL_CFG_3 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AW_QOS_CTRL_CFG_3    0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_3_M  (AXI_IC_REG_AW_QOS_CTRL_CFG_3_V << AXI_IC_REG_AW_QOS_CTRL_CFG_3_S)
#define AXI_IC_REG_AW_QOS_CTRL_CFG_3_V  0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_3_S  0

/** AXI_IC_AW_QOS_CTRL_CFG_4_REG register
 *  Config the QOS Regulator aw module control.
 */
#define AXI_IC_AW_QOS_CTRL_CFG_4_REG (DR_REG_AXI_IC_BASE + 0x184)
/** AXI_IC_REG_AW_QOS_CTRL_CFG_4 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AW_QOS_CTRL_CFG_4    0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_4_M  (AXI_IC_REG_AW_QOS_CTRL_CFG_4_V << AXI_IC_REG_AW_QOS_CTRL_CFG_4_S)
#define AXI_IC_REG_AW_QOS_CTRL_CFG_4_V  0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_4_S  0

/** AXI_IC_AW_QOS_CTRL_CFG_5_REG register
 *  Config the QOS Regulator aw module control.
 */
#define AXI_IC_AW_QOS_CTRL_CFG_5_REG (DR_REG_AXI_IC_BASE + 0x188)
/** AXI_IC_REG_AW_QOS_CTRL_CFG_5 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AW_QOS_CTRL_CFG_5    0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_5_M  (AXI_IC_REG_AW_QOS_CTRL_CFG_5_V << AXI_IC_REG_AW_QOS_CTRL_CFG_5_S)
#define AXI_IC_REG_AW_QOS_CTRL_CFG_5_V  0xFFFFFFFFU
#define AXI_IC_REG_AW_QOS_CTRL_CFG_5_S  0

/** AXI_IC_AR_QOS_CTRL_CFG_0_REG register
 *  Config the QOS Regulator ar module control.
 */
#define AXI_IC_AR_QOS_CTRL_CFG_0_REG (DR_REG_AXI_IC_BASE + 0x18c)
/** AXI_IC_REG_AR_QOS_CTRL_CFG_0 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AR_QOS_CTRL_CFG_0    0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_0_M  (AXI_IC_REG_AR_QOS_CTRL_CFG_0_V << AXI_IC_REG_AR_QOS_CTRL_CFG_0_S)
#define AXI_IC_REG_AR_QOS_CTRL_CFG_0_V  0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_0_S  0

/** AXI_IC_AR_QOS_CTRL_CFG_1_REG register
 *  Config the QOS Regulator ar module control.
 */
#define AXI_IC_AR_QOS_CTRL_CFG_1_REG (DR_REG_AXI_IC_BASE + 0x190)
/** AXI_IC_REG_AR_QOS_CTRL_CFG_1 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AR_QOS_CTRL_CFG_1    0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_1_M  (AXI_IC_REG_AR_QOS_CTRL_CFG_1_V << AXI_IC_REG_AR_QOS_CTRL_CFG_1_S)
#define AXI_IC_REG_AR_QOS_CTRL_CFG_1_V  0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_1_S  0

/** AXI_IC_AR_QOS_CTRL_CFG_2_REG register
 *  Config the QOS Regulator ar module control.
 */
#define AXI_IC_AR_QOS_CTRL_CFG_2_REG (DR_REG_AXI_IC_BASE + 0x194)
/** AXI_IC_REG_AR_QOS_CTRL_CFG_2 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AR_QOS_CTRL_CFG_2    0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_2_M  (AXI_IC_REG_AR_QOS_CTRL_CFG_2_V << AXI_IC_REG_AR_QOS_CTRL_CFG_2_S)
#define AXI_IC_REG_AR_QOS_CTRL_CFG_2_V  0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_2_S  0

/** AXI_IC_AR_QOS_CTRL_CFG_3_REG register
 *  Config the QOS Regulator ar module control.
 */
#define AXI_IC_AR_QOS_CTRL_CFG_3_REG (DR_REG_AXI_IC_BASE + 0x198)
/** AXI_IC_REG_AR_QOS_CTRL_CFG_3 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AR_QOS_CTRL_CFG_3    0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_3_M  (AXI_IC_REG_AR_QOS_CTRL_CFG_3_V << AXI_IC_REG_AR_QOS_CTRL_CFG_3_S)
#define AXI_IC_REG_AR_QOS_CTRL_CFG_3_V  0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_3_S  0

/** AXI_IC_AR_QOS_CTRL_CFG_4_REG register
 *  Config the QOS Regulator ar module control.
 */
#define AXI_IC_AR_QOS_CTRL_CFG_4_REG (DR_REG_AXI_IC_BASE + 0x19c)
/** AXI_IC_REG_AR_QOS_CTRL_CFG_4 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AR_QOS_CTRL_CFG_4    0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_4_M  (AXI_IC_REG_AR_QOS_CTRL_CFG_4_V << AXI_IC_REG_AR_QOS_CTRL_CFG_4_S)
#define AXI_IC_REG_AR_QOS_CTRL_CFG_4_V  0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_4_S  0

/** AXI_IC_AR_QOS_CTRL_CFG_5_REG register
 *  Config the QOS Regulator ar module control.
 */
#define AXI_IC_AR_QOS_CTRL_CFG_5_REG (DR_REG_AXI_IC_BASE + 0x1a0)
/** AXI_IC_REG_AR_QOS_CTRL_CFG_5 : R/W; bitpos: [31:0]; default: 8208;
 *  bit[0] enable channel qos, bit[1] enable debug, bit[7:4] mode of vlu regulate,
 *  bit[15:8] step of dly thr, bit[16] dly cnt rst.
 */
#define AXI_IC_REG_AR_QOS_CTRL_CFG_5    0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_5_M  (AXI_IC_REG_AR_QOS_CTRL_CFG_5_V << AXI_IC_REG_AR_QOS_CTRL_CFG_5_S)
#define AXI_IC_REG_AR_QOS_CTRL_CFG_5_V  0xFFFFFFFFU
#define AXI_IC_REG_AR_QOS_CTRL_CFG_5_S  0

/** AXI_IC_AW_QOS_TRANS_RATE_0_REG register
 *  Config the Current Master Write bandwidth.
 */
#define AXI_IC_AW_QOS_TRANS_RATE_0_REG (DR_REG_AXI_IC_BASE + 0x1a4)
/** AXI_IC_REG_AW_QOS_TRANS_RATE_0 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AW_QOS_TRANS_RATE_0    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_0_M  (AXI_IC_REG_AW_QOS_TRANS_RATE_0_V << AXI_IC_REG_AW_QOS_TRANS_RATE_0_S)
#define AXI_IC_REG_AW_QOS_TRANS_RATE_0_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_0_S  0

/** AXI_IC_AW_QOS_TRANS_RATE_1_REG register
 *  Config the Current Master Write bandwidth.
 */
#define AXI_IC_AW_QOS_TRANS_RATE_1_REG (DR_REG_AXI_IC_BASE + 0x1a8)
/** AXI_IC_REG_AW_QOS_TRANS_RATE_1 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AW_QOS_TRANS_RATE_1    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_1_M  (AXI_IC_REG_AW_QOS_TRANS_RATE_1_V << AXI_IC_REG_AW_QOS_TRANS_RATE_1_S)
#define AXI_IC_REG_AW_QOS_TRANS_RATE_1_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_1_S  0

/** AXI_IC_AW_QOS_TRANS_RATE_2_REG register
 *  Config the Current Master Write bandwidth.
 */
#define AXI_IC_AW_QOS_TRANS_RATE_2_REG (DR_REG_AXI_IC_BASE + 0x1ac)
/** AXI_IC_REG_AW_QOS_TRANS_RATE_2 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AW_QOS_TRANS_RATE_2    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_2_M  (AXI_IC_REG_AW_QOS_TRANS_RATE_2_V << AXI_IC_REG_AW_QOS_TRANS_RATE_2_S)
#define AXI_IC_REG_AW_QOS_TRANS_RATE_2_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_2_S  0

/** AXI_IC_AW_QOS_TRANS_RATE_3_REG register
 *  Config the Current Master Write bandwidth.
 */
#define AXI_IC_AW_QOS_TRANS_RATE_3_REG (DR_REG_AXI_IC_BASE + 0x1b0)
/** AXI_IC_REG_AW_QOS_TRANS_RATE_3 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AW_QOS_TRANS_RATE_3    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_3_M  (AXI_IC_REG_AW_QOS_TRANS_RATE_3_V << AXI_IC_REG_AW_QOS_TRANS_RATE_3_S)
#define AXI_IC_REG_AW_QOS_TRANS_RATE_3_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_3_S  0

/** AXI_IC_AW_QOS_TRANS_RATE_4_REG register
 *  Config the Current Master Write bandwidth.
 */
#define AXI_IC_AW_QOS_TRANS_RATE_4_REG (DR_REG_AXI_IC_BASE + 0x1b4)
/** AXI_IC_REG_AW_QOS_TRANS_RATE_4 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AW_QOS_TRANS_RATE_4    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_4_M  (AXI_IC_REG_AW_QOS_TRANS_RATE_4_V << AXI_IC_REG_AW_QOS_TRANS_RATE_4_S)
#define AXI_IC_REG_AW_QOS_TRANS_RATE_4_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_4_S  0

/** AXI_IC_AW_QOS_TRANS_RATE_5_REG register
 *  Config the Current Master Write bandwidth.
 */
#define AXI_IC_AW_QOS_TRANS_RATE_5_REG (DR_REG_AXI_IC_BASE + 0x1b8)
/** AXI_IC_REG_AW_QOS_TRANS_RATE_5 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the aw channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AW_QOS_TRANS_RATE_5    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_5_M  (AXI_IC_REG_AW_QOS_TRANS_RATE_5_V << AXI_IC_REG_AW_QOS_TRANS_RATE_5_S)
#define AXI_IC_REG_AW_QOS_TRANS_RATE_5_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_TRANS_RATE_5_S  0

/** AXI_IC_AR_QOS_TRANS_RATE_0_REG register
 *  Config the Current Master Read bandwidth.
 */
#define AXI_IC_AR_QOS_TRANS_RATE_0_REG (DR_REG_AXI_IC_BASE + 0x1bc)
/** AXI_IC_REG_AR_QOS_TRANS_RATE_0 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AR_QOS_TRANS_RATE_0    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_0_M  (AXI_IC_REG_AR_QOS_TRANS_RATE_0_V << AXI_IC_REG_AR_QOS_TRANS_RATE_0_S)
#define AXI_IC_REG_AR_QOS_TRANS_RATE_0_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_0_S  0

/** AXI_IC_AR_QOS_TRANS_RATE_1_REG register
 *  Config the Current Master Read bandwidth.
 */
#define AXI_IC_AR_QOS_TRANS_RATE_1_REG (DR_REG_AXI_IC_BASE + 0x1c0)
/** AXI_IC_REG_AR_QOS_TRANS_RATE_1 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AR_QOS_TRANS_RATE_1    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_1_M  (AXI_IC_REG_AR_QOS_TRANS_RATE_1_V << AXI_IC_REG_AR_QOS_TRANS_RATE_1_S)
#define AXI_IC_REG_AR_QOS_TRANS_RATE_1_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_1_S  0

/** AXI_IC_AR_QOS_TRANS_RATE_2_REG register
 *  Config the Current Master Read bandwidth.
 */
#define AXI_IC_AR_QOS_TRANS_RATE_2_REG (DR_REG_AXI_IC_BASE + 0x1c4)
/** AXI_IC_REG_AR_QOS_TRANS_RATE_2 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AR_QOS_TRANS_RATE_2    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_2_M  (AXI_IC_REG_AR_QOS_TRANS_RATE_2_V << AXI_IC_REG_AR_QOS_TRANS_RATE_2_S)
#define AXI_IC_REG_AR_QOS_TRANS_RATE_2_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_2_S  0

/** AXI_IC_AR_QOS_TRANS_RATE_3_REG register
 *  Config the Current Master Read bandwidth.
 */
#define AXI_IC_AR_QOS_TRANS_RATE_3_REG (DR_REG_AXI_IC_BASE + 0x1c8)
/** AXI_IC_REG_AR_QOS_TRANS_RATE_3 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AR_QOS_TRANS_RATE_3    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_3_M  (AXI_IC_REG_AR_QOS_TRANS_RATE_3_V << AXI_IC_REG_AR_QOS_TRANS_RATE_3_S)
#define AXI_IC_REG_AR_QOS_TRANS_RATE_3_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_3_S  0

/** AXI_IC_AR_QOS_TRANS_RATE_4_REG register
 *  Config the Current Master Read bandwidth.
 */
#define AXI_IC_AR_QOS_TRANS_RATE_4_REG (DR_REG_AXI_IC_BASE + 0x1cc)
/** AXI_IC_REG_AR_QOS_TRANS_RATE_4 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AR_QOS_TRANS_RATE_4    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_4_M  (AXI_IC_REG_AR_QOS_TRANS_RATE_4_V << AXI_IC_REG_AR_QOS_TRANS_RATE_4_S)
#define AXI_IC_REG_AR_QOS_TRANS_RATE_4_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_4_S  0

/** AXI_IC_AR_QOS_TRANS_RATE_5_REG register
 *  Config the Current Master Read bandwidth.
 */
#define AXI_IC_AR_QOS_TRANS_RATE_5_REG (DR_REG_AXI_IC_BASE + 0x1d0)
/** AXI_IC_REG_AR_QOS_TRANS_RATE_5 : R/W; bitpos: [15:0]; default: 12296;
 *  transaction rate, to control the ar channel bandwidth, default 8 cycles 3 tokens.
 */
#define AXI_IC_REG_AR_QOS_TRANS_RATE_5    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_5_M  (AXI_IC_REG_AR_QOS_TRANS_RATE_5_V << AXI_IC_REG_AR_QOS_TRANS_RATE_5_S)
#define AXI_IC_REG_AR_QOS_TRANS_RATE_5_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_TRANS_RATE_5_S  0

/** AXI_IC_AW_QOS_PEAK_RATE_0_REG register
 *  Config the Current Master Write Peak bandwidth.
 */
#define AXI_IC_AW_QOS_PEAK_RATE_0_REG (DR_REG_AXI_IC_BASE + 0x1d4)
/** AXI_IC_REG_AW_QOS_PEAK_RATE_0 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the aw channel request issue rate, default 0.
 */
#define AXI_IC_REG_AW_QOS_PEAK_RATE_0    0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_0_M  (AXI_IC_REG_AW_QOS_PEAK_RATE_0_V << AXI_IC_REG_AW_QOS_PEAK_RATE_0_S)
#define AXI_IC_REG_AW_QOS_PEAK_RATE_0_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_0_S  0

/** AXI_IC_AW_QOS_PEAK_RATE_1_REG register
 *  Config the Current Master Write Peak bandwidth.
 */
#define AXI_IC_AW_QOS_PEAK_RATE_1_REG (DR_REG_AXI_IC_BASE + 0x1d8)
/** AXI_IC_REG_AW_QOS_PEAK_RATE_1 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the aw channel request issue rate, default 0.
 */
#define AXI_IC_REG_AW_QOS_PEAK_RATE_1    0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_1_M  (AXI_IC_REG_AW_QOS_PEAK_RATE_1_V << AXI_IC_REG_AW_QOS_PEAK_RATE_1_S)
#define AXI_IC_REG_AW_QOS_PEAK_RATE_1_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_1_S  0

/** AXI_IC_AW_QOS_PEAK_RATE_2_REG register
 *  Config the Current Master Write Peak bandwidth.
 */
#define AXI_IC_AW_QOS_PEAK_RATE_2_REG (DR_REG_AXI_IC_BASE + 0x1dc)
/** AXI_IC_REG_AW_QOS_PEAK_RATE_2 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the aw channel request issue rate, default 0.
 */
#define AXI_IC_REG_AW_QOS_PEAK_RATE_2    0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_2_M  (AXI_IC_REG_AW_QOS_PEAK_RATE_2_V << AXI_IC_REG_AW_QOS_PEAK_RATE_2_S)
#define AXI_IC_REG_AW_QOS_PEAK_RATE_2_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_2_S  0

/** AXI_IC_AW_QOS_PEAK_RATE_3_REG register
 *  Config the Current Master Write Peak bandwidth.
 */
#define AXI_IC_AW_QOS_PEAK_RATE_3_REG (DR_REG_AXI_IC_BASE + 0x1e0)
/** AXI_IC_REG_AW_QOS_PEAK_RATE_3 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the aw channel request issue rate, default 0.
 */
#define AXI_IC_REG_AW_QOS_PEAK_RATE_3    0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_3_M  (AXI_IC_REG_AW_QOS_PEAK_RATE_3_V << AXI_IC_REG_AW_QOS_PEAK_RATE_3_S)
#define AXI_IC_REG_AW_QOS_PEAK_RATE_3_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_3_S  0

/** AXI_IC_AW_QOS_PEAK_RATE_4_REG register
 *  Config the Current Master Write Peak bandwidth.
 */
#define AXI_IC_AW_QOS_PEAK_RATE_4_REG (DR_REG_AXI_IC_BASE + 0x1e4)
/** AXI_IC_REG_AW_QOS_PEAK_RATE_4 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the aw channel request issue rate, default 0.
 */
#define AXI_IC_REG_AW_QOS_PEAK_RATE_4    0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_4_M  (AXI_IC_REG_AW_QOS_PEAK_RATE_4_V << AXI_IC_REG_AW_QOS_PEAK_RATE_4_S)
#define AXI_IC_REG_AW_QOS_PEAK_RATE_4_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_4_S  0

/** AXI_IC_AW_QOS_PEAK_RATE_5_REG register
 *  Config the Current Master Write Peak bandwidth.
 */
#define AXI_IC_AW_QOS_PEAK_RATE_5_REG (DR_REG_AXI_IC_BASE + 0x1e8)
/** AXI_IC_REG_AW_QOS_PEAK_RATE_5 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the aw channel request issue rate, default 0.
 */
#define AXI_IC_REG_AW_QOS_PEAK_RATE_5    0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_5_M  (AXI_IC_REG_AW_QOS_PEAK_RATE_5_V << AXI_IC_REG_AW_QOS_PEAK_RATE_5_S)
#define AXI_IC_REG_AW_QOS_PEAK_RATE_5_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_5_S  0

/** AXI_IC_AR_QOS_PEAK_RATE_0_REG register
 *  Config the Current Master Read Peak bandwidth.
 */
#define AXI_IC_AR_QOS_PEAK_RATE_0_REG (DR_REG_AXI_IC_BASE + 0x1ec)
/** AXI_IC_REG_AR_QOS_PEAK_RATE_0 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the ar channel request issue rate, default 0.
 */
#define AXI_IC_REG_AR_QOS_PEAK_RATE_0    0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_0_M  (AXI_IC_REG_AR_QOS_PEAK_RATE_0_V << AXI_IC_REG_AR_QOS_PEAK_RATE_0_S)
#define AXI_IC_REG_AR_QOS_PEAK_RATE_0_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_0_S  0

/** AXI_IC_AR_QOS_PEAK_RATE_1_REG register
 *  Config the Current Master Read Peak bandwidth.
 */
#define AXI_IC_AR_QOS_PEAK_RATE_1_REG (DR_REG_AXI_IC_BASE + 0x1f0)
/** AXI_IC_REG_AR_QOS_PEAK_RATE_1 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the ar channel request issue rate, default 0.
 */
#define AXI_IC_REG_AR_QOS_PEAK_RATE_1    0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_1_M  (AXI_IC_REG_AR_QOS_PEAK_RATE_1_V << AXI_IC_REG_AR_QOS_PEAK_RATE_1_S)
#define AXI_IC_REG_AR_QOS_PEAK_RATE_1_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_1_S  0

/** AXI_IC_AR_QOS_PEAK_RATE_2_REG register
 *  Config the Current Master Read Peak bandwidth.
 */
#define AXI_IC_AR_QOS_PEAK_RATE_2_REG (DR_REG_AXI_IC_BASE + 0x1f4)
/** AXI_IC_REG_AR_QOS_PEAK_RATE_2 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the ar channel request issue rate, default 0.
 */
#define AXI_IC_REG_AR_QOS_PEAK_RATE_2    0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_2_M  (AXI_IC_REG_AR_QOS_PEAK_RATE_2_V << AXI_IC_REG_AR_QOS_PEAK_RATE_2_S)
#define AXI_IC_REG_AR_QOS_PEAK_RATE_2_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_2_S  0

/** AXI_IC_AR_QOS_PEAK_RATE_3_REG register
 *  Config the Current Master Read Peak bandwidth.
 */
#define AXI_IC_AR_QOS_PEAK_RATE_3_REG (DR_REG_AXI_IC_BASE + 0x1f8)
/** AXI_IC_REG_AR_QOS_PEAK_RATE_3 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the ar channel request issue rate, default 0.
 */
#define AXI_IC_REG_AR_QOS_PEAK_RATE_3    0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_3_M  (AXI_IC_REG_AR_QOS_PEAK_RATE_3_V << AXI_IC_REG_AR_QOS_PEAK_RATE_3_S)
#define AXI_IC_REG_AR_QOS_PEAK_RATE_3_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_3_S  0

/** AXI_IC_AR_QOS_PEAK_RATE_4_REG register
 *  Config the Current Master Read Peak bandwidth.
 */
#define AXI_IC_AR_QOS_PEAK_RATE_4_REG (DR_REG_AXI_IC_BASE + 0x1fc)
/** AXI_IC_REG_AR_QOS_PEAK_RATE_4 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the ar channel request issue rate, default 0.
 */
#define AXI_IC_REG_AR_QOS_PEAK_RATE_4    0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_4_M  (AXI_IC_REG_AR_QOS_PEAK_RATE_4_V << AXI_IC_REG_AR_QOS_PEAK_RATE_4_S)
#define AXI_IC_REG_AR_QOS_PEAK_RATE_4_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_4_S  0

/** AXI_IC_AR_QOS_PEAK_RATE_5_REG register
 *  Config the Current Master Read Peak bandwidth.
 */
#define AXI_IC_AR_QOS_PEAK_RATE_5_REG (DR_REG_AXI_IC_BASE + 0x200)
/** AXI_IC_REG_AR_QOS_PEAK_RATE_5 : R/W; bitpos: [7:0]; default: 0;
 *  peak rate, to control the ar channel request issue rate, default 0.
 */
#define AXI_IC_REG_AR_QOS_PEAK_RATE_5    0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_5_M  (AXI_IC_REG_AR_QOS_PEAK_RATE_5_V << AXI_IC_REG_AR_QOS_PEAK_RATE_5_S)
#define AXI_IC_REG_AR_QOS_PEAK_RATE_5_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_5_S  0

/** AXI_IC_AW_QOS_BURSTINESS_0_REG register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
#define AXI_IC_AW_QOS_BURSTINESS_0_REG (DR_REG_AXI_IC_BASE + 0x204)
/** AXI_IC_REG_AW_QOS_BURSTINESS_0 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the aw channel token, default 128 tokens.
 */
#define AXI_IC_REG_AW_QOS_BURSTINESS_0    0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_0_M  (AXI_IC_REG_AW_QOS_BURSTINESS_0_V << AXI_IC_REG_AW_QOS_BURSTINESS_0_S)
#define AXI_IC_REG_AW_QOS_BURSTINESS_0_V  0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_0_S  0

/** AXI_IC_AW_QOS_BURSTINESS_1_REG register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
#define AXI_IC_AW_QOS_BURSTINESS_1_REG (DR_REG_AXI_IC_BASE + 0x208)
/** AXI_IC_REG_AW_QOS_BURSTINESS_1 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the aw channel token, default 128 tokens.
 */
#define AXI_IC_REG_AW_QOS_BURSTINESS_1    0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_1_M  (AXI_IC_REG_AW_QOS_BURSTINESS_1_V << AXI_IC_REG_AW_QOS_BURSTINESS_1_S)
#define AXI_IC_REG_AW_QOS_BURSTINESS_1_V  0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_1_S  0

/** AXI_IC_AW_QOS_BURSTINESS_2_REG register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
#define AXI_IC_AW_QOS_BURSTINESS_2_REG (DR_REG_AXI_IC_BASE + 0x20c)
/** AXI_IC_REG_AW_QOS_BURSTINESS_2 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the aw channel token, default 128 tokens.
 */
#define AXI_IC_REG_AW_QOS_BURSTINESS_2    0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_2_M  (AXI_IC_REG_AW_QOS_BURSTINESS_2_V << AXI_IC_REG_AW_QOS_BURSTINESS_2_S)
#define AXI_IC_REG_AW_QOS_BURSTINESS_2_V  0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_2_S  0

/** AXI_IC_AW_QOS_BURSTINESS_3_REG register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
#define AXI_IC_AW_QOS_BURSTINESS_3_REG (DR_REG_AXI_IC_BASE + 0x210)
/** AXI_IC_REG_AW_QOS_BURSTINESS_3 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the aw channel token, default 128 tokens.
 */
#define AXI_IC_REG_AW_QOS_BURSTINESS_3    0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_3_M  (AXI_IC_REG_AW_QOS_BURSTINESS_3_V << AXI_IC_REG_AW_QOS_BURSTINESS_3_S)
#define AXI_IC_REG_AW_QOS_BURSTINESS_3_V  0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_3_S  0

/** AXI_IC_AW_QOS_BURSTINESS_4_REG register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
#define AXI_IC_AW_QOS_BURSTINESS_4_REG (DR_REG_AXI_IC_BASE + 0x214)
/** AXI_IC_REG_AW_QOS_BURSTINESS_4 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the aw channel token, default 128 tokens.
 */
#define AXI_IC_REG_AW_QOS_BURSTINESS_4    0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_4_M  (AXI_IC_REG_AW_QOS_BURSTINESS_4_V << AXI_IC_REG_AW_QOS_BURSTINESS_4_S)
#define AXI_IC_REG_AW_QOS_BURSTINESS_4_V  0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_4_S  0

/** AXI_IC_AW_QOS_BURSTINESS_5_REG register
 *  The Depth for AW channel TOKEN Busrtiness.
 */
#define AXI_IC_AW_QOS_BURSTINESS_5_REG (DR_REG_AXI_IC_BASE + 0x218)
/** AXI_IC_REG_AW_QOS_BURSTINESS_5 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the aw channel token, default 128 tokens.
 */
#define AXI_IC_REG_AW_QOS_BURSTINESS_5    0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_5_M  (AXI_IC_REG_AW_QOS_BURSTINESS_5_V << AXI_IC_REG_AW_QOS_BURSTINESS_5_S)
#define AXI_IC_REG_AW_QOS_BURSTINESS_5_V  0x00000FFFU
#define AXI_IC_REG_AW_QOS_BURSTINESS_5_S  0

/** AXI_IC_AR_QOS_BURSTINESS_0_REG register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
#define AXI_IC_AR_QOS_BURSTINESS_0_REG (DR_REG_AXI_IC_BASE + 0x21c)
/** AXI_IC_REG_AR_QOS_BURSTINESS_0 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the ar channel token, default 128 tokens.
 */
#define AXI_IC_REG_AR_QOS_BURSTINESS_0    0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_0_M  (AXI_IC_REG_AR_QOS_BURSTINESS_0_V << AXI_IC_REG_AR_QOS_BURSTINESS_0_S)
#define AXI_IC_REG_AR_QOS_BURSTINESS_0_V  0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_0_S  0

/** AXI_IC_AR_QOS_BURSTINESS_1_REG register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
#define AXI_IC_AR_QOS_BURSTINESS_1_REG (DR_REG_AXI_IC_BASE + 0x220)
/** AXI_IC_REG_AR_QOS_BURSTINESS_1 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the ar channel token, default 128 tokens.
 */
#define AXI_IC_REG_AR_QOS_BURSTINESS_1    0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_1_M  (AXI_IC_REG_AR_QOS_BURSTINESS_1_V << AXI_IC_REG_AR_QOS_BURSTINESS_1_S)
#define AXI_IC_REG_AR_QOS_BURSTINESS_1_V  0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_1_S  0

/** AXI_IC_AR_QOS_BURSTINESS_2_REG register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
#define AXI_IC_AR_QOS_BURSTINESS_2_REG (DR_REG_AXI_IC_BASE + 0x224)
/** AXI_IC_REG_AR_QOS_BURSTINESS_2 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the ar channel token, default 128 tokens.
 */
#define AXI_IC_REG_AR_QOS_BURSTINESS_2    0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_2_M  (AXI_IC_REG_AR_QOS_BURSTINESS_2_V << AXI_IC_REG_AR_QOS_BURSTINESS_2_S)
#define AXI_IC_REG_AR_QOS_BURSTINESS_2_V  0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_2_S  0

/** AXI_IC_AR_QOS_BURSTINESS_3_REG register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
#define AXI_IC_AR_QOS_BURSTINESS_3_REG (DR_REG_AXI_IC_BASE + 0x228)
/** AXI_IC_REG_AR_QOS_BURSTINESS_3 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the ar channel token, default 128 tokens.
 */
#define AXI_IC_REG_AR_QOS_BURSTINESS_3    0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_3_M  (AXI_IC_REG_AR_QOS_BURSTINESS_3_V << AXI_IC_REG_AR_QOS_BURSTINESS_3_S)
#define AXI_IC_REG_AR_QOS_BURSTINESS_3_V  0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_3_S  0

/** AXI_IC_AR_QOS_BURSTINESS_4_REG register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
#define AXI_IC_AR_QOS_BURSTINESS_4_REG (DR_REG_AXI_IC_BASE + 0x22c)
/** AXI_IC_REG_AR_QOS_BURSTINESS_4 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the ar channel token, default 128 tokens.
 */
#define AXI_IC_REG_AR_QOS_BURSTINESS_4    0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_4_M  (AXI_IC_REG_AR_QOS_BURSTINESS_4_V << AXI_IC_REG_AR_QOS_BURSTINESS_4_S)
#define AXI_IC_REG_AR_QOS_BURSTINESS_4_V  0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_4_S  0

/** AXI_IC_AR_QOS_BURSTINESS_5_REG register
 *  The Depth for AR channel TOKEN Busrtiness.
 */
#define AXI_IC_AR_QOS_BURSTINESS_5_REG (DR_REG_AXI_IC_BASE + 0x230)
/** AXI_IC_REG_AR_QOS_BURSTINESS_5 : R/W; bitpos: [11:0]; default: 128;
 *  the maximum numbers of the ar channel token, default 128 tokens.
 */
#define AXI_IC_REG_AR_QOS_BURSTINESS_5    0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_5_M  (AXI_IC_REG_AR_QOS_BURSTINESS_5_V << AXI_IC_REG_AR_QOS_BURSTINESS_5_S)
#define AXI_IC_REG_AR_QOS_BURSTINESS_5_V  0x00000FFFU
#define AXI_IC_REG_AR_QOS_BURSTINESS_5_S  0

/** AXI_IC_AW_QOS_CFG_VLU_0_REG register
 *  Config the QOS Priority in AW channel.
 */
#define AXI_IC_AW_QOS_CFG_VLU_0_REG (DR_REG_AXI_IC_BASE + 0x234)
/** AXI_IC_REG_AW_QOS_CFG_VLU_0 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the aw channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AW_QOS_CFG_VLU_0    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_0_M  (AXI_IC_REG_AW_QOS_CFG_VLU_0_V << AXI_IC_REG_AW_QOS_CFG_VLU_0_S)
#define AXI_IC_REG_AW_QOS_CFG_VLU_0_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_0_S  0

/** AXI_IC_AW_QOS_CFG_VLU_1_REG register
 *  Config the QOS Priority in AW channel.
 */
#define AXI_IC_AW_QOS_CFG_VLU_1_REG (DR_REG_AXI_IC_BASE + 0x238)
/** AXI_IC_REG_AW_QOS_CFG_VLU_1 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the aw channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AW_QOS_CFG_VLU_1    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_1_M  (AXI_IC_REG_AW_QOS_CFG_VLU_1_V << AXI_IC_REG_AW_QOS_CFG_VLU_1_S)
#define AXI_IC_REG_AW_QOS_CFG_VLU_1_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_1_S  0

/** AXI_IC_AW_QOS_CFG_VLU_2_REG register
 *  Config the QOS Priority in AW channel.
 */
#define AXI_IC_AW_QOS_CFG_VLU_2_REG (DR_REG_AXI_IC_BASE + 0x23c)
/** AXI_IC_REG_AW_QOS_CFG_VLU_2 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the aw channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AW_QOS_CFG_VLU_2    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_2_M  (AXI_IC_REG_AW_QOS_CFG_VLU_2_V << AXI_IC_REG_AW_QOS_CFG_VLU_2_S)
#define AXI_IC_REG_AW_QOS_CFG_VLU_2_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_2_S  0

/** AXI_IC_AW_QOS_CFG_VLU_3_REG register
 *  Config the QOS Priority in AW channel.
 */
#define AXI_IC_AW_QOS_CFG_VLU_3_REG (DR_REG_AXI_IC_BASE + 0x240)
/** AXI_IC_REG_AW_QOS_CFG_VLU_3 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the aw channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AW_QOS_CFG_VLU_3    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_3_M  (AXI_IC_REG_AW_QOS_CFG_VLU_3_V << AXI_IC_REG_AW_QOS_CFG_VLU_3_S)
#define AXI_IC_REG_AW_QOS_CFG_VLU_3_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_3_S  0

/** AXI_IC_AW_QOS_CFG_VLU_4_REG register
 *  Config the QOS Priority in AW channel.
 */
#define AXI_IC_AW_QOS_CFG_VLU_4_REG (DR_REG_AXI_IC_BASE + 0x244)
/** AXI_IC_REG_AW_QOS_CFG_VLU_4 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the aw channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AW_QOS_CFG_VLU_4    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_4_M  (AXI_IC_REG_AW_QOS_CFG_VLU_4_V << AXI_IC_REG_AW_QOS_CFG_VLU_4_S)
#define AXI_IC_REG_AW_QOS_CFG_VLU_4_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_4_S  0

/** AXI_IC_AW_QOS_CFG_VLU_5_REG register
 *  Config the QOS Priority in AW channel.
 */
#define AXI_IC_AW_QOS_CFG_VLU_5_REG (DR_REG_AXI_IC_BASE + 0x248)
/** AXI_IC_REG_AW_QOS_CFG_VLU_5 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the aw channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AW_QOS_CFG_VLU_5    0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_5_M  (AXI_IC_REG_AW_QOS_CFG_VLU_5_V << AXI_IC_REG_AW_QOS_CFG_VLU_5_S)
#define AXI_IC_REG_AW_QOS_CFG_VLU_5_V  0x0000FFFFU
#define AXI_IC_REG_AW_QOS_CFG_VLU_5_S  0

/** AXI_IC_AR_QOS_CFG_VLU_0_REG register
 *  Config the QOS Priority in AR channel.
 */
#define AXI_IC_AR_QOS_CFG_VLU_0_REG (DR_REG_AXI_IC_BASE + 0x24c)
/** AXI_IC_REG_AR_QOS_CFG_VLU_0 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the ar channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AR_QOS_CFG_VLU_0    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_0_M  (AXI_IC_REG_AR_QOS_CFG_VLU_0_V << AXI_IC_REG_AR_QOS_CFG_VLU_0_S)
#define AXI_IC_REG_AR_QOS_CFG_VLU_0_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_0_S  0

/** AXI_IC_AR_QOS_CFG_VLU_1_REG register
 *  Config the QOS Priority in AR channel.
 */
#define AXI_IC_AR_QOS_CFG_VLU_1_REG (DR_REG_AXI_IC_BASE + 0x250)
/** AXI_IC_REG_AR_QOS_CFG_VLU_1 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the ar channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AR_QOS_CFG_VLU_1    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_1_M  (AXI_IC_REG_AR_QOS_CFG_VLU_1_V << AXI_IC_REG_AR_QOS_CFG_VLU_1_S)
#define AXI_IC_REG_AR_QOS_CFG_VLU_1_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_1_S  0

/** AXI_IC_AR_QOS_CFG_VLU_2_REG register
 *  Config the QOS Priority in AR channel.
 */
#define AXI_IC_AR_QOS_CFG_VLU_2_REG (DR_REG_AXI_IC_BASE + 0x254)
/** AXI_IC_REG_AR_QOS_CFG_VLU_2 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the ar channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AR_QOS_CFG_VLU_2    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_2_M  (AXI_IC_REG_AR_QOS_CFG_VLU_2_V << AXI_IC_REG_AR_QOS_CFG_VLU_2_S)
#define AXI_IC_REG_AR_QOS_CFG_VLU_2_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_2_S  0

/** AXI_IC_AR_QOS_CFG_VLU_3_REG register
 *  Config the QOS Priority in AR channel.
 */
#define AXI_IC_AR_QOS_CFG_VLU_3_REG (DR_REG_AXI_IC_BASE + 0x258)
/** AXI_IC_REG_AR_QOS_CFG_VLU_3 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the ar channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AR_QOS_CFG_VLU_3    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_3_M  (AXI_IC_REG_AR_QOS_CFG_VLU_3_V << AXI_IC_REG_AR_QOS_CFG_VLU_3_S)
#define AXI_IC_REG_AR_QOS_CFG_VLU_3_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_3_S  0

/** AXI_IC_AR_QOS_CFG_VLU_4_REG register
 *  Config the QOS Priority in AR channel.
 */
#define AXI_IC_AR_QOS_CFG_VLU_4_REG (DR_REG_AXI_IC_BASE + 0x25c)
/** AXI_IC_REG_AR_QOS_CFG_VLU_4 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the ar channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AR_QOS_CFG_VLU_4    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_4_M  (AXI_IC_REG_AR_QOS_CFG_VLU_4_V << AXI_IC_REG_AR_QOS_CFG_VLU_4_S)
#define AXI_IC_REG_AR_QOS_CFG_VLU_4_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_4_S  0

/** AXI_IC_AR_QOS_CFG_VLU_5_REG register
 *  Config the QOS Priority in AR channel.
 */
#define AXI_IC_AR_QOS_CFG_VLU_5_REG (DR_REG_AXI_IC_BASE + 0x260)
/** AXI_IC_REG_AR_QOS_CFG_VLU_5 : R/W; bitpos: [15:0]; default: 55906;
 *  the four level of the ar channel qos priority, default is 2,6,10,13
 */
#define AXI_IC_REG_AR_QOS_CFG_VLU_5    0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_5_M  (AXI_IC_REG_AR_QOS_CFG_VLU_5_V << AXI_IC_REG_AR_QOS_CFG_VLU_5_S)
#define AXI_IC_REG_AR_QOS_CFG_VLU_5_V  0x0000FFFFU
#define AXI_IC_REG_AR_QOS_CFG_VLU_5_S  0

/** AXI_IC_AW_QOS_DLY_THR0_0_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR0_0_REG (DR_REG_AXI_IC_BASE + 0x264)
/** AXI_IC_REG_AW_QOS_DLY_THR0_0 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the aw channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR0_0    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_0_M  (AXI_IC_REG_AW_QOS_DLY_THR0_0_V << AXI_IC_REG_AW_QOS_DLY_THR0_0_S)
#define AXI_IC_REG_AW_QOS_DLY_THR0_0_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_0_S  0

/** AXI_IC_AW_QOS_DLY_THR0_1_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR0_1_REG (DR_REG_AXI_IC_BASE + 0x268)
/** AXI_IC_REG_AW_QOS_DLY_THR0_1 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the aw channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR0_1    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_1_M  (AXI_IC_REG_AW_QOS_DLY_THR0_1_V << AXI_IC_REG_AW_QOS_DLY_THR0_1_S)
#define AXI_IC_REG_AW_QOS_DLY_THR0_1_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_1_S  0

/** AXI_IC_AW_QOS_DLY_THR0_2_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR0_2_REG (DR_REG_AXI_IC_BASE + 0x26c)
/** AXI_IC_REG_AW_QOS_DLY_THR0_2 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the aw channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR0_2    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_2_M  (AXI_IC_REG_AW_QOS_DLY_THR0_2_V << AXI_IC_REG_AW_QOS_DLY_THR0_2_S)
#define AXI_IC_REG_AW_QOS_DLY_THR0_2_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_2_S  0

/** AXI_IC_AW_QOS_DLY_THR0_3_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR0_3_REG (DR_REG_AXI_IC_BASE + 0x270)
/** AXI_IC_REG_AW_QOS_DLY_THR0_3 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the aw channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR0_3    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_3_M  (AXI_IC_REG_AW_QOS_DLY_THR0_3_V << AXI_IC_REG_AW_QOS_DLY_THR0_3_S)
#define AXI_IC_REG_AW_QOS_DLY_THR0_3_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_3_S  0

/** AXI_IC_AW_QOS_DLY_THR0_4_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR0_4_REG (DR_REG_AXI_IC_BASE + 0x274)
/** AXI_IC_REG_AW_QOS_DLY_THR0_4 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the aw channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR0_4    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_4_M  (AXI_IC_REG_AW_QOS_DLY_THR0_4_V << AXI_IC_REG_AW_QOS_DLY_THR0_4_S)
#define AXI_IC_REG_AW_QOS_DLY_THR0_4_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_4_S  0

/** AXI_IC_AW_QOS_DLY_THR0_5_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR0_5_REG (DR_REG_AXI_IC_BASE + 0x278)
/** AXI_IC_REG_AW_QOS_DLY_THR0_5 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the aw channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR0_5    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_5_M  (AXI_IC_REG_AW_QOS_DLY_THR0_5_V << AXI_IC_REG_AW_QOS_DLY_THR0_5_S)
#define AXI_IC_REG_AW_QOS_DLY_THR0_5_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR0_5_S  0

/** AXI_IC_AW_QOS_DLY_THR1_0_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR1_0_REG (DR_REG_AXI_IC_BASE + 0x27c)
/** AXI_IC_REG_AW_QOS_DLY_THR1_0 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the aw channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR1_0    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_0_M  (AXI_IC_REG_AW_QOS_DLY_THR1_0_V << AXI_IC_REG_AW_QOS_DLY_THR1_0_S)
#define AXI_IC_REG_AW_QOS_DLY_THR1_0_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_0_S  0

/** AXI_IC_AW_QOS_DLY_THR1_1_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR1_1_REG (DR_REG_AXI_IC_BASE + 0x280)
/** AXI_IC_REG_AW_QOS_DLY_THR1_1 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the aw channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR1_1    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_1_M  (AXI_IC_REG_AW_QOS_DLY_THR1_1_V << AXI_IC_REG_AW_QOS_DLY_THR1_1_S)
#define AXI_IC_REG_AW_QOS_DLY_THR1_1_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_1_S  0

/** AXI_IC_AW_QOS_DLY_THR1_2_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR1_2_REG (DR_REG_AXI_IC_BASE + 0x284)
/** AXI_IC_REG_AW_QOS_DLY_THR1_2 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the aw channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR1_2    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_2_M  (AXI_IC_REG_AW_QOS_DLY_THR1_2_V << AXI_IC_REG_AW_QOS_DLY_THR1_2_S)
#define AXI_IC_REG_AW_QOS_DLY_THR1_2_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_2_S  0

/** AXI_IC_AW_QOS_DLY_THR1_3_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR1_3_REG (DR_REG_AXI_IC_BASE + 0x288)
/** AXI_IC_REG_AW_QOS_DLY_THR1_3 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the aw channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR1_3    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_3_M  (AXI_IC_REG_AW_QOS_DLY_THR1_3_V << AXI_IC_REG_AW_QOS_DLY_THR1_3_S)
#define AXI_IC_REG_AW_QOS_DLY_THR1_3_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_3_S  0

/** AXI_IC_AW_QOS_DLY_THR1_4_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR1_4_REG (DR_REG_AXI_IC_BASE + 0x28c)
/** AXI_IC_REG_AW_QOS_DLY_THR1_4 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the aw channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR1_4    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_4_M  (AXI_IC_REG_AW_QOS_DLY_THR1_4_V << AXI_IC_REG_AW_QOS_DLY_THR1_4_S)
#define AXI_IC_REG_AW_QOS_DLY_THR1_4_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_4_S  0

/** AXI_IC_AW_QOS_DLY_THR1_5_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR1_5_REG (DR_REG_AXI_IC_BASE + 0x290)
/** AXI_IC_REG_AW_QOS_DLY_THR1_5 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the aw channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR1_5    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_5_M  (AXI_IC_REG_AW_QOS_DLY_THR1_5_V << AXI_IC_REG_AW_QOS_DLY_THR1_5_S)
#define AXI_IC_REG_AW_QOS_DLY_THR1_5_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR1_5_S  0

/** AXI_IC_AW_QOS_DLY_THR2_0_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR2_0_REG (DR_REG_AXI_IC_BASE + 0x294)
/** AXI_IC_REG_AW_QOS_DLY_THR2_0 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the aw channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR2_0    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_0_M  (AXI_IC_REG_AW_QOS_DLY_THR2_0_V << AXI_IC_REG_AW_QOS_DLY_THR2_0_S)
#define AXI_IC_REG_AW_QOS_DLY_THR2_0_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_0_S  0

/** AXI_IC_AW_QOS_DLY_THR2_1_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR2_1_REG (DR_REG_AXI_IC_BASE + 0x298)
/** AXI_IC_REG_AW_QOS_DLY_THR2_1 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the aw channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR2_1    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_1_M  (AXI_IC_REG_AW_QOS_DLY_THR2_1_V << AXI_IC_REG_AW_QOS_DLY_THR2_1_S)
#define AXI_IC_REG_AW_QOS_DLY_THR2_1_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_1_S  0

/** AXI_IC_AW_QOS_DLY_THR2_2_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR2_2_REG (DR_REG_AXI_IC_BASE + 0x29c)
/** AXI_IC_REG_AW_QOS_DLY_THR2_2 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the aw channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR2_2    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_2_M  (AXI_IC_REG_AW_QOS_DLY_THR2_2_V << AXI_IC_REG_AW_QOS_DLY_THR2_2_S)
#define AXI_IC_REG_AW_QOS_DLY_THR2_2_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_2_S  0

/** AXI_IC_AW_QOS_DLY_THR2_3_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR2_3_REG (DR_REG_AXI_IC_BASE + 0x2a0)
/** AXI_IC_REG_AW_QOS_DLY_THR2_3 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the aw channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR2_3    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_3_M  (AXI_IC_REG_AW_QOS_DLY_THR2_3_V << AXI_IC_REG_AW_QOS_DLY_THR2_3_S)
#define AXI_IC_REG_AW_QOS_DLY_THR2_3_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_3_S  0

/** AXI_IC_AW_QOS_DLY_THR2_4_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR2_4_REG (DR_REG_AXI_IC_BASE + 0x2a4)
/** AXI_IC_REG_AW_QOS_DLY_THR2_4 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the aw channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR2_4    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_4_M  (AXI_IC_REG_AW_QOS_DLY_THR2_4_V << AXI_IC_REG_AW_QOS_DLY_THR2_4_S)
#define AXI_IC_REG_AW_QOS_DLY_THR2_4_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_4_S  0

/** AXI_IC_AW_QOS_DLY_THR2_5_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR2_5_REG (DR_REG_AXI_IC_BASE + 0x2a8)
/** AXI_IC_REG_AW_QOS_DLY_THR2_5 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the aw channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR2_5    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_5_M  (AXI_IC_REG_AW_QOS_DLY_THR2_5_V << AXI_IC_REG_AW_QOS_DLY_THR2_5_S)
#define AXI_IC_REG_AW_QOS_DLY_THR2_5_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR2_5_S  0

/** AXI_IC_AW_QOS_DLY_THR3_0_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR3_0_REG (DR_REG_AXI_IC_BASE + 0x2ac)
/** AXI_IC_REG_AW_QOS_DLY_THR3_0 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the aw channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR3_0    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_0_M  (AXI_IC_REG_AW_QOS_DLY_THR3_0_V << AXI_IC_REG_AW_QOS_DLY_THR3_0_S)
#define AXI_IC_REG_AW_QOS_DLY_THR3_0_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_0_S  0

/** AXI_IC_AW_QOS_DLY_THR3_1_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR3_1_REG (DR_REG_AXI_IC_BASE + 0x2b0)
/** AXI_IC_REG_AW_QOS_DLY_THR3_1 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the aw channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR3_1    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_1_M  (AXI_IC_REG_AW_QOS_DLY_THR3_1_V << AXI_IC_REG_AW_QOS_DLY_THR3_1_S)
#define AXI_IC_REG_AW_QOS_DLY_THR3_1_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_1_S  0

/** AXI_IC_AW_QOS_DLY_THR3_2_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR3_2_REG (DR_REG_AXI_IC_BASE + 0x2b4)
/** AXI_IC_REG_AW_QOS_DLY_THR3_2 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the aw channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR3_2    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_2_M  (AXI_IC_REG_AW_QOS_DLY_THR3_2_V << AXI_IC_REG_AW_QOS_DLY_THR3_2_S)
#define AXI_IC_REG_AW_QOS_DLY_THR3_2_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_2_S  0

/** AXI_IC_AW_QOS_DLY_THR3_3_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR3_3_REG (DR_REG_AXI_IC_BASE + 0x2b8)
/** AXI_IC_REG_AW_QOS_DLY_THR3_3 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the aw channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR3_3    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_3_M  (AXI_IC_REG_AW_QOS_DLY_THR3_3_V << AXI_IC_REG_AW_QOS_DLY_THR3_3_S)
#define AXI_IC_REG_AW_QOS_DLY_THR3_3_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_3_S  0

/** AXI_IC_AW_QOS_DLY_THR3_4_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR3_4_REG (DR_REG_AXI_IC_BASE + 0x2bc)
/** AXI_IC_REG_AW_QOS_DLY_THR3_4 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the aw channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR3_4    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_4_M  (AXI_IC_REG_AW_QOS_DLY_THR3_4_V << AXI_IC_REG_AW_QOS_DLY_THR3_4_S)
#define AXI_IC_REG_AW_QOS_DLY_THR3_4_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_4_S  0

/** AXI_IC_AW_QOS_DLY_THR3_5_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AW_QOS_DLY_THR3_5_REG (DR_REG_AXI_IC_BASE + 0x2c0)
/** AXI_IC_REG_AW_QOS_DLY_THR3_5 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the aw channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AW_QOS_DLY_THR3_5    0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_5_M  (AXI_IC_REG_AW_QOS_DLY_THR3_5_V << AXI_IC_REG_AW_QOS_DLY_THR3_5_S)
#define AXI_IC_REG_AW_QOS_DLY_THR3_5_V  0x00001FFFU
#define AXI_IC_REG_AW_QOS_DLY_THR3_5_S  0

/** AXI_IC_AR_QOS_DLY_THR0_0_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR0_0_REG (DR_REG_AXI_IC_BASE + 0x2c4)
/** AXI_IC_REG_AR_QOS_DLY_THR0_0 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the ar channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR0_0    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_0_M  (AXI_IC_REG_AR_QOS_DLY_THR0_0_V << AXI_IC_REG_AR_QOS_DLY_THR0_0_S)
#define AXI_IC_REG_AR_QOS_DLY_THR0_0_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_0_S  0

/** AXI_IC_AR_QOS_DLY_THR0_1_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR0_1_REG (DR_REG_AXI_IC_BASE + 0x2c8)
/** AXI_IC_REG_AR_QOS_DLY_THR0_1 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the ar channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR0_1    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_1_M  (AXI_IC_REG_AR_QOS_DLY_THR0_1_V << AXI_IC_REG_AR_QOS_DLY_THR0_1_S)
#define AXI_IC_REG_AR_QOS_DLY_THR0_1_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_1_S  0

/** AXI_IC_AR_QOS_DLY_THR0_2_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR0_2_REG (DR_REG_AXI_IC_BASE + 0x2cc)
/** AXI_IC_REG_AR_QOS_DLY_THR0_2 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the ar channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR0_2    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_2_M  (AXI_IC_REG_AR_QOS_DLY_THR0_2_V << AXI_IC_REG_AR_QOS_DLY_THR0_2_S)
#define AXI_IC_REG_AR_QOS_DLY_THR0_2_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_2_S  0

/** AXI_IC_AR_QOS_DLY_THR0_3_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR0_3_REG (DR_REG_AXI_IC_BASE + 0x2d0)
/** AXI_IC_REG_AR_QOS_DLY_THR0_3 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the ar channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR0_3    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_3_M  (AXI_IC_REG_AR_QOS_DLY_THR0_3_V << AXI_IC_REG_AR_QOS_DLY_THR0_3_S)
#define AXI_IC_REG_AR_QOS_DLY_THR0_3_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_3_S  0

/** AXI_IC_AR_QOS_DLY_THR0_4_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR0_4_REG (DR_REG_AXI_IC_BASE + 0x2d4)
/** AXI_IC_REG_AR_QOS_DLY_THR0_4 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the ar channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR0_4    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_4_M  (AXI_IC_REG_AR_QOS_DLY_THR0_4_V << AXI_IC_REG_AR_QOS_DLY_THR0_4_S)
#define AXI_IC_REG_AR_QOS_DLY_THR0_4_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_4_S  0

/** AXI_IC_AR_QOS_DLY_THR0_5_REG register
 *  Config the first level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR0_5_REG (DR_REG_AXI_IC_BASE + 0x2d8)
/** AXI_IC_REG_AR_QOS_DLY_THR0_5 : R/W; bitpos: [12:0]; default: 32;
 *  the first level of the ar channel delay threshold, default is 32.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR0_5    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_5_M  (AXI_IC_REG_AR_QOS_DLY_THR0_5_V << AXI_IC_REG_AR_QOS_DLY_THR0_5_S)
#define AXI_IC_REG_AR_QOS_DLY_THR0_5_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR0_5_S  0

/** AXI_IC_AR_QOS_DLY_THR1_0_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR1_0_REG (DR_REG_AXI_IC_BASE + 0x2dc)
/** AXI_IC_REG_AR_QOS_DLY_THR1_0 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the ar channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR1_0    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_0_M  (AXI_IC_REG_AR_QOS_DLY_THR1_0_V << AXI_IC_REG_AR_QOS_DLY_THR1_0_S)
#define AXI_IC_REG_AR_QOS_DLY_THR1_0_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_0_S  0

/** AXI_IC_AR_QOS_DLY_THR1_1_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR1_1_REG (DR_REG_AXI_IC_BASE + 0x2e0)
/** AXI_IC_REG_AR_QOS_DLY_THR1_1 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the ar channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR1_1    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_1_M  (AXI_IC_REG_AR_QOS_DLY_THR1_1_V << AXI_IC_REG_AR_QOS_DLY_THR1_1_S)
#define AXI_IC_REG_AR_QOS_DLY_THR1_1_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_1_S  0

/** AXI_IC_AR_QOS_DLY_THR1_2_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR1_2_REG (DR_REG_AXI_IC_BASE + 0x2e4)
/** AXI_IC_REG_AR_QOS_DLY_THR1_2 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the ar channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR1_2    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_2_M  (AXI_IC_REG_AR_QOS_DLY_THR1_2_V << AXI_IC_REG_AR_QOS_DLY_THR1_2_S)
#define AXI_IC_REG_AR_QOS_DLY_THR1_2_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_2_S  0

/** AXI_IC_AR_QOS_DLY_THR1_3_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR1_3_REG (DR_REG_AXI_IC_BASE + 0x2e8)
/** AXI_IC_REG_AR_QOS_DLY_THR1_3 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the ar channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR1_3    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_3_M  (AXI_IC_REG_AR_QOS_DLY_THR1_3_V << AXI_IC_REG_AR_QOS_DLY_THR1_3_S)
#define AXI_IC_REG_AR_QOS_DLY_THR1_3_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_3_S  0

/** AXI_IC_AR_QOS_DLY_THR1_4_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR1_4_REG (DR_REG_AXI_IC_BASE + 0x2ec)
/** AXI_IC_REG_AR_QOS_DLY_THR1_4 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the ar channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR1_4    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_4_M  (AXI_IC_REG_AR_QOS_DLY_THR1_4_V << AXI_IC_REG_AR_QOS_DLY_THR1_4_S)
#define AXI_IC_REG_AR_QOS_DLY_THR1_4_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_4_S  0

/** AXI_IC_AR_QOS_DLY_THR1_5_REG register
 *  Config the second level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR1_5_REG (DR_REG_AXI_IC_BASE + 0x2f0)
/** AXI_IC_REG_AR_QOS_DLY_THR1_5 : R/W; bitpos: [12:0]; default: 160;
 *  the second level of the ar channel delay threshold, default is 160.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR1_5    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_5_M  (AXI_IC_REG_AR_QOS_DLY_THR1_5_V << AXI_IC_REG_AR_QOS_DLY_THR1_5_S)
#define AXI_IC_REG_AR_QOS_DLY_THR1_5_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR1_5_S  0

/** AXI_IC_AR_QOS_DLY_THR2_0_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR2_0_REG (DR_REG_AXI_IC_BASE + 0x2f4)
/** AXI_IC_REG_AR_QOS_DLY_THR2_0 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the ar channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR2_0    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_0_M  (AXI_IC_REG_AR_QOS_DLY_THR2_0_V << AXI_IC_REG_AR_QOS_DLY_THR2_0_S)
#define AXI_IC_REG_AR_QOS_DLY_THR2_0_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_0_S  0

/** AXI_IC_AR_QOS_DLY_THR2_1_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR2_1_REG (DR_REG_AXI_IC_BASE + 0x2f8)
/** AXI_IC_REG_AR_QOS_DLY_THR2_1 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the ar channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR2_1    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_1_M  (AXI_IC_REG_AR_QOS_DLY_THR2_1_V << AXI_IC_REG_AR_QOS_DLY_THR2_1_S)
#define AXI_IC_REG_AR_QOS_DLY_THR2_1_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_1_S  0

/** AXI_IC_AR_QOS_DLY_THR2_2_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR2_2_REG (DR_REG_AXI_IC_BASE + 0x2fc)
/** AXI_IC_REG_AR_QOS_DLY_THR2_2 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the ar channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR2_2    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_2_M  (AXI_IC_REG_AR_QOS_DLY_THR2_2_V << AXI_IC_REG_AR_QOS_DLY_THR2_2_S)
#define AXI_IC_REG_AR_QOS_DLY_THR2_2_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_2_S  0

/** AXI_IC_AR_QOS_DLY_THR2_3_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR2_3_REG (DR_REG_AXI_IC_BASE + 0x300)
/** AXI_IC_REG_AR_QOS_DLY_THR2_3 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the ar channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR2_3    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_3_M  (AXI_IC_REG_AR_QOS_DLY_THR2_3_V << AXI_IC_REG_AR_QOS_DLY_THR2_3_S)
#define AXI_IC_REG_AR_QOS_DLY_THR2_3_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_3_S  0

/** AXI_IC_AR_QOS_DLY_THR2_4_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR2_4_REG (DR_REG_AXI_IC_BASE + 0x304)
/** AXI_IC_REG_AR_QOS_DLY_THR2_4 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the ar channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR2_4    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_4_M  (AXI_IC_REG_AR_QOS_DLY_THR2_4_V << AXI_IC_REG_AR_QOS_DLY_THR2_4_S)
#define AXI_IC_REG_AR_QOS_DLY_THR2_4_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_4_S  0

/** AXI_IC_AR_QOS_DLY_THR2_5_REG register
 *  Config the third level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR2_5_REG (DR_REG_AXI_IC_BASE + 0x308)
/** AXI_IC_REG_AR_QOS_DLY_THR2_5 : R/W; bitpos: [12:0]; default: 416;
 *  the third level of the ar channel delay threshold, default is 416.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR2_5    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_5_M  (AXI_IC_REG_AR_QOS_DLY_THR2_5_V << AXI_IC_REG_AR_QOS_DLY_THR2_5_S)
#define AXI_IC_REG_AR_QOS_DLY_THR2_5_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR2_5_S  0

/** AXI_IC_AR_QOS_DLY_THR3_0_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR3_0_REG (DR_REG_AXI_IC_BASE + 0x30c)
/** AXI_IC_REG_AR_QOS_DLY_THR3_0 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the ar channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR3_0    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_0_M  (AXI_IC_REG_AR_QOS_DLY_THR3_0_V << AXI_IC_REG_AR_QOS_DLY_THR3_0_S)
#define AXI_IC_REG_AR_QOS_DLY_THR3_0_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_0_S  0

/** AXI_IC_AR_QOS_DLY_THR3_1_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR3_1_REG (DR_REG_AXI_IC_BASE + 0x310)
/** AXI_IC_REG_AR_QOS_DLY_THR3_1 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the ar channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR3_1    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_1_M  (AXI_IC_REG_AR_QOS_DLY_THR3_1_V << AXI_IC_REG_AR_QOS_DLY_THR3_1_S)
#define AXI_IC_REG_AR_QOS_DLY_THR3_1_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_1_S  0

/** AXI_IC_AR_QOS_DLY_THR3_2_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR3_2_REG (DR_REG_AXI_IC_BASE + 0x314)
/** AXI_IC_REG_AR_QOS_DLY_THR3_2 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the ar channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR3_2    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_2_M  (AXI_IC_REG_AR_QOS_DLY_THR3_2_V << AXI_IC_REG_AR_QOS_DLY_THR3_2_S)
#define AXI_IC_REG_AR_QOS_DLY_THR3_2_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_2_S  0

/** AXI_IC_AR_QOS_DLY_THR3_3_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR3_3_REG (DR_REG_AXI_IC_BASE + 0x318)
/** AXI_IC_REG_AR_QOS_DLY_THR3_3 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the ar channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR3_3    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_3_M  (AXI_IC_REG_AR_QOS_DLY_THR3_3_V << AXI_IC_REG_AR_QOS_DLY_THR3_3_S)
#define AXI_IC_REG_AR_QOS_DLY_THR3_3_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_3_S  0

/** AXI_IC_AR_QOS_DLY_THR3_4_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR3_4_REG (DR_REG_AXI_IC_BASE + 0x31c)
/** AXI_IC_REG_AR_QOS_DLY_THR3_4 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the ar channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR3_4    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_4_M  (AXI_IC_REG_AR_QOS_DLY_THR3_4_V << AXI_IC_REG_AR_QOS_DLY_THR3_4_S)
#define AXI_IC_REG_AR_QOS_DLY_THR3_4_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_4_S  0

/** AXI_IC_AR_QOS_DLY_THR3_5_REG register
 *  Config the forth level delay threshold for first priority in AW channel.
 */
#define AXI_IC_AR_QOS_DLY_THR3_5_REG (DR_REG_AXI_IC_BASE + 0x320)
/** AXI_IC_REG_AR_QOS_DLY_THR3_5 : R/W; bitpos: [12:0]; default: 928;
 *  the forth level of the ar channel delay threshold, default is 928.
 */
#define AXI_IC_REG_AR_QOS_DLY_THR3_5    0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_5_M  (AXI_IC_REG_AR_QOS_DLY_THR3_5_V << AXI_IC_REG_AR_QOS_DLY_THR3_5_S)
#define AXI_IC_REG_AR_QOS_DLY_THR3_5_V  0x00001FFFU
#define AXI_IC_REG_AR_QOS_DLY_THR3_5_S  0

/** AXI_IC_AW_QOS_BUR_LEN_THR_0_REG register
 *  Config the min value of token which can enable axi write transaction.
 */
#define AXI_IC_AW_QOS_BUR_LEN_THR_0_REG (DR_REG_AXI_IC_BASE + 0x324)
/** AXI_IC_REG_AW_QOS_BUR_LEN_THR_0 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of aw channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_0    0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_0_M  (AXI_IC_REG_AW_QOS_BUR_LEN_THR_0_V << AXI_IC_REG_AW_QOS_BUR_LEN_THR_0_S)
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_0_V  0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_0_S  0

/** AXI_IC_AW_QOS_BUR_LEN_THR_1_REG register
 *  Config the min value of token which can enable axi write transaction.
 */
#define AXI_IC_AW_QOS_BUR_LEN_THR_1_REG (DR_REG_AXI_IC_BASE + 0x328)
/** AXI_IC_REG_AW_QOS_BUR_LEN_THR_1 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of aw channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_1    0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_1_M  (AXI_IC_REG_AW_QOS_BUR_LEN_THR_1_V << AXI_IC_REG_AW_QOS_BUR_LEN_THR_1_S)
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_1_V  0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_1_S  0

/** AXI_IC_AW_QOS_BUR_LEN_THR_2_REG register
 *  Config the min value of token which can enable axi write transaction.
 */
#define AXI_IC_AW_QOS_BUR_LEN_THR_2_REG (DR_REG_AXI_IC_BASE + 0x32c)
/** AXI_IC_REG_AW_QOS_BUR_LEN_THR_2 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of aw channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_2    0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_2_M  (AXI_IC_REG_AW_QOS_BUR_LEN_THR_2_V << AXI_IC_REG_AW_QOS_BUR_LEN_THR_2_S)
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_2_V  0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_2_S  0

/** AXI_IC_AW_QOS_BUR_LEN_THR_3_REG register
 *  Config the min value of token which can enable axi write transaction.
 */
#define AXI_IC_AW_QOS_BUR_LEN_THR_3_REG (DR_REG_AXI_IC_BASE + 0x330)
/** AXI_IC_REG_AW_QOS_BUR_LEN_THR_3 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of aw channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_3    0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_3_M  (AXI_IC_REG_AW_QOS_BUR_LEN_THR_3_V << AXI_IC_REG_AW_QOS_BUR_LEN_THR_3_S)
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_3_V  0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_3_S  0

/** AXI_IC_AW_QOS_BUR_LEN_THR_4_REG register
 *  Config the min value of token which can enable axi write transaction.
 */
#define AXI_IC_AW_QOS_BUR_LEN_THR_4_REG (DR_REG_AXI_IC_BASE + 0x334)
/** AXI_IC_REG_AW_QOS_BUR_LEN_THR_4 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of aw channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_4    0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_4_M  (AXI_IC_REG_AW_QOS_BUR_LEN_THR_4_V << AXI_IC_REG_AW_QOS_BUR_LEN_THR_4_S)
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_4_V  0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_4_S  0

/** AXI_IC_AW_QOS_BUR_LEN_THR_5_REG register
 *  Config the min value of token which can enable axi write transaction.
 */
#define AXI_IC_AW_QOS_BUR_LEN_THR_5_REG (DR_REG_AXI_IC_BASE + 0x338)
/** AXI_IC_REG_AW_QOS_BUR_LEN_THR_5 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of aw channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_5    0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_5_M  (AXI_IC_REG_AW_QOS_BUR_LEN_THR_5_V << AXI_IC_REG_AW_QOS_BUR_LEN_THR_5_S)
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_5_V  0x000003FFU
#define AXI_IC_REG_AW_QOS_BUR_LEN_THR_5_S  0

/** AXI_IC_AR_QOS_BUR_LEN_THR_0_REG register
 *  Config the min value of token which can enable axi read transaction.
 */
#define AXI_IC_AR_QOS_BUR_LEN_THR_0_REG (DR_REG_AXI_IC_BASE + 0x33c)
/** AXI_IC_REG_AR_QOS_BUR_LEN_THR_0 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of ar channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_0    0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_0_M  (AXI_IC_REG_AR_QOS_BUR_LEN_THR_0_V << AXI_IC_REG_AR_QOS_BUR_LEN_THR_0_S)
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_0_V  0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_0_S  0

/** AXI_IC_AR_QOS_BUR_LEN_THR_1_REG register
 *  Config the min value of token which can enable axi read transaction.
 */
#define AXI_IC_AR_QOS_BUR_LEN_THR_1_REG (DR_REG_AXI_IC_BASE + 0x340)
/** AXI_IC_REG_AR_QOS_BUR_LEN_THR_1 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of ar channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_1    0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_1_M  (AXI_IC_REG_AR_QOS_BUR_LEN_THR_1_V << AXI_IC_REG_AR_QOS_BUR_LEN_THR_1_S)
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_1_V  0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_1_S  0

/** AXI_IC_AR_QOS_BUR_LEN_THR_2_REG register
 *  Config the min value of token which can enable axi read transaction.
 */
#define AXI_IC_AR_QOS_BUR_LEN_THR_2_REG (DR_REG_AXI_IC_BASE + 0x344)
/** AXI_IC_REG_AR_QOS_BUR_LEN_THR_2 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of ar channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_2    0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_2_M  (AXI_IC_REG_AR_QOS_BUR_LEN_THR_2_V << AXI_IC_REG_AR_QOS_BUR_LEN_THR_2_S)
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_2_V  0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_2_S  0

/** AXI_IC_AR_QOS_BUR_LEN_THR_3_REG register
 *  Config the min value of token which can enable axi read transaction.
 */
#define AXI_IC_AR_QOS_BUR_LEN_THR_3_REG (DR_REG_AXI_IC_BASE + 0x348)
/** AXI_IC_REG_AR_QOS_BUR_LEN_THR_3 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of ar channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_3    0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_3_M  (AXI_IC_REG_AR_QOS_BUR_LEN_THR_3_V << AXI_IC_REG_AR_QOS_BUR_LEN_THR_3_S)
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_3_V  0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_3_S  0

/** AXI_IC_AR_QOS_BUR_LEN_THR_4_REG register
 *  Config the min value of token which can enable axi read transaction.
 */
#define AXI_IC_AR_QOS_BUR_LEN_THR_4_REG (DR_REG_AXI_IC_BASE + 0x34c)
/** AXI_IC_REG_AR_QOS_BUR_LEN_THR_4 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of ar channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_4    0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_4_M  (AXI_IC_REG_AR_QOS_BUR_LEN_THR_4_V << AXI_IC_REG_AR_QOS_BUR_LEN_THR_4_S)
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_4_V  0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_4_S  0

/** AXI_IC_AR_QOS_BUR_LEN_THR_5_REG register
 *  Config the min value of token which can enable axi read transaction.
 */
#define AXI_IC_AR_QOS_BUR_LEN_THR_5_REG (DR_REG_AXI_IC_BASE + 0x350)
/** AXI_IC_REG_AR_QOS_BUR_LEN_THR_5 : R/W; bitpos: [9:0]; default: 64;
 *  limit the starting token value of ar channel to turn on transmission, default value
 *  is 64, 0 means no limit.
 */
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_5    0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_5_M  (AXI_IC_REG_AR_QOS_BUR_LEN_THR_5_V << AXI_IC_REG_AR_QOS_BUR_LEN_THR_5_S)
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_5_V  0x000003FFU
#define AXI_IC_REG_AR_QOS_BUR_LEN_THR_5_S  0

/** AXI_IC_AW_QOS_DBG_TK_THR_0_REG register
 *  Config the debug smple interval in aw channel.
 */
#define AXI_IC_AW_QOS_DBG_TK_THR_0_REG (DR_REG_AXI_IC_BASE + 0x354)
/** AXI_IC_REG_AW_QOS_DBG_TK_THR_0 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_0    0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_0_M  (AXI_IC_REG_AW_QOS_DBG_TK_THR_0_V << AXI_IC_REG_AW_QOS_DBG_TK_THR_0_S)
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_0_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_0_S  0

/** AXI_IC_AW_QOS_DBG_TK_THR_1_REG register
 *  Config the debug smple interval in aw channel.
 */
#define AXI_IC_AW_QOS_DBG_TK_THR_1_REG (DR_REG_AXI_IC_BASE + 0x358)
/** AXI_IC_REG_AW_QOS_DBG_TK_THR_1 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_1    0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_1_M  (AXI_IC_REG_AW_QOS_DBG_TK_THR_1_V << AXI_IC_REG_AW_QOS_DBG_TK_THR_1_S)
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_1_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_1_S  0

/** AXI_IC_AW_QOS_DBG_TK_THR_2_REG register
 *  Config the debug smple interval in aw channel.
 */
#define AXI_IC_AW_QOS_DBG_TK_THR_2_REG (DR_REG_AXI_IC_BASE + 0x35c)
/** AXI_IC_REG_AW_QOS_DBG_TK_THR_2 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_2    0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_2_M  (AXI_IC_REG_AW_QOS_DBG_TK_THR_2_V << AXI_IC_REG_AW_QOS_DBG_TK_THR_2_S)
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_2_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_2_S  0

/** AXI_IC_AW_QOS_DBG_TK_THR_3_REG register
 *  Config the debug smple interval in aw channel.
 */
#define AXI_IC_AW_QOS_DBG_TK_THR_3_REG (DR_REG_AXI_IC_BASE + 0x360)
/** AXI_IC_REG_AW_QOS_DBG_TK_THR_3 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_3    0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_3_M  (AXI_IC_REG_AW_QOS_DBG_TK_THR_3_V << AXI_IC_REG_AW_QOS_DBG_TK_THR_3_S)
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_3_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_3_S  0

/** AXI_IC_AW_QOS_DBG_TK_THR_4_REG register
 *  Config the debug smple interval in aw channel.
 */
#define AXI_IC_AW_QOS_DBG_TK_THR_4_REG (DR_REG_AXI_IC_BASE + 0x364)
/** AXI_IC_REG_AW_QOS_DBG_TK_THR_4 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_4    0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_4_M  (AXI_IC_REG_AW_QOS_DBG_TK_THR_4_V << AXI_IC_REG_AW_QOS_DBG_TK_THR_4_S)
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_4_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_4_S  0

/** AXI_IC_AW_QOS_DBG_TK_THR_5_REG register
 *  Config the debug smple interval in aw channel.
 */
#define AXI_IC_AW_QOS_DBG_TK_THR_5_REG (DR_REG_AXI_IC_BASE + 0x368)
/** AXI_IC_REG_AW_QOS_DBG_TK_THR_5 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_5    0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_5_M  (AXI_IC_REG_AW_QOS_DBG_TK_THR_5_V << AXI_IC_REG_AW_QOS_DBG_TK_THR_5_S)
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_5_V  0x000000FFU
#define AXI_IC_REG_AW_QOS_DBG_TK_THR_5_S  0

/** AXI_IC_AR_QOS_DBG_TK_THR_0_REG register
 *  Config the debug smple interval in ar channel.
 */
#define AXI_IC_AR_QOS_DBG_TK_THR_0_REG (DR_REG_AXI_IC_BASE + 0x36c)
/** AXI_IC_REG_AR_QOS_DBG_TK_THR_0 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_0    0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_0_M  (AXI_IC_REG_AR_QOS_DBG_TK_THR_0_V << AXI_IC_REG_AR_QOS_DBG_TK_THR_0_S)
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_0_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_0_S  0

/** AXI_IC_AR_QOS_DBG_TK_THR_1_REG register
 *  Config the debug smple interval in ar channel.
 */
#define AXI_IC_AR_QOS_DBG_TK_THR_1_REG (DR_REG_AXI_IC_BASE + 0x370)
/** AXI_IC_REG_AR_QOS_DBG_TK_THR_1 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_1    0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_1_M  (AXI_IC_REG_AR_QOS_DBG_TK_THR_1_V << AXI_IC_REG_AR_QOS_DBG_TK_THR_1_S)
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_1_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_1_S  0

/** AXI_IC_AR_QOS_DBG_TK_THR_2_REG register
 *  Config the debug smple interval in ar channel.
 */
#define AXI_IC_AR_QOS_DBG_TK_THR_2_REG (DR_REG_AXI_IC_BASE + 0x374)
/** AXI_IC_REG_AR_QOS_DBG_TK_THR_2 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_2    0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_2_M  (AXI_IC_REG_AR_QOS_DBG_TK_THR_2_V << AXI_IC_REG_AR_QOS_DBG_TK_THR_2_S)
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_2_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_2_S  0

/** AXI_IC_AR_QOS_DBG_TK_THR_3_REG register
 *  Config the debug smple interval in ar channel.
 */
#define AXI_IC_AR_QOS_DBG_TK_THR_3_REG (DR_REG_AXI_IC_BASE + 0x378)
/** AXI_IC_REG_AR_QOS_DBG_TK_THR_3 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_3    0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_3_M  (AXI_IC_REG_AR_QOS_DBG_TK_THR_3_V << AXI_IC_REG_AR_QOS_DBG_TK_THR_3_S)
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_3_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_3_S  0

/** AXI_IC_AR_QOS_DBG_TK_THR_4_REG register
 *  Config the debug smple interval in ar channel.
 */
#define AXI_IC_AR_QOS_DBG_TK_THR_4_REG (DR_REG_AXI_IC_BASE + 0x37c)
/** AXI_IC_REG_AR_QOS_DBG_TK_THR_4 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_4    0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_4_M  (AXI_IC_REG_AR_QOS_DBG_TK_THR_4_V << AXI_IC_REG_AR_QOS_DBG_TK_THR_4_S)
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_4_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_4_S  0

/** AXI_IC_AR_QOS_DBG_TK_THR_5_REG register
 *  Config the debug smple interval in ar channel.
 */
#define AXI_IC_AR_QOS_DBG_TK_THR_5_REG (DR_REG_AXI_IC_BASE + 0x380)
/** AXI_IC_REG_AR_QOS_DBG_TK_THR_5 : R/W; bitpos: [7:0]; default: 16;
 *  limit the debug sample interval, default value is 16
 */
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_5    0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_5_M  (AXI_IC_REG_AR_QOS_DBG_TK_THR_5_V << AXI_IC_REG_AR_QOS_DBG_TK_THR_5_S)
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_5_V  0x000000FFU
#define AXI_IC_REG_AR_QOS_DBG_TK_THR_5_S  0

/** AXI_IC_MAX_OST_AW_S_0_REG register
 *  Config the max outstanding num in current slave write port.
 */
#define AXI_IC_MAX_OST_AW_S_0_REG (DR_REG_AXI_IC_BASE + 0x384)
/** AXI_IC_REG_MAX_OST_AW_S_0 : R/W; bitpos: [5:0]; default: 15;
 *  limit the number of outstanding for slave port, default value is 15, max is 32.
 */
#define AXI_IC_REG_MAX_OST_AW_S_0    0x0000003FU
#define AXI_IC_REG_MAX_OST_AW_S_0_M  (AXI_IC_REG_MAX_OST_AW_S_0_V << AXI_IC_REG_MAX_OST_AW_S_0_S)
#define AXI_IC_REG_MAX_OST_AW_S_0_V  0x0000003FU
#define AXI_IC_REG_MAX_OST_AW_S_0_S  0

/** AXI_IC_MAX_OST_AW_S_1_REG register
 *  Config the max outstanding num in current slave write port.
 */
#define AXI_IC_MAX_OST_AW_S_1_REG (DR_REG_AXI_IC_BASE + 0x388)
/** AXI_IC_REG_MAX_OST_AW_S_1 : R/W; bitpos: [5:0]; default: 15;
 *  limit the number of outstanding for slave port, default value is 15, max is 32.
 */
#define AXI_IC_REG_MAX_OST_AW_S_1    0x0000003FU
#define AXI_IC_REG_MAX_OST_AW_S_1_M  (AXI_IC_REG_MAX_OST_AW_S_1_V << AXI_IC_REG_MAX_OST_AW_S_1_S)
#define AXI_IC_REG_MAX_OST_AW_S_1_V  0x0000003FU
#define AXI_IC_REG_MAX_OST_AW_S_1_S  0

/** AXI_IC_MAX_OST_AW_S_2_REG register
 *  Config the max outstanding num in current slave write port.
 */
#define AXI_IC_MAX_OST_AW_S_2_REG (DR_REG_AXI_IC_BASE + 0x38c)
/** AXI_IC_REG_MAX_OST_AW_S_2 : R/W; bitpos: [5:0]; default: 15;
 *  limit the number of outstanding for slave port, default value is 15, max is 32.
 */
#define AXI_IC_REG_MAX_OST_AW_S_2    0x0000003FU
#define AXI_IC_REG_MAX_OST_AW_S_2_M  (AXI_IC_REG_MAX_OST_AW_S_2_V << AXI_IC_REG_MAX_OST_AW_S_2_S)
#define AXI_IC_REG_MAX_OST_AW_S_2_V  0x0000003FU
#define AXI_IC_REG_MAX_OST_AW_S_2_S  0

/** AXI_IC_MAX_OST_AR_S_0_REG register
 *  Config the max outstanding num in current slave read port.
 */
#define AXI_IC_MAX_OST_AR_S_0_REG (DR_REG_AXI_IC_BASE + 0x390)
/** AXI_IC_REG_MAX_OST_AR_S_0 : R/W; bitpos: [7:0]; default: 15;
 *  limit the number of outstanding for slave port, default value is 15
 */
#define AXI_IC_REG_MAX_OST_AR_S_0    0x000000FFU
#define AXI_IC_REG_MAX_OST_AR_S_0_M  (AXI_IC_REG_MAX_OST_AR_S_0_V << AXI_IC_REG_MAX_OST_AR_S_0_S)
#define AXI_IC_REG_MAX_OST_AR_S_0_V  0x000000FFU
#define AXI_IC_REG_MAX_OST_AR_S_0_S  0

/** AXI_IC_MAX_OST_AR_S_1_REG register
 *  Config the max outstanding num in current slave read port.
 */
#define AXI_IC_MAX_OST_AR_S_1_REG (DR_REG_AXI_IC_BASE + 0x394)
/** AXI_IC_REG_MAX_OST_AR_S_1 : R/W; bitpos: [7:0]; default: 15;
 *  limit the number of outstanding for slave port, default value is 15
 */
#define AXI_IC_REG_MAX_OST_AR_S_1    0x000000FFU
#define AXI_IC_REG_MAX_OST_AR_S_1_M  (AXI_IC_REG_MAX_OST_AR_S_1_V << AXI_IC_REG_MAX_OST_AR_S_1_S)
#define AXI_IC_REG_MAX_OST_AR_S_1_V  0x000000FFU
#define AXI_IC_REG_MAX_OST_AR_S_1_S  0

/** AXI_IC_MAX_OST_AR_S_2_REG register
 *  Config the max outstanding num in current slave read port.
 */
#define AXI_IC_MAX_OST_AR_S_2_REG (DR_REG_AXI_IC_BASE + 0x398)
/** AXI_IC_REG_MAX_OST_AR_S_2 : R/W; bitpos: [7:0]; default: 15;
 *  limit the number of outstanding for slave port, default value is 15
 */
#define AXI_IC_REG_MAX_OST_AR_S_2    0x000000FFU
#define AXI_IC_REG_MAX_OST_AR_S_2_M  (AXI_IC_REG_MAX_OST_AR_S_2_V << AXI_IC_REG_MAX_OST_AR_S_2_S)
#define AXI_IC_REG_MAX_OST_AR_S_2_V  0x000000FFU
#define AXI_IC_REG_MAX_OST_AR_S_2_S  0

/** AXI_IC_AW_QOS_PEAK_RATE_MODE_REG register
 *  Config the aw channel token store mode in peak rate enable.
 */
#define AXI_IC_AW_QOS_PEAK_RATE_MODE_REG (DR_REG_AXI_IC_BASE + 0x39c)
/** AXI_IC_REG_AW_QOS_PEAK_RATE_MODE : R/W; bitpos: [5:0]; default: 0;
 *  peak rate, to control the aw channel request token num more than 1, default 0.
 */
#define AXI_IC_REG_AW_QOS_PEAK_RATE_MODE    0x0000003FU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_MODE_M  (AXI_IC_REG_AW_QOS_PEAK_RATE_MODE_V << AXI_IC_REG_AW_QOS_PEAK_RATE_MODE_S)
#define AXI_IC_REG_AW_QOS_PEAK_RATE_MODE_V  0x0000003FU
#define AXI_IC_REG_AW_QOS_PEAK_RATE_MODE_S  0

/** AXI_IC_AR_QOS_PEAK_RATE_MODE_REG register
 *  Config the ar channel token store mode in peak rate enable.
 */
#define AXI_IC_AR_QOS_PEAK_RATE_MODE_REG (DR_REG_AXI_IC_BASE + 0x3a0)
/** AXI_IC_REG_AR_QOS_PEAK_RATE_MODE : R/W; bitpos: [5:0]; default: 0;
 *  peak rate, to control the ar channel request token num  more than 1, default 0.
 */
#define AXI_IC_REG_AR_QOS_PEAK_RATE_MODE    0x0000003FU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_MODE_M  (AXI_IC_REG_AR_QOS_PEAK_RATE_MODE_V << AXI_IC_REG_AR_QOS_PEAK_RATE_MODE_S)
#define AXI_IC_REG_AR_QOS_PEAK_RATE_MODE_V  0x0000003FU
#define AXI_IC_REG_AR_QOS_PEAK_RATE_MODE_S  0

#ifdef __cplusplus
}
#endif
