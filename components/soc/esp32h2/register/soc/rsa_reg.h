/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** RSA_M_MEM register
 *  Represents M
 */
#define RSA_M_MEM_REG (DR_REG_RSA_BASE + 0x0)
#define RSA_M_MEM_SIZE_BYTES 16

/** RSA_Z_MEM register
 *  Represents Z
 */
#define RSA_Z_MEM_REG (DR_REG_RSA_BASE + 0x200)
#define RSA_Z_MEM_SIZE_BYTES 16

/** RSA_Y_MEM register
 *  Represents Y
 */
#define RSA_Y_MEM_REG (DR_REG_RSA_BASE + 0x400)
#define RSA_Y_MEM_SIZE_BYTES 16

/** RSA_X_MEM register
 *  Represents X
 */
#define RSA_X_MEM_REG (DR_REG_RSA_BASE + 0x600)
#define RSA_X_MEM_SIZE_BYTES 16

/** RSA_M_PRIME_REG register
 *  Represents M’
 */
#define RSA_M_PRIME_REG (DR_REG_RSA_BASE + 0x800)
/** RSA_M_PRIME : R/W; bitpos: [31:0]; default: 0;
 *  Represents M’
 */
#define RSA_M_PRIME    0xFFFFFFFFU
#define RSA_M_PRIME_M  (RSA_M_PRIME_V << RSA_M_PRIME_S)
#define RSA_M_PRIME_V  0xFFFFFFFFU
#define RSA_M_PRIME_S  0

/** RSA_MODE_REG register
 *  Configures RSA length
 */
#define RSA_MODE_REG (DR_REG_RSA_BASE + 0x804)
/** RSA_MODE : R/W; bitpos: [6:0]; default: 0;
 *  Configures the RSA length.
 */
#define RSA_MODE    0x0000007FU
#define RSA_MODE_M  (RSA_MODE_V << RSA_MODE_S)
#define RSA_MODE_V  0x0000007FU
#define RSA_MODE_S  0

/** RSA_QUERY_CLEAN_REG register
 *  RSA clean register
 */
#define RSA_QUERY_CLEAN_REG (DR_REG_RSA_BASE + 0x808)
/** RSA_QUERY_CLEAN : RO; bitpos: [0]; default: 0;
 *  Represents whether or not the RSA memory completes initialization.
 *
 *  0: Not complete
 *
 *  1: Completed
 *
 */
#define RSA_QUERY_CLEAN    (BIT(0))
#define RSA_QUERY_CLEAN_M  (RSA_QUERY_CLEAN_V << RSA_QUERY_CLEAN_S)
#define RSA_QUERY_CLEAN_V  0x00000001U
#define RSA_QUERY_CLEAN_S  0

/** RSA_SET_START_MODEXP_REG register
 *  Starts modular exponentiation
 */
#define RSA_SET_START_MODEXP_REG (DR_REG_RSA_BASE + 0x80c)
/** RSA_SET_START_MODEXP : WT; bitpos: [0]; default: 0;
 *  Configure whether or not to start the modular exponentiation.
 *
 *  0: No effect
 *
 *  1: Start
 *
 */
#define RSA_SET_START_MODEXP    (BIT(0))
#define RSA_SET_START_MODEXP_M  (RSA_SET_START_MODEXP_V << RSA_SET_START_MODEXP_S)
#define RSA_SET_START_MODEXP_V  0x00000001U
#define RSA_SET_START_MODEXP_S  0

/** RSA_SET_START_MODMULT_REG register
 *  Starts modular multiplication
 */
#define RSA_SET_START_MODMULT_REG (DR_REG_RSA_BASE + 0x810)
/** RSA_SET_START_MODMULT : WT; bitpos: [0]; default: 0;
 *  Configure whether or not to start the modular multiplication.
 *
 *  0: No effect
 *
 *  1: Start
 *
 */
#define RSA_SET_START_MODMULT    (BIT(0))
#define RSA_SET_START_MODMULT_M  (RSA_SET_START_MODMULT_V << RSA_SET_START_MODMULT_S)
#define RSA_SET_START_MODMULT_V  0x00000001U
#define RSA_SET_START_MODMULT_S  0

/** RSA_SET_START_MULT_REG register
 *  Starts multiplication
 */
#define RSA_SET_START_MULT_REG (DR_REG_RSA_BASE + 0x814)
/** RSA_SET_START_MULT : WT; bitpos: [0]; default: 0;
 *  Configure whether or not to start the multiplication.
 *
 *  0: No effect
 *
 *  1: Start
 *
 */
#define RSA_SET_START_MULT    (BIT(0))
#define RSA_SET_START_MULT_M  (RSA_SET_START_MULT_V << RSA_SET_START_MULT_S)
#define RSA_SET_START_MULT_V  0x00000001U
#define RSA_SET_START_MULT_S  0

/** RSA_QUERY_IDLE_REG register
 *  Represents the RSA status
 */
#define RSA_QUERY_IDLE_REG (DR_REG_RSA_BASE + 0x818)
/** RSA_QUERY_IDLE : RO; bitpos: [0]; default: 0;
 *  Represents the RSA status.
 *
 *  0: Busy
 *
 *  1: Idle
 *
 */
#define RSA_QUERY_IDLE    (BIT(0))
#define RSA_QUERY_IDLE_M  (RSA_QUERY_IDLE_V << RSA_QUERY_IDLE_S)
#define RSA_QUERY_IDLE_V  0x00000001U
#define RSA_QUERY_IDLE_S  0

/** RSA_INT_CLR_REG register
 *  Clears RSA interrupt
 */
#define RSA_INT_CLR_REG (DR_REG_RSA_BASE + 0x81c)
/** RSA_CLEAR_INTERRUPT : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear the RSA interrupt.
 */
#define RSA_CLEAR_INTERRUPT    (BIT(0))
#define RSA_CLEAR_INTERRUPT_M  (RSA_CLEAR_INTERRUPT_V << RSA_CLEAR_INTERRUPT_S)
#define RSA_CLEAR_INTERRUPT_V  0x00000001U
#define RSA_CLEAR_INTERRUPT_S  0

/** RSA_CONSTANT_TIME_REG register
 *  Configures the constant_time option
 */
#define RSA_CONSTANT_TIME_REG (DR_REG_RSA_BASE + 0x820)
/** RSA_CONSTANT_TIME : R/W; bitpos: [0]; default: 1;
 *  Configures the constant_time option.
 *
 *  0: Acceleration
 *
 *  1: No acceleration (default)
 *
 */
#define RSA_CONSTANT_TIME    (BIT(0))
#define RSA_CONSTANT_TIME_M  (RSA_CONSTANT_TIME_V << RSA_CONSTANT_TIME_S)
#define RSA_CONSTANT_TIME_V  0x00000001U
#define RSA_CONSTANT_TIME_S  0

/** RSA_SEARCH_ENABLE_REG register
 *  Configures the search option
 */
#define RSA_SEARCH_ENABLE_REG (DR_REG_RSA_BASE + 0x824)
/** RSA_SEARCH_ENABLE : R/W; bitpos: [0]; default: 0;
 *  Configure the search option.
 *
 *  0: No acceleration (default)
 *
 *  1: Acceleration
 *
 *  This option should be used together with RSA_SEARCH_POS.
 */
#define RSA_SEARCH_ENABLE    (BIT(0))
#define RSA_SEARCH_ENABLE_M  (RSA_SEARCH_ENABLE_V << RSA_SEARCH_ENABLE_S)
#define RSA_SEARCH_ENABLE_V  0x00000001U
#define RSA_SEARCH_ENABLE_S  0

/** RSA_SEARCH_POS_REG register
 *  Configures the search position
 */
#define RSA_SEARCH_POS_REG (DR_REG_RSA_BASE + 0x828)
/** RSA_SEARCH_POS : R/W; bitpos: [11:0]; default: 0;
 *  Configures the starting address to start search. This field should be used together
 *  with RSA_SEARCH_ENABLE. The field is only valid when RSA_SEARCH_ENABLE is high.
 */
#define RSA_SEARCH_POS    0x00000FFFU
#define RSA_SEARCH_POS_M  (RSA_SEARCH_POS_V << RSA_SEARCH_POS_S)
#define RSA_SEARCH_POS_V  0x00000FFFU
#define RSA_SEARCH_POS_S  0

/** RSA_INT_ENA_REG register
 *  Enables the RSA interrupt
 */
#define RSA_INT_ENA_REG (DR_REG_RSA_BASE + 0x82c)
/** RSA_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable the RSA interrupt.
 */
#define RSA_INT_ENA    (BIT(0))
#define RSA_INT_ENA_M  (RSA_INT_ENA_V << RSA_INT_ENA_S)
#define RSA_INT_ENA_V  0x00000001U
#define RSA_INT_ENA_S  0

/** RSA_DATE_REG register
 *  Version control register
 */
#define RSA_DATE_REG (DR_REG_RSA_BASE + 0x830)
/** RSA_DATE : R/W; bitpos: [29:0]; default: 538969624;
 *  Version control register.
 */
#define RSA_DATE    0x3FFFFFFFU
#define RSA_DATE_M  (RSA_DATE_V << RSA_DATE_S)
#define RSA_DATE_V  0x3FFFFFFFU
#define RSA_DATE_S  0

#ifdef __cplusplus
}
#endif
