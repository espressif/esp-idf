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

/** LP_ADI_VER_DATE_REG register
 *  need_des
 */
#define LP_ADI_VER_DATE_REG (DR_REG_LP_ADI_BASE + 0x0)
/** LP_ADI_VER_DATE : R/W; bitpos: [30:0]; default: 2426628;
 *  need_des
 */
#define LP_ADI_VER_DATE    0x7FFFFFFFU
#define LP_ADI_VER_DATE_M  (LP_ADI_VER_DATE_V << LP_ADI_VER_DATE_S)
#define LP_ADI_VER_DATE_V  0x7FFFFFFFU
#define LP_ADI_VER_DATE_S  0
/** LP_ADI_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define LP_ADI_CLK_EN    (BIT(31))
#define LP_ADI_CLK_EN_M  (LP_ADI_CLK_EN_V << LP_ADI_CLK_EN_S)
#define LP_ADI_CLK_EN_V  0x00000001U
#define LP_ADI_CLK_EN_S  31

/** LP_ADI_DCDC_TOP_2_STS_REG register
 *  ANALOG DCDC 2 related status
 */
#define LP_ADI_DCDC_TOP_2_STS_REG (DR_REG_LP_ADI_BASE + 0x4)
/** LP_ADI_DCDC_TOP_2_CAL_DONE : RO; bitpos: [0]; default: 0;
 *  cali done signal from dcdc 2 top
 */
#define LP_ADI_DCDC_TOP_2_CAL_DONE    (BIT(0))
#define LP_ADI_DCDC_TOP_2_CAL_DONE_M  (LP_ADI_DCDC_TOP_2_CAL_DONE_V << LP_ADI_DCDC_TOP_2_CAL_DONE_S)
#define LP_ADI_DCDC_TOP_2_CAL_DONE_V  0x00000001U
#define LP_ADI_DCDC_TOP_2_CAL_DONE_S  0
/** LP_ADI_DCDC_TOP_2_IN_VCM : RO; bitpos: [1]; default: 0;
 *  in vcm signal from dcdc 2 top
 */
#define LP_ADI_DCDC_TOP_2_IN_VCM    (BIT(1))
#define LP_ADI_DCDC_TOP_2_IN_VCM_M  (LP_ADI_DCDC_TOP_2_IN_VCM_V << LP_ADI_DCDC_TOP_2_IN_VCM_S)
#define LP_ADI_DCDC_TOP_2_IN_VCM_V  0x00000001U
#define LP_ADI_DCDC_TOP_2_IN_VCM_S  1
/** LP_ADI_DCDC_TOP_2_IN_CCM : RO; bitpos: [2]; default: 0;
 *  in ccm signal from dcdc 2 top
 */
#define LP_ADI_DCDC_TOP_2_IN_CCM    (BIT(2))
#define LP_ADI_DCDC_TOP_2_IN_CCM_M  (LP_ADI_DCDC_TOP_2_IN_CCM_V << LP_ADI_DCDC_TOP_2_IN_CCM_S)
#define LP_ADI_DCDC_TOP_2_IN_CCM_V  0x00000001U
#define LP_ADI_DCDC_TOP_2_IN_CCM_S  2
/** LP_ADI_DCDC_TOP_2_VCM_READY : RO; bitpos: [3]; default: 0;
 *  vcm_ready signal from dcdc 2 top
 */
#define LP_ADI_DCDC_TOP_2_VCM_READY    (BIT(3))
#define LP_ADI_DCDC_TOP_2_VCM_READY_M  (LP_ADI_DCDC_TOP_2_VCM_READY_V << LP_ADI_DCDC_TOP_2_VCM_READY_S)
#define LP_ADI_DCDC_TOP_2_VCM_READY_V  0x00000001U
#define LP_ADI_DCDC_TOP_2_VCM_READY_S  3

/** LP_ADI_DCDC_TOP_2_CFG_REG register
 *  ANALOG DCDC 2 related CONFIG
 */
#define LP_ADI_DCDC_TOP_2_CFG_REG (DR_REG_LP_ADI_BASE + 0x8)
/** LP_ADI_DCDC_TOP_2_DREG_MODE : R/W; bitpos: [1:0]; default: 3;
 *  DCDC 2 dreg mode
 */
#define LP_ADI_DCDC_TOP_2_DREG_MODE    0x00000003U
#define LP_ADI_DCDC_TOP_2_DREG_MODE_M  (LP_ADI_DCDC_TOP_2_DREG_MODE_V << LP_ADI_DCDC_TOP_2_DREG_MODE_S)
#define LP_ADI_DCDC_TOP_2_DREG_MODE_V  0x00000003U
#define LP_ADI_DCDC_TOP_2_DREG_MODE_S  0
/** LP_ADI_DCDC_TOP_2_CLEAR_VCM_RDY : R/W; bitpos: [2]; default: 0;
 *  1: clear vcm rdy signal
 */
#define LP_ADI_DCDC_TOP_2_CLEAR_VCM_RDY    (BIT(2))
#define LP_ADI_DCDC_TOP_2_CLEAR_VCM_RDY_M  (LP_ADI_DCDC_TOP_2_CLEAR_VCM_RDY_V << LP_ADI_DCDC_TOP_2_CLEAR_VCM_RDY_S)
#define LP_ADI_DCDC_TOP_2_CLEAR_VCM_RDY_V  0x00000001U
#define LP_ADI_DCDC_TOP_2_CLEAR_VCM_RDY_S  2
/** LP_ADI_DCDC_TOP_2_IFSM_PCUR_LIMIT : R/W; bitpos: [5:3]; default: 0;
 *  Current limit config
 */
#define LP_ADI_DCDC_TOP_2_IFSM_PCUR_LIMIT    0x00000007U
#define LP_ADI_DCDC_TOP_2_IFSM_PCUR_LIMIT_M  (LP_ADI_DCDC_TOP_2_IFSM_PCUR_LIMIT_V << LP_ADI_DCDC_TOP_2_IFSM_PCUR_LIMIT_S)
#define LP_ADI_DCDC_TOP_2_IFSM_PCUR_LIMIT_V  0x00000007U
#define LP_ADI_DCDC_TOP_2_IFSM_PCUR_LIMIT_S  3

/** LP_ADI_PM_TOP_STS_REG register
 *  ANALOG PM TOP related status
 */
#define LP_ADI_PM_TOP_STS_REG (DR_REG_LP_ADI_BASE + 0xc)
/** LP_ADI_PM_TOP_CAL_DONE : RO; bitpos: [0]; default: 0;
 *  cali done signal from PM TOP
 */
#define LP_ADI_PM_TOP_CAL_DONE    (BIT(0))
#define LP_ADI_PM_TOP_CAL_DONE_M  (LP_ADI_PM_TOP_CAL_DONE_V << LP_ADI_PM_TOP_CAL_DONE_S)
#define LP_ADI_PM_TOP_CAL_DONE_V  0x00000001U
#define LP_ADI_PM_TOP_CAL_DONE_S  0
/** LP_ADI_PM_TOP_DCDC_VCM_RDY : RO; bitpos: [1]; default: 0;
 *  vcm_ready signal from PM TOP
 */
#define LP_ADI_PM_TOP_DCDC_VCM_RDY    (BIT(1))
#define LP_ADI_PM_TOP_DCDC_VCM_RDY_M  (LP_ADI_PM_TOP_DCDC_VCM_RDY_V << LP_ADI_PM_TOP_DCDC_VCM_RDY_S)
#define LP_ADI_PM_TOP_DCDC_VCM_RDY_V  0x00000001U
#define LP_ADI_PM_TOP_DCDC_VCM_RDY_S  1

/** LP_ADI_PM_TOP_CFG_REG register
 *  ANALOG PM TOP related CONFIG
 */
#define LP_ADI_PM_TOP_CFG_REG (DR_REG_LP_ADI_BASE + 0x10)
/** LP_ADI_PM_TOP_DREG_MODE : R/W; bitpos: [1:0]; default: 3;
 *  PM TOP dreg mode
 */
#define LP_ADI_PM_TOP_DREG_MODE    0x00000003U
#define LP_ADI_PM_TOP_DREG_MODE_M  (LP_ADI_PM_TOP_DREG_MODE_V << LP_ADI_PM_TOP_DREG_MODE_S)
#define LP_ADI_PM_TOP_DREG_MODE_V  0x00000003U
#define LP_ADI_PM_TOP_DREG_MODE_S  0
/** LP_ADI_PM_TOP_DCDC_CLR_VCM_RDY : R/W; bitpos: [2]; default: 0;
 *  1: clear vcm rdy signal
 */
#define LP_ADI_PM_TOP_DCDC_CLR_VCM_RDY    (BIT(2))
#define LP_ADI_PM_TOP_DCDC_CLR_VCM_RDY_M  (LP_ADI_PM_TOP_DCDC_CLR_VCM_RDY_V << LP_ADI_PM_TOP_DCDC_CLR_VCM_RDY_S)
#define LP_ADI_PM_TOP_DCDC_CLR_VCM_RDY_V  0x00000001U
#define LP_ADI_PM_TOP_DCDC_CLR_VCM_RDY_S  2
/** LP_ADI_PM_TOP_DIG_REG_DSFMOS : R/W; bitpos: [5:3]; default: 0;
 *  Current limit config
 */
#define LP_ADI_PM_TOP_DIG_REG_DSFMOS    0x00000007U
#define LP_ADI_PM_TOP_DIG_REG_DSFMOS_M  (LP_ADI_PM_TOP_DIG_REG_DSFMOS_V << LP_ADI_PM_TOP_DIG_REG_DSFMOS_S)
#define LP_ADI_PM_TOP_DIG_REG_DSFMOS_V  0x00000007U
#define LP_ADI_PM_TOP_DIG_REG_DSFMOS_S  3

#ifdef __cplusplus
}
#endif
