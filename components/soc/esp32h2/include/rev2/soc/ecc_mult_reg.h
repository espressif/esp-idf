/**
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** ECC_MULT_INT_RAW_REG register
 *  Add later.
 */
#define ECC_MULT_INT_RAW_REG (DR_REG_ECC_MULT_BASE + 0xc)
/** ECC_MULT_CALC_DONE_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  Add later.
 */
#define ECC_MULT_CALC_DONE_INT_RAW    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_RAW_M  (ECC_MULT_CALC_DONE_INT_RAW_V << ECC_MULT_CALC_DONE_INT_RAW_S)
#define ECC_MULT_CALC_DONE_INT_RAW_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_RAW_S  0

/** ECC_MULT_INT_ST_REG register
 *  Add later.
 */
#define ECC_MULT_INT_ST_REG (DR_REG_ECC_MULT_BASE + 0x10)
/** ECC_MULT_CALC_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  Add later.
 */
#define ECC_MULT_CALC_DONE_INT_ST    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ST_M  (ECC_MULT_CALC_DONE_INT_ST_V << ECC_MULT_CALC_DONE_INT_ST_S)
#define ECC_MULT_CALC_DONE_INT_ST_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_ST_S  0

/** ECC_MULT_INT_ENA_REG register
 *  Add later.
 */
#define ECC_MULT_INT_ENA_REG (DR_REG_ECC_MULT_BASE + 0x14)
/** ECC_MULT_CALC_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Add later.
 */
#define ECC_MULT_CALC_DONE_INT_ENA    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ENA_M  (ECC_MULT_CALC_DONE_INT_ENA_V << ECC_MULT_CALC_DONE_INT_ENA_S)
#define ECC_MULT_CALC_DONE_INT_ENA_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_ENA_S  0

/** ECC_MULT_INT_CLR_REG register
 *  Add later.
 */
#define ECC_MULT_INT_CLR_REG (DR_REG_ECC_MULT_BASE + 0x18)
/** ECC_MULT_CALC_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Add later.
 */
#define ECC_MULT_CALC_DONE_INT_CLR    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_CLR_M  (ECC_MULT_CALC_DONE_INT_CLR_V << ECC_MULT_CALC_DONE_INT_CLR_S)
#define ECC_MULT_CALC_DONE_INT_CLR_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_CLR_S  0

/** ECC_MULT_CONF_REG register
 *  Add later.
 */
#define ECC_MULT_CONF_REG (DR_REG_ECC_MULT_BASE + 0x1c)
/** ECC_MULT_START : R/W/SC; bitpos: [0]; default: 0;
 *  Add later.
 */
#define ECC_MULT_START    (BIT(0))
#define ECC_MULT_START_M  (ECC_MULT_START_V << ECC_MULT_START_S)
#define ECC_MULT_START_V  0x00000001U
#define ECC_MULT_START_S  0
/** ECC_MULT_RESET : WT; bitpos: [1]; default: 0;
 *  Add later.
 */
#define ECC_MULT_RESET    (BIT(1))
#define ECC_MULT_RESET_M  (ECC_MULT_RESET_V << ECC_MULT_RESET_S)
#define ECC_MULT_RESET_V  0x00000001U
#define ECC_MULT_RESET_S  1
/** ECC_MULT_KEY_LENGTH : R/W; bitpos: [2]; default: 0;
 *  Add later.
 */
#define ECC_MULT_KEY_LENGTH    (BIT(2))
#define ECC_MULT_KEY_LENGTH_M  (ECC_MULT_KEY_LENGTH_V << ECC_MULT_KEY_LENGTH_S)
#define ECC_MULT_KEY_LENGTH_V  0x00000001U
#define ECC_MULT_KEY_LENGTH_S  2
/** ECC_MULT_SECURITY_MODE : R/W; bitpos: [3]; default: 0;
 *  Add later.
 */
#define ECC_MULT_SECURITY_MODE    (BIT(3))
#define ECC_MULT_SECURITY_MODE_M  (ECC_MULT_SECURITY_MODE_V << ECC_MULT_SECURITY_MODE_S)
#define ECC_MULT_SECURITY_MODE_V  0x00000001U
#define ECC_MULT_SECURITY_MODE_S  3
/** ECC_MULT_CLK_EN : R/W; bitpos: [4]; default: 0;
 *  Add later.
 */
#define ECC_MULT_CLK_EN    (BIT(4))
#define ECC_MULT_CLK_EN_M  (ECC_MULT_CLK_EN_V << ECC_MULT_CLK_EN_S)
#define ECC_MULT_CLK_EN_V  0x00000001U
#define ECC_MULT_CLK_EN_S  4
/** ECC_MULT_WORK_MODE : R/W; bitpos: [6:5]; default: 0;
 *  Add later.
 */
#define ECC_MULT_WORK_MODE    0x00000003U
#define ECC_MULT_WORK_MODE_M  (ECC_MULT_WORK_MODE_V << ECC_MULT_WORK_MODE_S)
#define ECC_MULT_WORK_MODE_V  0x00000003U
#define ECC_MULT_WORK_MODE_S  5
/** ECC_MULT_VERIFICATION_RESULT : RO/SS; bitpos: [7]; default: 0;
 *  Add later.
 */
#define ECC_MULT_VERIFICATION_RESULT    (BIT(7))
#define ECC_MULT_VERIFICATION_RESULT_M  (ECC_MULT_VERIFICATION_RESULT_V << ECC_MULT_VERIFICATION_RESULT_S)
#define ECC_MULT_VERIFICATION_RESULT_V  0x00000001U
#define ECC_MULT_VERIFICATION_RESULT_S  7

/** ECC_MULT_DATE_REG register
 *  Add later.
 */
#define ECC_MULT_DATE_REG (DR_REG_ECC_MULT_BASE + 0xfc)
/** ECC_MULT_DATE : R/W; bitpos: [27:0]; default: 33628720;
 *  ECC mult version control register
 */
#define ECC_MULT_DATE    0x0FFFFFFFU
#define ECC_MULT_DATE_M  (ECC_MULT_DATE_V << ECC_MULT_DATE_S)
#define ECC_MULT_DATE_V  0x0FFFFFFFU
#define ECC_MULT_DATE_S  0

#ifdef __cplusplus
}
#endif
