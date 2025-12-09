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

/** TRNG_CONF_REG register
 *  CFG reg
 */
#define TRNG_CONF_REG (DR_REG_TRNG_BASE + 0x0)
/** TRNG_NOISE_SOURCE_SEL : R/W; bitpos: [4:0]; default: 0;
 *  sel noise source
 */
#define TRNG_NOISE_SOURCE_SEL    0x0000001FU
#define TRNG_NOISE_SOURCE_SEL_M  (TRNG_NOISE_SOURCE_SEL_V << TRNG_NOISE_SOURCE_SEL_S)
#define TRNG_NOISE_SOURCE_SEL_V  0x0000001FU
#define TRNG_NOISE_SOURCE_SEL_S  0
/** TRNG_NOISE_POS_SEL : R/W; bitpos: [9:5]; default: 0;
 *  sel noise pos
 */
#define TRNG_NOISE_POS_SEL    0x0000001FU
#define TRNG_NOISE_POS_SEL_M  (TRNG_NOISE_POS_SEL_V << TRNG_NOISE_POS_SEL_S)
#define TRNG_NOISE_POS_SEL_V  0x0000001FU
#define TRNG_NOISE_POS_SEL_S  5
/** TRNG_REPETITION_VALUE_C : R/W; bitpos: [18:10]; default: 0;
 *  parameter
 */
#define TRNG_REPETITION_VALUE_C    0x000001FFU
#define TRNG_REPETITION_VALUE_C_M  (TRNG_REPETITION_VALUE_C_V << TRNG_REPETITION_VALUE_C_S)
#define TRNG_REPETITION_VALUE_C_V  0x000001FFU
#define TRNG_REPETITION_VALUE_C_S  10
/** TRNG_ADPATIVE_VALUE_C : R/W; bitpos: [27:19]; default: 0;
 *  parameter
 */
#define TRNG_ADPATIVE_VALUE_C    0x000001FFU
#define TRNG_ADPATIVE_VALUE_C_M  (TRNG_ADPATIVE_VALUE_C_V << TRNG_ADPATIVE_VALUE_C_S)
#define TRNG_ADPATIVE_VALUE_C_V  0x000001FFU
#define TRNG_ADPATIVE_VALUE_C_S  19
/** TRNG_FIFO_RESET : WT; bitpos: [28]; default: 0;
 *  fifo_reset
 */
#define TRNG_FIFO_RESET    (BIT(28))
#define TRNG_FIFO_RESET_M  (TRNG_FIFO_RESET_V << TRNG_FIFO_RESET_S)
#define TRNG_FIFO_RESET_V  0x00000001U
#define TRNG_FIFO_RESET_S  28
/** TRNG_RANDOM_OUTPUT_MODE : R/W; bitpos: [29]; default: 0;
 *  standard and nonstandart output
 */
#define TRNG_RANDOM_OUTPUT_MODE    (BIT(29))
#define TRNG_RANDOM_OUTPUT_MODE_M  (TRNG_RANDOM_OUTPUT_MODE_V << TRNG_RANDOM_OUTPUT_MODE_S)
#define TRNG_RANDOM_OUTPUT_MODE_V  0x00000001U
#define TRNG_RANDOM_OUTPUT_MODE_S  29
/** TRNG_NOISE_CRC_EN : R/W; bitpos: [30]; default: 1;
 *  standard and nonstandart output
 */
#define TRNG_NOISE_CRC_EN    (BIT(30))
#define TRNG_NOISE_CRC_EN_M  (TRNG_NOISE_CRC_EN_V << TRNG_NOISE_CRC_EN_S)
#define TRNG_NOISE_CRC_EN_V  0x00000001U
#define TRNG_NOISE_CRC_EN_S  30
/** TRNG_SAMPLE_ENABLE : R/W; bitpos: [31]; default: 0;
 *  1: enable rng sample loop
 */
#define TRNG_SAMPLE_ENABLE    (BIT(31))
#define TRNG_SAMPLE_ENABLE_M  (TRNG_SAMPLE_ENABLE_V << TRNG_SAMPLE_ENABLE_S)
#define TRNG_SAMPLE_ENABLE_V  0x00000001U
#define TRNG_SAMPLE_ENABLE_S  31

/** TRNG_DEBUG_CONF_REG register
 *  reserved
 */
#define TRNG_DEBUG_CONF_REG (DR_REG_TRNG_BASE + 0x4)
/** TRNG_STARTUP_TEST_START : WT; bitpos: [0]; default: 0;
 *  startup start
 */
#define TRNG_STARTUP_TEST_START    (BIT(0))
#define TRNG_STARTUP_TEST_START_M  (TRNG_STARTUP_TEST_START_V << TRNG_STARTUP_TEST_START_S)
#define TRNG_STARTUP_TEST_START_V  0x00000001U
#define TRNG_STARTUP_TEST_START_S  0
/** TRNG_STARTUP_TEST_LIMIT : R/W; bitpos: [16:1]; default: 1024;
 *  startup limit
 */
#define TRNG_STARTUP_TEST_LIMIT    0x0000FFFFU
#define TRNG_STARTUP_TEST_LIMIT_M  (TRNG_STARTUP_TEST_LIMIT_V << TRNG_STARTUP_TEST_LIMIT_S)
#define TRNG_STARTUP_TEST_LIMIT_V  0x0000FFFFU
#define TRNG_STARTUP_TEST_LIMIT_S  1
/** TRNG_DEBUG_DATA : RO; bitpos: [24:17]; default: 0;
 *  debug data
 */
#define TRNG_DEBUG_DATA    0x000000FFU
#define TRNG_DEBUG_DATA_M  (TRNG_DEBUG_DATA_V << TRNG_DEBUG_DATA_S)
#define TRNG_DEBUG_DATA_V  0x000000FFU
#define TRNG_DEBUG_DATA_S  17
/** TRNG_DEBUG_MODE : R/W; bitpos: [25]; default: 0;
 *  1 for debug mode 0 for nomarl mode
 */
#define TRNG_DEBUG_MODE    (BIT(25))
#define TRNG_DEBUG_MODE_M  (TRNG_DEBUG_MODE_V << TRNG_DEBUG_MODE_S)
#define TRNG_DEBUG_MODE_V  0x00000001U
#define TRNG_DEBUG_MODE_S  25
/** TRNG_HEALTH_TEST_ERROR_CODE : RO; bitpos: [28:26]; default: 0;
 *  error code
 */
#define TRNG_HEALTH_TEST_ERROR_CODE    0x00000007U
#define TRNG_HEALTH_TEST_ERROR_CODE_M  (TRNG_HEALTH_TEST_ERROR_CODE_V << TRNG_HEALTH_TEST_ERROR_CODE_S)
#define TRNG_HEALTH_TEST_ERROR_CODE_V  0x00000007U
#define TRNG_HEALTH_TEST_ERROR_CODE_S  26
/** TRNG_HEALTH_TEST_END : WT; bitpos: [29]; default: 0;
 *  health test end
 */
#define TRNG_HEALTH_TEST_END    (BIT(29))
#define TRNG_HEALTH_TEST_END_M  (TRNG_HEALTH_TEST_END_V << TRNG_HEALTH_TEST_END_S)
#define TRNG_HEALTH_TEST_END_V  0x00000001U
#define TRNG_HEALTH_TEST_END_S  29
/** TRNG_HEALTH_TEST_BYPASS : R/W; bitpos: [30]; default: 0;
 *  health test bypass
 */
#define TRNG_HEALTH_TEST_BYPASS    (BIT(30))
#define TRNG_HEALTH_TEST_BYPASS_M  (TRNG_HEALTH_TEST_BYPASS_V << TRNG_HEALTH_TEST_BYPASS_S)
#define TRNG_HEALTH_TEST_BYPASS_V  0x00000001U
#define TRNG_HEALTH_TEST_BYPASS_S  30

/** TRNG_CONDITION_KEY0_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY0_REG (DR_REG_TRNG_BASE + 0x8)
/** TRNG_CONDITION_KEY0 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY0    0xFFFFFFFFU
#define TRNG_CONDITION_KEY0_M  (TRNG_CONDITION_KEY0_V << TRNG_CONDITION_KEY0_S)
#define TRNG_CONDITION_KEY0_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY0_S  0

/** TRNG_CONDITION_KEY1_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY1_REG (DR_REG_TRNG_BASE + 0xc)
/** TRNG_CONDITION_KEY1 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY1    0xFFFFFFFFU
#define TRNG_CONDITION_KEY1_M  (TRNG_CONDITION_KEY1_V << TRNG_CONDITION_KEY1_S)
#define TRNG_CONDITION_KEY1_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY1_S  0

/** TRNG_CONDITION_KEY2_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY2_REG (DR_REG_TRNG_BASE + 0x10)
/** TRNG_CONDITION_KEY2 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY2    0xFFFFFFFFU
#define TRNG_CONDITION_KEY2_M  (TRNG_CONDITION_KEY2_V << TRNG_CONDITION_KEY2_S)
#define TRNG_CONDITION_KEY2_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY2_S  0

/** TRNG_CONDITION_KEY3_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY3_REG (DR_REG_TRNG_BASE + 0x14)
/** TRNG_CONDITION_KEY3 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY3    0xFFFFFFFFU
#define TRNG_CONDITION_KEY3_M  (TRNG_CONDITION_KEY3_V << TRNG_CONDITION_KEY3_S)
#define TRNG_CONDITION_KEY3_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY3_S  0

/** TRNG_CONDITION_KEY4_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY4_REG (DR_REG_TRNG_BASE + 0x18)
/** TRNG_CONDITION_KEY4 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY4    0xFFFFFFFFU
#define TRNG_CONDITION_KEY4_M  (TRNG_CONDITION_KEY4_V << TRNG_CONDITION_KEY4_S)
#define TRNG_CONDITION_KEY4_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY4_S  0

/** TRNG_CONDITION_KEY5_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY5_REG (DR_REG_TRNG_BASE + 0x1c)
/** TRNG_CONDITION_KEY5 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY5    0xFFFFFFFFU
#define TRNG_CONDITION_KEY5_M  (TRNG_CONDITION_KEY5_V << TRNG_CONDITION_KEY5_S)
#define TRNG_CONDITION_KEY5_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY5_S  0

/** TRNG_CONDITION_KEY6_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY6_REG (DR_REG_TRNG_BASE + 0x20)
/** TRNG_CONDITION_KEY6 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY6    0xFFFFFFFFU
#define TRNG_CONDITION_KEY6_M  (TRNG_CONDITION_KEY6_V << TRNG_CONDITION_KEY6_S)
#define TRNG_CONDITION_KEY6_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY6_S  0

/** TRNG_CONDITION_KEY7_REG register
 *  condition key reg
 */
#define TRNG_CONDITION_KEY7_REG (DR_REG_TRNG_BASE + 0x24)
/** TRNG_CONDITION_KEY7 : R/W; bitpos: [31:0]; default: 0;
 *  condition key
 */
#define TRNG_CONDITION_KEY7    0xFFFFFFFFU
#define TRNG_CONDITION_KEY7_M  (TRNG_CONDITION_KEY7_V << TRNG_CONDITION_KEY7_S)
#define TRNG_CONDITION_KEY7_V  0xFFFFFFFFU
#define TRNG_CONDITION_KEY7_S  0

/** TRNG_INT_ENA_REG register
 *  Read only register for error and done
 */
#define TRNG_INT_ENA_REG (DR_REG_TRNG_BASE + 0x28)
/** TRNG_ERROR_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  error flag
 */
#define TRNG_ERROR_INT_ENA    (BIT(0))
#define TRNG_ERROR_INT_ENA_M  (TRNG_ERROR_INT_ENA_V << TRNG_ERROR_INT_ENA_S)
#define TRNG_ERROR_INT_ENA_V  0x00000001U
#define TRNG_ERROR_INT_ENA_S  0

/** TRNG_INT_RAW_REG register
 *  Read only register for error and done
 */
#define TRNG_INT_RAW_REG (DR_REG_TRNG_BASE + 0x2c)
/** TRNG_ERROR_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  error flag
 */
#define TRNG_ERROR_INT_RAW    (BIT(0))
#define TRNG_ERROR_INT_RAW_M  (TRNG_ERROR_INT_RAW_V << TRNG_ERROR_INT_RAW_S)
#define TRNG_ERROR_INT_RAW_V  0x00000001U
#define TRNG_ERROR_INT_RAW_S  0

/** TRNG_INT_CLR_REG register
 *  Read only register for error and done
 */
#define TRNG_INT_CLR_REG (DR_REG_TRNG_BASE + 0x30)
/** TRNG_ERROR_INT_CLR : WT; bitpos: [0]; default: 0;
 *  error flag
 */
#define TRNG_ERROR_INT_CLR    (BIT(0))
#define TRNG_ERROR_INT_CLR_M  (TRNG_ERROR_INT_CLR_V << TRNG_ERROR_INT_CLR_S)
#define TRNG_ERROR_INT_CLR_V  0x00000001U
#define TRNG_ERROR_INT_CLR_S  0

/** TRNG_INT_ST_REG register
 *  Read only register for error and done
 */
#define TRNG_INT_ST_REG (DR_REG_TRNG_BASE + 0x34)
/** TRNG_ERROR_INT_ST : RO; bitpos: [0]; default: 0;
 *  error flag
 */
#define TRNG_ERROR_INT_ST    (BIT(0))
#define TRNG_ERROR_INT_ST_M  (TRNG_ERROR_INT_ST_V << TRNG_ERROR_INT_ST_S)
#define TRNG_ERROR_INT_ST_V  0x00000001U
#define TRNG_ERROR_INT_ST_S  0

/** TRNG_ARBIT_CFG_REG register
 *  arbit cfg
 */
#define TRNG_ARBIT_CFG_REG (DR_REG_TRNG_BASE + 0x38)
/** TRNG_ARBIT_PRIORITY_0 : R/W; bitpos: [3:0]; default: 0;
 *  reg_arbit_priority_0
 */
#define TRNG_ARBIT_PRIORITY_0    0x0000000FU
#define TRNG_ARBIT_PRIORITY_0_M  (TRNG_ARBIT_PRIORITY_0_V << TRNG_ARBIT_PRIORITY_0_S)
#define TRNG_ARBIT_PRIORITY_0_V  0x0000000FU
#define TRNG_ARBIT_PRIORITY_0_S  0
/** TRNG_ARBIT_PRIORITY_1 : R/W; bitpos: [7:4]; default: 0;
 *  reg_arbit_priority_1
 */
#define TRNG_ARBIT_PRIORITY_1    0x0000000FU
#define TRNG_ARBIT_PRIORITY_1_M  (TRNG_ARBIT_PRIORITY_1_V << TRNG_ARBIT_PRIORITY_1_S)
#define TRNG_ARBIT_PRIORITY_1_V  0x0000000FU
#define TRNG_ARBIT_PRIORITY_1_S  4
/** TRNG_ARBIT_PRIORITY_2 : R/W; bitpos: [11:8]; default: 0;
 *  reg_arbit_priority_2
 */
#define TRNG_ARBIT_PRIORITY_2    0x0000000FU
#define TRNG_ARBIT_PRIORITY_2_M  (TRNG_ARBIT_PRIORITY_2_V << TRNG_ARBIT_PRIORITY_2_S)
#define TRNG_ARBIT_PRIORITY_2_V  0x0000000FU
#define TRNG_ARBIT_PRIORITY_2_S  8
/** TRNG_ARBIT_PRIORITY_3 : R/W; bitpos: [15:12]; default: 0;
 *  reg_arbit_priority_3
 */
#define TRNG_ARBIT_PRIORITY_3    0x0000000FU
#define TRNG_ARBIT_PRIORITY_3_M  (TRNG_ARBIT_PRIORITY_3_V << TRNG_ARBIT_PRIORITY_3_S)
#define TRNG_ARBIT_PRIORITY_3_V  0x0000000FU
#define TRNG_ARBIT_PRIORITY_3_S  12
/** TRNG_ARBIT_EN : R/W; bitpos: [16]; default: 0;
 *  reg_arbit_en
 */
#define TRNG_ARBIT_EN    (BIT(16))
#define TRNG_ARBIT_EN_M  (TRNG_ARBIT_EN_V << TRNG_ARBIT_EN_S)
#define TRNG_ARBIT_EN_V  0x00000001U
#define TRNG_ARBIT_EN_S  16

/** TRNG_SW_READ_REG register
 *  sw read random reg
 */
#define TRNG_SW_READ_REG (DR_REG_TRNG_BASE + 0x3c)
/** TRNG_SW_RANDOM_DATA : RO; bitpos: [31:0]; default: 0;
 *  sw read random reg
 */
#define TRNG_SW_RANDOM_DATA    0xFFFFFFFFU
#define TRNG_SW_RANDOM_DATA_M  (TRNG_SW_RANDOM_DATA_V << TRNG_SW_RANDOM_DATA_S)
#define TRNG_SW_RANDOM_DATA_V  0xFFFFFFFFU
#define TRNG_SW_RANDOM_DATA_S  0

/** TRNG_SW_READ_ST_REG register
 *  sw read st reg
 */
#define TRNG_SW_READ_ST_REG (DR_REG_TRNG_BASE + 0x40)
/** TRNG_SW_DATA_VALID : RO; bitpos: [0]; default: 0;
 *  sw random data st
 */
#define TRNG_SW_DATA_VALID    (BIT(0))
#define TRNG_SW_DATA_VALID_M  (TRNG_SW_DATA_VALID_V << TRNG_SW_DATA_VALID_S)
#define TRNG_SW_DATA_VALID_V  0x00000001U
#define TRNG_SW_DATA_VALID_S  0
/** TRNG_SW_RANDOM_REQ : WT; bitpos: [1]; default: 0;
 *  sw read random req
 */
#define TRNG_SW_RANDOM_REQ    (BIT(1))
#define TRNG_SW_RANDOM_REQ_M  (TRNG_SW_RANDOM_REQ_V << TRNG_SW_RANDOM_REQ_S)
#define TRNG_SW_RANDOM_REQ_V  0x00000001U
#define TRNG_SW_RANDOM_REQ_S  1

/** TRNG_CRC_DATA_REG register
 *  sw read data
 */
#define TRNG_CRC_DATA_REG (DR_REG_TRNG_BASE + 0x44)
/** TRNG_SW_CRC_RANDOM_DATA : RO; bitpos: [31:0]; default: 0;
 *  sw read crc random data
 */
#define TRNG_SW_CRC_RANDOM_DATA    0xFFFFFFFFU
#define TRNG_SW_CRC_RANDOM_DATA_M  (TRNG_SW_CRC_RANDOM_DATA_V << TRNG_SW_CRC_RANDOM_DATA_S)
#define TRNG_SW_CRC_RANDOM_DATA_V  0xFFFFFFFFU
#define TRNG_SW_CRC_RANDOM_DATA_S  0

/** TRNG_CRC_SYNC_DATA_REG register
 *  sw read data sync
 */
#define TRNG_CRC_SYNC_DATA_REG (DR_REG_TRNG_BASE + 0x48)
/** TRNG_SW_CRC_RANDOM_DATA_SYNC : RO; bitpos: [31:0]; default: 0;
 *  sw read crc random sync data
 */
#define TRNG_SW_CRC_RANDOM_DATA_SYNC    0xFFFFFFFFU
#define TRNG_SW_CRC_RANDOM_DATA_SYNC_M  (TRNG_SW_CRC_RANDOM_DATA_SYNC_V << TRNG_SW_CRC_RANDOM_DATA_SYNC_S)
#define TRNG_SW_CRC_RANDOM_DATA_SYNC_V  0xFFFFFFFFU
#define TRNG_SW_CRC_RANDOM_DATA_SYNC_S  0

/** TRNG_DATE_REG register
 *  Date register.
 */
#define TRNG_DATE_REG (DR_REG_TRNG_BASE + 0xfc)
/** TRNG_DATE : R/W; bitpos: [27:0]; default: 2426672;
 *  TRNG date information
 */
#define TRNG_DATE    0x0FFFFFFFU
#define TRNG_DATE_M  (TRNG_DATE_V << TRNG_DATE_S)
#define TRNG_DATE_V  0x0FFFFFFFU
#define TRNG_DATE_S  0
/** TRNG_CLK_EN : R/W; bitpos: [28]; default: 0;
 *  clock enable
 */
#define TRNG_CLK_EN    (BIT(28))
#define TRNG_CLK_EN_M  (TRNG_CLK_EN_V << TRNG_CLK_EN_S)
#define TRNG_CLK_EN_V  0x00000001U
#define TRNG_CLK_EN_S  28

#ifdef __cplusplus
}
#endif
