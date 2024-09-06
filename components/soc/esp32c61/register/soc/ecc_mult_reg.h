/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
 *  ECC raw interrupt status register
 */
#define ECC_MULT_INT_RAW_REG (DR_REG_ECC_MULT_BASE + 0xc)
/** ECC_MULT_CALC_DONE_INT_RAW : RO/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status of the ECC_CALC_DONE_INT interrupt.
 */
#define ECC_MULT_CALC_DONE_INT_RAW    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_RAW_M  (ECC_MULT_CALC_DONE_INT_RAW_V << ECC_MULT_CALC_DONE_INT_RAW_S)
#define ECC_MULT_CALC_DONE_INT_RAW_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_RAW_S  0

/** ECC_MULT_INT_ST_REG register
 *  ECC masked interrupt status register
 */
#define ECC_MULT_INT_ST_REG (DR_REG_ECC_MULT_BASE + 0x10)
/** ECC_MULT_CALC_DONE_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status of the ECC_CALC_DONE_INT interrupt.
 */
#define ECC_MULT_CALC_DONE_INT_ST    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ST_M  (ECC_MULT_CALC_DONE_INT_ST_V << ECC_MULT_CALC_DONE_INT_ST_S)
#define ECC_MULT_CALC_DONE_INT_ST_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_ST_S  0

/** ECC_MULT_INT_ENA_REG register
 *  ECC interrupt enable register
 */
#define ECC_MULT_INT_ENA_REG (DR_REG_ECC_MULT_BASE + 0x14)
/** ECC_MULT_CALC_DONE_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Write 1 to enable the ECC_CALC_DONE_INT interrupt.
 */
#define ECC_MULT_CALC_DONE_INT_ENA    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_ENA_M  (ECC_MULT_CALC_DONE_INT_ENA_V << ECC_MULT_CALC_DONE_INT_ENA_S)
#define ECC_MULT_CALC_DONE_INT_ENA_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_ENA_S  0

/** ECC_MULT_INT_CLR_REG register
 *  ECC interrupt clear register
 */
#define ECC_MULT_INT_CLR_REG (DR_REG_ECC_MULT_BASE + 0x18)
/** ECC_MULT_CALC_DONE_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Write 1 to clear the ECC_CALC_DONE_INT interrupt.
 */
#define ECC_MULT_CALC_DONE_INT_CLR    (BIT(0))
#define ECC_MULT_CALC_DONE_INT_CLR_M  (ECC_MULT_CALC_DONE_INT_CLR_V << ECC_MULT_CALC_DONE_INT_CLR_S)
#define ECC_MULT_CALC_DONE_INT_CLR_V  0x00000001U
#define ECC_MULT_CALC_DONE_INT_CLR_S  0

/** ECC_MULT_CONF_REG register
 *  ECC configuration register
 */
#define ECC_MULT_CONF_REG (DR_REG_ECC_MULT_BASE + 0x1c)
/** ECC_MULT_START : R/W/SC; bitpos: [0]; default: 0;
 *  Configures whether to start calculation of ECC Accelerator. This bit will be
 *  self-cleared after the calculation is done. \\
 *  0: No effect\\
 *  1: Start calculation of ECC Accelerator\\
 */
#define ECC_MULT_START    (BIT(0))
#define ECC_MULT_START_M  (ECC_MULT_START_V << ECC_MULT_START_S)
#define ECC_MULT_START_V  0x00000001U
#define ECC_MULT_START_S  0
/** ECC_MULT_RESET : WT; bitpos: [1]; default: 0;
 *  Configures whether to reset ECC Accelerator. \\
 *  0: No effect\\
 *  1: Reset\\
 */
#define ECC_MULT_RESET    (BIT(1))
#define ECC_MULT_RESET_M  (ECC_MULT_RESET_V << ECC_MULT_RESET_S)
#define ECC_MULT_RESET_V  0x00000001U
#define ECC_MULT_RESET_S  1
/** ECC_MULT_KEY_LENGTH : R/W; bitpos: [2]; default: 0;
 *  Configures the key length mode bit of ECC Accelerator. \\
 *  0: P-192\\
 *  1: P-256\\
 */
#define ECC_MULT_KEY_LENGTH    (BIT(2))
#define ECC_MULT_KEY_LENGTH_M  (ECC_MULT_KEY_LENGTH_V << ECC_MULT_KEY_LENGTH_S)
#define ECC_MULT_KEY_LENGTH_V  0x00000001U
#define ECC_MULT_KEY_LENGTH_S  2
/** ECC_MULT_MOD_BASE : R/W; bitpos: [3]; default: 0;
 *  Configures the mod base of mod operation, only valid in work_mode 8-11. \\
 *  0: n(order of curve)\\
 *  1: p(mod base of curve)\\
 */
#define ECC_MULT_MOD_BASE    (BIT(3))
#define ECC_MULT_MOD_BASE_M  (ECC_MULT_MOD_BASE_V << ECC_MULT_MOD_BASE_S)
#define ECC_MULT_MOD_BASE_V  0x00000001U
#define ECC_MULT_MOD_BASE_S  3
/** ECC_MULT_WORK_MODE : R/W; bitpos: [7:4]; default: 0;
 *  Configures the work mode of ECC Accelerator.\\
 *  0: Point Multi mode\\
 *  1: Reserved\\
 *  2: Point Verif mode\\
 *  3: Point Verif + Multi mode\\
 *  4: Jacobian Point Multi mode\\
 *  5: Reserved\\
 *  6: Jacobian Point Verif mode\\
 *  7: Point Verif + Jacobian Point Multi mode\\
 *  8: Mod Add mode\\
 *  9. Mod Sub mode\\
 *  10: Mod Multi mode\\
 *  11: Mod Div mode\\
 */
#define ECC_MULT_WORK_MODE    0x0000000FU
#define ECC_MULT_WORK_MODE_M  (ECC_MULT_WORK_MODE_V << ECC_MULT_WORK_MODE_S)
#define ECC_MULT_WORK_MODE_V  0x0000000FU
#define ECC_MULT_WORK_MODE_S  4
/** ECC_MULT_SECURITY_MODE : R/W; bitpos: [8]; default: 0;
 *  Configures the security mode of ECC Accelerator.\\
 *  0: no secure function enabled.\\
 *  1: enable constant-time calculation in all point multiplication modes.\\
 */
#define ECC_MULT_SECURITY_MODE    (BIT(8))
#define ECC_MULT_SECURITY_MODE_M  (ECC_MULT_SECURITY_MODE_V << ECC_MULT_SECURITY_MODE_S)
#define ECC_MULT_SECURITY_MODE_V  0x00000001U
#define ECC_MULT_SECURITY_MODE_S  8
/** ECC_MULT_VERIFICATION_RESULT : RO/SS; bitpos: [29]; default: 0;
 *  Represents the verification result of ECC Accelerator, valid only when calculation
 *  is done.
 */
#define ECC_MULT_VERIFICATION_RESULT    (BIT(29))
#define ECC_MULT_VERIFICATION_RESULT_M  (ECC_MULT_VERIFICATION_RESULT_V << ECC_MULT_VERIFICATION_RESULT_S)
#define ECC_MULT_VERIFICATION_RESULT_V  0x00000001U
#define ECC_MULT_VERIFICATION_RESULT_S  29
/** ECC_MULT_CLK_EN : R/W; bitpos: [30]; default: 0;
 *  Configures whether to force on register clock gate. \\
 *  0: No effect\\
 *  1: Force on\\
 */
#define ECC_MULT_CLK_EN    (BIT(30))
#define ECC_MULT_CLK_EN_M  (ECC_MULT_CLK_EN_V << ECC_MULT_CLK_EN_S)
#define ECC_MULT_CLK_EN_V  0x00000001U
#define ECC_MULT_CLK_EN_S  30
/** ECC_MULT_MEM_CLOCK_GATE_FORCE_ON : R/W; bitpos: [31]; default: 0;
 *  Configures whether to force on ECC memory clock gate. \\
 *  0: No effect\\
 *  1: Force on\\
 */
#define ECC_MULT_MEM_CLOCK_GATE_FORCE_ON    (BIT(31))
#define ECC_MULT_MEM_CLOCK_GATE_FORCE_ON_M  (ECC_MULT_MEM_CLOCK_GATE_FORCE_ON_V << ECC_MULT_MEM_CLOCK_GATE_FORCE_ON_S)
#define ECC_MULT_MEM_CLOCK_GATE_FORCE_ON_V  0x00000001U
#define ECC_MULT_MEM_CLOCK_GATE_FORCE_ON_S  31

/** ECC_MULT_DATE_REG register
 *  Version control register
 */
#define ECC_MULT_DATE_REG (DR_REG_ECC_MULT_BASE + 0xfc)
/** ECC_MULT_DATE : R/W; bitpos: [27:0]; default: 37752928;
 *  ECC mult version control register
 */
#define ECC_MULT_DATE    0x0FFFFFFFU
#define ECC_MULT_DATE_M  (ECC_MULT_DATE_V << ECC_MULT_DATE_S)
#define ECC_MULT_DATE_V  0x0FFFFFFFU
#define ECC_MULT_DATE_S  0

/** ECC_MULT_K_MEM register
 *  The memory that stores k.
 */
#define ECC_MULT_K_MEM (DR_REG_ECC_MULT_BASE + 0x100)
#define ECC_MULT_K_MEM_SIZE_BYTES 32

/** ECC_MULT_PX_MEM register
 *  The memory that stores Px.
 */
#define ECC_MULT_PX_MEM (DR_REG_ECC_MULT_BASE + 0x120)
#define ECC_MULT_PX_MEM_SIZE_BYTES 32

/** ECC_MULT_PY_MEM register
 *  The memory that stores Py.
 */
#define ECC_MULT_PY_MEM (DR_REG_ECC_MULT_BASE + 0x140)
#define ECC_MULT_PY_MEM_SIZE_BYTES 32

/** ECC_MULT_QX_MEM register
 *  The memory that stores Qx.
 */
#define ECC_MULT_QX_MEM (DR_REG_ECC_MULT_BASE + 0x160)
#define ECC_MULT_QX_MEM_SIZE_BYTES 32

/** ECC_MULT_QY_MEM register
 *  The memory that stores Qy.
 */
#define ECC_MULT_QY_MEM (DR_REG_ECC_MULT_BASE + 0x180)
#define ECC_MULT_QY_MEM_SIZE_BYTES 32

/** ECC_MULT_QZ_MEM register
 *  The memory that stores Qz.
 */
#define ECC_MULT_QZ_MEM (DR_REG_ECC_MULT_BASE + 0x1a0)
#define ECC_MULT_QZ_MEM_SIZE_BYTES 32

#ifdef __cplusplus
}
#endif
